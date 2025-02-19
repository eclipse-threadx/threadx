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
/**   Initialize                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#include "tx_port.h"
.global __RAM_segment_used_end__ 
.global tx_port_setup_timer


    .section .text
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_initialize_low_level                           RISC-V64/GNU     */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for any low-level processor            */
/*    initialization, including setting up interrupt vectors, setting     */
/*    up a periodic timer interrupt source, saving the system stack       */
/*    pointer for use in ISR processing later, and finding the first      */
/*    available RAM memory address for tx_application_define.             */
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
/*    _tx_initialize_kernel_enter           ThreadX entry function        */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  03-08-2023      Scott Larson            Initial Version 6.2.1         */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_initialize_low_level(VOID)
{  */
    .global  _tx_initialize_low_level
_tx_initialize_low_level:
    sd      sp, _tx_thread_system_stack_ptr, t0     // Save system stack pointer

    la      t0, __RAM_segment_used_end__            // Pickup first free address
    addi    t0, t0, 8                               // add 8
    sd      t0, _tx_initialize_unused_memory, t1    // Save unused memory address

#ifdef __riscv_flen
    fscsr x0
#endif
    
    // set up interrupt and timer(mtime)
#if( portasmHAS_SIFIVE_CLINT != 0 )
	/* If there is a clint use a unified interrupt/trap entry */
	la t0, threadx_trap_entry
	csrw mtvec, t0
#endif

	addi sp, sp, -1*REGBYTES
	STORE ra, 1*REGBYTES(sp)                        // save ra
	call tx_port_setup_timer                        // setup timer interrupt
	LOAD ra, 1*REGBYTES(sp)
	addi sp, sp, 1*REGBYTES
    ret


    /* Define the actual timer interrupt/exception handler.  */

.global do_irq  // user irq handler, irq num can be got by PLIC
#ifdef THEAD_C906
.global pulMachineTimerCompareRegisterL
.global pulMachineTimerCompareRegisterH
#else
.global pullMachineTimerCompareRegister
#endif
.global pullNextTime
.global uxTimerIncrementsForOneTick /* size_t type so 32-bit on 32-bit core and 64-bits on 64-bit core. */
.global _tx_timer_interrupt
.global _tx_thread_context_save
.global _tx_thread_context_restore

.global threadx_trap_entry

.align 8
.func
threadx_trap_entry: 
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
	addi sp, sp, -65*REGBYTES         // Allocate space for all registers - with floating point enabled
#else
	addi sp, sp, -32*REGBYTES         // Allocate space for all registers - without floating point enabled
#endif

	STORE   x1, 28*REGBYTES(sp)       // Store RA, 28*REGBYTES(224 for riscv64)
    
    call    _tx_thread_context_save    // Call ThreadX context save

	/* exception handle part */
	
	csrr a0, mcause
	csrr a1, mepc

test_if_asynchronous:
	srli a2, a0, __riscv_xlen - 1		/* MSB of mcause is 1 if handing an asynchronous interrupt - shift to LSB to clear other bits. */
	beq a2, x0, handle_synchronous		/* Branch past interrupt handing if not asynchronous. */

	/* unmodified exception return address(mepc) was saved in tx_thread_context_save.S */

handle_asynchronous:

#if( portasmHAS_MTIME != 0 )

	test_if_mtimer:						/* If there is a CLINT then the mtimer is used to generate the tick interrupt. */

		addi t0, x0, 1  /* t0 = 1*/

		slli t0, t0, __riscv_xlen - 1   /* LSB is already set, shift into MSB.  Shift 31 on 32-bit or 63 on 64-bit cores. */
		addi t1, t0, 7					/* 0x8000[]0007 == machine timer interrupt. */
		bne a0, t1, test_if_external_interrupt

    #ifdef THEAD_C906
		    LOAD t0, pulMachineTimerCompareRegisterL  /* Load address of compare Lo register into t0. */
    #else
		    LOAD t0, pullMachineTimerCompareRegister  /* Load address of compare register into t0. */
    #endif
		
        LOAD t1, pullNextTime           /* Load the address of ullNextTime into t1. */

    #ifdef THEAD_C906
			/* Update the 64-bit mtimer compare match value in two 32-bit writes. */
			ld t2, 0(t1)			 	/* Load ullNextTime into t2. */
			sw t2, 0(t0)				/* Store ullNextTime into compare register L. */
			srli t3, t2, 32
			sw t3, 4(t0)				/* Store ullNextTime into compare register H. */
			ld t0, uxTimerIncrementsForOneTick  /* Load the value of ullTimerIncrementForOneTick into t0 (could this be optimized by storing in an array next to pullNextTime?). */
			add t4, t0, t2				/* Add ullNextTime to the timer increments for one tick. */
			sd t4, 0(t1)				/* Store ullNextTime. */
    #else
			/* Update the 64-bit mtimer compare match value. */
			ld t2, 0(t1)			 	/* Load ullNextTime into t2. */
			sd t2, 0(t0)				/* Store ullNextTime into compare register. */
			ld t0, uxTimerIncrementsForOneTick  /* Load the value of ullTimerIncrementForOneTick into t0 (could this be optimized by storing in an array next to pullNextTime?). */
			add t4, t0, t2				/* Add ullNextTime to the timer increments for one tick. */
			sd t4, 0(t1)				/* Store ullNextTime. */
    #endif

        // use _tx_thread_system_stack_ptr, switched in tx_thread_context_save.S
		jal _tx_timer_interrupt         /* threadx do not return yield flag */
        j context_restore               /* jump to _tx_thread_context_restore */

	test_if_external_interrupt:			/* If there is a CLINT and the mtimer interrupt is not pending then check to see if an external interrupt is pending. */
		addi t1, t1, 4					/* 0x80000007 + 4 = 0x8000000b == Machine external interrupt. */
		bne a0, t1, as_yet_unhandled	/* Something as yet unhandled. */

#endif /* portasmHAS_MTIME */

    // use _tx_thread_system_stack_ptr, switched in tx_thread_context_save.S
	jal do_irq			/* Jump to the interrupt handler if there is no CLINT or if there is a CLINT and it has been determined that an external interrupt is pending. */
	j context_restore                   /* jump to _tx_thread_context_restore */

handle_synchronous:
	addi a1, a1, 4						/* Synchronous so updated exception return address to the instruction after the instruction that generated the exeption. */

    /* Save updated exception return address. */
	LOAD    t0, _tx_thread_current_ptr   // Pickup current thread pointer
    beqz    t0, _tx_thread_idle_system   // If NULL, idle system was interrupted
	STORE   a1, 30*REGBYTES(sp)          // Save new mepc again 
	/* idle system do not need save mepc as it will directly jump to schedule */
_tx_thread_idle_system: 

test_if_environment_call:
	li t0, 11 							/* 11 == environment call. */
	bne a0, t0, is_exception			/* Not an M environment call, so some other exception. */
	j context_restore                   /* jump to _tx_thread_context_restore */

is_exception:
	csrr t0, mcause						/* For viewing in the debugger only. */
	csrr t1, mepc						/* For viewing in the debugger only */
	csrr t2, mstatus
		
	j is_exception						/* No other exceptions handled yet. */

as_yet_unhandled:
	csrr t0, mcause						/* For viewing in the debugger only. */
	
	j as_yet_unhandled

context_restore:
    j       _tx_thread_context_restore  /* Jump to ThreadX context restore function. Note: this does not return!*/

	.endfunc