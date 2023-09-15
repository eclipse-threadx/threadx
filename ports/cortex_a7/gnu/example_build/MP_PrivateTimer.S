// ------------------------------------------------------------
// Cortex-A MPCore - Private timer functions
//
// Copyright ARM Ltd 2009. All rights reserved.
// ------------------------------------------------------------

  .text
  .align 3

  // PPI ID 29


  // Typical set of calls to enable Timer:
  // init_private_timer(0xXXXX, 0)   <-- Counter down value of 0xXXXX, with auto-reload
  // start_private_timer()

  // Timer offset from base of private peripheral space --> 0x600

// ------------------------------------------------------------

  // void init_private_timer(unsigned int load_value, unsigned int auto_reload)
  // Sets up the private timer
  // r0: initial load value
  // r1:  IF 0 (AutoReload) ELSE (SingleShot) AutoReload not supported on Cortex-A7
  .global init_private_timer
  .type init_private_timer,function
init_private_timer:

  // Setup timeout value (CNTP_TVAL)
  MCR     p15, 0, r0, c14, c2, 0
  BX      lr

// ------------------------------------------------------------

  // void start_private_timer(void)
  // Starts the private timer
  .global start_private_timer
  .type start_private_timer,function
start_private_timer:

  MOV     r0, #0x1

  // Enable timer (CNTP_CTL)
  MCR     p15, 0, r0, c14, c2, 1

  BX      lr

// ------------------------------------------------------------

  // void stop_private_timer(void)
  // Stops the private timer
  .global stop_private_timer
  .type stop_private_timer,function
stop_private_timer:

  BX      lr

// ------------------------------------------------------------

  // unsigned int read_private_timer(void)
  // Reads the current value of the timer count register
  .global get_private_timer_count
  .type get_private_timer_count,function
get_private_timer_count:

  BX      lr
  
// ------------------------------------------------------------

  // void clear_private_timer_irq(void)
  // Clears the private timer interrupt
  .global clear_private_timer_irq
  .type clear_private_timer_irq,function
clear_private_timer_irq:

  BX      lr

// ------------------------------------------------------------
// End of code
// ------------------------------------------------------------

// ------------------------------------------------------------
// End of MP_PrivateTimer.s
// ------------------------------------------------------------
