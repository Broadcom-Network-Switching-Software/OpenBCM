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

#define BCM56990_FIELD_NUM_PIPES 4

/* Default VFP Virtual Slice Group Value Array */
int *def_vfp_vslice_grp_val[BCM_MAX_NUM_UNITS];

/* Default EFP Virtual Slice Group Value Array */
int *def_efp_vslice_grp_val[BCM_MAX_NUM_UNITS];

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
        bool group_info,
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
        if ((group_info) &&
                ((stage_info->stage_id == bcmiFieldStageEgress) ||
                 (stage_info->stage_id == bcmiFieldStageVlan)) &&
                ((sal_strncmp(lt_map_ref->lt_field_name,
                              "QUAL_SRC_IP6_UPPER", 18) == 0) ||
                 (sal_strncmp(lt_map_ref->lt_field_name,
                              "QUAL_SRC_IP6_LOWER", 18) == 0) ||
                 (sal_strncmp(lt_map_ref->lt_field_name,
                              "QUAL_DST_IP6_UPPER", 18) == 0) ||
                 (sal_strncmp(lt_map_ref->lt_field_name,
                              "QUAL_DST_IP6_LOWER", 18) == 0))) {
            BCMINT_FIELD_MEM_ALLOC
                (*lt_field_name,
                 strlen(lt_map_ref->lt_field_name) + 1 - strlen("_UPPER"),
                 "grp_lt_qual_name");
            sal_strncpy(*lt_field_name,
                    lt_map_ref->lt_field_name,
                    (strlen(lt_map_ref->lt_field_name) - strlen("_LOWER")));
        } else {
            BCMINT_FIELD_MEM_ALLOC
                (*lt_field_name, strlen(lt_map_ref->lt_field_name) + 1,
                 "grp_lt_qual_name");
            sal_strcpy(*lt_field_name, lt_map_ref->lt_field_name);
        }
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
bcm56990_a0_field_egr_group_qset_validate(
    int unit,
    bcm_field_group_t group_id,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_qset_t *new_qset,
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
                    new_qset,
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
                    unit, new_qset, stage_info, 0, NULL)) {
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
    int dunit = 0;
    bcmi_field_ha_group_oper_t *oper_temp = NULL;
    bcm_field_group_mode_type_t mode_type = bcmFieldGroupModeTypeAuto;
    bcm_field_group_packet_type_t packet_type = bcmFieldGroupPacketTypeDefault;
    bcm_field_qset_t grp_qset;
    int index = 0;
    bool skip = FALSE;
    bcm_field_group_mode_type_t mode_type_final = bcmFieldGroupModeTypeAuto;
    bcm_field_group_packet_type_t packet_type_final = bcmFieldGroupPacketTypeDefault;
    char *mode_type_char = NULL;
    char *packet_type_char = NULL;

    SHR_FUNC_ENTER(unit);
    /* go thru the groups present*/
    for (index = 1;
         index <= ((stage_info->group_info->gid_max)-
         BCMINT_FIELD_GROUP_RESERVE_GID_COUNT); index++) {
        if (SHR_BITGET(stage_info->group_info->grpid_bmp.w, index)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_oper_lookup(unit, index,
                                                stage_info,
                                                &oper_temp));

            if ((group_config->priority != oper_temp->priority)) {
                continue;
            }
            /* Same prio groups with different mode cannot exist in EFP. */
            BCM_FIELD_QSET_INIT(grp_qset);
            BCMINT_FIELD_QUAL_ARR_TO_QSET(oper_temp->qual_cnt,
                                          oper_temp->qset_arr,
                                          grp_qset);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_a0_field_egr_group_qset_validate(dunit,
                                                           index,
                                                           stage_info,
                                                           &grp_qset,
                                                           &mode_type,
                                                           &packet_type));
            if ((group_config->mode_type != bcmFieldGroupModeTypeAuto) &&
            (group_config->mode_type != mode_type)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
            } else if (group_config->mode_type == bcmFieldGroupModeTypeAuto) {
                skip = TRUE;
                mode_type_final = mode_type;
                if (group_config->packet_type == bcmFieldGroupPacketTypeDefault) {
                    packet_type_final = packet_type;
                } else {
                    packet_type_final = group_config->packet_type;
                }
            }
        }
        if (skip == TRUE) {
            break;
        }
    }

    for (key_mode = 0; key_mode < BCMINT_FIELD_EFP_KEYS; key_mode++) {
        if (skip) {
            if ((mode_type_final == stage_info->efp_key_qual_map[key_mode].mode_type) &&
                (packet_type_final == stage_info->efp_key_qual_map[key_mode].packet_type)) {
                break;
            }
        } else {
           if (TRUE == bcmint_field_find_is_subset_qset(
                &(group_config->qset),
                &(stage_info->efp_key_qual_map[key_mode].key_qset))) {
                if (group_config->mode == bcmFieldGroupModeAuto) {
                    if (group_config->mode_type == bcmFieldGroupModeTypeAuto) {
                        if ((group_config->packet_type
                            == stage_info->efp_key_qual_map[key_mode].packet_type) ||
                            ((group_config->packet_type == bcmFieldGroupPacketTypeDefault) &&
                            (stage_info->efp_key_qual_map[key_mode].default_mode == true)) ||
                            (group_config->packet_type == bcmFieldGroupPacketTypeAuto)){
                            break;
                        }
                    } else {
                        if ((((group_config->packet_type == bcmFieldGroupPacketTypeDefault) &&
                            (stage_info->efp_key_qual_map[key_mode].default_mode == true)) ||
                            (group_config->packet_type == bcmFieldGroupPacketTypeAuto)
                            || (group_config->packet_type
                            == stage_info->efp_key_qual_map[key_mode].packet_type)) &&
                            (group_config->mode_type
                            == stage_info->efp_key_qual_map[key_mode].mode_type)) {
                            break;
                        }
                    }
                } else  if (group_config->mode == bcmFieldGroupModeSingle) {
                    if ((stage_info->efp_key_qual_map[key_mode].mode_type
                        >= bcmFieldGroupModeTypeL2Single) &&
                        (stage_info->efp_key_qual_map[key_mode].mode_type
                        <= bcmFieldGroupModeTypeL3SingleAny)) {
                    if (group_config->mode_type == bcmFieldGroupModeTypeAuto) {
                        if ((group_config->packet_type
                            == stage_info->efp_key_qual_map[key_mode].packet_type) ||
                            ((group_config->packet_type == bcmFieldGroupPacketTypeDefault) &&
                            (stage_info->efp_key_qual_map[key_mode].default_mode == true)) ||
                            (group_config->packet_type == bcmFieldGroupPacketTypeAuto)){
                            break;
                        }
                    } else {
                        if ((((group_config->packet_type == bcmFieldGroupPacketTypeDefault) &&
                            (stage_info->efp_key_qual_map[key_mode].default_mode == true)) ||
                            (group_config->packet_type == bcmFieldGroupPacketTypeAuto)
                            || (group_config->packet_type
                            == stage_info->efp_key_qual_map[key_mode].packet_type)) &&
                            (group_config->mode_type
                            == stage_info->efp_key_qual_map[key_mode].mode_type)) {
                            break;
                        }
                    }
                  }
                } else  if (group_config->mode == bcmFieldGroupModeDouble) {
                    if ((stage_info->efp_key_qual_map[key_mode].mode_type
                        >= bcmFieldGroupModeTypeL3Double) &&
                        (stage_info->efp_key_qual_map[key_mode].mode_type
                        <= bcmFieldGroupModeTypeL3DoubleAlternate)) {
                        if (group_config->mode_type == bcmFieldGroupModeTypeAuto) {
                            if ((group_config->packet_type
                                == stage_info->efp_key_qual_map[key_mode].packet_type) ||
                                ((group_config->packet_type == bcmFieldGroupPacketTypeDefault) &&
                                (stage_info->efp_key_qual_map[key_mode].default_mode == true)) ||
                                (group_config->packet_type == bcmFieldGroupPacketTypeAuto)){
                                break;
                            }
                        } else {
                            if ((((group_config->packet_type == bcmFieldGroupPacketTypeDefault) &&
                                (stage_info->efp_key_qual_map[key_mode].default_mode == true)) ||
                                (group_config->packet_type == bcmFieldGroupPacketTypeAuto)
                                || (group_config->packet_type
                                == stage_info->efp_key_qual_map[key_mode].packet_type)) &&
                                (group_config->mode_type
                                == stage_info->efp_key_qual_map[key_mode].mode_type)) {
                                break;
                            }
                        }
                    }
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
                }
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

    mode_type_char = stage_info->efp_key_qual_map[key_mode].grp_mode;
    packet_type_char = stage_info->efp_key_qual_map[key_mode].port_pkt_type;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(grp_template, "MODE",
                mode_type_char));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(grp_template, "PORT_PKT_TYPE",
                packet_type_char));
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

static int
bcm56990_a0_ltsw_field_pbmp_mask_get(int unit,
                                     int pipe,
                                     bcmint_field_stage_info_t *stage_info,
                                     bcm_pbmp_t *in_pbmp_mask,
                                     uint8_t *out_pbmp_idx_mask)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(in_pbmp_mask, SHR_E_INTERNAL);
    SHR_NULL_CHECK(out_pbmp_idx_mask, SHR_E_INTERNAL);

    sal_memcpy(in_pbmp_mask, out_pbmp_idx_mask, sizeof(bcm_pbmp_t));

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

    if (0 == (BCM_GPORT_IS_MPLS_PORT(gport) ||
              BCM_GPORT_IS_WLAN_PORT(gport) ||
              BCM_GPORT_IS_VLAN_PORT(gport) ||
              BCM_GPORT_IS_VXLAN_PORT(gport) ||
              BCM_GPORT_IS_NIV_PORT(gport) ||
              BCM_GPORT_IS_MIM_PORT(gport) ||
              BCM_GPORT_IS_FLOW_PORT(gport) ||
              BCM_GPORT_IS_MODPORT(gport) ||
              BCM_GPORT_IS_TRUNK(gport))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((stage_info->stage_id == bcmiFieldStageIngress) ||
            (stage_info->stage_id == bcmiFieldStageExactMatch)) {
        if (1 == (BCM_GPORT_IS_MODPORT(gport))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (!(BCM_GPORT_IS_TRUNK(gport))) {
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
                (0 == strcmp(group_mode,"L3_ANY_DOUBLE_WIDE"))) {
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

int
bcm56990_ltsw_field_group_slice_info_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int group_id,
    uint8_t max_slices,
    uint32_t **pslice_id,
    uint32_t **vslice_id,
    uint8_t *slice_cnt)
{
    int dunit = 0;
    uint8_t part;
    uint64_t num_parts = 0;
    uint32_t num_elem = 0;
    uint64_t num_slice = 0;
    uint32_t num_slices = 0;
    char *pslice_fid = NULL;
    char *vslice_fid = NULL;
    uint8_t s, cnt = 0;
    uint64_t pslice[BCMI_FIELD_HA_VSLICE_MAX];
    uint64_t vslice[BCMI_FIELD_HA_VSLICE_MAX];
    bcmlt_entry_handle_t lt_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t grp_info_templ = BCMLT_INVALID_HDL;
    bcmint_field_tbls_info_t *tbls_info = stage_info->tbls_info;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_NULL_CHECK(tbls_info->group_info_partition_sid, SHR_E_UNAVAIL);

    /* Allocate and Read grp info template of source entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &grp_info_templ,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL,
                                      tbls_info->group_info_sid,
                                      tbls_info->group_info_key_fid,
                                      0, group_id, NULL,
                                      NULL,
                                      0,
                                      0, 0,
                                      0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(grp_info_templ,
                               tbls_info->group_info_num_parts_fid,
                               &num_parts));

    vslice_fid = tbls_info->group_info_virtual_slice_fid;
    pslice_fid = tbls_info->group_info_slice_fid;

    /* Entry handle allocate for operational mode. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              tbls_info->group_info_partition_sid,
                              &lt_hdl));

    group_id = group_id & BCM_FIELD_ID_MASK;

    /* Add group_template_id field to Group part info LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(lt_hdl,
                               tbls_info->group_info_partition_key_fid,
                               group_id));

    for (part = 1; part <= num_parts; part++) {
        /* Add first partition ID field to Group part info LT */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(lt_hdl,
                                   tbls_info->group_info_common_partition_key_fid,
                                   part));

        /* Lookup the group partition info */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, lt_hdl, BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(lt_hdl,
                                   tbls_info->group_info_num_slice_fid,
                                   &num_slice));
        num_slices = COMPILER_64_LO(num_slice);
        if (max_slices < num_slices) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }


        if (pslice_id) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(lt_hdl,
                                             pslice_fid,
                                             0,
                                             pslice,
                                             num_slices,
                                             &num_elem));
        }
        if (vslice_id) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(lt_hdl,
                                             vslice_fid,
                                             0,
                                             vslice,
                                             num_slices,
                                             &num_elem));
        }

        for (s = 0; s < num_slices; s++) {
            *(*pslice_id + cnt) = COMPILER_64_LO(pslice[s]);
            *(*vslice_id + cnt) = COMPILER_64_LO(vslice[s]);
            cnt++;
        }
    }

    if (slice_cnt) {
        *slice_cnt = cnt;
    }

