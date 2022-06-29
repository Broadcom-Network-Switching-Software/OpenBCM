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

#include <bcm/types.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_rand.h>
#include <bcm_int/dnx/algo/cal/algo_cal.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_calendar.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_nif_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_nif_access.h>

#ifdef INCLUDE_XFLOW_MACSEC
#include <dnx/dnx_sec.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_macsec.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/sec_access.h>
#endif

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#define DNX_PORT_NIF_PORT_TYPE_IS_ARB_ETH(calendar_type) ((calendar_type) >= DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR && (calendar_type) <= DNX_PORT_NIF_ARB_TX_PPM7_CALENDAR)
#define DNX_PORT_NIF_PORT_PM_ID_GET(calendar_type) ((calendar_type) - DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR)
#define DNX_PORT_NIF_PORT_MACSEC_ID_GET(calendar_type) ((calendar_type) - DNX_PORT_NIF_MACSEC0_CALENDAR)

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
static shr_error_e dnx_port_nif_slots_per_port_prepare(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    int nof_calendar_clients,
    int direction,
    int granularity,
    bcm_pbmp_t * ports_tests,
    bcm_pbmp_t * total_tests,
    bcm_pbmp_t * check_old_cal);
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
static shr_error_e dnx_port_nif_calendar_build_and_fix(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    int nof_calendar_clients,
    uint32 *calendar,
    int calendar_length,
    bcm_pbmp_t * ports_tests,
    bcm_pbmp_t total_tests,
    dnx_port_nif_calendar_check_t tests_data[DNX_PORT_NIF_CALENDAR_CHECK_NOF]);
static shr_error_e dnx_port_nif_calendar_current_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    int calendar_length,
    uint32 *calendar);
static shr_error_e dnx_port_calendar_slots_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *calendar,
    int calendar_id,
    int calendar_length);
static shr_error_e dnx_port_calendar_slots_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 active_calendar_id,
    int calendar_length,
    uint32 *calendar);

