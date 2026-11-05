/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef RH850_RLIN3_REGS_H
#define RH850_RLIN3_REGS_H

#define RH850_RLIN3_LWBR   0x01
#define RH850_RLIN3_LBRP01 0x02
#define RH850_RLIN3_LMD    0x08
#define RH850_RLIN3_LBFC   0x09
#define RH850_RLIN3_LEDE   0x0d
#define RH850_RLIN3_LCUC   0x0e
#define RH850_RLIN3_LMST   0x11
#define RH850_RLIN3_LST    0x12
#define RH850_RLIN3_LUOER  0x20
#define RH850_RLIN3_LUOR1  0x21
#define RH850_RLIN3_LUTDR  0x24

#define RH850_RLIN3_SAMPLES 16
#define RH850_RLIN3_LMD_UART 0x01
#define RH850_RLIN3_LCUC_OM0  (1 << 0)
#define RH850_RLIN3_LMST_OMM0 (1 << 0)
#define RH850_RLIN3_LST_UTS   (1 << 4)
#define RH850_RLIN3_LUOER_UTOE (1 << 0)
#define RH850_RLIN3_LUOER_UROE (1 << 1)

#endif
