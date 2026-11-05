/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include "board.h"
#include "rh850_board.h"
#include "rh850_rlin3.h"
#include "test_exit.h"

void board_init(void)
{
	soc_init();
	rh850_rlin3_init(BOARD_CONSOLE_BASE, BOARD_CONSOLE_CLK_HZ, BOARD_CONSOLE_BAUD);
}

void board_putc(char c)
{
	if (c == '\n') {
		rh850_rlin3_putc(BOARD_CONSOLE_BASE, '\r');
	}
	rh850_rlin3_putc(BOARD_CONSOLE_BASE, c);
}

void board_exit(int code)
{
	if (code == 0) {
		test_exit_pass(BOARD_TEST_EXIT_BASE);
	}
	test_exit_fail(BOARD_TEST_EXIT_BASE, (uint16_t)code);
}
