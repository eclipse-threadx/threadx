// ------------------------------------------------------------
// ARMv8-A AArch64 System Timer
// Header Filer
//
// Copyright ARM Ltd 2013. All rights reserved.
// ------------------------------------------------------------

#ifndef _ARMV8A_SYSTEM_TIMER_H
#define _ARMV8A_SYSTEM_TIMER_H

// ------------------------------------------------------------
// CNTFRQ holds the frequency of the system counter
// Readable in all ELs
// Writable only by EL3

// Returns the value of CNTFRQ_EL0
unsigned int getCNTFRQ(void);

// Sets the value of CNTFRQ_EL0 (EL3 only!)
// freq - The value to be written into CNTFRQ_EL3
void setCNTFRQ(unsigned int freq);

// ------------------------------------------------------------
// CNTPCT_EL0 and CNTVCT_EL0 hold the physical and virtual counts
// Always accessable in Hpy and Secure EL1
// Access from EL2 and Non-Secure EL1 is configurable

// Returns the current value of physical count (CNTPCT_EL0)
unsigned long long getPhysicalCount(void);

// Returns the current value of the virtual count register (CNTVCT_EL0)
unsigned long long getVirtualCount(void);

// ------------------------------------------------------------
// The CNTKCTL register controls whether CNTPCT can be accessed from EL0
// Only acceable from EL1 and EL2

#define CNTKCTL_PCTEN    (1 << 0)   // Controls whether the physical counter, CNTPCT, and the frequency register CNTFRQ, are accessible from EL0
#define CNTKCTL_VCTEN    (1 << 1)   // Controls whether the virtual counter, CNTVCT, and the frequency register CNTFRQ, are accessible from
#define CNTKCTL_EVNTEN   (1 << 2)   // Enables the generation of an event stream from the virtual counter
#define CNTKCTL_EVNTDIR  (1 << 3)   // Controls which transition of the CNTVCT trigger bit, defined by EVNTI, generates an event

// Returns the value of EL1 Timer Control Register (CNTKCTL_EL1)
unsigned int getEL1Ctrl(void);

// Sets the value of EL1 Timer Control Register (CNTKCTL_EL1)
// value - The value to be written into CNTKCTL_EL1
void setEL1Ctrl(unsigned int value);

// ------------------------------------------------------------
// The CNTHCTL_EL2 register controls whether CNTPCT_EL0 can be accessed from EL1
// Only accessable from EL2 and EL3

#define CNTHCTL_CNTPCT   (1 << 0)
#define CNTHCTL_EVNTEN   (1 << 2)
#define CNTHCTL_EVNTDIR  (1 << 3)

// Returns the value of the EL2 Timer Control Register (CNTHCTL_EL2)
unsigned int getEL2Ctrl(void);

// Sets the value of EL2 Timer Control Register (CNTHCTL_EL2)
// value - The value to be written into CNTHCTL_EL2
void setEL2Ctrl(unsigned int value);

// ------------------------------------------------------------
// Non-Secure Physical Timer
// ------------------------------------------------------------
// Accessible from EL3, EL2 and EL1

// Returns the value of Non-Secure EL1 Physical Compare Value Register (CNTP_CVAL_EL0)
unsigned long long getNSEL1PhysicalCompValue(void);

// Sets the value of the Non-Secure EL1 Physical Compare Value Register (CNTP_CVAL_EL0)
// value - The value to be written into CNTP_CVAL_EL0
void setNSEL1PhysicalCompValue(unsigned long long value);

// Returns the value of Non-Secure EL1 Physical Timer Value Register (CNTP_TVAL_EL0)
unsigned int getNSEL1PhysicalTimerValue(void);

// Sets the value of the Non-Secure EL1 Physical Timer Value Register (CNTP_TVAL_EL0)
// value - The value to be written into CNTP_TVAL_EL0
void setNSEL1PhysicalTimerValue(unsigned int value);

#define CNTP_CTL_ENABLE  (1 << 0)
#define CNTP_CTL_MASK    (1 << 1)
#define CNTP_CTL_STATUS  (1 << 2)

// Returns the value of Non-Secure EL1 Physical Timer Control Register (CNTP_CTL_EL0)
unsigned int getNSEL1PhysicalTimerCtrl(void);

// Sets the value of the Non-Secure EL1 Physical Timer Control Register (CNTP_CTL_EL0)
// value - The value to be written into CNTP_CTL_EL0
void setNSEL1PhysicalTimerCtrl(unsigned int value);

// ------------------------------------------------------------
// Secure Physical Timer
// ------------------------------------------------------------
// Accessible from EL3, and configurably from secure EL1

// Returns the value of Secure EL1 Physical Compare Value Register (CNTPS_CVAL_EL1)
unsigned long long getSEL1PhysicalCompValue(void);

