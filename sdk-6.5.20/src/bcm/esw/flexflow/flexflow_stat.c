/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        flexflow_stat.c
 * Purpose:     Flex flow stat APIs.
 */

#include <shared/bsl.h>
#include <soc/defs.h>

#ifdef INCLUDE_L3

#include <assert.h>
#include <bcm/error.h>
#include <soc/error.h>
#include <bcm/types.h>
#include <soc/drv.h>
#include <soc/format.h>
#include <bcm/stat.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/flow.h>
#include <soc/esw/flow_db.h>
#include <soc/esw/flow_db_core.h>
#include <bcm_int/esw/l3.h>
#include <soc/l3x.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/l2gre.h>

#define _BCM_FLOW_TUNNEL_ID_IF_INVALID_RETURN(unit, id) \
    do {                                                 \
        if (((id) < 0) || (id) >= (soc_mem_index_count((unit), \
                 EGR_DVP_ATTRIBUTEm))) { \
            return (BCM_E_BADID);                        \
        }                                                \
    } while(0)

#define BCM_FLOW_IS_FIXED_TABLE(info) \
        ((info)->function_type == SOC_FLOW_DB_FUNC_ID_START)

#define BCM_FLOW_IS_FIXED_VIEW(info) \
        ((info)->flow_option == SOC_FLOW_DB_FLOW_OP_MATCH_NONE)

#define BCM_FLEXFLOW_FLEX_COUNTER_MAX_TABLE 3
STATIC
 int _bcm_esw_flow_stat_mem_direction_get(int unit,
                                          soc_mem_t mem,
                                          uint32 *direction);
STATIC
int _bcm_esw_flow_stat_info_validate(int unit,
                            bcm_flow_stat_info_t *info);

STATIC
int _bcm_esw_flow_l3_host_index_get(int unit,
                               bcm_flow_stat_info_t *info,
                               bcm_flow_logical_field_t *field,
                               uint32 num_of_fields,
                               int *index);
STATIC
int _bcm_esw_flow_stat_fixed_table_index_get(int unit,
                       bcm_flow_stat_info_t *info,
                       bcm_stat_flex_direction_t direction,
                       soc_mem_t *mem,
                       int  *index);
STATIC
int _bcm_esw_flow_stat_table_index_get(int unit,
                            soc_flow_db_mem_view_info_t vinfo,
                            uint32 *entry,
                            uint32 is_flex_view,
                            soc_mem_t mem,
                            int  *index);
STATIC
int _bcm_esw_flow_stat_fixed_view_index_get(int unit,
                            bcm_flow_stat_info_t *info,
                            uint32 num_of_fields,
                            bcm_flow_logical_field_t *field,
                            bcm_stat_flex_direction_t direction,
                            int attach,
                            soc_mem_t *mem,
                            int  *index);
STATIC
int _bcm_esw_flow_stat_flex_view_index_get(int unit,
                            bcm_flow_stat_info_t *info,
                            bcm_flow_logical_field_t *field,
                            uint32 num_of_fields,
                            soc_flow_db_mem_view_info_t vinfo,
                            bcm_stat_flex_direction_t direction,
                            int attach,
                            int  *index);
STATIC
int bcmi_flow_mac_addr_is_valid(bcm_mac_t addr)
{
    uint32 idx = 0;
    for (idx = 0; idx < 5; idx++) {
        if (addr[idx]) {
           return TRUE;
        }
    }
    return FALSE;
}

/*
 * Function:
 *      _bcm_esw_flow_stat_flex_mem_is_valid
 * Purpose:
 *      Checks if the object resolved table matches the
 *      resolved fixed table, fixed view or flexible
 *      view memory
 *
 *      unit                - (IN)     unit number.
 *      info      - (IN)     flow stat config structure.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
STATIC
 int _bcm_esw_flow_stat_flex_mem_is_valid(int unit,
                                     soc_mem_t mem,
                                     uint32 num_of_tables,
                                     soc_mem_t *tables,
                                     uint32 *valid)
{
    int i = 0;

    if ((mem == INVALIDm) ||
        (tables == NULL) ||
        (valid == NULL)) {
        return  BCM_E_INTERNAL;
    }

    *valid = FALSE;
    for (i = 0; i < num_of_tables; i++) {
        if (mem == tables[i]) {
            *valid = TRUE;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

STATIC
 int _bcm_esw_flow_stat_mem_direction_get(int unit,
                                          soc_mem_t mem,
                                          uint32 *direction)
{
    int rv = BCM_E_NONE;

    if((mem == INVALIDm)||
       (direction == NULL)) {
        return BCM_E_PARAM;
    }

    switch(mem) {
    case PORT_TABm:
    case LPORT_TABm:
    case VLAN_TABm:
    case VLAN_XLATEm:
    case VLAN_XLATE_1_DOUBLEm:
    case VLAN_XLATE_2_DOUBLEm:
    case VFIm:
    case VRFm:
    case VRF_ATTRS_2m:
    case SOURCE_VPm:
    case MPLS_ENTRYm:
    case MPLS_ENTRY_EXTDm:
    case VFP_POLICY_TABLEm:
    case VFP_POLICY_TABLE_PIPE0m:
    case VFP_POLICY_TABLE_PIPE1m:
    case VFP_POLICY_TABLE_PIPE2m:
    case VFP_POLICY_TABLE_PIPE3m:
    case L3_TUNNELm:
    case L3_ENTRY_2m:
    case L3_ENTRY_4m:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_IPV6_MULTICASTm:
    case L3_ENTRY_QUADm:
    case L3_DEFIPm:
    case L3_DEFIP_PAIR_128m:
    case L3_DEFIP_ALPM_IPV4_1m:
    case L3_DEFIP_ALPM_IPV6_64_1m:
    case L3_DEFIP_ALPM_IPV6_128m:
         *direction = bcmStatFlexDirectionIngress;
         break;

    case EGR_PORTm:
    case EGR_LPORT_PROFILEm:
    case EGR_VLANm:
    case EGR_VLAN_XLATEm:
    case EGR_VLAN_XLATE_1_DOUBLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
    case EGR_VFIm:
    case EGR_L3_NEXT_HOPm:
    case EGR_DVP_ATTRIBUTE_1m:
    case EGR_DVP_ATTRIBUTEm:
    case EGR_NAT_PACKET_EDIT_INFOm:
    case EFP_POLICY_TABLEm:
    case EFP_POLICY_TABLE_PIPE0m:
    case EFP_POLICY_TABLE_PIPE1m:
    case  EGR_IP_TUNNEL_MPLSm:
         *direction = bcmStatFlexDirectionEgress;
         break;
    default:
         rv = BCM_E_PARAM;
         break;
    }
    return rv;
}

STATIC
int _bcm_esw_flow_stat_info_validate(int unit,
                            bcm_flow_stat_info_t *info)
{

    /* verify against the valid bitmap */
    if (info->valid_elements & BCM_FLOW_STAT_FLOW_PORT_VALID) {
        if (0 == info->flow_port) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and flow_port field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_PORT_VALID) {
        if (0 == info->port) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and port field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_VPN_VALID) {
        if (0 == info->vpn) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and vpn field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_INNER_VLAN_VALID) {
        if (0 == info->inner_vlan) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and inner_vlan field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_VLAN_VALID) {
        if (0 == info->vlan) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and vlan field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_VNID_VALID) {
        if (0 == info->vnid) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and vnid field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_SIP_VALID) {
        if (0 == info->sip) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and sip field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_DIP_VALID) {
        if (0 == info->dip) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and dip field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_SIP_MASK_VALID) {
        if (0 == info->sip_mask) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and sip_mask field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_DIP_MASK_VALID) {
        if (0 == info->dip_mask) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and dip_mask field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_SIP6_VALID) {
        if (0 == bcmi_flow_ip6_addr_is_valid(info->sip6)) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and sip6 field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_DIP6_VALID) {
        if (0 == bcmi_flow_ip6_addr_is_valid(info->dip6)) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and dip6 field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_SIP6_MASK_VALID) {
        if (0 == bcmi_flow_ip6_addr_is_valid(info->sip6_mask)) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and sip6_mask field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_DIP6_MASK_VALID) {
        if (0 == bcmi_flow_ip6_addr_is_valid(info->dip6_mask)) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and dip6_mask field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_UDP_SRC_PORT_VALID) {
        if (0 == info->udp_src_port) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and udp_src_port field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_UDP_DST_PORT_VALID) {
        if (0 == info->udp_dst_port) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and udp_dst_port field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_PROTOCOL_VALID) {
        if (0 == info->protocol) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and protocol field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_TUNNEL_ID_VALID) {
        if (0 == info->tunnel_id) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and tunnel_id field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_EGRESS_IF_VALID) {
        if (0 == info->egress_if) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and egress_if field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_INTF_ID_VALID) {
        if (0 == info->intf_id) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and intf_id field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_MAC_VALID) {
        if (0 == bcmi_flow_mac_addr_is_valid(info->mac)) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and mac field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_L3A_FLAGS_VALID) {
        LOG_DEBUG(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             No Range check for L3A flags \
             Since 0 is also valid value")));

    }
    if (info->valid_elements & BCM_FLOW_STAT_L3A_VRF_VALID) {
        if (0 == info->l3a_vrf) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and l3a_vrf field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_DVP_GROUP_VALID) {
        if (0 == info->dvp_group) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and dvp_group field\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->valid_elements & BCM_FLOW_STAT_OIF_GROUP_VALID) {
        if (0 == info->oif_group) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
            "Flow Stat : \
             mismatch valid_elements flag and oif_group field\n")));
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

