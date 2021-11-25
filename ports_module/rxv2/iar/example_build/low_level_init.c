/*
 * lowinit.c
 *
 * This module contains the function '__low_level_init', a function
 * that is called before the 'main' function of the program.  Normally
 * low-level initializations - such as setting the prefered interrupt
 * level or setting the watchdog - can be performed here.
 *
 * Note that this function is called before the data segments are
 * initialized, this means that this function can't rely on the
 * values of global or static variables.
 *
 * When this function returns zero, the startup code will inhibit the
 * initialization of the data segments.  The result is faster startup,
 * the drawback is that neither global nor static data will be
 * initialized.
 *
 *
 * Copyright 2009-2010 IAR Systems AB.
 *
 * $Revision: 2717 $
 */

#pragma language=extended

#include <iorx65n.h>
#include <intrinsics.h>
#include "hwsetup.h"

__intrinsic
int __low_level_init ( void )
{
  hardware_setup();
  
  /*==================================*/
  /* Choose if segment initialization */
  /* should be done or not.           */
  /* Return: 0 to omit seg_init       */
  /*         1 to run seg_init        */
  /*==================================*/
  return (1);
}
#pragma language=default
