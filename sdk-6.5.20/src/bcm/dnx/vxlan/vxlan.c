/** \file vxlan.c
 *  General vxlan functionality for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VXLAN
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/vxlan.h>
#include <bcm/vlan.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/vsi/vswitch_vpn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/lif_mngr/algo_out_lif_profile.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/l2/l2_learn.h>
#include <bcm_int/dnx/switch/switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include "vxlan.h"
/*
 * }
 */

/*
 * MACROs and ENUMs
 * {
 */

/*
 * Replace flags: Replace VSI (vpn), mc-groups
 */
#define _BCM_DNX_VXLAN_VPN_REPLACE_FLAGS (BCM_VXLAN_VPN_REPLACE               | \
                                      BCM_VXLAN_VPN_UNKNOWN_UCAST_REPLACE | \
                                      BCM_VXLAN_VPN_UNKNOWN_MCAST_REPLACE | \
                                      BCM_VXLAN_VPN_BCAST_REPLACE)

#define _BCM_DNX_VXLAN_CREATE_SUPPORTED_FLAGS (_BCM_DNX_VXLAN_VPN_REPLACE_FLAGS | \
                                                BCM_VXLAN_VPN_WITH_ID | \
                                                BCM_VXLAN_VPN_WITH_VPNID)
/*
 * VXLAN - Networking Standard
 */
#define _BCM_DNX_VXLAN_VNI_NOF_BITS (24)
#define _BCM_DNX_VXLAN_VNI_MAX      (SAL_UPTO_BIT(_BCM_DNX_VXLAN_VNI_NOF_BITS))

#define _BCM_VXLAN_PORT_HAS_OUTLIF(_vxlan_port) (UTILEX_NUM2BOOL((_vxlan_port)->egress_tunnel_id != 0))
#define _BCM_VXLAN_PORT_HAS_VALID_OUTLIF(_vxlan_port, _global_out_lif) \
             (_BCM_VXLAN_PORT_HAS_OUTLIF(_vxlan_port) && (_global_out_lif != UTILEX_INTERN_VAL_INVALID_32))
/* indicate that the destination is a FEC. */
#define _BCM_VXLAN_PORT_HAS_FEC(_vxlan_port) (UTILEX_NUM2BOOL(_vxlan_port->flags & BCM_VXLAN_PORT_EGRESS_TUNNEL))
/*  indicate if global out lif is symmetric with global in lif : global outlif = global inlif*/
#define _BCM_VXLAN_PORT_HAS_SYMMETRIC_IN_LIF_WITH_OUT_LIF(_vxlan_port, _tunnel_term_gport_hw_resources, _tunnel_init_gport_hw_resources) \
             (_BCM_VXLAN_PORT_HAS_OUTLIF(_vxlan_port) &&                                                                                 \
             ((_tunnel_term_gport_hw_resources)->global_in_lif == (_tunnel_init_gport_hw_resources)->global_out_lif))

/**
 Internal learn type
 */
typedef enum
{
    DNX_VXLAN_LEARN_TYPE_FEC_ASYM_OUTLIF,
    DNX_VXLAN_LEARN_TYPE_FEC_SYM_OUTLIF,
    DNX_VXLAN_LEARN_TYPE_PORT_SYM_OUTLIF,
    DNX_VXLAN_LEARN_TYPE_FEC_ONLY
} dnx_vxlan_learn_type_e;

/** 
 * Internal inlif table fields 
 */
typedef struct
{
    uint32 inlif_profile;
    uint32 fodo;
} dnx_vxlan_in_lif_table_fields_t;

/*
 * End of MACROs
 * }
 */

/*
 * Verify functions
 * {
 */

shr_error_e
dnx_vxlan_vpn_create_type_get(
    int unit,
    int vsi,
    uint32 *value)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI_TYPE_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VXLAN_VPN_MAPPING_CREATED_BY_API, INST_SINGLE, value);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * Used by bcm_dnx_vxlan_vpn_destroy to verify API input and vxlan_vpn structs.
 */
static shr_error_e
dnx_vxlan_vpn_destroy_verify(
    int unit,
    bcm_vpn_t l2vpn)
{
    uint8 is_allocated;
    int nof_vsis;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    /*
     * Check L2VPN range
     */
    if (l2vpn >= nof_vsis)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VPN is out of range \n");
    }

    /*
     * Check if L2VPN is allocated:
     */
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, l2vpn, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "L2VPN %d requested for destruction is not allocated. \n", l2vpn);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_vxlan_vpn_get to verify API input and vxlan vpn structs.
 */
static shr_error_e
dnx_vxlan_vpn_get_verify(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_vxlan_vpn_config_t * info)
{
    uint32 vxlan_vpn_create;
    uint8 is_allocated = 0;
    int nof_vsis;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    /*
     * Check L2VPN range
     */
    if (l2vpn >= nof_vsis)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VPN is out of range \n");
    }

    /*
     * Check flag support
     */
    if (info->flags)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "invalid flags 0x%x, the dnx_vxlan_vpn_get api does not support any flags  \r\n",
                     info->flags);
    }

    /*
     * check network domain range
     */
    if (info->match_port_class >= (dnx_data_l2.vlan_domain.nof_vlan_domains_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "match_port_class is higher than the maximum match_port_class %d \r\n",
                     (dnx_data_l2.vlan_domain.nof_vlan_domains_get(unit) - 1));
    }

    /*
     * check if L2VPN allocated
     */
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, l2vpn, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "L2VPN %d is not allocated \n", l2vpn);
    }

    SHR_IF_ERR_EXIT(dnx_vxlan_vpn_create_type_get(unit, l2vpn, &vxlan_vpn_create));
    if (vxlan_vpn_create != DBAL_ENUM_FVAL_VXLAN_VPN_MAPPING_CREATED_BY_API_VXLAN_VPN_CREATE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Trying to get mapping, which was created from vxlan_netwrok_domain_config_add API. \n");
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * Verify function used in
 */
static shr_error_e
dnx_vxlan_network_domain_config_traverse_verify(
    int unit,
    bcm_vxlan_network_domain_config_additional_info_t * additional_info,
    bcm_vxlan_network_domain_config_traverse_cb cb,
    void *user_data,
    bcm_vxlan_network_domain_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(additional_info, _SHR_E_PARAM, "additional_info");
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * Used by bcm_dnx_vxlan_vpn_create to verify API input and vxlan_vpn structs.
 */
static shr_error_e
dnx_vxlan_vpn_create_verify(
    int unit,
    bcm_vxlan_vpn_config_t * info)
{
    int with_id, update;
    uint32 vxlan_vpn_create;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    SHR_IF_ERR_EXIT(dnx_vxlan_vpn_create_type_get(unit, info->vpn, &vxlan_vpn_create));
    /*
     * Check flag support
     */
    if (info->flags & ~_BCM_DNX_VXLAN_CREATE_SUPPORTED_FLAGS)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "The used flag = 0x%08X is not in the range of supported ones", info->flags);
    }

    if (!(info->flags & BCM_VXLAN_VPN_WITH_VPNID))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_VXLAN_VPN_WITH_VPNID, must be present in vpn create \r\n");
    }

    if (info->match_port_class != 0)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "To create mapping of VNI to VSI with network domain different than 0, "
                     "use bcm_vxlan_network_domain_config_add, for more information, refer to the PG \r\n");
    }
    /*
     * Check with_id and replace
     */
    with_id = (info->flags & BCM_VXLAN_VPN_WITH_ID) ? TRUE : FALSE;
    update = (info->flags & _BCM_DNX_VXLAN_VPN_REPLACE_FLAGS) ? TRUE : FALSE;
    if (update && !with_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Replace flags (BCM_VXLAN_VPN_REPLACE, BCM_VXLAN_VPN_UNKNOWN_UCAST_REPLACE, BCM_VXLAN_VPN_UNKNOWN_MCAST_REPLACE "
                     "or BCM_VXLAN_VPN_BCAST_REPLACE) can't be used without BCM_VXLAN_VPN_WITH_ID flag \r\n");
    }

    /*
     * Check vni range
     */
    if (info->vnid > (_BCM_DNX_VXLAN_VNI_MAX) && (info->vnid != BCM_VXLAN_VNI_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "vnid is out of range \r\n");
    }

    /*
     * Make sure BCM_VXLAN_VNI_INVALID is only used on update when the mapping was created from the same API
     */
    if (update && (vxlan_vpn_create == DBAL_ENUM_FVAL_VXLAN_VPN_MAPPING_CREATED_BY_API_VXLAN_NETWORK_CONFIG))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Can not unmap vni2vsi, since the mapping was created from another API call");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_vxlan_network_domain_config_add to verify API input.
 */
static shr_error_e
dnx_vxlan_network_domain_config_add_verify(
    int unit,
    bcm_vxlan_network_domain_config_t * config)
{
    uint32 vxlan_vpn_create;
    uint8 is_vsi_used;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");
    /*
     * Check the flags provided
     */
    if ((config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY)
        && (config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid flags combination 0x%x, the mapping can not be ingress and egress on the same API call. \n",
                     config->flags);
    }
    if (!(config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY)
        && !(config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid flags combination 0x%x, the mapping should be at least ingress or egress. \n",
                     config->flags);
    }
    if ((config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING)
        && (config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flags combination 0x%x, the mapping can be only L2 or L3. \n",
                     config->flags);
    }
    if (!(config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING)
        && !(config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flags combination 0x%x, the mapping should be at least L2 or L3. \n",
                     config->flags);
    }
    if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING) && config->vrf)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "When the flag 0x%x is used, VRF shouldn't be used. \n", config->flags);
    }
    if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING) && config->vsi)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "When the flag 0x%x is used, VSI shouldn't be used. \n", config->flags);
    }
    if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING))
    {
        SHR_IF_ERR_EXIT(dnx_vxlan_vpn_create_type_get(unit, config->vsi, &vxlan_vpn_create));
        if (vxlan_vpn_create == DBAL_ENUM_FVAL_VXLAN_VPN_MAPPING_CREATED_BY_API_VXLAN_VPN_CREATE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Trying to add mapping, which was created from bcm_vxlan_vpn_create API. \n");
        }
        SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_per_type_get(unit, config->vsi, _bcmDnxVsiTypeVxlan, &is_vsi_used));
        if (!is_vsi_used)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "VSI should be created first with bcm_vxlan_vpn_create  \n");
        }
    }
    if (dnx_data_esem.feature.feature_get(unit, dnx_data_esem_feature_etps_properties_assignment))
    {
        if ((config->qos_map_id != DNX_QOS_INITIAL_MAP_ID) && !DNX_QOS_MAP_IS_REMARK(config->qos_map_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "qos_map_id[%x] is not remark!\n", config->qos_map_id);
        }
    }
    else
    {
        if (config->qos_map_id != DNX_QOS_INITIAL_MAP_ID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "qos_map_id is only valid on JR2.");
        }
    }
    /** qos map id is valid for L3 only */
    if ((config->qos_map_id != DNX_QOS_INITIAL_MAP_ID)
        && (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "qos_map_id is for L3 mapping only. qos_map_id for l2 mapping is configured by bcm_dnx_qos_port_map_set. \n");
    }
    if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY)
        && _SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING))
    {
        if (config->qos_map_id != DNX_QOS_INITIAL_MAP_ID)
        {
            if (!DNX_QOS_MAP_IS_EGRESS(config->qos_map_id) || !DNX_QOS_MAP_IS_REMARK(config->qos_map_id))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "qos_map_id is should be egress remark. \n");
            }
        }
    }
    /*
     * Check vni range
     */
    if (config->vni > (_BCM_DNX_VXLAN_VNI_MAX))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "vnid is out of range \r\n");
    }
    /*
     * check network domain range
     */
    if (config->network_domain >= (dnx_data_l2.vlan_domain.nof_vlan_domains_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "network_domain is higher than the maximum network_domain %d \r\n",
                     (dnx_data_l2.vlan_domain.nof_vlan_domains_get(unit) - 1));
    }
    if (config->vrf >= dnx_data_l3.vrf.nof_vrf_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VRF %d is out of range, should be lower then %d.", config->vrf,
                     dnx_data_l3.vrf.nof_vrf_get(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_vxlan_network_domain_config_get to verify API input.
 */
static shr_error_e
dnx_vxlan_network_domain_config_get_verify(
    int unit,
    bcm_vxlan_network_domain_config_t * config)
{
    uint32 vxlan_network_domain_add;
    uint8 is_vsi_used;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");
    SHR_IF_ERR_EXIT(dnx_vxlan_vpn_create_type_get(unit, config->vsi, &vxlan_network_domain_add));
    /*
     * Check the flags provided
     */
    if ((config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY)
        && (config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid flags combination 0x%x, can't get both ingress and egress mapping on same API call. \n",
                     config->flags);
    }
    if (!(config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY)
        && !(config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid flags combination 0x%x, need to provide at least ingress or egress flag. \n",
                     config->flags);
    }
    if ((config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING)
        && (config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid flags combination 0x%x, can't get both L2 and L3 mapping on same API call. \n",
                     config->flags);
    }
    if (!(config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING)
        && !(config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flags combination 0x%x, the mapping should be at least L2 or L3. \n",
                     config->flags);
    }
    if ((vxlan_network_domain_add == DBAL_ENUM_FVAL_VXLAN_VPN_MAPPING_CREATED_BY_API_VXLAN_VPN_CREATE) &&
        (config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING) &&
        (config->flags & BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY) && (config->network_domain == 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Trying to get mapping, which was created from vxlan_vpn_create API. \n");
    }
    /**Verify the input vsi is allocated only for egress mapping, as for ingress it is a return parameter*/
    if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING) &&
        _SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY))
    {
        SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_per_type_get(unit, config->vsi, _bcmDnxVsiTypeVxlan, &is_vsi_used));
        if (!is_vsi_used)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "trying to get mapping for VSI, which doesn't exist \n");
        }
    }
    /*
     * check network domain range
     */
    if (config->network_domain >= (dnx_data_l2.vlan_domain.nof_vlan_domains_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "network_domain is higher than the maximum network_domain %d \r\n",
                     (dnx_data_l2.vlan_domain.nof_vlan_domains_get(unit) - 1));
    }
    /*
     * Check vni range
     */
    if (config->vni > (_BCM_DNX_VXLAN_VNI_MAX))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "vnid is out of range \r\n");
    }
    if (config->vrf >= dnx_data_l3.vrf.nof_vrf_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VRF %d is out of range, should be lower then %d.", config->vrf,
                     dnx_data_l3.vrf.nof_vrf_get(unit));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * End of Verify functions
 * }
 */