STATIC
int _bcm_esw_flow_l3_host_index_get(int unit,
                               bcm_flow_stat_info_t *info,
                               bcm_flow_logical_field_t *field,
                               uint32 num_of_fields,
                               int *index)
{
    int rv = BCM_E_NONE;
    _bcm_l3_cfg_t l3cfg;

    BCM_IF_ERROR_RETURN(bcmi_l3_init_check(unit));

    if (info->function_type != bcmFlowFuncTypeL3Host) {
        return BCM_E_PARAM;
    }

    if (info->valid_elements &  BCM_FLOW_STAT_L3A_VRF_VALID) {
        if ((info->l3a_vrf > SOC_VRF_MAX(unit)) ||
            (info->l3a_vrf < BCM_L3_VRF_GLOBAL)) {
            return (BCM_E_PARAM);
        }

    }

    sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));

    if (info->valid_elements &  BCM_FLOW_STAT_FLEX_KEY_VALID) {
        l3cfg.l3c_flow_handle = info->flow_handle;
        l3cfg.l3c_flow_option_handle = info->flow_option;
        l3cfg.l3c_num_of_fields = num_of_fields;
        sal_memcpy(l3cfg.l3c_logical_fields, field,
                   num_of_fields*sizeof(bcm_flow_logical_field_t));
    }
    if (info->valid_elements &  BCM_FLOW_STAT_L3A_FLAGS_VALID) {
        /*  Check that device supports ipv6. */
        if (BCM_L3_NO_IP6_SUPPORT(unit, info->l3a_flags)) {
            return (BCM_E_UNAVAIL);
        }

        L3_LOCK(unit);
        l3cfg.l3c_flags = info->l3a_flags;
        l3cfg.l3c_vrf = info->l3a_vrf;
        if ((info->l3a_flags & BCM_L3_IP6) &&
            (info->valid_elements & BCM_FLOW_STAT_SIP6_VALID)) {
            sal_memcpy(l3cfg.l3c_ip6, info->sip6, BCM_IP6_ADDRLEN);
            rv = mbcm_driver[unit]->mbcm_l3_ip6_get(unit, &l3cfg);
        } else if (info->valid_elements & BCM_FLOW_STAT_SIP_VALID) {
            l3cfg.l3c_ip_addr = info->sip;
            rv = mbcm_driver[unit]->mbcm_l3_ip4_get(unit, &l3cfg);

        }
        L3_UNLOCK(unit);
    }

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    *index = l3cfg.l3c_hw_index;
    return BCM_E_NONE;
}

