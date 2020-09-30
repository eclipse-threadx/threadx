/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Zynq UltraScale+ MPSoC / Cortex-A53-SMP - Low-level functions       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#include "tx_api.h"
#include "tx_zynqmp.h"
#include <stdint.h>
#include <stdio.h>
#include <arm64_ghs.h>

/* Disable the data cache. By default the data cache is enabled. */
/*
#define TX_DCACHE_OFF
*/

/* GIC base address (CBAR register) */
#define INIT_CBAR           uint64_t CBAR = __MRS(__GENERIC_SYS_REG(3,1,15,3,0));

/* GIC Distributor Registers */
#define GIC_BASEADDR        (CBAR+0x10000ull)
#define GIC_REG(offset)     *((volatile uint32_t *)(GIC_BASEADDR+offset))

#define GICD_CTLR           GIC_REG(0x000)
#define GICD_ISENABLER(i)   GIC_REG(0x100 + 4*(i))
#define GICD_ICENABLER(i)   GIC_REG(0x180 + 4*(i))
#define GICD_ICPENDR(i)     GIC_REG(0x280 + 4*(i))
#define GICD_ISACTIVER(i)   GIC_REG(0x300 + 4*(i))
#define GICD_ICACTIVER(i)   GIC_REG(0x380 + 4*(i))
#define GICD_IPRIORITY(i)   GIC_REG(0x400 + 4*(i))
#define GICD_IPRIORITYB(i)  *((volatile uint8_t *)(GIC_BASEADDR+0x400+(i)))
#define GICD_ITARGETSR(i)   GIC_REG(0x800 + 4*(i))
#define GICD_ITARGETSRB(i)  *((volatile uint8_t *)(GIC_BASEADDR+0x800+(i)))
#define GICD_ICFGR(i)       GIC_REG(0xc00 + 4*(i))

/* GIC CPU Registers */
#define GIC_CPU_BASEADDR    (CBAR+0x20000ull)
#define GIC_CPU_REG(offset) *((volatile uint32_t *)(GIC_CPU_BASEADDR+offset))

#define GICC_CTLR           GIC_CPU_REG(0x000)
#define GICC_PMR            GIC_CPU_REG(0x004)
#define GICC_IAR            GIC_CPU_REG(0x00c)
#define GICC_EOIR           GIC_CPU_REG(0x010)

/* CRF_APB Clock and Reset control registers */
#define RST_FPD_APU         *((volatile uint32_t *) 0xfd1a0104ull)


/* Interrupt handler table */
#define IRQ_ID_MAX          192
uint64_t _tx_platform_irq_handlers[2*IRQ_ID_MAX];

/* default handler */
static void tx_irq_default_handler(uint64_t id)
{
    INIT_CBAR

    /* unexpected interrupt... disable it! */
    GICD_ICENABLER(id>>5) = 1 << (id & 0x1f);

#if 0   /* debug */
    while(1);
#endif
}

/* inter-processor software interrupt */
static void tx_core_interrupt(void *data)
{
    /* nothing to do, just used to wakeup the core */
}


/* Generic Timer Registers */
static inline uint32_t CNTFRQ_READ(void)
{
    return __MRS(__CNTFRQ_EL0);
}

static inline uint64_t CNTPCT_READ(void)
{
    return __MRS(__CNTPCT_EL0);
}

static inline void CNTP_CTL_WRITE(uint32_t v)
{
    __MSR(__CNTPS_CTL_EL1, v);
}

static inline uint64_t CNTP_CVAL_READ(void)
{
    return __MRS(__CNTPS_CVAL_EL1);
}

static inline void CNTP_CVAL_WRITE(uint64_t v)
{
    __MSR(__CNTPS_CVAL_EL1, v);
}

/* Generic Timer Interrupt */
#define GENERIC_TIMER_IRQ_ID    29


/* ThreadX timer interrupt */
extern void _tx_timer_interrupt(void);

static uint32_t tx_timer_delay;

static void tx_generic_timer_interrupt(void *data)
{
    /* update next timer expiration */
    CNTP_CVAL_WRITE(CNTP_CVAL_READ() + tx_timer_delay);

    /* call ThreadX timer interrupt handler */
    _tx_timer_interrupt();
}

