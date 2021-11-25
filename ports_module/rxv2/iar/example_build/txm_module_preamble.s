    /* Alignment of 4 (16-byte) */
    SECTION .text:CODE (4)
    
    
    /* Define public symbols.  */

    PUBLIC __txm_module_preamble


    /* Define application-specific start/stop entry points for the module.  */

    EXTERN _demo_module_start


    /* Define common external refrences.  */

        EXTERN  __txm_module_thread_shell_entry
        EXTERN  __txm_module_callback_request_thread_entry
        EXTERN  ROPI$$Length
        EXTERN  RWPI$$Length

        DATA
__txm_module_preamble:
        DC32      0x4D4F4455                                        // Module ID
        DC32      0x5                                               // Module Major Version
        DC32      0x6                                               // Module Minor Version
        DC32      32                                                // Module Preamble Size in 32-bit words
        DC32      0x12345678                                        // Module ID (application defined) 
        DC32      0x00000007                                        // Module Properties where:
                                                                    //   Bits 31-24: Compiler ID
                                                                    //           0 -> IAR
                                                                    //           1 -> RVDS
                                                                    //           2 -> GNU
                                                                    //   Bit 0:  0 -> Privileged mode execution
                                                                    //           1 -> User mode execution
                                                                    //   Bit 1:  0 -> No MPU protection
                                                                    //           1 -> MPU protection (must have user mode selected)
                                                                    //   Bit 2:  0 -> Disable shared/external memory access
                                                                    //           1 -> Enable shared/external memory access
        DC32      __txm_module_thread_shell_entry - $               // Module Shell Entry Point
        DC32      _demo_module_start - $                            // Module Start Thread Entry Point
        DC32      0                                                 // Module Stop Thread Entry Point 
        DC32      1                                                 // Module Start/Stop Thread Priority
        DC32      1024                                              // Module Start/Stop Thread Stack Size
        DC32      __txm_module_callback_request_thread_entry - $    // Module Callback Thread Entry
        DC32      1                                                 // Module Callback Thread Priority     
        DC32      1024                                              // Module Callback Thread Stack Size    
        DC32      ROPI$$Length                                      // Module Code Size
        DC32      RWPI$$Length                                      // Module Data Size
        DC32      0                                                 // Reserved 0   
        DC32      0                                                 // Reserved 1
        DC32      0                                                 // Reserved 2
        DC32      0                                                 // Reserved 3
        DC32      0                                                 // Reserved 4
        DC32      0                                                 // Reserved 5     
        DC32      0                                                 // Reserved 6     
        DC32      0                                                 // Reserved 7   
        DC32      0                                                 // Reserved 8  
        DC32      0                                                 // Reserved 9
        DC32      0                                                 // Reserved 10
        DC32      0                                                 // Reserved 11
        DC32      0                                                 // Reserved 12
        DC32      0                                                 // Reserved 13
        DC32      0                                                 // Reserved 14
        DC32      0                                                 // Reserved 15

        END


