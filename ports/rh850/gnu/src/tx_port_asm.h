/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#ifndef TX_PORT_ASM_H
#define TX_PORT_ASM_H

#include "rh850_core.h"
#include "rh850_asm.h"

#ifndef RH850_FLOAT_HARD
#error "RH850_FLOAT_HARD (0 or 1) must be set by the build"
#endif

#if RH850_CORE_HAS_FPU && RH850_FLOAT_HARD
#define TX_PORT_USE_FPU 1
#else
#define TX_PORT_USE_FPU 0
#endif

#define TX_THREAD_RUN_COUNT  4
#define TX_THREAD_STACK_PTR  8
#define TX_THREAD_STACK_END  16
#define TX_THREAD_TIME_SLICE 24

#define TX_FRAME_PLMR  0
#define TX_FRAME_FPEPC 4
#define TX_FRAME_FPSR  8
#define TX_FRAME_EIPSW 12
#define TX_FRAME_EIPC  16
#define TX_FRAME_R1    20
#define TX_FRAME_R2    24
#define TX_FRAME_R(n)  (28 + ((n) - 4) * 4)
#define TX_FRAME_SIZE  144

/* Nested interrupt frame: the call-clobbered registers only */
#define TX_ISR_EIPC   0
#define TX_ISR_EIPSW  4
#define TX_ISR_FPSR   8
#define TX_ISR_FPEPC  12
#define TX_ISR_R1     16
#define TX_ISR_R2     20
#define TX_ISR_R5     24
#define TX_ISR_R(n)   (28 + ((n) - 6) * 4)
#define TX_ISR_R30    84
#define TX_ISR_R31    88
#define TX_ISR_SIZE   96

#ifdef __ASSEMBLER__

	.macro	tx_frame_save
#if TX_PORT_USE_FPU && RH850_CORE_HAS_SYNCE
	/* Complete FPU operations and FPI reports before FPSR is read */
	synce
#endif
	addi	-TX_FRAME_SIZE, sp, sp
	st.w	r1, TX_FRAME_R1[sp]
	st.w	r2, TX_FRAME_R2[sp]
	.irp	n, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, \
		19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
	st.w	r\n, TX_FRAME_R(\n)[sp]
	.endr
	rh850_stsr	RH850_SR_EIPC, r10
	st.w	r10, TX_FRAME_EIPC[sp]
	rh850_stsr	RH850_SR_EIPSW, r10
	st.w	r10, TX_FRAME_EIPSW[sp]
#if TX_PORT_USE_FPU
	rh850_stsr	RH850_SR_FPSR, r10
	st.w	r10, TX_FRAME_FPSR[sp]
	rh850_stsr	RH850_SR_FPEPC, r10
	st.w	r10, TX_FRAME_FPEPC[sp]
#endif
	rh850_stsr	RH850_CORE_MASK_SR, r10
	st.w	r10, TX_FRAME_PLMR[sp]
	.endm

	/* While an FPI is pending (FPEC.FPIVD), keep FPEPC and the FPSR cause bits */
	.macro	tx_fpu_restore fpsr, fpepc
	ld.w	\fpsr[sp], r10
#if RH850_CORE_HAS_FPEC
	rh850_stsr	RH850_SR_FPEC, r11
	andi	RH850_FPEC_FPIVD, r11, r11
	bz	8f
	rh850_stsr	RH850_SR_FPSR, r11
	andi	RH850_FPSR_XC_MASK, r11, r11
	andi	RH850_FPSR_XC_MASK, r10, r12
	xor	r12, r10
	or	r11, r10
	ldsr	r10, RH850_SR_FPSR
	br	9f
8:
#endif
	ldsr	r10, RH850_SR_FPSR
	ld.w	\fpepc[sp], r10
	ldsr	r10, RH850_SR_FPEPC
9:
	.endm

	.macro	tx_frame_restore
	ld.w	TX_FRAME_PLMR[sp], r10
	ldsr	r10, RH850_CORE_MASK_SR
#if TX_PORT_USE_FPU
	tx_fpu_restore	TX_FRAME_FPSR, TX_FRAME_FPEPC
#endif
	ld.w	TX_FRAME_EIPSW[sp], r10
	ldsr	r10, RH850_SR_EIPSW
	ld.w	TX_FRAME_EIPC[sp], r10
	ldsr	r10, RH850_SR_EIPC
	ld.w	TX_FRAME_R1[sp], r1
	ld.w	TX_FRAME_R2[sp], r2
	.irp	n, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, \
		19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
	ld.w	TX_FRAME_R(\n)[sp], r\n
	.endr
	addi	TX_FRAME_SIZE, sp, sp
	eiret
	.endm

	.macro	tx_isr_save
#if TX_PORT_USE_FPU && RH850_CORE_HAS_SYNCE
	synce
#endif
	addi	-TX_ISR_SIZE, sp, sp
	st.w	r1, TX_ISR_R1[sp]
	st.w	r2, TX_ISR_R2[sp]
	st.w	r5, TX_ISR_R5[sp]
	.irp	n, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
	st.w	r\n, TX_ISR_R(\n)[sp]
	.endr
	st.w	r30, TX_ISR_R30[sp]
	st.w	r31, TX_ISR_R31[sp]
	rh850_stsr	RH850_SR_EIPC, r10
	st.w	r10, TX_ISR_EIPC[sp]
	rh850_stsr	RH850_SR_EIPSW, r10
	st.w	r10, TX_ISR_EIPSW[sp]
#if TX_PORT_USE_FPU
	rh850_stsr	RH850_SR_FPSR, r10
	st.w	r10, TX_ISR_FPSR[sp]
	rh850_stsr	RH850_SR_FPEPC, r10
	st.w	r10, TX_ISR_FPEPC[sp]
#endif
	.endm

	.macro	tx_isr_restore
#if TX_PORT_USE_FPU
	tx_fpu_restore	TX_ISR_FPSR, TX_ISR_FPEPC
#endif
	ld.w	TX_ISR_EIPSW[sp], r10
	ldsr	r10, RH850_SR_EIPSW
	ld.w	TX_ISR_EIPC[sp], r10
	ldsr	r10, RH850_SR_EIPC
	ld.w	TX_ISR_R1[sp], r1
	ld.w	TX_ISR_R2[sp], r2
	ld.w	TX_ISR_R5[sp], r5
	.irp	n, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
	ld.w	TX_ISR_R(\n)[sp], r\n
	.endr
	ld.w	TX_ISR_R30[sp], r30
	ld.w	TX_ISR_R31[sp], r31
	addi	TX_ISR_SIZE, sp, sp
	eiret
	.endm

#endif

#endif
