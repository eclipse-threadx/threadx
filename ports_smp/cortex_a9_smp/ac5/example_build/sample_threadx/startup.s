; ------------------------------------------------------------
; Cortex-A9 MPCore SMP Prime Number Generator Example
;
; Copyright ARM Ltd 2009. All rights reserved.
; ------------------------------------------------------------

    PRESERVE8

  AREA  StartUp,CODE,READONLY

; ------------------------------------------------------------
; Define some values
; ------------------------------------------------------------

; - Standard definitions of mode bits and interrupt (I&F) flags in PSRs
Mode_USR          EQU   0x10
Mode_FIQ          EQU   0x11
Mode_IRQ          EQU   0x12
Mode_SVC          EQU   0x13
Mode_ABT          EQU   0x17
Mode_UNDEF        EQU   0x1B
Mode_SYS          EQU   0x1F
I_Bit             EQU   0x80 ; when I bit is set, IRQ is disabled
F_Bit             EQU   0x40 ; when F bit is set, FIQ is disabled



; ------------------------------------------------------------
; Porting defines
; ------------------------------------------------------------

L1_COHERENT       EQU   0x00014c06  ; Template descriptor for coherent memory
L1_NONCOHERENT    EQU   0x00000c1e  ; Template descriptor for non-coherent memory
L1_DEVICE         EQU   0x00000c16  ; Template descriptor for device memory

; ------------------------------------------------------------

  ENTRY

  EXPORT Vectors

Vectors
  B      Reset_Handler
  B      Undefined_Handler
  B      SWI_Handler
  B      Prefetch_Handler
  B      Abort_Handler
  NOP    ;Reserved vector
  B      IRQ_Handler
  B      FIQ_Handler

; ------------------------------------------------------------
; Handlers for unused exceptions
; ------------------------------------------------------------

Undefined_Handler
  B       Undefined_Handler
SWI_Handler
  B       SWI_Handler
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
IRQ_Handler   PROC
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

  IMPORT ||Image$$ARM_LIB_STACK$$ZI$$Limit||
  IMPORT ||Image$$IRQ_STACKS$$ZI$$Limit||
  IMPORT ||Image$$PAGETABLES$$ZI$$Base||
  IMPORT enable_branch_prediction
  IMPORT invalidate_caches

  EXPORT Reset_Handler   ; Exported for callgraph purposes!
Reset_Handler PROC

;
; Disable caches, MMU and branch prediction in case they were left enabled from an earlier run
; This does not need to be done from a cold reset
; ------------------------------------------------------------
  MRC     p15, 0, r0, c1, c0, 0       ; Read System Control Register
  BIC     r0, r0, #(0x1 << 12)        ; Clear I, bit 12, to disable I Cache
  BIC     r0, r0, #(0x1 << 11)        ; Clear Z, bit 11, to disable branch prediction
  BIC     r0, r0, #(0x1 <<  2)        ; Clear C, bit  2, to disable D Cache
  BIC     r0, r0, #(0x1 <<  1)        ; Clear A, bit  1, to disable strict alignment fault checking
  BIC     r0, r0, #0x1                ; Clear M, bit  0, to disable MMU
  MCR     p15, 0, r0, c1, c0, 0       ; Write System Control Register
  ISB

; The MMU is enabled later, before calling main().  Caches and branch prediction are enabled inside main(),
; after the MMU has been enabled and scatterloading has been performed.

  ;
  ; Setup stacks
  ;---------------

  MRC     p15, 0, r0, c0, c0, 5       ; Read CPU ID register
  ANDS    r0, r0, #0x03               ; Mask off, leaving the CPU ID field

  MSR     CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit
  LDR     r1, =||Image$$IRQ_STACKS$$ZI$$Limit||
  SUB     r1, r1, r0, LSL #8          ; 256 bytes of IRQ stack per CPU - see scatter.scat
  MOV     sp, r1

; [EL Change Start]
  ;MSR     CPSR_c, #Mode_SYS:OR:I_Bit:OR:F_Bit   ; Interrupts initially disabled
  MSR     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit   ; Interrupts initially disabled
; [EL Change End]
  LDR     r1, =||Image$$ARM_LIB_STACK$$ZI$$Limit|| ; App stacks for all CPUs
  SUB     r1, r1, r0, LSL #12         ; 0x1000 bytes of App stack per CPU - see scatter.scat
  MOV     sp, r1

; [EL Change Start] - leave MMU disabled

