/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UTILS_I2C_MEM_H_INCLUDED__
/* { */
#define __UTILS_I2C_MEM_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif


#define I2C_MEM_MAX_IC2_BUFF_SIZE  32


#define I2C_MEM_ERR_BASE 6660

#define I2C_MEM_ERR_01 (I2C_MEM_ERR_BASE + 1)
#define I2C_MEM_ERR_02 (I2C_MEM_ERR_BASE + 2)
#define I2C_MEM_ERR_03 (I2C_MEM_ERR_BASE + 3)
#define I2C_MEM_ERR_04 (I2C_MEM_ERR_BASE + 4)
#define I2C_MEM_ERR_05 (I2C_MEM_ERR_BASE + 5)
#define I2C_MEM_ERR_06 (I2C_MEM_ERR_BASE + 6)
#define I2C_MEM_ERR_07 (I2C_MEM_ERR_BASE + 7)
#define I2C_MEM_ERR_08 (I2C_MEM_ERR_BASE + 8)
#define I2C_MEM_ERR_09 (I2C_MEM_ERR_BASE + 9)
#define I2C_MEM_ERR_10 (I2C_MEM_ERR_BASE + 10)

/*
 * }
 */


int
  utils_i2c_mem_read(
    unsigned char device_address,/*7 bits*/
    unsigned char i2c_data[I2C_MEM_MAX_IC2_BUFF_SIZE],
    unsigned short  i2c_data_len,
    unsigned int  silent
  );


int
  utils_i2c_mem_write(
    unsigned char device_address,/*7 bits*/
    unsigned char i2c_data[I2C_MEM_MAX_IC2_BUFF_SIZE],
    unsigned short  i2c_data_len,
    unsigned int  silent
  );


#ifdef  __cplusplus
}
#endif


/* } __UTILS_I2C_MEM_H_INCLUDED__*/
#endif



