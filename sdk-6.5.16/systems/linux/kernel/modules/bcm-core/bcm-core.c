/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

/*
 * This version of the BCM core driver is written provides the
 * necessary API functions for the linux-bcm-net Linux network
 * device driver to run on top of it.
 *
 * It is also possible to load this module in stand-alone mode
 * in which case all BCM units are initialized when the module
 * is loaded.
 *
 * For a list of supported module parameters, please see below.
 */

#include <gmodule.h> /* Must be included first */
#include <kconfig.h>

#include <shared/bslext.h>

#include <sal/core/boot.h>

#include <appl/diag/sysconf.h>
#include <appl/cpudb/cpudb.h>
#include <appl/cputrans/atp.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cmext.h>
#include <soc/l2x.h>

#include <bcm/init.h>
#include <bcm/error.h>
#include <bcm/port.h>
#include <bcm/link.h>
#include <bcm/stat.h>
#include <bcm/stack.h>
#include <bcm/l2.h>
#include <bcm/rx.h>

#include <ibde.h>
#include <linux-bde.h>
#include <bcm-core.h>

#ifdef BCM_ESW_SUPPORT
#include <soc/phy/phyctrl.h>
#endif


MODULE_AUTHOR("Broadcom Corporation");
MODULE_DESCRIPTION("BCM Core Device Driver");
MODULE_LICENSE("Proprietary");

static char *init = "";
LKM_MOD_PARAM(init, "s", charp, 0); 
MODULE_PARM_DESC(init,
"Optionally initialize BCM (init=bcm) and switching functions (init=all)");

/* Warm Boot (reload) Signalling */
static int warm_boot = 0;
LKM_MOD_PARAM(warm_boot, "i", int, 0);
MODULE_PARM_DESC(warm_boot,
"Optionally indicate device is being warm booted");

/* Shutdown after freeing up resources */
static int clean_shut = 0;
LKM_MOD_PARAM(clean_shut, "i", int, 0);
MODULE_PARM_DESC(clean_shut,
"Optionally indicate graceful shutdown");

/* Debug output */
static int debug;
LKM_MOD_PARAM(debug, "i", int, 0);
MODULE_PARM_DESC(debug,
"Set debug level (default 0)");

static int pci2eb_override = 0;
LKM_MOD_PARAM(pci2eb_override, "i", int, 0);
MODULE_PARM_DESC(pci2eb_override,
        "pci2eb_override property bitmap. Bit-0 corresponds to unit 0");

/* Module Information */
#define MODULE_MAJOR 126
#define MODULE_NAME "linux-bcm-core"

static sal_thread_t _bcm_core_thread;
static sal_sem_t _bcm_core_sem;

static int
_bcore_debug_out(bsl_meta_t *meta, const char* format, va_list args)
    __attribute__ ((format (printf, 2, 0)));

/*
 * The system BDE.
 * This BDE can be used by all client modules.
 */
ibde_t* bde = NULL;

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
void *
sal_dma_alloc(unsigned int size, char *name)
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
 *    We cannot print s (the user description) since the referenced
 *    module may have been unloaded already, thus making s point to
 *    invalid memory.
 */
static void
_purge_print_func(void *ptr, unsigned int size, char *s)
{
    if (debug >= 1) {
        gprintk("Freeing %d bytes @ %08lx\n", size, (unsigned long)ptr);
    }
}

/*
 * Default driver debug output vectors
 */

static int
_bcore_debug_check(bsl_packed_meta_t meta_pack)
{
    int severity = BSL_SEVERITY_GET(meta_pack);

    if (severity <= bslSeverityWarn) {
        return 1;
    }
    return 0;
}

static int
_bcore_debug_out(bsl_meta_t *meta, const char* format, va_list args)
{
    static char _buf[256];

    vsnprintf(_buf, sizeof(_buf)-1, format, args);
    return gprintk("%s", _buf);
}

/*
 * Function: bcore_assert_set_default
 *
 * Purpose:
 *    Set/restore default BCM core driver assert handler.
 * Parameters:
 *    None
 * Returns:
 *    Always 0
 */
