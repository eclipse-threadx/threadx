arm-none-eabi-gcc -c -g -mcpu=cortex-r5 reset.S
arm-none-eabi-gcc -c -g -mcpu=cortex-r5 crt0.S
arm-none-eabi-gcc -c -g -mcpu=cortex-r5 tx_initialize_low_level.S
arm-none-eabi-gcc -c -g -mcpu=cortex-r5 -I../../../../common/inc -I../inc sample_threadx.c
arm-none-eabi-gcc -g -nostartfiles -mcpu=cortex-r5 -T sample_threadx.ld --specs=nosys.specs -o sample_threadx.out -Wl,-Map=sample_threadx.map reset.o crt0.o tx_initialize_low_level.o sample_threadx.o tx.a

