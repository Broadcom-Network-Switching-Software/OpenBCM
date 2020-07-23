/*
 * Common driver function definitions for field module for Tomahawk-4 family
 * devices.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>
#include <bcm_int/ltsw/field_int.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/field.h>
#include <bcm_int/ltsw/mbcm/field.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/xgs/field.h>
#include <bcm_int/ltsw/chip/bcm56990_a0/field_lt_map.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/mirror.h>
#include <bcmltd/chip/bcmltd_str.h>
#include "sub_dispatch.h"

#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_FP

#define BCMI_TH4_IPBM_INDEX_PER_PIPE 64

static const struct {
    uint8 api, hw_data, hw_mask;
} pkt_res_xlate_tbl[] = {
    {  BCM_FIELD_PKT_RES_UNKNOWN,         0x00, 0x3f },
    {  BCM_FIELD_PKT_RES_CONTROL,         0x01, 0x3f },
    {  BCM_FIELD_PKT_RES_BFD,             0x03, 0x3f },
    {  BCM_FIELD_PKT_RES_BPDU,            0x04, 0x3f },
    {  BCM_FIELD_PKT_RES_IEEE1588,        0x06, 0x3f },
    {  BCM_FIELD_PKT_RES_L2_ANY,          0x08, 0x38 },
    {  BCM_FIELD_PKT_RES_L2UC,            0x08, 0x3f },
    {  BCM_FIELD_PKT_RES_L2UNKNOWN,       0x09, 0x3f },
    {  BCM_FIELD_PKT_RES_L2MC_ANY,        0x0a, 0x3e },
    {  BCM_FIELD_PKT_RES_L2MCKNOWN,       0x0a, 0x3f },
    {  BCM_FIELD_PKT_RES_L2MCUNKNOWN,     0x0b, 0x3f },
    {  BCM_FIELD_PKT_RES_L2BC,            0x0c, 0x3f },
    {  BCM_FIELD_PKT_RES_L3UCKNOWN,       0x10, 0x3f },
    {  BCM_FIELD_PKT_RES_L3UCUNKNOWN,     0x11, 0x3f },
    {  BCM_FIELD_PKT_RES_L3MCKNOWN,       0x12, 0x3f },
    {  BCM_FIELD_PKT_RES_L3MCUNKNOWN,     0x13, 0x3f },
    {  BCM_FIELD_PKT_RES_MPLSUNKNOWN,     0x19, 0x3f },
    {  BCM_FIELD_PKT_RES_MPLSL3KNOWN,     0x1a, 0x3f },
    {  BCM_FIELD_PKT_RES_MPLSKNOWN,       0x1c, 0x3f }
};

static const bcm_field_control_map_info_t bcm_field_control_DlbMonitorIngressRandomSeed_map[] = {
    {
        .lt_name       = "DLB_ECMP_CONTROL",
        .lt_field_name = "FP_ING_SEED",
    },
};

static const bcm_field_control_map_info_t bcm_field_control_EcmpHashEnable_map[] = {
    {
        .lt_name       = "FP_CONTROL",
        .lt_field_name = "FP_ING_ECMP_HASH_ENABLE",
    },
};

static const bcm_field_control_map_info_t bcm_field_control_EcmpHashOffset_map[] = {
    {
        .lt_name       = "FP_CONTROL",
        .lt_field_name = "FP_ING_ECMP_HASH_OFFSET",
    },
};

static const bcm_field_control_map_info_t bcm_field_control_EcmpHashUseCrc_map[] = {
    {
        .lt_name       = "FP_CONTROL",
        .lt_field_name = "FP_ING_ECMP_HASH_USE_CRC",
    },
};

static const bcm_field_control_map_info_t bcm_field_control_EcmpHashUseUpper5Bits_map[] = {
    {
        .lt_name       = "FP_CONTROL",
        .lt_field_name = "FP_ING_ECMP_USE_UPPER_5_BITS",
    },
};

static const bcm_field_control_map_info_t bcm_field_control_RedirectExcludeEtherSrcPort_map[] = {
    {
        .lt_name       = "FP_CONTROL",
        .lt_field_name = "FP_ING_REDIRECT_EXCLUDE_SRCPORT",
    },
};

static const bcm_field_control_map_info_t bcm_field_control_RedirectExcludeHiGigSrcPort_map[] = {
    {
        .lt_name       = "FP_CONTROL",
        .lt_field_name = "FP_ING_REDIRECT_EXCLUDE_HGSRCPORT",
    },
};

static const bcm_field_control_map_info_t bcm_field_control_RedirectIngressVlanCheck_map[] = {
    {
        .lt_name       = "FP_CONTROL",
        .lt_field_name = "FP_ING_REDIRECT_ING_VLANCHECKS",
    },
};

static const bcm_field_control_map_info_t bcm_field_control_RedirectNonUcastEtherTrunkResolve_map[] = {
    {
        .lt_name       = "FP_CONTROL",
        .lt_field_name = "FP_ING_REDIRECT_NONUC_TRUNKRESOLVE",
    },
};

static const bcm_field_control_map_info_t bcm_field_control_RedirectVlanFloodBlock_map[] = {
    {
        .lt_name       = "FP_CONTROL",
        .lt_field_name = "FP_ING_REDIRECT_VLAN_FLOODBLOCK",
    },
};

static const bcm_field_control_map_info_t bcm_field_control_RedirectPortFloodBlock_map[] = {
    {
        .lt_name       = "FP_CONTROL",
        .lt_field_name = "FP_ING_REDIRECT_PORT_FLOODBLOCK",
    },
};

static const bcm_field_control_map_info_t bcm_field_control_OverrideStageLookupPhb_map[] = {
    {
        .lt_name       = "FP_CONTROL",
        .lt_field_name = "FP_VLAN_OVERRIDE_PHB",
    },
};

static const bcm_field_control_map_info_t bcm_field_control_RedirectNextHopExcludeSrcPort_map[] = {
    {
        .lt_name       = "FP_CONTROL",
        .lt_field_name = "FP_ING_REDIRECT_NHI_EXCLUDE_SRCPORT",
    },
};

static const bcm_field_control_map_info_t bcm_field_control_ArpAsIp_map[] = {
    {
        .lt_name       = "FP_CONTROL",
        .lt_field_name = "FP_ING_ARP_AS_IP",
    },
};

static const bcm_field_control_map_info_t bcm_field_control_RarpAsIp_map[] = {
    {
        .lt_name       = "FP_CONTROL",
        .lt_field_name = "FP_ING_RARP_AS_IP",
    },
};

static const bcm_field_control_map_info_t bcm_field_control_RedirectExcludeSrcPort_map[] = {
    {
        .lt_name       = "FP_CONTROL",
        .lt_field_name = "FP_ING_REDIRECT_EXCLUDE_SRCPORT",
    },
    {
        .lt_name       = "FP_CONTROL",
        .lt_field_name = "FP_ING_REDIRECT_EXCLUDE_HGSRCPORT",
    },
};

const bcm_field_control_map_t bcm56990_a0_field_control_data[] = {
   {
        .control = bcmFieldControlRedirectIngressVlanCheck,
        .num_maps = 1,
        .map = bcm_field_control_RedirectIngressVlanCheck_map,
    },
    {
        .control = bcmFieldControlRedirectExcludeSrcPort,
        .num_maps = 2,
        .map = bcm_field_control_RedirectExcludeSrcPort_map,
    },
    {
        .control = bcmFieldControlRedirectPortFloodBlock,
        .num_maps = 1,
        .map = bcm_field_control_RedirectPortFloodBlock_map,
    },
    {
        .control = bcmFieldControlRedirectVlanFloodBlock,
        .num_maps = 1,
        .map = bcm_field_control_RedirectVlanFloodBlock_map,
    },
    {
        .control = bcmFieldControlArpAsIp,
        .num_maps = 1,
        .map = bcm_field_control_ArpAsIp_map,
    },
    {
        .control = bcmFieldControlRarpAsIp,
        .num_maps = 1,
        .map = bcm_field_control_RarpAsIp_map,
    },
    {
        .control = bcmFieldControlRedirectNonUcastEtherTrunkResolve,
        .num_maps = 1,
        .map = bcm_field_control_RedirectNonUcastEtherTrunkResolve_map,
    },
    {
        .control = bcmFieldControlRedirectNextHopExcludeSrcPort,
        .num_maps = 1,
        .map = bcm_field_control_RedirectNextHopExcludeSrcPort_map,
    },
    {
        .control = bcmFieldControlEcmpHashOffset,
        .num_maps = 1,
        .map = bcm_field_control_EcmpHashOffset_map,
    },
    {
        .control = bcmFieldControlEcmpHashUseCrc,
        .num_maps = 1,
        .map = bcm_field_control_EcmpHashUseCrc_map,
    },
    {
        .control = bcmFieldControlEcmpHashUseUpper5Bits,
        .num_maps = 1,
        .map = bcm_field_control_EcmpHashUseUpper5Bits_map,
    },
    {
        .control = bcmFieldControlEcmpHashEnable,
        .num_maps = 1,
        .map = bcm_field_control_EcmpHashEnable_map,
    },
    {
        .control = bcmFieldControlOverrideStageLookupPhb,
        .num_maps = 1,
        .map = bcm_field_control_OverrideStageLookupPhb_map,
    },
    {
        .control = bcmFieldControlRedirectExcludeEtherSrcPort,
        .num_maps = 1,
        .map = bcm_field_control_RedirectExcludeEtherSrcPort_map,
    },
    {
        .control = bcmFieldControlRedirectExcludeHiGigSrcPort,
        .num_maps = 1,
        .map = bcm_field_control_RedirectExcludeHiGigSrcPort_map,
    },
    {
        .control = bcmFieldControlDlbMonitorIngressRandomSeed,
        .num_maps = 1,
        .map = bcm_field_control_DlbMonitorIngressRandomSeed_map,
    },
};

/******************************************************************************
 * Private functions
 */

