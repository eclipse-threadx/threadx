/*
 * GICv3.h - data types and function prototypes for GICv3 utility routines
 *
 * Copyright (c) 2014-2017 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */
#ifndef GICV3_h
#define GICV3_h

#include <stdint.h>

/*
 * extra flags for GICD enable
 */
typedef enum
{
    gicdctlr_EnableGrp0 = (1 << 0),
    gicdctlr_EnableGrp1NS = (1 << 1),
    gicdctlr_EnableGrp1A = (1 << 1),
    gicdctlr_EnableGrp1S = (1 << 2),
    gicdctlr_EnableAll = (1 << 2) | (1 << 1) | (1 << 0),
    gicdctlr_ARE_S = (1 << 4),	/* Enable Secure state affinity routing */
    gicdctlr_ARE_NS = (1 << 5),	/* Enable Non-Secure state affinity routing */
    gicdctlr_DS = (1 << 6),	/* Disable Security support */
    gicdctlr_E1NWF = (1 << 7)	/* Enable "1-of-N" wakeup model */
} GICDCTLRFlags_t;

/*
 * modes for SPI routing
 */
typedef enum
{
    gicdirouter_ModeSpecific = 0,
    gicdirouter_ModeAny = (1 << 31)
} GICDIROUTERBits_t;

typedef enum
{
    gicdicfgr_Level = 0,
    gicdicfgr_Edge = (1 << 1)
} GICDICFGRBits_t;

typedef enum
{
    gicigroupr_G0S = 0,
    gicigroupr_G1NS = (1 << 0),
    gicigroupr_G1S = (1 << 2)
} GICIGROUPRBits_t;

typedef enum
{
    gicrwaker_ProcessorSleep = (1 << 1),
    gicrwaker_ChildrenAsleep = (1 << 2)
} GICRWAKERBits_t;

/**********************************************************************/

/*
 * Utility macros & functions
 */
#define RANGE_LIMIT(x)		((sizeof(x) / sizeof((x)[0])) - 1)

static inline uint64_t gicv3PackAffinity(uint32_t aff3, uint32_t aff2,
					 uint32_t aff1, uint32_t aff0)
{
    /*
     * only need to cast aff3 to get type promotion for all affinities
     */
    return ((((uint64_t)aff3 & 0xff) << 32) |
	    ((aff2 & 0xff) << 16) |
	    ((aff1 & 0xff) << 8) | aff0);
}

/**********************************************************************/

/*
 * GIC Distributor Function Prototypes
 */

/*
 * ConfigGICD - configure GIC Distributor prior to enabling it
 *
 * Inputs:
 *
 *  control - control flags
 *
 * Returns:
 *
 *  <nothing>
 *
 * NOTE:
 *
 *  ConfigGICD() will set an absolute flags value, whereas
 *  {En,Dis}ableGICD() will only {set,clear} the flag bits
 *  passed as a parameter
 */
void ConfigGICD(GICDCTLRFlags_t flags);

/*
 * EnableGICD - top-level enable for GIC Distributor
 *
 * Inputs:
 *
 *  flags - new control flags to set
 *
 * Returns:
 *
 *  <nothing>
 *
 * NOTE:
 *
 *  ConfigGICD() will set an absolute flags value, whereas
 *  {En,Dis}ableGICD() will only {set,clear} the flag bits
 *  passed as a parameter
 */
void EnableGICD(GICDCTLRFlags_t flags);

/*
 * DisableGICD - top-level disable for GIC Distributor
 *
 * Inputs
 *
 *  flags - control flags to clear
 *
 * Returns
 *
 *  <nothing>
 *
 * NOTE:
 *
 *  ConfigGICD() will set an absolute flags value, whereas
 *  {En,Dis}ableGICD() will only {set,clear} the flag bits
 *  passed as a parameter
 */
void DisableGICD(GICDCTLRFlags_t flags);

/*
 * SyncAREinGICD - synchronise GICD Address Routing Enable bits
 *
 * Inputs
 *
 *  flags - absolute flag bits to set in GIC Distributor
 *
 *  dosync - flag whether to wait for ARE bits to match passed
 *      flag field (dosync = true), or whether to set absolute
 *      flag bits (dosync = false)
 *
 * Returns
 *
 *  <nothing>
 *
 * NOTE:
 *
 * This function is used to resolve a race in an MP system whereby secondary
 * CPUs cannot reliably program all Redistributor registers until the
 * primary CPU has enabled Address Routing. The primary CPU will call this
 * function with dosync = false, while the secondaries will call it with
 * dosync = true.
 */
