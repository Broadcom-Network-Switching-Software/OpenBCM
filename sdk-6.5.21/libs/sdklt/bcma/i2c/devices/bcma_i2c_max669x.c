/*! \file bcma_i2c_isl68127.c
 *
 *  Application to control power regulator ISL68127.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcma/i2c/bcma_i2ccmd_i2c.h>
#include <bcma/i2c/bcma_i2c_device.h>
#include <bcmbd/bcmbd_i2c.h>
#include <bcma/io/bcma_io_file.h>
#include <bcmbd/bcmbd.h>

#define BSL_LOG_MODULE  BSL_LS_APPL_I2C

/*! The MAX669X registers */

/*! The MAX669X local temperature regs */
#define MAX669X_REG_LOCAL_TEMP_R               0x00

/*! The MAX669X remote temperature regs */
#define MAX669X_REG_REMOTE_TEMP_R              0x01

/*! The MAX669X status regs */
#define MAX669X_REG_STATUS_BYTE_R              0x02

/*! The MAX669X config byte regs */
#define MAX669X_REG_CONFIG_BYTE_R              0x03

/*! The MAX669X converstion rate regs */
#define MAX669X_REG_CONVERSION_RATE_BYTE_R     0x04

/*! The MAX669X local alert hi regs */
#define MAX669X_REG_LOCAL_ALERT_HI_LIMIT_R     0x05

/*! The MAX669X local alert low regs */
#define MAX669X_REG_LOCAL_ALERT_LO_LIMIT_R     0x06

/*! The MAX669X remote alert hi regs */
#define MAX669X_REG_REMOTE_ALERT_HI_LIMIT_R    0x07

/*! The MAX669X remote alert low regs */
#define MAX669X_REG_REMOTE_ALERT_LO_LIMIT_R    0x08

/*! The MAX669X config byte regs */
#define MAX669X_REG_CONFIG_BYTE_W              0x09

/*! The MAX669X converstion rate regs */
#define MAX669X_REG_CONVERSTION_RATE_BYTE_W    0x0a

/*! The MAX669X local alert hi regs */
#define MAX669X_REG_LOCAL_ALERT_HI_LIMIT_W     0x0b

/*! The MAX669X local alert low regs */
#define MAX669X_REG_LOCAL_ALERT_LO_LIMIT_W     0x0c

/*! The MAX669X remote alert hi regs */
#define MAX669X_REG_REMOTE_ALERT_HI_LIMIT_W    0x0d

/*! The MAX669X remote alert low regs */
#define MAX669X_REG_REMOTE_ALERT_LO_LIMIT_W    0x0e

/*! The MAX669X one shot regs */
#define MAX669X_REG_ONE_SHOT                   0x0f

/*! The MAX669X remote extended temp regs */
#define MAX669X_REG_REMOTE_EXTENDED_TEMP_R     0x10

/*! The MAX669X local extended temp regs */
#define MAX669X_REG_LOCAL_EXTENDED_TEMP_R      0x11

/*! The MAX669X remote over temp regs */
#define MAX669X_REG_REMOTE_OVERTEMP_LIMIT_RW   0x19

/*! The MAX669X local over temp regs */
#define MAX669X_REG_LOCAL_OVERTEMP_LIMIT_RW    0x20

/*! The MAX669X over temp regs */
#define MAX669X_REG_OVERTEMP_HYSTERESIS        0x21

/*! The MAX669X fault queue regs */
#define MAX669X_REG_FAULT_QUEUE                0x22

/*! The MAX669X manufacture ID regs */
#define MAX669X_REG_MANUFACTURER_ID            0xFE

/*! The MAX669X revision regs */
#define MAX669X_REG_REVISION                   0xFF

/*! Initial values */

/*! The MAX669X read operation */
#define MAX669X_OP_READ         0x1

/*! The MAX669X write operation */
#define MAX669X_OP_WRITE        0x2

/*! Maximum temperature devices */
#define MAX_I2C_TEMP_DEVICES    10

/*!
 * MAX669X Hardware information : registers and their human-readable
 * counterparts.
 */
typedef struct max669x_reg_s {
    /*! Local temp. */
    uint8_t local_temp;

    /*! Remote temperature 1 */
    uint8_t remote_temp_1;

    /*! Remote temperature 2 */
    uint8_t remote_temp_2;

    /*! Configuration */
    uint8_t config;

    /*! Status */
    uint8_t status;
} max669x_reg_t;

/*! at least 1 degree change */
#define MINDIFF         1

/*! Max 664x temperature sensor */
#define MAX669X_DEVICE_TYPE  0x0000004d /* Device ID */

/*! Temperature sensor 18 */
#define I2C_MAX669X_18       "temp_18"

/*! Temperature sensor address 0x18 */
#define I2C_MAX669X_SADDR18   0x18

/*! Max 664x temperature sensor */
#define I2C_MAX669X_19       "temp_19"

/*! Temperature sensor address 0x19 */
#define I2C_MAX669X_SADDR19   0x19

/*
 * Function: max669x_ioctl
 *
 * Purpose: read temperature data from the MAX669X temperature probe
 *          chip.
 *
 * Parameters:
 *    unit   - StrataSwitch device number or I2C bus number
 *    saddr  - chip address
 *    opcode - CPU request (MAX669X_OP_READ)
 *    data   - address of max669x_reg_t data
 *    len    - size of test data (not used)
 *
 * Returns:
 *     SHR_E_NONE - no error
 *     SHR_E_TIMEOUT - chip temperature reading unavailable or IO error.
 */
