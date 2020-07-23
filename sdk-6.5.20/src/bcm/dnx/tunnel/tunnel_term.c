/** \file tunnel_term.c
 *  General TUNNEL termination functionality for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL
/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/tunnel.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/algo/tunnel/algo_tunnel.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/qos/qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tunnel.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_tunnel_access.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/lif_mngr/algo_in_lif_profile.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
/*
 * }
 */

/*
 * MACROs and ENUMs
 * {
 */

/** mask indicating that exact match is requested by the user */
#define DNX_TUNNEL_TERM_EM_IP_MASK 0xFFFFFFFF
#define DNX_TUNNEL_TERM_LOCAL_IN_LIF_ENCODING 0x100000
#define DNX_TUNNEL_TERM_FLAGS_UNSUPPORTED \
 (BCM_TUNNEL_INIT_USE_INNER_DF         |   \
 BCM_TUNNEL_TERM_WLAN_REMOTE_TERMINATE |     BCM_TUNNEL_TERM_WLAN_SET_ROAM   | \
 BCM_TUNNEL_INIT_IPV4_SET_DF           |     BCM_TUNNEL_INIT_IPV6_SET_DF | \
 BCM_TUNNEL_INIT_IP4_ID_SET_FIXED      |   \
 BCM_TUNNEL_INIT_WLAN_MTU             |     BCM_TUNNEL_INIT_WLAN_FRAG_ENABLE | \
 BCM_TUNNEL_INIT_WLAN_VLAN_TAGGED     |     BCM_TUNNEL_INIT_WLAN_TUNNEL_WITH_ID | \
 BCM_TUNNEL_TERM_USE_OUTER_DSCP | BCM_TUNNEL_TERM_USE_OUTER_TTL | \
 BCM_TUNNEL_TERM_KEEP_INNER_DSCP)
#define DNX_TUNNEL_TERM_EM_L2TPV2_UDP_DST_PORT 1701
/*
 * The UDP destination port value in case of ESPoUDP
 */
#define DNX_TUNNEL_TERM_ESP_UDP_DST_PORT       500
#define DNX_TUNNEL_TERM_GPTV1U_UDP_DST_PORT    2152

#define DNX_TUNNEL_TERM_TYPE_IS_VXLAN_GPE(_tunnel_type) ((_tunnel_type == bcmTunnelTypeVxlanGpe) || (_tunnel_type == bcmTunnelTypeVxlan6Gpe))

/*
 * End of MACROs
 * }
 */

/*
 * Verify functions
 * {
 */

/*
 * return weather tunnel type is ipv6
 */
static void
dnx_tunnel_terminator_type_is_ipv6(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 *is_ipv6)
{
    switch (tunnel_type)
    {
        case bcmTunnelTypeIpAnyIn6:
        case bcmTunnelTypeGreAnyIn6:
        case bcmTunnelTypeVxlan6:
        case bcmTunnelTypeVxlan6Gpe:
        case bcmTunnelTypeUdp6:
        case bcmTunnelTypeSR6:
        case bcmTunnelTypeEsp6:
        case bcmTunnelTypeEthSR6:
        case bcmTunnelTypeEthIn6:
            *is_ipv6 = TRUE;
            break;
        default:
            *is_ipv6 = FALSE;
            break;
    }
}

