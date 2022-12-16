mips-sde-elf-gcc -c -g -mmt -EL -I. copy_c2_ram.S
mips-sde-elf-gcc -c -g -mmt -EL -I. demo_threadx.c
mips-sde-elf-gcc -c -g -mmt -EL -I. start.S
mips-sde-elf-gcc -c -g -mmt -EL -I. set_gpr_boot_values.S
mips-sde-elf-gcc -c -g -mmt -EL -I. init_gpr.S
mips-sde-elf-gcc -c -g -mmt -EL -I. init_tlb.S
mips-sde-elf-gcc -c -g -mmt -EL -I. init_itc.S
mips-sde-elf-gcc -c -g -mmt -EL -I. init_caches2.S
mips-sde-elf-gcc -c -g -mmt -EL -I. init_cp0.S
mips-sde-elf-gcc -c -g -mmt -EL -I. init_cm.S
mips-sde-elf-gcc -c -g -mmt -EL -I. release_mp.S
mips-sde-elf-gcc -c -g -mmt -EL -I. join_domain.S
mips-sde-elf-gcc -c -g -mmt -EL -I. init_gic.S
mips-sde-elf-gcc -c -g -mmt -EL -I. init_cpc.S
mips-sde-elf-gcc -c -g -mmt -EL -I. init_vpe1.S
mips-sde-elf-gcc -c -g -mmt -EL -I. init_CoreFPGA6_mem.S
mips-sde-elf-ld -EL copy_c2_ram.o start.o set_gpr_boot_values.o init_gpr.o init_tlb.o init_itc.o init_caches2.o init_cp0.o init_cm.o release_mp.o join_domain.o init_gic.o init_cpc.o init_vpe1.o demo_threadx.o libc.a tx.a -T demo_threadx.ld -o demo_threadx.out -M > demo_threadx.map
mips-sde-elf-objcopy -O srec demo_threadx.out demo_threadx.srec


