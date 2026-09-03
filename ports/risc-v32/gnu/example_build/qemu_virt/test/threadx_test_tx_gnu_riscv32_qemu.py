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
    """Write content to nonblocking stdout with os.write."""
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
    """Get an unused TCP port."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(('', 0))
        return s.getsockname()[1]

def run_qemu_test(elf_path, qemu_bin, gdb_bin, skip_fpu):
    """Run one QEMU and GDB test cycle."""
    print(f"Testing ELF: {elf_path}")
    print(f"QEMU: {qemu_bin}")
    print(f"GDB: {gdb_bin}")

    # Get an unused GDB port.
    gdb_port = get_free_port()
    print(f"Using GDB port: {gdb_port}")

    # Start QEMU and wait for GDB.
    GDB_TIMEOUT_S = 120
    qemu_log_path = os.path.abspath("qemu-riscv32.log")
    qemu_cmd = [
        qemu_bin,
        "-M", "virt",
        "-nographic",
        "-bios", "none", # Start without OpenSBI at 0x80000000.
        "-kernel", elf_path,
        "-gdb", f"tcp::{gdb_port}", "-S",
        "-monitor", "none", # Disable the QEMU monitor.
        "-serial", "stdio"  # Send serial output to stdout.
    ]
    
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

    # Create the GDB command file.
    gdb_cmds = """
file {elf}
target remote :{port}
set pagination off
set confirm off

# Set breakpoints.
break tx_application_define
break thread_0_entry
break thread_6_and_7_entry
break _tx_timer_interrupt

disable 4

# Run to the application setup.
continue


continue
print/x $mstatus
continue

# Check for fpu_test_val before GDB uses it.
# An absent symbol fails the required FPU test.
python gdb.execute("set $fpu_sym = %d" % (1 if gdb.lookup_global_symbol("fpu_test_val") else 0))
set $fpu_required = {fpu_required}

set $fpu_iter = 0
if $fpu_sym != 0
  set $fpu_before = fpu_test_val
  # Continue until one FPU addition completes.
  # Limit the wait to 64 thread passes.
  while fpu_test_val == $fpu_before && $fpu_iter < 64
    set $fpu_iter = $fpu_iter + 1
    continue
  end
end

print/x $mstatus
info registers float

if $fpu_sym == 0
  if $fpu_required != 0
    printf "FPU_VERIFIED_FAIL_NO_SYMBOL\\n"
  else
    printf "FPU_VERIFIED_SKIP_SOFT_FLOAT\\n"
  end
else
  print fpu_test_val
  # Check for a multiple of 1.1f with a 0.01 tolerance.
  set $fpu_n = (int)((fpu_test_val / 1.1) + 0.5)
  set $fpu_err = fpu_test_val - ($fpu_n * 1.1)
  if $fpu_n >= 1 && $fpu_err > -0.01 && $fpu_err < 0.01
    printf "FPU_VERIFIED_OK value=%f adds=%d\\n", fpu_test_val, $fpu_n
  else
    printf "FPU_VERIFIED_FAIL value=%f adds=%d passes=%d\\n", fpu_test_val, $fpu_n, $fpu_iter
  end
end

# Enable only the timer breakpoint.
# Then $ra contains the ISR return address.
disable 1
disable 2
disable 3
enable 4
continue
print "Hit Timer Interrupt"

# Save mepc at timer entry.
print/x $mepc
set $saved_pc = $mepc

# Read the system clock before the ISR completes.
set $clock_before = _tx_timer_system_clock
print $clock_before

# Force time-slice processing.
set _tx_timer_time_slice = 1
set _tx_timer_expired_time_slice = 0
set $ts_handler_called = 0

# Record time-slice handler calls.
tbreak _tx_thread_time_slice
commands
  set $ts_handler_called = 1
  continue
end

# Stop at the ISR return address.
# Disable the timer breakpoint during this interval.
set $ret_addr = $ra
disable 4
tbreak *$ret_addr
continue

