/** \file tunnel_term.c
 *  General TUNNEL termination functionality for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL
/*
 * Include files.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include "tunnel_term_txsci.h"
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/tunnel.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/algo/tunnel/algo_tunnel.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/qos/qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tunnel.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_tunnel_access.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/lif_mngr/algo_in_lif_profile.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
#include <src/bcm/dnx/tunnel/tunnel_term_srv6_utils.h>
#include "tunnel_term_srv6.h"
#include "tunnel_term.h"
#include "tunnel_types.h"

/*
 * }
 */

/*
 * MACROs and ENUMs
 * {
 */

typedef struct dnx_tunnel_term_ipv6_tcam_table_result_s
{
    /*
     * depending on dip_idx_type (see result_dbal_field), it indicates: - index, to be used for next lookup - index or
     * default inlif if next lookup is not found 
     */
    uint32 dip_idx_data_or_local_inlif;
    /*
     * dip_idx_type is encoded in the dbal field prefix. See field type 
     */
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type_value;
} dnx_tunnel_term_ipv6_tcam_table_result_t;

typedef struct dnx_tunnel_term_inlif_info_s
{
    uint32 global_inlif;
    int propagation_prof;
    uint32 in_lif_profile;
    uint32 ecn_mapping_profile;
    uint32 flags;
    uint32 fodo;
    bcm_vlan_t default_vsi;
    uint32 vrf;
    uint32 fodo_assignment_mode;
} dnx_tunnel_term_inlif_info_t;

/** mask indicating that exact match is requested by the user */
#define DNX_TUNNEL_TERM_FLAGS_UNSUPPORTED \
 (BCM_TUNNEL_TERM_WLAN_REMOTE_TERMINATE |     BCM_TUNNEL_TERM_WLAN_SET_ROAM   | \
 BCM_TUNNEL_INIT_IPV4_SET_DF           |     BCM_TUNNEL_INIT_IPV6_SET_DF | \
 BCM_TUNNEL_INIT_IP4_ID_SET_FIXED      |   \
 BCM_TUNNEL_INIT_WLAN_TUNNEL_WITH_ID | BCM_TUNNEL_TERM_USE_OUTER_DSCP | BCM_TUNNEL_TERM_USE_OUTER_TTL | \
 BCM_TUNNEL_TERM_KEEP_INNER_DSCP)
#define DNX_TUNNEL_TERM_EM_L2TPV2_UDP_DST_PORT 1701
/*
 * The UDP destination port value in case of ESPoUDP
 */
#define DNX_TUNNEL_TERM_ESP_UDP_DST_PORT       500
#define DNX_TUNNEL_TERM_GPTV1U_UDP_DST_PORT    2152

typedef enum
{
    TUNNEL_TERM_OPERATION_CREATE,
    TUNNEL_TERM_OPERATION_GET,
    TUNNEL_TERM_OPERATION_DELETE,
    NOF_TUNNEL_TERM_FLOW_OPERATIONS
} tunnel_term_operation_e;

/*
 * End of MACROs
 * }
 */

/*
 * Verify functions
 * {
 */

uint8
dnx_tunnel_terminator_tunnel_type_is_ipv4(
    bcm_tunnel_type_t tunnel_type)
{
    switch (tunnel_type)
    {
        case bcmTunnelTypeGreAnyIn4:
        case bcmTunnelTypeIpAnyIn4:
        case bcmTunnelTypeUdp:
        case bcmTunnelTypeVxlan:
        case bcmTunnelTypeVxlanGpe:
        case bcmTunnelTypeGeneve:
            return TRUE;
        default:
            return FALSE;
    }
}

uint8
dnx_tunnel_terminator_tunnel_type_is_txsci(
    bcm_tunnel_type_t tunnel_type)
{
    switch (tunnel_type)
    {
        case bcmTunnelTypeEsp:
        case bcmTunnelTypeVxlanEsp:
            return TRUE;
        default:
            return FALSE;
    }
}

static uint8
dnx_tunnel_terminator_tunnel_type_is_cascaded(
    bcm_tunnel_type_t tunnel_type)
{
    return DNX_TUNNEL_TERM_SRV6_IS_CASCADED_TYPE(tunnel_type);
}

/*
 * return the TT lookup type
 */
tunnel_term_lookup_type_e
dnx_tunnel_terminator_lookup_type_get(
    bcm_tunnel_type_t tunnel_type)
{
    if (DNX_TUNNEL_TYPE_IS_IPV6(tunnel_type))
    {
        return TUNNEL_TERM_LOOKUP_TYPE_IPV6;
    }
    else if (dnx_tunnel_terminator_tunnel_type_is_ipv4(tunnel_type))
    {
        return TUNNEL_TERM_LOOKUP_TYPE_IPV4;
    }
    else if (dnx_tunnel_terminator_tunnel_type_is_txsci(tunnel_type))
    {
        return TUNNEL_TERM_LOOKUP_TYPE_TXSCI;
    }
    else if (dnx_tunnel_terminator_tunnel_type_is_cascaded(tunnel_type))
    {
        return TUNNEL_TERM_LOOKUP_TYPE_CASCADED;
    }
    else
    {
        return TUNNEL_TERM_LOOKUP_TYPE_UNSUPPORTED;
    }
}

/**
 * \brief
 * Used by bcm_dnx_tunnel_terminator_create_verify and bcm_dnx_tunnel_terminator_get_verify functions
 * to check tunnel type support.
 */
static shr_error_e
dnx_tunnel_terminator_tunnel_type_verify(
    int unit,
    bcm_tunnel_type_t tunnel_type)
{
    tunnel_term_lookup_type_e tunnel_term_lookup_type = dnx_tunnel_terminator_lookup_type_get(tunnel_type);
    SHR_FUNC_INIT_VARS(unit);
    if (tunnel_term_lookup_type == TUNNEL_TERM_LOOKUP_TYPE_UNSUPPORTED)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel termination type value (%d) is not supported", tunnel_type);
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_ipv6_tcam_table_id_get(
    int unit,
    bcm_tunnel_terminator_t * info,
    dbal_tables_e * table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    SHR_NULL_CHECK(table_id, _SHR_E_PARAM, "table_id");

    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_BUD))
    {
        *table_id = DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS;
    }
    else
    {
        *table_id = DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get ipv4 DBAL table id from tunnel_terminator API parameter.
 */
static shr_error_e
dnx_tunnel_terminator_dbal_table_id_get(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 *table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    SHR_NULL_CHECK(table_id, _SHR_E_PARAM, "table_id");

    if (DNX_TUNNEL_TERM_IS_IPV4_P2P(info))
    {
        /** EM lookup with DIP, SIP on 2nd pass tunnel termination */
        if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_BUD))
        {
            *table_id = DBAL_TABLE_IPV4_TT_P2P_EM_2ND_PASS;
        }
        else
        {
            /** EM lookup with DIP SIP */
            *table_id = DBAL_TABLE_IPV4_TT_P2P_EM_BASIC;
        }
    }
    else if (DNX_TUNNEL_TERM_IS_IPV4_MP(info))
    {
        /** EM lookup with DIP only on 2nd pass tunnel termination */
        if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_BUD))
        {
            *table_id = DBAL_TABLE_IPV4_TT_MP_EM_2ND_PASS;
        }
        else
        {
            /** EM lookup with DIP only (no SIP) */
            *table_id = DBAL_TABLE_IPV4_TT_MP_EM_BASIC;
        }
    }
    else
    {
        /** TCAM lookup with partial DIP/SIP masks on 2nd pass tunnel termination */
        if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_BUD))
        {
            *table_id = DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS;
        }
        else
        {
            /** TCAM lookup with partial DIP/SIP masks on 1st pass tunnel termination */
            *table_id = DBAL_TABLE_IPV4_TT_TCAM_BASIC;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_tunnel_terminator_config_ APIs to verify config_key struct.
 */
static shr_error_e
dnx_tunnel_terminator_config_key_verify(
    int unit,
    bcm_tunnel_terminator_config_key_t * config_key)
{
    tunnel_term_lookup_type_e lookup_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(config_key, _SHR_E_PARAM, "config_key");

    /** Verify that tunnel type is IPv6 */
    lookup_type = dnx_tunnel_terminator_lookup_type_get(config_key->type);

    if (lookup_type != TUNNEL_TERM_LOOKUP_TYPE_IPV6)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "tunnel_termintor_config APIs support only IPv6 P2P tunnels! Given config_key->type is %d",
                     config_key->type);
    }

    /** Verify VRF */
    if ((config_key->vrf < 0) || (config_key->vrf >= dnx_data_l3.vrf.nof_vrf_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VRF (%d) is out of range. The range should be 0 - %d", config_key->vrf,
                     (dnx_data_l3.vrf.nof_vrf_get(unit) - 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API: bcm_dnx_tunnel_terminator_traverse
 * \param [in] unit - the unit number.
 * \param [in] cb - the name of the callback function,
 *          it receives the value of the user_data variable and
 *          the key and result values for each found entry
 * \param [in] user_data - user data that will be sent to the callback function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  *
 */
static shr_error_e
dnx_tunnel_terminator_traverse_verify(
    int unit,
    bcm_tunnel_terminator_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "No callback function has been provided to the traverse.");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_class_id_range_verify(
    int unit,
    int tunnel_class)
{
    int tunnel_class_max_value = 0;
    SHR_FUNC_INIT_VARS(unit);

    {
        SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get(unit,
                                                        DBAL_TABLE_IPV6_TT_P2P_EM_BASIC,
                                                        DBAL_FIELD_IPV6_MP_TUNNEL_IDX, TRUE, 0, INST_SINGLE,
                                                        &tunnel_class_max_value));
    }

    if (tunnel_class > tunnel_class_max_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tunnel class value (%d) is invalid, max value: %d",
                     tunnel_class, tunnel_class_max_value);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_tunnel_terminator_config_ APIs to verify config_action
 * struct.
 */
static shr_error_e
dnx_tunnel_terminator_config_action_verify(
    int unit,
    bcm_tunnel_terminator_config_action_t * config_action)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(config_action, _SHR_E_PARAM, "config_action");

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_class_id_range_verify(unit, config_action->tunnel_class));

    /** Make sure tunnel_class (lif/vtep index) fits in nof available bits (6 as in J1) */
    if (config_action->tunnel_class >= 64)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tunnel class value (%d) is invalid, max nof bits available: %d",
                     config_action->tunnel_class, 6);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_tunnel_terminator_config_ APIs to verify info
 * struct.
 */
static shr_error_e
dnx_tunnel_terminator_config_verify(
    int unit,
    bcm_tunnel_terminator_config_key_t * config_key,
    bcm_tunnel_terminator_config_action_t * config_action)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_config_key_verify(unit, config_key));

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_config_action_verify(unit, config_action));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_create_vrf_verify(
    int unit,
    bcm_vrf_t vrf,
    const char *vrf_field_name)
{
    SHR_FUNC_INIT_VARS(unit);
    /** Verify VRF */
    if ((vrf < 0) || (vrf >= dnx_data_l3.vrf.nof_vrf_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "%s (%d) is out of range. The range should be 0 - %d", vrf_field_name, vrf,
                     (dnx_data_l3.vrf.nof_vrf_get(unit) - 1));
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief
 *  ensure default_vrf is used only for vxlan-gpe cases
 */
static shr_error_e
dnx_tunnel_terminator_create_default_vrf_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (DNX_TUNNEL_TYPE_IS_VXLAN_GPE(info->type))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_create_vrf_verify(unit, info->default_vrf, "default_vrf"));
    }

    if ((info->default_vrf != 0) &&
        (!(DNX_TUNNEL_TYPE_IS_VXLAN_GPE(info->type) || DNX_TUNNEL_TYPE_IS_GENEVE(info->type))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "default_vrf is supported only for tunnel type %d, %d, %d or %d, ",
                     bcmTunnelTypeVxlanGpe, bcmTunnelTypeVxlan6Gpe, bcmTunnelTypeGeneve, bcmTunnelTypeGeneve6);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_cascaded_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    uint8 is_cascaded = dnx_tunnel_terminator_tunnel_type_is_cascaded(info->type);
    uint8 is_valid_default_tunnel_id = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_TUNNEL(info->default_tunnel_id))
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        if (dnx_algo_gpm_gport_to_hw_resources
            (unit, info->default_tunnel_id, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources))
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "info->default_tunnel_id(0x%08X) does not exist!", info->default_tunnel_id);
        }
        else
        {
            is_valid_default_tunnel_id = TRUE;
        }
    }

    if (!is_valid_default_tunnel_id && is_cascaded)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "We expect to have a valid default tunnel id with cascaded types");
    }

    if (is_valid_default_tunnel_id && !is_cascaded)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "default_tunnel_id field is supported with cascaded type only");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_tunnel_class_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    /** Verify the info->tunnel_class is not supplied as API input*/
    if (BCM_GPORT_IS_SET(info->tunnel_class) && !BCM_GPORT_IS_TUNNEL(info->tunnel_class))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The tunnel_class must be tunnel GPORT or normal value (not GPORT)");
    }
    /** the tunnel_class is not a gport, it's a number set by the user, we validate its range */
    if (!BCM_GPORT_IS_SET(info->tunnel_class))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_class_id_range_verify(unit, info->tunnel_class));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_tunnel_terminator_verify to verify key parameters from info struct.
 */
