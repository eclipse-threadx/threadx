           Microsoft's ThreadX for Xtensa Configurable Processors

                            Using the Xtensa Tools

1.  Xtensa Configuration Requirements and Restrictions

The Xtensa configurable architecture supports a vast space of processor
features. This port of ThreadX to the Xtensa architecture is based on
a Cadence Design Systems RTOS porting layer that takes care of Xtensa specifics 
that are common to most embedded real-time operating systems. It supports all
Xtensa features (including context-switching custom processor extensions
defined in the TIE language) with certain minimum requirements. You
must use Xtensa Tools to compile and link ThreadX for your particular
configuration. ThreadX uses the Xtensa Hardware Abstraction Layer (HAL)
to adapt your ThreadX compilation to your particular Xtensa processor
configuration. ThreadX also provides optional thread-safe support for
the Xtensa C library and the newlib C library distributed with Xtensa Tools
(for use in threads only, not in interrupt handlers).

ThreadX for Xtensa configurable processors requires the following minimum 
processor configuration options:
- Timer interrupt option with at least one interruptible timer for ThreadX.
- Interrupt option (implied by the timer interrupt option).
- Xtensa Exception Architecture 2 (XEA2) or Exception Architecture 3 (XEA3).
  Please note that XEA1 is NOT supported.

Minimal support for certain evaluation boards is provided via a board
independent "XTBSP" API implemented by a board specific library. This
provides the board clock frequency and basic polled drivers for the display
and console I/O device (usually a UART), and ThreadX can be built to
take advantage of these (note: it is not specific to ThreadX and is not a
traditional RTOS "board support package" with RTOS specific interrupt-driven
drivers). Note that ThreadX can run on any Xtensa board without
this board support (a "raw" platform), but you will have to provide the
clock frequency and drivers for any on-board devices you want to use.


2.  Building the ThreadX run-time Library

By default, you will build for the Xtensa instruction set simulator. If
you have a supported emulation board, you can build to run on that. You
can also build to run on a raw Xtensa core with no "board support", a
good starting point for supporting your own target platform. Cadence Design Systems
recommends doing functional development on the simulator because it
is easier to debug with, then move to a board if/when you need to test
hardware drivers or real-time performance.

ThreadX/Xtensa comes with a Makefile as well as DOS .bat file, taking
advantage of xt-make, a version of GNU make that comes with Xtensa Tools
(you can just as well use generic GNU make if you have it installed).
The Makefile works on any host platform and provides for incremental builds.
The .bat file works on DOS/Windows only and always cleans before building.
The build for each target platform is placed in a sub-directory so several
platform builds can coexist even with incremental rebuilds.

First, make sure you have installed Xtensa Tools and your processor
configuration, and be sure that Xtensa Tools are in your search path.

If you wish to build for an evaluation board that is supported by an
external package, be sure the appropriate package is installed. See the
introduction (section 0) to determine if you need an external package.
If you are using an external board package, set the environment variable
XTENSA_BOARDS to the absolute path of the root of the installed support 
package (or you can pass this to xt-make commands on the command line).
eg. XTENSA_BOARDS = C:\usr\xtensa\RB-2007.1-xtav60 for Avnet LX60 board.
You do not need to set XTENSA_BOARDS if using a Cadence Design Systems supported 
board with Xtensa Tools RB-2007.2 and up (support is bundled with the tools).

Next, change directories to the ThreadX installation directory, as follows:

> cd c:\threadx\xtensa\gnu

Now build the ThreadX library (tx.a) by executing the build_threadx.bat
batch file (or using the Makefile directly) while inside the threadx
directory, as follows:

> build_threadx.bat

which always builds for the simulator (but you can edit it), or

> xt-make

which by default builds for the simulator (PLATFORM=sim), or either of

> xt-make PLATFORM=board BOARD=<board>
or
> xt-make BOARD=<board>

which builds for a specified supported board (note PLATFORM=board is
default when BOARD is defined). eg. BOARD=xtav60 for the Avnet LX60
(XT-AV60) board.

> xt-make PLATFORM=raw
 
which builds for a raw Xtensa core with no "board support".

> xt-make PLATFORM=gdbio

