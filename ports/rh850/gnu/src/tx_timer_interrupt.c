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
/**   Timer                                                               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE

#include "tx_api.h"
#include "tx_timer.h"
#include "tx_thread.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_timer_interrupt                                   RH850/GNU     */
/*                                                           6.5.2        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Parthiban Nallathambi, Linumiz                                      */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes the hardware timer interrupt. This          */
/*    processing includes incrementing the system clock and checking for  */
/*    time slice and/or timer expiration. If either is found, the         */
/*    expiration functions are called.                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _tx_timer_expiration_process         Timer expiration processing    */
/*    _tx_thread_time_slice                Time slice interrupted thread  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    tick interrupt                       Board tick ISR                 */
/*                                                                        */
/**************************************************************************/
VOID _tx_timer_interrupt(VOID)
{
	TX_INTERRUPT_SAVE_AREA
	UINT expired;
	UINT slice;

	TX_DISABLE
	_tx_timer_system_clock++;
	if (_tx_timer_time_slice != 0U) {
		_tx_timer_time_slice--;
		if (_tx_timer_time_slice == 0U) {
			_tx_timer_expired_time_slice = TX_TRUE;
		}
	}
	if (*_tx_timer_current_ptr != TX_NULL) {
		_tx_timer_expired = TX_TRUE;
	} else {
		_tx_timer_current_ptr++;
		if (_tx_timer_current_ptr == _tx_timer_list_end) {
			_tx_timer_current_ptr = _tx_timer_list_start;
		}
	}
	expired = _tx_timer_expired;
	slice = _tx_timer_expired_time_slice;
	TX_RESTORE

	if (expired != TX_FALSE) {
		_tx_timer_expiration_process();
	}
	if (slice != TX_FALSE) {
		_tx_thread_time_slice();
	}
}