/*
 * Inner functions
 * {
 */

shr_error_e
dnx_vxlan_vpn_create_type_set(
    int unit,
    int vsi,
    uint32 value)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI_TYPE_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_VXLAN_VPN_MAPPING_CREATED_BY_API, INST_SINGLE, value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update bcm_vxlan_vpn_config_t struct with MC groups according to flags.
 * Replacing the vpn will cause all MC groups to be replaced.
 * Otherwise we'll update specific MC groups according to flags.
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [out] vlan_control - new properties of the vlan are updated:
 *   * vlan_control->unknown_unicast_group
 *   * vlan_control->unknown_multicast_group
 *   * vlan_control->broadcast_group
 *   \param [in] info -
 *   A pointer to the struct that holds information for the
 *   vxlan vpn instance, see \see bcm_vxlan_vpn_config_t.
 */
static shr_error_e
dnx_vxlan_vpn_create_vlan_control_update(
    int unit,
    bcm_vlan_control_vlan_t * vlan_control,
    bcm_vxlan_vpn_config_t * info)
{
    uint32 replace_flags;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Update unknown unicast mc-group, unknown multicast mc-group and broadcast mc-group based on replace flags.
     */
    SHR_IF_ERR_EXIT(bcm_dnx_vlan_control_vlan_get(unit, info->vpn, vlan_control));

    /*
     * Replacing the vpn will cause all MC groups to be replaced.
     */
    if (info->flags & BCM_VXLAN_VPN_REPLACE)
    {
        replace_flags = (BCM_VXLAN_VPN_UNKNOWN_UCAST_REPLACE |
                         BCM_VXLAN_VPN_UNKNOWN_MCAST_REPLACE | BCM_VXLAN_VPN_BCAST_REPLACE);
    }
    /*
     * If vpn is not replaced, then replace specific mc-groups only
     */
    else
    {
        replace_flags = (info->flags & (BCM_VXLAN_VPN_UNKNOWN_UCAST_REPLACE |
                                        BCM_VXLAN_VPN_UNKNOWN_MCAST_REPLACE | BCM_VXLAN_VPN_BCAST_REPLACE));
    }

    if (replace_flags & BCM_VXLAN_VPN_UNKNOWN_UCAST_REPLACE)
    {
        vlan_control->unknown_unicast_group = info->unknown_unicast_group;
    }

    if (replace_flags & BCM_VXLAN_VPN_UNKNOWN_MCAST_REPLACE)
    {
        vlan_control->unknown_multicast_group = info->unknown_multicast_group;
    }

    if (replace_flags & BCM_VXLAN_VPN_BCAST_REPLACE)
    {
        vlan_control->broadcast_group = info->broadcast_group;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Map in ISEM: VNI, network domain to VSI
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] vni - Vxlan header field: VXLAN Network Identifier.
 *   VNI is used to identify the local vpn in the network.
 *   \param [in] network_domain - forwarding domain. Identify the network in which the VNI belongs to.
 *   \param [in] vsi - virtual switch instance.
 *   \param [in] update - indicates whether the DBAL action is update or commit
 *   \return
 *    \retval Zero in case of NO ERROR.
 *    \retval Negative in case of an error, see \see shr_error_e.
 *   \remark
 *   * Write an entry in ISEM table {VNI, network domain} to VSI
 */
static int
dnx_vxlan_vpn_vni_to_vsi_set(
    int unit,
    uint32 vni,
    uint32 network_domain,
    bcm_vpn_t vsi,
    uint8 update)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VNI2VSI, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VNI, vni);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, network_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, vsi);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_PD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FODO_PD_RESULT_TYPE_RESULT_FOUND);

    if (update)
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function returns VSI value for given VNI and network domain
 */
static int
dnx_vxlan_network_domain_map_vni_to_vsi_get(
    int unit,
    uint32 vni,
    uint32 network_domain,
    uint32 *vsi)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VNI2VSI, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VNI, vni);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, network_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, vsi);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function returns VRF value for given VNI and network domain
 */
static int
dnx_vxlan_network_domain_map_vni_to_vrf_get(
    int unit,
    uint32 vni,
    uint32 network_domain,
    uint32 *vrf)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VNI2VRF, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VNI, vni);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, network_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, vrf);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * Map in ESEM: VSI, network domain to VNI.
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] vsi - virtual switch instance.
 *   \param [in] network_domain - forwarding domain. Identify the network in which the VNI belongs to.
 *   \param [in] vni - Vxlan header field: VXLAN Network Identifier.
 *   VNI is used to identify the local vpn in the network.
 *   \param [in] result_type - result type
 *   \param [in] update - indicates we are updating an existing entry in the dbal
 *   \return
 *    \retval Zero in case of NO ERROR.
 *    \retval Negative in case of an error, see \see shr_error_e.
 *   \remark
 *   * Write an entry in ESEM table {VSI, network domain} to VNI
 */
static int
dnx_vxlan_vpn_vsi_to_vni_set(
    int unit,
    bcm_vpn_t vsi,
    uint32 network_domain,
    uint32 vni,
    uint32 result_type,
    uint8 update)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, network_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    /** C_VID field is used only for AC entries*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_DOMAIN_NWK_NAME, INST_SINGLE, vni);
    if (update)
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The functions maps VRF to network domain in ESEM
 */
static int
dnx_vxlan_vpn_vrf_to_vni_set(
    int unit,
    bcm_vpn_t vrf,
    uint32 network_domain,
    uint32 vni,
    int qos_map_id,
    uint8 update)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, network_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, vrf);
    /** C_VID field is used only for AC entries*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L3_FODO);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_DOMAIN_NWK_NAME, INST_SINGLE, vni);
    if (dnx_data_esem.feature.feature_get(unit, dnx_data_esem_feature_etps_properties_assignment))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE,
                                     DNX_QOS_MAP_PROFILE_GET(qos_map_id));
    }
    if (update)
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_vxlan_vpn_port_add to verify API input and vxlan port struct.
 */
static int
dnx_vxlan_port_add_verify(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_vxlan_port_t * vxlan_port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(vxlan_port, _SHR_E_PARAM, "vxlan_port");

    if (l2vpn != BCM_VXLAN_VPN_INVALID)
    {
        uint8 is_allocated = 0;
        SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, l2vpn, &is_allocated));
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "l2vpn %d is not allocated \n", l2vpn);
        }
    }

    if (vxlan_port->criteria != BCM_VXLAN_PORT_MATCH_VN_ID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "only vxlan ports can be created by this API \n");
    }

    if (vxlan_port->flags & BCM_VXLAN_PORT_WITH_ID)
    {
        if (!BCM_GPORT_IS_VXLAN_PORT(vxlan_port->vxlan_port_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "given id is not vxlan-port \n");
        }
    }

    /*
     * use egress_tunnel if valid, points egress objects for protection/ECMP
     */
    if ((vxlan_port->flags & BCM_VXLAN_PORT_EGRESS_TUNNEL))
    {
        if (!BCM_L3_ITF_TYPE_IS_FEC(vxlan_port->egress_if))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "egress_if is not valid  \n");
        }
    }

    /*
     * check given match tunnel
     */
    if (!BCM_GPORT_IS_TUNNEL(vxlan_port->match_tunnel_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "match_tunnel_id is not a tunnel \n");
    }

    /*
     * Verify ingress_network_group_id range:
     */
    SHR_IF_ERR_EXIT(dnx_switch_network_group_id_verify(unit, TRUE, vxlan_port->network_group_id));

    /*
     * Verify egress_network_group_id range:
     */
    if (_BCM_VXLAN_PORT_HAS_OUTLIF(vxlan_port))
    {
        SHR_IF_ERR_EXIT(dnx_switch_network_group_id_verify(unit, FALSE, vxlan_port->network_group_id));
    }

    /*
     * Split Horizon
     */
    if _SHR_IS_FLAG_SET
        (vxlan_port->flags, BCM_VXLAN_PORT_NETWORK)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_VXLAN_PORT_NETWORK is not supported\n");
    }

    

    

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get gport hw resources for tunnel termination inlif and tunnel initiator outlif from vxlan port struct
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] vxlan_port -
 *   A pointer to the struct that holds information for vxlan_port, \see bcm_vxlan_port_t.
 *   \param [out] tunnel_term_gport_hw_resources - Contains global/local in/out lif , dbal table and dbal result type.
 *   \param [out] tunnel_init_gport_hw_resources - Contains global/local in/out lif , dbal table and dbal result type.
 *   \return
 *    \retval Zero in case of NO ERROR.
 *    \retval Negative in case of an error, see \see shr_error_e.
 */
