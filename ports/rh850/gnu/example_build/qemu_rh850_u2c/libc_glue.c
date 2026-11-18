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
#include <unistd.h>
#include "rh850_board.h"

static int rh850_stdio_putc(char c, FILE *file)
{
	(void)file;
	board_putc(c);
	return (unsigned char)c;
}

static FILE rh850_stdio = FDEV_SETUP_STREAM(rh850_stdio_putc, NULL, NULL,
					     _FDEV_SETUP_WRITE);

FILE *const stdout = &rh850_stdio;
__strong_reference(stdout, stderr);

void _exit(int code)
{
	board_exit(code);
}
