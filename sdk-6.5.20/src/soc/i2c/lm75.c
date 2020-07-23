/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM56xx I2C Device Driver for National Semiconductor LM75
 * The LM75 is a temperature sensor, Delta-Sigma ADC, and digital
 * over-temp detector with an I2C bus interface. The host can query
 * the LM75 at any time to read the temperature. For more information,
 * see the National Semiconductor LM75 Datasheet.
 */
#include <sal/types.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <shared/bsl.h>
/* The LM75 registers */
#define LM75_REG_TEMP		0x00
#define LM75_REG_CONF		0x01
#define LM75_REG_THYST		0x02
#define LM75_REG_TOS		0x03

/*
 * Function: lm75_limit
 *
 * Purpose:  Range check the input value.
 *
 * Parameters:
 *   value - input comparator value
 *   low - low value
 *   high - high value
 *
 * Returns:
 *    value or high or low value if out of range.
 */
STATIC INLINE int
lm75_limit(long value, long low, long high)
{
    if (value < low)
	return low;
    else if (value > high)
	return high;
    else
	return value;
}

/*
 * Function: lm75_swap
 *
 * Purpose: byte swap a 16 bit value returned from LM75
 *
 * Parameters:
 *    value - input 16 bit value.
 *
 * Returns:
 *    byte swapped value.
 */
STATIC uint16
lm75_swap(uint16 val)
{
    return (val >> 8) | (val << 8);
}

/*
 * Convert from hardware values to tenths of a degree C.
 *
 * Rounding and limit checking is only done on the TO_REG variants.
 * Note that you should be a bit careful with which arguments
 * these macros are called: arguments may be evaluated more than once.
 *
 * TEMP_FROM_REG converts hardware register value to software temperature
 * value. Bit[15:7] (i.e. the most significant 9 bit) of hardware register is
 * the temperature. These 9-bit value is in 2's complement format, each unit
 * represents 0.5 degree C, the range can be from -128.0 to +127.5 degree C.
 * Softwrae value is also in fixed value format, each unit represents 0.1
 * C, the range of the converted value can be from -1280 to +1275. The
 * conversion first signed extended the 16 bit signed value to 32 bit signed
 * value to avoid overflow. Using arithmatic division by 128 instead of right
 * shift 7 bit to avoid the complier implementation-dependent behavior on
 * most significant filled bits on right shift operation.
 */
#define TEMP_FROM_REG(val) ((int)(int16)val * 5 / 128)
#define TEMP_TO_REG(val)   (lm75_limit((val<0?(0x200+\
			     ((val)/5))<<7:(((val) + 2) / 5) << 7),0,0xffff))

/* Initial values */
#define LM75_INIT_TEMP_OS    600	/* overtemp shutdown: 60.0 */
#define LM75_INIT_TEMP_HYST  500	/* clear alarm temp: 50.0 */
#define LM75_OP_READ         0x1
#define LM75_OP_WRITE        0x2

/*
 * LM75 Hardware information : registers and their human-readable
 * counterparts.
 */
typedef struct lm75_s {
    uint16	temp;		/* Hardware info */
    uint8	conf;
    uint16	t_hist;
    uint16	t_os;
    int		temp_val;	/* Human readable values in fixed point */
    int		hist_val;
    int		os_val;
} lm75_t;

/*
 * Function: lm75_ioctl
 *
 * Purpose: read temperature data from the LM75 temperature probe
 *          chip.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    opcode - CPU request (LM75_OP_READ)
 *    data - address of lm75_t data
 *    len - size of test data (not used)
 *
 * Returns:
 *     SOC_E_NONE - no error
 *     SOC_E_TIMEOUT - chip temperature reading unavailable or IO error.
 */
