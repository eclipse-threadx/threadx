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

/* Regression test start-up for CPU0 of the TC375 Lite Kit without a driver
   library. The start-up code of the compiler calls main. printf output goes
   polled to ASCLIN0 (P14.0, 115200 8N1), the virtual COM port of the Lite
   Kit.

   The clock system keeps its reset state, so the fast ASCLIN clock is the
   back-up clock of 100 MHz divided by SCU_CCUCON2.ASCLINFDIV (1 after
   reset).  */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "tx_api.h"


/* CPU0 watchdog: ENDINIT protects ASCLIN0_CLC.  */
#define SCU_WDTCPU0_CON0            (*(volatile unsigned int *) 0xF003624CUL)
#define WDT_CON0_ENDINIT            0x00000001UL
#define WDT_CON0_LCK                0x00000002UL
#define WDT_CON0_PW_OFF             2
#define WDT_CON0_PW_MSK             0x3FFFUL
#define WDT_CON0_REL_MSK            0xFFFF0000UL
#define WDT_CON0_PW_READ_INVERT     0x003FUL

/* ASCLIN0.  */
#define ASCLIN0_CLC                 (*(volatile unsigned int *) 0xF0000600UL)
#define ASCLIN0_TXFIFOCON           (*(volatile unsigned int *) 0xF000060CUL)
#define ASCLIN0_BITCON              (*(volatile unsigned int *) 0xF0000614UL)
#define ASCLIN0_FRAMECON            (*(volatile unsigned int *) 0xF0000618UL)
#define ASCLIN0_DATCON              (*(volatile unsigned int *) 0xF000061CUL)
#define ASCLIN0_BRG                 (*(volatile unsigned int *) 0xF0000620UL)
#define ASCLIN0_TXDATA              (*(volatile unsigned int *) 0xF0000644UL)
#define ASCLIN0_CSR                 (*(volatile unsigned int *) 0xF000064CUL)

#define ASCLIN_CLC_DISS             0x00000002UL
#define ASCLIN_CSR_CLKSEL_FAST      0x00000002UL    /* fASCLINF */
#define ASCLIN_CSR_CON              0x80000000UL
#define ASCLIN_FRAMECON_STOP_1      (1UL << 9)
#define ASCLIN_FRAMECON_MODE_ASC    (1UL << 16)
#define ASCLIN_DATCON_DATLEN_8      7UL
#define ASCLIN_TXFIFOCON_FLUSH      0x00000001UL
#define ASCLIN_TXFIFOCON_ENO        0x00000002UL
#define ASCLIN_TXFIFOCON_INW_1      (1UL << 6)
#define ASCLIN_TXFIFOCON_FILL_OFF   16
#define ASCLIN_TXFIFOCON_FILL_MSK   0x1FUL
#define ASCLIN_TX_FIFO_SIZE         16U

/* 115200 baud from fASCLINF = 100 MHz: no prescaler, 16 samples per bit
   (6.25 MHz) and the fractional divider 75/4069, which gives 115200.3 baud.
   The bit is sampled once, at sample 8.  */
#define ASCLIN_BITCON_115200        ((8UL << 24) | (15UL << 16) | 0UL)
#define ASCLIN_BRG_115200           ((75UL << 16) | 4069UL)

/* P14.0: push-pull output of alternate function 2, ASCLIN0 ATX.  */
#define P14_IOCR0                   (*(volatile unsigned int *) 0xF003AE10UL)
#define PORT_IOCR0_PC0_MSK          0x000000F8UL
#define PORT_IOCR0_PC0_ALT2_PP      (0x12UL << 3)


void tx_regression_main(void);


/* Sets (endinit = WDT_CON0_ENDINIT) or clears (endinit = 0) the ENDINIT bit
   of the CPU0 watchdog.  */
static void cpu0_endinit_write(unsigned int endinit)
{
    unsigned int con0;
    unsigned int password;

    con0 = SCU_WDTCPU0_CON0;
    password = (((con0 >> WDT_CON0_PW_OFF) & WDT_CON0_PW_MSK) ^ WDT_CON0_PW_READ_INVERT) << WDT_CON0_PW_OFF;
    if ((con0 & WDT_CON0_LCK) != 0U)
    {
        SCU_WDTCPU0_CON0 = (con0 & WDT_CON0_REL_MSK) | password | WDT_CON0_ENDINIT;
    }
    SCU_WDTCPU0_CON0 = (con0 & WDT_CON0_REL_MSK) | password | WDT_CON0_LCK | endinit;
    (void) SCU_WDTCPU0_CON0;
}


static void uart_init(void)
{
    /* Enable the module.  */
    cpu0_endinit_write(0U);
    ASCLIN0_CLC = 0U;
    cpu0_endinit_write(WDT_CON0_ENDINIT);
    while ((ASCLIN0_CLC & ASCLIN_CLC_DISS) != 0U)
    {
    }

    /* The kernel clock is off while the module is configured.  */
    ASCLIN0_CSR = 0U;
    while ((ASCLIN0_CSR & ASCLIN_CSR_CON) != 0U)
    {
    }

    ASCLIN0_FRAMECON = 0U;
    ASCLIN0_BITCON = ASCLIN_BITCON_115200;
    ASCLIN0_BRG = ASCLIN_BRG_115200;
    ASCLIN0_DATCON = ASCLIN_DATCON_DATLEN_8;
    ASCLIN0_FRAMECON = ASCLIN_FRAMECON_STOP_1 | ASCLIN_FRAMECON_MODE_ASC;
    ASCLIN0_TXFIFOCON = ASCLIN_TXFIFOCON_INW_1 | ASCLIN_TXFIFOCON_ENO | ASCLIN_TXFIFOCON_FLUSH;

    P14_IOCR0 = (P14_IOCR0 & ~PORT_IOCR0_PC0_MSK) | PORT_IOCR0_PC0_ALT2_PP;

    ASCLIN0_CSR = ASCLIN_CSR_CLKSEL_FAST;
    while ((ASCLIN0_CSR & ASCLIN_CSR_CON) == 0U)
    {
    }
}


static void uart_put(char c)
{
    while (((ASCLIN0_TXFIFOCON >> ASCLIN_TXFIFOCON_FILL_OFF) & ASCLIN_TXFIFOCON_FILL_MSK) >= (ASCLIN_TX_FIFO_SIZE - 1U))
    {
    }
    ASCLIN0_TXDATA = (unsigned char) c;
}


/* Replaces write() of libos, which forwards output to a virtual I/O host.  */
int write(int desc, void *buf, size_t len)
{
    const char *p = (const char *) buf;
    size_t      i;

    (void) desc;
    for (i = 0; i < len; i++)
    {
        if (p[i] == '\n')
        {
            uart_put('\r');
        }
        uart_put(p[i]);
    }
    return (int) len;
}


void external_exit(UINT code)
{
    printf("**** REGRESSION EXIT CODE %u ****\n", code);
    while (1)
    {
    }
}


/* Start and end of the test memory in DSPR1, from regression.ld. The start-up
   code clears only the sections of the default linker script.  */
extern unsigned char __bss_cpu1_start[];
extern unsigned char __bss_cpu1_end[];


int main(void)
{
    memset(__bss_cpu1_start, 0, (size_t) (__bss_cpu1_end - __bss_cpu1_start));

    uart_init();
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("\n**** REGRESSION START ****\n");

    tx_regression_main();

    while (1)
    {
    }
}
