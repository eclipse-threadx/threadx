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
/**   Module                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  APPLICATION INTERFACE DEFINITION                       RELEASE        */
/*                                                                        */
/*    txm_module_port.h                                    RXv2/IAR       */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the basic module constants, interface structures, */
/*    and function prototypes.                                            */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  01-31-2022      Scott Larson            Initial Version 6.1.10        */
/*                                                                        */
/**************************************************************************/

#ifndef TXM_MODULE_PORT_H
#define TXM_MODULE_PORT_H

/* Determine if the optional Modules user define file should be used.  */

#ifdef TXM_MODULE_INCLUDE_USER_DEFINE_FILE


/* Yes, include the user defines in txm_module_user.h. The defines in this file may
   alternately be defined on the command line.  */

#include "txm_module_user.h"
#endif

/* It is assumed that the base ThreadX tx_port.h file has been modified to add the
  following extensions to the ThreadX thread control block (this code should replace
  the corresponding macro define in tx_port.h):

#define TX_THREAD_EXTENSION_2               VOID    *tx_thread_module_instance_ptr;         \
                                            VOID    *tx_thread_module_entry_info_ptr;       \
                                            ULONG   tx_thread_module_current_user_mode;     \
                                            ULONG   tx_thread_module_user_mode;             \
                                            VOID    *tx_thread_module_kernel_stack_start;   \
                                            VOID    *tx_thread_module_kernel_stack_end;     \
                                            ULONG   tx_thread_module_kernel_stack_size;     \
                                            VOID    *tx_thread_module_stack_ptr;            \
                                            VOID    *tx_thread_module_stack_start;          \
                                            VOID    *tx_thread_module_stack_end;            \
                                            ULONG   tx_thread_module_stack_size;            \
                                            VOID    *tx_thread_module_reserved;             \
                                            VOID    *tx_thread_iar_tls_pointer;

The following extensions must also be defined in tx_port.h:

#define TX_EVENT_FLAGS_GROUP_EXTENSION          VOID    *tx_event_flags_group_module_instance; \
                                                VOID   (*tx_event_flags_group_set_module_notify)(struct TX_EVENT_FLAGS_GROUP_STRUCT *group_ptr);

#define TX_QUEUE_EXTENSION                      VOID    *tx_queue_module_instance; \
                                                VOID   (*tx_queue_send_module_notify)(struct TX_QUEUE_STRUCT *queue_ptr);

#define TX_SEMAPHORE_EXTENSION                  VOID    *tx_semaphore_module_instance; \
                                                VOID   (*tx_semaphore_put_module_notify)(struct TX_SEMAPHORE_STRUCT *semaphore_ptr);

#define TX_TIMER_EXTENSION                      VOID    *tx_timer_module_instance; \
                                                VOID   (*tx_timer_module_expiration_function)(ULONG id);
*/

#define TXM_MODULE_THREAD_ENTRY_INFO_USER_EXTENSION 

/* Define constants specific to the tools the module can be built with for this particular modules port.  */

#define TXM_MODULE_IAR_COMPILER                 0x00000000
#define TXM_MODULE_RVDS_COMPILER                0x01000000
#define TXM_MODULE_GNU_COMPILER                 0x02000000
#define TXM_MODULE_COMPILER_MASK                0xFF000000
#define TXM_MODULE_OPTIONS_MASK                 0x000000FF


/* Define the properties for this particular module port.  */

#define TXM_MODULE_MEMORY_PROTECTION_ENABLED
#ifdef  TXM_MODULE_MEMORY_PROTECTION_ENABLED
#define TXM_MODULE_REQUIRE_ALLOCATED_OBJECT_MEMORY
#else
#define TXM_MODULE_REQUIRE_LOCAL_OBJECT_MEMORY
#endif

#define TXM_MODULE_USER_MODE                        0x00000001
#define TXM_MODULE_MEMORY_PROTECTION                0x00000002
#define TXM_MODULE_SHARED_EXTERNAL_MEMORY_ACCESS    0x00000004

/* Define the kernel stack size for a module thread.  */
#ifndef TXM_MODULE_KERNEL_STACK_SIZE
#define TXM_MODULE_KERNEL_STACK_SIZE            512
#endif


