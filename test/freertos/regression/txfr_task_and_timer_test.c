/***************************************************************************
 * Copyright (c) 2026-present Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include <string.h>
#include "txfr_test_harness.h"

#define TXFR_TASK_DRIVER_PRIO     ((UBaseType_t)2U)
#define TXFR_TASK_HIGH_PRIO       ((UBaseType_t)4U)
#define TXFR_TASK_LOW_PRIO        ((UBaseType_t)1U)
#define TXFR_GENERAL_WAIT_TICKS   ((TickType_t)80U)

static StaticTask_t static_task_cb;
static StackType_t static_task_stack[configMINIMAL_STACK_SIZE * 2U];
static StaticEventGroup_t static_ev_cb;
static StaticTimer_t static_timer_cb;

static SemaphoreHandle_t test_done_sem = NULL;
static volatile uint32_t suspended_worker_ran = 0U;
static volatile uint32_t aborted_worker_done = 0U;
static volatile uint32_t timer_fire_count = 0U;
static void * volatile last_timer_id = NULL;

static void suspendable_worker_task(void *arg)
{
    (void)arg;

    suspended_worker_ran = 1U;
    vTaskSuspend(NULL);
    suspended_worker_ran = 2U;
    vTaskSuspend(NULL);
    suspended_worker_ran = 3U;

    vTaskDelete(NULL);
}

static void delayed_abort_worker_task(void *arg)
{
    SemaphoreHandle_t block_sem = (SemaphoreHandle_t)arg;

    if (block_sem != NULL)
    {
        (void)xSemaphoreTake(block_sem, TXFR_GENERAL_WAIT_TICKS * 10U);
    }
    else
    {
        vTaskDelay(TXFR_GENERAL_WAIT_TICKS * 10U);
    }
    aborted_worker_done = 1U;
    vTaskDelete(NULL);
}

static void notify_target_task(void *arg)
{
    uint32_t val = 0U;
    SemaphoreHandle_t ack_sem = (SemaphoreHandle_t)arg;

    txfr_test_check("ulTaskNotifyTake(pdFALSE) returns 1 and decrements",
                    ulTaskNotifyTake(pdFALSE, TXFR_GENERAL_WAIT_TICKS) == 1U);
    xSemaphoreGive(ack_sem);

    txfr_test_check("ulTaskNotifyTake(pdTRUE) returns 1 and clears",
                    ulTaskNotifyTake(pdTRUE, TXFR_GENERAL_WAIT_TICKS) == 1U);
    txfr_test_check("ulTaskNotifyTake with 0 wait after clear returns 0",
                    ulTaskNotifyTake(pdTRUE, 0U) == 0U);
    xSemaphoreGive(ack_sem);

    txfr_test_check("xTaskNotifyWait receives eSetBits notification",
                    xTaskNotifyWait(0U, 0xFFFFFFFFU, &val, TXFR_GENERAL_WAIT_TICKS) == pdPASS);
    txfr_test_check("xTaskNotifyWait sees expected bitmask", val == 0x0AU);
    xSemaphoreGive(ack_sem);

    vTaskDelete(NULL);
}

static void ev_setter_task(void *arg)
{
    EventGroupHandle_t ev = (EventGroupHandle_t)arg;

    vTaskDelay(2U);
    xEventGroupSetBits(ev, 0x05U);
    vTaskDelete(NULL);
}

static void sample_timer_callback(TimerHandle_t xTimer)
{
    timer_fire_count++;
    last_timer_id = pvTimerGetTimerID(xTimer);
}

static void test_task_lifecycle_and_scheduler(void)
{
    TaskHandle_t self_task = xTaskGetCurrentTaskHandle();
    TaskHandle_t worker = NULL;
    TaskHandle_t abort_worker = NULL;
    TaskHandle_t stat_worker = NULL;
    TickType_t prev_wake;
    TickType_t t0;
    TickType_t t1;
    UBaseType_t base_count;

    txfr_test_check("current task handle and name are valid",
                    (self_task != NULL) && (strcmp(pcTaskGetName(self_task), "task_drv") == 0));
    txfr_test_check("driver running state and priority match",
                    (eTaskGetState(self_task) == eRunning) &&
                    (uxTaskPriorityGet(self_task) == TXFR_TASK_DRIVER_PRIO) &&
                    (uxTaskPriorityGetFromISR(self_task) == TXFR_TASK_DRIVER_PRIO));
    txfr_test_check("scheduler state starts running",
                    xTaskGetSchedulerState() == taskSCHEDULER_RUNNING);

    vTaskSuspendAll();
    txfr_test_check("scheduler state reports suspended during SuspendAll",
                    xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED);
    (void)xTaskResumeAll();
    txfr_test_check("scheduler state restored after ResumeAll",
                    xTaskGetSchedulerState() == taskSCHEDULER_RUNNING);

    base_count = uxTaskGetNumberOfTasks();
    suspended_worker_ran = 0U;
    txfr_test_check("create suspendable worker",
                    xTaskCreate(suspendable_worker_task, "susp_wrk",
                                configMINIMAL_STACK_SIZE * 2U, NULL,
                                TXFR_TASK_HIGH_PRIO, &worker) == pdPASS);
    txfr_test_check("task count increased by 1",
                    uxTaskGetNumberOfTasks() == (base_count + 1U));
    txfr_test_check("higher prio worker ran and suspended itself",
                    (suspended_worker_ran == 1U) && (eTaskGetState(worker) == eSuspended));

    vTaskResume(worker);
    txfr_test_check("vTaskResume runs worker to stage 2",
                    (suspended_worker_ran == 2U) && (eTaskGetState(worker) == eSuspended));

    (void)xTaskResumeFromISR(worker);
    {
        uint32_t wait_ticks;
        for (wait_ticks = 0U; (wait_ticks < 20U) && (suspended_worker_ran != 3U); wait_ticks++)
        {
            vTaskDelay(1U);
        }
    }
    txfr_test_check("xTaskResumeFromISR ran worker to completion",
                    suspended_worker_ran == 3U);

    stat_worker = xTaskCreateStatic(delayed_abort_worker_task, "stat_wrk",
                                    configMINIMAL_STACK_SIZE * 2U, NULL,
                                    TXFR_TASK_LOW_PRIO, static_task_stack,
                                    &static_task_cb);
    txfr_test_check("create static task succeeds", stat_worker != NULL);
    txfr_test_check("change static worker priority cleanly",
                    (vTaskPrioritySet(stat_worker, TXFR_TASK_HIGH_PRIO),
                     uxTaskPriorityGet(stat_worker) == TXFR_TASK_HIGH_PRIO));
    vTaskDelete(stat_worker);
    {
        uint32_t wait_ticks;
        for (wait_ticks = 0U; (wait_ticks < 20U) && (uxTaskGetNumberOfTasks() > base_count); wait_ticks++)
        {
            vTaskDelay(1U);
        }
    }
    txfr_test_check("vTaskDelete cleans up static task",
                    uxTaskGetNumberOfTasks() <= base_count);

    aborted_worker_done = 0U;
    txfr_test_check("create long-sleeping worker for AbortDelay",
                    xTaskCreate(delayed_abort_worker_task, "abort_wrk",
                                configMINIMAL_STACK_SIZE * 2U, NULL,
                                TXFR_TASK_HIGH_PRIO, &abort_worker) == pdPASS);
    txfr_test_check("sleeping abort_wrk reports eSuspended state",
                    eTaskGetState(abort_worker) == eSuspended);
    txfr_test_check("xTaskAbortDelay unblocks sleeping task",
                    xTaskAbortDelay(abort_worker) == pdPASS);
    {
        uint32_t wait_ticks;
        for (wait_ticks = 0U; (wait_ticks < 20U) && (aborted_worker_done != 1U); wait_ticks++)
        {
            vTaskDelay(1U);
        }
    }
    txfr_test_check("aborted worker finished after abort",
                    aborted_worker_done == 1U);

    {
        SemaphoreHandle_t blk_sem = xSemaphoreCreateBinary();
        TaskHandle_t blk_worker = NULL;

        txfr_test_check("create blocking sem and worker for eBlocked check",
                        (blk_sem != NULL) &&
                        (xTaskCreate(delayed_abort_worker_task, "blk_wrk",
                                     configMINIMAL_STACK_SIZE * 2U, blk_sem,
                                     TXFR_TASK_HIGH_PRIO, &blk_worker) == pdPASS));
        txfr_test_check("semaphore-blocked worker reports eBlocked state",
                        eTaskGetState(blk_worker) == eBlocked);
        xSemaphoreGive(blk_sem);
        vTaskDelay(1U);
        vSemaphoreDelete(blk_sem);
    }

    t0 = xTaskGetTickCount();
    prev_wake = t0;
    vTaskDelayUntil(&prev_wake, 3U);
    t1 = xTaskGetTickCountFromISR();
    txfr_test_check("vTaskDelayUntil advances tick count",
                    (t1 - t0) >= 3U);
}

static void test_task_notifications(void)
{
    TaskHandle_t self_task = xTaskGetCurrentTaskHandle();
    TaskHandle_t worker = NULL;
    SemaphoreHandle_t ack_sem = xSemaphoreCreateCounting(4U, 0U);
    uint32_t prev = 0U;
    uint32_t val = 0U;
    BaseType_t woken = pdFALSE;

    txfr_test_check("create notification test ack sem", ack_sem != NULL);

    txfr_test_check("xTaskNotify eSetValueWithOverwrite succeeds",
                    xTaskNotify(self_task, 0x04U, eSetValueWithOverwrite) == pdPASS);
    txfr_test_check("xTaskNotify eSetValueWithoutOverwrite fails while pending",
                    xTaskNotify(self_task, 0x08U, eSetValueWithoutOverwrite) == pdFAIL);
    txfr_test_check("xTaskNotifyAndQuery eSetBits returns prev and updates",
                    (xTaskNotifyAndQuery(self_task, 0x02U, eSetBits, &prev) == pdPASS) &&
                    (prev == 0x04U));
    txfr_test_check("xTaskNotifyFromISR eIncrement updates pending value",
                    xTaskNotifyFromISR(self_task, 0U, eIncrement, &woken) == pdPASS);
    txfr_test_check("xTaskNotifyAndQueryFromISR eNoAction preserves value",
                    (xTaskNotifyAndQueryFromISR(self_task, 99U, eNoAction, &prev, &woken) == pdPASS) &&
                    (prev == 0x07U));
    txfr_test_check("ulTaskNotifyValueClear clears requested bits",
                    ulTaskNotifyValueClear(self_task, 0x03U) == 0x07U);
    txfr_test_check("xTaskNotifyWait reads remaining bits and clears on exit",
                    (xTaskNotifyWait(0U, 0xFFFFFFFFU, &val, 0U) == pdPASS) && (val == 0x04U));
    txfr_test_check("xTaskNotifyStateClear on idle notification returns pdFALSE",
                    xTaskNotifyStateClear(self_task) == pdFALSE);

    txfr_test_check("create notification target worker",
                    xTaskCreate(notify_target_task, "ntf_wrk",
                                configMINIMAL_STACK_SIZE * 2U, ack_sem,
                                TXFR_TASK_HIGH_PRIO, &worker) == pdPASS);
    txfr_test_check("xTaskNotifyGive unblocks ulTaskNotifyTake(pdFALSE)",
                    xTaskNotifyGive(worker) == pdPASS);
    txfr_test_check("target completed ulTaskNotifyTake(pdFALSE) stage",
                    xSemaphoreTake(ack_sem, TXFR_GENERAL_WAIT_TICKS) == pdPASS);

    vTaskNotifyGiveFromISR(worker, &woken);
    txfr_test_check("target completed ulTaskNotifyTake(pdTRUE) stage",
                    xSemaphoreTake(ack_sem, TXFR_GENERAL_WAIT_TICKS) == pdPASS);

    txfr_test_check("notify blocked target via eSetBits",
                    xTaskNotify(worker, 0x0AU, eSetBits) == pdPASS);
    txfr_test_check("target completed xTaskNotifyWait stage",
                    xSemaphoreTake(ack_sem, TXFR_GENERAL_WAIT_TICKS) == pdPASS);

    vSemaphoreDelete(ack_sem);
}

static void test_event_groups_and_timers(void)
{
    EventGroupHandle_t ev_dyn = xEventGroupCreate();
    EventGroupHandle_t ev_stat = xEventGroupCreateStatic(&static_ev_cb);
    EventBits_t bits;
    TaskHandle_t ev_worker = NULL;
    BaseType_t woken = pdFALSE;
    TimerHandle_t t_one_shot;
    TimerHandle_t t_periodic;
    TimerHandle_t t_static;
    int id_a = 1;
    int id_b = 2;

    txfr_test_check("event groups created", (ev_dyn != NULL) && (ev_stat != NULL));

    (void)xEventGroupSetBits(ev_dyn, 0x03U);
    txfr_test_check("xEventGroupGetBits sees set bits",
                    xEventGroupGetBits(ev_dyn) == 0x03U);
    txfr_test_check("xEventGroupClearBits clears bit 0",
                    (xEventGroupClearBits(ev_dyn, 0x01U),
                     xEventGroupGetBitsFromISR(ev_dyn) == 0x02U));
    txfr_test_check("xEventGroupSetBitsFromISR and ClearBitsFromISR succeed",
                    ((xEventGroupSetBitsFromISR(ev_stat, 0x10U, &woken) & 0x10U) == 0x10U) &&
                    (xEventGroupClearBitsFromISR(ev_stat, 0x10U) == (BaseType_t)0x10U) &&
                    (xEventGroupGetBitsFromISR(ev_stat) == 0U));

    txfr_test_check("WaitBits timeout without matching bits returns 0",
                    xEventGroupWaitBits(ev_dyn, 0x80U, pdFALSE, pdTRUE, 2U) == 0U);
    txfr_test_check("spawn event setter task for AND wait",
                    xTaskCreate(ev_setter_task, "ev_set",
                                configMINIMAL_STACK_SIZE * 2U, ev_dyn,
                                TXFR_TASK_LOW_PRIO, &ev_worker) == pdPASS);
    bits = xEventGroupWaitBits(ev_dyn, 0x05U, pdTRUE, pdTRUE, TXFR_GENERAL_WAIT_TICKS);
    txfr_test_check("WaitBits unblocks on AND mask and clears waited bits",
                    ((bits & 0x05U) == 0x05U) && ((xEventGroupGetBits(ev_dyn) & 0x05U) == 0U));

    vEventGroupDelete(ev_dyn);
    vEventGroupDelete(ev_stat);

    timer_fire_count = 0U;
    last_timer_id = NULL;
    t_one_shot = xTimerCreate("one_shot", 2U, pdFALSE, &id_a, sample_timer_callback);
    t_periodic = xTimerCreate("periodic", 2U, pdTRUE, &id_b, sample_timer_callback);
    t_static = xTimerCreateStatic("stat_tmr", 4U, pdFALSE, &id_a,
                                  sample_timer_callback,
                                  &static_timer_cb);
    txfr_test_check("timers created",
                    (t_one_shot != NULL) && (t_periodic != NULL) && (t_static != NULL));
    txfr_test_check("timer metadata name and period match",
                    (strcmp(pcTimerGetName(t_one_shot), "one_shot") == 0) &&
                    (xTimerGetPeriod(t_one_shot) == 2U));
    txfr_test_check("unstarted timer is inactive",
                    xTimerIsTimerActive(t_one_shot) == pdFALSE);

    txfr_test_check("start one-shot timer",
                    xTimerStart(t_one_shot, TXFR_GENERAL_WAIT_TICKS) == pdPASS);
    txfr_test_check("started timer is active with valid expiry time",
                    (xTimerIsTimerActive(t_one_shot) == pdTRUE) &&
                    (xTimerGetExpiryTime(t_one_shot) >= xTaskGetTickCount()));
    vTaskDelay(5U);
    txfr_test_check("one-shot timer fired once with expected ID",
                    (timer_fire_count == 1U) && (last_timer_id == &id_a));

    vTimerSetTimerID(t_one_shot, &id_b);
    txfr_test_check("xTimerResetFromISR restarts timer with updated ID",
                    xTimerResetFromISR(t_one_shot, &woken) == pdPASS);
    vTaskDelay(5U);
    txfr_test_check("restarted one-shot timer fired with new ID",
                    (timer_fire_count == 2U) && (last_timer_id == &id_b));

    timer_fire_count = 0U;
    vTimerSetReloadMode(t_one_shot, pdFALSE);
    txfr_test_check("start periodic timer and wait multiple expirations",
                    xTimerStartFromISR(t_periodic, &woken) == pdPASS);
    vTaskDelay(7U);
    txfr_test_check("periodic timer fired multiple times", timer_fire_count >= 2U);
    txfr_test_check("xTimerChangePeriod and Stop succeed",
                    (xTimerChangePeriod(t_periodic, 4U, TXFR_GENERAL_WAIT_TICKS) == pdPASS) &&
                    (xTimerChangePeriodFromISR(t_periodic, 5U, &woken) == pdPASS) &&
                    (xTimerReset(t_periodic, TXFR_GENERAL_WAIT_TICKS) == pdPASS) &&
                    (xTimerStop(t_periodic, TXFR_GENERAL_WAIT_TICKS) == pdPASS) &&
                    (xTimerStopFromISR(t_static, &woken) == pdPASS) &&
                    (xTimerIsTimerActive(t_periodic) == pdFALSE));

    txfr_test_check("timers deleted cleanly",
                    (xTimerDelete(t_one_shot, TXFR_GENERAL_WAIT_TICKS) == pdPASS) &&
                    (xTimerDelete(t_periodic, TXFR_GENERAL_WAIT_TICKS) == pdPASS) &&
                    (xTimerDelete(t_static, TXFR_GENERAL_WAIT_TICKS) == pdPASS));
}

static void task_and_timer_driver(void *arg)
{
    (void)arg;

    test_task_lifecycle_and_scheduler();
    test_task_notifications();
    test_event_groups_and_timers();

    xSemaphoreGive(test_done_sem);
    vTaskDelete(NULL);
}

void txfr_test_body(void)
{
    TaskHandle_t drv_handle = NULL;

    test_done_sem = xSemaphoreCreateBinary();
    txfr_test_check("create task/timer completion semaphore", test_done_sem != NULL);

    txfr_test_check("create task/timer driver",
                    xTaskCreate(task_and_timer_driver, "task_drv",
                                configMINIMAL_STACK_SIZE * 2U, NULL,
                                TXFR_TASK_DRIVER_PRIO, &drv_handle) == pdPASS);

    txfr_test_check("task and timer runtime driver finishes",
                    xSemaphoreTake(test_done_sem, TXFR_GENERAL_WAIT_TICKS * 6U) == pdPASS);
    vSemaphoreDelete(test_done_sem);
}
