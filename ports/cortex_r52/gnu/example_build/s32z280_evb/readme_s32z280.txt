Cortex-R52 example build for the NXP S32Z280-594EVB
===================================================

RTU0 core 0 (R52_0_0) on the S32Z280-594EVB daughtercard, reached through an
S32 Debug Probe.  This is silicon bring-up: the FVP example under
example_build/fvp_baser_aemv8r validates the architecture, and this one
validates the implementation.  MIDR here is 0x411FD133 (Cortex-R52 r1p3);
the FVP reports 0x410FD0F0, part 0xD0F, which is an architecture envelope
model and not an R52 at all.


1.  Build

        cmake -B build_s32z -G Ninja \
              -DCMAKE_TOOLCHAIN_FILE=cmake/cortex_r52.cmake \
              -DTX_R52_BUILD_S32Z280_EXAMPLE=ON .
        ninja -C build_s32z s32z280_boot.elf

    The option is separate from TX_R52_BUILD_FVP_EXAMPLE so that FVP
    regression stays green regardless of what bring-up is doing, and so the
    two boards' differing reset state and memory maps cannot interact.


2.  Run

        S32Z280_ELF=build_s32z/ports/cortex_r52/gnu/example_build/s32z280_evb/s32z280_boot.elf \
            arm-none-eabi-gdb-py -batch -x tools/read_identity.gdb

    The host side is not optional and not obvious; see setup.md section 9.  In
    short: a CCS must be listening on 41475 and it must be the *Windows* CCS,
    because its code pushes to the probe complete while the Linux CCS's stall
    after "Sending code ... done".  Under WSL2 mirrored networking both share
    one port namespace, so a stray Linux CCS silently blocks the Windows one.
    GTA runs on 45000, and arm-none-eabi-gdb-py needs a source-built Python
    3.10.11 in PYTHONHOME.


3.  Three silicon facts this build exists to encode

    a.  The core resets in THUMB state.  CPSR reads 0x1FA out of reset -- Hyp
        mode with T set -- because the RTU boot instruction NXP plants at the
        boot address is a T32 branch.  _start is therefore T32 and switches to
        A32 itself.  An A32 entry would execute the first halfword of its own
        instruction as Thumb and end up anywhere.

    b.  The debugger holds every core in debug state.  NXP's
        _reset_to_first_instruction() asserts MDM_AP CONTROL2[19:16] =
        CR52_RTU0_{3,2,1,0}_EDBGREQ and nothing ever clears them, despite the
        comment in NXP's own source claiming start_debug_by_core_name() does.
        While EDBGREQ is asserted the core cannot execute at all, yet registers
        and memory still respond and MC_ME/RGM report the core released and
        clocked -- so it presents as working hardware that ignores every
        attempt to run.  tools/read_identity.gdb clears core 0's bit and
        verifies the clear took.

    c.  CNTFRQ reads zero, the same as on the FVP.  It is writable only at the
        highest implemented exception level, so it must be programmed at EL2.
        entry.S deliberately records it rather than writing a value, because
        the correct frequency for this board is not yet established and a wrong
        one would silently mis-scale every interval derived from it.  Any timer
        work here must program CNTFRQ first.


