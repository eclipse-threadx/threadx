/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include <stdio.h>
#include "tx_api.h"
#include "tx_thread.h"
#include "rh850_board.h"
#include "test_common.h"

extern char stack_top[] __asm__("__stack_top");
extern char stack_size[] __asm__("__stack_size");

#define SYS_FILL 0x5aa5c33cUL
#define SYS_CANARY 0xc0deca11UL
#define CANARY_EXIT 131

uintptr_t test_sys_stack_bottom(void)
{
	return (uintptr_t)stack_top - (uintptr_t)stack_size;
}

uintptr_t test_sys_stack_top(void)
{
	return (uintptr_t)stack_top;
}

uintptr_t test_sp(void)
{
	uintptr_t sp;

	__asm__ volatile("mov sp, %0" : "=r"(sp));
	return sp;
}

void test_spin(unsigned int n)
{
	for (volatile unsigned int i = 0; i < n; i++) {
	}
}

static void stack_error(TX_THREAD *thread_ptr)
{
	printf("stack FAIL: %s overflow\n", thread_ptr->tx_thread_name);
	board_exit(130);
}

void test_stack_init(void)
{
	ULONG *p = (ULONG *)test_sys_stack_bottom();
	ULONG *end = (ULONG *)(test_sp() - 32U);

	*p++ = SYS_CANARY;
	while (p < end) {
		*p++ = SYS_FILL;
	}
	tx_thread_stack_error_notify(stack_error);
}

static ULONG free_bytes(const ULONG *p, const ULONG *end, ULONG fill)
{
	const ULONG *start = p;

	while (p < end && *p == fill) {
		p++;
	}
	return (ULONG)(p - start) * sizeof(ULONG);
}

void test_stack_report(void)
{
	TX_THREAD *thread_ptr = _tx_thread_created_ptr;

	if (*(const ULONG *)test_sys_stack_bottom() != SYS_CANARY) {
		printf("stack FAIL: system stack canary overwritten\n");
		board_exit(CANARY_EXIT);
	}

	for (ULONG i = 0; i < _tx_thread_created_count; i++) {
		printf("stack: %-10s %5lu of %5lu bytes free\n", thread_ptr->tx_thread_name,
		       free_bytes(thread_ptr->tx_thread_stack_start, thread_ptr->tx_thread_stack_end,
				  TX_STACK_FILL),
		       thread_ptr->tx_thread_stack_size);
		thread_ptr = thread_ptr->tx_thread_created_next;
	}
	printf("stack: %-10s %5lu of %5lu bytes free\n", "system",
	       free_bytes((const ULONG *)test_sys_stack_bottom() + 1, (const ULONG *)test_sys_stack_top(),
			  SYS_FILL),
	       (ULONG)(uintptr_t)stack_size);
}
