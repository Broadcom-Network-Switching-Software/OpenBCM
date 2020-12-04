/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        eth_lm_dm_pkt.h
 * Purpose:     ETH_LM_DM Packet Format definitions
 *              common to SDK and uKernel.
 */

#ifndef   _ETH_LM_DM_PKT_H_
#define   _ETH_LM_DM_PKT_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif
#include <soc/shared/olp_pkt.h>
#include <soc/shared/olp_pack.h>
#include <shared/pack.h>



/*******************************************
 * Pack/Unpack Macros
 * 
 * Data is packed/unpacked in/from Network byte order
 */
#define SHR_ETH_LM_DM_ENCAP_PACK_U8(_buf, _var)     _SHR_PACK_U8(_buf, _var)
#define SHR_ETH_LM_DM_ENCAP_PACK_U16(_buf, _var)    _SHR_PACK_U16(_buf, _var)
#define SHR_ETH_LM_DM_ENCAP_PACK_U32(_buf, _var)    _SHR_PACK_U32(_buf, _var)
#define SHR_ETH_LM_DM_ENCAP_UNPACK_U8(_buf, _var)   _SHR_UNPACK_U8(_buf, _var)
#define SHR_ETH_LM_DM_ENCAP_UNPACK_U16(_buf, _var)  _SHR_UNPACK_U16(_buf, _var)
#define SHR_ETH_LM_DM_ENCAP_UNPACK_U32(_buf, _var)  _SHR_UNPACK_U32(_buf, _var)


/******************************************
 * Network Packet Format definitions
 *
 * Note: LENGTH is specified in bytes unless noted otherwise
 */

/* ETH_LM_DM Control Packet lengths */
#define SHR_ETH_LM_DM_HEADER_LENGTH 						4               

/*Macros for Packet replacement offset */
#define ETH_LM_DM_RPL_OFFSET_START              14
#define ETH_LM_DM_RPL_OFFSET_DIVIDER_BY_SHIFT   1


/* Ether Type */
#define SHR_ETH_LM_DM_L2_VLAN_ETYPE                  0x8100
#define SHR_ETH_LM_DM_L2_ETYPE_OAM	                 0x8902
#define SHR_ETH_LM_DM_L2_HEADER_TAGGED_LENGTH        18
#define SHR_ETH_LM_DM_L2_HEADER_LENGTH        		 12
#define SHR_ETH_LM_DM_L2_HEADER_TAGGED_ETYPE_OFFSET  16

/* ETH_LM_DM op-code */
#define SHR_ETH_LM_DM_OPCODE_LEN                     1
#define SHR_ETH_LM_DM_OPCODE_LM_PREFIX              42
#define SHR_ETH_LM_DM_OPCODE_DM_PREFIX              44


#define SHR_ETH_LM_DM_OLP_L2_HEADER_LENGTH			18
#define SHR_ETH_LM_DM_OLP_RX_HEADER_LENGTH			20



/*************************************
 * ETH_LM_DM Control Packet Format
 */

/*
 * ETH_LM_DM Y.1731 Header
 */
typedef struct shr_eth_lm_dm_header_s {
    /* Mandatory */
#ifdef LE_HOST
    uint32 tlv_offset:8,
    	   flags_type:1,
           flags_rsvd:7,
           opcode:8,
           version:5,
           mel:3;
#else
    uint32 mel:3,
           version:5,
           opcode:8,
           flags_rsvd:7,
           flags_type:1,
           tlv_offset:8;
#endif  /* LE_HOST */
} shr_eth_lm_dm_header_t;

typedef struct shr_eth_lm_dm_slm_info_header_s {
	
	uint16 src_mep_id;
	uint16 resp_mep_id;
	uint32 test_id;

  
} shr_eth_lm_dm_slm_info_header_t;


/*
 * ETH_LM_DM OLP Header
 */

typedef struct soc_olp_rx_hdr olp_rx_hdr_t;

typedef	struct soc_olp_tx_hdr olp_tx_hdr_t;

/* L2 Header */
typedef struct soc_olp_l2_hdr_s  _olp_l2_header_t;




#endif /* _ETH_LM_DM_PKT_H_ */
