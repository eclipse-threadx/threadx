// ------------------------------------------------------------
// Cortex-A MPCore - Mutex
// Header File
//
// Copyright ARM Ltd 2009. All rights reserved.
// ------------------------------------------------------------

#ifndef _CORTEXA_MUTEX_
#define _CORTEXA_MUTEX_

// Struct
// 0xFF=unlocked  0x0 = Locked by CPU 0, 
//                0x1 = Locked by CPU 1, 
//                0x2 = Locked by CPU 2, 
//                0x3 = Locked by CPU 3
typedef struct
{
  unsigned int lock;
}mutex_t;

// Places mutex into a known state
// r0 = address of mutex_t
void init_mutex(mutex_t* pMutex);

// Blocking call, returns once successfully locked a mutex
// r0 = address of mutex_t
void lock_mutex(mutex_t* pMutex);

// Releases (unlock) mutex.  Fails if CPU not owner of mutex.
// returns 0x0 for success, and 0x1 for failure
// r0 = address of mutex_t
unsigned int unlock_mutex(mutex_t* pMutex);

// Returns 0x0 if mutex unlocked, 0x1 is locked
// r0 = address of mutex_t
void is_mutex_locked(mutex_t* pMutex);

#endif

// ------------------------------------------------------------
// End of MP_Mutexes.h
// ------------------------------------------------------------