4.  Memory map

    Code SRAM is visible through two windows onto the same physical memory:
    0x79900000 for instruction fetch, 0x32100000 as data.  The core resets to
    0x79900000 -- MC_ME_PRTN0_CORE0_ADDR reads exactly that -- so .text is
    linked and loaded there.  That window is writable over the debug AXI port
    even though NXP's debugger memory map declares it read-only, so a
    debugger-loaded image needs no alias.  A flash-booted image would.

    Data, bss and the per-mode stacks live in RTU-local data SRAM at
    0x31780000 (256 KB); 0x31800000 gives 2 MB if an image outgrows it.

    Reads of 0x30000000/0x30100000/0x30200000 do fault, but the earlier note
    here had the reason wrong.  Reading the region registers on this part shows
    ATCM already ENABLED at both exception levels, 64 KB with one wait state,
    and every BASEADDRESS field zero -- so ATCM is live at address 0x00000000,
    not at 0x30000000.  The faulting reads were of an address the TCM is not
    at.  ATCM's enables reset set because CFGTCMBOOTx is tied high here.  BTCM
    (16 KB, 0 wait states) and CTCM (16 KB, 1 wait state) really are disabled.
    Sizes and wait states agree with the S32Z2 reference manual exactly.

    ECC is implemented and enabled -- IMP_MEMPROTCTLR reads 0x11 -- so per
    Cortex-R52 TRM 6.2.2 a TCM location must be written before it is read, and
    ATCM needs 64-bit aligned stores where BTCM and CTCM accept 32-bit.  Using
    TCM therefore takes more than setting an enable bit.

    entry.S now programs ATCM to 0x30000000 and enables it at both exception
    levels, at EL2 -- writing ENABLEEL2 from EL1 is silently ignored, measured on
    two banks.  The boot image preloads it (ECC check bits are not initialised by
    the core) and proves it holds data both before and after the MPU is enabled.

    BTCM and CTCM are left disabled on purpose, and that is a measurement rather
    than caution.  Enabling any second bank removes all measurable data-cache
    benefit on this part:

        ATCM only                      cache gain 24%
        ATCM + BTCM                    cache gain  0%
        ATCM + BTCM + CTCM             cache gain  0%
        ATCM + BTCM at another base    cache gain  0%
        ATCM + CTCM, BTCM disabled     cache gain  0%

    Five configurations, one variable.  Not a particular bank, not its address,
    not the ECC preload -- enabling a second bank at all.  The benchmark buffer is
    in non-RTU-local SRAM at 0x31800000, outside every TCM window, and CCSIDR
    reports the same 16 KB four-way cache throughout.

    No erratum covers this.  Checked SDEN-857344 issue 19 (all 25 Cortex-R52
    entries) and the S32Z2 0P91J mask set errata, whose RTU and R52 entries are
    ERR050509, ERR051107, ERR051153, ERR051441, ERR051613, ERR051614 and
    ERR052126.  A RAM pool shared between the RTU's last-level cache and the TCMs
    would explain it -- the LLC "allocates ways to specific domains" -- but that is
    a guess for NXP to confirm, and worth raising with the other RTU questions.

    Losing nothing by this: the reference manual describes TCM_A as the bank
    "optimized for small, regularly executed code such as interrupt service
    routines or OS kernels", which is what a TCM is wanted for here.  Enabling the
    other two is one line each in entry.S when there is an answer.

    The DDR window at 0x7A000000 is dark until DDR is initialised.  TCM is still
    absent from link.lds: nothing is placed there yet.


5.  What this image reports

    boot_stage says how far the reset path got, so a fault is distinguishable
    from a hang; if a fault handler ran it records the vector, syndrome and
    faulting address before parking.  r52_identity.magic is written last, so a
    half-filled structure cannot be mistaken for a complete one.  bsp_done() is
    a deliberate breakpoint target, so "the image finished" is observed rather
    than inferred from a timeout.

    Verified on the board:

        MIDR      0x411FD133   Cortex-R52 r1p3
        MPUIR     0x00001400   20 MPU regions at EL1
        HMPUIR    0x00000014   EL2; the count field sits in bits [7:0] here
                               rather than [15:8] as in MPUIR, which still
                               needs confirming against the Armv8-R spec
        CTR       0x8144C004
        MPIDR     0x80000000
        ID_PFR0   0x00000131
        ID_PFR1   0x10111001   virtualisation field 1: EL2 implemented
        SCTLR     0x70C50838   MPU, D-cache and I-cache all off at reset
        CNTFRQ    0x00000000

        CPSR at EL2  0x000001DA  mode 0x1A, Hyp
        CPSR at EL1  0x600001D3  mode 0x13, Supervisor

    The two CPSR values are the point of the milestone: they are the EL2 to EL1
    drop performed by this code on silicon rather than on a model.


6.  Not here yet

    No console.  LIN9 reaches the host through the daughtercard USB-UART
    (jumper J248), which is LINFlex_9 at 0x42980000 -- not LINFlex_0 at
    0x40170000, the tempting wrong choice as the first instance in the
    Reference Manual's list.  What is missing is the clock configuration needed
    to compute a baud rate; a console at the wrong rate produces garbage
    indistinguishable from a crash, which is why this milestone reports through
    memory instead.

    No timer, no GIC, no MPU programming and no ThreadX: this image does not
    link the kernel, so a failure here is unambiguously a boot or board problem
    rather than a kernel one.
