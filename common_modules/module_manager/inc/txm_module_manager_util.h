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
/**   Module Manager                                                      */ 
/**                                                                       */ 
/**************************************************************************/ 
/**************************************************************************/ 


/**************************************************************************/ 
/*                                                                        */ 
/*  COMPONENT DEFINITION                                   RELEASE        */ 
/*                                                                        */ 
/*    txm_module_manager_util.h                           PORTABLE C      */ 
/*                                                           6.0.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file declares prototypes of utility functions used by the      */ 
/*    module manager.                                                     */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  06-30-2020     Scott Larson             Initial Version 6.0.1         */
/*                                                                        */
/**************************************************************************/

#ifndef TXM_MODULE_MANAGER_UTIL_H
#define TXM_MODULE_MANAGER_UTIL_H

/* Define utility macros.  */

/* Define inside/outside check macros. The _INCLUSIVE/_EXCLUSIVE suffix applies only to range_end.  */

#define TXM_MODULE_MANAGER_CHECK_OUTSIDE_RANGE_INCLUSIVE(range_start, range_end, obj_start, obj_size) \
    ((obj_start) > (ALIGN_TYPE) (range_end) || \
     ((obj_start) + (obj_size)) <= (ALIGN_TYPE) (range_start))

#define TXM_MODULE_MANAGER_CHECK_OUTSIDE_RANGE_EXCLUSIVE(range_start, range_end, obj_start, obj_size) \
    ((obj_start) >= (ALIGN_TYPE) (range_end) || \
     ((obj_start) + (obj_size)) <= (ALIGN_TYPE) (range_start))

#define TXM_MODULE_MANAGER_CHECK_INSIDE_RANGE_INCLUSIVE(range_start, range_end, obj_start, obj_size) \
    (((obj_start) >= (ALIGN_TYPE) (range_start)) && \
     (((obj_start) + (obj_size)) <= (ALIGN_TYPE) (range_end) + 1))

#define TXM_MODULE_MANAGER_CHECK_INSIDE_RANGE_INCLUSIVE_BYTE(range_start, range_end, byte_ptr) \
    (((byte_ptr) >= (ALIGN_TYPE) (range_start)) && \
     ((byte_ptr) <= (ALIGN_TYPE) (range_end)))

#define TXM_MODULE_MANAGER_CHECK_INSIDE_RANGE_EXCLUSIVE(range_start, range_end, obj_start, obj_size) \
    (((obj_start) >= (ALIGN_TYPE) (range_start)) && \
     (((obj_start) + (obj_size)) <= (ALIGN_TYPE) (range_end)))

#define TXM_MODULE_MANAGER_CHECK_INSIDE_RANGE_EXCLUSIVE_BYTE(range_start, range_end, byte_ptr) \
    (((byte_ptr) >= (ALIGN_TYPE) (range_start)) && \
     ((byte_ptr) < (ALIGN_TYPE) (range_end)))

/* Define check macros for modules.  */

#define TXM_MODULE_MANAGER_CHECK_OUTSIDE_DATA(module_instance, obj_ptr, obj_size) \
    TXM_MODULE_MANAGER_CHECK_OUTSIDE_RANGE_INCLUSIVE(module_instance -> txm_module_instance_data_start, \
                                                     module_instance -> txm_module_instance_data_end, \
                                                     obj_ptr, obj_size)

#define TXM_MODULE_MANAGER_CHECK_OUTSIDE_CODE(module_instance, obj_ptr, obj_size) \
    TXM_MODULE_MANAGER_CHECK_OUTSIDE_RANGE_INCLUSIVE(module_instance -> txm_module_instance_code_start, \
                                                     module_instance -> txm_module_instance_code_end, \
                                                     obj_ptr, obj_size)

#define TXM_MODULE_MANAGER_CHECK_INSIDE_DATA(module_instance, obj_ptr, obj_size) \
    TXM_MODULE_MANAGER_CHECK_INSIDE_RANGE_INCLUSIVE(module_instance -> txm_module_instance_data_start, \
                                                    module_instance -> txm_module_instance_data_end, \
                                                    obj_ptr, obj_size)

#define TXM_MODULE_MANAGER_CHECK_INSIDE_DATA_BYTE(module_instance, byte_ptr) \
    TXM_MODULE_MANAGER_CHECK_INSIDE_RANGE_INCLUSIVE_BYTE(module_instance -> txm_module_instance_data_start, \
                                                         module_instance -> txm_module_instance_data_end, \
                                                         byte_ptr)

#define TXM_MODULE_MANAGER_CHECK_INSIDE_CODE(module_instance, obj_ptr, obj_size) \
    TXM_MODULE_MANAGER_CHECK_INSIDE_RANGE_INCLUSIVE(module_instance -> txm_module_instance_code_start, \
                                                    module_instance -> txm_module_instance_code_end, \
                                                    obj_ptr, obj_size)

#define TXM_MODULE_MANAGER_CHECK_INSIDE_CODE_BYTE(module_instance, byte_ptr) \
    TXM_MODULE_MANAGER_CHECK_INSIDE_RANGE_INCLUSIVE_BYTE(module_instance -> txm_module_instance_code_start, \
                                                    module_instance -> txm_module_instance_code_end, \
                                                    byte_ptr)

