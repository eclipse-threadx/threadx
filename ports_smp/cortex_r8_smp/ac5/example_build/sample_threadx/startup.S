; ------------------------------------------------------------
; Cortex-R8 MPCore SMP Prime Number Generator Example
;
; Copyright (c) 2011-2018 Arm Limited (or its affiliates). All rights reserved.
; Use, modification and redistribution of this file is subject to your possession of a
; valid End User License Agreement for the Arm Product of which these examples are part of
; and your compliance with all applicable terms and conditions of such licence agreement.
; ------------------------------------------------------------

; MPU region defines

    PRESERVE8

  AREA  StartUp,CODE,READONLY


; Region size <256 bytes is unpredictable

Region_256B     EQU     0x07
Region_512B     EQU     0x08
Region_1K       EQU     0x09
Region_2K       EQU     0x0a
Region_4K       EQU     0x0b
Region_8K       EQU     0x0c
Region_16K      EQU     0x0d
Region_32K      EQU     0x0e
Region_64K      EQU     0x0f
Region_128K     EQU     0x10
Region_256K     EQU     0x11
Region_512K     EQU     0x12
Region_1M       EQU     0x13
Region_2M       EQU     0x14
Region_4M       EQU     0x15
Region_8M       EQU     0x16
Region_16M      EQU     0x17
Region_32M      EQU     0x18
Region_64M      EQU     0x19
Region_128M     EQU     0x1a
Region_256M     EQU     0x1b
Region_512M     EQU     0x1c
Region_1G       EQU     0x1d
Region_2G       EQU     0x1e
Region_4G       EQU     0x1f

Region_Enable   EQU     0x01
Execute_Never   EQU     0x1000

Normal_nShared  EQU     0x03 ; Outer and Inner write-back, no write-allocate
Device_Shared   EQU     (0x0<<0x3 :OR: 0x1)
Device_nShared  EQU     0x10

Full_Access     EQU     0x03
Read_Only       EQU     0x06

Shared          EQU     0x04


; Standard definitions of mode bits and interrupt (I&F) flags in PSRs

Mode_USR    EQU     0x10
Mode_FIQ    EQU     0x11
Mode_IRQ    EQU     0x12
Mode_SVC    EQU     0x13
Mode_ABT    EQU     0x17
Mode_UND    EQU     0x1B
Mode_SYS    EQU     0x1F

I_Bit       EQU     0x80 ; when I bit is set, IRQ is disabled
F_Bit       EQU     0x40 ; when F bit is set, FIQ is disabled


; ------------------------------------------------------------
; Porting defines
; ------------------------------------------------------------

L1_COHERENT     EQU     0x00014c06 ; Template descriptor for coherent memory
L1_NONCOHERENT  EQU     0x00000c1e ; Template descriptor for non-coherent memory
L1_DEVICE       EQU     0x00000c16 ; Template descriptor for device memory

; ------------------------------------------------------------

    ENTRY

    EXPORT Vectors

Vectors
  B      Reset_Handler
  B      Undefined_Handler
  B      SVC_Handler
  B      Prefetch_Handler
  B      Abort_Handler
  B .    ;Reserved vector
  B      IRQ_Handler
  B      FIQ_Handler

; ------------------------------------------------------------
; Handlers for unused exceptions
; ------------------------------------------------------------

Undefined_Handler
  B       Undefined_Handler
SVC_Handler
  B       SVC_Handler
Prefetch_Handler
  B       Prefetch_Handler
Abort_Handler
  B       Abort_Handler
FIQ_Handler
  B       FIQ_Handler

