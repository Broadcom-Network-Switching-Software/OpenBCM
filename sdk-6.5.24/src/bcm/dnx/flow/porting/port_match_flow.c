/** \file port_match_flow.c
 *  General port match functionality using flow lif for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnx/port/port_match.h>
#include "port_match_flow.h"
#include "vlan_port_flow.h"

/*
 * Add/Delete match to an existing port
 */
static shr_error_e
dnx_port_match_action_flow(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info,
    dnx_flow_match_action_e matchaction)
{

    bcm_flow_handle_info_t flow_handle_info = { 0 };

    bcm_vlan_port_t vlan_port;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Convert the match struct in order to use the VLAN-Port set function
     */
    SHR_IF_ERR_EXIT(dnx_port_match_ingress_match_to_vlan_port_match_convert(unit, port, match_info, &vlan_port));

    BCM_GPORT_TUNNEL_ID_SET(flow_handle_info.flow_id, BCM_GPORT_SUB_TYPE_LIF_VAL_GET(port));

    if (_SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_REPLACE))
    {
        flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;
    }

    SHR_IF_ERR_EXIT(dnx_vlan_port_terminator_flow_match_action(unit, &flow_handle_info, &vlan_port, matchaction));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_port_match_create_flow(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_match_action_flow(unit, port, match_info, FLOW_MATCH_ACTION_ADD));

    /*
     * Set/update the match info in sw table
     */
    if (_SHR_IS_FLAG_SET(match_info->flags, BCM_PORT_MATCH_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_port_match_ingress_match_info_sw_update(unit, port, match_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_match_ingress_match_info_sw_add(unit, port, match_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_port_match_clear_flow(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_match_action_flow(unit, port, match_info, FLOW_MATCH_ACTION_DELETE));

    SHR_IF_ERR_EXIT(dnx_port_match_ingress_match_info_sw_delete(unit, port, match_info));

exit:
    SHR_FUNC_EXIT;
}
