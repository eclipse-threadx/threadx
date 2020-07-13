/******************************************************************************
	TMS320C66xx KeyStone Multicore DSP Software Development Kit (SDK). Rev 2A.
	Definitions, macros and API functions for DSP Environment.
	(C) MicroLAB Systems, 2015


	http://www.mlabsys.com
	ftp://ftp.mlabsys.com
	email: techsupport@mlabsys.com


	Description:
	------------
	This file contains definitions, macros and API functions for KeyStone
	Multicore DSP environment and must be included in the user C-application.
	DSP Software Development Kit library is based on TI Chip Support Library
	(CSL), so it's needed to add CSL libraries into the project along with DSP
	Software Development Kit library.

	Note that C66xx CorePac interrupt controller (INTC) functions are not
	included in this DSP Software Utilities library.
	It's delivered as a separate library from the remaining DSP Software
	Utilities. When using an embedded operating system that contains interrupt
	controller/dispatcher support, do not link in the INTC library.
	For interrupt controller support, SYS/BIOS users should use
	the HWI (Hardware Interrupt) and ECM (Event Combiner Manager) modules
	supported under SYS/BIOS v5.21 or later.


	Revision history:
	-----------------
	rev.1A		- 2014, initial release for C6678 DSP.
	rev.1B		- 2015, fixed minor bugs.
	rev.1C		- 2015:
						- changed C66XX_init_ddr3() DDR3 initialization
							function;
						- added DSP chip-level (CPINTC) interrupt controller
							functions;
	rev.2A		- 2015, totally redesigned DSP SDK;


	Notes:
	------
	1.	This C-header file is an include file for TI C6xxx C/C++ Code
		Generation Tools, which must be invoked to compile code for C66xx DSP
		platform.

	2.	The following additional libraries should be included:
		TI Chip Support Library (CSL) (ti.csl.ae66) should be included in
		Linker options (this library is located at
		$(TI_PDK_C6678_INSTALL_DIR)\packages\ti\csl\lib).

	3.	This file is best viewed with the TAB setting set to '4'.

	4.	This header file is externally controlled from user C-code by run-time 
		compiler keys definitions in order to apply DSP-type specific
		definitions to refer to particular definitions included for different
		DSP type:

			__C66XX_SELECT_C6678_DSP__
				- if defined in user code prior inclusion of this header file,
				  then DSP-type specific definitions are applied to C6678 DSP

	Copyright:
	----------
	This utility is supplied free of charge as it is without any obligation
	from MicroLAB Systems. No responsibility is assumed for any use or misuse
	of these utilities.

******************************************************************************/


/**
 * @file  C66XX.h
 *
 * @brief  KeyStone Multicore DSP SDK include file
 *
 * This file contains definitions, macros and API functions for KeyStone
 * Multicore DSP environment
 *
 */


#ifndef __C66XX_H__											// check for this file has been already included
#define __C66XX_H__											1


// General defs
#define C66XX_ON											1
#define C66XX_OFF											0


//=============================================================================
//------------ DSP-type specific run-time compiler keys processing ------------
// (this is required in order to exclude key confusions)

// Default setting is C6678 definitions
#ifndef __C66XX_SELECT_C6678_DSP__
#define __C66XX_SELECT_C6678_DSP__							1
#endif /* __C66XX_SELECT_C6678_DSP__ */

//=============================================================================


/** KeyStone Multicore DSP SDK revision ID */
#define C66XX_SDK_REVISION_ID								"2A"


// Include TI Chip Support Library (CSL) defs
// CSL Chip Functional Layer
#include <ti/csl/csl_chipAux.h>
// CSL TSC Functional Layer
#include <ti/csl/csl_tsc.h>
// CSL Cache Functional Layer
#include <ti/csl/csl_cacheAux.h>
// CSL Boot configuration defs
#include <ti/csl/csl_bootcfgAux.h>
// CSL Power and Sleep Controller defs
#include <ti/csl/csl_pscAux.h>
// CSL XMC Controller defs
#include <ti/csl/csl_xmcAux.h>
// CSL DDR3 defs
#include <ti/csl/csl_emif4f.h>
#include <ti/csl/csl_emif4fAux.h>
// CSL INTC defs
#include <ti/csl/src/intc/csl_intcAux.h>
// CSL CPINTC defs
#include <ti/csl/csl_cpIntcAux.h>
// CSL SGMII defs
#include <ti/csl/csl_cpsgmiiAux.h>
#include <ti/csl/csl_cpgmac_slAux.h>
// CSL GPIO Functional Layer
#include <ti/csl/csl_gpioAux.h>
// CSL Timer Functional Layer
#include <ti/csl/csl_tmrAux.h>
// CSL UART Functional Layer
#include <ti/csl/cslr_uart.h>
// CSL SRIO Functional Layer
#include <ti/csl/csl_srioAux.h>
#include <ti/csl/csl_srioAuxPhyLayer.h>


// Include C66xx DSP defs, macros and aux functions
#include "C66XX_DEF.hxx"
#include "C66XX_MACROS.hxx"
#include "C66XX_FUNCTIONS.hxx"


//=============================================================================
#endif /* __C66XX_H__ */
