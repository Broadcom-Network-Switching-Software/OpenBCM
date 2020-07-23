/*! \file bcmbd_i2c_cmicx_internal.h
 *
 * BD I2C internal API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_I2C_CMICX_INTERNAL_H
#define BCMBD_I2C_CMICX_INTERNAL_H

/*! Data type for slave address. */
typedef uint8_t i2c_addr_t;

/*! CMICX SMBus channel number. */
#define NUM_I2C_CHAN      2

/*! Read mask bit to append to address byte. */
#define I2C_READ_MASK     0x01

/*! Add operation bit to RX address. */
#define I2C_RX_ADDR(addr) (((addr)<<1) | (I2C_READ_MASK))

/*! Add operation bit to TX address. */
#define I2C_TX_ADDR(addr) (((addr)<<1) & ~(I2C_READ_MASK))

/*! Types of CMICX i2c operations. */
typedef enum cmicx_i2c_protocol_s {
    /*! Quick_command. */
    CMICX_I2C_PROTOCOL_QUICK_CMD = 0,

    /*! Send data byte only. */
    CMICX_I2C_PROTOCOL_SEND_BYTE,

    /*! Read data byte only. */
    CMICX_I2C_PROTOCOL_RECEIVE_BYTE,

    /*! Write one data byte with command. */
    CMICX_I2C_PROTOCOL_WRITE_BYTE,

    /*! Read one data byte with command. */
    CMICX_I2C_PROTOCOL_READ_BYTE,

    /*! Write two bytes with command. */
    CMICX_I2C_PROTOCOL_WRITE_WORD,

    /*! Read two bytes with command. */
    CMICX_I2C_PROTOCOL_READ_WORD,

    /*! Block write with command and count. */
    CMICX_I2C_PROTOCOL_WRITE_BLOCK,

    /*! Block read with command and count. */
    CMICX_I2C_PROTOCOL_READ_BLOCK,

    /*! Multiple write with command. */
    CMICX_I2C_PROTOCOL_WRITE_PROCCALL,

    /*! Multiple read with command. */
    CMICX_I2C_PROTOCOL_READ_PROCCALL
} cmicx_i2c_protocol_t;

#endif /* BCMBD_I2C_CMICX_INTERNAL_H */
