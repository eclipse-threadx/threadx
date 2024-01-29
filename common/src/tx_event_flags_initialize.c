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
/**   Event Flags                                                         */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_event_flags.h"


#ifndef TX_INLINE_INITIALIZATION

/* Locate event flags component data in this file.  */
/* Define the head pointer of the created event flags list.  */

TX_EVENT_FLAGS_GROUP * _tx_event_flags_created_ptr;


/* Define the variable that holds the number of created event flag groups. */

ULONG                  _tx_event_flags_created_count;


#ifdef TX_EVENT_FLAGS_ENABLE_PERFORMANCE_INFO

/* Define the total number of event flag sets.  */

ULONG                  _tx_event_flags_performance_set_count;


/* Define the total number of event flag gets.  */

ULONG                  _tx_event_flags_performance_get_count;


/* Define the total number of event flag suspensions.  */

ULONG                  _tx_event_flags_performance_suspension_count;


/* Define the total number of event flag timeouts.  */

ULONG                  _tx_event_flags_performance_timeout_count;


#endif



/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_event_flags_initialize                          PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function initializes the various control data structures for   */
/*    the event flags component.                                          */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_initialize_high_level         High level initialization         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     William E. Lamie         Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s),          */
/*                                            opt out of function when    */
/*                                            TX_INLINE_INITIALIZATION is */
/*                                            defined,                    */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
VOID  _tx_event_flags_initialize(VOID)
{

#ifndef TX_DISABLE_REDUNDANT_CLEARING

    /* Initialize the head pointer of the created event flags list and the
       number of event flags created.  */
    _tx_event_flags_created_ptr =        TX_NULL;
    _tx_event_flags_created_count =      TX_EMPTY;

#ifdef TX_EVENT_FLAGS_ENABLE_PERFORMANCE_INFO

    /* Initialize event flags performance counters.  */
    _tx_event_flags_performance_set_count =         ((ULONG) 0);
    _tx_event_flags_performance_get_count =         ((ULONG) 0);
    _tx_event_flags_performance_suspension_count =  ((ULONG) 0);
    _tx_event_flags_performance_timeout_count =     ((ULONG) 0);
#endif
#endif
}
#endif