static int
max669x_ioctl(int unit, int saddr, int opcode, void *data, int len)
{
    int rv = SHR_E_NONE;
    max669x_reg_t *maxreg = (max669x_reg_t *) data;

    if (opcode == MAX669X_OP_READ) {

        rv = bcmbd_i2c_read_byte_data(unit, saddr,
                                      MAX669X_REG_LOCAL_TEMP_R,
                                      &maxreg->local_temp);
        if (rv < 0) {
            return rv;
        }

        rv = bcmbd_i2c_read_byte_data(unit, saddr,
                                      MAX669X_REG_REMOTE_TEMP_R,
                                      &maxreg->remote_temp_1);
        if (rv < 0) {
            return rv;
        }

        rv = bcmbd_i2c_read_byte_data(unit, saddr,
                                      MAX669X_REG_CONFIG_BYTE_R,
                                      &maxreg->config);
        if (rv < 0) {
            return rv;
        }

        rv = bcmbd_i2c_write_byte_data(unit, saddr,
                                       MAX669X_REG_CONFIG_BYTE_W,
                                       (maxreg->config | (1<<3)));
        if (rv < 0) {
            return rv;
        }

        rv = bcmbd_i2c_read_byte_data(unit, saddr,
                                      MAX669X_REG_REMOTE_TEMP_R,
                                      &maxreg->remote_temp_2);
        if (rv < 0) {
            return rv;
        }

        rv = bcmbd_i2c_write_byte_data(unit, saddr,
                                       MAX669X_REG_CONFIG_BYTE_W,
                                       maxreg->config);
        if (rv < 0) {
            return rv;
        }

        rv = bcmbd_i2c_read_byte_data(unit, saddr,
                                      MAX669X_REG_STATUS_BYTE_R,
                                      &maxreg->status);
        if (rv < 0) {
            return rv;
        }

    } else if (opcode == MAX669X_OP_WRITE) {

        rv = bcmbd_i2c_write_byte_data(unit, saddr,
                                       MAX669X_REG_CONFIG_BYTE_W,
                                       maxreg->config);
        if (rv < 0) {
            return rv;
        }

    } else {
        LOG_CLI((BSL_META_U(unit,
                            "invalid command for max6699 - must be read or write\n")));
        rv = SHR_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function: max669x_temp_show
 *
 * Purpose:  Query MAX669X chip for temperature and report status in
 *           degrees Celcius and Degrees Fahrenheit.
 *
 * Parameters:
 * unit - switch unit
 * saddr - slave addr
 * thermal_record - data holder for history value
 * force - if true, show temp even if it hasn't changed
 */
static void
max669x_temp_show(int unit, int saddr, void *thermal_record, int force)
{
    max669x_reg_t maxreg;
    temperature_dev_info_t *temp_info;
    bcma_temperature_t ctemp, ftemp, lasttemp;
    bcma_temperature_t difftemp;
    int cnt;

    temp_info = (temperature_dev_info_t  *)thermal_record;
    if (temp_info == NULL) {
        return;
    }

    /* Read back stats */
    if (max669x_ioctl(unit, saddr,
                      MAX669X_OP_READ, &maxreg, sizeof(max669x_reg_t)) < 0) {
        cli_out("Maxim device not responding\n");
        return;
    }

    /* loop 3x for local and two remote temp */
    for (cnt = 0; cnt < 3; cnt++) {
        if (cnt == 0) {
            ctemp = maxreg.local_temp;
            lasttemp = temp_info->local_temp;
        } else if (cnt == 1) {
            ctemp = maxreg.remote_temp_1;
            lasttemp = temp_info->remote_temp_1;
        } else {
            ctemp = maxreg.remote_temp_2;
            lasttemp = temp_info->remote_temp_2;
        }

        if (lasttemp == 0) {
            /* first time through */
            difftemp = MINDIFF;
        } else {
            difftemp = lasttemp - ctemp;
            if (difftemp < 0) {
                difftemp = -difftemp;
            }
        }

        if (force || difftemp >= MINDIFF) {

            if (cnt == 0) {
                temp_info->local_temp = ctemp;
            } else if (cnt == 1) {
                temp_info->remote_temp_1 = ctemp;
            } else {
                temp_info->remote_temp_2 = ctemp;
            }

            /* 20 degree C change: something is wrong */
            if (difftemp > 20) {
                if (force) {
                    if (lasttemp > 0) {
                        ftemp = C2F(lasttemp);
                        LOG_CLI((BSL_META_U(unit,
                                            "%s: last temperature %d.%dC, %d.%dF\n"),
                                 "max",
                                 (lasttemp / 10), lasttemp%10,
                                 (ftemp / 10), ftemp%10));
                    }
                }

                return;
            }
        }
    }

    return;
}

/*
 * Function: max669x_init
 *
 * Purpose: Initialize the MAX669X temperature sensor chip.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    saddr - chip device address
 *    data - not used
 *    len - not used
 *
 * Returns:
 *     SHR_E_NONE - no failure
 *     SHR_E_TIMEOUT - chip or device access failure.
 */
static int
max669x_init(int unit, int saddr, void *data, int len)
{
    return SHR_E_NONE;
}

/* NOT USED */
static int
max669x_read(int unit, int saddr,
             uint16_t addr, uint8_t *data, uint32_t *len)
{
    return SHR_E_NONE;
}

/* NOT USED */
static int
max669x_write(int unit, int saddr,
              uint16_t addr, uint8_t *data, uint32_t len)
{
    return SHR_E_NONE;
}

static int
max669x_cleanup(int unit, int saddr, void *data, int len)
{
    return SHR_E_NONE;
}

/*!
 * \brief MAX669X Temperature Sensor Driver callout
 */
i2c_driver_t bcma_i2c_max669x_driver = {
    0x0,
    0x0,
    MAX669X_DEVICE_TYPE,
    max669x_read,
    max669x_write,
    max669x_ioctl,
    max669x_init,
    max669x_cleanup,
    max669x_temp_show,
};
