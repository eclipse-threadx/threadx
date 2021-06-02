#include "gic.h"

/* Systick registers */
#define PRIVTIM_IRQ     34
#define PRIVATE_TIMER_BASE (0xB0110000)
#define PRIVTIM_RELOAD  (*((volatile uint32_t *)(PRIVATE_TIMER_BASE + 0x0)))
#define PRIVTIM_CURRENT (*((volatile uint32_t *)(PRIVATE_TIMER_BASE + 0x4)))
#define PRIVTIM_CTRL    (*((volatile uint32_t *)(PRIVATE_TIMER_BASE + 0x8)))
#define PRIVTIM_STATUS  (*((volatile uint32_t *)(PRIVATE_TIMER_BASE + 0xc)))

#define OS_CLOCK       12000000
#define OS_TICK        1000
#define OS_TRV          ((uint32_t)(((double)OS_CLOCK*(double)OS_TICK)/1E6)-1)

#define U32 uint32_t
#define GICD_ICDICER0   (*((volatile U32 *)(GICDistributor_BASE + 0x180))) /* - RW - Interrupt Clear-Enable Registers */
#define GICD_ICDISER0   (*((volatile U32 *)(GICDistributor_BASE + 0x100))) /* - RW - Interrupt Set-Enable Registers */
#define GICD_ICDIPR0    (*((volatile U32 *)(GICDistributor_BASE + 0x400))) /* - RW - Interrupt Priority Registers */
#define GICD_ICDSGIR    (*((volatile U32 *)(GICDistributor_BASE + 0xf00))) /* - RW - Interrupt Software Interrupt Register */
#define GICD_ICDICERx(irq)   *(volatile U32 *)(&GICD_ICDICER0 + irq/32)
#define GICD_ICDISERx(irq)   *(volatile U32 *)(&GICD_ICDISER0 + irq/32)

void timer_init()
{
    GIC_EnableIRQ(34);
    PRIVTIM_RELOAD  = 0x200;
    PRIVTIM_CTRL    |= 0xC0; /* Enable timer, periodic mode */
}
