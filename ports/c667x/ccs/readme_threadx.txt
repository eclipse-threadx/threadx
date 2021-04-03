                  Microsoft's Azure RTOS ThreadX for TMS320C667x 

                       Using the TI Code Composer Tools

1.  Installation

TI Code Composer Studio and the TI MCSDK must be installed prior to
building ThreadX. The following links can be used to download these
packages:


http://processors.wiki.ti.com/index.php/Download_CCS
http://software-dl.ti.com/sdoemb/sdoemb_public_sw/bios_mcsdk/latest/index_FDS.html

It is assumed the tools are installed in the default directories:

CCS path by default - c:\ti\ccsv(version number)
MCSDK path by default - c:\ti

If the packages are installed in different directories, the ThreadX project 
settings must be adjusted.

2. Open the Azure RTOS Workspace 

In order to build the ThreadX library and the ThreadX demonstration first open 
the Azure RTOS Workspace inside your ThreadX installation directory. 


3. Building the ThreadX run-time Library

Building the ThreadX library is easy; simply import the CCS project file 
"tx" and then select the build button. You should now observe the compilation 
and assembly of the ThreadX library. This project build produces the ThreadX 
library file tx.lib.


4.  Demonstration System

The ThreadX demonstration is designed to execute on the C6678EVM evaluation board. 

Building the demonstration is easy; simply import the "sample_threadx_c6678evm" project.
Now select "Project -> Build Active Project" to build the ThreadX demonstration, 
which produces the sample_threadx.out file in the "Debug" directory. You are now 
ready to run the ThreadX demonstration on the C6678EVM evaluation board.

Please refer to Chapter 6 of the ThreadX User Guide for a complete description 
of this demonstration. 


5.  System Initialization

The entry point in ThreadX for the TMS320C667x using the TI tools is at label 
_c_int00. This is defined within the TI library. In addition, this is 
where all static and global pre-set C variable initialization processing 
takes place.

The ThreadX initialization file tx_initialize_low_level.asm is responsible 
for setting up various system data structures, the vector area, and a periodic 
timer interrupt source. By default, the vector area is defined to be located in 
the "vectors" section, which is defined at the top of tx_initialize_low_level.asm. 
This area is located at address 0 for the demonstration. 

tx_initialize_low_level.asm is also where initialization of a periodic timer 
interrupt source should take place.

In addition, _tx_initialize_low_level determines the first available address 
for use by the application. By default, free memory is assumed to start after 
the .zend section in RAM (defined in tx_initialize_low_level). This section
must be placed at the end of your other RAM sections. Please see sample_threadx.cmd
for an example. The address of this section is passed to the application definition 
function, tx_application_define.


6.  Register Usage and Stack Frames

The TI TMS320C667x compiler assumes that registers A0-A9, A16-A31, B0-B9, and 
B16-B31 are scratch registers for each function. All other registers used by 
a C function must be preserved by the function. ThreadX takes advantage of this 
in situations where a context switch happens as a result of making a ThreadX 
service call (which is itself a C function). In such cases, the saved context 
of a thread is only the non-scratch registers.

