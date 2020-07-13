/******************************************************************************
	TORNADO AMC modules Software Development Kit (SDK). Rev 4A.
	General definitions and API functions.
	(C) MicroLAB Systems, 2015-2017


	http://www.mlabsys.com
	ftp://ftp.mlabsys.com
	email: techsupport@mlabsys.com


	Description:
	------------
	This file contains general definitions and API functions for TORNADO AMC 
	modules SDK and must be included in the user C-application for
	TORNADO AMC modules.


	Revision history:
	-----------------
	rev.2A		- 2015, initial release for TORNADO-A6678 board rev.1B;
	rev.3A		- 2016:
						- added support for TORNADO-A6678/FMC board rev.1A;
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


	Copyright:
	----------
	This utility is supplied free of charge as it is without any obligation
	from MicroLAB Systems. No responsibility is assumed for any use or misuse
	of these utilities.

******************************************************************************/


/**
 * @file  TA66XX_DSP.h
 *
 * @brief  Main include file
 *
 * This file contains general definitions and API functions for TORNADO
 * AMC modules SDK
 *
 */


#ifndef __TA66XX_DSP_H__									// check for this file has been already included
#define __TA66XX_DSP_H__									1


// General defs
#define TA66XX_ON											1
#define TA66XX_OFF											0


// Include C66xx DSP defs, macros and aux functions
#include "C66XX.h"
// Include TORNADO AMC SDK functions errors
#include "TASDK_ERR.h"



//=============================================================================
/** TORNADO AMC SDK revision ID */
#define TA66XX_SDK_REVISION_ID								"4A"
//=============================================================================



//=============================================================================
//============ TORNADO AMC SDK API functions ID defs ==========================
//=============================================================================
/** @addtogroup  TA66XX_FUNCTIONS_ID  SDK API functions ID defs
 *  @{
 */


/**< Function name max size */
#define TA66XX_FUNCTION_NAME_LEN_MAX						64


enum
{
	TA66XX_GET_ERROR_MESSAGE_FUNCTION_ID = 0,
	TA66XX_GET_FUNCTION_NAME_FUNCTION_ID,
	TA66XX_BC_SET_I2C_INTERRUPT_NUMBER_FUNCTION_ID,
	TA66XX_BC_SET_UART_INTERRUPT_NUMBER_FUNCTION_ID,
	TA66XX_BC_INIT_FUNCTION_ID,
	TA66XX_BC_MAP_DSP_GPIO_INTERRUPT_FUNCTION_ID,
	TA66XX_BC_UNMAP_DSP_GPIO_INTERRUPT_FUNCTION_ID,
	TA66XX_BC_INIT_UART_FUNCTION_ID,
	TA66XX_BC_WRITE_EEPROM_FUNCTION_ID,
	TA66XX_BC_READ_EEPROM_FUNCTION_ID,
	TA66XX_BC_ERASE_EEPROM_FUNCTION_ID,
	TA66XX_BC_GET_DEVICE_INFO_FUNCTION_ID,
	TA66XX_BC_GET_HW_CFG_INFO_FUNCTION_ID,
	TA66XX_BC_GET_FMC_INFO_FUNCTION_ID,
	TA66XX_BC_GET_TEMP_STATE_FUNCTION_ID,
	TA66XX_BC_GET_POWER_STATE_FUNCTION_ID,
	TA66XX_BC_GET_FMC_POWER_STATE_FUNCTION_ID,
	TA66XX_BC_GET_SFP_INFO_FUNCTION_ID,
	TA66XX_BC_SET_FPGA_HIF_CONFIG_FUNCTION_ID,
	TA66XX_BC_GET_FPGA_HIF_CONFIG_FUNCTION_ID,
	TA66XX_BC_CLEAR_FPGA_CNF_FUNCTION_ID,
	TA66XX_BC_LOAD_FPGA_CNF_FUNCTION_ID,
	TA66XX_BC_GET_GBE_PORT_INFO_FUNCTION_ID,
	TA66XX_BC_INIT_GBE_PORT_FUNCTION_ID,
	TA66XX_BC_ERASE_FLASH_FUNCTION_ID,
	TA66XX_BC_WRITE_FLASH_FUNCTION_ID,
	TA66XX_BC_READ_FLASH_FUNCTION_ID,
	TA66XX_SC_INIT_FLASH_RECORD_LIST_FUNCTION_ID,
	TA66XX_SC_GET_NUMBER_OF_FLASH_RECORDS_FUNCTION_ID,
	TA66XX_SC_GET_FLASH_RECORD_LIST_FUNCTION_ID,
	TA66XX_SC_GET_FLASH_RECORD_INFO_FUNCTION_ID,
	TA66XX_SC_READ_FLASH_RECORD_DATA_FUNCTION_ID,
	TA66XX_SC_CREATE_FLASH_RECORD_FUNCTION_ID,
	TA66XX_SC_WRITE_FLASH_RECORD_DATA_FUNCTION_ID,
	TA66XX_SC_XCLOSE_FLASH_RECORD_FUNCTION_ID,
	TA66XX_SC_UPDATE_FLASH_RECORD_FUNCTION_ID,
	TA66XX_SC_DELETE_FLASH_RECORD_FUNCTION_ID,
	TA66XX_SC_SQUEEZE_FLASH_RECORDS_FUNCTION_ID,
	TA66XX_SC_GET_FLASH_FREE_SPACE_FUNCTION_ID,
	TA66XX_SC_GET_BOOT_SEQ_FUNCTION_ID,
	TA66XX_SC_DELETE_BOOT_SEQ_FUNCTION_ID,
	TA66XX_SC_SET_BOOT_SEQ_FUNCTION_ID,
	TA66XX_SC_LOAD_DSP_APPL_FROM_MEMORY_FUNCTION_ID,
	TA66XX_SC_LOAD_DSP_APPL_FROM_FLASH_RECORD_FUNCTION_ID,
	TA66XX_SC_LOAD_FPGA_CNF_FROM_FLASH_RECORD_FUNCTION_ID,
	TA66XX_NUMBER_OF_FUNCTIONS
};
/** @}*/
//=============================================================================



