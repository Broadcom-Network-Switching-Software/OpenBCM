/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#include "dnx_port_nif_arb_internal.h"
#include "dnx_port_nif_ofr_internal.h"
#include "../imb/imb_framer_internal.h"

#include <bcm/types.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_rand.h>
#include <bcm_int/dnx/algo/cal/algo_cal.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_feu_phy.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_calendar.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_nif_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_nif_access.h>
#ifdef INCLUDE_XFLOW_MACSEC
#include <dnx/dnx_sec.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_macsec.h>
#endif

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#define DNX_PORT_NIF_PORT_TYPE_IS_ARB_ETH(calendar_type) ((calendar_type) >= DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR && (calendar_type) <= DNX_PORT_NIF_ARB_TX_PPM7_CALENDAR)
#define DNX_PORT_NIF_PORT_PM_ID_GET(calendar_type) ((calendar_type) - DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR)
#define DNX_PORT_NIF_PORT_MACSEC_ID_GET(calendar_type) ((calendar_type) - DNX_PORT_NIF_MACSEC0_CALENDAR)
#define DNX_PORT_NIF_PORT_TYPE_IS_MACSEC(calendar_type) ((calendar_type) >= DNX_PORT_NIF_MACSEC0_CALENDAR && (calendar_type) <= DNX_PORT_NIF_MACSEC1_CALENDAR)

/* Static functions */
/** General */
static shr_error_e dnx_port_calendar_calc_slots_parameters_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    int *direction,
    int *granularity);
static shr_error_e dnx_port_calendar_nof_clients_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_calendar_clients);
static shr_error_e dnx_port_nif_slots_per_port_prepare(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    int nof_calendar_clients,
    int direction,
    int granularity,
    bcm_pbmp_t * ports_tests,
    bcm_pbmp_t * total_tests,
    bcm_pbmp_t * check_old_cal,
    dnx_port_nif_calendar_check_t tests_data[DNX_PORT_NIF_CALENDAR_CHECK_NOF]);
static shr_error_e dnx_port_nif_scheduler_mode_get(
    int unit,
    bcm_port_t logical_port,
    int *scheduler_mode);
static shr_error_e dnx_port_nif_rx_scheduler_calendar_slots_per_port_build(
    int unit,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
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
    int granularity);
static shr_error_e dnx_port_nif_arb_tx_tmac_slots_per_port_build(
    int unit,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
    int granularity);
static shr_error_e dnx_port_nif_framer_sb_slots_per_port_build(
    int unit,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
    int direction,
    int granularity);
static shr_error_e dnx_port_nif_feu_phy_slots_per_port_build(
    int unit,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
    int direction,
    int granularity);
static shr_error_e dnx_port_nif_calendar_with_transition_prepare_and_build(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    int nof_clients,
    uint32 *calendar,
    uint32 calendar_length,
    dnx_algo_cal_random_start_info_t * info,
    bcm_pbmp_t * ports_tests,
    bcm_pbmp_t check_old_cal,
    bcm_pbmp_t total_tests,
    dnx_port_nif_calendar_check_t tests_data[DNX_PORT_NIF_CALENDAR_CHECK_NOF],
    dnx_algo_cal_random_object_transition_info_t * trans_info);
static shr_error_e dnx_port_nif_calendar_build_and_fix(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    int nof_calendar_clients,
    uint32 *calendar,
    uint32 *calendar_length,
    bcm_pbmp_t * ports_tests,
    bcm_pbmp_t check_old_cal,
    bcm_pbmp_t total_tests,
    dnx_port_nif_calendar_check_t tests_data[DNX_PORT_NIF_CALENDAR_CHECK_NOF]);
static shr_error_e dnx_port_calendar_slots_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *calendar,
    int calendar_id,
    int calendar_length);

/** SwState */
static shr_error_e dnx_port_nif_calendar_config(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *calendar,
    int calendar_length);
static shr_error_e dnx_port_nif_calendar_active_data_db_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 nof_calendar_clients,
    uint32 *calendar,
    uint32 calendar_length);
static shr_error_e dnx_port_nif_calendar_last_objects_db_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 nof_calendar_clients,
    uint32 **calendar_last_objects);
static shr_error_e dnx_port_nif_calendar_object_db_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 client_id,
    uint32 level,
    uint32 entry_index);
static shr_error_e dnx_port_nif_calendar_object_db_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 client_id,
    uint32 level,
    uint32 *entry_index);
static shr_error_e dnx_port_nif_calendar_nof_slots_per_client_db_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 client_id,
    uint32 nof_slots_per_client);
static shr_error_e dnx_port_nif_calendar_nof_slots_per_client_db_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 client_id,
    uint32 *nof_slots_per_client);
static shr_error_e dnx_port_nif_calendar_last_client_db_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 last_client_in_calendar);
static shr_error_e dnx_port_nif_calendar_last_client_db_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *last_client_in_calendar);

/** dbal */
static shr_error_e dnx_port_calendar_table_id_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    dbal_tables_e * table_id);
static shr_error_e dnx_port_calendar_table_bubble_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *bubble_val);
static shr_error_e dnx_port_calendar_config_table_id_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    dbal_tables_e * table_id);
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
dnx_port_shadow_calendar_build(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type)
{
    uint32 *calendar = NULL, calendar_length, nof_calendar_clients;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get calendar's length & nof_clients
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_length_get(unit, calendar_type, &calendar_length));
    SHR_IF_ERR_EXIT(dnx_port_calendar_nof_clients_get(unit, calendar_type, &nof_calendar_clients));

    /*
     * Allocating arrays memory 
     */
    SHR_ALLOC_SET_ZERO_ERR_EXIT(calendar, calendar_length * sizeof(uint32),
                                "Calendar to set slots per port", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Calculating new calendar 
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_build_only(unit, calendar_type, &calendar_length, calendar));

    /*
     * Setting new calendar to hw 
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_calendar_config(unit, calendar_type, calendar, calendar_length));

    /** Saving last objects to sw state */
    SHR_IF_ERR_EXIT(dnx_port_nif_calendar_active_data_db_set
                    (unit, calendar_type, nof_calendar_clients, calendar, calendar_length));

