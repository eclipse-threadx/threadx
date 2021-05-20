/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** ThreadX Component                                                     */ 
/**                                                                       */
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* #define TX_SOURCE_CODE  */


/* Include necessary system files.  */

/*  #include "tx_api.h"
    #include "tx_thread.h"  */

    SECTION `.text`:CODE:REORDER:NOROOT(2)
    CODE
/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_thread_stack_build                             RISC-V32/IAR     */
/*                                                           6.1          */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Microsoft Corporation                             */ 
/*    Tom van Leeuwen, Technolution B.V.                                  */
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function builds a stack frame on the supplied thread's stack.  */
/*    The stack frame results in a fake interrupt return to the supplied  */
/*    function pointer.                                                   */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    thread_ptr                            Pointer to thread control blk */
/*    function_ptr                          Pointer to return function    */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    None                                                                */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    _tx_thread_create                     Create thread service         */
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  09-30-2020      William E. Lamie        Initial Version 6.1           */ 
/*                                                                        */ 
/**************************************************************************/ 
/* VOID   _tx_thread_stack_build(TX_THREAD *thread_ptr, VOID (*function_ptr)(VOID))
{  */
    PUBLIC  _tx_thread_stack_build
_tx_thread_stack_build:

       
    /* Build a fake interrupt frame.  The form of the fake interrupt stack
       on the RISC-V RV32 should look like the following after it is built:
       
       Stack Top:      1       (00)    Interrupt stack frame type
                       x27     (04)    Initial s11
                       x26     (08)    Initial s10
                       x25     (12)    Initial s9
                       x24     (16)    Initial s8
                       x23     (20)    Initial s7
                       x22     (24)    Initial s6
                       x21     (28)    Initial s5
                       x20     (32)    Initial s4
                       x19     (36)    Initial s3
                       x18     (40)    Initial s2
                       x9      (44)    Initial s1
                       x8      (48)    Initial s0
                       x31     (52)    Initial t6
                       x30     (56)    Initial t5
                       x29     (60)    Initial t4
                       x28     (64)    Initial t3
                       x7      (68)    Initial t2
                       x6      (72)    Initial t1
                       x5      (76)    Initial t0
                       x17     (80)    Initial a7
                       x16     (84)    Initial a6
                       x15     (88)    Initial a5
                       x14     (92)    Initial a4
                       x13     (96)    Initial a3
                       x12     (100)   Initial a2
                       x11     (104)   Initial a1
                       x10     (108)   Initial a0
                       x1      (112)   Initial ra
                       mepc    (120)   Initial mepc
If floating point support:
                       f0      (124)   Inital ft0
                       f1      (128)   Inital ft1
                       f2      (132)   Inital ft2
                       f3      (136)   Inital ft3
                       f4      (140)   Inital ft4
                       f5      (144)   Inital ft5
                       f6      (148)   Inital ft6
                       f7      (152)   Inital ft7
                       f8      (156)   Inital fs0
                       f9      (160)   Inital fs1
                       f10     (164)   Inital fa0
                       f11     (168)   Inital fa1
                       f12     (172)   Inital fa2
                       f13     (176)   Inital fa3
                       f14     (180)   Inital fa4
                       f15     (184)   Inital fa5
                       f16     (188)   Inital fa6
                       f17     (192)   Inital fa7
                       f18     (196)   Inital fs2
                       f19     (200)   Inital fs3
                       f20     (204)   Inital fs4
                       f21     (208)   Inital fs5
                       f22     (212)   Inital fs6
                       f23     (216)   Inital fs7
                       f24     (220)   Inital fs8
                       f25     (224)   Inital fs9
                       f26     (228)   Inital fs10
                       f27     (232)   Inital fs11
                       f28     (236)   Inital ft8
                       f29     (240)   Inital ft9
                       f30     (244)   Inital ft10
                       f31     (248)   Inital ft11
                       fscr    (252)   Inital fscr

    Stack Bottom: (higher memory address)  */

    lw      t0, 16(a0)                                  ; Pickup end of stack area
    li      t1, ~15                                     ; Build 16-byte alignment mask
    and     t0, t0, t1                                  ; Make sure 16-byte alignment

    /* Actually build the stack frame.  */

#if __iar_riscv_base_isa == rv32e 
    addi    t0, t0, -260
#else
    addi    t0, t0, -128                                ; Allocate space for the stack frame
