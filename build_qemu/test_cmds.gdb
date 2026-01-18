
target remote :63370
file /Users/winston/Documents/碩二/CA/threadx/build_qemu/kernel.elf
break tx_application_define
break thread_0_entry
break thread_6_and_7_entry
break _tx_timer_interrupt
continue
continue
print/x $mstatus
continue
finish
step
step
step
print/x $mstatus
info registers float
print fpu_test_val
continue
quit
