/******************************************************************************
	TMS320C66xx KeyStone Multicore DSP Software Development Kit (SDK). Rev 2A.
	(C) MicroLAB Systems, 2014-2015

	File:	Macros
	-----

	Notes:
	------
	1.	This C-header file contains C66xx DSP macro definitions and
		is used with C66XX.h C-header file.

	2.	This file is best viewed with the TAB setting set to '4'.

******************************************************************************/


/**
 * @file  C66XX_MACROS.hxx
 *
 * @brief  Macros
 *
 * This file contains C66xx DSP macro definitions
 *
 */


#ifndef __C66XX_MACROS_HXX__								// check for this file has been already included
#define __C66XX_MACROS_HXX__								1


//=============================================================================
//------------ I/O peripherals data type selector for C66xx DSP ---------------
// C66xx DSP has all I/O peripherals aligned as LSB of 32-bit words, then
// I/O peripherals should be accessed as UNSIGNED only.

// The declared __C66XX_IO_DATA_TYPE__ is used to unify below macros and
// functions and to make particular selection at the run-time compilation time.
//================================================================

typedef volatile uint32_t									__C66XX_IO_DATA_TYPE__;

// read-back data bitmask for DSP memory-mapped registers (32-bit wide) 
#define C66XX_RG_DATA_BITMASK								0xffffffff
//=============================================================================



//=============================================================================
//============ General register access macros =================================
//=============================================================================
// Macro to get selected register value
#define C66XX_GET_RG_VALUE(addr)							((*(__C66XX_IO_DATA_TYPE__ *) addr) & C66XX_RG_DATA_BITMASK)

// Macro to set selected register to value
#define C66XX_SET_RG_VALUE(addr, val)						((*(__C66XX_IO_DATA_TYPE__ *) addr) = (val & C66XX_RG_DATA_BITMASK))

// Macro to make FIELD for the supplied value
#define C66XX_MAKE_FIELD(FIELD, val)						(((val) << C66XX_##FIELD##_BITSHIFT) & C66XX_##FIELD##_BITMASK)

// Macro to get FIELD value from the selected register
#define C66XX_GET_FIELD_VALUE(addr, FIELD)					(((*(__C66XX_IO_DATA_TYPE__ *) addr) & C66XX_##FIELD##_BITMASK) >> C66XX_##FIELD##_BITSHIFT)

// Macro to set FIELD to value in the selected register
#define C66XX_SET_FIELD_VALUE(addr, FIELD, val)				((*(__C66XX_IO_DATA_TYPE__ *) addr) = ((*(__C66XX_IO_DATA_TYPE__ *) addr) & ~C66XX_##FIELD##_BITMASK) | C66XX_MAKE_FIELD(FIELD, val))

//=============================================================================



//=============================================================================
//============ DSP core registers macros ======================================
//=============================================================================

//------------ DSP CorePack revision register macros --------------------------
#define C66XX_get_core_mm_revid_rg()						C66XX_GET_RG_VALUE(C66XX_CORE_MM_REVID_RG_ADDR)

	// dedicated bit specific macros
#define C66XX_get_core_mm_revid_version()					C66XX_GET_FIELD_VALUE(C66XX_CORE_MM_REVID_RG_ADDR, CORE_MM_REVID_VERSION)
#define C66XX_get_core_mm_revid_revision()					C66XX_GET_FIELD_VALUE(C66XX_CORE_MM_REVID_RG_ADDR, CORE_MM_REVID_REVISION)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_CORE_MM_REVID_VERSION_IS_C6678				(C66XX_get_core_mm_revid_version() == C66XX_CORE_MM_REVID_VERSION_C6678)
#define C66XX_CORE_MM_REVID_REVISION_IS_1_0					(C66XX_get_core_mm_revid_revision() == C66XX_CORE_MM_REVID_REVISION_1_0)
#define C66XX_CORE_MM_REVID_REVISION_IS_2_0					(C66XX_get_core_mm_revid_revision() == C66XX_CORE_MM_REVID_REVISION_2_0)

//------------ DSP interrupt controller registers macros ----------------------
// i index corresonds to DSP Event registers number
// Note that i index should be from 0 to 3 !!!
#define C66XX_get_core_evtflag_rg_addr(i)					(C66XX_CORE_EVTFLAG_RG_BADDR + i * C66XX_CORE_EVTFLAG_RG_OFFSET)
#define C66XX_get_core_evtflag_rg(i)						C66XX_GET_RG_VALUE(C66XX_get_core_evtflag_rg_addr(i))

#define C66XX_get_core_evtset_rg_addr(i)					(C66XX_CORE_EVTSET_RG_BADDR + i * C66XX_CORE_EVTSET_RG_OFFSET)
#define C66XX_set_core_evtset_rg(i, v)						C66XX_SET_RG_VALUE(C66XX_get_core_evtset_rg_addr(i), v)

#define C66XX_get_core_evtclr_rg_addr(i)					(C66XX_CORE_EVTCLR_RG_BADDR + i * C66XX_CORE_EVTCLR_RG_OFFSET)
#define C66XX_set_core_evtclr_rg(i, v)						C66XX_SET_RG_VALUE(C66XX_get_core_evtclr_rg_addr(i), v)

#define C66XX_get_core_evtmask_rg_addr(i)					(C66XX_CORE_EVTMASK_RG_BADDR + i * C66XX_CORE_EVTMASK_RG_OFFSET)
#define C66XX_get_core_evtmask_rg(i)						C66XX_GET_RG_VALUE(C66XX_get_core_evtmask_rg_addr(i))
#define C66XX_set_core_evtmask_rg(i, v)						C66XX_SET_RG_VALUE(C66XX_get_core_evtmask_rg_addr(i), v)

//------------ DSP interrupt macros -------------------------------------------
// These macros are used to set/get particular DSP Event in DSP Event registers
// i index corresonds to DSP Event ID number (0 - 127)
#define C66XX_get_core_event_id_rg(i)						(i / 32)
#define C66XX_get_core_event_id_bitmask(i)					(0x1 << (i % 32))

#define C66XX_get_core_event_id_flag(i)						(C66XX_get_core_evtflag_rg(C66XX_get_core_event_id_rg(i)) & C66XX_get_core_event_id_bitmask(i))
#define C66XX_set_core_event_id_flag(i)						(C66XX_set_core_evtset_rg(C66XX_get_core_event_id_rg(i), C66XX_get_core_event_id_bitmask(i)))
#define C66XX_clear_core_event_id_flag(i)					(C66XX_set_core_evtclr_rg(C66XX_get_core_event_id_rg(i), C66XX_get_core_event_id_bitmask(i)))
#define C66XX_get_core_event_id_mask(i)						(C66XX_get_core_evtmask_rg(C66XX_get_core_event_id_rg(i)) & C66XX_get_core_event_id_bitmask(i))
#define C66XX_set_core_event_id_mask(i)						(C66XX_set_core_evtmask_rg(C66XX_get_core_event_id_rg(i), C66XX_get_core_event_id_bitmask(i)))

//=============================================================================



//=============================================================================
//============ PLL controller registers macros ================================
//=============================================================================

//------------ PLL control register macros ------------------------------------
#define C66XX_get_pll_pllctl_rg()							C66XX_GET_RG_VALUE(C66XX_PLL_PLLCTL_RG_ADDR)
#define C66XX_set_pll_pllctl_rg(v)							C66XX_SET_RG_VALUE(C66XX_PLL_PLLCTL_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_pll_pllctl_pllensrc()						C66XX_GET_FIELD_VALUE(C66XX_PLL_PLLCTL_RG_ADDR, PLL_PLLCTL_PLLENSRC)
#define C66XX_set_pll_pllctl_pllensrc(v)					C66XX_SET_FIELD_VALUE(C66XX_PLL_PLLCTL_RG_ADDR, PLL_PLLCTL_PLLENSRC, v)
#define C66XX_get_pll_pllctl_pllrst()						C66XX_GET_FIELD_VALUE(C66XX_PLL_PLLCTL_RG_ADDR, PLL_PLLCTL_PLLRST)
#define C66XX_set_pll_pllctl_pllrst(v)						C66XX_SET_FIELD_VALUE(C66XX_PLL_PLLCTL_RG_ADDR, PLL_PLLCTL_PLLRST, v)
#define C66XX_get_pll_pllctl_pllpwrdn()						C66XX_GET_FIELD_VALUE(C66XX_PLL_PLLCTL_RG_ADDR, PLL_PLLCTL_PLLPWRDN)
#define C66XX_set_pll_pllctl_pllpwrdn(v)					C66XX_SET_FIELD_VALUE(C66XX_PLL_PLLCTL_RG_ADDR, PLL_PLLCTL_PLLPWRDN, v)
#define C66XX_get_pll_pllctl_pllen()						C66XX_GET_FIELD_VALUE(C66XX_PLL_PLLCTL_RG_ADDR, PLL_PLLCTL_PLLEN)
#define C66XX_set_pll_pllctl_pllen(v)						C66XX_SET_FIELD_VALUE(C66XX_PLL_PLLCTL_RG_ADDR, PLL_PLLCTL_PLLEN, v)

	// direct bit set macros
#define C66XX_SET_PLL_PLLCTL_PLLENSRC_ON					C66XX_set_pll_pllctl_pllensrc(C66XX_ON)
#define C66XX_SET_PLL_PLLCTL_PLLENSRC_OFF					C66XX_set_pll_pllctl_pllensrc(C66XX_OFF)
#define C66XX_SET_PLL_PLLCTL_PLLRST_ON						C66XX_set_pll_pllctl_pllrst(C66XX_ON)
#define C66XX_SET_PLL_PLLCTL_PLLRST_OFF						C66XX_set_pll_pllctl_pllrst(C66XX_OFF)
#define C66XX_SET_PLL_PLLCTL_PLLPWRDN_ON					C66XX_set_pll_pllctl_pllpwrdn(C66XX_ON)
#define C66XX_SET_PLL_PLLCTL_PLLPWRDN_OFF					C66XX_set_pll_pllctl_pllpwrdn(C66XX_OFF)
#define C66XX_SET_PLL_PLLCTL_PLLEN_ON						C66XX_set_pll_pllctl_pllen(C66XX_ON)
#define C66XX_SET_PLL_PLLCTL_PLLEN_OFF						C66XX_set_pll_pllctl_pllen(C66XX_OFF)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_PLL_PLLCTL_PLLENSRC_IS_ON						(C66XX_get_pll_pllctl_pllensrc() == C66XX_ON)
#define C66XX_PLL_PLLCTL_PLLRST_IS_ON						(C66XX_get_pll_pllctl_pllrst() == C66XX_ON)
#define C66XX_PLL_PLLCTL_PLLPWRDN_IS_ON						(C66XX_get_pll_pllctl_pllpwrdn() == C66XX_ON)
#define C66XX_PLL_PLLCTL_PLLEN_IS_ON						(C66XX_get_pll_pllctl_pllen() == C66XX_ON)


//------------ PLL secondary control register macros --------------------------
#define C66XX_get_pll_secctl_rg()							C66XX_GET_RG_VALUE(C66XX_PLL_SECCTL_RG_ADDR)
#define C66XX_set_pll_secctl_rg(v)							C66XX_SET_RG_VALUE(C66XX_PLL_SECCTL_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_pll_secctl_bypass()						C66XX_GET_FIELD_VALUE(C66XX_PLL_SECCTL_RG_ADDR, PLL_SECCTL_BYPASS)
#define C66XX_set_pll_secctl_bypass(v)						C66XX_SET_FIELD_VALUE(C66XX_PLL_SECCTL_RG_ADDR, PLL_SECCTL_BYPASS, v)
#define C66XX_get_pll_secctl_output_divide()				C66XX_GET_FIELD_VALUE(C66XX_PLL_SECCTL_RG_ADDR, PLL_SECCTL_OUTPUT_DIVIDE)
#define C66XX_set_pll_secctl_output_divide(v)				C66XX_SET_FIELD_VALUE(C66XX_PLL_SECCTL_RG_ADDR, PLL_SECCTL_OUTPUT_DIVIDE, v)

	// direct bit set macros
#define C66XX_SET_PLL_SECCTL_BYPASS_ON						C66XX_set_pll_secctl_bypass(C66XX_ON)
#define C66XX_SET_PLL_SECCTL_BYPASS_OFF						C66XX_set_pll_secctl_bypass(C66XX_OFF)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_PLL_SECCTL_BYPASS_IS_ON						(C66XX_get_pll_secctl_bypass() == C66XX_ON)


//------------ PLL multiplier control register macros -------------------------
#define C66XX_get_pll_pllm_rg()								C66XX_GET_RG_VALUE(C66XX_PLL_PLLM_RG_ADDR)
#define C66XX_set_pll_pllm_rg(v)							C66XX_SET_RG_VALUE(C66XX_PLL_PLLM_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_pll_pllm_pllm()							C66XX_GET_FIELD_VALUE(C66XX_PLL_PLLM_RG_ADDR, PLL_PLLM_PLLM)
#define C66XX_set_pll_pllm_pllm(v)							C66XX_SET_FIELD_VALUE(C66XX_PLL_PLLM_RG_ADDR, PLL_PLLM_PLLM, v)


//------------ PLL controller divider registers macros ------------------------
// i index corresonds to PLL controller divider registers number
// Note that i index should be from 0 to 15 instead of 1 to 16 !!!
#define C66XX_get_pll_plldiv_rg_addr(i)						(i < 3 ? (C66XX_PLL_PLLDIV1_RG_ADDR + i * 0x4) : (C66XX_PLL_PLLDIV4_RG_ADDR + (i - 3) * 0x4))
#define C66XX_get_pll_plldiv_rg(i)							C66XX_GET_RG_VALUE(C66XX_get_pll_plldiv_rg_addr(i))
#define C66XX_set_pll_plldiv_rg(i, v)						C66XX_SET_RG_VALUE(C66XX_get_pll_plldiv_rg_addr(i), v)

	// dedicated bit specific macros
#define C66XX_get_pll_plldiv_den(i)							C66XX_GET_FIELD_VALUE(C66XX_get_pll_plldiv_rg_addr(i), PLL_PLLDIV_DEN)
#define C66XX_set_pll_plldiv_den(i, v)						C66XX_SET_FIELD_VALUE(C66XX_get_pll_plldiv_rg_addr(i), PLL_PLLDIV_DEN, v)
#define C66XX_get_pll_plldiv_ratio(i)						C66XX_GET_FIELD_VALUE(C66XX_get_pll_plldiv_rg_addr(i), PLL_PLLDIV_RATIO)
#define C66XX_set_pll_plldiv_ratio(i, v)					C66XX_SET_FIELD_VALUE(C66XX_get_pll_plldiv_rg_addr(i), PLL_PLLDIV_RATIO, v)

	// direct bit set macros
