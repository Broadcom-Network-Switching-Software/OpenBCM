/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM56xx I2C Device Driver for Texas Instruments LM63
 * The LM63 is a Remote Diode Digital Temperature Sensor with Integrated Fan
 * Control temperature sensor with an I2C bus interface. The host can query
 * the LM63 at any time to read the temperature. For more information,
 * see the Texas Instruments LM63 Datasheet.
 */
#include <sal/types.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <shared/bsl.h>


#define LM63_REG_TEMP        0x00

/*
 * Convert from hardware values to degree C.
 *
 * Note that you should be a bit careful with which arguments
 * these macros are called: arguments may be evaluated more than once.
 *
 * TEMP_FROM_REG converts hardware register value to software temperature
 * value. Bits[7:0]. This 8-bit value is in 2's complement format, with an
 * LSB equal to 1 degree C, the range can be from -128 to +127 degree C.
 * So, to ease the conversion we typecast the value to signed char.
 * Software value is also in fixed value format, each unit represents 1
 * C, the range of the converted value can be from -128 to +127.
 */
#define TEMP_FROM_REG(val) ((signed char)val)

/* Initial values */
#define LM63_OP_READ         0x1
#define LM63_OP_WRITE        0x2

/*
 * LM63 Hardware information : registers and their human-readable
 * counterparts.
 */
typedef struct lm63_s {
    uint8    temp;        /* Hardware info */
    int temp_val;
} lm63_t;

/*
 * Function: lm63_ioctl
 *
 * Purpose: read local temperature data from the LM63 temperature chip.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    opcode - CPU request (LM63_OP_READ)
 *    data - address of lm63_t data
 *    len - size of test data (not used)
 *
 * Returns:
 *     SOC_E_NONE - no error
 *     SOC_E_TIMEOUT - chip temperature reading unavailable or IO error.
 */
STATIC int
lm63_ioctl(int unit, int devno, int opcode, void* data, int len)
{
    int r = SOC_E_NONE;
    lm63_t* l = (lm63_t*)data;
    uint8 saddr = soc_i2c_addr(unit, devno);

    if (opcode == LM63_OP_READ) {

        r = soc_i2c_read_byte_data(unit, saddr, LM63_REG_TEMP, &l->temp);
        if (r < 0) {
            return r;
        }

        soc_i2c_device(unit, devno)->rbyte += 1;

        /* Convert the register values to temperature values */
        l->temp_val = TEMP_FROM_REG(l->temp);
    }
    return r;
}

/*
 * LM63 control structure.
 */
typedef struct {
    int    sleep;                /* Thread sleep time (0 to exit) */
    int    temp[MAX_I2C_DEVICES];        /* last temp values */
} lm63_dev_info_t;

static lm63_dev_info_t *lm63_info[SOC_MAX_NUM_DEVICES];

#define C2F(_d) ((9*(_d)/5) + 32)   /* celsius to fahrenheit */
#define MINDIFF 1                   /* at least 1 degree change */

/*
 * Function: lm63_temp_show
 *
 * Purpose:  Query LM63 chip for temperature and report status in
 *           degrees Celsius and Degrees Fahrenheit.
 *
 * Parameters:
 *    unit    - switch unit
 *    dev    - i2c device
 *    force    - if true, show temp even if it hasn't changed
 */
STATIC void
lm63_temp_show(int unit, int dev, int force)
{
    lm63_t        lm63;
    lm63_dev_info_t    *lmi;
    int            ctemp, ftemp, lasttemp;
    int            difftemp;

    lmi = lm63_info[unit];
    if (lmi == NULL) {
        return;
    }

    /* Read back stats */
    if (lm63_ioctl(unit, dev, LM63_OP_READ, &lm63, sizeof(lm63_t)) < 0) {
        LOG_CLI((BSL_META_U(unit, "%s: ERROR: device not responding\n"),
                 soc_i2c_devname(unit, dev)));
        lmi->sleep = 0;        /* exit thread */
        return;
    }

    ctemp = lm63.temp_val;
    lasttemp = lmi->temp[dev];

    if (lasttemp == 0) {    /* first time through */
        difftemp = MINDIFF;
    } else {
        difftemp = lasttemp - ctemp;
        if (difftemp < 0) {
            difftemp = -difftemp;
        }
    }

    if (force || difftemp >= MINDIFF) {
    lmi->temp[dev] = ctemp;

    /* 20 degree C change: something is wrong */
    if (difftemp > 20) {
        if (force) {
            LOG_CLI((BSL_META_U(unit, "%s: NOTICE: Temperature Unavailable\n"),
                     soc_i2c_devname(unit, dev)));
            if (lasttemp > 0) {
                ftemp = C2F(lasttemp);
                LOG_CLI((BSL_META_U(unit, "%s: Last Temperature %dC, %dF\n"),
                         soc_i2c_devname(unit, dev),
                         lasttemp, ftemp));
            }
        }
        return;
    }

    ftemp = C2F(ctemp);

    LOG_CLI((BSL_META_U(unit, "%s: Temperature %dC, %dF\n"),
                 soc_i2c_devname(unit, dev),
                 ctemp, ftemp));
    }
}

