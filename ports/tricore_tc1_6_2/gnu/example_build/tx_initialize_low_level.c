/***************************************************************************
 * Copyright (c) 2026 safecubed GmbH
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* Portions of this file were generated with AI assistance. */

/* Low-level initialisation of ThreadX on CPU0 of the TC375 Lite Kit without
   a driver library.

   The start-up code of the compiler (crt0) has disabled the CPU0 watchdog
   and the safety watchdog, initialised the stacks, the context save areas
   and the C data, and calls main of sample_threadx.c, which calls
   tx_kernel_enter. The clock system keeps its reset state: the system runs
   from the back-up clock of 100 MHz.

   _tx_initialize_low_level runs from tx_kernel_enter before the kernel and
   the application are initialised. It installs the trap and interrupt
   vector tables, the ThreadX timer on compare 0 of STM0 and the dispatch
   interrupt of the port. Interrupts stay disabled until _tx_thread_schedule
   starts the first thread.  */

#include "tx_api.h"


/* Core special function registers.  */
#define CSFR_BIV                    0xFE20
#define CSFR_BTV                    0xFE24

/* CPU0 watchdog: ENDINIT protects BIV and BTV.  */
#define SCU_WDTCPU0_CON0            (*(volatile unsigned int *) 0xF003624CUL)
#define WDT_CON0_ENDINIT            0x00000001UL
#define WDT_CON0_LCK                0x00000002UL
#define WDT_CON0_PW_OFF             2
#define WDT_CON0_PW_MSK             0x3FFFUL
#define WDT_CON0_REL_MSK            0xFFFF0000UL
/* CON0.PW reads with bits 5:0 inverted.  */
#define WDT_CON0_PW_READ_INVERT     0x003FUL

/* Clock control: fSTM = fSOURCE / CCUCON0.STMDIV, where fSOURCE is the
   back-up clock while CCUCON0.CLKSEL = 0.  */
#define SCU_CCUCON0                 (*(volatile unsigned int *) 0xF0036030UL)
#define CCUCON0_STMDIV_MSK          0x0000000FUL
#define BACKUP_CLOCK_FREQUENCY      100000000UL

/* STM0 compare 0.  */
#define STM0_TIM0                   (*(volatile unsigned int *) 0xF0001010UL)
#define STM0_CMP0                   (*(volatile unsigned int *) 0xF0001030UL)
#define STM0_CMCON                  (*(volatile unsigned int *) 0xF0001038UL)
#define STM0_ICR                    (*(volatile unsigned int *) 0xF000103CUL)
#define STM0_ISCR                   (*(volatile unsigned int *) 0xF0001040UL)
#define STM_CMCON_CMP0_MSK          0x00001F1FUL    /* MSTART0 and MSIZE0 */
#define STM_CMCON_CMP0_32_BITS      0x0000001FUL    /* MSTART0 = 0, MSIZE0 = 31 */
#define STM_ICR_CMP0_MSK            0x00000007UL    /* CMP0EN, CMP0IR and CMP0OS */
#define STM_ICR_CMP0EN              0x00000001UL    /* CMP0OS = 0: service request STMIR0 */
#define STM_ISCR_CMP0IRR            0x00000001UL

/* Service request STMIR0 of STM0.  */
#define SRC_STM0SR0                 (*(volatile unsigned int *) 0xF0038300UL)


VOID    _tx_timer_interrupt(VOID);

#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
VOID    _tx_execution_isr_enter(VOID);
VOID    _tx_execution_isr_exit(VOID);
#endif

extern VOID     *_tx_initialize_unused_memory;

/* From the trap vector table below and the linker script.  */
extern UCHAR    _tx_trap_table[];
extern UCHAR    __INTTAB_CPU0[];
extern UCHAR    __TX_FREE_MEM[];

/* STM ticks per ThreadX timer tick.  */
static ULONG    timer_period;


/* Sets (endinit = WDT_CON0_ENDINIT) or clears (endinit = 0) the ENDINIT bit
   of the CPU0 watchdog. A password access unlocks CON0, the modify access
   then writes ENDINIT and locks CON0 again.  */
static VOID cpu0_endinit_write(ULONG endinit)
{

    ULONG   con0;
    ULONG   password;

    con0 = SCU_WDTCPU0_CON0;
    password = (((con0 >> WDT_CON0_PW_OFF) & WDT_CON0_PW_MSK) ^ WDT_CON0_PW_READ_INVERT) << WDT_CON0_PW_OFF;

    if ((con0 & WDT_CON0_LCK) != 0U)
    {
        SCU_WDTCPU0_CON0 = (con0 & WDT_CON0_REL_MSK) | password | WDT_CON0_ENDINIT;
    }

    SCU_WDTCPU0_CON0 = (con0 & WDT_CON0_REL_MSK) | password | WDT_CON0_LCK | endinit;

    /* The read back completes the write before ENDINIT protected registers
       are accessed.  */
    (VOID) SCU_WDTCPU0_CON0;
}