Provides some very slow I/O support through the xt-gdb debugger.
For GDBIO to work, xt-gdb must remain connected to the target.

If you are building for an Xtensa processor configuration that is not the
default you selected when you installed Xtensa Tools, you need to define the 
environment variable XTENSA_CORE. If your configuration is not in the 
default registry you selected when you installed Xtensa Tools, you also
need to define the environment variable XTENSA_SYSTEM. See tools manuals.
You can avoid defining these in your environment if you pass the variables 
you need to redefine into xt-make as follows:

> xt-make XTENSA_CORE=<your_config_name> XTENSA_SYSTEM=<your_registry> ...

There are more details about build options in the comment in the Makefile.

At this point, all the ThreadX objects are located in a library file: tx.a .
This file must be linked with your application in order to use ThreadX.
This library and all the intermediate object files are placed in a platform
specific sub-directory named the same as BOARD or PLATFORM (if BOARD is not
defined), for example "sim", "xtkc705".

To build ThreadX with thread-safe C library support, define TX_THREAD_SAFE_CLIB
in your build, as described in section 5 and in the Makefile. Please note
that the C library is only safe for use in threads, not in interrupt handlers.
It may also safely be used in tx_application_define (after tx_kernel_enter, 
before threads are running).


3.  Demonstration System

The ThreadX demonstration is designed to execute under Xtensa instruction set
simulator (ISS) or on a supported evaluation board programmed with your Xtensa
processor configuration.

Building the demonstration is easy, simply execute the build_threadx_demo.bat 
batch file while inside threadx directory, as follows:

> build_threadx_demo.bat 

or

> xt-make demo

Be sure to set or pass into xt-make the variables described in section 2 above
for building the ThreadX library, including the PLATFORM or BOARD you want to
run on.

This compiles demo_threadx.c (which is the demonstration application) and links 
it with the ThreadX objects in tx.a. The resulting file demo_threadx.out is a 
ELF binary file that can be downloaded and executed on the target.

The demo binary appears in the platform specific sub-directory described earlier.
For the following commands, change to that directory or prepend it as the path
of demo_threadx.out.


To execute on the simulator:

> xt-run [--turbo] demo_threadx.out

The option --turbo provides much faster, but non-cycle-accurate simulation.
Refer to the Xtensa simulator user manual for further simulator options.


To execute on the simulator using the Xplorer GUI based debugger:

> xplorer --debug demo_threadx.out


To execute on a supported evaluation board, download demo_threadx.out per
instructions in the tools manuals. Be sure the board has been programmed
with the correct configuration and is set up to boot from RAM and debug
a downloaded program!  Optionally you may connect a terminal or terminal
emulator to the serial port on the board with settings as described in
the board user manual, and see the output of printf on the terminal (if
the demo was compiled with DEMO_USE_PRINTF or DEMO_USE_XTUTIL).

To obtain I/O on a "raw" platform such as an unsupported board, you need
to provide low level I/O drivers (eg. inbyte() and outbyte() for character
I/O if you want to use printf etc.).  You can run "raw" executables on
any Xtensa platform, including simulator and any board, but you will not
see any behavior specific to the platform (eg. display, printed output,
stopping simulation at end of program). You can, while debugging, use a
debugger mechanism called GDBIO to obtain basic I/O. Use PLATFORM=GDBIO
on your xt-make command line - this is the same as "raw" except it links
some stubs that communicate through the debugger. It is very slow!

WARNING: It is tempting to add printf calls to other threads in the demo.
If you modify the code in any way, you may need adjust affected threads' 
stack sizes. This is especially true if you add a printf call. See 4.5.


4.  System Initialization

4.1 ThreadX Kernel

ThreadX uses the reset vector and low level initialization from the Xtensa
libraries provided by the linker support package (LSP). It does not provide
its own reset vector (which would have to be maintained with tools upgrades).

ThreadX is initialized by tx_kernel_enter which is usually called from
main. The primary initialization function is _tx_initialize_low_level
and is located in the file tx_initialize_low_level.c. This function is
responsible for setting up various system data structures. Interrupt and
exception vectors are linked into your program and installed at load time
(they are not dynamically installed). The interrupt and exception vectors
for the ThreadX Xtensa port are defined in xtensa_vectors.S (for XEA2) and
xtensa_vectors_xea3.S (for XEA3).

