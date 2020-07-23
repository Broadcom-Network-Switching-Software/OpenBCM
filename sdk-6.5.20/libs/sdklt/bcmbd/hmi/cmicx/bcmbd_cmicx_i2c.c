/*! \file bcmbd_cmicx_i2c.c
 *
 * CMICx MIIM operations
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_timeout.h>
#include <sal/sal_sleep.h>
#include <sal/sal_mutex.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd_cmicx_acc.h>
#include <bcmbd/bcmbd_i2c_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_I2C

/*******************************************************************************
 * Local definitions
 */
#define I2C_READ_FIFO_SIZE 64

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Recover SMBus timeout with Bit Bang method.
 *
 * \param [in] unit Unit number.
 * \param [in] chan SMBus channel.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
static int
cmicx_i2c_timeout_recovery(int unit, uint8_t chan)
{
    SMBUS_BIT_BANG_CONTROLr_t bit_bang_ctrl;
    SMBUS_CONFIGr_t config;
    SMBUS_MASTER_COMMANDr_t command;
    SMBUS_MASTER_FIFO_CONTROLr_t fifo_control;
    int clk;
    int i, rv = 0;

    SMBUS_BIT_BANG_CONTROLr_CLR(bit_bang_ctrl);
    SMBUS_CONFIGr_CLR(config);
    SMBUS_MASTER_COMMANDr_CLR(command);
    SMBUS_MASTER_FIFO_CONTROLr_CLR(fifo_control);

    READ_SMBUS_BIT_BANG_CONTROLr(unit, chan, &bit_bang_ctrl);
    if (SMBUS_BIT_BANG_CONTROLr_SMBDAT_INf_GET(bit_bang_ctrl) == 0) {
        /* Enable Bit-Bang mode. */
        READ_SMBUS_CONFIGr(unit, chan, &config);
        SMBUS_CONFIGr_BIT_BANG_ENf_SET(config, 1);
        WRITE_SMBUS_CONFIGr(unit, chan, config);

        sal_usleep(60);

        READ_SMBUS_BIT_BANG_CONTROLr(unit, chan, &bit_bang_ctrl);
        SMBUS_BIT_BANG_CONTROLr_SMBCLK_OUT_ENf_SET(bit_bang_ctrl, 1);
        SMBUS_BIT_BANG_CONTROLr_SMBDAT_OUT_ENf_SET(bit_bang_ctrl, 1);
        WRITE_SMBUS_BIT_BANG_CONTROLr(unit, chan, bit_bang_ctrl);

        clk = 1;

        for (i = 0; i < 18; i++) {
            if (clk == 0) {
                SMBUS_BIT_BANG_CONTROLr_SMBCLK_OUT_ENf_SET(bit_bang_ctrl, 1);
                clk = 1;
            } else {
                SMBUS_BIT_BANG_CONTROLr_SMBCLK_OUT_ENf_SET(bit_bang_ctrl, 0);
                clk = 0;
            }

            WRITE_SMBUS_BIT_BANG_CONTROLr(unit, chan, bit_bang_ctrl);
            sal_usleep(5);
        }

        /*
         * SMBus data line stuck to low means time out happend, master
         * driver then generate stop signal to reset the bus.
         *
         * This reset logic will be looped 5 times till SHR_E_NONE in
         * caller function to kick off a clean start.
         */
        READ_SMBUS_BIT_BANG_CONTROLr(unit, chan, &bit_bang_ctrl);
        if (SMBUS_BIT_BANG_CONTROLr_SMBDAT_INf_GET(bit_bang_ctrl) == 0) {
            rv = SHR_E_TIMEOUT;

            /* Disable bit-bang mode. */
            READ_SMBUS_CONFIGr(unit, chan, &config);
            SMBUS_CONFIGr_BIT_BANG_ENf_SET(config, 0);
            WRITE_SMBUS_CONFIGr(unit, chan, config);
            sal_usleep(60);
        } else {
            /* Generate a stop signal to reset the bus onlf if DATA line high */
            READ_SMBUS_BIT_BANG_CONTROLr(unit, chan, &bit_bang_ctrl);
            SMBUS_BIT_BANG_CONTROLr_SMBCLK_OUT_ENf_SET(bit_bang_ctrl, 0);
            WRITE_SMBUS_BIT_BANG_CONTROLr(unit, chan, bit_bang_ctrl);
            sal_usleep(2);

            SMBUS_BIT_BANG_CONTROLr_SMBDAT_OUT_ENf_SET(bit_bang_ctrl, 0);
            WRITE_SMBUS_BIT_BANG_CONTROLr(unit, chan, bit_bang_ctrl);
            sal_usleep(2);


            SMBUS_BIT_BANG_CONTROLr_SMBCLK_OUT_ENf_SET(bit_bang_ctrl, 1);
            WRITE_SMBUS_BIT_BANG_CONTROLr(unit, chan, bit_bang_ctrl);
            sal_usleep(2);

            SMBUS_BIT_BANG_CONTROLr_SMBDAT_OUT_ENf_SET(bit_bang_ctrl, 1);
            WRITE_SMBUS_BIT_BANG_CONTROLr(unit, chan, bit_bang_ctrl);
            sal_usleep(2);

            READ_SMBUS_CONFIGr(unit, chan, &config);
            SMBUS_CONFIGr_BIT_BANG_ENf_SET(config, 0);
            WRITE_SMBUS_CONFIGr(unit, chan, config);

            sal_usleep(60);
        }
    }

    READ_SMBUS_MASTER_COMMANDr(unit, chan, &command);
    if (SMBUS_MASTER_COMMANDr_MASTER_START_BUSY_COMMANDf_GET(command) != 0 ||
        SMBUS_MASTER_COMMANDr_MASTER_STATUSf_GET(command) != 0) {

        /* Config to start RESET */
        READ_SMBUS_CONFIGr(unit, chan, &config);
        SMBUS_CONFIGr_RESETf_SET(config, 1);
        WRITE_SMBUS_CONFIGr(unit, chan, config);
        sal_usleep(60);

        READ_SMBUS_MASTER_FIFO_CONTROLr(unit, chan, &fifo_control);
        SMBUS_MASTER_FIFO_CONTROLr_MASTER_TX_FIFO_FLUSHf_SET(fifo_control, 1);
        SMBUS_MASTER_FIFO_CONTROLr_MASTER_RX_FIFO_FLUSHf_SET(fifo_control, 1);
        WRITE_SMBUS_MASTER_FIFO_CONTROLr(unit, chan, fifo_control);

        READ_SMBUS_CONFIGr(unit, chan, &config);
        SMBUS_CONFIGr_SMB_ENf_SET(config, 1);
        WRITE_SMBUS_CONFIGr(unit, chan, config);

        sal_usleep(60);
    }

    return rv;
}

