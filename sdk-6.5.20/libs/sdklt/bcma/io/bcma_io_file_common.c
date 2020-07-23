/*! \file bcma_io_file_common.c
 *
 * Shared file I/O.
 *
 * The functions in this file are built on top of the system-dependent
 * file I/O functions, and as such, the functions should always remain
 * system-independent.
 *
 * The main purpose of having these system-independent functions is to
 * reduce the effort required to add support for a new file I/O
 * abstraction.
 *
 * An application will still be able to override these functions with
 * optimized versions if so desired.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <stdarg.h>

#include <sal/sal_libc.h>

#include <bcma/io/bcma_io_file.h>

char *
bcma_io_file_gets(bcma_io_file_handle_t fh, char *buf, int size)
{
    int c;
    char *ptr = buf;

    if (fh == NULL || size <= 0) {
        return NULL;
    }

    while (--size) {
        c = bcma_io_file_getc(fh);
        if (c == -1) {
            if (ptr == buf) {
                return NULL;
            }
            break;
        }

        if ((*ptr++ = c) == '\n') {
            break;
        }
    }

    *ptr = '\0';

    return buf;
}

int
bcma_io_file_getc(bcma_io_file_handle_t fh)
{
    char c;

    if (1 != bcma_io_file_read(fh, &c, 1, 1)) {
        return -1;
    }

    return (int)c;
}

int
bcma_io_file_puts(bcma_io_file_handle_t fh, const char *buf)
{
    int len, wb;

    len = sal_strlen(buf);
    wb = bcma_io_file_write(fh, buf, 1, len);

    return (wb == len) ? 0 : -1;
}

int
bcma_io_file_putc(bcma_io_file_handle_t fh, int c)
{
    uint8_t ch = (uint8_t)c;

    if (bcma_io_file_write(fh, &ch, 1, 1) != 1) {
        c = -1;
    }

    return c;
}

int
bcma_io_file_printf(bcma_io_file_handle_t fh, const char *format, ...)
{
    int rv;
    va_list ap;

    va_start(ap, format);
    rv = bcma_io_file_vprintf(fh, format, ap);
    va_end(ap);

    return rv;
}
