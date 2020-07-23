/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    mpls_lm_dm_sdk_pack.h
 */

#ifndef _MPLS_LM_DM_SDK_PACK_H_
#define _MPLS_LM_DM_SDK_PACK_H_

#if defined(INCLUDE_MPLS_LM_DM)

#include <bcm_int/esw/mpls_lm_dm_sdk_msg.h>


uint8* mpls_lm_dm_sdk_msg_ctrl_init_pack(uint8                          *buf,
                                         mpls_lm_dm_sdk_msg_ctrl_init_t *msg);

uint8 *
mpls_lm_dm_sdk_msg_ctrl_loss_add_pack(uint8 *buf,
                                      mpls_lm_dm_sdk_msg_ctrl_loss_add_t *msg);
uint8 *
mpls_lm_dm_sdk_msg_ctrl_delay_add_pack(uint8 *buf,
                                       mpls_lm_dm_sdk_msg_ctrl_delay_add_t *msg);

uint8 *
mpls_lm_dm_sdk_msg_ctrl_delay_data_unpack(uint8 *buf,
        mpls_lm_dm_sdk_msg_ctrl_delay_data_t *msg);

#endif /* INCLUDE_MPLS_LM_DM */
#endif /* _MPLS_LM_DM_SDK_PACK_H_ */
