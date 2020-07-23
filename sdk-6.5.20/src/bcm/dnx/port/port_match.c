/*
 ** \file port_match.c $Id$ PORT Match procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bslenum.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/port.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/port/port_match.h>
#include <bcm_int/dnx/port/port_esem.h>
#include <bcm_int/dnx/port/port_sit.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_port_pp_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/port/port_esem.h>
#include <bcm_int/dnx/switch/switch_svtag.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

/*
 * }
 */

/** L2 gport: VLAN_PORT, EXTENDER_PORT or MPLS_port.*/
#define DNX_GPORT_IS_L2_GPORT(_gport) (BCM_GPORT_IS_VLAN_PORT(_gport) || BCM_GPORT_IS_EXTENDER_PORT(_gport) || \
                                       BCM_GPORT_IS_MPLS_PORT(_gport) || BCM_GPORT_IS_TUNNEL(_gport))

/** MPLS L2 GPorts: MPLS_PORT, TUNNEL (for EVPN) */
#define DNX_GPORT_IS_MPLS_L2_GPORT(_gport) (BCM_GPORT_IS_MPLS_PORT(_gport) || BCM_GPORT_IS_TUNNEL(_gport))

/** Enum for dnx_port_match_esem_cmd_update.*/
typedef enum
{
    /**
     * Delete ESEM default profile from the CMD
     */
    DNX_PORT_ESEM_CMD_DEL = 0,

    /**
     * Add ESEM default profile to the CMD
     */
    DNX_PORT_ESEM_CMD_ADD
} dnx_port_match_esem_cmd_oper_e;

/** Enum for dnx_port_match_ingress_match_info_sw_set.*/
typedef enum
{
    /**
     * Delete port match entry from the table
     */
    DNX_PORT_MATCH_DEL = 0,

    /**
     * Delete port match entry from the table
     */
    DNX_PORT_MATCH_ADD = 1,
} dnx_port_match_info_oper_e;

/**
 * \brief
 * Verify function for BCM-API:
 *    bcm_dnx_port_match_add
 *    bcm_dnx_port_match_delete
 */
static shr_error_e
dnx_port_match_verify(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    int is_ingress, is_egress, is_native_ac;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(match_info, _SHR_E_PARAM, "match_info");

    /*
     * Verify flags:
     *  - One of ingres/egress MUST be set.
     *  - Only one of ingres/egress is allowed to be set.
     */
    is_ingress = _SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_INGRESS_ONLY) ? TRUE : FALSE;
    is_egress = _SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_EGRESS_ONLY) ? TRUE : FALSE;
    is_native_ac = _SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_NATIVE);

    if ((is_ingress == FALSE) && (is_egress == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong flags setting. Neither INGRESS nor EGRESS are set!!! flags = 0x%08X\n",
                     match_info->flags);
    }

    if ((is_ingress == TRUE) && (is_egress == TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong flags setting. Both INGRESS and EGRESS are set!!! flags = 0x%08X\n",
                     match_info->flags);
    }

    if (is_ingress == TRUE)
    {
        /*
         * match criteria for ingress:
         */

        if (is_native_ac)
        {
            /*
             * Native matching over PWE validation
             */
            if (!BCM_GPORT_IS_VLAN_PORT(port))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Ingress unsupported match. VLAN tags in Native ETH can only be matched to VLAN port.");
            }
            if (!DNX_GPORT_IS_MPLS_L2_GPORT(match_info->port))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Ingress unsupported match. VLAN tags in Native ETH can only be matched over MPLS L2 port "
                             "(MPLS Port or EVPN).");
            }
        }
        else
        {
            /*
             * Verify that the gport type is VLAN PORT
             */
            if (BCM_GPORT_SUB_TYPE_LIF_VAL_GET(BCM_GPORT_VLAN_PORT_ID_GET(port)) == -1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. port = 0x%08X is not a VLAN port LIF.\n", port);
            }
        }

        /*
         * Verify match criteria for ingress:
         */
        switch (match_info->match)
        {
                
            case BCM_PORT_MATCH_PORT:
            case BCM_PORT_MATCH_PORT_PON_TUNNEL:
            case BCM_PORT_MATCH_PORT_PON_TUNNEL_UNTAGGED:
                break;

            case BCM_PORT_MATCH_PORT_VLAN:
            case BCM_PORT_MATCH_PORT_CVLAN:
            case BCM_PORT_MATCH_PORT_PON_TUNNEL_VLAN:
            case BCM_PORT_MATCH_PORT_PON_TUNNEL_CVLAN:
                /*
                 * Verify match_vlan is in range:
                 */
                BCM_DNX_VLAN_CHK_ID(unit, match_info->match_vlan);

                /*
                 * BCM_PORT_MATCH_PORT_CVLAN is same as BCM_PORT_MATCH_PORT_VLAN for Native.
                 */
                if ((is_native_ac == TRUE) && (match_info->match == BCM_PORT_MATCH_PORT_CVLAN))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, Native Ingress unsupported match! match = BCM_PORT_MATCH_PORT_CVLAN (%d) is no supported for Native. Please use BCM_PORT_MATCH_PORT_VLAN (%d) instead. flags = 0x%08X\n",
                                 BCM_PORT_MATCH_PORT_CVLAN, BCM_PORT_MATCH_PORT_VLAN, match_info->flags);
                }

                break;

            case BCM_PORT_MATCH_PORT_VLAN_STACKED:
            case BCM_PORT_MATCH_PORT_CVLAN_STACKED:
            case BCM_PORT_MATCH_PORT_SVLAN_STACKED:
                /*
                 * Verify match_vlan and match_inner_vlan are in range:
                 */
                BCM_DNX_VLAN_CHK_ID(unit, match_info->match_vlan);
                BCM_DNX_VLAN_CHK_ID(unit, match_info->match_inner_vlan);
                break;
            case BCM_PORT_MATCH_PORT_PON_TUNNEL_VLAN_STACKED:
            case BCM_PORT_MATCH_PORT_PON_TUNNEL_CVLAN_STACKED:
            case BCM_PORT_MATCH_PORT_PON_TUNNEL_SVLAN_STACKED:
            {
                int is_pon_enable, is_pon_dtc_enable;
                dnx_algo_gpm_gport_phy_info_t gport_info;

                if (is_ingress)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
                    SHR_IF_ERR_EXIT(dnx_port_sit_pon_enable_get(unit, gport_info.local_port, &is_pon_enable));
                    if (is_pon_enable)
                    {
                        SHR_IF_ERR_EXIT(bcm_vlan_control_port_get
                                        (unit, gport_info.local_port, bcmVlanPortDoubleLookupEnable,
                                         &is_pon_dtc_enable));
                    }
                }

                if (!is_pon_dtc_enable)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "BCM_PORT_MATCH_PORT_XVLAN_STACKED (%d) match for PON AC is not supported. flags = 0x%08X\n",
                                 match_info->match, match_info->flags);
                }
                /*
                 * Verify match_vlan and match_inner_vlan are in range:
                 */
                BCM_DNX_VLAN_CHK_ID(unit, match_info->match_vlan);
                BCM_DNX_VLAN_CHK_ID(unit, match_info->match_inner_vlan);
            }
                break;

            case BCM_PORT_MATCH_PORT_PCP_VLAN:

                if (is_native_ac)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "BCM_PORT_MATCH_PORT_PCP_VLAN (%d) match for native AC is not supported. flags = 0x%08X\n",
                                 BCM_PORT_MATCH_PORT_PCP_VLAN, match_info->flags);
                }

                /*
                 * Verify match_vlan and match_pcp are in range:
                 */
                BCM_DNX_VLAN_CHK_ID(unit, match_info->match_vlan);
                BCM_DNX_PCP_CHK_ID(unit, match_info->match_pcp);
                break;

            case BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED:

                if (is_native_ac)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED (%d) match for native AC is not supported. flags = 0x%08X\n",
                                 BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED, match_info->flags);
                }

                /*
                 * Verify match_vlan match_pcp and match_inner_vlan are in range:
                 */
                BCM_DNX_VLAN_CHK_ID(unit, match_info->match_vlan);
                BCM_DNX_VLAN_CHK_ID(unit, match_info->match_inner_vlan);
                BCM_DNX_PCP_CHK_ID(unit, match_info->match_pcp);
                break;

            case BCM_PORT_MATCH_PORT_UNTAGGED:

                if (is_native_ac)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "BCM_PORT_MATCH_PORT_UNTAGGED (%d) match for native AC is not supported. flags = 0x%08X\n",
                                 BCM_PORT_MATCH_PORT_UNTAGGED, match_info->flags);
                }

                break;

            default:

                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Ingress unsupported match. match = %d, only BCM_PORT_MATCH_PORT(%d), BCM_PORT_MATCH_PORT_VLAN(%d),"
                             "BCM_PORT_MATCH_PORT_VLAN_STACKED(%d), BCM_PORT_MATCH_PORT_CVLAN(%d), BCM_PORT_MATCH_PORT_PCP_VLAN(%d), "
                             "BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED(%d), BCM_PORT_MATCH_PORT_CVLAN_STACKED(%d), "
                             "BCM_PORT_MATCH_PORT_SVLAN_STACKED(%d), BCM_PORT_MATCH_PORT_UNTAGGED (%d), "
                             "BCM_PORT_MATCH_PORT_PON_TUNNEL(%d), BCM_PORT_MATCH_PORT_PON_TUNNEL_VLAN (%d), "
                             "BCM_PORT_MATCH_PORT_PON_TUNNEL_CVLAN(%d), BCM_PORT_MATCH_PORT_PON_TUNNEL_VLAN_STACKED (%d), "
                             "BCM_PORT_MATCH_PORT_PON_TUNNEL_CVLAN_STACKED(%d), BCM_PORT_MATCH_PORT_PON_TUNNEL_SVLAN_STACKED (%d), "
                             "BCM_PORT_MATCH_PORT_PON_TUNNEL_UNTAGGED(%d) are supported\n",
                             match_info->match, BCM_PORT_MATCH_PORT, BCM_PORT_MATCH_PORT_VLAN,
                             BCM_PORT_MATCH_PORT_VLAN_STACKED, BCM_PORT_MATCH_PORT_CVLAN, BCM_PORT_MATCH_PORT_PCP_VLAN,
                             BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED, BCM_PORT_MATCH_PORT_SVLAN_STACKED,
                             BCM_PORT_MATCH_PORT_CVLAN_STACKED, BCM_PORT_MATCH_PORT_UNTAGGED,
                             BCM_PORT_MATCH_PORT_PON_TUNNEL, BCM_PORT_MATCH_PORT_PON_TUNNEL_VLAN,
                             BCM_PORT_MATCH_PORT_PON_TUNNEL_CVLAN, BCM_PORT_MATCH_PORT_PON_TUNNEL_VLAN_STACKED,
                             BCM_PORT_MATCH_PORT_PON_TUNNEL_CVLAN_STACKED, BCM_PORT_MATCH_PORT_PON_TUNNEL_SVLAN_STACKED,
                             BCM_PORT_MATCH_PORT_PON_TUNNEL_UNTAGGED);
                break;
        }
    }
    else
    {
        /*
         * match criteria for egress:
         */
        switch (match_info->match)
        {
            case BCM_PORT_MATCH_PORT:
            {
                if (BCM_GPORT_IS_SET(port))
                {
                    uint8 is_phy_port = FALSE;

                    /*
                     * In case of adding esem default result to a port/LIF,
                     * 'port' should be virtual gport for esem default result profile.
                     * 'match_info->port' must be gport for vlan_port or extender_port, mpls_port, or
                     * a physical port/lag gport.
                     */
                    if (((BCM_GPORT_IS_VLAN_PORT(port) || (BCM_GPORT_IS_EXTENDER_PORT(port))) &&
                         BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(port)))
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, match_info->port, &is_phy_port));
                        if (is_phy_port == FALSE)
                        {
                            /** port should be a L2 GPORT: vlan_port, extender_port, mpls_port.*/
                            if (!DNX_GPORT_IS_L2_GPORT(match_info->port))
                            {
                                SHR_ERR_EXIT(_SHR_E_PARAM,
                                             "In case of adding esem default result to out-lif, Match Port = 0x%08X "
                                             "should be vlan/extender/mpls port or mpls tunnel for EVPN.\n",
                                             match_info->port);
                            }
                        }
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "in case of adding esem default result to port/LIF, Port = 0x%08X should be with subtype of virtual egress default.\n",
                                     port);
                    }
                }
                else
                {
                    /*
                     * Add esem match entries to physical port, port should be virtual gport.
                     * So, 'port' must be a gport for virtual gport or outlif-id
                     */
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Port = 0x%08X should not be a physical port.\n", port);
                }
                break;
            }
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Egress unsupported match. match = %d, only BCM_PORT_MATCH_PORT(%d) is supported\n",
                             match_info->match, BCM_PORT_MATCH_PORT);
                break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API bcm_dnx_port_match_set
 */
