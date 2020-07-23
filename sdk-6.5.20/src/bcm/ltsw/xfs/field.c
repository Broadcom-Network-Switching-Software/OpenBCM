/*! \file field.c
 *
 * Field Driver for XFS chips.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/field.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/field_int.h>
#include <bcm_int/ltsw/udf_int.h>
#include <bcm_int/ltsw/mirror.h>
#include <bcm_int/ltsw/stack.h>
#include <bcm_int/ltsw/l3_egress.h>
#include <bcm_int/ltsw/xfs/field.h>
#include <bcm_int/ltsw/xfs/udf.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/mbcm/field.h>
#include <bcm_int/common/multicast.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_FP

/* In TD4, FP have 4 pipes where in every pipe we can match on 48 unique
 * IPBM indexes which is split into 3 LT fields per pipe that is
 * ING_OBJ_BUS_IPBM_15_0_PIPEx, 31_16_PIPEx, 47_32_PIPEx */
#define BCMI_TD4_IPBM_INDEX_PER_PIPE 48

/* Compression is not enabled on current NPL */
const bool compression_support = 0;

typedef struct xfs_field_egr_mask_profile_s {
    uint8_t         mask_target; /* Egr Mask Target - L2, L3, L2_L3 */
    uint8_t         mask_action; /* Egr Mask Action - AND, OR */
    bcm_pbmp_t      pbmp;        /* Egress Mask */
} xfs_field_egr_mask_profile_t;

static ltsw_field_profile_info_t xfs_field_egr_mask_profile_info[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */
int
xfs_field_ing_match_id_enums_init(
        int unit,
        bcmint_field_stage_info_t *stage_info)
{
    int map_count = 0, iter = 0;
    bcm_field_qual_map_t *lt_map_ref = NULL;
    SHR_FUNC_ENTER(unit);

    /* bcmFieldQualifyL2PktType */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifyL2PktType,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeEnumtoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L2_HDR_L2")) {
            (lt_map_ref->map[iter]).value = bcmFieldL2PktTypeAny;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L2_HDR_SNAP_OR_LLC")) {
            (lt_map_ref->map[iter]).value = bcmFieldL2PktTypeSnapOrLLC;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L2_HDR_HG3_BASE")) {
            (lt_map_ref->map[iter]).value = bcmFieldL2PktTypeHG3Base;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L2_HDR_HG3_EXT_0")) {
            (lt_map_ref->map[iter]).value = bcmFieldL2PktTypeHG3Ext0;
        } else if (0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L2_HDR_SVTAG")) {
            (lt_map_ref->map[iter]).value = bcmFieldL2PktTypeSvtag;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* bcmFieldQualifyOverlayL2PktType */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifyOverlayL2PktType,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeEnumtoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L2_HDR_L2")) {
            (lt_map_ref->map[iter]).value = bcmFieldL2PktTypeAny;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L2_HDR_SNAP_OR_LLC")) {
            (lt_map_ref->map[iter]).value = bcmFieldL2PktTypeSnapOrLLC;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* bcmFieldQualifyIpOptionHdrType */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifyIpOptionHdrType,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeEnumtoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_AUTH_EXT_1")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeAH;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_AUTH_EXT_2")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeAHExt2;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_ESP_EXT")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeESP;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_FRAG_EXT_1")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeFrag;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_FRAG_EXT_2")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeFragExt2;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* bcmFieldQualifyOverlayIpOptionHdrType */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifyOverlayIpOptionHdrType,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeEnumtoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_AUTH_EXT_1")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeAH;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_AUTH_EXT_2")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeAHExt2;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_ESP_EXT")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeESP;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_FRAG_EXT_1")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeFrag;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_FRAG_EXT_2")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeFragExt2;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* bcmFieldQualifyPktType */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifyPktType,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeEnumtoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_ARP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeArp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_BFD")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpBfd;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_ETHERTYPE")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeEthertype;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_GPE")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGpe;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_GRE")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGre;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_GRE_CHKSUM")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGreChksum;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_GRE_KEY")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGreKey;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_GRE_ROUT")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGreRout;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_GRE_SEQ")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGreSeq;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_ICMP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpIcmp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_IFA_METADATA")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpIfa;
        } else if (0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_IFA_HEADER")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpIfa;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_IGMP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpIgmp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_IPV4")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIp4Any;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_IPV6")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIp6Any;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS_ACH")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsAch;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS0")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsOneLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS1")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsTwoLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS2")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsThreeLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS3")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsFourLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS4")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsFiveLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS5")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsSixLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS6")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsSevenLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_P_1588")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktType1588;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_RARP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeRarp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_TCP_FIRST_4BYTES")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpTcp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_UDP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpUdp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_UNKNOWN_L3")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeUnknownL3;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_UNKNOWN_L4")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeUnknownL4;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_UNKNOWN_L5")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeUnknownL5;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_VXLAN")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpVxlan;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* bcmFieldQualifyOverlayPktType */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifyOverlayPktType,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeEnumtoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_ARP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeArp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_BFD")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpBfd;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_ETHERTYPE")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeEthertype;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_ICMP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpIcmp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_IGMP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpIgmp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_IPV4")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIp4Any;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_IPV6")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIp6Any;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_P_1588")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktType1588;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_RARP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeRarp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_TCP_FIRST_4BYTES")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpTcp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_UDP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpUdp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_UNKNOWN_L3")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeUnknownL3;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_UNKNOWN_L4")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeUnknownL4;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L3_L4_HDR_UNKNOWN_L5")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeUnknownL5;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* bcmFieldQualifySysHdrType */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifySysHdrType,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeEnumtoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_SYS_HDR_LOOPBACK")) {
            (lt_map_ref->map[iter]).value = bcmFieldSysHdrLoopbackAny;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_SYS_HDR_EP_NIH")) {
            (lt_map_ref->map[iter]).value = bcmFieldSysHdrRecircle;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* Macro bcmFieldQualifyVlanFormat*/
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifyVlanFormat,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeMacrotoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L2_HDR_ITAG")) {
            (lt_map_ref->map[iter]).value = BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L2_HDR_OTAG")) {
            (lt_map_ref->map[iter]).value = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* Macro bcmFieldQualifyOverlayVlanFormat*/
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifyOverlayVlanFormat,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeMacrotoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L2_HDR_ITAG")) {
            (lt_map_ref->map[iter]).value = BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_INNER_L2_HDR_OTAG")) {
            (lt_map_ref->map[iter]).value = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_field_egr_match_id_enums_init(
        int unit,
        bcmint_field_stage_info_t *stage_info)
{
    int map_count = 0, iter = 0;
    bcm_field_qual_map_t *lt_map_ref = NULL;
    SHR_FUNC_ENTER(unit);

    /* bcmFieldQualifyL2PktType */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifyL2PktType,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeEnumtoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L2_HDR_L2")) {
            (lt_map_ref->map[iter]).value = bcmFieldL2PktTypeAny;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L2_HDR_SNAP_OR_LLC")) {
            (lt_map_ref->map[iter]).value = bcmFieldL2PktTypeSnapOrLLC;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L2_HDR_HG3_BASE")) {
            (lt_map_ref->map[iter]).value = bcmFieldL2PktTypeHG3Base;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L2_HDR_HG3_EXT_0")) {
            (lt_map_ref->map[iter]).value = bcmFieldL2PktTypeHG3Ext0;
        } else if (0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L2_HDR_SVTAG")) {
            (lt_map_ref->map[iter]).value = bcmFieldL2PktTypeSvtag;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* bcmFieldQualifyIpOptionHdrType */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifyIpOptionHdrType,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeEnumtoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_AUTH_EXT_1")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeAH;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_AUTH_EXT_2")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeAHExt2;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_ESP_EXT")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeESP;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_FRAG_EXT_1")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeFrag;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_FRAG_EXT_2")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeFragExt2;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* bcmFieldQualifyPktType */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifyPktType,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeEnumtoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_ARP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeArp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_BFD")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpBfd;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_ETHERTYPE")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeEthertype;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_GPE")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGpe;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_GRE")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGre;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_GRE_CHKSUM")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGreChksum;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_GRE_KEY")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGreKey;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_GRE_ROUT")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGreRout;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_GRE_SEQ")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGreSeq;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_ICMP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpIcmp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_IFA_METADATA")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpIfa;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_IFA_HEADER")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpIfa;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_IGMP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpIgmp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_IPV4")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIp4Any;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_IPV6")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIp6Any;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_MPLS_ACH")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsAch;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_MPLS0")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsOneLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_MPLS1")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsTwoLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_MPLS2")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsThreeLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_MPLS3")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsFourLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_MPLS4")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsFiveLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_MPLS5")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsSixLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_MPLS6")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsSevenLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_P_1588")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktType1588;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_RARP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeRarp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_TCP_FIRST_4BYTES")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpTcp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_UDP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpUdp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_UNKNOWN_L3")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeUnknownL3;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_UNKNOWN_L4")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeUnknownL4;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_UNKNOWN_L5")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeUnknownL5;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L3_L4_HDR_VXLAN")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpVxlan;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* bcmFieldQualifySysHdrType */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifySysHdrType,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeEnumtoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_SYS_HDR_LOOPBACK")) {
            (lt_map_ref->map[iter]).value = bcmFieldSysHdrLoopbackAny;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_SYS_HDR_EP_NIH")) {
            (lt_map_ref->map[iter]).value = bcmFieldSysHdrRecircle;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* Macro bcmFieldQualifyVlanFormat*/
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifyVlanFormat,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeMacrotoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L2_HDR_ITAG")) {
            (lt_map_ref->map[iter]).value = BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "EGRESS_PKT_FWD_L2_HDR_OTAG")) {
            (lt_map_ref->map[iter]).value = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_field_vlan_match_id_enums_init(
        int unit,
        bcmint_field_stage_info_t *stage_info)
{
    int map_count = 0, iter = 0;
    bcm_field_qual_map_t *lt_map_ref = NULL;
    SHR_FUNC_ENTER(unit);

    /* bcmFieldQualifyL2PktType */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifyL2PktType,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeEnumtoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L2_HDR_L2")) {
            (lt_map_ref->map[iter]).value = bcmFieldL2PktTypeAny;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L2_HDR_SNAP_OR_LLC")) {
            (lt_map_ref->map[iter]).value = bcmFieldL2PktTypeSnapOrLLC;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L2_HDR_HG3_BASE")) {
            (lt_map_ref->map[iter]).value = bcmFieldL2PktTypeHG3Base;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L2_HDR_HG3_EXT_0")) {
            (lt_map_ref->map[iter]).value = bcmFieldL2PktTypeHG3Ext0;
        } else if (0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L2_HDR_SVTAG")) {
            (lt_map_ref->map[iter]).value = bcmFieldL2PktTypeSvtag;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* bcmFieldQualifyIpOptionHdrType */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifyIpOptionHdrType,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeEnumtoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_AUTH_EXT_1")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeAH;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_AUTH_EXT_2")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeAHExt2;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_ESP_EXT")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeESP;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_FRAG_EXT_1")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeFrag;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_FRAG_EXT_2")) {
            (lt_map_ref->map[iter]).value = bcmFieldIpOptionHdrTypeFragExt2;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* bcmFieldQualifyPktType */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifyPktType,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeEnumtoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_ARP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeArp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_BFD")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpBfd;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_ETHERTYPE")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeEthertype;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_GPE")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGpe;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_GRE")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGre;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_GRE_CHKSUM")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGreChksum;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_GRE_KEY")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGreKey;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_GRE_ROUT")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGreRout;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_GRE_SEQ")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpGreSeq;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_ICMP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpIcmp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_IFA_METADATA")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpIfa;
        } else if (0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_IFA_HEADER")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpIfa;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_IGMP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpIgmp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_IPV4")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIp4Any;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_IPV6")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIp6Any;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS_ACH")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsAch;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS0")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsOneLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS1")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsTwoLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS2")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsThreeLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS3")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsFourLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS4")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsFiveLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS5")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsSixLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS6")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeMplsSevenLabel;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_P_1588")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktType1588;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_RARP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeRarp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_TCP_FIRST_4BYTES")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpTcp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_UDP")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpUdp;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_UNKNOWN_L3")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeUnknownL3;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_UNKNOWN_L4")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeUnknownL4;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_UNKNOWN_L5")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeUnknownL5;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L3_L4_HDR_VXLAN")) {
            (lt_map_ref->map[iter]).value = bcmFieldPktTypeIpVxlan;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* bcmFieldQualifySysHdrType */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifySysHdrType,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeEnumtoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_SYS_HDR_LOOPBACK")) {
            (lt_map_ref->map[iter]).value = bcmFieldSysHdrLoopbackAny;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_SYS_HDR_EP_NIH")) {
            (lt_map_ref->map[iter]).value = bcmFieldSysHdrRecircle;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* Macro bcmFieldQualifyVlanFormat*/
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, bcmFieldQualifyVlanFormat,
                                    stage_info, 1, 0, &map_count,
                                    &lt_map_ref));

    (lt_map_ref)->data_type = bcmFieldDataTypeMacrotoLtField;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L2_HDR_ITAG")) {
            (lt_map_ref->map[iter]).value = BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
        } else if(0 == strcmp((lt_map_ref->map[iter]).lt_field_name,
                    "INGRESS_PKT_OUTER_L2_HDR_OTAG")) {
            (lt_map_ref->map[iter]).value = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
ltsw_field_lt_entry_commit(int unit,
                           bcmlt_entry_handle_t *template,
                           bcmlt_opcode_t operation,
                           bcmlt_priority_level_t priority,
                           char *table_name,
                           char *key_name,
                           uint64_t key_val,
                           char *field_name,
                           uint64_t field_val,
                           int free_template)
{
    int dunit;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    if (*template == BCMLT_INVALID_HDL) {
        /* Entry handle allocate for given template */
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_allocate(dunit, table_name, template));
    }

    /* Add given key to the template. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(*template, key_name, key_val));

    if (field_name != NULL) {
        /* Add given field value to the given field_name. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*template, field_name, field_val));
    }

    /* Perform the operation */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_lt_entry_commit(unit, *template, operation, priority));