STATIC int
lm75_ioctl(int unit, int devno,
	   int opcode, void* data, int len)
{
    int r = SOC_E_NONE;
    lm75_t* l = (lm75_t*)data;
    uint8 saddr = soc_i2c_addr(unit, devno);
    int t_os, t_hist;

    if (opcode == LM75_OP_READ) {

	r = soc_i2c_read_word_data(unit, saddr, LM75_REG_TEMP, &l->temp);
	if (r < 0) {
	    return r;
	}

	soc_i2c_device(unit, devno)->rbyte += 2;

	r = soc_i2c_read_word_data(unit, saddr, LM75_REG_TOS, &l->t_os);
	if (r < 0) {
	    return r;
	}

	soc_i2c_device(unit, devno)->rbyte += 2;

	r = soc_i2c_read_word_data(unit, saddr, LM75_REG_THYST, &l->t_hist);
	if (r < 0) {
	    return r;
	}

	soc_i2c_device(unit, devno)->rbyte += 2;

	/* Byte swap the 16 bit registers */
	l->temp = lm75_swap(l->temp);
	l->t_hist = lm75_swap(l->t_hist);
	l->t_os = lm75_swap(l->t_os);

	/* Convert the register values to temperature values */
	l->temp_val = TEMP_FROM_REG(l->temp);
	l->hist_val = TEMP_FROM_REG(l->t_hist);
	l->os_val = TEMP_FROM_REG(l->t_os);
    } else {
	/* Write regvalues to HW */
	t_os = l->t_os;
	t_hist = l->t_hist;
	/* Convert to register format */
	t_hist = TEMP_TO_REG(t_hist);
	t_os = TEMP_TO_REG(t_os);
	/* Byte swap */
	t_hist = lm75_swap(t_hist);
	t_os = lm75_swap(t_os);

	/* Feed back to chip */
	r = soc_i2c_write_word_data(unit, saddr, LM75_REG_TOS, t_os);
	if (r < 0) {
	    return r;
	}

	soc_i2c_device(unit, devno)->tbyte += 2;

	r = soc_i2c_write_word_data(unit, saddr, LM75_REG_THYST, t_hist);
	if (r < 0) {
	    return r;
	}

	soc_i2c_device(unit, devno)->tbyte += 2;

	r = soc_i2c_write_byte_data(unit, saddr, LM75_REG_TEMP, l->conf);
	if (r < 0) {
	    return r;
	}

	soc_i2c_device(unit, devno)->tbyte++;
    }
    return r;
}

/*
 * LM75 control structure.
 */
typedef struct {
    int	sleep;				/* Thread sleep time (0 to exit) */
    int	temp[MAX_I2C_DEVICES];		/* last temp values */
} lm75_dev_info_t;

static lm75_dev_info_t *lm75_info[SOC_MAX_NUM_DEVICES];

#define	C2F(_d)		((9*(_d)/5) + 320)	/* celsius*10 to farenheit*10 */

#define	MINDIFF		10			/* at least 1 degree change */

/*
 * Function: lm75_temp_show
 *
 * Purpose:  Query LM75 chip for temperature and report status in
 *           degrees Celcius and Degrees Fahrenheit.
 *
 * Parameters:
 *	unit	- switch unit
 *	dev	- i2c device
 *	force	- if true, show temp even if it hasn't changed
 */
STATIC void
lm75_temp_show(int unit, int dev, int force)
{
    lm75_t		lm75;
    lm75_dev_info_t	*lmi;
    int			ctemp, ftemp, lasttemp;
    int			difftemp, alarm_temp, shut_temp;

    lmi = lm75_info[unit];
    if (lmi == NULL) {
	return;
    }

    /* Read back stats */
    if (lm75_ioctl(unit, dev, LM75_OP_READ, &lm75, sizeof(lm75_t)) < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "%s: ERROR: device not responding\n"),
                 soc_i2c_devname(unit, dev)));
        lmi->sleep = 0;		/* exit thread */
        return;
    }

    ctemp = lm75.temp_val;
    lasttemp = lmi->temp[dev];

    if (lasttemp == 0) {	/* first time through */
	difftemp = MINDIFF;
    } else {
	difftemp = lasttemp - ctemp;
	if (difftemp < 0) {
	    difftemp = -difftemp;
	}
    }

    if (force || difftemp >= MINDIFF) {
	lmi->temp[dev] = ctemp;
	alarm_temp = lm75.hist_val;
	shut_temp = lm75.os_val;

	/* 20 degree C change: something is wrong */
	if (difftemp > 200) {
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

	if (ctemp > shut_temp) {
	    LOG_CLI((BSL_META_U(unit,
                                "%s: temperature %d.%dC, %d.%dF "
                                "WARNING: over shutdown %d.%dC\n"),
                     soc_i2c_devname(unit, dev),
                     ctemp/10, ctemp%10,
                     ftemp/10, ftemp%10,
                     shut_temp/10, shut_temp%10));
	} else if (ctemp > alarm_temp) {
	    LOG_CLI((BSL_META_U(unit,
                                "%s: temperature %d.%dC, %d.%dF "
                                "WARNING: over alarm %d.%dC\n"),
                     soc_i2c_devname(unit, dev),
                     ctemp/10, ctemp%10,
                     ftemp/10, ftemp%10,
                     alarm_temp/10, alarm_temp%10));
	} else {
	    LOG_CLI((BSL_META_U(unit,
                                "%s: temperature %d.%dC, %d.%dF\n"),
                     soc_i2c_devname(unit, dev),
                     ctemp/10, ctemp%10,
                     ftemp/10, ftemp%10));
	}

    }
}