exit:
    if (lt_hdl != BCMLT_INVALID_HDL) {
        (void)(bcmlt_entry_free(lt_hdl));
    }

    if (grp_info_templ != BCMLT_INVALID_HDL) {
        (void)(bcmlt_entry_free(grp_info_templ));
    }

    SHR_FUNC_EXIT();
}

int
bcm56990_ltsw_field_p2v_slice_map_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int8_t *p2v_slice_mapping)
{
    int dunit;
    int gid = 0;
    uint8_t ps;
    uint8_t iter = 0;
    uint8_t num_slices = 0;
    uint8_t max_slices = 0;
    uint64_t pipe_val;
    uint64_t group_id;
    uint32_t *vslice_arr = NULL;
    uint32_t *pslice_arr = NULL;
    bcmlt_entry_handle_t grp_template = BCMLT_INVALID_HDL;
    SHR_FUNC_ENTER(unit);

    if ((stage_info->tbls_info->group_info_partition_sid == NULL) ||
        (stage_info->tbls_info->group_info_slice_fid == NULL) ||
        (stage_info->tbls_info->group_info_num_slice_fid == NULL) ||
        (stage_info->tbls_info->group_info_virtual_slice_fid == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    max_slices = stage_info->num_slices;
    /* Allocate action stength */
    BCMINT_FIELD_MEM_ALLOC(pslice_arr, sizeof(uint32_t) * max_slices,
                           "physical slice array alloc");
    BCMINT_FIELD_MEM_ALLOC(vslice_arr, sizeof(uint32_t) * max_slices,
                           "virtual slice array alloc");

    for (ps = 0; ps < max_slices; ps++) {
        pslice_arr[ps] = vslice_arr[ps] = -1;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->group_sid,
                              &grp_template));

    while ((bcmi_lt_entry_commit(unit, grp_template,
                    BCMLT_OPCODE_TRAVERSE,
                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        if (pipe != -1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(grp_template,
                                       "PIPE",
                                       &pipe_val));
            if (pipe != COMPILER_64_LO(pipe_val)) {
                continue;
            }
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(grp_template,
                                   stage_info->tbls_info->group_key_fid,
                                   &group_id));
        gid = COMPILER_64_LO(group_id);

        num_slices = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56990_ltsw_field_group_slice_info_get(unit, stage_info,
                                                      gid,
                                                      max_slices,
                                                      &pslice_arr,
                                                      &vslice_arr,
                                                      &num_slices));
        for (ps = 0; ps < max_slices; ps++) {
            for (iter = 0; iter < num_slices; iter++) {
                if (pslice_arr[iter] == ps) {
                    p2v_slice_mapping[ps] = vslice_arr[iter];
                }
            }
        }
    }

exit:
    if (grp_template !=  BCMLT_INVALID_HDL) {
        (void)(bcmlt_entry_free(grp_template));
    }

    if (pslice_arr) {
        BCMINT_FIELD_MEM_FREE(pslice_arr);
    }

    if (vslice_arr) {
        BCMINT_FIELD_MEM_FREE(vslice_arr);
    }
    SHR_FUNC_EXIT();
}

int
ltsw_fp_group_flexctr_action_update(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper)
{
    uint32_t idx;
    uint32_t slice_id;
    uint8_t cnt, a_cnt = 0;
    bcm_field_group_t group_id;
    uint32_t flexctr_action_id[10] = {0};
    bcmi_ltsw_flexctr_counter_info_t ctr_info;
    bcmi_field_ha_blk_info_t *entry_base = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);

    group_id = group_oper->group_id | stage_info->stage_base_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_primary_slice_id_get(unit, group_id, &slice_id));

    FP_HA_ENT_BASE_BLK_GET(unit,
                           stage_info->stage_id,
                           0,
                           entry_base);
    for (idx = 0; idx < stage_info->entry_info->hash_size; idx++) {
        FP_HA_ENT_BASE_OPER_GET(unit,
                                stage_info->stage_id,
                                entry_base,
                                idx,
                                entry_oper);
        if (entry_oper == NULL) {
            continue;
        }

        /* Search the list of entries */
        do {
            if ((entry_oper->flexctr_action_id == 0) ||
                (entry_oper->group_id != group_oper->group_id)) {
                FP_HA_ENT_OPER_NEXT(unit, entry_oper, entry_oper);
                continue;
            }

            if (a_cnt != 0) {
                for (cnt = 0; cnt < a_cnt; cnt++) {
                    if (flexctr_action_id[cnt] == entry_oper->flexctr_action_id) {
                        break;
                    }
                }
                if (cnt != a_cnt) {
                    FP_HA_ENT_OPER_NEXT(unit, entry_oper, entry_oper);
                    continue;
                }
            }
            flexctr_action_id[a_cnt] = entry_oper->flexctr_action_id;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_flexctr_counter_id_info_get(unit,
                                                       flexctr_action_id[a_cnt],
                                                       &ctr_info));
            if ((ctr_info.act_cfg.hint_type == bcmFlexctrHintFieldGroupId) ||
                (ctr_info.act_cfg.hint_type == bcmFlexctrHintPool)) {
                ctr_info.act_cfg.index_operation.object_id[0] = slice_id;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_flexctr_action_object_id_update(
                                   unit,
                                   entry_oper->flexctr_action_id,
                                   &(ctr_info.act_cfg.index_operation)));
            }
            a_cnt++;
            FP_HA_ENT_OPER_NEXT(unit, entry_oper, entry_oper);
        } while (entry_oper != NULL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
ltsw_fp_group_auto_expand_flexctr_action_update(
    int unit,
    int pipe,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    uint32_t *ctr_action_vmap,
    uint8_t action_cnt)
{
    uint8_t index, iter, ctr;
    uint32_t obj_id;
    bool obj_id_set;
    bcm_flexctr_object_t object;
    bool update[BCMI_FIELD_HA_VSLICE_MAX] = {false};
    uint32_t flexctr_action_id[BCMI_FIELD_HA_VSLICE_MAX] = {0};
    bcmi_ltsw_flexctr_counter_info_t *ctr_info_ptr;
    bcmi_ltsw_flexctr_counter_info_t ctr_info[BCMI_FIELD_HA_VSLICE_MAX];
    SHR_FUNC_ENTER(unit);

    sal_memset(&ctr_info, 0x0,
           sizeof(bcmi_ltsw_flexctr_counter_info_t) * BCMI_FIELD_HA_VSLICE_MAX);
    for (index = 0; index < action_cnt; index++) {
        obj_id_set = false;
        ctr_info_ptr = NULL;
        ctr = 0;
        for (iter = 0; iter < BCMI_FIELD_HA_VSLICE_MAX; iter++) {
            if (ctr_info[iter].action_index == ctr_action_vmap[index]) {
                ctr_info_ptr = &ctr_info[iter];
                obj_id_set = true;
                ctr = 1;
                break;
            }
        }

        if (ctr_info_ptr == NULL) {
            ctr_info_ptr = &ctr_info[index];

            ctr_info_ptr->source = bcmFlexctrSourceEfp;
            ctr_info_ptr->stage = BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR;
            ctr_info_ptr->action_index = ctr_action_vmap[index];
            ctr_info_ptr->pipe = (pipe == -1) ? 0 : pipe;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_flexctr_counter_id_get(unit, ctr_info_ptr,
                                                  &flexctr_action_id[index]));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_flexctr_counter_id_info_get(unit,
                                                       flexctr_action_id[index],
                                                       ctr_info_ptr));
        }

        for (; ctr < 2; ctr++) {
            object = ctr_info_ptr->act_cfg.index_operation.object[ctr];
            if (object != bcmFlexctrObjectStaticEgrFieldStageEgress) {
                continue;
            }
            obj_id = ctr_info_ptr->act_cfg.index_operation.object_id[ctr];
            if (group_oper->ctr_action_vmap_arr[obj_id] != ctr_info_ptr->action_index) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }

            ctr_info_ptr->act_cfg.index_operation.object_id[ctr] = index;
            if (obj_id_set == false) {
                update[index] = true;
            }
            break;
        }
    }

    for (index = 0; index < BCMI_FIELD_HA_VSLICE_MAX; index++) {
        if (update[index] == true) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                            "Flexctr update for flexctr_action_id:[0x%x]"
                            " object_id[0]:[%d] object_id[1]:%d\n\r"),
                        flexctr_action_id[index],
                        ctr_info[index].act_cfg.index_operation.object_id[0],
                        ctr_info[index].act_cfg.index_operation.object_id[1]));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_flexctr_action_object_id_update(
                                unit,
                                flexctr_action_id[index],
                                &(ctr_info[index].act_cfg.index_operation)));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int