;  ;
;  ; Set vector base address
;  ; ------------------------
;  LDR    r0, =Vectors
;  MCR    p15, 0, r0, c12, c0, 0 ; Write Secure or Non-secure Vector Base Address
;  ; Ensure that V-bit is cleared
;  MRC     p15, 0, r0, c1, c0, 0 ; Read Control Register
;  BIC     r0, r0, #(1 << 13)    ; Clear the V bit (bit 13)
;  MCR     p15, 0, r0, c1, c0, 0 ; Write Control Register
;  ISB
;
;  ; Invalidate caches
;  ; ------------------
;  BL      invalidate_caches
;
;  ;
;  ; Clear Branch Prediction Array
;  ; ------------------------------
;  MOV     r0, #0x0
;  MCR     p15, 0, r0, c7, c5, 6     ; BPIALL - Invalidate entire branch predictor array
;
;  ;
;  ; Invalidate TLBs
;  ;------------------
;  MOV     r0, #0x0
;  MCR     p15, 0, r0, c8, c7, 0     ; TLBIALL - Invalidate entire Unified TLB
;
;  ;
;  ; Set up Domain Access Control Reg
;  ; ----------------------------------
;  ; b00 - No Access (abort)
;  ; b01 - Client (respect table entry)
;  ; b10 - RESERVED
;  ; b11 - Manager (ignore access permissions)
;
;  MRC     p15, 0, r0, c3, c0, 0      ; Read Domain Access Control Register
;  LDR     r0, =0x55555555            ; Initialize every domain entry to b01 (client)
;  MCR     p15, 0, r0, c3, c0, 0      ; Write Domain Access Control Register
;
;  ;;
;  ;; Enable L1 Preloader - Auxiliary Control
;  ;; -----------------------------------------
;  ;; Seems to undef on panda?
;  ;MRC     p15, 0, r0, c1, c0, 1      ; Read ACTLR
;  ;ORR     r0, r0, #0x4
;  ;MCR     p15, 0, r0, c1, c0, 1      ; Write ACTLR
;  ISB
;
;
;  ;
;  ; Set location of level 1 page table
;  ;------------------------------------
;  ; 31:14 - Base addr
;  ; 13:5  - 0x0
;  ; 4:3   - RGN 0x0 (Outer Noncachable)
;  ; 2     - P   0x0
;  ; 1     - S   0x0 (Non-shared)
;  ; 0     - C   0x0 (Inner Noncachable)
;  LDR     r0, =||Image$$PAGETABLES$$ZI$$Base||
;  MSR     TTBR0, r0

; [EL Change End]

  ;
  ; Activate VFP/NEON, if required
  ;-------------------------------

  IF {TARGET_FEATURE_NEON} || {TARGET_FPU_VFP}

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

; [EL Change Start]

  LDR     r0, =_tx_thread_smp_release_cores_flag         ; Build address of release cores flag
  MOV     r1, #0
  STR     r1, [r0]

; [EL Change End]

  ;
  ; SMP initialization
  ; -------------------
  MRC     p15, 0, r0, c0, c0, 5     ; Read CPU ID register
  ANDS    r0, r0, #0x03             ; Mask off, leaving the CPU ID field
  BLEQ    primary_cpu_init
  BLNE    secondary_cpus_init

  ENDP

; ------------------------------------------------------------
; Initialization for PRIMARY CPU
; ------------------------------------------------------------

   EXPORT primary_cpu_init
primary_cpu_init PROC

; [EL Change Start] - leave MMU disabled

