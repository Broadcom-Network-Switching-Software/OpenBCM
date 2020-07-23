
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef OAM_OAMP_UTILS_H_INCLUDED

#define OAM_OAMP_UTILS_H_INCLUDED

 
#include <sal/types.h>





#define OAM_OAMP_CRC_CALC_NOF_BYTES_PER_ITERATION  (16)
#define OAM_OAMP_CRC_BUFFER_SIZE                   (128)
#define OAM_OAMP_CRC_MASK_MSB_BIT_SIZE  (64)
#define OAM_OAMP_CRC_MASK_MSB_BYTE_SIZE BITS2BYTES(OAM_OAMP_CRC_MASK_MSB_BIT_SIZE)
#define OAM_OAMP_CRC_MASK_LSB_BYTE_SIZE (120)






typedef struct
{
    uint8 msb_mask[OAM_OAMP_CRC_MASK_MSB_BYTE_SIZE];
    uint32 lsbyte_mask[BITS2WORDS(OAM_OAMP_CRC_MASK_LSB_BYTE_SIZE)];
} oam_oamp_crc_mask_t;

int dnx_oam_oamp_crc16(
    int unit,
    const uint8 *buff,
    uint32 start_index,
    uint32 data_length,
    uint16 *calculated_crc);

#endif 
