// ------------------------------------------------------------
// Cortex-A9 MPCore - Snoop Control Unit
// Header File
//
// Copyright (c) 2011-2018 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------

#ifndef _CORTEXA_SCU_
#define _CORTEXA_SCU_

// ------------------------------------------------------------
// Misc
// ------------------------------------------------------------

// Returns the number of cores in the cluster
// This is the format of the register, decided to leave it unchanged.
unsigned int getNumCPUs(void);

// Go to sleep, never returns
void goToSleep(void);

// ------------------------------------------------------------
// SCU
// ------------------------------------------------------------

// Enables the SCU
void enableSCU(void);

// The return value is 1 bit per core:
// bit 0 - CPU 0
// bit 1 - CPU 1
// etc...
unsigned int getCPUsInSMP(void);

 //Enable the broadcasting of cache & TLB maintenance operations
// When enabled AND in SMP, broadcast all "inner sharable"
// cache and TLM maintenance operations to other SMP cores
void enableMaintenanceBroadcast(void);

// Disable the broadcasting of cache & TLB maintenance operations
void disableMaintenanceBroadcast(void);

// cpu: 0x0=CPU 0 0x1=CPU 1 etc...
// This function invalidates the SCU copy of the tag rams
// for the specified core.
void secureSCUInvalidate(unsigned int cpu, unsigned int ways);

#endif

// ------------------------------------------------------------
// End of MP_SCU.h
// ------------------------------------------------------------