/*
 * Function: lm75_thread
 *
 * Purpose: Poll LM75 devices and report stats every poll_delay seconds
 *
 * Parameters:
 *    unitp	- void * version of unit
 */
STATIC void
lm75_thread(void *unitp)
{
    int			unit, dev, ndev;
    lm75_dev_info_t	*lmi;

    unit = PTR_TO_INT(unitp);
    lmi = lm75_info[unit];

    while (lmi && lmi->sleep) {
	ndev = soc_i2c_device_count(unit);
	for (dev = 0; dev < ndev; dev++) {
	    if (soc_i2c_devtype(unit, dev) == LM75_DEVICE_TYPE) {
		lm75_temp_show(unit, dev, FALSE);
	    }
	}
	sal_usleep(lmi->sleep * SECOND_USEC);
    }
    LOG_CLI((BSL_META_U(unit,
                        "unit %d: thermal monitoring completed\n"), unit));
    sal_thread_exit(0);
}

/*
 * Function: lm75_init
 *
 * Purpose: Initialize the LM75 temperature sensor chip.
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
lm75_init(int unit, int devno,
	  void* data, int len)
{
    int		rv;
    lm75_t	lm75;

    soc_i2c_devdesc_set(unit, devno, "LM75 Temperature Sensor");

    /* Init chip */
    lm75.t_hist = LM75_INIT_TEMP_HYST;
    lm75.t_os = LM75_INIT_TEMP_OS;
    lm75.conf = 0;
    rv = lm75_ioctl(unit, devno, LM75_OP_WRITE, &lm75, sizeof(lm75_t));
    if (rv < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "%s: ERROR: device did not initialize: %s\n"),
                 soc_i2c_devname(unit, devno),
                 soc_errmsg(rv)));
    }

    /* Init devinfo */
    if (lm75_info[unit] == NULL) {
	lm75_info[unit] = sal_alloc(sizeof(lm75_dev_info_t), "lm75_info");
	if (lm75_info[unit] == NULL) {
	    return SOC_E_MEMORY;
	}
	sal_memset(lm75_info[unit], 0, sizeof(lm75_dev_info_t));
    }

    return SOC_E_NONE;
}

/*
 * Function: soc_i2c_lm75_monitor
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
soc_i2c_lm75_monitor(int unit, int enable, int nsecs)
{
    if (!soc_i2c_is_attached(unit)) {
	soc_i2c_attach(unit, 0, 0);
    }
    if (lm75_info[unit] == NULL) {
	return;
    }
    if (!enable) {
	lm75_info[unit]->sleep = 0;
	return;
    }
    if (lm75_info[unit]->sleep) {
	return;
    }
    lm75_info[unit]->sleep = nsecs;

    sal_thread_create("bcmTEMP",
		      SAL_THREAD_STKSZ,
		      50,
		      (void (*)(void*))lm75_thread,
		      INT_TO_PTR(unit));
    LOG_CLI((BSL_META_U(unit,
                        "unit %d: thermal monitoring enabled\n"), unit));
}


/*
 * Function: soc_i2c_lm75_temperature_show
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
soc_i2c_lm75_temperature_show(int unit)
{
    int		dev, ndev;

    if (!soc_i2c_is_attached(unit)) {
	soc_i2c_attach(unit, 0, 0);
    }
    if (lm75_info[unit] == NULL) {
	return;
    }
    ndev = soc_i2c_device_count(unit);
    for (dev = 0; dev < ndev; dev++) {
	if (soc_i2c_devtype(unit, dev) == LM75_DEVICE_TYPE) {
	    lm75_temp_show(unit, dev, TRUE);
	}
    }
}

/* NOT USED */
STATIC int
lm75_read(int unit, int devno,
	  uint16 addr, uint8* data, uint32* len)
{
    return SOC_E_NONE;
}

/* NOT USED */
STATIC int
lm75_write(int unit, int devno,
	   uint16 addr, uint8* data, uint32 len)
{
    return SOC_E_NONE;
}


/* LM75 Temperature Sensor Driver callout */
i2c_driver_t _soc_i2c_lm75_driver = {
    0x0, 0x0, /* System assigned bytes */
    LM75_DEVICE_TYPE,
    lm75_read,
    lm75_write,
    lm75_ioctl,
    lm75_init,
    NULL,
};
