##############################################################################
# Copyright (c) 2024 Microsoft Corporation
# Copyright (c) 2026 Eclipse ThreadX contributors
#
# This program and the accompanying materials are made available under the
# terms of the MIT License which is available at
# https://opensource.org/licenses/MIT.
#
# SPDX-License-Identifier: MIT
##############################################################################

import subprocess
import sys
import os
import argparse
import socket
import select

def print_content(content):
    """Prints content using os.write to handle non-blocking stdout robustly."""
    msg = f"{content}\n".encode('utf-8')
    total_len = len(msg)
    written = 0
    fd = sys.stdout.fileno()
    while written < total_len:
        try:
            n = os.write(fd, msg[written:])
            written += n
        except BlockingIOError:
            select.select([], [fd], [])

def get_free_port():
    """Finds a free TCP port."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(('', 0))
        return s.getsockname()[1]

def run_qemu_test(elf_path, qemu_bin, gdb_bin, cpu):
    """
    Runs a test cycle using QEMU and GDB.
    """
    print(f"Testing ELF: {elf_path}")
    print(f"QEMU: {qemu_bin}")
    print(f"GDB: {gdb_bin}")

    # Find a free port for GDB connection
    gdb_port = get_free_port()
    print(f"Using GDB port: {gdb_port}")

    # 1. Start QEMU in the background
    GDB_TIMEOUT_S = 120
    qemu_log_path = os.path.abspath("qemu-riscv64.log")
    qemu_cmd = [
        qemu_bin,
        "-M", "virt",
        "-nographic",
        "-bios", "none",       # Disable default OpenSBI
        "-kernel", elf_path,
        "-gdb", f"tcp::{gdb_port}", "-S",
        "-monitor", "none",    # Disable monitor
        "-serial", "stdio"     # Redirect serial output to stdio
    ]
    if cpu:
        qemu_cmd.extend(["-cpu", cpu])

    print(f"Starting QEMU: {' '.join(qemu_cmd)}")
    qemu_log = open(qemu_log_path, "w", encoding="utf-8")
    try:
        qemu_process = subprocess.Popen(
            qemu_cmd,
            stdout=qemu_log,
            stderr=subprocess.STDOUT,
            text=True
        )
    except OSError as e:
        qemu_log.close()
        print_content(f"FAILURE: QEMU did not start: {e}")
        return False

    print_content(
        f"QEMU PID {qemu_process.pid}; expected runtime under 30s; "
        f"wall timeout {GDB_TIMEOUT_S}s; log {qemu_log_path}"
    )

    if qemu_process.poll() is not None:
        qemu_log.close()
        print_content("FAILURE: QEMU exited during startup.")
        with open(qemu_log_path, encoding="utf-8") as failed_log:
            print_content(failed_log.read())
        return False

    # 2. Create a GDB command file
    gdb_cmds = """
file {elf}
target remote :{port}
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
  printf "FPU_VERIFIED_FAIL_NO_SYMBOL\\n"
else
  print fpu_test_val
  # Assert the float unit produced an exact multiple of 1.1f. This does
  # not assume how many passes ran, only that every pass added 1.1f.
  set $fpu_n = (int)((fpu_test_val / 1.1) + 0.5)
  set $fpu_err = fpu_test_val - ($fpu_n * 1.1)
  if $fpu_n >= 1 && $fpu_err > -0.01 && $fpu_err < 0.01
    printf "FPU_VERIFIED_OK value=%f adds=%d\\n", fpu_test_val, $fpu_n
  else
    printf "FPU_VERIFIED_FAIL value=%f adds=%d passes=%d\\n", fpu_test_val, $fpu_n, $fpu_iter
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
  printf "MEPC_VERIFIED_OK mepc=0x%lx frame_pc=0x%lx\\n", $mepc, $frame_pc
else
  printf "MEPC_VERIFIED_FAIL saved=0x%lx mepc=0x%lx frame_pc=0x%lx\\n", $saved_pc, $mepc, $frame_pc
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
      printf "PREEMPT_CHECK current_prio=%d execute_prio=%d\\n", $curr_prio, $exec_prio
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
  printf "PREEMPT_VERIFIED_OK ticks=%d\\n", $preempt_iter
else
  if $preempt_null == $preempt_iter
    printf "PREEMPT_VERIFIED_FAIL_NULL\\n"
  else
    printf "PREEMPT_VERIFIED_FAIL_NOT_OBSERVED ticks=%d\\n", $preempt_iter
  end
end