4.2 Memory Allocation

In addition, _tx_initialize_low_level also determines the first available 
address for use by the application. By default, the first available address 
is assumed to be above all linker-allocated sections at symbol _end. This 
is passed to the application definition function, tx_application_define.
This is a convenience to the application developer. Ultimately the developer
has full control over memory allocation and can choose to use this or not.
The top of available memory is not provided and should be obtained from the
tools if needed.

4.3 Location and Extent of System (Interrupt) Stack

ThreadX/Xtensa supports a user-definable system stack. By default, the
top of the system stack is defined by the symbol _xt_interrupt_stack_top.
See the file xtensa_intr_asm.S for the default system stack definition.
This stack may be resized and/or relocated according to the application
needs. The application developer must ensure that the system stack is
sized appropriately for the application. All interrupts handled by 
ThreadX will use the system stack. Handlers written in assembly must
not switch to the system stack, since it will not be possible to detect
whether the stack is currently in use.

The macro TX_SYSTEM_STACK_SIZE defines the size of the system stack.
As a convenience, a macro TX_MINIMUM_STACK_SYSTEM is provided with the
minimum size required for the system stack. This is based on the maximum
possible interrupt nesting level per the Xtensa processor configuration,
assuming very simple C handlers that do not call deeper than one or two 
levels.  If the application uses more complex handlers, it will be
necessary to add to this value (accounting for nesting) to determine 
the space required for the system stack.

4.4 Location and Extent of C Library Heap

If the optional thread-safe support for the C library is used, memory
for the heap is allocated like this: by default, half the space between
the first available memory address and the end of system memory is
made available to the heap.

The heap location and limit are available in two global variables, and 
can be fully customized in tx_application_define by assigning to them:

    _tx_clib_heap_start     Base address of heap.
    _tx_clib_heap_end       Upper limit address to which heap can grow.

This must be done BEFORE any C library calls. It is advised that it be
done at the beginning of tx_application_define.

Please note that when the thread-safe C library support is used, the heap
is not initialized before tx_kernel_enter has been called, so malloc will 
fail. It is recommended to avoid C library calls that use the heap (such
as printf) outside of ThreadX (eg. in main). The C library is NOT safe 
for use in interrupt or exception handlers, so this should be avoided.

4.5 Thread Stack Sizes

The application must ensure that every thread has enough space for its
stack. This must account for the deepest call depth and allow for one
interrupt stack frame as defined in xtensa_context.h . Several factors
influence the size of the stack required, including compiler optimization
level (-O0 is worst), use of TX_ENABLE_STACK_CHECKING option, and of 
course your Xtensa configuration. Some stack size guidelines and macros 
are provided in tx_port.h assuming no optimization (default, -O0).

Threads that call C library functions may need larger stacks than those
that don't. In particular, use of printf requires a very large stack and
will usually cause a stack overflow if inserted in a thread without 
enlarging its stack size. See DEMO_STACK_SIZE_PRINTF in demo_threadx.c
for a guideline. Use printf with care!


5.  Assembler / Compiler Switches

The following are compiler switches used in building the ThreadX library
and demonstration system. These can be supplied by editing the Makefile
or by overriding the COPT or CFLAGS variables in the make command line
(eg. make COPT="-O2 -DTX_THREAD_SAFE_CLIB"). More details in Makefile.

Compiler Switch                 Meaning

    -g                      Specifies debug information.
    -c                      Specifies object code generation.
    -On                     Sets compiler optimization level n (default -O0).
    -mlongcalls             Allows assembler and linker to convert call 
                            instructions to longer indirect call sequences 
                            when target is out of range.
    -x assembler-with-cpp   Passes .s and .S files through C preprocessor.
    -Dmacro                 Define a preprocessor macro with no value.
    -Dmacro=value           Define a preprocessor macro with a value.

