/*
 * board_setup.h
 *
 */


#ifndef BOARD_SETUP_H										/* check for this file has been already included */
#define BOARD_SETUP_H										1


#include "ti\platform\platform.h"


/*===========================================================================*/
/*============ Printf output definitions ====================================*/
/*===========================================================================*/
#define printf												platform_write
/*===========================================================================*/


/*------------ hardware_setup() function --------------------------------------
 * DESCRIPTION: Function intializes board hardware
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
int hardware_setup();


/*------------ tx_nmi_handler() function --------------------------------------
 * DESCRIPTION: Function handles NMI interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_nmi_handler(void);


/*------------ tx_int4_handler() function -------------------------------------
 * DESCRIPTION: Function handles INT4 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int4_handler(void);


/*------------ tx_int5_handler() function -------------------------------------
 * DESCRIPTION: Function handles INT5 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int5_handler(void);


/*------------ tx_int6_handler() function -------------------------------------
 * DESCRIPTION: Function handles INT6 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int6_handler(void);


/*------------ tx_int7_handler() function -------------------------------------
 * DESCRIPTION: Function handles INT7 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int7_handler(void);


/*------------ tx_int8_handler() function -------------------------------------
 * DESCRIPTION: Function handles INT8 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int8_handler(void);


/*------------ tx_int9_handler() function -------------------------------------
 * DESCRIPTION: Function handles INT9 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int9_handler(void);


/*------------ tx_int10_handler() function ------------------------------------
 * DESCRIPTION: Function handles INT10 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int10_handler(void);


/*------------ tx_int11_handler() function ------------------------------------
 * DESCRIPTION: Function handles INT11 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int11_handler(void);


/*------------ tx_int12_handler() function ------------------------------------
 * DESCRIPTION: Function handles INT12 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int12_handler(void);


/*------------ tx_int13_handler() function ------------------------------------
 * DESCRIPTION: Function handles INT13 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int13_handler(void);


/*------------ tx_int14_handler() function ------------------------------------
 * DESCRIPTION: Function handles INT14 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int14_handler(void);


/*------------ tx_int15_handler() function ------------------------------------
 * DESCRIPTION: Function handles INT15 interrupt
 * ARGUMENTS:
 * None
 * RETURNED VALUE: None
-----------------------------------------------------------------------------*/
void tx_int15_handler(void);


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
uint8_t *Osal_platformMalloc(uint32_t num_bytes, uint32_t alignment);


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
void Osal_platformFree(uint8_t *mem_ptr, uint32_t num_bytes);


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
void Osal_platformSpiCsEnter(void);


/*------------ Osal_platformSpiCsExit() function -------------------------*//**
 * @brief  Function is used to exit a critical section protected using
 * Osal_paMtCsEnter() API.
 *
 * @param[in]  key - handle for unlocking critical section
 *
 * @return  None
 *
-----------------------------------------------------------------------------*/
void Osal_platformSpiCsExit(void);


/*===========================================================================*/
#endif /* BOARD_SETUP_H */


