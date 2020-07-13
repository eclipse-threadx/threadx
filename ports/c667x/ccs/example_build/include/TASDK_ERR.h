/******************************************************************************
	TORNADO AMC modules Software Development Kit (SDK). Rev 4A.
	(C) MicroLAB Systems, 2015-2017


	File:	TORNADO AMC SDK functions errors
	-----

******************************************************************************/


/**
 * @file  TASDK_ERR.h
 *
 * @brief  SDK functions errors
 *
 * This file contains TORNADO AMC SDK functions errors
 *
 */


#ifndef __TASDK_ERR_H__										// check for this file has been already included
#define __TASDK_ERR_H__										1


//=============================================================================
//============ Returned error codes ===========================================
//=============================================================================
/** @addtogroup  TASDK_ERRORS  SDK API functions returned error codes
 *  @{
 */
#define TASDK_OK											0			/**< No errors */
#define TASDK_PARAM_ERR										-100		/**< Erroneous function parameter */
#define TASDK_INVALID_HW_ERR								-101		/**< Invalid H/W error (invalid board, etc.) */
#define TASDK_HW_ERR										-102		/**< H/W error (error writing to the DSP control registers, etc) */
#define TASDK_SYS_CALL_ERR									-103		/**< system API call error */
#define TASDK_INVALID_DSP_CORE_ERR							-104		/**< Only DSP core 0 may call the function */
#define TASDK_INVALID_DSP_CORE_BOOT_ADDR_ERR				-105		/**< Invalid DSP core boot address */
#define TASDK_API_OPEN_ERR									-106		/**< API has not been open */
#define TASDK_MMC_TO_CPU_COMM_CHECKSUM_ERR					-150		/**< Invalid data frame checksum */
#define TASDK_MMC_TO_CPU_COMM_DATA_BUF_LEN_ERR				-151		/**< Data buffer is not large enough to store received data frame */
#define TASDK_MMC_TO_CPU_COMM_INV_CMD_ERR					-152		/**< Invalid command */
#define TASDK_MMC_TO_CPU_COMM_RECV_DATA_LEN_ERR				-153		/**< Invalid received data length */
#define TASDK_MMC_TO_CPU_COMM_RECV_DATA_INV_ERR				-154		/**< Received data is invalid */
#define TASDK_MMC_TO_CPU_COMM_TIMEOUT_ERR					-155		/**< MMC-to-DSP communication timeout error */
#define TASDK_SFP_NOT_INSTALLED_ERR							-156		/**< SFP module is not installed */
#define TASDK_FMC_NOT_INSTALLED_ERR							-157		/**< FMC module is not installed */
#define TASDK_FMC_INFO_INVALID_ERR							-158		/**< FMC module info is invalid */
#define TASDK_FLASH_INV_DEV_ERR								-200		/**< unsupported FLASH device */
#define TASDK_FLASH_ERASE_ERR								-201		/**< FLASH erase error */
#define TASDK_FLASH_WR_ERR									-202		/**< FLASH write error */
#define TASDK_FLASH_CANNOT_WR_ERR							-203		/**< FLASH contents does not allow to write this data */
#define TASDK_FLASH_INV_RD_ERR								-204		/**< invalid FLASH read data during AUTO-SELECT */
#define TASDK_FLASH_INFO_NOT_MATCH_ERR						-205		/**< FLASH info from the FLASH chip does not match FLASH_LEN_ID from System Configuration register */
#define TASDK_FLASH_SECTOR_WRPROT_ERR						-206		/**< FLASH sector is write-protected */
#define TASDK_FLASH_WR_BUF_ABORT_ERR						-207		/**< FLASH write-to-buffer program is aborted */
#define TASDK_FLASH_HW_WP_ERR								-208		/**< FLASH writes are disabled by on-board switch */
#define TASDK_FLASH_WR_DISABLED_ERR							-209		/**< FLASH writes are disabled by application */
#define TASDK_FPGA_CNF_BITFILE_HD_FORMAT_ERR				-500		/**< invalid FPGA bitfile header format */
#define TASDK_FPGA_CNF_LOAD_ERR								-501		/**< error during FPGA configuration */
#define TASDK_FPGA_CNF_CRC_ERR								-502		/**< CRC check failed during FPGA configuration */
#define TASDK_FPGA_CNF_LOAD_LEN_ERR							-503		/**< not all the data has been loaded */
#define TASDK_FPGA_CNF_BITFILE_INV_PART_ERR					-504		/**< invalid FPGA bitfile header part name */
#define TASDK_FPGA_CNF_INV_IF_ERR							-505		/**< invalid FPGA configuration interface selected */
#define TASDK_FLASH_RECORD_GLOBAL_HD_NOT_VALID_ERR			-600		/**< FLASH record global header is invalid */
#define TASDK_FLASH_RECORD_HD_NOT_VALID_ERR					-601		/**< FLASH record header is invalid */
#define TASDK_FLASH_RECORD_HD_LIST_NOT_VALID_ERR			-602		/**< FLASH record header list is invalid */
#define TASDK_FLASH_RECORD_FREE_SPACE_ERR					-603		/**< not available free space in FLASH */
#define TASDK_FLASH_RECORD_CHECKSUM_ERR						-604		/**< FLASH record checksum mismatch */
#define TASDK_FLASH_RECORD_ALREADY_OPENED_ERR				-605		/**< another FLASH record is opened for writing */
#define TASDK_FLASH_RECORD_INVALID_LOAD_ADDR_ERR			-606		/**< invalid FLASH record load address */
#define TASDK_FLASH_RECORD_INV_NUMBER_ERR					-607		/**< invalid FLASH record number */
#define TASDK_FLASH_BOOT_SEQ_NOT_VALID_ERR					-608		/**< FLASH boot sequence is invalid */
#define ELF_LOADER_BASE_ERR									-1000						/**< ELF file loader base error */
#define ELF_LOADER_PARAM_ERR								(ELF_LOADER_BASE_ERR)		/**< erroneous function parameter */
#define ELF_LOADER_INV_ELF_HEADER_ERR						(ELF_LOADER_BASE_ERR - 1)	/**< invalid ELF header */
#define ELF_LOADER_INV_FILE_TYPE_ERR						(ELF_LOADER_BASE_ERR - 2)	/**< invalid file type (non-executable) */
#define ELF_LOADER_INV_ENTRY_POINT_ERR						(ELF_LOADER_BASE_ERR - 3)	/**< invalid entry-point address */
#define ELF_LOADER_INV_SEGMENT_ERR							(ELF_LOADER_BASE_ERR - 4)	/**< invalid or missing segment data */
/** @}*/
//=============================================================================



//=============================================================================
#endif /* __TASDK_ERR_H__ */

