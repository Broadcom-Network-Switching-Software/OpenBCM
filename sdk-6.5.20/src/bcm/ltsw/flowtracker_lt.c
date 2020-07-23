/*! \file flowtracker_lt.c
 *
 * Common functions for both XGS and XFS platforms for
 * accessing MON_FLOWTRACKER_* LTs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* API related header files */
#include <bcm/flowtracker.h>
#include <bcm/field.h>

/* Internal common header files */
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/util.h>

/* Internal flowtracker header files */
#include <bcm_int/ltsw/flowtracker_int.h>
#include <bcm_int/ltsw/mbcm/flowtracker.h>

/* Internal header files of other modules */
#include <bcm_int/ltsw/port.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/ltsw/l3_egress.h>

/* SDKLT specific header files */
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>

/* Shared library header files */
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>

#define BSL_LOG_MODULE BSL_LS_BCM_FLOWTRACKER

/******************************************************************************
 * Private Functions
 */
static
const char *bcm_ltsw_flowtracker_tracking_param_type_symbol_get(
                             bcm_flowtracker_tracking_param_type_t type)
{
    switch(type) {
        case bcmFlowtrackerTrackingParamTypeSrcIPv4:
            return SRC_IPV4s;
            break;
        case bcmFlowtrackerTrackingParamTypeDstIPv4:
            return DST_IPV4s;
            break;
        case bcmFlowtrackerTrackingParamTypeSrcIPv6:
            return SRC_IPV6s;
            break;
        case bcmFlowtrackerTrackingParamTypeDstIPv6:
            return DST_IPV6s;
            break;
        case bcmFlowtrackerTrackingParamTypeL4SrcPort:
            return L4_SRC_PORTs;
            break;
        case bcmFlowtrackerTrackingParamTypeL4DstPort:
            return L4_DST_PORTs;
            break;
        case bcmFlowtrackerTrackingParamTypeIPProtocol:
            return IP_PROTOCOLs;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerSrcIPv4:
            return INNER_SRC_IPV4s;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerDstIPv4:
            return INNER_DST_IPV4s;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerSrcIPv6:
            return INNER_SRC_IPV6s;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerDstIPv6:
            return INNER_DST_IPV6s;
        case bcmFlowtrackerTrackingParamTypeInnerL4SrcPort:
            return INNER_L4_SRC_PORTs;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerL4DstPort:
            return INNER_L4_DST_PORTs;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerIPProtocol:
            return INNER_IP_PROTOCOLs;
            break;
        case bcmFlowtrackerTrackingParamTypeVxlanNetworkId:
            return VNIDs;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPort:
            return IN_PORTs;
            break;
        case bcmFlowtrackerTrackingParamTypeCustom:
            return CUSTOMs;
            break;
       default:
            break;
    }

    return("");
}

static
bcm_flowtracker_tracking_param_type_t
bcm_ltsw_flowtracker_tracking_param_symbol_type_get(
                             const char *type_str)
{
    if(!sal_strcmp(type_str, SRC_IPV4s)) {
        return bcmFlowtrackerTrackingParamTypeSrcIPv4;
    }
    if(!sal_strcmp(type_str, DST_IPV4s)) {
        return bcmFlowtrackerTrackingParamTypeDstIPv4;
    }
    if(!sal_strcmp(type_str, SRC_IPV6s)) {
        return bcmFlowtrackerTrackingParamTypeSrcIPv6;
    }
    if(!sal_strcmp(type_str, DST_IPV6s)) {
        return bcmFlowtrackerTrackingParamTypeDstIPv6;
    }
    if(!sal_strcmp(type_str, L4_SRC_PORTs)) {
        return bcmFlowtrackerTrackingParamTypeL4SrcPort;
    }
    if(!sal_strcmp(type_str, L4_DST_PORTs)) {
        return bcmFlowtrackerTrackingParamTypeL4DstPort;
    }
    if(!sal_strcmp(type_str, IP_PROTOCOLs)) {
        return bcmFlowtrackerTrackingParamTypeIPProtocol;
    }
    if(!sal_strcmp(type_str, INNER_SRC_IPV4s)) {
        return bcmFlowtrackerTrackingParamTypeInnerSrcIPv4;
    }
    if(!sal_strcmp(type_str, INNER_DST_IPV4s)) {
        return bcmFlowtrackerTrackingParamTypeInnerDstIPv4;
    }
    if(!sal_strcmp(type_str, INNER_SRC_IPV6s)) {
        return bcmFlowtrackerTrackingParamTypeInnerSrcIPv6;
    }
    if(!sal_strcmp(type_str, INNER_DST_IPV6s)) {
        return bcmFlowtrackerTrackingParamTypeInnerDstIPv6;
    }
    if(!sal_strcmp(type_str, INNER_L4_SRC_PORTs)) {
        return bcmFlowtrackerTrackingParamTypeInnerL4SrcPort;
    }
    if(!sal_strcmp(type_str, INNER_L4_DST_PORTs)) {
        return bcmFlowtrackerTrackingParamTypeInnerL4DstPort;
    }
    if(!sal_strcmp(type_str, INNER_IP_PROTOCOLs)) {
        return bcmFlowtrackerTrackingParamTypeInnerIPProtocol;
    }
    if(!sal_strcmp(type_str, VNIDs)) {
        return bcmFlowtrackerTrackingParamTypeVxlanNetworkId;
    }
    if(!sal_strcmp(type_str, IN_PORTs)) {
        return bcmFlowtrackerTrackingParamTypeIngPort;
    }
    if(!sal_strcmp(type_str, CUSTOMs)) {
        return bcmFlowtrackerTrackingParamTypeCustom;
    }
    return(bcmFlowtrackerTrackingParamTypeNone);
}

static
const char *bcm_ltsw_flowtracker_export_trigger_type_symbol_get(
                             bcm_flowtracker_export_trigger_t type)
{
    switch(type) {
        case bcmFlowtrackerExportTriggerTimer:
            return TIMERs;
            break;
        case bcmFlowtrackerExportTriggerNewLearn:
            return NEW_FLOW_LEARNs;
            break;
        case bcmFlowtrackerExportTriggerAgeOut:
            return FLOW_AGE_OUTs;
            break;
        default:
            break;
    }

    return("");
}

static
bcm_flowtracker_export_trigger_t
bcm_ltsw_flowtracker_export_trigger_symbol_type_get(
                             const char *type_str)
{
    if(!sal_strcmp(type_str, TIMERs)) {
        return bcmFlowtrackerExportTriggerTimer;
    }
    if(!sal_strcmp(type_str, NEW_FLOW_LEARNs)) {
        return bcmFlowtrackerExportTriggerNewLearn;
    }
    if(!sal_strcmp(type_str, FLOW_AGE_OUTs)) {
        return bcmFlowtrackerExportTriggerAgeOut;
    }

    return(bcmFlowtrackerExportTriggerNone);
}

static
const char *bcm_ltsw_flowtracker_export_element_type_symbol_get(
        ft_template_info_elem_t element)
{
    switch(element) {
        case FT_TEMPLATE_INFO_ELEM_SRC_IPV4:
            return SRC_IPV4s;
            break;
        case FT_TEMPLATE_INFO_ELEM_DST_IPV4:
            return DST_IPV4s;
            break;
        case FT_TEMPLATE_INFO_ELEM_L4_SRC_PORT:
            return SRC_L4_PORTs;
            break;
        case FT_TEMPLATE_INFO_ELEM_L4_DST_PORT:
            return DST_L4_PORTs;
            break;
        case FT_TEMPLATE_INFO_ELEM_IP_PROTOCOL:
            return IP_PROTOCOLs;
            break;
        case FT_TEMPLATE_INFO_ELEM_PKT_TOTAL_COUNT:
            return PKT_COUNTs;
            break;
        case FT_TEMPLATE_INFO_ELEM_BYTE_TOTAL_COUNT:
            return BYTE_COUNTs;
            break;
        case FT_TEMPLATE_INFO_ELEM_INNER_SRC_IPV4:
            return INNER_SRC_IPV4s;
            break;
        case FT_TEMPLATE_INFO_ELEM_INNER_DST_IPV4:
            return INNER_DST_IPV4s;
            break;
        case FT_TEMPLATE_INFO_ELEM_INNER_L4_SRC_PORT:
            return INNER_SRC_L4_PORTs;
            break;
        case FT_TEMPLATE_INFO_ELEM_INNER_L4_DST_PORT:
            return INNER_DST_L4_PORTs;
            break;
        case FT_TEMPLATE_INFO_ELEM_INNER_IP_PROTOCOL:
            return INNER_IP_PROTOCOLs;
            break;
        case FT_TEMPLATE_INFO_ELEM_CUSTOM:
            return CUSTOMs;
            break;
        case FT_TEMPLATE_INFO_ELEM_FLOW_START_TS_MSEC:
            return FLOW_START_TIMESTAMPs;
            break;
        case FT_TEMPLATE_INFO_ELEM_OBS_TS_MSEC:
            return OBSERVATION_TIMESTAMPs;
            break;
        case FT_TEMPLATE_INFO_ELEM_GROUP_ID:
            return FLOWTRACKER_GROUPs;
            break;
        case FT_TEMPLATE_INFO_ELEM_VNID:
            return VNIDs;
            break;
        case FT_TEMPLATE_INFO_ELEM_INNER_SRC_IPV6:
            return INNER_SRC_IPV6s;
        case FT_TEMPLATE_INFO_ELEM_INNER_DST_IPV6:
            return INNER_DST_IPV6s;
        default:
            break;
    }
    return("");
}