static shr_error_e
dnx_port_match_set_verify(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_port_match_info_t * match_array)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(match_array, _SHR_E_PARAM, "match_array");

    SHR_MAX_VERIFY(1, size, _SHR_E_PARAM, "size must be higher than 0");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API bcm_dnx_port_match_multi_get
 */
static shr_error_e
dnx_port_match_multi_get_verify(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_port_match_info_t * match_array,
    int *count)
{
    int esem_default_result_profile;

    SHR_FUNC_INIT_VARS(unit);

    /** Check if 'size' and 'match_array' matches.*/
    if (size > 0)
    {
        SHR_NULL_CHECK(match_array, _SHR_E_PARAM, "match_array must be a valid pointer in case of size > 0.");
    }
    else if (size < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "size (%d) should not be less than 0!\n", size);
    }

    /** Check if 'port' is supported.*/
    if (!BCM_GPORT_IS_VLAN_PORT(port) || BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT,
                     "port (0x%08X) is not supported. Only vlan-port and not VIRTUAL_EGRESS_MATCH is supported!\n",
                     port);
    }

    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(port))
    {
        /** 'port' mustn't be predefined default esem result profile which is referred to by all port/lifs.*/
        esem_default_result_profile = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(port);
        if (esem_default_result_profile >= dnx_data_esem.default_result_profile.nof_allocable_profiles_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PORT, "port (0x%08X) for default esem result profile is illegal!\n", port);
        }
    }

    /** 'count' mustn't be NULL.*/
    SHR_NULL_CHECK(count, _SHR_E_PARAM, "count must be a valid pointer!");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API bcm_dnx_port_match_delete_all
 */
