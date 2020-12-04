/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:	bcmi2c.c
 * Purpose:	BCM I2C API
 */

#ifdef INCLUDE_I2C

#include <sal/types.h>

#include <soc/debug.h>
#include <soc/i2c.h>

#include <bcm/bcmi2c.h>
#include <bcm/error.h>

#include <bcm_int/esw_dispatch.h>
/*
 * Function:
 *	bcm_esw_i2c_open
 * Purpose:
 *	Open device, return valid file descriptor or -1 on error.
 * Parameters:
 *	unit - StrataSwitch device number or I2C bus number
 *	devname - I2C device name string
 *	flags - arguments to pass to attach, default value should be zero
 *	speed - I2C bus speed, if non-zero, this speed is configured, normally
 *            this argument should be zero unless a speed is desired.
 * Returns:
 *      Device identifier for all I2C operations
 * Notes:
 *      This routine should be called before attempting to communicate
 *      with an I2C device which has a registered driver.
 *      A valid driver with this device name must be installed in the system.
 */

int
bcm_esw_i2c_open(int unit, char *devname, uint32 flags, int speed)
{
    return soc_i2c_devopen(unit, devname, flags, speed);
}

/*
 * Function:
 *	bcm_esw_i2c_write
 * Purpose:
 *	Write to a device
 * Parameters:
 *	unit - StrataSwitch device number or I2C bus number
 *	fd - I2C device ID
 *	addr - device register or memory address
 *	data - data byte buffer
 *	nbytes - number of bytes of data
 * Returns:
 *	Number of bytes written on success
 *	BCM_E_XXX on failure.
 * Notes:
 *      This routine requires a driver.
 */

int
bcm_esw_i2c_write(int unit, int fd, uint32 addr, uint8 *data, uint32 nbytes)
{
    if ((fd < 0) || (fd >= soc_i2c_device_count(unit))) {
	return BCM_E_PARAM;
    }

    if (soc_i2c_device(unit, fd)->driver == NULL) {
	return BCM_E_PARAM;
    }

    return soc_i2c_device(unit, fd)->driver->write(unit, fd,
						   addr, data, nbytes);
}

/*
 * Function:
 *	bcm_esw_i2c_read
 * Purpose:
 *	Read from a device
 * Parameters:
 *	unit - StrataSwitch device number or I2C bus number
 *	fd - I2C device ID
 *	addr - device register or memory address
 *	data - data byte buffer to read into
 *	nbytes - number of bytes of data, updated on success.
 * Returns:
 *	On success, number of bytes read; nbytes updated with number
 *	of bytes read from device; BCM_E_XXX on failure.
 * Notes:
 *      This routine requires a driver.
 */

int
bcm_esw_i2c_read(int unit, int fd, uint32 addr, uint8 *data, uint32 * nbytes)
{
    if ((fd < 0) || (fd >= soc_i2c_device_count(unit))) {
	return BCM_E_PARAM;
    }

    if (soc_i2c_device(unit, fd)->driver == NULL) {
	return BCM_E_PARAM;
    }

    return soc_i2c_device(unit, fd)->driver->read(unit, fd,
						  addr, data, nbytes);
}

/*
 * Function:
 *	bcm_esw_i2c_ioctl
 * Purpose:
 *	Device specific I/O control
 * Parameters:
 *	unit - StrataSwitch device number or I2C bus number
 *	fd - I2C device ID
 *	opcode - device command code (device-specific).
 *	data - data byte buffer for command
 *	nbytes - number of bytes of data
 * Returns:
 *	On success, application specific value greater than zero,
 *	BCM_E_XXX otherwise.
 * Notes:
 *      This routine requires a driver.
 */

int
bcm_esw_i2c_ioctl(int unit, int fd, int opcode, void *data, int len)
{
    if ((fd < 0) || (fd >= soc_i2c_device_count(unit))) {
	return BCM_E_PARAM;
    }

    if (soc_i2c_device(unit, fd)->driver == NULL) {
	return BCM_E_PARAM;
    }

    return soc_i2c_device(unit, fd)->driver->ioctl(unit, fd, opcode,
						   data, len);
}

#endif /* INCLUDE_I2C */
