// ------------------------------------------------------------
// Cortex-A8 MPCore - Interrupt Controller functions
// Header File
//
// Copyright (c) 2011-2018 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------

#ifndef _CORTEXA_GIC_
#define _CORTEXA_GIC_

// ------------------------------------------------------------
// GIC
// ------------------------------------------------------------

// Typical calls to enable interrupt ID X:
// enable_irq_id(X)                 <-- Enable that ID
// set_irq_priority(X, 0)           <-- Set the priority of X to 0 (the max priority)
// set_priority_mask(0x1F)          <-- Set Core's priority mask to 0x1F (the lowest priority)
// enable_GIC()                     <-- Enable the GIC (global)
// enable_gic_processor_interface() <-- Enable the CPU interface (local to the core)
//


//  Global enable of the Interrupt Distributor
void enableGIC(void);

// Global disable of the Interrupt Distributor
void disableGIC(void);

// Enables the interrupt source number ID
void enableIntID(unsigned int ID);

// Disables the interrupt source number ID
void disableIntID(unsigned int ID);

// Sets the priority of the specified ID
void setIntPriority(unsigned int ID, unsigned int priority);

// Enables the processor interface
// Must be done on each core separately
void enableGICProcessorInterface(void);

// Disables the processor interface
// Must be done on each core separately
void disableGICProcessorInterface(void);

// Sets the Priority mask register for the core run on
// The reset value masks ALL interrupts!
void setPriorityMask(unsigned int priority);

// Sets the Binary Point Register for the core run on
void setBinaryPoint(unsigned int priority);

//  Returns the value of the Interrupt Acknowledge Register
unsigned int readIntAck(void);

// Writes ID to the End Of Interrupt register
void writeEOI(unsigned int ID);

// ------------------------------------------------------------
// SGI
// ------------------------------------------------------------

// Send a software generate interrupt
void sendSGI(unsigned int ID, unsigned int core_list, unsigned int filter_list);

#endif

// ------------------------------------------------------------
// End of MP_GIC.h
// ------------------------------------------------------------