#endif
    li      t1, 1                                       ; Build stack type
    sw      t1, 0(t0)                                   ; Place stack type on the top
    sw      x0, 4(t0)                                   ; Initial s11
    sw      x0, 8(t0)                                   ; Initial s10
    sw      x0, 12(t0)                                  ; Initial s9
    sw      x0, 16(t0)                                  ; Initial s8
    sw      x0, 20(t0)                                  ; Initial s7
    sw      x0, 24(t0)                                  ; Initial s6
    sw      x0, 28(t0)                                  ; Initial s5
    sw      x0, 32(t0)                                  ; Initial s4
    sw      x0, 36(t0)                                  ; Initial s3
    sw      x0, 40(t0)                                  ; Initial s2
    sw      x0, 44(t0)                                  ; Initial s1
    sw      x0, 48(t0)                                  ; Initial s0
    sw      x0, 52(t0)                                  ; Initial t6
    sw      x0, 56(t0)                                  ; Initial t5
    sw      x0, 60(t0)                                  ; Initial t4
    sw      x0, 64(t0)                                  ; Initial t3
    sw      x0, 68(t0)                                  ; Initial t2
    sw      x0, 72(t0)                                  ; Initial t1
    sw      x0, 76(t0)                                  ; Initial t0
    sw      x0, 80(t0)                                  ; Initial a7
    sw      x0, 84(t0)                                  ; Initial a6
    sw      x0, 88(t0)                                  ; Initial a5
    sw      x0, 92(t0)                                  ; Initial a4
    sw      x0, 96(t0)                                  ; Initial a3
    sw      x0, 100(t0)                                 ; Initial a2
    sw      x0, 104(t0)                                 ; Initial a1
    sw      x0, 108(t0)                                 ; Initial a0
    sw      x0, 112(t0)                                 ; Initial ra
    sw      a1, 120(t0)                                 ; Initial mepc
#if __iar_riscv_base_isa == rv32e 
    sw      x0, 124(t0)                                 ; Inital ft0
    sw      x0, 128(t0)                                 ; Inital ft1
    sw      x0, 132(t0)                                 ; Inital ft2
    sw      x0, 136(t0)                                 ; Inital ft3
    sw      x0, 140(t0)                                 ; Inital ft4
    sw      x0, 144(t0)                                 ; Inital ft5
    sw      x0, 148(t0)                                 ; Inital ft6
    sw      x0, 152(t0)                                 ; Inital ft7
    sw      x0, 156(t0)                                 ; Inital fs0
    sw      x0, 160(t0)                                 ; Inital fs1
    sw      x0, 164(t0)                                 ; Inital fa0
    sw      x0, 168(t0)                                 ; Inital fa1
    sw      x0, 172(t0)                                 ; Inital fa2
    sw      x0, 176(t0)                                 ; Inital fa3
    sw      x0, 180(t0)                                 ; Inital fa4
    sw      x0, 184(t0)                                 ; Inital fa5
    sw      x0, 188(t0)                                 ; Inital fa6
    sw      x0, 192(t0)                                 ; Inital fa7
    sw      x0, 196(t0)                                 ; Inital fs2
    sw      x0, 200(t0)                                 ; Inital fs3
    sw      x0, 204(t0)                                 ; Inital fs4
    sw      x0, 208(t0)                                 ; Inital fs5
    sw      x0, 212(t0)                                 ; Inital fs6
    sw      x0, 216(t0)                                 ; Inital fs7
    sw      x0, 220(t0)                                 ; Inital fs8
    sw      x0, 224(t0)                                 ; Inital fs9
    sw      x0, 228(t0)                                 ; Inital fs10
    sw      x0, 232(t0)                                 ; Inital fs11
    sw      x0, 236(t0)                                 ; Inital ft8
    sw      x0, 240(t0)                                 ; Inital ft9
    sw      x0, 244(t0)                                 ; Inital ft10
    sw      x0, 248(t0)                                 ; Inital ft11
    csrr    a1, fcsr                                    ; Read fcsr and use it for initial value for each thread
    sw      a1, 252(t0)                                 ; Initial fscr
    sw      x0, 256(t0)                                 ; Reserved word (0)
#else
    sw      x0, 124(t0)                                 ; Reserved word (0)
#endif

    /* Setup stack pointer.  */
    /* thread_ptr -> tx_thread_stack_ptr =  t0;  */

    sw      t0, 8(a0)                                   ; Save stack pointer in thread's
    ret                                                 ;   control block and return
/* }  */
    END
    
