/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 * Copyright (c) 2026-present Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

// Portions of this file were generated with AI assistance.


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Module Manager                                                      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    txm_module_manager_util.h                           PORTABLE C      */
/*                                                           6.4.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file declares prototypes of utility functions used by the      */
/*    module manager.                                                     */
/*                                                                        */
/**************************************************************************/

#ifndef TXM_MODULE_MANAGER_UTIL_H
#define TXM_MODULE_MANAGER_UTIL_H

/* Define check macros for modules.  */

/* A port supplies TXM_MODULE_MANAGER_CHECK_INSIDE_DATA to decide whether a caller-supplied
   range lies inside the module's writable data or shared memory. Ports whose check can tell
   a read-only region from a writable one also supply the two intent-specific variants below.
   Ports that cannot make the distinction inherit their single check for both intents, so
   their behaviour is unchanged.  */

#ifndef TXM_MODULE_MANAGER_CHECK_INSIDE_DATA_READ
#define TXM_MODULE_MANAGER_CHECK_INSIDE_DATA_READ(module_instance, obj_ptr, obj_size) \
    TXM_MODULE_MANAGER_CHECK_INSIDE_DATA(module_instance, obj_ptr, obj_size)
#endif

#ifndef TXM_MODULE_MANAGER_CHECK_INSIDE_DATA_WRITE
#define TXM_MODULE_MANAGER_CHECK_INSIDE_DATA_WRITE(module_instance, obj_ptr, obj_size) \
    TXM_MODULE_MANAGER_CHECK_INSIDE_DATA(module_instance, obj_ptr, obj_size)
#endif

/* A range is outside the module's data only when no part of it is reachable by the module.
   The read intent is used here because it describes the widest set of reachable addresses.
   A port whose inside check cannot prove the whole range at once must supply its own
   definition, because negating a partial-range check would report a partly reachable range
   as being outside the module.  */

#ifndef TXM_MODULE_MANAGER_CHECK_OUTSIDE_DATA
#define TXM_MODULE_MANAGER_CHECK_OUTSIDE_DATA(module_instance, obj_ptr, obj_size) \
    (!(TXM_MODULE_MANAGER_CHECK_INSIDE_DATA_READ(module_instance, obj_ptr, obj_size)))
#endif

#define TXM_MODULE_MANAGER_CHECK_INSIDE_CODE(module_instance, obj_ptr, obj_size) \
    (((obj_ptr) < ((obj_ptr) + (obj_size))) && \
     ((obj_ptr) >= (ALIGN_TYPE) module_instance -> txm_module_instance_code_start) && \
     (((obj_ptr) + (obj_size)) <= ((ALIGN_TYPE) module_instance -> txm_module_instance_code_end + 1)))

#define TXM_MODULE_MANAGER_CHECK_OUTSIDE_CODE(module_instance, obj_ptr, obj_size) \
    (!(TXM_MODULE_MANAGER_CHECK_INSIDE_CODE(module_instance, obj_ptr, obj_size)))

/* Add sizeof(TXM_MODULE_ALLOCATED_OBJECT) to pool start because the object can't exist before that. */
#define TXM_MODULE_MANAGER_CHECK_INSIDE_OBJ_POOL(module_instance, obj_ptr, obj_size) \
    ((_txm_module_manager_object_pool_created == TX_TRUE) && \
     ((obj_ptr) < ((obj_ptr) + (obj_size))) && \
     (((obj_ptr) >= ((ALIGN_TYPE) _txm_module_manager_object_pool.tx_byte_pool_start + sizeof(TXM_MODULE_ALLOCATED_OBJECT))) && \
      (((obj_ptr) + (obj_size)) <= (ALIGN_TYPE) (_txm_module_manager_object_pool.tx_byte_pool_start + _txm_module_manager_object_pool.tx_byte_pool_size))))

/* Define macros for module.  */

/* The module reads from these ranges, so a read-only region is acceptable.  */
#define TXM_MODULE_MANAGER_ENSURE_INSIDE_MODULE(module_instance, obj_ptr, obj_size) \
    (TXM_MODULE_MANAGER_CHECK_INSIDE_DATA_READ(module_instance, obj_ptr, obj_size) || \
     TXM_MODULE_MANAGER_CHECK_INSIDE_CODE(module_instance, obj_ptr, obj_size))