#define C66XX_SET_PLL_PLLDIV_DEN_ON(i)						C66XX_set_pll_plldiv_den(i, C66XX_ON)
#define C66XX_SET_PLL_PLLDIV_DEN_OFF(i)						C66XX_set_pll_plldiv_den(i, C66XX_OFF)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_PLL_PLLDIV_DEN_IS_ON(i)						(C66XX_get_pll_plldiv_den(i) == C66XX_ON)


//------------ PLL controller command register macros -------------------------
#define C66XX_get_pll_pllcmd_rg()							C66XX_GET_RG_VALUE(C66XX_PLL_PLLCMD_RG_ADDR)
#define C66XX_set_pll_pllcmd_rg(v)							C66XX_SET_RG_VALUE(C66XX_PLL_PLLCMD_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_pll_pllcmd_goset()						C66XX_GET_FIELD_VALUE(C66XX_PLL_PLLCMD_RG_ADDR, PLL_PLLCMD_GOSET)
#define C66XX_set_pll_pllcmd_goset(v)						C66XX_SET_FIELD_VALUE(C66XX_PLL_PLLCMD_RG_ADDR, PLL_PLLCMD_GOSET, v)

	// direct bit set macros
#define C66XX_SET_PLL_PLLCMD_GOSET_ON						C66XX_set_pll_pllcmd_goset(C66XX_ON)
#define C66XX_SET_PLL_PLLCMD_GOSET_OFF						C66XX_set_pll_pllcmd_goset(C66XX_OFF)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_PLL_PLLCMD_GOSET_IS_ON						(C66XX_get_pll_pllcmd_goset() == C66XX_ON)


//------------ PLL controller status register macros --------------------------
#define C66XX_get_pll_pllstat_rg()							C66XX_GET_RG_VALUE(C66XX_PLL_PLLSTAT_RG_ADDR)
#define C66XX_set_pll_pllstat_rg(v)							C66XX_SET_RG_VALUE(C66XX_PLL_PLLSTAT_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_pll_pllstat_gostat()						C66XX_GET_FIELD_VALUE(C66XX_PLL_PLLSTAT_RG_ADDR, PLL_PLLSTAT_GOSTAT)
#define C66XX_set_pll_pllstat_gostat(v)						C66XX_SET_FIELD_VALUE(C66XX_PLL_PLLSTAT_RG_ADDR, PLL_PLLSTAT_GOSTAT, v)

	// direct bit set macros
#define C66XX_SET_PLL_PLLSTAT_GOSTAT_ON						C66XX_set_pll_pllstat_gostat(C66XX_ON)
#define C66XX_SET_PLL_PLLSTAT_GOSTAT_OFF					C66XX_set_pll_pllstat_gostat(C66XX_OFF)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_PLL_PLLSTAT_GOSTAT_IS_ON						(C66XX_get_pll_pllstat_gostat() == C66XX_ON)


//------------ PLL controller clock align control register macros -------------
#define C66XX_get_pll_alnctl_rg()							C66XX_GET_RG_VALUE(C66XX_PLL_ALNCTL_RG_ADDR)
#define C66XX_set_pll_alnctl_rg(v)							C66XX_SET_RG_VALUE(C66XX_PLL_ALNCTL_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_pll_alnctl_aln()							C66XX_GET_FIELD_VALUE(C66XX_PLL_ALNCTL_RG_ADDR, PLL_ALNCTL_ALN)
#define C66XX_set_pll_alnctl_aln(v)							C66XX_SET_FIELD_VALUE(C66XX_PLL_ALNCTL_RG_ADDR, PLL_ALNCTL_ALN, v)


//------------ PLL controller divider ratio change status register macros -----
#define C66XX_get_pll_dchange_rg()							C66XX_GET_RG_VALUE(C66XX_PLL_DCHANGE_RG_ADDR)
#define C66XX_set_pll_dchange_rg(v)							C66XX_SET_RG_VALUE(C66XX_PLL_DCHANGE_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_pll_dchange_sys()							C66XX_GET_FIELD_VALUE(C66XX_PLL_DCHANGE_RG_ADDR, PLL_DCHANGE_SYS)
#define C66XX_set_pll_dchange_sys(v)						C66XX_SET_FIELD_VALUE(C66XX_PLL_DCHANGE_RG_ADDR, PLL_DCHANGE_SYS, v)


//------------ SYSCLK status register macros ----------------------------------
#define C66XX_get_pll_systat_rg()							C66XX_GET_RG_VALUE(C66XX_PLL_SYSTAT_RG_ADDR)

	// dedicated bit specific macros
#define C66XX_get_pll_systat_syson()						C66XX_GET_FIELD_VALUE(C66XX_PLL_SYSTAT_RG_ADDR, PLL_SYSTAT_SYSON)

//=============================================================================



//=============================================================================
//============ Device State Control registers macros ==========================
//=============================================================================

//------------ Main PLL Control register 0 macros -----------------------------
#define C66XX_get_bootcfg_mainpllctl0_rg()					C66XX_GET_RG_VALUE(C66XX_BOOTCFG_MAINPLLCTL0_RG_ADDR)
#define C66XX_set_bootcfg_mainpllctl0_rg(v)					C66XX_SET_RG_VALUE(C66XX_BOOTCFG_MAINPLLCTL0_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_bootcfg_mainpllctl0_bwadj()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_MAINPLLCTL0_RG_ADDR, BOOTCFG_MAINPLLCTL0_BWADJ)
#define C66XX_set_bootcfg_mainpllctl0_bwadj(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_MAINPLLCTL0_RG_ADDR, BOOTCFG_MAINPLLCTL0_BWADJ, v)
#define C66XX_get_bootcfg_mainpllctl0_pllm()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_MAINPLLCTL0_RG_ADDR, BOOTCFG_MAINPLLCTL0_PLLM)
#define C66XX_set_bootcfg_mainpllctl0_pllm(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_MAINPLLCTL0_RG_ADDR, BOOTCFG_MAINPLLCTL0_PLLM, v)
#define C66XX_get_bootcfg_mainpllctl0_plld()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_MAINPLLCTL0_RG_ADDR, BOOTCFG_MAINPLLCTL0_PLLD)
#define C66XX_set_bootcfg_mainpllctl0_plld(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_MAINPLLCTL0_RG_ADDR, BOOTCFG_MAINPLLCTL0_PLLD, v)


//------------ Main PLL Control register 1 macros -----------------------------
#define C66XX_get_bootcfg_mainpllctl1_rg()					C66XX_GET_RG_VALUE(C66XX_BOOTCFG_MAINPLLCTL1_RG_ADDR)
#define C66XX_set_bootcfg_mainpllctl1_rg(v)					C66XX_SET_RG_VALUE(C66XX_BOOTCFG_MAINPLLCTL1_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_bootcfg_mainpllctl1_ensat()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_MAINPLLCTL1_RG_ADDR, BOOTCFG_MAINPLLCTL1_ENSAT)
#define C66XX_set_bootcfg_mainpllctl1_ensat(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_MAINPLLCTL1_RG_ADDR, BOOTCFG_MAINPLLCTL1_ENSAT, v)
#define C66XX_get_bootcfg_mainpllctl1_bwadj()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_MAINPLLCTL1_RG_ADDR, BOOTCFG_MAINPLLCTL1_BWADJ)
#define C66XX_set_bootcfg_mainpllctl1_bwadj(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_MAINPLLCTL1_RG_ADDR, BOOTCFG_MAINPLLCTL1_BWADJ, v)

	// direct bit set macros
#define C66XX_SET_BOOTCFG_MAINPLLCTL1_ENSAT_ON				C66XX_set_bootcfg_mainpllctl1_ensat(C66XX_ON)
#define C66XX_SET_BOOTCFG_MAINPLLCTL1_ENSAT_OFF				C66XX_set_bootcfg_mainpllctl1_ensat(C66XX_OFF)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_BOOTCFG_MAINPLLCTL1_ENSAT_IS_ON				(C66XX_get_bootcfg_mainpllctl1_ensat() == C66XX_ON)


//------------ DDR3 PLL Control register 0 macros -----------------------------
#define C66XX_get_bootcfg_ddr3pllctl0_rg()					C66XX_GET_RG_VALUE(C66XX_BOOTCFG_DDR3PLLCTL0_RG_ADDR)
#define C66XX_set_bootcfg_ddr3pllctl0_rg(v)					C66XX_SET_RG_VALUE(C66XX_BOOTCFG_DDR3PLLCTL0_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_bootcfg_ddr3pllctl0_bwadj()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_DDR3PLLCTL0_RG_ADDR, BOOTCFG_DDR3PLLCTL0_BWADJ)
#define C66XX_set_bootcfg_ddr3pllctl0_bwadj(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_DDR3PLLCTL0_RG_ADDR, BOOTCFG_DDR3PLLCTL0_BWADJ, v)
#define C66XX_get_bootcfg_ddr3pllctl0_bypass()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_DDR3PLLCTL0_RG_ADDR, BOOTCFG_DDR3PLLCTL0_BYPASS)
#define C66XX_set_bootcfg_ddr3pllctl0_bypass(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_DDR3PLLCTL0_RG_ADDR, BOOTCFG_DDR3PLLCTL0_BYPASS, v)
#define C66XX_get_bootcfg_ddr3pllctl0_pllm()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_DDR3PLLCTL0_RG_ADDR, BOOTCFG_DDR3PLLCTL0_PLLM)
#define C66XX_set_bootcfg_ddr3pllctl0_pllm(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_DDR3PLLCTL0_RG_ADDR, BOOTCFG_DDR3PLLCTL0_PLLM, v)
#define C66XX_get_bootcfg_ddr3pllctl0_plld()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_DDR3PLLCTL0_RG_ADDR, BOOTCFG_DDR3PLLCTL0_PLLD)
#define C66XX_set_bootcfg_ddr3pllctl0_plld(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_DDR3PLLCTL0_RG_ADDR, BOOTCFG_DDR3PLLCTL0_PLLD, v)

	// direct bit set macros
#define C66XX_SET_BOOTCFG_DDR3PLLCTL0_BYPASS_ON				C66XX_set_bootcfg_ddr3pllctl0_bypass(C66XX_ON)
#define C66XX_SET_BOOTCFG_DDR3PLLCTL0_BYPASS_OFF			C66XX_set_bootcfg_ddr3pllctl0_bypass(C66XX_OFF)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_BOOTCFG_DDR3PLLCTL0_BYPASS_IS_ON				(C66XX_get_bootcfg_ddr3pllctl0_bypass() == C66XX_ON)


//------------ DDR3 PLL Control register 1 macros -----------------------------
#define C66XX_get_bootcfg_ddr3pllctl1_rg()					C66XX_GET_RG_VALUE(C66XX_BOOTCFG_DDR3PLLCTL1_RG_ADDR)
#define C66XX_set_bootcfg_ddr3pllctl1_rg(v)					C66XX_SET_RG_VALUE(C66XX_BOOTCFG_DDR3PLLCTL1_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_bootcfg_ddr3pllctl1_pllrst()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_DDR3PLLCTL1_RG_ADDR, BOOTCFG_DDR3PLLCTL1_PLLRST)
#define C66XX_set_bootcfg_ddr3pllctl1_pllrst(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_DDR3PLLCTL1_RG_ADDR, BOOTCFG_DDR3PLLCTL1_PLLRST, v)
#define C66XX_get_bootcfg_ddr3pllctl1_ensat()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_DDR3PLLCTL1_RG_ADDR, BOOTCFG_DDR3PLLCTL1_ENSAT)
#define C66XX_set_bootcfg_ddr3pllctl1_ensat(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_DDR3PLLCTL1_RG_ADDR, BOOTCFG_DDR3PLLCTL1_ENSAT, v)
#define C66XX_get_bootcfg_ddr3pllctl1_bwadj()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_DDR3PLLCTL1_RG_ADDR, BOOTCFG_DDR3PLLCTL1_BWADJ)
#define C66XX_set_bootcfg_ddr3pllctl1_bwadj(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_DDR3PLLCTL1_RG_ADDR, BOOTCFG_DDR3PLLCTL1_BWADJ, v)

	// direct bit set macros
#define C66XX_SET_BOOTCFG_DDR3PLLCTL1_PLLRST_ON				C66XX_set_bootcfg_ddr3pllctl1_pllrst(C66XX_ON)
#define C66XX_SET_BOOTCFG_DDR3PLLCTL1_PLLRST_OFF			C66XX_set_bootcfg_ddr3pllctl1_pllrst(C66XX_OFF)
#define C66XX_SET_BOOTCFG_DDR3PLLCTL1_ENSAT_ON				C66XX_set_bootcfg_ddr3pllctl1_ensat(C66XX_ON)
#define C66XX_SET_BOOTCFG_DDR3PLLCTL1_ENSAT_OFF				C66XX_set_bootcfg_ddr3pllctl1_ensat(C66XX_OFF)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_BOOTCFG_DDR3PLLCTL1_PLLRST_IS_ON				(C66XX_get_bootcfg_ddr3pllctl1_pllrst() == C66XX_ON)
#define C66XX_BOOTCFG_DDR3PLLCTL1_ENSAT_IS_ON				(C66XX_get_bootcfg_ddr3pllctl1_ensat() == C66XX_ON)


//------------ PASS PLL Control register 0 macros -----------------------------
#define C66XX_get_bootcfg_passpllctl0_rg()					C66XX_GET_RG_VALUE(C66XX_BOOTCFG_PASSPLLCTL0_RG_ADDR)
#define C66XX_set_bootcfg_passpllctl0_rg(v)					C66XX_SET_RG_VALUE(C66XX_BOOTCFG_PASSPLLCTL0_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_bootcfg_passpllctl0_bwadj()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_PASSPLLCTL0_RG_ADDR, BOOTCFG_PASSPLLCTL0_BWADJ)
#define C66XX_set_bootcfg_passpllctl0_bwadj(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_PASSPLLCTL0_RG_ADDR, BOOTCFG_PASSPLLCTL0_BWADJ, v)
#define C66XX_get_bootcfg_passpllctl0_bypass()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_PASSPLLCTL0_RG_ADDR, BOOTCFG_PASSPLLCTL0_BYPASS)
#define C66XX_set_bootcfg_passpllctl0_bypass(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_PASSPLLCTL0_RG_ADDR, BOOTCFG_PASSPLLCTL0_BYPASS, v)
#define C66XX_get_bootcfg_passpllctl0_pllm()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_PASSPLLCTL0_RG_ADDR, BOOTCFG_PASSPLLCTL0_PLLM)
#define C66XX_set_bootcfg_passpllctl0_pllm(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_PASSPLLCTL0_RG_ADDR, BOOTCFG_PASSPLLCTL0_PLLM, v)
#define C66XX_get_bootcfg_passpllctl0_plld()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_PASSPLLCTL0_RG_ADDR, BOOTCFG_PASSPLLCTL0_PLLD)
#define C66XX_set_bootcfg_passpllctl0_plld(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_PASSPLLCTL0_RG_ADDR, BOOTCFG_PASSPLLCTL0_PLLD, v)

	// direct bit set macros
