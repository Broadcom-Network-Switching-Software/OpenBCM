/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ifa_pack.h
 */

#ifndef SHR_IFA_PACK_H_
#define SHR_IFA_PACK_H_

#ifdef BCM_UKERNEL
/* Build for uKernel not SDK */
#include "sdk_typedefs.h"
#else
#include <sal/types.h>
#endif
#include <soc/shared/ifa_msg.h>

uint8*
shr_ifa_msg_ctrl_init_pack(uint8 *buf, shr_ifa_msg_ctrl_init_t *msg);

uint8*
shr_ifa_msg_ctrl_init_unpack(uint8 *buf, shr_ifa_msg_ctrl_init_t *msg);

uint8*
shr_ifa_msg_ctrl_collector_info_pack(uint8* buf,
        shr_ifa_msg_ctrl_collector_info_t *msg);

uint8*
shr_ifa_msg_ctrl_collector_info_unpack(uint8* buf,
        shr_ifa_msg_ctrl_collector_info_t *msg);

uint8*
shr_ifa_msg_ctrl_config_info_pack(uint8* buf,
        shr_ifa_msg_ctrl_config_info_t *msg);

uint8*
shr_ifa_msg_ctrl_config_info_unpack(uint8* buf,
        shr_ifa_msg_ctrl_config_info_t *msg);

uint8*
shr_ifa_msg_ctrl_stat_info_pack(uint8* buf,
        shr_ifa_msg_ctrl_stat_info_t *msg);

uint8*
shr_ifa_msg_ctrl_stat_info_unpack(uint8* buf,
        shr_ifa_msg_ctrl_stat_info_t *msg);
#endif /* SHR_IFA_PACK_H_ */
