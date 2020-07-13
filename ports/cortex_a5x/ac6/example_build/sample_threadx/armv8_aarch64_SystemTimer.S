// ------------------------------------------------------------
// ARMv8-A AArch64 Generic Timer Access Functions
//
// Copyright ARM Ltd 2013. All rights reserved.
// ------------------------------------------------------------

 
	.section  AArch64_GenericTimer,"ax"
  .align 3

// ------------------------------------------------------------

	.global getCNTFRQ
  // uint32_t getCNTFRQ(void)
  // Returns the value of CNTFRQ_EL0
	.type getCNTFRQ, @function
getCNTFRQ:	
  MRS     x0, CNTFRQ_EL0
  RET
   

// ------------------------------------------------------------

	.global setCNTFRQ
  // void setCNTFRQ(uint32_t freq)
  // Sets the value of CNTFRQ_EL0 (only possible at EL3)
  // w0 - freq - The value to be written into CNTFRQ_EL0
	.type setCNTFRQ, @function
setCNTFRQ:	
  MSR     CNTFRQ_EL0, x0
  RET


// ------------------------------------------------------------

	.global getPhysicalCount
  // uint64_t getPhysicalCount(void)
  // Returns the current value of physical count (CNTPCT_EL0)
	.type getPhysicalCount, @function
getPhysicalCount:	
  MRS      x0, CNTPCT_EL0
  RET
   

// ------------------------------------------------------------

	.global getVirtualCount
  // uint64_t getVirtualCount(void)
  // Returns the current value of the virtual count register (CNTVCT_EL0)
	.type getVirtualCount, @function
getVirtualCount:	
  MRS     x0, CNTVCT_EL0
  RET
   

// ------------------------------------------------------------

	.global getEL1Ctrl
  // uint32_t getEL1Ctrl(void)
  // Returns the value of EL1 Timer Control Register (CNTKCTL_EL1)
	.type getEL1Ctrl, @function
getEL1Ctrl:	
  MRS      x0, CNTKCTL_EL1
  RET
   

// ------------------------------------------------------------

	.global setEL1Ctrl
  // void setEL1Ctrl(uint32_t value)
  // Sets the value of Counter Non-secure EL1 Control Register (CNTKCTL_EL1)
  // 0 - value - The value to be written into CNTKCTL_EL1
	.type setEL1Ctrl, @function
setEL1Ctrl:	
  MSR     CNTKCTL_EL1, x0
  RET
   

// ------------------------------------------------------------

	.global getEL2Ctrl
  // uint32_t getEL2Ctrl(void)
  // Returns the value of the EL2 Timer Control Register (CNTHCTL_EL2)
	.type getEL2Ctrl, @function
getEL2Ctrl:	
  MRS     x0, CNTHCTL_EL2
  RET
   

// ------------------------------------------------------------

	.global setEL2Ctrl
  // void setEL2Ctrl(uint32_t value)
  // Sets the value of the EL2 Timer Control Register (CNTHCTL_EL2)
  // x0 - value - The value to be written into CNTHCTL_EL2
	.type setEL2Ctrl, @function
setEL2Ctrl:	
  MSR     CNTHCTL_EL2, x0
  RET
   

// ------------------------------------------------------------
// Non-Secure Physical Timer
// ------------------------------------------------------------

	.global getNSEL1PhysicalCompValue
  // uint64_t getNSEL1PhysicalCompValue(void)
  // Returns the value of Non-Secure EL1 Physical Compare Value Register (CNTP_CVAL_EL0)
	.type getNSEL1PhysicalCompValue, @function
getNSEL1PhysicalCompValue:	
  MRS     x0, CNTP_CVAL_EL0
  RET
   

// ------------------------------------------------------------

	.global setNSEL1PhysicalCompValue
  // void setNSEL1PhysicalCompValue(uint64_t value)
  // Sets the value of the Non-Secure EL1 Physical Compare Value Register (CNTP_CVAL_EL0)
  // x0 - value - The value to be written into CNTP_CVAL_EL0
	.type setNSEL1PhysicalCompValue, @function
