//----------------------------------------------------------------
// Cortex-R4(F) Embedded example - Startup Code
//
// Copyright (c) 2006-2018 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
//----------------------------------------------------------------


#define FIQ_MODE        0x11
#define IRQ_MODE        0x12
#define SVC_MODE        0x13
#define ABT_MODE        0x17
#define SYS_MODE        0x1F

//----------------------------------------------------------------

    .eabi_attribute Tag_ABI_align8_preserved,1

    .section  VECTORS,"ax"
    .align 3
    .cfi_sections .debug_frame  // put stack frame info into .debug_frame instead of .eh_frame

//----------------------------------------------------------------
// Exception Vector Table
//----------------------------------------------------------------
// Note: LDR PC instructions are used here, though branch (B) instructions
// could also be used, unless the exception handlers are >32MB away.

    .global Vectors

Vectors:
    LDR PC, Reset_Addr
    LDR pc,=__tx_undefined                  // Undefined handler
    LDR pc,=__tx_svc_interrupt              // Software interrupt handler
    LDR pc,=__tx_prefetch_handler           // Prefetch exception handler
    LDR pc,=__tx_abort_handler              // Abort exception handler
    LDR pc,=__tx_reserved_handler           // Reserved exception handler
    LDR pc,=__tx_irq_handler                // IRQ interrupt handler
    LDR pc,=__tx_fiq_handler                // FIQ interrupt handler


    .balign 4
Reset_Addr:     .word Reset_Handler
Undefined_Addr: .word __tx_undefined
SVC_Addr:       .word __tx_svc_interrupt
Prefetch_Addr:  .word __tx_prefetch_handler
Abort_Addr:     .word __tx_abort_handler
IRQ_Addr:       .word __tx_irq_handler
FIQ_Addr:       .word __tx_fiq_handler


//----------------------------------------------------------------
// Reset Handler
//----------------------------------------------------------------

    .global Reset_Handler
    .type Reset_Handler, "function"
Reset_Handler:

//----------------------------------------------------------------
// Disable MPU and caches
//----------------------------------------------------------------

// Disable MPU and cache in case it was left enabled from an earlier run
// This does not need to be done from a cold reset

        MRC     p15, 0, r0, c1, c0, 0       // Read System Control Register
        BIC     r0, r0, #(0x1 << 12)        // Clear I bit 12 to disable I Cache
        BIC     r0, r0, #(0x1 <<  2)        // Clear C bit  2 to disable D Cache
        BIC     r0, r0, #0x1                // Clear M bit  0 to disable MPU
        DSB                                 // Ensure all previous loads/stores have completed
        MCR     p15, 0, r0, c1, c0, 0       // Write System Control Register
        ISB                                 // Ensure subsequent insts execute wrt new MPU settings

//----------------------------------------------------------------
// Disable Branch prediction
//----------------------------------------------------------------

// In the Cortex-R4, the Z-bit of the SCTLR does not control the program flow prediction.
// Some control bits in the ACTLR control the program flow and prefetch features instead.
// These are enabled by default, but are shown here for completeness.

        MRC     p15, 0, r0, c1, c0, 1       // Read ACTLR
        ORR     r0, r0, #(0x1 << 17)        // Enable RSDIS bit 17 to disable the return stack
        ORR     r0, r0, #(0x1 << 16)        // Clear BP bit 15 and set BP bit 16:
        BIC     r0, r0, #(0x1 << 15)        // Branch always not taken and history table updates disabled
        MCR     p15, 0, r0, c1, c0, 1       // Write ACTLR
        ISB

//----------------------------------------------------------------
// Cache invalidation
//----------------------------------------------------------------

        DSB                 // Complete all outstanding explicit memory operations

        MOV     r0, #0

        MCR     p15, 0, r0, c7, c5, 0       // Invalidate entire instruction cache
        MCR     p15, 0, r0, c15, c5, 0      // Invalidate entire data cache


//----------------------------------------------------------------
// Initialize Supervisor Mode Stack using Linker symbol from scatter file.
// Stacks must be 8 byte aligned.
//----------------------------------------------------------------

    /****** NOTE ****** We must be in SVC MODE at this point.  Some monitors
       enter this routine in USER mode and require a software interrupt to
       change into SVC mode.  */

#ifdef TX_ENABLE_IRQ_NESTING
    /* Setup the system mode stack for nested interrupt support  */
    MOV     r3, #SYS_MODE                       // Build SYS mode CPSR
    MSR     CPSR_c, r3                          // Enter SYS mode
    LDR     sp, =Image$$SYS_STACK$$ZI$$Limit    // Setup SYS stack pointer
#endif

	CPS     #ABT_MODE                           // Build Abort mode CPSR
    LDR     sp, =Image$$ABORT_STACK$$ZI$$Limit  // Setup abort stack pointer

    CPS		#FIQ_MODE                       	// Build FIQ mode CPSR
    LDR     sp, =Image$$FIQ_STACK$$ZI$$Limit        // Setup FIQ stack pointer
    MOV     sl, #0                              // Clear sl
    MOV     fp, #0                              // Clear fp

    CPS 	#IRQ_MODE                       	// Build IRQ mode CPSR
    LDR     sp, =Image$$IRQ_STACK$$ZI$$Limit    // Setup IRQ stack pointer

    CPS    	#SVC_MODE                       	// Build SVC mode CPSR
    LDR     sp, =Image$$SVC_STACK$$ZI$$Limit    // Setup SVC stack pointer
    