/* The kernel writes to these ranges, so the module must be able to write them too.  */
#define TXM_MODULE_MANAGER_ENSURE_INSIDE_MODULE_DATA(module_instance, obj_ptr, obj_size) \
    TXM_MODULE_MANAGER_CHECK_INSIDE_DATA_WRITE(module_instance, obj_ptr, obj_size)

#define TXM_MODULE_MANAGER_ENSURE_OUTSIDE_MODULE(module_instance, obj_ptr, obj_size) \
    (TXM_MODULE_MANAGER_CHECK_OUTSIDE_DATA(module_instance, obj_ptr, obj_size) && \
     TXM_MODULE_MANAGER_CHECK_OUTSIDE_CODE(module_instance, obj_ptr, obj_size))

#define TXM_MODULE_MANAGER_ENSURE_INSIDE_OBJ_POOL(module_instance, obj_ptr, obj_size) \
    TXM_MODULE_MANAGER_CHECK_INSIDE_OBJ_POOL(module_instance, obj_ptr, obj_size)

/* Define macros for parameter types.  */

/* Buffers we read from can be in RW/RO/Shared areas.  */
#define TXM_MODULE_MANAGER_PARAM_CHECK_BUFFER_READ(module_instance, buffer_ptr, buffer_size) \
    ((TXM_MODULE_MANAGER_ENSURE_INSIDE_MODULE(module_instance, buffer_ptr, buffer_size)) || \
     ((void *) (buffer_ptr) == TX_NULL))

/* Buffers we write to can only be in RW/Shared areas.  */
#define TXM_MODULE_MANAGER_PARAM_CHECK_BUFFER_WRITE(module_instance, buffer_ptr, buffer_size) \
    ((TXM_MODULE_MANAGER_ENSURE_INSIDE_MODULE_DATA(module_instance, buffer_ptr, buffer_size)) || \
     ((void *) (buffer_ptr) == TX_NULL))

/* Kernel objects a module names must be authenticated before a privileged service is
   allowed to dereference them.  Being outside the module is necessary but nowhere near
   sufficient: the manager's object pool is outside every module, so an address shifted
   into the interior of one of the module's own privileged allocations satisfies that
   test while denoting no object at all.  Authentication answers the question the
   location test cannot -- is this the exact address of a live kernel object of the type
   this service expects -- and it answers it from manager and kernel bookkeeping rather
   than from fields of the purported object, which a module can influence.

   TXM_MODULE_MANAGER_PARAM_CHECK_TYPED_OBJECT_FOR_USE is the complete check and is what
   the dispatch table uses.  It is given the object type rather than a control block size
   so that it can also establish the type, which a size cannot: distinct object types of
   equal size exist.

   TXM_MODULE_MANAGER_PARAM_CHECK_OBJECT_FOR_USE is retained for dispatchers outside this
   repository that pass a size.  It authenticates addresses in the manager's object pool,
   which is what the shifted-pointer attack needs, but without a type it can neither
   establish the type nor authenticate an application-owned object, so in-repository code
   should use the typed form.  */
#define TXM_MODULE_MANAGER_PARAM_CHECK_OBJECT_FOR_USE(module_instance, obj_ptr, obj_size) \
    (_txm_module_manager_param_check_object_for_use(module_instance, obj_ptr, obj_size))

#define TXM_MODULE_MANAGER_PARAM_CHECK_TYPED_OBJECT_FOR_USE(module_instance, obj_ptr, obj_type) \
    (_txm_module_manager_param_check_typed_object_for_use(module_instance, obj_ptr, obj_type))

/* When creating an object, the object must be inside the object pool.  */
#define TXM_MODULE_MANAGER_PARAM_CHECK_OBJECT_FOR_CREATION(module_instance, obj_ptr, obj_size) \
    (_txm_module_manager_param_check_object_for_creation(module_instance, obj_ptr, obj_size))

/* When deleting an object, being allowed to use it is not enough.  Deletion is the
   inverse of creation, so it asks what creation asks: that the object is one this
   module allocated from the manager's object pool, at the exact address the manager
   gave it, for an object of this size.  A module may hold a pointer to an object it
   does not own -- the manager's object lookup service hands application-owned
   objects to modules by name, and sharing an object is what that service is for --
   and this is what separates using such an object from destroying it.  */
