/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include "rh850_asm.h"
#include "rh850_sysreg.h"
#include "rh850_core.h"

	.section .text.rh850_eiint, "ax"
	.weak	rh850_eiint_entry
rh850_eiint_entry:
	pushsp	r1-r2
	pushsp	r5-r19
	pushsp	r30-r31
	add	-12, sp
	rh850_stsr	RH850_SR_EIPC, r10
	st.w	r10, 0[sp]
	rh850_stsr	RH850_SR_EIPSW, r10
	st.w	r10, 4[sp]
#if RH850_CORE_HAS_FPU
	rh850_stsr	RH850_SR_FPSR, r10
	st.w	r10, 8[sp]
#endif

	rh850_stsr	RH850_SR_EIIC, r6
	jarl	C_SYM(rh850_intc_dispatch), lp

#if RH850_CORE_HAS_FPU
	/* A pending FPI keeps its FPSR cause bits */
	ld.w	8[sp], r10
#if RH850_CORE_HAS_FPEC
	rh850_stsr	RH850_SR_FPEC, r11
	andi	RH850_FPEC_FPIVD, r11, r11
	bz	1f
	rh850_stsr	RH850_SR_FPSR, r11
	andi	RH850_FPSR_XC_MASK, r11, r11
	andi	RH850_FPSR_XC_MASK, r10, r12
	xor	r12, r10
	or	r11, r10
1:
#endif
	ldsr	r10, RH850_SR_FPSR
#endif
	ld.w	4[sp], r10
	ldsr	r10, RH850_SR_EIPSW
	ld.w	0[sp], r10
	ldsr	r10, RH850_SR_EIPC
	add	12, sp
	popsp	r30-r31
	popsp	r5-r19
	popsp	r1-r2
	eiret
