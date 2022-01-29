/*
 * Armv8-A AArch64 - Basic Mutex Example
 *
 * Copyright (c) 2012-2014 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */
#ifndef MP_MUTEX_H
#define MP_MUTEX_H

/*
 * The Arm C library calls-out to these functions to manage multithreading.
 * They can also be called by user application code.
 *
 * Mutex type is specified by the Arm C library
 *
 * Declare function prototypes for libc mutex routines
 */
typedef signed int *mutex;

/*
 * int _mutex_initialize(mutex *m)
 *
 * Inputs
 *  mutex *m - pointer to the 32-bit word associated with the mutex
 *
 * Returns
 *  0 - application is non-threaded
 *  1 - application is threaded
 *  The C library uses the return result to indicate whether it is being used in a multithreaded environment.
 */
int _mutex_initialize(mutex *m);

/*
 * void _mutex_acquire(mutex *m)
 *
 * Inputs
 *  mutex *m - pointer to the 32-bit word associated with the mutex
 *
 * Returns
 *  <nothing>
 *
 * Side Effects
 *  Routine does not return until the mutex has been claimed. A load-acquire
 *  is used to guarantee that the mutex claim is properly ordered with
 *  respect to any accesses to the resource protected by the mutex
 */
void _mutex_acquire(mutex *m);

/*
 * void _mutex_release(mutex *m)
 *
 * Inputs
 *  mutex *m - pointer to the 32-bit word associated with the mutex
 *
 * Returns
 *  <nothing>
 *
 * Side Effects
 *  A store-release is used to guarantee that the mutex release is properly
 *  ordered with respect any accesses to the resource protected by the mutex
 */
void _mutex_release(mutex *m);

#endif
