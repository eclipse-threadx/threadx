/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/


#ifndef RISCV_PLIC_H
#define RISCV_PLIC_H

#include "csr.h"
#include <stdint.h>

#define PLIC    			0x0c000000L
#define PLIC_PRIORITY 			(PLIC + 0x0)
#define PLIC_PENDING 			(PLIC + 0x1000)
#define PLIC_MENABLE(hart) 		(PLIC + 0x2000 + (hart)*0x100)
#define PLIC_SENABLE(hart) 		(PLIC + 0x2080 + (hart)*0x100)
#define PLIC_MPRIORITY(hart) 		(PLIC + 0x200000 + (hart)*0x2000)
#define PLIC_SPRIORITY(hart) 		(PLIC + 0x201000 + (hart)*0x2000)
#define PLIC_MCLAIM(hart) 		(PLIC + 0x200004 + (hart)*0x2000)
#define PLIC_SCLAIM(hart) 		(PLIC + 0x201004 + (hart)*0x2000)
#define PLIC_MCOMPLETE(hart)            (PLIC + 0x200004 + (hart)*0x2000)
#define PLIC_SCOMPLETE(hart)            (PLIC + 0x201004 + (hart)*0x2000)


#define PLIC_GET_PRIO(irqno)   		(*(uint32_t *)(PLIC_PRIORITY + (irqno)*4))
#define PLIC_SET_PRIO(irqno, prio) 	(*(uint32_t *)(PLIC_PRIORITY + (irqno)*4) = (prio))

#define MAX_CALLBACK_NUM		128
typedef int (*irq_callback)(int irqno);

void plic_irq_enable(int irqno);
void plic_irq_disable(int irqno);
int  plic_prio_get(int irqno);
void plic_prio_set(int irqno, int prio);
int  plic_register_callback(int irqno, irq_callback callback);
int  plic_unregister_callback(int irqno);
int  plic_init(void);
int  plic_claim(void);
void plic_complete(int irqno);

int  plic_irq_intr(void);

#endif

