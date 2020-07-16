// ----------------------------------------------------------
// GIC400 - Generic Interrupt Controller
//
// GIC Exercise
// ----------------------------------------------------------

#include "gic400_gic.h"

struct gic400_dist_if
{
        volatile unsigned int GICD_CTLR;             // +0x000 - RW - Distributor Control Register
  const volatile unsigned int GICD_TYPRE;            // +0x004 - RO - Interrupt Controller Type Register
  const volatile unsigned int GICD_IIDR;             // +0x008 - RO - Distributor Implementer Identification Register

  const volatile unsigned int padding0[29];

        volatile unsigned int GICD_IGROUPR[32];      // +0x080 - RW - Interrupt Groupt Registers (Security Registers in GICv1)

        volatile unsigned int GICD_ISENABLER[32];    // +0x100 - RW - Interrupt Set-Enable Registers
        volatile unsigned int GICD_ICENABLER[32];    // +0x180 - RW - Interrupt Clear-Enable Registers
        volatile unsigned int GICD_ISPENDR[32];      // +0x200 - RW - Interrupt Set-Pending Registers
        volatile unsigned int GICD_ICPENDR[32];      // +0x280 - RW - Interrupt Clear-Pending Registers
        volatile unsigned int GICD_ISACTIVER[32];    // +0x300 - RW - Interrupt Set-Active Register
        volatile unsigned int GICD_ICACTIVER[32];    // +0x380 - RW - Interrupt Clear-Active Register

       volatile unsigned char GICD_IPRIORITYR[1024]; // +0x400 - RW - Interrupt Priority Registers
        volatile unsigned int GICD_ITARGETSR[256];   // +0x800 - RW - Interrupt Processor Targets Registers
        volatile unsigned int GICD_ICFGR[64];        // +0xC00 - RW - Interrupt Configuration Registers

  const volatile unsigned int padding2[128];

        volatile unsigned int GICD_SGIR;             // +0xF00 - WO - Software Generated Interrupt Register

};

struct gic400_physical_cpu_if
{
        volatile unsigned int GICC_CTLR;             // +0x000 - RW - CPU Interface Control Register
        volatile unsigned int GICC_PMR;              // +0x004 - RW - Interrupt Priority Mask Register
        volatile unsigned int GICC_BPR;              // +0x008 - RW - Binary Point Register
  const volatile unsigned int GICC_IAR;              // +0x00C - RO - Interrupt Acknowledge Register
        volatile unsigned int GICC_EOIR;             // +0x010 - WO - End of Interrupt Register
  const volatile unsigned int GICC_RPR;              // +0x014 - RO - Running Priority Register
  const volatile unsigned int GICC_HPPIR;            // +0x018 - RO - Highest Pending Interrupt Register
        volatile unsigned int GICC_ABPR;             // +0x01C - RW - Aliased Binary Point Register
  const volatile unsigned int GICC_AIAR;             // +0x020 - RO - Aliased Interrupt Acknowledge Register
        volatile unsigned int GICC_AEOIR;            // +0x024 - WO - Aliased End of Interrupt Register
  const volatile unsigned int GICC_AHPPIR;           // +0x028 - RO - Aliased Highest Pending Interrupt Register

  const volatile unsigned int padding0[52];

  const volatile unsigned int GICC_IIDR;             // +0x0FC - RO - CPU Interface Identification Register
};

struct gic400_dist_if*          gic_dist;
struct gic400_physical_cpu_if*  gic_cpu;


// ------------------------------------------------------------

void setGICAddr(void* dist, void* cpu)
{
  gic_dist = (struct gic400_dist_if*)dist;
  gic_cpu  = (struct gic400_physical_cpu_if*)cpu;
  return;
}

// ------------------------------------------------------------

//  Global enable of the Interrupt Distributor
void enableGIC(void)
{
  gic_dist->GICD_CTLR = 3;
  return;
}

// Global disable of the Interrupt Distributor
void disableGIC(void)
{
  gic_dist->GICD_CTLR = 0;
  return;
}

// ------------------------------------------------------------

// Enables the interrupt source number ID
void enableIntID(unsigned int ID)
{
  unsigned int bank;

  bank = ID/32;        // There are 32 IDs per register, need to work out which register to access
  ID   = ID & 0x1f;    // ... and which bit within the register

  ID   = 1 << ID;      // Move a '1' into the correct bit position

  gic_dist->GICD_ISENABLER[bank] = ID;

  return;
}

// Disables the interrupt source number ID
void disableIntID(unsigned int ID)
{
  unsigned int bank;
  
  bank = ID/32;        // There are 32 IDs per register, need to work out which register to access
  ID   = ID & 0x1f;    // ... and which bit within the register

  ID   = 1 << ID;      // Move a '1' into the correct bit position

  gic_dist->GICD_ICENABLER[bank] = ID;

  return;
}

