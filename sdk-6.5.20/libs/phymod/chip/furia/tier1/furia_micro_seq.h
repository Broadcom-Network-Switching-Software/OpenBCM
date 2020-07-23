/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.  
 */
#ifndef __FURIA_MICRO_SEQ_H__
#define __FURIA_MICRO_SEQ_H__
/*
 * Includes
 */
#include <phymod/phymod.h>

/*
 *  Defines
 */
#define MICRO_CHECKSUM_ZERO_CODE (0x600D)

/* 
 *  Types
 */
typedef enum FURIA_MSGOUT
{
    MSGOUT_DONTCARE = 0x0000,
    MSGOUT_GET_CNT = 0x8888,
    MSGOUT_GET_LSB = 0xABCD,
    MSGOUT_GET_MSB = 0x4321,
    MSGOUT_GET_2B = 0xEEEE,
    MSGOUT_GET_B = 0xF00D,
    MSGOUT_ERR = 0x0BAD,
    MSGOUT_NEXT = 0x2222, 
    MSGOUT_NOT_DWNLD = 0x0101, 
    MSGOUT_DWNLD_ALREADY = 0x0202,
    MSGOUT_DWNLD_DONE = 0x0303,
    MSGOUT_PRGRM_DONE = 0x1414
} FURIA_MSGOUT_E;

/*
 *  Macros
 */

/*
 *  Global Variables
 */
  
/*
 *  Functions
 */

/**   Wait master message out 
 *    This function is to ensure whether master has sent the previous message
 *    out successfully 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param exp_message        Expected message specified by user  
 *    @param poll_time          Poll interval 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _furia_wait_mst_msgout(const phymod_access_t *pa,
                            FURIA_MSGOUT_E exp_message,
                            int poll_time);


/**   Download and Fuse firmware 
 *    This function is used to download the firmware through I2C/MDIO
 *    and fuse it to SPI EEPROM if prg_eeprom flag is set 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param new_fw             Pointer to firmware array 
 *    @param fw_length          Length of the firmware array 
 *    @param prg_eeprom               Flag used to program EEPROM
 *
 *    @return num_bytes         number of bytes successfully downloaded
 */
int furia_download_prog_eeprom(const phymod_access_t *pa,
                                 uint8_t *new_fw,
                                 uint32_t fw_length,
                                 uint8_t prg_eeprom);

int furia_firmware_info_get(const phymod_access_t *pa,
                            phymod_core_firmware_info_t *fw_info);
#endif
