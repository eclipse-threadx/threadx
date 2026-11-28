/***************************************************************************
 * Copyright (c) 2026-present Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include "txfr_test_harness.h"

#define TXFR_QUEUE_LEN            3U
#define TXFR_DRIVER_PRIO          ((UBaseType_t)2U)
#define TXFR_WORKER_PRIO          ((UBaseType_t)3U)
#define TXFR_WAIT_TICKS           ((TickType_t)50U)

static StaticQueue_t static_queue_cb;
static uint8_t static_queue_storage[TXFR_QUEUE_LEN * sizeof(uint32_t)];
static SemaphoreHandle_t test_done_sem = NULL;
static QueueHandle_t shared_queue = NULL;

static void queue_producer_task(void *arg)
{
    uint32_t item;
    SemaphoreHandle_t ack_sem = (SemaphoreHandle_t)arg;

    txfr_test_check("consumer starts blocked on empty queue",
                    uxQueueMessagesWaiting(shared_queue) == 0U);
    item = 0x11111111U;
    txfr_test_check("producer sends item waking blocked consumer",
                    xQueueSendToBack(shared_queue, &item, TXFR_WAIT_TICKS) == pdPASS);

    xSemaphoreGive(ack_sem);
    vTaskDelete(NULL);
}

static void queue_consumer_task(void *arg)
{
    uint32_t received = 0U;
    SemaphoreHandle_t ack_sem = (SemaphoreHandle_t)arg;

    txfr_test_check("consumer receives item sent by producer",
                    xQueueReceive(shared_queue, &received, TXFR_WAIT_TICKS) == pdPASS);
    txfr_test_check("consumer receives expected payload",
                    received == 0x11111111U);

    xSemaphoreGive(ack_sem);
    vTaskDelete(NULL);
}

static void test_fifo_lifo_peek_reset_and_wrap(QueueHandle_t q)
{
    uint32_t vals[4] = {10U, 20U, 30U, 40U};
    uint32_t out = 0U;
    BaseType_t woken = pdFALSE;

    txfr_test_check("empty queue reports 0 messages",
                    uxQueueMessagesWaiting(q) == 0U);
    txfr_test_check("empty queue reports full spaces",
                    uxQueueSpacesAvailable(q) == TXFR_QUEUE_LEN);
    txfr_test_check("empty queue IsEmptyFromISR returns pdTRUE",
                    xQueueIsQueueEmptyFromISR(q) == pdTRUE);
    txfr_test_check("empty queue IsFullFromISR returns pdFALSE",
                    xQueueIsQueueFullFromISR(q) == pdFALSE);
    txfr_test_check("nonblocking receive on empty queue fails",
                    xQueueReceive(q, &out, 0U) == errQUEUE_EMPTY);
    txfr_test_check("nonblocking peek on empty queue fails",
                    xQueuePeek(q, &out, 0U) == errQUEUE_EMPTY);

    txfr_test_check("send back item 0", xQueueSend(q, &vals[0], 0U) == pdPASS);
    txfr_test_check("send back item 1", xQueueSendToBack(q, &vals[1], 0U) == pdPASS);
    txfr_test_check("send front item 2", xQueueSendToFront(q, &vals[2], 0U) == pdPASS);

    txfr_test_check("full queue count is TXFR_QUEUE_LEN",
                    uxQueueMessagesWaiting(q) == TXFR_QUEUE_LEN);
    txfr_test_check("full queue spaces available is 0",
                    uxQueueSpacesAvailable(q) == 0U);
    txfr_test_check("full queue IsFullFromISR returns pdTRUE",
                    xQueueIsQueueFullFromISR(q) == pdTRUE);
    txfr_test_check("send to full queue with zero wait returns errQUEUE_FULL",
                    xQueueSend(q, &vals[3], 0U) == errQUEUE_FULL);
    txfr_test_check("send to front on full queue with zero wait returns errQUEUE_FULL",
                    xQueueSendToFront(q, &vals[3], 0U) == errQUEUE_FULL);

    txfr_test_check("peek sees front item without removing",
                    xQueuePeek(q, &out, 0U) == pdPASS && out == 30U);
    txfr_test_check("peek FromISR also sees front item",
                    xQueuePeekFromISR(q, &out) == pdPASS && out == 30U);
    txfr_test_check("count unchanged after peek",
                    uxQueueMessagesWaitingFromISR(q) == TXFR_QUEUE_LEN);

    txfr_test_check("receive 1 gets front item 30",
                    xQueueReceive(q, &out, 0U) == pdPASS && out == 30U);
    txfr_test_check("send back item 3 while partially drained wraps tail",
                    xQueueSendToBackFromISR(q, &vals[3], &woken) == pdPASS);
    txfr_test_check("receive 2 gets 10",
                    xQueueReceiveFromISR(q, &out, &woken) == pdPASS && out == 10U);
    txfr_test_check("receive 3 gets 20",
                    xQueueReceive(q, &out, 0U) == pdPASS && out == 20U);
    txfr_test_check("receive 4 gets wrapped item 40",
                    xQueueReceive(q, &out, 0U) == pdPASS && out == 40U);
    txfr_test_check("queue is empty again after wrap drain",
                    uxQueueMessagesWaiting(q) == 0U);

    woken = pdFALSE;
    txfr_test_check("send FromISR succeeds",
                    xQueueSendFromISR(q, &vals[0], &woken) == pdPASS);
    txfr_test_check("send front FromISR succeeds",
                    xQueueSendToFrontFromISR(q, &vals[1], &woken) == pdPASS);
    txfr_test_check("xQueueReset clears queued items",
                    xQueueReset(q) == pdPASS && uxQueueMessagesWaiting(q) == 0U &&
                    uxQueueSpacesAvailable(q) == TXFR_QUEUE_LEN);
}

static void test_queue_overwrite_and_sets(void)
{
    QueueHandle_t q_len1;
    QueueHandle_t member_q1;
    QueueHandle_t member_q2;
    QueueSetHandle_t qset;
    uint32_t v = 100U;
    uint32_t out = 0U;
    BaseType_t woken = pdFALSE;

    q_len1 = xQueueCreate(1U, sizeof(uint32_t));
    txfr_test_check("create length-1 queue for overwrite", q_len1 != NULL);
    txfr_test_check("overwrite on empty queue succeeds",
                    xQueueOverwrite(q_len1, &v) == pdPASS);
    v = 200U;
    txfr_test_check("overwrite on full length-1 queue replaces item",
                    xQueueOverwrite(q_len1, &v) == pdPASS);
    txfr_test_check("count remains 1 after overwrite",
                    uxQueueMessagesWaiting(q_len1) == 1U);
    v = 300U;
    txfr_test_check("overwrite FromISR replaces item",
                    xQueueOverwriteFromISR(q_len1, &v, &woken) == pdPASS);
    txfr_test_check("receive after overwrites yields latest value",
                    xQueueReceive(q_len1, &out, 0U) == pdPASS && out == 300U);
    vQueueDelete(q_len1);

    member_q1 = xQueueCreate(2U, sizeof(uint32_t));
    member_q2 = xQueueCreate(2U, sizeof(uint32_t));
    qset = xQueueCreateSet(4U);
    txfr_test_check("create queue set and members",
                    (member_q1 != NULL) && (member_q2 != NULL) && (qset != NULL));
    txfr_test_check("add queues to queue set",
                    (xQueueAddToSet(member_q1, qset) == pdPASS) &&
                    (xQueueAddToSet(member_q2, qset) == pdPASS));
    txfr_test_check("select from empty queue set times out",
                    xQueueSelectFromSet(qset, 2U) == NULL);

    v = 555U;
    txfr_test_check("send to member_q2 wakes set selection",
                    xQueueSend(member_q2, &v, 0U) == pdPASS);
    txfr_test_check("select from set returns member_q2",
                    xQueueSelectFromSet(qset, 0U) == (QueueSetMemberHandle_t)member_q2);
    txfr_test_check("receive from selected member_q2",
                    xQueueReceive(member_q2, &out, 0U) == pdPASS && out == 555U);

    v = 666U;
    txfr_test_check("send to member_q1 for FromISR set select",
                    xQueueSend(member_q1, &v, 0U) == pdPASS);
    txfr_test_check("SelectFromSetFromISR returns member_q1",
                    xQueueSelectFromSetFromISR(qset) == (QueueSetMemberHandle_t)member_q1);
    txfr_test_check("receive from selected member_q1",
                    xQueueReceive(member_q1, &out, 0U) == pdPASS && out == 666U);

    txfr_test_check("remove member queues from set",
                    (xQueueRemoveFromSet(member_q1, qset) == pdPASS) &&
                    (xQueueRemoveFromSet(member_q2, qset) == pdPASS));
    vQueueDelete(member_q1);
    vQueueDelete(member_q2);
}

static void queue_driver_task(void *arg)
{
    QueueHandle_t dyn_q;
    QueueHandle_t stat_q;
    SemaphoreHandle_t ack_sem;
    uint32_t out = 0U;

    (void)arg;

    dyn_q = xQueueCreate(TXFR_QUEUE_LEN, sizeof(uint32_t));
    stat_q = xQueueCreateStatic(TXFR_QUEUE_LEN, sizeof(uint32_t),
                                static_queue_storage, &static_queue_cb);
    txfr_test_check("dynamic and static queues created",
                    (dyn_q != NULL) && (stat_q != NULL));

    test_fifo_lifo_peek_reset_and_wrap(dyn_q);
    test_fifo_lifo_peek_reset_and_wrap(stat_q);
    vQueueDelete(dyn_q);
    vQueueDelete(stat_q);

    shared_queue = xQueueCreate(1U, sizeof(uint32_t));
    ack_sem = xSemaphoreCreateCounting(2U, 0U);
    txfr_test_check("create cross-task queue and ack semaphore",
                    (shared_queue != NULL) && (ack_sem != NULL));

    txfr_test_check("timed receive on empty queue returns errQUEUE_EMPTY",
                    xQueueReceive(shared_queue, &out, 2U) == errQUEUE_EMPTY);

    TaskHandle_t cons_handle = NULL;
    TaskHandle_t prod_handle = NULL;

    txfr_test_check("spawn consumer task",
                    xTaskCreate(queue_consumer_task, "q_cons",
                                configMINIMAL_STACK_SIZE * 2U, ack_sem,
                                TXFR_WORKER_PRIO, &cons_handle) == pdPASS);
    txfr_test_check("spawn producer task",
                    xTaskCreate(queue_producer_task, "q_prod",
                                configMINIMAL_STACK_SIZE * 2U, ack_sem,
                                TXFR_DRIVER_PRIO, &prod_handle) == pdPASS);

    txfr_test_check("wait for producer completion",
                    xSemaphoreTake(ack_sem, TXFR_WAIT_TICKS) == pdPASS);
    txfr_test_check("wait for consumer completion",
                    xSemaphoreTake(ack_sem, TXFR_WAIT_TICKS) == pdPASS);

    vQueueDelete(shared_queue);
    vSemaphoreDelete(ack_sem);

    test_queue_overwrite_and_sets();

    xSemaphoreGive(test_done_sem);
    vTaskDelete(NULL);
}

void txfr_test_body(void)
{
    TaskHandle_t drv_handle = NULL;

    test_done_sem = xSemaphoreCreateBinary();
    txfr_test_check("create queue test completion semaphore", test_done_sem != NULL);

    txfr_test_check("create queue driver task",
                    xTaskCreate(queue_driver_task, "q_drv",
                                configMINIMAL_STACK_SIZE * 2U, NULL,
                                TXFR_DRIVER_PRIO, &drv_handle) == pdPASS);

    txfr_test_check("queue runtime driver finishes",
                    xSemaphoreTake(test_done_sem, TXFR_WAIT_TICKS * 4U) == pdPASS);
    vSemaphoreDelete(test_done_sem);
}
