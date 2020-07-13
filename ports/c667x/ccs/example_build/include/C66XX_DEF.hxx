/******************************************************************************
	TMS320C66xx KeyStone Multicore DSP Software Development Kit (SDK). Rev 2A.
	(C) MicroLAB Systems, 2014-2015

	File:	Definitions
	-----

	Notes:
	------
	1.	This C-header file contains general DSP definitions and
		is used with C66XX.h C-header file.

	2.	This file is best viewed with the TAB setting set to '4'.

******************************************************************************/


/**
 * @file  C66XX_DEF.hxx
 *
 * @brief  Definitions
 *
 * This file contains general C66xx DSP definitions
 *
 */


#ifndef __C66XX_DEF_HXX__									// check for this file has been already included
#define __C66XX_DEF_HXX__									1


//=============================================================================
//============ DSP CPU ID =====================================================
//=============================================================================

// DSP CPU ID is located at CSR register and is used to identify the chip
#define C66XX_DSP_CPU_ID									0x15
//=============================================================================



#ifdef __C66XX_SELECT_C6678_DSP__
//=============================================================================
//============ DSP cores number ===============================================
//=============================================================================
#define C66XX_DSP_NUMBER_OF_CORES							8
//=============================================================================


//=============================================================================
//============ DSP memory map =================================================
//=============================================================================
// All address ranges refer to 32-bit LOGICAL addresses!!!

// Local L2 SRAM - 512KB
#define C66XX_LOCAL_L2_SRAM_SADDR							0x00800000
#define C66XX_LOCAL_L2_SRAM_EADDR							0x0087ffff
#define C66XX_LOCAL_L2_SRAM_LEN								(C66XX_LOCAL_L2_SRAM_EADDR - C66XX_LOCAL_L2_SRAM_SADDR + 1)

// Local L1P SRAM - 32KB
#define C66XX_LOCAL_L1P_SRAM_SADDR							0x00e00000
#define C66XX_LOCAL_L1P_SRAM_EADDR							0x00e07fff
#define C66XX_LOCAL_L1P_SRAM_LEN							(C66XX_LOCAL_L1P_SRAM_EADDR - C66XX_LOCAL_L1P_SRAM_SADDR + 1)

// Local L1D SRAM - 32KB
#define C66XX_LOCAL_L1D_SRAM_SADDR							0x00f00000
#define C66XX_LOCAL_L1D_SRAM_EADDR							0x00f07fff
#define C66XX_LOCAL_L1D_SRAM_LEN							(C66XX_LOCAL_L1D_SRAM_EADDR - C66XX_LOCAL_L1D_SRAM_SADDR + 1)

// C66xx CorePack registers area - 4MB
#define C66XX_DSP_RG_AREA_SADDR								0x01800000
#define C66XX_DSP_RG_AREA_EADDR								0x01bfffff
#define C66XX_DSP_RG_AREA_LEN								(C66XX_DSP_RG_AREA_EADDR - C66XX_DSP_RG_AREA_SADDR + 1)

// Tracer MSMC 0 registers area - 128B
#define C66XX_TRACER_MSMC_0_RG_AREA_SADDR					0x01d00000
#define C66XX_TRACER_MSMC_0_RG_AREA_EADDR					0x01d0007f
#define C66XX_TRACER_MSMC_0_RG_AREA_LEN						(C66XX_TRACER_MSMC_0_RG_AREA_EADDR - C66XX_TRACER_MSMC_0_RG_AREA_SADDR + 1)

// Tracer MSMC 1 registers area - 128B
#define C66XX_TRACER_MSMC_1_RG_AREA_SADDR					0x01d08000
#define C66XX_TRACER_MSMC_1_RG_AREA_EADDR					0x01d0807f
#define C66XX_TRACER_MSMC_1_RG_AREA_LEN						(C66XX_TRACER_MSMC_1_RG_AREA_EADDR - C66XX_TRACER_MSMC_1_RG_AREA_SADDR + 1)

// Tracer MSMC 2 registers area - 128B
#define C66XX_TRACER_MSMC_2_RG_AREA_SADDR					0x01d10000
#define C66XX_TRACER_MSMC_2_RG_AREA_EADDR					0x01d1007f
#define C66XX_TRACER_MSMC_2_RG_AREA_LEN						(C66XX_TRACER_MSMC_2_RG_AREA_EADDR - C66XX_TRACER_MSMC_2_RG_AREA_SADDR + 1)

// Tracer MSMC 3 registers area - 128B
#define C66XX_TRACER_MSMC_3_RG_AREA_SADDR					0x01d18000
#define C66XX_TRACER_MSMC_3_RG_AREA_EADDR					0x01d1807f
#define C66XX_TRACER_MSMC_3_RG_AREA_LEN						(C66XX_TRACER_MSMC_3_RG_AREA_EADDR - C66XX_TRACER_MSMC_3_RG_AREA_SADDR + 1)

// Tracer QM DMA registers area - 128B
#define C66XX_TRACER_QM_DMA_RG_AREA_SADDR					0x01d20000
#define C66XX_TRACER_QM_DMA_RG_AREA_EADDR					0x01d2007f
#define C66XX_TRACER_QM_DMA_RG_AREA_LEN						(C66XX_TRACER_QM_DMA_RG_AREA_EADDR - C66XX_TRACER_QM_DMA_RG_AREA_SADDR + 1)

// Tracer DDR registers area - 128B
#define C66XX_TRACER_DDR_RG_AREA_SADDR						0x01d28000
#define C66XX_TRACER_DDR_RG_AREA_EADDR						0x01d2807f
#define C66XX_TRACER_DDR_RG_AREA_LEN						(C66XX_TRACER_DDR_RG_AREA_EADDR - C66XX_TRACER_DDR_RG_AREA_SADDR + 1)

// Tracer SM registers area - 128B
#define C66XX_TRACER_SM_RG_AREA_SADDR						0x01d30000
#define C66XX_TRACER_SM_RG_AREA_EADDR						0x01d3007f
#define C66XX_TRACER_SM_RG_AREA_LEN							(C66XX_TRACER_SM_RG_AREA_EADDR - C66XX_TRACER_SM_RG_AREA_SADDR + 1)

// Tracer QM CFG registers area - 128B
#define C66XX_TRACER_QM_CFG_RG_AREA_SADDR					0x01d38000
#define C66XX_TRACER_QM_CFG_RG_AREA_EADDR					0x01d3807f
#define C66XX_TRACER_QM_CFG_RG_AREA_LEN						(C66XX_TRACER_QM_CFG_RG_AREA_EADDR - C66XX_TRACER_QM_CFG_RG_AREA_SADDR + 1)

// Tracer CFG registers area - 128B
#define C66XX_TRACER_CFG_RG_AREA_SADDR						0x01d40000
#define C66XX_TRACER_CFG_RG_AREA_EADDR						0x01d4007f
#define C66XX_TRACER_CFG_RG_AREA_LEN						(C66XX_TRACER_CFG_RG_AREA_EADDR - C66XX_TRACER_CFG_RG_AREA_SADDR + 1)

// Tracer L2 0 registers area - 128B
#define C66XX_TRACER_L2_0_RG_AREA_SADDR						0x01d48000
#define C66XX_TRACER_L2_0_RG_AREA_EADDR						0x01d4807f
#define C66XX_TRACER_L2_0_RG_AREA_LEN						(C66XX_TRACER_L2_0_RG_AREA_EADDR - C66XX_TRACER_L2_0_RG_AREA_SADDR + 1)

// Tracer L2 1 registers area - 128B
#define C66XX_TRACER_L2_1_RG_AREA_SADDR						0x01d50000
#define C66XX_TRACER_L2_1_RG_AREA_EADDR						0x01d5007f
#define C66XX_TRACER_L2_1_RG_AREA_LEN						(C66XX_TRACER_L2_1_RG_AREA_EADDR - C66XX_TRACER_L2_1_RG_AREA_SADDR + 1)

// Tracer L2 2 registers area - 128B
#define C66XX_TRACER_L2_2_RG_AREA_SADDR						0x01d58000
#define C66XX_TRACER_L2_2_RG_AREA_EADDR						0x01d5807f
#define C66XX_TRACER_L2_2_RG_AREA_LEN						(C66XX_TRACER_L2_2_RG_AREA_EADDR - C66XX_TRACER_L2_2_RG_AREA_SADDR + 1)

// Tracer L2 3 registers area - 128B
#define C66XX_TRACER_L2_3_RG_AREA_SADDR						0x01d60000
#define C66XX_TRACER_L2_3_RG_AREA_EADDR						0x01d6007f
#define C66XX_TRACER_L2_3_RG_AREA_LEN						(C66XX_TRACER_L2_3_RG_AREA_EADDR - C66XX_TRACER_L2_3_RG_AREA_SADDR + 1)

// Tracer L2 4 registers area - 128B
#define C66XX_TRACER_L2_4_RG_AREA_SADDR						0x01d68000
#define C66XX_TRACER_L2_4_RG_AREA_EADDR						0x01d6807f
#define C66XX_TRACER_L2_4_RG_AREA_LEN						(C66XX_TRACER_L2_4_RG_AREA_EADDR - C66XX_TRACER_L2_4_RG_AREA_SADDR + 1)

// Tracer L2 5 registers area - 128B
#define C66XX_TRACER_L2_5_RG_AREA_SADDR						0x01d70000
#define C66XX_TRACER_L2_5_RG_AREA_EADDR						0x01d7007f
#define C66XX_TRACER_L2_5_RG_AREA_LEN						(C66XX_TRACER_L2_5_RG_AREA_EADDR - C66XX_TRACER_L2_5_RG_AREA_SADDR + 1)

// Tracer L2 6 registers area - 128B
#define C66XX_TRACER_L2_6_RG_AREA_SADDR						0x01d78000
#define C66XX_TRACER_L2_6_RG_AREA_EADDR						0x01d7807f
#define C66XX_TRACER_L2_6_RG_AREA_LEN						(C66XX_TRACER_L2_6_RG_AREA_EADDR - C66XX_TRACER_L2_6_RG_AREA_SADDR + 1)

// Tracer L2 7 registers area - 128B
#define C66XX_TRACER_L2_7_RG_AREA_SADDR						0x01d80000
#define C66XX_TRACER_L2_7_RG_AREA_EADDR						0x01d8007f
#define C66XX_TRACER_L2_7_RG_AREA_LEN						(C66XX_TRACER_L2_7_RG_AREA_EADDR - C66XX_TRACER_L2_7_RG_AREA_SADDR + 1)

// Telecom Serial Interface Port (TSIP) 0 registers area - 256KB
#define C66XX_TSIP_0_RG_AREA_SADDR							0x01e00000
#define C66XX_TSIP_0_RG_AREA_EADDR							0x01e3ffff
#define C66XX_TSIP_0_RG_AREA_LEN							(C66XX_TSIP_0_RG_AREA_EADDR - C66XX_TSIP_0_RG_AREA_SADDR + 1)

// Telecom Serial Interface Port (TSIP) 1 registers area - 256KB
#define C66XX_TSIP_1_RG_AREA_SADDR							0x01e80000
#define C66XX_TSIP_1_RG_AREA_EADDR							0x01ebffff
#define C66XX_TSIP_1_RG_AREA_LEN							(C66XX_TSIP_1_RG_AREA_EADDR - C66XX_TSIP_1_RG_AREA_SADDR + 1)

// Network Coprocessor (NETCP) registers area - 1MB
#define C66XX_NETCP_RG_AREA_SADDR							0x02000000
#define C66XX_NETCP_RG_AREA_EADDR							0x020fffff
#define C66XX_NETCP_RG_AREA_LEN								(C66XX_NETCP_RG_AREA_EADDR - C66XX_NETCP_RG_AREA_SADDR + 1)

// Timer0 registers area - 128B
#define C66XX_TIMER_0_RG_AREA_SADDR							0x02200000
#define C66XX_TIMER_0_RG_AREA_EADDR							0x0220007f
#define C66XX_TIMER_0_RG_AREA_LEN							(C66XX_TIMER_0_RG_AREA_EADDR - C66XX_TIMER_0_RG_AREA_SADDR + 1)

// Timer1 registers area - 128B
#define C66XX_TIMER_1_RG_AREA_SADDR							0x02210000
#define C66XX_TIMER_1_RG_AREA_EADDR							0x0221007f
#define C66XX_TIMER_1_RG_AREA_LEN							(C66XX_TIMER_1_RG_AREA_EADDR - C66XX_TIMER_1_RG_AREA_SADDR + 1)

// Timer2 registers area - 128B
#define C66XX_TIMER_2_RG_AREA_SADDR							0x02220000
#define C66XX_TIMER_2_RG_AREA_EADDR							0x0222007f
#define C66XX_TIMER_2_RG_AREA_LEN							(C66XX_TIMER_2_RG_AREA_EADDR - C66XX_TIMER_2_RG_AREA_SADDR + 1)

// Timer3 registers area - 128B
#define C66XX_TIMER_3_RG_AREA_SADDR							0x02230000
#define C66XX_TIMER_3_RG_AREA_EADDR							0x0223007f
#define C66XX_TIMER_3_RG_AREA_LEN							(C66XX_TIMER_3_RG_AREA_EADDR - C66XX_TIMER_3_RG_AREA_SADDR + 1)

// Timer4 registers area - 128B
#define C66XX_TIMER_4_RG_AREA_SADDR							0x02240000
#define C66XX_TIMER_4_RG_AREA_EADDR							0x0224007f
#define C66XX_TIMER_4_RG_AREA_LEN							(C66XX_TIMER_4_RG_AREA_EADDR - C66XX_TIMER_4_RG_AREA_SADDR + 1)

// Timer5 registers area - 128B
#define C66XX_TIMER_5_RG_AREA_SADDR							0x02250000
#define C66XX_TIMER_5_RG_AREA_EADDR							0x0225007f
#define C66XX_TIMER_5_RG_AREA_LEN							(C66XX_TIMER_5_RG_AREA_EADDR - C66XX_TIMER_5_RG_AREA_SADDR + 1)

// Timer6 registers area - 128B
#define C66XX_TIMER_6_RG_AREA_SADDR							0x02260000
#define C66XX_TIMER_6_RG_AREA_EADDR							0x0226007f
#define C66XX_TIMER_6_RG_AREA_LEN							(C66XX_TIMER_6_RG_AREA_EADDR - C66XX_TIMER_6_RG_AREA_SADDR + 1)

// Timer7 registers area - 128B
#define C66XX_TIMER_7_RG_AREA_SADDR							0x02270000
#define C66XX_TIMER_7_RG_AREA_EADDR							0x0227007f
#define C66XX_TIMER_7_RG_AREA_LEN							(C66XX_TIMER_7_RG_AREA_EADDR - C66XX_TIMER_7_RG_AREA_SADDR + 1)

// Timer8 registers area - 128B
#define C66XX_TIMER_8_RG_AREA_SADDR							0x02280000
#define C66XX_TIMER_8_RG_AREA_EADDR							0x0228007f
#define C66XX_TIMER_8_RG_AREA_LEN							(C66XX_TIMER_8_RG_AREA_EADDR - C66XX_TIMER_8_RG_AREA_SADDR + 1)

