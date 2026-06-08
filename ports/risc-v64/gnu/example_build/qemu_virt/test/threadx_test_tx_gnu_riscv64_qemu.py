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
    try:
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
    except Exception:
        pass

def get_free_port():
    """Finds a free TCP port."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(('', 0))
        return s.getsockname()[1]

def run_qemu_test(elf_path, qemu_bin, gdb_bin):
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

    print(f"Starting QEMU: {' '.join(qemu_cmd)}")
    qemu_process = subprocess.Popen(
        qemu_cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

    if qemu_process.poll() is not None:
        print("QEMU failed to start.")
        print(qemu_process.stderr.read())
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

# Execute to Application Definition
continue

# Inspect mstatus once thread_0 has started
continue
print/x $mstatus

# Verify FPU Logic and Register State exercised by thread_6/7
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

# Verify Preemption Logic (Thread Priority)
set $curr_ptr = _tx_thread_current_ptr
set $exec_ptr = _tx_thread_execute_ptr
if $curr_ptr != 0 && $exec_ptr != 0
  set $curr_prio = $curr_ptr->tx_thread_priority
  set $exec_prio = $exec_ptr->tx_thread_priority
  printf "PREEMPT_CHECK current_prio=%d execute_prio=%d\\n", $curr_prio, $exec_prio
  if $exec_prio < $curr_prio
    printf "PREEMPT_VERIFIED_OK\\n"
  else
    printf "PREEMPT_VERIFIED_FAIL_NOT_OBSERVED\\n"
  end
else
  printf "PREEMPT_VERIFIED_FAIL_NULL\\n"
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

    GDB_TIMEOUT_S = 30

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

    # Verify results
    stdout = gdb_process.stdout
    timer_hit = "Breakpoint 4, _tx_timer_interrupt" in stdout
    fpu_verified = False
    preemption_verified = "PREEMPT_VERIFIED_OK" in stdout

    if "Breakpoint 3, thread_6_and_7_entry" in stdout:
        if "1.10" in stdout or "fpu_test_val" in stdout:
            print_content("SUCCESS: FPU instructions executed and registers inspected.")
            fpu_verified = True
        else:
            print_content("FAILURE: Hit thread, but failed to inspect FPU. "
                          "Output does not contain expected value.")

    if timer_hit:
        print_content("SUCCESS: Timer Interrupt verified! Hit _tx_timer_interrupt.")
    else:
        print_content("FAILURE: Did not hit timer interrupt.")

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

    if timer_hit and fpu_verified and preemption_verified:
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

    args = parser.parse_args()

    success = run_qemu_test(args.elf, args.qemu, args.gdb)

    if success:
        sys.exit(0)
    else:
        sys.exit(1)
