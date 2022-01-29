                Microsoft's Azure RTOS ThreadX for Cortex-R7 

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
on the Green Hills Cortex-R7 simulator. The instructions that follow describe
how to get the ThreadX evaluation running under the MULTI Cortex-R7 simulation 
environment.

Building the demonstration is easy; simply select the MULTI project file 
sample_threadx.gpj. At this point, select the "Project Build" button and observe 
the compilation, assembly, and linkage of the ThreadX demonstration application. 

After the demonstration is built, invoke the  MULTI ARM simulator by selecting 
the simulator connection from within the sample_threadx.con connection file. 
Once connected to the simulator, select the "Debug" button. You should now 
observe the main function of sample_threadx.c. 

At this point, you should setup a simulated timer interrupt for ThreadX
by entering "timer 9999 irq" in the "target" window of the debugger.

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


6. User defines

The following defines and their associated action are as follows:

            Define                                          Meaning

    TX_ENABLE_IRQ_NESTING                       If defined, this brings in special IRQ
                                                interrupt nesting logic into the ThreadX
                                                library. This define should be applied
                                                to the entire ThreadX library.

    TX_ENABLE_FIQ_NESTING                       If defined, this brings in special FIQ
                                                interrupt nesting logic into the ThreadX
                                                library. This define should be applied
                                                to the entire ThreadX library and the 
                                                define TX_ENABLE_FIQ_SUPPORT should also
                                                be defined.

    TX_ENABLE_FIQ_SUPPORT                       If defined, this brings in FIQ context
                                                save and restore logic necessary for
                                                applications to call ThreadX services from
                                                FIQ interrupt handlers. This define 
                                                should be applied to the entire ThreadX 
                                                library.


    TX_DISABLE_ERROR_CHECKING                   If defined before tx_api.h is included,
                                                this define causes basic ThreadX error
                                                checking to be disabled. Please see
                                                Chapter 4 in the "ThreadX User Guide" 
                                                for more details.

    TX_ENABLE_EVENT_LOGGING                     This define enables event logging for any
                                                or all of the ThreadX source code. If this
                                                option is used anywhere, the tx_initialize_high_level.c 
                                                file must be compiled with it as well, since this
                                                is where the event log is initialized.

    TX_NO_EVENT_INFO                            This is a sub-option for event logging.
                                                If this is enabled, only basic information
                                                is saved in the log.

    TX_ENABLE_EVENT_FILTERS                     This is also a sub-option for event-logging.
                                                If this is enabled, run-time filtering logic
                                                is added to the event logging code.

    TX_MAX_PRIORITIES                           Defines the priority levels for ThreadX. 
                                                Legal values range from 32 through 
                                                1024 (inclusive) and MUST be evenly divisible 
                                                by 32. Increasing the number of priority levels 
                                                supported increases the RAM usage by 128 bytes 
                                                for every group of 32 priorities. However, there 
                                                is only a negligible effect on performance. By 
                                                default, this value is set to 32 priority levels.

    TX_MINIMUM_STACK                            Defines the minimum stack size (in bytes). It is 
                                                used for error checking when threads are created. 
                                                The default value is port-specific and is found 
                                                in tx_port.h.

    TX_TIMER_THREAD_STACK_SIZE                  Defines the stack size (in bytes) of the internal 
                                                ThreadX timer thread. This thread processes all 
                                                thread sleep requests as well as all service call 
                                                timeouts. In addition, all application timer callback 
                                                routines are invoked from this context. The default 
                                                value is port-specific and is found in tx_port.h.

    TX_TIMER_THREAD_PRIORITY                    Defines the priority of the internal ThreadX timer 
                                                thread. The default value is priority 0 - the highest 
                                                priority in ThreadX. The default value is defined 
                                                in tx_port.h.

    TX_TIMER_PROCESS_IN_ISR                     Defined, this option eliminates the internal system 
                                                timer thread for ThreadX. This results in improved 
                                                performance on timer events and smaller RAM requirements 
                                                because the timer stack and control block are no 
                                                longer needed. However, using this option moves all 
                                                the timer expiration processing to the timer ISR level. 
                                                By default, this option is not defined.

    TX_REACTIVATE_INLINE                        Defined, this option performs reactivation of ThreadX 
                                                timers in-line instead of using a function call. This 
                                                improves performance but slightly increases code size. 
                                                By default, this option is not defined.

    TX_DISABLE_STACK_FILLING                    Defined, placing the 0xEF value in each byte of each 
                                                thread's stack is disabled. By default, this option is 
                                                not defined.

    TX_ENABLE_STACK_CHECKING                    Defined, this option enables ThreadX run-time stack checking, 
                                                which includes analysis of how much stack has been used and 
                                                examination of data pattern "fences" before and after the 
                                                stack area. If a stack error is detected, the registered 
                                                application stack error handler is called. This option does 
                                                result in slightly increased overhead and code size. Please 
                                                review the tx_thread_stack_error_notify API for more information. 
                                                By default, this option is not defined.

    TX_DISABLE_PREEMPTION_THRESHOLD             Defined, this option disables the preemption-threshold feature 
                                                and slightly reduces code size and improves performance. Of course, 
                                                the preemption-threshold capabilities are no longer available. 
                                                By default, this option is not defined.

    TX_DISABLE_REDUNDANT_CLEARING               Defined, this option removes the logic for initializing ThreadX 
                                                global C data structures to zero. This should only be used if 
                                                the compiler's initialization code sets all un-initialized 
                                                C global data to zero. Using this option slightly reduces 
                                                code size and improves performance during initialization. 
                                                By default, this option is not defined.

    TX_DISABLE_NOTIFY_CALLBACKS                 Defined, this option disables the notify callbacks for various 
                                                ThreadX objects. Using this option slightly reduces code size 
                                                and improves performance.

    TX_BLOCK_POOL_ENABLE_PERFORMANCE_INFO       Defined, this option enables the gathering of performance 
                                                information on block pools. By default, this option is 
                                                not defined.

    TX_BYTE_POOL_ENABLE_PERFORMANCE_INFO        Defined, this option enables the gathering of performance 
                                                information on byte pools. By default, this option is 
                                                not defined.

    TX_EVENT_FLAGS_ENABLE_PERFORMANCE_INFO      Defined, this option enables the gathering of performance 
                                                information on event flags groups. By default, this option 
                                                is not defined.

    TX_MUTEX_ENABLE_PERFORMANCE_INFO            Defined, this option enables the gathering of performance 
                                                information on mutexes. By default, this option is 
                                                not defined.

    TX_QUEUE_ENABLE_PERFORMANCE_INFO            Defined, this option enables the gathering of performance 
                                                information on queues. By default, this option is 
                                                not defined.

    TX_SEMAPHORE_ENABLE_PERFORMANCE_INFO        Defined, this option enables the gathering of performance 
                                                information on semaphores. By default, this option is 
                                                not defined.

    TX_THREAD_ENABLE_PERFORMANCE_INFO           Defined, this option enables the gathering of performance 
                                                information on threads. By default, this option is 
                                                not defined.

    TX_TIMER_ENABLE_PERFORMANCE_INFO            Defined, this option enables the gathering of performance 
                                                information on timers. By default, this option is 
                                                not defined.