STATIC
int _bcm_esw_flow_l3_route_index_get(int unit,
                               bcm_flow_stat_info_t *info,
                               bcm_flow_logical_field_t *field,
                               uint32 num_of_fields,
                               int *index)
{
    int max_prefix_length;
    _bcm_defip_cfg_t defip;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(bcmi_l3_init_check(unit));

    if (info->function_type == bcmFlowFuncTypeL3Route) {
        return BCM_E_PARAM;
    }
    if (info->valid_elements &  BCM_FLOW_STAT_L3A_VRF_VALID) {
        if ((info->l3a_vrf > SOC_VRF_MAX(unit)) ||
            (info->l3a_vrf < BCM_L3_VRF_GLOBAL)) {
            return (BCM_E_PARAM);
        }

    }
    sal_memset(&defip, 0, sizeof(_bcm_defip_cfg_t));
    if (info->valid_elements &  BCM_FLOW_STAT_FLEX_KEY_VALID) {

        defip.defip_flow_handle = info->flow_handle;
        defip.defip_flow_option_handle = info->flow_option;
        defip.defip_num_of_fields = num_of_fields;
        sal_memcpy(defip.defip_logical_fields, field,
                   num_of_fields*sizeof(bcm_flow_logical_field_t));
    }
    if (info->valid_elements &  BCM_FLOW_STAT_L3A_FLAGS_VALID) {
        /*  Check that device supports ipv6. */
        if (BCM_L3_NO_IP6_SUPPORT(unit, info->l3a_flags)) {
            return (BCM_E_UNAVAIL);
        }

        L3_LOCK(unit);
        if ((info->l3a_flags & BCM_L3_IP6) &&
            (info->valid_elements & BCM_FLOW_STAT_SIP6_VALID) &&
            (info->valid_elements & BCM_FLOW_STAT_SIP6_MASK_VALID)){
            max_prefix_length =
            soc_feature(unit, soc_feature_lpm_prefix_length_max_128) ? 128 : 64;
            /* copy the subnet address */
            sal_memcpy(defip.defip_ip6_addr, info->sip6, BCM_IP6_ADDRLEN);
            defip.defip_sub_len    = bcm_ip6_mask_length(info->sip6_mask);
            if (defip.defip_sub_len > max_prefix_length) {
                L3_UNLOCK(unit);
                return (BCM_E_PARAM);
            }
            rv = mbcm_driver[unit]->mbcm_ip6_defip_cfg_get(unit, &defip);
        } else if ((info->valid_elements & BCM_FLOW_STAT_SIP_VALID) &&
                   (info->valid_elements & BCM_FLOW_STAT_SIP_MASK_VALID)) {
            defip.defip_ip_addr = info->sip & info->sip_mask;
            defip.defip_sub_len = bcm_ip_mask_length(info->sip_mask);
            rv = mbcm_driver[unit]->mbcm_ip4_defip_cfg_get(unit, &defip);
        }
        L3_UNLOCK(unit);
    }

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    *index = defip.defip_l3hw_index;
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flow_stat_fixed table_index_get
 * Purpose:
 *      Get the index for the fixed tables like SVP
 *      EGR_VFI,VFI,VC_SWAP
 *      unit                - (IN)     unit number.
 *      info      - (IN)     flow stat config structure.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
STATIC
int _bcm_esw_flow_stat_fixed_table_index_get(int unit,
                       bcm_flow_stat_info_t *info,
                       bcm_stat_flex_direction_t direction,
                       soc_mem_t *mem,
                       int  *index)
{
    int vfi = -1;
    int vp;

    /* Resolve the index */
    if (info->valid_elements & BCM_FLOW_STAT_FLOW_PORT_VALID)
    {
        vp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
            LOG_ERROR(BSL_LS_BCM_FLOW,
               (BSL_META_U(unit, "Flow stats : flow port ID is not valid\n")));

            return BCM_E_PORT;
        }
        *mem = SOURCE_VPm;
        *index = vp;
    }
    if (info->valid_elements & BCM_FLOW_STAT_VPN_VALID)
    {
        /*Validate the flow VPN */
        BCM_IF_ERROR_RETURN(bcmi_esw_flow_vpn_is_valid(unit, info->vpn));
        _BCM_FLOW_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, info->vpn);
        if (direction == bcmStatFlexDirectionIngress) {
            *mem = VFIm;
        } else {
            *mem = EGR_VFIm;
        }
        *index = vfi;
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_esw_flow_stat_table_index_get
 * Purpose:
 *      Get the index for the fixed tables like SVP
 *      EGR_VFI,VFI,VC_SWAP
 *      unit                - (IN)     unit number.
 *      info      - (IN)     flow stat config structure.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
STATIC
int _bcm_esw_flow_stat_table_index_get(int unit,
                            soc_flow_db_mem_view_info_t vinfo,
                            uint32 *entry,
                            uint32 is_flex_view,
                            soc_mem_t mem,
                            int  *index)
{
    uint32  mem_view_id = 0;
    soc_tunnel_term_t tnl_entry;
    soc_tunnel_term_t tnl_return_entry;
    uint32 return_entry[SOC_MAX_MEM_WORDS];

    sal_memset(&tnl_entry, 0, sizeof(soc_tunnel_term_t));
    sal_memset(&tnl_return_entry, 0, sizeof(soc_tunnel_term_t));
    sal_memset(return_entry, 0, sizeof(return_entry));

    if (is_flex_view) {
       /* flex view case */
        mem_view_id = vinfo.mem_view_id;
        if (vinfo.type == SOC_FLOW_DB_VIEW_TYPE_TCAM) {
            if (vinfo.mem == L3_TUNNELm) {
                BCM_IF_ERROR_RETURN(
                bcmi_esw_flow_entry_to_tnl_term(unit, entry, &tnl_entry));
                BCM_IF_ERROR_RETURN(
                     soc_tunnel_term_match(unit, &tnl_entry,
                              &tnl_return_entry, index));
            }
            else {
                return BCM_E_INTERNAL;
            }
        } else if (vinfo.type == SOC_FLOW_DB_VIEW_TYPE_HASH) {
            BCM_IF_ERROR_RETURN(
                soc_mem_search(unit, mem_view_id, MEM_BLOCK_ANY, index, entry,
                    return_entry, 0));
        } else if (vinfo.type == SOC_FLOW_DB_VIEW_TYPE_DIRECT) {
             return BCM_E_NONE;
        }
    } else {
        /* fixed view case
         * Assuming only hash tables
         * Other TCAM tables like L3_DEFIP is handled
         * seperately
         * L3 TUNNEL does not have fixed views
         * in case of CLASSIC VXLAN and CLASSIC L2GRE Flows.
         */
        BCM_IF_ERROR_RETURN(
            soc_mem_search(unit, mem, MEM_BLOCK_ANY, index, entry,
                return_entry, 0));

    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flow_stat_fixed_view_index_get
 * Purpose:
 *      Get the index for the fixed view of flex tables
 *      Usually programmed for the supported classic flows.
 *      unit                - (IN)     unit number.
 *      info      - (IN)     flow stat config structure.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
STATIC
int _bcm_esw_flow_stat_fixed_view_index_get(int unit,
                            bcm_flow_stat_info_t *info,
                            uint32 num_of_fields,
                            bcm_flow_logical_field_t *field,
                            bcm_stat_flex_direction_t direction,
                            int attach,
                            soc_mem_t *mem,
                            int  *index)
{
    int rv = BCM_E_NONE;
    uint32 key_type = 0;
    bcm_module_t mod_out = -1;
    bcm_port_t port_out = -1;
    bcm_trunk_t trunk_id = -1;
    bcm_port_t gport_id = -1;
    soc_field_t ovid_f;
    soc_field_t ivid_f;
    soc_field_t vnid_f;
    soc_field_t t_f;
    soc_field_t tgid_f;
    soc_field_t module_id_f;
    soc_field_t port_num_f;
    soc_field_t sip_f;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int soft_tnl_idx;
    _bcm_flow_bookkeeping_t *flow_info;
    uint32 mpath_flag=0;
    int nh_index = -1;
    int vfi = -1;
    soc_mem_t mem_id = 0;
    uint32 dvp = 0;
    uint32 vrf = 0;
    soc_field_t vfi_f;
    soc_field_t vrf_f;
    soc_field_t dvp_f;
    soc_field_t dvp_grp_f;
    soc_field_t oif_grp_f;
    soc_flow_db_mem_view_info_t vinfo;
    uint32 data_type;
    flow_info = FLOW_INFO(unit);

    sal_memset(entry, 0, sizeof(entry));

    if ((index == NULL) ||
        (mem == NULL) ||
        (info == NULL)) {
        return BCM_E_INTERNAL;
    }
    if (info->valid_elements &  BCM_FLOW_STAT_FLEX_KEY_VALID)
    {
        return BCM_E_PARAM;
    }

    if (info->function_type == bcmFlowFuncTypeMatch) {
        if ((info->flow_handle != SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) &&
            (info->flow_handle != SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE)){
             return BCM_E_PARAM;
        }

        t_f         = XLATE__Tf;
        tgid_f      = XLATE__TGIDf;
        module_id_f = XLATE__MODULE_IDf;
        port_num_f  = XLATE__PORT_NUMf;
        ovid_f      = XLATE__OVIDf;
        ivid_f      = XLATE__IVIDf;

        if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_PORT_VLAN) {
            mem_id = VLAN_XLATE_1_DOUBLEm;  /* fixed view */
            key_type = TR_VLXLT_HASH_KEY_TYPE_OVID;
        } else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_PORT_INNER_VLAN) {
            mem_id = VLAN_XLATE_1_DOUBLEm;
            key_type = TR_VLXLT_HASH_KEY_TYPE_IVID;
        } else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_PORT_VLAN_STACKED) {
            mem_id = VLAN_XLATE_1_DOUBLEm;
            key_type = TR_VLXLT_HASH_KEY_TYPE_IVID_OVID;
        } else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_VLAN_PRI) {
            mem_id = VLAN_XLATE_1_DOUBLEm;
            key_type = TR_VLXLT_HASH_KEY_TYPE_PRI_CFI;
        } else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_VN_ID) {
            mem_id = MPLS_ENTRYm;  /* fixed view */
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                    mem_id = VLAN_XLATE_1_DOUBLEm;
                    key_type = _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_FLEX_VNID;
                } else {
                    key_type = _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_VNID;
                }
            } else {
                key_type = _BCM_FLOW_MATCH_KEY_TYPE_L2GRE_VPNID;
            }
        } else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_SIP) {
            mem_id = MPLS_ENTRYm;  /* fixed view */
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                    mem_id = MPLS_ENTRY_SINGLEm;
                }
                key_type = _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_SIP;
            } else {
                key_type = _BCM_FLOW_MATCH_KEY_TYPE_L2GRE_SIP;
            }
        } else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_SIP_VNID) {
            mem_id = MPLS_ENTRYm;  /* fixed view */
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                    mem_id = VLAN_XLATE_1_DOUBLEm;
                    key_type = _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_FLEX_SIP_VNID;
                } else {
                    key_type = _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_SIP_VNID;
                }
            } else {
                key_type = _BCM_FLOW_MATCH_KEY_TYPE_L2GRE_SIP_VPNID;
            }
        }

        *mem = mem_id;
        soc_mem_field32_set(unit,mem_id, entry,  KEY_TYPEf,  key_type);
        if (key_type == _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_FLEX_VNID) {
            soc_mem_field32_set(unit, mem_id, entry, DATA_TYPEf,
                _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_FLEX_SIP_VNID);
        } else if (key_type == _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_SIP) {
            soc_mem_field32_set(unit, mem_id, entry, DATA_TYPEf,
                _BCM_VXLAN_FLEX_DATA_TYPE_IPV4_SIP);
        } else {
            soc_mem_field32_set(unit, mem_id, entry, DATA_TYPEf, key_type);
        }

        if (soc_mem_field_valid(unit, mem_id, BASE_VALIDf)) {
            soc_mem_field32_set(unit, mem_id, entry, BASE_VALIDf, 1);
        } else {
            soc_mem_field32_set(unit, mem_id, entry, BASE_VALID_0f, 3);
            soc_mem_field32_set(unit, mem_id, entry, BASE_VALID_1f, 7);
        }

        if (info->valid_elements & BCM_FLOW_STAT_PORT_VALID) {
            rv = _bcm_esw_gport_resolve(unit, info->port, &mod_out,
                            &port_out, &trunk_id, &gport_id);
            BCM_IF_ERROR_RETURN(rv);
        }
        if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_PORT_VLAN ||
            info->match_criteria == BCM_FLOW_MATCH_CRITERIA_PORT_INNER_VLAN ||
            info->match_criteria == BCM_FLOW_MATCH_CRITERIA_PORT_VLAN_STACKED ||
            info->match_criteria == BCM_FLOW_MATCH_CRITERIA_VLAN_PRI) {
            if (info->valid_elements & BCM_FLOW_STAT_PORT_VALID) {
                if (BCM_GPORT_IS_TRUNK(info->port)) {
                    soc_mem_field32_set(unit,mem_id, entry, t_f, 1);
                    soc_mem_field32_set(unit,mem_id, entry, tgid_f, trunk_id);
                } else {
                    if (mod_out != -1) {
                        soc_mem_field32_set(unit, mem_id, entry, module_id_f,
                                            mod_out);
                    }
                    soc_mem_field32_set(unit,mem_id, entry, port_num_f, port_out);
                }
            }
            soc_mem_field32_set(unit, mem_id, entry, SOURCE_TYPEf, 1);
        }

        if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_PORT_VLAN ) {
            if (!BCM_VLAN_VALID(info->vlan)) {
                LOG_ERROR(BSL_LS_BCM_FLOW,
                      (BSL_META_U(unit, "Invalid vlan\n")));
                 return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, ovid_f, info->vlan);

        } else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_PORT_INNER_VLAN) {
            if (!BCM_VLAN_VALID(info->inner_vlan)) {
                LOG_ERROR(BSL_LS_BCM_FLOW,
                      (BSL_META_U(unit, "Invalid inner vlan\n")));
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, ivid_f, info->inner_vlan);

        } else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_PORT_VLAN_STACKED) {
            if (!BCM_VLAN_VALID(info->vlan) ||
                    !BCM_VLAN_VALID(info->inner_vlan)) {
                LOG_ERROR(BSL_LS_BCM_FLOW,
                 (BSL_META_U(unit, "Invalid either outer vlan or inner vlan\n")));
                return BCM_E_PARAM;
            }

            soc_mem_field32_set(unit,mem_id, entry, ovid_f, info->vlan);
            soc_mem_field32_set(unit,mem_id, entry, ivid_f, info->inner_vlan);

        } else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_VLAN_PRI) {

            /* match_vlan : Bits 12-15 contains VLAN_PRI + CFI, vlan=BCM_E_NONE */
            soc_mem_field32_set(unit,mem_id, entry, OTAGf, info->vlan);

        }  else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_SIP) {
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                    sip_f  = VXLAN_FLEX_IPV4_SIP__SIPf;
                } else {
                    ovid_f = VXLAN_SIP__OVIDf;
                    sip_f  = VXLAN_SIP__SIPf;
                }
            } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                sip_f  = L2GRE_SIP__SIPf;
            } else {
                LOG_ERROR(BSL_LS_BCM_FLOW,
                      (BSL_META_U(unit, "Invalid legacy flow handle\n")));
                return BCM_E_PARAM;
            }
            if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination) &&
                (info->valid_elements & BCM_FLOW_STAT_VLAN_VALID)) {
                VLAN_CHK_ID(unit,info->vlan);
                soc_mem_field32_set(unit,mem_id, entry, ovid_f, info->vlan);
            }
            soc_mem_field32_set(unit,mem_id, entry, sip_f, info->sip);

        }  else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_VN_ID ||
                    info->match_criteria == BCM_FLOW_MATCH_CRITERIA_SIP_VNID) {

            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                    ovid_f = VXLAN_FLEX__OVIDf;
                    sip_f  = VXLAN_FLEX__SIPf;
                    vnid_f = VXLAN_FLEX__VNIDf;
                } else {
                    ovid_f = VXLAN_VN_ID__OVIDf;
                    sip_f  = VXLAN_VN_ID__SIPf;
                    vnid_f = VXLAN_VN_ID__VN_IDf;
                }
            } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                vnid_f = L2GRE_VPNID__VPNIDf;
                sip_f   = L2GRE_VPNID__SIPf;
            } else {
                LOG_ERROR(BSL_LS_BCM_FLOW,
                      (BSL_META_U(unit, "Invalid legacy flow handle\n")));
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, mem_id, entry, vnid_f, info->vnid);
            if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_SIP_VNID) {
                soc_mem_field32_set(unit,mem_id, entry, sip_f, info->sip);
            }

            if (soc_feature(unit,soc_feature_vrf_aware_vxlan_termination) &&
                (info->valid_elements & BCM_FLOW_STAT_VLAN_VALID)) {
                VLAN_CHK_ID(unit, info->vlan);
                soc_mem_field32_set(unit,mem_id, entry, ovid_f, info->vlan);
            }
        }
    /* End of Match */
    } else if (info->function_type == bcmFlowFuncTypeEncap) {
        if ((info->flow_handle != SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) &&
            (info->flow_handle != SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE)){
             return BCM_E_PARAM;
        }
        mem_id = EGR_VLAN_XLATE_1_DOUBLEm;
        if (info->encap_criteria == BCM_FLOW_ENCAP_CRITERIA_VRF_MAPPING) {
            key_type = _BCM_FLOW_ENCAP_KEY_TYPE_VRF_MAPPING;
        } else if (info->encap_criteria == BCM_FLOW_ENCAP_CRITERIA_VFI) {
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                key_type = _BCM_FLOW_ENCAP_VXLAN_KEY_TYPE_VFI;
            } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                key_type = _BCM_FLOW_ENCAP_L2GRE_KEY_TYPE_VFI;
            }
        } else if (info->encap_criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP) {
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                key_type = _BCM_FLOW_ENCAP_VXLAN_KEY_TYPE_VFI_DVP;
            } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                key_type = _BCM_FLOW_ENCAP_L2GRE_KEY_TYPE_VFI_DVP;
            } else if (!(info->flow_handle)) {  /* access DVP */
                key_type = _BCM_FLOW_ENCAP_KEY_TYPE_VLAN_XLATE_VFI;
            }
        } else if (info->encap_criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP) {
            key_type = _BCM_FLOW_ENCAP_KEY_TYPE_VFI_DVP_GROUP;
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN ||
                info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
            } else if (!(info->flow_handle)) {  /* access DVP */
                key_type = _BCM_FLOW_ENCAP_KEY_TYPE_VLAN_XLATE_VFI;
            }
        }
        *mem = mem_id;
        soc_mem_field32_set(unit, mem_id, entry,  KEY_TYPEf,  key_type);
        soc_mem_field32_set(unit, mem_id, entry, DATA_TYPEf, key_type);

        soc_mem_field32_set(unit, mem_id, entry, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem_id, entry, BASE_VALID_1f, 7);

        if (info->valid_elements & BCM_FLOW_STAT_FLOW_PORT_VALID) {
            dvp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
        }
        if (info->valid_elements & BCM_FLOW_STAT_VPN_VALID) {
            _BCM_FLOW_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, info->vpn);
        }
        if (info->encap_criteria == BCM_FLOW_ENCAP_CRITERIA_VRF_MAPPING) {
            /* KEY : VRF,VT_DVP_GROUP_ID,VT_L3_OIF_GROUP_ID
             * VT_DVP_GROUP_ID    : from EGR_DVP_ATTRIBUTEm
             * VT_L3_OIF_GROUP_ID : from EGR_L3_INTFm
             */
            _BCM_FLOW_VPN_GET(vrf, _BCM_VPN_TYPE_L3, info->vpn);
            vrf_f = VRF_MAPPING__VRFf;
            dvp_grp_f = VRF_MAPPING__VT_DVP_GROUP_IDf;
            oif_grp_f = VRF_MAPPING__VT_L3OIF_GROUP_IDf;
            if (info->valid_elements & BCM_FLOW_STAT_VPN_VALID) {
                soc_mem_field32_set(unit,mem_id, entry, vrf_f, vrf);
            }
            if (info->valid_elements & BCM_FLOW_STAT_DVP_GROUP_VALID) {
                soc_mem_field32_set(unit,mem_id, entry, dvp_grp_f, info->dvp_group);
            }
            if (info->valid_elements & BCM_FLOW_STAT_OIF_GROUP_VALID) {
                soc_mem_field32_set(unit,mem_id, entry, oif_grp_f, info->oif_group);
            }
        } else if (info->encap_criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP) {
            vfi_f = VFI_DVP_GROUP__VFIf;
            dvp_grp_f = VFI_DVP_GROUP__VT_DVP_GROUP_IDf;
            if (info->valid_elements & BCM_FLOW_STAT_VPN_VALID) {
                soc_mem_field32_set(unit,mem_id, entry, vfi_f, vfi);
            }
            if (info->valid_elements & BCM_FLOW_STAT_DVP_GROUP_VALID) {
                soc_mem_field32_set(unit,mem_id, entry, dvp_grp_f, info->dvp_group);
            }
            soc_mem_field32_set(unit,mem_id, entry,
                                 VFI_DVP_GROUP__DATA_TYPEf, 0);
        } else if (info->encap_criteria == BCM_FLOW_ENCAP_CRITERIA_VFI) {
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                vfi_f = VXLAN_VFI__VFIf;
            } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                vfi_f  = L2GRE_VFI__VFIf;
            } else {
                return BCM_E_PARAM;
            }
            if (info->valid_elements & BCM_FLOW_STAT_VPN_VALID) {
                soc_mem_field32_set(unit,mem_id, entry, vfi_f, vfi);
            }
        }  else if (info->encap_criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP) {
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                vfi_f = VXLAN_VFI__VFIf;
                dvp_f = VXLAN_VFI__DVPf;
            } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                vfi_f = L2GRE_VFI__VFIf;
                dvp_f = L2GRE_VFI__DVPf;
            } else {
                vfi_f = VLAN_XLATE_VFI__VFIf;
                dvp_f = VLAN_XLATE_VFI__DVPf;
            }
            if (info->valid_elements & BCM_FLOW_STAT_VPN_VALID) {
                soc_mem_field32_set(unit, mem_id, entry, vfi_f, vfi);
            }
            if (info->valid_elements & BCM_FLOW_STAT_FLOW_PORT_VALID) {
                soc_mem_field32_set(unit, mem_id, entry, dvp_f, dvp);
            }
        }
    } else if (info->function_type == bcmFlowFuncTypeTunnelInit) {
        /* Tunnel intitator id - validate the tunnel index*/
        if ((info->flow_handle != SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) &&
            (info->flow_handle != SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE)){
             return BCM_E_PARAM;
        }
        soft_tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
        _BCM_FLOW_TUNNEL_ID_IF_INVALID_RETURN(unit, soft_tnl_idx);
        if (!flow_info->init_tunnel[soft_tnl_idx].idx) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
               "Invalid replace action, tunnel not created\n")));
            return BCM_E_PARAM;
        }
        *mem = EGR_IP_TUNNELm;
        *index = flow_info->init_tunnel[soft_tnl_idx].idx;
        return BCM_E_NONE;

    } else if (info->function_type == bcmFlowFuncTypeTunnelTerm) {
        if ((info->flow_handle != SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) &&
            (info->flow_handle != SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE)){
             return BCM_E_PARAM;
        }
        mem_id = VLAN_XLATE_1_DOUBLEm;
        *mem = mem_id;
        if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                mem_id = MPLS_ENTRY_SINGLEm;
                *mem = mem_id;
                soc_mem_field32_set(unit, mem_id, entry, BASE_VALIDf, 1);
                soc_mem_field32_set(unit, mem_id, entry, DATA_TYPEf,
                                    _BCM_VXLAN_FLEX_KEY_TYPE_LOOKUP_DIP);
                soc_mem_field32_set(unit, mem_id, entry, KEY_TYPEf, _BCM_VXLAN_FLEX_KEY_TYPE_LOOKUP_DIP);
                soc_mem_field32_set(unit, mem_id, entry, VXLAN_FLEX_IPV4_DIP__DIPf, info->dip);
            } else {
                soc_mem_field32_set(unit, mem_id, entry, BASE_VALID_0f, 3);
                soc_mem_field32_set(unit, mem_id, entry, BASE_VALID_1f, 7);
                soc_mem_field32_set(unit, mem_id, entry, DATA_TYPEf,
                                    _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP);
                soc_mem_field32_set(unit, mem_id, entry, KEY_TYPEf, _BCM_VXLAN_KEY_TYPE_LOOKUP_DIP);
                soc_mem_field32_set(unit, mem_id, entry, VXLAN_DIP__DIPf, info->dip);
                if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) {
                    soc_mem_field32_set(unit, mem_id, entry,
                                    VXLAN_DIP__OVIDf, info->vlan);
                }
            }
        } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {

            soc_mem_field32_set(unit, mem_id, entry, BASE_VALID_0f, 3);
            soc_mem_field32_set(unit, mem_id, entry, BASE_VALID_1f, 7);
            soc_mem_field32_set(unit, mem_id, entry, KEY_TYPEf,
                                     _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP);
            soc_mem_field32_set(unit, mem_id, entry, DATA_TYPEf,
                                    _BCM_TD2_L2GRE_KEY_TYPE_LOOKUP_DIP);
            soc_mem_field32_set(unit, mem_id, entry,
                                L2GRE_DIP__DIPf, info->dip);
        }
    } else if (info->function_type == bcmFlowFuncTypeEgressObj) {
       if (info->valid_elements & BCM_FLOW_STAT_EGRESS_IF_VALID) {
            /* Get the nexthop index from the egress object
             * info->egress_if
             */
            rv = bcm_xgs3_get_nh_from_egress_object(unit, info->egress_if,
                                                 &mpath_flag,
                                                 attach,
                                                 &nh_index);
            BCM_IF_ERROR_RETURN(rv);
            if (attach == 0) {
                /* Decrement next hop reference count */
                int ref_count;
                rv = bcm_xgs3_get_ref_count_from_nhi(unit, mpath_flag, &ref_count, nh_index);
            }
            *mem = EGR_L3_NEXT_HOPm;
            *index = nh_index;
            return BCM_E_NONE;
        }
    } else if (info->function_type == bcmFlowFuncTypeEgressIntf) {
        if (info->valid_elements &  BCM_FLOW_STAT_INTF_ID_VALID) {
            /* index to EGR_L3_INTF */
            *mem = EGR_L3_INTFm;
            *index =  info->intf_id;
            return BCM_E_NONE;
        }

    } else if (info->function_type == bcmFlowFuncTypeL2Switch) {
        if (info->valid_elements &  BCM_FLOW_STAT_MAC_VALID) {
            /* L2_ENTRYm */
           *mem = mem_id = L2_ENTRY_SINGLEm;
           soc_mem_mac_addr_set(unit, mem_id, entry, MAC_DAf, info->mac);
        }
    } else if (info->function_type ==  bcmFlowFuncTypeL3Route) {
        *mem = L3_DEFIPm;
        return _bcm_esw_flow_l3_route_index_get(unit, info,
                            field, num_of_fields, index);
    } else if (info->function_type ==  bcmFlowFuncTypeL3Host) {
        *mem = L3_ENTRY_DOUBLEm;
        return _bcm_esw_flow_l3_host_index_get(unit, info,
                            field, num_of_fields, index);
    } else if (info->function_type ==  bcmFlowFuncTypeEgressLabel) {
        return BCM_E_NONE;
    } else if (info->function_type ==  bcmFlowFuncTypeEncapSet) {
        if ((info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) ||
             (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE)) {
            if (info->valid_elements ==  BCM_FLOW_STAT_FLOW_PORT_VALID) {
               mem_id = EGR_DVP_ATTRIBUTEm;
               dvp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
               if(!_BCM_FLOW_EGR_DVP_USED_GET(unit,dvp)) {
                   return BCM_E_NOT_FOUND;
               }
               BCM_IF_ERROR_RETURN(soc_mem_read(unit,mem_id, MEM_BLOCK_ANY, dvp,
                                         entry));
               data_type = soc_mem_field32_get(unit, mem_id, entry, DATA_TYPEf);
               if ((data_type == _BCM_FLOW_VXLAN_EGRESS_DEST_VP_TYPE) ||
                   (data_type == _BCM_FLOW_L2GRE_EGRESS_DEST_VP_TYPE)) {
                    *index = dvp;
                    *mem = mem_id;
                    return BCM_E_NONE;
               }
               return BCM_E_PARAM;
            } else {
                return BCM_E_PARAM;
            }
        } else {
            return BCM_E_PARAM;
        }
    } else {
        return BCM_E_PARAM;
    }
    /* Get the Hw index
     */
    rv = _bcm_esw_flow_stat_table_index_get(unit, vinfo/*vinfo*/,
                          entry, 0, mem_id, index);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flow_stat_flex_view_index_get
 * Purpose:
 *      Get the index for the flex view of flex tables
 *      Usually programmed for the supported flex flows.
 *      unit                - (IN)     unit number.
 *      info      - (IN)     flow stat config structure.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */

