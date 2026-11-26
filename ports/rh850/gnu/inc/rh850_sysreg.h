/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef RH850_SYSREG_H
#define RH850_SYSREG_H

#define RH850_SR_EIPC   0, 0
#define RH850_SR_EIPSW  1, 0
#define RH850_SR_FEPC   2, 0
#define RH850_SR_FEPSW  3, 0
#define RH850_SR_PSW    5, 0
#define RH850_SR_FPSR   6, 0
#define RH850_SR_FPEPC  7, 0
#define RH850_SR_FPST   8, 0
#define RH850_SR_FPCC   9, 0
#define RH850_SR_FPCFG  10, 0
#define RH850_SR_EIIC   13, 0
#define RH850_SR_FEIC   14, 0
#define RH850_SR_CTPC   16, 0
#define RH850_SR_CTPSW  17, 0
#define RH850_SR_CTBP   20, 0
#define RH850_SR_EIWR   28, 0
#define RH850_SR_FEWR   29, 0
#define RH850_SR_RBASE  2, 1
#define RH850_SR_EBASE  3, 1
#define RH850_SR_INTBP  4, 1
#define RH850_SR_MCTL   5, 1
#define RH850_SR_PID    6, 1
#define RH850_SR_SCCFG  11, 1
#define RH850_SR_SCBP   12, 1
#define RH850_SR_ISPR   10, 2
#define RH850_SR_ICSR   12, 2
#define RH850_SR_INTCFG 13, 2

#define RH850_PSW_ID  0x00000020
#define RH850_PSW_EP  0x00000040
#define RH850_PSW_NP  0x00000080
#define RH850_PSW_EBV 0x00008000
#define RH850_PSW_CU0 0x00010000
#define RH850_PSW_UM  0x40000000

#endif