#define C66XX_SET_BOOTCFG_PASSPLLCTL0_BYPASS_ON				C66XX_set_bootcfg_passpllctl0_bypass(C66XX_ON)
#define C66XX_SET_BOOTCFG_PASSPLLCTL0_BYPASS_OFF			C66XX_set_bootcfg_passpllctl0_bypass(C66XX_OFF)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_BOOTCFG_PASSPLLCTL0_BYPASS_IS_ON				(C66XX_get_bootcfg_passpllctl0_bypass() == C66XX_ON)


//------------ PASS PLL Control register 1 macros -----------------------------
#define C66XX_get_bootcfg_passpllctl1_rg()					C66XX_GET_RG_VALUE(C66XX_BOOTCFG_PASSPLLCTL1_RG_ADDR)
#define C66XX_set_bootcfg_passpllctl1_rg(v)					C66XX_SET_RG_VALUE(C66XX_BOOTCFG_PASSPLLCTL1_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_bootcfg_passpllctl1_pllrst()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_PASSPLLCTL1_RG_ADDR, BOOTCFG_PASSPLLCTL1_PLLRST)
#define C66XX_set_bootcfg_passpllctl1_pllrst(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_PASSPLLCTL1_RG_ADDR, BOOTCFG_PASSPLLCTL1_PLLRST, v)
#define C66XX_get_bootcfg_passpllctl1_pllselect()			C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_PASSPLLCTL1_RG_ADDR, BOOTCFG_PASSPLLCTL1_PLLSELECT)
#define C66XX_set_bootcfg_passpllctl1_pllselect(v)			C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_PASSPLLCTL1_RG_ADDR, BOOTCFG_PASSPLLCTL1_PLLSELECT, v)
#define C66XX_get_bootcfg_passpllctl1_ensat()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_PASSPLLCTL1_RG_ADDR, BOOTCFG_PASSPLLCTL1_ENSAT)
#define C66XX_set_bootcfg_passpllctl1_ensat(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_PASSPLLCTL1_RG_ADDR, BOOTCFG_PASSPLLCTL1_ENSAT, v)
#define C66XX_get_bootcfg_passpllctl1_bwadj()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_PASSPLLCTL1_RG_ADDR, BOOTCFG_PASSPLLCTL1_BWADJ)
#define C66XX_set_bootcfg_passpllctl1_bwadj(v)				C66XX_SET_FIELD_VALUE(C66XX_BOOTCFG_PASSPLLCTL1_RG_ADDR, BOOTCFG_PASSPLLCTL1_BWADJ, v)

	// direct bit set macros
#define C66XX_SET_BOOTCFG_PASSPLLCTL1_PLLRST_ON				C66XX_set_bootcfg_passpllctl1_pllrst(C66XX_ON)
#define C66XX_SET_BOOTCFG_PASSPLLCTL1_PLLRST_OFF			C66XX_set_bootcfg_passpllctl1_pllrst(C66XX_OFF)
#define C66XX_SET_BOOTCFG_PASSPLLCTL1_PLLSELECT_ON			C66XX_set_bootcfg_passpllctl1_pllselect(C66XX_ON)
#define C66XX_SET_BOOTCFG_PASSPLLCTL1_PLLSELECT_OFF			C66XX_set_bootcfg_passpllctl1_pllselect(C66XX_OFF)
#define C66XX_SET_BOOTCFG_PASSPLLCTL1_ENSAT_ON				C66XX_set_bootcfg_passpllctl1_ensat(C66XX_ON)
#define C66XX_SET_BOOTCFG_PASSPLLCTL1_ENSAT_OFF				C66XX_set_bootcfg_passpllctl1_ensat(C66XX_OFF)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_BOOTCFG_PASSPLLCTL1_PLLRST_IS_ON				(C66XX_get_bootcfg_passpllctl1_pllrst() == C66XX_ON)
#define C66XX_BOOTCFG_PASSPLLCTL1_PLLSELECT_IS_ON			(C66XX_get_bootcfg_passpllctl1_pllselect() == C66XX_ON)
#define C66XX_BOOTCFG_PASSPLLCTL1_ENSAT_IS_ON				(C66XX_get_bootcfg_passpllctl1_ensat() == C66XX_ON)


//------------ Device speed register macros -----------------------------------
#define C66XX_get_bootcfg_devspeed_rg()						C66XX_GET_RG_VALUE(C66XX_BOOTCFG_DEVSPEED_RG_ADDR)
	// dedicated bit specific macros
#define C66XX_get_bootcfg_devspeed_devspeed()				C66XX_GET_FIELD_VALUE(C66XX_BOOTCFG_DEVSPEED_RG_ADDR, BOOTCFG_DEVSPEED_DEVSPEED)

//=============================================================================



//=============================================================================
//============ I2C registers macros ===========================================
//=============================================================================

//------------ I2C own address register macros --------------------------------
#define C66XX_get_i2c_icoar_rg()							C66XX_GET_RG_VALUE(C66XX_I2C_ICOAR_RG_ADDR)
#define C66XX_set_i2c_icoar_rg(v)							C66XX_SET_RG_VALUE(C66XX_I2C_ICOAR_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_i2c_icoar_oaddr()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICOAR_RG_ADDR, I2C_ICOAR_OADDR)
#define C66XX_set_i2c_icoar_oaddr(v)						C66XX_SET_FIELD_VALUE(C66XX_I2C_ICOAR_RG_ADDR, I2C_ICOAR_OADDR, v)


//------------ I2C interrupt mask register macros -----------------------------
#define C66XX_get_i2c_icimr_rg()							C66XX_GET_RG_VALUE(C66XX_I2C_ICIMR_RG_ADDR)
#define C66XX_set_i2c_icimr_rg(v)							C66XX_SET_RG_VALUE(C66XX_I2C_ICIMR_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_i2c_icimr_aas()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICIMR_RG_ADDR, I2C_ICIMR_AAS)
#define C66XX_set_i2c_icimr_aas(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICIMR_RG_ADDR, I2C_ICIMR_AAS, v)
#define C66XX_get_i2c_icimr_scd()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICIMR_RG_ADDR, I2C_ICIMR_SCD)
#define C66XX_set_i2c_icimr_scd(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICIMR_RG_ADDR, I2C_ICIMR_SCD, v)
#define C66XX_get_i2c_icimr_icxrdy()						C66XX_GET_FIELD_VALUE(C66XX_I2C_ICIMR_RG_ADDR, I2C_ICIMR_ICXRDY)
#define C66XX_set_i2c_icimr_icxrdy(v)						C66XX_SET_FIELD_VALUE(C66XX_I2C_ICIMR_RG_ADDR, I2C_ICIMR_ICXRDY, v)
#define C66XX_get_i2c_icimr_icrdrdy()						C66XX_GET_FIELD_VALUE(C66XX_I2C_ICIMR_RG_ADDR, I2C_ICIMR_ICRDRDY)
#define C66XX_set_i2c_icimr_icrdrdy(v)						C66XX_SET_FIELD_VALUE(C66XX_I2C_ICIMR_RG_ADDR, I2C_ICIMR_ICRDRDY, v)
#define C66XX_get_i2c_icimr_ardy()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICIMR_RG_ADDR, I2C_ICIMR_ARDY)
#define C66XX_set_i2c_icimr_ardy(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICIMR_RG_ADDR, I2C_ICIMR_ARDY, v)
#define C66XX_get_i2c_icimr_nack()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICIMR_RG_ADDR, I2C_ICIMR_NACK)
#define C66XX_set_i2c_icimr_nack(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICIMR_RG_ADDR, I2C_ICIMR_NACK, v)
#define C66XX_get_i2c_icimr_al()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICIMR_RG_ADDR, I2C_ICIMR_AL)
#define C66XX_set_i2c_icimr_al(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICIMR_RG_ADDR, I2C_ICIMR_AL, v)

	// direct bit set macros
#define C66XX_SET_I2C_ICIMR_AAS_ON							C66XX_set_i2c_icimr_aas(C66XX_ON)
#define C66XX_SET_I2C_ICIMR_AAS_OFF							C66XX_set_i2c_icimr_aas(C66XX_OFF)
#define C66XX_SET_I2C_ICIMR_SCD_ON							C66XX_set_i2c_icimr_scd(C66XX_ON)
#define C66XX_SET_I2C_ICIMR_SCD_OFF							C66XX_set_i2c_icimr_scd(C66XX_OFF)
#define C66XX_SET_I2C_ICIMR_ICXRDY_ON						C66XX_set_i2c_icimr_icxrdy(C66XX_ON)
#define C66XX_SET_I2C_ICIMR_ICXRDY_OFF						C66XX_set_i2c_icimr_icxrdy(C66XX_OFF)
#define C66XX_SET_I2C_ICIMR_ICRDRDY_ON						C66XX_set_i2c_icimr_icrdrdy(C66XX_ON)
#define C66XX_SET_I2C_ICIMR_ICRDRDY_OFF						C66XX_set_i2c_icimr_icrdrdy(C66XX_OFF)
#define C66XX_SET_I2C_ICIMR_ARDY_ON							C66XX_set_i2c_icimr_ardy(C66XX_ON)
#define C66XX_SET_I2C_ICIMR_ARDY_OFF						C66XX_set_i2c_icimr_ardy(C66XX_OFF)
#define C66XX_SET_I2C_ICIMR_NACK_ON							C66XX_set_i2c_icimr_nack(C66XX_ON)
#define C66XX_SET_I2C_ICIMR_NACK_OFF						C66XX_set_i2c_icimr_nack(C66XX_OFF)
#define C66XX_SET_I2C_ICIMR_AL_ON							C66XX_set_i2c_icimr_al(C66XX_ON)
#define C66XX_SET_I2C_ICIMR_AL_OFF							C66XX_set_i2c_icimr_al(C66XX_OFF)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_I2C_ICIMR_AAS_IS_ON							(C66XX_get_i2c_icimr_aas() == C66XX_ON)
#define C66XX_I2C_ICIMR_SCD_IS_ON							(C66XX_get_i2c_icimr_scd() == C66XX_ON)
#define C66XX_I2C_ICIMR_ICXRDY_IS_ON						(C66XX_get_i2c_icimr_icxrdy() == C66XX_ON)
#define C66XX_I2C_ICIMR_ICRDRDY_IS_ON						(C66XX_get_i2c_icimr_icrdrdy() == C66XX_ON)
#define C66XX_I2C_ICIMR_ARDY_IS_ON							(C66XX_get_i2c_icimr_ardy() == C66XX_ON)
#define C66XX_I2C_ICIMR_NACK_IS_ON							(C66XX_get_i2c_icimr_nack() == C66XX_ON)
#define C66XX_I2C_ICIMR_AL_IS_ON							(C66XX_get_i2c_icimr_al() == C66XX_ON)


//------------ I2C interrupt status register macros ---------------------------
#define C66XX_get_i2c_icstr_rg()							C66XX_GET_RG_VALUE(C66XX_I2C_ICSTR_RG_ADDR)
#define C66XX_set_i2c_icstr_rg(v)							C66XX_SET_RG_VALUE(C66XX_I2C_ICSTR_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_i2c_icstr_sdir()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_SDIR)
#define C66XX_set_i2c_icstr_sdir(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_SDIR, v)
#define C66XX_get_i2c_icstr_nacksnt()						C66XX_GET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_NACKSNT)
#define C66XX_set_i2c_icstr_nacksnt(v)						C66XX_SET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_NACKSNT, v)
#define C66XX_get_i2c_icstr_bb()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_BB)
#define C66XX_set_i2c_icstr_bb(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_BB, v)
#define C66XX_get_i2c_icstr_rsfull()						C66XX_GET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_RSFULL)
#define C66XX_get_i2c_icstr_xsmt()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_XSMT)
#define C66XX_get_i2c_icstr_aas()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_AAS)
#define C66XX_get_i2c_icstr_ad0()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_AD0)
#define C66XX_get_i2c_icstr_scd()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_SCD)
#define C66XX_set_i2c_icstr_scd(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_SCD, v)
#define C66XX_get_i2c_icstr_icxrdy()						C66XX_GET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_ICXRDY)
#define C66XX_set_i2c_icstr_icxrdy(v)						C66XX_SET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_ICXRDY, v)
#define C66XX_get_i2c_icstr_icrdrdy()						C66XX_GET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_ICRDRDY)
#define C66XX_set_i2c_icstr_icrdrdy(v)						C66XX_SET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_ICRDRDY, v)
#define C66XX_get_i2c_icstr_ardy()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_ARDY)
#define C66XX_set_i2c_icstr_ardy(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_ARDY, v)
#define C66XX_get_i2c_icstr_nack()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_NACK)
#define C66XX_set_i2c_icstr_nack(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_NACK, v)
#define C66XX_get_i2c_icstr_al()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_AL)
#define C66XX_set_i2c_icstr_al(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICSTR_RG_ADDR, I2C_ICSTR_AL, v)

	// direct bit set macros
#define C66XX_CLEAR_I2C_ICSTR_SDIR							C66XX_set_i2c_icstr_sdir(C66XX_ON)
#define C66XX_CLEAR_I2C_ICSTR_NACKSNT						C66XX_set_i2c_icstr_nacksnt(C66XX_ON)
#define C66XX_CLEAR_I2C_ICSTR_BB							C66XX_set_i2c_icstr_bb(C66XX_ON)
#define C66XX_CLEAR_I2C_ICSTR_SCD							C66XX_set_i2c_icstr_scd(C66XX_ON)
#define C66XX_CLEAR_I2C_ICSTR_ICXRDY						C66XX_set_i2c_icstr_icxrdy(C66XX_ON)
#define C66XX_CLEAR_I2C_ICSTR_ICRDRDY						C66XX_set_i2c_icstr_icrdrdy(C66XX_ON)
#define C66XX_CLEAR_I2C_ICSTR_ARDY							C66XX_set_i2c_icstr_ardy(C66XX_ON)
#define C66XX_CLEAR_I2C_ICSTR_NACK							C66XX_set_i2c_icstr_nack(C66XX_ON)
#define C66XX_CLEAR_I2C_ICSTR_AL							C66XX_set_i2c_icstr_al(C66XX_ON)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_I2C_ICSTR_SDIR_IS_ON							(C66XX_get_i2c_icstr_sdir() == C66XX_ON)
#define C66XX_I2C_ICSTR_NACKSNT_IS_ON						(C66XX_get_i2c_icstr_nacksnt() == C66XX_ON)
#define C66XX_I2C_ICSTR_BB_IS_ON							(C66XX_get_i2c_icstr_bb() == C66XX_ON)
#define C66XX_I2C_ICSTR_RSFULL_IS_ON						(C66XX_get_i2c_icstr_rsfull() == C66XX_ON)
#define C66XX_I2C_ICSTR_XSMT_IS_ON							(C66XX_get_i2c_icstr_xsmt() == C66XX_ON)
#define C66XX_I2C_ICSTR_AAS_IS_ON							(C66XX_get_i2c_icstr_aas() == C66XX_ON)
#define C66XX_I2C_ICSTR_AD0_IS_ON							(C66XX_get_i2c_icstr_ad0() == C66XX_ON)
#define C66XX_I2C_ICSTR_SCD_IS_ON							(C66XX_get_i2c_icstr_scd() == C66XX_ON)
#define C66XX_I2C_ICSTR_ICXRDY_IS_ON						(C66XX_get_i2c_icstr_icxrdy() == C66XX_ON)
#define C66XX_I2C_ICSTR_ICRDRDY_IS_ON						(C66XX_get_i2c_icstr_icrdrdy() == C66XX_ON)
#define C66XX_I2C_ICSTR_ARDY_IS_ON							(C66XX_get_i2c_icstr_ardy() == C66XX_ON)
#define C66XX_I2C_ICSTR_NACK_IS_ON							(C66XX_get_i2c_icstr_nack() == C66XX_ON)
#define C66XX_I2C_ICSTR_AL_IS_ON							(C66XX_get_i2c_icstr_al() == C66XX_ON)


