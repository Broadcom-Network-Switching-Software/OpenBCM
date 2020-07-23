/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    l2.c
 * Purpose: Trident3 L2 functions implementation
 */

#include <shared/bsl.h>

#include <soc/defs.h>

#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_TRIDENT3_SUPPORT)

#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/hash.h>
#include <soc/l2x.h>
#include <soc/triumph.h>
#include <soc/trident2.h>
#include <soc/trident3.h>
#include <soc/format.h>
#include <soc/esw/flow_db.h>

#include <bcm/l2.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/mim.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/flow.h>

extern int
_bcm_td3_evxlt_entry_valid( int unit, soc_mem_t mem, uint32 *entry, int *valid);

#define DEST_TYPE_INVALID  0
#define DEST_TYPE0_DVP  1
#define DEST_TYPE0_DGPP 2
#define DEST_TYPE0_NHOP 3
#define DEST_TYPE1_L2MC 2
#define DEST_TYPE1_IPMC 3
#define DEST_TYPE2_LAG  1
#define DEST_TYPE3_MY_STATION 3

/* just need to clear SRC or DES hit bit. Customers may call API bcm_esw_l2_replace
*  with parameter *match_addr which value is NULL */
#define _CLEAR_ALL_ENTRIES_SRC_OR_DES_HIT_BIT(flag) \
                  (!(flag & (BCM_L2_REPLACE_MATCH_VLAN |\
                             BCM_L2_REPLACE_MATCH_MAC |\
                             BCM_L2_REPLACE_MATCH_DEST)) &&\
                   (flag & (BCM_L2_REPLACE_DES_HIT_CLEAR |\
                            BCM_L2_REPLACE_SRC_HIT_CLEAR)))

#define EXTRACT_MAC_DA_27_0(da)     ((da)[0] & 0x0fffffff)
#define EXTRACT_MAC_DA_47_28(da) \
    ((((da)[1]<<4) | (((da)[0]>>28) & 0xf)) & 0xfffff)

#define COMBINE_MAC_DA_32_0(da)     ((da)[0] | (((da)[1] & 0xf)<<28))
#define COMBINE_MAC_DA_47_32(da)    ((da)[1]>>4)

#define _BCM_EXLATE_CHANGE_L2_FIELDS_KEY_TYPE(_u_) \
     ((SOC_IS_HELIX5(_u_)) ? (0x13) : (0x11))

#define _BCM_TD3_EXLATE_CHANGE_L2_FIELDS_DATA_TYPE 0x16
#define _BCM_HX5_EXLATE_CHANGE_L2_FIELDS_DATA_TYPE 0x19

#define _BCM_EXLATE_CHANGE_L2_FIELDS_DATA_TYPE(_u_) \
    ((SOC_IS_HELIX5(_u_)) ? (25) : (22))

/*
 * Function:
 *      _bcm_td3_flex_l2_to_l2x
 * Purpose:
 *      Convert an L2 API data structure to a Trident3 L2 flex-view entry
 * Parameters:
 *      unit        Unit number
 *      l2x_entry   (OUT) Trident3 L2X entry
 *      l2addr      L2 API data structure
 *      key_only    Only construct key portion
 */
int
_bcm_td3_flex_l2_to_l2x(int unit, l2x_entry_t *l2x_entry, bcm_l2_addr_t *l2addr, int key_only)
{
    int vfi;
    int tid_is_vp_lag;
    bcm_cos_t cos_dst = l2addr->cos_dst;
    int group = l2addr->group;
    int my_station_config = 0;
    int i;
    int rv = BCM_E_NONE;
    uint32  view_id;
    uint32  field_array[128];
    uint32  field_count;
    uint32  opaque_array[16];
    uint32  opaque_count;
    bcm_flow_logical_field_t* user_fields;
    int j;

    uint32 action_set = 0;
    uint8 process_table_management = 0;
    uint8 process_fixed = 0;
    uint8 process_fwd_attributes = 0;
    uint8 process_qos = 0;
    uint8 process_class_id = 0;
    uint8 process_destination = 0;
    uint8 process_vlan=0;
    uint8 process_vfi=0;
    uint8 process_mac=0;

    /* Get view id corresponding to the specified flow. */
    rv = soc_flow_db_ffo_to_mem_view_id_get(unit,
                          l2addr->flow_handle,
                          l2addr->flow_option_handle,
                          SOC_FLOW_DB_FUNC_L2_SWITCH_ID,
                          &view_id);
    BCM_IF_ERROR_RETURN(rv);

    /* Initialize control field list for this view id. */
    rv = soc_flow_db_mem_view_entry_init(unit, view_id, (uint32*) l2x_entry);

    /* Get logical KEY field list for this view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,
                            16,
                            opaque_array,
                            &opaque_count);
    BCM_IF_ERROR_RETURN(rv);

    user_fields = l2addr->logical_fields;

    for (i=0; i<opaque_count; i++) {
        for (j = 0; j < l2addr->num_of_fields; j++) {
            if (user_fields[j].id == opaque_array[i]) {
                soc_mem_field32_set(unit, view_id, l2x_entry, user_fields[j].id,
                                user_fields[j].value);
                break;
            }
        }
    }


    /* Get KEY field list corresponding to the view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_KEY,
                            128,
                            field_array,
                            &field_count);
    BCM_IF_ERROR_RETURN(rv);
 
    for (i=0; i<field_count; i++) {
        switch (field_array[i]) {
            case VFIf:
                process_vfi = 1;
                break;
            case MAC_DAf:
                process_mac = 1;
                break;
            case VLAN_IDf:
                process_vlan = 1;
                break;
            default:
                break;
        }
    }

    if (_BCM_VPN_VFI_IS_SET(l2addr->vid)) {
        if (!process_vfi)  {
            return BCM_E_PARAM;
        }
        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, l2addr->vid);
        soc_mem_field32_set(unit, view_id, l2x_entry, VFIf, vfi);
    } else {
         if (!_BCM_MPLS_VPN_IS_VPWS(l2addr->vid)) {
             if (process_vlan)  {
                VLAN_CHK_ID(unit, l2addr->vid);
                soc_mem_field32_set(unit, view_id, l2x_entry, VLAN_IDf, l2addr->vid);
             }
         }
    }

    if (process_mac) {
        soc_mem_mac_addr_set(unit, view_id, l2x_entry, MAC_DAf, l2addr->mac);
    }

    if (key_only) {
        return BCM_E_NONE;
    }

    /* Get logical PDD field list for this view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                            16,
                            opaque_array,
                            &opaque_count);
    BCM_IF_ERROR_RETURN(rv);

    user_fields = l2addr->logical_fields;

    for (i=0; i<opaque_count; i++) {
        for (j = 0; j < l2addr->num_of_fields; j++) {
            if (user_fields[j].id == opaque_array[i]) {
                soc_mem_field32_set(unit, view_id, l2x_entry,
                    user_fields[j].id, user_fields[j].value);
                break;
            }
        }
    }

    /* Get PDD field list corresponding to the view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA,
                            128,
                            field_array,
                            &field_count);
    BCM_IF_ERROR_RETURN(rv);

    for (i=0; i<field_count; i++) {
        switch (field_array[i]) {
            case DESTINATION_ACTION_SETf:
                process_destination = 1;
                break;
            case CLASS_ID_ACTION_SETf:
                process_class_id = 1;
                break;
            case QOS_ACTION_SETf:
                process_qos = 1;
                break;
            case FWD_ATTRIBUTES_ACTION_SETf:
                process_fwd_attributes = 1;
                break;
            case FIXED_ACTION_SETf:
                process_fixed = 1;
                break;
            case TABLE_MANAGEMENT_ACTION_SETf:
                process_table_management = 1;
                break;
            default:
                return BCM_E_INTERNAL;
        }
    }

    if (!process_destination) {
        return BCM_E_INTERNAL;
    }
    if (l2addr->port == -1) {
        /* indicate destination is 0, needed for certain flows */
        soc_mem_field32_set(unit, view_id, l2x_entry,
                            DESTINATION_ACTION_SETf, 0);
    } else if (BCM_MAC_IS_MCAST(l2addr->mac)) {
        if (l2addr->l2mc_group < 0) {
            return BCM_E_PARAM;
        }

        action_set = 0;

        if (_BCM_MULTICAST_IS_FLOW(l2addr->l2mc_group)) {
            soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, DEST_TYPE1f, DEST_TYPE1_IPMC);
        }
        else {
            soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, DEST_TYPE1f, DEST_TYPE1_L2MC);
        }
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, L2MC_GROUPf,
                               _BCM_MULTICAST_ID_GET(l2addr->l2mc_group));

        soc_mem_field32_set(unit, view_id, l2x_entry,
                            DESTINATION_ACTION_SETf, action_set);
    } else if ((!l2addr->port) && l2addr->egress_if) {
#if defined(INCLUDE_L3)
        int offset;

        /* validate the parameter */
        if ((!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, l2addr->egress_if)) &&
            (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, l2addr->egress_if))) {
            return (BCM_E_PARAM);
        }
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, l2addr->egress_if)) {
            offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else {
            offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        }
        action_set = 0;
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, DEST_TYPE0f, DEST_TYPE0_NHOP);
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, NEXT_HOP_INDEXf,
                               l2addr->egress_if - offset);
        soc_mem_field32_set(unit, view_id, l2x_entry,
                            DESTINATION_ACTION_SETf, action_set);
#else /* INCLUDE_L3 */
        return BCM_E_PARAM;