setNSEL1PhysicalCompValue:	
  MSR     CNTP_CVAL_EL0, x0
  RET
   

// ------------------------------------------------------------

	.global getNSEL1PhysicalTimerValue
  // uint32_t getNSEL1PhysicalTimerValue(void)
  // Returns the value of Non-Secure EL1 Physical Timer Value Register (CNTP_TVAL_EL0)
	.type getNSEL1PhysicalTimerValue, @function
getNSEL1PhysicalTimerValue:	
  MRS     x0, CNTP_TVAL_EL0
  RET
   

// ------------------------------------------------------------

	.global setNSEL1PhysicalTimerValue
  // void setNSEL1PhysicalTimerValue(uint32_t value)
  // Sets the value of the Non-Secure EL1 Physical Timer Value Register (CNTP_TVAL_EL0)
  // w0 - value - The value to be written into CNTP_TVAL_EL0
	.type setNSEL1PhysicalTimerValue, @function
setNSEL1PhysicalTimerValue:	
  MSR     CNTP_TVAL_EL0, x0
  RET
   

// ------------------------------------------------------------

	.global getNSEL1PhysicalTimerCtrl
  // uint32_t getNSEL1PhysicalTimerCtrl(void)
  // Returns the value of Non-Secure EL1 Physical Timer Control Register (CNTP_CTL_EL0)
	.type getNSEL1PhysicalTimerCtrl, @function
getNSEL1PhysicalTimerCtrl:	
  MRS     x0, CNTP_CTL_EL0
  RET
   

// ------------------------------------------------------------

	.global setNSEL1PhysicalTimerCtrl
  // void setNSEL1PhysicalTimerCtrl(uint32_t value)
  // Sets the value of the Non-Secure EL1 Physical Timer Control Register (CNTP_CTL_EL0)
  // w0 - value - The value to be written into CNTP_CTL_EL0
	.type setNSEL1PhysicalTimerCtrl, @function
setNSEL1PhysicalTimerCtrl:	
  MSR     CNTP_CTL_EL0, x0
  RET
   

// ------------------------------------------------------------
// Secure Physical Timer
// ------------------------------------------------------------

	.global getSEL1PhysicalCompValue
  // uint64_t getSEL1PhysicalCompValue(void)
  // Returns the value of Secure EL1 Physical Compare Value Register (CNTPS_CVAL_EL1)
	.type getSEL1PhysicalCompValue, @function
getSEL1PhysicalCompValue:	
  MRS     x0, CNTPS_CVAL_EL1
  RET
   

// ------------------------------------------------------------

	.global setSEL1PhysicalCompValue
  // void setSEL1PhysicalCompValue(uint64_t value)
  // Sets the value of the Secure EL1 Physical Compare Value Register (CNTPS_CVAL_EL1)
  // x0 - value - The value to be written into CNTPS_CVAL_EL1
	.type setSEL1PhysicalCompValue, @function
setSEL1PhysicalCompValue:	
  MSR     CNTPS_CVAL_EL1, x0
  RET
   


// ------------------------------------------------------------

	.global getSEL1PhysicalTimerValue
  // uint32_t getSEL1PhysicalTimerValue(void)
  // Returns the value of Secure EL1 Physical Timer Value Register (CNTPS_TVAL_EL1)
	.type getSEL1PhysicalTimerValue, @function
getSEL1PhysicalTimerValue:	
  MRS     x0, CNTPS_TVAL_EL1
  RET
   

// ------------------------------------------------------------

	.global setSEL1PhysicalTimerValue
  // void setSEL1PhysicalTimerValue(uint32_t value)
  // Sets the value of the Secure EL1 Physical Timer Value Register (CNTPS_TVAL_EL1)
  // w0 - value - The value to be written into CNTPS_TVAL_EL1
	.type setSEL1PhysicalTimerValue, @function