// ------------------------------------------------------------

// Sets the priority of the specified ID
void setIntPriority(unsigned int ID, unsigned int priority)
{
  if (ID > 1020)               // Check ID in range
    return;

  // The priority registers allows byte accesses
  // meaning using a char array we can directly
  // reference the correct entry
  gic_dist->GICD_IPRIORITYR[ID] = priority;

  return;
}

// Returns the priority of the specified ID
unsigned int getIntPriority(unsigned int ID)
{
  if (ID > 1020)               // Check ID in range
    return 0;

  return gic_dist->GICD_IPRIORITYR[ID];
} 

// ------------------------------------------------------------

// Sets the target CPUs of the specified ID
// For 'target' use one of the above defines
void setIntTarget(unsigned int ID, unsigned int target)
{
  unsigned int bank, tmp;

  target = target & 0xFF;      // Target field is 8-bits, mask off unused bit
  bank = ID/4;                 // There are 4 IDs per register, need to work out which register to access
  ID   = ID & 0x3;             // ... and which field within the register
  ID   = ID * 8;               // Convert from which field to a bit offset (8-bits per field)

  target = target << ID;       // Move prioity value into correct bit position

  tmp = gic_dist->GICD_ITARGETSR[bank]; // Read the current value in the register
  tmp = tmp & ~(0xFF << ID);           // Blank out the field holding the value we're modifying
  tmp = tmp | target;                  // OR in the new target
  gic_dist->GICD_ITARGETSR[bank] = tmp;

  return;
}

// Returns the target CPUs of the specified ID
unsigned int getIntTarget(unsigned int ID)
{
  unsigned int bank, tmp;

  bank = ID/4;                 // There are 4 IDs per register, need to work out which register to access
  ID   = ID & 0x3;             // ... and which field within the register
  ID   = ID * 8;               // Convert from which field to a bit offset (8-bits per field)

  tmp = gic_dist->GICD_ITARGETSR[bank];

  tmp = tmp >> ID;             // Shift desired field to bit position 0
  tmp = tmp & 0xFF;            // Mask off the other bits

  return tmp;
}

// ----------------------------------------------------------

// Configures the specified ID as being level or edge triggered
void configureSPI(unsigned int ID, unsigned int conf)
{
  unsigned int bank, tmp;

  conf = conf & 0x3;            // Mask out unused bits

  bank = ID/16;                 // There are 16 IDs per register, need to work out which register to access
  ID   = ID & 0xF;              // ... and which field within the register
  ID   = ID * 2;                // Convert from which field to a bit offset (2-bits per field)

  conf = conf << ID;            // Move configuration value into correct bit position

  tmp = gic_dist->	GICD_ICFGR[bank]; // Read current vlase
  tmp = tmp & ~(0x3 << ID);         // Clear the bits for the specified field
  tmp = tmp | conf;                 // OR in new configuration
  gic_dist->	GICD_ICFGR[bank] = tmp; // Write updated value back

  return;
}

// ----------------------------------------------------------

// Sets the pending bit of the specified ID
void setIntPending(unsigned int ID)
{
  unsigned int bank;

  bank = ID/32;        // There are 32 IDs per register, need to work out which register to access
  ID   = ID & 0x1f;    // ... and which bit within the register

  ID   = 1 << ID;      // Move a '1' into the correct bit position

  gic_dist->GICD_ISPENDR[bank] = ID;

  return;
}

// Clears the pending bit of the specified ID
void clearIntPending(unsigned int ID)
{
  unsigned int bank;

  bank = ID/32;        // There are 32 IDs per register, need to work out which register to access
  ID   = ID & 0x1f;    // ... and which bit within the register

  ID   = 1 << ID;      // Move a '1' into the correct bit position

  gic_dist->GICD_ICPENDR[bank] = ID;

  return;
}

// Returns the value of the status bit of the specifed ID
unsigned int getIntPending(unsigned int ID)
{
  unsigned int bank, tmp;

  bank = ID/32;        // There are 32 IDs per register, need to work out which register to access
  ID   = ID & 0x1f;    // ... and which bit within the register

  tmp = gic_dist->GICD_ICPENDR[bank];  // Read the register containing the ID we are interested in
  tmp = tmp >> ID;                    // Shift the status bit for specified ID to position 0
  tmp = tmp & 0x1;                    // Mask off the rest of the register

  return tmp;
}

// ------------------------------------------------------------