/*!
 * \brief Start SMBus transaction and wait for it to finish.
 *
 * \param [in] unit Unit number.
 * \param [in] chan SMBus channel.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
static int
cmicx_i2c_start_wait(int unit, uint8_t chan)
{
    int rv = SHR_E_TIMEOUT;
    SMBUS_MASTER_COMMANDr_t command;
    uint32_t status;
    int time_out=100;

    SMBUS_MASTER_COMMANDr_CLR(command);

    /* Start Transaction */
    READ_SMBUS_MASTER_COMMANDr(unit, chan, &command);
    SMBUS_MASTER_COMMANDr_MASTER_START_BUSY_COMMANDf_SET(command, 1);
    WRITE_SMBUS_MASTER_COMMANDr(unit, chan, command);

    do {
        READ_SMBUS_MASTER_COMMANDr(unit, chan, &command);
        if (SMBUS_MASTER_COMMANDr_MASTER_START_BUSY_COMMANDf_GET(command) == 0) {
            rv = SHR_E_NONE;
            break;
        }
        time_out--;
        sal_usleep(60);
    } while (time_out > 0);

    READ_SMBUS_MASTER_COMMANDr(unit, chan, &command);
    status = SMBUS_MASTER_COMMANDr_MASTER_STATUSf_GET(command);

    if ((rv == 0) && (status == 0)) {
        rv = SHR_E_NONE;
    } else {
        cmicx_i2c_timeout_recovery(unit, chan);
        rv = SHR_E_TIMEOUT;
    }
    return rv;
}

