/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    mpls_lm_dm_sdk_msg.h
 */

#ifndef MPLS_LM_DM_SDK_MSG_H_
#define MPLS_LM_DM_SDK_MSG_H_

#include <soc/shared/mpls_lm_dm_msg.h>

/*
 * MPLS_LM_DM Initialization control message
 */
typedef struct mpls_lm_dm_sdk_msg_ctrl_init_s {
    uint32  num_sessions;        /* Max number of MPLS_LM_DM sessions */
    uint32  rx_channel;          /* Local RX DMA channel (0..3) */
    uint32  flags;               /* init flags */
} mpls_lm_dm_sdk_msg_ctrl_init_t;

/*
 * MPLS_LM_DM Loss Measurement add
 */
typedef struct mpls_lm_dm_sdk_msg_ctrl_loss_add_s {
    uint32  flags;
    uint32  sess_id;    /* MPLS_LM_DM session (endpoint) id */
    uint32  period;
    uint32  int_pri;
    uint8   pkt_pri;
    uint16  l2_encap_length; /* L2 Encap Length */
    uint8   l2_encap_data[SHR_MPLS_LM_DM_MAX_ENCAP_LENGTH]; /* L2 Encap Data */
    uint8   olp_encap_data[MPLS_LM_DM_OLP_HDR_LEN]; /* OLP (L2 + OAM) Encap */
    uint8   ctr_size;
    uint32  ctr_base_id[3];
    uint8   ctr_offset[3];
    uint8   ctr_action[3];
    uint8   ctr_byte_offset[3];
    uint32  session_id;
    uint8   session_num_entries;
} mpls_lm_dm_sdk_msg_ctrl_loss_add_t;

/*
 * MPLS_LM_DM Delay Measurement add
 */
typedef struct mpls_lm_dm_sdk_msg_ctrl_delay_add_s {
    uint32  flags;
    uint32  sess_id;    /* MPLS_LM_DM session (endpoint) id */
    uint32  period;
    uint32  int_pri;
    uint8   pkt_pri;
    uint8   dm_format;
    uint16  l2_encap_length; /* L2 Encap Length */
    uint8   l2_encap_data[SHR_MPLS_LM_DM_MAX_ENCAP_LENGTH]; /* L2 Encap Data */
    uint8   olp_encap_data[MPLS_LM_DM_OLP_HDR_LEN]; 
    uint32  session_id;
    uint8   session_num_entries;
} mpls_lm_dm_sdk_msg_ctrl_delay_add_t;

/*
 * MPLS_LM_DM Delay Measurement get
 */
typedef struct mpls_lm_dm_sdk_msg_ctrl_delay_data_s {
    uint32  flags;
    uint32  sess_id;    /* MPLS_LM_DM session (endpoint) id */
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
    uint32  int_pri;
    uint8   pkt_pri;
    uint8   dm_format;
    uint8   olp_tx_hdr[MPLS_LM_DM_OLP_OAM_HDR_LEN];
    uint32  olp_tx_hdr_length;
}mpls_lm_dm_sdk_msg_ctrl_delay_data_t;


/*
 * MPLS_LM_DM control messages
 */
typedef union mpls_lm_dm_sdk_msg_ctrl_s {
    mpls_lm_dm_sdk_msg_ctrl_init_t          init;
    mpls_lm_dm_sdk_msg_ctrl_loss_add_t      loss_add;
    mpls_lm_dm_sdk_msg_ctrl_delay_add_t     delay_add;
    mpls_lm_dm_sdk_msg_ctrl_delay_data_t    delay_get_olp;

    shr_mpls_lm_dm_msg_ctrl_loss_delete_t   loss_del;
    shr_mpls_lm_dm_msg_ctrl_loss_get_t      loss_get;
    shr_mpls_lm_dm_msg_ctrl_loss_data_t     loss_data;
    shr_mpls_lm_dm_msg_ctrl_delay_delete_t  delay_del;
    shr_mpls_lm_dm_msg_ctrl_delay_get_t     delay_get;
    shr_mpls_lm_dm_msg_ctrl_delay_data_t    delay_data;
} mpls_lm_dm_sdk_msg_ctrl_t;

#endif /* MPLS_LM_DM_SDK_MSG_H_ */