static void
dnx_tunnel_terminator_type_is_supported_with_usid(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint8 *is_supported_with_usid)
{
    switch (tunnel_type)
    {
        case bcmTunnelTypeUdp6:
        case bcmTunnelTypeIpAnyIn6:
        case bcmTunnelTypeSR6:
        case bcmTunnelTypeEthSR6:
        case bcmTunnelTypeEthIn6:
            *is_supported_with_usid = TRUE;
            break;
        default:
            *is_supported_with_usid = FALSE;
            break;
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
    SHR_FUNC_INIT_VARS(unit);

    /*
     * verify that tunnel type is supported
     */
    switch (tunnel_type)
    {
            /*
             * This is the list of all supported tunnel types
             */
            /*
             * IPv6 supported tunnel types
             */
        case bcmTunnelTypeIpAnyIn6:
        case bcmTunnelTypeGreAnyIn6:
        case bcmTunnelTypeVxlan6:
        case bcmTunnelTypeVxlan6Gpe:
        case bcmTunnelTypeUdp6:
        case bcmTunnelTypeEsp6:
        case bcmTunnelTypeEthSR6:
        case bcmTunnelTypeEthIn6:
            break;
            /*
             * IPv4 supported tunnel types
             */
        case bcmTunnelTypeGreAnyIn4:
        case bcmTunnelTypeIpAnyIn4:
        case bcmTunnelTypeEsp:
        case bcmTunnelTypeUdp:
        case bcmTunnelTypeVxlan:
        case bcmTunnelTypeVxlanGpe:
        case bcmTunnelTypeSR6:
            break;
            

        case bcmTunnelTypeNone:
        {
            /*
             * for tunnel termination, tunnelType is mandatory
             */
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel termination type value (%d) is invalid, type is a required field",
                         tunnel_type);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel termination type value (%d) is not supported", tunnel_type);
        }
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

    if ((info->dip_mask == DNX_TUNNEL_TERM_EM_IP_MASK) && (info->sip_mask == DNX_TUNNEL_TERM_EM_IP_MASK))
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
    else if ((info->dip_mask == DNX_TUNNEL_TERM_EM_IP_MASK) && (info->sip_mask == 0))
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
 * Used by bcm_dnx_tunnel_terminator_config_ APIs to verify config_key
 * struct.
 */
static shr_error_e
dnx_tunnel_terminator_config_key_verify(
    int unit,
    bcm_tunnel_terminator_config_key_t * config_key)
{
    uint32 tunnel_is_ipv6;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(config_key, _SHR_E_PARAM, "config_key");

    /** Verify that tunnel type is IPv6 */
    dnx_tunnel_terminator_type_is_ipv6(unit, config_key->type, &tunnel_is_ipv6);
    if (!tunnel_is_ipv6)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "tunnel_termintor_config APIs support only IPv6 P2P tunnels! Given config_key->type is %d",
                     config_key->type);
    }

    /** Verify that tunnel type is supported */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_tunnel_type_verify(unit, config_key->type));

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
dnx_tunnel_terminator_create_vsi_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (info->vlan != 0)
    {
        if ((info->type != bcmTunnelTypeEthSR6) && (info->type != bcmTunnelTypeEthIn6))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "vlan is supported only for tunnel types bcmTunnelTypeEthSR6 and bcmTunnelTypeEthIn6");
        }
    }
    SHR_EXIT();
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

    if (DNX_TUNNEL_TERM_TYPE_IS_VXLAN_GPE(info->type))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_create_vrf_verify(unit, info->default_vrf, "default_vrf"));
    }

    if ((info->default_vrf != 0) && (!DNX_TUNNEL_TERM_TYPE_IS_VXLAN_GPE(info->type)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "default_vrf is supported only for tunnel type %d or %d", bcmTunnelTypeVxlanGpe,
                     bcmTunnelTypeVxlan6Gpe);

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
    uint32 tunnel_is_ipv6 = 0;
    uint32 table_id = 0;
    uint8 type_is_supported_with_usid;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    /** Verify that tunnel type is supported */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_tunnel_type_verify(unit, info->type));

    /** Verify VRF */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_create_vrf_verify(unit, info->vrf, "vrf"));

    /** check REPLACE flag, must be used with WITH_ID flag */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_REPLACE) &&
        !_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_TUNNEL_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_TUNNEL_REPLACE can't be used without BCM_TUNNEL_TERM_TUNNEL_WITH_ID as well");
    }
    /** check the usage of BCM_TUNNEL_TERM_ETHERNET flag */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_ETHERNET))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_TUNNEL_TERM_ETHERNET is used only on Arad+ device");
    }
    if (info->flags & DNX_TUNNEL_TERM_FLAGS_UNSUPPORTED)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "The used flag or combination of flags is not supported");
    }

    /** Verify P2P is used only for SRv6 tunnels or uSID where SRH may not be present */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_CROSS_CONNECT) && ((info->type != bcmTunnelTypeSR6) &&
                                                                         (info->type != bcmTunnelTypeEthIn6)
                                                                         && (info->type != bcmTunnelTypeEthSR6))
        && !_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_MICRO_SEGMENT_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_TUNNEL_TERM_CROSS_CONNECT is supported only with tunnel type %d and/or with flag BCM_TUNNEL_TERM_MICRO_SEGMENT_ID,"
                     "current tunnel type %d, and flags %d", bcmTunnelTypeSR6, info->type, info->flags);
    }

    if (((info->type == bcmTunnelTypeEsp) || (info->type == bcmTunnelTypeEsp6))
        && !dnx_data_switch.svtag.supported_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The bcmTunnelTypeEsp and the bcmTunnelTypeEsp6 are available for SVTAG supported devices ");
    }
    /** Verify Ext termination is used only for SRv6 tunnels*/
    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_EXTENDED_TERMINATION) && ((info->type != bcmTunnelTypeSR6) &&
                                                                                (info->type != bcmTunnelTypeEthIn6)
                                                                                && (info->type != bcmTunnelTypeEthSR6)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_TUNNEL_TERM_EXTENDED_TERMINATION is supported only with tunnel types bcmTunnelTypeSR6, bcmTunnelTypeEthIn6 or bcmTunnelTypeEthSR6 not with %d",
                     info->type);
    }

    /** tunnel LIF no longer points to RIF */
    if (info->tunnel_if != BCM_IF_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "tunnel_if parameter cannot be used to create RIF. Please use bcm_l3_ingress_create instead with intf_id as IP tunnel.");
    }
    /** In-lif wide data feature is not available for tunnel termination */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_WIDE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flag BCM_TUNNEL_TERM_WIDE is not support");
    }

    /** Check if the USID flag is used with tunnel type, which supports it*/
    dnx_tunnel_terminator_type_is_supported_with_usid(unit, info->type, &type_is_supported_with_usid);
    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_MICRO_SEGMENT_ID) && !type_is_supported_with_usid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "uSID is supported only with tunnel type bcmTunnelTypeIpAnyIn6, bcmTunnelTypeSR6 or bcmTunnelTypeUdp6");
    }
    /** Verify the udp source port are not supplied as API input*/
    if (info->udp_src_port != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "UDP Source port are no longer user input");
    }

    /** Verify the udp destination port are only supplied as API input for L2tpv2 or for an ESP or for GTP*/
    if ((info->udp_dst_port != 0) && (info->udp_dst_port != DNX_TUNNEL_TERM_EM_L2TPV2_UDP_DST_PORT) &&
        !((info->udp_dst_port == DNX_TUNNEL_TERM_ESP_UDP_DST_PORT) && dnx_data_switch.svtag.supported_get(unit)) &&
        (info->udp_dst_port != DNX_TUNNEL_TERM_GPTV1U_UDP_DST_PORT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "UDP Destination port are no longer user input");
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
    if ((info->inlif_counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Statistics are supported with dedicated API, please read the backward compatibility document");
    }
    /** Verify the info->tunnel_class is not supplied as API input*/
    if (BCM_GPORT_IS_SET(info->tunnel_class) && !BCM_GPORT_IS_TUNNEL(info->tunnel_class))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The tunnel_class must be tunnel GPORT or normal value (not GPORT)");
    }
    /** Verify qos model.*/
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
        (info->type != bcmTunnelTypeVxlanGpe) && (info->type != bcmTunnelTypeVxlan6))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "only vxlanGPE and vxlan6 support ecn eligible");
    }

    dnx_tunnel_terminator_type_is_ipv6(unit, info->type, &tunnel_is_ipv6);
    /** Verify the priority field input */
    if (tunnel_is_ipv6)
    {
        if (info->flags & BCM_TUNNEL_TERM_USE_TUNNEL_CLASS)
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
    else
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

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_create_vsi_verify(unit, info));

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_create_default_vrf_verify(unit, info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_tunnel_terminator_get and bcm_dnx_tunnel_terminator_delete to verify info struct.
 */
static shr_error_e
dnx_tunnel_terminator_struct_verify(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    /** verify that tunnel type is supported */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_tunnel_type_verify(unit, info->type));

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
static shr_error_e
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
        {
            *ipv4_layer_qualifier_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE4;
            break;
        }
        case bcmTunnelTypeIpAnyIn4:
        {
            *ipv4_layer_qualifier_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_RAW;
            break;
        }
        case bcmTunnelTypeEsp:
        {
            *ipv4_layer_qualifier_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_ESPOIPSEC;
            break;
        }
        case bcmTunnelTypeUdp:
        {
            if ((info->udp_dst_port == DNX_TUNNEL_TERM_ESP_UDP_DST_PORT) && dnx_data_switch.svtag.supported_get(unit))
            {
                *ipv4_layer_qualifier_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_ESPOIPSEC_UDP;
            }
            else if (info->udp_dst_port == DNX_TUNNEL_TERM_EM_L2TPV2_UDP_DST_PORT)
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
        }
        case bcmTunnelTypeVxlan:
        {
            *ipv4_layer_qualifier_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_VXLANOUDP;
            break;
        }
        case bcmTunnelTypeVxlanGpe:
        {
            *ipv4_layer_qualifier_tunnel_type = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_VXLAN_GPEOUDP;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel type value (%d) is not supported", info->type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Convert IPV4 layer qualifer to tunnel type IPv4 tunnel type.
 */
static shr_error_e
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
        {
            *tunnel_type = bcmTunnelTypeGreAnyIn4;
            break;
        }
        case DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_RAW:
        {
            *tunnel_type = bcmTunnelTypeIpAnyIn4;
            break;
        }
        case DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_UDP:
        {
            *tunnel_type = bcmTunnelTypeUdp;
            break;
        }
        case DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_ESPOIPSEC:
        {
            *tunnel_type = bcmTunnelTypeEsp;
            break;
        }
        case DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_ESPOIPSEC_UDP:
        {
            *tunnel_type = bcmTunnelTypeUdp;
            *udp_dst_port = DNX_TUNNEL_TERM_ESP_UDP_DST_PORT;
            break;
        }
        case DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_L2TPV3OUDP:
        {
            *tunnel_type = bcmTunnelTypeUdp;
            *udp_dst_port = DNX_TUNNEL_TERM_EM_L2TPV2_UDP_DST_PORT;
            break;
        }
        case DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GTPUOUDP:
        {
            *tunnel_type = bcmTunnelTypeUdp;
            *udp_dst_port = DNX_TUNNEL_TERM_GPTV1U_UDP_DST_PORT;
            break;
        }
        case DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_VXLANOUDP:
        {
            *tunnel_type = bcmTunnelTypeVxlan;
            break;
        }
        case DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_VXLAN_GPEOUDP:
        {
            *tunnel_type = bcmTunnelTypeVxlanGpe;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel qualifier value (%d) is not supported",
                         ipv4_layer_qualifier_tunnel_type);
            break;
        }
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
        case bcmTunnelTypeGreAnyIn6:
        case bcmTunnelTypeIpAnyIn6:
        case bcmTunnelTypeVxlan6:
        case bcmTunnelTypeVxlan6Gpe:
        case bcmTunnelTypeUdp6:
        case bcmTunnelTypeEsp:
        case bcmTunnelTypeEsp6:
        
        case bcmTunnelTypeIp6In6:
        {
            /*
             * Next Layer Type: ANY
             */
            *next_layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel type value (%d) is not supported", tunnel_type);
            break;
        }
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
 *   \param [in] tunnel_type -
 *     Tunnel termination type.
 *   \param [in] udp_dst_port -
 *     The UDP destination port
 *   \param [out] first_additional_header -
 *     IPv4/IPv6 first additional header.
 *   \param [out] second_additional_header -
 *     IPv4/IPv6 second additional header.
 */
static shr_error_e
dnx_tunnel_terminator_type_to_ipv6_additional_headers(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 udp_dst_port,
    uint32 *first_additional_header,
    uint32 *second_additional_header)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(first_additional_header, _SHR_E_PARAM, "first_additional_header");
    SHR_NULL_CHECK(second_additional_header, _SHR_E_PARAM, "second_additional_header");

    switch (tunnel_type)
    {
        case bcmTunnelTypeGreAnyIn6:
        {
            /*
             * GRE6 tunnel: IPv6 first additional header = GRE4
             *              IPv6 second additional header = NONE
             */
            *first_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_GRE4;
            *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE;
            break;
        }
        case bcmTunnelTypeIpAnyIn6:
        {
            /*
             * IPv6 tunnel: IPv6 first additional header = NONE
             *              IPv6 second additional header = NONE
             */
            *first_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE;
            *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE;
            break;
        }
        case bcmTunnelTypeEsp6:
        {
            /*
             * IPv6 tunnel: IPv6 first additional header = ESP
             *              IPv6 second additional header = NONE
             */
            *first_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_ESP;
            *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE;
            break;
        }
        case bcmTunnelTypeVxlan6:
        {
            /*
             * IPv6 tunnel: IPv6 first additional header = UDP
             *              IPv6 second additional header = VXLAN
             */
            *first_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP;
            *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_VXLAN;
            break;
        }

        case bcmTunnelTypeVxlan6Gpe:
        {
            /*
             * IPv6 tunnel: IPv6 first additional header = UDP
             *              IPv6 second additional header = VXLAN GPE
             */
            *first_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP;
            *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_VXLAN_GPE;
            break;
        }
        case bcmTunnelTypeUdp6:
        {
            if ((udp_dst_port == DNX_TUNNEL_TERM_ESP_UDP_DST_PORT) && dnx_data_switch.svtag.supported_get(unit))
            {
                /*
                 * IPv6 tunnel: IPv6 first additional header = UDP
                 *              IPv6 second additional header = ESP_UDP
                 */
                *first_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP;
                *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_ESP_UDP;

            }
            else
            {
                /*
                 * IPv6 tunnel: IPv6 first additional header = UDP
                 *              IPv6 second additional header = NONE
                 */
                *first_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP;
                *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE;
            }
            break;
        }

        case bcmTunnelTypeSR6:
        {
            /*
             * SRv6 tunnel: IPv6 first additional header = SRv6
             *              IPv6 second additional header = NONE
             */
            *first_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_ROUTING;
            *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE;
            break;
        }
        case bcmTunnelTypeEthSR6:
        {
            /*
             * IPv6 tunnel: IPv6 first additional header = SRv6
             *              IPv6 second additional header = NONE
             */
            *first_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_ROUTING;
            *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE;
            break;
        }
        case bcmTunnelTypeEthIn6:
        {
            /*
             * IPv6 tunnel: IPv6 first additional header = NONE
             *              IPv6 second additional header = NONE
             */
            *first_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE;
            *second_additional_header = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel type value (%d) is not supported", tunnel_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** \brief
 * The function returns bcm tunnel type based on additional headers
 */
static shr_error_e
dnx_tunnel_terminator_additional_headers_to_ipv6_type(
    int unit,
    uint32 first_additional_header,
    uint32 second_additional_header,
    bcm_tunnel_type_t * tunnel_type,
    uint16 *udp_dst_port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tunnel_type, _SHR_E_PARAM, "tunnel_type");

    *udp_dst_port = 0;

    if (first_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_GRE4
        && second_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE)
    {
        *tunnel_type = bcmTunnelTypeGreAnyIn6;
    }
    else if (first_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE
             && second_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE)
    {
        *tunnel_type = bcmTunnelTypeIpAnyIn6;
    }
    else if (first_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP
             && second_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_VXLAN)
    {
        *tunnel_type = bcmTunnelTypeVxlan6;
    }
    else if (first_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP
             && second_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_VXLAN_GPE)
    {
        *tunnel_type = bcmTunnelTypeVxlan6Gpe;
    }
    else if (first_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP
             && second_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE)
    {
        *tunnel_type = bcmTunnelTypeUdp6;
    }
    else if (first_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP
             && second_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_ESP_UDP)
    {
        *udp_dst_port = DNX_TUNNEL_TERM_ESP_UDP_DST_PORT;
        *tunnel_type = bcmTunnelTypeUdp6;
    }
    else if (first_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_ROUTING
             && second_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE)
    {
        *tunnel_type = bcmTunnelTypeSR6;
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
    uint32 tunnel_is_ipv6 = FALSE;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    *tunnel_class = info->tunnel_class;
    dnx_tunnel_terminator_type_is_ipv6(unit, info->type, &tunnel_is_ipv6);

    if (tunnel_is_ipv6)
    {
        if (info->flags & BCM_TUNNEL_TERM_USE_TUNNEL_CLASS)
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
 * The function returns information stored for Ipv6 EM Tunnel Terminator
 */
static shr_error_e
dnx_tunnel_terminator_ipv6_em_get(
    int unit,
    uint32 entry_handle_id,
    bcm_ip6_t * dip6,
    bcm_ip6_t * dip6_mask,
    int *vrf,
    uint32 *local_inlif,
    bcm_tunnel_type_t * tunnel_type,
    uint16 *udp_dst_port)
{
    uint32 first_additional_header, second_additional_header, vrf_value;
    uint8 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint8 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint32 field_value2[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_masked_get
                    (unit, entry_handle_id, DBAL_FIELD_IPV6_DIP, field_value, field_mask));
    sal_memcpy(*dip6, field_value, sizeof(bcm_ip6_t));
    sal_memcpy(*dip6_mask, field_mask, sizeof(bcm_ip6_t));
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VRF, &vrf_value));
    *vrf = vrf_value;
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST, &first_additional_header));
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_2ND,
                     &second_additional_header));
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_additional_headers_to_ipv6_type
                    (unit, first_additional_header, second_additional_header, tunnel_type, udp_dst_port));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_IRPP_CROSS_STAGE_VAR_2, INST_SINGLE, field_value2));
    *local_inlif = field_value2[0];

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
    uint32 entry_access_id;
    uint32 table_id = 0;
    int core = DBAL_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** GET DBAL table ID */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_dbal_table_id_get(unit, info, &table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Create entry access id to TCAM or TCAM 2nd pass table*/
    if ((table_id == DBAL_TABLE_IPV4_TT_TCAM_BASIC) || (table_id == DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS))
    {
        /** Create TCAM access id */
        SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create(unit, core, table_id, info->priority, &entry_access_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
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

    SHR_IF_ERR_EXIT(algo_tunnel_db.tunnel_ipv6_sip_profile.allocate_single
                    (unit, 0, info->sip6, NULL, &sip_profile_id, &first_reference));

    if (first_reference)
    {

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_TT_SIP_MSB_COMPRESS_EM, &entry_handle_id));

        /** Set KEY fields */
        dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_IPV6, info->sip6);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

        /** Set VALUE fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_ADDRESS_MSBS_ID, INST_SINGLE,
                                     sip_profile_id);

        /** Write to HW */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
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
    uint32 entry_access_id;
    uint32 entry_handle_id;
    uint32 first_additional_header = 0;
    uint32 second_additional_header = 0;
    int core = DBAL_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_type_to_ipv6_additional_headers
                    (unit, info->type, info->udp_dst_port, &first_additional_header, &second_additional_header));
    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_BUD))
    {
        SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create
                        (unit, core, DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS, info->priority, &entry_access_id));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS, &entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create
                        (unit, core, DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC, info->priority, &entry_access_id));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC, &entry_handle_id));
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP, info->dip6, info->dip6_mask);

    /** add additional header only when additional header exist.
     *  Ex: parser recognize additional header hop-by-hop,
     *  tunnel lookup DIP, SIP, VRF, tunnel_type: ipanyin6 should
     *  hit.
     */
    if (first_additional_header != DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST,
                                   first_additional_header);
    }

    if (second_additional_header != DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_2ND,
                                   second_additional_header);
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, info->vrf);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_inlif);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

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
 */