;  ; Translation tables
;  ; -------------------
;  ; The translation tables are generated at boot time.  
;  ; First the table is zeroed.  Then the individual valid
;  ; entries are written in
;  ;
;
;  LDR     r0, =||Image$$PAGETABLES$$ZI$$Base||
;
;  ; Fill table with zeros
;  MOV     r2, #1024                 ; Set r3 to loop count (4 entries per iteration, 1024 iterations)
;  MOV     r1, r0                    ; Make a copy of the base dst
;  MOV     r3, #0
;  MOV     r4, #0
;  MOV     r5, #0
;  MOV     r6, #0
;ttb_zero_loop
;  STMIA   r1!, {r3-r6}              ; Store out four entries
;  SUBS    r2, r2, #1                ; Decrement counter
;  BNE     ttb_zero_loop
;
;  ;
;  ; STANDARD ENTRIES
;  ;
;
;  ; Region covering program code and data
;  IMPORT ||Image$$CODE$$Base||
;  LDR     r1,=||Image$$CODE$$Base|| ; Base physical address of program code and data
;  LSR     r1,#20                    ; Shift right to align to 1MB boundaries
;  LDR     r3, =L1_COHERENT          ; Descriptor template
;  ORR     r3, r1, LSL#20            ; Combine address and template
;  STR     r3, [r0, r1, LSL#2]       ; Store table entry
;
;  ; Entry for private address space
;  ; Needs to be marked as Device memory
;  MRC     p15, 4, r1, c15, c0, 0    ; Get base address of private address space
;  LSR     r1, r1, #20               ; Clear bottom 20 bits, to find which 1MB block it is in
;  LSL     r2, r1, #2                ; Make a copy, and multiply by four.  This gives offset into the page tables
;  LSL     r1, r1, #20               ; Put back in address format
;  LDR     r3, =L1_DEVICE            ; Descriptor template
;  ORR     r1, r1, r3                ; Combine address and template
;  STR     r1, [r0, r2]              ; Store table entry
;
;  ;
;  ; OPTIONAL ENTRIES
;  ; You will need additional translations if:
;  ; - No RAM at zero, so cannot use flat mapping
;  ; - You wish to retarget
;  ;
;  ; If you wish to output to stdio to a UART you will need
;  ; an additional entry
;  ;LDR     r1, =PABASE_UART          ; Physical address of UART
;  ;LSR     r1, r1, #20               ; Mask off bottom 20 bits to find which 1MB it is within
;  ;LSL     r2, r1, #2                ; Make a copy and multiply by 4 to get table offset
;  ;LSL     r1, r1, #20               ; Put back into address format
;  ;LDR     r3, =L1_DEVICE            ; Descriptor template
;  ;ORR     r1, r1, r3                ; Combine address and template
;  ;STR     r1, [r0, r2]              ; Store table entry
;
;  DSB
;
;
;  ; Enable MMU
;  ; -----------
;  ; Leave the caches disabled until after scatter loading.
;  MRC     p15, 0, r0, c1, c0, 0       ; Read System Control Register
;  ORR     r0, r0, #0x1                ; Set M bit 0 to enable MMU before scatter loading
;  MCR     p15, 0, r0, c1, c0, 0       ; Write System Control Register
;  ISB

; [EL Change End]

  ; Enable the SCU
  ; ---------------
  BL      enable_scu

  ;
  ; Join SMP
  ; ---------
  MOV     r0, #0x0                  ; Move CPU ID into r0
  MOV     r1, #0xF                  ; Move 0xF (represents all four ways) into r1
  BL      secure_SCU_invalidate
  BL      join_smp
  BL      enable_maintenance_broadcast

  ;
  ; GIC Init
  ; ---------
  BL      enable_GIC
  BL      enable_gic_processor_interface


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


  ;
  ; Branch to C lib code
  ; ----------------------

  B       __main

  ENDP

; ------------------------------------------------------------
; Initialization for SECONDARY CPUs
; ------------------------------------------------------------

  EXPORT secondary_cpus_init
secondary_cpus_init PROC

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

;  ;
;  ; Holding Pen
;  ; ------------
;  MOV     r10, #0                   ; Clear exit flag
;  CPSIE   i                         ; Enable interrupts
;holding_pen
;  CMP     r10, #0                   ; Exit flag will be set to 0x1 by IRQ handler on receiving SGI
;  DSB                               ; Clear all pending data accesses
;  WFIEQ
;  BEQ     holding_pen
;  CPSID   i                         ; Disable interrupts
;
;
;  ;
;  ; The translation tables are generated by the primary CPU
;  ; The MMU cannot be enabled on the secondary CPUs until
;  ; they are released from the holding-pen
;  ;
;
;  ; Enable MMU
;  ; -----------
;  ; Leave the caches disabled until after scatter loading.
;  MRC     p15, 0, r0, c1, c0, 0       ; Read System Control Register
;  ORR     r0, r0, #0x1                ; Set M bit 0 to enable MMU before scatter loading
;  MCR     p15, 0, r0, c1, c0, 0       ; Write System Control Register
;  ISB

; [EL Change End]

  ;
  ; Branch to C lib code
  ; ----------------------
;  B       __main

  B        _tx_thread_smp_initialize_wait
; [EL Change End]

  ENDP

; ------------------------------------------------------------
; End of code
; ------------------------------------------------------------

irq_handler_message0
   DCB   "\n#\n# Private Timer Interrupt!\n# \n\n   \0 "
irq_handler_message1
   DCB   "\n#\n# SGI on CPU0!\n# \n\n   \0\0\0 "

  END

; ------------------------------------------------------------
; End of startup.s
; ------------------------------------------------------------
