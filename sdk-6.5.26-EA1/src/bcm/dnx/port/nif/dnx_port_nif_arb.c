/**
 *
 * \file dnx_port_nif_arb.c 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * NIF Arbiter procedures for DNX.
 *
 */

/*
 * Include files:
 * {
 */
#include "dnx_port_nif_arb_internal.h"
#include "dnx_port_nif_link_list_internal.h"
#include "dnx_port_nif_calendar_internal.h"

#include <shared/pbmp.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/port/imb/imb_ethu.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/stat/stif/stif_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_nif_access.h>

/*
 * }
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

shr_error_e
dnx_port_arb_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_port_arb_enable_dynamic_memories_set(unit, TRUE));

    /*
     * Set tx bypass fifo out of reset
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_tx_bypass_fifo_reset_set(unit, IMB_COMMON_OUT_OF_RESET));

    /*
     * Set rx pm default minimal packet size
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_rx_pm_default_min_pkt_size_set(unit));

    /*
     * Initialize arb link list. 
     */
    SHR_IF_ERR_EXIT(dnx_port_link_list_init
                    (unit, DNX_PORT_NIF_ARB_LINK_LIST, dnx_data_nif.arb.tx_tmac_nof_sections_get(unit)));

    /*
     * Initialize arb calendars. 
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_init_all_calendars(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_power_down(
    int unit)
{
    int cdu_id;
    const int nof_cdus = dnx_data_nif.eth.cdu_nof_per_core_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_arb_global_powerdown_set(unit, TRUE));
    SHR_IF_ERR_EXIT(dnx_port_arb_tmac_powerdown_set(unit, TRUE));

    for (cdu_id = 0; cdu_id < nof_cdus; cdu_id++)
    {
        SHR_IF_ERR_EXIT(dnx_port_arb_pm_powerdown_set(unit, cdu_id, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_port_add(
    int unit,
    bcm_port_t logical_port)
{
    int mac_mode;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check if port is eth port 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1)))
    {
        /*
         * Stop data toward port macro
         */
        SHR_IF_ERR_EXIT(dnx_port_arb_stop_data_set(unit, logical_port, TRUE));

        /*
         * Read current mode & calc new Mac mode
         */
        SHR_IF_ERR_EXIT(dnx_port_arb_mac_mode_get(unit, logical_port, &mac_mode));

        /*
         * Set port mac mode config
         */
        SHR_IF_ERR_EXIT(dnx_port_arb_port_mac_mode_config_set(unit, logical_port, mac_mode));

        /*
         * Start data toward port macro
         */
        SHR_IF_ERR_EXIT(dnx_port_arb_stop_data_set(unit, logical_port, FALSE));
    }

    SHR_IF_ERR_EXIT(dnx_port_arb_disable_datapath_set(unit, logical_port, TRUE));

    /*
     * Set destination mapping as bypass as default value
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_map_destination_set(unit, logical_port, DNX_PORT_ARB_MAP_DESTINATION_BYPASS));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_port_remove(
    int unit,
    bcm_port_t logical_port)
{
    bcm_pbmp_t calendars_to_be_set;
    int mac_mode_config_nof_ports, nof_channels;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Set destination mapping to bypass
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_map_destination_set(unit, logical_port, DNX_PORT_ARB_MAP_DESTINATION_BYPASS));

    /*
     * get number of channels
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, logical_port, &nof_channels));

    /*
     * Check if port is eth port 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1)))
    {
        /*
         * Stop data toward port macro
         */
        SHR_IF_ERR_EXIT(dnx_port_arb_stop_data_set(unit, logical_port, TRUE));

        /*
         * get number of ports on the MAC mode config
         */
        SHR_IF_ERR_EXIT(dnx_port_arb_mac_mode_config_nof_ports_get(unit, logical_port, &mac_mode_config_nof_ports));

        if ((mac_mode_config_nof_ports == 1) && (nof_channels == 1))
        {
            /*
             * disable MAC mode mode when last channel in that mac mode config unit.
             */
            SHR_IF_ERR_EXIT(dnx_port_arb_port_mac_mode_config_set(unit, logical_port, 0));
        }
        /*
         * Rebuild relevant calendars
         */
        _SHR_PBMP_CLEAR(calendars_to_be_set);
        SHR_IF_ERR_EXIT(dnx_port_arb_rx_calendar_ids_get(unit, logical_port, &calendars_to_be_set));
        SHR_IF_ERR_EXIT(dnx_port_arb_tx_calendar_ids_get(unit, logical_port, &calendars_to_be_set));
        SHR_IF_ERR_EXIT(dnx_port_arb_calendar_set(unit, calendars_to_be_set));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_calendar_set(
    int unit,
    bcm_pbmp_t calendars)
{
    bcm_port_t calendar;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Setting only the relevant calendars.
     */
    _SHR_PBMP_ITER(calendars, calendar)
    {
        SHR_IF_ERR_EXIT(dnx_port_calendar_set(unit, calendar));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_tx_tmac_link_list_set(
    int unit,
    bcm_port_t logical_port,
    int enable)
{
    int speed = 0, has_speed, nof_sections;
    int client_id;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get arbiter context
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_tx_context_get(unit, logical_port, &client_id));

    if (enable)
    {
        /*
         * Get nof_sections from port's speed
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &speed));
        }
        SHR_IF_ERR_EXIT(dnx_port_arb_link_list_sections_required_get(unit, speed, &nof_sections));

        /*
         * Allocate new link list
         */
        SHR_IF_ERR_EXIT(dnx_port_link_list_alloc(unit, DNX_PORT_NIF_ARB_LINK_LIST, nof_sections, client_id));
    }
    else
    {
        /*
         * Free link list
         */
        SHR_IF_ERR_EXIT(dnx_port_link_list_free(unit, DNX_PORT_NIF_ARB_LINK_LIST, client_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_tx_sif_port_add(
    int unit,
    bcm_stat_stif_source_t source,
    bcm_port_t logical_port)
{
    int instance_id = DNX_STIF_MGMT_INSTANCE_INVALID;
    bcm_core_t port_core = BCM_CORE_ALL;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, logical_port, &ethu_info));

    /*
     * Get sif port id
     */
    SHR_IF_ERR_EXIT(dnx_stif_mgmt_logical_port_to_instance_id_get
                    (unit, logical_port, source, &port_core, &instance_id));

    /*
     * Enable sif bit. 
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_sif_enable_set(unit, instance_id, TRUE));

    /*
     * Set offset of sif port. 
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_sif_port_set(unit, instance_id, ethu_info.first_lane_in_port));

    /*
     * Take out of reset the sif async fifo. 
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_sif_reset_set(unit, instance_id, IMB_COMMON_OUT_OF_RESET));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_port_enable(
    int unit,
    bcm_port_t logical_port,
    uint32 direction,
    int enable)
{
    int in_reset;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    in_reset = (enable) ? IMB_COMMON_OUT_OF_RESET : IMB_COMMON_IN_RESET;

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (direction == ARB_TX_DIRECTION && DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info))
    {
        /*
         * Reset sif port
         */
        SHR_IF_ERR_EXIT(dnx_port_arb_sif_reset_set(unit, logical_port, in_reset));
    }

    /*
     * Check arbiter context type 
     */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1)))
    {
        /*
         * Reset eth port
         */
        SHR_IF_ERR_EXIT(dnx_port_arb_pm_port_reset(unit, logical_port, direction, in_reset));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, FALSE, FALSE))
    {
        /*
         * Reset tmac port
         */
        SHR_IF_ERR_EXIT(dnx_port_arb_tmac_port_reset(unit, logical_port, direction, in_reset));
    }
    /*
     * In case of ETH L1 and Framer MAC STAT keep the datapath disabled, traffic will be dropped (used only for monitoring)
     */
    if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, TRUE)
        && !DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC_STAT(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_port_arb_disable_datapath_set(unit, logical_port, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_credits_init(
    int unit,
    bcm_port_t logical_port,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_arb_credits_init_set(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_tmac_credits_set(
    int unit,
    bcm_port_t logical_port,
    int enable)
{
    int speed = 0, has_speed;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
    if (has_speed)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &speed));
    }
    SHR_IF_ERR_EXIT(dnx_port_arb_tmac_credits_calc_and_set(unit, logical_port, speed));
    /*
     * Enable credits in Arb. Tx
     */
    if (enable)
    {
        SHR_IF_ERR_EXIT(dnx_port_arb_credits_init(unit, logical_port, TRUE));
        SHR_IF_ERR_EXIT(dnx_port_arb_credits_init(unit, logical_port, FALSE));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_pm_credits_set(
    int unit,
    bcm_port_t logical_port,
    int credits_val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_arb_pm_credits_hw_set(unit, logical_port, credits_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_tmac_start_tx_threshold_set(
    int unit,
    bcm_port_t logical_port)
{
    int speed = 0, has_speed, threshold_value;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get port's speed
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
    if (has_speed)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &speed));
    }

    /*
     * Comparing port's speed to arbiter's high/low thresholds
     */
    if (speed >= dnx_data_nif.arb.high_start_tx_speed_threshold_get(unit))
    {
        threshold_value = dnx_data_nif.arb.high_start_tx_speed_value_get(unit);
    }
    else if (speed > dnx_data_nif.arb.low_start_tx_speed_threshold_get(unit))
    {
        threshold_value = dnx_data_nif.arb.middle_start_tx_speed_value_get(unit);
    }
    else
    {
        threshold_value = dnx_data_nif.arb.low_start_tx_speed_value_get(unit);
    }

    SHR_IF_ERR_EXIT(dnx_port_arb_tmac_start_tx_threshold_hw_set(unit, logical_port, threshold_value));

exit:
    SHR_FUNC_EXIT;
}
