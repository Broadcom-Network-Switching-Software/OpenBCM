/*! \file bcmbd_i2c_internal.h
 *
 * BD I2C internal API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_I2C_INTERNAL_H
#define BCMBD_I2C_INTERNAL_H

#include <sal/sal_spinlock.h>
#include <bcmbd/bcmbd_i2c_cmicx_internal.h>

/*!
 * \brief See \ref bcmbd_i2c_init.
 *
 * \param [in] unit Unit number.
 * \param [in] chan SMBus channel.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_MEMORY Insufficient resources.
 */
typedef int (*bcmbd_i2c_init_f)(int unit, uint8_t chan);

/*!
 * \brief See \ref bcmbd_i2c_cleanup.
 *
 * \param [in] unit Unit number.
 * \param [in] chan SMBus channel.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmbd_i2c_cleanup_f)(int unit, uint8_t chan);

/*!
 * \brief Write one byte data to slave device.
 *
 * \brief format on bus: Addr+W  Data_byte
 *
 * \param [in] unit  Unit number.
 * \param [in] chan  SMBus channel.
 * \param [in] saddr Slave device address.
 * \param [in] data  Data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_FAIL Hardware access returned an error.
 * \retval SHR_E_TIMEOUT Operation did not complete within normal time.
 */
typedef int (*bcmbd_i2c_write_byte_f)(int unit,
                                      uint8_t chan,
                                      i2c_addr_t saddr,
                                      uint8_t data);

/*!
 * \brief Read one byte from slave device.
 *
 * \brief format on bus: Addr+R  Data_byte
 *
 * \param [in]  unit   Unit number.
 * \param [in]  chan   SMBus channel.
 * \param [in]  saddr  Slave device address.
 * \param [out] *data  Read back data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
typedef int (*bcmbd_i2c_read_byte_f)(int unit,
                                     uint8_t chan,
                                     i2c_addr_t saddr,
                                     uint8_t *data);

/*!
 * \brief Write two bytes data to slave device.
 *
 * \brief format on bus: Addr+W  Data_byte_1 Data_byte_2
 *
 * \param [in] unit  Unit number.
 * \param [in] chan  SMBus channel.
 * \param [in] saddr Slave device address.
 * \param [in] data  Data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
typedef int (*bcmbd_i2c_write_word_f)(int unit,
                                      uint8_t chan,
                                      i2c_addr_t saddr,
                                      uint16_t data);


/*!
 * \brief Read two bytes data from slave device.
 *
 * \brief format on bus: Addr+R  Data_byte_1 Data_byte_2
 *
 * \param [in] unit   Unit number.
 * \param [in] chan   SMBus channel.
 * \param [in] saddr  Slave device address.
 * \param [out] data  Read back data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
typedef int (*bcmbd_i2c_read_word_f)(int unit,
                                     uint8_t chan,
                                     i2c_addr_t saddr,
                                     uint16_t *data);

/*!
 * \brief Write one byte data per specific command of slave device.
 *
 * \brief format on bus: Addr+W command Data_byte_1
 *
 * \param [in] unit    Unit number.
 * \param [in] chan    SMBus channel.
 * \param [in] saddr   Slave device address.
 * \param [in] command Command type per slave device specification.
 * \param [in] data    Data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
typedef int (*bcmbd_i2c_write_byte_data_f)(int unit,
                                           uint8_t chan,
                                           i2c_addr_t saddr,
                                           uint8_t command,
                                           uint8_t data);

/*!
 * \brief Read one byte data per specific command of slave device.
 *
 * \brief format on bus: Addr+W commmand Addr+R Data_byte_1
 *
 * \param [in] unit     Unit number.
 * \param [in] chan     SMBus channel.
 * \param [in] saddr    Slave device address.
 * \param [in] command  Command type per slave device specification.
 * \param [out] data    Read out data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
typedef int (*bcmbd_i2c_read_byte_data_f)(int unit,
                                          uint8_t chan,
                                          i2c_addr_t saddr,
                                          uint8_t command,
                                          uint8_t *data);

/*!
 * \brief Write two bytes data per specific command of slave device.
 *
 * \brief format on bus: Addr+W command Data_byte_1 Data_byte_2
 *
 * \param [in] unit    Unit number.
 * \param [in] chan    SMBus channel.
 * \param [in] saddr   Slave device address.
 * \param [in] command Command type per slave device specification.
 * \param [in] data   Data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */

typedef int (*bcmbd_i2c_write_word_data_f)(int unit,
                                           uint8_t chan,
                                           i2c_addr_t saddr,
                                           uint8_t command,
                                           uint16_t data);

