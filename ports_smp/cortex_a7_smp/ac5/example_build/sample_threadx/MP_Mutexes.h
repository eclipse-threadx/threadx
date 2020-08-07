// ------------------------------------------------------------
// MP Mutex Header File
//
// Copyright (c) 2011-2014 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------

#ifndef MP_MUTEX_H
#define MP_MUTEX_H

// 0xFF = unlocked
// 0x0 = Locked by CPU 0
// 0x1 = Locked by CPU 1
// 0x2 = Locked by CPU 2
// 0x3 = Locked by CPU 3
typedef struct
{
  unsigned int lock;
}mutex_t;

// Places mutex into a known state
// r0 = address of mutex_t
void initMutex(mutex_t* pMutex);

// Blocking call, returns once successfully locked a mutex
// r0 = address of mutex_t
void lockMutex(mutex_t* pMutex);

// Releases (unlock) mutex.  Fails if CPU not owner of mutex.
// returns 0x0 for success, and 0x1 for failure
// r0 = address of mutex_t
unsigned int unlockMutex(mutex_t* pMutex);

// Returns 0x0 if mutex unlocked, 0x1 is locked
// r0 = address of mutex_t
unsigned int isMutexLocked(mutex_t* pMutex);

#endif
