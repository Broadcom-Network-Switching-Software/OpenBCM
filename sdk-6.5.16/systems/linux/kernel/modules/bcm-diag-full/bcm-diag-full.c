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
#include <sal/appl/pci.h>

#include <linux-bde.h>

#include <appl/diag/sysconf.h>
#include <appl/diag/system.h>

#include <bcmx/cosq.h>
#include <bcmx/switch.h>
#include <bcm_int/common/ptp.h>
#include <bcm_int/common/sw_an.h>
#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <soc/tomahawk2.h>
#endif
#include <soc/i2c.h>
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif
#include <bcm_int/esw_dispatch.h>

/* All shell io is done using a user/kernel proxy service. */
#include <linux-uk-proxy.h>



MODULE_AUTHOR("Broadcom Corporation");
MODULE_DESCRIPTION("BCM Diag Shell Full");
MODULE_LICENSE("Proprietary");

/* Debug output */
static int debug;
LKM_MOD_PARAM(debug, "i", int, 0);
MODULE_PARM_DESC(debug,
"Set debug level (default 0)");

/* Create proxy service */
static int proxy = 1;
LKM_MOD_PARAM(proxy, "i", int, 0);
MODULE_PARM_DESC(proxy,
"Create proxy service (default 1)");

static int boot_flags = 0;
LKM_MOD_PARAM(boot_flags, "i", int, 0);
MODULE_PARM_DESC(boot_flags, "boot flags");

/*
 * Linux Module/Library References
 *
 * Not all of the BCM/BCMX functions are referenced by the driver
 * itself. Because of this, some of the orphan functions may not
 * get linked into the module. Since we want these functions to
 * be available to client modules which might be inserted later,
 * we reference them here to make sure they get linked in.
 */
typedef void (*orphan)(void);

static orphan orphans[] = {
#if defined(INCLUDE_BCMX)
    (orphan)bcmx_cosq_config_set,
    (orphan)bcmx_cosq_mapping_set,
    (orphan)bcmx_switch_control_set
#endif /* INCLUDE_BCMX */
};

/* Module Information */
#define MODULE_MAJOR 124
#define MODULE_MINOR 0
#define MODULE_NAME      "linux-bcm-diag-full"
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
static volatile int _bcm_shell_running = 0;
static sal_thread_t _bcm_shell_thread = 0;

extern int
tty_vprintf(const char* fmt, va_list args)
    __attribute__ ((format (printf, 1, 0)));

extern int
tty_printf(const char* fmt, ...)
    __attribute__ ((format (printf, 1, 2)));

/*
 * The system BDE.
 * This BDE can be used by all client modules.
 */
ibde_t* bde;

/* Linux kernel threads must check signals explicitely. */
void
check_exit_signals(void)
{
    if(signal_pending(current)) {
        sal_dpc_term();
        sal_thread_exit(0);
    }
}

/* Proc filesystem information */
static int
_pprint(void)
{
    pprintf("Broadcom Linux BCM Full Diagnostic Shell\n");
    pprintf("\tProxy Service: '%s'\n", PROXY_SERVICE);
    return 0;
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

int
tty_printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    return tty_vprintf(fmt, args);
}

char*
tty_gets(char* dst, int size)
{
    while (linux_uk_proxy_recv(PROXY_SERVICE, dst, (unsigned int *)&size) < 0) {
        sal_sleep(1);
    }
    check_exit_signals();
    return dst;
}

char*
tty_fgets(char* dst, int size, int fh)
{
    while (linux_uk_proxy_recv(PROXY_SERVICE, dst, (unsigned int *)&size) < 0) {
        sal_sleep(1);
    }
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
 * Function: bde_create
 *
 * Purpose:
 *    Create BDE for hardware platform.
 * Parameters:
 *    None
 * Returns:
 *    0 if no errors, otherwise -1.
 */
int
bde_create(void)
{
    linux_bde_bus_t bus;
    bus.be_pio = SYS_BE_PIO;
    bus.be_packet = SYS_BE_PACKET;
    bus.be_other = SYS_BE_OTHER;
    if (debug >= 2) gprintk("BDE create\n");
    return linux_bde_create(&bus, &bde);
}

/*
 * Function: sal_dma_alloc
 *
 * Purpose:
 *    SAL DMA memory allocation function
 * Parameters:
 *    size - number of bytes to allocate
 *    s - string associated with allocation
 * Returns:
 *    Pointer to allocated memory or NULL.
 */
void*
sal_dma_alloc(unsigned int size, char * name)
{
    return bde->salloc(0, size, name);
}

/*
 * Function: sal_dma_free
 *
 * Purpose:
 *    SAL DMA memory free function
 * Parameters:
 *    ptr - pointer to memory allocated memory with sal_dma_alloc
 * Returns:
 *    Nothing
 */
void
sal_dma_free(void *ptr)
{
    bde->sfree(0, ptr);
}

/*
 * When compiled in debug mode (-DBROADCOM_DEBUG) the sal_alloc_purge function
 * will cleanup all memory allocated by sal_alloc. The optional callback
 * function will allow the caller to print information about memory
 * freed by the sal_alloc_purge function.
 */
extern void
sal_alloc_purge(void (*print_func)(void *ptr, unsigned int size, char *s));

/*
 * Function: _purge_print_func
 *
 * Purpose:
 *    Print information about unfreed memory when module is unloaded.
 * Parameters:
 *    ptr - pointer to memory allocated memory with sal_alloc
 *    size - size of allocated memory
 *    s - user description of memory block
 * Returns:
 *    Nothing
 * Notes:
 *    Since this function may print a lot of information (which will
 *    take a while on a 9600 bps serial link), it will only be active
 *    if the module is loaded with the debug flag set to a non-zero
 *    value.
 */
static void
_purge_print_func(void *ptr, unsigned int size, char *s)
{
    if (debug >= 1) {
        gprintk("Freeing %d bytes @ %08lx (%s)\n", size, (unsigned long)ptr, s);
    }
}

/*
 * Generic module functions
 */

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
    if (proxy) {
        linux_uk_proxy_service_create(PROXY_SERVICE, 1, 0);
    } else {
        gprintk("Proxy service disabled\n");
    }
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
    /*
     * Try and close the shell
     */

    if (_bcm_shell_thread) {
        sal_thread_destroy(_bcm_shell_thread);
	sal_usleep(200000);
    }

    if (_bcm_shell_running) {
	sal_usleep(200000);
    }

    linux_uk_proxy_service_destroy(PROXY_SERVICE);

    /* Clean up all unfreed memory (debug mode only) */
    sal_alloc_purge(_purge_print_func);

    return 0;
}

