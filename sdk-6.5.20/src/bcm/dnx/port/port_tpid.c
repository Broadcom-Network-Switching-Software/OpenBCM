/** \file port_tpid.c
 * 
 *
 * Port TPID procedures for DNX.
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
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/mem.h>

#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm/switch.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/switch/switch_tpid.h>
#include <bcm_int/dnx/port/port_tpid.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port_pp.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/port_tpid_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_port_pp_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>
#include <src/bcm/dnx/trunk/trunk_utils.h>
#include <src/bcm/dnx/trunk/trunk_sw_db.h>

/*
 * }
 */

/**
 * \brief
 * Port TPID tag format mapping.
 * This structure type maps LLVP parser info (is_outer_prio,
 * outer_tpid and inner_tpid) to tag format value.
 */
typedef struct dnx_port_tpid_tag_format_mapping_s
{
    dnx_port_tpid_llvp_parser_info_t llvp_parser_info;
    bcm_port_tag_format_class_t tag_format_class_id;
} dnx_port_tpid_tag_format_mapping_t;

/**
 * \brief
 * Auxiliary indexes for handling outer/inner tpids.
 * \see
 * dnx_port_tpid_class_init_default.
 * bcm_dnx_port_tpid_class_set.
 */
#define TPID_INDEX_OUTER    0
#define TPID_INDEX_INNER    1
#define TPID_INDEX_NOF      2

/**
 * llvp tag format,  used to set llvp pcp dei map type
 */
#define DNX_VLAN_FORMAT_NO_TAG 0

/**
 * \brief -
 * Verify function for bcm_dnx_port_tpid_class_set - checking 'flags' as follows:
 *  - Only one of "Ingres only"/"Egress only" is allowed to be set.
 *  - "Ingress Only" cannot be set with the Egress flags -
 *    "Egress Native", "Egress IP Tunnel" or "Inner C".
 *  - "Egress Only" cannot be set with the Ingress flags - "Ingress Native".
 *  - Natives can be "Ingress only" or "Egress only" or both.
 *  - Egress IP Tunnel must be "Egress Only" and cannot be set with "Ingress Only", "Ingress Native" or "Egress Native".
 *  - Only one of "Outer C"/"Inner C" can be set at once
 *  - C-Tag flags at Egress are applicable only for Non-Native
 *    packets .
 *
 * \param [in] unit - the relevant unit
 * \param [in] flags -
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  None.
 * \see
 *   dnx_port_tpid_class_set_verify
 */
static shr_error_e
dnx_port_tpid_class_set_flags_verify(
    int unit,
    uint32 flags)
{
    int is_ingress_only, is_egress_only;
    int is_ingress_native, is_egress_native;
    int is_egress_ip_tunnel;
    int is_outer_c_tag, is_inner_c_tag;

    SHR_FUNC_INIT_VARS(unit);

    is_ingress_only = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_INGRESS_ONLY) ? TRUE : FALSE;
    is_egress_only = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_EGRESS_ONLY) ? TRUE : FALSE;

    if ((is_ingress_only == TRUE) && (is_egress_only == TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong flags setting. Both INGRESS_ONLY (0x%08X) and EGRESS_ONLY (0x%08X) are set!!! flags = 0x%08X\n",
                     BCM_PORT_TPID_CLASS_INGRESS_ONLY, BCM_PORT_TPID_CLASS_EGRESS_ONLY, flags);
    }

    is_ingress_native = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_NATIVE_IVE) ? TRUE : FALSE;
    is_egress_native = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_NATIVE_EVE) ? TRUE : FALSE;
    is_egress_ip_tunnel = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL) ? TRUE : FALSE;

    is_outer_c_tag = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_OUTER_C) ? TRUE : FALSE;
    is_inner_c_tag = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_INNER_C) ? TRUE : FALSE;

    /*
     * check under ext_vcp_enable, setting of priority is forbidden on ingress
     */
    if (dnx_data_vlan.llvp.ext_vcp_en_get(unit))
    {
        if (is_ingress_only && _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_OUTER_IS_PRIO))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong flags setting. BCM_PORT_TPID_CLASS_OUTER_IS_PRIO can't be set when ext_vcp_en is set \n");

        }
        if (is_ingress_only && _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong flags setting. BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO useless when ext_vcp_en is set \n");

        }
        if ((is_ingress_only == FALSE) && (is_egress_only == FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong flags setting. INGRESS_ONLY or EGRESS_ONLY shoudl be set when ext_vcp_en is set \n");
        }
    }

    if ((is_ingress_only == TRUE)
        && ((is_egress_native == TRUE) || (is_egress_ip_tunnel == TRUE) || (is_inner_c_tag == TRUE)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong flags setting. INGRESS_ONLY(0x%08X) was combined with one of the Egress only flags: NATIVE_EVE(0x%08X)/EGRESS_IP_TUNNEL(0x%08X)/INNER_C(0x%08X)!!! flags = 0x%08X\n",
                     BCM_PORT_TPID_CLASS_INGRESS_ONLY, BCM_PORT_TPID_CLASS_NATIVE_EVE,
                     BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL, BCM_PORT_TPID_CLASS_INNER_C, flags);
    }

    if ((is_egress_only == TRUE) && (is_ingress_native == TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong flags setting. Both EGRESS_ONLY (0x%08X) and NATIVE_IVE (0x%08X) are set!!! flags = 0x%08X\n",
                     BCM_PORT_TPID_CLASS_EGRESS_ONLY, BCM_PORT_TPID_CLASS_NATIVE_IVE, flags);
    }

    if (is_ingress_native == TRUE)
    {
        /*
         * Ingress native can be:
         *   - ingress only
         *   - native symmetric
         */
        if ((is_ingress_only == FALSE) && (is_egress_native == FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong flags setting. Ingress native (0x%08X) can be ingress_only (0x%08X) or symmetric with Egress native (0x%08X)!!! flags = 0x%08X\n",
                         BCM_PORT_TPID_CLASS_NATIVE_IVE, BCM_PORT_TPID_CLASS_INGRESS_ONLY,
                         BCM_PORT_TPID_CLASS_NATIVE_EVE, flags);
        }
    }

    if (is_egress_native == TRUE)
    {
        /*
         * Egress native can be:
         *   - egress only
         *   - native symmetric
         */
        if ((is_egress_only == FALSE) && (is_ingress_native == FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong flags setting. Egress native (0x%08X) can be egress_only (0x%08X) or symmetric with Ingress native (0x%08X)!!! flags = 0x%08X\n",
                         BCM_PORT_TPID_CLASS_NATIVE_EVE, BCM_PORT_TPID_CLASS_EGRESS_ONLY,
                         BCM_PORT_TPID_CLASS_NATIVE_IVE, flags);
        }

        /*
         * When Egress native is set, DISCARD shouldn't be allowed.
         */
        if (_SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_DISCARD))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong flags setting. Egress native (0x%08X) and Discard (0x%08X) are set!!! flags = 0x%08X\n",
                         BCM_PORT_TPID_CLASS_NATIVE_EVE, BCM_PORT_TPID_CLASS_DISCARD, flags);
        }
    }

    if (is_egress_ip_tunnel == TRUE)
    {
        /*
         * "Egress IP Tunnel" is related only to Egress thus the user must set the "Egress-only" flag.
         */
        if (is_egress_only == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong flags setting. Egress IP Tunnel flag (0x%08X) must be set with Egress-only (0x%08X) flags!!! flags = 0x%08X\n",
                         BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL, BCM_PORT_TPID_CLASS_EGRESS_ONLY, flags);
        }

        /*
         * Check that Ingress-only, Native IVE or Native EVE are not set.
         */
        if ((is_ingress_only == TRUE) || (is_ingress_native == TRUE) || (is_egress_native == TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong flags setting. Egress IP Tunnel flag (0x%08X) cannot be set with Ingress-only (0x%08X), Ingress Native (0x%08X) or Egress Native (0x%08X) flags!!! flags = 0x%08X\n",
                         BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL, BCM_PORT_TPID_CLASS_INGRESS_ONLY,
                         BCM_PORT_TPID_CLASS_NATIVE_IVE, BCM_PORT_TPID_CLASS_NATIVE_EVE, flags);
        }
    }

    /*
     * C_TAG flags validations
     */
    if ((is_outer_c_tag == TRUE) && (is_inner_c_tag == TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong flags setting. Both OUTER_C(0x%08X) and INNER_C(0x%08X) are set!!! flags = 0x%08X\n",
                     BCM_PORT_TPID_CLASS_OUTER_C, BCM_PORT_TPID_CLASS_INNER_C, flags);
    }

    if ((is_outer_c_tag || is_inner_c_tag) && (is_egress_ip_tunnel || is_egress_native))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong flags setting. One of the C_TAG flags OUTER_C(0x%08X) or INNER_C(0x%08X) are combined at Egress with a Native flag - EGRESS_IP_TUNNEL(0x%08X) or NATIVE_EVE(0x%08X)!!! flags = 0x%08X\n",
                     BCM_PORT_TPID_CLASS_OUTER_C, BCM_PORT_TPID_CLASS_INNER_C, BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL,
                     BCM_PORT_TPID_CLASS_NATIVE_EVE, flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify function for bcm_dnx_port_tpid_class_get - checking 'flags' as follows:
 *  - One of "Ingress only"/"Egress only" must be set and only one of them.
 *  - "Ingress Native" must be set with "Ingress only" flag and no other flag.
 *  - "Egress Native" must be set with "Egress only" flag and no other flag.
 *  - "Egress IP Tunnel" must be set with "Egress Only"flag and no other flag.
 *
 * \param [in] unit - the relevant unit
 * \param [in] flags -
 *
 * \return
 *   shr_error_e
 *
 * \remark
 * None.
 * \see
 *   dnx_port_tpid_class_get_verify
 */
static shr_error_e
dnx_port_tpid_class_get_flags_verify(
    int unit,
    uint32 flags)
{
    int is_ingress_only, is_egress_only;
    int is_ingress_native, is_egress_native;
    int is_egress_ip_tunnel;

    SHR_FUNC_INIT_VARS(unit);

    is_ingress_only = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_INGRESS_ONLY) ? TRUE : FALSE;
    is_egress_only = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_EGRESS_ONLY) ? TRUE : FALSE;

    if ((is_ingress_only == TRUE) && (is_egress_only == TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong flags setting. Both INGRESS_ONLY (0x%08X) and EGRESS_ONLY (0x%08X) are set!!! flags = 0x%08X\n",
                     BCM_PORT_TPID_CLASS_INGRESS_ONLY, BCM_PORT_TPID_CLASS_EGRESS_ONLY, flags);
    }

    if ((is_ingress_only == FALSE) && (is_egress_only == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong flags setting. One of INGRESS_ONLY (0x%08X) and EGRESS_ONLY (0x%08X) must be set!!! flags = 0x%08X\n",
                     BCM_PORT_TPID_CLASS_INGRESS_ONLY, BCM_PORT_TPID_CLASS_EGRESS_ONLY, flags);
    }

    is_ingress_native = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_NATIVE_IVE) ? TRUE : FALSE;
    is_egress_native = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_NATIVE_EVE) ? TRUE : FALSE;
    is_egress_ip_tunnel = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL) ? TRUE : FALSE;

    if (is_ingress_native == TRUE)
    {
        /*
         * "Ingress native" flag can only be set with "Ingress only" flag. No other flag should be set!
         */
        if (flags != (BCM_PORT_TPID_CLASS_INGRESS_ONLY | BCM_PORT_TPID_CLASS_NATIVE_IVE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong flags setting. Ingress native (0x%08X) must be set together with Ingress_only (0x%08X) and no other flag!!! flags = 0x%08X\n",
                         BCM_PORT_TPID_CLASS_NATIVE_IVE, BCM_PORT_TPID_CLASS_INGRESS_ONLY, flags);
        }
    }

    if (is_egress_native == TRUE)
    {
        /*
         * "Egress native" flag can only be set with "Egress only" flag. No other flag should be set!
         */
        if (flags != (BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_NATIVE_EVE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong flags setting. Egress native (0x%08X) must be set together with Egress_only (0x%08X) and no other flag!!! flags = 0x%08X\n",
                         BCM_PORT_TPID_CLASS_NATIVE_EVE, BCM_PORT_TPID_CLASS_EGRESS_ONLY, flags);
        }
    }

    if (is_egress_ip_tunnel == TRUE)
    {
        /*
         * "Egress IP Tunnel" flag can only be set with "Egress only" flag. No other flag should be set!
         */
        if (flags != (BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong flags setting. Egress IP Tunnel (0x%08X) must be set together with Egress_only (0x%08X) and no other flag!!! flags = 0x%08X\n",
                         BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL, BCM_PORT_TPID_CLASS_EGRESS_ONLY, flags);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify function for checking 'port' as follows:
 *  - If INGRESS only or both INGRESS and EGRESS setting:
 *      verify that the port is physical port.
 *  - If only EGRESS setting:
 *      the port is physical port or tunnel gport:
 *          verify that the port is physical port or tunnel gport.
 *
 * \param [in] unit - the relevant unit
 * \param [in] flags - ingress or egress flags - BCM_PORT_TPID_CLASS_XXX
 * \param [in] port - physical port or tunnel gport
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None
 * \see
 *   dnx_port_tpid_class_set_verify
 *   dnx_port_tpid_class_get_verify
 */
static shr_error_e
dnx_port_tpid_class_port_verify(
    int unit,
    uint32 flags,
    bcm_gport_t port)
{
    int is_ingress_only, is_egress_only, is_ingress_native, is_egress_native, is_egress_ip_tunnel;
    uint8 is_physical_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_ingress_only = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_INGRESS_ONLY) ? TRUE : FALSE;
    is_egress_only = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_EGRESS_ONLY) ? TRUE : FALSE;
    is_ingress_native = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_NATIVE_IVE) ? TRUE : FALSE;
    is_egress_native = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_NATIVE_EVE) ? TRUE : FALSE;
    is_egress_ip_tunnel = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL) ? TRUE : FALSE;

    if ((is_ingress_native == TRUE) || (is_egress_native == TRUE) || (is_egress_ip_tunnel == TRUE))
    {
        /*
         * Native IVE/EVE / Egress IP Tunnel; port is irrelevant, verify that it is not set:
         */
        if (port != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong port setting. For Ingress Native / Egress Native / Egress IP Tunnel, port is irrelevent thus must be set to zero! port = 0x%08X flags = 0x%08X\n",
                         port, flags);
        }
    }
    else
    {

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_physical_port));

        if ((is_egress_only == TRUE) || (is_ingress_only == TRUE))
        {
            /*
             * INGRESS / EGRESS only, the port is physical port or tunnel gport
             */
            if (is_physical_port == FALSE)
            {
                /*
                 * The port is not in physical port, check tunnel/mpls gport:
                 * Ingress:
                 *      IN_LIF_FORMAT_PWE
                 *      IN_LIF_IPvX_TUNNELS
                 * Egress:
                 *      EEDB_IPV4_TUNNEL
                 *      EEDB_IPV6_TUNNEL
                 *      EEDB_MPLS_TUNNEL
                 */
                if ((!BCM_GPORT_IS_TUNNEL(port)) && (!BCM_GPORT_IS_MPLS_PORT(port)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Wrong port setting. port = 0x%08X is not physical port and not a tunnel/mpls gport\n",
                                 port);
                }

                /*
                 * Discard is not allowed to Egress Tunnel Out-LIF
                 */
                if ((is_egress_only == TRUE) && _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_DISCARD))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Wrong port setting. Discard is not allowed to tunnel/mpls port = 0x%08X \n", port);
                }
            }
        }
        else
        {
            /*
             * Both INGRESS and EGRESS:
             * the port should be a physical port
             */
            if (is_physical_port == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong port setting. port = 0x%08X is not a physical port\n", port);
            }
        }

        if ((is_physical_port == FALSE) && (is_ingress_only == TRUE))
        {
            /*
             * the port is In-LIF gport, check that it's vlan_domain is not zero
             */
            uint32 entry_handle_id;
            uint32 next_layer_vlan_domain;
            dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

            /*
             * Get local LIFs using DNX Algo Gport Managment:
             */
            SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                        (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                         &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         gport_hw_resources.inlif_dbal_result_type);
            /*
             * getting the entry value
             */
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, INST_SINGLE,
                             &next_layer_vlan_domain));

            if (next_layer_vlan_domain == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Wrong port setting. port = 0x%08X is IN-LIF, it's next_layer_vlan_domain can't be zero.\n"
                             "The LLVP profile of vlan domain 0 always points to Ingress Native AC llvp profile!!!\n"
                             "Changing Ingress Native AC llvp profile can only be done using BCM NATIVE flag!!!\n",
                             port);
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify function for checking 'tpid' value:
 * If the value is not "INVALID" and not "ANY" - check that
 * it is part of the configured global TPIDs.
 * \param [in] unit - the relevant unit
 * \param [in] tpid - the tpid value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None
 * \see
 *   dnx_port_tpid_class_set_verify
 *   dnx_port_tpid_class_get_verify
 */
static shr_error_e
dnx_port_tpid_class_tpid_verify(
    int unit,
    uint32 tpid)
{
    int tpid_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the tpid is one of the global tpids
     */
    SHR_IF_ERR_EXIT(dnx_switch_tpid_index_get(unit, (uint16) tpid, &tpid_index));

    if ((tpid != BCM_PORT_TPID_CLASS_TPID_INVALID) && (tpid != BCM_PORT_TPID_CLASS_TPID_ANY))
    {
        if (tpid_index == BCM_DNX_SWITCH_TPID_INDEX_INVALID)
        {
            /*
             * tpid index not found
             */
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error! tpid=0x%x is not one of the global TPIDs!!!\n", tpid);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file for description
 */
shr_error_e
dnx_port_tpid_class_sw_id_is_created_verify(
    int unit,
    int is_symmetric,
    int is_ingress_only,
    bcm_port_tag_format_class_t tag_format_class_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((is_ingress_only == TRUE) || (is_symmetric == TRUE))
    {
        uint8 is_allocated = FALSE;

        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_ingress.is_allocated
                        (unit, tag_format_class_id, &is_allocated));

        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong tag_format_class_id (=%d) - it was not created at the Ingress!!!\n",
                         tag_format_class_id);
        }
    }

    if ((is_ingress_only == FALSE) || (is_symmetric == TRUE))
    {
        uint8 is_allocated = FALSE;

        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_egress.is_allocated
                        (unit, tag_format_class_id, &is_allocated));

        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong tag_format_class_id (=%d) - it was not created at the Egress!!!\n",
                         tag_format_class_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function for verification whether a HW TAG-Struct was created
 * by the bcm_dnx_port_tpid_class_create function.
 * \param [in] unit - relevant unit
 * \param [in] is_ingress -check ingress or egress.
 * \param [in] tag_struct_hw_id - the tag_struct.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  None
 * \see
 *   bcm_dnx_port_tpid_class_get
 */
static shr_error_e
dnx_port_tpid_class_hw_id_is_created_verify(
    int unit,
    int is_ingress,
    int tag_struct_hw_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((is_ingress == TRUE))
    {
        uint8 is_allocated = FALSE;

        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.incoming_tag_structure_ingress.is_allocated
                        (unit, tag_struct_hw_id, &is_allocated));

        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong tag_struct_hw_id (=%d) - it was not created at the Ingress!!!\n", tag_struct_hw_id);
        }
    }
    else
    {
        uint8 is_allocated = FALSE;

        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.incoming_tag_structure_egress.is_allocated
                        (unit, tag_struct_hw_id, &is_allocated));

        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong tag_struct_hw_id (=%d) - it was not created at the Egress!!!\n", tag_struct_hw_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify function for BCM API bcm_port_tpid_class_create.
 * It checks that:
 *  - BCM_PORT_TPID_CLASS_INGRESS_ONLY and BCM_PORT_TPID_CLASS_EGRESS_ONLY are not both set.
 *  - The TAG-Struct TYPE is a valid type.
 *  - The  pointer to tag_struct to be created is not NULL.
 *  - If "WITH_ID" option is used -
 *      - check that the TAG-Struct has valid value (5 bits).
 *      - check that the TAG-Struct is not already created.
 * \param [in] unit - relevant unit
 * \param [in] flags -BCM_PORT_TPID_CLASS_INGRESS_ONLY,
 *    BCM_PORT_TPID_CLASS_EGRESS_ONLY, BCM_PORT_TPID_CLASS_WITH_ID or none.
 * \param [in] tag_struct_type - tag-struct type
 * \param [in] tag_format_class_id - Pointer to tag_struct to be created.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None
 * \see
 *   bcm_dnx_port_tpid_class_create
 */