static int
bcm56990_a0_ltsw_field_control_info_init(
    int unit,
    bcmint_field_control_info_t *control_info)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(control_info, SHR_E_PARAM);

    /* Assign Field Control DB */
    control_info->control_lt_map_db = bcm56990_a0_field_control_data;

    /* Initialise Field Controls db counts */
    control_info->control_db_count = (sizeof(bcm56990_a0_field_control_data)) /
                                                    (sizeof(bcm_field_control_map_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_control_lt_info_init(unit, control_info));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_field_ingress_profile_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_field_ingress_profile_init(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_field_ingress_profile_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_field_ingress_profile_deinit(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_field_lt_qual_name_update(
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

    if ((group_map == true) &&
        ((stage_info->stage_id == bcmiFieldStageIngress) ||
         (stage_info->stage_id == bcmiFieldStageExactMatch)) &&
        (sal_strncmp(lt_map_ref->lt_field_name, "QUAL_UDF_CHUNKS", 15) == 0)) {
        BCMINT_FIELD_MEM_ALLOC
            (*lt_field_name, strlen(lt_map_ref->lt_field_name) + strlen("_BITMAP"),
             "grp_lt_qual_name");
        sal_strcpy(*lt_field_name, lt_map_ref->lt_field_name);
        sal_strcat(*lt_field_name, "_BITMAP");
    } else {
        BCMINT_FIELD_MEM_ALLOC
            (*lt_field_name, strlen(lt_map_ref->lt_field_name) + 1,
             "grp_lt_qual_name");
        sal_strcpy(*lt_field_name, lt_map_ref->lt_field_name);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_field_src_class_enum_to_lt_symbol_get(
        int unit,
        bcm_field_src_class_mode_t mode,
        char **key_fid)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key_fid, SHR_E_PARAM);

    switch (mode) {
        case bcmFieldSrcClassModeDefault:
            *key_fid = "LEGACY";
            break;
        case bcmFieldSrcClassModeSDN:
            *key_fid = "SDN";
            break;
        case bcmFieldSrcClassModeBalanced:
            *key_fid = "BALANCED";
            break;
        case bcmFieldSrcClassModeOverlayNetworks:
            *key_fid = "OVERLAY";
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

/* This api will return value/mask to be set against a macro
 * Values directly depend upon chipset.
 */
static int
bcm56990_a0_ltsw_field_qualify_macro_values_get(int unit,
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
        case bcmFieldQualifyIpInfo:
            qual_info_values->num = 1;
            qual_info_values->macro_flags[0] = BCM_FIELD_IP_CHECKSUM_OK;
            break;
        case bcmFieldQualifyVlanFormat:
            qual_info_values->num = 2;
            qual_info_values->macro_flags[0] = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
            qual_info_values->macro_flags[1] = BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
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
static int
bcm56990_a0_ltsw_field_src_class_lt_symbol_to_enum_get(
        int unit,
        char *key_fid,
        bcm_field_src_class_mode_t *mode)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key_fid, SHR_E_PARAM);
    SHR_NULL_CHECK(mode, SHR_E_PARAM);


    if (0 == sal_strcmp(key_fid, "LEGACY")) {
        *mode = bcmFieldSrcClassModeDefault;
    } else if (0 == sal_strcmp(key_fid, "SDN")) {
        *mode = bcmFieldSrcClassModeSDN;
    } else if (0 == sal_strcmp(key_fid, "BALANCED")) {
        *mode = bcmFieldSrcClassModeBalanced;
    } else if (0 == sal_strcmp(key_fid, "OVERLAY")) {
        *mode = bcmFieldSrcClassModeOverlayNetworks;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_field_src_class_hw_fields_encode_set(
        int unit,
        bcm_field_src_class_mode_t mode,
        bcm_field_src_class_t data,
        bcm_field_src_class_t mask,
        uint32 *hw_data,
        uint32 *hw_mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(hw_data, SHR_E_PARAM);
    SHR_NULL_CHECK(hw_mask, SHR_E_PARAM);

    switch(mode) {
        case bcmFieldSrcClassModeDefault:
            /* IFP_SOURCE_CLASS_FORMAT_0 */
            *hw_data = (uint32)((uint32)((data.intf_class_l2 & 0xF) << 28))
                | ((data.src_class_field & 0x3) << 26)
                | ((data.dst_class_field & 0x3FF) << 16)
                | ((data.udf_class & 0xFF) << 8)
                | (data.intf_class_port & 0xFF);

            *hw_mask = (uint32)((uint32)((mask.intf_class_l2 & 0xF) << 28))
                | ((mask.src_class_field & 0x3) << 26)
                | ((mask.dst_class_field & 0x3FF) << 16)
                | ((mask.udf_class & 0xFF) << 8)
                | (mask.intf_class_port & 0xFF);
            break;
        case bcmFieldSrcClassModeSDN:
            /* IFP_SOURCE_CLASS_FORMAT_1 */
            *hw_data = (uint32)((uint32)((data.intf_class_l2 & 0xF) << 24))
                | ((data.src_class_field & 0x3) << 22)
                | ((data.dst_class_field & 0x3FF) << 12)
                | ((data.udf_class & 0xF) << 8)
                | ((data.intf_class_port & 0xF) << 4)
                | (data.intf_class_l3 & 0xF);

            *hw_mask = (uint32)((uint32)((mask.intf_class_l2 & 0xF) << 24))
                | ((mask.src_class_field & 0x3) << 22)
                | ((mask.dst_class_field & 0x3FF) << 12)
                | ((mask.udf_class & 0xF) << 8)
                | ((mask.intf_class_port & 0xF) << 4)
                | (mask.intf_class_l3 & 0xF);
            break;
        case bcmFieldSrcClassModeBalanced:
            /* IFP_SOURCE_CLASS_FORMAT_2 */
            *hw_data = (uint32)((uint32)((data.intf_class_l2 & 0xF) << 24))
                | ((data.dst_class_field & 0xFF) << 16)
                | ((data.udf_class & 0xF) << 12)
                | ((data.intf_class_port & 0xFF) << 4)
                | (data.intf_class_l3 & 0xF);
            *hw_mask = (uint32)((uint32)((mask.intf_class_l2 & 0xF) << 24))
                | ((mask.dst_class_field & 0xFF) << 16)
                | ((mask.udf_class & 0xF) << 12)
                | ((mask.intf_class_port & 0xFF) << 4)
                | (mask.intf_class_l3 & 0xF);
            break;
        case bcmFieldSrcClassModeOverlayNetworks:
            /* IFP_SOURCE_CLASS_FORMAT_3 */
            *hw_data = (uint32)((uint32)((data.intf_class_l2 & 0xFF) << 16))
                | ((data.dst_class_field & 0xF) << 12)
                | ((data.intf_class_port & 0xF) << 8)
                | (data.intf_class_l3 & 0xFF);

            *hw_mask = (uint32)((uint32)((mask.intf_class_l2 & 0xFF) << 16))
                | ((data.dst_class_field & 0xF) << 12)
                | ((data.intf_class_port & 0xF) << 8)
                | (data.intf_class_l3 & 0xFF);
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}


static int
bcm56990_a0_ltsw_field_src_class_hw_fields_decode_get(
        int unit,
        bcm_field_src_class_mode_t mode,
        uint32 hw_data,
        uint32 hw_mask,
        bcm_field_src_class_t *data,
        bcm_field_src_class_t *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    switch(mode) {
        case bcmFieldSrcClassModeDefault:
            /* IFP_SOURCE_CLASS_FORMAT_0 */
            data->intf_class_l2     = (hw_data >> 28) & 0xF;
            data->src_class_field   = (hw_data >> 26) & 0x3;
            data->dst_class_field   = (hw_data >> 16) & 0x3FF;
            data->udf_class         = (hw_data >> 8) & 0xFF;
            data->intf_class_port   = hw_data & 0xFF;

            mask->intf_class_l2     = (hw_data >> 28) & 0xF;
            mask->src_class_field   = (hw_data >> 26) & 0x3;
            mask->dst_class_field   = (hw_data >> 16) & 0x3FF;
            mask->udf_class         = (hw_data >> 8) & 0xFF;
            mask->intf_class_port   = hw_data & 0xFF;
            break;
        case bcmFieldSrcClassModeSDN:
            /* IFP_SOURCE_CLASS_FORMAT_1 */
            data->intf_class_l2     = (hw_data >> 24) & 0xF;
            data->src_class_field   = (hw_data >> 22) & 0x3;
            data->dst_class_field   = (hw_data >> 12) & 0x3FF;
            data->udf_class         = (hw_data >> 8) & 0xF;
            data->intf_class_port   = (hw_data >> 4) & 0xF;
            data->intf_class_l3     = hw_data & 0xF;

            mask->intf_class_l2     = (hw_data >> 24) & 0xF;
            mask->src_class_field   = (hw_data >> 22) & 0x3;
            mask->dst_class_field   = (hw_data >> 12) & 0x3FF;
            mask->udf_class         = (hw_data >> 8) & 0xF;
            mask->intf_class_port   = (hw_data >> 4) & 0xF;
            mask->intf_class_l3     = hw_data & 0xF;
            break;
        case bcmFieldSrcClassModeBalanced:
            /* IFP_SOURCE_CLASS_FORMAT_2 */
            data->intf_class_l2     = (hw_data >> 24) & 0xF;
            data->dst_class_field   = (hw_data >> 16) & 0xFF;
            data->udf_class         = (hw_data >> 12) & 0xF;
            data->intf_class_port   = (hw_data >> 4) & 0xFF;
            data->intf_class_l3     = hw_data & 0xF;

            mask->intf_class_l2     = (hw_data >> 24) & 0xF;
            mask->dst_class_field   = (hw_data >> 16) & 0xFF;
            mask->udf_class         = (hw_data >> 12) & 0xF;
            mask->intf_class_port   = (hw_data >> 4) & 0xFF;
            mask->intf_class_l3     = hw_data & 0xF;
            break;
        case bcmFieldSrcClassModeOverlayNetworks:
            data->intf_class_l2     = (hw_data >> 16) & 0xFF;
            data->dst_class_field   = (hw_data >> 12) & 0xF;
            data->intf_class_port   = (hw_data >> 8) & 0xF;
            data->intf_class_l3     = hw_data & 0xFF;

            mask->intf_class_l2     = (hw_data >> 16) & 0xFF;
            mask->dst_class_field   = (hw_data >> 12) & 0xF;
            mask->intf_class_port   = (hw_data >> 8) & 0xF;
            mask->intf_class_l3     = hw_data & 0xFF;
            break;
        default:
            return (BCM_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_field_egr_group_qset_validate(
    int unit,
    bcm_field_group_t group_id,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_qset_t new_qset,
    bcm_field_group_mode_type_t *mode_type,
    bcm_field_group_packet_type_t *packet_type)
{
    int key_mode = 0;
    bcmlt_entry_handle_t grp_template= BCMLT_INVALID_HDL;
    const char* old_mode = NULL;
    const char* old_port_pkt_type = NULL;
    char* mode = NULL;
    char* port_pkt_type = NULL;
    SHR_FUNC_ENTER(unit);

    /* Group entry handle allocate for group template. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(unit,
                              stage_info->tbls_info->group_sid,
                              &grp_template ));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(grp_template,
                               stage_info->tbls_info->group_key_fid,
                               group_id));

    /* Search and retreive group LT entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, grp_template, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(grp_template, "MODE",
                                      &old_mode));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(grp_template, "PORT_PKT_TYPE",
                                      &old_port_pkt_type));
    for (key_mode = 0; key_mode < BCMINT_FIELD_EFP_KEYS; key_mode++) {
        if (TRUE == bcmint_field_find_is_subset_qset(
                    &new_qset,
                    &(stage_info->efp_key_qual_map[key_mode].key_qset))) {
            mode = stage_info->efp_key_qual_map[key_mode].grp_mode;
            port_pkt_type =
                stage_info->efp_key_qual_map[key_mode].port_pkt_type;

            if (0 == strcmp(old_port_pkt_type, port_pkt_type) &&
                (0 == strcmp(old_mode, mode))) {
                break;
            }
        }
    }

    /* Check whether it reached Max Key combinations */
    if (key_mode >= BCMINT_FIELD_EFP_KEYS) {
        /* Verify requested QSET is supported by the stage. */
        if (FALSE == bcmint_field_qset_is_subset(
                    unit, &new_qset, stage_info, 0, NULL)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        } else {
            /* Qset combination is not supported. */
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }
    }

    *mode_type = stage_info->efp_key_qual_map[key_mode].mode_type;
    *packet_type = stage_info->efp_key_qual_map[key_mode].packet_type;

exit:
    if (BCMLT_INVALID_HDL != grp_template) {
        (void) bcmlt_entry_free(grp_template);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_field_egress_group_mode_set(
        int unit,
        bcm_field_group_config_t *group_config,
        bcmint_field_stage_info_t *stage_info,
        bcmlt_entry_handle_t grp_template,
        bool *mode_auto,
        char **group_mode)
{
    int key_mode = 0;

    SHR_FUNC_ENTER(unit);

    for (key_mode = 0; key_mode < BCMINT_FIELD_EFP_KEYS; key_mode++) {
        if (TRUE == bcmint_field_find_is_subset_qset(
                    &(group_config->qset),
                    &(stage_info->efp_key_qual_map[key_mode].key_qset))) {
            if (group_config->mode == bcmFieldGroupModeAuto) {
                if (group_config->mode_type == bcmFieldGroupModeTypeAuto) {
                    if (group_config->packet_type
                        == stage_info->efp_key_qual_map[key_mode].packet_type) {
                        break;
                    }
                } else {
                    if ((group_config->packet_type
                        == stage_info->efp_key_qual_map[key_mode].packet_type) &&
                        (group_config->mode_type
                        == stage_info->efp_key_qual_map[key_mode].mode_type)) {
                        break;
                    }
                }
            } else  if (group_config->mode == bcmFieldGroupModeSingle) {
                if ((group_config->packet_type
                    == stage_info->efp_key_qual_map[key_mode].packet_type) &&
                    (stage_info->efp_key_qual_map[key_mode].mode_type
                    >= bcmFieldGroupModeTypeL2Single) &&
                    (stage_info->efp_key_qual_map[key_mode].mode_type
                    <= bcmFieldGroupModeTypeL3SingleAny)) {
                    if (group_config->mode_type == bcmFieldGroupModeTypeAuto) {
                        break;
                    } else if (group_config->mode_type
                        == stage_info->efp_key_qual_map[key_mode].mode_type) {
                        break;
                    }
                }
            } else  if (group_config->mode == bcmFieldGroupModeDouble) {
                if ((group_config->packet_type
                    == stage_info->efp_key_qual_map[key_mode].packet_type) &&
                    (stage_info->efp_key_qual_map[key_mode].mode_type
                    >= bcmFieldGroupModeTypeL3Double) &&
                    (stage_info->efp_key_qual_map[key_mode].mode_type
                    <= bcmFieldGroupModeTypeL3DoubleAlternate)) {
                    if (group_config->mode_type == bcmFieldGroupModeTypeAuto) {
                        break;
                    } else if (group_config->mode_type
                        == stage_info->efp_key_qual_map[key_mode].mode_type) {
                        break;
                    }
                }
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            }
        }
    }

    /* Check whether it reached Max Key combinations */
    if (key_mode >= BCMINT_FIELD_EFP_KEYS) {
        /* Verify requested QSET is supported by the stage. */
        if (FALSE == bcmint_field_qset_is_subset(
                    unit, &group_config->qset, stage_info, 0, NULL)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        } else {
            /* Qset combination is not supported. */
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(grp_template, "MODE",
                 stage_info->efp_key_qual_map[key_mode].grp_mode));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(grp_template, "PORT_PKT_TYPE",
                 stage_info->efp_key_qual_map[key_mode].port_pkt_type));
    /* Store Group mode in S/W */
    *group_mode = stage_info->efp_key_qual_map[key_mode].grp_mode;

exit:
    SHR_FUNC_EXIT();

}

static int
bcm56990_a0_ltsw_field_group_mode_set(
        int unit,
        bcm_field_group_config_t *group_config,
        bcmint_field_stage_info_t *stage_info,
        bcmlt_entry_handle_t grp_template,
        bool *mode_auto,
        char **group_mode)
{
    SHR_FUNC_ENTER(unit);

    if (bcmiFieldStageEgress == stage_info->stage_id) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56990_a0_ltsw_field_egress_group_mode_set(unit,
                                                          group_config,
                                                          stage_info,
                                                          grp_template,
                                                          mode_auto,
                                                          group_mode));
    } else {
        if (group_config->flags & BCM_FIELD_GROUP_CREATE_WITH_MODE) {
            if (group_config->mode == bcmFieldGroupModeAuto) {
                *mode_auto = 1;
                *group_mode = "MODE_AUTO";
            } else {
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
        /* Update Group Auto mode */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(grp_template, "MODE_AUTO", *mode_auto));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function - This api is called from qualifier_set call where user
 * passes logical port in bcm_pbmp_t structure. This pbmp has to
 * be coverted into equivalent ipbm index which need to be programmed
 * in rule template. This ipbm index comes from ING_SYSTEM_PORT_TABLE
 * which gets passed to FP in pipeline. */
static int
bcm56990_a0_ltsw_field_pbmp_index_get(int unit,
                                      bcm_field_group_t group,
                                      bcmint_field_stage_info_t *stage_info,
                                      bcm_pbmp_t *in_pbmp,
                                      bcm_pbmp_t *in_pbmp_mask,
                                      uint8_t *out_pbmp_idx,
                                      uint8_t *out_pbmp_idx_mask)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(in_pbmp, SHR_E_INTERNAL);
    SHR_NULL_CHECK(in_pbmp_mask, SHR_E_INTERNAL);
    SHR_NULL_CHECK(out_pbmp_idx, SHR_E_INTERNAL);
    SHR_NULL_CHECK(out_pbmp_idx_mask, SHR_E_INTERNAL);

    sal_memcpy(out_pbmp_idx, in_pbmp, sizeof(bcm_pbmp_t));
    sal_memcpy(out_pbmp_idx_mask, in_pbmp_mask, sizeof(bcm_pbmp_t));
exit:
    SHR_FUNC_EXIT();
}

/* Function - This api will take group from group config structure and return
 * pbmp set for a group. It do this by retreiving PIPE to which group belongs
 * to and then use port_config_get to retrieve ports in that pipe.
 * This api is chip specific as ports and FP pipe are specific to chip. */
static int
bcm56990_a0_ltsw_field_group_pbmp_get(int unit,
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


/* Function Notes -
 * This api is called during group create to get pipe to which
 * group belongs to based on ports filled in group config structure.
 * This function is stage specific because pipe to which
 * ports belong to depends upon port_config_get call and actual
 * pipe to which it belongs to which varies from chip to chip.
 * In TH4 case, port_config_get calls returns port in 8 pipes
 * where as from FP context there are 4 pipes. 2 pipes from device
 * are merged into one for TH4. Hence
 * Device Pipe 0 & 1 - FP Pipe 0
 * Device Pipe 2 & 3 - FP Pipe 1
 * Device Pipe 4 & 5 - FP Pipe 2
 * Device Pipe 6 & 7 - FP Pipe 3
 * In Group config structure we expect qset and ports to be passed.
 */
static int
bcm56990_a0_ltsw_field_group_pipe_get(int unit,
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

/* Function - This api is called from qualifier_get call where ibmp_index
 * is read from group entry LT table, based on pipe where index belongs to
 * we return logical port to user. */
static int
bcm56990_a0_ltsw_field_pbmp_get(int unit,
                                bcm_field_group_t group,
                                bcmint_field_stage_info_t *stage_info,
                                uint8_t *out_pbmp_idx,
                                uint8_t *out_pbmp_idx_mask,
                                bcm_pbmp_t *in_pbmp,
                                bcm_pbmp_t *in_pbmp_mask)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(in_pbmp, SHR_E_INTERNAL);
    SHR_NULL_CHECK(in_pbmp_mask, SHR_E_INTERNAL);
    SHR_NULL_CHECK(out_pbmp_idx, SHR_E_INTERNAL);
    SHR_NULL_CHECK(out_pbmp_idx_mask, SHR_E_INTERNAL);

    sal_memcpy(in_pbmp, out_pbmp_idx, sizeof(bcm_pbmp_t));
    sal_memcpy(in_pbmp_mask, out_pbmp_idx_mask, sizeof(bcm_pbmp_t));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_field_qual_gport_validate(int unit,
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
            (bcmi_ltsw_port_to_sys_port(unit, gport, port));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, gport, port));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_field_destination_init(int unit)
{
    return SHR_E_NONE;
}

static int
bcm56990_a0_ltsw_field_destination_deinit(int unit)
{
    return SHR_E_NONE;
}

static int
bcm56990_a0_ltsw_field_internal_entry_enable_get(int unit,
        bcmint_field_stage_info_t *stage_info,
        bool *enable)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, SHR_E_PARAM);
    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    *enable = FALSE;

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_ltsw_field_policer_attach(int unit,
                                      bcmint_field_stage_info_t *stage_info,
                                      bcmi_field_ha_entry_oper_t *entry_oper,
                                      bcm_policer_t policer_id)
{
    uint32_t pid = 0;
    bcm_field_entry_t entry_id = 0;
    bcmlt_entry_handle_t entry_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);

    pid = (policer_id & BCM_FIELD_ID_MASK);

    entry_id = entry_oper->entry_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &entry_template,
                                      BCMLT_OPCODE_UPDATE,
                                      BCMLT_PRIORITY_NORMAL,
                                      stage_info->tbls_info->entry_sid,
                                      stage_info->tbls_info->entry_key_fid,
                                      0, (entry_id & BCM_FIELD_ID_MASK), NULL,
                                      stage_info->tbls_info->meter_key_fid,
                                      pid,
                                      0, 0,
                                      1));
exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_ltsw_field_policer_detach(int unit,
                                      bcm_field_entry_t entry_id)
{
    uint32_t pid = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmlt_entry_handle_t entry_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry_id,
                                           &stage_info));

    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);

    pid = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &entry_template,
                                      BCMLT_OPCODE_UPDATE,
                                      BCMLT_PRIORITY_NORMAL,
                                      stage_info->tbls_info->entry_sid,
                                      stage_info->tbls_info->entry_key_fid,
                                      0, (entry_id & BCM_FIELD_ID_MASK), NULL,
                                      stage_info->tbls_info->meter_key_fid,
                                      pid,
                                      0, 0,
                                      1));
exit:
    SHR_FUNC_EXIT();

}

static int
bcm56990_a0_ltsw_field_qual_mask_lt_field_get(
        int unit,
        char *qual_field,
        char **qual_mask_field)
{
    SHR_FUNC_ENTER(unit);

    if ((0 == sal_strcmp("QUAL_SRC_IP6_LOWER", qual_field))
            || (0 == sal_strcmp("QUAL_SRC_IP6_UPPER", qual_field))
            || (0 == sal_strcmp("QUAL_DST_IP6_LOWER", qual_field))
            || (0 == sal_strcmp("QUAL_DST_IP6_UPPER", qual_field))) {
        sal_strncpy(*qual_mask_field, qual_field, strlen("QUAL_xxx_IP6"));
        sal_strcat(*qual_mask_field, "_MASK");
        sal_strcat(*qual_mask_field, (qual_field + sal_strlen(*qual_mask_field) - sal_strlen("_MASK")));
    } else if ((0 == sal_strcmp("QUAL_INNER_SRC_IP6_UPPER", qual_field))
            || (0 == sal_strcmp("QUAL_INNER_SRC_IP6_LOWER", qual_field))
            || (0 == sal_strcmp("QUAL_INNER_DST_IP6_UPPER", qual_field))
            || (0 == sal_strcmp("QUAL_INNER_DST_IP6_LOWER", qual_field))) {
        sal_strncpy(*qual_mask_field, qual_field, strlen("QUAL_INNER_xxx_IP6"));
        sal_strcat(*qual_mask_field, "_MASK");
        sal_strcat(*qual_mask_field, (qual_field + sal_strlen(*qual_mask_field) - sal_strlen("_MASK")));
    } else {
        sal_strcpy(*qual_mask_field, qual_field);
        sal_strcat(*qual_mask_field,"_MASK");
    }

    SHR_FUNC_EXIT();
}

#if 0
static int
bcm56990_a0_ltsw_field_action_profile_index_get(int unit,
                                        bcm_field_entry_t entry,
                                        bcm_field_action_t action,
                                        uint16_t *act_prof_idx)
{
    return SHR_E_NONE;
}
#endif

STATIC int
bcm56990_a0_ltsw_field_qualify_packetres_set(int unit,
                                             bcm_field_entry_t entry,
                                             uint32_t data,
                                             uint32_t mask)
{
    bcm_qual_field_t qual_info;
    char *lt_field_name = NULL;
    uint8_t i = 0;
    bool valid = 0;

    SHR_FUNC_ENTER(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Translate data #defines to hardware encodings */
    for (i = 0; i < COUNTOF(pkt_res_xlate_tbl); ++i) {
        if (data == pkt_res_xlate_tbl[i].api) {
            data = pkt_res_xlate_tbl[i].hw_data;
            mask = pkt_res_xlate_tbl[i].hw_mask;
            valid = 1;
            break;
        }
    }
    if (!valid)  {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    qual_info.sdk6_qual_enum = bcmFieldQualifyPacketRes;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (uint32_t  *)(&data);
    qual_info.qual_mask = (uint32_t  *)(&mask);

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

exit:
    BCMINT_FIELD_MEM_FREE(lt_field_name);
    SHR_FUNC_EXIT();
}

STATIC int
bcm56990_a0_ltsw_field_qualify_packetres_get(int unit,
                                             bcm_field_entry_t entry,
                                             uint32_t *data,
                                             uint32_t *mask)
{
    uint8_t i = 0;
    bool valid = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyPacketRes;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    /* Translate data #defines from hardware encodings */
    for (i = 0; i < COUNTOF(pkt_res_xlate_tbl); ++i) {
        if (*data  == pkt_res_xlate_tbl[i].hw_data ) {
            if (pkt_res_xlate_tbl[i].hw_mask == 0x3f) {
                if ( (*mask == pkt_res_xlate_tbl[i].hw_data) ||
                        (*mask == 0x3f) ) {
                    *data  = pkt_res_xlate_tbl[i].api;
                    if (*mask  == pkt_res_xlate_tbl[i].hw_data) {
                        *mask = pkt_res_xlate_tbl[i].api;
                    } else {
                        *mask  = BCM_FIELD_EXACT_MATCH_MASK;
                    }
                    valid = 1;
                    break;
                }
            } else {
                if ( (*mask != pkt_res_xlate_tbl[i].hw_data) &&
                        (*mask != 0x3f) ) {
                    *data = pkt_res_xlate_tbl[i].api;
                    *mask  = BCM_FIELD_EXACT_MATCH_MASK;
                    valid = 1;
                    break;
                }
            }
        }
    }
    if (!valid)  {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }


exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56990_a0_ltsw_field_qualify_mhopcode_set(int unit,
                                bcm_field_entry_t entry,
                                uint8 data, uint8 mask)
{
    uint8 data_val = 0, mask_val = -1;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    switch(data) {
        case BCM_FIELD_MHOPCODE_CONTROL:
            data_val = 0;
            break;
        case BCM_FIELD_MHOPCODE_UNICAST:
            data_val = 1;
            break;
        case BCM_FIELD_MHOPCODE_BROADCAST_DLF:
            data_val = 2;
            break;
        case BCM_FIELD_MHOPCODE_MULTICAST:
            data_val = 3;
            break;
        case BCM_FIELD_MHOPCODE_IPMULTICAST:
            data_val = 4;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    mask_val = mask;

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
STATIC int
bcm56990_a0_ltsw_field_qualify_mhopcode_get(int unit,
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
        case 0:
            *data = BCM_FIELD_MHOPCODE_CONTROL;
            break;
        case 1:
            *data = BCM_FIELD_MHOPCODE_UNICAST;
            break;
        case 2:
            *data = BCM_FIELD_MHOPCODE_BROADCAST_DLF;
            break;
        case 3:
            *data = BCM_FIELD_MHOPCODE_MULTICAST;
            break;
        case 4:
            *data = BCM_FIELD_MHOPCODE_IPMULTICAST;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    *mask = mask_val;

exit:
    SHR_FUNC_EXIT();
}


STATIC int
bcm56990_a0_ltsw_field_action_param_symbol_set(
                 int unit,
                 bcm_field_action_t action,
                 bcm_field_action_params_t *params,
                 char *param_name)
{
    SHR_FUNC_ENTER(unit);
    switch (action) {
        case bcmFieldActionEgressClassSelect:
            switch (params->param0) {
                case BCM_FIELD_EGRESS_CLASS_SELECT_PORT:
                    sal_strcpy(param_name, "PORT_SYSTEM");
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_COMPRESSION_ID_A:
                    sal_strcpy(param_name, "COMPRESSION_ID_A");
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_L3_IIF:
                    sal_strcpy(param_name, "L3_IIF");
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_FIELD_SRC:
                    sal_strcpy(param_name, "FP_VLAN_CLASS_1");
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_FIELD_DST:
                    sal_strcpy(param_name, "FP_VLAN_CLASS_0");
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_L2_SRC:
                    sal_strcpy(param_name, "L2_SRC");
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_L2_DST:
                    sal_strcpy(param_name, "L2_DST");
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_L3_SRC:
                    sal_strcpy(param_name, "L3_SRC");
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_L3_DST:
                    sal_strcpy(param_name, "L3_DST");
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_VLAN:
                    sal_strcpy(param_name, "VLAN");
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_VRF:
                    sal_strcpy(param_name, "VRF");
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_COMPRESSION_ID_B:
                    sal_strcpy(param_name, "COMPRESSION_ID_B");
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_EM_CLASS_ID:
                    sal_strcpy(param_name, "EM_CLASS_ID");
                    break;
                case BCM_FIELD_EGRESS_CLASS_SELECT_NEW:
                    sal_strcpy(param_name, "FP_ING");
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;
        case bcmFieldActionL3IngressStrengthSet:
            switch (params->param0) {
                case BCM_FIELD_L3INGRESS_STRENGTH_HIGH:
                    sal_strcpy(param_name, "PRIORITY_STRENGTH_HIGH");
                    break;
                case BCM_FIELD_L3INGRESS_STRENGTH_LOW:
                    sal_strcpy(param_name, "PRIORITY_STRENGTH_LOW");
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;
        case bcmFieldActionDropPrecedence:
        case bcmFieldActionGpDropPrecedence:
        case bcmFieldActionYpDropPrecedence:
        case bcmFieldActionRpDropPrecedence:
            switch (params->param0) {
                case BCM_FIELD_COLOR_GREEN:
                    sal_strcpy(param_name, "GREEN");
                    break;
                case BCM_FIELD_COLOR_YELLOW:
                    sal_strcpy(param_name, "YELLOW");
                    break;
                case BCM_FIELD_COLOR_RED:
                    sal_strcpy(param_name, "RED");
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();

}


STATIC int
bcm56990_a0_ltsw_field_action_param_symbol_get(
                 int unit,
                 bcm_field_action_t action,
                 bcm_field_action_params_t *params,
                 char *param_name)
{
    SHR_FUNC_ENTER(unit);

    switch (action) {
        case bcmFieldActionEgressClassSelect:
            if (0 == sal_strcmp(param_name, "PORT_SYSTEM")) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_PORT;
            } else if (0 == sal_strcmp(param_name, "COMPRESSION_ID_A")) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_COMPRESSION_ID_A;
            } else if (0 == sal_strcmp(param_name, "L3_IIF")) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_L3_IIF;
            } else if (0 == sal_strcmp(param_name, "FP_VLAN_CLASS_1")) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_FIELD_SRC;
            } else if (0 == sal_strcmp(param_name, "FP_VLAN_CLASS_0")) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_FIELD_DST;
            } else if (0 == sal_strcmp(param_name, "L2_SRC")) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_L2_SRC;
            } else if (0 == sal_strcmp(param_name, "L2_DST")) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_L2_DST;
            } else if (0 == sal_strcmp(param_name, "L3_SRC")) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_L3_SRC;
            } else if (0 == sal_strcmp(param_name, "L3_DST")) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_L3_DST;
            } else if (0 == sal_strcmp(param_name, "VLAN")) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_VLAN;
            } else if (0 == sal_strcmp(param_name, "VRF")) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_VRF;
            } else if (0 == sal_strcmp(param_name, "COMPRESSION_ID_B")) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_COMPRESSION_ID_B;
            } else if (0 == sal_strcmp(param_name, "EM_CLASS_ID")) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_EM_CLASS_ID;
            } else if (0 == sal_strcmp(param_name, "FP_ING")) {
                params->param0 = BCM_FIELD_EGRESS_CLASS_SELECT_NEW;
            } else {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            break;
        case bcmFieldActionL3IngressStrengthSet:
            if (0 == sal_strcmp(param_name, "PRIORITY_STRENGTH_LOW")) {
                params->param0 = BCM_FIELD_L3INGRESS_STRENGTH_LOW;
            } else if (0 == sal_strcmp(param_name, "PRIORITY_STRENGTH_HIGH")) {
                params->param0 = BCM_FIELD_L3INGRESS_STRENGTH_HIGH;
            } else if (0 == sal_strcmp(param_name, "PRIORITY_STRENGTH_HIGHEST")) {
                /* Default value */
                params->param0 = 0;
            } else {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            break;
        case bcmFieldActionDropPrecedence:
        case bcmFieldActionGpDropPrecedence:
        case bcmFieldActionYpDropPrecedence:
        case bcmFieldActionRpDropPrecedence:
            if (0 == sal_strcmp(param_name, "GREEN")) {
                params->param0 = BCM_FIELD_COLOR_GREEN;
            } else if (0 == sal_strcmp(param_name, "YELLOW")) {
                params->param0 = BCM_FIELD_COLOR_YELLOW;
            } else if (0 == sal_strcmp(param_name, "RED")) {
                params->param0 = BCM_FIELD_COLOR_RED;
            } else {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();

}

static int
bcm56990_a0_ltsw_field_aacl_class_id_get(int unit,
                        bcm_field_qualify_t qid,
                        bcmint_field_stage_info_t *stage_info,
                        bcmi_field_ha_entry_oper_t *entry_oper,
                        int *class_id,
                        bcm_field_qualify_t *pair_qual)
{
    SHR_FUNC_ENTER(unit);
    SHR_ERR_EXIT(BCM_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_field_action_stat_group_lt_field_validate(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_hintid_t hintid,
    const char *field_name,
    bool *is_valid)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);
    *is_valid = true;

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_ltsw_field_group_mode_get(
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

    if (stage_info->flags & BCMINT_FIELD_STAGE_KEY_TYPE_FIXED) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_get(grp_template,
                                          "MODE",
                                          &group_mode));
        if ((0 == strcmp(group_mode,"L2_SINGLE_WIDE")) ||
                (0 == strcmp(group_mode,"L3_SINGLE_WIDE")) ||
                (0 == strcmp(group_mode, "L3_ANY_SINGLE_WIDE")))
        {
            *mode = bcmFieldGroupModeSingle;
        } else if ((0 == strcmp(group_mode, "L3_DOUBLE_WIDE")) ||
                (0 == strcmp(group_mode,"L3_ALT_DOUBLE_WIDE")) ||
                (0 == strcmp(group_mode,"L3_SINGLE_WIDE"))) {
            *mode = bcmFieldGroupModeDouble;
        } else {
            LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit, "Error: %s Not a valid"
                                " FP group mode. \n"), group_mode));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
    } else {
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
    }

exit:
    if (BCMLT_INVALID_HDL != grp_template) {
        (void) bcmlt_entry_free(grp_template);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Field driver function variable for bcm56990_a0 device.
 */
static mbcm_ltsw_field_drv_t bcm56990_a0_ltsw_field_drv = {
    .field_control_info_init = bcm56990_a0_ltsw_field_control_info_init,
    .field_ingress_profile_init    = bcm56990_a0_ltsw_field_ingress_profile_init,
    .field_ingress_profile_deinit  = bcm56990_a0_ltsw_field_ingress_profile_deinit,
    .field_lt_qual_name_update = bcm56990_a0_ltsw_field_lt_qual_name_update,
    .field_src_class_lt_symbol_to_enum_get = bcm56990_a0_ltsw_field_src_class_lt_symbol_to_enum_get,
    .field_src_class_enum_to_lt_symbol_get = bcm56990_a0_ltsw_field_src_class_enum_to_lt_symbol_get,
    .field_qualify_macro_values_get = bcm56990_a0_ltsw_field_qualify_macro_values_get,
    .field_ifp_clear = xgs_ltsw_ifp_clear,
    .field_vfp_clear = xgs_ltsw_vfp_clear,
    .field_efp_clear = xgs_ltsw_efp_clear,
    .field_clear = xgs_ltsw_field_clear,
    .field_port_filter_enable_set = xgs_ltsw_field_port_filter_enable_set,
    .field_group_mode_set = bcm56990_a0_ltsw_field_group_mode_set,
    .field_pbmp_index_get = bcm56990_a0_ltsw_field_pbmp_index_get,
    .field_group_pbmp_get = bcm56990_a0_ltsw_field_group_pbmp_get,
    .field_group_pipe_get = bcm56990_a0_ltsw_field_group_pipe_get,
    .field_pbmp_get = bcm56990_a0_ltsw_field_pbmp_get,
    .field_src_class_hw_fields_encode_set = bcm56990_a0_ltsw_field_src_class_hw_fields_encode_set,
    .field_src_class_hw_fields_decode_get = bcm56990_a0_ltsw_field_src_class_hw_fields_decode_get,
    .field_internal_entry_enable_get = bcm56990_a0_ltsw_field_internal_entry_enable_get,
    .field_udf_qset_set = xgs_ltsw_field_udf_qset_set,
    .field_udf_obj_get = xgs_ltsw_field_udf_obj_get,
    .field_udf_qset_del = xgs_ltsw_field_udf_qset_del,
    .field_udf_qual_set = xgs_ltsw_field_udf_qual_set,
    .field_udf_qual_get = xgs_ltsw_field_udf_qual_get,
    .field_action_profile_index_get = xgs_ltsw_field_action_profile_index_get,
    .field_action_profile_add = xgs_ltsw_field_action_profile_add,
    .field_action_profile_get = xgs_ltsw_field_action_profile_get,
    .field_action_profile_del = xgs_ltsw_field_action_profile_del,
    .field_destination_init = bcm56990_a0_ltsw_field_destination_init,
    .field_destination_deinit = bcm56990_a0_ltsw_field_destination_deinit,
    .field_qual_gport_validate = bcm56990_a0_ltsw_field_qual_gport_validate,
    .field_policer_attach = bcm56990_a0_ltsw_field_policer_attach,
    .field_policer_detach = bcm56990_a0_ltsw_field_policer_detach,
    .field_qual_mask_lt_field_get = bcm56990_a0_ltsw_field_qual_mask_lt_field_get,
    .field_qualify_packetres_set = bcm56990_a0_ltsw_field_qualify_packetres_set,
    .field_qualify_packetres_get = bcm56990_a0_ltsw_field_qualify_packetres_get,
    .field_action_param_symbol_set = bcm56990_a0_ltsw_field_action_param_symbol_set,
    .field_action_param_symbol_get = bcm56990_a0_ltsw_field_action_param_symbol_get,
    .field_action_value_set = xgs_ltsw_field_action_value_set,
    .field_action_value_get = xgs_ltsw_field_action_value_get,
    .field_qual_value_set = xgs_ltsw_field_qual_value_set,
    .field_qual_value_get = xgs_ltsw_field_qual_value_get,
    .field_qual_info_set = xgs_ltsw_field_qual_info_set,
    .field_aacl_class_id_get = bcm56990_a0_ltsw_field_aacl_class_id_get,
    .field_action_stat_group_lt_field_validate =
        bcm56990_a0_ltsw_field_action_stat_group_lt_field_validate,
    .field_group_udf_qset_update_with_hints =
        xgs_ltsw_field_group_udf_qset_update_with_hints,
    .field_egr_group_qset_validate = bcm56990_a0_field_egr_group_qset_validate,
    .field_stage_info_init = BCM56990_A0_SUB_DRV,
    .field_action_lt_map_valid_info_get = xgs_ltsw_field_action_lt_map_valid_info_get,
    .field_action_mirror_set = xgs_ltsw_field_action_mirror_set,
    .field_action_mirror_remove = xgs_ltsw_field_action_mirror_remove,
    .field_action_mirror_index_get = xgs_ltsw_field_action_mirror_index_get,
    .field_action_default_value_get = xgs_ltsw_field_action_default_value_get,
    .field_qualify_port_get = xgs_ltsw_field_qualify_port_get,
    .field_qualify_port_set = xgs_ltsw_field_qualify_port_set,
    .field_qualify_dstl3egress_get = xgs_ltsw_field_qualify_dstl3egress_get,
    .field_qualify_dstl3egress_set = xgs_ltsw_field_qualify_dstl3egress_set,
    .field_compression_set = xgs_ltsw_field_compression_set,
    .field_compression_del = xgs_ltsw_field_compression_del,
    .field_group_mode_get = bcm56990_a0_ltsw_field_group_mode_get,
    .field_qualify_mhopcode_set = bcm56990_a0_ltsw_field_qualify_mhopcode_set,
    .field_qualify_mhopcode_get = bcm56990_a0_ltsw_field_qualify_mhopcode_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm56990_a0_ltsw_field_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv_set(unit, &bcm56990_a0_ltsw_field_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_sub_drv_attach(unit, &bcm56990_a0_ltsw_field_drv,
                                    BCM56990_A0_SUB_DRV_BCM56990_Ax,
                                    BCM56990_A0_SUB_MOD_FIELD));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach functions for other members.
 */
#define BCM56990_A0_DRV_ATTACH_ENTRY(_dn,_vn,_pf,_pd,_r0) \
int _vn##_ltsw_field_drv_attach(int unit) \
{ \
    SHR_FUNC_ENTER(unit); \
    SHR_IF_ERR_VERBOSE_EXIT \
        (mbcm_ltsw_field_drv_set(unit, &bcm56990_a0_ltsw_field_drv)); \
    SHR_IF_ERR_VERBOSE_EXIT \
        (bcm56990_a0_sub_drv_attach(unit, &bcm56990_a0_ltsw_field_drv, \
                                    BCM56990_A0_SUB_DRV_##_dn, \
                                    BCM56990_A0_SUB_MOD_FIELD)); \
exit: \
    SHR_FUNC_EXIT(); \
}
#include "sub_devlist.h"
