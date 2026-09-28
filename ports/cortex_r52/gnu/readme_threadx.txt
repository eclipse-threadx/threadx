                     Eclipse ThreadX for Cortex-R52
                              Using GNU Tools


1.  Building the ThreadX run-time Library

The port is built with CMake and Ninja.  From the repository root:

    cmake -B build_r52 -G Ninja \
          -DCMAKE_TOOLCHAIN_FILE=cmake/cortex_r52.cmake .
    ninja -C build_r52

This produces libthreadx.a for Cortex-R52 in AArch32 state.  The toolchain
file pins the reference cross compiler by absolute path so that the build
does not depend on PATH ordering; override it with

    -DARM_TOOLCHAIN_PATH=<directory containing arm-none-eabi-gcc>

Note this is the first R-profile port in the tree with a working CMake
build.  cmake/cortex_a9.cmake exists but ports/cortex_a9/gnu/CMakeLists.txt
is empty, so no A- or R-profile port could previously be built this way.


2.  Build options

    -DTX_R52_FLOAT_ABI=soft|hard      floating-point ABI, default soft
    -DTX_R52_ENABLE_VFP=ON            lazy VFP context save and restore
                                      (requires TX_R52_FLOAT_ABI=hard)
    -DTX_R52_ENABLE_FIQ=ON            FIQ support
    -DTX_R52_ENABLE_IRQ_NESTING=ON    nested IRQ support
    -DTX_R52_ENABLE_FIQ_NESTING=ON    nested FIQ support (requires the above)
    -DTX_R52_BUILD_FVP_EXAMPLE=ON     build the Armv8-R AEM FVP examples
    -DTX_R52_ENABLE_MPU=ON            PMSAv8-R protection and caches
    -DTX_R52_CONSOLE_PL011=ON         console on the PL011 UART, not semihosting

TX_R52_ENABLE_VFP is PUBLIC: it changes which registers the context switch
saves, so the library and the application must agree.  It requires
TX_R52_FLOAT_ABI=hard, and configure refuses the combination rather than
warning about it: the flag enables the VFP blocks in the port assembly, and a
soft float ABI leaves the assembler with no FPU to accept them, so the build
fails on the first VMRS rather than quietly skipping the VFP context path.

Cortex-R52 always implements at least a single-precision FPU.  GCC rejects
"-mcpu=cortex-r52+nofp" and offers only "+nofp.dp", so the soft-float
baseline selects the soft ABI rather than removing the FPU.


3.  Example builds and tests

    cmake -B build_r52 -G Ninja \
          -DCMAKE_TOOLCHAIN_FILE=cmake/cortex_r52.cmake \
          -DTX_R52_BUILD_FVP_EXAMPLE=ON .
    ninja -C build_r52 boot_check.elf demo_m2.elf demo_m3.elf \
                       demo_threadx.elf demo_mpu.elf demo_clz.elf
    ctest --test-dir build_r52

The images target the free Armv8-R AEM FVP (FVP_BaseR_AEMv8R):

    boot_check.elf      EL2 configuration, the drop to EL1 and the EL2 seam
    demo_m2.elf         cooperative context switching, no interrupts
    demo_m3.elf         generic timer tick, GICv3 and preemption
    demo_threadx.elf    the standard eight-thread demo plus verification
    demo_mpu.elf        PMSAv8-R protection and cache enable
    demo_clz.elf        the CLZ lowest-set-bit priority search
    demo_m5.elf         lazy VFP context save (needs TX_R52_ENABLE_VFP)

Each image reports its own result and terminates the model through the
semihosting SYS_EXIT call, so no host-side timeout is needed.  The test
runner treats a missing result line as failure, so a hang cannot pass.


4.  System Initialization

The entry point is _start in the example build's entry.S.  Cortex-R52
always implements EL2 and resets into it, so the reset path configures EL2
first, installs both the EL2 and EL1 vector tables, and only then drops to
EL1 (Supervisor mode) to run the kernel.  Define TX_R52_BOOT_AT_EL1 to skip
the EL2 stage where an earlier boot stage or a vendor EL2 monitor has
already dropped privilege.

Work that must happen at EL2, because the registers are inaccessible or
read-only later:

    CNTFRQ                  the model resets it to zero and it is writable
                            only at the highest implemented exception level
    CNTHCTL.PL1PCTEN/PL1PCEN  otherwise every EL1 access to CNTPCT or CNTP_*
                            traps to EL2
    ICC_HSRE.SRE/Enable     lets EL1 reach the GICv3 CPU interface through
                            its system registers
    HCR.HCD cleared         enables HVC, which the EL2 seam depends on
    HCPTR.TCP10/TCP11 cleared  lets EL1 use the FPU

_tx_initialize_low_level records the system stack and publishes the first
free memory address.  It is shorter than the A-profile equivalents because
entry.S has already given every AArch32 mode its own stack from dedicated
linker-script regions, so there is no stack carving and no run-time overlap
check to perform.  With TX_R52_USE_THREADX_IRQ it also calls board_init to
bring up GICv3 and the periodic tick; that is safe this early because
interrupts stay masked until _tx_thread_schedule enables them.