exit:
    if (free_template) {
       (void) bcmlt_entry_free(*template);
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */
/*!
 * \brief Clear IFP presel configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_ifp_presel_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field presel group table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_IFP_PRESEL_GRP_TEMPLATEs));

    /* Clear Field presel entry table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_IFP_PRESEL_TEMPLATEs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear VFP presel configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_vfp_presel_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field presel group table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_VFP_PRESEL_GRP_TEMPLATEs));

    /* Clear Field presel entry table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_VFP_PRESEL_TEMPLATEs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear EFP presel configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_efp_presel_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field presel group table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_EFP_PRESEL_GRP_TEMPLATEs));

    /* Clear Field presel entry table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_EFP_PRESEL_TEMPLATEs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear EM presel configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_em_presel_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field presel group table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, "DT_EM_FP_PRESEL_GRP_TEMPLATE"));

    /* Clear Field presel entry table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, "DT_EM_FP_PRESEL_TEMPLATE"));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear FT presel configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_ft_presel_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field presel group table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, "DT_EM_FT_PRESEL_GRP_TEMPLATE"));

    /* Clear Field presel entry table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, "DT_EM_FT_PRESEL_TEMPLATE"));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear IFP group configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_ifp_group_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field group table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_IFP_GRP_TEMPLATEs));

    /* Clear Field PDD table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_IFP_PDD_TEMPLATEs));

    /* Clear Field SBR table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_IFP_SBR_TEMPLATEs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear VFP group configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_vfp_group_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field group table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_VFP_GRP_TEMPLATEs));

    /* Clear Field PDD table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_VFP_PDD_TEMPLATEs));

    /* Clear Field SBR table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_VFP_SBR_TEMPLATEs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear EFP group configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_efp_group_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field group table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_EFP_GRP_TEMPLATEs));

    /* Clear Field PDD table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_EFP_PDD_TEMPLATEs));

    /* Clear Field SBR table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_EFP_SBR_TEMPLATEs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear EM group configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_em_group_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field group table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, "DT_EM_FP_GRP_TEMPLATE"));

    /* Clear Field PDD table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, "DT_EM_FP_PDD_TEMPLATE"));

    /* Clear Field SBR table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, "DT_EM_FP_SBR_TEMPLATE"));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear FT group configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_ft_group_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field group table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, "DT_EM_FT_GRP_TEMPLATE"));

    /* Clear Field PDD table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, "DT_EM_FT_PDD_TEMPLATE"));

    /* Clear Field SBR table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, "DT_EM_FT_SBR_TEMPLATE"));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear IFP entry configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_ifp_entry_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field Rule table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_IFP_RULE_TEMPLATEs));

    /* Clear Field Policy table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_IFP_ACTION_TEMPLATEs));

    /* Clear Field Entry table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_IFP_ENTRYs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear VFP entry configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_vfp_entry_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field Rule table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_VFP_RULE_TEMPLATEs));

    /* Clear Field Policy table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_VFP_ACTION_TEMPLATEs));

    /* Clear Field Entry table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_VFP_ENTRYs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear EFP entry configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_efp_entry_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field Rule table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_EFP_RULE_TEMPLATEs));

    /* Clear Field Policy table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_EFP_ACTION_TEMPLATEs));

    /* Clear Field Entry table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, DT_EFP_ENTRYs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear EM entry configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_em_entry_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field Rule table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, "DT_EM_FP_RULE_TEMPLATE"));

    /* Clear Field Policy table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, "DT_EM_FP_ACTION_TEMPLATE"));

    /* Clear Field Entry table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, "DT_EM_FP_ENTRY"));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear FT entry configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_ft_entry_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field Rule table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, "DT_EM_FT_RULE_TEMPLATE"));

    /* Clear Field Policy table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, "DT_EM_FT_ACTION_TEMPLATE"));

    /* Clear Field Entry table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, "DT_EM_FT_ENTRY"));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear Field IFP configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xfs_ltsw_ifp_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear field presel configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_ifp_presel_clear(unit));

    /* Clear field group configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_ifp_group_clear(unit));

    /* Clear field entry configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_ifp_entry_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear Field VFP configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xfs_ltsw_vfp_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear field presel configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_vfp_presel_clear(unit));

    /* Clear field group configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_vfp_group_clear(unit));

    /* Clear field entry configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_vfp_entry_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear Field EFP configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xfs_ltsw_efp_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear field presel configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_efp_presel_clear(unit));

    /* Clear field group configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_efp_group_clear(unit));

    /* Clear field entry configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_efp_entry_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear Field EM configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xfs_ltsw_em_clear(int unit)
{
    int rv, dunit;
    const char *tbl_desc;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    rv = bcmlt_table_desc_get(dunit, "DT_EM_FP_PRESEL_TEMPLATE", &tbl_desc);
    if (SHR_FAILURE(rv)) {
        SHR_EXIT();
    }

    /* Clear field presel configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_em_presel_clear(unit));

    /* Clear field group configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_em_group_clear(unit));

    /* Clear field entry configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_em_entry_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear Field FT configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xfs_ltsw_ft_clear(int unit)
{
    int rv, dunit;
    const char *tbl_desc;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);
    rv = bcmlt_table_desc_get(dunit, "DT_EM_FT_PRESEL_TEMPLATE", &tbl_desc);
    if (SHR_FAILURE(rv)) {
        SHR_EXIT();
    }

    /* Clear field presel configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_ft_presel_clear(unit));

    /* Clear field group configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_ft_group_clear(unit));

    /* Clear field entry configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_ft_entry_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear Field configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xfs_ltsw_field_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear IFP configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_ifp_clear(unit));

    /* Clear VFP configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_vfp_clear(unit));

    /* Clear EFP configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_efp_clear(unit));

    /* Clear EM configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_em_clear(unit));

    /* Clear FT configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_ft_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_field_entry_compr_qual_validate(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        uint32_t grp_flags,
        bcm_field_qset_t *qset,
        uint32_t *compr_flags)
{
    bool all_enabled = false;
    SHR_FUNC_ENTER(unit);

    if (!(grp_flags & BCMINT_FIELD_GROUP_COMPR_SRC_IP_ENABLED) &&
        !(grp_flags & BCMINT_FIELD_GROUP_COMPR_SRC_IP6_ENABLED) &&
        !(grp_flags & BCMINT_FIELD_GROUP_COMPR_DST_IP_ENABLED) &&
        !(grp_flags & BCMINT_FIELD_GROUP_COMPR_DST_IP6_ENABLED)) {
        *compr_flags = BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP_L4PORT |
                       BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP6_L4PORT |
                       BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP_L4PORT |
                       BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP6_L4PORT;
        all_enabled = TRUE;
    }

    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp)) {
        if ((all_enabled == TRUE) ||
            (grp_flags & BCMINT_FIELD_GROUP_COMPR_SRC_IP_ENABLED)) {
            *compr_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP_L4PORT;
        }
    }

    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp6)) {
        if ((all_enabled == TRUE) ||
            (grp_flags & BCMINT_FIELD_GROUP_COMPR_SRC_IP6_ENABLED)) {
            *compr_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP6_L4PORT;
        }
    }

    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp)) {
        if ((all_enabled == TRUE) ||
            (grp_flags & BCMINT_FIELD_GROUP_COMPR_DST_IP_ENABLED)) {
            *compr_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP_L4PORT;
        }
    }

    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp6)) {
        if ((all_enabled == TRUE) ||
            (grp_flags & BCMINT_FIELD_GROUP_COMPR_DST_IP6_ENABLED)) {
            *compr_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP6_L4PORT;
        }
    }

    SHR_FUNC_EXIT();
}


static int
ltsw_field_group_compr_qual_validate(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        uint32_t grp_flags,
        bcm_field_qset_t *qset,
        uint32_t *compr_flags)
{
    SHR_FUNC_ENTER(unit);

    if (!(grp_flags & BCM_FIELD_GROUP_CREATE_WITH_SRC_IP_COMPRESSION) &&
        !(grp_flags & BCM_FIELD_GROUP_CREATE_WITH_SRC_IP6_COMPRESSION) &&
        !(grp_flags & BCM_FIELD_GROUP_CREATE_WITH_DST_IP_COMPRESSION) &&
        !(grp_flags & BCM_FIELD_GROUP_CREATE_WITH_DST_IP6_COMPRESSION)) {
        if (!BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp) &&
            !BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp6) &&
            !BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp) &&
            !BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp6)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (!(stage_info->flags & BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP4_ONLY) ||
            !(stage_info->flags & BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP6_ONLY)) {
            if ((BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp) ||
                 BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp6)) &&
                 !(BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyL4SrcPort))) {
                 SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        }

        if (!(stage_info->flags & BCMINT_FIELD_STAGE_COMPRESSION_DST_IP4_ONLY) ||
            !(stage_info->flags & BCMINT_FIELD_STAGE_COMPRESSION_DST_IP6_ONLY)) {
            if ((BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp) ||
                 BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp6)) &&
                 !(BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyL4DstPort))) {
                 SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        }
        *compr_flags = BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP_L4PORT |
                       BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP6_L4PORT |
                       BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP_L4PORT |
                       BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP6_L4PORT;
        SHR_EXIT();
    }

    if (grp_flags & BCM_FIELD_GROUP_CREATE_WITH_SRC_IP_COMPRESSION) {
        if (!BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (!(stage_info->flags & BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP4_ONLY) &&
            !(BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyL4SrcPort))) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        *compr_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP_L4PORT;
    }

    if (grp_flags & BCM_FIELD_GROUP_CREATE_WITH_SRC_IP6_COMPRESSION) {
        if (!BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp6)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (!(stage_info->flags & BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP6_ONLY) &&
            !(BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyL4SrcPort))) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        *compr_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP6_L4PORT;
    }

    if (grp_flags & BCM_FIELD_GROUP_CREATE_WITH_DST_IP_COMPRESSION) {
        if (!BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (!(stage_info->flags & BCMINT_FIELD_STAGE_COMPRESSION_DST_IP4_ONLY) &&
            !(BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyL4DstPort))) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        *compr_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP_L4PORT;
    }

    if (grp_flags & BCM_FIELD_GROUP_CREATE_WITH_DST_IP6_COMPRESSION) {
        if (!BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp6)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (!(stage_info->flags & BCMINT_FIELD_STAGE_COMPRESSION_DST_IP6_ONLY) &&
            !(BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyL4DstPort))) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        *compr_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP6_L4PORT;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_compression_set(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_entry_oper_t *entry_oper,
        bcm_field_qset_t *qset,
        uint32_t group_flags,
        bcmlt_entry_handle_t templ)
{
    int iter;
    uint8_t num_types = 0;
    uint32_t c_valid_flags = 0x0;
    bcm_field_qualify_t qual[4] = {bcmFieldQualifySrcIp,
                                   bcmFieldQualifyDstIp,
                                   bcmFieldQualifySrcIp6,
                                   bcmFieldQualifyDstIp6};
    int compr_flags[4] = {BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP_L4PORT,
                          BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP_L4PORT,
                          BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP6_L4PORT,
                          BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP6_L4PORT};
    const char *comp_data[4] = {0};
    char *comp_type[4] = {"SRC_IPV4", "DST_IPV4", "SRC_IPV6", "DST_IPV6"};

    SHR_FUNC_ENTER(unit);

    if (templ == BCMLT_INVALID_HDL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (entry_oper == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_group_compr_qual_validate(unit, stage_info,
                                                  group_flags,
                                                  qset, &c_valid_flags));
    } else {
        bcmi_field_ha_group_oper_t *group_oper;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_oper_lookup(unit, entry_oper->group_id,
                                            stage_info,
                                            &group_oper));
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_entry_compr_qual_validate(unit, stage_info,
                                                  group_oper->group_flags,
                                                  qset, &c_valid_flags));
    }

    for (iter = 0; iter < 4; iter++) {
        if (BCM_FIELD_QSET_TEST(*qset, qual[iter])) {
            if (!(c_valid_flags & compr_flags[iter])) {
                continue;
            }
            comp_data[num_types++] = comp_type[iter];
        } else if ((entry_oper != NULL) &&
                   (entry_oper->entry_flags & compr_flags[iter])) {
            comp_data[num_types++] = comp_type[iter];
        }
    }

    if (num_types > 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_symbol_add(
                  templ, stage_info->tbls_info->compression_type_fid,
                  0, comp_data, num_types));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(templ,
                stage_info->tbls_info->compression_type_cnt_fid,
                num_types));
    }
exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_compression_del(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_qset_t *qset,
        bcmlt_entry_handle_t templ)
{
    int iter;
    int cnt = 0;
    uint64_t n_types = 0;
    uint32_t num_types = 0;
    bcm_field_qualify_t qual[4] = {bcmFieldQualifySrcIp,
                                   bcmFieldQualifyDstIp,
                                   bcmFieldQualifySrcIp6,
                                   bcmFieldQualifyDstIp6};
    const char *comp_data[4] = {0};
    char *comp_field = NULL;
    char *comp_type[4] = {"SRC_IPV4", "DST_IPV4", "SRC_IPV6", "DST_IPV6"};

    SHR_FUNC_ENTER(unit);

    if (templ == BCMLT_INVALID_HDL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (iter = 0; iter < 4; iter++) {
        if (BCM_FIELD_QSET_TEST(*qset, qual[iter])) {
            comp_field = comp_type[iter];
            break;
        }
    }

    if (comp_field == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(templ,
            stage_info->tbls_info->compression_type_cnt_fid,
            &n_types));

    if (n_types == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_symbol_get(
              templ,
              stage_info->tbls_info->compression_type_fid,
              0,
              comp_data,
              4,
              &num_types));
    cnt = 0;
    for (iter = 0; iter < n_types; iter++) {
         comp_data[cnt] = comp_data[iter];
         if (sal_strcmp(comp_data[iter], comp_field)) {
             cnt++;
         }
    }
    n_types = cnt;

    if (cnt) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_symbol_add(
                templ,
                stage_info->tbls_info->compression_type_fid,
                0,
                comp_data,
                cnt));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(templ,
                               stage_info->tbls_info->compression_type_cnt_fid,
                               n_types));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_group_udf_qset_update_with_hints(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_field_hint_t *hint,
    bcm_field_qset_t *udf_qset,
    bcmi_field_udf_qset_bitmap_t *udf_qset_bitmap)
{
    int flags = 0;
    uint32_t c;
    uint8_t cbmap_ct;
    int8_t shift_val;
    bcmi_field_qualify_t qual;
    bcm_udf_multi_chunk_info_t info;
    int len = 0;
    int valid_flag = (BCMINT_UDF_LT_ANCHOR_TYPE_OUTER |
                      BCMINT_UDF_LT_ANCHOR_TYPE_INNER);
    uint32_t *chunk_bitmap = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&info, 0x0, sizeof(info));
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_udf_multi_chunk_info_get(unit, udf_id, &info, &flags));

    if (hint != NULL) {
        len = hint->value;
    } else {
        len = info.width;
    }

    valid_flag = flags & valid_flag;
    if (valid_flag == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    if (valid_flag & BCMINT_UDF_LT_ANCHOR_TYPE_OUTER) {
        BCMI_FIELD_QSET_ADD(*udf_qset, bcmiFieldQualifyUdfExtOuter);
    } else {
        BCMI_FIELD_QSET_ADD(*udf_qset, bcmiFieldQualifyUdfExtInner);
    }

    /* Iterate through supported 4/2/1 Byte chunk_bitmaps */
    for (cbmap_ct = 0; cbmap_ct < 3; cbmap_ct++) {
        for (c = 0; c < BCMINT_UDF_MAX_CHUNKS(unit); c++) {
            if (len <= 0) {
                continue;
            }
            if (cbmap_ct == 0) {
                if (BCM_UDF_CBMP_CHUNK_TEST(info.four_byte_chunk_bmap, c) == 0) {
                    continue;
                }
                qual = bcmiFieldQualifyB4Chunk0 + c;
                shift_val = 4;
                chunk_bitmap = &udf_qset_bitmap->chunk_b4_bitmap[c];
            } else if (cbmap_ct == 1) {
                if (BCM_UDF_CBMP_CHUNK_TEST(info.two_byte_chunk_bmap, c) == 0) {
                    continue;
                }
                qual = bcmiFieldQualifyB2Chunk0 + c;
                shift_val = 2;
                chunk_bitmap = &udf_qset_bitmap->chunk_b2_bitmap[c];
            } else {
                if (BCM_UDF_CBMP_CHUNK_TEST(info.one_byte_chunk_bmap, c) == 0) {
                    continue;
                }
                qual = bcmiFieldQualifyB1Chunk0 + c;
                shift_val = 1;
                chunk_bitmap = &udf_qset_bitmap->chunk_b1_bitmap[c];
            }

            BCM_FIELD_QSET_ADD(*udf_qset, qual);
            shift_val = (len < shift_val) ? len : shift_val;
            SHR_BITSET_RANGE(chunk_bitmap, 0, shift_val * 8);
            len -= shift_val;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_udf_qual_set(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    bcm_udf_id_t udf_id,
    int length,
    uint8_t *data,
    uint8_t *mask)
{
    int dunit = 0;
    int map_ct = 0;
    int flags = 0;
    uint32_t c;
    bool found;
    uint64_t data64;
    uint64_t mask64;
    uint8_t cbmap_ct;
    int8_t shift_val;
    bcm_field_qset_t qset;
    bcmi_field_qualify_t qual;
    bcm_udf_multi_chunk_info_t info;
    char *lt_field_name = NULL;
    char *qual_mask_field = NULL;
    bcm_field_qual_map_t *lt_map_ref = NULL;
    bcmlt_entry_handle_t rule_templ = BCMLT_INVALID_HDL;
    int rv = 0;
    int len = 0;
    int bytes = 0;
    int byte_to_copy = 0;
    bcm_field_hint_t hint;

    SHR_FUNC_ENTER(unit);

    BCM_FIELD_QSET_INIT(qset);
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_field_udf_qset_set(unit, 1, &udf_id, &qset));

    sal_memset(&info, 0x0, sizeof(info));
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_udf_multi_chunk_info_get(unit, udf_id, &info, &flags));

    /* If provided entry data length is greater than UDF width */
    if ((length > info.width) || (length <= 0)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    hint.hint_type = bcmFieldHintTypeExtraction;
    hint.qual = bcmFieldQualifyUdf;
    hint.udf_id = 0;
    rv = bcmint_field_hints_get(unit, group_oper->hintid, &hint);
    if ((rv == SHR_E_NONE) && (hint.udf_id == udf_id)) {
       if (length > (hint.value)) {
           SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
       }
    }
    len = length - 1;

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Entry handle allocate for rule Template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->rule_sid,
                              &rule_templ));

    /* Add rule_template_id field to Rule LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(rule_templ,
                               stage_info->tbls_info->rule_key_fid,
                               entry_oper->entry_id));


    /* Iterate through supported 4/2/1 Byte chunk_bitmaps */
    for (cbmap_ct = 0; cbmap_ct < 3; cbmap_ct++) {
        for (c = 0; c < BCMINT_UDF_MAX_CHUNKS(unit); c++) {
            if (len < 0) {
                continue;
            }
            data64 = mask64 = 0;
            if (cbmap_ct == 0) {
                if (BCM_UDF_CBMP_CHUNK_TEST(info.four_byte_chunk_bmap, c) == 0) {
                    continue;
                }
                qual = bcmiFieldQualifyB4Chunk0 + c;
                shift_val = 4;
            } else if (cbmap_ct == 1) {
                if (BCM_UDF_CBMP_CHUNK_TEST(info.two_byte_chunk_bmap, c) == 0) {
                    continue;
                }
                qual = bcmiFieldQualifyB2Chunk0 + c;
                shift_val = 2;
            } else {
                if (BCM_UDF_CBMP_CHUNK_TEST(info.one_byte_chunk_bmap, c) == 0) {
                    continue;
                }
                qual = bcmiFieldQualifyB1Chunk0 + c;
                shift_val = 1;
            }

            FP_HA_GROUP_OPER_QSET_TEST(group_oper, qual, found);
            if (found == FALSE) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            for (bytes = shift_val-1; bytes >= 0; bytes--) {
                if (len < 0) {
                   break;
                }
                data64 |= data[byte_to_copy] << ((bytes) * 8);
                mask64 |= mask[byte_to_copy] << ((bytes) * 8);
                byte_to_copy++;
                len--;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_qual_map_find(unit, qual,
                                            stage_info, 0, 0,
                                            &map_ct, &lt_map_ref));

            BCMINT_FIELD_MEM_ALLOC
                (lt_field_name,
                 strlen((lt_map_ref->map[0]).lt_field_name) + 7,
                 "UDF qualifier data field");
            sal_strcpy(lt_field_name, (lt_map_ref->map[0]).lt_field_name);

            if (flags & BCMINT_UDF_LT_ANCHOR_TYPE_INNER) {
                sal_strcat(lt_field_name, "_INNER");
            } else {
                sal_strcat(lt_field_name, "_OUTER");
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(rule_templ,
                                             lt_field_name,
                                             c, &data64,
                                             1));
            if ((stage_info->stage_id != bcmiFieldStageExactMatch) &&
                (stage_info->stage_id != bcmiFieldStageFlowTracker)) {
                BCMINT_FIELD_MEM_ALLOC
                    (qual_mask_field,
                     (strlen(lt_field_name) + 6),
                     "qualifier mask field");
                sal_strcpy(qual_mask_field, lt_field_name);
                sal_strcat(qual_mask_field, "_MASK");
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(rule_templ,
                                                 qual_mask_field,
                                                 c, &mask64,
                                                 1));
                sal_free(qual_mask_field);
                qual_mask_field = NULL;
            }
            sal_free(lt_field_name);
            lt_field_name = NULL;
        }
    }

    /* Update rule template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(rule_templ, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != rule_templ) {
        (void) bcmlt_entry_free(rule_templ);
    }
    if (lt_field_name) {
        sal_free(lt_field_name);
    }
    if (qual_mask_field) {
        sal_free(qual_mask_field);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_udf_qual_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    bcm_udf_id_t udf_id,
    int max_len,
    uint8 *data,
    uint8 *mask,
    int *actual_len)
{
    int dunit = 0;
    int map_ct = 0;
    bool found;
    int flags = 0;
    uint8_t cbmap_ct;
    uint64_t data64;
    uint64_t mask64;
    uint32_t c;
    uint32_t num_elem;
    uint8_t base = 0;
    int8_t shift_val = 0;
    bcm_field_qset_t qset;
    bcmi_field_qualify_t qual;
    bcm_udf_multi_chunk_info_t info;
    char *lt_field_name = NULL;
    char *qual_mask_field = NULL;
    bcm_field_qual_map_t *lt_map_ref = NULL;
    bcmlt_entry_handle_t rule_templ = BCMLT_INVALID_HDL;
    int len = 0;
    int rv = 0;
    bcm_field_hint_t hint;

    SHR_FUNC_ENTER(unit);

    if ((data == NULL) || (mask == NULL) || (actual_len == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    BCM_FIELD_QSET_INIT(qset);
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_field_udf_qset_set(unit, 1, &udf_id, &qset));

    sal_memset(&info, 0x0, sizeof(info));
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_udf_multi_chunk_info_get(unit, udf_id, &info, &flags));

    hint.hint_type = bcmFieldHintTypeExtraction;
    hint.qual = bcmFieldQualifyUdf;
    hint.udf_id = 0;
    rv = bcmint_field_hints_get(unit, group_oper->hintid, &hint);
    if ((rv == SHR_E_NONE) && (hint.value > 0) &&
         (hint.udf_id == udf_id)) {
        if (max_len < (hint.value)) {
            len = max_len;
        } else {
            len = (hint.value);
        }
    } else {
        len = info.width;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Entry handle allocate for rule Template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->rule_sid,
                              &rule_templ));

    /* Add rule_template_id field to Rule LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(rule_templ,
                               stage_info->tbls_info->rule_key_fid,
                               entry_oper->entry_id));

    /* Lookup rule template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(rule_templ, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    /* Iterate through supported 4/2/1 Byte chunk_bitmaps */
    for (cbmap_ct = 0; cbmap_ct < 3; cbmap_ct++) {
        for (c = 0; c < BCMINT_UDF_MAX_CHUNKS(unit); c++) {
            if (len == base) {
                continue;
            }
            data64 = mask64 = 0;
            if (cbmap_ct == 0) {
                if (BCM_UDF_CBMP_CHUNK_TEST(info.four_byte_chunk_bmap, c) == 0) {
                    continue;
                }
                qual = bcmiFieldQualifyB4Chunk0 + c;
                shift_val = 24;
            } else if (cbmap_ct == 1) {
                if (BCM_UDF_CBMP_CHUNK_TEST(info.two_byte_chunk_bmap, c) == 0) {
                    continue;
                }
                qual = bcmiFieldQualifyB2Chunk0 + c;
                shift_val = 8;
            } else {
                if (BCM_UDF_CBMP_CHUNK_TEST(info.one_byte_chunk_bmap, c) == 0) {
                    continue;
                }
                qual = bcmiFieldQualifyB1Chunk0 + c;
                shift_val = 0;
            }

            FP_HA_GROUP_OPER_QSET_TEST(group_oper, qual, found);
            if (found == FALSE) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            if (BCM_FIELD_QSET_TEST(qset, qual) == 0) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_qual_map_find(unit, qual,
                                            stage_info, 0, 0,
                                            &map_ct, &lt_map_ref));
            BCMINT_FIELD_MEM_ALLOC
                (lt_field_name,
                 strlen((lt_map_ref->map[0]).lt_field_name) + 7,
                 "UDF qualifier data field");
            sal_strcpy(lt_field_name, (lt_map_ref->map[0]).lt_field_name);

            if (flags & BCMINT_UDF_LT_ANCHOR_TYPE_INNER) {
                sal_strcat(lt_field_name, "_INNER");
            } else {
                sal_strcat(lt_field_name, "_OUTER");
            }


            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(rule_templ,
                                             lt_field_name,
                                             c, &data64, 1, &num_elem));
            if ((stage_info->stage_id != bcmiFieldStageExactMatch) &&
                (stage_info->stage_id != bcmiFieldStageFlowTracker)) {
                BCMINT_FIELD_MEM_ALLOC(qual_mask_field,
                     strlen(lt_field_name) + 6,
                     "qualifier mask field");
                sal_strcpy(qual_mask_field, lt_field_name);
                sal_strcat(qual_mask_field, "_MASK");
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_get(rule_templ,
                                                 qual_mask_field,
                                                 c, &mask64, 1, &num_elem));
                sal_free(qual_mask_field);
                qual_mask_field = NULL;
            }

            for (; shift_val >= 0; shift_val -= 8, base++) {
                if (len == base) {
                   break;
                }
                 data[base] = (uint8_t)(data64 >> shift_val);
                 mask[base] = (uint8_t)(mask64 >> shift_val);
            }

            sal_free(lt_field_name);
            lt_field_name = NULL;
        }
    }

    *actual_len = base;