// Timer9 registers area - 128B
#define C66XX_TIMER_9_RG_AREA_SADDR							0x02290000
#define C66XX_TIMER_9_RG_AREA_EADDR							0x0229007f
#define C66XX_TIMER_9_RG_AREA_LEN							(C66XX_TIMER_9_RG_AREA_EADDR - C66XX_TIMER_9_RG_AREA_SADDR + 1)

// Timer10 registers area - 128B
#define C66XX_TIMER_10_RG_AREA_SADDR						0x022a0000
#define C66XX_TIMER_10_RG_AREA_EADDR						0x022a007f
#define C66XX_TIMER_10_RG_AREA_LEN							(C66XX_TIMER_10_RG_AREA_EADDR - C66XX_TIMER_10_RG_AREA_SADDR + 1)

// Timer11 registers area - 128B
#define C66XX_TIMER_11_RG_AREA_SADDR						0x022b0000
#define C66XX_TIMER_11_RG_AREA_EADDR						0x022b007f
#define C66XX_TIMER_11_RG_AREA_LEN							(C66XX_TIMER_11_RG_AREA_EADDR - C66XX_TIMER_11_RG_AREA_SADDR + 1)

// Timer12 registers area - 128B
#define C66XX_TIMER_12_RG_AREA_SADDR						0x022c0000
#define C66XX_TIMER_12_RG_AREA_EADDR						0x022c007f
#define C66XX_TIMER_12_RG_AREA_LEN							(C66XX_TIMER_12_RG_AREA_EADDR - C66XX_TIMER_12_RG_AREA_SADDR + 1)

// Timer13 registers area - 128B
#define C66XX_TIMER_13_RG_AREA_SADDR						0x022d0000
#define C66XX_TIMER_13_RG_AREA_EADDR						0x022d007f
#define C66XX_TIMER_13_RG_AREA_LEN							(C66XX_TIMER_13_RG_AREA_EADDR - C66XX_TIMER_13_RG_AREA_SADDR + 1)

// Timer14 registers area - 128B
#define C66XX_TIMER_14_RG_AREA_SADDR						0x022e0000
#define C66XX_TIMER_14_RG_AREA_EADDR						0x022e007f
#define C66XX_TIMER_14_RG_AREA_LEN							(C66XX_TIMER_14_RG_AREA_EADDR - C66XX_TIMER_14_RG_AREA_SADDR + 1)

// Timer15 registers area - 128B
#define C66XX_TIMER_15_RG_AREA_SADDR						0x022f0000
#define C66XX_TIMER_15_RG_AREA_EADDR						0x022f007f
#define C66XX_TIMER_15_RG_AREA_LEN							(C66XX_TIMER_15_RG_AREA_EADDR - C66XX_TIMER_15_RG_AREA_SADDR + 1)

// PLL controller registers area - 512B
#define C66XX_PLL_RG_AREA_SADDR								0x02310000
#define C66XX_PLL_RG_AREA_EADDR								0x023101ff
#define C66XX_PLL_RG_AREA_LEN								(C66XX_PLL_RG_AREA_EADDR - C66XX_PLL_RG_AREA_SADDR + 1)

// GPIO registers area - 256B
#define C66XX_GPIO_RG_AREA_SADDR							0x02320000
#define C66XX_GPIO_RG_AREA_EADDR							0x023200ff
#define C66XX_GPIO_RG_AREA_LEN								(C66XX_GPIO_RG_AREA_EADDR - C66XX_GPIO_RG_AREA_SADDR + 1)

// SmartReflex registers area - 256B
#define C66XX_SMARTREFLEX_RG_AREA_SADDR						0x02330000
#define C66XX_SMARTREFLEX_RG_AREA_EADDR						0x023303ff
#define C66XX_SMARTREFLEX_RG_AREA_LEN						(C66XX_SMARTREFLEX_RG_AREA_EADDR - C66XX_SMARTREFLEX_RG_AREA_SADDR + 1)

// Power Sleep Controller (PSC) registers area - 4KB
#define C66XX_PSC_RG_AREA_SADDR								0x02350000
#define C66XX_PSC_RG_AREA_EADDR								0x02350fff
#define C66XX_PSC_RG_AREA_LEN								(C66XX_PSC_RG_AREA_EADDR - C66XX_PSC_RG_AREA_SADDR + 1)

// Memory Protection Unit (MPU) 0 registers area - 1KB
#define C66XX_MPU_0_RG_AREA_SADDR							0x02360000
#define C66XX_MPU_0_RG_AREA_EADDR							0x023603ff
#define C66XX_MPU_0_RG_AREA_LEN								(C66XX_MPU_0_RG_AREA_EADDR - C66XX_MPU_0_RG_AREA_SADDR + 1)

// Memory Protection Unit (MPU) 1 registers area - 1KB
#define C66XX_MPU_1_RG_AREA_SADDR							0x02368000
#define C66XX_MPU_1_RG_AREA_EADDR							0x023683ff
#define C66XX_MPU_1_RG_AREA_LEN								(C66XX_MPU_1_RG_AREA_EADDR - C66XX_MPU_1_RG_AREA_SADDR + 1)

// Memory Protection Unit (MPU) 2 registers area - 1KB
#define C66XX_MPU_2_RG_AREA_SADDR							0x02370000
#define C66XX_MPU_2_RG_AREA_EADDR							0x023703ff
#define C66XX_MPU_2_RG_AREA_LEN								(C66XX_MPU_2_RG_AREA_EADDR - C66XX_MPU_2_RG_AREA_SADDR + 1)

// Memory Protection Unit (MPU) 3 registers area - 1KB
#define C66XX_MPU_3_RG_AREA_SADDR							0x02378000
#define C66XX_MPU_3_RG_AREA_EADDR							0x023783ff
#define C66XX_MPU_3_RG_AREA_LEN								(C66XX_MPU_3_RG_AREA_EADDR - C66XX_MPU_3_RG_AREA_SADDR + 1)

// Debug subsystem configuration registers area - 256KB
#define C66XX_DEBUG_CFG_RG_AREA_SADDR						0x02400000
#define C66XX_DEBUG_CFG_RG_AREA_EADDR						0x0243ffff
#define C66XX_DEBUG_CFG_RG_AREA_LEN							(C66XX_DEBUG_CFG_RG_AREA_EADDR - C66XX_DEBUG_CFG_RG_AREA_SADDR + 1)

// DSP trace formatter 0 registers area - 16KB
#define C66XX_DSP_TRACE_FORMATTER_0_RG_AREA_SADDR			0x02440000
#define C66XX_DSP_TRACE_FORMATTER_0_RG_AREA_EADDR			0x02443fff
#define C66XX_DSP_TRACE_FORMATTER_0_RG_AREA_LEN				(C66XX_DSP_TRACE_FORMATTER_0_RG_AREA_EADDR - C66XX_DSP_TRACE_FORMATTER_0_RG_AREA_SADDR + 1)

// DSP trace formatter 1 registers area - 16KB
#define C66XX_DSP_TRACE_FORMATTER_1_RG_AREA_SADDR			0x02450000
#define C66XX_DSP_TRACE_FORMATTER_1_RG_AREA_EADDR			0x02453fff
#define C66XX_DSP_TRACE_FORMATTER_1_RG_AREA_LEN				(C66XX_DSP_TRACE_FORMATTER_1_RG_AREA_EADDR - C66XX_DSP_TRACE_FORMATTER_1_RG_AREA_SADDR + 1)

// DSP trace formatter 2 registers area - 16KB
#define C66XX_DSP_TRACE_FORMATTER_2_RG_AREA_SADDR			0x02460000
#define C66XX_DSP_TRACE_FORMATTER_2_RG_AREA_EADDR			0x02463fff
#define C66XX_DSP_TRACE_FORMATTER_2_RG_AREA_LEN				(C66XX_DSP_TRACE_FORMATTER_2_RG_AREA_EADDR - C66XX_DSP_TRACE_FORMATTER_2_RG_AREA_SADDR + 1)

// DSP trace formatter 3 registers area - 16KB
#define C66XX_DSP_TRACE_FORMATTER_3_RG_AREA_SADDR			0x02470000
#define C66XX_DSP_TRACE_FORMATTER_3_RG_AREA_EADDR			0x02473fff
#define C66XX_DSP_TRACE_FORMATTER_3_RG_AREA_LEN				(C66XX_DSP_TRACE_FORMATTER_3_RG_AREA_EADDR - C66XX_DSP_TRACE_FORMATTER_3_RG_AREA_SADDR + 1)

// DSP trace formatter 4 registers area - 16KB
#define C66XX_DSP_TRACE_FORMATTER_4_RG_AREA_SADDR			0x02480000
#define C66XX_DSP_TRACE_FORMATTER_4_RG_AREA_EADDR			0x02483fff
#define C66XX_DSP_TRACE_FORMATTER_4_RG_AREA_LEN				(C66XX_DSP_TRACE_FORMATTER_4_RG_AREA_EADDR - C66XX_DSP_TRACE_FORMATTER_4_RG_AREA_SADDR + 1)

// DSP trace formatter 5 registers area - 16KB
#define C66XX_DSP_TRACE_FORMATTER_5_RG_AREA_SADDR			0x02490000
#define C66XX_DSP_TRACE_FORMATTER_5_RG_AREA_EADDR			0x02493fff
#define C66XX_DSP_TRACE_FORMATTER_5_RG_AREA_LEN				(C66XX_DSP_TRACE_FORMATTER_5_RG_AREA_EADDR - C66XX_DSP_TRACE_FORMATTER_5_RG_AREA_SADDR + 1)

// DSP trace formatter 6 registers area - 16KB
#define C66XX_DSP_TRACE_FORMATTER_6_RG_AREA_SADDR			0x024a0000
#define C66XX_DSP_TRACE_FORMATTER_6_RG_AREA_EADDR			0x024a3fff
#define C66XX_DSP_TRACE_FORMATTER_6_RG_AREA_LEN				(C66XX_DSP_TRACE_FORMATTER_6_RG_AREA_EADDR - C66XX_DSP_TRACE_FORMATTER_6_RG_AREA_SADDR + 1)

// DSP trace formatter 7 registers area - 16KB
#define C66XX_DSP_TRACE_FORMATTER_7_RG_AREA_SADDR			0x024b0000
#define C66XX_DSP_TRACE_FORMATTER_7_RG_AREA_EADDR			0x024b3fff
#define C66XX_DSP_TRACE_FORMATTER_7_RG_AREA_LEN				(C66XX_DSP_TRACE_FORMATTER_7_RG_AREA_EADDR - C66XX_DSP_TRACE_FORMATTER_7_RG_AREA_SADDR + 1)

// I2C registers area - 128B
#define C66XX_I2C_RG_AREA_SADDR								0x02530000
#define C66XX_I2C_RG_AREA_EADDR								0x0253007f
#define C66XX_I2C_RG_AREA_LEN								(C66XX_I2C_RG_AREA_EADDR - C66XX_I2C_RG_AREA_SADDR + 1)

// UART registers area - 64B
#define C66XX_UART_RG_AREA_SADDR							0x02540000
#define C66XX_UART_RG_AREA_EADDR							0x0254003f
#define C66XX_UART_RG_AREA_LEN								(C66XX_UART_RG_AREA_EADDR - C66XX_UART_RG_AREA_SADDR + 1)

// Chip Interrupt Controller (CIC) 0 registers area - 8KB
#define C66XX_CIC_0_RG_AREA_SADDR							0x02600000
#define C66XX_CIC_0_RG_AREA_EADDR							0x02601fff
#define C66XX_CIC_0_RG_AREA_LEN								(C66XX_CIC_0_RG_AREA_EADDR - C66XX_CIC_0_RG_AREA_SADDR + 1)

// Chip Interrupt Controller (CIC) 1 registers area - 8KB
#define C66XX_CIC_1_RG_AREA_SADDR							0x02604000
#define C66XX_CIC_1_RG_AREA_EADDR							0x02605fff
#define C66XX_CIC_1_RG_AREA_LEN								(C66XX_CIC_1_RG_AREA_EADDR - C66XX_CIC_1_RG_AREA_SADDR + 1)

// Chip Interrupt Controller (CIC) 2 registers area - 8KB
#define C66XX_CIC_2_RG_AREA_SADDR							0x02608000
#define C66XX_CIC_2_RG_AREA_EADDR							0x02609fff
#define C66XX_CIC_2_RG_AREA_LEN								(C66XX_CIC_2_RG_AREA_EADDR - C66XX_CIC_2_RG_AREA_SADDR + 1)

// Chip Interrupt Controller (CIC) 3 registers area - 8KB
#define C66XX_CIC_3_RG_AREA_SADDR							0x0260c000
#define C66XX_CIC_3_RG_AREA_EADDR							0x0260dfff
#define C66XX_CIC_3_RG_AREA_LEN								(C66XX_CIC_3_RG_AREA_EADDR - C66XX_CIC_3_RG_AREA_SADDR + 1)

// Device State Control registers area - 2KB
#define C66XX_BOOTCFG_RG_AREA_SADDR							0x02620000
#define C66XX_BOOTCFG_RG_AREA_EADDR							0x026207ff
#define C66XX_BOOTCFG_RG_AREA_LEN							(C66XX_BOOTCFG_RG_AREA_EADDR - C66XX_BOOTCFG_RG_AREA_SADDR + 1)

// Semaphore (SEM) registers area - 2KB
#define C66XX_SEM_RG_AREA_SADDR								0x02640000
#define C66XX_SEM_RG_AREA_EADDR								0x026407ff
#define C66XX_SEM_RG_AREA_LEN								(C66XX_SEM_RG_AREA_EADDR - C66XX_SEM_RG_AREA_SADDR + 1)

// EDMA3 Channel Controller (EDMA3CC) 0 registers area - 32KB
#define C66XX_EDMA3CC_0_RG_AREA_SADDR						0x02700000
#define C66XX_EDMA3CC_0_RG_AREA_EADDR						0x02707fff
#define C66XX_EDMA3CC_0_RG_AREA_LEN							(C66XX_EDMA3CC_0_RG_AREA_EADDR - C66XX_EDMA3CC_0_RG_AREA_SADDR + 1)

// EDMA3 Channel Controller (EDMA3CC) 1 registers area - 32KB
#define C66XX_EDMA3CC_1_RG_AREA_SADDR						0x02720000
#define C66XX_EDMA3CC_1_RG_AREA_EADDR						0x02727fff
#define C66XX_EDMA3CC_1_RG_AREA_LEN							(C66XX_EDMA3CC_1_RG_AREA_EADDR - C66XX_EDMA3CC_1_RG_AREA_SADDR + 1)

// EDMA3 Channel Controller (EDMA3CC) 2 registers area - 32KB
#define C66XX_EDMA3CC_2_RG_AREA_SADDR						0x02740000
#define C66XX_EDMA3CC_2_RG_AREA_EADDR						0x02747fff
#define C66XX_EDMA3CC_2_RG_AREA_LEN							(C66XX_EDMA3CC_2_RG_AREA_EADDR - C66XX_EDMA3CC_2_RG_AREA_SADDR + 1)

// EDMA3CC0 Transfer Controller (EDMA3TC) 0 registers area - 1KB
#define C66XX_EDMA3CC_0_TC_0_RG_AREA_SADDR					0x02760000
#define C66XX_EDMA3CC_0_TC_0_RG_AREA_EADDR					0x027603ff
#define C66XX_EDMA3CC_0_TC_0_RG_AREA_LEN					(C66XX_EDMA3CC_0_TC_0_RG_AREA_EADDR - C66XX_EDMA3CC_0_TC_0_RG_AREA_SADDR + 1)

