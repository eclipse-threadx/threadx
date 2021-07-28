/* Bare-metal example for Armv8-A FVP Base model */

/* Timer and interrupts */

/* Copyright (c) 2016-2018 Arm Limited (or its affiliates). All rights reserved. */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */

#include <stdio.h>

#include "GICv3.h"
#include "GICv3_gicc.h"
#include "sp804_timer.h"

void   _tx_timer_interrupt(void);

// LED Base address
#define LED_BASE (volatile unsigned int *)0x1C010008


void nudge_leds(void) // Move LEDs along
{
    static int state = 1;
    static int value = 1;

    if (state)
    {
        int max = (1 << 7);
        value <<= 1;
        if (value == max)
            state = 0;
    }
    else
    {
        value >>= 1;
        if (value == 1)
            state = 1;
    }

    *LED_BASE = value;  // Update LEDs hardware
}


// Initialize Timer 0 and Interrupt Controller
void init_timer(void)
{
    // Enable interrupts
    __asm("MSR DAIFClr, #0xF");
    setICC_IGRPEN1_EL1(igrpEnable);

    // Configure the SP804 timer to generate an interrupt
    setTimerBaseAddress(0x1C110000);
    initTimer(0x200, SP804_AUTORELOAD, SP804_GENERATE_IRQ);
    startTimer();

    // The SP804 timer generates SPI INTID 34.  Enable
    // this ID, and route it to core 0.0.0.0 (this one!)
    SetSPIRoute(34, 0, gicdirouter_ModeSpecific);    // Route INTID 34 to 0.0.0.0 (this core)
    SetSPIPriority(34, 0);                           // Set INTID 34 to priority to 0
    ConfigureSPI(34, gicdicfgr_Level);               // Set INTID 34 as level-sensitive
    EnableSPI(34);                                   // Enable INTID 34
}


// --------------------------------------------------------

void irqHandler(void)
{
  unsigned int ID;

  ID = getICC_IAR1(); // readIntAck();

  // Check for reserved IDs
  if ((1020 <= ID) && (ID <= 1023))
  {
      //printf("irqHandler() - Reserved INTID %d\n\n", ID);
      return;
  }

  switch(ID)
  {
    case 34:
      // Dual-Timer 0 (SP804)
      //printf("irqHandler() - External timer interrupt\n\n");
      nudge_leds();
      clearTimerIrq();

      /* Call ThreadX timer interrupt processing.  */
      _tx_timer_interrupt();

      break;

    default:
      // Unexpected ID value
      //printf("irqHandler() - Unexpected INTID %d\n\n", ID);
      break;
  }

  // Write the End of Interrupt register to tell the GIC
  // we've finished handling the interrupt
  setICC_EOIR1(ID); // writeAliasedEOI(ID);
}

// --------------------------------------------------------

// Not actually used in this example, but provided for completeness

void fiqHandler(void)
{
  unsigned int ID;
  unsigned int aliased = 0;

  ID = getICC_IAR0(); // readIntAck();
  //printf("fiqHandler() - Read %d from IAR0\n", ID);

  // Check for reserved IDs
  if ((1020 <= ID) && (ID <= 1023))
  {
    //printf("fiqHandler() - Reserved INTID %d\n\n", ID);
    ID = getICC_IAR1(); // readAliasedIntAck();
    //printf("fiqHandler() - Read %d from AIAR\n", ID);
    aliased = 1;

    // If still spurious then simply return
    if ((1020 <= ID) && (ID <= 1023))
        return;
  }

  switch(ID)
  {
    case 34:
      // Dual-Timer 0 (SP804)
      //printf("fiqHandler() - External timer interrupt\n\n");
      clearTimerIrq();
      break;

    default:
      // Unexpected ID value
      //printf("fiqHandler() - Unexpected INTID %d\n\n", ID);
      break;
  }

  // Write the End of Interrupt register to tell the GIC
  // we've finished handling the interrupt
  // NOTE: If the ID was read from the Aliased IAR, then
  // the aliased EOI register must be used
  if (aliased == 0)
    setICC_EOIR0(ID); // writeEOI(ID);
  else
    setICC_EOIR1(ID); // writeAliasedEOI(ID);
}