static shr_error_e
dnx_port_tpid_class_create_verify(
    int unit,
    uint32 flags,
    bcm_port_tag_struct_type_t tag_struct_type,
    bcm_port_tag_format_class_t * tag_format_class_id)
{
    int is_ingress_only, is_egress_only, is_symmetric;

    SHR_FUNC_INIT_VARS(unit);

    is_ingress_only = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_INGRESS_ONLY) ? TRUE : FALSE;
    is_egress_only = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_EGRESS_ONLY) ? TRUE : FALSE;
    is_symmetric = (is_ingress_only == FALSE) && (is_egress_only == FALSE);

    /*
     * Verify flags:
     * Check that both ingress and egress only flags are not set:
     */
    if ((is_ingress_only == TRUE) && (is_egress_only == TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong flags setting. Both INGRESS_ONLY and EGRESS_ONLY are set!!! flags = 0x%08X\n",
                     flags);
    }

    /*
     * Verify tag_struct_type is in range:
     */
    if (tag_struct_type >= bcmNofTagStructType)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong tag_struct_type setting. tag_struct_type (%d) is out of range {%d, %d}\n",
                     tag_struct_type, bcmTagStructTypeUntag, bcmNofTagStructType - 1);
    }

    /*
     * Verify that the tag_format_class_id pointer is not NULL:
     */
    SHR_NULL_CHECK(tag_format_class_id, _SHR_E_PARAM, "tag_format_class_id");

    /*
     * WITH_ID:
     * Verify that the specified SW-ID is valid (5 bits).
     * Vefiry that the specified SW-ID is not already allocated.
     */
    if (_SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_WITH_ID) == TRUE)
    {
        uint32 nof_vlan_tag_formats = dnx_data_vlan.vlan_translate.nof_vlan_tag_formats_get(unit);

        if (*tag_format_class_id >= nof_vlan_tag_formats)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. tag_format_class_id = %d, MAX value = %d!!!\n",
                         *tag_format_class_id, nof_vlan_tag_formats - 1);
        }

        if ((is_ingress_only == TRUE) || (is_symmetric == TRUE))
        {
            uint8 is_allocated = FALSE;

            SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_ingress.is_allocated
                            (unit, *tag_format_class_id, &is_allocated));

            if (is_allocated == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS,
                             "Wrong tag_format_class_id (=%d) - it already exists at the Ingress!!! flags = 0x%08X\n",
                             *tag_format_class_id, flags);
            }
        }

        if ((is_egress_only == TRUE) || (is_symmetric == TRUE))
        {
            uint8 is_allocated = FALSE;

            SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_egress.is_allocated
                            (unit, *tag_format_class_id, &is_allocated));

            if (is_allocated == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS,
                             "Wrong tag_format_class_id (=%d) - it already exists at the Egress!!! flags = 0x%08X\n",
                             *tag_format_class_id, flags);
            }
        }

        if (is_symmetric == TRUE)
        {
            uint8 is_allocated = FALSE;

            SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_symmetric.is_allocated
                            (unit, *tag_format_class_id, &is_allocated));

            if (is_allocated == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS,
                             "Wrong tag_format_class_id (=%d) - it already exists as Symmetric!!! flags = 0x%08X\n",
                             *tag_format_class_id, flags);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify function for BCM API bcm_port_tpid_class_destroy
 * \param [in] unit - relevant unit
 * \param [in] flags -
 * \param [in] tag_format_class_id - the tag_struct to be freed.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None
 * \see
 *   bcm_dnx_port_tpid_class_destroy
 */
static shr_error_e
dnx_port_tpid_class_destroy_verify(
    int unit,
    uint32 flags,
    bcm_port_tag_format_class_t tag_format_class_id)
{
    int is_ingress_only, is_egress_only, is_symmetric;
    uint8 symmetric_indication;

    SHR_FUNC_INIT_VARS(unit);

    is_ingress_only = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_INGRESS_ONLY) ? TRUE : FALSE;
    is_egress_only = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_EGRESS_ONLY) ? TRUE : FALSE;
    is_symmetric = (is_ingress_only == FALSE) && (is_egress_only == FALSE);

    /*
     * Verify flags:
     * Check that both ingress and egress only flags are not set:
     */
    if ((is_ingress_only == TRUE) && (is_egress_only == TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong flags setting. Both INGRESS_ONLY and EGRESS_ONLY are set!!! flags = 0x%08X\n",
                     flags);
    }

    /*
     * Vefiry that the specified SW-ID was allocated previously by bcm_port_tpid_class_create:
     */
    if ((is_ingress_only == TRUE) || (is_symmetric == TRUE))
    {
        uint8 is_allocated = FALSE;

        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_ingress.is_allocated
                        (unit, tag_format_class_id, &is_allocated));

        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "Wrong tag_format_class_id (=%d) - it does not exist at the Ingress!!! flags = 0x%08X\n",
                         tag_format_class_id, flags);
        }
    }

    if ((is_egress_only == TRUE) || (is_symmetric == TRUE))
    {
        uint8 is_allocated = FALSE;

        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_egress.is_allocated
                        (unit, tag_format_class_id, &is_allocated));

        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "Wrong tag_format_class_id (=%d) - it does not exist at the Egress!!! flags = 0x%08X\n",
                         tag_format_class_id, flags);
        }
    }

    /*
     * Verify that symmetric TAG-Struct is not partialy destroyed (as ingress/egress TAG-Struct),
     * and vise versa (ingress/egress TAG-Struct is not destroyed as symmetric).
     */
    SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_symmetric_indication.bit_get
                    (unit, tag_format_class_id, &symmetric_indication));

    if (is_symmetric != symmetric_indication)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Wrong tag_format_class_id (=%d) - its symmetric_indication = %d while flags = 0x%08X\n",
                     tag_format_class_id, symmetric_indication, flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify function for BCM API bcm_port_tpid_class_set
 * \param [in] unit - relevant unit
 * \param [in] tpid_class - Pointer to a TPID class structure that includes the port
 *          and the data to set.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None
 * \see
 *   bcm_dnx_port_tpid_class_get
 */
static shr_error_e
dnx_port_tpid_class_set_verify(
    int unit,
    bcm_port_tpid_class_t * tpid_class)
{
    int is_ingress_only, is_egress_only, is_symmetric;

    SHR_FUNC_INIT_VARS(unit);

    is_ingress_only = _SHR_IS_FLAG_SET(tpid_class->flags, BCM_PORT_TPID_CLASS_INGRESS_ONLY) ? TRUE : FALSE;
    is_egress_only = _SHR_IS_FLAG_SET(tpid_class->flags, BCM_PORT_TPID_CLASS_EGRESS_ONLY) ? TRUE : FALSE;
    is_symmetric = (is_ingress_only == FALSE) && (is_egress_only == FALSE);

    /*
     * Verify that the tpid_class pointer is not NULL:
     */
    SHR_NULL_CHECK(tpid_class, _SHR_E_PARAM, "tpid_class");

    /*
     * Verify that the field vlan_translation_action_id in tpid_class pointer is 0, unsupported otherwise
     */
    SHR_VAL_VERIFY(tpid_class->vlan_translation_action_id, 0, _SHR_E_UNAVAIL,
                   "No support for default IVEC on the device");

    /*
     * Verify that the field vlan_translation_qos_map_id in tpid_class pointer is 0, unsupported otherwise
     */
    SHR_VAL_VERIFY(tpid_class->vlan_translation_qos_map_id, 0, _SHR_E_UNAVAIL,
                   "No support for vlan_translation_qos_map_id on the device");

    /*
     * Verify that the field tpid_class_id in tpid_class pointer is 0, unsupported otherwise
     */
    SHR_VAL_VERIFY(tpid_class->tpid_class_id, 0, _SHR_E_UNAVAIL, "No support for tpid_class_id on the device");

    /*
     * Verify that untagged packets are classified only to tag format 0
     */
    if ((tpid_class->tpid1 == BCM_PORT_TPID_CLASS_TPID_INVALID) &&
        (tpid_class->tpid2 == BCM_PORT_TPID_CLASS_TPID_INVALID) && (tpid_class->tag_format_class_id != 0))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Untagged packets can be classified only to tag format 0!!! tpid1 = 0x%X, tpid2 = 0x%X,  tag_format_class_id = %d\n",
                     tpid_class->tpid1, tpid_class->tpid2, tpid_class->tag_format_class_id);

    }

    /*
     * Verify correct flags settings:
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_flags_verify(unit, tpid_class->flags));

    /*
     * Verify port:
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_port_verify(unit, tpid_class->flags, tpid_class->port));

    /*
     * Verify the the TAG-Struct was created:
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_sw_id_is_created_verify
                    (unit, is_symmetric, is_ingress_only, tpid_class->tag_format_class_id));

    /*
     * A symmetric tag can be "partially set" that is, only its ingress or only its egress.
     * Verify that for "symmetric set", the tag is indeed symmetric:
     */
    if (is_symmetric == TRUE)
    {
        uint8 symmetric_indication;

        /*
         * Verify symmetric indication
         */
        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_symmetric_indication.bit_get
                        (unit, tpid_class->tag_format_class_id, &symmetric_indication));

        if (symmetric_indication == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error !!! tag_format_class_id (=%d) - is set by flags = 0x%08X as symmetric while in DB symmetric_indication = FALSE\n",
                         tpid_class->tag_format_class_id, tpid_class->flags);
        }
    }

    /*
     * Verify range of tpid_class->tag_format_class_id
     * Note -
     * If the TAG-Struct was allocated, it must be in range.
     * This check is done in the bcm_dnx_port_tpid_class_create function.
     */

    /*
     * Verify that tpid value is valid
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_tpid_verify(unit, tpid_class->tpid1));
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_tpid_verify(unit, tpid_class->tpid2));

    
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify function for BCM API bcm_port_tpid_class_get
 * \param [in] unit - relevant unit
 * \param [in] tpid_class - Pointer to a TPID class structure
 *          that includes following input data:
 *          - flags
 *          - port
 *          - tpid1
 *          - tpid2
 * As output, tpid_class - Pointer to a TPID class structure
 *          that includes following output data:
 *          For INGRESS:
 *          - tag_format_class_id
 *          - vlan_translation_action_id
 *          - vlan_translation_qos_map_id
 *          For EGRESS:
 *          - tag_format_class_id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None
 * \see
 *   bcm_dnx_port_tpid_class_get
 */
static shr_error_e
dnx_port_tpid_class_get_verify(
    int unit,
    bcm_port_tpid_class_t * tpid_class)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the tpid_class pointer is not NULL:
     */
    SHR_NULL_CHECK(tpid_class, _SHR_E_PARAM, "tpid_class");

    /*
     * Verify correct flags settings:
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_get_flags_verify(unit, tpid_class->flags));

    /*
     * Verify port:
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_port_verify(unit, tpid_class->flags, tpid_class->port));

    /*
     * Verify that tpid value is valid
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_tpid_verify(unit, tpid_class->tpid1));
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_tpid_verify(unit, tpid_class->tpid2));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Utility function for resolving tpid value to global tpid
 * index.
 * \param [in] unit - Relevant unit.
 * \param [in] is_ingress - is ingress.
 * \param [in] tpid_vals - pointer to array of [2] containing
 *        outer and inner tpid values.
 * \param [out] tpid_indexes - pointer to array of [2][7]
 *        containing the resulted tpid indexs.
 * \param [in] nof_tpid_indexes - pointer to array of [2]
 *        containing the number of tpid indexs resulted.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 * - For Egress, the tpid index is the Global TPID index: [0..6]
 *   the configured TPIDs, 7 is "no valid tpid".
 * - For Ingress, the tpid index is the Ethernet-Layer-Qualifier
 *   TPID index: [1..7] TPIDs, 0 is "no tpid".
 * Special value "any": means all the global TPIDs thus
 *    the number of tpid index found is all the global tpid
 *    indexs (8).
 * - Special value "invalid": means no TPID found thus the
 *   number of tpid index found is 1; for Ingress: tpid index is
 *   0, for Egress: tpid index is 7.
 * \see
 *   * None
 */
static shr_error_e
dnx_port_tpid_class_utility_tpid_indexes_get(
    int unit,
    int is_ingress,
    uint32 tpid_vals[TPID_INDEX_NOF],
    int tpid_indexes[TPID_INDEX_NOF][BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS],
    int nof_tpid_indexes[TPID_INDEX_NOF])
{
    int indx;

    /*
     * Ingress TPID conversion:
     * From Global TPID index to Ethernet-Layer-Qualifier TPID index:
     */
    const int ingress_tpid_index_convert[BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS] = { 1, 2, 3, 4, 5, 6, 7, 0 };
    /*
     * Egress TPID conversion:
     * From Global TPID index Global TPID index:
     */
    const int egress_tpid_index_convert[BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS] = { 0, 1, 2, 3, 4, 5, 6, 7 };

    const int *tpid_index_convert;

    SHR_FUNC_INIT_VARS(unit);

    if (is_ingress == TRUE)
    {
        tpid_index_convert = ingress_tpid_index_convert;
    }
    else
    {
        tpid_index_convert = egress_tpid_index_convert;
    }

    for (indx = 0; indx < TPID_INDEX_NOF; indx++)
    {
        switch (tpid_vals[indx])
        {
            case BCM_PORT_TPID_CLASS_TPID_INVALID:

                /*
                 * Handle "INVALID" TPID ==> BCM_DNX_SWITCH_TPID_RESERVED_ENTRY
                 */
                tpid_indexes[indx][0] = tpid_index_convert[BCM_DNX_SWITCH_TPID_RESERVED_ENTRY];
                nof_tpid_indexes[indx] = 1;
                break;

            case BCM_PORT_TPID_CLASS_TPID_ANY:

                /*
                 * Handle "ANY" TPID ==> ALL global TPIDs
                 */
            {
                int tpid_indx;

                for (tpid_indx = 0; tpid_indx < BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS; tpid_indx++)
                {
                    tpid_indexes[indx][tpid_indx] = tpid_index_convert[tpid_indx];
                }
                nof_tpid_indexes[indx] = BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS;
            }
                break;

            default:
            {
                /*
                 * The TPID is not "INVALID" or "ANY".
                 * Search for the tpid value in global tpids and return its' index.
                 */
                int tmp_tpid_index;
                SHR_IF_ERR_EXIT(dnx_switch_tpid_index_get(unit, (uint16) tpid_vals[indx], &tmp_tpid_index));

                /*
                 * update nof tpids
                 */
                tpid_indexes[indx][0] = tpid_index_convert[tmp_tpid_index];
                nof_tpid_indexes[indx] = 1;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * TAG-Struct handling - internal function.
 * "Book" an HW-ID: update "HW to SW" tables with the SW-ID
 * \see
 * bcm_dnx_port_tpid_class_create
 */
static shr_error_e
dnx_port_tpid_class_hw_id_book(
    int unit,
    int is_symmetric,
    int is_ingress_only,
    int hw_id_ingress_key,
    int hw_id_egress_key,
    int sw_id_value)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Update ingress/egress table on the allocation.
     */
    if ((is_ingress_only == TRUE) || (is_symmetric == TRUE))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TAG_STRUCT_INGRESS_HW_ID_TO_SW_ID, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, hw_id_ingress_key);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_TAG_FORMAT_CLASS_ID, INST_SINGLE,
                                     sw_id_value);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    if ((is_ingress_only == FALSE) || (is_symmetric == TRUE))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TAG_STRUCT_EGRESS_HW_ID_TO_SW_ID, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, hw_id_egress_key);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_TAG_FORMAT_CLASS_ID, INST_SINGLE,
                                     sw_id_value);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * TAG-Struct handling - internal function.
 *  Allocate HW tag-structure ID.
 * Based on the type of the desired TAG-Struct, use the advanced allocation resource manager
 * to allocate ingress and/or egress HW TAG-Struct.
 * \see
 * bcm_dnx_port_tpid_class_create
 */
static shr_error_e
dnx_port_tpid_class_hw_id_alloc(
    int unit,
    bcm_port_tag_struct_type_t tag_struct_type,
    int is_symmetric,
    int is_ingress_only,
    int *tag_struct_hw_id_ingress,
    int *tag_struct_hw_id_egress)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((is_ingress_only == TRUE) || (is_symmetric == TRUE))
    {
        /*
         * Allocate Ingress HW tag-structure value per the specified type:
         */
        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.incoming_tag_structure_ingress.allocate_single
                        (unit, 0, &tag_struct_type, tag_struct_hw_id_ingress));
    }

    if ((is_ingress_only == FALSE) || (is_symmetric == TRUE))
    {
        /*
         * Allocate Egress HW tag-structure value per the specified type:
         */
        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.incoming_tag_structure_egress.allocate_single
                        (unit, 0, &tag_struct_type, tag_struct_hw_id_egress));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file for description
 */
shr_error_e
dnx_port_tpid_class_hw_id_get_info(
    int unit,
    int is_ingress,
    int hw_id_key,
    bcm_port_tag_format_class_t * tag_format_class_id)
{
    uint32 entry_handle_id;
    uint32 dbal_table_id;
    uint32 sw_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Read the HW to SW table
     */
    dbal_table_id = (is_ingress == TRUE) ?
        DBAL_TABLE_TAG_STRUCT_INGRESS_HW_ID_TO_SW_ID : DBAL_TABLE_TAG_STRUCT_EGRESS_HW_ID_TO_SW_ID;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, hw_id_key);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BCM_TAG_FORMAT_CLASS_ID, INST_SINGLE, &sw_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *tag_format_class_id = (bcm_port_tag_format_class_t) sw_id;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * TAG-Struct handling - internal function.
 * Free an allocated HW tag-structure ID.
 * It removes the "booking" of the HW ID at the "HW to SW" tables
 * and frees the HW-ID "allocation" at the resource management.
 * \see
 * bcm_dnx_port_tpid_class_destroy
 */
static shr_error_e
dnx_port_tpid_class_hw_id_free(
    int unit,
    int is_symmetric,
    int is_ingress_only,
    int hw_id_ingress_key,
    int hw_id_egress_key)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * 1. Clear " HW to SW"  table.
     * 2. Free HW tag-structure at the allocation manager.
     */
    if ((is_ingress_only == TRUE) || (is_symmetric == TRUE))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TAG_STRUCT_INGRESS_HW_ID_TO_SW_ID, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, hw_id_ingress_key);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.incoming_tag_structure_ingress.free_single(unit,
                                                                                                      hw_id_ingress_key));
    }

    if ((is_ingress_only == FALSE) || (is_symmetric == TRUE))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TAG_STRUCT_EGRESS_HW_ID_TO_SW_ID, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, hw_id_egress_key);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.incoming_tag_structure_egress.free_single
                        (unit, hw_id_egress_key));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * TAG-Struct handling - internal function.
 * "Book" a SW-ID: update "SW to HW" tables with the HW-ID
 * \see
 * bcm_dnx_port_tpid_class_create
 */
