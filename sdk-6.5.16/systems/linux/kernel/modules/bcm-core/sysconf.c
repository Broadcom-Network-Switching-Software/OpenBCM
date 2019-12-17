/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */
#include <gmodule.h> /* Must be included first */
#include <linux-bde.h>
#include <sal/types.h>

#include <soc/cm.h>
#include <soc/cmext.h>
#include <soc/drv.h>

#include <bcm/init.h>

#include <ibde.h>

#define  BCM_SYSCONF_INC /* needed for MACSEC */
#include <bcm-core.h>

#include <kconfig.h>

static int bcore_sysconf_probe_done;
static int bcore_sysconf_cm_dev[LINUX_BDE_MAX_DEVICES];

extern int bde_create(void);

/* SOC Configuration Manager device vectors */

static char *
_config_var_get(soc_cm_dev_t *dev, const char *property)
{
    COMPILER_REFERENCE(dev);

    return kconfig_get(property);
}

static void
_write(soc_cm_dev_t *dev, uint32 addr, uint32 data)
{
    bde->write(dev->dev, addr, data);
}

static uint32
_read(soc_cm_dev_t *dev, uint32 addr)
{
    return bde->read(dev->dev, addr);
}

static void
_pci_conf_write(soc_cm_dev_t *dev, uint32 addr, uint32 data)
{
    bde->pci_conf_write(dev->dev, addr, data);
}

static uint32
_pci_conf_read(soc_cm_dev_t *dev, uint32 addr)
{
    return bde->pci_conf_read(dev->dev, addr);
}

static void *
_salloc(soc_cm_dev_t *dev, int size, const char *name)
{
    COMPILER_REFERENCE(name);
    return bde->salloc(dev->dev, size, name);
}

static void
_sfree(soc_cm_dev_t *dev, void *ptr)
{
    bde->sfree(dev->dev, ptr);
}

static int
_sflush(soc_cm_dev_t *dev, void *addr, int length)
{
    return (bde->sflush) ? bde->sflush(dev->dev, addr, length) : 0;
}

static int
_sinval(soc_cm_dev_t *dev, void *addr, int length)
{
    return (bde->sinval) ? bde->sinval(dev->dev, addr, length) : 0;
}

static uint32 
_l2p(soc_cm_dev_t *dev, void *addr)
{
    return (bde->l2p) ? bde->l2p(dev->dev, addr) : 0;
}

static void*
_p2l(soc_cm_dev_t *dev, uint32 addr)
{
    return (bde->p2l) ? bde->p2l(dev->dev, addr) : 0;
}

static int
_interrupt_connect(soc_cm_dev_t *dev, soc_cm_isr_func_t handler, void *data)
{
    return bde->interrupt_connect(dev->dev, handler, data);
}

static int
_interrupt_disconnect(soc_cm_dev_t *dev)
{
    return bde->interrupt_disconnect(dev->dev);
}

static int
_spi_read(soc_cm_dev_t *dev, uint32 addr, uint8 *buf, int len)
{
    return (bde->spi_read) ? bde->spi_read(dev->dev, addr, buf, len) : -1;
}

static int
_spi_write(soc_cm_dev_t *dev, uint32 addr, uint8 *buf, int len)
{
    return (bde->spi_write) ? bde->spi_write(dev->dev, addr, buf, len) : -1;
}

/*
 * Function: bcore_sysconf_probe
 *
 * Purpose:
 *    Searches for known devices and creates Configuration 
 *    Manager instances.
 * Parameters:
 *    None
 * Returns:
 *    Always 0
 */
int
bcore_sysconf_probe(void)
{
    int u;
    int cm_dev = 0;
    uint16 devID;
    uint8 revID;

    if (bcore_sysconf_probe_done) {
	return -1;
    }
    bcore_sysconf_probe_done = 1;

    /* Initialize system BDE */
    if (bde_create()) {
	return -1;
    }

    /* Mark all devices as unused */
    for (u = 0; u < COUNTOF(bcore_sysconf_cm_dev); u++) {
        bcore_sysconf_cm_dev[u] = -1;
    }

    /* Iterate over devices */
    for (u = 0; u < bde->num_devices(BDE_ALL_DEVICES) && u < SOC_MAX_NUM_DEVICES; u++) {
	const ibde_dev_t *dev = bde->get_dev(u);
	devID = dev->device;
	revID = dev->rev;

	if (soc_cm_device_supported(devID, revID) < 0) {
	    /* Unsupported device; continue probing other devices */
	    continue;
	}

	/* Create a device handle, but don't initialize yet. */
	cm_dev = soc_cm_device_create(devID, revID, NULL);

	if (cm_dev < 0) {
            gprintk("attach: invalid CM device.\n");
            return -1;
        }

        bcore_sysconf_cm_dev[u] = cm_dev;
    }

    return 0;
}

