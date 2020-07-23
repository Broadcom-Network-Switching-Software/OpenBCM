/** \file fabric_control_cells.c
 * 
 *
 * Fabric control cells functions for DNX. \n
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC

/*
 * Includes:
 * {
 */
/** allow memregs.h include excplictly */
#ifdef _SOC_MEMREGS_H
#undef _SOC_MEMREGS_H
#endif
#include <sal/types.h>

#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/fabric/fabric.h>
#include <bcm_int/dnx/fabric/fabric_control_cells.h>
#include <bcm_int/dnx/fabric/fabric_if.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <sal/core/boot.h>
#include <soc/dnx/dbal/dbal.h>

/*
 * } 
 */

/*
 * Defines:
 * {
 */

/*
 * Mesh Topology Threshold
 */
#define DNX_FABRIC_CTRL_CELLS_MESH_TOPOLOGY_STATUS_TH (256)

#define DNX_FABRIC_CTRL_CELLS_INIT_TIMER_ITERATIONS      (1500)
#define DNX_FABRIC_CTRL_CELLS_INIT_FIFO_EMPTY_ITERATIONS (24)
#define DNX_FABRIC_CTRL_CELLS_INIT_RCH_STATUS_ITERATIONS (24)
#define DNX_FABRIC_CTRL_CELLS_INIT_STANDALONE_ITERATIONS (8)
#define DNX_FABRIC_CTRL_CELLS_INIT_TIMER_DELAY_MSEC      (32)

/*
 * } 
 */

/*
 * Type defs:
 * {
 */

/**
 * \brief
 *   Pointers to functions that being used by dnx_fabric_ctrl_cells_polling_t.
 */
typedef int (
    *dnx_ctrl_cells_poll_func_f) (
    int unit,
    uint32 iter_index,
    int *success);
typedef int (
    *dnx_ctrl_cells_fnly_func_f) (
    int unit);

/**
 * \brief
 *   Handle polling functions calling information
 */
typedef struct
{
    /** What the polling function do. We'll be printed in verbose */
    char *polling_name;
    /** Polling function */
    dnx_ctrl_cells_poll_func_f polling_func;
    /** Function to run in case of polling function failed */
    dnx_ctrl_cells_fnly_func_f failure_func;
    /**
     *  This is the number of iterations (at least one is performed).
     */
    uint32 nof_polling_iters;
    /** If TRUE, need to return error when polling function failed */
    uint32 err_on_fail;
} dnx_fabric_ctrl_cells_polling_t;

/*
 * } 
 */

/*
 * Functions:
 * {
 */

/**
 * \brief
 *   Polling function that return success when all links are down.
 * \see
 *   * dnx_fabric_ctrl_cells_enable_set.
 */
static shr_error_e
dnx_fabric_ctrl_cells_all_links_disable_polling(
    int unit,
    uint32 iter_index,
    int *all_down)
{
    uint32 entry_handle_id;
    int link_id = 0;
    uint32 nof_links = dnx_data_fabric.links.nof_links_get(unit);
    uint32 is_link_active = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *all_down = TRUE;

    if (nof_links == 0)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_LINK_ACTIVE, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    for (link_id = 0; link_id < nof_links; ++link_id)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IS_LINK_ACTIVE, link_id, &is_link_active));
        if (is_link_active == TRUE)
        {
            *all_down = FALSE;
            break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Polling function that return success when CPU cell FIFO is empty.
 */
static shr_error_e
dnx_fabric_ctrl_cells_fifo_empty_polling(
    int unit,
    uint32 iter_index,
    int *is_empty)
{
    uint32 entry_handle_id;
    uint32 buffer[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 is_fifo_not_empty = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Read cell from FIFO buffer
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_RECEIVE_DATA, &entry_handle_id));
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_CONTROL_CELL_DATA, INST_SINGLE, buffer);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Check if CPU-Control-FIFO is empty
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_RECEIVE_CONTROL, entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CONTROL_CELL_DATA_READY, INST_SINGLE,
                               &is_fifo_not_empty);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *is_empty = !is_fifo_not_empty;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set whether the device is in stand-alone mode.
 * \param [in] unit -
 *   The unit number.
 * \param [in] is_single_fap_mode -
 *   1 - stand-alone mode.
 *   0 - not stand-alone mode.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_ctrl_cells_stand_alone_fap_mode_set(
    int unit,
    int is_single_fap_mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_SINGLE_FAP, INST_SINGLE, is_single_fap_mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get whether the device is in stand-alone mode.
 * \param [in] unit -
 *   The unit number.
 * \param [out] is_single_fap_mode -
 *   1 - stand-alone mode.
 *   0 - not stand-alone mode.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_ctrl_cells_stand_alone_fap_mode_get(
    int unit,
    int *is_single_fap_mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_SINGLE_FAP, INST_SINGLE,
                               (uint32 *) &is_single_fap_mode);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   The following functions:
 *      dnx_fabric_ctrl_cells_standalone_polling
 *      dnx_fabric_ctrl_cells_standalone_failure
 *   works together.
 *   The 'polling' function read control cells counter.
 *   If control cells detected, that means we should set the device
 *   NOT in stand-alone mode.
 *   If control cells weren't detected, that means we should set the
 *   device in stand-alone mode, and we do it by calling the 'failure'
 *   function.
 *
 * \see
 *   * dnx_fabric_ctrl_cells_enable_set.
 */
static shr_error_e
dnx_fabric_ctrl_cells_standalone_polling(
    int unit,
    uint32 iter_index,
    int *success)
{
    uint32 entry_handle_id;
    uint32 nof_control_cells = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_STATUS, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RX_CONTROL_CELLS_TYPE1, INST_SINGLE,
                               &nof_control_cells);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * ignore first iteration in order to clear counter
     */
    *success = (nof_control_cells ? 1 : 0) && (iter_index != 0);

    /*
     * cfg not stand alone in success case 
     */
    if (*success)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_stand_alone_fap_mode_set(unit, FALSE));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   See dnx_fabric_ctrl_cells_standalone_polling comment.
 * \see
 *   * dnx_fabric_ctrl_cells_standalone_polling.
 */
