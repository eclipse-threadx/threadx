/*
 *                ThreadX C/C++ Library Support
 *
 *      Copyright 1983-2019 Green Hills Software LLC.
 *
 *  This program is the property of Green Hills Software LLC.,
 *  its contents are proprietary information and no part of it
 *  is to be disclosed to anyone except employees of Green Hills
 *  Software LLC., or as agreed in writing signed by the President
 *  of Green Hills Software LLC.
 */

#ifndef _TX_GHS_H_
#define _TX_GHS_H_

#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <setjmp.h>

#if defined(__ghs) && (__GHS_VERSION_NUMBER >= 500)
extern void *__ghs_GetThreadLocalStorageItem(int specifier);

/* Thread-local storage routines for Green Hills releases 5.x and beyond.
  The following specifiers are used when calling
  __ghs_GetThreadLocalStorageItem.

  If __ghs_GetThreadLocalStorageItem is customized to
  return a per-thread errno value, define the preprocessor symbol
  USE_THREAD_LOCAL_ERRNO in ind_errn.c.
 */

enum __ghs_ThreadLocalStorage_specifier {
    __ghs_TLS_asctime_buff,
    __ghs_TLS_tmpnam_space,
    __ghs_TLS_strtok_saved_pos,
    __ghs_TLS_Errno,
    __ghs_TLS_gmtime_temp,
    __ghs_TLS___eh_globals,
    __ghs_TLS_SignalHandlers
};
#else
/* Thread-local storage routines for Green Hills releases 4.x and 3.x . */
typedef void (*SignalHandler)(int);

typedef struct
{
    int           Errno;                   /* errno.  */
    SignalHandler SignalHandlers[_SIGMAX]; /* signal() buffer.  */
    char          tmpnam_space[L_tmpnam];  /* tmpnam(NULL) buffer.  */
    char          asctime_buff[30];        /* .  */
    char          *strtok_saved_pos;       /* strtok() position.  */
    struct tm     gmtime_temp;             /* gmtime() and localtime() buffer.  */
    void          *__eh_globals;           /* Pointer for C++ exception handling.  */
} ThreadLocalStorage;

ThreadLocalStorage *GetThreadLocalStorage(void);
#endif


void __ghsLock(void);
void __ghsUnlock(void);

int  __ghs_SaveSignalContext(jmp_buf);
void __ghs_RestoreSignalContext(jmp_buf);

/* prototypes for FILE lock routines.  */
void __ghs_flock_file(void *);
void __ghs_funlock_file(void *);
int  __ghs_ftrylock_file(void *);
void __ghs_flock_create(void **);
void __ghs_flock_destroy(void *);

/* prototype for GHS/ThreadX error shell checking.  */
void __ghs_rnerr(char *errMsg, int stackLevels, int stackTraceDisplay, void *hexVal);

#endif /* _TX_GHS_H_ */