exit:
    SHR_FREE(calendar);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_calendar_build_only(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *calendar_length,
    uint32 *calendar)
{
    uint32 *nof_slots_per_port = NULL, nof_calendar_clients;
    bcm_pbmp_t *ports_tests = NULL, total_tests, check_old_cal;
    int direction, granularity, client_id, check_old_cal_count;
    dnx_port_nif_calendar_check_t tests_data[DNX_PORT_NIF_CALENDAR_CHECK_NOF] = { {{0}} };
    SHR_FUNC_INIT_VARS(unit);

    if (calendar_length == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar_length is not allocated.");
    }

    /*
     * Get calendar parameters for algo & set tests_data for checking calendar's distribution
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_calc_slots_parameters_get(unit, calendar_type, &direction, &granularity));
    SHR_IF_ERR_EXIT(dnx_port_calendar_nof_clients_get(unit, calendar_type, &nof_calendar_clients));

    _SHR_PBMP_CLEAR(check_old_cal);
    tests_data[DNX_PORT_NIF_CALENDAR_CHECK_SINGLE_DISTANCE].distance_data.deviation = 0.5;
    tests_data[DNX_PORT_NIF_CALENDAR_CHECK_SINGLE_DISTANCE].distance_data.level = 1;
    tests_data[DNX_PORT_NIF_CALENDAR_CHECK_DOUBLE_DISTANCE].distance_data.deviation = 0.8;
    tests_data[DNX_PORT_NIF_CALENDAR_CHECK_DOUBLE_DISTANCE].distance_data.level = 2;
    tests_data[DNX_PORT_NIF_CALENDAR_CHECK_TRIPLE_DISTANCE].distance_data.deviation = 1.0;
    tests_data[DNX_PORT_NIF_CALENDAR_CHECK_TRIPLE_DISTANCE].distance_data.level = 3;
    tests_data[DNX_PORT_NIF_CALENDAR_CHECK_WB_SINGLE_DISTANCE].distance_data.deviation = 0.05;
    tests_data[DNX_PORT_NIF_CALENDAR_CHECK_WB_SINGLE_DISTANCE].distance_data.level = 1;
    tests_data[DNX_PORT_NIF_CALENDAR_CHECK_MACSEC_DISTANCE].min_distance_data.limit_value = 1;
    tests_data[DNX_PORT_NIF_CALENDAR_CHECK_MACSEC_DISTANCE].min_distance_data.max_occurrences = 0;
    tests_data[DNX_PORT_NIF_CALENDAR_CHECK_MINIMAL_DISTANCE].min_distance_data.limit_value = 5;
    tests_data[DNX_PORT_NIF_CALENDAR_CHECK_MINIMAL_DISTANCE].min_distance_data.max_occurrences = 8;

    /*
     * Allocating arrays memory
     */
    SHR_ALLOC_SET_ZERO_ERR_EXIT(nof_slots_per_port, nof_calendar_clients * sizeof(uint32),
                                "Saves how many slots each port needs", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(ports_tests, nof_calendar_clients * sizeof(bcm_pbmp_t),
                                "Saves which tests will be tested each port needs", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_port_nif_slots_per_port_prepare
                    (unit, calendar_type, nof_slots_per_port, nof_calendar_clients, direction, granularity,
                     ports_tests, &total_tests, &check_old_cal, tests_data));

    _SHR_PBMP_COUNT(check_old_cal, check_old_cal_count);
    if (check_old_cal_count > 0)
    {
        for (client_id = 0; client_id < nof_calendar_clients; ++client_id)
        {
            uint32 nof_client_slots;
            /** If nof_slot (i.e. port's speed) was changed, we should not test this port for transition */
            SHR_IF_ERR_EXIT(dnx_port_nif_calendar_nof_slots_per_client_db_get
                            (unit, calendar_type, client_id, &nof_client_slots));
            if (nof_slots_per_port[client_id] != nof_client_slots)
            {
                _SHR_PBMP_PORT_REMOVE(check_old_cal, client_id);
            }
        }
    }

    SHR_IF_ERR_EXIT(dnx_port_nif_calendar_build_and_fix
                    (unit, calendar_type, nof_slots_per_port, nof_calendar_clients, calendar, calendar_length,
                     ports_tests, check_old_cal, total_tests, tests_data));

    SHR_IF_ERR_EXIT(dnx_port_nif_calendar_verify
                    (unit, calendar_type, nof_slots_per_port, nof_calendar_clients, calendar,
                     *calendar_length, ports_tests, check_old_cal, tests_data));

exit:
    SHR_FREE(ports_tests);
    SHR_FREE(nof_slots_per_port);
    SHR_FUNC_EXIT;
}

shr_error_e
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

shr_error_e
dnx_port_calendar_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * build shadow calendar first
     */
    SHR_IF_ERR_EXIT(dnx_port_shadow_calendar_build(unit, calendar_type));

    /*
     * switch active calendar in HW
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_switch_active_id(unit, calendar_type));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_nif_calendar_verify(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    int nof_calendar_clients,
    uint32 *calendar,
    uint32 calendar_length,
    bcm_pbmp_t * clients_tests,
    bcm_pbmp_t check_old_calendar,
    dnx_port_nif_calendar_check_t tests_data[DNX_PORT_NIF_CALENDAR_CHECK_NOF])
{
    bcm_port_t test;
    bcm_pbmp_t min_distance_clients;
    int client_id, nof_slots, passed_cyclic = TRUE, passed_transition =
        TRUE, g_hao_client, check_old_cal_for_min_distance = FALSE, min_distance_clients_count;
    uint32 **active_calendar_last_objects = NULL;
    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_CLEAR(min_distance_clients);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(active_calendar_last_objects, sizeof(uint32 *) * nof_calendar_clients,
                                "active_calendar_last_objects", "%s%s%s", EMPTY, EMPTY, EMPTY);
    for (client_id = 0; client_id < nof_calendar_clients; ++client_id)
    {
        SHR_ALLOC_SET_ZERO_ERR_EXIT(active_calendar_last_objects[client_id],
                                    sizeof(uint32) * DNX_ALGO_CAL_MAX_LEVEL_DISTANCE_CHECK,
                                    "active_calendar_last_objects[client_id]", "%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SHR_IF_ERR_EXIT(dnx_port_nif_calendar_last_objects_db_get
                    (unit, calendar_type, nof_calendar_clients, active_calendar_last_objects));
    SHR_IF_ERR_EXIT(dnx_port_nif_db.g_hao_active_client.get(unit, &g_hao_client));
    for (client_id = 0; client_id < nof_calendar_clients; ++client_id)
    {
        nof_slots = nof_slots_per_port[client_id];
        if (nof_slots_per_port[client_id] > 0)
        {
            _SHR_PBMP_ITER(clients_tests[client_id], test)
            {
                switch (test)
                {
                    case DNX_PORT_NIF_CALENDAR_CHECK_TRIPLE_DISTANCE:
                        SHR_IF_ERR_EXIT(dnx_algo_cal_triple_distance_cyclic_check
                                        (unit, client_id, nof_slots, tests_data[test].distance_data.deviation,
                                         calendar, calendar_length, &passed_cyclic));
                        if (_SHR_PBMP_MEMBER(check_old_calendar, client_id) && g_hao_client != client_id)
                        {
                            SHR_IF_ERR_EXIT(dnx_algo_cal_distance_transition_check
                                            (unit, client_id, nof_slots, tests_data[test].distance_data.level,
                                             tests_data[test].distance_data.deviation, calendar,
                                             active_calendar_last_objects[client_id], calendar_length,
                                             &passed_transition));
                        }
                        if (passed_cyclic == FALSE || passed_transition == FALSE)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                         "Building calendar failed. Calendar's is not meeting the triple distance validation: Cyclic test %s. Transform test %s.",
                                         (passed_cyclic == FALSE) ? "failed" : "passed",
                                         (passed_transition == FALSE) ? "failed" : "passed");
                        }
                        break;

                    case DNX_PORT_NIF_CALENDAR_CHECK_DOUBLE_DISTANCE:
                        SHR_IF_ERR_EXIT(dnx_algo_cal_double_distance_cyclic_check
                                        (unit, client_id, nof_slots, tests_data[test].distance_data.deviation,
                                         calendar, calendar_length, &passed_cyclic));
                        if (_SHR_PBMP_MEMBER(check_old_calendar, client_id) && g_hao_client != client_id)
                        {
                            SHR_IF_ERR_EXIT(dnx_algo_cal_distance_transition_check
                                            (unit, client_id, nof_slots, tests_data[test].distance_data.level,
                                             tests_data[test].distance_data.deviation, calendar,
                                             active_calendar_last_objects[client_id], calendar_length,
                                             &passed_transition));
                        }
                        if (passed_cyclic == FALSE || passed_transition == FALSE)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                         "Building calendar failed. Calendar's distribution is not meeting the double distance validation: Cyclic test %s. Transform test %s.",
                                         (passed_cyclic == FALSE) ? "failed" : "passed",
                                         (passed_transition == FALSE) ? "failed" : "passed");
                        }
                        break;

                    case DNX_PORT_NIF_CALENDAR_CHECK_SINGLE_DISTANCE:
                    case DNX_PORT_NIF_CALENDAR_CHECK_WB_SINGLE_DISTANCE:
                        SHR_IF_ERR_EXIT(dnx_algo_cal_distance_cyclic_check
                                        (unit, client_id, nof_slots, tests_data[test].distance_data.deviation,
                                         calendar, calendar_length, &passed_cyclic));
                        if (_SHR_PBMP_MEMBER(check_old_calendar, client_id) && g_hao_client != client_id)
                        {
                            SHR_IF_ERR_EXIT(dnx_algo_cal_distance_transition_check
                                            (unit, client_id, nof_slots, tests_data[test].distance_data.level,
                                             tests_data[test].distance_data.deviation, calendar,
                                             active_calendar_last_objects[client_id], calendar_length,
                                             &passed_transition));
                        }
                        if (passed_cyclic == FALSE || passed_transition == FALSE)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                         "Building calendar failed. Calendar's distribution is not meeting the single distance validation: Cyclic test %s. Transform test %s.",
                                         (passed_cyclic == FALSE) ? "failed" : "passed",
                                         (passed_transition == FALSE) ? "failed" : "passed");
                        }
                        break;

                    case DNX_PORT_NIF_CALENDAR_CHECK_MACSEC_DISTANCE:
                        SHR_IF_ERR_EXIT(dnx_algo_cal_consecutive_cyclic_check
                                        (unit, client_id, nof_slots, calendar, calendar_length, &passed_cyclic));

                        if (passed_cyclic == FALSE)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                         "Building calendar failed. Calendar's distribution is not meeting the MacSec cyclic limitation.");
                        }
                        break;

                    case DNX_PORT_NIF_CALENDAR_CHECK_MINIMAL_DISTANCE:
                        check_old_cal_for_min_distance |= _SHR_PBMP_MEMBER(check_old_calendar, client_id);
                        _SHR_PBMP_PORT_ADD(min_distance_clients, client_id);
                        break;

                    default:
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Calendar's test %d is not supported.", test);
                }
            }
        }
    }

    /** After going per port, doing general calendar tests */
    if (calendar[0] != DNX_ALGO_CAL_ILLEGAL_OBJECT_ID
        && _SHR_PBMP_MEMBER(clients_tests[calendar[0]], DNX_PORT_NIF_CALENDAR_CHECK_MACSEC_DISTANCE)
        && _SHR_PBMP_MEMBER(check_old_calendar, calendar[0]) && calendar[0] != g_hao_client)
    {
        uint32 old_calendar_last_object;
        SHR_IF_ERR_EXIT(dnx_port_nif_calendar_last_client_db_get(unit, calendar_type, &old_calendar_last_object));

        if (old_calendar_last_object == calendar[0])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Building calendar failed. Calendar's distribution is not meeting the MacSec transition limitation.");
        }
    }

