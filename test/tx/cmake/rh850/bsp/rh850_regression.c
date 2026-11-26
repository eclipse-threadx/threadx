/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include <stdint.h>
#include <time.h>
#include "board.h"
#include "rh850_board.h"
#include "rh850_cpu.h"
#include "rh850_ostm.h"

void external_exit(unsigned int code);
/* ld applies --wrap to the C name; the "_" prefix of GCC v850 follows */
void __real_main(void);
int __wrap_main(void);

void external_exit(unsigned int code)
{
	board_exit((int)code);
}

time_t time(time_t *timer)
{
	static uint32_t last, high;
	uint32_t psw = rh850_irq_save();
	uint32_t now = rh850_ostm_count(BOARD_TIMER2_OSTM_BASE);

	if (now < last) {
		high++;
	}
	last = now;
	uint64_t clocks = ((uint64_t)high << 32) | now;
	rh850_irq_restore(psw);

	time_t seconds = (time_t)(clocks / BOARD_TIMER_CLK_HZ);

	if (timer != NULL) {
		*timer = seconds;
	}
	return seconds;
}

int __wrap_main(void)
{
	board_init();
	rh850_ostm_start_freerun(BOARD_TIMER2_OSTM_BASE, false);
	__real_main();
	return 1;
}
