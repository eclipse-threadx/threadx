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

	.section .text.rh850_fault, "ax"

	.global	rh850_fe_fault
rh850_fe_fault:
	mov	1, r6
	rh850_stsr	RH850_SR_FEIC, r7
	rh850_stsr	RH850_SR_FEPC, r8
	mov	hilo(C_SYM(rh850_fault)), r10
	jmp	[r10]

	.weak	rh850_trap0_entry
rh850_trap0_entry:
	.global	rh850_ei_fault
rh850_ei_fault:
	mov	0, r6
	rh850_stsr	RH850_SR_EIIC, r7
	rh850_stsr	RH850_SR_EIPC, r8
	mov	hilo(C_SYM(rh850_fault)), r10
	jmp	[r10]