/*!
 * \brief Read two bytes data per specific command of slave device.
 *
 * \brief format on bus: Addr+W command Addr_R Data_byte_1 Data_byte_2
 *
 * \param [in] unit     Unit number.
 * \param [in] chan     SMBus channel.
 * \param [in] saddr    Slave device address.
 * \param [in] command  Command type per slave device specification.
 * \param [out] data    Read out data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
typedef int (*bcmbd_i2c_read_word_data_f)(int unit,
                                          uint8_t chan,
                                          i2c_addr_t saddr,
                                          uint8_t command,
                                          uint16_t *data);

/*!
 * \brief Write number of bytes per specific command of slave device.
 *
 * \brief format on bus: Addr+W command Byte_cnt Data_byte_1 .. Data_byte_cnt
 *
 * \param [in] unit    Unit number.
 * \param [in] chan    SMBus channel.
 * \param [in] saddr   Slave device address.
 * \param [in] command Command type per slave device specification.
 * \param [in] cnt     Byte count.
 * \param [in] data    Buffer to hold data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
typedef int (*bcmbd_i2c_block_write_f)(int unit,
                                       uint8_t chan,
                                       i2c_addr_t saddr,
                                       uint8_t command,
                                       uint8_t cnt,
                                       uint8_t *data);

/*!
 * \brief Read number of bytes per specific command of slave device.
 *
 * \brief format on bus: Addr+W command Aaddr+R Byte_cnt  Data_byte_1 ... Data_byte_cnt
 *
 * \param [in] unit    Unit number.
 * \param [in] chan    SMBus channel.
 * \param [in] saddr   Slave device address.
 * \param [in] command Command type per slave device specification.
 * \param [out] cnt    Buffer to hold readout byte count.
 * \param [out] data   Buffer to hold data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
typedef int (*bcmbd_i2c_block_read_f)(int unit,
                                      uint8_t chan,
                                      i2c_addr_t saddr,
                                      uint8_t command,
                                      uint8_t *cnt,
                                      uint8_t *data);


/*!
 * \brief Write number of bytes per specific command of slave device.
 *
 * \brief format on bus: Addr+W command Data_byte_1 Data_byte_2 ... Data_byte_cnt
 *
 * \param [in] unit    Unit number.
 * \param [in] chan    SMBus channel.
 * \param [in] saddr   Slave device address.
 * \param [in] command Command type per slave device specification.
 * \param [in] cnt     Byte count.
 * \param [in] data    Buffer to hold data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
typedef int (*bcmbd_i2c_multi_write_f)(int unit,
                                       uint8_t chan,
                                       i2c_addr_t saddr,
                                       uint8_t command,
                                       uint8_t cnt,
                                       uint8_t *data);

/*!
 * \brief I2C exported operation functions.
 */
typedef struct bcmbd_i2c_ops_s {

    /*! Initialize I2C driver. */
    bcmbd_i2c_init_f init;

    /*! Clean up I2C driver. */
    bcmbd_i2c_cleanup_f cleanup;

    /*! Execute I2C write one byte. */
    bcmbd_i2c_write_byte_f write_byte;

    /*! Execute I2C read one byte. */
    bcmbd_i2c_read_byte_f read_byte;

    /*! Execute I2C write one word. */
    bcmbd_i2c_write_word_f write_word;

    /*! Execute I2C read one word. */
    bcmbd_i2c_read_word_f read_word;

    /*! Execute I2C write one byte with a command. */
    bcmbd_i2c_write_byte_data_f write_byte_data;

    /*! Execute I2C read one byte with a command. */
    bcmbd_i2c_read_byte_data_f read_byte_data;

    /*! Execute I2C write one word with a command. */
    bcmbd_i2c_write_word_data_f write_word_data;

    /*! Execute I2C read one word with a command. */
    bcmbd_i2c_read_word_data_f read_word_data;

    /*! Execute I2C block write. */
    bcmbd_i2c_block_write_f block_write;

    /*! Execute I2C block read. */
    bcmbd_i2c_block_read_f block_read;

    /*! Execute I2C multiple bytes write. */
    bcmbd_i2c_multi_write_f multi_write;
} bcmbd_i2c_ops_t;

/*!
 * \brief I2C device control.
 */
typedef struct bcmbd_i2c_ctrl_s {
    /*! Pointer to hold i2c operations. */
    bcmbd_i2c_ops_t *ops;

    /*! SMBus channel number. */
    uint8_t chan_num;

    /*! Active channel. */
    uint8_t chan;

    /*! Bus lock for active channel. */
    sal_spinlock_t lock;
} bcmbd_i2c_ctrl_t;

/*!
 * \brief Assign device-specific I2C driver.
 *
 * \param [in] unit Unit number.
 * \param [in] chan SMBus channel.
 * \param [in] ops I2C operation functions.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_i2c_drv_init(int unit, uint8_t chan, bcmbd_i2c_ops_t *ops);

/*!
 * \brief Cleanup device-specific I2C driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_i2c_drv_cleanup(int unit);

#endif /* BCMBD_I2C_INTERNAL_H */