static int
dnx_vxlan_port_gport_hw_resources_get(
    int unit,
    bcm_vxlan_port_t * vxlan_port,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_term_gport_hw_resources,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_init_gport_hw_resources)
{
    uint32 lif_flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(vxlan_port, _SHR_E_PARAM, "vxlan_port");
    SHR_NULL_CHECK(tunnel_term_gport_hw_resources, _SHR_E_PARAM, "tunnel_term_gport_hw_resources");
    SHR_NULL_CHECK(tunnel_init_gport_hw_resources, _SHR_E_PARAM, "tunnel_init_gport_hw_resources");

    /*
     * get global and local in_lif
     */
    sal_memset(tunnel_term_gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));

    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, vxlan_port->match_tunnel_id, lif_flags, tunnel_term_gport_hw_resources));

    /*
     * Get global and local out_lif
     */
    if (_BCM_VXLAN_PORT_HAS_OUTLIF(vxlan_port))
    {
        sal_memset(tunnel_init_gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
        lif_flags =
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, vxlan_port->egress_tunnel_id, lif_flags, tunnel_init_gport_hw_resources));
    }

exit:
    SHR_FUNC_EXIT;
}

static void
dnx_vxlan_port_dbal_field_learn_info_get(
    dnx_vxlan_learn_type_e dnx_vxlan_learn_type,
    dbal_fields_e * dbal_field_learn_info)
{
    switch (dnx_vxlan_learn_type)
    {
        case DNX_VXLAN_LEARN_TYPE_FEC_ASYM_OUTLIF:
            *dbal_field_learn_info = DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT;
            break;
        case DNX_VXLAN_LEARN_TYPE_FEC_SYM_OUTLIF:
            
            *dbal_field_learn_info = DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT;
            break;
        case DNX_VXLAN_LEARN_TYPE_PORT_SYM_OUTLIF:
            
            *dbal_field_learn_info = DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT;
            break;
        case DNX_VXLAN_LEARN_TYPE_FEC_ONLY:
        default:
            
            *dbal_field_learn_info = DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT;
            break;
    }
}
/**
 * \brief
 * build and set learn info in tunnel term inlif
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] entry_handle_id - entry handle id
 *   \param [in] dnx_vxlan_learn_type - indicate how to build the learn info
 *   \param [in] forward_info - Contains learning information
 *   \return
 *    \retval Zero in case of NO ERROR.
 *    \retval Negative in case of an error, see \see shr_error_e.
 */
static shr_error_e
dnx_vxlan_port_in_lif_table_learn_info_set(
    int unit,
    uint32 entry_handle_id,
    dnx_vxlan_learn_type_e dnx_vxlan_learn_type,
    dnx_algo_gpm_forward_info_t * forward_info)
{
    uint32 learn_info[2];
    dbal_fields_e dbal_field_learn_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(learn_info, 0x0, sizeof(learn_info));

    switch (dnx_vxlan_learn_type)
    {
        case DNX_VXLAN_LEARN_TYPE_FEC_ASYM_OUTLIF:
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, DBAL_FIELD_DESTINATION,
                             &forward_info->destination, learn_info));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, DBAL_FIELD_GLOB_OUT_LIF,
                             &forward_info->outlif, learn_info));
            break;
        case DNX_VXLAN_LEARN_TYPE_FEC_SYM_OUTLIF:
            
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, DBAL_FIELD_DESTINATION,
                             &forward_info->destination, learn_info));
            break;
        case DNX_VXLAN_LEARN_TYPE_PORT_SYM_OUTLIF:
            
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, DBAL_FIELD_DESTINATION,
                             &forward_info->destination, learn_info));
            break;
        case DNX_VXLAN_LEARN_TYPE_FEC_ONLY:
        default:
            
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, DBAL_FIELD_DESTINATION,
                             &forward_info->destination, learn_info));
            break;
    }
    dnx_vxlan_port_dbal_field_learn_info_get(dnx_vxlan_learn_type, &dbal_field_learn_info);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, dbal_field_learn_info, INST_SINGLE, learn_info);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * set learn payload context in tunnel term inlif
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] entry_handle_id - dbal handle id
 *   \param [in] dnx_vxlan_learn_type - indicate how to choose the learn payload context
 */
static void
dnx_vxlan_port_in_lif_table_learn_payload_context_set(
    int unit,
    uint32 entry_handle_id,
    dnx_vxlan_learn_type_e dnx_vxlan_learn_type)
{
    uint32 learn_payload_context;
    switch (dnx_vxlan_learn_type)
    {
        case DNX_VXLAN_LEARN_TYPE_FEC_ASYM_OUTLIF:
            learn_payload_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_ASYMMETRIC_LIF_BASIC;
            break;
        case DNX_VXLAN_LEARN_TYPE_FEC_SYM_OUTLIF:
        case DNX_VXLAN_LEARN_TYPE_PORT_SYM_OUTLIF:
            learn_payload_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_EXTENDED;
            break;
        case DNX_VXLAN_LEARN_TYPE_FEC_ONLY:
        default:
            learn_payload_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_EXTENDED;
            break;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT, INST_SINGLE,
                                 learn_payload_context);
}

static shr_error_e
dnx_vxlan_port_in_lif_table_default_vpn_set(
    int unit,
    uint32 entry_handle_id,
    bcm_vpn_t default_vpn)
{
    uint32 fodo = 0;
    uint32 default_vpn32 = (uint32) default_vpn;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set(unit, DBAL_FIELD_FODO, DBAL_FIELD_VSI, &default_vpn32, &fodo));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO, INST_SINGLE, fodo);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vxlan_port_in_lif_table_default_vpn_get(
    int unit,
    uint32 fodo,
    bcm_vpn_t * default_vpn)
{
    uint32 vsi = 0;
    dbal_fields_e sub_field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get(unit, DBAL_FIELD_FODO, fodo, &sub_field_id, &vsi));

    if (sub_field_id != DBAL_FIELD_VSI)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "expected FODO to be a VSI \n");
    }

    *default_vpn = (bcm_vpn_t) vsi;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Update tunnel termination lif with forwarding domain
 * assignment mode, in lif profile and learn information
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] tunnel_term_gport_hw_resources - Contains global/local in/out lif , dbal table and dbal result type.
 *   \param [in] in_lif_profile - Assign the in lif profile to the IN_LIF table.
 *   \param [in] dnx_vxlan_learn_type - indicate how to build the learn info
 *   \param [in] default_vpn - default vpn to use when VNI x network domain lookup missed
 *   \param [in] forward_info - Contains learning information
 *   \return
 *    \retval Zero in case of NO ERROR.
 *    \retval Negative in case of an error, see \see shr_error_e.
 */
static shr_error_e
dnx_vxlan_port_ingress_lif_update(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_term_gport_hw_resources,
    int in_lif_profile,
    dnx_vxlan_learn_type_e dnx_vxlan_learn_type,
    bcm_vpn_t default_vpn,
    dnx_algo_gpm_forward_info_t * forward_info)
{
    uint32 entry_handle_id;
    dbal_tables_e tunnel_term_dbal_table = tunnel_term_gport_hw_resources->inlif_dbal_table_id;
    lif_table_mngr_inlif_info_t lif_table_mngr_inlif_info;
    int local_in_lif = tunnel_term_gport_hw_resources->local_in_lif;

    uint32 learn_enable = 1;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&lif_table_mngr_inlif_info, 0, sizeof(lif_table_mngr_inlif_info_t));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, tunnel_term_dbal_table, &entry_handle_id));

    /** Set keys and gets all fields */
    {
        lif_table_mngr_inlif_info_t temp_lif_info;
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                        (unit, _SHR_CORE_ALL, local_in_lif, entry_handle_id, &temp_lif_info));
    }

    lif_table_mngr_inlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    lif_table_mngr_inlif_info.global_lif = tunnel_term_gport_hw_resources->global_in_lif;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, in_lif_profile);
    SHR_IF_ERR_EXIT(dnx_vxlan_port_in_lif_table_default_vpn_set(unit, entry_handle_id, default_vpn));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, INST_SINGLE, learn_enable);
    SHR_IF_ERR_EXIT(dnx_vxlan_port_in_lif_table_learn_info_set
                    (unit, entry_handle_id, dnx_vxlan_learn_type, forward_info));
    dnx_vxlan_port_in_lif_table_learn_payload_context_set(unit, entry_handle_id, dnx_vxlan_learn_type);
    /*
     * Write to HW
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
                    (unit, _SHR_CORE_ALL, entry_handle_id, &local_in_lif, &lif_table_mngr_inlif_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get inlif table fields 
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] tunnel_term_gport_hw_resources - Contains global/local in/out lif , dbal table and dbal result type.
 *   \param [out] in_lif_table_fields - in lif table fields 
 *   \return
 *    \retval Zero in case of NO ERROR.
 *    \retval Negative in case of an error, see \see shr_error_e.
 */
static shr_error_e
dnx_vxlan_port_ingress_inlif_table_fields_get(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_term_gport_hw_resources,
    dnx_vxlan_in_lif_table_fields_t * in_lif_table_fields)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id = tunnel_term_gport_hw_resources->inlif_dbal_table_id;
    int local_in_lif = tunnel_term_gport_hw_resources->local_in_lif;
    uint32 dbal_result_type = tunnel_term_gport_hw_resources->inlif_dbal_result_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * Set KEY field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                     &(in_lif_table_fields->inlif_profile)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FODO, INST_SINGLE, &(in_lif_table_fields->fodo)));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *Update ip tunnel eedb entry with outlif profile
 */
static int
dnx_vxlan_port_egress_lif_update(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_init_gport_hw_resources,
    int out_lif_profile)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    uint32 entry_handle_id;

    dbal_tables_e dbal_table_id = tunnel_init_gport_hw_resources->outlif_dbal_table_id;
    int local_out_lif = tunnel_init_gport_hw_resources->local_out_lif;
    uint32 dbal_result_type = tunnel_init_gport_hw_resources->outlif_dbal_result_type;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, dbal_result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, out_lif_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief                                                                        .
 * Save gport forward information, so we can retrieve outlif/fec from gport.
 * Used by bcm_l2_addr_add for example.
 */
static int
dnx_vxlan_port_gport_to_forward_information_set(
    int unit,
    bcm_vxlan_port_t * vxlan_port,
    int global_outlif,
    dnx_algo_gpm_forward_info_t * forward_info)
{

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(forward_info, 0, sizeof(dnx_algo_gpm_forward_info_t));

    /*
     * Fill destination (from Gport) info Forward Info table (SW state)
     */
    if (_BCM_VXLAN_PORT_HAS_FEC(vxlan_port))
    {
        bcm_gport_t gport_forward_port;
        /*
         * set destination with FEC
         */
        BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(gport_forward_port, vxlan_port->egress_if);
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, gport_forward_port, &forward_info->destination));

        /*
         * outlif + FEC
         */
        if (_BCM_VXLAN_PORT_HAS_VALID_OUTLIF(vxlan_port, global_outlif))
        {
            forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF;
            forward_info->outlif = global_outlif;
        }
        /*
         * FEC only
         */
        else
        {
            /*
             * Jericho mode beginning
             * {
             */
            if (dnx_data_headers.system_headers.system_headers_mode_get(unit)
                == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
            {
                forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF;
            }
            /*
             * Jericho mode end
             * }
             */
            else
            {
                forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY;
            }
        }
    }
    /*
     * outlif + port
     */
    else
    {
        forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF;
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, vxlan_port->match_port,
                         &forward_info->destination));
        forward_info->outlif = global_outlif;
    }
    SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_add(unit,
                                                       _SHR_IS_FLAG_SET(vxlan_port->flags, BCM_VXLAN_PORT_REPLACE),
                                                       vxlan_port->vxlan_port_id, forward_info));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief                                                                        .
 * Get gport forward information, so we can retrieve outlif/fec from gport.
 */