#endif /* INCLUDE_L3 */
    } else {
        bcm_port_t      port = -1;
        bcm_trunk_t     tgid = BCM_TRUNK_INVALID;
        bcm_module_t    modid = -1;
        int             gport_id = -1;

        if (BCM_GPORT_IS_SET(l2addr->port)) {
            _bcm_l2_gport_params_t  g_params;

            if (BCM_GPORT_IS_BLACK_HOLE(l2addr->port)) {
                if (!process_fwd_attributes) {
                    return BCM_E_PARAM;
                }
                action_set = 0;
                soc_format_field32_set(unit, FWD_ATTRIBUTES_ACTION_SETfmt,
                               &action_set, SRC_DISCARDf, 1);
                soc_mem_field32_set(unit, view_id, l2x_entry,
                                 FWD_ATTRIBUTES_ACTION_SETf, action_set);
            } else {
                /*soc_L2Xm_field32_set(unit, l2x_entry, SRC_DISCARDf, 0);*/
                BCM_IF_ERROR_RETURN(
                        _bcm_esw_l2_gport_parse(unit, l2addr, &g_params));

                switch (g_params.type) {
                    case _SHR_GPORT_TYPE_TRUNK:
                        tgid = g_params.param0;
                        break;
                    case  _SHR_GPORT_TYPE_MODPORT:
                        port = g_params.param0;
                        modid = g_params.param1;
                        break;
                    case _SHR_GPORT_TYPE_LOCAL_CPU:
                        port = g_params.param0;
                        BCM_IF_ERROR_RETURN(
                                bcm_esw_stk_my_modid_get(unit, &modid));
                        break;
                    case _SHR_GPORT_TYPE_SUBPORT_GROUP:
                        gport_id = g_params.param0;
                        break;
                    case _SHR_GPORT_TYPE_SUBPORT_PORT:
                        gport_id = g_params.param0;
                    break;
                    case _SHR_GPORT_TYPE_MPLS_PORT:
                    gport_id = g_params.param0;
                    break;
                    case _SHR_GPORT_TYPE_MIM_PORT:
                    case _SHR_GPORT_TYPE_WLAN_PORT:
                    case _SHR_GPORT_TYPE_VLAN_PORT:
                    case _SHR_GPORT_TYPE_NIV_PORT:
                    case _SHR_GPORT_TYPE_EXTENDER_PORT:
                        gport_id = g_params.param0;
                        break;
                    case _SHR_GPORT_TYPE_TRILL_PORT:
                    case _SHR_GPORT_TYPE_L2GRE_PORT:
                    case _SHR_GPORT_TYPE_VXLAN_PORT:
                    case _SHR_GPORT_TYPE_FLOW_PORT:

                        gport_id = g_params.param0;
                        break;
                    default:
                        return BCM_E_PORT;
                }
            }
        } else if (l2addr->flags & BCM_L2_TRUNK_MEMBER) {
            tgid = l2addr->tgid;

        } else {
            if ((NUM_MODID(unit) > 1) && (l2addr->port > SOC_MODPORT_MAX(unit))) {
                bcm_module_t my_modid;
                int          isLocal;
                BCM_IF_ERROR_RETURN (bcm_esw_stk_my_modid_get(unit, &my_modid));
                if (BCM_MODID_INVALID != my_modid) {
                   BCM_IF_ERROR_RETURN
                    (_bcm_esw_modid_is_local(unit, l2addr->modid, &isLocal));
                   if (isLocal) {
                      l2addr->port %= (SOC_MODPORT_MAX(unit) + 1);
                    }
                }
	    }
            BCM_IF_ERROR_RETURN(
                _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                       l2addr->modid, l2addr->port,
                                       &modid, &port));
            /* Check parameters */
            if (!SOC_MODID_ADDRESSABLE(unit, modid)) {
                return BCM_E_BADID;
            }
            if (!SOC_PORT_ADDRESSABLE(unit, port)) {
                return BCM_E_PORT;
            }
        }

        action_set = 0;
        /* Setting l2x_entry fields according to parameters */
        if ( BCM_TRUNK_INVALID != tgid) {
            BCM_IF_ERROR_RETURN(_bcm_esw_trunk_id_is_vp_lag(unit, tgid,
                            &tid_is_vp_lag));
            if (tid_is_vp_lag) {
                if (soc_feature(unit, soc_feature_vp_lag)) {
                    int vp_lag_vp;
                    /* Get the VP value representing VP LAG */
                    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_tid_to_vp_lag_vp(unit,
                                tgid, &vp_lag_vp));
                    soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                                       &action_set, DEST_TYPE0f, DEST_TYPE0_DVP);
                    soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                                       &action_set, DVPf, vp_lag_vp);
                } else {
                    return BCM_E_PORT;
                }
            } else {
                    soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                                       &action_set, DEST_TYPE2f, DEST_TYPE2_LAG);
                    soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                                       &action_set, LAGf, tgid);
            }
        } else if (-1 != port) {
            soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, DEST_TYPE0f, DEST_TYPE0_DGPP);
            soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, DGPPf, 
                               modid << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | port);
        } else if (-1 != gport_id) {
            soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                                   &action_set, DEST_TYPE0f, DEST_TYPE0_DVP);
            soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                                   &action_set, DVPf, gport_id);
        }
        soc_mem_field32_set(unit, view_id, l2x_entry,
                            DESTINATION_ACTION_SETf, action_set);
    }

/* not support in flex views */
#if 0
    if (SOC_MEM_FIELD_VALID(unit, L2Xm, L3f)) {
        if (l2addr->flags & BCM_L2_L3LOOKUP) {
            soc_L2Xm_field32_set(unit, l2x_entry, L3f, 1);
        }
    }
