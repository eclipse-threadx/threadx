/*
 * board_setup.c
 *
 */


#include "board_setup.h"
#include "C66XX.h"
#include <string.h>


//=============================================================================
//============ General definitions ============================================
//=============================================================================
// DSP Timer-8 definition
#define C66XX_DSP_TIMER										C66XX_TIMER_8
// DSP Timer-8 output frequency in Hz
#define C66XX_DSP_TIMER_FREQ								100
/* DSP Timer-8 interrupt event ID */
#define C66XX_DSP_TIMER_EVENT_ID							67
//=============================================================================


//=============================================================================
//============ Global functions ===============================================
//=============================================================================
void _tx_nmi_vector(void);
void _tx_int4_vector(void);
void _tx_int5_vector(void);
void _tx_int6_vector(void);
void _tx_int7_vector(void);
void _tx_int8_vector(void);
void _tx_int9_vector(void);
void _tx_int10_vector(void);
void _tx_int11_vector(void);
void _tx_int12_vector(void);
void _tx_int13_vector(void);
void _tx_int14_vector(void);
void _tx_int15_vector(void);
//=============================================================================


//=============================================================================
//============ Static functions ===============================================
//=============================================================================
static int32_t tx_timer_init(C66XX_TIMER timer, uint32_t frequency);
static int32_t tx_interrupt_init(void);
//=============================================================================


/*------------ init_output_timer() function -----------------------------------
 * DESCRIPTION: Function initializes Timer64 module
 * ARGUMENTS:
 * None
 * RETURNED VALUE: Error code
-----------------------------------------------------------------------------*/
static int32_t tx_timer_init(C66XX_TIMER timer, uint32_t frequency)
{
	int32_t r;
	C66XX_TIMER_CFG_DD cfg_dd;

	// Reset 64-bit timer
	if ((r = C66XX_TIMER_reset(timer, C66XX_TIMER_HW_CFG_64BIT)) != C66XX_OK)
		goto exit;

	// Fill configuration data descriptor
	memset(&cfg_dd, 0, C66XX_TIMER_CFG_DD_LEN);
	cfg_dd.timer_mode = C66XX_TIMER_MODE_32BIT_UNCHAINED;
	cfg_dd.timer_high.clk_src_output_mode = C66XX_TIMER_CLK_OUTPUT_MODE_CLK;
	// Init DSP Timer64 module
	if ((r = C66XX_TIMER_init(timer, frequency, &cfg_dd)) != C66XX_OK)
		goto exit;

	// Enable timer interrupt
	if ((r = C66XX_TIMER_enable_interrupts(timer, C66XX_TIMER_HW_CFG_32BIT_HIGH)) != C66XX_OK)
		goto exit;

	// Start 32-bit timer high to enable continuously
	if ((r = C66XX_TIMER_start(timer, C66XX_TIMER_HW_CFG_32BIT_HIGH, C66XX_TIMER_COUNT_MODE_CONTINUOUSLY)) != C66XX_OK)
		goto exit;

	// TIMER module configuration is completed
	printf("Timer #%u configuration is completed\n", timer);

exit:
	return (r);
}
//-----------------------------------------------------------------------------