static shr_error_e
dnx_tunnel_terminator_key_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    uint32 table_id = 0;
    tunnel_term_lookup_type_e lookup_type;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify that tunnel type is supported */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_tunnel_type_verify(unit, info->type));

    lookup_type = dnx_tunnel_terminator_lookup_type_get(info->type);

    if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_LOCATOR(info) || DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_FUNCT(info)
        || DNX_TUNNEL_TERM_SRV6_IS_CASCADED_MISS_DISABLE_TERM(info))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_key_verify(unit, info));
    }

    /** Verify the udp destination port are only supplied as API input for L2tpv2 or for an ESP or for GTP*/
    if ((info->udp_dst_port != 0) && (info->udp_dst_port != DNX_TUNNEL_TERM_EM_L2TPV2_UDP_DST_PORT) &&
        !((info->udp_dst_port == DNX_TUNNEL_TERM_ESP_UDP_DST_PORT) && dnx_data_switch.svtag.supported_get(unit)) &&
        (info->udp_dst_port != DNX_TUNNEL_TERM_GPTV1U_UDP_DST_PORT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "UDP Destination port are no longer user input");
    }

    /** Verify VRF */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_create_vrf_verify(unit, info->vrf, "vrf"));

    if (lookup_type == TUNNEL_TERM_LOOKUP_TYPE_IPV6)
    {
        if (DNX_TUNNEL_TERM_IS_IPV6_P2P(info))
        {
            if (info->priority != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Tunnel terminator entry configured for EM table, but priority is defined\n");
            }
        }
        else
        {
            if (info->priority == DNX_FIELD_TCAM_ENTRY_PRIORITY_INVALID
                || info->priority >= dnx_data_field.tcam.max_tcam_priority_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Priority value %d for TCAM entry is invalid - should be bigger than %d and should be less or equal than %d.\n",
                             info->priority, DNX_FIELD_TCAM_ENTRY_PRIORITY_INVALID,
                             dnx_data_field.tcam.max_tcam_priority_get(unit));
            }
        }
    }
    else if (lookup_type == TUNNEL_TERM_LOOKUP_TYPE_IPV4)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_dbal_table_id_get(unit, info, &table_id));
        if ((table_id == DBAL_TABLE_IPV4_TT_TCAM_BASIC) || (table_id == DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS))
        {
            if ((info->priority == DNX_FIELD_TCAM_ENTRY_PRIORITY_INVALID
                 || info->priority >= dnx_data_field.tcam.max_tcam_priority_get(unit)))
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Priority value %d for TCAM entry is invalid - should be bigger than %d and should be less or equal than %d.\n",
                             info->priority, DNX_FIELD_TCAM_ENTRY_PRIORITY_INVALID,
                             dnx_data_field.tcam.max_tcam_priority_get(unit));
        }
        else
        {
            if (info->priority != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Tunnel terminator entry configured for EM table, but priority is defined\n");
            }
        }
    }

    if (lookup_type == TUNNEL_TERM_LOOKUP_TYPE_TXSCI)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_term_txsci_create_verify(unit, info));
    }
    else
    {
        if (info->sci != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The TXSCI value (%d) should be set to 0.", info->sci);
        }
    }
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_tunnel_class_verify(unit, info));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_cascaded_verify(unit, info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_tunnel_terminator_create to verify tunnel type for the case of replace
 */
static shr_error_e
dnx_tunnel_terminator_tunnel_vxlan_geneve_type_verify(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 fodo_assignment_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_REPLACE))
    {
        uint8 next_set_type_is_vxlan_or_geneve, previous_set_type_is_vxlan_or_geneve;

        previous_set_type_is_vxlan_or_geneve =
            (fodo_assignment_mode == DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP);
        next_set_type_is_vxlan_or_geneve = DNX_TUNNEL_TYPE_IS_VXLAN_OR_VXLAN_GPE_OR_GENEVE(info->type);

        if (previous_set_type_is_vxlan_or_geneve && (!next_set_type_is_vxlan_or_geneve))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_TUNNEL_REPLACE vxlan or geneve tunnel type can't be replaced with non-vxlan or non-geneve tunnel type");
        }
        if ((!previous_set_type_is_vxlan_or_geneve) && next_set_type_is_vxlan_or_geneve)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_TUNNEL_REPLACE non-vxlan or non-geneve tunnel type can't be replaced with vxlan or geneve tunnel type");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_tunnel_terminator_key to verify lif parameters from info struct.
 */
static shr_error_e
dnx_tunnel_terminator_lif_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_lif_verify(unit, info));

    if ((info->ingress_qos_model.ingress_remark > bcmQosIngressModelUniform) ||
        (info->ingress_qos_model.ingress_phb > bcmQosIngressModelUniform) ||
        (info->ingress_qos_model.ingress_ttl > bcmQosIngressModelUniform) ||
        (info->ingress_qos_model.ingress_ecn > bcmQosIngressEcnModelEligible))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "one or more Ingress qos model remark(%d), phb(%d), ttl(%d) ECN(%d) are out of range",
                     info->ingress_qos_model.ingress_remark, info->ingress_qos_model.ingress_phb,
                     info->ingress_qos_model.ingress_ttl, info->ingress_qos_model.ingress_ecn);
    }

    if ((info->ingress_qos_model.ingress_ecn == bcmQosIngressEcnModelEligible) &&
        (info->type != bcmTunnelTypeVxlanGpe) && (info->type != bcmTunnelTypeVxlan6) &&
        (DNX_TUNNEL_TYPE_IS_GENEVE(info->type) == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "only VxLanGPE, Vxlan6, Geneve and Geneve6 support ecn eligible");
    }

    /** check REPLACE flag, must be used with WITH_ID flag */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_REPLACE) &&
        !_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_TUNNEL_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_TUNNEL_REPLACE can't be used without BCM_TUNNEL_TERM_TUNNEL_WITH_ID as well");
    }

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_create_default_vrf_verify(unit, info));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * Used by bcm_dnx_tunnel_terminator_key to verify unsupported parameters from info struct.
 */
static shr_error_e
dnx_tunnel_terminator_unsupported_params_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** tunnel LIF no longer points to RIF */
    if (info->tunnel_if != BCM_IF_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "tunnel_if parameter cannot be used to create RIF. Please use bcm_l3_ingress_create instead with intf_id as IP tunnel.");
    }

    /** check the usage of BCM_TUNNEL_TERM_ETHERNET flag */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_ETHERNET))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_TUNNEL_TERM_ETHERNET is used only on Arad+ device");
    }
    if (info->flags & DNX_TUNNEL_TERM_FLAGS_UNSUPPORTED)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "The used flag or combination of flags 0x%x is not supported with 0x%x",
                     info->flags, DNX_TUNNEL_TERM_FLAGS_UNSUPPORTED);
    }

    /** Verify the udp source port are not supplied as API input*/
    if (info->udp_src_port != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "UDP Source port are no longer user input");
    }

    /** Verify the pbmp is not supplied as API input*/
    if (BCM_PBMP_NOT_NULL(info->pbmp))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pbmp is no longer user input, please read the backward compatibility doc.");
    }
    /** Verify the ingress_failover_id and failover_info_id are not supplied as API input*/
    if ((info->ingress_failover_id != 0) || (info->failover_tunnel_id != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Failover ID is no longer user input, please read the backward compatibility doc.");
    }
    /** Verify the inlif_counting_profile is not supplied as API input*/
    if (info->inlif_counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Statistics are supported with dedicated API, please read the backward compatibility document");
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * Used by bcm_dnx_tunnel_terminator_create to verify info struct.
 */
static shr_error_e
dnx_tunnel_terminator_create_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    /** verification of key parameters integrity */
    SHR_INVOKE_VERIFY_DNXC(dnx_tunnel_terminator_key_verify(unit, info));

    /** verification of lif parameters integrity */
    SHR_INVOKE_VERIFY_DNXC(dnx_tunnel_terminator_lif_verify(unit, info));

    /** verification of unsupported parameters integrity */
    SHR_INVOKE_VERIFY_DNXC(dnx_tunnel_terminator_unsupported_params_verify(unit, info));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * Used by bcm_dnx_tunnel_terminator_get_delete to verify info struct key parameters.
 */
static shr_error_e
dnx_tunnel_terminator_get_delete_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    /** verification of key parameters integrity */
    SHR_INVOKE_VERIFY_DNXC(dnx_tunnel_terminator_key_verify(unit, info));

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

/**
 * \brief
 * Convert IPv4 tunnel type to IPV4 layer qualifer tunnel type.
 * Tunnel type is later used by DBAL to identify the tunnel
 * termination as part of the look-up key
 *
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] info -
 *     A pointer to the struct that holds information for the IPv4 tunnel terminator entry.
 *   \param [out] ipv4_layer_qualifier_tunnel_type -
 *     IPv4 layer qualifier tunnel type
 */
shr_error_e
dnx_tunnel_terminator_type_to_ipv4_layer_qualifier_tunnel_type(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 *ipv4_layer_qualifier_tunnel_type)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ipv4_layer_qualifier_tunnel_type, _SHR_E_PARAM, "ipv4_layer_qualifier_tunnel_type");

    switch (info->type)
    {
        case bcmTunnelTypeGreAnyIn4:
            *ipv4_layer_qualifier_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE4;
            break;
        case bcmTunnelTypeIpAnyIn4:
            *ipv4_layer_qualifier_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_RAW;
            break;
        case bcmTunnelTypeUdp:
            if (info->udp_dst_port == DNX_TUNNEL_TERM_EM_L2TPV2_UDP_DST_PORT)
            {
                *ipv4_layer_qualifier_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_L2TPV3OUDP;
            }
            else if (info->udp_dst_port == DNX_TUNNEL_TERM_GPTV1U_UDP_DST_PORT)
            {
                *ipv4_layer_qualifier_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GTPUOUDP;
            }
            else
            {
                *ipv4_layer_qualifier_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_UDP;
            }
            break;
        case bcmTunnelTypeVxlan:
            *ipv4_layer_qualifier_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_VXLANOUDP;
            break;
        case bcmTunnelTypeVxlanGpe:
            *ipv4_layer_qualifier_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_VXLAN_GPEOUDP;
            break;
        case bcmTunnelTypeGeneve:
            *ipv4_layer_qualifier_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GENEVEOUDP;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel type value (%d) is not supported", info->type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Convert IPV4 layer qualifier to tunnel type IPv4 tunnel type.
 */
shr_error_e
dnx_ipv4_layer_qualifier_tunnel_type_to_tunnel_terminator_type(
    int unit,
    uint32 ipv4_layer_qualifier_tunnel_type,
    bcm_tunnel_type_t * tunnel_type,
    uint32 *udp_dst_port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tunnel_type, _SHR_E_PARAM, "tunnel_type");

    switch (ipv4_layer_qualifier_tunnel_type)
    {
        case DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE4:
            *tunnel_type = bcmTunnelTypeGreAnyIn4;
            break;
        case DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_RAW:
            *tunnel_type = bcmTunnelTypeIpAnyIn4;
            break;
        case DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_UDP:
            *tunnel_type = bcmTunnelTypeUdp;
            break;
        case DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_L2TPV3OUDP:
            *tunnel_type = bcmTunnelTypeUdp;
            *udp_dst_port = DNX_TUNNEL_TERM_EM_L2TPV2_UDP_DST_PORT;
            break;
        case DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GTPUOUDP:
            *tunnel_type = bcmTunnelTypeUdp;
            *udp_dst_port = DNX_TUNNEL_TERM_GPTV1U_UDP_DST_PORT;
            break;
        case DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_VXLANOUDP:
            *tunnel_type = bcmTunnelTypeVxlan;
            break;
        case DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_VXLAN_GPEOUDP:
            *tunnel_type = bcmTunnelTypeVxlanGpe;
            break;
        case DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GENEVEOUDP:
            *tunnel_type = bcmTunnelTypeGeneve;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel qualifier value (%d) is not supported",
                         ipv4_layer_qualifier_tunnel_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Convert tunnel type to next layer type. Next layer type is
 * later used by DBAL to identify the tunnel termination as part
 * of the look-up key.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] tunnel_type -
 *     Tunnel termination type.
 *   \param [out] next_layer_type -
 *     Next layer type (above IP tunnel).
 */
static shr_error_e
dnx_tunnel_terminator_type_to_next_layer_type(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint8 *next_layer_type)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(next_layer_type, _SHR_E_PARAM, "next_layer_type");

    switch (tunnel_type)
    {
        case bcmTunnelTypeGreAnyIn4:
        case bcmTunnelTypeIpAnyIn4:
        case bcmTunnelTypeUdp:
        case bcmTunnelTypeVxlan:
        case bcmTunnelTypeVxlanGpe:
        case bcmTunnelTypeGeneve:
        case bcmTunnelTypeGreAnyIn6:
        case bcmTunnelTypeIpAnyIn6:
        case bcmTunnelTypeVxlan6:
        case bcmTunnelTypeVxlan6Gpe:
        case bcmTunnelTypeGeneve6:
        case bcmTunnelTypeUdp6:
        case bcmTunnelTypeIp6In6:
            /*
             * Next Layer Type: ANY
             */
            *next_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel type value (%d) is not supported", tunnel_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Convert IPV6 tunnel type to IPv6 layer qualifier tunnel type
 * first and second additional headers. These fields are later
 * used by DBAL to identify the tunnel termination as part of
 * the look-up key.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] tcam_table_key -
 *     Tunnel termination tcam table key.
 *   \param [out] first_additional_header -
 *     IPv4/IPv6 first additional header.
 *   \param [out] second_additional_header -
 *     IPv4/IPv6 second additional header.
 */
shr_error_e
dnx_tunnel_terminator_type_to_ipv6_additional_headers(
    int unit,
    dnx_tunnel_term_ipv6_tcam_table_key_t * tcam_table_key,
    uint32 *first_additional_header,
    uint32 *second_additional_header)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (tcam_table_key->type)
    {
        case bcmTunnelTypeGreAnyIn6:
            /*
             * GRE6 tunnel: IPv6 first additional header = GRE4
             *              IPv6 second additional header = NONE
             */
            *first_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_GRE4;
            *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE;
            break;
        case bcmTunnelTypeIpAnyIn6:
            /*
             * IPv6 tunnel: IPv6 first additional header = NONE
             *              IPv6 second additional header = NONE
             */
            *first_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE;
            *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE;
            break;
        case bcmTunnelTypeVxlan6:
            /*
             * IPv6 tunnel: IPv6 first additional header = UDP
             *              IPv6 second additional header = VXLAN
             */
            *first_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP;
            *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_VXLAN;
            break;
        case bcmTunnelTypeVxlan6Gpe:
            /*
             * IPv6 tunnel: IPv6 first additional header = UDP
             *              IPv6 second additional header = VXLAN GPE
             */
            *first_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP;
            *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_VXLAN_GPE;
            break;
        case bcmTunnelTypeUdp6:
            /*
             * IPv6 tunnel: IPv6 first additional header = UDP
             *              IPv6 second additional header = NONE
             */
            *first_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP;
            if (tcam_table_key->udp_dst_port == DNX_TUNNEL_TERM_GPTV1U_UDP_DST_PORT)
            {
                *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_GTPOUDP;
            }
            else
            {
                *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE;
            }

            break;
        case bcmTunnelTypeEthIn6:
            /*
             * IPv6 tunnel: IPv6 first additional header = NONE
             *              IPv6 second additional header = NONE
             */
            *first_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE;
            *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE;
            break;
        case bcmTunnelTypeGeneve6:
            /*
             * IPv6 tunnel: IPv6 first additional header = UDP
             *              IPv6 second additional header = GENEVE
             */
            *first_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP;
            *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_GENEVE;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel type value (%d) is not supported", tcam_table_key->type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/** \brief
 * The function returns bcm tunnel type based on additional headers
 */
shr_error_e
dnx_tunnel_terminator_additional_headers_to_ipv6_type(
    int unit,
    uint32 first_additional_header,
    uint32 second_additional_header,
    bcm_tunnel_type_t * tunnel_type,
    uint16 *udp_dst_port)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((first_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_GRE4) &&
        (second_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE))
    {
        *tunnel_type = bcmTunnelTypeGreAnyIn6;
    }
    else if ((first_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE) &&
             (second_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE))
    {
        *tunnel_type = bcmTunnelTypeIpAnyIn6;
    }
    else if ((first_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP) &&
             (second_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_VXLAN))
    {
        *tunnel_type = bcmTunnelTypeVxlan6;
    }
    else if ((first_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP) &&
             (second_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_VXLAN_GPE))
    {
        *tunnel_type = bcmTunnelTypeVxlan6Gpe;
    }
    else if ((first_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP) &&
             (second_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE))
    {
        *tunnel_type = bcmTunnelTypeUdp6;
    }
    else if ((first_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP) &&
             (second_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_GTPOUDP))
    {
        *tunnel_type = bcmTunnelTypeUdp6;
        *udp_dst_port = DNX_TUNNEL_TERM_GPTV1U_UDP_DST_PORT;
    }
    else if ((first_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP) &&
             (second_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_GENEVE))
    {
        *tunnel_type = bcmTunnelTypeGeneve6;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel type value (%d) is not supported", *tunnel_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set tunnel termination VTT lookup key fields based on table id (SEM or TCAM).
 *
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] core_id -
 *     Core ID
 *   \param [in] entry_handle_id -
 *     A DBAL table entry handle index.
 *   \param [in] table_id -
 *     ID of the DBAL table.
 *   \param [in] info -
 *     A pointer to the struct that holds information for the IPv4 tunnel terminator entry.
 *   \remark
 *   * SEM P2P DBAL table -  DBAL_TABLE_IPV4_TT_P2P_EM_BASIC or
 *                           DBAL_TABLE_IPV4_TT_P2P_EM_BASIC
 *
 *   * SEM MP DBAL table -   DBAL_TABLE_IPV4_TT_MP_EM_BASIC or
 *                           DBAL_TABLE_IPV4_TT_MP_EM_BASIC
 *
 *   * TCAM DBAL table -     DBAL_TABLE_IPV4_TT_TCAM_BASIC or
 *                           DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS
 *
 * Shared by:
 *  - dnx_tunnel_terminator_lookup_add_ipv4
 *  - dnx_tunnel_terminator_lookup_get_ipv4
 *  - dnx_tunnel_terminator_lookup_delete_ipv4
 *
 */
static shr_error_e
dnx_tunnel_terminator_lookup_key_set_ipv4(
    int unit,
    int core_id,
    uint32 entry_handle_id,
    uint32 table_id,
    bcm_tunnel_terminator_t * info)
{
    uint32 tunnel_type = 0;
    uint8 next_layer_type = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_type_to_ipv4_layer_qualifier_tunnel_type(unit, info, &tunnel_type));

    /** Set generic key fields: {VRF,SIP,DIP,IPV4 tunnel type} */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, info->vrf);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_TUNNEL_TYPE, tunnel_type);

    /** Set table-specific key fields */
    if ((table_id == DBAL_TABLE_IPV4_TT_TCAM_BASIC) || (table_id == DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS))
    {   /** TCAM access */
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_type_to_next_layer_type(unit, info->type, &next_layer_type));

        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, info->dip, info->dip_mask);
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, info->sip, info->sip_mask);

        if (next_layer_type == DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION)
        {
            dbal_entry_key_field8_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, next_layer_type, 0x0);
        }
        else
        {
            dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, next_layer_type);
        }
    }
    else
    {   /** ESEM access MP and P2P */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, info->dip);

        if ((table_id == DBAL_TABLE_IPV4_TT_P2P_EM_BASIC) || (table_id == DBAL_TABLE_IPV4_TT_P2P_EM_2ND_PASS))
        {   /** P2P */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, info->sip);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get tunnel termination VTT lookup key tunnel class(local-LIF) from Tunnel-ID(global-LIF).
 *
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] info -
 *     A pointer to the struct that holds information for the IPv4/IPv6 tunnel terminator entry.
 *   \param [out] tunnel_class -
 *     tunnel class.
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
static shr_error_e
dnx_tunnel_terminator_lookup_key_tunnel_class_get(
    int unit,
    bcm_tunnel_terminator_t * info,
    int *tunnel_class)
{
    tunnel_term_lookup_type_e lookup_type;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    *tunnel_class = info->tunnel_class;

    lookup_type = dnx_tunnel_terminator_lookup_type_get(info->type);

    if (lookup_type == TUNNEL_TERM_LOOKUP_TYPE_IPV6)
    {
        if (DNX_TUNNEL_TERM_IS_IPV6_P2P(info))
        {
            /** P2P lookup with the same DIP+VRF with the MP, the tunnel_class is the local-LIF */
            if (BCM_GPORT_IS_TUNNEL(info->tunnel_class))
            {
                /** Get Local-in-LIF ID */
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, info->tunnel_class,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));
                *tunnel_class = gport_hw_resources.local_in_lif;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set tunnel termination config lookup key to DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] entry_handle_id - DBAL entry handle id
 *   \param [in] tcam_table_key - tunnel termination key struct
 *
 * Used for cascaded tunnel termination lookup configuration:
 *   1) {DIP, VRF, Tunnel type} -> my-vtep-index (config_action.tunnel_class) - bcm_tunnel_terminator_config_add
 *   2) {SIP, my-vtep-index, VRF} -> IP-Tunnel In-LIF                         - bcm_tunnel_terminator_create
 *
 * Shared by:
 *  - bcm_dnx_tunnel_terminator_config_add
 *  - bcm_dnx_tunnel_terminator_config_get
 *  - bcm_dnx_tunnel_terminator_config_delete
 *
 */