5.  Interrupt Handling

The EL1 IRQ vector branches directly into _tx_thread_context_save, which
returns to __tx_irq_processing_return.  A C dispatcher acknowledges the
interrupt, re-arms the timer, calls _tx_timer_interrupt and finishes with
_tx_thread_context_restore.  The branch must be a plain B: context save
adjusts lr itself to locate the point of interrupt.

Exceptions routed to EL2 from EL1 or EL0 all arrive at the Hyp Trap Entry,
vector offset 0x14, not at 0x08.  Offsets 0x04 to 0x10 are exceptions taken
from Hyp mode itself, and 0x08 is specifically SVC from Hyp.  The handler
therefore decodes HSR.EC and dispatches; it services HVC (EC 0x12) and
reports anything else.  That single funnel is the seam that partitioning
work extends.

Every unhandled vector identifies itself over the console.  The FVP exposes
only an Iris server and no GDB stub, which makes a self-identifying fault
the primary debugging tool for this port.


6.  Floating Point

Floating-point context is saved lazily: only threads that call
tx_thread_vfp_enable() pay for it.  Note that function sets a per-thread
software flag for the context switch; it does not enable the FPU.  Enabling
the hardware is the board support package's responsibility -- CPACR grants
CP10/CP11 access and FPEXC.EN enables execution, both of which reset
disabled.  The example build does this in entry.S under __ARM_FP.

The flag lives in TX_THREAD_EXTENSION_2 and is reached from assembly by the
hard-coded offset 144.  It is defined unconditionally rather than under
TX_ENABLE_VFP_SUPPORT so that the structure layout does not change with the
floating-point build options.  tx_port_offset_check.c asserts that offset,
and the thread stack pointer and run counter offsets, at compile time: a
layout change becomes a build failure instead of silent corruption.


Also worth knowing: this port replaces tx_thread.h's portable lowest-set-bit
search with the CLZ instruction, which is what the scheduler uses to pick the
next thread to run.  Upstream gates that on __TARGET_ARCH_ARM, an Arm Compiler
5 predefine that GCC does not define, so the optimisation had never once been
compiled in under GCC; the guard here asks __ARM_FEATURE_CLZ instead.  It is
worth 40% of the priority search's code size (2188 to 1316 bytes in
tx_thread_system_suspend.o) and it applies in the default 32-priority
configuration, not only above 32.  A Thumb build deliberately keeps the
portable loop, because __ARM_FEATURE_CLZ describes the architecture rather
than the instruction set and Thumb-1 has no CLZ.  demo_clz.elf is the
regression test, and it fails to build rather than silently testing the
portable loop if the CLZ path is ever disabled again.


7.  Memory Protection

PMSAv8-R regions are described by a table rather than a sequence of
register writes, so that a region set can be swapped wholesale.  The
example programs three regions -- code read-only and executable, all
writable memory non-executable, peripherals as Device -- and explicitly
disables the regions it does not use.

Two cautions for anyone reusing this code on silicon:

  - The PRBAR.AP encoding is the standard Armv8-R one, as published:
    AP[2] selects read-only and AP[1] grants EL0 access.  No calibration is
    needed, and an earlier revision of this file said otherwise -- it
    claimed the two bits were reversed, on the strength of a real
    measurement with a wrong cause.  program_region() had been shifting
    every PRBAR field one bit too far left, so the AP value's low bit
    landed in the true AP[2] and writes faulted exactly when that bit was
    set, which looks precisely like a reversed encoding.  The shift is
    fixed; see the comment on the MPU_AP_* macros in mpu.h for the full
    calibration table and what it actually proved.

    The reason it took a silicon run to notice: region coverage is enforced
    even when permissions are not what you asked for, so an unmapped
    address faults while a "read-only" region quietly accepts writes.  A
    configuration-only review cannot tell the two apart -- provoke a real
    fault instead.  Verified on S32Z280 silicon: a write to an RO region
    faults and execution from an XN region takes a prefetch abort, the
    latter never having worked under the old shift.

  - The code and data regions must not share a 64-byte granule.  The
    linker script separates them with ".data ALIGN(64) :" on the output
    section.  An assignment to the location counter between sections does
    not advance a MEMORY region's allocation pointer, and an ALIGN at the
    end of the preceding section does not extend it when no data follows,
    so both of those leave writable data inside the read-only region.


8.  Validation

Functional validation is performed on the Armv8-R AEM FVP through the
images listed in section 3.  Structural coverage is not claimed from this
port: the platform-agnostic regression suite runs on the host, which is
where the coverage instrumentation and the certification evidence live.

The FVP is an architecture envelope model, not a Cortex-R52
implementation -- it reports MIDR 0x410FD0F0, part number 0xD0F.  It
validates architecture; implementation details such as MPU region count,
TCM, lockstep and RAS must be validated on silicon.  Nothing in this port
is gated on MIDR.