/* Define the supported options for this module.   */

#define TXM_MODULE_MANAGER_SUPPORTED_OPTIONS        (TXM_MODULE_USER_MODE | TXM_MODULE_MEMORY_PROTECTION | TXM_MODULE_SHARED_EXTERNAL_MEMORY_ACCESS)
#define TXM_MODULE_MANAGER_REQUIRED_OPTIONS         0


/* Define offset adjustments according to the compiler used to build the module.  */

#define TXM_MODULE_IAR_SHELL_ADJUST                 24
#define TXM_MODULE_IAR_START_ADJUST                 28
#define TXM_MODULE_IAR_STOP_ADJUST                  32
#define TXM_MODULE_IAR_CALLBACK_ADJUST              44

#define TXM_MODULE_RVDS_SHELL_ADJUST                0
#define TXM_MODULE_RVDS_START_ADJUST                0
#define TXM_MODULE_RVDS_STOP_ADJUST                 0
#define TXM_MODULE_RVDS_CALLBACK_ADJUST             0

#define TXM_MODULE_GNU_SHELL_ADJUST                 24
#define TXM_MODULE_GNU_START_ADJUST                 28
#define TXM_MODULE_GNU_STOP_ADJUST                  32
#define TXM_MODULE_GNU_CALLBACK_ADJUST              44


/* Define other module port-specific constants.  */

/* Define INLINE_DECLARE to inline for IAR compiler.  */

#define INLINE_DECLARE inline

/* Define the data area alignment mask, must be a power of 2.  */

#ifndef TXM_MODULE_DATA_ALIGNMENT
#define TXM_MODULE_DATA_ALIGNMENT                   16
#endif


/* Define the code area alignment mask, must be a power of 2.  */

#ifndef TXM_MODULE_CODE_ALIGNMENT
#define TXM_MODULE_CODE_ALIGNMENT                   16
#endif


/* Define the number of MPU entries assigned to the code and data sections.  */

#define TXM_MODULE_MANAGER_CODE_MPU_ENTRIES         2
#define TXM_MODULE_MANAGER_DATA_MPU_ENTRIES         1

/* Total MPU entries available.  */
#define TXM_MODULE_MANAGER_RX_MPU_ENTRIES           8
#define TXM_MODULE_MANAGER_REGISTERS_PER_MPU_ENTRY  2
#define TXM_MODULE_MANAGER_MPU_TABLE_SIZE           (TXM_MODULE_MANAGER_RX_MPU_ENTRIES * TXM_MODULE_MANAGER_REGISTERS_PER_MPU_ENTRY)

/* Shared memory MPU index.  */
#define TXM_MODULE_MANAGER_SHARED_MPU_INDEX         6

/* Addresses of MPU registers for trampoline protection.  */
#define RSPAGE0                                     (*(ULONG *) 0x00086400)
#define REPAGE0                                     (*(ULONG *) 0x00086404)

/* Shared memory region attributes.  */
#define TXM_MODULE_MANAGER_SHARED_ATTRIBUTE_EXECUTE     1
#define TXM_MODULE_MANAGER_SHARED_ATTRIBUTE_WRITE       2
#define TXM_MODULE_MANAGER_SHARED_ATTRIBUTE_READ        4

#define TXM_MODULE_MANAGER_ATTRIBUTE_EXECUTE_MPU_BIT    (TXM_MODULE_MANAGER_SHARED_ATTRIBUTE_EXECUTE << 1)
#define TXM_MODULE_MANAGER_ATTRIBUTE_WRITE_MPU_BIT      (TXM_MODULE_MANAGER_SHARED_ATTRIBUTE_WRITE << 1)
#define TXM_MODULE_MANAGER_ATTRIBUTE_READ_MPU_BIT       (TXM_MODULE_MANAGER_SHARED_ATTRIBUTE_READ << 1)

/* Define the port-extensions to the module manager instance structure.  */

#define TXM_MODULE_MANAGER_PORT_EXTENSION                                                       \
    ULONG               txm_module_instance_mpu_registers[TXM_MODULE_MANAGER_MPU_TABLE_SIZE];   \
    ULONG               txm_module_instance_shared_memory_address;                              \
    ULONG               txm_module_instance_shared_memory_length;