The following defines the saved context stack frames for context switches
that occur as a result of interrupt handling or from thread-level API calls.
All suspended threads have one of these two types of stack frames. The top
of the suspended thread's stack is pointed to by tx_thread_stack_ptr in the 
associated thread control block TX_THREAD.



    Offset        Interrupted Stack Frame        Non-Interrupt Stack Frame

     0x04                   1                           0
     0x08                   CSR                         CSR
     0x0C                   IPR                         B3
     0x10                   AMR                         AMR
     0x14                   A0                          A10
     0x18                   A1                          A11
     0x1C                   A2                          A12
     0x20                   A3                          A13
     0x24                   A4                          A14
     0x28                   A5                          A15
     0x2C                   A6                          B10
     0x30                   A7                          B11    
     0x34                   A8                          B12    
     0x38                   A9                          B13    
     0x3C                   A10                         ILC  
     0x40                   A11                         RILC
     0x44                   A12
     0x48                   A13
     0x4C                   A14
     0x50                   A15
     0x54                   B0
     0x58                   B1
     0x5C                   B2
     0x60                   B3
     0x64                   B4
     0x68                   B5
     0x6C                   B6
     0x70                   B7
     0x74                   B8
     0x78                   B9
     0x7C                   B10
     0x80                   B11
     0x84                   B12
     0x88                   B13
     0x8C                   A16
     0x90                   A17
     0x94                   A18
     0x98                   A19
     0x9C                   A20
     0xA0                   A21
     0xA4                   A22
     0xA8                   A23
     0xAC                   A24
     0xB0                   A25
     0xB4                   A26
     0xB8                   A27
     0xBC                   A28
     0xC0                   A29
     0xC4                   A30
     0xC8                   A31
     0xCC                   B16
     0xD0                   B17
     0xD4                   B18
     0xD8                   B19
     0xDC                   B20
     0xE0                   B21
     0xE4                   B22
     0xE8                   B23
     0xEC                   B24
     0xF0                   B25
     0xF4                   B26
     0xF8                   B27
     0xFC                   B28
     0x100                  B29
     0x104                  B30
     0x108                  B31
     0x10C                  ILC
     0x110                  RILC
     0x114                  ITSR
    

7.  Improving Performance

The distribution version of ThreadX is built without any compiler 
optimizations. This makes it easy to debug because you can trace or set 
breakpoints inside of ThreadX itself. Of course, this costs some performance. 
To make it run faster, you can replace the -g compiler option
to a -O3 in the ThreadX project file to enable all compiler optimizations. 

In addition, you can eliminate the ThreadX basic API error checking by 
compiling your application code with the symbol TX_DISABLE_ERROR_CHECKING 
defined. 


8.  Interrupt Handling

ThreadX provides complete and high-performance interrupt handling for 
TMS320C667x targets. There are a certain set of requirements that are 
defined in the following sub-sections:


8.1  Vector Area

The TMS320C667x interrupt vectors at in the section "vectors" and is defined at 
the top of tx_initialize_low_level.asm. Each interrupt vector entry contains 
a jump to a template interrupt processing shell. 


8.2  Interrupt Service Routine Shells

The following interrupt processing shells are defined at the bottom of 
tx_initialize_low_level.asm:


    __tx_int4_ISR
    __tx_int5_ISR
    __tx_int6_ISR
    __tx_int7_ISR
    __tx_int8_ISR
    __tx_int9_ISR
    __tx_int10_ISR
    __tx_int11_ISR
    __tx_int12_ISR
    __tx_int13_ISR
    __tx_int14_ISR
    __tx_int15_ISR

Each interrupt ISR is entered with B3, A0-A4 is available (these registers are 
saved in the initial vector processing). The default interrupt handling 
includes calls to __tx_thread_context_save and __tx_thread_context_restore.
Application ISR processing can be added between the context save/restore 
calls. Note that only the compiler scratch registers are available for use
after context save return to the ISR.

High-frequency interrupt handlers might not want to perform context 
save/restore processing on each interrupt. If this is the case, any 
additional registers used must be saved and restored by the ISR and
the interrupt return processing must restore the registers saved by the
initial vector processing. This can be accomplished by adding the 
following code to the end of the custom ISR handling:

        LDW         *+SP(20),A0                         ; Recover A0
        LDW         *+SP(24),A1                         ; Recover A1
        LDW         *+SP(28),A2                         ; Recover A2
        LDW         *+SP(32),A3                         ; Recover A3
        B           IRP                                 ; Return to point of interrupt
||      LDW         *+SP(36),A4                         ; Recover A4
        LDW         *+SP(96),B3                         ; Recover B3
        ADDK.S2     288,SP                              ; Recover stack space
        NOP         3                                   ; Delay slots


9.  Revision History

For generic code revision information, please refer to the readme_threadx_generic.txt
file, which is included in your distribution. The following details the revision
information associated with this specific port of ThreadX:

04-02-2021  Release 6.1.6 changes:
            tx_port.h                           Updated macro definition

09-30-2020  Initial ThreadX 6.1 version for TMS320C667x using TI Code Composer tools.


Copyright(c) 1996-2020 Microsoft Corporation


https://azure.com/rtos

