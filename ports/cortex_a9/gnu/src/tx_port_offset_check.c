/***************************************************************************
 * Copyright (c) 2026-present Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* Verify TX_THREAD offsets used by the ARMv7-A assembly.

   The assembly uses fixed byte offsets to access TX_THREAD fields.
   These checks prevent changes to the C structure layout from
   silently breaking the assembly's assumptions. */

#include "tx_api.h"
#include <stddef.h>

/* Offset of the VFP enable flag as encoded in the ARMv7-A assembly.
   Overridable only so that the assertion itself can be tested. */

#ifndef TX_PORT_VFP_ENABLE_OFFSET
#define TX_PORT_VFP_ENABLE_OFFSET    144
#endif

/* TX_THREAD offsets hard-coded by the ARMv7-A assembly:

   tx_thread_schedule.S:
     run_count #4, stack_ptr #8, time_slice #24, vfp_enable #144

   tx_thread_stack_build.S:
     stack_ptr #8, stack_start #12, stack_end #16

   tx_thread_context_restore.S, tx_thread_fiq_context_restore.S,
   tx_thread_system_return.S:
     stack_ptr #8, time_slice #24, vfp_enable #144 */

typedef char
tx_port_assert_run_count_offset_is_4[
    (offsetof(TX_THREAD, tx_thread_run_count) == 4) ? 1 : -1];

typedef char
tx_port_assert_stack_ptr_offset_is_8[
    (offsetof(TX_THREAD, tx_thread_stack_ptr) == 8) ? 1 : -1];

typedef char
tx_port_assert_stack_start_offset_is_12[
    (offsetof(TX_THREAD, tx_thread_stack_start) == 12) ? 1 : -1];

typedef char
tx_port_assert_stack_end_offset_is_16[
    (offsetof(TX_THREAD, tx_thread_stack_end) == 16) ? 1 : -1];

typedef char
tx_port_assert_time_slice_offset_is_24[
    (offsetof(TX_THREAD, tx_thread_time_slice) == 24) ? 1 : -1];

typedef char
tx_port_assert_vfp_enable_offset_is_144[
    (offsetof(TX_THREAD, tx_thread_vfp_enable) == TX_PORT_VFP_ENABLE_OFFSET)
        ? 1 : -1];