#ifdef INCLUDE_XFLOW_MACSEC
    if (DNX_PORT_NIF_PORT_TYPE_IS_MACSEC(calendar_type))
    {
        int passed = TRUE;
        SHR_IF_ERR_EXIT(dnx_xflow_macsec_shared_macsec_calendar_check(unit, calendar, calendar_length, &passed));
        if (passed == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Building calendar failed. Test is not meeting general Macsec rules.");
        }
    }
#endif

    _SHR_PBMP_COUNT(min_distance_clients, min_distance_clients_count);
    if (min_distance_clients_count > 0)
    {
        if (check_old_cal_for_min_distance)
        {
            SHR_IF_ERR_EXIT(dnx_algo_cal_min_distance_transition_check
                            (unit, min_distance_clients, calendar, active_calendar_last_objects, calendar_length,
                             tests_data[DNX_PORT_NIF_CALENDAR_CHECK_MINIMAL_DISTANCE].min_distance_data.limit_value,
                             tests_data[DNX_PORT_NIF_CALENDAR_CHECK_MINIMAL_DISTANCE].min_distance_data.max_occurrences,
                             &passed_transition));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_cal_min_distance_cyclic_check
                            (unit, min_distance_clients, calendar, calendar_length,
                             tests_data[DNX_PORT_NIF_CALENDAR_CHECK_MINIMAL_DISTANCE].min_distance_data.limit_value,
                             tests_data[DNX_PORT_NIF_CALENDAR_CHECK_MINIMAL_DISTANCE].min_distance_data.max_occurrences,
                             &passed_cyclic));
        }

        if (passed_cyclic == FALSE || passed_transition == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Building calendar failed. Minimal distance exceed its maximal occurrences.: Cyclic test %s. Transform test %s.",
                         (passed_cyclic == FALSE) ? "failed" : "passed",
                         (passed_transition == FALSE) ? "failed" : "passed");
        }
    }

exit:
    if (active_calendar_last_objects)
    {
        for (client_id = 0; client_id < nof_calendar_clients; ++client_id)
        {
            SHR_FREE(active_calendar_last_objects[client_id]);
        }
        SHR_FREE(active_calendar_last_objects);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_calendar_length_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *calendar_length)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (calendar_type)
    {
        case DNX_PORT_NIF_ARB_RX_QPM_CALENDAR:
            *calendar_length = dnx_data_nif.arb.rx_qpm_calendar_length_get(unit);
            break;

        case DNX_PORT_NIF_ARB_RX_SCH_CALENDAR:
            *calendar_length = dnx_data_nif.arb.rx_sch_calendar_length_get(unit);
            break;

        case DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM1_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM2_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM3_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM4_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM5_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM6_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM7_CALENDAR:
            *calendar_length = dnx_data_nif.arb.cdpm_calendar_length_get(unit);
            break;

        case DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR:
            *calendar_length = dnx_data_nif.arb.tmac_calendar_length_get(unit);
            break;

        case DNX_PORT_NIF_OFT_CALENDAR:
            *calendar_length = dnx_data_nif.oft.calendar_length_get(unit);
            break;

        case DNX_PORT_NIF_FRAMER_SB_RX_CALENDAR:
        case DNX_PORT_NIF_FRAMER_SB_TX_CALENDAR:
            *calendar_length = dnx_data_nif.mac_client.nof_sb_cal_slots_get(unit);
            break;

        case DNX_PORT_NIF_FLEXE_PHY_CALENDAR:
            *calendar_length = dnx_data_nif.flexe.nb_cal_nof_slots_get(unit);
            break;

#ifdef INCLUDE_XFLOW_MACSEC
        case DNX_PORT_NIF_MACSEC0_CALENDAR:
        case DNX_PORT_NIF_MACSEC1_CALENDAR:
            *calendar_length = dnx_data_macsec.general.macsec_tdm_cal_max_depth_get(unit);
            break;
#endif

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar type %d is not supported.", calendar_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_nif_calendars_db_init(
    int unit)
{
    uint32 nof_calendar_clients;
    dnx_port_nif_calendar_type_e calendar_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_nif_db.active_calendars.alloc(unit, DNX_PORT_NIF_CALENDAR_NOF));

    for (calendar_type = DNX_PORT_NIF_CALENDAR_FIRST; calendar_type < DNX_PORT_NIF_CALENDAR_NOF; ++calendar_type)
    {
        SHR_IF_ERR_EXIT(dnx_port_calendar_nof_clients_get(unit, calendar_type, &nof_calendar_clients));

        if (nof_calendar_clients)
        {
            SHR_IF_ERR_EXIT(dnx_port_nif_db.active_calendars.client_last_objects.alloc
                            (unit, calendar_type, DNX_ALGO_CAL_MAX_LEVEL_DISTANCE_CHECK, nof_calendar_clients));
            SHR_IF_ERR_EXIT(dnx_port_nif_db.active_calendars.
                            nof_client_slots.alloc(unit, calendar_type, nof_calendar_clients));
        }
    }