ltsw_field_flexctr_action_update(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int8_t *new_vslice_map,
    int8_t *old_vslice_map,
    uint8_t num_slices)
{
    uint8_t i = 0;
    uint8_t s_id = 0;
    uint32_t idx;
    int pipe_num;
    int8_t vslice_id = 0;
    int8_t old_vslice_id = 0;
    bcmi_field_ha_blk_info_t *grp_base = NULL;
    bcmi_field_ha_group_oper_t *grp_oper = NULL;
    uint32_t used_profiles[BCMI_FIELD_HA_VSLICE_MAX] = {0};
    uint32_t ctr_action_vmap[BCMI_FIELD_HA_VSLICE_MAX] = {0};

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(stage_info, SHR_E_PARAM);
    SHR_NULL_CHECK(new_vslice_map, SHR_E_PARAM);
    SHR_NULL_CHECK(old_vslice_map, SHR_E_PARAM);

    /* Field Stage clear */
    FP_HA_GRP_BASE_BLK_GET(unit,
            stage_info->stage_id,
            0,
            grp_base);
    if (grp_base == NULL) {
        SHR_EXIT();
    }

    /*
     *1. Retain ctr profile id at virtual slice ID location if
     *      old_vslice_id == new_virtual_slice_id
     *      old_vslice_id != -1
     *      new_vslice_id != -1
     *2. Copy old_vslice_id ctr profile id to new virtual slice id
     *   profile position if
     *      old_vslice_id != new_virtual_slice_id and
     *      old_vslice_id != -1
     *      new_vslice_id != -1
     *3. Retain ctr profile id at new virtual slice ID location if the profile
     *   ID is not used in above cases.
     *      old_vslice_id = -1
     *      new_vslice_id != -1
     *      used_profile = 0
     *3. Retain ctr profile id at virtual slice ID location if the profile
     *   ID is not used in above cases.
     *      old_vslice_id = -1
     *      new_vslice_id = -1
     *      used_profile = 0
     *
     * C --> copy old slice profile id to new vslice position
     * R --> Retain old profile Id at old vslice position
     * NA --> use remaining profiles after valid moves
     *
     *                          New Vslice ID
     *                    |  -1  |   x   |   Y
     *   -------------------------------------------
     *                 -1 |  NA  |   NA  |   NA
     *   Old vslice ID  x |  NA  |   R   |   C
     *                  y |  NA  |   C   |   R
     */
    for (idx = 0; idx < stage_info->group_info->hash_size; idx++) {
        grp_oper = NULL;
        FP_HA_GRP_BASE_OPER_GET(unit,
                stage_info->stage_id,
                grp_base,
                idx,
                grp_oper);
        if (grp_oper == NULL) {
            continue;
        }

        /* loop through all the valid groups in this stage, pipe */
        do {
            /* pipe check */
            if (pipe != -1) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_group_pipe_get(unit, stage_info,
                                                 grp_oper, &pipe_num));
                if (pipe != pipe_num) {
                    FP_HA_GRP_OPER_NEXT(unit, grp_oper, grp_oper);
                    continue;
                }
            }

            /* Check if multiple counter profiles are mapped to this group */
            if (grp_oper->group_flags & BCMINT_FIELD_GROUP_FLEXCTR_VMAP_ENABLED) {
                sal_memset(ctr_action_vmap, 0,
                        sizeof(uint32_t) * BCMI_FIELD_HA_VSLICE_MAX);
                sal_memset(used_profiles, 0,
                        sizeof(uint32_t) * BCMI_FIELD_HA_VSLICE_MAX);

                for (s_id = 0; s_id < num_slices; s_id++) {
                    if (old_vslice_map[s_id] != new_vslice_map[s_id]) {
                        /* New group or auto expansion to new slice case */
                        if ((old_vslice_map[s_id] == -1 &&
                             new_vslice_map[s_id] != -1)) {
                            vslice_id = new_vslice_map[s_id];
                            if (used_profiles[vslice_id] == 0) {
                                ctr_action_vmap[vslice_id] =
                                    grp_oper->ctr_action_vmap_arr[vslice_id];
                                used_profiles[vslice_id] = 1;
                            }
                            /* Group deletion or auto reduction case */
                        } else if (old_vslice_map[s_id] != -1 &&
                                   new_vslice_map[s_id] == -1) {
                            /*
                             * Do not update anything in the
                             * ctr_action_vmap. Update this depeneding on
                             * what is left out from used prof ID's
                             */
                        } else {
                            old_vslice_id = old_vslice_map[s_id];
                            vslice_id = new_vslice_map[s_id];
                            ctr_action_vmap[vslice_id] =
                                grp_oper->ctr_action_vmap_arr[old_vslice_id];
                            if (used_profiles[old_vslice_id] == 1)  {
                                /* reset the corresponding tmp_ctr */
                                ctr_action_vmap[old_vslice_id] = 0;
                            }
                            used_profiles[old_vslice_id] = 1;
                        }
                    } else {
                        if (new_vslice_map[s_id] != -1) {
                            vslice_id = new_vslice_map[s_id];
                            ctr_action_vmap[vslice_id] =
                                grp_oper->ctr_action_vmap_arr[vslice_id];
                            used_profiles[vslice_id] = 1;
                        }
                    }
                }
                for (vslice_id = 0;
                     vslice_id < num_slices;
                     vslice_id++) {
                    if (ctr_action_vmap[vslice_id] == 0) {
                        /*check if this is used already in grp_oper */
                        if (used_profiles[vslice_id] == 0) {
                            ctr_action_vmap[vslice_id] =
                                grp_oper->ctr_action_vmap_arr[vslice_id];
                            used_profiles[vslice_id] = 1;
                        } else {
                            /* check first unused profile */
                            for (i = 0;i <num_slices; i++) {
                                if (used_profiles[i] == 0) {
                                    ctr_action_vmap[vslice_id] =
                                        grp_oper->ctr_action_vmap_arr[i];
                                    used_profiles[i] = 1;
                                    break;
                                }
                            }
                        }
                    }
                }

                if (sal_memcmp(grp_oper->ctr_action_vmap_arr,
                               ctr_action_vmap,
                               sizeof(ctr_action_vmap))) {
                    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "group:[%d] old_ctr_action_vmap_arr: [%u %u %u %u]\n\r"),
                                grp_oper->group_id,
                                grp_oper->ctr_action_vmap_arr[0],
                                grp_oper->ctr_action_vmap_arr[1],
                                grp_oper->ctr_action_vmap_arr[2],
                                grp_oper->ctr_action_vmap_arr[3]));
                    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "group:[%d] new_ctr_action_vmap_arr: [%u %u %u %u]\n\r"),
                                grp_oper->group_id,
                                ctr_action_vmap[0],
                                ctr_action_vmap[1],
                                ctr_action_vmap[2],
                                ctr_action_vmap[3]));

                    SHR_IF_ERR_VERBOSE_EXIT
                        (ltsw_fp_group_auto_expand_flexctr_action_update(
                                          unit, pipe, stage_info,
                                          grp_oper, ctr_action_vmap,
                                          BCMI_FIELD_HA_VSLICE_MAX));
                    sal_memcpy(grp_oper->ctr_action_vmap_arr, ctr_action_vmap,
                               sizeof(ctr_action_vmap));
                }
            } else {
                /* For not auto-expanded groups */
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_fp_group_flexctr_action_update(
                               unit, stage_info,
                               grp_oper));
            }

            FP_HA_GRP_OPER_NEXT(unit, grp_oper, grp_oper);
        } while (grp_oper != NULL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_ltsw_field_flexctr_action_update(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe)
{
    int8_t idx;
    int8_t *curr_p2v_slice_mapping;
    int8_t *new_p2v_slice_mapping = NULL;
    bool more_than_one_group = false;

    SHR_FUNC_ENTER(unit);

    if (pipe == -1) {
        curr_p2v_slice_mapping = stage_info->p2v_slice_mapping;
    } else {
        curr_p2v_slice_mapping = &stage_info->p2v_slice_mapping[pipe];
    }

    BCMINT_FIELD_MEM_ALLOC(new_p2v_slice_mapping,
            sizeof(int8_t) * stage_info->num_slices,
            "New P2V slice mapping");

    for (idx = 0; idx < stage_info->num_slices; idx++) {
        new_p2v_slice_mapping[idx] = -1;
        if (curr_p2v_slice_mapping[idx] != -1) {
            more_than_one_group = true;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_ltsw_field_p2v_slice_map_get(unit, stage_info, pipe,
                                               new_p2v_slice_mapping));

    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "old_p2v_slice_mapping: [%d %d %d %d]\n\r"),
                curr_p2v_slice_mapping[0], curr_p2v_slice_mapping[1],
                curr_p2v_slice_mapping[2], curr_p2v_slice_mapping[3]));
    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "new_p2v_slice_mapping: [%d %d %d %d]\n\r"),
                new_p2v_slice_mapping[0], new_p2v_slice_mapping[1],
                new_p2v_slice_mapping[2], new_p2v_slice_mapping[3]));

    if (more_than_one_group == true) {
        if (sal_memcmp(curr_p2v_slice_mapping,
                       new_p2v_slice_mapping,
                       stage_info->num_slices) != 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_field_flexctr_action_update(unit, stage_info, pipe,
                                                  new_p2v_slice_mapping,
                                                  curr_p2v_slice_mapping,
                                                  stage_info->num_slices));
        }
    }

    sal_memcpy(curr_p2v_slice_mapping, new_p2v_slice_mapping,
               stage_info->num_slices);