//----------------------------------------------------------------
// TCM Configuration
//----------------------------------------------------------------

// Cortex-R4 optionally provides two Tightly-Coupled Memory (TCM) blocks (ATCM and BTCM) for fast access to code or data.
// ATCM typically holds interrupt or exception code that must be accessed at high speed,
//    without any potential delay resulting from a cache miss.
// BTCM typically holds a block of data for intensive processing, such as audio or video data.
// In the Cortex-R4 processor, both ATCM and BTCM support both instruction and data accesses.

// The following illustrates basic TCM configuration, as the basis for exploration by the user

#ifdef TCM
    .global  Image$$ATCM$$Base
    .global  Image$$BTCM0$$Base
    .global  Image$$BTCM1$$Base

        MRC     p15, 0, r0, c0, c0, 2       // Read TCM Type Register
        // r0 now contains ATCM & BTCM availability

        MRC     p15, 0, r0, c9, c1, 1       // Read ATCM Region Register
        // r0 now contains ATCM size in bits [6:2]

        MRC     p15, 0, r0, c9, c1, 0       // Read BTCM Region Register
        // r0 now contains BTCM size in bits [6:2]

// The Cortex-R4F Core Tile has
// 64K ATCM  from 0xE0FD0000 to 0xE0FDFFFF
// 64K BTCM0 from 0xE0FE0000 to 0xE0FEFFFF
// 64K BTCM1 from 0xE0FF0000 to 0xE0FFFFFF

        LDR     r0, =Image$$ATCM$$Base      // Set ATCM base address
        ORR     r0, r0, #1                  // Enable it
        MCR     p15, 0, r0, c9, c1, 1       // Write ATCM Region Register

        LDR     r0, =Image$$BTCM0$$Base     // Set BTCM base address
        ORR     r0, r0, #1                  // Enable it
        MCR     p15, 0, r0, c9, c1, 0       // Write BTCM Region Register

#endif

#ifdef __ARM_FP
//----------------------------------------------------------------
// Enable access to VFP by enabling access to Coprocessors 10 and 11.
// Enables Full Access i.e. in both privileged and non privileged modes
//----------------------------------------------------------------

        MRC     p15, 0, r0, c1, c0, 2      // Read Coprocessor Access Control Register (CPACR)
        ORR     r0, r0, #(0xF << 20)       // Enable access to CP 10 & 11
        MCR     p15, 0, r0, c1, c0, 2      // Write Coprocessor Access Control Register (CPACR)
        ISB

//----------------------------------------------------------------
// Switch on the VFP hardware
//----------------------------------------------------------------

        MOV     r0, #0x40000000
        VMSR    FPEXC, r0                   // Write FPEXC register, EN bit set
#endif

//----------------------------------------------------------------
// Enable Branch prediction
//----------------------------------------------------------------

// In the Cortex-R4, the Z-bit of the SCTLR does not control the program flow prediction. 
// Some control bits in the ACTLR control the program flow and prefetch features instead.
// These are enabled by default, but are shown here for completeness.

        MRC     p15, 0, r0, c1, c0, 1       // Read ACTLR
        BIC     r0, r0, #(0x1 << 17)        // Clear RSDIS bit 17 to enable return stack
        BIC     r0, r0, #(0x1 << 16)        // Clear BP bit 15 and BP bit 16:
        BIC     r0, r0, #(0x1 << 15)        // Normal operation, BP is taken from the global history table.
        MCR     p15, 0, r0, c1, c0, 1       // Write ACTLR
        ISB


        /* Enable the GIC.  */
        BL      GIC_Enable

    .global     __main
        B       __main

    .size Reset_Handler, . - Reset_Handler

//----------------------------------------------------------------
// Global Enable for Instruction and Data Caching
//----------------------------------------------------------------

    .global enable_caches

    .type enable_caches, "function"
    .cfi_startproc
enable_caches:

        MRC     p15, 0, r0, c1, c0, 0       // Read System Control Register
        ORR     r0, r0, #(0x1 << 12)        // enable I Cache
        ORR     r0, r0, #(0x1 << 2)         // enable D Cache
        MCR     p15, 0, r0, c1, c0, 0       // Write System Control Register
        ISB

        BX    lr
    .cfi_endproc

    .size enable_caches, . - enable_caches

/* Define initial heap/stack routine for the ARM RealView (and ADS) startup code.  This
    routine will set the initial stack to use the ThreadX IRQ & FIQ &
    (optionally SYS) stack areas.  */

    .global __user_initial_stackheap
    .type   __user_initial_stackheap, %function
__user_initial_stackheap:

    LDR     r1, =Image$$SVC_STACK$$ZI$$Limit
    BX      lr                                  // Return to caller