static
ft_template_info_elem_t
bcm_ltsw_flowtracker_export_element_symbol_type_get(
const char *type_str)
{

    if(!sal_strcmp(type_str, SRC_IPV4s)) {
        return FT_TEMPLATE_INFO_ELEM_SRC_IPV4;
    }
    if(!sal_strcmp(type_str, DST_IPV4s)) {
        return FT_TEMPLATE_INFO_ELEM_DST_IPV4;
    }
    if(!sal_strcmp(type_str, SRC_L4_PORTs)) {
        return FT_TEMPLATE_INFO_ELEM_L4_SRC_PORT;
    }
    if(!sal_strcmp(type_str, DST_L4_PORTs)) {
        return FT_TEMPLATE_INFO_ELEM_L4_DST_PORT;
    }
    if(!sal_strcmp(type_str, IP_PROTOCOLs)) {
        return FT_TEMPLATE_INFO_ELEM_IP_PROTOCOL;
    }
    if(!sal_strcmp(type_str, PKT_COUNTs)) {
        return FT_TEMPLATE_INFO_ELEM_PKT_TOTAL_COUNT;
    }
    if(!sal_strcmp(type_str, BYTE_COUNTs)) {
        return FT_TEMPLATE_INFO_ELEM_BYTE_TOTAL_COUNT;
    }
    if(!sal_strcmp(type_str, INNER_SRC_IPV4s)) {
        return FT_TEMPLATE_INFO_ELEM_INNER_SRC_IPV4;
    }
    if(!sal_strcmp(type_str, INNER_DST_IPV4s)) {
        return FT_TEMPLATE_INFO_ELEM_INNER_DST_IPV4;
    }
    if(!sal_strcmp(type_str, INNER_SRC_L4_PORTs)) {
        return FT_TEMPLATE_INFO_ELEM_INNER_L4_SRC_PORT;
    }
    if(!sal_strcmp(type_str, INNER_DST_L4_PORTs)) {
        return FT_TEMPLATE_INFO_ELEM_INNER_L4_DST_PORT;
    }
    if(!sal_strcmp(type_str, INNER_IP_PROTOCOLs)) {
        return FT_TEMPLATE_INFO_ELEM_INNER_IP_PROTOCOL;
    }
    if(!sal_strcmp(type_str, CUSTOMs)) {
        return FT_TEMPLATE_INFO_ELEM_CUSTOM;
    }
    if(!sal_strcmp(type_str, FLOW_START_TIMESTAMPs)) {
        return FT_TEMPLATE_INFO_ELEM_FLOW_START_TS_MSEC;
    }
    if(!sal_strcmp(type_str, OBSERVATION_TIMESTAMPs)) {
        return FT_TEMPLATE_INFO_ELEM_OBS_TS_MSEC;
    }
    if(!sal_strcmp(type_str, FLOWTRACKER_GROUPs)) {
        return FT_TEMPLATE_INFO_ELEM_GROUP_ID;
    }
    if(!sal_strcmp(type_str, VNIDs)) {
        return FT_TEMPLATE_INFO_ELEM_VNID;
    }
    if ((sal_strcmp(type_str, INNER_SRC_IPV6s)) == 0) {
        return FT_TEMPLATE_INFO_ELEM_INNER_SRC_IPV6;
    }
    if ((sal_strcmp(type_str, INNER_DST_IPV6s)) == 0) {
        return FT_TEMPLATE_INFO_ELEM_INNER_DST_IPV6;
    }
    return(FT_TEMPLATE_MAX_INFO_ELEMENTS);
}

static
const char *bcm_ltsw_flowtracker_exact_match_idx_mode_symbol_get(
                             bcm_flowtracker_exact_match_idx_mode_t mode)
{
    switch(mode) {
        case bcmFlowtrackerExactMatchIdxModeSingle:
            return SINGLEs;
            break;
        case bcmFlowtrackerExactMatchIdxModeDouble:
            return DOUBLEs;
            break;
        case bcmFlowtrackerExactMatchIdxModeQuad:
            return QUADs;
            break;
        default:
            break;
    }

    return("");
}

static bool
bcm_ltsw_flowtracker_action_field_internal(const char *actions_type_symbol)
{
    if (!sal_strcmp(actions_type_symbol, PKT_FLOW_ELIGIBILITYs)) {
        return true;
    }
    return false;
}

/*
 * This is currently common between TD4 x11 and x9. In future, might need
 * to be chip specific.
 */
static void
bcm_ltsw_flowtracker_grp_actn_em_ft_ioam_gbp_action_get(
            uint8_t action_val,
            bcm_int_ft_group_action_info_t *action_list,
            int *action_idx
            )
{
    if (action_val & (1<<0)) {
        action_list[*action_idx].action = bcmFlowtrackerGroupActionIfaInsert;
        (*action_idx)++;
    }
    if (action_val & (1<<1)) {
        action_list[*action_idx].action = bcmFlowtrackerGroupActionIfaDelete;
        (*action_idx)++;
    }
    if (action_val & (1<<2)) {
        action_list[*action_idx].action = bcmFlowtrackerGroupActionINTResidenceTimeEnable;
        (*action_idx)++;
    }
    if (action_val & (1<<3)) {
        action_list[*action_idx].action = bcmFlowtrackerGroupActionVxlanGbpEnable;
        (*action_idx)++;
    }
}

static bcm_flowtracker_group_action_dest_type_t
bcm_ltsw_flowtracker_group_actn_val_dst_type_get(uint8_t action_val)
{
    if (action_val == 1) {
        return bcmFlowtrackerGroupActionDestTypeL2EgrIntf;
    } else if (action_val == 3) {
        return bcmFlowtrackerGroupActionDestTypeVp;
    } else if (action_val == 4) {
        return bcmFlowtrackerGroupActionDestTypeEcmp;
    } else if (action_val == 5) {
        return bcmFlowtrackerGroupActionDestTypeL3Egr;
    } else if (action_val == 6) {
        return bcmFlowtrackerGroupActionDestTypeL2Mcast;
    } else if (action_val == 7) {
        return bcmFlowtrackerGroupActionDestTypeL3Mcast;
    }
    return bcmFlowtrackerGroupActionDestTypeCount;
}

static uint8_t
bcm_ltsw_flowtracker_group_actn_dst_type_val_get(bcm_flowtracker_group_action_dest_type_t action_type)
{
    if (action_type ==bcmFlowtrackerGroupActionDestTypeL2EgrIntf) {
        return 1;
    } else if (action_type == bcmFlowtrackerGroupActionDestTypeVp) {
        return 3;
    } else if (action_type == bcmFlowtrackerGroupActionDestTypeEcmp) {
        return 4;
    } else if (action_type == bcmFlowtrackerGroupActionDestTypeL3Egr) {
        return 5;
    } else if (action_type == bcmFlowtrackerGroupActionDestTypeL2Mcast) {
        return 6;
    } else if (action_type == bcmFlowtrackerGroupActionDestTypeL3Mcast) {
        return 7;
    }
    return 0;
}

