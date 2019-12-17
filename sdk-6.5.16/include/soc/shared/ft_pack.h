/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:    ft_pack.h
 */

#ifndef SHR_FT_PACK_H_
#define SHR_FT_PACK_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif
#include <soc/shared/ft_msg.h>

uint8*
shr_ft_msg_ctrl_init_pack(uint8 *buf, shr_ft_msg_ctrl_init_t *msg);

uint8*
shr_ft_msg_ctrl_init_unpack(uint8 *buf, shr_ft_msg_ctrl_init_t *msg);

uint8*
shr_ft_msg_ctrl_em_key_format_pack(uint8 *buf,
                                   shr_ft_msg_ctrl_em_key_format_t *msg);

uint8*
shr_ft_msg_ctrl_em_key_format_unpack(uint8 *buf,
                                     shr_ft_msg_ctrl_em_key_format_t *msg);

uint8*
shr_ft_msg_ctrl_em_group_create_pack(uint8 *buf,
                                     shr_ft_msg_ctrl_em_group_create_t *msg);

uint8*
shr_ft_msg_ctrl_em_group_create_unpack(uint8 *buf,
                                       shr_ft_msg_ctrl_em_group_create_t *msg);

uint8*
shr_ft_msg_ctrl_group_create_pack(uint8 *buf,
                                  shr_ft_msg_ctrl_group_create_t *msg);

uint8*
shr_ft_msg_ctrl_group_create_unpack(uint8 *buf,
                                    shr_ft_msg_ctrl_group_create_t *msg);

uint8*
shr_ft_msg_ctrl_group_get_pack(uint8* buf, shr_ft_msg_ctrl_group_get_t *msg);

uint8*
shr_ft_msg_ctrl_group_get_unpack(uint8* buf, shr_ft_msg_ctrl_group_get_t *msg);

uint8*
shr_ft_msg_ctrl_group_update_pack(uint8* buf,
                                  shr_ft_msg_ctrl_group_update_t *msg);

uint8*
shr_ft_msg_ctrl_group_update_unpack(uint8* buf,
                                    shr_ft_msg_ctrl_group_update_t *msg);

uint8*
shr_ft_msg_ctrl_template_create_pack(uint8* buf,
                                     shr_ft_msg_ctrl_template_create_t *msg);

uint8*
shr_ft_msg_ctrl_template_create_unpack(uint8* buf,
                                       shr_ft_msg_ctrl_template_create_t *msg);

uint8*
shr_ft_msg_ctrl_template_get_pack(uint8* buf,
                                  shr_ft_msg_ctrl_template_get_t *msg);

uint8*
shr_ft_msg_ctrl_template_get_unpack(uint8* buf,
                                    shr_ft_msg_ctrl_template_get_t *msg);

uint8*
shr_ft_msg_ctrl_collector_create_pack(uint8* buf,
                                      shr_ft_msg_ctrl_collector_create_t *msg);

uint8*
shr_ft_msg_ctrl_collector_create_unpack(uint8* buf,
                                        shr_ft_msg_ctrl_collector_create_t *msg);

uint8*
shr_ft_msg_ctrl_collector_get_pack(uint8* buf,
                                   shr_ft_msg_ctrl_collector_get_t *msg);

uint8*
shr_ft_msg_ctrl_collector_get_unpack(uint8* buf,
                                     shr_ft_msg_ctrl_collector_get_t *msg);

uint8*
shr_ft_msg_ctrl_group_flow_data_get_pack(uint8* buf,
                                         shr_ft_msg_ctrl_group_flow_data_get_t *msg);

uint8*
shr_ft_msg_ctrl_group_flow_data_get_unpack(
                                         uint8* buf,
                                         shr_ft_msg_ctrl_group_flow_data_get_t *msg);

uint8*
shr_ft_msg_ctrl_group_flow_data_get_reply_pack(
                                   uint8* buf,
                                   shr_ft_msg_ctrl_group_flow_data_get_reply_t *msg);