static shr_error_e
dnx_vxlan_port_gport_to_forward_information_get(
    int unit,
    bcm_vxlan_port_t * vxlan_port)
{
    dnx_algo_gpm_forward_info_t forward_info;
    bcm_gport_t destination;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&forward_info, 0, sizeof(dnx_algo_gpm_forward_info_t));

    SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_get(unit, vxlan_port->vxlan_port_id, &forward_info));
    if (forward_info.fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
    {
        /*
         * Jericho mode beginning
         * {
         */
        /*
         * In JR mode, Out-Lif may be 0 even fwd-info is dest+out_lif
         */
        if (dnx_data_headers.system_headers.system_headers_mode_get(unit)
            == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
        {
            if (forward_info.outlif != 0)
            {
                BCM_GPORT_TUNNEL_ID_SET(vxlan_port->egress_tunnel_id, forward_info.outlif);
            }
        }
        /*
         * Jericho mode end
         * }
         */
        else
        {
            BCM_GPORT_TUNNEL_ID_SET(vxlan_port->egress_tunnel_id, forward_info.outlif);
        }
    }

    SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, forward_info.destination, &destination));
    if ((forward_info.fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY) ||
        (forward_info.fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF))
    {
        /** destination is FEC */
        if (_SHR_GPORT_IS_FORWARD_PORT(destination))
        {
            vxlan_port->flags |= BCM_VXLAN_PORT_EGRESS_TUNNEL;
            BCM_GPORT_FORWARD_GROUP_TO_L3_ITF_FEC(vxlan_port->egress_if, destination);
        }
        /** destination is port */
        else
        {
            vxlan_port->match_port = destination;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** \brief
 * The function allocated vxlan egress outlif profile
 */
static shr_error_e
dnx_vxlan_port_egress_outlif_profile_exchange_internal(
    int unit,
    bcm_switch_network_group_t network_group_id,
    dbal_tables_e dbal_table_id,
    int out_lif_profile,
    int *new_out_lif_profile)
{
    etpp_out_lif_profile_info_t out_lif_profile_info;
    uint8 is_last;
    uint32 flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get out_lif_porfile data:
     */
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_get_data(unit, out_lif_profile, &out_lif_profile_info));

    /*
     * Update out_lif_porfile with the new outgoing_orientation value:
     */
    out_lif_profile_info.out_lif_orientation = network_group_id;

    /*
     * Exchange out_lif_porfile:
     */
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange
                    (unit, dbal_table_id, flags, &out_lif_profile_info, out_lif_profile, new_out_lif_profile,
                     &is_last));

exit:
    SHR_FUNC_EXIT;
}

/** \brief
 * The function allocated vxlan ingress inlif profile
 */
static shr_error_e
dnx_vxlan_port_ingress_inlif_profile_exchange_internal(
    int unit,
    bcm_switch_network_group_t network_group_id,
    dbal_tables_e inlif_dbal_table_id,
    int old_in_lif_profile,
    int *new_in_lif_profile)
{
    in_lif_profile_info_t in_lif_profile_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get in_lif_profile data:
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, old_in_lif_profile, &in_lif_profile_info, LIF));

    /*
     * Update in_lif_porfile with the new incoming_orientation value:
     */
    in_lif_profile_info.egress_fields.in_lif_orientation = network_group_id;

    /*
     * Exchange in_lif_porfile:
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, old_in_lif_profile, new_in_lif_profile, LIF, inlif_dbal_table_id));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *    Main function for updating the inlif profile for vxlan port.
 *    The function first gets the old inlif profiles, exchanges it with the new one and
 *    saves the information in the HW tables.
 */
static shr_error_e
dnx_vxlan_port_ingress_inlif_profile_exchange(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_term_gport_hw_resources,
    bcm_switch_network_group_t network_group_id,
    int *new_in_lif_profile)
{
    uint32 old_in_lif_profile = 0;
    dnx_vxlan_in_lif_table_fields_t in_lif_table_fields;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_vxlan_port_ingress_inlif_table_fields_get
                    (unit, tunnel_term_gport_hw_resources, &in_lif_table_fields));
    old_in_lif_profile = in_lif_table_fields.inlif_profile;
    SHR_IF_ERR_EXIT(dnx_vxlan_port_ingress_inlif_profile_exchange_internal
                    (unit, network_group_id, tunnel_term_gport_hw_resources->inlif_dbal_table_id, old_in_lif_profile,
                     new_in_lif_profile));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vxlan_port_egress_outlif_profile_get(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_init_gport_hw_resources,
    uint32 *outlif_profile)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id = tunnel_init_gport_hw_resources->outlif_dbal_table_id;
    int local_out_lif = tunnel_init_gport_hw_resources->local_out_lif;
    uint32 dbal_result_type = tunnel_init_gport_hw_resources->outlif_dbal_result_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the old outlif profile stored in the tunnel outlif table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, outlif_profile));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Main function for updating the outlif profile for vxlan port.
 *    The function first gets the old outlif profile, exchanges them with the new one and
 *    saves the information in the HW tables.
 */
static shr_error_e
dnx_vxlan_port_egress_outlif_profile_exchange(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_init_gport_hw_resources,
    bcm_switch_network_group_t network_group_id,
    int *new_out_lif_profile)
{
    uint32 old_out_lif_profile = DNX_ALGO_ETPP_OUT_LIF_DEFAULT_PROFILE;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_vxlan_port_egress_outlif_profile_get
                    (unit, tunnel_init_gport_hw_resources, &old_out_lif_profile));
    SHR_IF_ERR_EXIT(dnx_vxlan_port_egress_outlif_profile_exchange_internal
                    (unit, network_group_id, tunnel_init_gport_hw_resources->outlif_dbal_table_id, old_out_lif_profile,
                     new_out_lif_profile));
exit:
    SHR_FUNC_EXIT;
}

/**
 * Get internal learn type.
 *   \param [in] unit - unit
 *   \param [in] vxlan_port - vxlan port struct
 *   \param [in] tunnel_term_gport_hw_resources - tunnel term gport resources
 *   \param [in] tunnel_init_gport_hw_resources - tnunel init gport resources
 *   \param [in] dnx_vxlan_learn_type - internal vxlan learn type
 */
static void
dnx_vxlan_learn_type_get(
    int unit,
    bcm_vxlan_port_t * vxlan_port,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_term_gport_hw_resources,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_init_gport_hw_resources,
    dnx_vxlan_learn_type_e * dnx_vxlan_learn_type)
{
    uint8 is_fec = _BCM_VXLAN_PORT_HAS_FEC(vxlan_port);
    uint8 in_lif_symmetric_with_out_lif = FALSE;
    int global_out_lif = tunnel_init_gport_hw_resources->global_out_lif;

    in_lif_symmetric_with_out_lif =
        _BCM_VXLAN_PORT_HAS_SYMMETRIC_IN_LIF_WITH_OUT_LIF(vxlan_port, tunnel_term_gport_hw_resources,
                                                          tunnel_init_gport_hw_resources);

    /*
     * learn outlif + dest. if dest is not FEC, inlif and outlif are symmetric otherwise asymmetric lif and FEC.
     */
    if (_BCM_VXLAN_PORT_HAS_VALID_OUTLIF(vxlan_port, global_out_lif))
    {
        /*
         * learn dest:FEC + outlif
         */
        if (is_fec)
        {
            if (in_lif_symmetric_with_out_lif)
            {
                *dnx_vxlan_learn_type = DNX_VXLAN_LEARN_TYPE_FEC_SYM_OUTLIF;
            }
            else
            {
                *dnx_vxlan_learn_type = DNX_VXLAN_LEARN_TYPE_FEC_ASYM_OUTLIF;
            }
        }
        else
        {
            *dnx_vxlan_learn_type = DNX_VXLAN_LEARN_TYPE_PORT_SYM_OUTLIF;
        }
    }
    /*
     * learn destination only
     */
    else
    {
        /*
         * Jericho mode beginning
         * {
         */
        if (dnx_data_headers.system_headers.system_headers_mode_get(unit)
            == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
        {
            *dnx_vxlan_learn_type = DNX_VXLAN_LEARN_TYPE_FEC_ASYM_OUTLIF;
        }
        /*
         * Jericho mode end
         * }
         */
        else
        {
            *dnx_vxlan_learn_type = DNX_VXLAN_LEARN_TYPE_FEC_ONLY;
        }
    }
}

/**
 * \brief
 * Update tunnel termination inlif with:
 * - fodo assignment mode,
 * - learn information
 * update tunnel encapsulation of type outlif with:
 * - network domain,
 * update both tunnel termination inlif and tunnel encapsulation
 * outlif with:
 * - forwarding group
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] l2vpn -
 *
 *   \param [in,out] vxlan_port -
 *   A pointer to the struct that holds information for vxlan_port, \see bcm_vxlan_port_t.
 *   Fields description:
 *   * vxlan_port->flags
 *        BCM_VXLAN_PORT_WITH_ID: Indicate to use allocated vxlan_port_id.
 *        BCM_VXLAN_PORT_REPLACE: Indicate whether to modify previous setting of allocated vxlan_port_id.
 *        BCM_VXLAN_PORT_EGRESS_TUNNEL: Indicates simple IP-Out-LIF, and that the egress_tunnel_id tunnel is valid. For
 *           Primary/Secondary pair and ECMP, the IP  tunnels are defined in egress_if and egress_tunnel_id is invalid.
 *   * vxlan_port->vxlan_port_id: the returned allocated ID of the port, which must be equal to the In-LIF
 *     match_tunnel_id value.
 *   * vxlan_port->critieria:
 *   * vxlan_port->match_tunnel_id: The tunnel_id of IN-lif IP tunnel of type bcmTunnelTypeVxlan
 *   * vxlan_port->egress_tunnel_id: tunnel_id of out-lif IP tunnel of type bcmTunnelTypeVxlan.
 *   * vxlan_port->egress_if: FEC/ECMP pointing to ip tunnel encpsulation of type VXLAN.
 *   * vxlan_port->network_group_id: the group ID for orientation filtering.
 *   \return
 *    \retval Zero in case of NO ERROR.
 *    \retval Negative in case of an error, see \see shr_error_e.
 */
int
bcm_dnx_vxlan_port_add(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_vxlan_port_t * vxlan_port)
{

    dnx_algo_gpm_gport_hw_resources_t tunnel_term_gport_hw_resources;
    dnx_algo_gpm_gport_hw_resources_t tunnel_init_gport_hw_resources;
    int global_in_lif = UTILEX_INTERN_VAL_INVALID_32;
    int global_out_lif = UTILEX_INTERN_VAL_INVALID_32;
    uint8 is_fec = _BCM_VXLAN_PORT_HAS_FEC(vxlan_port);
    uint8 in_lif_symmetric_with_out_lif = FALSE;
    uint8 is_out_lif = (_BCM_VXLAN_PORT_HAS_OUTLIF(vxlan_port));
    int in_lif_profile = 0;
    int out_lif_profile = 0;
    dnx_vxlan_learn_type_e dnx_vxlan_learn_type;

    dnx_algo_gpm_forward_info_t forward_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vxlan_port_add_verify(unit, l2vpn, vxlan_port));

    SHR_IF_ERR_EXIT(dnx_vxlan_port_gport_hw_resources_get(unit, vxlan_port, &tunnel_term_gport_hw_resources,
                                                          &tunnel_init_gport_hw_resources));

    global_in_lif = tunnel_term_gport_hw_resources.global_in_lif;
    in_lif_symmetric_with_out_lif =
        _BCM_VXLAN_PORT_HAS_SYMMETRIC_IN_LIF_WITH_OUT_LIF(vxlan_port, &tunnel_term_gport_hw_resources,
                                                          &tunnel_init_gport_hw_resources);

    /*
     * indicate outlif, and therefore that tunnel_init_gport_hw_resources is filled
     */

    if (is_out_lif)
    {
        global_out_lif = tunnel_init_gport_hw_resources.global_out_lif;
    }

    /** Update the tunnel inlif orientation based on the inputs*/
    SHR_IF_ERR_EXIT(dnx_vxlan_port_ingress_inlif_profile_exchange
                    (unit, &tunnel_term_gport_hw_resources, vxlan_port->network_group_id, &in_lif_profile));
    if (is_out_lif)
    {
        /** Update the tunnel outlif orientation based on the inputs*/
        SHR_IF_ERR_EXIT(dnx_vxlan_port_egress_outlif_profile_exchange
                        (unit, &tunnel_init_gport_hw_resources, vxlan_port->network_group_id, &out_lif_profile));
    }
    /**
     *  If with_id, check that the inlif encoded in the vxlan gport
     *  is the inlif we extract from vxlan_port->match_tunnel_id
     */
    if (vxlan_port->flags & BCM_VXLAN_PORT_WITH_ID)
    {
        int global_in_lif_from_with_id = BCM_GPORT_VXLAN_PORT_ID_GET(vxlan_port->vxlan_port_id);
        if (global_in_lif != global_in_lif_from_with_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "given vxlan port gport is not encoded with the tunnel in-lif configured in match_tunnel_id \n");
        }
    }
    else
    {
        BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port->vxlan_port_id, global_in_lif);
    }

    /*
     * If out_lif used and no protection (!is_fec) then it has to be equal to in_lif
     */
    if (is_out_lif && !is_fec && !in_lif_symmetric_with_out_lif)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "match_tunnel 0x%x and egress_tunnel 0x%x have to be equal \n", global_in_lif,
                     global_out_lif);
    }

    /*
     * If no FEC, expect to have an out_lif
     */
    if (!is_out_lif && !is_fec)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Expect to have either egress_tunnel_id or egress_if configured \n");
    }

    /*
     * set vxlan port id
     */
    if (!(vxlan_port->flags & BCM_VXLAN_PORT_WITH_ID))
    {
        BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port->vxlan_port_id, global_in_lif);
    }

    /**
     * Save gport forward information, so we can retrieve outlif/fec from gport.
     * Used by bcm_l2_addr_add for example.
     */
    SHR_IF_ERR_EXIT(dnx_vxlan_port_gport_to_forward_information_set(unit, vxlan_port, global_out_lif, &forward_info));

    dnx_vxlan_learn_type_get(unit, vxlan_port, &tunnel_term_gport_hw_resources, &tunnel_init_gport_hw_resources,
                             &dnx_vxlan_learn_type);

    /**
     * Update ingress lif:
     * update fodo-assignment mode, lif profile and learn
     * information
 */
    SHR_IF_ERR_EXIT(dnx_vxlan_port_ingress_lif_update
                    (unit, &tunnel_term_gport_hw_resources, in_lif_profile, dnx_vxlan_learn_type,
                     vxlan_port->default_vpn, &forward_info));

    if (is_out_lif)
    {
        SHR_IF_ERR_EXIT(dnx_vxlan_port_egress_lif_update(unit, &tunnel_init_gport_hw_resources, out_lif_profile));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get both tunnel termination and tunnel encapsulation information
 * outlif with:
 * - forwarding group
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] l2vpn -
 *     l2vpn
 *   \param [in,out] vxlan_port -
 *   A pointer to the struct that holds information for vxlan_port, \see bcm_vxlan_port_t.
 *   Fields description:
 *   * vxlan_port->flags
 *        BCM_VXLAN_PORT_EGRESS_TUNNEL: Indicates simple IP-Out-LIF, and that the egress_tunnel_id tunnel is valid. For
 *           Primary/Secondary pair and ECMP, the IP  tunnels are defined in egress_if and egress_tunnel_id is invalid.
 *   * vxlan_port->vxlan_port_id: the input ID of the port, which must be equal to the In-LIF match_tunnel_id value.
 *   * vxlan_port->critieria: BCM_VXLAN_PORT_MATCH_VN_ID
 *   * vxlan_port->match_tunnel_id: The tunnel_id of IN-lif IP tunnel of type bcmTunnelTypeVxlan
 *   * vxlan_port->egress_tunnel_id: tunnel_id of out-lif IP tunnel of type bcmTunnelTypeVxlan.
 *   * vxlan_port->egress_if: FEC/ECMP pointing to ip tunnel encpsulation of type VXLAN.
 *   * vxlan_port->network_group_id: the group ID for orientation filtering.
 *   \return
 *    \retval Zero in case of NO ERROR.
 *    \retval Negative in case of an error, see \see shr_error_e.
 */