exit:
    if (BCMLT_INVALID_HDL != rule_templ) {
        (void) bcmlt_entry_free(rule_templ);
    }
    if (lt_field_name) {
        sal_free(lt_field_name);
    }
    if (qual_mask_field) {
        sal_free(qual_mask_field);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_udf_qset_set(
    int unit,
    int num_objects,
    int objects_list[],
    bcm_field_qset_t *qset)
{
    int idx, c;
    int flags = 0;
    int valid_flag = (BCMINT_UDF_LT_ANCHOR_TYPE_OUTER |
                      BCMINT_UDF_LT_ANCHOR_TYPE_INNER);
    bcm_udf_multi_chunk_info_t info;
    bcmi_field_qualify_t qual;
    uint32_t max_chunks = BCMINT_UDF_MAX_CHUNKS(unit);
    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < num_objects; idx++) {
        sal_memset(&info, 0x0, sizeof(info));

        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_ltsw_udf_multi_chunk_info_get(unit, objects_list[idx],
                                               &info, &flags));
        valid_flag = flags & valid_flag;
        if (valid_flag == 0) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (valid_flag & BCMINT_UDF_LT_ANCHOR_TYPE_OUTER) {
            if (BCMI_FIELD_QSET_TEST(*qset, bcmiFieldQualifyUdfExtInner)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            qual = bcmiFieldQualifyUdfExtOuter;
        } else {
            if (BCMI_FIELD_QSET_TEST(*qset, bcmiFieldQualifyUdfExtOuter)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            qual = bcmiFieldQualifyUdfExtInner;
        }
        BCMI_FIELD_QSET_ADD(*qset, qual);

        for (c = 0; c < BCMINT_UDF_MAX_CHUNKS(unit); c++) {
            if (BCM_UDF_CBMP_CHUNK_TEST(info.four_byte_chunk_bmap, c)) {
                BCMI_FIELD_QSET_ADD(*qset,
                                    bcmiFieldQualifyB4Chunk0 + c);
                /* setting udf_map helps in gets/deletes */
                SHR_BITSET(qset->udf_map, c);
            }
            if (BCM_UDF_CBMP_CHUNK_TEST(info.two_byte_chunk_bmap, c)) {
                BCMI_FIELD_QSET_ADD(*qset,
                                    bcmiFieldQualifyB2Chunk0 + c);
                /* setting udf_map helps in gets/deletes */
                SHR_BITSET(qset->udf_map, max_chunks + c);
            }
            if (BCM_UDF_CBMP_CHUNK_TEST(info.one_byte_chunk_bmap, c)) {
                BCMI_FIELD_QSET_ADD(*qset,
                                    bcmiFieldQualifyB1Chunk0 + c);
                /* setting udf_map helps in gets/deletes */
                SHR_BITSET(qset->udf_map, (max_chunks * 2) + c);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_field_udf_obj_get(
    int unit,
    bcm_field_qset_t qset,
    int max,
    int *objects_list,
    int *actual)
{
    int c, cnt = 0;
    int idx, actual_cnt = 0;
    uint32_t chunk_1b_bmap = 0;
    uint32_t chunk_2b_bmap = 0;
    uint32_t chunk_4b_bmap = 0;
    uint32_t comp_chunk_1b_bmap = 0;
    uint32_t comp_chunk_2b_bmap = 0;
    uint32_t comp_chunk_4b_bmap = 0;
    bcm_udf_id_t udf_obj_arr[100] = {0};
    bcm_udf_multi_chunk_info_t info;
    uint32_t max_chunks = BCMINT_UDF_MAX_CHUNKS(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(actual, SHR_E_PARAM);

    for (c = 0; c < BCMINT_UDF_MAX_CHUNKS(unit); c++) {
         if (SHR_BITGET(qset.udf_map, c)) {
             chunk_4b_bmap |= (1 << c);
         }
         if (SHR_BITGET(qset.udf_map, c + max_chunks)) {
             chunk_2b_bmap |= (1 << c);
         }
         if (SHR_BITGET(qset.udf_map, c + (2 * max_chunks))) {
             chunk_1b_bmap |= (1 << c);
         }
    }

    /* UDF function to traverse and retrieve the UDF chunks info */
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_udf_object_list_get(unit, 100, udf_obj_arr, &actual_cnt));

    *actual = 0;
    for (idx = 0; idx < actual_cnt; idx++) {
        sal_memset(&info, 0x0, sizeof(info));
        comp_chunk_4b_bmap = 0;
        comp_chunk_2b_bmap = 0;
        comp_chunk_1b_bmap = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_ltsw_udf_multi_chunk_info_get(unit, udf_obj_arr[idx],
                                               &info, NULL));
        for (c = 0; c < BCMINT_UDF_MAX_CHUNKS(unit); c++) {
            if (BCM_UDF_CBMP_CHUNK_TEST(info.four_byte_chunk_bmap, c)) {
                comp_chunk_4b_bmap |= (1 << c);
            }
            if (BCM_UDF_CBMP_CHUNK_TEST(info.two_byte_chunk_bmap, c)) {
                comp_chunk_2b_bmap |= (1 << c);
            }
            if (BCM_UDF_CBMP_CHUNK_TEST(info.one_byte_chunk_bmap, c)) {
                comp_chunk_1b_bmap |= (1 << c);
            }
        }

        if (((chunk_4b_bmap & comp_chunk_4b_bmap) ==
                    comp_chunk_4b_bmap) &&
            ((chunk_2b_bmap & comp_chunk_2b_bmap) ==
                    comp_chunk_2b_bmap) &&
            ((chunk_1b_bmap & comp_chunk_1b_bmap) ==
                    comp_chunk_1b_bmap)) {
            cnt++;
            if (max < cnt) {
                break;
            }
            objects_list[*actual] = udf_obj_arr[idx];
            *actual += 1;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_udf_qset_del(
    int unit,
    int num_objects,
    int objects_list[],
    bcm_field_qset_t *qset)
{
    int idx, c;
    bcm_udf_multi_chunk_info_t info;
    uint32_t max_chunks = BCMINT_UDF_MAX_CHUNKS(unit);
    SHR_FUNC_ENTER(unit);

    BCM_FIELD_QSET_INIT(*qset);
    for (idx = 0; idx < num_objects; idx++) {
        sal_memset(&info, 0x0, sizeof(info));

        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_ltsw_udf_multi_chunk_info_get(unit, objects_list[idx],
                                               &info, NULL));

        for (c = 0; c < BCMINT_UDF_MAX_CHUNKS(unit); c++) {
            if (BCM_UDF_CBMP_CHUNK_TEST(info.four_byte_chunk_bmap, c)) {
                BCMI_FIELD_QSET_REMOVE(*qset,
                                       bcmiFieldQualifyB4Chunk0 + c);
                SHR_BITCLR(qset->udf_map, c);
            }
            if (BCM_UDF_CBMP_CHUNK_TEST(info.two_byte_chunk_bmap, c)) {
                BCMI_FIELD_QSET_REMOVE(*qset,
                                       bcmiFieldQualifyB2Chunk0 + c);
                SHR_BITSET(qset->udf_map, max_chunks + c);
            }
            if (BCM_UDF_CBMP_CHUNK_TEST(info.one_byte_chunk_bmap, c)) {
                BCMI_FIELD_QSET_REMOVE(*qset,
                                       bcmiFieldQualifyB1Chunk0 + c);
                SHR_BITCLR(qset->udf_map, (max_chunks * 2) + c);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_group_mode_set(
        int unit,
        bcm_field_group_config_t *group_config,
        bcmint_field_stage_info_t *stage_info,
        bcmlt_entry_handle_t grp_template,
        bool *mode_auto,
        char **group_mode)
{
    SHR_FUNC_ENTER(unit);
    /* Check group mode flag and update mode LT field accordingly. */
    if (group_config->flags & BCM_FIELD_GROUP_CREATE_WITH_MODE) {
        if (group_config->mode == bcmFieldGroupModeAuto) {
            *mode_auto = 1;
            *group_mode = "MODE_AUTO";
        } else {
            if ((stage_info->stage_id == bcmiFieldStageExactMatch) &&
                (BCM_FIELD_ASET_TEST
                 (group_config->aset,
                  bcmFieldActionStatGroupWithoutCounterIndex)) &&
                (group_config->mode == bcmFieldGroupModeSingle)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
            }
            /* Update group mode if not auto */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_mode_validate(unit, grp_template,
                                                  group_config->mode,
                                                  group_mode));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(grp_template,
                                              "MODE",
                                              *group_mode));
        }
    } else {
        *mode_auto = 1;
        *group_mode = "MODE_AUTO";
    }

    /*
     * Allow Auto mode for EM only if flex counter is not present.
     */
    if ((stage_info->stage_id == bcmiFieldStageExactMatch) &&
        (BCM_FIELD_ASET_TEST
         (group_config->aset,
          bcmFieldActionStatGroupWithoutCounterIndex)) &&
        (*mode_auto == 1)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
    }

    /* Update Group Auto mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(grp_template, "MODE_AUTO", *mode_auto));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get egress mask profile entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 * \param [out] profile Profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_field_egr_mask_lt_get(int unit,
                          bcmi_ltsw_field_stage_t stage_id,
                          int index, void *profile)
{
    uint32_t cnt;
    uint64_t v64;
    int dunit, i, max_port_num;
    xfs_field_egr_mask_profile_t *p;
    int blk_size = 0;
    bcmlt_field_def_t fld_defs_ref;
    const char *mask_target, *mask_action;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit,
             stage_info->tbls_info->egress_mask_profile_sid,
             stage_info->tbls_info->egress_mask_profile_key_fid, &fld_defs_ref));

    blk_size = 1 << fld_defs_ref.width;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->egress_mask_profile_sid, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh,
                               stage_info->tbls_info->egress_mask_profile_section_fid,
                               index / blk_size));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh,
                               stage_info->tbls_info->egress_mask_profile_key_fid,
                               index % blk_size));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    p = (xfs_field_egr_mask_profile_t *)profile;

    /* Get egress mask */
    BCM_PBMP_CLEAR(p->pbmp);
    max_port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    for (i = 0; i < max_port_num; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(eh,
                                         stage_info->tbls_info->egress_mask_profile_ports_fid,
                                         i, &v64, 1, &cnt));
        if (v64) {
            BCM_PBMP_PORT_ADD(p->pbmp, i);
        }
    }

    /* Get target for egress mask */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh,
            stage_info->tbls_info->egress_mask_profile_target_fid, &mask_target));
    if (sal_strcmp(mask_target, L2_MEMBERs) == 0) {
        p->mask_target = BCMINT_FIELD_EGRESS_MASK_L2;
    } else if (sal_strcmp(mask_target, L3_MEMBERs) == 0) {
        p->mask_target = BCMINT_FIELD_EGRESS_MASK_L3;
    } else if (sal_strcmp(mask_target, L2_L3_MEMBERs) == 0) {
        p->mask_target = BCMINT_FIELD_EGRESS_MASK_L2_L3;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get operation for egress mask */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh,
            stage_info->tbls_info->egress_mask_profile_action_fid, &mask_action));
    if (sal_strcmp(mask_action, ORs) == 0) {
        p->mask_action = BCMINT_FIELD_EGRESS_MASK_OP_OR;
    } else if (sal_strcmp(mask_action, ANDs) == 0) {
        p->mask_action = BCMINT_FIELD_EGRESS_MASK_OP_AND;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert egress mask profile entry to LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 * \param [in] profile Profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_field_egr_mask_lt_insert(int unit,
                             bcmi_ltsw_field_stage_t stage_id,
                             int index, void *profile)
{
    uint64_t v64;
    int dunit, i, max_port_num;
    xfs_field_egr_mask_profile_t *p;
    int blk_size = 0;
    bcmlt_field_def_t fld_defs_ref;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    if (bcmi_warmboot_get(unit)) {
        SHR_EXIT();
    }
    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, stage_info->tbls_info->egress_mask_profile_sid,
             stage_info->tbls_info->egress_mask_profile_key_fid, &fld_defs_ref));
    blk_size = 1 << fld_defs_ref.width;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->egress_mask_profile_sid,
                              &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh,
                               stage_info->tbls_info->egress_mask_profile_section_fid,
                               index / blk_size));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh,
                               stage_info->tbls_info->egress_mask_profile_key_fid,
                               index % blk_size));

    p = (xfs_field_egr_mask_profile_t *)profile;

    /* Fill egress mask */
    max_port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    for (i = 0; i < max_port_num; i ++) {
        v64 = (BCM_PBMP_MEMBER(p->pbmp, i) ? 1 : 0);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add(eh,
                                         stage_info->tbls_info->egress_mask_profile_ports_fid,
                                         i, &v64, 1));
    }

    /* Fill target for egress mask */
    if (p->mask_target == BCMINT_FIELD_EGRESS_MASK_L2) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh,
                stage_info->tbls_info->egress_mask_profile_target_fid, L2_MEMBERs));
    } else if (p->mask_target == BCMINT_FIELD_EGRESS_MASK_L3) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh,
                stage_info->tbls_info->egress_mask_profile_target_fid, L3_MEMBERs));
    } else if (p->mask_target == BCMINT_FIELD_EGRESS_MASK_L2_L3) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh,
                stage_info->tbls_info->egress_mask_profile_target_fid, L2_L3_MEMBERs));
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Fill operation for egress mask */
    if (p->mask_action == BCMINT_FIELD_EGRESS_MASK_OP_OR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh,
                stage_info->tbls_info->egress_mask_profile_action_fid, ORs));
    } else if (p->mask_action == BCMINT_FIELD_EGRESS_MASK_OP_AND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh,
               stage_info->tbls_info->egress_mask_profile_action_fid, ANDs));
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit
         (unit, eh, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete egress mask profile entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_field_egr_mask_lt_delete(int unit,
                             bcmi_ltsw_field_stage_t stage_id,
                             int index)
{
    int dunit = 0;
    int blk_size = 0;
    bcmlt_field_def_t fld_defs_ref;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, stage_info->tbls_info->egress_mask_profile_sid,
             stage_info->tbls_info->egress_mask_profile_key_fid, &fld_defs_ref));
    blk_size = 1 << fld_defs_ref.width;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->egress_mask_profile_sid, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh,
                               stage_info->tbls_info->egress_mask_profile_section_fid,
                               index / blk_size));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh,
                               stage_info->tbls_info->egress_mask_profile_key_fid,
                               index % blk_size));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit
         (unit, eh, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

static int
xfs_field_egr_mask_lt_section_blk_get(int unit,
                                      bcmi_ltsw_field_stage_t stage_id,
                                      int index,
                                      int *prof_section,
                                      int *egr_blk_id)
{
    int blk_size = 0;
    bcmlt_field_def_t fld_defs_ref;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, stage_info->tbls_info->egress_mask_profile_sid,
             stage_info->tbls_info->egress_mask_profile_key_fid, &fld_defs_ref));
    blk_size = 1 << fld_defs_ref.width;

    *prof_section = index / blk_size;
    *egr_blk_id = index % blk_size;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate hash of egress mask profile set.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] entries_per_set Number of profile entries in the set.
 * \param [out] hash Hash value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_field_egr_mask_profile_hash(int unit, void *entries,
                    int entries_per_set, uint32_t *hash)
{
    int size = entries_per_set * sizeof(xfs_field_egr_mask_profile_t);
    *hash = shr_crc32(0, entries, size);
    return SHR_E_NONE;
}

