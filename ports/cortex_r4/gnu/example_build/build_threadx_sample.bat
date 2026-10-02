arm-none-eabi-gcc -c -g -mcpu=cortex-r4 reset.S
arm-none-eabi-gcc -c -g -mcpu=cortex-r4 crt0.S
arm-none-eabi-gcc -c -g -mcpu=cortex-r4 tx_initialize_low_level.S
arm-none-eabi-gcc -c -g -mcpu=cortex-r4 -I../../../../common/inc -I../inc sample_threadx.c
arm-none-eabi-gcc -g -nostartfiles -mcpu=cortex-r4 -T sample_threadx.ld --specs=nosys.specs -o sample_threadx.out -Wl,-Map=sample_threadx.map reset.o crt0.o tx_initialize_low_level.o sample_threadx.o tx.a

