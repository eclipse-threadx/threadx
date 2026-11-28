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

#define TXFR_SYNC_DRIVER_PRIO     ((UBaseType_t)2U)
#define TXFR_SYNC_WORKER_PRIO     ((UBaseType_t)3U)
#define TXFR_SYNC_WAIT_TICKS      ((TickType_t)50U)

static StaticSemaphore_t static_bin_sem;
static StaticSemaphore_t static_cnt_sem;
static StaticSemaphore_t static_mutex;
static StaticSemaphore_t static_rec_mutex;

static SemaphoreHandle_t test_done_sem = NULL;

typedef struct sync_worker_ctx {
    SemaphoreHandle_t sync_obj;
    SemaphoreHandle_t done_sem;
    BaseType_t take_before_give;
} sync_worker_ctx_t;

static void bin_sem_giver_task(void *arg)
{
    sync_worker_ctx_t *ctx = (sync_worker_ctx_t *)arg;

    vTaskDelay(2U);
    txfr_test_check("worker gives binary semaphore",
                    xSemaphoreGive(ctx->sync_obj) == pdPASS);
    xSemaphoreGive(ctx->done_sem);
    vTaskDelete(NULL);
}

static void mutex_contender_task(void *arg)
{
    sync_worker_ctx_t *ctx = (sync_worker_ctx_t *)arg;

    txfr_test_check("non-owner mutex give fails",
                    xSemaphoreGive(ctx->sync_obj) == pdFALSE);
    txfr_test_check("non-owner nonblocking mutex take fails while held",
                    xSemaphoreTake(ctx->sync_obj, 0U) == pdFALSE);
    txfr_test_check("contender blocks until owner releases mutex",
                    xSemaphoreTake(ctx->sync_obj, TXFR_SYNC_WAIT_TICKS) == pdPASS);
    txfr_test_check("contender becomes current mutex holder",
                    xSemaphoreGetMutexHolder(ctx->sync_obj) == xTaskGetCurrentTaskHandle());
    txfr_test_check("contender releases mutex",
                    xSemaphoreGive(ctx->sync_obj) == pdPASS);

    xSemaphoreGive(ctx->done_sem);
    vTaskDelete(NULL);
}

static void rec_mutex_contender_task(void *arg)
{
    sync_worker_ctx_t *ctx = (sync_worker_ctx_t *)arg;

    txfr_test_check("non-owner nonblocking recursive take fails while nested",
                    xSemaphoreTakeRecursive(ctx->sync_obj, 0U) == pdFALSE);
    txfr_test_check("contender acquires recursive mutex once fully released",
                    xSemaphoreTakeRecursive(ctx->sync_obj, TXFR_SYNC_WAIT_TICKS) == pdPASS);
    txfr_test_check("contender releases recursive mutex",
                    xSemaphoreGiveRecursive(ctx->sync_obj) == pdPASS);

    xSemaphoreGive(ctx->done_sem);
    vTaskDelete(NULL);
}

