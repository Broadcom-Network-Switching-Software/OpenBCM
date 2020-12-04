/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file diag_dnx_pp_cc.c
 *
 * PP Cross Connect traverse Diagnostics.
 */

/*************
 * INCLUDES  *
 */
#include "diag_dnx_pp.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_debug.h>
#include <bcm_int/dnx/instru/instru_visibility.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/l3/l3_fec.h>
#include <soc/dnx/dbal/dbal_string_apis.h>
#include <bcm/extender.h>
#include <bcm/vswitch.h>

/*************
 * DEFINES   *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGPPDNX

/*************
 *  MACROS  *
 */

/*************
 * TYPEDEFS  *
 */

/*************
 * GLOBALS   *
 */

/*************
 * FUNCTIONS *
 */
extern shr_error_e dnx_vswitch_cross_connect_get_apply(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_algo_gpm_forward_info_t * forward_info);

/**
 * \brief The function prints gport and encap for gport in cross connect
 */
int
sh_dnx_pp_cross_connect_gport_encap_print_cb(
    int unit,
    bcm_gport_t gport,
    int encap)
{
    dnx_algo_gpm_forward_info_t forward_info;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 flags = 0;
    dbal_fields_e dbal_dest_type = 0;
    uint32 dbal_dest_val = 0;
    uint32 dest_val = 0;
    char *gport_type_str = NULL;
    char *encoding_str = NULL;
    bcm_vlan_port_t vlan_port;
    bcm_mpls_port_t mpls_port;
    bcm_extender_port_t extender_port;
    bcm_failover_t failover_id = 0;
    int failover_type = 0;
    bcm_l3_egress_t egr_entry;
    char *dest_type_str = NULL;
    int rv = _SHR_E_NONE;
    bcm_switch_fec_property_config_t fec_config;
    bcm_l3_egress_ecmp_t ecmp_get;
    int max_group_size = dnx_data_l3.ecmp.max_group_size_get(unit);
    int intf_count = 0;
    bcm_if_t *intf_array = NULL;
    int ecmp_intf_idx = 0;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&forward_info, 0, sizeof(dnx_algo_gpm_forward_info_t));
    sal_memset(&fec_config, 0, sizeof(bcm_switch_fec_property_config_t));
    bcm_l3_egress_ecmp_t_init(&ecmp_get);
    intf_array = sal_alloc(sizeof(bcm_if_t) * max_group_size, "ecmp_intf array");
    if (intf_array == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "allocation_failed");
    }

    if (BCM_GPORT_IS_VLAN_PORT(gport) || BCM_GPORT_IS_MPLS_PORT(gport) || BCM_GPORT_IS_EXTENDER_PORT(gport))
    {
        /*
         * GPORT: Get local In-LIF using DNX Algo GPORT Managment:
         */
        flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF | DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, flags, &gport_hw_resources));
        rv = dnx_vswitch_cross_connect_get_apply(unit, &gport_hw_resources, &forward_info);
        /*
         * gport hasn't forward info when only create 1 direction cross connect 
         */
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NONE)
        {
            SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                            (unit, DBAL_FIELD_DESTINATION, forward_info.destination, &dbal_dest_type, &dest_val));
        }

        if (BCM_GPORT_IS_VLAN_PORT(gport))
        {
            gport_type_str = "VLAN-PORT";
            bcm_vlan_port_t_init(&vlan_port);
            vlan_port.vlan_port_id = gport;
            SHR_IF_ERR_EXIT(bcm_vlan_port_find(unit, &vlan_port));
            failover_id = vlan_port.ingress_failover_id;
        }
        else if (BCM_GPORT_IS_MPLS_PORT(gport))
        {
            gport_type_str = "MPLS-PORT";
            bcm_mpls_port_t_init(&mpls_port);
            mpls_port.mpls_port_id = gport;
            SHR_IF_ERR_EXIT(bcm_mpls_port_get(unit, 0, &mpls_port));
            failover_id = mpls_port.ingress_failover_id;
        }
        else if (BCM_GPORT_IS_EXTENDER_PORT(gport))
        {
            gport_type_str = "EXTENDER-PORT";
            bcm_extender_port_t_init(&extender_port);
            extender_port.extender_port_id = gport;
            SHR_IF_ERR_EXIT(bcm_extender_port_get(unit, &extender_port));
            failover_id = extender_port.ingress_failover_id;
        }
        else
        {
            gport_type_str = "UNKNOWN-PORT";
        }

        /*
         * May FEC protection, get failover_id from fec entry 
         */
        if ((dbal_dest_type == DBAL_FIELD_FEC) && (failover_id == 0))
        {
            fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
            SHR_IF_ERR_EXIT(bcm_switch_fec_property_get(unit, &fec_config));
            if (dest_val >= fec_config.start)
            {
                SHR_IF_ERR_EXIT(dnx_l3_egress_fec_info_get(unit, dest_val, &egr_entry));
                failover_id = egr_entry.failover_id;
            }
        }

        BCM_FAILOVER_TYPE_GET(failover_type, failover_id);
        switch (failover_type)
        {
            case BCM_FAILOVER_TYPE_INGRESS:
                encoding_str = "1+1 protection";
                break;
            case BCM_FAILOVER_TYPE_NONE:
                encoding_str = "No-protection";
                break;
            case BCM_FAILOVER_TYPE_FEC:
                encoding_str = "1:1 protection";
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "unexpected failover type %d.\n", failover_type);
                break;
        }

        LOG_CLI((BSL_META("   ")));
        LOG_CLI((BSL_META("%s 0x%x Encoding: %s "), gport_type_str, gport, encoding_str));
        /*
         * print lif if exist 
         */
        if (gport_hw_resources.global_in_lif != BCM_GPORT_INVALID)
        {
            LOG_CLI((BSL_META(",Global-LIF-ID: %d"), gport_hw_resources.global_in_lif));
        }
        LOG_CLI((BSL_META("\n")));
    }
    else if (BCM_GPORT_IS_TUNNEL(gport))
    {
        gport_type_str = "TUNNEL_ID";
        LOG_CLI((BSL_META("   ")));
        LOG_CLI((BSL_META("%s:0x%-10x "), gport_type_str, gport));
        LOG_CLI((BSL_META("\n")));
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, gport, &dest_val));
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_DESTINATION, dest_val, &dbal_dest_type, &dbal_dest_val));

        switch (dbal_dest_type)
        {
            case DBAL_FIELD_FLOW_ID:
                dest_type_str = "uc_flow";
                break;
            case DBAL_FIELD_LAG_ID:
                dest_type_str = "uc_lag";
                break;
            case DBAL_FIELD_PORT_ID:
                dest_type_str = "uc_port";
                break;
            case DBAL_FIELD_MC_ID:
                dest_type_str = "mc";
                break;
            case DBAL_FIELD_FEC:
                fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
                SHR_IF_ERR_EXIT(bcm_switch_fec_property_get(unit, &fec_config));
                if (dest_val >= fec_config.start)
                {
                    dest_type_str = "fec";
                }
                else
                {
                    dest_type_str = "ecmp";
                }
                break;
            case DBAL_FIELD_INGRESS_TRAP_DEST:
                dest_type_str = "trap";
                break;
            default:
                dest_type_str = "Unknown";
        }

        if (BCM_GPORT_IS_BLACK_HOLE(gport))
        {
            dest_type_str = "drop";
        }
        LOG_CLI((BSL_META("   phy gport: <%s,%d>\n"), dest_type_str, dbal_dest_val));
        if (0 == strcmp(dest_type_str, "ecmp"))
        {
            ecmp_get.ecmp_intf = dest_val;
            SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get(unit, &ecmp_get, max_group_size, intf_array, &intf_count));
            for (ecmp_intf_idx = 0; ecmp_intf_idx < intf_count; ecmp_intf_idx++)
            {
                LOG_CLI((BSL_META("      member: 0x%-10x\n"), intf_array[ecmp_intf_idx]));
            }
        }

        if (encap != 0)
        {
            LOG_CLI((BSL_META("   encap:     0x%-10x\n"), encap));
        }
    }

