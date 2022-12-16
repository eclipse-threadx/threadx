/*
 * interAptiv/start.S
 *
 *  Created on: Jan 12, 2011
 *  Author: MIPS TECHNOLOGIES, INC
 *  Start of boot code for interAptiv Family of Cores
*/
/*
Unpublished work (c) MIPS Technologies, Inc.  All rights reserved.  Unpublished rights reserved
under the copyright laws of the United States of America and other countries.

This code is confidential and proprietary to MIPS Technologies, Inc. ("MIPS Technologies") and
may be disclosed only as permitted in writing by MIPS Technologies or an authorized third party.
Any copying, reproducing, modifying, use or disclosure of this code (in whole or in part) that is
not expressly permitted in writing by MIPS Technologies or an authorized third party is strictly
prohibited. At a minimum, this code is protected under trade secret, unfair competition, and
copyright laws. Violations thereof may result in criminal penalties and fines.

MIPS Technologies reserves the right to change this code to improve function, design or
otherwise. MIPS Technologies does not assume any liability arising out of the application or use
of this code, or of any error or omission in such code.  Any warranties, whether express, statutory,
implied or otherwise, including but not limited to the implied warranties of merchantability or
fitness for a particular purpose, are excluded.  Except as expressly provided in any written license
agreement from MIPS Technologies or an authorized third party, the furnishing of this code does
not give recipient any license to any intellectual property rights, including any patent rights, that
cover this code.

This code shall not be exported, reexported, transferred, or released, directly or indirectly, in
violation of the law of any country or international law, regulation, treaty, Executive Order,
statute, amendments or supplements thereto.  Should a conflict arise regarding the export,
reexport, transfer, or release of this code, the laws of the United States of America shall be
the governing law.

This code may only be disclosed to the United States government ("Government"), or to
Government users, with prior written consent from MIPS Technologies or an authorized third
party.  This code constitutes one or more of the following: commercial computer software,
commercial computer software documentation or other commercial items.  If the user of this
code, or any related documentation of any kind, including related technical data or manuals, is an
agency, department, or other entity of the Government, the use, duplication, reproduction, release,
modification, disclosure, or transfer of this code, or any related documentation of any kind, is
restricted in accordance with Federal Acquisition Regulation 12.212 for civilian agencies and
Defense Federal Acquisition Regulation Supplement 227.7202 for military agencies.  The use of
this code by the Government is further restricted in accordance with the terms of the license
agreement(s) and/or applicable contract terms and conditions covering this code from MIPS
Technologies or an authorized third party.
*/

#include <boot.h>
#include <m32c0.h>
#include <regdef.h>
#include <cps.h>

/* EL Change.  Add address for optional user-specified vpe wakeup function.  */

#ifdef TX_THREAD_SMP_WAKEUP_LOGIC
    .globl      TX_THREAD_SMP_WAKEUP_LOGIC_SETUP
#endif

/* EL Change End.  */


	.set	noreorder           // Don't allow the assembler to reorder instructions.
	.set	noat                // Don't allow the assembler to use r1(at) for synthetic instr.
/**************************************************************************************
    R E S E T   E X C E P T I O N   H A N D L E R
**************************************************************************************/
// RAMHACK: Removed boot exception handlers.
/**************************************************************************************
    B O O T   E X C E P T I O N   H A N D L E R S
**************************************************************************************/

// RAMHACK: Removed boot exception handlers.


/**************************************************************************************
    R A M   E X C E P T I O N   H A N D L E R S
**************************************************************************************/

.section ".vector_0x000","ax"
    .globl tlb_refill_exception
tlb_refill_exception:    
    sdbbp

.section ".vector_0x100","ax"
    .globl cache_error_exception
cache_error_exception:    
    sdbbp

.section ".vector_0x180","ax"
    .globl general_exception
general_exception:    

/* EL Change: Branch to ThreadX error handling.  */

    la       k0, _tx_error_exceptions
    jalr     k1, k0
    nop

/* EL Change end.  */


    sdbbp

.section ".vector_0x200","ax"
    .globl iv1_interrupt
iv1_interrupt:

