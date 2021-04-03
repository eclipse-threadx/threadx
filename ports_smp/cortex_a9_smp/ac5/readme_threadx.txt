                   Microsoft's Azure RTOS ThreadX SMP for Cortex-A9 

                               Thumb & 32-bit Mode

                          Using the ARM Compiler 5 & DS


1. Import the ThreadX Projects

In order to build the ThreadX SMP library and the ThreadX SMP demonstration, first import 
the 'tx' and 'sample_threadx' projects (located in the "example_build" directory) 
into your DS workspace.

Note: the projects were made using DS-5, so DS will prompt you to migrate the projects.
This is expected, so please do so.


2.  Building the ThreadX SMP run-time Library

Building the ThreadX SMP library is easy; simply select the Eclipse project file 
"tx" and then select the build button. You should now observe the compilation 
and assembly of the ThreadX SMP library. This project build produces the ThreadX SMP 
library file tx.a.


3.  Demonstration System

The ThreadX SMP demonstration is designed to execute under the DS debugger on the
VE_Cortex-A9x4 Bare Metal simulator.

Building the demonstration is easy; simply open the workspace file, select the 
sample_threadx project, and select the build button. Next, expand the demo ThreadX 
project folder in the Project Explorer window, right-click on the 'Cortex-A9x4_tx.launch' 
file, click 'Debug As', and then click 'Cortex-A9x4_tx' from the submenu. This will cause the
debugger to load the sample_threadx.axf ELF file and run to main. You are now ready 
to execute the ThreadX demonstration.


4.  System Initialization

The entry point in ThreadX SMP for the Cortex-A9 using ARM tools is at label 
"ENTRY". This is defined within the ARM compiler's startup code. In addition, 
this is where all static and global pre-set C variable initialization processing 
takes place.

The ThreadX SMP tx_initialize_low_level.s file is responsible for determining the 
first available RAM address for use by the application, which is supplied as the 
sole input parameter to your application definition function, tx_application_define.


5.  Register Usage and Stack Frames

The ARM compiler assumes that registers r0-r3 (a1-a4) and r12 (ip) are scratch 
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
     0x04                   CPSR                        CPSR
     0x08                   r0  (a1)                    r4  (v1)
     0x0C                   r1  (a2)                    r5  (v2)
     0x10                   r2  (a3)                    r6  (v3)
     0x14                   r3  (a4)                    r7  (v4)
     0x18                   r4  (v1)                    r8  (v5)
     0x1C                   r5  (v2)                    r9  (v6)
     0x20                   r6  (v3)                    r10 (v7)
     0x24                   r7  (v4)                    r11 (fp)
     0x28                   r8  (v5)                    r14 (lr)
     0x2C                   r9  (v6)                        
     0x30                   r10 (v7)                        
     0x34                   r11 (fp)                        
     0x38                   r12 (ip)                         
     0x3C                   r14 (lr)
     0x40                   PC 


6.  Improving Performance

The distribution version of ThreadX is built without any compiler 
optimizations. This makes it easy to debug because you can trace or set 
breakpoints inside of ThreadX itself. Of course, this costs some 
performance. To make it run faster, you can change the build_threadx.bat file to 
remove the -g option and enable all compiler optimizations. 

In addition, you can eliminate the ThreadX basic API error checking by 
compiling your application code with the symbol TX_DISABLE_ERROR_CHECKING 
defined. 


7.  Interrupt Handling

ThreadX provides complete and high-performance interrupt handling for Cortex-A9
targets. There are a certain set of requirements that are defined in the 
following sub-sections:


7.1  Vector Area

The Cortex-A9 vectors start at address zero. The demonstration system startup
Init area contains the vectors and is loaded at address zero. On actual 
hardware platforms, this area might have to be copied to address 0. 


8.2  IRQ ISRs

ThreadX fully manages standard and vectored IRQ interrupts. ThreadX also supports nested
IRQ interrupts. The following sub-sections define the IRQ capabilities.


7.2.1 Standard IRQ ISRs

