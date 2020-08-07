/* ------------------------------------------
 * Copyright (c) 2016, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 --------------------------------------------- */

#include "arc/arc_reg.h"

/*********************************************************************
 * Core Intc setup
 *********************************************************************/

#ifndef __ARC_HDR__
#define __ARC_HDR__

#ifndef NULL
#define NULL 0
#endif

#define IRQ_TIMER	16
#define IRQ_IPI		19

#define CORE_PRIMARY   1
#define CORE_SECONDARY 2

#define INTERRUPT_ENABLE (1 << 4) // according ISA (SETI instruction details)
#define INTERRUPT_LEVEL(L) ((L) << 0)//simple macro for user-friendly name conversion



static inline int __ffs(unsigned long x)
{
	int n;

	asm volatile(
	"	ffs.f	%0, %1		\n"  /* 0..31; Z set if src 0 */
	"	mov.z	%0, 0		\n"  /* return 0 if 0 */
	: "=r"(n)
	: "r"(x)
	: "cc");

	return n;
}

#define AUX_ID		0x004

static inline int smp_processor_id()
{
	unsigned int id = _lr(AUX_ID);
	return (id >> 8) & 0xFF;
}

static inline void arc_halt(void)
{
  asm volatile("flag 1\n");
}

/* no need for volatile */
typedef unsigned int spinlock_t;

static inline void spinlock_acquire(unsigned int *lock)
{
	unsigned int val;

	asm volatile(
	"1:	llock	%[val], [%[slock]]	\n"
	"	breq	%[val], %[LOCKED], 1b	\n"	/* spin while LOCKED */
	"	scond	%[LOCKED], [%[slock]]	\n"	/* acquire */
	"	bnz	1b			\n"
	"	dmb 3				\n"
	: [val]		"=&r"	(val)
	: [slock]	"r"	(lock),
	  [LOCKED]	"r"	(1)
	: "memory", "cc");
}

static inline void spinlock_release(unsigned int *lock)
{
	__asm__ __volatile__(
	"	dmb 3				\n"
	"	st	%[UNLOCKED], [%[slock]]	\n"
	:
	: [slock]	"r"	(lock),
	  [UNLOCKED]	"r"	(0)
	: "memory");
}

static inline void arc_enable_ints(void)
{
  _seti(1);
}

extern void arc_timer_setup(unsigned int);
extern void arc_register_isr(int irq, void (*fn)(int), int arg_to_isr);
extern void arc_ici_send(unsigned int cpu);

#endif
