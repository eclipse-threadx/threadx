#!/bin/bash
##############################################################################
# Copyright (c) 2026 safecubed GmbH
# Copyright (c) 2026 Eclipse ThreadX contributors
#
# This program and the accompanying materials are made available under the
# terms of the MIT License which is available at
# https://opensource.org/licenses/MIT.
#
# SPDX-License-Identifier: MIT
##############################################################################

# Portions of this file were generated with AI assistance.

# Builds sample_threadx.elf for the AURIX TC375 Lite Kit. build_threadx.sh
# shall have built tx.a first. See build_env.sh for the required environment.

set -e
cd "$(dirname "$0")"
. ./build_env.sh

OBJ_DIR="${BUILD_DIR}/sample"
rm -rf "${OBJ_DIR}"
mkdir -p "${OBJ_DIR}"
rm -f sample_threadx.elf sample_threadx.map

SOURCES="sample_threadx.c \
tx_initialize_low_level.c \
Cpu0_Main.c \
Cpu1_Main.c \
Cpu2_Main.c \
Ifx_Cfg_Ssw.c \
Ifx_Cfg_SswBmhd.c \
${INFRA}/Platform/Tricore/Compilers/CompilerGnuc.c \
${SSW}/Ifx_Ssw_Infra.c \
${SSW}/Ifx_Ssw_Tc0.c \
${SSW}/Ifx_Ssw_Tc1.c \
${SSW}/Ifx_Ssw_Tc2.c \
${ILLD}/Cpu/Std/IfxCpu.c \
${ILLD}/Cpu/Irq/IfxCpu_Irq.c \
${ILLD}/Cpu/Trap/IfxCpu_Trap.c \
${ILLD}/Pms/Std/IfxPmsEvr.c \
${ILLD}/Scu/Std/IfxScuCcu.c \
${ILLD}/Scu/Std/IfxScuWdt.c \
${ILLD}/Src/Std/IfxSrc.c \
${ILLD}/Stm/Std/IfxStm.c \
${ILLD}/_Impl/IfxCpu_cfg.c \
${ILLD}/_Impl/IfxStm_cfg.c"

for src in ${SOURCES}; do
    obj="${OBJ_DIR}/$(basename "${src%.c}").o"
    "${CC}" ${CFLAGS} ${INCLUDES} -c "${src}" -o "${obj}"
done

"${CC}" -mtc162 -nocrt0 -T Lcf_Gnuc_Tricore_Tc.lsl -Wl,--gc-sections \
    -Wl,-Map=sample_threadx.map \
    -o sample_threadx.elf "${OBJ_DIR}"/*.o tx.a
