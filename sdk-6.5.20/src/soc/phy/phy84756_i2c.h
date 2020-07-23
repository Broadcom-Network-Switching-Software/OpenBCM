/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PHY84756_I2C_H
#define _PHY84756_I2C_H
/* I2C related defines */
#define PHY84756_BSC_XFER_MAX     0x1F9
#define PHY84756_BSC_WR_MAX       16
#define PHY84756_WRITE_START_ADDR 0x8007
#define PHY84756_READ_START_ADDR  0x8007
#define PHY84756_WR_FREQ_400KHZ   0x0100
#define PHY84756_2W_STAT          0x000C
#define PHY84756_2W_STAT_IDLE     0x0000
#define PHY84756_2W_STAT_COMPLETE 0x0004
#define PHY84756_2W_STAT_IN_PRG   0x0008
#define PHY84756_2W_STAT_FAIL     0x000C
#define PHY84756_BSC_WRITE_OP     0x22
#define PHY84756_BSC_READ_OP      0x2
#define PHY84756_I2CDEV_WRITE     0x1
#define PHY84756_I2CDEV_READ      0x0
#define PHY84756_I2C_8BIT         0
#define PHY84756_I2C_16BIT        1
#define PHY84756_I2C_TEMP_RAM     0xE
#define PHY84756_I2C_OP_TYPE(access_type,data_type) \
        ((access_type) | ((data_type) << 8))
#define PHY84756_I2C_ACCESS_TYPE(op_type) ((op_type) & 0xff)
#define PHY84756_I2C_DATA_TYPE(op_type)   (((op_type) >> 8) & 0xff)

extern int
_phy_84756_bsc_rw(phy_ctrl_t *pc, int dev_addr, int opr,
                    int addr, int count, void *data_array,buint32_t ram_start);

extern int
phy_84756_i2cdev_read(phy_ctrl_t *pc,
                     int dev_addr,  /* 7 bit I2C bus device address */
                     int offset,    /* starting data address to read */
                     int nbytes,    /* number of bytes to read */
                     buint8_t *read_array);   /* buffer to hold retrieved data */


extern int
phy_84756_i2cdev_write(phy_ctrl_t *pc,
                     int dev_addr,  /* I2C bus device address */
                     int offset,    /* starting data address to write to */
                     int nbytes,    /* number of bytes to write */
                     buint8_t *write_array);   /* buffer to hold written data */

#endif /* _PHY84756_I2C_H */


