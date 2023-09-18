// ------------------------------------------------------------
// Cortex-A MPCore - Interrupt Controller functions
//
// Copyright (c) 2011-2018 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------


  .text
  .cfi_sections .debug_frame  // put stack frame info into .debug_frame instead of .eh_frame


// ------------------------------------------------------------
// GIC
// ------------------------------------------------------------

  // CPU Interface offset from base of private peripheral space --> 0x0100
  // Interrupt Distributor offset from base of private peripheral space --> 0x1000

  // Typical calls to enable interrupt ID X:
  // disableIntID(X)               <-- Disable that ID
  // setIntPriority(X, 0)          <-- Set the priority of X to 0 (the max priority)
  // setPriorityMask(0x1F)         <-- Set CPU's priority mask to 0x1F (the lowest priority)
  // enableGIC()                   <-- Enable the GIC (global)
  // enableGICProcessorInterface() <-- Enable the CPU interface (local to the CPU)


  .global enableGIC
  // void enableGIC(void)
  // Global enable of the Interrupt Distributor
  .type enableGIC, "function"
  .cfi_startproc
enableGIC:

  // Get base address of private peripheral space
  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address
  ADD     r0, r0, #0x1000         // Add the GIC offset

  LDR     r1, [r0]                // Read the GIC Enable Register  (ICDDCR)
  ORR     r1, r1, #0x01           // Set bit 0, the enable bit
  STR     r1, [r0]                // Write the GIC Enable Register  (ICDDCR)

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global disableGIC
  // void disableGIC(void)
  // Global disable of the Interrupt Distributor
  .type disableGIC, "function"
  .cfi_startproc
disableGIC:

  // Get base address of private peripheral space
  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address
  ADD     r0, r0, #0x1000         // Add the GIC offset

  LDR     r1, [r0]                // Read the GIC Enable Register  (ICDDCR)
  BIC     r1, r1, #0x01           // Clear bit 0, the enable bit
  STR     r1, [r0]                // Write the GIC Enable Register  (ICDDCR)

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global enableIntID
  // void enableIntID(uint32_t ID)
  // Enables the interrupt source number ID
  .type enableIntID, "function"
  .cfi_startproc
enableIntID:

  // Get base address of private peripheral space
  MOV     r1, r0                  // Back up passed in ID value
  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address

  // Each interrupt source has an enable bit in the GIC.  These
  // are grouped into registers, with 32 sources per register
  // First, we need to identify which 32-bit block the interrupt lives in
  MOV     r2, r1                  // Make working copy of ID in r2
  MOV     r2, r2, LSR #5          // LSR by 5 places, affective divide by 32
                                  // r2 now contains the 32-bit block this ID lives in
  MOV     r2, r2, LSL #2          // Now multiply by 4, to convert offset into an address offset (four bytes per reg)

  // Now work out which bit within the 32-bit block the ID is
  AND     r1, r1, #0x1F           // Mask off to give offset within 32-bit block
  MOV     r3, #1                  // Move enable value into r3
  MOV     r3, r3, LSL r1          // Shift it left to position of ID

  ADD     r2, r2, #0x1100         // Add the base offset of the Enable Set registers to the offset for the ID
  STR     r3, [r0, r2]            // Store out (ICDISER)

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global disableIntID
  // void disableIntID(uint32_t ID)
  // Disables the interrupt source number ID
  .type disableIntID, "function"
  .cfi_startproc
disableIntID:

  // Get base address of private peripheral space
  MOV     r1, r0                  // Back up passed in ID value
  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address

  // First, we need to identify which 32-bit block the interrupt lives in
  MOV     r2, r1                  // Make working copy of ID in r2
  MOV     r2, r2, LSR #5          // LSR by 5 places, affective divide by 32
                                  // r2 now contains the 32-bit block this ID lives in
  MOV     r2, r2, LSL #2          // Now multiply by 4, to convert offset into an address offset (four bytes per reg)

  // Now work out which bit within the 32-bit block the ID is
  AND     r1, r1, #0x1F           // Mask off to give offset within 32-bit block
  MOV     r3, #1                  // Move enable value into r3
  MOV     r3, r3, LSL r1          // Shift it left to position of ID in 32-bit block

  ADD     r2, r2, #0x1180         // Add the base offset of the Enable Clear registers to the offset for the ID
  STR     r3, [r0, r2]            // Store out (ICDICER)

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global setIntPriority
  // void setIntPriority(uint32_t ID, uint32_t priority)
  // Sets the priority of the specified ID
  // r0 = ID
  // r1 = priority
  .type setIntPriority, "function"
  .cfi_startproc
