/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
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

#define DNX_PORT_NIF_PORT_TYPE_IS_ARB_ETH(calendar_type) ((calendar_type) >= DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR && (calendar_type) <= DNX_PORT_NIF_ARB_TX_PPM7_CALENDAR)
#define DNX_PORT_NIF_PORT_PM_ID_GET(calendar_type) ((calendar_type) - DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR)

/* Static functions */
/* General */
static shr_error_e dnx_port_nif_calendar_config(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *calendar,
    int calendar_length);
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
static shr_error_e dnx_port_nif_scheduler_mode_get(
    int unit,
    bcm_port_t logical_port,
    int *scheduler_mode);
static shr_error_e dnx_port_nif_scheduler_calendar_slots_per_port_build(
    int unit,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
    int direction,
    int granularity);
static shr_error_e dnx_port_nif_unified_calendar_slots_per_port_build(
    int unit,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
    int direction,
    int granularity);
static shr_error_e dnx_port_nif_arb_tx_eth_slots_per_port_build(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
    int direction,
    int granularity);
static shr_error_e dnx_port_nif_arb_tx_tmac_slots_per_port_build(
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
static shr_error_e dnx_port_calendar_slots_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *calendar,
    int calendar_id,
    int calendar_length);

/* dbal */
static shr_error_e dnx_port_calendar_table_id_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    dbal_tables_e * table_id,
    uint32 *bubble_val);
static shr_error_e dnx_port_calendar_config_table_id_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    dbal_tables_e * table_id);
static shr_error_e dnx_port_calendar_switch_active_id(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type);
static shr_error_e dnx_port_calendar_trigger_switch_active_id(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type);
static shr_error_e dnx_port_calendar_active_id_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    int active_id);
static shr_error_e dnx_port_calendar_active_id_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *active_id);
static shr_error_e dnx_port_qpm_calendar_slots_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *calendar,
    int calendar_id,
    int calendar_length);
static shr_error_e dnx_port_general_calendar_slots_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *calendar,
    int calendar_id,
    int calendar_length);