Application Defines (preprocessor macros definable with the -D option):

    TX_THREAD_SAFE_CLIB     Enable support for thread-safe C library.
                            Only the Xtensa C library and the newlib library
                            are supported for thread-safe operation.
                            When this is enabled, half the available memory 
                            space is allocated by default, below the system 
                            stack, for the heap. The heap size and location 
                            can be customized in tx_application_define.
                            Default off.

    NOTE: Thread safe support for Xtensa C library requires Xtensa Tools
          version RF-2015.2 or later.

    TX_ENABLE_STACK_CHECKING Enable generic ThreadX support for stack 
                            overflow checking. This can help avoid long
                            debugging sessions or customer support calls
                            by identifying many crashes caused by stack
                            overflow. Use of this option adds a small
                            premium to the required thread stack sizes
                            (the premium is included by the stack size
                            convenience macros defined in tx_port.h).

    TX_NO_TIMER             Enable generic ThreadX footprint reduction when
                            no periodic tick is needed. The Xtensa port also
                            reduces footprint by removing all code related
                            to timer interrupts. Related XT_* options below
                            (XT_CLOCK_FREQ, XT_TICK_PER_SEC, XT_TIMER_INDEX)
                            have no effect under TX_NO_TIMER.

    All generic ThreadX options in tx_user.h may also be defined with -D.

    Note, the above defines are not specific to Xtensa processors, so 
    their names begin with "TX_". Defines below are unique to the Xtensa
    port so have names beginning with "XT_".

    XT_SIMULATOR            Set this if building to run on the simulator.
                            Takes advantage of certain simulator control
                            and reporting facilities, and adjusts timing 
                            of periodic tick to provide a more acceptable
                            performance in simulation (see XT_CLOCK_FREQ).
                            Set by default unless PLATFORM is overridden.

    XT_BOARD=board          Set this if building for a supported board.
                            The value should be the identifier of a board
                            supported in the Xtensa Tools or an external
                            package (eg. XT_BOARD=xtkc705 for the Xilinx
                            KC705 board). If the board is supported by an
                            external package, the variable XTENSA_BOARDS
                            should point to the root of the package. Set by
                            provided Makefile when PLATFORM=board and BOARD
                            is defined (eg. PLATFORM=board BOARD=xtkc705).

    XT_CLOCK_FREQ=freq      Specifies the target processor's clock 
                            frequency in Hz. Used primarily to set the 
                            timer that generates the periodic interrupt.
                            Defaults are provided and may be edited in
                            xtensa_timer.h (see comments there also).
                            Default for simulator provides more acceptable
                            performance, but cannot provide real-time
                            performance due to variation in simulation
                            speed per host platform and insufficient 
                            cycles between interrupts to process them.
                            Supported board platforms by default leave 
                            this undefined and compute the clock frequency 
                            at initialization unless this is explicitly 
                            defined.

    XT_TICK_PER_SEC=n       Specifies the frequency of the periodic tick.

    XT_TIMER_INDEX=n        Specifies which timer to use for ThreadX.
                            Set this if your Xtensa processor configuration 
                            provides more than one suitable timer and you 
                            want to override the default. See xtensa_timer.h.

    XT_INTEXC_HOOKS         Enables hooks in interrupt vector handlers
                            to support dynamic installation of exception
                            and interrupt handlers. Used by automatic 
                            regression test programs. Disabled by default.
                            
    XT_USE_OVLY             Enable code overlay support.

    XT_USE_SWPRI            Enable software prioritization of interrupts.
                            Enabling this will prioritize interrupts with
                            higher bit numbers over those with lower bit
                            numbers at the same level. This works only for
                            low and medium priority interrupts that can be
                            dispatched to C handlers.

    TX_SYSTEM_STACK_SIZE=n  Specify the size of the interrupt stack, which
                            is the stack that all interrupt handlers switch
                            to while handling interrupts. See the section
                            describing the interrupt stack for more details.


6.  Register Usage and Stack Frames

The Xtensa architecture specifies two ABIs that determine how the general
purpose registers a0-a15 are used: the windowed ABI, and the Call0 ABI.
The choice of ABI is made when configuring the processor or its associated
software. Xtensa processors may have other special registers (including
coprocessor registers and other TIE "states") that are independent of this
choice of ABI. See Xtensa documentation for more details.