static shr_error_e
dnx_fabric_ctrl_cells_standalone_failure(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_stand_alone_fap_mode_set(unit, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Check Mesh Topology status
 */
static shr_error_e
dnx_fabric_ctrl_cells_mesh_topology_check(
    int unit,
    int stand_alone,
    int *success)
{
    uint32 entry_handle_id;
    uint32 threshold, pos_watermark, neg_watermark;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_STATUS, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATUS_1, INST_SINGLE, &pos_watermark);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATUS_2, INST_SINGLE, &neg_watermark);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    threshold = DNX_FABRIC_CTRL_CELLS_MESH_TOPOLOGY_STATUS_TH;

    if (((pos_watermark < threshold) && (neg_watermark < threshold)) || (stand_alone == TRUE))
    {
        *success = TRUE;
    }
    else
    {
        *success = FALSE;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Polling function for control cells status.
 *   If control cells init didn't finished, read control cells counter
 *   and check maybe we should be in stand-alone mode.
 * \see
 *   * dnx_fabric_ctrl_cells_enable_set.
 */
static shr_error_e
dnx_fabric_ctrl_cells_status_polling(
    int unit,
    uint32 iter_index,
    int *success)
{
    uint32 entry_handle_id;
    uint32 nof_control_cells = 0;
    int stand_alone = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_stand_alone_fap_mode_get(unit, &stand_alone));

    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_mesh_topology_check(unit, stand_alone, success));

    if (*success == FALSE)
    {
        /*
         * recheck stand alone once every DNX_FABRIC_CTRL_CELLS_INIT_STANDALONE_ITERATIONS
         */
        if ((iter_index % DNX_FABRIC_CTRL_CELLS_INIT_STANDALONE_ITERATIONS == 0) && (iter_index != 0))
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_STATUS, &entry_handle_id));
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RX_CONTROL_CELLS_TYPE1, INST_SINGLE,
                                       &nof_control_cells);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            /*
             * cfg stand alone when the counter is zero
             */
            if (nof_control_cells == 0)
            {
                SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_stand_alone_fap_mode_set(unit, TRUE));

                *success = TRUE;

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Control cells enable: Stand-alone mode detected on the %u iteration.\n"),
                             iter_index));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Polling function that returns whether there is any link up.
 * \see
 *   * dnx_fabric_ctrl_cells_enable_set.
 */
static shr_error_e
dnx_fabric_ctrl_cells_links_enable_polling(
    int unit,
    uint32 iter_index,
    int *any_up)
{
    int all_down;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_all_links_disable_polling(unit, iter_index, &all_down));

    *any_up = all_down ? 0 : 1;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Clear control cells counters.
 * \see
 *   * dnx_fabric_ctrl_cells_enable_set.
 */
static shr_error_e
dnx_fabric_ctrl_cells_counter_clear(
    int unit)
{
    uint32 entry_handle_id;
    uint32 control_cells_type1 = 0;
    uint32 status_1 = 0;
    uint32 status_2 = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_STATUS, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RX_CONTROL_CELLS_TYPE1, INST_SINGLE,
                               &control_cells_type1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATUS_1, INST_SINGLE, &status_1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATUS_2, INST_SINGLE, &status_2);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get array of enabled FMAC blocks
 */