// EDMA3CC0 Transfer Controller (EDMA3TC) 1 registers area - 1KB
#define C66XX_EDMA3CC_0_TC_1_RG_AREA_SADDR					0x02768000
#define C66XX_EDMA3CC_0_TC_1_RG_AREA_EADDR					0x027683ff
#define C66XX_EDMA3CC_0_TC_1_RG_AREA_LEN					(C66XX_EDMA3CC_0_TC_1_RG_AREA_EADDR - C66XX_EDMA3CC_0_TC_1_RG_AREA_SADDR + 1)

// EDMA3CC1 Transfer Controller (EDMA3TC) 0 registers area - 1KB
#define C66XX_EDMA3CC_1_TC_0_RG_AREA_SADDR					0x02770000
#define C66XX_EDMA3CC_1_TC_0_RG_AREA_EADDR					0x027703ff
#define C66XX_EDMA3CC_1_TC_0_RG_AREA_LEN					(C66XX_EDMA3CC_1_TC_0_RG_AREA_EADDR - C66XX_EDMA3CC_1_TC_0_RG_AREA_SADDR + 1)

// EDMA3CC1 Transfer Controller (EDMA3TC) 1 registers area - 1KB
#define C66XX_EDMA3CC_1_TC_1_RG_AREA_SADDR					0x02778000
#define C66XX_EDMA3CC_1_TC_1_RG_AREA_EADDR					0x027783ff
#define C66XX_EDMA3CC_1_TC_1_RG_AREA_LEN					(C66XX_EDMA3CC_1_TC_1_RG_AREA_EADDR - C66XX_EDMA3CC_1_TC_1_RG_AREA_SADDR + 1)

// EDMA3CC1 Transfer Controller (EDMA3TC) 2 registers area - 1KB
#define C66XX_EDMA3CC_1_TC_2_RG_AREA_SADDR					0x02780000
#define C66XX_EDMA3CC_1_TC_2_RG_AREA_EADDR					0x027803ff
#define C66XX_EDMA3CC_1_TC_2_RG_AREA_LEN					(C66XX_EDMA3CC_1_TC_2_RG_AREA_EADDR - C66XX_EDMA3CC_1_TC_2_RG_AREA_SADDR + 1)

// EDMA3CC1 Transfer Controller (EDMA3TC) 3 registers area - 1KB
#define C66XX_EDMA3CC_1_TC_3_RG_AREA_SADDR					0x02788000
#define C66XX_EDMA3CC_1_TC_3_RG_AREA_EADDR					0x027883ff
#define C66XX_EDMA3CC_1_TC_3_RG_AREA_LEN					(C66XX_EDMA3CC_1_TC_3_RG_AREA_EADDR - C66XX_EDMA3CC_1_TC_3_RG_AREA_SADDR + 1)

// EDMA3CC2 Transfer Controller (EDMA3TC) 0 registers area - 1KB
#define C66XX_EDMA3CC_2_TC_0_RG_AREA_SADDR					0x02790000
#define C66XX_EDMA3CC_2_TC_0_RG_AREA_EADDR					0x027903ff
#define C66XX_EDMA3CC_2_TC_0_RG_AREA_LEN					(C66XX_EDMA3CC_2_TC_0_RG_AREA_EADDR - C66XX_EDMA3CC_2_TC_0_RG_AREA_SADDR + 1)

// EDMA3CC2 Transfer Controller (EDMA3TC) 1 registers area - 1KB
#define C66XX_EDMA3CC_2_TC_1_RG_AREA_SADDR					0x02798000
#define C66XX_EDMA3CC_2_TC_1_RG_AREA_EADDR					0x027983ff
#define C66XX_EDMA3CC_2_TC_1_RG_AREA_LEN					(C66XX_EDMA3CC_2_TC_1_RG_AREA_EADDR - C66XX_EDMA3CC_2_TC_1_RG_AREA_SADDR + 1)

// EDMA3CC2 Transfer Controller (EDMA3TC) 2 registers area - 1KB
#define C66XX_EDMA3CC_2_TC_2_RG_AREA_SADDR					0x027a0000
#define C66XX_EDMA3CC_2_TC_2_RG_AREA_EADDR					0x027a03ff
#define C66XX_EDMA3CC_2_TC_2_RG_AREA_LEN					(C66XX_EDMA3CC_2_TC_2_RG_AREA_EADDR - C66XX_EDMA3CC_2_TC_2_RG_AREA_SADDR + 1)

// EDMA3CC2 Transfer Controller (EDMA3TC) 3 registers area - 1KB
#define C66XX_EDMA3CC_2_TC_3_RG_AREA_SADDR					0x027a8000
#define C66XX_EDMA3CC_2_TC_3_RG_AREA_EADDR					0x027a83ff
#define C66XX_EDMA3CC_2_TC_3_RG_AREA_LEN					(C66XX_EDMA3CC_2_TC_3_RG_AREA_EADDR - C66XX_EDMA3CC_2_TC_3_RG_AREA_SADDR + 1)

// TI embedded trace buffer (TETB) CorePac0 registers area - 4KB
#define C66XX_TETB_0_RG_AREA_SADDR							0x027d0000
#define C66XX_TETB_0_RG_AREA_EADDR							0x027d0fff
#define C66XX_TETB_0_RG_AREA_LEN							(C66XX_TETB_0_RG_AREA_EADDR - C66XX_TETB_0_RG_AREA_SADDR + 1)

// TI embedded trace buffer (TETB) CorePac1 registers area - 4KB
#define C66XX_TETB_1_RG_AREA_SADDR							0x027e0000
#define C66XX_TETB_1_RG_AREA_EADDR							0x027e0fff
#define C66XX_TETB_1_RG_AREA_LEN							(C66XX_TETB_1_RG_AREA_EADDR - C66XX_TETB_1_RG_AREA_SADDR + 1)

// TI embedded trace buffer (TETB) CorePac2 registers area - 4KB
#define C66XX_TETB_2_RG_AREA_SADDR							0x027f0000
#define C66XX_TETB_2_RG_AREA_EADDR							0x027f0fff
#define C66XX_TETB_2_RG_AREA_LEN							(C66XX_TETB_2_RG_AREA_EADDR - C66XX_TETB_2_RG_AREA_SADDR + 1)

// TI embedded trace buffer (TETB) CorePac3 registers area - 4KB
#define C66XX_TETB_3_RG_AREA_SADDR							0x02800000
#define C66XX_TETB_3_RG_AREA_EADDR							0x02800fff
#define C66XX_TETB_3_RG_AREA_LEN							(C66XX_TETB_3_RG_AREA_EADDR - C66XX_TETB_3_RG_AREA_SADDR + 1)

// TI embedded trace buffer (TETB) CorePac4 registers area - 4KB
#define C66XX_TETB_4_RG_AREA_SADDR							0x02810000
#define C66XX_TETB_4_RG_AREA_EADDR							0x02810fff
#define C66XX_TETB_4_RG_AREA_LEN							(C66XX_TETB_4_RG_AREA_EADDR - C66XX_TETB_4_RG_AREA_SADDR + 1)

// TI embedded trace buffer (TETB) CorePac5 registers area - 4KB
#define C66XX_TETB_5_RG_AREA_SADDR							0x02820000
#define C66XX_TETB_5_RG_AREA_EADDR							0x02820fff
#define C66XX_TETB_5_RG_AREA_LEN							(C66XX_TETB_5_RG_AREA_EADDR - C66XX_TETB_5_RG_AREA_SADDR + 1)

// TI embedded trace buffer (TETB) CorePac6 registers area - 4KB
#define C66XX_TETB_6_RG_AREA_SADDR							0x02830000
#define C66XX_TETB_6_RG_AREA_EADDR							0x02830fff
#define C66XX_TETB_6_RG_AREA_LEN							(C66XX_TETB_6_RG_AREA_EADDR - C66XX_TETB_6_RG_AREA_SADDR + 1)

// TI embedded trace buffer (TETB) CorePac7 registers area - 4KB
#define C66XX_TETB_7_RG_AREA_SADDR							0x02840000
#define C66XX_TETB_7_RG_AREA_EADDR							0x02840fff
#define C66XX_TETB_7_RG_AREA_LEN							(C66XX_TETB_7_RG_AREA_EADDR - C66XX_TETB_7_RG_AREA_SADDR + 1)

// TI embedded trace buffer (TETB) system registers area - 32KB
#define C66XX_TETB_SYSTEM_RG_AREA_SADDR						0x02850000
#define C66XX_TETB_SYSTEM_RG_AREA_EADDR						0x02857fff
#define C66XX_TETB_SYSTEM_RG_AREA_LEN						(C66XX_TETB_SYSTEM_RG_AREA_EADDR - C66XX_TETB_SYSTEM_RG_AREA_SADDR + 1)

// Serial RapidIO (SRIO) configuration registers area - 132KB
#define C66XX_SRIO_RG_AREA_SADDR							0x02900000
#define C66XX_SRIO_RG_AREA_EADDR							0x02920fff
#define C66XX_SRIO_RG_AREA_LEN								(C66XX_SRIO_RG_AREA_EADDR - C66XX_SRIO_RG_AREA_SADDR + 1)

// Queue manager subsystem (QMSS) configuration registers area - 2MB
#define C66XX_QMSS_RG_AREA_SADDR							0x02a00000
#define C66XX_QMSS_RG_AREA_EADDR							0x02bfffff
#define C66XX_QMSS_RG_AREA_LEN								(C66XX_QMSS_RG_AREA_EADDR - C66XX_QMSS_RG_AREA_SADDR + 1)

// Extended memory controller (XMC) configuration registers area - 64KB
#define C66XX_XMC_RG_AREA_SADDR								0x08000000
#define C66XX_XMC_RG_AREA_EADDR								0x0800ffff
#define C66XX_XMC_RG_AREA_LEN								(C66XX_XMC_RG_AREA_EADDR - C66XX_XMC_RG_AREA_SADDR + 1)

// Multicore shared memory controller (MSMC) configuration registers area - 1MB
#define C66XX_MSMC_RG_AREA_SADDR							0x0bc00000
#define C66XX_MSMC_RG_AREA_EADDR							0x0bcfffff
#define C66XX_MSMC_RG_AREA_LEN								(C66XX_MSMC_RG_AREA_EADDR - C66XX_MSMC_RG_AREA_SADDR + 1)

// Multicore shared memory (MSM) area - 4MB
#define C66XX_MSM_SRAM_AREA_SADDR							0x0c000000
#define C66XX_MSM_SRAM_AREA_EADDR							0x0c3fffff
#define C66XX_MSM_SRAM_AREA_LEN								(C66XX_MSM_SRAM_AREA_EADDR - C66XX_MSM_SRAM_AREA_SADDR + 1)

// CorePac0 L2 SRAM (address to access from external masters) - 512KB
#define C66XX_DSP_0_L2_SRAM_SADDR							0x10800000
#define C66XX_DSP_0_L2_SRAM_EADDR							0x1087ffff
#define C66XX_DSP_0_L2_SRAM_LEN								(C66XX_DSP_0_L2_SRAM_EADDR - C66XX_DSP_0_L2_SRAM_SADDR + 1)

// CorePac0 L1P SRAM - 32KB
#define C66XX_DSP_0_L1P_SRAM_SADDR							0x10e00000
#define C66XX_DSP_0_L1P_SRAM_EADDR							0x10e07fff
#define C66XX_DSP_0_L1P_SRAM_LEN							(C66XX_DSP_0_L1P_SRAM_EADDR - C66XX_DSP_0_L1P_SRAM_SADDR + 1)

// CorePac0 L1D SRAM - 32KB
#define C66XX_DSP_0_L1D_SRAM_SADDR							0x10f00000
#define C66XX_DSP_0_L1D_SRAM_EADDR							0x10f07fff
#define C66XX_DSP_0_L1D_SRAM_LEN							(C66XX_DSP_0_L1D_SRAM_EADDR - C66XX_DSP_0_L1D_SRAM_SADDR + 1)

// CorePac1 L2 SRAM (address to access from external masters) - 512KB
#define C66XX_DSP_1_L2_SRAM_SADDR							0x11800000
#define C66XX_DSP_1_L2_SRAM_EADDR							0x1187ffff
#define C66XX_DSP_1_L2_SRAM_LEN								(C66XX_DSP_1_L2_SRAM_EADDR - C66XX_DSP_1_L2_SRAM_SADDR + 1)

// CorePac1 L1P SRAM - 32KB
#define C66XX_DSP_1_L1P_SRAM_SADDR							0x11e00000
#define C66XX_DSP_1_L1P_SRAM_EADDR							0x11e07fff
#define C66XX_DSP_1_L1P_SRAM_LEN							(C66XX_DSP_1_L1P_SRAM_EADDR - C66XX_DSP_1_L1P_SRAM_SADDR + 1)

// CorePac1 L1D SRAM - 32KB
#define C66XX_DSP_1_L1D_SRAM_SADDR							0x11f00000
#define C66XX_DSP_1_L1D_SRAM_EADDR							0x11f07fff
#define C66XX_DSP_1_L1D_SRAM_LEN							(C66XX_DSP_1_L1D_SRAM_EADDR - C66XX_DSP_1_L1D_SRAM_SADDR + 1)

// CorePac2 L2 SRAM (address to access from external masters) - 512KB
#define C66XX_DSP_2_L2_SRAM_SADDR							0x12800000
#define C66XX_DSP_2_L2_SRAM_EADDR							0x1287ffff
#define C66XX_DSP_2_L2_SRAM_LEN								(C66XX_DSP_2_L2_SRAM_EADDR - C66XX_DSP_2_L2_SRAM_SADDR + 1)

// CorePac2 L1P SRAM - 32KB
#define C66XX_DSP_2_L1P_SRAM_SADDR							0x12e00000
#define C66XX_DSP_2_L1P_SRAM_EADDR							0x12e07fff
#define C66XX_DSP_2_L1P_SRAM_LEN							(C66XX_DSP_2_L1P_SRAM_EADDR - C66XX_DSP_2_L1P_SRAM_SADDR + 1)

// CorePac2 L1D SRAM - 32KB
#define C66XX_DSP_2_L1D_SRAM_SADDR							0x12f00000
#define C66XX_DSP_2_L1D_SRAM_EADDR							0x12f07fff
#define C66XX_DSP_2_L1D_SRAM_LEN							(C66XX_DSP_2_L1D_SRAM_EADDR - C66XX_DSP_2_L1D_SRAM_SADDR + 1)

// CorePac3 L2 SRAM (address to access from external masters) - 512KB
#define C66XX_DSP_3_L2_SRAM_SADDR							0x13800000
#define C66XX_DSP_3_L2_SRAM_EADDR							0x1387ffff
#define C66XX_DSP_3_L2_SRAM_LEN								(C66XX_DSP_3_L2_SRAM_EADDR - C66XX_DSP_3_L2_SRAM_SADDR + 1)

// CorePac3 L1P SRAM - 32KB
#define C66XX_DSP_3_L1P_SRAM_SADDR							0x13e00000
#define C66XX_DSP_3_L1P_SRAM_EADDR							0x13e07fff
#define C66XX_DSP_3_L1P_SRAM_LEN							(C66XX_DSP_3_L1P_SRAM_EADDR - C66XX_DSP_3_L1P_SRAM_SADDR + 1)