exit:
    SHR_FREE(intf_array);
    SHR_FUNC_EXIT;
}

/**
 * \brief The function prints gport and encap for both gports in cross connect
 */
int
sh_dnx_pp_cross_connect_print_cb(
    int unit,
    bcm_vswitch_cross_connect_t * gports,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * gport1 
     */
    LOG_CLI((BSL_META("gport1\n")));
    SHR_IF_ERR_EXIT(sh_dnx_pp_cross_connect_gport_encap_print_cb(unit, gports->port1, gports->encap1));
    /*
     * gport2: cross-connected gport information
     */
    LOG_CLI((BSL_META("cross-connected to gport2\n")));
    SHR_IF_ERR_EXIT(sh_dnx_pp_cross_connect_gport_encap_print_cb(unit, gports->port2, gports->encap2));
    LOG_CLI((BSL_META("\n")));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Diagnostic to see the packet parsing record
 *        that describes the header stack of network headers
 *        and their associated header qualifiers
 *        - Support SHORT mode which prints all but layer_qualifiers
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] args - according to diag mechanism definition
 *   \param [in] sand_control - according to diag mechanism
 *          definition
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
sh_dnx_pp_cc_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * call function 
     */
    SHR_IF_ERR_EXIT(bcm_vswitch_cross_connect_traverse(unit, sh_dnx_pp_cross_connect_print_cb, NULL));

exit:
    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_pp_cc_man = {
    .brief = "Show cross connection traverse information",
    .full = "Show cross connection traverse information",
    .examples = "\n" "SHORT"
};

sh_sand_option_t dnx_pp_cc_options[] = {
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"}
    ,
    {NULL}
};