/* dbal */
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
static shr_error_e dnx_port_qpm_calendar_slots_get(
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
static shr_error_e dnx_port_general_calendar_slots_get(
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
    int calendar_length;
    uint32 *calendar = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Calculating new calendar 
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_build_only(unit, calendar_type, &calendar, &calendar_length));

    /*
     * Setting new calendar to hw 
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_calendar_config(unit, calendar_type, calendar, calendar_length));

exit:
    SHR_FREE(calendar);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_calendar_build_only(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 **calendar,
    int *calendar_length)
{
    bcm_pbmp_t check_old_cal;
    bcm_pbmp_t *ports_tests = NULL, total_tests;
    int nof_calendar_clients, direction, granularity;
    uint32 *nof_slots_per_port = NULL, *old_calendar = NULL;
    dnx_port_nif_calendar_check_t tests_data[DNX_PORT_NIF_CALENDAR_CHECK_NOF] = { {{0}} };
    SHR_FUNC_INIT_VARS(unit);

    if (calendar_length == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar_length is not allocated.");
    }

    /*
     * Get calendar parameters for algo & set tests_data for checking calendar's distribution
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_algo_parameters_get
                    (unit, calendar_type, &nof_calendar_clients, calendar_length, &direction, &granularity));
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
    SHR_ALLOC_SET_ZERO_ERR_EXIT(*calendar, *calendar_length * sizeof(uint32),
                                "Calendar to set slots per port", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(nof_slots_per_port, nof_calendar_clients * sizeof(uint32),
                                "Saves how many slots each port needs", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(ports_tests, nof_calendar_clients * sizeof(bcm_pbmp_t),
                                "Saves which tests will be tested each port needs", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_port_nif_slots_per_port_prepare
                    (unit, calendar_type, nof_slots_per_port, nof_calendar_clients, direction, granularity,
                     ports_tests, &total_tests, &check_old_cal));

    
    if (0 && _SHR_PBMP_NOT_NULL(check_old_cal))
    {
        SHR_ALLOC_SET_ZERO_ERR_EXIT(old_calendar, *calendar_length * sizeof(uint32),
                                    "Calendar to get old slots per port", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(dnx_port_nif_calendar_current_get(unit, calendar_type, *calendar_length, old_calendar));
    }
    else
    {
        _SHR_PBMP_CLEAR(check_old_cal);
    }

    SHR_IF_ERR_EXIT(dnx_port_nif_calendar_build_and_fix
                    (unit, calendar_type, nof_slots_per_port, nof_calendar_clients, *calendar, *calendar_length,
                     ports_tests, total_tests, tests_data));

    SHR_IF_ERR_EXIT(dnx_port_nif_calendar_verify
                    (unit, calendar_type, nof_slots_per_port, nof_calendar_clients,
                     *calendar, old_calendar, *calendar_length, ports_tests, check_old_cal, tests_data));

exit:
    SHR_FREE(ports_tests);
    SHR_FREE(old_calendar);
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
    uint32 *old_calendar,
    int calendar_length,
    bcm_pbmp_t * clients_tests,
    bcm_pbmp_t check_old_calendar,
    dnx_port_nif_calendar_check_t tests_data[DNX_PORT_NIF_CALENDAR_CHECK_NOF])
{
    bcm_port_t test;
    bcm_pbmp_t min_distance_clients;
#ifdef INCLUDE_XFLOW_MACSEC
    int check_macsec_tests = FALSE;
#endif
    int client_id, nof_slots, passed_cyclic = TRUE, passed_transition =
        TRUE, g_hao_client, check_old_cal_for_min_distance = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_CLEAR(min_distance_clients);
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
                                             tests_data[test].distance_data.deviation, calendar, old_calendar,
                                             calendar_length, &passed_transition));
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
                                             tests_data[test].distance_data.deviation, calendar, old_calendar,
                                             calendar_length, &passed_transition));
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
                                             tests_data[test].distance_data.deviation, calendar, old_calendar,
                                             calendar_length, &passed_transition));
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
                        if (_SHR_PBMP_MEMBER(check_old_calendar, client_id) && g_hao_client != client_id)
                        {
                            passed_transition = (calendar[0] == client_id
                                                 && old_calendar[calendar_length - 1] == client_id);
                        }

                        if (passed_cyclic == FALSE || passed_transition == FALSE)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                         "Building calendar failed. Calendar's distribution is not meeting the MacSec limitation.");
                        }
                        break;
#ifdef INCLUDE_XFLOW_MACSEC
                    case DNX_PORT_NIF_CALENDAR_CHECK_MACSEC_GENERAL:
                        check_macsec_tests = TRUE;
                        break;
#endif
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

#ifdef INCLUDE_XFLOW_MACSEC
    /*
     * After going per port, doing general calendar tests 
     */
    if (check_macsec_tests)
    {
        int passed = FALSE;
        SHR_IF_ERR_EXIT(dnx_xflow_macsec_shared_macsec_calendar_check(unit, calendar, calendar_length, &passed));
        if (passed == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Building calendar failed. Test is not meeting general Macsec rules.");
        }
    }
#endif

    if (_SHR_PBMP_NOT_NULL(min_distance_clients))
    {
        if (check_old_cal_for_min_distance)
        {
            SHR_IF_ERR_EXIT(dnx_algo_cal_min_distance_transition_check
                            (unit, min_distance_clients, calendar, old_calendar, calendar_length,
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
                dnx_data_nif.phys.nof_phys_get(unit) + dnx_data_nif.mac_client.nof_normal_clients_get(unit);
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
            *nof_calendar_clients = dnx_data_nif.mac_client.nof_normal_clients_get(unit);
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

        case DNX_PORT_NIF_FRAMER_SB_RX_CALENDAR:
        case DNX_PORT_NIF_FRAMER_SB_TX_CALENDAR:
            /*
             * set Framer SB Rx & Tx calendar parameters
             */
            *nof_calendar_clients = dnx_data_nif.mac_client.nof_normal_clients_get(unit);
            *calendar_length = dnx_data_nif.mac_client.nof_sb_cal_slots_get(unit);
            *direction =
                (calendar_type ==
                 DNX_PORT_NIF_FRAMER_SB_TX_CALENDAR) ? DNX_ALGO_PORT_SPEED_F_TX : DNX_ALGO_PORT_SPEED_F_RX;
            *granularity = dnx_data_nif.mac_client.sb_cal_granularity_get(unit);
            break;

#ifdef INCLUDE_XFLOW_MACSEC
        case DNX_PORT_NIF_MACSEC0_CALENDAR:
        case DNX_PORT_NIF_MACSEC1_CALENDAR:
            /*
             * set MACSec calendar parameters
             */
            *nof_calendar_clients = dnx_data_macsec.general.ports_in_macsec_nof_get(unit);
            *calendar_length = dnx_data_macsec.general.macsec_tdm_cal_max_depth_get(unit);
            *direction = 0;
            *granularity =
                UTILEX_DIV_ROUND_UP(dnx_data_macsec.wrapper.wrapper_speed_max_mbps_get(unit), *calendar_length);
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
dnx_port_nif_slots_per_port_prepare(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    int nof_calendar_clients,
    int direction,
    int granularity,
    bcm_pbmp_t * ports_tests,
    bcm_pbmp_t * total_tests,
    bcm_pbmp_t * check_old_cal)
{
    bcm_port_t logical_port;
    bcm_pbmp_t all_ports, all_eth_ports, all_framer_ports;
    int core, client_id, speed, is_enable, has_speed;
#ifdef INCLUDE_XFLOW_MACSEC
    dnx_macsec_port_info_t macsec_port_info;
#endif
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get all logical ports
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH,
                     DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_L1 |
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_FRAMER_MGMT | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT,
                     &all_eth_ports));
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FRAMER_MAC,
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT, &all_framer_ports));

    /*
     * Clearing all bitmaps 
     */
    _SHR_PBMP_CLEAR(*check_old_cal);
    _SHR_PBMP_CLEAR(*total_tests);
    _SHR_PBMP_CLEAR(all_ports);
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
            }
            else
#endif
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Shared MACSec is not supported.");
            }
            break;

        case DNX_PORT_NIF_ARB_RX_SCH_CALENDAR:
            _SHR_PBMP_OR(all_ports, all_eth_ports);
            _SHR_PBMP_OR(all_ports, all_framer_ports);
            SHR_IF_ERR_EXIT(dnx_port_nif_rx_scheduler_calendar_slots_per_port_build
                            (unit, nof_slots_per_port, all_ports, granularity));
            break;

        case DNX_PORT_NIF_OFT_CALENDAR:
            _SHR_PBMP_OR(all_ports, all_framer_ports);
            _SHR_PBMP_OR(all_ports, all_eth_ports);
            SHR_IF_ERR_EXIT(dnx_port_nif_unified_calendar_slots_per_port_build
                            (unit, nof_slots_per_port, all_ports, direction, granularity));
            break;

        case DNX_PORT_NIF_ARB_RX_QPM_CALENDAR:
            _SHR_PBMP_OR(all_ports, all_eth_ports);
            SHR_IF_ERR_EXIT(dnx_port_nif_unified_calendar_slots_per_port_build
                            (unit, nof_slots_per_port, all_ports, direction, granularity));
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
            break;

        case DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR:
            _SHR_PBMP_OR(all_ports, all_framer_ports);
            SHR_IF_ERR_EXIT(dnx_port_nif_arb_tx_tmac_slots_per_port_build
                            (unit, nof_slots_per_port, all_ports, granularity));
            break;

        case DNX_PORT_NIF_FRAMER_SB_RX_CALENDAR:
        case DNX_PORT_NIF_FRAMER_SB_TX_CALENDAR:
            _SHR_PBMP_OR(all_ports, all_framer_ports);
            SHR_IF_ERR_EXIT(dnx_port_nif_framer_sb_slots_per_port_build
                            (unit, nof_slots_per_port, all_ports, direction, granularity));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar type %d is not supported.", calendar_type);
            break;
    }

    _SHR_PBMP_ITER(all_ports, logical_port)
    {
        speed = 0;
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, direction, &has_speed));
        if (has_speed)
        {
            /*
             * For arbiter Rx scheduler the clients are the scheduler's modes 
             */
            if (calendar_type == DNX_PORT_NIF_ARB_RX_SCH_CALENDAR)
            {
                SHR_IF_ERR_EXIT(dnx_port_nif_scheduler_mode_get(unit, logical_port, &client_id));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, logical_port, 0, &core, &client_id));
                if (DNX_PORT_NIF_PORT_TYPE_IS_ARB_ETH(calendar_type))
                {
                    client_id %= nof_calendar_clients;
                }
            }
            SHR_IF_ERR_EXIT(bcm_port_enable_get(unit, logical_port, &is_enable));
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, direction, &speed));
            if ((calendar_type) == DNX_PORT_NIF_FRAMER_SB_TX_CALENDAR)
            {
                _SHR_PBMP_PORT_ADD(ports_tests[client_id], DNX_PORT_NIF_CALENDAR_CHECK_WB_SINGLE_DISTANCE);
                _SHR_PBMP_PORT_ADD(*total_tests, DNX_PORT_NIF_CALENDAR_CHECK_WB_SINGLE_DISTANCE);
                _SHR_PBMP_PORT_ADD(ports_tests[client_id], DNX_PORT_NIF_CALENDAR_CHECK_DOUBLE_DISTANCE);
                _SHR_PBMP_PORT_ADD(*total_tests, DNX_PORT_NIF_CALENDAR_CHECK_DOUBLE_DISTANCE);

                if (speed < 200000)
                {
                    _SHR_PBMP_PORT_ADD(ports_tests[client_id], DNX_PORT_NIF_CALENDAR_CHECK_MINIMAL_DISTANCE);
                    _SHR_PBMP_PORT_ADD(*total_tests, DNX_PORT_NIF_CALENDAR_CHECK_MINIMAL_DISTANCE);
                }
            }
            else
            {
#ifdef INCLUDE_XFLOW_MACSEC
                if (calendar_type == DNX_PORT_NIF_ARB_RX_SCH_CALENDAR || calendar_type == DNX_PORT_NIF_OFT_CALENDAR
                    || calendar_type == DNX_PORT_NIF_MACSEC0_CALENDAR || calendar_type == DNX_PORT_NIF_MACSEC1_CALENDAR)
                {
                    SHR_IF_ERR_EXIT(dnx_xflow_macsec_port_to_macsec_port_info_get
                                    (unit, logical_port, &macsec_port_info));
                    if (macsec_port_info.macsec_id != -1)
                    {
                        _SHR_PBMP_PORT_ADD(ports_tests[client_id], DNX_PORT_NIF_CALENDAR_CHECK_MACSEC_DISTANCE);
                        _SHR_PBMP_PORT_ADD(*total_tests, DNX_PORT_NIF_CALENDAR_CHECK_MACSEC_DISTANCE);
                    }
                }
#endif
                _SHR_PBMP_PORT_SET(ports_tests[client_id], DNX_PORT_NIF_CALENDAR_CHECK_TRIPLE_DISTANCE);
                _SHR_PBMP_PORT_ADD(*total_tests, DNX_PORT_NIF_CALENDAR_CHECK_TRIPLE_DISTANCE);
                if (speed <= 100000)
                {
                    _SHR_PBMP_PORT_ADD(ports_tests[client_id], DNX_PORT_NIF_CALENDAR_CHECK_DOUBLE_DISTANCE);
                    _SHR_PBMP_PORT_ADD(*total_tests, DNX_PORT_NIF_CALENDAR_CHECK_DOUBLE_DISTANCE);
                }

                if (speed <= 5000)
                {
                    _SHR_PBMP_PORT_ADD(ports_tests[client_id], DNX_PORT_NIF_CALENDAR_CHECK_SINGLE_DISTANCE);
                    _SHR_PBMP_PORT_ADD(*total_tests, DNX_PORT_NIF_CALENDAR_CHECK_SINGLE_DISTANCE);
                }
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
dnx_port_nif_calendar_current_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    int calendar_length,
    uint32 *calendar)
{
    uint32 active_calendar_id;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get active calendar id
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_active_id_get(unit, calendar_type, &active_calendar_id));

    SHR_IF_ERR_EXIT(dnx_port_calendar_slots_get(unit, calendar_type, active_calendar_id, calendar_length, calendar));

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