In the windowed ABI the registers of the current window are used as follows:
    a0      = return address
    a1      = stack pointer (alias sp)
    a2      = first argument and result of call (in simple cases)
    a3-7    = second through sixth arguments of call (in simple cases).
              Note that complex or large arguments are passed on the
              stack. Details are in the Xtensa Tools manuals.
    a8-a15  = available for use as temporaries.

There are no callee-save registers. The windowed hardware automatically
saves registers a0-a3 on a call4, a0-a8 on a call8, a0-a12 on a call12,
by rotating the register window. Hardware triggers window overflow and
underflow exceptions as necessary when registers outside the current 
window need to be spilled to preallocated space in the stack frame, or
restored. Complete details are in the Xtensa manuals. The entire windowed
register file is saved and restored on interrupt or thread context switch.
Note that only call0 and call8 are available in XEA3 Windowed ABI.

The Call0 ABI does not make use of register windows, relying instead
on a fixed set of 16 registers without window rotation.
The Call0 ABI is more conventional and uses registers as follows:
    a0      = return address
    a1      = stack pointer (alias sp)
    a2      = first argument and result of call (in simple cases)
    a3-7    = second through sixth arguments of call (in simple cases).
              Note that complex or large arguments are passed on the
              stack. Details are in the Xtensa Tools manuals.
    a8-a11  = scratch.
    a12-a15 = callee-save (a function must preserve these for its caller).

On a ThreadX API call, callee-save registers are saved only when a thread
context switch occurs, and other registers are not saved at all (the caller
does not expect them to be preserved). On an interrupt, callee-saved
registers might only be saved and restored when a thread context-switch
occurs, but all other registers are always saved and restored.

An Xtensa processor has other special registers independent of the ABI,
depending on the configuration (including coprocessor registers and other
TIE state) that are part of the thread context. ThreadX preserves all such
registers over an unsolicited context-switch triggered by an interrupt
(including time-slice expiry). However it does NOT preserve these over
a solicited context-switch during a ThreadX API call. This bears some
explanation. These special registers are either ignored by the compiler
or treated as caller-saved, meaning that if kept "live" over a function
call (ie. need to be preserved) they must be saved and restored by the
caller. Since solicited entry to ThreadX is always made by a function
call, ThreadX assumes the caller has saved any of these registers that are
"live". ThreadX avoids a lot of overhead by not having to save and restore
every special register (there can be many) on every solicited context switch.

As a consequence, the application developer should NOT assume that special
registers are preserved over a ThreadX API call such as tx_thread_sleep.
If multiple threads use a register, the caller must save and restore it.

The saved context stack frames for context switches that occur as a result 
of interrupt handling (interrupt frame) or from thread-level API calls 
(solicited frame) are described in human readable form in xtensa_context.h .
All suspended threads have one of these two types of stack frames. The top
of the suspended thread's stack is pointed to by tx_thread_stack_ptr in the 
associated thread control block TX_THREAD. An Xtensa architecture port-specific
extension to the thread control block tx_thread_solicited contains 1 for a
thread that is currently suspended from an API call, otherwise contains 0.


7.  Improving Performance, Footprint, or Ease of Debugging

The distribution version of ThreadX is built with debug (-g) and without
compiler optimizations (-O0). This makes debugging easier inside ThreadX
itself. Of course, -O0 costs some performance. To make ThreadX run faster,
you can change the Makefile to enable the desired optimizations or set
a predefined optimization level (-O<level>) .

Maximum performance is achieved with -O3, but that might increase footprint
substantially. A good compromise is -O2. If code size is a concern, -Os may
yield better results. See the compiler manual for details.

You can eliminate the ThreadX basic API error checking by compiling your
application code with the symbol TX_DISABLE_ERROR_CHECKING defined before
tx_api.h is included. 

The Xtensa architecture port-specific assembly files are coded with no
file-scope labels inside functions (all labels inside functions begin with
".L"). This allows a profiler to accurately associate an address with a
function, and also allows the debugger's stack trace to show the correct
function wherever the program counter is within that function. However
there are some tradeoffs in debugging. Local (".L") labels are not
visible to the debugger, so the following limitations may be observed
during debugging:
- You cannot set a breakpoint on a local label inside a function.
- Disassembly will show the entire function, but will get out of sync and
  show incorrect opcodes if it crosses any padding before an aligned local
  branch target (".L" label, not ".Ln"). Restart disassembly specifying an
  address range explicitly between points where there is padding.