; ------------------------------------------------------------
; Imports
; ------------------------------------------------------------

  IMPORT read_irq_ack
  IMPORT write_end_of_irq
  IMPORT enable_GIC
  IMPORT enable_gic_processor_interface
  IMPORT set_priority_mask
  IMPORT enable_irq_id
  IMPORT set_irq_priority
  IMPORT enable_scu
  IMPORT join_smp
  IMPORT secure_SCU_invalidate
  IMPORT enable_maintenance_broadcast
  IMPORT init_private_timer
  IMPORT start_private_timer
  IMPORT clear_private_timer_irq
  IMPORT __main

  IMPORT __use_two_region_memory
  IMPORT ||Image$$IRQ_STACK$$ZI$$Limit||

; [EL Change Start]
  IMPORT _tx_thread_smp_initialize_wait
  IMPORT _tx_thread_smp_release_cores_flag
  IMPORT _tx_thread_context_save
  IMPORT _tx_thread_context_restore
  IMPORT _tx_timer_interrupt
  IMPORT _tx_thread_smp_inter_core_interrupts
; [EL Change End]


; ------------------------------------------------------------
; Interrupt Handler
; ------------------------------------------------------------

  EXPORT IRQ_Handler
  EXPORT __tx_irq_processing_return
IRQ_Handler PROC
; [EL Change Start]
;  SUB     lr, lr, #4          ; Pre-adjust lr
;  SRSFD   sp!, #Mode_IRQ      ; Save lr and SPRS to IRQ mode stack
;  PUSH    {r0-r4, r12}        ; Sace APCS corruptable registers to IRQ mode stack (and maintain 8 byte alignment)
;
;    /* Jump to context save to save system context.  */
    B       _tx_thread_context_save
__tx_irq_processing_return
    PUSH    {r4, r5}            ; Save some preserved registers (r5 is saved just for 8-byte alignment)
; [EL Change End]

  ; Acknowledge the interrupt
  BL      read_irq_ack
  MOV     r4, r0

  ;
  ; This example only uses (and enables) one.  At this point
  ; you would normally check the ID, and clear the source.
  ;

  ;
  ; Additonal code to handler private timer interrupt on CPU0
  ;

  CMP     r0, #29             ; If not Private Timer interrupt (ID 29), by pass
  BNE     by_pass

; [EL Change Start]
;  MOV     r0, #0x04 ; Code for SYS_WRITE0
;  LDR     r1, =irq_handler_message0
;  SVC     0x123456
; [EL Change End]

  ; Clear timer interrupt
  BL      clear_private_timer_irq
  DSB
; [EL Change Start]
    BL      _tx_timer_interrupt                 ; Timer interrupt handler
; [EL Change End]

  B       by_pass2

by_pass

; [EL Change Start]
  ;
  ;  Additional code to handle SGI on CPU0
  ;
;
;  MRC     p15, 0, r0, c0, c0, 5     ; Read CPU ID register
;  ANDS    r0, r0, #0x03             ; Mask off, leaving the CPU ID field
;  BNE     by_pass2
;  
;  MOV     r0, #0x04 ; Code for SYS_WRITE0
;  LDR     r1, =irq_handler_message1
;  SVC     0x123456
;
;    /* Just increment the per-thread interrupt count for analysis purposes.  */
;
    MRC     p15, 0, r0, c0, c0, 5                   ; Read CPU ID register
    AND     r0, r0, #0x03                           ; Mask off, leaving the CPU ID field
    LSL     r0, r0, #2                              ; Build offset to array indexes
    LDR     r1,=_tx_thread_smp_inter_core_interrupts ; Pickup base address of core interrupt counter array
    ADD     r1, r1, r0                              ; Build array index
    LDR     r0, [r1]                                ; Pickup counter
    ADD     r0, r0, #1                              ; Increment counter
    STR     r0, [r1]                                ; Store back counter
;
; [EL Change End]


by_pass2
  ; Write end of interrupt reg
  MOV     r0, r4
  BL      write_end_of_irq

; [EL Change Start]

;
;    /* Jump to context restore to restore system context.  */
    POP     {r4, r5}                ; Recover preserved registers
    B       _tx_thread_context_restore