static shr_error_e
dnx_port_tpid_class_sw_id_book(
    int unit,
    int is_symmetric,
    int is_ingress_only,
    bcm_port_tag_format_class_t tag_format_class_id,
    int hw_id_ingress_value,
    int hw_id_egress_value)
{
    uint32 entry_handle_id;
    uint32 sw_id_key;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sw_id_key = tag_format_class_id;

    /*
     * Update ingress/egress table on the allocation.
     */
    if ((is_ingress_only == TRUE) || (is_symmetric == TRUE))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TAG_STRUCT_INGRESS_SW_ID_TO_HW_ID, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_TAG_FORMAT_CLASS_ID, sw_id_key);
        dbal_entry_value_field32_set(unit,
                                     entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, INST_SINGLE,
                                     hw_id_ingress_value);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    if ((is_ingress_only == FALSE) || (is_symmetric == TRUE))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TAG_STRUCT_EGRESS_SW_ID_TO_HW_ID, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_TAG_FORMAT_CLASS_ID, sw_id_key);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, INST_SINGLE,
                                     hw_id_egress_value);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * TAG-Struct handling - internal function.
 *  Allocate SW tag-structure ID.
 * \see
 * bcm_dnx_port_tpid_class_create
 */
static shr_error_e
dnx_port_tpid_class_sw_id_alloc(
    int unit,
    int is_symmetric,
    int is_ingress_only,
    int is_with_id,
    bcm_port_tag_format_class_t * tag_format_class_id)
{
    uint32 flags;
    int tag_struct_sw_id;

    SHR_FUNC_INIT_VARS(unit);

    flags = (is_with_id == TRUE) ? DNX_ALGO_RES_ALLOCATE_WITH_ID : 0;
    tag_struct_sw_id = *tag_format_class_id;

    if (is_symmetric == TRUE)
    {

        /*
         * Allocate Symmetric SW tag-structure:
         * Note:
         * This allocation removes the tag_struct from being available for symmetric allocation.
         * The allocated symmetrical tag_struct is then used for both Ingress and Egress WITH_ID allocation.
         */
        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_symmetric.allocate_single
                        (unit, flags, NULL, &tag_struct_sw_id));

        /*
         * Allocate Ingress SW tag-structure - "WITH ID":
         */
        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_ingress.allocate_single
                        (unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL, &tag_struct_sw_id));

        /*
         * Allocate Egress SW tag-structure - "WITH ID":
         */
        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_egress.allocate_single
                        (unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL, &tag_struct_sw_id));

        /*
         * Mark this SW ID as symmetric :
         * Set the symmetric_bit to indicate that this allocation is symmetric.
         */
        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_symmetric_indication.bit_set
                        (unit, tag_struct_sw_id));

    }
    else
    {
        uint8 is_allocated;

        if (is_ingress_only == TRUE)
        {
            /*
             * Allocate Ingress SW tag-structure:
             * Note:
             * This allocation removes the tag_struct from being available for Ingress allocation.
             * The allocated ingress tag_struct is then used for symmetrical WITH_ID allocation (to also prevent symmetric allocation of this ID).
             */
            SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_ingress.allocate_single
                            (unit, flags, NULL, &tag_struct_sw_id));
        }
        else
        {
            /*
             * Allocate Egress SW tag-structure:
             * This allocation removes the tag_struct from being available for Egress allocation.
             * The allocated egress tag_struct is then used for symmetrical WITH_ID allocation (to also prevent symmetric allocation of this ID).
             */
            SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_egress.allocate_single
                            (unit, flags, NULL, &tag_struct_sw_id));
        }

        /*
         * Allocate Symmetric SW tag-structure - "WITH ID":
         * Note:
         * It can be already allocated in the symmetric DB in case it was previously allocated as Ingress/Egress only, thus need to
         * check first if not allocated:
         */
        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_symmetric.is_allocated
                        (unit, tag_struct_sw_id, &is_allocated));

        if (is_allocated == FALSE)
        {
            SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_symmetric.allocate_single
                            (unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL, &tag_struct_sw_id));
        }
    }

    *tag_format_class_id = (bcm_port_tag_format_class_t) tag_struct_sw_id;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file for description
 */
shr_error_e
dnx_port_tpid_class_sw_id_get_info(
    int unit,
    bcm_port_tag_format_class_t tag_format_class_id,
    int *hw_id_ingress_value,
    int *hw_id_egress_value)
{
    uint32 entry_handle_id;
    uint32 sw_id_key;
    uint32 hw_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sw_id_key = tag_format_class_id;

    /*
     * Read the SW to HW table
     */
    if (hw_id_ingress_value != NULL)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TAG_STRUCT_INGRESS_SW_ID_TO_HW_ID, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_TAG_FORMAT_CLASS_ID, sw_id_key);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, INST_SINGLE, &hw_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        *hw_id_ingress_value = (int) hw_id;
    }

    if (hw_id_egress_value != NULL)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TAG_STRUCT_EGRESS_SW_ID_TO_HW_ID, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_TAG_FORMAT_CLASS_ID, sw_id_key);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, INST_SINGLE, &hw_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        *hw_id_egress_value = (int) hw_id;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * TAG-Struct handling - internal function.
 * Free an allocated SW tag-structure ID.
 * \remark
 * The HW tag-struct ID should be freed separately.
 * \see
 * bcm_dnx_port_tpid_class_destroy
 */
static shr_error_e
dnx_port_tpid_class_sw_id_free(
    int unit,
    int is_symmetric,
    int is_ingress_only,
    int sw_id_key)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * 1. Clear " SW to HW"  table.
     */
    if ((is_ingress_only == TRUE) || (is_symmetric == TRUE))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TAG_STRUCT_INGRESS_SW_ID_TO_HW_ID, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_TAG_FORMAT_CLASS_ID, sw_id_key);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

    if ((is_ingress_only == FALSE) || (is_symmetric == TRUE))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TAG_STRUCT_EGRESS_SW_ID_TO_HW_ID, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_TAG_FORMAT_CLASS_ID, sw_id_key);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * 2. Free SW tag-structure at the allocation manager.
     */
    if (is_symmetric == TRUE)
    {
        /*
         * Free Symmetric SW tag-structure:
         */
        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_symmetric.free_single
                        (unit, sw_id_key));
        /*
         * Free Ingress SW tag-structure:
         */
        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_ingress.free_single
                        (unit, sw_id_key));

        /*
         * Allocate Egress SW tag-structure:
         */
        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_egress.free_single(unit, sw_id_key));

        /*
         * Unmark this SW ID as symmetric :
         */
        SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_symmetric_indication.bit_clear
                        (unit, sw_id_key));

    }
    else
    {
        uint8 is_allocated;

        if (is_ingress_only == TRUE)
        {
            /*
             * Free Ingress SW tag-structure:
             */
            SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_ingress.free_single
                            (unit, sw_id_key));

            /*
             * Free Symmetric SW tag-structure:
             * Note:
             * Before freeing  the Symmetric SW tag-struct, need to check that there is no same SW-ID key at the Egress
             */
            SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_egress.is_allocated
                            (unit, sw_id_key, &is_allocated));
        }
        else
        {
            /*
             * Allocate Egress SW tag-structure:
             */
            SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_egress.free_single
                            (unit, sw_id_key));
            /*
             * Free Symmetric SW tag-structure:
             * Note:
             * Before freeing the Symmetric SW tag-struct, need to check that there is no same SW-ID key at the ingress
             */
            SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_ingress.is_allocated
                            (unit, sw_id_key, &is_allocated));
        }

        if (is_allocated == FALSE)
        {
            SHR_IF_ERR_EXIT(port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_symmetric.free_single
                            (unit, sw_id_key));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function for converting bcm_port_tpid_class_t to
 * Ingress LLVP entry internal buffer
 *
 * \param [in] unit - relevant unit
 * \param [in] tpid_class - Pointer to a TPID class structure
 * \param [out] ingress_buff - Pointer to buffer which holds the
 *        generated Ingress LLVP entry data.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_tpid_class_init_default
 *   bcm_dnx_port_tpid_class_set
 */
static shr_error_e
dnx_port_tpid_class_set_ingress_tpid_class_to_buff(
    int unit,
    bcm_port_tpid_class_t * tpid_class,
    uint32 *ingress_buff)
{
    uint32 temp_value = 0;
    int tag_struct_hw_id_ingress;

    SHR_FUNC_INIT_VARS(unit);

    *ingress_buff = 0;

    /*
     * For ingress, all buffer bits are needed.
     */

    /*
     * 18:18 inner-cep
     */
    temp_value = (tpid_class->flags & BCM_PORT_TPID_CLASS_INNER_C) ? 1 : 0;
    temp_value = temp_value << 18;
    *ingress_buff |= temp_value;
    /*
     * 17:17 outer-cep
     */
    temp_value = (tpid_class->flags & BCM_PORT_TPID_CLASS_OUTER_C) ? 1 : 0;
    temp_value = temp_value << 17;
    *ingress_buff |= temp_value;
    /*
     * 16:16 discard
     */
    temp_value = (tpid_class->flags & BCM_PORT_TPID_CLASS_DISCARD) ? 1 : 0;
    *ingress_buff |= temp_value << 16;

    /*
     * 15:11 IncomingTagStructure
     */
    /*
     * Get HW tag-structure value:
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_sw_id_get_info
                    (unit, tpid_class->tag_format_class_id, &tag_struct_hw_id_ingress, NULL));

    temp_value = tag_struct_hw_id_ingress;
    temp_value = temp_value << 11;
    *ingress_buff |= temp_value;

    /*
     * 10:4 IVEC
     */
    temp_value = tpid_class->vlan_translation_action_id;
    *ingress_buff |= temp_value << 4;

    /*
     * 3:0  PCP-DEI-PROFILE
     */
    temp_value = tpid_class->vlan_translation_qos_map_id;
    *ingress_buff |= temp_value;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function for converting bcm_port_tpid_class_t to
 * Egress LLVP entry internal buffer
 *
 * \param [in] unit - relevant unit
 * \param [in] tpid_class - Pointer to a TPID class structure
 * \param [out] egress_buff - Pointer to buffer which holds the
 *        generated Egress LLVP entry data.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_tpid_class_init_default
 *   bcm_dnx_port_tpid_class_set
 */
static shr_error_e
dnx_port_tpid_class_set_egress_tpid_class_to_buff(
    int unit,
    bcm_port_tpid_class_t * tpid_class,
    uint32 *egress_buff)
{
    uint32 temp_value = 0;
    int tag_struct_hw_id_egress;

    SHR_FUNC_INIT_VARS(unit);

    *egress_buff = 0;

    /*
     * For egress, only TagFormat and CEP bits are needed.
     */

    /*
     * 18:18 inner-cep
     */
    temp_value = (tpid_class->flags & BCM_PORT_TPID_CLASS_INNER_C) ? 1 : 0;
    temp_value = temp_value << 18;
    *egress_buff |= temp_value;
    /*
     * 17:17 outer-cep
     */
    temp_value = (tpid_class->flags & BCM_PORT_TPID_CLASS_OUTER_C) ? 1 : 0;
    temp_value = temp_value << 17;
    *egress_buff |= temp_value;
    /*
     * 15:11 IncomingTagStructure
     */

    /*
     * Get HW tag-structure value:
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_sw_id_get_info
                    (unit, tpid_class->tag_format_class_id, NULL, &tag_struct_hw_id_egress));

    temp_value = tag_struct_hw_id_egress;
    temp_value = temp_value << 11;
    *egress_buff |= temp_value;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_tpid_class_set_buff_to_tpid_class(
    int unit,
    uint32 buff,
    bcm_port_tpid_class_t * tpid_class)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * 4 bit PCP-DEI-PROFILE
     */
    tpid_class->vlan_translation_qos_map_id = buff & 0xF;
    buff = buff >> 4;

    /*
     * 7 bit IVEC
     */
    tpid_class->vlan_translation_action_id = buff & 0x7F;
    buff = buff >> 7;

    /*
     * 5 bit IncomingTagStructure
     */
    tpid_class->tag_format_class_id = buff & 0x1F;
    buff = buff >> 5;

    /*
     * 1 bit discard
     */
    tpid_class->flags |= (buff & 0x1) ? BCM_PORT_TPID_CLASS_DISCARD : 0;
    buff = buff >> 1;

    /*
     * 1 bit outer-cep
     */
    tpid_class->flags |= (buff & 0x1) ? BCM_PORT_TPID_CLASS_OUTER_C : 0;
    buff = buff >> 1;

    /*
     * 1 bit inner-cep
     */
    tpid_class->flags |= (buff & 0x1) ? BCM_PORT_TPID_CLASS_INNER_C : 0;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_tpid_class_set_tag_priority_get(
    int unit,
    bcm_port_tpid_class_t * tpid_class,
    int *tag_priority_val,
    int *nof_tag_priority_vals)
{
    SHR_FUNC_INIT_VARS(unit);

    if (tpid_class->flags & BCM_PORT_TPID_CLASS_OUTER_IS_PRIO)
    {
        /*
         * BCM_PORT_TPID_CLASS_OUTER_IS_PRIO - outer tag is a priority tag
         */
        tag_priority_val[0] = 1;
        *nof_tag_priority_vals = 1;
    }
    else if (tpid_class->flags & BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO)
    {
        /*
         * BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO - outer tag is not a priority tag
         */
        tag_priority_val[0] = 0;
        *nof_tag_priority_vals = 1;
    }
    else
    {
        /*
         * No notion whether outer tag is priority tag or not.
         * In this case create two entries, for outer tag is priority tag
         * and outer tag is not a priority tag.
         */
        tag_priority_val[0] = 0;
        tag_priority_val[1] = 1;
        *nof_tag_priority_vals = 2;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file for description
 */
shr_error_e
dnx_port_tpid_class_set_parser_info_to_ingress_llvp_index(
    int unit,
    dnx_port_tpid_llvp_parser_info_t * llvp_parser_info,
    int *tag_structure_index)
{
    SHR_FUNC_INIT_VARS(unit);

    *tag_structure_index = (llvp_parser_info->is_outer_prio & 0x1) << 6;
    *tag_structure_index |= (llvp_parser_info->outer_tpid & 0x7) << 3;
    *tag_structure_index |= (llvp_parser_info->inner_tpid & 0x7);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file for definition
 */
shr_error_e
dnx_port_tpid_class_set_parser_info_to_egress_llvp_index(
    int unit,
    dnx_port_tpid_llvp_parser_info_t * llvp_parser_info,
    int *tag_structure_index)
{
    SHR_FUNC_INIT_VARS(unit);

    *tag_structure_index = (llvp_parser_info->outer_tpid & 0x7) << 4;
    *tag_structure_index |= (llvp_parser_info->is_outer_prio & 0x1) << 3;
    *tag_structure_index |= (llvp_parser_info->inner_tpid & 0x7);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_tpid_class_set_tpid_class_buffer_update(
    int unit,
    int is_ingress,
    dnx_port_tpid_llvp_parser_info_t * llvp_parser_info,
    uint32 tag_buff,
    uint32 *tpid_class_buffer)
{
    int tag_structure_index;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get LLVP key for a LLVP profile (Reminder: LLVP is divided by LLVP profile)
     */
    if (is_ingress == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_parser_info_to_ingress_llvp_index
                        (unit, llvp_parser_info, &tag_structure_index));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_parser_info_to_egress_llvp_index
                        (unit, llvp_parser_info, &tag_structure_index));
    }

    SHR_BITCOPY_RANGE(tpid_class_buffer,
                      DNX_PORT_TPID_BUFFER_BITS_PER_TAG_STRCT * tag_structure_index,
                      &tag_buff, 0, DNX_PORT_TPID_BUFFER_BITS_PER_TAG_STRCT);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Utility function for update tpid class block
 * \param [in] unit - Device number
 * \param [in] is_ingress - ingress TRUE, egress FALSE
 * \param [in] tpid_class - Pointer to a TPID class structure that includes the port
 *          and the data to set.
 * \param [in] tag_priority_vals - pointer to array of two cells, containing outer and inner priority tag
 * \param [in] nof_tag_priority_vals - number of priority tag (0,1 or 2)
 * \param [out] port_tpid_indexes - pointer to array of cells, containing outer and inner tpid indexs
 * \param [out] nof_port_tpid_indexes - pointer to array of cells, containing numer of outer and inner tpid indexs
 * \param [out] tag_buff - pointer to buffer holding one llvp entry (in llvp template format)
 * \param [out] tpid_class_buffer -pointer to buffer holding whole llvp profile (in llvp template format)
 *
 * \return
 *   None
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_port_tpid_class_set
 */
static shr_error_e
dnx_port_tpid_class_set_tpid_class_block_update(
    int unit,
    int is_ingress,
    bcm_port_tpid_class_t * tpid_class,
    int tag_priority_vals[2],
    int nof_tag_priority_vals,
    int port_tpid_indexes[TPID_INDEX_NOF][BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS],
    int nof_port_tpid_indexes[TPID_INDEX_NOF],
    uint32 *tag_buff,
    uint32 *tpid_class_buffer)
{
    int idx1, idx2, idx3;

    uint32 tpid_vals[TPID_INDEX_NOF];

    SHR_FUNC_INIT_VARS(unit);

    /*
     * calculate TPID indexes for the given two TPIDs
     */
    tpid_vals[TPID_INDEX_OUTER] = tpid_class->tpid1;
    tpid_vals[TPID_INDEX_INNER] = tpid_class->tpid2;
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_utility_tpid_indexes_get
                    (unit, is_ingress, tpid_vals, port_tpid_indexes, nof_port_tpid_indexes));

    /*
     * calculate new data buffer for tag class (one llvp entry)
     */
    if (is_ingress == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_ingress_tpid_class_to_buff(unit, tpid_class, tag_buff));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_egress_tpid_class_to_buff(unit, tpid_class, tag_buff));
    }

    /*
     * Update template with new LLVP fields
     */
    for (idx1 = 0; idx1 < nof_port_tpid_indexes[TPID_INDEX_OUTER]; idx1++)
    {
        for (idx2 = 0; idx2 < nof_port_tpid_indexes[TPID_INDEX_INNER]; idx2++)
        {
            for (idx3 = 0; idx3 < nof_tag_priority_vals; idx3++)
            {
                dnx_port_tpid_llvp_parser_info_t llvp_parser_info;
                llvp_parser_info.outer_tpid = port_tpid_indexes[TPID_INDEX_OUTER][idx1];
                llvp_parser_info.inner_tpid = port_tpid_indexes[TPID_INDEX_INNER][idx2];
                llvp_parser_info.is_outer_prio = tag_priority_vals[idx3];

                /*
                 * Update the tpid class buffer with tag buffer
                 */
                SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_tpid_class_buffer_update
                                (unit, is_ingress, &llvp_parser_info, *tag_buff, tpid_class_buffer));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Utility function for setting LLVP INGRESS/EGRESS Keys
 * \param [in] unit - Device number
 * \param [in] entry_handle_id - the DBAL table handle
 * \param [in] llvp_profile - the LLVP profile
 * \param [in] llvp_parser_info - pointer to Parser fields that
 *        are part of LLVP table key
 * \param [in] is_ingress - indicate set key for ingress llvp
 *
 * \return
 *   None
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_tpid_class_set_ingress_llvp_entry_to_dbal
 *   dnx_port_tpid_class_set_egress_llvp_entry_to_dbal
 *   dnx_port_tpid_class_get_ingress_llvp_entry_from_dbal
 *   dnx_port_tpid_class_get_egress_llvp_entry_from_dbal
 */
static void
dnx_port_tpid_class_utility_llvp_entry_dbal_keys_set(
    int unit,
    uint32 entry_handle_id,
    int llvp_profile,
    dnx_port_tpid_llvp_parser_info_t * llvp_parser_info,
    int is_ingress)
{
    uint32 is_ext_vcp_enable;

    is_ext_vcp_enable = dnx_data_vlan.llvp.ext_vcp_en_get(unit);

    /*
     * Set key fields
     */
    if (is_ingress == TRUE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TPID, llvp_parser_info->outer_tpid);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_TPID, llvp_parser_info->inner_tpid);
        if (!is_ext_vcp_enable)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_OUTER_PRIO,
                                       llvp_parser_info->is_outer_prio);
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_LLVP_PROFILE, llvp_profile);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LLVP_PROFILE, llvp_profile);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_OUTER_PRIO, llvp_parser_info->is_outer_prio);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TPID, llvp_parser_info->outer_tpid);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_TPID, llvp_parser_info->inner_tpid);
    }
}

