/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#include "dnx_port_nif_calendar_internal.h"
#include "dnx_port_nif_arb_internal.h"
#include <bcm/types.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/cal/algo_cal.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_nif_types.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#define DNX_PORT_NIF_ARB_RX_SCH_CALENDAR_BUBBLE_VALUE (9)

/* Static functions */
/* General */
static shr_error_e dnx_port_calendar_algo_parameters_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    int *nof_calendar_clients,
    int *calendar_length,
    int *direction,
    int *granularity);
static shr_error_e dnx_port_nif_slots_per_port_build(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    int nof_calendar_clients,
    int direction,
    int granularity);
static shr_error_e dnx_port_nif_unified_calendar_slots_per_port_build(
    int unit,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
    int direction,
    int granularity);
static shr_error_e dnx_port_nif_arb_tx_slots_per_port_build(
    int unit,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
    int direction,
    int granularity);
static shr_error_e dnx_port_nif_calendar_build(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    int nof_calendar_clients,
    uint32 *calendar,
    int calendar_length);
shr_error_e
dnx_port_calendar_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type)
{
    uint32 *nof_slots_per_port = NULL, *calendar = NULL;
    int nof_calendar_clients, calendar_length, direction, granularity;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get calendar parameters for algo
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_algo_parameters_get
                    (unit, calendar_type, &nof_calendar_clients, &calendar_length, &direction, &granularity));

    /*
     * Allocating arraies memory
     */
    SHR_ALLOC_SET_ZERO(calendar, calendar_length * sizeof(uint32),
                       "Calendar to set slots per port", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(nof_slots_per_port, nof_calendar_clients * sizeof(uint32),
                       "Saves how many slots each port needs", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_port_nif_slots_per_port_build
                    (unit, calendar_type, nof_slots_per_port, nof_calendar_clients, direction, granularity));

    SHR_IF_ERR_EXIT(dnx_port_nif_calendar_build
                    (unit, calendar_type, nof_slots_per_port, nof_calendar_clients, calendar, calendar_length));

exit:
    SHR_FREE(calendar);
    SHR_FREE(nof_slots_per_port);
    SHR_FUNC_EXIT;
}

/*
 * General static functions
 */