7. Register Usage and Stack Frames

The Green Hills compiler assumes that registers r0-r3 (a1-a4) and r12 (ip) 
are scratch registers for each function. All other registers used by a C 
function must be preserved by the function. ThreadX takes advantage of this 
in situations where a context switch happens as a result of making a ThreadX 
service call (which is itself a C function). In such cases, the saved 
context of a thread is only the non-scratch registers.

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


8. Improving Performance

The distribution version of ThreadX is built without any compiler 
optimizations. This makes it easy to debug because you can trace or set 
breakpoints inside of ThreadX itself. Of course, this costs some 
performance. To make ThreadX run faster, you can change the tx.gpj project
to disable debug information and enable the desired optimizations. 

In addition, you can eliminate the ThreadX basic API error checking by 
compiling your application code with the symbol TX_DISABLE_ERROR_CHECKING 
defined before tx_api.h is included. 


9. Interrupt Handling

ThreadX provides complete and high-performance interrupt handling for Cortex-R7
targets. There are a certain set of requirements that are defined in the 
following sub-sections:


9.1 Vector Area

The Cortex-R7 vectors start at address zero. The demonstration system reset.arm
file contains the reset section (which contains all the ARM vectors) and is 
typically loaded at address zero. On actual hardware platforms, this section
might have to be copied to address 0. 