static shr_error_e
dnx_port_match_delete_all_verify(
    int unit,
    bcm_gport_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if 'port' is supported.*/
    if (!BCM_GPORT_IS_VLAN_PORT(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "port (0x%08X) is not supported. Only vlan port is supported!\n", port);
    }
    else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "port (0x%08X) should be deleted by bcm_vlan_port_destroy!\n", port);
    }
    else
    {
        bcm_vlan_port_t vlan_port;
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.vlan_port_id = port;
        SHR_IF_ERR_EXIT(bcm_dnx_vlan_port_find(unit, &vlan_port));

        if (_SHR_IS_FLAG_SET(vlan_port.flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY)
            && !_SHR_IS_FLAG_SET(vlan_port.flags, BCM_VLAN_PORT_DEFAULT))
        {
            SHR_ERR_EXIT(_SHR_E_PORT, "port (0x%08X) egress matches should be deleted by bcm_vlan_port_destroy!\n",
                         port);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert the match enum to the equivalent VLAN-Port criteria enum
 *   \param [in] unit - unit id
 *   \param [in] match - The match enum value to convert
 *   \param [out] criteria - A pointer to the converted VLAN-Port enum value
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   dnx_port_match_ingress_match_to_vlan_port_match_convert
 */
static shr_error_e
dnx_port_match_ingress_match_val_to_criteria_convert(
    int unit,
    bcm_port_match_t match,
    bcm_vlan_port_match_t * criteria)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Convert the match enum to the equivalent VLAN-Port criteria enum
     */
    switch (match)
    {
        case BCM_PORT_MATCH_PORT:
            *criteria = BCM_VLAN_PORT_MATCH_PORT;
            break;
        case BCM_PORT_MATCH_PORT_VLAN:
            *criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            break;
        case BCM_PORT_MATCH_PORT_CVLAN:
            *criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
            break;
        case BCM_PORT_MATCH_PORT_VLAN_STACKED:
            *criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
            break;
        case BCM_PORT_MATCH_PORT_CVLAN_STACKED:
            *criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN_STACKED;
            break;
        case BCM_PORT_MATCH_PORT_SVLAN_STACKED:
            *criteria = BCM_VLAN_PORT_MATCH_PORT_SVLAN_STACKED;
            break;
        case BCM_PORT_MATCH_PORT_PCP_VLAN:
            *criteria = BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN;
            break;
        case BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED:
            *criteria = BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED;
            break;
        case BCM_PORT_MATCH_PORT_UNTAGGED:
            *criteria = BCM_VLAN_PORT_MATCH_PORT_UNTAGGED;
            break;
        case BCM_PORT_MATCH_PORT_PON_TUNNEL:
            *criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
            break;
        case BCM_PORT_MATCH_PORT_PON_TUNNEL_VLAN:
            *criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN;
            break;
        case BCM_PORT_MATCH_PORT_PON_TUNNEL_CVLAN:
            *criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN;
            break;
        case BCM_PORT_MATCH_PORT_PON_TUNNEL_VLAN_STACKED:
            *criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED;
            break;
        case BCM_PORT_MATCH_PORT_PON_TUNNEL_CVLAN_STACKED:
            *criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN_STACKED;
            break;
        case BCM_PORT_MATCH_PORT_PON_TUNNEL_SVLAN_STACKED:
            *criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_SVLAN_STACKED;
            break;
        case BCM_PORT_MATCH_PORT_PON_TUNNEL_UNTAGGED:
            *criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_UNTAGGED;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error, unsupport macth value - %d\n", match);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert the port match struct to the a lookup as
 * part of the VLAN-Port struct in order to later use VLAN-Port
 * functions.
 *   \param [in] unit - unit id
 *   \param [in] gport - The gport that is pointed by the match
 *          info.
 *   \param [in] port_match - Pointer to the port match struct
 *          to convert.
 *   \param [out] vlan_port - The result VLAN-Port struct.
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   bcm_dnx_port_match_add
 *   dnx_port_match_inlif_update
 */
static shr_error_e
dnx_port_match_ingress_match_to_vlan_port_match_convert(
    int unit,
    bcm_gport_t gport,
    bcm_port_match_info_t * port_match,
    bcm_vlan_port_t * vlan_port)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Copy the relevant fields from the match struct to the VLAN-Port struct
     */
    vlan_port->vlan_port_id = gport;
    vlan_port->port = port_match->port;
    vlan_port->match_vlan = port_match->match_vlan;
    vlan_port->match_inner_vlan = port_match->match_inner_vlan;
    vlan_port->match_ethertype = port_match->match_ethertype;
    vlan_port->match_pcp = port_match->match_pcp;
    vlan_port->match_tunnel_value = port_match->match_pon_tunnel;
    vlan_port->flags = 0;
    vlan_port->flags |= (_SHR_IS_FLAG_SET(port_match->flags, BCM_PORT_MATCH_NATIVE)) ? BCM_VLAN_PORT_NATIVE : 0;

    /*
     * Convert the match enum to the equivalent VLAN-Port criteria enum
     */
    SHR_IF_ERR_EXIT(dnx_port_match_ingress_match_val_to_criteria_convert
                    (unit, port_match->match, &(vlan_port->criteria)));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Ingress - get vlan_domain and check whether in_lif_profile is LIF scope
*/
shr_error_e
dnx_ingress_native_ac_interface_namespace_check(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    uint32 *vlan_domain,
    uint32 *is_intf_namespace)
{
    uint32 in_lif_profile = 0;
    in_lif_profile_info_t in_lif_profile_info;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get in_lif_profile and vlan_domain
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    if (vlan_domain)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, INST_SINGLE, vlan_domain));
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, &in_lif_profile));

    /** Get in_lif_porfile data: */
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, LIF));

    *is_intf_namespace =
        _SHR_IS_FLAG_SET(in_lif_profile_info.ingress_fields.cs_in_lif_profile_flags,
                         DNX_IN_LIF_PROFILE_CS_PROFILE_PWE_LIF_SCOPED);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get ESEM access key type, aka app-db-id, according to port match criteria.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] match_info - Port match information.
 * \param [out] esem_app_db - Esem access app-db-id.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