/* Define the memory fault information structure that is populated when a memory fault occurs.  */


typedef struct TXM_MODULE_MANAGER_MEMORY_FAULT_INFO_STRUCT
{
    TX_THREAD           *txm_module_manager_memory_fault_info_thread_ptr;
    VOID                *txm_module_manager_memory_fault_info_code_location;
    ULONG               txm_module_manager_memory_fault_info_mpests;
    ULONG               txm_module_manager_memory_fault_info_mpdea;
    ULONG               txm_module_manager_memory_fault_info_mhiti;
    ULONG               txm_module_manager_memory_fault_info_mhitd;
    ULONG               txm_module_manager_memory_fault_info_psw;
    ULONG               txm_module_manager_memory_fault_info_sp;
    ULONG               txm_module_manager_memory_fault_info_r1;
    ULONG               txm_module_manager_memory_fault_info_r2;
    ULONG               txm_module_manager_memory_fault_info_r3;
    ULONG               txm_module_manager_memory_fault_info_r4;
    ULONG               txm_module_manager_memory_fault_info_r5;
    ULONG               txm_module_manager_memory_fault_info_r6;
    ULONG               txm_module_manager_memory_fault_info_r7;
    ULONG               txm_module_manager_memory_fault_info_r8;
    ULONG               txm_module_manager_memory_fault_info_r9;
    ULONG               txm_module_manager_memory_fault_info_r10;
    ULONG               txm_module_manager_memory_fault_info_r11;
    ULONG               txm_module_manager_memory_fault_info_r12;
    ULONG               txm_module_manager_memory_fault_info_r13;
    ULONG               txm_module_manager_memory_fault_info_r14;
    ULONG               txm_module_manager_memory_fault_info_r15;
} TXM_MODULE_MANAGER_MEMORY_FAULT_INFO;


#define TXM_MODULE_MANAGER_FAULT_INFO                                               \
    TXM_MODULE_MANAGER_MEMORY_FAULT_INFO    _txm_module_manager_memory_fault_info;

/* Define the macro to check the stack available in dispatch.  */
//#define TXM_MODULE_MANAGER_CHECK_STACK_AVAILABLE                                    \
//    ULONG stack_available;                                                          \
//    __asm("MOV SP, %0" : "=r"(stack_available));                                    \
//    stack_available -= (ULONG)_tx_thread_current_ptr->tx_thread_stack_start;        \
//    if((stack_available < TXM_MODULE_MINIMUM_STACK_AVAILABLE) ||                    \
//       (stack_available > _tx_thread_current_ptr->tx_thread_stack_size))            \
//    {                                                                               \
//        return(TX_SIZE_ERROR);                                                      \
//    }

/* Define the macro to check the module version ID.  */

#define TXM_MODULE_MANAGER_CHECK_PROPERTIES(properties)                             \
        if ((properties & _txm_module_manager_properties_supported) != (properties & ~((ULONG) TXM_MODULE_COMPILER_MASK)))  \
        {                                                                           \
            _tx_mutex_put(&_txm_module_manager_mutex);                              \
            return(TXM_MODULE_INVALID_PROPERTIES);                                  \
        }                                                                           \
        if ((_txm_module_manager_properties_required & properties) != _txm_module_manager_properties_required) \
        {                                                                           \
            _tx_mutex_put(&_txm_module_manager_mutex);                              \
            return(TXM_MODULE_INVALID_PROPERTIES);                                  \
        }                                                                           


/* Define the macro to check the code alignment.  */

#define TXM_MODULE_MANAGER_CHECK_CODE_ALIGNMENT(module_location, code_alignment)    \
    {                                                                               \
        ULONG   temp;                                                               \
        temp =  (ULONG) module_location;                                            \
        temp =  temp & (code_alignment - 1);                                        \
        if (temp)                                                                   \
        {                                                                           \
            _tx_mutex_put(&_txm_module_manager_mutex);                              \
            return(TXM_MODULE_ALIGNMENT_ERROR);                                     \
        }                                                                           \
    }


