// ------------------------------------------------------------
// SP804 Dual Timer
// Header Filer
//
// Copyright (c) 2009-2017 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------

#ifndef _SP804_TIMER_
#define _SP804_TIMER_

#include <stdint.h>

// Set base address of timer
// address - virtual address of SP804 timer
void setTimerBaseAddress(uint64_t address);


// Sets up the private timer
// load_value  - Initial value of timer
// auto_reload - Periodic (SP804_AUTORELOAD) or one shot (SP804_SINGLESHOT)
// interrupt   - Whether to generate an interrupt

#define SP804_AUTORELOAD   (0)
#define SP804_SINGLESHOT   (1)
#define SP804_GENERATE_IRQ (1 << 5)
#define SP804_NO_IRQ       (0)

void initTimer(uint32_t load_value, uint32_t auto_reload, uint32_t interrupt);


// Starts the timer
void startTimer(void);


// Stops the timer
void stopTimer(void);


// Returns the current timer count
uint32_t getTimerCount(void);


// Clears the timer interrupt
void clearTimerIrq(void);

#endif

// ------------------------------------------------------------
// End of sp804_timer.h
// ------------------------------------------------------------
