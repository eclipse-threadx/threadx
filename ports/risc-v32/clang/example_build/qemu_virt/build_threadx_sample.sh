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


DIRNAME=$(dirname "$0")
BASEDIR=$DIRNAME/../../../../..

mkdir -p build

CC=clang-18
LD=ld.lld-18

$CC -I $BASEDIR/ports/risc-v32/clang/inc -I $BASEDIR/build/custom_inc -g --sysroot=/opt/riscv_rv32ima/riscv32-unknown-elf --target=riscv32 -march=rv32ima_zicsr -mabi=ilp32 -o build/entry.obj -c entry.S
$CC -DTX_INCLUDE_USER_DEFINE_FILE -I $BASEDIR/ports/risc-v32/clang/inc -I $BASEDIR/build/custom_inc -isystem $BASEDIR/common/inc -g --sysroot=/opt/riscv_rv32ima/riscv32-unknown-elf --target=riscv32 -march=rv32ima_zicsr -mabi=ilp32 -D__ASSEMBLER__ -o build/tx_initialize_low_level.obj -c tx_initialize_low_level.S
$CC -DTX_INCLUDE_USER_DEFINE_FILE -I $BASEDIR/ports/risc-v32/clang/inc -I $BASEDIR/build/custom_inc -isystem $BASEDIR/common/inc -g --sysroot=/opt/riscv_rv32ima/riscv32-unknown-elf --target=riscv32 -march=rv32ima_zicsr -mabi=ilp32 -o build/board.obj -c board.c
$CC -DTX_INCLUDE_USER_DEFINE_FILE -I $BASEDIR/ports/risc-v32/clang/inc -I $BASEDIR/build/custom_inc -isystem $BASEDIR/common/inc -g --sysroot=/opt/riscv_rv32ima/riscv32-unknown-elf --target=riscv32 -march=rv32ima_zicsr -mabi=ilp32 -o build/hwtimer.obj -c hwtimer.c
$CC -DTX_INCLUDE_USER_DEFINE_FILE -I $BASEDIR/ports/risc-v32/clang/inc -I $BASEDIR/build/custom_inc -isystem $BASEDIR/common/inc -g --sysroot=/opt/riscv_rv32ima/riscv32-unknown-elf --target=riscv32 -march=rv32ima_zicsr -mabi=ilp32 -o build/plic.obj -c plic.c
$CC -DTX_INCLUDE_USER_DEFINE_FILE -I $BASEDIR/ports/risc-v32/clang/inc -I $BASEDIR/build/custom_inc -isystem $BASEDIR/common/inc -g --sysroot=/opt/riscv_rv32ima/riscv32-unknown-elf --target=riscv32 -march=rv32ima_zicsr -mabi=ilp32 -o build/trap.obj -c trap.c
$CC -DTX_INCLUDE_USER_DEFINE_FILE -I $BASEDIR/ports/risc-v32/clang/inc -I $BASEDIR/build/custom_inc -isystem $BASEDIR/common/inc -g --sysroot=/opt/riscv_rv32ima/riscv32-unknown-elf --target=riscv32 -march=rv32ima_zicsr -mabi=ilp32 -o build/uart.obj -c uart.c
$CC -DTX_INCLUDE_USER_DEFINE_FILE -I $BASEDIR/ports/risc-v32/clang/inc -I $BASEDIR/build/custom_inc -isystem $BASEDIR/common/inc -g --sysroot=/opt/riscv_rv32ima/riscv32-unknown-elf --target=riscv32 -march=rv32ima_zicsr -mabi=ilp32 -o build/demo_threadx.obj -c demo_threadx.c
$LD -Tlink.lds --no-dynamic-linker -m elf32lriscv -static -nostdlib -o build/demo_threadx.elf --Map=build/demo_threadx.map build/entry.obj build/tx_initialize_low_level.obj build/board.obj build/hwtimer.obj build/plic.obj build/trap.obj build/uart.obj build/demo_threadx.obj $BASEDIR/build/libthreadx.a