int
bcm_dnx_vxlan_port_get(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_vxlan_port_t * vxlan_port)
{
    dnx_algo_gpm_gport_hw_resources_t tunnel_term_gport_hw_resources;
    dnx_algo_gpm_gport_hw_resources_t tunnel_init_gport_hw_resources;
    in_lif_profile_info_t in_lif_profile_info;
    dnx_vxlan_in_lif_table_fields_t in_lif_table_fields;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(vxlan_port, _SHR_E_PARAM, "vxlan_port");

    /*
     * const values
     */
    vxlan_port->criteria = BCM_VXLAN_PORT_MATCH_VN_ID;

    BCM_GPORT_TUNNEL_ID_SET(vxlan_port->match_tunnel_id, vxlan_port->vxlan_port_id);
    /**
     * Get gport forward information, so we can retrieve outlif/fec from gport.
     */
    SHR_IF_ERR_EXIT(dnx_vxlan_port_gport_to_forward_information_get(unit, vxlan_port));

    dnx_vxlan_port_gport_hw_resources_get(unit, vxlan_port, &tunnel_term_gport_hw_resources,
                                          &tunnel_init_gport_hw_resources);

    SHR_IF_ERR_EXIT(dnx_vxlan_port_ingress_inlif_table_fields_get
                    (unit, &tunnel_term_gport_hw_resources, &in_lif_table_fields));
    SHR_IF_ERR_EXIT(dnx_vxlan_port_in_lif_table_default_vpn_get
                    (unit, in_lif_table_fields.fodo, &(vxlan_port->default_vpn)));
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_table_fields.inlif_profile, &in_lif_profile_info, LIF));
    vxlan_port->network_group_id = in_lif_profile_info.egress_fields.in_lif_orientation;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vxlan_port_egress_update_outlif_profile_in_outlif(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_init_gport_hw_resources,
    int out_lif_profile)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id = tunnel_init_gport_hw_resources->outlif_dbal_table_id;
    int local_out_lif = tunnel_init_gport_hw_resources->local_out_lif;
    uint32 result_type = tunnel_init_gport_hw_resources->outlif_dbal_result_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, out_lif_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vxlan_port_egress_orientation_clear(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_init_gport_hw_resources)
{
    int orientation_clear_value = 0;
    int out_lif_profile;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_vxlan_port_egress_outlif_profile_exchange
                    (unit, tunnel_init_gport_hw_resources, orientation_clear_value, &out_lif_profile));
    SHR_IF_ERR_EXIT(dnx_vxlan_port_egress_update_outlif_profile_in_outlif
                    (unit, tunnel_init_gport_hw_resources, out_lif_profile));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vxlan_port_ingress_lif_clear(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_term_gport_hw_resources,
    int in_lif_profile,
    dnx_vxlan_learn_type_e dnx_vxlan_learn_type)
{
    uint32 entry_handle_id;
    dbal_tables_e tunnel_term_dbal_table = tunnel_term_gport_hw_resources->inlif_dbal_table_id;
    lif_table_mngr_inlif_info_t lif_table_mngr_inlif_info;
    dbal_fields_e dbal_field_learn_info;
    int local_in_lif = tunnel_term_gport_hw_resources->local_in_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&lif_table_mngr_inlif_info, 0, sizeof(lif_table_mngr_inlif_info_t));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, tunnel_term_dbal_table, &entry_handle_id));

    /** Set keys and gets all fields */
    {
        lif_table_mngr_inlif_info_t temp_lif_info;
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                        (unit, _SHR_CORE_ALL, local_in_lif, entry_handle_id, &temp_lif_info));
    }
    lif_table_mngr_inlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    lif_table_mngr_inlif_info.global_lif = tunnel_term_gport_hw_resources->global_in_lif;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, in_lif_profile);
    dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_FODO, INST_SINGLE);
    dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, INST_SINGLE);
    dnx_vxlan_port_dbal_field_learn_info_get(dnx_vxlan_learn_type, &dbal_field_learn_info);
    dbal_entry_value_field_unset(unit, entry_handle_id, dbal_field_learn_info, INST_SINGLE);
    dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT, INST_SINGLE);
    /*
     * Write to HW
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
                    (unit, _SHR_CORE_ALL, entry_handle_id, &local_in_lif, &lif_table_mngr_inlif_info));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_vxlan_port_delete_ingress(
    int unit,
    bcm_vxlan_port_t * vxlan_port,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_term_gport_hw_resources,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_init_gport_hw_resources)
{
    dnx_vxlan_learn_type_e dnx_vxlan_learn_type;
    int orientation_clear_value = 0;
    int in_lif_profile;

    SHR_FUNC_INIT_VARS(unit);

    dnx_vxlan_learn_type_get(unit, vxlan_port, tunnel_term_gport_hw_resources, tunnel_init_gport_hw_resources,
                             &dnx_vxlan_learn_type);

    SHR_IF_ERR_EXIT(dnx_vxlan_port_ingress_inlif_profile_exchange
                    (unit, tunnel_term_gport_hw_resources, orientation_clear_value, &in_lif_profile));

    SHR_IF_ERR_EXIT(dnx_vxlan_port_ingress_lif_clear
                    (unit, tunnel_term_gport_hw_resources, in_lif_profile, dnx_vxlan_learn_type));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vxlan_port_delete_egress(
    int unit,
    bcm_vxlan_port_t * vxlan_port,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_init_gport_hw_resources)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_BCM_VXLAN_PORT_HAS_OUTLIF(vxlan_port))
    {
        SHR_IF_ERR_EXIT(dnx_vxlan_port_egress_orientation_clear(unit, tunnel_init_gport_hw_resources));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete both tunnel termination and tunnel encapsulation information
 * outlif with:
 * - forwarding group
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] l2vpn -
 *    l2vpn
 *   \param [in] vxlan_port_id -
 *    vxlan port id.
 *   \return
 *    \retval Zero in case of NO ERROR.
 *    \retval Negative in case of an error, see \see shr_error_e.
 */
int
bcm_dnx_vxlan_port_delete(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_gport_t vxlan_port_id)
{
    bcm_vxlan_port_t vxlan_port;
    dnx_algo_gpm_gport_hw_resources_t tunnel_term_gport_hw_resources;
    dnx_algo_gpm_gport_hw_resources_t tunnel_init_gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.vxlan_port_id = vxlan_port_id;
    /**
     * Get vxlan_port info.
     */
    SHR_IF_ERR_EXIT(bcm_dnx_vxlan_port_get(unit, l2vpn, &vxlan_port));
    dnx_vxlan_port_gport_hw_resources_get(unit, &vxlan_port, &tunnel_term_gport_hw_resources,
                                          &tunnel_init_gport_hw_resources);

    SHR_IF_ERR_EXIT(dnx_vxlan_port_delete_ingress
                    (unit, &vxlan_port, &tunnel_term_gport_hw_resources, &tunnel_init_gport_hw_resources));
    SHR_IF_ERR_EXIT(dnx_vxlan_port_delete_egress(unit, &vxlan_port, &tunnel_init_gport_hw_resources));

    /**
     * Delete gport forward information from gport.
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_delete(unit, vxlan_port_id));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * clear an entry in ISEM: VNI, network domain to VSI
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] vni - Vxlan header field: VXLAN Network Identifier.
 *   VNI is used to identify the local vpn in the network.
 *   \param [in] forwarding_domain - forwarding domain. Identify the network in which the VNI belongs to.
 *   \return
 *    \retval Zero in case of NO ERROR.
 *    \retval Negative in case of an error, see \see shr_error_e.
 *   \remark
 *   * clear an entry in ISEM table {VNI, network domain} to VSI
 */
static int
dnx_vxlan_vpn_vni_to_vsi_clear(
    int unit,
    uint32 vni,
    uint32 forwarding_domain)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VNI2VSI, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VNI, vni);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, forwarding_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Function Clears the VNI to VRF mapping
 */