static shr_error_e
dnx_tunnel_terminator_lookup_add(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 local_inlif)
{
    uint32 tunnel_is_ipv6 = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    dnx_tunnel_terminator_type_is_ipv6(unit, info->type, &tunnel_is_ipv6);

    if (tunnel_is_ipv6)
    {
        if (info->flags & BCM_TUNNEL_TERM_USE_TUNNEL_CLASS)
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_add_ipv6_p2p(unit, info, local_inlif));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_add_ipv6_mp(unit, info, local_inlif));
        }
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
 * Adds Global to local lif mapping in ISEM
 * The mapping is used to retrieve the last tunnel of first pass in second pass
 * The mapping is used in RCY ext. termination flows
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] global_inlif -
 *     Allocated/Given Global-In-LIF.
 *   \param [in] local_inlif -
 *     Allocated Local-In-LIF.
 */
static shr_error_e
dnx_tunnel_terminator_global_lif_reclassify_add(
    int unit,
    uint32 global_inlif,
    uint32 local_inlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_LIF_RECLASSIFICATION, &entry_handle_id));

    /** Set KEY fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, global_inlif);
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
 * Reads Global to local lif mapping in ISEM
 * The mapping is used to retrieve the last tunnel of first pass in second pass
 * The mapping is used in RCY ext. termination flows
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] global_inlif -
 *     Allocated/Given Global-In-LIF.
 *   \param [out] found -
 *     Indication if entry is found
 */
