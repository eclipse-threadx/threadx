/***************************************************************************
 * Copyright (c) 2026 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* Minimal printf/puts for RISC-V QEMU virt regression tests.
   Avoids linking newlib's stdio which is compiled with mcmodel=medlow
   and causes relocation overflow on RV64 (DRAM at 0x80000000).
   Only the format specifiers used by the test suite are supported:
   %s, %d, %u, %lu, %ld, %%, and plain strings. */

#include <stdarg.h>
#include <stddef.h>

extern int _write(int fd, const char *buf, int count);

static void print_str(const char *s)
{
    const char *p = s;
    while (*p)
        p++;
    _write(1, s, (int)(p - s));
}

static void print_unsigned(unsigned long val)
{
    char buf[21];
    int i = 0;

    if (val == 0)
    {
        _write(1, "0", 1);
        return;
    }

    while (val > 0)
    {
        buf[i++] = (char)('0' + (int)(val % 10u));
        val /= 10u;
    }

    /* Reverse. */
    for (int j = i - 1; j >= 0; j--)
        _write(1, &buf[j], 1);
}

static void print_hex(unsigned long val)
{
    static const char hex[] = "0123456789abcdef";
    char buf[17];
    int i = 0;

    if (val == 0)
    {
        _write(1, "0", 1);
        return;
    }

    while (val > 0)
    {
        buf[i++] = hex[val & 0xfu];
        val >>= 4;
    }

    for (int j = i - 1; j >= 0; j--)
        _write(1, &buf[j], 1);
}

static void print_signed(long val)
{
    if (val < 0)
    {
        _write(1, "-", 1);
        print_unsigned((unsigned long)(-val));
    }
    else
    {
        print_unsigned((unsigned long)val);
    }
}

int printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    const char *p = fmt;
    const char *seg = p;

    while (*p)
    {
        if (*p == '%')
        {
            /* Flush preceding literal segment. */
            if (p > seg)
                _write(1, seg, (int)(p - seg));

            p++;
            /* Check for 'l' length modifier. */
            int is_long = 0;
            if (*p == 'l')
            {
                is_long = 1;
                p++;
            }

            switch (*p)
            {
            case 's':
                print_str(va_arg(ap, const char *));
                break;
            case 'd':
                if (is_long)
                    print_signed(va_arg(ap, long));
                else
                    print_signed((long)va_arg(ap, int));
                break;
            case 'u':
                if (is_long)
                    print_unsigned(va_arg(ap, unsigned long));
                else
                    print_unsigned((unsigned long)va_arg(ap, unsigned int));
                break;
            case 'x':
                if (is_long)
                    print_hex(va_arg(ap, unsigned long));
                else
                    print_hex((unsigned long)va_arg(ap, unsigned int));
                break;
            case '%':
                _write(1, "%", 1);
                break;
            default:
                /* Unknown specifier — print as-is. */
                _write(1, "%", 1);
                if (is_long)
                    _write(1, "l", 1);
                _write(1, p, 1);
                break;
            }
            p++;
            seg = p;
        }
        else
        {
            p++;
        }
    }

    /* Flush remaining literal segment. */
    if (p > seg)
        _write(1, seg, (int)(p - seg));

    va_end(ap);
    return 0;
}

int puts(const char *s)
{
    print_str(s);
    _write(1, "\n", 1);
    return 0;
}