#define TXM_MODULE_MANAGER_PARAM_CHECK_OBJECT_FOR_DELETION(module_instance, obj_ptr, obj_size) \
    (_txm_module_manager_allocated_object_check(module_instance, obj_ptr, obj_size))

/* Strings we dereference can be in RW/RO/Shared areas.  */
#define TXM_MODULE_MANAGER_PARAM_CHECK_DEREFERENCE_STRING(module_instance, string_ptr) \
    ((TXM_MODULE_MANAGER_ENSURE_INSIDE_MODULE(module_instance, string_ptr, 1)) || \
     ((void *) (string_ptr) == TX_NULL))

/* Strings we walk are checked over the whole range the walk may reach: the declared
   length plus the terminating character that follows it, since a length excludes the
   terminator and the comparison reads it.  A length whose range cannot be expressed
   is refused rather than truncated.  */
#define TXM_MODULE_MANAGER_PARAM_CHECK_DEREFERENCE_STRING_RANGE(module_instance, string_ptr, string_length) \
    (((((ALIGN_TYPE) (string_length)) < (~((ALIGN_TYPE) 0))) && \
      (TXM_MODULE_MANAGER_ENSURE_INSIDE_MODULE(module_instance, string_ptr, ((ALIGN_TYPE) (string_length)) + ((ALIGN_TYPE) 1)))) || \
     ((void *) (string_ptr) == TX_NULL))

#define TXM_MODULE_MANAGER_UTIL_MAX_VALUE_OF_TYPE_UNSIGNED(type) ((1ULL << (sizeof(type) * 8)) - 1)

#define TXM_MODULE_MANAGER_UTIL_MATH_ADD_ULONG(augend, addend, result) \
    if ((ULONG)-1 - (augend) < (addend))                 \
    {                                                    \
        return(TXM_MODULE_MATH_OVERFLOW);                \
    }                                                    \
    else                                                 \
    {                                                    \
        (result) = (augend) + (addend);                  \
    }

/* Define utility functions.  */

UINT    _txm_module_manager_object_memory_check(TXM_MODULE_INSTANCE *module_instance, ALIGN_TYPE object_ptr, ULONG object_size);
UINT    _txm_module_manager_object_size_check(ALIGN_TYPE object_ptr, ULONG object_size);
UINT    _txm_module_manager_object_name_compare(CHAR *object_name1, UINT object_name1_length, TX_NAME_CONST CHAR *object_name2);
UCHAR   _txm_module_manager_created_object_check(TXM_MODULE_INSTANCE *module_instance, void *object_ptr);
UINT    _txm_module_manager_param_check_object_for_creation(TXM_MODULE_INSTANCE *module_instance, ALIGN_TYPE object_ptr, ULONG object_size);
UINT    _txm_module_manager_param_check_object_for_use(TXM_MODULE_INSTANCE *module_instance, ALIGN_TYPE object_ptr, ULONG object_size);
UINT    _txm_module_manager_param_check_typed_object_for_use(TXM_MODULE_INSTANCE *module_instance, ALIGN_TYPE object_ptr, UINT object_type);
UINT    _txm_module_manager_allocated_object_check(TXM_MODULE_INSTANCE *module_instance, ALIGN_TYPE object_ptr, ULONG object_size);
TXM_MODULE_ALLOCATED_OBJECT
        *_txm_module_manager_allocated_object_find(TXM_MODULE_INSTANCE *module_instance, ALIGN_TYPE object_ptr, ULONG *object_size_ptr);
UINT    _txm_module_manager_object_type_size_get(UINT object_type, ULONG *object_size);
UINT    _txm_module_manager_created_object_type_check(ALIGN_TYPE object_ptr, UINT object_type);
UINT    _txm_module_manager_object_id_check(ALIGN_TYPE object_ptr, UINT object_type);
UINT    _txm_module_manager_live_object_check(ALIGN_TYPE object_ptr);
UINT    _txm_module_manager_util_code_allocation_size_and_alignment_get(TXM_MODULE_PREAMBLE *module_preamble, ULONG *code_alignment_dest, ULONG *code_allocation_size_dest);

#endif