shr_error_e
dnx_port_tpid_class_set_ingress_llvp_entry_to_dbal(
    int unit,
    int llvp_profile,
    dnx_port_tpid_llvp_parser_info_t * llvp_parser_info,
    dnx_port_tpid_ingress_llvp_entry_t * dnx_port_tpid_ingress_llvp_entry)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_LLVP_CLASSIFICATION, &entry_handle_id));

    /*
     * Set key fields
     */
    dnx_port_tpid_class_utility_llvp_entry_dbal_keys_set(unit, entry_handle_id, llvp_profile, llvp_parser_info, TRUE);

    /*
     * Set data fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_VID_EXIST, INST_SINGLE,
                                 dnx_port_tpid_ingress_llvp_entry->incoming_vid_exist);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_EXIST, INST_SINGLE,
                                 dnx_port_tpid_ingress_llvp_entry->incoming_tag_exist);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_S_TAG_EXIST, INST_SINGLE,
                                 dnx_port_tpid_ingress_llvp_entry->incoming_s_tag_exist);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCEPTABLE_FRAME_TYPE_ACTION, INST_SINGLE,
                                 dnx_port_tpid_ingress_llvp_entry->frame_type_action);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, INST_SINGLE,
                                 dnx_port_tpid_ingress_llvp_entry->llvp_tag_format);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IVEC_INDEX, INST_SINGLE,
                                 dnx_port_tpid_ingress_llvp_entry->ivec_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLVP_PCP_DEI_PROFILE, INST_SINGLE,
                                 dnx_port_tpid_ingress_llvp_entry->pcp_dei_profile);
    if (dnx_port_tpid_ingress_llvp_entry->llvp_tag_format == DNX_VLAN_FORMAT_NO_TAG)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLVP_PCP_DEI_MAP_TYPE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_PCP_DEI_MAP_TYPE_MAPPING);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLVP_PCP_DEI_MAP_TYPE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_PCP_DEI_MAP_TYPE_EXPLICIT);
    }

    /*
     * write to HW and release handle
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Write single entry to LLVP table
 */
shr_error_e
dnx_port_tpid_class_set_egress_llvp_entry_to_dbal(
    int unit,
    int llvp_profile,
    dnx_port_tpid_llvp_parser_info_t * llvp_parser_info,
    dnx_port_tpid_egress_llvp_entry_t * egress_llvp_entry)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_LLVP_CLASSIFICATION, &entry_handle_id));

    /*
     * Set key fields
     */
    dnx_port_tpid_class_utility_llvp_entry_dbal_keys_set(unit, entry_handle_id, llvp_profile, llvp_parser_info, FALSE);

    /*
     * Set data fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_C_TAG_OFFSET_TYPE, INST_SINGLE,
                                 egress_llvp_entry->c_tag_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_HAS_C_TAG, INST_SINGLE,
                                 egress_llvp_entry->packet_has_c_tag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_HAS_PCP_DEI, INST_SINGLE,
                                 egress_llvp_entry->packet_has_pcp_dei);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, INST_SINGLE,
                                 egress_llvp_entry->llvp_tag_format);

    /*
     * write to HW and release handle
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Read a single entry from INGRESS LLVP table
 * \param [in] unit - relevant unit.
 * \param [in] llvp_profile - the llvp profile.
 * \param [in] llvp_parser_info - pointer to Parser fields that
 *        are part of LLVP table key
 * \param [out] dnx_port_tpid_ingress_llvp_entry - pointer to
 *        ingress LLVP table payload
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_port_tpid_class_get
 */
static shr_error_e
dnx_port_tpid_class_get_ingress_llvp_entry_from_dbal(
    int unit,
    int llvp_profile,
    dnx_port_tpid_llvp_parser_info_t * llvp_parser_info,
    dnx_port_tpid_ingress_llvp_entry_t * dnx_port_tpid_ingress_llvp_entry)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_LLVP_CLASSIFICATION, &entry_handle_id));

    /*
     * Set key fields
     */
    dnx_port_tpid_class_utility_llvp_entry_dbal_keys_set(unit, entry_handle_id, llvp_profile, llvp_parser_info, TRUE);

    /*
     * Get data fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INCOMING_VID_EXIST, INST_SINGLE,
                               (uint32 *) &(dnx_port_tpid_ingress_llvp_entry->incoming_vid_exist));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_EXIST, INST_SINGLE,
                               (uint32 *) &(dnx_port_tpid_ingress_llvp_entry->incoming_tag_exist));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INCOMING_S_TAG_EXIST, INST_SINGLE,
                               (uint32 *) &(dnx_port_tpid_ingress_llvp_entry->incoming_s_tag_exist));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ACCEPTABLE_FRAME_TYPE_ACTION, INST_SINGLE,
                               (uint32 *) &(dnx_port_tpid_ingress_llvp_entry->frame_type_action));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, INST_SINGLE,
                               (uint32 *) &(dnx_port_tpid_ingress_llvp_entry->llvp_tag_format));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IVEC_INDEX, INST_SINGLE,
                               (uint32 *) &(dnx_port_tpid_ingress_llvp_entry->ivec_index));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LLVP_PCP_DEI_PROFILE, INST_SINGLE,
                               (uint32 *) &(dnx_port_tpid_ingress_llvp_entry->pcp_dei_profile));

    /*
     * Read from HW
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Read a single entry from EGRESS LLVP table
 * \param [in] unit - relevant unit.
 * \param [in] llvp_profile - the llvp profile.
 * \param [in] llvp_parser_info - pointer to Parser fields that
 *        are part of LLVP table key
 * \param [out] egress_llvp_entry - pointer to egress LLVP table
 *        payload
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_port_tpid_class_get
 */
static shr_error_e
dnx_port_tpid_class_get_egress_llvp_entry_from_dbal(
    int unit,
    int llvp_profile,
    dnx_port_tpid_llvp_parser_info_t * llvp_parser_info,
    dnx_port_tpid_egress_llvp_entry_t * egress_llvp_entry)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_LLVP_CLASSIFICATION, &entry_handle_id));

    /*
     * Set key fields
     */
    dnx_port_tpid_class_utility_llvp_entry_dbal_keys_set(unit, entry_handle_id, llvp_profile, llvp_parser_info, FALSE);

    /*
     * Read data fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_C_TAG_OFFSET_TYPE, INST_SINGLE,
                               (uint32 *) &(egress_llvp_entry->c_tag_offset));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PACKET_HAS_C_TAG, INST_SINGLE,
                               (uint32 *) &(egress_llvp_entry->packet_has_c_tag));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PACKET_HAS_PCP_DEI, INST_SINGLE,
                               (uint32 *) &(egress_llvp_entry->packet_has_pcp_dei));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, INST_SINGLE,
                               (uint32 *) &(egress_llvp_entry->llvp_tag_format));

    /*
     * Read from HW
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_tpid_class_set_ingress_llvp_entry_get(
    int unit,
    dnx_port_tpid_llvp_parser_info_t * llvp_parser_info,
    uint32 ingress_tag_buff,
    dnx_port_tpid_ingress_llvp_entry_t * dnx_port_tpid_ingress_llvp_entry)
{
    bcm_port_tpid_class_t tpid_class;

    SHR_FUNC_INIT_VARS(unit);
    bcm_port_tpid_class_t_init(&tpid_class);

    /*
     * Convert tag buffer to tpid class object
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_buff_to_tpid_class(unit, ingress_tag_buff, &tpid_class));

    /*
     * incoming tag exist if tag format is not zero
     */
    dnx_port_tpid_ingress_llvp_entry->incoming_tag_exist = (tpid_class.tag_format_class_id != 0);

    /*
     * vid exist if incoming tag exist and it's not priority
     */
    dnx_port_tpid_ingress_llvp_entry->incoming_vid_exist =
        (dnx_port_tpid_ingress_llvp_entry->incoming_tag_exist && !llvp_parser_info->is_outer_prio);

    /*
     * S-Tag exist if tagged and not C-Tag
     */
    dnx_port_tpid_ingress_llvp_entry->incoming_s_tag_exist =
        (dnx_port_tpid_ingress_llvp_entry->incoming_tag_exist && !(tpid_class.flags & BCM_PORT_TPID_CLASS_OUTER_C));

    /*
     * Set Acceptable-Frame-Type-Action
     */
    if (tpid_class.flags & BCM_PORT_TPID_CLASS_DISCARD)
    {
        dnx_port_tpid_ingress_llvp_entry->frame_type_action = DBAL_ENUM_FVAL_ACCEPTABLE_FRAME_TYPE_FILTER_ACTION_DROP;
    }
    else
    {
        dnx_port_tpid_ingress_llvp_entry->frame_type_action = DBAL_ENUM_FVAL_ACCEPTABLE_FRAME_TYPE_FILTER_ACTION_ACCEPT;
    }

    dnx_port_tpid_ingress_llvp_entry->llvp_tag_format = tpid_class.tag_format_class_id;
    dnx_port_tpid_ingress_llvp_entry->ivec_index = tpid_class.vlan_translation_action_id;
    dnx_port_tpid_ingress_llvp_entry->pcp_dei_profile = tpid_class.vlan_translation_qos_map_id;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_tpid_class_set_egress_llvp_entry_get(
    int unit,
    uint32 egress_tag_buff,
    dnx_port_tpid_egress_llvp_entry_t * dnx_port_tpid_egress_llvp_entry)
{
    int is_outer_c_tag;
    int is_inner_c_tag;
    int is_outer_s_tag;
    bcm_port_tpid_class_t tpid_class;

    SHR_FUNC_INIT_VARS(unit);
    bcm_port_tpid_class_t_init(&tpid_class);

    SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_buff_to_tpid_class(unit, egress_tag_buff, &tpid_class));

    /*
     * c-tag according to user indication
     */
    is_outer_c_tag = (tpid_class.flags & BCM_PORT_TPID_CLASS_OUTER_C) ? 1 : 0;
    is_inner_c_tag = (tpid_class.flags & BCM_PORT_TPID_CLASS_INNER_C) ? 1 : 0;
    /*
     * s-tag, if tagged and not ctag
     */
    is_outer_s_tag = (tpid_class.tag_format_class_id != 0) && (!is_outer_c_tag);

    
    dnx_port_tpid_egress_llvp_entry->c_tag_offset = DBAL_ENUM_FVAL_C_TAG_OFFSET_TYPE_OUTER_C_TAG;
    dnx_port_tpid_egress_llvp_entry->packet_has_pcp_dei = 0;
    dnx_port_tpid_egress_llvp_entry->llvp_tag_format = 0;
    dnx_port_tpid_egress_llvp_entry->packet_has_c_tag = 0;

    if (is_outer_c_tag)
    {
        dnx_port_tpid_egress_llvp_entry->c_tag_offset = DBAL_ENUM_FVAL_C_TAG_OFFSET_TYPE_OUTER_C_TAG;
        /*
         * is outer C-tag
         */
        dnx_port_tpid_egress_llvp_entry->packet_has_c_tag = TRUE;
    }
    else if (is_inner_c_tag)
    {
        dnx_port_tpid_egress_llvp_entry->c_tag_offset = DBAL_ENUM_FVAL_C_TAG_OFFSET_TYPE_INNER_C_TAG;
        /*
         * is outer C-tag
         */
        dnx_port_tpid_egress_llvp_entry->packet_has_c_tag = TRUE;
    }

    /*
     * if outer is S-Tag then has PCP-DEI
     */
    if (is_outer_s_tag)
    {
        dnx_port_tpid_egress_llvp_entry->packet_has_pcp_dei = TRUE;
    }

    dnx_port_tpid_egress_llvp_entry->llvp_tag_format = tpid_class.tag_format_class_id;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_tpid_class_set_egress_llvp_entry_write(
    int unit,
    int llvp_profile,
    dnx_port_tpid_llvp_parser_info_t * llvp_parser_info,
    uint32 egress_tag_buff)
{
    dnx_port_tpid_egress_llvp_entry_t llvp_entry;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Convert buffer and parser info to egress LLVP entry
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_egress_llvp_entry_get(unit, egress_tag_buff, &llvp_entry));

    /*
     * Write the entry to HW
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_egress_llvp_entry_to_dbal
                    (unit, llvp_profile, llvp_parser_info, &llvp_entry));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_tpid_class_set_ingress_llvp_entry_write(
    int unit,
    int llvp_profile,
    dnx_port_tpid_llvp_parser_info_t * llvp_parser_info,
    uint32 ingress_tag_buff)
{
    dnx_port_tpid_ingress_llvp_entry_t llvp_entry;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Convert buffer and parser info to ingress LLVP entry
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_ingress_llvp_entry_get
                    (unit, llvp_parser_info, ingress_tag_buff, &llvp_entry));

    /*
     * Write the entry to HW
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_ingress_llvp_entry_to_dbal
                    (unit, llvp_profile, llvp_parser_info, &llvp_entry));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_tpid_class_set_ingress_llvp_block_write(
    int unit,
    int llvp_profile,
    uint32 *tpid_class_buffer)
{
    int idx;
    uint32 entry_handle_id;
    uint32 temp_buffer_entry;
    uint32 is_ext_vcp_enable;
    int llvp_block_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_ext_vcp_enable = dnx_data_vlan.llvp.ext_vcp_en_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_LLVP_CLASSIFICATION, &entry_handle_id));
    if (is_ext_vcp_enable)
    {
        /** when ext_vcp_enable, the key is 3+3 */
        llvp_block_size = DNX_PORT_TPID_LLVP_BLOCK_SIZE >> 1;
    }
    else
    {
        llvp_block_size = DNX_PORT_TPID_LLVP_BLOCK_SIZE;
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_LLVP_PROFILE, llvp_profile);

    /*
     * Go over all entries
     */
    for (idx = 0; idx < llvp_block_size; idx++)
    {
        int temp;
        dnx_port_tpid_llvp_parser_info_t llvp_parser_info;
        dnx_port_tpid_ingress_llvp_entry_t llvp_entry;
        temp = idx;
        llvp_parser_info.inner_tpid = temp & 0x7;
        temp = temp >> 3;

        llvp_parser_info.outer_tpid = temp & 0x7;
        temp = temp >> 3;

        llvp_parser_info.is_outer_prio = temp & 0x1;

        if (!is_ext_vcp_enable)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_OUTER_PRIO, llvp_parser_info.is_outer_prio);
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TPID, llvp_parser_info.outer_tpid);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_TPID, llvp_parser_info.inner_tpid);

        SHR_BITCOPY_RANGE(&temp_buffer_entry,
                          0,
                          tpid_class_buffer,
                          DNX_PORT_TPID_BUFFER_BITS_PER_TAG_STRCT * idx, DNX_PORT_TPID_BUFFER_BITS_PER_TAG_STRCT);

        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_ingress_llvp_entry_get
                        (unit, &llvp_parser_info, temp_buffer_entry, &llvp_entry));

        /*
         * Set data fields
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_VID_EXIST, INST_SINGLE,
                                     llvp_entry.incoming_vid_exist);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_EXIST, INST_SINGLE,
                                     llvp_entry.incoming_tag_exist);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_S_TAG_EXIST, INST_SINGLE,
                                     llvp_entry.incoming_s_tag_exist);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCEPTABLE_FRAME_TYPE_ACTION, INST_SINGLE,
                                     llvp_entry.frame_type_action);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, INST_SINGLE,
                                     llvp_entry.llvp_tag_format);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IVEC_INDEX, INST_SINGLE, llvp_entry.ivec_index);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLVP_PCP_DEI_PROFILE, INST_SINGLE,
                                     llvp_entry.pcp_dei_profile);
        if (llvp_entry.llvp_tag_format == DNX_VLAN_FORMAT_NO_TAG)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLVP_PCP_DEI_MAP_TYPE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_PCP_DEI_MAP_TYPE_MAPPING);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLVP_PCP_DEI_MAP_TYPE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_PCP_DEI_MAP_TYPE_EXPLICIT);
        }

        /*
         * write to HW and don't release handle
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_tpid_class_set_egress_llvp_block_write(
    int unit,
    int llvp_profile,
    uint32 *tpid_class_buffer)
{
    int idx;
    int temp;
    uint32 entry_handle_id;
    uint32 temp_buffer_entry;
    dnx_port_tpid_llvp_parser_info_t llvp_parser_info;
    dnx_port_tpid_egress_llvp_entry_t llvp_entry;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Go over all entries
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_LLVP_CLASSIFICATION, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LLVP_PROFILE, llvp_profile);

    for (idx = 0; idx < DNX_PORT_TPID_LLVP_BLOCK_SIZE; idx++)
    {
        temp = idx;
        llvp_parser_info.inner_tpid = temp & 0x7;
        temp = temp >> 3;

        llvp_parser_info.is_outer_prio = temp & 0x1;
        temp = temp >> 1;

        llvp_parser_info.outer_tpid = temp & 0x7;

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_OUTER_PRIO, llvp_parser_info.is_outer_prio);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TPID, llvp_parser_info.outer_tpid);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_TPID, llvp_parser_info.inner_tpid);

        SHR_BITCOPY_RANGE(&temp_buffer_entry,
                          0,
                          tpid_class_buffer,
                          DNX_PORT_TPID_BUFFER_BITS_PER_TAG_STRCT * idx, DNX_PORT_TPID_BUFFER_BITS_PER_TAG_STRCT);

        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_egress_llvp_entry_get(unit, temp_buffer_entry, &llvp_entry));

        /*
         * Set data fields
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_C_TAG_OFFSET_TYPE, INST_SINGLE,
                                     llvp_entry.c_tag_offset);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_HAS_C_TAG, INST_SINGLE,
                                     llvp_entry.packet_has_c_tag);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_HAS_PCP_DEI, INST_SINGLE,
                                     llvp_entry.packet_has_pcp_dei);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, INST_SINGLE,
                                     llvp_entry.llvp_tag_format);

        /*
         * write to HW and don't release handle
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Utility function for writing tpid class block to HW
 * \param [in] unit - Device number
 * \param [in] is_ingress - ingress TRUE, egress FALSE
 * \param [in] tag_priority_vals - pointer to array of two cells, containing outer and inner priority tag
 * \param [in] nof_tag_priority_vals - number of priority tag (0,1 or 2)
 * \param [in] port_tpid_indexes - pointer to array of cells, containing outer and inner tpid indexs
 * \param [in] nof_port_tpid_indexes - pointer to array of cells, containing numer of outer and inner tpid indexs
 * \param [in] old_llvp_profile - the old llvp profile number
 * \param [in] new_llvp_profile - the new llvp profile number
 * \param [in] first_reference - is the new llvp profile is referenced on the first time (thus need to be written to HW)
 * \param [in] tag_buff - buffer holding one llvp entry (in llvp template format)
 * \param [in] tpid_class_buffer -pointer to buffer holding whole llvp profile (in llvp template format)
 *
 * \return
 *   None
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_port_tpid_class_set
 */