static shr_error_e
dnx_tunnel_terminator_global_lif_reclassify_get(
    int unit,
    uint32 global_inlif,
    uint8 *found)
{
    uint32 entry_handle_id;
    uint32 local_lif = 0;
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *found = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_LIF_RECLASSIFICATION, &entry_handle_id));

    /** Set KEY fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, global_inlif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &local_lif);

    /** Read from HW */
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT);
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_EXIT();
    }
    else if (rv == _SHR_E_NONE)
    {
        *found = 1;
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Deletes Global to local lif mapping in ISEM
 * The mapping is used to retrieve the last tunnel of first pass in second pass
 * The mapping is used in RCY ext. termination flows
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] global_inlif -
 *     Allocated/Given Global-In-LIF.
 */
static shr_error_e
dnx_tunnel_terminator_global_lif_reclassify_delete(
    int unit,
    uint32 global_inlif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_LIF_RECLASSIFICATION, &entry_handle_id));

    /** Set KEY fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, global_inlif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    /** Delete from HW */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

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

    SHR_IF_ERR_EXIT(algo_tunnel_db.tunnel_ipv6_sip_profile.profile_get(unit, info->sip6, &sip_profile_id));

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
    uint32 entry_handle_id, table_id, tcam_handler_id;
    uint32 first_additional_header = 0;
    uint32 second_additional_header = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_type_to_ipv6_additional_headers
                    (unit, info->type, info->udp_dst_port, &first_additional_header, &second_additional_header));
    /** If  BCM_TUNNEL_TERM_BUD flag is set - read from 2nd pass termination  table.*/
    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_BUD))
    {
        table_id = DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS;
    }
    else
    {
        table_id = DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP, info->dip6, info->dip6_mask);

    if (first_additional_header != DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST,
                                   first_additional_header);
    }

    if (second_additional_header != DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_2ND,
                                   second_additional_header);
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, info->vrf);

    SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, table_id, &tcam_handler_id));
    SHR_IF_ERR_EXIT_NO_MSG(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_priority_get
                    (unit, tcam_handler_id, entry_access_id, (uint32 *) &(info->priority)));

    /** Set pointer to receive field - {in LIF} */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_inlif);

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
    uint32 tunnel_is_ipv6 = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    dnx_tunnel_terminator_type_is_ipv6(unit, info->type, &tunnel_is_ipv6);

    if (tunnel_is_ipv6)
    {
        if (info->flags & BCM_TUNNEL_TERM_USE_TUNNEL_CLASS)
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_get_ipv6_p2p(unit, info, local_inlif));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_get_ipv6_mp(unit, info, local_inlif));
        }
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

    SHR_IF_ERR_EXIT(algo_tunnel_db.tunnel_ipv6_sip_profile.profile_get(unit, info->sip6, &sip_profile_id));

    SHR_IF_ERR_EXIT(algo_tunnel_db.tunnel_ipv6_sip_profile.free_single(unit, sip_profile_id, &last_reference));

    if (last_reference)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_TT_SIP_MSB_COMPRESS_EM, &entry_handle_id));

        /** Set KEY fields */
        dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_IPV6, info->sip6);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
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
    uint32 first_additional_header = 0;
    uint32 second_additional_header = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_type_to_ipv6_additional_headers
                    (unit, info->type, info->udp_dst_port, &first_additional_header, &second_additional_header));
    /** If  BCM_TUNNEL_TERM_BUD flag is present, access 2nd pass termination table*/
    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_BUD))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS, &entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC, &entry_handle_id));
    }

    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP, info->dip6, info->dip6_mask);

    if (first_additional_header != DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST,
                                   first_additional_header);
    }

    if (second_additional_header != DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_2ND,
                                   second_additional_header);
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, info->vrf);

    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_BUD))
    {
        SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS, entry_access_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC, entry_access_id));
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
        SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, table_id, entry_access_id));
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
    uint32 tunnel_is_ipv6 = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    dnx_tunnel_terminator_type_is_ipv6(unit, info->type, &tunnel_is_ipv6);

    if (tunnel_is_ipv6)
    {
        if (info->flags & BCM_TUNNEL_TERM_USE_TUNNEL_CLASS)
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_delete_ipv6_p2p(unit, info));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_delete_ipv6_mp(unit, info));
        }
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
 * Except for vxlan/vxlan-gpe, vrf will be updated by bcm_l3_ingress_create API.
 * But we want to force the lif table mgr to select a result type which include VRF field
 * vsi is set for srv6 
 * default vrf for vxlan gpe application when vni->vrf lookup miss 
 *   \param [in] unit - Relevant unit.
 *   \param [in] entry_handle_id - entry handle id
 *   \param [in] info - tunnel termination info. 
 *   \param [in] fodo_assignment_mode - fodo assignment mode
 *   \param [in] vrf - vrf
 */