/*!
 * \brief Write one byte data to slave device.
 *
 * \Data format on bus: Addr+W  Data_byte
 *
 * \param [in] unit  Unit number.
 * \param [in] chan  SMBus channel.
 * \param [in] saddr Slave device address.
 * \param [in] data  Data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
static
int cmicx_i2c_write_byte(int unit, uint8_t chan, i2c_addr_t saddr, uint8_t data)
{
    int rv = SHR_E_NONE;
    SMBUS_MASTER_DATA_WRITEr_t data_write;
    SMBUS_MASTER_COMMANDr_t master_command;
    uint32_t rval;
    int rt = 5;

    SMBUS_MASTER_DATA_WRITEr_CLR(data_write);
    SMBUS_MASTER_COMMANDr_CLR(master_command);

    while (rt > 0) {
        /* Slave address first. */
        rval = I2C_TX_ADDR(saddr);
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Data byte and mark it last byte */
        rval = data;
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        SMBUS_MASTER_DATA_WRITEr_MASTER_WR_STATUSf_SET(data_write, 1);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        rval = 0;
        SMBUS_MASTER_COMMANDr_SMBUS_PROTOCOLf_SET(master_command, CMICX_I2C_PROTOCOL_SEND_BYTE);
        WRITE_SMBUS_MASTER_COMMANDr(unit, chan, master_command);

        rv = cmicx_i2c_start_wait(unit, chan);
        if (rv == SHR_E_NONE) {
            break;
        } else {
           rt--;
        }
    }

    if (rt <= 0) {
        rv = SHR_E_TIMEOUT;
    }


    return rv;
}

/*!
 * \brief Read one byte from slave device.
 *
 * \Data format on bus: Addr+R  Data_byte
 *
 * \param [in]  unit   Unit number.
 * \param [in]  chan   SMBus channel.
 * \param [in]  saddr  Slave device address.
 * \param [out] *data  Read back data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
static int cmicx_i2c_read_byte(int unit, uint8_t chan, i2c_addr_t saddr,
                               uint8_t *data)
{
    int rv = SHR_E_NONE;
    SMBUS_MASTER_DATA_WRITEr_t data_write;
    SMBUS_MASTER_COMMANDr_t master_command;
    SMBUS_MASTER_DATA_READr_t  data_read;
    uint32_t rval;
    int rt = 5;

    SMBUS_MASTER_DATA_WRITEr_CLR(data_write);
    SMBUS_MASTER_COMMANDr_CLR(master_command);
    SMBUS_MASTER_DATA_READr_CLR(data_read);

    while (rt > 0) {
        /* Slave address first. */
        rval = I2C_RX_ADDR(saddr);
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        rval = 0;
        SMBUS_MASTER_COMMANDr_SMBUS_PROTOCOLf_SET(master_command, CMICX_I2C_PROTOCOL_RECEIVE_BYTE);
        WRITE_SMBUS_MASTER_COMMANDr(unit, chan, master_command);

        rv = cmicx_i2c_start_wait(unit, chan);
        if (rv == SHR_E_NONE) {
            READ_SMBUS_MASTER_DATA_READr(unit, chan, &data_read);
            *data = SMBUS_MASTER_DATA_READr_MASTER_SMBUS_RD_DATAf_GET(data_read);
            break;
        } else {
            rt--;
        }
    }

    if (rt <= 0) {
        rv = SHR_E_TIMEOUT;
    }

    return rv;
}

