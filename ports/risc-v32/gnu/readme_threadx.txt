                       Microsoft's Azure RTOS ThreadX for RISC-V  

                                     32-bit Mode

                                  Using the IAR Tools

1.  Building the ThreadX run-time Library

Building the ThreadX library is easy. First, open the Azure RTOS workspace
azure_rtos.eww. Next, make the tx.ewp project the "active project" in the 
IAR Embedded Workbench and select the "Make" button. You should observe 
assembly and compilation of a series of ThreadX source files. This 
results in the ThreadX run-time library file tx.a, which is needed by 
the application.


2.  Demonstration System

The ThreadX demonstration is designed to execute under the IAR
Windows-based RISC-V simulator.

Building the demonstration is easy; simply make the sample_threadx.ewp project
the "active project" in the IAR Embedded Workbench and select the 
"Make" button.

You should observe the compilation of sample_threadx.c (which is the demonstration 
application) and linking with tx.a. The resulting file sample_threadx.out is a
binary file that can be downloaded and executed on IAR's RISC-V simulator.


3.  System Initialization

The entry point in ThreadX for the RISC-V using IAR tools is at label 
__iar_program_start. This is defined within the IAR compiler's startup code. In 
addition, this is where all static and global preset C variable 
initialization processing takes place.

The ThreadX tx_initialize_low_level.s file is responsible for setting up 
various system data structures, and a periodic timer interrupt source. 

The _tx_initialize_low_level function inside of tx_initialize_low_level.s
also determines the first available address for use by the application, which 
is supplied as the sole input parameter to your application definition function, 
tx_application_define. To accomplish this, a section is created in 
tx_initialize_low_level.s called FREE_MEM, which must be located after all 
other RAM sections in memory.


4.  Register Usage and Stack Frames

The IAR RISC-V compiler assumes that registers t0-t6 and a0-a7 are scratch 
registers for each function. All other registers used by a C function must 
be preserved by the function. ThreadX takes advantage of this in situations 
where a context switch happens as a result of making a ThreadX service call 
(which is itself a C function). In such cases, the saved context of a thread 
is only the non-scratch registers.

The following defines the saved context stack frames for context switches
that occur as a result of interrupt handling or from thread-level API calls.
All suspended threads have one of these two types of stack frames. The top
of the suspended thread's stack is pointed to by tx_thread_stack_ptr in the 
associated thread control block TX_THREAD.



    Offset        Interrupted Stack Frame        Non-Interrupt Stack Frame

     0x00                   1                           0
     0x04                   s11 (x27)                   s11 (x27)
     0x08                   s10 (x26)                   s10 (x26)
     0x0C                   s9  (x25)                   s9  (x25)
     0x10                   s8  (x24)                   s8  (x24)
     0x14                   s7  (x23)                   s7  (x23)
     0x18                   s6  (x22)                   s6  (x22)
     0x1C                   s5  (x21)                   s5  (x21)
     0x20                   s4  (x20)                   s4  (x20)
     0x24                   s3  (x19)                   s3  (x19)
     0x28                   s2  (x18)                   s2  (x18)
     0x2C                   s1  (x9)                    s1  (x9) 
     0x30                   s0  (x8)                    s0  (x8)    
     0x34                   t6  (x31)                   ra  (x1)
     0x38                   t5  (x30)                   mstatus
     0x3C                   t4  (x29)                   fs0
     0x40                   t3  (x28)                   fs1
     0x44                   t2  (x7)                    fs2
     0x48                   t1  (x6)                    fs3
     0x4C                   t0  (x5)                    fs4
     0x50                   a7  (x17)                   fs5 
     0x54                   a6  (x16)                   fs6 
     0x58                   a5  (x15)                   fs7 
     0x5C                   a4  (x14)                   fs8 
     0x60                   a3  (x13)                   fs9 
     0x64                   a2  (x12)                   fs10 
     0x68                   a1  (x11)                   fs11 
     0x6C                   a0  (x10)                   fcsr
     0x70                   ra  (x1)
     0x74                   reserved
     0x78                   mepc
