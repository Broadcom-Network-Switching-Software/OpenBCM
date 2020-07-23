/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    eth_lm_dm_sdk_msg.h
 */

#ifndef ETH_LM_DM_SDK_MSG_H_
#define ETH_LM_DM_SDK_MSG_H_

#include <soc/shared/eth_lm_dm_msg.h>

/*
 * ETH LM/DM version exchae messages (Request/Reply)
 */

typedef struct eth_lm_dm_sdk_version_exchange_msg_s {
    uint32  version; /* ETH LM/DM SDK version */
}eth_lm_dm_sdk_version_exchange_msg_t;


/*
 * ETH_LM_DM Initialization control message
 */
typedef struct eth_lm_dm_uc_msg_ctrl_init_s {
    uint32  num_sessions;        /* Max number of ETH_LM_DM sessions */
    uint32  rx_channel;          /* Local RX DMA channel (0..3) */
    uint32  data_collection_mode;/* LM/DM data collection mode */
} eth_lm_dm_sdk_msg_ctrl_init_t;


/*
 *  * ETH_LM_DM control messages
 */
typedef union eth_lm_dm_sdk_msg_ctrl_s {
    eth_lm_dm_sdk_msg_ctrl_init_t           init;
    shr_eth_lm_dm_msg_ctrl_sess_set_t       sess_set;
    shr_eth_lm_dm_msg_ctrl_sess_get_t       sess_get;
    shr_eth_lm_dm_msg_ctrl_sess_delete_t    sess_delete;
    shr_eth_lm_dm_msg_ctrl_stat_req_t       stat_req;
    shr_eth_lm_dm_msg_ctrl_stat_reply_t     stat_reply; 
    shr_eth_lm_dm_msg_ctrl_loss_add_t       loss_add;
    shr_eth_lm_dm_msg_ctrl_loss_get_t       loss_get;
    shr_eth_lm_dm_msg_ctrl_loss_delete_t    loss_delete;
    shr_eth_lm_dm_msg_ctrl_delay_add_t      delay_add;
    shr_eth_lm_dm_msg_ctrl_delay_get_t      delay_get;
    shr_eth_lm_dm_msg_ctrl_delay_delete_t   delay_delete;
    eth_lm_dm_sdk_version_exchange_msg_t     eth_lm_dm_ver_msg;
} eth_lm_dm_sdk_msg_ctrl_t;

#endif /* ETH_LM_DM_SDK_MSG_H_ */