/* Define the macro to adjust the alignment and size for code/data areas.  */

#define TXM_MODULE_MANAGER_ALIGNMENT_ADJUST(module_preamble, code_size, code_alignment, data_size, data_alignment)   _txm_module_manager_alignment_adjust(module_preamble, &code_size, &code_alignment, &data_size, &data_alignment);


/* Define the macro to adjust the symbols in the module preamble.  */

#define TXM_MODULE_MANAGER_CALCULATE_ADJUSTMENTS(properties, shell_function_adjust, start_function_adjust, stop_function_adjust, callback_function_adjust) \
    if ((properties & TXM_MODULE_COMPILER_MASK) == TXM_MODULE_IAR_COMPILER)         \
    {                                                                               \
        shell_function_adjust =     TXM_MODULE_IAR_SHELL_ADJUST;                    \
        start_function_adjust =     TXM_MODULE_IAR_START_ADJUST -     (ULONG)(module_instance -> txm_module_instance_code_start);   \
        stop_function_adjust =      TXM_MODULE_IAR_STOP_ADJUST -      (ULONG)(module_instance -> txm_module_instance_code_start);   \
        callback_function_adjust =  TXM_MODULE_IAR_CALLBACK_ADJUST -  (ULONG)(module_instance -> txm_module_instance_code_start);   \
    }                                                                               \
    else if ((properties & TXM_MODULE_COMPILER_MASK) == TXM_MODULE_RVDS_COMPILER)   \
    {                                                                               \
        shell_function_adjust =     TXM_MODULE_RVDS_SHELL_ADJUST;                   \
        start_function_adjust =     TXM_MODULE_RVDS_START_ADJUST;                   \
        stop_function_adjust =      TXM_MODULE_RVDS_STOP_ADJUST;                    \
        callback_function_adjust =  TXM_MODULE_RVDS_CALLBACK_ADJUST;                \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        shell_function_adjust =     TXM_MODULE_GNU_SHELL_ADJUST;                    \
        start_function_adjust =     TXM_MODULE_GNU_START_ADJUST;                    \
        stop_function_adjust =      TXM_MODULE_GNU_STOP_ADJUST;                     \
        callback_function_adjust =  TXM_MODULE_GNU_CALLBACK_ADJUST;                 \
    }


/* Define the macro to populate the thread control block with module port-specific information.
   Check if the module is in user mode and set up txm_module_thread_entry_info_kernel_call_dispatcher accordingly.
*/
#define TXM_MODULE_MANAGER_THREAD_SETUP(thread_ptr, module_instance)                                                                    \
    thread_ptr -> tx_thread_module_current_user_mode =  module_instance -> txm_module_instance_property_flags & TXM_MODULE_USER_MODE;   \
    thread_ptr -> tx_thread_module_user_mode =          module_instance -> txm_module_instance_property_flags & TXM_MODULE_USER_MODE;   \
    if (thread_ptr -> tx_thread_module_user_mode)                                                                                       \
    {                                                                                                                                   \
        *(ULONG*)&(thread_entry_info -> txm_module_thread_entry_info_kernel_call_dispatcher) =                                          \
        (ULONG)_txm_module_manager_user_mode_entry - (ULONG)module_instance -> txm_module_instance_code_start;                          \
    }                                                                                                                                   \
    else                                                                                                                                \
    {                                                                                                                                   \
        *(ULONG*)&(thread_entry_info -> txm_module_thread_entry_info_kernel_call_dispatcher) =                                          \
            (ULONG)_txm_module_manager_kernel_dispatch - (ULONG)module_instance -> txm_module_instance_code_start;                      \
    }

/* Define the macro to populate the module control block with module port-specific information.
   If memory protection is enabled, set up the MPU registers.
*/
#define TXM_MODULE_MANAGER_MODULE_SETUP(module_instance)                                                                        \
    if (module_instance -> txm_module_instance_property_flags & TXM_MODULE_USER_MODE)                                           \
    {                                                                                                                           \
        if (module_instance -> txm_module_instance_property_flags & TXM_MODULE_MEMORY_PROTECTION)                               \
        {                                                                                                                       \
            _txm_module_manager_setup_mpu_registers(module_instance);                                                           \
        }                                                                                                                       \
    }                                                                                                                           \
    else                                                                                                                        \
    {                                                                                                                           \
        /* Do nothing.  */                                                                                                      \
    }

