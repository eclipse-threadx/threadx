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
/** POSIX wrapper for THREADX                                             */ 
/**                                                                       */
/**                                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/* Include necessary system files.  */

#include "tx_api.h"    /* Threadx API */
#include "pthread.h"  /* Posix API */
#include "px_int.h"    /* Posix helper functions */


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    pthread_attr_getstack                               PORTABLE C      */ 
/*                                                           6.1.7        */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*     This function gets the thread creation stack attributes stackaddr  */
/*     and stacksize in the attr object.                                  */
/*     The stack attributes specify the area of storage to be used for the*/
/*     created thread's stack. The base  (lowest addressable byte) of the */
/*     storage shall be stackaddr , and the size of the storage shall be  */
/*     stacksize bytes.                                                   */  
/*                                                                        */
/*  INPUT                                                                 */ 
/*                                                                        */
/*    attr                           Address of the thread attributes     */
/*    stackaddr                      Pointer to hold Address of stack     */
/*    stacksize                      Holds the stack size                 */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*     0                             if successful                        */ 
/*     Value                         in case of any error                 */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  06-02-2021     William E. Lamie         Initial Version 6.1.7         */
/*                                                                        */ 
/**************************************************************************/ 
INT pthread_attr_getstack( pthread_attr_t *attr,void **stackaddr,
                         size_t *stacksize)
{
    /* First check the attribute object is already destroyed? */
    if (attr->inuse == TX_FALSE)
    {
        posix_errno = EINVAL;
	    posix_set_pthread_errno(EINVAL);
        return(EINVAL);
    }   
    else
    {
        *stackaddr = attr->stack_address;
        *stacksize = attr->stack_size ;
        return(OK);
    }
}
