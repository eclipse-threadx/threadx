#!/bin/bash
##############################################################################
# Copyright (c) 2024 Microsoft Corporation
# Copyright (c) 2026 Eclipse ThreadX contributors
#
# This program and the accompanying materials are made available under the
# terms of the MIT License which is available at
# https://opensource.org/licenses/MIT.
#
# SPDX-License-Identifier: MIT
##############################################################################


dst=$(dirname $0)/../../regression/tx_initialize_low_level.c
src=$(dirname $0)/../../../../ports/linux/gnu/src/tx_initialize_low_level.c

line=`sed -n '/_tx_linux_timer_interrupt/=' $src | tail -n 1`
sed "${line}iVOID  test_interrupt_dispatch(VOID);" $src > tmp1
line=`sed -n '/_tx_timer_interrupt/=' $src | tail -n 1`
sed "${line}itest_interrupt_dispatch();" tmp1 > $dst
rm tmp1