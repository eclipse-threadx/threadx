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

#include <csi_config.h>
#include <stdbool.h>
#include <csi_core.h>
#include <soc.h>

void soc_irq_enable(uint32_t irq_num)
{
    csi_vic_enable_irq((int32_t)irq_num);
}

void soc_irq_disable(uint32_t irq_num)
{
    csi_vic_disable_irq((int32_t)irq_num);
}

bool soc_irq_is_enabled(uint32_t irq_num)
{
    bool ret;

    if (csi_vic_get_enabled_irq((int32_t)irq_num)) {
        ret = true;
    } else {
        ret = false;
    }

    return ret;
}

void soc_irq_priority(uint32_t irq_num, uint32_t priority)
{
    csi_vic_set_prio((int32_t)irq_num, priority);
}

/**
 * @brief get irq vector num
 * @return irq no
 */
uint32_t soc_irq_get_irq_num(void)
{
    int hartid = csi_get_cpu_id();
#if CONFIG_INTC_CLIC
    (void) hartid;
#if CONFIG_RISCV_SMODE
    return (__get_SCAUSE() & 0x3FFU);
#else
    return (__get_MCAUSE() & 0x3FFU);
#endif /* CONFIG_RISCV_SMODE */
#endif /* CONFIG_INTC_CLIC */

#if CONFIG_INTC_PLIC || CONFIG_INTC_CLIC_PLIC
    uint32_t num;
#if CONFIG_RISCV_SMODE
    uint32_t irqn = __get_SCAUSE() & 0x3FFU;
#else
    uint32_t irqn = __get_MCAUSE() & 0x3FFU;
#endif /* CONFIG_RISCV_SMODE */
    if (irqn == Machine_External_IRQn || irqn == Supervisor_External_IRQn) {
#if CONFIG_RISCV_SMODE
        num = PLIC_Hn_MSCLAIM_VAL(&PLIC->PLIC_H0_SCLAIM, hartid);
#else
        num = PLIC_Hn_MSCLAIM_VAL(&PLIC->PLIC_H0_MCLAIM, hartid);
#endif
#if CONFIG_INTC_CLIC_PLIC
        num += PLIC_IRQ_OFFSET;
#endif
    } else {
        num = irqn;
    }
    return num;
#endif /* CONFIG_INTC_PLIC || CONFIG_INTC_CLIC_PLIC */

#if CONFIG_INTC_APLIC || CONFIG_INTC_CLIC_APLIC
    uint32_t num;
#if CONFIG_RISCV_SMODE
    uint32_t irqn = __get_SCAUSE() & 0x3FFU;
#else
    uint32_t irqn = __get_MCAUSE() & 0x3FFU;
#endif  /* CONFIG_RISCV_SMODE */
    if (irqn == Machine_External_IRQn || irqn == Supervisor_External_IRQn) {
        num = csi_aplic_read_claimi(APLIC_BASE, hartid);
#if CONFIG_INTC_CLIC_APLIC
        num += APLIC_IRQ_OFFSET;
#endif
    } else {
        num = irqn;
    }
    return num;
#endif /* CONFIG_INTC_APLIC || CONFIG_INTC_CLIC_APLIC */

#if CONFIG_INTC_IMSIC_APLIC
    return g_handing_msi_num[hartid];
#endif
}

void soc_irq_end(uint32_t irq_num)
{
#if CONFIG_INTC_CLIC
    // DO NOTHING
#endif /* CONFIG_INTC_CLIC */

    /**
     * If aplic works in msi-mode
     * and the current interrupt is level-triggered
     * need retrigger
     */
#if CONFIG_INTC_IMSIC_APLIC
    extern csi_dev_t *g_cpu_irq_table[CONFIG_NR_CPUS][CONFIG_IRQ_NUM];
    csi_aplic_retrigger_level_irq(APLIC_BASE, g_cpu_irq_table[csi_get_cpu_id()][irq_num]->irq_num);
#endif


#if CONFIG_INTC_PLIC || CONFIG_INTC_CLIC_PLIC
#if CONFIG_INTC_CLIC_PLIC
    if (irq_num <= PLIC_IRQ_OFFSET) {
        return;
    }
    irq_num -= PLIC_IRQ_OFFSET;
#endif /* CONFIG_INTC_CLIC_PLIC */
#if CONFIG_RISCV_SMODE
    PLIC_Hn_MSCLAIM_VAL(&PLIC->PLIC_H0_SCLAIM, csi_get_cpu_id()) = irq_num;
#else
    PLIC_Hn_MSCLAIM_VAL(&PLIC->PLIC_H0_MCLAIM, csi_get_cpu_id()) = irq_num;
#endif
#endif /* CONFIG_INTC_PLIC || CONFIG_INTC_CLIC_PLIC */

#if CONFIG_INTC_APLIC || CONFIG_INTC_CLIC_APLIC
    // DO NOTHING
#endif /* CONFIG_INTC_APLIC || CONFIG_INTC_CLIC_APLIC */
}
