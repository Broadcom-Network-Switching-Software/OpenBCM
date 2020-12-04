/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * l2switch.c: Example L2 Switching Application
 */

#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/appl/pci.h>
#include <sal/appl/config.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cmext.h>

#include <bcm/init.h>
#include <bcm/rx.h>
#include <bcm/error.h>

#include <stdlib.h>


#include <appl/diag/system.h>		/* system_init */

/* Build Information */

extern char *_build_release;
extern char *_build_user;
extern char *_build_host;
extern char *_build_date;
extern char *_build_tree;
extern char *_build_arch;
extern char *_build_os;

/*
 * System Configuration (Config Manager and PCI Device Config)
 */

typedef struct sysconf_pci_dev_s {
    int         cm_dev;

    pci_dev_t	pciDevice;	/* Bus, device, function */
    uint32	pciMemBaseAddr;	/* PCI base memory address of SOC */
    uint16	pciDevID;	/* PCI device ID (0x5600, 0x5800, etc) */
    uint8	pciRevID;	/* PCI revision ID (chip rev) */
    uint8	pciIntPin;
    uint8	pciIntLine;
} sysconf_pci_dev_t;

static sysconf_pci_dev_t	_devices[SOC_MAX_NUM_DEVICES];
static int			_ndevices = 0;

char *
sysconf_get_property(const char *property)
{
    return sal_config_get(property);
}

static char *
_config_var_get(soc_cm_dev_t *dev, const char *property)
{
    return sysconf_get_property(property);
}

static void 
_write(soc_cm_dev_t *dev, uint32 addr, uint32 data)
{
#if defined(PLISIM)
    sysconf_pci_dev_t *soc = &_devices[dev->dev];
    pci_memory_putw(&soc->pciDevice, addr, data);
#else
    *((uint32*)addr) = data;
#endif
}

static uint32 
_read(soc_cm_dev_t *dev, uint32 addr)
{
#if defined(PLISIM)
    sysconf_pci_dev_t *soc = &_devices[dev->dev];
    return pci_memory_getw(&soc->pciDevice, addr);
#else
    return *((uint32*)addr);
#endif
}

static void *
_salloc(soc_cm_dev_t *dev, int size, const char *name)
{
    return sal_dma_alloc(size, (char *)name);
}

static void 
_sfree(soc_cm_dev_t *dev, void *ptr)
{
    sal_dma_free(ptr);
}

static int 
_sflush(soc_cm_dev_t *dev, void *addr, int length)
{
    return sal_dma_flush(addr, length), 0;
}

static int 
_sinval(soc_cm_dev_t *dev, void *addr, int length)
{
    return sal_dma_inval(addr, length), 0;
}

static int
_interrupt_connect(soc_cm_dev_t *dev, soc_cm_isr_func_t handler, void *data)
{
    sysconf_pci_dev_t *pci_dev = (sysconf_pci_dev_t *)dev->cookie;

    LOG_INFO(BSL_LS_SOC_PCI,
             (BSL_META("sysconf_probe: connecting INT%c# to IRQ %d\n"),
              'A' + pci_dev->pciIntPin - 1, pci_dev->pciIntLine));

    return pci_int_connect(pci_dev->pciIntLine, (pci_isr_t) handler, data);
}

static int 
_interrupt_disconnect(soc_cm_dev_t *dev)
{
    return 1;
}

/*
 * Function:
 *	sysconf_chip_override
 * Purpose:
 *	For driver testing only, allow properties to override the PCI
 *	device ID/rev ID for a particular unit to specified values.
 * Parameters:
 *	unit - Unit number
 *	devID, revID - (IN, OUT) original PCI ID info
 */

void
sysconf_chip_override(int unit, uint16 *devID, uint8 *revID)
{
    char		prop[64], *s;

    sprintf(prop, "pci_override_dev.%d", unit);

    if ((s = sysconf_get_property(prop)) == NULL) {
	s = sysconf_get_property("pci_override_dev");
    }

    if (s != NULL) {
	*devID = sal_ctoi(s, 0);
    }

    sprintf(prop, "pci_override_rev.%d", unit);

    if ((s = sysconf_get_property(prop)) == NULL) {
	s = sysconf_get_property("pci_override_rev");
    }

    if (s != NULL) {
	*revID = sal_ctoi(s, 0);
    }
}

/*
 * _setup_pci
 *
 *    Utility routine used by sysconf_probe
 */

