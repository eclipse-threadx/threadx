
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
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file contains macro constants for ThreadX API structures       */
/*    and enums that need to be used in assembly coded port sources.      */
/*    Most of these constants are derived from definitions in tx_api.h.   */
/*    Only the constants that are needed are included here to reduce      */
/*    the maintenance required when the structures or enums change.       */
/*    Structure offsets depend on the compiler, so are tools-specific,    */
/*    which usually means port-specific since a compiler's struct         */
/*    packing rules depend on properties of the target architecture.      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  12-31-2020     Cadence Design Systems   Initial Version 6.1.3         */
/*                                                                        */
/**************************************************************************/

#ifndef TX_API_ASM_H
#define TX_API_ASM_H

#include "tx_port.h"

/* API input parameters and general constants.  */

#define TX_TRUE                         1
#define TX_FALSE                        0
#define TX_NULL                         0

/* ThreadX thread control block structure.  */
//  typedef struct TX_THREAD_STRUCT
//  {

#define tx_thread_run_count             0x04
#define tx_thread_stack_ptr             0x08
#define tx_thread_stack_end             0x10
#define tx_thread_time_slice            0x18
#define tx_thread_new_time_slice        0x1C
#define tx_thread_solicited             0x28

#ifdef TX_THREAD_SAFE_CLIB
  #define tx_real_thread_entry          0x2C
  #define tx_thread_clib_ptr            0x30
  #define tx_thread_cp_state            0x34
#else
  #define tx_thread_cp_state            0x2C
#endif

//  }

#endif /* TX_API_ASM_H */

