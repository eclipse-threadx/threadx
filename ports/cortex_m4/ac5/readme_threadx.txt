                     Microsoft's Azure RTOS ThreadX for Cortex-M4 

                            Using ARM Compiler 5 (AC5)


1.  Building the ThreadX run-time Library

First make sure you are in the "example_build" directory. Also, make sure that 
you have setup your path and other environment variables necessary for the AC5 
development environment. At this point you may run the build_threadx.bat batch
file. This will build the ThreadX run-time environment in the "example_build"
directory. 

You should observe assembly and compilation of a series of ThreadX source 
files. At the end of the batch file, they are all combined into the 
run-time library file: tx.a. This file must be linked with your 
application in order to use ThreadX.


2.  Demonstration System

The ThreadX demonstration is designed to execute under the ARM
Windows-based simulator.

Building the demonstration is easy; simply execute the build_threadx_sample.bat 
batch file while inside the "example_build" directory. 

You should observe the compilation of sample_threadx.c (which is the demonstration 
application) and linking with tx.a. The resulting file sample_threadx.axf 
is a binary file that can be downloaded and executed on the ARM simulator.


3.  System Initialization

The entry point in ThreadX for the Cortex-M4 using AC5 tools is at label 
__main. This is defined within the AC5 compiler's startup code. In 
addition, this is where all static and global pre-set C variable 
initialization processing takes place.

The ThreadX tx_initialize_low_level.s file is responsible for setting up 
various system data structures, the vector area, and a periodic timer interrupt 
source. 

In addition, _tx_initialize_low_level determines the first available 
address for use by the application, which is supplied as the sole input 
parameter to your application definition function, tx_application_define.


4.  Register Usage and Stack Frames

The following defines the saved context stack frames for context switches
that occur as a result of interrupt handling or from thread-level API calls.
All suspended threads have the same stack frame in the Cortex-M4 version of
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
     0x9C               r10
     0xA0               r11
     0xA4               r0          (Hardware stack starts here!!)
     0xA8               r1
     0xAC               r2
     0xB0               r3
     0xB4               r12
     0xB8               lr
     0xBC               pc
     0xC0               xPSR


5.  Improving Performance

The distribution version of ThreadX is built without any compiler 
optimizations. This makes it easy to debug because you can trace or set 
breakpoints inside of ThreadX itself. Of course, this costs some 
performance. To make it run faster, you can change the ThreadX_Library.Uv2 
project to debugging and enable all compiler optimizations. 

In addition, you can eliminate the ThreadX basic API error checking by 
compiling your application code with the symbol TX_DISABLE_ERROR_CHECKING 
defined. 


6.  Interrupt Handling

ThreadX provides complete and high-performance interrupt handling for Cortex-M4
targets. There are a certain set of requirements that are defined in the 
following sub-sections:


6.1  Vector Area

The Cortex-M4 vectors start at the label __tx_vectors. The application may modify
the vector area according to its needs.


6.2 Managed Interrupts

ISRs for Cortex-M can be written completely in C (or assembly language) without any
calls to _tx_thread_context_save or _tx_thread_context_restore. These ISRs are allowed
access to the ThreadX API that is available to ISRs.

ISRs written in C will take the form (where "your_C_isr" is an entry in the vector table):

void    your_C_isr(void)
{

    /* ISR processing goes here, including any needed function calls.  */
}

ISRs written in assembly language will take the form:

    EXPORT  your_assembly_isr
your_assembly_isr

    PUSH    {r0, lr}

    ; ISR processing goes here, including any needed function calls.

    POP     {r0, lr}
    BX      lr


7. FPU Support

ThreadX for Cortex-M4 supports automatic ("lazy") VFP support, which means that applications threads 
can simply use the VFP and ThreadX automatically maintains the VFP registers as part of the thread 
context - no additional setup by the application.



8.  Revision History

For generic code revision information, please refer to the readme_threadx_generic.txt
file, which is included in your distribution. The following details the revision
information associated with this specific port of ThreadX:

08-14-2020  ThreadX update of Cortex-M4/AC5 port. The following files were 
            changed/added for port specific version 6.0.2:
            tx_thread_context_save.s        Fixed register names.
            *.s                             Modified comments and whitespace.

06/30/2020  Initial ThreadX 6.0.1 version for Cortex-M4 using AC5 tools.


Copyright(c) 1996-2020 Microsoft Corporation


https://azure.com/rtos