static int
bcm_ltsw_flowtracker_entry_get_action_info(int unit,
                                           bcmlt_entry_handle_t entry_hdl,
                                           uint32_t lt_num_actions,
                                           uint32_t action_flags,
                                           bcm_int_ft_group_action_info_t *action_list,
                                           uint16_t *conf_num_actions
                                           )
{
    uint64_t actions_val = 0;
    const char *actions_type_symbol = NULL;
    int i, action_idx = 0;
    uint32_t r_val_cnt = 0;
    bcmi_ltsw_gport_info_t gport_info;
    bcm_if_t if_id;
    SHR_FUNC_ENTER(unit);

    for (i = 0; i < lt_num_actions; i++) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_symbol_get(
                                                entry_hdl,
                                                ACTIONS_TYPEs,
                                                i,
                                                &actions_type_symbol,
                                                1,
                                                &r_val_cnt));

        if (bcm_ltsw_flowtracker_action_field_internal(actions_type_symbol)) {
            continue;
        }

        if(!sal_strcmp(actions_type_symbol, EM_FT_OPAQUE_OBJ0s)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(
                                             entry_hdl,
                                             ACTIONS_EM_FT_OPAQUE_OBJ0_VALs,
                                             i,
                                             &actions_val,
                                             1,
                                             &r_val_cnt));
            action_list[action_idx].action =
                bcmFlowtrackerGroupActionOpaqueObject;
            action_list[action_idx].params[0] = actions_val;
            action_idx++;
        } else if(!sal_strcmp(actions_type_symbol, FLEX_CTR_ACTIONs)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(
                                             entry_hdl,
                                             ACTIONS_FLEX_CTR_ACTION_VALs,
                                             i,
                                             &actions_val,
                                             1,
                                             &r_val_cnt));
            action_list[action_idx].action =
                bcmFlowtrackerGroupActionFlexCtrAssign;
            action_list[action_idx].params[0] = actions_val;
            action_idx++;
        } else if(!sal_strcmp(actions_type_symbol, EM_FT_IOAM_GBP_ACTIONs)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(
                                             entry_hdl,
                                             ACTIONS_EM_FT_IOAM_GBP_ACTION_VALs,
                                             i,
                                             &actions_val,
                                             1,
                                             &r_val_cnt));
            bcm_ltsw_flowtracker_grp_actn_em_ft_ioam_gbp_action_get(
                    actions_val, action_list, &action_idx);
        } else if(!sal_strcmp(actions_type_symbol, EM_FT_COPY_TO_CPUs)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(
                                             entry_hdl,
                                             ACTIONS_EM_FT_COPY_TO_CPU_VALs,
                                             i,
                                             &actions_val,
                                             1,
                                             &r_val_cnt));
            action_list[action_idx].action =
                bcmFlowtrackerGroupActionCopyToCpu;
            action_list[action_idx].params[0] = actions_val;
            action_idx++;
        } else if(!sal_strcmp(actions_type_symbol, EM_FT_DROP_ACTIONs)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(
                                             entry_hdl,
                                             ACTIONS_EM_FT_DROP_ACTION_VALs,
                                             i,
                                             &actions_val,
                                             1,
                                             &r_val_cnt));
            if (actions_val == 0) {
                action_list[action_idx].action =
                    bcmFlowtrackerGroupActionDropCancel;
            } else {
                action_list[action_idx].action =
                    bcmFlowtrackerGroupActionDrop;
            }
            action_idx++;
        } else if(!sal_strcmp(actions_type_symbol, DESTINATIONs)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(
                                             entry_hdl,
                                             ACTIONS_DESTINATION_VALs,
                                             i,
                                             &actions_val,
                                             1,
                                             &r_val_cnt));
            if (action_flags &
                    FT_GRP_ACTION_GPORT_MPLS) {
                action_list[action_idx].action =
                    bcmFlowtrackerGroupActionRedirectPort;
                gport_info.mpls_id = actions_val;
                gport_info.gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
            } else if (action_flags &
                    FT_GRP_ACTION_GPORT_WLAN) {
                action_list[action_idx].action =
                    bcmFlowtrackerGroupActionRedirectPort;
                gport_info.wlan_id = actions_val;
                gport_info.gport_type = _SHR_GPORT_TYPE_WLAN_PORT;
            } else if (action_flags &
                    FT_GRP_ACTION_GPORT_VXLAN) {
                action_list[action_idx].action =
                    bcmFlowtrackerGroupActionRedirectPort;
                gport_info.vxlan_id = actions_val;
                gport_info.gport_type = _SHR_GPORT_TYPE_VXLAN_PORT;
            } else if (action_flags &
                    FT_GRP_ACTION_GPORT_VLAN) {
                action_list[action_idx].action =
                    bcmFlowtrackerGroupActionRedirectPort;
                gport_info.vlan_vp_id = actions_val;
                gport_info.gport_type = _SHR_GPORT_TYPE_VLAN_PORT;
            } else if (action_flags &
                    FT_GRP_ACTION_GPORT_NIV) {
                action_list[action_idx].action =
                    bcmFlowtrackerGroupActionRedirectPort;
                gport_info.niv_id = actions_val;
                gport_info.gport_type = _SHR_GPORT_TYPE_NIV_PORT;
            } else if (action_flags &
                    FT_GRP_ACTION_GPORT_MIM) {
                action_list[action_idx].action =
                    bcmFlowtrackerGroupActionRedirectPort;
                gport_info.mim_id = actions_val;
                gport_info.gport_type = _SHR_GPORT_TYPE_MIM_PORT;
            } else if (action_flags &
                    FT_GRP_ACTION_GPORT_FLOW) {
                action_list[action_idx].action =
                    bcmFlowtrackerGroupActionRedirectPort;
                gport_info.flow_id = actions_val;
                gport_info.gport_type = _SHR_GPORT_TYPE_FLOW_PORT;
            } else if (action_flags &
                    FT_GRP_ACTION_GPORT_MODPORT) {
                action_list[action_idx].action =
                    bcmFlowtrackerGroupActionRedirectPort;
                gport_info.port = actions_val;
                gport_info.gport_type = _SHR_GPORT_TYPE_MODPORT;
            } else if (action_flags &
                    FT_GRP_ACTION_EGR_OBJ_OVERLAY) {
                action_list[action_idx].action =
                    bcmFlowtrackerGroupActionL3Switch;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_egress_obj_id_construct(unit, actions_val,
                                                          BCMI_LTSW_L3_EGR_OBJ_T_OL, &if_id));
                action_list[action_idx].params[0] = if_id;

            } else if (action_flags &
                    FT_GRP_ACTION_EGR_OBJ_ECMP_GROUP_OVERLAY) {
                action_list[action_idx].action =
                    bcmFlowtrackerGroupActionL3Switch;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_egress_obj_id_construct(unit, actions_val,
                                                          BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL, &if_id));
                action_list[action_idx].params[0] = if_id;
            } else if (action_flags & FT_GRP_ACTION_L2_MCAST) {
                action_list[action_idx].action =
                    bcmFlowtrackerGroupActionRedirectMcast;
                _BCM_MULTICAST_GROUP_SET(action_list[action_idx].params[0],
                        _BCM_MULTICAST_TYPE_L2, actions_val);

            } else if (action_flags & FT_GRP_ACTION_L3_MCAST) {
                action_list[action_idx].action =
                    bcmFlowtrackerGroupActionRedirectIpmc;
                _BCM_MULTICAST_GROUP_SET(action_list[action_idx].params[0],
                        _BCM_MULTICAST_TYPE_L3, actions_val);
            } else {
                SHR_ERR_EXIT(BCM_E_NOT_FOUND);
            }
            if (action_list[action_idx].action ==
                    bcmFlowtrackerGroupActionRedirectPort) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_gport_construct(unit, &gport_info,
                                       (bcm_gport_t *)&(action_list[action_idx].params[0])));
            }

            action_idx++;
        } else if(!sal_strcmp(actions_type_symbol, DESTINATION_TYPEs)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(
                                             entry_hdl,
                                             ACTIONS_DESTINATION_TYPE_VALs,
                                             i,
                                             &actions_val,
                                             1,
                                             &r_val_cnt));
            if (actions_val == 0) {
                action_list[action_idx].action =
                    bcmFlowtrackerGroupActionRedirectCancel;
            } else {
                action_list[action_idx].action =
                    bcmFlowtrackerGroupActionDestinationType;
            }
            action_list[action_idx].params[0] =
                bcm_ltsw_flowtracker_group_actn_val_dst_type_get(
                        actions_val);
            action_idx++;
        } else  if(!sal_strcmp(actions_type_symbol, EM_FT_FLEX_STATE_ACTIONs)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(
                                             entry_hdl,
                                             ACTIONS_EM_FT_FLEX_STATE_ACTION_VALs,
                                             i,
                                             &actions_val,
                                             1,
                                             &r_val_cnt));
            action_list[action_idx].action =
                bcmFlowtrackerGroupActionFlexStateAssign;
            action_list[action_idx].params[0] = actions_val;
            action_idx++;
        } else if(!sal_strcmp(actions_type_symbol, L2_IIF_SVP_MIRROR_INDEX_0s)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(
                                             entry_hdl,
                                             ACTIONS_L2_IIF_SVP_MIRROR_INDEX_0_VALs,
                                             i,
                                             &actions_val,
                                             1,
                                             &r_val_cnt));
            action_idx++;
        }
    }

    *conf_num_actions = action_idx;
exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_flowtracker_entry_set_action_info_internal_dest_valid(
                            int unit,
                            bcmlt_entry_handle_t entry_hdl,
                            int *action_idx)
{
    uint64_t actions_val = 0;
    const char *actions_type_symbol = NULL;
    SHR_FUNC_ENTER(unit);
    /*
     * Additionally configure PKT_FLOW_ELIGIBILITY to
     * destination valid.
     */
    actions_type_symbol = PKT_FLOW_ELIGIBILITYs;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_symbol_add(
                                            entry_hdl,
                                            ACTIONS_TYPEs,
                                            *action_idx,
                                            &actions_type_symbol,
                                            1));
    actions_val = (1 << 3); /* Destination valid bit setting */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(
                                     entry_hdl,
                                     ACTIONS_DESTINATION_VALs,
                                     *action_idx,
                                     &actions_val,
                                     1));
    (*action_idx)++;
exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_flowtracker_entry_set_action_info(int unit,
                                           bcmlt_entry_handle_t entry_hdl,
                                           uint16_t num_actions,
                                           bcm_int_ft_group_action_info_t *action_list,
                                           uint32_t *action_flags,
                                           uint32_t *r_num_actions)
{
    uint64_t actions_val = 0;
    const char *actions_type_symbol = NULL;
    int i, action_idx = 0;
    bcm_port_t lport;
    bcmi_ltsw_l3_egr_obj_type_t egr_obj_type;
    int egr_obj_idx;
    SHR_FUNC_ENTER(unit);

    for (i = 0; i < num_actions; i++) {
        /* Currently all actions only use param0. */
        actions_val = action_list[i].params[0];

        switch(action_list[i].action) {
            case bcmFlowtrackerGroupActionOpaqueObject:
                actions_type_symbol = EM_FT_OPAQUE_OBJ0s;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                                                        entry_hdl,
                                                        ACTIONS_TYPEs,
                                                        action_idx,
                                                        &actions_type_symbol,
                                                        1));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(
                                                 entry_hdl,
                                                 ACTIONS_EM_FT_OPAQUE_OBJ0_VALs,
                                                 action_idx,
                                                 &actions_val,
                                                 1));
                action_idx++;
                break;
            case bcmFlowtrackerGroupActionFlexCtrAssign:
                actions_type_symbol = FLEX_CTR_ACTIONs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                                                        entry_hdl,
                                                        ACTIONS_TYPEs,
                                                        action_idx,
                                                        &actions_type_symbol,
                                                        1));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(
                                                 entry_hdl,
                                                 ACTIONS_FLEX_CTR_ACTION_VALs,
                                                 action_idx,
                                                 &actions_val,
                                                 1));
                action_idx++;
                break;
            case bcmFlowtrackerGroupActionIfaInsert:
                actions_type_symbol = EM_FT_IOAM_GBP_ACTIONs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                                                        entry_hdl,
                                                        ACTIONS_TYPEs,
                                                        action_idx,
                                                        &actions_type_symbol,
                                                        1));
                actions_val = (1 << 0);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(
                                                 entry_hdl,
                                                 ACTIONS_EM_FT_IOAM_GBP_ACTION_VALs,
                                                 action_idx,
                                                 &actions_val,
                                                 1));
                action_idx++;
                break;
            case bcmFlowtrackerGroupActionIfaDelete:
                actions_type_symbol = EM_FT_IOAM_GBP_ACTIONs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                                                        entry_hdl,
                                                        ACTIONS_TYPEs,
                                                        action_idx,
                                                        &actions_type_symbol,
                                                        1));
                actions_val = (1 << 1);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(
                                                 entry_hdl,
                                                 ACTIONS_EM_FT_IOAM_GBP_ACTION_VALs,
                                                 action_idx,
                                                 &actions_val,
                                                 1));
                action_idx++;
                break;
            case bcmFlowtrackerGroupActionINTResidenceTimeEnable:
                actions_type_symbol = EM_FT_IOAM_GBP_ACTIONs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                                                        entry_hdl,
                                                        ACTIONS_TYPEs,
                                                        action_idx,
                                                        &actions_type_symbol,
                                                        1));
                actions_val = (1 << 2);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(
                                                 entry_hdl,
                                                 ACTIONS_EM_FT_IOAM_GBP_ACTION_VALs,
                                                 action_idx,
                                                 &actions_val,
                                                 1));
                action_idx++;
                break;
            case bcmFlowtrackerGroupActionVxlanGbpEnable:
                actions_type_symbol = EM_FT_IOAM_GBP_ACTIONs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                                                        entry_hdl,
                                                        ACTIONS_TYPEs,
                                                        action_idx,
                                                        &actions_type_symbol,
                                                        1));
                actions_val = (1 << 3);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(
                                                 entry_hdl,
                                                 ACTIONS_EM_FT_IOAM_GBP_ACTION_VALs,
                                                 action_idx,
                                                 &actions_val,
                                                 1));
                action_idx++;
                break;
            case bcmFlowtrackerGroupActionCopyToCpu:
                actions_type_symbol = EM_FT_COPY_TO_CPUs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                                                        entry_hdl,
                                                        ACTIONS_TYPEs,
                                                        action_idx,
                                                        &actions_type_symbol,
                                                        1));
                actions_val = 1;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(
                                                 entry_hdl,
                                                 ACTIONS_EM_FT_COPY_TO_CPU_VALs,
                                                 action_idx,
                                                 &actions_val,
                                                 1));
                action_idx++;
                break;
            case bcmFlowtrackerGroupActionDrop:
                actions_type_symbol = EM_FT_DROP_ACTIONs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                                                        entry_hdl,
                                                        ACTIONS_TYPEs,
                                                        action_idx,
                                                        &actions_type_symbol,
                                                        1));
                actions_val = 1;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(
                                                 entry_hdl,
                                                 ACTIONS_EM_FT_DROP_ACTION_VALs,
                                                 action_idx,
                                                 &actions_val,
                                                 1));
                action_idx++;
                break;
            case bcmFlowtrackerGroupActionDropCancel:
                actions_type_symbol = EM_FT_DROP_ACTIONs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                                                        entry_hdl,
                                                        ACTIONS_TYPEs,
                                                        action_idx,
                                                        &actions_type_symbol,
                                                        1));
                actions_val = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(
                                                 entry_hdl,
                                                 ACTIONS_EM_FT_DROP_ACTION_VALs,
                                                 action_idx,
                                                 &actions_val,
                                                 1));
                action_idx++;
                break;
            case bcmFlowtrackerGroupActionRedirectPort:
                if ((!BCM_GPORT_IS_MPLS_PORT(action_list[i].params[0])) &&
                        (!BCM_GPORT_IS_WLAN_PORT(action_list[i].params[0])) &&
                        (!BCM_GPORT_IS_VLAN_PORT(action_list[i].params[0])) &&
                        (!BCM_GPORT_IS_VXLAN_PORT(action_list[i].params[0])) &&
                        (!BCM_GPORT_IS_NIV_PORT(action_list[i].params[0])) &&
                        (!BCM_GPORT_IS_MIM_PORT(action_list[i].params[0])) &&
                        (!BCM_GPORT_IS_FLOW_PORT(action_list[i].params[0])) &&
                        (!BCM_GPORT_IS_MODPORT(action_list[i].params[0]))) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_gport_validate(unit, action_list[i].params[0], &lport));
                if (BCM_GPORT_IS_MPLS_PORT(action_list[i].params[0])) {
                    *action_flags |= FT_GRP_ACTION_GPORT_MPLS;
                } else if (BCM_GPORT_IS_WLAN_PORT(action_list[i].params[0])) {
                    *action_flags |= FT_GRP_ACTION_GPORT_WLAN;
                } else if (BCM_GPORT_IS_VXLAN_PORT(action_list[i].params[0])) {
                    *action_flags |= FT_GRP_ACTION_GPORT_VXLAN;
                } else if (BCM_GPORT_IS_VLAN_PORT(action_list[i].params[0])) {
                    *action_flags |= FT_GRP_ACTION_GPORT_VLAN;
                } else if (BCM_GPORT_IS_NIV_PORT(action_list[i].params[0])) {
                    *action_flags |= FT_GRP_ACTION_GPORT_NIV;
                } else if (BCM_GPORT_IS_MIM_PORT(action_list[i].params[0])) {
                    *action_flags |= FT_GRP_ACTION_GPORT_MIM;
                } else if (BCM_GPORT_IS_FLOW_PORT(action_list[i].params[0])) {
                    *action_flags |= FT_GRP_ACTION_GPORT_FLOW;
                } else {
                    *action_flags |= FT_GRP_ACTION_GPORT_MODPORT;
                }
                actions_val = lport;
                actions_type_symbol = DESTINATIONs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                                                        entry_hdl,
                                                        ACTIONS_TYPEs,
                                                        action_idx,
                                                        &actions_type_symbol,
                                                        1));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(
                                                 entry_hdl,
                                                 ACTIONS_DESTINATION_VALs,
                                                 action_idx,
                                                 &actions_val,
                                                 1));
                action_idx++;
                /* Call additional destination valid setting */
                SHR_IF_ERR_VERBOSE_EXIT(
                        bcm_ltsw_flowtracker_entry_set_action_info_internal_dest_valid(
                            unit,
                            entry_hdl,
                            &action_idx));
                break;
            case bcmFlowtrackerGroupActionRedirectIpmc:
                actions_type_symbol = DESTINATIONs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                                                        entry_hdl,
                                                        ACTIONS_TYPEs,
                                                        action_idx,
                                                        &actions_type_symbol,
                                                        1));
                if ((0 == BCM_MULTICAST_IS_SET(action_list[i].params[0])) ||
                        (!(_BCM_MULTICAST_IS_L3(action_list[i].params[0])))) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                actions_val = _BCM_MULTICAST_ID_GET(action_list[i].params[0]);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(
                                                 entry_hdl,
                                                 ACTIONS_DESTINATION_VALs,
                                                 action_idx,
                                                 &actions_val,
                                                 1));
                *action_flags |= FT_GRP_ACTION_L3_MCAST;
                action_idx++;
                /* Call additional destination valid setting */
                SHR_IF_ERR_VERBOSE_EXIT(
                        bcm_ltsw_flowtracker_entry_set_action_info_internal_dest_valid(
                            unit,
                            entry_hdl,
                            &action_idx));
                break;
            case bcmFlowtrackerGroupActionRedirectMcast:
                actions_type_symbol = DESTINATIONs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                                                        entry_hdl,
                                                        ACTIONS_TYPEs,
                                                        action_idx,
                                                        &actions_type_symbol,
                                                        1));
                if ((0 == BCM_MULTICAST_IS_SET(action_list[i].params[0])) ||
                        (!(_BCM_MULTICAST_IS_L2(action_list[i].params[0])))) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                actions_val = _BCM_MULTICAST_ID_GET(action_list[i].params[0]);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(
                                                 entry_hdl,
                                                 ACTIONS_DESTINATION_VALs,
                                                 action_idx,
                                                 &actions_val,
                                                 1));
                *action_flags |= FT_GRP_ACTION_L2_MCAST;
                action_idx++;
                /* Call additional destination valid setting */
                SHR_IF_ERR_VERBOSE_EXIT(
                        bcm_ltsw_flowtracker_entry_set_action_info_internal_dest_valid(
                            unit,
                            entry_hdl,
                            &action_idx));
                break;
            case bcmFlowtrackerGroupActionL3Switch:
                actions_type_symbol = DESTINATIONs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                                                        entry_hdl,
                                                        ACTIONS_TYPEs,
                                                        action_idx,
                                                        &actions_type_symbol,
                                                        1));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_egress_obj_id_resolve(unit, action_list[i].params[0],
                                                        &egr_obj_idx, &egr_obj_type));
                /* Check supported obj type */
                if (!((egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_OL) ||
                            (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL))) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                if (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_OL) {
                    *action_flags |=
                        FT_GRP_ACTION_EGR_OBJ_OVERLAY;
                } else {
                    *action_flags |=
                        FT_GRP_ACTION_EGR_OBJ_ECMP_GROUP_OVERLAY;
                }
                actions_val = egr_obj_idx;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(
                                                 entry_hdl,
                                                 ACTIONS_DESTINATION_VALs,
                                                 action_idx,
                                                 &actions_val,
                                                 1));
                action_idx++;
                /* Call additional destination valid setting */
                SHR_IF_ERR_VERBOSE_EXIT(
                        bcm_ltsw_flowtracker_entry_set_action_info_internal_dest_valid(
                            unit,
                            entry_hdl,
                            &action_idx));
                break;
            case bcmFlowtrackerGroupActionRedirectCancel:
                actions_type_symbol = DESTINATION_TYPEs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                                                        entry_hdl,
                                                        ACTIONS_TYPEs,
                                                        action_idx,
                                                        &actions_type_symbol,
                                                        1));
                actions_val = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(
                                                 entry_hdl,
                                                 ACTIONS_DESTINATION_TYPE_VALs,
                                                 action_idx,
                                                 &actions_val,
                                                 1));
                action_idx++;
                break;
            case bcmFlowtrackerGroupActionDestinationType:
                actions_type_symbol = DESTINATION_TYPEs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                                                        entry_hdl,
                                                        ACTIONS_TYPEs,
                                                        action_idx,
                                                        &actions_type_symbol,
                                                        1));
                actions_val =
                    bcm_ltsw_flowtracker_group_actn_dst_type_val_get(
                                                action_list[i].params[0]);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(
                                                 entry_hdl,
                                                 ACTIONS_DESTINATION_TYPE_VALs,
                                                 action_idx,
                                                 &actions_val,
                                                 1));
                action_idx++;
                break;
            case bcmFlowtrackerGroupActionFlexStateAssign:
                actions_type_symbol = EM_FT_FLEX_STATE_ACTIONs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                                                        entry_hdl,
                                                        ACTIONS_TYPEs,
                                                        action_idx,
                                                        &actions_type_symbol,
                                                        1));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(
                                                 entry_hdl,
                                                 ACTIONS_EM_FT_FLEX_STATE_ACTION_VALs,
                                                 action_idx,
                                                 &actions_val,
                                                 1));
                action_idx++;
                break;
            case bcmFlowtrackerGroupActionMirrorIndex:
                actions_type_symbol = L2_IIF_SVP_MIRROR_INDEX_0s;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_symbol_add(
                                                        entry_hdl,
                                                        ACTIONS_TYPEs,
                                                        action_idx,
                                                        &actions_type_symbol,
                                                        1));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(
                                                 entry_hdl,
                                                 ACTIONS_L2_IIF_SVP_MIRROR_INDEX_0_VALs,
                                                 action_idx,
                                                 &actions_val,
                                                 1));
                action_idx++;
                break;
            default:
                break;
        }
    }

    *r_num_actions = action_idx;
exit:
    SHR_FUNC_EXIT();
}

static
const char *bcm_ltsw_flowtracker_hw_learn_type_symbol_get(uint8_t hw_learn_en)
{
    if (hw_learn_en == FT_HW_LEARN_DISABLE) {
        return DISABLEs;
    } else if (hw_learn_en == FT_HW_LEARN_ENABLE) {
        return ENABLEs;
    } else if (hw_learn_en == FT_HW_LEARN_ENABLE_WO_EAPP) {
        return ENABLE_HARDWARE_ONLYs;
    }
    return DISABLEs;
}