static shr_error_e
dnx_port_tpid_class_set_llvp_block_write(
    int unit,
    int is_ingress,
    int tag_priority_vals[2],
    int nof_tag_priority_vals,
    int port_tpid_indexes[TPID_INDEX_NOF][BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS],
    int nof_port_tpid_indexes[TPID_INDEX_NOF],
    int old_llvp_profile,
    int new_llvp_profile,
    uint8 first_reference,
    uint32 tag_buff,
    uint32 *tpid_class_buffer)
{
    int idx1, idx2, idx3;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * If new port profile was created, write LLVP entries to HW
     */
    if (first_reference == TRUE)
    {
        if (old_llvp_profile == new_llvp_profile)
        {
            /*
             * if new template equal to old template we only need to update the
             * new LLVP entries
             */
            for (idx1 = 0; idx1 < nof_port_tpid_indexes[TPID_INDEX_OUTER]; idx1++)
            {
                for (idx2 = 0; idx2 < nof_port_tpid_indexes[TPID_INDEX_INNER]; idx2++)
                {
                    for (idx3 = 0; idx3 < nof_tag_priority_vals; idx3++)
                    {
                        dnx_port_tpid_llvp_parser_info_t llvp_parser_info;
                        llvp_parser_info.outer_tpid = port_tpid_indexes[TPID_INDEX_OUTER][idx1];
                        llvp_parser_info.inner_tpid = port_tpid_indexes[TPID_INDEX_INNER][idx2];
                        llvp_parser_info.is_outer_prio = tag_priority_vals[idx3];

                        /*
                         * Write to HW
                         */
                        if (is_ingress == TRUE)
                        {
                            SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_ingress_llvp_entry_write
                                            (unit, new_llvp_profile, &llvp_parser_info, tag_buff));
                        }
                        else
                        {
                            SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_egress_llvp_entry_write
                                            (unit, new_llvp_profile, &llvp_parser_info, tag_buff));
                        }
                    }
                }
            }
        }
        else
        {
            /*
             * a new template was created, we copy all buffer to the new locations
             */
            if (is_ingress == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_ingress_llvp_block_write
                                (unit, new_llvp_profile, tpid_class_buffer));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_egress_llvp_block_write
                                (unit, new_llvp_profile, tpid_class_buffer));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (port_tpid.h).
 */
shr_error_e
dnx_port_tpid_class_set_ingress_llvp_block_clear(
    int unit,
    int llvp_profile)
{
    uint32 entry_handle_id;
    uint32 is_ext_vcp_enable;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_ext_vcp_enable = dnx_data_vlan.llvp.ext_vcp_en_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_LLVP_CLASSIFICATION, &entry_handle_id));

    if (is_ext_vcp_enable)
    {
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_INNER_TPID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TPID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_LLVP_PROFILE, llvp_profile);
    }
    else
    {
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_INNER_TPID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TPID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IS_OUTER_PRIO, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_LLVP_PROFILE, llvp_profile);
    }

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (port_tpid.h).
 */
shr_error_e
dnx_port_tpid_class_set_egress_llvp_block_clear(
    int unit,
    int llvp_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_LLVP_CLASSIFICATION, &entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_INNER_TPID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IS_OUTER_PRIO, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TPID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LLVP_PROFILE, llvp_profile);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (port_tpid.h).
 */
shr_error_e
dnx_port_tpid_class_ingress_vlan_domain_llvp_profile_set(
    int unit,
    int vlan_domain,
    int llvp_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Update LLR per Vlan Domain table with LLVP profile
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_VLAN_DOMAIN_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_LLVP_PROFILE, INST_SINGLE, llvp_profile);
    /*
     * write to HW and release handle
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function sets the LLVP-Profile value per
 * In-Port and per Vlan-Domain for usage by the LLR stage.
 * \param [in] unit - relevant unit.
 * \param [in] port - port ID.
 * \param [in] llvp_profile - LLVP profile ID.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  None.
 * \see
 * bcm_dnx_port_tpid_class_set
 * dnx_port_tpid_class_ingress_default_per_port_init
 */
static shr_error_e
dnx_port_tpid_class_ingress_llvp_profile_set(
    int unit,
    bcm_gport_t port,
    int llvp_profile)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 entry_handle_id;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_TUNNEL(port) || BCM_GPORT_IS_MPLS_PORT(port))
    {
        uint32 vlan_domain;
        /*
         * Get Vlan Domain from LIF
         */
        /*
         *   IN_LIF_FORMAT_PWE
         *   IN_LIF_IPvX_TUNNELS
         */
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

        /*
         * Get local LIFs using DNX Algo Gport Managment:
         */
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                     &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources.inlif_dbal_result_type);
        /*
         * getting the entry value
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, INST_SINGLE, &vlan_domain));

        /*
         * Update LLR per Vlan Domain table with LLVP profile
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_ingress_vlan_domain_llvp_profile_set(unit, vlan_domain, llvp_profile));

    }
    else
    {

        /*
         * Get Port + Core
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        /*
         * Update LLR per INGRESS PORT table with LLVP profile
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                       gport_info.internal_port_pp_info.pp_port[pp_port_index]);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                       gport_info.internal_port_pp_info.core_id[pp_port_index]);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_LLVP_PROFILE, INST_SINGLE,
                                         llvp_profile);

            /*
             * write to HW and release handle
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  This function sets egress LLVP profile.
 * \param [in] unit - relevant unit.
 * \param [in] port - port ID.
 * \param [in] llvp_profile - LLVP profile ID.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_port_tpid_class_set
 */
static shr_error_e
dnx_port_tpid_class_egress_llvp_profile_set(
    int unit,
    bcm_gport_t port,
    int llvp_profile)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 entry_handle_id;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_TUNNEL(port) || BCM_GPORT_IS_MPLS_PORT(port))
    {
        /*
         * EEDB_IPV4_TUNNEL
         * EEDB_IPV6_TUNNEL
         * EEDB_MPLS_TUNNEL
         */
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

        /*
         * Get local LIFs using DNX Algo Gport Managment:
         */
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources),
                                    _SHR_E_NOT_FOUND, _SHR_E_PARAM);

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources.local_out_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources.outlif_dbal_result_type);
        /*
         * write the llvp_profile to HW and release handle
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LLVP_PROFILE, INST_SINGLE, llvp_profile);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    }
    else
    {
        /*
         * Physical port:
         * Get Port + Core
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_PP_PORT, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                       gport_info.internal_port_pp_info.pp_port[pp_port_index]);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                       gport_info.internal_port_pp_info.core_id[pp_port_index]);
            /*
             * write the llvp_profile to HW and release handle
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LLVP_PROFILE, INST_SINGLE,
                                         llvp_profile);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function returns ingress LLVP profile. It is
 * used for tunnels/pwe LIF when the LLVP profile
 * is an attribute of the LIFs VLAN-Domain (next_layer_vlan_domain).
 * \param [in] unit - relevant unit.
 * \param [in] port - port ID.
 * \param [out] llvp_profile - pointer to LLVP profile ID.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * dnx_port_tpid_class_ingress_llvp_profile_get
 */
static shr_error_e
dnx_port_tpid_class_ingress_lif_llvp_profile_get(
    int unit,
    bcm_gport_t port,
    int *llvp_profile)
{
    uint32 entry_handle_id;
    uint32 next_layer_vlan_domain;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * 1. Get Vlan Domain from LIF
     *   IN_LIF_FORMAT_PWE
     *   IN_LIF_IPvX_TUNNELS
     */

    /*
     * Get local LIFs using DNX Algo Gport Managment:
     */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                 &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    /*
     * getting the entry value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, INST_SINGLE,
                     &next_layer_vlan_domain));

    /*
     * 2. Get llvp_profile from Vlan Domain
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_VLAN_DOMAIN_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, next_layer_vlan_domain);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INGRESS_LLVP_PROFILE, INST_SINGLE,
                               (uint32 *) llvp_profile);

    /*
     * getting the entry value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function returns ingress LLVP profile. It is
 * used when the LLVP profile is an attribute of the PP-Port.
 * \param [in] unit - relevant unit.
 * \param [in] pp_port - pp_port id.
 * \param [in] core_id - core id.
 * \param [out] llvp_profile - pointer to LLVP profile ID.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * dnx_port_tpid_class_ingress_llvp_profile_get
 */
static shr_error_e
dnx_port_tpid_class_ingress_port_llvp_profile_get(
    int unit,
    int pp_port,
    int core_id,
    int *llvp_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INGRESS_LLVP_PROFILE, INST_SINGLE,
                               (uint32 *) llvp_profile);

    /*
     * getting the entry value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function returns ingress LLVP profile. It reads
 * from the LLR table in order to get the profile value.
 * \param [in] unit - relevant unit.
 * \param [in] port - port ID.
 * \param [out] llvp_profile - pointer to LLVP profile ID.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_port_tpid_class_set
 * bcm_dnx_port_tpid_class_get
 * dnx_port_tpid_class_ingress_default_per_port_deinit
 */
static shr_error_e
dnx_port_tpid_class_ingress_llvp_profile_get(
    int unit,
    bcm_gport_t port,
    int *llvp_profile)
{
    uint8 is_physical_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_physical_port));

    if (is_physical_port == FALSE)
    {
        /*
         *   IN_LIF_FORMAT_PWE
         *   IN_LIF_IPvX_TUNNELS
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_ingress_lif_llvp_profile_get(unit, port, llvp_profile));
    }
    else
    {
        dnx_algo_gpm_gport_phy_info_t gport_info;

        /*
         * Get Port + Core
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        SHR_IF_ERR_EXIT(dnx_port_tpid_class_ingress_port_llvp_profile_get
                        (unit, gport_info.internal_port_pp_info.pp_port[0], gport_info.internal_port_pp_info.core_id[0],
                         llvp_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function returns egress LLVP profile. It is
 * used for tunnels/mpls LIF when the LLVP profile
 * is an attribute of the Out-LIF.
 * \param [in] unit - relevant unit.
 * \param [in] port - port ID.
 * \param [out] llvp_profile - pointer to LLVP profile ID.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * dnx_port_tpid_class_egress_llvp_profile_get
 */
static shr_error_e
dnx_port_tpid_class_egress_lif_llvp_profile_get(
    int unit,
    bcm_gport_t port,
    int *llvp_profile)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     *   EEDB_IPV4_TUNNEL
     *   EEDB_IPV6_TUNNEL
     * EEDB_MPLS_TUNNEL
     */

    /*
     * Get local LIFs using DNX Algo Gport Managment:
     */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources.local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.outlif_dbal_result_type);
    /*
     * getting the entry value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_EGRESS_LLVP_PROFILE,
                                                        INST_SINGLE, (uint32 *) llvp_profile));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function returns egress LLVP profile. It is
 * used when the LLVP profile is an attribute of the PP-Port.
 * \param [in] unit - relevant unit.
 * \param [in] pp_port - pp_port id.
 * \param [in] core_id - core id.
 * \param [out] llvp_profile - pointer to LLVP profile ID.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * dnx_port_tpid_class_egress_llvp_profile_get
 */
static shr_error_e
dnx_port_tpid_class_egress_port_llvp_profile_get(
    int unit,
    int pp_port,
    int core_id,
    int *llvp_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EGRESS_LLVP_PROFILE, INST_SINGLE,
                               (uint32 *) llvp_profile);
    /*
     * getting the entry value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function returns egress LLVP profile.
 * \param [in] unit - relevant unit.
 * \param [in] port - port ID.
 * \param [out] llvp_profile - pointer to LLVP profile ID.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_port_tpid_class_set
 * bcm_dnx_port_tpid_class_get
 * dnx_port_tpid_class_egress_default_per_port_deinit
 */
static shr_error_e
dnx_port_tpid_class_egress_llvp_profile_get(
    int unit,
    bcm_gport_t port,
    int *llvp_profile)
{
    uint8 is_physical_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_physical_port));

    if (is_physical_port == FALSE)
    {
        /*
         *   EEDB_IPV4_TUNNEL
         *   EEDB_IPV6_TUNNEL
         * EEDB_MPLS_TUNNEL
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_egress_lif_llvp_profile_get(unit, port, llvp_profile));
    }
    else
    {
        dnx_algo_gpm_gport_phy_info_t gport_info;

        /*
         * Get Port + Core
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        SHR_IF_ERR_EXIT(dnx_port_tpid_class_egress_port_llvp_profile_get
                        (unit, gport_info.internal_port_pp_info.pp_port[0], gport_info.internal_port_pp_info.core_id[0],
                         llvp_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get acceptable_frame_type_profile from egress port table, per pp_port and core.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] pp_port - pp_port id.
 * \param [in] core_id - core id.
 * \param [out] acceptable_frame_type_profile - acceptable frame type profile number to conjunct with Acceptable Frame Type Table.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 */
static shr_error_e
dnx_port_tpid_egress_port_acceptable_frame_type_profile_get(
    int unit,
    int pp_port,
    int core_id,
    uint32 *acceptable_frame_type_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Read EGRESS PORT table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ACCEPTABLE_FRAME_TYPE_PROFILE, INST_SINGLE,
                               acceptable_frame_type_profile);

    /*
     * getting the entry with the default values
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get acceptable_frame_type_profile from egress port table.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [out] acceptable_frame_type_profile - acceptable frame type profile number to conjunct with Acceptable Frame Type Table.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 */
static shr_error_e
dnx_port_tpid_egress_acceptable_frame_type_profile_get(
    int unit,
    bcm_port_t port,
    uint32 *acceptable_frame_type_profile)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(dnx_port_tpid_egress_port_acceptable_frame_type_profile_get
                    (unit, gport_info.internal_port_pp_info.pp_port[0], gport_info.internal_port_pp_info.core_id[0],
                     acceptable_frame_type_profile));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Set acceptable_frame_type_profile to egress port table.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [in] acceptable_frame_type_profile - acceptable frame type profile number to conjunct with Acceptable Frame Type Table.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 */
static shr_error_e
dnx_port_tpid_egress_acceptable_frame_type_profile_set(
    int unit,
    bcm_port_t port,
    uint32 acceptable_frame_type_profile)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /*
     * Write to EGRESS PORT table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        /** Set key fields */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_PP_PORT, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        /** Set acceptable_frame_type_profile */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCEPTABLE_FRAME_TYPE_PROFILE, INST_SINGLE,
                                     acceptable_frame_type_profile);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Utility function to update egress acceptable frame type template buffer
 * \param [in] unit - Device number
 * \param [in] tpid_class - Pointer to a TPID class structure that includes the port
 *          and the data to set.
 * \param [in] port_tpid_indexes - pointer to array of cells, containing outer and inner tpid indexs
 * \param [in] nof_port_tpid_indexes - pointer to array of cells, containing numer of outer and inner tpid indexs
 * \param [in,out] acceptable_frame_type_template - pointer to template holding whole acceptable frame type profile data (in acceptable frame type template format)
 *
 * \return
 *   None
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_port_tpid_class_set
 */
static shr_error_e
dnx_port_tpid_class_set_egress_acceptable_frame_type_buffer_update(
    int unit,
    bcm_port_tpid_class_t * tpid_class,
    int port_tpid_indexes[TPID_INDEX_NOF][BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS],
    int nof_port_tpid_indexes[TPID_INDEX_NOF],
    acceptable_frame_type_template_t * acceptable_frame_type_template)
{
    int idx1, idx2;
    int outer_tpid_index;
    int inner_tpid_index;

    SHR_FUNC_INIT_VARS(unit);

    for (idx1 = 0; idx1 < nof_port_tpid_indexes[TPID_INDEX_OUTER]; idx1++)
    {
        for (idx2 = 0; idx2 < nof_port_tpid_indexes[TPID_INDEX_INNER]; idx2++)
        {
            outer_tpid_index = (port_tpid_indexes[TPID_INDEX_OUTER][idx1]);
            inner_tpid_index = (port_tpid_indexes[TPID_INDEX_INNER][idx2]);
            /*
             * Update the acceptable frame type template buffer per tag_structure_index
             */
            if (tpid_class->flags & BCM_PORT_TPID_CLASS_DISCARD)
            {
                acceptable_frame_type_template->acceptable_frame_type_template[outer_tpid_index][inner_tpid_index] =
                    DBAL_ENUM_FVAL_EGRESS_ACCEPTABLE_FRAME_TYPE_FILTER_ACTION_DROP;
            }
            else
            {
                acceptable_frame_type_template->acceptable_frame_type_template[outer_tpid_index][inner_tpid_index] =
                    DBAL_ENUM_FVAL_EGRESS_ACCEPTABLE_FRAME_TYPE_FILTER_ACTION_ACCEPT;
            }
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Utility function to update whole egress acceptable frame type table according to egress acceptable frame type profile data
 * \param [in] unit - Device number
 * \param [in] acceptable_frame_type_profile - acceptable frame type profile value
 * \param [in] acceptable_frame_type_template - pointer to template holding whole acceptable frame type profile data (in acceptable frame type template format)
 *
 * \return
 *   None
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_port_tpid_class_set
 */
static shr_error_e
dnx_port_tpid_class_set_egress_acceptable_frame_type_block_write(
    int unit,
    int acceptable_frame_type_profile,
    acceptable_frame_type_template_t * acceptable_frame_type_template)
{
    int outer_tpid_index;
    int inner_tpid_index;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Go over all entries
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ACCEPTABLE_FRAME_TYPE_TABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCEPTABLE_FRAME_TYPE_PROFILE,
                               acceptable_frame_type_profile);

    for (outer_tpid_index = 0; outer_tpid_index < DNX_PORT_TPID_TPID_INDEX_SIZE; outer_tpid_index++)
    {
        for (inner_tpid_index = 0; inner_tpid_index < DNX_PORT_TPID_TPID_INDEX_SIZE; inner_tpid_index++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TPID_INDEX, outer_tpid_index);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_TPID_INDEX, inner_tpid_index);

            /*
             * Set data fields
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_ACCEPTABLE_FRAME_TYPE_FILTER_ACTION,
                                         INST_SINGLE,
                                         acceptable_frame_type_template->acceptable_frame_type_template
                                         [outer_tpid_index][inner_tpid_index]);
            /*
             * Write to HW
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Utility function to clear egress acceptable frame type table according to egress acceptable frame type profile data
 * \param [in] unit - Device number
 * \param [in] acceptable_frame_type_profile - acceptable frame type profile value
 *
 * \return
 *   None
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_port_tpid_class_set
 */
static shr_error_e
dnx_port_tpid_class_set_egress_acceptable_frame_type_block_clear(
    int unit,
    int acceptable_frame_type_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ACCEPTABLE_FRAME_TYPE_TABLE, &entry_handle_id));

    /*
     * Go over all entries with specified acceptable frame type profile
     */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_INNER_TPID_INDEX, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TPID_INDEX, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCEPTABLE_FRAME_TYPE_PROFILE,
                               acceptable_frame_type_profile);

    /*
     * Set the values
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_ACCEPTABLE_FRAME_TYPE_FILTER_ACTION,
                                 INST_SINGLE, DBAL_ENUM_FVAL_EGRESS_ACCEPTABLE_FRAME_TYPE_FILTER_ACTION_ACCEPT);

    /*
     * Write to HW
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Retrieve single entry from egress acceptable frame type table
 * \param [in] unit - Device number
 * \param [in] outer_tpid_index - outer tpid index
 * \param [in] inner_tpid_index - inner tpid index
 * \param [in] acceptable_frame_type_profile - acceptable frame type profile profile number
 * \param [out] acceptable_frame_type_filter_action - indicate discard or not
 *
 * \return
 *   None
 *
 * \remark
 *   * None
 * \see
 *   bcm_dnx_port_tpid_class_get
 */
static shr_error_e
dnx_port_tpid_class_egress_acceptable_frame_type_get(
    int unit,
    int outer_tpid_index,
    int inner_tpid_index,
    int acceptable_frame_type_profile,
    dbal_enum_value_field_egress_acceptable_frame_type_filter_action_e * acceptable_frame_type_filter_action)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_ACCEPTABLE_FRAME_TYPE_TABLE, &entry_handle_id));

    /*
     * Set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TPID_INDEX, outer_tpid_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_TPID_INDEX, inner_tpid_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCEPTABLE_FRAME_TYPE_PROFILE,
                               acceptable_frame_type_profile);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EGRESS_ACCEPTABLE_FRAME_TYPE_FILTER_ACTION,
                               INST_SINGLE, acceptable_frame_type_filter_action);

    /*
     * getting the entry value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (port_tpid.h).
 */
