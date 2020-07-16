/*
 * board_setup.c
 *
 */


#include "board_setup.h"
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
	r = TASDK_OK;

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
	int r;

	/* Init DSP */
	if ((r = TA66XX_BC_init()) != TASDK_OK)
		goto exit;

	// Initialize UART
	if ((r = TA66XX_BC_init_uart(C66XX_UART_BAUD_RATE_115200, C66XX_UART_DATA_BITS_8BITS, C66XX_UART_PARITY_NONE, C66XX_UART_STOP_BITS_1BIT)) != TASDK_OK)
		goto exit;

	// Init DSP Timer
	if ((r = tx_timer_init(C66XX_DSP_TIMER, C66XX_DSP_TIMER_FREQ)) != TASDK_OK)
		goto exit;

	// Init DSP interrupt controller
	if ((r = tx_interrupt_init()) != TASDK_OK)
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


