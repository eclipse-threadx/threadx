/* ------------------------------------------
 * Copyright (c) 2016, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 --------------------------------------------- */

#include "arc.h"

/*********************************************************************
 * Core Intc setup
 *********************************************************************/

#define AUX_STAT32      0x00a

#define AUX_IVT_BASE	0x025
#define AUX_VOL         0x05e

#define AUX_IRQ_CTRL	0x00E
#define AUX_IRQ_SEL     0x40b
#define AUX_IRQ_PRIO	0x206

#define INTC_DEF_PRIO	1


/*********************************************************************
 * Timers uses timer0
 *********************************************************************/

#define AUX_TIM0_CNT	0x021
#define AUX_TIM0_CTRL	0x022
#define AUX_TIM0_LIMIT	0x023



/*********************************************************************
 * Inter Core interruts
 *********************************************************************/

#define AUX_MCIP_BCR		   0x0d0
#define AUX_MCIP_CMD		   0x600
#define AUX_MCIP_WDATA		   0x601
#define AUX_MCIP_READBK		   0x602

#define CMD_ICI_GENERATE_IRQ	0x1
#define CMD_ICI_GENERATE_ACK	0x2
#define CMD_ICI_READ_STATUS	    0x3
#define CMD_ICI_CHECK_SOURCE	0x4



void  _tx_thread_smp_initialize_wait(void);
void  arc_timer_setup(unsigned int cycles);


void arc_cpu_init(void)
{
	extern char VECT_TABLE_BASE[];  // from sample_threadx.lcf
	_sr((unsigned int)VECT_TABLE_BASE, AUX_IVT_BASE);
	
	/* 0xc000_0000 in uncached */
	_sr(0xc0000000, AUX_VOL);

	/* setup irqs to interrupt at default interruption threshhold */
	_sr(IRQ_TIMER, AUX_IRQ_SEL);
        _sr(INTC_DEF_PRIO, AUX_IRQ_PRIO);

	_sr(IRQ_IPI, AUX_IRQ_SEL);
        _sr(INTC_DEF_PRIO, AUX_IRQ_PRIO);

#ifndef TX_ZERO_BASED_CORE_ID
        if (smp_processor_id() > 1)
#else
            if (smp_processor_id() > 0)
#endif
    	_tx_thread_smp_initialize_wait();

    arc_timer_setup(19999);
}


void arc_timer_setup(unsigned int cycles)
{
	_sr(cycles, AUX_TIM0_LIMIT);	/* interupt after CNT == @cycles */
	_sr(0,      AUX_TIM0_CNT);	    /* initial CNT */
	_sr(0x3,    AUX_TIM0_CTRL);	    /* Interrupt enable, count only when NOT halted */
}



static inline void __mcip_cmd(unsigned int cmd, unsigned int param)
{
	struct mcip_cmd {
		unsigned int cmd:8, param:16, pad:8;
	} buf;

	buf.pad = 0;
	buf.cmd = cmd;
	buf.param = param;

	_sr(*(unsigned int *)&buf, AUX_MCIP_CMD);
}


void arc_ici_send(unsigned int cpu)
{
	int ipi_pend;

	__mcip_cmd(CMD_ICI_READ_STATUS, cpu);
	ipi_pend = _lr(AUX_MCIP_READBK);
	if (!ipi_pend)
		__mcip_cmd(CMD_ICI_GENERATE_IRQ, cpu);
}


void arc_ici_handler(void)
{
	unsigned int senders, c;


	__mcip_cmd(CMD_ICI_CHECK_SOURCE, 0);
	
	senders = _lr(AUX_MCIP_READBK);	/* 1,2,4,8... */

	/* No support interrupt coalescing yet */
	c = __ffs(senders);			/* 0,1,2,3 */
	__mcip_cmd(CMD_ICI_GENERATE_ACK, c);
}
