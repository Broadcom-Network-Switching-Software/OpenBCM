/*******************************************************************************
 *
 * \file field_api.c
 *
 * This file is auto-generated using HSDKGen.py.
 *
 * Edits to this file will be lost when it is regenerated.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>
#include <sal/core/libc.h>
#include <shared/field.h>
#include <bcm/policer.h>
#include <bcm/port.h>
#include <bcm/rx.h>
#include <bcm/flowtracker.h>
#include <bcm/subport.h>
#include <bcm/field.h>
#include <bcm_int/ltsw/field.h>
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <bcmlt/bcmlt.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_FP

/******************************************************************************
* Global variables
 */
const char bcm56990_a0_enum_str_any[] = "ANY";
const char bcm56990_a0_enum_str_ether_2[] = "ETHER_2";
const char bcm56990_a0_enum_str_snap[] = "SNAP";
const char bcm56990_a0_enum_str_llc[] = "LLC";
const char bcm56990_a0_enum_str_ieee802dot3[] = "IEEE802DOT3";
const char bcm56990_a0_enum_str_tcp[] = "TCP";
const char bcm56990_a0_enum_str_udp[] = "UDP";
const char bcm56990_a0_enum_str_igmp[] = "IGMP";
const char bcm56990_a0_enum_str_icmp[] = "ICMP";
const char bcm56990_a0_enum_str_ip6_icmp[] = "IP6_ICMP";
const char bcm56990_a0_enum_str_ip6_hop_by_hop[] = "IP6_HOP_BY_HOP";
const char bcm56990_a0_enum_str_ip_in_ip[] = "IP_IN_IP";
const char bcm56990_a0_enum_str_tcp_udp[] = "TCP_UDP";
const char bcm56990_a0_enum_str_l2[] = "L2";
const char bcm56990_a0_enum_str_l3[] = "L3";
const char bcm56990_a0_enum_str_l2_shared[] = "L2_SHARED";
const char bcm56990_a0_enum_str_l2_independent[] = "L2_INDEPENDENT";
const char bcm56990_a0_enum_str_l3_direct[] = "L3_DIRECT";
const char bcm56990_a0_enum_str_mpls[] = "MPLS";
const char bcm56990_a0_enum_str_trill[] = "TRILL";
const char bcm56990_a0_enum_str_port_extender[] = "PORT_EXTENDER";
const char bcm56990_a0_enum_str_non_ip[] = "NON_IP";
const char bcm56990_a0_enum_str_ip4_no_options[] = "IP4_NO_OPTIONS";
const char bcm56990_a0_enum_str_ip4_options[] = "IP4_OPTIONS";
const char bcm56990_a0_enum_str_any_ip4[] = "ANY_IP4";
const char bcm56990_a0_enum_str_ip6_no_ext_hdr[] = "IP6_NO_EXT_HDR";
const char bcm56990_a0_enum_str_ip6_one_ext_hdr[] = "IP6_ONE_EXT_HDR";
const char bcm56990_a0_enum_str_ip6_two_ext_hdr[] = "IP6_TWO_EXT_HDR";
const char bcm56990_a0_enum_str_any_ip6[] = "ANY_IP6";
const char bcm56990_a0_enum_str_ip[] = "IP";
const char bcm56990_a0_enum_str_arp[] = "ARP";
const char bcm56990_a0_enum_str_arp_request[] = "ARP_REQUEST";
const char bcm56990_a0_enum_str_arp_reply[] = "ARP_REPLY";
const char bcm56990_a0_enum_str_uc_mpls[] = "UC_MPLS";
const char bcm56990_a0_enum_str_mc_mpls[] = "MC_MPLS";
const char bcm56990_a0_enum_str_mim[] = "MIM";
const char bcm56990_a0_enum_str_auto_ip4_mc[] = "AUTO_IP4_MC";
const char bcm56990_a0_enum_str_l2gre[] = "L2GRE";
const char bcm56990_a0_enum_str_vxlan[] = "VXLAN";
const char bcm56990_a0_enum_str_none[] = "NONE";
const char bcm56990_a0_enum_str_non[] = "NON";
const char bcm56990_a0_enum_str_first[] = "FIRST";
const char bcm56990_a0_enum_str_non_or_first[] = "NON_OR_FIRST";
const char bcm56990_a0_enum_str_not_first[] = "NOT_FIRST";

static const bcm_field_qual_internal_map_info_t bcm_field_efp_qual_bcmFieldQualifySrcClassL2_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = "L2_SRC",
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_qual_bcmFieldQualifySrcClassL3_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = "L3_SRC",
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_qual_bcmFieldQualifySrcClassField_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = "FP_VLAN_CLASS_1",
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_qual_bcmFieldQualifyDstClassL2_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = "L2_DST",
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_qual_bcmFieldQualifyDstClassL3_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = "L3_SRC",
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_qual_bcmFieldQualifyDstClassField_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = "FP_VLAN_CLASS_0",
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_qual_bcmFieldQualifyInterfaceClassL3_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = "L3_IIF",
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_qual_bcmFieldQualifyIngressClassField_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = "FP_ING",
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_qual_bcmFieldQualifyIngressInterfaceClassPort_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = "VLAN",
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_qual_bcmFieldQualifyIngressInterfaceClassVPort_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = "SVP",
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_group_qual_bcmFieldQualifySrcClassL2_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = (void *)1,
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_group_qual_bcmFieldQualifySrcClassL3_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = (void *)1,
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_group_qual_bcmFieldQualifySrcClassField_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = (void *)1,
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyDstClassL2_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = (void *)1,
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyDstClassL3_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = (void *)1,
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyDstClassField_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = (void *)1,
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyInterfaceClassL3_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = (void *)1,
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyIngressClassField_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = (void *)1,
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyIngressInterfaceClassPort_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = (void *)1,
    },
};