#endif

    if (SOC_L2X_GROUP_ENABLE_GET(unit)) {
        if (soc_feature(unit, soc_feature_overlaid_address_class) &&
            !BCM_L2_PRE_SET(l2addr->flags)) {
            if ((l2addr->group > SOC_OVERLAID_ADDR_CLASS_MAX(unit)) ||
                (l2addr->group < 0)) {
                return BCM_E_PARAM;
            }
            cos_dst = (l2addr->group & 0x3C0) >> 6;
            group = l2addr->group & 0x3F;
        }
    }

    if (SOC_CONTROL(unit)->l2x_group_enable && process_class_id) {
        soc_mem_field32_set(unit, view_id, l2x_entry,
                         CLASS_ID_ACTION_SETf, group);
    }

    action_set = 0;
    soc_format_field32_set(unit, FORWARDING_1_QOS_ACTION_SETfmt,
                           &action_set, PRIf, cos_dst);
    if (l2addr->flags & BCM_L2_SETPRI) {
        if (!process_qos) {
            return BCM_E_PARAM;
        }
        soc_format_field32_set(unit, FORWARDING_1_QOS_ACTION_SETfmt,
                               &action_set, RPEf, 1);
    }
    if (l2addr->flags & BCM_L2_COS_SRC_PRI) {
        if (!process_qos) {
            return BCM_E_PARAM;
        }
        soc_format_field32_set(unit, FORWARDING_1_QOS_ACTION_SETfmt,
                               &action_set, SCPf, 1);
    }

    if (process_qos) {
        soc_mem_field32_set(unit, view_id, l2x_entry,
                        QOS_ACTION_SETf, action_set);
    }

    action_set = 0;
    if (l2addr->flags & BCM_L2_COPY_TO_CPU) {
        if (!process_fwd_attributes) {
            return BCM_E_PARAM;
        }
        soc_format_field32_set(unit, FWD_ATTRIBUTES_ACTION_SETfmt,
                               &action_set, CPUf, 1);
    }

    if (l2addr->flags & BCM_L2_DISCARD_DST) {
        if (!process_fwd_attributes) {
            return BCM_E_PARAM;
        }
        soc_format_field32_set(unit, FWD_ATTRIBUTES_ACTION_SETfmt,
                               &action_set, DST_DISCARDf, 1);
    }

    if (l2addr->flags & BCM_L2_DISCARD_SRC) {
        if (!process_fwd_attributes) {
            return BCM_E_PARAM;
        }
        soc_format_field32_set(unit, FWD_ATTRIBUTES_ACTION_SETfmt,
                               &action_set, SRC_DISCARDf, 1);
    }

    if (process_fwd_attributes) {
        soc_mem_field32_set(unit, view_id, l2x_entry,
                            FWD_ATTRIBUTES_ACTION_SETf, action_set);
    }

    if (l2addr->flags & BCM_L2_PENDING) {
        if (!process_table_management) {
            return BCM_E_PARAM;
        }
        action_set = 0;
        soc_format_field32_set(unit, TABLE_MANAGEMENT_ACTION_SETfmt,
                               &action_set, PENDINGf, 1);
        soc_mem_field32_set(unit, view_id, l2x_entry,
                            TABLE_MANAGEMENT_ACTION_SETf, action_set);
    }

    if (l2addr->flags & BCM_L2_STATIC) {
        if (!process_fixed) {
            return BCM_E_PARAM;
        }
        /* STATIC_BITf */
        soc_mem_field32_set(unit, view_id, l2x_entry,
                            FIXED_ACTION_SETf, 1);
    }

    if ((l2addr->flags & BCM_L2_DES_HIT) ||
        (l2addr->flags & BCM_L2_HIT)) {
        soc_mem_field32_set(unit, view_id, l2x_entry, HITDAf, 1);
    }

    if ((l2addr->flags & BCM_L2_SRC_HIT) ||
        (l2addr->flags & BCM_L2_HIT)) {
        soc_mem_field32_set(unit, view_id, l2x_entry, HITSAf, 1);
    }

    if (SOC_MEM_FIELD_VALID(unit, view_id, LOCAL_SAf)) {
        if (l2addr->flags & BCM_L2_NATIVE) {
            soc_mem_field32_set(unit, view_id, l2x_entry, LOCAL_SAf, 1);
        }
    }

    my_station_config = soc_property_get(unit, spn_L2_ENTRY_USED_AS_MY_STATION, 0);
    if (soc_feature(unit, soc_feature_l2_entry_used_as_my_station)
        && my_station_config) {
        if (soc_feature(unit, soc_feature_mysta_profile)) {
            uint32 mysta_profile_id;
            bcm_l2_station_t station;
            my_station_profile_1_entry_t entry;
            sal_memset(&station, 0, sizeof(bcm_l2_station_t));
            sal_memset(&entry, 0, sizeof(entry));
            station.flags = l2addr->station_flags;
            if (l2addr->station_flags != 0) {
                _bcm_l2_mysta_station_to_entry(unit, &station, MY_STATION_PROFILE_1m, &entry);
                BCM_IF_ERROR_RETURN(
                    _bcm_l2_mysta_profile_entry_add(unit, &entry, 0,
                    &mysta_profile_id));
                action_set = 0;
                soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                                       &action_set, DEST_TYPE3f,
                                       DEST_TYPE3_MY_STATION);
                soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                                       &action_set, MY_STATION_PROFILE_IDXf,
                                       mysta_profile_id);
                soc_mem_field32_set(unit, view_id, l2x_entry,
                                    DESTINATION_ACTION_SETf, action_set);
            }
        }
    } else {
        if (l2addr->station_flags & BCM_L2_STATION_IPV4
           || l2addr->station_flags & BCM_L2_STATION_IPV6
           || l2addr->station_flags & BCM_L2_STATION_ARP_RARP
           || l2addr->station_flags & BCM_L2_STATION_OAM
           || l2addr->station_flags & BCM_L2_STATION_FCOE
           || l2addr->station_flags & BCM_L2_STATION_IPV4_MCAST
           || l2addr->station_flags & BCM_L2_STATION_IPV6_MCAST) {
           return BCM_E_UNAVAIL;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_flex_l2_from_l2x
 * Purpose:
 *      Convert a Trident3 Flex L2X entry to an L2 API data structure
 * Parameters:
 *      unit        Unit number
 *      l2addr      (OUT) L2 API data structure
 *      l2x_entry   Trident3 L2X entry
 */
int
_bcm_td3_flex_l2_from_l2x(int unit, bcm_l2_addr_t *l2addr, l2x_entry_t *l2x_entry, int *mb_index)
{
    int l2mc_index, vfi, rval;
    int my_station_config = 0;
    int i;
    int rv = BCM_E_NONE;
    uint32 action_set = 0;

    uint8 process_table_management = 0;
    uint8 process_fixed = 0;
    uint8 process_fwd_attributes = 0;
    uint8 process_qos = 0;
    uint8 process_class_id = 0;
    uint8 process_destination = 0;
    uint8 process_mac_block = 0;
    uint8 process_vlan = 0;
    uint8 process_vfi = 0;
    uint8 process_mac = 0;

    uint32 key_type;
    uint32 view_id=0;
    uint32 flex_data_array[128];
    uint32 flex_data_count;
    uint32 flex_key_array[64];
    uint32 flex_key_count;
    uint32 opaque_array[16];
    uint32 opaque_count;
    bcm_flow_logical_field_t* user_fields;

    key_type = soc_mem_field32_get(unit, L2Xm, l2x_entry, KEY_TYPEf);

    /* Get view id corresponding to key type */
    rv = soc_flow_db_mem_to_view_id_get(unit,
                                   L2_ENTRY_SINGLEm,
                                   key_type,
                                   SOC_FLOW_DB_DATA_TYPE_INVALID,
                                   0,
                                   NULL,
                                   &view_id);

    BCM_IF_ERROR_RETURN(rv);

    /* Get logical KEY field list corresponding to the view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,
                            16,
                            opaque_array,
                            &opaque_count);
    BCM_IF_ERROR_RETURN(rv);

    user_fields = l2addr->logical_fields;

    for (i=0; i<opaque_count; i++) {
        user_fields[i].id = opaque_array[i];
        user_fields[i].value = soc_mem_field32_get(unit, view_id, l2x_entry,
                           opaque_array[i]);
    }
    l2addr->num_of_fields = opaque_count;

    user_fields = &l2addr->logical_fields[opaque_count];

     /* Get logical Data field list corresponding to the view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                            16,
                            opaque_array,
                            &opaque_count);
    BCM_IF_ERROR_RETURN(rv);

    for (i=0; i<opaque_count; i++) {
        user_fields[i].id = opaque_array[i];
        user_fields[i].value = soc_mem_field32_get(unit, view_id, l2x_entry,
                           opaque_array[i]);
    }
    l2addr->num_of_fields += opaque_count;
 
    /* Get KEY field list corresponding to the view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_KEY,
                            64,
                            flex_key_array,
                            &flex_key_count);
    BCM_IF_ERROR_RETURN(rv);
 
    for (i=0; i<flex_key_count; i++) {
        switch (flex_key_array[i]) {
            case VFIf:
                process_vfi = 1;
                break;
            case MAC_DAf:
                process_mac = 1;
                break;
            case VLAN_IDf:
                process_vlan = 1;
                break;
            default:
                break;
        }
    }

    /* Get PDD field list corresponding to the view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA,
                            128,
                            flex_data_array,
                            &flex_data_count);
    BCM_IF_ERROR_RETURN(rv);

    for (i=0; i<flex_data_count; i++) {
        switch (flex_data_array[i]) {
            case DESTINATION_ACTION_SETf:
                process_destination = 1;
                break;
            case CLASS_ID_ACTION_SETf:
                process_class_id = 1;
                break;
            case QOS_ACTION_SETf:
                process_qos = 1;
                break;
            case FWD_ATTRIBUTES_ACTION_SETf:
                process_fwd_attributes = 1;
                break;
            case FIXED_ACTION_SETf:
                process_fixed = 1;
                break;
            case TABLE_MANAGEMENT_ACTION_SETf:
                process_table_management = 1;
                break;
            case MAC_BLOCK_ACTION_SETf:
                process_mac_block = 1;
                break;
            default:
                break;
        }
    }

    if (!process_destination) {
        return BCM_E_INTERNAL;
    }
    if (process_mac) {
        soc_mem_mac_addr_get(unit, view_id, l2x_entry, MAC_DAf, l2addr->mac);
    }

    if (process_vfi) {
        vfi = soc_mem_field32_get(unit, view_id, l2x_entry, VFIf);
        COMPILER_REFERENCE(vfi);
        /* VPLS or MIM VPN */
#if defined(INCLUDE_L3)
        if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
            _BCM_MPLS_VPN_SET(l2addr->vid, _BCM_MPLS_VPN_TYPE_VPLS, vfi);
        } else if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
            _BCM_MIM_VPN_SET(l2addr->vid, _BCM_MIM_VPN_TYPE_MIM, vfi);
        } else  if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeL2Gre)) {
                if (soc_feature(unit, soc_feature_l2gre)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_tr3_l2gre_vpn_get(unit, vfi, &l2addr->vid));
                }
        } else if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeVxlan)) {
                if (soc_feature(unit, soc_feature_vxlan)) {
                    BCM_IF_ERROR_RETURN(
                      _bcm_td2_vxlan_vpn_get(unit, vfi, &l2addr->vid));
                }
        } else if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeFlow)) {
                if (soc_feature(unit, soc_feature_flex_flow)) {
                    BCM_IF_ERROR_RETURN(
                      bcmi_esw_flow_vpnid_get(unit, vfi, &l2addr->vid));
                }
        }
#endif /* INCLUDE_L3 */
    } else if (process_vlan) {
        l2addr->vid = soc_mem_field32_get(unit, view_id, l2x_entry, VLAN_IDf);
    }

    if (process_mac && BCM_MAC_IS_MCAST(l2addr->mac)) {
            uint32 dest_type1 = DEST_TYPE_INVALID;
            l2addr->flags |= BCM_L2_MCAST;
            action_set = soc_mem_field32_get(unit, view_id, l2x_entry,
                               DESTINATION_ACTION_SETf);
            l2mc_index = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                                &action_set, L2MC_GROUPf);
            dest_type1 = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                                &action_set, DEST_TYPE1f);
            if (dest_type1 != DEST_TYPE1_L2MC) {
                l2mc_index = 0;
            }
            l2addr->l2mc_group = l2mc_index;
            /* Translate l2mc index */
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_control_get(unit, bcmSwitchL2McIdxRetType, &rval));
            if (rval) {
               _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group, _BCM_MULTICAST_TYPE_L2,
                                                                    l2addr->l2mc_group);
            }
    } else {
        _bcm_gport_dest_t       dest;
        int                     dest_type_is_trunk = 0;
        int                     isGport = 0;
#if defined(INCLUDE_L3)
        int                     dest_type_is_dvp = 0;
        uint32 dest_type0 = DEST_TYPE_INVALID;
        uint32 dest_type2 = DEST_TYPE_INVALID;
#endif
        uint32 dest_value = 0;

        _bcm_gport_dest_t_init(&dest);
#if defined(INCLUDE_L3)
        action_set = soc_mem_field32_get(unit, view_id, l2x_entry,
                           DESTINATION_ACTION_SETf);
        dest_type0 = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                            &action_set, DEST_TYPE0f);
        dest_type2 = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                            &action_set, DEST_TYPE2f);
        if (dest_type0 == DEST_TYPE0_DVP) {
            dest_type_is_dvp = 1;
        } else if (dest_type2 == DEST_TYPE2_LAG) {
            dest_type_is_trunk = 1;
        }

        if (!action_set) {
            l2addr->port = -1;

        } else if (dest_type0 == DEST_TYPE0_NHOP) {
            int nhop;

            nhop = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                                &action_set, NEXT_HOP_INDEXf);
            l2addr->egress_if = nhop + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);

        } else if (dest_type_is_dvp) {
            int vp;
            vp = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                                &action_set, DVPf);

            /* MPLS/MiM virtual port unicast */
            if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                dest.mpls_id = vp;
                dest.gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                dest.mim_id = vp;
                dest.gport_type = _SHR_GPORT_TYPE_MIM_PORT;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
                dest.vxlan_id  = vp;
                dest.gport_type = _SHR_GPORT_TYPE_VXLAN_PORT;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
                dest.flow_id  = vp;
                dest.gport_type = _SHR_GPORT_TYPE_FLOW_PORT;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
                dest.l2gre_id  = vp;
                dest.gport_type = _SHR_GPORT_TYPE_L2GRE_PORT;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeSubport)) {
            /* Subport/WLAN unicast */
                    dest.subport_id = vp;
                    dest.gport_type = _SHR_GPORT_TYPE_SUBPORT_GROUP;
                    isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
                dest.wlan_id = vp;
                dest.gport_type = _SHR_GPORT_TYPE_WLAN_PORT;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                dest.vlan_vp_id = vp;
                dest.gport_type = _SHR_GPORT_TYPE_VLAN_PORT;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
                dest.niv_id = vp;
                dest.gport_type = _SHR_GPORT_TYPE_NIV_PORT;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeTrill)) {
                dest.trill_id  = vp;
                dest.gport_type = _SHR_GPORT_TYPE_TRILL_PORT;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
                dest.extender_id = vp;
                dest.gport_type = _SHR_GPORT_TYPE_EXTENDER_PORT;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVpLag)) {
                l2addr->flags |= BCM_L2_TRUNK_MEMBER;
                BCM_IF_ERROR_RETURN(_bcm_esw_trunk_vp_lag_vp_to_tid(unit,
                            vp, &l2addr->tgid));
                dest.tgid = l2addr->tgid;
                dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                            bcmSwitchUseGport, &isGport));
            } else {
                /* L2 entries with Stale Gport */
                dest.gport_type = BCM_GPORT_INVALID;
                isGport=0;
            }
        } else
