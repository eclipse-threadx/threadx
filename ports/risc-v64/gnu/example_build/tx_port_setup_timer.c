// setup timer interrupt(mtime)
#include "tx_port.h"
#include "tx_api.h"
#include "tx_arch_header.h"

ULONG64 ullNextTime = 0ULL;
const ULONG64 *pullNextTime = &ullNextTime;
const ULONG64 uxTimerIncrementsForOneTick = (ULONG64)((configSYS_CLOCK_HZ) / (TX_TIMER_TICKS_PER_SECOND));
UINT const ullMachineTimerCompareRegisterBase = configMTIMECMP_BASE_ADDRESS;

#if (defined THEAD_C906) && (!defined configMTIME_BASE_ADDRESS) && (configMTIMECMP_BASE_ADDRESS != 0)
volatile UINT *pulMachineTimerCompareRegisterL = NULL;
volatile UINT *pulMachineTimerCompareRegisterH = NULL;
#else
volatile uint64_t *pullMachineTimerCompareRegister = NULL;
#error define failed---------------------------------
#endif

void tx_port_setup_timer(void)
{

    asm volatile("csrc mstatus, %0" ::"r"(0x08));  // disable enterrupt

#ifdef THEAD_C906
    /* If there is a CLINT then it is ok to use the default implementation
    in this file, otherwise vPortSetupTimerInterrupt() must be implemented to
    configure whichever clock is to be used to generate the tick interrupt. */
    uint64_t ullCurrentTime;
    volatile uint32_t ulHartId;
    asm volatile("csrr %0, mhartid" : "=r"(ulHartId));

    // 32bit IO bus, need to get hi/lo seperately
    pulMachineTimerCompareRegisterL =
        (volatile uint32_t *)(ullMachineTimerCompareRegisterBase + (ulHartId * sizeof(uint64_t)));
    pulMachineTimerCompareRegisterH =
        (volatile uint32_t *)(ullMachineTimerCompareRegisterBase + sizeof(uint32_t) + (ulHartId * sizeof(uint64_t)));

    asm volatile("rdtime %0" : "=r"(ullCurrentTime));

    ullNextTime = (uint64_t)ullCurrentTime;
    ullNextTime += (uint64_t)uxTimerIncrementsForOneTick;
    *pulMachineTimerCompareRegisterL = (uint32_t)(ullNextTime & 0xFFFFFFFF);
    *pulMachineTimerCompareRegisterH = (uint32_t)(ullNextTime >> 32);

    /* Prepare the time to use after the next tick interrupt. */
    ullNextTime += (uint64_t)uxTimerIncrementsForOneTick;
#endif

#if ((configMTIME_BASE_ADDRESS != 0) && (configMTIMECMP_BASE_ADDRESS != 0) || (defined THEAD_C906))
    {
        /* Enable mtime and external interrupts. */
        asm volatile("csrs mie, %0" ::"r"(0x880));
    }
#else
    {
        /* Enable external interrupts. */
        __asm volatile("csrs mie, %0" ::"r"(0x800));
    }
#endif /* ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 ) */
}
