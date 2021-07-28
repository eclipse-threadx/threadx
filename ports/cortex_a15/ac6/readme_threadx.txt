                   Microsoft's Azure RTOS ThreadX for Cortex-A15 

                              Using the AC6 Tools

1. Import the ThreadX Projects

In order to build the ThreadX library and the ThreadX demonstration, first import 
the 'tx' and 'sample_threadx' projects (located in the "example_build" directory) 
into your DS workspace.


2.  Building the ThreadX run-time Library

Building the ThreadX library is easy; simply right-click the Eclipse project 
"tx" and then select the "Build Project" button. You should now observe the compilation 
and assembly of the ThreadX library. This project build produces the ThreadX
library file tx.a.


3.  Demonstration System

The ThreadX demonstration is designed to execute under the DS debugger on the
VE_Cortex-A15 Bare Metal simulator.

Building the demonstration is easy; simply right-click the Eclipse project 
"sample_threadx" and then select the "Build Project" button. You should now observe 
the compilation and assembly of the ThreadX demonstration. This project build produces 
the ThreadX library file sample_threadx.axf. Next, expand the demo ThreadX project folder 
in the Project Explorer window, right-click on the 'cortex-a15_tx.launch' file, click
'Debug As', and then click 'cortex-a15_tx' from the submenu. This will cause the
debugger to load the sample_threadx.axf ELF file and run to main. You are now ready 
to execute the ThreadX demonstration.


4.  System Initialization

The entry point in ThreadX for the Cortex-A15 using ARM tools is at label 
"Vectors". This is defined within startup.S in the sample_threadx project. In addition, 
this is where all static and global pre-set C variable initialization processing 
takes place.

This is also where initialization of a periodic timer interrupt source should take 
place.

In addition, _tx_initialize_low_level defines the first available address 
for use by the application, which is supplied as the sole input parameter 
to your application definition function, tx_application_define.


5.  Register Usage and Stack Frames

The AC6 compiler assumes that registers r0-r3 (a1-a4) and r12 (ip) are scratch 
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
     0x08                   r0  (a1)                    a15  (v1)
     0x0C                   r1  (a2)                    r5  (v2)
     0x10                   r2  (a3)                    r6  (v3)
     0x14                   r3  (a4)                    r7  (v4)
     0x18                   a15  (v1)                    r8  (v5)
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

In addition, you can eliminate the ThreadX basic API error checking by 
compiling your application code with the symbol TX_DISABLE_ERROR_CHECKING 
defined. 


7.  Interrupt Handling

ThreadX provides complete and high-performance interrupt handling for Cortex-A15
targets. There are a certain set of requirements that are defined in the 
following sub-sections:


7.1  Vector Area

The Cortex-A15 vectors start at address zero. The demonstration system startup
reset.S file contains the vectors and is loaded at address zero. On actual 
hardware platforms, this area might have to be copied to address 0. 


7.2  IRQ ISRs

ThreadX fully manages standard and vectored IRQ interrupts. ThreadX also supports 
nested IRQ interrupts. The following sub-sections define the IRQ capabilities.


7.2.1 Standard IRQ ISRs

The standard ARM IRQ mechanism has a single interrupt vector at address 0x18. This IRQ 
interrupt is managed by the __tx_irq_handler code in tx_initialize_low_level. The following 
is the default IRQ handler defined in tx_initialize_low_level.S:

    .global __tx_irq_handler
    .global __tx_irq_processing_return      
__tx_irq_handler:
@
@    /* Jump to context save to save system context.  */
    B       _tx_thread_context_save             @ Jump to the context save
__tx_irq_processing_return:
@
@    /* At this point execution is still in the IRQ mode. The CPSR, point of
@       interrupt, and all C scratch registers are available for use. Note 
@       that IRQ interrupts are still disabled upon return from the context
@       save function.  */
@
@    /* Application ISR call(s) go here!  */
@
@    /* Jump to context restore to restore system context.  */
    B       _tx_thread_context_restore