/* EL Change: Branch to ThreadX error handling.  */

    la      k0, _tx_exception_handler
    jalr    k1, k0
    nop

/* Disable previous exception handler, since ThreadX is taking care of all exceptions now.  */
#if 0
    li      k0, MALTA_DISP_ADDR			// Malta ASCII character display.
    mfc0    k1, C0_EBASE				// Get cp0 EBase
    ext     k1, k1, 0, 10				// Extract CPUNum
    sll     k1, k1, 3					// Build offset for display char.
    addiu   k1, 8
    addu    k0, k0, k1					// Pointer to a single display character.
    li      k1, 'H'						// Indicate that this vpe is in the handler.
    sw      k1, 0(k0)					// Write ASCII char to Malta ASCII display.

    li      k0, (GIC_SH_WEDGE | GIC_BASE_ADDR)
    mfc0    k1, C0_EBASE                // Get cp0 EBase
    ext     k1, k1, 0, 10				// Extract CPUNum
    addiu   k1, 0x20					// Offset to base of IPI interrupts.
    sw      k1, 0(k0)					// Clear this IPI.

    la      k0, start_test
    mfc0    k1, C0_EBASE				// Get cp0 EBase
    ext     k1, k1, 0, 10				// Extract CPUNum
    sll     k1, k1, 2
    addu    k0, k0, k1					// index into CMP global "C" variable start_test

    lw      k1, 0(k0)	# Pickup start_test value
    addiu   k1, k1, 1	# Increment start_test value

    sw      k1, 0(k0)					// Release "cpu"/vpe to execute "C" test code.
    eret
    nop
#endif

/* EL Change end.  */

    
.section ".vector_0x280","ax"
    .globl xtlb_refill
xtlb_refill:    
    sdbbp

.section ".vector_0x300","ax"
    .globl cache_error
cache_error:
    sdbbp


/**************************************************************************************
**************************************************************************************/
// RAMHACK: Entry point is _start (execution is started here in (uncached) kseg0.

.section ".text","ax"
               /* (with vaddr[12..0] = 0 for alignment with GCR_CO_RESET_BASE). */

	.globl _start
_start:
    b       verify_isa          # Note: Real systems might want to save/dump full context.
    nop
    .globl verify_isa
verify_isa: // Verify device ISA meets code requirements (MIPS32R2 or later.)
    mfc0    a0, C0_CONFIG				// Read CP0 Config
    srl     a0, 10					// Shift [AT AR] into LSBs.
    andi    a3, a0, 0x18				// Inspect CP0 Config[AT]
    beqz    a3, is_mips32				// Branch if executing on MIPS32 ISA.
    andi    a3, a0, 0x07				// Inspect CP0 Config[AR]
    sdbbp 								// Failed assertion: MIPS32R2.
    .globl is_mips32
is_mips32:
    bnez    a3, init_common_resources	// Continue if ISA is MIPS32R2 or later.
    nop
    sdbbp								// Failed assertion MIPS32R2.
/**************************************************************************************
 What is initialized on execution depends on the core/vpe executing it.
 (A non-MT device is treated as tc0/vpe0, non-CMP device is treated as core0.)
**************************************************************************************/
    .globl init_common_resources
init_common_resources:  // initializes resources for virtual or physical "cpu".
    la a2,init_gpr		// Fill register file with set  value.
    jalr a2
    nop
    la a2,	set_gpr_boot_values  // Fill register file boot info.
    jalr a2
    nop

    // Clear Malta Display if processor 0
    bnez    r9_vpe_num, clear_done
    lui v0, 0xbf00
    li v1, 0x20
    sw v1, 1048(v0)
    sw v1, 1056(v0)
    sw v1, 1064(v0)
    sw v1, 1072(v0)
    sw v1, 1080(v0)
    sw v1, 1088(v0)
    sw v1, 1096(v0)
    sw v1, 1104(v0)
    .globl clear_done    
