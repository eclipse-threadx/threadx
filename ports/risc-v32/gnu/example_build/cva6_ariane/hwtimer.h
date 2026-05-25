
/***************************************************************************
 * Copyright (c) 2026 Quintauris
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef RISCV_HWTIMER_H
#define RISCV_HWTIMER_H

#include <stdint.h>

#define TICKNUM_PER_SECOND 	50000000
#define TICKNUM_PER_TIMER	(TICKNUM_PER_SECOND / 100)

int hwtimer_init(void);

int hwtimer_handler(void);

#endif