setIntPriority:

  // Get base address of private peripheral space
  MOV     r2, r0                  // Back up passed in ID value
  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address

  // r0 = base addr
  // r1 = priority
  // r2 = ID

  // Make sure that priority value is only 5 bits, and convert to expected format
  AND     r1, r1, #0x1F
  MOV     r1, r1, LSL #3

  // Find which register this ID lives in
  BIC     r3, r2, #0x03           // Make a copy of the ID, clearing off the bottom two bits
                                  // There are four IDs per reg, by clearing the bottom two bits we get an address offset
  ADD     r3, r3, #0x1400         // Now add the offset of the Priority Level registers from the base of the private peripheral space
  ADD     r0, r0, r3              // Now add in the base address of the private peripheral space, giving us the absolute address


  // Now work out which ID in the register it is
  AND     r2, r2, #0x03           // Clear all but the bottom two bits, leaves which ID in the reg it is (which byte)
  MOV     r2, r2, LSL #3          // Multiply by 8, this gives a bit offset

  // Read -> Modify -> Write
  MOV     r12, #0xFF              // 8 bit field mask
  MOV     r12, r12, LSL r2        // Move mask into correct bit position
  MOV     r1, r1, LSL r2          // Also, move passed in priority value into correct bit position


  LDR     r3, [r0]                // Read current value of the Priority Level register
  BIC     r3, r3, r12             // Clear appropriate field
  ORR     r3, r3, r1              // Now OR in the priority value
  STR     r3, [r0]                // And store it back again (ICDIPR)

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global getIntPriority
  // uint32_t getIntPriority(void)
  // Returns the priority of the specified ID
  .type getIntPriority, "function"
  .cfi_startproc
getIntPriority:

  // TBD

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global setIntTarget
  // void setIntTarget(uint32_t ID, uint32_t target)
  // Sets the target CPUs of the specified ID
  .type setIntTarget, "function"
  .cfi_startproc
setIntTarget:

  // Get base address of private peripheral space
  MRC     p15, 4, r2, c15, c0, 0  // Read periph base address

  // r0 = ID
  // r1 = target
  // r2 = base addr

  // Clear unused bits
  AND     r1, r1, #0xF

  // Find which register this ID lives in
  BIC     r3, r0, #0x03           // Make a copy of the ID, clearing the bottom 2 bits
                                  // There are four IDs per reg, by clearing the bottom two bits we get an address offset
  ADD     r3, r3, #0x1800         // Now add the offset of the Target registers from the base of the private peripheral space
  ADD     r2, r2, r3              // Now add in the base address of the private peripheral space, giving us the absolute address

  // Now work out which ID in the register it is
  AND     r0, r0, #0x03           // Clear all but the bottom two bits, leaves which ID in the reg it is (which byte)
  MOV     r0, r0, LSL #3          // Multiply by 8, this gives a bit offset

  // Read -> Modify -> Write
  MOV     r12, #0xFF              // 8 bit field mask
  MOV     r12, r12, LSL r0        // Move mask into correct bit position
  MOV     r1, r1, LSL r0          // Also, move passed in target value into correct bit position

  LDR     r3, [r2]                // Read current value of the Target register
  BIC     r3, r3, r12             // Clear appropriate field
  ORR     r3, r3, r1              // Now OR in the target value
  STR     r3, [r2]                // And store it back again

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global getIntTarget
  // uint32_t getIntTarget(uint32_t ID)
  // Returns the target CPUs of the specified ID
  .type getIntTarget, "function"
  .cfi_startproc
