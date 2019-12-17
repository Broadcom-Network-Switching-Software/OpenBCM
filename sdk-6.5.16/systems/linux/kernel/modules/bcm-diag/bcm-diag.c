/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

#include <gmodule.h> /* Must be included first */

#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <sal/core/dpc.h>
#include <sal/appl/sal.h>
#include <soc/portmod/portmod_internal.h>

#include <linux-bde.h>

#include <appl/diag/sysconf.h>
#include <appl/diag/system.h>

#include <bcm-core.h>

/* All shell io is done using a user/kernel proxy service. */
#include <linux-uk-proxy.h>


MODULE_AUTHOR("Broadcom Corporation");
MODULE_DESCRIPTION("BCM Diag Shell");
MODULE_LICENSE("Proprietary");

/* Debug output */
static int debug;
LKM_MOD_PARAM(debug, "i", int, 0);
MODULE_PARM_DESC(debug,
"Set debug level (default 0)");

static int boot_flags = 0;
LKM_MOD_PARAM(boot_flags, "i", int, 0);
MODULE_PARM_DESC(boot_flags, "boot flags");

/* Module Information */
#define MODULE_MAJOR 124
#define MODULE_MINOR 0
#define MODULE_NAME      "linux-bcm-diag"
#define PROXY_SERVICE	 "BCM DIAG SHELL"

/* Maximum string we can handle in printf */
#define PROXY_STRING_MAX (LUK_MAX_DATA_SIZE * 6)

/* Message buffer */
#define DBUF_DATA_SIZE 128
typedef struct _dbuf_t {
    char data[DBUF_DATA_SIZE];
} dbuf_t;
#define DBUF_DATA(dbuf) dbuf->data

/* Buffer console messages from interrupt context */
#define DBUF_CNT_MAX 32
static dbuf_t dbuf[DBUF_CNT_MAX];
static dbuf_t dbuf_flush[DBUF_CNT_MAX];
static volatile int dbuf_cnt;
static spinlock_t dbuf_lock;
static struct semaphore dbuf_sem;

/* Thread control flags */
static volatile int _bcm_shell_running;
static sal_thread_t _bcm_shell_thread;
static sal_thread_t orig_main_thread;

extern int 
tty_vprintf(const char* fmt, va_list args)
    __attribute__ ((format (printf, 1, 0)));

extern int
tty_printf(const char* fmt, ...)
    __attribute__ ((format (printf, 1, 2)));

/* Linux kernel threads must check signals explicitely. */
static void
check_exit_signals(void)
{
    if(signal_pending(current)) {
        sal_dpc_term();
        sal_thread_exit(0);
    }
}

static void
_tty_flush(void *p1, void *p2, void *p3, void *p4, void *p5)
{
    int cnt;
    int dbuf_flush_cnt;
    unsigned long flags;
    dbuf_t *d;
    char *p;

    /* Prevent flushing from multiple threads simultaneously */
    if (down_interruptible(&dbuf_sem) != 0) {
        /* Something's wrong */
        return;
    }

    /* Copy pending data to flush buffer */
    spin_lock_irqsave(&dbuf_lock, flags);
    memcpy(dbuf_flush, dbuf, dbuf_cnt * sizeof(dbuf_t));
    dbuf_flush_cnt = dbuf_cnt;
    dbuf_cnt = 0;
    spin_unlock_irqrestore(&dbuf_lock, flags);

    /* Note that we may sleep during flush */
    for (cnt = 0; cnt < dbuf_flush_cnt; cnt++) {
        d = &dbuf_flush[cnt];
        p = DBUF_DATA(d);
        linux_uk_proxy_send(PROXY_SERVICE, p, strlen(p));
    }

    up(&dbuf_sem);
}

int 
tty_vprintf(const char* fmt, va_list args)
{
    int cnt, tmp_cnt, offset=0; 
    unsigned long flags;
    static char s[PROXY_STRING_MAX]; 

    if (sal_no_sleep()) {
        /* Schedule flush function */
        if (dbuf_cnt == 0) {
            sal_dpc(_tty_flush, 0, 0, 0, 0, 0);
        }
        /* Buffer message */
        spin_lock_irqsave(&dbuf_lock, flags);
        if (dbuf_cnt < DBUF_CNT_MAX) {
            dbuf_t *d = &dbuf[dbuf_cnt++];
            vsnprintf(DBUF_DATA(d), DBUF_DATA_SIZE-1, fmt, args);
        }
        spin_unlock_irqrestore(&dbuf_lock, flags);
        return 0;
    }

    if (dbuf_cnt) {
        /* Flush buffered messages */
        _tty_flush(0, 0, 0, 0, 0);
    }

    tmp_cnt = cnt = vsnprintf(s, PROXY_STRING_MAX - 1, fmt, args);
    if (tmp_cnt >= PROXY_STRING_MAX) {
        tmp_cnt = PROXY_STRING_MAX;
    }
    while (tmp_cnt > 0) {
        linux_uk_proxy_send(PROXY_SERVICE, &s[offset],
                   (tmp_cnt < LUK_MAX_DATA_SIZE) ? tmp_cnt : LUK_MAX_DATA_SIZE);
        tmp_cnt -= LUK_MAX_DATA_SIZE;
        offset += LUK_MAX_DATA_SIZE;
    }
    check_exit_signals();
    return cnt; 
}