exit:
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

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_calendar_active_data_db_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 nof_calendar_clients,
    uint32 *calendar,
    uint32 calendar_length)
{
    int entry_index, level;
    uint32 client_id, *client_id_objects_counter = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(client_id_objects_counter, sizeof(uint32) * nof_calendar_clients,
                                "client_id objects counter", "%s%s%s", EMPTY, EMPTY, EMPTY);

    for (entry_index = calendar_length - 1; entry_index >= 0; --entry_index)
    {
        client_id = calendar[entry_index];
        if (client_id != DNX_ALGO_CAL_ILLEGAL_OBJECT_ID)
        {
            level = client_id_objects_counter[client_id];
            if (client_id_objects_counter[client_id] < DNX_ALGO_CAL_MAX_LEVEL_DISTANCE_CHECK)
            {
                SHR_IF_ERR_EXIT(dnx_port_nif_calendar_object_db_set
                                (unit, calendar_type, client_id, level, entry_index));
            }

            ++client_id_objects_counter[client_id];
        }
    }

    for (client_id = 0; client_id < nof_calendar_clients; ++client_id)
    {
        SHR_IF_ERR_EXIT(dnx_port_nif_calendar_nof_slots_per_client_db_set(unit, calendar_type, client_id,
                                                                          client_id_objects_counter[client_id]));
    }

    SHR_IF_ERR_EXIT(dnx_port_nif_calendar_last_client_db_set(unit, calendar_type, calendar[calendar_length - 1]));

exit:
    SHR_FREE(client_id_objects_counter);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_calendar_last_objects_db_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 nof_calendar_clients,
    uint32 **calendar_last_objects)
{
    uint32 client_id, level;
    SHR_FUNC_INIT_VARS(unit);

    if (calendar_last_objects == NULL || *calendar_last_objects == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid calendar_last_objects, memory wasn't allocated.");
    }

    for (client_id = 0; client_id < nof_calendar_clients; ++client_id)
    {
        for (level = 0; level < DNX_ALGO_CAL_MAX_LEVEL_DISTANCE_CHECK; ++level)
        {
            SHR_IF_ERR_EXIT(dnx_port_nif_calendar_object_db_get
                            (unit, calendar_type, client_id, level, &(calendar_last_objects[client_id][level])));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_calendar_object_db_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 client_id,
    uint32 level,
    uint32 entry_index)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_nif_db.active_calendars.
                    client_last_objects.set(unit, calendar_type, level, client_id, entry_index));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_calendar_object_db_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 client_id,
    uint32 level,
    uint32 *entry_index)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_nif_db.active_calendars.
                    client_last_objects.get(unit, calendar_type, level, client_id, entry_index));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_calendar_nof_slots_per_client_db_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 client_id,
    uint32 nof_slots_per_client)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_nif_db.active_calendars.
                    nof_client_slots.set(unit, calendar_type, client_id, nof_slots_per_client));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_calendar_nof_slots_per_client_db_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 client_id,
    uint32 *nof_slots_per_client)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_nif_db.active_calendars.
                    nof_client_slots.get(unit, calendar_type, client_id, nof_slots_per_client));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_calendar_last_client_db_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 last_client_in_calendar)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_nif_db.active_calendars.last_client.set(unit, calendar_type, last_client_in_calendar));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_calendar_last_client_db_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *last_client_in_calendar)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_nif_db.active_calendars.last_client.get(unit, calendar_type, last_client_in_calendar));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_calendar_calc_slots_parameters_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    int *direction,
    int *granularity)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (calendar_type)
    {
        case DNX_PORT_NIF_ARB_RX_QPM_CALENDAR:
            *direction = DNX_ALGO_PORT_SPEED_F_RX;
            *granularity = dnx_data_nif.arb.rx_qpm_port_speed_granularity_get(unit);
            break;

        case DNX_PORT_NIF_ARB_RX_SCH_CALENDAR:
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
            *direction = DNX_ALGO_PORT_SPEED_F_TX;
            *granularity = dnx_data_nif.arb.tx_eth_port_speed_granularity_get(unit);
            break;

        case DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR:
            *direction = DNX_ALGO_PORT_SPEED_F_TX;
            *granularity = dnx_data_nif.arb.tx_tmac_port_speed_granularity_get(unit);
            break;

        case DNX_PORT_NIF_OFT_CALENDAR:
            *direction = DNX_ALGO_PORT_SPEED_F_TX;
            *granularity = dnx_data_nif.oft.calendar_speed_granularity_get(unit);
            break;

        case DNX_PORT_NIF_FRAMER_SB_RX_CALENDAR:
        case DNX_PORT_NIF_FRAMER_SB_TX_CALENDAR:
            *direction =
                (calendar_type ==
                 DNX_PORT_NIF_FRAMER_SB_TX_CALENDAR) ? DNX_ALGO_PORT_SPEED_F_TX : DNX_ALGO_PORT_SPEED_F_RX;
            *granularity = dnx_data_nif.mac_client.sb_cal_granularity_get(unit);
            break;

        case DNX_PORT_NIF_FLEXE_PHY_CALENDAR:
            *direction = 0;
            *granularity = dnx_data_nif.flexe.nb_cal_granularity_get(unit);
            break;

#ifdef INCLUDE_XFLOW_MACSEC
        case DNX_PORT_NIF_MACSEC0_CALENDAR:
        case DNX_PORT_NIF_MACSEC1_CALENDAR:
            *direction = 0;
            *granularity = dnx_data_macsec.wrapper.tdm_calendar_speed_granularity_get(unit);
            break;
#endif

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar type %d is not supported.", calendar_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_calendar_nof_clients_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_calendar_clients)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (calendar_type)
    {
        case DNX_PORT_NIF_ARB_RX_QPM_CALENDAR:
            *nof_calendar_clients = dnx_data_nif.phys.nof_phys_get(unit);
            break;

        case DNX_PORT_NIF_ARB_RX_SCH_CALENDAR:
            *nof_calendar_clients = dnx_data_nif.arb.rx_sch_calendar_nof_modes_get(unit);
            break;

        case DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM1_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM2_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM3_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM4_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM5_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM6_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM7_CALENDAR:
            *nof_calendar_clients = dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);
            break;

        case DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR:
            *nof_calendar_clients = dnx_data_nif.mac_client.nof_normal_clients_get(unit);
            break;

        case DNX_PORT_NIF_OFT_CALENDAR:
            *nof_calendar_clients = dnx_data_nif.oft.nof_contexts_get(unit);
            break;

        case DNX_PORT_NIF_FRAMER_SB_RX_CALENDAR:
        case DNX_PORT_NIF_FRAMER_SB_TX_CALENDAR:
            *nof_calendar_clients = dnx_data_nif.mac_client.nof_normal_clients_get(unit);
            break;

        case DNX_PORT_NIF_FLEXE_PHY_CALENDAR:
            *nof_calendar_clients = dnx_data_nif.flexe.nof_flexe_core_ports_get(unit);
            break;

        case DNX_PORT_NIF_MACSEC0_CALENDAR:
        case DNX_PORT_NIF_MACSEC1_CALENDAR:
#ifdef INCLUDE_XFLOW_MACSEC
            *nof_calendar_clients = dnx_data_macsec.general.ports_in_macsec_nof_get(unit);
#else
            *nof_calendar_clients = 0;
#endif
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar type %d is not supported.", calendar_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_calendar_client_id_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    bcm_port_t logical_port,
    int *client_id)
{
    int core;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    switch (calendar_type)
    {
        case DNX_PORT_NIF_OFT_CALENDAR:
        case DNX_PORT_NIF_ARB_RX_QPM_CALENDAR:
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, logical_port, 0, &core, client_id));
            break;

        case DNX_PORT_NIF_MACSEC0_CALENDAR:
        case DNX_PORT_NIF_MACSEC1_CALENDAR:
#ifdef INCLUDE_XFLOW_MACSEC
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, logical_port, 0, &core, client_id));
            (*client_id) %= dnx_data_macsec.general.ports_in_macsec_nof_get(unit);
