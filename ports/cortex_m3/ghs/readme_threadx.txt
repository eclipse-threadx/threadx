                Microsoft's Azure RTOS ThreadX for Cortex-M3

                    Using the Green Hills Software Tools

1. Open the ThreadX Project Workspace

In order to build the ThreadX library and the ThreadX demonstration first load 
the Azure RTOS Workspace azure_rtos_workspace.gpj, which is located inside the 
"example_build" directory. 


2. Building the ThreadX run-time Library

Building the ThreadX library is easy; simply select the MULTI project file 
tx.gpj and then select the build button. You should now observe the 
compilation and assembly of the ThreadX library. This project build produces 
the ThreadX library file tx.a.


3. Demonstration System

The ThreadX demonstration is designed to execute under the MULTI environment
on the Green Hills Cortex-M3 simulator. The instructions that follow describe
how to get the ThreadX evaluation running under the MULTI Cortex-M3 simulation 
environment.

Building the demonstration is easy; simply select the MULTI project file 
sample_threadx.gpj. At this point, select the "Project Build" button and observe 
the compilation, assembly, and linkage of the ThreadX demonstration application. 

After the demonstration is built, invoke the  MULTI ARM simulator by selecting 
the simulator connection from within the sample_threadx.con connection file. 
Once connected to the simulator, select the "Debug" button. You should now 
observe the main function of sample_threadx.c. 

You are now ready to execute the ThreadX demonstration system. Select 
breakpoints and data watches to observe the execution of the sample_threadx.c 
application.


4. EventAnalyzer Demonstration

To build a demonstration system that also logs events for the MULTI EventAnalyzer, 
perform the same steps as the regular demo, except build the ThreadX library with 
txe.gpj file and use the sample_threadx_el.gpj build file to build the demonstration. 
The resulting image will log all system events, which can then be displayed by the 
MULTI EventAnalyzer.


5. System Initialization

The system entry point using the Green Hills tools is at the label _start. 
This is defined within the crt0.arm file supplied by Green Hills. In addition, 
this is where all static and global preset C variable initialization 
processing is called from.

After the Green Hills startup function returns, ThreadX initialization is
called. The main initialization function is _tx_initialize_low_level and
is located in the file tx_initialize_low_level.arm. This function is responsible 
for setting up various system data structures, interrupt vectors, and the 
periodic timer interrupt source of ThreadX.

In addition, _tx_initialize_low_level determines where the first available 
RAM memory address is located. This address is supplied to tx_application_define.

By default, the first available RAM memory address is assumed to start at the 
beginning of the ThreadX section .free_mem. If changes are made to the 
sample_threadx.ld file, the .free_mem section should remain the last allocated 
section in the main RAM area. The starting address of this section is passed 
to tx_application_define.


6. Register Usage and Stack Frames

The following defines the saved context stack frames for context switches
that occur as a result of interrupt handling or from thread-level API calls.
All suspended threads have the same stack frame in the Cortex-M3 version of
ThreadX. The top of the suspended thread's stack is pointed to by 
tx_thread_stack_ptr in the associated thread control block TX_THREAD.


  Stack Offset     Stack Contents 

     0x00               r4
     0x04               r5
     0x08               r6
     0x0C               r7
     0x10               r8
     0x14               r9
     0x18               r10
     0x1C               r11
     0x20               r0          (Hardware stack starts here!!)
     0x24               r1
     0x28               r2
     0x2C               r3
     0x30               r12
     0x34               lr
     0x38               pc
     0x3C               xPSR


7. Improving Performance

The distribution version of ThreadX is built without any compiler 
optimizations. This makes it easy to debug because you can trace or set 
breakpoints inside of ThreadX itself. Of course, this costs some 
performance. To make ThreadX run faster, you can change the tx.gpj project
to disable debug information and enable the desired optimizations. 

In addition, you can eliminate the ThreadX basic API error checking by 
compiling your application code with the symbol TX_DISABLE_ERROR_CHECKING 
defined before tx_api.h is included. 


8. Interrupt Handling

ThreadX provides complete and high-performance interrupt handling for Cortex-M3
targets. There are a certain set of requirements that are defined in the 
following sub-sections:


8.1  Vector Area

The Cortex-M3 vectors start at the label __tx_vectors. The application may modify
the vector area according to its needs.


8.2 Managed Interrupts

A ThreadX managed interrupt is defined below. By following these conventions, the 
application ISR is then allowed access to various ThreadX services from the ISR.
Here is the standard template for managed ISRs in ThreadX:


        .globl  __tx_IntHandler
__tx_IntHandler:
        PUSH    {lr}
        BL      _tx_thread_context_save
        
            /* Do interrupt handler work here */

        B       _tx_thread_context_restore


9. Revision History

For generic code revision information, please refer to the readme_threadx_generic.txt
file, which is included in your distribution. The following details the revision
information associated with this specific port of ThreadX:

04-02-2021  Release 6.1.6 changes:
            tx_port.h                           Updated macro definition

03-02-2021  The following files were changed/added for version 6.1.5:
            tx_thread_schedule.s            Added low power feature

05/19/2020  Initial ThreadX version of Cortex-M3/Green Hills port. 


Copyright(c) 1996-2020 Microsoft Corporation


https://azure.com/rtos

