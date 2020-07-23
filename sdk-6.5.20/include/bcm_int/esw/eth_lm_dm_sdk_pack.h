/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    eth_lm_dm_sdk_pack.h
 */

#ifndef ETH_LM_DM_SDK_PACK_H_
#define ETH_LM_DM_SDK_PACK_H_

#include <shared/pack.h>

#include <soc/shared/eth_lm_dm_msg.h>
#include <bcm_int/esw/eth_lm_dm_sdk_msg.h>
#include <bcm_int/esw/eth_lm_dm_feature.h>

uint8 * eth_lm_dm_sdk_version_exchange_msg_unpack(uint8 *buf,
                                             eth_lm_dm_sdk_version_exchange_msg_t *msg);
uint8 * eth_lm_dm_sdk_version_exchange_msg_pack(uint8 *buf,
                                           eth_lm_dm_sdk_version_exchange_msg_t *msg);
uint8 * eth_lm_dm_sdk_msg_ctrl_init_pack(uint8 *buf,
                                        eth_lm_dm_sdk_msg_ctrl_init_t *msg);

#endif /* ETH_LM_DM_SDK_PACK_H_ */
