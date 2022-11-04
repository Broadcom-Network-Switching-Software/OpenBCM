/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:    epfc_sdk_pack.h
 */

#ifndef PFC_SDK_PACK_H_
#define PFC_SDK_PACK_H_

#include <bcm_int/dnx/port/epfc/epfc_sdk_msg.h>
#include <shared/pack.h>

uint8 *pfc_sdk_dpp_msg_ctrl_init_pack(
    uint8 *buf,
    pfc_sdk_msg_ctrl_init_t * msg);

uint8 *pfc_sdk_msg_stat_reply_unpack(
    uint8 *buf,
    pfc_sdk_msg_ctrl_stat_reply_t * msg);

uint8 *pfc_sdk_msg_stat_pack(
    uint8 *buf,
    pfc_sdk_msg_ctrl_stat_t * msg);

uint8 *pfc_sdk_msg_port_config_pack(
    uint8 *buf,
    pfc_sdk_msg_ctrl_port_config_t * msg);

uint8 *pfc_sdk_msg_port_get_reply_unpack(
    uint8 *buf,
    pfc_sdk_msg_ctrl_port_get_reply_t * msg);
#endif /* PFC_SDK_PACK_H_ */
