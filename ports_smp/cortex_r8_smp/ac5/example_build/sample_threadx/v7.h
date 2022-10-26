// ------------------------------------------------------------
// v7-A Cache, TLB and Branch Prediction Maintenance Operations
// Header File
//
// Copyright ARM Ltd 2009. All rights reserved.
// ------------------------------------------------------------

#ifndef _SEE_V7_h
#define _SEE_V7_h

#include "kernel.h"

//
// Note:
// *_is() stands for "inner shareable"
//

// ------------------------------------------------------------
// Caches

void enable_caches(void);
void disable_caches(void);

void clean_dcache(void);
void clean_invalidate_dcache(void);

void invalidate_caches(void);
void invalidate_caches_is(void);

// ------------------------------------------------------------
// TLBs

void invalidate_unified_tlb(void);
void invalidate_unified_tlb_is(void);

// ------------------------------------------------------------
// Branch prediction

void enable_branch_prediction(void);
void disable_branch_prediction(void);

void invalidate_branch_target_cache(void);
void invalidate_branch_target_cache_is(void);

// ------------------------------------------------------------
// High Vecs

void enable_highvecs(void);
void disable_highvecs(void);

// ------------------------------------------------------------
// ID Registers

uint32_t get_MIDR(void);

#define MIDR_IMPL_SHIFT  24
#define MIDR_IMPL_MASK   0xFF
#define MIDR_VAR_SHIFT   20
#define MIDR_VAR_MASK    0xF
#define MIDR_ARCH_SHIFT  16
#define MIDR_ARCH_MASK   0xF
#define MIDR_PART_SHIFT  4
#define MIDR_PART_MASK   0xFFF
#define MIDR_REV_SHIFT   0
#define MIDR_REV_MASK    0xF

// tmp = get_MIDR();
// implementor = (tmp >> MIDR_IMPL_SHIFT) & MIDR_IMPL_MASK;
// variant     = (tmp >> MIDR_VAR_SHIFT)  & MIDR_VAR_MASK;
// architecture= (tmp >> MIDR_ARCH_SHIFT) & MIDR_ARCH_MASK;
// part_number = (tmp >> MIDR_PART_SHIFT) & MIDR_PART_MASK;
// revision    = tmp & MIDR_REV_MASK;

#define MIDR_PART_CA5    0xC05
#define MIDR_PART_CA8    0xC08
#define MIDR_PART_CA9    0xC09

uint32_t get_MPIDR(void);

#define MPIDR_FORMAT_SHIFT  31
#define MPIDR_FORMAT_MASK   0x1
#define MPIDR_UBIT_SHIFT    30
#define MPIDR_UBIT_MASK     0x1
#define MPIDR_CLUSTER_SHIFT 7
#define MPIDR_CLUSTER_MASK  0xF
#define MPIDR_CPUID_SHIFT   0
#define MPIDR_CPUID_MASK    0x3

#define MPIDR_CPUID_CPU0    0x0
#define MPIDR_CPUID_CPU1    0x1
#define MPIDR_CPUID_CPU2    0x2
#define MPIDR_CPUID_CPU3    0x3

#define MPIDR_UNIPROCESSPR  0x1

#define MPDIR_NEW_FORMAT    0x1

// ------------------------------------------------------------
// Context ID

uint32_t get_context_id(void);
void set_context_id(uint32_t);

#define CONTEXTID_ASID_SHIFT   0
#define CONTEXTID_ASID_MASK    0xFF
#define CONTEXTID_PROCID_SHIFT 8
#define CONTEXTID_PROCID_MASK  0x00FFFFFF

// tmp    = get_context_id();
// ASID   = tmp & CONTEXTID_ASID_MASK;
// PROCID = (tmp >> CONTEXTID_PROCID_SHIFT) & CONTEXTID_PROCID_MASK;

#endif

// ------------------------------------------------------------
// End of v7.h
// ------------------------------------------------------------