/*!
 * \brief Compare profile set.
 *
 * Compare if given egress mask profile entries equals to
 * the entries in LT.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] entries_per_set Number of profile entries in the set.
 * \param [in] index Profile table index to be compared.
 * \param [out] cmp 0 if equal, otherwise not equal.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_field_egr_mask_profile_cmp(int unit,
                    void *entries,
                    int entries_per_set, int index, int *cmp)
{
    return bcmint_ltsw_field_ingress_profile_cmp
        (unit, entries, index, &xfs_field_egr_mask_profile_info[unit], cmp);
}

/*!
 * \brief Recover egress mask profile from after wb.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_field_egr_mask_profile_recover(int unit, ltsw_field_profile_info_t *pinfo)
{
    int p = 0;
    int dunit = 0;
    int blk_size = 0;
    uint32_t cnt = 0;
    uint64_t section = 0;
    uint64_t blk_id = 0;
    uint64_t data64 = 0;
    bcmlt_field_def_t fld_defs_ref;
    int index, prof_idx, max_port_num;
    xfs_field_egr_mask_profile_t prof;
    const char *mask_target, *mask_action;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const char *tbl_name = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, bcmiFieldStageIngress, &stage_info));

    tbl_name = stage_info->tbls_info->egress_mask_profile_sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, tbl_name, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, tbl_name,
             stage_info->tbls_info->egress_mask_profile_key_fid, &fld_defs_ref));
    blk_size = 1 << fld_defs_ref.width;
    max_port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;

    while ((bcmi_lt_entry_commit(unit, ent_hdl,
                                 BCMLT_OPCODE_TRAVERSE,
                                 BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
       sal_memset(&prof, 0x0, sizeof(prof));

       SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_get(ent_hdl,
               stage_info->tbls_info->egress_mask_profile_section_fid, &section));
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_get(ent_hdl,
                stage_info->tbls_info->egress_mask_profile_key_fid, &blk_id));
       prof_idx = (section * blk_size) + blk_id;

       BCM_PBMP_CLEAR(prof.pbmp);
       /* update egress mask pbmp */
       for (p = 0; p < max_port_num; p++) {
           SHR_IF_ERR_VERBOSE_EXIT
               (bcmlt_entry_field_array_get(ent_hdl,
                                            stage_info->tbls_info->egress_mask_profile_ports_fid,
                                            p, &data64, 1, &cnt));
           if (data64) {
              BCM_PBMP_PORT_ADD(prof.pbmp, p);
           }
       }

       /* update mask action */
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_symbol_get(ent_hdl,
               stage_info->tbls_info->egress_mask_profile_action_fid, &mask_action));
       if (sal_strcmp(mask_action, ORs) == 0) {
           prof.mask_action = BCMINT_FIELD_EGRESS_MASK_OP_OR;
       } else {
           prof.mask_action = BCMINT_FIELD_EGRESS_MASK_OP_AND;
       }

       /* update mask target */
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_symbol_get(ent_hdl,
                stage_info->tbls_info->egress_mask_profile_target_fid, &mask_target));
       if (sal_strcmp(mask_target, L2_L3_MEMBERs) == 0) {
           prof.mask_target = BCMINT_FIELD_EGRESS_MASK_L2_L3;
       } else if (sal_strcmp(mask_target, L3_MEMBERs) == 0) {
           prof.mask_target = BCMINT_FIELD_EGRESS_MASK_L3;
       } else {
           prof.mask_target = BCMINT_FIELD_EGRESS_MASK_L2;
       }

       /* allocate default profile entry, accessed by default fp rules. */
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmint_ltsw_field_profile_add(unit, stage_info->stage_id,
                                &prof, 1, pinfo, &index));
       if (index != prof_idx) {
           SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
       }
    }

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize egress mask profile information.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_field_egr_mask_profile_init(int unit)
{
    int entry_idx;
    uint32_t entry_num = 0;
    xfs_field_egr_mask_profile_t profile;
    ltsw_field_profile_info_t *pinfo = &xfs_field_egr_mask_profile_info[unit];
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, bcmiFieldStageIngress, &stage_info));

    pinfo->phd = BCMI_LTSW_PROFILE_FIELD_EGR_MASK;
    pinfo->entry_idx_min = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit,
                              stage_info->tbls_info->egress_mask_profile_sid,
                              &entry_num));
    pinfo->entry_idx_max = entry_num - 1;
    pinfo->entries_per_set = 1;
    pinfo->entry_size = sizeof(xfs_field_egr_mask_profile_t);
    pinfo->lt_get = xfs_field_egr_mask_lt_get;
    pinfo->lt_ins = xfs_field_egr_mask_lt_insert;
    pinfo->lt_del = xfs_field_egr_mask_lt_delete;

    /* unregister in case the profile already exists */
    if (bcmi_ltsw_profile_register_check(unit, pinfo->phd)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_unregister(unit, pinfo->phd));
    }

    /* register profile for IFP port flood block */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit,
            &pinfo->phd, &pinfo->entry_idx_min,
            &pinfo->entry_idx_max, 1,
            xfs_field_egr_mask_profile_hash,
            xfs_field_egr_mask_profile_cmp));

    if (bcmi_warmboot_get(unit)) {
        SHR_ERR_EXIT
            (xfs_field_egr_mask_profile_recover(unit, pinfo));
        SHR_EXIT();
    }

    /* clear profile table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, stage_info->tbls_info->egress_mask_profile_sid));

    /* default field egr mask flood profile entry */
    _SHR_PBMP_PORTS_RANGE_ADD(profile.pbmp, 0, bcmi_ltsw_dev_logic_port_max(unit) + 1);
    profile.mask_target = BCMINT_FIELD_EGRESS_MASK_L2_L3;
    profile.mask_action = BCMINT_FIELD_EGRESS_MASK_OP_AND;

    /* allocate default profile entry, accessed by default fp rules. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_profile_add(unit, stage_info->stage_id,
                                &profile, 1, pinfo, &entry_idx));

    /* first allocation so index must be zero */
    if (entry_idx != 0) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize egress mask profile information.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_field_egr_mask_profile_deinit(int unit)
{
    int rv = SHR_E_NONE;
    int warm = bcmi_warmboot_get(unit);
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit,
                                     bcmiFieldStageIngress,
                                     &stage_info));

    /* unregister in case the profile already exists */
    rv = bcmi_ltsw_profile_unregister(unit,
                        xfs_field_egr_mask_profile_info[unit].phd);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (warm) {
        SHR_EXIT();
    }

    /* clear profile table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, stage_info->tbls_info->egress_mask_profile_sid));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_ingress_profile_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_field_egr_mask_profile_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_ingress_profile_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_field_egr_mask_profile_deinit(unit));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_action_profile_index_get(int unit,
                                        bcm_field_entry_t entry,
                                        bcm_field_action_t action,
                                        uint16_t *act_prof_idx)
{
    uint64_t action_lt_val = 0x0;
    bcm_field_action_params_t params;
    bcm_field_action_match_config_t match_config;

    SHR_FUNC_ENTER(unit);

    /* Find prof index associated for valid actions. */
    if ((action == bcmFieldActionEgressMask) ||
        (action == bcmFieldActionEgressPortsAdd)) {

        sal_memset(&params, 0x0, sizeof(params));
        sal_memset(&match_config, 0, sizeof(match_config));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_config_info_get(unit, entry,
                                                 action, &action_lt_val,
                                                 &params, &match_config));
        *act_prof_idx = (uint16_t)action_lt_val;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_action_profile_add(int unit,
                                  bcmi_ltsw_field_stage_t stage_id,
                                  bcm_field_action_t action,
                                  bcm_field_action_params_t *params,
                                  bcm_field_action_match_config_t *match_config,
                                  uint64_t *act_prof_idx_val)
{
    bcm_pbmp_t port_pbmp;
    bcm_port_config_t port_config;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(params, SHR_E_INTERNAL);
    SHR_NULL_CHECK(act_prof_idx_val, SHR_E_INTERNAL);

    /* Populate egress mask profile params. */
    if ((action == bcmFieldActionEgressMask) ||
        (action == bcmFieldActionEgressPortsAdd)) {
        int egr_blk_id = 0;
        int egr_mask_idx = 0;
        int profile_section = 0;
        xfs_field_egr_mask_profile_t egr_mask_prof;

        /* Validate pbmp passed by user. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_config_get(unit, &port_config));
        BCM_PBMP_ASSIGN(port_pbmp, params->pbmp);
        BCM_PBMP_REMOVE(port_pbmp, port_config.all);
        if (BCM_PBMP_NOT_NULL(port_pbmp)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (action == bcmFieldActionEgressMask) {
            BCM_PBMP_REMOVE(port_config.all, params->pbmp);
            BCM_PBMP_ASSIGN(egr_mask_prof.pbmp, port_config.all);
        } else {
            BCM_PBMP_ASSIGN(egr_mask_prof.pbmp, params->pbmp);
        }
        egr_mask_prof.mask_target = BCMINT_FIELD_EGRESS_MASK_L2_L3;

        if (action == bcmFieldActionEgressPortsAdd) {
            egr_mask_prof.mask_action = BCMINT_FIELD_EGRESS_MASK_OP_OR;
        } else {
            egr_mask_prof.mask_action = BCMINT_FIELD_EGRESS_MASK_OP_AND;
        }

        /* Add egress mask profile entry and retreive index. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_field_profile_add(unit, stage_id,
                &egr_mask_prof, 1,
                &xfs_field_egr_mask_profile_info[unit], &egr_mask_idx));

        /* Retrieve the section and blk from the profile index */
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_field_egr_mask_lt_section_blk_get(unit, stage_id,
                                                   egr_mask_idx,
                                                   &profile_section,
                                                   &egr_blk_id));

        /* Send action profile index value. */
        *act_prof_idx_val = ((egr_blk_id << 2) | profile_section);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_action_profile_get(int unit,
                                  bcmi_ltsw_field_stage_t stage_id,
                                  bcm_field_action_t action,
                                  uint64_t *act_prof_idx_val,
                                  bcm_field_action_params_t *params,
                                  bcm_field_action_match_config_t *match_config)
{
    int profile_idx;
    int egr_mask_idx;
    int profile_section;
    bcm_pbmp_t port_pbmp;
    bcm_port_config_t port_config;
    xfs_field_egr_mask_profile_t egr_mask_prof;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(act_prof_idx_val, SHR_E_INTERNAL);
    SHR_NULL_CHECK(params, SHR_E_INTERNAL);

    /* Retreive egress profile entry. */
    if ((action == bcmFieldActionEgressMask) ||
        (action == bcmFieldActionEgressPortsAdd)) {
         /* Retreive egress mask profile index. */
         egr_mask_idx = (int)(COMPILER_64_LO(*act_prof_idx_val));

         /* Calculate profile index */
         profile_section = egr_mask_idx & 0x3;
         profile_idx = (profile_section * 256) + (egr_mask_idx >> 2);

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmint_ltsw_field_profile_get(unit, stage_id, profile_idx,
                 &xfs_field_egr_mask_profile_info[unit], &egr_mask_prof));

         SHR_IF_ERR_VERBOSE_EXIT
             (bcm_ltsw_port_config_get(unit, &port_config));

         /* Populate pbmp to user. */
         if (action == bcmFieldActionEgressMask) {
             BCM_PBMP_NEGATE(port_pbmp, egr_mask_prof.pbmp);
             BCM_PBMP_AND(port_pbmp, port_config.all);
             BCM_PBMP_ASSIGN(params->pbmp, port_pbmp);
         } else {
             BCM_PBMP_ASSIGN(params->pbmp, egr_mask_prof.pbmp);
         }
     }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_action_profile_del(int unit,
                                  bcmi_ltsw_field_stage_t stage_id,
                                  bcm_field_action_t action,
                                  uint64_t *act_prof_idx_val)
{
    int profile_idx;
    int egr_mask_idx;
    int profile_section;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(act_prof_idx_val, SHR_E_INTERNAL);

    /* Delete profile entry created. */
    if ((action == bcmFieldActionEgressMask) ||
        (action == bcmFieldActionEgressPortsAdd)) {
         /* Retreive egress mask profile index. */
         egr_mask_idx = (int)(COMPILER_64_LO(*act_prof_idx_val));

         /* Calculate profile index */
         profile_section = egr_mask_idx & 0x3;
         profile_idx = (profile_section * 256) + (egr_mask_idx >> 2);

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmint_ltsw_field_profile_delete(unit, stage_id,
                profile_idx, &xfs_field_egr_mask_profile_info[unit]));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_qual_info_lt_name_get(
    int unit,
    const char *lt_field_name,
    char *num_offset_field_name,
    char *offset_field_name,
    char *width_field_name) {

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_field_name, SHR_E_INTERNAL);
    SHR_NULL_CHECK(num_offset_field_name, SHR_E_INTERNAL);
    SHR_NULL_CHECK(offset_field_name, SHR_E_INTERNAL);
    SHR_NULL_CHECK(width_field_name, SHR_E_INTERNAL);

    sal_strcpy(num_offset_field_name, "NUM_QUAL_");
    sal_strcat(num_offset_field_name, lt_field_name);

    sal_strcpy(offset_field_name, "QUAL_");
    sal_strcat(offset_field_name, lt_field_name);
    sal_strcat(offset_field_name, "_OFFSET");

    sal_strcpy(width_field_name, "QUAL_");
    sal_strcat(width_field_name, lt_field_name);
    sal_strcat(width_field_name, "_WIDTH");

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the filter enable state in the Port Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xfs_ltsw_field_port_filter_enable_set(int unit, uint32 state,
                                      bcm_pbmp_t port_pbmp)
{
    SHR_FUNC_ENTER(unit);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*
 * Driver to handle different Qualification parameters
 * for all qualifiers and across all XFS devices.
 */
int
xfs_ltsw_field_qual_value_set(int unit,
                    bcm_field_entry_t entry_id,
                    bcm_qual_field_t *qual_info)
{
    uint8_t data_val = 0;
    uint8_t mask_val = -1;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(qual_info, SHR_E_INTERNAL);

    switch (qual_info->sdk6_qual_enum) {
        case bcmFieldQualifyIngressStpState:
            switch(*((uint8_t *)(qual_info->qual_data))) {
                case BCM_STG_STP_DISABLE:
                    data_val = 0;
                    break;
                case BCM_STG_STP_BLOCK:
                    data_val = 1;
                    break;
                case BCM_STG_STP_LEARN:
                    data_val = 2;
                    break;
                case BCM_STG_STP_FORWARD:
                    data_val = 3;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            qual_info->depth = 1;
            qual_info->data_type = BCM_QUAL_DATA_TYPE_UINT8;

            *((uint8_t *)qual_info->qual_data) =  data_val;
            *((uint8_t *)qual_info->qual_mask) =  mask_val;
            break;
        case bcmFieldQualifyOuterTpid:
        case bcmFieldQualifyInnerTpid:
            qual_info->depth = 1;
            qual_info->data_type = BCM_QUAL_DATA_TYPE_UINT16;
            break;
        case bcmFieldQualifyColor:
        case bcmFieldQualifyLoopBackColor:
            switch(*((uint8_t *)(qual_info->qual_data))) {
                case BCM_FIELD_COLOR_GREEN:
                    data_val = 0;
                    break;
                case BCM_FIELD_COLOR_YELLOW:
                    data_val = 3;
                    break;
                case BCM_FIELD_COLOR_RED:
                    data_val = 1;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            qual_info->depth = 1;
            qual_info->data_type = BCM_QUAL_DATA_TYPE_UINT8;

            *((uint8_t *)qual_info->qual_data) =  data_val;
            *((uint8_t *)qual_info->qual_mask) =  mask_val;
            break;
        case bcmFieldQualifyPktMiscCtrl:
            switch (*((bcm_field_pkt_misc_ctrl_t *)(qual_info->qual_data))) {
                case bcmFieldPktMiscCtrlNone:
                    qual_info->enum_value = 0;
                    qual_info->enum_mask = 0;
                    break;
                case bcmFieldPktMiscCtrlGoodIpPkt:
                    qual_info->enum_value = 1;
                    qual_info->enum_mask = 1;
                    break;
                case bcmFieldPktMiscCtrlNotGoodIpPkt:
                    qual_info->enum_value = 0;
                    qual_info->enum_mask = 1;
                    break;
                case bcmFieldPktMiscCtrlBpduPkt:
                    qual_info->enum_value = 2;
                    qual_info->enum_mask = 2;
                    break;
                case bcmFieldPktMiscCtrlNotBpduPkt:
                    qual_info->enum_value = 0;
                    qual_info->enum_mask = 2;
                    break;
                case bcmFieldPktMiscCtrlPimBidirPkt:
                    qual_info->enum_value = 4;
                    qual_info->enum_mask = 4;
                    break;
                case bcmFieldPktMiscCtrlNotPimBidirPkt:
                   qual_info->enum_value = 0;
                   qual_info->enum_mask = 4;
                   break;
                case bcmFieldPktMiscCtrlHiGig3MirrorPkt:
                   qual_info->enum_value = 8;
                   qual_info->enum_mask = 8;
                   break;
               case bcmFieldPktMiscCtrlNotHiGig3MirrorPkt:
                   qual_info->enum_value = 0;
                   qual_info->enum_mask = 8;
                   break;
               default:
                   SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;
        case bcmFieldQualifyLoopbackType:
            switch(*((bcm_field_LoopbackType_t *)(qual_info->qual_data))) {
                case bcmFieldLoopbackTypeL2Tunnel2ndPassFlow:
                    data_val = 0;
                    mask_val = 15;
                    break;
                case bcmFieldLoopbackTypeAny:
                    data_val = 15;
                    mask_val = 15;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            qual_info->depth = 1;
            qual_info->data_type = BCM_QUAL_DATA_TYPE_UINT8;

            *((uint8_t *)qual_info->qual_data) =  data_val;
            *((uint8_t *)qual_info->qual_mask) =  mask_val;
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_qual_value_get(int unit,
        bcm_field_entry_t entry,
        bcm_qual_field_t *qual_info)
{
    bcm_field_pkt_misc_ctrl_t pkt_misc_ctrl = bcmFieldPktMiscCtrlNone;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(qual_info, SHR_E_INTERNAL);

    switch (qual_info->sdk6_qual_enum) {
        case bcmFieldQualifyIngressStpState:
            switch (**((uint8_t **)qual_info->qual_data)) {
                case 0:
                    **((uint8_t **)qual_info->qual_data) = BCM_STG_STP_DISABLE;
                    break;
                case 1:
                    **((uint8_t **)qual_info->qual_data) = BCM_STG_STP_BLOCK;
                    break;
                case 2:
                    **((uint8_t **)qual_info->qual_data) = BCM_STG_STP_LEARN;
                    break;
                case 3:
                    **((uint8_t **)qual_info->qual_data) = BCM_STG_STP_FORWARD;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            break;
        case bcmFieldQualifyColor:
        case bcmFieldQualifyLoopBackColor:
            switch (**((uint8_t **)qual_info->qual_data)) {
                case 0:
                    **((uint8_t **)qual_info->qual_data) = BCM_FIELD_COLOR_GREEN;
                    break;
                case 1:
                    **((uint8_t **)qual_info->qual_data) = BCM_FIELD_COLOR_RED;
                    break;
                case 3:
                    **((uint8_t **)qual_info->qual_data) = BCM_FIELD_COLOR_YELLOW;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            break;
        case bcmFieldQualifyPktMiscCtrl:
            switch (qual_info->enum_mask) {
                case 0:
                   if (qual_info->enum_value == 0) {
                        pkt_misc_ctrl = bcmFieldPktMiscCtrlNone;
                    } else {
                        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
                    }
                    break;
                case 1:
                    if (qual_info->enum_value == 1) {
                        pkt_misc_ctrl = bcmFieldPktMiscCtrlGoodIpPkt;
                    } else {
                        pkt_misc_ctrl = bcmFieldPktMiscCtrlNotGoodIpPkt;
                    }
                    break;
                case 2:
                    if (qual_info->enum_value == 2) {
                        pkt_misc_ctrl = bcmFieldPktMiscCtrlBpduPkt;
                    } else {
                        pkt_misc_ctrl = bcmFieldPktMiscCtrlNotBpduPkt;
                   }
                   break;
               case 4:
                    if (qual_info->enum_value == 4) {
                       pkt_misc_ctrl = bcmFieldPktMiscCtrlPimBidirPkt;
                    } else {
                       pkt_misc_ctrl = bcmFieldPktMiscCtrlNotPimBidirPkt;
                    }
                    break;
               case 8:
                   if (qual_info->enum_value == 8) {
                       pkt_misc_ctrl = bcmFieldPktMiscCtrlHiGig3MirrorPkt;
                   } else {
                       pkt_misc_ctrl = bcmFieldPktMiscCtrlNotHiGig3MirrorPkt;
                   }
                   break;
               default:
                   SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            **((bcm_field_pkt_misc_ctrl_t **)qual_info->qual_data) = pkt_misc_ctrl;
            break;
        case bcmFieldQualifyLoopbackType:
            switch (**((uint8_t **)qual_info->qual_data)) {
                case 0:
                    **((uint8_t **)qual_info->qual_data) = bcmFieldLoopbackTypeL2Tunnel2ndPassFlow;
                    break;
                case 0xf:
                    **((uint8_t **)qual_info->qual_data) = bcmFieldLoopbackTypeAny;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_qual_info_set(int unit,
        bcm_field_entry_t entry,
        bcm_qual_field_t *qual_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(qual_info, SHR_E_INTERNAL);

    switch (qual_info->sdk6_qual_enum) {
        case bcmFieldQualifyIngressStpState:
        case bcmFieldQualifyColor:
        case bcmFieldQualifyLoopBackColor:
        case bcmFieldQualifyLoopbackType:
            qual_info->depth = 1;
            qual_info->data_type = BCM_QUAL_DATA_TYPE_UINT8;
            break;
        case bcmFieldQualifyOuterTpid:
        case bcmFieldQualifyInnerTpid:
            qual_info->depth = 1;
            qual_info->data_type = BCM_QUAL_DATA_TYPE_UINT16;
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_stage_group_sbr_prof_init(
    int unit,
    bcmint_field_stage_info_t *stage_info)
{
    int oper_mode;
    uint8_t num_pipe;
    bcmint_field_action_strength_prof_base_t *sbr_base_ptr = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_oper_mode_status_get(unit, stage_info, &oper_mode));
    if (oper_mode == bcmiFieldOperModePipeUnique) {
        num_pipe = bcmi_ltsw_dev_max_pp_pipe_num(unit);
    } else {
        num_pipe = 1;
    }

    BCMINT_FIELD_MEM_ALLOC(sbr_base_ptr,
         sizeof(bcmint_field_action_strength_prof_base_t) * num_pipe,
         "Field Group Action sbr profile");

    stage_info->strength_base_prof = sbr_base_ptr;
exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_fp_get_first_bit_set_pos(int bmap)
{
   int pos = 0;

   while (bmap) {
      if (bmap & 1) {
         break;
      }
      bmap = bmap >> 1;
      pos++;
   }
   return pos;
}

static int
xfs_ltsw_field_action_mirror_cont_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry_id,
    bcm_field_action_t action,
    int mirror_cont,
    char **cont_name)
{
    SHR_FUNC_ENTER(unit);

    if (stage_info->stage_id != bcmiFieldStageIngress) {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    switch (mirror_cont) {
        case bcmiMirrorCon2:
             *cont_name = "ING_CMD_BUS_L2_IIF_SVP_FP_MIRROR_INDEX_2";
             break;
        case bcmiMirrorCon3:
             *cont_name = "ING_CMD_BUS_L2_IIF_SVP_FP_MIRROR_INDEX_3";
             break;
        case bcmiMirrorCon4:
             *cont_name = "ING_CMD_BUS_IFP_MIRROR_INDEX_4";
             break;
        case bcmiMirrorCon5:
             *cont_name = "ING_CMD_BUS_IFP_MIRROR_INDEX_5";
             break;
        default:
             *cont_name = NULL;
             SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_action_meter_cont_get(
    int unit,
    bcmi_ltsw_field_stage_t stage,
    int slice_idx,
    char **cont_name)
{
    SHR_FUNC_ENTER(unit);

    if ((stage == bcmiFieldStageEgress) && (slice_idx != 0)) {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    switch (slice_idx) {
    case 0:
        if (stage == bcmiFieldStageIngress) {
            *cont_name = "ING_OBJ_BUS_METER_TABLE_0_ACTION_SET";
        } else {
            *cont_name = "EGR_METER_TRIGGERS_METER_ACTION_SET";
        }
        break;
    case 1:
        *cont_name = "ING_OBJ_BUS_METER_TABLE_1_ACTION_SET";
        break;
    case 2:
        *cont_name = "ING_OBJ_BUS_METER_TABLE_2_ACTION_SET";
        break;
    case 3:
        *cont_name = "ING_OBJ_BUS_METER_TABLE_3_ACTION_SET";
        break;
    default:
        *cont_name = NULL;
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_qual_gport_validate(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_qualify_t qual,
        bcm_gport_t gport,
        bcm_port_t *port)
{
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    if (BCM_GPORT_IS_MPLS_PORT(gport) ||
        BCM_GPORT_IS_WLAN_PORT(gport) ||
        BCM_GPORT_IS_VLAN_PORT(gport) ||
        BCM_GPORT_IS_VXLAN_PORT(gport) ||
        BCM_GPORT_IS_NIV_PORT(gport) ||
        BCM_GPORT_IS_MIM_PORT(gport) ||
        BCM_GPORT_IS_FLOW_PORT(gport) ||
        BCM_GPORT_IS_MODPORT(gport)) {
        if (BCM_GPORT_IS_MODPORT(gport)) {
            if (!((stage_info->stage_id == bcmiFieldStageIngress) &&
                ((qual == bcmFieldQualifyDstGport) ||
                 (qual == bcmFieldQualifyDstGports)))) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (BCM_GPORT_IS_MODPORT(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_to_l2_if(unit, gport, port));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, gport, port));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_action_value_get(
        int unit,
        bcm_field_entry_t entry_id,
        bcmi_ltsw_field_stage_t stage,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config,
        uint64_t *action_value)
{
    int iter = 0;
    uint8_t *pool_cancel = NULL;
    uint32_t alloc_sz = 0, offset = 0;
    bcmlt_field_def_t fld_def;
    bcm_if_t if_id = 0;
    bcm_gport_t dest_gport = 0;
    bcmi_ltsw_gport_info_t gport_info;
    const bcm_field_action_map_t *lt_map = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage, &stage_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info, &entry_oper));

   /* convert the sdk6 action values to sdklt action values */
   /* Do switch case based on
    *   Misc actions which needs convertions or chip specific.
    */
    switch (action) {
        /* SDK6
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_PORT  0
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_SVP   1
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_L3_IIF 2
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_FIELD_SRC 3
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_FIELD_DST 4
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_L2_SRC 5
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_L2_DST 6
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_L3_SRC 7
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_L3_DST 8
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_VLAN  9
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_VRF   10
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_NEW   11
         * SDKLT
         * I2E class_id assignment
         * interface_class_id ifp_class_id_sel[0:0] == 1
         * l3_dst_class_id ifp_class_id_sel[1:1] == 1
         * l3_src_class_id ifp_class_id_sel[2:2] == 1
         * ifp_class_id  ifp_class_id_sel[3:3] == 1
         */
        case bcmFieldActionEgressClassSelect:
            if (*action_value == 0x1) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_L3_IIF;
            } else if (*action_value == 0x2) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_L3_DST;
            } else if (*action_value == 0x4) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_L3_SRC;
            } else if (*action_value == 0x8) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_NEW;
            } else {
                SHR_ERR_EXIT(BCM_E_NOT_FOUND);
            }
            break;

        case bcmFieldActionPacketStatus:
            if (*action_value & 0x1) {
                params->param0 |= BCM_FIELD_PKT_STATUS_MCASTKNOWN;
            }
            if (*action_value & 0x2) {
                params->param0 |= BCM_FIELD_PKT_STATUS_MCASTUNKNOWN;
            }
            if (*action_value & 0x4) {
                params->param0 |= BCM_FIELD_PKT_STATUS_UCASTUNKNOWN;
            }
            if (*action_value & 0x8) {
                params->param0 |= BCM_FIELD_PKT_STATUS_BCAST;
            }
            break;

        case bcmFieldActionForwardingStatus:
            if (*action_value & 0x1) {
                params->param0 |= BCM_FIELD_FORWARDING_STATUS_L2_SWITCHED;
            }
            if (*action_value & 0x2) {
                params->param0 |= BCM_FIELD_FORWARDING_STATUS_L3_UNICAST_ROUTED;
            }
            if (*action_value & 0x4) {
                params->param0 |= BCM_FIELD_FORWARDING_STATUS_IPMC_ROUTED;
            }
            break;

        case bcmFieldActionIntStatReq0:
        case bcmFieldActionIntStatReq1:
            if (*action_value == 0x0) {
                params->param0 = BCM_FIELD_INT_DO_NOT_ADD_STATISTICS;
            } else if (*action_value == 0x1) {
                params->param0 = BCM_FIELD_INT_QUEUE_BUFFER;
            } else if (*action_value == 0x2) {
                params->param0 = BCM_FIELD_INT_QUEUE_WATERMARK;
            } else if (*action_value == 0x3) {
                params->param0 = BCM_FIELD_INT_POOL_BUFFER_AVAILABLE;
            } else if (*action_value == 0x4) {
                params->param0 = BCM_FIELD_INT_MIN_BUFFER_AVAILABLE;
            } else {
                SHR_ERR_EXIT(BCM_E_NOT_FOUND);
            }
            break;
        case bcmFieldActionIngressDstTypeSelect:
            if (*action_value == 1) {
                params->param0 = bcmFieldDstTypeL2EgrIntf;
            } else if (*action_value == 3) {
                params->param0 = bcmFieldDstTypeVp;
            } else if (*action_value == 4) {
                params->param0 = bcmFieldDstTypeEcmp;
            } else if (*action_value == 5) {
                params->param0 = bcmFieldDstTypeL3Egr;
            } else if (*action_value == 6) {
                params->param0 = bcmFieldDstTypeL2Mcast;
            } else if (*action_value == 7) {
                params->param0 = bcmFieldDstTypeL3Mcast;
            } else {
                SHR_ERR_EXIT(BCM_E_NOT_FOUND);
            }
            break;
        case bcmFieldActionRedirectPort:
            if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_MPLS) {
                gport_info.mpls_id = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
            } else if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_WLAN) {
                gport_info.wlan_id = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_WLAN_PORT;
            } else if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_VXLAN) {
                gport_info.vxlan_id = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_VXLAN_PORT;
            } else if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_VLAN) {
                gport_info.vlan_vp_id = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_VLAN_PORT;
            } else if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_NIV) {
                gport_info.niv_id = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_NIV_PORT;
            } else if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_MIM) {
                gport_info.mim_id = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_MIM_PORT;
            } else if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_FLOW) {
                gport_info.flow_id = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_FLOW_PORT;
            } else if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_MODPORT) {
                gport_info.port = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_MODPORT;
            } else {
                SHR_ERR_EXIT(BCM_E_NOT_FOUND);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_construct(unit, &gport_info, &dest_gport));
            params->param0 = dest_gport;
            break;
        case bcmFieldActionL3Switch:
            if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_EGR_OBJ_OVERLAY) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_egress_obj_id_construct(unit, *action_value,
                                            BCMI_LTSW_L3_EGR_OBJ_T_OL, &if_id));

            } else if (entry_oper->action_flags &
                        BCMINT_FIELD_ENTRY_ACTION_EGR_OBJ_ECMP_GROUP_OVERLAY) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_egress_obj_id_construct(unit, *action_value,
                                        BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL, &if_id));

            } else {
                SHR_ERR_EXIT(BCM_E_NOT_FOUND);
            }
            params->param0 = if_id;
            break;
        case bcmFieldActionRedirectMcast:
            _BCM_MULTICAST_GROUP_SET(params->param0,
                    _BCM_MULTICAST_TYPE_L2, *action_value);
            break;
        case bcmFieldActionRedirectIpmc:
            _BCM_MULTICAST_GROUP_SET(params->param0,
                    _BCM_MULTICAST_TYPE_L3, *action_value);
            break;
        case bcmFieldActionDlbEcmpMonitorEnable:
            if (*action_value == BCM_FIELD_DLB_ECMP_MON_DISABLE) {
                params->param0 = 1;
            } else if (*action_value == BCM_FIELD_DLB_ECMP_MON_ENABLE){
                params->param0 = 2;
            } else {
                SHR_ERR_EXIT(BCM_E_NOT_FOUND);
            }
            break;
        case bcmFieldActionDlbTrunkMonitorEnable:
            if (*action_value == BCM_FIELD_DLB_TRUNK_MON_DISABLE) {
                params->param0 = 1;
            } else if (*action_value == BCM_FIELD_DLB_TRUNK_MON_ENABLE){
                params->param0 = 2;
            } else {
                SHR_ERR_EXIT(BCM_E_NOT_FOUND);
            }
            break;
        case bcmFieldActionOuterVlanTagActions:
        case bcmFieldActionInnerVlanTagActions:
        case bcmFieldActionTunnelOuterVlanTagActions:
            if (*action_value == 0) {
                params->param0 = bcmFieldVlanTagActionDelete;
            } else if (*action_value == 1) {
                params->param0 = bcmFieldVlanTagActionAddOrReplace;
            } else {
                SHR_ERR_EXIT(BCM_E_NOT_FOUND);
            }
            break;
        case bcmFieldActionSrcNetworkGroup:
            if (*action_value == 0) {
                SHR_ERR_EXIT(BCM_E_NOT_FOUND);
            }
            params->param0 = ltsw_fp_get_first_bit_set_pos(*action_value);
            break;
        case bcmFieldActionRedirectEgrNextHop:
        case bcmFieldActionChangeL2Fields:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_id_construct(unit, *action_value,
                                            BCMI_LTSW_L3_EGR_OBJ_T_OL, &if_id));
            params->param0 = if_id;
            break;
        case bcmFieldActionRedirectEgrNextHopUnderlay:
        case bcmFieldActionAssignIfaHeaderFields:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_id_construct(unit, *action_value,
                                            BCMI_LTSW_L3_EGR_OBJ_T_UL, &if_id));
            params->param0 = if_id;
            break;
        case bcmFieldActionPrioIntNew:
        case bcmFieldActionMirrorOnDropProfileSet:
        case bcmFieldActionMatchPbmpDrop:
        case bcmFieldActionMatchPbmpRedirect:
        case bcmFieldActionPrioPktNew:
        case bcmFieldActionGpPrioPktNew:
        case bcmFieldActionYpPrioPktNew:
        case bcmFieldActionRpPrioPktNew:
        case bcmFieldActionGpPrioIntNew:
        case bcmFieldActionYpPrioIntNew:
        case bcmFieldActionRpPrioIntNew:
        case bcmFieldActionGpIntCongestionNotificationNew:
        case bcmFieldActionYpIntCongestionNotificationNew:
        case bcmFieldActionRpIntCongestionNotificationNew:
        case bcmFieldActionGpOpaqueCtrlId:
        case bcmFieldActionYpOpaqueCtrlId:
        case bcmFieldActionRpOpaqueCtrlId:
        case bcmFieldActionGpDscpNew:
        case bcmFieldActionYpDscpNew:
        case bcmFieldActionRpDscpNew:
        case bcmFieldActionGpStatGroup:
        case bcmFieldActionYpStatGroup:
        case bcmFieldActionRpStatGroup:
             params->param0 = *action_value;
             break;
        case bcmFieldActionOverrideTraceEvent:
             if (*action_value & 0x1) {
                 params->param0 |= BCM_FIELD_TRACE_EVENT_DO_NOT_MIRROR;
             }
             if (*action_value & 0x2) {
                 params->param0 |= BCM_FIELD_TRACE_EVENT_DO_NOT_COPY_TO_CPU;
             }
             break;
        case bcmFieldActionEgressPostOperation:
             if (*action_value == 0x0) {
                 params->param0 = BCM_FIELD_EGRESS_OPR_DEFAULT;
             } else if (*action_value == 0x1) {
                 params->param0 = BCM_FIELD_EGRESS_OPR_RECIRCLE_HDR_ENABLE;
             } else if (*action_value == 0x2) {
                 params->param0 = BCM_FIELD_EGRESS_OPR_IFA_DELETE ;
             } else {
                 SHR_ERR_EXIT(BCM_E_NOT_FOUND);
             }
             break;
        case bcmFieldActionColor:
        case bcmFieldActionGpDropPrecedence:
        case bcmFieldActionYpDropPrecedence:
        case bcmFieldActionRpDropPrecedence:
            if (*action_value == 0) {
                params->param0 = BCM_FIELD_COLOR_GREEN;
            } else if (*action_value == 3) {
                params->param0 = BCM_FIELD_COLOR_YELLOW;
            } else if (*action_value == 1) {
                params->param0 = BCM_FIELD_COLOR_RED;
            } else {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;
        case bcmFieldActionPolicerCancel:
           if (*action_value == 0) {
               params->param0 = BCM_POLICER_CANCEL_NONE;
               break;
           }

           SHR_IF_ERR_VERBOSE_EXIT
               (bcmint_field_action_map_find(unit, action,
                                             stage_info, &lt_map));
           alloc_sz = (sizeof(uint8_t) * lt_map->num_maps);
           BCMINT_FIELD_MEM_ALLOC(pool_cancel, alloc_sz, "pool cancel");

           offset = 0;
           for (iter = 0; iter < lt_map->num_maps; iter++) {
               sal_memset(&fld_def, 0, sizeof(fld_def));
               SHR_IF_ERR_VERBOSE_EXIT
                   (bcmi_lt_field_def_get(unit,
                                          stage_info->tbls_info->policy_sid,
                                          (lt_map->map[iter]).lt_field_name,
                                          &fld_def));
               if (((*action_value >> offset) & fld_def.max) == fld_def.max) {
                   pool_cancel[iter] = 1;
               }
               offset += fld_def.width;
           }

           if (pool_cancel[0] == 1) {
               if (entry_oper->action_flags &
                   BCMINT_FIELD_ENTRY_ACTION_POLICER_CANCEL_OVERLAY_0) {
                   params->param0 |= BCM_POLICER_CANCEL_POOL2;
               } else {
                   params->param0 |= BCM_POLICER_CANCEL_POOL0;
               }
           }
           if (pool_cancel[1] == 1) {
               if (entry_oper->action_flags &
                   BCMINT_FIELD_ENTRY_ACTION_POLICER_CANCEL_OVERLAY_1) {
                   params->param0 |= BCM_POLICER_CANCEL_POOL3;
               } else {
                   params->param0 |= BCM_POLICER_CANCEL_POOL1;
               }
           }
           break;
        default:
            SHR_ERR_EXIT(BCM_E_UNAVAIL);
    }

exit:
    if (pool_cancel != NULL) {
        BCMINT_FIELD_MEM_FREE(pool_cancel);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_action_value_set(
        int unit,
        bcm_field_entry_t entry_id,
        bcmi_ltsw_field_stage_t stage,
        bcm_field_action_t action,
        bcm_field_action_params_t *params,
        bcm_field_action_match_config_t *match_config,
        uint64_t *action_value)
{
    int egr_obj_idx = 0;
    int iter = 0;
    bcm_port_t lport = -1;
    uint8_t *pool_cancel = NULL;
    uint32_t alloc_sz = 0, offset = 0;
    bcmlt_field_def_t fld_def;
    bcmi_ltsw_l3_egr_obj_type_t egr_obj_type;
    const bcm_field_action_map_t *lt_map = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit,
                                     stage, &stage_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info, &entry_oper));

   /* convert the sdk6 action values to sdklt action values */
   /* Do switch case based on
    * Misc actions which needs convertions or chip specific.
    */
    switch (action) {
        /* SDK6
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_PORT  0
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_SVP   1
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_L3_IIF 2
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_FIELD_SRC 3
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_FIELD_DST 4
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_L2_SRC 5
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_L2_DST 6
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_L3_SRC 7
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_L3_DST 8
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_VLAN  9
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_VRF   10
         * #define BCM_FIELD_EGRESS_CLASS_SELECT_NEW   11
         * SDKLT
         * I2E class_id assignment
         * interface_class_id ifp_class_id_sel[0:0] == 1
         * l3_dst_class_id ifp_class_id_sel[1:1] == 1
         * l3_src_class_id ifp_class_id_sel[2:2] == 1
         * ifp_class_id  ifp_class_id_sel[3:3] == 1
         */
        case bcmFieldActionEgressClassSelect:
            if (params->param0 ==
                    BCM_FIELD_EGRESS_CLASS_SELECT_L3_IIF) {
                *action_value = 0x1;
            } else if (params->param0 ==
                    BCM_FIELD_EGRESS_CLASS_SELECT_L3_DST) {
                *action_value = 0x2;
            } else if (params->param0 ==
                    BCM_FIELD_EGRESS_CLASS_SELECT_L3_SRC) {
                *action_value = 0x4;
            } else if (params->param0 ==
                    BCM_FIELD_EGRESS_CLASS_SELECT_NEW) {
                *action_value = 0x8;
            } else {
                SHR_ERR_EXIT(BCM_E_PARAM);
            }
            break;

        case bcmFieldActionPacketStatus:
            if (params->param0 &
               (~(BCM_FIELD_PKT_STATUS_MCASTKNOWN |
                  BCM_FIELD_PKT_STATUS_MCASTUNKNOWN |
                  BCM_FIELD_PKT_STATUS_UCASTUNKNOWN |
                  BCM_FIELD_PKT_STATUS_BCAST))) {
                SHR_ERR_EXIT(BCM_E_PARAM);
            }
            if (params->param0 & BCM_FIELD_PKT_STATUS_MCASTKNOWN) {
                *action_value |= 0x1;
            }
            if (params->param0 & BCM_FIELD_PKT_STATUS_MCASTUNKNOWN) {
                *action_value |= 0x2;
            }
            if (params->param0 & BCM_FIELD_PKT_STATUS_UCASTUNKNOWN) {
                *action_value |= 0x4;
            }
            if (params->param0 & BCM_FIELD_PKT_STATUS_BCAST) {
                *action_value |= 0x8;
            }
            break;

        case bcmFieldActionForwardingStatus:
            if (params->param0 &
               (~(BCM_FIELD_FORWARDING_STATUS_L2_SWITCHED |
                  BCM_FIELD_FORWARDING_STATUS_L3_UNICAST_ROUTED |
                  BCM_FIELD_FORWARDING_STATUS_IPMC_ROUTED))) {
                SHR_ERR_EXIT(BCM_E_PARAM);
            }
            if (params->param0 & BCM_FIELD_FORWARDING_STATUS_L2_SWITCHED) {
                *action_value |= 0x1;
            }
            if (params->param0 & BCM_FIELD_FORWARDING_STATUS_L3_UNICAST_ROUTED) {
                *action_value |= 0x2;
            }
            if (params->param0 & BCM_FIELD_FORWARDING_STATUS_IPMC_ROUTED) {
                *action_value |= 0x4;
            }
            break;

        case bcmFieldActionIntStatReq0:
        case bcmFieldActionIntStatReq1:
            if (params->param0 ==
                    BCM_FIELD_INT_DO_NOT_ADD_STATISTICS) {
                *action_value = 0x0;
            } else if (params->param0 ==
                    BCM_FIELD_INT_QUEUE_BUFFER) {
                *action_value = 0x1;
            } else if (params->param0 ==
                    BCM_FIELD_INT_QUEUE_WATERMARK) {
                *action_value = 0x2;
            } else if (params->param0 ==
                    BCM_FIELD_INT_POOL_BUFFER_AVAILABLE) {
                *action_value = 0x3;
            } else if (params->param0 ==
                    BCM_FIELD_INT_MIN_BUFFER_AVAILABLE) {
                *action_value = 0x4;
            } else {
                SHR_ERR_EXIT(BCM_E_PARAM);
            }
            break;
        case bcmFieldActionIngressDstTypeSelect:
            if (params->param0 == bcmFieldDstTypeL2EgrIntf) {
                *action_value = 1;
            } else if (params->param0 == bcmFieldDstTypeVp) {
                *action_value = 3;
            } else if (params->param0 == bcmFieldDstTypeEcmp) {
                *action_value = 4;
            } else if (params->param0 == bcmFieldDstTypeL3Egr) {
                *action_value = 5;
            } else if (params->param0 == bcmFieldDstTypeL2Mcast) {
                *action_value = 6;
            } else if (params->param0 == bcmFieldDstTypeL3Mcast) {
                *action_value = 7;
            } else {
                SHR_ERR_EXIT(BCM_E_PARAM);
            }
            break;
        case bcmFieldActionRedirectPort:
            if ((!BCM_GPORT_IS_MPLS_PORT(params->param1)) &&
                (!BCM_GPORT_IS_WLAN_PORT(params->param1)) &&
                (!BCM_GPORT_IS_VLAN_PORT(params->param1)) &&
                (!BCM_GPORT_IS_VXLAN_PORT(params->param1)) &&
                (!BCM_GPORT_IS_NIV_PORT(params->param1)) &&
                (!BCM_GPORT_IS_MIM_PORT(params->param1)) &&
                (!BCM_GPORT_IS_FLOW_PORT(params->param1)) &&
                (!BCM_GPORT_IS_MODPORT(params->param1))) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_validate(unit, params->param1, &lport));
            if (BCM_GPORT_IS_MPLS_PORT(params->param1)) {
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_MPLS;
            } else if (BCM_GPORT_IS_WLAN_PORT(params->param1)) {
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_WLAN;
            } else if (BCM_GPORT_IS_VXLAN_PORT(params->param1)) {
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_VXLAN;
            } else if (BCM_GPORT_IS_VLAN_PORT(params->param1)) {
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_VLAN;
            } else if (BCM_GPORT_IS_NIV_PORT(params->param1)) {
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_NIV;
            } else if (BCM_GPORT_IS_MIM_PORT(params->param1)) {
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_MIM;
            } else if (BCM_GPORT_IS_FLOW_PORT(params->param1)) {
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_FLOW;
            } else {
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_MODPORT;
            }
            *action_value = lport;
            break;
        case bcmFieldActionL3Switch:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_id_resolve(unit, params->param0,
                                           &egr_obj_idx, &egr_obj_type));
            /* Check supported obj type */
            if (!((egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_OL) ||
                  (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL))) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            if (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_OL) {
                entry_oper->action_flags |=
                   BCMINT_FIELD_ENTRY_ACTION_EGR_OBJ_OVERLAY;
            } else {
                entry_oper->action_flags |=
                   BCMINT_FIELD_ENTRY_ACTION_EGR_OBJ_ECMP_GROUP_OVERLAY;
            }
            *action_value = egr_obj_idx;
            break;
        case bcmFieldActionRedirectMcast:
            if ((0 == BCM_MULTICAST_IS_SET(params->param0)) ||
                (!(_BCM_MULTICAST_IS_L2(params->param0)))) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            *action_value = _BCM_MULTICAST_ID_GET(params->param0);
            break;
        case bcmFieldActionRedirectIpmc:
            if ((0 == BCM_MULTICAST_IS_SET(params->param0)) ||
                (!(_BCM_MULTICAST_IS_L3(params->param0)))) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            *action_value = _BCM_MULTICAST_ID_GET(params->param0);
            break;
        case bcmFieldActionDlbEcmpMonitorEnable:
            if (params->param0 == BCM_FIELD_DLB_ECMP_MON_DISABLE) {
                *action_value = 1;
            } else if (params->param0 == BCM_FIELD_DLB_ECMP_MON_ENABLE) {
                *action_value = 2;
            } else {
                SHR_ERR_EXIT(BCM_E_PARAM);
            }
            break;
        case bcmFieldActionDlbTrunkMonitorEnable:
            if (params->param0 == BCM_FIELD_DLB_TRUNK_MON_DISABLE) {
                *action_value = 1;
            } else if (params->param0 == BCM_FIELD_DLB_TRUNK_MON_ENABLE) {
                *action_value = 2;
            } else {
                SHR_ERR_EXIT(BCM_E_PARAM);
            }
            break;
        case bcmFieldActionOuterVlanTagActions:
        case bcmFieldActionInnerVlanTagActions:
        case bcmFieldActionTunnelOuterVlanTagActions:
            if (params->param0 == bcmFieldVlanTagActionDelete) {
                *action_value = 0;
            } else if (params->param0 == bcmFieldVlanTagActionAddOrReplace) {
                *action_value = 1;
            } else {
                SHR_ERR_EXIT(BCM_E_PARAM);
            }
            break;
        case bcmFieldActionPrioIntNew:
        case bcmFieldActionMirrorOnDropProfileSet:
        case bcmFieldActionMatchPbmpDrop:
        case bcmFieldActionMatchPbmpRedirect:
        case bcmFieldActionPrioPktNew:
        case bcmFieldActionGpPrioPktNew:
        case bcmFieldActionYpPrioPktNew:
        case bcmFieldActionRpPrioPktNew:
        case bcmFieldActionGpPrioIntNew:
        case bcmFieldActionYpPrioIntNew:
        case bcmFieldActionRpPrioIntNew:
        case bcmFieldActionGpIntCongestionNotificationNew:
        case bcmFieldActionYpIntCongestionNotificationNew:
        case bcmFieldActionRpIntCongestionNotificationNew:
        case bcmFieldActionGpOpaqueCtrlId:
        case bcmFieldActionYpOpaqueCtrlId:
        case bcmFieldActionRpOpaqueCtrlId:
        case bcmFieldActionGpDscpNew:
        case bcmFieldActionYpDscpNew:
        case bcmFieldActionRpDscpNew:
        case bcmFieldActionGpStatGroup:
        case bcmFieldActionYpStatGroup:
        case bcmFieldActionRpStatGroup:
            *action_value = params->param0;
            break;
        case bcmFieldActionSrcNetworkGroup:
            *action_value = (1 << params->param0);
            break;
        case bcmFieldActionEcmpRandomRoundRobinHashCancel:
           if (params->param0 != 0) {
               SHR_ERR_EXIT(BCM_E_PARAM);
           }
           *action_value = 3;
           break;
        case bcmFieldActionTrunkRandomRoundRobinHashCancel:
        case bcmFieldActionDynamicTrunkCancel:
           if (params->param0 != 0) {
               SHR_ERR_EXIT(BCM_E_PARAM);
           }
           *action_value = 1;
           break;
        case bcmFieldActionRedirectCancel:
           *action_value = 0;
           break;
        case bcmFieldActionRedirectEgrNextHop:
        case bcmFieldActionChangeL2Fields:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_id_resolve(unit, params->param0,
                                           &egr_obj_idx, &egr_obj_type));
            /* Check supported obj type */
            if (!(egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_OL)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            *action_value = egr_obj_idx;
            break;
        case bcmFieldActionRedirectEgrNextHopUnderlay:
        case bcmFieldActionAssignIfaHeaderFields:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_id_resolve(unit, params->param0,
                                                    &egr_obj_idx, &egr_obj_type));
            /* Check supported obj type */
            if (!(egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_UL)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            *action_value = egr_obj_idx;
            break;
        case bcmFieldActionOverrideTraceEvent:
            if (params->param0 &
                    (~(BCM_FIELD_TRACE_EVENT_DO_NOT_MIRROR |
                       BCM_FIELD_TRACE_EVENT_DO_NOT_COPY_TO_CPU))) {
                SHR_ERR_EXIT(BCM_E_PARAM);
            }
            if (params->param0 & BCM_FIELD_TRACE_EVENT_DO_NOT_MIRROR) {
                *action_value |= 0x1;
            }
            if (params->param0 & BCM_FIELD_TRACE_EVENT_DO_NOT_COPY_TO_CPU) {
                *action_value |= 0x2;
            }
            break;
        case bcmFieldActionEgressPostOperation:
            if (params->param0 ==
                    BCM_FIELD_EGRESS_OPR_DEFAULT) {
                *action_value = 0x0;
            } else if (params->param0 ==
                    BCM_FIELD_EGRESS_OPR_RECIRCLE_HDR_ENABLE) {
                *action_value = 0x1;
            } else if (params->param0 ==
                    BCM_FIELD_EGRESS_OPR_IFA_DELETE) {
                *action_value = 0x2;
            } else {
                SHR_ERR_EXIT(BCM_E_PARAM);
            }
            break;
        case bcmFieldActionColor:
        case bcmFieldActionGpDropPrecedence:
        case bcmFieldActionYpDropPrecedence:
        case bcmFieldActionRpDropPrecedence:
            if (params->param0 == BCM_FIELD_COLOR_GREEN) {
                *action_value = 0;
            } else if (params->param0 == BCM_FIELD_COLOR_YELLOW) {
                *action_value = 3;
            } else if (params->param0 == BCM_FIELD_COLOR_RED) {
                *action_value = 1;
            } else {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;
        case bcmFieldActionEgressMembershipCheck:
           if (params->param0 != 0) {
               SHR_ERR_EXIT(BCM_E_PARAM);
           }
           *action_value = 1;
           break;
        case bcmFieldActionPolicerCancel:
           if (params->param0 == BCM_POLICER_CANCEL_NONE) {
               *action_value = 0;
               break;
           }
           SHR_IF_ERR_VERBOSE_EXIT
               (bcmint_field_action_map_find(unit, action,
                                             stage_info, &lt_map));
           alloc_sz = (sizeof(uint8_t) * lt_map->num_maps);
           BCMINT_FIELD_MEM_ALLOC(pool_cancel, alloc_sz, "pool cancel");

           if ((params->param0 & BCM_POLICER_CANCEL_POOL0) ||
               (params->param0 & BCM_POLICER_CANCEL_POOL2)) {
               pool_cancel[0] = 1;
               if (params->param0 & BCM_POLICER_CANCEL_POOL2) {
                   entry_oper->action_flags |=
                       BCMINT_FIELD_ENTRY_ACTION_POLICER_CANCEL_OVERLAY_0;
               }
           }
           if ((params->param0 & BCM_POLICER_CANCEL_POOL1) ||
               (params->param0 & BCM_POLICER_CANCEL_POOL3)) {
               pool_cancel[1] = 1;
               if (params->param0 & BCM_POLICER_CANCEL_POOL3) {
                   entry_oper->action_flags |=
                       BCMINT_FIELD_ENTRY_ACTION_POLICER_CANCEL_OVERLAY_1;
               }
           }

           offset = 0;
           for (iter = 0; iter < lt_map->num_maps; iter++) {
               sal_memset(&fld_def, 0, sizeof(fld_def));
               SHR_IF_ERR_VERBOSE_EXIT
                   (bcmi_lt_field_def_get(unit,
                                          stage_info->tbls_info->policy_sid,
                                          (lt_map->map[iter]).lt_field_name,
                                          &fld_def));
               if (pool_cancel[iter] == 1) {
                   *action_value |= (fld_def.max << offset);
               }
               offset += fld_def.width;
           }
           break;
        default:
            SHR_ERR_EXIT(BCM_E_UNAVAIL);
    }

exit:
    if (pool_cancel != NULL) {
        BCMINT_FIELD_MEM_FREE(pool_cancel);
    }
    SHR_FUNC_EXIT();
}

/* This api will return value/mask to be set against a macro
 * Values directly depend upon chipset.
 */
int
xfs_ltsw_field_qualify_macro_values_get(
        int unit,
        uint8 set_flag,
        bcm_qual_field_t *qual_info,
        bcm_qual_field_values_t *qual_info_values)
{
    int i = 0, flags_supported = 0;
    uint32 data_incoming_flag = 0, mask_incoming_flag = 0;
    SHR_FUNC_ENTER(unit);

    if ((qual_info == NULL) ||
        (qual_info_values == NULL)) {
        SHR_ERR_EXIT(BCM_E_INTERNAL);
    }

    switch(qual_info->sdk6_qual_enum) {

        case bcmFieldQualifyHiGig3Flags:
            qual_info_values->num = 4;
            qual_info_values->macro_flags[0] = 0;
            qual_info_values->macro_flags[1] = BCM_FIELD_HIGIG3_SWITCH_GENERATED;
            qual_info_values->macro_flags[2] = BCM_FIELD_HIGIG3_LAG_FAILOVER;
            qual_info_values->macro_flags[3] = BCM_FIELD_HIGIG3_DO_NOT_LEARN;
            break;

        case bcmFieldQualifyLoopbackHiGig3Flags:
            qual_info_values->num = 4;
            qual_info_values->macro_flags[0] = 0;
            qual_info_values->macro_flags[1] = BCM_FIELD_HIGIG3_SWITCH_GENERATED;
            qual_info_values->macro_flags[2] = BCM_FIELD_HIGIG3_LAG_FAILOVER;
            qual_info_values->macro_flags[3] = BCM_FIELD_HIGIG3_DO_NOT_LEARN;
            break;

        case bcmFieldQualifyL2MacInfo:
            qual_info_values->num = 8;
            qual_info_values->macro_flags[0] = BCM_FIELD_L2MAC_DMAC_MATCH_DIP;
            qual_info_values->macro_flags[1] = BCM_FIELD_L2MAC_SMAC_LT_DMAC;
            qual_info_values->macro_flags[2] = BCM_FIELD_L2MAC_SMAC_EQ_DMAC;
            qual_info_values->macro_flags[3] = BCM_FIELD_L2MAC_BCAST_DMAC;
            qual_info_values->macro_flags[4] = BCM_FIELD_L2MAC_MCAST_DMAC;
            qual_info_values->macro_flags[5] = BCM_FIELD_L2MAC_IPMCAST_DMAC;
            qual_info_values->macro_flags[6] = BCM_FIELD_L2MAC_ZERO_SMAC;
            qual_info_values->macro_flags[7] = BCM_FIELD_L2MAC_MCAST_SMAC;
            break;

        case bcmFieldQualifyIpInfo:

            qual_info_values->num = 17;
            qual_info_values->macro_flags[0] = BCM_FIELD_IP_VERSION_V4;
            qual_info_values->macro_flags[1] = BCM_FIELD_IP_VERSION_V6;
            qual_info_values->macro_flags[2] = BCM_FIELD_IP_CHECKSUM_OK;
            qual_info_values->macro_flags[3] = BCM_FIELD_IP_HDR_SIP_LT_DIP;
            qual_info_values->macro_flags[4] = BCM_FIELD_IP_HDR_SIP_EQ_DIP;
            qual_info_values->macro_flags[5] = BCM_FIELD_IP_HDR_BCAST_DIP;
            qual_info_values->macro_flags[6] = BCM_FIELD_IP_HDR_MCAST_DIP;
            qual_info_values->macro_flags[7] = BCM_FIELD_IP_HDR_RESERVED_MCAST_DIP;
            qual_info_values->macro_flags[8] = BCM_FIELD_IP_ERROR_DIP;
            qual_info_values->macro_flags[9] = BCM_FIELD_IP_ERROR_SIP;
            qual_info_values->macro_flags[10] = BCM_FIELD_IP_HDR_BFD_LOOPBACK_DIP;
            qual_info_values->macro_flags[11] = BCM_FIELD_IP_SPECIAL_DIP;
            qual_info_values->macro_flags[12] = BCM_FIELD_IP_SMALL_PKT;
            qual_info_values->macro_flags[13] = BCM_FIELD_IP_BIG_PKT;
            qual_info_values->macro_flags[14] = BCM_FIELD_IP_HDR_FLAGS_MF;
            qual_info_values->macro_flags[15] = BCM_FIELD_IP_HDR_FRAG_FLAGS_OFFSET_NON_ZERO;
            qual_info_values->macro_flags[16] = BCM_FIELD_IP_HDR_FRAG_FLAGS_OFFSET_ONE;
            break;

        case bcmFieldQualifyL4Info:
            qual_info_values->num = 3;
            qual_info_values->macro_flags[0] = BCM_FIELD_L4_HDR_SRCPORT_LT_1024;
            qual_info_values->macro_flags[1] = BCM_FIELD_L4_HDR_SRCPORT_EQ_DSTPORT;
            qual_info_values->macro_flags[2] = BCM_FIELD_L4_TCP_SEQUENCE_ZERO;
            break;

        case bcmFieldQualifyL2LearnCommands:
            qual_info_values->num = 4;
            qual_info_values->macro_flags[0] = BCM_FIELD_L2_LEARN_COMMANDS_DO_NOT_LEARN;
            qual_info_values->macro_flags[1] = BCM_FIELD_L2_LEARN_COMMANDS_LEARN_MACSA;
            qual_info_values->macro_flags[2] = BCM_FIELD_L2_LEARN_COMMANDS_RELEARN_MACSA;
            qual_info_values->macro_flags[3] = BCM_FIELD_L2_LEARN_COMMANDS_UNRESOLVED_MACSA;
            break;

        case bcmFieldQualifyTunnelTermStatus:
            qual_info_values->num = 4;
            qual_info_values->macro_flags[0] = BCM_FIELD_TUNNEL_TERM;
            qual_info_values->macro_flags[1] = BCM_FIELD_TUNNEL_TERM_L3;
            qual_info_values->macro_flags[2] = BCM_FIELD_TUNNEL_TERM_L2;
            qual_info_values->macro_flags[3] = BCM_FIELD_TUNNEL_TERM_TRIGGER_SECOND_PASS;
            break;

        case bcmFieldQualifyLoopbackHdrFlags:
            qual_info_values->num = 3;
            qual_info_values->macro_flags[0] = BCM_FIELD_LOOPBACK_VISIBILITY_PKT;
            qual_info_values->macro_flags[1] = BCM_FIELD_LOOPBACK_ROUTED_PKT;
            qual_info_values->macro_flags[2] = BCM_FIELD_LOOPBACK_RECIRCLE_HDR_PKT;
            break;

        case bcmFieldQualifyPacketStatus:

            qual_info_values->num = 4;
            qual_info_values->macro_flags[0] = BCM_FIELD_PKT_STATUS_MCASTKNOWN;
            qual_info_values->macro_flags[1] = BCM_FIELD_PKT_STATUS_MCASTUNKNOWN;
            qual_info_values->macro_flags[2] = BCM_FIELD_PKT_STATUS_UCASTUNKNOWN;
            qual_info_values->macro_flags[3] = BCM_FIELD_PKT_STATUS_BCAST;
            break;

        case bcmFieldQualifyForwardingStatus:
            qual_info_values->num = 3;
            qual_info_values->macro_flags[0] = BCM_FIELD_FORWARDING_STATUS_L2_SWITCHED;
            qual_info_values->macro_flags[1] = BCM_FIELD_FORWARDING_STATUS_L3_UNICAST_ROUTED;
            qual_info_values->macro_flags[2] = BCM_FIELD_FORWARDING_STATUS_IPMC_ROUTED;
            break;

        case bcmFieldQualifyForwardingEligible:
            qual_info_values->num = 4;
            qual_info_values->macro_flags[0] = BCM_FIELD_FORWARDING_ELIGIBLE_L3_UNICAST_ROUTE;
            qual_info_values->macro_flags[1] = BCM_FIELD_FORWARDING_ELIGIBLE_IPMC_ROUTE;
            qual_info_values->macro_flags[2] = BCM_FIELD_FORWARDING_ELIGIBLE_L2_SWITCH;
            qual_info_values->macro_flags[3] = BCM_FIELD_FORWARDING_ELIGIBLE_IPMC_FALLBACK_TO_L2_SWITCH;
            break;

        case bcmFieldQualifyOuterL2MacInfo:
            qual_info_values->num = 1;
            qual_info_values->macro_flags[0] = BCM_FIELD_L2MAC_DMAC_MATCH_DIP;
            break;

        case bcmFieldQualifyOuterIpInfo:
            qual_info_values->num = 6;
            qual_info_values->macro_flags[0] = BCM_FIELD_IP_VERSION_V4;
            qual_info_values->macro_flags[1] = BCM_FIELD_IP_VERSION_V6;
            qual_info_values->macro_flags[2] = BCM_FIELD_IP_CHECKSUM_OK;
            qual_info_values->macro_flags[3] = BCM_FIELD_IP_HDR_FLAGS_MF;
            qual_info_values->macro_flags[4] = BCM_FIELD_IP_HDR_FRAG_FLAGS_OFFSET_NON_ZERO;
            qual_info_values->macro_flags[5] = BCM_FIELD_IP_HDR_FRAG_FLAGS_OFFSET_ONE;
            break;

        case bcmFieldQualifyOuterL4Info:
            qual_info_values->num = 1;
            qual_info_values->macro_flags[0] = BCM_FIELD_L4_HDR_SRCPORT_LT_1024;
            break;

        case bcmFieldQualifyInnerL2MacInfo:
            qual_info_values->num = 1;
            qual_info_values->macro_flags[0] = BCM_FIELD_L2MAC_DMAC_MATCH_DIP;
            break;

        case bcmFieldQualifyInnerIpInfo:
            qual_info_values->num = 6;
            qual_info_values->macro_flags[0] = BCM_FIELD_IP_VERSION_V4;
            qual_info_values->macro_flags[1] = BCM_FIELD_IP_VERSION_V6;
            qual_info_values->macro_flags[2] = BCM_FIELD_IP_CHECKSUM_OK;
            qual_info_values->macro_flags[3] = BCM_FIELD_IP_HDR_FLAGS_MF;
            qual_info_values->macro_flags[4] = BCM_FIELD_IP_HDR_FRAG_FLAGS_OFFSET_NON_ZERO;
            qual_info_values->macro_flags[5] = BCM_FIELD_IP_HDR_FRAG_FLAGS_OFFSET_ONE;
            break;

        case bcmFieldQualifyInnerL4Info:
            qual_info_values->num = 1;
            qual_info_values->macro_flags[0] = BCM_FIELD_L4_HDR_SRCPORT_LT_1024;
            break;

        case bcmFieldQualifyMirrorPktType:
            qual_info_values->num = 3;
            qual_info_values->macro_flags[0] = BCM_FIELD_MIRROR_PKT_TYPE_INGRESS;
            qual_info_values->macro_flags[1] = BCM_FIELD_MIRROR_PKT_TYPE_COPY;
            qual_info_values->macro_flags[2] = BCM_FIELD_MIRROR_PKT_TYPE_COPY_TRUNCATE;
            break;

        case bcmFieldQualifyPtpPktCtrl:
            qual_info_values->num = 2;
            qual_info_values->macro_flags[0] = BCM_FIELD_PTP_MATCH_ETHERTYPE_OR_UDP_DEST_PORT;
            qual_info_values->macro_flags[1] = BCM_FIELD_PTP_MATCH_UDP_DEST_PORT;
            break;

        case bcmFieldQualifyL3Check:
            qual_info_values->num = 3;
            qual_info_values->macro_flags[0] =
                                   BCM_FIELD_L3_CHECK_GOOD_L3_UNICAST_ROUTE_PKT;
            qual_info_values->macro_flags[1] = BCM_FIELD_L3_CHECK_GOOD_IPMC_PKT;
            qual_info_values->macro_flags[2] = BCM_FIELD_L3_CHECK_PIM_FAIL;
            break;
        case bcmFieldQualifyVlanFormat:
        case bcmFieldQualifyOverlayVlanFormat:
            qual_info_values->num = 2;
            qual_info_values->macro_flags[0] = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
            qual_info_values->macro_flags[1] = BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
            break;
        case bcmFieldQualifyIntCtrl:
            qual_info_values->num = 2;
            qual_info_values->macro_flags[0] = BCM_FIELD_INT_CTRL_INSERT_IFA_RECORD;
            qual_info_values->macro_flags[1] = BCM_FIELD_INT_CTRL_DELETE_IFA_TRACE;
            break;

        default:
            SHR_ERR_EXIT(BCM_E_UNAVAIL);
    }

    /* fill macro values for input qualifier. */
    for (i = 0; i < qual_info_values->num; i++) {
        qual_info_values->macro_flags_value[i] = 1;
        qual_info_values->macro_flags_mask_value[i] = 1;
        qual_info_values->macro_flags_mask_width[i] = 1;
        flags_supported |= qual_info_values->macro_flags[i];
    }

    /* Check input values for incoming values in data & mask */
    if (set_flag) {

        data_incoming_flag = (*(uint32 *)qual_info->qual_data);
        mask_incoming_flag = (*(uint32 *)qual_info->qual_mask);

        if (data_incoming_flag & (~(flags_supported))) {
            SHR_ERR_EXIT(BCM_E_PARAM);
        }
        if (mask_incoming_flag & (~(flags_supported))) {
            SHR_ERR_EXIT(BCM_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_lt_qual_name_update(
        int unit,
        bool group_map,
        bcmint_field_stage_info_t *stage_info,
        const bcm_field_qset_t *qset,
        const bcm_field_qual_map_info_t *lt_map_ref,
        char **lt_field_name)
{
    SHR_FUNC_ENTER(unit);

    if ((NULL == lt_field_name) || (NULL != *lt_field_name)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((qset != NULL) &&
        ((sal_strncmp(lt_map_ref->lt_field_name, "CONTAINER_4_BYTE", 16) == 0) ||
         (sal_strncmp(lt_map_ref->lt_field_name, "CONTAINER_2_BYTE", 16) == 0) ||
         (sal_strncmp(lt_map_ref->lt_field_name, "CONTAINER_1_BYTE", 16) == 0))) {
        BCMINT_FIELD_MEM_ALLOC
            (*lt_field_name, strlen(lt_map_ref->lt_field_name) + 7,
             "grp_lt_qual_name");
        sal_strcpy(*lt_field_name, lt_map_ref->lt_field_name);
        if (BCMI_FIELD_QSET_TEST(*qset, bcmiFieldQualifyUdfExtOuter)) {
            sal_strcat(*lt_field_name, "_OUTER");
        } else if (BCMI_FIELD_QSET_TEST(*qset, bcmiFieldQualifyUdfExtInner)) {
            sal_strcat(*lt_field_name, "_INNER");
        }
    } else {
        BCMINT_FIELD_MEM_ALLOC
           (*lt_field_name, strlen(lt_map_ref->lt_field_name) + 1,
                "grp_lt_qual_name");
        sal_strcpy(*lt_field_name, lt_map_ref->lt_field_name);
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function Notes -
 * This api is called during group create to get pipe to which
 * group belongs to based on ports filled in group config structure.
 * This function is stage specific because pipe to which
 * ports belong to depends upon port_config_get call and actual
 * pipe to which it belongs to which varies from chip to chip.
 * In Group config structure we expect qset and ports to be passed.
 */
int
xfs_ltsw_field_group_pipe_get(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcm_pbmp_t pbmp,
        uint8_t *field_pipe)
{
    uint8_t pipe = 0;
    bcm_pbmp_t pc_pbmp;
    bcm_port_config_t port_config;
    int num_pipe = 0, oper_mode = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, BCM_E_INTERNAL);
    SHR_NULL_CHECK(field_pipe, BCM_E_INTERNAL);

    /* Retrieve port configuration */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_config_get(unit, &port_config));

    /* Retrieve operational mode status */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_oper_mode_status_get(unit, stage_info, &oper_mode));

    /* Check oper mode based on ports provided. */
    if (BCM_PBMP_EQ(pbmp, port_config.all)) {
        if (oper_mode == bcmiFieldOperModePipeUnique) {
            LOG_DEBUG(BSL_LS_BCM_FP,
               (BSL_META_U(unit, "Error: Stage operation mode"
                   " should be global mode.\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else {
        if (oper_mode != bcmiFieldOperModePipeUnique) {
            LOG_DEBUG(BSL_LS_BCM_FP,
               (BSL_META_U(unit, "Error: Stage operation mode"
                   " should be pipe unique mode.\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        /* per-pipe pbm */
        num_pipe = bcmi_ltsw_dev_max_pp_pipe_num(unit);

        for (pipe = 0; pipe < num_pipe; pipe++) {
            BCM_PBMP_ASSIGN(pc_pbmp, port_config.per_pp_pipe[pipe]);
            if (BCM_PBMP_EQ(pbmp, pc_pbmp)) {
                break;
            }
        }

        if (pipe == num_pipe) {
            LOG_DEBUG(BSL_LS_BCM_FP,
               (BSL_META_U(unit, "Error: Unable to find pipe"
                   " for provided group port.\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        /* Found pipe succesfully, return it to calling api. */
        *field_pipe = pipe;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function - This api will take group from group config structure and return
 * pbmp set for a group. It do this by retreiving PIPE to which group belongs
 * to and then use port_config_get to retrieve ports in that pipe.
 * This api is chip specific as ports and FP pipe are specific to chip. */
int
xfs_ltsw_field_group_pbmp_get(
        int unit,
        bcm_field_group_config_t *group_config,
        bcmint_field_stage_info_t *stage_info,
        bcm_pbmp_t *pbmp)
{
    int dunit = 0;
    int group_id = 0, oper_mode = 0;
    bcm_port_config_t  port_config;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmlt_entry_handle_t grp_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pbmp, SHR_E_INTERNAL);
    SHR_NULL_CHECK(group_config, SHR_E_INTERNAL);
    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);
    dunit = bcmi_ltsw_dev_dunit(unit);

    group_id = group_config->group & BCM_FIELD_ID_MASK;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, group_id,
                                        stage_info,
                                        &group_oper));

    /* Retrieve port configuration */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_config_get(unit, &port_config));

    /* Retrieve operational mode status */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_oper_mode_status_get(unit, stage_info, &oper_mode));

    if (oper_mode == bcmiFieldOperModePipeUnique) {
        uint64_t pipe;

        /* Entry handle allocate for group template */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                                  stage_info->tbls_info->group_sid,
                                  &grp_template));

        /* Add group_template_id field to group LT. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(grp_template,
                                   stage_info->tbls_info->group_key_fid,
                                   group_id));

        /* Lookup for group template entry with provided group id. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, grp_template, BCMLT_OPCODE_LOOKUP,
                                                   BCMLT_PRIORITY_NORMAL));

        /* Retreive PIPE field from template entry. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(grp_template, "PIPE", &pipe));

        /* Every TD4 FP Pipe consist of 2 pipes from device perspective. */
        BCM_PBMP_ASSIGN(*pbmp, port_config.per_pp_pipe[pipe]);
    } else {
        BCM_PBMP_ASSIGN(*pbmp, port_config.all);
    }

exit:
    if (BCMLT_INVALID_HDL != grp_template) {
        (void) bcmlt_entry_free(grp_template);
    }
    SHR_FUNC_EXIT();
}

/* Function - This api is called from qualifier_set call where user
 * passes logical port in bcm_pbmp_t structure. This pbmp has to
 * be coverted into equivalent ipbm index which need to be programmed
 * in rule template. This ipbm index comes from ING_SYSTEM_PORT_TABLE
 * which gets passed to FP in pipeline. */
int
xfs_ltsw_field_pbmp_index_get(
        int unit,
        bcm_field_group_t group,
        bcmint_field_stage_info_t *stage_info,
        bcm_pbmp_t *in_pbmp,
        bcm_pbmp_t *in_pbmp_mask,
        uint8_t *out_pbmp_idx,
        uint8_t *out_pbmp_idx_mask)
{
    int l_port = 0, shift = 0;
    int port_pipe = 0, gp_pipe = 0, l_port_index = 0;
    uint16_t *pbmp_ptr = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(in_pbmp, SHR_E_INTERNAL);
    SHR_NULL_CHECK(in_pbmp_mask, SHR_E_INTERNAL);
    SHR_NULL_CHECK(out_pbmp_idx, SHR_E_INTERNAL);
    SHR_NULL_CHECK(out_pbmp_idx_mask, SHR_E_INTERNAL);

    /* Get the group hash info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, (group & BCM_FIELD_ID_MASK),
                                        stage_info,
                                        &group_oper));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_pipe_get(unit, stage_info, group_oper, &gp_pipe));

    /* Loop trough all user provided logical ports. */
    pbmp_ptr = (uint16_t *)out_pbmp_idx;
    BCM_PBMP_ITER(*in_pbmp, l_port) {
        if (!(BCM_PBMP_MEMBER(*in_pbmp_mask, l_port))) {
            continue;
        }

        /* Retreive ipbm_index for provided logical port. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get(unit, l_port,
                            BCMI_PT_IFP_PBM_INDEX, &l_port_index));

        /* Retreive pp port pipe to which logical port belongs to. */
        port_pipe = bcmi_ltsw_dev_logic_port_pp_pipe(unit, l_port);

        /* If group is in per pipe mode and pipe for user provided
         * logical port does not match, then throw error. */
        if (gp_pipe != -1) {
            if (port_pipe != gp_pipe) {
                LOG_DEBUG(BSL_LS_BCM_FP,
                   (BSL_META_U(unit, "Error: Logical port %d pipe <%d> does"
                       " not match group pipe <%d>.\n"),
                           l_port, port_pipe, gp_pipe));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        }

        /* Determine shift based on pipe. Every FP pipe consist of 48 ports
         * per pipe hence shift data based on pipe. */
        shift = port_pipe * BCMI_TD4_IPBM_INDEX_PER_PIPE;
        BCMINT_FIELD_UINT16_BIT_SET(pbmp_ptr, (l_port_index + shift));
    }

    /* Loop trough all user provided logical ports. */
    pbmp_ptr = (uint16_t *)out_pbmp_idx_mask;
    BCM_PBMP_ITER(*in_pbmp_mask, l_port) {

        /* Retreive ipbm_index for provided logical port. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get(unit, l_port,
                            BCMI_PT_IFP_PBM_INDEX, &l_port_index));

        /* Retreive pp port pipe to which logical port belongs to. */
        port_pipe = bcmi_ltsw_dev_logic_port_pp_pipe(unit, l_port);

        /* Determine shift based on pipe. Every FP pipe consist of 48 ports
         * per pipe hence shift data based on pipe. */
        shift = port_pipe * BCMI_TD4_IPBM_INDEX_PER_PIPE;
        BCMINT_FIELD_UINT16_BIT_SET(pbmp_ptr, (l_port_index + shift));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function - This api is called from qualifier_get call where ibmp_index
 * is read from group entry LT table, based on pipe where index belongs to
 * we return logical port to user. */
int
xfs_ltsw_field_pbmp_get(
        int unit,
        bcm_field_group_t group,
        bcmint_field_stage_info_t *stage_info,
        uint8_t *out_pbmp_idx,
        uint8_t *out_pbmp_idx_mask,
        bcm_pbmp_t *in_pbmp,
        bcm_pbmp_t *in_pbmp_mask)
{
    int iter = 0, lport_val = 0;
    int pp_pipe = 0, ipbm_idx = 0;
    uint16_t idx_sz = 8 * sizeof(bcm_pbmp_t);
    uint16_t *pbmp_ptr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(out_pbmp_idx, SHR_E_INTERNAL);
    SHR_NULL_CHECK(out_pbmp_idx_mask, SHR_E_INTERNAL);
    SHR_NULL_CHECK(in_pbmp, SHR_E_INTERNAL);
    SHR_NULL_CHECK(in_pbmp_mask, SHR_E_INTERNAL);

    /* Retreive logical ports PBMP */
    pbmp_ptr = (uint16_t *)out_pbmp_idx;
    for (iter = 0; iter < idx_sz; iter++) {

        if (BCMINT_FIELD_UINT16_BIT_GET(pbmp_ptr, iter)) {

            pp_pipe = iter/BCMI_TD4_IPBM_INDEX_PER_PIPE;
            ipbm_idx = iter%BCMI_TD4_IPBM_INDEX_PER_PIPE;

            /* Retreive logical port based on
             * pp pipe and port retreived above. */
            lport_val =
                bcmi_ltsw_dev_pp_pipe_port_logic_port(unit, pp_pipe, ipbm_idx);

            /* Add logical port in user passed PBMP */
            BCM_PBMP_PORT_ADD(*in_pbmp, lport_val);
        }
    }

    /* Retreive logical ports mask PBMP */
    pbmp_ptr = (uint16_t *)out_pbmp_idx_mask;
    for (iter = 0; iter < idx_sz; iter++) {

        if (BCMINT_FIELD_UINT16_BIT_GET(pbmp_ptr, iter)) {

            pp_pipe = iter/BCMI_TD4_IPBM_INDEX_PER_PIPE;
            ipbm_idx = iter%BCMI_TD4_IPBM_INDEX_PER_PIPE;

            /* Retreive logical port based on
             * pp pipe and port retreived above. */
            lport_val =
                bcmi_ltsw_dev_pp_pipe_port_logic_port(unit, pp_pipe, ipbm_idx);

            /* Add logical port mask in user passed PBMP */
            BCM_PBMP_PORT_ADD(*in_pbmp_mask, lport_val);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_internal_entry_enable_get(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bool *enable)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, SHR_E_PARAM);
    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    *enable = TRUE;

exit:
    SHR_FUNC_EXIT();
}

/* This api will return value/mask to be set
 * against a copy to cpu reason high depend upon chipset.
 */
int
xfs_ltsw_field_qualify_copytocpureasonhigh_set(
        int unit,
        uint32_t *data,
        uint32_t *mask)
{
    int i = 0;
    uint32_t incoming_data= 0, incoming_mask = 0;
    uint32_t final_data= 0, final_mask = 0;

    SHR_FUNC_ENTER(unit);

    incoming_data = (uint32_t) *data;
    for (i = 0; i < 16; i++) {
        switch((incoming_data & (1<< i))) {
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_0:
                final_data |= 1 << 0;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_1:
                final_data |= 1 << 1;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_2:
                final_data |= 1 << 2;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_3:
                final_data |= 1 << 3;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_4:
                final_data |= 1 << 4;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_5:
                final_data |= 1 << 5;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_6:
                final_data |= 1 << 6;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_7:
                final_data |= 1 << 7;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_TRACE_DO_NOT_MIRROR:
                final_data |= 1 << 13;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_TRACE_DO_NOT_COPY_TO_CPU:
                final_data |= 1 << 14;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_TRACE_DOP:
                final_data |= 1 << 15;
                break;
           default:
                /* do nothing */
                break;
        }
    }
    *data = final_data;

    incoming_mask = (uint32_t) *mask;
    for (i = 0; i < 16; i++) {
        switch((incoming_mask & (1<< i))) {
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_0:
                final_mask |= 1 << 0;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_1:
                final_mask |= 1 << 1;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_2:
                final_mask |= 1 << 2;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_3:
                final_mask |= 1 << 3;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_4:
                final_mask |= 1 << 4;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_5:
                final_mask |= 1 << 5;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_6:
                final_mask |= 1 << 6;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_7:
                final_mask |= 1 << 7;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_TRACE_DO_NOT_MIRROR:
                final_mask |= 1 << 13;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_TRACE_DO_NOT_COPY_TO_CPU:
                final_mask |= 1 << 14;
                break;
            case BCM_FIELD_COPY_TO_CPU_REASON_HIGH_TRACE_DOP:
                final_mask |= 1 << 15;
                break;
           default:
                /* do nothing */
                break;
        }
    }
    *mask = final_mask;

    SHR_FUNC_EXIT();
}

/* This api will return value/mask to be set
 * against a copy to cpu reason high depend upon chipset.
 */
int
xfs_ltsw_field_qualify_copytocpureasonhigh_get(
        int unit,
        uint32_t *data,
        uint32_t *mask)
{
    int i = 0;
    uint32_t incoming_data= 0, incoming_mask = 0;
    uint32_t final_data= 0, final_mask = 0;

    SHR_FUNC_ENTER(unit);

    incoming_data = (uint32_t) *data;
    for (i = 0; i < 16; i++) {
        switch((incoming_data & (1<< i))) {
            case 1:
                final_data |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_0;
                break;
            case (1 << 1):
                final_data |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_1;
                break;
            case (1 << 2):
                final_data |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_2;
                break;
            case (1 << 3):
                final_data |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_3;
                break;
            case (1 << 4):
                final_data |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_4;
                break;
            case (1 << 5):
                final_data |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_5;
                break;
            case (1 << 6):
                final_data |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_6;
                break;
            case (1 << 7):
                final_data |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_7;
                break;
            case (1 << 13):
                final_data |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_TRACE_DO_NOT_MIRROR;
                break;
            case (1 << 14):
                final_data |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_TRACE_DO_NOT_COPY_TO_CPU;
                break;
            case (1 << 15):
                final_data |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_TRACE_DOP;
                break;
           default:
                /* do nothing */
                break;
        }
    }
    *data = final_data;

    incoming_mask = (uint32_t) *mask;
    for (i = 0; i < 16; i++) {
        switch((incoming_mask & (1<< i))) {
            case 1:
                final_mask |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_0;
                break;
            case (1 << 1):
                final_mask |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_1;
                break;
            case (1 << 2):
                final_mask |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_2;
                break;
            case (1 << 3):
                final_mask |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_3;
                break;
            case (1 << 4):
                final_mask |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_4;
                break;
            case (1 << 5):
                final_mask |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_5;
                break;
            case (1 << 6):
                final_mask |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_6;
                break;
            case (1 << 7):
                final_mask |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_MATCHED_RULE_BIT_7;
                break;
            case (1 << 13):
                final_mask |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_TRACE_DO_NOT_MIRROR;
                break;
            case (1 << 14):
                final_mask |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_TRACE_DO_NOT_COPY_TO_CPU;
                break;
            case (1 << 15):
                final_mask |= BCM_FIELD_COPY_TO_CPU_REASON_HIGH_TRACE_DOP;
                break;
           default:
                /* do nothing */
                break;
        }
    }
    *mask = final_mask;

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_policer_attach(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_entry_oper_t *entry_oper,
        bcm_policer_t policer_id)
{
    int dunit = 0, idx = 0;
    int pipe = -1, group_id = 0;
    bool meter_set = false;
    uint16_t pool_id = 0, alloc_sz = 0;
    uint32_t pid = 0;
    uint64_t meter_action_set = 0, val64 = 0;
    char *cont_field = NULL;
    bcm_field_entry_t entry = 0;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    const bcm_field_action_map_t *lt_map = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmlt_entry_handle_t meter_template = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t policy_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);

    pid = (policer_id & BCM_FIELD_ID_MASK);

    tbls_info = stage_info->tbls_info;
    SHR_NULL_CHECK(tbls_info, SHR_E_INTERNAL);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              tbls_info->meter_sid,
                              &meter_template));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(meter_template,
                               tbls_info->meter_key_fid,
                               pid));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, meter_template, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(meter_template,
                               "FP_METER_ACTION_SET",
                               &meter_action_set));

    group_id = entry_oper->group_id;

    /* Get the group hash info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, group_id,
                                        stage_info,
                                        &group_oper));

    /* Group pipe info get */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_pipe_get(unit, stage_info, group_oper, &pipe));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit,
                                      bcmFieldActionPolicerGroup,
                                      stage_info,
                                      &lt_map));

    pool_id = group_oper->policer_pool_id;
    entry = entry_oper->entry_id;
    for (idx = 0; idx < lt_map->num_maps; idx++) {
        if ((lt_map->map[idx]).color_index != pool_id) {
            continue;
        }

        if (meter_set == true) {
            val64 = (lt_map->map[idx]).param;
        } else {
            val64 = meter_action_set;
            meter_set = true;
        }

        alloc_sz = (strlen(lt_map->map[idx].lt_field_name) + 1);
        BCMINT_FIELD_MEM_ALLOC(cont_field, alloc_sz, "field name");
        sal_strcpy(cont_field, lt_map->map[idx].lt_field_name);

        policy_template = BCMLT_INVALID_HDL;
        /* Set METER_ACTION_SET field in the policy table */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_lt_entry_commit(unit, &policy_template,
                                          BCMLT_OPCODE_UPDATE,
                                          BCMLT_PRIORITY_NORMAL,
                                          tbls_info->policy_sid,
                                          tbls_info->policy_key_fid,
                                          0, entry & BCM_FIELD_ID_MASK, NULL,
                                          cont_field,
                                          val64,
                                          0, 0,
                                          1));

        /* Set/clear METER_ACTION_SET field in the SBR table */
        if (lt_map->map[idx].sbr_enabled == TRUE) {
            if (group_oper->group_flags & BCMINT_FIELD_GROUP_ENT_LVL_ACTION_RESOLVE) {
                if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_entry_strength_action_update(
                                unit,
                                stage_info,
                                group_oper,
                                entry_oper,
                                bcmFieldActionPolicerGroup,
                                lt_map,
                                1));
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_action_entry_update(unit,
                                                      entry_oper,
                                                      bcmFieldActionPolicerGroup,
                                                      true));
            }
        }
        BCMINT_FIELD_MEM_FREE(cont_field);
    }

exit:
    if (cont_field != NULL) {
        BCMINT_FIELD_MEM_FREE(cont_field);
    }
    if (BCMLT_INVALID_HDL != meter_template) {
        (void) bcmlt_entry_free(meter_template);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_policer_detach(
        int unit,
        bcm_field_entry_t entry)
{
    int entry_id = 0, group_id = 0;
    int idx = 0;
    uint16_t pool_id = 0, alloc_sz = 0;
    uint64_t val64 = 0;
    char *cont_field = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmlt_entry_handle_t policy_template = BCMLT_INVALID_HDL;
    const bcm_field_action_map_t *lt_map = NULL;

    SHR_FUNC_ENTER(unit);

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry,
                                           &stage_info));

    tbls_info = stage_info->tbls_info;
    SHR_NULL_CHECK(tbls_info, SHR_E_INTERNAL);

    entry_id = entry & BCM_FIELD_ID_MASK;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info,
                                        &entry_oper));

    group_id = entry_oper->group_id;

    /* Get the group hash info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, group_id,
                                        stage_info,
                                        &group_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit,
                                      bcmFieldActionPolicerGroup,
                                      stage_info,
                                      &lt_map));
    pool_id = group_oper->policer_pool_id;

    for (idx = 0; idx < lt_map->num_maps; idx++) {
        if ((lt_map->map[idx]).color_index != pool_id) {
            continue;
        }

        alloc_sz = (strlen(lt_map->map[pool_id].lt_field_name) + 1);
        BCMINT_FIELD_MEM_ALLOC(cont_field, alloc_sz, "field name");
        sal_strcpy(cont_field, lt_map->map[pool_id].lt_field_name);

        val64 = 0;
        /* Clear METER_ACTION_SET field in the policy table */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_lt_entry_commit(unit, &policy_template,
                                          BCMLT_OPCODE_UPDATE,
                                          BCMLT_PRIORITY_NORMAL,
                                          tbls_info->policy_sid,
                                          tbls_info->policy_key_fid,
                                          0, entry & BCM_FIELD_ID_MASK, NULL,
                                          cont_field,
                                          val64,
                                          0, 0,
                                          1));

        if ((lt_map->map[idx].sbr_enabled == TRUE) &&
            (group_oper->group_flags & BCMINT_FIELD_GROUP_ENT_LVL_ACTION_RESOLVE)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_action_entry_update(unit,
                                                  entry_oper,
                                                  bcmFieldActionPolicerGroup,
                                                  false));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_entry_strength_action_update(unit,
                                                           stage_info,
                                                           group_oper,
                                                           entry_oper,
                                                           bcmFieldActionPolicerGroup,
                                                           lt_map,
                                                           0));
        }

        BCMINT_FIELD_MEM_FREE(cont_field);
    }
exit:
    if (cont_field != NULL) {
        BCMINT_FIELD_MEM_FREE(cont_field);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_qual_mask_lt_field_get(
        int unit,
        char *qual_field,
        char **qual_mask_field)
{
    SHR_FUNC_ENTER(unit);

    sal_strcpy(*qual_mask_field, qual_field);
    sal_strcat(*qual_mask_field,"_MASK");

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_entry_table_id_get(
        int unit,
        const char *table_name,
        uint32_t *table_id,
        uint32_t *entry_idx)
{
    SHR_FUNC_ENTER(unit);
    if (0 == strcmp(table_name, "IFTA90_E2T_00_B0_DOUBLEm")) {
        *table_id = 0;
    } else if (0 == strcmp(table_name, "IFTA90_E2T_00_B0_QUADm")) {
        *table_id = 0;
        *entry_idx = (*entry_idx * 2);
    } else if (0 == strcmp(table_name, "IFTA90_E2T_00_B1_DOUBLEm")) {
        *table_id = 1;
    } else if (0 == strcmp(table_name, "IFTA90_E2T_00_B1_QUADm")) {
        *table_id = 1;
        *entry_idx = (*entry_idx * 2);
    } else if (0 == strcmp(table_name, "IFTA90_E2T_01_B0_DOUBLEm")) {
        *table_id = 2;
    } else if (0 == strcmp(table_name, "IFTA90_E2T_01_B0_QUADm")) {
        *table_id = 2;
        *entry_idx = (*entry_idx * 2);
    } else if (0 == strcmp(table_name, "IFTA90_E2T_01_B1_DOUBLEm")) {
        *table_id = 3;
    } else if (0 == strcmp(table_name, "IFTA90_E2T_01_B1_QUADm")) {
        *table_id = 3;
        *entry_idx = (*entry_idx * 2);
    } else if (0 == strcmp(table_name, "IFTA90_E2T_02_B0_DOUBLEm")) {
        *table_id = 4;
    } else if (0 == strcmp(table_name, "IFTA90_E2T_02_B0_QUADm")) {
        *table_id = 4;
        *entry_idx = (*entry_idx * 2);
    } else if (0 == strcmp(table_name, "IFTA90_E2T_02_B1_DOUBLEm")) {
        *table_id = 5;
    } else if (0 == strcmp(table_name, "IFTA90_E2T_02_B1_QUADm")) {
        *table_id = 5;
        *entry_idx = (*entry_idx * 2);
    } else if (0 == strcmp(table_name, "IFTA90_E2T_03_B0_DOUBLEm")) {
        *table_id = 6;
    } else if (0 == strcmp(table_name, "IFTA90_E2T_03_B0_QUADm")) {
        *table_id = 6;
        *entry_idx = (*entry_idx * 2);
    } else if (0 == strcmp(table_name, "IFTA90_E2T_03_B1_DOUBLEm")) {
        *table_id = 7;
    } else if (0 == strcmp(table_name, "IFTA90_E2T_03_B1_QUADm")) {
        *table_id = 7;
        *entry_idx = (*entry_idx * 2);
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_aacl_class_id_get(
        int unit,
        bcm_field_qualify_t qid,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_entry_oper_t *entry_oper,
        int *class_id,
        bcm_field_qualify_t *pair_qual)
{
    bool is_ip6 = 0;
    bool is_ip = 0;
    bcm_field_qualify_t qual = bcmFieldQualifyCount;
    bool is_pair_valid = 0;
    bool is_qual_valid = 0;
    int l4port = 0;
    int l4port_mask = 0;
    bcm_ip_t ip = 0x0;
    bcm_ip_t ip_mask = 0x0;
    bcm_ip6_t ip6;
    bcm_ip6_t ip6_mask;
    bcm_qual_field_t qual_info;
    bcm_qual_field_t pair_qual_info;
    bcmlt_entry_handle_t aacl_template_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    char *aacl_table_name = NULL;
    uint64_t class_id_val = 0;
    uint64_t ip6_val = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(entry_oper, SHR_E_INTERNAL);

    if ((qid == bcmFieldQualifySrcIp) ||
         (qid == bcmFieldQualifySrcIp6)) {
        if ((stage_info->flags &
                  BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP4_ONLY) &&
                  (qid == bcmFieldQualifySrcIp)) {
            is_ip = 1;
        } else if ((stage_info->flags &
                      BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP6_ONLY) &&
                      (qid == bcmFieldQualifySrcIp6)) {
            is_ip6 = 1;
        } else {
            if (qid == bcmFieldQualifySrcIp) {
                is_ip = 1;
                qual = bcmFieldQualifyL4SrcPort;
            } else {
                is_ip6 = 1;
                qual = bcmFieldQualifyL4SrcPort;
            }
        }
    } else if ((qid == bcmFieldQualifyDstIp) ||
               (qid == bcmFieldQualifyDstIp6)) {
        if ((stage_info->flags &
                  BCMINT_FIELD_STAGE_COMPRESSION_DST_IP4_ONLY) &&
                  (qid == bcmFieldQualifyDstIp)) {
            is_ip = 1;
        } else if ((stage_info->flags &
                      BCMINT_FIELD_STAGE_COMPRESSION_DST_IP6_ONLY) &&
                      (qid == bcmFieldQualifyDstIp6)) {
            is_ip6 = 1;
        } else {
            if (qid == bcmFieldQualifyDstIp) {
                is_ip = 1;
                qual = bcmFieldQualifyL4DstPort;
            } else {
                is_ip6 = 1;
                qual = bcmFieldQualifyL4DstPort;
            }
        }
    }

    sal_memset(&pair_qual_info, 0, sizeof(bcm_qual_field_t));
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* retrieve qualifier mask */
    if ((qual == bcmFieldQualifyL4SrcPort) ||
        (qual == bcmFieldQualifyL4DstPort)) {
        int *l4port_t = &l4port;
        int *l4port_t_mask = &l4port_mask;

        pair_qual_info.sdk6_qual_enum = qual;
        pair_qual_info.depth = 1;
        pair_qual_info.data_type = BCM_QUAL_DATA_TYPE_L4_PORT;
        pair_qual_info.flags = 0;

        pair_qual_info.qual_data = (void *) &l4port_t;
        pair_qual_info.qual_mask = (void *) &l4port_t_mask;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_field_qualifier_get(unit,
                  entry_oper->entry_id, &pair_qual_info));
        if (l4port_mask != 0) {
            is_pair_valid = 1;
        }
    }

    if (is_ip == 1) {
        bcm_ip_t *ip_t = &ip;
        bcm_ip_t *ip_t_mask = &ip_mask;

        qual_info.sdk6_qual_enum = qid;
        qual_info.depth = 1;
        qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
        qual_info.flags = 0;

        qual_info.qual_data = (void *)&ip_t;
        qual_info.qual_mask = (void *)&ip_t_mask;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_field_qualifier_get(unit,
                  entry_oper->entry_id, &qual_info));
        if (ip_mask != 0) {
            is_qual_valid = 1;
        }
    } else if (is_ip6 == 1) {
        bcm_ip6_t null_ip6;
        bcm_ip6_t *ip6_t = &ip6;
        bcm_ip6_t *ip6_t_mask = &ip6_mask;

        sal_memset(&null_ip6, 0x0, sizeof(bcm_ip6_t));
        qual_info.sdk6_qual_enum = qid;
        qual_info.depth = 1;
        qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
        qual_info.flags = 0;
        qual_info.qual_data = (void *)&ip6_t;
        qual_info.qual_mask = (void *)&ip6_t_mask;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_field_qualifier_get(unit,
                  entry_oper->entry_id, &qual_info));
        if (sal_memcmp(&ip6_mask, &null_ip6, sizeof(bcm_ip6_t))) {
            is_qual_valid = 1;
        }
    }

    /* retreive aacl class id */
    if (is_qual_valid) {

        dunit = bcmi_ltsw_dev_dunit(unit);
        switch (qual_info.sdk6_qual_enum) {
            case bcmFieldQualifySrcIp:
                aacl_table_name = "L3_IPV4_COMP_SRC";
                break;
            case bcmFieldQualifySrcIp6:
                aacl_table_name = "L3_IPV6_COMP_SRC";
                break;
            case bcmFieldQualifyDstIp:
                aacl_table_name = "L3_IPV4_COMP_DST";
                break;
            case bcmFieldQualifyDstIp6:
                aacl_table_name = "L3_IPV6_COMP_DST";
                break;
            default:
               SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }

        /* Entry handle allocate for action template */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                              aacl_table_name,
                              &aacl_template_hdl));
        if ((qual_info.sdk6_qual_enum == bcmFieldQualifySrcIp) ||
            (qual_info.sdk6_qual_enum == bcmFieldQualifyDstIp)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                               "IPV4",ip));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                               "IPV4_MASK", ip_mask));
        } else {
            bcmi_ltsw_util_ip6_half_to_uint64(&ip6_val,
                 &ip6, 1);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                               "IPV6_LOWER", ip6_val));

            bcmi_ltsw_util_ip6_half_to_uint64(&ip6_val,
                 &ip6, 0);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                               "IPV6_UPPER", ip6_val));

            bcmi_ltsw_util_ip6_half_to_uint64(&ip6_val,
                 &ip6_mask, 1);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                               "IPV6_LOWER_MASK", ip6_val));

            bcmi_ltsw_util_ip6_half_to_uint64(&ip6_val,
                 &ip6_mask, 0);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                               "IPV6_UPPER_MASK", ip6_val));
        }

        if (is_pair_valid) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                    "L4_PORT",
                    l4port));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                    "L4_PORT_MASK",
                    l4port_mask));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, aacl_template_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(aacl_template_hdl,
                               "CLASS_ID",
                               &class_id_val));
        *class_id = (int)class_id_val;

        if ((*class_id != -1) && is_pair_valid) {
            *pair_qual = pair_qual_info.sdk6_qual_enum;
        }
    }

