                     Microsoft's Azure RTOS ThreadX for Renesas RXv1

                            Using the GNU Tools


1.  Building the ThreadX run-time Library

Please see the Samples repository on GitHub for the Azure RTOS demonstrations
for the RXv1.


2.  Demonstration System


Please see the Samples repository on GitHub for the Azure RTOS demonstrations
for the RXv1.


3.  System Initialization

The system entry point using the GNU tools is at the label _PowerON_Reset. 

The vector area is setup in the file tx_initialize_low_level.S. This file is also 
responsible for setting up various system data structures, interrupt vectors, and 
the periodic timer interrupt. This file is also an ideal place to add additional hardware 
initialization code.

The ThreadX demonstration for the RXv1 utilizes CMT0 as a periodic timer interrupt 
source. The CMT0 interrupt is typically setup for 10ms periodic interrupts and the 
interrupt priority level is set to level 7. You may change any of the timer 
parameters as needed. Increasing the timer interrupt frequency increases the overhead
of the timer handling code on the system.

In addition, _tx_initialize_low_level determines the first available address for use 
by the application, which is supplied as the sole input parameter to your application 
definition function, tx_application_define. The first available memory is determined 
by the location of the '_end' label the is defined in the linker script.
'_end' should reference the first memory AFTER all other RAM 
sections in your linker control file.


4.  Context Switch, Register Usage and Stack Frames

The RXv1 port for ThreadX uses the first software interrupt, SWINT, i.e., interrupt #17,
to perform context switch. This ISR is thus reserved when using ThreadX and the SWINT
should not be manipulated in any way by the application. The port will setup the 
interrupt within _tx_initialize_low_level and the compiler will automatically install
the necessary interrupt vector. As such no additional initialization is necessary by the
application.

The following defines the saved context stack frame used by the ThreadX port. The
state of the CPU registers at the time of a context switch is saved on the running 
thread's stack The top of the suspended thread's stack is pointed to by 
tx_thread_stack_ptr in the associated thread control block TX_THREAD.

    Offset                  Stack Frame

     0x00                   ACC0
     0x04                   R6
     0x0C                   R7
     0x10                   R8
     0x14                   R9
     0x18                   R10
     0x1C                   R11
     0x20                   R12
     0x24                   R13
     0x28                   R14
     0x30                   R15
     0x34                   R3
     0x38                   R4
     0x3C                   R5
     0x40                   R1
     0x44                   R2
     0x48                   PC - return address
     0x4C                   PSW
     
Note: By default GNURX does not save the state of the accumulator register ACC0
when entering an ISR. This means that if the ISR uses any of the DSP instructions the
content of the accumulator could be corrupted. Saving and restoring of the accumulator
can be enabled by adding the -msave-acc-in-interrupts command line option.

     
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
a Renesas Fit CMT periodic timer module (rx_cmt) is used as the timer source.


7.  Interrupt Handling

Interrupt handling is unaffected by the ThreadX port as such user interrupts can be 
written according to the toolchain's documentation. It is recommended not to use interrupt
priority 15 as this is the priority of the context switch interrupt. However using interrupt
priority 15 won't cause any negative side effects but doing so may slightly reduce 
performance. Please refer to the toolchain documentation for additional details on how to
define interrupt service routines.


8. Execution Profiling

The RX port adds support for the Execution Profiling Kit (EPK). The EPK consists 
of the files tx_execution_profile.c and tx_execution_profile.h. See the documentation 
of the EPK for generic usage details.

To add the EPK to your RXv1 release make the following modifications:

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
            tx_port.h                           Added missing interrupt control defines
            tx_timer_interrupt.s                Added missing thread preemption logic

10-15-2021  Release 6.1.9 changes:
            tx_thread_context_restore.s         Removed unnecessary stack type placement 
            tx_thread_schedule.s                Removed unnecessary stack type checking
            tx_thread_stack_build.s             Removed unnecessary stack type placement 

08-02-2021  Initial ThreadX release for the RXv1 using GNURX tools, version 6.1.8


Copyright(c) 1996-2022 Microsoft Corporation


https://azure.com/rtos