/*
 * Function: lm63_thread
 *
 * Purpose: Poll LM63 devices and report stats every poll_delay seconds
 *
 * Parameters:
 *    unitp    - void * version of unit
 */
STATIC void
lm63_thread(void *unitp)
{
    int    unit, dev, ndev;
    lm63_dev_info_t    *lmi;

    unit = PTR_TO_INT(unitp);
    lmi = lm63_info[unit];

    while (lmi && lmi->sleep) {
        ndev = soc_i2c_device_count(unit);
        for (dev = 0; dev < ndev; dev++) {
            if (soc_i2c_devtype(unit, dev) == LM63_DEVICE_TYPE) {
                lm63_temp_show(unit, dev, FALSE);
            }
        }
        sal_usleep(lmi->sleep * SECOND_USEC);
    }
    LOG_CLI((BSL_META_U(unit, "unit %d: Thermal monitoring completed\n"), unit));
    sal_thread_exit(0);
}


STATIC int
lm63_init(int unit, int devno, void* data, int len)
{
    soc_i2c_devdesc_set(unit, devno, "LM63 Temperature Sensor");

    /* Init devinfo */
    if (lm63_info[unit] == NULL) {
        lm63_info[unit] = sal_alloc(sizeof(lm63_dev_info_t), "lm63_info");
        if (lm63_info[unit] == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(lm63_info[unit], 0, sizeof(lm63_dev_info_t));
    }
    return SOC_E_NONE;
}

/*
 * Function: soc_i2c_lm63_monitor
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
soc_i2c_lm63_monitor(int unit, int enable, int nsecs)
{
    if (!soc_i2c_is_attached(unit)) {
        soc_i2c_attach(unit, 0, 0);
    }
    if (lm63_info[unit] == NULL) {
        return;
    }
    if (!enable) {
        lm63_info[unit]->sleep = 0;
        return;
    }
    if (lm63_info[unit]->sleep) {
        return;
    }
    lm63_info[unit]->sleep = nsecs;

    sal_thread_create("bcmTEMP",
              SAL_THREAD_STKSZ,
              50,
              (void (*)(void*))lm63_thread,
              INT_TO_PTR(unit));
    LOG_CLI((BSL_META_U(unit, "unit %d: Thermal monitoring enabled\n"), unit));
}

/*
 * Function: soc_i2c_lm63_temperature_show
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
soc_i2c_lm63_temperature_show(int unit)
{
    int dev, ndev;

    if (!soc_i2c_is_attached(unit)) {
        soc_i2c_attach(unit, 0, 0);
    }
    if (lm63_info[unit] == NULL) {
        return;
    }
    ndev = soc_i2c_device_count(unit);
    for (dev = 0; dev < ndev; dev++) {
        if (soc_i2c_devtype(unit, dev) == LM63_DEVICE_TYPE) {
            lm63_temp_show(unit, dev, TRUE);
        }
    }
}

/* NOT USED */
STATIC int
lm63_read(int unit, int devno, uint16 addr, uint8* data, uint32* len)
{
    return SOC_E_NONE;
}

/* NOT USED */
STATIC int
lm63_write(int unit, int devno, uint16 addr, uint8* data, uint32 len)
{
    return SOC_E_NONE;
}

/* LM63 Temperature Sensor Driver callout */
i2c_driver_t _soc_i2c_lm63_driver = {
    0x0, 0x0, /* System assigned bytes */
    LM63_DEVICE_TYPE,
    lm63_read,
    lm63_write,
    lm63_ioctl,
    lm63_init,
    NULL,
};