static shr_error_e
dnx_tunnel_terminator_in_lif_table_optional_fodo_or_vrf_set(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_terminator_t * info,
    uint32 fodo_assignment_mode,
    uint32 vrf)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * vsi for srv6 
     */
    if (((info->type == bcmTunnelTypeEthSR6) || (info->type == bcmTunnelTypeEthIn6)) && (info->vlan != 0))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_fodo_vsi_set(unit, entry_handle_id, info->vlan));
    }
    /*
     * vrf for vxlan gpe when no hit in {vni, network domain -> FODO vrf} lookup 
     */
    else if (DNX_TUNNEL_TERM_TYPE_IS_VXLAN_GPE(info->type))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_fodo_vrf_set(unit, entry_handle_id, info->default_vrf));
    }
    else if (fodo_assignment_mode == DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, vrf);
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
 *   \param [out] local_inlif - Allocated Local-In-LIF.
 */
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
    uint32 *local_inlif)
{
    uint32 entry_handle_id;
    lif_table_mngr_inlif_info_t lif_table_mngr_inlif_info;
    uint8 is_replace;
    dbal_enum_value_field_vtt_lif_service_type_e vtt_lif_service_type =
        DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2MP;
    dbal_enum_value_field_service_per_flow_e service_per_flow = DBAL_ENUM_FVAL_SERVICE_PER_FLOW_SRV6_CLASSIC_GRE_NO_TNI;
    uint32 service_type = 0;
    uint8 set_service_type = FALSE;

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
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE,
                                 (uint32) propagation_prof);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                 (uint32) in_lif_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_MAPPING_PROFILE, INST_SINGLE,
                                 ecn_mapping_profile);

    /** set P2P if needed */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_CROSS_CONNECT))
    {
        set_service_type = TRUE;
        vtt_lif_service_type = DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P;
        /** If tunnel is cross connect, set service type to P2P */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, 0);
    }

    if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_MICRO_SEGMENT_ID))
    {
        set_service_type = TRUE;
        service_per_flow = DBAL_ENUM_FVAL_SERVICE_PER_FLOW_SRV6_MICRO_SID_GRE_TNI_FODO;
    }

    if (set_service_type)
    {
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_TYPE, &vtt_lif_service_type, &service_type));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_PER_FLOW, &service_per_flow, &service_type));
        /** If tunnel is cross connect, set service type to P2P */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE, service_type);
    }

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_optional_fodo_or_vrf_set
                    (unit, entry_handle_id, info, fodo_assignment_mode, vrf));

    /*
     * Set fodo assignment mode only when needed
     */
    if ((info->type == bcmTunnelTypeVxlan)
        || (info->type == bcmTunnelTypeVxlan6) || (DNX_TUNNEL_TERM_TYPE_IS_VXLAN_GPE(info->type)))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE,
                                     fodo_assignment_mode);
    }

    /** NEXT_LAYER_NETWORK_DOMAIN determine by different API and is a key for choosing GTPV1U LIF.
     * Therefore, it must exist in the IP LIF
 */
    if ((info->type == bcmTunnelTypeUdp) && (info->udp_dst_port == DNX_TUNNEL_TERM_GPTV1U_UDP_DST_PORT))
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
 * Read from IP tunnel termination in-LIF table and update info struct fields.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [out] global_inlif - global-In-LIF.
 *   \param [in] local_inlif - Allocated Local-In-LIF.
 *   \param [out] propagation_prof - Template allocated QOS
 *          propagation_prof.
 *   \param [out] in_lif_profile - In lif profile
 *   \param [out] ecn_mapping_profile - ECN mapping profile
 *   \param [out] flags - stat enable flag
 *   \param [out] fodo - forwarding domain 
 *   \param [out] vrf - vrf
 *   \param [out] tunnel_type - tunnel type
 */