static void test_binary_semaphores(void)
{
    SemaphoreHandle_t bin_dyn;
    SemaphoreHandle_t bin_stat;
    SemaphoreHandle_t ack_sem;
    sync_worker_ctx_t ctx;
    BaseType_t woken = pdFALSE;

    bin_dyn = xSemaphoreCreateBinary();
    bin_stat = xSemaphoreCreateBinaryStatic(&static_bin_sem);
    ack_sem = xSemaphoreCreateBinary();
    txfr_test_check("binary semaphores created",
                    (bin_dyn != NULL) && (bin_stat != NULL) && (ack_sem != NULL));

    txfr_test_check("binary semaphore starts empty",
                    (uxSemaphoreGetCount(bin_dyn) == 0U) &&
                    (xSemaphoreTake(bin_dyn, 0U) == pdFALSE));
    txfr_test_check("binary semaphore give sets count to 1",
                    (xSemaphoreGive(bin_dyn) == pdPASS) &&
                    (uxSemaphoreGetCount(bin_dyn) == 1U));
    txfr_test_check("second binary semaphore give hits ceiling 1",
                    xSemaphoreGive(bin_dyn) == pdFALSE);
    txfr_test_check("binary semaphore take empties it",
                    (xSemaphoreTake(bin_dyn, 0U) == pdPASS) &&
                    (uxSemaphoreGetCount(bin_dyn) == 0U));

    txfr_test_check("binary GiveFromISR sets count to 1",
                    (xSemaphoreGiveFromISR(bin_stat, &woken) == pdPASS) &&
                    (uxSemaphoreGetCount(bin_stat) == 1U));
    txfr_test_check("binary TakeFromISR takes token",
                    (xSemaphoreTakeFromISR(bin_stat, &woken) == pdPASS) &&
                    (uxSemaphoreGetCount(bin_stat) == 0U));

    TaskHandle_t worker_handle = NULL;

    ctx.sync_obj = bin_dyn;
    ctx.done_sem = ack_sem;
    ctx.take_before_give = pdTRUE;
    txfr_test_check("spawn binary semaphore giver worker",
                    xTaskCreate(bin_sem_giver_task, "bin_give",
                                configMINIMAL_STACK_SIZE * 2U, &ctx,
                                TXFR_SYNC_WORKER_PRIO, &worker_handle) == pdPASS);
    txfr_test_check("blocking take unblocks when worker gives",
                    xSemaphoreTake(bin_dyn, TXFR_SYNC_WAIT_TICKS) == pdPASS);
    txfr_test_check("worker finishes binary semaphore test",
                    xSemaphoreTake(ack_sem, TXFR_SYNC_WAIT_TICKS) == pdPASS);

    vSemaphoreDelete(bin_dyn);
    vSemaphoreDelete(bin_stat);
    vSemaphoreDelete(ack_sem);
}

static void test_counting_semaphores(void)
{
    SemaphoreHandle_t cnt_dyn;
    SemaphoreHandle_t cnt_stat;

    cnt_dyn = xSemaphoreCreateCounting(3U, 1U);
    cnt_stat = xSemaphoreCreateCountingStatic(2U, 2U, &static_cnt_sem);
    txfr_test_check("counting semaphores created",
                    (cnt_dyn != NULL) && (cnt_stat != NULL));

    txfr_test_check("initial counting value matches requested",
                    (uxSemaphoreGetCount(cnt_dyn) == 1U) &&
                    (uxSemaphoreGetCount(cnt_stat) == 2U));
    txfr_test_check("counting give increments up to max_count",
                    (xSemaphoreGive(cnt_dyn) == pdPASS) &&
                    (xSemaphoreGive(cnt_dyn) == pdPASS) &&
                    (uxSemaphoreGetCount(cnt_dyn) == 3U));
    txfr_test_check("counting give past max_count fails",
                    xSemaphoreGive(cnt_dyn) == pdFALSE);

    txfr_test_check("counting take drains all 3 tokens",
                    (xSemaphoreTake(cnt_dyn, 0U) == pdPASS) &&
                    (xSemaphoreTake(cnt_dyn, 0U) == pdPASS) &&
                    (xSemaphoreTake(cnt_dyn, 0U) == pdPASS) &&
                    (uxSemaphoreGetCount(cnt_dyn) == 0U));
    txfr_test_check("counting take on 0 with zero timeout fails",
                    xSemaphoreTake(cnt_dyn, 0U) == pdFALSE);
    txfr_test_check("counting take with short timeout times out",
                    xSemaphoreTake(cnt_dyn, 2U) == pdFALSE);

    vSemaphoreDelete(cnt_dyn);
    vSemaphoreDelete(cnt_stat);
}

