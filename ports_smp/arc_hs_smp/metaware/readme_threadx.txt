                    Microsoft's Azure RTOS ThreadX SMP for ARC HS

                             Using the MetaWare Tools

1. Open the ThreadX SMP Workspace

In order to build the ThreadX SMP library and the ThreadX SMP demonstration 
first load the Azure RTOS Workspace, which is located inside the "example_build" 
directory. 


2. Building the ThreadX SMP run-time Library

Building the ThreadX SMP library is easy; simply select the ThreadX library project 
file "tx" and then select the build button. You should now observe the compilation 
and assembly of the ThreadX SMP library. This project build produces the ThreadX SMP
library file tx.a.


3. Demonstration System

The ThreadX demonstration is designed to execute under the MetaWare ARC HS SMP
simulation. The instructions that follow describe how to get the ThreadX SMP
demonstration running. 

Building the demonstration is easy; simply select the demonstration project file 
"sample_threadx." At this point, select the build button and observe the 
compilation, assembly, and linkage of the ThreadX SMP demonstration application. 

After the demonstration is built, execute the "run_threadx_smp_demo.bat" batch file
to invoke and load the ThreadX SMP demonstration.

You are now ready to execute the ThreadX demonstration system. Select 
breakpoints and data watches to observe the execution of the sample_threadx.c 
SMP application.


4.  System Initialization

The system entry point using the MetaWare tools is at the label _start. 
This is defined within the crt1.s file supplied by MetaWare. In addition, 
this is where all static and global preset C variable initialization
processing is called from.

After the MetaWare startup function completes, ThreadX initialization is
called. The main initialization function is _tx_initialize_low_level and
is located in the file tx_initialize_low_level.s. This function is 
responsible for setting up various system data structures, and interrupt 
vectors.

By default free memory is assumed to start at the section .free_memory 
which is referenced in tx_initialize_low_level.s and located in the 
linker control file after all the linker defined RAM addresses. This is 
the address passed to the application definition function, tx_application_define.


5.  Register Usage and Stack Frames

The ARC compiler assumes that registers r0-r12 are scratch registers for 
each function. All other registers used by a C function must be preserved 
by the function. ThreadX takes advantage of this in situations where a 
context switch happens as a result of making a ThreadX service call (which 
is itself a C function). In such cases, the saved context of a thread is 
only the non-scratch registers.

The following defines the saved context stack frames for context switches
that occur as a result of interrupt handling or from thread-level API calls.
All suspended threads have one of these two types of stack frames. The top
of the suspended thread's stack is pointed to by tx_thread_stack_ptr in the 
associated thread control block TX_THREAD.



    Offset        Interrupted Stack Frame        Non-Interrupt Stack Frame

     0x00                   1                           0
     0x04                   LP_START                    blink
     0x08                   LP_END                      fp
     0x0C                   LP_COUNT                    r26
     0x10                   blink                       r25
     0x14                   ilink                       r24
     0x18                   fp                          r23
     0x1C                   r26                         r22
     0x20                   r25                         r21
     0x24                   r24                         r20
     0x28                   r23                         r19
     0x2C                   r22                         r18
     0x30                   r21                         r17
     0x34                   r20                         r16
     0x38                   r19                         r15
     0x3C                   r18                         r14
     0x40                   r17                         r13
     0x44                   r16                         STATUS32
     0x48                   r15                         r30
     0x4C                   r14
     0x50                   r13
     0x54                   r12
     0x58                   r11
     0x5C                   r10
     0x60                   r9
     0x64                   r8
     0x68                   r7
     0x6C                   r6
     0x70                   r5
     0x74                   r4
     0x78                   r3
     0x7C                   r2
     0x80                   r1
     0x84                   r0
     0x88                   r30
     0x8C                   r58 - ACCL (optional)
     0x90                   r59 - ACCH (optional)
     0x94                   reserved
     0x98                   reserved
     0x9C                   bta
     0xA0                   point of interrupt
     0xA4                   STATUS32
                    


6.  Improving Performance

The distribution version of ThreadX is built without any compiler 
optimizations. This makes it easy to debug because you can trace or set 
breakpoints inside of ThreadX itself. Of course, this costs some 
performance. To make it run faster, you can change the build_threadx.bat 
file to remove the -g option and enable all compiler optimizations. 

In addition, you can eliminate the ThreadX basic API error checking by 
compiling your application code with the symbol TX_DISABLE_ERROR_CHECKING 
defined. 


7.  Interrupt Handling

ThreadX provides complete and high-performance interrupt handling for the
ARC HS processor, including support for software interrupts and fast 
hardware interrupts.

7.1 Software Interrupt Handling

The following template should be used for software interrupts
managed by ThreadX:

    .global _tx_interrupt_x
_tx_interrupt_x:
    sub     sp, sp, 160                     ; Allocate an interrupt stack frame
    st      blink, [sp, 16]                 ; Save blink (blink must be saved before _tx_thread_context_save)
    bl      _tx_thread_context_save         ; Save interrupt context
;
;   /* Application ISR processing goes here!  Your ISR can be written in 
;      assembly language or in C.  If it is written in C, you must allocate
;      16 bytes of stack space before it is called.  This must also be 
;      recovered once your C ISR return.  An example of this is shown below. 
;
;      If the ISR is written in assembly language, only the compiler scratch
;      registers are available for use without saving/restoring (r0-r12). 
;      If use of additional registers are required they must be saved and
;      restored.  */
;
    bl.d    your_ISR_written_in_C           ; Call an ISR written in C
    sub     sp, sp, 16                      ; Allocate stack space (delay slot)
    add     sp, sp, 16                      ; Recover stack space
    
;
    b       _tx_thread_context_restore      ; Restore interrupt context


The application handles interrupts directly, which necessitates all register 
preservation by the application's ISR. ISRs that do not use the ThreadX
_tx_thread_context_save and _tx_thread_context_restore routines are not
allowed access to the ThreadX API. In addition, custom application ISRs
should be higher priority than all ThreadX-managed ISRs.


8.  ThreadX Timer Interrupt

ThreadX SMP requires a periodic interrupt source to manage all time-slicing, 
thread sleeps, timeouts, and application timers. Without such a timer 
interrupt source, these services are not functional but the remainder of 
ThreadX will still run.

By default, the ThreadX timer interrupt is mapped to the ARC HS auxiliary
timer 0, which generates low priority interrupts on interrupt vector 16.
It is easy to change the timer interrupt source and priority by changing the 
setup code in tx_initialize_low_level.s.  In addition, the ThreadX SMP timer
is mapped to core 0. To change to another core, please edit arc.c and 
_tx_timer_interrupt.s.  Only one core should be used as the ThreadX SMP
periodic timer interrupt source.


9.  Revision History

For generic code revision information, please refer to the readme_threadx_generic.txt
file, which is included in your distribution. The following details the revision
information associated with this specific port of ThreadX:

04-02-2021  Release 6.1.6 changes:
            tx_port.h                           Updated macro definition

09-30-2020  Initial ThreadX 6.1 for ARC HS using MetaWare tools.


Copyright(c) 1996-2020 Microsoft Corporation


https://azure.com/rtos

