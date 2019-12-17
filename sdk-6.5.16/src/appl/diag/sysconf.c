/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */


#include <sal/types.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <shared/bsl.h>
#include <sal/core/spl.h>
#include <sal/appl/pci.h>
#include <sal/appl/sal.h>
#include <sal/appl/config.h>
#include <sal/appl/io.h>

#include <soc/debug.h>
#include <soc/cmext.h>
#include <soc/drv.h>
#if defined(BCM_RCPU_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)
#include <soc/rcpu.h>
#endif /* defined(BCM_RCPU_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT) */
#include <bcm/init.h>
#include <appl/diag/system.h>
#include <appl/diag/sysconf.h>
#if defined(BCM_DFE_SUPPORT) || defined(BCM_PETRA_SUPPORT)
#if defined(INCLUDE_INTR)
#include <appl/dcmn/interrupts/interrupt_handler.h>
#endif
#endif
#include <ibde.h>
#include <soc/util.h>

extern int bde_create(void); /* provide by configuration/socdiag */
extern int soc_phy_fw_acquire_default(const char *file_name, uint8 **fw, int *fw_len);
extern int soc_phy_fw_release_default(const char *dev_name, uint8 *fw, int fw_len);

/* SOC Configuration Manager Device Vectors */

static char *
_sysconf_get_property(const char *property)
{
#ifndef NO_SAL_APPL
    return sal_config_get(property);
#else
    return NULL;
#endif
}

