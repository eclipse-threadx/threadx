/*
 * Copyright (c) 2016-2022 Arm Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __REGION_DEFS_H__
#define __REGION_DEFS_H__

#include "region_limits.h"

/* **************************************************************
 * WARNING: this file is parsed both by the C/C++ compiler
 * and the linker. As a result the syntax must be valid not only
 * for C/C++ but for the linker scripts too.
 * Beware of the following limitations:
 *   - LD (GCC linker) requires white space around operators.
 *   - UL postfix for macros is not suported by the linker script
 ****************************************************************/

/* Secure regions */
#define S_CODE_START     ( S_ROM_ALIAS )
#define S_CODE_SIZE      ( TOTAL_S_ROM_SIZE )
#define S_CODE_LIMIT     ( S_CODE_START + S_CODE_SIZE )

#define S_DATA_START     ( S_RAM_ALIAS )
#define S_DATA_SIZE      ( TOTAL_S_RAM_SIZE )
#define S_DATA_LIMIT     ( S_DATA_START + S_DATA_SIZE )

#define S_DDR4_START     ( S_DDR4_ALIAS )
#define S_DDR4_SIZE      ( TOTAL_S_DDR4_SIZE )
#define S_DDR4_LIMIT     ( S_DDR4_START + S_DDR4_SIZE )

#endif /* __REGION_DEFS_H__ */
