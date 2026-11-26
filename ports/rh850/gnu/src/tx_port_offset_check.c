/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#include <stddef.h>
#include "tx_api.h"
#include "tx_port_asm.h"

#define TX_PORT_CHECK(name, cond) typedef char tx_port_check_##name[(cond) ? 1 : -1]

TX_PORT_CHECK(run_count, offsetof(TX_THREAD, tx_thread_run_count) == TX_THREAD_RUN_COUNT);
TX_PORT_CHECK(stack_ptr, offsetof(TX_THREAD, tx_thread_stack_ptr) == TX_THREAD_STACK_PTR);
TX_PORT_CHECK(stack_end, offsetof(TX_THREAD, tx_thread_stack_end) == TX_THREAD_STACK_END);
TX_PORT_CHECK(time_slice, offsetof(TX_THREAD, tx_thread_time_slice) == TX_THREAD_TIME_SLICE);
TX_PORT_CHECK(frame_size, TX_FRAME_R(31) + 4 <= TX_FRAME_SIZE);
TX_PORT_CHECK(frame_align, (TX_FRAME_SIZE % 8) == 0);
TX_PORT_CHECK(min_stack, TX_MINIMUM_STACK >= 2 * TX_FRAME_SIZE);
