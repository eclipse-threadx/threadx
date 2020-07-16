/******************************************************************************
	TMS320C66xx KeyStone Multicore DSP Software Development Kit (SDK). Rev 2A.
	(C) MicroLAB Systems, 2014-2015

	File:	C66xx DSP SDK API functions declarations
	-----

	Notes:
	------
	1.	This C-header file contains C66xx DSP SDK API functions declarations
		and is used with C66XX.h C-header file.

	2.	This file is best viewed with the TAB setting set to '4'.

******************************************************************************/


/**
 * @file  C66XX_FUNCTIONS.hxx
 *
 * @brief  SDK API functions declarations
 *
 * This file contains C66xx DSP SDK API functions declarations
 *
 */


#ifndef __C66XX_FUNCTIONS_HXX__								// check for this file has been already included
#define __C66XX_FUNCTIONS_HXX__								1



//=============================================================================
//============ Returned error codes ===========================================
//=============================================================================
/** @addtogroup  C66XX_ERRORS  DSP SDK API returned error codes
 *  @{
 */
#define C66XX_OK											0			/**< No errors */
#define C66XX_PARAM_ERR										-1			/**< Invalid function parameter */
#define C66XX_INVALID_HW_ERR								-2			/**< Invalid H/W error (invalid board, etc.) */
#define C66XX_HW_ERR										-3			/**< H/W error (error writing to the DSP control registers, etc) */
#define C66XX_I2C_ERR										-10			/**< I2C module is not correctly operated */
#define C66XX_I2C_AL_ERR									-11			/**< I2C module arbitration lost error */
#define C66XX_I2C_RSFULL_ERR								-12			/**< I2C module overrun error */
#define C66XX_I2C_XSMT_ERR									-13			/**< I2C module underflow error */
#define C66XX_I2C_NACK_ERR									-14			/**< I2C module no-acknowledgement error */
#define C66XX_I2C_ARDY_ERR									-15			/**< I2C module register-access-ready error */
#define C66XX_I2C_READ_ERR									-16			/**< I2C module read error */
#define C66XX_I2C_WRITE_ERR									-17			/**< I2C module write error */
#define C66XX_I2C_TIMEOUT_ERR								-18			/**< I2C bus timeout error */
/** @}*/
//=============================================================================



//=============================================================================
//============ SDK API functions ID defs ======================================
//=============================================================================
/** @addtogroup  C66XX_FUNCTIONS_ID  SDK API functions ID defs
 *  @{
 */

enum
{
	C66XX_GET_ERROR_MESSAGE_FUNCTION_ID = 0,
	C66XX_GET_FUNCTION_NAME_FUNCTION_ID,
	C66XX_SYS_SET_CORE_RESET_STATE_FUNCTION_ID,
	C66XX_SYS_GET_CORE_RESET_STATE_FUNCTION_ID,
	C66XX_SYS_GET_CORE_RESET_SOURCE_FUNCTION_ID,
	C66XX_SYS_SET_CORE_BOOT_ADDRESS_FUNCTION_ID,
	C66XX_SYS_INIT_MAIN_PLL_FUNCTION_ID,
	C66XX_SYS_GET_MAIN_PLL_SETTINGS_FUNCTION_ID,
	C66XX_SYS_INIT_DDR3_PLL_FUNCTION_ID,
	C66XX_SYS_GET_DDR3_PLL_SETTINGS_FUNCTION_ID,
	C66XX_SYS_INIT_PASS_PLL_FUNCTION_ID,
	C66XX_SYS_GET_PASS_PLL_SETTINGS_FUNCTION_ID,
	C66XX_SYS_ENABLE_POWER_DOMAIN_FUNCTION_ID,
	C66XX_MEM_INIT_XMC_MPAX_SEGMENT_FUNCTION_ID,
	C66XX_MEM_INIT_DDR3_FUNCTION_ID,
	C66XX_INT_INIT_CORE_FUNCTION_ID,
	C66XX_INT_MAP_CORE_EVENT_HANDLER_FUNCTION_ID,
	C66XX_INT_UNMAP_CORE_EVENT_HANDLER_FUNCTION_ID,
	C66XX_INT_INIT_CHIP_FUNCTION_ID,
	C66XX_GPIO_INIT_FUNCTION_ID,
	C66XX_TIMER_INIT_FUNCTION_ID,
	C66XX_TIMER_START_FUNCTION_ID,
	C66XX_TIMER_STOP_FUNCTION_ID,
	C66XX_TIMER_RESET_FUNCTION_ID,
	C66XX_TIMER_ENABLE_INTERRUPTS_FUNCTION_ID,
	C66XX_TIMER_DISABLE_INTERRUPTS_FUNCTION_ID,
	C66XX_UART_INIT_FUNCTION_ID,
	C66XX_I2C_INIT_FUNCTION_ID,
	C66XX_I2C_WRITE_DATA_FUNCTION_ID,
	C66XX_I2C_READ_DATA_FUNCTION_ID,
	C66XX_GBE_INIT_SERDES_FUNCTION_ID,
	C66XX_GBE_INIT_SGMII_FUNCTION_ID,
	C66XX_SRIO_INIT_FUNCTION_ID,
	C66XX_SRIO_MAP_TX_QUEUE_FUNCTION_ID,
	C66XX_SRIO_ROUTE_DOORBELL_INTERRUPT_FUNCTION_ID,
	C66XX_NUMBER_OF_FUNCTIONS
};
/** @}*/
//=============================================================================