static shr_error_e
dnx_vxlan_vpn_vni_to_vrf_clear(
    int unit,
    uint32 vni,
    uint32 forwarding_domain)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VNI2VRF, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VNI, vni);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, forwarding_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**

 * \brief
 * get VNI, result of ESEM entry VSI, network domain to VNI.
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] vsi - virtual switch instance.
 *   \param [in] forwarding_domain - forwarding domain. Identify the network in which the VNI belongs to.
 *   \param [in] result_type - result type
 *   \param [in] display_err - enable/disable error print
 *   \param [out] vni - Vxlan header field: VXLAN Network Identifier.
 *   VNI is used to identify the local vpn in the network.
 *   \return
 *    \retval Zero in case of NO ERROR.
 *    \retval Negative in case of an error, see \see shr_error_e.
 */
static int
dnx_vxlan_vpn_vsi_to_vni_get(
    int unit,
    bcm_vpn_t vsi,
    uint32 forwarding_domain,
    uint32 *vni,
    uint32 *result_type,
    uint8 display_err)
{
    uint32 entry_handle_id;
    shr_error_e rv = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, forwarding_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    /** C_VID field is used only for AC entries*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, 0);
    if (display_err)
    {
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    }
    else
    {
        rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
    }

    if (rv == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type));
        if (((*result_type) == DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO) ||
            ((*result_type) == DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO_VLANS))
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_FWD_DOMAIN_NWK_NAME, INST_SINGLE, vni));
        }
    }
    else if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_SET_CURRENT_ERR(rv);
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** \brief
 *  Get the result type for VSI to VNI.
 *  There are 2 different result types:
 *  1) VSI, network_domain -> VNI
 *  2) VSI, network_domain -> VNI, AC
 *
 * For entry: fodo (VSI), network domain -> VNI, AC information
 * Entry may has already been set by another API: bcm_vlan_translation_set:
 * 1) AC information is represented by a virtual AC gport.
 *    created by bcm_vlan_port_create with criteria: BCM_VLAN_PORT_MATCH_NAMESPACE_VSI
 * 2) vlan_port_translation_set with flag: BCM_VLAN_ACTION_SET_EGRESS,
 *    add an entry fodo (VSI), network domain -> VNI, AC information.
 *    This API is responsible for setting the AC information. It doesn't modify VNI.
 *
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] vsi - virtual switch instance.
 *   \param [in] forwarding_domain_namespace - forwarding
 *          domain. Identify the network in which the VNI
 *          belongs to.
 *   \param [in] qos_map_id - configured qos_map_id.
 *   \param [out] existing_entry - Indicate if the entry already
 *          exist.
 *          It can return true even if UPDATE flags are not set.
 *          In case VSI, network_domain -> VNI, AC was added by
 *          bcm_vlan_translation_set API.
 *   \param [out] result_type - result_type
 *   \return
 *    \retval Zero in case of NO ERROR.
 *    \retval Negative in case of an error, see \see shr_error_e.*
 */
static int
dnx_vxlan_vpn_vsi_to_vni_result_type_get(
    int unit,
    bcm_vpn_t vsi,
    uint32 forwarding_domain_namespace,
    uint32 qos_map_id,
    uint8 *existing_entry,
    uint32 *result_type)
{
    uint32 vni;
    uint32 rv_vsi2vni;
    SHR_FUNC_INIT_VARS(unit);

    rv_vsi2vni = dnx_vxlan_vpn_vsi_to_vni_get(unit, vsi, forwarding_domain_namespace, &vni, result_type, FALSE);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv_vsi2vni, _SHR_E_NOT_FOUND);

    *existing_entry = UTILEX_NUM2BOOL(rv_vsi2vni != _SHR_E_NOT_FOUND);

    /*
     * fodo (VSI), network domain -> VNI, AC information was not found
     */
    if (rv_vsi2vni == _SHR_E_NOT_FOUND)
    {

        if (qos_map_id != DNX_QOS_INITIAL_MAP_ID)
        {
            *result_type = DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO_VLANS;
        }
        else
        {
            *result_type = DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get the mapping VRF to VNI
 */
static int
dnx_vxlan_vpn_vrf_to_vni_get(
    int unit,
    bcm_vpn_t vrf,
    uint32 forwarding_domain,
    uint32 *qos_map_id,
    uint32 *vni)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, forwarding_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, vrf);
    /** C_VID field is used only for AC entries*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L3_FODO);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FWD_DOMAIN_NWK_NAME, INST_SINGLE, vni);
    if (dnx_data_esem.feature.feature_get(unit, dnx_data_esem_feature_etps_properties_assignment))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, qos_map_id);
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * clear an entry in ESEM: VSI, network domain to VNI.
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] vsi - virtual switch instance.
 *   \param [in] forwarding_domain - forwarding domain. Identify the network in which the VNI belongs to.
 *   \return
 *    \retval Zero in case of NO ERROR.
 *    \retval Negative in case of an error, see \see shr_error_e.
 *   \remark
 *   * clear an entry in ESEM table {VSI, network domain} to VNI
 */
static int
dnx_vxlan_vpn_vsi_to_vni_clear(
    int unit,
    bcm_vpn_t vsi,
    uint32 forwarding_domain)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, forwarding_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    /** C_VID field is used only for AC entries*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, 0);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * clear an entry in ESEM: VSI, network domain to VNI.
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] vrf - virtual routing instance.
 *   \param [in] network_domain - Identify the network in which the VNI belongs to.
 *   \return
 *    \retval Zero in case of NO ERROR.
 *    \retval Negative in case of an error, see \see shr_error_e.
 *   \remark
 *   * clear an entry in ESEM table {VRF, network domain} to VNI
 */
static int
dnx_vxlan_vpn_vrf_to_vni_clear(
    int unit,
    bcm_vpn_t vrf,
    uint32 network_domain)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, network_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, vrf);
    /** C_VID field is used only for AC entries*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, 0);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Write Network domain to Vni mapping per VRF
 */
static shr_error_e
dnx_vxlan_vpn_vni_to_vrf_set(
    int unit,
    uint32 vni,
    uint32 network_domain,
    bcm_vpn_t vrf,
    uint8 update)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VNI2VRF, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, network_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VNI, vni);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, vrf);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_PD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FODO_PD_RESULT_TYPE_RESULT_FOUND);
    if (update)
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * End of Inner functions
 * }
 */

/*
 * APIs
 * {
 */

/**
 * \brief
 * Allocate and create a VSI, map VNI to VSI, configure MC groups for unknown \n
 * destinations
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in,out] info - A pointer to the struct that holds information for the vxlan vpn instance, \n
 *      [in,out] info.vpn - virtual switch instance (VSI). \n
 *      [in] info.vnid - Vxlan header field: VXLAN Network Identifier. VNI is used to identify/encapsulate the local vpn in the network. \n
 *      In case vnid is BCM_VXLAN_VNI_INVALID upon replace - remove identify/encapsulate entry.
 *      [in] info.match_port_class - Network domain (In DNX terminology VLAN domain). \n
 *      [in] info.broadcast_group - MC group for broadcast \n
 *      [in] info.unknown_multicast_group - MC group for unknown MC \n
 *      [in] info.unknown_unicast_group - MC group for unknown UC \n
 *      [in] info.flags - BCM_VXLAN_XXX flags \n
 *        BCM_VXLAN_VPN_WITH_ID - WITH-ID on the VPN ID (otherwise it is allocated) \n
 *        BCM_VXLAN_VPN_WITH_VPNID - WITH-ID on the VNI, must be set on API call. \n
 *        BCM_VXLAN_VPN_UNKNOWN_UCAST_REPLACE| BCM_VXLAN_VPN_UNKNOWN_MCAST_REPLACE| BCM_VXLAN_VPN_BCAST_REPLACE - \n
 *        Replace functionality for MC groups, must be called after object already created \n
 *        BCM_VXLAN_VPN_REPLACE - update vpnid.
 *
 *  \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error, see \see shr_error_e.
 * \remark
 *   * Allocated VSI.
 *   * The 3 MC groups (unknown MC, UC, BC) must have the same value.
 *   * Write an entry in ISEM table {VNI, network domain} to VSI
 *   * Write an entry in ESEM table {VSI, network domain} to VNI
 *   * info->vpn - See 'info->vpn' in DIRECT INPUT above.
 * \see
 *   * bcm_vxlan_vpn_config_t
 */