;  POP     {r0-r4, r12}        ; Restore stacked APCS registers
;  MOV     r2, #0x01           ; Set r2 so CPU leaves holding pen
;  RFEFD   sp!                 ; Return from exception
; [EL Change End]

  ENDP


; ------------------------------------------------------------
; Reset Handler - Generic initialization, run by all CPUs
; ------------------------------------------------------------

  IMPORT ||Image$$IRQ_STACK$$ZI$$Limit||
  IMPORT ||Image$$ARM_LIB_STACK$$ZI$$Limit||
  IMPORT enable_branch_prediction
  IMPORT invalidate_caches

  EXPORT Reset_Handler   ; Exported for callgraph purposes!
Reset_Handler PROC

;----------------------------------------------------------------
; Disable MPU and caches
;----------------------------------------------------------------

; Disable MPU and cache in case it was left enabled from an earlier run
; This does not need to be done from a cold reset

        MRC p15, 0, r0, c1, c0, 0       ; Read System Control Register
        BIC r0, r0, #0x05               ; Disable MPU (M bit) and data cache (C bit)
        BIC r0, r0, #0x800              ; Disable branch prediction (Z bit)
        BIC r0, r0, #0x1000             ; Disable instruction cache (I bit)
        DSB                             ; Ensure all previous loads/stores have completed
        MCR p15, 0, r0, c1, c0, 0       ; Write System Control Register
        ISB                             ; Ensure subsequent insts execute wrt new MPU settings

; [EL Change Start]
  ;
  ; Setup stacks
  ;---------------
;;  MSR     CPSR_c, #Mode_IRQ :OR: I_Bit :OR: F_Bit
;;  LDR     sp, =||Image$$IRQ_STACK$$ZI$$Limit||
;
;;  MSR     CPSR_c, #Mode_SYS :OR: I_Bit :OR :F_Bit   ; No interrupts
;;  LDR     sp, =||Image$$ARM_LIB_STACK$$ZI$$Limit||

  MSR     CPSR_c, #Mode_IRQ :OR: I_Bit :OR: F_Bit
  LDR     sp, =||Image$$IRQ_STACK$$ZI$$Limit||

  MSR     CPSR_c, #Mode_SVC :OR: I_Bit :OR: F_Bit   ; No interrupts
  LDR     sp, =||Image$$ARM_LIB_STACK$$ZI$$Limit||

  MRC     p15, 0, r1, c0, c0, 5     ; Read Multiprocessor Affinity Register
  ANDS    r1, r1, #0x03             ; Mask off, leaving the CPU ID field
  CMP     r1,#0                     ; Is it core 0?
  BEQ     _stacks_setup             ;
_core_1
  MSR     CPSR_c, #Mode_IRQ :OR: I_Bit :OR: F_Bit
  MOV     r1, #0x800
  SUB     sp, sp, r1

  MSR     CPSR_c, #Mode_SVC :OR: I_Bit :OR: F_Bit   ; No interrupts
  MOV     r1, #0x1000
  SUB     sp, sp, r1
_stacks_setup
; [EL Change End]

  ;
  ; Invalidate caches
  ; ------------------
  BL      invalidate_caches


  ; Clear Branch Prediction Array
  ; ------------------------------
  MOV     r0, #0x0
  MCR     p15, 0, r0, c7, c5, 6     ; BPIALL - Invalidate entire branch predictor array


  ;
  ; Activate VFP/NEON, if required
  ;-------------------------------

    IF  {TARGET_FPU_VFP} = {TRUE}

  ; Enable access to NEON/VFP by enabling access to Coprocessors 10 and 11.
  ; Enables Full Access i.e. in both privileged and non privileged modes
    MRC     p15, 0, r0, c1, c0, 2     ; Read Coprocessor Access Control Register (CPACR)
    ORR     r0, r0, #(0xF << 20)      ; Enable access to CP 10 & 11
    MCR     p15, 0, r0, c1, c0, 2     ; Write Coprocessor Access Control Register (CPACR)
    ISB

  ; Switch on the VFP and NEON hardware
    MOV     r0, #0x40000000
    VMSR    FPEXC, r0                   ; Write FPEXC register, EN bit set

    ENDIF

