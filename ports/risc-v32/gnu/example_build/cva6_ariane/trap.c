#include "csr.h"
#include <stdint.h>
#include "uart.h"
#include "hwtimer.h"
#include "plic.h"
#include <tx_port.h>
#include <tx_api.h>

#define OS_IS_INTERUPT(mcause)     (mcause & 0x80000000u)
#define OS_IS_EXCEPTION(mcause)    (~(OS_IS_INTERUPT))
#define OS_IS_TICK_INT(mcause)     (mcause == 0x80000007u)
#define OS_IS_SOFT_INT(mcause)     (mcause == 0x80000003u)
#define OS_IS_EXT_INT(mcause)      (mcause == 0x8000000bu)
#define OS_IS_TRAP_USER(mcause)    (mcause == 0x0000000bu)
extern void _tx_timer_interrupt(void);

extern int uart_putc(int ch);

static void print_hex(uintptr_t val)
{
    char digits[] = "0123456789ABCDEF";
    uart_putc('0');
    uart_putc('x');
    for(int i = (sizeof(uintptr_t)*2) - 1; i >= 0; i--) {
        int d = (val >> (i*4)) & 0xF;
        uart_putc(digits[d]);
    }
    uart_putc('\n');
}

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
		puts("mcause:");
		print_hex(mcause);
		puts("mepc:");
		print_hex(mepc);
		puts("mtval:");
		print_hex(mtval);
		while(1) ;
	}
}
