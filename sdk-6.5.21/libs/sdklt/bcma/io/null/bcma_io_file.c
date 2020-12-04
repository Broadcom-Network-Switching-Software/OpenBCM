/*! \file bcma_io_file.c
 *
 * NULL implementation of file I/O.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/io/bcma_io_file.h>

bcma_io_file_handle_t
bcma_io_file_open(const char *filename, const char *mode)
{
    return ((bcma_io_file_handle_t)0);
}

int
bcma_io_file_close(bcma_io_file_handle_t fh)
{
    return -1;
}

int
bcma_io_file_read(bcma_io_file_handle_t fh, void *buf, int size, int num)
{
    return -1;
}

int
bcma_io_file_write(bcma_io_file_handle_t fh, const void *buf, int size, int num)
{
    return -1;
}

int
bcma_io_file_flush(bcma_io_file_handle_t fh)
{
    return -1;
}

int
bcma_io_file_vprintf(bcma_io_file_handle_t fh, const char *format, va_list ap)
{
    return 0;
}

int
bcma_io_file_size(bcma_io_file_handle_t fh)
{
    return -1;
}

int
bcma_io_file_delete(const char *filename)
{
    return -1;
}

int
bcma_io_file_move(const char *old_filename, const char *new_filename)
{
    return -1;
}

int
bcma_io_file_mktemp(char *template)
{
    return -1;
}
