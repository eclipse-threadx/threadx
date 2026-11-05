/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef RH850_ASM_H
#define RH850_ASM_H

/* GCC v850 puts __USER_LABEL_PREFIX__ ("_") in front of C symbols */
#define RH850_CAT2(a, b) a##b
#define RH850_CAT(a, b)  RH850_CAT2(a, b)
#define C_SYM(s)         RH850_CAT(__USER_LABEL_PREFIX__, s)

#ifdef __ASSEMBLER__
	.macro	rh850_stsr id, sel, reg
	stsr	\id, \reg, \sel
	.endm
#endif

#endif
