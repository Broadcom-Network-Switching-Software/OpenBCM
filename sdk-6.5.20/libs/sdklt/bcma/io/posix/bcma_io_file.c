/*! \file bcma_io_file.c
 *
 * POSIX file I/O.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <bcma/io/bcma_io_file.h>

bcma_io_file_handle_t
bcma_io_file_open(const char *filename, const char *mode)
{
    if (filename == NULL || mode == NULL) {
        return NULL;
    }
    if (strlen(filename) == 0 || strlen(mode) == 0) {
        return NULL;
    }
    return (bcma_io_file_handle_t)fopen(filename, mode);
}

int
bcma_io_file_close(bcma_io_file_handle_t fh)
{
    if (fh == NULL) {
        return -1;
    }
    return fclose((FILE *)fh);
}

int
bcma_io_file_read(bcma_io_file_handle_t fh, void *buf, int size, int num)
{
    FILE *f = (FILE *)fh;
    int rv;

    if (fh == NULL || buf == NULL) {
        return -1;
    }

    rv = fread(buf, size, num, f);

    /* Test if error indicator is set */
    if (ferror(f)) {
        return -1;
    }

    return rv;
}

int
bcma_io_file_write(bcma_io_file_handle_t fh, const void *buf, int size, int num)
{
    FILE *f = (FILE *)fh;
    int rv;

    if (fh == NULL || buf == NULL) {
        return -1;
    }

    rv = fwrite(buf, size, num, f);

    /* Test if error indicator is set */
    if (ferror(f)) {
        return -1;
    }

    return rv;
}

int
bcma_io_file_flush(bcma_io_file_handle_t fh)
{
    if (fh == NULL) {
        return -1;
    }
    return fflush((FILE *)fh);
}

int
bcma_io_file_vprintf(bcma_io_file_handle_t fh, const char *format, va_list ap)
{
    if (fh == NULL) {
        return -1;
    }
    return vfprintf((FILE *)fh, format, ap);
}

int
bcma_io_file_size(bcma_io_file_handle_t fh)
{
    int rv;
    FILE *f = (FILE *)fh;

    if (fh == NULL) {
        return -1;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        return -1;
    }
    rv = (int)ftell(f);
    if (fseek(f, 0, SEEK_SET) != 0) {
        return -1;
    }
    return rv < 0 ? -1 : rv;
}

int
bcma_io_file_delete(const char *filename)
{
    if (filename == NULL || strlen(filename) == 0) {
        return -1;
    }
    return unlink(filename);
}

int
bcma_io_file_move(const char *old_filename, const char *new_filename)
{
    if (old_filename == NULL || new_filename == NULL) {
        return -1;
    }
    if (strlen(old_filename) == 0 || strlen(new_filename) == 0) {
        return -1;
    }
    return rename(old_filename, new_filename);
}

int
bcma_io_file_mktemp(char *template)
{
    int rv;

    /* coverity[secure_temp] */
    rv = mkstemp(template);
    if (rv < 0) {
        return -1;
    }
    close(rv);
    return 0;
}
