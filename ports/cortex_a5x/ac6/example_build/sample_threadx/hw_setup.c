//
// Copyright ARM LTD, 2013

#include <stdio.h>
#include "gic400_gic.h"
#include "armv8_aarch64_SystemTimer.h"
#include "tx_api.h"


void _tx_timer_interrupt(void);



// --------------------------------------------------------

void hw_setup(void)
{
  long long current_time;


  setGICAddr((void*)0x2C001000, (void*)0x2C002000);

  //
  // Configure interrupt controller (GICv2 assumed)
  //
  enableGIC();
  enableCPUInterface();
  enableNonSecureCPUInterface();
  setPriorityMask(0xFF);
  enableSecureFIQs();

  // Secure Physical Timer      (ID 29)
  enableIntID(29);
  setIntPriority(29, 0);
  makeIntGroup0(29);

  // Non-Secure Physical Timer  (ID 30)
  enableIntID(30);
  setIntPriority(30, 0);
  makeIntGroup1(30);


  //
  // Configure timer
  //
  configSecureEL1TimerAccess(SCR_ENABLE_SECURE_EL1_ACCESS);

  current_time = getPhysicalCount();

  // Configure the Secure Physical Timer
  setSEL1PhysicalCompValue(current_time + 10000);
  setSEL1PhysicalTimerCtrl(CNTPS_CTL_ENABLE);

  // Configure the Non-Secure Physical Timer
  setNSEL1PhysicalCompValue(current_time + 20000);
  setNSEL1PhysicalTimerCtrl(CNTP_CTL_ENABLE);


  // NOTE:
  // This code assumes that the IRQ and FIQ exceptions
  // have been routed to the appropriate EL.  In this 
  // example that is done in the startup.s file
}


// --------------------------------------------------------

void fiqHandler(void)
{
  unsigned int ID;

//  printf("Hello from the FIQ handler\n");

  ID = readIntAck();
  setSEL1PhysicalTimerCtrl(0);  // Disable timer to clear interrupt
  writeEOI(ID);

  return;
}

// --------------------------------------------------------

void irqHandler(void)
{
  unsigned int ID;
  unsigned long long current_time;

//  printf("Hello from the IRQ handler\n");

  ID = readAliasedIntAck();
  setNSEL1PhysicalTimerCtrl(0);  // Disable timer to clear interrupt
  writeAliasedEOI(ID);

  current_time = getPhysicalCount();
  setNSEL1PhysicalCompValue(current_time + 20000);
  setNSEL1PhysicalTimerCtrl(CNTP_CTL_ENABLE);

  //tx_thread_resume(&thread_1);
  _tx_timer_interrupt();

  return;
}

// --------------------------------------------------------
