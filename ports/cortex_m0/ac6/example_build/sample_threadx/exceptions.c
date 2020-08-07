/*
** Copyright (c) 2006-2017 Arm Limited (or its affiliates). All rights reserved.
** Use, modification and redistribution of this file is subject to your possession of a
** valid End User License Agreement for the Arm Product of which these examples are part of 
** and your compliance with all applicable terms and conditions of such licence agreement.
*/

/* This file contains the default exception handlers and vector table.
All exceptions are handled in Handler mode.  Processor state is automatically
pushed onto the stack when an exception occurs, and popped from the stack at
the end of the handler */


/* Exception Handlers */
/* Marking as __attribute__((interrupt)) avoids them being accidentally called from elsewhere */

__attribute__((interrupt)) void NMIException(void)
{   while(1); }

__attribute__((interrupt)) void HardFaultException(void)
{   while(1); }

void __tx_SVCallHandler(void);

void __tx_PendSVHandler(void);

/* For SysTick, use handler in timer.c */
void SysTick_Handler(void);

__attribute__((interrupt)) void InterruptHandler(void)
{   while(1); }


/* typedef for the function pointers in the vector table */
typedef void(* const ExecFuncPtr)(void) __attribute__((interrupt));

/* Linker-generated Stack Base address */
#ifdef TWO_REGION
extern unsigned int Image$$ARM_LIB_STACK$$ZI$$Limit;     /* for Two Region model */
#else
extern unsigned int Image$$ARM_LIB_STACKHEAP$$ZI$$Limit; /* for (default) One Region model */
#endif

/* Entry point for C run-time initialization */
extern int __main(void);


/* Vector table
Create a named ELF section for the vector table that can be placed in a scatter file.
The first two entries are:
    Initial SP = |Image$$ARM_LIB_STACKHEAP$$ZI$$Limit| for (default) One Region model
              or |Image$$ARM_LIB_STACK$$ZI$$Limit|     for Two Region model
    Initial PC= &__main (with LSB set to indicate Thumb)
*/

ExecFuncPtr vector_table[] __attribute__((section("vectors"))) = {
     /* Configure Initial Stack Pointer using linker-generated symbol */
#ifdef TWO_REGION
    #pragma import(__use_two_region_memory)
    (ExecFuncPtr)&Image$$ARM_LIB_STACK$$ZI$$Limit,
#else /* (default) One Region model */
    (ExecFuncPtr)&Image$$ARM_LIB_STACKHEAP$$ZI$$Limit,
#endif
    (ExecFuncPtr)__main, /* Initial PC, set to entry point  */
    NMIException,
    HardFaultException,
    0, 0, 0,                /* Reserved */
    0, 0, 0, 0,             /* Reserved */
	__tx_SVCallHandler,
    0,                      /* Reserved */
    0,                      /* Reserved */
	__tx_PendSVHandler,
	SysTick_Handler,

    /* Add up to 32 interrupt handlers, starting here... */
    InterruptHandler,
    InterruptHandler,      /* Some dummy interrupt handlers */
    InterruptHandler
    /*
    :
    */
};

