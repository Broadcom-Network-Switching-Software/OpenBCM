/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        eth_lm_dm_pack.h
 * Purpose:     Interface to pack and unpack routines common to
 *              SDK and uKernel for:
 *              - ETH_LM_DM Control messages
 *              - Network Packet headers
 *
 *              This is to be shared between SDK host and uKernel.
 */

#ifndef   _ETH_LM_DM_PACK_H_
#define   _ETH_LM_DM_PACK_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <bcm/types.h>
#endif

#include <soc/shared/eth_lm_dm_msg.h>
#include <soc/shared/eth_lm_dm_pkt.h>

/*
 * TLV Types
 */
#define SHR_ETH_LM_DM_TLV_END            0x00


#define SHR_ETH_LM_DM_ID_ZERO_LEN 24
#define _ETH_LM_DM_MAC_ADDR_LENGTH 6


/* ETH_LM_DM Control Messages pack/unpack */
extern uint8 *
shr_eth_lm_dm_msg_ctrl_init_pack(uint8 *buf, shr_eth_lm_dm_msg_ctrl_init_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_init_unpack(uint8 *buf, shr_eth_lm_dm_msg_ctrl_init_t *msg);

extern uint8 *
shr_eth_lm_dm_msg_ctrl_sess_set_pack(uint8 *buf,
                               shr_eth_lm_dm_msg_ctrl_sess_set_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_sess_set_unpack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_sess_set_t *msg);

extern uint8 *
shr_eth_lm_dm_msg_ctrl_sess_get_pack(uint8 *buf,
                               shr_eth_lm_dm_msg_ctrl_sess_get_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_sess_get_unpack(uint8 *buf,
				 shr_eth_lm_dm_msg_ctrl_sess_get_t *msg); 

extern uint8 *
shr_eth_lm_dm_msg_ctrl_sess_delete_pack(uint8 *buf,
				  shr_eth_lm_dm_msg_ctrl_sess_delete_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_sess_delete_unpack(uint8 *buf,
				    shr_eth_lm_dm_msg_ctrl_sess_delete_t *msg);

extern uint8 *
shr_eth_lm_dm_msg_ctrl_stat_req_pack(uint8 *buf,
                               shr_eth_lm_dm_msg_ctrl_stat_req_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_stat_req_unpack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_stat_req_t *msg);

extern uint8 *
shr_eth_lm_dm_msg_ctrl_stat_reply_pack(uint8 *buf,
                                 shr_eth_lm_dm_msg_ctrl_stat_reply_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_stat_reply_unpack(uint8 *buf,
                                   shr_eth_lm_dm_msg_ctrl_stat_reply_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_loss_add_pack(uint8 *buf,
				   shr_eth_lm_dm_msg_ctrl_loss_add_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_loss_add_unpack(uint8 *buf,
				     shr_eth_lm_dm_msg_ctrl_loss_add_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_loss_delete_pack(uint8 *buf,
				   shr_eth_lm_dm_msg_ctrl_loss_delete_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_loss_delete_unpack(uint8 *buf,
				     shr_eth_lm_dm_msg_ctrl_loss_delete_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_loss_get_pack(uint8 *buf,
				   shr_eth_lm_dm_msg_ctrl_loss_get_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_loss_get_unpack(uint8 *buf,
				     shr_eth_lm_dm_msg_ctrl_loss_get_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_delay_add_pack(uint8 *buf,
				   shr_eth_lm_dm_msg_ctrl_delay_add_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_delay_add_unpack(uint8 *buf,
				     shr_eth_lm_dm_msg_ctrl_delay_add_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_delay_delete_pack(uint8 *buf,
				   shr_eth_lm_dm_msg_ctrl_delay_delete_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_delay_delete_unpack(uint8 *buf,
				     shr_eth_lm_dm_msg_ctrl_delay_delete_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_delay_get_pack(uint8 *buf,
				   shr_eth_lm_dm_msg_ctrl_delay_get_t *msg);
extern uint8 *
shr_eth_lm_dm_msg_ctrl_delay_get_unpack(uint8 *buf,
				     shr_eth_lm_dm_msg_ctrl_delay_get_t *msg);


/* ETH_LM_DM Network Packet headers pack/unpack */
extern uint8 *
shr_eth_lm_dm_header_pack(uint8 *buf, shr_eth_lm_dm_header_t *eth_lm_dm);
extern uint8 *
shr_eth_lm_dm_header_unpack(uint8 *buf, shr_eth_lm_dm_header_t *eth_lm_dm);
extern uint8 *
shr_eth_lm_dm_slm_info_header_pack(uint8 *buf, shr_eth_lm_dm_slm_info_header_t *eth_lm_dm_slm_info);
extern uint8 *
shr_eth_lm_dm_slm_info_header_unpack(uint8 *buf, shr_eth_lm_dm_slm_info_header_t *eth_lm_dm_slm_info);
#endif /* _ETH_LM_DM_PACK_H_ */