9.2 IRQ ISRs

ThreadX fully manages standard and vectored IRQ interrupts. ThreadX also supports nested
IRQ interrupts. The following sub-sections define the IRQ capabilities.


9.2.1 Standard IRQ ISRs

The standard ARM IRQ mechanism has a single interrupt vector at address 0x18. This IRQ 
interrupt is managed by the __tx_irq_handler code in tx_initialize_low_level. The following 
is the default IRQ handler defined in tx_initialize_low_level.arm:


    .globl  __tx_irq_handler
    .globl  __tx_irq_processing_return      
__tx_irq_handler:

    /* Jump to context save to save system context. */
    B       _tx_thread_context_save
__tx_irq_processing_return:

    /* At this point execution is still in the IRQ mode. The CPSR, point of
       interrupt, and all C scratch registers are available for use. Note 
       that IRQ interrupts are still disabled upon return from the context
       save function. */

    /* Application ISR call(s) go here!  */

    /* Jump to context restore to restore system context. */
    B       _tx_thread_context_restore


9.2.2 Vectored IRQ ISRs

The vectored ARM IRQ mechanism has multiple interrupt vectors at addresses specified
by the particular implementation. The following is an example IRQ handler defined in 
tx_initialize_low_level.arm:

    .globl  __tx_irq_example_handler
__tx_irq_example_handler:

    /* Call context save to save system context. */

    STMDB   sp!, {r0-r3}                        # Save some scratch registers
    MRS     r0, SPSR                            # Pickup saved SPSR
    SUB     lr, lr, #4                          # Adjust point of interrupt 
    STMDB   sp!, {r0, r10, r12, lr}             # Store other scratch registers
    BL      _tx_thread_vectored_context_save    # Call the vectored IRQ context save

    /* At this point execution is still in the IRQ mode. The CPSR, point of
       interrupt, and all C scratch registers are available for use. Note 
       that IRQ interrupts are still disabled upon return from the context
       save function. */

    /* Application ISR call goes here!  */

    /* Jump to context restore to restore system context. */
    B       _tx_thread_context_restore


9.2.3  Nested IRQ Support

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
setup in tx_initialize_low_level.arm. When nested IRQ interrupts are no 
longer required, calling the _tx_thread_irq_nesting_end service disables nesting 
by disabling IRQ interrupts and switching back to IRQ mode in preparation for 
the IRQ context restore service.

The following is an example of enabling IRQ nested interrupts in the 
typical IRQ handler:

    .globl  __tx_irq_handler
    .globl  __tx_irq_processing_return      
__tx_irq_handler:

    /* Jump to context save to save system context. */
    B       _tx_thread_context_save
__tx_irq_processing_return:

    /* Enable nested IRQ interrupts. NOTE:  Since this service returns
       with IRQ interrupts enabled, all IRQ interrupt sources must be 
       cleared prior to calling this service. */
    BL      _tx_thread_irq_nesting_start
    
    /* Application ISR call(s) go here!  */

    /* Disable nested IRQ interrupts. The mode is switched back to
       IRQ mode and IRQ interrupts are disable upon return. */
    BL      _tx_thread_irq_nesting_end

    /* Jump to context restore to restore system context. */
    B       _tx_thread_context_restore


9.3 FIQ Interrupts

By default, Cortex-R7 FIQ interrupts are left completely enabled by ThreadX. 
Of course, this means that the application is fully responsible for 
saving/restoring any registers used in the FIQ ISR processing. In addition, 
no ThreadX service calls are allowed from the default FIQ ISRs. The default
FIQ interrupt shell is located in tx_initialize_low_level.arm.

