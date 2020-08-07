; ------------------------------------------------------------
; Cortex-A MPCore - Private timer functions
;
; Copyright ARM Ltd 2009. All rights reserved.
; ------------------------------------------------------------

  PRESERVE8

  AREA  MP_PrivateTimer, CODE, READONLY

  ; PPI ID 29


  ; Typical set of calls to enable Timer:
  ; init_private_timer(0xXXXX, 0)   <-- Counter down value of 0xXXXX, with auto-reload
  ; start_private_timer()

  ; Timer offset from base of private peripheral space --> 0x600

; ------------------------------------------------------------

  EXPORT init_private_timer
  ; void init_private_timer(unsigned int load_value, unsigned int auto_reload)
  ; Sets up the private timer
  ; r0: initial load value
  ; r1:  IF 0 (AutoReload) ELSE (SingleShot)
init_private_timer PROC

  ; Get base address of private perpherial space
  MOV     r2, r0                  ; Make a copy of r0 before corrupting
  MRC     p15, 4, r0, c15, c0, 0  ; Read periph base address

  ; Set the load value
  STR     r2, [r0, #0x600]

  ; Control register bit layout
  ; Bit 0 - Enable
  ; Bit 1 - Auto-Reload           ; see DE681117
  ; Bit 2 - IRQ Generation

  ; Form control reg value
  CMP     r1, #0                  ; Check whether to enable auto-reload
  MOVNE   r2, #0x04               ; No auto-reload
  MOVEQ   r2, #0x06               ; With auto-reload

  ; Store to control register
  STR     r2, [r0, #0x608]

  BX      lr
  ENDP

; ------------------------------------------------------------

  EXPORT start_private_timer
  ; void start_private_timer(void)
  ; Starts the private timer
start_private_timer PROC

  MRC     p15, 4, r0, c15, c0, 0  ; Read periph base address

  LDR     r1, [r0, #0x608]        ; Read control reg
  ORR     r1, r1, #0x01           ; Set enable bit
  STR     r1, [r0, #0x608]        ; Write modified value back

  BX      lr
  ENDP

; ------------------------------------------------------------

  EXPORT stop_private_timer
  ; void stop_private_timer(void)
  ; Stops the private timer
stop_private_timer PROC

  MRC     p15, 4, r0, c15, c0, 0  ; Read periph base address

  LDR     r1, [r0, #0x608]        ; Read control reg
  BIC     r1, r1, #0x01           ; Clear enable bit
  STR     r1, [r0, #0x608]        ; Write modified value back

  BX      lr
  ENDP

; ------------------------------------------------------------

  EXPORT get_private_timer_count
  ; unsigned int read_private_timer(void)
  ; Reads the current value of the timer count register
get_private_timer_count PROC

  MRC     p15, 4, r0, c15, c0, 0  ; Read periph base address

  LDR     r0, [r0, #0x604]        ; Read count register

  BX      lr
  ENDP
  
; ------------------------------------------------------------

  EXPORT clear_private_timer_irq
  ; void clear_private_timer_irq(void)
  ; Clears the private timer interrupt
clear_private_timer_irq PROC
  MRC     p15, 4, r0, c15, c0, 0  ; Read periph base address

  ; Clear the interrupt by writing 0x1 to the Timer's Interrupt Status register
  MOV     r1, #1
  STR     r1, [r0, #0x60C]

  BX      lr
  ENDP

; ------------------------------------------------------------
; End of code
; ------------------------------------------------------------

  END

; ------------------------------------------------------------
; End of MP_PrivateTimer.s
; ------------------------------------------------------------
