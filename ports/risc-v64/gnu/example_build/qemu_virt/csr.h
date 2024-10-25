/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/


#ifndef RISCV_CSR_H
#define RISCV_CSR_H


// Machine Status Register, mstatus
#define MSTATUS_MPP_MASK 	(3L << 11)   // previous mode.
#define MSTATUS_MPP_M 		(3L << 11)
#define MSTATUS_MPP_S 		(1L << 11)
#define MSTATUS_MPP_U 		(0L << 11)
#define MSTATUS_MIE 		(1L << 3)    // machine-mode interrupt enable.
#define MSTATUS_MPIE		(1L << 7)
#define MSTATUS_FS		(1L << 13)

// Machine-mode Interrupt Enable
#define MIE_MTIE		(1L << 7)  
#define MIE_MSIE		(1L << 3)
#define MIE_MEIE		(1L << 11)
#define MIE_STIE 		(1L << 5)  // supervisor timer
#define MIE_SSIE		(1L << 1)
#define MIE_SEIE		(1L << 9)

// Supervisor Status Register, sstatus
#define SSTATUS_SPP 		(1L << 8)  // Previous mode, 1=Supervisor, 0=User
#define SSTATUS_SPIE 		(1L << 5) // Supervisor Previous Interrupt Enable
#define SSTATUS_UPIE 		(1L << 4) // User Previous Interrupt Enable
#define SSTATUS_SIE 		(1L << 1)  // Supervisor Interrupt Enable
#define SSTATUS_UIE 		(1L << 0)  // User Interrupt Enable
#define SSTATUS_SPIE		(1L << 5)
#define SSTATUS_UPIE		(1L << 4)

// Supervisor Interrupt Enable
#define SIE_SEIE 		(1L << 9) // external
#define SIE_STIE 		(1L << 5) // timer
#define SIE_SSIE 		(1L << 1) // software

#ifndef __ASSEMBLER__

#include <stdint.h>

static inline uint64_t riscv_get_core()
{
  uint64_t x;
  asm volatile("csrr %0, mhartid" : "=r" (x) );
  return x;
}

static inline uint64_t riscv_get_mstatus()
{
  uint64_t x;
  asm volatile("csrr %0, mstatus" : "=r" (x) );
  return x;
}

static inline void riscv_writ_mstatus(uint64_t x)
{
  asm volatile("csrw mstatus, %0" : : "r" (x));
}

// machine exception program counter, holds the
// instruction address to which a return from
// exception will go.
static inline void riscv_writ_mepc(uint64_t x)
{
  asm volatile("csrw mepc, %0" : : "r" (x));
}

static inline uint64_t riscv_get_sstatus()
{
  uint64_t x;
  asm volatile("csrr %0, sstatus" : "=r" (x) );
  return x;
}

static inline void riscv_writ_sstatus(uint64_t x)
{
  asm volatile("csrw sstatus, %0" : : "r" (x));
}

// Supervisor Interrupt Pending
static inline uint64_t riscv_get_sip()
{
  uint64_t x;
  asm volatile("csrr %0, sip" : "=r" (x) );
  return x;
}

static inline void riscv_writ_sip(uint64_t x)
{
  asm volatile("csrw sip, %0" : : "r" (x));
}

static inline uint64_t riscv_get_sie()
{
  uint64_t x;
  asm volatile("csrr %0, sie" : "=r" (x) );
  return x;
}

static inline void riscv_writ_sie(uint64_t x)
{
  asm volatile("csrw sie, %0" : : "r" (x));
}

static inline uint64_t riscv_get_mie()
{
  uint64_t x;
  asm volatile("csrr %0, mie" : "=r" (x) );
  return x;
}

static inline void riscv_writ_mie(uint64_t x)
{
  asm volatile("csrw mie, %0" : : "r" (x));
}

// supervisor exception program counter, holds the
// instruction address to which a return from
// exception will go.
static inline void riscv_writ_sepc(uint64_t x)
{
  asm volatile("csrw sepc, %0" : : "r" (x));
}

static inline uint64_t riscv_get_sepc()
{
  uint64_t x;
  asm volatile("csrr %0, sepc" : "=r" (x) );
  return x;
}

// Machine Exception Delegation
static inline uint64_t riscv_get_medeleg()
{
  uint64_t x;
  asm volatile("csrr %0, medeleg" : "=r" (x) );
  return x;
}

static inline void riscv_writ_medeleg(uint64_t x)
{
  asm volatile("csrw medeleg, %0" : : "r" (x));
}

// Machine Interrupt Delegation
static inline uint64_t riscv_get_mideleg()
{
  uint64_t x;
  asm volatile("csrr %0, mideleg" : "=r" (x) );
  return x;
}

static inline void riscv_writ_mideleg(uint64_t x)
{
  asm volatile("csrw mideleg, %0" : : "r" (x));
}

// Supervisor Trap-Vector Base Address
// low two bits are mode.
static inline void riscv_writ_stvec(uint64_t x)
{
  asm volatile("csrw stvec, %0" : : "r" (x));
}