#ifndef TX_DCACHE_OFF

/* MMU Tables */

#pragma ghs section bss=".mmu_tbl0"
static uint64_t mmu_tbl0[2];

#pragma ghs section bss=".mmu_tbl1"
static uint64_t mmu_tbl1[0x400];

#pragma ghs section bss=".mmu_tbl2"
static uint64_t mmu_tbl2[0x800];

#pragma ghs section bss=default

/* set MMU tables */
static void mmu_tbl_init(void)
{
    int i;
    uint64_t sect;
/*|                       | Memory Range                | Definition in Translation Table   |
 *|-----------------------|-----------------------------|-----------------------------------|
 *| DDR                   | 0x0000000000 - 0x007FFFFFFF | Normal write-back Cacheable       |
 *| PL                    | 0x0080000000 - 0x00BFFFFFFF | Strongly Ordered                  |
 *| QSPI, lower PCIe      | 0x00C0000000 - 0x00EFFFFFFF | Strongly Ordered                  |
 *| Reserved              | 0x00F0000000 - 0x00F7FFFFFF | Unassigned                        |
 *| STM Coresight         | 0x00F8000000 - 0x00F8FFFFFF | Strongly Ordered                  |
 *| GIC                   | 0x00F9000000 - 0x00F91FFFFF | Strongly Ordered                  |
 *| Reserved              | 0x00F9200000 - 0x00FCFFFFFF | Unassigned			                  |
 *| FPS, LPS slaves       | 0x00FD000000 - 0x00FFBFFFFF | Strongly Ordered                  |
 *| CSU, PMU              | 0x00FFC00000 - 0x00FFDFFFFF | Strongly Ordered                  |
 *| TCM, OCM              | 0x00FFE00000 - 0x00FFFFFFFF | Normal inner write-back cacheable |
 *| Reserved              | 0x0100000000 - 0x03FFFFFFFF | Unassigned                        |
 *| PL, PCIe              | 0x0400000000 - 0x07FFFFFFFF | Strongly Ordered                  |
 *| DDR                   | 0x0800000000 - 0x0FFFFFFFFF | Normal inner write-back cacheable |
 *| PL, PCIe              | 0x1000000000 - 0xBFFFFFFFFF | Strongly Ordered                  |
 *| Reserved              | 0xC000000000 - 0xFFFFFFFFFF | Unassigned                        |*/

#define MMU_RESERVED    0
#define MMU_MEMORY      0x705
#define MMU_DEVICE      (0x409 | (1ull << 53) | (1ull << 54))

    /* 0x00_0000_0000 - 0x7F_FFFF_FFFF */
    mmu_tbl0[0] = ((uint64_t) mmu_tbl1) + 0x3;
    /* 0x80_0000_0000 - 0xFF_FFFF_FFFF */
    mmu_tbl0[1] = ((uint64_t) mmu_tbl1) + 0x1000 + 0x3;

    /* 0x00_0000_0000 - 0x00_FFFF_FFFF */
    /* 2GB DDR, PL, other devices memory */
    sect = (uint64_t) mmu_tbl2;
    i = 0;
    for (; i < 0x004; i++, sect += 0x1000) {
      mmu_tbl1[i] = sect + 0x3;
    }
    /* 0x01_0000_0000 - 0x03_FFFF_FFFF */
    /* 16GB Reserved */
    sect = 0x100000000ull;
    for (; i < 0x010; i++, sect += 0x40000000) {
      mmu_tbl1[i] = sect + MMU_RESERVED;
    }
    /* 0x04_0000_0000 - 0x07_FFFF_FFFF */
    /* 8GB PL, 8GB PCIe */
    for (; i < 0x020; i++, sect += 0x40000000) {
      mmu_tbl1[i] = sect + MMU_DEVICE;
    }
    /* 0x08_0000_0000 - 0x0F_7FFF_FFFF */
    /* 2GB DDR */
#define DDR_1_SIZE  0x80000000u
#define DDR_1_REG   (DDR_1_SIZE/0x40000000)
    for (; i < (0x020 + DDR_1_REG); i++, sect += 0x40000000) {
      mmu_tbl1[i] = sect + MMU_MEMORY;
    }
#if DDR_1_REG < 0x20
    /* reserved for region where DDR is absent */
    for (; i < 0x040; i++, sect += 0x40000000) {
      mmu_tbl1[i] = sect + MMU_RESERVED;
    }
#endif
    /* 0x10_0000_0000 - 0x7F_FFFF_FFFF */
    /* 448GB PL */
    for (; i < 0x200; i++, sect += 0x40000000) {
      mmu_tbl1[i] = sect + MMU_DEVICE;
    }
    /* 0x80_0000_0000 - 0xBF_FFFF_FFFF */
    /* 256GB PCIe */
    for (; i < 0x300; i++, sect += 0x40000000) {
      mmu_tbl1[i] = sect + MMU_DEVICE;
    }
    /* 0xC0_0000_0000 - 0xFF_FFFF_FFFF */
    /* 256GB reserved */
    for (; i < 0x400; i++, sect += 0x40000000) {
      mmu_tbl1[i] = sect + MMU_RESERVED;
    }

    sect = 0;
    i = 0;
    /* 0x0000_0000 - 0x7FFF_FFFF */
    /* 2GB DDR */
#define DDR_0_SIZE  0x80000000u
#define DDR_0_REG   (DDR_0_SIZE/0x200000)
    for (; i < DDR_0_REG; i++, sect += 0x200000) {
      mmu_tbl2[i] = sect + MMU_MEMORY;
    }
    /* reserved for region where DDR is absent */
    for (; i < 0x400; i++, sect += 0x200000) {
      mmu_tbl2[i] = sect + MMU_RESERVED;
    }
    /* 0x8000_0000 - 0xBFFF_FFFF */
    /* 1GB lower PL */
    for (; i < 0x600; i++, sect += 0x200000) {
      mmu_tbl2[i] = sect + MMU_DEVICE;
    }
    /* 0xC000_0000 - 0xDFFF_FFFF */
    /* 512MB QSPI */
    for (; i < 0x700; i++, sect += 0x200000) {
      mmu_tbl2[i] = sect + MMU_DEVICE;
    }
    /* 0xE000_0000 - 0xEFFF_FFFF */
    /* 256MB lower PCIe */
    for (; i < 0x780; i++, sect += 0x200000) {
      mmu_tbl2[i] = sect + MMU_DEVICE;
    }
    /* 0xF000_0000 - 0xF7FF_FFFF */
    /* 128MB Reserved */
    for (; i < 0x7c0; i++, sect += 0x200000) {
      mmu_tbl2[i] = sect + MMU_RESERVED;
    }
    /* 0xF800_0000 - 0xFFDF_FFFF */
    /* set all range as device */
    for (; i < 0x7ff; i++, sect += 0x200000) {
      mmu_tbl2[i] = sect + MMU_DEVICE;
    }
    /* 0xFFE0_0000 - 0xFFFF_FFFF*/
    /* 2MB OCM/TCM */
    for (; i < 0x800; i++, sect += 0x200000) {
      mmu_tbl2[i] = sect + MMU_MEMORY;
    }
}

