    Microsoft's Azure RTOS ThreadX for ThreadX SMP for MIPS32 interAptiv/VPE

                      Using the Green Hills Software Tools

1. Installation

ThreadX for the MIPS32 interAptiv is delivered on a single CD-ROM compatible disk. 
The entire distribution can be found in the sub-directory:

\threadx

To install ThreadX to your hard-disk, either run the supplied installer 
program Setup.exe or copy the distribution from the CD manually. 

To copy the ThreadX distribution manually, make a threadx directory on your 
hard-disk (we recommend C:\threadx\mips32_interaptiv\green) and copy all the contents 
of the threadx sub-directory on the distribution disk. The following 
is an example MS-DOS copy command from the distribution directory
(assuming source is d: and c: is your hard-drive):


d:\threadx> xcopy /S *.* c:\threadx\mips32_interaptiv\green


2. Open the ThreadX Project Workspace

In order to build the ThreadX library and the ThreadX demonstration first
load the ThreadX project workspace threadx_project_workspace.gpj, which is 
located inside your ThreadX directory:

C:\threadx\mips32_interaptiv\green\threadx_project_workspace.gpj


3. Building the ThreadX run-time Library

Building the ThreadX library is easy; simply select the MULTI project file 
tx.gpj and then select the build button. You should now observe the 
compilation and assembly of the ThreadX library. This project build produces 
the ThreadX library file tx.a.


4. Demonstration System

The ThreadX demonstration is designed to execute under the MULTI environment
on the MIPS interAptiv MALTA board. By default, the demonstration is setup for a 
3 core (6 VPE) interAptiv configuration. The instructions that follow describe
how to get the ThreadX evaluation running under the MIPS interAptiv MALTA board.

Building the demonstration is easy; simply select the MULTI project file 
demo_threadx.gpj. At this point, select the "Project Build" button and observe 
the compilation, assembly, and linkage of the ThreadX demonstration application. 

After the demonstration is built, follow these steps to download and execute the
ThreadX SMP interAptiv demonstration:

A. Select the "demo_threadx_ram_interAptiv_3c2v4t.ghsmc" multi-core configuration 
   file and then the debug button (or [F5] key). You should observe a debugger 
   window with 6 unconnected demo_threadx.elf executables.
   
B. Select the first of the 6 unconnected demo_threadx.elf images and click the 
   "prepare target" button (or right-click -> prepare target). You should now
   get a "Connection Chooser" dialog box.
   
C. Connect to the "GHS_Probe_interAptive_3c2v4t" target. You should observe the
   "prepare target" dialog. 
   
D. In the "prepare target" dialog, select "download". You should now see a 
   connection to 12 threads, as follows:
   
    Id 0 -> c0v0t0  boot_mips.elf @ _start ) (code and data are loaded on Id 0)
    Id 1 -> c0v1t1  boot_mips.elf @ 0xdeadbeef (scripted indication of uninitialized vpe1)
    Id 2 -> c0v1t2  (not used)
    Id 3 -> c0v1t3  (not used)
    Id 4 -> c1v0t0  boot_mips.elf @ _start (scripted init triggered by load on Id 0)
    Id 5 -> c1v1t1  boot_mips.elf @ 0xdeadbeef (scripted indication of uninitialized vpe1)
    Id 6 -> c1v1t2  (not used)
    Id 7 -> c1v1t3  (not used)
    Id 8 -> c2v0t0  boot_mips.elf @ _start (scripted init triggered by load on Id 0)
    Id 9 -> c2v1t1  boot_mips.elf @ 0xdeadbeef (scripted indication of uninitialized vpe1)
    Id 10 -> c2v1t2  (not used)
    Id 11 -> c2v1t3  (not used)

E. To start execution, select and run Id 0, Id 4, and Id 8. All the cores are now 
   running and you should observe messages being displayed on the MALTA board.


5. EventAnalyzer Demonstration

To build a demonstration system that also logs events for the MULTI EventAnalyzer, 
perform the same steps as the regular demo, except build the ThreadX library with 
txe.gpj file and use the demo_threadx_el.gpj build file to build the demonstration. 
The resulting image will log all system events, which can then be displayed by the 
MULTI EventAnalyzer.