STATIC
int _bcm_esw_flow_stat_flex_view_index_get(int unit,
                            bcm_flow_stat_info_t *info,
                            bcm_flow_logical_field_t *field,
                            uint32 num_of_fields,
                            soc_flow_db_mem_view_info_t vinfo,
                            bcm_stat_flex_direction_t direction,
                            int attach,
                            int  *index)
{
    int rv = BCM_E_NONE;
    uint32  mem_view_id = 0;
    int i = 0;
    int j = 0;
    bcm_module_t mod_out = -1;
    bcm_port_t port_out = -1;
    bcm_trunk_t trunk_id = -1;
    soc_field_t ovid_f;
    soc_field_t ivid_f;
    soc_field_t vnid_f;
    soc_field_t t_f;
    soc_field_t tgid_f;
    soc_field_t module_id_f;
    soc_field_t port_num_f;
    soc_field_t lbl_f;
    soc_field_t vfi_f;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 key_lg_list[_BCM_FLOW_LOGICAL_FIELD_MAX];
    uint32 num_lg_keys = 0;
    uint32 key_list[_BCM_FLOW_LOGICAL_FIELD_MAX];
    uint32 num_keys = 0;
    int soft_tnl_idx;
    _bcm_flow_bookkeeping_t *flow_info;
    uint32 mpath_flag=0;
    int nh_index = -1;
    int vfi = -1;
    uint32 dvp = 0;
    uint32 vrf = 0;
    soc_field_t vrf_f = 0;
    soc_field_t dvp_grp_f;
    soc_field_t oif_grp_f;
    soc_field_t dvp_f;
    soc_field_t sip_f;
    bcm_port_t gport_id = -1;

    flow_info = FLOW_INFO(unit);

    mem_view_id = vinfo.mem_view_id;

    /* needed for flex function match */
    sal_memset(entry, 0, sizeof(entry));
    BCM_IF_ERROR_RETURN(
      soc_flow_db_mem_view_entry_init(unit, mem_view_id, entry));
    if (info->function_type == bcmFlowFuncTypeL3Route) {
        return _bcm_esw_flow_l3_route_index_get(unit, info,
                                field, num_of_fields, index);
    } else if (info->function_type == bcmFlowFuncTypeL3Host) {
        return _bcm_esw_flow_l3_host_index_get(unit, info,
                                field, num_of_fields, index);
    } else if (info->function_type == bcmFlowFuncTypeEncapSet) {
        if (info->valid_elements &
            BCM_FLOW_STAT_FLOW_PORT_VALID) {
            dvp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
            if (!_BCM_FLOW_EGR_DVP_USED_GET(unit, dvp)) {
                LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                  "DVP %d not programmed for Egress DVP \n"), dvp));
                return BCM_E_NOT_FOUND;
            }
            if (!_bcm_vp_used_get(unit, dvp, _bcmVpTypeFlow)) {
                LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                  "DVP %d is not flow port, create flow port first\n"), dvp));
                return BCM_E_NOT_FOUND;
            }
            *index = dvp;
            return BCM_E_NONE;
        }
    } else if (info->function_type ==  bcmFlowFuncTypeEncap) {
        if (info->valid_elements &  BCM_FLOW_STAT_FLOW_PORT_VALID) {
            dvp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
            if (!_bcm_vp_used_get(unit, dvp, _bcmVpTypeFlow)) {
                LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                   "DVP %d is not the flow port\n"), dvp));
                return BCM_E_NOT_FOUND;
            }
        }
        if (info->valid_elements & BCM_FLOW_STAT_VPN_VALID) {
            _BCM_FLOW_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, info->vpn);
        }

        if (info->encap_criteria == BCM_FLOW_ENCAP_CRITERIA_DVP) {
            if (!dvp) {
                 return BCM_E_PARAM;
            }
            /* only apply to flex view */
            if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
                return BCM_E_PARAM;
            }
            if (SOC_MEM_FIELD_VALID(unit, mem_view_id, DVPf)) {
                soc_mem_field32_set(unit, mem_view_id, entry, DVPf, dvp);
            } else {
               LOG_VERBOSE(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                 "DVPf not in memory view definition\n")));
                return BCM_E_PARAM;
            }

        } else if (info->encap_criteria ==
                BCM_FLOW_ENCAP_CRITERIA_L3_INTF) {

            if (info->valid_elements & BCM_FLOW_STAT_INTF_ID_VALID) {
                /* only apply to flex view */
                if (SOC_MEM_FIELD_VALID(unit,mem_view_id, L3_OIFf)) {
                    soc_mem_field32_set(unit,mem_view_id, entry, L3_OIFf, info->intf_id);
                } else {
                    LOG_VERBOSE(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                     "L3_OIFf not in memory view definition\n")));
                    return BCM_E_PARAM;
                }
            }
        } else if (info->encap_criteria == BCM_FLOW_ENCAP_CRITERIA_VRF_MAPPING) {
            /* KEY : VRF,VT_DVP_GROUP_ID,VT_L3_OIF_GROUP_ID
             * VT_DVP_GROUP_ID    : from EGR_DVP_ATTRIBUTEm
             * VT_L3_OIF_GROUP_ID : from EGR_L3_INTFm
             */
            _BCM_FLOW_VPN_GET(vrf, _BCM_VPN_TYPE_L3, info->vpn);
            vrf_f = VRFf;
            dvp_grp_f = VT_DVP_GROUP_IDf;
            oif_grp_f = VT_L3OIF_GROUP_IDf;
            if (info->valid_elements & BCM_FLOW_STAT_VPN_VALID) {
                soc_mem_field32_set(unit,mem_view_id, entry, vrf_f, vrf);
            }
            if (info->valid_elements & BCM_FLOW_STAT_DVP_GROUP_VALID) {
                soc_mem_field32_set(unit,mem_view_id, entry, dvp_grp_f, info->dvp_group);
            }
            if (info->valid_elements & BCM_FLOW_STAT_OIF_GROUP_VALID) {
                soc_mem_field32_set(unit,mem_view_id, entry, oif_grp_f, info->oif_group);
            }
        } else if (info->encap_criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP) {
            vfi_f = VFIf;
            dvp_grp_f = VT_DVP_GROUP_IDf;
            if (info->valid_elements & BCM_FLOW_STAT_VPN_VALID) {
                soc_mem_field32_set(unit,mem_view_id, entry, vfi_f, vfi);
            }
            if (info->valid_elements & BCM_FLOW_STAT_DVP_GROUP_VALID) {
                soc_mem_field32_set(unit,mem_view_id, entry, dvp_grp_f, info->dvp_group);
            }
        } else if (info->encap_criteria == BCM_FLOW_ENCAP_CRITERIA_VFI) {
	    vfi_f = VFIf;
            if (info->valid_elements & BCM_FLOW_STAT_VPN_VALID) {
                soc_mem_field32_set(unit,mem_view_id, entry, vfi_f, vfi);
            }
        }  else if (info->encap_criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP) {
            vfi_f = VFIf;
            dvp_f = DVPf;
            if (info->valid_elements & BCM_FLOW_STAT_VPN_VALID) {
                soc_mem_field32_set(unit, mem_view_id, entry, vfi_f, vfi);
            }
            if (info->valid_elements & BCM_FLOW_STAT_FLOW_PORT_VALID) {
                soc_mem_field32_set(unit, mem_view_id, entry, dvp_f, dvp);
            }
        } else if (info->encap_criteria == BCM_FLOW_ENCAP_CRITERIA_FLEX) {
            rv = soc_flow_db_mem_view_field_list_get(unit,
                          mem_view_id,
                          SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,
                          _BCM_FLOW_LOGICAL_FIELD_MAX, key_lg_list, &num_lg_keys);
            BCM_IF_ERROR_RETURN(rv);
            for (i = 0; i < num_of_fields; i++) {
                for (j = 0; j < num_lg_keys; j++) {
                    if (field[i].id == key_lg_list[j]) {
                        soc_mem_field32_set(unit,mem_view_id, entry,
                              field[i].id, field[i].value);
                        break;
                    }
                }
            }
            if (info->valid_elements & BCM_FLOW_STAT_FLOW_PORT_VALID) {
                soc_mem_field32_set(unit,mem_view_id, entry, DVPf, dvp);
            }
            if (info->valid_elements & BCM_FLOW_STAT_VPN_VALID) {
                soc_mem_field32_set(unit,mem_view_id, entry, VFIf, vfi);
            }
            if (info->valid_elements & BCM_FLOW_STAT_INTF_ID_VALID) {
                soc_mem_field32_set(unit,mem_view_id, entry, L3_OIFf, info->intf_id);
            }
            if (info->valid_elements & BCM_FLOW_STAT_DVP_GROUP_VALID) {
                soc_mem_field32_set(unit,mem_view_id, entry, VT_DVP_GROUP_IDf,
                        info->dvp_group);
            }
            if (info->valid_elements & BCM_FLOW_STAT_OIF_GROUP_VALID) {
                soc_mem_field32_set(unit,mem_view_id, entry, VT_L3OIF_GROUP_IDf,
                        info->oif_group);
            }
        }
    } else if (info->function_type ==  bcmFlowFuncTypeMatch) {

        t_f         = Tf;
        tgid_f      = TGIDf;
        module_id_f = MODULE_IDf;
        port_num_f  = PORT_NUMf;
        ovid_f      = OVIDf;
        ivid_f      = IVIDf;
        if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_PORT_VLAN ||
            info->match_criteria == BCM_FLOW_MATCH_CRITERIA_PORT_INNER_VLAN ||
            info->match_criteria == BCM_FLOW_MATCH_CRITERIA_PORT_VLAN_STACKED ||
            info->match_criteria == BCM_FLOW_MATCH_CRITERIA_VLAN_PRI) {
            if (info->valid_elements & BCM_FLOW_STAT_PORT_VALID) {
                BCM_IF_ERROR_RETURN(
                   _bcm_esw_gport_resolve(unit, info->port, &mod_out,
                        &port_out, &trunk_id, &gport_id));
                if (BCM_GPORT_IS_TRUNK(info->port)) {
                    soc_mem_field32_set(unit, mem_view_id, entry, t_f, 1);
                    soc_mem_field32_set(unit, mem_view_id, entry, tgid_f, trunk_id);
                } else {
                    if (mod_out != -1) {
                        soc_mem_field32_set(unit, mem_view_id, entry, module_id_f,
                                            mod_out);
                    }
                    soc_mem_field32_set(unit, mem_view_id, entry, port_num_f, port_out);
                }
            }
            soc_mem_field32_set(unit, mem_view_id, entry, SOURCE_TYPEf, 1);
        }
        if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_PORT_VLAN ) {
            if (!BCM_VLAN_VALID(info->vlan)) {
                LOG_ERROR(BSL_LS_BCM_FLOW,
                      (BSL_META_U(unit, "Invalid vlan\n")));
                 return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, mem_view_id, entry, ovid_f, info->vlan);

        } else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_PORT_INNER_VLAN) {
            if (!BCM_VLAN_VALID(info->inner_vlan)) {
                LOG_ERROR(BSL_LS_BCM_FLOW,
                      (BSL_META_U(unit, "Invalid inner vlan\n")));
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, mem_view_id, entry, ivid_f, info->inner_vlan);

        } else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_PORT_VLAN_STACKED) {
            if (!BCM_VLAN_VALID(info->vlan) ||
                    !BCM_VLAN_VALID(info->inner_vlan)) {
                LOG_ERROR(BSL_LS_BCM_FLOW,
                 (BSL_META_U(unit, "Invalid either outer vlan or inner vlan\n")));
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, mem_view_id, entry, ovid_f, info->vlan);
            soc_mem_field32_set(unit, mem_view_id, entry, ivid_f, info->inner_vlan);

        } else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_VLAN_PRI) {

            /* match_vlan : Bits 12-15 contains VLAN_PRI + CFI, vlan=BCM_E_NONE */
            soc_mem_field32_set(unit, mem_view_id, entry, OTAGf, info->vlan);

        } else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_SIP) {
            ovid_f = OVIDf;
            if (SOC_MEM_FIELD_VALID(unit, mem_view_id, IPV4__SIPf)) {
                sip_f = IPV4__SIPf;
            } else {
                sip_f = SIPf;
            }
            if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination) &&
                (info->valid_elements & BCM_FLOW_STAT_VLAN_VALID)) {
                VLAN_CHK_ID(unit,info->vlan);
                soc_mem_field32_set(unit, mem_view_id, entry, ovid_f, info->vlan);
            }
            soc_mem_field32_set(unit, mem_view_id, entry, sip_f, info->sip);

        }  else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_VN_ID ||
                    info->match_criteria == BCM_FLOW_MATCH_CRITERIA_SIP_VNID) {

            ovid_f = OVIDf;
            vnid_f = VNIDf;
            if (SOC_MEM_FIELD_VALID(unit, mem_view_id, IPV4__SIPf)) {
                sip_f = IPV4__SIPf;
            } else {
                sip_f = SIPf;
            }
            soc_mem_field32_set(unit, mem_view_id, entry, vnid_f, info->vnid);
            if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_SIP_VNID) {
                soc_mem_field32_set(unit, mem_view_id, entry, sip_f, info->sip);
            }

            if (soc_feature(unit,soc_feature_vrf_aware_vxlan_termination) &&
                (info->valid_elements & BCM_FLOW_STAT_VLAN_VALID)) {
                VLAN_CHK_ID(unit, info->vlan);
                soc_mem_field32_set(unit, mem_view_id, entry, ovid_f, info->vlan);
            }
        } else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_MPLS_LABEL) {
            if (SOC_MEM_FIELD_VALID(unit, mem_view_id, MPLS_LABEL_1f)) {
                lbl_f = MPLS_LABEL_1f;
            } else if (SOC_MEM_FIELD_VALID(unit, mem_view_id, MPLS_LABEL_2f)) {
                lbl_f = MPLS_LABEL_2f;
            } else {
                LOG_ERROR(BSL_LS_BCM_FLOW,
                 (BSL_META_U(unit, "mpls_label field not found\n")));
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, mem_view_id, entry, lbl_f, info->mpls_label);
        } else if (info->match_criteria == BCM_FLOW_MATCH_CRITERIA_FLEX) {
            ovid_f = OVIDf;
            vnid_f = VNIDf;

            rv = soc_flow_db_mem_view_field_list_get(unit,
                          mem_view_id,
                          SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,
                          _BCM_FLOW_LOGICAL_FIELD_MAX, key_lg_list, &num_lg_keys);
            if (rv != SOC_E_NONE && rv != SOC_E_NOT_FOUND) {
                return rv;
            }
            if (rv == SOC_E_NONE) {
                for (i = 0; i < num_of_fields; i++) {
                    for (j = 0; j < num_lg_keys; j++) {
                        if (field[i].id == key_lg_list[j]) {
                            soc_mem_field32_set(unit, mem_view_id, entry,
                                 field[i].id, field[i].value);
                            break;
                        }
                    }
                }
            }
            rv = SOC_E_NONE;
            if (info->valid_elements & BCM_FLOW_STAT_VNID_VALID) {
                if (!SOC_MEM_FIELD_VALID(unit, mem_view_id,vnid_f)) {
                    return SOC_E_PARAM;
                }
                soc_mem_field32_set(unit, mem_view_id, entry, vnid_f, info->vnid);
            }
            if (info->valid_elements & BCM_FLOW_STAT_VLAN_VALID) {
                if (!SOC_MEM_FIELD_VALID(unit, mem_view_id,ovid_f)) {
                    return SOC_E_PARAM;
                }
                soc_mem_field32_set(unit, mem_view_id, entry, ovid_f, info->vlan);
            }
            if (info->valid_elements & BCM_FLOW_STAT_INNER_VLAN_VALID) {
                if (!SOC_MEM_FIELD_VALID(unit, mem_view_id, ivid_f)) {
                    return SOC_E_PARAM;
                }
                soc_mem_field32_set(unit, mem_view_id, entry, ivid_f, info->inner_vlan);
            }
            if (info->valid_elements & BCM_FLOW_STAT_PORT_VALID) {
                if (BCM_GPORT_IS_TRUNK(info->port)) {
                    if (!(SOC_MEM_FIELD_VALID(unit, mem_view_id,t_f) &&
                         SOC_MEM_FIELD_VALID(unit, mem_view_id, tgid_f))) {
                        return SOC_E_PARAM;
                    }

                    soc_mem_field32_set(unit, mem_view_id, entry, t_f, 1);
                    soc_mem_field32_set(unit, mem_view_id, entry, tgid_f, trunk_id);
                } else {
                    if ((SOC_MEM_FIELD_VALID(unit, mem_view_id,module_id_f) &&
                         SOC_MEM_FIELD_VALID(unit, mem_view_id,port_num_f))) {
                        soc_mem_field32_set(unit, mem_view_id, entry, module_id_f,
                                             mod_out);
                        soc_mem_field32_set(unit, mem_view_id, entry, port_num_f,
                                             port_out);
                    } else if (SOC_MEM_FIELD_VALID(unit, mem_view_id, SGPPf)) {
                        soc_mem_field32_set(unit, mem_view_id, entry, SGPPf,
                                             port_out | (mod_out << 8));
                    } else {
                        return SOC_E_PARAM;
                    }
                }
            }
            if (info->valid_elements & BCM_FLOW_STAT_SIP_VALID) {
                if (SOC_MEM_FIELD_VALID(unit, mem_view_id, SIPf)) {
                    sip_f = SIPf;
                } else if (SOC_MEM_FIELD_VALID(unit, mem_view_id, IPV4__SIPf)) {
                    sip_f = IPV4__SIPf;
                } else {
                    return SOC_E_PARAM;
                }
                soc_mem_field32_set(unit, mem_view_id, entry, sip_f, info->sip);
            }
            if (info->valid_elements & BCM_FLOW_STAT_SIP6_VALID) {
                if (!SOC_MEM_FIELD_VALID(unit, mem_view_id,IPV6__SIPf)) {
                    return SOC_E_PARAM;
                }
                soc_mem_ip6_addr_set(unit, mem_view_id, entry,IPV6__SIPf, info->sip6, 0);
            }
            if (info->valid_elements & BCM_FLOW_STAT_DIP_VALID) {
                if (!SOC_MEM_FIELD_VALID(unit, mem_view_id,IPV4__DIPf)) {
                    return SOC_E_PARAM;
                }
                soc_mem_field32_set(unit, mem_view_id, entry, IPV4__DIPf, info->dip);
            }
            if (info->valid_elements & BCM_FLOW_STAT_DIP6_VALID) {
                if (!SOC_MEM_FIELD_VALID(unit, mem_view_id,IPV6__DIPf)) {
                    return SOC_E_PARAM;
                }
                soc_mem_ip6_addr_set(unit, mem_view_id, entry,IPV6__DIPf, info->dip6, 0);
            }
            if (info->valid_elements & BCM_FLOW_STAT_PROTOCOL_VALID) {
                if (!SOC_MEM_FIELD_VALID(unit, mem_view_id, IPV4__PROTOCOLf)) {
                    return SOC_E_PARAM;
                }
                soc_mem_field32_set(unit, mem_view_id, entry, IPV4__PROTOCOLf,
                        info->protocol);
            }
            if (info->valid_elements & BCM_FLOW_STAT_UDP_SRC_PORT_VALID) {
                if (!SOC_MEM_FIELD_VALID(unit, mem_view_id,L4_SRC_PORTf)) {
                    return SOC_E_PARAM;
                }
                soc_mem_field32_set(unit, mem_view_id, entry, L4_SRC_PORTf,
                        info->udp_src_port);
            }
            if (info->valid_elements & BCM_FLOW_STAT_UDP_DST_PORT_VALID) {
                if (!SOC_MEM_FIELD_VALID(unit, mem_view_id, L4_DST_PORTf)) {
                    return SOC_E_PARAM;
                }
                soc_mem_field32_set(unit, mem_view_id, entry, L4_DST_PORTf,
                        info->udp_dst_port);
            }
        }
    } else if (info->function_type ==  bcmFlowFuncTypeTunnelTerm) {
        /* flex view, initialize all control fields such as data_type */
        BCM_IF_ERROR_RETURN(soc_flow_db_mem_view_field_list_get(unit,
                            mem_view_id,
                            SOC_FLOW_DB_FIELD_TYPE_KEY,
                            _BCM_FLOW_LOGICAL_FIELD_MAX,
                            key_list,
                            &num_keys));
        for (i = 0; i < num_keys; i++) {
            if (key_list[i] == IPV4__DIPf) {
                if (info->valid_elements & BCM_FLOW_STAT_DIP_VALID) {
                    rv = (info->dip) ? BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem_view_id, entry,
                                     IPV4__DIPf, info->dip);
                }
            } else if (key_list[i] == IPV4__SIPf) {
                if (info->valid_elements & BCM_FLOW_STAT_SIP_VALID) {
                    rv = (info->sip) ? BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem_view_id, entry,
                                     IPV4__SIPf, info->sip);
                }
           } else if (key_list[i] == IPV4__DIP_MASKf) {
                if (info->valid_elements & BCM_FLOW_STAT_DIP_MASK_VALID) {
                    rv = (info->dip_mask) ? BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem_view_id, entry,
                                     IPV4__DIP_MASKf, info->dip);
                }
            } else if (key_list[i] == IPV4__SIP_MASKf) {
                if (info->valid_elements & BCM_FLOW_STAT_SIP_MASK_VALID) {
                    rv = (info->sip_mask) ? BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem_view_id, entry,
                                     IPV4__SIP_MASKf, info->sip_mask);
                }
            } else if (key_list[i] == L4_DEST_PORTf) {
                if(info->valid_elements & BCM_FLOW_STAT_UDP_DST_PORT_VALID) {
                    rv = (info->udp_dst_port) ? BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem_view_id, entry,
                                     L4_DEST_PORTf, info->udp_dst_port);
                }
            } else if (key_list[i] == L4_SRC_PORTf) {
                if(info->valid_elements & BCM_FLOW_STAT_UDP_SRC_PORT_VALID) {
                    rv = (info->udp_src_port) ? BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem_view_id, entry,
                           L4_SRC_PORTf, info->udp_src_port);
                }
            } else if (key_list[i] == PROTOCOLf) {
                if(info->valid_elements & BCM_FLOW_STAT_PROTOCOL_VALID) {
                    rv = (info->protocol) ? BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_field32_set(unit, mem_view_id, entry,
                          PROTOCOLf, info->protocol);
                }
            } else if (key_list[i] == IPV6__SIPf) {
                if (info->valid_elements & BCM_FLOW_STAT_SIP6_VALID) {
                    rv = (bcmi_flow_ip6_addr_is_valid(info->sip6)) ?
                          BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_ip6_addr_set(unit, mem_view_id, entry,
                               IPV6__SIPf, info->sip6, 0);
                }
            } else if (key_list[i] == IPV6__DIPf) {
                if (info->valid_elements & BCM_FLOW_STAT_DIP6_VALID) {
                    rv = (bcmi_flow_ip6_addr_is_valid(info->dip6)) ?
                          BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_ip6_addr_set(unit, mem_view_id, entry,
                             IPV6__DIPf, info->dip6, 0);
                }
             } else if (key_list[i] == IPV6__SIP_MASKf) {
                if (info->valid_elements & BCM_FLOW_STAT_SIP6_MASK_VALID) {
                    rv = (bcmi_flow_ip6_addr_is_valid(info->sip6_mask)) ?
                          BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_ip6_addr_set(unit, mem_view_id, entry,
                             IPV6__SIP_MASKf, info->sip6_mask, 0);
                }
            } else if (key_list[i] == IPV6__DIP_MASKf) {
                if (info->valid_elements & BCM_FLOW_STAT_DIP6_MASK_VALID) {
                    rv = (bcmi_flow_ip6_addr_is_valid(info->dip6_mask)) ?
                          BCM_E_NONE :  BCM_E_PARAM;
                    BCM_IF_ERROR_RETURN(rv);
                    soc_mem_ip6_addr_set(unit, mem_view_id, entry,
                             IPV6__DIP_MASKf, info->dip6_mask, 0);
                }
            }
        }
        if (info->valid_elements & BCM_FLOW_STAT_FLEX_KEY_VALID) {
            /* Logical key fields */
            BCM_IF_ERROR_RETURN(
                soc_flow_db_mem_view_field_list_get(unit,
                            mem_view_id,
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,
                            SOC_FLOW_DB_MAX_VIEW_FIELDS,
                            key_lg_list,
                            &num_lg_keys));
            for (i = 0;(i < num_of_fields); i++) {
                for (j = 0; j < num_lg_keys; j++) {
                    if (field[i].id == key_lg_list[j]) {
                        soc_mem_field32_set(unit, mem_view_id, entry,
                             field[i].id, field[i].value);
                    }
                }
            }
        }

    } else if ((info->function_type ==  bcmFlowFuncTypeTunnelInit) &&
          (info->valid_elements &  BCM_FLOW_STAT_TUNNEL_ID_VALID)) {
        /* Tunnel intitator id - validate the tunnel index*/
        soft_tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
        _BCM_FLOW_TUNNEL_ID_IF_INVALID_RETURN(unit, soft_tnl_idx);
        if (!flow_info->init_tunnel[soft_tnl_idx].idx) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
               "Invalid replace action, tunnel not created\n")));
            return BCM_E_PARAM;
        }
        *index = flow_info->init_tunnel[soft_tnl_idx].idx;
        
        return BCM_E_NONE;
    } else if ((info->function_type ==  bcmFlowFuncTypeEgressObj) &&
            (info->valid_elements &  BCM_FLOW_STAT_EGRESS_IF_VALID)) {
        /* Get the nexthop index from the egress object
         * info->egress_if
         */
        rv = bcm_xgs3_get_nh_from_egress_object(unit, info->egress_if,
                                             &mpath_flag,
                                             attach,
                                             &nh_index);
         BCM_IF_ERROR_RETURN(rv);
        *index = nh_index;
        if (attach == 0) {
            /* Decrement next hop reference count */
            int ref_count;
            rv = bcm_xgs3_get_ref_count_from_nhi(unit, mpath_flag, &ref_count, nh_index);
        }
        return BCM_E_NONE;
    } else if ((info->function_type == bcmFlowFuncTypeEgressIntf) &&
          (info->valid_elements &  BCM_FLOW_STAT_INTF_ID_VALID)) {
        /* index to EGR_L3_INTF */
        *index =  info->intf_id;
        return BCM_E_NONE;
    } else if ((info->function_type == bcmFlowFuncTypeL2Switch) &&
       (info->valid_elements &  BCM_FLOW_STAT_MAC_VALID)) {
        /* L2_ENTRYm */
       soc_mem_mac_addr_set(unit, mem_view_id, entry, MAC_DAf, info->mac);
    }
    rv = _bcm_esw_flow_stat_table_index_get(unit,
                          vinfo,
                          entry, 1, mem_view_id, index);

    return rv;
}