#define TXM_MODULE_MANAGER_CHECK_INSIDE_OBJ_POOL(module_instance, obj_ptr, obj_size) \
    ((_txm_module_manager_object_pool_created == TX_TRUE) && \
     TXM_MODULE_MANAGER_CHECK_INSIDE_RANGE_EXCLUSIVE(_txm_module_manager_object_pool.tx_byte_pool_start, \
                                                     _txm_module_manager_object_pool.tx_byte_pool_start + _txm_module_manager_object_pool.tx_byte_pool_size, \
                                                     obj_ptr, obj_size))

/* Define macros for module.  */

#define TXM_MODULE_MANAGER_ENSURE_INSIDE_MODULE(module_instance, obj_ptr, obj_size) \
    (TXM_MODULE_MANAGER_CHECK_INSIDE_DATA(module_instance, obj_ptr, obj_size) || \
     _txm_module_manager_shared_memory_check_inside(module_instance, (ALIGN_TYPE) obj_ptr, obj_size) || \
     TXM_MODULE_MANAGER_CHECK_INSIDE_CODE(module_instance, obj_ptr, obj_size))

#define TXM_MODULE_MANAGER_ENSURE_INSIDE_MODULE_BYTE(module_instance, byte_ptr) \
    (TXM_MODULE_MANAGER_CHECK_INSIDE_DATA_BYTE(module_instance, byte_ptr) || \
     _txm_module_manager_shared_memory_check_inside_byte(module_instance, (ALIGN_TYPE) byte_ptr) || \
     TXM_MODULE_MANAGER_CHECK_INSIDE_CODE_BYTE(module_instance, byte_ptr))

#define TXM_MODULE_MANAGER_ENSURE_INSIDE_MODULE_DATA(module_instance, obj_ptr, obj_size) \
    (TXM_MODULE_MANAGER_CHECK_INSIDE_DATA(module_instance, obj_ptr, obj_size) || \
     _txm_module_manager_shared_memory_check_inside(module_instance, (ALIGN_TYPE) obj_ptr, obj_size))

#define TXM_MODULE_MANAGER_ENSURE_OUTSIDE_MODULE(module_instance, obj_ptr, obj_size) \
    (TXM_MODULE_MANAGER_CHECK_OUTSIDE_DATA(module_instance, obj_ptr, obj_size) && \
     _txm_module_manager_shared_memory_check_outside(module_instance, (ALIGN_TYPE) obj_ptr, obj_size) && \
     TXM_MODULE_MANAGER_CHECK_OUTSIDE_CODE(module_instance, obj_ptr, obj_size))

#define TXM_MODULE_MANAGER_ENSURE_INSIDE_OBJ_POOL(module_instance, obj_ptr, obj_size) \
    (TXM_MODULE_MANAGER_CHECK_INSIDE_OBJ_POOL(module_instance, obj_ptr, obj_size))

/* Define macros for parameter types.  */

/* Buffers we read from can be in RW/RO/Shared areas.  */
#define TXM_MODULE_MANAGER_PARAM_CHECK_BUFFER_READ(module_instance, buffer_ptr, buffer_size) \
    (((void *) (buffer_ptr) == TX_NULL) || \
     (TXM_MODULE_MANAGER_ENSURE_INSIDE_MODULE(module_instance, buffer_ptr, buffer_size)))

/* Buffers we write to can only be in RW/Shared areas.  */
#define TXM_MODULE_MANAGER_PARAM_CHECK_BUFFER_WRITE(module_instance, buffer_ptr, buffer_size) \
    (((void *) (buffer_ptr) == TX_NULL) || \
     (TXM_MODULE_MANAGER_ENSURE_INSIDE_MODULE_DATA(module_instance, buffer_ptr, buffer_size)))

/* Kernel objects should be outside the module at the very least.  */
#define TXM_MODULE_MANAGER_PARAM_CHECK_OBJECT_FOR_USE(module_instance, obj_ptr, obj_size) \
    (((void *) (obj_ptr) == TX_NULL) || \
     (TXM_MODULE_MANAGER_ENSURE_OUTSIDE_MODULE(module_instance, obj_ptr, obj_size)))

/* When creating an object, the object must be inside the object pool.  */
#define TXM_MODULE_MANAGER_PARAM_CHECK_OBJECT_FOR_CREATION(module_instance, obj_ptr, obj_size) \
    (((void *) (obj_ptr) == TX_NULL) || \
     (TXM_MODULE_MANAGER_ENSURE_INSIDE_OBJ_POOL(module_instance, obj_ptr, obj_size) && \
      (_txm_module_manager_object_size_check(obj_ptr, obj_size) == TX_SUCCESS)))

/* Strings we dereference can be in RW/RO/Shared areas.  */
#define TXM_MODULE_MANAGER_PARAM_CHECK_DEREFERENCE_STRING(module_instance, string_ptr) \
    (((void *) (string_ptr) == TX_NULL) || \
     (TXM_MODULE_MANAGER_ENSURE_INSIDE_MODULE_BYTE(module_instance, string_ptr)))

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
UINT    _txm_module_manager_object_name_compare(CHAR *object_name1, UINT object_name1_length, CHAR *object_name2);
UCHAR   _txm_module_manager_created_object_check(TXM_MODULE_INSTANCE *module_instance, void *object_ptr);
UINT    _txm_module_manager_util_code_allocation_size_and_alignment_get(TXM_MODULE_PREAMBLE *module_preamble, ULONG *code_alignment_dest, ULONG *code_allocation_size_dest);

#endif
