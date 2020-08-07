; ------------------------------------------------------------
; Cortex-A MPCore - SCU functions
;
; Copyright ARM Ltd 2009. All rights reserved.
; ------------------------------------------------------------

  PRESERVE8

  AREA  MP_SCU, CODE, READONLY

; ------------------------------------------------------------
; Misc
; ------------------------------------------------------------

  EXPORT get_base_addr
  ; unsigned int get_base_addr(void)
  ; Returns the base address of the private peripheral memory space
get_base_addr PROC
  MRC     p15, 4, r0, c15, c0, 0  ; Read periph base address      (see DE593076)
  BX      lr
  ENDP

; ------------------------------------------------------------

  EXPORT get_cpu_id
  ; unsigned int get_cpu_id(void)
  ; Returns the CPU ID (0 to 3) of the CPU executed on
get_cpu_id PROC
  MRC     p15, 0, r0, c0, c0, 5   ; Read CPU ID register
  AND     r0, r0, #0x03           ; Mask off, leaving the CPU ID field
  BX      lr
  ENDP

; ------------------------------------------------------------

  EXPORT get_num_cpus
  ; unsigned int get_num_cpus(void)
  ; Returns the number of CPUs in the A9 Cluster
get_num_cpus PROC

  ; Get base address of private perpherial space
  MRC     p15, 4, r0, c15, c0, 0  ; Read periph base address
  
  LDR     r0, [r0, #0x004]        ; Read SCU Configuration register
  AND     r0, r0, #0x3            ; Bits 1:0 gives the number of cores

  BX      lr
  ENDP
  
; ------------------------------------------------------------

  EXPORT go_to_sleep
  ; void go_to_sleep(void)
go_to_sleep PROC
  WFI                             ; Go into standby
  B       go_to_sleep             ; Catch in case of rogue events
  BX      lr
  ENDP

; ------------------------------------------------------------
; SCU
; ------------------------------------------------------------

  ; SCU offset from base of private peripheral space --> 0x000

  EXPORT  enable_scu
  ; void enable_scu(void)
  ; Enables the SCU
enable_scu PROC

  MRC     p15, 4, r0, c15, c0, 0  ; Read periph base address

  LDR     r1, [r0, #0x0]          ; Read the SCU Control Register
  ORR     r1, r1, #0x1            ; Set bit 0 (The Enable bit)
  STR     r1, [r0, #0x0]          ; Write back modifed value

  BX      lr
  ENDP

; ------------------------------------------------------------

  EXPORT  join_smp
  ; void join_smp(void)
  ; Set this CPU as participating in SMP
join_smp  PROC

  ; SMP status is controlled by bit 6 of the CP15 Aux Ctrl Reg

  MRC     p15, 0, r0, c1, c0, 1   ; Read ACTLR
  ORR     r0, r0, #0x040          ; Set bit 6
  MCR     p15, 0, r0, c1, c0, 1   ; Write ACTLR

  BX      lr
  ENDP

; ------------------------------------------------------------

  EXPORT leave_smp
  ; void leave_smp(void)
  ; Set this CPU as NOT participating in SMP
leave_smp PROC

  ; SMP status is controlled by bit 6 of the CP15 Aux Ctrl Reg

  MRC     p15, 0, r0, c1, c0, 1   ; Read ACTLR
  BIC     r0, r0, #0x040          ; Clear bit 6
  MCR     p15, 0, r0, c1, c0, 1   ; Write ACTLR

  BX      lr
  ENDP

; ------------------------------------------------------------

  EXPORT get_cpus_in_smp
  ; unsigned int get_cpus_in_smp(void)
  ; The return value is 1 bit per core:
  ; bit 0 - CPU 0
  ; bit 1 - CPU 1
  ; etc...
get_cpus_in_smp PROC

  MRC     p15, 4, r0, c15, c0, 0  ; Read periph base address

  LDR     r0, [r0, #0x004]        ; Read SCU Configuration register
  MOV     r0, r0, LSR #4          ; Bits 7:4 gives the cores in SMP mode, shift then mask
  AND     r0, r0, #0x0F

  BX      lr
  ENDP

; ------------------------------------------------------------

  EXPORT enable_maintenance_broadcast
  ; void enable_maintenance_broadcast(void)
  ; Enable the broadcasting of cache & TLB maintenance operations
  ; When enabled AND in SMP, broadcast all "inner sharable"
  ; cache and TLM maintenance operations to other SMP cores
enable_maintenance_broadcast PROC
  MRC     p15, 0, r0, c1, c0, 1   ; Read Aux Ctrl register
  ORR     r0, r0, #0x01           ; Set the FW bit (bit 0)
  MCR     p15, 0, r0, c1, c0, 1   ; Write Aux Ctrl register

  BX      lr
  ENDP

; ------------------------------------------------------------

  EXPORT disable_maintenance_broadcast
  ; void disable_maintenance_broadcast(void)
  ; Disable the broadcasting of cache & TLB maintenance operations
disable_maintenance_broadcast PROC
  MRC     p15, 0, r0, c1, c0, 1   ; Read Aux Ctrl register
  BIC     r0, r0, #0x01           ; Clear the FW bit (bit 0)
  MCR     p15, 0, r0, c1, c0, 1   ; Write Aux Ctrl register

  BX      lr
  ENDP

; ------------------------------------------------------------

  EXPORT secure_SCU_invalidate
  ; void secure_SCU_invalidate(unsigned int cpu, unsigned int ways)
  ; cpu: 0x0=CPU 0 0x1=CPU 1 etc...
  ; This function invalidates the SCU copy of the tag rams
  ; for the specified core.  Typically only done at start-up.
  ; Possible flow:
  ; - Invalidate L1 caches
  ; - Invalidate SCU copy of TAG RAMs
  ; - Join SMP
secure_SCU_invalidate PROC
  AND     r0, r0, #0x03           ; Mask off unused bits of CPU ID
  MOV     r0, r0, LSL #2          ; Convert into bit offset (four bits per core)
  
  AND     r1, r1, #0x0F           ; Mask off unused bits of ways
  MOV     r1, r1, LSL r0          ; Shift ways into the correct CPU field

  MRC     p15, 4, r2, c15, c0, 0  ; Read periph base address

  STR     r1, [r2, #0x0C]         ; Write to SCU Invalidate All in Secure State
  
  BX      lr

  ENDP

; ------------------------------------------------------------
; End of code
; ------------------------------------------------------------

  END

; ------------------------------------------------------------
; End of MP_SCU.s
; ------------------------------------------------------------
