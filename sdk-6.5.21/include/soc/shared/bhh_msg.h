/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    bhh_msg.h
 * Purpose: BHH Messages definitions common to SDK and uKernel.
 *
 *          Messages between SDK and uKernel.
 */

#ifndef   _SOC_SHARED_BHH_MSG_H_
#define   _SOC_SHARED_BHH_MSG_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif

#include <soc/shared/mos_msg_common.h>
#include <shared/bhh.h>
#include <soc/shared/oam_pm_pack.h>


/********************************
 * BHH Control Messages
 *
 *   SDK Host <--> uController
 */

#define SHR_BHH_CARRIER_CODE_LEN 6

/*
 * BHH Initialization control message
 */
typedef struct shr_bhh_msg_ctrl_init_s {
    uint32  num_sessions;        /* Max number of BHH sessions */
    uint32  rx_channel;          /* Local RX DMA channel (0..3) */
    uint32  node_id;             /* Node ident */
    uint8   carrier_code[SHR_BHH_CARRIER_CODE_LEN];
} shr_bhh_msg_ctrl_init_t;

/*
 * BHH Encapsulation control message
 *
 * Encapsulation Maximum length
 * OLP Tx(34) + L2(22) + 9 labels(36) + ACH(4) = 96
 *
 *
 * BHH Encapsulation Table
 *
 * The BHH Encapsulation is maintained in the uKernel
 * It contains the encapsulation for all current BHH sessions.
 *
 * A BHH encapsulation does NOT include the BHH PDU (BHH Control Packet).
 * This is built by the uKernel side.
 */
#define SHR_BHH_MAX_ENCAP_LENGTH    162
#define SHR_BHH_MEG_ID_LENGTH       48

/*
 * BHH Session Set control message
 */
typedef struct shr_bhh_msg_ctrl_sess_enable_s {
    uint32  sess_id;
    uint32  flags;
    uint16  remote_mep_id;
    uint8   enable;
} shr_bhh_msg_ctrl_sess_enable_t;


/*
 * BHH Session delete control message
 */
typedef struct shr_bhh_msg_ctrl_sess_delete_s {
    uint32  sess_id;
} shr_bhh_msg_ctrl_sess_delete_t;


/*
 * BHH Session Set control message
 */
typedef struct shr_bhh_msg_ctrl_sess_set_s {
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
} shr_bhh_msg_ctrl_sess_set_t;


/*
 * BHH Session Get control message
 */
typedef struct shr_bhh_msg_ctrl_sess_get_s {
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
    uint32   period;
    uint8    encap_type;    /* Raw, UDP-IPv4/IPv6, used for UDP checksum */
    uint32   encap_length;  /* BHH encapsulation length */
    uint8    encap_data[SHR_BHH_MAX_ENCAP_LENGTH];  /* Encapsulation data */
    uint32   tx_port;
    uint32   tx_cos;
    uint32   tx_pri;
    uint32   tx_qnum;
} shr_bhh_msg_ctrl_sess_get_t;


/*
 * BHH Statistics control messages (Request/Reply)
 */
typedef struct shr_bhh_msg_ctrl_stat_req_s {
    uint32  sess_id;    /* BHH session (endpoint) id */
    uint32  clear;      /* Clear stats */
} shr_bhh_msg_ctrl_stat_req_t;

typedef struct shr_bhh_msg_ctrl_stat_reply_s {
    uint32  sess_id;           /* BHH session (endpoint) id */
    uint32  packets_in;        /* Total packets in */
    uint32  packets_out;       /* Total packets out */
    uint32  packets_drop;      /* Total packets drop */
    uint32  packets_auth_drop; /* Packets drop due to authentication failure */
} shr_bhh_msg_ctrl_stat_reply_t;

/*
 * BHH Loopback add
 */
typedef struct shr_bhh_msg_ctrl_loopback_add_s {
    uint32  flags;
    uint32  sess_id;    /* BHH session (endpoint) id */
    uint32  period;
    uint32  ttl;
} shr_bhh_msg_ctrl_loopback_add_t;

/*
 * BHH Loopback delete
 */
typedef struct shr_bhh_msg_ctrl_loopback_delete_s {
    uint32  sess_id;    /* BHH session (endpoint) id */
} shr_bhh_msg_ctrl_loopback_delete_t;

/*
 * BHH Loopback get
 */
typedef struct shr_bhh_msg_ctrl_loopback_get_s {
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
} shr_bhh_msg_ctrl_loopback_get_t;

/*
 * BHH Loss Measurement add
 */
typedef struct shr_bhh_msg_ctrl_loss_add_s {
    uint32  flags;
    uint32  sess_id;    /* BHH session (endpoint) id */
    uint32  period;
    uint32  int_pri;
    uint8   pkt_pri;         
} shr_bhh_msg_ctrl_loss_add_t;

/*
 * BHH Loss Measurement delete
 */
typedef struct shr_bhh_msg_ctrl_loss_delete_s {
    uint32  sess_id;    /* BHH session (endpoint) id */
} shr_bhh_msg_ctrl_loss_delete_t;

