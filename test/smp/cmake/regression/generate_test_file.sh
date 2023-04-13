#!/bin/bash

dst=$(dirname $0)/../../regression/tx_initialize_low_level.c
src=$(dirname $0)/../../../../ports_smp/linux/gnu/src/tx_initialize_low_level.c

line=`sed -n '/_tx_linux_timer_interrupt/=' $src | tail -n 1`
sed "${line}iVOID  test_interrupt_dispatch(VOID);" $src > tmp1
line=`sed -n '/_tx_timer_interrupt/=' $src | tail -n 1`
sed "${line}itest_interrupt_dispatch();" tmp1 > $dst
rm tmp1