/*
 * Function: bcore_sysconf_attach
 *
 * Purpose:
 *    Install SOC Configuration Manager device vectors and
 *    initialize SOC device.
 * Parameters:
 *    unit - SOC device
 * Returns:
 *    SOC_E_XXX
 */
int
bcore_sysconf_attach(int dev)
{
    soc_cm_device_vectors_t vectors;
    int unit;

    if (dev < 0 || dev >= COUNTOF(bcore_sysconf_cm_dev)) {
        gprintk("attach: dev out of range %d\n", dev);
        return -1;
    }

    unit = bcore_sysconf_cm_dev[dev];
    if (unit < 0) {
        /* Unsupported device */
        return 0;
    }

    bde->pci_bus_features(unit,
                          &vectors.big_endian_pio,
			  &vectors.big_endian_packet,
			  &vectors.big_endian_other);

    vectors.config_var_get = _config_var_get;
    vectors.interrupt_connect = _interrupt_connect;
    vectors.interrupt_disconnect= _interrupt_disconnect;
#if defined(PCI_DECOUPLED) || defined(BCM_ICS)
    vectors.base_address = 0;
#else
    vectors.base_address = bde->get_dev(unit)->base_address;
#endif
    vectors.read = _read;
    vectors.write = _write;
    vectors.pci_conf_read = _pci_conf_read;
    vectors.pci_conf_write = _pci_conf_write;
    vectors.salloc = _salloc;
    vectors.sfree = _sfree;
    vectors.sinval = _sinval;
    vectors.sflush = _sflush;
    vectors.l2p = (void *)_l2p;
    vectors.p2l = (void *)_p2l;
    vectors.bus_type = bde->get_dev_type(unit);
    vectors.spi_read = _spi_read;
    vectors.spi_write = _spi_write;
    /* Initialize default config values */
    kconfig_set("os", "linux");

    return soc_cm_device_init(unit, &vectors);
}

/*
 * Function: bcore_sysconf_attach
 *
 * Purpose:
 *    Shut down device
 * Parameters:
 *    dev - BDE device
 * Returns:
 *    SOC_E_XXX
 */
int
bcore_sysconf_detach(int dev, int clean_shut)
{
    int unit;

    if (dev < 0 || dev >= COUNTOF(bcore_sysconf_cm_dev)) {
        gprintk("detach: dev out of range %d\n", dev);
        return -1;
    }

    unit = bcore_sysconf_cm_dev[dev];
    if (unit < 0) {
        /* Unsupported device */
        return 0;
    }

    if (CMDEV(unit).dev.info == NULL) {
        gprintk("detach: invalid CM device.\n");
        return -1;
    }

    if (CMDEV(unit).dev.info->dev_type & SOC_SWITCH_DEV_TYPE) {
        if (clean_shut) {
#ifdef BCM_WARM_BOOT_SUPPORT
            _bcm_shutdown(unit);
#ifdef BCM_ESW_SUPPORT
            soc_shutdown(unit);
#endif
#else
            bcm_detach(unit);
            if (CMDEV(unit).dev.info->dev_type & SOC_SPI_DEV_TYPE) { 
            } else {
#ifdef BCM_ESW_SUPPORT        
                soc_detach(unit);
#endif
            }
#endif /* BCM_WARM_BOOT_SUPPORT */
        } else {
            bcm_detach(unit);
            if (CMDEV(unit).dev.info->dev_type & SOC_SPI_DEV_TYPE) { 
            } else {
#ifdef BCM_ESW_SUPPORT        
                soc_detach(unit);
#endif
            }

        }
    } 

    return 0;
}