static shr_error_e
dnx_tunnel_terminator_ipv6_tcam_table_key_set_internal(
    int unit,
    uint32 entry_handle_id,
    dnx_tunnel_term_ipv6_tcam_table_key_t * tcam_table_key)
{
    uint32 first_additional_header = 0;
    uint32 second_additional_header = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_type_to_ipv6_additional_headers
                    (unit, tcam_table_key, &first_additional_header, &second_additional_header));

    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP, tcam_table_key->dip6,
                                         tcam_table_key->dip6_mask);

    if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_FIRST_ADDITIONAL_HEADER(first_additional_header))
    {
        dnx_tunnel_terminator_srv6_ipv6_tcam_table_key_set_first_additional_header_set(unit, entry_handle_id,
                                                                                       first_additional_header);
    }
    else if (first_additional_header != DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST,
                                   first_additional_header);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_2ND,
                                   second_additional_header);
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, tcam_table_key->vrf);

exit:
    SHR_FUNC_EXIT;
}

void
dnx_tunnel_term_config_key_to_ipv6_tcam_table_key(
    bcm_tunnel_terminator_config_key_t * config_key,
    dnx_tunnel_term_ipv6_tcam_table_key_t * tcam_table_key)
{
    tcam_table_key->type = config_key->type;
    tcam_table_key->udp_dst_port = config_key->udp_dst_port;
    sal_memcpy(tcam_table_key->dip6, config_key->dip6, sizeof(bcm_ip6_t));
    sal_memcpy(tcam_table_key->dip6_mask, config_key->dip6_mask, sizeof(bcm_ip6_t));
    tcam_table_key->vrf = config_key->vrf;
}

void
dnx_tunnel_term_to_ipv6_tcam_table_key(
    bcm_tunnel_terminator_t * tunnel_terminator,
    dnx_tunnel_term_ipv6_tcam_table_key_t * tcam_table_key)
{
    tcam_table_key->type = tunnel_terminator->type;
    tcam_table_key->udp_dst_port = tunnel_terminator->udp_dst_port;
    sal_memcpy(tcam_table_key->dip6, tunnel_terminator->dip6, sizeof(bcm_ip6_t));
    sal_memcpy(tcam_table_key->dip6_mask, tunnel_terminator->dip6_mask, sizeof(bcm_ip6_t));
    tcam_table_key->vrf = tunnel_terminator->vrf;
}

static shr_error_e
dnx_tunnel_terminator_config_ipv6_tcam_table_key_set(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_terminator_config_key_t * config_key)
{
    dnx_tunnel_term_ipv6_tcam_table_key_t tcam_table_key;
    SHR_FUNC_INIT_VARS(unit);
    dnx_tunnel_term_config_key_to_ipv6_tcam_table_key(config_key, &tcam_table_key);
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_tcam_table_key_set_internal(unit, entry_handle_id, &tcam_table_key));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_ipv6_tcam_table_key_set(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_terminator_t * info)
{
    dnx_tunnel_term_ipv6_tcam_table_key_t tcam_table_key;
    SHR_FUNC_INIT_VARS(unit);
    dnx_tunnel_term_to_ipv6_tcam_table_key(info, &tcam_table_key);
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_tcam_table_key_set_internal(unit, entry_handle_id, &tcam_table_key));
exit:
    SHR_FUNC_EXIT;
}

static void
dnx_tunnel_term_to_ipv6_tcam_table_result(
    bcm_tunnel_terminator_t * info,
    uint32 dip_idx_data_or_local_inlif,
    dnx_tunnel_term_ipv6_tcam_table_result_t * tcam_table_result)
{
    tcam_table_result->dip_idx_data_or_local_inlif = dip_idx_data_or_local_inlif;
    dnx_tunnel_terminator_srv6_to_ipv6_tcam_table_result_dbal_field(info, &tcam_table_result->dip_idx_type_value);
}

static void
dnx_tunnel_term_ipv6_tcam_table_result_set_internal(
    int unit,
    uint32 entry_handle_id,
    dnx_tunnel_term_ipv6_tcam_table_result_t * tcam_table_result)
{
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP_IDX_TYPE, INST_SINGLE,
                                 tcam_table_result->dip_idx_type_value);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP_IDX_DATA, INST_SINGLE,
                                 tcam_table_result->dip_idx_data_or_local_inlif);
}

static void
dnx_tunnel_terminator_ipv6_tcam_table_result_set(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_terminator_t * info,
    uint32 local_inlif)
{
    dnx_tunnel_term_ipv6_tcam_table_result_t tcam_table_result;
    dnx_tunnel_term_to_ipv6_tcam_table_result(info, local_inlif, &tcam_table_result);
    dnx_tunnel_term_ipv6_tcam_table_result_set_internal(unit, entry_handle_id, &tcam_table_result);
}

static void
dnx_tunnel_term_config_key_to_ipv6_tcam_table_result(
    uint32 tunnel_class,
    dnx_tunnel_term_ipv6_tcam_table_result_t * tcam_table_result)
{
    tcam_table_result->dip_idx_type_value = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_INTERMIDIATE_RESULT;
    tcam_table_result->dip_idx_data_or_local_inlif = tunnel_class;
}

static void
dnx_tunnel_terminator_config_ipv6_tcam_table_result_set(
    int unit,
    uint32 entry_handle_id,
    uint32 tunnel_class)
{
    dnx_tunnel_term_ipv6_tcam_table_result_t tcam_table_result;
    dnx_tunnel_term_config_key_to_ipv6_tcam_table_result(tunnel_class, &tcam_table_result);
    dnx_tunnel_term_ipv6_tcam_table_result_set_internal(unit, entry_handle_id, &tcam_table_result);
}

static shr_error_e
dnx_tunnel_terminator_ipv6_tcam_result_get(
    int unit,
    uint32 entry_handle_id,
    dbal_enum_value_field_ipv6_dip_idx_type_e * dip_idx_type,
    uint32 *local_inlif)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IPV6_DIP_IDX_TYPE, INST_SINGLE, dip_idx_type));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IPV6_DIP_IDX_DATA, INST_SINGLE, local_inlif));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_ipv6_tcam_key_field_tunnel_type(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_type_t * tunnel_type,
    uint16 *udp_dst_port)
{
    uint32 first_additional_header, second_additional_header;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST, &first_additional_header));
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_2ND,
                     &second_additional_header));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_additional_headers_to_ipv6_type
                    (unit, first_additional_header, second_additional_header, tunnel_type, udp_dst_port));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function returns information stored for Ipv6 TCAM Tunnel Terminator
 */
static shr_error_e
dnx_tunnel_terminator_ipv6_tcam_key_fields(
    int unit,
    uint32 entry_handle_id,
    bcm_ip6_t * dip6,
    bcm_ip6_t * dip6_mask,
    int *vrf,
    bcm_tunnel_type_t * tunnel_type,
    uint16 *udp_dst_port)
{
    uint32 vrf_value = 0;
    uint8 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint8 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_masked_get
                    (unit, entry_handle_id, DBAL_FIELD_IPV6_DIP, field_value, field_mask));
    sal_memcpy(*dip6, field_value, sizeof(bcm_ip6_t));
    sal_memcpy(*dip6_mask, field_mask, sizeof(bcm_ip6_t));
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VRF, &vrf_value));
    *vrf = vrf_value;
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_tcam_key_field_tunnel_type
                    (unit, entry_handle_id, tunnel_type, udp_dst_port));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Write tunnel termination VTT lookup to HW (SEM or TCAM).
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] info -
 *     A pointer to the struct that holds information for the IPv4 tunnel terminator entry.
 *   \param [in] local_inlif -
 *     Allocated Local-In-LIF.
 *   \remark
 *   * SEM P2P DBAL table -  DBAL_TABLE_IPV4_TT_P2P_EM_BASIC or
 *                           DBAL_TABLE_IPV4_TT_P2P_EM_BASIC
 *
 *   * SEM MP DBAL table -   DBAL_TABLE_IPV4_TT_MP_EM_BASIC or
 *                           DBAL_TABLE_IPV4_TT_MP_EM_BASIC
 *
 *   * TCAM DBAL table -     DBAL_TABLE_IPV4_TT_TCAM_BASIC or
 *                           DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS
 */
static shr_error_e
dnx_tunnel_terminator_lookup_add_ipv4(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 local_inlif)
{
    uint32 entry_handle_id;
    uint32 table_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** GET DBAL table ID */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_dbal_table_id_get(unit, info, &table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Create entry access id to TCAM or TCAM 2nd pass table*/
    if ((table_id == DBAL_TABLE_IPV4_TT_TCAM_BASIC) || (table_id == DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS))
    {
        SHR_IF_ERR_EXIT(dbal_entry_attribute_set(unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, info->priority));
    }

    /** Set lookup-specific tunnel termination key fields based on table_id */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_key_set_ipv4(unit, DBAL_CORE_ALL, entry_handle_id, table_id, info));

    /** Set DATA field - {in LIF} */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_inlif);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_sip6_msbs_lsb_get(
    int unit,
    uint32 *sip6_msbs_lsb)
{
    uint32 ipv6_nof_bits = UTILEX_PP_IPV6_ADDRESS_NOF_BITS;
    int ipv6_msb_nof_bits = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, DBAL_TABLE_IPV6_TT_SIP_MSB_COMPRESS_EM, DBAL_FIELD_IPV6, TRUE, 0, 0, &ipv6_msb_nof_bits));
    *sip6_msbs_lsb = ipv6_nof_bits - ipv6_msb_nof_bits;
exit:
    SHR_FUNC_EXIT;
}

