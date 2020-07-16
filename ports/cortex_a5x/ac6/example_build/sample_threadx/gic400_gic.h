// ----------------------------------------------------------
// GIC400 - Generic Interrupt Controller
// Header
//
// Martin Weidmann Dec 2011
// ----------------------------------------------------------

#ifndef __gic400_gic_h
#define __gic400_gic_h

#define GIC_GIC400_PPI0         (0)
#define GIC_GIC400_PPI1         (1)
#define GIC_GIC400_PPI2         (2)
#define GIC_GIC400_PPI3         (3)
#define GIC_GIC400_PPI4         (4)
#define GIC_GIC400_PPI5         (5)
#define GIC_GIC400_PPI6         (6)
#define GIC_GIC400_PPI7         (7)
#define GIC_GIC400_PPI8         (8)
#define GIC_GIC400_PPI9         (9)
#define GIC_GIC400_PPI10        (10)
#define GIC_GIC400_PPI11        (11)
#define GIC_GIC400_PPI12        (12)
#define GIC_GIC400_PPI13        (13)
#define GIC_GIC400_PPI14        (14)
#define GIC_GIC400_PPI15        (15)
#define GIC_GIC400_PPI16        (16)
#define GIC_GIC400_PPI17        (17)
#define GIC_GIC400_PPI18        (18)
#define GIC_GIC400_PPI19        (19)
#define GIC_GIC400_PPI20        (20)
#define GIC_GIC400_PPI21        (21)
#define GIC_GIC400_PPI22        (22)
#define GIC_GIC400_PPI23        (23)
#define GIC_GIC400_PPI24        (24)
#define GIC_GIC400_PPI25        (25)
#define GIC_GIC400_PPI26        (26)
#define GIC_GIC400_PPI27        (27)
#define GIC_GIC400_PPI28        (28)
#define GIC_GIC400_PPI29        (29)
#define GIC_GIC400_PPI30        (30)
#define GIC_GIC400_PPI31        (31)

// ----------------------------------------------------------

// Sets the address of the GIC's distributor and CPU interfaces
void setGICAddr(void* dist, void* cpu);

// ----------------------------------------------------------

//  Global enable of the Interrupt Distributor
void enableGIC(void);

// Global disable of the Interrupt Distributor
void disableGIC(void);

// ----------------------------------------------------------

// Enables the interrupt source number ID
void enableIntID(unsigned int ID);

// Disables the interrupt source number ID
void disableIntID(unsigned int ID);

// ----------------------------------------------------------

// Sets the priority of the specified ID
void setIntPriority(unsigned int ID, unsigned int priority);

// Returns the priority of the specified ID
unsigned int getIntPriority(unsigned int ID);

// ----------------------------------------------------------

#define GIC_GIC400_TARGET_CPU0                (0x01)
#define GIC_GIC400_TARGET_CPU1                (0x02)
#define GIC_GIC400_TARGET_CPU2                (0x04)
#define GIC_GIC400_TARGET_CPU3                (0x08)
#define GIC_GIC400_TARGET_CPU4                (0x10)
#define GIC_GIC400_TARGET_CPU5                (0x20)
#define GIC_GIC400_TARGET_CPU6                (0x40)
#define GIC_GIC400_TARGET_CPU7                (0x80)

// Sets the target CPUs of the specified ID
// For 'target' use one of the above defines
void setIntTarget(unsigned int ID, unsigned int target);

// Returns the target CPUs of the specified ID
unsigned int getIntTarget(unsigned int ID);

// ----------------------------------------------------------

#define GIC_GIC400_CONFIG_LEVEL               (0)
#define GIC_GIC400_CONFIG_EDGE                (2)

// Configures the specified ID as being level or edge triggered

void configureSPI(unsigned int ID, unsigned int conf);

// ----------------------------------------------------------

// Sets the pending bit of the specified ID
void setIntPending(unsigned int ID);

// Clears the pending bit of the specified ID
void clearIntPending(unsigned int ID);

#define GIC_GIC400_PENDING_IS_SET             (1)
#define GIC_GIC400_PENDING_IS_CLEAR           (0)

// Returns the value of the status bit of the specified ID
unsigned int getIntPending(unsigned int ID);

// ----------------------------------------------------------

#define GIC_GIC400_SGI_SECURE              (0)
#define GIC_GIC400_SGI_NONSECURE           (1)
#define GIC_GIC400_SGI_FILTER_USE_LIST     (0)
#define GIC_GIC400_SGI_FILTER_NOT_THIS_CPU (1)
#define GIC_GIC400_SGI_FILTER_THIS_CPU     (2)
#define GIC_GIC400_SGI_CPU0                (0x01)
#define GIC_GIC400_SGI_CPU1                (0x02)
#define GIC_GIC400_SGI_CPU2                (0x04)
#define GIC_GIC400_SGI_CPU3                (0x08)
#define GIC_GIC400_SGI_CPU4                (0x10)
#define GIC_GIC400_SGI_CPU5                (0x20)
#define GIC_GIC400_SGI_CPU6                (0x40)
#define GIC_GIC400_SGI_CPU7                (0x80)

// Send a software generate interrupt
void sendSGI(unsigned int ID, unsigned int cpu_list, unsigned int filter_list, unsigned int SATT);

// ----------------------------------------------------------

// Sets the specified ID as secure
void makeIntGroup0(unsigned int ID);

// Set the specified ID as non-secure
void makeIntGroup1(unsigned int ID);

// Returns the security of the specified ID
unsigned int getIntGroup(unsigned int ID);

// ------------------------------------------------------------
// CPU Interface functions
// ------------------------------------------------------------

// Enables the processor interface
// Must been done one each core seperately
void enableCPUInterface(void);

// Enables the group 1 (non-secure) CPU interface
// This function can only be called from the Secure world
// Must been done one each core seperately
void enableNonSecureCPUInterface(void);

// Disables the processor interface
void disableCPUInterface(void);

// Enables the sending of secure interrupts as FIQs
void enableSecureFIQs(void);

// Disables the sending of secure interrupts as FIQs
void disableSecureFIQs(void);

//  Returns the value of the Interrupt Acknowledge Register
unsigned int readIntAck(void);

// Writes ID to the End Of Interrupt register
void writeEOI(unsigned int ID);

//  Returns the value of the Aliased Interrupt Acknowledge Register
unsigned int readAliasedIntAck(void);

// Writes ID to the Aliased End Of Interrupt register
void writeAliasedEOI(unsigned int ID);

// Sets the Priority mask register for the core run on
// The reset value masks ALL interrupts!
void setPriorityMask(unsigned int priority);

// Sets the Binary Point Register for the core run on
void setBinaryPoint(unsigned int priority);

// Sets the Aliased Binary Point Register for the core run on
void setAliasedBinaryPoint(unsigned int priority);

#endif

// ----------------------------------------------------------
// End of gic400_gic.h
// ----------------------------------------------------------
