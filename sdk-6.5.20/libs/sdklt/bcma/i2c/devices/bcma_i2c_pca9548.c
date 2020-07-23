/*! \file bcma_i2c_pca9548.c
 *
 *  Application to control power regulator PCA9548.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
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
pca9548_read(int unit,
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
pca9548_write(int unit,
              int saddr,
              uint16_t addr,
              uint8_t *channel,
              uint32_t len)
{
    int rv;

    /* Channel number is indexed by bit. */
    rv = bcmbd_i2c_write_byte(unit, saddr, (1 << *channel));

    return rv;
}

/*!
 * \brief Device specific ioctl function.
 */
static int
pca9548_ioctl(int unit, int saddr, int opcode, void *data, int len)
{
    return SHR_E_NONE;
}

/*!
 * \brief Device specific init function.
 */
static int
pca9548_init(int unit, int saddr, void *data, int len)
{
    /* enable channel 0 by default */
    return bcmbd_i2c_write_byte(unit, saddr, 1);
}

/*!
 * \brief Device specific unload function.
 */
static int
pca9548_cleanup(int unit, int saddr, void *data, int len)
{
    return SHR_E_NONE;
}

/*!
 * \brief PCA9548 I2C mux chip Driver callout
 */
i2c_driver_t bcma_i2c_pca9548_driver = {
    0x0,
    0x0,
    PCA9548_DEVICE_TYPE,
    pca9548_read,
    pca9548_write,
    pca9548_ioctl,
    pca9548_init,
    pca9548_cleanup,
    NULL,
};

