/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    eth_lm_dm_msg.h
 * Purpose: ETH_LM_DM Messages definitions common to SDK and uKernel.
 *
 *          Messages between SDK and uKernel.
 */

#ifndef   _ETH_LM_DM_MSG_H_
#define   _ETH_LM_DM_MSG_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <bcm/types.h>
#endif

#include <soc/defs.h>
#include <soc/shared/olp_pkt.h>
#include <soc/shared/mos_msg_common.h>


/********************************
 * ETH_LM_DM Control Messages
 *
 *   SDK Host <--> uController
 */


/*
 * ETH_LM_DM Initialization control message
 */
typedef struct shr_eth_lm_dm_msg_ctrl_init_s {
    uint32  num_sessions;        /* Max number of ETH_LM_DM sessions */
    uint32  rx_channel;          /* Local RX DMA channel (0..3) */
} shr_eth_lm_dm_msg_ctrl_init_t;

/*
 * ETH_LM_DM Encapsulation control message
 *
 * Encapsulation Maximum length 
 * L2 + ETH_LM_DM
 *
 *
 * ETH_LM_DM Encapsulation Table
 *
 * The ETH_LM_DM Encapsulation is maintained in the uKernel
 * It contains the encapsulation for all current ETH_LM_DM sessions.
 *
 * A ETH_LM_DM encapsulation currently includes:
 *     L2
 *     [MPLS-TP]
 *
 * A ETH_LM_DM encapsulation does NOT include the ETH_LM_DM PDU (ETH_LM_DM Control Packet).
 * This is built by the uKernel side.
 */
#define SHR_ETH_LM_DM_MAX_ENCAP_LENGTH    162
#define SHR_ETH_LM_DM_OLP_TX_HDR_LENGTH   sizeof(soc_olp_tx_hdr_t)
#define SHR_ETH_LM_DM_OLP_L2_HDR_LENGTH   sizeof(soc_olp_l2_hdr_t)
#define SHR_ETH_LM_DM_MEG_ID_LENGTH       48


/*
 * ETH_LM_DM Session delete control message
 */
typedef struct shr_eth_lm_dm_msg_ctrl_sess_delete_s {
    uint32  sess_id;
} shr_eth_lm_dm_msg_ctrl_sess_delete_t;


/*
 * ETH_LM_DM Session Set control message
 */
typedef struct shr_eth_lm_dm_msg_ctrl_sess_set_s {
    uint32  sess_id;
    uint32  flags;
    uint8   passive;
    uint8   mel;    /* MEG level */
    uint16  mep_id;
    uint32  period;
    uint32  eth_l2_encap_length;  /* ETH_L2 encapsulation length */
    uint8	encap_data[SHR_ETH_LM_DM_MAX_ENCAP_LENGTH]; /* OLP and ETH L2 encap */
    int32   tx_port;
    uint32  lm_counter_index;
    uint16	vlan_id;
    uint16 	outer_vlan_id;
    uint8	 mep_type;
    uint16	group_id;

} shr_eth_lm_dm_msg_ctrl_sess_set_t;


/*
 * ETH_LM_DM Session Get control message
 */
typedef struct shr_eth_lm_dm_msg_ctrl_sess_get_s {
    uint32   sess_id;
    uint8    enable;
    uint8    passive;
    uint8    mel;
    uint16   mep_id;
    uint32   period;
    uint32   eth_l2_encap_length;  /* ETH_L2 encapsulation length */
    uint8	 encap_data[SHR_ETH_LM_DM_MAX_ENCAP_LENGTH]; /* OLP and ETH L2 encap */
    uint32   tx_port;
    uint32   lm_counter_index;
    uint16	 vlan_id;
    uint16 	 outer_vlan_id;
    uint8	 mep_type;
    uint16	 group_id;

} shr_eth_lm_dm_msg_ctrl_sess_get_t;


/*
 * ETH_LM_DM Statistics control messages (Request/Reply)
 */
typedef struct shr_eth_lm_dm_msg_ctrl_stat_req_s {
    uint32  sess_id;    /* ETH_LM_DM session (endpoint) id */
    uint32  clear;      /* Clear stats */
} shr_eth_lm_dm_msg_ctrl_stat_req_t;

typedef struct shr_eth_lm_dm_msg_ctrl_stat_reply_s {
    uint32  sess_id;           /* ETH_LM_DM session (endpoint) id */
    uint32  packets_in;        /* Total packets in */
    uint32  packets_out;       /* Total packets out */
    uint32  packets_drop;      /* Total packets drop */
} shr_eth_lm_dm_msg_ctrl_stat_reply_t;

/*
 * ETH_LM_DM Loss Measurement add
 */
typedef struct shr_eth_lm_dm_msg_ctrl_loss_add_s {
    uint32  flags;
    uint32  sess_id;    /* ETH_LM_DM session (endpoint) id */
    uint32  period;
    uint32  tx_cos;
    uint8   tx_pri;
    uint32  tx_qnum;
    bcm_mac_t dst_mac;
    uint8   olp_tx_hdr[SHR_ETH_LM_DM_OLP_TX_HDR_LENGTH];
    uint32  olp_tx_hdr_length;
    uint32  remote_mep_id;
} shr_eth_lm_dm_msg_ctrl_loss_add_t;

