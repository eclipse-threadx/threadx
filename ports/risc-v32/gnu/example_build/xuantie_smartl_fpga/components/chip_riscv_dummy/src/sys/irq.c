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

#include <stdint.h>
#include <soc.h>
#include <csi_core.h>
#include <csi_config.h>
#include <drv/common.h>

extern uint32_t soc_irq_get_irq_num(void);
extern void soc_irq_end(uint32_t irq_num);

#if CONFIG_AOS_OSAL
#include <aos/kernel.h>
#include <dbg.h>
#define CSI_INTRPT_ENTER() aos_kernel_intrpt_enter()
#define CSI_INTRPT_EXIT()  aos_kernel_intrpt_exit()
#else
#ifdef CONFIG_KERNEL_FREERTOS
#include <FreeRTOS.h>
extern int freertos_intrpt_enter(void);
extern int freertos_intrpt_exit(void);
#define CSI_INTRPT_ENTER() freertos_intrpt_enter()
#define CSI_INTRPT_EXIT()  freertos_intrpt_exit()
#elif defined(CONFIG_KERNEL_RTTHREAD)
#include <rtthread.h>
#define printk rt_kprintf
extern void rt_interrupt_enter(void);
extern void rt_interrupt_leave(void);
#define CSI_INTRPT_ENTER() rt_interrupt_enter()
#define CSI_INTRPT_EXIT()  rt_interrupt_leave()
#else
#define printk printf
#define CSI_INTRPT_ENTER()
#define CSI_INTRPT_EXIT()
#endif
#endif /* end CONFIG_AOS_OSAL */


#if CONFIG_INTC_IMSIC_APLIC
csi_dev_t *g_cpu_irq_table[CONFIG_NR_CPUS][CONFIG_IRQ_NUM];
volatile msi_entry_t g_msi_map[CONFIG_NR_CPUS][CONFIG_IRQ_NUM];
#define g_irq_table g_cpu_irq_table[csi_get_cpu_id()]
#else
csi_dev_t *g_irq_table[CONFIG_IRQ_NUM];
#endif

#if defined(CONFIG_SMP) && CONFIG_SMP
volatile uint32_t  g_irq_nested_level[CONFIG_NR_CPUS];
#else
volatile uint32_t g_irq_nested_level;
#endif

/**
  \brief       register irq handler(deprecated).
  \param[in]   irq_num Number of IRQ.
  \return      None.
*/
void csi_irq_attach(uint32_t irq_num, void *irq_handler, csi_dev_t *dev)
{
#if CONFIG_INTC_IMSIC_APLIC
    int cpu_id = csi_get_cpu_id();
    int msi_num = csi_imsic_irqnum_alloc(cpu_id, MSI_SOURCE_APLIC, irq_num);
    g_cpu_irq_table[cpu_id][msi_num] = dev;
    csi_imsic_irq_attach(cpu_id, msi_num, irq_handler);
#else
    dev->irq_handler = irq_handler;
    g_irq_table[irq_num] = dev;
#endif
}

/**
  \brief       Attach irq handler2 for compatible(Recommended).
  \param[in]   irq_num      Number of IRQ.
  \param[in]   irq_handler2 IRQ Handler.
  \param[in]   dev          The dev to operate
  \param[in]   arg          user data of irq_handler2
  \return      None.
*/
void csi_irq_attach2(uint32_t irq_num, void *irq_handler2, csi_dev_t *dev, void *arg)
{
#if CONFIG_INTC_IMSIC_APLIC
    int cpu_id = csi_get_cpu_id();
    int msi_num = csi_imsic_irqnum_alloc(cpu_id, MSI_SOURCE_APLIC, irq_num);
    g_cpu_irq_table[cpu_id][msi_num] = dev;
    csi_imsic_irq_attach2(cpu_id, msi_num, irq_handler2, arg);
#else
    dev->arg             = arg;
    dev->irq_handler2    = irq_handler2;
    g_irq_table[irq_num] = dev;
#endif
}

/**
  \brief       unregister irq handler.
  \param[in]   irq_num Number of IRQ.
  \param[in]   irq_handler IRQ Handler.
  \return      None.
*/
void csi_irq_detach(uint32_t irq_num)
{
#if CONFIG_INTC_IMSIC_APLIC
    int msi_num = csi_aplic_get_target_eiid(APLIC_BASE, irq_num);
    int cpu_id = csi_aplic_get_target_hart(APLIC_BASE, irq_num);
    CSI_ASSERT(csi_imsic_irq_detach(cpu_id, msi_num) == CSI_OK);
    CSI_ASSERT(csi_imsic_irqnum_free(cpu_id, msi_num) == CSI_OK);
    CSI_ASSERT(g_cpu_irq_table[cpu_id][msi_num] != NULL);
    g_cpu_irq_table[cpu_id][msi_num] = NULL;
#else
    g_irq_table[irq_num] = NULL;
#endif
}