/*
 * BHH Loss Measurement get
 */
typedef struct shr_bhh_msg_ctrl_loss_get_s {
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
} shr_bhh_msg_ctrl_loss_get_t;

/*
 * BHH Delay Measurement add
 */
typedef struct shr_bhh_msg_ctrl_delay_add_s {
    uint32  flags;
    uint32  sess_id;    /* BHH session (endpoint) id */
    uint32  period;
    uint32  int_pri;    
    uint8   pkt_pri;         
    uint8   dm_format;
} shr_bhh_msg_ctrl_delay_add_t;

/*
 * BHH Delay Measurement delete
 */
typedef struct shr_bhh_msg_ctrl_delay_delete_s {
    uint32  sess_id;    /* BHH session (endpoint) id */
} shr_bhh_msg_ctrl_delay_delete_t;

/*
 * BHH Delay Measurement get
 */
typedef struct shr_bhh_msg_ctrl_delay_get_s {
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
} shr_bhh_msg_ctrl_delay_get_t;

typedef shr_oam_pm_msg_ctrl_pm_stats_get_t shr_bhh_msg_ctrl_pm_stats_get_t;

/*
 * BHH control messages
 */
typedef union shr_bhh_msg_ctrl_s {
    shr_bhh_msg_ctrl_init_t         init;
    shr_bhh_msg_ctrl_sess_set_t     sess_set;
    shr_bhh_msg_ctrl_sess_get_t     sess_get;
    shr_bhh_msg_ctrl_sess_delete_t  sess_delete;
    shr_bhh_msg_ctrl_stat_req_t     stat_req;
    shr_bhh_msg_ctrl_stat_reply_t   stat_reply;
    shr_bhh_msg_ctrl_loopback_add_t loopback_add;
    shr_bhh_msg_ctrl_loopback_get_t loopback_get;
    shr_bhh_msg_ctrl_pm_stats_get_t pm_stats_get;
} shr_bhh_msg_ctrl_t;


/****************************************
 * BHH event message
 */
#define BHH_BTE_EVENT_LB_TIMEOUT                  0x000001
#define BHH_BTE_EVENT_LB_DISCOVERY_UPDATE         0x000002
#define BHH_BTE_EVENT_CCM_TIMEOUT                 0x000004
#define BHH_BTE_EVENT_STATE                       0x000008
#define BHH_BTE_EVENT_CCM_RDI                     0x000010
#define BHH_BTE_EVENT_CCM_UNKNOWN_MEG_LEVEL       0x000020
#define BHH_BTE_EVENT_CCM_UNKNOWN_MEG_ID          0x000040
#define BHH_BTE_EVENT_CCM_UNKNOWN_MEP_ID          0x000080
#define BHH_BTE_EVENT_CCM_UNKNOWN_PERIOD          0x000100
#define BHH_BTE_EVENT_CCM_UNKNOWN_PRIORITY        0x000200
#define BHH_BTE_EVENT_CCM_RDI_CLEAR               0x000400
#define BHH_BTE_EVENT_CCM_UNKNOWN_MEG_LEVEL_CLEAR 0x000800
#define BHH_BTE_EVENT_CCM_UNKNOWN_MEG_ID_CLEAR    0x001000
#define BHH_BTE_EVENT_CCM_UNKNOWN_MEP_ID_CLEAR    0x002000
#define BHH_BTE_EVENT_CCM_UNKNOWN_PERIOD_CLEAR    0x004000
#define BHH_BTE_EVENT_CCM_UNKNOWN_PRIORITY_CLEAR  0x008000
#define BHH_BTE_EVENT_CCM_TIMEOUT_CLEAR           0x010000
#define BHH_BTE_EVENT_PM_STATS_COUNTER_ROLLOVER   0x020000
#define BHH_BTE_EVENT_CSF_LOS                     0x040000
#define BHH_BTE_EVENT_CSF_FDI                     0x080000
#define BHH_BTE_EVENT_CSF_RDI                     0x100000
#define BHH_BTE_EVENT_CSF_DCI                     0x200000


#define BHH_BTE_CSF_FAULTS (BHH_BTE_EVENT_CSF_LOS | \
                            BHH_BTE_EVENT_CSF_FDI | \
                            BHH_BTE_EVENT_CSF_RDI)
/*
 *  The BHH event message is defined as a short message (use mos_msg_data_t).
 *
 *  The fields of mos_msg_data_t are used as followed:
 *      mclass   (uint8)  - MOS_MSG_CLASS_BHH_EVENT
 *      subclass (uint8)  - Unused
 *      len      (uint16) - BHH Session ID
 *      data     (uint32) - Events mask
 */
typedef mos_msg_data_t  bhh_msg_event_t;

#define SHR_BHH_MAX_PORTS        128
#define SHR_BHH_INVALID_TRUNK_ID 0xFFFF

#endif /* _SOC_SHARED_BHH_MSG_H_ */