// CorePac3 L1D SRAM - 32KB
#define C66XX_DSP_3_L1D_SRAM_SADDR							0x13f00000
#define C66XX_DSP_3_L1D_SRAM_EADDR							0x13f07fff
#define C66XX_DSP_3_L1D_SRAM_LEN							(C66XX_DSP_3_L1D_SRAM_EADDR - C66XX_DSP_3_L1D_SRAM_SADDR + 1)

// CorePac4 L2 SRAM (address to access from external masters) - 512KB
#define C66XX_DSP_4_L2_SRAM_SADDR							0x14800000
#define C66XX_DSP_4_L2_SRAM_EADDR							0x1487ffff
#define C66XX_DSP_4_L2_SRAM_LEN								(C66XX_DSP_4_L2_SRAM_EADDR - C66XX_DSP_4_L2_SRAM_SADDR + 1)

// CorePac4 L1P SRAM - 32KB
#define C66XX_DSP_4_L1P_SRAM_SADDR							0x14e00000
#define C66XX_DSP_4_L1P_SRAM_EADDR							0x14e07fff
#define C66XX_DSP_4_L1P_SRAM_LEN							(C66XX_DSP_4_L1P_SRAM_EADDR - C66XX_DSP_4_L1P_SRAM_SADDR + 1)

// CorePac4 L1D SRAM - 32KB
#define C66XX_DSP_4_L1D_SRAM_SADDR							0x14f00000
#define C66XX_DSP_4_L1D_SRAM_EADDR							0x14f07fff
#define C66XX_DSP_4_L1D_SRAM_LEN							(C66XX_DSP_4_L1D_SRAM_EADDR - C66XX_DSP_4_L1D_SRAM_SADDR + 1)

// CorePac5 L2 SRAM (address to access from external masters) - 512KB
#define C66XX_DSP_5_L2_SRAM_SADDR							0x15800000
#define C66XX_DSP_5_L2_SRAM_EADDR							0x1587ffff
#define C66XX_DSP_5_L2_SRAM_LEN								(C66XX_DSP_5_L2_SRAM_EADDR - C66XX_DSP_5_L2_SRAM_SADDR + 1)

// CorePac5 L1P SRAM - 32KB
#define C66XX_DSP_5_L1P_SRAM_SADDR							0x15e00000
#define C66XX_DSP_5_L1P_SRAM_EADDR							0x15e07fff
#define C66XX_DSP_5_L1P_SRAM_LEN							(C66XX_DSP_5_L1P_SRAM_EADDR - C66XX_DSP_5_L1P_SRAM_SADDR + 1)

// CorePac5 L1D SRAM - 32KB
#define C66XX_DSP_5_L1D_SRAM_SADDR							0x15f00000
#define C66XX_DSP_5_L1D_SRAM_EADDR							0x15f07fff
#define C66XX_DSP_5_L1D_SRAM_LEN							(C66XX_DSP_5_L1D_SRAM_EADDR - C66XX_DSP_5_L1D_SRAM_SADDR + 1)

// CorePac6 L2 SRAM (address to access from external masters) - 512KB
#define C66XX_DSP_6_L2_SRAM_SADDR							0x16800000
#define C66XX_DSP_6_L2_SRAM_EADDR							0x1687ffff
#define C66XX_DSP_6_L2_SRAM_LEN								(C66XX_DSP_6_L2_SRAM_EADDR - C66XX_DSP_6_L2_SRAM_SADDR + 1)

// CorePac6 L1P SRAM - 32KB
#define C66XX_DSP_6_L1P_SRAM_SADDR							0x16e00000
#define C66XX_DSP_6_L1P_SRAM_EADDR							0x16e07fff
#define C66XX_DSP_6_L1P_SRAM_LEN							(C66XX_DSP_6_L1P_SRAM_EADDR - C66XX_DSP_6_L1P_SRAM_SADDR + 1)

// CorePac6 L1D SRAM - 32KB
#define C66XX_DSP_6_L1D_SRAM_SADDR							0x16f00000
#define C66XX_DSP_6_L1D_SRAM_EADDR							0x16f07fff
#define C66XX_DSP_6_L1D_SRAM_LEN							(C66XX_DSP_6_L1D_SRAM_EADDR - C66XX_DSP_6_L1D_SRAM_SADDR + 1)

// CorePac7 L2 SRAM (address to access from external masters) - 512KB
#define C66XX_DSP_7_L2_SRAM_SADDR							0x17800000
#define C66XX_DSP_7_L2_SRAM_EADDR							0x1787ffff
#define C66XX_DSP_7_L2_SRAM_LEN								(C66XX_DSP_7_L2_SRAM_EADDR - C66XX_DSP_7_L2_SRAM_SADDR + 1)

// CorePac7 L1P SRAM - 32KB
#define C66XX_DSP_7_L1P_SRAM_SADDR							0x17e00000
#define C66XX_DSP_7_L1P_SRAM_EADDR							0x17e07fff
#define C66XX_DSP_7_L1P_SRAM_LEN							(C66XX_DSP_7_L1P_SRAM_EADDR - C66XX_DSP_7_L1P_SRAM_SADDR + 1)

// CorePac7 L1D SRAM - 32KB
#define C66XX_DSP_7_L1D_SRAM_SADDR							0x17f00000
#define C66XX_DSP_7_L1D_SRAM_EADDR							0x17f07fff
#define C66XX_DSP_7_L1D_SRAM_LEN							(C66XX_DSP_7_L1D_SRAM_EADDR - C66XX_DSP_7_L1D_SRAM_SADDR + 1)

// System trace manager (STM) configuration registers area - 1MB
#define C66XX_STM_RG_AREA_SADDR								0x20000000
#define C66XX_STM_RG_AREA_EADDR								0x200fffff
#define C66XX_STM_RG_AREA_LEN								(C66XX_STM_RG_AREA_EADDR - C66XX_STM_RG_AREA_SADDR + 1)

// Boot ROM - 128KB
#define C66XX_BOOT_ROM_SADDR								0x20b00000
#define C66XX_BOOT_ROM_EADDR								0x20b1ffff
#define C66XX_BOOT_ROM_LEN									(C66XX_BOOT_ROM_EADDR - C66XX_BOOT_ROM_SADDR + 1)

// SPI configuration registers area - 512B
#define C66XX_SPI_RG_AREA_SADDR								0x20bf0000
#define C66XX_SPI_RG_AREA_EADDR								0x20bf01ff
#define C66XX_SPI_RG_AREA_LEN								(C66XX_SPI_RG_AREA_EADDR - C66XX_SPI_RG_AREA_SADDR + 1)

// EMIF16 configuration registers area - 256B
#define C66XX_EMIF16_RG_AREA_SADDR							0x20c00000
#define C66XX_EMIF16_RG_AREA_EADDR							0x20c000ff
#define C66XX_EMIF16_RG_AREA_LEN							(C66XX_EMIF16_RG_AREA_EADDR - C66XX_EMIF16_RG_AREA_SADDR + 1)

// DDR3 EMIF configuration registers area - 512B
#define C66XX_DDR3_EMIF_RG_AREA_SADDR						0x21000000
#define C66XX_DDR3_EMIF_RG_AREA_EADDR						0x210001ff
#define C66XX_DDR3_EMIF_RG_AREA_LEN							(C66XX_DDR3_EMIF_RG_AREA_EADDR - C66XX_DDR3_EMIF_RG_AREA_SADDR + 1)

// HyperLink configuration registers area - 256B
#define C66XX_HYPERLINK_RG_AREA_SADDR						0x21400000
#define C66XX_HYPERLINK_RG_AREA_EADDR						0x214000ff
#define C66XX_HYPERLINK_RG_AREA_LEN							(C66XX_HYPERLINK_RG_AREA_EADDR - C66XX_HYPERLINK_RG_AREA_SADDR + 1)

// PCIe configuration registers area - 32KB
#define C66XX_PCIE_RG_AREA_SADDR							0x21800000
#define C66XX_PCIE_RG_AREA_EADDR							0x21807fff
#define C66XX_PCIE_RG_AREA_LEN								(C66XX_PCIE_RG_AREA_EADDR - C66XX_PCIE_RG_AREA_SADDR + 1)

// Queue manager subsystem (QMSS) data area - 2MB
#define C66XX_QMSS_DATA_AREA_SADDR							0x34000000
#define C66XX_QMSS_DATA_AREA_EADDR							0x341fffff
#define C66XX_QMSS_DATA_AREA_LEN							(C66XX_QMSS_DATA_AREA_EADDR - C66XX_QMSS_DATA_AREA_SADDR + 1)

// HyperLink data area - 256MB
#define C66XX_HYPERLINK_DATA_AREA_SADDR						0x40000000
#define C66XX_HYPERLINK_DATA_AREA_EADDR						0x4fffffff
#define C66XX_HYPERLINK_DATA_AREA_LEN						(C66XX_HYPERLINK_DATA_AREA_EADDR - C66XX_HYPERLINK_DATA_AREA_SADDR + 1)

// PCIe data area - 256MB
#define C66XX_PCIE_DATA_AREA_SADDR							0x60000000
#define C66XX_PCIE_DATA_AREA_EADDR							0x6fffffff
#define C66XX_PCIE_DATA_AREA_LEN							(C66XX_PCIE_DATA_AREA_EADDR - C66XX_PCIE_DATA_AREA_SADDR + 1)

// EMIF16 CE0 area - 64MB
#define C66XX_EMIF16_CE0_AREA_SADDR							0x70000000
#define C66XX_EMIF16_CE0_AREA_EADDR							0x73ffffff
#define C66XX_EMIF16_CE0_AREA_LEN							(C66XX_EMIF16_CE0_AREA_EADDR - C66XX_EMIF16_CE0_AREA_SADDR + 1)

// EMIF16 CE1 area - 64MB
#define C66XX_EMIF16_CE1_AREA_SADDR							0x74000000
#define C66XX_EMIF16_CE1_AREA_EADDR							0x77ffffff
#define C66XX_EMIF16_CE1_AREA_LEN							(C66XX_EMIF16_CE1_AREA_EADDR - C66XX_EMIF16_CE1_AREA_SADDR + 1)

// EMIF16 CE2 area - 64MB
#define C66XX_EMIF16_CE2_AREA_SADDR							0x78000000
#define C66XX_EMIF16_CE2_AREA_EADDR							0x7bffffff
#define C66XX_EMIF16_CE2_AREA_LEN							(C66XX_EMIF16_CE2_AREA_EADDR - C66XX_EMIF16_CE2_AREA_SADDR + 1)

// EMIF16 CE3 area - 64MB
#define C66XX_EMIF16_CE3_AREA_SADDR							0x7c000000
#define C66XX_EMIF16_CE3_AREA_EADDR							0x7fffffff
#define C66XX_EMIF16_CE3_AREA_LEN							(C66XX_EMIF16_CE3_AREA_EADDR - C66XX_EMIF16_CE3_AREA_SADDR + 1)

// DDR3 EMIF data area - 2GB
#define C66XX_DDR3_AREA_SADDR								0x80000000
#define C66XX_DDR3_AREA_EADDR								0xffffffff
#define C66XX_DDR3_AREA_LEN									(C66XX_DDR3_AREA_EADDR - C66XX_DDR3_AREA_SADDR + 1)

//=============================================================================
#endif /* __C66XX_SELECT_C6678_DSP__ */



//=============================================================================
//============ DSP core registers =============================================
//=============================================================================
// DSP CorePack revision register - r-only
#define C66XX_CORE_MM_REVID_RG_ADDR							0x01812000

//------------ DSP CorePack revision register defs ----------------------------
#define C66XX_CORE_MM_REVID_VERSION_BITMASK					0xffff0000
#define C66XX_CORE_MM_REVID_VERSION_BITSHIFT				16
#define C66XX_CORE_MM_REVID_REVISION_BITMASK				0x0000ffff
#define C66XX_CORE_MM_REVID_REVISION_BITSHIFT				0

#define C66XX_CORE_MM_REVID_VERSION_C6678					0x0008
#define C66XX_CORE_MM_REVID_REVISION_1_0					0x0
#define C66XX_CORE_MM_REVID_REVISION_2_0					0x1


// DSP interrupt controller registers addresses

// DSP Event Flag register 0 - r-only
#define C66XX_CORE_EVTFLAG0_RG_ADDR							0x01800000
// DSP Event Flag register 1 - r-only
#define C66XX_CORE_EVTFLAG1_RG_ADDR							0x01800004
// DSP Event Flag register 2 - r-only
#define C66XX_CORE_EVTFLAG2_RG_ADDR							0x01800008
// DSP Event Flag register 3 - r-only
#define C66XX_CORE_EVTFLAG3_RG_ADDR							0x0180000c
// DSP Event Set register 0 - r/w
#define C66XX_CORE_EVTSET0_RG_ADDR							0x01800020
// DSP Event Set register 1 - r/w
#define C66XX_CORE_EVTSET1_RG_ADDR							0x01800024
// DSP Event Set register 2 - r/w
#define C66XX_CORE_EVTSET2_RG_ADDR							0x01800028
// DSP Event Set register 3 - r/w
#define C66XX_CORE_EVTSET3_RG_ADDR							0x0180002c
// DSP Event Clear register 0 - r/w
#define C66XX_CORE_EVTCLR0_RG_ADDR							0x01800040
// DSP Event Clear register 1 - r/w
#define C66XX_CORE_EVTCLR1_RG_ADDR							0x01800044
// DSP Event Clear register 2 - r/w
#define C66XX_CORE_EVTCLR2_RG_ADDR							0x01800048
// DSP Event Clear register 3 - r/w
#define C66XX_CORE_EVTCLR3_RG_ADDR							0x0180004c
// DSP Event Mask register 0 - r/w
#define C66XX_CORE_EVTMASK0_RG_ADDR							0x01800080
// DSP Event Mask register 1 - r/w
#define C66XX_CORE_EVTMASK1_RG_ADDR							0x01800084
// DSP Event Mask register 2 - r/w
#define C66XX_CORE_EVTMASK2_RG_ADDR							0x01800088
// DSP Event Mask register 3 - r/w
#define C66XX_CORE_EVTMASK3_RG_ADDR							0x0180008c

// DSP Event Flag registers base address
#define C66XX_CORE_EVTFLAG_RG_BADDR							0x01800000
// DSP Event Flag registers offset
#define C66XX_CORE_EVTFLAG_RG_OFFSET						0x00000004

// DSP Event Set registers base address
#define C66XX_CORE_EVTSET_RG_BADDR							0x01800020
// DSP Event Set registers offset
#define C66XX_CORE_EVTSET_RG_OFFSET							0x00000004

// DSP Event Clear registers base address
#define C66XX_CORE_EVTCLR_RG_BADDR							0x01800040
// DSP Event Clear registers offset
#define C66XX_CORE_EVTCLR_RG_OFFSET							0x00000004

// DSP Event Mask registers base address
#define C66XX_CORE_EVTMASK_RG_BADDR							0x01800080
// DSP Event Mask registers offset
#define C66XX_CORE_EVTMASK_RG_OFFSET						0x00000004

//=============================================================================