static shr_error_e
dnx_fabric_ctrl_cells_fmac_get(
    int unit,
    int *nof_enabled_fmacs,
    int fmacs_array[DNX_DATA_MAX_FABRIC_BLOCKS_NOF_INSTANCES_FMAC])
{
    uint32 fmac = 0;
    uint32 nof_instances_fmac = dnx_data_fabric.blocks.nof_instances_fmac_get(unit);
    int count = 0;
    SHR_FUNC_INIT_VARS(unit);

    for (fmac = 0; fmac < nof_instances_fmac; ++fmac)
    {
        int enable;
        SHR_IF_ERR_EXIT(dnx_fabric_if_fmac_block_enable_get(unit, fmac, &enable));
        if (enable)
        {
            fmacs_array[count++] = fmac;
        }
    }

    *nof_enabled_fmacs = count;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Actions are taken according to the 'data' param:
 *      If data includes register param - write it to HW.
 *      If data includes polling funtion - run it until success.
 *      If data includes delay - wait that time before continuing execution.
 * \see
 *   * dnx_fabric_ctrl_cells_enable_set.
 */
static shr_error_e
dnx_fabric_ctrl_cells_enable_polling(
    int unit,
    dnx_fabric_ctrl_cells_polling_t * data)
{
    int success = TRUE;
    int is_sim = FALSE;
    uint32 iter_index = 0;
    SHR_FUNC_INIT_VARS(unit);

#ifdef PLISIM
    if (SAL_BOOT_PLISIM)
    {
        is_sim = TRUE;
    }
#endif

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Control cells enable: Start polling stage - %s.\n"), data->polling_name));

    /*
     * If there polling field exists, refer to delay as the expected value 
     */
    for (iter_index = 0; iter_index < data->nof_polling_iters; ++iter_index)
    {
        if (is_sim)
        {
            success = (data->failure_func && !data->err_on_fail) ? FALSE : TRUE;
        }
        else
        {
            SHR_IF_ERR_EXIT(data->polling_func(unit, iter_index, &success));
        }

        if (success)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Control cells enable: Succeeded polling stage - %s after %u times.\n"),
                         data->polling_name, iter_index));
            SHR_EXIT();
        }
        sal_msleep(DNX_FABRIC_CTRL_CELLS_INIT_TIMER_DELAY_MSEC);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U
                 (unit, "Control cells enable: Failed polling stage - %s. Polling function run %u times.\n"),
                 data->polling_name, iter_index));

    if (data->failure_func)
    {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Control cells enable: Run failure function stage - %s.\n"), data->polling_name));
        SHR_IF_ERR_EXIT(data->failure_func(unit));
    }

    if (data->err_on_fail)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Control cells enable: Failed polling stage - %s.\n", data->polling_name);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fabric_ctrl_cells_standalone_state_modify(
    int unit)
{
    dnx_fabric_ctrl_cells_polling_t data;
    SHR_FUNC_INIT_VARS(unit);

    /** On standlone mode skip the polling*/
    if (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_SINGLE_FAP)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_stand_alone_fap_mode_set(unit, TRUE));
    }
    else
    {
        /*
         * Polling on recieving control cells (to determine stand-alone mode)
         */
        sal_memset(&data, 0, sizeof(data));
        data.polling_name = "recieving control cells (to determine stand-alone mode)";
        data.polling_func = dnx_fabric_ctrl_cells_standalone_polling;
        data.failure_func = dnx_fabric_ctrl_cells_standalone_failure;
        data.nof_polling_iters = DNX_FABRIC_CTRL_CELLS_INIT_STANDALONE_ITERATIONS;
        SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_polling(unit, &data));

        /*
         * Polling to see whether control cells init is done
         */
        sal_memset(&data, 0, sizeof(data));
        data.polling_name = "whether control cells init is done";
        data.polling_func = dnx_fabric_ctrl_cells_status_polling;
        data.nof_polling_iters = DNX_FABRIC_CTRL_CELLS_INIT_TIMER_ITERATIONS;
        data.err_on_fail = TRUE; /** Throw error in case polling failed */
        SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_polling(unit, &data));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Calculate rmgr_global HW value.
 *   The period is in HW resolution.
 */