#endif /* INCLUDE_L3 */
        if (dest_type_is_trunk) {
            /* Trunk group */
            l2addr->flags |= BCM_L2_TRUNK_MEMBER;
            dest_value = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                                &action_set, DVPf);
            l2addr->tgid = dest_value & SOC_MEM_FIF_DGPP_TGID_MASK;

            dest.tgid = l2addr->tgid;
            dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
        } else {
            bcm_module_t    mod_in, mod_out;
            bcm_port_t      port_in, port_out;

            dest_value = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                                &action_set, DGPPf);
            port_in = dest_value & SOC_MEM_FIF_DGPP_PORT_MASK;
            mod_in = (dest_value & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                            SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;

            BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                        mod_in, port_in, &mod_out, &port_out));
            l2addr->modid = mod_out;
            l2addr->port = port_out;
            dest.port = l2addr->port;
            dest.modid = l2addr->modid;
            dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
        }

        if (isGport) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_construct(unit, &dest, &(l2addr->port)));
        }
    }

    if (SOC_CONTROL(unit)->l2x_group_enable && process_class_id) {
        l2addr->group = soc_mem_field32_get(unit, view_id, l2x_entry,
                                           CLASS_ID_ACTION_SETf);
    } else if (process_mac_block) {
        action_set = soc_mem_field32_get(unit, view_id, l2x_entry,
                               MAC_BLOCK_ACTION_SETf);
        *mb_index = soc_format_field32_get(unit, MAC_BLOCK_ACTION_SETfmt,
                                &action_set, MAC_BLOCK_INDEXf);
    }

    if (process_qos) {
        action_set = soc_mem_field32_get(unit, view_id, l2x_entry,
                               QOS_ACTION_SETf);
        if (soc_format_field32_get(unit, FORWARDING_1_QOS_ACTION_SETfmt,
                                &action_set, RPEf)) {
            l2addr->flags |= BCM_L2_SETPRI;
        }
        if (soc_format_field32_get(unit, FORWARDING_1_QOS_ACTION_SETfmt,
                                &action_set, SCPf)) {
            l2addr->flags |= BCM_L2_COS_SRC_PRI;
        }

        l2addr->cos_dst = soc_format_field32_get(unit,
                             FORWARDING_1_QOS_ACTION_SETfmt,&action_set, PRIf);
        l2addr->cos_src = soc_format_field32_get(unit,
                             FORWARDING_1_QOS_ACTION_SETfmt,&action_set, PRIf);
    }

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        if (soc_feature(unit, soc_feature_overlaid_address_class) &&
            !BCM_L2_PRE_SET(l2addr->flags)) {
            l2addr->group |= (l2addr->cos_dst & 0xF) << 6;
        }
    } 

    if (process_fwd_attributes) {
        action_set = soc_mem_field32_get(unit, view_id, l2x_entry,
                               FWD_ATTRIBUTES_ACTION_SETf);
        if (soc_format_field32_get(unit, FORWARDING_1_QOS_ACTION_SETfmt,
                                &action_set, CPUf)) {
            l2addr->flags |= BCM_L2_COPY_TO_CPU;
        }

        if (soc_format_field32_get(unit, FORWARDING_1_QOS_ACTION_SETfmt,
                                &action_set, DST_DISCARDf)) {
            l2addr->flags |= BCM_L2_DISCARD_DST;
        }

        if (soc_format_field32_get(unit, FORWARDING_1_QOS_ACTION_SETfmt,
                                &action_set, SRC_DISCARDf)) {
            l2addr->flags |= BCM_L2_DISCARD_SRC;
        }
    }

    if (process_fixed) {
        if (soc_mem_field32_get(unit, view_id, l2x_entry, STATIC_BITf)) {
            l2addr->flags |= BCM_L2_STATIC;
        }
    }

    if (process_table_management) {
        action_set = soc_mem_field32_get(unit, view_id, l2x_entry,
                               TABLE_MANAGEMENT_ACTION_SETf);
        if (soc_format_field32_get(unit, TABLE_MANAGEMENT_ACTION_SETfmt,
                                &action_set, PENDINGf)) {
            l2addr->flags |= BCM_L2_PENDING;
        }
    }

    if (soc_mem_field_valid(unit, view_id, HITDAf)) {
        if (soc_mem_field32_get(unit, view_id, l2x_entry, HITDAf)) {
            l2addr->flags |= BCM_L2_DES_HIT;
            l2addr->flags |= BCM_L2_HIT;
        }
    }

    if (soc_mem_field_valid(unit, view_id, HITSAf)) {
        if (soc_mem_field32_get(unit, view_id, l2x_entry, HITSAf)) {
            l2addr->flags |= BCM_L2_SRC_HIT;
            l2addr->flags |= BCM_L2_HIT;
        }
    }

    if (soc_mem_field_valid(unit, view_id, LOCAL_SAf)) {
        if (soc_mem_field32_get(unit, view_id, l2x_entry, LOCAL_SAf)) {
            l2addr->flags |= BCM_L2_NATIVE;
        }
    }

    my_station_config = soc_property_get(unit, spn_L2_ENTRY_USED_AS_MY_STATION, 0);
    if (soc_feature(unit, soc_feature_l2_entry_used_as_my_station)
        && my_station_config) {
         if (soc_feature(unit, soc_feature_mysta_profile)) {
              bcm_l2_station_t station;
              uint32 mysta_prof_id = 0;
              uint32 dest_type3 = DEST_TYPE_INVALID;
              my_station_profile_1_entry_t entry_prof;

              sal_memset(&station, 0, sizeof(bcm_l2_station_t));

              action_set = soc_mem_field32_get(unit, view_id, l2x_entry,
                               DESTINATION_ACTION_SETf);
              dest_type3 = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                            &action_set, DEST_TYPE3f);
              mysta_prof_id = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                            &action_set, MY_STATION_PROFILE_IDXf);
              if (dest_type3 == DEST_TYPE3_MY_STATION) {
                  _bcm_l2_mysta_profile_entry_get(unit, &entry_prof, 0, mysta_prof_id);
                  _bcm_l2_mysta_entry_to_station(unit, &entry_prof, MY_STATION_PROFILE_1m, &station);
                  l2addr->station_flags = station.flags;
              } else {
                  l2addr->station_flags = 0;
              }
          }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_flex_l2_replace_data_mask_setup
 * Purpose:
 *      Setup the replace data structure with the data and mask of field
 *      being replaced/deleted
 * Parameters:
 *      unit        Unit number
 *      rep         Replacement data structure
 */

