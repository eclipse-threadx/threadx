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
/**   Module                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TXM_MODULE
#include "txm_module.h"
#ifndef TXM_TRACE_ENABLE_CALL_NOT_USED
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_trace_enable                                    PORTABLE C      */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function initializes the ThreadX trace buffer and the          */
/*    associated control variables, enabling it for operation.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    trace_buffer_start                    Start of trace buffer         */
/*    trace_buffer_size                     Size (bytes) of trace buffer  */
/*    registry_entries                      Number of object registry     */
/*                                            entries.                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _txm_module_kernel_call_dispatcher                                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Module application code                                             */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020      Scott Larson            Initial Version 6.1           */
/*  01-31-2022      Scott Larson            Modified comments and added   */
/*                                            CALL_NOT_USED option,       */
/*                                            resulting in version 6.1.10 */
/*                                                                        */
/**************************************************************************/
UINT _tx_trace_enable(VOID *trace_buffer_start, ULONG trace_buffer_size, ULONG registry_entries)
{

UINT return_value;

    /* Call module manager dispatcher.  */
    return_value = (UINT) (_txm_module_kernel_call_dispatcher)(TXM_TRACE_ENABLE_CALL, (ALIGN_TYPE) trace_buffer_start, (ALIGN_TYPE) trace_buffer_size, (ALIGN_TYPE) registry_entries);

    /* Return value to the caller.  */
    return(return_value);
}
#endif