#endif /* !TX_DCACHE_OFF */

/* enable MMU and caches */
static void tx_caches_enable(void)
{
#ifndef TX_DCACHE_OFF
    /* set level 0 TTBR0_EL3 */
    __DSB_OPT(__BARRIER_SY);
    __MSR(__TTBR0_EL3, (uint64_t) mmu_tbl0);
    /* set memory attributes */
    __MSR(__MAIR_EL3, 0x000000BB0400FF44ull);
    /* set TCR_EL3 */
    __MSR(__TCR_EL3, 0x80823518ull);

    /* set SCTLR_EL3: enable mmu and caches + SP alignment check */
    __MSR(__SCTLR_EL3, __MRS(__SCTLR_EL3) | 0x100d);

    __DSB_OPT(__BARRIER_SY);
    __ISB();
#else
    /* set SCTLR_EL3: enable instruction cache + SP alignment check */
    __MSR(__SCTLR_EL3, __MRS(__SCTLR_EL3) | 0x1008);
#endif
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_platform_initialize_low_level                                   */
/*                              Zynq UltraScale+ MPSoC Cortex-A53-SMP/GHS */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function initializes the Interrupt Controller and configures   */
/*    the Generic Timer for the ThreadX periodic timer interrupt source   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_initialize_low_level           ThreadX low level initialization */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
void _tx_platform_initialize_low_level(void)
{
    INIT_CBAR
    int i;

#ifndef TX_DCACHE_OFF
    /* initialize MMU tables */
    mmu_tbl_init();
#endif
    /* enable data and instruction caches */
    tx_caches_enable();

    /* set default interrupt handlers */
    for (i=0; i<IRQ_ID_MAX; i++)
    {
        _tx_platform_irq_handlers[2*i]   = (uint64_t) tx_irq_default_handler;
        _tx_platform_irq_handlers[2*i+1] = (uint64_t) i;
    }

    /* initialize the interrupt controller: */
    /* enable global distributor */
    GICD_CTLR |= 1;
    /* disable all interrupts */
    for (i=0; i<(IRQ_ID_MAX + 31)/32; i++)
    {
        GICD_ICENABLER(i) = ~0;
        GICD_ICPENDR(i) = ~0;
        GICD_ICACTIVER(i) = ~0;
    }
    /* redirect all interrupts to cpu #0 */
    for (i=8; i<(IRQ_ID_MAX + 3)/4; i++)
    {
        GICD_ITARGETSR(i) = 0x01010101;
    }
    /* set default priority to 0 */
    for (i=0; i<(IRQ_ID_MAX + 3)/4; i++)
    {
        GICD_IPRIORITY(i) = 0x00000000;
    }
    /* set default configuration as level-sensitive */
    for (i=2; i<(IRQ_ID_MAX + 15)/16; i++)
    {
        GICD_ICFGR(i) &= 0x55555555;
    }

    /* initialize the interrupt controller: */
    /* enable physical cpu interface */
    GICC_CTLR |= 1;
    /* enable all priorities */
    GICC_PMR = 0xff;

    /* Use the Generic Timer for generating clock ticks: */
    /* compute tick delay from counter frequency */
    tx_timer_delay = CNTFRQ_READ() / TX_TIMER_TICKS_PER_SECOND;
    /* set timer expiration from current counter value */
    CNTP_CVAL_WRITE(CNTPCT_READ() + tx_timer_delay);
    /* configure CNTP_CTL to enable timer interrupts */
    CNTP_CTL_WRITE(1);
    /* install timer interrupt handler */
    tx_zynqmp_irq_enable(GENERIC_TIMER_IRQ_ID, tx_generic_timer_interrupt, NULL);

    /* set handler for software interrupt 0 */
    tx_zynqmp_irq_enable(0, tx_core_interrupt, NULL);

    /* release reset for cores 1-(MAX_CORES-1) */
    RST_FPD_APU &= ~((1 << TX_THREAD_SMP_MAX_CORES) - 2);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_platform_smp_initialize_low_level                               */
/*                              Zynq UltraScale+ MPSoC Cortex-A53-SMP/GHS */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function initializes the cores 1-3 for SMP processing          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _boot_smp                             cores 1-3 startup code        */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
void _tx_platform_smp_initialize_low_level(void)
{
    INIT_CBAR

    /* enable data and instruction caches */
    tx_caches_enable();

    /* initialize the interrupt controller: */
    /* enable physical cpu interface */
    GICC_CTLR |= 1;
    /* enable all priorities */
    GICC_PMR = 0xff;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    tx_zynqmp_irq_enable      Zynq UltraScale+ MPSoC Cortex-A53-SMP/GHS */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function attachs a handler to an interrupt and enables         */
/*    the interrupt.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    id                                    The ID of the interrupt       */
/*    handler                               The interrupt handler         */
/*    data                                  The data to pass to the       */
/*                                          handler.                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
void tx_zynqmp_irq_enable(unsigned id, void (*handler)(void *), void *data)
{
    INIT_CBAR

    if (id >= IRQ_ID_MAX)
    {
        return;
    }

    /* set handler */
    _tx_platform_irq_handlers[2*id]   = (uint64_t) handler;
    _tx_platform_irq_handlers[2*id+1] = (uint64_t) data;
    __DSB_OPT(__BARRIER_SY);  // ensure we're in sync...

    /* enable interrupt */
    GICD_ISENABLER(id>>5) = 1 << (id & 0x1f);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    tx_zynqmp_irq_disable     Zynq UltraScale+ MPSoC Cortex-A53-SMP/GHS */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function disables an interrupt and removes the previously      */
/*    attached handler                                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    id                                    The ID of the interrupt       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
void tx_zynqmp_irq_disable(unsigned id)
{
    INIT_CBAR

    if (id >= IRQ_ID_MAX)
    {
        return;
    }

    /* disable interrupt */
    GICD_ICENABLER(id>>5) = 1 << (id & 0x1f);

    /* clear pending state */
    GICD_ICPENDR(id>>5) = 1 << (id & 0x1f);

    /* ensure the interrupt is not still active before returning */
    while (GICD_ISACTIVER(id>>5) & (1 << (id & 0x1f)))
    {
    }

    /* restore default handler */
    _tx_platform_irq_handlers[2*id]   = (uint64_t) tx_irq_default_handler;
    _tx_platform_irq_handlers[2*id+1] = (uint64_t) id;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    tx_zynqmp_irq_priority    Zynq UltraScale+ MPSoC Cortex-A53-SMP/GHS */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets the priority of an interrupt                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    id                                    The ID of the interrupt       */
/*    prio                                  The interrupt priority 0-255  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
void tx_zynqmp_irq_priority(unsigned id, unsigned prio)
{
    INIT_CBAR

    if (id >= IRQ_ID_MAX)
    {
        return;
    }

    /* set priority */
    GICD_IPRIORITYB(id) = (uint8_t) prio;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    tx_zynqmp_irq_config          Zynq UltraScale+ MPSoC Cortex-A53/GHS */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function configures an interrupt as level-sensitive or         */
/*    edge-triggered                                                      */
/*    Only SPI interrupts (ID >= 32) can be configured                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    id                                    The ID of the interrupt       */
/*    edge                                  Configure the interrupt as    */
/*                                          level-sensitive (0) or        */
/*                                          edge-triggered (non 0)        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
void tx_zynqmp_irq_config(unsigned id, int edge)
{
    INIT_CBAR
    TX_INTERRUPT_SAVE_AREA
    uint32_t value;

    if (id < 32 || id >= IRQ_ID_MAX)
    {
        return;
    }

    TX_DISABLE

    /* set edge or level sensitive */
    value = GICD_ICFGR(id>>4);
    if (edge)
    {
        value |= 2 << (2*(id & 0xf));
    }
    else
    {
        value &= ~(2 << (2*(id & 0xf)));
    }
    GICD_ICFGR(id>>4) = value;

    TX_RESTORE
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    tx_zynqmp_irq_smp_core        Zynq UltraScale+ MPSoC Cortex-A53/GHS */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function selects on which core an interrupt will be handled    */
/*    Only SPI interrupts (ID >= 32) can be configured                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    irq_id                                The ID of the interrupt       */
/*    core_id                               The ID of the core 0-3        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
void tx_zynqmp_irq_smp_core(unsigned irq_id, unsigned core_id)
{
    INIT_CBAR

    if (irq_id < 32 || irq_id >= IRQ_ID_MAX || core_id >= TX_THREAD_SMP_MAX_CORES)
    {
        return;
    }

    /* set interrupt processor target */
    GICD_ITARGETSRB(irq_id) = 1 << core_id;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    tx_zynqmp_udelay              Zynq UltraScale+ MPSoC Cortex-A53/GHS */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function waits for a given small delay expressed in            */
/*    microseconds.                                                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    usecs                                 The number of microseconds    */
/*                                          to wait for                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
void tx_zynqmp_udelay(unsigned usecs)
{
    uint64_t t;
    /* get current time */
    t = CNTPCT_READ();
    /* compute expiration time */
    t += ((uint64_t) usecs * CNTFRQ_READ()) / 1000000U;
    while ((int64_t) (t - CNTPCT_READ()) > 0) {}
}