#ifdef __cplusplus
extern "C" {
#endif



//=============================================================================
//============ General SDK API functions declarations =========================
//=============================================================================

/** @addtogroup  TA66XX_GENERAL  TORNADO AMC SDK general API functions
 *  @{
 */

/*------------ TA66XX_set_error() function -------------------------------*//**
 * @brief  Function should be used on exit from SDK API functions in
 * order to set error code and function ID of SDK API functions
 *
 * @param[in]  err - error code;
 * @param[in]  function_id - function ID of SDK API functions;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_set_error(int32_t err, int32_t function_id);


/*------------ TA66XX_get_last_error() function --------------------------*//**
 * @brief  Function returns error code of last executed API function
 *
 * @return  Error code of last executed API function
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_get_last_error(void);


/*------------ TA66XX_get_error_flag() function --------------------------*//**
 * @brief  Function returns status of ERROR_FLAG for API functions
 *
 * ERROR_FLAG is set by each API function in case error has been detected.
 * ERROR_FLAG can be reset by TA66XX_clear_error_flag() API function
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
int32_t TA66XX_get_error_flag(int32_t *error_code, int32_t *error_function_id);


/*------------ TA66XX_clear_error_flag() function ------------------------*//**
 * @brief  Function clear ERROR_FLAG for API functions
 *
 * @return  Always OK
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_clear_error_flag(void);


/*------------ TA66XX_get_error_message() function -----------------------*//**
 * @brief  Function returns the text interpretation of the error code.
 *
 * @param[in]  error - error code to be interpreted;
 * @param[out]  error_message - pointer to the returned string. In case pointer
 *				is NULL, then no text is filled in;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_get_error_message(int32_t error, char *error_message);


/*------------ TA66XX_get_function_name() function -----------------------*//**
 * @brief  Function returns the text interpretation of API function.
 *
 * @param[in]  function_id - ID of the API function to be interpreted;
 * @param[out]  function_name - pointer to the returned string. In case pointer
 *				is NULL, then no text is filled in;
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t TA66XX_get_function_name(int32_t function_id, char *function_name);


/** @}*/
//=============================================================================



#ifdef __cplusplus
}
#endif



//=============================================================================
#endif /* __TA66XX_DSP_H__ */