/*!
 * \brief Write two bytes data to slave device.
 *
 * \Data format on bus: Addr+W  Data_byte_1 Data_byte_2
 *
 * \param [in] unit  Unit number.
 * \param [in] chan  SMBus channel.
 * \param [in] saddr Slave device address.
 * \param [in] data  Data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
static int cmicx_i2c_write_word(int unit, uint8_t chan, i2c_addr_t saddr,
                                uint16_t data)
{
    int rv = SHR_E_NONE;
    SMBUS_MASTER_DATA_WRITEr_t data_write;
    SMBUS_MASTER_COMMANDr_t master_command;
    uint8_t byte0 = (uint8_t) (data & 0x00ff);
    uint8_t byte1 = (uint8_t) ((data & 0xff00) >> 8);
    uint32_t rval;
    int rt = 5;

    SMBUS_MASTER_DATA_WRITEr_CLR(data_write);
    SMBUS_MASTER_COMMANDr_CLR(master_command);

    while (rt > 0) {
        /* Slave address first. */
        rval = I2C_TX_ADDR(saddr);
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Write byte 0 to FIFO */
        rval = byte0;
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Write byte 1 to FIFO and mark it last byte */
        rval = byte1;
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        SMBUS_MASTER_DATA_WRITEr_MASTER_WR_STATUSf_SET(data_write, 1);

        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        rval = 0;
        SMBUS_MASTER_COMMANDr_SMBUS_PROTOCOLf_SET(master_command, CMICX_I2C_PROTOCOL_WRITE_BYTE);
        WRITE_SMBUS_MASTER_COMMANDr(unit, chan, master_command);

        rv = cmicx_i2c_start_wait(unit, chan);
        if (rv == SHR_E_NONE) {
            break;
        } else {
           rt--;
        }
    }

    if (rt <= 0) {
        rv = SHR_E_TIMEOUT;
    }

    return rv;
}

/*!
 * \brief Read two bytes data from slave device.
 *
 * \Data format on bus: Addr+R  Data_byte_1 Data_byte_2
 *
 * \param [in] unit   Unit number.
 * \param [in] chan   SMBus channel.
 * \param [in] saddr  Slave device address.
 * \param [out] data  Read back data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
static int cmicx_i2c_read_word(int unit, uint8_t chan, i2c_addr_t saddr,
                               uint16_t *data)
{
    int rv = SHR_E_NONE;
    SMBUS_MASTER_DATA_WRITEr_t data_write;
    SMBUS_MASTER_COMMANDr_t master_command;
    SMBUS_MASTER_DATA_READr_t  data_read;
    uint8_t byte0;
    uint8_t byte1;
    uint32_t rval;
    int rt = 5;

    SMBUS_MASTER_DATA_WRITEr_CLR(data_write);
    SMBUS_MASTER_COMMANDr_CLR(master_command);
    SMBUS_MASTER_DATA_READr_CLR(data_read);

    while (rt > 0) {
        /* Slave address first. */
        rval = I2C_RX_ADDR(saddr);
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        rval = 0;
        SMBUS_MASTER_COMMANDr_SMBUS_PROTOCOLf_SET(master_command, CMICX_I2C_PROTOCOL_READ_BYTE);
        WRITE_SMBUS_MASTER_COMMANDr(unit, chan, master_command);

        rv = cmicx_i2c_start_wait(unit, chan);
        if (rv == SHR_E_NONE) {
            READ_SMBUS_MASTER_DATA_READr(unit, chan, &data_read);
            byte0 = SMBUS_MASTER_DATA_READr_MASTER_SMBUS_RD_DATAf_GET(data_read);

            READ_SMBUS_MASTER_DATA_READr(unit, chan, &data_read);
            byte1 = SMBUS_MASTER_DATA_READr_MASTER_SMBUS_RD_DATAf_GET(data_read);

            *data = ((byte1 & 0xff) << 8) + (byte0 & 0xff);
            break;
        } else {
            rt--;
        }
    }

    if (rt <= 0) {
        rv = SHR_E_TIMEOUT;
    }

    return rv;
}

