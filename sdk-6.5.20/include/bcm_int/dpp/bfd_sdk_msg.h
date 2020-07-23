/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * *
 * File:    bfd_sdk_msg.h
 */

#ifndef BFD_SDK_MSG_H_
#define BFD_SDK_MSG_H_

#include <soc/shared/bfd_msg.h>



/*
 * BFD Initialization control message
 */
typedef struct bfd_sdk_msg_ctrl_init_s {
    uint32  num_sessions;        /* Max number of BFD sessions */
    uint32  encap_size;          /* Total encapsulation table size */
    uint32  num_auth_sha1_keys;  /* Max number of sha1 auth keys */
    uint32  num_auth_sp_keys;    /* Max number of simple pwd auth keys */
    uint32  rx_channel;          /* Local RX DMA channel (0..3) */
    uint32  config_flags;        /* Flags representing various config
                                    properties on SDK side */
    uint32  num_punt_buffers;    /* Number of punt buffers */
    /* Protection Packet Feature */
    uint32 protection_packet_enable; /* BFD IPV6 Protection packet feature enable */
    uint32 protection_pkt_event_fifo_size; /* Indicates internal event FIFO threshold size */
    uint32 protection_pkt_event_fifo_timer; /* Indicates maximum wait timer to read internal event FIFO */

} bfd_sdk_msg_ctrl_init_t;

/*
 * BFD Session Set control message
 */