#endif
            break;

        case DNX_PORT_NIF_ARB_RX_SCH_CALENDAR:
            SHR_IF_ERR_EXIT(dnx_port_nif_scheduler_mode_get(unit, logical_port, client_id));
            break;

        case DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM1_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM2_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM3_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM4_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM5_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM6_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM7_CALENDAR:
            SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, logical_port, &ethu_info));
            *client_id = ethu_info.first_lane_in_port;
            break;

        case DNX_PORT_NIF_FLEXE_PHY_CALENDAR:
            SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, logical_port, client_id));
            break;

        case DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR:
        case DNX_PORT_NIF_FRAMER_SB_RX_CALENDAR:
        case DNX_PORT_NIF_FRAMER_SB_TX_CALENDAR:
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, logical_port, client_id));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar type %d is not supported.", calendar_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef INCLUDE_XFLOW_MACSEC
static shr_error_e
dnx_port_nif_macsec_calendar_tests_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    bcm_pbmp_t all_ports,
    bcm_pbmp_t * ports_tests,
    bcm_pbmp_t * total_tests)
{
    bcm_port_t logical_port;
    int client_id, is_bypass;
    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(all_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_xflow_macsec_lport_is_bypassed_get(unit, logical_port, &is_bypass));
        if (!is_bypass)
        {
            SHR_IF_ERR_EXIT(dnx_port_nif_calendar_client_id_get(unit, calendar_type, logical_port, &client_id));
            _SHR_PBMP_PORT_ADD(ports_tests[client_id], DNX_PORT_NIF_CALENDAR_CHECK_MACSEC_DISTANCE);
            _SHR_PBMP_PORT_ADD(*total_tests, DNX_PORT_NIF_CALENDAR_CHECK_MACSEC_DISTANCE);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

static shr_error_e
dnx_port_nif_framer_sb_tx_calendar_tests_get(
    int unit,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
    bcm_pbmp_t * ports_tests,
    bcm_pbmp_t * total_tests,
    bcm_pbmp_t * check_old_cal,
    dnx_port_nif_calendar_check_t tests_data[DNX_PORT_NIF_CALENDAR_CHECK_NOF])
{
    bcm_port_t logical_port;
    int client_id, speed, has_speed, is_enable;
    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(all_ports, logical_port)
    {
        speed = 0;
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_port_nif_calendar_client_id_get
                            (unit, DNX_PORT_NIF_FRAMER_SB_TX_CALENDAR, logical_port, &client_id));
            SHR_IF_ERR_EXIT(bcm_port_enable_get(unit, logical_port, &is_enable));
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &speed));

            _SHR_PBMP_PORT_ADD(ports_tests[client_id], DNX_PORT_NIF_CALENDAR_CHECK_DOUBLE_DISTANCE);
            _SHR_PBMP_PORT_ADD(*total_tests, DNX_PORT_NIF_CALENDAR_CHECK_DOUBLE_DISTANCE);

            if (speed >= 1000 && speed <= 5000 && nof_slots_per_port[client_id] >
                tests_data[DNX_PORT_NIF_CALENDAR_CHECK_WB_SINGLE_DISTANCE].distance_data.level)
            {
                _SHR_PBMP_PORT_ADD(ports_tests[client_id], DNX_PORT_NIF_CALENDAR_CHECK_WB_SINGLE_DISTANCE);
                _SHR_PBMP_PORT_ADD(*total_tests, DNX_PORT_NIF_CALENDAR_CHECK_WB_SINGLE_DISTANCE);
            }

            if (speed < 200000 && nof_slots_per_port[client_id] >= 2)
            {
                _SHR_PBMP_PORT_ADD(ports_tests[client_id], DNX_PORT_NIF_CALENDAR_CHECK_MINIMAL_DISTANCE);
                _SHR_PBMP_PORT_ADD(*total_tests, DNX_PORT_NIF_CALENDAR_CHECK_MINIMAL_DISTANCE);
            }

            if (is_enable)
            {
                _SHR_PBMP_PORT_ADD(*check_old_cal, client_id);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_general_calendar_tests_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    int direction,
    bcm_pbmp_t all_ports,
    bcm_pbmp_t * ports_tests,
    bcm_pbmp_t * total_tests,
    bcm_pbmp_t * check_old_cal,
    dnx_port_nif_calendar_check_t tests_data[DNX_PORT_NIF_CALENDAR_CHECK_NOF])
{
    bcm_port_t logical_port;
    int client_id, speed, has_speed, is_enable;
    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(all_ports, logical_port)
    {
        speed = 0;
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, direction, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_port_nif_calendar_client_id_get(unit, calendar_type, logical_port, &client_id));
            SHR_IF_ERR_EXIT(bcm_port_enable_get(unit, logical_port, &is_enable));
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, direction, &speed));

#ifdef INCLUDE_XFLOW_MACSEC
            if (dnx_data_macsec.general.is_macsec_enabled_get(unit)
                && (calendar_type == DNX_PORT_NIF_ARB_RX_SCH_CALENDAR || calendar_type == DNX_PORT_NIF_OFT_CALENDAR))
            {
                int is_bypass;
                SHR_IF_ERR_EXIT(dnx_xflow_macsec_lport_is_bypassed_get(unit, logical_port, &is_bypass));
                if (!is_bypass)
                {
                    _SHR_PBMP_PORT_ADD(ports_tests[client_id], DNX_PORT_NIF_CALENDAR_CHECK_MACSEC_DISTANCE);
                    _SHR_PBMP_PORT_ADD(*total_tests, DNX_PORT_NIF_CALENDAR_CHECK_MACSEC_DISTANCE);
                }
            }