static void test_mutexes_and_recursive_mutexes(void)
{
    SemaphoreHandle_t mtx_dyn;
    SemaphoreHandle_t mtx_stat;
    SemaphoreHandle_t rmtx_dyn;
    SemaphoreHandle_t rmtx_stat;
    SemaphoreHandle_t ack_sem;
    sync_worker_ctx_t ctx;
    TaskHandle_t self_handle = xTaskGetCurrentTaskHandle();
    TaskHandle_t contender_handle = NULL;
    TaskHandle_t rec_contender_handle = NULL;
    BaseType_t woken = pdFALSE;

    mtx_dyn = xSemaphoreCreateMutex();
    mtx_stat = xSemaphoreCreateMutexStatic(&static_mutex);
    rmtx_dyn = xSemaphoreCreateRecursiveMutex();
    rmtx_stat = xSemaphoreCreateRecursiveMutexStatic(&static_rec_mutex);
    ack_sem = xSemaphoreCreateBinary();
    txfr_test_check("mutexes and recursive mutexes created",
                    (mtx_dyn != NULL) && (mtx_stat != NULL) &&
                    (rmtx_dyn != NULL) && (rmtx_stat != NULL) &&
                    (ack_sem != NULL));

    txfr_test_check("TakeFromISR on mutex returns pdFALSE",
                    xSemaphoreTakeFromISR(mtx_dyn, &woken) == pdFALSE);

    txfr_test_check("owner acquires mutex",
                    xSemaphoreTake(mtx_dyn, 0U) == pdPASS);
    txfr_test_check("non-recursive mutex refuses same-owner re-take",
                    xSemaphoreTake(mtx_dyn, 0U) == pdFALSE);
    txfr_test_check("mutex holder matches current task",
                    (xSemaphoreGetMutexHolder(mtx_dyn) == self_handle) &&
                    (xSemaphoreGetMutexHolderFromISR(mtx_dyn) == self_handle));

    ctx.sync_obj = mtx_dyn;
    ctx.done_sem = ack_sem;
    ctx.take_before_give = pdFALSE;
    txfr_test_check("spawn mutex contender",
                    xTaskCreate(mutex_contender_task, "mtx_cnt",
                                configMINIMAL_STACK_SIZE * 2U, &ctx,
                                TXFR_SYNC_WORKER_PRIO, &contender_handle) == pdPASS);
    vTaskDelay(2U);
    txfr_test_check("owner releases mutex waking contender",
                    xSemaphoreGive(mtx_dyn) == pdPASS);
    txfr_test_check("mutex contender finishes",
                    xSemaphoreTake(ack_sem, TXFR_SYNC_WAIT_TICKS) == pdPASS);

    txfr_test_check("recursive mutex first take succeeds",
                    xSemaphoreTakeRecursive(rmtx_dyn, 0U) == pdPASS);
    txfr_test_check("recursive mutex second nested take succeeds",
                    xSemaphoreTakeRecursive(rmtx_dyn, 0U) == pdPASS);
    txfr_test_check("first recursive give keeps ownership (count 2 -> 1)",
                    xSemaphoreGiveRecursive(rmtx_dyn) == pdPASS);

    ctx.sync_obj = rmtx_dyn;
    ctx.done_sem = ack_sem;
    txfr_test_check("spawn recursive mutex contender",
                    xTaskCreate(rec_mutex_contender_task, "rmtx_cnt",
                                configMINIMAL_STACK_SIZE * 2U, &ctx,
                                TXFR_SYNC_WORKER_PRIO, &rec_contender_handle) == pdPASS);
    vTaskDelay(2U);
    txfr_test_check("second recursive give releases ownership",
                    xSemaphoreGiveRecursive(rmtx_dyn) == pdPASS);
    txfr_test_check("recursive contender finishes",
                    xSemaphoreTake(ack_sem, TXFR_SYNC_WAIT_TICKS) == pdPASS);

    vSemaphoreDelete(mtx_dyn);
    vSemaphoreDelete(mtx_stat);
    vSemaphoreDelete(rmtx_dyn);
    vSemaphoreDelete(rmtx_stat);
    vSemaphoreDelete(ack_sem);
}

static void sync_driver_task(void *arg)
{
    (void)arg;

    test_binary_semaphores();
    test_counting_semaphores();
    test_mutexes_and_recursive_mutexes();

    xSemaphoreGive(test_done_sem);
    vTaskDelete(NULL);
}

void txfr_test_body(void)
{
    TaskHandle_t drv_handle = NULL;

    test_done_sem = xSemaphoreCreateBinary();
    txfr_test_check("create sync completion semaphore", test_done_sem != NULL);

    txfr_test_check("create sync driver task",
                    xTaskCreate(sync_driver_task, "sync_drv",
                                configMINIMAL_STACK_SIZE * 2U, NULL,
                                TXFR_SYNC_DRIVER_PRIO, &drv_handle) == pdPASS);

    txfr_test_check("sync runtime driver finishes",
                    xSemaphoreTake(test_done_sem, TXFR_SYNC_WAIT_TICKS * 4U) == pdPASS);
    vSemaphoreDelete(test_done_sem);
}
