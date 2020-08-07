set NSIM_MULTICORE=1
mdb -pset=1 -psetname=Core1 sample_threadx\Debug\sample_threadx.elf -arconnect -connect_ici -av2hs -Xatomic -Xunaligned -core3 -Xmpy_option=mpy -Xtimer0 -Xtimer1	-nooptions -interrupts=32 -ext_interrupts=3 -prop=nsim_isa_num_actionpoints=8 -off=prefer_soft_bp -nogoifmain 
mdb -pset=2 -psetname=Core2 sample_threadx\Debug\sample_threadx.elf -arconnect -connect_ici -av2hs -Xatomic -Xunaligned -core3 -Xmpy_option=mpy -Xtimer0 -Xtimer1	-nooptions -interrupts=32 -ext_interrupts=3 -prop=nsim_isa_num_actionpoints=8 -off=prefer_soft_bp -nogoifmain 
mdb -multifiles=Core1,Core2 -OKN
