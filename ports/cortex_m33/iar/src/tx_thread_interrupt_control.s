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
;
;
    SECTION `.text`:CODE:NOROOT(2)
    THUMB
;/**************************************************************************/
;/*                                                                        */
;/*  FUNCTION                                               RELEASE        */
;/*                                                                        */
;/*    _tx_thread_interrupt_control                      Cortex-M33/IAR    */
;/*                                                           6.1          */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    Scott Larson, Microsoft Corporation                                 */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */
;/*    This function is responsible for changing the interrupt lockout     */
;/*    posture of the system.                                              */
;/*                                                                        */
;/*  INPUT                                                                 */
;/*                                                                        */
;/*    new_posture                           New interrupt lockout posture */
;/*                                                                        */
;/*  OUTPUT                                                                */
;/*                                                                        */
;/*    old_posture                           Old interrupt lockout posture */
;/*                                                                        */
;/*  CALLS                                                                 */
;/*                                                                        */
;/*    None                                                                */
;/*                                                                        */
;/*  CALLED BY                                                             */
;/*                                                                        */
;/*    Application Code                                                    */
;/*                                                                        */
;/*  RELEASE HISTORY                                                       */
;/*                                                                        */
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  09-30-2020      Scott Larson            Initial Version 6.1           */
;/*                                                                        */
;/**************************************************************************/
;UINT   _tx_thread_interrupt_control(UINT new_posture)
;{
    PUBLIC  _tx_thread_interrupt_control
_tx_thread_interrupt_control:
;
;    /* Pickup current interrupt lockout posture.  */
;
    MRS     r1, PRIMASK
    MSR     PRIMASK, r0
    MOV     r0, r1
    BX      lr
;
;}
    END
