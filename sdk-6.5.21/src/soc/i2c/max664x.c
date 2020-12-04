/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM I2C Device Driver for MAXIM 664x
 * The MAX664x (max6646/7/9)is a temperature sensor with an I2C bus interface. The host can query
 * the MAX664x at any time to read the temperature. For more information,
 * see MAX664x data sheet.
 */
#include <sal/types.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <shared/bsl.h>
/* The MAX664X registers */
#define MAX664X_REG_LOCAL_TEMP_R	           0x00
#define MAX664X_REG_REMOTE_TEMP_R    	       0x01 
#define MAX664X_REG_STATUS_BYTE_R              0x02
#define MAX664X_REG_CONFIG_BYTE_R              0x03
#define MAX664X_REG_CONVERSION_RATE_BYTE_R     0x04
#define MAX664X_REG_LOCAL_ALERT_HI_LIMIT_R     0x05
#define MAX664X_REG_LOCAL_ALERT_LO_LIMIT_R     0x06
#define MAX664X_REG_REMOTE_ALERT_HI_LIMIT_R    0x07
#define MAX664X_REG_REMOTE_ALERT_LO_LIMIT_R    0x08
#define MAX664X_REG_CONFIG_BYTE_W              0x09
#define MAX664X_REG_CONVERSTION_RATE_BYTE_W    0x0a
#define MAX664X_REG_LOCAL_ALERT_HI_LIMIT_W     0x0b
#define MAX664X_REG_LOCAL_ALERT_LO_LIMIT_W     0x0c
#define MAX664X_REG_REMOTE_ALERT_HI_LIMIT_W    0x0d
#define MAX664X_REG_REMOTE_ALERT_LO_LIMIT_W    0x0e 
#define MAX664X_REG_ONE_SHOT                   0x0f
#define MAX664X_REG_REMOTE_EXTENDED_TEMP_R     0x10
#define MAX664X_REG_LOCAL_EXTENDED_TEMP_R      0x11
#define MAX664X_REG_REMOTE_OVERTEMP_LIMIT_RW   0x19
#define MAX664X_REG_LOCAL_OVERTEMP_LIMIT_RW    0x20
#define MAX664X_REG_OVERTEMP_HYSTERESIS        0x21
#define MAX664X_REG_FAULT_QUEUE                0x22
#define MAX664X_REG_MANUFACTURER_ID            0xFE
#define MAX664X_REG_REVISION                   0xFF

/* Initial values */
#define MAX664X_OP_READ         0x1
#define MAX664X_OP_WRITE        0x2

/*
 * MAX664X Hardware information : registers and their human-readable
 * counterparts.
 */
typedef struct max664x_reg_s {
    uint8	local_temp;
    uint8	remote_temp;
    uint8	config;
    uint8   status;
} max664x_reg_t;

/*
 * Function: max664x_ioctl
 *
 * Purpose: read temperature data from the MAX664X temperature probe
 *          chip.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    opcode - CPU request (MAX664X_OP_READ)
 *    data - address of max664x_reg_t data
 *    len - size of test data (not used)
 *
 * Returns:
 *     SOC_E_NONE - no error
 *     SOC_E_TIMEOUT - chip temperature reading unavailable or IO error.
 */
STATIC int
max664x_ioctl(int unit, int devno, int opcode, void* data, int len)
{
    int rv = SOC_E_NONE;
    max664x_reg_t* maxreg = (max664x_reg_t*)data;
    uint8 saddr = soc_i2c_addr(unit, devno);

    if (opcode == MAX664X_OP_READ) {

        rv = soc_i2c_read_byte_data(unit, saddr, MAX664X_REG_LOCAL_TEMP_R, &maxreg->local_temp);
        if (rv < 0) {
            return rv;
        }

        soc_i2c_device(unit, devno)->rbyte += 1;
        
        rv = soc_i2c_read_byte_data(unit, saddr, MAX664X_REG_REMOTE_TEMP_R, &maxreg->remote_temp);
        if (rv < 0) {
            return rv;
        }
        soc_i2c_device(unit, devno)->rbyte += 1;
        
        rv = soc_i2c_read_byte_data(unit, saddr,  MAX664X_REG_CONFIG_BYTE_R, &maxreg->config);
        if (rv < 0) {
            return rv;
        }
        soc_i2c_device(unit, devno)->rbyte += 1;

        rv = soc_i2c_read_byte_data(unit, saddr,  MAX664X_REG_STATUS_BYTE_R, &maxreg->status);
        if (rv < 0) {
            return rv;
        }
        soc_i2c_device(unit, devno)->rbyte += 1;


    } else if (opcode == MAX664X_OP_WRITE) {
        
        rv = soc_i2c_write_byte_data(unit, saddr, MAX664X_REG_CONFIG_BYTE_W, maxreg->config);
        if (rv < 0) {
            return rv;
        }
        
        soc_i2c_device(unit, devno)->tbyte += 1;

    } else {
        LOG_CLI((BSL_META_U(unit,
                            "invalid command for max6649 - must be read or write\n")));
        rv = SOC_E_UNAVAIL;
    }

    return rv;
}

