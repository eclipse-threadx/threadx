arm-none-eabi-gcc -c -g -mcpu=cortex-a9 reset.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 crt0.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 tx_initialize_low_level.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 -I../../../../common/inc -I../inc sample_threadx.c
arm-none-eabi-ld -A cortex-a9 -T sample_threadx.ld reset.o crt0.o tx_initialize_low_level.o sample_threadx.o tx.a libc.a libgcc.a -o sample_threadx.out -M > sample_threadx.map