//=============================================================================
//============ PLL controller registers =======================================
//=============================================================================
// PLL control register - r/w 
#define C66XX_PLL_PLLCTL_RG_OFFSET							0x100
// PLL secondary control register - r/w 
#define C66XX_PLL_SECCTL_RG_OFFSET							0x108
// PLL multiplier control register - r/w 
#define C66XX_PLL_PLLM_RG_OFFSET							0x110
// PLL controller divider 1 register - r/w 
#define C66XX_PLL_PLLDIV1_RG_OFFSET							0x118
// PLL controller divider 2 register - r/w 
#define C66XX_PLL_PLLDIV2_RG_OFFSET							0x11c
// PLL controller divider 3 register - r/w 
#define C66XX_PLL_PLLDIV3_RG_OFFSET							0x120
// PLL controller command register - r/w 
#define C66XX_PLL_PLLCMD_RG_OFFSET							0x138
// PLL controller status register - r/w 
#define C66XX_PLL_PLLSTAT_RG_OFFSET							0x13c
// PLL controller clock align control register - r/w 
#define C66XX_PLL_ALNCTL_RG_OFFSET							0x140
// PLL controller divider ratio change status register - r/w 
#define C66XX_PLL_DCHANGE_RG_OFFSET							0x144
// SYSCLK status register - r-only
#define C66XX_PLL_SYSTAT_RG_OFFSET							0x150
// PLL controller divider 4 register - r/w 
#define C66XX_PLL_PLLDIV4_RG_OFFSET							0x160
// PLL controller divider 5 register - r/w 
#define C66XX_PLL_PLLDIV5_RG_OFFSET							0x164
// PLL controller divider 6 register - r/w 
#define C66XX_PLL_PLLDIV6_RG_OFFSET							0x168
// PLL controller divider 7 register - r/w 
#define C66XX_PLL_PLLDIV7_RG_OFFSET							0x16c
// PLL controller divider 8 register - r/w 
#define C66XX_PLL_PLLDIV8_RG_OFFSET							0x170
// PLL controller divider 9 register - r/w 
#define C66XX_PLL_PLLDIV9_RG_OFFSET							0x174
// PLL controller divider 10 register - r/w 
#define C66XX_PLL_PLLDIV10_RG_OFFSET						0x178
// PLL controller divider 11 register - r/w 
#define C66XX_PLL_PLLDIV11_RG_OFFSET						0x17c
// PLL controller divider 12 register - r/w 
#define C66XX_PLL_PLLDIV12_RG_OFFSET						0x180
// PLL controller divider 13 register - r/w 
#define C66XX_PLL_PLLDIV13_RG_OFFSET						0x184
// PLL controller divider 14 register - r/w 
#define C66XX_PLL_PLLDIV14_RG_OFFSET						0x188
// PLL controller divider 15 register - r/w 
#define C66XX_PLL_PLLDIV15_RG_OFFSET						0x18c
// PLL controller divider 16 register - r/w 
#define C66XX_PLL_PLLDIV16_RG_OFFSET						0x190

#define C66XX_PLL_PLLCTL_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLCTL_RG_OFFSET)
#define C66XX_PLL_SECCTL_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_SECCTL_RG_OFFSET)
#define C66XX_PLL_PLLM_RG_ADDR								(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLM_RG_OFFSET)
#define C66XX_PLL_PLLDIV1_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLDIV1_RG_OFFSET)
#define C66XX_PLL_PLLDIV2_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLDIV2_RG_OFFSET)
#define C66XX_PLL_PLLDIV3_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLDIV3_RG_OFFSET)
#define C66XX_PLL_PLLCMD_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLCMD_RG_OFFSET)
#define C66XX_PLL_PLLSTAT_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLSTAT_RG_OFFSET)
#define C66XX_PLL_ALNCTL_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_ALNCTL_RG_OFFSET)
#define C66XX_PLL_DCHANGE_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_DCHANGE_RG_OFFSET)
#define C66XX_PLL_SYSTAT_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_SYSTAT_RG_OFFSET)
#define C66XX_PLL_PLLDIV4_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLDIV4_RG_OFFSET)
#define C66XX_PLL_PLLDIV5_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLDIV5_RG_OFFSET)
#define C66XX_PLL_PLLDIV6_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLDIV6_RG_OFFSET)
#define C66XX_PLL_PLLDIV7_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLDIV7_RG_OFFSET)
#define C66XX_PLL_PLLDIV8_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLDIV8_RG_OFFSET)
#define C66XX_PLL_PLLDIV9_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLDIV9_RG_OFFSET)
#define C66XX_PLL_PLLDIV10_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLDIV10_RG_OFFSET)
#define C66XX_PLL_PLLDIV11_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLDIV11_RG_OFFSET)
#define C66XX_PLL_PLLDIV12_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLDIV12_RG_OFFSET)
#define C66XX_PLL_PLLDIV13_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLDIV13_RG_OFFSET)
#define C66XX_PLL_PLLDIV14_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLDIV14_RG_OFFSET)
#define C66XX_PLL_PLLDIV15_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLDIV15_RG_OFFSET)
#define C66XX_PLL_PLLDIV16_RG_ADDR							(C66XX_PLL_RG_AREA_SADDR + C66XX_PLL_PLLDIV16_RG_OFFSET)


//------------ PLL control register defs --------------------------------------
#define C66XX_PLL_PLLCTL_PLLENSRC_BITMASK					0x20
#define C66XX_PLL_PLLCTL_PLLENSRC_BITSHIFT					5
#define C66XX_PLL_PLLCTL_PLLRST_BITMASK						0x8
#define C66XX_PLL_PLLCTL_PLLRST_BITSHIFT					3
#define C66XX_PLL_PLLCTL_PLLPWRDN_BITMASK					0x2
#define C66XX_PLL_PLLCTL_PLLPWRDN_BITSHIFT					1
#define C66XX_PLL_PLLCTL_PLLEN_BITMASK						0x1
#define C66XX_PLL_PLLCTL_PLLEN_BITSHIFT						0


//------------ PLL secondary control register defs ----------------------------
#define C66XX_PLL_SECCTL_BYPASS_BITMASK						0x00800000
#define C66XX_PLL_SECCTL_BYPASS_BITSHIFT					23
#define C66XX_PLL_SECCTL_OUTPUT_DIVIDE_BITMASK				0x00780000
#define C66XX_PLL_SECCTL_OUTPUT_DIVIDE_BITSHIFT				19


//------------ PLL multiplier control register defs ---------------------------
#define C66XX_PLL_PLLM_PLLM_BITMASK							0x3f
#define C66XX_PLL_PLLM_PLLM_BITSHIFT						0


//------------ PLL controller divider registers defs --------------------------
#define C66XX_PLL_PLLDIV_DEN_BITMASK						0x8000
#define C66XX_PLL_PLLDIV_DEN_BITSHIFT						15
#define C66XX_PLL_PLLDIV_RATIO_BITMASK						0xff
#define C66XX_PLL_PLLDIV_RATIO_BITSHIFT						0


//------------ PLL controller command register defs ---------------------------
#define C66XX_PLL_PLLCMD_GOSET_BITMASK						0x1
#define C66XX_PLL_PLLCMD_GOSET_BITSHIFT						0


//------------ PLL controller status register defs ----------------------------
#define C66XX_PLL_PLLSTAT_GOSTAT_BITMASK					0x1
#define C66XX_PLL_PLLSTAT_GOSTAT_BITSHIFT					0


//------------ PLL controller clock align control register defs ---------------
#define C66XX_PLL_ALNCTL_ALN_BITMASK						0xffff
#define C66XX_PLL_ALNCTL_ALN_BITSHIFT						0


//------------ PLL controller divider ratio change status register defs -------
#define C66XX_PLL_DCHANGE_SYS_BITMASK						0xffff
#define C66XX_PLL_DCHANGE_SYS_BITSHIFT						0


//------------ SYSCLK status register defs ------------------------------------
#define C66XX_PLL_SYSTAT_SYSON_BITMASK						0xffff
#define C66XX_PLL_SYSTAT_SYSON_BITSHIFT						0

//=============================================================================



//=============================================================================
//============ Device State Control registers =================================
//=============================================================================
/*
 * !!! Note that this section contains only those definitions
 * that are missed in TI C6000 Chip Support Library (CSL) !!!
 */

// JTAG ID register - r-only
#define C66XX_BOOTCFG_JTAGID_RG_OFFSET						0x18
// Device status register - r/w
#define C66XX_BOOTCFG_DEVSTAT_RG_OFFSET						0x20
// Boot kicker 0 register - r/w
#define C66XX_BOOTCFG_KICK0_RG_OFFSET						0x38
// Boot kicker 1 register - r/w
#define C66XX_BOOTCFG_KICK1_RG_OFFSET						0x3c
// DSP0 boot address register - r/w
#define C66XX_BOOTCFG_DSP_BOOT_ADDR0_RG_OFFSET				0x40
// DSP1 boot address register - r/w
#define C66XX_BOOTCFG_DSP_BOOT_ADDR1_RG_OFFSET				0x44
// DSP2 boot address register - r/w
#define C66XX_BOOTCFG_DSP_BOOT_ADDR2_RG_OFFSET				0x48
// DSP3 boot address register - r/w
#define C66XX_BOOTCFG_DSP_BOOT_ADDR3_RG_OFFSET				0x4c
// DSP4 boot address register - r/w
#define C66XX_BOOTCFG_DSP_BOOT_ADDR4_RG_OFFSET				0x50
// DSP5 boot address register - r/w
#define C66XX_BOOTCFG_DSP_BOOT_ADDR5_RG_OFFSET				0x54
// DSP6 boot address register - r/w
#define C66XX_BOOTCFG_DSP_BOOT_ADDR6_RG_OFFSET				0x58
// DSP7 boot address register - r/w
#define C66XX_BOOTCFG_DSP_BOOT_ADDR7_RG_OFFSET				0x5c
// MAC 1 address register - r-only
#define C66XX_BOOTCFG_MACID1_RG_OFFSET						0x110
// MAC 2 address register - r-only
#define C66XX_BOOTCFG_MACID2_RG_OFFSET						0x114
// LRESETNMI PIN status clear register - r/w
#define C66XX_BOOTCFG_LRSTNMIPINSTAT_CLR_RG_OFFSET			0x130
// Reset status clear register - r/w
#define C66XX_BOOTCFG_RESET_STAT_CLR_RG_OFFSET				0x134
// Boot complete register - r/w
#define C66XX_BOOTCFG_BOOTCOMPLETE_RG_OFFSET				0x13c
// Reset status register - r-only
#define C66XX_BOOTCFG_RESET_STAT_RG_OFFSET					0x144
// LRESETNMI PIN status register - r-only
#define C66XX_BOOTCFG_LRSTNMIPINSTAT_RG_OFFSET				0x148
// Device configuration register - r/w
#define C66XX_BOOTCFG_DEVCFG_RG_OFFSET						0x14c
// Power state control register - r/w
#define C66XX_BOOTCFG_PWRSTATECTL_RG_OFFSET					0x150
// SRIO SerDes macro status register - r-only
#define C66XX_BOOTCFG_SRIO_SERDES_STS_RG_OFFSET				0x154
// SMGII SerDes macro status register - r-only
#define C66XX_BOOTCFG_SMGII_SERDES_STS_RG_OFFSET			0x158
// PCIe SerDes macro status register - r-only
#define C66XX_BOOTCFG_PCIE_SERDES_STS_RG_OFFSET				0x15c
// HyperLink SerDes macro status register - r-only
#define C66XX_BOOTCFG_HYPERLINK_SERDES_STS_RG_OFFSET		0x160
// NMI generation for CorePac0 register - r/w
#define C66XX_BOOTCFG_NMIGR0_RG_OFFSET						0x200
// NMI generation for CorePac1 register - r/w
#define C66XX_BOOTCFG_NMIGR1_RG_OFFSET						0x204
// NMI generation for CorePac2 register - r/w
#define C66XX_BOOTCFG_NMIGR2_RG_OFFSET						0x208
// NMI generation for CorePac3 register - r/w
#define C66XX_BOOTCFG_NMIGR3_RG_OFFSET						0x20c
// NMI generation for CorePac4 register - r/w
#define C66XX_BOOTCFG_NMIGR4_RG_OFFSET						0x210
// NMI generation for CorePac5 register - r/w
#define C66XX_BOOTCFG_NMIGR5_RG_OFFSET						0x214
// NMI generation for CorePac6 register - r/w
#define C66XX_BOOTCFG_NMIGR6_RG_OFFSET						0x218
// NMI generation for CorePac7 register - r/w
#define C66XX_BOOTCFG_NMIGR7_RG_OFFSET						0x21c
// IPC generation for CorePac0 register - r/w
#define C66XX_BOOTCFG_IPCGR0_RG_OFFSET						0x240
// IPC generation for CorePac1 register - r/w
#define C66XX_BOOTCFG_IPCGR1_RG_OFFSET						0x244
// IPC generation for CorePac2 register - r/w
#define C66XX_BOOTCFG_IPCGR2_RG_OFFSET						0x248
// IPC generation for CorePac3 register - r/w
#define C66XX_BOOTCFG_IPCGR3_RG_OFFSET						0x24c
// IPC generation for CorePac4 register - r/w
#define C66XX_BOOTCFG_IPCGR4_RG_OFFSET						0x250
// IPC generation for CorePac5 register - r/w
#define C66XX_BOOTCFG_IPCGR5_RG_OFFSET						0x254
// IPC generation for CorePac6 register - r/w
#define C66XX_BOOTCFG_IPCGR6_RG_OFFSET						0x258
// IPC generation for CorePac7 register - r/w
#define C66XX_BOOTCFG_IPCGR7_RG_OFFSET						0x25c

// Timer Input Selection register - r/w
#define C66XX_BOOTCFG_TINPSEL_RG_OFFSET						0x300
// Timer Output Selection register - r/w
#define C66XX_BOOTCFG_TOUTPSEL_RG_OFFSET					0x304

// Main PLL Control register 0 - r/w
#define C66XX_BOOTCFG_MAINPLLCTL0_RG_OFFSET					0x328
// Main PLL Control register 1 - r/w
#define C66XX_BOOTCFG_MAINPLLCTL1_RG_OFFSET					0x32c
// DDR3 PLL Control register 0 - r/w
#define C66XX_BOOTCFG_DDR3PLLCTL0_RG_OFFSET					0x330
// DDR3 PLL Control register 1 - r/w
#define C66XX_BOOTCFG_DDR3PLLCTL1_RG_OFFSET					0x334
// Pass PLL Control register 0 - r/w
#define C66XX_BOOTCFG_PASSPLLCTL0_RG_OFFSET					0x338
// Pass PLL Control register 1 - r/w
#define C66XX_BOOTCFG_PASSPLLCTL1_RG_OFFSET					0x33c
// Device speed register - r-only
#define C66XX_BOOTCFG_DEVSPEED_RG_OFFSET					0x3f8

