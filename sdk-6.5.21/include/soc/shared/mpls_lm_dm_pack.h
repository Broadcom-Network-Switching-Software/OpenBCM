/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mpls_lm_dm_pack.h
 * Purpose:     Interface to pack and unpack routines common to
 *              SDK and uKernel for:
 *              - MPLS_LM_DM Control messages
 *              - Network Packet headers
 *
 *              This is to be shared between SDK host and uKernel.
 */

#ifndef   _SOC_SHARED_MPLS_LM_DM_PACK_H_
#define   _SOC_SHARED_MPLS_LM_DM_PACK_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif

#include <soc/shared/mpls_lm_dm_msg.h>
#include <soc/shared/mpls_lm_dm_pkt.h>

/*
 * TLV Types
 */
#define SHR_MPLS_LM_DM_TLV_END            0x00
#define SHR_MPLS_LM_DM_TLV_TARGET_MEP     0x21
#define SHR_MPLS_LM_DM_TLV_REPLYING_MEP   0x22
#define SHR_MPLS_LM_DM_TLV_REQUESTING_MEP 0x23

/*
 * TLV Subtypes
 */
#define SHR_MPLS_LM_DM_TLV_MEP_SUB_INGRESS_DISCOVERY 0x00
#define SHR_MPLS_LM_DM_TLV_MEP_SUB_EGRESS_DISCOVERY  0x01
#define SHR_MPLS_LM_DM_TLV_MEP_SUB_MEP_ICC_ID        0x02
#define SHR_MPLS_LM_DM_TLV_MEP_SUB_MIP_ICC_ID        0x03

#define SHR_MPLS_LM_DM_ID_ZERO_LEN 24


typedef struct {
    uint8 zero[24];
} mpls_lm_dm_discovery_id_t;

typedef struct {
    uint16 mep_id;
    uint8 zero[22];
} mpls_lm_dm_reply_icc_t;

typedef struct {
    uint8 carrier_code[6];
    uint32 node_id;
    uint32 if_num;
    uint8  zero[10];
} mpls_lm_dm_reply_mip_icc_t;

typedef struct {
    uint16 mep_id;
    uint8  meg_id[48];
    uint16 reserved;
} mpls_lm_dm_requesting_mep_t;

#define SHR_MPLS_LM_DM_TLV_MAX_VALUE_LEN sizeof(mpls_lm_dm_requesting_mep_t)

typedef struct {
    uint8  type;
    uint16 length;
    uint8  subtype;
    union {
	uint8 data[SHR_MPLS_LM_DM_TLV_MAX_VALUE_LEN];
	mpls_lm_dm_discovery_id_t   discovery;
	mpls_lm_dm_reply_icc_t      reply_icc;
	mpls_lm_dm_reply_mip_icc_t  reply_mip_icc;
	mpls_lm_dm_requesting_mep_t requesting;
    } value;
} shr_mpls_lm_dm_tlv_t;

/* MPLS_LM_DM Control Messages pack/unpack */
extern uint8 *
shr_mpls_lm_dm_msg_ctrl_init_pack(uint8 *buf, shr_mpls_lm_dm_msg_ctrl_init_t *msg);
extern uint8 *
shr_mpls_lm_dm_msg_ctrl_init_unpack(uint8 *buf, shr_mpls_lm_dm_msg_ctrl_init_t *msg);
extern uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_add_pack(uint8 *buf,
				   shr_mpls_lm_dm_msg_ctrl_loss_add_t *msg);
extern uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_add_unpack(uint8 *buf,
				     shr_mpls_lm_dm_msg_ctrl_loss_add_t *msg);
extern uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_delete_pack(uint8 *buf,
				   shr_mpls_lm_dm_msg_ctrl_loss_delete_t *msg);
extern uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_delete_unpack(uint8 *buf,
				     shr_mpls_lm_dm_msg_ctrl_loss_delete_t *msg);
extern uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_get_pack(uint8 *buf,
				   shr_mpls_lm_dm_msg_ctrl_loss_get_t *msg);
extern uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_get_unpack(uint8 *buf,
				     shr_mpls_lm_dm_msg_ctrl_loss_get_t *msg);
extern uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_data_pack(uint8 *buf,
				   shr_mpls_lm_dm_msg_ctrl_loss_data_t *msg);
extern uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_data_unpack(uint8 *buf,
				     shr_mpls_lm_dm_msg_ctrl_loss_data_t *msg);

extern uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_add_pack(uint8 *buf,
				   shr_mpls_lm_dm_msg_ctrl_delay_add_t *msg);
extern uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_add_unpack(uint8 *buf,
				     shr_mpls_lm_dm_msg_ctrl_delay_add_t *msg);
extern uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_delete_pack(uint8 *buf,
				   shr_mpls_lm_dm_msg_ctrl_delay_delete_t *msg);
extern uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_delete_unpack(uint8 *buf,
				     shr_mpls_lm_dm_msg_ctrl_delay_delete_t *msg);
extern uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_get_pack(uint8 *buf,
				   shr_mpls_lm_dm_msg_ctrl_delay_get_t *msg);
extern uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_get_unpack(uint8 *buf,
				     shr_mpls_lm_dm_msg_ctrl_delay_get_t *msg);
extern uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_data_pack(uint8 *buf,
				   shr_mpls_lm_dm_msg_ctrl_delay_data_t *msg);
extern uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_data_unpack(uint8 *buf,
				     shr_mpls_lm_dm_msg_ctrl_delay_data_t *msg);


#endif /* _SOC_SHARED_MPLS_LM_DM_PACK_H_ */
