;/**************************************************************************/
;/*                                                                        */
;/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
;/*                                                                        */
;/*       This software is licensed under the Microsoft Software License   */
;/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
;/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
;/*       and in the root directory of this software.                      */
;/*                                                                        */
;/**************************************************************************/
;
;
;/**************************************************************************/
;/**************************************************************************/
;/**                                                                       */
;/** ThreadX Component                                                     */
;/**                                                                       */
;/**   Thread                                                              */
;/**                                                                       */
;/**************************************************************************/
;/**************************************************************************/

    section .text:CODE:ROOT
    
;/**************************************************************************/
;/*                                                                        */
;/*  FUNCTION                                               RELEASE        */
;/*                                                                        */
;/*    _tx_thread_system_return                             RXv2/IAR       */
;/*                                                           6.1.3        */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */
;/*    This function is target processor specific.  It is used to transfer */
;/*    control from a thread back to the system.  Only a minimal context   */
;/*    is saved since the compiler assumes temp registers are going to get */
;/*    slicked by a function call anyway.                                  */
;/*                                                                        */
;/*  INPUT                                                                 */
;/*                                                                        */
;/*    None                                                                */
;/*                                                                        */
;/*  OUTPUT                                                                */
;/*                                                                        */
;/*    None                                                                */
;/*                                                                        */
;/*  CALLS                                                                 */
;/*                                                                        */
;/*    _tx_thread_schedule                   Thread scheduling loop        */
;/*                                                                        */
;/*  CALLED BY                                                             */
;/*                                                                        */
;/*    ThreadX components                                                  */
;/*                                                                        */
;/*  RELEASE HISTORY                                                       */
;/*                                                                        */
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  12-31-2020     William E. Lamie         Initial Version 6.1.3         */
;/*                                                                        */
;/**************************************************************************/

    public __tx_thread_system_return

__tx_thread_system_return:

    BRA __tx_thread_system_return

    RTS

    END