static shr_error_e
dnx_fabric_ctrl_cells_reachability_global_rmgr_period_get(
    int unit,
    uint32 *period)
{
    uint32 nof_clock_cycles = 0;
    dnxcmn_time_t time;
    SHR_FUNC_INIT_VARS(unit);

    time.time_units = DNXCMN_TIME_UNIT_USEC;
    COMPILER_64_SET(time.time, 0, dnx_data_fabric.reachability.full_cycle_period_usec_get(unit));
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time, &nof_clock_cycles));

    *period = UTILEX_DIV_ROUND_DOWN(nof_clock_cycles, dnx_data_fabric.reachability.resolution_get(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Calculate reachability watchdog HW value.
 *   The period is in HW resolution.
 */
static shr_error_e
dnx_fabric_ctrl_cells_reachability_watchdog_period_get(
    int unit,
    uint32 *period)
{
    uint32 nof_clock_cycles = 0;
    dnxcmn_time_t time;
    SHR_FUNC_INIT_VARS(unit);

    time.time_units = DNXCMN_TIME_UNIT_USEC;
    COMPILER_64_SET(time.time, 0, dnx_data_fabric.reachability.watchdog_period_usec_get(unit));
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time, &nof_clock_cycles));

    *period = UTILEX_DIV_ROUND_UP(nof_clock_cycles, dnx_data_fabric.reachability.watchdog_resolution_get(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get which value should be configured in the following field:
 *   MESH_TOPOLOGY_INITr CONFIG_1f
 *
 * \see
 *  * dnx_fabric_ctrl_cells_enable_set
 */
static shr_error_e
dnx_fabric_ctrl_cells_mesh_topology_init_val_get(
    int unit,
    uint32 *field_val)
{
    uint32 entry_handle_id;
    uint32 mesh_config_1 = dnx_data_fabric.mesh_topology.mesh_config_1_get(unit);
    uint32 nof_links = dnx_data_fabric.links.nof_links_get(unit);
    uint32 link_id = 0;
    int is_enabled = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_MESH)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_LINKS, &entry_handle_id));
        for (link_id = 0; link_id < nof_links; ++link_id)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link_id);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, (uint32 *) &is_enabled);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            if (is_enabled)
            {
                break;
            }
        }

        *field_val = (is_enabled) ? mesh_config_1 : 0x0;
    }
    else if (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_SINGLE_FAP)
    {
        *field_val = 0;
    }
    else
    {
        *field_val = mesh_config_1;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable control cells
 * \remark
 *   dnx_fabric_all_ctrl_cells_disable_set and dnx_fabric_all_ctrl_cells_enable_set
 *   are coupled, which means that is something is added/removed from one of them
 *   there is a good chance that it will be needed to be done also to the second
 *   function.
 */
static shr_error_e
dnx_fabric_all_ctrl_cells_enable_set(
    int unit,
    int is_soft_reset)
{
    uint32 entry_handle_id;
    uint32 rate = 0;
    uint32 global_rate = 0;
    uint32 watchdog_threshold = 0;
    uint32 field_val = 0;
    int link_id = 0;
    dnx_fabric_ctrl_cells_polling_t data;
    int nof_enabled_fmacs = 0;
    int fmacs_array[DNX_DATA_MAX_FABRIC_BLOCKS_NOF_INSTANCES_FMAC] = { 0 };
    int fmac_index = 0;
    uint32 nof_links_in_fmac = dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Initialize enabled FMACs array
     */
    /** FMAC Does not work in JR2 emulation */
    if (!(soc_sand_is_emulation_system(unit) && DNX_FABRIC_FMAC_EMULATION_NOT_SUPPORTED(unit)))
    {
        SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_fmac_get(unit, &nof_enabled_fmacs, fmacs_array));
    }

    /*
     * Clear control cells counters
     */
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_counter_clear(unit));

    /*
     * Enable reachability messages
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_REACHABILITY, &entry_handle_id));
    rate = dnx_data_fabric.reachability.gen_period_get(unit);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE, INST_SINGLE, rate);
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_reachability_global_rmgr_period_get(unit, &global_rate));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_RATE, INST_SINGLE, global_rate);
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_reachability_watchdog_period_get(unit, &watchdog_threshold));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WATCHDOG_THRESHOLD, INST_SINGLE, watchdog_threshold);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LINK_STATUS_MASK_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_MSG_UPDATE_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Enable reachability control cells
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_TX_DISCARD, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_REACHABILITY_DISCARD, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Make sure that incoming control cells are not trapped to the CPU
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_RX_DISCARD, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TRAP_ALL, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** On standlone mode skip the polling */
    if (dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_SINGLE_FAP)
    {
        /*
         * Polling until there will be any fabric link up
         */
        sal_memset(&data, 0, sizeof(data));
        data.polling_name = "any fabric link active";
        data.polling_func = dnx_fabric_ctrl_cells_links_enable_polling;
        data.nof_polling_iters = DNX_FABRIC_CTRL_CELLS_INIT_RCH_STATUS_ITERATIONS;
        SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_polling(unit, &data));
    }

    /*
     * Polling until FCR fifo will be empty
     */
    sal_memset(&data, 0, sizeof(data));
    data.polling_name = "whether control cells FCR FIFO is empty";
    data.polling_func = dnx_fabric_ctrl_cells_fifo_empty_polling;
    data.nof_polling_iters = DNX_FABRIC_CTRL_CELLS_INIT_FIFO_EMPTY_ITERATIONS;
    data.err_on_fail = TRUE; /** Throw error in case polling failed */
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_polling(unit, &data));

    /*
     * Set Mesh Topology link configurations
     */
    /** FMAC Does not work in jr2 emulation */
    if (!(soc_sand_is_emulation_system(unit) && DNX_FABRIC_FMAC_EMULATION_NOT_SUPPORTED(unit)))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_LINKS, entry_handle_id));
        for (fmac_index = 0; fmac_index < nof_enabled_fmacs; ++fmac_index)
        {
            int base_link_id = fmacs_array[fmac_index] * nof_links_in_fmac;
            for (link_id = base_link_id; link_id < base_link_id + nof_links_in_fmac; ++link_id)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link_id);
                if (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE)
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_0, INST_SINGLE, 1);
                }
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_2, INST_SINGLE, 1);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }

    /*
     * Set Mesh Topology configurations
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE, 0x14);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Do not config as part of soft reset sequance */
    if (!is_soft_reset)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FAST, INST_SINGLE,
                                     dnx_data_fabric.debug.mesh_topology_fast_get(unit) ? 0x1 : 0x0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONFIG_6, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_INIT, entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_mesh_topology_init_val_get(unit, &field_val));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_CONFIG_1, INST_SINGLE, field_val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Determine whether the device is standalone or not according to
     * received control cells
     */
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_standalone_state_modify(unit));

    if (!is_soft_reset)
    {
        /** Do not config as part of soft reset sequance */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FAST, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONFIG_5, INST_SINGLE, 0x7);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Start sending control cells
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_TX_DISCARD, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_FLOW_STATUS_DISCARD, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_CREDIT_DISCARD, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Enable LLFC control cells
     */
    /** FMAC Does not work in JR2 emulation */
    if (!(soc_sand_is_emulation_system(unit) && DNX_FABRIC_FMAC_EMULATION_NOT_SUPPORTED(unit)))
    {   /* FMAC Does not work in emulation */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_LLFC, entry_handle_id));
        for (fmac_index = 0; fmac_index < nof_enabled_fmacs; ++fmac_index)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC, fmacs_array[fmac_index]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        sal_msleep(16);
    }

    /*
     * Disable packets discard
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_TX_DISCARD, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ALL_PACKETS_DISCARD, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Disable control cells
 * \remark
 *   dnx_fabric_all_ctrl_cells_disable_set and dnx_fabric_all_ctrl_cells_enable_set
 *   are coupled, which means that is something is added/removed from one of them
 *   there is a good chance that it will be needed to be done also to the second
 *   function.
 *   Note also that the sequence is essentialy backwards of the 'enable' function.
 */
