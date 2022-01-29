/*
 *
 * Armv8-A AArch64 common helper functions
 *
 * Copyright (c) 2012-2014 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#ifndef V8_AARCH64_H
#define V8_AARCH64_H

/*
 * Parameters for data barriers
 */
#define OSHLD   1
#define OSHST   2
#define OSH     3
#define NSHLD   5
#define NSHST   6
#define NSH     7
#define ISHLD   9
#define ISHST  10
#define ISH    11
#define LD     13
#define ST     14
#define SY     15

/**********************************************************************/

/*
 * function prototypes
 */

/*
 * void InvalidateUDCaches(void)
 *   invalidates all Unified and Data Caches
 *
 * Inputs
 *   <none>
 *
 * Returns
 *   <nothing>
 *
 * Side Effects
 *   guarantees that all levels of cache will be invalidated before
 *   returning to caller
 */
void InvalidateUDCaches(void);

/*
 * unsigned long long EnableCachesEL1(void)
 *   enables I- and D- caches at EL1
 *
 * Inputs
 *   <none>
 *
 * Returns
 *   New value of SCTLR_EL1
 *
 * Side Effects
 *   context will be synchronised before returning to caller
 */
unsigned long long EnableCachesEL1(void);

/*
 * unsigned long long GetMIDR(void)
 *   returns the contents of MIDR_EL0
 *
 * Inputs
 *   <none>
 *
 * Returns
 *   MIDR_EL0
 */
unsigned long long GetMIDR(void);

/*
 * unsigned long long GetMPIDR(void)
 *   returns the contents of MPIDR_EL0
 *
 * Inputs
 *   <none>
 *
 * Returns
 *   MPIDR_EL0
 */
unsigned long long GetMPIDR(void);

/*
 * unsigned int GetCPUID(void)
 *   returns the Aff0 field of MPIDR_EL0
 *
 * Inputs
 *   <none>
 *
 * Returns
 *   MPIDR_EL0[7:0]
 */
unsigned int GetCPUID(void);

#endif