//------------ I2C clock low-time divider register macros ---------------------
#define C66XX_get_i2c_icclkl_rg()							C66XX_GET_RG_VALUE(C66XX_I2C_ICCLKL_RG_ADDR)
#define C66XX_set_i2c_icclkl_rg(v)							C66XX_SET_RG_VALUE(C66XX_I2C_ICCLKL_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_i2c_icclkl_iccl()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICCLKL_RG_ADDR, I2C_ICCLKL_ICCL)
#define C66XX_set_i2c_icclkl_iccl(v)						C66XX_SET_FIELD_VALUE(C66XX_I2C_ICCLKL_RG_ADDR, I2C_ICCLKL_ICCL, v)


//------------ I2C clock high-time divider register macros --------------------
#define C66XX_get_i2c_icclkh_rg()							C66XX_GET_RG_VALUE(C66XX_I2C_ICCLKH_RG_ADDR)
#define C66XX_set_i2c_icclkh_rg(v)							C66XX_SET_RG_VALUE(C66XX_I2C_ICCLKH_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_i2c_icclkh_icch()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICCLKH_RG_ADDR, I2C_ICCLKH_ICCH)
#define C66XX_set_i2c_icclkh_icch(v)						C66XX_SET_FIELD_VALUE(C66XX_I2C_ICCLKH_RG_ADDR, I2C_ICCLKH_ICCH, v)


//------------ I2C data count register macros ---------------------------------
#define C66XX_get_i2c_iccnt_rg()							C66XX_GET_RG_VALUE(C66XX_I2C_ICCNT_RG_ADDR)
#define C66XX_set_i2c_iccnt_rg(v)							C66XX_SET_RG_VALUE(C66XX_I2C_ICCNT_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_i2c_iccnt_icdc()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICCNT_RG_ADDR, I2C_ICCNT_ICDC)
#define C66XX_set_i2c_iccnt_icdc(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICCNT_RG_ADDR, I2C_ICCNT_ICDC, v)


//------------ I2C data receive register macros -------------------------------
#define C66XX_get_i2c_icdrr_rg()							C66XX_GET_RG_VALUE(C66XX_I2C_ICDRR_RG_ADDR)

	// dedicated bit specific macros
#define C66XX_get_i2c_icdrr_d()								C66XX_GET_FIELD_VALUE(C66XX_I2C_ICDRR_RG_ADDR, I2C_ICDRR_D)


//------------ I2C slave address register macros ------------------------------
#define C66XX_get_i2c_icsar_rg()							C66XX_GET_RG_VALUE(C66XX_I2C_ICSAR_RG_ADDR)
#define C66XX_set_i2c_icsar_rg(v)							C66XX_SET_RG_VALUE(C66XX_I2C_ICSAR_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_i2c_icsar_saddr()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICSAR_RG_ADDR, I2C_ICSAR_SADDR)
#define C66XX_set_i2c_icsar_saddr(v)						C66XX_SET_FIELD_VALUE(C66XX_I2C_ICSAR_RG_ADDR, I2C_ICSAR_SADDR, v)


//------------ I2C data transmit register macros ------------------------------
#define C66XX_get_i2c_icdxr_rg()							C66XX_GET_RG_VALUE(C66XX_I2C_ICDXR_RG_ADDR)
#define C66XX_set_i2c_icdxr_rg(v)							C66XX_SET_RG_VALUE(C66XX_I2C_ICDXR_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_i2c_icdxr_d()								C66XX_GET_FIELD_VALUE(C66XX_I2C_ICDXR_RG_ADDR, I2C_ICDXR_D)
#define C66XX_set_i2c_icdxr_d(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICDXR_RG_ADDR, I2C_ICDXR_D, v)


//------------ I2C mode register macros ---------------------------------------
#define C66XX_get_i2c_icmdr_rg()							C66XX_GET_RG_VALUE(C66XX_I2C_ICMDR_RG_ADDR)
#define C66XX_set_i2c_icmdr_rg(v)							C66XX_SET_RG_VALUE(C66XX_I2C_ICMDR_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_i2c_icmdr_nackmod()						C66XX_GET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_NACKMOD)
#define C66XX_set_i2c_icmdr_nackmod(v)						C66XX_SET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_NACKMOD, v)
#define C66XX_get_i2c_icmdr_free()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_FREE)
#define C66XX_set_i2c_icmdr_free(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_FREE, v)
#define C66XX_get_i2c_icmdr_stt()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_STT)
#define C66XX_set_i2c_icmdr_stt(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_STT, v)
#define C66XX_get_i2c_icmdr_stp()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_STP)
#define C66XX_set_i2c_icmdr_stp(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_STP, v)
#define C66XX_get_i2c_icmdr_mst()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_MST)
#define C66XX_set_i2c_icmdr_mst(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_MST, v)
#define C66XX_get_i2c_icmdr_trx()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_TRX)
#define C66XX_set_i2c_icmdr_trx(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_TRX, v)
#define C66XX_get_i2c_icmdr_xa()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_XA)
#define C66XX_set_i2c_icmdr_xa(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_XA, v)
#define C66XX_get_i2c_icmdr_rm()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_RM)
#define C66XX_set_i2c_icmdr_rm(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_RM, v)
#define C66XX_get_i2c_icmdr_dlb()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_DLB)
#define C66XX_set_i2c_icmdr_dlb(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_DLB, v)
#define C66XX_get_i2c_icmdr_irs()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_IRS)
#define C66XX_set_i2c_icmdr_irs(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_IRS, v)
#define C66XX_get_i2c_icmdr_stb()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_STB)
#define C66XX_set_i2c_icmdr_stb(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_STB, v)
#define C66XX_get_i2c_icmdr_fdf()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_FDF)
#define C66XX_set_i2c_icmdr_fdf(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_FDF, v)
#define C66XX_get_i2c_icmdr_bc()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_BC)
#define C66XX_set_i2c_icmdr_bc(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICMDR_RG_ADDR, I2C_ICMDR_BC, v)

	// direct bit set macros
#define C66XX_SET_I2C_ICMDR_NACKMOD_ON						C66XX_set_i2c_icmdr_nackmod(C66XX_ON)
#define C66XX_SET_I2C_ICMDR_NACKMOD_OFF						C66XX_set_i2c_icmdr_nackmod(C66XX_OFF)
#define C66XX_SET_I2C_ICMDR_FREE_ON							C66XX_set_i2c_icmdr_free(C66XX_ON)
#define C66XX_SET_I2C_ICMDR_FREE_OFF						C66XX_set_i2c_icmdr_free(C66XX_OFF)
#define C66XX_SET_I2C_ICMDR_STT_ON							C66XX_set_i2c_icmdr_stt(C66XX_ON)
#define C66XX_SET_I2C_ICMDR_STT_OFF							C66XX_set_i2c_icmdr_stt(C66XX_OFF)
#define C66XX_SET_I2C_ICMDR_STP_ON							C66XX_set_i2c_icmdr_stp(C66XX_ON)
#define C66XX_SET_I2C_ICMDR_STP_OFF							C66XX_set_i2c_icmdr_stp(C66XX_OFF)
#define C66XX_SET_I2C_ICMDR_MST_ON							C66XX_set_i2c_icmdr_mst(C66XX_ON)
#define C66XX_SET_I2C_ICMDR_MST_OFF							C66XX_set_i2c_icmdr_mst(C66XX_OFF)
#define C66XX_SET_I2C_ICMDR_TRX_ON							C66XX_set_i2c_icmdr_trx(C66XX_ON)
#define C66XX_SET_I2C_ICMDR_TRX_OFF							C66XX_set_i2c_icmdr_trx(C66XX_OFF)
#define C66XX_SET_I2C_ICMDR_XA_OFF							C66XX_set_i2c_icmdr_xa(C66XX_OFF)
#define C66XX_SET_I2C_ICMDR_XA_ON							C66XX_set_i2c_icmdr_xa(C66XX_ON)
#define C66XX_SET_I2C_ICMDR_RM_OFF							C66XX_set_i2c_icmdr_rm(C66XX_OFF)
#define C66XX_SET_I2C_ICMDR_RM_ON							C66XX_set_i2c_icmdr_rm(C66XX_ON)
#define C66XX_SET_I2C_ICMDR_DLB_OFF							C66XX_set_i2c_icmdr_dlb(C66XX_OFF)
#define C66XX_SET_I2C_ICMDR_DLB_ON							C66XX_set_i2c_icmdr_dlb(C66XX_ON)
#define C66XX_SET_I2C_ICMDR_IRS_OFF							C66XX_set_i2c_icmdr_irs(C66XX_OFF)
#define C66XX_SET_I2C_ICMDR_IRS_ON							C66XX_set_i2c_icmdr_irs(C66XX_ON)
#define C66XX_SET_I2C_ICMDR_STB_OFF							C66XX_set_i2c_icmdr_stb(C66XX_OFF)
#define C66XX_SET_I2C_ICMDR_STB_ON							C66XX_set_i2c_icmdr_stb(C66XX_ON)
#define C66XX_SET_I2C_ICMDR_FDF_OFF							C66XX_set_i2c_icmdr_fdf(C66XX_OFF)
#define C66XX_SET_I2C_ICMDR_FDF_ON							C66XX_set_i2c_icmdr_fdf(C66XX_ON)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_I2C_ICMDR_NACKMOD_IS_ON						(C66XX_get_i2c_icmdr_nackmod() == C66XX_ON)
#define C66XX_I2C_ICMDR_FREE_IS_ON							(C66XX_get_i2c_icmdr_free() == C66XX_ON)
#define C66XX_I2C_ICMDR_STT_IS_ON							(C66XX_get_i2c_icmdr_stt() == C66XX_ON)
#define C66XX_I2C_ICMDR_STP_IS_ON							(C66XX_get_i2c_icmdr_stp() == C66XX_ON)
#define C66XX_I2C_ICMDR_MST_IS_ON							(C66XX_get_i2c_icmdr_mst() == C66XX_ON)
#define C66XX_I2C_ICMDR_TRX_IS_ON							(C66XX_get_i2c_icmdr_trx() == C66XX_ON)
#define C66XX_I2C_ICMDR_XA_IS_ON							(C66XX_get_i2c_icmdr_xa() == C66XX_ON)
#define C66XX_I2C_ICMDR_RM_IS_ON							(C66XX_get_i2c_icmdr_rm() == C66XX_ON)
#define C66XX_I2C_ICMDR_DLB_IS_ON							(C66XX_get_i2c_icmdr_dlb() == C66XX_ON)
#define C66XX_I2C_ICMDR_IRS_IS_ON							(C66XX_get_i2c_icmdr_irs() == C66XX_ON)
#define C66XX_I2C_ICMDR_STB_IS_ON							(C66XX_get_i2c_icmdr_stb() == C66XX_ON)
#define C66XX_I2C_ICMDR_FDF_IS_ON							(C66XX_get_i2c_icmdr_fdf() == C66XX_ON)


//------------ I2C interrupt vector register macros ---------------------------
#define C66XX_get_i2c_icivr_rg()							C66XX_GET_RG_VALUE(C66XX_I2C_ICIVR_RG_ADDR)

	// dedicated bit specific macros
#define C66XX_get_i2c_icivr_intcode()						C66XX_GET_FIELD_VALUE(C66XX_I2C_ICIVR_RG_ADDR, I2C_ICIVR_INTCODE)


//------------ I2C extended mode register macros ------------------------------
#define C66XX_get_i2c_icemdr_rg()							C66XX_GET_RG_VALUE(C66XX_I2C_ICEMDR_RG_ADDR)
#define C66XX_set_i2c_icemdr_rg(v)							C66XX_SET_RG_VALUE(C66XX_I2C_ICEMDR_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_i2c_icemdr_ignack()						C66XX_GET_FIELD_VALUE(C66XX_I2C_ICEMDR_RG_ADDR, I2C_ICEMDR_IGNACK)
#define C66XX_set_i2c_icemdr_ignack(v)						C66XX_SET_FIELD_VALUE(C66XX_I2C_ICEMDR_RG_ADDR, I2C_ICEMDR_IGNACK, v)
#define C66XX_get_i2c_icemdr_bcm()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICEMDR_RG_ADDR, I2C_ICEMDR_BCM)
#define C66XX_set_i2c_icemdr_bcm(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICEMDR_RG_ADDR, I2C_ICEMDR_BCM, v)

	// direct bit set macros
#define C66XX_SET_I2C_ICEMDR_IGNACK_ON						C66XX_set_i2c_icemdr_ignack(C66XX_ON)
#define C66XX_SET_I2C_ICEMDR_IGNACK_OFF						C66XX_set_i2c_icemdr_ignack(C66XX_OFF)
#define C66XX_SET_I2C_ICEMDR_BCM_ON							C66XX_set_i2c_icemdr_bcm(C66XX_ON)
#define C66XX_SET_I2C_ICEMDR_BCM_OFF						C66XX_set_i2c_icemdr_bcm(C66XX_OFF)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_I2C_ICEMDR_IGNACK_IS_ON						(C66XX_get_i2c_icemdr_ignack() == C66XX_ON)
#define C66XX_I2C_ICEMDR_BCM_IS_ON							(C66XX_get_i2c_icemdr_bcm() == C66XX_ON)


//------------ I2C prescaler register macros ----------------------------------
#define C66XX_get_i2c_icpsc_rg()							C66XX_GET_RG_VALUE(C66XX_I2C_ICPSC_RG_ADDR)
#define C66XX_set_i2c_icpsc_rg(v)							C66XX_SET_RG_VALUE(C66XX_I2C_ICPSC_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_i2c_icpsc_ipsc()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICPSC_RG_ADDR, I2C_ICPSC_IPSC)
#define C66XX_set_i2c_icpsc_ipsc(v)							C66XX_SET_FIELD_VALUE(C66XX_I2C_ICPSC_RG_ADDR, I2C_ICPSC_IPSC, v)


