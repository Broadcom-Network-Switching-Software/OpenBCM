/*! \file bcm56880_a0_cth_oam.c
 *
 * Chip stub for BCMCTH OAM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_oam_constants.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmlrd/chip/bcm56880_a0/bcm56880_a0_lrd_enum_ctype.h>
#include <bcmcth/bcmcth_oam_drv.h>
#include <bcmpkt/flexhdr/bcmpkt_rxpmd_flex.h>

#define BSL_LOG_MODULE BSL_LS_BCMCTH_OAM

static int
bcm56880_a0_cth_oam_bfd_num_vlans_get(uint8_t vlan_tag_type, uint8_t *num_vlans)
{
    switch (vlan_tag_type) {
        case BCM56880_A0_LRD_VLAN_TAG_TYPE_T_T_UNTAGGED:
            *num_vlans = 0;
            break;

        case BCM56880_A0_LRD_VLAN_TAG_TYPE_T_T_SINGLE_TAGGED:
            *num_vlans = 1;
            break;

        case BCM56880_A0_LRD_VLAN_TAG_TYPE_T_T_DOUBLE_TAGGED:
            *num_vlans = 2;
            break;

        default:
            return SHR_E_PARAM;
    }
    return SHR_E_NONE;
}

static int
bcm56880_a0_cth_oam_bfd_rxpmd_flex_fids_get(int unit, int max_fids,
                                            int *num_fids, int *fids)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* bcm56880 requires 1 field to be extracted from Rx PMD flex data. */
    if (max_fids < 1) {
        /* Not enough memory allocted by caller. */
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    *num_fids = 0;
    rv = bcmpkt_rxpmd_flex_field_id_get("I2E_CLASS_ID_15_0", &fids[0]);
    if (SHR_SUCCESS(rv)) {
        *num_fids = 1;
    }
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

static const char *
bcm56880_a0_cth_oam_bfd_match_id_string_get(int unit, mcs_bfd_match_id_fields_t match_id)
{
    switch (match_id) {
        case MCS_BFD_MATCH_ID_OTAG:
            return "INGRESS_PKT_OUTER_L2_HDR_OTAG";
        case MCS_BFD_MATCH_ID_ITAG:
            return "INGRESS_PKT_OUTER_L2_HDR_ITAG";
        case MCS_BFD_MATCH_ID_MPLS_0:
            return "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS0";
        case MCS_BFD_MATCH_ID_MPLS_1:
            return "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS1";
        case MCS_BFD_MATCH_ID_MPLS_2:
            return "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS2";
        case MCS_BFD_MATCH_ID_MPLS_3:
            return "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS3";
        case MCS_BFD_MATCH_ID_MPLS_4:
            return "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS4";
        case MCS_BFD_MATCH_ID_MPLS_5:
            return "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS5";
        case MCS_BFD_MATCH_ID_MPLS_6:
            return "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS6";
        case MCS_BFD_MATCH_ID_ACH:
            return "INGRESS_PKT_OUTER_L3_L4_HDR_MPLS_ACH";
        case MCS_BFD_MATCH_ID_INNER_IPV4:
            return "INGRESS_PKT_INNER_L3_L4_HDR_IPV4";
        case MCS_BFD_MATCH_ID_INNER_IPV6:
            return "INGRESS_PKT_INNER_L3_L4_HDR_IPV6";
        case MCS_BFD_MATCH_ID_GRE:
            return "INGRESS_PKT_OUTER_L3_L4_HDR_GRE";
        default:
                return NULL;
    }
}

static int
bcm56880_a0_cth_oam_deinit(int unit)
{
    return SHR_E_NONE;
}

static int
bcm56880_a0_cth_oam_init(int unit, bool warm)
{
    return SHR_E_NONE;
}

/* BFD device specific parameters */
static bcmcth_oam_bfd_drv_t bcm56880_a0_cth_oam_bfd_drv = {
    .max_mpls_labels = MAX_MPLS_LABELS,
    .num_vlans_get = &bcm56880_a0_cth_oam_bfd_num_vlans_get,
    .rxpmd_flex_fids_get = &bcm56880_a0_cth_oam_bfd_rxpmd_flex_fids_get,
    .match_id_string_get = &bcm56880_a0_cth_oam_bfd_match_id_string_get,
    .max_endpoints = 2048,
    .max_auth_sp_keys = 2048,
    .max_auth_sha1_keys = 2048
};

/* bcm56880 OAM driver object. */
static bcmcth_oam_drv_t bcm56880_a0_cth_oam_drv = {
   .oam_init = &bcm56880_a0_cth_oam_init,
   .oam_deinit = &bcm56880_a0_cth_oam_deinit,
   .bfd_drv = &bcm56880_a0_cth_oam_bfd_drv
};

/*******************************************************************************
 * Public functions
 */

bcmcth_oam_drv_t *
bcm56880_a0_cth_oam_drv_get(int unit)
{
    return &bcm56880_a0_cth_oam_drv;
}