shr_error_e
dnx_port_tpid_class_default_init(
    int unit)
{
    uint8 is_init;
    int ingress_native_llvp_profile;
    uint32 entry_handle_id;
    bcm_port_tag_format_class_t tag_format_class_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(port_tpid_db.is_init(unit, &is_init));

    if (!is_init)
    {
        SHR_IF_ERR_EXIT(port_tpid_db.init(unit));
    }

    /*
     * port_tpid_db.native_ac_llvp_templates.ingress default value should be "all zero".
     * port_tpid_db.native_ac_llvp_templates.egress default value should be "all zero".
     * port_tpid_db.egress_ip_tunnel_llvp_template default value should be "all zero".
     * Since this is the default value of the SW state, nothing to do here.
     */

    /*
     * Update ingress llvp profile of ALL vlan_domain - points to ingress native AC llvp profile.
     */
    ingress_native_llvp_profile = (uint32) dnx_data_port_pp.general.default_ingress_native_ac_llvp_profile_id_get(unit);

    /*
     * Update LLR per Vlan Domain table with ingress native AC llvp profile
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_VLAN_DOMAIN_INFO, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_LLVP_PROFILE, INST_SINGLE,
                                 ingress_native_llvp_profile);
    /*
     * write to HW and release handle
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Initiate LLVP table acceptable fram action as drop by defaoult since drop id is not 0.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_LLVP_CLASSIFICATION, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_INNER_TPID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TPID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    if (!dnx_data_vlan.llvp.ext_vcp_en_get(unit))
    {
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IS_OUTER_PRIO, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
    }
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_LLVP_PROFILE, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCEPTABLE_FRAME_TYPE_ACTION, INST_SINGLE,
                                 DBAL_ENUM_FVAL_ACCEPTABLE_FRAME_TYPE_FILTER_ACTION_DROP);
    /*
     * Create Tag-Struct 0 for UNTAG
     */
    tag_format_class_id = 0;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_create
                    (unit, BCM_PORT_TPID_CLASS_WITH_ID, bcmTagStructTypeUntag, &tag_format_class_id));

    /*
     * Update TAG-STRUCT SW tables:
     *  - DBAL_TABLE_TAG_STRUCT_HW_ID_TO_SW_ID
     *  - DBAL_TABLE_TAG_STRUCT_SW_ID_TO_HW_ID
     * Since DBAL SW state table default is zero, nothing to do here
     */

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Init Trunk PP-Port  SW DB.
 */
static shr_error_e
dnx_port_tpid_class_ingress_default_trunk_init(
    int unit,
    bcm_gport_t logical_port,
    int *is_ingress_llvp_profile_valid)
{

    bcm_trunk_t trunk_id = BCM_GPORT_TRUNK_GET(logical_port);
    int core_id;
    bcm_trunk_pp_port_allocation_info_t allocation_info;
    uint32 core_bitmap;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_trunk_pp_port_allocation_get(unit, trunk_id, 0, &allocation_info));

    /*
     * See dnx_trunk_pp_ports_allocate - pp_handle was allocated per core+tid (can be only on one core, or more).
     */
    *is_ingress_llvp_profile_valid = -1;
    core_id = 0;
    core_bitmap = allocation_info.core_bitmap;
    do
    {
        if (core_bitmap & 0x1)
        {
            dnx_algo_port_db_2d_handle_t pp_handle;
            uint32 pp_port;

            /** take the core's pp_port */
            pp_port = allocation_info.pp_port_per_core_array[core_id];

            pp_handle.h0 = core_id;
            pp_handle.h1 = pp_port;

            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ingress_llvp_profile_valid.get
                            (unit, pp_handle.h0, pp_handle.h1, is_ingress_llvp_profile_valid));

            if (*is_ingress_llvp_profile_valid == 0)
            {
                /** Set the LLVP valid to true */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ingress_llvp_profile_valid.set
                                (unit, pp_handle.h0, pp_handle.h1, 1));
            }
        }

        /** handle next core */
        core_bitmap = core_bitmap >> 1;
        core_id += 1;

    }
    while (core_bitmap);

    /** For at least one core, the handle should have been found */
    SHR_IF_ERR_EXIT_WITH_LOG(*is_ingress_llvp_profile_valid == -1,
                             "Error! Trunk = 0x%08x - ingress pp_handle was not found! core_bitmap = 0x%08x, pp_port_per_core[0] = %d \n",
                             logical_port, allocation_info.core_bitmap, allocation_info.pp_port_per_core_array[0]);

    SHR_IF_ERR_EXIT_WITH_LOG(*is_ingress_llvp_profile_valid == -1,
                             "Error! Trunk = 0x%08x - ingress pp_handle was not found! core_bitmap = 0x%08x, pp_port_per_core[1] = %d \n",
                             logical_port, allocation_info.core_bitmap, allocation_info.pp_port_per_core_array[1]);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * De-init Trunk PP-Port  SW DB.
 */
shr_error_e
dnx_port_tpid_class_ingress_default_trunk_deinit(
    int unit,
    bcm_gport_t logical_port)
{
    bcm_trunk_t trunk_id = BCM_GPORT_TRUNK_GET(logical_port);
    int core_id;
    bcm_trunk_pp_port_allocation_info_t allocation_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_trunk_pp_port_allocation_get(unit, trunk_id, 0, &allocation_info));

    /*
     * See dnx_trunk_pp_ports_allocate - pp_handle was allocated per core+tid (can be only on one core, or more).
     */
    core_id = 0;
    do
    {
        if (allocation_info.core_bitmap & 0x1)
        {
            dnx_algo_port_db_2d_handle_t pp_handle;
            uint32 pp_port;

            /** take the core's pp_port */
            pp_port = allocation_info.pp_port_per_core_array[core_id];

            pp_handle.h0 = core_id;
            pp_handle.h1 = pp_port;
            /** Set the LLVP valid to invalid */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ingress_llvp_profile_valid.set(unit, pp_handle.h0, pp_handle.h1, 0));
        }

        /** handle next core */
        allocation_info.core_bitmap = allocation_info.core_bitmap >> 1;
        core_id += 1;

    }
    while (allocation_info.core_bitmap);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Init Trunk PP-Port  SW DB.
 */
static shr_error_e
dnx_port_tpid_class_egress_default_trunk_init(
    int unit,
    bcm_gport_t logical_port,
    int *is_egress_llvp_profile_valid)
{
    bcm_trunk_t trunk_id = BCM_GPORT_TRUNK_GET(logical_port);
    int core_id;
    bcm_trunk_pp_port_allocation_info_t allocation_info;
    uint32 core_bitmap;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_trunk_pp_port_allocation_get(unit, trunk_id, 0, &allocation_info));

    /*
     * See dnx_trunk_pp_ports_allocate - pp_handle was allocated per core+tid (can be only on one core, or more).
     */
    *is_egress_llvp_profile_valid = -1;
    core_id = 0;
    core_bitmap = allocation_info.core_bitmap;
    do
    {
        if (core_bitmap & 0x1)
        {
            dnx_algo_port_db_2d_handle_t pp_handle;
            uint32 pp_port;

            /** take the core's pp_port */
            pp_port = allocation_info.pp_port_per_core_array[core_id];

            pp_handle.h0 = core_id;
            pp_handle.h1 = pp_port;

            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.egress_llvp_profile_valid.get
                            (unit, pp_handle.h0, pp_handle.h1, is_egress_llvp_profile_valid));

            if (*is_egress_llvp_profile_valid == 0)
            {
                /** Set the LLVP valid to true */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.egress_llvp_profile_valid.set(unit, pp_handle.h0, pp_handle.h1, 1));
            }
        }

        /** handle next core */
        core_bitmap = core_bitmap >> 1;
        core_id += 1;

    }
    while (core_bitmap);

    /** For at least one core, the handle should have been found */
    SHR_IF_ERR_EXIT_WITH_LOG(*is_egress_llvp_profile_valid == -1,
                             "Error! Trunk = 0x%08x - egress pp_handle was not found! core_bitmap = 0x%08x, pp_port_per_core[0] = %d \n",
                             logical_port, allocation_info.core_bitmap, allocation_info.pp_port_per_core_array[0]);

    SHR_IF_ERR_EXIT_WITH_LOG(*is_egress_llvp_profile_valid == -1,
                             "Error! Trunk = 0x%08x - egress pp_handle was not found! core_bitmap = 0x%08x, pp_port_per_core[1] = %d \n",
                             logical_port, allocation_info.core_bitmap, allocation_info.pp_port_per_core_array[1]);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * De-init Trunk PP-Port  SW DB.
 */
shr_error_e
dnx_port_tpid_class_egress_default_trunk_deinit(
    int unit,
    bcm_gport_t logical_port)
{
    bcm_trunk_t trunk_id = BCM_GPORT_TRUNK_GET(logical_port);
    int core_id;
    bcm_trunk_pp_port_allocation_info_t allocation_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_trunk_pp_port_allocation_get(unit, trunk_id, 0, &allocation_info));

    /*
     * See dnx_trunk_pp_ports_allocate - pp_handle was allocated per core+tid (can be only on one core, or more).
     */
    core_id = 0;
    do
    {
        if (allocation_info.core_bitmap & 0x1)
        {
            dnx_algo_port_db_2d_handle_t pp_handle;
            uint32 pp_port;

            /** take the core's pp_port */
            pp_port = allocation_info.pp_port_per_core_array[core_id];

            pp_handle.h0 = core_id;
            pp_handle.h1 = pp_port;
            /** Set the LLVP valid to invalid */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.egress_llvp_profile_valid.set(unit, pp_handle.h0, pp_handle.h1, 0));
        }

        /** handle next core */
        allocation_info.core_bitmap = allocation_info.core_bitmap >> 1;
        core_id += 1;

    }
    while (allocation_info.core_bitmap);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (port_tpid.h).
 */
