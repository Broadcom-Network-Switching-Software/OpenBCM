/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * I2C driver for BSC (Broadcom Serial control). This
 * module provides for discovery and loading of I2C device drivers and
 * manipulation of I2C device state.
 *
 * See also: bus.c
 */

#if defined(INCLUDE_I2C)

#include <sal/types.h>
#include <sal/core/boot.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/cmic.h>
#include <soc/i2c.h>
#include <soc/bsc.h>
#include <shared/bsl.h>

#ifdef INCLUDE_I2C_SFP
extern bsc_driver_t _soc_bsc_sfp_driver;
#endif

#ifdef BCM_FE2000_SUPPORT
/* Temperature Monitors and PWM Fan Controllers */
extern bsc_driver_t _soc_bsc_max6653_driver;

/* Digital Power Manager */
extern bsc_driver_t _soc_bsc_zm73xx_driver;

extern bsc_driver_t _soc_bsc_at24c64_driver;
#endif

/* Data: bsc_devices
 * Purpose: BSC device descriptor table.
 *          Devices are added to this table, along with their slave
 *          address, a string description, and their driver (if available).
 */
STATIC bsc_device_t bsc_devices[] = {
#ifdef BCM_FE2000_SUPPORT
	{
		"max6653_0",				/* device name */
		BSC_PCA9548_SADDR,			/* mux i2c address */
		BSC_PCA9548_CHAN_0,			/* mux channel for this device */
		BSC_MAX6653_SADDR,			/* device i2c address */
		&_soc_bsc_max6653_driver,		/* device driver */
		"Temperature and Fan speed sensor"	/* description */
	},
	{
		"max6653_1",				/* device name */
		BSC_PCA9548_SADDR,			/* mux i2c address */
		BSC_PCA9548_CHAN_1,			/* mux channel for this device */
		BSC_MAX6653_SADDR,			/* device i2c address */
		&_soc_bsc_max6653_driver,		/* device driver */
		"Temperature and Fan speed sensor"	/* description */
	},
	{
		"max6653_2",				/* device name */
		BSC_PCA9548_SADDR,			/* mux i2c address */
		BSC_PCA9548_CHAN_2,			/* mux channel for this device */
		BSC_MAX6653_SADDR,			/* device i2c address */
		&_soc_bsc_max6653_driver,		/* device driver */
		"Temperature and Fan speed sensor"	/* description */
	},
	{
		"max6653_3",				/* device name */
		BSC_PCA9548_SADDR,			/* mux i2c address */
		BSC_PCA9548_CHAN_3,			/* mux channel for this device */
		BSC_MAX6653_SADDR,			/* device i2c address */
		&_soc_bsc_max6653_driver,		/* device driver */
		"Temperature and Fan speed sensor"	/* description */
	},
	{
		"max6653_4",				/* device name */
		BSC_PCA9548_SADDR,			/* mux i2c address */
		BSC_PCA9548_CHAN_4,			/* mux channel for this device */
		BSC_MAX6653_SADDR,			/* device i2c address */
		&_soc_bsc_max6653_driver,		/* device driver */
		"Temperature and Fan speed sensor"	/* description */
	},
	{
		"zm73xx_0",				/* device name */
		BSC_PCA9548_SADDR,			/* mux i2c address */
		BSC_PCA9548_CHAN_5,			/* mux channel for this device */
		BSC_ZM73XX_SADDR,			/* device i2c address */
		&_soc_bsc_zm73xx_driver,		/* device driver */
		"Digital Power Manager"			/* description */
	},
	{
		BSC_AT24C32_NAME,			/* device name */
		0,					/* no mux i2c address */
		0,					/* no mux channel */
		BSC_AT24C32_SADDR,			/* device i2c address */
		&_soc_bsc_at24c64_driver,		/* device driver */
		"Atmel 24C64 serial EEPROM"		/* description */
	},
#endif
#ifdef INCLUDE_I2C_SFP
        {
		BSC_SFP_00_NAME,	/* device name */
		BSC_PCA9548_SADDR_1,	/* mux i2c address */
		BSC_PCA9548_CHAN_0,	/* mux channel for this device */
		BSC_SFP_SADDR,	/* device i2c address */
		&_soc_bsc_sfp_driver,		/* device driver */
		"SFP"		/* description */
	},
        {
		BSC_SFP_01_NAME,	/* device name */
		BSC_PCA9548_SADDR_1,	/* mux i2c address */
		BSC_PCA9548_CHAN_1,	/* mux channel for this device */
		BSC_SFP_SADDR,	/* device i2c address */
		&_soc_bsc_sfp_driver,		/* device driver */
		"SFP"		/* description */
	},
        {
		BSC_SFP_02_NAME,	/* device name */
		BSC_PCA9548_SADDR_1,	/* mux i2c address */
		BSC_PCA9548_CHAN_2,	/* mux channel for this device */
		BSC_SFP_SADDR,	/* device i2c address */
		&_soc_bsc_sfp_driver,		/* device driver */
		"SFP"		/* description */
	},
        {
		BSC_SFP_03_NAME,	/* device name */
		BSC_PCA9548_SADDR_1,	/* mux i2c address */
		BSC_PCA9548_CHAN_3,	/* mux channel for this device */
		BSC_SFP_SADDR,	/* device i2c address */
		&_soc_bsc_sfp_driver,		/* device driver */
		"SFP"		/* description */
	},
        {
		BSC_SFP_04_NAME,	/* device name */
		BSC_PCA9548_SADDR_3,	/* mux i2c address */
		BSC_PCA9548_CHAN_4,	/* mux channel for this device */
		BSC_SFP_SADDR,	/* device i2c address */
		&_soc_bsc_sfp_driver,		/* device driver */
		"SFP"		/* description */
	},
        {
		BSC_SFP_05_NAME,	/* device name */
		BSC_PCA9548_SADDR_1,	/* mux i2c address */
		BSC_PCA9548_CHAN_5,	/* mux channel for this device */
		BSC_SFP_SADDR,	/* device i2c address */
		&_soc_bsc_sfp_driver,		/* device driver */
		"SFP"		/* description */
	},
        {
		BSC_SFP_06_NAME,	/* device name */
		BSC_PCA9548_SADDR_1,	/* mux i2c address */
		BSC_PCA9548_CHAN_6,	/* mux channel for this device */
		BSC_SFP_SADDR,	/* device i2c address */
		&_soc_bsc_sfp_driver,		/* device driver */
		"SFP"		/* description */
	},
        {
		BSC_SFP_07_NAME,	/* device name */
		BSC_PCA9548_SADDR_1,	/* mux i2c address */
		BSC_PCA9548_CHAN_7,	/* mux channel for this device */
		BSC_SFP_SADDR,	/* device i2c address */
		&_soc_bsc_sfp_driver,		/* device driver */
		"SFP"		/* description */
	},
        {
		BSC_SFP_08_NAME,	/* device name */
		BSC_PCA9548_SADDR_2,	/* mux i2c address */
		BSC_PCA9548_CHAN_0,	/* mux channel for this device */
		BSC_SFP_SADDR,	/* device i2c address */
		&_soc_bsc_sfp_driver,		/* device driver */
		"SFP"		/* description */
	},
        {
		BSC_SFP_09_NAME,	/* device name */
		BSC_PCA9548_SADDR_2,	/* mux i2c address */
		BSC_PCA9548_CHAN_1,	/* mux channel for this device */
		BSC_SFP_SADDR,	/* device i2c address */
		&_soc_bsc_sfp_driver,		/* device driver */
		"SFP"		/* description */
	},
        {
		BSC_SFP_10_NAME,	/* device name */
		BSC_PCA9548_SADDR_2,	/* mux i2c address */
		BSC_PCA9548_CHAN_2,	/* mux channel for this device */
		BSC_SFP_SADDR,	/* device i2c address */
		&_soc_bsc_sfp_driver,		/* device driver */
		"SFP"		/* description */
	},
        {
		BSC_SFP_11_NAME,	/* device name */
		BSC_PCA9548_SADDR_2,	/* mux i2c address */
		BSC_PCA9548_CHAN_3,	/* mux channel for this device */
		BSC_SFP_SADDR,	/* device i2c address */
		&_soc_bsc_sfp_driver,		/* device driver */
		"SFP"		/* description */
	},
#endif
	{
		NULL,
		0,
		0,
		0,
		NULL,
		NULL            /* Dummy entry to circumvent PEDANTIC */ 
	}
};