/******************************************************************************
 * Public Functions
 */
int
bcm_ltsw_flowtracker_control_entry_get(int unit,
                                       bcm_int_ft_info_t *entry)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t max_flows_oper[BCM_PIPES_MAX] = {0};
    int i;
    uint32_t r_val_cnt = 0;
    uint64_t val = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_CONTROLs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, FLOWTRACKERs, &val));
    entry->enable = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, OPERATIONAL_STATEs, &val));
    if (val != 0) {
        /* Non zero indicates some sort of failure */
        entry->init = false;
    } else {
        /* Zero indicates success */
        entry->init = true;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, OBSERVATION_DOMAINs, &val));

    entry->observation_domain_id = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, HOST_MEM_OPERs, &val));

    entry->host_mem = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, SCAN_INTERVAL_USECS_OPERs, &val));

    entry->scan_interval_usecs = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, ETRAP_OPERs, &val));

    entry->elph_mode = val;

    entry->max_flows_total = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_get(entry_hdl,
                                     MAX_FLOWS_OPERs,
                                     0,
                                     max_flows_oper,
                                     BCM_PIPES_MAX,
                                     &r_val_cnt));
    for(i = 0; i < r_val_cnt; i++) {
        entry->max_flows[i] = max_flows_oper[i];
        entry->max_flows_total += max_flows_oper[i];
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, SCAN_INTERVAL_USECS_OPERs, &val));

    entry->scan_interval_usecs = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, FLOW_START_TIMESTAMP_ENABLE_OPERs, &val));

    entry->flow_start_ts = val;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_hw_learn_global_enable_set(int unit, bool enable)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    int rv = SHR_E_NONE;
    char *reg_str, *field_str;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    rv = mbcm_ltsw_flowtracker_hw_learn_global_enable_reg_field_name_get(unit,
                                                                        &reg_str,
                                                                    &field_str);
    if (rv != SHR_E_NONE) {
        /* Return silently since this chip does not need this configuration */
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, reg_str, &entry_hdl));

    if (enable) {
        val = enable;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, field_str, val));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_control_entry_set(int unit,
                                       bool update,
                                       bcm_int_ft_info_t *entry)
{
    int dunit, i;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    uint64_t max_flows[FLOWTRACKER_PIPES_MAX]={0};
    const char *hw_learn_sym;
    bool enable;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_CONTROLs, &entry_hdl));

    if (entry->enable) {
        enable = true;
    }
    val = enable;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOWTRACKERs, val));

    if (entry->hw_learn_en != FT_HW_LEARN_ENABLE_WO_EAPP) {
        val = entry->observation_domain_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, OBSERVATION_DOMAINs, val));

    }

    if (!update) {
        if (entry->hw_learn_en != FT_HW_LEARN_ENABLE_WO_EAPP) {
            val = entry->max_flow_groups;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, MAX_GROUPSs, val));

            val = entry->max_export_pkt_length;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, MAX_EXPORT_LENGTHs, val));

            val = entry->flex_counter_id;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, PKT_BYTE_CTR_ING_EFLEX_ACTION_PROFILE_IDs, val));

            val = entry->host_mem;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, HOST_MEMs, val));
        }

        if (entry->hw_learn_en == FT_HW_LEARN_ENABLE) {
            val = entry->db_wide_flex_counter_id;
            if (val) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(
                    entry_hdl,
                    DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_IDs,
                    val));
            }
            val = entry->qd_wide_flex_counter_id;
            if (val) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(
                    entry_hdl,
                    QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_IDs,
                    val));
            }
            val = entry->db_wide_flex_counter_grp_act_hw_idx;
            if (val) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(
                    entry_hdl,
                    DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTIONs,
                    val));
            }
            val = entry->qd_wide_flex_counter_grp_act_hw_idx;
            if (val) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(
                    entry_hdl,
                    QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTIONs,
                    val));
            }
        }

        if (entry->hw_learn_en != FT_HW_LEARN_ENABLE_WO_EAPP) {
            val = entry->elph_mode;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, ETRAPs, val));

            val = entry->scan_interval_usecs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, SCAN_INTERVAL_USECSs, val));

            val = entry->flow_start_ts;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, FLOW_START_TIMESTAMP_ENABLEs, val));
        }

        hw_learn_sym = bcm_ltsw_flowtracker_hw_learn_type_symbol_get(
                        entry->hw_learn_en);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(entry_hdl,
                                          HARDWARE_LEARNs,
                                          hw_learn_sym));

        for (i = 0; i < FLOWTRACKER_PIPES_MAX; i++) {
            max_flows[i] = entry->max_flows[i];
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add(entry_hdl,
                                         MAX_FLOWSs,
                                         0,
                                         max_flows,
                                         FLOWTRACKER_PIPES_MAX));
    }
    if (update == false) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_group_entry_get(int unit,
                                     bcm_int_ft_flow_group_info_t *entry)
{
    int dunit, i;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0, oper_state = 0;
    uint32_t r_val_cnt = 0;
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    uint64_t tracking_params_udf_ids[FLOWTRACKER_TRACKING_PARAMETERS_MAX]={0};
    const char *tracking_params_type_symbols[FLOWTRACKER_TRACKING_PARAMETERS_MAX];
    const char *export_triggers_type_symbols[FLOWTRACKER_EXPORT_TRIGGERS_MAX];
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_GROUPs, &entry_hdl));


    /* Key field */
    val = entry->flow_group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_GROUP_IDs, val));


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    /*
     * Currently global mode is only supported for EM.
     * So we take only field_group[0] and duplicate across pipes
     * In future, when we support per pipe mode, we have to change
     * SDKLT module to take more than field group.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, DT_EM_GRP_TEMPLATE_IDs, &val));
    for (i = 0; i < ft_info->num_pipes; i++) {
        entry->field_group[i] = val;
    }

    /* Tracking params */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NUM_TRACKING_PARAMETERSs, &val));
    entry->num_tracking_params = val;

    if (entry->num_tracking_params > 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_symbol_get(entry_hdl,
                                                TRACKING_PARAMETERS_TYPEs,
                                                0,tracking_params_type_symbols,
                                                entry->num_tracking_params,
                                                &r_val_cnt));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(entry_hdl,
                                         TRACKING_PARAMETERS_UDF_POLICY_IDs,
                                         0,
                                         tracking_params_udf_ids,
                                         entry->num_tracking_params,
                                         &r_val_cnt));
        for (i = 0; i < r_val_cnt; i++) {
            entry->tracking_params[i].param =
                bcm_ltsw_flowtracker_tracking_param_symbol_type_get(
                        tracking_params_type_symbols[i]);
            entry->tracking_params[i].udf_id = tracking_params_udf_ids[i];
        }
    }

    /* Export triggers */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NUM_EXPORT_TRIGGERSs, &val));
    entry->num_export_triggers = val;

    if (entry->num_export_triggers > 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_symbol_get(
                                                entry_hdl,
                                                EXPORT_TRIGGERSs,
                                                0,
                                                export_triggers_type_symbols,
                                                entry->num_export_triggers,
                                                &r_val_cnt));
        for (i = 0; i < r_val_cnt; i++) {
            entry->export_triggers[i] =
                bcm_ltsw_flowtracker_export_trigger_symbol_type_get(
                        export_triggers_type_symbols[i]);
        }
    }

    /* Actions */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NUM_ACTIONSs, &val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flowtracker_entry_get_action_info(
                        unit,
                        entry_hdl,
                        val,
                        ft_info->grp_action_flags[entry->flow_group_id],
                        entry->action_list,
                        &(entry->num_actions)));

    /* Flow limit */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, FLOW_LIMITs, &val));
    entry->flow_limit = val;

    /* Aging interval */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, AGING_INTERVAL_MSs, &val));
    entry->aging_interval_msecs = val;

    /* Elephant profile ID */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl,
                               MON_FLOWTRACKER_ELEPHANT_PROFILE_IDs,
                               &val));
    entry->elph_profile_id = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, LEARNs, &val));
    entry->enable = val;

    /* Operational state */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, OPERATIONAL_STATEs, &val));
    oper_state = val;

    /* Dont care about type of oper_state. Return E_INTERNAL. */
    if (oper_state != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_group_entry_set(int unit,
                                     bool update,
                                     bcm_int_ft_flow_group_info_t *entry)
{
    int dunit, i;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    uint64_t tracking_params_udf_ids[FLOWTRACKER_TRACKING_PARAMETERS_MAX]={0};
    const char *tracking_params_type_symbols[FLOWTRACKER_TRACKING_PARAMETERS_MAX];
    const char *export_triggers_type_symbols[FLOWTRACKER_EXPORT_TRIGGERS_MAX];
    uint32_t r_num_actions = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_GROUPs, &entry_hdl));


    /* Key field */
    val = entry->flow_group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_GROUP_IDs, val));

    if (ft_info->hw_learn_en != FT_HW_LEARN_ENABLE_WO_EAPP) {
        val = entry->enable;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, LEARNs, val));
    }

    if (update == false) {
        /*
         * Currently global mode is only supported for EM.
         * So we take only field_group[0]. In future,
         * when we support per pipe mode, we have to change
         * SDKLT module to take more than field group.
         */

        /*
         * BCM_FIELD_ID_MASK : Mask to get the actual group ID.
         */
        val = entry->field_group[0] & BCM_FIELD_ID_MASK;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, DT_EM_GRP_TEMPLATE_IDs, val));

    } else {

        /* Tracking params */
        val = entry->num_tracking_params;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, NUM_TRACKING_PARAMETERSs, val));

        if (entry->num_tracking_params > 0) {
            for (i = 0; i < entry->num_tracking_params; i++) {
                tracking_params_type_symbols[i] =
                    bcm_ltsw_flowtracker_tracking_param_type_symbol_get(
                            entry->tracking_params[i].param);
                tracking_params_udf_ids[i] = entry->tracking_params[i].udf_id;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_symbol_add(
                                            entry_hdl,
                                            TRACKING_PARAMETERS_TYPEs,
                                            0,tracking_params_type_symbols,
                                            entry->num_tracking_params));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(entry_hdl,
                                             TRACKING_PARAMETERS_UDF_POLICY_IDs,
                                             0,
                                             tracking_params_udf_ids,
                                             entry->num_tracking_params));
        }

        /* Export triggers */
        val = entry->num_export_triggers;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, NUM_EXPORT_TRIGGERSs, val));

        if (entry->num_export_triggers > 0) {
            for (i = 0; i < entry->num_export_triggers; i++) {
                export_triggers_type_symbols[i] =
                    bcm_ltsw_flowtracker_export_trigger_type_symbol_get(
                            entry->export_triggers[i]);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_symbol_add(
                                                   entry_hdl,
                                                   EXPORT_TRIGGERSs,
                                                   0,
                                                   export_triggers_type_symbols,
                                                   entry->num_export_triggers));
        }

        /* Actions */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_flowtracker_entry_set_action_info(
                                    unit,
                                    entry_hdl,
                                    entry->num_actions,
                                    entry->action_list,
                                    &(ft_info->grp_action_flags[entry->flow_group_id]),
                                    &r_num_actions));
        val = r_num_actions;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, NUM_ACTIONSs, val));

        /* Flow limit */
        val = entry->flow_limit;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, FLOW_LIMITs, val));

        /* Aging interval */
        val = entry->aging_interval_msecs;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, AGING_INTERVAL_MSs, val));

        /* Elephant profile ID */
        val = entry->elph_profile_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl,
                                   MON_FLOWTRACKER_ELEPHANT_PROFILE_IDs,
                                   val));
    }

    if (update == false) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_group_entry_delete(
                                    int unit,
                                    bcm_flowtracker_group_t flow_group_id)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_GROUPs, &entry_hdl));


    /* Key field */
    val = flow_group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_GROUP_IDs, val));


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_template_transmit_config_entry_get(int unit,
    bcm_int_ft_template_transmit_config_t *entry)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_EXPORT_TEMPLATEs, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_EXPORT_TEMPLATE_IDs,
                               entry->export_template_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, TRANSMIT_INTERVALs, &val));
    entry->transmit_interval = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, INITIAL_BURSTs, &val));
    entry->initial_burst = val;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}