shr_error_e
dnx_port_tpid_class_ingress_default_per_port_init(
    int unit,
    bcm_gport_t logical_port)
{
    int llvp_profile, old_llvp_profile;
    uint8 first_reference, is_last;
    int is_ingress_llvp_profile_valid;

    uint32 default_template_data[DNX_PORT_TPID_CLASS_TEMPLATE_SIZE_NUM_OF_UINT32];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(default_template_data, 0, sizeof(default_template_data));

    if (!BCM_GPORT_IS_TRUNK(logical_port))
    {
        uint32 port;
        dnx_algo_port_db_2d_handle_t pp_handle;

        if (BCM_GPORT_IS_SYSTEM_PORT(logical_port))
        {
            port = BCM_GPORT_SYSTEM_PORT_ID_GET(logical_port);
        }
        else
        {
            port = logical_port;
        }
        /*
         * See dnx_algo_port_tm_add and dnx_algo_port_pp_port_allocate - pp_handle is per logical port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, port, &pp_handle));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ingress_llvp_profile_valid.get
                        (unit, pp_handle.h0, pp_handle.h1, &is_ingress_llvp_profile_valid));

        if (is_ingress_llvp_profile_valid == 0)
        {
            /** Set the LLVP valid to true */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ingress_llvp_profile_valid.set(unit, pp_handle.h0, pp_handle.h1, 1));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_ingress_default_trunk_init
                        (unit, logical_port, &is_ingress_llvp_profile_valid));
    }

    /*
     * 1. Allocate an ingress LLVP classification template.
     * 2. Write the llvp template to HW.
     * 3. Set INGREES port to this default profile.
     * Note:
     * On SDK Init, steps #2 and #3 can be skipped because:
     *      - the allocated LLVP profile ID is 0.
     *      - port default llvp ID is 0.
     *      - the LLVP HW table content is also "all zero"
     * In case of dynamically allocating of new Eth PP port, the above three assumption are not true,
     * thus we have to write the profile to HW and set port default to it!
     */
    first_reference = FALSE;
    is_last = FALSE;
    old_llvp_profile = -1;

    if (is_ingress_llvp_profile_valid == 0)
    {
        SHR_IF_ERR_EXIT(algo_port_pp_db.port_pp_tpid_profile_ingress.allocate_single
                        (unit, 0, default_template_data, NULL, &llvp_profile, &first_reference));

    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_ingress_llvp_profile_get(unit, logical_port, &old_llvp_profile));

        /*
         * Exchange ingress port profile template
         */
        SHR_IF_ERR_EXIT(algo_port_pp_db.port_pp_tpid_profile_ingress.exchange
                        (unit, 0, default_template_data, old_llvp_profile, NULL, &llvp_profile,
                         &first_reference, &is_last));
    }

    if (first_reference == TRUE)
    {
        /*
         * Write LLVP entries to HW
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_ingress_llvp_block_write(unit, llvp_profile, default_template_data));
    }

    /*
     * Set port LLVP profile
     */
    if (llvp_profile != old_llvp_profile)
    {
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_ingress_llvp_profile_set(unit, logical_port, llvp_profile));
    }

    /*
     * The port will be pointing to llvp_profile, old_llvp_profile can be cleared if needed
     */
    if ((is_last == TRUE) && (old_llvp_profile != llvp_profile))
    {
        /*
         * Clear the HW:
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_ingress_llvp_block_clear(unit, old_llvp_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (port_tpid.h).
 */
shr_error_e
dnx_port_tpid_class_ingress_default_per_port_deinit(
    int unit,
    bcm_gport_t logical_port)
{
    int llvp_profile;
    uint8 last_reference;
    dnx_algo_port_db_2d_handle_t pp_handle;

    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_GPORT_IS_TRUNK(logical_port))
    {
        uint32 port;

        if (BCM_GPORT_IS_SYSTEM_PORT(logical_port))
        {
            port = BCM_GPORT_SYSTEM_PORT_ID_GET(logical_port);
        }
        else
        {
            port = logical_port;
        }
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, port, &pp_handle));

        /** Set the LLVP valid to invalid */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ingress_llvp_profile_valid.set(unit, pp_handle.h0, pp_handle.h1, 0));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_ingress_default_trunk_deinit(unit, logical_port));
    }

    /*
     * 1. Free the allocated ingress LLVP classification template.
     * 2. Clear the HW if it was the last reference to this LLVP profile.
     */
    last_reference = FALSE;

    SHR_IF_ERR_EXIT(dnx_port_tpid_class_ingress_llvp_profile_get(unit, logical_port, &llvp_profile));

    SHR_IF_ERR_EXIT(algo_port_pp_db.port_pp_tpid_profile_ingress.free_single(unit, llvp_profile, &last_reference));

    if (last_reference == TRUE)
    {
        /*
         * Clear the HW:
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_ingress_llvp_block_clear(unit, llvp_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (port_tpid.h).
 */
shr_error_e
dnx_port_tpid_class_egress_default_per_port_init(
    int unit,
    bcm_gport_t logical_port)
{
    uint8 first_reference, is_last;

    int llvp_profile, old_llvp_profile;
    uint32 default_template_data[DNX_PORT_TPID_CLASS_TEMPLATE_SIZE_NUM_OF_UINT32];
    int is_egress_llvp_profile_valid;

    int acceptable_frame_type_profile, old_acceptable_frame_type_profile;
    int outer_tpid_index;
    int inner_tpid_index;
    acceptable_frame_type_template_t default_acceptable_frame_type_template;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(default_template_data, 0, sizeof(default_template_data));

    /*
     * Initialize default egress acceptable frame type template as all 1, which inidcates all packets are acceptable
     */
    for (outer_tpid_index = 0; outer_tpid_index < DNX_PORT_TPID_TPID_INDEX_SIZE; outer_tpid_index++)
    {
        for (inner_tpid_index = 0; inner_tpid_index < DNX_PORT_TPID_TPID_INDEX_SIZE; inner_tpid_index++)
        {
            default_acceptable_frame_type_template.acceptable_frame_type_template[outer_tpid_index][inner_tpid_index] =
                DBAL_ENUM_FVAL_EGRESS_ACCEPTABLE_FRAME_TYPE_FILTER_ACTION_ACCEPT;
        }
    }

    if (!BCM_GPORT_IS_TRUNK(logical_port))
    {
        uint32 port;
        dnx_algo_port_db_2d_handle_t pp_handle;

        if (BCM_GPORT_IS_SYSTEM_PORT(logical_port))
        {
            port = BCM_GPORT_SYSTEM_PORT_ID_GET(logical_port);
        }
        else
        {
            port = logical_port;
        }
        /*
         * See dnx_algo_port_tm_add and dnx_algo_port_pp_port_allocate - pp_handle is per logical port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, port, &pp_handle));

        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.egress_llvp_profile_valid.get
                        (unit, pp_handle.h0, pp_handle.h1, &is_egress_llvp_profile_valid));

        if (is_egress_llvp_profile_valid == 0)
        {
            /** Set the LLVP valid to true */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.egress_llvp_profile_valid.set(unit, pp_handle.h0, pp_handle.h1, 1));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_egress_default_trunk_init
                        (unit, logical_port, &is_egress_llvp_profile_valid));
    }

    /*
     * 1. Allocate an egress LLVP classification template.
     * 2. Write the llvp template to HW.
     * 3. Set EGREES port to this default profile.
     * Note:
     * On SDK Init, steps #2 and #3 can be skipped because:
     *      - the allocated LLVP profile ID is 0.
     *      - port default llvp ID is 0.
     *      - the LLVP HW table content is also "all zero"
     * In case of dynamically allocating of new Eth PP port, the above three assumption are not true,
     * thus we have to write the profile to HW and set port default to it!
     */
    first_reference = FALSE;
    is_last = FALSE;
    old_llvp_profile = -1;

    if (is_egress_llvp_profile_valid == 0)
    {
        SHR_IF_ERR_EXIT(algo_port_pp_db.port_pp_tpid_profile_egress.allocate_single
                        (unit, 0, default_template_data, NULL, &llvp_profile, &first_reference));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_egress_llvp_profile_get(unit, logical_port, &old_llvp_profile));

        /*
         * Exchange egress port profile template
         */
        SHR_IF_ERR_EXIT(algo_port_pp_db.port_pp_tpid_profile_egress.exchange
                        (unit, 0, default_template_data, old_llvp_profile, NULL, &llvp_profile,
                         &first_reference, &is_last));
    }

    if (first_reference == TRUE)
    {
        /*
         * Write LLVP entries to HW
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_egress_llvp_block_write(unit, llvp_profile, default_template_data));
    }

    /*
     * Set port LLVP profile
     */
    if (llvp_profile != old_llvp_profile)
    {
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_egress_llvp_profile_set(unit, logical_port, llvp_profile));
    }

    /*
     * The port is pointing to llvp_profile, old_llvp_profile can be cleared if needed
     */
    if ((is_last == TRUE) && (old_llvp_profile != llvp_profile))
    {
        /*
         * Clear the HW:
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_egress_llvp_block_clear(unit, old_llvp_profile));
    }

    /*
     * 4. Allocate an egress acceptable frame type template.
     * 5. update acceptable frame type table per default template data.
     * 6. Set this acceptable frame type profile to egress port.
     * 7. Set enablers of egress accepatable frame type profile to be TRUE
     */
    first_reference = FALSE;
    is_last = FALSE;
    old_acceptable_frame_type_profile = -1;

    if (is_egress_llvp_profile_valid == 0)
    {
        SHR_IF_ERR_EXIT(algo_port_pp_db.egress_acceptable_frame_type_profile.allocate_single
                        (unit, 0, &default_acceptable_frame_type_template, NULL,
                         &acceptable_frame_type_profile, &first_reference));
    }
    else
    {
        /*
         * Get current acceptable frame type profile
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_egress_acceptable_frame_type_profile_get
                        (unit, logical_port, (uint32 *) &old_acceptable_frame_type_profile));

        /*
         * Exchange egress acceptable frame type profile template
         */
        SHR_IF_ERR_EXIT(algo_port_pp_db.egress_acceptable_frame_type_profile.exchange
                        (unit, 0, &default_acceptable_frame_type_template,
                         old_acceptable_frame_type_profile, NULL, &acceptable_frame_type_profile,
                         &first_reference, &is_last));
    }

    if (first_reference == TRUE)
    {
        /*
         * Update acceptable frame type table
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_egress_acceptable_frame_type_block_write
                        (unit, acceptable_frame_type_profile, &default_acceptable_frame_type_template));
    }

    /*
     * Set port acceptable frame type profile to egress port
     */
    if (acceptable_frame_type_profile != old_acceptable_frame_type_profile)
    {
        SHR_IF_ERR_EXIT(dnx_port_tpid_egress_acceptable_frame_type_profile_set
                        (unit, logical_port, acceptable_frame_type_profile));
    }

    /*
     * When old acceptable frame type profile is not referenced, clear related entries in acceptable frame type table
     */
    if ((is_last == TRUE) && (acceptable_frame_type_profile != old_acceptable_frame_type_profile))
    {
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_egress_acceptable_frame_type_block_clear
                        (unit, old_acceptable_frame_type_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (port_tpid.h).
 */
shr_error_e
dnx_port_tpid_class_egress_default_per_port_deinit(
    int unit,
    bcm_gport_t logical_port)
{
    uint8 last_reference;
    int llvp_profile;
    int acceptable_frame_type_profile;
    dnx_algo_port_db_2d_handle_t pp_handle;

    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_GPORT_IS_TRUNK(logical_port))
    {
        uint32 port;

        if (BCM_GPORT_IS_SYSTEM_PORT(logical_port))
        {
            port = BCM_GPORT_SYSTEM_PORT_ID_GET(logical_port);
        }
        else
        {
            port = logical_port;
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, port, &pp_handle));
        /** Set the LLVP valid to invalid */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.egress_llvp_profile_valid.set(unit, pp_handle.h0, pp_handle.h1, 0));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_egress_default_trunk_deinit(unit, logical_port));
    }

    /*
     * 1. Free the allocated egress LLVP classification template.
     * 2. Clear the HW if it was the last reference to this LLVP profile.
     */
    last_reference = FALSE;

    SHR_IF_ERR_EXIT(dnx_port_tpid_class_egress_llvp_profile_get(unit, logical_port, &llvp_profile));

    SHR_IF_ERR_EXIT(algo_port_pp_db.port_pp_tpid_profile_egress.free_single(unit, llvp_profile, &last_reference));

    if (last_reference == TRUE)
    {
        /*
         * Clear the HW:
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_egress_llvp_block_clear(unit, llvp_profile));
    }

    /*
     * 3. Free allocated egress acceptable frame type template.
     */
    last_reference = FALSE;

    SHR_IF_ERR_EXIT(dnx_port_tpid_egress_acceptable_frame_type_profile_get
                    (unit, logical_port, (uint32 *) &acceptable_frame_type_profile));

    SHR_IF_ERR_EXIT(algo_port_pp_db.egress_acceptable_frame_type_profile.free_single
                    (unit, acceptable_frame_type_profile, &last_reference));

    if (last_reference == TRUE)
    {
        /*
         * Clear the HW:
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_egress_acceptable_frame_type_block_clear
                        (unit, acceptable_frame_type_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *
 * BCM-API: Get VLAN tag classification for a port. Given the
 * identified TPIDs on packet, gets the tag-classifier.
 * \param [in] unit - Device number
 * \param [in,out] tpid_class - Pointer to a TPID class structure
 *          that includes following input data:
 *          - flags
 *          - port
 *          - tpid1
 *          - tpid2
 * As output, tpid_class - Pointer to a TPID class structure
 *          that includes following output data:
 *          For INGRESS:
 *          - flags (for BCM_PORT_TPID_CLASS_DISCARD and
 *            BCM_PORT_TPID_CLASS_OUTER_C)
 *          - tag_format_class_id
 *          - vlan_translation_action_id
 *          - vlan_translation_qos_map_id
 *          For EGRESS:
 *          - flags (for BCM_PORT_TPID_CLASS_OUTER_C and
 *            BCM_PORT_TPID_CLASS_INNER_C)
 *          - tag_format_class_id
 *
 * \return
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX - Fails.
 *
 * \remark
 *   * None
 * \see
 *   bcm_dnx_port_tpid_class_set
 */
shr_error_e
bcm_dnx_port_tpid_class_get(
    int unit,
    bcm_port_tpid_class_t * tpid_class)
{
    int nof_port_tpid_indexes[TPID_INDEX_NOF];
    int port_tpid_indexes[TPID_INDEX_NOF][BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS];
    uint32 tpid_vals[TPID_INDEX_NOF];
    /*
     * The below variables, tag_priority_vals and nof_tag_priority_vals, are used for handling the tag priority.
     * Possible cases:
     *                 1. outer tag is priority tag.
     *                 2. outer tag is not priority tag.
     *                 3. don't care - create 1 and 2.
     * See dnx_port_tpid_class_set_tag_priority_get.
     */
    int tag_priority_vals[2];
    int nof_tag_priority_vals;
    uint8 is_ingress, is_egress;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Verify user input
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_tpid_class_get_verify(unit, tpid_class));

    is_ingress = _SHR_IS_FLAG_SET(tpid_class->flags, BCM_PORT_TPID_CLASS_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = _SHR_IS_FLAG_SET(tpid_class->flags, BCM_PORT_TPID_CLASS_INGRESS_ONLY) ? FALSE : TRUE;

    sal_memset(nof_port_tpid_indexes, 0x0, sizeof(nof_port_tpid_indexes));
    sal_memset(port_tpid_indexes, 0x0, sizeof(port_tpid_indexes));

    /*
     * Calculate outer tag is priority tag LLVP key value
     * possible cases:
     *                 1. outer tag is priority tag.
     *                 2. outer tag is not priority tag.
     *                 3. don't care - create 1 and 2.
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_tag_priority_get
                    (unit, tpid_class, tag_priority_vals, &nof_tag_priority_vals));

    /*
     * Clear flag options that are output
     */
    tpid_class->flags &= ~(BCM_PORT_TPID_CLASS_DISCARD | BCM_PORT_TPID_CLASS_OUTER_C | BCM_PORT_TPID_CLASS_INNER_C);

    /*
     * Read ingress LLVP table
     */
    if (is_ingress == TRUE)
    {
        uint8 is_ingress_native;
        int new_llvp_profile;
        dnx_port_tpid_llvp_parser_info_t llvp_parser_info;
        dnx_port_tpid_ingress_llvp_entry_t dnx_port_tpid_ingress_llvp_entry;

        is_ingress_native = _SHR_IS_FLAG_SET(tpid_class->flags, BCM_PORT_TPID_CLASS_NATIVE_IVE) ? TRUE : FALSE;

        /*
         * calculate TPID indexes for given two TPIDs
         */
        tpid_vals[TPID_INDEX_OUTER] = tpid_class->tpid1;
        tpid_vals[TPID_INDEX_INNER] = tpid_class->tpid2;
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_utility_tpid_indexes_get
                        (unit, TRUE, tpid_vals, port_tpid_indexes, nof_port_tpid_indexes));

        /*
         * Get port LLVP profile
         */
        if (is_ingress_native == TRUE)
        {
            new_llvp_profile = dnx_data_port_pp.general.default_ingress_native_ac_llvp_profile_id_get(unit);
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_port_tpid_class_ingress_llvp_profile_get(unit, tpid_class->port, &new_llvp_profile));
        }

        /*
         * Need to read from the HW one entry.
         * Based on the inputed tpid1, tpid2 and the flags for tag priority, we could get multiple keys (for example TPID_ANY).
         * bcm_dnx_port_tpid_class_set sets same parameters on multiple keys.
         * Thus, we can read any one of them, let's take key 0:
         */
        llvp_parser_info.outer_tpid = port_tpid_indexes[TPID_INDEX_OUTER][0];
        llvp_parser_info.inner_tpid = port_tpid_indexes[TPID_INDEX_INNER][0];
        llvp_parser_info.is_outer_prio = tag_priority_vals[0];

        SHR_IF_ERR_EXIT(dnx_port_tpid_class_get_ingress_llvp_entry_from_dbal
                        (unit, new_llvp_profile, &llvp_parser_info, &dnx_port_tpid_ingress_llvp_entry));

        /*
         * Update output parameters:
         */

        /*
         * Update flags: BCM_PORT_TPID_CLASS_DISCARD
         * See dnx_port_tpid_class_set_ingress_llvp_entry_get
         */
        if (dnx_port_tpid_ingress_llvp_entry.frame_type_action ==
            DBAL_ENUM_FVAL_ACCEPTABLE_FRAME_TYPE_FILTER_ACTION_DROP)
        {
            tpid_class->flags |= BCM_PORT_TPID_CLASS_DISCARD;
        }

        /*
         * Update flags: BCM_PORT_TPID_CLASS_OUTER_C
         * See dnx_port_tpid_class_set_ingress_llvp_entry_get
         */
        if ((dnx_port_tpid_ingress_llvp_entry.llvp_tag_format != 0)
            && (dnx_port_tpid_ingress_llvp_entry.incoming_s_tag_exist == 0))
        {
            tpid_class->flags |= BCM_PORT_TPID_CLASS_OUTER_C;
        }

        /*
         * Verify that the HW TAG-Struct was created:
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_hw_id_is_created_verify
                        (unit, TRUE, dnx_port_tpid_ingress_llvp_entry.llvp_tag_format));

        /*
         * Convert HW TAG-Struct to SW TAG-Struct
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_hw_id_get_info
                        (unit, TRUE, dnx_port_tpid_ingress_llvp_entry.llvp_tag_format,
                         &(tpid_class->tag_format_class_id)));

        tpid_class->vlan_translation_action_id = dnx_port_tpid_ingress_llvp_entry.ivec_index;
        tpid_class->vlan_translation_qos_map_id = dnx_port_tpid_ingress_llvp_entry.pcp_dei_profile;

    }
    else if (is_egress == TRUE)
    {

        uint8 is_egress_native, is_egress_ip_tunnel;
        uint8 is_physical_port = FALSE;
        int new_llvp_profile;
        dnx_port_tpid_llvp_parser_info_t llvp_parser_info;
        dnx_port_tpid_egress_llvp_entry_t dnx_port_tpid_egress_llvp_entry;

        dbal_enum_value_field_egress_acceptable_frame_type_filter_action_e acceptable_frame_type_filter_action =
            DBAL_ENUM_FVAL_EGRESS_ACCEPTABLE_FRAME_TYPE_FILTER_ACTION_ACCEPT;
        int acceptable_frame_type_profile = 0;

        is_egress_native = _SHR_IS_FLAG_SET(tpid_class->flags, BCM_PORT_TPID_CLASS_NATIVE_EVE) ? TRUE : FALSE;
        is_egress_ip_tunnel = _SHR_IS_FLAG_SET(tpid_class->flags, BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL) ? TRUE : FALSE;

        /*
         * calculate TPID indexes for given two TPIDs
         */
        tpid_vals[TPID_INDEX_OUTER] = tpid_class->tpid1;
        tpid_vals[TPID_INDEX_INNER] = tpid_class->tpid2;
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_utility_tpid_indexes_get
                        (unit, FALSE, tpid_vals, port_tpid_indexes, nof_port_tpid_indexes));

        /*
         * Get port LLVP profile
         */
        if (is_egress_native == TRUE)
        {
            new_llvp_profile = dnx_data_port_pp.general.default_egress_native_ac_llvp_profile_id_get(unit);
        }
        else if (is_egress_ip_tunnel == TRUE)
        {
            new_llvp_profile = dnx_data_port_pp.general.default_egress_ip_tunnel_llvp_profile_id_get(unit);
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_port_tpid_class_egress_llvp_profile_get(unit, tpid_class->port, &new_llvp_profile));
        }

        /*
         * Need to read from the HW one entry.
         * Based on the inputed tpid1, tpid2 and the flags for tag priority, we could get multiple keys (for example TPID_ANY).
         * bcm_dnx_port_tpid_class_set sets same parameters on multiple keys.
         * Thus, we can read any one of them, let's take key 0:
         */
        llvp_parser_info.outer_tpid = port_tpid_indexes[TPID_INDEX_OUTER][0];
        llvp_parser_info.inner_tpid = port_tpid_indexes[TPID_INDEX_INNER][0];
        llvp_parser_info.is_outer_prio = tag_priority_vals[0];

        SHR_IF_ERR_EXIT(dnx_port_tpid_class_get_egress_llvp_entry_from_dbal
                        (unit, new_llvp_profile, &llvp_parser_info, &dnx_port_tpid_egress_llvp_entry));

        /*
         * Skip Acceptable frame type filter for Tunnel Out-LIFs configuration
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, tpid_class->port, &is_physical_port));
        if (is_physical_port)
        {

            /*
             * Get egress acceptable frame type profile
             */
            SHR_IF_ERR_EXIT(dnx_port_tpid_egress_acceptable_frame_type_profile_get
                            (unit, tpid_class->port, (uint32 *) &acceptable_frame_type_profile));
            /*
             * Get entry from acceptable frame type table
             * 1 bit for each entry
             * 0 - discard, flag BCM_PORT_TPID_CLASS_DISCARD should be set
             * 1 - accept
             */
            SHR_IF_ERR_EXIT(dnx_port_tpid_class_egress_acceptable_frame_type_get
                            (unit, port_tpid_indexes[TPID_INDEX_OUTER][0], port_tpid_indexes[TPID_INDEX_INNER][0],
                             acceptable_frame_type_profile, &acceptable_frame_type_filter_action));
        }

        /*
         * Update output parameter:
         */

        /*
         * Update flags: BCM_PORT_TPID_CLASS_OUTER_C, BCM_PORT_TPID_CLASS_INNER_C
         * See dnx_port_tpid_class_set_egress_llvp_entry_get
         */
        if (dnx_port_tpid_egress_llvp_entry.packet_has_c_tag == TRUE)
        {
            if (dnx_port_tpid_egress_llvp_entry.c_tag_offset == DBAL_ENUM_FVAL_C_TAG_OFFSET_TYPE_OUTER_C_TAG)
            {
                tpid_class->flags |= BCM_PORT_TPID_CLASS_OUTER_C;
            }
            else if (dnx_port_tpid_egress_llvp_entry.c_tag_offset == DBAL_ENUM_FVAL_C_TAG_OFFSET_TYPE_INNER_C_TAG)
            {
                tpid_class->flags |= BCM_PORT_TPID_CLASS_INNER_C;
            }
        }
        /*
         * Update flags: BCM_PORT_TPID_CLASS_DISCARD
         */
        if (is_physical_port)
        {
            if (acceptable_frame_type_filter_action == DBAL_ENUM_FVAL_EGRESS_ACCEPTABLE_FRAME_TYPE_FILTER_ACTION_DROP)
            {
                tpid_class->flags |= BCM_PORT_TPID_CLASS_DISCARD;
            }
        }

        /*
         * Verify that the HW TAG-Struct was created:
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_hw_id_is_created_verify
                        (unit, FALSE, dnx_port_tpid_egress_llvp_entry.llvp_tag_format));

        /*
         * Convert HW TAG-Struct to SW TAG-Struct
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_hw_id_get_info
                        (unit, FALSE, dnx_port_tpid_egress_llvp_entry.llvp_tag_format,
                         &(tpid_class->tag_format_class_id)));

    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief -
 *
 * BCM-API: Set VLAN tag classification for a port.
 * Given the identified TPIDs on packet, sets the
 * tag-classifier.
 *
 * \param [in] unit - Device number
 * \param [in] tpid_class - Pointer to a TPID class structure that includes the port
 *          and the data to set. \n
 *    action: \n
 *    tpid_class.flags - BCM_PORT_TPID_CLASS_DISCARD - discard packet for given key (can be used for ingress & egress non native only)
 *
 * \return
 *   shr_error_e
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX - Fails.
 *
 * \remark
 *   The following tables are updated:
 *   - INGRESS_LLVP_CLASSIFICATION
 *   - EGRESS_LLVP_CLASSIFICATION
 *  flags support:
 *  1. Ingress only:
 *          - non native: flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY
 *          - native: flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY + BCM_PORT_TPID_CLASS_NATIVE_IVE
 *  2. Egress only:
 *          - non native: flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY
 *          - native: flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY + BCM_PORT_TPID_CLASS_NATIVE_EVE
 *          - IP Tunnel: flags BCM_PORT_TPID_CLASS_EGRESS_ONLY + BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL
 *  3. Symmetric (both ingress and egress are set):
 *      The API only support either symmetric of non-native or symmetric of native.
 *      Can't mixed setting of one native with one non-native:
 *          - non native: flags = 0
 *          - native: flags = BCM_PORT_TPID_CLASS_NATIVE_IVE + BCM_PORT_TPID_CLASS_NATIVE_EVE
 * \see
 *  PP BCM API User manual.
 */