/** clear sip6 lsbs */
static shr_error_e
dnx_tunnel_terminator_sip6_msbs_get(
    int unit,
    bcm_ip6_t * sip6,
    bcm_ip6_t * sip6_msbs)
{
    uint32 sip6_long[UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S];
    uint32 sip6_msbs_lsb = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(utilex_pp_ipv6_address_struct_to_long(*sip6, sip6_long));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_sip6_msbs_lsb_get(unit, &sip6_msbs_lsb));
    SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range(sip6_long, 0, sip6_msbs_lsb - 1));
    SHR_IF_ERR_EXIT(utilex_pp_ipv6_address_long_to_struct(sip6_long, *sip6_msbs));

exit:
    SHR_FUNC_EXIT;
}

/** Allocate msb sip6 profile */
shr_error_e
dnx_tunnel_terminator_sip6_msbs_profile_allocate(
    int unit,
    bcm_ip6_t * sip6,
    int *sip_profile_id,
    uint8 *first_reference)
{
    bcm_ip6_t sip6_msbs;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_sip6_msbs_get(unit, sip6, &sip6_msbs));
    SHR_IF_ERR_EXIT(algo_tunnel_db.tunnel_ipv6_sip_profile.allocate_single
                    (unit, 0, &(sip6_msbs), NULL, sip_profile_id, first_reference));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_msb_sip6_profile_get(
    int unit,
    bcm_ip6_t * sip6,
    int *sip_profile_id)
{
    bcm_ip6_t sip6_msbs;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_sip6_msbs_get(unit, sip6, &sip6_msbs));
    SHR_IF_ERR_EXIT(algo_tunnel_db.tunnel_ipv6_sip_profile.profile_get
                    (unit, (const bcm_ip6_t *) &sip6_msbs, sip_profile_id));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Add an entry from dbal table ipv6_sip_msb_compress_em.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] ipv6_sip - A pointer to the struct that holds
 *          ipv6 sip used as key in the table
 *   \param [in] ipv6_sip_profile - the sip profile index
 *   \remark
 *   * DBAL_TABLE_IPV6_TT_SIP_MSB_COMPRESS_EM
 */
shr_error_e
dnx_tunnel_terminator_ipv6_sip_compress_table_entry_add(
    int unit,
    uint8 ipv6_sip[16],
    int ipv6_sip_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_TT_SIP_MSB_COMPRESS_EM, &entry_handle_id));

    /** Set KEY fields */
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_IPV6, ipv6_sip);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    /** Set VALUE fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_ADDRESS_MSBS_ID, INST_SINGLE, ipv6_sip_profile);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Write ipv6 P2P tunnel termination VTT lookup to HW .
 *
 *   \param [in] unit -Relevant unit.
 *   \param [in] info -A pointer to the struct that holds
 *     information for the IPv6 tunnel terminator entry.
 *   \param [in] local_inlif - Allocated Local-In-LIF.
 *   \remark
 *   * DBAL_TABLE_IPV6_TT_SIP_MSB_COMPRESS_EM
 *   * DBAL_TABLE_IPV6_TT_P2P_EM_BASIC
 */
static shr_error_e
dnx_tunnel_terminator_lookup_add_ipv6_p2p(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 local_inlif)
{
    uint32 entry_handle_id;
    int sip_profile_id;
    int tunnel_class;
    uint8 first_reference;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_sip6_msbs_profile_allocate
                    (unit, &(info->sip6), &sip_profile_id, &first_reference));

    if (first_reference)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_sip_compress_table_entry_add(unit, info->sip6, sip_profile_id));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_TT_P2P_EM_BASIC, &entry_handle_id));

    /** Set KEY fields */
    /** Set IPv6 SIP MSBs profile */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_ADDRESS_MSBS_ID, sip_profile_id);
    /** Set IPv6 SIP LSBs */
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_IPV6, info->sip6);

    /** Get tunnel class */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_key_tunnel_class_get(unit, info, &tunnel_class));

    /** Set IPv6 tunnel class */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_MP_TUNNEL_IDX, tunnel_class);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    /** Set VALUE fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_inlif);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Write IPv6 MP tunnel termination VTT lookup to HW.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] info - A pointer to the struct that holds
 *     information for the IPv6 tunnel terminator entry.
 *   \param [in] local_inlif - Allocated Local-In-LIF.
 *   \remark
 *   * DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC
 *   * DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS
 */
static shr_error_e
dnx_tunnel_terminator_lookup_add_ipv6_mp(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 local_inlif)
{
    uint32 entry_handle_id;

    dbal_tables_e tcam_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_tcam_table_id_get(unit, info, &tcam_table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, tcam_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_attribute_set(unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, info->priority));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_tcam_table_key_set(unit, entry_handle_id, info));
    dnx_tunnel_terminator_ipv6_tcam_table_result_set(unit, entry_handle_id, info, local_inlif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get IPv6 MP tunnel termination VTT lookup from HW.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] info - A pointer to the struct that holds
 *          information for the IPv6 tunnel terminator entry.
 *   \param [out] local_inlif - Allocated Local-in-LIF.
 *   \remark
 *   * DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC
 *   * DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS
 */
static shr_error_e
dnx_tunnel_terminator_lookup_get_ipv6_mp(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 *local_inlif)
{
    uint32 entry_access_id;
    uint32 entry_handle_id, tcam_handler_id;
    dbal_tables_e table_id;
    dbal_fields_e sub_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_tcam_table_id_get(unit, info, &table_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_tcam_table_key_set(unit, entry_handle_id, info));

    SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, table_id, &tcam_handler_id));
    SHR_IF_ERR_EXIT_NO_MSG(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_priority_get
                    (unit, tcam_handler_id, entry_access_id, (uint32 *) &(info->priority)));
    /** Get the tcam result */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Resolve the tcam result */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_tcam_result_get(unit, entry_handle_id, &sub_field_id, local_inlif));
    dnx_tunnel_terminator_srv6_ipv6_tcam_result_flag_resolve(unit, sub_field_id, &info->flags);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Write tunnel termination VTT lookup to HW (SEM or TCAM).
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] info - A pointer to the struct that holds
 *     information for the IPv4/IPv6 tunnel terminator entry.
 *   \param [in] local_inlif -
 *     Allocated Local-In-LIF.
 *
 */
static shr_error_e
dnx_tunnel_terminator_lookup_add(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 local_inlif)
{
    tunnel_term_lookup_type_e lookup_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    lookup_type = dnx_tunnel_terminator_lookup_type_get(info->type);

    if (lookup_type == TUNNEL_TERM_LOOKUP_TYPE_TXSCI)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_term_txsci_lookup_add(unit, info, local_inlif));
    }
    else if (lookup_type == TUNNEL_TERM_LOOKUP_TYPE_IPV6)
    {
        if (DNX_TUNNEL_TERM_IS_IPV6_P2P(info))
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_add_ipv6_p2p(unit, info, local_inlif));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_add_ipv6_mp(unit, info, local_inlif));
        }
    }
    else if (lookup_type == TUNNEL_TERM_LOOKUP_TYPE_CASCADED)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_lookup_add_ipv6_cascaded_funct(unit, info, local_inlif));

    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_add_ipv4(unit, info, local_inlif));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get tunnel termination VTT lookup from HW (SEM or TCAM).
 *
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] info -
 *     A pointer to the struct that holds information for the IPv4 tunnel terminator entry.
 *   \param [out] local_inlif -
 *     Allocated Local-in-LIF.
 *   \remark
 *   * SEM P2P DBAL table -  DBAL_TABLE_IPV4_TT_P2P_EM_BASIC or
 *                           DBAL_TABLE_IPV4_TT_P2P_EM_BASIC
 *
 *   * SEM MP DBAL table -   DBAL_TABLE_IPV4_TT_MP_EM_BASIC or
 *                           DBAL_TABLE_IPV4_TT_MP_EM_BASIC
 *
 *   * TCAM DBAL table -     DBAL_TABLE_IPV4_TT_TCAM_BASIC or
 *                           DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS
 */
static shr_error_e
dnx_tunnel_terminator_lookup_get_ipv4(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 *local_inlif)
{
    uint32 entry_handle_id;
    uint32 entry_access_id, tcam_handler_id;
    uint32 table_id = 0;
    int core_id = DBAL_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_dbal_table_id_get(unit, info, &table_id));

    /** take handle to read from DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** In DPC table, core needs to be well defined in order to perform a get operation */
    if (core_id == DBAL_CORE_ALL)
    {
        core_id = DBAL_CORE_DEFAULT;
    }

    /** Set lookup-specific tunnel termination key fields based on table_id */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_key_set_ipv4(unit, core_id, entry_handle_id, table_id, info));

    if ((table_id == DBAL_TABLE_IPV4_TT_TCAM_BASIC) || (table_id == DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS))
    {
        SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, table_id, &tcam_handler_id));
        SHR_IF_ERR_EXIT_NO_MSG(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_priority_get
                        (unit, tcam_handler_id, entry_access_id, (uint32 *) &(info->priority)));
    }

    /** Set pointer to receive field - {in LIF} */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_inlif);

    /** Read from DBAL - check the returned value in calling function */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get IPv6 P2P tunnel termination VTT lookup from HW.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] info - A pointer to the struct that holds
 *     information for the IPv6 tunnel terminator entry.
 *   \param [out] local_inlif - Allocated Local-in-LIF.
 *   \remark
 *   * DBAL_TABLE_IPV6_TT_SIP_MSB_COMPRESS_EM
 *   * DBAL_TABLE_IPV6_TT_P2P_EM_BASIC
 */
static shr_error_e
dnx_tunnel_terminator_lookup_get_ipv6_p2p(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 *local_inlif)
{
    uint32 entry_handle_id;
    int sip_profile_id;
    int tunnel_class;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_msb_sip6_profile_get(unit, &(info->sip6), &sip_profile_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_TT_P2P_EM_BASIC, &entry_handle_id));

    /** Set KEY fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_ADDRESS_MSBS_ID, sip_profile_id);
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_IPV6, info->sip6);
    /** Get tunnel class */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_key_tunnel_class_get(unit, info, &tunnel_class));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_MP_TUNNEL_IDX, tunnel_class);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);

    /** Set pointer to receive field - {in LIF} */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_inlif);

    /** Read from DBAL - check the returned value in calling function */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get tunnel termination VTT lookup from HW (SEM or TCAM).
 *
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] info -
 *     A pointer to the struct that holds information for the IPv4 tunnel terminator entry.
 *   \param [out] local_inlif -
 *     Allocated Local-in-LIF.
 *   \remark
 *   * SEM DBAL table - DBAL_TABLE_IPV4_TT_P2P_EM_BASIC or
 *                      DBAL_TABLE_IPV4_TT_P2P_EM_2ND_PASS
 *   * TCAM DBAL table - DBAL_TABLE_IPV4_TT_TCAM_BASIC or
 *                       DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS
 */
static shr_error_e
dnx_tunnel_terminator_lookup_get(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 *local_inlif)
{
    tunnel_term_lookup_type_e lookup_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    lookup_type = dnx_tunnel_terminator_lookup_type_get(info->type);

    if (lookup_type == TUNNEL_TERM_LOOKUP_TYPE_TXSCI)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_term_txsci_lookup_get(unit, info, local_inlif));
    }
    else if (lookup_type == TUNNEL_TERM_LOOKUP_TYPE_IPV6)
    {
        if (DNX_TUNNEL_TERM_IS_IPV6_P2P(info))
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_get_ipv6_p2p(unit, info, local_inlif));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_get_ipv6_mp(unit, info, local_inlif));
        }
    }
    else if (lookup_type == TUNNEL_TERM_LOOKUP_TYPE_CASCADED)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_lookup_get_ipv6_cascaded_funct(unit, info, local_inlif));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_get_ipv4(unit, info, local_inlif));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete an entry from dbal table ipv6_sip_msb_compress_em.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] ipv6_sip - A pointer to the struct that holds
 *          ipv6 sip used as key in the table
 *   \remark
 *   * DBAL_TABLE_IPV6_TT_SIP_MSB_COMPRESS_EM
 */
shr_error_e
dnx_tunnel_terminator_ipv6_sip_compress_table_entry_delete(
    int unit,
    uint8 ipv6_sip[16])
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_TT_SIP_MSB_COMPRESS_EM, &entry_handle_id));

    /** Set KEY fields */
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_IPV6, ipv6_sip);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete ipv6 P2P tunnel termination VTT lookup from HW .
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] info - A pointer to the struct that holds
 *          information for the IPv6 P2P tunnel terminator
 *          entry.
 *   \remark
 *   * DBAL_TABLE_IPV6_TT_SIP_MSB_COMPRESS_EM
 *   * DBAL_TABLE_IPV6_TT_P2P_EM_BASIC
 */
static shr_error_e
dnx_tunnel_terminator_lookup_delete_ipv6_p2p(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    uint8 last_reference = FALSE;
    uint32 entry_handle_id;
    int sip_profile_id;
    int tunnel_class;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_msb_sip6_profile_get(unit, &(info->sip6), &sip_profile_id));
    SHR_IF_ERR_EXIT(algo_tunnel_db.tunnel_ipv6_sip_profile.free_single(unit, sip_profile_id, &last_reference));

    if (last_reference)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_sip_compress_table_entry_delete(unit, info->sip6));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_TT_P2P_EM_BASIC, &entry_handle_id));

    /** Set KEY fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_ADDRESS_MSBS_ID, sip_profile_id);
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_IPV6, info->sip6);
    /** Get tunnel class */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_key_tunnel_class_get(unit, info, &tunnel_class));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_MP_TUNNEL_IDX, tunnel_class);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete IPv6 mp tunnel termination VTT lookup from HW .
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] info - A pointer to the struct that holds
 *          information for the IPv6 mp tunnel terminator entry.
 *   \remark
 *   * DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC
 *   * DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS
 */
static shr_error_e
dnx_tunnel_terminator_lookup_delete_ipv6_mp(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    uint32 entry_access_id;
    uint32 entry_handle_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_tcam_table_id_get(unit, info, &table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_tcam_table_key_set(unit, entry_handle_id, info));

    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete tunnel termination VTT lookup from HW (SEM or TCAM).
 *
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] info -
 *     A pointer to the struct that holds information for the IPv4 tunnel terminator entry.
 *   \remark
 *   * SEM P2P DBAL table -  DBAL_TABLE_IPV4_TT_P2P_EM_BASIC or
 *                           DBAL_TABLE_IPV4_TT_P2P_EM_BASIC
 *
 *   * SEM MP DBAL table -   DBAL_TABLE_IPV4_TT_MP_EM_BASIC or
 *                           DBAL_TABLE_IPV4_TT_MP_EM_BASIC
 *
 *   * TCAM DBAL table -     DBAL_TABLE_IPV4_TT_TCAM_BASIC or
 *                           DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS
 */