exit:
    if (BCMLT_INVALID_HDL != aacl_template_hdl) {
        (void) bcmlt_entry_free(aacl_template_hdl);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_action_stat_group_lt_field_validate(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_hintid_t hintid,
        const char *field_name,
        bool *is_valid)
{
    int rv = 0;
    bcm_field_hint_t hint;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);

    *is_valid = false;
    if ((stage_info->stage_id == bcmiFieldStageIngress) ||
        (stage_info->stage_id == bcmiFieldStageEgress)) {
        if (sal_strcmp(field_name, "FLEX_CTR_VALID") == 0) {
            *is_valid = true;
        } else if (stage_info->stage_id == bcmiFieldStageIngress) {
            int idx;
            bool hint_valid = false;
            int num_ctrs = 14;
            int h_val[] =   {bcmFlexctrObjectIngIfpOpaqueObj0,
                             bcmFlexctrObjectIngIfpOpaqueObj1,
                             bcmFlexctrObjectIngIfpOpaqueObj1_0,
                             bcmFlexctrObjectIngIfpOpaqueObj1_1,
                             bcmFlexctrObjectIngIfpOpaqueObj1_2,
                             bcmFlexctrObjectIngIfpOpaqueObj1_3,
                             bcmFlexctrObjectIngIfpOpaqueObj2,
                             bcmFlexctrObjectIngIfpOpaqueObj3,
                             bcmFlexctrObjectIngIfpOpaqueObj4,
                             bcmFlexctrObjectIngIfpOpaqueObj5,
                             bcmFlexctrObjectIngIfpOpaqueObj6,
                             bcmFlexctrObjectIngIfpOpaqueObj7,
                             bcmFlexctrObjectIngIfpOpaqueObj8,
                             bcmFlexctrObjectIngIfpOpaqueObj9};
            char *flex_ctr_fid[] =   {stage_info->tbls_info->flex_ctr_idx0_fid,
                                      stage_info->tbls_info->flex_ctr_idx1_fid,
                                      stage_info->tbls_info->flex_ctr_idx1_fid,
                                      stage_info->tbls_info->flex_ctr_idx1_fid,
                                      stage_info->tbls_info->flex_ctr_idx1_fid,
                                      stage_info->tbls_info->flex_ctr_idx1_fid,
                                      stage_info->tbls_info->flex_ctr_idx2_fid,
                                      stage_info->tbls_info->flex_ctr_idx3_fid,
                                      stage_info->tbls_info->flex_ctr_idx4_fid,
                                      stage_info->tbls_info->flex_ctr_idx5_fid,
                                      stage_info->tbls_info->flex_ctr_idx6_fid,
                                      stage_info->tbls_info->flex_ctr_idx7_fid,
                                      stage_info->tbls_info->flex_ctr_idx8_fid,
                                      stage_info->tbls_info->flex_ctr_idx9_fid};

            for (idx = 0; idx < num_ctrs; idx++) {
                hint.value = h_val[idx];
                hint.hint_type = bcmFieldHintTypeFlexCtrOpaqueObjectSel;
                rv = bcmint_field_hints_get(unit, hintid, &hint);
                if (SHR_SUCCESS(rv)) {
                    hint_valid = true;

                    if ((sal_strcmp(field_name, flex_ctr_fid[idx]) == 0)) {
                        *is_valid = true;
                    }
                }
            }

            if (hint_valid == false) {
                *is_valid = true;
            }
        } else if (stage_info->stage_id == bcmiFieldStageEgress) {
            bool hint0 = false;
            bool hint1 = false;

            /*get hint 0 & 1*/
            hint.value = bcmFlexctrObjectEgrEfpOpaqueObj1;
            hint.hint_type = bcmFieldHintTypeFlexCtrOpaqueObjectSel;
            rv = bcmint_field_hints_get(unit, hintid, &hint);
            if (SHR_SUCCESS(rv)) {
                hint0 = true;
            }

            hint.value = bcmFlexctrObjectEgrEfpOpaqueObj2;
            hint.hint_type = bcmFieldHintTypeFlexCtrOpaqueObjectSel;
            rv = bcmint_field_hints_get(unit, hintid, &hint);
            if (SHR_SUCCESS(rv)) {
                hint1 = true;
            }

            if (hint0 || hint1) {
                if (((hint0) &&
                     (sal_strcmp(field_name,
                         stage_info->tbls_info->flex_ctr_idx0_fid) == 0)) ||
                     ((hint1) &&
                     (sal_strcmp(field_name,
                         stage_info->tbls_info->flex_ctr_idx1_fid) == 0))) {
                    *is_valid = true;
                }
            } else {
                *is_valid = true;
            }
        }
    } else {
        *is_valid = true;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_action_lt_map_valid_info_get(
       int unit,
       bcmi_ltsw_field_stage_t stage,
       bcm_field_entry_t entry_id,
       bcm_field_action_t action,
       uint16_t lt_map_flags,
       uint16_t map_count,
       bool *lt_field_map)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_action_mirror_set(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_gport_t mirror_gport)
{
    int flags = 0x0;
    int mirror_cont = -1;
    uint64_t mirror_index = 0;
    char *cont_field = NULL;
    bcmi_ltsw_mirror_info_t mirror_info;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    bcmlt_entry_handle_t policy_template = BCMLT_INVALID_HDL;
    SHR_FUNC_ENTER(unit);

    /* action validation */
    if (action == bcmFieldActionMirrorIngress) {
        flags = BCMI_LTSW_MIRROR_INGRESS;
    } else if (action == bcmFieldActionMirrorEgress) {
        flags = BCMI_LTSW_MIRROR_EGRESS;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&mirror_info, 0x0, sizeof(mirror_info));

    /* Find lt_map for provided qualifier from db. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action, stage_info, &lt_map_ref));

    /* Mirror dest container alloc */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_field_dest_add(unit, entry, flags,
                                         mirror_gport,
                                         &mirror_info));

    /* Retrieve Policy Mirror Container */
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_field_action_mirror_cont_get(unit,
                                               stage_info,
                                               entry,
                                               action,
                                               (int)mirror_info.mirror_cont,
                                               &cont_field));

    mirror_index = mirror_info.mirror_index;
    /* set mirror index in the policy table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &policy_template,
                                      BCMLT_OPCODE_UPDATE,
                                      BCMLT_PRIORITY_NORMAL,
                                      stage_info->tbls_info->policy_sid,
                                      stage_info->tbls_info->policy_key_fid,
                                      0, entry & BCM_FIELD_ID_MASK, NULL,
                                      cont_field,
                                      mirror_index,
                                      mirror_cont, 1,
                                      1));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_action_mirror_remove(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_gport_t mirror_gport)
{
    int flags = 0x0;
    char *cont_field;
    int mirror_cont = -1;
    bcmi_ltsw_mirror_info_t mirror_info;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    bcmlt_entry_handle_t policy_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* action validation */
    if (action == bcmFieldActionMirrorIngress) {
        flags = BCMI_LTSW_MIRROR_INGRESS;
    } else if (action == bcmFieldActionMirrorEgress) {
        flags = BCMI_LTSW_MIRROR_EGRESS;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&mirror_info, 0x0, sizeof(mirror_info));

    /* Find lt_map for provided qualifier from db. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action, stage_info, &lt_map_ref));

    /* Get Mirror dest container */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_field_dest_get(unit, entry, flags,
                                         mirror_gport,
                                         &mirror_info));

    /* Delete Mirror dest container */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_field_dest_delete(unit, entry, flags,
                                            mirror_gport));

    /* Retrieve Policy Mirror Container */
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_field_action_mirror_cont_get(unit,
                                               stage_info,
                                               entry,
                                               action,
                                               (int)mirror_info.mirror_cont,
                                               &cont_field));

    mirror_cont = mirror_info.mirror_index;
    /* set mirror index to 0 in the policy table */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_lt_entry_commit(unit, &policy_template,
                                     BCMLT_OPCODE_UPDATE,
                                     BCMLT_PRIORITY_NORMAL,
                                     stage_info->tbls_info->policy_sid,
                                     stage_info->tbls_info->policy_key_fid,
                                     0, entry & BCM_FIELD_ID_MASK, NULL,
                                     cont_field,
                                     0, mirror_cont,
                                     1, 1));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_action_mirror_index_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int entry,
    bcm_field_action_t action,
    int mirror_cont,
    bool *enable,
    int *mirror_index)
{
    char *cont_field;
    uint64_t mirror_idx = 0;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    bcmlt_entry_handle_t policy_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mirror_index, SHR_E_PARAM);

    /* Find lt_map for provided qualifier from db. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action,
                                      stage_info, &lt_map_ref));


    /* Retrieve Policy Mirror Container */
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_field_action_mirror_cont_get(unit,
                                               stage_info,
                                               entry,
                                               action,
                                               mirror_cont,
                                               &cont_field));

    /* Perform policy lookup */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &policy_template,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL,
                                      stage_info->tbls_info->policy_sid,
                                      stage_info->tbls_info->policy_key_fid,
                                      0, entry & BCM_FIELD_ID_MASK, NULL,
                                      NULL,
                                      0, 0, 0,
                                      0));


    /* Get Mirror index for a given mirror container */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(policy_template,
                               cont_field,
                               &mirror_idx));

    *mirror_index = (int)mirror_idx;
    if (*mirror_index) {
        *enable = true;
    }