static shr_error_e
dnx_port_match_criteria_to_esem_app_db_get(
    int unit,
    bcm_port_match_info_t * match_info,
    dbal_enum_value_field_esem_app_db_id_e * esem_app_db)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(esem_app_db, _SHR_E_INTERNAL, "A valid pointer for esem_app_db is needed!\n");

    /** Init esem-app-db-id */
    *esem_app_db = DBAL_ENUM_FVAL_ESEM_APP_DB_ID_FODO_NAMESPACE_CTAG;

    /** Assign esem-app-db-id according to criteria */
    switch (match_info->match)
    {
        case BCM_PORT_MATCH_PORT:
        {
            *esem_app_db = DBAL_ENUM_FVAL_ESEM_APP_DB_ID_FODO_NAMESPACE_CTAG;
            if (_SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_NATIVE))
            {
                *esem_app_db = DBAL_ENUM_FVAL_ESEM_APP_DB_ID_FODO_OUTLIF;
            }
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unavailable criteria(%d) in ESEM!\n", match_info->match);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Set an esem access command to a port or LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - physical port, or out-lif-id in gport format.
 * \param [in] esem_cmd - esem access command that should be linked to the port
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
static shr_error_e
dnx_port_match_esem_cmd_port_set(
    int unit,
    bcm_gport_t port,
    int esem_cmd)
{
    uint8 is_phy_port;
    uint32 entry_handle_id, pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Link the cmd to port/LIF
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
    if (is_phy_port == FALSE)
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        uint32 flags;

        /** Get the local-out-lif.*/
        flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, port, flags, &gport_hw_resources),
                                    _SHR_E_NOT_FOUND, _SHR_E_PARAM);

        /** Update the esem-cmd in OUT-LIF table.*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources.local_out_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources.outlif_dbal_result_type);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE, esem_cmd);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        dnx_algo_gpm_gport_phy_info_t gport_info;

        /** Get the pp-port.*/
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        /** Update the esem-cmd in EGR_PORT table.*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                       gport_info.internal_port_pp_info.core_id[pp_port_index]);
            dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                       gport_info.internal_port_pp_info.pp_port[pp_port_index]);
            dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_CMD, INST_SINGLE, esem_cmd);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_port_match_esem_cmd_port_get(
    int unit,
    bcm_gport_t port,
    int *esem_cmd)
{
    uint8 is_phy_port;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(esem_cmd, _SHR_E_INTERNAL, "A valid pointer for receive the original esem access cmd is needed!");

    /*
     * Retrieve the old esem-cmd from port/LIF
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
    if (is_phy_port == FALSE)
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        uint32 flags;
        int res = _SHR_E_NONE;

        /** Get the local-out-lif.*/
        flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, port, flags, &gport_hw_resources),
                                    _SHR_E_NOT_FOUND, _SHR_E_PARAM);

        /** Get the old esem-cmd.*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources.local_out_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources.outlif_dbal_result_type);
        res = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
        if (res == _SHR_E_NOT_FOUND)
        {
            *esem_cmd = dnx_data_esem.access_cmd.no_action_get(unit);
        }
        else
        {
            SHR_IF_ERR_EXIT(res);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE, (uint32 *) esem_cmd));
        }
    }
    else
    {
        dnx_algo_gpm_gport_phy_info_t gport_info;

        /** Get the pp-port.*/
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        /** Get the old esem-cmd.*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[0]);
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[0]);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_CMD, INST_SINGLE, (uint32 *) esem_cmd);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Exchange a ESEM access command and update the port or LIF with it.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - virtual gport representing a ESEM default profile.
 * \param [in] match_info - pointer to the match information.
 * \param [in] add_or_delete - Is add or delete operation, see dnx_port_match_esem_cmd_oper_e.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
static shr_error_e
dnx_port_match_esem_cmd_update(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info,
    dnx_port_match_esem_cmd_oper_e add_or_delete)
{
    int esem_default_result_profile;
    int new_esem_cmd, old_esem_cmd;
    uint8 is_last = 0, is_first = 0;
    uint32 esem_cmd_flags;
    dnx_esem_access_if_t esem_if;
    dnx_esem_cmd_data_t esem_cmd_data;
    dnx_esem_access_type_t esem_access_type, old_access_type;
    dbal_enum_value_field_esem_app_db_id_e esem_app_db;
    dnx_port_esem_cmd_info_t esem_cmd_info;
    dnx_esem_cmd_suffix_size_t esem_cmd_size = DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_MAX;
    uint32 prefix = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    esem_cmd_info.nof_accesses = 0;

    /** In case of default_result_profile is provided (must be), use it.*/
    esem_default_result_profile = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(port);

    /** Get esem access type according to default result entry */
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_type_get(unit, esem_default_result_profile, &esem_access_type));

    /** Get esem access app_db_id according to match criteria */
    SHR_IF_ERR_EXIT(dnx_port_match_criteria_to_esem_app_db_get(unit, match_info, &esem_app_db));

    /*
     * 1) Get the original ESEM_ACC_CMND
     */
    SHR_IF_ERR_EXIT(dnx_port_match_esem_cmd_port_get(unit, match_info->port, &old_esem_cmd));

    /** Get the original ESEM_ACC_CMND profile data*/
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_sw_get(unit, old_esem_cmd, &esem_cmd_data, NULL));

    /** Get the ESEM_ACC_CMND interface*/
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_info_get(unit, 0, esem_access_type, &esem_if, NULL, NULL));

    /** In case of deleting, Checking whether the given ESEM default profile is in use*/
    if (add_or_delete == DNX_PORT_ESEM_CMD_DEL)
    {
        int old_esem_handle;

        old_esem_handle = esem_cmd_data.esem[esem_if].default_result_profile;

        if (old_esem_handle != esem_default_result_profile)
        {
            BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_SET(old_esem_handle, old_esem_handle);
            BCM_GPORT_VLAN_PORT_ID_SET(old_esem_handle, old_esem_handle);

            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The default AC gport (0x%08X) to remove doesn't match with the one (0x%08X) in use by the port (%d)!\n",
                         port, old_esem_handle, match_info->port);
        }

        /** Reset the default profile to predefined default entry*/
        esem_default_result_profile = (esem_access_type == ESEM_ACCESS_TYPE_ETH_AC) ?
            dnx_data_esem.default_result_profile.default_ac_get(unit) :
            (esem_access_type == ESEM_ACCESS_TYPE_ETH_NATIVE_AC) ?
            dnx_data_esem.default_result_profile.default_native_get(unit) :
            dnx_data_esem.default_result_profile.default_dual_homing_get(unit);
    }

    /*
     * 2) Allocate ESEM_ACC_CMND by exchanging
     *    For add, esem_default_result_profile is from input;
     *    for delete, it is reset to default predefined.
     */

    /** We suppose to allocate a new ESEM cmd with an access. It may be updated later. */
    esem_cmd_flags = 0;

    /** Check the original cmd and decide to either update the default profile or add new access.*/
    if (old_esem_cmd != dnx_data_esem.access_cmd.no_action_get(unit))
    {
        if (esem_cmd_data.esem[esem_if].valid == TRUE)
        {
            /** Update the existing cmd with the given default profile.*/
            SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_type_get(unit,
                                                              esem_cmd_data.esem[esem_if].default_result_profile,
                                                              &old_access_type));
            /**Check if the access type matches*/
            if (old_access_type != esem_access_type)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "The access types(%d, %d) don't match between the given and orignal default result profile!\n",
                             esem_access_type, old_access_type);
            }
            else
            {
                /** Using the original app_db_id and access_type, only update the default profile.*/
                esem_app_db = esem_cmd_data.esem[esem_if].app_db_id;
            }
        }

        esem_cmd_flags = DNX_PORT_ESEM_CMD_UPDATE;
    }

    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_add
                    (unit, 0, esem_access_type, esem_app_db, esem_default_result_profile, &esem_cmd_info));

    /** Get current esem cmd size and prefix */
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_size_and_prefix_get(unit, match_info->port, &esem_cmd_size, &prefix));

    /** Allocate an esem access cmd by exchanging.*/
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_exchange
                    (unit, esem_cmd_flags, esem_cmd_size, prefix, &esem_cmd_info,
                     old_esem_cmd, &new_esem_cmd, &esem_cmd_data, &is_first, &is_last));

    /*
     * 3)Connect Port->ESEM_ACC_CMND->AC_PROFILE
     */
    if (is_first == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_set(unit, new_esem_cmd, esem_cmd_data));
    }
    SHR_IF_ERR_EXIT(dnx_port_match_esem_cmd_port_set(unit, match_info->port, new_esem_cmd));

    /*
     * 4) Clear the old cmd entry in hardware if necessary
     */
    if ((is_last == TRUE) && (new_esem_cmd != old_esem_cmd))
    {
        /** Remove the old attributes from esem-cmd table in the old cmd-id*/
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_clear(unit, old_esem_cmd));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Exchange an ESEM access cmd and link the default esem entry to the given
 *  port/LIF by the cmd.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - virtual gport representing a ESEM default profile.
 * \param [in] match_info - pointer to the match information.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  Configure VLAN PORT default egress configuration(egress vlan encapsulation)
 *  per port by configuring tables that map the following:
 *  PORT ->ESEM_ACCESS_CMD -> AC_Profile -> AC info
 *
 * \see
 *  dnx_vlan_port_egress_match_port_create
 *  dnx_port_match_esem_access_cmd_alloc
 */