static shr_error_e
dnx_tunnel_terminator_lookup_delete_ipv4(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    uint32 entry_handle_id;
    uint32 entry_access_id;
    uint32 table_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_dbal_table_id_get(unit, info, &table_id));

    /** take handle to delete from DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Set lookup-specific tunnel termination key fields based on table_id */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_key_set_ipv4(unit, DBAL_CORE_ALL, entry_handle_id, table_id, info));

    if ((table_id == DBAL_TABLE_IPV4_TT_TCAM_BASIC) || (table_id == DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS))
    {
        SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
        /** Delete entry from DBAL table*/
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        /** Delete entry from DBAL table*/
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete tunnel termination VTT lookup from HW (SEM or TCAM).
 *
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] info -
 *     A pointer to the struct that holds information for the IPv4 tunnel terminator entry.
 *   \remark
 *   * SEM DBAL table - DBAL_TABLE_IPV4_TT_P2P_EM_BASIC
 *   * TCAM DBAL table - DBAL_TABLE_IPV4_TT_TCAM_BASIC
 */
static shr_error_e
dnx_tunnel_terminator_lookup_delete(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    tunnel_term_lookup_type_e lookup_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    lookup_type = dnx_tunnel_terminator_lookup_type_get(info->type);

    if (lookup_type == TUNNEL_TERM_LOOKUP_TYPE_TXSCI)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_term_txsci_lookup_delete(unit, info));
    }
    else if (lookup_type == TUNNEL_TERM_LOOKUP_TYPE_IPV6)
    {
        if (DNX_TUNNEL_TERM_IS_IPV6_P2P(info))
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_delete_ipv6_p2p(unit, info));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_delete_ipv6_mp(unit, info));
        }
    }
    else if (lookup_type == TUNNEL_TERM_LOOKUP_TYPE_CASCADED)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_lookup_delete_ipv6_cascaded_funct(unit, info));

    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_delete_ipv4(unit, info));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get optional vsi in tunnel term lif
 *   \param [in] unit - Relevant unit.
 *   \param [in] fodo - forwarding_domain. 
 *   \param [out] vsi - vsi from the inlif table entry
 */
static shr_error_e
dnx_tunnel_terminator_in_lif_table_optional_vsi_get(
    int unit,
    uint32 fodo,
    bcm_vlan_t * vsi)
{
    uint32 vsi32;
    dbal_fields_e sub_field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get(unit, DBAL_FIELD_FODO, fodo, &sub_field_id, &vsi32));

    if (sub_field_id != DBAL_FIELD_VSI)
    {
        *vsi = 0;
    }
    else
    {
        *vsi = (bcm_vlan_t) vsi32;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * 
 * @author ms946006 (16-Jun-20)
 * 
 *   \param [in] unit - Relevant unit.
 *   \param [in] entry_handle_id - entry handle id
 *   \param fodo_is_vsi - 
 * 
 * @return shr_error_e 
 */
static shr_error_e
dnx_tunnel_terminator_in_lif_table_optional_fodo_is_vsi(
    int unit,
    uint32 entry_handle_id,
    uint8 *fodo_is_vsi)
{
    uint8 is_field_valid;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_FODO, &is_field_valid));
    if (is_field_valid == TRUE)
    {
        uint32 fodo;
        uint32 vsi;
        dbal_fields_e sub_field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_FODO, INST_SINGLE, &fodo));
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get(unit, DBAL_FIELD_FODO, fodo, &sub_field_id, &vsi));
        *fodo_is_vsi = (sub_field_id == DBAL_FIELD_VSI);
    }
    else
    {
        *fodo_is_vsi = FALSE;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get optional vrf in tunnel term lif
 *   \param [in] unit - Relevant unit.
 *   \param [in] entry_handle_id - entry handle id
 *   \param [out] vrf - vrf from the inlif table entry
 */
static shr_error_e
dnx_tunnel_terminator_in_lif_table_optional_vrf_get(
    int unit,
    uint32 entry_handle_id,
    uint32 *vrf)
{
    uint8 is_field_valid;
    uint8 fodo_is_vsi = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** in case fodo vsi, can't ask if vrf, otherwise return error */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_optional_fodo_is_vsi(unit, entry_handle_id, &fodo_is_vsi));
    if (fodo_is_vsi)
    {
        *vrf = 0;
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_VRF, &is_field_valid));
    if (is_field_valid == TRUE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, vrf));
    }
    else
    {
        *vrf = 0;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * set fodo vsi to lif table 
 *   \param [in] unit - Relevant unit.
 *   \param [in] entry_handle_id - entry handle id
 *   \param [in] vsi - vsi
 */
static shr_error_e
dnx_tunnel_terminator_in_lif_table_fodo_vsi_set(
    int unit,
    uint32 entry_handle_id,
    bcm_vlan_t vsi)
{
    uint32 fodo = 0;
    uint32 vsi32 = (uint32) vsi;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set(unit, DBAL_FIELD_FODO, DBAL_FIELD_VSI, &vsi32, &fodo));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO, INST_SINGLE, fodo);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * set fodo vrf to lif table 
 *   \param [in] unit - Relevant unit.
 *   \param [in] entry_handle_id - entry handle id
 *   \param [in] vrf - vrf
 */
static shr_error_e
dnx_tunnel_terminator_in_lif_table_fodo_vrf_set(
    int unit,
    uint32 entry_handle_id,
    bcm_vrf_t vrf)
{
    uint32 fodo = 0;
    uint32 default_vrf = (uint32) vrf;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set(unit, DBAL_FIELD_FODO, DBAL_FIELD_VRF, &default_vrf, &fodo));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO, INST_SINGLE, fodo);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * set fodo or vrf, according to case.  
 * vrf is not set by tunnel_terminator_create api
 * Except for vxlan/vxlan-gpe/geneve, vrf will be updated by bcm_l3_ingress_create API.
 * But we want to force the lif table mgr to select a result type which include VRF field
 * vsi is set for srv6 
 * default vrf for vxlan gpe application when vni->vrf lookup miss 
 *   \param [in] unit - Relevant unit.
 *   \param [in] entry_handle_id - entry handle id
 *   \param [in] info - tunnel termination info. 
 *   \param [in] fodo_assignment_mode - fodo assignment mode
 *   \param [in] vrf - vrf
 *   \param [in] default_vsi - default vsi
 */
static shr_error_e
dnx_tunnel_terminator_in_lif_table_optional_vsi_or_vrf_set(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_terminator_t * info,
    uint32 fodo_assignment_mode,
    uint32 vrf,
    bcm_vlan_t default_vsi)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * vsi for srv6
     */
    if (DNX_TUNNEL_TERM_SRV6_IS_L2_VPN(info->type))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_fodo_vsi_set(unit, entry_handle_id, info->vlan));
    }
    /*
     * vrf for vxlan gpe when no hit in {vni, network domain} lookup
     */
    else if (DNX_TUNNEL_TYPE_IS_VXLAN_OR_VXLAN_GPE_OR_GENEVE(info->type))
    {
        if (default_vsi)
        {
            /*
             * vrf for vxlan gpe when no hit in {vni, network domain -> FODO vsi} lookup
             */
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_fodo_vsi_set(unit, entry_handle_id, default_vsi));
        }
        else
        {
            /*
             * vrf for vxlan gpe when no hit in {vni, network domain -> FODO vrf} lookup
             */
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_fodo_vrf_set(unit, entry_handle_id, info->default_vrf));
        }
    }
    else if (fodo_assignment_mode == DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, vrf);
    }
    else if (DNX_TUNNEL_TYPE_IS_VXLAN_OR_VXLAN_GPE_OR_GENEVE(info->type) && (default_vsi))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, default_vsi);
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get optional default vrf in tunnel term lif
 *   \param [in] unit - Relevant unit.
 *   \param [in] fodo - forwarding_domain. 
 *   \param [out] default_vrf - default vrf from the inlif table entry
 */
static shr_error_e
dnx_tunnel_terminator_in_lif_table_optional_default_vrf_get(
    int unit,
    uint32 fodo,
    bcm_vrf_t * default_vrf)
{
    uint32 default_vrf32;
    dbal_fields_e sub_field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get(unit, DBAL_FIELD_FODO, fodo, &sub_field_id, &default_vrf32));

    if (sub_field_id != DBAL_FIELD_VRF)
    {
        *default_vrf = 0;
    }
    else
    {
        *default_vrf = (bcm_vrf_t) default_vrf32;
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 * Allocate local inlif.
 * Write to IP tunnel termination in-LIF table.
 *
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] info -
 *     A pointer to the struct that holds information for the IPv4 tunnel terminator entry.
 *   \param [in] propagation_prof -
 *     Allocated propagation profile.
 *   \param [in] fodo_assignment_mode - fodo assignment mode
 *   \param [out] global_inlif - Allocated Glboal-In-LIF.
 *   \param [in] in_lif_profile - In lif profile,
 *   \param [in] ecn_mapping_profile - ECN mapping profile
 *   \param [in] vrf - vrf retrieved from lif entry.
 *   \param [in] default_vsi - default vsi
 *   \param [out] local_inlif - Allocated Local-In-LIF.
 *   */
static shr_error_e
dnx_tunnel_terminator_allocate_lif_and_in_lif_table_set(
    int unit,
    bcm_tunnel_terminator_t * info,
    int propagation_prof,
    uint32 fodo_assignment_mode,
    uint32 in_lif_profile,
    uint32 global_inlif,
    uint32 ecn_mapping_profile,
    uint32 vrf,
    bcm_vlan_t default_vsi,
    uint32 *local_inlif)
{
    uint32 entry_handle_id;
    lif_table_mngr_inlif_info_t lif_table_mngr_inlif_info;
    uint8 is_replace;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    sal_memset(&lif_table_mngr_inlif_info, 0, sizeof(lif_table_mngr_inlif_info_t));

    is_replace = _SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_REPLACE);

    if (is_replace)
    {
        lif_table_mngr_inlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    }

    lif_table_mngr_inlif_info.global_lif = global_inlif;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_IPvX_TUNNELS, &entry_handle_id));

    /*
     * Set DATA fields
     */
    /** Result type is superset, will be decided internally by lif table manager */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE,
                                 BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROFILE, INST_SINGLE,
                                 (uint32) propagation_prof);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                 (uint32) in_lif_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_MAPPING_PROFILE, INST_SINGLE,
                                 ecn_mapping_profile);

    /** set P2P if needed */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_CROSS_CONNECT))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P);
        /** If tunnel is cross connect, set service type to P2P */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, 0);
    }

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_optional_vsi_or_vrf_set
                    (unit, entry_handle_id, info, fodo_assignment_mode, vrf, default_vsi));

    if (DNX_TUNNEL_TYPE_IS_VXLAN_OR_VXLAN_GPE_OR_GENEVE(info->type))
    {
        /*
         * Set fodo assignment mode only when needed
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE,
                                     fodo_assignment_mode);

        /** for vxlan or geneve, we set the learn enable as TRUE only at lif creation */
        if ((DNX_TUNNEL_TYPE_IS_VXLAN(info->type) || DNX_TUNNEL_TYPE_IS_GENEVE(info->type)) && (!is_replace))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, INST_SINGLE, TRUE);
        }
    }

    /** NEXT_LAYER_NETWORK_DOMAIN determine by different API and is a key for choosing GTPV1U LIF.
     * Therefore, it must exist in the IP LIF
     * */
    if (((info->type == bcmTunnelTypeUdp) || (info->type == bcmTunnelTypeUdp6))
        && (info->udp_dst_port == DNX_TUNNEL_TERM_GPTV1U_UDP_DST_PORT))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, INST_SINGLE, 0);
    }
    /*
     * Set stat fields (STAT_OBJECT_ID, STAT_OBJECT_CMD) to default value (0) if indicated by the user
     */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_STAT_ENABLE))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, 0);
    }

    /*
     * wide-data support(10bits)
     */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_WIDE))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_GENERIC_DATA_0, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_GENERIC_DATA_1, INST_SINGLE, 0);
    }

    /*
     * Write to HW
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
                    (unit, _SHR_CORE_ALL, entry_handle_id, (int *) local_inlif, &lif_table_mngr_inlif_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Read qos propagation type from propagation profile
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] propagation_prof - QOS propagation profile.
 *   \param [out] info - tunnel terination information.
 */
static shr_error_e
dnx_tunnel_terminator_ingress_qos_model_get(
    int unit,
    int propagation_prof,
    bcm_tunnel_terminator_t * info)
{
    dnx_qos_propagation_type_e phb_propagation_type;
    dnx_qos_propagation_type_e remark_propagation_type;
    dnx_qos_propagation_type_e ecn_propagation_type;
    dnx_qos_propagation_type_e ttl_propagation_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_attr_get
                    (unit, propagation_prof, &phb_propagation_type, &remark_propagation_type, &ecn_propagation_type,
                     &ttl_propagation_type));

    if (phb_propagation_type != DNX_QOS_PROPAGATION_TYPE_PIPE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, phb_propagation_type, &(info->ingress_qos_model.ingress_phb)));
    }

    if (remark_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, remark_propagation_type, &(info->ingress_qos_model.ingress_remark)));
    }

    if (ttl_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, ttl_propagation_type, &(info->ingress_qos_model.ingress_ttl)));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Read from IP tunnel termination in-LIF table and update info struct fields.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] local_inlif - Allocated Local-In-LIF.
 *   \param [in] tunnel_term_inlif_info - 
 *               Pointer to inlif fields struct
 *
 */
