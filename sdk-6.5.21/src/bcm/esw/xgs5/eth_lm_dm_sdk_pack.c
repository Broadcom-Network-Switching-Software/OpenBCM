/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    eth_lm_dm_sdk_pack.c
 */

#if defined(INCLUDE_ETH_LM_DM)
#include <bcm_int/esw/eth_lm_dm_sdk_pack.h>

/* eth_lm_dm_sdk_ctrl_version_msg_pack
 * @brief
 * Pack the eth_lm_dm sdk version
 */
uint8 *
eth_lm_dm_sdk_version_exchange_msg_pack(uint8 *buf, eth_lm_dm_sdk_version_exchange_msg_t *msg)
{
    _SHR_PACK_U32(buf, msg->version);
    return buf;
}


/* eth_lm_dm_sdk_ctrl_version_msg_unpack
 * @brief
 * UnPack the eth_lm_dm SDK version
 */
uint8 *
eth_lm_dm_sdk_version_exchange_msg_unpack(uint8 *buf, eth_lm_dm_sdk_version_exchange_msg_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->version);
    return buf;
}

uint8 *
eth_lm_dm_sdk_msg_ctrl_init_pack(uint8 *buf, eth_lm_dm_sdk_msg_ctrl_init_t *msg)
{
    _SHR_PACK_U32(buf, msg->num_sessions);
    _SHR_PACK_U32(buf, msg->rx_channel);
    if(ETH_LM_DM_UC_FEATURE_CHECK(ETH_LM_DM_PM)) {
        _SHR_PACK_U32(buf, msg->data_collection_mode);
    }
    return buf;
}
#else /* INCLUDE_ETH_LM_DM */
typedef int bcm_xgs5_eth_lm_dm_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_ETH_LM_DM */
