/*! \file bcma_i2c_device.c
 *
 *  Utility function to trace I2C device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <bcma/i2c/bcma_i2ccmd_i2c.h>
#include <bcma/i2c/bcma_i2c_device.h>
#include <bcmbd/bcmbd_i2c.h>
#include <bcma/io/bcma_io_file.h>
#include <bcmbd/bcmbd.h>

#define BSL_LOG_MODULE  BSL_LS_APPL_I2C

/*!
 * \brief Derive I2C device address.
 *
 * \param [in] unit  Unit number
 * \param [in] devid Device ID
 * \param [in] dev_info Device array
 *
 * \return I2C slave device address.
 */
i2c_addr_t
bcma_i2c_addr(int unit, int devid, i2c_device_t *dev_info)
{
    return (dev_info[devid].saddr);
}

/*!
 * \brief Derive I2C device name.
 *
 * \param [in] unit  Unit number
 * \param [in] devid Device ID
 * \param [in] dev_info Device array
 *
 * \return I2C slave device name.
 */
const char *
bcma_i2c_devname(int unit, int devid, i2c_device_t *dev_info)
{
    return (dev_info[devid].devname);
}