setSEL1PhysicalTimerValue:	
  MSR     CNTPS_TVAL_EL1, x0
  RET
   

// ------------------------------------------------------------

	.global getSEL1PhysicalTimerCtrl
  // uint32_t getSEL1PhysicalTimerCtrl(void)
  // Returns the value of Secure EL1 Physical Timer Control Register (CNTPS_CTL_EL1)
	.type getSEL1PhysicalTimerCtrl, @function
getSEL1PhysicalTimerCtrl:	
  MRS     x0, CNTPS_CTL_EL1
  RET
   

// ------------------------------------------------------------

	.global setSEL1PhysicalTimerCtrl
  // void setSEL1PhysicalTimerCtrl(uint32_t value)
  // Sets the value of the Secure EL1 Physical Timer Control Register (CNTPS_CTL_EL1)
  // w0 - value - The value to be written into CNTPS_CTL_EL1
	.type setSEL1PhysicalTimerCtrl, @function
setSEL1PhysicalTimerCtrl:	
  MSR     CNTPS_CTL_EL1, x0
  RET
   
  
// ------------------------------------------------------------

	.global configSecureEL1TimerAccess
  // void configSecureEL1TimerAccess(unsigned int config)//
  // Sets the values of the SCR_EL3.ST bit (bit 11) based on the value in x0
  // EL3 accessible only!
	.type configSecureEL1TimerAccess, @function
configSecureEL1TimerAccess:	
  MRS     x1, SCR_EL3
  BFI     x1, x0, #11, #1
  MSR     SCR_EL3, x1
  RET
   

// ------------------------------------------------------------
// Virtual Timer
// ------------------------------------------------------------

	.global getEL1VirtualCompValue
  // uint64_t getEL1VirtualCompValue(void)
  // Returns the value of EL1 Virtual Compare Value Register (CNTV_CVAL_EL0)
	.type getEL1VirtualCompValue, @function
getEL1VirtualCompValue:	
  MRS      x0, CNTV_CVAL_EL0
  RET
   

// ------------------------------------------------------------

	.global setEL1VirtualCompValue
  // void setEL1VirtualCompValue(uint64_t value)
  // Sets the value of the EL1 Virtual Compare Value Register (CNTV_CVAL_EL0)
  // x0 - value - The value to be written into CNTV_CVAL_EL0
	.type setEL1VirtualCompValue, @function
setEL1VirtualCompValue:	
  MSR     CNTV_CVAL_EL0, x0
  RET


// ------------------------------------------------------------

	.global getEL1VirtualTimerValue
  // uint32_t getEL1VirtualTimerValue(void)
  // Returns the value of EL1 Virtual Timer Value Register (CNTV_TVAL_EL0)
	.type getEL1VirtualTimerValue, @function
getEL1VirtualTimerValue:	
  MRS     x0, CNTV_TVAL_EL0
  RET
   

// ------------------------------------------------------------

	.global setEL1VirtualTimerValue
  // void setEL1VirtualTimerValue(uint32_t value)
  // Sets the value of the EL1 Virtual Timer Value Register (CNTV_TVAL_EL0)
  // w0 - value - The value to be written into CNTV_TVAL_EL0
	.type setEL1VirtualTimerValue, @function
setEL1VirtualTimerValue:	
  MSR     CNTV_TVAL_EL0, x0
  RET
   

// ------------------------------------------------------------

	.global getEL1VirtualTimerCtrl
  // uint32_t getEL1VirtualTimerCtrl(void)
  // Returns the value of EL1 Virtual Timer Control Register (CNTV_CTL_EL0)
	.type getEL1VirtualTimerCtrl, @function
getEL1VirtualTimerCtrl:	
  MRS     x0, CNTV_CTL_EL0
  RET
   

// ------------------------------------------------------------

	.global setEL1VirtualTimerCtrl
  // void setEL1VirtualTimerCtrl(uint32_t value)
  // Sets the value of the EL1 Virtual Timer Control Register (CNTV_CTL_EL0)
  // w0 - value - The value to be written into CNTV_CTL_EL0
	.type setEL1VirtualTimerCtrl, @function
