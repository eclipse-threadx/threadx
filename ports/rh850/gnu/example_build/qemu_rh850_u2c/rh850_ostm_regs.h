/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef RH850_OSTM_REGS_H
#define RH850_OSTM_REGS_H

#define RH850_OSTM_CMP 0x00
#define RH850_OSTM_CNT 0x04
#define RH850_OSTM_TE  0x10
#define RH850_OSTM_TS  0x14
#define RH850_OSTM_TT  0x18
#define RH850_OSTM_CTL 0x20

#define RH850_OSTM_CTL_MD0 (1 << 0)
#define RH850_OSTM_CTL_MD1 (1 << 1)
#define RH850_OSTM_CTL_MD2 (1 << 2)
#define RH850_OSTM_CTL_IE  (1 << 7)

#endif