#define C66XX_BOOTCFG_TINPSEL_RG_ADDR						(C66XX_BOOTCFG_RG_AREA_SADDR + C66XX_BOOTCFG_TINPSEL_RG_OFFSET)
#define C66XX_BOOTCFG_TOUTPSEL_RG_ADDR						(C66XX_BOOTCFG_RG_AREA_SADDR + C66XX_BOOTCFG_TOUTPSEL_RG_OFFSET)
#define C66XX_BOOTCFG_MAINPLLCTL0_RG_ADDR					(C66XX_BOOTCFG_RG_AREA_SADDR + C66XX_BOOTCFG_MAINPLLCTL0_RG_OFFSET)
#define C66XX_BOOTCFG_MAINPLLCTL1_RG_ADDR					(C66XX_BOOTCFG_RG_AREA_SADDR + C66XX_BOOTCFG_MAINPLLCTL1_RG_OFFSET)
#define C66XX_BOOTCFG_DDR3PLLCTL0_RG_ADDR					(C66XX_BOOTCFG_RG_AREA_SADDR + C66XX_BOOTCFG_DDR3PLLCTL0_RG_OFFSET)
#define C66XX_BOOTCFG_DDR3PLLCTL1_RG_ADDR					(C66XX_BOOTCFG_RG_AREA_SADDR + C66XX_BOOTCFG_DDR3PLLCTL1_RG_OFFSET)
#define C66XX_BOOTCFG_PASSPLLCTL0_RG_ADDR					(C66XX_BOOTCFG_RG_AREA_SADDR + C66XX_BOOTCFG_PASSPLLCTL0_RG_OFFSET)
#define C66XX_BOOTCFG_PASSPLLCTL1_RG_ADDR					(C66XX_BOOTCFG_RG_AREA_SADDR + C66XX_BOOTCFG_PASSPLLCTL1_RG_OFFSET)
#define C66XX_BOOTCFG_DEVSPEED_RG_ADDR						(C66XX_BOOTCFG_RG_AREA_SADDR + C66XX_BOOTCFG_DEVSPEED_RG_OFFSET)


//------------ Main PLL Control register 0 defs -------------------------------
#define C66XX_BOOTCFG_MAINPLLCTL0_BWADJ_BITMASK				0xff000000
#define C66XX_BOOTCFG_MAINPLLCTL0_BWADJ_BITSHIFT			24
#define C66XX_BOOTCFG_MAINPLLCTL0_PLLM_BITMASK				0x7f000
#define C66XX_BOOTCFG_MAINPLLCTL0_PLLM_BITSHIFT				12
#define C66XX_BOOTCFG_MAINPLLCTL0_PLLD_BITMASK				0x3f
#define C66XX_BOOTCFG_MAINPLLCTL0_PLLD_BITSHIFT				0


//------------ Main PLL Control register 1 defs -------------------------------
#define C66XX_BOOTCFG_MAINPLLCTL1_ENSAT_BITMASK				0x40
#define C66XX_BOOTCFG_MAINPLLCTL1_ENSAT_BITSHIFT			6
#define C66XX_BOOTCFG_MAINPLLCTL1_BWADJ_BITMASK				0xf
#define C66XX_BOOTCFG_MAINPLLCTL1_BWADJ_BITSHIFT			0


//------------ DDR3 PLL Control register 0 defs -------------------------------
#define C66XX_BOOTCFG_DDR3PLLCTL0_BWADJ_BITMASK				0xff000000
#define C66XX_BOOTCFG_DDR3PLLCTL0_BWADJ_BITSHIFT			24
#define C66XX_BOOTCFG_DDR3PLLCTL0_BYPASS_BITMASK			0x800000
#define C66XX_BOOTCFG_DDR3PLLCTL0_BYPASS_BITSHIFT			23
#define C66XX_BOOTCFG_DDR3PLLCTL0_PLLM_BITMASK				0x7ffc0
#define C66XX_BOOTCFG_DDR3PLLCTL0_PLLM_BITSHIFT				6
#define C66XX_BOOTCFG_DDR3PLLCTL0_PLLD_BITMASK				0x3f
#define C66XX_BOOTCFG_DDR3PLLCTL0_PLLD_BITSHIFT				0


//------------ DDR3 PLL Control register 1 defs -------------------------------
#define C66XX_BOOTCFG_DDR3PLLCTL1_PLLRST_BITMASK			0x2000
#define C66XX_BOOTCFG_DDR3PLLCTL1_PLLRST_BITSHIFT			13
#define C66XX_BOOTCFG_DDR3PLLCTL1_ENSAT_BITMASK				0x40
#define C66XX_BOOTCFG_DDR3PLLCTL1_ENSAT_BITSHIFT			6
#define C66XX_BOOTCFG_DDR3PLLCTL1_BWADJ_BITMASK				0xf
#define C66XX_BOOTCFG_DDR3PLLCTL1_BWADJ_BITSHIFT			0


//------------ PASS PLL Control register 0 defs -------------------------------
#define C66XX_BOOTCFG_PASSPLLCTL0_BWADJ_BITMASK				0xff000000
#define C66XX_BOOTCFG_PASSPLLCTL0_BWADJ_BITSHIFT			24
#define C66XX_BOOTCFG_PASSPLLCTL0_BYPASS_BITMASK			0x800000
#define C66XX_BOOTCFG_PASSPLLCTL0_BYPASS_BITSHIFT			23
#define C66XX_BOOTCFG_PASSPLLCTL0_PLLM_BITMASK				0x7ffc0
#define C66XX_BOOTCFG_PASSPLLCTL0_PLLM_BITSHIFT				6
#define C66XX_BOOTCFG_PASSPLLCTL0_PLLD_BITMASK				0x3f
#define C66XX_BOOTCFG_PASSPLLCTL0_PLLD_BITSHIFT				0


//------------ PASS PLL Control register 1 defs -------------------------------
#define C66XX_BOOTCFG_PASSPLLCTL1_PLLRST_BITMASK			0x4000
#define C66XX_BOOTCFG_PASSPLLCTL1_PLLRST_BITSHIFT			14
#define C66XX_BOOTCFG_PASSPLLCTL1_PLLSELECT_BITMASK			0x2000
#define C66XX_BOOTCFG_PASSPLLCTL1_PLLSELECT_BITSHIFT		13
#define C66XX_BOOTCFG_PASSPLLCTL1_ENSAT_BITMASK				0x40
#define C66XX_BOOTCFG_PASSPLLCTL1_ENSAT_BITSHIFT			6
#define C66XX_BOOTCFG_PASSPLLCTL1_BWADJ_BITMASK				0xf
#define C66XX_BOOTCFG_PASSPLLCTL1_BWADJ_BITSHIFT			0


//------------ Device speed register defs -------------------------------------
#define C66XX_BOOTCFG_DEVSPEED_DEVSPEED_BITMASK				0xff800000
#define C66XX_BOOTCFG_DEVSPEED_DEVSPEED_BITSHIFT			23

// DSP core speed defs in MHz
#define C66XX_DSP_CORE_SPEED_800MHZ							800
#define C66XX_DSP_CORE_SPEED_1000MHZ						1000
#define C66XX_DSP_CORE_SPEED_1200MHZ						1200
#define C66XX_DSP_CORE_SPEED_1250MHZ						1250
#define C66XX_DSP_CORE_SPEED_1400MHZ						1400


//------------ MACID2 register defs -------------------------------------------
#define C66XX_BOOTCFG_MACID2_MACID_BITMASK					0xffff
#define C66XX_BOOTCFG_MACID2_MACID_BITSHIFT					0

//=============================================================================



//=============================================================================
//============ Power & sleep controller definitions ===========================
//=============================================================================
/*
 * !!! Note that this section contains only those definitions
 * that are missed in TI C6000 Chip Support Library (CSL) !!!
 */

// Power Domains Definitions
#define C66XX_PSC_PD_ALWAYSON								0
#define C66XX_PSC_PD_TETB									1
#define C66XX_PSC_PD_PA										2
#define C66XX_PSC_PD_PCIE									3
#define C66XX_PSC_PD_SRIO									4
#define C66XX_PSC_PD_HYPERLINK								5
#define C66XX_PSC_PD_MSM_SRAM								7
#define C66XX_PSC_PD_DSP0									8
#define C66XX_PSC_PD_DSP1									9
#define C66XX_PSC_PD_DSP2									10
#define C66XX_PSC_PD_DSP3									11
#define C66XX_PSC_PD_DSP4									12
#define C66XX_PSC_PD_DSP5									13
#define C66XX_PSC_PD_DSP6									14
#define C66XX_PSC_PD_DSP7									15
#define C66XX_PSC_PD_COUNT									(C66XX_PSC_PD_DSP7 + 1)

// Power Domains State Definitions
#define C66XX_PSC_PD_STATE_OFF								PSC_PDSTATE_OFF
#define C66XX_PSC_PD_STATE_ON								PSC_PDSTATE_ON


// Power & Sleep Controller Clock Domains Definitions
#define C66XX_PSC_LPSC_SHARED_LPSC							0
#define C66XX_PSC_LPSC_SMARTREFLEX							1
#define C66XX_PSC_LPSC_DDR3_EMIF							2
#define C66XX_PSC_LPSC_EMIF16_SPI							3
#define C66XX_PSC_LPSC_TSIP									4
#define C66XX_PSC_LPSC_DEBUG_TRACE							5
#define C66XX_PSC_LPSC_TETB									6
#define C66XX_PSC_LPSC_PA									7
#define C66XX_PSC_LPSC_ETHERNET								8
#define C66XX_PSC_LPSC_SA									9
#define C66XX_PSC_LPSC_PCIE									10
#define C66XX_PSC_LPSC_SRIO									11
#define C66XX_PSC_LPSC_HYPERLINK							12
#define C66XX_PSC_LPSC_MSM_SRAM								14
#define C66XX_PSC_LPSC_DSP0									15
#define C66XX_PSC_LPSC_DSP1									16
#define C66XX_PSC_LPSC_DSP2									17
#define C66XX_PSC_LPSC_DSP3									18
#define C66XX_PSC_LPSC_DSP4									19
#define C66XX_PSC_LPSC_DSP5									20
#define C66XX_PSC_LPSC_DSP6									21
#define C66XX_PSC_LPSC_DSP7									22
#define C66XX_PSC_LPSC_COUNT								(C66XX_PSC_LPSC_DSP7 + 1)

// Clock Domains State Definitions
#define C66XX_PSC_LPSC_STATE_OFF							PSC_MODSTATE_SWRSTDISABLE
#define C66XX_PSC_LPSC_STATE_ON								PSC_MODSTATE_ENABLE

//=============================================================================



//=============================================================================
//============ I2C definitions ================================================
//=============================================================================
/*
 * !!! Note that this section contains only those definitions
 * that are missed in TI C6000 Chip Support Library (CSL) !!!
 */

// I2C own address register - r/w
#define C66XX_I2C_ICOAR_RG_OFFSET							0x00
// I2C interrupt mask register - r/w
#define C66XX_I2C_ICIMR_RG_OFFSET							0x04
// I2C interrupt status register - r/w
#define C66XX_I2C_ICSTR_RG_OFFSET							0x08
// I2C clock low-time divider register - r/w
#define C66XX_I2C_ICCLKL_RG_OFFSET							0x0c
// I2C clock high-time divider register - r/w
#define C66XX_I2C_ICCLKH_RG_OFFSET							0x10
// I2C data count register - r/w
#define C66XX_I2C_ICCNT_RG_OFFSET							0x14
// I2C data receive register - r-only
#define C66XX_I2C_ICDRR_RG_OFFSET							0x18
// I2C slave address register - r/w
#define C66XX_I2C_ICSAR_RG_OFFSET							0x1c
// I2C data transmit register - r/w
#define C66XX_I2C_ICDXR_RG_OFFSET							0x20
// I2C mode register - r/w
#define C66XX_I2C_ICMDR_RG_OFFSET							0x24
// I2C interrupt vector register - r/w
#define C66XX_I2C_ICIVR_RG_OFFSET							0x28
// I2C extended mode register - r/w
#define C66XX_I2C_ICEMDR_RG_OFFSET							0x2c
// I2C prescaler register - r/w
#define C66XX_I2C_ICPSC_RG_OFFSET							0x30
// I2C peripheral identification 1 register - r/w
#define C66XX_I2C_ICPID1_RG_OFFSET							0x34
// I2C peripheral identification 2 register - r/w
#define C66XX_I2C_ICPID2_RG_OFFSET							0x38

#define C66XX_I2C_ICOAR_RG_ADDR								(C66XX_I2C_RG_AREA_SADDR + C66XX_I2C_ICOAR_RG_OFFSET)
#define C66XX_I2C_ICIMR_RG_ADDR								(C66XX_I2C_RG_AREA_SADDR + C66XX_I2C_ICIMR_RG_OFFSET)
#define C66XX_I2C_ICSTR_RG_ADDR								(C66XX_I2C_RG_AREA_SADDR + C66XX_I2C_ICSTR_RG_OFFSET)
#define C66XX_I2C_ICCLKL_RG_ADDR							(C66XX_I2C_RG_AREA_SADDR + C66XX_I2C_ICCLKL_RG_OFFSET)
#define C66XX_I2C_ICCLKH_RG_ADDR							(C66XX_I2C_RG_AREA_SADDR + C66XX_I2C_ICCLKH_RG_OFFSET)
#define C66XX_I2C_ICCNT_RG_ADDR								(C66XX_I2C_RG_AREA_SADDR + C66XX_I2C_ICCNT_RG_OFFSET)
#define C66XX_I2C_ICDRR_RG_ADDR								(C66XX_I2C_RG_AREA_SADDR + C66XX_I2C_ICDRR_RG_OFFSET)
#define C66XX_I2C_ICSAR_RG_ADDR								(C66XX_I2C_RG_AREA_SADDR + C66XX_I2C_ICSAR_RG_OFFSET)
#define C66XX_I2C_ICDXR_RG_ADDR								(C66XX_I2C_RG_AREA_SADDR + C66XX_I2C_ICDXR_RG_OFFSET)
#define C66XX_I2C_ICMDR_RG_ADDR								(C66XX_I2C_RG_AREA_SADDR + C66XX_I2C_ICMDR_RG_OFFSET)
#define C66XX_I2C_ICIVR_RG_ADDR								(C66XX_I2C_RG_AREA_SADDR + C66XX_I2C_ICIVR_RG_OFFSET)
#define C66XX_I2C_ICEMDR_RG_ADDR							(C66XX_I2C_RG_AREA_SADDR + C66XX_I2C_ICEMDR_RG_OFFSET)
#define C66XX_I2C_ICPSC_RG_ADDR								(C66XX_I2C_RG_AREA_SADDR + C66XX_I2C_ICPSC_RG_OFFSET)
#define C66XX_I2C_ICPID1_RG_ADDR							(C66XX_I2C_RG_AREA_SADDR + C66XX_I2C_ICPID1_RG_OFFSET)
#define C66XX_I2C_ICPID2_RG_ADDR							(C66XX_I2C_RG_AREA_SADDR + C66XX_I2C_ICPID2_RG_OFFSET)


//------------ I2C own address register defs ----------------------------------
#define C66XX_I2C_ICOAR_OADDR_BITMASK						0x3ff
#define C66XX_I2C_ICOAR_OADDR_BITSHIFT						0


