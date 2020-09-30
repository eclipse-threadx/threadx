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

#ifndef TX_ZYNQMP_H
#define TX_ZYNQMP_H

#include <stdint.h>
#include <stdlib.h>

#ifdef   __cplusplus

/* Yes, C++ compiler is present.  Use standard C.  */
extern   "C" {
#endif


/* Define Interrupt Handling Interface functions. */

void tx_zynqmp_irq_enable(unsigned id, void (*handler)(void *), void *data);
void tx_zynqmp_irq_disable(unsigned id);
void tx_zynqmp_irq_priority(unsigned id, unsigned prio);
void tx_zynqmp_irq_config(unsigned id, int edge);


/* ThreadX SMP Extensions */

void tx_zynqmp_irq_smp_core(unsigned irq_id, unsigned core_id);


/* Wait for small pauses */

void tx_zynqmp_udelay(unsigned usecs);


#if 1   /* need compiler option -gnu_asm */

/* Define the size of a cache line */

#define TX_ZYNQMP_DCACHE_LINE_SIZE  64

/* Flush (Clean & Invalidate) memory region */

static inline void tx_zynqmp_dcache_flush(uintptr_t ptr, uintptr_t ptr_max)
{
    while (ptr < ptr_max)
    {
        /* Clean & Invalidate data cache by VA to PoC */
        asm volatile ( "DC CIVAC, %0" : : "r" (ptr));
        ptr += TX_ZYNQMP_DCACHE_LINE_SIZE;
    }
    /* wait for completion */
    asm volatile ( "DSB SY");
}

/* Invalidate memory region */

static inline void tx_zynqmp_dcache_invalidate(uintptr_t ptr, uintptr_t ptr_max)
{
    while (ptr < ptr_max)
    {
        /* Invalidate data cache by VA to PoC */
        asm volatile ( "DC IVAC, %0" : : "r" (ptr));
        ptr += TX_ZYNQMP_DCACHE_LINE_SIZE;
    }
    /* wait for completion */
    asm volatile ( "DSB SY");
}

/* Clean memory region (without Invalidate) */

static inline void tx_zynqmp_dcache_clean(uintptr_t ptr, uintptr_t ptr_max)
{
    while (ptr < ptr_max)
    {
        /* Clean data cache by VA to PoC */
        asm volatile ( "DC CVAC, %0" : : "r" (ptr));
        ptr += TX_ZYNQMP_DCACHE_LINE_SIZE;
    }
    /* wait for completion */
    asm volatile ( "DSB SY");
}

#endif

#ifdef   __cplusplus
/* Yes, C++ compiler is present.  Use standard C.  */
    }
#endif

#endif /* TX_ZYNQMP_H */