static shr_error_e
dnx_fabric_all_ctrl_cells_disable_set(
    int unit,
    int is_soft_reset)
{
    uint32 entry_handle_id;
    dnx_fabric_ctrl_cells_polling_t data;
    int link_id = 0;
    int nof_enabled_fmacs = 0;
    int fmacs_array[DNX_DATA_MAX_FABRIC_BLOCKS_NOF_INSTANCES_FMAC] = { 0 };
    int fmac_index = 0;
    uint32 nof_links_in_fmac = dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Initialize enabled FMACs array
     */
    /** FMAC Does not work in JR2 emulation */
    if (!(soc_sand_is_emulation_system(unit) && DNX_FABRIC_FMAC_EMULATION_NOT_SUPPORTED(unit)))
    {
        SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_fmac_get(unit, &nof_enabled_fmacs, fmacs_array));
    }

    /*
     * Clear control cells counters
     */
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_counter_clear(unit));

    /*
     * Enable packets discard
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_TX_DISCARD, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ALL_PACKETS_DISCARD, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Disable LLFC control cells
     */
    /** FMAC Does not work in JR2 emulation */
    if (!(soc_sand_is_emulation_system(unit) && DNX_FABRIC_FMAC_EMULATION_NOT_SUPPORTED(unit)))
    {   /* FMAC Does not work in emulation */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_LLFC, entry_handle_id));
        for (fmac_index = 0; fmac_index < nof_enabled_fmacs; ++fmac_index)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC, fmacs_array[fmac_index]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 0);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        sal_msleep(16);
    }

    /*
     * Discard all control cells
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_TX_DISCARD, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_FLOW_STATUS_DISCARD, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_CREDIT_DISCARD, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_REACHABILITY_DISCARD, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Trap incoming control cells to the CPU
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_RX_DISCARD, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TRAP_ALL, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Polling until all fabric links will be down
     */
    sal_memset(&data, 0, sizeof(data));
    data.polling_name = "all fabric links are down";
    data.polling_func = dnx_fabric_ctrl_cells_all_links_disable_polling;
    data.nof_polling_iters = DNX_FABRIC_CTRL_CELLS_INIT_TIMER_ITERATIONS;
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_polling(unit, &data));

    /*
     * Clear Mesh Topology configurations
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONFIG_5, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_INIT, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_CONFIG_1, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Do not config as part of soft reset sequance */
    if (!is_soft_reset)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONFIG_6, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Clear Mesh Topology link configurations
     */
    /** FMAC Does not work in emulation */
    if (!(soc_sand_is_emulation_system(unit) && DNX_FABRIC_FMAC_EMULATION_NOT_SUPPORTED(unit)))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_LINKS, entry_handle_id));
        for (fmac_index = 0; fmac_index < nof_enabled_fmacs; ++fmac_index)
        {
            int base_link_id = fmacs_array[fmac_index] * nof_links_in_fmac;
            for (link_id = base_link_id; link_id < base_link_id + nof_links_in_fmac; ++link_id)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link_id);
                if (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE)
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_0, INST_SINGLE, 0);
                }
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_2, INST_SINGLE, 0);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }

    /*
     * Set Mesh Topology configurations
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Disable reachability messages
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_REACHABILITY, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_RATE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WATCHDOG_THRESHOLD, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LINK_STATUS_MASK_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_MSG_UPDATE_ENABLE, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_fabric_ctrl_cells_enable_set(
    int unit,
    int enable,
    int is_soft_reset)
{
    SHR_FUNC_INIT_VARS(unit);

    if (enable)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_all_ctrl_cells_enable_set(unit, is_soft_reset));
    }
    else
    {
        int is_control_cells_enabled = 0;

        SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_get(unit, &is_control_cells_enabled));

        /*
         * Disable control cells only if they are enabled.
         * No need to disable them if they are already disabled (and it also takes too
         * much time to double disable the control cells).
         * Need to disable them also on init sequence, thus initialize the control
         * cells as disabled.
         */
        if (is_control_cells_enabled || !dnx_init_is_init_done_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_fabric_all_ctrl_cells_disable_set(unit, is_soft_reset));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_fabric_ctrl_cells_enable_get(
    int unit,
    int *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     *  Just read one register as a representative
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CONFIG_6, INST_SINGLE, (uint32 *) enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify inpurt parameters of bcm_dnx_fabric_reachability_status_get.
 */
static shr_error_e
dnx_fabric_reachability_status_get_verify(
    int unit,
    int moduleid,
    int links_max,
    uint32 *links_array,
    int *links_count)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(moduleid, 0, dnx_data_device.general.nof_faps_get(unit) - 1, _SHR_E_PARAM,
                     "modid out of bound.\n");

    if (!dnx_data_fabric.links.nof_links_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "bcm_fabric_reachability_status_get is not supported for devices without fabric links\n");
    }

    if (links_max < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "links_max must be a positive number\n");
    }

    SHR_NULL_CHECK(links_array, _SHR_E_PARAM, "links_array");
    SHR_NULL_CHECK(links_count, _SHR_E_PARAM, "links_count");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get reachability status.
 * \param [in] unit -
 *   The unit number.
 * \param [in] moduleid -
 *   Module to check reachbility to.
 * \param [in] links_max -
 *   Max size of links_array.
 * \param [out] links_array -
 *   Links which moduleid is reachable through.
 * \param [out] links_count -
 *   Size of links_array.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_reachability_status_get(
    int unit,
    int moduleid,
    int links_max,
    uint32 *links_array,
    int *links_count)
{
    uint32 reachability_status_handle_id;
    uint32 link_active_handle_id;
    int link_id;
    bcm_port_t logical_port;
    bcm_pbmp_t fabric_ports_bitmap;
    uint32 is_fap_reachable;
    uint32 is_link_active;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get all links from which moduleid is reachable
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_REACHABILITY_STATUS, &reachability_status_handle_id));
    dbal_entry_key_field32_set(unit, reachability_status_handle_id, DBAL_FIELD_FAP_ID, moduleid);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, reachability_status_handle_id, DBAL_GET_ALL_FIELDS));

    /*
     * Get all active fabric links
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_LINK_ACTIVE, &link_active_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, link_active_handle_id, DBAL_GET_ALL_FIELDS));

    *links_count = 0;

    BCM_PBMP_CLEAR(fabric_ports_bitmap);
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FABRIC, 0, &fabric_ports_bitmap));
    BCM_PBMP_ITER(fabric_ports_bitmap, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &link_id));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, reachability_status_handle_id, DBAL_FIELD_IS_REACHABLE, link_id, &is_fap_reachable));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, link_active_handle_id, DBAL_FIELD_IS_LINK_ACTIVE, link_id, &is_link_active));

        if (is_fap_reachable && is_link_active)
        {
            if (*links_count >= links_max)
            {
                SHR_ERR_EXIT(_SHR_E_FULL, "links_array is too small");
            }

            links_array[*links_count] = link_id;
            (*links_count)++;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get reachability status.
 * \param [in] unit -
 *   The unit number.
 * \param [in] moduleid -
 *   Module to check reachbility to.
 * \param [in] links_max -
 *   Max size of links_array.
 * \param [out] links_array -
 *   Links which moduleid is reachable through.
 * \param [out] links_count -
 *   Size of links_array.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
int
bcm_dnx_fabric_reachability_status_get(
    int unit,
    int moduleid,
    int links_max,
    uint32 *links_array,
    int *links_count)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_fabric_reachability_status_get_verify
                          (unit, moduleid, links_max, links_array, links_count));

    SHR_IF_ERR_EXIT(dnx_fabric_reachability_status_get(unit, moduleid, links_max, links_array, links_count));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate the params of bcm_dnx_fabric_link_connectivity_status_get
 */
