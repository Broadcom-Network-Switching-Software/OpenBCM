/*! \file bcmbd_i2c.c
 *
 * BD TimeSync driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_chip.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd_i2c.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_I2C

/*******************************************************************************
 * Local definitions
 */
static bcmbd_i2c_ctrl_t bd_i2c_ctrl[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

int
bcmbd_i2c_drv_init(int unit, uint8_t chan, bcmbd_i2c_ops_t *ops)
{
    sal_spinlock_t lock;

    if (!ops || chan >= NUM_I2C_CHAN) {
        return SHR_E_PARAM;
    }

    bd_i2c_ctrl[unit].ops = ops;
    bd_i2c_ctrl[unit].chan_num = NUM_I2C_CHAN;
    bd_i2c_ctrl[unit].chan = chan;

    lock = sal_spinlock_create("bcmbdI2cLock");
    if (!lock) {
        return SHR_E_MEMORY;
    }
    bd_i2c_ctrl[unit].lock = lock;

    return SHR_E_NONE;
}

int
bcmbd_i2c_drv_cleanup(int unit)
{
    bcmbd_i2c_ctrl_t *ctrl = &bd_i2c_ctrl[unit];

    ctrl->ops = NULL;
    ctrl->chan_num = 0;
    if (ctrl->lock) {
        sal_spinlock_destroy(ctrl->lock);
        ctrl->lock = NULL;
    }

    return SHR_E_NONE;
}

int
bcmbd_i2c_init(int unit)
{
    bcmbd_i2c_ctrl_t *ctrl = &bd_i2c_ctrl[unit];
    int rv = SHR_E_NONE;

    if (!ctrl || !ctrl->ops || !ctrl->ops->init) {
        return SHR_E_UNAVAIL;
    }

    if (!ctrl->lock || ctrl->chan >= ctrl->chan_num) {
        return SHR_E_INTERNAL;
    }

    sal_spinlock_lock(ctrl->lock);
    rv = ctrl->ops->init(unit, ctrl->chan);
    sal_spinlock_unlock(ctrl->lock);

    return rv;
}

int
bcmbd_i2c_cleanup(int unit)
{
    bcmbd_i2c_ctrl_t *ctrl = &bd_i2c_ctrl[unit];
    int rv = SHR_E_NONE;

    if (!ctrl || !ctrl->ops || !ctrl->ops->cleanup) {
        return SHR_E_UNAVAIL;
    }

    if (!ctrl->lock || ctrl->chan >= ctrl->chan_num) {
        return SHR_E_INTERNAL;
    }

    sal_spinlock_lock(ctrl->lock);
    rv = ctrl->ops->cleanup(unit, ctrl->chan);
    sal_spinlock_unlock(ctrl->lock);

    return rv;
}

/*!
 * \brief Read one byte from slave device.
 *
 * \Data format on bus: Addr+R  Data_byte
 *
 * \param [in]  unit  Unit number.
 * \param [in]  saddr Slave device address.
 * \param [out] data  Read back data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
int bcmbd_i2c_read_byte(int unit, i2c_addr_t saddr, uint8_t *data)
{
    bcmbd_i2c_ctrl_t *ctrl = &bd_i2c_ctrl[unit];
    int rv = SHR_E_NONE;

    if (!ctrl || !ctrl->ops || !ctrl->ops->read_byte) {
        return SHR_E_UNAVAIL;
    }

    if (!ctrl->lock || ctrl->chan >= ctrl->chan_num) {
        return SHR_E_INTERNAL;
    }

    sal_spinlock_lock(ctrl->lock);
    rv = ctrl->ops->read_byte(unit, ctrl->chan, saddr, data);
    sal_spinlock_unlock(ctrl->lock);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "I2C read byte, addr: 0x%x, byte0: 0x%x\n"),
                 saddr, (int)*data));
    return rv;
}

/*!
 * \brief Write one byte data to slave device.
 *
 * \Data format on bus: Addr+W  Data_byte
 *
 * \param [in] unit  Unit number.
 * \param [in] saddr Slave device address.
 * \param [in] data  Data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
int bcmbd_i2c_write_byte(int unit, i2c_addr_t saddr, uint8_t data)
{
    bcmbd_i2c_ctrl_t *ctrl = &bd_i2c_ctrl[unit];
    int rv = SHR_E_NONE;

    if (!ctrl || !ctrl->ops || !ctrl->ops->write_byte) {
        return SHR_E_UNAVAIL;
    }

    if (!ctrl->lock || ctrl->chan >= ctrl->chan_num) {
        return SHR_E_INTERNAL;
    }

    sal_spinlock_lock(ctrl->lock);
    rv = ctrl->ops->write_byte(unit, ctrl->chan, saddr, data);
    sal_spinlock_unlock(ctrl->lock);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "I2C write byte, addr: 0x%x, byte0: 0x%x\n"),
                 saddr, data));
    return rv;
}

/*!
 * \brief Read two bytes data from slave device.
 *
 * \Data format on bus: Addr+R  Data_byte_1 Data_byte_2
 *
 * \param [in] unit   Unit number.
 * \param [in] saddr  Slave device address.
 * \param [out] data  Read back data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
int bcmbd_i2c_read_word(int unit, i2c_addr_t saddr, uint16_t *data)
{
    bcmbd_i2c_ctrl_t *ctrl = &bd_i2c_ctrl[unit];
    int rv = SHR_E_NONE;

    if (!ctrl || !ctrl->ops || !ctrl->ops->read_word) {
        return SHR_E_UNAVAIL;
    }

    if (!ctrl->lock || ctrl->chan >= ctrl->chan_num) {
        return SHR_E_INTERNAL;
    }

    sal_spinlock_lock(ctrl->lock);
    rv = ctrl->ops->read_word(unit, ctrl->chan, saddr, data);
    sal_spinlock_unlock(ctrl->lock);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "I2C read word, addr: 0x%x, byte0: 0x%x\n"),
                saddr, *data));
    return rv;
}

/*!
 * \brief Write two bytes data to slave device.
 *
 * \Data format on bus: Addr+W  Data_byte_1 Data_byte_2
 *
 * \param [in] unit  Unit number.
 * \param [in] saddr Slave device address.
 * \param [in] data  Data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
int bcmbd_i2c_write_word(int unit, i2c_addr_t saddr, uint16_t data)
{
    bcmbd_i2c_ctrl_t *ctrl = &bd_i2c_ctrl[unit];
    int rv = SHR_E_NONE;

    if (!ctrl || !ctrl->ops || !ctrl->ops->write_word) {
        return SHR_E_UNAVAIL;
    }

    if (!ctrl->lock || ctrl->chan >= ctrl->chan_num) {
        return SHR_E_INTERNAL;
    }

    sal_spinlock_lock(ctrl->lock);
    rv = ctrl->ops->write_word(unit, ctrl->chan, saddr, data);
    sal_spinlock_unlock(ctrl->lock);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "I2C write word, addr: 0x%x, byte0: 0x%x\n"),
                 saddr, data));
    return rv;
}

/*!
 * \brief Read one byte data from a specific command of slave device.
 *
 * \Data format on bus: Addr+W command Addr+R Data_byte_1
 *
 * \param [in] unit    Unit number.
 * \param [in] saddr   Slave device address.
 * \param [in] command Command type per slave device specification.
 * \param [out] data   Read out data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
int bcmbd_i2c_read_byte_data(int unit,
                             i2c_addr_t saddr,
                             uint8_t command,
                             uint8_t *data)
{
    bcmbd_i2c_ctrl_t *ctrl = &bd_i2c_ctrl[unit];
    int rv = SHR_E_NONE;

    if (!ctrl || !ctrl->ops || !ctrl->ops->read_byte_data) {
        return SHR_E_UNAVAIL;
    }

    if (!ctrl->lock || ctrl->chan >= ctrl->chan_num) {
        return SHR_E_INTERNAL;
    }

    sal_spinlock_lock(ctrl->lock);
    rv = ctrl->ops->read_byte_data(unit, ctrl->chan, saddr, command, data);
    sal_spinlock_unlock(ctrl->lock);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "I2C read byte data, addr: 0x%x, byte0: 0x%x\n"),
                saddr, (int)*data));
    return rv;
}

/*!
 * \brief Write one byte data per command type of slave device.
 *
 * \Data format on bus: Addr+W command Data_byte_1
 *
 * \param [in] unit    Unit number.
 * \param [in] saddr   Slave device address.
 * \param [in] command Command type per slave device specification.
 * \param [in] data    Data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
int bcmbd_i2c_write_byte_data(int unit, i2c_addr_t saddr,
                uint8_t command, uint8_t data)
{
    bcmbd_i2c_ctrl_t *ctrl = &bd_i2c_ctrl[unit];
    int rv = SHR_E_NONE;

    if (!ctrl || !ctrl->ops || !ctrl->ops->write_byte_data) {
        return SHR_E_UNAVAIL;
    }

    if (!ctrl->lock || ctrl->chan >= ctrl->chan_num) {
        return SHR_E_INTERNAL;
    }

    sal_spinlock_lock(ctrl->lock);
    rv = ctrl->ops->write_byte_data(unit, ctrl->chan, saddr, command, data);
    sal_spinlock_unlock(ctrl->lock);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "I2C write byte data, addr: 0x%x, byte0: 0x%x\n"),
                 saddr, data));
    return rv;
}

/*!
 * \brief Read two bytes data from a specific command of slave device.
 *
 * \Data format on bus: Addr+W command Addr_R Data_byte_1 Data_byte_2
 *
 * \param [in] unit    Unit number.
 * \param [in] saddr   Slave device address.
 * \param [in] command Command type per slave device specification.
 * \param [out] data   Read out data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
int bcmbd_i2c_read_word_data(int unit,
                             i2c_addr_t saddr,
                             uint8_t command,
                             uint16_t *data)
{
    bcmbd_i2c_ctrl_t *ctrl = &bd_i2c_ctrl[unit];
    int rv = SHR_E_NONE;

    if (!ctrl || !ctrl->ops || !ctrl->ops->read_word_data) {
        return SHR_E_UNAVAIL;
    }

    if (!ctrl->lock || ctrl->chan >= ctrl->chan_num) {
        return SHR_E_INTERNAL;
    }

    sal_spinlock_lock(ctrl->lock);
    rv = ctrl->ops->read_word_data(unit, ctrl->chan, saddr, command, data);
    sal_spinlock_unlock(ctrl->lock);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "I2C read word data, addr: 0x%x, byte0: 0x%x\n"),
                 saddr, *data));
    return rv;
}

/*!
 * \brief Write two bytes data per command type of slave device.
 *
 * \Data format on bus: Addr+W command Data_byte_1 Data_byte_2
 *
 * \param [in] unit    Unit number.
 * \param [in] saddr   Slave device address.
 * \param [in] command Command type per slave device specification.
 * \param [in] data    Data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
int bcmbd_i2c_write_word_data(int unit,
                              i2c_addr_t saddr,
                              uint8_t command,
                              uint16_t data)
{
    bcmbd_i2c_ctrl_t *ctrl = &bd_i2c_ctrl[unit];
    int rv = SHR_E_NONE;

    if (!ctrl || !ctrl->ops || !ctrl->ops->write_word_data) {
        return SHR_E_UNAVAIL;
    }

    if (!ctrl->lock || ctrl->chan >= ctrl->chan_num) {
        return SHR_E_INTERNAL;
    }

    sal_spinlock_lock(ctrl->lock);
    rv = ctrl->ops->write_word_data(unit, ctrl->chan, saddr, command, data);
    sal_spinlock_unlock(ctrl->lock);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "I2C write word data, addr: 0x%x, byte0: 0x%x\n"),
                saddr, data));
    return rv;
}


/*!
 * \brief Read number of bytes from a specific command of slave device.
 *
 * \Data format on bus: Addr+W command Aaddr+R Byte_cnt  Data_byte_1 ... Data_byte_cnt
 *
 * \param [in] unit    Unit number.
 * \param [in] saddr   Slave device address.
 * \param [in] command Command type per slave device specification.
 * \param [out] cnt    Buffer to hold readout byte count.
 * \param [out] data   Buffer to hold data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
int bcmbd_i2c_block_read(int unit,
                         i2c_addr_t saddr,
                         uint8_t command,
                         uint8_t *cnt,
                         uint8_t *data)
{
    bcmbd_i2c_ctrl_t *ctrl = &bd_i2c_ctrl[unit];
    int rv = SHR_E_NONE;

    if (!ctrl || !ctrl->ops || !ctrl->ops->block_read) {
        return SHR_E_UNAVAIL;
    }

    if (!ctrl->lock || ctrl->chan >= ctrl->chan_num) {
        return SHR_E_INTERNAL;
    }

    sal_spinlock_lock(ctrl->lock);
    rv = ctrl->ops->block_read(unit, ctrl->chan, saddr, command, cnt, data);
    sal_spinlock_unlock(ctrl->lock);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "I2C block read, addr: 0x%x, byte0: 0x%x\n"),
                saddr, *data));
    return rv;
}

/*!
 * \brief Read number of bytes from a specific command of slave device.
 *
 * \Data format on bus: Addr+W command Aaddr+R Byte_cnt  Data_byte_1 ... Data_byte_cnt
 *
 * \param [in] unit    Unit number.
 * \param [in] saddr   Slave device address.
 * \param [in] command Command type per slave device specification.
 * \param [out] cnt    Buffer to hold readout byte count.
 * \param [out] data   Buffer to hold data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
int bcmbd_i2c_block_write(int unit,
                          i2c_addr_t saddr,
                          uint8_t command,
                          uint8_t cnt,
                          uint8_t *data)
{
    bcmbd_i2c_ctrl_t *ctrl = &bd_i2c_ctrl[unit];
    int rv = SHR_E_NONE;

    if (!ctrl || !ctrl->ops || !ctrl->ops->block_write) {
        return SHR_E_UNAVAIL;
    }

    if (!ctrl->lock || ctrl->chan >= ctrl->chan_num) {
        return SHR_E_INTERNAL;
    }

    sal_spinlock_lock(ctrl->lock);
    rv = ctrl->ops->block_write(unit, ctrl->chan, saddr, command, cnt, data);
    sal_spinlock_unlock(ctrl->lock);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "I2C block write, addr: 0x%x, byte0: 0x%x\n"),
                saddr, *data));
    return rv;
}

/*!
 * \brief Write number of bytes per command type of slave device.
 * \brief Data count information is not shown on bus.
 *
 * \Data format on bus: Addr+W command Data_byte_1 Data_byte_2 ... Data_byte_cnt
 *
 * \param [in] unit    Unit number.
 * \param [in] saddr   Slave device address.
 * \param [in] command Command type per slave device specification.
 * \param [in] cnt     Byte count.
 * \param [in] data    Buffer to hold data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */

int bcmbd_i2c_multi_write(int unit,
                          i2c_addr_t saddr,
                          uint8_t command,
                          uint8_t cnt,
                          uint8_t *data)
{
    bcmbd_i2c_ctrl_t *ctrl = &bd_i2c_ctrl[unit];
    int rv = SHR_E_NONE;

    if (!ctrl || !ctrl->ops || !ctrl->ops->multi_write) {
        return SHR_E_UNAVAIL;
    }

    if (!ctrl->lock || ctrl->chan >= ctrl->chan_num) {
        return SHR_E_INTERNAL;
    }

    sal_spinlock_lock(ctrl->lock);
    rv = ctrl->ops->multi_write(unit, ctrl->chan, saddr, command, cnt, data);
    sal_spinlock_unlock(ctrl->lock);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "I2C multi write, addr: 0x%x, byte0: 0x%x\n"),
                saddr, *data));
    return rv;
}
