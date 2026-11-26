/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

MEMORY
{
	FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 8M
	RAM   (rwx) : ORIGIN = 0xfdc00000, LENGTH = 128K
	CRAM  (rwx) : ORIGIN = 0xfe000000, LENGTH = 0x138000
}

REGION_ALIAS("RAM_LARGE", CRAM);