static shr_error_e
dnx_tunnel_terminator_in_lif_table_get(
    int unit,
    uint32 local_inlif,
    dnx_tunnel_term_inlif_info_t * tunnel_term_inlif_info)
{
    uint32 entry_handle_id;
    uint8 is_field_valid = FALSE;
    lif_table_mngr_inlif_info_t lif_table_mngr_inlif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tunnel_term_inlif_info, _SHR_E_PARAM, "tunnel_term_inlif_info");

    /** take handle to read from DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_IPvX_TUNNELS, &entry_handle_id));

    /**  Get tunnel inlif info from lif tble manager  */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                    (unit, _SHR_CORE_ALL, local_inlif, entry_handle_id, &lif_table_mngr_inlif_info));
    {
        uint32 dbal_field[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

        sal_memset(dbal_field, 0, sizeof(dbal_field));
        /** read from dbal */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, dbal_field));
        tunnel_term_inlif_info->global_inlif = dbal_field[0];
        sal_memset(dbal_field, 0, sizeof(dbal_field));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROFILE, INST_SINGLE, (uint32 *) dbal_field));
        tunnel_term_inlif_info->propagation_prof = (int) (dbal_field[0]);
        sal_memset(dbal_field, 0, sizeof(dbal_field));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, (uint32 *) dbal_field));
        tunnel_term_inlif_info->in_lif_profile = dbal_field[0];
        sal_memset(dbal_field, 0, sizeof(dbal_field));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ECN_MAPPING_PROFILE, INST_SINGLE, dbal_field));
        tunnel_term_inlif_info->ecn_mapping_profile = dbal_field[0];

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                        (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, &is_field_valid));
        if (is_field_valid == TRUE)
        {
            tunnel_term_inlif_info->flags |= BCM_TUNNEL_TERM_STAT_ENABLE;
        }
        else
        {
            tunnel_term_inlif_info->flags |= 0;
        }

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                        (unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, &is_field_valid));
        if (is_field_valid == TRUE)
        {
            dbal_enum_value_field_vtt_lif_service_type_e vtt_lif_service_type;

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE, &vtt_lif_service_type));

            if (vtt_lif_service_type == DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P)
            {
                tunnel_term_inlif_info->flags |= BCM_TUNNEL_TERM_CROSS_CONNECT;
            }
        }

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                        (unit, entry_handle_id, DBAL_FIELD_LIF_GENERIC_DATA_0, &is_field_valid));
        if (is_field_valid == TRUE)
        {
            tunnel_term_inlif_info->flags |= BCM_TUNNEL_TERM_WIDE;
        }

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, entry_handle_id, DBAL_FIELD_FODO, &is_field_valid, &tunnel_term_inlif_info->fodo));

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, &is_field_valid,
                         &tunnel_term_inlif_info->fodo_assignment_mode));

        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_optional_vrf_get
                        (unit, entry_handle_id, &tunnel_term_inlif_info->vrf));
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_optional_vsi_get
                        (unit, tunnel_term_inlif_info->fodo, &tunnel_term_inlif_info->default_vsi));
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
 * Add IP-Tunnel terminator object and set its properties.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] flags - Relevant flags.
 * \param [in] config_key - A pointer to the struct that holds
 *     information for the config keys.
 *     Fields description:
 *     * config_key->vrf - VRF of the packet to terminate,
 *       resulted from ETH-RIF object.
 *     * config_key->dip - DIP of the packet to terminate.
 *     * config_key->dip6 - DIP6 of the packet to terminate.
 *     * config_key->type - Tunnel type of the packet to
 *       terminate.
 *     * config_key->dst_port - Destination port of the packet
 *       to be terminated.
 * \param [in] config_action - A pointer to the struct that
 *     holds information for the config action.
 *     Fields description:
 *     * config_action->tunnel_class - user managed DIP index.
 *
 * \return:
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 *
 * write to IPV6_MP_TT_TCAM_BASIC dbal table
 */
int
bcm_dnx_tunnel_terminator_config_add(
    int unit,
    uint32 flags,
    bcm_tunnel_terminator_config_key_t * config_key,
    bcm_tunnel_terminator_config_action_t * config_action)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** VERIFICATION of input parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_tunnel_terminator_config_verify(unit, config_key, config_action));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_attribute_set
                    (unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, _shr_ip6_mask_length(config_key->dip6_mask)));
    dnx_tunnel_terminator_config_ipv6_tcam_table_key_set(unit, entry_handle_id, config_key);
    dnx_tunnel_terminator_config_ipv6_tcam_table_result_set(unit, entry_handle_id, config_action->tunnel_class);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get tunnel terminator config entry
 *
 * \param [in] unit - Relevant unit.
 * \param [in] config_key - A pointer to the struct that holds
 *     information for the config keys.
 *     Fields description:
 *     * config_key->vrf - VRF of the packet to terminate,
 *       resulted from ETH-RIF object.
 *     * config_key->dip - DIP of the packet to terminate.
 *     * config_key->dip6 - DIP6 of the packet to terminate.
 *     * config_key->type - Tunnel type of the packet to
 *       terminate.
 *     * config_key->dst_port - Destination port of the packet
 *       to be terminated.
 *
 * \param [out] config_action - A pointer to the struct that
 *     holds information for the config action.
 *     Fields description:
 *     * config_action->tunnel_class - user managed DIP index.
 *
 * get entry from IPV6_MP_TT_TCAM_BASIC dbal table
 */
int
bcm_dnx_tunnel_terminator_config_get(
    int unit,
    bcm_tunnel_terminator_config_key_t * config_key,
    bcm_tunnel_terminator_config_action_t * config_action)
{
    uint32 entry_handle_id;
    uint32 entry_access_id;
    uint32 tunnel_class = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** VERIFICATION of input parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_tunnel_terminator_config_verify(unit, config_key, config_action));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_config_ipv6_tcam_table_key_set(unit, entry_handle_id, config_key));

    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP_IDX_DATA, INST_SINGLE, &tunnel_class);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    config_action->tunnel_class = tunnel_class;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete tunnel terminator config entry
 *
 * \param [in] unit - Relevant unit.
 * \param [in] config_key - A pointer to the struct that holds
 *     information for the config keys.
 *     Fields description:
 *     * config_key->vrf - VRF of the packet to terminate,
 *       resulted from ETH-RIF object.
 *     * config_key->dip - DIP of the packet to terminate.
 *     * config_key->dip6 - DIP6 of the packet to terminate.
 *     * config_key->type - Tunnel type of the packet to
 *       terminate.
 *     * config_key->dst_port - Destination port of the packet
 *       to be terminated.
 *
 * delete entry from IPV6_MP_TT_TCAM_BASIC dbal table
 */
int
bcm_dnx_tunnel_terminator_config_delete(
    int unit,
    bcm_tunnel_terminator_config_key_t * config_key)
{
    uint32 entry_handle_id;
    uint32 entry_access_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_tunnel_terminator_config_key_verify(unit, config_key));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_config_ipv6_tcam_table_key_set(unit, entry_handle_id, config_key));

    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

    /** Delete entry from DBAL table*/
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get fodo assignment mode according to tunnel type.
 * \see bcm_vxlan_port_add, for VXLAN, this api is responsible
 * for updating the fodo assignment mode
 */
void
dnx_tunnel_term_fodo_assignment_mode_get(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 *fodo_assignment_mode)
{
    /**
     * Update fodo assignment mode for vxlan gpe.
     */
    if (DNX_TUNNEL_TYPE_IS_VXLAN_OR_VXLAN_GPE_OR_GENEVE(tunnel_type))
    {
        *fodo_assignment_mode = DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP;
    }
    else
    {
        *fodo_assignment_mode = DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF;
    }
}

static shr_error_e
dnx_tunnel_term_using_flow_app(
    int unit,
    tunnel_term_operation_e operation,
    bcm_tunnel_terminator_t * info,
    uint8 *use_flow_app)
{
    SHR_FUNC_INIT_VARS(unit);

    *use_flow_app = FALSE;

    if (!(dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_flow_enabled_for_legacy_applications)))
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_create_using_flow_app(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint8 *use_flow_app)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_term_using_flow_app(unit, TUNNEL_TERM_OPERATION_CREATE, info, use_flow_app));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Create IP-Tunnel terminator object and set some of its properties.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] info -
 *     A pointer to the struct that holds information for the IPv4/IPv6 tunnel terminator entry.
 *     Fields description:
 *     * info-flags:
 *          * BCM_TUNNEL_TERM_TUNNEL_WITH_ID - create a tunnel terminator with Global LIF ID passed by tunnel->tunnel_id.
 *          * BCM_TUNNEL_REPLACE - replace an already allocated tunnel terminator entry.
 *             The flag can be used only with the BCM_TUNNEL_WITH_ID flag and a valid tunnel_id.
 *          * BCM_TUNNEL_TERM_CROSS_CONNECT- Create a P2P lif tunnel.
 *             The flag can be used only with the Srv6 tunnels only
 *          * BCM_TUNNEL_TERM_EXTENDED_TERMINATION- Lif is the last tunnel in 1st pass of an extended termination flow
 *             The flag can be used only with the Srv6 tunnels only
 *     * info->vrf - VRF of the packet to terminate, resulted from ETH-RIF object.
 *     * info->sip - IPV4 SIP of the packet to terminate.
 *     * info->dip - IPV4 DIP of the packet to terminate.
 *     * info->sip_mask - Mask to be applied on the IPV4 SIP.
 *     * info->dip_mask - Mask to be applied on the IPV4 DIP.
 *     * info->sip6 - IPV6 SIP of the packet to terminate.
 *     * info->dip6 - IPV6 DIP of the packet to terminate.
 *     * info->dip6_mask - Mask to be applied on the IPV6 DIP.
 *     * info->type - Tunnel type of the packet to terminate.
 *          * bcmTunnelTypeIpAnyIn4 - used for all IPv4 tunnels.
 *          * bcmTunnelTypeGreAnyIn4 - used for all GRe4oIPv4 tunnels.
 *          * bcmTunnelTypeUdp - used for UDPoIPv4 tunnels.
 *          * bcmTunnelTypeIpAnyIn6 - used for all IPv6 tunnels.
 *          * bcmTunnelTypeGreAnyIn6 - used for all GRe4oIPv6 tunnels.
 *          * bcmTunnelTypeUdp6 - used for UDPoIPv6 tunnels.
 *          * bcmTunnelTypeVxlan - used for VxlanoUdpoIpv4 tunnels.
 *          * bcmTunnelTypeVxlan6 - used for VxlanoUdpoIpv6 tunnels.
 *          * bcmTunnelTypeVxlanGpe - used for Vxlan-GpeoUdpoIpv4 tunnels.
 *          * bcmTunnelTypeVxlanGpe6 - used for Vxlan-GPEoUdpoIpv6 tunnels.
 *          * bcmTunnelTypeGeneve - used for GENEVEoUdpoIpv4 tunnels
 *          * bcmTunnelTypeGeneve6 - used for GENEVEoUdpoIpv6 tunnels
 *     * info->tunnel_id - As output - global if id of the created tunnel.
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
int
bcm_dnx_tunnel_terminator_create(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    int global_lif = 0;
    dnx_qos_propagation_type_e ttl_propagation_type;
    dnx_qos_propagation_type_e phb_propagation_type;
    dnx_qos_propagation_type_e remark_propagation_type;
    dnx_qos_propagation_type_e ecn_propagation_type;
    int propag_profile, old_propag_profile;
    uint8 qos_propagation_prof_first_ref;
    uint8 qos_propagation_prof_last_ref;
    uint32 fodo_assignment_mode;
    uint32 local_inlif;
    uint32 ecn_mapping_profile;
    uint32 vrf = 0;
    uint32 flags = 0;
    dnx_in_lif_profile_last_info_t last_profile;
    in_lif_profile_info_t in_lif_profile_info;
    int new_in_lif_profile = 0, old_in_lif_profile = 0;
    uint8 use_flow_app = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    dnx_in_lif_profile_last_info_t_init(&last_profile);
    /** VERIFICATION of input parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_tunnel_terminator_create_verify(unit, info));

    SHR_IF_ERR_EXIT(dnx_tunnel_term_create_using_flow_app(unit, info, &use_flow_app));
    if (use_flow_app)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));

    /** Set DSCP model (remark model) */
    phb_propagation_type = DNX_QOS_PROPAGATION_TYPE_PIPE;
    remark_propagation_type = ecn_propagation_type = ttl_propagation_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;

    if (info->ingress_qos_model.ingress_phb != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, info->ingress_qos_model.ingress_phb, &phb_propagation_type));
    }

    if (info->ingress_qos_model.ingress_remark != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, info->ingress_qos_model.ingress_remark, &remark_propagation_type));
    }

    if (info->ingress_qos_model.ingress_ttl != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, info->ingress_qos_model.ingress_ttl, &ttl_propagation_type));
    }

    if (info->ingress_qos_model.ingress_ecn == bcmQosIngressEcnModelEligible)
    {
        ecn_propagation_type = DNX_QOS_PROPAGATION_TYPE_PIPE;
    }

    /**
     * Get fodo assignment mode according to tunnel type
     */
    dnx_tunnel_term_fodo_assignment_mode_get(unit, info->type, &fodo_assignment_mode);

    /** Allocate LIF and set its properties */
    if (!_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_REPLACE))
    {
        uint32 lif_alloc_flags = 0;

        /** allocate global lif  */
        if (info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID)
        {
            lif_alloc_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
            global_lif = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
        }

        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                        (unit, lif_alloc_flags, DNX_ALGO_LIF_INGRESS, NULL, &global_lif));

        BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id, global_lif);

        /** Alloc propagation profile ID corresponding to chosen QOS model */
        old_propag_profile = DNX_QOS_INGRESS_DEFAULT_PROPAGATION_PROFILE;
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_alloc(unit, phb_propagation_type,
                                                                  remark_propagation_type, ecn_propagation_type,
                                                                  ttl_propagation_type, old_propag_profile,
                                                                  &propag_profile,
                                                                  &qos_propagation_prof_first_ref,
                                                                  &qos_propagation_prof_last_ref));
        ecn_mapping_profile = DNX_QOS_ECN_MAPPING_PROFILE_DEFAULT;
        if (info->ingress_qos_model.ingress_ecn == bcmQosIngressEcnModelEligible)
        {
            ecn_mapping_profile = DNX_QOS_ECN_MAPPING_PROFILE_ELIGIBLE;
        }

        /*
         * Update H/W
         */
        if (qos_propagation_prof_first_ref == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_set(unit, propag_profile, phb_propagation_type,
                                                                       remark_propagation_type,
                                                                       ecn_propagation_type, ttl_propagation_type));
        }

        /*
         * allocate inlif profile
         */
        if (info->type == bcmTunnelTypeVxlanEsp)
        {
            in_lif_profile_info.ingress_fields.cs_in_lif_profile_flags =
                DNX_IN_LIF_PROFILE_CS_PROFILE_LIF_VXLAN_OVER_ESP;
            in_lif_profile_info.ingress_fields.lif_gport = info->tunnel_id;
        }
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_optional_in_lif_profile_info_set(unit, info, &in_lif_profile_info));

        SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange(unit, &in_lif_profile_info, old_in_lif_profile,
                                                    &new_in_lif_profile, LIF, DBAL_TABLE_IN_LIF_IPvX_TUNNELS,
                                                    &last_profile));

        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_allocate_lif_and_in_lif_table_set
                        (unit, info, propag_profile, fodo_assignment_mode, (uint32) new_in_lif_profile, global_lif,
                         ecn_mapping_profile, vrf, 0, &local_inlif));
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_add(unit, info, local_inlif));

        if (DNX_TUNNEL_TERM_SRV6_IS_EXTENDED_TERMINATION(info))
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_lif_reclassify(unit, global_lif, local_inlif));
        }

        if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_LOCATOR(info))
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_gport_to_dip_idx_type_add(unit, info));
        }
    }
    /** Update allocated LIF properties */
    else
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        uint32 fodo;
        bcm_vlan_t default_vsi;
        dnx_tunnel_term_inlif_info_t tunnel_term_inlif_info;

        sal_memset(&tunnel_term_inlif_info, 0, sizeof(dnx_tunnel_term_inlif_info_t));

        /** Get Local-in-LIF ID */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, info->tunnel_id,
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));
        local_inlif = (uint32) gport_hw_resources.local_in_lif;

        /** Get optional vrf and vsi */
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_get
                        (unit, gport_hw_resources.local_in_lif, &tunnel_term_inlif_info));
        global_lif = tunnel_term_inlif_info.global_inlif;
        propag_profile = tunnel_term_inlif_info.propagation_prof;
        old_in_lif_profile = tunnel_term_inlif_info.in_lif_profile;
        flags = tunnel_term_inlif_info.flags;
        fodo = tunnel_term_inlif_info.fodo;
        default_vsi = tunnel_term_inlif_info.default_vsi;
        vrf = tunnel_term_inlif_info.vrf;

        /** verify that tunnel type does not change from vxlan type to another or vice-versa */
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_tunnel_vxlan_geneve_type_verify(unit, info,
                                                                              tunnel_term_inlif_info.fodo_assignment_mode));

        /** Get in_lif_profile_info */
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data
                        (unit, old_in_lif_profile, &in_lif_profile_info, DBAL_TABLE_IN_LIF_IPvX_TUNNELS, LIF));
        if (DNX_TUNNEL_TERM_SRV6_NEED_IN_LIF_PROFILE_UPDATE(info))
        {
            dnx_tunnel_terminator_srv6_optional_in_lif_profile_update(info, &in_lif_profile_info);
        }
        else if (info->type == bcmTunnelTypeVxlanEsp)
        {
            in_lif_profile_info.ingress_fields.cs_in_lif_profile_flags =
                DNX_IN_LIF_PROFILE_CS_PROFILE_LIF_VXLAN_OVER_ESP;
        }

        SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                        (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                         DBAL_TABLE_IN_LIF_IPvX_TUNNELS, &last_profile));

        /** Verify that the type of the l2vpn is not aimed to be replaced */
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_l2vpn_replace_type_verify(unit, info, fodo));

        /** Cannot replace LIF with different Cross-connect behavior */
        if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_CROSS_CONNECT) !=
            _SHR_IS_FLAG_SET(flags, BCM_TUNNEL_TERM_CROSS_CONNECT))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Replacing a tunnel cannot replace the Cross-connect behavior. Exist = %d, New = %d",
                         _SHR_IS_FLAG_SET(flags, BCM_TUNNEL_TERM_CROSS_CONNECT) ? 1 : 0, _SHR_IS_FLAG_SET(info->flags,
                                                                                                          BCM_TUNNEL_TERM_CROSS_CONNECT)
                         ? 1 : 0);
        }

        /** in some case, global inlif is 0, and should get it from sw state*/
        if (global_lif == 0)
        {
            bcm_gport_t gport;

            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                            (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS, _SHR_CORE_ALL, local_inlif,
                             &gport));
            global_lif = BCM_GPORT_TUNNEL_ID_GET(gport);
        }

        old_propag_profile = propag_profile;

        /** Update propagation profile ID.
         *  tunnel_term REPLACE can only update the DSCP model and TTL model LIF attributes. */
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_alloc
                        (unit, phb_propagation_type, remark_propagation_type,
                         ecn_propagation_type, ttl_propagation_type, old_propag_profile, &propag_profile,
                         &qos_propagation_prof_first_ref, &qos_propagation_prof_last_ref));

        /** Update the profile table if it is first used*/
        if (qos_propagation_prof_first_ref == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_set(unit, propag_profile, phb_propagation_type,
                                                                       remark_propagation_type,
                                                                       ecn_propagation_type, ttl_propagation_type));
        }

        ecn_mapping_profile = DNX_QOS_ECN_MAPPING_PROFILE_DEFAULT;
        if (info->ingress_qos_model.ingress_ecn == bcmQosIngressEcnModelEligible)
        {
            ecn_mapping_profile = DNX_QOS_ECN_MAPPING_PROFILE_ELIGIBLE;
        }

        /** Update LIF attributes */
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_allocate_lif_and_in_lif_table_set
                        (unit, info, propag_profile, fodo_assignment_mode, new_in_lif_profile, global_lif,
                         ecn_mapping_profile, vrf, default_vsi, &local_inlif));

        /** Clear the unused profile which is indicated by last-referrence*/
        if (qos_propagation_prof_last_ref == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_clear(unit, old_propag_profile));
        }
    }

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_destroy(unit, last_profile, LIF));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_get_using_flow_app(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint8 *use_flow_app)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_tunnel_term_using_flow_app(unit, TUNNEL_TERM_OPERATION_GET, info, use_flow_app));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get IP-Tunnel terminator object.
 *
 * \par DIRECT INPUT
 *    \param [in] unit -
 *     Relevant unit.
 *     \param [in] info -
 *     A pointer to the struct that holds information for the IPv4 tunnel terminator entry.
 *     Fields description:
 *     * info-flags:
 *          * BCM_TUNNEL_TERM_TUNNEL_WITH_ID - create a tunnel terminator with Global LIF ID passed by tunnel->tunnel_id.
 *          * BCM_TUNNEL_REPLACE - replace an already allocated tunnel terminator entry.
 *             The flag can be used only with the BCM_TUNNEL_WITH_ID flag and a valid tunnel_id.
 *     * info->vrf - VRF of the packet to terminate, resulted from ETH-RIF object.
 *     * info->sip - IPV4 SIP of the packet to terminate.
 *     * info->dip - IPV4 DIP of the packet to terminate.
 *     * info->sip_mask - Mask to be applied on the IPV4 SIP.
 *     * info->dip_mask - Mask to be applied on the IPV4 DIP.
 *     * info->sip6 - IPV6 SIP of the packet to terminate.
 *     * info->dip6 - IPV6 DIP of the packet to terminate.
 *     * info->dip6_mask - Mask to be applied on the IPV6 DIP.
 *     * info->type - Tunnel type of the packet to terminate.
 *          * bcmTunnelTypeIpAnyIn4 - used for all IPv4 tunnels.
 *          * bcmTunnelTypeGreAnyIn4 - used for all GRe4oIPv4 tunnels.
 *          * bcmTunnelTypeUdp - used for UDPoIPv4 tunnels.
 *          * bcmTunnelTypeIpAnyIn6 - used for all IPv6 tunnels.
 *          * bcmTunnelTypeGreAnyIn6 - used for all GRe4oIPv6 tunnels.
 *          * bcmTunnelTypeUdp6 - used for UDPoIPv6 tunnels.
 *          * bcmTunnelTypeVxlan - used for VxlanoUdpoIpv4 tunnels.
 *          * bcmTunnelTypeVxlan6 - used for VxlanoUdpoIpv6 tunnels.
 *          * bcmTunnelTypeVxlanGpe - used for Vxlan-GpeoUdpoIpv4 tunnels.
 *          * bcmTunnelTypeVxlanGpe6 - used for Vxlan-GPEoUdpoIpv6 tunnels.
 *          * bcmTunnelTypeGeneve - used for GENEVEoUdpoIpv4 tunnels
 *          * bcmTunnelTypeGeneve6 - used for GENEVEoUdpoIpv6 tunnels
 * \par DIRECT OUTPUT:
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 * \par INDIRECT OUTPUT
 *   * info->tunnel_id - See 'info->tunnel_id' in DIRECT INPUT above.
 */
