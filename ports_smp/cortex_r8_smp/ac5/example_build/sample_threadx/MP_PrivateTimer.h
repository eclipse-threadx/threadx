// ------------------------------------------------------------
// Cortex-R MPCore - Private timer functions
// Header Filer
//
// Copyright ARM Ltd 2009. All rights reserved.
// ------------------------------------------------------------

#ifndef _CORTEXA_PRIVATE_TIMER_
#define _CORTEXA_PRIVATE_TIMER_

// Typical set of calls to enable Timer:
// init_private_timer(0xXXXX, 0)   <-- Counter down value of 0xXXXX, with auto-reload
// start_private_timer()

// Sets up the private timer
// r0: initial load value
// r1:  IF 0 (AutoReload) ELSE (SingleShot)
void init_private_timer(unsigned int load_value, unsigned int auto_reload);

// Starts the private timer
void start_private_timer(void);

// Stops the private timer
void stop_private_timer(void);

// Reads the current value of the timer count register
unsigned int get_private_timer_count(void);

// Clears the private timer interrupt
void clear_private_timer_irq(void);

#endif

// ------------------------------------------------------------
// End of MP_PrivateTimer.h
// ------------------------------------------------------------
