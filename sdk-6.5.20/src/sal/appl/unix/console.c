/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	console.c
 * Purpose:	User TTY I/O
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <sal/appl/io.h>

static sal_mutex_t console_mutex;

/*
 * sal_console_init
 */

int
sal_console_init(void)
{
    console_mutex = sal_mutex_create("console mutex");

    return 0;
}

/*
 * Function:
 *	sal_console_info
 * Purpose:
 *	Get console information
 * Parameters:
 *	info - console info
 * Returns:
 *      0 on success
 *      -1 if stdin is not a tty
 */

int
sal_console_info_get(sal_console_info_t *info)
{
#if	defined(TIOCGWINSZ)
    struct winsize	W;
#endif	/* defined(TIOCGWINSZ) */

    if (!isatty(0)) {
        return -1;
    }
    if (info == NULL) {
        return 0;
    }
    memset(info, 0, sizeof(*info));
#if	defined(TIOCGWINSZ)
    if (ioctl(0, TIOCGWINSZ, &W) >= 0) {
	info->cols = (int)W.ws_col;
	info->rows = (int)W.ws_row;
    }
#endif	/* defined(TIOCGWINSZ) */
    return 0;
}

/*
 * Function:
 *	sal_console_write
 * Purpose:
 *	Write characters to console
 * Parameters:
 *	buf - buffer to write
 *	count - number of characters in buffer
 */

int
sal_console_write(const void *buf, int count)
{
    return write(0, buf, count);
}

/*
 * Function:
 *	sal_console_read
 * Purpose:
 *	Read characters from console
 * Parameters:
 *	buf - buffer to fill
 *	count - number of characters to read
 */

int
sal_console_read(void *buf, int count)
{
    return read(0, buf, count);
}

/*
 * Function:
 *	sal_console_gets
 * Purpose:
 *	Read line from console
 * Parameters:
 *	buf - input buffer
 *	size - size of input buffer
 */

char *
sal_console_gets(char *buf, int size)
{
    char *p = fgets(buf, size, stdin);
    if (p == NULL) {
	clearerr(stdin);
    }
    return p;
}

#ifndef SDK_CONFIG_SAL_VPRINTF
/*
 * sal_vprintf
 *
 *   This is the base routine upon which all standard printing is built.
 */

int sal_vprintf(const char *fmt, va_list varg)
{
    int			retv;
#ifndef	SAL_THREAD_NAME_PRINT_DISABLE
    char		thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t	thread;

    thread = sal_thread_self();
    thread_name[0] = 0;

    if (thread != sal_thread_main_get()) {
	sal_thread_name(thread, thread_name, sizeof (thread_name));
    }
#endif	/* !SAL_THREAD_NAME_PRINT_DISABLE */

    sal_mutex_take(console_mutex, sal_mutex_FOREVER);

#ifndef	SAL_THREAD_NAME_PRINT_DISABLE
    if (thread_name[0] != 0) {
	(void) printf("[%s]", thread_name);
    }
#endif	/* !SAL_THREAD_NAME_PRINT_DISABLE */

    retv = vprintf(fmt, varg);

    fflush(stdout);

    sal_mutex_give(console_mutex);

    return retv;
}
#endif /* !SDK_CONFIG_SAL_VPRINTF */

int sal_printf(const char *fmt, ...)
{
    int retv;
    va_list varg;
    va_start(varg, fmt);
    retv = sal_vprintf(fmt, varg);
    va_end(varg);
    return retv;
}