shr_error_e dnx_algo_cal_random_start_build_cb(
    int unit,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    void *additional_info,
    uint32 *calendar);

static shr_error_e
dnx_port_nif_calendar_build_and_fix(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    int nof_calendar_clients,
    uint32 *calendar,
    int calendar_length,
    bcm_pbmp_t * ports_tests,
    bcm_pbmp_t total_tests,
    dnx_port_nif_calendar_check_t tests_data[DNX_PORT_NIF_CALENDAR_CHECK_NOF])
{
    int i, nof_tests;
    UTILEX_RAND rand_state;
    dnx_algo_cal_detailed_entry_t *cal_dist_info = NULL;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&rand_state, 0, sizeof(rand_state));
    _SHR_PBMP_COUNT(total_tests, nof_tests);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(cal_dist_info, sizeof(dnx_algo_cal_detailed_entry_t) * calendar_length * nof_tests,
                                "calendar info", "%s%s%s", EMPTY, EMPTY, EMPTY);

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
            SHR_IF_ERR_EXIT(dnx_algo_cal_group_and_build(unit, dnx_algo_cal_random_start_build_cb, nof_slots_per_port,
                                                         nof_calendar_clients, calendar_length, calendar_length,
                                                         &rand_state, calendar));

#ifdef INCLUDE_XFLOW_MACSEC
            if (calendar_type == DNX_PORT_NIF_MACSEC0_CALENDAR || calendar_type == DNX_PORT_NIF_MACSEC1_CALENDAR)
            {
                if (dnx_data_macsec.general.feature_get(unit, dnx_data_macsec_general_is_shared_macsec_supported))
                {
                    SHR_IF_ERR_EXIT(dnx_xflow_macsec_shared_macsec_calendar_padding
                                    (unit, DNX_PORT_NIF_PORT_MACSEC_ID_GET(calendar_type), calendar, &calendar_length));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Shared MACSec is not supported.");
                }
            }