int bcmi_esw_flow_stat_object_get(int unit,
                          bcm_flow_handle_t flow_handle,
                          bcm_flow_option_id_t flow_option_id,
                          bcm_flow_function_type_t function_type,
                          bcm_stat_object_t *stat_object)
{
    int rv = BCM_E_NONE;
    uint32 mem_view_id = 0;
    soc_mem_t mem = 0;
    soc_flow_db_mem_view_info_t view_info;
    bcm_stat_flex_direction_t  direction;

    direction = bcmStatFlexDirectionIngress;
    /* get the view mem */
    /* mem - Ingress/Egress */

    rv = soc_flow_db_ffo_to_mem_view_id_get(unit,
                                flow_handle, flow_option_id,
                                function_type, &mem_view_id);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FLOW,
                    (BSL_META_U(unit,
                      "Invalid combination of flow, \
                      flow_option and function_type.\n")));
        return rv;
    }

    LOG_VERBOSE(BSL_LS_BCM_FLOW,
                    (BSL_META_U(unit,
                    "Stat object get for View ID %d \n"),
                    mem_view_id));
    BCM_IF_ERROR_RETURN(soc_flow_db_mem_view_info_get(unit, mem_view_id,
                                      &view_info));

    mem = view_info.mem;

    BCM_IF_ERROR_RETURN(_bcm_esw_flow_stat_mem_direction_get(unit, mem,
                                      &direction));
    if (direction == bcmStatFlexDirectionIngress) {
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_ingress_object(
                            unit, mem_view_id,
                            0,NULL,stat_object));
    } else if (direction == bcmStatFlexDirectionEgress) {
        BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_egress_object(
                                     unit, mem_view_id,
                                     0, NULL,stat_object));
    } else {
        return BCM_E_INTERNAL;
    }

    return rv;
}