int
bcore_debug_set_default(void)
{
    bsl_config_t bsl_config;

    bsl_config_t_init(&bsl_config);
    bsl_config.out_hook = _bcore_debug_out;
    bsl_config.check_hook = _bcore_debug_check;
    bsl_init(&bsl_config);

    return 0;
}

/*
 * Function: _assert
 *
 * Purpose:
 *    Custom handler for assertion failures in the driver.
 * Parameters:
 *    expr - expression that evaluated to FALSE
 *    file - filename of failed assertion
 *    line - linenumber of failed assertion
 * Returns:
 *    Never
 * Notes:
 *    This is not a replacement for the assert function, but
 *    merely a method for reporting the results of a failed
 *    assert statement.
 */
static void
_assert(const char *expr, const char *file, int line)
{
    gprintk("ERROR: Assertion failed: (%s) at %s:%d\n", expr, file, line);
    if (in_interrupt()) {
        gprintk("assert in kernel interrupt code - system halted.\n");
        for (;;) sal_sleep(1000);
    } else {
        gprintk("kernel thread is dead.\n");
        for (;;) sal_sleep(1000);
    }
}

/*
 * Function: bcore_assert_set_default
 *
 * Purpose:
 *    Set/restore default BCM core driver assert handler.
 * Parameters:
 *    None
 * Returns:
 *    Always 0
 */
int
bcore_assert_set_default(void)
{
    sal_assert_set(_assert);
    return 0;
}

/*
 * Function: _startup
 *
 * Purpose:
 *    Main module initialization function.
 * Parameters:
 *    None
 * Returns:
 *    0 if no errors, otherwise -1.
 */
static int
_startup(void)
{
    int	unit;
    int	rv;

    if (sal_core_init() < 0) {
	gprintk("SAL Initialization failed\n");
	return -1;
    }

    bcore_debug_set_default();
    bcore_assert_set_default();

    soc_cm_init();

    if ((rv = bcore_sysconf_probe()) < 0) {
	gprintk("ERROR: PCI device probe failed\n");
	return -1;
    }

    for (unit = 0; unit < bde->num_devices(BDE_ALL_DEVICES); unit++) {
        if (pci2eb_override & (1 << unit)) {
            char cvar_str[24];
            char cvar_num_str[3];
            cvar_num_str[0] = '0' + (unit % 10);
            if (unit / 10) {
                cvar_num_str[1] = '0' + (unit / 10);
            } else {
                cvar_num_str[1] = 0;
            }
            cvar_num_str[2] = 0;
            strcpy(cvar_str, "pci2eb_override.");
            strcpy(&cvar_str[strlen(cvar_str)], cvar_num_str);
            kconfig_set(cvar_str, "1");
        }
    }

    for (unit = 0; unit < bde->num_devices(BDE_ALL_DEVICES); unit++) {
	if (bcore_sysconf_attach(unit) < 0) {
	    gprintk("ERROR: PCI device attach %d failed\n",unit);
	    return -1;
	}
    }

    return 0;
}

/*
 * Function: _bcm_core_shutdown
 *
 * Purpose:
 *    Thread used for shutting down all running BCM threads when
 *    the module is unloaded.
 * Parameters:
 *    None
 * Returns:
 *    Nothing
 */
static void
_bcm_core_shutdown(void* p)
{
    _bcm_core_thread = sal_thread_self();

    /* Ensure that all other module threads exit with this thread */
    sal_thread_main_set(sal_thread_self());

    sal_sem_give(_bcm_core_sem);
}

/*
 * Function: _bcore_cleanup
 *
 * Purpose:
 *    Cleanup threads and resources.
 * Parameters:
 *    None
 * Returns:
 *    Always 0
 * Notes:
 *    All BCM threads will be destroyed to avoid page faults.
 */
