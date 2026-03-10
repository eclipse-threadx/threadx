
target remote :65299
file /Users/winston/Documents/碩二/CA/threadx/build_qemu/kernel.elf
set pagination off
set confirm off

# Setup Breakpoints
break tx_application_define
break thread_0_entry
break thread_6_and_7_entry
break _tx_timer_interrupt

# Execute to Application Definition
continue

# Verify Lazy FPU Context (Expect FS=Initial)
continue
print/x $mstatus

# Verify FPU Logic and Register State
continue
finish
step
step
step
print/x $mstatus
info registers float
print fpu_test_val

# Await Timer Interrupt
continue
print "Hit Timer Interrupt"

# Verify MEPC Integrity - Save State
print/x $mepc
set $saved_pc = $mepc

# Verify System Timer Before ISR
set $clock_before = _tx_timer_system_clock
print $clock_before

# Configure Time-Slice Test Conditions
set _tx_timer_time_slice = 1
set _tx_timer_expired_time_slice = 0
set $ts_handler_called = 0

# Set Breakpoint at Time-Slice Handler with Auto-Continue
tbreak _tx_thread_time_slice
commands
  set $ts_handler_called = 1
  continue
end

# Set Breakpoint at ISR Return Address
set $ret_addr = $ra
tbreak *$ret_addr
continue

# Verify Time-Slice Handler Was Called
if $ts_handler_called == 1
  print "SUCCESS: Time-slice handler called."
else
  print "FAILURE: Time-slice handler NOT called."
end

# Verify System Timer Increment (Monotonicity)
set $clock_after = _tx_timer_system_clock
print $clock_after

if $clock_after > $clock_before
  print "SUCCESS: System timer incremented."
else
  print "FAILURE: System timer did not increment."
end

# Verify MEPC Restoration Post-ISR
tbreak *$saved_pc
continue

print "Back from ISR"
print/x $pc
set $diff = (long)$pc - (long)$saved_pc
if $diff == 0
  print "SUCCESS: MEPC restored correctly."
else
  print "FAILURE: PC does not match saved MEPC."
end

# Verify Preemption Logic (Thread Priority)
break tx_thread_context_restore.S:320

set $max_loops = 5
set $loop_cnt = 0
set $found_preemption = 0

while $loop_cnt < $max_loops
  continue
  set $loop_cnt = $loop_cnt + 1
  
  print "Hit Preemption Restore Path"

  set $curr_ptr = _tx_thread_current_ptr
  set $exec_ptr = _tx_thread_execute_ptr

  if $curr_ptr != 0 && $exec_ptr != 0
    set $curr_prio = $curr_ptr->tx_thread_priority
    set $exec_prio = $exec_ptr->tx_thread_priority

    print $curr_prio
    print $exec_prio

    if $exec_prio < $curr_prio
      print "SUCCESS: Thread Preemption Verified."
      set $found_preemption = 1
      loop_break
    end

    if $exec_prio > $curr_prio
      print "FAILURE: Preemption logic error - Lower priority running."
      loop_break
    end
  else
    print "FAILURE: Null thread pointers."
    loop_break
  end
end

if $found_preemption == 0
  print "FAILURE: Preemption not observed."
end

quit