//------------ I2C interrupt mask register defs -------------------------------
#define C66XX_I2C_ICIMR_AAS_BITMASK							0x40
#define C66XX_I2C_ICIMR_AAS_BITSHIFT						6
#define C66XX_I2C_ICIMR_SCD_BITMASK							0x20
#define C66XX_I2C_ICIMR_SCD_BITSHIFT						5
#define C66XX_I2C_ICIMR_ICXRDY_BITMASK						0x10
#define C66XX_I2C_ICIMR_ICXRDY_BITSHIFT						4
#define C66XX_I2C_ICIMR_ICRDRDY_BITMASK						0x8
#define C66XX_I2C_ICIMR_ICRDRDY_BITSHIFT					3
#define C66XX_I2C_ICIMR_ARDY_BITMASK						0x4
#define C66XX_I2C_ICIMR_ARDY_BITSHIFT						2
#define C66XX_I2C_ICIMR_NACK_BITMASK						0x2
#define C66XX_I2C_ICIMR_NACK_BITSHIFT						1
#define C66XX_I2C_ICIMR_AL_BITMASK							0x1
#define C66XX_I2C_ICIMR_AL_BITSHIFT							0
#define C66XX_I2C_ICIMR_RG_BITMASK							0x7f


//------------ I2C interrupt status register defs -----------------------------
#define C66XX_I2C_ICSTR_SDIR_BITMASK						0x4000
#define C66XX_I2C_ICSTR_SDIR_BITSHIFT						14
#define C66XX_I2C_ICSTR_NACKSNT_BITMASK						0x2000
#define C66XX_I2C_ICSTR_NACKSNT_BITSHIFT					13
#define C66XX_I2C_ICSTR_BB_BITMASK							0x1000
#define C66XX_I2C_ICSTR_BB_BITSHIFT							12
#define C66XX_I2C_ICSTR_RSFULL_BITMASK						0x800
#define C66XX_I2C_ICSTR_RSFULL_BITSHIFT						11
#define C66XX_I2C_ICSTR_XSMT_BITMASK						0x400
#define C66XX_I2C_ICSTR_XSMT_BITSHIFT						10
#define C66XX_I2C_ICSTR_AAS_BITMASK							0x200
#define C66XX_I2C_ICSTR_AAS_BITSHIFT						9
#define C66XX_I2C_ICSTR_AD0_BITMASK							0x100
#define C66XX_I2C_ICSTR_AD0_BITSHIFT						8
#define C66XX_I2C_ICSTR_SCD_BITMASK							0x20
#define C66XX_I2C_ICSTR_SCD_BITSHIFT						5
#define C66XX_I2C_ICSTR_ICXRDY_BITMASK						0x10
#define C66XX_I2C_ICSTR_ICXRDY_BITSHIFT						4
#define C66XX_I2C_ICSTR_ICRDRDY_BITMASK						0x8
#define C66XX_I2C_ICSTR_ICRDRDY_BITSHIFT					3
#define C66XX_I2C_ICSTR_ARDY_BITMASK						0x4
#define C66XX_I2C_ICSTR_ARDY_BITSHIFT						2
#define C66XX_I2C_ICSTR_NACK_BITMASK						0x2
#define C66XX_I2C_ICSTR_NACK_BITSHIFT						1
#define C66XX_I2C_ICSTR_AL_BITMASK							0x1
#define C66XX_I2C_ICSTR_AL_BITSHIFT							0


//------------ I2C clock low-time divider register defs -----------------------
#define C66XX_I2C_ICCLKL_ICCL_BITMASK						0xffff
#define C66XX_I2C_ICCLKL_ICCL_BITSHIFT						0


//------------ I2C clock high-time divider register defs ----------------------
#define C66XX_I2C_ICCLKH_ICCH_BITMASK						0xffff
#define C66XX_I2C_ICCLKH_ICCH_BITSHIFT						0


//------------ I2C data count register defs -----------------------------------
#define C66XX_I2C_ICCNT_ICDC_BITMASK						0xffff
#define C66XX_I2C_ICCNT_ICDC_BITSHIFT						0


//------------ I2C data receive register defs ---------------------------------
#define C66XX_I2C_ICDRR_D_BITMASK							0xff
#define C66XX_I2C_ICDRR_D_BITSHIFT							0


//------------ I2C slave address register defs --------------------------------
#define C66XX_I2C_ICSAR_SADDR_BITMASK						0x3ff
#define C66XX_I2C_ICSAR_SADDR_BITSHIFT						0


//------------ I2C data transmit register defs --------------------------------
#define C66XX_I2C_ICDXR_D_BITMASK							0xff
#define C66XX_I2C_ICDXR_D_BITSHIFT							0


//------------ I2C mode register defs -----------------------------------------
#define C66XX_I2C_ICMDR_NACKMOD_BITMASK						0x8000
#define C66XX_I2C_ICMDR_NACKMOD_BITSHIFT					15
#define C66XX_I2C_ICMDR_FREE_BITMASK						0x4000
#define C66XX_I2C_ICMDR_FREE_BITSHIFT						14
#define C66XX_I2C_ICMDR_STT_BITMASK							0x2000
#define C66XX_I2C_ICMDR_STT_BITSHIFT						13
#define C66XX_I2C_ICMDR_STP_BITMASK							0x800
#define C66XX_I2C_ICMDR_STP_BITSHIFT						11
#define C66XX_I2C_ICMDR_MST_BITMASK							0x400
#define C66XX_I2C_ICMDR_MST_BITSHIFT						10
#define C66XX_I2C_ICMDR_TRX_BITMASK							0x200
#define C66XX_I2C_ICMDR_TRX_BITSHIFT						9
#define C66XX_I2C_ICMDR_XA_BITMASK							0x100
#define C66XX_I2C_ICMDR_XA_BITSHIFT							8
#define C66XX_I2C_ICMDR_RM_BITMASK							0x80
#define C66XX_I2C_ICMDR_RM_BITSHIFT							7
#define C66XX_I2C_ICMDR_DLB_BITMASK							0x40
#define C66XX_I2C_ICMDR_DLB_BITSHIFT						6
#define C66XX_I2C_ICMDR_IRS_BITMASK							0x20
#define C66XX_I2C_ICMDR_IRS_BITSHIFT						5
#define C66XX_I2C_ICMDR_STB_BITMASK							0x10
#define C66XX_I2C_ICMDR_STB_BITSHIFT						4
#define C66XX_I2C_ICMDR_FDF_BITMASK							0x8
#define C66XX_I2C_ICMDR_FDF_BITSHIFT						3
#define C66XX_I2C_ICMDR_BC_BITMASK							0x7
#define C66XX_I2C_ICMDR_BC_BITSHIFT							0


//------------ I2C interrupt vector register defs -----------------------------
#define C66XX_I2C_ICIVR_INTCODE_BITMASK						0x7
#define C66XX_I2C_ICIVR_INTCODE_BITSHIFT					0


//------------ I2C extended mode register defs --------------------------------
#define C66XX_I2C_ICEMDR_IGNACK_BITMASK						0x2
#define C66XX_I2C_ICEMDR_IGNACK_BITSHIFT					1
#define C66XX_I2C_ICEMDR_BCM_BITMASK						0x1
#define C66XX_I2C_ICEMDR_BCM_BITSHIFT						0


//------------ I2C prescaler register defs ------------------------------------
#define C66XX_I2C_ICPSC_IPSC_BITMASK						0xff
#define C66XX_I2C_ICPSC_IPSC_BITSHIFT						0


//------------ I2C peripheral identification 1 register defs ------------------
#define C66XX_I2C_ICPID1_CLASS_BITMASK						0xff00
#define C66XX_I2C_ICPID1_CLASS_BITSHIFT						8
#define C66XX_I2C_ICPID1_REVISION_BITMASK					0xff
#define C66XX_I2C_ICPID1_REVISION_BITSHIFT					0


//------------ I2C peripheral identification 2 register defs ------------------
#define C66XX_I2C_ICPID2_TYPE_BITMASK						0xffff
#define C66XX_I2C_ICPID2_TYPE_BITSHIFT						0

//=============================================================================



//=============================================================================
//============ EMIF16 configuration registers =================================
//=============================================================================
// Revision code and status register - r-only
#define C66XX_EMIF16_RCSR_RG_OFFSET							0x00
// Async wait cycle config register - r/w
#define C66XX_EMIF16_AWCCR_RG_OFFSET						0x04
// Async 1 (CE0) config register - r/w
#define C66XX_EMIF16_A1CR_RG_OFFSET							0x10
// Async 2 (CE1) config register - r/w
#define C66XX_EMIF16_A2CR_RG_OFFSET							0x14
// Async 3 (CE2) config register - r/w
#define C66XX_EMIF16_A3CR_RG_OFFSET							0x18
// Async 4 (CE3) config register - r/w
#define C66XX_EMIF16_A4CR_RG_OFFSET							0x1c
// Interrupt raw register - r/w
#define C66XX_EMIF16_IRR_RG_OFFSET							0x40
// Interrupt masked register - r/w
#define C66XX_EMIF16_IMR_RG_OFFSET							0x44
// Interrupt mask set register - r/w
#define C66XX_EMIF16_IMSR_RG_OFFSET							0x48
// Interrupt mask clear register - r/w
#define C66XX_EMIF16_IMCR_RG_OFFSET							0x4c
// Page mode control register - r/w
#define C66XX_EMIF16_PMCR_RG_OFFSET							0x68

#define C66XX_EMIF16_RCSR_RG_ADDR							(C66XX_EMIF16_RG_AREA_SADDR + C66XX_EMIF16_RCSR_RG_OFFSET)
#define C66XX_EMIF16_AWCCR_RG_ADDR							(C66XX_EMIF16_RG_AREA_SADDR + C66XX_EMIF16_AWCCR_RG_OFFSET)
#define C66XX_EMIF16_A1CR_RG_ADDR							(C66XX_EMIF16_RG_AREA_SADDR + C66XX_EMIF16_A1CR_RG_OFFSET)
#define C66XX_EMIF16_A2CR_RG_ADDR							(C66XX_EMIF16_RG_AREA_SADDR + C66XX_EMIF16_A2CR_RG_OFFSET)
#define C66XX_EMIF16_A3CR_RG_ADDR							(C66XX_EMIF16_RG_AREA_SADDR + C66XX_EMIF16_A3CR_RG_OFFSET)
#define C66XX_EMIF16_A4CR_RG_ADDR							(C66XX_EMIF16_RG_AREA_SADDR + C66XX_EMIF16_A4CR_RG_OFFSET)
#define C66XX_EMIF16_IRR_RG_ADDR							(C66XX_EMIF16_RG_AREA_SADDR + C66XX_EMIF16_IRR_RG_OFFSET)
#define C66XX_EMIF16_IMR_RG_ADDR							(C66XX_EMIF16_RG_AREA_SADDR + C66XX_EMIF16_IMR_RG_OFFSET)
#define C66XX_EMIF16_IMSR_RG_ADDR							(C66XX_EMIF16_RG_AREA_SADDR + C66XX_EMIF16_IMSR_RG_OFFSET)
#define C66XX_EMIF16_IMCR_RG_ADDR							(C66XX_EMIF16_RG_AREA_SADDR + C66XX_EMIF16_IMCR_RG_OFFSET)
#define C66XX_EMIF16_PMCR_RG_ADDR							(C66XX_EMIF16_RG_AREA_SADDR + C66XX_EMIF16_PMCR_RG_OFFSET)


//------------ Revision code and status register defs -------------------------
#define C66XX_EMIF16_RCSR_BE_BITMASK						0x80000000
#define C66XX_EMIF16_RCSR_BE_BITSHIFT						31
#define C66XX_EMIF16_RCSR_MOD_ID_BITMASK					0x3fff0000
#define C66XX_EMIF16_RCSR_MOD_ID_BITSHIFT					16
#define C66XX_EMIF16_RCSR_MJ_REV_BITMASK					0xff00
#define C66XX_EMIF16_RCSR_MJ_REV_BITSHIFT					8
#define C66XX_EMIF16_RCSR_MIN_REV_BITMASK					0xff
#define C66XX_EMIF16_RCSR_MIN_REV_BITSHIFT					0

#define C66XX_EMIF16_RCSR_MOD_ID_DEFAULT_VALUE				0x46
#define C66XX_EMIF16_RCSR_MJ_REV_DEFAULT_VALUE				0x4
#define C66XX_EMIF16_RCSR_MIN_REV_DEFAULT_VALUE				0x0


//------------ Async wait cycle config register defs --------------------------
#define C66XX_EMIF16_AWCCR_WP1_BITMASK						0x20000000
#define C66XX_EMIF16_AWCCR_WP1_BITSHIFT						29
#define C66XX_EMIF16_AWCCR_WP0_BITMASK						0x10000000
#define C66XX_EMIF16_AWCCR_WP0_BITSHIFT						28
#define C66XX_EMIF16_AWCCR_CS5_WAIT_BITMASK					0xc00000
#define C66XX_EMIF16_AWCCR_CS5_WAIT_BITSHIFT				22
#define C66XX_EMIF16_AWCCR_CS4_WAIT_BITMASK					0x300000
#define C66XX_EMIF16_AWCCR_CS4_WAIT_BITSHIFT				20
#define C66XX_EMIF16_AWCCR_CS3_WAIT_BITMASK					0xc0000
#define C66XX_EMIF16_AWCCR_CS3_WAIT_BITSHIFT				18
#define C66XX_EMIF16_AWCCR_CS2_WAIT_BITMASK					0x30000
#define C66XX_EMIF16_AWCCR_CS2_WAIT_BITSHIFT				16
#define C66XX_EMIF16_AWCCR_MAX_EXT_WAIT_BITMASK				0xff
#define C66XX_EMIF16_AWCCR_MAX_EXT_WAIT_BITSHIFT			0


//------------ Async 1 config register defs -----------------------------------
#define C66XX_EMIF16_A1CR_SS_BITMASK						0x80000000
#define C66XX_EMIF16_A1CR_SS_BITSHIFT						31
#define C66XX_EMIF16_A1CR_EW_BITMASK						0x40000000
#define C66XX_EMIF16_A1CR_EW_BITSHIFT						30
#define C66XX_EMIF16_A1CR_W_SETUP_BITMASK					0x3c000000
#define C66XX_EMIF16_A1CR_W_SETUP_BITSHIFT					26
#define C66XX_EMIF16_A1CR_W_STROBE_BITMASK					0x3f00000
#define C66XX_EMIF16_A1CR_W_STROBE_BITSHIFT					20
#define C66XX_EMIF16_A1CR_W_HOLD_BITMASK					0xe0000
#define C66XX_EMIF16_A1CR_W_HOLD_BITSHIFT					17
#define C66XX_EMIF16_A1CR_R_SETUP_BITMASK					0x1e000
#define C66XX_EMIF16_A1CR_R_SETUP_BITSHIFT					13
#define C66XX_EMIF16_A1CR_R_STROBE_BITMASK					0x1f80
#define C66XX_EMIF16_A1CR_R_STROBE_BITSHIFT					7
#define C66XX_EMIF16_A1CR_R_HOLD_BITMASK					0x70
#define C66XX_EMIF16_A1CR_R_HOLD_BITSHIFT					4
#define C66XX_EMIF16_A1CR_TA_BITMASK						0xc
#define C66XX_EMIF16_A1CR_TA_BITSHIFT						2
#define C66XX_EMIF16_A1CR_ASIZE_BITMASK						0x3
#define C66XX_EMIF16_A1CR_ASIZE_BITSHIFT					0

#define C66XX_EMIF16_A1CR_ASIZE_8BIT						0
#define C66XX_EMIF16_A1CR_ASIZE_16BIT						1


