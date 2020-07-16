armasm -g --cpu ARM9TDMI --apcs /interwork tx_initialize_low_level.s
armcc -g -c -O2 --cpu ARM9TDMI --apcs /interwork -I../../../../common/inc -I../inc sample_threadx.c
armlink -d -o sample_threadx.axf --elf --ro 0 --first tx_initialize_low_level.o(Init) --remove --map --symbols --list sample_threadx.map tx_initialize_low_level.o sample_threadx.o tx.a

