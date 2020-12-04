/** \file mpls_l2vpn.c MPLS L2-VPN functionality for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLS

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/mpls_l2vpn/mpls_l2vpn.h>

#include "mpls_l2vpn_egress.h"
#include "mpls_l2vpn_ingress.h"
/*
 * }
 */
/*
 * Verification functions.
 *
 * These are not meant to re-verify all the information. This should be done
 * in the BCM modules. These functions are meant to check consistency and
 * integrity directly related to the MPLS-L2VPN layer, like the L2VPN type
 * is defined and that the information is not null.
 */

/**
 * \brief Verify the general structure and ranges of an mpls_l2vpn_info object
 */
static shr_error_e
dnx_mpls_l2vpn_info_verify(
    int unit,
    dnx_mpls_l2vpn_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    SHR_NULL_CHECK(info->mpls_port, _SHR_E_PARAM, "mpls_port");
    SHR_RANGE_VERIFY(info->type, MPLS_L2VPN_TYPE_NOT_DEFINED + 1, MPLS_L2VPN_TYPE_NOF - 1,
                     _SHR_E_PARAM, "L2 VPN unsupported type: %d", info->type);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Verify the structure of the given info struct for creation/replacement of an MPLS L2VPN object
 */
static shr_error_e
dnx_mpls_l2vpn_add_verify(
    int unit,
    dnx_mpls_l2vpn_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_info_verify(unit, info));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Verify the structure of the given info struct for retrieval of an MPLS L2VPN object
 */
static shr_error_e
dnx_mpls_l2vpn_get_verify(
    int unit,
    dnx_mpls_l2vpn_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_info_verify(unit, info));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief Verify the arguments given to delete an MPLS L2VPN object
 */
static shr_error_e
dnx_mpls_l2vpn_delete_verify(
    int unit,
    dnx_mpls_l2vpn_type_e l2vpn_type,
    bcm_gport_t mpls_l2vpn_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_RANGE_VERIFY(l2vpn_type, MPLS_L2VPN_TYPE_NOT_DEFINED + 1, MPLS_L2VPN_TYPE_NOF - 1,
                     _SHR_E_PARAM, "L2 VPN unsupported type: %d", l2vpn_type);
exit:
    SHR_FUNC_EXIT;
}

/*
 * Internal APIs
 */

void
dnx_mpls_l2vpn_info_t_init(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn_info,
    dnx_mpls_l2vpn_type_e type,
    bcm_mpls_port_t * mpls_port)
{
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(l2vpn_info, 0, sizeof(dnx_mpls_l2vpn_info_t));
    l2vpn_info->type = type;
    l2vpn_info->mpls_port = mpls_port;
    SHR_EXIT();
exit:
    SHR_VOID_FUNC_EXIT;
}

shr_error_e
dnx_mpls_l2vpn_add(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_l2vpn_add_verify(unit, l2vpn_info));

    if (_SHR_IS_FLAG_SET(l2vpn_info->mpls_port->flags2, BCM_MPLS_PORT2_INGRESS_ONLY))
    {
        SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_add_ingress_only(unit, l2vpn_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_add_egress_only(unit, l2vpn_info));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_mpls_l2vpn_delete(
    int unit,
    dnx_mpls_l2vpn_type_e l2vpn_type,
    int is_ingress,
    bcm_gport_t mpls_l2vpn_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_l2vpn_delete_verify(unit, l2vpn_type, mpls_l2vpn_id));

    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_delete_ingress_only(unit, mpls_l2vpn_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_delete_egress_only(unit, mpls_l2vpn_id));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_mpls_l2vpn_get(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_l2vpn_get_verify(unit, l2vpn_info));

    if (_SHR_IS_FLAG_SET(l2vpn_info->mpls_port->flags2, BCM_MPLS_PORT2_INGRESS_ONLY))
    {
        SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_get_ingress_only(unit, l2vpn_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_mpls_l2vpn_get_egress_only(unit, l2vpn_info));
    }
    /*
     * For backwards compatibility
     */
    l2vpn_info->mpls_port->flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;

exit:
    SHR_FUNC_EXIT;
}

/*
 * End of internal APIs
 * }
 */