exit:
    if (policy_template != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(policy_template);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_qualify_port_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_qualify_t qual,
    bcm_module_t *data_modid,
    bcm_module_t *mask_modid,
    bcm_port_t *data_port,
    bcm_port_t *mask_port)
{
    int sys_port = 0;
    int sys_port_mask = 0;
    bcm_gport_t gport = 0;
    bcm_gport_t gport_mask = 0;
    uint32 mod_port_data = 0;   /* concatenated modid and port */
    uint32 mod_port_mask = 0;   /* concatenated modid and port */
    uint32 *mod_port_data_get = &mod_port_data;   /* concatenated modid and port */
    uint32 *mod_port_mask_get = &mod_port_mask;   /* concatenated modid and port */

    bcm_qual_field_t qual_info;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    if ((qual != bcmFieldQualifySrcPort) &&
        (qual != bcmFieldQualifyDstPort)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = qual;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_PORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &mod_port_data_get;
    qual_info.qual_mask = (void *) &mod_port_mask_get;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    sys_port = mod_port_data;
    sys_port_mask = mod_port_mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_sys_port_to_port(unit, sys_port, &gport));

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_sys_port_to_port(unit, sys_port_mask, &gport_mask));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info,
                                        &entry_oper));

    if (((qual == bcmFieldQualifySrcPort) &&
         !(entry_oper->entry_flags & BCMINT_FIELD_ENTRY_SRC_PORT_TYPE_GPORT)) ||
        ((qual == bcmFieldQualifyDstPort) &&
         !(entry_oper->entry_flags & BCMINT_FIELD_ENTRY_DST_PORT_TYPE_GPORT))) {
         *data_port = BCM_GPORT_MODPORT_PORT_GET(gport);
         *data_modid = BCM_GPORT_MODPORT_MODID_GET(gport);

         *mask_port = BCM_GPORT_MODPORT_PORT_GET(gport_mask);
         *mask_modid = BCM_GPORT_MODPORT_MODID_GET(gport_mask);

         if (*mask_modid == bcmi_ltsw_dev_max_modid(unit)) {
             *mask_modid = BCM_FIELD_EXACT_MATCH_MASK;
         }

         if (*mask_port == bcmi_ltsw_dev_max_port_addr(unit)) {
             *mask_port = BCM_FIELD_EXACT_MATCH_MASK;
         }
    }

