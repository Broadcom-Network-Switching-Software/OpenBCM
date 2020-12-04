/*! \file bcma_i2c_pcf8574.c
 *
 *  Application to control board ID chip pcf8574.
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

/*!
 * \brief Device specific read function.
 */
static int
pcf8574_read(int unit,
             int saddr,
             uint16_t addr,
             uint8_t *data,
             uint32_t *len)
{

    int rv;

    rv = bcmbd_i2c_read_byte(unit, saddr, data);
    *len = 1;

    return rv;
}

/*!
 * \brief Device specific write function.
 */
static int
pcf8574_write(int unit,
              int saddr,
              uint16_t addr,
              uint8_t *data,
              uint32_t len)
{
    return (bcmbd_i2c_write_byte(unit, saddr, *data));
}

/*!
 * \brief Device specific ioctl function.
 */
static int
pcf8574_ioctl(int unit,
              int saddr,
              int opcode,
              void *data,
              int len)
{
    return SHR_E_NONE;
}

/*!
 * \brief Device specific init function.
 */
static int
pcf8574_init(int unit,
             int saddr,
             void *data,
             int len)
{
    uint32_t bytes;
    uint8_t lpt_val = 0;

    /* This device used as input; write 0xFF to disable quasi-outputs */
    lpt_val = 0xFF;
    pcf8574_write(unit, saddr, 0, &lpt_val, 1);

    /* Now read what is being driven onto the quasi-inputs */
    pcf8574_read(unit, saddr, 0, &lpt_val, &bytes);

    /* Try to read from the PCA9554 device */
    if (lpt_val == 0xFF || lpt_val == 0x0) {
        /* Write 0x00 to command register first */
        lpt_val = 0;
        pcf8574_write(unit, saddr, 0, &lpt_val, 1);
        pcf8574_read(unit, saddr, 0, &lpt_val, &bytes);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s: baseboard id 0x%x\n"),
                 "PCF8574", lpt_val));

    /* Return board id */
    *(char *)data = lpt_val;

    return SHR_E_NONE;
}

/*!
 * \brief Device specific unload function.
 */
static int
pcf8574_cleanup(int unit,
                int saddr,
                void *data,
                int len)
{
    return SHR_E_NONE;
}

/*!
 * \brief PCF8574 Board ID Chip Driver callout
 */
i2c_driver_t bcma_i2c_pcf8574_driver = {
    0x0,
    0x0,
    PCF8574_DEVICE_TYPE,
    pcf8574_read,
    pcf8574_write,
    pcf8574_ioctl,
    pcf8574_init,
    pcf8574_cleanup,
    NULL,
};