The standard ARM IRQ mechanism has a single interrupt vector at address 0x18. This IRQ 
interrupt is managed by the __tx_irq_handler code in startup.s. The following 
is the default IRQ handler defined in startup.s:

    EXPORT IRQ_Handler
    EXPORT  __tx_irq_processing_return      
IRQ_Handler PROC
;
;    /* Jump to context save to save system context.  */
    B       _tx_thread_context_save             ; Jump to the context save
__tx_irq_processing_return
;
;    /* At this point execution is still in the IRQ mode. The CPSR, point of
;       interrupt, and all C scratch registers are available for use. Note 
;       that IRQ interrupts are still disabled upon return from the context
;       save function.  */
;
;    /* Application ISR call(s) go here!  */
;
;    /* Jump to context restore to restore system context.  */
    B       _tx_thread_context_restore


7.3  FIQ Interrupts

By default, Cortex-A9 FIQ interrupts are left alone by ThreadX. Of course, this 
means that the application is fully responsible for enabling the FIQ interrupt 
and saving/restoring any registers used in the FIQ ISR processing. To globally 
enable FIQ interrupts, the application should enable FIQ interrupts at the 
beginning of each thread or before any threads are created in tx_application_define. 
In addition, the application must ensure that no ThreadX service calls are made 
from default FIQ ISRs, which is located in tx_initialize_low_level.s.


7.3.1  Managed FIQ Interrupts

Full ThreadX management of FIQ interrupts is provided if the ThreadX sources
are built with the TX_ENABLE_FIQ_SUPPORT defined. If the library is built
this way, the FIQ interrupt handlers are very similar to the IRQ interrupt
handlers defined previously. The following is default FIQ handler 
defined in tx_initialize_low_level.s:


    EXPORT  __tx_fiq_handler
    EXPORT  __tx_fiq_processing_return
__tx_fiq_handler
;
;    /* Jump to fiq context save to save system context.  */
    B       _tx_thread_fiq_context_save
__tx_fiq_processing_return:
;
;    /* At this point execution is still in the FIQ mode. The CPSR, point of
;       interrupt, and all C scratch registers are available for use.  */
;
;    /* Application FIQ handlers can be called here!  */
;
;    /* Jump to fiq context restore to restore system context.  */
    B       _tx_thread_fiq_context_restore


8.  ThreadX Timer Interrupt

ThreadX requires a periodic interrupt source to manage all time-slicing, 
thread sleeps, timeouts, and application timers. Without such a timer 
interrupt source, these services are not functional. However, all other
ThreadX services are operational without a periodic timer source.

To add the timer interrupt processing, simply make a call to 
_tx_timer_interrupt in the IRQ processing. An example of this can be
found in the file tx_initialize_low_level.s in the Integrator sub-directories.


9.  Thumb/Cortex-A9 Mixed Mode

By default, ThreadX is setup for running in Cortex-A9 32-bit mode. This is 
also true for the demonstration system. It is possible to build any 
ThreadX file and/or the application in Thumb mode. If any Thumb code 
is used the entire ThreadX source- both C and assembly - should be built 
with the "-apcs /interwork" option.


10. VFP Support

By default, VFP support is disabled for each thread. If saving the context of the VFP registers
is needed, the following API call must be made from the context of the application thread - before 
the VFP usage:

void    tx_thread_vfp_enable(void);

After this API is called in the application, VFP registers will be saved/restored for this thread if it
is preempted via an interrupt. All other suspension of the this thread will not require the VFP registers
to be saved/restored.

To disable VFP register context saving, simply call the following API:

void    tx_thread_vfp_disable(void);


11.  Revision History

For generic code revision information, please refer to the readme_threadx_generic.txt
file, which is included in your distribution. The following details the revision
information associated with this specific port of ThreadX:

04-02-2021  Release 6.1.6 changes:
            tx_port.h                           Updated macro definition

09-30-2020  Initial ThreadX 6.1 version for Cortex-A9 using AC5 tools.


Copyright(c) 1996-2020 Microsoft Corporation


https://azure.com/rtos

