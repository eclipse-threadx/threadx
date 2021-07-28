    .text
    .align 4
    .section Init
    
    // External references
    .global  _txm_module_thread_shell_entry
    .global  _txm_module_callback_request_thread_entry
    .global  demo_module_start
    .global  Image$$ER_RO$$Length
    .global  Image$$ER_RW$$Length
    .global  Image$$ER_ZI$$Length

    .global __txm_module_preamble
__txm_module_preamble:
    .dc.l   0x4D4F4455                                          // Module ID
    .dc.l   0x6                                                 // Module Major Version
    .dc.l   0x1                                                 // Module Minor Version
    .dc.l   32                                                  // Module Preamble Size in 32-bit words
    .dc.l   0x12345678                                          // Module ID (application defined)
    .dc.l   0x01000000                                          // Module Properties where:
                                                                //   Bits 31-24: Compiler ID
                                                                //           0 -> IAR
                                                                //           1 -> ARM
                                                                //           2 -> GNU
                                                                //   Bit 0:  0 -> Privileged mode execution
                                                                //           1 -> User mode execution
                                                                //   Bit 1:  0 -> No MPU protection
                                                                //           1 -> MPU protection (must have user mode selected)
                                                                //   Bit 2:  0 -> Disable shared/external memory access
                                                                //           1 -> Enable shared/external memory access
    .dc.l   _txm_module_thread_shell_entry                      // Module Shell Entry Point
    .dc.l   demo_module_start                                   // Module Start Thread Entry Point
    .dc.l   0                                                   // Module Stop Thread Entry Point
    .dc.l   1                                                   // Module Start/Stop Thread Priority
    .dc.l   4096                                                // Module Start/Stop Thread Stack Size
    .dc.l   _txm_module_callback_request_thread_entry           // Module Callback Thread Entry
    .dc.l   1                                                   // Module Callback Thread Priority
    .dc.l   4096                                                // Module Callback Thread Stack Size
    .dc.l   Image$$ER_RO$$Length                                // Module Code Size
    .dc.l   Image$$ER_ZI$$Length                                // Module Data Size
    .dc.l   0                                                   // Reserved 0
    .dc.l   0                                                   // Reserved 1
    .dc.l   0                                                   // Reserved 2
    .dc.l   0                                                   // Reserved 3
    .dc.l   0                                                   // Reserved 4
    .dc.l   0                                                   // Reserved 5
    .dc.l   0                                                   // Reserved 6
    .dc.l   0                                                   // Reserved 7
    .dc.l   0                                                   // Reserved 8
    .dc.l   0                                                   // Reserved 9
    .dc.l   0                                                   // Reserved 10
    .dc.l   0                                                   // Reserved 11
    .dc.l   0                                                   // Reserved 12
    .dc.l   0                                                   // Reserved 13
    .dc.l   0                                                   // Reserved 14
    .dc.l   0                                                   // Reserved 15

.image__er_ro__length:
    .quad Image$$ER_RO$$Length
.image__er_rw__length:
    .quad Image$$ER_RW$$Length
.image__er_zi__length:
    .quad Image$$ER_ZI$$Length
