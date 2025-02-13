#include "plic.h"
#include <stddef.h>
irq_callback callbacks[MAX_CALLBACK_NUM];

void plic_irq_enable(int irqno)
{
	int hart = riscv_get_core();
	*(uint32_t*)PLIC_MENABLE(hart) = (*(uint32_t*)PLIC_MENABLE(hart) | (1 << irqno));
	return;
}

void plic_irq_disable(int irqno)
{
	int hart = riscv_get_core();
        *(uint32_t*)PLIC_MENABLE(hart) = (*(uint32_t*)PLIC_MENABLE(hart) & (~(1 << irqno)));
        return;
}

void plic_prio_set(int irqno, int prio)
{
	PLIC_SET_PRIO(irqno, prio);
}

int  plic_prio_get(int irqno)
{
	return PLIC_GET_PRIO(irqno);
}

int  plic_register_callback(int irqno, irq_callback callback)
{
	if(!(irqno >=0 && irqno < MAX_CALLBACK_NUM))
		return -1;
	callbacks[irqno] = callback;
	return 0;
}

int  plic_unregister_callback(int irqno)
{
	return plic_register_callback(irqno, NULL);
}

int  plic_init(void)
{
	for(int i=0;i<MAX_CALLBACK_NUM;i++)
	{
		callbacks[i] = NULL;
	}
	return 0;
}

int  plic_claim(void)
{
	int hart = riscv_get_core();
	return (*(uint32_t*)PLIC_MCLAIM(hart));
}

void plic_complete(int irqno)
{
	int hart = riscv_get_core();
	*(uint32_t*)(PLIC_MCOMPLETE(hart)) = (uint32_t)irqno;
	return;
}

int  plic_irq_intr(void)
{
	int ret = -1;
	int irqno = plic_claim();
	if(callbacks[irqno] != NULL)
		ret = (callbacks[irqno])(irqno);
	plic_complete(irqno);
	return ret;
}