#endif
            /*
             * No need to fix calendar if there aren't any tests for it 
             */
            if (nof_tests > 0)
            {
                SHR_IF_ERR_EXIT(dnx_port_nif_calendar_dist_calc
                                (unit, calendar, calendar_length, nof_calendar_clients, ports_tests, total_tests,
                                 tests_data, cal_dist_info));
                SHR_IF_ERR_EXIT(algo_cal_fix(unit, calendar_length, nof_tests, nof_tests, TRUE, cal_dist_info));

                for (i = 0; i < calendar_length; ++i)
                {
                    calendar[i] = cal_dist_info[i].object;
                }
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar type %d is not supported.", calendar_type);
            break;
    }

exit:
    SHR_FREE(cal_dist_info);
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
dnx_port_calendar_slots_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 active_calendar_id,
    int calendar_length,
    uint32 *calendar)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (calendar_type)
    {
        case DNX_PORT_NIF_ARB_RX_QPM_CALENDAR:
            SHR_IF_ERR_EXIT(dnx_port_qpm_calendar_slots_get
                            (unit, calendar_type, calendar, active_calendar_id, calendar_length));
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
            SHR_IF_ERR_EXIT(dnx_port_general_calendar_slots_get
                            (unit, calendar_type, calendar, active_calendar_id, calendar_length));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Calendar type %d is not supported.", calendar_type);
            break;
    }

