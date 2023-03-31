/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/*   Copyright (c) Cadence Design Systems, Inc.                           */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** ThreadX Component                                                     */ 
/**                                                                       */
/**   Support for Xtensa applications                                     */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


#ifdef TX_INCLUDE_USER_DEFINE_FILE
#include "tx_user.h"
#endif

#ifdef TX_ENABLE_STACK_CHECKING

/* Include necessary system files.  */

#include "tx_api.h"

#include "xtensa_rtos.h"
#ifdef XT_BOARD
#include <xtensa/xtbsp.h>
#endif
#ifdef XT_SIMULATOR
#include <xtensa/simcall.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


/**************************************************************************/ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    Callback to notify of a stack overflow when registered with         */
/*    tx_stack_error_notify and stack checking is enabled (ThreadX        */
/*    is compiled with TX_ENABLE_STACK_CHECKING defined).                 */
/*                                                                        */
/*    The handler notifies the user in any/all of the following ways:     */
/*    - A message via the simulator (extremely reliable, simulator only). */
/*    - A message on the board's display (emulation board only).          */
/*    - A message to stdout (uses low-level write to avoid printf which   */
/*      is large and would trash state the user might want to examine).   */
/*    The most reliable methods are done first. Several might work.       */
/*                                                                        */
/*    After notifying the user as best it can, the handler stops the      */
/*    application in the most reliable of the following ways:             */
/*    - Passes control to the debugger (if attached).                     */
/*    - Terminates the simulation (simulator only).                       */
/*    - Panics.                                                           */
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-2020     Cadence Design Systems   Initial Version 6.1.3         */
/*                                                                        */ 
/**************************************************************************/ 

VOID  _tx_xtensa_stack_error_handler(TX_THREAD * thread)
{
    #ifdef XT_SIMULATOR
    register int32_t sc  __asm__ ("a2") = SYS_log_msg;
    register char *  msg __asm__ ("a3") 
        = "**** Stack overflow in thread 0x%08x.\n";
    register TX_THREAD * thd __asm__ ("a4") = thread;
    __asm__ volatile ("simcall" :: "a" (sc), "a" (msg), "a" (thd) );
    #endif

    #ifdef XT_BOARD
    xtbsp_display_string("StkOflow");
    #endif

    write(1, "**** Stack overflow in thread \"", 31);
    write(1, thread->tx_thread_name, strlen(thread->tx_thread_name));
    write(1, "\"\n", 2);

    #ifdef XT_SIMULATOR
    sc = SYS_gdb_abort;
    __asm__ volatile ("simcall");           /* control to debugger or exit */
    #else
    __asm__ volatile ("break 1, 15");       /* control to debugger or panic */
    #endif
}

#endif /* TX_ENABLE_STACK_CHECKING */