int
bcm_dnx_tunnel_terminator_get(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    uint32 local_inlif;
    int propagation_prof;
    uint32 global_inlif;
    uint32 ecn_mapping_profile;
    uint32 fodo = 0;
    dnx_tunnel_term_inlif_info_t tunnel_term_inlif_info;
    uint8 use_flow_app;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    sal_memset(&tunnel_term_inlif_info, 0, sizeof(dnx_tunnel_term_inlif_info_t));

    SHR_IF_ERR_EXIT(dnx_tunnel_term_get_using_flow_app(unit, info, &use_flow_app));
    if (use_flow_app)
    {
        SHR_EXIT();
    }

    /** VERIFICATION of input parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_tunnel_terminator_get_delete_verify(unit, info));

    /** Get entry from ISEM to find local lif id */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_get(unit, info, &local_inlif));

    /** Get entry from IP tunnel in-LIF table */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_get(unit, local_inlif, &tunnel_term_inlif_info));
    global_inlif = tunnel_term_inlif_info.global_inlif;
    propagation_prof = tunnel_term_inlif_info.propagation_prof;
    ecn_mapping_profile = tunnel_term_inlif_info.ecn_mapping_profile;
    info->flags |= tunnel_term_inlif_info.flags;
    fodo = tunnel_term_inlif_info.fodo;

    /** VSI could be set only with tunnel types of bcmTunnelTypeEthIn6 and bcmTunnelTypeEthCascadedFunct*/
    if (DNX_TUNNEL_TERM_SRV6_IS_L2_VPN(info->type))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_optional_vsi_get(unit, fodo, &(info->vlan)));
    }

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_optional_flags_get
                    (unit, info, global_inlif, tunnel_term_inlif_info.in_lif_profile, &(info->flags)));

    if (DNX_TUNNEL_TYPE_IS_VXLAN_GPE(info->type) || DNX_TUNNEL_TYPE_IS_GENEVE(info->type))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_optional_default_vrf_get(unit, fodo, &(info->default_vrf)));
    }
    /** Set tunnel_id */
    BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id, global_inlif);

    if (ecn_mapping_profile == DNX_QOS_ECN_MAPPING_PROFILE_ELIGIBLE)
    {
        info->ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelEligible;
    }
    else
    {
        info->ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelInvalid;
    }

    /** get qos model according to returned propagation_prof */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ingress_qos_model_get(unit, propagation_prof, info));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_delete_using_flow_app(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint8 *use_flow_app)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_term_using_flow_app(unit, TUNNEL_TERM_OPERATION_DELETE, info, use_flow_app));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete IP-Tunnel terminator object.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] info -
 *     A pointer to the struct that holds information for the
 *     IPv4/IPv6 tunnel terminator entry. The struct parameters
 *     are part of the VTT look-up key and re used to get the
 *     local-in-LIF. Fields description:
 *     * info->vrf - VRF of the IP header to terminate, property of the ETH-RIF.
 *     * info->sip - SIP of the tunnel header to terminate.
 *     * info->dip - DIP of the tunnel header to terminate.
 *     * info->type - Tunnel type of the tunnel header to terminate.
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
int
bcm_dnx_tunnel_terminator_delete(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    uint32 local_inlif;
    uint32 global_inlif;
    int propagation_prof, new_in_lif_profile;
    uint32 in_lif_profile;
    dnx_tunnel_term_inlif_info_t tunnel_term_inlif_info;
    in_lif_profile_info_t in_lif_profile_info;
    uint8 use_flow_app = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    sal_memset(&tunnel_term_inlif_info, 0, sizeof(dnx_tunnel_term_inlif_info_t));

    SHR_IF_ERR_EXIT(dnx_tunnel_term_delete_using_flow_app(unit, info, &use_flow_app));
    if (use_flow_app)
    {
        SHR_EXIT();
    }

    new_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    /** VERIFICATION of input parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_tunnel_terminator_get_delete_verify(unit, info));

    /** Get entry from ISEM to find local lif id */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_get(unit, info, &local_inlif));

    /** Get entry from IP tunnel in-LIF table */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_get(unit, local_inlif, &tunnel_term_inlif_info));
    global_inlif = tunnel_term_inlif_info.global_inlif;
    propagation_prof = tunnel_term_inlif_info.propagation_prof;
    in_lif_profile = tunnel_term_inlif_info.in_lif_profile;

    if (global_inlif == 0)
    {
        bcm_gport_t gport;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                        (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS, _SHR_CORE_ALL, local_inlif,
                         &gport));
        global_inlif = BCM_GPORT_TUNNEL_ID_GET(gport);
    }

    /** Delete lookup entry from ISEM */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_delete(unit, info));

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_optional_delete(unit, info, global_inlif));

    /** Free propagation profile template */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_free_and_hw_clear(unit, propagation_prof, NULL));

    /** Delete entry from IP tunnel in-LIF table.
     *  Delete local in LIF */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_inlif_info_clear
                    (unit, _SHR_CORE_ALL, local_inlif, DBAL_TABLE_IN_LIF_IPvX_TUNNELS, 0));
    /** Delete global in LIF */
    dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_INGRESS, global_inlif);

    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_dealloc(unit, in_lif_profile, &new_in_lif_profile, &in_lif_profile_info, LIF));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

static void
dnx_tunnel_terminator_ipv6_tcam_table_result_is_lif(
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type,
    uint8 *result_is_lif)
{
    *result_is_lif = UTILEX_NUM2BOOL(dip_idx_type == DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_LIF_ELIGIBLE);
}

/**
 * \brief
 * indicate if the ipv6 tcam result is encoded as a lif:
 * when it's explicitly LIF or when it's encoded for SRV6
 */
static shr_error_e
dnx_tunnel_terminator_ipv6_tcam_result_optional_lif(
    int unit,
    uint32 entry_handle_id,
    uint8 *result_is_local_inlif,
    uint32 *local_inlif)
{

    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type;
    uint8 srv6_result_is_local_inlif;
    uint8 result_is_local_inlif_internal;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_tcam_result_get(unit, entry_handle_id, &dip_idx_type, local_inlif));
    dnx_tunnel_terminator_srv6_ipv6_tcam_table_result_is_lif(dip_idx_type, &srv6_result_is_local_inlif);
    dnx_tunnel_terminator_ipv6_tcam_table_result_is_lif(dip_idx_type, &result_is_local_inlif_internal);
    *result_is_local_inlif = (srv6_result_is_local_inlif || result_is_local_inlif_internal);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_terminator_traverse_ipv6_p2p_sip6_get(
    int unit,
    uint32 entry_handle_id,
    bcm_ip6_t * sip6)
{
    int ref_count;
    uint32 sip_profile_id;
    bcm_ip6_t sip6_msbs;
    bcm_ip6_t sip6_lsbs;
    uint32 sip6_long[UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S];
    uint32 sip6_lsbs_long[UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S];

    SHR_FUNC_INIT_VARS(unit);

    /** get sip6 msbs */
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_IPV6_ADDRESS_MSBS_ID, &sip_profile_id));
    SHR_IF_ERR_EXIT(algo_tunnel_db.tunnel_ipv6_sip_profile.profile_data_get(unit, sip_profile_id,
                                                                            &ref_count, (void *) &sip6_msbs));
    SHR_IF_ERR_EXIT(utilex_pp_ipv6_address_struct_to_long(sip6_msbs, sip6_long));

    /*
     * get sip6 lsbs 
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_IPV6, sip6_lsbs));
    SHR_IF_ERR_EXIT(utilex_pp_ipv6_address_struct_to_long(sip6_lsbs, sip6_lsbs_long));

    /** add sip6 lsbs to sip6 msbs */
    SHR_IF_ERR_EXIT(utilex_bitstream_or(sip6_long, sip6_lsbs_long, UTILEX_PP_IPV6_ADDRESS_NOF_UINT32S));
    SHR_IF_ERR_EXIT(utilex_pp_ipv6_address_long_to_struct(sip6_long, *sip6));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