int bcmi_esw_flow_stat_attach(int unit,
                          bcm_flow_stat_info_t *info,
                          uint32 num_of_fields,
                          bcm_flow_logical_field_t *field,
                          uint32 stat_counter_id)
{
    int rv = BCM_E_NONE;
    uint32 mem_view_id = 0;
    soc_mem_t mem = 0;
    soc_flow_db_mem_view_info_t vinfo;
    soc_mem_t                  table[BCM_FLEXFLOW_FLEX_COUNTER_MAX_TABLE];
    uint32                     act_num_tables = 0;
    uint32                     pool_number = 0;
    uint32                     base_index = 0;
    bcm_stat_flex_mode_t       offset_mode = 0;
    bcm_stat_object_t          object = bcmStatObjectIngPort;
    bcm_stat_group_mode_t      group_mode = bcmStatGroupModeSingle;
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    int                        index = -1;
    uint32                     valid = FALSE;


    /* resolve the index */

    sal_memset(table, 0, sizeof(table));
    BCM_IF_ERROR_RETURN(
        _bcm_esw_flow_stat_info_validate(unit,
                            info));

    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode, &object, &offset_mode, &pool_number, &base_index);

    /* Validate object and group mode */
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit, object, &direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit, group_mode));

    /* Get Table index to attach flexible counter */
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit, object, BCM_FLEXFLOW_FLEX_COUNTER_MAX_TABLE,
                        &act_num_tables, &table[0], &direction));
    LOG_INFO(BSL_LS_BCM_FLOW,
         (BSL_META_U(unit,"flex counter attach for object %d table %d\n"),
          object, mem));

    if (soc_flow_db_mem_view_flow_handle_valid(unit, info->flow_handle)) {
        return BCM_E_PARAM;
    }

    /* Flex Table - flex view */
    rv = soc_flow_db_ffo_to_mem_view_id_get(unit,
                                info->flow_handle,
                                info->flow_option,
                                info->function_type,
                                &mem_view_id);

    if (BCM_SUCCESS(rv)) {
        /* Flex Table - flex view */
        BCM_IF_ERROR_RETURN(
        soc_flow_db_mem_view_info_get(unit, mem_view_id, &vinfo));
        mem = vinfo.mem_view_id;

        BCM_IF_ERROR_RETURN(
          _bcm_esw_flow_stat_flex_mem_is_valid(unit,
                                          vinfo.mem,
                                          act_num_tables,
                                          &table[0],
                                          &valid));

        if (valid != TRUE) {
            LOG_ERROR(BSL_LS_BCM_FLOW,
                 (BSL_META_U(unit,
                  "Mismatch in stat_object_id %d resolved memory \
                  and resolved memory %d.\n"),
                  object, vinfo.mem));
            return BCM_E_PARAM;
        }

        /* Resolve the index from the key fields */
        BCM_IF_ERROR_RETURN(
        _bcm_esw_flow_stat_flex_view_index_get(unit, info,
                                         field, num_of_fields,
                                         vinfo, direction, 1, &index));


    } else if (BCM_FLOW_IS_FIXED_TABLE(info)) { /* == 0 */
        /* fixed tables SOURCE_VP, VFI, EGR_VFI
         * which are programmed as part of the flex flows
         */
        BCM_IF_ERROR_RETURN(
           _bcm_esw_flow_stat_fixed_table_index_get(unit,
                                             info,
                                             direction,
                                             &mem,
                                             &index));
        BCM_IF_ERROR_RETURN(
          _bcm_esw_flow_stat_flex_mem_is_valid(unit,
                                          mem,
                                          act_num_tables,
                                          &table[0],
                                          &valid));

        if (valid != TRUE) {
            LOG_ERROR(BSL_LS_BCM_FLOW,
                 (BSL_META_U(unit,
                  "Mismatch in stat_object_id %d resolved memory\
                  and resolved memory %d.\n"),
                  object, mem));
            return BCM_E_PARAM;
        }

    } else if (BCM_FLOW_IS_FIXED_VIEW(info)) {
        /* Flex table - fixed views */
        /* Classic flows */

        BCM_IF_ERROR_RETURN(
            _bcm_esw_flow_stat_fixed_view_index_get(unit,
                                            info, num_of_fields,
                                            field, direction, 1, &mem,
                                            &index));
        BCM_IF_ERROR_RETURN(
          _bcm_esw_flow_stat_flex_mem_is_valid(unit,
                                          mem,
                                          act_num_tables,
                                          &table[0],
                                          &valid));

        if (valid != TRUE) {
            LOG_ERROR(BSL_LS_BCM_FLOW,
                 (BSL_META_U(unit,
                  "Mismatch in stat_object_id %d resolved memory\
                  and resolved memory %d.\n"),
                  object, mem));
            return BCM_E_PARAM;
        }

    } else {
        LOG_ERROR(BSL_LS_BCM_FLOW,
                    (BSL_META_U(unit,
                      "Invalid combination of flow, \
                      flow_option and function_type \
                      or not a supported memory\n")));
        return BCM_E_PARAM;
    }


    if (direction == bcmStatFlexDirectionIngress) {
        rv = _bcm_esw_stat_flex_attach_ingress_table_counters(
                   unit,
                   mem,
                   index,
                   offset_mode,
                   base_index,
                   pool_number);
    } else {
        rv = _bcm_esw_stat_flex_attach_egress_table_counters(
                 unit,
                 mem,
                 index,
                 object,
                 offset_mode,
                 base_index,
                 pool_number);
    }

    return rv;
}

