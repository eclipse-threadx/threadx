/***************************************************************************
 * Copyright (c) 2026 Quintauris
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Port Specific                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  PORT SPECIFIC C INFORMATION                            RELEASE        */
/*                                                                        */
/*    tx_port.h                                        RISC-V32/Clang    */
/*                                                           6.4.x        */
/*                                                                        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Francisco Merino, Quintauris                                        */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file contains data type definitions that make the ThreadX      */
/*    real-time kernel function identically on a variety of different     */
/*    processor architectures.  For example, the size or number of bits   */
/*    in an "int" data type vary between microprocessor architectures and */
/*    even C compilers for the same microprocessor.  ThreadX does not     */
/*    directly use native C data types.  Instead, ThreadX creates its     */
/*    own special types that can be mapped to actual data types by this   */
/*    file to guarantee consistency in the interface and functionality.   */
/*                                                                        */
/**************************************************************************/

#ifndef TX_PORT_H
#define TX_PORT_H

/* Include shared RISC-V32 port definitions common to all toolchain ports.  */
#include "../../common/tx_port_riscv32_common.h"


/* Define the version ID of ThreadX.  This may be utilized by the application.  */

#ifndef __ASSEMBLER__
#ifdef TX_THREAD_INIT
CHAR                            _tx_version_id[] =
                                    "(c) 2024 Microsoft Corp. (c) 2026-present Eclipse ThreadX contributors. * ThreadX RISC-V32/GNU Version 6.5.1.202602a *";
#else
extern  CHAR                    _tx_version_id[];
#endif /* TX_THREAD_INIT */
#endif /* __ASSEMBLER__ */

#endif /* TX_PORT_H */
