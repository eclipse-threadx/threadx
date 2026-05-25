/***************************************************************************
 * Copyright (c) 2026 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* Newlib system call stubs for RISC-V QEMU virt regression tests.
   Provides _write (UART output for printf), _exit (QEMU test finisher),
   and other minimal stubs needed by newlib. */

#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>

#define UART0_THR   (*(volatile unsigned char *)0x10000000L)
#define UART0_LSR   (*(volatile unsigned char *)0x10000005L)
#define LSR_TX_IDLE (1 << 5)

/* QEMU virt sifive_test device for clean exit. */
#define VIRT_TEST       (*(volatile uint32_t *)0x100000L)
#define VIRT_TEST_PASS  0x5555u
#define VIRT_TEST_FAIL  0x3333u

/* Called by testcontrol.c when EXTERNAL_EXIT is defined. */
__attribute__((noreturn)) void external_exit(unsigned int code)
{
    if (code == 0)
        VIRT_TEST = VIRT_TEST_PASS;
    else
        VIRT_TEST = (code << 16) | VIRT_TEST_FAIL;

    /* Should not reach here, but halt if it does. */
    while (1)
        ;
}

int _write(int fd, const char *buf, int count)
{
    (void)fd;
    for (int i = 0; i < count; i++)
    {
        while ((UART0_LSR & LSR_TX_IDLE) == 0)
            ;
        UART0_THR = (unsigned char)buf[i];
    }
    return count;
}

extern char _end[];
static char *heap_ptr = 0;

void *_sbrk(int incr)
{
    if (heap_ptr == 0)
        heap_ptr = _end;

    char *prev = heap_ptr;
    heap_ptr += incr;
    return prev;
}

int _close(int fd)
{
    (void)fd;
    errno = EBADF;
    return -1;
}

int _fstat(int fd, struct stat *st)
{
    (void)fd;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int fd)
{
    (void)fd;
    return 1;
}

int _lseek(int fd, int offset, int whence)
{
    (void)fd;
    (void)offset;
    (void)whence;
    errno = ESPIPE;
    return -1;
}

int _read(int fd, char *buf, int count)
{
    (void)fd;
    (void)buf;
    (void)count;
    return 0;
}

__attribute__((noreturn)) void _exit(int code)
{
    external_exit((unsigned int)code);
}

/* Override newlib's exit() to avoid pulling in __call_exitprocs and
   __stdio_exit_handler (compiled with mcmodel=medlow). */
__attribute__((noreturn)) void exit(int code)
{
    external_exit((unsigned int)code);
}

/* Minimal rand/srand to avoid pulling newlib's version which uses
   _impure_ptr (compiled with mcmodel=medlow, unreachable on RV64). */
static unsigned long rand_seed = 1;

int rand(void)
{
    rand_seed = rand_seed * 1103515245UL + 12345UL;
    return (int)((rand_seed >> 16) & 0x7fff);
}

void srand(unsigned int seed)
{
    rand_seed = seed;
}