shr_error_e
bcm_dnx_port_tpid_class_set(
    int unit,
    bcm_port_tpid_class_t * tpid_class)
{
    /*
     * key of the llvp table: tpid indexes, tag priority, llvp profile
     */
    int nof_port_tpid_indexes[TPID_INDEX_NOF];
    int port_tpid_indexes[TPID_INDEX_NOF][BCM_DNX_SWITCH_TPID_NUM_OF_GLOBALS];

    /*
     * The below variables, tag_priority_vals and nof_tag_priority_vals, are used for handling the tag priority.
     * See dnx_port_tpid_class_set_tag_priority_get.
     */
    int tag_priority_vals[2];
    int nof_tag_priority_vals;

    int old_llvp_profile;
    int new_llvp_profile;
    uint8 first_reference, is_last;
    /*
     * compressed ingress/egress llvp entry information
     */
    uint32 ingress_tag_buff;
    uint32 egress_tag_buff;

    /*
     * compressed ingress/egress llvp table per llvp profile. Used by template manager.
     */
    llvp_template_t llvp_template;
    uint32 *tpid_class_buffer;

    uint8 is_ingress, is_egress, is_ingress_native, is_egress_native, is_egress_ip_tunnel;

    /*
     * Below variables for acceptable frame type profile
     */
    int old_acceptable_frame_type_profile;
    int new_acceptable_frame_type_profile;
    acceptable_frame_type_template_t acceptable_frame_type_template;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Verify user input
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_tpid_class_set_verify(unit, tpid_class));

    tpid_class_buffer = &llvp_template.llvp_template[0];

    is_ingress = _SHR_IS_FLAG_SET(tpid_class->flags, BCM_PORT_TPID_CLASS_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = _SHR_IS_FLAG_SET(tpid_class->flags, BCM_PORT_TPID_CLASS_INGRESS_ONLY) ? FALSE : TRUE;
    is_ingress_native = _SHR_IS_FLAG_SET(tpid_class->flags, BCM_PORT_TPID_CLASS_NATIVE_IVE);
    is_egress_native = _SHR_IS_FLAG_SET(tpid_class->flags, BCM_PORT_TPID_CLASS_NATIVE_EVE);
    is_egress_ip_tunnel = _SHR_IS_FLAG_SET(tpid_class->flags, BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL);

    sal_memset(nof_port_tpid_indexes, 0x0, sizeof(nof_port_tpid_indexes));
    sal_memset(port_tpid_indexes, 0x0, sizeof(port_tpid_indexes));

    /*
     * Calculate outer tag is priority tag LLVP key value
     * possible cases:
     *                 1. outer tag is priority tag.
     *                 2. outer tag is not priority tag.
     *                 3. don't care - create 1 and 2.
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_tag_priority_get
                    (unit, tpid_class, tag_priority_vals, &nof_tag_priority_vals));

    /*
     * Update ingress non-native LLVP table
     */
    if ((is_ingress == TRUE) && (is_ingress_native == FALSE))
    {
        /*
         * Read SW template of this port
         */
        int ref_count;
        int ingress_native_ac_profile = dnx_data_port_pp.general.default_ingress_native_ac_llvp_profile_id_get(unit);

        SHR_IF_ERR_EXIT(dnx_port_tpid_class_ingress_llvp_profile_get(unit, tpid_class->port, &old_llvp_profile));

        if (old_llvp_profile == ingress_native_ac_profile)
        {
            /*
             * Get Ingress Native AC profile data
             */
            SHR_IF_ERR_EXIT(port_tpid_db.native_ac_llvp_templates.ingress.get(unit, &llvp_template));
        }
        else
        {
            /*
             * Get profile data from template manager:
             */
            SHR_IF_ERR_EXIT(algo_port_pp_db.port_pp_tpid_profile_ingress.profile_data_get
                            (unit, old_llvp_profile, &ref_count, tpid_class_buffer));
        }

        /*
         * Calcultate new llvp template -
         *   ingress_tag_buff - is one llvp entry (in llvp template format)
         *   tpid_class_buffer -is the whole llvp profile (in llvp template format)
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_tpid_class_block_update
                        (unit, TRUE, tpid_class, tag_priority_vals, nof_tag_priority_vals, port_tpid_indexes,
                         nof_port_tpid_indexes, &ingress_tag_buff, tpid_class_buffer));

        if (old_llvp_profile == ingress_native_ac_profile)
        {
            /*
             * Alloc ingress port profile template
             */
            SHR_IF_ERR_EXIT(algo_port_pp_db.port_pp_tpid_profile_ingress.allocate_single
                            (unit, 0, tpid_class_buffer, NULL, &new_llvp_profile, &first_reference));
            /*
             * the old profile is the ingress native AC, avoid clearing it
             */
            is_last = FALSE;
        }
        else
        {
            /*
             * Exchange ingress port profile template
             */
            SHR_IF_ERR_EXIT(algo_port_pp_db.port_pp_tpid_profile_ingress.exchange
                            (unit, 0, tpid_class_buffer, old_llvp_profile, NULL, &new_llvp_profile,
                             &first_reference, &is_last));
        }
        /*
         * Write the new LLVP entries to HW
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_llvp_block_write
                        (unit, TRUE, tag_priority_vals, nof_tag_priority_vals, port_tpid_indexes, nof_port_tpid_indexes,
                         old_llvp_profile, new_llvp_profile, first_reference, ingress_tag_buff, tpid_class_buffer));

        /*
         * Set port LLVP profile
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_ingress_llvp_profile_set(unit, tpid_class->port, new_llvp_profile));

        /*
         * Now, the port/LIF are pointing to new_llvp_profile, old_llvp_profile can be cleared if needed
         */
        if ((is_last == TRUE) && (old_llvp_profile != new_llvp_profile))
        {
            /*
             * Clear the HW:
             */
            SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_ingress_llvp_block_clear(unit, old_llvp_profile));
        }
    }

    /*
     * Update egress non-native LLVP table
     */
    if ((is_egress == TRUE) && (is_egress_native == FALSE) && (is_egress_ip_tunnel == FALSE))
    {
        /*
         * Read SW template of this port
         */
        int ref_count;
        int egr_native_ac_profile = dnx_data_port_pp.general.default_egress_native_ac_llvp_profile_id_get(unit);
        uint8 is_physical_port = FALSE;

        SHR_IF_ERR_EXIT(dnx_port_tpid_class_egress_llvp_profile_get(unit, tpid_class->port, &old_llvp_profile));

        if (old_llvp_profile == egr_native_ac_profile)
        {
            /*
             * Get Default Native AC profile data
             */
            SHR_IF_ERR_EXIT(port_tpid_db.native_ac_llvp_templates.egress.get(unit, &llvp_template));
        }
        else
        {
            /*
             * Get profile data from template manager:
             */
            SHR_IF_ERR_EXIT(algo_port_pp_db.port_pp_tpid_profile_egress.profile_data_get
                            (unit, old_llvp_profile, &ref_count, tpid_class_buffer));
        }

        /*
         * Calcultate new llvp template -
         *   egress_tag_buff - is one llvp entry (in llvp template format)
         *   tpid_class_buffer -is the whole llvp profile (in llvp template format)
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_tpid_class_block_update
                        (unit, FALSE, tpid_class, tag_priority_vals, nof_tag_priority_vals, port_tpid_indexes,
                         nof_port_tpid_indexes, &egress_tag_buff, tpid_class_buffer));

        if (old_llvp_profile == egr_native_ac_profile)
        {
            /*
             * Alloc egress port profile template
             */
            SHR_IF_ERR_EXIT(algo_port_pp_db.port_pp_tpid_profile_egress.allocate_single
                            (unit, 0, tpid_class_buffer, NULL, &new_llvp_profile, &first_reference));
            /*
             * the old profile is the egress native AC, avoid clearing it
             */
            is_last = FALSE;
        }
        else
        {
            /*
             * Exchange egress port profile template
             */
            SHR_IF_ERR_EXIT(algo_port_pp_db.port_pp_tpid_profile_egress.exchange
                            (unit, 0, tpid_class_buffer, old_llvp_profile, NULL, &new_llvp_profile,
                             &first_reference, &is_last));
        }
        /*
         * Write the new LLVP entries to HW
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_llvp_block_write
                        (unit, FALSE, tag_priority_vals, nof_tag_priority_vals, port_tpid_indexes,
                         nof_port_tpid_indexes, old_llvp_profile, new_llvp_profile, first_reference, egress_tag_buff,
                         tpid_class_buffer));

        /*
         * Set port/LIF LLVP profile
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_egress_llvp_profile_set(unit, tpid_class->port, new_llvp_profile));

        /*
         * Now, the port/LIF are pointing to new_llvp_profile, old_llvp_profile can be cleared if needed
         */

        if ((is_last == TRUE) && (old_llvp_profile != new_llvp_profile))
        {
            /*
             * Clear the HW:
             */
            SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_egress_llvp_block_clear(unit, old_llvp_profile));
        }

        /*
         * Skip Acceptable frame type filter for Tunnel Out-LIFs configuration
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, tpid_class->port, &is_physical_port));
        if (is_physical_port)
        {
            /*
             * Get current acceptable frame type profile
             */
            SHR_IF_ERR_EXIT(dnx_port_tpid_egress_acceptable_frame_type_profile_get
                            (unit, tpid_class->port, (uint32 *) &old_acceptable_frame_type_profile));

            /*
             * Get current profile data from template manager:
             */
            SHR_IF_ERR_EXIT(algo_port_pp_db.egress_acceptable_frame_type_profile.profile_data_get
                            (unit, old_acceptable_frame_type_profile, &ref_count, &acceptable_frame_type_template));

            /*
             * Calcultate new acceptable frame type template
             */
            SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_egress_acceptable_frame_type_buffer_update
                            (unit, tpid_class, port_tpid_indexes, nof_port_tpid_indexes,
                             &acceptable_frame_type_template));

            /*
             * Exchange egress acceptable frame type profile template
             */
            first_reference = FALSE;
            is_last = FALSE;
            SHR_IF_ERR_EXIT(algo_port_pp_db.egress_acceptable_frame_type_profile.exchange
                            (unit, 0, &acceptable_frame_type_template,
                             old_acceptable_frame_type_profile, NULL, &new_acceptable_frame_type_profile,
                             &first_reference, &is_last));
            /*
             * Update Egress Acceptable Frame Type table to HW
             */
            if (first_reference == TRUE)
            {
                /*
                 * Update acceptable frame type table
                 */
                SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_egress_acceptable_frame_type_block_write
                                (unit, new_acceptable_frame_type_profile, &acceptable_frame_type_template));
            }

            /*
             * Set new acceptable frame type profile to port
             */
            SHR_IF_ERR_EXIT(dnx_port_tpid_egress_acceptable_frame_type_profile_set
                            (unit, tpid_class->port, new_acceptable_frame_type_profile));

            /*
             * When old acceptable frame type profile is not referenced, clear related entries in acceptable frame type table
             */
            if ((is_last == TRUE))
            {
                SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_egress_acceptable_frame_type_block_clear
                                (unit, old_acceptable_frame_type_profile));
            }
        }
    }

    /*
     * Update Ingress Native AC
     */
    if (is_ingress_native == TRUE)
    {
        new_llvp_profile = dnx_data_port_pp.general.default_ingress_native_ac_llvp_profile_id_get(unit);

        /** tag priority is useless when ext_vcp_enable */
        if (dnx_data_vlan.llvp.ext_vcp_en_get(unit))
        {
            tag_priority_vals[0] = 0;
            nof_tag_priority_vals = 1;
        }

        /*
         * Get Ingress Native AC profile template data
         */
        SHR_IF_ERR_EXIT(port_tpid_db.native_ac_llvp_templates.ingress.get(unit, &llvp_template));

        /*
         * Calcultate new llvp template -
         *   ingress_tag_buff - is one llvp entry (in llvp template format)
         *   tpid_class_buffer -is the whole llvp profile (in llvp template format)
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_tpid_class_block_update
                        (unit, TRUE, tpid_class, tag_priority_vals, nof_tag_priority_vals, port_tpid_indexes,
                         nof_port_tpid_indexes, &ingress_tag_buff, tpid_class_buffer));

        /*
         * Write LLVP entries to HW
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_llvp_block_write
                        (unit, TRUE, tag_priority_vals, nof_tag_priority_vals, port_tpid_indexes,
                         nof_port_tpid_indexes, new_llvp_profile, new_llvp_profile, TRUE, ingress_tag_buff,
                         tpid_class_buffer));

        /*
         * Write back Ingress Native AC profile template data
         */
        SHR_IF_ERR_EXIT(port_tpid_db.native_ac_llvp_templates.ingress.set(unit, &llvp_template));

    }

    /*
     * Update Egress Native AC
     */
    if (is_egress_native == TRUE)
    {
        new_llvp_profile = dnx_data_port_pp.general.default_egress_native_ac_llvp_profile_id_get(unit);

        /*
         * Get Egress Native AC profile template data
         */
        SHR_IF_ERR_EXIT(port_tpid_db.native_ac_llvp_templates.egress.get(unit, &llvp_template));

        /*
         * Calcultate new llvp template -
         *   egress_tag_buff - is one llvp entry (in llvp template format)
         *   tpid_class_buffer -is the whole llvp profile (in llvp template format)
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_tpid_class_block_update
                        (unit, FALSE, tpid_class, tag_priority_vals, nof_tag_priority_vals, port_tpid_indexes,
                         nof_port_tpid_indexes, &egress_tag_buff, tpid_class_buffer));

        /*
         * Write LLVP entries to HW
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_llvp_block_write
                        (unit, FALSE, tag_priority_vals, nof_tag_priority_vals, port_tpid_indexes,
                         nof_port_tpid_indexes, new_llvp_profile, new_llvp_profile, TRUE, egress_tag_buff,
                         tpid_class_buffer));

        /*
         * Write back Egress Native AC profile template data
         */
        SHR_IF_ERR_EXIT(port_tpid_db.native_ac_llvp_templates.egress.set(unit, &llvp_template));

    }

    /*
     * Update Egress IP Tunnel
     */
    if (is_egress_ip_tunnel == TRUE)
    {
        new_llvp_profile = dnx_data_port_pp.general.default_egress_ip_tunnel_llvp_profile_id_get(unit);

        /*
         * Get Egress IP Tunnel profile template data
         */
        SHR_IF_ERR_EXIT(port_tpid_db.egress_ip_tunnel_llvp_template.get(unit, &llvp_template));

        /*
         * Calcultate new llvp template -
         *   egress_tag_buff - is one llvp entry (in llvp template format)
         *   tpid_class_buffer -is the whole llvp profile (in llvp template format)
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_tpid_class_block_update
                        (unit, FALSE, tpid_class, tag_priority_vals, nof_tag_priority_vals, port_tpid_indexes,
                         nof_port_tpid_indexes, &egress_tag_buff, tpid_class_buffer));

        /*
         * Write LLVP entries to HW
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_set_llvp_block_write
                        (unit, FALSE, tag_priority_vals, nof_tag_priority_vals, port_tpid_indexes,
                         nof_port_tpid_indexes, new_llvp_profile, new_llvp_profile, TRUE, egress_tag_buff,
                         tpid_class_buffer));

        /*
         * Write back Egress IP Tunnel profile template data
         */
        SHR_IF_ERR_EXIT(port_tpid_db.egress_ip_tunnel_llvp_template.set(unit, &llvp_template));

    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *
 * BCM-API: create a VLAN tag classification.
 * This function allocates a TAG-Struct based on the TAG-Struct type and maps this TAG-Struct
 * to proper HW TAG-Struct value.
 * This allocated TAG-Struct can later be used for BCM API bcm_port_tpid_class_set/get.
 *
 * \param [in] unit - Device number
 * \param [in] flags - BCM_PORT_TPID_CLASS_INGRESS_ONLY / BCM_PORT_TPID_CLASS_EGRESS_ONLY / BCM_PORT_TPID_CLASS_WITH_ID
 * \param [in] tag_struct_type - VLAN tag structure type.
 * \param [out] tag_format_class_id - pointer to the created tag-structure.
 *
 * \return
 *   shr_error_e
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX - Fails.
 *
 * \remark
 * The following data bases are updated by this function:
 *  - port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_ingress - resource allocation manager for ingress TAG-Struct.
 *  - port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_egress - resource allocation manager for egress TAG-Struct.
 *  - port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_symmetric - resource allocation manager for symmetric TAG-Struct.
 *  - port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_symmetric_indication - resource allocation bitmap for marking TAG-Struct as symmetric.
 *  - port_tpid_db.tag_struct_allocation.incoming_tag_structure_ingress - resource allocation manager for ingress HW TAG-Struct.
 *  - port_tpid_db.tag_struct_allocation.incoming_tag_structure_egress - resource allocation manager for egress HW TAG-Struct.
 *  - DBAL_TABLE_TAG_STRUCT_INGRESS_HW_ID_TO_SW_ID - sw state table - mapping HW-ID to SW-ID  -ingress tag_struct.
 *  - DBAL_TABLE_TAG_STRUCT_EGRESS_HW_ID_TO_SW_ID - sw state table - mapping HW-ID to SW-ID  -egress tag_struct
 *  - DBAL_TABLE_TAG_STRUCT_INGRESS_SW_ID_TO_HW_ID - sw state table - mapping SW-ID to HW-ID  -ingress tag_struct.
 *  - DBAL_TABLE_TAG_STRUCT_EGRESS_SW_ID_TO_HW_ID - sw state table - mapping SW-ID to HW-ID  -egress tag_struct
 * \see
 *  PP BCM API User manual.
 */
shr_error_e
bcm_dnx_port_tpid_class_create(
    int unit,
    uint32 flags,
    bcm_port_tag_struct_type_t tag_struct_type,
    bcm_port_tag_format_class_t * tag_format_class_id)
{
    int is_ingress_only, is_egress_only, is_symmetric, is_with_id_flag;
    int tag_struct_hw_id_ingress, tag_struct_hw_id_egress;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Verify user input
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_tpid_class_create_verify(unit, flags, tag_struct_type, tag_format_class_id));

    is_ingress_only = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_INGRESS_ONLY) ? TRUE : FALSE;
    is_egress_only = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_EGRESS_ONLY) ? TRUE : FALSE;
    is_symmetric = (is_ingress_only == FALSE) && (is_egress_only == FALSE);
    is_with_id_flag = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_WITH_ID) ? TRUE : FALSE;

    /*
     * Allocate HW tag-structure value:
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_hw_id_alloc
                    (unit, tag_struct_type, is_symmetric, is_ingress_only, &tag_struct_hw_id_ingress,
                     &tag_struct_hw_id_egress));

    /*
     * Allocate SW tag-structure value:
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_sw_id_alloc
                    (unit, is_symmetric, is_ingress_only, is_with_id_flag, tag_format_class_id));

    /*
     * Book the HW-ID:
     * Update "HW to SW" table with the SW-ID
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_hw_id_book
                    (unit, is_symmetric, is_ingress_only, tag_struct_hw_id_ingress, tag_struct_hw_id_egress,
                     *tag_format_class_id));

    /*
     * Book the SW-ID:
     * Update "SW to HW" table with the HW-ID
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_sw_id_book
                    (unit, is_symmetric, is_ingress_only, *tag_format_class_id, tag_struct_hw_id_ingress,
                     tag_struct_hw_id_egress));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *
 * BCM-API: free an allocated VLAN tag classification.
 * This function frees the TAG-Struct allocation and clears its mapping to the HW TAG-Struct value.
 *
 * \param [in] unit - Device number
 * \param [in] flags - BCM_PORT_TPID_CLASS_INGRESS_ONLY / BCM_PORT_TPID_CLASS_EGRESS_ONLY / BCM_PORT_TPID_CLASS_WITH_ID
 * \param [out] tag_format_class_id - the tag-structure to be freed.
 *
 * \return
 *   shr_error_e
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX - Fails.
 *
 * \remark
 * The following data bases are updated by this function:
 *  - port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_ingress - resource allocation manager for ingress TAG-Struct.
 *  - port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_egress - resource allocation manager for egress TAG-Struct.
 *  - port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_symmetric - resource allocation manager for symmetric TAG-Struct.
 *  - port_tpid_db.tag_struct_allocation.bcm_tag_format_class_id_symmetric_indication - resource allocation bitmap for marking TAG-Struct as symmetric.
 *  - port_tpid_db.tag_struct_allocation.incoming_tag_structure_ingress - resource allocation manager for ingress HW TAG-Struct.
 *  - port_tpid_db.tag_struct_allocation.incoming_tag_structure_egress - resource allocation manager for egress HW TAG-Struct.
 *  - DBAL_TABLE_TAG_STRUCT_INGRESS_HW_ID_TO_SW_ID - sw state table - mapping HW-ID to SW-ID  -ingress tag_struct.
 *  - DBAL_TABLE_TAG_STRUCT_EGRESS_HW_ID_TO_SW_ID - sw state table - mapping HW-ID to SW-ID  -egress tag_struct
 *  - DBAL_TABLE_TAG_STRUCT_INGRESS_SW_ID_TO_HW_ID - sw state table - mapping SW-ID to HW-ID  -ingress tag_struct.
 *  - DBAL_TABLE_TAG_STRUCT_EGRESS_SW_ID_TO_HW_ID - sw state table - mapping SW-ID to HW-ID  -egress tag_struct
 *
 * \see
 *  PP BCM API User manual.
 */
shr_error_e
bcm_dnx_port_tpid_class_destroy(
    int unit,
    uint32 flags,
    bcm_port_tag_format_class_t tag_format_class_id)
{
    int is_ingress_only, is_egress_only, is_symmetric;
    int tag_struct_hw_id_ingress, tag_struct_hw_id_egress;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Verify user input
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_tpid_class_destroy_verify(unit, flags, tag_format_class_id));

    is_ingress_only = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_INGRESS_ONLY) ? TRUE : FALSE;
    is_egress_only = _SHR_IS_FLAG_SET(flags, BCM_PORT_TPID_CLASS_EGRESS_ONLY) ? TRUE : FALSE;
    is_symmetric = (is_ingress_only == FALSE) && (is_egress_only == FALSE);

    /*
     * Get HW tag-structure value:
     */
    if (is_symmetric == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_sw_id_get_info
                        (unit, tag_format_class_id, &tag_struct_hw_id_ingress, &tag_struct_hw_id_egress));
    }
    else if (is_ingress_only == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_sw_id_get_info(unit, tag_format_class_id, &tag_struct_hw_id_ingress, NULL));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_sw_id_get_info(unit, tag_format_class_id, NULL, &tag_struct_hw_id_egress));
    }

    /*
     * Free HW tag-structure value:
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_hw_id_free
                    (unit, is_symmetric, is_ingress_only, tag_struct_hw_id_ingress, tag_struct_hw_id_egress));

    /*
     * Free SW tag-structure value:
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_sw_id_free(unit, is_symmetric, is_ingress_only, tag_format_class_id));

exit:
    SHR_FUNC_EXIT;
}