Since ThreadX is provided in source form, it is not difficult to remove
the ".L" and ".Ln" prefixes from local labels if you want them visible.
They can also be made visible by passing the '-L' option to the assembler
and linker (see the assembler and linker manuals for details).


8.  Interrupt and Exception Handling

NOTE: The material in this section is mostly an overview. For a more
detailed explanation please refer to the Xtensa ISA manual and the System
Software manual.

ThreadX provides a complete set of efficient exception and first-level
interrupt handlers installed at the appropriate exception and interrupt
vector locations. The XEA2 architecture supports several different
classes of exceptions and interrupts. Being a configurable architecture,
many of these are optional, and the vector locations are determined by
your processor configuration. The handlers provided use conditional
compilation to adapt to your processor configuration and include only 
the code that is needed.

Xtensa vector locations may reside almost anywhere, including in ROM.
And the amount of code space available at each of these locations is
often very small (e.g. due to following vectors). A small stub of code
installed at the vector jumps to the corresponding handler, usually in RAM.
The exception and interrupt handlers are defined in xtensa_vectors.S .
They are not specific to ThreadX, but call into ThreadX where appropriate
via macros defined in xtensa_rtos.h .

Interrupt/Exception Handling in XEA2
------------------------------------

The handlers provided for low and medium priority interrupts are just
dispatchers that save relevant state and call user-definable handlers.
See the files xtensa_vectors.S and xtensa_api.h for more details of how
to create and install application-specific user interrupt handlers.
Similarly, user-defined handlers can be installed for exceptions (other
than a few which are always handled by the OS).

The high priority interrupt handlers provided may be considered templates
into which the application adds code to service specific interrupts.
The places where application handlers should be inserted are tagged with
the comment "USER_EDIT" in xtensa_vectors.S.

This ThreadX port supports strict priority-based nesting of interrupts.
An interrupt may only nest on top of one of strictly lower priority.
Equal priority interrupts concurrently pending are handled in an
application-defined sequence before any lower priority interrupts
are handled. During interrupt and exception handling, the processor's
interrupt level (PS.INTLEVEL) is used to control the interrupt priority
level that can be accepted; interrupt sources are not controlled
individually by ThreadX (the application is free to access the INTENABLE
register directly to enable/disable individual interrupts, e.g. using
Xtensa HAL services). This approach provides the most deterministic
bounds on interrupt latency (for a given priority) and system stack depth.

Software prioritization of interrupts at the same priority is controlled
by the definition of XT_USE_SWPRI. See above for a description of this
parameter.

Interrupt and Exception Handling in XEA3
----------------------------------------

In XEA3 no distinction is made between low, medium and high priority interrupt
levels. Interrupts at all levels up to the highest (NMI) level can be dispatched
to handlers written in C. Interrupts are dispatched by the XEA3 dispatcher and
the handlers are found from a handler table. Handlers are installed into the
handler table by calling xt_set_interrupt_handler. As there is no concept of
a high priority interrupt any longer, handlers can only be specified per
interrupt, not per level. Since all levels can be handled in C, the value of
EXCM_LEVEL is set to the highest possible level (NMI level).
Software prioritization is not available for XEA3.


The following subsections describe the handling of each class of exception
and interrupt in more detail. Many have nothing to do with ThreadX but are
mentioned because there is code to handle them in xtensa_vectors.S.

