/** \file switch_tune.c
 * Tune trap module file
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
 * Include files. {
 */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include "switch_tune.h"

/*
 * }  Include files.
 */

/**
 * \brief - Init Split Horizon trap.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *  None.
 *
 * \see
 *  None.
 */
static shr_error_e
dnx_tune_switch_split_horizon_init(
    int unit)
{
    bcm_switch_network_group_t source_network_group_id;
    bcm_switch_network_group_config_t group_config;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialization Split Horizon:
     * Only HUB-HUB is trapped.
     */
    source_network_group_id = 1;
    group_config.dest_network_group_id = 1;
    group_config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;

    SHR_IF_ERR_EXIT(bcm_switch_network_group_config_set(unit, source_network_group_id, &group_config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init default forwarding destination to be notFound trap
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_tune_switch_fwd_not_found_trap_init(
    int unit)
{
    int trap_unknown_dst;
    bcm_rx_trap_config_t config;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUnknownDest, &trap_unknown_dst));
    bcm_rx_trap_config_t_init(&config);
    config.flags |= BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = BCM_GPORT_BLACK_HOLE;
    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_unknown_dst, &config));
    SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchForwardLookupNotFoundTrap, trap_unknown_dst));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init FlexE SAR cell size
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_tune_switch_flexe_sar_cell_size_init(
    int unit)
{
    uint8 is_active;
    bcm_switch_flexe_sar_cell_mode_t cell_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_active_get(unit, &is_active));

    if (is_active)
    {
        cell_mode = bcmSwitchFlexeSarCellMode29x66b;
        SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchFlexeSarCellMode, cell_mode));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See switch_tune.h file
 */
shr_error_e
dnx_tune_switch_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialization Split Horizon:
     */
    if (dnx_data_lif.in_lif.in_lif_profile_allocate_orientation_get(unit))
    {
        SHR_IF_ERR_EXIT(dnx_tune_switch_split_horizon_init(unit));
    }
    /*
     * Initialize not found trap destination.
     */
    SHR_IF_ERR_EXIT(dnx_tune_switch_fwd_not_found_trap_init(unit));
    /*
     * Initialize the FlexE SAR cell size
     */
    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_is_supported))
    {
        if (dnx_data_nif.flexe.flexe_mode_get(unit) == DNX_FLEXE_MODE_DISTRIBUTED)
        {
            SHR_IF_ERR_EXIT(dnx_tune_switch_flexe_sar_cell_size_init(unit));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
