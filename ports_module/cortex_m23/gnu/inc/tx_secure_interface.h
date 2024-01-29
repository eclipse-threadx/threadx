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


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    tx_secure_interface.h                               Cortex-M23      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the ThreadX secure thread stack components,       */
/*    including data types and external references.                       */
/*    It is assumed that tx_api.h and tx_port.h have already been         */
/*    included.                                                           */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020      Scott Larson            Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/

#ifndef TX_SECURE_INTERFACE_H
#define TX_SECURE_INTERFACE_H

/* Define internal secure thread stack function prototypes.  */

extern void    _tx_thread_secure_stack_initialize(void);
extern UINT    _tx_thread_secure_mode_stack_allocate(TX_THREAD *thread_ptr, ULONG stack_size);
extern UINT    _tx_thread_secure_mode_stack_free(TX_THREAD *thread_ptr);
extern void    _tx_thread_secure_stack_context_save(TX_THREAD *thread_ptr);
extern void    _tx_thread_secure_stack_context_restore(TX_THREAD *thread_ptr);

#endif