shr_error_e
dnx_port_calendar_init(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_config_table_id_get(unit, calendar_type, &table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    if (DNX_PORT_NIF_PORT_TYPE_IS_ARB_ETH(calendar_type))
    {
        /*
         * If calendar is eth (ppm0-ppm7).
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU_ID,
                                   DNX_PORT_NIF_PORT_PM_ID_GET(calendar_type));
    }

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_INIT_DONE, INST_SINGLE, 1);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
    SHR_ALLOC_SET_ZERO_ERR_EXIT(calendar, calendar_length * sizeof(uint32),
                                "Calendar to set slots per port", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(nof_slots_per_port, nof_calendar_clients * sizeof(uint32),
                                "Saves how many slots each port needs", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_port_nif_slots_per_port_build
                    (unit, calendar_type, nof_slots_per_port, nof_calendar_clients, direction, granularity));

    SHR_IF_ERR_EXIT(dnx_port_nif_calendar_build
                    (unit, calendar_type, nof_slots_per_port, nof_calendar_clients, calendar, calendar_length));

    SHR_IF_ERR_EXIT(dnx_port_nif_calendar_config(unit, calendar_type, calendar, calendar_length));

exit:
    SHR_FREE(calendar);
    SHR_FREE(nof_slots_per_port);
    SHR_FUNC_EXIT;
}

/*
 * General static functions
 */

static shr_error_e
dnx_port_nif_calendar_config(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *calendar,
    int calendar_length)
{
    uint32 active_id, working_calendar_id;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get active calendar id
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_active_id_get(unit, calendar_type, &active_id));
    working_calendar_id = !active_id;

    /*
     * Update shadow calendar with the new entries 
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_slots_set(unit, calendar_type, calendar, working_calendar_id, calendar_length));

    /*
     * switch active calendar in HW
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_switch_active_id(unit, calendar_type));

exit:
    SHR_FUNC_EXIT;
}

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
            *nof_calendar_clients =
                dnx_data_nif.phys.nof_phys_get(unit) + dnx_data_nif.mac_client.nof_clients_get(unit);
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

        case DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM1_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM2_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM3_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM4_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM5_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM6_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM7_CALENDAR:
            /*
             * set Tx ETH calendar parameters
             */
            *nof_calendar_clients = dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);
            *calendar_length = dnx_data_nif.arb.cdpm_calendar_length_get(unit);
            *direction = DNX_ALGO_PORT_SPEED_F_TX;
            *granularity = dnx_data_nif.arb.tx_eth_port_speed_granularity_get(unit);
            break;

        case DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR:
            /*
             * set Tx TMAC calendar
             */
            *nof_calendar_clients = dnx_data_nif.mac_client.nof_clients_get(unit);
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
            *granularity = dnx_data_nif.oft.calendar_speed_granularity_get(unit);
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
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH,
                     DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_L1, &all_eth_ports));
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FRAMER_MAC,
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_FLEXE_MAC_STAT_ONLY, &all_tmac_ports));

    switch (calendar_type)
    {
        case DNX_PORT_NIF_ARB_RX_SCH_CALENDAR:
            _SHR_PBMP_OR(all_eth_ports, all_tmac_ports);
            SHR_IF_ERR_EXIT(dnx_port_nif_scheduler_calendar_slots_per_port_build
                            (unit, nof_slots_per_port, all_eth_ports, direction, granularity));
            break;

        case DNX_PORT_NIF_ARB_RX_QPM_CALENDAR:
        case DNX_PORT_NIF_OFT_CALENDAR:
            _SHR_PBMP_OR(all_eth_ports, all_tmac_ports);
            SHR_IF_ERR_EXIT(dnx_port_nif_unified_calendar_slots_per_port_build
                            (unit, nof_slots_per_port, all_eth_ports, direction, granularity));
            break;

        case DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM1_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM2_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM3_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM4_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM5_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM6_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM7_CALENDAR:
            SHR_IF_ERR_EXIT(dnx_port_nif_arb_tx_eth_slots_per_port_build
                            (unit, calendar_type, nof_slots_per_port, all_eth_ports, direction, granularity));
            break;

        case DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR:
            SHR_IF_ERR_EXIT(dnx_port_nif_arb_tx_tmac_slots_per_port_build
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
dnx_port_nif_scheduler_mode_get(
    int unit,
    bcm_port_t logical_port,
    int *scheduler_mode)
{
    int src_index, core, nif_port;
    dnx_port_map_destination_e dst_index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, logical_port, 0, &core, &nif_port));
    src_index =
        UTILEX_MIN((nif_port / dnx_data_nif.arb.nof_lanes_in_qpm_get(unit)), dnx_data_nif.arb.nof_rx_qpms_get(unit));
    SHR_IF_ERR_EXIT(dnx_port_arb_map_destination_get(unit, logical_port, &dst_index));

    /*
     * src_index is the external index and dst_index is the internal one.
     */
    *scheduler_mode = dnx_data_nif.arb.nof_rx_sources_get(unit) * src_index + dst_index;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_scheduler_calendar_slots_per_port_build(
    int unit,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
    int direction,
    int granularity)
{
    bcm_port_t logical_port;
    int speed, has_speed, scheduler_mode = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterate through rx ports
     */
    _SHR_PBMP_ITER(all_ports, logical_port)
    {
        speed = 0;
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, 0, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, 0, &speed));
        }

        if (!speed)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_port_nif_scheduler_mode_get(unit, logical_port, &scheduler_mode));
        nof_slots_per_port[scheduler_mode] += (speed / granularity);
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
    int speed, core, nif_port, has_speed;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterate through rx ports
     */
    _SHR_PBMP_ITER(all_ports, logical_port)
    {
        speed = 0;
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, 0, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, 0, &speed));
        }

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
dnx_port_nif_arb_tx_eth_slots_per_port_build(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
    int direction,
    int granularity)
{
    bcm_port_t logical_port;
    int speed, nif_port, has_speed;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterate through tx ports
     */
    _SHR_PBMP_ITER(all_ports, logical_port)
    {
        speed = 0;
        SHR_IF_ERR_EXIT(dnx_port_arb_tx_context_get(unit, logical_port, &nif_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, 0, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, 0, &speed));
        }

        if (!speed
            || nif_port / dnx_data_nif.eth.nof_lanes_in_cdu_get(unit) != (DNX_PORT_NIF_PORT_PM_ID_GET(calendar_type)))
        {
            continue;
        }
        nof_slots_per_port[nif_port % dnx_data_nif.eth.cdu_nof_get(unit)] = (speed / granularity);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_arb_tx_tmac_slots_per_port_build(
    int unit,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
    int direction,
    int granularity)
{
    bcm_port_t logical_port;
    int speed, nif_port, has_speed;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterate through tx ports
     */
    _SHR_PBMP_ITER(all_ports, logical_port)
    {
        speed = 0;
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, 0, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, 0, &speed));
        }

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
        case DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM1_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM2_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM3_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM4_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM5_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM6_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM7_CALENDAR:
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

