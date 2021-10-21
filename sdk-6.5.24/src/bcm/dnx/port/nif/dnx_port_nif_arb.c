/**
 *
 * \file dnx_port_nif_arb.c 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * NIF Arbiter procedures for DNX.
 */

/*
 * Include files:
 * {
 */
#include "dnx_port_nif_arb_internal.h"
#include "dnx_port_nif_link_list_internal.h"
#include "dnx_port_nif_calendar_internal.h"

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

    /*
     * Initialize arb link list. 
     */
    SHR_IF_ERR_EXIT(dnx_port_link_list_init
                    (unit, DNX_PORT_NIF_ARB_LINK_LIST, dnx_data_nif.arb.tx_tmac_nof_sections_get(unit),
                     dnx_data_nif.flexe.nof_clients_get(unit)));

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
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, TRUE))
    {
        /*
         * Read current mode & calc new Mac mode
         */
        SHR_IF_ERR_EXIT(dnx_port_arb_mac_mode_get(unit, logical_port, &mac_mode));

        /*
         * Set port mac mode config
         */
        SHR_IF_ERR_EXIT(dnx_port_arb_port_mac_mode_config_set(unit, logical_port, mac_mode));

    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_port_remove(
    int unit,
    bcm_port_t logical_port)
{
    int mac_mode_config_nof_ports, nof_channels;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get number of channels
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, logical_port, &nof_channels));

    /*
     * Check if port is eth port 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, TRUE))
    {
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
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_calendar_set(
    int unit,
    uint32 direction)
{
    SHR_FUNC_INIT_VARS(unit);

    if (direction & ARB_RX_DIRECTION)
    {
        /*
         * set Rx QPM calendar 
         */
        SHR_IF_ERR_EXIT(dnx_port_calendar_set(unit, DNX_PORT_NIF_ARB_RX_QPM_CALENDAR));

        /*
         * set Rx SCH calendar 
         */
        
        if (0)
        {
            SHR_IF_ERR_EXIT(dnx_port_calendar_set(unit, DNX_PORT_NIF_ARB_RX_SCH_CALENDAR));
        }
    }

    if (direction & ARB_TX_DIRECTION)
    {
        /*
         * set Tx eth calendar 
         */
        SHR_IF_ERR_EXIT(dnx_port_calendar_set(unit, DNX_PORT_NIF_ARB_TX_ETH_CALENDAR));

        /*
         * set Tx TMAC calendar 
         */
        SHR_IF_ERR_EXIT(dnx_port_calendar_set(unit, DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR));
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
    int speed, nof_sections;
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
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &speed));
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
dnx_port_arb_port_enable(
    int unit,
    bcm_port_t logical_port,
    uint32 direction,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_credits_init(
    int unit,
    bcm_port_t logical_port,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}
