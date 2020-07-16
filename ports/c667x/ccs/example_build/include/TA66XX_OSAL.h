/******************************************************************************
	TORNADO AMC modules Software Development Kit (SDK). Rev 3A.
	TI SYS/BIOS RTOS abstraction layer for TORNADO AMC embedded controllers.
	(C) MicroLAB Systems, 2014-2015


	File:	This file contains TI SYS/BIOS RTOS abstraction layer definitions
	-----		for TORNADO AMC embedded controllers, which is used for various
				low-level drivers (LLD) and libraries that the application use.


	Revision history:
	-----------------
	rev.1A		- 2014, initial release for TORNADO-A6678 board
	rev.2A		- 2015, totally redesigned SDK;
	rev.3A		- 2016, added support for TORNADO-A6678/FMC board rev.1A;

******************************************************************************/


/**
 * @file  TA66XX_OSAL.h
 *
 * @brief  RTOS abstraction layer definitions
 *
 * This file contains TI SYS/BIOS RTOS abstraction layer (OSAL) definitions for
 * TORNADO AMC embedded controllers, which is used for various
 * low-level drivers (LLD) and libraries that the application use.
 *
 */


#ifndef __TA66XX_OSAL_H__									// check for this file has been already included
#define __TA66XX_OSAL_H__									1