//------------ I2C peripheral identification 1 register macros ----------------
#define C66XX_get_i2c_icpid1_rg()							C66XX_GET_RG_VALUE(C66XX_I2C_ICPID1_RG_ADDR)
#define C66XX_set_i2c_icpid1_rg(v)							C66XX_SET_RG_VALUE(C66XX_I2C_ICPID1_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_i2c_icpid1_class()						C66XX_GET_FIELD_VALUE(C66XX_I2C_ICPID1_RG_ADDR, I2C_ICPID1_CLASS)
#define C66XX_set_i2c_icpid1_class(v)						C66XX_SET_FIELD_VALUE(C66XX_I2C_ICPID1_RG_ADDR, I2C_ICPID1_CLASS, v)
#define C66XX_get_i2c_icpid1_revision()						C66XX_GET_FIELD_VALUE(C66XX_I2C_ICPID1_RG_ADDR, I2C_ICPID1_REVISION)
#define C66XX_set_i2c_icpid1_revision(v)					C66XX_SET_FIELD_VALUE(C66XX_I2C_ICPID1_RG_ADDR, I2C_ICPID1_REVISION, v)


//------------ I2C peripheral identification 2 register macros ----------------
#define C66XX_get_i2c_icpid2_rg()							C66XX_GET_RG_VALUE(C66XX_I2C_ICPID2_RG_ADDR)
#define C66XX_set_i2c_icpid2_rg(v)							C66XX_SET_RG_VALUE(C66XX_I2C_ICPID2_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_i2c_icpid2_type()							C66XX_GET_FIELD_VALUE(C66XX_I2C_ICPID2_RG_ADDR, I2C_ICPID2_TYPE)
#define C66XX_set_i2c_icpid2_type(v)						C66XX_SET_FIELD_VALUE(C66XX_I2C_ICPID2_RG_ADDR, I2C_ICPID2_TYPE, v)
//=============================================================================



//=============================================================================
//============ EMIF16 configuration macros ====================================
//=============================================================================

//------------ Revision code and status register macros -----------------------
#define C66XX_get_emif16_rcsr_rg()							C66XX_GET_RG_VALUE(C66XX_EMIF16_RCSR_RG_ADDR)

	// dedicated bit specific macros
#define C66XX_get_emif16_rcsr_be()							C66XX_GET_FIELD_VALUE(C66XX_EMIF16_RCSR_RG_ADDR, EMIF16_RCSR_BE)
#define C66XX_get_emif16_rcsr_mod_id()						C66XX_GET_FIELD_VALUE(C66XX_EMIF16_RCSR_RG_ADDR, EMIF16_RCSR_MOD_ID)
#define C66XX_get_emif16_rcsr_mj_rev()						C66XX_GET_FIELD_VALUE(C66XX_EMIF16_RCSR_RG_ADDR, EMIF16_RCSR_MJ_REV)
#define C66XX_get_emif16_rcsr_min_rev()						C66XX_GET_FIELD_VALUE(C66XX_EMIF16_RCSR_RG_ADDR, EMIF16_RCSR_MIN_REV)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_EMIF16_RCSR_BE_IS_ON							(C66XX_get_emif16_rcsr_be() == C66XX_ON)
#define C66XX_EMIF16_RCSR_MOD_ID_IS_DEFAULT_VALUE			(C66XX_get_emif16_rcsr_mod_id() == C66XX_EMIF16_RCSR_MOD_ID_DEFAULT_VALUE)
#define C66XX_EMIF16_RCSR_MJ_REV_IS_DEFAULT_VALUE			(C66XX_get_emif16_rcsr_mj_rev() == C66XX_EMIF16_RCSR_MJ_REV_DEFAULT_VALUE)
#define C66XX_EMIF16_RCSR_MIN_REV_IS_DEFAULT_VALUE			(C66XX_get_emif16_rcsr_min_rev() == C66XX_EMIF16_RCSR_MIN_REV_DEFAULT_VALUE)


//------------ Async wait cycle config register macros ------------------------
#define C66XX_get_emif16_awccr_rg()							C66XX_GET_RG_VALUE(C66XX_EMIF16_AWCCR_RG_ADDR)
#define C66XX_set_emif16_awccr_rg(v)						C66XX_SET_RG_VALUE(C66XX_EMIF16_AWCCR_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_emif16_awccr_wp1()						C66XX_GET_FIELD_VALUE(C66XX_EMIF16_AWCCR_RG_ADDR, EMIF16_AWCCR_WP1)
#define C66XX_set_emif16_awccr_wp1(v)						C66XX_SET_FIELD_VALUE(C66XX_EMIF16_AWCCR_RG_ADDR, EMIF16_AWCCR_WP1, v)
#define C66XX_get_emif16_awccr_wp0()						C66XX_GET_FIELD_VALUE(C66XX_EMIF16_AWCCR_RG_ADDR, EMIF16_AWCCR_WP0)
#define C66XX_set_emif16_awccr_wp0(v)						C66XX_SET_FIELD_VALUE(C66XX_EMIF16_AWCCR_RG_ADDR, EMIF16_AWCCR_WP0, v)
#define C66XX_get_emif16_awccr_cs5_wait()					C66XX_GET_FIELD_VALUE(C66XX_EMIF16_AWCCR_RG_ADDR, EMIF16_AWCCR_CS5_WAIT)
#define C66XX_set_emif16_awccr_cs5_wait(v)					C66XX_SET_FIELD_VALUE(C66XX_EMIF16_AWCCR_RG_ADDR, EMIF16_AWCCR_CS5_WAIT, v)
#define C66XX_get_emif16_awccr_cs4_wait()					C66XX_GET_FIELD_VALUE(C66XX_EMIF16_AWCCR_RG_ADDR, EMIF16_AWCCR_CS4_WAIT)
#define C66XX_set_emif16_awccr_cs4_wait(v)					C66XX_SET_FIELD_VALUE(C66XX_EMIF16_AWCCR_RG_ADDR, EMIF16_AWCCR_CS4_WAIT, v)
#define C66XX_get_emif16_awccr_cs3_wait()					C66XX_GET_FIELD_VALUE(C66XX_EMIF16_AWCCR_RG_ADDR, EMIF16_AWCCR_CS3_WAIT)
#define C66XX_set_emif16_awccr_cs3_wait(v)					C66XX_SET_FIELD_VALUE(C66XX_EMIF16_AWCCR_RG_ADDR, EMIF16_AWCCR_CS3_WAIT, v)
#define C66XX_get_emif16_awccr_cs2_wait()					C66XX_GET_FIELD_VALUE(C66XX_EMIF16_AWCCR_RG_ADDR, EMIF16_AWCCR_CS2_WAIT)
#define C66XX_set_emif16_awccr_cs2_wait(v)					C66XX_SET_FIELD_VALUE(C66XX_EMIF16_AWCCR_RG_ADDR, EMIF16_AWCCR_CS2_WAIT, v)
#define C66XX_get_emif16_awccr_max_ext_wait()				C66XX_GET_FIELD_VALUE(C66XX_EMIF16_AWCCR_RG_ADDR, EMIF16_AWCCR_MAX_EXT_WAIT)
#define C66XX_set_emif16_awccr_max_ext_wait(v)				C66XX_SET_FIELD_VALUE(C66XX_EMIF16_AWCCR_RG_ADDR, EMIF16_AWCCR_MAX_EXT_WAIT, v)

	// direct bit set macros
#define C66XX_SET_EMIF16_AWCCR_WP1_ON						C66XX_set_emif16_awccr_wp1(C66XX_ON)
#define C66XX_SET_EMIF16_AWCCR_WP1_OFF						C66XX_set_emif16_awccr_wp1(C66XX_OFF)
#define C66XX_SET_EMIF16_AWCCR_WP0_ON						C66XX_set_emif16_awccr_wp0(C66XX_ON)
#define C66XX_SET_EMIF16_AWCCR_WP0_OFF						C66XX_set_emif16_awccr_wp0(C66XX_OFF)
#define C66XX_SET_EMIF16_AWCCR_CS5_WAIT1					C66XX_set_emif16_awccr_cs5_wait(C66XX_ON)
#define C66XX_SET_EMIF16_AWCCR_CS5_WAIT0					C66XX_set_emif16_awccr_cs5_wait(C66XX_OFF)
#define C66XX_SET_EMIF16_AWCCR_CS4_WAIT1					C66XX_set_emif16_awccr_cs4_wait(C66XX_ON)
#define C66XX_SET_EMIF16_AWCCR_CS4_WAIT0					C66XX_set_emif16_awccr_cs4_wait(C66XX_OFF)
#define C66XX_SET_EMIF16_AWCCR_CS3_WAIT1					C66XX_set_emif16_awccr_cs3_wait(C66XX_ON)
#define C66XX_SET_EMIF16_AWCCR_CS3_WAIT0					C66XX_set_emif16_awccr_cs3_wait(C66XX_OFF)
#define C66XX_SET_EMIF16_AWCCR_CS2_WAIT1					C66XX_set_emif16_awccr_cs2_wait(C66XX_ON)
#define C66XX_SET_EMIF16_AWCCR_CS2_WAIT0					C66XX_set_emif16_awccr_cs2_wait(C66XX_OFF)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_EMIF16_AWCCR_WP1_IS_ON						(C66XX_get_emif16_awccr_wp1() == C66XX_ON)
#define C66XX_EMIF16_AWCCR_WP0_IS_ON						(C66XX_get_emif16_awccr_wp0() == C66XX_ON)
#define C66XX_EMIF16_AWCCR_CS5_WAIT_IS_WAIT1				(C66XX_get_emif16_awccr_cs5_wait() == C66XX_ON)
#define C66XX_EMIF16_AWCCR_CS4_WAIT_IS_WAIT1				(C66XX_get_emif16_awccr_cs4_wait() == C66XX_ON)
#define C66XX_EMIF16_AWCCR_CS3_WAIT_IS_WAIT1				(C66XX_get_emif16_awccr_cs3_wait() == C66XX_ON)
#define C66XX_EMIF16_AWCCR_CS2_WAIT_IS_WAIT1				(C66XX_get_emif16_awccr_cs2_wait() == C66XX_ON)


//------------ Async config registers macros ----------------------------------
// i index corresonds to Async config registers number
#define C66XX_get_emif16_acr_rg_addr(i)						(C66XX_EMIF16_A1CR_RG_ADDR + i * 0x4)
#define C66XX_get_emif16_acr_rg(i)							C66XX_GET_RG_VALUE(C66XX_get_emif16_acr_rg_addr(i))
#define C66XX_set_emif16_acr_rg(i,v)						C66XX_SET_RG_VALUE(C66XX_get_emif16_acr_rg_addr(i), v)

	// dedicated bit specific macros
#define C66XX_get_emif16_acr_ss(i)							C66XX_GET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_SS)
#define C66XX_set_emif16_acr_ss(i,v)						C66XX_SET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_SS, v)
#define C66XX_get_emif16_acr_ew(i)							C66XX_GET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_EW)
#define C66XX_set_emif16_acr_ew(i,v)						C66XX_SET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_EW, v)
#define C66XX_get_emif16_acr_w_setup(i)						C66XX_GET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_W_SETUP)
#define C66XX_set_emif16_acr_w_setup(i,v)					C66XX_SET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_W_SETUP, v)
#define C66XX_get_emif16_acr_w_strobe(i)					C66XX_GET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_W_STROBE)
#define C66XX_set_emif16_acr_w_strobe(i,v)					C66XX_SET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_W_STROBE, v)
#define C66XX_get_emif16_acr_w_hold(i)						C66XX_GET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_W_HOLD)
#define C66XX_set_emif16_acr_w_hold(i,v)					C66XX_SET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_W_HOLD, v)
#define C66XX_get_emif16_acr_r_setup(i)						C66XX_GET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_R_SETUP)
#define C66XX_set_emif16_acr_r_setup(i,v)					C66XX_SET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_R_SETUP, v)
#define C66XX_get_emif16_acr_r_strobe(i)					C66XX_GET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_R_STROBE)
#define C66XX_set_emif16_acr_r_strobe(i,v)					C66XX_SET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_R_STROBE, v)
#define C66XX_get_emif16_acr_r_hold(i)						C66XX_GET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_R_HOLD)
#define C66XX_set_emif16_acr_r_hold(i,v)					C66XX_SET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_R_HOLD, v)
#define C66XX_get_emif16_acr_ta(i)							C66XX_GET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_TA)
#define C66XX_set_emif16_acr_ta(i,v)						C66XX_SET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_TA, v)
#define C66XX_get_emif16_acr_asize(i)						C66XX_GET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_ASIZE)
#define C66XX_set_emif16_acr_asize(i,v)						C66XX_SET_FIELD_VALUE(C66XX_get_emif16_acr_rg_addr(i), EMIF16_A1CR_ASIZE, v)

	// direct bit set macros
#define C66XX_SET_EMIF16_ACR_SS_ON(i)						C66XX_set_emif16_acr_ss(i, C66XX_ON)
#define C66XX_SET_EMIF16_ACR_SS_OFF(i)						C66XX_set_emif16_acr_ss(i, C66XX_OFF)
#define C66XX_SET_EMIF16_ACR_EW_ON(i)						C66XX_set_emif16_acr_ew(i, C66XX_ON)
#define C66XX_SET_EMIF16_ACR_EW_OFF(i)						C66XX_set_emif16_acr_ew(i, C66XX_OFF)
#define C66XX_SET_EMIF16_ACR_ASIZE_16BIT(i)					C66XX_set_emif16_acr_asize(i, C66XX_EMIF16_A1CR_ASIZE_16BIT)
#define C66XX_SET_EMIF16_ACR_ASIZE_8BIT(i)					C66XX_set_emif16_acr_asize(i, C66XX_EMIF16_A1CR_ASIZE_8BIT)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_EMIF16_ACR_SS_IS_ON(i)						(C66XX_get_emif16_acr_ss(i) == C66XX_ON)
#define C66XX_EMIF16_ACR_EW_IS_ON(i)						(C66XX_get_emif16_acr_ew(i) == C66XX_ON)
#define C66XX_EMIF16_ACR_ASIZE_IS_16BIT(i)					(C66XX_get_emif16_acr_asize(i) == C66XX_EMIF16_A1CR_ASIZE_16BIT)


//------------ Interrupt raw register macros ----------------------------------
#define C66XX_get_emif16_irr_rg()							C66XX_GET_RG_VALUE(C66XX_EMIF16_IRR_RG_ADDR)
#define C66XX_set_emif16_irr_rg(v)							C66XX_SET_RG_VALUE(C66XX_EMIF16_IRR_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_emif16_irr_wr()							C66XX_GET_FIELD_VALUE(C66XX_EMIF16_IRR_RG_ADDR, EMIF16_IRR_WR)
#define C66XX_set_emif16_irr_wr(v)							C66XX_SET_FIELD_VALUE(C66XX_EMIF16_IRR_RG_ADDR, EMIF16_IRR_WR, v)
#define C66XX_get_emif16_irr_at()							C66XX_GET_FIELD_VALUE(C66XX_EMIF16_IRR_RG_ADDR, EMIF16_IRR_AT)
#define C66XX_set_emif16_irr_at(v)							C66XX_SET_FIELD_VALUE(C66XX_EMIF16_IRR_RG_ADDR, EMIF16_IRR_AT, v)

	// direct bit set macros
