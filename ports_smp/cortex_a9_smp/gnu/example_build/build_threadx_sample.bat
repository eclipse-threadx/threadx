arm-none-eabi-gcc -c -g -I../../../../common_smp/inc -I../inc -mcpu=cortex-a5 sample_threadx.c
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 startup.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 MP_GIC.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 MP_SCU.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 MP_Mutexes.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 MP_PrivateTimer.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 v7.S
arm-none-eabi-gcc -T sample_threadx.ld -e Vectors -o sample_threadx.axf MP_PrivateTimer.o MP_GIC.o MP_Mutexes.o MP_SCU.o sample_threadx.o startup.o v7.o tx.a -Wl,-M > sample_threadx.map
