               Microsoft's Azure RTOS ThreadX SMP for Cortex-A5x 

                     Using the Green Hills Software Tools

1. Open the ThreadX SMP Project Workspace

In order to build the ThreadX SMP library and the ThreadX SMP demonstration 
first load the ThreadX SMP project workspace azure_rtos_workspace.gpj, which is 
located inside your ThreadX SMP directory. 


2. Building the ThreadX SMP run-time Library

Building the ThreadX SMP library is easy; simply select the MULTI project file 
tx.gpj and then select the build button. You should now observe the 
compilation and assembly of the ThreadX SMP library. This project build produces 
the ThreadX SMP library file tx.a.


3. Demonstration System

The ThreadX SMP demonstration is designed to execute under the MULTI environment
on the Xilinx UltraScale+ ZCU102 evaluation board. 

Building the demonstration is easy; simply select the MULTI project file 
sample_threadx.gpj. At this point, select the "Project Build" button and observe 
the compilation, assembly, and linkage of the ThreadX SMP demonstration application. 

You are now ready to download the ELF image using the Xilinx tools on the ZCU102
evaluation board.


4. System Initialization

The system entry point using the Green Hills tools is at the label _boot. 
This is defined within the tx_boot.a64 file. In addition, this is where all static 
and global preset C variable initialization processing is called from.

After the Green Hills startup function returns, ThreadX SMP initialization is
called. The main initialization function is _tx_initialize_low_level and
is located in the file tx_initialize_low_level.a64. This function is responsible 
for setting up various system data structures, interrupt vectors, and the 
periodic timer interrupt source of ThreadX SMP.

In addition, _tx_initialize_low_level determines where the first available 
RAM memory address is located. This address is supplied to tx_application_define.

By default, the first available RAM memory address is assumed to start at the 
beginning of the ThreadX SMP section .free_mem. If changes are made to the 
sample_threadx.ld file, the .free_mem section should remain the last allocated 
section in the main RAM area. The starting address of this section is passed 
to tx_application_define.


5. Register Usage and Stack Frames

The 64-bit Green Hills compiler assumes that registers x0-x18 are scratch 
registers for each function. All other registers used by a C function must 
be preserved by the function. ThreadX SMP takes advantage of this in 
situations where a context switch happens as a result of making a ThreadX SMP 
service call (which is itself a C function). In such cases, the saved 
context of a thread is only the non-scratch registers.

The following defines the saved context stack frames for context switches
that occur as a result of interrupt handling or from thread-level API calls.
All suspended threads have one of these two types of stack frames. The top
of the suspended thread's stack is pointed to by tx_thread_stack_ptr in the 
associated thread control block TX_THREAD.

FP not enabled and TX_THREAD.tx_thread_fp_enable == 0:

    Offset        Interrupted Stack Frame        Non-Interrupt Stack Frame

     0x000                  SPSR                        DAIF
     0x008                  ELR                         0
     0x010                  x28                         x27
     0x018                  reserved                    x28
     0x020                  x26                         x25
     0x028                  x27                         x26
     0x030                  x24                         x23
     0x038                  x25                         x24
     0x040                  x22                         x21
     0x048                  x23                         x22
     0x050                  x20                         x19
     0x058                  x21                         x20    
     0x060                  x18                         x29    
     0x068                  x19                         x30    
     0x070                  x16                          
     0x078                  x17
     0x080                  x14
     0x088                  x15
     0x090                  x12
     0x098                  x13
     0x0A0                  x10
     0x0A8                  x11
     0x0B0                  x8
     0x0B8                  x9
     0x0C0                  x6
     0x0C8                  x7
     0x0D0                  x4
     0x0D8                  x5
     0x0E0                  x2
     0x0E8                  x3
     0x0F0                  x0
     0x0F8                  x1
     0x100                  x29
     0x108                  x30 