static int
_bcore_cleanup(void)
{
    int unit;

    for (unit = 0; unit < bde->num_devices(BDE_ALL_DEVICES); unit++) {
	bcore_sysconf_detach(unit, clean_shut);
    }
    
    /* Create shutdown thread */
    _bcm_core_sem = sal_sem_create("bcm_core_sem", TRUE, 0);
    sal_thread_create("bcm-core-shutdown", 0, 0, _bcm_core_shutdown, 0);

    /* Wait for shutdown thread */
    sal_sem_take(_bcm_core_sem, 2000000);
    if (_bcm_core_thread) {
        sal_thread_destroy(_bcm_core_thread);
        /* Wait for other threads to exit */
        sal_usleep(1000000);
    }

    /* Clean up all unfreed memory (debug mode only) */
    sal_alloc_purge(_purge_print_func);

    return 0;
}

#define SYSTEM_INIT_CHECK(action, description) \
    if ((rv = (action)) < 0) { \
        msg = (description); \
        goto done; \
    }

/*
 * Function: bcore_init
 *
 * Purpose:
 *    Reset and initialize BCM device.
 * Parameters:
 *    unit - StrataSwitch unit #.
 * Returns:
 *    SOC_E_XXX
 */
int
bcore_init(int unit)
{
    soc_port_t		port;
    int			rv;
    sal_usecs_t		usec;
    char		*msg = NULL;
    pbmp_t		pbmp;

    if (!warm_boot) {
        gprintk("Cold boot..\n");
#ifdef BCM_ESW_SUPPORT
        SYSTEM_INIT_CHECK(soc_reset_init(unit), "Device reset");
        SYSTEM_INIT_CHECK(soc_misc_init(unit), "Misc init");
        SYSTEM_INIT_CHECK(soc_mmu_init(unit), "MMU init");
#endif
    } else {
        if (SOC_IS_ESW(unit)) {
            gprintk("Warm boot..\n");
#ifdef BCM_ESW_SUPPORT
            SOC_WARM_BOOT_START(unit);
            SYSTEM_INIT_CHECK(soc_init(unit), "Device init noreset");
#endif
       }
    }

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_arl_hashed)) {
        usec = soc_property_get(unit, spn_L2XMSG_THREAD_USEC, 3000000);
        rv = soc_l2x_start(unit, 0, usec);
        if ((rv < 0) && rv != SOC_E_UNAVAIL) {
            msg = "L2X thread init";
            goto done;
        }
    }
#endif /* BCM_XGS_SWITCH_SUPPORT */

    SYSTEM_INIT_CHECK(bcm_init(unit), "BCM driver layer init");

    if (warm_boot) {
        /*
         * Enable writes to hardware
         */
        SOC_WARM_BOOT_DONE(unit);
    }

    usec = soc_property_get(unit, spn_BCM_LINKSCAN_INTERVAL,
                            BCM_LINKSCAN_INTERVAL_DEFAULT);
    SYSTEM_INIT_CHECK(bcm_linkscan_enable_set(unit, usec), "Linkscan enable");

    pbmp = PBMP_PORT_ALL(unit);

    PBMP_ITER(pbmp, port) {
	int abil;

	SYSTEM_INIT_CHECK(bcm_port_stp_set(
            unit, port, (int)BCM_PORT_STP_FORWARD), "Port Forwarding");

	SYSTEM_INIT_CHECK(bcm_port_ability_get(
            unit, port, (bcm_port_abil_t *)&abil), "Port ability");

	SYSTEM_INIT_CHECK(bcm_port_advert_set(
            unit, port, abil), "Port advert");

	SYSTEM_INIT_CHECK(bcm_port_autoneg_set(
            unit, port, TRUE), "Autoneg enable");

	SYSTEM_INIT_CHECK(bcm_linkscan_mode_set(
            unit, port, BCM_LINKSCAN_MODE_SW), "Linkscan mode set");

	SYSTEM_INIT_CHECK(bcm_stat_clear(
            unit, port), "Stat clear");

	SYSTEM_INIT_CHECK(bcm_port_learn_set(
            unit, port, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD), "learn mode");
    }

 done:
    if (msg != NULL) {
	gprintk("bcore_init: %s failed: %s\n",
                msg, soc_errmsg(rv));
    }

    return rv;
}

