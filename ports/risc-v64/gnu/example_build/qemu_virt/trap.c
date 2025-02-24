#include "csr.h"
#include <stdint.h>
#include "uart.h"
#include "hwtimer.h"
#include "plic.h"
#include <tx_port.h>
#include <tx_api.h>

#define OS_IS_INTERUPT(mcause)     (mcause & 0x8000000000000000ull)
#define OS_IS_EXCEPTION(mcause)    (~(OS_IS_INTERUPT))
#define OS_IS_TICK_INT(mcause)     (mcause == 0x8000000000000007ull)
#define OS_IS_SOFT_INT(mcause)     (mcause == 0x8000000000000003ull)
#define OS_IS_EXT_INT(mcause)      (mcause == 0x800000000000000bull)
#define OS_IS_TRAP_USER(mcause)    (mcause == 0x000000000000000bull)
extern void _tx_timer_interrupt(void);

void trap_handler(uintptr_t mcause, uintptr_t mepc, uintptr_t mtval)
{
	if(OS_IS_INTERUPT(mcause))
	{
		if(OS_IS_TICK_INT(mcause))
		{
			hwtimer_handler();
			_tx_timer_interrupt();
		}
		else if(OS_IS_EXT_INT(mcause))
		{
			int ret = plic_irq_intr();
			if(ret)
			{
				puts("[INTERRUPT]: handler irq error!");
				while(1) ;
			}
		}
		else
		{
			puts("[INTERRUPT]: now can't deal with the interrupt!");
			while(1) ;
		}
	}
	else
	{
		puts("[EXCEPTION] : Unkown Error!!");
		while(1) ;
	}
}