9.3.1 Managed FIQ Interrupts

Full ThreadX management of FIQ interrupts is provided if the ThreadX sources
are built with the TX_ENABLE_FIQ_SUPPORT defined. If the library is built
this way, the FIQ interrupt handlers are very similar to the IRQ interrupt
handlers defined previously. The following is default FIQ handler 
defined in tx_initialize_low_level.arm:


    .globl  __tx_fiq_handler
    .globl  __tx_fiq_processing_return
__tx_fiq_handler:

    /* Jump to fiq context save to save system context. */
    B       _tx_thread_fiq_context_save
__tx_fiq_processing_return:

    /* At this point execution is still in the FIQ mode. The CPSR, point of
       interrupt, and all C scratch registers are available for use. */

    /* Application FIQ handlers can be called here!  */

    /* Jump to fiq context restore to restore system context. */
    B       _tx_thread_fiq_context_restore


9.3.1.1 Nested FIQ Support

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
setup in tx_initialize_low_level.arm. When nested FIQ interrupts are no longer 
required, calling the _tx_thread_fiq_nesting_end service disables nesting by 
disabling FIQ interrupts and switching back to FIQ mode in preparation for the FIQ 
context restore service.

The following is an example of enabling FIQ nested interrupts in the 
typical FIQ handler:


    .globl  __tx_fiq_handler
    .globl  __tx_fiq_processing_return
__tx_fiq_handler:

    /* Jump to fiq context save to save system context. */
    B       _tx_thread_fiq_context_save
__tx_fiq_processing_return:

    /* At this point execution is still in the FIQ mode. The CPSR, point of
       interrupt, and all C scratch registers are available for use. */

    /* Enable nested FIQ interrupts. NOTE:  Since this service returns
       with FIQ interrupts enabled, all FIQ interrupt sources must be 
       cleared prior to calling this service. */
    BL      _tx_thread_fiq_nesting_start

    /* Application FIQ handlers can be called here!  */

    /* Disable nested FIQ interrupts. The mode is switched back to
       FIQ mode and FIQ interrupts are disable upon return. */
    BL      _tx_thread_fiq_nesting_end

    /* Jump to fiq context restore to restore system context. */
    B       _tx_thread_fiq_context_restore



10. ThreadX Timer Interrupt

ThreadX requires a periodic interrupt source to manage all time-slicing, 
thread sleeps, timeouts, and application timers. Without such a timer 
interrupt source, these services are not functional. However, all other
ThreadX services are operational without a periodic timer source.

To add the timer interrupt processing, simply make a call to 
_tx_timer_interrupt in the IRQ processing. An example of this can be
found in the file tx_initialize_low_level.arm.


11. Thumb/Cortex-R7 Mixed Mode

By default, ThreadX is setup for running in Cortex-R7 32-bit mode. This is 
also true for the demonstration system. It is possible to build any 
ThreadX file and/or the application in Thumb mode. The only exception
to this is the file tx_thread_shell_entry.c. This file must always be 
built in 32-bit mode.


12. VFP Support

By default, VFP support is disabled for each thread. If saving the context of the VFP registers
is needed, the following API call must be made from the context of the application thread - before 
the VFP usage:

void    tx_thread_vfp_enable(void);

After this API is called in the application, VFP registers will be saved/restored for this thread if it
is preempted via an interrupt. All other suspension of the this thread will not require the VFP registers
to be saved/restored.

To disable VFP register context saving, simply call the following API:

void    tx_thread_vfp_disable(void);


13. Revision History

For generic code revision information, please refer to the readme_threadx_generic.txt
file, which is included in your distribution. The following details the revision
information associated with this specific port of ThreadX:

04-02-2021  Release 6.1.6 changes:
            tx_port.h                           Updated macro definition

05/19/2020  Initial ThreadX version of Cortex-R7/Green Hills port. 


Copyright(c) 1996-2020 Microsoft Corporation


https://azure.com/rtos

