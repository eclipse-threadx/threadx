/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
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
/**   Low Power Timer Management                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    tx_low_power.h                                      PORTABLE C      */
/*                                                           6.1.5        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Express Logic, Inc.                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines prototypes for the low-power timer additions      */
/*    required for sleep mode.                                            */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  03-02-2021      William E. Lamie        Initial Version 6.1.5         */
/*                                                                        */
/**************************************************************************/

#ifndef  TX_LOW_POWER_H
#define  TX_LOW_POWER_H

/* Declare low-power function prototypes.  */

VOID        tx_low_power_enter(VOID);
VOID        tx_low_power_exit(VOID);
VOID        tx_time_increment(ULONG time_increment);
ULONG       tx_timer_get_next(ULONG *next_timer_tick_ptr);

#endif