/* Define the macro to perform port-specific functions when unloading the module.  */
/* Nothing needs to be done for this port.  */
#define TXM_MODULE_MANAGER_MODULE_UNLOAD(module_instance)


/* Define the macro to perform port-specific functions when passing pointer to kernel.  */
/* Determine if the pointer is within the module's data or shared memory.  */
//#define TXM_MODULE_MANAGER_CHECK_DATA_POINTER(module_instance, pointer)                             \
//    if ((pointer < (ULONG) module_instance -> txm_module_instance_data_start) ||                    \
//        ((pointer+sizeof(pointer)) > (ULONG) module_instance -> txm_module_instance_data_end))      \
//    {                                                                                               \
//        if((pointer < module_instance -> txm_module_instance_shared_memory_address) ||              \
//          ((pointer+sizeof(pointer)) > module_instance -> txm_module_instance_shared_memory_address \
//            + module_instance -> txm_module_instance_shared_memory_length))                         \
//        {                                                                                           \
//            return(TXM_MODULE_INVALID_MEMORY);                                                      \
//        }                                                                                           \
//    }

/* Define the macro to perform port-specific functions when passing function pointer to kernel.  */
/* Determine if the pointer is within the module's code memory.  */
//#define TXM_MODULE_MANAGER_CHECK_FUNCTION_POINTER(module_instance, pointer)                         \
//    if (((pointer < sizeof(TXM_MODULE_PREAMBLE)) ||                                                 \
//        ((pointer+sizeof(pointer)) > ((ULONG) module_instance -> txm_module_instance_code_end -     \
//                                      (ULONG) module_instance -> txm_module_instance_code_start)))  \
//       && (pointer != (ULONG) TX_NULL))                                                             \
//    {                                                                                               \
//        return(TX_PTR_ERROR);                                                                       \
//    }

/* Define macro to make sure object is inside the module's data.  */
#define TXM_MODULE_MANAGER_CHECK_INSIDE_DATA(module_instance, obj_ptr, obj_size) \
    /* Check for overflow. */   \
    (((obj_ptr) < ((obj_ptr) + (obj_size))) && \
    /* Check if it's inside module data.  */ \
     ((((obj_ptr) >= (ALIGN_TYPE) module_instance -> txm_module_instance_data_start) && \
     (((obj_ptr) + (obj_size)) <= ((ALIGN_TYPE) module_instance -> txm_module_instance_data_end + 1))) || \
    /* Check if it's inside shared memory.  */ \
     (((obj_ptr) >= (ALIGN_TYPE) module_instance -> txm_module_instance_shared_memory_address) && \
     (((obj_ptr) + (obj_size)) <= (ALIGN_TYPE) (module_instance -> txm_module_instance_shared_memory_address + module_instance -> txm_module_instance_shared_memory_length)))))


/* Define some internal prototypes to this module port.  */

#ifndef TX_SOURCE_CODE
#define txm_module_manager_memory_fault_notify              _txm_module_manager_memory_fault_notify
#endif


#define TXM_MODULE_MANAGER_ADDITIONAL_PROTOTYPES                                                                                \
VOID  _txm_module_manager_alignment_adjust(TXM_MODULE_PREAMBLE *module_preamble, ULONG *code_size, ULONG *code_alignment, ULONG *data_size, ULONG *data_alignment);   \
VOID  _txm_module_manager_memory_fault_handler(VOID);                                                                           \
UINT  _txm_module_manager_memory_fault_notify(VOID (*notify_function)(TX_THREAD *, TXM_MODULE_INSTANCE *));                     \
VOID  _txm_module_manager_setup_mpu_registers(TXM_MODULE_INSTANCE *module_instance);


#define TXM_MODULE_MANAGER_VERSION_ID   \
CHAR                            _txm_module_manager_version_id[] =  \
                                    "Copyright (c) Microsoft Corporation. All rights reserved.  *  ThreadX Module RXv2/IAR Version 6.2.1 *";

#endif