/*
 * MAX664X control structure.
 */
typedef struct {
    int	sleep;				/* Thread sleep time (0 to exit) */
    int	remote_temp[MAX_I2C_DEVICES];		/* last temp values */
    int	local_temp[MAX_I2C_DEVICES];		/* last temp values */
} max664x_dev_info_t;

static max664x_dev_info_t *max664x_info[SOC_MAX_NUM_DEVICES];

#define	C2F(_d)		((9*(_d)/5) + 32)	/* celsius to farenheit */

#define	MINDIFF		1			/* at least 1 degree change */

/*
 * Function: max664x_temp_show
 *
 * Purpose:  Query MAX664X chip for temperature and report status in
 *           degrees Celcius and Degrees Fahrenheit.
 *
 * Parameters:
 *	unit	- switch unit
 *	dev	    - i2c device
 *	force	- if true, show temp even if it hasn't changed
 */
STATIC void
max664x_temp_show(int unit, int dev, int force)
{
    max664x_reg_t       maxreg;
    max664x_dev_info_t	*max_info;
    int			        ctemp, ftemp, lasttemp;
    int			        difftemp;
    int                 cnt;

    max_info = max664x_info[unit];
    if (max_info == NULL) {
        return;
    }

    /* Read back stats */
    if (max664x_ioctl(unit, dev, MAX664X_OP_READ, &maxreg, sizeof(max664x_reg_t)) < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "%s: ERROR: device not responding\n"),
                 soc_i2c_devname(unit, dev)));
	max_info->sleep = 0;		/* exit thread */
	return;
    }

    
    /* loop 2x for local and remote temp */
    for (cnt=0; cnt<2; cnt++) {
        if (cnt==0) {
            ctemp = maxreg.local_temp;
            lasttemp = max_info->local_temp[dev];
        } else {
            ctemp = maxreg.remote_temp;
            lasttemp = max_info->remote_temp[dev];
        }

        if (lasttemp == 0) {	/* first time through */
            difftemp = MINDIFF;
        } else {
            difftemp = lasttemp - ctemp;
            if (difftemp < 0) {
                difftemp = -difftemp;
            }
        }

        if (force || difftemp >= MINDIFF) {

            if (cnt==0) {
                max_info->local_temp[dev] = ctemp;
            } else {
                max_info->remote_temp[dev] = ctemp;
            }
            
            /* 20 degree C change: something is wrong */
            if (difftemp > 20) {
                if (force) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%s: NOTICE: temperature unavailable\n"),
                             soc_i2c_devname(unit, dev)));
                    if (lasttemp > 0) {
                        ftemp = C2F(lasttemp);
                        LOG_CLI((BSL_META_U(unit,
                                            "%s: last temperature %d.%dC, %d.%dF\n"),
                                 soc_i2c_devname(unit, dev),
                                 lasttemp/10, lasttemp%10,
                                 ftemp/10, ftemp%10));
                    }
                }
                return;
            }
            
            ftemp = C2F(ctemp);

            if (cnt==0) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: local temperature %dC, %dF\n"),
                         soc_i2c_devname(unit, dev),
                         ctemp,
                         ftemp));
            } else {

                LOG_CLI((BSL_META_U(unit,
                                    "%s: sirius temperature %dC, %dF\n"),
                         soc_i2c_devname(unit, dev),
                         ctemp,
                         ftemp));
            }

        }
    }
}


/*
 * Function: max664x_thread
 *
 * Purpose: Poll MAX664X devices and report stats every poll_delay seconds
 *
 * Parameters:
 *    unitp	- void * version of unit
 */