static char *
_config_var_get(soc_cm_dev_t *dev, const char *property)
{
    COMPILER_REFERENCE(dev);

    return _sysconf_get_property(property);
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
_write_null(soc_cm_dev_t *dev, uint32 addr, uint32 data)
{
    return; 
}

static uint32
_read_null(soc_cm_dev_t *dev, uint32 addr)
{
    return 0; 
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

static sal_paddr_t
_l2p(soc_cm_dev_t *dev, void *addr)
{
    return (bde->l2p) ? bde->l2p(dev->dev, addr) : 0;
}

static void*
_p2l(soc_cm_dev_t *dev, sal_paddr_t addr)
{
    return (bde->p2l) ? bde->p2l(dev->dev, addr) : 0;
}

static void
_iproc_write(soc_cm_dev_t *dev, uint32 addr, uint32 data)
{
    bde->iproc_write(dev->dev, addr, data);
}

static uint32
_iproc_read(soc_cm_dev_t *dev, uint32 addr)
{
    return bde->iproc_read(dev->dev, addr);
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
_interrupt_connect_null(soc_cm_dev_t *dev, soc_cm_isr_func_t handler, void *data)
{
    return 0; 
}

static int
_interrupt_disconnect_null(soc_cm_dev_t *dev)
{
    return 0;
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

static int
_i2c_device_read(soc_cm_dev_t *dev, uint32 addr, uint32 *value)
{
    return (bde->i2c_device_read) ? bde->i2c_device_read(dev->dev, addr, value) : -1;
}

static int
_i2c_device_write(soc_cm_dev_t *dev, uint32 addr, uint32 value)
{
    return (bde->i2c_device_write) ? bde->i2c_device_write(dev->dev, addr, value) : -1;
}

/*
 * Function:
 *    sysconf_chip_override
 * Purpose:
 *    For driver testing only, allow properties to override the PCI
 *    device ID/rev ID for a particular unit to specified values.
 * Parameters:
 *    unit - Unit number
 *    devID, revID - (IN, OUT) original PCI ID info
 */

void
sysconf_chip_override(int unit, uint16 *devID, uint8 *revID)
{
    char        prop[64], *s;

    if ((*devID == BCM88670_DEVICE_ID) && (*revID == QMX_A0_REV_ID))
    {
        *devID = QMX_DEVICE_ID;
    }

    sal_sprintf(prop, "pci_override_dev.%d", unit);

    if ((s = _sysconf_get_property(prop)) == NULL) {
    s = _sysconf_get_property("pci_override_dev");
    }

    if (s != NULL) {
    *devID = sal_ctoi(s, 0);
    }

    sal_sprintf(prop, "pci_override_rev.%d", unit);

    if ((s = _sysconf_get_property(prop)) == NULL) {
    s = _sysconf_get_property("pci_override_rev");
    }

    if (s != NULL) {
    *revID = sal_ctoi(s, 0);
    }

    if (SAL_BOOT_BCMSIM && *devID == 0xb260)
    {
        *devID = 0xb460;
    }
}

/*
 * _setup_bus
 *
 *    Utility routine used by sysconf_probe
 */

static int
_setup_bus(int unit)
{
    uint16        driverDevID;
    uint8        driverRevID;
    const ibde_dev_t    *dev = bde->get_dev(unit);
    char                *bus_type;
    uint8               revID = dev->rev;

#if defined(BCM_ENDURO_SUPPORT)
    /*
     * For Enduro, read revision ID from CMIC instead of PCIe config space.
     */
    if (!SAL_BOOT_BCMSIM &&
        ((dev->device == BCM56132_DEVICE_ID) || 
         (dev->device == BCM56134_DEVICE_ID) ||
         (dev->device == BCM56320_DEVICE_ID) || 
         (dev->device == BCM56321_DEVICE_ID) || 
         (dev->device == BCM56331_DEVICE_ID) || 
         (dev->device == BCM56333_DEVICE_ID) ||
         (dev->device == BCM56334_DEVICE_ID) ||
         (dev->device == BCM56338_DEVICE_ID) ||
         (dev->device == BCM56230_DEVICE_ID) ||
         (dev->device == BCM56231_DEVICE_ID))) {
        revID = (bde->read(unit, CMIC_REVID_DEVID) >> 16) & 0xff;
    }
#endif /* BCM_ENDURO_SUPPORT */

    soc_cm_get_id_driver(dev->device, revID,
             &driverDevID, &driverRevID);

    switch(bde->get_dev_type(unit) & BDE_DEV_BUS_TYPE_MASK)
    {
        case BDE_PCI_DEV_TYPE:
            bus_type = "PCI";
            break;
        case BDE_SPI_DEV_TYPE:
            bus_type = "SPI";
            break;
        case BDE_EB_DEV_TYPE:
            bus_type = "EB";
            break;
        case BDE_ICS_DEV_TYPE:
            bus_type = "ICS";
            break;
        case (BDE_SPI_DEV_TYPE|BDE_EB_DEV_TYPE) :
            bus_type = "SPI/EB";
            break;
        case BDE_ET_DEV_TYPE:
            bus_type = "ETH";
            break;
        case BDE_EMMI_DEV_TYPE:
            bus_type = "EMMI";
            break;    
        case BDE_AXI_DEV_TYPE:
            bus_type = "AXI";
            break;  
       case BDE_I2C_DEV_TYPE:
            bus_type = "I2C";
            break;
        default:
            cli_out("Error : Unknow bus type 0x%x !!\n",
                    bde->get_dev_type(unit) & BDE_DEV_BUS_TYPE_MASK);
            return -1;
    }

    cli_out("%s unit %d: "
            "Dev 0x%04x, Rev 0x%02x, Chip %s, Driver %s\n",
            bus_type, unit,
            dev->device, revID,
            soc_cm_get_device_name(dev->device, revID),
            soc_cm_get_device_name(driverDevID, driverRevID));

    /*
     * Sanity check to ensure first CMIC register is accessible.
     */

    if (dev->base_address) {
#ifndef __KERNEL__
    if (sal_memory_check((uint32)dev->base_address) < 0) {
        cli_out("sysconf_probe: unable to probe address 0x%x\n",
                (uint32)dev->base_address);
        return -1;
    }
#endif
    }

    return 0;
}


#if defined(BCM_RCPU_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)

#ifdef BCM_CMICM_SUPPORT
static uint32
_rcpu_read(soc_cm_dev_t *dev, uint32 addr)
{
    return soc_rcpu_cmic_read(dev->dev, addr);
}

static void
_rcpu_write(soc_cm_dev_t *dev, uint32 addr, uint32 data)
{
    soc_rcpu_cmic_write(dev->dev, addr, data); 
}
#endif /* BCM_CMICM_SUPPORT */

static void
_rcpu_config_get(int unit, soc_rcpu_cfg_t *cfg)
{
    char *str;
    char prop[64];

    sal_sprintf(prop, "rcpu_lmac.%d", unit);
    str = _sysconf_get_property(prop);
    if (str != NULL) {
        (void)parse_macaddr(str, cfg->dst_mac);
    } else {
        str = DEFAULT_RCPU_DST_MAC;        
        (void)parse_macaddr(str, cfg->dst_mac);
        cfg->dst_mac[5] = unit;
    }

    sal_sprintf(prop, "rcpu_src_mac.%d", unit);
    str = _sysconf_get_property(prop);
    if (str != NULL) {
        (void)parse_macaddr(str, cfg->src_mac);
    } else {
        str = DEFAULT_RCPU_SRC_MAC;        
        (void)parse_macaddr(str, cfg->dst_mac);
    }

    cfg->tpid = DEFAULT_RCPU_TPID;

    sal_sprintf(prop, "rcpu_vlan.%d", unit);
    str = _sysconf_get_property(prop);
    if (str != NULL) {
        cfg->vlan = sal_ctoi(str, 0);
    } else {
        cfg->vlan = DEFAULT_RCPU_VLAN;
    }

    sal_sprintf(prop, "rcpu_port.%d", unit);
    str = _sysconf_get_property(prop);
    if (str != NULL) {
        cfg->port = sal_ctoi(str, 0);
    } else {
        cfg->port = -1;
    }

    cfg->ether_type = DEFAULT_RCPU_ETHER_TYPE;
    cfg->signature = DEFAULT_RCPU_SIGNATURE;
}
#endif /* BCM_RCPU_SUPPORT && BCM_XGS3_SWITCH_SUPPORT */

/*
 * sysconf_probe
 *
 * Searches for known devices and creates Configuration Manager instances.
 */

int sysconf_probe_done;

int
sysconf_probe(void)
{
    int u;
#ifndef NDEBUG
    int cm_dev = 0;
#endif /* !NDEBUG*/
    uint16 devID;
    uint8 revID;
#if defined(BCM_RCPU_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)
    soc_rcpu_cfg_t *rcpu_cfg = NULL;
#endif
    if (sysconf_probe_done) {
        cli_out("sysconf_probe: cannot probe more than once\n");
        return -1;
    }


    /*
     * Initialize system BDE
     */
    if (bde_create()) {
        return -1;
    }

    /* Iterate over device */
    for (u = 0; u < bde->num_devices(BDE_ALL_DEVICES) && u < SOC_MAX_NUM_DEVICES; u++) {
        const ibde_dev_t *dev = bde->get_dev(u);
        devID = dev->device;
        revID = dev->rev;

#if defined(BCM_ENDURO_SUPPORT)
        /*
         * For Enduro, read revision ID from CMIC instead of PCIe config space.
         */
        if (!SAL_BOOT_BCMSIM &&
            ((devID == BCM56132_DEVICE_ID) || (devID == BCM56134_DEVICE_ID) ||
             (devID == BCM56320_DEVICE_ID) || (devID == BCM56321_DEVICE_ID) || 
             (devID == BCM56331_DEVICE_ID) || (devID == BCM56333_DEVICE_ID) ||
             (devID == BCM56334_DEVICE_ID) || (devID == BCM56338_DEVICE_ID) ||
             (devID == BCM56230_DEVICE_ID) || (devID == BCM56231_DEVICE_ID))) {
            uint32          config = 0;
            int             big_pio, big_packet, big_other;

            bde->pci_bus_features(u, &big_pio, &big_packet, &big_other);

            if (big_pio) {
                config |= ES_BIG_ENDIAN_PIO;
            }
            if (big_packet) {
                config |= ES_BIG_ENDIAN_DMA_PACKET;
            }
            if (big_other) {
                config |= ES_BIG_ENDIAN_DMA_OTHER;
            }

            bde->write(u, CMIC_ENDIAN_SELECT, config);            
            revID = (bde->read(u, CMIC_REVID_DEVID) >> 16) & 0xff;
        }
#endif /* BCM_ENDURO_SUPPORT */

        sysconf_chip_override(u, &devID, &revID);

        if (soc_cm_device_supported(devID, revID) < 0) {
            /* Not a switch chip; continue probing other devices */
            cli_out("warning: device 0x%x revision 0x%x is not supported\n", (unsigned)devID, (unsigned)revID);
            return 0;
        }

        if (_setup_bus(u) < 0) {
            /*
             * Error message already printed; continue probing other
             * devices
             */
            return 0;
        }

        /*
         * Create a device handle, but don't initialize yet.  This
         * sneakily relies upon the fact that the cm handle = bde
         * handle, which will probably get me into trouble some day.
         * Since this is really just a minimal port to support the BDE
         * interface, I'm hoping it will get rewritten more than this.
         */
#ifndef NDEBUG
        cm_dev =
#endif /* !NDEBUG*/
        soc_cm_device_create(devID, revID, NULL);

        assert(cm_dev >= 0);
        assert(cm_dev == u);
        sysconf_probe_done++;
    }


    

    if(_sysconf_get_property("extra_unit_min") &&
       _sysconf_get_property("extra_unit_max")) {

        int u, min, max; 

        min = sal_ctoi(_sysconf_get_property("extra_unit_min"), 0); 
        max = sal_ctoi(_sysconf_get_property("extra_unit_max"), 0); 
        
        for(u = min; u <= max; u++) {
            char prop[64]; 
            uint16 devid = 0; 
            uint8 revid; 
            sal_sprintf(prop, "extra_unit.%d", u); 
            if(_sysconf_get_property(prop)) {
                sysconf_chip_override(u, &devid, &revid); 
#if defined(BCM_RCPU_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)
                sal_sprintf(prop, "rcpu_only.%d", u);
                if(_sysconf_get_property(prop) && 
                   (1 == sal_ctoi(_sysconf_get_property(prop), 0))) {
                    rcpu_cfg = (soc_rcpu_cfg_t *)sal_alloc(sizeof(soc_rcpu_cfg_t),
                                                           "SYSCONF RCPU");
                    _rcpu_config_get(u, rcpu_cfg);
                    soc_cm_device_create(devid, revid, rcpu_cfg);
                } else
#endif /* defined(BCM_RCPU_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT) */              
                {
                    soc_cm_device_create(devid, revid, NULL);
                }
            }
        }
    }

    return 0;
}

int
sysconf_attach(int unit)
{
    /* Ready to install into configuration manager */
    soc_cm_device_vectors_t vectors;
    char prop[64]; 
#if defined(BCM_RCPU_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)
    char *s;
#endif /* BCM_RCPU_SUPPORT && BCM_XGS3_SWITCH_SUPPORT */

    sal_memset(&vectors, 0, sizeof(soc_cm_device_vectors_t));
    /*
     * Install extra devices separately to void calls to the BDE. 
     */
    sal_sprintf(prop, "extra_unit.%d", unit); 
    if(_sysconf_get_property(prop)) {
        vectors.config_var_get = _config_var_get;
        vectors.interrupt_connect = _interrupt_connect_null;
        vectors.interrupt_disconnect= _interrupt_disconnect_null;
        vectors.base_address = 0;
        vectors.read = _read_null;
        vectors.write = _write_null; 
        vectors.pci_conf_read = _pci_conf_read;
        vectors.pci_conf_write = _pci_conf_write;
        vectors.salloc = _salloc;
        vectors.sfree = _sfree;
        vectors.sinval = _sinval;
        vectors.sflush = _sflush;
        vectors.l2p = _l2p;
        vectors.p2l = _p2l;
        vectors.bus_type = 0;
        vectors.i2c_device_read = _i2c_device_read;
        vectors.i2c_device_write = _i2c_device_write;
#if defined(BCM_RCPU_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)
        sal_sprintf(prop, "rcpu_only.%d", unit);
        s = _sysconf_get_property(prop);
        if(NULL != s && (1 == sal_ctoi(s, 0))) {
            vectors.bus_type = SOC_RCPU_DEV_TYPE;
#ifdef BCM_CMICM_SUPPORT
            vectors.read = _rcpu_read;
            vectors.write = _rcpu_write; 
#endif /* BCM_CMICM_SUPPORT */
#ifdef BCM_OOB_RCPU_SUPPORT
            sal_sprintf(prop, "rcpu_use_oob.%d", unit);
            s = _sysconf_get_property(prop);
            if(NULL != s && (1 == sal_ctoi(s, 0))) {
                vectors.rcpu_tp = &rcpu_oob_trans_ptr;
            } else
#endif /* BCM_OOB_RCPU_SUPPORT */
            {
                vectors.rcpu_tp = &rcpu_trans_ptr;
            }
        }
#endif /* BCM_RCPU_SUPPORT && BCM_XGS3_SWITCH_SUPPORT */

        if (soc_cm_device_init(unit, &vectors) < 0) {
            cli_out("sysconf_attach: bcm device init failed\n");
            return -1;
        }
        return 0; 
    }   

    /* Proceed with BDE device initialization */

    assert(unit >= 0 && unit < bde->num_devices(BDE_ALL_DEVICES));

    bde->pci_bus_features(unit, &vectors.big_endian_pio,
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
    vectors.l2p = _l2p;
    vectors.p2l = _p2l;
    vectors.iproc_read = _iproc_read;
    vectors.iproc_write = _iproc_write;
    vectors.bus_type = bde->get_dev_type(unit);
    vectors.spi_read = _spi_read;
    vectors.spi_write = _spi_write;
    vectors.i2c_device_read = _i2c_device_read;
    vectors.i2c_device_write = _i2c_device_write;
    if (soc_cm_device_init(unit, &vectors) < 0) {
        cli_out("sysconf_attach: bcm device init failed\n");
        return -1;
    }
    return 0;
}

int
sysconf_detach(int unit)
{
#ifdef BCM_RCPU_SUPPORT
    assert(unit >= 0 && unit < soc_cm_get_num_devices());
#else
    assert(unit >= 0 && unit < bde->num_devices(BDE_ALL_DEVICES));
#endif
#if (defined(BCM_DFE_SUPPORT) || defined(BCM_PETRA_SUPPORT)) && defined(INCLUDE_INTR)
    if ((SOC_IS_FE1600(unit)) || (SOC_IS_ARAD(unit))){
        interrupt_handler_appl_deinit(unit);
    }
#endif
    if (!bcm_attach_check(unit)) {
        if (bcm_detach(unit) < 0) {
            cli_out("sysconf_detach: bcm detach failed\n");
            return -1;
        }
    }

    /* COVERITY
     * Intentional stack usage
     */
    /* coverity[stack_use_return : FALSE] */
    /* coverity[stack_use_callee_max : FALSE] */
    /* coverity[stack_use_overflow : FALSE] */
    if (soc_cm_device_destroy(unit) < 0) {
        cli_out("sysconf_detach: soc_cm_device_destroy failed\n");
        return -1;
    }
    sysconf_probe_done--;

    return 0;
}

int
sysconf_init(void)
{
    soc_phy_fw_init();
    soc_phy_fw_acquire = soc_phy_fw_acquire_default;
    soc_phy_fw_release = soc_phy_fw_release_default;

    return soc_cm_init();
}
