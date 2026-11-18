/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

	.macro	vector target
	.balign	16
	jr	\target
	.endm

	.section .vectors, "ax"
	.balign	512
	.global	rh850_vectors
rh850_vectors:
	vector	__start			/* 000H RESET */
	vector	rh850_fe_fault		/* 010H SYSERR */
	vector	rh850_fe_fault		/* 020H reserved */
	vector	rh850_fe_fault		/* 030H FETRAP */
	vector	rh850_trap0_entry	/* 040H TRAP0 */
	vector	rh850_ei_fault		/* 050H TRAP1 */
	vector	rh850_fe_fault		/* 060H RIE */
	vector	rh850_ei_fault		/* 070H FPE, FPI */
	vector	rh850_fe_fault		/* 080H UCPOP */
	vector	rh850_fe_fault		/* 090H MIP, MDP */
	vector	rh850_fe_fault		/* 0A0H PIE */
	vector	rh850_fe_fault		/* 0B0H reserved */
	vector	rh850_fe_fault		/* 0C0H MAE */
	vector	rh850_fe_fault		/* 0D0H reserved */
	vector	rh850_fe_fault		/* 0E0H FENMI */
	vector	rh850_fe_fault		/* 0F0H FEINT */
	.rept	16
	vector	rh850_eiint_entry	/* 100H-1F0H EIINT priority 0-15 */
	.endr