int bcm_ltsw_flowtracker_export_template_entry_update(int unit,
    bcm_int_ft_template_transmit_config_t *entry)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_EXPORT_TEMPLATEs, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_EXPORT_TEMPLATE_IDs,
                               entry->export_template_id));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, TRANSMIT_INTERVALs, entry->transmit_interval));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, INITIAL_BURSTs, entry->initial_burst));

    if (entry->collector_type == LTSW_COLLECTOR_TYPE_IPV4_UDP) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry_hdl, COLLECTOR_TYPEs, "IPV4"));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, MON_COLLECTOR_IPV4_IDs,
                                   entry->collector_ipv4_id));
    } else if (entry->collector_type == LTSW_COLLECTOR_TYPE_IPV6_UDP) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry_hdl, COLLECTOR_TYPEs, "IPV6"));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, MON_COLLECTOR_IPV6_IDs,
                                   entry->collector_ipv6_id));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_group_status_get(int unit,
                                     bcm_int_ft_flow_group_info_t *entry)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_GROUP_STATUSs, &entry_hdl));


    /* Key field */
    val = entry->flow_group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_GROUP_IDs, val));


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Flow count */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, FLOW_COUNTs, &val));
    entry->flow_count = val;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_group_collector_map_entry_set(int unit,
        bool update,
        bcm_int_ft_group_col_map_t *entry)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_GROUP_COLLECTOR_MAPs,
                              &entry_hdl));

    val = entry->group_col_map_id;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_GROUP_COLLECTOR_MAP_IDs,
                               val));

    val = entry->ft_group_id;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_GROUP_IDs, val));

    if (entry->collector_type == LTSW_COLLECTOR_TYPE_IPV4_UDP) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry_hdl, COLLECTOR_TYPEs, "IPV4"));

        val = entry->collector_ipv4_id;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, MON_COLLECTOR_IPV4_IDs,
                                   val));
    } else if (entry->collector_type == LTSW_COLLECTOR_TYPE_IPV6_UDP) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry_hdl, COLLECTOR_TYPEs, "IPV6"));

        val = entry->collector_ipv6_id;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, MON_COLLECTOR_IPV6_IDs,
                                   val));
    }

    val = entry->export_profile_id;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_EXPORT_PROFILE_IDs,
                               val));

    val = entry->export_template_id;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_EXPORT_TEMPLATE_IDs,
                               val));

    if (update == false) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_INSERT,
                                  BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_UPDATE,
                                  BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_group_collector_map_delete(int unit,
                                     int flow_group_id)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_GROUP_COLLECTOR_MAPs, &entry_hdl));


    /* Key field */
    val = flow_group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_GROUP_COLLECTOR_MAP_IDs, val));


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_is_collector_attached( int unit, uint32_t flow_group_id)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t value;
    bool group_found = false;
    const char* str_value;
    uint64_t col_id = 0;
    uint32_t rv;

    SHR_FUNC_ENTER(unit);
    /* Traverse through MON_FLOWTRACKER_GROUP_COLLECTOR_MAP and check if collector is already attached*/
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(unit, MON_FLOWTRACKER_GROUP_COLLECTOR_MAPs, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_TRAVERSE,
                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, MON_FLOWTRACKER_GROUP_IDs, &value));

        if (value == flow_group_id) {
            group_found = true;
            break;
        }
    }

    if (group_found == true) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_get(entry_hdl,
                                          COLLECTOR_TYPEs, &str_value));

        if (!sal_strcmp(str_value, "IPV4")) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, MON_COLLECTOR_IPV4_IDs, &col_id));
        } else if (!sal_strcmp(str_value, "IPV6")) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, MON_COLLECTOR_IPV6_IDs, &col_id));
        }

        /* Check if the flow group is already attached to a collector */
        if (col_id != FT_COLLECTOR_INVALID_COLLECTOR_ID) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_elephant_profile_entry_set(int unit,
        bool update,
        int *id,
        uint32_t options,
        bcm_flowtracker_elephant_profile_info_t *entry)
{
    int dunit, i;
    uint64_t incr_rate[FT_ELEPHANT_MAX_PROMOTION_FILTERS];
    uint64_t monitor_interval_usecs[FT_ELEPHANT_MAX_PROMOTION_FILTERS];
    uint64_t rate_low_threshold_kbits_sec[FT_ELEPHANT_MAX_PROMOTION_FILTERS];
    uint64_t rate_high_threshold_kbits_sec[FT_ELEPHANT_MAX_PROMOTION_FILTERS];
    uint64_t size_threshold_bytes[FT_ELEPHANT_MAX_PROMOTION_FILTERS];
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_ELEPHANT_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_ELEPHANT_PROFILE_IDs, *id));

    if(entry->num_promotion_filters > 0) {
        for(i=0; i < entry->num_promotion_filters; i++) {
            if (options & BCM_FLOWTRACKER_ELEPHANT_FILTER_FLAGS_INCREASING_RATE) {
                incr_rate[i] = true;
            }
            monitor_interval_usecs[i] =
                entry->promotion_filters[i].monitor_interval_usecs;
            rate_low_threshold_kbits_sec[i] =
                entry->promotion_filters[i].rate_low_threshold_kbits_sec;
            rate_high_threshold_kbits_sec[i] =
                entry->promotion_filters[i].rate_high_threshold_kbits_sec;
            size_threshold_bytes[i] =
                entry->promotion_filters[i].size_threshold_bytes;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, NUM_PROMOTION_FILTERSs,
                                   entry->num_promotion_filters));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add(entry_hdl,
                                         PROMOTION_FILTERS_INCR_RATEs,
                                         0,
                                         incr_rate,
                                         entry->num_promotion_filters));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add(entry_hdl,
                                         PROMOTION_FILTERS_MONITOR_INTERVAL_USECSs,
                                         0,
                                         monitor_interval_usecs,
                                         entry->num_promotion_filters));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add(entry_hdl,
                                         PROMOTION_FILTERS_RATE_HIGH_THRESHOLD_KBITS_SECs,
                                         0,
                                         rate_low_threshold_kbits_sec,
                                         entry->num_promotion_filters));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add(entry_hdl,
                                         PROMOTION_FILTERS_RATE_LOW_THRESHOLD_KBITS_SECs,
                                         0,
                                         rate_high_threshold_kbits_sec,
                                         entry->num_promotion_filters));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add(entry_hdl,
                                         PROMOTION_FILTERS_SIZE_THRESHOLD_BYTESs,
                                         0,
                                         size_threshold_bytes,
                                         entry->num_promotion_filters));
    }

    if (options & BCM_FLOWTRACKER_ELEPHANT_FILTER_FLAGS_INCREASING_RATE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, DEMOTION_FILTER_INCR_RATEs, true));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, DEMOTION_FILTER_MONITOR_INTERVAL_USECSs,
                               entry->demotion_filter.monitor_interval_usecs));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, DEMOTION_FILTER_RATE_HIGH_THRESHOLD_KBITS_SECs,
                               entry->demotion_filter.rate_low_threshold_kbits_sec));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, DEMOTION_FILTER_RATE_LOW_THRESHOLD_KBITS_SECs,
                               entry->demotion_filter.rate_high_threshold_kbits_sec));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, DEMOTION_FILTER_SIZE_THRESHOLD_BYTESs,
                               entry->demotion_filter.size_threshold_bytes));

    if (update == false) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_INSERT,
                                  BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_UPDATE,
                                  BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_group_flow_data_get(
                                        int unit,
                                        bcm_flowtracker_group_t flow_group_id,
                                        bcm_flowtracker_flow_key_t *flow_key,
                                        bcm_flowtracker_flow_data_t *flow_data)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0, ipv6_u64[2];
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_FLOW_DATAs, &entry_hdl));


    /* Key fields */
    val = flow_group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_GROUP_IDs, val));

    val = flow_key->src_ip.addr.ipv4_addr;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_SRC_IPV4s, val));

    val = flow_key->dst_ip.addr.ipv4_addr;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_DST_IPV4s, val));

    val = flow_key->l4_src_port;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_SRC_L4_PORTs, val));

    val = flow_key->l4_dst_port;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_DST_L4_PORTs, val));

    val = flow_key->ip_protocol;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_IP_PROTOs, val));

    val = flow_key->inner_src_ip.addr.ipv4_addr;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_INNER_SRC_IPV4s, val));

    val = flow_key->inner_dst_ip.addr.ipv4_addr;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_INNER_DST_IPV4s, val));

    bcmi_ltsw_util_ip6_to_uint64(ipv6_u64, &flow_key->inner_src_ip.addr.ipv6_addr);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_INNER_SRC_IPV6_UPPERs, ipv6_u64[1]));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_INNER_SRC_IPV6_LOWERs, ipv6_u64[0]));

    bcmi_ltsw_util_ip6_to_uint64(ipv6_u64, &flow_key->inner_dst_ip.addr.ipv6_addr);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_INNER_DST_IPV6_UPPERs, ipv6_u64[1]));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_INNER_DST_IPV6_LOWERs, ipv6_u64[0]));

    val = flow_key->inner_l4_src_port;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_INNER_SRC_L4_PORTs, val));

    val = flow_key->inner_l4_dst_port;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_INNER_DST_L4_PORTs, val));

    val = flow_key->inner_ip_protocol;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_INNER_IP_PROTOs, val));

    val = flow_key->vxlan_network_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_VNIDs, val));

    sal_memcpy(&val, &(flow_key->custom[0]), 8);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_CUSTOM_KEY_LOWERs, val));

    sal_memcpy(&val, &(flow_key->custom[8]), 8);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_CUSTOM_KEY_UPPERs, val));

    val = flow_key->in_port;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_ING_PORT_IDs, val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Flow Data */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, DATA_PKT_COUNTs, &val));
    flow_data->packet_count = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, DATA_BYTE_COUNTs, &val));
    flow_data->byte_count = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, DATA_FLOW_START_TIMESTAMP_MSECSs,
                               &val));
    flow_data->flow_start_timestamp_msecs = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, DATA_OBSERVATION_TIMESTAMP_MSECSs,
                               &val));
    flow_data->observation_timestamp_msecs = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, STATICs,
                               &val));
    flow_data->is_static = val;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}


