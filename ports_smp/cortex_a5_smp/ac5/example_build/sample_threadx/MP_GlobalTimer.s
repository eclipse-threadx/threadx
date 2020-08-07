; ------------------------------------------------------------
; Cortex-A MPCore - Global timer functions
;
; Copyright ARM Ltd 2009. All rights reserved.
; ------------------------------------------------------------

  PRESERVE8

  AREA  MP_PrivateTimer, CODE, READONLY

  ; PPI ID 27


  ; Typical set of calls to enable Timer:
  ; init_global_timer()
  ; set_global_timer_comparator()
  ; start_global_timer()

; ------------------------------------------------------------

  EXPORT init_global_timer
  ; void init_global_timer(unsigned int auto_increment, unsigned int increment_value)
  ; Initializes the Global Timer, but does NOT set the enable bit
  ; r0: IF 0 (AutoIncrement) ELSE (SingleShot)
  ; r1: increment value
init_global_timer PROC

  ; Get base address of private perpherial space
  MRC     p15, 4, r2, c15, c0, 0  ; Read periph base address

  ; Control register bit layout
  ; Bit 0 - Timer enable
  ; Bit 1 - Comp enable
  ; Bit 2 - IRQ enable
  ; Bit 3 - Auto-increment enable

  ; Ensure the timer is disabled
  LDR     r3, [r2, #0x208]        ; Read control reg
  BIC     r3, r3, #0x01           ; Clear enable bit
  STR     r3, [r2, #0x208]        ; Write control reg

  ; Form control reg value
  CMP     r0, #0                  ; Check whether to enable auto-reload
  MOVNE   r0, #0x00               ; No auto-reload
  MOVEQ   r0, #0x04               ; With auto-reload
  STR     r0, [r2, #0x208]        ; Store to control register

  ; Store increment value
  STREQ   r1, [r2, #0x218]                                   
  
  ; Clear timer value
  MOV     r0, #0x0
  STR     r0, [r2, #0x0]
  STR     r0, [r2, #0x4]

  BX      lr
  ENDP

; ------------------------------------------------------------

  EXPORT set_global_timer_comparator
  ; void set_global_timer_comparator(unsigned int top, unsigned int bottom);
  ; Writes the comparator registers, and enable the comparator bit in the control register
  ; r0: 63:32 of the comparator value
  ; r1: 31:0  of the comparator value
set_global_timer_comparator PROC

  MRC     p15, 4, r2, c15, c0, 0  ; Read periph base address

  ; Disable comparator before updating register
  LDR     r1, [r2, #0x208]        ; Read control reg
  BIC     r3, r3, #0x02           ; Clear comparator enable bit
  STR     r3, [r2, #0x208]        ; Write modified value back
  
  ; Write the comparator registers
  STR     r1, [r2, #0x210]        ; Write lower 32 bits
  STR     r0, [r2, #0x214]        ; Write upper 32 bits
  DMB
  
  ; Re-enable the comparator
  ORR     r3, r3, #0x02           ; Set comparator enable bit
  STR     r3, [r2, #0x208]        ; Write modified value back

  BX      lr
  ENDP

; ------------------------------------------------------------

  EXPORT start_global_timer
  ; void start_global_timer(void)
  ; Starts the global timer
start_global_timer PROC

  MRC     p15, 4, r0, c15, c0, 0  ; Read periph base address

  LDR     r1, [r0, #0x208]        ; Read control reg
  ORR     r1, r1, #0x01           ; Set enable bit
  STR     r1, [r0, #0x208]        ; Write modified value back

  BX      lr
  ENDP

; ------------------------------------------------------------

  EXPORT stop_global_timer
  ; void stop_global_timer
  ; Stops the stop_global_timer timer
stop_global_timer PROC

  MRC     p15, 4, r0, c15, c0, 0  ; Read periph base address

  LDR     r1, [r0, #0x208]        ; Read control reg
  BIC     r1, r1, #0x01           ; Clear enable bit
  STR     r1, [r0, #0x208]        ; Write modified value back

  BX      lr
  ENDP

; ------------------------------------------------------------

  EXPORT get_global_timer_count
  ; void read_global_timer(unsigned int* top, unsigned int* bottom)
  ; Reads the current value of the timer count register
  ; r0: Address of unsigned int for bits 63:32
  ; r1: Address of unsigned int for bits 31:0
get_global_timer_count PROC

get_global_timer_count_loop
  MRC     p15, 4, r2, c15, c0, 0  ; Read periph base address

  LDR     r12,[r2, #0x04]         ; Read bits 63:32
  LDR     r3, [r2, #0x00]         ; Read bits 31:0
  LDR     r2, [r2, #0x04]         ; Re-read bits 63:32

  CMP     r2, r12                 ; Have the top bits changed?
  BNE     get_global_timer_count_loop

  ; Store result out to pointers
  STR     r2, [r0]
  STR     r3, [r1]

  BX      lr
  ENDP
  
; ------------------------------------------------------------

  EXPORT clear_global_timer_irq
  ; void clear_global_timer_irq(void)
  ; Clears the global timer interrupt
clear_global_timer_irq PROC
  MRC     p15, 4, r0, c15, c0, 0  ; Read periph base address

  ; Clear the interrupt by writing 0x1 to the Timer's Interrupt Status register
  MOV     r1, #1
  STR     r1, [r0, #0x20C]

  BX      lr
  ENDP

; ------------------------------------------------------------
; End of code
; ------------------------------------------------------------

  END

; ------------------------------------------------------------
; End of MP_GlobalTimer.s
; ------------------------------------------------------------