typedef struct bfd_sdk_msg_ctrl_sess_set_s {
    uint32  sess_id;
    uint32  flags;
    uint8   passive;
    uint8   local_demand;
    uint8   local_diag;
    uint8   local_detect_mult;
    uint32  local_discriminator;
    uint32  remote_discriminator;
    uint32  local_min_tx;
    uint32  local_min_rx;
    uint32  local_min_echo_rx;
    uint8   auth_type;
    uint32  auth_key;
    uint32  xmt_auth_seq;
    uint8   encap_type;    /* Raw, UDP-IPv4/IPv6, used for UDP checksum */
    uint32  encap_length;  /* BFD encapsulation length */
    uint8   encap_data[SHR_BFD_MAX_ENCAP_LENGTH];  /* Encapsulation data */
    uint16  lkey_etype;    /* Lookup key Ether Type */
    uint16  lkey_offset;   /* Lookup key offset */
    uint16  lkey_length;   /* Lookup key length */
    uint32  mep_id_length; /* MPLS-TP CV Source MEP-ID TLV length */
    uint8   mep_id[_SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CV
                                                            Source MEP-ID */
    uint8   mpls_label[SHR_BFD_MPLS_LABEL_LENGTH]; /* Incoming inner MPLS
                                                      label packet format */
    uint32  tx_port;
    uint32  tx_cos;
    uint32  tx_pri;
    uint32  tx_qnum;
    uint8   local_echo;
#ifdef SDK_45223_DISABLED
    uint32  remote_mep_id_length;
    uint8   remote_mep_id[_SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CV
                                                                 Remote MEP-ID */
#endif
    uint8   sampling_ratio;  /* 0 - No packets sampled to the CPU.
                               1-N - Count of packets (with events)
                               that need to arrive before one is
                               sampled to the CPU. */
    uint32   actual_tx_interval; /* SSO update : Actual tx_interval */
    uint8    remote_detect_mult;
} bfd_sdk_msg_ctrl_sess_set_t;

/*
 * BFD Session Get control message
 */
typedef struct bfd_sdk_msg_ctrl_sess_get_s {
    uint32   sess_id;
    uint8    enable;
    uint8    passive;
    uint8    poll;
    uint8    local_demand;
    uint8    remote_demand;
    uint8    local_diag;
    uint8    remote_diag;
    uint8    local_sess_state;
    uint8    remote_sess_state;
    uint8    local_detect_mult;
    uint8    remote_detect_mult;
    uint32   local_discriminator;
    uint32   remote_discriminator;
    uint32   local_min_tx;
    uint32   remote_min_tx;
    uint32   local_min_rx;
    uint32   remote_min_rx;
    uint32   local_min_echo_rx;
    uint32   remote_min_echo_rx;
    uint8    auth_type;
    uint32   auth_key;
    uint32   xmt_auth_seq;
    uint32   rcv_auth_seq;
    uint8    encap_type;    /* Raw, UDP-IPv4/IPv6, used for UDP checksum */
    uint32   encap_length;  /* BFD encapsulation length */
    uint8    encap_data[SHR_BFD_MAX_ENCAP_LENGTH];  /* Encapsulation data */
    uint16   lkey_etype;    /* Lookup key Ether Type */
    uint16   lkey_offset;   /* Lookup key offset */
    uint16   lkey_length;   /* Lookup key length */
    uint32   mep_id_length; /* MPLS-TP CV Source MEP-ID TLV length */
    uint8    mep_id[_SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CV
                                                             Source MEP-ID */
    uint8    mpls_label[SHR_BFD_MPLS_LABEL_LENGTH]; /* Incoming inner MPLS
                                                       label packet format */
    uint32   tx_port;
    uint32   tx_cos;
    uint32   tx_pri;
    uint32   tx_qnum;
    uint8   local_echo;
#ifdef SDK_45223_DISABLED
    uint32   remote_mep_id_length;
    uint8    remote_mep_id[_SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CV
                                                                 Remote MEP-ID */
    uint32   mis_conn_mep_id_length;
    uint8    mis_conn_mep_id[_SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CV
                                                       Mis connectivity MEP-ID */
#endif
    uint8   sampling_ratio;  /* 0 - No packets sampled to the CPU.
                               1-N - Count of packets (with events)
                               that need to arrive before one is
                               sampled to the CPU. */
    uint32  actual_tx_interval;          /* Actual_tx_inteval : SSO update */
} bfd_sdk_msg_ctrl_sess_get_t;

typedef struct bfd_sdk_msg_ctrl_trace_log_enable_s {
    uint32 addr_hi;
    uint32 addr_lo;
} bfd_sdk_msg_ctrl_trace_log_enable_t;

typedef struct bfd_sdk_msg_ctrl_stat_reply_s {
    uint32  sess_id;           /* BFD session (endpoint) id */
    uint32  packets_in;        /* Total packets in */
    uint32  packets_out;       /* Total packets out */
    uint32  packets_drop;      /* Total packets drop */
    uint32  packets_auth_drop; /* Packets drop due to authentication failure */
    uint32  packets_echo_reply; /*Total number of echo reply packets processed*/
    uint32  packets_echo_reply_hi;
} bfd_sdk_msg_ctrl_stat_reply_t;

/*
 * BFD discard stat get control message
 */
typedef struct bfd_sdk_msg_ctrl_discard_stat_get_s {
    uint32 bfd_ver_err;
    uint32 bfd_len_err;
    uint32 bfd_detect_mult;
    uint32 bfd_my_disc;
    uint32 bfd_p_f_bit;
    uint32 bfd_m_bit;
    uint32 bfd_auth_type_mismatch;
    uint32 bfd_auth_simple_err;
    uint32 bfd_auth_m_sha1_err;
    uint32 bfd_sess_mismatch;
} bfd_sdk_msg_ctrl_discard_stat_get_t;

/*
 * BFD control messages
 */
typedef union shr_bfd_msg_ctrl_s {
    bfd_sdk_msg_ctrl_init_t         init;
    bfd_sdk_msg_ctrl_sess_set_t     sess_set;
    bfd_sdk_msg_ctrl_sess_get_t     sess_get;
    shr_bfd_msg_ctrl_auth_sp_t      auth_sp;
    shr_bfd_msg_ctrl_auth_sha1_t    auth_sha1;
    shr_bfd_msg_ctrl_stat_req_t     stat_req;
    bfd_sdk_msg_ctrl_stat_reply_t   stat_reply;
    bfd_sdk_msg_ctrl_trace_log_enable_t trace_enable;
    bfd_sdk_msg_ctrl_discard_stat_get_t pkt_discard_stat;
} bfd_sdk_msg_ctrl_t;

#endif /* BFD_SDK_MSG_H_ */