/*
 * Function: bcore_init_all
 *
 * Purpose:
 *    Reset and initialize all attached BCM devices.
 * Parameters:
 *    None
 * Returns:
 *    SOC_E_XXX
 */
int
bcore_init_all(void)
{
    int	unit;
    int rv;

    for (unit = 0; unit < bde->num_devices(BDE_ALL_DEVICES); unit++) {
        if ((bde->get_dev_type(unit) & BDE_SWITCH_DEV_TYPE) == 0) {
            continue;
        }
        if ((rv = bcore_init(unit)) < 0) {
            return rv;
        }
    }
    return BCM_E_NONE;
}

/*
 * Linux Module/Library References
 *
 * Not all of the BCM/SOC functions are referenced by the driver
 * itself. Because of this, some of the orphan functions may not
 * get linked into the module. Since we want these functions to
 * be available to client modules which might be inserted later,
 * we reference them here to make sure they get linked in.
 *
 * The functions referenced here are only the ones necessary to
 * run the linux-bcm-net module.
 */
#include <soc/drv.h>
#ifdef BCM_ESW_SUPPORT
#include <soc/er_tcam.h>
#endif
#ifdef INCLUDE_MACSEC
extern int bmacsec_cli_register(void *);
#endif
typedef void (*orphan)(void);

extern int phy_cdMain(int unit, soc_port_t port,
                      soc_port_cable_diag_t *status);
static orphan orphans[] = {
#ifdef BCM_ESW_SUPPORT
    (orphan) soc_bist,
    (orphan) soc_tcam_init,
    (orphan) shr_avl_create,
    (orphan) cpudb_key_parse,
    (orphan) atp_register,
#endif    
#ifdef INCLUDE_MACSEC
    (orphan) bmacsec_cli_register,
#endif
    (orphan) bcm_module_name,  
};

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
    pprintf("Broadcom BCM Core System Module\n");
    /* put some goodies here */
    return 0;
}

/*
 * Function: _init
 *
 * Purpose:
 *    Module initialization.
 *    Attached SOC all devices and optionally initializes these.
 * Parameters:
 *    None
 * Returns:
 *    0 on success, otherwise -1
 */
static int
_init(void)
{
    linux_bde_bus_t bus;
    int rv;
    int	unit;

    bus.be_pio = SYS_BE_PIO;
    bus.be_packet = SYS_BE_PACKET;
    bus.be_other = SYS_BE_OTHER;
    COMPILER_REFERENCE(bus);

    if ((rv = _startup()) < 0) {
	gprintk("_startup() failed: %d\n", rv);
        _bcore_cleanup();
	return -1;
    }

    if (!strcmp(init, "bcm") || !strcmp(init, "all")) {
	if ((rv = bcore_init_all()) < 0) {
	    gprintk("ERROR: Driver initialization failed: %s\n",
		   bcm_errmsg(rv));
            _bcore_cleanup();
	    return -1;
	}
        gprintk("BCM initialized.\n");
    }

    if (!strcmp(init, "all")) {
        for (unit = 0; unit < bde->num_devices(BDE_ALL_DEVICES); unit++) {
            if ((bde->get_dev_type(unit) & BDE_SWITCH_DEV_TYPE) == 0) {
                continue;
            }
            if ((rv = bcm_rx_start(unit, NULL)) < 0) {
                gprintk("ERROR: RX not started: %s\n", bcm_errmsg(rv));
            }
        }
        gprintk("Switching started.\n");
    }

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
 */
static int
_cleanup(void)
{
    _bcore_cleanup();
    return 0;
}

static gmodule_t _gmodule = {
    name: MODULE_NAME,
    major: MODULE_MAJOR,
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


#ifdef LKM_2_6
#include "bcm-core-symbols.h"
#endif /* LKM_2_6 */