6. System Initialization

The system entry point using the Green Hills tools is at the label _start. 
This is defined within the start.mip file supplied by MIPS. In addition, 
this is where all static and global preset C variable initialization 
processing is called from.

Once the startup function finishes, main is called, which is also where ThreadX 
initialization takes place. The main initialization function for ThreadX is 
_tx_initialize_low_level and is located in the file tx_initialize_low_level.mip. 
This function is responsible for setting up various system data structures, 
interrupt vectors, and the periodic timer interrupt source of ThreadX.

In addition, _tx_initialize_low_level determines where the first available 
RAM memory address is located. This address is supplied to tx_application_define.

By default, the first available RAM memory address is assumed to start at the 
beginning of the ThreadX section .free_mem. If changes are made to the 
demo_threadx.ld file, the .free_mem section should remain the last allocated 
section in the main RAM area. The starting address of this section is passed 
to tx_application_define.


7. User defines

Please reference the ThreadX_SMP_User_Guide.pdf for details on build options.


8. Register Usage and Stack Frames

The Green Hills MIPS compiler assumes that registers t0-t9 ($8-$15, $24, $25) 
are scratch registers for each function. All other registers used by a 
C function must be preserved by the function. ThreadX takes advantage 
of this in situations where a context switch happens as a result of making a 
ThreadX service call (which is itself a C function). In such cases, the 
saved context of a thread is only the non-scratch registers.

The following defines the saved context stack frames for context switches
that occur as a result of interrupt handling or from thread-level API calls.
All suspended threads have one of these two types of stack frames. The top
of the suspended thread's stack is pointed to by tx_thread_stack_ptr in the 
associated thread control block TX_THREAD.



    Offset        Interrupted Stack Frame        Non-Interrupt Stack Frame

     0x000                  1                           0
     0x004                  s8  ($30)                   s8  ($30)
     0x008                  s7  ($23)                   s7  ($23)
     0x00C                  s6  ($22)                   s6  ($22)
     0x010                  s5  ($21)                   s5  ($21)
     0x014                  s4  ($20)                   s4  ($20)
     0x018                  s3  ($19)                   s3  ($19)
     0x01C                  s2  ($18)                   s2  ($18)
     0x020                  s1  ($17)                   s1  ($17)
     0x024                  s0  ($16)                   s0  ($16)
     0x028                  hi                          hi
     0x02C                  lo                          lo
     0x030                  t9  ($25)                   ra  ($31)
     0x034                  t8  ($24)                   SR
     0x038                  t7  ($15)                   f31    <------------+
     0x03C                  t6  ($14)                                       |
     0x040                  t5  ($13)                   f30                 |
     0x044                  t4  ($12)                                       |
     0x048                  t3  ($11)                   f29                 |
     0x04C                  t2  ($10)                                       |
     0x050                  t1  ($9)                    f28                 |
     0x054                  t0  ($8)                                        |
     0x058                  a3  ($7)                    f27                 |
     0x05C                  a2  ($6)                                        |
     0x060                  a1  ($5)                    f26                 |
     0x064                  a0  ($4)                                        
     0x068                  v1  ($3)                    f25    TX_ENABLE_64BIT_FPU_SUPPORT
     0x06C                  v0  ($2)
     0x070                  at  ($1)                    f24                 |
     0x074                  ra  ($31)                                       |
     0x078                  SR                          f23                 |
     0x07C                  EPC                                             |
     0x080                  f31    <-----------+        f22                 |
     0x088                  f30                |        f21                 |
     0x090                  f29                |        f20                 |
     0x098                  f28                |        fcr31  <------------+
     0x09C                                     |        not used
     0x0A0                  f27                |        
     0x0A4                                     |        
     0x0A8                  f26                |        
     0x0AC                                     |        
     0x0B0                  f25                |        
     0x0B4                                     |        
     0x0B8                  f24                |        
     0x0BC                                     |        
     0x0C0                  f23                |
     0x0C8                  f22                |
     0x0D0                  f21                |
     0x0D8                  f20                |
     0x0E0                  f19                |
     0x0E8                  f18                |
     0x0F0                  f17
     0x0F8                  f16    TX_ENABLE_64BIT_FPU_SUPPORT
     0x100                  f15
     0x108                  f14                |
     0x110                  f13                |
     0x118                  f12                |
     0x120                  f11                |
     0x128                  f10                |
     0x130                  f9                 |
     0x138                  f8                 |
     0x140                  f7                 |
     0x148                  f6                 |
     0x150                  f5                 |
     0x158                  f4                 |
     0x160                  f3                 |
     0x168                  f2                 |
     0x170                  f1                 |
     0x178                  f0                 |
     0x180                  fcr31  <-----------+
     0x184                  not used


