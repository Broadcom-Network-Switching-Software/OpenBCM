/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ifa2_pack.h
 * Purpose:     IFAv2.0 message packing functions.
 */

#ifndef _BCM_INT_IFA2_PACK_H_
#define _BCM_INT_IFA2_PACK_H_

#include <soc/shared/ifa2_msg.h>

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_init_pack(uint8 *buf,
                                  shr_ifa2_msg_ctrl_init_t *msg);

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_md_format_set_pack(uint8 *buf,
                                           shr_ifa2_msg_ctrl_md_format_set_t *msg);

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_config_update_pack(uint8 *buf,
                                           shr_ifa2_msg_ctrl_config_update_t *msg);

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_collector_create_pack(uint8 *buf,
                                              shr_ifa2_msg_ctrl_collector_create_t *msg);

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_stats_set_pack(uint8 *buf,
                                       shr_ifa2_msg_ctrl_stats_set_t *msg);

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_stats_get_unpack(uint8 *buf,
                                         shr_ifa2_msg_ctrl_stats_get_t *msg);

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_template_pack(uint8 *buf,
                                      shr_ifa2_msg_export_template_info_t *msg);

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_template_unpack(uint8 *buf,
                                        shr_ifa2_msg_export_template_info_t *msg);

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_template_xmit_pack(uint8 *buf,
                                           shr_ifa2_msg_ctrl_template_xmit_t *msg);

uint8 *
_bcm_xgs5_ifa2_msg_ctrl_template_xmit_unpack(uint8 *buf,
                                             shr_ifa2_msg_ctrl_template_xmit_t *msg);
#endif /* _BCM_INT_IFA2_PACK_H_ */