static const bcm_field_qual_internal_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyIngressInterfaceClassVPort_QUAL_ING_CLASS_ID_internal_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID_TYPE",
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = (void *)1,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifySrcIp6_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP6_UPPER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_SRC_IP6_LOWER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDstIp6_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP6_UPPER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_DST_IP6_LOWER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifySrcIp6High_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP6_UPPER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDstIp6High_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP6_UPPER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifySrcIp6Low_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP6_LOWER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDstIp6Low_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP6_LOWER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifySrcMac_map[] = {
    {
        .lt_field_name = "QUAL_SRC_MAC",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDstMac_map[] = {
    {
        .lt_field_name = "QUAL_DST_MAC",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifySrcIp_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP4",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDstIp_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP4",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyInPort_map[] = {
    {
        .lt_field_name = "QUAL_INPORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDrop_map[] = {
    {
        .lt_field_name = "QUAL_DROP_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIp6FlowLabel_map[] = {
    {
        .lt_field_name = "QUAL_IP6_FLOW_LABEL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyOuterVlan_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {12},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OUTER_VLAN_CFI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OUTER_VLAN_PRI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {3},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyOuterVlanId_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyOuterVlanPri_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_PRI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyOuterVlanCfi_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_CFI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyRangeCheck_map[] = {
    {
        .lt_field_name = "QUAL_RANGE_CHECKID_BMP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyL4SrcPort_map[] = {
    {
        .lt_field_name = "QUAL_L4SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyL4DstPort_map[] = {
    {
        .lt_field_name = "QUAL_L4DST_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyEtherType_map[] = {
    {
        .lt_field_name = "QUAL_ETHERTYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIpProtocol_map[] = {
    {
        .lt_field_name = "QUAL_IP_PROTOCOL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDSCP_map[] = {
    {
        .lt_field_name = "QUAL_TOS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyTtl_map[] = {
    {
        .lt_field_name = "QUAL_TTL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifySrcPort_map[] = {
    {
        .lt_field_name = "QUAL_SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifySrcTrunk_map[] = {
    {
        .lt_field_name = "QUAL_SRC_TRUNK",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDstPort_map[] = {
    {
        .lt_field_name = "QUAL_DST_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDstTrunk_map[] = {
    {
        .lt_field_name = "QUAL_DST_TRUNK",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyTcpControl_map[] = {
    {
        .lt_field_name = "QUAL_TCP_FLAGS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyPacketRes_map[] = {
    {
        .lt_field_name = "QUAL_PKT_RESOLUTION",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifySrcClassL2_map[] = {
    {
        .lt_field_name = "QUAL_L2_SRC_CLASS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifySrcClassL3_map[] = {
    {
        .lt_field_name = "QUAL_L3_SRC_CLASS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifySrcClassField_map[] = {
    {
        .lt_field_name = "QUAL_FP_VLAN_CLASS1",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDstClassL2_map[] = {
    {
        .lt_field_name = "QUAL_L2_DST_CLASS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDstClassL3_map[] = {
    {
        .lt_field_name = "QUAL_L3_DST_CLASS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDstClassField_map[] = {
    {
        .lt_field_name = "QUAL_FP_VLAN_CLASS0",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIpInfo_map[] = {
    {
        .lt_field_name = "QUAL_IP_CHECKSUM_VALID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyMHOpcode_map[] = {
    {
        .lt_field_name = "QUAL_MH_OPCODE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIpFlags_map[] = {
    {
        .lt_field_name = "QUAL_IP_FLAGS_MF",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_IP_FLAGS_DF",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyInnerTpid_map[] = {
    {
        .lt_field_name = "QUAL_INNER_TPID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyOuterTpid_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_TPID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyLoopbackType_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyLoopback_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIpType_map[] = {
    {
        .lt_field_name = "QUAL_IP_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyForwardingType_map[] = {
    {
        .lt_field_name = "QUAL_FWD_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyInterfaceClassL2_map[] = {
    {
        .lt_field_name = "QUAL_VLAN_CLASS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyInterfaceClassL3_map[] = {
    {
        .lt_field_name = "QUAL_L3_IIF_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyInterfaceClassPort_map[] = {
    {
        .lt_field_name = "QUAL_PORT_SYSTEM_CLASS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyL3Routable_map[] = {
    {
        .lt_field_name = "QUAL_L3_ROUTABLE_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIpFrag_map[] = {
    {
        .lt_field_name = "QUAL_IP_FRAG",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyVrf_map[] = {
    {
        .lt_field_name = "QUAL_VRF",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyL3Ingress_map[] = {
    {
        .lt_field_name = "QUAL_L3_IIF",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyL2Format_map[] = {
    {
        .lt_field_name = "QUAL_L2_FORMAT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyVlanFormat_map[] = {
    {
        .lt_field_name = "QUAL_VLAN_OUTER_PRESENT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OPAQUE_TAG_PRESENT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyExtensionHeaderType_map[] = {
    {
        .lt_field_name = "QUAL_IP_FIRST_EH_PROTO",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyExtensionHeaderSubCode_map[] = {
    {
        .lt_field_name = "QUAL_IP_FIRST_EH_SUBCODE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyExtensionHeader2Type_map[] = {
    {
        .lt_field_name = "QUAL_IP_SECOND_EH_PROTO",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyL4Ports_map[] = {
    {
        .lt_field_name = "QUAL_L4_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyMplsTerminated_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_BOS_TERMINATED",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyForwardingVlanValid_map[] = {
    {
        .lt_field_name = "QUAL_FWD_VLAN_VALID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIngressStpState_map[] = {
    {
        .lt_field_name = "QUAL_ING_STP_STATE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyL2SrcHit_map[] = {
    {
        .lt_field_name = "QUAL_L2_SRC_HIT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyL2SrcStatic_map[] = {
    {
        .lt_field_name = "QUAL_L2_SRC_STATIC",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyL2DestHit_map[] = {
    {
        .lt_field_name = "QUAL_L2_DST_HIT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyL2StationMove_map[] = {
    {
        .lt_field_name = "QUAL_L2STATION_MOVE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyL2CacheHit_map[] = {
    {
        .lt_field_name = "QUAL_L2CACHE_HIT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyL3DestRouteHit_map[] = {
    {
        .lt_field_name = "QUAL_L3_DST_LPM_HIT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDosAttack_map[] = {
    {
        .lt_field_name = "QUAL_DOSATTACK_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIcmpTypeCode_map[] = {
    {
        .lt_field_name = "QUAL_ICMP_TYPE_CODE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDstL3Egress_map[] = {
    {
        .lt_field_name = "QUAL_NHOP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyColor_map[] = {
    {
        .lt_field_name = "QUAL_COLOR",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIntPriority_map[] = {
    {
        .lt_field_name = "QUAL_INT_PRI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyForwardingVlanId_map[] = {
    {
        .lt_field_name = "QUAL_FWD_VLAN_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyVpn_map[] = {
    {
        .lt_field_name = "QUAL_VPN",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyMyStationHit_map[] = {
    {
        .lt_field_name = "QUAL_MYSTATIONTCAM_HIT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDstMultipath_map[] = {
    {
        .lt_field_name = "QUAL_OVERLAY_ECMP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyL2PayloadFirstEightBytes_map[] = {
    {
        .lt_field_name = "QUAL_BYTES_0_7_AFTER_L2HEADER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDstL3EgressNextHops_map[] = {
    {
        .lt_field_name = "QUAL_NHOP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyRecoverableDrop_map[] = {
    {
        .lt_field_name = "QUAL_PROTECTION_DATA_DROP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIpTunnelHit_map[] = {
    {
        .lt_field_name = "QUAL_L3_TNL_HIT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyMplsLabel1Hit_map[] = {
    {
        .lt_field_name = "QUAL_MPLSENTRY_FIRSTLOOKUP_HIT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyMplsLabel2Hit_map[] = {
    {
        .lt_field_name = "QUAL_MPLSENTRY_SECONDLOOKUP_HIT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyGenericAssociatedChannelLabelValid_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_SPECIAL_LABEL_GAL_DETECTED",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyRouterAlertLabelValid_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_SPECIAL_LABEL_RAL_DETECTED",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyNormalizeIpAddrs_map[] = {
    {
        .lt_field_name = "QUAL_IPADDR_NORMALIZE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyNormalizeMacAddrs_map[] = {
    {
        .lt_field_name = "QUAL_MACADDR_NORMALIZE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyMplsForwardingLabel_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {20},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_EXP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {3},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_BOS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_TTL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {8},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyMplsForwardingLabelTtl_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_TTL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyMplsForwardingLabelBos_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_BOS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyMplsForwardingLabelExp_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_EXP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyMplsForwardingLabelId_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyMplsControlWord_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_CTRL_WORD",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyRtag7AHashUpper_map[] = {
    {
        .lt_field_name = "QUAL_RTAG7A_HASH_UPPER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyRtag7AHashLower_map[] = {
    {
        .lt_field_name = "QUAL_RTAG7A_HASH_LOWER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyRtag7BHashUpper_map[] = {
    {
        .lt_field_name = "QUAL_RTAG7B_HASH_UPPER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyRtag7BHashLower_map[] = {
    {
        .lt_field_name = "QUAL_RTAG7B_HASH_LOWER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyMplsForwardingLabelAction_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_ACTION",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyMplsControlWordValid_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_SPECIAL_LABEL_CW_DETECTED",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifySrcGport_map[] = {
    {
        .lt_field_name = "QUAL_SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyL3SrcRouteHit_map[] = {
    {
        .lt_field_name = "QUAL_L3_SRC_LPM_HIT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyVxlanNetworkId_map[] = {
    {
        .lt_field_name = "QUAL_VXLAN_VNID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyVxlanFlags_map[] = {
    {
        .lt_field_name = "QUAL_VXLAN_FLAGS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIcmpError_map[] = {
    {
        .lt_field_name = "QUAL_ICMP_ERROR_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIntCongestionNotification_map[] = {
    {
        .lt_field_name = "QUAL_INT_CN",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifySrcGports_map[] = {
    {
        .lt_field_name = "QUAL_SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyVxlanHeaderBits8_31_map[] = {
    {
        .lt_field_name = "QUAL_VXLAN_RSVD_24",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyVxlanHeaderBits56_63_map[] = {
    {
        .lt_field_name = "QUAL_VXLAN_RSVD_8",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIpProtocolClass_map[] = {
    {
        .lt_field_name = "QUAL_C_IP_PROTOCOL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyEtherTypeClass_map[] = {
    {
        .lt_field_name = "QUAL_C_ETHERTYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyTcpClassZero_map[] = {
    {
        .lt_field_name = "QUAL_C_TCP_FLAGS0",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyTosClassZero_map[] = {
    {
        .lt_field_name = "QUAL_C_TOS0",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyTtlClassZero_map[] = {
    {
        .lt_field_name = "QUAL_C_TTL0",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyTcpClassOne_map[] = {
    {
        .lt_field_name = "QUAL_C_TCP_FLAGS1",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyTosClassOne_map[] = {
    {
        .lt_field_name = "QUAL_C_TOS1",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyTtlClassOne_map[] = {
    {
        .lt_field_name = "QUAL_C_TTL1",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIpFragNonOrFirst_map[] = {
    {
        .lt_field_name = "QUAL_NON_OR_FIRST_FRAGMENT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyPacketLength_map[] = {
    {
        .lt_field_name = "QUAL_PKT_LENGTH",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyGroupClass_map[] = {
    {
        .lt_field_name = "QUAL_PRESEL_CLASS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyUdfClass_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CLASS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyInterfaceLookupClassPort_map[] = {
    {
        .lt_field_name = "QUAL_FP_VLAN_PORT_GRP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyInterfaceIngressKeySelectClassPort_map[] = {
    {
        .lt_field_name = "QUAL_FP_ING_GRP_SEL_CLASS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyBfdYourDiscriminator_map[] = {
    {
        .lt_field_name = "QUAL_BFD_DISCRIMINATOR",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDstL2MulticastGroup_map[] = {
    {
        .lt_field_name = "QUAL_L2_MC_GROUP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDstL3MulticastGroup_map[] = {
    {
        .lt_field_name = "QUAL_L3_MC_GROUP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyExactMatchActionClassId_map[] = {
    {
        .lt_field_name = "QUAL_EM_FIRST_LOOKUP_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyExactMatchGroupClassId_map[] = {
    {
        .lt_field_name = "QUAL_EM_GROUP_CLASS_ID_0",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyL2PayLoad_map[] = {
    {
        .lt_field_name = "QUAL_BYTES_0_7_AFTER_L2HEADER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_BYTES_8_15_AFTER_L2HEADER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_BYTES_16_23_AFTER_L2HEADER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_BYTES_24_31_AFTER_L2HEADER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyStpState_map[] = {
    {
        .lt_field_name = "QUAL_ING_STP_STATE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifySystemPortBitmap_map[] = {
    {
        .lt_field_name = "QUAL_SYSTEM_PORTS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDevicePortBitmap_map[] = {
    {
        .lt_field_name = "QUAL_DEVICE_PORTS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIntPktLengthValid_map[] = {
    {
        .lt_field_name = "QUAL_INBAND_TELEMETRY_MAX_LENGTH_CHECK",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIntPktFinalHop_map[] = {
    {
        .lt_field_name = "QUAL_INBAND_TELEMETRY_HOP_LIMIT_CHECK_STATUS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIpTunnelTtl_map[] = {
    {
        .lt_field_name = "QUAL_TNL_IP_TTL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIpTunnelTtlClassZero_map[] = {
    {
        .lt_field_name = "QUAL_C_TNL_IP_TTL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyRangeCheckGroup_map[] = {
    {
        .lt_field_name = "QUAL_RANGE_CHECK_GRP_ID_BMP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyOpaqueTagHigh_map[] = {
    {
        .lt_field_name = "QUAL_OPAQUE_TAG_HIGH",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyOpaqueTagLow_map[] = {
    {
        .lt_field_name = "QUAL_OPAQUE_TAG_LOW",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIfaHopLimitEqualToZero_map[] = {
    {
        .lt_field_name = "QUAL_INBAND_TELEMETRY_HOP_LIMIT_CHECK_STATUS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIntFlags_map[] = {
    {
        .lt_field_name = "QUAL_INBAND_TELEMETRY_FLAGS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIntHdrType_map[] = {
    {
        .lt_field_name = "QUAL_INBAND_TELEMETRY_HDR_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIntUdpChecksumEqualsZero_map[] = {
    {
        .lt_field_name = "QUAL_INBAND_TELEMETRY_UDP_CHECKSUM_ZERO",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIntMetadataHdrType_map[] = {
    {
        .lt_field_name = "QUAL_INBAND_TELEMETRY_MD_HDR_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyIntHdrPresent_map[] = {
    {
        .lt_field_name = "QUAL_INBAND_TELEMETRY_HDR_IN_TNL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyCompressionSrcHit_map[] = {
    {
        .lt_field_name = "QUAL_COMPRESSED_L3_SRC_HIT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyCompressionDstHit_map[] = {
    {
        .lt_field_name = "QUAL_COMPRESSED_L3_DST_HIT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifySrcCompressionClassId_map[] = {
    {
        .lt_field_name = "QUAL_L3_L4_COMPRESSION_ID_B_0_15",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_qual_bcmFieldQualifyDstCompressionClassId_map[] = {
    {
        .lt_field_name = "QUAL_L3_L4_COMPRESSION_ID_A_0_15",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifySrcIp6_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP6_UPPER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_SRC_IP6_LOWER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyDstIp6_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP6_UPPER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_DST_IP6_LOWER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifySrcIp6High_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP6_HIGH",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyDstIp6High_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP6_HIGH",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifySrcMac_map[] = {
    {
        .lt_field_name = "QUAL_SRC_MAC",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyDstMac_map[] = {
    {
        .lt_field_name = "QUAL_DST_MAC",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifySrcIp_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP4",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyDstIp_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP4",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyInPort_map[] = {
    {
        .lt_field_name = "QUAL_INPORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyDrop_map[] = {
    {
        .lt_field_name = "QUAL_DROP_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyOuterVlan_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {12},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OUTER_VLAN_CFI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OUTER_VLAN_PRI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {3},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyOuterVlanId_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyOuterVlanPri_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_PRI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyOuterVlanCfi_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_CFI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyL4SrcPort_map[] = {
    {
        .lt_field_name = "QUAL_L4SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyL4DstPort_map[] = {
    {
        .lt_field_name = "QUAL_L4DST_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyEtherType_map[] = {
    {
        .lt_field_name = "QUAL_ETHERTYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyIpProtocol_map[] = {
    {
        .lt_field_name = "QUAL_IP_PROTOCOL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyDSCP_map[] = {
    {
        .lt_field_name = "QUAL_TOS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyTtl_map[] = {
    {
        .lt_field_name = "QUAL_TTL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyTcpControl_map[] = {
    {
        .lt_field_name = "QUAL_TCP_FLAGS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifySrcClassL2_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_qual_bcmFieldQualifySrcClassL2_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifySrcClassL3_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_qual_bcmFieldQualifySrcClassL3_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifySrcClassField_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_qual_bcmFieldQualifySrcClassField_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyDstClassL2_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_qual_bcmFieldQualifyDstClassL2_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyDstClassL3_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_qual_bcmFieldQualifyDstClassL3_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyDstClassField_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_qual_bcmFieldQualifyDstClassField_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyLoopbackType_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyIpType_map[] = {
    {
        .lt_field_name = "QUAL_IP_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyForwardingType_map[] = {
    {
        .lt_field_name = "QUAL_FWD_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyInterfaceClassL3_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_qual_bcmFieldQualifyInterfaceClassL3_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyInterfaceClassPort_map[] = {
    {
        .lt_field_name = "QUAL_PORT_GRP_OR_PORT_SYSTEM_CLASS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyL3Routable_map[] = {
    {
        .lt_field_name = "QUAL_L3_ROUTABLE_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyIpFrag_map[] = {
    {
        .lt_field_name = "QUAL_IP_FRAG",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyVrf_map[] = {
    {
        .lt_field_name = "QUAL_VRF",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyOutPort_map[] = {
    {
        .lt_field_name = "QUAL_OUTPORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyIp4_map[] = {
    {
        .lt_field_name = "QUAL_IP_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyIp6_map[] = {
    {
        .lt_field_name = "QUAL_IP_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyL2Format_map[] = {
    {
        .lt_field_name = "QUAL_L2_FORMAT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyVlanFormat_map[] = {
    {
        .lt_field_name = "QUAL_VLAN_OUTER_PRESENT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OPAQUE_TAG_PRESENT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyExtensionHeaderType_map[] = {
    {
        .lt_field_name = "QUAL_IP_FIRST_EH_PROTO",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyExtensionHeaderSubCode_map[] = {
    {
        .lt_field_name = "QUAL_IP_FIRST_EH_SUBCODE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyL4Ports_map[] = {
    {
        .lt_field_name = "QUAL_L4_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyMirrorCopy_map[] = {
    {
        .lt_field_name = "QUAL_MIRR_COPY",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyIcmpTypeCode_map[] = {
    {
        .lt_field_name = "QUAL_ICMP_TYPE_CODE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyColor_map[] = {
    {
        .lt_field_name = "QUAL_COLOR",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyIntPriority_map[] = {
    {
        .lt_field_name = "QUAL_INT_PRI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyForwardingVlanId_map[] = {
    {
        .lt_field_name = "QUAL_FWD_VLAN_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyVpn_map[] = {
    {
        .lt_field_name = "QUAL_VPN",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyEgressClass_map[] = {
    {
        .lt_field_name = "QUAL_EGR_NHOP_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyEgressClassL3Interface_map[] = {
    {
        .lt_field_name = "QUAL_EGR_L3_IIF_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyCpuQueue_map[] = {
    {
        .lt_field_name = "QUAL_CPU_COS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyIngressClassField_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_qual_bcmFieldQualifyIngressClassField_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyIngressInterfaceClassPort_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_qual_bcmFieldQualifyIngressInterfaceClassPort_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyIntCongestionNotification_map[] = {
    {
        .lt_field_name = "QUAL_INT_CN",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyIngressInterfaceClassVPort_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_qual_bcmFieldQualifyIngressInterfaceClassVPort_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyLoopBackQueue_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK_QUEUE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyLoopBackSrcGport_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK_SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyPktIsVisible_map[] = {
    {
        .lt_field_name = "QUAL_VISIBILITY_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyLoopBackCpuMasqueradePktProfile_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK_MASQUERADE_PKT_PROFILE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyLoopBackColor_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK_COLOR",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyLoopBackTrafficClass_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK_TRAFFIC_CLASS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyLoopBackPacketProcessingPort_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK_PP_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifyL2PayLoad_map[] = {
    {
        .lt_field_name = "QUAL_BYTES_0_7_AFTER_L2HEADER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_BYTES_8_15_AFTER_L2HEADER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_BYTES_16_17_AFTER_L2HEADER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {16},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifySampledPkt_map[] = {
    {
        .lt_field_name = "QUAL_SFLOW_SAMPLED",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_qual_bcmFieldQualifySrcIp6SrhValid_map[] = {
    {
        .lt_field_name = "QUAL_SRV6_SRH_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifySrcIp6_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP6_UPPER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_SRC_IP6_LOWER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyDstIp6_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP6_UPPER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_DST_IP6_LOWER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifySrcIp6High_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP6_HIGH",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyDstIp6High_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP6_HIGH",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifySrcMac_map[] = {
    {
        .lt_field_name = "QUAL_SRC_MAC",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyDstMac_map[] = {
    {
        .lt_field_name = "QUAL_DST_MAC",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifySnap_map[] = {
    {
        .lt_field_name = "QUAL_SNAP_HEADER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifySrcIp_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP4",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyDstIp_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP4",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyLlc_map[] = {
    {
        .lt_field_name = "QUAL_LLC_HEADER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyInPort_map[] = {
    {
        .lt_field_name = "QUAL_INPORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyOuterVlan_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {12},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OUTER_VLAN_CFI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OUTER_VLAN_PRI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {3},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyOuterVlanId_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyOuterVlanPri_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_PRI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyOuterVlanCfi_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_CFI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyL4SrcPort_map[] = {
    {
        .lt_field_name = "QUAL_L4SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyL4DstPort_map[] = {
    {
        .lt_field_name = "QUAL_L4DST_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyEtherType_map[] = {
    {
        .lt_field_name = "QUAL_ETHERTYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyIpProtocol_map[] = {
    {
        .lt_field_name = "QUAL_IP_PROTOCOL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyDSCP_map[] = {
    {
        .lt_field_name = "QUAL_TOS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyTtl_map[] = {
    {
        .lt_field_name = "QUAL_TTL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifySrcPort_map[] = {
    {
        .lt_field_name = "QUAL_SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifySrcTrunk_map[] = {
    {
        .lt_field_name = "QUAL_SRC_TRUNK",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyTcpControl_map[] = {
    {
        .lt_field_name = "QUAL_TCP_FLAGS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyIpInfo_map[] = {
    {
        .lt_field_name = "QUAL_IP_CHECKSUM_VALID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyIpProtocolCommon_map[] = {
    {
        .lt_field_name = "QUAL_IP_PROTO_COMMON",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyOuterTpid_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_TPID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyInnerIpType_map[] = {
    {
        .lt_field_name = "QUAL_INNER_IP_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyIpType_map[] = {
    {
        .lt_field_name = "QUAL_IP_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyHiGig_map[] = {
    {
        .lt_field_name = "QUAL_HIGIG_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyInterfaceClassL3_map[] = {
    {
        .lt_field_name = "QUAL_TNL_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyInterfaceClassPort_map[] = {
    {
        .lt_field_name = "QUAL_PORT_GRP_OR_PORT_SYSTEM_CLASS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyIpFrag_map[] = {
    {
        .lt_field_name = "QUAL_IP_FRAG",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyL2Format_map[] = {
    {
        .lt_field_name = "QUAL_L2_FORMAT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyVlanFormat_map[] = {
    {
        .lt_field_name = "QUAL_VLAN_OUTER_PRESENT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OPAQUE_TAG_PRESENT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyExtensionHeaderType_map[] = {
    {
        .lt_field_name = "QUAL_IP_FIRST_EH_PROTO",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyExtensionHeaderSubCode_map[] = {
    {
        .lt_field_name = "QUAL_IP_FIRST_EH_SUBCODE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyL4Ports_map[] = {
    {
        .lt_field_name = "QUAL_L4_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyTunnelTerminated_map[] = {
    {
        .lt_field_name = "QUAL_TNL_TERMINATED",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyInnerSrcIp_map[] = {
    {
        .lt_field_name = "QUAL_INNER_SRC_IP4",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyInnerDstIp_map[] = {
    {
        .lt_field_name = "QUAL_INNER_DST_IP4",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyInnerSrcIp6_map[] = {
    {
        .lt_field_name = "QUAL_INNER_SRC_IP6_UPPER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_INNER_SRC_IP6_LOWER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyInnerDstIp6_map[] = {
    {
        .lt_field_name = "QUAL_INNER_DST_IP6_UPPER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_INNER_DST_IP6_LOWER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyInnerSrcIp6High_map[] = {
    {
        .lt_field_name = "QUAL_INNER_SRC_IP6_HIGH",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyInnerDstIp6High_map[] = {
    {
        .lt_field_name = "QUAL_INNER_DST_IP6_HIGH",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyInnerTtl_map[] = {
    {
        .lt_field_name = "QUAL_INNER_TTL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyInnerTos_map[] = {
    {
        .lt_field_name = "QUAL_INNER_TOS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyInnerIpProtocol_map[] = {
    {
        .lt_field_name = "QUAL_INNER_IP_PROTOCOL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyInnerIpFrag_map[] = {
    {
        .lt_field_name = "QUAL_INNER_IP_FRAG",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyIcmpTypeCode_map[] = {
    {
        .lt_field_name = "QUAL_ICMP_TYPE_CODE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyInnerIpProtocolCommon_map[] = {
    {
        .lt_field_name = "QUAL_INNER_IP_PROTO_COMMON",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyInnerL4SrcPort_map[] = {
    {
        .lt_field_name = "QUAL_INNER_L4SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyInnerL4DstPort_map[] = {
    {
        .lt_field_name = "QUAL_INNER_L4DST_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifySrcGport_map[] = {
    {
        .lt_field_name = "QUAL_SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifySrcGports_map[] = {
    {
        .lt_field_name = "QUAL_SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_qual_bcmFieldQualifyOpaqueTagType_map[] = {
    {
        .lt_field_name = "QUAL_OPAQUE_TAG_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_presel_qual_bcmFieldQualifyDrop_map[] = {
    {
        .lt_field_name = "QUAL_DROP_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_presel_qual_bcmFieldQualifyPacketRes_map[] = {
    {
        .lt_field_name = "QUAL_PKT_RESOLUTION",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_presel_qual_bcmFieldQualifyTunnelType_map[] = {
    {
        .lt_field_name = "QUAL_TNL_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_presel_qual_bcmFieldQualifyIpType_map[] = {
    {
        .lt_field_name = "QUAL_IP_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_presel_qual_bcmFieldQualifyForwardingType_map[] = {
    {
        .lt_field_name = "QUAL_FWD_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_presel_qual_bcmFieldQualifyL4Ports_map[] = {
    {
        .lt_field_name = "QUAL_L4_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_presel_qual_bcmFieldQualifyForwardingVlanValid_map[] = {
    {
        .lt_field_name = "QUAL_FWD_VLAN_VALID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_presel_qual_bcmFieldQualifyIngressStpState_map[] = {
    {
        .lt_field_name = "QUAL_ING_STP_STATE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_presel_qual_bcmFieldQualifyDosAttack_map[] = {
    {
        .lt_field_name = "QUAL_DOSATTACK_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_presel_qual_bcmFieldQualifyMyStationHit_map[] = {
    {
        .lt_field_name = "QUAL_MYSTATIONTCAM_HIT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_presel_qual_bcmFieldQualifyIpTunnelHit_map[] = {
    {
        .lt_field_name = "QUAL_L3_TNL_HIT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_presel_qual_bcmFieldQualifyMplsLabel1Hit_map[] = {
    {
        .lt_field_name = "QUAL_MPLSENTRY_FIRSTLOOKUP_HIT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_presel_qual_bcmFieldQualifyMplsLabel2Hit_map[] = {
    {
        .lt_field_name = "QUAL_MPLSENTRY_SECONDLOOKUP_HIT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_presel_qual_bcmFieldQualifyMixedSrcClassId_map[] = {
    {
        .lt_field_name = "QUAL_MIXED_SRC_CLASS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_presel_qual_bcmFieldQualifyExactMatchGroupClassId_map[] = {
    {
        .lt_field_name = "QUAL_EM_FIRST_LOOKUP_LTID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_presel_qual_bcmFieldQualifyStpState_map[] = {
    {
        .lt_field_name = "QUAL_ING_STP_STATE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifySrcIp6_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP6_BITMAP_UPPER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_SRC_IP6_BITMAP_LOWER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDstIp6_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP6_BITMAP_UPPER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_DST_IP6_BITMAP_LOWER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifySrcIp6High_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP6_BITMAP_UPPER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDstIp6High_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP6_BITMAP_UPPER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifySrcIp6Low_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP6_BITMAP_LOWER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDstIp6Low_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP6_BITMAP_LOWER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifySrcMac_map[] = {
    {
        .lt_field_name = "QUAL_SRC_MAC_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDstMac_map[] = {
    {
        .lt_field_name = "QUAL_DST_MAC_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifySrcIp_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP4_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDstIp_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP4_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyInPort_map[] = {
    {
        .lt_field_name = "QUAL_INPORT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDrop_map[] = {
    {
        .lt_field_name = "QUAL_DROP_PKT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIp6FlowLabel_map[] = {
    {
        .lt_field_name = "QUAL_IP6_FLOW_LABEL_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyOuterVlan_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_ID_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {12},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OUTER_VLAN_CFI_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OUTER_VLAN_PRI_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {3},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyOuterVlanId_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_ID_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyOuterVlanPri_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_PRI_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyOuterVlanCfi_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_CFI_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyRangeCheck_map[] = {
    {
        .lt_field_name = "QUAL_RANGE_CHECKID_BMP_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyL4SrcPort_map[] = {
    {
        .lt_field_name = "QUAL_L4SRC_PORT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyL4DstPort_map[] = {
    {
        .lt_field_name = "QUAL_L4DST_PORT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyEtherType_map[] = {
    {
        .lt_field_name = "QUAL_ETHERTYPE_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIpProtocol_map[] = {
    {
        .lt_field_name = "QUAL_IP_PROTOCOL_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDSCP_map[] = {
    {
        .lt_field_name = "QUAL_TOS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyTtl_map[] = {
    {
        .lt_field_name = "QUAL_TTL_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifySrcPort_map[] = {
    {
        .lt_field_name = "QUAL_SRC_PORT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifySrcTrunk_map[] = {
    {
        .lt_field_name = "QUAL_SRC_TRUNK_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDstPort_map[] = {
    {
        .lt_field_name = "QUAL_DST_PORT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDstTrunk_map[] = {
    {
        .lt_field_name = "QUAL_DST_TRUNK_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyTcpControl_map[] = {
    {
        .lt_field_name = "QUAL_TCP_FLAGS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyPacketRes_map[] = {
    {
        .lt_field_name = "QUAL_PKT_RESOLUTION_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifySrcClassL2_map[] = {
    {
        .lt_field_name = "QUAL_L2_SRC_CLASS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifySrcClassL3_map[] = {
    {
        .lt_field_name = "QUAL_L3_SRC_CLASS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifySrcClassField_map[] = {
    {
        .lt_field_name = "QUAL_FP_VLAN_CLASS1_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDstClassL2_map[] = {
    {
        .lt_field_name = "QUAL_L2_DST_CLASS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDstClassL3_map[] = {
    {
        .lt_field_name = "QUAL_L3_DST_CLASS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDstClassField_map[] = {
    {
        .lt_field_name = "QUAL_FP_VLAN_CLASS0_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIpInfo_map[] = {
    {
        .lt_field_name = "QUAL_IP_CHECKSUM_VALID_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyMHOpcode_map[] = {
    {
        .lt_field_name = "QUAL_MH_OPCODE_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIpFlags_map[] = {
    {
        .lt_field_name = "QUAL_IP_FLAGS_MF_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_IP_FLAGS_DF_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyInnerTpid_map[] = {
    {
        .lt_field_name = "QUAL_INNER_TPID_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyOuterTpid_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_TPID_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyLoopbackType_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK_TYPE_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyLoopback_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIpType_map[] = {
    {
        .lt_field_name = "QUAL_IP_TYPE_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyForwardingType_map[] = {
    {
        .lt_field_name = "QUAL_FWD_TYPE_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyInterfaceClassL2_map[] = {
    {
        .lt_field_name = "QUAL_VLAN_CLASS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyInterfaceClassL3_map[] = {
    {
        .lt_field_name = "QUAL_L3_IIF_CLASS_ID_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyInterfaceClassPort_map[] = {
    {
        .lt_field_name = "QUAL_PORT_SYSTEM_CLASS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyL3Routable_map[] = {
    {
        .lt_field_name = "QUAL_L3_ROUTABLE_PKT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIpFrag_map[] = {
    {
        .lt_field_name = "QUAL_IP_FRAG_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyVrf_map[] = {
    {
        .lt_field_name = "QUAL_VRF_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyL3Ingress_map[] = {
    {
        .lt_field_name = "QUAL_L3_IIF_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyL2Format_map[] = {
    {
        .lt_field_name = "QUAL_L2_FORMAT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyVlanFormat_map[] = {
    {
        .lt_field_name = "QUAL_VLAN_OUTER_PRESENT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OPAQUE_TAG_PRESENT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyExtensionHeaderType_map[] = {
    {
        .lt_field_name = "QUAL_IP_FIRST_EH_PROTO_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyExtensionHeaderSubCode_map[] = {
    {
        .lt_field_name = "QUAL_IP_FIRST_EH_SUBCODE_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyExtensionHeader2Type_map[] = {
    {
        .lt_field_name = "QUAL_IP_SECOND_EH_PROTO_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyL4Ports_map[] = {
    {
        .lt_field_name = "QUAL_L4_PKT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyMplsTerminated_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_BOS_TERMINATED_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyForwardingVlanValid_map[] = {
    {
        .lt_field_name = "QUAL_FWD_VLAN_VALID_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIngressStpState_map[] = {
    {
        .lt_field_name = "QUAL_ING_STP_STATE_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyL2SrcHit_map[] = {
    {
        .lt_field_name = "QUAL_L2_SRC_HIT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyL2SrcStatic_map[] = {
    {
        .lt_field_name = "QUAL_L2_SRC_STATIC_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyL2DestHit_map[] = {
    {
        .lt_field_name = "QUAL_L2_DST_HIT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyL2StationMove_map[] = {
    {
        .lt_field_name = "QUAL_L2STATION_MOVE_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyL2CacheHit_map[] = {
    {
        .lt_field_name = "QUAL_L2CACHE_HIT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyL3DestRouteHit_map[] = {
    {
        .lt_field_name = "QUAL_L3_DST_LPM_HIT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDosAttack_map[] = {
    {
        .lt_field_name = "QUAL_DOSATTACK_PKT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIcmpTypeCode_map[] = {
    {
        .lt_field_name = "QUAL_ICMP_TYPE_CODE_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDstL3Egress_map[] = {
    {
        .lt_field_name = "QUAL_NHOP_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyColor_map[] = {
    {
        .lt_field_name = "QUAL_COLOR_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIntPriority_map[] = {
    {
        .lt_field_name = "QUAL_INT_PRI_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyForwardingVlanId_map[] = {
    {
        .lt_field_name = "QUAL_FWD_VLAN_ID_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyVpn_map[] = {
    {
        .lt_field_name = "QUAL_VPN_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyMyStationHit_map[] = {
    {
        .lt_field_name = "QUAL_MYSTATIONTCAM_HIT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDstMultipath_map[] = {
    {
        .lt_field_name = "QUAL_OVERLAY_ECMP_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyL2PayloadFirstEightBytes_map[] = {
    {
        .lt_field_name = "QUAL_BYTES_0_7_AFTER_L2HEADER_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDstL3EgressNextHops_map[] = {
    {
        .lt_field_name = "QUAL_NHOP_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyRecoverableDrop_map[] = {
    {
        .lt_field_name = "QUAL_PROTECTION_DATA_DROP_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIpTunnelHit_map[] = {
    {
        .lt_field_name = "QUAL_L3_TNL_HIT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyMplsLabel1Hit_map[] = {
    {
        .lt_field_name = "QUAL_MPLSENTRY_FIRSTLOOKUP_HIT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyMplsLabel2Hit_map[] = {
    {
        .lt_field_name = "QUAL_MPLSENTRY_SECONDLOOKUP_HIT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyGenericAssociatedChannelLabelValid_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_SPECIAL_LABEL_GAL_DETECTED_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyRouterAlertLabelValid_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_SPECIAL_LABEL_RAL_DETECTED_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyNormalizeIpAddrs_map[] = {
    {
        .lt_field_name = "QUAL_IPADDR_NORMALIZE_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyNormalizeMacAddrs_map[] = {
    {
        .lt_field_name = "QUAL_MACADDR_NORMALIZE_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyMplsForwardingLabel_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_ID_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_EXP_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_BOS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_TTL_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyMplsForwardingLabelTtl_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_TTL_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyMplsForwardingLabelBos_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_BOS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyMplsForwardingLabelExp_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_EXP_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyMplsForwardingLabelId_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_ID_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyMplsControlWord_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_CTRL_WORD_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyRtag7AHashUpper_map[] = {
    {
        .lt_field_name = "QUAL_RTAG7A_HASH_UPPER_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyRtag7AHashLower_map[] = {
    {
        .lt_field_name = "QUAL_RTAG7A_HASH_LOWER_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyRtag7BHashUpper_map[] = {
    {
        .lt_field_name = "QUAL_RTAG7B_HASH_UPPER_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyRtag7BHashLower_map[] = {
    {
        .lt_field_name = "QUAL_RTAG7B_HASH_LOWER_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyMplsForwardingLabelAction_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_FWD_LABEL_ACTION_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyMplsControlWordValid_map[] = {
    {
        .lt_field_name = "QUAL_MPLS_SPECIAL_LABEL_CW_DETECTED_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifySrcGport_map[] = {
    {
        .lt_field_name = "QUAL_SRC_PORT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyL3SrcRouteHit_map[] = {
    {
        .lt_field_name = "QUAL_L3_SRC_LPM_HIT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyVxlanNetworkId_map[] = {
    {
        .lt_field_name = "QUAL_VXLAN_VNID_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyVxlanFlags_map[] = {
    {
        .lt_field_name = "QUAL_VXLAN_FLAGS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIcmpError_map[] = {
    {
        .lt_field_name = "QUAL_ICMP_ERROR_PKT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIntCongestionNotification_map[] = {
    {
        .lt_field_name = "QUAL_INT_CN_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifySrcGports_map[] = {
    {
        .lt_field_name = "QUAL_SRC_PORT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyVxlanHeaderBits8_31_map[] = {
    {
        .lt_field_name = "QUAL_VXLAN_RSVD_24_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyVxlanHeaderBits56_63_map[] = {
    {
        .lt_field_name = "QUAL_VXLAN_RSVD_8_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIpProtocolClass_map[] = {
    {
        .lt_field_name = "QUAL_C_IP_PROTOCOL_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyEtherTypeClass_map[] = {
    {
        .lt_field_name = "QUAL_C_ETHERTYPE_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyTcpClassZero_map[] = {
    {
        .lt_field_name = "QUAL_C_TCP_FLAGS0_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyTosClassZero_map[] = {
    {
        .lt_field_name = "QUAL_C_TOS0_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyTtlClassZero_map[] = {
    {
        .lt_field_name = "QUAL_C_TTL0_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyTcpClassOne_map[] = {
    {
        .lt_field_name = "QUAL_C_TCP_FLAGS1_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyTosClassOne_map[] = {
    {
        .lt_field_name = "QUAL_C_TOS1_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyTtlClassOne_map[] = {
    {
        .lt_field_name = "QUAL_C_TTL1_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIpFragNonOrFirst_map[] = {
    {
        .lt_field_name = "QUAL_NON_OR_FIRST_FRAGMENT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyPacketLength_map[] = {
    {
        .lt_field_name = "QUAL_PKT_LENGTH_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyGroupClass_map[] = {
    {
        .lt_field_name = "QUAL_PRESEL_CLASS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyUdfClass_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CLASS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyInterfaceLookupClassPort_map[] = {
    {
        .lt_field_name = "QUAL_FP_VLAN_PORT_GRP_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyInterfaceIngressKeySelectClassPort_map[] = {
    {
        .lt_field_name = "QUAL_FP_ING_GRP_SEL_CLASS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyBfdYourDiscriminator_map[] = {
    {
        .lt_field_name = "QUAL_BFD_DISCRIMINATOR_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDstL2MulticastGroup_map[] = {
    {
        .lt_field_name = "QUAL_L2_MC_GROUP_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDstL3MulticastGroup_map[] = {
    {
        .lt_field_name = "QUAL_L3_MC_GROUP_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyExactMatchActionClassId_map[] = {
    {
        .lt_field_name = "QUAL_EM_FIRST_LOOKUP_CLASS_ID_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyExactMatchGroupClassId_map[] = {
    {
        .lt_field_name = "QUAL_EM_GROUP_CLASS_ID_0_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyL2PayLoad_map[] = {
    {
        .lt_field_name = "QUAL_BYTES_0_7_AFTER_L2HEADER_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_BYTES_8_15_AFTER_L2HEADER_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_BYTES_16_23_AFTER_L2HEADER_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_BYTES_24_31_AFTER_L2HEADER_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {64},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyStpState_map[] = {
    {
        .lt_field_name = "QUAL_ING_STP_STATE_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifySystemPortBitmap_map[] = {
    {
        .lt_field_name = "QUAL_SYSTEM_PORTS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyDevicePortBitmap_map[] = {
    {
        .lt_field_name = "QUAL_DEVICE_PORTS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = BCMI_FIELD_STAGE_LTMAP_IFP_ONLY,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIntPktLengthValid_map[] = {
    {
        .lt_field_name = "QUAL_INBAND_TELEMETRY_MAX_LENGTH_CHECK_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIntPktFinalHop_map[] = {
    {
        .lt_field_name = "QUAL_INBAND_TELEMETRY_HOP_LIMIT_CHECK_STATUS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIpTunnelTtl_map[] = {
    {
        .lt_field_name = "QUAL_TNL_IP_TTL_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIpTunnelTtlClassZero_map[] = {
    {
        .lt_field_name = "QUAL_C_TNL_IP_TTL_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyRangeCheckGroup_map[] = {
    {
        .lt_field_name = "QUAL_RANGE_CHECK_GRP_ID_BMP_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyOpaqueTagHigh_map[] = {
    {
        .lt_field_name = "QUAL_OPAQUE_TAG_HIGH_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyOpaqueTagLow_map[] = {
    {
        .lt_field_name = "QUAL_OPAQUE_TAG_LOW_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIfaHopLimitEqualToZero_map[] = {
    {
        .lt_field_name = "QUAL_INBAND_TELEMETRY_HOP_LIMIT_CHECK_STATUS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIntFlags_map[] = {
    {
        .lt_field_name = "QUAL_INBAND_TELEMETRY_FLAGS_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIntHdrType_map[] = {
    {
        .lt_field_name = "QUAL_INBAND_TELEMETRY_HDR_TYPE_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIntUdpChecksumEqualsZero_map[] = {
    {
        .lt_field_name = "QUAL_INBAND_TELEMETRY_UDP_CHECKSUM_ZERO_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIntMetadataHdrType_map[] = {
    {
        .lt_field_name = "QUAL_INBAND_TELEMETRY_MD_HDR_TYPE_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyIntHdrPresent_map[] = {
    {
        .lt_field_name = "QUAL_INBAND_TELEMETRY_HDR_IN_TNL_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyCompressionSrcHit_map[] = {
    {
        .lt_field_name = "QUAL_COMPRESSED_L3_SRC_HIT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_ifp_group_qual_bcmFieldQualifyCompressionDstHit_map[] = {
    {
        .lt_field_name = "QUAL_COMPRESSED_L3_DST_HIT_BITMAP",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifySrcIp6_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP6",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyDstIp6_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP6",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifySrcIp6High_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP6_HIGH",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyDstIp6High_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP6_HIGH",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifySrcMac_map[] = {
    {
        .lt_field_name = "QUAL_SRC_MAC",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyDstMac_map[] = {
    {
        .lt_field_name = "QUAL_DST_MAC",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifySrcIp_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP4",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyDstIp_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP4",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyInPort_map[] = {
    {
        .lt_field_name = "QUAL_INPORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyDrop_map[] = {
    {
        .lt_field_name = "QUAL_DROP_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyOuterVlan_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OUTER_VLAN_CFI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OUTER_VLAN_PRI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyOuterVlanId_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyOuterVlanPri_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_PRI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyOuterVlanCfi_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_CFI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyL4SrcPort_map[] = {
    {
        .lt_field_name = "QUAL_L4SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyL4DstPort_map[] = {
    {
        .lt_field_name = "QUAL_L4DST_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyEtherType_map[] = {
    {
        .lt_field_name = "QUAL_ETHERTYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyIpProtocol_map[] = {
    {
        .lt_field_name = "QUAL_IP_PROTOCOL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyDSCP_map[] = {
    {
        .lt_field_name = "QUAL_TOS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyTtl_map[] = {
    {
        .lt_field_name = "QUAL_TTL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyTcpControl_map[] = {
    {
        .lt_field_name = "QUAL_TCP_FLAGS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifySrcClassL2_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_group_qual_bcmFieldQualifySrcClassL2_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifySrcClassL3_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_group_qual_bcmFieldQualifySrcClassL3_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifySrcClassField_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_group_qual_bcmFieldQualifySrcClassField_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyDstClassL2_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_group_qual_bcmFieldQualifyDstClassL2_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyDstClassL3_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_group_qual_bcmFieldQualifyDstClassL3_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyDstClassField_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_group_qual_bcmFieldQualifyDstClassField_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyLoopbackType_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyIpType_map[] = {
    {
        .lt_field_name = "QUAL_IP_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyForwardingType_map[] = {
    {
        .lt_field_name = "QUAL_FWD_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyInterfaceClassL3_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_group_qual_bcmFieldQualifyInterfaceClassL3_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyInterfaceClassPort_map[] = {
    {
        .lt_field_name = "QUAL_PORT_GRP_OR_PORT_SYSTEM_CLASS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyL3Routable_map[] = {
    {
        .lt_field_name = "QUAL_L3_ROUTABLE_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyIpFrag_map[] = {
    {
        .lt_field_name = "QUAL_IP_FRAG",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyVrf_map[] = {
    {
        .lt_field_name = "QUAL_VRF",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyOutPort_map[] = {
    {
        .lt_field_name = "QUAL_OUTPORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyIp4_map[] = {
    {
        .lt_field_name = "QUAL_IP_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyIp6_map[] = {
    {
        .lt_field_name = "QUAL_IP_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyL2Format_map[] = {
    {
        .lt_field_name = "QUAL_L2_FORMAT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyVlanFormat_map[] = {
    {
        .lt_field_name = "QUAL_VLAN_OUTER_PRESENT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OPAQUE_TAG_PRESENT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyExtensionHeaderType_map[] = {
    {
        .lt_field_name = "QUAL_IP_FIRST_EH_PROTO",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyExtensionHeaderSubCode_map[] = {
    {
        .lt_field_name = "QUAL_IP_FIRST_EH_SUBCODE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyL4Ports_map[] = {
    {
        .lt_field_name = "QUAL_L4_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyMirrorCopy_map[] = {
    {
        .lt_field_name = "QUAL_MIRR_COPY",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyIcmpTypeCode_map[] = {
    {
        .lt_field_name = "QUAL_ICMP_TYPE_CODE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyColor_map[] = {
    {
        .lt_field_name = "QUAL_COLOR",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyIntPriority_map[] = {
    {
        .lt_field_name = "QUAL_INT_PRI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyForwardingVlanId_map[] = {
    {
        .lt_field_name = "QUAL_FWD_VLAN_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyVpn_map[] = {
    {
        .lt_field_name = "QUAL_VPN",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyEgressClass_map[] = {
    {
        .lt_field_name = "QUAL_EGR_NHOP_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyEgressClassL3Interface_map[] = {
    {
        .lt_field_name = "QUAL_EGR_L3_IIF_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyCpuQueue_map[] = {
    {
        .lt_field_name = "QUAL_CPU_COS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyIngressClassField_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_group_qual_bcmFieldQualifyIngressClassField_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyIngressInterfaceClassPort_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_group_qual_bcmFieldQualifyIngressInterfaceClassPort_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyIntCongestionNotification_map[] = {
    {
        .lt_field_name = "QUAL_INT_CN",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyIngressInterfaceClassVPort_map[] = {
    {
        .lt_field_name = "QUAL_ING_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 1,
        .internal_map = bcm_field_efp_group_qual_bcmFieldQualifyIngressInterfaceClassVPort_QUAL_ING_CLASS_ID_internal_map,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyLoopBackQueue_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK_QUEUE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyLoopBackSrcGport_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK_SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyPktIsVisible_map[] = {
    {
        .lt_field_name = "QUAL_VISIBILITY_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyLoopBackCpuMasqueradePktProfile_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK_MASQUERADE_PKT_PROFILE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyLoopBackColor_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK_COLOR",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyLoopBackTrafficClass_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK_TRAFFIC_CLASS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyLoopBackPacketProcessingPort_map[] = {
    {
        .lt_field_name = "QUAL_LOOPBACK_PP_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifyL2PayLoad_map[] = {
    {
        .lt_field_name = "QUAL_BYTES_0_7_AFTER_L2HEADER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_BYTES_8_15_AFTER_L2HEADER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_BYTES_16_17_AFTER_L2HEADER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifySampledPkt_map[] = {
    {
        .lt_field_name = "QUAL_SFLOW_SAMPLED",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_efp_group_qual_bcmFieldQualifySrcIp6SrhValid_map[] = {
    {
        .lt_field_name = "QUAL_SRV6_SRH_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifySrcIp6_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP6",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyDstIp6_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP6",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifySrcIp6High_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP6_HIGH",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyDstIp6High_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP6_HIGH",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifySrcMac_map[] = {
    {
        .lt_field_name = "QUAL_SRC_MAC",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyDstMac_map[] = {
    {
        .lt_field_name = "QUAL_DST_MAC",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifySnap_map[] = {
    {
        .lt_field_name = "QUAL_SNAP_HEADER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifySrcIp_map[] = {
    {
        .lt_field_name = "QUAL_SRC_IP4",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyDstIp_map[] = {
    {
        .lt_field_name = "QUAL_DST_IP4",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyLlc_map[] = {
    {
        .lt_field_name = "QUAL_LLC_HEADER",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyInPort_map[] = {
    {
        .lt_field_name = "QUAL_INPORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyOuterVlan_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OUTER_VLAN_CFI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OUTER_VLAN_PRI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyOuterVlanId_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyOuterVlanPri_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_PRI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyOuterVlanCfi_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_VLAN_CFI",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyL4SrcPort_map[] = {
    {
        .lt_field_name = "QUAL_L4SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyL4DstPort_map[] = {
    {
        .lt_field_name = "QUAL_L4DST_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyEtherType_map[] = {
    {
        .lt_field_name = "QUAL_ETHERTYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyIpProtocol_map[] = {
    {
        .lt_field_name = "QUAL_IP_PROTOCOL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyDSCP_map[] = {
    {
        .lt_field_name = "QUAL_TOS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyTtl_map[] = {
    {
        .lt_field_name = "QUAL_TTL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifySrcPort_map[] = {
    {
        .lt_field_name = "QUAL_SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifySrcTrunk_map[] = {
    {
        .lt_field_name = "QUAL_SRC_TRUNK",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyTcpControl_map[] = {
    {
        .lt_field_name = "QUAL_TCP_FLAGS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyIpInfo_map[] = {
    {
        .lt_field_name = "QUAL_IP_CHECKSUM_VALID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyIpProtocolCommon_map[] = {
    {
        .lt_field_name = "QUAL_IP_PROTO_COMMON",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyOuterTpid_map[] = {
    {
        .lt_field_name = "QUAL_OUTER_TPID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyInnerIpType_map[] = {
    {
        .lt_field_name = "QUAL_INNER_IP_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyIpType_map[] = {
    {
        .lt_field_name = "QUAL_IP_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyHiGig_map[] = {
    {
        .lt_field_name = "QUAL_HIGIG_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyInterfaceClassL3_map[] = {
    {
        .lt_field_name = "QUAL_TNL_CLASS_ID",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyInterfaceClassPort_map[] = {
    {
        .lt_field_name = "QUAL_PORT_GRP_OR_PORT_SYSTEM_CLASS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyIpFrag_map[] = {
    {
        .lt_field_name = "QUAL_IP_FRAG",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyL2Format_map[] = {
    {
        .lt_field_name = "QUAL_L2_FORMAT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyVlanFormat_map[] = {
    {
        .lt_field_name = "QUAL_VLAN_OUTER_PRESENT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "QUAL_OPAQUE_TAG_PRESENT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyExtensionHeaderType_map[] = {
    {
        .lt_field_name = "QUAL_IP_FIRST_EH_PROTO",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyExtensionHeaderSubCode_map[] = {
    {
        .lt_field_name = "QUAL_IP_FIRST_EH_SUBCODE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyL4Ports_map[] = {
    {
        .lt_field_name = "QUAL_L4_PKT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyTunnelTerminated_map[] = {
    {
        .lt_field_name = "QUAL_TNL_TERMINATED",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyInnerSrcIp_map[] = {
    {
        .lt_field_name = "QUAL_INNER_SRC_IP4",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyInnerDstIp_map[] = {
    {
        .lt_field_name = "QUAL_INNER_DST_IP4",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyInnerSrcIp6_map[] = {
    {
        .lt_field_name = "QUAL_INNER_SRC_IP6",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyInnerDstIp6_map[] = {
    {
        .lt_field_name = "QUAL_INNER_DST_IP6",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyInnerSrcIp6High_map[] = {
    {
        .lt_field_name = "QUAL_INNER_SRC_IP6_HIGH",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyInnerDstIp6High_map[] = {
    {
        .lt_field_name = "QUAL_INNER_DST_IP6_HIGH",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyInnerTtl_map[] = {
    {
        .lt_field_name = "QUAL_INNER_TTL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyInnerTos_map[] = {
    {
        .lt_field_name = "QUAL_INNER_TOS",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyInnerIpProtocol_map[] = {
    {
        .lt_field_name = "QUAL_INNER_IP_PROTOCOL",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyInnerIpFrag_map[] = {
    {
        .lt_field_name = "QUAL_INNER_IP_FRAG",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyIcmpTypeCode_map[] = {
    {
        .lt_field_name = "QUAL_ICMP_TYPE_CODE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyInnerIpProtocolCommon_map[] = {
    {
        .lt_field_name = "QUAL_INNER_IP_PROTO_COMMON",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyInnerL4SrcPort_map[] = {
    {
        .lt_field_name = "QUAL_INNER_L4SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyInnerL4DstPort_map[] = {
    {
        .lt_field_name = "QUAL_INNER_L4DST_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifySrcGport_map[] = {
    {
        .lt_field_name = "QUAL_SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifySrcGports_map[] = {
    {
        .lt_field_name = "QUAL_SRC_PORT",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static bcm_field_qual_map_info_t bcm_field_vfp_group_qual_bcmFieldQualifyOpaqueTagType_map[] = {
    {
        .lt_field_name = "QUAL_OPAQUE_TAG_TYPE",
        .lt_field_mask_name = NULL,
        .lt_group_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .em = 0,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_internal_map_info_t bcm_field_ifp_action_bcmFieldActionRedirectPbmp_ACTION_FP_ING_ADD_REDIRECT_DATA_ID_internal_map[] = {
    {
        .lt_field_name = "ACTION_REDIRECT_PORTS_BROADCAST_PKT",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .sbr_enabled = FALSE,
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = 1,
    },
};

static const bcm_field_action_internal_map_info_t bcm_field_ifp_action_bcmFieldActionRedirectBcastPbmp_ACTION_FP_ING_ADD_REDIRECT_DATA_ID_internal_map[] = {
    {
        .lt_field_name = "ACTION_REDIRECT_PORTS_VLAN_BROADCAST_FP_ING",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .sbr_enabled = FALSE,
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = 1,
    },
};

static const bcm_field_action_internal_map_info_t bcm_field_ifp_action_bcmFieldActionEgressMask_ACTION_FP_ING_REMOVE_REDIRECT_DATA_ID_internal_map[] = {
    {
        .lt_field_name = "ACTION_REDIRECT_REMOVE_PORTS_BROADCAST",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .sbr_enabled = FALSE,
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = 1,
    },
};

static const bcm_field_action_internal_map_info_t bcm_field_ifp_action_bcmFieldActionEgressPortsAdd_ACTION_FP_ING_ADD_REDIRECT_DATA_ID_internal_map[] = {
    {
        .lt_field_name = "ACTION_REDIRECT_ADD_PORTS_BROADCAST",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .sbr_enabled = FALSE,
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = 1,
    },
};

static const bcm_field_action_internal_map_info_t bcm_field_ifp_action_bcmFieldActionMirrorIngress_ACTION_MIRROR_INSTANCE_ID_internal_map[] = {
    {
        .lt_field_name = "ACTION_MIRROR_INSTANCE_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .sbr_enabled = FALSE,
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = 1,
    },
};

static const bcm_field_action_internal_map_info_t bcm_field_ifp_action_bcmFieldActionMirrorEgress_ACTION_MIRROR_INSTANCE_ID_internal_map[] = {
    {
        .lt_field_name = "ACTION_MIRROR_INSTANCE_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .sbr_enabled = FALSE,
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = 1,
    },
};

static const bcm_field_action_internal_map_info_t bcm_field_ifp_action_bcmFieldActionStatGroup_CTR_ING_EFLEX_OBJECT_internal_map[] = {
    {
        .lt_field_name = "ACTION_FLEX_CTR_R_COUNT",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .sbr_enabled = FALSE,
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = 1,
    },
    {
        .lt_field_name = "ACTION_FLEX_CTR_Y_COUNT",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .sbr_enabled = FALSE,
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = 1,
    },
    {
        .lt_field_name = "ACTION_FLEX_CTR_G_COUNT",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .sbr_enabled = FALSE,
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = 1,
    },
};

static const bcm_field_action_internal_map_info_t bcm_field_ifp_action_bcmFieldActionMatchPbmpDrop_ACTION_FP_DELAYED_DROP_ID_internal_map[] = {
    {
        .lt_field_name = "ACTION_DELAYED_DROP_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .sbr_enabled = FALSE,
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = 1,
    },
};

static const bcm_field_action_internal_map_info_t bcm_field_ifp_action_bcmFieldActionMatchPbmpRedirect_ACTION_DELAYED_REDIRECT_PORT_internal_map[] = {
    {
        .lt_field_name = "ACTION_DELAYED_REDIRECT_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .sbr_enabled = FALSE,
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = 1,
    },
};

static const bcm_field_action_internal_map_info_t bcm_field_ifp_action_bcmFieldActionMirrorFlowClass_ACTION_MIRROR_FLOW_CLASS_internal_map[] = {
    {
        .lt_field_name = "ACTION_MIRROR_FLOW_CLASS_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .sbr_enabled = FALSE,
        .num_offsets = 0,
        .offset = {},
        .width = {},
        .value = 1,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionCosQNew_map[] = {
    {
        .lt_field_name = "ACTION_G_NEW_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {4},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_NEW_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {4},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_NEW_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {4},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionCosQCpuNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_CPU_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionUcastCosQNew_map[] = {
    {
        .lt_field_name = "ACTION_G_NEW_UC_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {3},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_NEW_UC_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {3},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_NEW_UC_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {3},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionMcastCosQNew_map[] = {
    {
        .lt_field_name = "ACTION_G_NEW_MC_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {3},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_NEW_MC_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {3},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_NEW_MC_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {3},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionPrioPktNew_map[] = {
    {
        .lt_field_name = "ACTION_G_NEW_OUTER_DOT1P",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {3},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_NEW_OUTER_DOT1P",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {3},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_NEW_OUTER_DOT1P",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {3},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionPrioPktTos_map[] = {
    {
        .lt_field_name = "ACTION_G_OUTER_DOT1P_TO_TOS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_OUTER_DOT1P_TO_TOS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_OUTER_DOT1P_TO_TOS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionPrioPktCancel_map[] = {
    {
        .lt_field_name = "ACTION_R_DOT1P_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_DOT1P_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_G_DOT1P_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionPrioIntNew_map[] = {
    {
        .lt_field_name = "ACTION_G_NEW_INTPRI",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {4},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_NEW_INTPRI",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {4},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_NEW_INTPRI",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {4},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionPrioIntTos_map[] = {
    {
        .lt_field_name = "ACTION_G_INTPRI_TO_TOS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_INTPRI_TO_TOS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_INTPRI_TO_TOS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionPrioIntCancel_map[] = {
    {
        .lt_field_name = "ACTION_G_INTPRI_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_INTPRI_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_INTPRI_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionDscpNew_map[] = {
    {
        .lt_field_name = "ACTION_R_NEW_DSCP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {6},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_NEW_DSCP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {6},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_G_NEW_DSCP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {6},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionDscpCancel_map[] = {
    {
        .lt_field_name = "ACTION_G_DSCP_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_DSCP_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_DSCP_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionEcnNew_map[] = {
    {
        .lt_field_name = "ACTION_R_NEW_ECN",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {2},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_G_NEW_ECN",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {2},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_NEW_ECN",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {2},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionCopyToCpu_map[] = {
    {
        .lt_field_name = "ACTION_G_COPY_TO_CPU",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_COPY_TO_CPU",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_COPY_TO_CPU",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_MATCH_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {8},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParam1,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionCopyToCpuCancel_map[] = {
    {
        .lt_field_name = "ACTION_G_COPY_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_COPY_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_COPY_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_G_COPY_AND_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_COPY_AND_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_COPY_AND_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_G_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionSwitchToCpuCancel_map[] = {
    {
        .lt_field_name = "ACTION_G_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_G_COPY_AND_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_COPY_AND_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_COPY_AND_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_G_COPY_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_COPY_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_COPY_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionSwitchToCpuReinstate_map[] = {
    {
        .lt_field_name = "ACTION_G_SWITCH_TO_CPU_REINSATE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_SWITCH_TO_CPU_REINSATE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_SWITCH_TO_CPU_REINSATE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionTimeStampToCpu_map[] = {
    {
        .lt_field_name = "ACTION_G_COPY_TO_CPU_WITH_TIMESTAMP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_COPY_TO_CPU_WITH_TIMESTAMP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_COPY_TO_CPU_WITH_TIMESTAMP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_MATCH_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {8},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParam1,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRedirectPort_map[] = {
    {
        .lt_field_name = "ACTION_REDIRECT_TO_PORT",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRedirectTrunk_map[] = {
    {
        .lt_field_name = "ACTION_REDIRECT_TO_TRUNK",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRedirectCancel_map[] = {
    {
        .lt_field_name = "ACTION_REDIRECT_UC_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRedirectPbmp_map[] = {
    {
        .lt_field_name = "ACTION_FP_ING_ADD_REDIRECT_DATA_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 1,
        .internal_map = bcm_field_ifp_action_bcmFieldActionRedirectPbmp_ACTION_FP_ING_ADD_REDIRECT_DATA_ID_internal_map,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRedirectIpmc_map[] = {
    {
        .lt_field_name = "ACTION_REDIRECT_L3_MC_NHOP_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRedirectMcast_map[] = {
    {
        .lt_field_name = "ACTION_REDIRECT_L2_MC_GROUP_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRedirectVlan_map[] = {
    {
        .lt_field_name = "ACTION_REDIRECT_PORTS_VLAN_BROADCAST",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRedirectBcastPbmp_map[] = {
    {
        .lt_field_name = "ACTION_FP_ING_ADD_REDIRECT_DATA_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 1,
        .internal_map = bcm_field_ifp_action_bcmFieldActionRedirectBcastPbmp_ACTION_FP_ING_ADD_REDIRECT_DATA_ID_internal_map,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionEgressMask_map[] = {
    {
        .lt_field_name = "ACTION_FP_ING_REMOVE_REDIRECT_DATA_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 1,
        .internal_map = bcm_field_ifp_action_bcmFieldActionEgressMask_ACTION_FP_ING_REMOVE_REDIRECT_DATA_ID_internal_map,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionEgressPortsAdd_map[] = {
    {
        .lt_field_name = "ACTION_FP_ING_ADD_REDIRECT_DATA_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 1,
        .internal_map = bcm_field_ifp_action_bcmFieldActionEgressPortsAdd_ACTION_FP_ING_ADD_REDIRECT_DATA_ID_internal_map,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionDrop_map[] = {
    {
        .lt_field_name = "ACTION_R_DROP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_DROP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_G_DROP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionDropCancel_map[] = {
    {
        .lt_field_name = "ACTION_R_DROP_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_DROP_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_G_DROP_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionMirrorOverride_map[] = {
    {
        .lt_field_name = "ACTION_MIRROR_OVERRIDE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionMirrorIngress_map[] = {
    {
        .lt_field_name = "ACTION_MIRROR_INSTANCE_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 1,
        .internal_map = bcm_field_ifp_action_bcmFieldActionMirrorIngress_ACTION_MIRROR_INSTANCE_ID_internal_map,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionMirrorEgress_map[] = {
    {
        .lt_field_name = "ACTION_MIRROR_INSTANCE_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 1,
        .internal_map = bcm_field_ifp_action_bcmFieldActionMirrorEgress_ACTION_MIRROR_INSTANCE_ID_internal_map,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionL3Switch_map[] = {
    {
        .lt_field_name = "ACTION_SWITCH_TO_ECMP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {12},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_SWITCH_TO_L3UC",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {15},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionL3SwitchCancel_map[] = {
    {
        .lt_field_name = "ACTION_L3_SWITCH_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionDropPrecedence_map[] = {
    {
        .lt_field_name = "ACTION_R_NEW_COLOR",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {2},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_NEW_COLOR",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {2},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_G_NEW_COLOR",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {2},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpDrop_map[] = {
    {
        .lt_field_name = "ACTION_R_DROP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpDropCancel_map[] = {
    {
        .lt_field_name = "ACTION_R_DROP_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpDropPrecedence_map[] = {
    {
        .lt_field_name = "ACTION_R_NEW_COLOR",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpCopyToCpu_map[] = {
    {
        .lt_field_name = "ACTION_R_COPY_TO_CPU",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_MATCH_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {8},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParam1,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpCopyToCpuCancel_map[] = {
    {
        .lt_field_name = "ACTION_R_COPY_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_COPY_AND_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpSwitchToCpuCancel_map[] = {
    {
        .lt_field_name = "ACTION_R_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_COPY_AND_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_COPY_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpSwitchToCpuReinstate_map[] = {
    {
        .lt_field_name = "ACTION_R_SWITCH_TO_CPU_REINSATE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpTimeStampToCpu_map[] = {
    {
        .lt_field_name = "ACTION_R_COPY_TO_CPU_WITH_TIMESTAMP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_MATCH_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {8},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParam1,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpDscpNew_map[] = {
    {
        .lt_field_name = "ACTION_R_NEW_DSCP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpDscpCancel_map[] = {
    {
        .lt_field_name = "ACTION_R_DSCP_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpEcnNew_map[] = {
    {
        .lt_field_name = "ACTION_R_NEW_ECN",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpPrioPktNew_map[] = {
    {
        .lt_field_name = "ACTION_R_NEW_OUTER_DOT1P",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpPrioPktTos_map[] = {
    {
        .lt_field_name = "ACTION_R_OUTER_DOT1P_TO_TOS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpPrioPktCancel_map[] = {
    {
        .lt_field_name = "ACTION_R_DOT1P_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpCosQNew_map[] = {
    {
        .lt_field_name = "ACTION_R_NEW_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpUcastCosQNew_map[] = {
    {
        .lt_field_name = "ACTION_R_NEW_UC_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpMcastCosQNew_map[] = {
    {
        .lt_field_name = "ACTION_R_NEW_MC_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpPrioIntNew_map[] = {
    {
        .lt_field_name = "ACTION_R_NEW_INTPRI",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpPrioIntTos_map[] = {
    {
        .lt_field_name = "ACTION_R_INTPRI_TO_TOS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpPrioIntCancel_map[] = {
    {
        .lt_field_name = "ACTION_R_INTPRI_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpDrop_map[] = {
    {
        .lt_field_name = "ACTION_Y_DROP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpDropCancel_map[] = {
    {
        .lt_field_name = "ACTION_Y_DROP_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpDropPrecedence_map[] = {
    {
        .lt_field_name = "ACTION_Y_NEW_COLOR",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpCopyToCpu_map[] = {
    {
        .lt_field_name = "ACTION_Y_COPY_TO_CPU",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_MATCH_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {8},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParam1,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpCopyToCpuCancel_map[] = {
    {
        .lt_field_name = "ACTION_Y_COPY_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_COPY_AND_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpSwitchToCpuCancel_map[] = {
    {
        .lt_field_name = "ACTION_Y_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_COPY_AND_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_COPY_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpSwitchToCpuReinstate_map[] = {
    {
        .lt_field_name = "ACTION_Y_SWITCH_TO_CPU_REINSATE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpTimeStampToCpu_map[] = {
    {
        .lt_field_name = "ACTION_Y_COPY_TO_CPU_WITH_TIMESTAMP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_MATCH_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {8},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParam1,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpDscpNew_map[] = {
    {
        .lt_field_name = "ACTION_Y_NEW_DSCP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpDscpCancel_map[] = {
    {
        .lt_field_name = "ACTION_Y_DSCP_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpEcnNew_map[] = {
    {
        .lt_field_name = "ACTION_Y_NEW_ECN",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpPrioPktNew_map[] = {
    {
        .lt_field_name = "ACTION_Y_NEW_OUTER_DOT1P",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpPrioPktTos_map[] = {
    {
        .lt_field_name = "ACTION_Y_OUTER_DOT1P_TO_TOS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpPrioPktCancel_map[] = {
    {
        .lt_field_name = "ACTION_Y_DOT1P_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpCosQNew_map[] = {
    {
        .lt_field_name = "ACTION_Y_NEW_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpUcastCosQNew_map[] = {
    {
        .lt_field_name = "ACTION_Y_NEW_UC_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpMcastCosQNew_map[] = {
    {
        .lt_field_name = "ACTION_Y_NEW_MC_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpPrioIntNew_map[] = {
    {
        .lt_field_name = "ACTION_Y_NEW_INTPRI",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpPrioIntTos_map[] = {
    {
        .lt_field_name = "ACTION_Y_INTPRI_TO_TOS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpPrioIntCancel_map[] = {
    {
        .lt_field_name = "ACTION_Y_INTPRI_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpDrop_map[] = {
    {
        .lt_field_name = "ACTION_G_DROP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpDropCancel_map[] = {
    {
        .lt_field_name = "ACTION_G_DROP_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpDropPrecedence_map[] = {
    {
        .lt_field_name = "ACTION_G_NEW_COLOR",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpCopyToCpu_map[] = {
    {
        .lt_field_name = "ACTION_G_COPY_TO_CPU",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_MATCH_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {8},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParam1,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpCopyToCpuCancel_map[] = {
    {
        .lt_field_name = "ACTION_G_COPY_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_G_COPY_AND_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_G_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpSwitchToCpuCancel_map[] = {
    {
        .lt_field_name = "ACTION_G_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_G_COPY_AND_SWITCH_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_G_COPY_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpSwitchToCpuReinstate_map[] = {
    {
        .lt_field_name = "ACTION_G_SWITCH_TO_CPU_REINSATE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpTimeStampToCpu_map[] = {
    {
        .lt_field_name = "ACTION_G_COPY_TO_CPU_WITH_TIMESTAMP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_MATCH_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {8},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParam1,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpDscpNew_map[] = {
    {
        .lt_field_name = "ACTION_G_NEW_DSCP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpDscpCancel_map[] = {
    {
        .lt_field_name = "ACTION_G_DSCP_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpEcnNew_map[] = {
    {
        .lt_field_name = "ACTION_G_NEW_ECN",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpTosPrecedenceNew_map[] = {
    {
        .lt_field_name = "ACTION_G_NEW_TOS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpTosPrecedenceCopy_map[] = {
    {
        .lt_field_name = "ACTION_G_TOS_TO_PRE_FP_ING_OUTER_DOT1P",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpPrioPktNew_map[] = {
    {
        .lt_field_name = "ACTION_G_NEW_OUTER_DOT1P",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpPrioPktTos_map[] = {
    {
        .lt_field_name = "ACTION_G_OUTER_DOT1P_TO_TOS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpPrioPktCancel_map[] = {
    {
        .lt_field_name = "ACTION_G_DOT1P_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpCosQNew_map[] = {
    {
        .lt_field_name = "ACTION_G_NEW_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpUcastCosQNew_map[] = {
    {
        .lt_field_name = "ACTION_G_NEW_UC_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpMcastCosQNew_map[] = {
    {
        .lt_field_name = "ACTION_G_NEW_MC_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpPrioIntNew_map[] = {
    {
        .lt_field_name = "ACTION_G_NEW_INTPRI",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpPrioIntTos_map[] = {
    {
        .lt_field_name = "ACTION_G_INTPRI_TO_TOS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpPrioIntCancel_map[] = {
    {
        .lt_field_name = "ACTION_G_INTPRI_UPDATES_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionDoNotChangeTtl_map[] = {
    {
        .lt_field_name = "ACTION_DO_NOT_CHANGE_TTL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionNewClassId_map[] = {
    {
        .lt_field_name = "ACTION_FP_ING_CLASS_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionMultipathHash_map[] = {
    {
        .lt_field_name = "ACTION_ECMP_HASH",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionServicePoolIdNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_SERVICE_POOL_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRedirectEgrNextHop_map[] = {
    {
        .lt_field_name = "ACTION_REDIRECT_TO_ECMP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {12},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_REDIRECT_TO_NHOP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {12},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionEgressClassSelect_map[] = {
    {
        .lt_field_name = "ACTION_ING_CLASS_ID_SELECT",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionPolicerGroup_map[] = {
    {
        .lt_field_name = "ACTION_METER_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionStatGroup_map[] = {
    {
        .lt_field_name = "CTR_ING_EFLEX_OBJECT",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 3,
        .internal_map = bcm_field_ifp_action_bcmFieldActionStatGroup_CTR_ING_EFLEX_OBJECT_internal_map,
    },
    {
        .lt_field_name = "ACTION_CTR_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionIngSampleEnable_map[] = {
    {
        .lt_field_name = "ACTION_SFLOW_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionUnmodifiedPacketRedirectPort_map[] = {
    {
        .lt_field_name = "ACTION_UNMODIFIED_REDIRECT_TO_PORT",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {9},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_UNMODIFIED_REDIRECT_TO_TRUNK",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {9},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionPfcClassNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_UNTAG_PKT_PRIORITY",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpIntCongestionNotificationNew_map[] = {
    {
        .lt_field_name = "ACTION_R_NEW_INTCN",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpIntCongestionNotificationNew_map[] = {
    {
        .lt_field_name = "ACTION_Y_NEW_INTCN",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpIntCongestionNotificationNew_map[] = {
    {
        .lt_field_name = "ACTION_G_NEW_INTCN",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGroupClassSet_map[] = {
    {
        .lt_field_name = "PRESEL_CLASS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionDot1pPreserve_map[] = {
    {
        .lt_field_name = "ACTION_G_PRESERVE_DOT1P",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_PRESERVE_DOT1P",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_PRESERVE_DOT1P",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpDot1pPreserve_map[] = {
    {
        .lt_field_name = "ACTION_G_PRESERVE_DOT1P",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpDot1pPreserve_map[] = {
    {
        .lt_field_name = "ACTION_Y_PRESERVE_DOT1P",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpDot1pPreserve_map[] = {
    {
        .lt_field_name = "ACTION_R_PRESERVE_DOT1P",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionDscpPreserve_map[] = {
    {
        .lt_field_name = "ACTION_R_PRESERVE_DSCP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_G_PRESERVE_DSCP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_PRESERVE_DSCP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpDscpPreserve_map[] = {
    {
        .lt_field_name = "ACTION_G_PRESERVE_DSCP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpDscpPreserve_map[] = {
    {
        .lt_field_name = "ACTION_Y_PRESERVE_DSCP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpDscpPreserve_map[] = {
    {
        .lt_field_name = "ACTION_R_PRESERVE_DSCP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionChangeL2FieldsCancel_map[] = {
    {
        .lt_field_name = "ACTION_CHANGE_PKT_L2_FIELDS_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionBFDSessionIdNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_BFD_SEESSION_IDX",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionCosMapNew_map[] = {
    {
        .lt_field_name = "ACTION_G_FP_ING_COS_Q_INT_PRI_MAP_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {2},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_FP_ING_COS_Q_INT_PRI_MAP_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {2},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_R_FP_ING_COS_Q_INT_PRI_MAP_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {2},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionGpCosMapNew_map[] = {
    {
        .lt_field_name = "ACTION_G_FP_ING_COS_Q_INT_PRI_MAP_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionYpCosMapNew_map[] = {
    {
        .lt_field_name = "ACTION_Y_FP_ING_COS_Q_INT_PRI_MAP_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionRpCosMapNew_map[] = {
    {
        .lt_field_name = "ACTION_R_FP_ING_COS_Q_INT_PRI_MAP_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionChangeL2Fields_map[] = {
    {
        .lt_field_name = "ACTION_CHANGE_PKT_L2_FIELDS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionExactMatchClassId_map[] = {
    {
        .lt_field_name = "ACTION_EXACT_MATCH_CLASS_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionTrunkRandomRoundRobinHashCancel_map[] = {
    {
        .lt_field_name = "ACTION_TRUNK_SPRAY_HASH_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionEcmpRandomRoundRobinHashCancel_map[] = {
    {
        .lt_field_name = "ACTION_ECMP_SPRAY_HASH_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionPacketTraceEnable_map[] = {
    {
        .lt_field_name = "ACTION_VISIBILITY_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionUntaggedPacketPriorityNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_UNTAG_PKT_PRIORITY",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionDoNotCutThrough_map[] = {
    {
        .lt_field_name = "ACTION_DO_NOT_CUT_THROUGH",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionProtectionSwitchingDrop_map[] = {
    {
        .lt_field_name = "ACTION_PROTECTION_SWITCHING_DROP_OVERRIDE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionServicePoolIdPrecedenceNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_SERVICE_POOL_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {3},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_NEW_SERVICE_POOL_PRECEDENCE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {3},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParam1,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionEgressTimeStampInsert_map[] = {
    {
        .lt_field_name = "ACTION_EGR_TIMESTAMP_INSERT",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionIngressTimeStampInsert_map[] = {
    {
        .lt_field_name = "ACTION_ING_TIMESTAMP_INSERT",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionIngressTimeStampInsertCancel_map[] = {
    {
        .lt_field_name = "ACTION_ING_TIMESTAMP_INSERT_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionEgressTimeStampInsertCancel_map[] = {
    {
        .lt_field_name = "ACTION_EGR_TIMESTAMP_INSERT_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionDynamicEcmpEnable_map[] = {
    {
        .lt_field_name = "ACTION_DYNAMIC_ECMP_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionDgm_map[] = {
    {
        .lt_field_name = "ACTION_DGM",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionDgmThreshold_map[] = {
    {
        .lt_field_name = "ACTION_DGM_THRESHOLD",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionDgmBias_map[] = {
    {
        .lt_field_name = "ACTION_DGM_BIAS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionDgmCost_map[] = {
    {
        .lt_field_name = "ACTION_DGM_COST",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionIntEncapEnable_map[] = {
    {
        .lt_field_name = "ACTION_INBAND_TELEMETRY_ENCAP_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionMatchPbmpDrop_map[] = {
    {
        .lt_field_name = "ACTION_FP_DELAYED_DROP_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {7},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 1,
        .internal_map = bcm_field_ifp_action_bcmFieldActionMatchPbmpDrop_ACTION_FP_DELAYED_DROP_ID_internal_map,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionMatchPbmpRedirect_map[] = {
    {
        .lt_field_name = "ACTION_FP_DELAYED_REDIRECT_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {7},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_DELAYED_REDIRECT_PORT",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {9},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 1,
        .internal_map = bcm_field_ifp_action_bcmFieldActionMatchPbmpRedirect_ACTION_DELAYED_REDIRECT_PORT_internal_map,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionIntEncapDisable_map[] = {
    {
        .lt_field_name = "ACTION_INBAND_TELEMETRY_ENCAP_DISABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionElephantLookupEnable_map[] = {
    {
        .lt_field_name = "ACTION_ETRAP_LOOKUP_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionElephantLookupDisable_map[] = {
    {
        .lt_field_name = "ACTION_ETRAP_LOOKUP_DISABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionElephantColorEnable_map[] = {
    {
        .lt_field_name = "ACTION_ETRAP_COLOR_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionElephantColorDisable_map[] = {
    {
        .lt_field_name = "ACTION_ETRAP_COLOR_DISABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionElephantQueueEnable_map[] = {
    {
        .lt_field_name = "ACTION_ETRAP_QUEUE_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionElephantQueueDisable_map[] = {
    {
        .lt_field_name = "ACTION_ETRAP_QUEUE_DISABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionDlbEcmpMonitorEnable_map[] = {
    {
        .lt_field_name = "ACTION_DLB_ECMP_MONITOR_DISABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_DLB_ECMP_MONITOR_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionMirrorZeroingEnable_map[] = {
    {
        .lt_field_name = "ACTION_MIRROR_EGR_ZERO_PAYLOAD_PROFILE_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionMirrorOnDropEnable_map[] = {
    {
        .lt_field_name = "ACTION_MIRROR_ON_DROP_TM_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionMirrorOnDropProfileSet_map[] = {
    {
        .lt_field_name = "ACTION_MIRROR_ON_DROP_TM_PROFILE_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionIntCongestionNotification_map[] = {
    {
        .lt_field_name = "ACTION_R_NEW_INTCN",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {2},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_G_NEW_INTCN",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {2},
        .sbr_enabled = FALSE,
        .em = BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_Y_NEW_INTCN",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {2},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionIntStatReq0_map[] = {
    {
        .lt_field_name = "ACTION_INBAND_TELEMETRY_MMU_STAT_REQ_0",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionIntStatReq1_map[] = {
    {
        .lt_field_name = "ACTION_INBAND_TELEMETRY_MMU_STAT_REQ_1",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionFlexSampleProfile_map[] = {
    {
        .lt_field_name = "ACTION_MIRROR_ING_FLEX_SFLOW_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionMirrorOnDropDisable_map[] = {
    {
        .lt_field_name = "ACTION_MIRROR_ON_DROP_TM_DISABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionMirrorFlowClass_map[] = {
    {
        .lt_field_name = "ACTION_MIRROR_FLOW_CLASS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 1,
        .internal_map = bcm_field_ifp_action_bcmFieldActionMirrorFlowClass_ACTION_MIRROR_FLOW_CLASS_internal_map,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionIntActionProfileSet_map[] = {
    {
        .lt_field_name = "ACTION_INBAND_TELEMETRY_PROFILE_IDX",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionIntTermActionProfileSet_map[] = {
    {
        .lt_field_name = "ACTION_INBAND_TELEMETRY_TERM_ACTION_PROFILE_IDX",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionIntFlowClassSet_map[] = {
    {
        .lt_field_name = "ACTION_INBAND_TELEMETRY_FLOW_CLASS_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionIntApplyOnSampledPkt_map[] = {
    {
        .lt_field_name = "ACTION_INBAND_TELEMETRY_SAMPLE_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_ifp_action_bcmFieldActionIntLoopbackProfileSet_map[] = {
    {
        .lt_field_name = "ACTION_INBAND_TELEMETRY_EGR_LOOPBACK_PROFILE_IDX",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_efp_action_bcmFieldActionDscpNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_DSCP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_efp_action_bcmFieldActionEcnNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_ECN",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_efp_action_bcmFieldActionDrop_map[] = {
    {
        .lt_field_name = "ACTION_DROP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_efp_action_bcmFieldActionDropCancel_map[] = {
    {
        .lt_field_name = "ACTION_DROP_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_efp_action_bcmFieldActionOuterVlanNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_OUTER_VLANID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_efp_action_bcmFieldActionOuterVlanPrioNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_OUTER_DOT1P",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_efp_action_bcmFieldActionOuterVlanCfiNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_OUTER_CFI",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_efp_action_bcmFieldActionOuterTpidNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_OUTER_TPID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_efp_action_bcmFieldActionStatGroup_map[] = {
    {
        .lt_field_name = "CTR_EGR_EFLEX_OBJECT",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_efp_action_bcmFieldActionLoopbackSrcPortGportNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_LOOPBACK_SRC_PORT",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_efp_action_bcmFieldActionLoopbackCpuMasqueradePktProfileNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_LOOPBACK_PACKET_PROFILE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_efp_action_bcmFieldActionLoopbackPacketProcessingPortNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_LOOPBACK_PP_PORT",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_efp_action_bcmFieldActionMplsLabel1ExpNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_MPLS_EXP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_efp_action_bcmFieldActionDscpMask_map[] = {
    {
        .lt_field_name = "ACTION_DSCP_MASK",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_efp_action_bcmFieldActionEcnMask_map[] = {
    {
        .lt_field_name = "ACTION_ECN_MASK",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionCosQCpuNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_CPU_COS",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionPrioIntNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_INTPRI",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionCopyToCpu_map[] = {
    {
        .lt_field_name = "ACTION_COPY_TO_CPU",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_MATCH_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {8},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParam1,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionCopyToCpuCancel_map[] = {
    {
        .lt_field_name = "ACTION_COPY_TO_CPU_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionDrop_map[] = {
    {
        .lt_field_name = "ACTION_DROP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionDropCancel_map[] = {
    {
        .lt_field_name = "ACTION_DROP_CANCEL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionDropPrecedence_map[] = {
    {
        .lt_field_name = "ACTION_NEW_COLOR",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionClassDestSet_map[] = {
    {
        .lt_field_name = "ACTION_VLAN_CLASS_0",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionClassSourceSet_map[] = {
    {
        .lt_field_name = "ACTION_VLAN_CLASS_1",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionVrfSet_map[] = {
    {
        .lt_field_name = "ACTION_NEW_VRF",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionOuterVlanNew_map[] = {
    {
        .lt_field_name = "ACTION_REPLACE_OUTER_TAG",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionOuterVlanAdd_map[] = {
    {
        .lt_field_name = "ACTION_ADD_OUTER_TAG",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionOuterVlanLookup_map[] = {
    {
        .lt_field_name = "ACTION_SET_FWD_VLAN_TAG",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionOuterVlanPrioNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_OUTER_DOT1P",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionOuterVlanCfiNew_map[] = {
    {
        .lt_field_name = "ACTION_NEW_OUTER_CFI",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionDoNotCheckVlan_map[] = {
    {
        .lt_field_name = "ACTION_DISABLE_VLAN_CHECK",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionDoNotLearn_map[] = {
    {
        .lt_field_name = "ACTION_DO_NOT_LEARN",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionStatGroup_map[] = {
    {
        .lt_field_name = "CTR_ING_EFLEX_OBJECT",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionQosMapIdNew_map[] = {
    {
        .lt_field_name = "ACTION_PHB_ING_IP_DSCP_TO_INT_PRI_REMAP_ID",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionL3IngressSet_map[] = {
    {
        .lt_field_name = "ACTION_L3_IIF_SET",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionEnableVlanCheck_map[] = {
    {
        .lt_field_name = "ACTION_ENABLE_VLAN_CHECK",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionMplsLookupEnable_map[] = {
    {
        .lt_field_name = "ACTION_MPLS_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_MPLS_DISABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionTerminationAllowed_map[] = {
    {
        .lt_field_name = "ACTION_MPLS_TERMINATION",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_IPV4_TERMINATION",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_IPV6_TERMINATION",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_IPV4_MC_TERMINATION",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_IPV6_MC_TERMINATION",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_ARP_RARP_TERMINATION",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
    {
        .lt_field_name = "ACTION_L3_TNL_TERMINATION",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {1},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionVisibilityEnable_map[] = {
    {
        .lt_field_name = "ACTION_VISIBILITY_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionL3IngressStrengthSet_map[] = {
    {
        .lt_field_name = "ACTION_PRI_MODIFIER",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionL3TunnelTypeSet_map[] = {
    {
        .lt_field_name = "ACTION_L3_TNL_TYPE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionL3TunnelAuto_map[] = {
    {
        .lt_field_name = "ACTION_TNL_AUTO",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionUseOuterHeaderDscp_map[] = {
    {
        .lt_field_name = "ACTION_USE_OUTER_HDR_DSCP",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionUseOuterHeaderTtl_map[] = {
    {
        .lt_field_name = "ACTION_USE_OUTER_HDR_TTL",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionDisableInnerHeaderDscpChange_map[] = {
    {
        .lt_field_name = "ACTION_INNER_HDR_DSCP_CHANGE_DISABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

static const bcm_field_action_map_info_t bcm_field_vfp_action_bcmFieldActionBfdEnable_map[] = {
    {
        .lt_field_name = "ACTION_BFD_ENABLE",
        .lt_pdd_field_name = NULL,
        .lt_sbr_field_name = NULL,
        .num_offsets = 1,
        .offset = {0},
        .width = {0},
        .sbr_enabled = FALSE,
        .em = 0,
        .policer = FALSE,
        .policer_pdd = FALSE,
        .color_index = -1,
        .pdd_sbr_index = -1,
        .param = bcmiFieldActionParamNone,
        .num_internal_maps = 0,
        .internal_map = NULL,
    },
};

bcm_field_qual_map_t bcm56990_a0_ifp_qual_data[147] = {
    {
        .qual = bcmFieldQualifySrcIp6,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_qual_bcmFieldQualifySrcIp6_map,
    },
    {
        .qual = bcmFieldQualifyDstIp6,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDstIp6_map,
    },
    {
        .qual = bcmFieldQualifySrcIp6High,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifySrcIp6High_map,
    },
    {
        .qual = bcmFieldQualifyDstIp6High,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDstIp6High_map,
    },
    {
        .qual = bcmFieldQualifySrcIp6Low,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifySrcIp6Low_map,
    },
    {
        .qual = bcmFieldQualifyDstIp6Low,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDstIp6Low_map,
    },
    {
        .qual = bcmFieldQualifySrcMac,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifySrcMac_map,
    },
    {
        .qual = bcmFieldQualifyDstMac,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDstMac_map,
    },
    {
        .qual = bcmFieldQualifySrcIp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifySrcIp_map,
    },
    {
        .qual = bcmFieldQualifyDstIp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDstIp_map,
    },
    {
        .qual = bcmFieldQualifyInPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyInPort_map,
    },
    {
        .qual = bcmFieldQualifyDrop,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDrop_map,
    },
    {
        .qual = bcmFieldQualifyIp6FlowLabel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIp6FlowLabel_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlan,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_qual_bcmFieldQualifyOuterVlan_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyOuterVlanId_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanPri,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyOuterVlanPri_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanCfi,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyOuterVlanCfi_map,
    },
    {
        .qual = bcmFieldQualifyRangeCheck,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyRangeCheck_map,
    },
    {
        .qual = bcmFieldQualifyL4SrcPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyL4SrcPort_map,
    },
    {
        .qual = bcmFieldQualifyL4DstPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyL4DstPort_map,
    },
    {
        .qual = bcmFieldQualifyEtherType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyEtherType_map,
    },
    {
        .qual = bcmFieldQualifyIpProtocol,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIpProtocol_map,
    },
    {
        .qual = bcmFieldQualifyDSCP,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDSCP_map,
    },
    {
        .qual = bcmFieldQualifyTtl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyTtl_map,
    },
    {
        .qual = bcmFieldQualifySrcPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifySrcPort_map,
    },
    {
        .qual = bcmFieldQualifySrcTrunk,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifySrcTrunk_map,
    },
    {
        .qual = bcmFieldQualifyDstPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDstPort_map,
    },
    {
        .qual = bcmFieldQualifyDstTrunk,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDstTrunk_map,
    },
    {
        .qual = bcmFieldQualifyTcpControl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyTcpControl_map,
    },
    {
        .qual = bcmFieldQualifyPacketRes,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyPacketRes_map,
    },
    {
        .qual = bcmFieldQualifySrcClassL2,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifySrcClassL2_map,
    },
    {
        .qual = bcmFieldQualifySrcClassL3,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifySrcClassL3_map,
    },
    {
        .qual = bcmFieldQualifySrcClassField,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifySrcClassField_map,
    },
    {
        .qual = bcmFieldQualifyDstClassL2,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDstClassL2_map,
    },
    {
        .qual = bcmFieldQualifyDstClassL3,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDstClassL3_map,
    },
    {
        .qual = bcmFieldQualifyDstClassField,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDstClassField_map,
    },
    {
        .qual = bcmFieldQualifyIpInfo,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIpInfo_map,
    },
    {
        .qual = bcmFieldQualifyMHOpcode,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyMHOpcode_map,
    },
    {
        .qual = bcmFieldQualifyIpFlags,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIpFlags_map,
    },
    {
        .qual = bcmFieldQualifyInnerTpid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyInnerTpid_map,
    },
    {
        .qual = bcmFieldQualifyOuterTpid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyOuterTpid_map,
    },
    {
        .qual = bcmFieldQualifyLoopbackType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyLoopbackType_map,
    },
    {
        .qual = bcmFieldQualifyLoopback,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyLoopback_map,
    },
    {
        .qual = bcmFieldQualifyIpType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIpType_map,
    },
    {
        .qual = bcmFieldQualifyForwardingType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyForwardingType_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceClassL2,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyInterfaceClassL2_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceClassL3,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyInterfaceClassL3_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceClassPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyInterfaceClassPort_map,
    },
    {
        .qual = bcmFieldQualifyL3Routable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyL3Routable_map,
    },
    {
        .qual = bcmFieldQualifyIpFrag,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIpFrag_map,
    },
    {
        .qual = bcmFieldQualifyVrf,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyVrf_map,
    },
    {
        .qual = bcmFieldQualifyL3Ingress,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyL3Ingress_map,
    },
    {
        .qual = bcmFieldQualifyL2Format,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyL2Format_map,
    },
    {
        .qual = bcmFieldQualifyVlanFormat,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_qual_bcmFieldQualifyVlanFormat_map,
    },
    {
        .qual = bcmFieldQualifyExtensionHeaderType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyExtensionHeaderType_map,
    },
    {
        .qual = bcmFieldQualifyExtensionHeaderSubCode,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyExtensionHeaderSubCode_map,
    },
    {
        .qual = bcmFieldQualifyExtensionHeader2Type,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyExtensionHeader2Type_map,
    },
    {
        .qual = bcmFieldQualifyL4Ports,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyL4Ports_map,
    },
    {
        .qual = bcmFieldQualifyMplsTerminated,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyMplsTerminated_map,
    },
    {
        .qual = bcmFieldQualifyForwardingVlanValid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyForwardingVlanValid_map,
    },
    {
        .qual = bcmFieldQualifyIngressStpState,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIngressStpState_map,
    },
    {
        .qual = bcmFieldQualifyL2SrcHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyL2SrcHit_map,
    },
    {
        .qual = bcmFieldQualifyL2SrcStatic,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyL2SrcStatic_map,
    },
    {
        .qual = bcmFieldQualifyL2DestHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyL2DestHit_map,
    },
    {
        .qual = bcmFieldQualifyL2StationMove,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyL2StationMove_map,
    },
    {
        .qual = bcmFieldQualifyL2CacheHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyL2CacheHit_map,
    },
    {
        .qual = bcmFieldQualifyL3DestRouteHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyL3DestRouteHit_map,
    },
    {
        .qual = bcmFieldQualifyDosAttack,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDosAttack_map,
    },
    {
        .qual = bcmFieldQualifyIcmpTypeCode,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIcmpTypeCode_map,
    },
    {
        .qual = bcmFieldQualifyDstL3Egress,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDstL3Egress_map,
    },
    {
        .qual = bcmFieldQualifyColor,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyColor_map,
    },
    {
        .qual = bcmFieldQualifyIntPriority,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIntPriority_map,
    },
    {
        .qual = bcmFieldQualifyForwardingVlanId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyForwardingVlanId_map,
    },
    {
        .qual = bcmFieldQualifyVpn,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyVpn_map,
    },
    {
        .qual = bcmFieldQualifyMyStationHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyMyStationHit_map,
    },
    {
        .qual = bcmFieldQualifyDstMultipath,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDstMultipath_map,
    },
    {
        .qual = bcmFieldQualifyL2PayloadFirstEightBytes,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyL2PayloadFirstEightBytes_map,
    },
    {
        .qual = bcmFieldQualifyDstL3EgressNextHops,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDstL3EgressNextHops_map,
    },
    {
        .qual = bcmFieldQualifyRecoverableDrop,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyRecoverableDrop_map,
    },
    {
        .qual = bcmFieldQualifyIpTunnelHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIpTunnelHit_map,
    },
    {
        .qual = bcmFieldQualifyMplsLabel1Hit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyMplsLabel1Hit_map,
    },
    {
        .qual = bcmFieldQualifyMplsLabel2Hit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyMplsLabel2Hit_map,
    },
    {
        .qual = bcmFieldQualifyGenericAssociatedChannelLabelValid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyGenericAssociatedChannelLabelValid_map,
    },
    {
        .qual = bcmFieldQualifyRouterAlertLabelValid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyRouterAlertLabelValid_map,
    },
    {
        .qual = bcmFieldQualifyNormalizeIpAddrs,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyNormalizeIpAddrs_map,
    },
    {
        .qual = bcmFieldQualifyNormalizeMacAddrs,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyNormalizeMacAddrs_map,
    },
    {
        .qual = bcmFieldQualifyMplsForwardingLabel,
        .flags = 0,
        .num_maps = 4,
        .map = bcm_field_ifp_qual_bcmFieldQualifyMplsForwardingLabel_map,
    },
    {
        .qual = bcmFieldQualifyMplsForwardingLabelTtl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyMplsForwardingLabelTtl_map,
    },
    {
        .qual = bcmFieldQualifyMplsForwardingLabelBos,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyMplsForwardingLabelBos_map,
    },
    {
        .qual = bcmFieldQualifyMplsForwardingLabelExp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyMplsForwardingLabelExp_map,
    },
    {
        .qual = bcmFieldQualifyMplsForwardingLabelId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyMplsForwardingLabelId_map,
    },
    {
        .qual = bcmFieldQualifyMplsControlWord,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyMplsControlWord_map,
    },
    {
        .qual = bcmFieldQualifyRtag7AHashUpper,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyRtag7AHashUpper_map,
    },
    {
        .qual = bcmFieldQualifyRtag7AHashLower,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyRtag7AHashLower_map,
    },
    {
        .qual = bcmFieldQualifyRtag7BHashUpper,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyRtag7BHashUpper_map,
    },
    {
        .qual = bcmFieldQualifyRtag7BHashLower,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyRtag7BHashLower_map,
    },
    {
        .qual = bcmFieldQualifyMplsForwardingLabelAction,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyMplsForwardingLabelAction_map,
    },
    {
        .qual = bcmFieldQualifyMplsControlWordValid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyMplsControlWordValid_map,
    },
    {
        .qual = bcmFieldQualifySrcGport,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifySrcGport_map,
    },
    {
        .qual = bcmFieldQualifyL3SrcRouteHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyL3SrcRouteHit_map,
    },
    {
        .qual = bcmFieldQualifyVxlanNetworkId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyVxlanNetworkId_map,
    },
    {
        .qual = bcmFieldQualifyVxlanFlags,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyVxlanFlags_map,
    },
    {
        .qual = bcmFieldQualifyIcmpError,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIcmpError_map,
    },
    {
        .qual = bcmFieldQualifyIntCongestionNotification,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIntCongestionNotification_map,
    },
    {
        .qual = bcmFieldQualifySrcGports,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifySrcGports_map,
    },
    {
        .qual = bcmFieldQualifyVxlanHeaderBits8_31,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyVxlanHeaderBits8_31_map,
    },
    {
        .qual = bcmFieldQualifyVxlanHeaderBits56_63,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyVxlanHeaderBits56_63_map,
    },
    {
        .qual = bcmFieldQualifyIpProtocolClass,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIpProtocolClass_map,
    },
    {
        .qual = bcmFieldQualifyEtherTypeClass,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyEtherTypeClass_map,
    },
    {
        .qual = bcmFieldQualifyTcpClassZero,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyTcpClassZero_map,
    },
    {
        .qual = bcmFieldQualifyTosClassZero,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyTosClassZero_map,
    },
    {
        .qual = bcmFieldQualifyTtlClassZero,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyTtlClassZero_map,
    },
    {
        .qual = bcmFieldQualifyTcpClassOne,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyTcpClassOne_map,
    },
    {
        .qual = bcmFieldQualifyTosClassOne,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyTosClassOne_map,
    },
    {
        .qual = bcmFieldQualifyTtlClassOne,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyTtlClassOne_map,
    },
    {
        .qual = bcmFieldQualifyIpFragNonOrFirst,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIpFragNonOrFirst_map,
    },
    {
        .qual = bcmFieldQualifyPacketLength,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyPacketLength_map,
    },
    {
        .qual = bcmFieldQualifyGroupClass,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyGroupClass_map,
    },
    {
        .qual = bcmFieldQualifyUdfClass,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyUdfClass_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceLookupClassPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyInterfaceLookupClassPort_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceIngressKeySelectClassPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyInterfaceIngressKeySelectClassPort_map,
    },
    {
        .qual = bcmFieldQualifyBfdYourDiscriminator,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyBfdYourDiscriminator_map,
    },
    {
        .qual = bcmFieldQualifyDstL2MulticastGroup,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDstL2MulticastGroup_map,
    },
    {
        .qual = bcmFieldQualifyDstL3MulticastGroup,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDstL3MulticastGroup_map,
    },
    {
        .qual = bcmFieldQualifyExactMatchActionClassId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyExactMatchActionClassId_map,
    },
    {
        .qual = bcmFieldQualifyExactMatchGroupClassId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyExactMatchGroupClassId_map,
    },
    {
        .qual = bcmFieldQualifyL2PayLoad,
        .flags = 0,
        .num_maps = 4,
        .map = bcm_field_ifp_qual_bcmFieldQualifyL2PayLoad_map,
    },
    {
        .qual = bcmFieldQualifyStpState,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyStpState_map,
    },
    {
        .qual = bcmFieldQualifySystemPortBitmap,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifySystemPortBitmap_map,
    },
    {
        .qual = bcmFieldQualifyDevicePortBitmap,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDevicePortBitmap_map,
    },
    {
        .qual = bcmFieldQualifyIntPktLengthValid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIntPktLengthValid_map,
    },
    {
        .qual = bcmFieldQualifyIntPktFinalHop,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIntPktFinalHop_map,
    },
    {
        .qual = bcmFieldQualifyIpTunnelTtl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIpTunnelTtl_map,
    },
    {
        .qual = bcmFieldQualifyIpTunnelTtlClassZero,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIpTunnelTtlClassZero_map,
    },
    {
        .qual = bcmFieldQualifyRangeCheckGroup,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyRangeCheckGroup_map,
    },
    {
        .qual = bcmFieldQualifyOpaqueTagHigh,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyOpaqueTagHigh_map,
    },
    {
        .qual = bcmFieldQualifyOpaqueTagLow,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyOpaqueTagLow_map,
    },
    {
        .qual = bcmFieldQualifyIfaHopLimitEqualToZero,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIfaHopLimitEqualToZero_map,
    },
    {
        .qual = bcmFieldQualifyIntFlags,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIntFlags_map,
    },
    {
        .qual = bcmFieldQualifyIntHdrType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIntHdrType_map,
    },
    {
        .qual = bcmFieldQualifyIntUdpChecksumEqualsZero,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIntUdpChecksumEqualsZero_map,
    },
    {
        .qual = bcmFieldQualifyIntMetadataHdrType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIntMetadataHdrType_map,
    },
    {
        .qual = bcmFieldQualifyIntHdrPresent,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyIntHdrPresent_map,
    },
    {
        .qual = bcmFieldQualifyCompressionSrcHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyCompressionSrcHit_map,
    },
    {
        .qual = bcmFieldQualifyCompressionDstHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyCompressionDstHit_map,
    },
    {
        .qual = bcmFieldQualifySrcCompressionClassId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifySrcCompressionClassId_map,
    },
    {
        .qual = bcmFieldQualifyDstCompressionClassId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_qual_bcmFieldQualifyDstCompressionClassId_map,
    },
};

bcm_field_qual_map_t bcm56990_a0_efp_qual_data[66] = {
    {
        .qual = bcmFieldQualifySrcIp6,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_efp_qual_bcmFieldQualifySrcIp6_map,
    },
    {
        .qual = bcmFieldQualifyDstIp6,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_efp_qual_bcmFieldQualifyDstIp6_map,
    },
    {
        .qual = bcmFieldQualifySrcIp6High,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifySrcIp6High_map,
    },
    {
        .qual = bcmFieldQualifyDstIp6High,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyDstIp6High_map,
    },
    {
        .qual = bcmFieldQualifySrcMac,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifySrcMac_map,
    },
    {
        .qual = bcmFieldQualifyDstMac,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyDstMac_map,
    },
    {
        .qual = bcmFieldQualifySrcIp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifySrcIp_map,
    },
    {
        .qual = bcmFieldQualifyDstIp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyDstIp_map,
    },
    {
        .qual = bcmFieldQualifyInPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyInPort_map,
    },
    {
        .qual = bcmFieldQualifyDrop,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyDrop_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlan,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_efp_qual_bcmFieldQualifyOuterVlan_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyOuterVlanId_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanPri,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyOuterVlanPri_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanCfi,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyOuterVlanCfi_map,
    },
    {
        .qual = bcmFieldQualifyL4SrcPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyL4SrcPort_map,
    },
    {
        .qual = bcmFieldQualifyL4DstPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyL4DstPort_map,
    },
    {
        .qual = bcmFieldQualifyEtherType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyEtherType_map,
    },
    {
        .qual = bcmFieldQualifyIpProtocol,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyIpProtocol_map,
    },
    {
        .qual = bcmFieldQualifyDSCP,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyDSCP_map,
    },
    {
        .qual = bcmFieldQualifyTtl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyTtl_map,
    },
    {
        .qual = bcmFieldQualifyTcpControl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyTcpControl_map,
    },
    {
        .qual = bcmFieldQualifySrcClassL2,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifySrcClassL2_map,
    },
    {
        .qual = bcmFieldQualifySrcClassL3,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifySrcClassL3_map,
    },
    {
        .qual = bcmFieldQualifySrcClassField,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifySrcClassField_map,
    },
    {
        .qual = bcmFieldQualifyDstClassL2,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyDstClassL2_map,
    },
    {
        .qual = bcmFieldQualifyDstClassL3,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyDstClassL3_map,
    },
    {
        .qual = bcmFieldQualifyDstClassField,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyDstClassField_map,
    },
    {
        .qual = bcmFieldQualifyLoopbackType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyLoopbackType_map,
    },
    {
        .qual = bcmFieldQualifyIpType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyIpType_map,
    },
    {
        .qual = bcmFieldQualifyForwardingType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyForwardingType_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceClassL3,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyInterfaceClassL3_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceClassPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyInterfaceClassPort_map,
    },
    {
        .qual = bcmFieldQualifyL3Routable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyL3Routable_map,
    },
    {
        .qual = bcmFieldQualifyIpFrag,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyIpFrag_map,
    },
    {
        .qual = bcmFieldQualifyVrf,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyVrf_map,
    },
    {
        .qual = bcmFieldQualifyOutPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyOutPort_map,
    },
    {
        .qual = bcmFieldQualifyIp4,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyIp4_map,
    },
    {
        .qual = bcmFieldQualifyIp6,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyIp6_map,
    },
    {
        .qual = bcmFieldQualifyL2Format,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyL2Format_map,
    },
    {
        .qual = bcmFieldQualifyVlanFormat,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_efp_qual_bcmFieldQualifyVlanFormat_map,
    },
    {
        .qual = bcmFieldQualifyExtensionHeaderType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyExtensionHeaderType_map,
    },
    {
        .qual = bcmFieldQualifyExtensionHeaderSubCode,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyExtensionHeaderSubCode_map,
    },
    {
        .qual = bcmFieldQualifyL4Ports,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyL4Ports_map,
    },
    {
        .qual = bcmFieldQualifyMirrorCopy,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyMirrorCopy_map,
    },
    {
        .qual = bcmFieldQualifyIcmpTypeCode,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyIcmpTypeCode_map,
    },
    {
        .qual = bcmFieldQualifyColor,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyColor_map,
    },
    {
        .qual = bcmFieldQualifyIntPriority,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyIntPriority_map,
    },
    {
        .qual = bcmFieldQualifyForwardingVlanId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyForwardingVlanId_map,
    },
    {
        .qual = bcmFieldQualifyVpn,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyVpn_map,
    },
    {
        .qual = bcmFieldQualifyEgressClass,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyEgressClass_map,
    },
    {
        .qual = bcmFieldQualifyEgressClassL3Interface,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyEgressClassL3Interface_map,
    },
    {
        .qual = bcmFieldQualifyCpuQueue,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyCpuQueue_map,
    },
    {
        .qual = bcmFieldQualifyIngressClassField,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyIngressClassField_map,
    },
    {
        .qual = bcmFieldQualifyIngressInterfaceClassPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyIngressInterfaceClassPort_map,
    },
    {
        .qual = bcmFieldQualifyIntCongestionNotification,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyIntCongestionNotification_map,
    },
    {
        .qual = bcmFieldQualifyIngressInterfaceClassVPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyIngressInterfaceClassVPort_map,
    },
    {
        .qual = bcmFieldQualifyLoopBackQueue,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyLoopBackQueue_map,
    },
    {
        .qual = bcmFieldQualifyLoopBackSrcGport,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyLoopBackSrcGport_map,
    },
    {
        .qual = bcmFieldQualifyPktIsVisible,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyPktIsVisible_map,
    },
    {
        .qual = bcmFieldQualifyLoopBackCpuMasqueradePktProfile,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyLoopBackCpuMasqueradePktProfile_map,
    },
    {
        .qual = bcmFieldQualifyLoopBackColor,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyLoopBackColor_map,
    },
    {
        .qual = bcmFieldQualifyLoopBackTrafficClass,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyLoopBackTrafficClass_map,
    },
    {
        .qual = bcmFieldQualifyLoopBackPacketProcessingPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifyLoopBackPacketProcessingPort_map,
    },
    {
        .qual = bcmFieldQualifyL2PayLoad,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_efp_qual_bcmFieldQualifyL2PayLoad_map,
    },
    {
        .qual = bcmFieldQualifySampledPkt,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifySampledPkt_map,
    },
    {
        .qual = bcmFieldQualifySrcIp6SrhValid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_qual_bcmFieldQualifySrcIp6SrhValid_map,
    },
};

bcm_field_qual_map_t bcm56990_a0_vfp_qual_data[56] = {
    {
        .qual = bcmFieldQualifySrcIp6,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_vfp_qual_bcmFieldQualifySrcIp6_map,
    },
    {
        .qual = bcmFieldQualifyDstIp6,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_vfp_qual_bcmFieldQualifyDstIp6_map,
    },
    {
        .qual = bcmFieldQualifySrcIp6High,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifySrcIp6High_map,
    },
    {
        .qual = bcmFieldQualifyDstIp6High,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyDstIp6High_map,
    },
    {
        .qual = bcmFieldQualifySrcMac,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifySrcMac_map,
    },
    {
        .qual = bcmFieldQualifyDstMac,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyDstMac_map,
    },
    {
        .qual = bcmFieldQualifySnap,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifySnap_map,
    },
    {
        .qual = bcmFieldQualifySrcIp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifySrcIp_map,
    },
    {
        .qual = bcmFieldQualifyDstIp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyDstIp_map,
    },
    {
        .qual = bcmFieldQualifyLlc,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyLlc_map,
    },
    {
        .qual = bcmFieldQualifyInPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyInPort_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlan,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_vfp_qual_bcmFieldQualifyOuterVlan_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyOuterVlanId_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanPri,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyOuterVlanPri_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanCfi,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyOuterVlanCfi_map,
    },
    {
        .qual = bcmFieldQualifyL4SrcPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyL4SrcPort_map,
    },
    {
        .qual = bcmFieldQualifyL4DstPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyL4DstPort_map,
    },
    {
        .qual = bcmFieldQualifyEtherType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyEtherType_map,
    },
    {
        .qual = bcmFieldQualifyIpProtocol,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyIpProtocol_map,
    },
    {
        .qual = bcmFieldQualifyDSCP,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyDSCP_map,
    },
    {
        .qual = bcmFieldQualifyTtl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyTtl_map,
    },
    {
        .qual = bcmFieldQualifySrcPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifySrcPort_map,
    },
    {
        .qual = bcmFieldQualifySrcTrunk,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifySrcTrunk_map,
    },
    {
        .qual = bcmFieldQualifyTcpControl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyTcpControl_map,
    },
    {
        .qual = bcmFieldQualifyIpInfo,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyIpInfo_map,
    },
    {
        .qual = bcmFieldQualifyIpProtocolCommon,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyIpProtocolCommon_map,
    },
    {
        .qual = bcmFieldQualifyOuterTpid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyOuterTpid_map,
    },
    {
        .qual = bcmFieldQualifyInnerIpType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyInnerIpType_map,
    },
    {
        .qual = bcmFieldQualifyIpType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyIpType_map,
    },
    {
        .qual = bcmFieldQualifyHiGig,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyHiGig_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceClassL3,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyInterfaceClassL3_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceClassPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyInterfaceClassPort_map,
    },
    {
        .qual = bcmFieldQualifyIpFrag,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyIpFrag_map,
    },
    {
        .qual = bcmFieldQualifyL2Format,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyL2Format_map,
    },
    {
        .qual = bcmFieldQualifyVlanFormat,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_vfp_qual_bcmFieldQualifyVlanFormat_map,
    },
    {
        .qual = bcmFieldQualifyExtensionHeaderType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyExtensionHeaderType_map,
    },
    {
        .qual = bcmFieldQualifyExtensionHeaderSubCode,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyExtensionHeaderSubCode_map,
    },
    {
        .qual = bcmFieldQualifyL4Ports,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyL4Ports_map,
    },
    {
        .qual = bcmFieldQualifyTunnelTerminated,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyTunnelTerminated_map,
    },
    {
        .qual = bcmFieldQualifyInnerSrcIp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyInnerSrcIp_map,
    },
    {
        .qual = bcmFieldQualifyInnerDstIp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyInnerDstIp_map,
    },
    {
        .qual = bcmFieldQualifyInnerSrcIp6,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_vfp_qual_bcmFieldQualifyInnerSrcIp6_map,
    },
    {
        .qual = bcmFieldQualifyInnerDstIp6,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_vfp_qual_bcmFieldQualifyInnerDstIp6_map,
    },
    {
        .qual = bcmFieldQualifyInnerSrcIp6High,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyInnerSrcIp6High_map,
    },
    {
        .qual = bcmFieldQualifyInnerDstIp6High,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyInnerDstIp6High_map,
    },
    {
        .qual = bcmFieldQualifyInnerTtl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyInnerTtl_map,
    },
    {
        .qual = bcmFieldQualifyInnerTos,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyInnerTos_map,
    },
    {
        .qual = bcmFieldQualifyInnerIpProtocol,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyInnerIpProtocol_map,
    },
    {
        .qual = bcmFieldQualifyInnerIpFrag,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyInnerIpFrag_map,
    },
    {
        .qual = bcmFieldQualifyIcmpTypeCode,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyIcmpTypeCode_map,
    },
    {
        .qual = bcmFieldQualifyInnerIpProtocolCommon,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyInnerIpProtocolCommon_map,
    },
    {
        .qual = bcmFieldQualifyInnerL4SrcPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyInnerL4SrcPort_map,
    },
    {
        .qual = bcmFieldQualifyInnerL4DstPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyInnerL4DstPort_map,
    },
    {
        .qual = bcmFieldQualifySrcGport,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifySrcGport_map,
    },
    {
        .qual = bcmFieldQualifySrcGports,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifySrcGports_map,
    },
    {
        .qual = bcmFieldQualifyOpaqueTagType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_qual_bcmFieldQualifyOpaqueTagType_map,
    },
};

bcm_field_qual_map_t bcm56990_a0_em_qual_data[0] = {
};

bcm_field_qual_map_t bcm56990_a0_ifp_presel_qual_data[16] = {
    {
        .qual = bcmFieldQualifyDrop,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_presel_qual_bcmFieldQualifyDrop_map,
    },
    {
        .qual = bcmFieldQualifyPacketRes,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_presel_qual_bcmFieldQualifyPacketRes_map,
    },
    {
        .qual = bcmFieldQualifyTunnelType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_presel_qual_bcmFieldQualifyTunnelType_map,
    },
    {
        .qual = bcmFieldQualifyIpType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_presel_qual_bcmFieldQualifyIpType_map,
    },
    {
        .qual = bcmFieldQualifyForwardingType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_presel_qual_bcmFieldQualifyForwardingType_map,
    },
    {
        .qual = bcmFieldQualifyL4Ports,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_presel_qual_bcmFieldQualifyL4Ports_map,
    },
    {
        .qual = bcmFieldQualifyForwardingVlanValid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_presel_qual_bcmFieldQualifyForwardingVlanValid_map,
    },
    {
        .qual = bcmFieldQualifyIngressStpState,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_presel_qual_bcmFieldQualifyIngressStpState_map,
    },
    {
        .qual = bcmFieldQualifyDosAttack,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_presel_qual_bcmFieldQualifyDosAttack_map,
    },
    {
        .qual = bcmFieldQualifyMyStationHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_presel_qual_bcmFieldQualifyMyStationHit_map,
    },
    {
        .qual = bcmFieldQualifyIpTunnelHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_presel_qual_bcmFieldQualifyIpTunnelHit_map,
    },
    {
        .qual = bcmFieldQualifyMplsLabel1Hit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_presel_qual_bcmFieldQualifyMplsLabel1Hit_map,
    },
    {
        .qual = bcmFieldQualifyMplsLabel2Hit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_presel_qual_bcmFieldQualifyMplsLabel2Hit_map,
    },
    {
        .qual = bcmFieldQualifyMixedSrcClassId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_presel_qual_bcmFieldQualifyMixedSrcClassId_map,
    },
    {
        .qual = bcmFieldQualifyExactMatchGroupClassId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_presel_qual_bcmFieldQualifyExactMatchGroupClassId_map,
    },
    {
        .qual = bcmFieldQualifyStpState,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_presel_qual_bcmFieldQualifyStpState_map,
    },
};

bcm_field_qual_map_t bcm56990_a0_efp_presel_qual_data[0] = {
};

bcm_field_qual_map_t bcm56990_a0_vfp_presel_qual_data[0] = {
};

bcm_field_qual_map_t bcm56990_a0_em_presel_qual_data[0] = {
};

bcm_field_qual_map_t bcm56990_a0_ifp_group_qual_data[145] = {
    {
        .qual = bcmFieldQualifySrcIp6,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifySrcIp6_map,
    },
    {
        .qual = bcmFieldQualifyDstIp6,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDstIp6_map,
    },
    {
        .qual = bcmFieldQualifySrcIp6High,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifySrcIp6High_map,
    },
    {
        .qual = bcmFieldQualifyDstIp6High,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDstIp6High_map,
    },
    {
        .qual = bcmFieldQualifySrcIp6Low,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifySrcIp6Low_map,
    },
    {
        .qual = bcmFieldQualifyDstIp6Low,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDstIp6Low_map,
    },
    {
        .qual = bcmFieldQualifySrcMac,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifySrcMac_map,
    },
    {
        .qual = bcmFieldQualifyDstMac,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDstMac_map,
    },
    {
        .qual = bcmFieldQualifySrcIp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifySrcIp_map,
    },
    {
        .qual = bcmFieldQualifyDstIp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDstIp_map,
    },
    {
        .qual = bcmFieldQualifyInPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyInPort_map,
    },
    {
        .qual = bcmFieldQualifyDrop,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDrop_map,
    },
    {
        .qual = bcmFieldQualifyIp6FlowLabel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIp6FlowLabel_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlan,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyOuterVlan_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyOuterVlanId_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanPri,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyOuterVlanPri_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanCfi,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyOuterVlanCfi_map,
    },
    {
        .qual = bcmFieldQualifyRangeCheck,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyRangeCheck_map,
    },
    {
        .qual = bcmFieldQualifyL4SrcPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyL4SrcPort_map,
    },
    {
        .qual = bcmFieldQualifyL4DstPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyL4DstPort_map,
    },
    {
        .qual = bcmFieldQualifyEtherType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyEtherType_map,
    },
    {
        .qual = bcmFieldQualifyIpProtocol,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIpProtocol_map,
    },
    {
        .qual = bcmFieldQualifyDSCP,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDSCP_map,
    },
    {
        .qual = bcmFieldQualifyTtl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyTtl_map,
    },
    {
        .qual = bcmFieldQualifySrcPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifySrcPort_map,
    },
    {
        .qual = bcmFieldQualifySrcTrunk,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifySrcTrunk_map,
    },
    {
        .qual = bcmFieldQualifyDstPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDstPort_map,
    },
    {
        .qual = bcmFieldQualifyDstTrunk,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDstTrunk_map,
    },
    {
        .qual = bcmFieldQualifyTcpControl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyTcpControl_map,
    },
    {
        .qual = bcmFieldQualifyPacketRes,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyPacketRes_map,
    },
    {
        .qual = bcmFieldQualifySrcClassL2,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifySrcClassL2_map,
    },
    {
        .qual = bcmFieldQualifySrcClassL3,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifySrcClassL3_map,
    },
    {
        .qual = bcmFieldQualifySrcClassField,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifySrcClassField_map,
    },
    {
        .qual = bcmFieldQualifyDstClassL2,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDstClassL2_map,
    },
    {
        .qual = bcmFieldQualifyDstClassL3,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDstClassL3_map,
    },
    {
        .qual = bcmFieldQualifyDstClassField,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDstClassField_map,
    },
    {
        .qual = bcmFieldQualifyIpInfo,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIpInfo_map,
    },
    {
        .qual = bcmFieldQualifyMHOpcode,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyMHOpcode_map,
    },
    {
        .qual = bcmFieldQualifyIpFlags,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIpFlags_map,
    },
    {
        .qual = bcmFieldQualifyInnerTpid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyInnerTpid_map,
    },
    {
        .qual = bcmFieldQualifyOuterTpid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyOuterTpid_map,
    },
    {
        .qual = bcmFieldQualifyLoopbackType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyLoopbackType_map,
    },
    {
        .qual = bcmFieldQualifyLoopback,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyLoopback_map,
    },
    {
        .qual = bcmFieldQualifyIpType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIpType_map,
    },
    {
        .qual = bcmFieldQualifyForwardingType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyForwardingType_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceClassL2,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyInterfaceClassL2_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceClassL3,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyInterfaceClassL3_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceClassPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyInterfaceClassPort_map,
    },
    {
        .qual = bcmFieldQualifyL3Routable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyL3Routable_map,
    },
    {
        .qual = bcmFieldQualifyIpFrag,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIpFrag_map,
    },
    {
        .qual = bcmFieldQualifyVrf,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyVrf_map,
    },
    {
        .qual = bcmFieldQualifyL3Ingress,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyL3Ingress_map,
    },
    {
        .qual = bcmFieldQualifyL2Format,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyL2Format_map,
    },
    {
        .qual = bcmFieldQualifyVlanFormat,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyVlanFormat_map,
    },
    {
        .qual = bcmFieldQualifyExtensionHeaderType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyExtensionHeaderType_map,
    },
    {
        .qual = bcmFieldQualifyExtensionHeaderSubCode,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyExtensionHeaderSubCode_map,
    },
    {
        .qual = bcmFieldQualifyExtensionHeader2Type,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyExtensionHeader2Type_map,
    },
    {
        .qual = bcmFieldQualifyL4Ports,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyL4Ports_map,
    },
    {
        .qual = bcmFieldQualifyMplsTerminated,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyMplsTerminated_map,
    },
    {
        .qual = bcmFieldQualifyForwardingVlanValid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyForwardingVlanValid_map,
    },
    {
        .qual = bcmFieldQualifyIngressStpState,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIngressStpState_map,
    },
    {
        .qual = bcmFieldQualifyL2SrcHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyL2SrcHit_map,
    },
    {
        .qual = bcmFieldQualifyL2SrcStatic,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyL2SrcStatic_map,
    },
    {
        .qual = bcmFieldQualifyL2DestHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyL2DestHit_map,
    },
    {
        .qual = bcmFieldQualifyL2StationMove,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyL2StationMove_map,
    },
    {
        .qual = bcmFieldQualifyL2CacheHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyL2CacheHit_map,
    },
    {
        .qual = bcmFieldQualifyL3DestRouteHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyL3DestRouteHit_map,
    },
    {
        .qual = bcmFieldQualifyDosAttack,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDosAttack_map,
    },
    {
        .qual = bcmFieldQualifyIcmpTypeCode,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIcmpTypeCode_map,
    },
    {
        .qual = bcmFieldQualifyDstL3Egress,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDstL3Egress_map,
    },
    {
        .qual = bcmFieldQualifyColor,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyColor_map,
    },
    {
        .qual = bcmFieldQualifyIntPriority,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIntPriority_map,
    },
    {
        .qual = bcmFieldQualifyForwardingVlanId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyForwardingVlanId_map,
    },
    {
        .qual = bcmFieldQualifyVpn,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyVpn_map,
    },
    {
        .qual = bcmFieldQualifyMyStationHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyMyStationHit_map,
    },
    {
        .qual = bcmFieldQualifyDstMultipath,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDstMultipath_map,
    },
    {
        .qual = bcmFieldQualifyL2PayloadFirstEightBytes,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyL2PayloadFirstEightBytes_map,
    },
    {
        .qual = bcmFieldQualifyDstL3EgressNextHops,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDstL3EgressNextHops_map,
    },
    {
        .qual = bcmFieldQualifyRecoverableDrop,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyRecoverableDrop_map,
    },
    {
        .qual = bcmFieldQualifyIpTunnelHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIpTunnelHit_map,
    },
    {
        .qual = bcmFieldQualifyMplsLabel1Hit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyMplsLabel1Hit_map,
    },
    {
        .qual = bcmFieldQualifyMplsLabel2Hit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyMplsLabel2Hit_map,
    },
    {
        .qual = bcmFieldQualifyGenericAssociatedChannelLabelValid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyGenericAssociatedChannelLabelValid_map,
    },
    {
        .qual = bcmFieldQualifyRouterAlertLabelValid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyRouterAlertLabelValid_map,
    },
    {
        .qual = bcmFieldQualifyNormalizeIpAddrs,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyNormalizeIpAddrs_map,
    },
    {
        .qual = bcmFieldQualifyNormalizeMacAddrs,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyNormalizeMacAddrs_map,
    },
    {
        .qual = bcmFieldQualifyMplsForwardingLabel,
        .flags = 0,
        .num_maps = 4,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyMplsForwardingLabel_map,
    },
    {
        .qual = bcmFieldQualifyMplsForwardingLabelTtl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyMplsForwardingLabelTtl_map,
    },
    {
        .qual = bcmFieldQualifyMplsForwardingLabelBos,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyMplsForwardingLabelBos_map,
    },
    {
        .qual = bcmFieldQualifyMplsForwardingLabelExp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyMplsForwardingLabelExp_map,
    },
    {
        .qual = bcmFieldQualifyMplsForwardingLabelId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyMplsForwardingLabelId_map,
    },
    {
        .qual = bcmFieldQualifyMplsControlWord,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyMplsControlWord_map,
    },
    {
        .qual = bcmFieldQualifyRtag7AHashUpper,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyRtag7AHashUpper_map,
    },
    {
        .qual = bcmFieldQualifyRtag7AHashLower,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyRtag7AHashLower_map,
    },
    {
        .qual = bcmFieldQualifyRtag7BHashUpper,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyRtag7BHashUpper_map,
    },
    {
        .qual = bcmFieldQualifyRtag7BHashLower,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyRtag7BHashLower_map,
    },
    {
        .qual = bcmFieldQualifyMplsForwardingLabelAction,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyMplsForwardingLabelAction_map,
    },
    {
        .qual = bcmFieldQualifyMplsControlWordValid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyMplsControlWordValid_map,
    },
    {
        .qual = bcmFieldQualifySrcGport,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifySrcGport_map,
    },
    {
        .qual = bcmFieldQualifyL3SrcRouteHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyL3SrcRouteHit_map,
    },
    {
        .qual = bcmFieldQualifyVxlanNetworkId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyVxlanNetworkId_map,
    },
    {
        .qual = bcmFieldQualifyVxlanFlags,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyVxlanFlags_map,
    },
    {
        .qual = bcmFieldQualifyIcmpError,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIcmpError_map,
    },
    {
        .qual = bcmFieldQualifyIntCongestionNotification,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIntCongestionNotification_map,
    },
    {
        .qual = bcmFieldQualifySrcGports,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifySrcGports_map,
    },
    {
        .qual = bcmFieldQualifyVxlanHeaderBits8_31,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyVxlanHeaderBits8_31_map,
    },
    {
        .qual = bcmFieldQualifyVxlanHeaderBits56_63,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyVxlanHeaderBits56_63_map,
    },
    {
        .qual = bcmFieldQualifyIpProtocolClass,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIpProtocolClass_map,
    },
    {
        .qual = bcmFieldQualifyEtherTypeClass,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyEtherTypeClass_map,
    },
    {
        .qual = bcmFieldQualifyTcpClassZero,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyTcpClassZero_map,
    },
    {
        .qual = bcmFieldQualifyTosClassZero,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyTosClassZero_map,
    },
    {
        .qual = bcmFieldQualifyTtlClassZero,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyTtlClassZero_map,
    },
    {
        .qual = bcmFieldQualifyTcpClassOne,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyTcpClassOne_map,
    },
    {
        .qual = bcmFieldQualifyTosClassOne,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyTosClassOne_map,
    },
    {
        .qual = bcmFieldQualifyTtlClassOne,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyTtlClassOne_map,
    },
    {
        .qual = bcmFieldQualifyIpFragNonOrFirst,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIpFragNonOrFirst_map,
    },
    {
        .qual = bcmFieldQualifyPacketLength,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyPacketLength_map,
    },
    {
        .qual = bcmFieldQualifyGroupClass,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyGroupClass_map,
    },
    {
        .qual = bcmFieldQualifyUdfClass,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyUdfClass_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceLookupClassPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyInterfaceLookupClassPort_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceIngressKeySelectClassPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyInterfaceIngressKeySelectClassPort_map,
    },
    {
        .qual = bcmFieldQualifyBfdYourDiscriminator,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyBfdYourDiscriminator_map,
    },
    {
        .qual = bcmFieldQualifyDstL2MulticastGroup,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDstL2MulticastGroup_map,
    },
    {
        .qual = bcmFieldQualifyDstL3MulticastGroup,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDstL3MulticastGroup_map,
    },
    {
        .qual = bcmFieldQualifyExactMatchActionClassId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyExactMatchActionClassId_map,
    },
    {
        .qual = bcmFieldQualifyExactMatchGroupClassId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyExactMatchGroupClassId_map,
    },
    {
        .qual = bcmFieldQualifyL2PayLoad,
        .flags = 0,
        .num_maps = 4,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyL2PayLoad_map,
    },
    {
        .qual = bcmFieldQualifyStpState,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyStpState_map,
    },
    {
        .qual = bcmFieldQualifySystemPortBitmap,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifySystemPortBitmap_map,
    },
    {
        .qual = bcmFieldQualifyDevicePortBitmap,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyDevicePortBitmap_map,
    },
    {
        .qual = bcmFieldQualifyIntPktLengthValid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIntPktLengthValid_map,
    },
    {
        .qual = bcmFieldQualifyIntPktFinalHop,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIntPktFinalHop_map,
    },
    {
        .qual = bcmFieldQualifyIpTunnelTtl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIpTunnelTtl_map,
    },
    {
        .qual = bcmFieldQualifyIpTunnelTtlClassZero,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIpTunnelTtlClassZero_map,
    },
    {
        .qual = bcmFieldQualifyRangeCheckGroup,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyRangeCheckGroup_map,
    },
    {
        .qual = bcmFieldQualifyOpaqueTagHigh,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyOpaqueTagHigh_map,
    },
    {
        .qual = bcmFieldQualifyOpaqueTagLow,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyOpaqueTagLow_map,
    },
    {
        .qual = bcmFieldQualifyIfaHopLimitEqualToZero,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIfaHopLimitEqualToZero_map,
    },
    {
        .qual = bcmFieldQualifyIntFlags,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIntFlags_map,
    },
    {
        .qual = bcmFieldQualifyIntHdrType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIntHdrType_map,
    },
    {
        .qual = bcmFieldQualifyIntUdpChecksumEqualsZero,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIntUdpChecksumEqualsZero_map,
    },
    {
        .qual = bcmFieldQualifyIntMetadataHdrType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIntMetadataHdrType_map,
    },
    {
        .qual = bcmFieldQualifyIntHdrPresent,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyIntHdrPresent_map,
    },
    {
        .qual = bcmFieldQualifyCompressionSrcHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyCompressionSrcHit_map,
    },
    {
        .qual = bcmFieldQualifyCompressionDstHit,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_group_qual_bcmFieldQualifyCompressionDstHit_map,
    },
};

bcm_field_qual_map_t bcm56990_a0_efp_group_qual_data[66] = {
    {
        .qual = bcmFieldQualifySrcIp6,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifySrcIp6_map,
    },
    {
        .qual = bcmFieldQualifyDstIp6,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyDstIp6_map,
    },
    {
        .qual = bcmFieldQualifySrcIp6High,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifySrcIp6High_map,
    },
    {
        .qual = bcmFieldQualifyDstIp6High,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyDstIp6High_map,
    },
    {
        .qual = bcmFieldQualifySrcMac,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifySrcMac_map,
    },
    {
        .qual = bcmFieldQualifyDstMac,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyDstMac_map,
    },
    {
        .qual = bcmFieldQualifySrcIp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifySrcIp_map,
    },
    {
        .qual = bcmFieldQualifyDstIp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyDstIp_map,
    },
    {
        .qual = bcmFieldQualifyInPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyInPort_map,
    },
    {
        .qual = bcmFieldQualifyDrop,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyDrop_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlan,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyOuterVlan_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyOuterVlanId_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanPri,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyOuterVlanPri_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanCfi,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyOuterVlanCfi_map,
    },
    {
        .qual = bcmFieldQualifyL4SrcPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyL4SrcPort_map,
    },
    {
        .qual = bcmFieldQualifyL4DstPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyL4DstPort_map,
    },
    {
        .qual = bcmFieldQualifyEtherType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyEtherType_map,
    },
    {
        .qual = bcmFieldQualifyIpProtocol,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyIpProtocol_map,
    },
    {
        .qual = bcmFieldQualifyDSCP,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyDSCP_map,
    },
    {
        .qual = bcmFieldQualifyTtl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyTtl_map,
    },
    {
        .qual = bcmFieldQualifyTcpControl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyTcpControl_map,
    },
    {
        .qual = bcmFieldQualifySrcClassL2,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifySrcClassL2_map,
    },
    {
        .qual = bcmFieldQualifySrcClassL3,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifySrcClassL3_map,
    },
    {
        .qual = bcmFieldQualifySrcClassField,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifySrcClassField_map,
    },
    {
        .qual = bcmFieldQualifyDstClassL2,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyDstClassL2_map,
    },
    {
        .qual = bcmFieldQualifyDstClassL3,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyDstClassL3_map,
    },
    {
        .qual = bcmFieldQualifyDstClassField,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyDstClassField_map,
    },
    {
        .qual = bcmFieldQualifyLoopbackType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyLoopbackType_map,
    },
    {
        .qual = bcmFieldQualifyIpType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyIpType_map,
    },
    {
        .qual = bcmFieldQualifyForwardingType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyForwardingType_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceClassL3,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyInterfaceClassL3_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceClassPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyInterfaceClassPort_map,
    },
    {
        .qual = bcmFieldQualifyL3Routable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyL3Routable_map,
    },
    {
        .qual = bcmFieldQualifyIpFrag,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyIpFrag_map,
    },
    {
        .qual = bcmFieldQualifyVrf,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyVrf_map,
    },
    {
        .qual = bcmFieldQualifyOutPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyOutPort_map,
    },
    {
        .qual = bcmFieldQualifyIp4,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyIp4_map,
    },
    {
        .qual = bcmFieldQualifyIp6,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyIp6_map,
    },
    {
        .qual = bcmFieldQualifyL2Format,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyL2Format_map,
    },
    {
        .qual = bcmFieldQualifyVlanFormat,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyVlanFormat_map,
    },
    {
        .qual = bcmFieldQualifyExtensionHeaderType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyExtensionHeaderType_map,
    },
    {
        .qual = bcmFieldQualifyExtensionHeaderSubCode,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyExtensionHeaderSubCode_map,
    },
    {
        .qual = bcmFieldQualifyL4Ports,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyL4Ports_map,
    },
    {
        .qual = bcmFieldQualifyMirrorCopy,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyMirrorCopy_map,
    },
    {
        .qual = bcmFieldQualifyIcmpTypeCode,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyIcmpTypeCode_map,
    },
    {
        .qual = bcmFieldQualifyColor,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyColor_map,
    },
    {
        .qual = bcmFieldQualifyIntPriority,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyIntPriority_map,
    },
    {
        .qual = bcmFieldQualifyForwardingVlanId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyForwardingVlanId_map,
    },
    {
        .qual = bcmFieldQualifyVpn,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyVpn_map,
    },
    {
        .qual = bcmFieldQualifyEgressClass,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyEgressClass_map,
    },
    {
        .qual = bcmFieldQualifyEgressClassL3Interface,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyEgressClassL3Interface_map,
    },
    {
        .qual = bcmFieldQualifyCpuQueue,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyCpuQueue_map,
    },
    {
        .qual = bcmFieldQualifyIngressClassField,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyIngressClassField_map,
    },
    {
        .qual = bcmFieldQualifyIngressInterfaceClassPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyIngressInterfaceClassPort_map,
    },
    {
        .qual = bcmFieldQualifyIntCongestionNotification,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyIntCongestionNotification_map,
    },
    {
        .qual = bcmFieldQualifyIngressInterfaceClassVPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyIngressInterfaceClassVPort_map,
    },
    {
        .qual = bcmFieldQualifyLoopBackQueue,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyLoopBackQueue_map,
    },
    {
        .qual = bcmFieldQualifyLoopBackSrcGport,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyLoopBackSrcGport_map,
    },
    {
        .qual = bcmFieldQualifyPktIsVisible,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyPktIsVisible_map,
    },
    {
        .qual = bcmFieldQualifyLoopBackCpuMasqueradePktProfile,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyLoopBackCpuMasqueradePktProfile_map,
    },
    {
        .qual = bcmFieldQualifyLoopBackColor,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyLoopBackColor_map,
    },
    {
        .qual = bcmFieldQualifyLoopBackTrafficClass,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyLoopBackTrafficClass_map,
    },
    {
        .qual = bcmFieldQualifyLoopBackPacketProcessingPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyLoopBackPacketProcessingPort_map,
    },
    {
        .qual = bcmFieldQualifyL2PayLoad,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_efp_group_qual_bcmFieldQualifyL2PayLoad_map,
    },
    {
        .qual = bcmFieldQualifySampledPkt,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifySampledPkt_map,
    },
    {
        .qual = bcmFieldQualifySrcIp6SrhValid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_group_qual_bcmFieldQualifySrcIp6SrhValid_map,
    },
};

bcm_field_qual_map_t bcm56990_a0_vfp_group_qual_data[56] = {
    {
        .qual = bcmFieldQualifySrcIp6,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifySrcIp6_map,
    },
    {
        .qual = bcmFieldQualifyDstIp6,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyDstIp6_map,
    },
    {
        .qual = bcmFieldQualifySrcIp6High,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifySrcIp6High_map,
    },
    {
        .qual = bcmFieldQualifyDstIp6High,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyDstIp6High_map,
    },
    {
        .qual = bcmFieldQualifySrcMac,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifySrcMac_map,
    },
    {
        .qual = bcmFieldQualifyDstMac,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyDstMac_map,
    },
    {
        .qual = bcmFieldQualifySnap,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifySnap_map,
    },
    {
        .qual = bcmFieldQualifySrcIp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifySrcIp_map,
    },
    {
        .qual = bcmFieldQualifyDstIp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyDstIp_map,
    },
    {
        .qual = bcmFieldQualifyLlc,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyLlc_map,
    },
    {
        .qual = bcmFieldQualifyInPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyInPort_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlan,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyOuterVlan_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyOuterVlanId_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanPri,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyOuterVlanPri_map,
    },
    {
        .qual = bcmFieldQualifyOuterVlanCfi,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyOuterVlanCfi_map,
    },
    {
        .qual = bcmFieldQualifyL4SrcPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyL4SrcPort_map,
    },
    {
        .qual = bcmFieldQualifyL4DstPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyL4DstPort_map,
    },
    {
        .qual = bcmFieldQualifyEtherType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyEtherType_map,
    },
    {
        .qual = bcmFieldQualifyIpProtocol,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyIpProtocol_map,
    },
    {
        .qual = bcmFieldQualifyDSCP,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyDSCP_map,
    },
    {
        .qual = bcmFieldQualifyTtl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyTtl_map,
    },
    {
        .qual = bcmFieldQualifySrcPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifySrcPort_map,
    },
    {
        .qual = bcmFieldQualifySrcTrunk,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifySrcTrunk_map,
    },
    {
        .qual = bcmFieldQualifyTcpControl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyTcpControl_map,
    },
    {
        .qual = bcmFieldQualifyIpInfo,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyIpInfo_map,
    },
    {
        .qual = bcmFieldQualifyIpProtocolCommon,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyIpProtocolCommon_map,
    },
    {
        .qual = bcmFieldQualifyOuterTpid,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyOuterTpid_map,
    },
    {
        .qual = bcmFieldQualifyInnerIpType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyInnerIpType_map,
    },
    {
        .qual = bcmFieldQualifyIpType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyIpType_map,
    },
    {
        .qual = bcmFieldQualifyHiGig,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyHiGig_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceClassL3,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyInterfaceClassL3_map,
    },
    {
        .qual = bcmFieldQualifyInterfaceClassPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyInterfaceClassPort_map,
    },
    {
        .qual = bcmFieldQualifyIpFrag,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyIpFrag_map,
    },
    {
        .qual = bcmFieldQualifyL2Format,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyL2Format_map,
    },
    {
        .qual = bcmFieldQualifyVlanFormat,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyVlanFormat_map,
    },
    {
        .qual = bcmFieldQualifyExtensionHeaderType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyExtensionHeaderType_map,
    },
    {
        .qual = bcmFieldQualifyExtensionHeaderSubCode,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyExtensionHeaderSubCode_map,
    },
    {
        .qual = bcmFieldQualifyL4Ports,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyL4Ports_map,
    },
    {
        .qual = bcmFieldQualifyTunnelTerminated,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyTunnelTerminated_map,
    },
    {
        .qual = bcmFieldQualifyInnerSrcIp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyInnerSrcIp_map,
    },
    {
        .qual = bcmFieldQualifyInnerDstIp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyInnerDstIp_map,
    },
    {
        .qual = bcmFieldQualifyInnerSrcIp6,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyInnerSrcIp6_map,
    },
    {
        .qual = bcmFieldQualifyInnerDstIp6,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyInnerDstIp6_map,
    },
    {
        .qual = bcmFieldQualifyInnerSrcIp6High,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyInnerSrcIp6High_map,
    },
    {
        .qual = bcmFieldQualifyInnerDstIp6High,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyInnerDstIp6High_map,
    },
    {
        .qual = bcmFieldQualifyInnerTtl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyInnerTtl_map,
    },
    {
        .qual = bcmFieldQualifyInnerTos,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyInnerTos_map,
    },
    {
        .qual = bcmFieldQualifyInnerIpProtocol,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyInnerIpProtocol_map,
    },
    {
        .qual = bcmFieldQualifyInnerIpFrag,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyInnerIpFrag_map,
    },
    {
        .qual = bcmFieldQualifyIcmpTypeCode,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyIcmpTypeCode_map,
    },
    {
        .qual = bcmFieldQualifyInnerIpProtocolCommon,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyInnerIpProtocolCommon_map,
    },
    {
        .qual = bcmFieldQualifyInnerL4SrcPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyInnerL4SrcPort_map,
    },
    {
        .qual = bcmFieldQualifyInnerL4DstPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyInnerL4DstPort_map,
    },
    {
        .qual = bcmFieldQualifySrcGport,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifySrcGport_map,
    },
    {
        .qual = bcmFieldQualifySrcGports,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifySrcGports_map,
    },
    {
        .qual = bcmFieldQualifyOpaqueTagType,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_group_qual_bcmFieldQualifyOpaqueTagType_map,
    },
};

bcm_field_qual_map_t bcm56990_a0_em_group_qual_data[0] = {
};

const bcm_field_action_map_t bcm56990_a0_ifp_action_data[170] = {
    {
        .action = bcmFieldActionCosQNew,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionCosQNew_map,
    },
    {
        .action = bcmFieldActionCosQCpuNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionCosQCpuNew_map,
    },
    {
        .action = bcmFieldActionUcastCosQNew,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionUcastCosQNew_map,
    },
    {
        .action = bcmFieldActionMcastCosQNew,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionMcastCosQNew_map,
    },
    {
        .action = bcmFieldActionPrioPktNew,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionPrioPktNew_map,
    },
    {
        .action = bcmFieldActionPrioPktTos,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionPrioPktTos_map,
    },
    {
        .action = bcmFieldActionPrioPktCancel,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionPrioPktCancel_map,
    },
    {
        .action = bcmFieldActionPrioIntNew,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionPrioIntNew_map,
    },
    {
        .action = bcmFieldActionPrioIntTos,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionPrioIntTos_map,
    },
    {
        .action = bcmFieldActionPrioIntCancel,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionPrioIntCancel_map,
    },
    {
        .action = bcmFieldActionDscpNew,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionDscpNew_map,
    },
    {
        .action = bcmFieldActionDscpCancel,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionDscpCancel_map,
    },
    {
        .action = bcmFieldActionEcnNew,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionEcnNew_map,
    },
    {
        .action = bcmFieldActionCopyToCpu,
        .flags = 0,
        .num_maps = 4,
        .map = bcm_field_ifp_action_bcmFieldActionCopyToCpu_map,
    },
    {
        .action = bcmFieldActionCopyToCpuCancel,
        .flags = 0,
        .num_maps = 9,
        .map = bcm_field_ifp_action_bcmFieldActionCopyToCpuCancel_map,
    },
    {
        .action = bcmFieldActionSwitchToCpuCancel,
        .flags = 0,
        .num_maps = 9,
        .map = bcm_field_ifp_action_bcmFieldActionSwitchToCpuCancel_map,
    },
    {
        .action = bcmFieldActionSwitchToCpuReinstate,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionSwitchToCpuReinstate_map,
    },
    {
        .action = bcmFieldActionTimeStampToCpu,
        .flags = 0,
        .num_maps = 4,
        .map = bcm_field_ifp_action_bcmFieldActionTimeStampToCpu_map,
    },
    {
        .action = bcmFieldActionRedirectPort,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRedirectPort_map,
    },
    {
        .action = bcmFieldActionRedirectTrunk,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRedirectTrunk_map,
    },
    {
        .action = bcmFieldActionRedirectCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRedirectCancel_map,
    },
    {
        .action = bcmFieldActionRedirectPbmp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRedirectPbmp_map,
    },
    {
        .action = bcmFieldActionRedirectIpmc,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRedirectIpmc_map,
    },
    {
        .action = bcmFieldActionRedirectMcast,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRedirectMcast_map,
    },
    {
        .action = bcmFieldActionRedirectVlan,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRedirectVlan_map,
    },
    {
        .action = bcmFieldActionRedirectBcastPbmp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRedirectBcastPbmp_map,
    },
    {
        .action = bcmFieldActionEgressMask,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionEgressMask_map,
    },
    {
        .action = bcmFieldActionEgressPortsAdd,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionEgressPortsAdd_map,
    },
    {
        .action = bcmFieldActionDrop,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionDrop_map,
    },
    {
        .action = bcmFieldActionDropCancel,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionDropCancel_map,
    },
    {
        .action = bcmFieldActionMirrorOverride,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionMirrorOverride_map,
    },
    {
        .action = bcmFieldActionMirrorIngress,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionMirrorIngress_map,
    },
    {
        .action = bcmFieldActionMirrorEgress,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionMirrorEgress_map,
    },
    {
        .action = bcmFieldActionL3Switch,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_action_bcmFieldActionL3Switch_map,
    },
    {
        .action = bcmFieldActionL3SwitchCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionL3SwitchCancel_map,
    },
    {
        .action = bcmFieldActionDropPrecedence,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionDropPrecedence_map,
    },
    {
        .action = bcmFieldActionRpDrop,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpDrop_map,
    },
    {
        .action = bcmFieldActionRpDropCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpDropCancel_map,
    },
    {
        .action = bcmFieldActionRpDropPrecedence,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpDropPrecedence_map,
    },
    {
        .action = bcmFieldActionRpCopyToCpu,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_action_bcmFieldActionRpCopyToCpu_map,
    },
    {
        .action = bcmFieldActionRpCopyToCpuCancel,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionRpCopyToCpuCancel_map,
    },
    {
        .action = bcmFieldActionRpSwitchToCpuCancel,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionRpSwitchToCpuCancel_map,
    },
    {
        .action = bcmFieldActionRpSwitchToCpuReinstate,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpSwitchToCpuReinstate_map,
    },
    {
        .action = bcmFieldActionRpTimeStampToCpu,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_action_bcmFieldActionRpTimeStampToCpu_map,
    },
    {
        .action = bcmFieldActionRpDscpNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpDscpNew_map,
    },
    {
        .action = bcmFieldActionRpDscpCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpDscpCancel_map,
    },
    {
        .action = bcmFieldActionRpEcnNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpEcnNew_map,
    },
    {
        .action = bcmFieldActionRpPrioPktNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpPrioPktNew_map,
    },
    {
        .action = bcmFieldActionRpPrioPktTos,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpPrioPktTos_map,
    },
    {
        .action = bcmFieldActionRpPrioPktCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpPrioPktCancel_map,
    },
    {
        .action = bcmFieldActionRpCosQNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpCosQNew_map,
    },
    {
        .action = bcmFieldActionRpUcastCosQNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpUcastCosQNew_map,
    },
    {
        .action = bcmFieldActionRpMcastCosQNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpMcastCosQNew_map,
    },
    {
        .action = bcmFieldActionRpPrioIntNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpPrioIntNew_map,
    },
    {
        .action = bcmFieldActionRpPrioIntTos,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpPrioIntTos_map,
    },
    {
        .action = bcmFieldActionRpPrioIntCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpPrioIntCancel_map,
    },
    {
        .action = bcmFieldActionYpDrop,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpDrop_map,
    },
    {
        .action = bcmFieldActionYpDropCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpDropCancel_map,
    },
    {
        .action = bcmFieldActionYpDropPrecedence,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpDropPrecedence_map,
    },
    {
        .action = bcmFieldActionYpCopyToCpu,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_action_bcmFieldActionYpCopyToCpu_map,
    },
    {
        .action = bcmFieldActionYpCopyToCpuCancel,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionYpCopyToCpuCancel_map,
    },
    {
        .action = bcmFieldActionYpSwitchToCpuCancel,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionYpSwitchToCpuCancel_map,
    },
    {
        .action = bcmFieldActionYpSwitchToCpuReinstate,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpSwitchToCpuReinstate_map,
    },
    {
        .action = bcmFieldActionYpTimeStampToCpu,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_action_bcmFieldActionYpTimeStampToCpu_map,
    },
    {
        .action = bcmFieldActionYpDscpNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpDscpNew_map,
    },
    {
        .action = bcmFieldActionYpDscpCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpDscpCancel_map,
    },
    {
        .action = bcmFieldActionYpEcnNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpEcnNew_map,
    },
    {
        .action = bcmFieldActionYpPrioPktNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpPrioPktNew_map,
    },
    {
        .action = bcmFieldActionYpPrioPktTos,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpPrioPktTos_map,
    },
    {
        .action = bcmFieldActionYpPrioPktCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpPrioPktCancel_map,
    },
    {
        .action = bcmFieldActionYpCosQNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpCosQNew_map,
    },
    {
        .action = bcmFieldActionYpUcastCosQNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpUcastCosQNew_map,
    },
    {
        .action = bcmFieldActionYpMcastCosQNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpMcastCosQNew_map,
    },
    {
        .action = bcmFieldActionYpPrioIntNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpPrioIntNew_map,
    },
    {
        .action = bcmFieldActionYpPrioIntTos,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpPrioIntTos_map,
    },
    {
        .action = bcmFieldActionYpPrioIntCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpPrioIntCancel_map,
    },
    {
        .action = bcmFieldActionGpDrop,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpDrop_map,
    },
    {
        .action = bcmFieldActionGpDropCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpDropCancel_map,
    },
    {
        .action = bcmFieldActionGpDropPrecedence,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpDropPrecedence_map,
    },
    {
        .action = bcmFieldActionGpCopyToCpu,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_action_bcmFieldActionGpCopyToCpu_map,
    },
    {
        .action = bcmFieldActionGpCopyToCpuCancel,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionGpCopyToCpuCancel_map,
    },
    {
        .action = bcmFieldActionGpSwitchToCpuCancel,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionGpSwitchToCpuCancel_map,
    },
    {
        .action = bcmFieldActionGpSwitchToCpuReinstate,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpSwitchToCpuReinstate_map,
    },
    {
        .action = bcmFieldActionGpTimeStampToCpu,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_action_bcmFieldActionGpTimeStampToCpu_map,
    },
    {
        .action = bcmFieldActionGpDscpNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpDscpNew_map,
    },
    {
        .action = bcmFieldActionGpDscpCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpDscpCancel_map,
    },
    {
        .action = bcmFieldActionGpEcnNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpEcnNew_map,
    },
    {
        .action = bcmFieldActionGpTosPrecedenceNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpTosPrecedenceNew_map,
    },
    {
        .action = bcmFieldActionGpTosPrecedenceCopy,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpTosPrecedenceCopy_map,
    },
    {
        .action = bcmFieldActionGpPrioPktNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpPrioPktNew_map,
    },
    {
        .action = bcmFieldActionGpPrioPktTos,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpPrioPktTos_map,
    },
    {
        .action = bcmFieldActionGpPrioPktCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpPrioPktCancel_map,
    },
    {
        .action = bcmFieldActionGpCosQNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpCosQNew_map,
    },
    {
        .action = bcmFieldActionGpUcastCosQNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpUcastCosQNew_map,
    },
    {
        .action = bcmFieldActionGpMcastCosQNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpMcastCosQNew_map,
    },
    {
        .action = bcmFieldActionGpPrioIntNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpPrioIntNew_map,
    },
    {
        .action = bcmFieldActionGpPrioIntTos,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpPrioIntTos_map,
    },
    {
        .action = bcmFieldActionGpPrioIntCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpPrioIntCancel_map,
    },
    {
        .action = bcmFieldActionDoNotChangeTtl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionDoNotChangeTtl_map,
    },
    {
        .action = bcmFieldActionNewClassId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionNewClassId_map,
    },
    {
        .action = bcmFieldActionMultipathHash,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionMultipathHash_map,
    },
    {
        .action = bcmFieldActionServicePoolIdNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionServicePoolIdNew_map,
    },
    {
        .action = bcmFieldActionRedirectEgrNextHop,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_action_bcmFieldActionRedirectEgrNextHop_map,
    },
    {
        .action = bcmFieldActionEgressClassSelect,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionEgressClassSelect_map,
    },
    {
        .action = bcmFieldActionPolicerGroup,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionPolicerGroup_map,
    },
    {
        .action = bcmFieldActionStatGroup,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_action_bcmFieldActionStatGroup_map,
    },
    {
        .action = bcmFieldActionIngSampleEnable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionIngSampleEnable_map,
    },
    {
        .action = bcmFieldActionUnmodifiedPacketRedirectPort,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_action_bcmFieldActionUnmodifiedPacketRedirectPort_map,
    },
    {
        .action = bcmFieldActionPfcClassNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionPfcClassNew_map,
    },
    {
        .action = bcmFieldActionRpIntCongestionNotificationNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpIntCongestionNotificationNew_map,
    },
    {
        .action = bcmFieldActionYpIntCongestionNotificationNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpIntCongestionNotificationNew_map,
    },
    {
        .action = bcmFieldActionGpIntCongestionNotificationNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpIntCongestionNotificationNew_map,
    },
    {
        .action = bcmFieldActionGroupClassSet,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGroupClassSet_map,
    },
    {
        .action = bcmFieldActionDot1pPreserve,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionDot1pPreserve_map,
    },
    {
        .action = bcmFieldActionGpDot1pPreserve,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpDot1pPreserve_map,
    },
    {
        .action = bcmFieldActionYpDot1pPreserve,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpDot1pPreserve_map,
    },
    {
        .action = bcmFieldActionRpDot1pPreserve,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpDot1pPreserve_map,
    },
    {
        .action = bcmFieldActionDscpPreserve,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionDscpPreserve_map,
    },
    {
        .action = bcmFieldActionGpDscpPreserve,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpDscpPreserve_map,
    },
    {
        .action = bcmFieldActionYpDscpPreserve,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpDscpPreserve_map,
    },
    {
        .action = bcmFieldActionRpDscpPreserve,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpDscpPreserve_map,
    },
    {
        .action = bcmFieldActionChangeL2FieldsCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionChangeL2FieldsCancel_map,
    },
    {
        .action = bcmFieldActionBFDSessionIdNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionBFDSessionIdNew_map,
    },
    {
        .action = bcmFieldActionCosMapNew,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionCosMapNew_map,
    },
    {
        .action = bcmFieldActionGpCosMapNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionGpCosMapNew_map,
    },
    {
        .action = bcmFieldActionYpCosMapNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionYpCosMapNew_map,
    },
    {
        .action = bcmFieldActionRpCosMapNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionRpCosMapNew_map,
    },
    {
        .action = bcmFieldActionChangeL2Fields,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionChangeL2Fields_map,
    },
    {
        .action = bcmFieldActionExactMatchClassId,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionExactMatchClassId_map,
    },
    {
        .action = bcmFieldActionTrunkRandomRoundRobinHashCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionTrunkRandomRoundRobinHashCancel_map,
    },
    {
        .action = bcmFieldActionEcmpRandomRoundRobinHashCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionEcmpRandomRoundRobinHashCancel_map,
    },
    {
        .action = bcmFieldActionPacketTraceEnable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionPacketTraceEnable_map,
    },
    {
        .action = bcmFieldActionUntaggedPacketPriorityNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionUntaggedPacketPriorityNew_map,
    },
    {
        .action = bcmFieldActionDoNotCutThrough,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionDoNotCutThrough_map,
    },
    {
        .action = bcmFieldActionProtectionSwitchingDrop,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionProtectionSwitchingDrop_map,
    },
    {
        .action = bcmFieldActionServicePoolIdPrecedenceNew,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_action_bcmFieldActionServicePoolIdPrecedenceNew_map,
    },
    {
        .action = bcmFieldActionEgressTimeStampInsert,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionEgressTimeStampInsert_map,
    },
    {
        .action = bcmFieldActionIngressTimeStampInsert,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionIngressTimeStampInsert_map,
    },
    {
        .action = bcmFieldActionIngressTimeStampInsertCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionIngressTimeStampInsertCancel_map,
    },
    {
        .action = bcmFieldActionEgressTimeStampInsertCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionEgressTimeStampInsertCancel_map,
    },
    {
        .action = bcmFieldActionDynamicEcmpEnable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionDynamicEcmpEnable_map,
    },
    {
        .action = bcmFieldActionDgm,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionDgm_map,
    },
    {
        .action = bcmFieldActionDgmThreshold,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionDgmThreshold_map,
    },
    {
        .action = bcmFieldActionDgmBias,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionDgmBias_map,
    },
    {
        .action = bcmFieldActionDgmCost,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionDgmCost_map,
    },
    {
        .action = bcmFieldActionIntEncapEnable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionIntEncapEnable_map,
    },
    {
        .action = bcmFieldActionMatchPbmpDrop,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionMatchPbmpDrop_map,
    },
    {
        .action = bcmFieldActionMatchPbmpRedirect,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_action_bcmFieldActionMatchPbmpRedirect_map,
    },
    {
        .action = bcmFieldActionIntEncapDisable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionIntEncapDisable_map,
    },
    {
        .action = bcmFieldActionElephantLookupEnable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionElephantLookupEnable_map,
    },
    {
        .action = bcmFieldActionElephantLookupDisable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionElephantLookupDisable_map,
    },
    {
        .action = bcmFieldActionElephantColorEnable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionElephantColorEnable_map,
    },
    {
        .action = bcmFieldActionElephantColorDisable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionElephantColorDisable_map,
    },
    {
        .action = bcmFieldActionElephantQueueEnable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionElephantQueueEnable_map,
    },
    {
        .action = bcmFieldActionElephantQueueDisable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionElephantQueueDisable_map,
    },
    {
        .action = bcmFieldActionDlbEcmpMonitorEnable,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_ifp_action_bcmFieldActionDlbEcmpMonitorEnable_map,
    },
    {
        .action = bcmFieldActionMirrorZeroingEnable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionMirrorZeroingEnable_map,
    },
    {
        .action = bcmFieldActionMirrorOnDropEnable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionMirrorOnDropEnable_map,
    },
    {
        .action = bcmFieldActionMirrorOnDropProfileSet,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionMirrorOnDropProfileSet_map,
    },
    {
        .action = bcmFieldActionIntCongestionNotification,
        .flags = 0,
        .num_maps = 3,
        .map = bcm_field_ifp_action_bcmFieldActionIntCongestionNotification_map,
    },
    {
        .action = bcmFieldActionIntStatReq0,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionIntStatReq0_map,
    },
    {
        .action = bcmFieldActionIntStatReq1,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionIntStatReq1_map,
    },
    {
        .action = bcmFieldActionFlexSampleProfile,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionFlexSampleProfile_map,
    },
    {
        .action = bcmFieldActionMirrorOnDropDisable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionMirrorOnDropDisable_map,
    },
    {
        .action = bcmFieldActionMirrorFlowClass,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionMirrorFlowClass_map,
    },
    {
        .action = bcmFieldActionIntActionProfileSet,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionIntActionProfileSet_map,
    },
    {
        .action = bcmFieldActionIntTermActionProfileSet,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionIntTermActionProfileSet_map,
    },
    {
        .action = bcmFieldActionIntFlowClassSet,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionIntFlowClassSet_map,
    },
    {
        .action = bcmFieldActionIntApplyOnSampledPkt,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionIntApplyOnSampledPkt_map,
    },
    {
        .action = bcmFieldActionIntLoopbackProfileSet,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_ifp_action_bcmFieldActionIntLoopbackProfileSet_map,
    },
};

const bcm_field_action_map_t bcm56990_a0_efp_action_data[15] = {
    {
        .action = bcmFieldActionDscpNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_action_bcmFieldActionDscpNew_map,
    },
    {
        .action = bcmFieldActionEcnNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_action_bcmFieldActionEcnNew_map,
    },
    {
        .action = bcmFieldActionDrop,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_action_bcmFieldActionDrop_map,
    },
    {
        .action = bcmFieldActionDropCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_action_bcmFieldActionDropCancel_map,
    },
    {
        .action = bcmFieldActionOuterVlanNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_action_bcmFieldActionOuterVlanNew_map,
    },
    {
        .action = bcmFieldActionOuterVlanPrioNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_action_bcmFieldActionOuterVlanPrioNew_map,
    },
    {
        .action = bcmFieldActionOuterVlanCfiNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_action_bcmFieldActionOuterVlanCfiNew_map,
    },
    {
        .action = bcmFieldActionOuterTpidNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_action_bcmFieldActionOuterTpidNew_map,
    },
    {
        .action = bcmFieldActionStatGroup,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_action_bcmFieldActionStatGroup_map,
    },
    {
        .action = bcmFieldActionLoopbackSrcPortGportNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_action_bcmFieldActionLoopbackSrcPortGportNew_map,
    },
    {
        .action = bcmFieldActionLoopbackCpuMasqueradePktProfileNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_action_bcmFieldActionLoopbackCpuMasqueradePktProfileNew_map,
    },
    {
        .action = bcmFieldActionLoopbackPacketProcessingPortNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_action_bcmFieldActionLoopbackPacketProcessingPortNew_map,
    },
    {
        .action = bcmFieldActionMplsLabel1ExpNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_action_bcmFieldActionMplsLabel1ExpNew_map,
    },
    {
        .action = bcmFieldActionDscpMask,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_action_bcmFieldActionDscpMask_map,
    },
    {
        .action = bcmFieldActionEcnMask,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_efp_action_bcmFieldActionEcnMask_map,
    },
};

const bcm_field_action_map_t bcm56990_a0_vfp_action_data[31] = {
    {
        .action = bcmFieldActionCosQCpuNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionCosQCpuNew_map,
    },
    {
        .action = bcmFieldActionPrioIntNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionPrioIntNew_map,
    },
    {
        .action = bcmFieldActionCopyToCpu,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_vfp_action_bcmFieldActionCopyToCpu_map,
    },
    {
        .action = bcmFieldActionCopyToCpuCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionCopyToCpuCancel_map,
    },
    {
        .action = bcmFieldActionDrop,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionDrop_map,
    },
    {
        .action = bcmFieldActionDropCancel,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionDropCancel_map,
    },
    {
        .action = bcmFieldActionDropPrecedence,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionDropPrecedence_map,
    },
    {
        .action = bcmFieldActionClassDestSet,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionClassDestSet_map,
    },
    {
        .action = bcmFieldActionClassSourceSet,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionClassSourceSet_map,
    },
    {
        .action = bcmFieldActionVrfSet,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionVrfSet_map,
    },
    {
        .action = bcmFieldActionOuterVlanNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionOuterVlanNew_map,
    },
    {
        .action = bcmFieldActionOuterVlanAdd,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionOuterVlanAdd_map,
    },
    {
        .action = bcmFieldActionOuterVlanLookup,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionOuterVlanLookup_map,
    },
    {
        .action = bcmFieldActionOuterVlanPrioNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionOuterVlanPrioNew_map,
    },
    {
        .action = bcmFieldActionOuterVlanCfiNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionOuterVlanCfiNew_map,
    },
    {
        .action = bcmFieldActionDoNotCheckVlan,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionDoNotCheckVlan_map,
    },
    {
        .action = bcmFieldActionDoNotLearn,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionDoNotLearn_map,
    },
    {
        .action = bcmFieldActionStatGroup,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionStatGroup_map,
    },
    {
        .action = bcmFieldActionQosMapIdNew,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionQosMapIdNew_map,
    },
    {
        .action = bcmFieldActionL3IngressSet,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionL3IngressSet_map,
    },
    {
        .action = bcmFieldActionEnableVlanCheck,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionEnableVlanCheck_map,
    },
    {
        .action = bcmFieldActionMplsLookupEnable,
        .flags = 0,
        .num_maps = 2,
        .map = bcm_field_vfp_action_bcmFieldActionMplsLookupEnable_map,
    },
    {
        .action = bcmFieldActionTerminationAllowed,
        .flags = 0,
        .num_maps = 7,
        .map = bcm_field_vfp_action_bcmFieldActionTerminationAllowed_map,
    },
    {
        .action = bcmFieldActionVisibilityEnable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionVisibilityEnable_map,
    },
    {
        .action = bcmFieldActionL3IngressStrengthSet,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionL3IngressStrengthSet_map,
    },
    {
        .action = bcmFieldActionL3TunnelTypeSet,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionL3TunnelTypeSet_map,
    },
    {
        .action = bcmFieldActionL3TunnelAuto,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionL3TunnelAuto_map,
    },
    {
        .action = bcmFieldActionUseOuterHeaderDscp,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionUseOuterHeaderDscp_map,
    },
    {
        .action = bcmFieldActionUseOuterHeaderTtl,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionUseOuterHeaderTtl_map,
    },
    {
        .action = bcmFieldActionDisableInnerHeaderDscpChange,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionDisableInnerHeaderDscpChange_map,
    },
    {
        .action = bcmFieldActionBfdEnable,
        .flags = 0,
        .num_maps = 1,
        .map = bcm_field_vfp_action_bcmFieldActionBfdEnable_map,
    },
};

const bcm_field_action_map_t bcm56990_a0_em_action_data[0] = {
};

/******************************************************************************
* Public functions
 */

int bcm56990_a0_ltsw_field_auto_enum_to_lt_enum_value_get(
    int unit,
    bcm_field_qualify_t qual,
    uint32 qual_enum,
    bcm_qual_field_t *qual_info)
{
    SHR_FUNC_ENTER(unit);

    switch (qual) {
        case bcmFieldQualifyL2Format:
            switch (qual_enum) {
                case bcmFieldL2FormatAny:
                    qual_info->enum_name = bcm56990_a0_enum_str_any;
                    break;
                case bcmFieldL2FormatEthII:
                    qual_info->enum_name = bcm56990_a0_enum_str_ether_2;
                    break;
                case bcmFieldL2FormatSnap:
                    qual_info->enum_name = bcm56990_a0_enum_str_snap;
                    break;
                case bcmFieldL2FormatLlc:
                    qual_info->enum_name = bcm56990_a0_enum_str_llc;
                    break;
                case bcmFieldL2Format802dot3:
                    qual_info->enum_name = bcm56990_a0_enum_str_ieee802dot3;
                    break;
                default:
                    LOG_ERROR(BSL_LS_BCM_FP,
                        (BSL_META_U(unit, "Error: Enum value not correct.\n")));
                    SHR_ERR_EXIT(SHR_E_UNAVAIL);

            }
            break;

        case bcmFieldQualifyIpProtocolCommon:
        case bcmFieldQualifyInnerIpProtocolCommon:
            switch (qual_enum) {
                case bcmFieldIpProtocolCommonTcp:
                    qual_info->enum_name = bcm56990_a0_enum_str_tcp;
                    break;
                case bcmFieldIpProtocolCommonUdp:
                    qual_info->enum_name = bcm56990_a0_enum_str_udp;
                    break;
                case bcmFieldIpProtocolCommonIgmp:
                    qual_info->enum_name = bcm56990_a0_enum_str_igmp;
                    break;
                case bcmFieldIpProtocolCommonIcmp:
                    qual_info->enum_name = bcm56990_a0_enum_str_icmp;
                    break;
                case bcmFieldIpProtocolCommonIp6Icmp:
                    qual_info->enum_name = bcm56990_a0_enum_str_ip6_icmp;
                    break;
                case bcmFieldIpProtocolCommonIp6HopByHop:
                    qual_info->enum_name = bcm56990_a0_enum_str_ip6_hop_by_hop;
                    break;
                case bcmFieldIpProtocolCommonIpInIp:
                    qual_info->enum_name = bcm56990_a0_enum_str_ip_in_ip;
                    break;
                case bcmFieldIpProtocolCommonTcpUdp:
                    qual_info->enum_name = bcm56990_a0_enum_str_tcp_udp;
                    break;
                default:
                    LOG_ERROR(BSL_LS_BCM_FP,
                        (BSL_META_U(unit, "Error: Enum value not correct.\n")));
                    SHR_ERR_EXIT(SHR_E_UNAVAIL);

            }
            break;

        case bcmFieldQualifyForwardingType:
            switch (qual_enum) {
                case bcmFieldForwardingTypeAny:
                    qual_info->enum_name = bcm56990_a0_enum_str_any;
                    break;
                case bcmFieldForwardingTypeL2:
                    qual_info->enum_name = bcm56990_a0_enum_str_l2;
                    break;
                case bcmFieldForwardingTypeL3:
                    qual_info->enum_name = bcm56990_a0_enum_str_l3;
                    break;
                case bcmFieldForwardingTypeL2Shared:
                    qual_info->enum_name = bcm56990_a0_enum_str_l2_shared;
                    break;
                case bcmFieldForwardingTypeL2Independent:
                    qual_info->enum_name = bcm56990_a0_enum_str_l2_independent;
                    break;
                case bcmFieldForwardingTypeL3Direct:
                    qual_info->enum_name = bcm56990_a0_enum_str_l3_direct;
                    break;
                case bcmFieldForwardingTypeMpls:
                    qual_info->enum_name = bcm56990_a0_enum_str_mpls;
                    break;
                case bcmFieldForwardingTypeTrill:
                    qual_info->enum_name = bcm56990_a0_enum_str_trill;
                    break;
                case bcmFieldForwardingTypePortExtender:
                    qual_info->enum_name = bcm56990_a0_enum_str_port_extender;
                    break;
                default:
                    LOG_ERROR(BSL_LS_BCM_FP,
                        (BSL_META_U(unit, "Error: Enum value not correct.\n")));
                    SHR_ERR_EXIT(SHR_E_UNAVAIL);

            }
            break;

        case bcmFieldQualifyInnerIpType:
        case bcmFieldQualifyIpType:
            switch (qual_enum) {
                case bcmFieldIpTypeAny:
                    qual_info->enum_name = bcm56990_a0_enum_str_any;
                    break;
                case bcmFieldIpTypeNonIp:
                    qual_info->enum_name = bcm56990_a0_enum_str_non_ip;
                    break;
                case bcmFieldIpTypeIpv4NoOpts:
                    qual_info->enum_name = bcm56990_a0_enum_str_ip4_no_options;
                    break;
                case bcmFieldIpTypeIpv4WithOpts:
                    qual_info->enum_name = bcm56990_a0_enum_str_ip4_options;
                    break;
                case bcmFieldIpTypeIpv4Any:
                    qual_info->enum_name = bcm56990_a0_enum_str_any_ip4;
                    break;
                case bcmFieldIpTypeIpv6NoExtHdr:
                    qual_info->enum_name = bcm56990_a0_enum_str_ip6_no_ext_hdr;
                    break;
                case bcmFieldIpTypeIpv6OneExtHdr:
                    qual_info->enum_name = bcm56990_a0_enum_str_ip6_one_ext_hdr;
                    break;
                case bcmFieldIpTypeIpv6TwoExtHdr:
                    qual_info->enum_name = bcm56990_a0_enum_str_ip6_two_ext_hdr;
                    break;
                case bcmFieldIpTypeIpv6:
                    qual_info->enum_name = bcm56990_a0_enum_str_any_ip6;
                    break;
                case bcmFieldIpTypeIp:
                    qual_info->enum_name = bcm56990_a0_enum_str_ip;
                    break;
                case bcmFieldIpTypeArp:
                    qual_info->enum_name = bcm56990_a0_enum_str_arp;
                    break;
                case bcmFieldIpTypeArpRequest:
                    qual_info->enum_name = bcm56990_a0_enum_str_arp_request;
                    break;
                case bcmFieldIpTypeArpReply:
                    qual_info->enum_name = bcm56990_a0_enum_str_arp_reply;
                    break;
                case bcmFieldIpTypeMplsUnicast:
                    qual_info->enum_name = bcm56990_a0_enum_str_uc_mpls;
                    break;
                case bcmFieldIpTypeMplsMulticast:
                    qual_info->enum_name = bcm56990_a0_enum_str_mc_mpls;
                    break;
                case bcmFieldIpTypeTrill:
                    qual_info->enum_name = bcm56990_a0_enum_str_trill;
                    break;
                case bcmFieldIpTypeMim:
                    qual_info->enum_name = bcm56990_a0_enum_str_mim;
                    break;
                default:
                    LOG_ERROR(BSL_LS_BCM_FP,
                        (BSL_META_U(unit, "Error: Enum value not correct.\n")));
                    SHR_ERR_EXIT(SHR_E_UNAVAIL);

            }
            break;

        case bcmFieldQualifyTunnelType:
            switch (qual_enum) {
                case bcmFieldTunnelTypeAny:
                    qual_info->enum_name = bcm56990_a0_enum_str_any;
                    break;
                case bcmFieldTunnelTypeIp:
                    qual_info->enum_name = bcm56990_a0_enum_str_ip;
                    break;
                case bcmFieldTunnelTypeMpls:
                    qual_info->enum_name = bcm56990_a0_enum_str_mpls;
                    break;
                case bcmFieldTunnelTypeMim:
                    qual_info->enum_name = bcm56990_a0_enum_str_mim;
                    break;
                case bcmFieldTunnelTypeAutoMulticast:
                    qual_info->enum_name = bcm56990_a0_enum_str_auto_ip4_mc;
                    break;
                case bcmFieldTunnelTypeTrill:
                    qual_info->enum_name = bcm56990_a0_enum_str_trill;
                    break;
                case bcmFieldTunnelTypeL2Gre:
                    qual_info->enum_name = bcm56990_a0_enum_str_l2gre;
                    break;
                case bcmFieldTunnelTypeVxlan:
                    qual_info->enum_name = bcm56990_a0_enum_str_vxlan;
                    break;
                case bcmFieldTunnelTypeNone:
                    qual_info->enum_name = bcm56990_a0_enum_str_none;
                    break;
                default:
                    LOG_ERROR(BSL_LS_BCM_FP,
                        (BSL_META_U(unit, "Error: Enum value not correct.\n")));
                    SHR_ERR_EXIT(SHR_E_UNAVAIL);

            }
            break;

        case bcmFieldQualifyIpFrag:
        case bcmFieldQualifyInnerIpFrag:
            switch (qual_enum) {
                case bcmFieldIpFragAny:
                    qual_info->enum_name = bcm56990_a0_enum_str_any;
                    break;
                case bcmFieldIpFragNon:
                    qual_info->enum_name = bcm56990_a0_enum_str_non;
                    break;
                case bcmFieldIpFragFirst:
                    qual_info->enum_name = bcm56990_a0_enum_str_first;
                    break;
                case bcmFieldIpFragNonOrFirst:
                    qual_info->enum_name = bcm56990_a0_enum_str_non_or_first;
                    break;
                case bcmFieldIpFragNotFirst:
                    qual_info->enum_name = bcm56990_a0_enum_str_not_first;
                    break;
                default:
                    LOG_ERROR(BSL_LS_BCM_FP,
                        (BSL_META_U(unit, "Error: Enum value not correct.\n")));
                    SHR_ERR_EXIT(SHR_E_UNAVAIL);

            }
            break;

        default:
            SHR_ERR_EXIT(SHR_E_NONE);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcm56990_a0_ltsw_field_auto_lt_enum_value_to_enum_get(
    int unit,
    bcm_field_qualify_t qual,
    bcm_qual_field_t *qual_info,
    uint32 *qual_enum)
{
    SHR_FUNC_ENTER(unit);

    switch (qual) {
        case bcmFieldQualifyL2Format:
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_any) == 0) {
                    *qual_enum = bcmFieldL2FormatAny;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_ether_2) == 0) {
                    *qual_enum = bcmFieldL2FormatEthII;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_snap) == 0) {
                    *qual_enum = bcmFieldL2FormatSnap;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_llc) == 0) {
                    *qual_enum = bcmFieldL2FormatLlc;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_ieee802dot3) == 0) {
                    *qual_enum = bcmFieldL2Format802dot3;
                    break;
                }
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
                break;

        case bcmFieldQualifyIpProtocolCommon:
        case bcmFieldQualifyInnerIpProtocolCommon:
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_tcp) == 0) {
                    *qual_enum = bcmFieldIpProtocolCommonTcp;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_udp) == 0) {
                    *qual_enum = bcmFieldIpProtocolCommonUdp;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_igmp) == 0) {
                    *qual_enum = bcmFieldIpProtocolCommonIgmp;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_icmp) == 0) {
                    *qual_enum = bcmFieldIpProtocolCommonIcmp;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_ip6_icmp) == 0) {
                    *qual_enum = bcmFieldIpProtocolCommonIp6Icmp;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_ip6_hop_by_hop) == 0) {
                    *qual_enum = bcmFieldIpProtocolCommonIp6HopByHop;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_ip_in_ip) == 0) {
                    *qual_enum = bcmFieldIpProtocolCommonIpInIp;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_tcp_udp) == 0) {
                    *qual_enum = bcmFieldIpProtocolCommonTcpUdp;
                    break;
                }
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
                break;

        case bcmFieldQualifyForwardingType:
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_any) == 0) {
                    *qual_enum = bcmFieldForwardingTypeAny;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_l2) == 0) {
                    *qual_enum = bcmFieldForwardingTypeL2;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_l3) == 0) {
                    *qual_enum = bcmFieldForwardingTypeL3;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_l2_shared) == 0) {
                    *qual_enum = bcmFieldForwardingTypeL2Shared;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_l2_independent) == 0) {
                    *qual_enum = bcmFieldForwardingTypeL2Independent;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_l3_direct) == 0) {
                    *qual_enum = bcmFieldForwardingTypeL3Direct;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_mpls) == 0) {
                    *qual_enum = bcmFieldForwardingTypeMpls;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_trill) == 0) {
                    *qual_enum = bcmFieldForwardingTypeTrill;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_port_extender) == 0) {
                    *qual_enum = bcmFieldForwardingTypePortExtender;
                    break;
                }
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
                break;

        case bcmFieldQualifyInnerIpType:
        case bcmFieldQualifyIpType:
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_any) == 0) {
                    *qual_enum = bcmFieldIpTypeAny;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_non_ip) == 0) {
                    *qual_enum = bcmFieldIpTypeNonIp;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_ip4_no_options) == 0) {
                    *qual_enum = bcmFieldIpTypeIpv4NoOpts;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_ip4_options) == 0) {
                    *qual_enum = bcmFieldIpTypeIpv4WithOpts;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_any_ip4) == 0) {
                    *qual_enum = bcmFieldIpTypeIpv4Any;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_ip6_no_ext_hdr) == 0) {
                    *qual_enum = bcmFieldIpTypeIpv6NoExtHdr;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_ip6_one_ext_hdr) == 0) {
                    *qual_enum = bcmFieldIpTypeIpv6OneExtHdr;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_ip6_two_ext_hdr) == 0) {
                    *qual_enum = bcmFieldIpTypeIpv6TwoExtHdr;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_any_ip6) == 0) {
                    *qual_enum = bcmFieldIpTypeIpv6;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_ip) == 0) {
                    *qual_enum = bcmFieldIpTypeIp;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_arp) == 0) {
                    *qual_enum = bcmFieldIpTypeArp;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_arp_request) == 0) {
                    *qual_enum = bcmFieldIpTypeArpRequest;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_arp_reply) == 0) {
                    *qual_enum = bcmFieldIpTypeArpReply;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_uc_mpls) == 0) {
                    *qual_enum = bcmFieldIpTypeMplsUnicast;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_mc_mpls) == 0) {
                    *qual_enum = bcmFieldIpTypeMplsMulticast;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_trill) == 0) {
                    *qual_enum = bcmFieldIpTypeTrill;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_mim) == 0) {
                    *qual_enum = bcmFieldIpTypeMim;
                    break;
                }
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
                break;

        case bcmFieldQualifyTunnelType:
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_any) == 0) {
                    *qual_enum = bcmFieldTunnelTypeAny;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_ip) == 0) {
                    *qual_enum = bcmFieldTunnelTypeIp;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_mpls) == 0) {
                    *qual_enum = bcmFieldTunnelTypeMpls;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_mim) == 0) {
                    *qual_enum = bcmFieldTunnelTypeMim;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_auto_ip4_mc) == 0) {
                    *qual_enum = bcmFieldTunnelTypeAutoMulticast;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_trill) == 0) {
                    *qual_enum = bcmFieldTunnelTypeTrill;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_l2gre) == 0) {
                    *qual_enum = bcmFieldTunnelTypeL2Gre;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_vxlan) == 0) {
                    *qual_enum = bcmFieldTunnelTypeVxlan;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_none) == 0) {
                    *qual_enum = bcmFieldTunnelTypeNone;
                    break;
                }
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
                break;

        case bcmFieldQualifyIpFrag:
        case bcmFieldQualifyInnerIpFrag:
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_any) == 0) {
                    *qual_enum = bcmFieldIpFragAny;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_non) == 0) {
                    *qual_enum = bcmFieldIpFragNon;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_first) == 0) {
                    *qual_enum = bcmFieldIpFragFirst;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_non_or_first) == 0) {
                    *qual_enum = bcmFieldIpFragNonOrFirst;
                    break;
                }
                if (strcmp(qual_info->enum_name, bcm56990_a0_enum_str_not_first) == 0) {
                    *qual_enum = bcmFieldIpFragNotFirst;
                    break;
                }
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
                break;

        default:
            SHR_ERR_EXIT(SHR_E_NONE);
    }

exit:
    SHR_FUNC_EXIT();
}