#define C66XX_CLEAR_EMIF16_IRR_AT							C66XX_set_emif16_irr_at(C66XX_ON)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_EMIF16_IRR_AT_IS_ON							(C66XX_get_emif16_irr_at() == C66XX_ON)


//------------ Interrupt masked register macros -------------------------------
#define C66XX_get_emif16_imr_rg()							C66XX_GET_RG_VALUE(C66XX_EMIF16_IMR_RG_ADDR)
#define C66XX_set_emif16_imr_rg(v)							C66XX_SET_RG_VALUE(C66XX_EMIF16_IMR_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_emif16_imr_wr_masked()					C66XX_GET_FIELD_VALUE(C66XX_EMIF16_IMR_RG_ADDR, EMIF16_IMR_WR_MASKED)
#define C66XX_set_emif16_imr_wr_masked(v)					C66XX_SET_FIELD_VALUE(C66XX_EMIF16_IMR_RG_ADDR, EMIF16_IMR_WR_MASKED, v)
#define C66XX_get_emif16_imr_at_masked()					C66XX_GET_FIELD_VALUE(C66XX_EMIF16_IMR_RG_ADDR, EMIF16_IMR_AT_MASKED)
#define C66XX_set_emif16_imr_at_masked(v)					C66XX_SET_FIELD_VALUE(C66XX_EMIF16_IMR_RG_ADDR, EMIF16_IMR_AT_MASKED, v)

	// direct bit set macros
#define C66XX_CLEAR_EMIF16_IMR_AT_MASKED					C66XX_set_emif16_imr_at_masked(C66XX_ON)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_EMIF16_IMR_AT_MASKED_IS_ON					(C66XX_get_emif16_imr_at_masked() == C66XX_ON)


//------------ Interrupt mask set register macros -----------------------------
#define C66XX_get_emif16_imsr_rg()							C66XX_GET_RG_VALUE(C66XX_EMIF16_IMSR_RG_ADDR)
#define C66XX_set_emif16_imsr_rg(v)							C66XX_SET_RG_VALUE(C66XX_EMIF16_IMSR_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_emif16_imsr_wr_mask_set()					C66XX_GET_FIELD_VALUE(C66XX_EMIF16_IMSR_RG_ADDR, EMIF16_IMSR_WR_MASK_SET)
#define C66XX_set_emif16_imsr_wr_mask_set(v)				C66XX_SET_FIELD_VALUE(C66XX_EMIF16_IMSR_RG_ADDR, EMIF16_IMSR_WR_MASK_SET, v)
#define C66XX_get_emif16_imsr_at_mask_set()					C66XX_GET_FIELD_VALUE(C66XX_EMIF16_IMSR_RG_ADDR, EMIF16_IMSR_AT_MASK_SET)
#define C66XX_set_emif16_imsr_at_mask_set(v)				C66XX_SET_FIELD_VALUE(C66XX_EMIF16_IMSR_RG_ADDR, EMIF16_IMSR_AT_MASK_SET, v)

	// direct bit set macros
#define C66XX_SET_EMIF16_IMSR_AT_MASK_SET_ON				C66XX_set_emif16_imsr_at_mask_set(C66XX_ON)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_EMIF16_IMSR_AT_MASK_SET_IS_ON					(C66XX_get_emif16_imsr_at_mask_set() == C66XX_ON)


//------------ Interrupt mask clear register macros -----------------------------
#define C66XX_get_emif16_imcr_rg()							C66XX_GET_RG_VALUE(C66XX_EMIF16_IMCR_RG_ADDR)
#define C66XX_set_emif16_imcr_rg(v)							C66XX_SET_RG_VALUE(C66XX_EMIF16_IMCR_RG_ADDR, v)

	// dedicated bit specific macros
#define C66XX_get_emif16_imcr_wr_mask_clr()					C66XX_GET_FIELD_VALUE(C66XX_EMIF16_IMCR_RG_ADDR, EMIF16_IMCR_WR_MASK_CLR)
#define C66XX_set_emif16_imcr_wr_mask_clr(v)				C66XX_SET_FIELD_VALUE(C66XX_EMIF16_IMCR_RG_ADDR, EMIF16_IMCR_WR_MASK_CLR, v)
#define C66XX_get_emif16_imcr_at_mask_clr()					C66XX_GET_FIELD_VALUE(C66XX_EMIF16_IMCR_RG_ADDR, EMIF16_IMCR_AT_MASK_CLR)
#define C66XX_set_emif16_imcr_at_mask_clr(v)				C66XX_SET_FIELD_VALUE(C66XX_EMIF16_IMCR_RG_ADDR, EMIF16_IMCR_AT_MASK_CLR, v)

	// direct bit set macros
#define C66XX_SET_EMIF16_IMCR_AT_MASK_CLR_ON				C66XX_set_emif16_imcr_at_mask_clr(C66XX_ON)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_EMIF16_IMCR_AT_MASK_CLR_IS_ON					(C66XX_get_emif16_imcr_at_mask_clr() == C66XX_ON)
//=============================================================================



//=============================================================================
//============ Timer registers macros =========================================
//=============================================================================

//------------ Emulation Management and Clock Speed register macros -----------
#define C66XX_get_timer_emumgt_clkspd_rg(timer)				C66XX_GET_RG_VALUE(C66XX_TIMER_EMUMGT_CLKSPD_RG_ADDR(timer))

	// dedicated bit specific macros
#define C66XX_get_timer_emumgt_clkspd_clkdiv(timer)			C66XX_GET_FIELD_VALUE(C66XX_TIMER_EMUMGT_CLKSPD_RG_ADDR(timer), TIMER_EMUMGT_CLKSPD_CLKDIV)


//------------ Counter register low register macros ---------------------------
#define C66XX_get_timer_cntlo_rg(timer)						C66XX_GET_RG_VALUE(C66XX_TIMER_CNTLO_RG_ADDR(timer))
#define C66XX_set_timer_cntlo_rg(timer,v)					C66XX_SET_RG_VALUE(C66XX_TIMER_CNTLO_RG_ADDR(timer), v)


//------------ Counter register high register macros --------------------------
#define C66XX_get_timer_cnthi_rg(timer)						C66XX_GET_RG_VALUE(C66XX_TIMER_CNTHI_RG_ADDR(timer))
#define C66XX_set_timer_cnthi_rg(timer,v)					C66XX_SET_RG_VALUE(C66XX_TIMER_CNTHI_RG_ADDR(timer), v)


//------------ Period register low register macros ----------------------------
#define C66XX_get_timer_prdlo_rg(timer)						C66XX_GET_RG_VALUE(C66XX_TIMER_PRDLO_RG_ADDR(timer))
#define C66XX_set_timer_prdlo_rg(timer,v)					C66XX_SET_RG_VALUE(C66XX_TIMER_PRDLO_RG_ADDR(timer), v)


//------------ Period register high register macros ---------------------------
#define C66XX_get_timer_prdhi_rg(timer)						C66XX_GET_RG_VALUE(C66XX_TIMER_PRDHI_RG_ADDR(timer))
#define C66XX_set_timer_prdhi_rg(timer,v)					C66XX_SET_RG_VALUE(C66XX_TIMER_PRDHI_RG_ADDR(timer), v)


//------------ Timer control register macros ----------------------------------
#define C66XX_get_timer_tcr_rg(timer)						C66XX_GET_RG_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer))
#define C66XX_set_timer_tcr_rg(timer,v)						C66XX_SET_RG_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), v)

	// dedicated bit specific macros
#define C66XX_get_timer_tcr_readrstmode_hi(timer)			C66XX_GET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_READRSTMODE_HI)
#define C66XX_set_timer_tcr_readrstmode_hi(timer,v)			C66XX_SET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_READRSTMODE_HI, v)
#define C66XX_get_timer_tcr_enamode_hi(timer)				C66XX_GET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_ENAMODE_HI)
#define C66XX_set_timer_tcr_enamode_hi(timer,v)				C66XX_SET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_ENAMODE_HI, v)
#define C66XX_get_timer_tcr_pwid_hi(timer)					C66XX_GET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_PWID_HI)
#define C66XX_set_timer_tcr_pwid_hi(timer,v)				C66XX_SET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_PWID_HI, v)
#define C66XX_get_timer_tcr_cp_hi(timer)					C66XX_GET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_CP_HI)
#define C66XX_set_timer_tcr_cp_hi(timer,v)					C66XX_SET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_CP_HI, v)
#define C66XX_get_timer_tcr_invoutp_hi(timer)				C66XX_GET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_INVOUTP_HI)
#define C66XX_set_timer_tcr_invoutp_hi(timer,v)				C66XX_SET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_INVOUTP_HI, v)
#define C66XX_get_timer_tcr_tstat_hi(timer)					C66XX_GET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_TSTAT_HI)
#define C66XX_get_timer_tcr_capevtmode_lo(timer)			C66XX_GET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_CAPEVTMODE_LO)
#define C66XX_set_timer_tcr_capevtmode_lo(timer,v)			C66XX_SET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_CAPEVTMODE_LO, v)
#define C66XX_get_timer_tcr_capmode_lo(timer)				C66XX_GET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_CAPMODE_LO)
#define C66XX_set_timer_tcr_capmode_lo(timer,v)				C66XX_SET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_CAPMODE_LO, v)
#define C66XX_get_timer_tcr_readrstmode_lo(timer)			C66XX_GET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_READRSTMODE_LO)
#define C66XX_set_timer_tcr_readrstmode_lo(timer,v)			C66XX_SET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_READRSTMODE_LO, v)
#define C66XX_get_timer_tcr_tien_lo(timer)					C66XX_GET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_TIEN_LO)
#define C66XX_set_timer_tcr_tien_lo(timer,v)				C66XX_SET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_TIEN_LO, v)
#define C66XX_get_timer_tcr_clksrc_lo(timer)				C66XX_GET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_CLKSRC_LO)
#define C66XX_set_timer_tcr_clksrc_lo(timer,v)				C66XX_SET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_CLKSRC_LO, v)
#define C66XX_get_timer_tcr_enamode_lo(timer)				C66XX_GET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_ENAMODE_LO)
#define C66XX_set_timer_tcr_enamode_lo(timer,v)				C66XX_SET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_ENAMODE_LO, v)
#define C66XX_get_timer_tcr_pwid_lo(timer)					C66XX_GET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_PWID_LO)
#define C66XX_set_timer_tcr_pwid_lo(timer,v)				C66XX_SET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_PWID_LO, v)
#define C66XX_get_timer_tcr_cp_lo(timer)					C66XX_GET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_CP_LO)
#define C66XX_set_timer_tcr_cp_lo(timer,v)					C66XX_SET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_CP_LO, v)
#define C66XX_get_timer_tcr_invinp_lo(timer)				C66XX_GET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_INVINP_LO)
#define C66XX_set_timer_tcr_invinp_lo(timer,v)				C66XX_SET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_INVINP_LO, v)
#define C66XX_get_timer_tcr_invoutp_lo(timer)				C66XX_GET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_INVOUTP_LO)
#define C66XX_set_timer_tcr_invoutp_lo(timer,v)				C66XX_SET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_INVOUTP_LO, v)
#define C66XX_get_timer_tcr_tstat_lo(timer)					C66XX_GET_FIELD_VALUE(C66XX_TIMER_TCR_RG_ADDR(timer), TIMER_TCR_TSTAT_LO)

	// direct bit set macros
