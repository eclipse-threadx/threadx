arm-none-eabi-gcc -c -g -mcpu=cortex-a9 reset.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 crt0.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 tx_initialize_low_level.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 -I../../../../common/inc -I../inc sample_threadx.c
arm-none-eabi-gcc -g -mcpu=cortex-a8 -T sample_threadx.ld --specs=nosys.specs -o sample_threadx.out -Wl,-Map=sample_threadx.map tx_initialize_low_level.o sample_threadx.o tx.a