int
_bcm_td3_flex_l2_replace_data_mask_setup(int unit, _bcm_l2_replace_t *rep)
{
    bcm_mac_t mac_mask;
    bcm_mac_t mac_data;
    int field_len;
    int i;
    uint32 view_id;
    int rv = BCM_E_NONE;
    uint32 action_set = 0;

    uint32  flex_key_array[64];
    uint32  flex_key_count;

    uint8 process_vlan = 0;
    uint8 process_vfi = 0;
    uint8 process_mac = 0;

    sal_memset(&rep->match_mask, 0, sizeof(l2x_entry_t));
    sal_memset(&rep->match_data, 0, sizeof(l2x_entry_t));
    sal_memset(&rep->new_mask, 0, sizeof(l2x_entry_t));
    sal_memset(&rep->new_data, 0, sizeof(l2x_entry_t));

    view_id = rep->view_id;
    /* Initialize control field list for this view id. */
    rv = soc_flow_db_mem_view_entry_init(unit, view_id, (uint32*) &rep->match_data);
    BCM_IF_ERROR_RETURN(rv);

    field_len = soc_mem_field_length(unit, view_id, BASE_VALIDf);
    soc_mem_field32_set(unit, view_id, &rep->match_mask, BASE_VALIDf,
                        (1 << field_len) - 1);

    field_len = soc_mem_field_length(unit, view_id, KEY_TYPEf);
    soc_mem_field32_set(unit, view_id, &rep->match_mask, KEY_TYPEf,
                        (1 << field_len) - 1);

    field_len = soc_mem_field_length(unit, view_id, DATA_TYPEf);
    soc_mem_field32_set(unit, view_id, &rep->match_mask, DATA_TYPEf,
                        (1 << field_len) - 1);

    /* Get KEY field list corresponding to the view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_KEY,
                            128,
                            flex_key_array,
                            &flex_key_count);
    BCM_IF_ERROR_RETURN(rv);

    for (i=0; i<flex_key_count; i++) {
        switch (flex_key_array[i]) {
            case VFIf:
                process_vfi = 1;
                break;
            case MAC_DAf:
                process_mac = 1;
                break;
            case VLAN_IDf:
                process_vlan = 1;
                break;
            default:
                break;
        }
    }

    if (!SOC_MEM_FIELD_VALID(unit, view_id, DESTINATION_ACTION_SETf)) {
        return BCM_E_INTERNAL;
    }
    if (rep->flags & BCM_L2_REPLACE_MATCH_VLAN) {
        if (rep->key_vfi != -1) {
            if (!process_vfi) return BCM_E_PARAM;
            field_len = soc_mem_field_length(unit, view_id, VFIf);
            soc_mem_field32_set(unit, view_id, &rep->match_mask, VFIf,
                                (1 << field_len) - 1);
            soc_mem_field32_set(unit, view_id, &rep->match_data, VFIf,
                                rep->key_vfi);
        } else {
            if (!process_vlan) return BCM_E_PARAM;
            soc_mem_field32_set(unit, view_id, &rep->match_mask, VLAN_IDf, 0xfff);
            soc_mem_field32_set(unit, view_id, &rep->match_data, VLAN_IDf,
                                rep->key_vlan);
        }
    }

    if (rep->flags & BCM_L2_REPLACE_MATCH_MAC) {
        if (!process_mac) return BCM_E_PARAM;
        sal_memset(&mac_mask, 0xff, sizeof(mac_mask));
        soc_mem_mac_addr_set(unit, view_id, &rep->match_mask, MAC_DAf,
                             mac_mask);
        soc_mem_mac_addr_set(unit, view_id, &rep->match_data, MAC_DAf,
                             rep->key_mac);
    }

    if ((rep->flags & BCM_L2_REPLACE_MATCH_UC) && 
        !(rep->flags & BCM_L2_REPLACE_MATCH_MC)) {

        if (!process_mac) return BCM_E_PARAM;
        /* The 40th bit of Unicast must be 0 */
        sal_memset(&mac_mask, 0x00, sizeof(mac_mask));
        mac_mask[0] = 0x01;
        sal_memset(&mac_data, 0x00, sizeof(mac_data));
        soc_mem_mac_addr_set(unit, view_id, &rep->match_mask, MAC_DAf,
                             mac_mask);
        soc_mem_mac_addr_set(unit, view_id, &rep->match_data, MAC_DAf,
                             mac_data);
    } else if ((rep->flags & BCM_L2_REPLACE_MATCH_MC) &&
        !(rep->flags & BCM_L2_REPLACE_MATCH_UC)) {
        /* The 40th bit of Multicast must be 1 */
        if (!process_mac) return BCM_E_PARAM;

        sal_memset(&mac_mask, 0x00, sizeof(mac_mask));
        mac_mask[0] = 0x01;
        sal_memset(&mac_data, 0x00, sizeof(mac_data));
        mac_data[0] = 0x01;
        soc_mem_mac_addr_set(unit, view_id, &rep->match_mask, MAC_DAf,
                             mac_mask);
        soc_mem_mac_addr_set(unit, view_id, &rep->match_data, MAC_DAf,
                             mac_data);
    }

    if (rep->flags & BCM_L2_REPLACE_MATCH_DEST) {
        if (rep->match_dest.vp != -1) {
            action_set = 0;
            field_len = soc_mem_field_length(unit, view_id, DESTINATION_ACTION_SETf);
            soc_mem_field32_set(unit, view_id, &rep->match_mask, DESTINATION_ACTION_SETf, 
                                (1 << field_len) - 1);

            soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                                   &action_set, DEST_TYPE0f, DEST_TYPE0_DVP);
            soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                                   &action_set, DVPf, rep->match_dest.vp);
            soc_mem_field32_set(unit, view_id, &rep->match_data, 
                                DESTINATION_ACTION_SETf, action_set);
        } else {
            action_set = 0;
            soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                    &action_set, DGPPf,
                    SOC_MEM_FIF_DGPP_MOD_ID_MASK | SOC_MEM_FIF_DGPP_PORT_MASK);
            soc_mem_field32_set(unit, view_id, &rep->match_mask,
                                DESTINATION_ACTION_SETf, action_set);
            action_set = 0;
            if (rep->match_dest.trunk != -1) {
                soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                                   &action_set, DEST_TYPE2f, DEST_TYPE2_LAG);
                soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                                   &action_set, LAGf, rep->match_dest.trunk);
                soc_mem_field32_set(unit, view_id, &rep->match_data, 
                            DESTINATION_ACTION_SETf, action_set);
            } else {
                soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                       &action_set, DEST_TYPE0f, DEST_TYPE0_DGPP);
                soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                       &action_set, DGPPf, 
                       rep->match_dest.module << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS |
                     rep->match_dest.port);
                soc_mem_field32_set(unit, view_id, &rep->match_data, 
                            DESTINATION_ACTION_SETf, action_set);
                /* Ignore multicast mac from bulk delete */
                mac_mask[0] = 0x01;
                mac_mask[1] = mac_mask[2] = mac_mask[3] = 
                              mac_mask[4] = mac_mask[5] = 0x00;
                soc_mem_mac_addr_set(unit, view_id, &rep->match_mask, 
                    MAC_DAf, mac_mask);
            }
        }
    }
    
    /* BCM_L2_REPLACE_MATCH_STATIC means to replace static as well as
    * non-static entries */
    if (!(rep->flags & BCM_L2_REPLACE_MATCH_STATIC) &&
         (SOC_MEM_FIELD_VALID(unit, view_id, STATIC_BITf))) {
        if (rep->int_flags & _BCM_L2_REPLACE_INT_MATCH_ONLY_STATIC) {
            soc_mem_field32_set(unit, view_id, &rep->match_mask, STATIC_BITf, 1);
            soc_mem_field32_set(unit, view_id, &rep->match_data, STATIC_BITf, 1);
        } else {
            soc_mem_field32_set(unit, view_id, &rep->match_mask, STATIC_BITf, 1);
            /* STATIC_BIT field in data is 0 */
        }
    }
    /* STATIC_BIT field in mask is 0 */ 

    if (SOC_MEM_FIELD_VALID(unit, view_id, TABLE_MANAGEMENT_ACTION_SETf)) {
        action_set = 0;
        soc_format_field32_set(unit, TABLE_MANAGEMENT_ACTION_SETfmt,
                               &action_set, PENDINGf, 1);
        soc_mem_field32_set(unit, view_id, &rep->match_mask, 
                            TABLE_MANAGEMENT_ACTION_SETf, action_set);
        if (rep->flags & BCM_L2_REPLACE_PENDING) {
            soc_mem_field32_set(unit, view_id, &rep->match_data, 
                            TABLE_MANAGEMENT_ACTION_SETf, action_set);
        }
    }

    if (rep->int_flags & _BCM_L2_REPLACE_INT_MATCH_DISCARD_SRC) {
        if (!SOC_MEM_FIELD_VALID(unit, view_id, FWD_ATTRIBUTES_ACTION_SETf)) {
            return BCM_E_PARAM;
        }
        action_set = 0;
        soc_format_field32_set(unit, FWD_ATTRIBUTES_ACTION_SETfmt,
                                   &action_set, SRC_DISCARDf, 1);
        soc_mem_field32_set(unit, view_id, &rep->match_mask, 
                            FWD_ATTRIBUTES_ACTION_SETf, action_set);
        if (rep->key_l2_flags & BCM_L2_DISCARD_SRC) {
            soc_mem_field32_set(unit, view_id, &rep->match_data, 
                                FWD_ATTRIBUTES_ACTION_SETf, action_set);
        }
    }

    if (rep->int_flags & _BCM_L2_REPLACE_INT_MATCH_SRC_HIT) {
        if (!SOC_MEM_FIELD_VALID(unit, view_id, HITSAf)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, view_id, &rep->match_mask, HITSAf, 1);
        if (rep->key_l2_flags & BCM_L2_SRC_HIT) {
            soc_mem_field32_set(unit, view_id, &rep->match_data, HITSAf, 1);
        }
    }
    if (rep->int_flags & _BCM_L2_REPLACE_INT_MATCH_DES_HIT) {
        if (!SOC_MEM_FIELD_VALID(unit, view_id, HITDAf)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, view_id, &rep->match_mask, HITDAf, 1);
        if (rep->key_l2_flags & BCM_L2_DES_HIT) {
            soc_mem_field32_set(unit, view_id, &rep->match_data, HITDAf, 1);
        }
    }
    if (SOC_MEM_FIELD_VALID(unit, view_id, LOCAL_SAf)) {
        if (rep->int_flags & _BCM_L2_REPLACE_INT_MATCH_NATIVE) {
            soc_mem_field32_set(unit, view_id, &rep->match_mask, LOCAL_SAf, 1);
            if (rep->key_l2_flags & BCM_L2_NATIVE) {
                soc_mem_field32_set(unit, view_id, &rep->match_data, LOCAL_SAf,
                                    1);
            }
        }
    }
    if (_CLEAR_ALL_ENTRIES_SRC_OR_DES_HIT_BIT(rep->flags)) {
        if (rep->flags & BCM_L2_REPLACE_DES_HIT_CLEAR) {
            if (!SOC_MEM_FIELD_VALID(unit, view_id, HITDAf)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, view_id, &rep->match_mask, HITDAf, 1);
            soc_mem_field32_set(unit, view_id, &rep->match_data, HITDAf, 1);
        }
        if (rep->flags & BCM_L2_REPLACE_SRC_HIT_CLEAR) {
            if (!SOC_MEM_FIELD_VALID(unit, view_id, HITSAf)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, view_id, &rep->match_mask, HITSAf, 1);
            soc_mem_field32_set(unit, view_id, &rep->match_data, HITSAf, 1);
        }
    }
    if (!(rep->flags & (BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_AGE))) {
        if (!_CLEAR_ALL_ENTRIES_SRC_OR_DES_HIT_BIT(rep->flags)) {
            if (rep->new_dest.vp != -1) {
                action_set = 0;
                field_len = soc_mem_field_length(unit, view_id, DESTINATION_ACTION_SETf);
                soc_mem_field32_set(unit, view_id, &rep->new_mask, DESTINATION_ACTION_SETf, 
                                    (1 << field_len) - 1);
                soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                                       &action_set, DEST_TYPE0f, DEST_TYPE0_DVP);
                soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                                       &action_set, DVPf, rep->new_dest.vp);
                soc_mem_field32_set(unit, view_id, &rep->new_data, 
                                    DESTINATION_ACTION_SETf, action_set);
            } else {
                action_set = 0;
                field_len = soc_mem_field_length(unit, view_id, DESTINATION_ACTION_SETf);
                soc_mem_field32_set(unit, view_id, &rep->new_mask, DESTINATION_ACTION_SETf,
                                            (1 << field_len) - 1);
                if (rep->new_dest.trunk != -1) {
                    soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                                       &action_set, DEST_TYPE2f, DEST_TYPE2_LAG);
                    soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                                       &action_set, LAGf, rep->new_dest.trunk);
                    soc_mem_field32_set(unit, view_id, &rep->new_data, 
                                DESTINATION_ACTION_SETf, action_set);
                } else {
                    soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                                &action_set, DEST_TYPE0f, DEST_TYPE0_DGPP);
                    soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, DGPPf, 
                                rep->new_dest.module << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS |
                                rep->new_dest.port);
                    soc_mem_field32_set(unit, view_id, &rep->new_data, 
                            DESTINATION_ACTION_SETf, action_set);
                }
            }
        }
        if (rep->flags & BCM_L2_REPLACE_DES_HIT_CLEAR) {
            if (!SOC_MEM_FIELD_VALID(unit, view_id, HITDAf)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, view_id, &rep->new_mask, HITDAf, 1);
        }
        if (rep->flags & BCM_L2_REPLACE_SRC_HIT_CLEAR) {
            if (!SOC_MEM_FIELD_VALID(unit, view_id, HITSAf)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, view_id, &rep->new_mask, HITSAf, 1);
        }
    }

    return BCM_E_NONE;
}
/*
 *     _bcm_td3_flex_l2_bulk_replace_modport
 * Description:
 *     Match Flex L2 entries with these fields: DEST_TYPE + MODULE_ID + PORT_NUM 
 *     Replace with one of following:
 *         DEST_TYPE = 0, new MODULE_ID, new PORT_NUM 
 *         DEST_TYPE = 1, new TGID
 * Parameters:
 *     unit         device number
 *     rep_st       structure with match/replace information
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_td3_flex_l2_bulk_replace_modport(int unit, _bcm_l2_replace_t *rep_st)
{
    l2_bulk_entry_t match_mask;
    l2_bulk_entry_t match_data;
    l2_bulk_entry_t repl_mask;
    l2_bulk_entry_t repl_data;
    int field_len;
    int rv;
    uint32 view_id;
    uint32 action_set=0;

    sal_memset(&match_mask, 0, sizeof(match_mask));
    sal_memset(&match_data, 0, sizeof(match_data));
    sal_memset(&repl_mask, 0, sizeof(repl_mask));
    sal_memset(&repl_data, 0, sizeof(repl_data));

    view_id = rep_st->view_id;
    /* Initialize control field list for this view id. */
    rv = soc_flow_db_mem_view_entry_init(unit, view_id, (uint32*) &match_data);
    BCM_IF_ERROR_RETURN(rv);

    soc_mem_field32_set(unit, view_id, &match_mask, BASE_VALIDf, 1);
    soc_mem_field32_set(unit, view_id, &match_mask, VALIDf, 1);

    field_len = soc_mem_field_length(unit, view_id, KEY_TYPEf);
    soc_mem_field32_set(unit, view_id, &match_mask, KEY_TYPEf,
                        (1 << field_len) - 1);

    field_len = soc_mem_field_length(unit, view_id, DATA_TYPEf);
    soc_mem_field32_set(unit, view_id, &match_mask, DATA_TYPEf,
                        (1 << field_len) - 1);

    action_set = 0;
    soc_format_field32_set(unit, DESTINATION_FORMATfmt,
            &action_set, DGPPf,
            SOC_MEM_FIF_DGPP_MOD_ID_MASK | SOC_MEM_FIF_DGPP_PORT_MASK);
    soc_mem_field32_set(unit, view_id, &match_mask,
                        DESTINATION_ACTION_SETf, action_set);
    action_set = 0;
    soc_format_field32_set(unit, DESTINATION_FORMATfmt,
        &action_set, DEST_TYPE0f, DEST_TYPE0_DGPP);
    soc_format_field32_set(unit, DESTINATION_FORMATfmt,
        &action_set, DGPPf, 
        rep_st->match_dest.module << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | 
        rep_st->match_dest.port);
    soc_mem_field32_set(unit, view_id, &match_data,
                        DESTINATION_ACTION_SETf, action_set);

    if (!(rep_st->flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        soc_mem_field32_set(unit, view_id, &match_mask,
                            STATIC_BITf, 1);
        /* STATIC_BIT field in data is 0 */
    }

    if (rep_st->flags & BCM_L2_REPLACE_PENDING) {
        if (!soc_mem_field_valid(unit, view_id, TABLE_MANAGEMENT_ACTION_SETf)) {
            return BCM_E_PARAM;
        }
        action_set = 0;
        soc_format_field32_set(unit, TABLE_MANAGEMENT_ACTION_SETfmt,
                               &action_set, PENDINGf, 1);
        soc_mem_field32_set(unit, view_id, &match_mask, 
                            TABLE_MANAGEMENT_ACTION_SETf, action_set);
        soc_mem_field32_set(unit, view_id, &match_data, 
                            TABLE_MANAGEMENT_ACTION_SETf, action_set);
    }

    soc_mem_field32_set(unit, view_id, &match_mask, EVEN_PARITYf,
                        1);

    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_MATCH_MASK_INX, 
                        &match_mask));

    field_len = soc_mem_field_length(unit, L2_BULKm, DEST_TYPEf);
    soc_mem_field32_set(unit, L2_BULKm, &repl_mask, DEST_TYPEf,
                        (1 << field_len) - 1);

    action_set = 0;
    soc_format_field32_set(unit, DESTINATION_FORMATfmt,
            &action_set, DGPPf,
            SOC_MEM_FIF_DGPP_MOD_ID_MASK | SOC_MEM_FIF_DGPP_PORT_MASK);
    soc_mem_field32_set(unit, view_id, &repl_mask,
                        DESTINATION_ACTION_SETf, action_set);
    action_set = 0;
    if (rep_st->new_dest.trunk != -1) {
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
            &action_set, DEST_TYPE2f, DEST_TYPE2_LAG);
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
            &action_set, LAGf, 
            rep_st->new_dest.trunk);
    } else {
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
            &action_set, DEST_TYPE0f, DEST_TYPE0_DGPP);
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
            &action_set, DGPPf, 
            rep_st->new_dest.module << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | 
            rep_st->new_dest.port);
    }
    soc_mem_field32_set(unit, view_id, &repl_data,
                        DESTINATION_ACTION_SETf, action_set);

    soc_mem_field32_set(unit, L2_BULKm, &repl_mask, EVEN_PARITYf,
                        1);
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_REPLACE_MASK_INX, 
                        &repl_mask));

    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, L2_BULK_CONTROLr, REG_PORT_ANY, ACTIONf,
                                2));
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, L2_BULK_CONTROLr, REG_PORT_ANY,
                            NUM_ENTRIESf, soc_mem_index_count(unit, view_id)));

    /* Replace all entries having EVEN_PARITY == 0 */
    /* EVEN_PARITY field in data is 0 */
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_MATCH_DATA_INX, 
                        &match_data));

    soc_mem_field32_set(unit, L2_BULKm, &repl_data, EVEN_PARITYf,
                        1);
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_REPLACE_DATA_INX, 
                        &repl_data));

    BCM_IF_ERROR_RETURN(soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr));

    /* Replace all entries having EVEN_PARITY == 1 */
    soc_mem_field32_set(unit, L2_BULKm, &match_data, EVEN_PARITYf,
                        1);
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_MATCH_DATA_INX, 
                        &match_data));

    soc_mem_field32_set(unit, L2_BULKm, &repl_data, EVEN_PARITYf,
                        0);
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_REPLACE_DATA_INX,
                        &repl_data));

    BCM_IF_ERROR_RETURN(soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr));

    return BCM_E_NONE;
}

