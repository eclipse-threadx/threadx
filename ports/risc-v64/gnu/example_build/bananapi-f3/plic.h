/***************************************************************************
 * Copyright (c) 2025 10xEngineers
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

/*
 * SpacemiT K1 PLIC - SiFive-compatible Platform-Level Interrupt Controller.
 * 
 *  From bananapi-f3.dts (linux main tree): 
 *  interrupt-controller@e0000000 {
 *			reg-names = "control";
 *			interrupts-extended = <0x10 0x0b 0x10 0x09 0x11 0x0b 0x11 0x09 0x12 0x0b 0x12 0x09 0x13 0x0b 0x13 0x09 0x14 0x0b 0x14 0x09 0x15 0x0b 0x15 0x09 0x16 0x0b 0x16 0x09 0x17 0x0b 0x17 0x09>;
 *			compatible = "riscv,plic0";
 *			#interrupt-cells = <0x01>;
 *			reg = <0x00 0xe0000000 0x00 0x4000000>;
 *			phandle = <0x1e>;
 *			riscv,ndev = <0x9f>;         // (159 external interrupt sources)
 *			riscv,max-priority = <0x07>; // (priority levels 1..7; 0 = disabled)
 *			interrupt-controller;
 *		};

 * The RISCV_APB block is at 0xE000_0000 with size 0x1000_0000.
 * Within that block the PLIC occupies 0xE000_0000..0xE3FF_FFFF
 * (64 MiB) and the CLINT lives at 0xE400_0000.
 *
 * The K1 main CPU has up to 8 X60 cores (harts 0..7).  Each hart
 * exposes both an M-mode and S-mode interrupt context to the PLIC.
 *
 * PLIC register layout:
 *
 *   Offset          Size    Description
 *   ──────────────  ──────  ────────────────────────────────────────────
 *   0x000000        4/src   Source priority  (src 0 reserved, 1..159)
 *   0x001000        20B     Pending bits     (5 × 32-bit words)
 *   0x002000        0x100/h Enable bits per hart (M-mode at ctx 0)
 *   0x002080        0x100/h Enable bits per hart (S-mode at ctx 1)
 *   0x200000        0x2000/h Threshold + Claim/Complete (M-mode ctx 0)
 *   0x201000        0x2000/h Threshold + Claim/Complete (S-mode ctx 1)
 *
 * Context mapping:
 *   EN_PER_HART     = 0x100    (two contexts × 0x80 each)
 *   EN_PER_CONTEXT  = 0x80
 *   THRES_PER_HART  = 0x2000   (two contexts × 0x1000 each)
 *   THRES_PER_CTX   = 0x1000
 *   THRES_CLAIM_OFF = 0x4      (claim/complete at threshold + 4)
 *
 */

#define PLIC                    0xE0000000UL
#define PLIC_SIZE               0x04000000UL   /* 64 MiB, from DTS reg */

#define PLIC_PRIORITY           (PLIC + 0x0)
#define PLIC_PRIO_PER_ID        4              /* stride: 4 bytes/source */

#define PLIC_EN                 (PLIC + 0x2000)
#define PLIC_EN_PER_HART        0x100
#define PLIC_EN_PER_CONTEXT     0x80


#define PLIC_SENABLE(hart)      (PLIC_EN + (hart) * PLIC_EN_PER_HART + PLIC_EN_PER_CONTEXT)

#define PLIC_THRES              (PLIC + 0x200000)
#define PLIC_THRES_PER_HART     0x2000
#define PLIC_THRES_PER_CONTEXT  0x1000
#define PLIC_THRES_CLAIM_OFF    0x4

/*
 * S-mode threshold/claim = M-mode base + one context offset (0x1000).
 */
#define PLIC_SPRIORITY(hart)    (PLIC_THRES + (hart) * PLIC_THRES_PER_HART + PLIC_THRES_PER_CONTEXT)
#define PLIC_SCLAIM(hart)       (PLIC_SPRIORITY(hart) + PLIC_THRES_CLAIM_OFF)
#define PLIC_SCOMPLETE(hart)    PLIC_SCLAIM(hart)

#define PLIC_GET_PRIO(irqno)            (*(volatile uint32_t *)(PLIC_PRIORITY + (irqno) * PLIC_PRIO_PER_ID))
#define PLIC_SET_PRIO(irqno, prio)      (*(volatile uint32_t *)(PLIC_PRIORITY + (irqno) * PLIC_PRIO_PER_ID) = (prio))


#define PLIC_NUM_SOURCES        159
#define PLIC_MAX_PRIORITY       7
#define PLIC_DEFAULT_PRIORITY   2            
#define MAX_CALLBACK_NUM        (PLIC_NUM_SOURCES + 1) 

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

#endif /* RISCV_PLIC_H */
