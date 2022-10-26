                    Microsoft's Azure RTOS ThreadX for Renesas RXv2

                                Using the IAR Tools


1.  Building the ThreadX run-time Library

Please see the Samples repository on GitHub for the Azure RTOS demonstrations
for the RXv2.


2.  Demonstration System

Please see the Samples repository on GitHub for the Azure RTOS demonstrations
for the RXv2.


3.  System Initialization

The system entry point using the IAR tools is at the label __iar_program_start. 

The vector area is setup in the file tx_initialize_low_level.s. This file is also 
responsible for setting up various system data structures, interrupt vectors, and 
the periodic timer interrupt. This file is also an ideal place add hardware 
initialization code.

The ThreadX demonstration for the RXv2 utilizes CMT0 as a periodic timer interrupt 
source. The CMT0 interrupt is typically setup for 10ms periodic interrupts and the 
interrupt priority level is set to level 5 with the symbol CMT_RX_CFG_IPR in 
r_cmt_rx_config.h of Renesas CMT timer module(r_cmt_rx). You may change any of the 
timer parameters as needed.

In addition, _tx_initialize_low_level determines the first available address for use 
by the application, which is supplied as the sole input parameter to your application 
definition function, tx_application_define. The first available memory is determined 
by the location of the FREEMEM section so it should be placed AFTER all other RAM 
sections in your linker control file.


4.  Context Switch, Register Usage and Stack Frames

The RXv2 port for ThreadX uses the first software interrupt, SWINT, i.e., interrupt #27,
to perform context switch with the interrupt priority level 1. This ISR is thus reserved 
when using ThreadX and the SWINT should not be manipulated in any way by the application. 
The port will setup the interrupt within _tx_initialize_low_level and the compiler will 
automatically install the necessary interrupt vector. As such no additional initialization 
is necessary by the application.

The following defines the saved context stack frame used by the ThreadX port. The
state of the CPU registers at the time of a context switch is saved on the running 
thread's stack The top of the suspended thread's stack is pointed to by 
tx_thread_stack_ptr in the associated thread control block TX_THREAD.

    Offset        Interrupted Stack Frame

     0x00                   1
     0x04                   ACC0
     0x08                   ACC1
     0x0C                   R6
     0x10                   R7
     0x14                   R8
     0x18                   R9
     0x1C                   R10
     0x20                   R11
     0x24                   R12
     0x28                   R13
     0x2C                   FPSW
     0x30                   R14
     0x34                   R15
     0x38                   R3
     0x3C                   R4
     0x40                   R5
     0x44                   R1
     0x48                   R2
     0x4C                   PC - return address
     0x50                   PSW
     
Note: By default IAR does not save the state of the accumulator registers ACC0 and ACC1
when entering an ISR. This means that if the ISR uses any of the DSP instructions the
content of those registers could be corrupted. Saving and restoring of the acummulators
can be enabled by adding the --save_acc command line option.
     

5.  Improving Performance

The distribution version of ThreadX is built without any compiler optimizations.  This 
makes it easy to debug because you can trace or set breakpoints inside of ThreadX itself.  
Of course, this costs some performance. To make ThreadX run faster, you can change the 
ThreadX Library project to disable debug information and enable the desired optimizations.  

In addition, you can eliminate the ThreadX basic API error checking by compiling your 
application code with the symbol TX_DISABLE_ERROR_CHECKING defined before tx_api.h 
is included. 


6. Timer Processing

Timer processign is performed by calling __tx_timer_interrupt(). This should usually be done
from within the callback of a periodic timer with a period of 100Hz. In the sample projects
a Renesas Fit CMT periodic timer module (r_cmt_rx) is used as the timer source.


7.  Interrupt Handling

Interrupt handling is unaffected by the ThreadX port as such user interrupts can be 
written according to the toolchain's documentation. It is recommended not to use interrupt
priority 1 as this is the priority of the context switch interrupt. However using interrupt
priority 1 won't cause any negative side effects but doing so may slightly reduce 
performance. Please refer to the toolchain documentation for additional details on how to
define interrupt service routines.


8. Execution Profiling

The RX port adds support for the Execution Profiling Kit (EPK). The EPK consists 
of the files tx_execution_profile.c and tx_execution_profile.h. See the documentation 
of the EPK for generic usage details.

To add the EPK to your RXv2 release make the following modifications:

* Enable the following define for both the Threadx library and the application
TX_EXECUTION_PROFILE_ENABLE

* Setup CMT1 as a free running 16 bit timer.

* In tx_execution_profile.h, change following around line 52:

#ifdef TX_EXECUTION_64BIT_TIME
typedef unsigned long long              EXECUTION_TIME;
#define TX_EXECUTION_MAX_TIME_SOURCE    0xFFFFFFFFFFFFFFFF
#else
typedef unsigned long                   EXECUTION_TIME;
#define TX_EXECUTION_MAX_TIME_SOURCE    0xFFFF
#endif
                                        
/* Define basic constants for the execution profile kit.  */

#define TX_EXECUTION_TIME_SOURCE         (EXECUTION_TIME)  *((USHORT *) 0x8800A)

Rebuild the Threadx library and the application.
Refer to the EPK documentation how to interpret the results.


9.  Revision History

For generic code revision information, please refer to the readme_threadx_generic.txt
file, which is included in your distribution. The following details the revision
information associated with this specific port of ThreadX:

04-25-2022  Release 6.1.11 changes:
            tx_thread_schedule.s                Added low power support

01-31-2022  Release 6.1.10 changes:
            tx_port.h                           Removed system state macro, and added 
                                                missing interrupt control defines
            tx_timer_interrupt.s                Added missing thread preemption logic

10-15-2021  Release 6.1.9 changes:
            tx_thread_context_restore.s         Removed unnecessary stack type placement 
            tx_thread_schedule.s                Removed unnecessary stack type checking
            tx_thread_stack_build.s             Removed unnecessary stack type placement 

06-02-2021  Release 6.1.7 changes:
            readme_threadx.txt                  Updated instructions on how to use execution profile.

04-02-2021  Release 6.1.6 changes:
            tx_port.h                           Updated macro definition

12-31-2020  Initial ThreadX release for the RXv2using IAR tools, version 6.1.3


Copyright(c) 1996-2022 Microsoft Corporation


https://azure.com/rtos