; [EL Change Start] - we don't use the MPU
  ;BL      configMPU
; [EL Change End]

  ;
  ; SMP initialization
  ; -------------------
  MRC     p15, 0, r0, c0, c0, 5     ; Read CPU ID register
  ANDS    r0, r0, #0x03             ; Mask off, leaving the CPU ID field
  BEQ     primaryCPUInit
  BNE     secondaryCPUsInit

  ENDP


;----------------------------------------------------------------
; MPU Configuration
;----------------------------------------------------------------

   ; Notes:
   ; * Regions apply to both instruction and data accesses.
   ; * Each region base address must be a multiple of its size
   ; * Any address range not covered by an enabled region will abort
   ; * The region at 0x0 over the Vector table is needed to support semihosting

   ; Region 0: Init       Base = 0x48000000  Size = 64KB   Normal  Non-shared  Full access  Executable
   ; Region 1: Code       Base = 0x48010000  Size = 32KB   Normal  Non-shared  Full access  Executable
   ; Region 1: Data       Base = 0x48018000  Size = 16KB   Normal  Non-shared  Full access  Not Executable
   ; Region 2: Stack/Heap         Base = 0x4801C000  Size = 8KB    Normal   Non-shared  Full access  Not Executable
   ; Region 4: Vectors    Base = 0x0000  Size =  64KB   Normal  Nonshared  Full access  Executable
   ; Region 5: GIC      Base = 0xAE000000  Size =  8KB   Device  shared  Full access    Not Executable

  EXPORT