/* Our module vectors */
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
    COMPILER_REFERENCE(orphans);
    return &_gmodule;
}

/*
 * These stubs are here for legacy compatability reasons.
 * They are used only by the diag/test code, not the driver,
 * so they are really not that important.
 */

void pci_print_all(void)
{
}

#ifdef BCM_ESW_SUPPORT
#ifdef INCLUDE_PTP
/*
 * Function:
 *      diag_port_state_description
 * Purpose:
 *      Interpret PTP port state values.
 * Parameters:
 *      state - (IN) PTP port state code.
 * Returns:
 *      Port state description
 * Notes:
 *      Ref. IEEE Std. 1588-2008, Chapter 8.2.5.3.1, Table 8.
 */
STATIC const char*
diag_port_state_description(_bcm_ptp_port_state_t state)
{
    switch(state) {
    case _bcm_ptp_state_initializing:
        return "Init";

    case _bcm_ptp_state_faulty:
        return "Faulty";

    case _bcm_ptp_state_disabled:
        return "Disabled";

    case _bcm_ptp_state_listening:
        return "Listening";

    case _bcm_ptp_state_pre_master:
        return "Pre-Master";

    case _bcm_ptp_state_master:
        return "Master";

    case _bcm_ptp_state_passive:
        return "Passive";

    case _bcm_ptp_state_uncalibrated:
        return "Uncalibrated";

    case _bcm_ptp_state_slave:
        return "Slave";

    default:
        return "<invalid>";
    }
}

EXPORT_SYMBOL(diag_port_state_description);

#endif


EXPORT_SYMBOL(bcm_esw_port_control_get);
#endif /* End BCM_ESW_SUPPORT */

LKM_EXPORT_SYM(check_exit_signals);
LKM_EXPORT_SYM(tty_vprintf);
LKM_EXPORT_SYM(tty_printf);
LKM_EXPORT_SYM(tty_gets);
LKM_EXPORT_SYM(bde_create);
LKM_EXPORT_SYM(sal_dma_alloc);
LKM_EXPORT_SYM(sal_dma_free);
LKM_EXPORT_SYM(sal_alloc_purge);
LKM_EXPORT_SYM(pci_print_all);
LKM_EXPORT_SYM(bde);
#ifdef SW_AUTONEG_SUPPORT
EXPORT_SYMBOL(bcm_sw_an_enable_get);
EXPORT_SYMBOL(bcm_sw_an_enable_set);
EXPORT_SYMBOL(bcm_sw_an_port_diag);
#endif
#if defined(BCM_TOMAHAWK2_SUPPORT)
EXPORT_SYMBOL(soc_tomahawk2_sed_base_index_check);
#endif
EXPORT_SYMBOL(soc_i2c_custom_device_add);
EXPORT_SYMBOL(soc_i2c_custom_device_remove);
EXPORT_SYMBOL(soc_i2c_dev_driver_get);
#if defined(BCM_TRIDENT2_SUPPORT)
EXPORT_SYMBOL(_bcm_td2_cosq_hw_idx_get);
#endif

#ifdef LKM_2_6
/* Export all BCM and BCMX API symbols */
#include <bcm_export.h>
#if defined(INCLUDE_BCMX)
#include <bcmx_export.h>
#endif /* INCLUDE_BCMX */


#if  defined(INCLUDE_MACSEC)
#include <bcm-core.h>
#endif /* INCLUDE_MACSEC */
#endif /* LKM_2_6 */
#if defined(BCM_DFE_SUPPORT) || defined(BCM_DPP_SUPPORT)
extern int _cpu_write(int d, uint32 addr, uint32 *buf);
extern int _cpu_read(int d, uint32 addr, uint32 *buf);
EXPORT_SYMBOL(_cpu_write);
EXPORT_SYMBOL(_cpu_read);
#endif /* defined(BCM_DFE_SUPPORT) || defined(BCM_DPP_SUPPORT) */
#ifdef BCM_DPP_SUPPORT
extern int _cpu_pci_register(int d);
EXPORT_SYMBOL(_cpu_pci_register);
#endif /* BCM_DPP_SUPPORT */
EXPORT_SYMBOL(sal_get_alloc_counters);

#if defined (INCLUDE_REGEX)
#include <appl/diag/cmdlist.h>
EXPORT_SYMBOL(cmd_regex);
EXPORT_SYMBOL(cmd_regex_usage);
#endif /* INCLUDE_REGEX */

