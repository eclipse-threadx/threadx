 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
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

/******************************************************************************
 * @file     weak.c
 * @brief    source file for the weak
 * @version  V1.0
 * @date     04. April 2024
 ******************************************************************************/

#include <csi_config.h>
#include <soc.h>
#include <csi_core.h>

__WEAK void soc_dcache_clean_invalid_range(unsigned long addr, uint32_t size)
{
    csi_dcache_clean_invalid_range((unsigned long *)addr, size);
}

__WEAK void soc_dcache_clean_invalid_all(void)
{
    csi_dcache_clean_invalid();
}

__WEAK void soc_dcache_invalid_range(unsigned long addr, uint32_t size)
{
    csi_dcache_invalid_range((unsigned long *)addr, size);
}

__WEAK void soc_dcache_clean(void)
{
    csi_dcache_clean();
}

__WEAK void soc_icache_invalid(void)
{
    csi_icache_invalid();
}

__WEAK unsigned long soc_dma_address_remap(unsigned long addr)
{
    return addr;
}