static inline uint64_t riscv_get_stvec()
{
  uint64_t x;
  asm volatile("csrr %0, stvec" : "=r" (x) );
  return x;
}

// Supervisor Timer Comparison Register
static inline uint64_t riscv_get_stimecmp()
{
  uint64_t x;
  // asm volatile("csrr %0, stimecmp" : "=r" (x) );
  asm volatile("csrr %0, 0x14d" : "=r" (x) );
  return x;
}

static inline void riscv_writ_stimecmp(uint64_t x)
{
  // asm volatile("csrw stimecmp, %0" : : "r" (x));
  asm volatile("csrw 0x14d, %0" : : "r" (x));
}

// Machine Environment Configuration Register
static inline uint64_t riscv_get_menvcfg()
{
  uint64_t x;
  // asm volatile("csrr %0, menvcfg" : "=r" (x) );
  asm volatile("csrr %0, 0x30a" : "=r" (x) );
  return x;
}

static inline void riscv_writ_menvcfg(uint64_t x)
{
  // asm volatile("csrw menvcfg, %0" : : "r" (x));
  asm volatile("csrw 0x30a, %0" : : "r" (x));
}

// Physical Memory Protection
static inline void riscv_writ_pmpcfg0(uint64_t x)
{
  asm volatile("csrw pmpcfg0, %0" : : "r" (x));
}

static inline void riscv_writ_pmpaddr0(uint64_t x)
{
  asm volatile("csrw pmpaddr0, %0" : : "r" (x));
}

// supervisor address translation and protection;
// holds the address of the page table.
static inline void riscv_writ_satp(uint64_t x)
{
  asm volatile("csrw satp, %0" : : "r" (x));
}

static inline uint64_t riscv_get_satp()
{
  uint64_t x;
  asm volatile("csrr %0, satp" : "=r" (x) );
  return x;
}

// Supervisor Trap Cause
static inline uint64_t riscv_get_scause()
{
  uint64_t x;
  asm volatile("csrr %0, scause" : "=r" (x) );
  return x;
}

// Supervisor Trap Value
static inline uint64_t riscv_get_stval()
{
  uint64_t x;
  asm volatile("csrr %0, stval" : "=r" (x) );
  return x;
}

// Machine-mode Counter-Enable
static inline void riscv_writ_mcounteren(uint64_t x)
{
  asm volatile("csrw mcounteren, %0" : : "r" (x));
}

static inline uint64_t riscv_get_mcounteren()
{
  uint64_t x;
  asm volatile("csrr %0, mcounteren" : "=r" (x) );
  return x;
}

// machine-mode cycle counter
static inline uint64_t riscv_get_time()
{
  uint64_t x;
  asm volatile("csrr %0, time" : "=r" (x) );
  return x;
}

// enable device interrupts
static inline void riscv_sintr_on()
{
  uint64_t sstatus = riscv_get_sstatus();
  sstatus |= SSTATUS_SIE;
  riscv_writ_sstatus(sstatus);
}

// disable device interrupts
static inline void riscv_sintr_off()
{
  uint64_t sstatus = riscv_get_sstatus();
  sstatus &= (~SSTATUS_SIE);
  riscv_writ_sstatus(sstatus);
}

// are device interrupts enabled?
static inline int riscv_sintr_get()
{
  uint64_t x = riscv_get_sstatus();
  return (x & SSTATUS_SIE) != 0;
}

static inline void riscv_sintr_restore(int x)
{
  if(x)
    riscv_sintr_on();
  else
    riscv_sintr_off();
}

// enable device interrupts
static inline void riscv_mintr_on()
{
  uint64_t mstatus = riscv_get_mstatus();
  mstatus |= MSTATUS_MIE;
  riscv_writ_mstatus(mstatus);
}

// disable device interrupts
static inline void riscv_mintr_off()
{
  uint64_t mstatus = riscv_get_mstatus();
  mstatus &= (~MSTATUS_MIE);
  riscv_writ_mstatus(mstatus);
}

// are device interrupts enabled?
static inline int riscv_mintr_get()
{
  uint64_t x = riscv_get_mstatus();
  return (x & MSTATUS_MIE) != 0;
}

static inline void riscv_mintr_restore(int x)
{
  if(x)
    riscv_mintr_on();
  else
    riscv_mintr_off();
}

static inline uint64_t riscv_get_sp()
{
  uint64_t x;
  asm volatile("mv %0, sp" : "=r" (x) );
  return x;
}

// read and write tp, the thread pointer, which xv6 uses to hold
// this core's hartid (core number), the index into cpus[].
static inline uint64_t riscv_get_tp()
{
  uint64_t x;
  asm volatile("mv %0, tp" : "=r" (x) );
  return x;
}

static inline void riscv_writ_tp(uint64_t x)
{
  asm volatile("mv tp, %0" : : "r" (x));
}

static inline uint64_t riscv_get_ra()
{
  uint64_t x;
  asm volatile("mv %0, ra" : "=r" (x) );
  return x;
}

// flush the TLB.
static inline void sfence_vma()
{
  // the zero, zero means flush all TLB entries.
  asm volatile("sfence.vma zero, zero");
}

#endif // __ASSEMBLER__

#endif