7.2.2 Vectored IRQ ISRs

The vectored ARM IRQ mechanism has multiple interrupt vectors at addresses specified
by the particular implementation. The following is an example IRQ handler defined in 
tx_initialize_low_level.S:

    .global __tx_irq_example_handler
__tx_irq_example_handler:
@
@    /* Call context save to save system context.  */

    STMDB   sp!, {r0-r3}                        @ Save some scratch registers
    MRS     r0, SPSR                            @ Pickup saved SPSR
    SUB     lr, lr, #4                          @ Adjust point of interrupt 
    STMDB   sp!, {r0, r10, r12, lr}             @ Store other scratch registers
    BL      _tx_thread_vectored_context_save    @ Call the vectored IRQ context save
@
@    /* At this point execution is still in the IRQ mode. The CPSR, point of
@       interrupt, and all C scratch registers are available for use. Note 
@       that IRQ interrupts are still disabled upon return from the context
@       save function.  */
@
@    /* Application ISR call goes here!  */
@
@    /* Jump to context restore to restore system context.  */
    B       _tx_thread_context_restore


7.2.3  Nested IRQ Support

By default, nested IRQ interrupt support is not enabled. To enable nested
IRQ support, the entire library should be built with TX_ENABLE_IRQ_NESTING
defined. With this defined, two new IRQ interrupt management services are
available, namely _tx_thread_irq_nesting_start and _tx_thread_irq_nesting_end.
These function should be called between the IRQ context save and restore
calls. 

Execution between the calls to _tx_thread_irq_nesting_start and 
_tx_thread_irq_nesting_end is enabled for IRQ nesting. This is achieved 
by switching from IRQ mode to SYS mode and enabling IRQ interrupts.
The SYS mode stack is used during the SYS mode operation, which was 
setup in tx_initialize_low_level.S. When nested IRQ interrupts are no 
longer required, calling the _tx_thread_irq_nesting_end service disables 
nesting by disabling IRQ interrupts and switching back to IRQ mode in 
preparation for the IRQ context restore service.

The following is an example of enabling IRQ nested interrupts in a standard 
IRQ handler:

    .global __tx_irq_handler
    .global __tx_irq_processing_return      
__tx_irq_handler:
@
@    /* Jump to context save to save system context.  */
    B       _tx_thread_context_save
__tx_irq_processing_return:
@
@    /* Enable nested IRQ interrupts. NOTE:  Since this service returns
@       with IRQ interrupts enabled, all IRQ interrupt sources must be 
@       cleared prior to calling this service.  */
    BL      _tx_thread_irq_nesting_start
@    
@    /* Application ISR call(s) go here!  */
@
@    /* Disable nested IRQ interrupts. The mode is switched back to
@       IRQ mode and IRQ interrupts are disable upon return.  */
    BL      _tx_thread_irq_nesting_end
@
@    /* Jump to context restore to restore system context.  */
    B       _tx_thread_context_restore


7.3  FIQ Interrupts

By default, FIQ interrupts are left alone by ThreadX. Of course, this 
means that the application is fully responsible for enabling the FIQ interrupt 
and saving/restoring any registers used in the FIQ ISR processing. To globally 
enable FIQ interrupts, the application should enable FIQ interrupts at the 
beginning of each thread or before any threads are created in tx_application_define. 
In addition, the application must ensure that no ThreadX service calls are made 
from default FIQ ISRs, which is located in tx_initialize_low_level.S.


7.3.1  Managed FIQ Interrupts

Full ThreadX management of FIQ interrupts is provided if the ThreadX sources
are built with the TX_ENABLE_FIQ_SUPPORT defined. If the library is built
this way, the FIQ interrupt handlers are very similar to the IRQ interrupt
handlers defined previously. The following is default FIQ handler 
defined in tx_initialize_low_level.S:


    .global __tx_fiq_handler
    .global __tx_fiq_processing_return
