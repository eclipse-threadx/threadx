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
#include "rh850_board.h"
#include "test_common.h"

#define SECONDS  10U
#define COUNTERS 10U
#define STACK    1024U

int demo_main(void);
void demo_application_define(void *first_unused_memory);

extern ULONG thread_0_counter, thread_1_counter, thread_1_messages_sent, thread_2_counter,
	thread_2_messages_received, thread_3_counter, thread_4_counter, thread_5_counter,
	thread_6_counter, thread_7_counter;

static volatile ULONG *const counter[COUNTERS] = {
	&thread_0_counter, &thread_1_counter, &thread_1_messages_sent, &thread_2_counter,
	&thread_2_messages_received, &thread_3_counter, &thread_4_counter, &thread_5_counter,
	&thread_6_counter, &thread_7_counter,
};
static const char *const counter_name[COUNTERS] = {
	"thread_0", "thread_1", "sent", "thread_2", "received",
	"thread_3", "thread_4", "thread_5", "thread_6", "thread_7",
};

static TX_THREAD monitor;
static ULONG monitor_stack[STACK / sizeof(ULONG)];

static void monitor_thread(ULONG arg)
{
	ULONG last[COUNTERS] = { 0 };

	(void)arg;
	for (ULONG s = 0; s < SECONDS; s++) {
		tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND);
		for (ULONG i = 0; i < COUNTERS; i++) {
			ULONG c = *counter[i];

			if (c == last[i]) {
				printf("demo_threadx FAIL: %s stalled at %lu after %lu s\n",
				       counter_name[i], c, s + 1U);
				board_exit(240 + (int)i);
			}
			last[i] = c;
		}
	}
	printf("demo_threadx: after %u s:", SECONDS);
	for (ULONG i = 0; i < COUNTERS; i++) {
		printf(" %s %lu", counter_name[i], last[i]);
	}
	printf("\n");
	test_stack_report();
	printf("demo_threadx pass\n");
	board_exit(0);
}

void tx_application_define(void *first_unused_memory)
{
	test_stack_init();
	demo_application_define(first_unused_memory);
	tx_thread_create(&monitor, "monitor", monitor_thread, 0, monitor_stack, STACK, 0, 0,
			 TX_NO_TIME_SLICE, TX_AUTO_START);
}

int main(void)
{
	board_init();
	return demo_main();
}