static shr_error_e
dnx_tunnel_terminator_in_lif_table_get(
    int unit,
    uint32 local_inlif,
    uint32 *global_inlif,
    int *propagation_prof,
    uint32 *in_lif_profile,
    uint32 *ecn_mapping_profile,
    uint32 *flags,
    uint32 *fodo,
    uint32 *vrf,
    bcm_tunnel_type_t * tunnel_type)
{
    uint32 entry_handle_id;
    uint8 is_field_valid = FALSE;
    lif_table_mngr_inlif_info_t inlif_info;
    uint32 vsi;
    dbal_fields_e sub_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(global_inlif, _SHR_E_PARAM, "global_inlif");
    SHR_NULL_CHECK(propagation_prof, _SHR_E_PARAM, "propagation_prof");

    /** take handle to read from DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_IPvX_TUNNELS, &entry_handle_id));

    /**  Get tunnel inlif info from lif tble manager  */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info(unit, _SHR_CORE_ALL, local_inlif, entry_handle_id, &inlif_info));
    {
        uint32 dbal_field[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

        sal_memset(dbal_field, 0, sizeof(dbal_field));
        /** read from dbal */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, dbal_field));
        *global_inlif = dbal_field[0];
        sal_memset(dbal_field, 0, sizeof(dbal_field));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE, (uint32 *) dbal_field));
        *propagation_prof = (int) (dbal_field[0]);
        sal_memset(dbal_field, 0, sizeof(dbal_field));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, (uint32 *) dbal_field));
        *in_lif_profile = dbal_field[0];
        sal_memset(dbal_field, 0, sizeof(dbal_field));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ECN_MAPPING_PROFILE, INST_SINGLE, dbal_field));
        *ecn_mapping_profile = dbal_field[0];

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                        (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, &is_field_valid));
        if (is_field_valid == TRUE)
        {
            *flags |= BCM_TUNNEL_TERM_STAT_ENABLE;
        }
        else
        {
            *flags = *flags | 0;
        }

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                        (unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, &is_field_valid));
        if (is_field_valid == TRUE)
        {
            dbal_enum_value_field_vtt_lif_service_type_e vtt_lif_service_type;
            dbal_enum_value_field_service_per_flow_e service_per_flow;

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE, dbal_field));

            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_TYPE, &vtt_lif_service_type, dbal_field));
            if (vtt_lif_service_type == DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P)
            {
                *flags |= BCM_TUNNEL_TERM_CROSS_CONNECT;
            }

            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_PER_FLOW, &service_per_flow, dbal_field));
            if (service_per_flow == DBAL_ENUM_FVAL_SERVICE_PER_FLOW_SRV6_MICRO_SID_GRE_TNI_FODO)
            {
                *flags |= BCM_TUNNEL_TERM_MICRO_SEGMENT_ID;
            }
        }

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, entry_handle_id, DBAL_FIELD_FODO, &is_field_valid, fodo));
        if (is_field_valid)
        {
            SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get(unit, DBAL_FIELD_FODO, *fodo, &sub_field_id, &vsi));
            if ((sub_field_id == DBAL_FIELD_VSI) && ((*tunnel_type) == bcmTunnelTypeSR6))
            {
                *tunnel_type = bcmTunnelTypeEthSR6;
            }
            if ((sub_field_id == DBAL_FIELD_VSI) && ((*tunnel_type) == bcmTunnelTypeIpAnyIn6))
            {
                *tunnel_type = bcmTunnelTypeEthIn6;
            }
        }

        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_optional_vrf_get(unit, entry_handle_id, vrf));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set tunnel termination config lookup key to DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] entry_handle_id - DBAL entry handle id
 *   \param [in] config_key - tunnel termination konfig key struct
 *
 * Used for cascaded tunnel termination lookup configuration:
 *   1) {DIP, VRF, Tunnel type} -> my-vtep-index (config_action.tunnel_class) - bcm_tunnel_terminator_config_add
 *   2) {SIP, my-vtep-index, VRF} -> IP-Tunnel In-LIF                         - bcm_tunnel_terminator_create
 *
 * Shared by:
 *  - bcm_dnx_tunnel_terminator_config_add
 *  - bcm_dnx_tunnel_terminator_config_get
 *  - bcm_dnx_tunnel_terminator_config_delete
 */
static shr_error_e
dnx_tunnel_terminator_config_lookup_key_set(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_terminator_config_key_t * config_key)
{
    uint32 first_additional_header = 0;
    uint32 second_additional_header = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_type_to_ipv6_additional_headers
                    (unit, config_key->type, config_key->udp_dst_port, &first_additional_header,
                     &second_additional_header));

    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP, config_key->dip6,
                                         config_key->dip6_mask);

    if (first_additional_header != DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST,
                                   first_additional_header);
    }

    if (second_additional_header != DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_2ND,
                                   second_additional_header);
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, config_key->vrf);

