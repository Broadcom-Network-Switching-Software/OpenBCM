/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef __DNXC_CRC_H__
#define __DNXC_CRC_H__

shr_error_e soc_dnxc_crc_calc(
    int unit,
    uint8 *payload,
    int payload_size,
    uint16 *crc_result);

#endif
