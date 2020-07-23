/*! \file bcma_io_term_winsize.c
 *
 * Helper function for command line editing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#include <bcma/io/bcma_io_term.h>

#define DEFAULT_IO_TERM_COLUMNS 80
#define DEFAULT_IO_TERM_ROWS    24

/*
 * Get terminal columns from environment variable or default value.
 */
static int
default_columns_get()
{
    int cols = 0;
    char *columns = getenv("COLUMNS");

    if (columns) {
        cols = atoi(columns);
    }
    if (cols <= 0) {
        cols = DEFAULT_IO_TERM_COLUMNS;
    }
    return cols;
}

/*
 * Get terminal rows from environment variable or default value.
 */
static int
default_rows_get()
{
    int rows = 0;
    char *lines = getenv("LINES");

    if (lines) {
        rows = atoi(lines);
    }
    if (rows <= 0) {
        rows = DEFAULT_IO_TERM_ROWS;
    }
    return rows;
}

/*
 * Note that the interface used is not POSIX-compliant.
 */
int
bcma_io_term_winsize_get(int *cols, int *rows)
{
    if (cols == NULL || rows == NULL) {
        return -1;
    }
    *cols = 0;
    *rows = 0;

#ifdef TIOCGWINSZ
    if (isatty(0)) {
        struct winsize ws;

        if (ioctl(0, TIOCGWINSZ, &ws) >= 0) {
            *cols = (int)ws.ws_col;
            *rows = (int)ws.ws_row;
        }
    }
#endif /* TIOCGWINSZ */

    if (*cols <= 0) {
        *cols = default_columns_get();
    }
    if (*rows <= 0) {
        *rows = default_rows_get();
    }

    return 0;
}