exit:
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
    uint32 entry_access_id;
    uint32 entry_handle_id;
    int core = DBAL_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** VERIFICATION of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_tunnel_terminator_config_verify(unit, config_key, config_action));

    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create(unit, core, DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC,
                                                     _shr_ip6_mask_length(config_key->dip6_mask), &entry_access_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC, &entry_handle_id));

    dnx_tunnel_terminator_config_lookup_key_set(unit, entry_handle_id, config_key);

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DIP_IDX_INTERMIDIATE, INST_SINGLE,
                                 config_action->tunnel_class);

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
    SHR_INVOKE_VERIFY_DNX(dnx_tunnel_terminator_config_verify(unit, config_key, config_action));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_config_lookup_key_set(unit, entry_handle_id, config_key));

    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DIP_IDX_INTERMIDIATE, INST_SINGLE, &tunnel_class);

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

    SHR_INVOKE_VERIFY_DNX(dnx_tunnel_terminator_config_key_verify(unit, config_key));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_config_lookup_key_set(unit, entry_handle_id, config_key));

    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

    /** Delete entry from DBAL table*/
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC, entry_access_id));
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
int
dnx_tunnel_term_fodo_assignment_mode_get(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 *fodo_assignment_mode)
{
    SHR_FUNC_INIT_VARS(unit);
    /**
     * Update fodo assignment mode for vxlan gpe.
     */
    if ((tunnel_type == bcmTunnelTypeVxlan) || (tunnel_type == bcmTunnelTypeVxlan6)
        || (DNX_TUNNEL_TERM_TYPE_IS_VXLAN_GPE(tunnel_type)))
    {
        *fodo_assignment_mode = DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP;
    }
    else
    {
        *fodo_assignment_mode = DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF;
    }
    SHR_EXIT();

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
 *             The flag can be used only with the bcmTunnelTypeSR6 tunnel type only
 *          * BCM_TUNNEL_TERM_EXTENDED_TERMINATION- Lif is the last tunnel in 1st pass of an extended termination flow
 *             The flag can be used only with the bcmTunnelTypeSR6 tunnel type only
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
    bcm_tunnel_type_t type = bcmTunnelTypeNone;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** VERIFICATION of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_tunnel_terminator_create_verify(unit, info));

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

    /** Allocate LIF and set its properies */
    if (!_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_REPLACE))
    {
        uint32 lif_alloc_flags = 0;
        int old_in_lif_profile = 0;
        int new_in_lif_profile;
        in_lif_profile_info_t in_lif_profile_info;

        /** allocate global lif  */
        if (info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID)
        {
            lif_alloc_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
            global_lif = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
        }

        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                        (unit, lif_alloc_flags, DNX_ALGO_LIF_INGRESS, &global_lif));

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
        SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));

        SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                        (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                         DBAL_TABLE_IN_LIF_IPvX_TUNNELS));

        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_allocate_lif_and_in_lif_table_set
                        (unit, info, propag_profile, fodo_assignment_mode, (uint32) new_in_lif_profile, global_lif,
                         ecn_mapping_profile, vrf, &local_inlif));
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_add(unit, info, local_inlif));

        if (_SHR_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_EXTENDED_TERMINATION))
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_global_lif_reclassify_add(unit, global_lif, local_inlif));
        }
    }
    /** Update allocated LIF properties */
    else
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        uint32 in_lif_profile;
        uint32 fodo;

        /** Get Local-in-LIF ID */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, info->tunnel_id,
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));
        local_inlif = (uint32) gport_hw_resources.local_in_lif;

        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_get
                        (unit, gport_hw_resources.local_in_lif,
                         (uint32 *) &global_lif, &propag_profile, &in_lif_profile, &ecn_mapping_profile, &flags,
                         &fodo, &vrf, &type));

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

        if (info->ingress_qos_model.ingress_ecn == bcmQosIngressEcnModelEligible)
        {
            ecn_mapping_profile = DNX_QOS_ECN_MAPPING_PROFILE_ELIGIBLE;
        }

        /** Update LIF attributes */
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_allocate_lif_and_in_lif_table_set
                        (unit, info, propag_profile, fodo_assignment_mode, in_lif_profile, global_lif,
                         ecn_mapping_profile, vrf, &local_inlif));

        /** Clear the unused profile which is indicated by last-referrence*/
        if (qos_propagation_prof_last_ref == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_clear(unit, old_propag_profile));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
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
    uint8 ext_term_found;
    dnx_qos_propagation_type_e phb_propagation_type;
    dnx_qos_propagation_type_e remark_propagation_type;
    dnx_qos_propagation_type_e ecn_propagation_type;
    dnx_qos_propagation_type_e ttl_propagation_type;
    uint32 in_lif_profile;
    uint32 ecn_mapping_profile;
    uint32 fodo = 0;
    uint32 vrf;
    bcm_tunnel_type_t type = bcmTunnelTypeNone;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** VERIFICATION of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_tunnel_terminator_struct_verify(unit, info));

    /** Get entry from ISEM to find local lif id */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_get(unit, info, &local_inlif));

    /** Get entry from IP tunnel in-LIF table */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_get
                    (unit, local_inlif, &global_inlif, &propagation_prof, &in_lif_profile, &ecn_mapping_profile,
                     &(info->flags), &fodo, &vrf, &type));

    /** VSI could be set only with tunnel types of bcmTunnelTypeEthSR6 or bcmTunnelTypeEthIn6*/
    if ((info->type == bcmTunnelTypeEthSR6) || (info->type == bcmTunnelTypeEthIn6))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_optional_vsi_get(unit, fodo, &(info->vlan)));
    }
    if ((info->type == bcmTunnelTypeSR6) || (info->type == bcmTunnelTypeEthSR6))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_global_lif_reclassify_get(unit, global_inlif, &ext_term_found));
        if (ext_term_found)
        {
            info->flags |= BCM_TUNNEL_TERM_EXTENDED_TERMINATION;
        }
    }

    if (DNX_TUNNEL_TERM_TYPE_IS_VXLAN_GPE(info->type))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_optional_default_vrf_get(unit, fodo, &(info->default_vrf)));
    }
    /** Set tunnel_id */
    BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id, global_inlif);

    
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

    if (ecn_mapping_profile == DNX_QOS_ECN_MAPPING_PROFILE_ELIGIBLE)
    {
        info->ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelEligible;
    }
    else
    {
        info->ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelInvalid;
    }

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
    uint32 ecn_mapping_profile;
    uint32 flags = 0;
    uint32 fodo = 0;
    uint8 ext_term_found;
    bcm_tunnel_type_t type = bcmTunnelTypeNone;
    uint32 vrf;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    new_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    /** VERIFICATION of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_tunnel_terminator_struct_verify(unit, info));

    /** Get entry from ISEM to find local lif id */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_get(unit, info, &local_inlif));

    /** Get entry from IP tunnel in-LIF table */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_get
                    (unit, local_inlif, &global_inlif, &propagation_prof, &in_lif_profile, &ecn_mapping_profile,
                     &flags, &fodo, &vrf, &type));
    if (global_inlif == 0)
    {
        bcm_gport_t gport;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                        (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS, _SHR_CORE_ALL, local_inlif,
                         &gport));
        global_inlif = BCM_GPORT_TUNNEL_ID_GET(gport);
    }

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_dealloc(unit, in_lif_profile, &new_in_lif_profile, LIF));
    /** Delete lookup entry from ISEM */
    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_lookup_delete(unit, info));

    if ((info->type == bcmTunnelTypeSR6) || (info->type == bcmTunnelTypeEthSR6))
    {
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_global_lif_reclassify_get(unit, global_inlif, &ext_term_found));
        if (ext_term_found)
        {
            SHR_IF_ERR_EXIT(dnx_tunnel_terminator_global_lif_reclassify_delete(unit, global_inlif));
        }
    }

    /** Free propagation profile template */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_free_and_hw_clear(unit, propagation_prof, NULL));

    /** Delete entry from IP tunnel in-LIF table.
     *  Delete local in LIF */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_inlif_info_clear
                    (unit, _SHR_CORE_ALL, local_inlif, DBAL_TABLE_IN_LIF_IPvX_TUNNELS, 0));
    /** Delete global in LIF */
    dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_INGRESS, global_inlif);

exit:
    DNX_ERR_RECOVERY_END(unit);
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
 */