#ifdef __cplusplus
extern "C" {
#endif


//=============================================================================
//============ General SDK API functions declarations =========================
//=============================================================================

/** @addtogroup  C66XX_GENERAL_FUNCTIONS  General DSP SDK API functions
 *  @{
 */

/*------------ C66XX_get_last_error() function ---------------------------*//**
 * @brief  Function returns error code of last executed API function
 *
 * @return  Error code of last executed API function
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_get_last_error(void);


/*------------ C66XX_get_error_flag() function ---------------------------*//**
 * @brief  Function returns status of ERROR_FLAG for API functions
 *
 * ERROR_FLAG is set by each API function in case error has been detected.
 * ERROR_FLAG can be reset by C66XX_clear_error_flag() API function
 *
 * @param[out]  error_code - pointer to a variable to receive the error code in
 *					case error has been detected. In case pointer is NULL, then
 *					no error code is returned;
 * @param[out]  error_function_id - pointer to a variable to receive ID of the
 *					first API function, which has returned with error. In case
 *					pointer is NULL, then no function ID is returned;
 *
 * @return  Status of ERROR_FLAG: ON or OFF
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_get_error_flag(int32_t *error_code, int32_t *error_function_id);


/*------------ C66XX_clear_error_flag() function -------------------------*//**
 * @brief  Function clear ERROR_FLAG for API functions
 *
 * @return  Always OK
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_clear_error_flag(void);


/*------------ C66XX_get_error_message() function ------------------------*//**
 * @brief  Function returns the text interpretation of the error code.
 *
 * @param[in]  error - error code to be interpreted;
 * @param[out]  error_message - pointer to the returned string. In case pointer
 *				is NULL, then no text is filled in;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_get_error_message(int32_t error, char *error_message);


/*------------ C66XX_get_function_name() function ------------------------*//**
 * @brief  Function returns the text interpretation of API function.
 *
 * @param[in]  function_id - ID of the API function to be interpreted;
 * @param[out]  function_name - pointer to the returned string. In case pointer
 *				is NULL, then no text is filled in;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_get_function_name(int32_t function_id, char *function_name);


/** @}*/
//=============================================================================



//=============================================================================
//============ System functions ===============================================
//=============================================================================

/** @addtogroup  C66XX_SYS  System functions
 *  @{
 */


/** DSP core reset source is global reset */
#define C66XX_DSP_CORE_RESET_SRC_IS_GLOBAL_RESET			1
/** DSP core reset source is local reset */
#define C66XX_DSP_CORE_RESET_SRC_IS_LOCAL_RESET				2


/** Main PLL Multiplier bitmask */
#define C66XX_MAIN_PLL_PLLM_BITMASK							0x1fff
/** Main PLL Divider bitmask */
#define C66XX_MAIN_PLL_PLLD_BITMASK							0x3f
/** Number of PLL dividers */
#define C66XX_MAIN_PLL_PLLDIV_COUNT							16

/** Main PLL divider data descriptor */
typedef struct
{
	uint32_t					enable;									/**< PLL divider enable flag */
	uint32_t					ratio;									/**< PLL divider value (8-bit) */
} C66XX_MAIN_PLL_PLLDIV_DD;

// Main PLL divider data descriptor length in bytes
#define C66XX_MAIN_PLL_PLLDIV_DD_LEN						sizeof(C66XX_MAIN_PLL_PLLDIV_DD)


/** Main PLL data descriptor (contains parameters which are used to init PLL) */
typedef struct
{
	uint32_t					bypass;									/**< PLL mode: bypass or not */
	uint32_t					pllm;									/**< PLL multiplier value (13-bit) */
	uint32_t					plld;									/**< PLL divider value (6-bit) */
	uint32_t					output_divide;							/**< PLL output divider value (4-bit) */
	C66XX_MAIN_PLL_PLLDIV_DD	plldiv[C66XX_MAIN_PLL_PLLDIV_COUNT];	/**< PLL dividers */
} C66XX_MAIN_PLL_DD;

// Main PLL data descriptor length in bytes
#define C66XX_MAIN_PLL_DD_LEN								sizeof(C66XX_MAIN_PLL_DD)


/** DDR3 PLL data descriptor (contains parameters which are used to init DDR3 PLL) */
typedef struct
{
	uint32_t					pllm;									/**< PLL multiplier value (13-bit) */
	uint32_t					plld;									/**< PLL divider value (6-bit) */
} C66XX_DDR3_PLL_DD;

// DDR3 PLL data descriptor length in bytes
#define C66XX_DDR3_PLL_DD_LEN								sizeof(C66XX_DDR3_PLL_DD)


/** PASS PLL data descriptor (contains parameters which are used to init PASS PLL) */
typedef struct
{
	uint32_t					pllm;									/**< PLL multiplier value (13-bit) */
	uint32_t					plld;									/**< PLL divider value (6-bit) */
} C66XX_PASS_PLL_DD;

// PASS PLL data descriptor length in bytes
#define C66XX_PASS_PLL_DD_LEN								sizeof(C66XX_PASS_PLL_DD)


/*------------ C66XX_SYS_get_core_number() function ----------------------*//**
 * @brief  Function returns DSP core number at which the program is running
 *
 * @return  DSP core number
 *
-----------------------------------------------------------------------------*/
uint32_t C66XX_SYS_get_core_number(void);


/*------------ C66XX_SYS_get_max_core_freq() function --------------------*//**
 * @brief  Function returns DSP core max speed in MHz
 *
 * @return  DSP core max speed in MHz
 *
-----------------------------------------------------------------------------*/
uint32_t C66XX_SYS_get_max_core_freq(void);


/*------------ C66XX_SYS_set_core_reset_state() function -----------------*//**
 * @brief  Function sets or releases DSP core local reset
 *
 * @param[in]  core - DSP core number to set or release from local reset
 * @param[in]  state - DSP core local reset state: ON or OFF
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_SYS_set_core_reset_state(uint32_t core, uint32_t state);


/*------------ C66XX_SYS_get_core_reset_state() function -----------------*//**
 * @brief  Function returns DSP core local reset state
 *
 * @param[in]  core - DSP core number to return local reset state
 * @param[out]  state - pointer to a variable to receive DSP core local reset
 *					state: ON or OFF
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_SYS_get_core_reset_state(uint32_t core, uint32_t *state);


/*------------ C66XX_SYS_get_core_reset_source() function ----------------*//**
 * @brief  Function returns DSP core reset source: global or local
 *
 * @param[in]  core - DSP core number to return reset source
 * @param[out]  state - pointer to a variable to receive DSP core reset source:
 *					global or local
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_SYS_get_core_reset_source(uint32_t core, uint32_t *source);


/*------------ C66XX_SYS_set_core_boot_address() function ----------------*//**
 * @brief  Function sets DSP core boot address. After releasing DSP core from
 * reset it will start execution from this memory address.
 * Note that DSP core boot address should be 1024 bytes aligned
 *
 * @param[in]  core - DSP core number to set boot address
 * @param[in]  addr - DSP core boot address: should be 1024 bytes aligned
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_SYS_set_core_boot_address(uint32_t core, uint32_t addr);


/*------------ C66XX_SYS_init_main_pll() function ------------------------*//**
 * @brief  Function inits DSP Main PLL according to supplied parameters
 *
 * @param[in]  pll_dd - Main PLL data descriptor
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_SYS_init_main_pll(C66XX_MAIN_PLL_DD *pll_dd);


/*------------ C66XX_SYS_get_main_pll_settings() function ----------------*//**
 * @brief  Function returns DSP Main PLL settings
 *
 * @param[out]  pll_dd - Pointer to a main PLL data descriptor to receive data
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_SYS_get_main_pll_settings(C66XX_MAIN_PLL_DD *pll_dd);


/*------------ C66XX_SYS_init_ddr3_pll() function ------------------------*//**
 * @brief  Function inits DSP DDR3 PLL according to supplied parameters
 *
 * @param[in]  ddr3_pll_dd - DDR3 PLL data descriptor
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_SYS_init_ddr3_pll(C66XX_DDR3_PLL_DD *ddr3_pll_dd);


/*------------ C66XX_SYS_get_ddr3_pll_settings() function ----------------*//**
 * @brief  Function returns DSP DDR3 PLL settings
 *
 * @param[out]  ddr3_pll_dd - Pointer to DDR3 PLL data descriptor to receive
*				data
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_SYS_get_ddr3_pll_settings(C66XX_DDR3_PLL_DD *ddr3_pll_dd);


/*------------ C66XX_SYS_init_pass_pll() function ------------------------*//**
 * @brief  Function inits DSP PASS PLL according to supplied parameters
 *
 * @param[in]  pass_pll_dd - PASS PLL data descriptor
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_SYS_init_pass_pll(C66XX_PASS_PLL_DD *pass_pll_dd);


/*------------ C66XX_SYS_get_pass_pll_settings() function ----------------*//**
 * @brief  Function returns DSP PASS PLL settings
 *
 * @param[out]  pass_pll_dd - Pointer to PASS PLL data descriptor to receive
*				data
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_SYS_get_pass_pll_settings(C66XX_PASS_PLL_DD *pass_pll_dd);


/*------------ C66XX_SYS_enable_power_domain() function ------------------*//**
 * @brief  Function powers up selected power domain
 *
 * @param[in]  domain - power domain number to enable
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_SYS_enable_power_domain(uint32_t domain);


/** @}*/
//=============================================================================


//=============================================================================
//============ DSP memory controller functions ================================
//=============================================================================

/** @addtogroup  C66XX_MEM  DSP memory controller functions
 *  @{
 */


/** DSP L1 (P or D) cache sizes */
#define C66XX_CACHE_L1_0KB									CACHE_L1_0KCACHE
#define C66XX_CACHE_L1_4KB									CACHE_L1_4KCACHE
#define C66XX_CACHE_L1_8KB									CACHE_L1_8KCACHE
#define C66XX_CACHE_L1_16KB									CACHE_L1_16KCACHE
#define C66XX_CACHE_L1_32KB									CACHE_L1_32KCACHE
#define C66XX_CACHE_L1_MAX_SIZE								CACHE_L1_MAXIM3


/** DSP L2 cache sizes */
#define C66XX_CACHE_L2_0KB									CACHE_0KCACHE
#define C66XX_CACHE_L2_32KB									CACHE_32KCACHE
#define C66XX_CACHE_L2_64KB									CACHE_64KCACHE
#define C66XX_CACHE_L2_128KB								CACHE_128KCACHE
#define C66XX_CACHE_L2_256KB								CACHE_256KCACHE
#define C66XX_CACHE_L2_512KB								CACHE_512KCACHE
#define C66XX_CACHE_L2_MAX_SIZE								C66XX_CACHE_L2_512KB


/** DDR3 controller initialization data descriptor (contains parameters which are used to init DDR3 controller) */
typedef struct
{
	EMIF4F_TIMING1_CONFIG		timing1_config;						/**< DDR3 controller Timing 1 configuration */
	EMIF4F_TIMING2_CONFIG		timing2_config;						/**< DDR3 controller Timing 2 configuration */
	EMIF4F_TIMING3_CONFIG		timing3_config;						/**< DDR3 controller Timing 3 configuration */
	EMIF4F_SDRAM_CONFIG			sdram_config;						/**< DDR3 controller SDRAM configuration */
} C66XX_DDR3_EMIF_DD;

// DDR3 controller initialization data descriptor length in bytes
#define C66XX_DDR3_EMIF_DD_LEN								sizeof(C66XX_DDR3_EMIF_DD)


/** DDR3 controller physical interface (PHY) data descriptor (contains parameters which are used to perform leveling) */
// All these values should be obtained from DDR3 PHY Calc spreadsheet (sprabl2) provided by TI
typedef struct
{
	uint32_t					data0_wrlvl_init_ratio;				/**< DDR3 byte lane 7 write leveling initialization ratio */
	uint32_t					data1_wrlvl_init_ratio;				/**< DDR3 byte lane 6 write leveling initialization ratio */
	uint32_t					data2_wrlvl_init_ratio;				/**< DDR3 byte lane 5 write leveling initialization ratio */
	uint32_t					data3_wrlvl_init_ratio;				/**< DDR3 byte lane 4 write leveling initialization ratio */
	uint32_t					data4_wrlvl_init_ratio;				/**< DDR3 byte lane 3 write leveling initialization ratio */
	uint32_t					data5_wrlvl_init_ratio;				/**< DDR3 byte lane 2 write leveling initialization ratio */
	uint32_t					data6_wrlvl_init_ratio;				/**< DDR3 byte lane 1 write leveling initialization ratio */
	uint32_t					data7_wrlvl_init_ratio;				/**< DDR3 byte lane 0 write leveling initialization ratio */
	uint32_t					data8_wrlvl_init_ratio;				/**< DDR3 ECC byte lane write leveling initialization ratio */
	uint32_t					data0_gatelvl_init_ratio;			/**< DDR3 byte lane 7 gate leveling initialization ratio */
	uint32_t					data1_gatelvl_init_ratio;			/**< DDR3 byte lane 6 gate leveling initialization ratio */
	uint32_t					data2_gatelvl_init_ratio;			/**< DDR3 byte lane 5 gate leveling initialization ratio */
	uint32_t					data3_gatelvl_init_ratio;			/**< DDR3 byte lane 4 gate leveling initialization ratio */
	uint32_t					data4_gatelvl_init_ratio;			/**< DDR3 byte lane 3 gate leveling initialization ratio */
	uint32_t					data5_gatelvl_init_ratio;			/**< DDR3 byte lane 2 gate leveling initialization ratio */
	uint32_t					data6_gatelvl_init_ratio;			/**< DDR3 byte lane 1 gate leveling initialization ratio */
	uint32_t					data7_gatelvl_init_ratio;			/**< DDR3 byte lane 0 gate leveling initialization ratio */
	uint32_t					data8_gatelvl_init_ratio;			/**< DDR3 ECC byte lane gate leveling initialization ratio */
	uint32_t					ddr3_config_reg_12_bitmask;			/**< DDR3 configuration 12 register bitmask for INVERT_CLK_OUT field */
} C66XX_DDR3_PHY_DD;

// DDR3 controller initialization data descriptor length in bytes
#define C66XX_DDR3_PHY_DD_LEN								sizeof(C66XX_DDR3_PHY_DD)


/*------------ C66XX_MEM_set_L1P_cache_size() function -------------------*//**
 * @brief  Function sets DSP L1P cache to the new size
 *
 * @param[in]  new_size - new size of DSP L1P cache
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void C66XX_MEM_set_L1P_cache_size(uint32_t new_size);


/*------------ C66XX_MEM_get_L1P_cache_size() function -------------------*//**
 * @brief  Function returns DSP L1P cache size
 *
 * @return  DSP L1P cache size
 *
-----------------------------------------------------------------------------*/
#define C66XX_MEM_get_L1P_cache_size()						CACHE_getL1PSize()


/*------------ C66XX_MEM_set_L1D_cache_size() function -------------------*//**
 * @brief  Function sets DSP L1D cache to the new size
 *
 * @param[in]  new_size - new size of DSP L1D cache
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void C66XX_MEM_set_L1D_cache_size(uint32_t new_size);


/*------------ C66XX_MEM_get_L1D_cache_size() function -------------------*//**
 * @brief  Function returns DSP L1D cache size
 *
 * @return  DSP L1D cache size
 *
-----------------------------------------------------------------------------*/
#define C66XX_MEM_get_L1D_cache_size()						CACHE_getL1DSize()


/*------------ C66XX_MEM_set_L2_cache_size() function --------------------*//**
 * @brief  Function sets DSP L2 cache to the new size
 *
 * @param[in]  new_size - new size of DSP L2 cache
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void C66XX_MEM_set_L2_cache_size(uint32_t new_size);


/*------------ C66XX_MEM_get_L2_cache_size() function --------------------*//**
 * @brief  Function returns DSP L2 cache size
 *
 * @return  DSP L2 cache size
 *
-----------------------------------------------------------------------------*/
#define C66XX_MEM_get_L2_cache_size()						CACHE_getL2Size()


/*------------ C66XX_MEM_set_memory_region_cache_config() function -------*//**
 * @brief  Function enables or disables DSP caching for specified memory region
 *
 * @param[in]  mar - MAR register number from 0 to 255 (note that the first 12
 *				memory regions are read-only)
 * @param[in]  cacheable - cache mode to set: ON - enable caching,
 *				OFF - disable caching
 * @param[in]  prefetchable - DSP XMC controller prefetch mode to set:
 *				ON - enable prefetch support, OFF - disable prefetch support
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
#define C66XX_MEM_set_memory_region_cache_config(mar, cacheable, prefetchable)		CACHE_setMemRegionInfo(mar, cacheable, prefetchable)


/*------------ C66XX_MEM_get_memory_region_cache_config() function -------*//**
 * @brief  Function returns DSP cache configuration for specified memory region
 *
 * @param[in]  mar - MAR register number from 0 to 255 (note that the first 12
 *				memory regions are read-only)
 * @param[in]  cacheable - pointer to a variable to receive current cache mode:
 *				ON - caching is enabled, OFF - caching is disabled
 * @param[in]  prefetchable - pointer to a variable to receive current DSP XMC
 *				controller prefetch mode: ON - prefetch support is enabled,
 *				OFF - prefetch support is disabled
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
#define C66XX_MEM_get_memory_region_cache_config(mar, cacheable, prefetchable)		CACHE_getMemRegionInfo(mar, cacheable, prefetchable)


/*------------ C66XX_MEM_init_xmc_mpax_segment() function ----------------*//**
 * @brief  Function configures XMC MPAX segment according to supplied
 * parameters
 *
 * @param[in]  index - MPAX segment index from 0 to 15 (note that the first 2
 *				segments are already configured by default)
 * @param[in]  baddr - upper 20 bit of base 32-bit address to remap
 * @param[in]  size - encoded segment size
 * @param[in]  raddr - upper 24 bit of replacement 36-bit address
 * @param[in]  perm - access types allowed in this segment
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_MEM_init_xmc_mpax_segment(uint32_t index, uint32_t baddr, uint32_t size, uint32_t raddr, uint32_t perm);


/*------------ C66XX_MEM_convert_local_to_global_address() function ------*//**
 * @brief  Function converts local DSP core address to global address
 *
 * @param[in]  addr - local DSP core address to be converted
 *
 * @return  Global address
 *
-----------------------------------------------------------------------------*/
uint32_t C66XX_MEM_convert_local_to_global_address(uint32_t addr);


/*------------ C66XX_MEM_init_ddr3() function --------------------------------*//**
 * @brief  Function inits DDR3 controller according to supplied parameters
 *
 * @param[in]  ddr3_emif_dd - DDR3 controller initialization data descriptor
 * @param[in]  ddr3_phy_dd - DDR3 PHY controller initialization data descriptor
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_MEM_init_ddr3(C66XX_DDR3_EMIF_DD *ddr3_emif_dd, C66XX_DDR3_PHY_DD *ddr3_phy_dd);

/** @}*/
//=============================================================================



//=============================================================================
//============ Timer TSC functions ============================================
//=============================================================================

/** @addtogroup  C66XX_TSC  DSP 64-bit Time Stamp Counter (TSC) functions
 *  @{
 */

/*------------ C66XX_TSC_get_current_value() function --------------------*//**
 * @brief  Function returns DSP TSC current value
 *
 * @return  64-bit Time Stamp Counter value
 *
-----------------------------------------------------------------------------*/
uint64_t C66XX_TSC_get_current_value(void);


/*------------ C66XX_TSC_get_duration_s() function -----------------------*//**
 * @brief  Function returns time duration in seconds from Time Stamp Counter
 * start value
 *
 * @param[in]  start - 64-bit Time Stamp Counter value
 *
 * @return  Time duration in seconds
 *
-----------------------------------------------------------------------------*/
double C66XX_TSC_get_duration_s(uint64_t start);


/*------------ C66XX_TSC_set_delay_us() function -------------------------*//**
 * @brief  Function is used to delay the execution for selected number of
 * microseconds
 *
 * @param[in]  us - Number of microseconds to delay
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void C66XX_TSC_set_delay_us(uint32_t us);

/** @}*/
//=============================================================================



//=============================================================================
//============ C66xx interrupt controller functions ===========================
//=============================================================================

/** @addtogroup  C66XX_INT  C66xx interrupt controller functions
 *  @{
 */


/** DSP interrupt vector IDs */
#define C66XX_DSP_VECTID_NMI								CSL_INTC_VECTID_NMI
#define C66XX_DSP_VECTID_4									CSL_INTC_VECTID_4
#define C66XX_DSP_VECTID_5									CSL_INTC_VECTID_5
#define C66XX_DSP_VECTID_6									CSL_INTC_VECTID_6
#define C66XX_DSP_VECTID_7									CSL_INTC_VECTID_7
#define C66XX_DSP_VECTID_8									CSL_INTC_VECTID_8
#define C66XX_DSP_VECTID_9									CSL_INTC_VECTID_9
#define C66XX_DSP_VECTID_10									CSL_INTC_VECTID_10
#define C66XX_DSP_VECTID_11									CSL_INTC_VECTID_11
#define C66XX_DSP_VECTID_12									CSL_INTC_VECTID_12
#define C66XX_DSP_VECTID_13									CSL_INTC_VECTID_13
#define C66XX_DSP_VECTID_14									CSL_INTC_VECTID_14
#define C66XX_DSP_VECTID_15									CSL_INTC_VECTID_15
#define C66XX_DSP_VECTID_COMBINE							CSL_INTC_VECTID_COMBINE
#define C66XX_DSP_VECTID_EXCEP								CSL_INTC_VECTID_EXCEP


/*------------ C66XX_INT_init_core() function ----------------------------*//**
 * @brief  Function initializes C66x CorePac interrupt controller (INTC) using
 * CSL library.
 *
 * If SYS/BIOS RTOS is used, then it's recommended to use the relevant
 * SYS/BIOS interrupt API (HWI, EventCombiner and CpIntc), as there will be
 * conflicts since both CSL and SYS/BIOS will use their own Interrupt Service
 * Table Pointer (ISTP).
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_INT_init_core(void);


/*------------ C66XX_INT_map_core_event_handler() function ---------------*//**
 * @brief  Function maps DSP interrupt vector to INTC input event ID, plugs
 * the specified function as DSP interrupt handler function, and enables DSP
 * interrupt
 *
 * @param[in]  dsp_vector_id - DSP interrupt vector
 * @param[in]  input_event_id - INTC input event ID
 * @param[in]  handler - pointer to DSP interrupt handler function
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_INT_map_core_event_handler(uint32_t dsp_vector_id, uint32_t input_event_id, CSL_IntcEventHandler handler);


/*------------ C66XX_INT_unmap_core_event_handler() function -------------*//**
 * @brief  Function unmaps DSP interrupt vector and disables the corresponding
 * DSP interrupt.
 *
 * @param[in]  dsp_vector_id - DSP interrupt vector
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_INT_unmap_core_event_handler(uint32_t dsp_vector_id);


/*------------ C66XX_INT_set_core_dsp_interrupt_handler() function -------*//**
 * @brief  Function sets the specified function as a direct handler for DSP
 * interrupt vector ID.
 *
 * This handler function will be branched to from DSP interrupt vector table
 * for specified DSP interrupt vector ID, so either it should be declared in C
 * with "interrupt" keyword or manually save and restore interrupt context and
 * return with "B IRP" assembler instruction.
 * The specified DSP interrupt will be enabled too.
 *
 * @param[in]  dsp_vector_id - DSP interrupt vector
 * @param[in]  isr_handler - direct handler function for DSP interrupt vector
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_INT_set_core_dsp_interrupt_handler(uint32_t dsp_vector_id, void *isr_handler);


/** Chip interrupt controllers defs */
#define C66XX_CPINTC_ID_0									0
#define C66XX_CPINTC_ID_1									1
#define C66XX_CPINTC_ID_2									2
#define C66XX_CPINTC_ID_3									3


/*------------ C66XX_INT_init_chip() function ----------------------------*//**
 * @brief  Function initializes the specified chip interrupt controller
 * (CPINTC or CIC) and returns a handle which should be used in all subsequent
 * CPINTC function calls.
 *
 * @param[in]  cpintc_id - chip interrupt controller number to initialize (0-3)
 *
 * @return  Handle to the CPINTC instance: >0 - OK,
 *											0 - error is occurred
 *
-----------------------------------------------------------------------------*/
uint32_t C66XX_INT_init_chip(uint32_t cpintc_id);


/*------------ C66XX_INT_map_chip_system_to_host_event() function --------*//**
 * @brief  Function maps chip-level event (system event) to CPINTC output event
 * (host event) end enables it.
 *
 * System events are those events generated by a hardware module in the system.
 * These events are inputs into CPINTC.
 * Host events are the output events of CPINTC, which act as event inputs to 
 * C66x CorePac interrupt controllers (INTC).
 *
 * @param[in]  cpintc_handle - chip interrupt controller handle returned by
 *					C66XX_init_cpintc() function call;
 * @param[in]  system_event_id - chip-level (system) event from a hardware
 *					module in the system.
 * @param[in]  host_event_id - CPINTC output event ID
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_INT_map_chip_system_to_host_event(uint32_t cpintc_handle, uint32_t system_event_id, uint32_t host_event_id);


/*------------ C66XX_INT_enable_chip_host_event() function ---------------*//**
 * @brief  Function enables CPINTC output event (host event).
 *
 * @param[in]  cpintc_handle - chip interrupt controller handle returned by
 *					C66XX_init_cpintc() function call;
 * @param[in]  host_event_id - CPINTC output event ID to enable
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
#define C66XX_INT_enable_chip_host_event(cpintc_handle, host_event_id)			CSL_CPINTC_enableHostInterrupt(cpintc_handle, host_event_id)


/*------------ C66XX_INT_disable_chip_host_event() function --------------*//**
 * @brief  Function disables CPINTC output event (host event).
 *
 * @param[in]  cpintc_handle - chip interrupt controller handle returned by
 *					C66XX_init_cpintc() function call;
 * @param[in]  host_event_id - CPINTC output event ID to disable
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
#define C66XX_INT_disable_chip_host_event(cpintc_handle, host_event_id)			CSL_CPINTC_disableHostInterrupt(cpintc_handle, host_event_id)


/*------------ C66XX_INT_clear_chip_system_event() function --------------*//**
 * @brief  Function clears CPINTC input event (system event).
 *
 * @param[in]  cpintc_handle - chip interrupt controller handle returned by
 *					C66XX_init_cpintc() function call;
 * @param[in]  sys_event_id - CPINTC input event ID to clear
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
#define C66XX_INT_clear_chip_system_event(cpintc_handle, sys_event_id)			CSL_CPINTC_clearSysInterrupt(cpintc_handle, sys_event_id)


/** @}*/
//=============================================================================



//=============================================================================
//============ General purpose I/O (GPIO) functions ===========================
//=============================================================================

/** @addtogroup  C66XX_GPIO  DSP GPIO functions
 *  @{
 */

/** GPIO pins number defs */
enum C66XX_GPIO_PINS
{
	C66XX_GPIO_PIN_0 = 0,
	C66XX_GPIO_PIN_1,
	C66XX_GPIO_PIN_2,
	C66XX_GPIO_PIN_3,
	C66XX_GPIO_PIN_4,
	C66XX_GPIO_PIN_5,
	C66XX_GPIO_PIN_6,
	C66XX_GPIO_PIN_7,
	C66XX_GPIO_PIN_8,
	C66XX_GPIO_PIN_9,
	C66XX_GPIO_PIN_10,
	C66XX_GPIO_PIN_11,
	C66XX_GPIO_PIN_12,
	C66XX_GPIO_PIN_13,
	C66XX_GPIO_PIN_14,
	C66XX_GPIO_PIN_15
};

/** GPIO pins bitmask defs */
enum C66XX_GPIO_PIN_BITMASKS
{
	C66XX_GPIO_PIN_0_BITMASK = 0x0001,
	C66XX_GPIO_PIN_1_BITMASK = 0x0002,
	C66XX_GPIO_PIN_2_BITMASK = 0x0004,
	C66XX_GPIO_PIN_3_BITMASK = 0x0008,
	C66XX_GPIO_PIN_4_BITMASK = 0x0010,
	C66XX_GPIO_PIN_5_BITMASK = 0x0020,
	C66XX_GPIO_PIN_6_BITMASK = 0x0040,
	C66XX_GPIO_PIN_7_BITMASK = 0x0080,
	C66XX_GPIO_PIN_8_BITMASK = 0x0100,
	C66XX_GPIO_PIN_9_BITMASK = 0x0200,
	C66XX_GPIO_PIN_10_BITMASK = 0x0400,
	C66XX_GPIO_PIN_11_BITMASK = 0x0800,
	C66XX_GPIO_PIN_12_BITMASK = 0x1000,
	C66XX_GPIO_PIN_13_BITMASK = 0x2000,
	C66XX_GPIO_PIN_14_BITMASK = 0x4000,
	C66XX_GPIO_PIN_15_BITMASK = 0x8000
};


/** GPIO bank number for CSL utilities */
#define C66XX_GPIO_BANK_NUMBER								0
/** GPIO data definitions */
#define C66XX_GPIO_DATA_OFF									0
#define C66XX_GPIO_DATA_ON									1
/** GPIO direction definitions */
#define C66XX_GPIO_DIR_OUT									0
#define C66XX_GPIO_DIR_IN									1
/** GPIO pin edge defs */
#define C66XX_GPIO_PIN_EDGE_RISING							0
#define C66XX_GPIO_PIN_EDGE_FALLING							1
#define C66XX_GPIO_PIN_EDGE_ANY								2
/** GPIO pins bitmask definitions */
#define C66XX_GPIO_DATA_BITMASK								0x0000ffff


/*------------ C66XX_GPIO_init() function --------------------------------*//**
 * @brief  Function inits the GPIO peripheral: all pins are configured as
 * inputs, interrupts are disabled
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_GPIO_init(void);


/*------------ C66XX_GPIO_set_pin_direction() function -------------------*//**
 * @brief  Function configures the specified GPIO pin direction
 *
 * @param[in]  pin_number - GPIO pin number to configure
 * @param[in]  direction - GPIO direction definition
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void C66XX_GPIO_set_pin_direction(uint32_t pin_number, uint32_t direction);


/*------------ C66XX_GPIO_get_pin_direction() function -------------------*//**
 * @brief  Function returns the specified GPIO pin direction
 *
 * @param[in]  pin_number - GPIO pin number to get direction
 *
 * @return  GPIO direction definition
 *
-----------------------------------------------------------------------------*/
uint32_t C66XX_GPIO_get_pin_direction(uint32_t pin_number);


/*------------ C66XX_GPIO_set_pin_data() function ------------------------*//**
 * @brief  Function sets the specified GPIO pin state to data
 *
 * @param[in]  pin_number - GPIO pin number to configure
 * @param[in]  data - GPIO pin state: ON or OFF
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void C66XX_GPIO_set_pin_data(uint32_t pin_number, uint32_t data);


/*------------ C66XX_GPIO_get_pin_data() function ------------------------*//**
 * @brief  Function returns the specified GPIO pin state
 *
 * @param[in]  pin_number - GPIO pin number to get pin state
 *
 * @return  GPIO pin state: ON or OFF
 *
-----------------------------------------------------------------------------*/
uint32_t C66XX_GPIO_get_pin_data(uint32_t pin_number);


/*------------ C66XX_GPIO_set_direction() function -----------------------*//**
 * @brief  Function configures the specified GPIO pins direction
 *
 * @param[in]  pin_bitmask - GPIO pins (ORed) bitmask to configure
 * @param[in]  direction - GPIO direction definition
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void C66XX_GPIO_set_direction(uint32_t pin_bitmask, uint32_t direction);


/*------------ C66XX_GPIO_get_direction() function -----------------------*//**
 * @brief  Function returns the specified GPIO pins direction
 *
 * @param[in]  pin_bitmask - GPIO pins (ORed) bitmask to get pins direction
 *
 * @return  Specified GPIO pins direction
 *
-----------------------------------------------------------------------------*/
uint32_t C66XX_GPIO_get_direction(uint32_t pin_bitmask);


/*------------ C66XX_GPIO_set_data() function ----------------------------*//**
 * @brief  Function sets the specified GPIO pins state to data
 *
 * @param[in]  pin_bitmask - GPIO pins (ORed) bitmask to configure
 * @param[in]  data - GPIO pins state: ON or OFF
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void C66XX_GPIO_set_data(uint32_t pin_bitmask, uint32_t data);


/*------------ C66XX_GPIO_get_data() function ----------------------------*//**
 * @brief  Function returns the specified GPIO pins state
 *
 * @param[in]  pin_bitmask - GPIO pins (ORed) bitmask to get pins state
 *
 * @return  Specified GPIO pins state
 *
-----------------------------------------------------------------------------*/
uint32_t C66XX_GPIO_get_data(uint32_t pin_bitmask);


/*------------ C66XX_GPIO_enable_interrupts() function -------------------*//**
 * @brief  Function enables GPIO peripheral interrupts
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void C66XX_GPIO_enable_interrupts(void);


/*------------ C66XX_GPIO_disable_interrupts() function ------------------*//**
 * @brief  Function disables GPIO peripheral interrupts
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void C66XX_GPIO_disable_interrupts(void);


/*------------ C66XX_GPIO_enable_edge_interrupt() function ---------------*//**
 * @brief  Function enables the specified GPIO pin edge interrupt
 *
 * @param[in]  pin - GPIO pin number to configure
 * @param[in]  edge - GPIO pin edge which triggers GPIO interrupt
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void C66XX_GPIO_enable_edge_interrupt(uint32_t pin, uint32_t edge);


/*------------ C66XX_GPIO_disable_edge_interrupt() function --------------*//**
 * @brief  Function disables the specified GPIO pin edge interrupt
 *
 * @param[in]  pin - GPIO pin number to configure
 * @param[in]  edge - GPIO pin edge which shouldn't trigger GPIO interrupt
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void C66XX_GPIO_disable_edge_interrupt(uint32_t pin, uint32_t edge);


/** @}*/
//=============================================================================



//=============================================================================
//============ Timer module functions =========================================
//=============================================================================

/** @addtogroup  C66XX_TIMER  DSP 64-bit Timer module functions
 *  @{
 */


/** DSP 64-bit Timer module number defs */
typedef enum
{
	C66XX_TIMER_0 = 0,
	C66XX_TIMER_1,
	C66XX_TIMER_2,
	C66XX_TIMER_3,
	C66XX_TIMER_4,
	C66XX_TIMER_5,
	C66XX_TIMER_6,
	C66XX_TIMER_7,
	C66XX_TIMER_8,
	C66XX_TIMER_9,
	C66XX_TIMER_10,
	C66XX_TIMER_11,
	C66XX_TIMER_12,
	C66XX_TIMER_13,
	C66XX_TIMER_14,
	C66XX_TIMER_15
} C66XX_TIMER;


/** DSP 64-bit Timer mode defs */
typedef enum
{
	C66XX_TIMER_MODE_64BIT_GPT = 0,
	C66XX_TIMER_MODE_32BIT_UNCHAINED,
	C66XX_TIMER_MODE_64BIT_WDT,
	C66XX_TIMER_MODE_32BIT_CHAINED
} C66XX_TIMER_MODE;


/** DSP 64-bit Timer hardware configuration defs */
typedef enum
{
	C66XX_TIMER_HW_CFG_64BIT = 0,
	C66XX_TIMER_HW_CFG_32BIT_LOW,
	C66XX_TIMER_HW_CFG_32BIT_HIGH
} C66XX_TIMER_HW_CFG;


/** DSP 64-bit Timer count mode defs */
typedef enum
{
	C66XX_TIMER_COUNT_MODE_DISABLED = 0,
	C66XX_TIMER_COUNT_MODE_ONE_SHOT,
	C66XX_TIMER_COUNT_MODE_CONTINUOUSLY,
	C66XX_TIMER_COUNT_MODE_CONTINUOUSLY_RELOAD
} C66XX_TIMER_COUNT_MODE;


/** DSP timer input enable defs */
typedef enum
{
	C66XX_TIMER_CLK_INPUT_DISABLED = 0,
	C66XX_TIMER_CLK_INPUT_ENABLED
} C66XX_TIMER_CLK_INPUT;


/** DSP timer input inverter control defs */
typedef enum
{
	C66XX_TIMER_CLK_INPUT_INVERTER_DISABLED = 0,
	C66XX_TIMER_CLK_INPUT_INVERTER_ENABLED
} C66XX_TIMER_CLK_INPUT_INVERTER;


/** DSP timer clock source defs */
typedef enum
{
	C66XX_TIMER_CLK_SRC_INTERNAL = 0,
	C66XX_TIMER_CLK_SRC_EXTERNAL
} C66XX_TIMER_CLK_SRC;


/** DSP timer clock/pulse mode for timer output defs */
typedef enum
{
	C66XX_TIMER_CLK_OUTPUT_MODE_PULSE = 0,
	C66XX_TIMER_CLK_OUTPUT_MODE_CLK
} C66XX_TIMER_CLK_OUTPUT_MODE;


/** DSP timer pulse width used in pulse mode for timer output defs */
typedef enum
{
	C66XX_TIMER_CLK_OUTPUT_PULSE_WIDTH_1CLK = 0,
	C66XX_TIMER_CLK_OUTPUT_PULSE_WIDTH_2CLK,
	C66XX_TIMER_CLK_OUTPUT_PULSE_WIDTH_3CLK,
	C66XX_TIMER_CLK_OUTPUT_PULSE_WIDTH_4CLK
} C66XX_TIMER_CLK_OUTPUT_PULSE_WIDTH;


/** DSP timer output inverter control defs */
typedef enum
{
	C66XX_TIMER_CLK_OUTPUT_INVERTER_DISABLED = 0,
	C66XX_TIMER_CLK_OUTPUT_INVERTER_ENABLED
} C66XX_TIMER_CLK_OUTPUT_INVERTER;


/** DSP 32-bit Timer configuration data descriptor */
typedef struct
{
	C66XX_TIMER_CLK_SRC						clk_src;							/**< Clock source for 32-bit timer */
	C66XX_TIMER_CLK_INPUT					clk_input_enabled;					/**< Timer clock is gated by the timer input for 32-bit timer */
	C66XX_TIMER_CLK_INPUT_INVERTER			clk_input_inverter_enabled;			/**< An inverted timer input drives the 32-bit timer */
	C66XX_TIMER_CLK_OUTPUT_MODE				clk_src_output_mode;				/**< Clock output mode for 32-bit timer */
	C66XX_TIMER_CLK_OUTPUT_PULSE_WIDTH		clk_src_output_pulse_width;			/**< Clock output pulse width used in pulse mode for 32-bit timer */
	C66XX_TIMER_CLK_OUTPUT_INVERTER			clk_output_inverter_enabled;		/**< 32-bit timer output is inverted */
} C66XX_TIMER_32BIT_CFG_DD;


/** DSP 64-bit Timer module configuration data descriptor */
typedef struct
{
	C66XX_TIMER_MODE						timer_mode;							/**< Timer mode: 64-bit GPT, 64-bit WDT, dual 32-bit unchained or chained */
	C66XX_TIMER_32BIT_CFG_DD				timer_high;							/**< Configuration data descriptor for 32-bit HIGH timer */
	C66XX_TIMER_32BIT_CFG_DD				timer_low;							/**< Configuration data descriptor for 32-bit LOW timer */
} C66XX_TIMER_CFG_DD;

// DSP 64-bit Timer module configuration data descriptor length in bytes
#define C66XX_TIMER_CFG_DD_LEN								sizeof(C66XX_TIMER_CFG_DD)


/*------------ C66XX_TIMER_init() function -------------------------------*//**
 * @brief  Function configures selected DSP 64-bit timer module
 *
 * @param[in]  timer_number - DSP 64-bit timer module number
 * @param[in]  frequency - DSP timer output frequency in Hz
 * @param[in]  cfg_dd - pointer to filled DSP 64-bit timer module configuration
 *				 data descriptor
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_TIMER_init(C66XX_TIMER timer_number, uint32_t frequency, C66XX_TIMER_CFG_DD *cfg_dd);


/*------------ C66XX_TIMER_start() function ------------------------------*//**
 * @brief  Function starts selected DSP timer
 *
 * @param[in]  timer_number - DSP 64-bit timer module number
 * @param[in]  timer_to_start - hardware timer to start (64BIT/32BIT_LOW/
 *				32BIT_HIGH)
 * @param[in]  count_mode - DSP timer counting mode (DISABLED/ONE_SHOT/
 *				CONTINUOUSLY/CONTINUOUSLY_RELOAD)
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_TIMER_start(C66XX_TIMER timer_number, C66XX_TIMER_HW_CFG timer_to_start, C66XX_TIMER_COUNT_MODE count_mode);


/*------------ C66XX_TIMER_stop() function -------------------------------*//**
 * @brief  Function stops selected DSP timer
 *
 * @param[in]  timer_number - DSP 64-bit timer module number
 * @param[in]  timer_to_stop - hardware timer to stop (64BIT/32BIT_LOW/
 *				32BIT_HIGH)
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_TIMER_stop(C66XX_TIMER timer_number, C66XX_TIMER_HW_CFG timer_to_stop);


/*------------ C66XX_TIMER_reset() function ------------------------------*//**
 * @brief  Function resets selected DSP timer
 *
 * @param[in]  timer_number - DSP 64-bit timer module number
 * @param[in]  timer_to_reset - hardware timer to reset (64BIT/32BIT_LOW/
 *				32BIT_HIGH)
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_TIMER_reset(C66XX_TIMER timer_number, C66XX_TIMER_HW_CFG timer_to_reset);


/*------------ C66XX_TIMER_enable_interrupts() function ------------------*//**
 * @brief  Function enables interrupts from selected DSP timer
 *
 * @param[in]  timer_number - DSP 64-bit timer module number
 * @param[in]  timer_to_enable - hardware timer to enable interrupts from
 *				(64BIT/32BIT_LOW/32BIT_HIGH)
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_TIMER_enable_interrupts(C66XX_TIMER timer_number, C66XX_TIMER_HW_CFG timer_to_enable);


/*------------ C66XX_TIMER_disable_interrupts() function ------------------*//**
 * @brief  Function disables interrupts from selected DSP timer
 *
 * @param[in]  timer_number - DSP 64-bit timer module number
 * @param[in]  timer_to_disable - hardware timer to disable interrupts from
 *				(64BIT/32BIT_LOW/32BIT_HIGH)
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_TIMER_disable_interrupts(C66XX_TIMER timer_number, C66XX_TIMER_HW_CFG timer_to_disable);


/** @}*/
//=============================================================================



//=============================================================================
//============ UART functions =================================================
//=============================================================================

/** @addtogroup  C66XX_UART  DSP UART functions
 *  @{
 */

// UART baud rate defs (max 128 kBauds)
#define C66XX_UART_BAUD_RATE_2400							2400		/**< UART baud rate 2400 bps */
#define C66XX_UART_BAUD_RATE_4800							4800		/**< UART baud rate 4800 bps */
#define C66XX_UART_BAUD_RATE_9600							9600		/**< UART baud rate 9600 bps */
#define C66XX_UART_BAUD_RATE_19200							19200		/**< UART baud rate 19200 bps */
#define C66XX_UART_BAUD_RATE_38400							38400		/**< UART baud rate 38400 bps */
#define C66XX_UART_BAUD_RATE_57600							57600		/**< UART baud rate 57600 bps */
#define C66XX_UART_BAUD_RATE_115200							115200		/**< UART baud rate 115200 bps */

// UART data bits defs
#define C66XX_UART_DATA_BITS_5BITS							5			/**< UART 5-bits data words */
#define C66XX_UART_DATA_BITS_6BITS							6			/**< UART 6-bits data words */
#define C66XX_UART_DATA_BITS_7BITS							7			/**< UART 7-bits data words */
#define C66XX_UART_DATA_BITS_8BITS							8			/**< UART 8-bits data words */

// UART parity parameter defs
#define C66XX_UART_PARITY_NONE								0			/**< no parity */
#define C66XX_UART_PARITY_EVEN								1			/**< even parity */
#define C66XX_UART_PARITY_ODD								2			/**< odd parity */

// UART stop bits defs
#define C66XX_UART_STOP_BITS_1BIT							0			/**< one stop bit */
#define C66XX_UART_STOP_BITS_1_5BITS						1			/**< one and a half stop bits */
#define C66XX_UART_STOP_BITS_2BITS							2			/**< two stop bits */

// UART key defs
#define C66XX_UART_KEY_BEEP									0x7			/**< BELL value in HEX */
#define C66XX_UART_KEY_ESC									0x1b		/**< ESC value in HEX */
#define C66XX_UART_KEY_CR									0xd			/**< Carriage Return value in HEX */
#define C66XX_UART_KEY_BS									0x8			/**< Back space value in HEX */
#define C66XX_UART_LINE_LEN_MAX								80			/**< Maximum available received line length */



/*------------ C66XX_UART_init() function --------------------------------*//**
 * @brief  Function inits the UART peripheral
 *
 * @param[in]  core_clk - DSP core clock frequency in MHz;
 * @param[in]  baud_rate - desired baud rate;
 * @param[in]  data_bits - number of data bits;
 * @param[in]  parity - parity bit;
 * @param[in]  stop_bits - number of stop bits;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_UART_init(uint32_t core_clk, uint32_t baud_rate, uint32_t data_bits, uint32_t parity, uint32_t stop_bits);


/*------------ C66XX_UART_receiver_is_ready() function -------------------*//**
 * @brief  Function checks if a character is received over UART
 *
 * @return   1 - a character is received over UART,
 *			 0 - otherwise
 *
-----------------------------------------------------------------------------*/
uint32_t C66XX_UART_receiver_is_ready(void);


/*------------ C66XX_UART_receive_char() function ------------------------*//**
 * @brief  Function receives a character over UART
 *
 * @return  Received character
 *
-----------------------------------------------------------------------------*/
uint8_t C66XX_UART_receive_char(void);


/*------------ C66XX_UART_transmit_char() function -----------------------*//**
 * @brief  Function transmits a character over UART
 *
 * @param[in]  c - A character to transmit
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void C66XX_UART_transmit_char(uint8_t c);


/*------------ C66XX_UART_transmit_string() function ---------------------*//**
 * @brief  Function transmits a string until '0' character
 *
 * @param[in]  s - A pointer to the string to transmit
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void C66XX_UART_transmit_string(char *s);


/*------------ C66XX_UART_receive_line_string() function -----------------*//**
 * @brief  Function receives a line ended with CR character, and stores
 * received characters into string with '\0' symbol.
 *
 * Note that maximum received line length should not exceed 
 * C66XX_UART_LINE_LEN_MAX value!
 *
 * @param[in]  s - Pointer to a string to store received characters
 *
 * @return  Number of received characters without '\0' symbol.
 *
-----------------------------------------------------------------------------*/
uint32_t C66XX_UART_receive_line_string(char *s);


/** @}*/
//=============================================================================



//=============================================================================
//============ I2C functions ==================================================
//=============================================================================

/** @addtogroup  C66XX_I2C  DSP I2C functions
 *  @{
 */


// I2C interrupt codes
#define C66XX_I2C_INTCODE_NONE								0			/**< No interrupt */
#define C66XX_I2C_INTCODE_AL								1			/**< Arbitration-lost interrupt */
#define C66XX_I2C_INTCODE_NACK								2			/**< No-acknowledge interrupt */
#define C66XX_I2C_INTCODE_ARDY								3			/**< Register-access-ready interrupt */
#define C66XX_I2C_INTCODE_ICRRDY							4			/**< Receive-data-ready interrupt */
#define C66XX_I2C_INTCODE_ICXRDY							5			/**< Transmit-data-ready interrupt */
#define C66XX_I2C_INTCODE_SCD								6			/**< Stop-condition-detected interrupt */
#define C66XX_I2C_INTCODE_AAS								7			/**< Address-as-slave interrupt */


/*------------ C66XX_I2C_init() function ---------------------------------*//**
 * @brief  Function inits the I2C peripheral
 *
 * @param[in]  core_clk - DSP core clock frequency in MHz;
 * @param[in]  i2c_clk - I2C clock in Hz;
 * @param[in]  own_addr - I2C own slave address;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_I2C_init(uint32_t core_clk, uint32_t i2c_clk, uint32_t own_addr);


/*------------ C66XX_I2C_write_data() function ---------------------------*//**
 * @brief  Function writes data[len] to I2C chip_addr at mem_addr memory
 * address
 *
 * @param[in]  chip_addr - I2C chip address;
 * @param[in]  mem_addr - memory (register) address within the chip;
 * @param[in]  mem_addr_len - number of bytes to use for mem_addr (typically 1,
 *				2 for larger memories, 0 for register type devices with only
 *				one register);
 * @param[in]  data - pointer to data;
 * @param[in]  len - data len in bytes;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_I2C_write_data(uint8_t chip_addr, uint32_t mem_addr, uint32_t mem_addr_len, uint8_t *data, uint32_t len);


/*------------ C66XX_I2C_read_data() function ----------------------------*//**
 * @brief  Function reads data[len] from I2C chip_addr at mem_addr memory
 *
 * @param[in]  chip_addr - I2C chip address;
 * @param[in]  mem_addr - memory (register) address within the chip;
 * @param[in]  mem_addr_len - number of bytes to use for mem_addr (typically 1,
 *				2 for larger memories, 0 for register type devices with only
 *				one register);
 * @param[out]  buf - pointer to data buffer to store data;
 * @param[in]  len - data len in bytes;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_I2C_read_data(uint8_t chip_addr, uint32_t mem_addr, uint32_t mem_addr_len, uint8_t *buf, uint32_t len);


/** @}*/
//=============================================================================



//=============================================================================
//============ Gigabit Ethernet (GbE) Switch functions ========================
//=============================================================================

/** @addtogroup  C66XX_GBE  DSP Gigabit Ethernet Switch functions
 *  @{
 */

/*------------ C66XX_GBE_get_mac_id() function ---------------------------*//**
 * @brief  Function returns DSP MAC address for this device
 *
 * @return  64-bit variable that contains 48-bit MAC ID
 *
-----------------------------------------------------------------------------*/
uint64_t C66XX_GBE_get_mac_id(void);


/*------------ C66XX_GBE_init_serdes() function --------------------------*//**
 * @brief  Function inits Gigabit Ethernet Serdes block to the default state
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_GBE_init_serdes(void);


/** SGMII port Slave operation mode */
#define C66XX_SGMII_PORT_SLAVE_MODE							0
/** SGMII port Master operation mode */
#define C66XX_SGMII_PORT_MASTER_MODE						1


/** SGMII port autonegotiation disabled */
#define C66XX_SGMII_PORT_AUTONEGOTIATION_DISABLED			0
/** SGMII port autonegotiation enabled */
#define C66XX_SGMII_PORT_AUTONEGOTIATION_ENABLED			1


/*------------ C66XX_GBE_init_sgmii() function ---------------------------*//**
 * @brief  Function inits Gigabit Ethernet SGMII block
 *
 * Note that this function requires the presence of link partner connected
 * at specified SGMII port - PHY or another SGMII.
 *
 * @param[in]  port - MAC port number for which the SGMII port setup should
 *					be performed
 * @param[in]  mode - SGMII port operation mode: master or slave.
 * @param[in]  autoneg - flag to enable autonegotiation on this SGMII port.
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_GBE_init_sgmii(uint32_t port, uint32_t mode, uint32_t autoneg);


/** @}*/
//=============================================================================



//=============================================================================
//============ SRIO functions =================================================
//=============================================================================

/** @addtogroup  C66XX_SRIO  DSP SRIO functions
 *  @{
 */

/** SRIO vendor identifier */
#define C66XX_SRIO_DEVICE_VENDOR_ID							0x30
/** SRIO device revision */
#define C66XX_SRIO_DEVICE_REVISION							0x0
/** SRIO assembly identifier */
#define C66XX_SRIO_DEVICE_ASSEMBLY_ID						0x0
/** SRIO assembly vendor identifier */
#define C66XX_SRIO_DEVICE_ASSEMBLY_VENDOR_ID				C66XX_SRIO_DEVICE_VENDOR_ID
/** SRIO assembly device revision */
#define C66XX_SRIO_DEVICE_ASSEMBLY_REVISION					C66XX_SRIO_DEVICE_REVISION
/** SRIO assembly extension features */
#define C66XX_SRIO_DEVICE_ASSEMBLY_FEATURES					0x0100


/** SRIO link rate 1.25 Gbps */
#define C66XX_SRIO_LINK_RATE_1_25GB							1
/** SRIO link rate 2.5 Gbps */
#define C66XX_SRIO_LINK_RATE_2_5GB							2
/** SRIO link rate 3.125 Gbps */
#define C66XX_SRIO_LINK_RATE_3_125GB						3
/** SRIO link rate 5 Gbps */
#define C66XX_SRIO_LINK_RATE_5GB							4


// SRIO port widths defs - corresponds to SRIO path modes
/** SRIO port width with 1 receive and transmit lane */
#define C66XX_SRIO_PORT_WIDTH_1X							0
/** SRIO port width with 2 receive and transmit lanes */
#define C66XX_SRIO_PORT_WIDTH_2X							3
/** SRIO port width with 4 receive and transmit lanes */
#define C66XX_SRIO_PORT_WIDTH_4X							4


/** SRIO port 0 */
#define C66XX_SRIO_PORT_0									0
/** SRIO port 1 */
#define C66XX_SRIO_PORT_1									1
/** SRIO port 2 */
#define C66XX_SRIO_PORT_2									2
/** SRIO port 3 */
#define C66XX_SRIO_PORT_3									3


/** SRIO peripheral normal operation mode */
#define C66XX_SRIO_OPMODE_NORMAL							0
/** SRIO peripheral loopback operation mode - used to test the peripheral */
#define C66XX_SRIO_OPMODE_LOOPBACK							1


/** SRIO device identificator data descriptor */
typedef struct
{
	uint32_t					dev_id_8bit;							/**< SRIO 8-bit device identificator */
	uint32_t					dev_id_16bit;							/**< SRIO 16-bit device identificator */
} C66XX_SRIO_DEVICE_ID_DD;

// SRIO device identificator data descriptor length in bytes
#define C66XX_SRIO_DEVICE_ID_DD_LEN							sizeof(C66XX_SRIO_DEVICE_ID_DD)


/** SRIO destination device identificators number */
#define C66XX_SRIO_DESTINATION_DEVICE_ID_COUNT				4


/** SRIO messages (Type9 and Type11) transmit queues mapping data descriptor */
typedef struct
{
	uint32_t					port;									/**< SRIO port to which the queue is mapped */
	uint32_t					priority;								/**< priority bit */
} C66XX_SRIO_TX_QUEUE_SCH_DD;

// SRIO messages (Type9 and Type11) transmit queues mapping data descriptor length in bytes
#define C66XX_SRIO_TX_QUEUE_SCH_DD_LEN						sizeof(C66XX_SRIO_TX_QUEUE_SCH_DD)


/** SRIO peripheral initialization data descriptor */
typedef struct
{
	uint32_t					mode;													/**< SRIO peripheral operation mode */
	uint32_t					link_rate;												/**< SRIO link rate definition */
	uint32_t					port_width;												/**< SRIO port width definition (1x, 2x, or 4x) */
	C66XX_SRIO_DEVICE_ID_DD		src_dev_id_dd;											/**< SRIO base device identificators */
	C66XX_SRIO_DEVICE_ID_DD		dst_dev_id_dd[C66XX_SRIO_DESTINATION_DEVICE_ID_COUNT];	/**< SRIO destination device identificators */
} C66XX_SRIO_DD;

// SRIO peripheral initialization data descriptor length in bytes
#define C66XX_SRIO_DD_LEN									sizeof(C66XX_SRIO_DD)


/*------------ C66XX_SRIO_init() function --------------------------------*//**
 * @brief  Function inits the SRIO peripheral
 *
 * The function does the following:
 * 1. SRIO peripheral supports directIO, doorbell, data messaging (Type9
 *    packets) and data streaming (Type11 packets) operations.
 * 2. Max MTU length is set to 256 bytes.
 * 3. SRIO 8-bit and 16-bit base device IDs are set to supplied parameters.
 * 4. Available destination SRIO 8-bit and 16-bit device IDs are set 
 *    according to supplied parameters.
 * 5. Operation mode is set according to supplied parameter.
 * 6. Link rate and ports configuration (4 ports are available) are set
 *    according to supplied parameters.
 * 7. SRIO transmit queues used in data message (Type9 and Type11) operations
 *    are configured by default: all 16 queues send packets onto Port 0 and
 *    operate at the same minimum priority level. In case the different mapping
 *    is needed then C66XX_srio_map_tx_queue() function should be called.
 * 8. 64 SRIO doorbell interrupts are configured by default: the first 16
 *    interrupts (0-15) are routed to INTDST16 (Interrupt destination 16),
 *    16-31 - to INTDST17 (Interrupt destination 17), 32-47 - to INTDST18
 *    (Interrupt destination 18), 48-63 - to INTDST19 (Interrupt destination
 *    19). In case the different interrupt routing is needed then
 *    C66XX_srio_route_doorbell_interrupt() function should be called.
 * 9. GARBAGE queues, which can used by the TXU, are not configured.
 *
 * @param[in]  srio_dd - SRIO peripheral initialization data descriptor
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_SRIO_init(C66XX_SRIO_DD *srio_dd);


/*------------ C66XX_SRIO_map_tx_queue() function ------------------------*//**
 * @brief  Function configures the selected SRIO transmit queue according to
 * supplied parameters: SRIO port to which the queue is mapped and priority
 * bit
 *
 * @param[in]  queue - transmit queue number (0 - 15)
 * @param[in]  tx_queue_sch_dd - transmit queue mapping data descriptor
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_SRIO_map_tx_queue(uint32_t queue, C66XX_SRIO_TX_QUEUE_SCH_DD *tx_queue_sch_dd);


/*------------ C66XX_SRIO_route_doorbell_interrupt() function ------------*//**
 * @brief  Function configures the selected SRIO doorbell interrupt to
 * specified interrupt destination number
 *
 * @param[in]  doorbell - SRIO doorbell interrupt number (0 - 63)
 * @param[in]  intdst - interrupt destination number (0 - 23)
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t C66XX_SRIO_route_doorbell_interrupt(uint32_t doorbell, uint32_t intdst);


/** @}*/
//=============================================================================



#ifdef __cplusplus
}
#endif



//=============================================================================
#endif /* __C66XX_FUNCTIONS_HXX__ */
