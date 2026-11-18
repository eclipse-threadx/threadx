/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include "rh850_cpu.h"
#include "rh850_fault.h"
#include "rh850_io.h"
#include "rh850_intc.h"
#include "rh850_intc_regs.h"
#include "soc.h"

#ifndef RH850_INTC_HANDLERS
#define RH850_INTC_HANDLERS SOC_INTC_CHANNELS
#endif

#if RH850_INTC_HANDLERS < 255
typedef uint8_t rh850_intc_slot_t;
#else
typedef uint16_t rh850_intc_slot_t;
#endif

struct rh850_intc_entry {
	rh850_isr_t isr;
	void *arg;
};

static rh850_intc_slot_t rh850_intc_slot[SOC_INTC_CHANNELS];
static struct rh850_intc_entry rh850_intc_table[RH850_INTC_HANDLERS];
static unsigned int rh850_intc_used;

static uintptr_t eic_addr(unsigned int ch)
{
	uintptr_t base = (ch < SOC_INTC1_CHANNELS) ? SOC_INTC1_BASE : SOC_INTC2_BASE;

	return base + (uintptr_t)ch * RH850_EIC_STRIDE;
}

/* Write the low byte of EICn only, so EIRF stays */
static void eic_update(unsigned int ch, uint8_t clear, uint8_t set)
{
	uintptr_t addr = eic_addr(ch);
	uint32_t key = rh850_irq_save();
	uint8_t v = rh850_read8(addr);

	/* EIOV is kept; bit 4 is reserved and written as 0 */
	v &= (uint8_t)~(clear | 0x10U);
	v |= set;
	rh850_write8(addr, v);
	(void)rh850_read8(addr);
	rh850_syncp();
	rh850_irq_restore(key);
}

void rh850_intc_connect(unsigned int ch, rh850_isr_t isr, void *arg)
{
	if (ch >= SOC_INTC_CHANNELS) {
		return;
	}

	uint32_t key = rh850_irq_save();
	unsigned int slot = rh850_intc_slot[ch];

	if (slot == 0U) {
		if (rh850_intc_used == RH850_INTC_HANDLERS) {
			rh850_fault(0, 0x10000U | ch, 0);
		}
		slot = ++rh850_intc_used;
		rh850_intc_slot[ch] = (rh850_intc_slot_t)slot;
	}
	rh850_intc_table[slot - 1U].isr = isr;
	rh850_intc_table[slot - 1U].arg = arg;
	rh850_irq_restore(key);
}

void rh850_intc_set_priority(unsigned int ch, unsigned int prio)
{
	if (ch < SOC_INTC_CHANNELS) {
		eic_update(ch, RH850_EIC_EIP_MASK | RH850_EIC_EITB,
			   (uint8_t)(prio & RH850_EIC_EIP_MASK));
	}
}

void rh850_intc_enable(unsigned int ch)
{
	if (ch < SOC_INTC_CHANNELS) {
		eic_update(ch, RH850_EIC_EIMK, 0);
	}
}

void rh850_intc_disable(unsigned int ch)
{
	if (ch < SOC_INTC_CHANNELS) {
		eic_update(ch, 0, RH850_EIC_EIMK);
	}
}

void rh850_intc_raise(unsigned int ch)
{
	if (ch < SOC_INTC_CHANNELS) {
		uintptr_t addr = eic_addr(ch) + 1U;

	/* Upper byte: EIRF is bit 4, the other bits read-only or 0 */
		rh850_write8(addr, (uint8_t)(RH850_EIC_EIRF >> 8));
		(void)rh850_read8(addr);
		rh850_syncp();
	}
}

void rh850_intc_clear(unsigned int ch)
{
	if (ch < SOC_INTC_CHANNELS) {
		uintptr_t addr = eic_addr(ch) + 1U;

		rh850_write8(addr, 0);
		(void)rh850_read8(addr);
		rh850_syncp();
	}
}

void rh850_intc_dispatch(uint32_t eiic)
{
	uint32_t ch = (eiic & 0xffffU) - RH850_EIIC_EIINT;
	unsigned int slot = (ch < SOC_INTC_CHANNELS) ? rh850_intc_slot[ch] : 0U;

	if ((slot == 0U) || (rh850_intc_table[slot - 1U].isr == NULL)) {
		rh850_fault(0, eiic, RH850_STSR(RH850_SR_EIPC));
	}

	rh850_intc_table[slot - 1U].isr(rh850_intc_table[slot - 1U].arg);
}