shr_error_e
bcm_dnx_tunnel_terminator_traverse(
    int unit,
    bcm_tunnel_terminator_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id, entry_handle_id_internal;
    uint32 in_lif_profile;
    uint32 dbal_tables[] =
        { DBAL_TABLE_IPV4_TT_P2P_EM_2ND_PASS, DBAL_TABLE_IPV4_TT_P2P_EM_BASIC, DBAL_TABLE_IPV4_TT_MP_EM_2ND_PASS,
        DBAL_TABLE_IPV4_TT_MP_EM_BASIC, DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS, DBAL_TABLE_IPV4_TT_TCAM_BASIC,
        DBAL_TABLE_IPV6_TT_P2P_EM_BASIC, DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC, DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS
    };
    uint32 nof_tables = sizeof(dbal_tables) / sizeof(dbal_tables[0]);
    uint32 table, local_inlif, local_inlif_p2p, global_inlif, vrf_value, sip_profile_id, tunnel_class;
    uint32 ecn_mapping_profile, flags = 0;
    uint16 udp_dst_port;
    int is_end, ref_count, is_end_ipv6, vrf;
    bcm_tunnel_type_t type = bcmTunnelTypeNone;
    bcm_tunnel_terminator_t info;
    int propagation_prof;
    uint32 ipv4_tunnel_type_qualifier = 0;
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    bcm_ip6_t dip6, dip6_mask;
    uint32 fodo;
    uint32 dummy_vrf;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_tunnel_terminator_traverse_verify(unit, cb, user_data));

    /*
     * Iterate over all tables and all their entries
     */
    for (table = 0; table < nof_tables; table++)
    {
        /*
         * Allocate handle to the table of the iteration and initialise an iterator entity.
         * The iterator is in mode ALL, which means that it will consider all entries regardless
         * of them being default entries or not.
         */
        if (table == 0)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[table], &entry_handle_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_tables[table], entry_handle_id));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

        /*
         * Receive first entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            global_inlif = LIF_MNGR_INVALID;

            bcm_tunnel_terminator_t_init(&info);
            if (dbal_tables[table] == DBAL_TABLE_IPV6_TT_P2P_EM_BASIC)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_IPV6, field_value));
                sal_memcpy(info.dip6, field_value, sizeof(bcm_ip6_t));
                info.flags = BCM_TUNNEL_TERM_USE_TUNNEL_CLASS;
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_IPV6_ADDRESS_MSBS_ID, &sip_profile_id));
                SHR_IF_ERR_EXIT(algo_tunnel_db.tunnel_ipv6_sip_profile.profile_data_get(unit, sip_profile_id,
                                                                                        &ref_count,
                                                                                        (void *) &info.sip6));
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_IPV6_MP_TUNNEL_IDX, &tunnel_class));
                /** Set pointer to receive field - {in LIF} */
                dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE,
                                                        &local_inlif_p2p);
                info.tunnel_class = tunnel_class;
                /** Allocate another handle to traverse internaly the TCAM table,
                 * This is needed in case of P2P and MP tunnels working together, in  this case we created P2P tunnel
                 * and used its inlif as intermediate object. The MP tunnel serves as created from bcm_tunnel_terminator_config_add
                 */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC, &entry_handle_id_internal));
                SHR_IF_ERR_EXIT(dbal_iterator_init
                                (unit, entry_handle_id_internal, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_internal, &is_end_ipv6));
                while (!is_end_ipv6)
                {
                    /** When the entry is written in the TCAM table from tunnel_terminator_config_add and
                     * tunnel_terminator_create APIs it has a different format regarding the IRPP_CROSS_STAGE_VAR_2 field
                     * It might serve as tunnel_class and inlif. The difference is the MSB 3 bits
                     */
                    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_em_get
                                    (unit, entry_handle_id_internal, &dip6, &dip6_mask, &vrf, &local_inlif, &type,
                                     &udp_dst_port));
                    if (((local_inlif & (DNX_TUNNEL_TERM_LOCAL_IN_LIF_ENCODING - 1)) == tunnel_class)
                        && ((local_inlif & DNX_TUNNEL_TERM_LOCAL_IN_LIF_ENCODING) ==
                            DNX_TUNNEL_TERM_LOCAL_IN_LIF_ENCODING))
                    {
                        local_inlif = local_inlif & (DNX_TUNNEL_TERM_LOCAL_IN_LIF_ENCODING - 1);
                        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                        (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS, _SHR_CORE_ALL,
                                         tunnel_class, &info.tunnel_class));
                        /** Get information about the global inlif and convert it to tunnel gport */
                        /** Get entry from IP tunnel in-LIF table */
                        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_get
                                        (unit, local_inlif_p2p, &global_inlif, &propagation_prof, &in_lif_profile,
                                         &ecn_mapping_profile, &flags, &fodo, &dummy_vrf, &type));
                        break;
                    }
                    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_internal, &is_end_ipv6));
                }
            }
            else if ((dbal_tables[table] == DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS)
                     || (dbal_tables[table] == DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC))
            {
                SHR_IF_ERR_EXIT(dnx_tunnel_terminator_ipv6_em_get
                                (unit, entry_handle_id, &info.dip6, &info.dip6_mask, &info.vrf, &local_inlif,
                                 &info.type, &udp_dst_port));
                info.udp_dst_port = udp_dst_port;

                if ((local_inlif & DNX_TUNNEL_TERM_LOCAL_IN_LIF_ENCODING) == DNX_TUNNEL_TERM_LOCAL_IN_LIF_ENCODING)
                {
                    local_inlif = local_inlif & (DNX_TUNNEL_TERM_LOCAL_IN_LIF_ENCODING - 1);
                    type = info.type;
                    /** Get information about the global inlif and convert it to tunnel gport */
                    /** Get entry from IP tunnel in-LIF table */
                    SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_get
                                    (unit, local_inlif, &global_inlif, &propagation_prof, &in_lif_profile,
                                     &ecn_mapping_profile, &flags, &fodo, &dummy_vrf, &type));
                    /**If the tunnel type has vsi set on it, then its either bcmTunnelTypeEthIn6 or bcmTunnelTypeEthSR6*/
                    if ((type == bcmTunnelTypeEthIn6) || (type == bcmTunnelTypeEthSR6))
                    {
                        info.type = type;
                        info.vlan = fodo;
                    }
                }
                /** If the entry is not encoded with lif field,
                 * it means it was added by bcm_tunnel_terminator_config_add API and is not a subject of bcm_tunnel_terminator_traverse*/
                else
                {
                    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                    continue;
                }
            }
            /** Get info about IPV4 ingress tunnels */
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_VRF, &vrf_value));
                info.vrf = vrf_value;
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_IPV4_TUNNEL_TYPE, &ipv4_tunnel_type_qualifier));
                SHR_IF_ERR_EXIT(dnx_ipv4_layer_qualifier_tunnel_type_to_tunnel_terminator_type
                                (unit, ipv4_tunnel_type_qualifier, &info.type, &info.udp_dst_port));
                if ((dbal_tables[table] == DBAL_TABLE_IPV4_TT_TCAM_BASIC)
                    || (dbal_tables[table] == DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS))
                {
                    /** handle coverity issues */
                    uint32 dip_mask_arr[1] = { 0 };
                    uint32 sip_mask_arr[1] = { 0 };

                    /** TCAM access - get the DIP/SIP and masks */
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                                    (unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, &info.dip, dip_mask_arr));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                                    (unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, &info.sip, sip_mask_arr));
                    info.dip_mask = dip_mask_arr[0];
                    info.sip_mask = sip_mask_arr[0];
                }
                else
                {
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_IPV4_DIP, &info.dip));
                    info.dip_mask = DNX_TUNNEL_TERM_EM_IP_MASK;
                    info.sip_mask = 0;
                    if ((dbal_tables[table] == DBAL_TABLE_IPV4_TT_P2P_EM_BASIC)
                        || (dbal_tables[table] == DBAL_TABLE_IPV4_TT_P2P_EM_2ND_PASS))
                    {   /** P2P */
                        info.sip_mask = DNX_TUNNEL_TERM_EM_IP_MASK;
                        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                        (unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, &info.sip));
                    }
                }
                /** Set pointer to receive field - {in LIF} */
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &local_inlif));

                /** Get entry from IP tunnel in-LIF table */
                SHR_IF_ERR_EXIT(dnx_tunnel_terminator_in_lif_table_get
                                (unit, local_inlif, &global_inlif, &propagation_prof, &in_lif_profile,
                                 &ecn_mapping_profile, &flags, &fodo, &dummy_vrf, &type));
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
 */

shr_error_e
bcm_dnx_tunnel_terminator_config_traverse(
    int unit,
    bcm_tunnel_terminator_config_traverse_info_t additional_info,
    bcm_tunnel_terminator_config_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id, vrf_value, first_additional_header, second_additional_header;
    int is_end;
    uint8 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint8 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
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
        bcm_tunnel_terminator_config_key_t_init(&config_key);
        /**
         * Get the key fields of the table to extract the config_key information,
         * which is comprised of IPV6 DIP, tunnel_type and VRF
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_masked_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV6_DIP, field_value, field_mask));
        sal_memcpy(config_key.dip6, field_value, sizeof(bcm_ip6_t));
        sal_memcpy(config_key.dip6_mask, field_mask, sizeof(bcm_ip6_t));

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VRF, &vrf_value));
        config_key.vrf = vrf_value;
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST,
                         &first_additional_header));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_2ND,
                         &second_additional_header));
        SHR_IF_ERR_EXIT(dnx_tunnel_terminator_additional_headers_to_ipv6_type
                        (unit, first_additional_header, second_additional_header, &config_key.type,
                         &config_key.udp_dst_port));
        if (cb != NULL)
        {
            /*
             * Invoke callback function
             */
            SHR_IF_ERR_EXIT((*cb) (unit, &config_key, user_data));
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