#endif
            if (nof_slots_per_port[client_id] >
                tests_data[DNX_PORT_NIF_CALENDAR_CHECK_TRIPLE_DISTANCE].distance_data.level)
            {
                _SHR_PBMP_PORT_ADD(ports_tests[client_id], DNX_PORT_NIF_CALENDAR_CHECK_TRIPLE_DISTANCE);
                _SHR_PBMP_PORT_ADD(*total_tests, DNX_PORT_NIF_CALENDAR_CHECK_TRIPLE_DISTANCE);
            }
            if (speed <= 100000
                && nof_slots_per_port[client_id] >
                tests_data[DNX_PORT_NIF_CALENDAR_CHECK_DOUBLE_DISTANCE].distance_data.level)
            {
                _SHR_PBMP_PORT_ADD(ports_tests[client_id], DNX_PORT_NIF_CALENDAR_CHECK_DOUBLE_DISTANCE);
                _SHR_PBMP_PORT_ADD(*total_tests, DNX_PORT_NIF_CALENDAR_CHECK_DOUBLE_DISTANCE);
            }

            if (speed <= 5000
                && nof_slots_per_port[client_id] >
                tests_data[DNX_PORT_NIF_CALENDAR_CHECK_SINGLE_DISTANCE].distance_data.level)
            {
                _SHR_PBMP_PORT_ADD(ports_tests[client_id], DNX_PORT_NIF_CALENDAR_CHECK_SINGLE_DISTANCE);
                _SHR_PBMP_PORT_ADD(*total_tests, DNX_PORT_NIF_CALENDAR_CHECK_SINGLE_DISTANCE);
            }

            if (is_enable)
            {
                _SHR_PBMP_PORT_ADD(*check_old_cal, client_id);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_slots_per_port_prepare(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    int nof_calendar_clients,
    int direction,
    int granularity,
    bcm_pbmp_t * ports_tests,
    bcm_pbmp_t * total_tests,
    bcm_pbmp_t * check_old_cal,
    dnx_port_nif_calendar_check_t tests_data[DNX_PORT_NIF_CALENDAR_CHECK_NOF])
{
    int client_id;
    bcm_pbmp_t all_ports, all_eth_ports, all_framer_ports, phy_ports;
    bcm_pbmp_t all_stat_ports, all_protection_ports;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get all logical ports
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH,
                     DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_L1 |
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_FRAMER_MGMT | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT |
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_FEEDBACK, &all_eth_ports));
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FRAMER_MAC,
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_L1 | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT |
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_FLEXE_MAC_STAT_ONLY |
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_FLEXE_MAC_PROTECTION, &all_framer_ports));
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FRAMER_PHY,
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT, &phy_ports));
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FRAMER_MAC_STAT,
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT, &all_stat_ports));
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FRAMER_MAC_PROTECTION,
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT, &all_protection_ports));

    /*
     * Clearing all bitmaps 
     */
    _SHR_PBMP_CLEAR(*check_old_cal);
    _SHR_PBMP_CLEAR(*total_tests);
    _SHR_PBMP_CLEAR(all_ports);
    sal_memset(nof_slots_per_port, 0, sizeof(uint32) * nof_calendar_clients);
    for (client_id = 0; client_id < nof_calendar_clients; ++client_id)
    {
        _SHR_PBMP_CLEAR(ports_tests[client_id]);
    }
    switch (calendar_type)
    {
        case DNX_PORT_NIF_MACSEC0_CALENDAR:
        case DNX_PORT_NIF_MACSEC1_CALENDAR:
#ifdef INCLUDE_XFLOW_MACSEC
            if (dnx_data_macsec.general.feature_get(unit, dnx_data_macsec_general_is_shared_macsec_supported))
            {
                SHR_IF_ERR_EXIT(dnx_xflow_macsec_shared_macsec_all_ports_get
                                (unit, DNX_PORT_NIF_PORT_MACSEC_ID_GET(calendar_type), nof_calendar_clients,
                                 &all_ports));
                SHR_IF_ERR_EXIT(dnx_xflow_macsec_shared_macsec_calendar_slots_per_port_build
                                (unit, DNX_PORT_NIF_PORT_MACSEC_ID_GET(calendar_type), all_ports, granularity,
                                 nof_slots_per_port));
                SHR_IF_ERR_EXIT(dnx_port_nif_macsec_calendar_tests_get
                                (unit, calendar_type, all_ports, ports_tests, total_tests));
            }
            else
#endif
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Shared MACSec is required.");
            }
            break;

        case DNX_PORT_NIF_ARB_RX_SCH_CALENDAR:
            _SHR_PBMP_OR(all_ports, all_eth_ports);
            _SHR_PBMP_OR(all_ports, all_framer_ports);
            SHR_IF_ERR_EXIT(dnx_port_nif_rx_scheduler_calendar_slots_per_port_build
                            (unit, nof_slots_per_port, all_ports, granularity));
            SHR_IF_ERR_EXIT(dnx_port_nif_general_calendar_tests_get(unit, calendar_type, nof_slots_per_port,
                                                                    direction, all_ports, ports_tests, total_tests,
                                                                    check_old_cal, tests_data));
            break;

        case DNX_PORT_NIF_OFT_CALENDAR:
            _SHR_PBMP_OR(all_ports, all_framer_ports);
            _SHR_PBMP_OR(all_ports, all_eth_ports);
            SHR_IF_ERR_EXIT(dnx_port_nif_unified_calendar_slots_per_port_build
                            (unit, nof_slots_per_port, all_ports, direction, granularity));
            SHR_IF_ERR_EXIT(dnx_port_nif_general_calendar_tests_get(unit, calendar_type, nof_slots_per_port,
                                                                    direction, all_ports, ports_tests, total_tests,
                                                                    check_old_cal, tests_data));
            break;

        case DNX_PORT_NIF_ARB_RX_QPM_CALENDAR:
            _SHR_PBMP_OR(all_ports, all_eth_ports);
            SHR_IF_ERR_EXIT(dnx_port_nif_unified_calendar_slots_per_port_build
                            (unit, nof_slots_per_port, all_ports, direction, granularity));
            SHR_IF_ERR_EXIT(dnx_port_nif_general_calendar_tests_get(unit, calendar_type, nof_slots_per_port,
                                                                    direction, all_ports, ports_tests, total_tests,
                                                                    check_old_cal, tests_data));
            break;

        case DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM1_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM2_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM3_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM4_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM5_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM6_CALENDAR:
        case DNX_PORT_NIF_ARB_TX_PPM7_CALENDAR:
            _SHR_PBMP_OR(all_ports, all_eth_ports);
            SHR_IF_ERR_EXIT(dnx_port_nif_arb_tx_eth_slots_per_port_build
                            (unit, calendar_type, nof_slots_per_port, all_ports, granularity));
            SHR_IF_ERR_EXIT(dnx_port_nif_general_calendar_tests_get(unit, calendar_type, nof_slots_per_port,
                                                                    direction, all_ports, ports_tests, total_tests,
                                                                    check_old_cal, tests_data));
            break;

        case DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR:
            _SHR_PBMP_OR(all_ports, all_framer_ports);
            SHR_IF_ERR_EXIT(dnx_port_nif_arb_tx_tmac_slots_per_port_build
                            (unit, nof_slots_per_port, all_ports, granularity));
            SHR_IF_ERR_EXIT(dnx_port_nif_general_calendar_tests_get(unit, calendar_type, nof_slots_per_port,
                                                                    direction, all_ports, ports_tests, total_tests,
                                                                    check_old_cal, tests_data));
            break;

        case DNX_PORT_NIF_FRAMER_SB_RX_CALENDAR:
            _SHR_PBMP_OR(all_ports, all_framer_ports);
            _SHR_PBMP_OR(all_ports, all_stat_ports);
            _SHR_PBMP_OR(all_ports, all_protection_ports);
            SHR_IF_ERR_EXIT(dnx_port_nif_framer_sb_slots_per_port_build
                            (unit, nof_slots_per_port, all_ports, direction, granularity));
            SHR_IF_ERR_EXIT(dnx_port_nif_general_calendar_tests_get(unit, calendar_type, nof_slots_per_port,
                                                                    direction, all_ports, ports_tests, total_tests,
                                                                    check_old_cal, tests_data));
            break;

        case DNX_PORT_NIF_FRAMER_SB_TX_CALENDAR:
            _SHR_PBMP_OR(all_ports, all_framer_ports);
            _SHR_PBMP_OR(all_ports, all_stat_ports);
            _SHR_PBMP_OR(all_ports, all_protection_ports);
            SHR_IF_ERR_EXIT(dnx_port_nif_framer_sb_slots_per_port_build
                            (unit, nof_slots_per_port, all_ports, direction, granularity));
            SHR_IF_ERR_EXIT(dnx_port_nif_framer_sb_tx_calendar_tests_get
                            (unit, nof_slots_per_port, all_ports, ports_tests, total_tests, check_old_cal, tests_data));

            break;

        case DNX_PORT_NIF_FLEXE_PHY_CALENDAR:
            _SHR_PBMP_OR(all_ports, phy_ports);
            SHR_IF_ERR_EXIT(dnx_port_nif_feu_phy_slots_per_port_build
                            (unit, nof_slots_per_port, all_ports, direction, granularity));
            SHR_IF_ERR_EXIT(dnx_port_nif_general_calendar_tests_get(unit, calendar_type, nof_slots_per_port,
                                                                    direction, all_ports, ports_tests, total_tests,
                                                                    check_old_cal, tests_data));
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
dnx_port_nif_rx_scheduler_calendar_slots_per_port_build(
    int unit,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
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
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &speed));
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
     * Iterate through ports
     */
    _SHR_PBMP_ITER(all_ports, logical_port)
    {
        speed = 0;
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, direction, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, direction, &speed));
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
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &speed));
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
dnx_port_nif_feu_phy_slots_per_port_build(
    int unit,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
    int direction,
    int granularity)
{
    bcm_port_t logical_port, flexe_core_port;
    int speed, has_speed;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterate through tx ports
     */
    BCM_PBMP_ITER(all_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, direction, &has_speed));
        if (!has_speed)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, direction, &speed));

        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, logical_port, &flexe_core_port));

        nof_slots_per_port[flexe_core_port] = speed / granularity;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_framer_sb_slots_per_port_build(
    int unit,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
    int direction,
    int granularity)
{
    bcm_port_t logical_port;
    int speed, client_channel, has_speed;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterate through tx ports
     */
    _SHR_PBMP_ITER(all_ports, logical_port)
    {
        speed = 0;
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, direction, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, direction, &speed));
        }

        if (!speed)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, logical_port, &client_channel));

        nof_slots_per_port[client_channel] = UTILEX_MAX((speed / granularity), 1);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_arb_tx_tmac_slots_per_port_build(
    int unit,
    uint32 *nof_slots_per_port,
    bcm_pbmp_t all_ports,
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
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &speed));
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
dnx_port_nif_calendar_dist_calc(
    int unit,
    uint32 *calendar,
    int calendar_length,
    int nof_clients,
    bcm_pbmp_t * ports_tests,
    bcm_pbmp_t clients_tests,
    dnx_port_nif_calendar_check_t tests_data[DNX_PORT_NIF_CALENDAR_CHECK_NOF],
    dnx_algo_cal_detailed_entry_t * cal_dist_info)
{
    bcm_port_t test;
    int client, current_test_index = 0, *should_check_client = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(should_check_client, sizeof(int) * nof_clients,
                                "should_check_client", "%s%s%s", EMPTY, EMPTY, EMPTY);
    _SHR_PBMP_ITER(clients_tests, test)
    {
        for (client = 0; client < nof_clients; ++client)
        {
            should_check_client[client] = _SHR_PBMP_MEMBER(ports_tests[client], test);
        }

        switch (test)
        {
            case DNX_PORT_NIF_CALENDAR_CHECK_SINGLE_DISTANCE:
            case DNX_PORT_NIF_CALENDAR_CHECK_DOUBLE_DISTANCE:
            case DNX_PORT_NIF_CALENDAR_CHECK_TRIPLE_DISTANCE:
            case DNX_PORT_NIF_CALENDAR_CHECK_WB_SINGLE_DISTANCE:
                SHR_IF_ERR_EXIT(algo_cal_dist_calc
                                (unit, nof_clients, calendar_length, calendar,
                                 tests_data[test].distance_data.level, tests_data[test].distance_data.deviation, 0,
                                 FALSE, should_check_client, &(cal_dist_info[current_test_index * calendar_length])));
                break;
            case DNX_PORT_NIF_CALENDAR_CHECK_MINIMAL_DISTANCE:
            case DNX_PORT_NIF_CALENDAR_CHECK_MACSEC_DISTANCE:
                /*
                 * Macsec & minimal distance test don't have deviation & level data so they are using single distance's 
                 * data 
                 */
                SHR_IF_ERR_EXIT(algo_cal_dist_calc
                                (unit, nof_clients, calendar_length, calendar, 1,
                                 tests_data[test].min_distance_data.limit_value, 0, TRUE,
                                 should_check_client, &(cal_dist_info[current_test_index * calendar_length])));
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Calendar's test %d is not supported.", test);
        }
        ++current_test_index;
    }

exit:
    SHR_FREE(should_check_client);
    SHR_FUNC_EXIT;
}

