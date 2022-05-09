/** \file tsn_tune.c
 * Tuning of tsn features
 * 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TSN

/*
 * Include files.
 * {
 */

#include <bcm/port.h>
#include <bcm_int/dnx/auto_generated/dnx_port_dispatch.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#include <soc/sand/shrextend/shrextend_debug.h>

/*
 * }
 * Include files.
 */

/**
 * \brief - frame preemption default settings
 *         set the default frame preemption fragment sizes.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_tsn_tune_frame_preemption_port_add(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Configure preemption fragment sizes */
    SHR_IF_ERR_EXIT(bcm_dnx_port_preemption_control_set(unit, port, bcmPortPreemptControlNonFinalFragSizeRx, 256));
    SHR_IF_ERR_EXIT(bcm_dnx_port_preemption_control_set(unit, port, bcmPortPreemptControlFinalFragSizeRx, 64));
    SHR_IF_ERR_EXIT(bcm_dnx_port_preemption_control_set(unit, port, bcmPortPreemptControlNonFinalFragSizeTx, 512));
    SHR_IF_ERR_EXIT(bcm_dnx_port_preemption_control_set(unit, port, bcmPortPreemptControlFinalFragSizeTx, 64));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_tsn_tune_port_add(
    int unit,
    bcm_port_t port)
{
    int frame_preemptable = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_frame_preemptable_get(unit, port, &frame_preemptable));
    if (frame_preemptable)
    {
        /** Frame preemption tuning */
        SHR_IF_ERR_EXIT(dnx_tsn_tune_frame_preemption_port_add(unit, port));
    }

exit:
    SHR_FUNC_EXIT;
}
