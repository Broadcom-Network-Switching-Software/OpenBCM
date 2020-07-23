 /* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef SOC_DPP_I2C_H
#define SOC_DPP_I2C_H

#ifdef  __cplusplus
extern "C" {
#endif


  

#define SOC_DPP_I2C_DEVICE_FILE_NAME_PREFIX "/dev/i2c-" 

#define SOC_DPP_I2C_MEM_ACCESS_FD_INVALID (-1)
  
typedef enum 
{
  SOC_DPP_I2C_ALEN_CHAR = 0x1,
  SOC_DPP_I2C_ALEN_WORD = 0x2,
  SOC_DPP_I2C_ALEN_LONG = 0x4,
  SOC_DPP_I2C_ALEN_CHAR_DLEN_WORD = 0x12, 
  SOC_DPP_I2C_ALEN_CHAR_DLEN_LONG = 0x14, 
  SOC_DPP_I2C_ALEN_WORD_DLEN_LONG = 0x24,
  SOC_DPP_I2C_ALEN_LONG_DLEN_WORD = 0x42, 
  SOC_DPP_I2C_ALEN_LONG_DLEN_LONG = 0x44, 
  SOC_DPP_I2C_ALEN_WORD_DLEN_CHAR = 0x21, 
  SOC_DPP_I2C_ALEN_LONG_DLEN_CHAR = 0x41 
} SOC_DPP_I2C_BUS_LEN;

typedef struct  
{
  int file_desc;
  char chip; 
  int addr;
  int alen; 
  int data;
  int no_addr; 
} SocDppI2CAccess;

int soc_dpp_i2c_main(int argc, char* argv[]);


int soc_dpp_i2c_init(int* p_fd, char dev_num);


int soc_dpp_i2c_deinit(int fd);


int soc_dpp_i2c_lock(void);


int soc_dpp_i2c_unlock(void);


int soc_dpp_i2c_read(SocDppI2CAccess* p_i2c_access);


int soc_dpp_i2c_write(SocDppI2CAccess* p_i2c_access);


typedef enum 
{
  SOC_DPP_I2C_CARD_LINE = 0,
  SOC_DPP_I2C_CARD_FABRIC,
  SOC_DPP_I2C_CARD_MESH,
  SOC_DPP_I2C_CARD_MGMT,
  SOC_DPP_I2C_CARD_CHASSIS,
  SOC_DPP_I2C_CARD_NUM_CARDS
} SOC_DPP_I2C_CARD;


typedef enum 
{
  
  SOC_DPP_I2C_CHIP_LINE_EEPROM = 0,
  SOC_DPP_I2C_CHIP_LINE_FPGA,
  SOC_DPP_I2C_CHIP_LINE_TEMP_0,
  SOC_DPP_I2C_CHIP_LINE_TEMP_1,
  SOC_DPP_I2C_CHIP_LINE_TEMP_2,
  SOC_DPP_I2C_CHIP_LINE_AD_0,
  SOC_DPP_I2C_CHIP_LINE_AD_1,
  SOC_DPP_I2C_CHIP_LINE_AD_2,
  SOC_DPP_I2C_CHIP_LINE_TEMP_PETRA_0,
  SOC_DPP_I2C_CHIP_LINE_TEMP_PETRA_1,
  SOC_DPP_I2C_CHIP_LINE_TEMP_PETRA_2,
  SOC_DPP_I2C_CHIP_LINE_TEMP_PETRA_3,
  SOC_DPP_I2C_CHIP_LINE_NUM_CHIPS,
  
  
  SOC_DPP_I2C_CHIP_FABRIC_AD_0  = 0, 
  SOC_DPP_I2C_CHIP_FABRIC_GPIO_0, 
  SOC_DPP_I2C_CHIP_FABRIC_GPIO_1, 
  SOC_DPP_I2C_CHIP_FABRIC_TEMP_0, 
  SOC_DPP_I2C_CHIP_FABRIC_EEPROM, 
  SOC_DPP_I2C_CHIP_FABRIC_FAN_0, 
  SOC_DPP_I2C_CHIP_FABRIC_FE_0, 
  SOC_DPP_I2C_CHIP_FABRIC_FPGA,
  SOC_DPP_I2C_CHIP_FABRIC_NUM_CHIPS,

  
  SOC_DPP_I2C_CHIP_MESH_EEPROM  = 0, 
  SOC_DPP_I2C_CHIP_MESH_FAN_0,
  SOC_DPP_I2C_CHIP_MESH_FAN_1,
  SOC_DPP_I2C_CHIP_MESH_TEMP_0,
  SOC_DPP_I2C_CHIP_MESH_NUM_CHIPS,

  
  SOC_DPP_I2C_CHIP_MGMT_EEPROM  = 0, 
  SOC_DPP_I2C_CHIP_MGMT_TEMP_0 , 
  SOC_DPP_I2C_CHIP_MGMT_TEMP_1 , 
  SOC_DPP_I2C_CHIP_MGMT_NUM_CHIPS,
  
  
  SOC_DPP_I2C_CHIP_CHASSIS_EEPROM  = 0, 
  SOC_DPP_I2C_CHIP_CHASSIS_NUM_CHIPS
} SOC_DPP_I2C_CHIP;



int soc_dpp_i2c_write_external(int chip, int no_addr, int addr, SOC_DPP_I2C_BUS_LEN alen, int val);


int soc_dpp_i2c_read_external(int chip, int no_addr, int addr, SOC_DPP_I2C_BUS_LEN alen, int* p_val);

#ifdef  __cplusplus
}
#endif

#endif 