/*
 * Function: tty_printf
 *
 * Purpose:
 *    Application-specific console output function.
 * Parameters:
 *    Same as standard C printf.
 * Returns:
 *    Same as standard C printf.
 */
int
tty_printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    return tty_vprintf(fmt, args);
}

/*
 * Function: tty_gets
 *
 * Purpose:
 *    Application-specific console input function.
 * Parameters:
 *    Same as standard C fgets.
 * Returns:
 *    Same as standard C fgets.
 */
char *
tty_gets(char* dst, int size)
{
    linux_uk_proxy_recv(PROXY_SERVICE, dst, (unsigned int *)&size); 
    check_exit_signals();
    return dst; 
}

/*
 * Function: _bcm_shell
 *
 * Purpose:
 *    Thread entry used to run an instance of the BCM diag shell
 * Parameters:
 *    None
 * Returns:
 *    Nothing
 */
static void
_bcm_shell(void* p)
{
    if (sal_core_init() < 0 || sal_appl_init() < 0) {
	gprintk("SAL Initialization failed\n");
	sal_thread_exit(0); 
    }
    _bcm_shell_thread = sal_thread_self();

    if (boot_flags) {
        sal_boot_flags_set(boot_flags);
    }

    if (debug >= 1) gprintk("BCM Diag Module Initialized. Starting proxy...\n"); 

    /* A small delay here prevents the telnet proxy 
     * from choking on the first command.
     */
    sal_usleep(100*1000);
    
    _bcm_shell_running = 1; 
    if (debug >= 1) gprintk("Starting Diag Shell...\n"); 
    diag_shell();
    if (debug >= 1) gprintk("Diag Shell is done.\n"); 
    sal_dpc_term();
    linux_bde_destroy(bde);
    _bcm_shell_running = 0; 
}

/*
 * Generic module functions
 */

/*
 * Function: _pprint
 *
 * Purpose:
 *    Print proc filesystem information.
 * Parameters:
 *    None
 * Returns:
 *    Always 0
 */
static int
_pprint(void)
{	
    pprintf("Broadcom Linux BCM Diagnostic Shell\n"); 
    pprintf("\tProxy Service: '%s'\n", PROXY_SERVICE); 
    return 0;
}

/*
 * Function: _init
 *
 * Purpose:
 *    Module initialization.
 *    Starts the BCM diag thread. 
 * Parameters:
 *    None
 * Returns:
 *    Always 0
 */
static int
_init(void)
{
    spin_lock_init(&dbuf_lock);
    sema_init(&dbuf_sem, 1);
    orig_main_thread = sal_thread_main_get();
    linux_uk_proxy_service_create(PROXY_SERVICE, 1, 0); 
    sal_thread_create("bcm-shell", 0, 0, _bcm_shell, 0);
    return 0;
}

/*
 * Function: _cleanup
 *
 * Purpose:
 *    Module cleanup function
 * Parameters:
 *    None
 * Returns:
 *    Always 0
 * Notes:
 *    The BCM diag thread will be destroyed to avoid page faults.
 */
static int
_cleanup(void)
{
    /* Restore debug print vectors */
    bcore_debug_set_default();

    /* Restore assert handler */
    bcore_assert_set_default();

    /* Close the shell */
    if (_bcm_shell_thread) {
        sal_thread_destroy(_bcm_shell_thread);
	sal_usleep(200000);
    }
    /* This should only be relavant on a really busy system */
    if (_bcm_shell_running) {
	sal_usleep(200000);
    }

    linux_uk_proxy_service_destroy(PROXY_SERVICE);

    /* Restore main thread */
    sal_thread_main_set(orig_main_thread);

    return 0;
}	

/* Module vectors */
static gmodule_t _gmodule = {
    name: MODULE_NAME, 
    major: MODULE_MAJOR, 
    minor: MODULE_MINOR, 
    init: _init,
    cleanup: _cleanup, 
    pprint: _pprint, 
    ioctl: NULL,
    open: NULL, 
    close: NULL, 
}; 

gmodule_t*
gmodule_get(void)
{
#ifdef LKM_2_4
    EXPORT_NO_SYMBOLS;
#endif
    return &_gmodule;
}

#ifdef LKM_2_6
EXPORT_SYMBOL(tty_vprintf);
EXPORT_SYMBOL(tty_printf);
EXPORT_SYMBOL(tty_gets);
#endif

#if defined(INCLUDE_REGEX)
#include <appl/diag/cmdlist.h>
EXPORT_SYMBOL(cmd_regex);
EXPORT_SYMBOL(cmd_regex_usage);
#endif /* INCLUDE_REGEX */

EXPORT_SYMBOL(portmod_pm_info_get);
