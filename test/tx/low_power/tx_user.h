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

void            test_low_power_timer_setup(unsigned long next_expiration);
void            test_low_power_user_enter(void);
void            test_low_power_user_exit(void);
unsigned long   test_low_power_user_timer_adjust(void);

#define TX_LOW_POWER_TIMER_SETUP(next_expiration)  test_low_power_timer_setup((unsigned long) (next_expiration))
#define TX_LOW_POWER_USER_ENTER                    test_low_power_user_enter()
#define TX_LOW_POWER_USER_EXIT                     test_low_power_user_exit()
#define TX_LOW_POWER_USER_TIMER_ADJUST             test_low_power_user_timer_adjust()

#endif