int
dnx_port_nif_sb_tx_calendar_slots_cmp(
    void *a,
    void *b)
{
    /** nof_slots_for_5G_sb_tx_cal = 500(5G) / 250 (dnx data nif.mac_client.sb_cal_granularity).
      * dnx data can't be use here because the unit id cannot be provided in the comparison function). */
    const int nof_slots_for_5G_sb_tx_cal = 5000 / 250;
    dnx_algo_cal_object_t *cal_object_a = ((dnx_algo_cal_object_t *) a);
    dnx_algo_cal_object_t *cal_object_b = ((dnx_algo_cal_object_t *) b);

    /** Sorting should be done on (1G < speed <= 5G) before (5G < speed) and in a decreasing order inside. */
    if ((cal_object_b->orig_nof_slots > nof_slots_for_5G_sb_tx_cal
         && cal_object_a->orig_nof_slots > nof_slots_for_5G_sb_tx_cal)
        || (cal_object_b->orig_nof_slots <= nof_slots_for_5G_sb_tx_cal
            && cal_object_a->orig_nof_slots <= nof_slots_for_5G_sb_tx_cal))
    {
        /** Comparing nof_slots (objects' speeds are in the same speed group. */
        return cal_object_b->nof_slots - cal_object_a->nof_slots;
    }
    else
    {
        /** Comparing by the rule (1G < speed <= 5G) is before (speed > 5G). */
        return cal_object_a->orig_nof_slots - cal_object_b->orig_nof_slots;
    }
}

static shr_error_e
dnx_port_nif_calendar_build_and_fix(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    int nof_calendar_clients,
    uint32 *calendar,
    uint32 *calendar_length,
    bcm_pbmp_t * ports_tests,
    bcm_pbmp_t check_old_cal,
    bcm_pbmp_t total_tests,
    dnx_port_nif_calendar_check_t tests_data[DNX_PORT_NIF_CALENDAR_CHECK_NOF])
{
    int i, nof_tests, should_check_transition;
    dnx_algo_cal_random_start_info_t info;
    dnx_algo_cal_detailed_entry_t *cal_dist_info = NULL;
    dnx_algo_cal_random_object_transition_info_t *trans_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&info, 0, sizeof(info));
    info.compare_cb =
        (calendar_type == DNX_PORT_NIF_FRAMER_SB_TX_CALENDAR) ? dnx_port_nif_sb_tx_calendar_slots_cmp : NULL;
    _SHR_PBMP_COUNT(total_tests, nof_tests);
    _SHR_PBMP_COUNT(check_old_cal, should_check_transition);

    switch (calendar_type)
    {
        case DNX_PORT_NIF_MACSEC0_CALENDAR:
        case DNX_PORT_NIF_MACSEC1_CALENDAR:
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
        case DNX_PORT_NIF_FRAMER_SB_RX_CALENDAR:
        case DNX_PORT_NIF_FRAMER_SB_TX_CALENDAR:
        case DNX_PORT_NIF_FLEXE_PHY_CALENDAR:
            if (should_check_transition)
            {
                SHR_ALLOC_SET_ZERO_ERR_EXIT(trans_info,
                                            sizeof(dnx_algo_cal_random_object_transition_info_t) * nof_calendar_clients,
                                            "Transition info object", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
                SHR_IF_ERR_EXIT(dnx_port_nif_calendar_with_transition_prepare_and_build
                                (unit, calendar_type, nof_slots_per_port, nof_calendar_clients, calendar,
                                 *calendar_length, &info, ports_tests, check_old_cal, total_tests, tests_data,
                                 trans_info));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_algo_cal_group_and_build
                                (unit, dnx_algo_cal_random_start_build_cb, nof_slots_per_port, nof_calendar_clients,
                                 *calendar_length, *calendar_length, &info, calendar));
            }

            /*
             * No need to fix calendar if there aren't any tests for it 
             */
            if (nof_tests > 0)
            {
                SHR_ALLOC_SET_ZERO_ERR_EXIT(cal_dist_info,
                                            sizeof(dnx_algo_cal_detailed_entry_t) * (*calendar_length) * nof_tests,
                                            "calendar info", "%s%s%s", EMPTY, EMPTY, EMPTY);

                SHR_IF_ERR_EXIT(dnx_port_nif_calendar_dist_calc
                                (unit, calendar, *calendar_length, nof_calendar_clients, ports_tests, total_tests,
                                 tests_data, cal_dist_info));
                SHR_IF_ERR_EXIT(algo_cal_fix(unit, *calendar_length, nof_tests, nof_tests, TRUE, cal_dist_info));

                /** We should not check macsec calendars for transition because calendar length isn't const. */
                if (should_check_transition && !DNX_PORT_NIF_PORT_TYPE_IS_MACSEC(calendar_type))
                {
                    SHR_IF_ERR_EXIT(algo_cal_transition_fix
                                    (unit, *calendar_length, nof_calendar_clients, trans_info, nof_tests,
                                     cal_dist_info));
                }

                for (i = 0; i < *calendar_length; ++i)
                {
                    calendar[i] = cal_dist_info[i].object;
                }
            }

#ifdef INCLUDE_XFLOW_MACSEC
            if (DNX_PORT_NIF_PORT_TYPE_IS_MACSEC(calendar_type))
            {
                if (dnx_data_macsec.general.feature_get(unit, dnx_data_macsec_general_is_shared_macsec_supported))
                {
                    SHR_IF_ERR_EXIT(dnx_xflow_macsec_shared_macsec_calendar_padding
                                    (unit, DNX_PORT_NIF_PORT_MACSEC_ID_GET(calendar_type), calendar, calendar_length));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Shared MACSec is required.");
                }
            }
#endif
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar type %d is not supported.", calendar_type);
            break;
    }

