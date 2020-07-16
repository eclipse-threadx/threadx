//==================================================================
// Copyright ARM Ltd 2012. All rights reserved.
//
// ARMv8 example - Startup Code
//==================================================================

  .section  BOOT,"ax"
  .align 3

// ------------------------------------------------------------

.equ Mode_USR, 0x10

.equ AArch32_Mode_USR,		 0x10
.equ AArch32_Mode_FIQ,		 0x11
.equ AArch32_Mode_IRQ,		 0x12
.equ AArch32_Mode_SVC,		 0x13
.equ AArch32_Mode_ABT,		 0x17
.equ AArch32_Mode_UNDEF,   0x1B
.equ AArch32_Mode_SYS,		 0x1F
.equ AArch32_Mode_HYP,     0x1A
.equ AArch32_Mode_MON,     0x16

.equ AArch64_EL2_SP2,      0x09    // EL2h
.equ AArch64_EL2_SP0,      0x08    // EL2t
.equ AArch64_EL1_SP1,      0x05    // EL1h
.equ AArch64_EL1_SP0,      0x04    // EL1t
.equ AArch64_EL0_SP0,      0x00

.equ AArch32_State_Thumb,  0x20
.equ AArch32_State_ARM,    0x00

// ------------------------------------------------------------

.equ TT_S1_TABLE,          0x00000000000000003    // NSTable=0, PXNTable=0, UXNTable=0, APTable=0

// TT block entries templates   (L1 and L2, NOT L3)
// Assuming table contents:
// 0 = b01000100 = Normal, Inner/Outer Non-Cacheable
// 1 = b11111111 = Normal, Inner/Outer WB/WA/RA
// 2 = b00000000 = Device-nGnRnE
.equ TT_S1_FAULT,           0x0
.equ TT_S1_NORMAL_NO_CACHE, 0x00000000000000401    // Index = 0, AF=1
.equ TT_S1_NORMAL_WBWA,     0x00000000000000405    // Index = 1, AF=1
.equ TT_S1_DEVICE_nGnRnE,   0x00000000000000409    // Index = 2, AF=1

.equ TT_S1_UXN,             (1 << 54)
.equ TT_S1_PXN,             (1 << 53)
.equ TT_S1_nG,              (1 << 11)
.equ TT_S1_NS,              (1 << 5)

.equ TT_S1_NON_SHARED,      (0 << 8)               // Non-shareable
.equ TT_S1_INNER_SHARED,    (3 << 8)               // Inner-shareable
.equ TT_S1_OUTER_SHARED,    (2 << 8)               // Outer-shareable

.equ TT_S1_PRIV_RW,         (0x0)
.equ TT_S1_PRIV_RO,         (0x2 << 6)
.equ TT_S1_USER_RW,         (0x1 << 6)
.equ TT_S1_USER_RO,         (0x3 << 6)

// ------------------------------------------------------------

  .global start64
  .type start64, @function
start64:

  // Clear registers
  // ---------------
  // This is primarily for RTL simulators, to avoid
  // possibility of X propergation
  MOV      x0, #0
  MOV      x1, #0
  MOV      x2, #0
  MOV      x3, #0
  MOV      x4, #0
  MOV      x5, #0
  MOV      x6, #0
  MOV      x7, #0
  MOV      x8, #0
  MOV      x9, #0
  MOV      x10, #0
  MOV      x11, #0
  MOV      x12, #0
  MOV      x13, #0
  MOV      x14, #0
  MOV      x15, #0
  MOV      x16, #0
  MOV      x17, #0
  MOV      x18, #0
  MOV      x19, #0
  MOV      x20, #0
  MOV      x21, #0
  MOV      x22, #0
  MOV      x23, #0
  MOV      x24, #0
  MOV      x25, #0
  MOV      x26, #0
  MOV      x27, #0
  MOV      x28, #0
  MOV      x29, #0
  MOV      x30, #0
  
  // Which core am I
  // ----------------
  MRS      x0, MPIDR_EL1
  AND      x0, x0, #0xFF                     // Mask off to leave Aff0
  CBZ      x0, boot                          // If core 0, run the primary init code
sleep:
  WFI
  B        sleep
boot:
  

  // Disable trapping of CPTR_EL3 accesses or use of Adv.SIMD/FPU
  // -------------------------------------------------------------
  MOV      x0, #0                           // Clear all trap bits
  MSR      CPTR_EL3, x0
  
  
  // Install vector table
  // ---------------------
  LDR      x0, vector_table_address
  MSR      VBAR_EL3, x0

  
  // Configure SCR_EL3
  // ------------------
  MOV      w1, #0              // Initial value of register is unknown
  ORR      w1, w1, #(1 << 11)  // Set ST bit (Secure EL1 can access CNTPS_TVAL_EL1, CNTPS_CTL_EL1 & CNTPS_CVAL_EL1)
  ORR      w1, w1, #(1 << 10)  // Set RW bit (EL1 is AArch64, as this is the Secure world)
  ORR      w1, w1, #(1 << 3)   // Set EA bit (SError routed to EL3)
  ORR      w1, w1, #(1 << 2)   // Set FIQ bit (FIQs routed to EL3)
  ORR      w1, w1, #(1 << 1)   // Set IRQ bit (IRQs routed to EL3)
  MSR      SCR_EL3, x1


  //
  // Cortex-A series specified configuration
  //
  .ifdef CORTEXA
    // Configure ACTLR_EL1
    // --------------------
    // These bits are IMP DEF, so need to different for different
    // processors
    //MRS      x1, ACTLR_EL1
    //ORR      x1, x1, #1          // Enable EL1 access to ACTLR_EL1
    //ORR      x1, x1, #(1 << 1)   // Enable EL1 access to CPUECTLR_EL1
    //ORR      x1, x1, #(1 << 4)   // Enable EL1 access to L2CTLR_EL1
    //ORR      x1, x1, #(1 << 5)   // Enable EL1 access to L2ECTLR_EL1
    //ORR      x1, x1, #(1 << 6)   // Enable EL1 access to L2ACTLR_EL1
    //MSR      ACTLR_EL1, x1

    // Configure CPUECTLR_EL1
    // -----------------------
    // These bits are IMP DEF, so need to different for different
    // processors
    // SMPEN - bit 6 - Enables the processor to receive cache
    //                 and TLB maintenance operations
    //
    // NOTE: For Cortex-A57/53 CPUEN should be set beforebefore
    //       enabling the caches and MMU, or performing any cache
    //       and TLB maintenance operations.
    //MRS      x0, S3_1_c15_c2_1  // Read EL1 CPU Extended Control Register
    //ORR      x0, x0, #(1 << 6)  // Set the SMPEN bit
    //MSR      S3_1_c15_c2_1, x0  // Write EL1 CPU Extended Control Register
    //ISB
  .endif


  // Ensure changes to system register are visible before MMU enabled
  ISB


  // Enable Interrupts
  // ------------------
  MSR      DAIFClr, 0x3


  // Branch to scatter loading and C library init code
  .global  __main
  B        __main

//==================================================================
// Manually created literals pool
//==================================================================
  .align 3

lit_stackheap_limit:
  .global  Image$$ARM_LIB_STACKHEAP$$ZI$$Limit   // Linker symbol from scatter file
  .quad (Image$$ARM_LIB_STACKHEAP$$ZI$$Limit)

vector_table_address:
  .global  el3_vectors
  .quad    el3_vectors

// ------------------------------------------------------------
// End of file
// ------------------------------------------------------------