8.1 User Exception and Interrupt Handler (Low/Medium Priority)

    All Xtensa 'general exceptions' come to the user, kernel, or double
    exception vector.  The exception type is identified by the EXCCAUSE
    special register (level 1 interrupts are one particular cause of a
    general exception). This port sets up PS to direct all such exceptions
    to the user vector. Exceptions taken at the other two vectors usually
    indicate a kernel or application bug.

    In XEA3, everything except a double exception will come to a common
    vector. This vector saves the requisite processor state and then checks
    for the highest priority interrupt to dispatch. If no interrupts are
    pending then a check is made for exceptions.

    Level 1 interrupts are identified at the beginning of the handler
    and are dispatched to a dedicated handler. Then, syscall and alloca
    exceptions are identified and dispatched to special handlers described
    below. After this, coprocessor exceptions are identified and dispatched
    to the coprocessor handler.

    Any remaining exceptions are processed as follows:

    Having allocated the exception stack frame, the user exception handler
    calls _tx_thread_context_save, which saves the rest of the interrupt
    context. After this the handler sets up a C environment and enables
    the high-priority class of interrupts (which do not interact with
    ThreadX), then reads EXCCAUSE and uses the cause (number) to index 
    into a table of user-specified handlers. The correct handler is then
    called. If the handler returns, the context is restored and control
    is returned to the code that caused the exception. The user-defined
    handler may alter the saved context, or any other system state, that
    allows the faulting instruction to be retried.

    If the cause is a level 1 (low-priority) or medium-priority interrupt,
    the handler enables all interrupts above that priority level after
    saving the thread context and switching to the interrupt stack if it
    is not a nested interrupt. It then sets up the environment for C code
    and then calls the handler (found in the handler table) for the 
    interrupt number. If the user has not specified a handler, then the
    default handler will be called, which will terminate the program.

    If the interrupt is for the system timer, it calls a special interrupt
    handler for the system timer tick, which calls _tx_timer_interrupt then
    clears its bit from the mask. This interrupt cannot be hooked by the
    user-defined handler.

    Finally, the handler calls _tx_thread_context_restore to allow ThreadX
    to perform any scheduling necessary and return either to the interrupted
    thread or another.

    If software prioritization is enabled, the handler will re-enable all
    interrupts at the same level that are numerically higher than the current
    one, before calling the user handler. This allows a higher priority
    interrupt to pre-empt the lower priority handler.

8.2 Medium Priority Interrupt Handlers (XEA2)

    Medium priority interrupts are those at levels 2 up to XCHAL_EXCM_LEVEL, 
    a configuration-specific maximum interrupt level affected by the global 
    'exception mode' bit in the processor status word (PS.EXCM).
    Interrupt levels above XCHAL_EXCM_LEVEL are of the high-priority class.
    The Xtensa hardware documentation considers medium priority interrupts
    to be a special case of high-priority interrupts, but from a software
    perspective they are very different.

    Dispatch of medium-priority interrupts is discussed in the section
    above.

8.3 High Priority Interrupt Handlers (XEA2)

    High priority interrupts are those strictly above XCHAL_EXCM_LEVEL, 
    a configuration-specific maximum interrupt level affected by the 
    global 'exception mode' bit in the processor status word (PS.EXCM).
    High priority handlers may not directly interact with ThreadX at all,
    and are described here only for the sake of completeness. They must
    be coded in assembler (may not be coded in C) and are intended to be 
    used for handling extremely high frequency hardware events that need
    to be handled in only a few cycles. A high priority interrupt handler
    may trigger a software interrupt at a medium or low priority level to
    occasionally signal ThreadX. Please see Xtensa documentation.

    There is a separate vector and a few special registers for each high
    priority interrupt, providing for fast dispatch and efficient nesting
    on top of lower priority interrupts. Handlers are templates included
    only for the vectors that exist in your Xtensa processor configuration.
    These templates are written for only one interrupt per high priority 
    level to minimize latency servicing very fast time-critical interrupts.
    The vector code jumps to the corresponding first-level interrupt handler,
    which then executes application-provided assembler code before returning
    quickly to the interrupted thread or lower priority handler.

8.4 Kernel Exception Handler (XEA2)

    Kernel mode is not used in this port of ThreadX, and therefore kernel
    exceptions should not happen. A stub is provided for the vector that
    triggers the debugger (if connected) or calls _xt_panic to freeze the 
    processor should a kernel exception occur.

8.5 Alloca Exception Handler

    Alloca exceptions are generated by the 'movsp' instruction, which
    is used only in the windowed ABI. Its purpose is to allocate some
    space on top of the stack. Because the window hardware may have
    spilled some registers to the 16 byte "base save" area below the
    stack pointer, it is necessary to protect those values. The alloca
    handler accomplishes this quickly without setting up an interrupt
    frame or entering ThreadX, by emulating a register underflow and
    re-executing 'movsp'.