9. Improving Performance

The distribution version of ThreadX is built without any compiler 
optimizations. This makes it easy to debug because you can trace or set 
breakpoints inside of ThreadX itself. Of course, this costs some 
performance. To make ThreadX run faster, you can change the tx.gpj project
to disable debug information and enable the desired optimizations. 

In addition, you can eliminate the ThreadX basic API error checking by 
compiling your application code with the symbol TX_DISABLE_ERROR_CHECKING 
defined before tx_api.h is included. 


10. Interrupt Handling

ThreadX provides complete and high-performance interrupt handling for  MIPS32 interAptiv
targets. The general exception handler is at address: 0x80000180 (0xA0000180 non-
cached). The ThreadX general exception handler is defined in the file 
tx_initialize_low_level.mip at the label _tx_exception_handler. A small piece of 
code to jump to this exception handler is copied to the general exception handler 
address during initialization.

10.1  Application ISRs

Multiple exceptions may be processed with a single execution of the exception 
handler. This is because the Cause register could indicate more than a single
exception. Processing for each exception is also located in the general 
exception handler that starts at the label: _tx_exception_handler. Application 
ISRs can be added into this handler.


11. Theory of Operation - SMP

ThreadX for the MIPS interAptiv brings Symmetric Multi-Processing (SMP) technology to 
the MIPS interAptiv. ThreadX application threads (of varying priority) that are "READY" 
to run are dynamically allocated to VPEs during scheduling, thus taking full 
advantage of all available MIPS interAptiv VPEs. This results in true SMP processing, 
including automatic load balancing of application thread execution across all 
available MIPS interAptiv VPEs. 

Initialization is done exclusively in VPE 0, which is the default running VPE
after reset. The additional VPEs on the interAptiv are initialized by VPE 0 and simply 
wait until VPE 0 completes the initialization before they start running.

During thread execution, multithreading in the MIPS interAptiv is fully enabled. This 
means that application threads may be preempted by higher priority threads, may 
suspend themselves, or may exit the system upon completion of their work. Protection
between VPEs is accomplished via a conditional load-store structure (see the variable
_tx_thread_smp_protection and the typedef TX_THREAD_SMP_PROTECT found in tx_thread.h).

All VPEs are eligible to handle interrupts under the direction of the application. The
ThreadX timer interrupt is by default assigned to VPE 0 for processing. Please see
the code in tx_timer_interrupt.mip for the implementation.

ThreadX for the MIPS interAptiv also optionally supports the MIPS interAptiv FPU. 

The number of VPEs is defined by the compile time constant TX_THREAD_SMP_MAX_CORES. 
By default, this is set to 2 in tx_port.h. It may be changed to support any number 
of cores either in tx_port.h or on the command line via a -D symbol definition. 


12. Current Limitations

1. Hardware priority assignment for each TC is not setup.
2. DSP registers are not saved/restored.


13. Debug Information

ThreadX SMP for MIPS32 interAptiv has a built-in debug facility to capture SMP scheduling 
information. This is enabled by building the system with TX_THREAD_SMP_DEBUG_ENABLE
defined. This results in the creation of circular log containing debug information.
The log is defined in the variable _tx_thread_smp_debug_info_array.


14. Revision History

For generic code revision information, please refer to the readme_threadx_generic.txt
file, which is included in your distribution. The following details the revision
information associated with this specific port of ThreadX:


03-08-2023  Initial ThreadX version 6.2.1 of MIPS32_interAptiv VPE/Green Hills port. 


Copyright(c) 1996-2020 Microsoft Corporation

https://azure.com/rtos
