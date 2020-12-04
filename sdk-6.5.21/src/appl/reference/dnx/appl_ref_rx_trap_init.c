/** \file appl_ref_rx_trap_init.c
 * 
 *
 * Rx trap application procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RX

 /*
  * Include files.
  * {
  */

/** soc */
#include <soc/schanmsg.h>
#include <shared/utilex/utilex_seq.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm/rx.h>
/** sal */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>

/*
 * }
 */

/*
 * DEFINEs
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/**
 * \brief
 *   This function creates bcmRxTrapDfltRedirectToCpuPacket trap and
 *   configures it's destination to CPU port
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_rx_trap_to_cpu_init(
    int unit)
{
    int trap_id;
    bcm_rx_trap_config_t trap_config;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapDfltRedirectToCpuPacket, &trap_id));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
    trap_config.dest_port = BCM_GPORT_LOCAL_CPU;
    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &trap_config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function initializes rx trap application
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e
appl_dnx_rx_trap_init(
    int unit)
{
    bcm_port_config_t port_config;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    if (BCM_PBMP_NOT_NULL(port_config.cpu))
    {
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_to_cpu_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
