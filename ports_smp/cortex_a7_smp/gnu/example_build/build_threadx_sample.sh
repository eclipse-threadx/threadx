#!/bin/sh
set -e
arm-none-eabi-gcc -c -g -I../../../../common_smp/inc -I../inc -mcpu=cortex-a7 sample_threadx.c
arm-none-eabi-gcc -c -g -mcpu=cortex-a7 startup.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a7 MP_GIC.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a7 MP_Mutexes.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a7 v7.S
arm-none-eabi-gcc -T sample_threadx.ld --specs=nosys.specs -e Vectors -o sample_threadx.out MP_GIC.o MP_Mutexes.o sample_threadx.o startup.o v7.o tx.a -Wl,-M > sample_threadx.map
