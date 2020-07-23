/** \file port_ingr_reassembly_utils.c
 * 
 * 
 * Internal DNX Port APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/** ----------------------------------------------------------------------------------------------------------- */
/** STUBS */

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>

/**
 * \brief - Returns TRUE iff port belongs to interleaved interface
 */
shr_error_e
dnx_port_ingr_reassembly_is_interface_interleaved(
    int unit,
    bcm_port_t port,
    int *is_interleaved)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    *is_interleaved = FALSE;

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    /*
     * if both non interleaved ILKN and dedicated TDM context are supported
     * interface with dedicated TDM context should be excluded here
     */

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 0))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_is_ingress_interleave_get(unit, port, is_interleaved));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Returns true in should_handle_tdm argument iff 
 * reassembly context is configured for TDM ports of this port type
 */
shr_error_e
dnx_port_ingr_reassembly_port_should_handle_tdm(
    int unit,
    bcm_port_t port,
    int *should_handle_tdm)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0, 0))
    {
        *should_handle_tdm = 1;
    }
    else
    {
        *should_handle_tdm = 0;
    }

exit:
    SHR_FUNC_EXIT;
}