#if __iar_riscv_base_isa == rv32e 
     0x7C                   ft0
     0x80                   ft1
     0x84                   ft2
     0x88                   ft3
     0x8C                   ft4
     0x90                   ft5
     0x94                   ft6
     0x98                   ft7
     0x9C                   fs0
     0xA0                   fs1
     0xA4                   fa0
     0xA8                   fa1
     0xAC                   fa2
     0xB0                   fa3
     0xB4                   fa4
     0xB8                   fa5
     0xBC                   fa6
     0xC0                   fa7
     0xC4                   fs2
     0xC8                   fs3
     0xCC                   fs4
     0xD0                   fs5
     0xD4                   fs6
     0xD8                   fs7
     0xDC                   fs8
     0xE0                   fs9
     0xE4                   fs10
     0xE8                   fs11
     0xEC                   ft8
     0xF0                   ft9
     0xF4                   ft10
     0xF8                   ft11
     0xFC                   fcsr
#endif


5.  Improving Performance

The distribution version of ThreadX is built without any compiler 
optimizations. This makes it easy to debug because you can trace or set 
breakpoints inside of ThreadX itself. Of course, this costs some 
performance. To make ThreadX run faster, you can change the project 
options to disable debug information and enable the desired 
compiler optimizations. 

In addition, you can eliminate the ThreadX basic API error checking by 
compiling your application code with the symbol TX_DISABLE_ERROR_CHECKING 
defined before tx_api.h is included. 


6.  Interrupt Handling

ThreadX provides complete and high-performance interrupt handling for RISC-V
targets.The ThreadX general exception handler sample is defined as follows, 
where "*" represents the interrupt vector number:

    PUBLIC  _sample_interrupt_handler
    PUBLIC  __minterrupt_00000*
    EXTWEAK __require_minterrupt_vector_table
_sample_interrupt_handler:
__minterrupt_00000*:
    REQUIRE __require_minterrupt_vector_table


    /* Before calling _tx_thread_context_save, we have to allocate an interrupt
       stack frame and save the current value of x1 (ra). */
#if __iar_riscv_base_isa == rv32e 
    addi    sp, sp, -260                            ; Allocate space for all registers - with floating point enabled
#else
    addi    sp, sp, -128                            ; Allocate space for all registers - without floating point enabled
#endif
    sw      x1, 0x70(sp)                            ; Store RA
    call    _tx_thread_context_save                 ; Call ThreadX context save

    /* Call your ISR processing here!  */
    call    your_ISR_processing

    /* Timer interrupt processing is done, jump to ThreadX context restore.  */
    j       _tx_thread_context_restore              ; Jump to ThreadX context restore function. Note: this does not return!


Some additional conditions:

        1. In the project settings Linker -> Extra Options, --auto_vector_setup should be defined.
        2. The project linker control file should have the following sections to include the vector table:

        define block MVECTOR with alignment = 128 { ro section .mintvec };

        if (isdefinedsymbol(_uses_clic))
        {
          define block MINTERRUPT with alignment = 128 { ro section .mtext };
          define block MINTERRUPTS { block MVECTOR,
                                        block MINTERRUPT };
        }
        else
        {
          define block MINTERRUPTS with maximum size =  64k { ro section .mtext,
                                                                   midway block MVECTOR };
        }

6.1 Sample Timer ISR 

The following sample timer ISR using vector 7 is defined in tx_initialize_low_level.s such that timer 
functionality is available under IAR simulation:

    PUBLIC  _tx_timer_interrupt_handler
    PUBLIC  __minterrupt_000007
    EXTWEAK __require_minterrupt_vector_table
_tx_timer_interrupt_handler:
__minterrupt_000007:
    REQUIRE __require_minterrupt_vector_table


    /* Before calling _tx_thread_context_save, we have to allocate an interrupt
       stack frame and save the current value of x1 (ra). */
#if __iar_riscv_base_isa == rv32e 
    addi    sp, sp, -260                            ; Allocate space for all registers - with floating point enabled
#else
    addi    sp, sp, -128                            ; Allocate space for all registers - without floating point enabled
#endif
    sw      x1, 0x70(sp)                            ; Store RA
    call    _tx_thread_context_save                 ; Call ThreadX context save

    /* Call the ThreadX timer routine.  */
    call    _tx_timer_interrupt                     ; Call timer interrupt handler

    /* Timer interrupt processing is done, jump to ThreadX context restore.  */
    j       _tx_thread_context_restore              ; Jump to ThreadX context restore function. Note: this does not return!


7.  Revision History

For generic code revision information, please refer to the readme_threadx_generic.txt
file, which is included in your distribution. The following details the revision
information associated with this specific port of ThreadX:

04-02-2021  Release 6.1.6 changes:
            tx_port.h                           Updated macro definition

08/09/2020  Initial ThreadX version for RISC-V using IAR Tools.


Copyright(c) 1996-2020 Microsoft Corporation


https://azure.com/rtos