#if defined(INCLUDE_L3)
int
_bcm_td3_l2_change_fields_egress_key_set(
    int unit,
    soc_mem_t mem,
    bcm_l2_change_fields_t *l2_info,
    uint32 *entry)
{
    int rv = BCM_E_NONE;
    uint32 data_type = 0;

    if (soc_mem_field_valid(unit, mem, BASE_VALIDf)) {
        soc_mem_field32_set(unit, mem, entry, BASE_VALIDf, 1);
    } else {
        soc_mem_field32_set(unit, mem, entry, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, entry, BASE_VALID_1f, 7);
    }

    soc_mem_field32_set(unit, mem, entry, KEY_TYPEf,
        _BCM_EXLATE_CHANGE_L2_FIELDS_KEY_TYPE(unit));

    data_type = _BCM_EXLATE_CHANGE_L2_FIELDS_DATA_TYPE(unit);

    soc_mem_field32_set(unit, mem, entry, DATA_TYPEf, data_type);

    soc_mem_field32_set(unit, mem, entry,
             CHANGE_L2_FIELDS__OPAQUE_3f, l2_info->change_l2_fields_class_id);

    if (l2_info->flags & BCM_L2_CHANGE_FIELDS_DVP) {
        uint32 dvp;
        /* validate DVP */
        if (BCM_GPORT_IS_VXLAN_PORT(l2_info->dvp)) {
            dvp = BCM_GPORT_VXLAN_PORT_ID_GET(l2_info->dvp);
        }
        else if (BCM_GPORT_IS_FLOW_PORT(l2_info->dvp)) {
            dvp = BCM_GPORT_FLOW_PORT_ID_GET(l2_info->dvp);
        }
        else if (BCM_GPORT_IS_L2GRE_PORT(l2_info->dvp)) {
            dvp = BCM_GPORT_L2GRE_PORT_ID_GET(l2_info->dvp);
        }
        else if (BCM_GPORT_IS_MIM_PORT(l2_info->dvp)) {
            dvp = BCM_GPORT_MIM_PORT_ID_GET(l2_info->dvp);
        }
        else if (BCM_GPORT_IS_MPLS_PORT(l2_info->dvp)) {
            dvp = BCM_GPORT_MPLS_PORT_ID_GET(l2_info->dvp);
        }
        else {
            /* unregonized gport */
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, mem, entry,
             CHANGE_L2_FIELDS__DVPf, dvp);
    }

    return rv;
}

