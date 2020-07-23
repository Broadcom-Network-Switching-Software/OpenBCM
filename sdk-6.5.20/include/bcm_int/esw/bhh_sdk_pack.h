/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    bhh_sdk_pack.h
 */

#ifndef BHH_SDK_PACK_H_
#define BHH_SDK_PACK_H_

#if defined(INCLUDE_BHH)

#include <soc/shared/bhh_msg.h>
#include <bcm_int/esw/bhh_feature.h>
#include <bcm_int/esw/bhh_sdk_msg.h>

uint8 * bhh_sdk_version_exchange_msg_unpack(uint8 *buf,
                                    bhh_sdk_version_exchange_msg_t *msg);

uint8 * bhh_sdk_version_exchange_msg_pack(uint8 *buf,
                                    bhh_sdk_version_exchange_msg_t *msg);

uint8 * bhh_sdk_msg_ctrl_sess_get_unpack(uint8 *buf,
                              bhh_sdk_msg_ctrl_sess_get_t *msg);
uint8 * bhh_sdk_msg_ctrl_loss_get_unpack(uint8 *buf,
                              bhh_sdk_msg_ctrl_loss_get_t *msg);

uint8*
bhh_sdk_msg_ctrl_init_pack(uint8 unit, uint8 *buf, bhh_sdk_msg_ctrl_init_t *msg);

uint8*
bhh_sdk_msg_ctrl_rmep_create_pack(uint8 *buf, bhh_sdk_msg_ctrl_rmep_create_t *msg);

uint8*
bhh_sdk_msg_ctrl_loss_add_pack(uint8 *buf, bhh_sdk_msg_ctrl_loss_add_t *msg);

uint8*
bhh_sdk_msg_ctrl_delay_add_pack(uint8 *buf, bhh_sdk_msg_ctrl_delay_add_t *msg);

uint8*
bhh_sdk_msg_ctrl_loopback_add_pack(uint8 *buf,
                                   bhh_sdk_msg_ctrl_loopback_add_t *msg);
uint8 *
bhh_sdk_msg_ctrl_sess_set_pack(uint8 *buf, bhh_sdk_msg_ctrl_sess_set_t *msg);

uint8 *
bhh_sdk_msg_ctrl_session_update_pack(uint8 *buf, bhh_sdk_msg_session_update_t *msg);

uint8 *
bhh_sdk_msg_ctrl_delay_get_unpack(uint8 *buf, bhh_sdk_msg_ctrl_delay_get_t *msg);

uint8 *
bhh_sdk_msg_ctrl_loopback_get_unpack(uint8 *buf,
                                     bhh_sdk_msg_ctrl_loopback_get_t *msg);
uint8 *
bhh_sdk_msg_session_update_pack(uint8 *buf, bhh_sdk_msg_session_update_t *msg);

uint8 *
bhh_sdk_msg_ctrl_trunk_map_update_pack(uint8 *buf,
                                       bhh_sdk_msg_ctrl_trunk_map_update_t *msg);

uint8 *
bhh_sdk_msg_ctrl_trunk_map_get_unpack(uint8 *buf,
                                      bhh_sdk_msg_ctrl_trunk_map_get_t *msg);
uint8 *
bhh_sdk_msg_ctrl_csf_add_pack(uint8 *buf, bhh_sdk_msg_ctrl_csf_add_t *msg);

uint8 *
bhh_sdk_msg_ctrl_csf_get_unpack(uint8 *buf, bhh_sdk_msg_ctrl_csf_get_t *msg);

uint8 *
bhh_sdk_msg_ctrl_ach_channel_type_msg_pack(uint8 *buf, uint32 *msg);

#endif
#endif /* BHH_SDK_PACK_H_ */