exit:
    SHR_FREE(trans_info);
    SHR_FREE(cal_dist_info);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_calendar_is_relative_deviation_get(
    int unit,
    dnx_port_nif_calendar_check_type_e test,
    int *is_relative_deviation)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (test)
    {
        case DNX_PORT_NIF_CALENDAR_CHECK_SINGLE_DISTANCE:
        case DNX_PORT_NIF_CALENDAR_CHECK_DOUBLE_DISTANCE:
        case DNX_PORT_NIF_CALENDAR_CHECK_TRIPLE_DISTANCE:
        case DNX_PORT_NIF_CALENDAR_CHECK_WB_SINGLE_DISTANCE:
            *is_relative_deviation = TRUE;
            break;

        case DNX_PORT_NIF_CALENDAR_CHECK_MINIMAL_DISTANCE:
        case DNX_PORT_NIF_CALENDAR_CHECK_MACSEC_DISTANCE:
            *is_relative_deviation = FALSE;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Calendar's test %d is not supported.", test);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_calendar_constraints_calc(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    uint32 calendar_length,
    int nof_clients,
    bcm_pbmp_t * clients_tests,
    bcm_pbmp_t check_old_cal,
    bcm_pbmp_t total_tests,
    dnx_port_nif_calendar_check_t tests_data[DNX_PORT_NIF_CALENDAR_CHECK_NOF],
    dnx_algo_cal_random_object_transition_info_t * trans_info)
{
    bcm_port_t test;
    float deviation, optimal_gap;
    uint32 entry_index;
    int client, actual_gap, min_allowed_gap, max_allowed_gap, level, max_level, is_relative_deviation, nof_tests;
    SHR_FUNC_INIT_VARS(unit);

    for (client = 0; client < nof_clients; ++client)
    {
        int min_index, max_index;
        _SHR_PBMP_COUNT(clients_tests[client], nof_tests);
        if (!nof_tests)
        {
            continue;
        }

        if (_SHR_PBMP_MEMBER(check_old_cal, client) && nof_slots_per_port[client] > 0)
        {
            _SHR_PBMP_MAX_ITER(clients_tests[client], DNX_PORT_NIF_CALENDAR_CHECK_NOF, test)
            {
                SHR_IF_ERR_EXIT(dnx_port_nif_calendar_is_relative_deviation_get(unit, test, &is_relative_deviation));

                optimal_gap = 1.0 * calendar_length / nof_slots_per_port[client];
                if (is_relative_deviation)
                {
                    max_level = tests_data[test].distance_data.level;
                    deviation = tests_data[test].distance_data.deviation;
                    max_allowed_gap = ceil((max_level + deviation) * optimal_gap);
                    min_allowed_gap = trunc((max_level - deviation) * optimal_gap);
                }
                else
                {
                    max_level = 1;
                    deviation = tests_data[test].min_distance_data.limit_value;
                    max_allowed_gap = -1;
                    min_allowed_gap = deviation + 1;
                }

                for (level = 0; level < max_level; ++level)
                {
                    SHR_IF_ERR_EXIT(dnx_port_nif_calendar_object_db_get
                                    (unit, calendar_type, client, level, &entry_index));
                    actual_gap = calendar_length - entry_index;
                    min_index = (min_allowed_gap > actual_gap) ? (min_allowed_gap - actual_gap) : 0;
                    max_index = (max_allowed_gap < 0) ? calendar_length : (max_allowed_gap - actual_gap);

                    if (!trans_info[client].slot_info[max_level - 1 - level].valid)
                    {
                        trans_info[client].slot_info[max_level - 1 - level].min_first_slot = min_index;
                        trans_info[client].slot_info[max_level - 1 - level].max_first_slot = max_index;
                    }
                    else
                    {
                        trans_info[client].slot_info[max_level - 1 - level].min_first_slot =
                            UTILEX_MAX(trans_info[client].slot_info[max_level - 1 - level].min_first_slot, min_index);
                        trans_info[client].slot_info[max_level - 1 - level].max_first_slot =
                            UTILEX_MIN(trans_info[client].slot_info[max_level - 1 - level].max_first_slot, max_index);

                    }
                    trans_info[client].slot_info[max_level - 1 - level].valid = TRUE;
                    if (trans_info[client].slot_info[max_level - 1 - level].min_first_slot >
                        trans_info[client].slot_info[max_level - 1 - level].max_first_slot)
                    {
                        trans_info[client].slot_info[max_level - 1 - level].valid = FALSE;
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_calendar_with_transition_prepare_and_build(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    int nof_clients,
    uint32 *calendar,
    uint32 calendar_length,
    dnx_algo_cal_random_start_info_t * info,
    bcm_pbmp_t * ports_tests,
    bcm_pbmp_t check_old_cal,
    bcm_pbmp_t total_tests,
    dnx_port_nif_calendar_check_t tests_data[DNX_PORT_NIF_CALENDAR_CHECK_NOF],
    dnx_algo_cal_random_object_transition_info_t * trans_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_nif_calendar_constraints_calc
                    (unit, calendar_type, nof_slots_per_port, calendar_length, nof_clients, ports_tests,
                     check_old_cal, total_tests, tests_data, trans_info));

    SHR_IF_ERR_EXIT(dnx_algo_cal_random_start_build
                    (unit, nof_slots_per_port, nof_clients, calendar_length, calendar_length, info, trans_info,
                     calendar));
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
    dbal_tables_e * table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (calendar_type)
    {
        case DNX_PORT_NIF_ARB_RX_QPM_CALENDAR:
            *table_id = DBAL_TABLE_NIF_ARB_RX_QPM_CALENDAR;
            break;

        case DNX_PORT_NIF_ARB_RX_SCH_CALENDAR:
            *table_id = DBAL_TABLE_NIF_ARB_RX_SCH_CALENDAR;
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
            break;

        case DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR:
            *table_id = DBAL_TABLE_NIF_ARB_TX_TMAC_CALENDAR;
            break;

        case DNX_PORT_NIF_OFT_CALENDAR:
            *table_id = DBAL_TABLE_NIF_OFT_CALENDAR;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar type %d is not supported.", calendar_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_calendar_table_bubble_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *bubble_val)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (calendar_type)
    {
        case DNX_PORT_NIF_ARB_RX_QPM_CALENDAR:
            *bubble_val = dnx_data_nif.arb.rx_qpm_calendar_nof_clients_get(unit);
            break;

        case DNX_PORT_NIF_ARB_RX_SCH_CALENDAR:
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
            *bubble_val = dnx_data_nif.eth.nof_lanes_in_cdu_get(unit) + 1;
            break;

        case DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR:
            *bubble_val = dnx_data_nif.mac_client.nof_normal_clients_get(unit);
            break;

        case DNX_PORT_NIF_OFT_CALENDAR:
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
    const int nof_qpm_clients = dnx_data_nif.arb.rx_qpm_calendar_nof_clients_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_table_id_get(unit, calendar_type, &table_id));
    SHR_IF_ERR_EXIT(dnx_port_calendar_table_bubble_get(unit, calendar_type, &bubble_val));
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
            client_val = calendar[calendar_entry_index] % nof_qpm_clients;
            qpm_id = calendar[calendar_entry_index] / nof_qpm_clients;
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
    SHR_IF_ERR_EXIT(dnx_port_calendar_table_id_get(unit, calendar_type, &table_id));
    SHR_IF_ERR_EXIT(dnx_port_calendar_table_bubble_get(unit, calendar_type, &bubble_val));
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
