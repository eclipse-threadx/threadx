// ------------------------------------------------------------
// SP804 Dual Timer
//
// Copyright (c) 2009-2017 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------

#include "sp804_timer.h"

#define TIMER_SP804_CTRL_TIMEREN     (1 << 7)
#define TIMER_SP804_CTRL_TIMERMODE   (1 << 6)        // Bit 6:
#define TIMER_SP804_CTRL_INTENABLE   (1 << 5)
#define TIMER_SP804_CTRL_TIMERSIZE   (1 << 1)        // Bit 1: 0=16-bit, 1=32-bit
#define TIMER_SP804_CTRL_ONESHOT     (1 << 0)        // Bit 0: 0=wrapping, 1=one-shot

#define TIMER_SP804_CTRL_PRESCALE_1  (0 << 2)        // clk/1
#define TIMER_SP804_CTRL_PRESCALE_4  (1 << 2)        // clk/4
#define TIMER_SP804_CTRL_PRESCALE_8  (2 << 2)        // clk/8

struct sp804_timer
{
        volatile uint32_t Time1Load;     // +0x00
  const volatile uint32_t Time1Value;    // +0x04 - RO
        volatile uint32_t Timer1Control; // +0x08
        volatile uint32_t Timer1IntClr;  // +0x0C - WO
  const volatile uint32_t Timer1RIS;     // +0x10 - RO
  const volatile uint32_t Timer1MIS;     // +0x14 - RO
        volatile uint32_t Timer1BGLoad;  // +0x18

        volatile uint32_t Time2Load;     // +0x20
        volatile uint32_t Time2Value;    // +0x24
        volatile uint8_t  Timer2Control; // +0x28
        volatile uint32_t Timer2IntClr;  // +0x2C - WO
  const volatile uint32_t Timer2RIS;     // +0x30 - RO
  const volatile uint32_t Timer2MIS;     // +0x34 - RO
        volatile uint32_t Timer2BGLoad;  // +0x38

  // Not including ID registers

};

// Instance of the dual timer, will be placed using the scatter file
struct sp804_timer* dual_timer;


// Set base address of timer
// address - virtual address of SP804 timer
void setTimerBaseAddress(uint64_t address)
{
  dual_timer = (struct sp804_timer*)address;
  return;
}


// Sets up the private timer
// load_value  - Initial value of timer
// auto_reload - Periodic (SP804_AUTORELOAD) or one shot (SP804_SINGLESHOT)
// interrupt   - Whether to generate an interrupt
void initTimer(uint32_t load_value, uint32_t auto_reload, uint32_t interrupt)
{
  uint32_t tmp = 0;

  dual_timer->Time1Load = load_value;

  // Fixed setting: 32-bit, no prescaling
  tmp = TIMER_SP804_CTRL_TIMERSIZE | TIMER_SP804_CTRL_PRESCALE_1 | TIMER_SP804_CTRL_TIMERMODE;

  // Settings from parameters: interrupt generation & reload
  tmp = tmp | interrupt | auto_reload;

  // Write control register
  dual_timer->Timer1Control = tmp;

  return;
}


// Starts the timer
void startTimer(void)
{
  uint32_t tmp;

  tmp = dual_timer->Timer1Control;
  tmp = tmp | TIMER_SP804_CTRL_TIMEREN;      // Set TimerEn (bit 7)
  dual_timer->Timer1Control = tmp;

  return;
}


// Stops the timer
void stopTimer(void)
{
  uint32_t tmp;

  tmp = dual_timer->Timer1Control;
  tmp = tmp & ~TIMER_SP804_CTRL_TIMEREN;     // Clear TimerEn (bit 7)
  dual_timer->Timer1Control = tmp;

  return;
}


// Returns the current timer count
uint32_t getTimerCount(void)
{
  return dual_timer->Time1Value;
}


void clearTimerIrq(void)
{
  // A write to this register, of any value, clears the interrupt
  dual_timer->Timer1IntClr = 1;
}


// ------------------------------------------------------------
// End of sp804_timer.c
// ------------------------------------------------------------