STATIC int
_bcm_td3_l2_change_fields_tag_set(
        int unit,
        bcm_l2_change_fields_t *info,
        soc_mem_t mem,
        uint32 *entry,
        int *tpid_index)
{
    int action_present = 0;
    int action_not_present = 0;
    int sd_tag_remark_cfi_set = 0;
    int old_tpid_inx = 0;
    uint32 profile_index = 0;
    soc_field_t vlan_f = CHANGE_L2_FIELDS__VIDf;
    soc_field_t pri_f = CHANGE_L2_FIELDS__PCPf;
    soc_field_t cfi_f = CHANGE_L2_FIELDS__DEf;
    soc_field_t pri_cfi_sel_f = CHANGE_L2_FIELDS__PCP_DE_SELf;
    soc_field_t tag_act_f = CHANGE_L2_FIELDS__TAG_ACTION_PROFILE_PTRf;
    soc_field_t tpid_f = CHANGE_L2_FIELDS__TPID_INDEXf;

    if (info->vlan >= BCM_VLAN_INVALID) {
        return  BCM_E_PARAM;
    }
    soc_mem_field32_set(unit, mem, entry, vlan_f, info->vlan);

    if (info->pri > BCM_PRIO_MAX) {
        return  BCM_E_PARAM;
    }
    soc_mem_field32_set(unit, mem, entry, pri_f, info->pri);

    if (info->cfi > 1) {
       return  BCM_E_PARAM;
    }
    soc_mem_field32_set(unit, mem, entry, cfi_f, info->cfi);

    if (info->flags & BCM_L2_CHANGE_FIELDS_VLAN_ADD) {
        sd_tag_remark_cfi_set = TRUE;
        action_not_present = 0x1;  /* ADD */
    }

    if (info->flags & BCM_L2_CHANGE_FIELDS_VLAN_TPID_REPLACE) {
        action_present = 0x1; /* REPLACE_VID_TPID */

    } else if (info->flags & BCM_L2_CHANGE_FIELDS_VLAN_REPLACE) {
        action_present = 0x2;  /* REPLACE_VID_ONLY */

    } else if (info->flags & BCM_L2_CHANGE_FIELDS_VLAN_DELETE) {
        action_present = 0x3;  /* DELETE */

    } else if (info->flags & BCM_L2_CHANGE_FIELDS_VLAN_PRI_TPID_REPLACE) {
        sd_tag_remark_cfi_set = TRUE;
        action_present = 0x4;  /* REPLACE_VID_PRI_TPID */

    } else if (info->flags & BCM_L2_CHANGE_FIELDS_VLAN_PRI_REPLACE){
        sd_tag_remark_cfi_set = TRUE;
        action_present = 0x5;  /* REPLACE_VID_PRI */

    } else if (info->flags & BCM_L2_CHANGE_FIELDS_PRI_REPLACE ) {
        sd_tag_remark_cfi_set = TRUE;
        action_present = 0x6;  /*  REPLACE_PRI_ONLY */

    } else if (info->flags & BCM_L2_CHANGE_FIELDS_TPID_REPLACE) {
        action_present = 0x7;  /* REPLACE_TPID_ONLY */
    }

    soc_mem_field32_set(unit, mem, entry, pri_cfi_sel_f, sd_tag_remark_cfi_set);
    profile_index = soc_mem_field32_get(unit, mem, entry, tag_act_f);

    _bcm_td3_sd_tag_action_profile_get(unit, action_present,
            action_not_present, &profile_index);

    soc_mem_field32_set(unit, mem, entry, tag_act_f, profile_index);

    if ((info->flags & BCM_L2_CHANGE_FIELDS_VLAN_ADD) ||
        (info->flags & BCM_L2_CHANGE_FIELDS_VLAN_TPID_REPLACE) ||
        (info->flags & BCM_L2_CHANGE_FIELDS_TPID_REPLACE) ||
        (info->flags & BCM_L2_CHANGE_FIELDS_VLAN_PRI_TPID_REPLACE)) {
        if (info->flags & BCM_L2_CHANGE_FIELDS_REPLACE) {
            old_tpid_inx = soc_mem_field32_get(unit, mem, entry, tpid_f);
            BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_delete(unit,
                                old_tpid_inx));
        }

        /* TPID value is used */
        BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_add(unit, info->tpid,
                             tpid_index));
        soc_mem_field32_set(unit, mem, entry, tpid_f, *tpid_index);
    }
    return  BCM_E_NONE;
}

STATIC void
_bcm_td3_l2_change_fields_tag_get(
        int unit,
        bcm_l2_change_fields_t *info,
        soc_mem_t mem,
        uint32 *entry,
        int  *tpid_idx)
{
    int action_present = 0;
    int action_not_present = 0;
    int tpid_index = 0;
    uint32 profile_index = 0;
    int rv = BCM_E_NONE;
    soc_field_t vlan_f = CHANGE_L2_FIELDS__VIDf;
    soc_field_t pri_f = CHANGE_L2_FIELDS__PCPf;
    soc_field_t cfi_f = CHANGE_L2_FIELDS__DEf;
    soc_field_t tag_act_f = CHANGE_L2_FIELDS__TAG_ACTION_PROFILE_PTRf;
    soc_field_t tpid_f = CHANGE_L2_FIELDS__TPID_INDEXf;

    info->vlan = soc_mem_field32_get(unit, mem, entry, vlan_f);
    info->pri = soc_mem_field32_get(unit, mem, entry, pri_f);
    info->cfi = soc_mem_field32_get(unit, mem, entry, cfi_f);

    profile_index = soc_mem_field32_get(unit, mem, entry, tag_act_f);

    (void)_bcm_td3_sd_tag_action_get(unit, profile_index, &action_present,
                    &action_not_present);

    if (action_not_present) {
        info->flags |= BCM_L2_CHANGE_FIELDS_VLAN_ADD;
    }

    if (action_present == 0x1) {
        info->flags |= BCM_L2_CHANGE_FIELDS_VLAN_TPID_REPLACE;

    } else if (action_present == 0x2) {
        info->flags |= BCM_L2_CHANGE_FIELDS_VLAN_REPLACE;

    } else if (action_present == 0x3) {
        info->flags |= BCM_L2_CHANGE_FIELDS_VLAN_DELETE;

    } else if (action_present == 0x4) {
        info->flags |= BCM_L2_CHANGE_FIELDS_VLAN_PRI_TPID_REPLACE;

    } else if (action_present == 0x5) {
        info->flags |= BCM_L2_CHANGE_FIELDS_VLAN_PRI_REPLACE;

    } else if (action_present == 0x6) {
        info->flags |= BCM_L2_CHANGE_FIELDS_PRI_REPLACE;

    } else if (action_present == 0x7) {
        info->flags |= BCM_L2_CHANGE_FIELDS_TPID_REPLACE;
    }

    if ((info->flags & BCM_L2_CHANGE_FIELDS_VLAN_ADD) ||
        (info->flags & BCM_L2_CHANGE_FIELDS_VLAN_TPID_REPLACE) ||
        (info->flags & BCM_L2_CHANGE_FIELDS_TPID_REPLACE) ||
        (info->flags & BCM_L2_CHANGE_FIELDS_VLAN_PRI_TPID_REPLACE)) {
        tpid_index = soc_mem_field32_get(unit, mem, entry, tpid_f);

        rv = _bcm_fb2_outer_tpid_entry_get(unit, &info->tpid, tpid_index);
        if (BCM_FAILURE(rv)) {
            info->tpid = tpid_index;
        }
        if (tpid_idx != NULL) {
            *tpid_idx = tpid_index;
        }
    }
}

STATIC int
_bcm_td3_l2_change_fields_egress_entry_set(
    int unit,
    soc_mem_t mem,
    bcm_l2_change_fields_t *l2_info,
    uint32 *entry)
{
    int rv = BCM_E_NONE;
    int index = -1;
    uint32 return_entry[SOC_MAX_MEM_WORDS];
    int new_entry_add = FALSE;
    uint32   mac_da_field[2];
    int tpid_index = -1;

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, entry,
                        return_entry, 0);

    if (BCM_E_NONE == rv) {
        /* found memory entry */
        if (l2_info->flags & BCM_L2_CHANGE_FIELDS_REPLACE) {
            sal_memcpy(entry,return_entry,SOC_MAX_MEM_WORDS * 4);
        } else {
            return BCM_E_EXISTS;
        }
    } else if (BCM_E_NOT_FOUND == rv) {
        if (l2_info->flags & BCM_L2_CHANGE_FIELDS_REPLACE) {
            return BCM_E_NOT_FOUND;
        } else {
            new_entry_add = TRUE;
        }
    } else {
        return rv;
    }

    soc_mem_mac_addr_set(unit, mem, entry,
        CHANGE_L2_FIELDS__MAC_SAf, l2_info->macsa);

    SAL_MAC_ADDR_TO_UINT32(l2_info->macda, mac_da_field);

    soc_mem_field32_set(unit, mem, entry,
       CHANGE_L2_FIELDS__MAC_DA_27_0f, EXTRACT_MAC_DA_27_0(mac_da_field));

    soc_mem_field32_set(unit, mem, entry,
        CHANGE_L2_FIELDS__MAC_DA_47_28f, EXTRACT_MAC_DA_47_28(mac_da_field));

    BCM_IF_ERROR_RETURN(
        _bcm_td3_l2_change_fields_tag_set(
            unit,
            l2_info,
            mem,
            entry,
            &tpid_index));

    if (new_entry_add) {
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, entry);
    } else { /* replace action */
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
    }

    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }
    return rv;