/*------------ tx_interrupt_init() function -----------------------------------
 * DESCRIPTION: Function initializes CorePack interrupt module
 * ARGUMENTS:
 * None
 * RETURNED VALUE: Error code
-----------------------------------------------------------------------------*/
static int32_t tx_interrupt_init(void)
{
	int32_t r;

	// Set DSP interrupt handlers to the ones defined in tx_initialize_low_level.asm
	if ((r = C66XX_INT_set_core_dsp_interrupt_handler(C66XX_DSP_VECTID_NMI, _tx_nmi_vector)) != C66XX_OK)
		goto exit;
	if ((r = C66XX_INT_set_core_dsp_interrupt_handler(C66XX_DSP_VECTID_4, _tx_int4_vector)) != C66XX_OK)
		goto exit;
	if ((r = C66XX_INT_set_core_dsp_interrupt_handler(C66XX_DSP_VECTID_5, _tx_int5_vector)) != C66XX_OK)
		goto exit;
	if ((r = C66XX_INT_set_core_dsp_interrupt_handler(C66XX_DSP_VECTID_6, _tx_int6_vector)) != C66XX_OK)
		goto exit;
	if ((r = C66XX_INT_set_core_dsp_interrupt_handler(C66XX_DSP_VECTID_7, _tx_int7_vector)) != C66XX_OK)
		goto exit;
	if ((r = C66XX_INT_set_core_dsp_interrupt_handler(C66XX_DSP_VECTID_8, _tx_int8_vector)) != C66XX_OK)
		goto exit;
	if ((r = C66XX_INT_set_core_dsp_interrupt_handler(C66XX_DSP_VECTID_9, _tx_int9_vector)) != C66XX_OK)
		goto exit;
	if ((r = C66XX_INT_set_core_dsp_interrupt_handler(C66XX_DSP_VECTID_10, _tx_int10_vector)) != C66XX_OK)
		goto exit;
	if ((r = C66XX_INT_set_core_dsp_interrupt_handler(C66XX_DSP_VECTID_11, _tx_int11_vector)) != C66XX_OK)
		goto exit;
	if ((r = C66XX_INT_set_core_dsp_interrupt_handler(C66XX_DSP_VECTID_12, _tx_int12_vector)) != C66XX_OK)
		goto exit;
	if ((r = C66XX_INT_set_core_dsp_interrupt_handler(C66XX_DSP_VECTID_13, _tx_int13_vector)) != C66XX_OK)
		goto exit;
	if ((r = C66XX_INT_set_core_dsp_interrupt_handler(C66XX_DSP_VECTID_14, _tx_int14_vector)) != C66XX_OK)
		goto exit;
	if ((r = C66XX_INT_set_core_dsp_interrupt_handler(C66XX_DSP_VECTID_15, _tx_int15_vector)) != C66XX_OK)
		goto exit;

	/* CorePack interrupt module configuration is completed */
	printf("INTC configuration is completed\n");
	// Exit without errors
	r = C66XX_OK;

exit:
	return (r);
}
//-----------------------------------------------------------------------------


/*------------ hardware_setup() function --------------------------------------
 * DESCRIPTION: Function intializes board hardware
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
int hardware_setup()
{
	platform_init_flags init_flags;
	platform_init_config init_config;
	platform_info p_info;
	int32_t r;

	/*
	 * Initialize all platform peripherals with default values:
	 * PLL, DDR, TCSL, PHY, ECC
	 */
	init_flags.pll = 1;
	init_flags.ddr = 1;
	init_flags.tcsl = 1;
	init_flags.phy = 1;
	init_flags.ecc = 0;
	memset(&init_config, 0, sizeof(platform_init_config));
	if ((r = platform_init(&init_flags, &init_config)) != Platform_EOK)
		goto exit;

	/* Initialize platform UART */
	if ((r = platform_uart_init()) != Platform_EOK)
		goto exit;
	if ((r = platform_uart_set_baudrate(115200)) != Platform_EOK)
		goto exit;

	/* Get platform info */
	platform_get_info(&p_info);
	/* Write data to the UART */
	platform_write("Platform library version is %s\n", p_info.version);
	platform_write("Board name is %s\n", p_info.board_name);
	platform_write("Board serial number is %s\n", p_info.serial_nbr);
	platform_write("Board revision ID is %u\n", p_info.board_rev);
	platform_write("CPU name is %s\n", p_info.cpu.name);
	platform_write("CPU revision ID is %u\n", p_info.cpu.revision_id);
	platform_write("Number of CPU cores is %u\n", p_info.cpu.core_count);
	platform_write("CPU frequency is %u MHz\n", p_info.frequency);

	// Init CorePac INTC
	if ((r = C66XX_INT_init_core()) != C66XX_OK)
		goto exit;

	// Init DSP Timer
	if ((r = tx_timer_init(C66XX_DSP_TIMER, C66XX_DSP_TIMER_FREQ)) != C66XX_OK)
		goto exit;

	// Init DSP interrupt controller
	if ((r = tx_interrupt_init()) != C66XX_OK)
		goto exit;

	printf("Board is initialized\n");
	/* Exit with no errors */

exit:
	return (r);
}
//-----------------------------------------------------------------------------