#define C66XX_SET_TIMER_TCR_READRSTMODE_HI_ON(timer)			C66XX_set_timer_tcr_readrstmode_hi(timer, C66XX_ON)
#define C66XX_SET_TIMER_TCR_READRSTMODE_HI_OFF(timer)			C66XX_set_timer_tcr_readrstmode_hi(timer, C66XX_OFF)
#define C66XX_SET_TIMER_TCR_ENAMODE_HI_DISABLED(timer)			C66XX_set_timer_tcr_enamode_hi(timer, C66XX_TIMER_TCR_ENAMODE_DISABLED)
#define C66XX_SET_TIMER_TCR_ENAMODE_HI_ONE_SHOT(timer)			C66XX_set_timer_tcr_enamode_hi(timer, C66XX_TIMER_TCR_ENAMODE_ONE_SHOT)
#define C66XX_SET_TIMER_TCR_ENAMODE_HI_CONT(timer)				C66XX_set_timer_tcr_enamode_hi(timer, C66XX_TIMER_TCR_ENAMODE_CONT)
#define C66XX_SET_TIMER_TCR_ENAMODE_HI_CONT_RELOAD(timer)		C66XX_set_timer_tcr_enamode_hi(timer, C66XX_TIMER_TCR_ENAMODE_CONT_RELOAD)
#define C66XX_SET_TIMER_TCR_PWID_HI_1_CLK(timer)				C66XX_set_timer_tcr_pwid_hi(timer, C66XX_TIMER_TCR_PWID_1_CLK)
#define C66XX_SET_TIMER_TCR_PWID_HI_2_CLK(timer)				C66XX_set_timer_tcr_pwid_hi(timer, C66XX_TIMER_TCR_PWID_2_CLK)
#define C66XX_SET_TIMER_TCR_PWID_HI_3_CLK(timer)				C66XX_set_timer_tcr_pwid_hi(timer, C66XX_TIMER_TCR_PWID_3_CLK)
#define C66XX_SET_TIMER_TCR_PWID_HI_4_CLK(timer)				C66XX_set_timer_tcr_pwid_hi(timer, C66XX_TIMER_TCR_PWID_4_CLK)
#define C66XX_SET_TIMER_TCR_CP_HI_ON(timer)						C66XX_set_timer_tcr_cp_hi(timer, C66XX_ON)
#define C66XX_SET_TIMER_TCR_CP_HI_OFF(timer)					C66XX_set_timer_tcr_cp_hi(timer, C66XX_OFF)
#define C66XX_SET_TIMER_TCR_INVOUTP_HI_ON(timer)				C66XX_set_timer_tcr_invoutp_hi(timer, C66XX_ON)
#define C66XX_SET_TIMER_TCR_INVOUTP_HI_OFF(timer)				C66XX_set_timer_tcr_invoutp_hi(timer, C66XX_OFF)
#define C66XX_SET_TIMER_TCR_CAPEVTMODE_LO_RISING_EDGE(timer)	C66XX_set_timer_tcr_capevtmode_lo(timer, C66XX_TIMER_TCR_CAPEVTMODE_RISING_EDGE)
#define C66XX_SET_TIMER_TCR_CAPEVTMODE_LO_FALLING_EDGE(timer)	C66XX_set_timer_tcr_capevtmode_lo(timer, C66XX_TIMER_TCR_CAPEVTMODE_FALLING_EDGE)
#define C66XX_SET_TIMER_TCR_CAPEVTMODE_LO_ANY_EDGE(timer)		C66XX_set_timer_tcr_capevtmode_lo(timer, C66XX_TIMER_TCR_CAPEVTMODE_ANY_EDGE)
#define C66XX_SET_TIMER_TCR_CAPMODE_LO_ON(timer)				C66XX_set_timer_tcr_capmode_lo(timer, C66XX_ON)
#define C66XX_SET_TIMER_TCR_CAPMODE_LO_OFF(timer)				C66XX_set_timer_tcr_capmode_lo(timer, C66XX_OFF)
#define C66XX_SET_TIMER_TCR_READRSTMODE_LO_ON(timer)			C66XX_set_timer_tcr_readrstmode_lo(timer, C66XX_ON)
#define C66XX_SET_TIMER_TCR_READRSTMODE_LO_OFF(timer)			C66XX_set_timer_tcr_readrstmode_lo(timer, C66XX_OFF)
#define C66XX_SET_TIMER_TCR_TIEN_LO_ON(timer)					C66XX_set_timer_tcr_tien_lo(timer, C66XX_ON)
#define C66XX_SET_TIMER_TCR_TIEN_LO_OFF(timer)					C66XX_set_timer_tcr_tien_lo(timer, C66XX_OFF)
#define C66XX_SET_TIMER_TCR_CLKSRC_LO_ON(timer)					C66XX_set_timer_tcr_clksrc_lo(timer, C66XX_ON)
#define C66XX_SET_TIMER_TCR_CLKSRC_LO_OFF(timer)				C66XX_set_timer_tcr_clksrc_lo(timer, C66XX_OFF)
#define C66XX_SET_TIMER_TCR_ENAMODE_LO_DISABLED(timer)			C66XX_set_timer_tcr_enamode_lo(timer, C66XX_TIMER_TCR_ENAMODE_DISABLED)
#define C66XX_SET_TIMER_TCR_ENAMODE_LO_ONE_SHOT(timer)			C66XX_set_timer_tcr_enamode_lo(timer, C66XX_TIMER_TCR_ENAMODE_ONE_SHOT)
#define C66XX_SET_TIMER_TCR_ENAMODE_LO_CONT(timer)				C66XX_set_timer_tcr_enamode_lo(timer, C66XX_TIMER_TCR_ENAMODE_CONT)
#define C66XX_SET_TIMER_TCR_ENAMODE_LO_CONT_RELOAD(timer)		C66XX_set_timer_tcr_enamode_lo(timer, C66XX_TIMER_TCR_ENAMODE_CONT_RELOAD)
#define C66XX_SET_TIMER_TCR_PWID_LO_1_CLK(timer)				C66XX_set_timer_tcr_pwid_lo(timer, C66XX_TIMER_TCR_PWID_1_CLK)
#define C66XX_SET_TIMER_TCR_PWID_LO_2_CLK(timer)				C66XX_set_timer_tcr_pwid_lo(timer, C66XX_TIMER_TCR_PWID_2_CLK)
#define C66XX_SET_TIMER_TCR_PWID_LO_3_CLK(timer)				C66XX_set_timer_tcr_pwid_lo(timer, C66XX_TIMER_TCR_PWID_3_CLK)
#define C66XX_SET_TIMER_TCR_PWID_LO_4_CLK(timer)				C66XX_set_timer_tcr_pwid_lo(timer, C66XX_TIMER_TCR_PWID_4_CLK)
#define C66XX_SET_TIMER_TCR_CP_LO_ON(timer)						C66XX_set_timer_tcr_cp_lo(timer, C66XX_ON)
#define C66XX_SET_TIMER_TCR_CP_LO_OFF(timer)					C66XX_set_timer_tcr_cp_lo(timer, C66XX_OFF)
#define C66XX_SET_TIMER_TCR_INVINP_LO_ON(timer)					C66XX_set_timer_tcr_invinp_lo(timer, C66XX_ON)
#define C66XX_SET_TIMER_TCR_INVINP_LO_OFF(timer)				C66XX_set_timer_tcr_invinp_lo(timer, C66XX_OFF)
#define C66XX_SET_TIMER_TCR_INVOUTP_LO_ON(timer)				C66XX_set_timer_tcr_invoutp_lo(timer, C66XX_ON)
#define C66XX_SET_TIMER_TCR_INVOUTP_LO_OFF(timer)				C66XX_set_timer_tcr_invoutp_lo(timer, C66XX_OFF)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_TIMER_TCR_READRSTMODE_HI_IS_ON(timer)				(C66XX_get_timer_tcr_readrstmode_hi(timer) == C66XX_ON)
#define C66XX_TIMER_TCR_ENAMODE_HI_IS_DISABLED(timer)			(C66XX_get_timer_tcr_enamode_hi(timer) == C66XX_TIMER_TCR_ENAMODE_DISABLED)
#define C66XX_TIMER_TCR_ENAMODE_HI_IS_ONE_SHOT(timer)			(C66XX_get_timer_tcr_enamode_hi(timer) == C66XX_TIMER_TCR_ENAMODE_ONE_SHOT)
#define C66XX_TIMER_TCR_ENAMODE_HI_IS_CONT(timer)				(C66XX_get_timer_tcr_enamode_hi(timer) == C66XX_TIMER_TCR_ENAMODE_CONT)
#define C66XX_TIMER_TCR_ENAMODE_HI_IS_CONT_RELOAD(timer)		(C66XX_get_timer_tcr_enamode_hi(timer) == C66XX_TIMER_TCR_ENAMODE_CONT_RELOAD)
#define C66XX_TIMER_TCR_PWI_HI_IS_1_CLK(timer)					(C66XX_get_timer_tcr_pwid_hi(timer) == C66XX_TIMER_TCR_PWID_1_CLK)
#define C66XX_TIMER_TCR_PWI_HI_IS_2_CLK(timer)					(C66XX_get_timer_tcr_pwid_hi(timer) == C66XX_TIMER_TCR_PWID_2_CLK)
#define C66XX_TIMER_TCR_PWI_HI_IS_3_CLK(timer)					(C66XX_get_timer_tcr_pwid_hi(timer) == C66XX_TIMER_TCR_PWID_3_CLK)
#define C66XX_TIMER_TCR_PWI_HI_IS_4_CLK(timer)					(C66XX_get_timer_tcr_pwid_hi(timer) == C66XX_TIMER_TCR_PWID_4_CLK)
#define C66XX_TIMER_TCR_CP_HI_IS_ON(timer)						(C66XX_get_timer_tcr_cp_hi(timer) == C66XX_ON)
#define C66XX_TIMER_TCR_INVOUTP_HI_IS_ON(timer)					(C66XX_get_timer_tcr_invoutp_hi(timer) == C66XX_ON)
#define C66XX_TIMER_TCR_TSTAT_HI_IS_ON(timer)					(C66XX_get_timer_tcr_tstat_hi(timer) == C66XX_ON)
#define C66XX_TIMER_TCR_CAPEVTMODE_LO_IS_RISING_EDGE(timer)		(C66XX_get_timer_tcr_capevtmode_lo(timer) == C66XX_TIMER_TCR_CAPEVTMODE_RISING_EDGE)
#define C66XX_TIMER_TCR_CAPEVTMODE_LO_IS_FALLING_EDGE(timer)	(C66XX_get_timer_tcr_capevtmode_lo(timer) == C66XX_TIMER_TCR_CAPEVTMODE_FALLING_EDGE)
#define C66XX_TIMER_TCR_CAPEVTMODE_LO_IS_ANY_EDGE(timer)		(C66XX_get_timer_tcr_capevtmode_lo(timer) == C66XX_TIMER_TCR_CAPEVTMODE_ANY_EDGE)
#define C66XX_TIMER_TCR_CAPMODE_LO_IS_ON(timer)					(C66XX_get_timer_tcr_capmode_lo(timer) == C66XX_ON)
#define C66XX_TIMER_TCR_READRSTMODE_LO_IS_ON(timer)				(C66XX_get_timer_tcr_readrstmode_lo(timer) == C66XX_ON)
#define C66XX_TIMER_TCR_TIEN_LO_IS_ON(timer)					(C66XX_get_timer_tcr_tien_lo(timer) == C66XX_ON)
#define C66XX_TIMER_TCR_CLKSRC_LO_IS_ON(timer)					(C66XX_get_timer_tcr_clksrc_lo(timer) == C66XX_ON)
#define C66XX_TIMER_TCR_ENAMODE_LO_IS_DISABLED(timer)			(C66XX_get_timer_tcr_enamode_lo(timer) == C66XX_TIMER_TCR_ENAMODE_DISABLED)
#define C66XX_TIMER_TCR_ENAMODE_LO_IS_ONE_SHOT(timer)			(C66XX_get_timer_tcr_enamode_lo(timer) == C66XX_TIMER_TCR_ENAMODE_ONE_SHOT)
#define C66XX_TIMER_TCR_ENAMODE_LO_IS_CONT(timer)				(C66XX_get_timer_tcr_enamode_lo(timer) == C66XX_TIMER_TCR_ENAMODE_CONT)
#define C66XX_TIMER_TCR_ENAMODE_LO_IS_CONT_RELOAD(timer)		(C66XX_get_timer_tcr_enamode_lo(timer) == C66XX_TIMER_TCR_ENAMODE_CONT_RELOAD)
#define C66XX_TIMER_TCR_PWI_LO_IS_1_CLK(timer)					(C66XX_get_timer_tcr_pwid_lo(timer) == C66XX_TIMER_TCR_PWID_1_CLK)
#define C66XX_TIMER_TCR_PWI_LO_IS_2_CLK(timer)					(C66XX_get_timer_tcr_pwid_lo(timer) == C66XX_TIMER_TCR_PWID_2_CLK)
#define C66XX_TIMER_TCR_PWI_LO_IS_3_CLK(timer)					(C66XX_get_timer_tcr_pwid_lo(timer) == C66XX_TIMER_TCR_PWID_3_CLK)
#define C66XX_TIMER_TCR_PWI_LO_IS_4_CLK(timer)					(C66XX_get_timer_tcr_pwid_lo(timer) == C66XX_TIMER_TCR_PWID_4_CLK)
#define C66XX_TIMER_TCR_CP_LO_IS_ON(timer)						(C66XX_get_timer_tcr_cp_lo(timer) == C66XX_ON)
#define C66XX_TIMER_TCR_INVINP_LO_IS_ON(timer)					(C66XX_get_timer_tcr_invinp_lo(timer) == C66XX_ON)
#define C66XX_TIMER_TCR_INVOUTP_LO_IS_ON(timer)					(C66XX_get_timer_tcr_invoutp_lo(timer) == C66XX_ON)
#define C66XX_TIMER_TCR_TSTAT_LO_IS_ON(timer)					(C66XX_get_timer_tcr_tstat_lo(timer) == C66XX_ON)


//------------ Timer global control register macros ---------------------------
#define C66XX_get_timer_tgcr_rg(timer)						C66XX_GET_RG_VALUE(C66XX_TIMER_TGCR_RG_ADDR(timer))
#define C66XX_set_timer_tgcr_rg(timer,v)					C66XX_SET_RG_VALUE(C66XX_TIMER_TGCR_RG_ADDR(timer), v)

	// dedicated bit specific macros
#define C66XX_get_timer_tgcr_tddrhi(timer)					C66XX_GET_FIELD_VALUE(C66XX_TIMER_TGCR_RG_ADDR(timer), TIMER_TGCR_TDDRHI)
#define C66XX_set_timer_tgcr_tddrhi(timer,v)				C66XX_SET_FIELD_VALUE(C66XX_TIMER_TGCR_RG_ADDR(timer), TIMER_TGCR_TDDRHI, v)
#define C66XX_get_timer_tgcr_pschi(timer)					C66XX_GET_FIELD_VALUE(C66XX_TIMER_TGCR_RG_ADDR(timer), TIMER_TGCR_PSCHI)
#define C66XX_set_timer_tgcr_pschi(timer,v)					C66XX_SET_FIELD_VALUE(C66XX_TIMER_TGCR_RG_ADDR(timer), TIMER_TGCR_PSCHI, v)
#define C66XX_get_timer_tgcr_plusen(timer)					C66XX_GET_FIELD_VALUE(C66XX_TIMER_TGCR_RG_ADDR(timer), TIMER_TGCR_PLUSEN)
#define C66XX_set_timer_tgcr_plusen(timer,v)				C66XX_SET_FIELD_VALUE(C66XX_TIMER_TGCR_RG_ADDR(timer), TIMER_TGCR_PLUSEN, v)
#define C66XX_get_timer_tgcr_timmode(timer)					C66XX_GET_FIELD_VALUE(C66XX_TIMER_TGCR_RG_ADDR(timer), TIMER_TGCR_TIMMODE)
#define C66XX_set_timer_tgcr_timmode(timer,v)				C66XX_SET_FIELD_VALUE(C66XX_TIMER_TGCR_RG_ADDR(timer), TIMER_TGCR_TIMMODE, v)
#define C66XX_get_timer_tgcr_timhirs(timer)					C66XX_GET_FIELD_VALUE(C66XX_TIMER_TGCR_RG_ADDR(timer), TIMER_TGCR_TIMHIRS)
#define C66XX_set_timer_tgcr_timhirs(timer,v)				C66XX_SET_FIELD_VALUE(C66XX_TIMER_TGCR_RG_ADDR(timer), TIMER_TGCR_TIMHIRS, v)
#define C66XX_get_timer_tgcr_timlors(timer)					C66XX_GET_FIELD_VALUE(C66XX_TIMER_TGCR_RG_ADDR(timer), TIMER_TGCR_TIMLORS)
#define C66XX_set_timer_tgcr_timlors(timer,v)				C66XX_SET_FIELD_VALUE(C66XX_TIMER_TGCR_RG_ADDR(timer), TIMER_TGCR_TIMLORS, v)

	// direct bit set macros
