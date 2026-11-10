/***************************************************************************
 * Copyright (c) 2026 safecubed GmbH
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* Portions of this file were generated with AI assistance. */

/* iLLD trap hook configuration. IfxCpu_Trap.h includes this file when
   Ifx_Cfg.h defines IFX_CFG_EXTEND_TRAP_HOOKS.  */

#ifndef IFX_CFG_TRAP_H
#define IFX_CFG_TRAP_H

/* ThreadX system call handler, defined in tx_thread_schedule.c of the port.
   The types match VOID and UCHAR in tx_port.h.  */
void _tx_thread_syscall_handler(unsigned char id);

/* The ThreadX port enters the scheduler through a system call on CPU0. The
   hook shall call _tx_thread_syscall_handler directly: the port's context
   save walks a fixed number of CSA links back from the handler
   (TX_PORT_TRICORE_SYSCALL_CALL_DEPTH in src/tx_port_tricore.h).  */
#define IFX_CFG_CPU_TRAP_SYSCALL_CPU0_HOOK(trapWatch)   _tx_thread_syscall_handler((trapWatch).tId)

#endif /* IFX_CFG_TRAP_H */
