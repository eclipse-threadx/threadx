// ------------------------------------------------------------
// Cortex-R MPCore - Snoop Control Unit
// Header File
//
// Copyright ARM Ltd 2009. All rights reserved.
// ------------------------------------------------------------

#ifndef _CORTEXA_SCU_
#define _CORTEXA_SCU_

// ------------------------------------------------------------
// SCU
// ------------------------------------------------------------

// Returns the base address of the private peripheral memory space
unsigned int get_base_addr(void);

// Returns the CPU ID (0 to 3) of the CPU executed on
unsigned int get_cpu_id(void);

// Returns the number of cores in the A9 Cluster
// NOTE:
// returns 0 = 1 core
//         1 = 2 cores etc...
// This is the format of the register, decided to leave it unchanged.
unsigned int get_num_cpus(void);

// Go to sleep, never returns
void go_to_sleep(void);

// ------------------------------------------------------------
// SCU
// ------------------------------------------------------------

// Enables the SCU
void enable_scu(void);

// Set this core as participating in SMP
void join_smp(void);

// Set this core as NOT participating in SMP
void leave_smp(void);

// The return value is 1 bit per core:
// bit 0 - CPU 0
// bit 1 - CPU 1
// etc...
unsigned int get_cpus_in_smp(void);

 //Enable the broadcasting of cache & TLB maintenance operations
// When enabled AND in SMP, broadcast all "inner sharable"
// cache and TLM maintenance operations to other SMP cores
void enable_maintenance_broadcast(void);

// Disable the broadcasting of cache & TLB maintenance operations
void disable_maintenance_broadcast(void);

// cpu: 0x0=CPU 0 0x1=CPU 1 etc...
// This function invalidates the SCU copy of the tag rams
// for the specified core.
void secure_SCU_invalidate(unsigned int cpu, unsigned int ways);

#endif

// ------------------------------------------------------------
// End of MP_SCU.h
// ------------------------------------------------------------