// Sets the value of the Secure EL1 Physical Compare Value Register (CNTPS_CVAL_EL1)
// value - The value to be written into CNTPS_CVAL_EL1
void setSEL1PhysicalCompValue(unsigned long long value);

// Returns the value of Secure EL1 Physical Timer Value Register (CNTPS_TVAL_EL1)
unsigned int getSEL1PhysicalTimerValue(void);

// Sets the value of the Secure EL1 Physical Timer Value Register (CNTPS_TVAL_EL1)
// value - The value to be written into CNTPS_TVAL_EL1
void setSEL1PhysicalTimerValue(unsigned int value);

#define CNTPS_CTL_ENABLE  (1 << 0)
#define CNTPS_CTL_MASK    (1 << 1)
#define CNTPS_CTL_STATUS  (1 << 2)

// Returns the value of Secure EL1 Physical Timer Control Register (CNTPS_CTL_EL1)
unsigned int getSEL1PhysicalTimerCtrl(void);

// Sets the value of the Secure EL1 Physical Timer Control Register (CNTPS_CTL_EL1)
// value - The value to be written into CNTPS_CTL_EL1
void setSEL1PhysicalTimerCtrl(unsigned int value);

// The SCR_EL3 register controls whether CNTPS_TVAL_EL1,
// CNTPS_CTL_EL1, and CNTPS_CVAL_EL1 can be accessed by secure
// EL1.
// Only accessible from EL3

#define SCR_ENABLE_SECURE_EL1_ACCESS  (1)
#define SCR_DISABLE_SECURE_EL1_ACCESS (0)

// Sets the values of the SCR_EL3.ST bit (bit 11) based on the value passed in 'config'
void configSecureEL1TimerAccess(unsigned int config);

// ------------------------------------------------------------
// Virtual Timer
// ------------------------------------------------------------
// Accessible from Non-Secure EL1 and EL2

//  Returns the value of EL1 Virtual Compare Value Register (CNTV_CVAL)
unsigned long long getEL1VirtualCompValue(void);

// Sets the value of the EL1 Virtual Compare Value Register (CNTV_CVAL)
// value - The value to be written into CNTV_CVAL
void setEL1VirtualCompValue(unsigned long long value);

// Returns the value of EL1 Virtual Timer Value Register (CNTV_TVAL)
unsigned int getEL1VirtualTimerValue(void);

// Sets the value of the EL1 Virtual Timer Value Register (CNTV_TVAL)
// value - The value to be written into CNTV_TVAL
void setEL1VirtualTimerValue(unsigned int value);

#define CNTV_CTL_ENABLE  (1 << 0)
#define CNTV_CTL_MASK    (1 << 1)
#define CNTV_CTL_STATUS  (1 << 2)

// Returns the value of EL1 Virtual Timer Control Register (CNTV_CTL)
unsigned int getEL1VirtualTimerCtrl(void);

// Sets the value of the EL1 Virtual Timer Control Register (CNTV_CTL)
// value - The value to be written into CNTV_CTL
void setEL1VirtualTimerCtrl(unsigned int value);

//
// Virtual timer functions to be called by EL2
//

// CNTVCT_EL2 holds the offset the virtual count is from the physical count
// Only accessable from EL2 and EL3

// Returns the value of the Counter Virtual Offset Register (CNTVOFF_EL2)
unsigned long long getVirtualCounterOffset(void);

// Sets the value of the Counter Virtual Offset Register (CNTVOFF_EL2)
// offset - The value to be written into CNTVOFF_EL2
void setVirtualCounterOffset(unsigned long long offset);

// ------------------------------------------------------------
// Hypervisor (EL2) Timer
// ------------------------------------------------------------

// Returns the value of EL2 Physical Compare Value Register (CNTHP_CVAL_EL2)
unsigned long long getEL2PhysicalCompValue(void);

// Sets the value of the EL2 Physical Compare Value Register (CNTHP_CVAL_EL2)
// value - The value to be written into CNTHP_CVAL_EL2
void setEL2PhysicalCompValue(unsigned long long value);

// Returns the value of EL2 Physical Timer Value Register (CNTHP_TVAL_EL2)
unsigned int getEL2PhysicalTimerValue(void);

#define CNTHP_CTL_ENABLE  (1 << 0)
#define CNTHP_CTL_MASK    (1 << 1)
#define CNTHP_CTL_STATUS  (1 << 2)

// Sets the value of the EL2 Physical Timer Value Register (CNTHP_TVAL_EL2)
// value - The value to be written into CNTHP_TVAL_EL2
void setEL2PhysicalTimerValue(unsigned int value);

#endif

// ------------------------------------------------------------
// End of armv8_aarch64_SystemTimer.h
// ------------------------------------------------------------