int bcmi_esw_flow_stat_detach(int unit,
                          bcm_flow_stat_info_t *info,
                          uint32 num_of_fields,
                          bcm_flow_logical_field_t *field,
                          uint32 stat_counter_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_error_t err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};
    uint32 mem_view_id = 0;
    soc_mem_t mem = 0;
    soc_flow_db_mem_view_info_t vinfo;
    bcm_stat_object_t          object = bcmStatObjectIngPort;
    soc_mem_t                  table[BCM_FLEXFLOW_FLEX_COUNTER_MAX_TABLE];
    uint32                     act_num_tables = 0;
    int                        index = -1;
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     pool_number = 0;
    uint32                     base_index = 0;
    bcm_stat_flex_mode_t       offset_mode = 0;
    bcm_stat_group_mode_t      group_mode = bcmStatGroupModeSingle;
    uint32                     valid = FALSE;


    sal_memset(table, 0, sizeof(table));
    BCM_IF_ERROR_RETURN(
        _bcm_esw_flow_stat_info_validate(unit,
                            info));

    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode, &object, &offset_mode, &pool_number, &base_index);

    /* Validate object and group mode */
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit, object, &direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit, group_mode));

    /* Get Table index to attach flexible counter */
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit, object, BCM_FLEXFLOW_FLEX_COUNTER_MAX_TABLE,
                        &act_num_tables, &table[0], &direction));
    LOG_INFO(BSL_LS_BCM_FLOW,
         (BSL_META_U(unit,"flex counter attach for object %d table %d\n"),
          object, mem));


    if (soc_flow_db_mem_view_flow_handle_valid(unit, info->flow_handle)) {
        return BCM_E_PARAM;
    }
    rv = soc_flow_db_ffo_to_mem_view_id_get(unit,
                                info->flow_handle,
                                info->flow_option,
                                info->function_type, &mem_view_id);

    if (BCM_SUCCESS(rv)) {
        /* Flex Table - flex view */
        BCM_IF_ERROR_RETURN(
        soc_flow_db_mem_view_info_get(unit, mem_view_id, &vinfo));
        mem = vinfo.mem_view_id;
        BCM_IF_ERROR_RETURN(
          _bcm_esw_flow_stat_flex_mem_is_valid(unit,
                                          vinfo.mem,
                                          act_num_tables,
                                          &table[0],
                                          &valid));

        if (valid != TRUE) {
            LOG_ERROR(BSL_LS_BCM_FLOW,
                 (BSL_META_U(unit,
                  "Mismatch in stat_object_id %d resolved memory \
                  and resolved memory %d.\n"),
                  object, vinfo.mem));
            return BCM_E_PARAM;
        }

        /* Resolve the index from the key fields */
        BCM_IF_ERROR_RETURN(
        _bcm_esw_flow_stat_flex_view_index_get(unit, info,
                                         field, num_of_fields,
                                         vinfo, direction, 0, &index));
    } else if (BCM_FLOW_IS_FIXED_TABLE(info)) { /* == 0 */
        /* fixed tables SOURCE_VP, VFI, EGR_VFI
         * which are programmed as part of the flex flows
         */
        BCM_IF_ERROR_RETURN(
           _bcm_esw_flow_stat_fixed_table_index_get(unit,
                                             info,
                                             direction,
                                             &mem,
                                             &index));
        BCM_IF_ERROR_RETURN(
          _bcm_esw_flow_stat_flex_mem_is_valid(unit,
                                          mem,
                                          act_num_tables,
                                          &table[0],
                                          &valid));

        if (valid != TRUE) {
            LOG_ERROR(BSL_LS_BCM_FLOW,
                 (BSL_META_U(unit,
                  "Mismatch in stat_object_id %d resolved memory \
                  and resolved memory %d.\n"),
                  object, mem));
            return BCM_E_PARAM;
        }

    } else if (BCM_FLOW_IS_FIXED_VIEW(info)) {
        /* Flex table - fixed views */
        /* Classic flows */
        /* Resolve the index from the key fields*/

        BCM_IF_ERROR_RETURN(
            _bcm_esw_flow_stat_fixed_view_index_get(unit,
                                            info, num_of_fields,
                                            field, direction, 0, &mem,
                                             &index));
        BCM_IF_ERROR_RETURN(
          _bcm_esw_flow_stat_flex_mem_is_valid(unit,
                                          mem,
                                          act_num_tables,
                                          &table[0],
                                          &valid));

        if (valid != TRUE) {
            LOG_ERROR(BSL_LS_BCM_FLOW,
                 (BSL_META_U(unit,
                  "Mismatch in stat_object_id %d resolved memory \
                  and resolved memory %d.\n"),
                  object, mem));
            return BCM_E_PARAM;
        }

    } else {
        LOG_ERROR(BSL_LS_BCM_FLOW,
                    (BSL_META_U(unit,
                      "Invalid combination of flow, \
                      flow_option and function_type or  \
                      or not a supported memory\n")));
        return BCM_E_PARAM;
    }


    if (direction == bcmStatFlexDirectionIngress) {
        rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                    unit,
                    mem,
                    index);
        /* remember the first error code, but allow loop finish */
        if (BCM_E_NONE != rv &&
            BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
            err_code[bcmStatFlexDirectionIngress] = rv;
        }

    } else {
        rv = _bcm_esw_stat_flex_detach_egress_table_counters(
                 unit,
                 object,
                 mem,
                 index);
        if (BCM_E_NONE != rv &&
            BCM_E_NONE == err_code[bcmStatFlexDirectionEgress]) {
            err_code[bcmStatFlexDirectionEgress] = rv;
        }

    }

    /* In case of success on one direction, and return NOT_FOUND on the other,
     *  we still take it as a success */
    BCM_STAT_FLEX_DETACH_RETURN(err_code)
    return rv;

}

