/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:	i2c.h
 * Purpose: Implementation of I2C bus commands
 */

#ifndef	_SAL_I2C_H
#define	_SAL_I2C_H

#include <sal/types.h>

/* 
 * i2c_bus  - The number of the I2C bus to access on
 * i2c_dev  - the device on the I2C bus (slave device ID/address) to be accessed
 * addr     - The address in the device to be accessed (register address). Used if alen>0
 * addr_len - the length of the address in bytes, should be between 0 and 4
 * data     - the data buffer for the I2C operation
 * data_len - the length of the data
 *
 * Returns      : 0 - success, otherwise - failure
 */
extern int
sal_i2c_read(int i2c_bus, uint16 i2c_dev, uint32 addr, uint8 addr_len, uint8 *data, 
             uint8 data_len);

/* 
 * i2c_bus  - The number of the I2C bus to access on
 * i2c_dev  - the device on the I2C bus (slave device ID/address) to be accessed
 * addr     - The address in the device to be accessed (register address). Used if addr_len>0
 * addr_len - the length of the address in bytes, should be between 0 and 4
 * data     - the data buffer for the I2C operation
 * data_len - the length of the data
 * 
 * Returns      : 0 - success, otherwise - failure
 */
extern int
sal_i2c_write(int i2c_bus, uint16 i2c_dev, uint32 addr, uint8 addr_len, uint8 *data, 
              uint8 data_len);

/* 
 * Function     : sal_i2c_write_split
 * Purpose      : Provide i2c bus write functionality using CPU i2c controller    
 *                Splits the I2C write operation in the implementation to address and data.
 * Returns      : 0 - success, otherwise - failure
 */
extern int
sal_i2c_write_split(
  int i2c_bus,    /* The number of the I2C bus to access on */
  uint16 i2c_dev, /* the device on the I2C bus (slave device ID/address) to be accessed */
  uint32 addr,    /* The address in the device to be accessed (register address). Used if addr_len>0 */
  uint8 addr_len, /* the length of the address in bytes, should be between 0 and 4. */
  uint8 *data,    /* the buffer of the data to be written. */
  uint8 data_len);/* the length of the data to write. */

/*
 * Function: cpu_i2c_device_present
 * Purpose: Probe the I2C bus using i2c_dev, report if a device responds.
 *          The I2C bus is released upon return.
 *          No further action is taken.
 *
 * Parameters:
 *    i2c_dev - I2C slave address
 *
 * Return:
 *     _SHR_E_NONE - An I2C device responds at the indicated slave address i2c_dev.
 *     otherwise  - No I2C response at the indicated saddr, or I/O error.
 */
extern int
sal_i2c_device_present(
  int i2c_bus,    /* The number of the I2C bus to access on */
  uint16 i2c_dev);/* the device on the I2C bus (slave device ID/address) to be accessed */

#define SAL_I2C_FAST_ACCESS     (0x1)
/* 
 * unit     - I2C controller 
 * flags    - SAL_I2C_* flags 
 */
extern int
sal_i2c_config_set(int unit, uint32 flags);

/* 
 * unit     - I2C controller 
 * flags    - SAL_I2C_* flags 
 */
extern int
sal_i2c_config_get(int unit, uint32 *flags);
#endif /* _SAL_I2C_H */