static shr_error_e
dnx_port_match_egress_match_port_add(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * This code is handling Default port match on egress.
     * bcm_vlan_port_create allocates AC_PROFILE, returns allocated id
     * bcm_port_match_add will get allocated ac_profile as parameter (through gport) and
     * 1. Allocate ESEM_ACC_CMND
     * 2. Connect Port->ESEM_ACC_CMND->AC_PROFILE
     */

    /*
     * Allocate esem cmd with the new default profile and connect it to port
     */
    SHR_IF_ERR_EXIT(dnx_port_match_esem_cmd_update(unit, port, match_info, DNX_PORT_ESEM_CMD_ADD));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Link an esem entry pointed by virtual gport to a port or LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - physical port, virtual port or out-lif-id in gport format.
 * \param [in] match_info - pointer to the match information.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  The ESEM match entry and its virtual gport should be created by other APIs
 *  per corresponding applications, such as for ac, they should be created by
 *  bcm_vlan_port_create with the flag of BCM_VLAN_PORT_VLAN_TRANSLATION.
 *  Note that the creation won't link the ESEM match entry to port/LIF except
 *  for port default cases. User need to call bcm_port_match_add for the linkage.
 *  In a word, bcm_vlan_port_create create ESEM match entry or ESEM default entry,
 *  and bcm_port_match_add link the entry to port/LIF. Two type of virtual gport
 *  is supported.
 *  if the virtual gport is an ESEM match entry:
 *     Same match criteria and paramters should be set in match_info to ensure
 *     the exact ESEM match entry is linked.
 *     'port' here is the target to attach the entry.
 *  if the virtual gport is an ESEM default profile:
 *     match_info->port is the target to attach the entry.
 *     'port' here is the virtual gport representing an ESEM default profile.
 *
 * \see
 *  None
 */
static shr_error_e
dnx_port_match_egress_add(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (match_info->match)
    {
        case BCM_PORT_MATCH_PORT:
        {
            SHR_IF_ERR_EXIT(dnx_port_match_egress_match_port_add(unit, port, match_info));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported match criteria(%d), only BCM_PORT_MATCH_PORT is supported\n",
                         match_info->match);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Delete the default esem entry from the given port/LIF and free the ESEM access cmd.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - virtual gport representing a ESEM default profile.
 * \param [in] match_info - pointer to the match information.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  dnx_port_match_egress_match_port_add
 */
static shr_error_e
dnx_port_match_egress_match_port_delete(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Allocate esem cmd with the default AC profile and connect it to port
     */
    SHR_IF_ERR_EXIT(dnx_port_match_esem_cmd_update(unit, port, match_info, DNX_PORT_ESEM_CMD_DEL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  delete an esem entry pointed by virtual gport from a port or LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - physical port, virtual port or out-lif-id in gport format.
 * \param [in] match_info - pointer to the match information.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  dnx_port_match_egress_add
 */
static shr_error_e
dnx_port_match_egress_delete(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (match_info->match)
    {
        case BCM_PORT_MATCH_PORT:
        {
            SHR_IF_ERR_EXIT(dnx_port_match_egress_match_port_delete(unit, port, match_info));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported match criteria(%d), only BCM_PORT_MATCH_PORT is supported\n",
                         match_info->match);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get multiple match objects according to the given virtual egress default sub-type
 *  of gport.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 * \param [in] size - Number of elements in match array.
 * \param [out] match_array - Match array.
 * \param [out] count - Number of valid match objects found.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *  bcm_dnx_port_match_multi_get
 */
static shr_error_e
dnx_port_match_multi_get_virtual_egress_default(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_port_match_info_t * match_array,
    int *count)
{
    int rv;
    int esem_default_profile;
    int esem_cmd, port_esem_cmd, nof_expected_esem_cmds = 0;
    int ref_count = 0, total_ref_count = 0;
    int *esem_expected_cmds = NULL;
    dnx_esem_cmd_data_t esem_cmd_data;
    SW_STATE_HASH_TABLE_ITER iter;
    egress_lif_info_t egress_lif_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init the output
     */
    *count = 0;

    /*
     * Look for the esem cmds that are using the default profile.
     */
    esem_default_profile = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(port);
    esem_expected_cmds =
        sal_alloc(sizeof(int) * dnx_data_esem.access_cmd.nof_cmds_get(unit), "Array for expected esem cmds.");
    if (esem_expected_cmds == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Alloc memory failed\n");
    }

    for (esem_cmd = 0; esem_cmd < dnx_data_esem.access_cmd.nof_cmds_get(unit); esem_cmd++)
    {
        rv = dnx_port_esem_cmd_data_sw_get(unit, esem_cmd, &esem_cmd_data, &ref_count);
        if (rv == _SHR_E_NOT_FOUND)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(rv);

        /** Check if the result profile was used by the cmd in any access interface.*/
        if ((esem_cmd_data.esem[0].valid && esem_cmd_data.esem[0].default_result_profile == esem_default_profile) ||
            (esem_cmd_data.esem[1].valid && esem_cmd_data.esem[1].default_result_profile == esem_default_profile) ||
            (esem_cmd_data.esem[2].valid && esem_cmd_data.esem[2].default_result_profile == esem_default_profile))
        {
            esem_expected_cmds[nof_expected_esem_cmds] = esem_cmd;
            nof_expected_esem_cmds++;

            total_ref_count += ref_count;
            ref_count = 0;
        }
    }

    /*
     * Look for the port/LIF that are using these esem cmds.
     */
    if (size == 0)
    {
        /** Return the only number of port/LIFs that are using these esem cmds*/
        *count = total_ref_count;
    }
    else
    {
        bcm_port_config_t port_info_config;
        dnx_algo_gpm_gport_phy_info_t gport_info;
        bcm_vlan_port_t vlan_port;
        uint32 match_flags, lif_flags;
        int local_outlif_id = 0;
        uint8 has_esem_cmd;

        /** Prepare match info*/
        *count = 0;

        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.vlan_port_id = port;
        SHR_IF_ERR_EXIT(bcm_vlan_port_find(unit, &vlan_port));
        match_flags = BCM_PORT_MATCH_EGRESS_ONLY;
        if (_SHR_IS_FLAG_SET(vlan_port.flags, BCM_VLAN_PORT_NATIVE))
        {
            match_flags |= BCM_PORT_MATCH_NATIVE;
        }

        /** lif flags for translating local lif to gport*/
        lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT;

        /** Iterate all pp ports*/
        SHR_IF_ERR_EXIT(bcm_dnx_port_config_get(unit, &port_info_config));
        _SHR_PBMP_ITER(port_info_config.e, port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                            (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &gport_info));

            if (gport_info.internal_port_pp_info.nof_pp_ports == 0)
            {
                /** The local port is a lag member, skip it.*/
                continue;
            }

            SHR_IF_ERR_EXIT(dnx_port_match_esem_cmd_port_get(unit, port, &port_esem_cmd));
            for (esem_cmd = 0; esem_cmd < nof_expected_esem_cmds; esem_cmd++)
            {
                if (esem_expected_cmds[esem_cmd] == port_esem_cmd)
                {
                    (*count)++;
                    match_array[*count - 1].port = port;
                    match_array[*count - 1].flags = match_flags;
                    match_array[*count - 1].match = BCM_PORT_MATCH_PORT;
                    break;
                }
            }

            if (*count > size)
            {
                /** We have got enough objects, exit.*/
                break;
            }
        }

        /** Iterate all allocated local-out-lif */
        DNX_LIF_MNGR_OUTLIF_SW_INFO_ITER_START(&iter);
        DNX_LIF_MNGR_OUTLIF_SW_INFO_ITER_NEXT(unit, &iter, &local_outlif_id, &egress_lif_info);

        while ((!DNX_LIF_MNGR_OUTLIF_SW_INFO_ITER_IS_END(&iter)) && (*count <= size))
        {
            if ((egress_lif_info.dbal_table_id != DBAL_TABLE_EEDB_PWE) &&
                (egress_lif_info.dbal_table_id != DBAL_TABLE_EEDB_EVPN) &&
                (egress_lif_info.dbal_table_id != DBAL_TABLE_EEDB_MPLS_TUNNEL) &&
                (egress_lif_info.dbal_table_id != DBAL_TABLE_EEDB_IPV4_TUNNEL) &&
                (egress_lif_info.dbal_table_id != DBAL_TABLE_EEDB_IPV6_TUNNEL))
            {
                DNX_LIF_MNGR_OUTLIF_SW_INFO_ITER_NEXT(unit, &iter, &local_outlif_id, &egress_lif_info);
                continue;
            }

            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_valid_field(unit, _SHR_CORE_ALL,
                                                              local_outlif_id, egress_lif_info.dbal_table_id, FALSE,
                                                              DBAL_FIELD_ESEM_COMMAND, &has_esem_cmd));
            if (has_esem_cmd)
            {
                /** translate the local-outlif-id to gport */
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit, lif_flags, _SHR_CORE_ALL, local_outlif_id, &port));

                /** Get the esem cmd from the lif entry */
                SHR_IF_ERR_EXIT(dnx_port_match_esem_cmd_port_get(unit, port, &port_esem_cmd));

                for (esem_cmd = 0; esem_cmd < nof_expected_esem_cmds; esem_cmd++)
                {
                    if (esem_expected_cmds[esem_cmd] == port_esem_cmd)
                    {
                        (*count)++;
                        match_array[*count - 1].port = port;
                        match_array[*count - 1].flags = match_flags;
                        match_array[*count - 1].match = BCM_PORT_MATCH_PORT;
                        break;
                    }
                }
            }

            /*
             * Get the LIF info for the next local Out-LIF
             */
            DNX_LIF_MNGR_OUTLIF_SW_INFO_ITER_NEXT(unit, &iter, &local_outlif_id, &egress_lif_info);
        }
    }

exit:
    if (esem_expected_cmds != NULL)
    {
        sal_free(esem_expected_cmds);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get multiple match objects according to the given gport.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 * \param [in] size - Number of elements in match array.
 * \param [out] match_array - Match array.
 * \param [out] count - Number of valid match objects found.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *  bcm_dnx_port_match_multi_get
 */
static shr_error_e
dnx_port_match_multi_get_ingress(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_port_match_info_t * match_array,
    int *count)
{
    uint32 entry_handle_id;
    int is_end = 0;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 flags;
    uint32 match_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Init the output
     */
    *count = 0;

    /*
     * Get the port match flags
     */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                 &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    if (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB)
    {
        flags = BCM_PORT_MATCH_INGRESS_ONLY;
    }
    else
    {
        flags = BCM_PORT_MATCH_INGRESS_ONLY | BCM_PORT_MATCH_NATIVE;
    }

    /*
     * Iterate the SW table to find all match objects
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VLAN_PORT_MATCH_KEY_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_GPORT, 0, DBAL_CONDITION_EQUAL_TO, (uint32 *) &port, NULL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end && ((*count < size) || (size == 0)))
    {
        /** Store the match_info per 'size'*/
        if (size != 0)
        {
            bcm_port_match_info_t_init(match_array + *count);
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_CRITERIA, &match_value));
            match_array[*count].match = match_value;
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_MATCH_PORT, &match_value));
            match_array[*count].port = match_value;
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_ETH_TYPE, &match_value));
            match_array[*count].match_ethertype = match_value;
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, &match_value));
            match_array[*count].match_pcp = match_value;
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, &match_value));
            match_array[*count].match_vlan = match_value;
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, &match_value));
            match_array[*count].match_inner_vlan = match_value;
            match_array[*count].flags = flags;
        }

        /** Increase the count*/
        (*count)++;

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Deleting all ingress matches added by bcm_port_match_add of an existing port.
 * It removes entries from the correct ISEM table which points to the LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   Both outer and native ingress AC matches are deleted.
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_port_match_ingress_delete_all(
    int unit,
    bcm_gport_t port)
{
    uint32 entry_handle_id;
    int is_end = 0;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_port_match_info_t match_info;
    uint32 flags;
    uint32 match_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Check if it is NATIVE per the given port (result of match entry)*/
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                 &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    if (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB)
    {
        flags = BCM_PORT_MATCH_INGRESS_ONLY;
    }
    else
    {
        flags = BCM_PORT_MATCH_INGRESS_ONLY | BCM_PORT_MATCH_NATIVE;
    }

    /** Iterate SW table to find and delete all match entries that point to the given port.*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VLAN_PORT_MATCH_KEY_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_GPORT, 0, DBAL_CONDITION_EQUAL_TO, (uint32 *) &port, NULL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        bcm_port_match_info_t_init(&match_info);
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_CRITERIA, &match_value));
        match_info.match = match_value;
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_MATCH_PORT, &match_value));
        match_info.port = match_value;
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_ETH_TYPE, &match_value));
        match_info.match_ethertype = match_value;
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, &match_value));
        match_info.match_pcp = match_value;
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, &match_value));
        match_info.match_vlan = match_value;
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, &match_value));
        match_info.match_inner_vlan = match_value;
        match_info.flags = flags;

        SHR_IF_ERR_EXIT(bcm_dnx_port_match_delete(unit, port, &match_info));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Deleting all egress matches added by bcm_port_match_add of an existing port.
 * It resets all LIFs that are using the virtual default AC with a global default value.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None.
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_port_match_egress_virtual_default_delete_all(
    int unit,
    bcm_gport_t port)
{
    int nof_matches, entry_idx;
    bcm_port_match_info_t *match_array = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get number of entries firstly.*/
    SHR_IF_ERR_EXIT(bcm_dnx_port_match_multi_get(unit, port, 0, NULL, &nof_matches));

    if (nof_matches != 0)
    {
        /** Alloc memory for all posiible entries*/
        match_array = sal_alloc(sizeof(bcm_port_match_info_t) * nof_matches, "Array for match entries.");
        if (match_array == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Alloc memory failed\n");
        }

        /** Get all match entries.*/
        SHR_IF_ERR_EXIT(bcm_dnx_port_match_multi_get(unit, port, nof_matches, match_array, &nof_matches));

        /** Delete the entries one by one.*/
        for (entry_idx = 0; entry_idx < nof_matches; entry_idx++)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_port_match_delete(unit, port, (match_array + entry_idx)));
        }
    }

