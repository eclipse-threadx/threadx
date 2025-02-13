#include "tx_port.h"
#include "csr.h"
#include "hwtimer.h"

#define CLINT		       (0x02000000L)
#define CLINT_TIME             (CLINT+0xBFF8)
#define CLINT_TIMECMP(hart_id) (CLINT+0x4000+8*(hart_id))


int hwtimer_init(void)
{
	int hart = riscv_get_core();
	uint64_t time = *((uint64_t*)CLINT_TIME);
    	*((uint64_t*)CLINT_TIMECMP(hart)) = time + TICKNUM_PER_TIMER;
	return 0;
}

int hwtimer_handler(void)
{
	int hart = riscv_get_core();
        uint64_t time = *((uint64_t*)CLINT_TIME);
        *((uint64_t*)CLINT_TIMECMP(hart)) = time + TICKNUM_PER_TIMER;
	return 0;
}