/*!
 * \brief Write one byte data per command type of slave device.
 *
 * \Data format on bus: Addr+W command Data_byte_1
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
static int cmicx_i2c_write_byte_data(int unit, uint8_t chan, i2c_addr_t saddr,
                                     uint8_t command, uint8_t data)
{
    int rv = SHR_E_NONE;
    SMBUS_MASTER_DATA_WRITEr_t data_write;
    SMBUS_MASTER_COMMANDr_t master_command;
    uint32_t rval;
    int rt = 5;

    SMBUS_MASTER_DATA_WRITEr_CLR(data_write);
    SMBUS_MASTER_COMMANDr_CLR(master_command);

    while (rt > 0) {
        /* Frist byte - Slave address for write op. */
        rval = I2C_TX_ADDR(saddr);
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Second byte - command to write to. */
        rval = command;
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Third byte - data to write. */
        rval = data;
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        SMBUS_MASTER_DATA_WRITEr_MASTER_WR_STATUSf_SET(data_write, 1);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        rval = 0;
        SMBUS_MASTER_COMMANDr_SMBUS_PROTOCOLf_SET(master_command, CMICX_I2C_PROTOCOL_WRITE_BYTE);
        WRITE_SMBUS_MASTER_COMMANDr(unit, chan, master_command);

        rv = cmicx_i2c_start_wait(unit, chan);
        if (rv == SHR_E_NONE) {
            break;
        } else {
           rt--;
        }
    }

    if (rt <= 0) {
        rv = SHR_E_TIMEOUT;
    }

    return rv;
}

/*!
 * \brief Read one byte data from a specific command of slave device.
 *
 * \Data format on bus: Addr+W command Addr+R Data_byte_1
 *
 * \param [in] unit    Unit number.
 * \param [in] chan    SMBus channel.
 * \param [in] saddr   Slave device address.
 * \param [in] command Command type per slave device specification.
 * \param [out] data   Read out data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
static int cmicx_i2c_read_byte_data(int unit, uint8_t chan, i2c_addr_t saddr,
                                    uint8_t command, uint8_t *data)
{
    int rv = SHR_E_NONE;
    SMBUS_MASTER_DATA_WRITEr_t data_write;
    SMBUS_MASTER_COMMANDr_t master_command;
    SMBUS_MASTER_DATA_READr_t  data_read;
    uint32_t rval;
    int rt = 5;

    SMBUS_MASTER_DATA_WRITEr_CLR(data_write);
    SMBUS_MASTER_COMMANDr_CLR(master_command);
    SMBUS_MASTER_DATA_READr_CLR(data_read);

    while (rt > 0) {
        /* First byte - slave address for write op. */
        rval = I2C_TX_ADDR(saddr);
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Second byte - command to read data from. */
        rval = command;
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Third byte - slave address for read op and mark it last byte. */
        rval = I2C_RX_ADDR(saddr);
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        SMBUS_MASTER_DATA_WRITEr_MASTER_WR_STATUSf_SET(data_write, 1);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        rval = 0;
        SMBUS_MASTER_COMMANDr_SMBUS_PROTOCOLf_SET(master_command, CMICX_I2C_PROTOCOL_READ_BYTE);
        WRITE_SMBUS_MASTER_COMMANDr(unit, chan, master_command);

        rv = cmicx_i2c_start_wait(unit, chan);
        if (rv == SHR_E_NONE) {
            READ_SMBUS_MASTER_DATA_READr(unit, chan, &data_read);
            *data = SMBUS_MASTER_DATA_READr_MASTER_SMBUS_RD_DATAf_GET(data_read);
            break;
        } else {
            rt--;
        }
    }

    if (rt <= 0) {
        rv = SHR_E_TIMEOUT;
    }

    return rv;
}

/*!
 * \brief Write two bytes data per command type of slave device.
 *
 * \Data format on bus: Addr+W command Data_byte_1 Data_byte_2
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
static int cmicx_i2c_write_word_data(int unit, uint8_t chan, i2c_addr_t saddr,
                                     uint8_t command, uint16_t data)
{
    int rv = SHR_E_NONE;
    SMBUS_MASTER_DATA_WRITEr_t data_write;
    SMBUS_MASTER_COMMANDr_t master_command;
    uint32_t rval;
    int rt = 5;

    SMBUS_MASTER_DATA_WRITEr_CLR(data_write);
    SMBUS_MASTER_COMMANDr_CLR(master_command);

    while (rt > 0) {
        /* Frist byte - Slave address for write op. */
        rval = I2C_TX_ADDR(saddr);
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Second byte - command to write to. */
        rval = command;
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Third byte - 1st data byte to write. */
        rval = (uint8_t) data & 0x00ff;
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Fourth byte - second data to write. */
        rval = (uint8_t) ((data >> 8) & 0x00ff);
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        SMBUS_MASTER_DATA_WRITEr_MASTER_WR_STATUSf_SET(data_write, 1);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        rval = 0;
        SMBUS_MASTER_COMMANDr_SMBUS_PROTOCOLf_SET(master_command, CMICX_I2C_PROTOCOL_WRITE_WORD);
        WRITE_SMBUS_MASTER_COMMANDr(unit, chan, master_command);

        rv = cmicx_i2c_start_wait(unit, chan);
        if (rv == SHR_E_NONE) {
            break;
        } else {
           rt--;
        }
    }

    if (rt <= 0) {
        rv = SHR_E_TIMEOUT;
    }

    return rv;
}