exit:
    if (match_array != NULL)
    {
        sal_free(match_array);
    }
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set ingress match key per match criteria.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] entry_handle_id - entry handle id used by the dbal table.
 * \param [in] match_info - match information.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None.
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_port_match_ingress_match_key_set(
    int unit,
    uint32 entry_handle_id,
    bcm_port_match_info_t * match_info)
{
    SHR_FUNC_INIT_VARS(unit);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRITERIA, match_info->match);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MATCH_PORT, match_info->port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETH_TYPE, match_info->match_ethertype);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, 0);

    switch (match_info->match)
    {
             /* coverity[unterminated_case:FALSE]  */
        case BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED:
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, match_info->match_pcp);
        }
        case BCM_PORT_MATCH_PORT_VLAN_STACKED:
        case BCM_PORT_MATCH_PORT_SVLAN_STACKED:
             /* coverity[unterminated_case:FALSE]  */
        case BCM_PORT_MATCH_PORT_CVLAN_STACKED:
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, match_info->match_inner_vlan);
        }
        case BCM_PORT_MATCH_PORT_VLAN:
        case BCM_PORT_MATCH_PORT_CVLAN:
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, match_info->match_vlan);

            break;
        }
        case BCM_PORT_MATCH_PORT_PCP_VLAN:
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, match_info->match_vlan);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, match_info->match_pcp);

            break;
        }
        case BCM_PORT_MATCH_PORT:
        case BCM_PORT_MATCH_PORT_UNTAGGED:
            break;
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Ingress unsupported match. match = %d, only BCM_PORT_MATCH_PORT, BCM_PORT_MATCH_PORT_VLAN, "
                         "BCM_PORT_MATCH_PORT_PCP_VLAN,BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED"
                         "BCM_PORT_MATCH_PORT_CVLAN, BCM_PORT_MATCH_PORT_VLAN_STACKED and BCM_PORT_MATCH_PORT_UNTAGGED are supported\n",
                         match_info->match);
        }
    }

    SHR_IF_ERR_EXIT(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Add the match info to SW table.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 * \param [in] match_info - pointer to match information.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   Call it after adding the match information to HW. So that if the match information
 *   exists already, it won't be added to SW table too.
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_port_match_ingress_match_info_sw_add(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    uint32 entry_handle_id;
    int rv = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VLAN_PORT_MATCH_KEY_SW, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_port_match_ingress_match_key_set(unit, entry_handle_id, match_info));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, port);
    rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);

    /** Add the same match_port entry multiple times, No error should return as HW.*/
    if ((match_info->match == BCM_PORT_MATCH_PORT) && (match_info->match_ethertype == 0) && (rv == _SHR_E_EXISTS))
    {
        rv = _SHR_E_NONE;
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Delete the match info from SW table.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 * \param [in] match_info - pointer to match information.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None.
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_port_match_ingress_match_info_sw_delete(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    uint32 entry_handle_id;
    int rv = _SHR_E_NONE;
    uint32 is_native = 0;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Check if it is native AC .
     */
    if (BCM_GPORT_IS_VLAN_PORT(port)
        && (BCM_GPORT_SUB_TYPE_IS_LIF(port) || BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(port)))
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));
        if ((gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION) ||
            (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT))
        {
            is_native = TRUE;
        }
    }

    /*
     * Clear the port match entry from SW table.
     * Note: In non-native and native in network scope cases, the network domain is used as part of the key rather port itself.
     *       So, the same port as in adding the match entry should be used for deleting.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VLAN_PORT_MATCH_KEY_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_port_match_ingress_match_key_set(unit, entry_handle_id, match_info));
    rv = dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT);
    if ((is_native == FALSE)
        && (match_info->match == BCM_PORT_MATCH_PORT) && (match_info->match_ethertype == 0) && (rv == _SHR_E_NOT_FOUND))
    {
        rv = _SHR_E_NONE;
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API for add a match to an existing port.
 * This API adds an entry to the correct ISEM/ESEM table to
 * points to the LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 * \param [in] match_info - pointer to the match information.
 *   [in] match_info.port - Match port \n
 *   [in] match_info.flags - \n
 *        BCM_PORT_MATCH_INGRESS_ONLY - Indicates Ingress settings \n
 *        BCM_PORT_MATCH_NATIVE - Indicates Native Ethernet settings \n
 *        BCM_PORT_MATCH_EGRESS_ONLY - Indicates Egress settings \n
 *   [in] match_info.match - Match criteria \n
 *        BCM_PORT_MATCH_PORT - Match on module/port or trunk \n
 *        BCM_PORT_MATCH_PORT_VLAN - Match on module/port/trunk + outer VLAN \n
 *        BCM_PORT_MATCH_PORT_CVLAN - Match on module/port/trunk + C-VLAN \n
 *        BCM_PORT_MATCH_PORT_VLAN_STACKED - Match on module/port/trunk + inner/outer VLAN \n
 *   [in] match_info.match_vlan - Outer VLAN ID to match, when flags!=BCM_PORT_MATCH_PORT \n
 *   [in] match_info.match_inner_vlan - Inner VLAN ID to match, when flagsPORT_VLAN_STACKED \n
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  For ingress, It creates an ISEM entry pointing to an IN-LIF.
 *
 *  For egress, It allocates an esem access cmd, and links an esem match entry
 *  to a port or LIF by the match criteria.
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_match_add(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_vlan_port_t vlan_port;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_match_verify(unit, port, match_info));

    /*
     * Different meaning in ingress and egress
     */
    if (_SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_INGRESS_ONLY))
    {
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                     &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);

        /*
         * Convert the match struct in order to use the VLAN-Port set function
         */
        SHR_IF_ERR_EXIT(dnx_port_match_ingress_match_to_vlan_port_match_convert(unit, port, match_info, &vlan_port));

        /*
         * Create the match entry to the local In-LIF using the converted VLAN-Port struct
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_match_inlif_set(unit, &vlan_port, gport_hw_resources.local_in_lif, FALSE));

        /*
         * Set the match info in sw table
         */
        SHR_IF_ERR_EXIT(dnx_port_match_ingress_match_info_sw_add(unit, port, match_info));
    }
    else
    {
        /*
         * Egress match criteria.
         */
        SHR_IF_ERR_EXIT(dnx_port_match_egress_add(unit, port, match_info));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API for deleting a given match to an existing gport.
 * This API removes an entry from the correct ISEM/ESEM table or
 * other which points to the LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 * \param [in] match_info - pointer to the match information.
 *
 *   [in] match_info.port - Match port \n
 *   [in] match_info.flags - \n
 *        BCM_PORT_MATCH_INGRESS_ONLY - Indicates Ingress settings \n
 *        BCM_PORT_MATCH_NATIVE - Indicates Native Ethernet settings \n
 *        BCM_PORT_MATCH_EGRESS_ONLY - Indicates Egress settings, unsupported yet \n
 *   [in] match_info.match - Match criteria \n
 *        BCM_PORT_MATCH_PORT - Remove match on module/port or trunk \n
 *        BCM_PORT_MATCH_PORT_VLAN - Remove match on module/port/trunk + outer VLAN \n
 *        BCM_PORT_MATCH_PORT_CVLAN - Remove match on module/port/trunk + C-VLAN \n
 *        BCM_PORT_MATCH_PORT_VLAN_STACKED - Remove match on module/port/trunk + inner/outer VLAN \n
 *   [in] match_info.match_vlan - Outer VLAN ID match to remove, when flags!=BCM_PORT_MATCH_PORT \n
 *   [in] match_info.match_inner_vlan - Inner VLAN ID match to remove, when flagsPORT_VLAN_STACKED \n
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  For ingress, It removes an entry from ISEM pointing to an IN-LIF.
 *
 *  For egress, It resets the ESEM access cmd of a port/LIF to default and try to
 *  release the access cmd.
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_match_delete(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    bcm_vlan_port_t vlan_port_lookup;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_match_verify(unit, port, match_info));

    /*
     * Different meaning in ingress and egress
     */
    if (_SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_INGRESS_ONLY))
    {
        /*
         * Convert the match struct in order to use the VLAN-Port set function
         */
        SHR_IF_ERR_EXIT(dnx_port_match_ingress_match_to_vlan_port_match_convert
                        (unit, port, match_info, &vlan_port_lookup));

        if (_SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_NATIVE))
        {
            /*
             * Delete the lookup entry that points to a native local In-LIF (Virtual or not)
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_native_match_clear(unit, &vlan_port_lookup));

        }
        else
        {
            /*
             * Delete the lookup entry that points to the Non-Native In-LIF
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_non_native_match_clear(unit, &vlan_port_lookup));
        }

        /*
         * Clear the match info in sw table
         */
        SHR_IF_ERR_EXIT(dnx_port_match_ingress_match_info_sw_delete(unit, port, match_info));
    }
    else
    {
        /*
         * Delete Egress match criteria.
         */
        SHR_IF_ERR_EXIT(dnx_port_match_egress_delete(unit, port, match_info));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API for deleting all matches added by bcm_port_match_add of an existing port.
 * This API remove entries from the correct ISEM/ESEM table which
 * points to the LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_match_delete_all(
    int unit,
    bcm_gport_t port)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_match_delete_all_verify(unit, port));

    /** For egress side, only port default is relevant.*/
    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(port))
    {
        SHR_IF_ERR_EXIT(dnx_port_match_egress_virtual_default_delete_all(unit, port));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_match_ingress_delete_all(unit, port));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  BCM API to get multiple match objects that are using the given virtual port
 *  per the request by input size. The actual number of match objects is returned
 *  by output of count. If size is 0, it count all the related objects.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Vlan port for egress vlan translation.
 * \param [in] size - Number of elements in match array. May be 0, means not to receive
 *                    the match objects, count them only.
 * \param [out] match_array - Pointer for receive match objects. May be NULL.
 *                            See bcm_port_match_info_t for the structure details.
 * \param [out] count - Number of valid match objects found. (Must be a valid pointer.)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  Only virtual egress default type of "port" is supported. If size is 0 or match_array
 *  is NULL, only count will be returned for the number of objects that are using the "port".
 *
 * \see
 *  bcm_dnx_port_match_add
 */
