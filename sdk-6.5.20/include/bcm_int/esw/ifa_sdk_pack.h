/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ifa_sdk_pack.h
 */

#ifndef IFA_SDK_PACK_H_
#define IFA_SDK_PACK_H_

#include <bcm_int/esw/ifa_sdk_msg.h>
#include <bcm_int/esw/ifa_feature.h>

uint8*
ifa_sdk_msg_ctrl_config_info_pack(uint8* buf,
        ifa_sdk_msg_ctrl_config_info_t *msg);

uint8*
ifa_sdk_msg_ctrl_config_info_unpack(uint8* buf,
        ifa_sdk_msg_ctrl_config_info_t *msg);
#endif /* IFA_SDK_PACK_H_ */