getIntTarget:

  // TBD

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global enableGICProcessorInterface
  // void enableGICProcessorInterface(void)
  // Enables the processor interface
  // Must be done on each core separately
  .type enableGICProcessorInterface, "function"
  .cfi_startproc
enableGICProcessorInterface:

  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address

  LDR     r1, [r0, #0x100]        // Read the Processor Interface Control register (ICCICR/ICPICR)
  ORR     r1, r1, #0x03           // Bit 0: Enables secure interrupts, Bit 1: Enables Non-Secure interrupts
  STR     r1, [r0, #0x100]        // Write the Processor Interface Control register (ICCICR/ICPICR)

  BX      lr
  .cfi_endproc



// ------------------------------------------------------------

  .global disableGICProcessorInterface
  // void disableGICProcessorInterface(void)
  // Disables the processor interface
  // Must be done on each core separately
  .type disableGICProcessorInterface, "function"
  .cfi_startproc
disableGICProcessorInterface:

  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address

  LDR     r1, [r0, #0x100]        // Read the Processor Interface Control register (ICCICR/ICPICR)
  BIC     r1, r1, #0x03           // Bit 0: Enables secure interrupts, Bit 1: Enables Non-Secure interrupts
  STR     r1, [r0, #0x100]        // Write the Processor Interface Control register (ICCICR/ICPICR)

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global setPriorityMask
  // void setPriorityMask(uint32_t priority)
  // Sets the Priority mask register for the CPU run on
  // The reset value masks ALL interrupts!
  .type setPriorityMask, "function"
  .cfi_startproc
setPriorityMask:
  MRC     p15, 4, r1, c15, c0, 0  // Read periph base address

  STR     r0, [r1, #0x0104]       // Write the Priority Mask register

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global setBinaryPoint
  // void setBinaryPoint(uint32_t priority)
  // Sets the Binary Point Register for the CPU run on
  .type setBinaryPoint, "function"
  .cfi_startproc
setBinaryPoint:
  MRC     p15, 4, r1, c15, c0, 0  // Read periph base address

  STR     r0, [r1, #0x0108]       // Write the Priority Mask register (ICCPMR/ICCIPMR)

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global readIntAck
  // uint32_t readIntAck(void)
  // Returns the value of the Interrupt Acknowledge Register
  .type readIntAck, "function"
  .cfi_startproc
readIntAck:
  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address
  LDR     r0, [r0, #0x010C]       // Read the Interrupt Acknowledge Register
  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global writeEOI
  // void writeEOI(uint32_t ID)
  // Writes ID to the End Of Interrupt register
  .type writeEOI, "function"
  .cfi_startproc
writeEOI:
  MRC     p15, 4, r1, c15, c0, 0  // Read periph base address

  STR     r0, [r1, #0x0110]       // Write ID to the End of Interrupt register

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------
// SGI
// ------------------------------------------------------------

  .global sendSGI
  // void sendSGI(uint32_t ID, uint32_t target_list, uint32_t filter_list)
  // Send a software generate interrupt
  .type sendSGI, "function"
  .cfi_startproc
sendSGI:

  AND     r3, r0, #0x0F           // Mask off unused bits of ID, and move to r3
  AND     r1, r1, #0x0F           // Mask off unused bits of target_filter
  AND     r2, r2, #0x0F           // Mask off unused bits of filter_list

  ORR     r3, r3, r1, LSL #16     // Combine ID and target_filter
  ORR     r3, r3, r2, LSL #24     // and now the filter list

  // Get the address of the GIC
  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address
  ADD     r0, r0, #0x1F00         // Add offset of the sgi_trigger reg

  STR     r3, [r0]                // Write to the Software Generated Interrupt Register (ICDSGIR)

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------
// TrustZone
// ------------------------------------------------------------

  .global enableSecureFIQs
  // void enableSecureFIQs(void)
  // Enables the sending of secure interrupts as FIQs
  .type enableSecureFIQs, "function"
  .cfi_startproc
enableSecureFIQs:

  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address

  LDR     r1, [r0, #0x100]        // Read the Processor Interface Control register
  ORR     r1, r1, #0x08           // Bit 3: Controls whether secure interrupts are signalled as IRQs or FIQs
  STR     r1, [r0, #0x100]        // Write the Processor Interface Control register

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global disableSecureFIQs
  // void disableSecureFIQs(void)
  // Disables the sending of secure interrupts as FIQs
  .type disableSecureFIQs, "function"
  .cfi_startproc
disableSecureFIQs:

  MRC     p15, 4, r0, c15, c0, 0  // Read periph base address

  LDR     r1, [r0, #0x100]        // Read the Processor Interface Control register
  BIC     r1, r1, #0x08           // Bit 3: Controls whether secure interrupts are signalled as IRQs or FIQs
  STR     r1, [r0, #0x100]        // Write the Processor Interface Control register

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global makeIntSecure
  // void makeIntSecure(uint32_t ID)
  // Sets the specified ID as being Secure
  // r0 - ID
  .type makeIntSecure, "function"
  .cfi_startproc
makeIntSecure:

  MRC     p15, 4, r1, c15, c0, 0  // Read periph base address

  // Each interrupt source has a secutiy bit in the GIC.  These
  // are grouped into registers, with 32 sources per register
  // First, we need to identify which 32-bit block the interrupt lives in
  MOV     r2, r0                  // Make working copy of ID in r2
  MOV     r2, r2, LSR #5          // LSR by 5 places, affective divide by 32
                                  // r2 now contains the 32-bit block this ID lives in
  MOV     r2, r2, LSL #2          // Now multiply by 4, to convert offset into an address offset (four bytes per reg)

  // Now work out which bit within the 32-bit block the ID is
  AND     r0, r0, #0x1F           // Mask off to give offset within 32-bit block
  MOV     r3, #1                  // Move enable value into r3
  MOV     r3, r3, LSL r0          // Shift it left to position of ID

  ADD     r2, r2, #0x1080         // Add the base offset of the Interrupt Configuration registers to the offset for the ID

  LDR     r0, [r1, r2]            // Read appropriate Interrupt Configuration
  BIC     r0, r0, r3              // Clear bit (0 = secure)
  STR     r0, [r1, r2]            // Store out

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global makeIntNonSecure
  // void makeIntNonSecure(uint32_t ID)
  // Sets the specified ID as being non-secure
  // r0 - ID
  .type makeIntNonSecure, "function"
  .cfi_startproc
makeIntNonSecure:

  MRC     p15, 4, r1, c15, c0, 0  // Read periph base address

  // Each interrupt source has a secutiy bit in the GIC.  These
  // are grouped into registers, with 32 sources per register
  // First, we need to identify which 32-bit block the interrupt lives in
  MOV     r2, r0                  // Make working copy of ID in r2
  MOV     r2, r2, LSR #5          // LSR by 5 places, affective divide by 32
                                  // r2 now contains the 32-bit block this ID lives in
  MOV     r2, r2, LSL #2          // Now multiply by 4, to convert offset into an address offset (four bytes per reg)

  // Now work out which bit within the 32-bit block the ID is
  AND     r0, r0, #0x1F           // Mask off to give offset within 32-bit block
  MOV     r3, #1                  // Move enable value into r3
  MOV     r3, r3, LSL r0          // Shift it left to position of ID

  ADD     r2, r2, #0x1080         // Add the base offset of the Interrupt Configuration registers to the offset for the ID

  LDR     r0, [r1, r2]            // Read appropriate Interrupt Configuration
  ORR     r0, r0, r3              // Set bit (1 = secure)
  STR     r0, [r1, r2]            // Store out

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global getIntSecurity
  // uint32_t getIntSecurity(uint32_t ID, uint32_t security)
  // Returns the security of the specified ID
  .type getIntSecurity, "function"
  .cfi_startproc
getIntSecurity:

  // TBD

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------
// End of MP_GIC.s
// ------------------------------------------------------------