int bcm_ltsw_flowtracker_is_elephant_profile_used( int unit, uint32_t id, bool *is_used)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t value;
    uint32_t rv;

    SHR_FUNC_ENTER(unit);

    *is_used = false;

    /* Traverse through MON_FLOWTRACKER_GROUP and check if elephant profile is used */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(unit, MON_FLOWTRACKER_GROUPs, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_TRAVERSE,
                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, MON_FLOWTRACKER_ELEPHANT_PROFILE_IDs, &value));

        if (value == id) {
            *is_used = true;
            break;
        }
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_group_flow_data_clear_all(
                                        int unit,
                                        bcm_flowtracker_group_t flow_group_id)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv = SHR_E_NONE;
    uint64_t val = 0;
    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_FLOW_DATAs, &entry_hdl));

    while ((rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, MON_FLOWTRACKER_GROUP_IDs,
                                   &val));

        if (flow_group_id != val) {
            /* Not part of the group that we are looking for */
            continue;
        }
        /* Update is used to clear the data fields */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_elephant_profile_entry_delete(int unit,
        int id)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_ELEPHANT_PROFILEs, &entry_hdl));


    /* Key field */
    val = id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_ELEPHANT_PROFILE_IDs, val));


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_elephant_profile_entry_get(int unit,
        int id,
        bcm_flowtracker_elephant_profile_info_t *entry)
{
    int dunit, i;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    uint32_t r_val_cnt = 0;
    uint64_t incr_rate[FT_ELEPHANT_MAX_PROMOTION_FILTERS];
    uint64_t monitor_interval_usecs[FT_ELEPHANT_MAX_PROMOTION_FILTERS];
    uint64_t rate_low_threshold_kbits_sec[FT_ELEPHANT_MAX_PROMOTION_FILTERS];
    uint64_t rate_high_threshold_kbits_sec[FT_ELEPHANT_MAX_PROMOTION_FILTERS];
    uint64_t size_threshold_bytes[FT_ELEPHANT_MAX_PROMOTION_FILTERS];

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_ELEPHANT_PROFILEs, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_ELEPHANT_PROFILE_IDs,
                               id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NUM_PROMOTION_FILTERSs, &val));
    entry->num_promotion_filters = val;

    if (entry->num_promotion_filters > 0) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(entry_hdl,
                                         PROMOTION_FILTERS_INCR_RATEs,
                                         0,
                                         incr_rate,
                                         entry->num_promotion_filters,
                                         &r_val_cnt));
        for (i = 0; i < r_val_cnt; i++) {
            if(incr_rate[i]) {
                entry->promotion_filters[i].flags |=
                    BCM_FLOWTRACKER_ELEPHANT_FILTER_FLAGS_INCREASING_RATE;
            }
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(entry_hdl,
                                         PROMOTION_FILTERS_MONITOR_INTERVAL_USECSs,
                                         0,
                                         monitor_interval_usecs,
                                         entry->num_promotion_filters,
                                         &r_val_cnt));
        for (i = 0; i < r_val_cnt; i++) {
            entry->promotion_filters[i].monitor_interval_usecs =
                monitor_interval_usecs[i];
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(entry_hdl,
                                         PROMOTION_FILTERS_RATE_LOW_THRESHOLD_KBITS_SECs,
                                         0,
                                         rate_low_threshold_kbits_sec,
                                         entry->num_promotion_filters,
                                         &r_val_cnt));
        for (i = 0; i < r_val_cnt; i++) {
            entry->promotion_filters[i].rate_low_threshold_kbits_sec =
                rate_low_threshold_kbits_sec[i];
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(entry_hdl,
                                         PROMOTION_FILTERS_RATE_HIGH_THRESHOLD_KBITS_SECs,
                                         0,
                                         rate_high_threshold_kbits_sec,
                                         entry->num_promotion_filters,
                                         &r_val_cnt));
        for (i = 0; i < r_val_cnt; i++) {
            entry->promotion_filters[i].rate_high_threshold_kbits_sec =
                rate_high_threshold_kbits_sec[i];
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(entry_hdl,
                                         PROMOTION_FILTERS_SIZE_THRESHOLD_BYTESs,
                                         0,
                                         size_threshold_bytes,
                                         entry->num_promotion_filters,
                                         &r_val_cnt));
        for (i = 0; i < r_val_cnt; i++) {
            entry->promotion_filters[i].size_threshold_bytes =
                size_threshold_bytes[i];
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, DEMOTION_FILTER_INCR_RATEs, &val));
    if(val) {
        entry->demotion_filter.flags |= BCM_FLOWTRACKER_ELEPHANT_FILTER_FLAGS_INCREASING_RATE;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, DEMOTION_FILTER_MONITOR_INTERVAL_USECSs,
                               &val));
    entry->demotion_filter.monitor_interval_usecs = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, DEMOTION_FILTER_RATE_HIGH_THRESHOLD_KBITS_SECs,
                               &val));
    entry->demotion_filter.rate_low_threshold_kbits_sec = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, DEMOTION_FILTER_RATE_LOW_THRESHOLD_KBITS_SECs,
                               &val));
    entry->demotion_filter.rate_high_threshold_kbits_sec = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, DEMOTION_FILTER_SIZE_THRESHOLD_BYTESs,
                               &val));
    entry->demotion_filter.size_threshold_bytes = val;
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_export_template_entry_insert(int unit,
        bcm_int_ft_export_template_info_t *entry)
{
    int dunit, i;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char* type[FT_TEMPLATE_MAX_INFO_ELEMENTS];
    uint64_t data_size[FT_TEMPLATE_MAX_INFO_ELEMENTS];
    uint64_t enterprise[FT_TEMPLATE_MAX_INFO_ELEMENTS];
    uint64_t enterprise_id[FT_TEMPLATE_MAX_INFO_ELEMENTS];

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_EXPORT_TEMPLATEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_EXPORT_TEMPLATE_IDs, entry->template_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, SET_IDs, entry->set_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, NUM_EXPORT_ELEMENTSs, entry->num_export_elements));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TRANSMIT_INTERVALs, entry->interval_secs));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, INITIAL_BURSTs, entry->initial_burst));

    if ( entry->num_export_elements > 0) {
        for(i=0; i < entry->num_export_elements; i++) {
            type[i] = bcm_ltsw_flowtracker_export_element_type_symbol_get(
                      entry->elements[i].element);
            data_size[i] = entry->elements[i].data_size;
            enterprise[i] = entry->elements[i].enterprise ;
            enterprise_id[i] = entry->elements[i].id;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_symbol_add(entry_hdl,
                                         EXPORT_ELEMENTS_TYPEs,
                                         0,
                                         type,
                                         entry->num_export_elements));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add(entry_hdl,
                                         EXPORT_ELEMENTS_DATA_SIZEs,
                                         0,
                                         data_size,
                                         entry->num_export_elements));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add(entry_hdl,
                                         EXPORT_ELEMENTS_ENTERPRISEs,
                                         0,
                                         enterprise,
                                         entry->num_export_elements));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add(entry_hdl,
                                         EXPORT_ELEMENTS_ENTERPRISE_IDs,
                                         0,
                                         enterprise_id,
                                         entry->num_export_elements));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_is_export_template_used( int unit, uint32_t id, bool *is_used)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t value;
    uint32_t rv;

    SHR_FUNC_ENTER(unit);

    *is_used = false;
    /* Traverse through MON_FLOWTRACKER_GROUP_COLLECTOR_MAP and
       check if export template is used */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(unit, MON_FLOWTRACKER_GROUP_COLLECTOR_MAPs,
        &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_TRAVERSE,
                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, MON_EXPORT_PROFILE_IDs, &value));

        if (value == id) {
            *is_used = true;
            break;
        }
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flowtracker_export_template_entry_get(int unit,
        int id,
        bcm_int_ft_export_template_info_t *entry)
{
    int dunit, i;
    uint64_t val = 0;
    uint32_t r_val_cnt = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *type[FT_TEMPLATE_MAX_INFO_ELEMENTS];
    uint64_t data_size[FT_TEMPLATE_MAX_INFO_ELEMENTS];
    uint64_t enterprise[FT_TEMPLATE_MAX_INFO_ELEMENTS];
    uint64_t enterprise_id[FT_TEMPLATE_MAX_INFO_ELEMENTS];

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_EXPORT_TEMPLATEs, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_EXPORT_TEMPLATE_IDs,
                               id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, SET_IDs, &val));
    entry->set_id = val;


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NUM_EXPORT_ELEMENTSs, &val));
    entry->num_export_elements = val;

    if (entry->num_export_elements > 0) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_symbol_get(entry_hdl,
                                         EXPORT_ELEMENTS_TYPEs,
                                         0,
                                         type,
                                         entry->num_export_elements,
                                         &r_val_cnt));
        for (i = 0; i < r_val_cnt; i++) {
            entry->elements[i].element = bcm_ltsw_flowtracker_export_element_symbol_type_get(type[i]);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(entry_hdl,
                                         EXPORT_ELEMENTS_DATA_SIZEs,
                                         0,
                                         data_size,
                                         entry->num_export_elements,
                                         &r_val_cnt));
        for (i = 0; i < r_val_cnt; i++) {
            entry->elements[i].data_size = data_size[i];
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(entry_hdl,
                                         EXPORT_ELEMENTS_ENTERPRISEs,
                                         0,
                                         enterprise,
                                         entry->num_export_elements,
                                         &r_val_cnt));
        for (i = 0; i < r_val_cnt; i++) {
            entry->elements[i].enterprise = enterprise[i];
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(entry_hdl,
                                         EXPORT_ELEMENTS_ENTERPRISE_IDs,
                                         0,
                                         enterprise,
                                         entry->num_export_elements,
                                         &r_val_cnt));
        for (i = 0; i < r_val_cnt; i++) {
            entry->elements[i].id = enterprise_id[i];
        }
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_export_template_entry_delete(
        int unit,
        bcm_flowtracker_export_template_t id)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_EXPORT_TEMPLATEs, &entry_hdl));

    /* Key field */
    val = id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_EXPORT_TEMPLATE_IDs, val));


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}


