/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    bfd_feature.h
 */

#ifndef BFD_SDK_PACK_H_
#define BFD_SDK_PACK_H_

#include <soc/shared/bfd_msg.h>
#include <bcm_int/dpp/bfd_sdk_msg.h>

uint8 *
bfd_sdk_dpp_msg_ctrl_sess_set_pack(uint8 *buf,
                                bfd_sdk_msg_ctrl_sess_set_t *msg);

uint8 *
bfd_sdk_dpp_msg_ctrl_sess_get_unpack(uint8 *buf,
                                 bfd_sdk_msg_ctrl_sess_get_t *msg);
uint8 *
bfd_sdk_dpp_msg_ctrl_init_pack(uint8 *buf, bfd_sdk_msg_ctrl_init_t *msg);

uint8 * bfd_sdk_dpp_msg_ctrl_trace_log_enable_unpack(uint8 *buf,
                                                     bfd_sdk_msg_ctrl_trace_log_enable_t *msg);
uint8 *
bfd_sdk_dpp_msg_ctrl_stat_req_pack(uint8 *buf,
                                   shr_bfd_msg_ctrl_stat_req_t *msg);
uint8 * bfd_sdk_dpp_msg_ctrl_stat_reply_unpack(uint8 *buf,
                                                     bfd_sdk_msg_ctrl_stat_reply_t *msg);
uint8 *
bfd_sdk_dpp_msg_ctrl_discard_stat_get_unpack(uint8 *buf,
                                         bfd_sdk_msg_ctrl_discard_stat_get_t *msg);
uint8 *
bfd_sdk_dpp_msg_ctrl_auth_sha1_pack(uint8 *buf,
                                    shr_bfd_msg_ctrl_auth_sha1_t *msg);
uint8 *
bfd_sdk_dpp_msg_ctrl_auth_sha1_unpack(uint8 *buf,
                                      shr_bfd_msg_ctrl_auth_sha1_t *msg);
uint8 *
bfd_sdk_dpp_msg_ctrl_auth_sp_pack(uint8 *buf,
                                  shr_bfd_msg_ctrl_auth_sp_t *msg);
uint8 *
bfd_sdk_dpp_msg_ctrl_auth_sp_unpack(uint8 *buf,
                                    shr_bfd_msg_ctrl_auth_sp_t *msg);

#endif /* BFD_SDK_PACK_H_ */