static shr_error_e
dnx_port_calendar_table_id_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    dbal_tables_e * table_id,
    uint32 *bubble_val)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (calendar_type)
    {
        case DNX_PORT_NIF_ARB_RX_QPM_CALENDAR:
            *table_id = DBAL_TABLE_NIF_ARB_RX_QPM_CALENDAR;
            *bubble_val = dnx_data_nif.arb.rx_qpm_calendar_nof_clients_get(unit);
            break;

        case DNX_PORT_NIF_ARB_RX_SCH_CALENDAR:
            *table_id = DBAL_TABLE_NIF_ARB_RX_SCH_CALENDAR;
            *bubble_val = dnx_data_nif.arb.rx_sch_calendar_nof_modes_get(unit);
            break;

        case DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM1_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM2_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM3_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM4_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM5_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM6_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM7_CALENDAR:
            *table_id = DBAL_TABLE_NIF_ARB_TX_PPM_CALENDAR;
            *bubble_val = dnx_data_nif.eth.nof_lanes_in_cdu_get(unit) + 1;
            break;

        case DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR:
            *table_id = DBAL_TABLE_NIF_ARB_TX_TMAC_CALENDAR;
            *bubble_val = dnx_data_nif.mac_client.nof_clients_get(unit);
            break;

        case DNX_PORT_NIF_OFT_CALENDAR:
            *table_id = DBAL_TABLE_NIF_OFT_CALENDAR;
            *bubble_val = dnx_data_nif.oft.nof_contexts_get(unit);
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar type %d is not supported.", calendar_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_calendar_config_table_id_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    dbal_tables_e * table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (calendar_type)
    {
        case DNX_PORT_NIF_ARB_RX_QPM_CALENDAR:
            *table_id = DBAL_TABLE_NIF_ARB_RX_QPM_CALENDAR_CONFIG;
            break;

        case DNX_PORT_NIF_ARB_RX_SCH_CALENDAR:
            *table_id = DBAL_TABLE_NIF_ARB_RX_SCH_CALENDAR_CONFIG;
            break;

        case DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM1_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM2_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM3_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM4_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM5_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM6_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM7_CALENDAR:
            *table_id = DBAL_TABLE_NIF_ARB_TX_PPM_CALENDAR_CONFIG;
            break;

        case DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR:
            *table_id = DBAL_TABLE_NIF_ARB_TX_TMAC_CALENDAR_CONFIG;
            break;

        case DNX_PORT_NIF_OFT_CALENDAR:
            *table_id = DBAL_TABLE_NIF_OFT_CALENDAR_CONFIG;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar type %d is not supported.", calendar_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_calendar_switch_active_id(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type)
{
    uint32 active_id;
    SHR_FUNC_INIT_VARS(unit);

    switch (calendar_type)
    {
        case DNX_PORT_NIF_ARB_RX_QPM_CALENDAR:
        case DNX_PORT_NIF_ARB_RX_SCH_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM1_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM2_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM3_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM4_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM5_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM6_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM7_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR:
            SHR_IF_ERR_EXIT(dnx_port_calendar_active_id_get(unit, calendar_type, &active_id));
            SHR_IF_ERR_EXIT(dnx_port_calendar_active_id_set(unit, calendar_type, !active_id));
            break;

        case DNX_PORT_NIF_OFT_CALENDAR:
            SHR_IF_ERR_EXIT(dnx_port_calendar_trigger_switch_active_id(unit, calendar_type));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar type %d is not supported.", calendar_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_calendar_trigger_switch_active_id(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_config_table_id_get(unit, calendar_type, &table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRIGGER_SWITCH, INST_SINGLE, TRUE);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRIGGER_SWITCH, INST_SINGLE, FALSE);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_calendar_active_id_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    int active_id)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_config_table_id_get(unit, calendar_type, &table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    if (DNX_PORT_NIF_PORT_TYPE_IS_ARB_ETH(calendar_type))
    {
        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU_ID,
                                   DNX_PORT_NIF_PORT_PM_ID_GET(calendar_type));
    }

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTIVE_CALENDAR, INST_SINGLE, active_id);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_calendar_active_id_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *active_id)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_config_table_id_get(unit, calendar_type, &table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    if (DNX_PORT_NIF_PORT_TYPE_IS_ARB_ETH(calendar_type))
    {
        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU_ID,
                                   DNX_PORT_NIF_PORT_PM_ID_GET(calendar_type));
    }

    /*
     * set value fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ACTIVE_CALENDAR, INST_SINGLE, active_id);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_calendar_slots_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *calendar,
    int calendar_id,
    int calendar_length)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (calendar_type)
    {
        case DNX_PORT_NIF_ARB_RX_QPM_CALENDAR:
            SHR_IF_ERR_EXIT(dnx_port_qpm_calendar_slots_set
                            (unit, calendar_type, calendar, calendar_id, calendar_length));
            break;

        case DNX_PORT_NIF_ARB_RX_SCH_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR:
        case DNX_PORT_NIF_OFT_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM1_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM2_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM3_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM4_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM5_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM6_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM7_CALENDAR:
            SHR_IF_ERR_EXIT(dnx_port_general_calendar_slots_set
                            (unit, calendar_type, calendar, calendar_id, calendar_length));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar type %d is not supported.", calendar_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_qpm_calendar_slots_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *calendar,
    int calendar_id,
    int calendar_length)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;
    uint32 client_val, bubble_val;
    uint32 qpm_id, other_qpm_id;
    int calendar_entry_index;
    const int nof_eth_ports = dnx_data_nif.phys.nof_phys_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_table_id_get(unit, calendar_type, &table_id, &bubble_val));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /*
     * set const key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_INDEX, calendar_id);

    for (calendar_entry_index = 0; calendar_entry_index < calendar_length; ++calendar_entry_index)
    {
        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_INDEX, calendar_entry_index);

        if (calendar[calendar_entry_index] != DNX_ALGO_CAL_ILLEGAL_OBJECT_ID)
        {
            /*
             * set QPM_ID (ports 0-31 to QPM0, 32 - 63 to QPM1). the other QPM in this entry is bubble.
             */
            client_val = calendar[calendar_entry_index];
            qpm_id = client_val / (nof_eth_ports / 2);
            other_qpm_id = dnx_data_nif.arb.nof_rx_qpms_get(unit) - qpm_id - 1;

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPM_ID, qpm_id);

            /*
             * set value fields
             */

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT, INST_SINGLE, client_val);

            /*
             * commit the value
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPM_ID, other_qpm_id);

            /*
             * set value fields
             */

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT, INST_SINGLE, bubble_val);

            /*
             * commit the value
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        }
        else
        {
            dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_QPM_ID, 0,
                                             dnx_data_nif.arb.nof_rx_qpms_get(unit) - 1);

            /*
             * set value fields
             */

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT, INST_SINGLE, bubble_val);

            /*
             * commit the value
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_general_calendar_slots_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *calendar,
    int calendar_id,
    int calendar_length)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id, client_val, bubble_val;
    int calendar_entry_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_table_id_get(unit, calendar_type, &table_id, &bubble_val));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /*
     * set const key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_INDEX, calendar_id);

    /*
     * If calendar is eth (ppm0-ppm7).
     */
    if (DNX_PORT_NIF_PORT_TYPE_IS_ARB_ETH(calendar_type))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU_ID,
                                   DNX_PORT_NIF_PORT_PM_ID_GET(calendar_type));
    }

    for (calendar_entry_index = 0; calendar_entry_index < calendar_length; ++calendar_entry_index)
    {
        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_INDEX, calendar_entry_index);

        /*
         * set value fields
         */
        client_val =
            (calendar[calendar_entry_index] ==
             DNX_ALGO_CAL_ILLEGAL_OBJECT_ID) ? bubble_val : calendar[calendar_entry_index];

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT, INST_SINGLE, client_val);

        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