static shr_error_e
dnx_fabric_link_connectivity_status_get_verify(
    int unit,
    int link_partner_max,
    bcm_fabric_link_connectivity_t * link_partner_array,
    int *link_partner_count)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_fabric.links.nof_links_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "bcm_fabric_link_connectivity_status_get is not supported for devices without fabric links\n");
    }

    if (link_partner_max < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "link_partner_max must be a positive number\n");
    }

    SHR_NULL_CHECK(link_partner_array, _SHR_E_PARAM, "link_partner_array");
    SHR_NULL_CHECK(link_partner_count, _SHR_E_PARAM, "link_partner_count");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get link's topology connectivity status.
 * \param [in] unit -
 *   The unit number.
 * \param [in] link_id -
 *   The fabric link index.
 * \param [out] link_connectivity -
 *   'link_id' topology connectivity status.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * bcm_dnx_fabric_topology_status_connectivity_get.
 */
static shr_error_e
dnx_fabric_topology_status_connectivity_get(
    int unit,
    int link_id,
    bcm_fabric_link_connectivity_t * link_connectivity)
{
    uint32 connectivity_status_handle_id;
    uint32 link_active_handle_id;
    uint32 connected_device_level;
    uint32 is_link_active;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(link_connectivity, 0, sizeof(bcm_fabric_link_connectivity_t));

    /*
     * Get is link active
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_LINK_ACTIVE, &link_active_handle_id));
    dbal_value_field32_request(unit, link_active_handle_id, DBAL_FIELD_IS_LINK_ACTIVE, link_id, &is_link_active);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, link_active_handle_id, DBAL_COMMIT));

    if (is_link_active)
    {
        /*
         * Get connectivity information
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_CONNECTIVITY_STATUS, &connectivity_status_handle_id));
        dbal_entry_key_field32_set(unit, connectivity_status_handle_id, DBAL_FIELD_FABRIC_LINK_ID, link_id);
        dbal_value_field32_request(unit, connectivity_status_handle_id, DBAL_FIELD_CONNECTED_DEVICE_FAP_ID, INST_SINGLE,
                                   &link_connectivity->module_id);
        dbal_value_field32_request(unit, connectivity_status_handle_id, DBAL_FIELD_CONNECTED_DEVICE_LEVEL, INST_SINGLE,
                                   &connected_device_level);
        dbal_value_field32_request(unit, connectivity_status_handle_id, DBAL_FIELD_CONNECTED_DEVICE_LINK_ID,
                                   INST_SINGLE, &link_connectivity->link_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, connectivity_status_handle_id, DBAL_COMMIT));

#ifdef PLISIM
        if (SAL_BOOT_PLISIM)
        {
            connected_device_level = 0x3;       /* To be FE2 */
        }
