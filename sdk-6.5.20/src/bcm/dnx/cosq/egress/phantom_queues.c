/*
 * pahtom_queues.c
 *
 *  Created on: Dec 16, 2018
 *      Author: si888124
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/phantom_queues_access.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>

#include "phantom_queues_dbal.h"

/*
 * see .h file
 */
void
dnx_cosq_ohantom_queues_profile_print_cb(
    int unit,
    const void *data)
{
    uint32 *threshold = (uint32 *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Phantom Queues Threshold", *threshold, NULL,
                                   NULL);

    SW_STATE_PRETTY_PRINT_FINISH();
}

/**
 * \brief - Create template manager for phantom queues threshold profiles
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_phantom_queues_profile_init(
    int unit)
{
    bcm_core_t core;
    dnx_algo_template_create_data_t create_data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&create_data, 0, sizeof(create_data));

    /** Fill the create data for the template manager */
    create_data.flags = 0;
    create_data.first_profile = 0;
    create_data.max_references = dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit);
    create_data.data_size = sizeof(uint32);
    create_data.nof_profiles = dnx_data_egr_queuing.phantom_queues.nof_threshold_profiles_get(unit);
    sal_strncpy(create_data.name, "Phantom Queues Threshold Profile", DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    /** call the template manager create function */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_phantom_queues_db.phantom_queues.create(unit, core, &create_data, NULL));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 *  brief - init function for phantom queues.
 */
shr_error_e
dnx_phantom_queues_init(
    int unit)
{
    int egq_if;
    int nof_egq_if = dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    /*
     * init SW state DB
     */
    SHR_IF_ERR_EXIT(dnx_phantom_queues_db.init(unit));

    /*
     * template manager init
     */
    SHR_IF_ERR_EXIT(dnx_cosq_phantom_queues_profile_init(unit));

    /*
     * enable phantom queues globally in PP, but there is still an enabler for each EGQ IF,
     * which should be enabled by the user using API.
     */
    SHR_IF_ERR_EXIT(dnx_phantom_queues_dbal_global_enable_set(unit, TRUE));

    /*
     * disable Phantom Queues per EGQ IF
     */
    for (egq_if = 0; egq_if < nof_egq_if; ++egq_if)
    {
        SHR_IF_ERR_EXIT(dnx_phantom_queues_dbal_egq_if_enable_set(unit, BCM_CORE_ALL, egq_if, FALSE, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - veriy function for dnx_phantom_queues_threshold_set
 */
static shr_error_e
dnx_phantom_queues_threshold_set_verify(
    int unit,
    bcm_port_t logical_port,
    int threshold)
{
    dnx_algo_port_info_s port_info;
    uint32 max_threshold = dnx_data_egr_queuing.phantom_queues.max_threshold_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    /*
     * verify port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Invalid port %d\n", logical_port);
    }

    SHR_RANGE_VERIFY(threshold, 0, max_threshold, _SHR_E_PARAM, "Threshold %d is out of bounds", threshold);
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_phantom_queues_threshold_set(
    int unit,
    bcm_port_t port,
    int threshold)
{
    bcm_core_t core;
    int egq_if, new_thresh_profile = 0;
    uint32 is_enabled, to_enable, old_thresh_profile, thresh_val = threshold;
    uint8 is_first = FALSE, is_last = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_phantom_queues_threshold_set_verify(unit, port, threshold));

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egq_if));

    /** get old profile */
    SHR_IF_ERR_EXIT(dnx_phantom_queues_dbal_egq_if_enable_get(unit, core, egq_if, &is_enabled, &old_thresh_profile));

    to_enable = (threshold > 0) ? TRUE : FALSE;

    if (to_enable)
    {
        if (is_enabled)
        {
            /** exchange to a profile with the requested delta */
            SHR_IF_ERR_EXIT(dnx_phantom_queues_db.
                            phantom_queues.exchange(unit, core, 0, &thresh_val, old_thresh_profile, NULL,
                                                    &new_thresh_profile, &is_first, &is_last));
        }
        else
        {
            /** allocate a new profile */
            SHR_IF_ERR_EXIT(dnx_phantom_queues_db.phantom_queues.allocate_single(unit, core, 0, &thresh_val, NULL,
                                                                                 &new_thresh_profile, &is_first));
        }
    }
    else if (!to_enable && is_enabled)
    {
        /** free the allocated profile */
        SHR_IF_ERR_EXIT(dnx_phantom_queues_db.phantom_queues.free_single(unit, core, old_thresh_profile, &is_last));
    }

    if (is_first)
    {
        /** configure threshold in HW */
        SHR_IF_ERR_EXIT(dnx_phantom_queues_dbal_threshold_set(unit, core, new_thresh_profile, threshold));
    }
    if (is_last)
    {
        /** configure old profile threshold to 0 */
        SHR_IF_ERR_EXIT(dnx_phantom_queues_dbal_threshold_set(unit, core, old_thresh_profile, 0));
    }

    /** Set mapping to new profile */
    SHR_IF_ERR_EXIT(dnx_phantom_queues_dbal_egq_if_enable_set(unit, core, egq_if, to_enable, new_thresh_profile));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_phantom_queues_threshold_get(
    int unit,
    bcm_port_t port,
    int *threshold)
{
    bcm_core_t core;
    int egq_if;
    uint32 is_enabled, thresh_profile, thresh_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egq_if));

    /** get profile */
    SHR_IF_ERR_EXIT(dnx_phantom_queues_dbal_egq_if_enable_get(unit, core, egq_if, &is_enabled, &thresh_profile));

    if (!is_enabled)
    {
        *threshold = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_phantom_queues_dbal_threshold_get(unit, core, thresh_profile, &thresh_val));
        *threshold = thresh_val;
    }
exit:
    SHR_FUNC_EXIT;
}

#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ
