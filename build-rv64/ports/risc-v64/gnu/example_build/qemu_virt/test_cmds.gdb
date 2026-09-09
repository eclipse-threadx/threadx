
file /tmp/claude-1000/-home-fdesbiens-652-threadx/3e4a51a5-e5ba-416c-90fd-fe90ff96f461/scratchpad/wt698/build-rv64/ports/risc-v64/gnu/example_build/qemu_virt/kernel.elf
target remote :46936
set pagination off
set confirm off

# Setup Breakpoints
break tx_application_define
break thread_0_entry
break thread_6_and_7_entry
break _tx_timer_interrupt

# The timer interrupt fires at TX_TIMER_TICKS_PER_SECOND Hz. With every
# breakpoint armed at once a bare `continue` is a race: a tick can steal
# the stop that the script expects on an application breakpoint. Keep the
# timer breakpoint disabled until the timer phase, so every stop below is
# unambiguous at any tick rate.
disable 4

# Execute to Application Definition
continue

# Inspect mstatus once thread_0 has started
continue
print/x $mstatus

# Verify FPU Logic and Register State exercised by thread_6/7.
# Only breakpoint 3 can stop us here, so this is thread_6_and_7_entry on
# its first pass, before the thread has touched fpu_test_val.
continue

# fpu_test_val exists only in a demo that builds the FPU exercise. GDB
# aborts a sourced command file on an unknown symbol, which would skip
# every later check without saying so, so probe for the symbol first. A
# missing symbol still FAILS the FPU check below; it is not tolerated.
python gdb.execute("set $fpu_sym = %d" % (1 if gdb.lookup_global_symbol("fpu_test_val") else 0))

set $fpu_iter = 0
if $fpu_sym != 0
  set $fpu_before = fpu_test_val
  # Wait for the FPU add (fpu_test_val += 1.1f) to retire, instead of
  # stepping a fixed number of instructions past an assumed stop. Each
  # further stop is another pass through thread_6_and_7_entry, so the cap
  # bounds thread passes rather than ticks, and exhausting it fails below.
  while fpu_test_val == $fpu_before && $fpu_iter < 64
    set $fpu_iter = $fpu_iter + 1
    continue
  end
end

print/x $mstatus
info registers float

if $fpu_sym == 0
  printf "FPU_VERIFIED_FAIL_NO_SYMBOL\n"
else
  print fpu_test_val
  # Assert the float unit produced an exact multiple of 1.1f. This does
  # not assume how many passes ran, only that every pass added 1.1f.
  set $fpu_n = (int)((fpu_test_val / 1.1) + 0.5)
  set $fpu_err = fpu_test_val - ($fpu_n * 1.1)
  if $fpu_n >= 1 && $fpu_err > -0.01 && $fpu_err < 0.01
    printf "FPU_VERIFIED_OK value=%f adds=%d\n", fpu_test_val, $fpu_n
  else
    printf "FPU_VERIFIED_FAIL value=%f adds=%d passes=%d\n", fpu_test_val, $fpu_n, $fpu_iter
  end
end

# Await Timer Interrupt. Arm only the timer breakpoint, so this stop is
# _tx_timer_interrupt and $ra below really is the ISR return address.
disable 1
disable 2
disable 3
enable 4
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

# Set Breakpoint at ISR Return Address. Disable the timer breakpoint for
# this window, so the next tick cannot stop us short of the return.
set $ret_addr = $ra
disable 4
tbreak *$ret_addr
continue

# Verify that the live CSR and saved frame still contain the interrupted PC.
set $frame_pc = ((unsigned long *)_tx_thread_current_ptr->tx_thread_stack_ptr)[30]
if $mepc == $saved_pc && $frame_pc == $saved_pc
  printf "MEPC_VERIFIED_OK mepc=0x%lx frame_pc=0x%lx\n", $mepc, $frame_pc
else
  printf "MEPC_VERIFIED_FAIL saved=0x%lx mepc=0x%lx frame_pc=0x%lx\n", $saved_pc, $mepc, $frame_pc
end

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

# Verify Preemption Logic (Thread Priority)
#
# We are now stopped at the return address from _tx_timer_interrupt,
# after _tx_thread_time_slice and the timer expiration processing have had
# a chance to update _tx_thread_execute_ptr but before trap_handler
# returns into _tx_thread_context_restore.
#
# Whether any ONE tick lands inside a preemption window depends on
# scheduling phase, not on the tick rate, so poll consecutive ticks at
# this same sample point until the priority relation holds. Each retry
# re-arms the time slice and re-derives the return address from $ra, so
# the sample point is identical every iteration. The cap bounds ticks and
# fails loudly on exhaustion; it never passes the check.
set $preempt_ok = 0
set $preempt_null = 0
set $preempt_iter = 0
while $preempt_iter < 250 && $preempt_ok == 0
  set $curr_ptr = _tx_thread_current_ptr
  set $exec_ptr = _tx_thread_execute_ptr
  if $curr_ptr == 0 || $exec_ptr == 0
    set $preempt_null = $preempt_null + 1
  else
    set $curr_prio = $curr_ptr->tx_thread_priority
    set $exec_prio = $exec_ptr->tx_thread_priority
    if $exec_prio < $curr_prio
      printf "PREEMPT_CHECK current_prio=%d execute_prio=%d\n", $curr_prio, $exec_prio
      set $preempt_ok = 1
    end
  end
  if $preempt_ok == 0
    set $preempt_iter = $preempt_iter + 1
    enable 4
    continue
    set _tx_timer_time_slice = 1
    set _tx_timer_expired_time_slice = 0
    set $ret_addr = $ra
    disable 4
    tbreak *$ret_addr
    continue
  end
end

if $preempt_ok == 1
  printf "PREEMPT_VERIFIED_OK ticks=%d\n", $preempt_iter
else
  if $preempt_null == $preempt_iter
    printf "PREEMPT_VERIFIED_FAIL_NULL\n"
  else
    printf "PREEMPT_VERIFIED_FAIL_NOT_OBSERVED ticks=%d\n", $preempt_iter
  end
end

quit