static int
_setup_pci(int unit)
{
    sysconf_pci_dev_t	*soc	= &_devices[unit];
    pci_dev_t		*dev	= &soc->pciDevice;
    uint16		driverDevID;
    uint16		driverRevID;
  
    soc_cm_get_id_driver(soc->pciDevID, soc->pciRevID,
			 &driverDevID, &driverRevID);

    LOG_CLI((BSL_META_U(unit,
                        "PCI Discovery: found unit %d: %s "
                        "on bus %d, dev 0x%x, fn %d\n"
                        "Using driver: %s\n"),		 
             unit,
             soc_cm_get_device_name(soc->pciDevID, soc->pciRevID),
             dev->busNo, dev->devNo, dev->funcNo,
             soc_cm_get_device_name(driverDevID, driverRevID)));

    /* Write control word (turns on parity detect) */

    pci_config_putw(dev, PCI_CONF_COMM, (PCI_COMM_ME |
					 PCI_COMM_MAE |
					 PCI_COMM_PARITY_DETECT));

    /*
     * On platforms where the underlying OS or BIOS has not
     * automatically assigned a value to the StrataSwitch MBAR and/or
     * intLine, we need to write them here.  PCI_SOC_MBAR(unit) and
     * PCI_SOC_INTLINE(unit) are defined in <sal/appl/pci.h> for these
     * platforms, which include PLISIM, Sandpoint, Mousse, and IDT.
     */

#ifdef PCI_SOC_MBAR
    pci_config_putw(dev, PCI_CONF_BASE0, PCI_SOC_MBAR(unit));
    LOG_INFO(BSL_LS_SOC_PCI,
             (BSL_META_U(unit,
                         "Set PCI MBAR 0x%x\n"), PCI_SOC_MBAR(unit)));
#endif
  
#ifdef PCI_SOC_INTLINE
    {
	uint32 tmp = pci_config_getw(dev, PCI_CONF_ILINE);
	tmp = (tmp & ~0xff) | PCI_SOC_INTLINE(unit);
	pci_config_putw(dev, PCI_CONF_ILINE, tmp);
    }
#endif
  
    soc->pciMemBaseAddr =
	pci_config_getw(dev, PCI_CONF_BASE0) & PCI_MBASE_MASK;
  
    LOG_INFO(BSL_LS_SOC_PCI,
             (BSL_META_U(unit,
                         "Read back PCI MBAR 0x%x\n"), soc->pciMemBaseAddr));
  
    /*
     * Set # retries to infinite.  Otherwise other devices using the bus
     * may monopolize it long enough for us to time out.
     */

    pci_config_putw(dev, PCI_CONF_TRDY_TO, 0x0080);

    /*
     * Sanity check to ensure first CMIC register is accessible.
     */
  
    if (sal_memory_check(soc->pciMemBaseAddr) < 0) {
	LOG_CLI((BSL_META_U(unit,
                            "sysconf_probe: unable to probe address 0x%x\n"),
                 soc->pciMemBaseAddr));
	return -1;
    }
  
    /*
     * Hook up PCI device interrupt to our ISR.  The interrupt number is
     * determined by reading the value configured by the BIOS from PCI
     * config space.  The SOC uses PCI pin INTA#.
     */
  
    soc->pciIntLine = pci_config_getw(dev, PCI_CONF_ILINE) >> 0 & 0xff;
    soc->pciIntPin  = pci_config_getw(dev, PCI_CONF_ILINE) >> 8 & 0xff;
  
    return 0;
}

STATIC int
_sysconf_probe_dev(pci_dev_t *dev,
		   uint16 venID, uint16 devID, uint8 revID)
{
    sysconf_pci_dev_t	*cookie;
    int			device_handle;
    int			unit;

    unit = _ndevices;

    sysconf_chip_override(unit, &devID, &revID);

    if (venID != BROADCOM_VENDOR_ID ||
	soc_cm_device_supported(devID, revID) < 0) {
	/* Not a switch chip; continue probing other devices */
	return 0;
    }

    if (unit == SOC_MAX_NUM_DEVICES) {
	/* Already found max chips; stop probing */
	return -1;
    }

    cookie = &_devices[unit];

    sal_memcpy(&cookie->pciDevice, dev, sizeof (pci_dev_t));

    cookie->pciDevID = devID;
    cookie->pciRevID = revID;

    if (_setup_pci(unit) < 0) {
	/* Error message already printed; continue probing other devices */
	return 0;
    }

    /* Create a device handle, but don't initialize yet */

    device_handle = soc_cm_device_create(cookie->pciDevID, 
					 cookie->pciRevID,
					 cookie);
    assert(device_handle >= 0);
    cookie->cm_dev = device_handle;

    _ndevices++;

    return 0;
}