exit:
    SHR_FUNC_EXIT()
}

int
xfs_ltsw_field_qualify_port_set(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_qualify_t qual,
    bcm_module_t data_modid,
    bcm_module_t mask_modid,
    bcm_port_t data_port,
    bcm_port_t mask_port)
{
    bcm_qual_field_t qual_info;
    bcm_gport_t gport = 0;
    bcm_gport_t gport_mask = 0;
    int sys_port = 0;
    int sys_port_mask = 0;

    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bool is_gport = false;

    SHR_FUNC_ENTER(unit);

    if ((qual != bcmFieldQualifySrcPort) &&
        (qual != bcmFieldQualifyDstPort)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info,
                                        &entry_oper));

    if (BCM_GPORT_IS_SET(data_port)) {
        if (!(BCM_GPORT_IS_MODPORT(data_port))) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        mask_modid = BCM_FIELD_EXACT_MATCH_MASK;
        gport = data_port;
        is_gport = true;
    } else {
        BCM_GPORT_MODPORT_SET(gport, data_modid, data_port);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_to_sys_port(unit, gport, &sys_port));

    /* Set masks to all ones if exact match is specified. */
    if (mask_modid == BCM_FIELD_EXACT_MATCH_MASK) {
        mask_modid = bcmi_ltsw_dev_max_modid(unit);
    }
    if (mask_port == BCM_FIELD_EXACT_MATCH_MASK) {
        mask_port = bcmi_ltsw_dev_max_port_addr(unit);
    }

    BCM_GPORT_MODPORT_SET(gport_mask, mask_modid, mask_port);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_to_sys_port(unit, gport_mask, &sys_port_mask));

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = qual;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_PORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &sys_port;
    qual_info.qual_mask = (void *) &sys_port_mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));
    if (is_gport) {
        if (qual == bcmFieldQualifySrcPort) {
            entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_SRC_PORT_TYPE_GPORT;
        } else {
            entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_DST_PORT_TYPE_GPORT;
        }
    } else {
        if (qual == bcmFieldQualifySrcPort) {
            entry_oper->entry_flags &= ~BCMINT_FIELD_ENTRY_SRC_PORT_TYPE_GPORT;
        } else {
            entry_oper->entry_flags &= ~BCMINT_FIELD_ENTRY_DST_PORT_TYPE_GPORT;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_qualify_dstl3egress_set(
        int unit,
        bcm_field_entry_t entry,
        bcm_if_t if_id)
{
    int mask = 0;
    int egr_obj_idx = 0;
    bcm_qual_field_t qual_info;
    bcmi_ltsw_l3_egr_obj_type_t egr_obj_type;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));
    mask = -1;

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, if_id,
                                            &egr_obj_idx, &egr_obj_type));

    /* Check supported obj type */
    if (!((egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_OL) ||
                (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstL3Egress;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_INT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &egr_obj_idx;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

    if (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_OL) {
        entry_oper->entry_flags |=
            BCMINT_FIELD_ENTRY_EGR_OBJ_OVERLAY;
    } else {
        entry_oper->entry_flags |=
            BCMINT_FIELD_ENTRY_EGR_OBJ_ECMP_GROUP_OVERLAY;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_qualify_dstl3egress_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_if_t *if_id)
{
    int mask = 0;
    int *mask_get = &mask;
    int egr_obj_idx = 0;
    int *egr_obj_idx_get = &egr_obj_idx;
    bcm_qual_field_t qual_info;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstL3Egress;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_INT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &egr_obj_idx_get;
    qual_info.qual_mask = (void *) &mask_get;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));
    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_EGR_OBJ_OVERLAY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_egress_obj_id_construct(unit, egr_obj_idx,
                                    BCMI_LTSW_L3_EGR_OBJ_T_OL, if_id));
    } else if (entry_oper->entry_flags &
            BCMINT_FIELD_ENTRY_EGR_OBJ_ECMP_GROUP_OVERLAY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_egress_obj_id_construct(unit, egr_obj_idx,
                                    BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL, if_id));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_group_mode_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcm_field_group_mode_t *mode)
{
    int dunit = 0;
    bcmlt_entry_handle_t grp_template = BCMLT_INVALID_HDL;
    const char *group_mode = NULL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Entry handle allocate for Group Template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->group_sid,
                              &grp_template));

    /* Add group_template_id field to Group LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(grp_template,
                               stage_info->tbls_info->group_key_fid,
                               group_oper->group_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, grp_template, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(grp_template,
                                      "MODE_OPER",
                                      &group_mode));

    if (0 == strcmp(group_mode,"SINGLE")) {
        *mode = bcmFieldGroupModeSingle;
    } else if (0 == strcmp(group_mode,"DBLINTRA")) {
        *mode = bcmFieldGroupModeIntraSliceDouble;
    } else if (0 == strcmp(group_mode,"DBLINTER")) {
        *mode = bcmFieldGroupModeDouble;
    } else if (0 == strcmp(group_mode,"TRIPLE")) {
        *mode = bcmFieldGroupModeTriple;
    } else if (0 == strcmp(group_mode,"QUAD")) {
        *mode = bcmFieldGroupModeQuad;
    } else {
         LOG_DEBUG(BSL_LS_BCM_FP,
             (BSL_META_U(unit, "Error: %s Not a valid"
                         " FP group mode. \n"), group_mode));
         SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    if (BCMLT_INVALID_HDL != grp_template) {
        (void) bcmlt_entry_free(grp_template);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_qualify_mhopcode_set(int unit,
                                bcm_field_entry_t entry,
                                uint8 data, uint8 mask)
{
    uint8 data_val = 0, mask_val = -1;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    switch(data) {
        case BCM_FIELD_MHOPCODE_UNICAST:
            data_val = 1;
            break;
        case BCM_FIELD_MHOPCODE_IPMULTICAST:
            data_val = 4;
            break;
        case BCM_FIELD_MHOPCODE_L2MULTICAST:
            data_val = 3;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    switch(mask) {
        case BCM_FIELD_MHOPCODE_UNICAST:
            mask_val = 1;
            break;
        case BCM_FIELD_MHOPCODE_IPMULTICAST:
            mask_val = 4;
            break;
        case BCM_FIELD_MHOPCODE_L2MULTICAST:
            mask_val = 3;
            break;
        default:
            mask_val = mask;
    }

    qual_info.sdk6_qual_enum = bcmFieldQualifyMHOpcode;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data_val;
    qual_info.qual_mask = (void *) &mask_val;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

exit:
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyMHOpcode (Macro based). */
int
xfs_ltsw_field_qualify_mhopcode_get(int unit,
                                    bcm_field_entry_t entry,
                                    uint8 *data, uint8 *mask)
{
    uint8 data_val = 0, mask_val = 0;
    uint8 *data_tmp = &data_val, *mask_tmp = &mask_val;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyMHOpcode;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data_tmp;
    qual_info.qual_mask = (void *) &mask_tmp;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    if ((data_val == 0) && (mask_val == 0)) {
        *data = *mask = 0;
        SHR_EXIT();
    }

    switch (data_val) {
        case 1:
            *data = BCM_FIELD_MHOPCODE_UNICAST;
            break;
        case 3:
            *data = BCM_FIELD_MHOPCODE_L2MULTICAST;
            break;
        case 4:
            *data = BCM_FIELD_MHOPCODE_IPMULTICAST;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    switch (mask_val) {
        case 1:
            *mask = BCM_FIELD_MHOPCODE_UNICAST;
            break;
        case 3:
            *mask = BCM_FIELD_MHOPCODE_L2MULTICAST;
            break;
        case 4:
            *mask = BCM_FIELD_MHOPCODE_IPMULTICAST;
            break;
        default:
            *mask = mask_val;
    }

exit:
    SHR_FUNC_EXIT();
}