clear_done:

    la a2,     init_cp0    // Init CP0 Status, Count, Compare, Watch*, and Cause.
    jalr a2
    nop

 	// Determine if we have a TLB
	mfc0    v1, C0_CONFIG			// read C0_Config
	ext	    v1, v1, 7, 3			// extract MT field
	li	    a3, 0x1					// load a 1 to check against
	bne	    v1, a3, done_tlb		// no tlb?
	nop

	// determined if this is VPE 0 so skip shared TLB check
    beqz    r9_vpe_num, do_tlb		// VPE 0?
    nop

    // Must be VPE1 determined if TLB is shared if it is skip init_tlb (already done by VPE0)
    mfc0    a0, $0, 1				// MVPControl
    ext     a0, a0, 3, 1    		// MVPControl[STLB]
    beq		a0, zero, done_tlb		// TLB shared?
	nop

do_tlb:
    la a2,     init_tlb    			// Generate unique EntryHi contents per entry pair.
    jalr a2
    nop
    .globl done_tlb
done_tlb:
    la a2,     init_gic    // Configure the global interrupt controller.
    jalr a2
    nop

    bnez    r9_vpe_num, init_done // If we are not a vpe0 then we are done.
    nop
    .globl init_core_resources
init_core_resources:    // We are a vpe0.

    la a2,     disable_L23   // Disable L2/L3 caches
    jalr a2
    nop

    la a2,     init_icache // Initialize the L1 instruction cache. (Executing using I$ on return.)
    jalr a2
    nop

// The changing of Kernel mode cacheability must be done from KSEG1
// Since the code is executing from KSEG0 It needs to do a jump to KSEG1 change K0 and jump back to KSEG0
	la a2,		change_k0_cca
	li      	a1, 0xf
	ins     	a2, a1, 29, 1 // changed to KSEG1 address by setting bit 29
    jalr a2
    nop

// L1 Cache icache is now enabled

    la a2,     init_dcache // Initialize the L1 data cache
    jalr a2
    nop

    la a2,     init_itc    // Initialize Inter-Thread Communications unit
    jalr a2
    nop

    bnez    r8_core_num, init_sys_resources_done // Only core0/vpe0 needs to init systems resources.
    nop

    .globl init_sys_resources
init_sys_resources:     // We are core0 vpe0.
    la a2,     init_cpc    // Initialize the CPS CPC (Cluster Power Controller.)
    jalr a2
    nop

    la a2,     init_cm     // Initialize the CPS CM (Coherence Manager.)
    jalr a2
    nop

#ifdef DENALI
    la a2,     init_CoreFPGA6_mem     // Initialize the ROC-it2 MC (Memory Controller.)
    jalr a2
    nop
#endif

    la a2,     copy_c2_ram // Copy "C" code and data to RAM and zero bss (uncached.)
    jalr a2
    nop

    la a2,     init_L23   // Initialize the unified L2 and L3 caches
    jalr a2
    nop

    /* EL Change.  Ensure that the VPE release flag is cleared ahead of BSS clear.  */
    
    la      $8, _tx_thread_smp_release_cores_flag   # Build address of release flag
    sw      $0, 0($8)                               # Clear the flag explicity to make other VPEs don't see it before everything is initialized
    
    /* EL end Change.  */


    la a2,     release_mp  // Release other cores to execute this boot code.
    jalr a2
    nop

init_sys_resources_done:		// All Cores (VPE0)

    la a2,     join_domain // Join the Coherence  domain. (OK to use D$ on return.)
    jalr a2
    nop

    la a2,     init_vpe1   // Set up MT ASE vpe1 to execute this boot code also.
    jalr a2
    nop

    /* EL Change.  Add optional user-specified VPE wakeup setup code call.  */

#ifdef TX_THREAD_SMP_WAKEUP_LOGIC

    /* Call optional user-specific code for VPE initialization.  */

    subu    $29, 8                              # Allocate space to save ra
    sw      $31, 4($29)                         # Save R31
    la      $9, TX_THREAD_SMP_WAKEUP_LOGIC_SETUP
    jalr    $9                                  # User-specified VPE setup logic
    nop                                         #
    lw      $31, 4($29)                         # Restore ra
    addu    $29, 8                              # De-allocate stack storage
#endif

    /* EL Change End.  */

    .globl init_done