int
bcm_dnx_vxlan_vpn_create(
    int unit,
    bcm_vxlan_vpn_config_t * info)
{
    bcm_vlan_control_vlan_t vlan_control;
    uint8 with_id, update;
    uint32 alloc_flags;
    uint8 replaced = 0;
    uint32 old_vnid;
    int rv_vsi2vni;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vxlan_vpn_create_verify(unit, info));

    with_id = (info->flags & BCM_VXLAN_VPN_WITH_ID) ? TRUE : FALSE;
    update = (info->flags & _BCM_DNX_VXLAN_VPN_REPLACE_FLAGS) ? TRUE : FALSE;

    if (!update)
    {
        int vpn = (int) info->vpn;
        /*
         * allocate VSI (vpn)
         */
        alloc_flags = 0;
        if (with_id)
        {
            alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
        }
        SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_alloc(unit, alloc_flags, _bcmDnxVsiTypeVxlan, &vpn, &replaced));

        /*
         * if replaced, then update flag should be set
         */
        if (replaced)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "entry exist, BCM_VXLAN_VPN_REPLACE must be present in vpn create \r\n");
        }
        else
        {
            /*
             * Write defaults to HW VSI table
             */
            SHR_IF_ERR_EXIT(dnx_vsi_table_default_set(unit, vpn));
        }

        info->vpn = (bcm_vpn_t) vpn;
        /*
         * map vni <-> vsi (vpn)
         */
        if (info->vnid != BCM_VXLAN_VNI_INVALID)
        {
            uint32 vsi_to_vni_result_type;
            uint8 existing_vsi_to_vni_entry;
            /*
             * For entry: fodo (VSI), network domain -> VNI, AC information
             * Entry may has already been set by another API: bcm_vlan_translation_set:
             * 1) AC information is represented by a virtual AC gport.
             *    created by bcm_vlan_port_create with criteria: BCM_VLAN_PORT_MATCH_NAMESPACE_VSI
             * 2) vlan_port_translation_set with flag: BCM_VLAN_ACTION_SET_EGRESS,
             *    add an entry fodo (VSI), network domain -> VNI, AC information.
             *    This API is responsible for setting the AC information. It doesn't modify VNI.
             */
            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vsi_to_vni_result_type_get
                            (unit, info->vpn, info->match_port_class, DNX_QOS_INITIAL_MAP_ID,
                             &existing_vsi_to_vni_entry, &vsi_to_vni_result_type));
            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vni_to_vsi_set(unit, info->vnid, info->match_port_class, info->vpn, update));
            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vsi_to_vni_set
                            (unit, info->vpn, info->match_port_class, info->vnid,
                             vsi_to_vni_result_type, existing_vsi_to_vni_entry));
        }
        if (info->vnid != BCM_VXLAN_VNI_INVALID)
        {
            /** The mapping VNI2VSI is created by bcm_vxlan_vpn_create API */
            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_create_type_set
                            (unit, info->vpn, DBAL_ENUM_FVAL_VXLAN_VPN_MAPPING_CREATED_BY_API_VXLAN_VPN_CREATE));
        }
    }

    /*
     * create vswitch instance, configure MC group
     */
    bcm_vlan_control_vlan_t_init(&vlan_control);
    /*
     * Replace support: Replace MC groups, replace VNI.
     */
    if (update)
    {
        /** Update multicast groups if flags are set, replace all 3 if BCM_VXLAN_VPN_REPLACE flag is set*/
        SHR_IF_ERR_EXIT(dnx_vxlan_vpn_create_vlan_control_update(unit, &vlan_control, info));

        /** Replace the VNID for the VPN if BCM_VXLAN_VPN_REPLACE flag is set */
        if (info->flags & BCM_VXLAN_VPN_REPLACE)
        {
            uint32 vsi_to_vni_result_type;
            /** Try to get the old VNI from the VSI -> VNI mapping */
            rv_vsi2vni =
                dnx_vxlan_vpn_vsi_to_vni_get(unit, info->vpn, info->match_port_class, &old_vnid,
                                             &vsi_to_vni_result_type, FALSE);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv_vsi2vni, _SHR_E_NOT_FOUND);

            /** Check that VSI <-> VNI are not already unmapped due to update with BCM_VXLAN_VNI_INVALID */
            if (rv_vsi2vni != _SHR_E_NOT_FOUND)
            {
                /** Clear old VNI -> VSI mapping */
                SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vni_to_vsi_clear(unit, old_vnid, info->match_port_class));

                if (info->vnid == BCM_VXLAN_VNI_INVALID)
                {
                    /** If found and received BCM_VXLAN_VNI_INVALID, clear old VSI -> VNI mapping too */
                    SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vsi_to_vni_clear(unit, info->vpn, info->match_port_class));
                }
                else
                {
                    /** If found and received new valid VNI, update VSI -> VNI mapping and map the new VNI -> VSI */
                    SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vsi_to_vni_set
                                    (unit, info->vpn, info->match_port_class, info->vnid,
                                     vsi_to_vni_result_type, update));
                    SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vni_to_vsi_set
                                    (unit, info->vnid, info->match_port_class, info->vpn, FALSE));
                }
            }
            else if (info->vnid != BCM_VXLAN_VNI_INVALID)
            {
                /** entry wasn't found, get the result type  */
                uint8 existing_entry;
                SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vsi_to_vni_result_type_get
                                (unit, info->vpn, info->match_port_class, DNX_QOS_INITIAL_MAP_ID, &existing_entry,
                                 &vsi_to_vni_result_type));
                /** If not found and received valid VNI, create new VSI <-> VNI mappings */
                SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vsi_to_vni_set
                                (unit, info->vpn, info->match_port_class, info->vnid, vsi_to_vni_result_type, FALSE));
                SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vni_to_vsi_set
                                (unit, info->vnid, info->match_port_class, info->vpn, FALSE));
            }
        }
    }
    else
    {
        vlan_control.broadcast_group = info->broadcast_group;
        vlan_control.unknown_multicast_group = info->unknown_multicast_group;
        vlan_control.unknown_unicast_group = info->unknown_unicast_group;
        if (dnx_data_l2.general.l2_learn_limit_mode_get(unit) == LEARN_LIMIT_MODE_VLAN)
        {
            vlan_control.aging_cycles = 8;
        }
    }

    /** forwarding_vlan must be equal to VSI */
    vlan_control.forwarding_vlan = info->vpn;

    SHR_IF_ERR_EXIT(bcm_dnx_vlan_control_vlan_set(unit, info->vpn, vlan_control));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Gets VNID from VSI
 *   \param [in] unit -
 *     Relevant unit.
 *     \param [in] l2vpn - VPN VSI number
 *   \param [in,out] info -
 *   A pointer to the struct that holds information for the
 *   vxlan vpn instance, see \see bcm_vxlan_vpn_config_t.
 *   Field description:
 *   * [in] info->vpn - virtual switch instance (VSI).
 *   * [out] info->vni - Vxlan header field: VXLAN Network
 *   Identifier. VNI is used to identify the local vpn in the
 *   network.
 *   * [in] info->match_port_class - Network domain.
 *   * [out] info->broadcast_group,
 *   * [out] info->unknown_multicast_group,
 *   * [out] info->unknown_unicast_group - MC group for
 *     respectively broadcast, unknown MC and unknown UC. Those
 *     3 MC groups must have the same value.
 *  \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error, see \see shr_error_e.
 * \remark
 *   * Get an entry from ESEM table {VSI, network domain} to VNI
 * \see
 *   * None
 */
int
bcm_dnx_vxlan_vpn_get(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_vxlan_vpn_config_t * info)
{

    uint32 entry_handle_id;
    uint32 result_type;
    bcm_vlan_control_vlan_t vlan_control;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vxlan_vpn_get_verify(unit, l2vpn, info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, info->match_port_class);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, l2vpn);
    /** C_VID field is used only for AC entries*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, 0);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));

    if ((result_type == DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO)
        || (result_type == DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO_VLANS))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_FWD_DOMAIN_NWK_NAME, INST_SINGLE, &info->vnid));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "RESULT_TYPE  %d is not found! \n", result_type);
    }

    /** Get the multicast groups from vlan control */
    SHR_IF_ERR_EXIT(bcm_dnx_vlan_control_vlan_get(unit, l2vpn, &vlan_control));

    info->broadcast_group = vlan_control.broadcast_group;
    info->unknown_multicast_group = vlan_control.unknown_multicast_group;
    info->unknown_unicast_group = vlan_control.unknown_unicast_group;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Destroy and deallocate a VSI, unmap VNI to VSI,
 * re-initialize vswitch instance and MC groups
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] l2vpn - VSI number
 *  \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error, see \see shr_error_e.
 * \remark
 *   * Deallocated VSI.
 *   * Erase the entry in ISEM table {VNI, network domain} to VSI
 *   * Erase the entry in ESEM table {VSI, network domain} to VNI
 * \see
 *   * None
 */
int
bcm_dnx_vxlan_vpn_destroy(
    int unit,
    bcm_vpn_t l2vpn)
{
    uint32 vnid;
    uint32 network_domain, vxlan_vpn_create;
    int rv_vsi2vni;
    uint8 is_vsi_used;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vxlan_vpn_destroy_verify(unit, l2vpn));

    network_domain = 0x0;

    /*
     * get the vnid as only VPN/VSI is given
     */
    SHR_IF_ERR_EXIT(dnx_vxlan_vpn_create_type_get(unit, l2vpn, &vxlan_vpn_create));
    /** Unmap VNI2VSI only if the mapping was created by bcm_vxlan_vpn_create API*/
    if (vxlan_vpn_create == DBAL_ENUM_FVAL_VXLAN_VPN_MAPPING_CREATED_BY_API_VXLAN_VPN_CREATE)
    {
        rv_vsi2vni = dnx_vxlan_vpn_vsi_to_vni_get(unit, l2vpn, network_domain, &vnid, &result_type, FALSE);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv_vsi2vni, _SHR_E_NOT_FOUND);

        /*
         * unmap VNI <-> VSI (vpn) unless they were already unmapped due to update with BCM_VXLAN_VNI_INVALID
         */
        if (rv_vsi2vni != _SHR_E_NOT_FOUND)
        {
            /** If found clear old VSI <-> VNI mappings */
            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vsi_to_vni_clear(unit, l2vpn, network_domain));
            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vni_to_vsi_clear(unit, vnid, network_domain));
        }
    }

    /** check if VSI used by only by Vswitch */
    is_vsi_used = 0;
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_check_except_type(unit, l2vpn, _bcmDnxVsiTypeVxlan, &is_vsi_used));

    /** free vsi resource */
    SHR_IF_ERR_EXIT(dnx_vswitch_vsi_usage_dealloc(unit, _bcmDnxVsiTypeVxlan, l2vpn));
    SHR_IF_ERR_EXIT(dnx_vxlan_vpn_create_type_set
                    (unit, l2vpn, DBAL_ENUM_FVAL_VXLAN_VPN_MAPPING_CREATED_BY_API_UNKNOWN));

    if (!is_vsi_used)
    {
        SHR_IF_ERR_EXIT(dnx_vsi_table_clear_set(unit, l2vpn));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The API creates VNI-VSI or VNI-VRF mapping.
 * VSI Forwarding domain is used when the forwarding header after VXLAN/VXLAN-GPE is Ethernet.
 * VRF Forwarding domain is used when the forwarding header after VXLAN-GPE is IP.
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in,out] config -
 *   A pointer to the struct that holds information for the
 *   network domain instance, see \see bcm_vxlan_network_domain_config_t.
 *   Field description:
 *   * [in] config->vsi - virtual switch instance (VSI).
 *   * [in] config->vrf - virtual switch instance (VRF).
 *   * [in] config->network_domain - Network identifier. Used to distinct between multiple network.
 *   * [in] config->vni - VNI of the packet
 *   * [in] config->flags:
 *              * BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY - Add ingress only mapping
 *              * BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY - Add egress only mapping
 *              * BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING - Configure L2 mapping.
 *              * BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING - Configure L3 mapping.
 *              * BCM_VXLAN_NETWORK_DOMAIN_CONFIG_REPLACE - Replace the VNI to VSI or VNI to VRF mapping.
 *  \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error, see \see shr_error_e.
 * \remark
 *   * For ingress L2 mapping user is expected to configure VSI per network_domain x VNI.
 *   * For ingress L3 mapping user is expected to configure VRF per network_domain x VNI.
 *   * For egress L2 mapping user is expected to configure VNI per network_domain x VSI.
 *   * For egress L3 mapping user is expected to configure VNI per network_domain x VRF.

 * \see
 *   * None
 */
