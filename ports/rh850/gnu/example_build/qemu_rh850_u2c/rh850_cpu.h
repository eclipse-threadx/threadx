/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef RH850_CPU_H
#define RH850_CPU_H

#include <stdint.h>
#include "rh850_sysreg.h"

#define rh850_stsr_(id, sel) ({ \
	uint32_t v_; \
	__asm__ volatile("stsr %1, %0, %2" : "=r"(v_) : "i"(id), "i"(sel)); \
	v_; })
#define rh850_ldsr_(v, id, sel) \
	__asm__ volatile("ldsr %0, %1, %2" :: "r"(v), "i"(id), "i"(sel) : "memory")

#define RH850_STSR(sr)    rh850_stsr_(sr)
#define RH850_LDSR(v, sr) rh850_ldsr_((uint32_t)(v), sr)

static inline void rh850_di(void)
{
	__asm__ volatile("di" ::: "memory");
}

static inline void rh850_ei(void)
{
	__asm__ volatile("ei" ::: "memory");
}

static inline void rh850_syncp(void)
{
	__asm__ volatile("syncp" ::: "memory");
}

static inline void rh850_halt(void)
{
	__asm__ volatile("halt" ::: "memory");
}

static inline uint32_t rh850_irq_save(void)
{
	uint32_t psw = RH850_STSR(RH850_SR_PSW);

	rh850_di();
	return psw;
}

static inline void rh850_irq_restore(uint32_t psw)
{
	if ((psw & RH850_PSW_ID) == 0U) {
		rh850_ei();
	}
}

#endif
