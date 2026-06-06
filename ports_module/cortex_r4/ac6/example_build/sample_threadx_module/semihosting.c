/***************************************************************************/
/* Copyright (c) 2024 Microsoft Corporation                                */
/* Copyright (c) 2026 Eclipse ThreadX contributors                         */
/*                                                                         */
/* This program and the accompanying materials are made available under    */
/* the terms of the MIT License which is available at                      */
/* https://opensource.org/licenses/MIT.                                    */
/*                                                                         */
/* SPDX-License-Identifier: MIT                                            */
/***************************************************************************/

/* Disable semihosting */
#include <rt_sys.h>

#if (__ARMCC_VERSION < 6000000)
#pragma import(__use_no_semihosting)
#else
void use_no_semihosting(void)
{
	__asm(".global __use_no_semihosting\n\t");
}
#endif

char *$Sub$$_sys_command_string(char *cmd, int len)
{
	return 0;
}

__attribute__((noreturn)) void $Sub$$_sys_exit(int return_code)
{
	while(1);
}

__attribute__((noreturn)) int $Sub$$__raise(int signal, int type)
{
	while(1);
}

