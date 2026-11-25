/***************************************************************************
 * Copyright (c) 2026 Quintauris
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include "plic.h"
#include "hwtimer.h"
#include "uart.h"
#include <stdint.h>
#include <stddef.h>

void *memset(void *des, int c,size_t n)
{
    if((des == NULL) || n <=0)
            return (void*)des;
    char* t = (char*)des;
    int i;
    for(i=0;i<n;i++)
        t[i]=c;
    return t;
}


int board_init(void)
{
	int ret;
	ret = plic_init();
	if(ret)
		return ret;
	ret = uart_init();
	if(ret)
		return ret;
	ret = hwtimer_init();
	if(ret)
		return ret;
	return 0;
}
