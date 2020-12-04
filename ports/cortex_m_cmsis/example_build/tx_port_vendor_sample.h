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
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Port Specific                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/**************************************************************************/
/*                                                                        */
/*  PORT SPECIFIC C INFORMATION                            RELEASE        */
/*                                                                        */
/*    tx_port_vendor_sample.h                           Cortex-M/CMSIS    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file contains vendor specific customizations of the            */
/*    Cortex-M/CMSIS port.                                                */
/*                                                                        */
/**************************************************************************/

/* If the MCU supports PSPLIM, define TX_PORT_PSPLIM_PRESENT. */
/* #define TX_PORT_PSPLIM_PRESENT */

/* If a vendor specific stack monitor is used, define the following:
 *   - TX_PORT_VENDOR_STACK_MONITOR_ENABLE
 *   - TX_PORT_VENDOR_ASM_STACK_MONITOR_CONFIGURE (new thread pointer is in r6)
 *   - TX_PORT_VENDOR_ASM_STACK_MONITOR_ENABLE
 */
/* #define TX_PORT_VENDOR_STACK_MONITOR_ENABLE */

/* Prototype of function that can be overridden to handle low power control
 * in idle processing. */
void * _tx_port_wait_thread_ready(void);
