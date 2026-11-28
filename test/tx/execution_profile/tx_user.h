/***************************************************************************
 * Copyright (c) 2026-present Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef TX_USER_H
#define TX_USER_H

unsigned long   test_epk_clock_read(void);

#define TX_EXECUTION_PROFILE_ENABLE
#define TX_EXECUTION_TIME_SOURCE         ((EXECUTION_TIME_SOURCE_TYPE) test_epk_clock_read())
#define TX_EXECUTION_MAX_TIME_SOURCE     0xFFFFFFFFUL

#endif