exit:
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

static shr_error_e
dnx_port_qpm_calendar_slots_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *calendar,
    int calendar_id,
    int calendar_length)
{
    uint32 entry_handle_id;
    uint32 client_val, bubble_val;
    int calendar_entry_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_table_bubble_get(unit, calendar_type, &bubble_val));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_RX_QPM_CALENDAR, &entry_handle_id));

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

        /*
         * set QPM_ID = 0, if getting a bubble getting value from QPM_ID = 1
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPM_ID, 0);

        /*
         * set value fields
         */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CLIENT, INST_SINGLE, &client_val);

        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * If got a bubble then taking the other QPM slot
         */
        if (client_val == bubble_val)
        {
            /*
             * set QPM_ID = 1 after QPM_ID = 0 has bubble
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPM_ID, 1);

            /*
             * set value fields
             */
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CLIENT, INST_SINGLE, &client_val);

            /*
             * commit the value
             */
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            calendar[calendar_entry_index] = (client_val == bubble_val) ? DNX_ALGO_CAL_ILLEGAL_OBJECT_ID : client_val;
        }

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_general_calendar_slots_get(
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
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CLIENT, INST_SINGLE, &client_val);

        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        calendar[calendar_entry_index] = (client_val == bubble_val) ? DNX_ALGO_CAL_ILLEGAL_OBJECT_ID : client_val;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
