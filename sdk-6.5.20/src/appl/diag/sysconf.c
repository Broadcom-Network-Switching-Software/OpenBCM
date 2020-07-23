/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
#ifdef BCM_LTSW_SUPPORT
#include <appl/diag/sysconf_ltsw.h>
#endif
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
_setup_bus(int bde_dev)
{
    uint16        driverDevID;
    uint16        driverRevID;
    const ibde_dev_t    *dev = bde->get_dev(bde_dev);
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
        revID = (bde->read(bde_dev, CMIC_REVID_DEVID) >> 16) & 0xff;
    }
#endif /* BCM_ENDURO_SUPPORT */

    soc_cm_get_id_driver(dev->device, revID,
             &driverDevID, &driverRevID);

    switch(bde->get_dev_type(bde_dev) & BDE_DEV_BUS_TYPE_MASK)
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
                    bde->get_dev_type(bde_dev) & BDE_DEV_BUS_TYPE_MASK);
            return -1;
    }

    cli_out("BDE dev %d (%s), "
            "Dev 0x%04x, Rev 0x%02x, Chip %s, Driver %s\n",
            bde_dev, bus_type,
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

#define UNIT_NOT_USED 0
#define UNIT_RESERVED 1
#define UNIT_ASSIGNED 2
#define UNIT_IN_COMPOSITE 3
int sysconf_probe_done;

#define MAX_NOF_COMPOSITE_DEVICE_ARGS (2 * SOC_MAX_NUM_DEVICES)
int
sysconf_probe(void)
{
    int bde_dev, unit = -1, i;
    int nof_values, nof_bde_devices;
    int nof_reserved_units = 0, reserved_units_left, last_assigned_unit = -1;
    int cm_dev = 0;
    uint16 devID;
    uint8 revID;
#if defined(BCM_RCPU_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)
    soc_rcpu_cfg_t *rcpu_cfg = NULL;
#endif
    uint8 units[SOC_MAX_NUM_DEVICES] = {UNIT_NOT_USED}; /* usage status of each possible unit */
    uint32 unique_ids[SOC_MAX_NUM_DEVICES];             /* unique IDs of devices a unit is reserved for */
    soc_cm_dev_id reserved_units[SOC_MAX_NUM_DEVICES];  /* Units reserved for a device */
    int values[MAX_NOF_COMPOSITE_DEVICE_ARGS];          /* values read from soc property */

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
    nof_bde_devices = bde->num_devices(BDE_ALL_DEVICES);
    if (nof_bde_devices > SOC_MAX_NUM_DEVICES) {
        cli_out("warning: BDE found %d devices, but only the first %d of them will be considered\n", nof_bde_devices, SOC_MAX_NUM_DEVICES);
        nof_bde_devices = SOC_MAX_NUM_DEVICES;
    }

    /* read the information for devices configured to a specific unit and for composite devices */
    nof_values = soc_property_get_csv(SOC_UNIT_NONE_SAL_CONFIG, spn_ASSIGN_UNITS, MAX_NOF_COMPOSITE_DEVICE_ARGS, values);
    if (nof_values > 0) {
        if ((nof_values & 1)) {
            cli_out("soc property %s must have an even number of comma separated values\n", spn_ASSIGN_UNITS);
            return -1;
        } else if (nof_values > MAX_NOF_COMPOSITE_DEVICE_ARGS) {
            cli_out("soc property %s can not reserve more than %u units\n", spn_ASSIGN_UNITS, SOC_MAX_NUM_DEVICES);
            return -1;
        }
        for (i = 0; i < nof_values;) {
            unit = values[i++];
            if (unit >= SOC_MAX_NUM_DEVICES || unit < 0) {
                cli_out("unit %d is not in range 0..%u\n", unit, SOC_MAX_NUM_DEVICES-1);
                return -1;
            } else if (units[unit] != UNIT_NOT_USED) {
                cli_out("unit %d is specified more than once in %s\n", unit, spn_ASSIGN_UNITS);
                return -1;
            }
            units[unit] = UNIT_RESERVED; /* mark the unit as reserved */
            unique_ids[nof_reserved_units] = values[i++]; /* store the unique ID and unit of the reservation */
            reserved_units[nof_reserved_units++] = unit;
        }
    }
    reserved_units_left = nof_reserved_units;

    /* Iterate over devices found by BDE */
    for (bde_dev = 0; bde_dev < nof_bde_devices; bde_dev++) {
        const ibde_dev_t *dev = bde->get_dev(bde_dev);
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

            bde->pci_bus_features(bde_dev, &big_pio, &big_packet, &big_other);

            if (big_pio) {
                config |= ES_BIG_ENDIAN_PIO;
            }
            if (big_packet) {
                config |= ES_BIG_ENDIAN_DMA_PACKET;
            }
            if (big_other) {
                config |= ES_BIG_ENDIAN_DMA_OTHER;
            }

            bde->write(bde_dev, CMIC_ENDIAN_SELECT, config);            
            revID = (bde->read(bde_dev, CMIC_REVID_DEVID) >> 16) & 0xff;
        }
#endif /* BCM_ENDURO_SUPPORT */

        sysconf_chip_override(bde_dev, &devID, &revID);

        if (soc_cm_device_supported(devID, revID) < 0) {
            /* Not a switch chip; continue probing other devices */
            cli_out("warning: device 0x%x revision 0x%x is not supported\n", (unsigned)devID, (unsigned)revID);
            return 0;
        }

        if (_setup_bus(bde_dev) < 0) {
            /*
             * Error message already printed; continue probing other
             * devices
             */
            return 0;
        }

        /*
         * Create a device handle, but don't initialize yet. If no units are
         * reserved, this sneakily relies upon the fact that the cm handle = bde
         * handle, which will probably get me into trouble some day.
         * Since this is really just a minimal port to support the BDE
         * interface, I'm hoping it will get rewritten more than this.
         */
        if (nof_reserved_units == 0) { /* When no units are reserved, unit==bde_dev */
            cm_dev = soc_cm_device_create(devID, revID, NULL);
        } else {
            /* check if a unit was reserved for this device */
            uint32 unique_id = dev->dev_unique_id;
            int found = FALSE;
            for (i = 0; i < reserved_units_left; ++i) {
                if (unique_id == unique_ids[i]) { /* found a unit reservation */;
                    unit = reserved_units[i];
                    /* update the data structures of left reserved units */
                    unique_ids[i] = unique_ids[--reserved_units_left];
                    reserved_units[i] = reserved_units[reserved_units_left];
                    LOG_ERROR(BSL_LS_SOC_COMMON, ("BDE device %d DevID 0x%x revision 0x%x unique_id 0x%x assigned to reserved unit %d\n",
                      bde_dev, (unsigned)devID, (unsigned)revID, unique_id, unit));
                    found = TRUE;
                    break;
                }
            }

            if (!found) { /* did not find a reservation, use the first free non-reserved unit */
                for (unit = last_assigned_unit + 1; ; ++unit) {
                    if (unit >= SOC_MAX_NUM_DEVICES) {
                        cli_out("Error: BDE device %d DevID 0x%x revision 0x%x has no free unit due to over reservation\n",
                          bde_dev, (unsigned)devID, (unsigned)revID);
                        return -1;
                    }
                    if (units[unit] == UNIT_NOT_USED) {
                        break;
                    }
                }
                last_assigned_unit = unit;
                LOG_WARN(BSL_LS_SOC_COMMON, ("BDE device %d DevID 0x%x revision 0x%x unique_id 0x%x assigned to 1st free unit %d\n",
                  bde_dev, (unsigned)devID, (unsigned)revID, unique_id, unit));
            }
            cm_dev = soc_cm_device_create_id(devID, revID, NULL, unit);
            if (cm_dev != unit) {
                cli_out("error: Failed to create CM device %d for DEvID 0x%x RevID 0x%x, received cm_dev %d instead\n",
                  unit, (unsigned)devID, (unsigned)revID, cm_dev);
                return -1;
            }
        }
        if (cm_dev >= SOC_MAX_NUM_DEVICES || cm_dev < 0) {
            cli_out("unit %d assigned to BDE dev %d is not in range 0..%u\n", cm_dev, bde_dev, SOC_MAX_NUM_DEVICES-1);
            return -1;
        }
        units[cm_dev] = UNIT_ASSIGNED;

#ifndef BCM_LTSW_SUPPORT
        if (nof_reserved_units != 0)
#endif
        /*
         * In HSDK system and when reserving units, cm_dev may not be consistent
         * with bde handle, so keep the bde handle info in CMDEV.
         */
        {
            if (soc_cm_dev_num_set(cm_dev, bde_dev) != SOC_E_NONE) {
                cli_out("error: Failed to assign BDE dev %d to CM device %d DEvID 0x%x RevID 0x%x\n",
                  bde_dev, cm_dev, (unsigned)devID, (unsigned)revID);
                return -1;
            }
        }
#ifndef BCM_LTSW_SUPPORT
        else { /* If the bde device must be equal to the CM device, verify */
            assert(cm_dev == bde_dev);
        }
#endif
        sysconf_probe_done++;
    }


    

    if(_sysconf_get_property("extra_unit_min") &&
       _sysconf_get_property("extra_unit_max")) {

        int min, max; 

        min = sal_ctoi(_sysconf_get_property("extra_unit_min"), 0); 
        max = sal_ctoi(_sysconf_get_property("extra_unit_max"), 0); 
        
        for (unit = min; unit <= max; unit++) {
            char prop[64]; 
            uint16 devid = 0; 
            uint8 revid; 
            sal_sprintf(prop, "extra_unit.%d", unit); 
            if (_sysconf_get_property(prop)) {
                sysconf_chip_override(unit, &devid, &revid); 
#if defined(BCM_RCPU_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)
                sal_sprintf(prop, "rcpu_only.%d", unit);
                if (_sysconf_get_property(prop) && 
                   (1 == sal_ctoi(_sysconf_get_property(prop), 0))) {
                    rcpu_cfg = (soc_rcpu_cfg_t *)sal_alloc(sizeof(soc_rcpu_cfg_t),
                                                           "SYSCONF RCPU");
                    _rcpu_config_get(unit, rcpu_cfg);
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

/**
 * this function should be used when running device init to
 *  make sure we have the correct mapping between cm to bde
 *  when sysconf_probe is not used
 */
int
sysconf_cm_to_bde_orignal_map_get(
    int cm_unit,
    int* bde_unit)
{
    int nof_bde_devices;
    int nof_values;
    int values[MAX_NOF_COMPOSITE_DEVICE_ARGS];
    uint8 units[SOC_MAX_NUM_DEVICES] = {UNIT_NOT_USED};
    uint32 unique_ids[SOC_MAX_NUM_DEVICES];
    soc_cm_dev_id reserved_units[SOC_MAX_NUM_DEVICES];
    int nof_reserved_units = 0;
    int reserved_units_left;
    int bde_dev;
    int last_assigned_unit = -1;
    int i;
    int unit;

    /** get number of BDE devices */
    nof_bde_devices = bde->num_devices(BDE_ALL_DEVICES);

    /* read the information for devices configured to a specific unit and for composite devices */
    nof_values = soc_property_get_csv(SOC_UNIT_NONE_SAL_CONFIG, spn_ASSIGN_UNITS, MAX_NOF_COMPOSITE_DEVICE_ARGS, values);
    if (nof_values > 0) {
        if ((nof_values & 1)) {
            cli_out("soc property %s must have an even number of comma separated values\n", spn_ASSIGN_UNITS);
            return -1;
        } else if (nof_values > MAX_NOF_COMPOSITE_DEVICE_ARGS) {
            cli_out("soc property %s can not reserve more than %u units\n", spn_ASSIGN_UNITS, SOC_MAX_NUM_DEVICES);
            return -1;
        }
        for (i = 0; i < nof_values;) {
            unit = values[i++];
            if (unit >= SOC_MAX_NUM_DEVICES || unit < 0) {
                cli_out("unit %d is not in range 0..%u\n", unit, SOC_MAX_NUM_DEVICES-1);
                return -1;
            } else if (units[unit] != UNIT_NOT_USED) {
                cli_out("unit %d is specified more than once in %s\n", unit, spn_ASSIGN_UNITS);
                return -1;
            }
            units[unit] = UNIT_RESERVED; /* mark the unit as reserved */
            unique_ids[nof_reserved_units] = values[i++]; /* store the unique ID and unit of the reservation */
            reserved_units[nof_reserved_units++] = unit;
        }
    }
    reserved_units_left = nof_reserved_units;

    /** if there are no reserved unit, bde_unit == cm_unit */
    if(nof_reserved_units == 0)
    {
        *bde_unit = cm_unit;
        return 0;
    }

    /* Iterate over devices found by BDE */
    for (bde_dev = 0; bde_dev < nof_bde_devices; bde_dev++)
    {
        const ibde_dev_t *dev = bde->get_dev(bde_dev);
        uint32 unique_id = dev->dev_unique_id;
        int found = FALSE;
        int cm_dev;

        /* check if a unit was reserved for this device */
        for (i = 0; i < reserved_units_left; ++i)
        {
            if (unique_id == unique_ids[i])
            {
                /** found a unit reservation */
                cm_dev = reserved_units[i];
                /* update the data structures of left reserved units */
                unique_ids[i] = unique_ids[--reserved_units_left];
                reserved_units[i] = reserved_units[reserved_units_left];
                found = TRUE;
                break;
            }
        }

        /* did not find a reservation, use the first free non-reserved unit */
        if (!found)
        {
            for (cm_dev = last_assigned_unit + 1; ; ++cm_dev)
            {
                if (cm_dev >= SOC_MAX_NUM_DEVICES)
                {
                    return -1;
                }
                if (units[cm_dev] == UNIT_NOT_USED)
                {
                    break;
                }
            }
            last_assigned_unit = cm_dev;
        }

        /** update for next units array for next BDE device */
        units[cm_dev] = UNIT_ASSIGNED;

        /** check if last found device match input device, if so return its BDE */
        if(cm_dev == cm_unit)
        {
            *bde_unit = bde_dev;
            return 0;
        }
    }

    /** if by this point the BDE unit is not found, return an error and set DBE unit to negative */
    cli_out("no matching BDE unit was found for provided CM unit (%d)\n", cm_unit);
    *bde_unit = -1;
    return -1;
}

int
sysconf_attach(int unit)
{
    /* Ready to install into configuration manager */
    soc_cm_device_vectors_t vectors;
    int bdx = CMDEV(unit).dev.dev; /* BDE handle */
    char prop[64]; 
#if defined(BCM_RCPU_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)
    char *s;
#endif /* BCM_RCPU_SUPPORT && BCM_XGS3_SWITCH_SUPPORT */

#ifdef BCM_LTSW_SUPPORT
    if (soc_cm_get_dev_type(unit) & SOC_LTSW_DRV_TYPE) {
        return sysconf_ltsw_attach(unit);
    }
#endif

    sal_memset(&vectors, 0, sizeof(soc_cm_device_vectors_t));
    /*
     * Install extra devices separately to void calls to the BDE. 
     */
    sal_sprintf(prop, "extra_unit.%d", unit); 
    if (_sysconf_get_property(prop) || CM_IS_COMPOSITE_DEVICE(unit)) {
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

    assert(unit >= 0 && unit < SOC_MAX_NUM_DEVICES);

    bde->pci_bus_features(bdx, &vectors.big_endian_pio,
                          &vectors.big_endian_packet,
                          &vectors.big_endian_other);
    vectors.config_var_get = _config_var_get;
    vectors.interrupt_connect = _interrupt_connect;
    vectors.interrupt_disconnect= _interrupt_disconnect;
#if defined(PCI_DECOUPLED) || defined(BCM_ICS)
    vectors.base_address = 0;
#else
    vectors.base_address = bde->get_dev(bdx)->base_address;
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
    vectors.bus_type = bde->get_dev_type(bdx);
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
    assert(unit >= 0 && unit < SOC_MAX_NUM_DEVICES);

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
#ifdef BCM_LTSW_SUPPORT
    if (SOC_IS_LTSW(unit)) {
        return sysconf_ltsw_detach(unit);
    }
#endif

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
#ifdef BCM_LTSW_SUPPORT
    if (sysconf_ltsw_init() < 0) {
        cli_out("sysconf_init: LTSW initialization failed\n");
    }
#endif
    return soc_cm_init();
}
