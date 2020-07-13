/*
 * board_setup.h
 *
 */


#ifndef BOARD_SETUP_H										// check for this file has been already included
#define BOARD_SETUP_H										1


/* Include T-AMC DSP API */
#include "TA66XX_DSP_BC.h"


//=============================================================================
//============ Printf output definitions ======================================
//=============================================================================
#define printf												TA66XX_BC_uart_printf
//=============================================================================


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


//=============================================================================
#endif /* BOARD_SETUP_H */