#endif

        /*
         * 3'bx0x => FOP
         * 3'b010 => FE3,
         * 3'bX11 => FE2,
         * 3'b110 => FE1
         */

        if ((connected_device_level & 0x2) == 0)
        {
            link_connectivity->device_type = bcmFabricDeviceTypeFAP;
        }
        else if (connected_device_level == 0x2)
        {
            link_connectivity->device_type = bcmFabricDeviceTypeFE13;
        }
        else if ((connected_device_level & 0x3) == 0x3)
        {
            link_connectivity->device_type = bcmFabricDeviceTypeFE2;
        }
        else if ((connected_device_level & 0x6) == 0x6)
        {
            link_connectivity->device_type = bcmFabricDeviceTypeFE13;
        }
        else
        {
            link_connectivity->device_type = bcmFabricDeviceTypeUnknown;
        }
    }
    else
    {
        link_connectivity->link_id = BCM_FABRIC_LINK_NO_CONNECTIVITY;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get fabric links topology connectivity status.
 * \param [in] unit -
 *   The unit number.
 * \param [in] link_partner_max -
 *   The size of 'link_partner_array'.
 * \param [out] link_partner_array -
 *   Array with all fabric links topology connectivity status.
 * \param [out] link_partner_count -
 *   How many entries were actually filled in 'link_partner_array'.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
int
bcm_dnx_fabric_link_connectivity_status_get(
    int unit,
    int link_partner_max,
    bcm_fabric_link_connectivity_t * link_partner_array,
    int *link_partner_count)
{
    bcm_port_t logical_port;
    bcm_pbmp_t fabric_ports_bitmap;
    int link_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_fabric_link_connectivity_status_get_verify
                          (unit, link_partner_max, link_partner_array, link_partner_count));

    *link_partner_count = 0;

    BCM_PBMP_CLEAR(fabric_ports_bitmap);
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FABRIC, 0, &fabric_ports_bitmap));
    BCM_PBMP_ITER(fabric_ports_bitmap, logical_port)
    {
        if (*link_partner_count >= link_partner_max)
        {
            *link_partner_count = 0;
            SHR_ERR_EXIT(_SHR_E_PARAM, "link_partner_max %d is too small", link_partner_max);
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &link_id));

        SHR_IF_ERR_EXIT(dnx_fabric_topology_status_connectivity_get
                        (unit, link_id, &link_partner_array[*link_partner_count]));

        (*link_partner_count)++;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate bcm_dnx_fabric_link_connectivity_status_single_get parameters.
 * \param [in] unit -
 *   The unit id.
 * \param [in] link_id -
 *   Fabric port.
 * \param [out] link_partner_info -
 *   Information about the link partner
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Note that the name of link_id is confusing. It is actually a port,
 *     not a link
 * \see
 *   * bcm_dnx_fabric_link_connectivity_status_single_get.
 */