configMPU PROC

    MRC     p15, 0, r0, c0, c0, 5       ; Read CPU ID register
    ANDS    r0, r0, #0x03               ; Mask off, leaving the CPU ID field
        ; Import linker symbols to get region base addresses
    IMPORT  ||Image$$INIT$$Base||
    IMPORT  ||Image$$CODE$$Base||
    IMPORT  ||Image$$SHARED_DATA$$Base||
    IMPORT  ||Image$$ARM_LIB_STACK$$Base||

    ;; Region 0 - Init
        MOV     r1, #0
        MCR     p15, 0, r1, c6, c2, 0       ; Set memory region number register
        ISB                                 ; Ensure subsequent insts execute wrt this region
        LDR     r2, =||Image$$INIT$$Base||
        MCR     p15, 0, r2, c6, c1, 0       ; Set region base address register
        LDR     r2, =0x0  :OR:  (Region_64K << 1)   :OR:  Region_Enable
        MCR     p15, 0, r2, c6, c1, 2       ; Set region size & enable register
        LDR     r2, =0x0 :OR: (Full_Access  << 8)  :OR: Normal_nShared
        MCR     p15, 0, r2, c6, c1, 4       ; Set region access control register

        ; Region 1 - Code
        ADD     r1, r1, #1
        MCR     p15, 0, r1, c6, c2, 0       ; Set memory region number register
        ISB                                 ; Ensure subsequent insts execute wrt this region
        LDR     r2, =||Image$$CODE$$Base||
        MCR     p15, 0, r2, c6, c1, 0       ; Set region base address register
        LDR     r2, =0x0 :OR: (Region_64K << 1) :OR: Region_Enable
        MCR     p15, 0, r2, c6, c1, 2       ; Set region size & enable register
        LDR     r2, =0x0 :OR: (Full_Access  << 8)  :OR: Normal_nShared
        MCR     p15, 0, r2, c6, c1, 4       ; Set region access control register

        ; Region 2 - Data
        ADD     r1, r1, #1
        MCR     p15, 0, r1, c6, c2, 0       ; Set memory region number register
        ISB                                 ; Ensure subsequent insts execute wrt this region
        LDR     r2, =||Image$$SHARED_DATA$$Base||
        MCR     p15, 0, r2, c6, c1, 0       ; Set region base address register
        LDR     r2, =0x0 :OR: (Region_16K << 1) :OR: Region_Enable
        MCR     p15, 0, r2, c6, c1, 2       ; Set region size & enable register
        LDR     r2, =0x0 :OR: (Full_Access << 8) :OR: Normal_nShared :OR: Execute_Never
        MCR     p15, 0, r2, c6, c1, 4       ; Set region access control register

        ; Region 3 - Stack/heap
        ADD     r1, r1, #1
        MCR     p15, 0, r1, c6, c2, 0       ; Set memory region number register
        ISB                                 ; Ensure subsequent insts execute wrt this region
        LDR     r2, =||Image$$ARM_LIB_STACK$$Base||
        MCR     p15, 0, r2, c6, c1, 0       ; Set region base address register
        LDR     r2, =0x0 :OR: (Region_16K << 1) :OR: Region_Enable
        MCR     p15, 0, r2, c6, c1, 2       ; Set region size & enable register
        LDR     r2, =0x0 :OR: (Full_Access << 8) :OR: Normal_nShared :OR: Execute_Never
        MCR     p15, 0, r2, c6, c1, 4       ; Set region access control register

        ; Region 4 - Vectors
        ADD     r1, r1, #1
        MCR     p15, 0, r1, c6, c2, 0       ; Set memory region number register
        ISB                                 ; Ensure subsequent insts execute wrt this region
        LDR     r2, =0
        MCR     p15, 0, r2, c6, c1, 0       ; Set region base address register
        LDR     r2, =0x0 :OR: (Region_64K  << 1) :OR: Region_Enable
        MCR     p15, 0, r2, c6, c1, 2       ; Set region size & enable register
        LDR     r2, =0x0 :OR: (Full_Access << 8) :OR:Normal_nShared :OR: Shared
        MCR     p15, 0, r2, c6, c1, 4       ; Set region access control register

        ; Region 5 - GIC
        ADD     r1, r1, #1
        MCR     p15, 0, r1, c6, c2, 0       ; Set memory region number register
        ISB                                 ; Ensure subsequent insts execute wrt this region
        LDR     r2, =0xAE000000
        MCR     p15, 0, r2, c6, c1, 0       ; Set region base address register
        LDR     r2, =0x0 :OR: (Region_8K  << 1):OR: Region_Enable
        MCR     p15, 0, r2, c6, c1, 2       ; Set region size & enable register
        LDR     r2, =0x0 :OR: (Full_Access << 8) :OR: Device_Shared :OR: Execute_Never
        MCR     p15, 0, r2, c6, c1, 4       ; Set region access control register


        ; Disable all higher priority regions (assumes unified regions, which is always true for Cortex-R8)
        MRC     p15, 0, r0, c0, c0, 4       ; Read MPU Type register (MPUIR)
        LSR     r0, r0, #8
        AND     r0, r0, #0xff               ; r0 = Number of MPU regions (12, 16, 20, or 24 for Cortex-R8)
        MOV     r2, #0                      ; Value to write to disable region
region_loop
        ADD     r1, r1, #1
        CMP     r0, r1
        BLS     regions_done
        MCR     p15, 0, r1, c6, c2, 0       ; Set memory region number register (RGNR)
        MCR     p15, 0, r2, c6, c1, 2       ; Set region size & enable register (DRSR)
        B       region_loop
regions_done
        BX      lr

  ENDP

; ------------------------------------------------------------
; Initialization for PRIMARY CPU
; ------------------------------------------------------------

  EXPORT primaryCPUInit
primaryCPUInit PROC

  ; Enable the SCU
  ; ---------------
  BL      enable_scu

  ;
  ; Join SMP
  ; ---------
  MOV     r0, #0x0                  ; Move CPU ID into r0
  MOV     r1, #0xF                  ; Move 0xF (represents all four ways) into r1
  BL      join_smp

  ;
  ; GIC Init
  ; ---------
  BL      enable_GIC
  BL      enable_gic_processor_interface

