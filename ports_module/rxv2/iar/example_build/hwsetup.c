/* Adapted for use with IAR Embedded Workbench */
/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name     : hwsetup.c
* Version      : 1.0 
* Device(s)    : RX
* H/W Platform : RX65N
* Description  : Defines the initialisation routines used each time the MCU is restarted.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include <intrinsics.h>

/* Contains delcarations for the functions defined in this file */
#include "hwsetup.h"

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* Clock configuration function delcaration */
void operating_frequency_set(void);

/* MCU I/O port configuration function delcaration */
//static void output_ports_configure(void);

/* Interrupt configuration function delcaration */
static void interrupts_configure(void);

/* MCU peripheral module configuration function declaration */
//static void peripheral_modules_enable(void);


/*******************************************************************************
* Function name: hardware_setup
* Description  : Contains setup functions called at device restart
* Arguments    : none
* Return value : none
*******************************************************************************/
void hardware_setup(void)
{
  operating_frequency_set();
  interrupts_configure();
}



/*******************************************************************************
* Function name: operating_frequency_set
* Description  : Configures the clock settings for each of the device clocks
* Arguments    : none
* Return value : none
*******************************************************************************/
void operating_frequency_set(void)
{

 /*
  Clock Description              Frequency
  ----------------------------------------
  Input Clock Frequency............  24 MHz
  Divisor..........................       2
  PLL frequency (x16).............. 192 MHz
  Internal Clock Frequency.........  96 MHz
  Peripheral Clock Frequency.......  48 MHz
  USB Clock Frequency..............  48 MHz
  External Bus Clock Frequency.....  24 MHz */
  
  volatile unsigned int i;
  
  /* Protect off. */
  SYSTEM.PRCR.WORD = 0xA50B;
  
  /* Uncomment if not using sub-clock */
  //SYSTEM.SOSCCR.BYTE = 0x01;          /* stop sub-clock */
  SYSTEM.SOSCCR.BYTE = 0x00;            /* Enable sub-clock for RTC */
  
  /* Wait 131,072 cycles * 12 MHz = 10.9 ms */
  SYSTEM.MOSCWTCR.BYTE = 0x0D;
  
  /* x16 @PLL, 2 divisor */
  SYSTEM.PLLCR.WORD = 0x0F01;
  
  /* EXTAL ON */
  SYSTEM.MOSCCR.BYTE = 0x00;
  
  /* PLL ON */
  SYSTEM.PLLCR2.BYTE = 0x00;
  
  for(i = 0;i< 0x168;i++)
  {
    /* Wait over 12ms */
    __no_operation();
  }
  
  /* Setup system clocks
  SCKCR - System Clock Control Register
  b31:b28 FCK[3:0]  0x02 = Flash clock: PLL/4 = (192 / 4) = 48 MHz
  b27:b24 ICK[3:0]  0x01 = System clock: PLL/2 = (192 / 2) = 96 MHz
  b23     PSTOP1    0x00 = BCLK pin output is enabled
  b19:b16 BCK[3:0]  0x03 = BCLK: PLL/8 = 24 MHz
  b11:b8  PCKB[3:0] 0x02 = Peripheral clock B: PLL/4 = 48 MHz
  */
  SYSTEM.SCKCR.LONG = 0x21031222;  /* ICK=PLL/2,BCK,FCK,PCK=PLL/4 */
  
  /* Setup IEBUS and USB clocks
  SCKCR2 - System Clock Control Register 2 
  b7:b4 UCK[3:0]   0x03 = USB clock is PLL/4 = 48 MHz
  b3:b0 IEBCK[3:0] 0x01 = IE Bus clock is PLL/2 = 96 MHz
  */
  SYSTEM.SCKCR2.WORD = 0x0031;
  
  /* ICLK, PCLKB, FCLK, BCLK, IECLK, and USBCLK all come from PLL circuit */
  SYSTEM.SCKCR3.WORD = 0x0400;
  
  /* Protect on. */
  SYSTEM.PRCR.WORD = 0xA500;
}


/***********************************************************************************************************************
* Function name: interrupts_configure
* Description  : Configures interrupts used
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
void interrupts_configure(void)
{
    /* Protect off. */
  SYSTEM.PRCR.WORD = 0xA50B;
  
  /* Enable the bus error interrupt to catch accesses to illegal/reserved areas
     of memory. */

  /* Clear any pending interrupts */
  IR(BSC,BUSERR) = 0;
  /* Make this the highest priority interrupt (adjust as necessary for your
     application) */
  IPR(BSC,BUSERR) = 0x0F; 
  /* Enable the interrupt in the ICU */
  IEN(BSC,BUSERR) = 1; 
  /* Enable illegal address interrupt in the BSC */
  BSC.BEREN.BIT.IGAEN = 1;
}


/******************************************************************************
* Function name: buserr_isr
* Description  : Sample ISR for bus error (must do hardware setup first!)
*                By default, this demo code enables the Bus Error Interrupt.
*                This interrupt will fire if the user tries to access code
*                or data from one of the reserved areas in the memory map, 
*                including the areas covered by disabled chip selects.
*                A nop statement is included here as a convenient place
*                to set a breakpoint during debugging and development, and
*                further handling should be added by the user for their 
*                application.
* Arguments    : none
* Return value : none
******************************************************************************/
#pragma vector = VECT_BSC_BUSERR
__interrupt void buserr_isr(void)
{
  /* To find the address that was accessed when the bus error occured, read
     the register BSC.BERSR2.WORD.  The upper 13 bits of this register
     contain the upper 13-bits of the offending address (in 512K byte units).
  */
  
  /* Add your own code here to handle this interrupt */
  __no_operation();
}
