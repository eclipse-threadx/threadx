#!/bin/bash
printf "y\n" | rm -rf ../../../../../build/
rm -f kernel.elf

pushd ../../../../../
cmake -Bbuild -GNinja -DCMAKE_TOOLCHAIN_FILE=cmake/riscv32_gnu.cmake .
cmake --build ./build/
popd

riscv32-unknown-elf-gcc \
  -march=rv32gc -mabi=ilp32d \
  -mcmodel=medany -O0 -g3 -Wall \
  -ffunction-sections -fdata-sections \
  -I../../../../../common/inc \
  -I../../inc \
  entry.s \
  tx_initialize_low_level.S \
  board.c uart.c hwtimer.c plic.c trap.c demo_threadx.c \
  -L../../../../../build -lthreadx \
  -T link.lds -nostartfiles \
  -o kernel.elf


qemu-system-riscv32 -nographic -smp 1 -bios none -m 128M -machine virt -kernel kernel.elf