#define C66XX_SET_TIMER_TGCR_PLUSEN_ON(timer)				C66XX_set_timer_tgcr_plusen(timer, C66XX_ON)
#define C66XX_SET_TIMER_TGCR_PLUSEN_OFF(timer)				C66XX_set_timer_tgcr_plusen(timer, C66XX_OFF)
#define C66XX_SET_TIMER_TGCR_TIMMODE_64BIT_GPT(timer)		C66XX_set_timer_tgcr_timmode(timer, C66XX_TIMER_TGCR_TIMMODE_64BIT_GPT)
#define C66XX_SET_TIMER_TGCR_TIMMODE_32BIT_UNCHAINED(timer)	C66XX_set_timer_tgcr_timmode(timer, C66XX_TIMER_TGCR_TIMMODE_32BIT_UNCHAINED)
#define C66XX_SET_TIMER_TGCR_TIMMODE_64BIT_WDT(timer)		C66XX_set_timer_tgcr_timmode(timer, C66XX_TIMER_TGCR_TIMMODE_64BIT_WDT)
#define C66XX_SET_TIMER_TGCR_TIMMODE_32BIT_CHAINED(timer)	C66XX_set_timer_tgcr_timmode(timer, C66XX_TIMER_TGCR_TIMMODE_32BIT_CHAINED)
#define C66XX_SET_TIMER_TGCR_TIMHIRS_ON(timer)				C66XX_set_timer_tgcr_timhirs(timer, C66XX_ON)
#define C66XX_SET_TIMER_TGCR_TIMHIRS_OFF(timer)				C66XX_set_timer_tgcr_timhirs(timer, C66XX_OFF)
#define C66XX_SET_TIMER_TGCR_TIMLORS_ON(timer)				C66XX_set_timer_tgcr_timlors(timer, C66XX_ON)
#define C66XX_SET_TIMER_TGCR_TIMLORS_OFF(timer)				C66XX_set_timer_tgcr_timlors(timer, C66XX_OFF)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_TIMER_TGCR_PLUSEN_IS_ON(timer)				(C66XX_get_timer_tgcr_plusen(timer) == C66XX_ON)
#define C66XX_TIMER_TGCR_TIMMODE_IS_64BIT_GPT(timer)		(C66XX_get_timer_tgcr_timmode(timer) == C66XX_TIMER_TGCR_TIMMODE_64BIT_GPT)
#define C66XX_TIMER_TGCR_TIMMODE_IS_32BIT_UNCHAINED(timer)	(C66XX_get_timer_tgcr_timmode(timer) == C66XX_TIMER_TGCR_TIMMODE_32BIT_UNCHAINED)
#define C66XX_TIMER_TGCR_TIMMODE_IS_64BIT_WDT(timer)		(C66XX_get_timer_tgcr_timmode(timer) == C66XX_TIMER_TGCR_TIMMODE_64BIT_WDT)
#define C66XX_TIMER_TGCR_TIMMODE_IS_32BIT_CHAINED(timer)	(C66XX_get_timer_tgcr_timmode(timer) == C66XX_TIMER_TGCR_TIMMODE_32BIT_CHAINED)
#define C66XX_TIMER_TGCR_TIMHIRS_IS_ON(timer)				(C66XX_get_timer_tgcr_timhirs(timer) == C66XX_ON)
#define C66XX_TIMER_TGCR_TIMLORS_IS_ON(timer)				(C66XX_get_timer_tgcr_timlors(timer) == C66XX_ON)


//------------ Watchdog Timer Control Register macros -------------------------
#define C66XX_get_timer_wdtcr_rg(timer)						C66XX_GET_RG_VALUE(C66XX_TIMER_WDTCR_RG_ADDR(timer))
#define C66XX_set_timer_wdtcr_rg(timer,v)					C66XX_SET_RG_VALUE(C66XX_TIMER_WDTCR_RG_ADDR(timer), v)

	// dedicated bit specific macros
#define C66XX_get_timer_wdtcr_wdkey(timer)					C66XX_GET_FIELD_VALUE(C66XX_TIMER_WDTCR_RG_ADDR(timer), TIMER_WDTCR_WDKEY)
#define C66XX_set_timer_wdtcr_wdkey(timer,v)				C66XX_SET_FIELD_VALUE(C66XX_TIMER_WDTCR_RG_ADDR(timer), TIMER_WDTCR_WDKEY, v)
#define C66XX_get_timer_wdtcr_wdflag(timer)					C66XX_GET_FIELD_VALUE(C66XX_TIMER_WDTCR_RG_ADDR(timer), TIMER_WDTCR_WDFLAG)
#define C66XX_set_timer_wdtcr_wdflag(timer,v)				C66XX_SET_FIELD_VALUE(C66XX_TIMER_WDTCR_RG_ADDR(timer), TIMER_WDTCR_WDFLAG, v)
#define C66XX_get_timer_wdtcr_wden(timer)					C66XX_GET_FIELD_VALUE(C66XX_TIMER_WDTCR_RG_ADDR(timer), TIMER_WDTCR_WDEN)
#define C66XX_set_timer_wdtcr_wden(timer,v)					C66XX_SET_FIELD_VALUE(C66XX_TIMER_WDTCR_RG_ADDR(timer), TIMER_WDTCR_WDEN, v)

	// direct bit set macros
#define C66XX_SET_TIMER_WDTCR_WDKEY_FIRST_KEY(timer)		C66XX_set_timer_wdtcr_wdkey(timer, C66XX_TIMER_WDTCR_WDKEY_FIRST_KEY)
#define C66XX_SET_TIMER_WDTCR_WDKEY_SECOND_KEY(timer)		C66XX_set_timer_wdtcr_wdkey(timer, C66XX_TIMER_WDTCR_WDKEY_SECOND_KEY)
#define C66XX_SET_TIMER_WDTCR_WDFLAG_ON(timer)				C66XX_set_timer_wdtcr_wdflag(timer, C66XX_ON)
#define C66XX_SET_TIMER_WDTCR_WDFLAG_OFF(timer)				C66XX_set_timer_wdtcr_wdflag(timer, C66XX_OFF)
#define C66XX_SET_TIMER_WDTCR_WDEN_ON(timer)				C66XX_set_timer_wdtcr_wden(timer, C66XX_ON)
#define C66XX_SET_TIMER_WDTCR_WDEN_OFF(timer)				C66XX_set_timer_wdtcr_wden(timer, C66XX_OFF)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_TIMER_WDTCR_WDFLAG_IS_ON(timer)				(C66XX_get_timer_wdtcr_wdflag(timer) == C66XX_ON)
#define C66XX_TIMER_WDTCR_WDEN_IS_ON(timer)					(C66XX_get_timer_wdtcr_wden(timer) == C66XX_ON)


//------------ Timer Reload register low register macros ----------------------
#define C66XX_get_timer_rello_rg(timer)						C66XX_GET_RG_VALUE(C66XX_TIMER_RELLO_RG_ADDR(timer))
#define C66XX_set_timer_rello_rg(timer,v)					C66XX_SET_RG_VALUE(C66XX_TIMER_RELLO_RG_ADDR(timer), v)


//------------ Timer Reload register high register macros ---------------------
#define C66XX_get_timer_relhi_rg(timer)						C66XX_GET_RG_VALUE(C66XX_TIMER_RELHI_RG_ADDR(timer))
#define C66XX_set_timer_relhi_rg(timer,v)					C66XX_SET_RG_VALUE(C66XX_TIMER_RELHI_RG_ADDR(timer), v)


//------------ Timer Capture register low register macros ---------------------
#define C66XX_get_timer_caplo_rg(timer)						C66XX_GET_RG_VALUE(C66XX_TIMER_CAPLO_RG_ADDR(timer))
#define C66XX_set_timer_caplo_rg(timer,v)					C66XX_SET_RG_VALUE(C66XX_TIMER_CAPLO_RG_ADDR(timer), v)


//------------ Timer Capture register high register macros --------------------
#define C66XX_get_timer_caphi_rg(timer)						C66XX_GET_RG_VALUE(C66XX_TIMER_CAPHI_RG_ADDR(timer))
#define C66XX_set_timer_caphi_rg(timer,v)					C66XX_SET_RG_VALUE(C66XX_TIMER_CAPHI_RG_ADDR(timer), v)


//------------ Timer interrupt control and status register macros -------------
#define C66XX_get_timer_intctlstat_rg(timer)				C66XX_GET_RG_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer))
#define C66XX_set_timer_intctlstat_rg(timer,v)				C66XX_SET_RG_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer), v)

	// dedicated bit specific macros
#define C66XX_get_timer_intctlstat_evtintstat_hi(timer)		C66XX_GET_FIELD_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer), TIMER_INTCTLSTAT_EVTINTSTAT_HI)
#define C66XX_set_timer_intctlstat_evtintstat_hi(timer,v)	C66XX_SET_FIELD_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer), TIMER_INTCTLSTAT_EVTINTSTAT_HI, v)
#define C66XX_get_timer_intctlstat_evtinten_hi(timer)		C66XX_GET_FIELD_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer), TIMER_INTCTLSTAT_EVTINTEN_HI)
#define C66XX_set_timer_intctlstat_evtinten_hi(timer,v)		C66XX_SET_FIELD_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer), TIMER_INTCTLSTAT_EVTINTEN_HI, v)
#define C66XX_get_timer_intctlstat_prdintstat_hi(timer)		C66XX_GET_FIELD_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer), TIMER_INTCTLSTAT_PRDINTSTAT_HI)
#define C66XX_set_timer_intctlstat_prdintstat_hi(timer,v)	C66XX_SET_FIELD_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer), TIMER_INTCTLSTAT_PRDINTSTAT_HI, v)
#define C66XX_get_timer_intctlstat_prdinten_hi(timer)		C66XX_GET_FIELD_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer), TIMER_INTCTLSTAT_PRDINTEN_HI)
#define C66XX_set_timer_intctlstat_prdinten_hi(timer,v)		C66XX_SET_FIELD_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer), TIMER_INTCTLSTAT_PRDINTEN_HI, v)
#define C66XX_get_timer_intctlstat_evtintstat_lo(timer)		C66XX_GET_FIELD_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer), TIMER_INTCTLSTAT_EVTINTSTAT_LO)
#define C66XX_set_timer_intctlstat_evtintstat_lo(timer,v)	C66XX_SET_FIELD_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer), TIMER_INTCTLSTAT_EVTINTSTAT_LO, v)
#define C66XX_get_timer_intctlstat_evtinten_lo(timer)		C66XX_GET_FIELD_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer), TIMER_INTCTLSTAT_EVTINTEN_LO)
#define C66XX_set_timer_intctlstat_evtinten_lo(timer,v)		C66XX_SET_FIELD_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer), TIMER_INTCTLSTAT_EVTINTEN_LO, v)
#define C66XX_get_timer_intctlstat_prdintstat_lo(timer)		C66XX_GET_FIELD_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer), TIMER_INTCTLSTAT_PRDINTSTAT_LO)
#define C66XX_set_timer_intctlstat_prdintstat_lo(timer,v)	C66XX_SET_FIELD_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer), TIMER_INTCTLSTAT_PRDINTSTAT_LO, v)
#define C66XX_get_timer_intctlstat_prdinten_lo(timer)		C66XX_GET_FIELD_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer), TIMER_INTCTLSTAT_PRDINTEN_LO)
#define C66XX_set_timer_intctlstat_prdinten_lo(timer,v)		C66XX_SET_FIELD_VALUE(C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer), TIMER_INTCTLSTAT_PRDINTEN_LO, v)

	// direct bit set macros
#define C66XX_CLEAR_TIMER_INTCTLSTAT_EVTINTSTAT_HI(timer)	C66XX_set_timer_intctlstat_evtintstat_hi(timer, C66XX_ON)
#define C66XX_SET_TIMER_INTCTLSTAT_EVTINTEN_HI_ON(timer)	C66XX_set_timer_intctlstat_evtinten_hi(timer, C66XX_ON)
#define C66XX_SET_TIMER_INTCTLSTAT_EVTINTEN_HI_OFF(timer)	C66XX_set_timer_intctlstat_evtinten_hi(timer, C66XX_OFF)
#define C66XX_CLEAR_TIMER_INTCTLSTAT_PRDINTSTAT_HI(timer)	C66XX_set_timer_intctlstat_prdintstat_hi(timer, C66XX_ON)
#define C66XX_SET_TIMER_INTCTLSTAT_PRDINTEN_HI_ON(timer)	C66XX_set_timer_intctlstat_prdinten_hi(timer, C66XX_ON)
#define C66XX_SET_TIMER_INTCTLSTAT_PRDINTEN_HI_OFF(timer)	C66XX_set_timer_intctlstat_prdinten_hi(timer, C66XX_OFF)
#define C66XX_CLEAR_TIMER_INTCTLSTAT_EVTINTSTAT_LO(timer)	C66XX_set_timer_intctlstat_evtintstat_lo(timer, C66XX_ON)
#define C66XX_SET_TIMER_INTCTLSTAT_EVTINTEN_LO_ON(timer)	C66XX_set_timer_intctlstat_evtinten_lo(timer, C66XX_ON)
#define C66XX_SET_TIMER_INTCTLSTAT_EVTINTEN_LO_OFF(timer)	C66XX_set_timer_intctlstat_evtinten_lo(timer, C66XX_OFF)
#define C66XX_CLEAR_TIMER_INTCTLSTAT_PRDINTSTAT_LO(timer)	C66XX_set_timer_intctlstat_prdintstat_lo(timer, C66XX_ON)
#define C66XX_SET_TIMER_INTCTLSTAT_PRDINTEN_LO_ON(timer)	C66XX_set_timer_intctlstat_prdinten_lo(timer, C66XX_ON)
#define C66XX_SET_TIMER_INTCTLSTAT_PRDINTEN_LO_OFF(timer)	C66XX_set_timer_intctlstat_prdinten_lo(timer, C66XX_OFF)

	// condition check macros (for use in IF() and other conditional operators)
#define C66XX_TIMER_INTCTLSTAT_EVTINTSTAT_HI_IS_ON(timer)	(C66XX_get_timer_intctlstat_evtintstat_hi(timer) == C66XX_ON)
#define C66XX_TIMER_INTCTLSTAT_EVTINTEN_HI_IS_ON(timer)		(C66XX_get_timer_intctlstat_evtinten_hi(timer) == C66XX_ON)
#define C66XX_TIMER_INTCTLSTAT_PRDINTSTAT_HI_IS_ON(timer)	(C66XX_get_timer_intctlstat_prdintstat_hi(timer) == C66XX_ON)
#define C66XX_TIMER_INTCTLSTAT_PRDINTEN_HI_IS_ON(timer)		(C66XX_get_timer_intctlstat_prdinten_hi(timer) == C66XX_ON)
#define C66XX_TIMER_INTCTLSTAT_EVTINTSTAT_LO_IS_ON(timer)	(C66XX_get_timer_intctlstat_evtintstat_lo(timer) == C66XX_ON)
#define C66XX_TIMER_INTCTLSTAT_EVTINTEN_LO_IS_ON(timer)		(C66XX_get_timer_intctlstat_evtinten_lo(timer) == C66XX_ON)
#define C66XX_TIMER_INTCTLSTAT_PRDINTSTAT_LO_IS_ON(timer)	(C66XX_get_timer_intctlstat_prdintstat_lo(timer) == C66XX_ON)
#define C66XX_TIMER_INTCTLSTAT_PRDINTEN_LO_IS_ON(timer)		(C66XX_get_timer_intctlstat_prdinten_lo(timer) == C66XX_ON)

//=============================================================================



//=============================================================================
#endif /* __C66XX_MACROS_HXX__ */