static shr_error_e
dnx_port_calendar_algo_parameters_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    int *nof_calendar_clients,
    int *calendar_length,
    int *direction,
    int *granularity)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (calendar_type)
    {
        case DNX_PORT_NIF_ARB_RX_QPM_CALENDAR:
            /*
             * set Rx QPM calendar parameters
             */
            *nof_calendar_clients = dnx_data_nif.phys.nof_phys_get(unit) + dnx_data_nif.flexe.nof_clients_get(unit);
            *calendar_length = dnx_data_nif.arb.rx_qpm_calendar_length_get(unit);
            *direction = DNX_ALGO_PORT_SPEED_F_RX;
            *granularity = dnx_data_nif.arb.rx_qpm_port_speed_granularity_get(unit);
            break;

        case DNX_PORT_NIF_ARB_RX_SCH_CALENDAR:
            /*
             * set Rx SCH calendar parameters
             */
            *nof_calendar_clients = dnx_data_nif.arb.rx_sch_calendar_nof_modes_get(unit);
            *calendar_length = dnx_data_nif.arb.rx_sch_calendar_length_get(unit);
            *direction = DNX_ALGO_PORT_SPEED_F_RX;
            *granularity = dnx_data_nif.arb.rx_sch_port_speed_granularity_get(unit);
            break;

        case DNX_PORT_NIF_ARB_TX_ETH_CALENDAR:
            /*
             * set Tx ETH calendar parameters
             */
            *nof_calendar_clients = dnx_data_nif.phys.nof_phys_get(unit);
            *calendar_length = dnx_data_nif.arb.cdpm_calendar_length_get(unit);
            *direction = DNX_ALGO_PORT_SPEED_F_TX;
            *granularity = dnx_data_nif.arb.tx_eth_port_speed_granularity_get(unit);
            break;

        case DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR:
            /*
             * set Tx TMAC calendar
             */
            *nof_calendar_clients = dnx_data_nif.flexe.nof_clients_get(unit);
            *calendar_length = dnx_data_nif.arb.tmac_calendar_length_get(unit);
            *direction = DNX_ALGO_PORT_SPEED_F_TX;
            *granularity = dnx_data_nif.arb.tx_tmac_port_speed_granularity_get(unit);
            break;

        case DNX_PORT_NIF_OFT_CALENDAR:
            /*
             * set OFT calendar parameters
             */
            *nof_calendar_clients = dnx_data_nif.oft.nof_contexts_get(unit);
            *calendar_length = dnx_data_nif.oft.calendar_length_get(unit);
            *direction = DNX_ALGO_PORT_SPEED_F_TX;
            *granularity = dnx_data_nif.oft.port_speed_granularity_get(unit);
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar type %d is not supported.", calendar_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_slots_per_port_build(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    int nof_calendar_clients,
    int direction,
    int granularity)
{
    bcm_pbmp_t all_eth_ports, all_tmac_ports;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get all logical ports
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                     &all_eth_ports));
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_MAC, 0, &all_tmac_ports));

    switch (calendar_type)
    {
        case DNX_PORT_NIF_ARB_RX_SCH_CALENDAR:
            _SHR_PBMP_OR(all_eth_ports, all_tmac_ports);
            break;

        case DNX_PORT_NIF_ARB_RX_QPM_CALENDAR:
        case DNX_PORT_NIF_OFT_CALENDAR:
            _SHR_PBMP_OR(all_eth_ports, all_tmac_ports);
            SHR_IF_ERR_EXIT(dnx_port_nif_unified_calendar_slots_per_port_build
                            (unit, nof_slots_per_port, all_eth_ports, direction, granularity));
            break;

        case DNX_PORT_NIF_ARB_TX_ETH_CALENDAR:
            SHR_IF_ERR_EXIT(dnx_port_nif_arb_tx_slots_per_port_build
                            (unit, nof_slots_per_port, all_eth_ports, direction, granularity));
            break;

        case DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR:
            SHR_IF_ERR_EXIT(dnx_port_nif_arb_tx_slots_per_port_build
                            (unit, nof_slots_per_port, all_tmac_ports, direction, granularity));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar type %d is not supported.", calendar_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_unified_calendar_slots_per_port_build(
    int unit,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
    int direction,
    int granularity)
{
    bcm_port_t logical_port;
    int speed, core, nif_port;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterate through rx ports
     */
    _SHR_PBMP_ITER(all_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, direction, &speed));
        if (!speed)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, logical_port, 0, &core, &nif_port));
        nof_slots_per_port[nif_port] = (speed / granularity);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_arb_tx_slots_per_port_build(
    int unit,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
    int direction,
    int granularity)
{
    bcm_port_t logical_port;
    int speed, nif_port;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterate through tx ports
     */
    _SHR_PBMP_ITER(all_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, direction, &speed));
        if (!speed)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_port_arb_tx_context_get(unit, logical_port, &nif_port));
        nof_slots_per_port[nif_port] = (speed / granularity);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_calendar_build(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    int nof_calendar_clients,
    uint32 *calendar,
    int calendar_length)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (calendar_type)
    {
        case DNX_PORT_NIF_ARB_RX_QPM_CALENDAR:
        case DNX_PORT_NIF_ARB_RX_SCH_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_ETH_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR:
        case DNX_PORT_NIF_OFT_CALENDAR:
            SHR_IF_ERR_EXIT(dnx_algo_cal_simple_fixed_len_cal_build
                            (unit, TRUE, nof_slots_per_port, nof_calendar_clients, calendar_length, calendar_length,
                             calendar));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar type %d is not supported.", calendar_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * DBAL static functions
 */