# Check the live CSR and the saved frame PC.
set $frame_pc = ((unsigned int *)_tx_thread_current_ptr->tx_thread_stack_ptr)[30]
if $mepc == $saved_pc && $frame_pc == $saved_pc
  printf "MEPC_VERIFIED_OK mepc=0x%x frame_pc=0x%x\\n", $mepc, $frame_pc
else
  printf "MEPC_VERIFIED_FAIL saved=0x%x mepc=0x%x frame_pc=0x%x\\n", $saved_pc, $mepc, $frame_pc
end

# Check time-slice handler execution.
if $ts_handler_called == 1
  print "SUCCESS: Time-slice handler called."
else
  print "FAILURE: Time-slice handler NOT called."
end

# Check that the system clock increased.
set $clock_after = _tx_timer_system_clock
print $clock_after

if $clock_after > $clock_before
  print "SUCCESS: System timer incremented."
else
  print "FAILURE: System timer did not increment."
end

# Compare the current and selected thread priorities at the ISR return.
# Check each tick until preemption occurs or 250 ticks expire.
# Each retry uses the same ISR point and starts a new time slice.
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
""".format(port=gdb_port, elf=elf_path, fpu_required=0 if skip_fpu else 1)

    gdb_cmd_file = "test_cmds.gdb"
    with open(gdb_cmd_file, "w") as f:
        f.write(gdb_cmds)

    # Run GDB in batch mode.
    gdb_cmd = [
        gdb_bin,
        "--batch",
        "-x", gdb_cmd_file
    ]

    print_content(f"Starting GDB: {' '.join(gdb_cmd)}")
    
    # Limit GDB to 120 seconds.
    # The loop uses at most 250 ticks and two gdbstub exchanges for each tick.
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
        # Stop QEMU and close its log.
        print_content("Stopping QEMU...")
        qemu_process.terminate()
        try:
            qemu_process.wait(timeout=2)
        except subprocess.TimeoutExpired:
            print_content("QEMU did not terminate gracefully, killing it forcefully.")
            qemu_process.kill()
            qemu_process.wait()
        qemu_log.close()

    # Check each required test result.
    stdout = gdb_process.stdout
    gdb_succeeded = gdb_process.returncode == 0
    timer_hit = "Breakpoint 4, _tx_timer_interrupt" in stdout
    fpu_verified = skip_fpu
    mepc_verified = "MEPC_VERIFIED_OK" in stdout
    preemption_verified = "PREEMPT_VERIFIED_OK" in stdout
    time_slice_verified = "SUCCESS: Time-slice handler called." in stdout
    system_timer_verified = "SUCCESS: System timer incremented." in stdout

    if not gdb_succeeded:
        print_content(f"FAILURE: GDB exited with status {gdb_process.returncode}.")

    if skip_fpu and "FPU_VERIFIED_SKIP_SOFT_FLOAT" in stdout:
        print_content("SUCCESS: Soft-float mode omitted the hardware FPU check.")
    elif "Breakpoint 3, thread_6_and_7_entry" in stdout:
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
             print_content("FAILURE: Hit thread, but failed to inspect FPU. Output does not contain expected value.")

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
    parser = argparse.ArgumentParser(description="Run ThreadX QEMU/GDB Test")
    parser.add_argument("--elf", required=True, help="Path to the kernel ELF file")
    parser.add_argument("--qemu", default="qemu-system-riscv32", help="Path to QEMU binary")
    parser.add_argument("--gdb", default="riscv-none-elf-gdb", help="Path to GDB binary")
    parser.add_argument("--skip-fpu", action="store_true",
                        help="Skip the hardware FPU check for a soft-float build")
    
    args = parser.parse_args()
    
    success = run_qemu_test(args.elf, args.qemu, args.gdb, args.skip_fpu)
    
    if success:
        sys.exit(0)
    else:
        sys.exit(1)