exit:
    if (new_p2v_slice_mapping) {
        BCMINT_FIELD_MEM_FREE(new_p2v_slice_mapping);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56990_ltsw_field_vslice_group_alloc(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int grp_prio,
    int action_res_id,
    int *free_index)
{
    int *def_vslice_grp_val_p = NULL;
    bcmi_field_ha_group_oper_t *oper = NULL;
    SHR_FUNC_ENTER(unit);


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_first_group_get(unit, stage_info, -1,
                                            -1, 0, &oper));
    *free_index = -1;
    if (action_res_id == BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT) {
        int vs;
        bool init = false;
        int pipe_val;

        if ((oper != NULL) &&
            !(oper->group_flags & BCMINT_FIELD_GROUP_DEFAULT_VSLICE_GRP_SET)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (pipe == -1) {
            pipe_val = 1;
        } else {
            pipe_val = 4;
        }

        if (stage_info->stage_id == bcmiFieldStageEgress) {
            if (def_efp_vslice_grp_val[unit] == NULL) {
                BCMINT_FIELD_MEM_ALLOC(def_efp_vslice_grp_val[unit],
                        sizeof(int) * BCMI_FIELD_HA_VSLICE_MAX * pipe_val,
                        "Default VFP slice group val alloc");
                init = true;
            }

            def_vslice_grp_val_p = def_efp_vslice_grp_val[unit];
        } else if (stage_info->stage_id == bcmiFieldStageVlan) {
            if (def_vfp_vslice_grp_val[unit] == NULL) {
                BCMINT_FIELD_MEM_ALLOC(def_vfp_vslice_grp_val[unit],
                        sizeof(int) * BCMI_FIELD_HA_VSLICE_MAX * pipe_val,
                        "Default VFP slice group val alloc");
                init = true;
            }

            def_vslice_grp_val_p = def_vfp_vslice_grp_val[unit];
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }

        if (init == true) {
            int p;
            int *temp_ptr;

            for (p = 0; p < pipe_val; p++) {
                temp_ptr = def_vslice_grp_val_p + (p * BCMI_FIELD_HA_VSLICE_MAX);
                for (vs = 0; vs < BCMI_FIELD_HA_VSLICE_MAX; vs++) {
                    temp_ptr[vs] = -1;
                }
            }
        }

        if (pipe != -1) {
            def_vslice_grp_val_p = def_vslice_grp_val_p +
                                            (pipe * BCMI_FIELD_HA_VSLICE_MAX);
        }

        for (vs = 0; vs < BCMI_FIELD_HA_VSLICE_MAX; vs++) {
            if (def_vslice_grp_val_p[vs] == -1) {
                *free_index = vs;
            }

            if (def_vslice_grp_val_p[vs] == grp_prio) {
                *free_index = vs;
                break;
            }
        }

        if (*free_index == -1) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
        }

        def_vslice_grp_val_p[*free_index] = grp_prio;
    } else {
        bcmlt_field_def_t fld_defs;

        if ((oper != NULL) &&
            (oper->group_flags & BCMINT_FIELD_GROUP_DEFAULT_VSLICE_GRP_SET)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        sal_memset(&fld_defs, 0, sizeof(bcmlt_field_def_t));
        /* Retreive field definition for provided lt field */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit,
                                   stage_info->tbls_info->group_sid,
                                   stage_info->tbls_info->virtual_slice_grp_fid,
                                   &fld_defs));

        if (action_res_id & ~(1 << fld_defs.width)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        *free_index = action_res_id;
    }


exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_ltsw_field_vslice_group_dealloc(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int grp_priority)
{
    int vs, num_grps = 0;
    int *def_vslice_grp_val_p = NULL;
    SHR_FUNC_ENTER(unit);

    if (stage_info->stage_id == bcmiFieldStageEgress) {
        if (def_efp_vslice_grp_val[unit] == NULL) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }

        def_vslice_grp_val_p = def_efp_vslice_grp_val[unit];
    } else if (stage_info->stage_id == bcmiFieldStageVlan) {
        if (def_vfp_vslice_grp_val[unit] == NULL) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }

        def_vslice_grp_val_p = def_vfp_vslice_grp_val[unit];
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    if (pipe != -1) {
        def_vslice_grp_val_p = def_vslice_grp_val_p +
            (pipe * BCMI_FIELD_HA_VSLICE_MAX);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_group_num_get(unit, stage_info,
                                          pipe, grp_priority,
                                          0, &num_grps));

    if (num_grps <= 1) {
        for (vs = 0; vs < BCMI_FIELD_HA_VSLICE_MAX; vs++) {
            if (def_vslice_grp_val_p[vs] == grp_priority) {
                def_vslice_grp_val_p[vs] = -1;
                break;
            }
        }

        if (vs == BCMI_FIELD_HA_VSLICE_MAX) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_ltsw_field_stage_reinit(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bool warm)
{
    bool init = false;
    uint64_t pipe;
    uint64_t action_res_id;
    int idx, vslice_idx = 0;
    int p, num_pipes, oper_mode = 0;
    int *def_vslice_grp_val_p = NULL;
    bcmi_field_ha_blk_info_t *grp_base = NULL;
    bcmi_field_ha_group_oper_t *grp_oper = NULL;
    bcmlt_entry_handle_t grp_template= BCMLT_INVALID_HDL;
    SHR_FUNC_ENTER(unit);

    if (warm == FALSE) {
        SHR_EXIT();
    }

    /* Retrieve operational mode status */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_oper_mode_status_get(unit, stage_info, &oper_mode));
    if (oper_mode == bcmiFieldOperModePipeUnique) {
        p = 0;
        num_pipes=  4;
    } else {
        p = -1;
        num_pipes = 1;
    }

    if ((stage_info->flags & BCMINT_FIELD_STAGE_FLEXSTAT_SLICE_INFO) &&
        (stage_info->flags & BCMINT_FIELD_STAGE_KEY_TYPE_FIXED)) {
        int8_t *mapping_ptr = stage_info->p2v_slice_mapping;

        if (mapping_ptr == NULL) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }

        if (p == -1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_ltsw_field_p2v_slice_map_get(unit, stage_info, p,
                                                       &mapping_ptr[0]));
        } else {
            for (p = 0; p < num_pipes; p++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcm56990_ltsw_field_p2v_slice_map_get(unit, stage_info, p,
                                                           &mapping_ptr[p]));
            }
        }
    }

    /* Field Stage clear */
    FP_HA_GRP_BASE_BLK_GET(unit,
                           stage_info->stage_id,
                           0,
                           grp_base);
    if (grp_base == NULL) {
        SHR_EXIT();
    }

    for (idx = 0; idx < stage_info->group_info->hash_size; idx++) {
        grp_oper = NULL;
        FP_HA_GRP_BASE_OPER_GET(unit,
                                stage_info->stage_id,
                                grp_base,
                                idx,
                                grp_oper);
        if (grp_oper == NULL) {
            continue;
        }
        do {
            if (!(grp_oper->group_flags &
                        BCMINT_FIELD_GROUP_DEFAULT_VSLICE_GRP_SET)) {
                SHR_EXIT();
            }

            if (stage_info->stage_id == bcmiFieldStageEgress) {
                if (def_efp_vslice_grp_val[unit] == NULL) {
                    BCMINT_FIELD_MEM_ALLOC(def_efp_vslice_grp_val[unit],
                            sizeof(int) * BCMI_FIELD_HA_VSLICE_MAX * num_pipes,
                            "Default VFP slice group val alloc");
                    init = true;
                }

                def_vslice_grp_val_p = def_efp_vslice_grp_val[unit];
            } else if (stage_info->stage_id == bcmiFieldStageVlan) {
                if (def_vfp_vslice_grp_val[unit] == NULL) {
                    BCMINT_FIELD_MEM_ALLOC(def_vfp_vslice_grp_val[unit],
                            sizeof(int) * BCMI_FIELD_HA_VSLICE_MAX * num_pipes,
                            "Default VFP slice group val alloc");
                    init = true;
                }

                def_vslice_grp_val_p = def_vfp_vslice_grp_val[unit];
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }

            if (init == true) {
                int vs;
                int *temp_ptr;

                for (p = 0; p < num_pipes; p++) {
                    temp_ptr = def_vslice_grp_val_p + (p * BCMI_FIELD_HA_VSLICE_MAX);
                    for (vs = 0; vs < BCMI_FIELD_HA_VSLICE_MAX; vs++) {
                        temp_ptr[vs] = -1;
                    }
                }
                init = false;
            }

            /* Group template lookup */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_lt_entry_commit(unit,
                                              &grp_template,
                                              BCMLT_OPCODE_LOOKUP,
                                              BCMLT_PRIORITY_NORMAL,
                                              stage_info->tbls_info->group_sid,
                                              stage_info->tbls_info->group_key_fid,
                                              0, grp_oper->group_id, NULL,
                                              NULL, 0, 0, 0, 0));
            if (oper_mode == bcmiFieldOperModePipeUnique) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(grp_template,
                                           "PIPE",
                                           &pipe));
                p = COMPILER_64_LO(pipe);

                def_vslice_grp_val_p = def_vslice_grp_val_p + (p * BCMI_FIELD_HA_VSLICE_MAX);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(grp_template,
                                       stage_info->tbls_info->virtual_slice_grp_fid,
                                       &action_res_id));
            vslice_idx = COMPILER_64_LO(action_res_id);

            def_vslice_grp_val_p[vslice_idx] = grp_oper->priority;

            (void) bcmlt_entry_free(grp_template);
            grp_template = BCMLT_INVALID_HDL;

            FP_HA_GRP_OPER_NEXT(unit, grp_oper, grp_oper);
        } while (grp_oper != NULL);
    }

