                Microsoft's Azure RTOS ThreadX for Cortex-M7

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
on the Green Hills Cortex-M7 simulator. The instructions that follow describe
how to get the ThreadX evaluation running under the MULTI Cortex-M7 simulation 
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
All suspended threads have the same stack frame in the Cortex-M7 version of
ThreadX. The top of the suspended thread's stack is pointed to by 
tx_thread_stack_ptr in the associated thread control block TX_THREAD.


Non-FPU Stack Frame:

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

FPU Stack Frame (only interrupted thread with FPU enabled):

  Stack Offset     Stack Contents 

     0x00               s0
     0x04               s1
     0x08               s2
     0x0C               s3
     0x10               s4
     0x14               s5
     0x18               s6
     0x1C               s7
     0x20               s8
     0x24               s9
     0x28               s10
     0x2C               s11
     0x30               s12
     0x34               s13
     0x38               s14
     0x3C               s15
     0x40               s16
     0x44               s17
     0x48               s18
     0x4C               s19
     0x50               s20
     0x54               s21
     0x58               s22
     0x5C               s23
     0x60               s24
     0x64               s25
     0x68               s26
     0x6C               s27
     0x70               s28
     0x74               s29
     0x78               s30
     0x7C               s31
     0x80               fpscr
     0x84               r4          
     0x88               r5          
     0x8C               r6          
     0x90               r7          
     0x94               r8          
     0x98               r9          
     0x9C               r10 (sl)    
     0xA0               r11         
     0xA4               r0          (Hardware stack starts here!!)
     0xA8               r1          
     0xAC               r2          
     0xB0               r3          
     0xB4               r12         
     0xB8               lr          
     0xBC               pc          
     0xC0               xPSR        


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

ThreadX provides complete and high-performance interrupt handling for Cortex-M7
targets. There are a certain set of requirements that are defined in the 
following sub-sections:


8.1  Vector Area

The Cortex-M7 vectors start at the label __tx_vectors. The application may modify
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


9. FPU Support

By default, FPU support is disabled for each thread. If saving the context of the FPU registers
is needed, the ThreadX library should be re-built with TX_ENABLE_FPU_SUPPORT defined. In addition,
the following API call must be made from the context of the application thread - before 
the FPU usage:

void    tx_thread_fpu_enable(void);

After this API is called in the application, FPU registers will be saved/restored for this thread if it
is preempted via an interrupt. All other suspension of the this thread will not require the FPU registers
to be saved/restored.

To disable FPU register context saving, simply call the following API:

void    tx_thread_fpu_disable(void);



10. Revision History

For generic code revision information, please refer to the readme_threadx_generic.txt
file, which is included in your distribution. The following details the revision
information associated with this specific port of ThreadX:

05/19/2020  Initial ThreadX version of Cortex-M7/Green Hills port. 


Copyright(c) 1996-2020 Microsoft Corporation


https://azure.com/rtos

