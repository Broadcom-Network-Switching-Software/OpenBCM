/*! \file bcmbd_i2c.h
 *
 * I2C Base Driver (BD) APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_I2C_H
#define BCMBD_I2C_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcmbd/bcmbd_i2c_internal.h>

/*! Maximum buffer count to hold operation data. */
#define BCMBD_OP_DATA_SIZE 256

/*! I2C write operation. */
#define I2C_WRITE 1

/*! I2C read operation. */
#define I2C_READ  0

/*!
 * \brief Initialize I2C driver.
 *
 * Allocates locks and other resources.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_MEMORY Insufficient resources.
 */
extern int
bcmbd_i2c_init(int unit);

/*!
 * \brief Clean up I2C driver.
 *
 * Free resources allocated by \ref bcmbd_i2c_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_i2c_cleanup(int unit);

/*!
 * \brief Read one byte from slave device.
 *
 * \brief format on bus: Addr+R  Data_byte
 *
 * \param [in]  unit   Unit number.
 * \param [in]  saddr  Slave device address.
 * \param [out] *data  Read back data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_i2c_read_byte(int unit,
                    i2c_addr_t saddr,
                    uint8_t *data);

/*!
 * \brief Write one byte data to slave device.
 *
 * \brief format on bus: Addr+W  Data_byte
 *
 * \param [in] unit  Unit number.
 * \param [in] saddr Slave device address.
 * \param [in] data  Data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_i2c_write_byte(int unit,
                     i2c_addr_t saddr,
                     uint8_t data);

/*!
 * \brief Read two bytes data from slave device.
 *
 * \brief format on bus: Addr+R  Data_byte_1 Data_byte_2
 *
 * \param [in] unit   Unit number.
 * \param [in] saddr  Slave device address.
 * \param [out] data  Read back data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_i2c_read_word(int unit,
                    i2c_addr_t saddr,
                    uint16_t *data);

/*!
 * \brief Write two bytes data to slave device.
 *
 * \brief format on bus: Addr+W  Data_byte_1 Data_byte_2
 *
 * \param [in] unit  Unit number.
 * \param [in] saddr Slave device address.
 * \param [in] data  Data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_i2c_write_word(int unit,
                     i2c_addr_t saddr,
                     uint16_t data);

/*!
 * \brief Read one byte data from a specific command of slave device.
 *
 * \brief format on bus: Addr+W command Addr+R Data_byte_1
 *
 * \param [in] unit    Unit number.
 * \param [in] saddr   Slave device address.
 * \param [in] command  Command type per slave device specification.
 * \param [out] data   Read out data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_i2c_read_byte_data(int unit,
                         i2c_addr_t saddr,
                         uint8_t command,
                         uint8_t *data);

/*!
 * \brief Write one byte data per command type of slave device.
 *
 * \brief format on bus: Addr+W command Data_byte_1
 *
 * \param [in] unit   Unit number.
 * \param [in] saddr  Slave device address.
 * \param [in] command Command type per slave device specification.
 * \param [in] data   Data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_i2c_write_byte_data(int unit,
                          i2c_addr_t saddr,
                          uint8_t command,
                          uint8_t data);

/*!
 * \brief Read two bytes data from a specific command of slave device.
 *
 * \brief format on bus: Addr+W command Addr_R Data_byte_1 Data_byte_2
 *
 * \param [in] unit    Unit number.
 * \param [in] saddr   Slave device address.
 * \param [in] command  Command type per slave device specification.
 * \param [out] data   Read out data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_i2c_read_word_data(int unit,
                         i2c_addr_t saddr,
                         uint8_t command,
                         uint16_t *data);

/*!
 * \brief Write two bytes data per command type of slave device.
 *
 * \brief format on bus: Addr+W command Data_byte_1 Data_byte_2
 *
 * \param [in] unit   Unit number.
 * \param [in] saddr  Slave device address.
 * \param [in] command Command type per slave device specification.
 * \param [in] data   Data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_i2c_write_word_data(int unit,
                          i2c_addr_t saddr,
                          uint8_t command,
                          uint16_t data);

/*!
 * \brief Read number of bytes from a specific command of slave device.
 *
 * \brief format on bus: Addr+W command Aaddr+R Byte_cnt  Data_byte_1 ... Data_byte_cnt
 *
 * \param [in] unit    Unit number.
 * \param [in] saddr   Slave device address.
 * \param [in] command  Command type per slave device specification.
 * \param [out] cnt    Buffer to hold readout byte count.
 * \param [out] data   Buffer to hold data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_i2c_block_read(int unit,
                     i2c_addr_t saddr,
                     uint8_t command,
                     uint8_t *cnt,
                     uint8_t *data);

/*!
 * \brief Read number of bytes from a specific command of slave device.
 *
 * \brief format on bus: Addr+W command Aaddr+R Byte_cnt  Data_byte_1 ... Data_byte_cnt
 *
 * \param [in] unit    Unit number.
 * \param [in] saddr   Slave device address.
 * \param [in] command  Command type per slave device specification.
 * \param [out] cnt    Buffer to hold readout byte count.
 * \param [out] data   Buffer to hold data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_i2c_block_write(int unit,
                      i2c_addr_t saddr,
                      uint8_t command,
                      uint8_t cnt,
                      uint8_t *data);

/*!
 * \brief Write number of bytes per command type of slave device.
 * \brief Data count information is not shown on bus.
 *
 * \brief format on bus: Addr+W command Data_byte_1 Data_byte_2 ... Data_byte_cnt
 *
 * \param [in] unit   Unit number.
 * \param [in] saddr  Slave device address.
 * \param [in] command Command type per slave device specification.
 * \param [in] cnt    Byte count.
 * \param [in] data   Buffer to hold data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */

extern int
bcmbd_i2c_multi_write(int unit,
                      i2c_addr_t saddr,
                      uint8_t command,
                      uint8_t cnt,
                      uint8_t *data);

#endif /* BCMBD_I2C_H */
