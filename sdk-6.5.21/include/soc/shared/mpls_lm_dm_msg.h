/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    mpls_lm_dm_msg.h
 * Purpose: MPLS_LM_DM Messages definitions common to SDK and uKernel.
 *
 *          Messages between SDK and uKernel.
 */

#ifndef   _SOC_SHARED_MPLS_LM_DM_MSG_H_
#define   _SOC_SHARED_MPLS_LM_DM_MSG_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif

#include <soc/shared/mos_msg_common.h>
#include <soc/shared/mpls_lm_dm_pkt.h>
/* #include <shared/mpls_lm_dm.h> */



/*****************************************
 * MPLS_LM_DM uController Error codes
 */
typedef enum shr_mpls_lm_dm_uc_error_e {
    SHR_MPLS_LM_DM_UC_E_NONE = 0,
    SHR_MPLS_LM_DM_UC_E_INTERNAL,
    SHR_MPLS_LM_DM_UC_E_MEMORY,
    SHR_MPLS_LM_DM_UC_E_PARAM,
    SHR_MPLS_LM_DM_UC_E_RESOURCE,
    SHR_MPLS_LM_DM_UC_E_EXISTS,
    SHR_MPLS_LM_DM_UC_E_NOT_FOUND,
    SHR_MPLS_LM_DM_UC_E_UNAVAIL,
    SHR_MPLS_LM_DM_UC_E_VERSION,
    SHR_MPLS_LM_DM_UC_E_INIT
} shr_mpls_lm_dm_uc_error_t;

/********************************
 * MPLS_LM_DM Control Messages
 *
 *   SDK Host <--> uController
 */

/*
 * The MPLS_LM_DM Encapsulation is maintained in the uKernel
 * It contains the encapsulation for all current MPLS_LM_DM sessions.
 *
 * A MPLS_LM_DM encapsulation currently includes:
 *     L2 + [MPLS-TP]
 *     DMAC + SMAC + VLAN*2 + MPLS_ETH_TYPE + 9*MPLS_LABELS + ACH_HEADER
 *     6 + 6 + 4*2 + 2 + 9*4 + 4 = 62
 *
 * A MPLS_LM_DM encapsulation does NOT include the MPLS_LM_DM PDU (MPLS_LM_DM Control Packet).
 * This is built by the uKernel side.
 */
#define SHR_MPLS_LM_DM_MAX_ENCAP_LENGTH     62

/*
 * MPLS_LM_DM Initialization control message
 */
typedef struct shr_mpls_lm_dm_msg_ctrl_init_s {
    uint32  num_sessions;        /* Max number of MPLS_LM_DM sessions */
    uint32  rx_channel;          /* Local RX DMA channel (0..3) */
} shr_mpls_lm_dm_msg_ctrl_init_t;

/*
 * MPLS_LM_DM Loss Measurement add
 */
typedef struct shr_mpls_lm_dm_msg_ctrl_loss_add_s {
    uint32  flags;
    uint32  sess_id;    /* MPLS_LM_DM session (endpoint) id */
    uint32  period;
    uint32  int_pri;
    uint8   pkt_pri;
    uint16  l2_encap_length; /* L2 Encap Lenght */
    uint8   l2_encap_data[SHR_MPLS_LM_DM_MAX_ENCAP_LENGTH]; /* L2 Encap Data */
    /* OLP (L2 + OAM) Encap Data */
    uint8   olp_encap_data[MPLS_LM_DM_OLP_HDR_LEN]; 
    uint8   ctr_size;
    uint32  ctr_base_id[3];
    uint8   ctr_offset[3];
    uint8   ctr_action[3];
    uint8   ctr_byte_offset[3];
} shr_mpls_lm_dm_msg_ctrl_loss_add_t;

/*
 * MPLS_LM_DM Loss Measurement delete
 */
typedef struct shr_mpls_lm_dm_msg_ctrl_loss_delete_s {
    uint32  flags;
    uint32  sess_id;    /* MPLS_LM_DM session (endpoint) id */
} shr_mpls_lm_dm_msg_ctrl_loss_delete_t;

/*
 * MPLS_LM_DM Loss Measurement get
 */
typedef struct shr_mpls_lm_dm_msg_ctrl_loss_get_s {
    uint32  flags;
    uint32  sess_id;    /* MPLS_LM_DM session (endpoint) id */
} shr_mpls_lm_dm_msg_ctrl_loss_get_t;