cleanup:
    if (tpid_index != -1) {
        (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
    }
    return rv;
}

int
_bcm_td3_l2_change_fields_add(
    int unit,
    bcm_l2_change_fields_t *l2_info)
{
    soc_mem_t mem = EGR_VLAN_XLATE_2_DOUBLEm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int rv;

    sal_memset(entry, 0, sizeof(entry));

    BCM_IF_ERROR_RETURN(
        _bcm_td3_l2_change_fields_egress_key_set(unit,
             mem, l2_info, entry));

    soc_mem_lock(unit,mem);

    /* set the match object and attributes and write to memory */
    rv = _bcm_td3_l2_change_fields_egress_entry_set(unit, mem,
                        l2_info, entry);

    soc_mem_unlock(unit,mem);

    return rv;
}

STATIC void
_bcm_td3_l2_change_fields_egress_data_get(
    int unit,
    soc_mem_t mem,
    uint32* entry,
    bcm_l2_change_fields_t *l2_info)
{
    uint32   mac_da_field[2];

    soc_mem_mac_addr_get(unit, mem, entry, CHANGE_L2_FIELDS__MAC_SAf, l2_info->macsa);

    mac_da_field[0] = soc_mem_field32_get(unit, mem, entry,
         CHANGE_L2_FIELDS__MAC_DA_27_0f);

    mac_da_field[1] = soc_mem_field32_get(unit, mem, entry,
         CHANGE_L2_FIELDS__MAC_DA_47_28f);

    mac_da_field[0] = COMBINE_MAC_DA_32_0(mac_da_field);
    mac_da_field[1] = COMBINE_MAC_DA_47_32(mac_da_field);

    SAL_MAC_ADDR_FROM_UINT32(l2_info->macda, mac_da_field);

    _bcm_td3_l2_change_fields_tag_get(unit,
        l2_info, mem, entry, NULL);
}

int
_bcm_td3_l2_change_fields_egress_entry_get(
    int unit,
    soc_mem_t mem,
    uint32* entry,
    bcm_l2_change_fields_t *l2_info)
{
    int index = -1;
    uint32 return_entry[SOC_MAX_MEM_WORDS];

    SOC_IF_ERROR_RETURN(soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
              entry, return_entry, 0));

    _bcm_td3_l2_change_fields_egress_data_get(unit,
         mem, return_entry, l2_info);

    return BCM_E_NONE;
}

STATIC int
_bcm_td3_change_l2_fields_entry_parse(
    int unit,
    soc_mem_t mem,
    uint32 *entry,
    bcm_l2_change_fields_t *l2_info)
{
    uint32 key_type;
    uint32 dvp;

    key_type = soc_mem_field32_get(unit, mem, entry, KEY_TYPEf);

    if (key_type != _BCM_EXLATE_CHANGE_L2_FIELDS_KEY_TYPE(unit)) {
        return BCM_E_NOT_FOUND;
    }

    l2_info->change_l2_fields_class_id = soc_mem_field32_get(unit,
        mem, entry, CHANGE_L2_FIELDS__OPAQUE_3f);

    dvp = soc_mem_field32_get(unit, mem, entry, CHANGE_L2_FIELDS__DVPf);

    if (_bcm_vp_used_get(unit, dvp, _bcmVpTypeVxlan)) {
        BCM_GPORT_VXLAN_PORT_ID_SET(l2_info->dvp, dvp);
        l2_info->flags |= BCM_L2_CHANGE_FIELDS_DVP;
    } else if (_bcm_vp_used_get(unit, dvp, _bcmVpTypeFlow)) {
        BCM_GPORT_FLOW_PORT_ID_SET(l2_info->dvp, dvp);
        l2_info->flags |= BCM_L2_CHANGE_FIELDS_DVP;
    } else if (_bcm_vp_used_get(unit, dvp, _bcmVpTypeL2Gre)) {
        BCM_GPORT_L2GRE_PORT_ID_SET(l2_info->dvp, dvp);
        l2_info->flags |= BCM_L2_CHANGE_FIELDS_DVP;
    } else if (_bcm_vp_used_get(unit, dvp, _bcmVpTypeMim)) {
        BCM_GPORT_MIM_PORT_ID_SET(l2_info->dvp, dvp);
        l2_info->flags |= BCM_L2_CHANGE_FIELDS_DVP;
    } else if (_bcm_vp_used_get(unit, dvp, _bcmVpTypeMpls)) {
        BCM_GPORT_MPLS_PORT_ID_SET(l2_info->dvp, dvp);
        l2_info->flags |= BCM_L2_CHANGE_FIELDS_DVP;
    }

    _bcm_td3_l2_change_fields_egress_data_get(unit,
        mem, entry, l2_info);

    return BCM_E_NONE;
}

int _bcm_td3_change_l2_fields_entry_traverse(
    int unit,
    soc_mem_t mem,
    bcm_l2_change_fields_traverse_cb cb,
    void *user_data)
{

    /* Indexes to iterate over memories, chunks and entries */
    int             chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int             buf_size, chunksize, chnk_end;
    /* Buffer to store chunk of table we currently work on */
    uint32          *ls_tbl_chnk;
    uint32          *entry;
    int             valid, rv = BCM_E_NONE;
    bcm_l2_change_fields_t l2_info;

    if (INVALIDm == mem) {
        return BCM_E_MEMORY;
    }
    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = 256;

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    ls_tbl_chnk = soc_cm_salloc(unit, buf_size, "change l2 fields traverse");
    if (NULL == ls_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem);
         chnk_idx <= mem_idx_max;
         chnk_idx += chunksize) {
        sal_memset((void *)ls_tbl_chnk, 0, buf_size);

        chnk_idx_max =
            ((chnk_idx + chunksize) <= mem_idx_max) ?
            chnk_idx + chunksize - 1: mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, ls_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            entry =
                soc_mem_table_idx_to_pointer(unit, mem, uint32 *,
                                             ls_tbl_chnk, ent_idx);
            rv = _bcm_td3_evxlt_entry_valid(unit, mem, entry, &valid);
            if (BCM_FAILURE(rv)) {
                break;
            }
            if (!valid) {
                continue;
            }

            bcm_l2_change_fields_t_init(&l2_info);

            rv =_bcm_td3_change_l2_fields_entry_parse(unit, mem, entry, &l2_info);
            if (rv == BCM_E_NOT_FOUND) { /* not a large scale nat entry */
                continue;
            } else if (BCM_FAILURE(rv)) {
                break;
            }
            rv = cb(unit, &l2_info, user_data);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }
    soc_cm_sfree(unit, ls_tbl_chnk);
    return rv;
}
#ifdef BCM_WARM_BOOT_SUPPORT
int
_bcm_td3_l2_change_fields_reinit(int unit)
{
    /*
     * Recover VLAN Tag Action Profile and TPID ref counts from
     * EGR_VLAN_XLATE_2_DOUBLE::CHANGE_L2_FIELDS view.
     */
    int rv = BCM_E_NONE;
    char   *egr_xlate_buf = NULL;
    int i, index_min, index_max;
    uint32 entry_type = 0;
    egr_vlan_xlate_2_double_entry_t *egr_xlate_entry;
    soc_mem_t egr_mem = EGR_VLAN_XLATE_2_DOUBLEm;
    int profile_index, tpid_idx;
    uint32 data_type = 0;

    egr_xlate_buf = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, EGR_VLAN_XLATE_2_DOUBLEm), "EGR VLAN XLATE buffer");

    if (NULL == egr_xlate_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, egr_mem);
    index_max = soc_mem_index_max(unit, egr_mem);
    rv = soc_mem_read_range(unit, egr_mem, MEM_BLOCK_ANY,
            index_min, index_max, egr_xlate_buf);

    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    if (SOC_IS_HELIX5(unit)) {
        data_type = _BCM_HX5_EXLATE_CHANGE_L2_FIELDS_DATA_TYPE;
    } else {
        data_type = _BCM_TD3_EXLATE_CHANGE_L2_FIELDS_DATA_TYPE;
    }

    for (i = index_min; i <= index_max; i++) {
        egr_xlate_entry = soc_mem_table_idx_to_pointer(unit, egr_mem,
                egr_vlan_xlate_2_double_entry_t *, egr_xlate_buf, i);

        /* Check entry type */
        entry_type = soc_mem_field32_get(unit, egr_mem, egr_xlate_entry,
                DATA_TYPEf);

        if (entry_type == data_type) {
            profile_index = soc_mem_field32_get(unit, egr_mem, egr_xlate_entry,
                  CHANGE_L2_FIELDS__TAG_ACTION_PROFILE_PTRf);

            if (profile_index != 0) {
                _bcm_trx_egr_vlan_action_profile_entry_increment(unit,
                    profile_index);
            }
            tpid_idx = soc_mem_field32_get(unit, egr_mem,
                egr_xlate_entry, CHANGE_L2_FIELDS__TPID_INDEXf);

            rv = _bcm_fb2_outer_tpid_tab_ref_count_add(unit,tpid_idx,1);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }
cleanup:
    if (egr_xlate_buf) {
        soc_cm_sfree(unit, egr_xlate_buf);
    }
    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* INCLUDE_L3 */

#endif /* BCM_TRIDENT3_SUPPORT */