void SyncAREinGICD(GICDCTLRFlags_t flags, uint32_t dosync);

/*
 * EnableSPI - enable a specific shared peripheral interrupt
 *
 * Inputs:
 *
 *  id - which interrupt to enable
 *
 * Returns:
 *
 *  <nothing>
 */
void EnableSPI(uint32_t id);

/*
 * DisableSPI - disable a specific shared peripheral interrupt
 *
 * Inputs:
 *
 *  id - which interrupt to disable
 *
 * Returns:
 *
 *  <nothing>
 */
void DisableSPI(uint32_t id);

/*
 * SetSPIPriority - configure the priority for a shared peripheral interrupt
 *
 * Inputs:
 *
 *  id - interrupt identifier
 *
 *  priority - 8-bit priority to program (see note below)
 *
 * Returns:
 *
 *  <nothing>
 *
 * Note:
 *
 *  The GICv3 architecture makes this function sensitive to the Security
 *  context in terms of what effect it has on the programmed priority: no
 *  attempt is made to adjust for the reduced priority range available
 *  when making Non-Secure accesses to the GIC
 */
void SetSPIPriority(uint32_t id, uint32_t priority);

/*
 * GetSPIPriority - determine the priority for a shared peripheral interrupt
 *
 * Inputs:
 *
 *  id - interrupt identifier
 *
 * Returns:
 *
 *  interrupt priority in the range 0 - 0xff
 */
uint32_t GetSPIPriority(uint32_t id);

/*
 * SetSPIRoute - specify interrupt routing when gicdctlr_ARE is enabled
 *
 * Inputs:
 *
 *  id - interrupt identifier
 *
 *  affinity - prepacked "dotted quad" affinity routing. NOTE: use the
 *      gicv3PackAffinity() helper routine to generate this input
 *
 *  mode - select routing mode (specific affinity, or any recipient)
 *
 * Returns:
 *
 *  <nothing>
 */
void SetSPIRoute(uint32_t id, uint64_t affinity, GICDIROUTERBits_t mode);

/*
 * GetSPIRoute - read ARE-enabled interrupt routing information
 *
 * Inputs:
 *
 *  id - interrupt identifier
 *
 * Returns:
 *
 *  routing configuration
 */
uint64_t GetSPIRoute(uint32_t id);

/*
 * SetSPITarget - configure the set of processor targets for an interrupt
 *
 * Inputs
 *
 *  id - interrupt identifier
 *
 *  target - 8-bit target bitmap
 *
 * Returns
 *
 *  <nothing>
 */
void SetSPITarget(uint32_t id, uint32_t target);

/*
 * GetSPITarget - read the set of processor targets for an interrupt
 *
 * Inputs
 *
 *  id - interrupt identifier
 *
 * Returns
 *
 *  8-bit target bitmap
 */
uint32_t GetSPITarget(uint32_t id);

/*
 * ConfigureSPI - setup an interrupt as edge- or level-triggered
 *
 * Inputs
 *
 *   id - interrupt identifier
 *
 *   config - desired configuration
 *
 * Returns
 *
 *   <nothing>
 */
void ConfigureSPI(uint32_t id, GICDICFGRBits_t config);

/*
 * SetSPIPending - mark an interrupt as pending
 *
 * Inputs
 *
 *  id - interrupt identifier
 *
 * Returns
 *
 *   <nothing>
 */
void SetSPIPending(uint32_t id);

/*
 * ClearSPIPending - mark an interrupt as not pending
 *
 * Inputs
 *
 *  id - interrupt identifier
 *
 * Returns
 *
 *   <nothing>
 */
void ClearSPIPending(uint32_t id);

/*
 * GetSPIPending - query whether an interrupt is pending
 *
 * Inputs
 *
 *  id - interrupt identifier
 *
 * Returns
 *
 *   pending status
 */
uint32_t GetSPIPending(uint32_t id);

/*
 * SetSPISecurity - mark a shared peripheral interrupt as
 *     security <group>
 *
 * Inputs
 *
 *  id - which interrupt to mark
 *
 *  group - the group for the interrupt
 *
 * Returns
 *
 *  <nothing>
 */
void SetSPISecurity(uint32_t id, GICIGROUPRBits_t group);

/*
 * SetSPISecurityBlock - mark a block of 32 shared peripheral
 *     interrupts as security <group>
 *
 * Inputs:
 *
 *  block - which block to mark (e.g. 1 = Ints 32-63)
 *
 *  group - the group for the interrupts
 *
 * Returns:
 *
 *  <nothing>
 */
void SetSPISecurityBlock(uint32_t block, GICIGROUPRBits_t group);