int bcmi_esw_flow_stat_id_get(int unit,
                         bcm_flow_stat_info_t *info,
                         uint32 num_of_fields,
                         bcm_flow_logical_field_t *field,
                         bcm_stat_object_t stat_object,
                         uint32 *stat_counter_id)
{
    int rv = BCM_E_NONE;
    uint32 mem_view_id = 0;
    soc_mem_t mem = 0;
    soc_flow_db_mem_view_info_t vinfo;
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    int index = -1;
    bcm_stat_flex_table_info_t table_info;
    uint32 num_stat_counter_ids = 0;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_flow_stat_info_validate(unit,
                            info));
    /* Validate object and direction */
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit, stat_object, &direction));
    if (soc_flow_db_mem_view_flow_handle_valid(unit, info->flow_handle)) {
        return BCM_E_PARAM;
    }

    rv = soc_flow_db_ffo_to_mem_view_id_get(unit,
                                info->flow_handle,
                                info->flow_option,
                                info->function_type,
                                &mem_view_id);

    if (BCM_SUCCESS(rv)) {
        /* Flex view of flows*/
        BCM_IF_ERROR_RETURN(
        soc_flow_db_mem_view_info_get(unit, mem_view_id, &vinfo));

        mem = vinfo.mem_view_id;
        /* Resolve the index from the key fields */
        BCM_IF_ERROR_RETURN(
        _bcm_esw_flow_stat_flex_view_index_get(unit, info,
                                         field, num_of_fields,
                                         vinfo, direction, 0, &index));

    } else if (BCM_FLOW_IS_FIXED_TABLE(info)) { /* == 0 */
        /* Fixed table of Flows - SOURCE_VP,VFI */
        BCM_IF_ERROR_RETURN(
           _bcm_esw_flow_stat_fixed_table_index_get(unit,
                                             info,
                                             direction,
                                             &mem,
                                             &index));

    } else if (BCM_FLOW_IS_FIXED_VIEW(info)) {
        /* Fixed Views of Flows */
        BCM_IF_ERROR_RETURN(
            _bcm_esw_flow_stat_fixed_view_index_get(unit,
                                             info, num_of_fields, field,
                                             direction, 0,
                                             &mem,
                                             &index));


   } else {
        LOG_ERROR(BSL_LS_BCM_FLOW,
                    (BSL_META_U(unit,
                      "Invalid combination of flow, \
                      flow_option and function_type or \
                      or not a supported memory\n")));
        return BCM_E_PARAM;
   }

   table_info.table = mem;
   table_info.index = index;
   table_info.direction = direction;

   return _bcm_esw_stat_flex_get_counter_id(unit, 1, &table_info,
                  &num_stat_counter_ids, stat_counter_id);

}

#endif  /* INCLUDE_L3 */