#define num_bsc_devices (COUNTOF(bsc_devices) - 1)

/*
 * Function: soc_bsc_probe
 * Purpose: Probe BSC devices on bus, report devices found.
 *          This routine will walk through our internal BSC device driver
 *          tables, attempt to find the device on the BSC bus, and if
 *          successful, register a device driver for that device.
 *
 *          This allows for the device to be used in an API context as
 *          when the devices are found, the device driver table is filled
 *          with the correct entries for that device (r/w function, etc).
 *
 * Parameters:
 *	unit - device number
 *
 * Return:
 *	count of found devices or SOC_E_***
 */
int
soc_bsc_probe(int unit)
{
	int found;
	soc_bsc_bus_t *bscbus;
#if defined(BCM_FE2000_SUPPORT) || defined(INCLUDE_I2C_SFP)
	int devno, retv;
#endif

	/* Make sure that we're already attached, or go get attached */
	if (!soc_bsc_is_attached(unit)) {
		return soc_bsc_attach(unit);
	}

	bscbus = BSCBUS(unit);
	found = 0; /* Number of installed BSC devices */

    COMPILER_REFERENCE(bscbus);

/* Coverity Fix : We don't need to probe if no devices are supported */
#if defined(BCM_FE2000_SUPPORT) || defined(INCLUDE_I2C_SFP)
	/* Probe the currently configured (i.e. MUX'd) I2C bus. */
	for (devno = 0; devno < num_bsc_devices; devno++) {

		bscbus->devs[devno] = &bsc_devices[devno];

		/* Check device existence */
		retv = bsc_devices[devno].driver->ioctl(
				unit, devno, BSC_IOCTL_DEVICE_PRESENT, 0, 0);
		if (!retv) {
			/* I2C device not present */
			bscbus->devs[devno] = NULL;
			continue;
		}

		/* set flags, device id, hook up bus, etc  */
		bsc_devices[devno].driver->devno = devno;

		retv =  bsc_devices[devno].driver->init(unit, devno);
		if (retv < 0) {
			LOG_INFO(BSL_LS_SOC_I2C,
                                 (BSL_META_U(unit,
                                             "unit %d i2c 0x%x: init failed %s on channel: 0x%02x\n"),
                                  unit,
                                  bsc_devices[devno].saddr,
                                  bsc_devices[devno].devname,
                                  bsc_devices[devno].chan));
		}
		LOG_INFO(BSL_LS_SOC_I2C,
                         (BSL_META_U(unit,
                                     "unit %d bsc 0x%x: found %s on channel 0x%02x: %s\n"),
                          unit,
                          bsc_devices[devno].saddr,
                          bsc_devices[devno].devname,
                          bsc_devices[devno].chan,
                          bsc_devices[devno].desc ?
                          bsc_devices[devno].desc : ""));

		found += 1;
	}
#endif
	return found;
}