exit:
    if (BCMLT_INVALID_HDL != grp_template) {
        (void) bcmlt_entry_free(grp_template);
    }
    SHR_FUNC_EXIT();
}

int
bcm56990_ltsw_field_stage_cleanup(
    int unit,
    bcmint_field_stage_info_t *stage_info)
{
    SHR_FUNC_ENTER(unit);

    if (stage_info->stage_id == bcmiFieldStageEgress) {
        if (def_efp_vslice_grp_val[unit] != NULL) {
            BCMINT_FIELD_MEM_FREE(def_efp_vslice_grp_val[unit]);
        }
    } else if (stage_info->stage_id == bcmiFieldStageVlan) {
        if (def_vfp_vslice_grp_val[unit] != NULL) {
            BCMINT_FIELD_MEM_FREE(def_vfp_vslice_grp_val[unit]);
        }
    }

    if (stage_info->p2v_slice_mapping) {
        BCMINT_FIELD_MEM_FREE(stage_info->p2v_slice_mapping);
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
    .field_pbmp_mask_get = bcm56990_a0_ltsw_field_pbmp_mask_get,
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
    .field_aacl_class_id_get = xgs_ltsw_field_aacl_class_id_get,
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
    .field_qual_info_lt_name_get = xgs_ltsw_field_qual_info_lt_name_get,
    .field_flexctr_action_update = bcm56990_ltsw_field_flexctr_action_update,
    .field_vslice_group_alloc = bcm56990_ltsw_field_vslice_group_alloc,
    .field_vslice_group_dealloc = bcm56990_ltsw_field_vslice_group_dealloc,
    .field_stage_reinit = bcm56990_ltsw_field_stage_reinit,
    .field_stage_cleanup = bcm56990_ltsw_field_stage_cleanup,
    .field_qualify_SrcTrunk_get = xgs_ltsw_field_qualify_SrcTrunk_get,
    .field_qualify_SrcTrunk_set = xgs_ltsw_field_qualify_SrcTrunk,
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
