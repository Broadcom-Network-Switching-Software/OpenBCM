/*! \file stg_port.c
 * 
 *
 * Spanning Tree procedures per port for DNX.
 * Allows to Set/Get STP state for a port.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_STG

/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>

#include <bcm/types.h>
#include <bcm_int/dnx/stg/stg.h>

/*
 * }
 */

/**
 * \brief
 *   Set the spanning tree state for a port.
 *   See stg.h for more information.
 */
shr_error_e
dnx_port_stp_set(
    int unit,
    bcm_port_t port,
    int state)
{

    bcm_stg_t *list = NULL;
    shr_error_e rv = _SHR_E_NONE;
    int count = 0, index;

    SHR_FUNC_INIT_VARS(unit);

    rv = bcm_dnx_stg_list(unit, &list, &count);

    /*
     * For the CHIPs on which STP are not supported, if configuring stp state
     * is not forward, return error.
     */
    if (rv == _SHR_E_UNAVAIL)
    {
        if (state == BCM_STG_STP_FORWARD)
        {
            rv = _SHR_E_NONE;
        }
        else
        {
            rv = _SHR_E_PARAM;
        }
    }
    else if (SHR_SUCCESS(rv))
    {
        for (index = 0; index < count; index++)
        {
            rv = bcm_dnx_stg_stp_set(unit, list[index], port, state);
            if (SHR_FAILURE(rv))
            {
                break;
            }
        }
    }
    SHR_IF_ERR_EXIT(bcm_dnx_stg_list_destroy(unit, list, count));
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the spanning tree state for a port in the default STG.
 *   See stg.h for more information.
 */
shr_error_e
dnx_port_stp_get(
    int unit,
    bcm_port_t port,
    int *state)
{
    bcm_stg_t stg_defl;
    shr_error_e rv = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(state, _SHR_E_PARAM, "stp_state");

    rv = bcm_dnx_stg_default_get(unit, &stg_defl);
    if (SHR_SUCCESS(rv))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_stg_stp_get(unit, stg_defl, port, state));
    }
    if (rv == _SHR_E_UNAVAIL)
    {
        /** If STG-STP APIs are not available, return forward directly. */
        rv = _SHR_E_NONE;
        *state = BCM_STG_STP_FORWARD;
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}