/*
 * sysconf_probe
 *
 * Probes PCI bus for available units (devices), sets soc_ndev to the
 * number of units found, and fills in the following components of the
 * sysconf_pci_dev_t driver structure for each unit:
 *
 *	pciDevice (bus, device, function)
 *	pciMemBaseAddr
 *	pciIntLine
 *	pciIntPin
 *
 * Each device's PCI memory space is mapped and interrupts are enabled.
 * No chip initialization is performed (see soc_attach and soc_init).
 */

int 
sysconf_probe(void)
{
    int		rv = -1;

    LOG_INFO(BSL_LS_SOC_PCI,
             (BSL_META("sysconf_probe: checking for SOC devices\n")));

    if (_ndevices > 0) {
	LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("sysconf_probe: probe already done\n")));
	goto done;		/* Should never probe more than once */
    }

    pci_device_iter(_sysconf_probe_dev);

    if (_ndevices == 0) {
	LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("WARNING: No StrataSwitch unit found.\n")));
	goto done;
    }

    rv = 0;

 done:
    LOG_INFO(BSL_LS_SOC_PCI,
             (BSL_META("soc_probe: rv=%d\n"), rv));
    return rv;
}

int 
sysconf_attach(int unit)
{
    /* Ready to install into configuration manager */

    soc_cm_device_vectors_t vectors;
    sysconf_pci_dev_t* cookie;
  
    assert(unit >= 0 && unit < _ndevices);
  
    cookie = &_devices[unit];
  
    vectors.big_endian_pio = 0;
    vectors.big_endian_packet = 0;
    vectors.big_endian_other = 0;
  
#if !defined(IDTRP334)
# if !defined(LE_HOST)
    vectors.big_endian_other = 1;
#  ifdef COLDFIRE_NATIVE_INTERFACE
    vectors.big_endian_packet = 1;
#  endif
# endif /* !defined(LE_HOST) */
#endif
  
#if defined(IDTRP334)
    vectors.big_endian_packet = 1;
#endif

    vectors.config_var_get = _config_var_get;
    vectors.interrupt_connect = _interrupt_connect;
    vectors.interrupt_disconnect= _interrupt_disconnect;
    vectors.base_address = cookie->pciMemBaseAddr;
    vectors.read = _read;
    vectors.write = _write;
    vectors.salloc = _salloc;
    vectors.sfree = _sfree;
    vectors.sinval = _sinval;
    vectors.sflush = _sflush;
    vectors.l2p = NULL;
    vectors.p2l = NULL;

    if (soc_cm_device_init(cookie->cm_dev, &vectors) < 0) {
	LOG_CLI((BSL_META_U(unit,
                            "sysconf_attach: bcm device init failed\n")));
	return -1;
    }

    return 0;
}

int
sysconf_detach(int unit)
{
    assert(unit >= 0 && unit < _ndevices);
    return soc_cm_device_destroy(unit);
}

pci_dev_t *
sysconf_get_pci_device(int unit)
{
    return &_devices[unit].pciDevice;
}

static int
_sysconf_debug_out(uint32 flags, const char *format, va_list args)
{
    return (flags) ? vdebugk(flags, format, args) : vprintk(format, args);
}

static int
_sysconf_dump(soc_cm_dev_t *cookie)
{
    sysconf_pci_dev_t *dev = (sysconf_pci_dev_t *)cookie->cookie;

    LOG_CLI((BSL_META("PCI: Base=0x%x Bus=%d Dev=0x%x iPin=%d iLine=%d\n"),
             dev->pciMemBaseAddr,
             dev->pciDevice.busNo, dev->pciDevice.devNo,
             dev->pciIntPin, dev->pciIntLine));

    return 0;
}

int
sysconf_init(void)
{
    soc_cm_init_t init_data;

    _ndevices = 0;

    init_data.debug_out = _sysconf_debug_out;
    init_data.debug_check = debugk_check;
    init_data.debug_dump = _sysconf_dump;

    return soc_cm_init(&init_data);
}

