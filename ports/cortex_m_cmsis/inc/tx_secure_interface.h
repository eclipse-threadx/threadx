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
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    tx_secure_interface.h                             Cortex-M/CMSIS    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the ThreadX secure thread stack components,       */
/*    including data types and external references.                       */
/*    It is assumed that tx_api.h and tx_port.h have already been         */
/*    included.                                                           */
/*                                                                        */
/**************************************************************************/

#ifndef TX_SECURE_INTERFACE_H
#define TX_SECURE_INTERFACE_H

/* Define SVC numbers used to call CMSIS TrustZone secure context functions.  */

#define TX_SVC_NUM_SECURE_ALLOC               1
#define TX_SVC_NUM_SECURE_FREE                2
#define TX_SVC_NUM_SECURE_INIT                3

#endif