STATIC void
max664x_thread(void *unitp)
{
    int			unit, dev, ndev;
    max664x_dev_info_t	*maxinfo;
    
    unit = PTR_TO_INT(unitp);
    maxinfo = max664x_info[unit];
    
    while (maxinfo && maxinfo->sleep) {
        ndev = soc_i2c_device_count(unit);
        for (dev = 0; dev < ndev; dev++) {
            if (soc_i2c_devtype(unit, dev) == MAX664X_DEVICE_TYPE) {
                max664x_temp_show(unit, dev, FALSE);
            }
        }
        sal_usleep(maxinfo->sleep * SECOND_USEC);
    }
    LOG_CLI((BSL_META_U(unit,
                        "unit %d: thermal monitoring completed\n"), unit));
    sal_thread_exit(0);
}

/*
 * Function: max664x_init
 *
 * Purpose: Initialize the MAX664X temperature sensor chip.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    data - not used
 *    len - not used
 *
 * Returns:
 *     SOC_E_NONE - no failure
 *     SOC_E_TIMEOUT - chip or device access failure.
 */
STATIC int
max664x_init(int unit, int devno,
	  void* data, int len)
{
    int	rv;
    max664x_reg_t maxreg;

    soc_i2c_devdesc_set(unit, devno, "MAX664X Temperature Sensor");

    /* Init chip */
    maxreg.config = 0x80; /* bit 7 - mask out alert interrupts, bit 6 RUN=0 enables temp sensor */

    rv = max664x_ioctl(unit, devno, MAX664X_OP_WRITE, &maxreg, sizeof(max664x_reg_t));
    if (rv < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "%s: ERROR: device did not initialize: %s\n"),
                 soc_i2c_devname(unit, devno),
                 soc_errmsg(rv)));
    }
    LOG_VERBOSE(BSL_LS_SOC_I2C, (BSL_META_U(unit,
                "MAX664X hardware status=0x%x\n"), maxreg.status));

    /* Init devinfo */
    if (max664x_info[unit] == NULL) {
        max664x_info[unit] = sal_alloc(sizeof(max664x_dev_info_t), "max664x_info");
        if (max664x_info[unit] == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(max664x_info[unit], 0, sizeof(max664x_dev_info_t));
    }
    
    return SOC_E_NONE;
}

/*
 * Function: soc_i2c_max664x_monitor
 *
 * Purpose:
 *     Monitor configuration: start or stop a temperature probe
 *     task to monitor temperatures every nsecs.
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    enable - start or stop a temperature probe task
 *    nsecs - number of seconds delay, between succesive queries.
 *  Returns:
 *    None
 *  Notes:
 *    None
 */
void
soc_i2c_max664x_monitor(int unit, int enable, int nsecs)
{
    if (!soc_i2c_is_attached(unit)) {
        soc_i2c_attach(unit, 0, 0);
    }
    if (max664x_info[unit] == NULL) {
        return;
    }
    if (!enable) {
        max664x_info[unit]->sleep = 0;
        return;
    }
    if (max664x_info[unit]->sleep) {
        return;
    }
    max664x_info[unit]->sleep = nsecs;
    
    sal_thread_create("bcmTEMP",
                      SAL_THREAD_STKSZ,
                      50,
                      (void (*)(void*))max664x_thread,
                      INT_TO_PTR(unit));
    LOG_CLI((BSL_META_U(unit,
                        "unit %d: thermal monitoring enabled\n"), unit));
}


/*
 * Function: soc_i2c_max664x_temperature_show
 *
 * Purpose: Show all temperature stats on all chips.
 *
 * Parameters:
 *   unit - StrataSwitch device number or I2C bus number
 *
 * Returns:
 *   None
 */
void
soc_i2c_max664x_temperature_show(int unit)
{
    int		dev, ndev;
    
    if (!soc_i2c_is_attached(unit)) {
        soc_i2c_attach(unit, 0, 0);
    }
    if (max664x_info[unit] == NULL) {
        return;
    }
    ndev = soc_i2c_device_count(unit);
    for (dev = 0; dev < ndev; dev++) {
        if (soc_i2c_devtype(unit, dev) == MAX664X_DEVICE_TYPE) {
            max664x_temp_show(unit, dev, TRUE);
        }
    }
}

/* NOT USED */
STATIC int
max664x_read(int unit, int devno,
             uint16 addr, uint8* data, uint32* len)
{
    return SOC_E_NONE;
}

/* NOT USED */
STATIC int
max664x_write(int unit, int devno,
              uint16 addr, uint8* data, uint32 len)
{
    return SOC_E_NONE;
}


/* MAX664X Temperature Sensor Driver callout */
i2c_driver_t _soc_i2c_max664x_driver = {
    0x0, 0x0, /* System assigned bytes */
    MAX664X_DEVICE_TYPE,
    max664x_read,
    max664x_write,
    max664x_ioctl,
    max664x_init,
    NULL,
};

