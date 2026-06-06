/***************************************************************************/
/* Copyright (c) 2024 Microsoft Corporation                                */
/* Copyright (c) 2026 Eclipse ThreadX contributors                         */
/*                                                                         */
/* This program and the accompanying materials are made available under    */
/* the terms of the MIT License which is available at                      */
/* https://opensource.org/licenses/MIT.                                    */
/*                                                                         */
/* SPDX-License-Identifier: MIT                                            */
/***************************************************************************/

#include "tx_api.h"


/* Define the prototypes for the test entry points.  */

void    tm_main(void);


/* Define main entry point.  */
int main()
{

    /* Initialize the platform if required. */
    /* Custom code goes here. */

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}


/* Define what the initial system looks like.  */

void    tx_application_define(void *first_unused_memory)
{

     /* Enter the Thread-Metric test main function for initialization and to start the test.  */
     tm_main();
}