__tx_fiq_handler:
@
@    /* Jump to fiq context save to save system context.  */
    B       _tx_thread_fiq_context_save
__tx_fiq_processing_return:
@
@    /* At this point execution is still in the FIQ mode. The CPSR, point of
@       interrupt, and all C scratch registers are available for use.  */
@
@    /* Application FIQ handlers can be called here!  */
@
@    /* Jump to fiq context restore to restore system context.  */
    B       _tx_thread_fiq_context_restore


7.3.1.1 Nested FIQ Support

By default, nested FIQ interrupt support is not enabled. To enable nested
FIQ support, the entire library should be built with TX_ENABLE_FIQ_NESTING
defined. With this defined, two new FIQ interrupt management services are
available, namely _tx_thread_fiq_nesting_start and _tx_thread_fiq_nesting_end.
These function should be called between the FIQ context save and restore
calls. 

Execution between the calls to _tx_thread_fiq_nesting_start and 
_tx_thread_fiq_nesting_end is enabled for FIQ nesting. This is achieved 
by switching from FIQ mode to SYS mode and enabling FIQ interrupts.
The SYS mode stack is used during the SYS mode operation, which was 
setup in tx_initialize_low_level.S. When nested FIQ interrupts are no longer required,
calling the _tx_thread_fiq_nesting_end service disables nesting by disabling
FIQ interrupts and switching back to FIQ mode in preparation for the FIQ 
context restore service.

The following is an example of enabling FIQ nested interrupts in the 
typical FIQ handler:


    .global __tx_fiq_handler
    .global __tx_fiq_processing_return
__tx_fiq_handler:
@
@    /* Jump to fiq context save to save system context.  */
    B       _tx_thread_fiq_context_save
__tx_fiq_processing_return:
@
@    /* At this point execution is still in the FIQ mode. The CPSR, point of
@       interrupt, and all C scratch registers are available for use.  */
@
@    /* Enable nested FIQ interrupts. NOTE:  Since this service returns
@       with FIQ interrupts enabled, all FIQ interrupt sources must be 
@       cleared prior to calling this service.  */
    BL      _tx_thread_fiq_nesting_start
@
@    /* Application FIQ handlers can be called here!  */
@
@    /* Disable nested FIQ interrupts. The mode is switched back to
@       FIQ mode and FIQ interrupts are disable upon return.  */
    BL      _tx_thread_fiq_nesting_end
@
@    /* Jump to fiq context restore to restore system context.  */
    B       _tx_thread_fiq_context_restore


8.  ThreadX Timer Interrupt

ThreadX requires a periodic interrupt source to manage all time-slicing, 
thread sleeps, timeouts, and application timers. Without such a timer 
interrupt source, these services are not functional but the remainder of 
ThreadX will still run.

To add the timer interrupt processing, simply make a call to 
_tx_timer_interrupt in the IRQ processing. An example of this can be
found in the file tx_initialize_low_level.S for the demonstration system.


9. VFP Support

By default, VFP support is disabled for each thread. If saving the context of the VFP registers
is needed, the following API call must be made from the context of the application thread - before 
the VFP usage:

void    tx_thread_vfp_enable(void);

After this API is called in the application, VFP registers will be saved/restored for this thread if it
is preempted via an interrupt. All other suspension of the this thread will not require the VFP registers
to be saved/restored.

To disable VFP register context saving, simply call the following API:

void    tx_thread_vfp_disable(void);


10.  Revision History

For generic code revision information, please refer to the readme_threadx_generic.txt
file, which is included in your distribution. The following details the revision
information associated with this specific port of ThreadX:

04-02-2021  Release 6.1.6 changes:
            tx_port.h                           Updated macro definition

09-30-2020  Initial ThreadX 6.1 version for Cortex-A15 using AC6 tools.


Copyright(c) 1996-2020 Microsoft Corporation


https://azure.com/rtos