#if 0
/*
 * Custom handler for assertion failures in the driver
 */

void _diag_assert(const char *expr, const char *file, int line)
{
#ifdef VXWORKS
    extern void sysReboot(void);
# if defined(MOUSSE) || defined(BMW)
    extern void sysBacktracePpc(char *pfx, char *sfx, int direct);
# endif
#endif
    int			int_ctxt;

#ifdef UNIX
    /*
     * You can have assertion failures call abort() when you're in GDB
     * by adding the following to your ~/.gdbinit: set environment GDB 1
     */

    if (getenv("GDB"))
	abort();
#endif

    LOG_CLI((BSL_META("ERROR: Assertion failed: (%s) at %s:%d\n"), expr, file, line));

    int_ctxt = sal_int_context();

#ifdef VXWORKS
# if defined(MOUSSE) || defined(BMW)
    sysBacktracePpc("ERROR: Stack trace follows\n", "", int_ctxt);
# endif
    if (int_ctxt)
	sysReboot();
#endif /* VXWORKS */

    /*
     * Allow user to continue or return to command prompt.
     */

    LOG_CLI((BSL_META("WARNING: continuing: correct behavior no longer guaranteed!\n")));
}
#endif

bcm_rx_t
packet_process(int unit, bcm_pkt_t *pkt, void *cookie)
{
    int		i;
    uint8	*p;

    LOG_CLI((BSL_META_U(unit,
                        "=== Packet received: port %d length %d ===\n"),
             pkt->rx_port, pkt->tot_len));

    p = BCM_PKT_IEEE(pkt);
    for (i = 0; i < pkt->tot_len; i++) {
	LOG_CLI((BSL_META_U(unit,
                            "%02x%s"), p[i], (i % 16) == 15 ? "\n" : " "));
    }
    if ((pkt->tot_len % 16) != 0) {
	LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }
    return BCM_RX_HANDLED;
}

/*
 * Example L2 Switching Application
 * Main entry point
 */

int main(int argc, char *argv[])
{
    int			unit = 0;
    int			rv;

    if (sal_core_init() < 0 || sal_appl_init() < 0) {
	LOG_CLI((BSL_META_U(unit,
                            "SAL Initialization failed\n")));
	exit(1);
    }

#if 0
    sal_assert_set(_diag_assert);
#endif

#ifdef UNIX
    if (getenv("DK"))
	debugk_enable(sal_ctoi(getenv("DK"), 0));
#endif

    LOG_CLI((BSL_META_U(unit,
                        "Example L2 Switching Application\n")));
    LOG_CLI((BSL_META_U(unit,
                        "Version %s built %s\n"), _build_release, _build_date));

    sysconf_init();

    if ((rv = sysconf_probe()) < 0) {
	LOG_CLI((BSL_META_U(unit,
                            "ERROR: PCI device probe failed\n")));
	return -1;
    }

    unit = 0;

    LOG_CLI((BSL_META_U(unit,
                        "Attaching unit %d...\n"), unit));

    if (sysconf_attach(unit) < 0) {
	LOG_CLI((BSL_META_U(unit,
                            "ERROR: PCI device attach failed\n")));
	return -1;
    }

    LOG_CLI((BSL_META_U(unit,
                        "Initializing unit %d...\n"), unit));

    if ((rv = system_init(unit)) < 0) {
	LOG_CLI((BSL_META_U(unit,
                            "ERROR: Driver initialization failed: %s\n"), bcm_errmsg(rv)));
	return -1;
    }

    LOG_CLI((BSL_META_U(unit,
                        "Initializing packet receiver...\n")));

    rv = bcm_rx_start(unit, NULL);
    if (rv < 0) {
	LOG_CLI((BSL_META_U(unit,
                            "ERROR: Packet receiver not started: %s\n"), bcm_errmsg(rv)));
    }

    rv = bcm_rx_register(unit, "l2switch", packet_process,
			 BCM_RX_PRIO_MAX, (void *)0, BCM_RCO_F_ALL_COS);
    if (rv < 0) {
	LOG_CLI((BSL_META_U(unit,
                            "ERROR: Packet monitor registration failed: %s\n"),
                 bcm_errmsg(rv)));
    }

    LOG_CLI((BSL_META_U(unit,
                        "Switching started.\n")));

    for (;;) {
	/* Process packets and stuff */
	sal_usleep(250000);
    }

    /* NOTREACHED */
    return 0;
}
