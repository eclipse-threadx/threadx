/******************************************************************************
	TORNADO AMC modules Software Development Kit (SDK). Rev 3B.
	(C) MicroLAB Systems, 2014-2017

	File:	TORNADO AMC SDK functions for DSP environment declarations
	-----

	Notes:
	------
	1.	This C-header file contains TORNADO AMC modules SDK functions 
		for DSP environment declarations and is an include file for
		TI C6xxx C/C++ Code Generation Tools, which must be invoked to compile
		for TORNADO AMC platform.

	2.	This file is best viewed with the TAB setting set to '4'.

******************************************************************************/


/**
 * @file  TA66XX_DSP_BC_FUNCTIONS.hxx
 *
 * @brief  Board control (BC) SDK functions declarations
 *
 * This file contains declarations of TORNADO AMC modules SDK functions for
 * board control
 *
 */


#ifndef __TA66XX_DSP_BC_FUNCTIONS_HXX__						// check for this file has been already included
#define __TA66XX_DSP_BC_FUNCTIONS_HXX__						1



#ifdef __cplusplus
extern "C" {
#endif



//=============================================================================
//============ DSP environment functions ======================================
//=============================================================================

/** @addtogroup  TA66XX_BC_INIT  TORNADO AMC DSP environment init functions
 *  @{
 */


/** Default DSP interrupt vector for DSP I2C interrupt */
#define TA66XX_BC_I2C_INTERRUPT_NUMBER_USED					4


/*------------ TA66XX_BC_set_i2c_interrupt_number() function -------------*//**
 * @brief  Function should be called before calling TA66XX_BC_init() function
 * and is used to select DSP interrupt vector number for DSP I2C interrupt.
 *
 * @param[in]  dsp_vector_id - DSP interrupt vector number, which will be used
 *				for DSP I2C interrupt.
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_set_i2c_interrupt_number(uint32_t dsp_vector_id);


/*------------ TA66XX_BC_get_i2c_interrupt_number() function -------------*//**
 * @brief  Function returns current DSP interrupt vector number for DSP I2C
 * interrupt.
 *
 * @return  DSP interrupt vector number, which is used for DSP I2C interrupt.
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_i2c_interrupt_number(void);


/** Default DSP interrupt vector for DSP UART interrupt */
#define TA66XX_BC_UART_INTERRUPT_NUMBER_USED				5


/*------------ TA66XX_BC_set_uart_interrupt_number() function ------------*//**
 * @brief  Function should be called before calling TA66XX_BC_init() function
 * and is used to select DSP interrupt vector number for DSP UART interrupt.
 *
 * @param[in]  dsp_vector_id - DSP interrupt vector number, which will be used
 *				for DSP UART interrupt.
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_set_uart_interrupt_number(uint32_t dsp_vector_id);


/*------------ TA66XX_BC_get_uart_interrupt_number() function ------------*//**
 * @brief  Function returns current DSP interrupt vector number for DSP UART
 * interrupt.
 *
 * @return  DSP interrupt vector number, which is used for DSP UART interrupt.
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_uart_interrupt_number(void);


/*------------ TA66XX_BC_init() function ---------------------------------*//**
 * @brief  Function performs initialization of TORNADO AMC modules
 * DSP environment in accordance with board type
 *
 * The function does the following:
 * 1. DSP on-chip caches are configured by default: L1P and L1D caches are
 *    enabled and set to max available size, L2 cache is disabled so L2 memory
 *    acts as SRAM.
 * 2. DSP main PLL, DDR3 PLL and PASS PLL are configured by default: DSP core
 *    clock is set to TA66XX_BC_DSP_CORE_SPEED_VALUE, DDR3 and PASS controllers
 *    to default values
 * 3. DDR3 memory is configured and ready to use
 * 4. All DSP power domains are enabled
 * 5. DSP EMIF-16 bus is configured.
 * 6. In case external 1GE interface is installed then DSP SGMII port is
 *    initialized.
 * 7. DSP GPIO pins are configured as outputs and cleared.
 * 8. DSP LEDs are enabled.
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_init(void);


/*------------ TA66XX_BC_get_ddr_area_base_addr() function ---------------*//**
 * @brief  Function returns DDR3 area base address
 *
 * @return  DDR3 area base address
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_ddr_area_base_addr(void);


/*------------ TA66XX_BC_get_ddr_area_length() function ------------------*//**
 * @brief  Function returns DDR3 area length in bytes
 *
 * @return  DDR3 area length in bytes
 *
-----------------------------------------------------------------------------*/
uint64_t TA66XX_BC_get_ddr_area_length(void);


//-----------------------------------------------------------------------------
// These functions are used to map either lower 2 GB DDR3 page or high 2 GB
// DDR3 page of 4GB DDR3 memory area to DSP 32-bit logical memory map.

/** The lower 2GB DDR3 memory page */
#define TA66XX_BC_DDR3_MEM_PAGE0							0
/** The high 2GB DDR3 memory page */
#define TA66XX_BC_DDR3_MEM_PAGE1							1
//-----------------------------------------------------------------------------

/*------------ TA66XX_BC_set_ddr_area_page() function --------------------*//**
 * @brief  Function sets the specified 2 GB DDR3 memory page (0 or 1) to be
 * mapped to DSP 32-bit logical memory map.
 *
 * This function should be used only in case of 4 GB and more DDR3 memory is
 * installed otherwise it does nothing.
 *
 * @param[in]  page - 2 GB DDR3 memory page (0 or 1);
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_set_ddr_area_page(uint32_t page);


/*------------ TA66XX_BC_get_ddr_area_page() function --------------------*//**
 * @brief  Function returns current selected 2 GB DDR3 memory page (0 or 1)
 * which is mapped to DSP 32-bit logical memory map.
 *
 * @return  Current selected 2 GB DDR3 memory page (0 or 1)
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_ddr_area_page(void);


/** @}*/
//=============================================================================



//=============================================================================
//============ DSP LED functions ==============================================
//=============================================================================

/** @addtogroup  TA66XX_BC_DSP_LED  TORNADO AMC DSP LED functions
 *  @{
 */

/** DSP LED definitions */
enum TA66XX_BC_DSP_LEDS
{
	TA66XX_BC_DSP_LED0 = 0,						/**< DSP LED-0 */
	TA66XX_BC_DSP_LED1,							/**< DSP LED-1 */
	TA66XX_BC_DSP_LED2,							/**< DSP LED-2 */
	TA66XX_BC_DSP_LED3,							/**< DSP LED-3 */
	TA66XX_BC_DSP_LED4,							/**< DSP LED-4 */
	TA66XX_BC_DSP_LED5,							/**< DSP LED-5 */
	TA66XX_BC_DSP_LED6,							/**< DSP LED-6 */
	TA66XX_BC_DSP_LED7							/**< DSP LED-7 */
};

/** DSP LED bitmask definitions */
enum TA66XX_BC_DSP_LED_BITMASKS
{
	TA66XX_BC_DSP_LED0_BITMASK = 0x0001,		/**< DSP LED-0 bitmask */
	TA66XX_BC_DSP_LED1_BITMASK = 0x0002,		/**< DSP LED-1 bitmask */
	TA66XX_BC_DSP_LED2_BITMASK = 0x0004,		/**< DSP LED-2 bitmask */
	TA66XX_BC_DSP_LED3_BITMASK = 0x0008,		/**< DSP LED-3 bitmask */
	TA66XX_BC_DSP_LED4_BITMASK = 0x0010,		/**< DSP LED-4 bitmask */
	TA66XX_BC_DSP_LED5_BITMASK = 0x0020,		/**< DSP LED-5 bitmask */
	TA66XX_BC_DSP_LED6_BITMASK = 0x0040,		/**< DSP LED-6 bitmask */
	TA66XX_BC_DSP_LED7_BITMASK = 0x0080,		/**< DSP LED-7 bitmask */
	TA66XX_BC_DSP_LEDS_BITMASK = 0x00ff			/**< DSP LEDs bitmask */
};

// Definitions for DSP LEDs used for TORNADO-A6678/FMC rev.1A and later boards
/** DSP LED GREEN-0 */
#define TA66XX_BC_DSP_LED_GREEN0							TA66XX_BC_DSP_LED0
#define TA66XX_BC_DSP_LED_GREEN0_BITMASK					TA66XX_BC_DSP_LED0_BITMASK
/** DSP LED RED-0 */
#define TA66XX_BC_DSP_LED_RED0								TA66XX_BC_DSP_LED1
#define TA66XX_BC_DSP_LED_RED0_BITMASK						TA66XX_BC_DSP_LED1_BITMASK
/** DSP LED GREEN-1 */
#define TA66XX_BC_DSP_LED_GREEN1							TA66XX_BC_DSP_LED2
#define TA66XX_BC_DSP_LED_GREEN1_BITMASK					TA66XX_BC_DSP_LED2_BITMASK
/** DSP LED RED-1 */
#define TA66XX_BC_DSP_LED_RED1								TA66XX_BC_DSP_LED3
#define TA66XX_BC_DSP_LED_RED1_BITMASK						TA66XX_BC_DSP_LED3_BITMASK
/** DSP LED GREEN-2 */
#define TA66XX_BC_DSP_LED_GREEN2							TA66XX_BC_DSP_LED4
#define TA66XX_BC_DSP_LED_GREEN2_BITMASK					TA66XX_BC_DSP_LED4_BITMASK
/** DSP LED YELLOW-0 */
#define TA66XX_BC_DSP_LED_YELLOW0							TA66XX_BC_DSP_LED5
#define TA66XX_BC_DSP_LED_YELLOW0_BITMASK					TA66XX_BC_DSP_LED5_BITMASK
/** DSP LED YELLOW-1 */
#define TA66XX_BC_DSP_LED_YELLOW1							TA66XX_BC_DSP_LED6
#define TA66XX_BC_DSP_LED_YELLOW1_BITMASK					TA66XX_BC_DSP_LED6_BITMASK
/** DSP LED RED-2 */
#define TA66XX_BC_DSP_LED_RED2								TA66XX_BC_DSP_LED7
#define TA66XX_BC_DSP_LED_RED2_BITMASK						TA66XX_BC_DSP_LED7_BITMASK

/** DSP LED aliases for compatibility with TORNADO-A6678 rev.1B-1 board definitions */
#define TA66XX_BC_DSP_LED_RED								TA66XX_BC_DSP_LED_RED0
#define TA66XX_BC_DSP_LED_AMBER								TA66XX_BC_DSP_LED_YELLOW0
#define TA66XX_BC_DSP_LED_BLUE								TA66XX_BC_DSP_LED_GREEN2
#define TA66XX_BC_DSP_LED_WHITE								TA66XX_BC_DSP_LED_RED2


// Aliases for DSP LEDs used for TORNADO-A6678 rev.1B-1 board
/** DSP LED GREEN-0 */
#define TA6678_REV1B_BC_DSP_LED_GREEN0						TA66XX_BC_DSP_LED0
#define TA6678_REV1B_BC_DSP_LED_GREEN0_BITMASK				TA66XX_BC_DSP_LED0_BITMASK
/** DSP LED GREEN-1 */
#define TA6678_REV1B_BC_DSP_LED_GREEN1						TA66XX_BC_DSP_LED1
#define TA6678_REV1B_BC_DSP_LED_GREEN1_BITMASK				TA66XX_BC_DSP_LED1_BITMASK
/** DSP LED YELLOW-0 */
#define TA6678_REV1B_BC_DSP_LED_YELLOW0						TA66XX_BC_DSP_LED2
#define TA6678_REV1B_BC_DSP_LED_YELLOW0_BITMASK				TA66XX_BC_DSP_LED2_BITMASK
/** DSP LED YELLOW-1 */
#define TA6678_REV1B_BC_DSP_LED_YELLOW1						TA66XX_BC_DSP_LED3
#define TA6678_REV1B_BC_DSP_LED_YELLOW1_BITMASK				TA66XX_BC_DSP_LED3_BITMASK
/** DSP LED AMBER */
#define TA6678_REV1B_BC_DSP_LED_AMBER						TA66XX_BC_DSP_LED4
#define TA6678_REV1B_BC_DSP_LED_AMBER_BITMASK				TA66XX_BC_DSP_LED4_BITMASK
/** DSP LED RED */
#define TA6678_REV1B_BC_DSP_LED_RED							TA66XX_BC_DSP_LED5
#define TA6678_REV1B_BC_DSP_LED_RED_BITMASK					TA66XX_BC_DSP_LED5_BITMASK
/** DSP LED BLUE */
#define TA6678_REV1B_BC_DSP_LED_BLUE						TA66XX_BC_DSP_LED6
#define TA6678_REV1B_BC_DSP_LED_BLUE_BITMASK				TA66XX_BC_DSP_LED6_BITMASK
/** DSP LED WHITE */
#define TA6678_REV1B_BC_DSP_LED_WHITE						TA66XX_BC_DSP_LED7
#define TA6678_REV1B_BC_DSP_LED_WHITE_BITMASK				TA66XX_BC_DSP_LED7_BITMASK



/*------------ TA66XX_BC_set_dsp_leds_enable() function ------------------*//**
 * @brief  Function enables or disables on-board DSP LEDs
 *
 * @param[in]  state - DSP LEDs enable state: TA66XX_ON or TA66XX_OFF;
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_set_dsp_leds_enable(uint32_t enable);


/*------------ TA66XX_BC_get_dsp_leds_enable_status() function -----------*//**
 * @brief  Function returns enable status of on-board DSP LEDs
 *
 * @return  DSP LEDs enable state: TA66XX_ON or TA66XX_OFF
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_dsp_leds_enable_status(void);


/*------------ TA66XX_BC_set_dsp_led_state() function --------------------*//**
 * @brief  Function sets selected DSP LED to the desired state: ON or OFF
 *
 * @param[in]  led_number - DSP LED number definition;
 * @param[in]  state - DSP LED state: TA66XX_ON or TA66XX_OFF;
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_set_dsp_led_state(uint32_t led_number, uint32_t state);


/*------------ TA66XX_BC_get_dsp_led_state() function --------------------*//**
 * @brief  Function returns selected DSP LED state: ON or OFF
 *
 * @param[in]  led_number - DSP LED number definition;
 *
 * @return  DSP LED state: TA66XX_ON or TA66XX_OFF
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_dsp_led_state(uint32_t led_number);


/*------------ TA66XX_BC_set_dsp_leds() function -------------------------*//**
 * @brief  Function sets selected DSP LEDs to the desired state: ON or OFF
 *
 * @param[in]  led_bitmask - DSP LEDs (ORed) bitmask;
 * @param[in]  state - DSP LEDs state: TA66XX_ON or TA66XX_OFF;
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_set_dsp_leds(uint32_t led_bitmask, uint32_t state);


/*------------ TA66XX_BC_get_dsp_leds() function -------------------------*//**
 * @brief  Function returns selected DSP LEDs state: ON or OFF
 *
 * @param[in]  led_bitmask - DSP LEDs (ORed) bitmask;
 *
 * @return  DSP LEDs state
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_dsp_leds(uint32_t led_bitmask);


/** @}*/
//=============================================================================



//=============================================================================
//============ DSP-to-FPGA GPIO functions =====================================
//=============================================================================

/** @addtogroup  TA66XX_BC_DSP_FPGA_GPIO  TORNADO AMC DSP-to-FPGA GPIO functions
 *  @{
 */

/** DSP-to-FPGA GPIO pins definitions */
enum TA66XX_BC_DSP_FPGA_GPIO
{
	TA66XX_BC_DSP_FPGA_GPIO0 = 0,					/**< DSP-to-FPGA GPIO pin 0 */
	TA66XX_BC_DSP_FPGA_GPIO1,						/**< DSP-to-FPGA GPIO pin 1 */
	TA66XX_BC_DSP_FPGA_GPIO2,						/**< DSP-to-FPGA GPIO pin 2 */
	TA66XX_BC_DSP_FPGA_GPIO3,						/**< DSP-to-FPGA GPIO pin 3 */
	TA66XX_BC_DSP_FPGA_GPIO4,						/**< DSP-to-FPGA GPIO pin 4 */
	TA66XX_BC_DSP_FPGA_GPIO5,						/**< DSP-to-FPGA GPIO pin 5 */
	TA66XX_BC_DSP_FPGA_GPIO6,						/**< DSP-to-FPGA GPIO pin 6 */
	TA66XX_BC_DSP_FPGA_GPIO7						/**< DSP-to-FPGA GPIO pin 7 */
};

/** DSP-to-FPGA GPIO pins bitmask definitions */
enum TA66XX_BC_DSP_FPGA_GPIO_BITMASKS
{
	TA66XX_BC_DSP_FPGA_GPIO0_BITMASK = 0x0001,		/**< DSP-to-FPGA GPIO pin 0 bitmask */
	TA66XX_BC_DSP_FPGA_GPIO1_BITMASK = 0x0002,		/**< DSP-to-FPGA GPIO pin 1 bitmask */
	TA66XX_BC_DSP_FPGA_GPIO2_BITMASK = 0x0004,		/**< DSP-to-FPGA GPIO pin 2 bitmask */
	TA66XX_BC_DSP_FPGA_GPIO3_BITMASK = 0x0008,		/**< DSP-to-FPGA GPIO pin 3 bitmask */
	TA66XX_BC_DSP_FPGA_GPIO4_BITMASK = 0x0010,		/**< DSP-to-FPGA GPIO pin 4 bitmask */
	TA66XX_BC_DSP_FPGA_GPIO5_BITMASK = 0x0020,		/**< DSP-to-FPGA GPIO pin 5 bitmask */
	TA66XX_BC_DSP_FPGA_GPIO6_BITMASK = 0x0040,		/**< DSP-to-FPGA GPIO pin 6 bitmask */
	TA66XX_BC_DSP_FPGA_GPIO7_BITMASK = 0x0080,		/**< DSP-to-FPGA GPIO pin 7 bitmask */
	TA66XX_BC_DSP_FPGA_GPIO_BITMASK = 0x00ff		/**< DSP-to-FPGA GPIO pins bitmask */
};


/** DSP-to-FPGA GPIO pins direction definitions */
#define TA66XX_BC_DSP_FPGA_GPIO_DIR_OUT						C66XX_GPIO_DIR_OUT
#define TA66XX_BC_DSP_FPGA_GPIO_DIR_IN						C66XX_GPIO_DIR_IN


/*------------ TA66XX_BC_set_dsp_to_fpga_gpio_enable() function ----------*//**
 * @brief  Function enables or disables on-board DSP-to-FPGA GPIO pins
 *
 * @param[in]  state - DSP-to-FPGA GPIO pins enable state: TA66XX_ON or
 *					TA66XX_OFF;
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_set_dsp_to_fpga_gpio_enable(uint32_t enable);


/*------------ TA66XX_BC_get_dsp_to_fpga_gpio_enable_status() function ---*//**
 * @brief  Function returns enable status of on-board DSP-to-FPGA GPIO pins
 *
 * @return  DSP-to-FPGA GPIO pins enable state: TA66XX_ON or TA66XX_OFF;
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_dsp_to_fpga_gpio_enable_status(void);


/*------------ TA66XX_BC_set_dsp_to_fpga_gpio_pin_dir() function ---------*//**
 * @brief  Function sets selected DSP-to-FPGA GPIO pin direction: IN or OUT
 *
 * @param[in]  pin - DSP-to-FPGA GPIO pin number;
 * @param[in]  dir - DSP-to-FPGA GPIO pin direction: IN or OUT
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_set_dsp_to_fpga_gpio_pin_dir(uint32_t pin, uint32_t dir);


/*------------ TA66XX_BC_get_dsp_to_fpga_gpio_pin_dir() function ---------*//**
 * @brief  Function returns selected DSP-to-FPGA GPIO pin direction: IN or OUT
 *
 * @param[in]  pin - DSP-to-FPGA GPIO pin number;
 *
 * @return  DSP-to-FPGA GPIO pin direction: IN or OUT
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_dsp_to_fpga_gpio_pin_dir(uint32_t pin);


/*------------ TA66XX_BC_set_dsp_to_fpga_gpio_pin_data() function --------*//**
 * @brief  Function sets selected DSP-to-FPGA GPIO pin to the desired state: ON
 * or OFF
 *
 * @param[in]  pin - DSP-to-FPGA GPIO pin number;
 * @param[in]  data - DSP-to-FPGA GPIO pin state: TA66XX_ON or TA66XX_OFF;
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_set_dsp_to_fpga_gpio_pin_data(uint32_t pin, uint32_t data);


/*------------ TA66XX_BC_get_dsp_to_fpga_gpio_pin_data() function --------*//**
 * @brief  Function returns selected DSP-to-FPGA GPIO pin state: ON or OFF
 *
 * @param[in]  pin - DSP-to-FPGA GPIO pin number;
 *
 * @return  DSP-to-FPGA GPIO pin state: TA66XX_ON or TA66XX_OFF
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_dsp_to_fpga_gpio_pin_data(uint32_t pin);


/*------------ TA66XX_BC_set_dsp_to_fpga_gpio_dir() function -------------*//**
 * @brief  Function sets selected DSP-to-FPGA GPIO pins direction: IN or OUT
 *
 * @param[in]  pin_bitmask - DSP-to-FPGA GPIO pins (ORed) bitmask;
 * @param[in]  dir - DSP-to-FPGA GPIO pin direction: IN or OUT
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_set_dsp_to_fpga_gpio_dir(uint32_t pin_bitmask, uint32_t dir);


/*------------ TA66XX_BC_get_dsp_to_fpga_gpio_dir() function -------------*//**
 * @brief  Function returns selected DSP-to-FPGA GPIO pins direction: IN or OUT
 *
 * @param[in]  pin - DSP-to-FPGA GPIO pins (ORed) bitmask;
 *
 * @return  DSP-to-FPGA GPIO pins direction: IN or OUT
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_dsp_to_fpga_gpio_dir(uint32_t pin_bitmask);


/*------------ TA66XX_BC_set_dsp_to_fpga_gpio_data() function ------------*//**
 * @brief  Function sets selected DSP-to-FPGA GPIO pins to the desired state: ON
 * or OFF
 *
 * @param[in]  pin_bitmask - DSP-to-FPGA GPIO pins (ORed) bitmask;
 * @param[in]  data - DSP-to-FPGA GPIO pins state: TA66XX_ON or TA66XX_OFF;
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_set_dsp_to_fpga_gpio_data(uint32_t pin_bitmask, uint32_t data);


/*------------ TA66XX_BC_get_dsp_to_fpga_gpio_data() function ------------*//**
 * @brief  Function returns selected DSP-to-FPGA GPIO pins state: ON or OFF
 *
 * @param[in]  pin - DSP-to-FPGA GPIO pins (ORed) bitmask;
 *
 * @return  DSP-to-FPGA GPIO pins state
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_dsp_to_fpga_gpio_data(uint32_t pin_bitmask);


/** @}*/
//=============================================================================



//=============================================================================
//============ DSP GPIO IRQ functions =========================================
//=============================================================================

/** @addtogroup  TA66XX_BC_DSP_GPIO_IRQ  TORNADO AMC DSP GPIO IRQ mapping functions
 *  @{
 */

/** DSP GPIO pins IRQ source definitions */
enum TA66XX_BC_DSP_GPIO_IRQ_SEL_SOURCE
{
	TA66XX_BC_DSP_GPIO_IRQ_SEL_OFF = 0,					/**< DSP IRQ is disabled */
	TA66XX_BC_DSP_GPIO_IRQ_SEL_MMC_TO_DSP_PD_ALERT,		/**< MMC-to-DSP power-down alert is used to generate DSP IRQ */
	TA66XX_BC_DSP_GPIO_IRQ_SEL_MMC_TO_DSP_COMM_RQ,		/**< MMC-to-DSP communication request is used to generate DSP IRQ */
	TA66XX_BC_DSP_GPIO_IRQ_SEL_1GE_PHY_IRQ				/**< 1GE PHY IRQ is used to generate DSP IRQ */
};

/** DSP GPIO-6 and GPIO-7 pins definitions for TA6678 rev.1B and TA6678/FMC rev.1A boards */
#define TA66XX_BC_DSP_GPIO6_PIN								6
#define TA66XX_BC_DSP_GPIO7_PIN								7
/** DSP GPIO-8 and GPIO-9 pins definitions for TA6678/FMC rev.1B and later boards */
#define TA66XX_BC_DSP_GPIO8_PIN								8
#define TA66XX_BC_DSP_GPIO9_PIN								9


/*------------ TA66XX_BC_map_dsp_gpio_interrupt() function ---------------*//**
 * @brief  Function maps IRQ source to selected DSP GPIO pin
 *
 * @param[in]  gpio_pin - DSP GPIO pin;
 * @param[in]  irq_src - IRQ source;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_map_dsp_gpio_interrupt(uint32_t gpio_pin, uint32_t irq_src);


/*------------ TA66XX_BC_unmap_dsp_gpio_interrupt() function -------------*//**
 * @brief  Function unmaps IRQ source from selected DSP GPIO pin
 *
 * @param[in]  gpio_pin - DSP GPIO pin;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_unmap_dsp_gpio_interrupt(uint32_t gpio_pin);


/** @}*/
//=============================================================================



//=============================================================================
//============ UART functions =================================================
//=============================================================================

/** @addtogroup  TA66XX_BC_UART  TORNADO AMC UART functions
 *  @{
 */

/** DSP UART printf-like string max length */
#define TA66XX_BC_UART_PRINTF_LEN_MAX						1023


/*------------ TA66XX_BC_init_uart() function ---------------------------*//**
 * @brief  Function inits the DSP UART peripheral
 *
 * @param[in]  baud_rate - desired baud rate;
 * @param[in]  data_bits - number of data bits;
 * @param[in]  parity - parity bit;
 * @param[in]  stop_bits - number of stop bits;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_init_uart(uint32_t baud_rate, uint32_t data_bits, uint32_t parity, uint32_t stop_bits);


/*------------ TA66XX_BC_uart_receiver_is_ready() function ---------------*//**
 * @brief  Function checks if a character is received over DSP UART
 *
 * @return  1 - a character is received over DSP UART,
 *		    0 - otherwise
 *
-----------------------------------------------------------------------------*/
#define TA66XX_BC_uart_receiver_is_ready()					C66XX_UART_receiver_is_ready()


/*------------ TA66XX_BC_uart_receive_char() function --------------------*//**
 * @brief  Function receives a character over DSP UART
 *
 * @return  Received character
 *
-----------------------------------------------------------------------------*/
#define TA66XX_BC_uart_receive_char()						C66XX_UART_receive_char()


/*------------ TA66XX_BC_uart_transmit_char() function -------------------*//**
 * @brief  Function transmits a character over DSP UART
 *
 * @param[in]  c - A character to transmit
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
#define TA66XX_BC_uart_transmit_char(c)						C66XX_UART_transmit_char(c)


/*------------ TA66XX_BC_uart_transmit_string() function -----------------*//**
 * @brief  Function transmits a string until '0' charachter
 *
 * @param[in]  s - A pointer to the string to transmit
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
#define TA66XX_BC_uart_transmit_string(s)					C66XX_UART_transmit_string(s)


/*------------ TA66XX_BC_uart_receive_line_string() function -------------*//**
 * @brief  Function receives a line ended with CR character, and stores
 * received characters into string with '\0' symbol.
 *
 * @param[in]  s - Pointer to a string to store received characters
 *
 * @return  Number of received characters without '\0' symbol.
 *
-----------------------------------------------------------------------------*/
#define TA66XX_BC_uart_receive_line_string(s)				C66XX_UART_receive_line_string(s)


/*------------ TA66XX_BC_uart_printf() function --------------------------*//**
 * @brief  Function prints messages to UART
 *
 * @param[in]  format - format string;
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_uart_printf(const char *format, ...);


/** @}*/
//=============================================================================



//=============================================================================
//============ EEPROM functions ===============================================
//=============================================================================

/** @addtogroup  TA66XX_BC_EEPROM  TORNADO AMC EEPROM memory functions
 *  @{
 */

/*------------ TA66XX_BC_get_eeprom_length() function --------------------*//**
 * @brief  Function returns on-board EEPROM memory length in bytes
 *
 * @return  On-board EEPROM memory length in bytes
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_eeprom_length(void);


/*------------ TA66XX_BC_write_eeprom() function -------------------------*//**
 * @brief  Function writes data[len] to EEPROM memory starting at memory
 * address addr
 *
 * @param[in]  addr - memory address within the chip;
 * @param[in]  data - pointer to data;
 * @param[in]  len - data len in bytes;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_write_eeprom(uint32_t addr, uint8_t *data, uint32_t len);


/*------------ TA66XX_BC_read_eeprom() function --------------------------*//**
 * @brief  Function reads data[len] from EEPROM memory starting at memory
 * address addr
 *
 * @param[in]  addr - memory address within the chip;
 * @param[out]  buf - pointer to data buffer to store data;
 * @param[in]  len - data len in bytes;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_read_eeprom(uint32_t addr, uint8_t *buf, uint32_t len);


/*------------ TA66XX_BC_erase_eeprom() function -------------------------*//**
 * @brief  Function erases len bytes from EEPROM memory starting at memory
 * address addr
 *
 * @param[in]  addr - memory address within the chip;
 * @param[in]  len - number of bytes to erase;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_erase_eeprom(uint32_t addr, uint32_t len);


/** @}*/
//=============================================================================



//=============================================================================
//============ Board info functions ===========================================
//=============================================================================

/** @addtogroup  TA66XX_BC_BOARD_INFO  TORNADO AMC board info functions
 *  @{
 */


/** Board info data descriptor IDs */
#define TA66XX_BC_BOARD_INFO_ID_TA6678_REV1B				1
#define TA66XX_BC_BOARD_INFO_ID_TA6678FMC_REV1A				2
#define TA66XX_BC_BOARD_INFO_ID_TA6678FMC_REV1B				3


/** Board temperature data descriptor */
typedef struct
{
	float						temp_pcb,								/**< PCB current temperature */
								temp_dsp,								/**< DSP current temperature */
								temp_fpga;								/**< FPGA current temperature */

	// TEMP zone classification status
	// This var is set according to TA6678_MC_TEMP_ZONE_ID_xxxx defs.
	uint32_t					temp_zone_id;							/**< Temperature zone ID */

	// 8-bit integer thresholds for MMC TEMP zones
	// These thresholds are used to classify summary temparature, which MMC derives
	// from PCB, DSP and FPGA temperatures.
	char						overheated_low_zone_threshold,			/**< OVERHEATED low threshold (P/S will be switched OFF at this threshold and upper) */
								hot_low_zone_threshold,					/**< HOT low threshold (P/S will be not switched ON at this threshold and upper) */
								warm3_low_zone_threshold,				/**< WARM3 low threshold */
								warm2_low_zone_threshold,				/**< WARM2 low threshold */
								warm1_low_zone_threshold,				/**< WARM1 low threshold */
								normal_low_zone_threshold,				/**< NORMAL low threshold */
								cold1_low_zone_threshold,				/**< COLD1 low threshold */
								cold2_low_zone_threshold,				/**< COLD2 low threshold */
								cold3_low_zone_threshold,				/**< COLD3 low threshold (P/S will be not switched ON below this threshold) */
								frosty_low_zone_threshold;				/**< FROSTY low threshold (P/S will be switched OFF below this threshold) */

	// 8-bit integer thresholds for AMC TEMP zones for every on-board temp sensor
	char						amc_pcb_temp_noncritical_threshold,		/**< PCB temperature AMC UPPER NON-CRITICAL ASSERTING EVENT threshold */
								amc_pcb_temp_critical_threshold,		/**< PCB temperature AMC UPPER CRITICAL ASSERTING EVENT threshold */
								amc_pcb_temp_nonrecoverable_threshold,	/**< PCB temperature AMC UPPER NON-RECOVERABLE ASSERTING EVENT threshold */
								amc_pcb_temp_hysteresis,				/**< PCB temperature AMC positive going hysteresis */

								amc_dsp_temp_noncritical_threshold,		/**< DSP temperature AMC UPPER NON-CRITICAL ASSERTING EVENT threshold */
								amc_dsp_temp_critical_threshold,		/**< DSP temperature AMC UPPER CRITICAL ASSERTING EVENT threshold */
								amc_dsp_temp_nonrecoverable_threshold,	/**< DSP temperature AMC UPPER NON-RECOVERABLE ASSERTING EVENT threshold */
								amc_dsp_temp_hysteresis,				/**< DSP temperature AMC positive going hysteresis */

								amc_fpga_temp_noncritical_threshold,	/**< FPGA temperature AMC UPPER NON-CRITICAL ASSERTING EVENT threshold */
								amc_fpga_temp_critical_threshold,		/**< FPGA temperature AMC UPPER CRITICAL ASSERTING EVENT threshold */
								amc_fpga_temp_nonrecoverable_threshold,	/**< FPGA temperature AMC UPPER NON-RECOVERABLE ASSERTING EVENT threshold */
								amc_fpga_temp_hysteresis;				/**< FPGA temperature AMC positive going hysteresis */

} TA66XX_BC_TEMP_DATA_DD;
// Board temperature data descriptor length
#define TA66XX_BC_TEMP_DATA_DD_LEN							sizeof(TA66XX_BC_TEMP_DATA_DD)


/** Board current power data descriptor for TA6678 rev.1B-1 board */
typedef struct
{
	float						v_12v,									/**< Input +12V power supply voltage value */
								i_12v,									/**< Input +12V power supply current value */
								v_3v3_aux,								/**< On-board auxiliary +3.3V power supply voltage value */
								i_3v3_aux,								/**< On-board auxiliary +3.3V power supply current value */
								v_mc,									/**< Module management power supply voltage value */
								i_mc,									/**< Module management power supply current value */
								v_3v3,									/**< On-board +3.3V power supply voltage value */
								i_3v3,									/**< On-board +3.3V power supply current value */
								v_2v5,									/**< On-board +2.5V power supply voltage value */
								i_2v5,									/**< On-board +2.5V power supply current value */
								v_1v8,									/**< On-board +1.8V power supply voltage value */
								i_1v8,									/**< On-board +1.8V power supply current value */
								v_1v5,									/**< On-board +1.5V power supply voltage value */
								i_1v5,									/**< On-board +1.5V power supply current value */
								v_1v0,									/**< On-board +1.0V power supply voltage value */
								i_1v0,									/**< On-board +1.0V power supply current value */
								v_ddr_vtt,								/**< On-board DDR3 VTT power supply voltage value */
								v_dsp_vcore,							/**< On-board DSP core power supply voltage value */
								i_dsp_vcore,							/**< On-board DSP core power supply current value */
								v_fpga_vcore,							/**< On-board FPGA core power supply voltage value */
								i_fpga_vcore,							/**< On-board FPGA core power supply current value */
								v_fpga_mgtavcc,							/**< On-board FPGA MGTAVCC power supply voltage value */
								v_fpga_mgtavtt;							/**< On-board FPGA MGTAVTT power supply voltage value */

} TA6678_REV1B_BC_POWER_DATA_DD;
// Board current power data descriptor for TA6678 rev.1B-1 board length
#define TA6678_REV1B_BC_POWER_DATA_DD_LEN					sizeof(TA6678_REV1B_BC_POWER_DATA_DD)


/** Board current power data descriptor for TA6678/FMC rev.1A and later boards */
typedef struct
{
	float						power_12v,								/**< Input +12V power supply value */
								power_fpga_core,						/**< On-board FPGA core power supply value */
								power_dsp_core,							/**< On-board DSP core power supply value */
								v_12v,									/**< Input +12V power supply voltage value */
								i_12v,									/**< Input +12V power supply current value */
								i_fpga_core,							/**< On-board FPGA core power supply current value */
								i_dsp_core;								/**< On-board DSP core power supply current value */

} TA66XX_BC_PRIMARY_POWER_DATA_DD;
// Board current power data descriptor length
#define TA66XX_BC_PRIMARY_POWER_DATA_DD_LEN					sizeof(TA66XX_BC_PRIMARY_POWER_DATA_DD)


/** Board current power data union */
typedef union
{
	TA6678_REV1B_BC_POWER_DATA_DD			ta6678_rev1b_dd;
	TA66XX_BC_PRIMARY_POWER_DATA_DD			primary_power_dd;
} POWER_DATA_DD;


/** Board current power data descriptor */
typedef struct
{
	unsigned					id;										// ID to decode the UNION structure type
	POWER_DATA_DD				power_data_dd;

} TA66XX_BC_POWER_DATA_DD;
// Board current power data descriptor length
#define TA66XX_BC_POWER_DATA_DD_LEN							sizeof(TA66XX_BC_POWER_DATA_DD)


/** FMC interface power data descriptor for TA6678/FMC rev.1A and later boards */
typedef struct
{
	float						power_fmc_12v,							/**< FMC I/F +12V power supply value */
								power_fmc_3v3,							/**< FMC I/F +3.3V power supply value */
								power_fmc_vadj,							/**< FMC I/F Vadj power supply value */
								v_fmc_12v,								/**< FMC I/F +12V power supply voltage value */
								i_fmc_12v,								/**< FMC I/F +12V power supply current value */
								v_fmc_3v3,								/**< FMC I/F +3.3V power supply voltage value */
								i_fmc_3v3,								/**< FMC I/F +3.3V power supply current value */
								v_fmc_vadj,								/**< FMC I/F Vadj power supply voltage value */
								i_fmc_vadj,								/**< FMC I/F Vadj power supply current value */
								v_fmc_vio_b_m2c,						/**< FMC I/F VIO_B_M2C power supply voltage value */
								v_fmc_vref_a_m2c,						/**< FMC I/F VREF_A_M2C power supply voltage value */
								v_fmc_vref_b_m2c;						/**< FMC I/F VREF_B_M2C power supply voltage value */

} TA66XX_BC_FMC_POWER_DATA_DD;
// FMC interface power data descriptor length
#define TA66XX_BC_FMC_POWER_DATA_DD_LEN						sizeof(TA66XX_BC_FMC_POWER_DATA_DD)


/** Max string length in device info structure */
#define TA66XX_BC_DEVICE_INFO_STRING_LEN_MAX				32
#define TA66XX_BC_DEVICE_PN_STRING_LEN_MAX					64
#define TA66XX_BC_DEVICE_HW_REVISION_STRING_LEN_MAX			16


/** Board info data descriptor for TA6678 rev.1B-1 board */
typedef struct
{
	uint32_t					op_mode;													/**< Operation mode: '0' - stand-alone mode, '1' - AMC host mode */
	uint32_t					amc_slot_id;												/**< AMC slot ID (in case AMC host mode is used) */
	uint8_t						manufacturing_date_day_of_month;							/**< Manufacturing day: 1..31 */
	uint8_t						manufacturing_date_month;									/**< Manufacturing month: 1..12 ('1' - January) */
	uint16_t					manufacturing_date_year;									/**< Manufacturing year: 2015.. */
	char						device_sn[TA66XX_BC_DEVICE_INFO_STRING_LEN_MAX];			/**< Device serial number */
	char						mmc_fw_rev[TA66XX_BC_DEVICE_INFO_STRING_LEN_MAX];			/**< MMC firmware revision string */
	char						tammc_fw_rev[TA66XX_BC_DEVICE_INFO_STRING_LEN_MAX];			/**< TAMMC firmware revision string */
	char						mmc_core_fw_rev[TA66XX_BC_DEVICE_INFO_STRING_LEN_MAX];		/**< MMC core firmware revision string */
} TA6678_REV1B_BC_DEVICE_INFO_DATA_DD;
// Board info data descriptor for TA6678 rev.1B-1 board length
#define TA6678_REV1B_BC_DEVICE_INFO_DATA_DD_LEN				sizeof(TA6678_REV1B_BC_DEVICE_INFO_DATA_DD)


/** Board info data descriptor for TA6678/FMC rev.1A and later boards */
typedef struct
{
	uint32_t					op_mode;															/**< Operation mode: '0' - stand-alone mode, '1' - AMC host mode */
	uint32_t					amc_slot_id;														/**< AMC slot ID (in case AMC host mode is used) */
	uint8_t						manufacturing_date_day_of_month;									/**< Manufacturing day: 1..31 */
	uint8_t						manufacturing_date_month;											/**< Manufacturing month: 1..12 ('1' - January) */
	uint16_t					manufacturing_date_year;											/**< Manufacturing year: 2015.. */
	char						manufacturer_name[TA66XX_BC_DEVICE_INFO_STRING_LEN_MAX];			/**< Manufacturer name */
	char						device_name[TA66XX_BC_DEVICE_INFO_STRING_LEN_MAX];					/**< Device name */
	char						device_pn[TA66XX_BC_DEVICE_PN_STRING_LEN_MAX];						/**< Device part number */
	char						device_sn[TA66XX_BC_DEVICE_INFO_STRING_LEN_MAX];					/**< Device serial number */
	char						device_hw_revision[TA66XX_BC_DEVICE_HW_REVISION_STRING_LEN_MAX];	/**< Device H/W revision */
	char						mmc_fw_rev[TA66XX_BC_DEVICE_INFO_STRING_LEN_MAX];					/**< MMC firmware revision string */
	char						tammc_fw_rev[TA66XX_BC_DEVICE_INFO_STRING_LEN_MAX];					/**< TAMMC firmware revision string */
	char						mmc_core_fw_rev[TA66XX_BC_DEVICE_INFO_STRING_LEN_MAX];				/**< MMC core firmware revision string */
} TA66XX_BC_FULL_DEVICE_INFO_DATA_DD;
// Board info data descriptor length
#define TA66XX_BC_FULL_DEVICE_INFO_DATA_DD_LEN				sizeof(TA66XX_BC_FULL_DEVICE_INFO_DATA_DD)


/** Board current power data union */
typedef union
{
	TA6678_REV1B_BC_DEVICE_INFO_DATA_DD			ta6678_rev1b_dd;
	TA66XX_BC_FULL_DEVICE_INFO_DATA_DD			ta66xx_full_device_info_dd;
} DEVICE_INFO_DATA_DD;


/** Board current power data descriptor */
typedef struct
{
	uint32_t					id;										// ID to decode the UNION structure type
	DEVICE_INFO_DATA_DD			device_info_data_dd;

} TA66XX_BC_DEVICE_INFO_DATA_DD;
// Board current power data descriptor length
#define TA66XX_BC_DEVICE_INFO_DATA_DD_LEN					sizeof(TA66XX_BC_DEVICE_INFO_DATA_DD)


/** Hardware configuration info data descriptor for TA6678/FMC rev.1A and later boards */
typedef struct
{
	uint32_t					fpga_ddr0_len;														/**< FPGA DDR memory bank #0 length in MB: 0 - not installed */
	uint32_t					fpga_ddr1_len;														/**< FPGA DDR memory bank #1 length in MB: 0 - not installed */
	uint32_t					cpu_ddr_len;														/**< CPU DDR memory length in MB: 0 - not installed */
	uint32_t					flash_len;															/**< FLASH memory length in MB: 0 - not installed */
	uint32_t					mram_len;															/**< MRAM memory length in KB: 0 - not installed */
	uint32_t					i2c_eeprom_len;														/**< I2C EEPROM memory length in KB: 0 - not installed */
	uint32_t					cpu_clk_freq_id;													/**< CPU input clock frequency in MHz: 0 - not available */
	uint32_t					x1ge0_phy_installed_id;												/**< External 1GE PHY #0 installed ID: 0 - not installed */
	uint32_t					x1ge1_phy_installed_id;												/**< External 1GE PHY #1 installed ID: 0 - not installed */
	uint32_t					sfp0_if_installed_id;												/**< SFP interface #0 installed ID: 0 - not installed */
	uint32_t					sfp1_if_installed_id;												/**< SFP interface #1 installed ID: 0 - not installed */
	uint32_t					qsfp_if_installed_id;												/**< QSFP interface installed ID: 0 - not installed */
	uint32_t					fmc_if_installed_id;												/**< FMC interface installed ID: 0 - not installed */
	uint32_t					sd_slot_installed_id;												/**< uSD slot installed ID: 0 - not installed */
} TA66XX_BC_HW_CFG_INFO_DATA_DD;
// Hardware configuration info data descriptor length
#define TA66XX_BC_HW_CFG_INFO_DATA_DD_LEN					sizeof(TA66XX_BC_HW_CFG_INFO_DATA_DD)


/** Max string length in FMC info structure */
#define TA66XX_BC_FMC_STATUS_STRING_LEN_MAX					48
#define TA66XX_BC_FMC_INFO_STRING_LEN_MAX					64
#define TA66XX_BC_MANUFACTURING_TIME_STRING_LEN_MAX			32


/** FMC info data descriptor for TA6678/FMC rev.1A and later boards */
typedef struct
{
	char						fmc_status_text[TA66XX_BC_FMC_STATUS_STRING_LEN_MAX];				/**< FMC status */
	char						manufacturer_name[TA66XX_BC_FMC_INFO_STRING_LEN_MAX];				/**< Manufacturer name */
	char						device_name[TA66XX_BC_FMC_INFO_STRING_LEN_MAX];						/**< Device name */
	char						device_pn[TA66XX_BC_FMC_INFO_STRING_LEN_MAX];						/**< Device part number */
	char						device_sn[TA66XX_BC_FMC_INFO_STRING_LEN_MAX];						/**< Device serial number */
	char						device_version[TA66XX_BC_FMC_INFO_STRING_LEN_MAX];					/**< Device version */
	char						manufacturing_time[TA66XX_BC_MANUFACTURING_TIME_STRING_LEN_MAX];	/**< Manufacturing time */
} TA66XX_BC_FMC_INFO_DATA_DD;
// FMC info data descriptor length
#define TA66XX_BC_FMC_INFO_DATA_DD_LEN						sizeof(TA66XX_BC_FMC_INFO_DATA_DD)



/*------------ TA66XX_BC_get_device_info() function ----------------------*//**
 * @brief  Function returns AMC module device info: operation mode, serial
 * number, manufacturing date, firmware revisions
 *
 * @param[out]  info_dd - pointer to a buffer that receives device info data
 *				descriptor in case function is completed without errors.
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_get_device_info(TA66XX_BC_DEVICE_INFO_DATA_DD *info_dd);


/*------------ TA66XX_BC_get_hw_cfg_info() function ----------------------*//**
 * @brief  Function returns AMC module hardware configuration info: installed
 * memories, interfaces, etc.
 *
 * @param[out]  info_dd - pointer to a buffer that receives hardware
 *				configuration data descriptor in case function is completed
 *				without errors.
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_get_hw_cfg_info(TA66XX_BC_HW_CFG_INFO_DATA_DD *info_dd);


/*------------ TA66XX_BC_get_fmc_info() function -------------------------*//**
 * @brief  Function returns FMC module device info: installed status, device 
 * name, serial number, manufacturing date, firmware revisions, etc.
 *
 * @param[out]  info_dd - pointer to a buffer that receives FMC info data
 *				descriptor in case function is completed without errors.
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_get_fmc_info(TA66XX_BC_FMC_INFO_DATA_DD *info_dd);


/*------------ TA66XX_BC_get_temp_state() function -----------------------*//**
 * @brief  Function returns AMC module temperature monitor current info
 *
 * @param[out]  temp_data_dd - pointer to a buffer that receives board
 *				temperature data descriptor in case function is completed
 *				without errors.
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_get_temp_state(TA66XX_BC_TEMP_DATA_DD *temp_data_dd);


/*------------ TA66XX_BC_get_power_state() function ----------------------*//**
 * @brief  Function returns AMC module power monitor current info
 *
 * @param[out]  power_data_dd - pointer to a buffer that receives board power
 *				data descriptor in case function is completed without errors.
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_get_power_state(TA66XX_BC_POWER_DATA_DD *power_data_dd);


/*------------ TA66XX_BC_get_fmc_power_state() function ------------------*//**
 * @brief  Function returns FMC module power monitor current info
 *
 * @param[out]  fmc_power_data_dd - pointer to a buffer that receives FMC
 *				module power data descriptor in case function is completed
 *				without errors.
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_get_fmc_power_state(TA66XX_BC_FMC_POWER_DATA_DD *fmc_power_data_dd);


/*------------ TA66XX_BC_set_mmc_to_dsp_rq() function --------------------*//**
 * @brief  Function sets MMC-to-DSP request from DSP (for test purposes)
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_set_mmc_to_dsp_rq(void);


/*------------ TA66XX_BC_set_mmc_power_down_notification() function ------*//**
 * @brief  Function sets MMC-to-DSP power down notification from DSP
 * (for test purposes)
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_set_mmc_power_down_notification(void);


/*------------ TA66XX_BC_get_sfp_info() function -------------------------*//**
 * @brief  Function returns identification info about installed SFP
 * transceiver: 256-byte array read from address 0x50 (identification info
 * according to SFF-8472) and 256-byte array read from address 0x51 (digital
 * diagnostic monitoring interface (DDMI) data) 
 *
 * @param[out]  id_data - pointer to a buffer that receives 256-byte array read
 *				from address 0x50 (identification info according to SFF-8472).
 *				In case pointer is NULL, then no data are filled in;
 * @param[out]  ddmi_data - pointer to a buffer that receives 256-byte array
 *				read from address 0x51 (DDMI data). In case pointer is NULL,
 *				then no data are filled in;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_get_sfp_info(uint8_t *id_data, uint8_t *ddmi_data);


/** @}*/

//=============================================================================



//=============================================================================
//============ FPGA functions =================================================
//=============================================================================

/** @addtogroup  TA66XX_BC_FPGA  TORNADO AMC FPGA functions
 *  @{
 */

/** FPGA host interface (HIF) configuration data descriptor */
typedef struct
{
	uint32_t					w_setup;			/**< Write strobe setup EMIF-16 cycles */
	uint32_t					w_strobe;			/**< Write strobe duration EMIF-16 cycles */
	uint32_t					w_hold;				/**< Write strobe hold EMIF-16 cycles */
	uint32_t					r_setup;			/**< Read strobe setup EMIF-16 cycles */
	uint32_t					r_strobe;			/**< Read strobe duration EMIF-16 cycles */
	uint32_t					r_hold;				/**< Read strobe hold EMIF-16 cycles */
	uint32_t					turn_around;		/**< Turn around EMIF-16 cycles */
} TA66XX_BC_FPGA_HIF_CNF_DD;
// FPGA HIF configuration data descriptor length in bytes
#define TA66XX_BC_FPGA_HIF_CNF_DD_LEN						sizeof(TA66XX_BC_FPGA_HIF_CNF_DD)


/*------------ TA66XX_BC_set_fpga_hif_cnf() function ---------------------*//**
 * @brief  Function sets new configuration to FPGA host interface according to
 * supplied parameters
 *
 * @param[in]  cfg_dd - pointer to new FPGA HIF configuration data descriptor;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_set_fpga_hif_cnf(TA66XX_BC_FPGA_HIF_CNF_DD *cfg_dd);


/*------------ TA66XX_BC_get_fpga_hif_cnf() function ---------------------*//**
 * @brief  Function returns current configuration of FPGA host interface
 *
 * @param[in]  cfg_dd - pointer to buffer to receive current FPGA HIF
 *				configuration data descriptor;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_get_fpga_hif_cnf(TA66XX_BC_FPGA_HIF_CNF_DD *cfg_dd);


/*------------ TA66XX_BC_get_fpga_hif_base_addr() function ---------------*//**
 * @brief  Function returns FPGA host interface base address
 *
 * @return  FPGA host interface base address
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_fpga_hif_base_addr(void);


/*------------ TA66XX_BC_get_fpga_hif_length() function ------------------*//**
 * @brief  Function returns FPGA host interface length in bytes
 *
 * @return  FPGA host interface length in bytes
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_fpga_hif_length(void);


/*------------ TA66XX_BC_set_fpga_hif_reset() function -------------------*//**
 * @brief  Function sets or releases FPGA host interface reset
 *
 * @param[in]  state - FPGA host interface reset state: ON or OFF
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_set_fpga_hif_reset(uint32_t state);


/*------------ TA66XX_BC_get_fpga_hif_reset_state() function -------------*//**
 * @brief  Function returns FPGA host interface reset state
 *
 * @return  FPGA host interface reset state: ON or OFF
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_fpga_hif_reset_state(void);


/*------------ TA66XX_BC_read_fpga_hif_byte() function -------------------*//**
 * @brief  Function reads a byte from FPGA host interface at address addr.
 *
 * @param[in]  addr - FPGA address to read from
 *
 * @return  A byte read from FPGA host interface
 *
-----------------------------------------------------------------------------*/
uint8_t TA66XX_BC_read_fpga_hif_byte(uint32_t addr);


/*------------ TA66XX_BC_write_fpga_hif_byte() function ------------------*//**
 * @brief  Function writes a byte to FPGA host interface at address addr.
 *
 * @param[in]  addr - FPGA address to write to
 * @param[in]  v - A byte to write
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_write_fpga_hif_byte(uint32_t addr, uint8_t v);


/*------------ TA66XX_BC_read_fpga_hif_w16() function --------------------*//**
 * @brief  Function reads a 16-bit word from FPGA host interface at address
 * addr.
 *
 * @param[in]  addr - FPGA address to read from
 *
 * @return  A 16-bit word read from FPGA host interface
 *
-----------------------------------------------------------------------------*/
uint16_t TA66XX_BC_read_fpga_hif_w16(uint32_t addr);


/*------------ TA66XX_BC_write_fpga_hif_w16() function -------------------*//**
 * @brief  Function writes a 16-bit word to FPGA host interface at address
 * addr.
 *
 * @param[in]  addr - FPGA address to write to
 * @param[in]  v - A 16-bit word to write
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_write_fpga_hif_w16(uint32_t addr, uint16_t v);


/*------------ TA66XX_BC_read_fpga_hif_w32() function --------------------*//**
 * @brief  Function reads a 32-bit word from FPGA host interface at address
 * addr.
 *
 * @param[in]  addr - FPGA address to read from
 *
 * @return  A 32-bit word read from FPGA host interface
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_read_fpga_hif_w32(uint32_t addr);


/*------------ TA66XX_BC_write_fpga_hif_w32() function -------------------*//**
 * @brief  Function writes a 32-bit word to FPGA host interface at address
 * addr.
 *
 * @param[in]  addr - FPGA address to write to
 * @param[in]  v - A 32-bit word to write
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_write_fpga_hif_w32(uint32_t addr, uint32_t v);


/*------------ TA66XX_BC_read_fpga_hif_w64() function --------------------*//**
 * @brief  Function reads a 64-bit word from FPGA host interface at address
 * addr.
 *
 * @param[in]  addr - FPGA address to read from
 *
 * @return  A 64-bit word read from FPGA host interface
 *
-----------------------------------------------------------------------------*/
uint64_t TA66XX_BC_read_fpga_hif_w64(uint32_t addr);


/*------------ TA66XX_BC_write_fpga_hif_w64() function -------------------*//**
 * @brief  Function writes a 64-bit word to FPGA host interface at address
 * addr.
 *
 * @param[in]  addr - FPGA address to write to
 * @param[in]  v - A 64-bit word to write
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_write_fpga_hif_w64(uint32_t addr, uint64_t v);


/*------------ TA66XX_BC_clear_fpga_cnf() function -----------------------*//**
 * @brief  Function clears FPGA configuration memory
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_clear_fpga_cnf(void);


/*------------ TA66XX_BC_load_fpga_cnf() function ------------------------*//**
 * @brief  Function loads FPGA firmware file using the supported configuration
 * interfaces (serial or parallel)
 *
 * @param[in]  addr - address where FPGA firmware file data were saved;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_load_fpga_cnf(uint32_t addr);


/*------------ TA66XX_BC_get_fpga_cnf_status() function ------------------*//**
 * @brief  Function returns FPGA configuration memory status: either
 * loaded by FPGA firmware file or not
 *
 * @return  TA66XX_OK - FPGA configuration memory is loaded,
 *			TA66XX_FPGA_CNF_LOAD_ERR - FPGA configuration memory is cleared.
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_get_fpga_cnf_status(void);


/** @}*/
//=============================================================================



//=============================================================================
//============ FLASH functions ================================================
//=============================================================================

/** @addtogroup  TA66XX_BC_FLASH  TORNADO AMC FLASH API functions
 *  @{
 */


/** FLASH memory sector is protected for write/erase operations */
#define TA66XX_BC_FLASH_SECTOR_PROTECTED					1
/** FLASH memory sector is unprotected for write/erase operations */
#define TA66XX_BC_FLASH_SECTOR_UNPROTECTED					0


/*------------ TA66XX_BC_get_flash_area_base_addr() function -------------*//**
 * @brief  Function returns FLASH area base address
 *
 * @return  FLASH area base address
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_flash_area_base_addr(void);


/*------------ TA66XX_BC_get_flash_length() function ---------------------*//**
 * @brief  Function returns on-board FLASH memory length in bytes
 *
 * @return  On-board FLASH memory length in bytes
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_flash_length(void);


/*------------ TA66XX_BC_get_flash_hw_wp_enable_status() function --------*//**
 * @brief  Function returns enable status of on-board FLASH memory hardware 
 * (via on-board switch) write-protection
 *
 * @return  On-board FLASH memory hardware write-protection enable state:
 * TA66XX_ON or TA66XX_OFF
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_flash_hw_wp_enable_status(void);


/*------------ TA66XX_BC_get_flash_sw_wp_enable_status() function --------*//**
 * @brief  Function returns enable status of on-board FLASH memory software 
 * write-protection
 *
 * @return  On-board FLASH memory software write-protection enable state:
 * TA66XX_ON or TA66XX_OFF
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_flash_sw_wp_enable_status(void);


/*------------ TA66XX_BC_set_flash_sw_wp_enable() function ---------------*//**
 * @brief  Function enables or disables on-board FLASH memory software
 * write-protection
 *
 * @param[in]  enable - on-board FLASH memory software write-protection enable
 *				state: TA66XX_ON or TA66XX_OFF;
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_set_flash_sw_wp_enable(uint32_t enable);


/*------------ TA66XX_BC_erase_flash() function --------------------------*//**
 * @brief  Function erases FLASH block, which starts from address a
 * and of size len (in bytes)
 *
 * @param[in]  addr - Block start address (in bytes)
 * @param[in]  len - block length (in bytes)
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_erase_flash(uint32_t addr, uint32_t len);


/** Flag to disable FLASH memory erasing before write operation */
#define TA66XX_BC_FLASH_ERASE_IS_DISABLED					0
/** Flag to enable FLASH memory erasing before write operation */
#define TA66XX_BC_FLASH_ERASE_IS_ENABLED					1


/*------------ TA66XX_BC_write_flash() function --------------------------*//**
 * @brief  Function writes data block of size len to FLASH address a
 * The function erases FLASH before writing data to FLASH if erase input
 * parameter is set.
 *
 * @param[in]  a - FLASH address (in bytes)
 * @param[in]  ptr - data block to write
 * @param[in]  len - data block length (in bytes)
 * @param[in]  erase - flag to enable FLASH erasing before write operation
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_write_flash(uint32_t a, void *ptr, uint32_t len, uint32_t erase);


/*------------ TA66XX_BC_write_flash_byte() function ---------------------*//**
 * @brief  Function writes data byte d to FLASH address a
 *
 * @param[in]  a - FLASH address (in bytes)
 * @param[in]  d - data byte to write
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_write_flash_byte(uint32_t addr, uint8_t d);


/*------------ TA66XX_BC_write_flash_w16() function ----------------------*//**
 * @brief  Function writes 16-bit data word d to FLASH address a
 *
 * @param[in]  addr - FLASH address (in bytes)
 * @param[in]  d - 16-bit data word to write
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_write_flash_w16(uint32_t addr, uint16_t d);


/*------------ TA66XX_BC_write_flash_w32() function ----------------------*//**
 * @brief  Function writes 32-bit data word d to FLASH address a
 *
 * @param[in]  addr - FLASH address (in bytes)
 * @param[in]  d - 32-bit data word to write
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_write_flash_w32(uint32_t addr, uint32_t d);


/*------------ TA66XX_BC_read_flash() function ---------------------------*//**
 * @brief  Function reads data block of size len from FLASH address a
 *
 * @param[in]  addr - FLASH address (in bytes)
 * @param[in]  ptr - pointer to data buffer to store data block
 * @param[in]  len - data block length (in bytes)
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_read_flash(uint32_t addr, void *ptr, uint32_t len);


/** @}*/
//=============================================================================



//=============================================================================
//============ MRAM functions =================================================
//=============================================================================

/** @addtogroup  TA66XX_BC_MRAM  TORNADO AMC MRAM API functions
 *  @{
 */


/*------------ TA66XX_BC_get_mram_area_base_addr() function --------------*//**
 * @brief  Function returns MRAM area base address
 *
 * @return  MRAM area base address
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_mram_area_base_addr(void);


/*------------ TA66XX_BC_get_mram_length() function ----------------------*//**
 * @brief  Function returns on-board MRAM memory length in bytes
 *
 * @return  On-board MRAM memory length in bytes
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_mram_length(void);


/*------------ TA66XX_BC_get_mram_sw_wp_enable_status() function ---------*//**
 * @brief  Function returns enable status of on-board MRAM memory software 
 * write-protection
 *
 * @return  On-board MRAM memory software write-protection enable state:
 * TA66XX_ON or TA66XX_OFF
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_get_mram_sw_wp_enable_status(void);


/*------------ TA66XX_BC_set_mram_sw_wp_enable() function ----------------*//**
 * @brief  Function enables or disables on-board MRAM memory software
 * write-protection
 *
 * @param[in]  enable - on-board MRAM memory software write-protection enable
 *				state: TA66XX_ON or TA66XX_OFF;
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_set_mram_sw_wp_enable(uint32_t enable);


/*------------ TA66XX_BC_read_mram_byte() function -----------------------*//**
 * @brief  Function reads a byte from MRAM memory at address addr.
 *
 * @param[in]  addr - MRAM memory address to read from
 *
 * @return  A byte read from MRAM memory
 *
-----------------------------------------------------------------------------*/
uint8_t TA66XX_BC_read_mram_byte(uint32_t addr);


/*------------ TA66XX_BC_write_mram_byte() function ----------------------*//**
 * @brief  Function writes a byte to MRAM memory at address addr.
 *
 * @param[in]  addr - MRAM memory address to write to
 * @param[in]  v - A byte to write
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_write_mram_byte(uint32_t addr, uint8_t v);


/*------------ TA66XX_BC_read_mram_w16() function ------------------------*//**
 * @brief  Function reads a 16-bit word from MRAM memory at address addr.
 *
 * @param[in]  addr - MRAM memory address to read from
 *
 * @return  A 16-bit word read from MRAM memory
 *
-----------------------------------------------------------------------------*/
uint16_t TA66XX_BC_read_mram_w16(uint32_t addr);


/*------------ TA66XX_BC_write_mram_w16() function -----------------------*//**
 * @brief  Function writes a 16-bit word to MRAM memory at address addr.
 *
 * @param[in]  addr - MRAM memory address to write to
 * @param[in]  v - A 16-bit word to write
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_write_mram_w16(uint32_t addr, uint16_t v);


/*------------ TA66XX_BC_read_mram_w32() function ------------------------*//**
 * @brief  Function reads a 32-bit word from MRAM memory at address addr.
 *
 * @param[in]  addr - MRAM memory address to read from
 *
 * @return  A 32-bit word read from MRAM memory
 *
-----------------------------------------------------------------------------*/
uint32_t TA66XX_BC_read_mram_w32(uint32_t addr);


/*------------ TA66XX_BC_write_mram_w32() function -----------------------*//**
 * @brief  Function writes a 32-bit word to MRAM memory at address addr.
 *
 * @param[in]  addr - MRAM memory address to write to
 * @param[in]  v - A 32-bit word to write
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_write_mram_w32(uint32_t addr, uint32_t v);


/*------------ TA66XX_BC_read_mram_w64() function ------------------------*//**
 * @brief  Function reads a 64-bit word from MRAM memory at address addr.
 *
 * @param[in]  addr - MRAM memory address to read from
 *
 * @return  A 64-bit word read from MRAM memory
 *
-----------------------------------------------------------------------------*/
uint64_t TA66XX_BC_read_mram_w64(uint32_t addr);


/*------------ TA66XX_BC_write_mram_w64() function -----------------------*//**
 * @brief  Function writes a 64-bit word to MRAM memory at address addr.
 *
 * @param[in]  addr - MRAM memory address to write to
 * @param[in]  v - A 64-bit word to write
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void TA66XX_BC_write_mram_w64(uint32_t addr, uint64_t v);


/** @}*/
//=============================================================================



//=============================================================================
//============ Gigabit Ethernet (GbE) Switch functions ========================
//=============================================================================

/** @addtogroup  TA66XX_BC_GBE  TORNADO AMC Gigabit Ethernet Switch functions
 *  @{
 */

/** Gigabit Ethernet ports count available at the board */
#define TA66XX_BC_GBE_PORT_COUNT							2

/** DSP SGMII port number which is routed to AMC connector */
#define TA66XX_BC_GBE_PORT_AMC_PORT_NUMBER					0
/** Alias for DSP SGMII port-0 number */
#define TA66XX_BC_GBE_PORT_AMC0_PORT_NUMBER					TA66XX_BC_GBE_PORT_AMC_PORT_NUMBER
/** DSP SGMII port number which is routed to external PHY */
#define TA66XX_BC_GBE_PORT_X1GE_PORT_NUMBER					1
/** Alias for DSP SGMII port-1 number */
#define TA66XX_BC_GBE_PORT_AMC1_PORT_NUMBER					TA66XX_BC_GBE_PORT_X1GE_PORT_NUMBER


/** Gigabit Ethernet port type */
typedef enum
{
	TA66XX_BC_GBE_PORT_TYPE_NONE = 0,					/**< Gigabit Ethernet port is not used */
	TA66XX_BC_GBE_PORT_TYPE_PHY,						/**< Gigabit Ethernet port is connected to on-board PHY */
	TA66XX_BC_GBE_PORT_TYPE_AMC,						/**< Gigabit Ethernet port is connected to the backplane AMC chassis */
	TA66XX_BC_GBE_PORT_TYPE_MAX							/**< End of port type */
} TA66XX_BC_GBE_PORT_TYPE;


/** Gigabit Ethernet port link status */
typedef enum
{
	TA66XX_BC_GBE_PORT_LINK_STATUS_DOWN = 0,			/**< Gigabit Ethernet port link is down */
	TA66XX_BC_GBE_PORT_LINK_STATUS_UP,					/**< Gigabit Ethernet port link is up */
	TA66XX_BC_GBE_PORT_LINK_STATUS_MAX					/**< End of link status */
} TA66XX_BC_GBE_PORT_LINK_STATUS;


/** T-AMC Gigabit Ethernet port data descriptor */
typedef struct
{
	uint32_t							number;					/**< Port number */
	TA66XX_BC_GBE_PORT_TYPE				type;					/**< Gigabit Ethernet port type */
	TA66XX_BC_GBE_PORT_LINK_STATUS		link_status;			/**< Gigabit Ethernet port link status */
	uint8_t								mac_address[6];			/**< MAC (physical) address of the port */
} TA66XX_BC_GBE_PORT_DD;
// T-AMC Gigabit Ethernet port data descriptor length in bytes
#define TA66XX_BC_GBE_PORT_DD_LEN							sizeof(TA66XX_BC_GBE_PORT_DD)


/** IP address string max size in bytes*/
#define TA66XX_BC_IP_ADDRESS_LEN_MAX						16
/** Host name max size in bytes*/
#define TA66XX_BC_HOSTNAME_LEN_MAX							256


/** Static IP configuration */
#define TA66XX_BC_IP_CFG_TYPE_STATIC						0
/** Dynamic (DHCP) IP configuration */
#define TA66XX_BC_IP_CFG_TYPE_DYNAMIC						1


/*------------ TA66XX_BC_init_gbe_port() function ------------------------*//**
 * @brief  Function initializes selected DSP Gigabit Ethernet port
 *
 * @param[in]  port - DSP Gigabit Ethernet port number
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_init_gbe_port(uint32_t port);


/*------------ TA66XX_BC_get_gbe_port_info() function --------------------*//**
 * @brief  Function returns Gigabit Ethernet port info: port mode, MAC address,
 * link status
 *
 * @param[in]  port - Gigabit Ethernet port number for which port info should
 *					be returned
 * @param[out]  port_dd - pointer to a buffer to which Gigabit Ethernet port
 *					data descriptor should be returned
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_BC_get_gbe_port_info(uint32_t port, TA66XX_BC_GBE_PORT_DD *port_dd);


/** @}*/
//=============================================================================



#ifdef __cplusplus
}
#endif



//=============================================================================
#endif /* __TA66XX_BC_DSP_BC_FUNCTIONS_HXX__ */