/*
 * ETH_LM_DM Loss Measurement delete
 */
typedef struct shr_eth_lm_dm_msg_ctrl_loss_delete_s {
    uint32  sess_id;    /* ETH_LM_DM session (endpoint) id */
} shr_eth_lm_dm_msg_ctrl_loss_delete_t;

/*
 * ETH_LM_DM Loss Measurement get
 */
typedef struct shr_eth_lm_dm_msg_ctrl_loss_get_s {
    uint32  flags;
    uint32  sess_id;    /* ETH_LM_DM session (endpoint) id */
    uint32  period;
    uint32  loss_threshold;             
    uint32  loss_nearend;               
    uint32  loss_farend;    
    uint32  tx_nearend;            
    uint32  rx_nearend;              
    uint32  tx_farend;               
    uint32  rx_farend;               
    uint32  rx_oam_packets;          
    uint32  tx_oam_packets;          
    uint32  tx_cos;
    uint8   tx_pri;
    uint32  tx_qnum;
    bcm_mac_t dst_mac;
    uint8   olp_tx_hdr[SHR_ETH_LM_DM_OLP_TX_HDR_LENGTH];
    uint32  olp_tx_hdr_length;
    uint32  remote_mep_id;
} shr_eth_lm_dm_msg_ctrl_loss_get_t;

/*
 * ETH_LM_DM Delay Measurement add
 */
typedef struct shr_eth_lm_dm_msg_ctrl_delay_add_s {
    uint32  flags;
    uint32  sess_id;    /* ETH_LM_DM session (endpoint) id */
    uint32  period;
    uint32  tx_cos;    
    uint8   tx_pri;
    uint32  tx_qnum;
    uint8   dm_format;
    bcm_mac_t dst_mac;
    uint8   olp_tx_hdr[SHR_ETH_LM_DM_OLP_TX_HDR_LENGTH];
    uint32  olp_tx_hdr_length;
    uint32  remote_mep_id;
} shr_eth_lm_dm_msg_ctrl_delay_add_t;

/*
 * ETH_LM_DM Delay Measurement delete
 */
typedef struct shr_eth_lm_dm_msg_ctrl_delay_delete_s {
    uint32  sess_id;    /* ETH_LM_DM session (endpoint) id */
} shr_eth_lm_dm_msg_ctrl_delay_delete_t;

/*
 * ETH_LM_DM Delay Measurement get
 */
typedef struct shr_eth_lm_dm_msg_ctrl_delay_get_s {
    uint32  flags;
    uint32  sess_id;    /* ETH_LM_DM session (endpoint) id */
    uint32  period;
    uint32  delay_seconds;
    uint32  delay_nanoseconds;
    uint32  txf_seconds;  
    uint32  txf_nanoseconds;
    uint32  rxf_seconds;
    uint32  rxf_nanoseconds;
    uint32  txb_seconds;
    uint32  txb_nanoseconds;
    uint32  rxb_seconds;
    uint32  rxb_nanoseconds;
    uint32  rx_oam_packets;
    uint32  tx_oam_packets; 
    uint32  tx_cos;    
    uint8   tx_pri;
    uint32  tx_qnum;
    uint8   dm_format;
    bcm_mac_t dst_mac;
    uint8   olp_tx_hdr[SHR_ETH_LM_DM_OLP_TX_HDR_LENGTH];
    uint32  olp_tx_hdr_length;
    uint32  remote_mep_id;
} shr_eth_lm_dm_msg_ctrl_delay_get_t;


/*
 * ETH_LM_DM control messages
 */
typedef union shr_eth_lm_dm_msg_ctrl_s {
    shr_eth_lm_dm_msg_ctrl_init_t         	init;
    shr_eth_lm_dm_msg_ctrl_sess_set_t     	sess_set;
    shr_eth_lm_dm_msg_ctrl_sess_get_t     	sess_get;
    shr_eth_lm_dm_msg_ctrl_sess_delete_t  	sess_delete;
    shr_eth_lm_dm_msg_ctrl_stat_req_t     	stat_req;
    shr_eth_lm_dm_msg_ctrl_stat_reply_t   	stat_reply;	
    shr_eth_lm_dm_msg_ctrl_loss_add_t		loss_add;
    shr_eth_lm_dm_msg_ctrl_loss_get_t 		loss_get;
    shr_eth_lm_dm_msg_ctrl_loss_delete_t 	loss_delete;
    shr_eth_lm_dm_msg_ctrl_delay_add_t 		delay_add;
    shr_eth_lm_dm_msg_ctrl_delay_get_t 		delay_get;
    shr_eth_lm_dm_msg_ctrl_delay_delete_t 	delay_delete;
} shr_eth_lm_dm_msg_ctrl_t;


/****************************************
 * ETH_LM_DM event message
 */
#define ETH_LM_DM_BTE_EVENT_PM_STATS_COUNTER_ROLLOVER   0x000001

#endif /* _ETH_LM_DM_MSG_H_ */