/*!
 * \brief Read two bytes data from a specific command of slave device.
 *
 * \Data format on bus: Addr+W command Addr_R Data_byte_1 Data_byte_2
 *
 * \param [in] unit    Unit number.
 * \param [in] chan    SMBus channel.
 * \param [in] saddr   Slave device address.
 * \param [in] command Command type per slave device specification.
 * \param [out] data   Read out data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
static int cmicx_i2c_read_word_data(int unit, uint8_t chan, i2c_addr_t saddr,
                                    uint8_t command, uint16_t *data)
{
    int rv = SHR_E_NONE;
    SMBUS_MASTER_DATA_WRITEr_t data_write;
    SMBUS_MASTER_COMMANDr_t master_command;
    SMBUS_MASTER_DATA_READr_t data_read;
    uint32_t rval;
    uint8_t byte0, byte1;
    int rt = 5;

    SMBUS_MASTER_DATA_WRITEr_CLR(data_write);
    SMBUS_MASTER_COMMANDr_CLR(master_command);
    SMBUS_MASTER_DATA_READr_CLR(data_read);

    while (rt > 0) {
        /* First byte - slave address for write op. */
        rval = I2C_TX_ADDR(saddr);
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Second byte - command to read data from. */
        rval = command;
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Third byte - slave address for read op and mark it last byte. */
        rval = I2C_RX_ADDR(saddr);
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        SMBUS_MASTER_DATA_WRITEr_MASTER_WR_STATUSf_SET(data_write, 1);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        rval = 0;
        SMBUS_MASTER_COMMANDr_SMBUS_PROTOCOLf_SET(master_command, CMICX_I2C_PROTOCOL_READ_WORD);
        WRITE_SMBUS_MASTER_COMMANDr(unit, chan, master_command);

        rv = cmicx_i2c_start_wait(unit, chan);
        if (rv == SHR_E_NONE) {
            /* Read out first data byte */
            READ_SMBUS_MASTER_DATA_READr(unit, chan, &data_read);
            byte0 = SMBUS_MASTER_DATA_READr_MASTER_SMBUS_RD_DATAf_GET(data_read);

            /* Read out second data byte */
            READ_SMBUS_MASTER_DATA_READr(unit, chan, &data_read);
            byte1 = SMBUS_MASTER_DATA_READr_MASTER_SMBUS_RD_DATAf_GET(data_read);

            *data = ((byte1 & 0xff) << 8) + (byte0 & 0xff);
            break;
        } else {
            rt--;
        }
    }

    if (rt <= 0) {
        rv = SHR_E_TIMEOUT;
    }

    return rv;
}