uint8*
shr_ft_msg_ctrl_group_flow_data_get_reply_unpack(
                                    uint8* buf,
                                    shr_ft_msg_ctrl_group_flow_data_get_reply_t *msg);
uint8*
shr_ft_msg_ctrl_ser_event_pack(uint8* buf, shr_ft_msg_ctrl_ser_event_t *msg);

uint8*
shr_ft_msg_ctrl_ser_event_unpack(uint8* buf, shr_ft_msg_ctrl_ser_event_t *msg);

uint8*
shr_ft_msg_ctrl_template_xmit_pack(uint8* buf,
                                   shr_ft_msg_ctrl_template_xmit_t *msg);

uint8*
shr_ft_msg_ctrl_template_xmit_unpack(uint8* buf,
                                     shr_ft_msg_ctrl_template_xmit_t *msg);

uint8*
shr_ft_msg_ctrl_elph_profile_create_pack(uint8* buf,
                                      shr_ft_msg_ctrl_elph_profile_create_t *msg);

uint8*
shr_ft_msg_ctrl_elph_profile_create_unpack(uint8* buf,
                                        shr_ft_msg_ctrl_elph_profile_create_t *msg);

uint8*
shr_ft_msg_ctrl_elph_profile_get_pack(uint8* buf,
                                      shr_ft_msg_ctrl_elph_profile_get_t *msg);

uint8*
shr_ft_msg_ctrl_elph_profile_get_unpack(uint8* buf,
                                        shr_ft_msg_ctrl_elph_profile_get_t *msg);

uint8*
shr_ft_msg_ctrl_stats_learn_get_pack(uint8* buf,
                                     shr_ft_msg_ctrl_stats_learn_t *msg);

uint8*
shr_ft_msg_ctrl_stats_learn_get_unpack(uint8* buf,
                                       shr_ft_msg_ctrl_stats_learn_t *msg);

uint8*
shr_ft_msg_ctrl_stats_export_get_pack(uint8* buf,
                                      shr_ft_msg_ctrl_stats_export_t *msg);

uint8*
shr_ft_msg_ctrl_stats_export_get_unpack(uint8* buf,
                                        shr_ft_msg_ctrl_stats_export_t *msg);

uint8*
shr_ft_msg_ctrl_stats_age_get_pack(uint8* buf,
                                   shr_ft_msg_ctrl_stats_age_t *msg);

uint8*
shr_ft_msg_ctrl_stats_age_get_unpack(uint8* buf,
                                     shr_ft_msg_ctrl_stats_age_t *msg);

uint8*
shr_ft_msg_ctrl_stats_elph_get_pack(uint8* buf,
                                    shr_ft_msg_ctrl_stats_elph_t *msg);

uint8*
shr_ft_msg_ctrl_stats_elph_get_unpack(uint8* buf,
                                      shr_ft_msg_ctrl_stats_elph_t *msg);

uint8*
shr_ft_msg_ctrl_sdk_features_pack(uint8* buf,
                                  shr_ft_msg_ctrl_sdk_features_t *msg);

uint8*
shr_ft_msg_ctrl_sdk_features_unpack(uint8* buf,
                                    shr_ft_msg_ctrl_sdk_features_t *msg);

uint8*
shr_ft_msg_ctrl_uc_features_pack(uint8* buf,
                                 shr_ft_msg_ctrl_uc_features_t *msg);

uint8*
shr_ft_msg_ctrl_uc_features_unpack(uint8* buf,
                                   shr_ft_msg_ctrl_uc_features_t *msg);

uint8*
shr_ft_msg_ctrl_group_actions_set_pack(uint8* buf,
                                       shr_ft_msg_ctrl_group_actions_set_t *msg);

uint8*
shr_ft_msg_ctrl_group_actions_set_unpack(uint8* buf,
                                         shr_ft_msg_ctrl_group_actions_set_t *msg);

#endif /* SHR_FT_PACK_H_ */
