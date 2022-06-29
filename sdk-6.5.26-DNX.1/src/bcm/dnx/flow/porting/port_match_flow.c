/** \file port_match_flow.c
 *  General port match functionality using flow lif for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/dnx/auto_generated/dnx_flow_dispatch.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnx/port/port_match.h>
#include <bcm_int/dnx/port/port_esem.h>
#include "port_match_flow.h"
#include "vlan_port_flow.h"

/*
 * Add/Delete ingress match to an existing port
 */
static shr_error_e
dnx_port_match_ingress_action_flow(
    int unit,
    bcm_gport_t flow_gport,
    bcm_port_match_info_t * match_info,
    dnx_flow_match_action_e matchaction)
{

    bcm_flow_handle_info_t flow_handle_info = { 0 };

    bcm_vlan_port_t vlan_port;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Convert the match struct in order to use the VLAN-Port set function
     */
    SHR_IF_ERR_EXIT(dnx_port_match_ingress_match_to_vlan_port_match_convert(unit, flow_gport, match_info, &vlan_port));

    flow_handle_info.flow_id = flow_gport;

    if (_SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_REPLACE))
    {
        flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;
    }

    if (_SHR_IS_FLAG_SET(match_info->flags, BCM_VLAN_PORT_NATIVE))
    {
        flow_handle_info.flags |= BCM_FLOW_HANDLE_NATIVE;
    }

    SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_match_action(unit, &flow_handle_info, &vlan_port, matchaction));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_port_match_ingress_create_flow(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    bcm_gport_t flow_gport;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, port, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &flow_gport));

    SHR_IF_ERR_EXIT(dnx_port_match_ingress_action_flow(unit, flow_gport, match_info, FLOW_MATCH_ACTION_ADD));

    /*
     * Set/update the match info in sw table
     */
    if (_SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_port_match_ingress_match_info_sw_update(unit, flow_gport, match_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_match_ingress_match_info_sw_add(unit, flow_gport, match_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_port_match_ingress_clear_flow(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{

    bcm_gport_t flow_gport;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, port, BCM_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &flow_gport));

    SHR_IF_ERR_EXIT(dnx_port_match_ingress_action_flow(unit, flow_gport, match_info, FLOW_MATCH_ACTION_DELETE));

    SHR_IF_ERR_EXIT(dnx_port_match_ingress_match_info_sw_delete(unit, flow_gport, match_info));

exit:
    SHR_FUNC_EXIT;
}
