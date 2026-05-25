/***************************************************************************
 * Copyright (c) 2026 Quintauris
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include "csr.h"
#include "tx_port.h"

 .section .text
 .align 4
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    trap_entry                           		     RISC-V32/GNU */
/*                                                           6.4.x        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Francisco Merino, Quintauris   		                          */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for riscv processor trap handle        */
/*    It will do the contex save and call c trap_handler and do contex    */
/*    load 								  */
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
/*    trap_handler                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    hardware exception                                                  */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Initialize                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
    .global  trap_entry
    .extern  trap_handler
    .extern  _tx_thread_context_restore
    trap_entry:
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
	addi sp, sp, -65*4         // Allocate space for all registers - with floating point enabled
#else
	addi sp, sp, -32*4         // Allocate space for all registers - without floating point enabled
#endif

	sw   x1, 28*4(sp)       // Store RA, 28*4(because call will override ra [ra is a calle register in riscv])

    	call    _tx_thread_context_save    	

	csrr a0, mcause
	csrr a1, mepc
        csrr a2, mtval
	addi    sp, sp, -4
    	sw      ra, 0(sp)
    	call    trap_handler
    	lw      ra, 0(sp)
    	addi    sp, sp, 4
	call _tx_thread_context_restore 
	// it will nerver return
_err:
	wfi
	j _err  
    .section .text
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_initialize_low_level                           RISC-V32/GNU     */
/*                                                           6.4.x        */
/*  AUTHOR                                                                */
/*                                                                        */
/*  Francisco Merino, Quintauris      	     	                          */
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
/*  11-03-2026    Francisco Merino     Adapted for RV32 from RV64 port    */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_initialize_low_level(VOID)
{  */
    .global  _tx_initialize_low_level
    .weak    _tx_initialize_low_level
    .extern  _end
    .extern  board_init
_tx_initialize_low_level:

    .section .text

    la      t0, _tx_thread_system_stack_ptr
    sw      sp, 0(t0)                               // Save system stack pointer

    la      t0, _end                                // Pickup first free address
    la      t1, _tx_initialize_unused_memory
    sw      t0, 0(t1)                               // Save unused memory address
    li      t0, MSTATUS_MIE
    csrrc   zero, mstatus, t0			    // clear MSTATUS_MIE bit
    li 	    t0, (MSTATUS_MPP_M | MSTATUS_MPIE )
    csrrs   zero, mstatus, t0			    // set MSTATUS_MPP, MPIE bit
    li	    t0, (MIE_MTIE | MIE_MSIE | MIE_MEIE)
    csrrs   zero, mie, t0			    // set mie
#ifdef __riscv_flen
    li      t0, MSTATUS_FS
    csrrs   zero, mstatus, t0			    // set MSTATUS_FS bit to open f/d isa in riscv
    fscsr   x0
#endif
    addi    sp, sp, -4
    sw	    ra, 0(sp)
    call    board_init
    lw	    ra, 0(sp)
    addi    sp, sp, 4
    la	    t0, trap_entry
    csrw    mtvec, t0
    ret