setEL1VirtualTimerCtrl:	
  MSR     CNTV_CTL_EL0, x0
  RET
   
  
// ------------------------------------------------------------
// Virtual Timer functions to be called by EL2
// ------------------------------------------------------------

	.global getVirtualCounterOffset
  // uint64_t getVirtualCounterOffset(void)
  // Returns the value of the Counter Virtual Offset Register (CNTVOFF_EL2)
  // EL2 and EL3 only
	.type getVirtualCounterOffset, @function
getVirtualCounterOffset:	
  MRS     x0, CNTVOFF_EL2
  RET
   

// ------------------------------------------------------------

	.global setVirtualCounterOffset
  // void setVirtualCounterOffset(uint64_t offset)
  // Sets the value of the Counter Virtual Offset Register (CNTVOFF_EL2)
  // x0 - offset - The value to be written into CNTVOFF_EL2
  // EL2 and EL3 only
	.type setVirtualCounterOffset, @function
setVirtualCounterOffset:	
  MSR     CNTVOFF_EL2, x0
  RET
   
  
// ------------------------------------------------------------
// EL2 Physical Timer
// ------------------------------------------------------------

	.global getEL2PhysicalCompValue
  // uint64_t getEL2PhysicalCompValue(void)
  // Returns the value of EL2 Physical Compare Value Register (CNTHP_CVAL_EL2)
	.type getEL2PhysicalCompValue, @function
getEL2PhysicalCompValue:	
  MRS     x0, CNTHP_CVAL_EL2
  RET
   

// ------------------------------------------------------------

	.global setEL2PhysicalCompValue
  // void setEL2PhysicalCompValue(uint64_t value)
  // Sets the value of the EL2 Physical Compare Value Register (CNTHP_CVAL_EL2)
  // x0 - value - The value to be written into CNTHP_CVAL_EL2
	.type setEL2PhysicalCompValue, @function
setEL2PhysicalCompValue:	
  MSR     CNTHP_CVAL_EL2, x0
  RET
   


// ------------------------------------------------------------

	.global getEL2PhysicalTimerValue
  // uint32_t getEL2PhysicalTimerValue(void)
  // Returns the value of EL2 Physical Timer Value Register (CNTHP_TVAL_EL2)
	.type getEL2PhysicalTimerValue, @function
getEL2PhysicalTimerValue:	
  MRS     x0, CNTHP_TVAL_EL2
  RET
   

// ------------------------------------------------------------

	.global setEL2PhysicalTimerValue
  // void setEL2PhysicalTimerValue(uint32_t value)
  // Sets the value of the EL2 Physical Timer Value Register (CNTHP_TVAL_EL2)
  // w0 - value - The value to be written into CNTHP_TVAL_EL2
	.type setEL2PhysicalTimerValue, @function
setEL2PhysicalTimerValue:	
  MSR     CNTHP_TVAL_EL2, x0
  RET
   

// ------------------------------------------------------------

	.global getEL2PhysicalTimerCtrl
  // uint32_t getEL2PhysicalTimerCtrl(void)
  // Returns the value of EL2 Physical Timer Control Register (CNTHP_CTL_EL2)
	.type getEL2PhysicalTimerCtrl, @function
getEL2PhysicalTimerCtrl:	
  MRS     x0, CNTHP_CTL_EL2
  RET
   

// ------------------------------------------------------------

	.global setEL2PhysicalTimerCtrl
  // void setEL2PhysicalTimerCtrl(uint32_t value)
  // Sets the value of the EL2 Physical Timer Control Register (CNTHP_CTL_EL2)
  // w0 - value - The value to be written into CNTHP_CTL_EL2
	.type setEL2PhysicalTimerCtrl, @function
setEL2PhysicalTimerCtrl:	
  MSR     CNTHP_CTL_EL2, x0
  RET
   

// ------------------------------------------------------------
// End of code
// ------------------------------------------------------------