#ifdef __cplusplus
extern "C" {
#endif



//=============================================================================
//------------ Definitions ----------------------------------------------------
//=============================================================================
/** @addtogroup  TA66XX_OSAL_GENERAL  OS abstraction layer (OSAL) general definitions
 *  @{
 */

// Hardware semaphore defs
#define TA66XX_OSAL_CPPI_HW_SEM								1		/**< CPPI LLD hardware semaphore */
#define TA66XX_OSAL_QMSS_HW_SEM								2		/**< QMSS LLD hardware semaphore */
#define TA66XX_OSAL_SRIO_HW_SEM								3		/**< SRIO LLD hardware semaphore */
#define TA66XX_OSAL_SPI_HW_SEM								4		/**< SPI driver hardware semaphore */

/** Macro to define high-priority QMSS queue for accumulator (depends on DSP event ID and core number) */
#define TA66XX_OSAL_set_qmss_queue_acc(eventid, core)		(704 + (eventid - 48) * 8 + core)

// Interrupts and event IDs used by NIMU library (DSP core 0 is assumed to be used)
#define TA66XX_OSAL_ETHERNET_AMC_EVENTID					48		/**< Ethernet AMC port event - used by NIMU library */
#define TA66XX_OSAL_ETHERNET_AMC_INTERRUPT					7		/**< Ethernet AMC port interrupt - used by NIMU library */
/** QMSS queue for AMC Ethernet port accumulator (depends on TA66XX_OSAL_ETHERNET_AMC_EVENTID) */
#define TA66XX_OSAL_ETHERNET_AMC_QMSS_QUEUE_ACC				TA66XX_OSAL_set_qmss_queue_acc(TA66XX_OSAL_ETHERNET_AMC_EVENTID, 0)
#define TA66XX_OSAL_ETHERNET_PHY_EVENTID					49		/**< Ethernet PHY port event - used by NIMU library */
#define TA66XX_OSAL_ETHERNET_PHY_INTERRUPT					8		/**< Ethernet PHY port interrupt - used by NIMU library */
/** QMSS queue for PHY Ethernet port accumulator (depends on TA66XX_OSAL_ETHERNET_PHY_EVENTID) */
#define TA66XX_OSAL_ETHERNET_PHY_QMSS_QUEUE_ACC				TA66XX_OSAL_set_qmss_queue_acc(TA66XX_OSAL_ETHERNET_PHY_EVENTID, 0)

// Interrupts and event IDs used by SRIO (DSP core 0 is assumed to be used)
#define TA66XX_OSAL_SRIO_MESSAGES_EVENTID					50		/**< SRIO event ID used by messages (Type 9 and Type 11) */
#define TA66XX_OSAL_SRIO_INTERRUPT							9		/**< SRIO interrupt */
/** QMSS queue for SRIO accumulator (depends on TA66XX_OSAL_SRIO_MESSAGES_EVENTID) */
#define TA66XX_OSAL_SRIO_QMSS_QUEUE_ACC						TA66XX_OSAL_set_qmss_queue_acc(TA66XX_OSAL_SRIO_MESSAGES_EVENTID, 0)
#define TA66XX_OSAL_SRIO_DIO_EVENTID						20		/**< SRIO event ID used by DirectIO (INTDST(n + 16), n - core number) */
/** @}*/
//=============================================================================



//=============================================================================
//------------ Global variables -----------------------------------------------
//=============================================================================
extern uint32_t qmssMallocCounter;
extern uint32_t qmssFreeCounter;
extern uint32_t cppiMallocCounter;
extern uint32_t cppiFreeCounter;
extern uint32_t srioMallocCounter;
extern uint32_t srioFreeCounter;
extern uint32_t srioDataBufferMallocCounter;
extern uint32_t srioDataBufferFreeCounter;
//=============================================================================



//=============================================================================
//============ T-AMC controllers OSAL functions declarations ==================
//=============================================================================
/** @addtogroup  TA66XX_OSAL_FUNCTIONS  TORNADO AMC OSAL functions
 *  @{
 */


//=============================================================================
//============ QMSS LLD OSAL functions ========================================
//=============================================================================

/*------------ Osal_qmssBeginMemAccess() function ------------------------*//**
 * @brief  Function is used to indicate that a block of memory is about to be
 * accessed
 *
 * If the memory block is cached then this indicates that the application
 * would need to ensure that the cache is updated with the data from the actual
 * memory
 *
 * @param[in]  ptr - address of memory block
 * @param[in]  size - size of memory block
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_qmssBeginMemAccess(void *ptr, uint32_t size);


/*------------ Osal_qmssEndMemAccess() function --------------------------*//**
 * @brief  Function is used to indicate that the block of memory has finished
 * being accessed
 *
 * If the memory block is cached then the application would need to ensure that
 * the contents of the cache are updated immediately to the actual memory.
 *
 * @param[in]  ptr - address of memory block
 * @param[in]  size - size of memory block
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_qmssEndMemAccess(void *ptr, uint32_t size);


/*------------ Osal_qmssMalloc() function --------------------------------*//**
 * @brief  Function implements the memory allocation library function
 *
 * This function allocates a memory block of a given size specified by input
 * parameter 'num_bytes'.
 *
 * @param[in]  num_bytes - number of bytes to be allocated
 *
 * @return  Allocated block address
 *
-----------------------------------------------------------------------------*/
Ptr Osal_qmssMalloc(uint32_t num_bytes);


/*------------ Osal_qmssFree() function ----------------------------------*//**
 * @brief  Function implements the memory free library function
 *
 * This function frees up memory allocated using Osal_qmssMalloc()
 * function call.
 *
 * @param[in]  dataPtr - pointer to the memory block to be cleaned up
 * @param[in]  num_bytes - size of the memory block to be cleaned up in bytes
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_qmssFree(Ptr dataPtr, uint32_t num_bytes);


/*------------ Osal_qmssCsEnter() function -------------------------------*//**
 * @brief  Function is used to enter a critical section
 *
 * Function protects against access from multiple cores and access from
 * multiple threads on single core
 *
 * @return  Handle used to lock critical section
 *
-----------------------------------------------------------------------------*/
void *Osal_qmssCsEnter(void);


/*------------ Osal_qmssCsExit() function --------------------------------*//**
 * @brief  Function is used to exit a critical section protected using
 * Osal_qmssCsEnter() API.
 *
 * @param[in]  CsHandle - handle for unlocking critical section
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_qmssCsExit(void *CsHandle);

//=============================================================================



//=============================================================================
//============ CPPI LLD OSAL functions ========================================
//=============================================================================

/*------------ Osal_cppiBeginMemAccess() function ------------------------*//**
 * @brief  Function is used to indicate that a block of memory is about to be
 * accessed
 *
 * If the memory block is cached then this indicates that the application
 * would need to ensure that the cache is updated with the data from the actual
 * memory
 *
 * @param[in]  ptr - address of memory block
 * @param[in]  size - size of memory block
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_cppiBeginMemAccess(void *ptr, uint32_t size);


/*------------ Osal_cppiEndMemAccess() function --------------------------*//**
 * @brief  Function is used to indicate that the block of memory has finished
 * being accessed
 *
 * If the memory block is cached then the application would need to ensure that
 * the contents of the cache are updated immediately to the actual memory.
 *
 * @param[in]  ptr - address of memory block
 * @param[in]  size - size of memory block
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_cppiEndMemAccess(void *ptr, uint32_t size);


/*------------ Osal_cppiMalloc() function --------------------------------*//**
 * @brief  Function implements the memory allocation library function
 *
 * This function allocates a memory block of a given size specified by input
 * parameter 'num_bytes'.
 *
 * @param[in]  num_bytes - number of bytes to be allocated
 *
 * @return  Allocated block address
 *
-----------------------------------------------------------------------------*/
Ptr Osal_cppiMalloc(uint32_t num_bytes);


/*------------ Osal_cppiFree() function ----------------------------------*//**
 * @brief  Function implements the memory free library function
 *
 * This function frees up memory allocated using Osal_cppiMalloc()
 * function call.
 *
 * @param[in]  dataPtr - pointer to the memory block to be cleaned up
 * @param[in]  num_bytes - size of the memory block to be cleaned up in bytes
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_cppiFree(Ptr dataPtr, uint32_t num_bytes);


/*------------ Osal_cppiCsEnter() function -------------------------------*//**
 * @brief  Function is used to enter a critical section
 *
 * Function protects against access from multiple cores and access from
 * multiple threads on single core
 *
 * @return  Handle used to lock critical section
 *
-----------------------------------------------------------------------------*/
void *Osal_cppiCsEnter(void);


/*------------ Osal_cppiCsExit() function --------------------------------*//**
 * @brief  Function is used to exit a critical section protected using
 * Osal_cppiCsEnter() API.
 *
 * @param[in]  CsHandle - handle for unlocking critical section
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_cppiCsExit(void *CsHandle);

//=============================================================================


#ifdef __TA66XX_FUNCTIONS_INCLUDE_SRIO__


//=============================================================================
//============ SRIO LLD OSAL functions ========================================
//=============================================================================

/*------------ Osal_srioBeginMemAccess() function ------------------------*//**
 * @brief  Function is used to indicate that a block of memory is about to be
 * accessed
 *
 * If the memory block is cached then this indicates that the application
 * would need to ensure that the cache is updated with the data from the actual
 * memory
 *
 * @param[in]  ptr - address of memory block
 * @param[in]  size - size of memory block
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_srioBeginMemAccess(void *ptr, uint32_t size);


/*------------ Osal_srioEndMemAccess() function --------------------------*//**
 * @brief  Function is used to indicate that the block of memory has finished
 * being accessed
 *
 * If the memory block is cached then the application would need to ensure that
 * the contents of the cache are updated immediately to the actual memory.
 *
 * @param[in]  ptr - address of memory block
 * @param[in]  size - size of memory block
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_srioEndMemAccess(void *ptr, uint32_t size);


/*------------ Osal_srioBeginDescriptorAccess() function -----------------*//**
 * @brief  Function is invoked by the SRIO LLD to indicate that a descriptor is
 * being accessed.
 *
 * @param[in]  drvHandle - driver instance for which descriptor is being
 *				accessed
 * @param[in]  ptr - pointer to the descriptor being accessed
 * @param[in]  descSize - size of the descriptor (valid only for driver managed
 *				configuration)
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_srioBeginDescriptorAccess(Srio_DrvHandle drvHandle, void *ptr, uint32_t descSize);


/*------------ Osal_srioEndDescriptorAccess() function -------------------*//**
 * @brief  Function is invoked by the SRIO LLD to indicate that a descriptor is
 * finished being accessed.
 *
 * @param[in]  drvHandle - driver instance for which descriptor is being
 *				accessed
 * @param[in]  ptr - pointer to the descriptor being accessed
 * @param[in]  descSize - size of the descriptor (valid only for driver managed
 *				configuration)
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_srioEndDescriptorAccess(Srio_DrvHandle drvHandle, void *ptr, uint32_t descSize);


/*------------ Osal_srioMalloc() function --------------------------------*//**
 * @brief  Function implements the memory allocation library function
 *
 * This function allocates a memory block of a given size specified by input
 * parameter 'num_bytes'.
 *
 * @param[in]  num_bytes - number of bytes to be allocated
 *
 * @return  Allocated block address
 *
-----------------------------------------------------------------------------*/
Ptr Osal_srioMalloc(uint32_t num_bytes);


/*------------ Osal_srioFree() function ----------------------------------*//**
 * @brief  Function implements the memory free library function
 *
 * This function frees up memory allocated using Osal_srioMalloc()
 * function call.
 *
 * @param[in]  dataPtr - pointer to the memory block to be cleaned up
 * @param[in]  num_bytes - size of the memory block to be cleaned up in bytes
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_srioFree(Ptr dataPtr, uint32_t num_bytes);


/*------------ Osal_dataBufferInitMemory() function ----------------------*//**
 * @brief  Function is used to allocate a block of memory for all the data
 * buffer operations. This function is called by the application.
 *
 * @param[in]  dataBufferSize - size of each data buffer
 *
 * @return  Error code
 *
-----------------------------------------------------------------------------*/
int32_t Osal_dataBufferInitMemory(uint32_t dataBufferSize);


/*------------ Osal_srioDataBufferMalloc() function ----------------------*//**
 * @brief  Function is used to allocate a data buffer of the specified size.
 * Data buffers should always be allocated from the global address space.
 *
 * @param[in]  numBytes - number of bytes to be allocated
 *
 * @return  Allocated block address
 *
-----------------------------------------------------------------------------*/
void *Osal_srioDataBufferMalloc(uint32_t numBytes);


/*------------ Osal_srioDataBufferFree() function ------------------------*//**
 * @brief  Function is used to clean up a previously allocated data buffer 
 * block. All data buffers are in the global address space.
 *
 * @param[in]  ptr - pointer to the memory block to be cleaned up
 * @param[in]  numBytes - size of the memory block to be cleaned up in bytes
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_srioDataBufferFree(void *ptr, uint32_t numBytes);


/*------------ Osal_srioLog() function -----------------------------------*//**
 * @brief  Function is used to log the messages from SRIO LLD on the console.
 *
 * @param[in]  fmt - formatted string
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_srioLog(String fmt, ... );


/*------------ Osal_srioCreateSem() function -----------------------------*//**
 * @brief  Function is used to create a critical section.
 *
 * @return  Semaphore handle created
 *
-----------------------------------------------------------------------------*/
void *Osal_srioCreateSem(void);


/*------------ Osal_srioDeleteSem() function -----------------------------*//**
 * @brief  Function is used to delete a critical section.
 *
 * @param[in]  semHandle - semaphore handle to be deleted
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_srioDeleteSem(void *semHandle);


/*------------ Osal_srioPendSem() function -------------------------------*//**
 * @brief  Function is used to pend on a semaphore
 *
 * @param[in]  semHandle - semaphore handle on which the API will pend
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_srioPendSem(void *semHandle);


/*------------ Osal_srioPostSem() function -------------------------------*//**
 * @brief  Function is used to post a semaphore
 *
 * @param[in]  semHandle - semaphore handle which will be posted
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_srioPostSem(void *semHandle);


/*------------ Osal_srioEnterMultipleCoreCriticalSection() function ------*//**
 * @brief  Function is used to protect the driver shared resources across
 * multiple cores.
 *
 * @return  Handle used to lock critical section
 *
-----------------------------------------------------------------------------*/
void *Osal_srioEnterMultipleCoreCriticalSection(void);


/*------------ Osal_srioExitMultipleCoreCriticalSection() function -------*//**
 * @brief  Function is called to end the critical section which was protecting
 * shared resources from access across multiple cores.
 *
 * @param[in]  critSectHandle - handle for unlocking critical section
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_srioExitMultipleCoreCriticalSection(void *critSectHandle);


/*------------ Osal_srioEnterSingleCoreCriticalSection() function --------*//**
 * @brief  Function is used to provide critical section to prevent access of
 * shared resources from single core and multiple threads.
 *
 * @param[in]  drvHandle - driver handle which needs critical section to
 *				protect its resources
 *
 * @return  Handle used to lock critical section
 *
-----------------------------------------------------------------------------*/
void *Osal_srioEnterSingleCoreCriticalSection(Srio_DrvHandle drvHandle);


/*------------ Osal_srioExitSingleCoreCriticalSection() function ---------*//**
 * @brief  Function is called to end the critical section access of shared
 * resources from single cores.
 *
 * @param[in]  drvHandle - driver handle which needs critical section to
 *				protect its resources
 * @param[in]  critSectHandle - critical handle retreived by
 *				Osal_srioEnterSingleCoreCriticalSection() function call
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_srioExitSingleCoreCriticalSection(Srio_DrvHandle drvHandle, void *critSectHandle);

//=============================================================================


#endif /* __TA66XX_FUNCTIONS_INCLUDE_SRIO__ */


//=============================================================================
//============ NIMU LLD OSAL functions ========================================
//=============================================================================

/*------------ Osal_nimuMalloc() function --------------------------------*//**
 * @brief  Function implements the memory allocate function for the NIMU
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
Ptr Osal_nimuMalloc(uint32_t num_bytes, uint32_t alignment);


/*------------ Osal_nimuFree() function ----------------------------------*//**
 * @brief  Function implements the memory free function for the NIMU library.
 *
 * This function frees up memory allocated using Osal_nimuMalloc()
 * function call.
 *
 * @param[in]  dataPtr - pointer to the memory block to be cleaned up
 * @param[in]  num_bytes - size of the memory block to be cleaned up in bytes
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_nimuFree(Ptr dataPtr, uint32_t num_bytes);

//=============================================================================



//=============================================================================
//============ PASS LLD OSAL functions ========================================
//=============================================================================


/*------------ Osal_paBeginMemAccess() function --------------------------*//**
 * @brief  Function is used to indicate that a block of memory is about to be
 * accessed
 *
 * If the memory block is cached then this indicates that the application
 * would need to ensure that the cache is updated with the data from the actual
 * memory
 *
 * @param[in]  addr - address of memory block
 * @param[in]  size - size of memory block
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_paBeginMemAccess(Ptr addr, uint32_t size);


/*------------ Osal_paEndMemAccess() function ----------------------------*//**
 * @brief  Function is used to indicate that the block of memory has finished
 * being accessed
 *
 * If the memory block is cached then the application would need to ensure that
 * the contents of the cache are updated immediately to the actual memory.
 *
 * @param[in]  addr - address of memory block
 * @param[in]  size - size of memory block
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_paEndMemAccess(Ptr addr, uint32_t size);


/*------------ Osal_paMtCsEnter() function -------------------------------*//**
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
void Osal_paMtCsEnter(uint32_t *key);


/*------------ Osal_paMtCsExit() function --------------------------------*//**
 * @brief  Function is used to exit a critical section protected using
 * Osal_paMtCsEnter() API.
 *
 * @param[in]  key - handle for unlocking critical section
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_paMtCsExit(uint32_t key);

/** @}*/
//=============================================================================



#ifdef __cplusplus
}
#endif



//=============================================================================
#endif /* __TA66XX_OSAL_H__ */
