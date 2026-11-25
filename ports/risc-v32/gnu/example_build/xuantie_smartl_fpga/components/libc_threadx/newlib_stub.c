 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <reent.h>
#include <errno.h>
#include <stdio.h>
#include <sys/unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <tx_api.h>
#include <drv/uart.h>

extern csi_uart_t g_console_handle;

int _execve_r(struct _reent *ptr, const char *name, char *const *argv,
              char *const *env)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _fcntl_r(struct _reent *ptr, int fd, int cmd, int arg)
{
    ptr->_errno = EBADF;
    return -1;
}

int _fork_r(struct _reent *ptr)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _getpid_r(struct _reent *ptr)
{
    ptr->_errno = ENOSYS;
    return 0;
}

int _isatty_r(struct _reent *ptr, int fd)
{
    if (fd >= 0 && fd < 3) {
        return 1;
    }

    ptr->_errno = ENOTTY;
    return 0;
}

int _kill_r(struct _reent *ptr, int pid, int sig)
{
    ptr->_errno = ENOSYS;
    return -1;
}

_off_t _lseek_r(struct _reent *ptr, int fd, _off_t pos, int whence)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _mkdir_r(struct _reent *ptr, const char *name, int mode)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _open_r(struct _reent *ptr, const char *file, int flags, int mode)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _close_r(struct _reent *ptr, int fd)
{
    ptr->_errno = EBADF;
    return -1;
}

_ssize_t _read_r(struct _reent *ptr, int fd, void *buf, size_t nbytes)
{
    ptr->_errno = EBADF;
    return -1;
}

int fputc(int ch, FILE *stream)
{
    if (ch == '\n') {
        csi_uart_putc(&g_console_handle, '\r');
    }

    csi_uart_putc(&g_console_handle, ch);
    return 0;
}

int fgetc(FILE *stream)
{
    (void)stream;

    return csi_uart_getc(&g_console_handle);
}

int putc(int c, FILE *stream)
{
    return fputc(c, stream);
}

int puts(const char *s)
{
    while(*s !='\0') {
        fputc(*s, (void *)-1);
        s++;
    }
    fputc('\n', (void *)-1);
    return 0;
}

static void _putchar(char character)
{
    if (character == '\n') {
        csi_uart_putc(&g_console_handle, '\r');
    }

    csi_uart_putc(&g_console_handle, character);

}

int putchar(int c)
{
    _putchar(c);
    return 0;
}

_ssize_t _write_r(struct _reent *ptr, int fd, const void *buf, size_t nbytes)
{
    if ((fd == STDOUT_FILENO) || (fd == STDERR_FILENO)) {
        for (int i = 0; i < nbytes; i++)
            _putchar((*((char*)buf + i)));
        return nbytes;
    } else {
        return -1;
    }
}

int ioctl(int fildes, int request, ... /* arg */)
{
    return -1;
}

int _rename_r(struct _reent *ptr, const char *oldname, const char *newname)
{
    ptr->_errno = ENOSYS;
    return -1;
}

void *_sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
    ptr->_errno = ENOSYS;
    return NULL;
}

int _stat_r(struct _reent *ptr, const char *file, struct stat *pstat)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _fstat_r(struct _reent *ptr, int fd, struct stat *buf)
{
    ptr->_errno = EBADF;
    return -1;
}

_CLOCK_T_ _times_r(struct _reent *ptr, struct tms *ptms)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _link_r(struct _reent *ptr, const char *oldpath, const char *newpath)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _unlink_r(struct _reent *ptr, const char *file)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _wait_r(struct _reent *ptr, int *status)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _gettimeofday_r(struct _reent *ptr, struct timeval *tv, void *__tzp)
{
    return 0;
}

long timezone = 8; /* default CTS */

struct tm* localtime_r(const time_t* t, struct tm* r)
{
    time_t time_tmp;
    time_tmp = *t + timezone * 3600;
    return gmtime_r(&time_tmp, r);
}

struct tm* localtime(const time_t* t)
{
    struct tm* timeinfo;
    static struct tm tm_tmp;

    timeinfo = localtime_r(t, &tm_tmp);

    return timeinfo;
}

extern TX_BYTE_POOL tx_byte_pool_0;
void *_malloc_r(struct _reent *ptr, size_t size)
{
    UINT ret;
    void *p;

    ret = tx_byte_allocate(&tx_byte_pool_0, &p, size, TX_NO_WAIT);
    if(ret != TX_SUCCESS) {
        return NULL;
    }
    return p;
}

void *_realloc_r(struct _reent *ptr, void *old, size_t newlen)
{
    return NULL;
}

void *_calloc_r(struct _reent *ptr, size_t size, size_t len)
{
    UINT ret;
    void *p;

    ret = tx_byte_allocate(&tx_byte_pool_0, &p, size * len, TX_NO_WAIT);
    if(ret != TX_SUCCESS) {
        return NULL;
    }
    memset(p, 0, size * len);

    return p;
}

void *_memalign_r(struct _reent *ptr, size_t alignment, size_t size)
{
    return NULL;
}

void _free_r(struct _reent *ptr, void *addr)
{
    if (!addr)
        return;
    tx_byte_release(addr);
}

void _exit(int status)
{
    while (1)
        ;
}

void exit(int status)
{
    __builtin_unreachable(); // fix noreturn warning
}

__attribute__((weak)) void _fini()
{
}

void _system(const char *s)
{
    return;
}

void abort(void)
{
    __builtin_unreachable(); // fix noreturn warning
}

int isatty(int fd)
{
    if (fd == fileno(stdin) || fd == fileno(stdout) || fd == fileno(stderr)) {
        return -1;
    }
    return 0;
}

