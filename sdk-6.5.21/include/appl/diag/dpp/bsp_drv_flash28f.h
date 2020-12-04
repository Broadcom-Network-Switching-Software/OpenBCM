/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __BSP_DRV_FLASH28F_INCLUDED
#define __BSP_DRV_FLASH28F_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* defines */


#define  FLASH28_CMD_READ_ID        (FLASH_28F_DEF) 0x90909090
#define  FLASH28_CMD_ERASE_SETUP    (FLASH_28F_DEF) 0x20202020
#define  FLASH28_CMD_PROG_SETUP     (FLASH_28F_DEF) 0x40404040
#define  FLASH28_CMD_RESET           (FLASH_28F_DEF) 0xffffffff
#define  FLASH28F008_CMD_READ_STATUS  (FLASH_28F_DEF) 0x70707070
#define  FLASH28F008_CMD_CLEAR_STATUS (FLASH_28F_DEF) 0x50505050
#define  FLASH28F008_CMD_ERASE        (FLASH_28F_DEF) 0xD0D0D0D0

#define  FLASH28F320_BSP_CMD_LOCK_PREFIX  (FLASH_28F_DEF) 0x60606060
#define  FLASH28F320_BSP_CMD_CLEAR_LOCKS  (FLASH_28F_DEF) 0xD0D0D0D0
#define  FLASH28F320_BSP_CMD_RESUME       (FLASH_28F_DEF) 0xD0D0D0D0

#define  FLASH28F008_BSP_STAT_WSMS    (FLASH_28F_DEF) 0x00800080

#define  FLASH28F008_STAT_BWS     (FLASH_28F_DEF) 0x10101010
#define  FLASH28F008_STAT_EWS     (FLASH_28F_DEF) 0x20202020

#ifdef __cplusplus
}
#endif

#endif /* __BSP_DRV_FLASH28F_INCLUDED */