void
soc_bsc_show(int unit)
{
#if defined(BCM_FE2000_SUPPORT) || defined(INCLUDE_I2C_SFP)
	int devno;
#endif
	soc_bsc_bus_t *bscbus;

	/* Make sure that we're already attached, or go get attached */
	if (!soc_bsc_is_attached(unit)) {
		if (soc_bsc_attach(unit) < 0) {
			return;
		}
	}

	bscbus = BSCBUS(unit);

    COMPILER_REFERENCE(bscbus);

/* Coverity Fix : We don't need to show if no devices are supported */
#if defined(BCM_FE2000_SUPPORT) || defined(INCLUDE_I2C_SFP)
	/* Show the currently configured (i.e. MUX'd) I2C bus. */
	for (devno = 0; devno < num_bsc_devices; devno++) {
		if (bscbus->devs[devno]) {
			LOG_CLI((BSL_META_U(unit,
                                            "unit: %d devno: %d name: %s saddr: 0x%x "
                                            "chan: 0x%02x - %s\n"),
                                 unit,
                                 devno,
                                 bsc_devices[devno].devname,
                                 bsc_devices[devno].saddr,
                                 bsc_devices[devno].chan,
                                 bsc_devices[devno].desc ?
                                 bsc_devices[devno].desc : ""));
		}
	}
#endif
}

