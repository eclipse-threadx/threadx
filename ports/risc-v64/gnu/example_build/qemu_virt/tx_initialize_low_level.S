/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
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
/*    trap_entry                           		     RISC-V64/GNU */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Jer6y , luojun@oerv.isrc.iscas.ac.cn    			          */
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
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-25-2024      Jerry Luo                                             */
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
	addi sp, sp, -65*REGBYTES         // Allocate space for all registers - with floating point enabled
#else
	addi sp, sp, -32*REGBYTES         // Allocate space for all registers - without floating point enabled
#endif

	STORE   x1, 28*REGBYTES(sp)       // Store RA, 28*REGBYTES(because call will override ra [ra is a calle register in riscv])

    	call    _tx_thread_context_save    	

	csrr a0, mcause
	csrr a1, mepc
        csrr a2, mtval
	addi    sp, sp, -8
    	sd      ra, 0(sp)
    	call    trap_handler
    	ld      ra, 0(sp)
    	addi    sp, sp, 8
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
    .weak    _tx_initialize_low_level
    .extern  _end
    .extern  board_init
_tx_initialize_low_level:
    sd      sp, _tx_thread_system_stack_ptr, t0     // Save system stack pointer

    la      t0, _end            		    // Pickup first free address
    sd      t0, _tx_initialize_unused_memory, t1    // Save unused memory address
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
    addi    sp, sp, -8
    sd	    ra, 0(sp)
    call    board_init
    ld	    ra, 0(sp)
    addi    sp, sp, 8
    la	    t0, trap_entry
    csrw    mtvec, t0
    ret
