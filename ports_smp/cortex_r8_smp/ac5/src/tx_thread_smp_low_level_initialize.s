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
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

    AREA ||.text||, CODE, READONLY
        PRESERVE8
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_smp_low_level_initialize             SMP/Cortex-R8/ARM   */
/*                                                           6.2.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function performs low-level initialization of the booting      */
/*    core.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    number_of_cores                       Number of cores               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_initialize_high_level            ThreadX high-level init        */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-31-2022      Scott Larson            Initial Version 6.2.0         */
/*                                                                        */
/**************************************************************************/
    EXPORT  _tx_thread_smp_low_level_initialize
_tx_thread_smp_low_level_initialize

    IF  {INTER} = {TRUE}
    BX      lr                                  // Return to caller
    ELSE
    MOV     pc, lr                              // Return to caller
    ENDIF

    END