int
soc_bsc_addr(int unit, int devno)
{
    soc_bsc_bus_t *bscbus;

    bscbus = BSCBUS(unit);
    if (bscbus && bscbus->devs[devno]) {
        return bscbus->devs[devno]->saddr;
    }
    return 0;
}

const char *
soc_bsc_devname(int unit, int devno)
{
    soc_bsc_bus_t *bscbus;

    bscbus = BSCBUS(unit);
    if (bscbus && bscbus->devs[devno]) {
        return bscbus->devs[devno]->devname;
    }
    return NULL;
}

int
soc_bsc_device_count(int unit)
{
	COMPILER_REFERENCE(unit);
	return num_bsc_devices;
}

int
soc_bsc_devdesc_set(int unit, int devno, char *desc)
{
	soc_bsc_bus_t *bscbus;

	bscbus = BSCBUS(unit);
	if (bscbus && bscbus->devs[devno]) {
		bscbus->devs[devno]->desc = desc;
	}
	return SOC_E_NONE;
}

int
soc_bsc_devdesc_get(int unit, int devno, char **desc)
{
	soc_bsc_bus_t *bscbus;

	bscbus = BSCBUS(unit);
	if (bscbus && bscbus->devs[devno]) {
		*desc = bscbus->devs[devno]->desc;
	} else {
		*desc = "";
	}
	return SOC_E_NONE;
}

bsc_device_t *
soc_bsc_device(int unit, int devno)
{
	soc_bsc_bus_t *bscbus;

	bscbus = BSCBUS(unit);
	if (bscbus && bscbus->devs[devno]) {
		return bscbus->devs[devno];
	}
	return NULL;
}

int
soc_bsc_devopen(int unit, char* devname)
{
#if defined(BCM_FE2000_SUPPORT) || defined(INCLUDE_I2C_SFP)
	int devno;
#endif
	soc_bsc_bus_t *bscbus;

	/* Make sure that we're already attached, or go get attached */
	if (!soc_bsc_is_attached(unit)) {
		if (soc_bsc_attach(unit) < 0) {
			return SOC_E_FAIL;
		}
	}
	bscbus = BSCBUS(unit);

    COMPILER_REFERENCE(bscbus);

/* Coverity Fix : We don't need to search if no devices are supported */
#if defined(BCM_FE2000_SUPPORT) || defined(INCLUDE_I2C_SFP)
	for (devno = 0; devno < num_bsc_devices; devno++) {
		if (bscbus->devs[devno]) {
			if (sal_strcmp(bscbus->devs[devno]->devname, devname) == 0) {
				return devno;
			}
		}
	}
#endif
	return SOC_E_NOT_FOUND;
}

int
soc_bsc_devtype(int unit, int devno)
{
	soc_bsc_bus_t *bscbus;

	bscbus = BSCBUS(unit);
	if (bscbus && bscbus->devs[devno] && bscbus->devs[devno]->driver) {
		return bscbus->devs[devno]->driver->id;
	}
	return 0;
}

/*
 *
 * Function: soc_i2c_is_attached
 *
 * Purpose:  Return true if I2C bus driver is attached.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *
 * Returns:
 *    True if the bus driver for I2C is attached.
 *
 * Notes:
 *    None
 */
int
soc_bsc_is_attached(int unit)
{
    uint32 dev_idx;
    uint8 found = 0;
	soc_bsc_bus_t *bscbus;

    for(dev_idx = 0; dev_idx < soc_ndev ; ++dev_idx) {
        if(SOC_NDEV_IDX2DEV(dev_idx) == unit) {
            found = 1;
            break;
        }
    }
    if (!found) {
		return 0;
	}

	bscbus = BSCBUS(unit);
	if (bscbus == NULL) {
		return 0;
	}
	return (bscbus->flags & SOC_BSC_ATTACHED) != 0;
}

#else  /* BCM_FE2000_SUPPORT && INCLUDE_I2C */
typedef int _soc_i2c_bsc_devs_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_FE2000_SUPPORT && INCLUDE_I2C */