/*!
 * \brief Write number of bytes per command type of slave device.
 *
 * \Data format on bus: Addr+W command Byte_cnt Data_byte_1 .. Data_byte_cnt
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
static int cmicx_i2c_block_write(int unit, uint8_t chan, i2c_addr_t saddr,
                                 uint8_t command, uint8_t cnt, uint8_t *data)
{
    int rv = SHR_E_NONE;
    SMBUS_MASTER_DATA_WRITEr_t data_write;
    SMBUS_MASTER_COMMANDr_t master_command;
    uint32_t rval;
    int rt = 5;
    int ix;
    uint8_t *ptr;


    if (cnt < 1) {
       return SHR_E_PARAM;
    }

    SMBUS_MASTER_DATA_WRITEr_CLR(data_write);
    SMBUS_MASTER_COMMANDr_CLR(master_command);

    while (rt > 0){
        /* Frist byte - Slave address for write op. */
        rval = I2C_TX_ADDR(saddr);
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Second byte - command to write to. */
        rval = command;
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Third byte - total byte cnt. */
        rval = cnt;
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        SMBUS_MASTER_DATA_WRITEr_MASTER_WR_STATUSf_SET(data_write, 1);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Write data byte to FIFO one by one */
        ptr = data;
        for (ix = 0; ix < cnt ; ix++, ptr++) {
            rval = (uint32_t) *ptr;
            SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
            if (ix == (cnt-1)) {
                /* Mark last byte */
                SMBUS_MASTER_DATA_WRITEr_MASTER_WR_STATUSf_SET(data_write, 1);
            }
            WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);
        }

        rval = 0;
        SMBUS_MASTER_COMMANDr_SMBUS_PROTOCOLf_SET(master_command, CMICX_I2C_PROTOCOL_WRITE_BLOCK);
        WRITE_SMBUS_MASTER_COMMANDr(unit, chan, master_command);

        rv = cmicx_i2c_start_wait(unit, chan);
        if (rv == SHR_E_NONE) {
            break;
        } else {
           rt--;
        }
    }

    if (rt <= 0) {
        rv = SHR_E_TIMEOUT;
    }

    return rv;
}

/*!
 * \brief Read number of bytes from a specific command of slave device.
 *
 * Data formats: Addr+W command Addr+R Byte_cnt Data_1 ... Data_cnt
 *
 * *cnt is read back from slave device, it is up to the caller of this API
 * to make sure "data buffer" to hold up to I2C_READ_FIFO_SIZE.
 *
 * If user is not sure about how to use this API, use byte or word read.
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
static int cmicx_i2c_block_read(int unit, uint8_t chan, i2c_addr_t saddr,
                                uint8_t command, uint8_t *cnt, uint8_t *data)
{
    int rv = SHR_E_NONE;
    SMBUS_MASTER_DATA_WRITEr_t data_write;
    SMBUS_MASTER_COMMANDr_t master_command;
    SMBUS_MASTER_DATA_READr_t  data_read;
    uint32_t rval;
    uint8_t ix;
    uint8_t *ptr;
    int rt = 5;

    SMBUS_MASTER_DATA_WRITEr_CLR(data_write);
    SMBUS_MASTER_COMMANDr_CLR(master_command);
    SMBUS_MASTER_DATA_READr_CLR(data_read);

    while (rt > 0) {
        /* First byte - slave address for write op. */
        rval = I2C_TX_ADDR(saddr);
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Second byte - command to read data from. */
        rval = command;
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Third byte - slave address for read op and mark it last byte. */
        rval = I2C_RX_ADDR(saddr);
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        SMBUS_MASTER_DATA_WRITEr_MASTER_WR_STATUSf_SET(data_write, 1);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        rval = 0;
        SMBUS_MASTER_COMMANDr_SMBUS_PROTOCOLf_SET(master_command, CMICX_I2C_PROTOCOL_READ_BLOCK);
        WRITE_SMBUS_MASTER_COMMANDr(unit, chan, master_command);

        rv = cmicx_i2c_start_wait(unit, chan);
        if (rv == SHR_E_NONE) {
            /* First byte in read buffer is cnt */
            READ_SMBUS_MASTER_DATA_READr(unit, chan, &data_read);

            *cnt = SMBUS_MASTER_DATA_READr_MASTER_SMBUS_RD_DATAf_GET(data_read);
            if (*cnt > I2C_READ_FIFO_SIZE) {
                return SHR_E_MEMORY;
            }

            ptr = data;
            for (ix = 0; ix < *cnt; ix++, ptr++) {
                READ_SMBUS_MASTER_DATA_READr(unit, chan, &data_read);
                *ptr = (uint8_t)(SMBUS_MASTER_DATA_READr_MASTER_SMBUS_RD_DATAf_GET(data_read) & 0xff);
            }
            break;

        } else {
            rt--;
        }
    }

    if (rt <= 0) {
        rv = SHR_E_TIMEOUT;
    }

    return rv;
}

