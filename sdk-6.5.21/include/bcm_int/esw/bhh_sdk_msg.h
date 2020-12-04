/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * *
 * File:    bhh_sdk_msg.h
 */

#ifndef BHH_SDK_MSG_H_
#define BHH_SDK_MSG_H_

/*
 * BHH Statistics control messages (Request/Reply)
 */

typedef struct bhh_sdk_version_exchange_msg_s {
    uint32  version; /* BHH SDK or appl version */
}bhh_sdk_version_exchange_msg_t;


typedef struct bhh_sdk_msg_ctrl_sess_get_s {
    uint32   sess_id;
    uint8    enable;
    uint8    passive;
    uint8    local_demand;
    uint8    remote_demand;
    uint8    local_sess_state;
    uint8    remote_sess_state;
    uint8    mel;
    uint16   mep_id;
    uint8    meg_id[SHR_BHH_MEG_ID_LENGTH];
    uint32   local_period;
    uint8    encap_type;
    uint32   encap_length;  /* BHH encapsulation length */
    uint8    encap_data[SHR_BHH_MAX_ENCAP_LENGTH];  /* Encapsulation data */
    uint32   tx_port;
    uint32   tx_cos;
    uint32   tx_pri;
    uint32   tx_qnum;
    uint32   mpls_label;
    uint32   if_num;
    uint32   flags;
    uint16   remote_mep_id;
    uint32   fault_flags;
    uint32   remote_period;
    uint8    priority;
} bhh_sdk_msg_ctrl_sess_get_t;

/*
 *  * BHH Loss Measurement get
 */
typedef struct bhh_sdk_msg_ctrl_loss_get_s {
    uint32  flags;
    uint32  sess_id;    /* BHH session (endpoint) id */
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
    uint32  int_pri;
    uint8   pkt_pri;
    uint16  encap_length;
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];
} bhh_sdk_msg_ctrl_loss_get_t;

/* Flag to indicate the feature that only Final event is given if Timeout and
 * State change are present in BHH event mask */
#define BHH_SDK_MSG_CTRL_INIT_ONLY_FINAL_EVENT 0x00000001

/*
 * BHH Initialization control message
 */
typedef struct bhh_sdk_msg_ctrl_init_s {
    uint32  num_sessions;        /* Max number of BHH sessions */
    uint32  rx_channel;          /* Local RX DMA channel (0..3) */
    uint32  node_id;             /* Node ident */
    uint8   carrier_code[SHR_BHH_CARRIER_CODE_LEN];
    uint32  data_collection_mode;/* LM/DM data collection mode */
    uint16  max_encap_length;      /* Max length of encap */
    uint32   flags; /* Flags that can be used to indicate configurations */
} bhh_sdk_msg_ctrl_init_t;


/*
 * BHH Session enable message
 */
typedef struct bhh_sdk_msg_ctrl_rmep_create_s {
    uint32  sess_id;
    uint32  flags;
    uint16  remote_mep_id;
    uint8   enable;
    uint32  period;
} bhh_sdk_msg_ctrl_rmep_create_t;

/*
 * BHH Loss Measurement add
 */
typedef struct bhh_sdk_msg_ctrl_loss_add_s {
    uint32  flags;
    uint32  sess_id;    /* BHH session (endpoint) id */
    uint32  period;
    uint32  int_pri;
    uint8   pkt_pri;
    uint16  encap_length;
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];
} bhh_sdk_msg_ctrl_loss_add_t;

/*
 * BHH Delay Measurement add
 */
typedef struct bhh_sdk_msg_ctrl_delay_add_s {
    uint32  flags;
    uint32  sess_id;    /* BHH session (endpoint) id */
    uint32  period;
    uint32  int_pri;
    uint8   pkt_pri;
    uint8   dm_format;
    uint16  encap_length;
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];
    uint16  data_tlv_len; /*Because of memory constraint in FW specific data pattern 
                            is not supported in Data tlv value*/
} bhh_sdk_msg_ctrl_delay_add_t;

/*
 * BHH Loopback add
 */
typedef struct bhh_sdk_msg_ctrl_loopback_add_s {
    uint32  flags;
    uint32  sess_id;    /* BHH session (endpoint) id */
    uint32  period;
    uint32  ttl;
    uint32  int_pri;
    uint16  encap_length;
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];
    uint8   pkt_pri;
} bhh_sdk_msg_ctrl_loopback_add_t;

