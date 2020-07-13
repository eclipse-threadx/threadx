/******************************************************************************
	TORNADO AMC modules Software Development Kit (SDK). Rev 4A.
	Definitions, macros and API functions for DSP Environment.
	(C) MicroLAB Systems, 2014-2017


	http://www.mlabsys.com
	ftp://ftp.mlabsys.com
	email: techsupport@mlabsys.com


	Description:
	------------
	This file contains definitions, macros and API functions for TORNADO AMC 
	modules on-board DSP environment and must be included in the user
	C-application for TORNADO AMC modules.


	Revision history:
	-----------------
	rev.1A		- 2014, initial release for TORNADO-A6678 board rev.1A.
	rev.1B		- 2015, fixed some bugs.
	rev.1C		- 2015, added support for TORNADO-A6678 board rev.1B:
							- updated DSP external control registers area;
							- updated TA66XX_init_dsp() DSP initialization
								function;
							- added MMC-to-DSP communication functions in DSP
								I2C functions.
	rev.2A		- 2015, totally redesigned SDK for TORNADO-A6678 board rev.1B;
	rev.3A		- 2016, added support for TORNADO-A6678/FMC board rev.1A;
	rev.3B		- 2017:
						- added support for TORNADO-A6678/FMC board rev.1B;
	rev.4A		- 2017:
					- added support for TORNADO-AZ/FMC board rev.1A;


	Notes:
	------
	1.	This C-header file is an include file for TI C6xxx C/C++ Code
		Generation Tools, which must be invoked to compile for C66xx DSP
		platform.

	2.	This file is best viewed with the TAB setting set to '4'.

	3.	This library uses dynamic memory allocation in erase FLASH functions,
		so user should provide enough memory area for dynamic memory allocation
		(-heap parameter) and place the section (.sysmem) in the corresponding
		memory area in linker command file.
		The max requested memory allocation block length is up to FLASH memory
		sector length in bytes (128 KB).

	4.	This header file is externally controlled from user C-code by run-time
		compiler keys definitions in order to apply board-type specific
		definitions to refer to particular definitions included for different
		board type:

			__TA66XX_BC_USE_BIOS__
				- if defined in user code prior inclusion of this header file,
				  then TI SYS/BIOS modules(HWI (Hardware Interrupt) and ECM
				  (Event Combiner Manager)), will be used to support
				  C66xx CorePac interrupt controller (INTC).
				  TI INTC Chip Support Library (CSL) should be used in case
				  NO embedded operating system is used in user application.
					DEFAULT is NOT DEFINED, so CSL INTC library will be used
					to support C66xx CorePac interrupt controller.

	Copyright:
	----------
	This utility is supplied free of charge as it is without any obligation
	from MicroLAB Systems. No responsibility is assumed for any use or misuse
	of these utilities.

******************************************************************************/


/**
 * @file  TA66XX_DSP_BC.h
 *
 * @brief  Main include file
 *
 * This file contains definitions, macros and API functions for TORNADO
 * AMC modules on-board DSP environment
 *
 */


#ifndef __TA66XX_DSP_BC_H__									// check for this file has been already included
#define __TA66XX_DSP_BC_H__									1


#if defined __TA66XX_BC_USE_BIOS__
// Include this file to prevent compiler errors
// when using SYS/BIOS
#include <xdc/std.h>
#endif /* __TA66XX_BC_USE_BIOS__ */


// Include general TORNADO AMC SDK defs
#include "TA66XX_DSP.h"


//=============================================================================
//------------ board-type specific run-time compiler keys processing ----------
// (this is required in order to exclude key confusions)

// Default setting is C6678 DSP definitions
#define __C66XX_SELECT_C6678_DSP__							1

// Finally we must define C-code data type for accessing I/O areas
typedef volatile uint16_t									__TA66XX_BC_DSP_XCR_RG_DATA_TYPE__;
typedef volatile uint16_t									__TA66XX_BC_FLASH_DATA_TYPE__;
//=============================================================================


// Include TORNADO AMC SDK functions
#include "TA66XX_DSP_BC_FUNCTIONS.hxx"


//=============================================================================
#endif /* __TA66XX_DSP_BC_H__ */

