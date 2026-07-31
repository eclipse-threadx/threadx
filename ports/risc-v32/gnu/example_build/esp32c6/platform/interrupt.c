/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 *
 * Inspired by and derived in part from architectural patterns in:
 * - Espressif Systems ESP-IDF (Apache-2.0)
 * - Zephyr RTOS Project (Apache-2.0)
 **************************************************************************/

#include "esp32c6_example.h"

#define MAX_CPU_INTERRUPTS 32

static esp32c6_isr_handler_t s_isr_table[MAX_CPU_INTERRUPTS];
static void                 *s_isr_arg[MAX_CPU_INTERRUPTS];

void esp32c6_interrupt_init(void)
{
    for (uint32_t i = 0; i < MAX_CPU_INTERRUPTS; i++) {
        s_isr_table[i] = NULL;
        s_isr_arg[i]   = NULL;
    }

    /* Enable PCR INTMTX bus clock (bit 0 of 0x60096090) & clear reset */
    REG_WRITE(0x60096090, 1);

    /* Disable all PLIC CPU interrupt lines & clear pending interrupts on boot */
    REG_WRITE(PLIC_MXINT_ENABLE_REG, 0);
    REG_WRITE(PLIC_MXINT_CLEAR_REG, 0xFFFFFFFF);
    REG_WRITE(PLIC_MXINT_THRESH_REG, 0);

    /* Enable Machine External & Timer Interrupts in RISC-V mie CSR */
    uint32_t mie_mask = RISCV_MIE_MEIE | RISCV_MIE_MTIE;
    __asm__ __volatile__ ("csrs mie, %0" :: "r"(mie_mask));
}

void esp32c6_interrupt_attach(uint32_t cpu_intr_num,
                              uint32_t intr_source,
                              esp32c6_isr_handler_t handler,
                              void *arg)
{
    if (cpu_intr_num == 0 || cpu_intr_num >= MAX_CPU_INTERRUPTS || handler == NULL) {
        return;
    }

    s_isr_table[cpu_intr_num] = handler;
    s_isr_arg[cpu_intr_num]   = arg;

    /* Route hardware peripheral interrupt source to CPU interrupt line */
    interrupt_plic_ll_route(intr_source, cpu_intr_num);

    /* Set default priority (level 1) */
    REG_WRITE(PLIC_MXINT_PRI_REG(cpu_intr_num), 1);

    /* Enable CPU interrupt line in PLIC */
    REG_WRITE(PLIC_MXINT_ENABLE_REG, REG_READ(PLIC_MXINT_ENABLE_REG) | (1UL << cpu_intr_num));
}

void esp32c6_interrupt_dispatch(void)
{
    uint32_t mcause_val;
    __asm__ __volatile__ ("csrr %0, mcause" : "=r"(mcause_val));

    /* Check if cause is a RISC-V Machine Hardware Interrupt (highest bit set) */
    if (mcause_val & RISCV_MCAUSE_INTERRUPT_FLAG) {
        /*
         * RISC-V PLIC Hardware Dispatch:
         * Claim active CPU interrupt line from PLIC_MXINT_CLAIM_REG (0x20001094).
         * For PLIC Machine External Interrupts (mcause = 11), PLIC Claim Register returns
         * the active CPU Interrupt Line Number (1-31).
         */
        uint32_t cpu_intr_num = REG_READ(PLIC_MXINT_CLAIM_REG);

        if (cpu_intr_num > 0 && cpu_intr_num < MAX_CPU_INTERRUPTS) {
            if (s_isr_table[cpu_intr_num] != NULL) {
                s_isr_table[cpu_intr_num](s_isr_arg[cpu_intr_num]);
            }
            /* Complete PLIC claim transaction by writing claimed line back to PLIC */
            REG_WRITE(PLIC_MXINT_CLAIM_REG, cpu_intr_num);
        }
    }
}