/**
  \brief       gets whether in irq context
  \return      true or false.
*/
bool csi_irq_context(void)
{
#if defined(CONFIG_SMP) && CONFIG_SMP
    return ((g_irq_nested_level[csi_get_cpu_id()] > 0U) ? true : false);
#else
    return ((g_irq_nested_level > 0U) ? true : false);
#endif
}

static volatile int g_nmi_cnt;
__attribute__((weak)) void handle_nmi_exception(void)
{
    g_nmi_cnt++;
#if CONFIG_SUPPORT_NMI_DEMO
    extern void timer_clear_irq();
    timer_clear_irq();
#endif
}

//FIXME: For Non CLIC mode
extern void tick_irq_handler(void *arg);
void CORET_IRQHandler(void)
{
#if defined(CONFIG_SMP) && CONFIG_SMP
    g_irq_nested_level[csi_get_cpu_id()]++;
#else
    g_irq_nested_level++;
#endif
    CSI_INTRPT_ENTER();
    tick_irq_handler(NULL);
    CSI_INTRPT_EXIT();
#if defined(CONFIG_SMP) && CONFIG_SMP
    g_irq_nested_level[csi_get_cpu_id()]--;
#else
    g_irq_nested_level--;
#endif
}

#if CONFIG_ECC_L1_ENABLE || CONFIG_ECC_L2_ENABLE
static struct {
    int err_cnt_l1;
    int err_cnt_l2;
} g_ecc_stat;

void __attribute__((weak)) ecc_l1_irqhandler(void *arg)
{
    g_ecc_stat.err_cnt_l1++;

    if (!(__get_MCER() >> 31) || (__get_MCER() & (0x1 << 30))) {
        /* may be ecc fatal error happens */
        while (1);
    } else {
        /* clear MCER EE_VLD */
#if __riscv_xlen == 32
        __set_MCER(0);
        __set_MCERH(0);
#else
        __set_MCER(0);
#endif
    }
}

void __attribute__((weak)) ecc_l2_irqhandler(void *arg)
{
    g_ecc_stat.err_cnt_l2++;

#if __riscv_xlen == 32
    if((__get_MCER2H() >> 30) == 0x2) {
        /* clear MCER EE_VLD */
        __set_MCER2(0);
        __set_MCER2H(0);
    } else {
        /* may be ecc fatal error happens */
        while (1);
    }
#else
    if((__get_MCER2() >> 62) == 0x2) {
        /* clear MCER EE_VLD */
        __set_MCER2(0);
    } else {
        /* may be ecc fatal error happens */
        while (1);
    }
#endif
}

void ECC_L1_IRQHandler(void)
{
#if defined(CONFIG_SMP) && CONFIG_SMP
    g_irq_nested_level[csi_get_cpu_id()]++;
#else
    g_irq_nested_level++;
#endif
    CSI_INTRPT_ENTER();
    ecc_l1_irqhandler(NULL);
    CSI_INTRPT_EXIT();
#if defined(CONFIG_SMP) && CONFIG_SMP
    g_irq_nested_level[csi_get_cpu_id()]--;
#else
    g_irq_nested_level--;
#endif
}
#endif /* CONFIG_ECC_L1_ENABLE || CONFIG_ECC_L2_ENABLE */

/**
  \brief       dispatching irq handlers(only handle external irq)
  \return      None.
*/
void do_irq(void)
{
    uint32_t irqn;

#if defined(CONFIG_SMP) && CONFIG_SMP
    g_irq_nested_level[csi_get_cpu_id()]++;
#else
    g_irq_nested_level++;
#endif
    CSI_INTRPT_ENTER();
    irqn = soc_irq_get_irq_num();

    if (irqn > sizeof(g_irq_table) / sizeof(g_irq_table[0]) - 1 ) {
        printk("undefined interrupt: irqn = 0x%x\n", irqn);
        while(1);
    }
    if (g_irq_table[irqn]) {
        if (g_irq_table[irqn]->irq_handler) {
            /* for compatibility */
            g_irq_table[irqn]->irq_handler(g_irq_table[irqn]);
        }
        else if (g_irq_table[irqn]->irq_handler2) {
            g_irq_table[irqn]->irq_handler2(irqn, g_irq_table[irqn]->arg);
        }
        else {
            printk("undefined interrupt2: irqn = 0x%x\n", irqn);
            /*the interrupt has no registered isr*/
            while(1);
        }
    } else {
        printk("null irq_handler: irqn = 0x%x\n", irqn);
        while(1);
    }

    soc_irq_end(irqn);
    CSI_INTRPT_EXIT();
#if defined(CONFIG_SMP) && CONFIG_SMP
    g_irq_nested_level[csi_get_cpu_id()]--;
#else
    g_irq_nested_level--;
#endif
}