FP enabled and TX_THREAD.tx_thread_fp_enable == 1:

    Offset        Interrupted Stack Frame        Non-Interrupt Stack Frame

     0x000                  SPSR                        DAIF
     0x008                  ELR                         0
     0x010                  FPSR                        FPSR
     0x018                  FPCR                        FPCR
     0x020                  q30                         q14
     0x030                  q31                         q15
     0x040                  q28                         q12
     0x050                  q29                         q13
     0x060                  q26                         q10
     0x070                  q27                         q11
     0x080                  q24                         q8
     0x090                  q25                         q9
     0x0A0                  q22                         x27
     0x0A8                                              x28
     0x0B0                  q23                         x25
     0x0B8                                              x26
     0x0C0                  q20                         x23
     0x0C8                                              x24
     0x0D0                  q21                         x21
     0x0D8                                              x22
     0x0E0                  q18                         x19
     0x0E8                                              x20
     0x0F0                  q19                         x29
     0x0F8                                              x30
     0x100                  q16
     0x110                  q17
     0x120                  q14
     0x130                  q15
     0x140                  q12
     0x150                  q13
     0x160                  q10
     0x170                  q11
     0x180                  q8
     0x190                  q9
     0x1A0                  q6
     0x1B0                  q7
     0x1C0                  q4
     0x1D0                  q5
     0x1E0                  q2
     0x1F0                  q3
     0x200                  q0
     0x210                  q1
     0x220                  x28                         
     0x228                  reserved                    
     0x230                  x26                         
     0x238                  x27                         
     0x240                  x24                         
     0x248                  x25                         
     0x250                  x22                         
     0x258                  x23                         
     0x260                  x20                         
     0x268                  x21                             
     0x270                  x18                             
     0x278                  x19                             
     0x280                  x16                          
     0x288                  x17
     0x290                  x14
     0x298                  x15
     0x2A0                  x12
     0x2A8                  x13
     0x2B0                  x10
     0x2B8                  x11
     0x2C0                  x8
     0x2C8                  x9
     0x2D0                  x6
     0x2D8                  x7
     0x2E0                  x4
     0x2E8                  x5
     0x2F0                  x2
     0x2F8                  x3
     0x300                  x0
     0x308                  x1
     0x310                  x29
     0x318                  x30 


6. Improving Performance

The distribution version of ThreadX SMP is built without any compiler 
optimizations. This makes it easy to debug because you can trace or set 
breakpoints inside of ThreadX SMP itself. Of course, this costs some 
performance. To make ThreadX SMP run faster, you can change the tx.gpj project
to disable debug information and enable the desired optimizations. 

In addition, you can eliminate the ThreadX SMP basic API error checking by 
compiling your application code with the symbol TX_DISABLE_ERROR_CHECKING 
defined before tx_api.h is included. 


7. Interrupt Handling

ThreadX SMP provides complete and high-performance interrupt handling for Cortex-A5x
targets. Interrupts handlers for the 64-bit mode of the Cortex-A5x have the following
format:

        .global __irq_handler
__irq_handler:
  STP      x29, x30, [sp, #-16]!
  BL       _tx_thread_context_save

  /* Your ISR call goes here!  */
  BL       application_isr_handler

  B        _tx_thread_context_restore

By default, ThreadX SMP assumes EL3 level of execution. Running and taking exceptions in EL1 
and EL2 can be done by simply building the ThreadX library with either EL1 or EL2 defined.


8. ThreadX SMP Timer Interrupt

ThreadX SMP requires a periodic interrupt source to manage all time-slicing, thread sleeps, 
timeouts, and application timers. Without such a timer interrupt source, these services are 
not functional. However, all other ThreadX SMP services are operational without a periodic 
timer source.

To add the timer interrupt processing, simply make a call to _tx_timer_interrupt in the IRQ 
processing. An example of this can be found in the file tx_initialize_low_level.a64.


9. FP Support

By default, FP support is disabled for each thread. If saving the context of the FP registers
is needed, the following API call must be made from the context of the application thread - before 
the FP usage:

void    tx_thread_fp_enable(void);

After this API is called in the application, FP registers will be saved/restored for this thread if it
is preempted via an interrupt. All other suspension of the this thread will not require the FP registers
to be saved/restored.

To disable FP register context saving, simply call the following API:

void    tx_thread_fp_disable(void);


10. Revision History

For generic code revision information, please refer to the readme_threadx_generic.txt
file, which is included in your distribution. The following details the revision
information associated with this specific port of ThreadX SMP:

04-02-2021  Release 6.1.6 changes:
            tx_port.h                           Updated macro definition

09/30/2020  Initial ThreadX SMP version 6.1 of Cortex-A5x/Green Hills port. 


Copyright(c) 1996-2020 Microsoft Corporation


https://azure.com/rtos