bcm_dnx_tunnel_terminator_ipv4_traverse_key_fields(
    int unit,
    uint32 entry_handle_id,
    uint32 traversed_table,
    bcm_tunnel_terminator_t * info)
{
    uint32 vrf_value;
    uint32 ipv4_tunnel_type_qualifier = 0;
    uint32 entry_access_id, tcam_handler_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VRF, &vrf_value));
    info->vrf = vrf_value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_IPV4_TUNNEL_TYPE, &ipv4_tunnel_type_qualifier));
    SHR_IF_ERR_EXIT(dnx_ipv4_layer_qualifier_tunnel_type_to_tunnel_terminator_type
                    (unit, ipv4_tunnel_type_qualifier, &info->type, &info->udp_dst_port));

    if ((traversed_table == DBAL_TABLE_IPV4_TT_TCAM_BASIC) || (traversed_table == DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS))
    {
        /** handle coverity issues */
        uint32 dip_mask_arr[1] = { 0 };
        uint32 sip_mask_arr[1] = { 0 };

        /** TCAM access - get the DIP/SIP and masks */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, &info->dip, dip_mask_arr));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, &info->sip, sip_mask_arr));
        info->dip_mask = dip_mask_arr[0];
        info->sip_mask = sip_mask_arr[0];

        SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, traversed_table, &tcam_handler_id));
        SHR_IF_ERR_EXIT_NO_MSG(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_priority_get
                        (unit, tcam_handler_id, entry_access_id, (uint32 *) &(info->priority)));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, &info->dip));
        info->dip_mask = DNX_TUNNEL_TERM_EM_IP_MASK;
        info->sip_mask = 0;
        if ((traversed_table == DBAL_TABLE_IPV4_TT_P2P_EM_BASIC)
            || (traversed_table == DBAL_TABLE_IPV4_TT_P2P_EM_2ND_PASS))
        {   /** P2P */
            info->sip_mask = DNX_TUNNEL_TERM_EM_IP_MASK;
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, &info->sip));
        }
    }
    if ((traversed_table == DBAL_TABLE_IPV4_TT_MP_EM_2ND_PASS) ||
        (traversed_table == DBAL_TABLE_IPV4_TT_P2P_EM_2ND_PASS) ||
        (traversed_table == DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS))
    {
        info->flags |= BCM_TUNNEL_TERM_BUD;
    }

exit:
    SHR_FUNC_EXIT;
}

/* \brief
 * MP TCAM tunnel result can be a local lif id or a user defined value.
 * In case it's a local lif id, info.tunnel_class is a gport.
 * \param [in] unit - the unit number.
 * \param [in] tunnel_class_from_p2p_tunnel
 *          tunnel class from P2P tunnel entry
 * \param [in] tunnel_class_gport -
 *          gport tunnel lif
 * \param [in] tunnel_class_is_gport -
 *          indicate if the tunnel class gport is valid
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
static shr_error_e
dnx_tunnel_terminator_traverse_tunnel_type_and_optional_tunnel_class_as_gport(
    int unit,
    uint32 tunnel_class_from_p2p_tunnel,
    bcm_tunnel_type_t * tunnel_type,
    bcm_gport_t * tunnel_class_gport,
    uint8 *tunnel_class_is_gport)
{
    uint32 entry_handle_id, local_inlif;
    int is_end_ipv6;
    uint8 result_is_local_inlif = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *tunnel_class_is_gport = FALSE;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end_ipv6));
    while (!is_end_ipv6)
    {
        result_is_local_inlif = FALSE;
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_tcam_result_optional_lif
                        (unit, entry_handle_id, &result_is_local_inlif, &local_inlif));

        /** Get information about the global inlif and convert it to tunnel gport */
        if (local_inlif == tunnel_class_from_p2p_tunnel)
        {
            uint16 dummy_port = 0;

            *tunnel_class_is_gport = result_is_local_inlif;
            if (result_is_local_inlif)
            {
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS, _SHR_CORE_ALL,
                                 tunnel_class_from_p2p_tunnel, tunnel_class_gport));
            }

            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_tcam_key_field_tunnel_type
                            (unit, entry_handle_id, tunnel_type, &dummy_port));
            break;
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end_ipv6));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* MP TCAM tunnel result can be a local lif id or a user defined value.
 * In case it's a local lif id, info.tunnel_class is a gport.
 * Otherwise info.tunnel_class is the user defined value
 */
static shr_error_e
dnx_tunnel_terminator_traverse_tunnel_class_and_tunnel_type_get(
    int unit,
    uint32 entry_handle_id,
    bcm_gport_t * tunnel_class,
    bcm_tunnel_type_t * type)
{
    uint32 tunnel_class_from_p2p;
    bcm_gport_t tunnel_class_gport;
    uint8 tunnel_class_is_gport;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_IPV6_MP_TUNNEL_IDX, &tunnel_class_from_p2p));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_traverse_tunnel_type_and_optional_tunnel_class_as_gport
                    (unit, tunnel_class_from_p2p, type, &tunnel_class_gport, &tunnel_class_is_gport));
    *tunnel_class = (tunnel_class_is_gport ? tunnel_class_gport : tunnel_class_from_p2p);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_traverse_p2p_key_fields(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    info->flags = BCM_TUNNEL_TERM_USE_TUNNEL_CLASS;
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_traverse_ipv6_p2p_sip6_get(unit, entry_handle_id, &info->sip6));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_traverse_tunnel_class_and_tunnel_type_get
                    (unit, entry_handle_id, &info->tunnel_class, &info->type));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_traverse_key_fields(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e dbal_table,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dbal_table == DBAL_TABLE_IPV6_TT_P2P_EM_BASIC)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_term_traverse_p2p_key_fields(unit, entry_handle_id, info));
    }
    else if ((dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS) || (dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC))
    {
        uint16 udp_dst_port = 0;
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_tcam_key_fields
                        (unit, entry_handle_id, &info->dip6, &info->dip6_mask, &info->vrf, &info->type, &udp_dst_port));
        info->udp_dst_port = udp_dst_port;
    }
    else if (dbal_table == DBAL_TABLE_IPVX_TT_IPSEC)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_term_txsci_traverse_key_fields(unit, entry_handle_id, info));
    }
    else if (dbal_table == DBAL_TABLE_IPV6_TT_MP_EM_16)
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_traverse_funct_key_fields(unit, entry_handle_id, info));
    }
    /** Get info about IPV4 ingress tunnels */
    else
    {
        SHR_IF_ERR_EXIT(bcm_dnx_tunnel_terminator_ipv4_traverse_key_fields(unit, entry_handle_id, dbal_table, info));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_term_traverse_local_lif_id_get(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e dbal_table,
    uint32 *local_inlif,
    uint8 *is_local_inlif)
{
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    SHR_FUNC_INIT_VARS(unit);

    if ((dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS) || (dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_tcam_result_optional_lif
                        (unit, entry_handle_id, is_local_inlif, local_inlif));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, field_value));
        *local_inlif = field_value[0];
        *is_local_inlif = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tunnel_terminator_traverse_srv6_from_lif(
    int unit,
    dnx_tunnel_term_inlif_info_t * tunnel_term_inlif_info,
    dbal_tables_e dbal_table,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    /**If we have set FODO VSI on the tunnel, update the type to:
     * bcmTunnelTypeEthIn6 or bcmTunnelTypeCascadedFunct*/
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_l2vpn_type_get(unit, info, tunnel_term_inlif_info->fodo, &info->type));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_optional_vsi_get
                    (unit, tunnel_term_inlif_info->fodo, &info->vlan));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_optional_flags_get
                    (unit, info, tunnel_term_inlif_info->global_inlif, tunnel_term_inlif_info->in_lif_profile,
                     &info->flags));

    if ((dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS) || (dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC))
    {
        /** get sid format flag from global lif */
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_srv6_optional_dip_idx_type_flag_get
                        (unit, tunnel_term_inlif_info->global_inlif, &info->flags));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse all IP-Tunnel terminator objects.
 * \param [in] unit - the unit number.
 * \param [in] cb - the name of the callback function,
 *          it receives the value of the user_data variable and
 *          the key and result values for each found entry.
 * \param [in] user_data - user data that will be sent to the callback function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  *
 */
shr_error_e
bcm_dnx_tunnel_terminator_traverse(
    int unit,
    bcm_tunnel_terminator_traverse_cb cb,
    void *user_data)
{

    uint32 entry_handle_id;
    uint32 core_id = DBAL_CORE_DEFAULT;
    /*
     * Cascaded table order is important: cascaded tables must be accessed before tables they are cascaded from:
     * cascaded tables means they use other tables result as part of its key. If traverse is used to delete tables, We
     * first need to delete the cascaded table and then the table its cascaded from. 
     */
    dbal_tables_e dbal_tables[] = {
        /*
         * ipv4 tables 
         */
        DBAL_TABLE_IPV4_TT_P2P_EM_2ND_PASS, DBAL_TABLE_IPV4_TT_P2P_EM_BASIC, DBAL_TABLE_IPV4_TT_MP_EM_2ND_PASS,
        DBAL_TABLE_IPV4_TT_MP_EM_BASIC, DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS, DBAL_TABLE_IPV4_TT_TCAM_BASIC,
        /*
         * ipv6 cascaded tables 
         */
        DBAL_TABLE_IPV6_TT_MP_EM_16,
        /*
         * ipv6 tables 
         */
        DBAL_TABLE_IPV6_TT_P2P_EM_BASIC, DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC,
        DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS,
        /*
         * ipsec tables 
         */
        DBAL_TABLE_IPVX_TT_IPSEC
    };
    uint32 nof_tables = sizeof(dbal_tables) / sizeof(dbal_tables[0]);
    uint32 table, local_inlif, global_inlif;
    int is_end;
    bcm_tunnel_terminator_t info;
    dbal_core_mode_e core_mode;
    uint8 result_is_local_inlif = FALSE;
    dbal_tables_e dbal_table;
    dnx_tunnel_term_inlif_info_t tunnel_term_inlif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    if (dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_flow_enabled_for_legacy_applications))
    {
    }

    SHR_INVOKE_VERIFY_DNXC(dnx_tunnel_terminator_traverse_verify(unit, cb, user_data));

    /*
     * Iterate over all tables and all their entries
     */
    for (table = 0; table < nof_tables; table++)
    {
        dbal_table = dbal_tables[table];
        /*
         * Allocate handle to the table of the iteration and initialise an iterator entity.
         * The iterator is in mode ALL, which means that it will consider all entries regardless
         * of them being default entries or not.
         */
        if (table == 0)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table, entry_handle_id));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

        SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, dbal_table, &core_mode));
        if (core_mode == DBAL_CORE_MODE_DPC)
        {
            /** Add KEY rule to skip duplicated entry in core 1 */
            SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                                                   DBAL_CONDITION_EQUAL_TO, &core_id, NULL));
        }

        /*
         * Receive first entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            global_inlif = LIF_MNGR_INVALID;
            bcm_tunnel_terminator_t_init(&info);
            SHR_IF_ERR_EXIT(dnx_tunnel_term_traverse_key_fields(unit, entry_handle_id, dbal_table, &info));
            SHR_IF_ERR_EXIT(dnx_tunnel_term_traverse_local_lif_id_get
                            (unit, entry_handle_id, dbal_table, &local_inlif, &result_is_local_inlif));
            /** If the entry is not encoded with lif field,
             * it means it was added by bcm_tunnel_terminator_config_add API and is not a subject of bcm_tunnel_terminator_traverse*/
            if (!result_is_local_inlif)
            {
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                continue;
            }
            /** Get entry from IP tunnel in-LIF table */
            sal_memset(&tunnel_term_inlif_info, 0, sizeof(dnx_tunnel_term_inlif_info_t));
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_get(unit, local_inlif, &tunnel_term_inlif_info));
            global_inlif = tunnel_term_inlif_info.global_inlif;
            info.flags |= tunnel_term_inlif_info.flags;

            if (DNX_TUNNEL_TERM_SRV6_IS_SRV6_DBAL_TABLE(dbal_table))
            {
                SHR_IF_ERR_EXIT(dnx_tunnel_terminator_traverse_srv6_from_lif
                                (unit, &tunnel_term_inlif_info, dbal_table, &info));
            }

            /** get default vrf*/
            if (DNX_TUNNEL_TYPE_IS_VXLAN_GPE(info.type) || DNX_TUNNEL_TYPE_IS_GENEVE(info.type))
            {
                SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_optional_default_vrf_get
                                (unit, tunnel_term_inlif_info.fodo, &info.default_vrf));
            }

            if (dbal_table == DBAL_TABLE_IPVX_TT_IPSEC)
            {
                SHR_IF_ERR_EXIT(dnx_tunnel_term_txsci_traverse_type_get
                                (unit, global_inlif, tunnel_term_inlif_info.in_lif_profile, &info.type));
            }

            /** get qos model according to returned propagation_prof */
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ingress_qos_model_get
                            (unit, tunnel_term_inlif_info.propagation_prof, &info));

            if (tunnel_term_inlif_info.ecn_mapping_profile == DNX_QOS_ECN_MAPPING_PROFILE_ELIGIBLE)
            {
                info.ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelEligible;
            }
            else
            {
                info.ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelInvalid;
            }
            /** Set tunnel_id */
            if (global_inlif != LIF_MNGR_INVALID)
            {
                BCM_GPORT_TUNNEL_ID_SET(info.tunnel_id, global_inlif);
            }
            if (cb != NULL)
            {
                /*
                 * Invoke callback function
                 */
                SHR_IF_ERR_EXIT((*cb) (unit, &info, user_data));
            }
            /** Receive next entry in table.*/
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse all IP-Tunnel terminator config objects.
 * \param [in] unit - the unit number.
 * \param [in] additional_info - the additional info for tunnel terminator.
 * \param [in] cb - the name of the callback function,
 *          it receives the value of the user_data variable and
 *          the key and result values for each found entry.
 * \param [in] user_data - user data that will be sent to the callback function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  *
 */

shr_error_e
bcm_dnx_tunnel_terminator_config_traverse(
    int unit,
    bcm_tunnel_terminator_config_traverse_info_t additional_info,
    bcm_tunnel_terminator_config_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id, local_inlif;
    uint8 result_is_local_inlif;
    int is_end;
    bcm_tunnel_terminator_config_key_t config_key;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Allocate handle to the table of the iteration and initialise an iterator entity.
     * The iterator is in mode ALL, which means that it will consider all entries regardless
     * of them being default entries or not.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        result_is_local_inlif = FALSE;
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_tcam_key_fields
                        (unit, entry_handle_id, &config_key.dip6, &config_key.dip6_mask, &config_key.vrf,
                         &config_key.type, &config_key.udp_dst_port));
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_tcam_result_optional_lif
                        (unit, entry_handle_id, &result_is_local_inlif, &local_inlif));
        /*
         * used as tunnel_class 
         */
        if (!result_is_local_inlif)
        {
            if (cb != NULL)
            {
                /*
                 * Invoke callback function
                 */
                SHR_IF_ERR_EXIT((*cb) (unit, &config_key, user_data));
            }
        }
        /** Receive next entry in table.*/
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }
exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * End of APIs
 * }
 */
