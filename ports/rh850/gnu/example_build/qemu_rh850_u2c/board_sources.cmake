# Copyright (c) 2026 Eclipse ThreadX contributors
# SPDX-License-Identifier: MIT

set(RH850_U2C_DIR ${CMAKE_CURRENT_LIST_DIR})
set(RH850_U2C_SOURCES
    ${RH850_U2C_DIR}/crt0.S
    ${RH850_U2C_DIR}/vectors.S
    ${RH850_U2C_DIR}/irq_entry.S
    ${RH850_U2C_DIR}/fault_entry.S
    ${RH850_U2C_DIR}/fault.c
    ${RH850_U2C_DIR}/libc_glue.c
    ${RH850_U2C_DIR}/soc.c
    ${RH850_U2C_DIR}/board.c
    ${RH850_U2C_DIR}/rh850_intc.c
    ${RH850_U2C_DIR}/rh850_ostm.c
    ${RH850_U2C_DIR}/rh850_rlin3.c
    ${RH850_U2C_DIR}/test_exit.c
    ${RH850_U2C_DIR}/board_threadx.c
)
set(RH850_U2C_DEFINITIONS RH850_INTC_HANDLERS=8)
set(RH850_U2C_LINK_OPTIONS
    -T${RH850_U2C_DIR}/rh850.ld
    -L${RH850_U2C_DIR}
    -nostartfiles
    -Wl,--gc-sections
)
set(RH850_U2C_QEMU_ARGS -M rh850-u2c,test-exit=on -icount shift=4,sleep=on -nographic)