/*------------ tx_nmi_handler() function --------------------------------------
 * DESCRIPTION: Function handles NMI interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_nmi_handler(void)
{
}
//-----------------------------------------------------------------------------


/*------------ tx_int4_handler() function -------------------------------------
 * DESCRIPTION: Function handles INT4 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int4_handler(void)
{
}
//-----------------------------------------------------------------------------


/*------------ tx_int5_handler() function -------------------------------------
 * DESCRIPTION: Function handles INT5 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int5_handler(void)
{
}
//-----------------------------------------------------------------------------


/*------------ tx_int6_handler() function -------------------------------------
 * DESCRIPTION: Function handles INT6 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int6_handler(void)
{
}
//-----------------------------------------------------------------------------


/*------------ tx_int7_handler() function -------------------------------------
 * DESCRIPTION: Function handles INT7 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int7_handler(void)
{
}
//-----------------------------------------------------------------------------


/*------------ tx_int8_handler() function -------------------------------------
 * DESCRIPTION: Function handles INT8 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int8_handler(void)
{
}
//-----------------------------------------------------------------------------


/*------------ tx_int9_handler() function -------------------------------------
 * DESCRIPTION: Function handles INT9 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int9_handler(void)
{
}
//-----------------------------------------------------------------------------


/*------------ tx_int10_handler() function ------------------------------------
 * DESCRIPTION: Function handles INT10 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int10_handler(void)
{
}
//-----------------------------------------------------------------------------


/*------------ tx_int11_handler() function ------------------------------------
 * DESCRIPTION: Function handles INT11 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int11_handler(void)
{
}
//-----------------------------------------------------------------------------


/*------------ tx_int12_handler() function ------------------------------------
 * DESCRIPTION: Function handles INT12 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int12_handler(void)
{
}
//-----------------------------------------------------------------------------


/*------------ tx_int13_handler() function ------------------------------------
 * DESCRIPTION: Function handles INT13 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int13_handler(void)
{
}
//-----------------------------------------------------------------------------


/*------------ tx_int14_handler() function ------------------------------------
 * DESCRIPTION: Function handles INT14 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int14_handler(void)
{
}
//-----------------------------------------------------------------------------


/*------------ tx_int15_handler() function ------------------------------------
 * DESCRIPTION: Function handles INT15 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int15_handler(void)
{
}
//-----------------------------------------------------------------------------


//=============================================================================
//============ Platform OSAL functions ========================================
//=============================================================================

/*------------ Osal_platformMalloc() function ----------------------------*//**
 * @brief  Function implements the memory allocate function for the platform
 * library.
 *
 * This function allocates a memory block of a given size specified by input
 * parameter 'num_bytes'.
 *
 * @param[in]  num_bytes - number of bytes to be allocated
 * @param[in]  alignment - alignment of allocated memory block in bytes
 *
 * @return  Allocated block address
 *
-----------------------------------------------------------------------------*/
uint8_t *Osal_platformMalloc(uint32_t num_bytes, uint32_t alignment)
{
	// Allocate memory from default system heap
	return (NULL);
}
//-----------------------------------------------------------------------------


/*------------ Osal_platformFree() function ------------------------------*//**
 * @brief  Function implements the memory free function for the platform
 * library.
 *
 * This function frees up memory allocated using Osal_platformMalloc()
 * function call.
 *
 * @param[in]  mem_ptr - pointer to the memory block to be cleaned up
 * @param[in]  num_bytes - size of the memory block to be cleaned up in bytes
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_platformFree(uint8_t *mem_ptr, uint32_t num_bytes)
{
}
//-----------------------------------------------------------------------------


/*------------ Osal_platformSpiCsEnter() function ------------------------*//**
 * @brief  Function is used to enter a critical section
 *
 * Function protects against access from multiple cores and access from
 * multiple threads on single core
 *
 * @param[out]  key - pointer to a variable to receive a handle for unlocking
 *				critical section

 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_platformSpiCsEnter(void)
{
}
//-----------------------------------------------------------------------------


/*------------ Osal_platformSpiCsExit() function -------------------------*//**
 * @brief  Function is used to exit a critical section protected using
 * Osal_paMtCsEnter() API.
 *
 * @param[in]  key - handle for unlocking critical section
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_platformSpiCsExit(void)
{
}
//-----------------------------------------------------------------------------


//=============================================================================