// Send a software generate interrupt
void sendSGI(unsigned int ID, unsigned int cpu_list, unsigned int filter_list, unsigned int SATT)
{
  // Ensure unused bits are clear, and shift into correct bit position
  ID = ID & 0xF;
  SATT = (SATT & 0x1) << 15;
  cpu_list = (cpu_list & 0xFF) << 16;
  filter_list = (filter_list & 0x3) << 24;

  // Combine fields
  ID = ID | SATT | cpu_list | filter_list;

  gic_dist->GICD_SGIR = ID;

  return;
}

// ------------------------------------------------------------

// Sets the specified ID as secure
void makeIntGroup0(unsigned int ID)
{
  unsigned int bank, tmp;

  bank = ID/32;        // There are 32 IDs per register, need to work out which register to access
  ID   = ID & 0x1f;    // ... and which bit within the register

  ID   = 1 << ID;      // Move a '1' into the correct bit position
  ID   = ~ID;          // Invert to get mask

  tmp = gic_dist->GICD_IGROUPR[bank];  // Read current value
  tmp = tmp & ID;
  gic_dist->GICD_IGROUPR[bank] = tmp;

  return;
}

// Set the specified ID as non-secure
void makeIntGroup1(unsigned int ID)
{
  unsigned int bank, tmp;

  bank = ID/32;        // There are 32 IDs per register, need to work out which register to access
  ID   = ID & 0x1f;    // ... and which bit within the register

  ID   = 1 << ID;      // Move a '1' into the correct bit position

  tmp = gic_dist->GICD_IGROUPR[bank];  // Read current value
  tmp = tmp | ID;                     // Or with bit mask to set the bit
  gic_dist->GICD_IGROUPR[bank] = tmp;  // Write-back
}

// Returns the security of the specified ID
unsigned int getIntGroup(unsigned int ID)
{
  // TBD
  return 0;
}

// ------------------------------------------------------------
// CPU Interface functions
// ------------------------------------------------------------

// Enables the CPU interface
// Must been done one each core seperately
void enableCPUInterface(void)
{
  unsigned int tmp;

  tmp = gic_cpu->GICC_CTLR;
  tmp = tmp | 0x1;          // Set bit 0
  gic_cpu->GICC_CTLR = tmp;
}

// Enables the group 1 (non-secure) CPU interface
// This function can only be called from the Secure world
// Must been done one each core seperately
void enableNonSecureCPUInterface(void)
{
  unsigned int tmp;

  tmp = gic_cpu->GICC_CTLR;
  tmp = tmp | 0x2;          // Set bit 1
  gic_cpu->GICC_CTLR = tmp;
}

// Disables the processor interface
void disableCPUInterface(void)
{
  unsigned int tmp;

  tmp = gic_cpu->GICC_CTLR;
  tmp = tmp & 0xFFFFFFFFE;  // Clear bit 0
  gic_cpu->GICC_CTLR = tmp;
}

// Enables the sending of secure interrupts as FIQs
void enableSecureFIQs(void)
{
  unsigned int tmp;

  tmp = gic_cpu->GICC_CTLR;
  tmp = tmp | 0x8;           // Set bit 3
  gic_cpu->GICC_CTLR = tmp;
}

// Disables the sending of secure interrupts as FIQs
void disableSecureFIQs(void)
{
  unsigned int tmp;

  tmp = gic_cpu->GICC_CTLR;
  tmp = tmp | 0xFFFFFFFF7;  // Clear bit 3
  gic_cpu->GICC_CTLR = tmp;
}

//  Returns the value of the Interrupt Acknowledge Register
unsigned int readIntAck(void)
{
  return gic_cpu->GICC_IAR;
}

// Writes ID to the End Of Interrupt register
void writeEOI(unsigned int ID)
{
  gic_cpu->GICC_EOIR = ID;
  return;
}

//  Returns the value of the Aliased Interrupt Acknowledge Register
unsigned int readAliasedIntAck(void)
{
  return gic_cpu->GICC_AIAR;
}

// Writes ID to the Aliased End Of Interrupt register
void writeAliasedEOI(unsigned int ID)
{
  gic_cpu->GICC_AEOIR = ID;
  return;
}

// Sets the Priority mask register for the core run on
// The reset value masks ALL interrupts!
void setPriorityMask(unsigned int priority)
{
  gic_cpu->GICC_PMR = (priority & 0xFF);
  return;
}

// Sets the Binary Point Register for the core run on
void setBinaryPoint(unsigned int priority)
{
  gic_cpu->GICC_BPR = (priority & 0xFF);
  return;
}

// Sets the Aliased Binary Point Register for the core run on
void setAliasedBinaryPoint(unsigned int priority)
{
  gic_cpu->GICC_ABPR = (priority & 0xFF);
  return;
}

// ------------------------------------------------------------
// End of gic400_gic.c
// ------------------------------------------------------------