8.6 Syscall Exception Handler

    Syscall exceptions are generated by a 'syscall' instruction.
    The windowed ABI specifies that executing this instruction with
    a value of zero in register a2 must spill any unsaved registers
    in the windowed register file to their pre-determined locations
    on the caller's stack. The handler does exactly that, and skips
    over the 'syscall' instruction before returning to the caller.
    If a2 is non-zero, the handler returns a2 == -1 to the caller.

8.7 Co-Processor Exception Handler

    A coprocessor exception is generated when a thread accesses a
    coprocessor that it does not "own". Ownership represents which 
    thread's state is currently in the coprocessor. Co-processors are 
    context-switched "lazily" (on demand) only when a non-owning thread 
    uses a coprocessor instruction, otherwise a thread retains ownership 
    even when it is preempted from the main processor. The coprocessor
    exception handler performs the context-switch and manages ownership.

    Co-processors may not be used by any code outside the context of a
    thread. A coprocessor exception triggered by code that is not part
    of a running thread is a fatal error and ThreadX/Xtensa will panic.
    This restriction is intended to reduce the overhead of saving and 
    restoring coprocessor state (which can be quite large) and in 
    particular remove that overhead from interrupt handlers. It also
    reduces the thread stack size requirement by allowing coprocessor
    state to be saved in the thread control block rather than the stack.

8.8 Debug Exception Handler

    A debug exception is caused as a result of running code, such as by
    a 'break' instruction or hardware breakpoints and watchpoints, or
    as a result of an external debug interrupt, such as from an OCD based
    debugger or multiprocessor debug events ("breakin/breakout"). If the
    processor is running in OCD mode under control of an OCD-based debugger,
    the trigger event immediately halts the processor and gives control to
    the OCD debugger. Otherwise control is transferred to the debug vector.
    The debug vector handler calls the simulator if running on the ISS,
    which then takes control and interacts with any attached debugger.
    If running on hardware and not in OCD mode, debug exceptions are not
    expected, so the debug handler calls _xt_panic to freeze the processor.

8.9 Double Exception Handler

    A double exception is a general exception that happens while the
    processor is in exception mode (PS.EXCM set), and thus indicates a
    bug in kernel code. The double exception vector handler triggers
    the debugger (if connected) or calls _xt_panic to freeze the 
    processor.

8.10 Window Overflow and Underflow Exception Handlers

    Window overflow and underflow handlers are required for use of the
    windowed ABI. Each has its own dedicated vector and highly optimized
    code that is independent of OS. See Xtensa documentation for details.

8.11 Hooks for Dynamic Installation of Handlers

    Optional hooks are provided in the user exception and low level
    interrupt handler and all medium and high priority interrupt handlers,
    to dynamically install a handler function (which may be coded in C,
    unless in a high-priority interrupt handler). These hooks are enabled
    and used by automatic regression tests, they are not part of a normal
    ThreadX build. However an application is free to take advantage of
    them. The interrupt/exception hooks are described in xtensa_rtos.h .

    It is recommended that the application not make use of these hooks, but
    rather use xt_set_interrupt_handler() and xt_set_exception_handler()
    to install application-specific handlers. This method is more convenient
    and allows arguments to be passed to the handlers. Software prioritization
    of interrupts works only with this method. See xtensa_api.h for details.

9.  Overlay Support (XEA2 only)

ThreadX supports the overlay feature of the Xtensa toolsuite. To enable overlay support,
the ThreadX library should be built with XT_USE_OVLY defined. In addition, the linker 
command line must use the overlay library via the -loverlay linker option and the 
xtensa_overlay_os_hook.o object file must be explicitly specified in order to override
the overlay libary version.

You will also need to generate a custom LSP for overlay use. Please reference the 
Xtensa System Software Reference and Xtenas Linker Support Packages (LSPs) for more 
information on using overlays.


10.  Revision History

For generic code revision information, please refer to the readme_threadx_generic.txt
file, which is included in your distribution. The following details the revision
information associated with this specific port of ThreadX:


04-02-2021  Release 6.1.6 changes:
            tx_port.h                           Updated macro definition

12-31-2020  Initial Version 6.1.3


Copyright(c) 1996-2020 Microsoft Corporation


https://azure.com/rtos