int bcm_ltsw_flowtracker_group_collector_map_entry_get(int unit,
        bcm_int_ft_group_col_map_t *entry)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char* str_value;
    uint64_t val = 0, oper_state = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_GROUP_COLLECTOR_MAPs,
                              &entry_hdl));

    val = entry->group_col_map_id;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_GROUP_COLLECTOR_MAP_IDs,
                               val));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MON_FLOWTRACKER_GROUP_IDs, &val));
    entry->ft_group_id = val;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, MON_EXPORT_PROFILE_IDs,
                               &val ));
    entry->export_profile_id = val;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, MON_FLOWTRACKER_EXPORT_TEMPLATE_IDs,
                               &val ));
    entry->export_template_id = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl,
                                      COLLECTOR_TYPEs, &str_value));

    if (!sal_strcmp(str_value, "IPV4")) {
        entry->collector_type = LTSW_COLLECTOR_TYPE_IPV4_UDP;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, MON_COLLECTOR_IPV4_IDs, &val));
        entry->collector_ipv4_id = val;
    } else if (!sal_strcmp(str_value, "IPV6")) {
        entry->collector_type = LTSW_COLLECTOR_TYPE_IPV6_UDP;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, MON_COLLECTOR_IPV6_IDs, &val));
        entry->collector_ipv6_id = val;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, OPERATIONAL_STATUSs, &val));
    oper_state = val;


    /* Dont care about type of oper_state. Return E_INTERNAL. */
    if (oper_state != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_flowtracker_group_static_flow_entry_set(
                                int unit,
                                bool delete,
                                bcm_flowtracker_group_t flow_group_id,
                                bcm_flowtracker_flow_key_t *flow_key)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_FLOW_STATICs, &entry_hdl));


    /* Key fields */
    val = flow_group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_GROUP_IDs, val));

    val = flow_key->src_ip.addr.ipv4_addr;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_SRC_IPV4s, val));

    val = flow_key->dst_ip.addr.ipv4_addr;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_DST_IPV4s, val));

    val = flow_key->l4_src_port;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_SRC_L4_PORTs, val));

    val = flow_key->l4_dst_port;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_DST_L4_PORTs, val));

    val = flow_key->ip_protocol;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_IP_PROTOs, val));

    val = flow_key->inner_src_ip.addr.ipv4_addr;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_INNER_SRC_IPV4s, val));

    val = flow_key->inner_dst_ip.addr.ipv4_addr;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_INNER_DST_IPV4s, val));

    val = flow_key->inner_l4_src_port;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_INNER_SRC_L4_PORTs, val));

    val = flow_key->inner_l4_dst_port;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_INNER_DST_L4_PORTs, val));

    val = flow_key->inner_ip_protocol;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_INNER_IP_PROTOs, val));

    val = flow_key->vxlan_network_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_VNIDs, val));

    sal_memcpy(&val, &(flow_key->custom[0]), 8);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_CUSTOM_KEY_LOWERs, val));

    sal_memcpy(&val, &(flow_key->custom[8]), 8);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_CUSTOM_KEY_UPPERs, val));

    val = flow_key->in_port;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FLOW_KEY_ING_PORT_IDs, val));


    if (delete == true) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_DELETE,
                                  BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_INSERT,
                                  BCMLT_PRIORITY_NORMAL));
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_group_static_flow_entry_get_all(
                                int unit,
                                bcm_flowtracker_group_t flow_group_id,
                                int max_size,
                                bcm_flowtracker_flow_key_t *flow_key,
                                int *list_size)
{
    int dunit,rv;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int count = 0;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_FLOW_STATICs, &entry_hdl));


    while ((rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_TRAVERSE,
                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, MON_FLOWTRACKER_GROUP_IDs,
                                   &val));

        if (flow_group_id != val) {
            /* Not part of the group that we are looking for */
            continue;
        }

        if (max_size > 0) {

            if (count >= max_size) {
                break;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, FLOW_KEY_SRC_IPV4s, &val));
            flow_key[count].src_ip.addr.ipv4_addr = val;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, FLOW_KEY_DST_IPV4s, &val));
            flow_key[count].dst_ip.addr.ipv4_addr = val;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, FLOW_KEY_SRC_L4_PORTs, &val));
            flow_key[count].l4_src_port = val;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, FLOW_KEY_DST_L4_PORTs, &val));
            flow_key[count].l4_dst_port = val;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, FLOW_KEY_IP_PROTOs, &val));
            flow_key[count].ip_protocol = val;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, FLOW_KEY_INNER_SRC_IPV4s,
                                       &val));
            flow_key[count].inner_src_ip.addr.ipv4_addr = val;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, FLOW_KEY_INNER_DST_IPV4s,
                                       &val));
            flow_key[count].inner_dst_ip.addr.ipv4_addr = val;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, FLOW_KEY_INNER_SRC_L4_PORTs,
                                       &val));
            flow_key[count].inner_l4_src_port = val;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, FLOW_KEY_INNER_DST_L4_PORTs,
                                       &val));
            flow_key[count].inner_l4_dst_port = val;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, FLOW_KEY_INNER_IP_PROTOs,
                                       &val));
            flow_key[count].inner_ip_protocol = val;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, FLOW_KEY_VNIDs, &val));
            flow_key[count].vxlan_network_id = val;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, FLOW_KEY_CUSTOM_KEY_LOWERs,
                                       &val));
            sal_memcpy(&(flow_key[count].custom[0]), &val, 8);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, FLOW_KEY_CUSTOM_KEY_UPPERs,
                                       &val));
            sal_memcpy(&(flow_key[count].custom[8]), &val, 8);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, FLOW_KEY_ING_PORT_IDs, &val));
            flow_key[count].in_port = val;
        }
        count++;
    }

    *list_size = count;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_group_static_flow_entry_delete_all(
                                int unit,
                                bcm_flowtracker_group_t flow_group_id)
{
    int dunit, rv;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_FLOW_STATICs, &entry_hdl));


    while ((rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_TRAVERSE,
                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, MON_FLOWTRACKER_GROUP_IDs,
                                   &val));

        if (flow_group_id != val) {
            /* Not part of the group that we are looking for */
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_DELETE,
                                  BCMLT_PRIORITY_NORMAL));
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_hw_learn_flow_action_control_set(
            int unit,
            ft_hw_learn_flow_cmd_type_t cmd_type,
            bcm_flowtracker_group_t id,
            bcm_flowtracker_group_flow_action_info_t *action_info,
            int num_actions,
            bcm_int_ft_group_action_info_t *action_list)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    const char *exact_match_idx_mode = NULL;
    const char *cmd = NULL;
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    uint32_t action_flags = 0, r_num_actions = 0;
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLs, &entry_hdl));


    /* Key fields */
    val = action_info->pipe_idx;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PIPEs, val));

    val = action_info->exact_match_idx;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, EXACT_MATCH_INDEXs, val));

    /* Data fields */
    exact_match_idx_mode = bcm_ltsw_flowtracker_exact_match_idx_mode_symbol_get(
            action_info->mode);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, MODEs, exact_match_idx_mode));

    val = id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_FLOWTRACKER_GROUP_IDs, val));

    if (cmd_type == FT_HW_LEARN_FLOW_CMD_DELETE) {
        cmd = DELETEs;
    } else if (cmd_type == FT_HW_LEARN_FLOW_CMD_MODIFY){
        cmd = MODIFYs;
        /* Handle actions if cmd = MODIFY */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_flowtracker_entry_set_action_info(
                                                        unit,
                                                        entry_hdl,
                                                        num_actions,
                                                        action_list,
                                                        &action_flags,
                                                        &r_num_actions));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, CMDs, cmd));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flowtracker_hw_learn_flow_action_state_get(
            int unit,
            uint8_t pipe_idx)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    bcmltd_common_flowtracker_hw_learn_flow_action_state_t_t state;
    bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    SHR_FUNC_ENTER(unit);

    if (ft_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATEs, &entry_hdl));


    /* Key fields */
    val = pipe_idx;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PIPEs, val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    /* Data fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, STATEs, &val));
    state = val;

    if (state ==
            BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATE_T_T_NOT_SUPPORTED) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    } else if (state ==
            BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATE_T_T_FAILURE) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    } else if (state ==
            BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATE_T_T_ACTION_MISMATCH) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    } else if (state ==
            BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_STATE_T_T_GROUP_NOT_PRESENT) {
        /* Should not happen */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}