init_done:

    /* EL Change.  Removed call to demo code and instead setup for ThreadX execution.   */

    /* For all VPEs, store the cpu number into the UserLocal field so it is easy to pickup.  */

    mtc0    r23_cpu_num, $4,2                   # Save the logical VPE in UserLocal so we don't have calculate it over and over!  */

    /* Save the stack pointer in the array indexed by cpu number.  */
    
    la      $8, _tx_thread_system_stack_ptr     # Build address of base of system stack array
    sll     $9, r23_cpu_num, 2                  # Build offset into array
    addu    $8, $8, $9                          # Build address of array entry
    sw      $29, 0($8)                          # Store system stack for each VPE

    /* Setup status register in preparation for entering ThreadX.  */
    
    mfc0    $8, $12                             # Pickup SR
    li      $9, 0xFFFFFFF8                      # Build mask to clear error, exception bits
    and     $8, $8, $9                          # Clear bits
    ori     $8, $8, 1                           # Disable all interrupts
    mtc0    $8, $12                             # Setup the status register
    ehb                                         #

    /* Check for Core 0, VPE 0 for processing ThreadX initialization.  All other VPEs will wait until the first
       VPE has completed initialization before running.  */
    
    bne     r23_cpu_num, $0, _additional_vpe    # If non-zero, an additional vpe is present
    nop                                         # 
    
    /* Core 0, VPE 0 processing.  */
    
    /* Save some information in globals.  */
    
    la      $8, _tx_thread_smp_detected_cores   # Build address of total number of cores detected
    addiu   $9, r19_more_cores, 1               # Calculate the total cores
    sw      $9, 0($8)                           # Save in global variable
    
    la      $8, _tx_thread_smp_detected_vpes_per_core # Build address of VPEs per core detected
    addiu   $9, r20_more_vpes, 1                # Caculate the total vpes per core
    sw      $9, 0($8)                           # Save in global variable

  
    /* Simply branch to main to finish initializing ThreadX SMP.  */
    
    bal     main                                # Branch to main
    nop                                         # 
    
    /* If return, branch to all_done code.  */
    
    b      all_done
    nop 
    
        
_additional_vpe:

   
    /* Additional VPE, transfer control to ThreadX.  */
    
    b       _tx_thread_smp_initialize_wait          # Enter ThreadX for additional VPEs
    nop

    .globl all_done
all_done:
    b       all_done
    
/* Comment out the previous "init_done" code since it is replaced with ThreadX-specfic code.  */
#if 0

    mtc0    r23_cpu_num, $4,2              # Setup UserLocal

    // Prepare for eret to main (sp and gp set up per vpe in init_gpr).
    la      ra, all_done	// If main return then go to all_done:.
    la      a1, main
    mtc0    a1, C0_ERRPC 			// ErrorEPC

    // initialize global variable num_cores.
//    la      a1, num_cores
    // RAMHACK: caches are coherent use cached reference to "C" static "num_cores".
    // ins     a1, r1_all_ones, 29, 1 // Uncached kseg1
//    add     a0, r19_more_cores, 1
//    sw      a0, 0(a1)

    // Prepare arguments for main()
    move    a0, r23_cpu_num			// main(arg0) is the "cpu" number (cp0 EBase[CPUNUM].)
    move    a1, r8_core_num			// main(arg1) is the core number.
    move    a2, r9_vpe_num			// main(arg2) is the vpe number.
    addiu   a3, r20_more_vpes, 1	// main(arg3) is the number of vpe on this core.

    // RAMHACK: Main appears to take first arg from r16 (s0) in GHS build?
    move    s0, r23_cpu_num			// main(arg0) is the "cpu" number (cp0 EBase[CPUNUM].)

    eret    // Exit reset exception handler for this vpe and start execution of main().


/**************************************************************************************
**************************************************************************************/
    .globl all_done
all_done:

    // Main returns the "cpu" number
    // All cpu spin on atomic potato++

//    la a1, potato
    
try_again:
    ll a0, 0(a1)
    addiu a0, a0, 1
    sc a0, 0(a1)      // Attempt atomic potato++

    beq a0, $0, try_again       // Fail, retry.
    nop

    b try_again                           // Success, do again.
    nop
 
    b       all_done
    nop
#endif
    /* EL Change end.  */