shr_error_e
bcm_dnx_port_match_multi_get(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_port_match_info_t * match_array,
    int *count)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Verify the inputs
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_match_multi_get_verify(unit, port, size, match_array, count));

    /*
     * Get the match objects
     */
    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(port))
    {
        /** Get egress port match objects.*/
        SHR_IF_ERR_EXIT(dnx_port_match_multi_get_virtual_egress_default(unit, port, size, match_array, count));
    }
    else
    {
        /** Get ingress port match objects.*/
        SHR_IF_ERR_EXIT(dnx_port_match_multi_get_ingress(unit, port, size, match_array, count));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API to replace a match to an existing port with a new match.
 * This API adds an entry and removes another entry to/from the correct
 * ISEM/ESEM table that points to the LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 * \param [in] old_match - pointer to the match information to replace.
 * \param [in] new_match - pointer to the match information to replace with.
 *   [in] match_info.port - Match port \n
 *   [in] match_info.flags - \n
 *        BCM_PORT_MATCH_INGRESS_ONLY - Indicates Ingress settings \n
 *        BCM_PORT_MATCH_NATIVE - Indicates Native Ethernet settings \n
 *        BCM_PORT_MATCH_EGRESS_ONLY - Indicates Egress settings \n
 *   [in] match_info.match - Match criteria \n
 *        BCM_PORT_MATCH_PORT - Match on module/port or trunk \n
 *        BCM_PORT_MATCH_PORT_VLAN - Match on module/port/trunk + outer VLAN \n
 *        BCM_PORT_MATCH_PORT_CVLAN - Match on module/port/trunk + C-VLAN \n
 *        BCM_PORT_MATCH_PORT_VLAN_STACKED - Match on module/port/trunk + inner/outer VLAN \n
 *   [in] match_info.match_vlan - Outer VLAN ID to match, when flags!=BCM_PORT_MATCH_PORT \n
 *   [in] match_info.match_inner_vlan - Inner VLAN ID to match, when flagsPORT_VLAN_STACKED \n
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_match_replace(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * old_match,
    bcm_port_match_info_t * new_match)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * New match will be verified in bcm_dnx_port_match_add.
     * Checking the old match here as well in case it isn't
     * valid which will prevent adding the match.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_match_verify(unit, port, old_match));

    /*
     * Add new match
     */
    SHR_IF_ERR_EXIT(bcm_dnx_port_match_add(unit, port, new_match));

    /*
     * Delete old match
     */
    SHR_IF_ERR_EXIT(bcm_dnx_port_match_delete(unit, port, old_match));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * BCM API to add a set of matches to an existing port.
 * This API adds several entries to the correct ISEM/ESEM table that
 * points to the LIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 * \param [in] size - size of array.
 * \param [in] match_array - a pointer to an array of matches information.
 *   [in] match_info.port - Match port \n
 *   [in] match_info.flags - \n
 *        BCM_PORT_MATCH_INGRESS_ONLY - Indicates Ingress settings \n
 *        BCM_PORT_MATCH_NATIVE - Indicates Native Ethernet settings \n
 *        BCM_PORT_MATCH_EGRESS_ONLY - Indicates Egress settings \n
 *   [in] match_info.match - Match criteria \n
 *        BCM_PORT_MATCH_PORT - Match on module/port or trunk \n
 *        BCM_PORT_MATCH_PORT_VLAN - Match on module/port/trunk + outer VLAN \n
 *        BCM_PORT_MATCH_PORT_CVLAN - Match on module/port/trunk + C-VLAN \n
 *        BCM_PORT_MATCH_PORT_VLAN_STACKED - Match on module/port/trunk + inner/outer VLAN \n
 *   [in] match_info.match_vlan - Outer VLAN ID to match, when flags!=BCM_PORT_MATCH_PORT \n
 *   [in] match_info.match_inner_vlan - Inner VLAN ID to match, when flagsPORT_VLAN_STACKED \n
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  For ingress, It creates an ISEM entry pointing to an IN-LIF.
 *
 *  For egress, It allocates an esem access cmd, and links an esem match entry
 *  to a port or LIF by the match criteria.
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_match_set(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_port_match_info_t * match_array)
{
    uint32 array_index;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_match_set_verify(unit, port, size, match_array));

    for (array_index = 0; array_index < size; array_index++)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_match_add(unit, port, match_array + array_index));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_match_svtag_esem_cmd_set(
    int unit,
    bcm_gport_t gport,
    uint8 enable)
{
    dnx_esem_cmd_data_t esem_cmd_data;
    dnx_port_esem_cmd_info_t esem_cmd_info;
    int old_esem_cmd, new_esem_cmd;
    uint8 is_first;
    uint8 is_last;
    uint8 svtag_present = FALSE;
    uint32 flags = DNX_PORT_ESEM_CMD_UPDATE;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    esem_cmd_info.nof_accesses = 0;

    sal_memset(&esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));

    SHR_IF_ERR_EXIT(dnx_port_match_esem_cmd_port_get(unit, gport, &old_esem_cmd));

    /** Get the original ESEM_ACC_CMND profile data*/
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_sw_get(unit, old_esem_cmd, &esem_cmd_data, NULL));

    if (enable)
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_add
                        (unit, 0, ESEM_ACCESS_TYPE_SVTAG, DBAL_ENUM_FVAL_ESEM_APP_DB_ID_OUT_TM_PORT,
                         DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID, &esem_cmd_info));

    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_switch_svtag_is_present_in_esem_cmd(unit, &esem_cmd_data, &svtag_present));

        esem_cmd_info.nof_accesses_remove = 1;
        esem_cmd_info.remove_accesses[0] = ESEM_ACCESS_TYPE_SVTAG;
        flags |= DNX_PORT_ESEM_CMD_ACCESS_REMOVE;
    }
    /*
     * In case the SVTAG is enabled or removed update the ESEM CMD
     */
    if (enable || svtag_present)
    {
        dnx_esem_cmd_suffix_size_t esem_cmd_size;
        uint32 prefix;
        /** Get current esem cmd size and prefix */
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_size_and_prefix_get(unit, gport, &esem_cmd_size, &prefix));

        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_exchange
                        (unit, flags, esem_cmd_size, prefix, &esem_cmd_info, old_esem_cmd,
                         &new_esem_cmd, &esem_cmd_data, &is_first, &is_last));

        if (is_first)
        {
            SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_set(unit, new_esem_cmd, esem_cmd_data));
        }

        SHR_IF_ERR_EXIT(dnx_port_match_esem_cmd_port_set(unit, gport, new_esem_cmd));

        if ((is_last == TRUE) && (new_esem_cmd != old_esem_cmd))
        {
            /** Remove the old attributes from esem-cmd table in the old cmd-id*/
            SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_clear(unit, old_esem_cmd));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Fill the match fields into a match-port structure
 *  according to a supplied DBAL entry handle ID that was filled
 *  retrieved from the match SW DB. The relevant fields to be
 *  set are determined by the match criteria.
 *  flags and any other fields that aren't match fields are
 *  ignored.
 *
 *   \param [in] unit - unit id
 *   \param [in] entry_handle_id - DBAL entry handle ID from the
 *          match SW DB.
 *   \param [out] match_info - A pointer to the match structure
 *          to be filled with match information.
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   dnx_port_match_inlif_update
 */