//------------ Interrupt raw register defs ------------------------------------
#define C66XX_EMIF16_IRR_WR_BITMASK							0x3c
#define C66XX_EMIF16_IRR_WR_BITSHIFT						2
#define C66XX_EMIF16_IRR_AT_BITMASK							0x1
#define C66XX_EMIF16_IRR_AT_BITSHIFT						0


//------------ Interrupt masked register defs ---------------------------------
#define C66XX_EMIF16_IMR_WR_MASKED_BITMASK					0x3c
#define C66XX_EMIF16_IMR_WR_MASKED_BITSHIFT					2
#define C66XX_EMIF16_IMR_AT_MASKED_BITMASK					0x1
#define C66XX_EMIF16_IMR_AT_MASKED_BITSHIFT					0


//------------ Interrupt mask set register defs -------------------------------
#define C66XX_EMIF16_IMSR_WR_MASK_SET_BITMASK				0x3c
#define C66XX_EMIF16_IMSR_WR_MASK_SET_BITSHIFT				2
#define C66XX_EMIF16_IMSR_AT_MASK_SET_BITMASK				0x1
#define C66XX_EMIF16_IMSR_AT_MASK_SET_BITSHIFT				0


//------------ Interrupt mask clear register defs -----------------------------
#define C66XX_EMIF16_IMCR_WR_MASK_CLR_BITMASK				0x3c
#define C66XX_EMIF16_IMCR_WR_MASK_CLR_BITSHIFT				2
#define C66XX_EMIF16_IMCR_AT_MASK_CLR_BITMASK				0x1
#define C66XX_EMIF16_IMCR_AT_MASK_CLR_BITSHIFT				0

//=============================================================================



//=============================================================================
//============ Timer definitions ==============================================
//=============================================================================

// Emulation Management and Clock Speed register - r-only
#define C66XX_TIMER_EMUMGT_CLKSPD_RG_OFFSET					0x0004
// Counter register low register - r/w
#define C66XX_TIMER_CNTLO_RG_OFFSET							0x0010
// Counter register high register - r/w
#define C66XX_TIMER_CNTHI_RG_OFFSET							0x0014
// Period register low register - r/w
#define C66XX_TIMER_PRDLO_RG_OFFSET							0x0018
// Period register high register - r/w
#define C66XX_TIMER_PRDHI_RG_OFFSET							0x001c
// Timer control register - r/w
#define C66XX_TIMER_TCR_RG_OFFSET							0x0020
// Timer global control register - r/w
#define C66XX_TIMER_TGCR_RG_OFFSET							0x0024
// Watchdog timer control register - r/w
#define C66XX_TIMER_WDTCR_RG_OFFSET							0x0028
// Timer Reload register low register - r/w
#define C66XX_TIMER_RELLO_RG_OFFSET							0x0034
// Timer Reload register high register - r/w
#define C66XX_TIMER_RELHI_RG_OFFSET							0x0038
// Timer Capture register low register - r/w
#define C66XX_TIMER_CAPLO_RG_OFFSET							0x003c
// Timer Capture register high register - r/w
#define C66XX_TIMER_CAPHI_RG_OFFSET							0x0040
// Timer interrupt control and status register - r/w
#define C66XX_TIMER_INTCTLSTAT_RG_OFFSET					0x0044

// Timer 0-15 registers area offset addresses
#define C66XX_TIMER_RG_AREA_OFFSET							0x00010000
// Timer 0-15 registers addresses: timer = 0-15
#define C66XX_TIMER_EMUMGT_CLKSPD_RG_ADDR(timer)			(C66XX_TIMER_0_RG_AREA_SADDR + (timer * C66XX_TIMER_RG_AREA_OFFSET) + C66XX_TIMER_EMUMGT_CLKSPD_RG_OFFSET)
#define C66XX_TIMER_CNTLO_RG_ADDR(timer)					(C66XX_TIMER_0_RG_AREA_SADDR + (timer * C66XX_TIMER_RG_AREA_OFFSET) + C66XX_TIMER_CNTLO_RG_OFFSET)
#define C66XX_TIMER_CNTHI_RG_ADDR(timer)					(C66XX_TIMER_0_RG_AREA_SADDR + (timer * C66XX_TIMER_RG_AREA_OFFSET) + C66XX_TIMER_CNTHI_RG_OFFSET)
#define C66XX_TIMER_PRDLO_RG_ADDR(timer)					(C66XX_TIMER_0_RG_AREA_SADDR + (timer * C66XX_TIMER_RG_AREA_OFFSET) + C66XX_TIMER_PRDLO_RG_OFFSET)
#define C66XX_TIMER_PRDHI_RG_ADDR(timer)					(C66XX_TIMER_0_RG_AREA_SADDR + (timer * C66XX_TIMER_RG_AREA_OFFSET) + C66XX_TIMER_PRDHI_RG_OFFSET)
#define C66XX_TIMER_TCR_RG_ADDR(timer)						(C66XX_TIMER_0_RG_AREA_SADDR + (timer * C66XX_TIMER_RG_AREA_OFFSET) + C66XX_TIMER_TCR_RG_OFFSET)
#define C66XX_TIMER_TGCR_RG_ADDR(timer)						(C66XX_TIMER_0_RG_AREA_SADDR + (timer * C66XX_TIMER_RG_AREA_OFFSET) + C66XX_TIMER_TGCR_RG_OFFSET)
#define C66XX_TIMER_WDTCR_RG_ADDR(timer)					(C66XX_TIMER_0_RG_AREA_SADDR + (timer * C66XX_TIMER_RG_AREA_OFFSET) + C66XX_TIMER_WDTCR_RG_OFFSET)
#define C66XX_TIMER_RELLO_RG_ADDR(timer)					(C66XX_TIMER_0_RG_AREA_SADDR + (timer * C66XX_TIMER_RG_AREA_OFFSET) + C66XX_TIMER_RELLO_RG_OFFSET)
#define C66XX_TIMER_RELHI_RG_ADDR(timer)					(C66XX_TIMER_0_RG_AREA_SADDR + (timer * C66XX_TIMER_RG_AREA_OFFSET) + C66XX_TIMER_RELHI_RG_OFFSET)
#define C66XX_TIMER_CAPLO_RG_ADDR(timer)					(C66XX_TIMER_0_RG_AREA_SADDR + (timer * C66XX_TIMER_RG_AREA_OFFSET) + C66XX_TIMER_CAPLO_RG_OFFSET)
#define C66XX_TIMER_CAPHI_RG_ADDR(timer)					(C66XX_TIMER_0_RG_AREA_SADDR + (timer * C66XX_TIMER_RG_AREA_OFFSET) + C66XX_TIMER_CAPHI_RG_OFFSET)
#define C66XX_TIMER_INTCTLSTAT_RG_ADDR(timer)				(C66XX_TIMER_0_RG_AREA_SADDR + (timer * C66XX_TIMER_RG_AREA_OFFSET) + C66XX_TIMER_INTCTLSTAT_RG_OFFSET)


//------------ Emulation Management and Clock Speed register defs -------------
#define C66XX_TIMER_EMUMGT_CLKSPD_CLKDIV_BITMASK			0x000f0000
#define C66XX_TIMER_EMUMGT_CLKSPD_CLKDIV_BITSHIFT			16


//------------ Timer control register defs ------------------------------------
#define C66XX_TIMER_TCR_READRSTMODE_HI_BITMASK				0x04000000
#define C66XX_TIMER_TCR_READRSTMODE_HI_BITSHIFT				26
#define C66XX_TIMER_TCR_ENAMODE_HI_BITMASK					0x00c00000
#define C66XX_TIMER_TCR_ENAMODE_HI_BITSHIFT					22
#define C66XX_TIMER_TCR_PWID_HI_BITMASK						0x00300000
#define C66XX_TIMER_TCR_PWID_HI_BITSHIFT					20
#define C66XX_TIMER_TCR_CP_HI_BITMASK						0x00080000
#define C66XX_TIMER_TCR_CP_HI_BITSHIFT						19
#define C66XX_TIMER_TCR_INVOUTP_HI_BITMASK					0x00020000
#define C66XX_TIMER_TCR_INVOUTP_HI_BITSHIFT					17
#define C66XX_TIMER_TCR_TSTAT_HI_BITMASK					0x00010000
#define C66XX_TIMER_TCR_TSTAT_HI_BITSHIFT					16
#define C66XX_TIMER_TCR_CAPEVTMODE_LO_BITMASK				0x00003000
#define C66XX_TIMER_TCR_CAPEVTMODE_LO_BITSHIFT				12
#define C66XX_TIMER_TCR_CAPMODE_LO_BITMASK					0x00000800
#define C66XX_TIMER_TCR_CAPMODE_LO_BITSHIFT					11
#define C66XX_TIMER_TCR_READRSTMODE_LO_BITMASK				0x00000400
#define C66XX_TIMER_TCR_READRSTMODE_LO_BITSHIFT				10
#define C66XX_TIMER_TCR_TIEN_LO_BITMASK						0x00000200
#define C66XX_TIMER_TCR_TIEN_LO_BITSHIFT					9
#define C66XX_TIMER_TCR_CLKSRC_LO_BITMASK					0x00000100
#define C66XX_TIMER_TCR_CLKSRC_LO_BITSHIFT					8
#define C66XX_TIMER_TCR_ENAMODE_LO_BITMASK					0x000000c0
#define C66XX_TIMER_TCR_ENAMODE_LO_BITSHIFT					6
#define C66XX_TIMER_TCR_PWID_LO_BITMASK						0x00000030
#define C66XX_TIMER_TCR_PWID_LO_BITSHIFT					4
#define C66XX_TIMER_TCR_CP_LO_BITMASK						0x00000008
#define C66XX_TIMER_TCR_CP_LO_BITSHIFT						3
#define C66XX_TIMER_TCR_INVINP_LO_BITMASK					0x00000004
#define C66XX_TIMER_TCR_INVINP_LO_BITSHIFT					2
#define C66XX_TIMER_TCR_INVOUTP_LO_BITMASK					0x00000002
#define C66XX_TIMER_TCR_INVOUTP_LO_BITSHIFT					1
#define C66XX_TIMER_TCR_TSTAT_LO_BITMASK					0x00000001
#define C66XX_TIMER_TCR_TSTAT_LO_BITSHIFT					0

#define C66XX_TIMER_TCR_ENAMODE_DISABLED					0
#define C66XX_TIMER_TCR_ENAMODE_ONE_SHOT					1
#define C66XX_TIMER_TCR_ENAMODE_CONT						2
#define C66XX_TIMER_TCR_ENAMODE_CONT_RELOAD					3

#define C66XX_TIMER_TCR_PWID_1_CLK							0
#define C66XX_TIMER_TCR_PWID_2_CLK							1
#define C66XX_TIMER_TCR_PWID_3_CLK							2
#define C66XX_TIMER_TCR_PWID_4_CLK							3

#define C66XX_TIMER_TCR_CAPEVTMODE_RISING_EDGE				0
#define C66XX_TIMER_TCR_CAPEVTMODE_FALLING_EDGE				1
#define C66XX_TIMER_TCR_CAPEVTMODE_ANY_EDGE					2


//------------ Timer global control register defs -----------------------------
#define C66XX_TIMER_TGCR_TDDRHI_BITMASK						0x0000f000
#define C66XX_TIMER_TGCR_TDDRHI_BITSHIFT					12
#define C66XX_TIMER_TGCR_PSCHI_BITMASK						0x00000f00
#define C66XX_TIMER_TGCR_PSCHI_BITSHIFT						8
#define C66XX_TIMER_TGCR_PLUSEN_BITMASK						0x00000010
#define C66XX_TIMER_TGCR_PLUSEN_BITSHIFT					4
#define C66XX_TIMER_TGCR_TIMMODE_BITMASK					0x0000000c
#define C66XX_TIMER_TGCR_TIMMODE_BITSHIFT					2
#define C66XX_TIMER_TGCR_TIMHIRS_BITMASK					0x00000002
#define C66XX_TIMER_TGCR_TIMHIRS_BITSHIFT					1
#define C66XX_TIMER_TGCR_TIMLORS_BITMASK					0x00000001
#define C66XX_TIMER_TGCR_TIMLORS_BITSHIFT					0

#define C66XX_TIMER_TGCR_TIMMODE_64BIT_GPT					0
#define C66XX_TIMER_TGCR_TIMMODE_32BIT_UNCHAINED			1
#define C66XX_TIMER_TGCR_TIMMODE_64BIT_WDT					2
#define C66XX_TIMER_TGCR_TIMMODE_32BIT_CHAINED				3


//------------ Watchdog Timer Control Register defs ---------------------------
#define C66XX_TIMER_WDTCR_WDKEY_BITMASK						0xffff0000
#define C66XX_TIMER_WDTCR_WDKEY_BITSHIFT					16
#define C66XX_TIMER_WDTCR_WDFLAG_BITMASK					0x00008000
#define C66XX_TIMER_WDTCR_WDFLAG_BITSHIFT					15
#define C66XX_TIMER_WDTCR_WDEN_BITMASK						0x00004000
#define C66XX_TIMER_WDTCR_WDEN_BITSHIFT						14

#define C66XX_TIMER_WDTCR_WDKEY_FIRST_KEY					0xa5c6
#define C66XX_TIMER_WDTCR_WDKEY_SECOND_KEY					0xda7e


//------------ Timer interrupt control and status register defs ---------------
#define C66XX_TIMER_INTCTLSTAT_EVTINTSTAT_HI_BITMASK		0x00080000
#define C66XX_TIMER_INTCTLSTAT_EVTINTSTAT_HI_BITSHIFT		19
#define C66XX_TIMER_INTCTLSTAT_EVTINTEN_HI_BITMASK			0x00040000
#define C66XX_TIMER_INTCTLSTAT_EVTINTEN_HI_BITSHIFT			18
#define C66XX_TIMER_INTCTLSTAT_PRDINTSTAT_HI_BITMASK		0x00020000
#define C66XX_TIMER_INTCTLSTAT_PRDINTSTAT_HI_BITSHIFT		17
#define C66XX_TIMER_INTCTLSTAT_PRDINTEN_HI_BITMASK			0x00010000
#define C66XX_TIMER_INTCTLSTAT_PRDINTEN_HI_BITSHIFT			16
#define C66XX_TIMER_INTCTLSTAT_EVTINTSTAT_LO_BITMASK		0x00000008
#define C66XX_TIMER_INTCTLSTAT_EVTINTSTAT_LO_BITSHIFT		3
#define C66XX_TIMER_INTCTLSTAT_EVTINTEN_LO_BITMASK			0x00000004
#define C66XX_TIMER_INTCTLSTAT_EVTINTEN_LO_BITSHIFT			2
#define C66XX_TIMER_INTCTLSTAT_PRDINTSTAT_LO_BITMASK		0x00000002
#define C66XX_TIMER_INTCTLSTAT_PRDINTSTAT_LO_BITSHIFT		1
#define C66XX_TIMER_INTCTLSTAT_PRDINTEN_LO_BITMASK			0x00000001
#define C66XX_TIMER_INTCTLSTAT_PRDINTEN_LO_BITSHIFT			0

//=============================================================================



//=============================================================================
#endif /* __C66XX_DEF_HXX__ */