quit
""".format(port=gdb_port, elf=elf_path)

    gdb_cmd_file = "test_cmds.gdb"
    with open(gdb_cmd_file, "w") as f:
        f.write(gdb_cmds)

    # 3. Run GDB
    gdb_cmd = [
        gdb_bin,
        "--batch",
        "-x", gdb_cmd_file
    ]

    print_content(f"Starting GDB: {' '.join(gdb_cmd)}")

    # Cap the GDB session so a wedged batch script (e.g. a `continue` that
    # never hits its breakpoint) cannot hang CI. The preemption poll may
    # step through up to 250 ticks; at the port's 100 Hz tick that is 2.5 s
    # of guest time, but each tick costs two gdbstub round trips, so allow
    # 120 s of wall clock before declaring the session wedged.
    try:
        gdb_process = subprocess.run(
            gdb_cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            timeout=GDB_TIMEOUT_S,
        )

        print_content("GDB Output:")
        print_content(gdb_process.stdout)
        if gdb_process.stderr:
            print_content("GDB Error Output:")
            print_content(gdb_process.stderr)

    except subprocess.TimeoutExpired as e:
        print_content(
            f"FAILURE: GDB session exceeded {GDB_TIMEOUT_S}s timeout; "
            "likely stuck on a `continue` that never matched a breakpoint."
        )
        if e.stdout:
            print_content("GDB Output (partial):")
            print_content(e.stdout if isinstance(e.stdout, str)
                          else e.stdout.decode(errors='replace'))
        if e.stderr:
            print_content("GDB Error Output (partial):")
            print_content(e.stderr if isinstance(e.stderr, str)
                          else e.stderr.decode(errors='replace'))
        return False

    except Exception as e:
        print_content(f"An error occurred during test execution: {e}")
        return False

    finally:
        # 4. Clean up
        print_content("Stopping QEMU...")
        qemu_process.terminate()
        try:
            qemu_process.wait(timeout=2)
        except subprocess.TimeoutExpired:
            print_content("QEMU did not terminate gracefully, killing it forcefully.")
            qemu_process.kill()
            qemu_process.wait()
        qemu_log.close()

    # Verify results
    stdout = gdb_process.stdout
    gdb_succeeded = gdb_process.returncode == 0
    timer_hit = "Breakpoint 4, _tx_timer_interrupt" in stdout
    fpu_verified = False
    mepc_verified = "MEPC_VERIFIED_OK" in stdout
    preemption_verified = "PREEMPT_VERIFIED_OK" in stdout
    time_slice_verified = "SUCCESS: Time-slice handler called." in stdout
    system_timer_verified = "SUCCESS: System timer incremented." in stdout

    if not gdb_succeeded:
        print_content(f"FAILURE: GDB exited with status {gdb_process.returncode}.")

    if "Breakpoint 3, thread_6_and_7_entry" in stdout:
        if "FPU_VERIFIED_OK" in stdout:
            print_content("SUCCESS: FPU instructions executed and registers inspected.")
            fpu_verified = True
        elif "FPU_VERIFIED_FAIL_NO_SYMBOL" in stdout:
            print_content("FAILURE: Hit thread, but this demo defines no "
                          "fpu_test_val, so the FPU result is unverifiable.")
        elif "FPU_VERIFIED_FAIL" in stdout:
            print_content("FAILURE: Hit thread, but fpu_test_val is not an exact "
                          "multiple of 1.1f. Output does not contain expected value.")
        else:
            print_content("FAILURE: Hit thread, but failed to inspect FPU. "
                          "Output does not contain expected value.")

    if timer_hit:
        print_content("SUCCESS: Timer Interrupt verified! Hit _tx_timer_interrupt.")
    else:
        print_content("FAILURE: Did not hit timer interrupt.")

    if mepc_verified:
        print_content("SUCCESS: MEPC and the saved frame PC are unchanged.")
    else:
        print_content("FAILURE: MEPC or the saved frame PC changed in the ISR.")

    if time_slice_verified:
        print_content("SUCCESS: Time-slice handler verified.")
    else:
        print_content("FAILURE: Time-slice handler was not called.")

    if system_timer_verified:
        print_content("SUCCESS: System timer increment verified.")
    else:
        print_content("FAILURE: System timer did not increment.")

    if preemption_verified:
        print_content("SUCCESS: Preemption verified (higher-priority thread "
                      "preempted a lower-priority one).")
    else:
        if "PREEMPT_VERIFIED_FAIL_INVERTED" in stdout:
            print_content("FAILURE: Preemption inverted -- lower priority "
                          "thread scheduled over higher priority one.")
        elif "PREEMPT_VERIFIED_FAIL_NULL" in stdout:
            print_content("FAILURE: Preemption check saw NULL thread pointers.")
        elif "PREEMPT_VERIFIED_FAIL_NOT_OBSERVED" in stdout:
            print_content("FAILURE: Preemption was not observed within the "
                          "loop budget.")
        else:
            print_content("FAILURE: Preemption check did not run to completion.")

    if (gdb_succeeded and timer_hit and fpu_verified and mepc_verified and
            time_slice_verified and system_timer_verified and
            preemption_verified):
        return True
    else:
        return False

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run ThreadX RV64 QEMU/GDB Test")
    parser.add_argument("--elf", required=True, help="Path to the kernel ELF file")
    parser.add_argument("--qemu", default="qemu-system-riscv64",
                        help="Path to QEMU binary")
    parser.add_argument("--gdb", default="riscv64-unknown-elf-gdb",
                        help="Path to GDB binary")
    parser.add_argument("--cpu", help="QEMU CPU model and properties")

    args = parser.parse_args()

    success = run_qemu_test(args.elf, args.qemu, args.gdb, args.cpu)

    if success:
        sys.exit(0)
    else:
        sys.exit(1)