int
bcm_dnx_vxlan_network_domain_config_add(
    int unit,
    bcm_vxlan_network_domain_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vxlan_network_domain_config_add_verify(unit, config));
    /** Set ingress VNI to VSI mapping*/
    if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY)
        && _SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING))
    {
        /**
         * When user wants to use bcm_vxlan_network_domain_config_add, there might be 2 reasons to do it:
         * 1) vxlan vpn was created with bcm_vxlan_vpn_create with vni = invalid.
         * In this case to create the mapping vni2vsi bcm_vxlan_network_domain_config_add is used without REPLACE flag.
         * If the network domain is 0, store information from which api the mapping was created.
         * If network domain is != 0, the mapping can be created only from bcm_vxlan_network_domain_config_add api.
         *
         * 2) user wants to replace already created mapping by bcm_vxlan_network_domain_config_add api.
         */
        if (config->network_domain == 0)
        {
            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_create_type_set
                            (unit, config->vsi, DBAL_ENUM_FVAL_VXLAN_VPN_MAPPING_CREATED_BY_API_VXLAN_NETWORK_CONFIG));
        }
        if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_REPLACE))
        {
            /** update the mapping or create a new one based on the indication of update_mapping*/
            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vni_to_vsi_set(unit, config->vni, config->network_domain, config->vsi, TRUE));
        }
        /** Create new mapping VNI to VSI in ISEM*/
        else
        {

            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vni_to_vsi_set
                            (unit, config->vni, config->network_domain, config->vsi, FALSE));
        }
    }
    /** Set ingress VNI to VRF mapping*/
    else if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY)
             && _SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING))
    {
        if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_REPLACE))
        {
            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vni_to_vrf_set(unit, config->vni, config->network_domain, config->vrf, TRUE));
        }
        /** Create new VNI to VRF mappings at ingress*/
        else
        {
            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vni_to_vrf_set
                            (unit, config->vni, config->network_domain, config->vrf, FALSE));
        }
    }
    /** Set egress VSI to VNI mapping*/
    else if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY)
             && _SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING))
    {
        uint8 existing_entry;
        uint32 vsi_to_vni_result_type;
        SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vsi_to_vni_result_type_get
                        (unit, config->vsi, config->network_domain, config->qos_map_id, &existing_entry,
                         &vsi_to_vni_result_type));
        if (config->network_domain == 0)
        {
            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_create_type_set
                            (unit, config->vsi, DBAL_ENUM_FVAL_VXLAN_VPN_MAPPING_CREATED_BY_API_VXLAN_NETWORK_CONFIG));
        }
        if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_REPLACE))
        {
            /** Update VSI <-> VNI mapping */
            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vsi_to_vni_set
                            (unit, config->vsi, config->network_domain, config->vni, vsi_to_vni_result_type, TRUE));
        }
        else
        {

            /** If we don't want to update - create new VSI to VNI mappings at egress*/
            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vsi_to_vni_set
                            (unit, config->vsi, config->network_domain, config->vni, vsi_to_vni_result_type, FALSE));
        }
    }
    /** Set egress VRF to VNI mapping*/
    else if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY)
             && _SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING))
    {
        if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_REPLACE))
        {
            /** If not found and received valid VNI, create new VRF <-> VNI mappings */
            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vrf_to_vni_set
                            (unit, config->vrf, config->network_domain, config->vni, config->qos_map_id, TRUE));
        }
        else
        {
            /** If we don't want to update - create new VRF to VNI mappings at egress*/
            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vrf_to_vni_set
                            (unit, config->vrf, config->network_domain, config->vni, config->qos_map_id, FALSE));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The API gets VNI-VSI or VNI-VRF mapping.
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in,out] config -
 *   A pointer to the struct that holds information for the
 *   network domain instance, see \see bcm_vxlan_network_domain_config_t.
 *   Field description:
 *   * [in,out] config->vsi - virtual switch instance (VSI).
 *   * [in,out] config->vrf - virtual switch instance (VRF).
 *   * [in,out] config->vni - VNI of the packet
 *   * [in] config->network_domain - Network identifier. Used to distinct between multiple network.
 *   * [in] config->flags:
 *              * BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY - Get ingress only mapping
 *              * BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY - Get egress only mapping
 *              * BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING - Get L2 mapping.
 *              * BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING - Get L3 mapping.
 *  \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error, see \see shr_error_e.
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_vxlan_network_domain_config_get(
    int unit,
    bcm_vxlan_network_domain_config_t * config)
{
    uint32 vrf;
    uint32 qos_map_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vxlan_network_domain_config_get_verify(unit, config));
    /** Get ingress VNI to VSI mapping*/
    if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY)
        && _SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING))
    {
        SHR_IF_ERR_EXIT(dnx_vxlan_network_domain_map_vni_to_vsi_get
                        (unit, config->vni, config->network_domain, &config->vsi));
    }
    /** Get ingress VNI to VRF mapping*/
    else if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY)
             && _SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING))
    {
        SHR_IF_ERR_EXIT(dnx_vxlan_network_domain_map_vni_to_vrf_get(unit, config->vni, config->network_domain, &vrf));
        config->vrf = (bcm_vrf_t) vrf;
    }
    /** Get egress VSI to VNI mapping*/
    else if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY)
             && _SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING))
    {
        uint32 result_type;
        SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vsi_to_vni_get
                        (unit, config->vsi, config->network_domain, &config->vni, &result_type, TRUE));
    }
    /** Get egress VRF to VNI mapping*/
    else if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY)
             && _SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING))
    {
        SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vrf_to_vni_get
                        (unit, config->vrf, config->network_domain, &qos_map_id, &config->vni));
    }
    config->qos_map_id = (int) qos_map_id;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The API removes VNI-VSI or VNI-VRF mapping.
 * VSI Forwarding domain is used when the forwarding header after VXLAN/VXLAN-GPE is Ethernet.
 * VRF Forwarding domain is used when the forwarding header after VXLAN-GPE is IP.
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in,out] config -
 *   A pointer to the struct that holds information for the
 *   network domain instance, see \see bcm_vxlan_network_domain_config_t.
 *   Field description:
 *   * [in] config->vsi - virtual switch instance (VSI).
 *   * [in] config->vrf - virtual switch instance (VRF).
 *   * [in] config->network_domain - Network identifier. Used to distinct between multiple network.
 *   * [in] config->vni - VNI of the packet
 *   * [in] config->flags:
 *              * BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY - Remove ingress only mapping
 *              * BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY - Remove egress only mapping
 *              * BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING - Configure L2 mapping.
 *              * BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING - Configure L3 mapping.
 *  \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error, see \see shr_error_e.
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_vxlan_network_domain_config_remove(
    int unit,
    bcm_vxlan_network_domain_config_t * config)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vxlan_network_domain_config_get_verify(unit, config));
    /** Clear ingress VNI to VSI mapping*/
    if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY)
        && _SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING))
    {
        if (config->network_domain == 0)
        {
            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_create_type_set
                            (unit, config->vsi, DBAL_ENUM_FVAL_VXLAN_VPN_MAPPING_CREATED_BY_API_UNKNOWN));
        }
        SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vni_to_vsi_clear(unit, config->vni, config->network_domain));
    }
    /** Clear ingress VNI to VRF mapping*/
    else if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY)
             && _SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING))
    {
        SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vni_to_vrf_clear(unit, config->vni, config->network_domain));
    }
    /** Clear egress VSI to VNI mapping*/
    else if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY)
             && _SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING))
    {
        if (config->network_domain == 0)
        {
            SHR_IF_ERR_EXIT(dnx_vxlan_vpn_create_type_set
                            (unit, config->vsi, DBAL_ENUM_FVAL_VXLAN_VPN_MAPPING_CREATED_BY_API_UNKNOWN));
        }
        SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vsi_to_vni_clear(unit, config->vsi, config->network_domain));
    }
    /** Clear egress VRF to VNI mapping*/
    else if (_SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY)
             && _SHR_IS_FLAG_SET(config->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING))
    {
        SHR_IF_ERR_EXIT(dnx_vxlan_vpn_vrf_to_vni_clear(unit, config->vrf, config->network_domain));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The API traverses the mapping of VNI->VSI/VNI->VRF or VSI->VNI/VRF->VNI based on input flags.
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] additional_info -
 *   A pointer to the struct that defines specific rules for traverse.
 *   Field description:
 *   * [in] additional_info->flags:
 *              * BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY - ingress only mapping
 *              * BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY - egress only mapping
 *              * BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING - L2 mapping.
 *              * BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING - L3 mapping.
 *   \param [in] cb - cb function provided by  the user
 *   \param [in, out] user_data - data, returned by the traverse fun ction.
 *  \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error, see \see shr_error_e.
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_vxlan_network_domain_config_traverse(
    int unit,
    bcm_vxlan_network_domain_config_additional_info_t * additional_info,
    bcm_vxlan_network_domain_config_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id, vsi_or_vrf;
    uint32 core_id = DBAL_CORE_DEFAULT;
    uint32 dbal_tables[] = { 0, 0, 0 }, vni, vrf, fodo, result_type;
    uint32 nof_tables, vxlan_network_domain_add;
    dbal_fields_e sub_field_id;
    int is_end, table;
    bcm_vxlan_network_domain_config_t config;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);
    is_end = 0;
    SHR_INVOKE_VERIFY_DNX(dnx_vxlan_network_domain_config_traverse_verify
                          (unit, additional_info, cb, user_data, &config));
    /** Iterate over VNI2VSI table in case L2_MAPPING and INGRESS_ONLY flags are provided */
    if (_SHR_IS_FLAG_SET(additional_info->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING)
        && _SHR_IS_FLAG_SET(additional_info->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY))
    {
        dbal_tables[0] = DBAL_TABLE_VNI2VSI;
        nof_tables = 1;

    }
    /** Iterate over VNI2VRF table in case L3_MAPPING and INGRESS_ONLY flags are provided */
    else if (_SHR_IS_FLAG_SET(additional_info->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING)
             && _SHR_IS_FLAG_SET(additional_info->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY))
    {
        dbal_tables[0] = DBAL_TABLE_VNI2VRF;
        nof_tables = 1;
    }
    /** Iterate over ESEM_FORWARD_DOMAIN_MAPPING_DB table in case EGRESS_ONLY flag is provided */
    else if (_SHR_IS_FLAG_SET(additional_info->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY))
    {
        dbal_tables[0] = DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB;
        nof_tables = 1;
    }
    else if (_SHR_IS_FLAG_SET(additional_info->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY))
    {
        dbal_tables[0] = DBAL_TABLE_VNI2VSI;
        dbal_tables[1] = DBAL_TABLE_VNI2VRF;
        nof_tables = 2;
    }
    else
    {
        dbal_tables[0] = DBAL_TABLE_VNI2VSI;
        dbal_tables[1] = DBAL_TABLE_VNI2VRF;
        dbal_tables[2] = DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB;
        nof_tables = 3;
    }

    /*
     * Allocate handle to the table of the iteration and initialize an iterator entity.
     * The iterator is in mode ALL, which means that it will consider all entries regardless
     * of them being default entries or not.
     */
    for (table = 0; table < nof_tables; table++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[table], &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        if ((dbal_tables[table] != DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB))
        {
            /** Add KEY rule to skip duplicated entry in core 1 */
            SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                                                   DBAL_CONDITION_EQUAL_TO, &core_id, NULL));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            bcm_vxlan_network_domain_config_t_init(&config);
            /** For ingress L2 mapping get the vsi, network_domain and vni values*/
            if ((dbal_tables[table] == DBAL_TABLE_VNI2VSI))
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_VNI, &config.vni));
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, &config.network_domain));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, &config.vsi));
                SHR_IF_ERR_EXIT(dnx_vxlan_vpn_create_type_get(unit, config.vsi, &vxlan_network_domain_add));
                config.flags =
                    (BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY);
                if ((vxlan_network_domain_add != DBAL_ENUM_FVAL_VXLAN_VPN_MAPPING_CREATED_BY_API_VXLAN_NETWORK_CONFIG)
                    && (config.network_domain == 0))
                {
                    /** Receive next entry in table.*/
                    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                    continue;
                }
            }
            /** For ingress L3 mapping get the vrf, network_domain and vni values*/
            else if (dbal_tables[table] == DBAL_TABLE_VNI2VRF)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_VNI, &config.vni));
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, &config.network_domain));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, &vrf));
                config.vrf = vrf;
                config.flags =
                    (BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY);
            }
            /** For egress mapping read the DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB table and update the relevant fields*/
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, &config.network_domain));
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_FODO, &fodo));
                SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                                (unit, DBAL_FIELD_FODO, fodo, &sub_field_id, &vsi_or_vrf));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));
                if (result_type == DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L3_FODO)
                {
                    config.flags =
                        (BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY);
                    /** If only l2 egress entries are to be traversed, continue*/
                    if (_SHR_IS_FLAG_SET(additional_info->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING))
                    {
                        /** Receive next entry in table.*/
                        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                        continue;
                    }
                    else
                    {
                        config.vrf = vsi_or_vrf;
                    }
                    if (dnx_data_esem.feature.feature_get(unit, dnx_data_esem_feature_etps_properties_assignment))
                    {
                        uint32 qos_map_id = DNX_QOS_INITIAL_MAP_ID;

                        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE,
                                                            INST_SINGLE, &qos_map_id);
                        if (qos_map_id != DNX_QOS_INITIAL_MAP_ID)
                        {
                            DNX_QOS_EGRESS_MAP_SET(qos_map_id);
                            DNX_QOS_REMARK_MAP_SET(qos_map_id);
                        }
                        config.qos_map_id = qos_map_id;
                    }
                }
                else
                {
                    config.flags =
                        (BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY);
                    /** If only l3 egress entries are to be traversed, continue*/
                    if (_SHR_IS_FLAG_SET(additional_info->flags, BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING))
                    {
                        /** Receive next entry in table.*/
                        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                        continue;
                    }
                    else
                    {
                        config.vsi = vsi_or_vrf;
                    }
                }
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_FWD_DOMAIN_NWK_NAME, INST_SINGLE, &vni));
                config.vni = vni;
            }
            if (cb != NULL)
            {
                /*
                 * Invoke callback function
                 */
                SHR_IF_ERR_EXIT((*cb) (unit, &config, user_data));
            }
            /** Receive next entry in table.*/
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * End of APIs
 * }
 */