/*
 * SetSPISecurityAll - mark all shared peripheral interrupts
 *     as security <group>
 *
 * Inputs:
 *
 *  group - the group for the interrupts
 *
 * Returns:
 *
 *  <nothing>
 */
void SetSPISecurityAll(GICIGROUPRBits_t group);

/**********************************************************************/

/*
 * GIC Re-Distributor Function Prototypes
 *
 * The model for calling Redistributor functions is that, rather than
 * identifying the target redistributor with every function call, the
 * SelectRedistributor() function is used to identify which redistributor
 * is to be used for all functions until a different redistributor is
 * explicitly selected
 */

/*
 * WakeupGICR - wake up a Redistributor
 *
 * Inputs:
 *
 *  gicr - which Redistributor to wakeup
 *
 * Returns:
 *
 *  <nothing>
 */
void WakeupGICR(uint32_t gicr);

/*
 * EnablePrivateInt - enable a private (SGI/PPI) interrupt
 *
 * Inputs:
 *
 *  gicr - which Redistributor to program
 *
 *  id - which interrupt to enable
 *
 * Returns:
 *
 *  <nothing>
 */
void EnablePrivateInt(uint32_t gicr, uint32_t id);

/*
 * DisablePrivateInt - disable a private (SGI/PPI) interrupt
 *
 * Inputs:
 *
 *  gicr - which Redistributor to program
 *
 *  id - which interrupt to disable
 *
 * Returns:
 *
 *  <nothing>
 */
void DisablePrivateInt(uint32_t gicr, uint32_t id);

/*
 * SetPrivateIntPriority - configure the priority for a private
 *     (SGI/PPI) interrupt
 *
 * Inputs:
 *
 *  gicr - which Redistributor to program
 *
 *  id - interrupt identifier
 *
 *  priority - 8-bit priority to program (see note below)
 *
 * Returns:
 *
 *  <nothing>
 *
 * Note:
 *
 *  The GICv3 architecture makes this function sensitive to the Security
 *  context in terms of what effect it has on the programmed priority: no
 *  attempt is made to adjust for the reduced priority range available
 *  when making Non-Secure accesses to the GIC
 */
void SetPrivateIntPriority(uint32_t gicr, uint32_t id, uint32_t priority);

/*
 * GetPrivateIntPriority - configure the priority for a private
 *     (SGI/PPI) interrupt
 *
 * Inputs:
 *
 *  gicr - which Redistributor to program
 *
 *  id - interrupt identifier
 *
 * Returns:
 *
 *   Int priority
 */
uint32_t GetPrivateIntPriority(uint32_t gicr, uint32_t id);

/*
 * SetPrivateIntPending - mark a private (SGI/PPI) interrupt as pending
 *
 * Inputs
 *
 *  gicr - which Redistributor to program
 *
 *  id - interrupt identifier
 *
 * Returns
 *
 *   <nothing>
 */
void SetPrivateIntPending(uint32_t gicr, uint32_t id);

/*
 * ClearPrivateIntPending - mark a private (SGI/PPI) interrupt as not pending
 *
 * Inputs
 *
 *  gicr - which Redistributor to program
 *
 *  id - interrupt identifier
 *
 * Returns
 *
 *   <nothing>
 */
void ClearPrivateIntPending(uint32_t gicr, uint32_t id);

/*
 * GetPrivateIntPending - query whether a private (SGI/PPI) interrupt is pending
 *
 * Inputs
 *
 *  gicr - which Redistributor to program
 *
 *  id - interrupt identifier
 *
 * Returns
 *
 *   pending status
 */
uint32_t GetPrivateIntPending(uint32_t gicr, uint32_t id);

/*
 * SetPrivateIntSecurity - mark a private (SGI/PPI) interrupt as
 *     security <group>
 *
 * Inputs
 *
 *  gicr - which Redistributor to program
 *
 *  id - which interrupt to mark
 *
 *  group - the group for the interrupt
 *
 * Returns
 *
 *  <nothing>
 */
void SetPrivateIntSecurity(uint32_t gicr, uint32_t id, GICIGROUPRBits_t group);

/*
 * SetPrivateIntSecurityBlock - mark all 32 private (SGI/PPI)
 *     interrupts as security <group>
 *
 * Inputs:
 *
 *  gicr - which Redistributor to program
 *
 *  group - the group for the interrupt
 *
 * Returns:
 *
 *  <nothing>
 */
void SetPrivateIntSecurityBlock(uint32_t gicr, GICIGROUPRBits_t group);

#endif /* ndef GICV3_h */

/* EOF GICv3.h */