VOID   _tx_initialize_low_level(VOID)
{

    ULONG   stm_frequency;

    _disable();

    _tx_initialize_unused_memory = (VOID *) __TX_FREE_MEM;

    /* Trap and interrupt vector tables. BIV.VSS = 0: vector spacing of 32
       bytes.  */
    cpu0_endinit_write(0U);
    __mtcr(CSFR_BTV, (ULONG) _tx_trap_table);
    __mtcr(CSFR_BIV, (ULONG) __INTTAB_CPU0);
    _isync();
    cpu0_endinit_write(WDT_CON0_ENDINIT);

    /* ThreadX timer on compare 0 of STM0. The compare covers all 32 bits of
       the lower timer word.  */
    stm_frequency = BACKUP_CLOCK_FREQUENCY / (SCU_CCUCON0 & CCUCON0_STMDIV_MSK);
    timer_period = (stm_frequency + (TX_TIMER_TICKS_PER_SECOND / 2U)) / TX_TIMER_TICKS_PER_SECOND;

    STM0_CMCON = (STM0_CMCON & ~STM_CMCON_CMP0_MSK) | STM_CMCON_CMP0_32_BITS;
    STM0_CMP0 = STM0_TIM0 + timer_period;
    STM0_ISCR = STM_ISCR_CMP0IRR;
    STM0_ICR = (STM0_ICR & ~STM_ICR_CMP0_MSK) | STM_ICR_CMP0EN;
    SRC_STM0SR0 = TX_PORT_TRICORE_SRC_CLRR_MSK | TX_PORT_TRICORE_SRC_SRE_MSK |
                  (0UL << TX_PORT_TRICORE_SRC_TOS_OFF) | TX_PORT_TRICORE_TIMER_INTERRUPT_PRIORITY;

    /* Dispatch interrupt of the port.  */
    TX_PORT_TRICORE_DISPATCH_SRC = TX_PORT_TRICORE_SRC_CLRR_MSK | TX_PORT_TRICORE_SRC_SRE_MSK |
                                   (0UL << TX_PORT_TRICORE_SRC_TOS_OFF) | TX_PORT_TRICORE_DISPATCH_PRIORITY;
}


TX_PORT_TRICORE_INTERRUPT(_tx_timer_isr, TX_PORT_TRICORE_TIMER_INTERRUPT_PRIORITY)
{

    ULONG   now;
    ULONG   next;

#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    _tx_execution_isr_enter();
#endif

    STM0_ISCR = STM_ISCR_CMP0IRR;

    /* The next compare follows the previous one by one period. If the timer
       has already passed that value, for example after a debugger halt, the
       next compare is one period from now and the ticks in between are lost.
       Without this check, the next interrupt would occur only after the
       32-bit timer wraps.  */
    next = STM0_CMP0 + timer_period;
    now  = STM0_TIM0;
    if ((LONG) (next - now) <= 0)
    {
        next = now + timer_period;
    }
    STM0_CMP0 = next;

    _tx_timer_interrupt();

#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    _tx_execution_isr_exit();
#endif
}


/* Receives the trap class and TIN of every trap except the system call and
   shall not return.  */
VOID __attribute__( ( weak, noreturn ) ) tx_trap_error_hook(UINT trap_class, UINT tin)
{
    (VOID) trap_class;
    (VOID) tin;

    _debug();
    while (TX_TRUE)
    {
    }
}

/* Trap vector table of CPU0: eight entries of 32 bytes, aligned to 256
   bytes. The error entries jump, because a context management trap can
   leave no free CSA.  */
__asm__ (".pushsection .traptab, \"ax\", @progbits\n"
         "    .align  8\n"
         "    .global _tx_trap_table\n"
         "    .type   _tx_trap_table, @function\n"
         "_tx_trap_table:\n"

         /* Class 0: MMU.  */
         "    mov     %d4, 0\n"
         "    mov     %d5, %d15\n"
         "    j       tx_trap_error_hook\n"
         "    .align  5\n"

         /* Class 1: internal protection.  */
         "    mov     %d4, 1\n"
         "    mov     %d5, %d15\n"
         "    j       tx_trap_error_hook\n"
         "    .align  5\n"

         /* Class 2: instruction errors.  */
         "    mov     %d4, 2\n"
         "    mov     %d5, %d15\n"
         "    j       tx_trap_error_hook\n"
         "    .align  5\n"

         /* Class 3: context management.  */
         "    mov     %d4, 3\n"
         "    mov     %d5, %d15\n"
         "    j       tx_trap_error_hook\n"
         "    .align  5\n"

         /* Class 4: system bus and peripheral errors.  */
         "    mov     %d4, 4\n"
         "    mov     %d5, %d15\n"
         "    j       tx_trap_error_hook\n"
         "    .align  5\n"

         /* Class 5: assertion.  */
         "    mov     %d4, 5\n"
         "    mov     %d5, %d15\n"
         "    j       tx_trap_error_hook\n"
         "    .align  5\n"

         /* Class 6: system call. The handler shall be called directly
            (TX_PORT_TRICORE_SYSCALL_CALL_DEPTH).  */
         "    svlcx\n"
         "    mov     %d4, %d15\n"
         "    call    _tx_thread_syscall_handler\n"
         "    rslcx\n"
         "    rfe\n"
         "    .align  5\n"

         /* Class 7: non-maskable interrupt.  */
         "    mov     %d4, 7\n"
         "    mov     %d5, %d15\n"
         "    j       tx_trap_error_hook\n"
         "    .align  5\n"

         "    .size   _tx_trap_table, . - _tx_trap_table\n"
         ".popsection\n");