/*!
 * \brief Write number of bytes per command type of slave device.
 * \brief Data count information is not shown on bus.
 *
 * \Data format on bus: Addr+W command Data_byte_1 Data_byte_2 ... Data_byte_cnt
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
static int cmicx_i2c_multi_write(int unit, uint8_t chan, i2c_addr_t saddr,
                                 uint8_t command, uint8_t cnt, uint8_t *data)
{
    int rv = SHR_E_NONE;
    SMBUS_MASTER_DATA_WRITEr_t data_write;
    SMBUS_MASTER_COMMANDr_t master_command;
    uint32_t rval;
    int rt = 5;
    int ix;
    uint8_t *ptr;


    if (cnt < 1) {
       return SHR_E_PARAM;
    }

    SMBUS_MASTER_DATA_WRITEr_CLR(data_write);
    SMBUS_MASTER_COMMANDr_CLR(master_command);

    while (rt > 0) {
        /* Frist byte - Slave address for write op. */
        rval = I2C_TX_ADDR(saddr);
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Second byte - command to write to. */
        rval = command;
        SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
        WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);

        /* Write data byte to FIFO one by one */
        ptr = data;
        for (ix = 0; ix < cnt ; ix++, ptr++) {
            rval = (uint32_t) *ptr;
            SMBUS_MASTER_DATA_WRITEr_MASTER_SMBUS_WR_DATAf_SET(data_write, rval);
            if (ix == (cnt-1)) {
                /* Mark last byte */
                SMBUS_MASTER_DATA_WRITEr_MASTER_WR_STATUSf_SET(data_write, 1);
            }
            WRITE_SMBUS_MASTER_DATA_WRITEr(unit, chan, data_write);
        }

        rval = 0;
        SMBUS_MASTER_COMMANDr_SMBUS_PROTOCOLf_SET(master_command, CMICX_I2C_PROTOCOL_WRITE_BLOCK);
        WRITE_SMBUS_MASTER_COMMANDr(unit, chan, master_command);

        rv = cmicx_i2c_start_wait(unit, chan);
        if (rv == SHR_E_NONE) {
            break;
        } else {
           rt--;
        }
    }

    if (rt <= 0) {
        rv = SHR_E_TIMEOUT;
    }

    return rv;
}


static int
cmicx_i2c_init(int unit, uint8_t chan)
{
    SMBUS_CONFIGr_t config;

    /* claim bus */
    SMBUS_CONFIGr_CLR(config);

    READ_SMBUS_CONFIGr(unit, chan, &config);
    SMBUS_CONFIGr_SMB_ENf_SET(config, 1);
    WRITE_SMBUS_CONFIGr(unit, chan, config);

    return SHR_E_NONE;
}

static int
cmicx_i2c_cleanup(int unit, uint8_t chan)
{
    SMBUS_CONFIGr_t config;

    /* release bus */
    SMBUS_CONFIGr_CLR(config);

    READ_SMBUS_CONFIGr(unit, chan, &config);
    SMBUS_CONFIGr_SMB_ENf_SET(config, 0);
    WRITE_SMBUS_CONFIGr(unit, chan, config);

    return SHR_E_NONE;
}

/*******************************************************************************
 * Driver object
 */

static bcmbd_i2c_ops_t cmicx_i2c_drv = {
    .cleanup            = cmicx_i2c_cleanup,
    .init               = cmicx_i2c_init,
    .write_byte         = cmicx_i2c_write_byte,
    .read_byte          = cmicx_i2c_read_byte,
    .write_word         = cmicx_i2c_write_word,
    .read_word          = cmicx_i2c_read_word,
    .write_byte_data    = cmicx_i2c_write_byte_data,
    .read_byte_data     = cmicx_i2c_read_byte_data,
    .write_word_data    = cmicx_i2c_write_word_data,
    .read_word_data     = cmicx_i2c_read_word_data,
    .block_write        = cmicx_i2c_block_write,
    .block_read         = cmicx_i2c_block_read,
    .multi_write        = cmicx_i2c_multi_write
};

/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicx_i2c_drv_init(int unit, uint8_t chan)
{
    return bcmbd_i2c_drv_init(unit, chan, &cmicx_i2c_drv);
}

int bcmbd_cmicx_i2c_drv_cleanup(int unit, uint8_t chan)
{
    return bcmbd_i2c_drv_cleanup(unit);
}

