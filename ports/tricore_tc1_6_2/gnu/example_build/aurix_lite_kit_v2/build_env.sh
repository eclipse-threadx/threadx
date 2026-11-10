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

# Common settings for build_threadx.sh and build_threadx_sample.sh.
#
# The TriCore TC1.6.2 port and the sample depend on the Infineon iLLD. The iLLD is
# not part of this repository. AURIX_ILLD_DIR shall name the directory that
# holds the iLLD, Infra and Service folders. Two layouts are accepted:
#
#   - the iLLD release package for all TC3xx devices (iLLD/TC3xx), for
#     example src/BaseSw of iLLD 1.22.0. Ifx_Cfg.h selects the device.
#   - the TC37A iLLD of an AURIX Development Studio project (iLLD/TC37A),
#     that is the project's Libraries folder.
#
#     AURIX_ILLD_DIR=/path/to/illd_release_tc3x/src/BaseSw ./build_threadx_sample.sh
#
# CROSS_COMPILE selects the toolchain prefix. The default is the AURIX GCC
# (tricore-elf-), which shall be on PATH.

: "${CROSS_COMPILE:=tricore-elf-}"
CC="${CROSS_COMPILE}gcc"
AR="${CROSS_COMPILE}ar"

THREADX_ROOT="../../../../.."
BUILD_DIR="build"
INFRA="${AURIX_ILLD_DIR:-}/Infra"
SERVICE="${AURIX_ILLD_DIR:-}/Service/CpuGeneric"

if [ -d "${AURIX_ILLD_DIR:-}/iLLD/TC3xx/Tricore" ]; then
    ILLD="${AURIX_ILLD_DIR}/iLLD/TC3xx/Tricore"
    SFR="${INFRA}/Sfr/TC37x"
    SSW="${INFRA}/Ssw/TC3xx/Tricore"
    PINMAP="${ILLD}/_PinMap/TC37x"
elif [ -d "${AURIX_ILLD_DIR:-}/iLLD/TC37A/Tricore" ]; then
    ILLD="${AURIX_ILLD_DIR}/iLLD/TC37A/Tricore"
    SFR="${INFRA}/Sfr/TC37A/_Reg"
    SSW="${INFRA}/Ssw/TC37A/Tricore"
    PINMAP="${ILLD}/_PinMap"
else
    echo "AURIX_ILLD_DIR shall name a directory that contains iLLD/TC3xx/Tricore" >&2
    echo "or iLLD/TC37A/Tricore, together with Infra and Service." >&2
    exit 1
fi

CFLAGS="-std=c99 -O0 -g3 -Wall -fmessage-length=0 -fno-common \
-fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162"

INCLUDES="-I. \
-I${THREADX_ROOT}/common/inc \
-I../../inc \
-I${ILLD} \
-I${ILLD}/Cpu/Std \
-I${ILLD}/Cpu/Irq \
-I${ILLD}/Cpu/Trap \
-I${ILLD}/Scu/Std \
-I${ILLD}/Src/Std \
-I${ILLD}/Stm/Std \
-I${ILLD}/Port/Std \
-I${ILLD}/Pms/Std \
-I${ILLD}/_Impl \
-I${ILLD}/_Lib \
-I${ILLD}/_PinMap \
-I${PINMAP} \
-I${INFRA}/Platform \
-I${INFRA}/Platform/Tricore/Compilers \
-I${SFR} \
-I${SSW} \
-I${SERVICE} \
-I${SERVICE}/SysSe/Bsp \
-I${SERVICE}/_Utilities"
