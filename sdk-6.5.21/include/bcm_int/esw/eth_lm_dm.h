
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    eth_lm_dm.h
 * Purpose: ETH_LM_DM definitions common to SDK and uKernel
 */
#ifndef __ETH_LM_DM_H__
#define __ETH_LM_DM_H__
#if defined(INCLUDE_ETH_LM_DM)

#include <bcm/types.h>
#include <soc/uc_msg.h>
#include <soc/shared/mos_msg_common.h>
#include <soc/shared/eth_lm_dm.h>
#include <soc/shared/eth_lm_dm_pkt.h>
#include <soc/shared/eth_lm_dm_msg.h>
#include <soc/shared/eth_lm_dm_pack.h>

#include <bcm_int/common/rx.h>

/*
 * ETH_LM_DM Encapsulation Definitions
 *
 * Defines for building the ETH_LM_DM packet encapsulation
 */

#define BCM_ETH_ENDPOINT_MAX_MEP_ID_LENGTH 32

/**************************************************************************************/
#define _ETH_LM_DM_UC_MSG_TIMEOUT_USECS     5000000

#define BCM_ETH_ENDPOINT_PASSIVE            0x0004     /* Specifies endpoint
                                                          takes passive role */
#define BCM_ETH_ENDPOINT_DEMAND             0x0008     /* Specifies local*/
#define BCM_ETH_ENDPOINT_ENCAP_SET          0x0010     /* Update encapsulation
                                                          on existing BFD
                                                          endpoint */
/*
 * ETH_LM_DM Encapsulation Format Header flags
 *
 * Indicates the type of headers/labels present in a ETH_LM_DM packet.
 */
#define _ETH_LM_DM_ENCAP_PKT_ETH_LM_DM                     (1 << 11)
/*
 * Internal flags for ETH_LM_DM.
 */
#define _ETH_LM_DM_F_SESSION_IS_SET                        0x00000001
#define _ETH_LM_DM_F_DM_IS_SET                             0x00000002
#define _ETH_LM_DM_F_LM_IS_SET                             0x00000004 

#define ETH_LMDM_EVENT_THREAD_PRIORITY                     200

/*
 * Macros to pack uint8, uint16, uint32 in Network byte order
 */
#define _ETH_LM_DM_ENCAP_PACK_U8(_buf, _var)   SHR_ETH_LM_DM_ENCAP_PACK_U8(_buf, _var)
#define _ETH_LM_DM_ENCAP_PACK_U16(_buf, _var)  SHR_ETH_LM_DM_ENCAP_PACK_U16(_buf, _var)
#define _ETH_LM_DM_ENCAP_PACK_U32(_buf, _var)  SHR_ETH_LM_DM_ENCAP_PACK_U32(_buf, _var)


/* VLAN Tag - 802.1Q */
typedef struct _eth_lm_dm_vlan_tag_s {
    uint16      tpid;    /* 16: Tag Protocol Identifier */
    struct {
        uint16   prio:3,    /*  3: Priority Code Point */
           		 cfi:1,     /*  1: Canonical Format Indicator */
        		 vid:12;     /* 12: Vlan Identifier */
    } tci;               /* Tag Control Identifier */
} _eth_lm_dm_vlan_tag_t;

/* L2 Header */
typedef struct _eth_l2_header_s {
    bcm_mac_t    dst_mac;     /* 48: Destination MAC */
    bcm_mac_t    src_mac;     /* 48: Source MAC */
    _eth_lm_dm_vlan_tag_t  vlan_tag;    /* VLAN Tag */
    _eth_lm_dm_vlan_tag_t  inner_vlan_tag; /* VLAN Tag */
    uint16       etype;       /* 16: Ether Type */
} _eth_l2_header_t;

#endif 
#endif