static shr_error_e
dnx_port_match_ingress_match_info_get(
    int unit,
    uint32 entry_handle_id,
    bcm_port_match_info_t * match_info)
{
    uint32 tmp32bits;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    bcm_port_match_info_t_init(match_info);

    /*
     * Retrieve from the handle the match criteria and other fields that are relevant
     * for all the matches
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_CRITERIA, &tmp32bits));
    match_info->match = tmp32bits;

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_MATCH_PORT, &tmp32bits));
    match_info->port = tmp32bits;

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_ETH_TYPE, &tmp32bits));
    match_info->match_ethertype = tmp32bits;

    /*
     * Retrieve the rest of the fields according the match
     */
    switch (match_info->match)
    {
        case BCM_PORT_MATCH_PORT:
        case BCM_PORT_MATCH_PORT_UNTAGGED:
            break;      /* Port already retrieved */

        case BCM_PORT_MATCH_PORT_VLAN:
        case BCM_PORT_MATCH_PORT_CVLAN:
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, &tmp32bits));
            match_info->match_vlan = tmp32bits;
            break;

        case BCM_PORT_MATCH_PORT_VLAN_STACKED:
        case BCM_PORT_MATCH_PORT_SVLAN_STACKED:
        case BCM_PORT_MATCH_PORT_CVLAN_STACKED:
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, &tmp32bits));
            match_info->match_vlan = tmp32bits;
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, &tmp32bits));
            match_info->match_inner_vlan = tmp32bits;
            break;

        case BCM_PORT_MATCH_PORT_PCP_VLAN:
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, &tmp32bits));
            match_info->match_vlan = tmp32bits;
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, &tmp32bits));
            match_info->match_pcp = tmp32bits;
            break;

        case BCM_PORT_MATCH_PORT_PCP_VLAN_STACKED:
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, &tmp32bits));
            match_info->match_vlan = tmp32bits;
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, &tmp32bits));
            match_info->match_inner_vlan = tmp32bits;
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, &tmp32bits));
            match_info->match_pcp = tmp32bits;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected match for In-LIF lookup entry: %d\n", match_info->match);
            break;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See the .h file
 */
shr_error_e
dnx_port_match_inlif_update(
    int unit,
    uint32 algo_flags,
    int is_virtual,
    uint32 old_local_in_lif_id,
    uint32 new_local_in_lif_id)
{
    uint32 entry_handle_id;
    bcm_gport_t gport;
    int is_traverse_end;
    bcm_port_match_info_t match_info;
    bcm_vlan_port_t vlan_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Handle the ISEM and Port entries that were added using the port_match APIs.
     * First, get the local_in_lif's gport, before
     * Traversing the port match entries
     */
    if (is_virtual)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_native_virtual_match_sw_state
                        (unit, (int) old_local_in_lif_id, &gport));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit, algo_flags, _SHR_CORE_ALL, old_local_in_lif_id, &gport));
    }

    /*
     * Prepare the traverse of the port_match SW DB
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VLAN_PORT_MATCH_KEY_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_GPORT, 0, DBAL_CONDITION_EQUAL_TO, (uint32 *) &gport, NULL));

    /*
     * Traverse the port_match SW DB for entries pointing to the in_lif's gport
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_traverse_end));
    while (!is_traverse_end)
    {
        /*
         * Retrieve the match fields
         */
        SHR_IF_ERR_EXIT(dnx_port_match_ingress_match_info_get(unit, entry_handle_id, &match_info));

        /*
         * Set the match Native indication according to the algo flags
         */
        match_info.flags |=
            (algo_flags == DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS) ? 0 : BCM_PORT_MATCH_NATIVE;

        /*
         * Convert the match struct in order to use the VLAN-Port set function
         */
        SHR_IF_ERR_EXIT(dnx_port_match_ingress_match_to_vlan_port_match_convert(unit, gport, &match_info, &vlan_port));

        /*
         * Update the local In-LIF in the match entry using the converted VLAN-Port struct
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_match_inlif_set(unit, &vlan_port, new_local_in_lif_id, TRUE));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_traverse_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