; [EL Change Start]
;  /* Leave MPU disabled */
;  ; Enable MPU
;  ; -----------
;  ; Leave the caches disabled until after scatter loading.
;  MRC     p15, 0, r0, c1, c0, 0       ; Read System Control Register
;  ORR     r0, r0, #0x1                ; Set M bit 0 to enable MMU before scatter loading
;  MCR     p15, 0, r0, c1, c0, 0       ; Write System Control Register

  ;
  ; Enable Private Timer for periodic IRQ
  ; --------------------------------------
  MOV     r0, #0x1F
  BL      set_priority_mask            ; Set priority mask (local)

  ; [EL] Change start - don't enable interrupts here!  
  ;CPSIE   i                            ; Clear CPSR I bit
  ; [EL] Change end

  ; Enable the Private Timer Interrupt Source
  MOV     r0, #29
  MOV     r1, #0
  BL      enable_irq_id

  ; Set the priority
  MOV     r0, #29
  MOV     r1, #0
  BL      set_irq_priority

  ; Configure Timer
  MOV     r0, #0xF0000
  MOV     r1, #0x0
  BL      init_private_timer
  BL      start_private_timer
  
  ;
  ; Enable receipt of SGI 0
  ; ------------------------
  MOV     r0, #0x0                  ; ID
  BL      enable_irq_id

  MOV     r0, #0x0                  ; ID
  MOV     r1, #0x0                  ; Priority
  BL      set_irq_priority
; [EL Change End]

  ;
  ; Branch to C lib code
  ; ----------------------
  B       __main

  ENDP



; ------------------------------------------------------------
; Initialization for SECONDARY CPUs
; ------------------------------------------------------------

  EXPORT secondaryCPUsInit
secondaryCPUsInit PROC

; [EL Change Start] - Don't configure MPU
  ;BL      configMPU
; [EL Change End]

  ;
  ; GIC Init
  ; ---------
  BL      enable_gic_processor_interface

  MOV     r0, #0x1F                 ; Priority
  BL      set_priority_mask

  MOV     r0, #0x0                  ; ID
  BL      enable_irq_id

  MOV     r0, #0x0                  ; ID
  MOV     r1, #0x0                  ; Priority
  BL      set_irq_priority

  ;
  ; Join SMP
  ; ---------
  MRC     p15, 0, r0, c0, c0, 5     ; Read CPU ID register
  ANDS    r0, r0, #0x03             ; Mask off, leaving the CPU ID field
  MOV     r1, #0xF                  ; Move 0xF (represents all four ways) into r1
  BL      secure_SCU_invalidate

  BL      join_smp
  BL      enable_maintenance_broadcast

; [EL Change Start]
;  /* Leave MPU disabled */
;  ; Enable MPU
;  ; -----------
;  ; Leave the caches disabled until after scatter loading.
;  MRC     p15, 0, r0, c1, c0, 0       ; Read System Control Register
;  ORR     r0, r0, #0x1                ; Set M bit 0 to enable MMU before scatter loading
;  MCR     p15, 0, r0, c1, c0, 0       ; Write System Control Register
;  ISB

  ;
  ; Holding Pen
  ; ------------
;  MOV     r2, #0x00                 ; Clear r2
;  CPSIE   i                         ; Enable interrupts
;holding_pen
;  CMP     r2, #0x0                  ; r2 will be set to 0x1 by IRQ handler on receiving SGI
;  WFIEQ
;  BEQ     holding_pen
;  CPSID   i                         ; IRQs not used in reset of example, so mask out interrupts
;skip
;
  ;
  ; Branch to C lib code
  ; ----------------------
;  B       __main

  B        _tx_thread_smp_initialize_wait
; [EL Change End]

  ENDP

  END

; ------------------------------------------------------------
; End of startup.s
; ------------------------------------------------------------