/*
 * MPLS_LM_DM Loss Measurement get
 */
typedef struct shr_mpls_lm_dm_msg_ctrl_loss_data_s {
    uint32  flags;
    uint32  sess_id;    /* MPLS_LM_DM session (endpoint) id */
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
    uint8   ctr_size;
    uint32  ctr_base_id[3];
    uint8   ctr_offset[3];
    uint8   ctr_action[3];
    uint8   ctr_byte_offset[3];
    uint32  tx_nearend_upper;
    uint32  rx_nearend_upper;
    uint32  tx_farend_upper;
    uint32  rx_farend_upper;
    uint32  loss_nearend_upper;
    uint32  loss_farend_upper;
} shr_mpls_lm_dm_msg_ctrl_loss_data_t;

/*
 * MPLS_LM_DM Delay Measurement add
 */
typedef struct shr_mpls_lm_dm_msg_ctrl_delay_add_s {
    uint32  flags;
    uint32  sess_id;    /* MPLS_LM_DM session (endpoint) id */
    uint32  period;
    uint32  int_pri;    
    uint8   pkt_pri;         
    uint8   dm_format;
    uint16  l2_encap_length; /* L2 Encap Lenght */
    uint8   l2_encap_data[SHR_MPLS_LM_DM_MAX_ENCAP_LENGTH]; /* L2 Encap Data */
    uint8   olp_encap_data[MPLS_LM_DM_OLP_HDR_LEN]; 
} shr_mpls_lm_dm_msg_ctrl_delay_add_t;

/*
 * MPLS_LM_DM Delay Measurement delete
 */
typedef struct shr_mpls_lm_dm_msg_ctrl_delay_delete_s {
    uint32  flags;
    uint32  sess_id;    /* MPLS_LM_DM session (endpoint) id */
} shr_mpls_lm_dm_msg_ctrl_delay_delete_t;

/*
 * MPLS_LM_DM Delay Measurement get
 */
typedef struct shr_mpls_lm_dm_msg_ctrl_delay_get_s {
    uint32  flags;
    uint32  sess_id;    /* MPLS_LM_DM session (endpoint) id */
}shr_mpls_lm_dm_msg_ctrl_delay_get_t;

/*
 * MPLS_LM_DM Delay Measurement get
 */
typedef struct shr_mpls_lm_dm_msg_ctrl_delay_data_s {
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
}shr_mpls_lm_dm_msg_ctrl_delay_data_t;


/*
 * MPLS_LM_DM control messages
 */
typedef union shr_mpls_lm_dm_msg_ctrl_s {
    shr_mpls_lm_dm_msg_ctrl_init_t          init;
    shr_mpls_lm_dm_msg_ctrl_loss_add_t      loss_add;
    shr_mpls_lm_dm_msg_ctrl_loss_delete_t   loss_del;
    shr_mpls_lm_dm_msg_ctrl_loss_get_t      loss_get;
    shr_mpls_lm_dm_msg_ctrl_loss_data_t     loss_data;
    shr_mpls_lm_dm_msg_ctrl_delay_add_t     delay_add;
    shr_mpls_lm_dm_msg_ctrl_delay_delete_t  delay_del;
    shr_mpls_lm_dm_msg_ctrl_delay_get_t     delay_get;
    shr_mpls_lm_dm_msg_ctrl_delay_data_t    delay_data;
} shr_mpls_lm_dm_msg_ctrl_t;


/****************************************
 * MPLS_LM_DM event message
 */
#define MPLS_LM_DM_BTE_EVENT_STATE                      0x0001


/*
 *  The MPLS_LM_DM event message is defined as a short message (use mos_msg_data_t).
 *
 *  The fields of mos_msg_data_t are used as followed:
 *      mclass   (uint8)  - MOS_MSG_CLASS_MPLS_LM_DM_EVENT
 *      subclass (uint8)  - Unused
 *      len      (uint16) - MPLS_LM_DM Session ID
 *      data     (uint32) - Events mask
 */
typedef mos_msg_data_t  mpls_lm_dm_msg_event_t;

/****************************************
 * MPLS_LM_DM event message
 */
#define MPLS_LM_DM_BTE_EVENT_PM_STATS_COUNTER_ROLLOVER   0x000001

#endif /* _SOC_SHARED_MPLS_LM_DM_MSG_H_ */