static shr_error_e
dnx_fabric_link_connectivity_status_single_get_verify(
    int unit,
    bcm_port_t link_id,
    bcm_fabric_link_connectivity_t * link_partner_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_fabric.links.nof_links_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "bcm_fabric_link_connectivity_status_get is not supported for devices without fabric links\n");
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_type_verify(unit, link_id, DNX_ALGO_PORT_TYPE_FABRIC));

    SHR_NULL_CHECK(link_partner_info, _SHR_E_PARAM, "link_partner_info");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get a single fabric link topology connectivity status.
 * \param [in] unit -
 *   The unit id.
 * \param [in] link_id -
 *   Fabric port.
 * \param [out] link_partner_info -
 *   Information about the link partner
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Note that the name of link_id is confusing. It is actually a port,
 *     not a link
 * \see
 *   * None.
 */
int
bcm_dnx_fabric_link_connectivity_status_single_get(
    int unit,
    bcm_port_t link_id,
    bcm_fabric_link_connectivity_t * link_partner_info)
{
    int link_index;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_fabric_link_connectivity_status_single_get_verify(unit, link_id, link_partner_info));

    /*
     * Conver fabric port into link index.
     * The name of the param 'link_id' is confusing - it actually means
     * fabric port and not fabric link.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, link_id, &link_index));

    SHR_IF_ERR_EXIT(dnx_fabric_topology_status_connectivity_get(unit, link_index, link_partner_info));

exit:
    SHR_FUNC_EXIT;
}

/*
 * } 
 */
