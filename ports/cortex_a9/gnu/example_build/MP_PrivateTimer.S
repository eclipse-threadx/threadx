// ------------------------------------------------------------
// Cortex-A MPCore - Private timer functions
//
// Copyright ARM Ltd 2009. All rights reserved.
// ------------------------------------------------------------

  .text
  .align 4

  // PPI ID 29


  // Typical set of calls to enable Timer:
  // init_private_timer(0xXXXX, 0)   <-- Counter down value of 0xXXXX, with auto-reload
  // start_private_timer()

  // Timer offset from base of private peripheral space --> 0x600

// ------------------------------------------------------------

  .global init_private_timer
  .type init_private_timer,function
  // void init_private_timer(unsigned int load_value, unsigned int auto_reload)
  // Sets up the private timer
  // r0: initial load value
  // r1:  IF 0 (AutoReload) ELSE (SingleShot)
init_private_timer:

  // Get base address of private perpherial space
  MOV     r2, r0                  // Make a copy of r0 before corrupting
  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address

  // Set the load value
  STR     r2, [r0, #0x600]

  // Control register bit layout
  // Bit 0 - Enable
  // Bit 1 - Auto-Reload           // see DE681117
  // Bit 2 - IRQ Generation

  // Form control reg value
  CMP     r1, #0                  // Check whether to enable auto-reload
  MOVNE   r2, #0x04               // No auto-reload
  MOVEQ   r2, #0x06               // With auto-reload

  // Store to control register
  STR     r2, [r0, #0x608]

  BX      lr

// ------------------------------------------------------------

  // void start_private_timer(void)
  // Starts the private timer
  .global start_private_timer
  .type start_private_timer,function
start_private_timer:

  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address

  LDR     r1, [r0, #0x608]        // Read control reg
  ORR     r1, r1, #0x01           // Set enable bit
  STR     r1, [r0, #0x608]        // Write modified value back

  BX      lr

// ------------------------------------------------------------

  // void stop_private_timer(void)
  // Stops the private timer
  .global stop_private_timer
  .type stop_private_timer,function
stop_private_timer:

  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address

  LDR     r1, [r0, #0x608]        // Read control reg
  BIC     r1, r1, #0x01           // Clear enable bit
  STR     r1, [r0, #0x608]        // Write modified value back

  BX      lr

// ------------------------------------------------------------

  // unsigned int read_private_timer(void)
  // Reads the current value of the timer count register
  .global get_private_timer_count
  .type get_private_timer_count,function
get_private_timer_count:

  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address

  LDR     r0, [r0, #0x604]        // Read count register

  BX      lr
  
// ------------------------------------------------------------

  // void clear_private_timer_irq(void)
  // Clears the private timer interrupt
  .global clear_private_timer_irq
  .type clear_private_timer_irq,function
clear_private_timer_irq:
  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address

  // Clear the interrupt by writing 0x1 to the Timer's Interrupt Status register
  MOV     r1, #1
  STR     r1, [r0, #0x60C]

  BX      lr

// ------------------------------------------------------------
// End of code
// ------------------------------------------------------------

// ------------------------------------------------------------
// End of MP_PrivateTimer.s
// ------------------------------------------------------------
