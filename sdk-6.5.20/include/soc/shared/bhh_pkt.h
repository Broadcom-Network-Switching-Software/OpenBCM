/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        bhh_pkt.h
 * Purpose:     BHH Packet Format definitions
 *              common to SDK and uKernel.
 */

#ifndef   _SOC_SHARED_BHH_PKT_H_
#define   _SOC_SHARED_BHH_PKT_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif

#include <shared/pack.h>
#include <shared/bhh.h>


/*******************************************
 * Pack/Unpack Macros
 * 
 * Data is packed/unpacked in/from Network byte order
 */
#define SHR_BHH_ENCAP_PACK_U8(_buf, _var)      _SHR_PACK_U8(_buf, _var)
#define SHR_BHH_ENCAP_PACK_U16(_buf, _var)     _SHR_PACK_U16(_buf, _var)
#define SHR_BHH_ENCAP_PACK_U32(_buf, _var)     _SHR_PACK_U32(_buf, _var)
#define SHR_BHH_ENCAP_UNPACK_U8(_buf, _var)    _SHR_UNPACK_U8(_buf, _var)
#define SHR_BHH_ENCAP_UNPACK_U16(_buf, _var)   _SHR_UNPACK_U16(_buf, _var)
#define SHR_BHH_ENCAP_UNPACK_U32(_buf, _var)   _SHR_UNPACK_U32(_buf, _var)

#define SHR_BHH_ENCAP_UNPACK_ETYPE(_buf, _var) _SHR_UNPACK_U16(_buf, _var)

/******************************************
 * Network Packet Format definitions
 *
 * Note: LENGTH is specified in bytes unless noted otherwise
 */

/* BHH Control Packet lengths */
#define SHR_BHH_HEADER_LENGTH                4
#define SHR_BHH_ACH_LENGTH                   4
#define SHR_BHH_ACH_TYPE_LENGTH              2
#define SHR_BHH_LM_OFFSET_INSIDE_CCM_PDU     58

/* Associated Channel Header */
#define SHR_BHH_ACH_FIRST_NIBBLE             0x1
#define SHR_BHH_ACH_VERSION                  0x0
#define SHR_BHH_ACH_CHANNEL_TYPE             0x8902
#define SHR_BHH_ACH_CHANNEL_TYPE_1           0x7FFA


/* MPLS */
#define SHR_BHH_MPLS_ROUTER_ALERT_LABEL      1
#define SHR_BHH_MPLS_GAL_LABEL               13
#define SHR_BHH_MPLS_PW_LABEL                0xFF


/* MPLS */
#define SHR_BHH_MPLS_LABEL_LENGTH            4
#define SHR_BHH_MPLS_ROUTER_ALERT_LABEL      1
#define SHR_BHH_MPLS_GAL_LABEL               13
#define SHR_BHH_MPLS_LABEL_LABEL_LENGTH      3  /* Label field in MPLS */
#define SHR_BHH_MPLS_LABEL_MASK              0xFFFFF000

#define SHR_BHH_MPLS_IS_BOS(_label)                     \
     (((_label)[2]) & 0x1)
#define SHR_BHH_MPLS_IS_GAL(_label)                     \
    (((_label)[0] == 0x00) && ((_label)[1] == 0x00) &&  \
     ((((_label)[2]) & 0xF0) == 0xD0))

/* Ether Type */
#define SHR_BHH_L2_ETYPE_MPLS_UCAST            0x8847

/* Ethernet Frame */
#define SHR_BHH_ENET_UNTAGGED_MIN_PKT_SIZE     64
#define SHR_BHH_ENET_TAGGED_MIN_PKT_SIZE       68
#define SHR_BHH_L2_MAC_ADDR_LENGTH             6
#define SHR_BHH_L2_VLAN_HEADER_LENGTH          4
#define SHR_BHH_L2_ETYPE_LENGTH                2

/* BHH op-code */
#define SHR_BHH_OPCODE_LEN                     1
#define SHR_BHH_OPCODE_LM_PREFIX              42
#define SHR_BHH_OPCODE_DM_PREFIX              44
#define SHR_BHH_OPCODE_LM_MASK                0xFE
#define SHR_BHH_OPCODE_DM_MASK                0xFC


typedef struct shr_bhh_mpls_header_s {
#ifdef LE_HOST
    uint32 ttl:8,
	s:1,
	exp:3,
	label:20;
#else
    uint32 label:20,
	exp:3,
	s:1,
	ttl:8;
#endif
} shr_bhh_mpls_header_t;

#endif /* _SOC_SHARED_BHH_PKT_H_ */