typedef struct bhh_sdk_msg_ctrl_sess_set_s {
    uint32  sess_id;
    uint32  flags;
    uint8   passive;
    uint8   mel;    /* MEG level */
    uint16  mep_id;
    uint8   meg_id[SHR_BHH_MEG_ID_LENGTH];
    uint32  period;
    uint8   encap_type;    /* Raw, UDP-IPv4/IPv6, used for UDP checksum */
    uint32  encap_length;  /* BHH encapsulation length */
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];  /* Encapsulation data */
    uint32  tx_port;
    uint32  tx_cos;
    uint32  tx_pri;
    uint32  tx_qnum;
    uint32  mpls_label;
    uint32  if_num;
    uint32  lm_counter_index;
    uint16  inner_vlan;
    uint16  outer_vlan;
    uint8   endpoint_type;
    uint8   num_session_entries;
    uint8   priority;
    uint16  rx_port;
    uint8   trunk_valid;
} bhh_sdk_msg_ctrl_sess_set_t;

/*
 * BHH Session Update
 */
typedef struct bhh_sdk_msg_session_update_s {
    uint32  flags;
    uint32  sess_id;
    uint8   msg_type;
    uint8   rdi;
    uint16  encap_length;
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];
} bhh_sdk_msg_session_update_t;

/*
 * BHH Delay Measurement get
 */
typedef struct bhh_sdk_msg_ctrl_delay_get_s {
    uint32  flags;
    uint32  sess_id;    /* BHH session (endpoint) id */
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
    uint16  encap_length;
    uint16  data_tlv_len;
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];
} bhh_sdk_msg_ctrl_delay_get_t;

/*
 * BHH Loopback get
 */
typedef struct bhh_sdk_msg_ctrl_loopback_get_s {
    uint32  flags;
    uint32  sess_id;    /* BHH session (endpoint) id */
    uint32  period;
    uint32  ttl;
    uint32  discovery_flags;
    uint32  discovery_id;
    uint32  discovery_ttl;
    uint32  rx_count;
    uint32  tx_count;
    uint32  drop_count;
    uint32  unexpected_response;
    uint32  out_of_sequence;
    uint32  local_mipid_missmatch;
    uint32  remote_mipid_missmatch;
    uint32  invalid_target_mep_tlv;
    uint32  invalid_mep_tlv_subtype;
    uint32  invalid_tlv_offset;
    uint32  int_pri;
    uint16  encap_length;
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];
    uint8   pkt_pri;
} bhh_sdk_msg_ctrl_loopback_get_t;

/*
 * BHH Trunk Map update
 */
typedef struct bhh_uc_msg_ctrl_trunk_map_update_s {
    uint16 num_ports;
    uint16 trunk_id;
    uint16 port_list[SHR_BHH_MAX_PORTS];
} bhh_sdk_msg_ctrl_trunk_map_update_t;

/*
 * BHH Trunk Map get
 */
typedef struct bhh_uc_msg_ctrl_trunk_map_get_s {
    uint16 num_ports;
    uint16 trunk_id;
    uint16 port_list[SHR_BHH_MAX_PORTS];
} bhh_sdk_msg_ctrl_trunk_map_get_t;

/*
 *  BHH control messages
 */
typedef union bhh_sdk_msg_ctrl_s {
    bhh_sdk_msg_ctrl_init_t             init;
    bhh_sdk_msg_ctrl_sess_set_t         sess_set;
    bhh_sdk_msg_ctrl_sess_get_t         sess_get;
    shr_bhh_msg_ctrl_sess_delete_t      sess_delete;
    shr_bhh_msg_ctrl_stat_req_t         stat_req;
    shr_bhh_msg_ctrl_stat_reply_t       stat_reply;
    bhh_sdk_msg_ctrl_loopback_add_t     loopback_add;
    bhh_sdk_msg_ctrl_loopback_get_t     loopback_get;
    shr_bhh_msg_ctrl_pm_stats_get_t     pm_stats_get;
    bhh_sdk_version_exchange_msg_t      bhh_ver_msg;
    bhh_sdk_msg_ctrl_loss_add_t         loss_add;
    bhh_sdk_msg_ctrl_loss_get_t         loss_get;
    bhh_sdk_msg_ctrl_delay_add_t        delay_add;
    bhh_sdk_msg_ctrl_delay_get_t        delay_get;
    bhh_sdk_msg_ctrl_trunk_map_update_t trunk_update;
    bhh_sdk_msg_ctrl_trunk_map_get_t    trunk_get;
} bhh_sdk_msg_ctrl_t;

/*
 * BHH CSF add message
 */
typedef struct bhh_sdk_msg_ctrl_csf_add_s {
    uint16  sess_id;
    uint32  flags;
    uint8   type;
    uint32  period;
    uint8   int_pri;
    uint8   pkt_pri;
    uint16  encap_length;
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];
} bhh_sdk_msg_ctrl_csf_add_t;

/*
 * BHH CSF Measurement get
 */
typedef struct bhh_sdk_msg_ctrl_csf_get_s {
    uint32  flags;
    uint32  type;
    uint32  period;
    uint32  int_pri;
    uint8   pkt_pri;
    uint16  encap_length;
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];
    uint32  tx_csf_packets;
} bhh_sdk_msg_ctrl_csf_get_t;

#endif /* BHH_SDK_MSG_H_ */

