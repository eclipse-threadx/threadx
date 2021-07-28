// ------------------------------------------------------------
// Cortex-A MPCore - Snoop Control Unit (SCU)
// Suitable for Cortex-A5 MPCore and Cortex-A9 MPCore
//
// Copyright (c) 2011-2015 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------


  .text


// ------------------------------------------------------------
// Misc
// ------------------------------------------------------------

  .global getNumCPUs
  // uint32_t getNumCPUs(void)
  // Returns the number of CPUs in the Cluster
  .type getNumCPUs, "function"
getNumCPUs:

  // Get base address of private peripheral space
  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address

  LDR     r0, [r0, #0x004]        // Read SCU Configuration register
  AND     r0, r0, #0x3            // Bits 1:0 gives the number of cores-1
  ADD     r0, r0, #1
  BX      lr


// ------------------------------------------------------------
// SCU
// ------------------------------------------------------------

  // SCU offset from base of private peripheral space --> 0x000

  .global enableSCU
  // void enableSCU(void)
  // Enables the SCU
  .type enableSCU, "function"
enableSCU:

  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address

  LDR     r1, [r0, #0x0]          // Read the SCU Control Register
  ORR     r1, r1, #0x1            // Set bit 0 (The Enable bit)
  STR     r1, [r0, #0x0]          // Write back modifed value

  BX      lr


// ------------------------------------------------------------

  .global getCPUsInSMP
  // uint32_t getCPUsInSMP(void)
  // The return value is 1 bit per core:
  // bit 0 - CPU 0
  // bit 1 - CPU 1
  // etc...
  .type getCPUsInSMP, "function"
getCPUsInSMP:

  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address

  LDR     r0, [r0, #0x004]        // Read SCU Configuration register
  MOV     r0, r0, LSR #4          // Bits 7:4 gives the cores in SMP mode, shift then mask
  AND     r0, r0, #0x0F

  BX      lr


// ------------------------------------------------------------

  .global enableMaintenanceBroadcast
  // void enableMaintenanceBroadcast(void)
  // Enable the broadcasting of cache & TLB maintenance operations
  // When enabled AND in SMP, broadcast all "inner sharable"
  // cache and TLM maintenance operations to other SMP cores
  .type enableMaintenanceBroadcast, "function"
enableMaintenanceBroadcast:
  MRC     p15, 0, r0, c1, c0, 1   // Read Aux Ctrl register
  MOV     r1, r0
  ORR     r0, r0, #0x01           // Set the FW bit (bit 0)
  CMP     r0, r1
  MCRNE   p15, 0, r0, c1, c0, 1   // Write Aux Ctrl register

  BX      lr


// ------------------------------------------------------------

  .global disableMaintenanceBroadcast
  // void disableMaintenanceBroadcast(void)
  // Disable the broadcasting of cache & TLB maintenance operations
  .type disableMaintenanceBroadcast, "function"
disableMaintenanceBroadcast:
  MRC     p15, 0, r0, c1, c0, 1   // Read Aux Ctrl register
  BIC     r0, r0, #0x01           // Clear the FW bit (bit 0)
  MCR     p15, 0, r0, c1, c0, 1   // Write Aux Ctrl register

  BX      lr


// ------------------------------------------------------------

  .global secureSCUInvalidate
  // void secureSCUInvalidate(uint32_t cpu, uint32_t ways)
  // cpu: 0x0=CPU 0 0x1=CPU 1 etc...
  // This function invalidates the SCU copy of the tag rams
  // for the specified core.  Typically only done at start-up.
  // Possible flow:
  // - Invalidate L1 caches
  // - Invalidate SCU copy of TAG RAMs
  // - Join SMP
  .type secureSCUInvalidate, "function"
secureSCUInvalidate:
  AND     r0, r0, #0x03           // Mask off unused bits of CPU ID
  MOV     r0, r0, LSL #2          // Convert into bit offset (four bits per core)

  AND     r1, r1, #0x0F           // Mask off unused bits of ways
  MOV     r1, r1, LSL r0          // Shift ways into the correct CPU field

  MRC     p15, 4, r2, c15, c0, 0  // Read periph base address

  STR     r1, [r2, #0x0C]         // Write to SCU Invalidate All in Secure State

  BX      lr



// ------------------------------------------------------------
// TrustZone
// ------------------------------------------------------------

  .global setPrivateTimersNonSecureAccess
  // void setPrivateTimersNonSecureAccess(uint32_t secure, uint32_t cpu)
  // Sets whether the Private Timer & Watchdog can be accessed in NS world
  // r0 -  IF 0 (secure access only) ELSE (ns access allowed)
  .type setPrivateTimersNonSecureAccess, "function"
setPrivateTimersNonSecureAccess:
  AND     r0, r0, #0x01           // Mask
  ADD     r1, r1, #0x04           // Adjust r1, as field starts at bit 4
  MOV     r0, r0, LSL r1          // Shift bit into correct position for CPU

  MOV     r12, #1
  MOV     r12, r12, LSL r1        // Form a mask to clear existing bit value

  MRC     p15, 4, r2, c15, c0, 0  // Read periph base address

  LDR     r3, [r2, #0x54]         // Read SCU Secure Access Control (SSAC) register
  BIC     r3, r3, r12             // Clear current value
  ORR     r3, r3, r0              // Set to specified value
  STR     r3, [r2, #0x54]         // Write SCU Secure Access Control (SSAC) register

  BX      lr


// ------------------------------------------------------------

  .global setGlobalTimerNonSecureAccess
  // void setGlobalTimerNonSecureAccess(uint32_t secure, uint32_t cpu)
  // Sets whether the Global Timer can be accessed in NS world
  // r0 -  IF 0 (secure access only) ELSE (ns access allowed)
  .type setGlobalTimerNonSecureAccess, "function"
setGlobalTimerNonSecureAccess:
  AND     r0, r0, #0x01           // Mask
  ADD     r1, r1, #0x08           // Adjust r1, as field starts at bit 8
  MOV     r0, r0, LSL r1          // Shift bit into correct position for CPU

  MOV     r12, #1
  MOV     r12, r12, LSL r1        // Form a mask to clear existing bit value

  MRC     p15, 4, r2, c15, c0, 0  // Read periph base address

  LDR     r3, [r2, #0x54]         // Read SCU Secure Access Control (SSAC) register
  BIC     r3, r3, r12             // Clear current value
  ORR     r3, r3, r0              // Set to specified value
  STR     r3, [r2, #0x54]         // Write SCU Secure Access Control (SSAC) register

  BX      lr


// ------------------------------------------------------------
// End of MP_SCU.s
// ------------------------------------------------------------
