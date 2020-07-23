/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file algo_flexe_core.c
 *
 *  FlexE core resource manager.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_flexe_core_access.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/flexe_core/algo_flexe_core.h>
#include <bcm_int/dnx/port/flexe/flexe_core.h>

/*
 * }
 */
/*
 * Macros
 * {
 */
/*
 * }
 */
/*
 * Defines
 * {
 */
/*
 * }
 */

/**
 * \brief - Create allocation manager.
 */
static shr_error_e
dnx_algo_flexe_core_alloc_manager_create(
    int unit)
{
    dnx_algo_res_create_data_t create_data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create allocate manager
     */
    sal_memset(&create_data, 0, sizeof(create_data));
    /*
     * FlexE core instance allocation manager create
     */
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_nif.flexe.nof_flexe_instances_get(unit);
    sal_strncpy_s(create_data.name, "FLEXE_CORE_INSTANCE_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.instance.create(unit, &create_data, NULL));

    /*
     * FlexE core LPHY allocation manager create
     */
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_nif.flexe.nof_flexe_lphys_get(unit);
    sal_strncpy_s(create_data.name, "FLEXE_CORE_LPHY_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.lphy.create(unit, &create_data, NULL));

    /*
     * FlexE core MAC timeslots allocation manager create
     */
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_nif.flexe.nof_mac_timeslots_get(unit);
    sal_strncpy_s(create_data.name, "FLEXE_CORE_MAC_TIMESLOTS_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.mac_timeslot.create(unit, &create_data, NULL));

    /*
     * FlexE core SAR timeslots allocation manager create
     */
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_nif.flexe.nof_sar_timeslots_get(unit);
    sal_strncpy_s(create_data.name, "FLEXE_CORE_SAR_RX_TIMESLOTS_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.sar_rx_timeslot.create(unit, &create_data, NULL));
    sal_strncpy_s(create_data.name, "FLEXE_CORE_SAR_TX_TIMESLOTS_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.sar_tx_timeslot.create(unit, &create_data, NULL));

    /*
     * FlexE core rate adapter timeslots allocation manager create
     */
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_nif.flexe.max_nof_slots_get(unit);
    sal_strncpy_s(create_data.name, "FLEXE_CORE_RATEADPT_TIMESLOTS_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.rateadpt_timeslot.create(unit, &create_data, NULL));

exit:
    SHR_FUNC_EXIT;
}
#if defined(INCLUDE_FLEXE) || defined(INCLUDE_FLEXE_DBG)
/**
 * \brief - Select the flexe core driver.
 */
static shr_error_e
dnx_algo_flexe_core_driver_mode_set(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_dev_init.general.flexe_core_drv_select_get(unit))
    {
#if defined(INCLUDE_FLEXE)
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.drv_type.set(unit, flexe_coreDispatchTypeFlexe_std));
#else
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "STD driver is not compiled!\n");
#endif
    }
    else
    {
#if defined(INCLUDE_FLEXE_DBG)
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.drv_type.set(unit, flexe_coreDispatchTypeFlexe_dbg));
#else
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "DBG driver is not compiled!\n");
#endif
    }
exit:
    SHR_FUNC_EXIT;
}
#endif
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_init(
    int unit)
{
    sw_state_ll_init_info_t port_init_info;
    sw_state_ll_init_info_t client_init_info;
    int group;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create SW State instance
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.init(unit));

    /*
     * Allocate SW STATE DBs
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.alloc(unit));

    sal_memset(&port_init_info, 0, sizeof(port_init_info));
    port_init_info.max_nof_elements = dnx_data_nif.flexe.nof_pcs_get(unit) + 1;
    port_init_info.expected_nof_elements = dnx_data_nif.flexe.nof_pcs_get(unit) + 1;
    port_init_info.key_cmp_cb_name = "sw_state_sorted_list_cmp32";

    sal_memset(&client_init_info, 0, sizeof(client_init_info));
    client_init_info.max_nof_elements = dnx_data_nif.flexe.nof_flexe_clients_get(unit) + 1;
    client_init_info.expected_nof_elements = dnx_data_nif.flexe.nof_flexe_clients_get(unit) + 1;

    /*
     * Init flexe_ports sorted link list in group
     */
    for (group = 0; group < dnx_data_nif.flexe.nof_groups_get(unit); ++group)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.flexe_ports.create_empty(unit, group, &port_init_info));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.clients_rx.create_empty(unit, group, &client_init_info));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.clients_tx.create_empty(unit, group, &client_init_info));
    }

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.mac_client.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.sar_client.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.flexe_client.alloc(unit));

    /** Alloc Manager create */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_alloc_manager_create(unit));

#if defined(INCLUDE_FLEXE) || defined(INCLUDE_FLEXE_DBG)
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_driver_mode_set(unit));
#endif

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_instance_alloc(
    int unit,
    int speed,
    int start_pos,
    int end_pos,
    int *instance)
{
    int nof_instances;
    int index, ii;
    int inst_to_allocate = 0;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    nof_instances = FLEXE_PHY_NOF_INSTANCES(speed);

    start_pos = MAX(start_pos, 0);
    end_pos = MIN(end_pos, dnx_data_nif.flexe.nof_flexe_instances_get(unit) - 1);

    /*
     * Align start to nof_instances
     */
    start_pos = ((start_pos + nof_instances - 1) / nof_instances) * nof_instances;

    /*
     * Allocate consequent nof_instances aligned to nof_instances.
     * The first available nof_instances are returned.
     */
    for (index = start_pos; index <= end_pos;)
    {
        for (ii = 0; ii < nof_instances; ++ii)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.
                            instance.is_allocated(unit, index + ii, &is_allocated));
            if (is_allocated)
            {
                /*
                 * Pont to the position after index+ii
                 */
                index += ii + 1;
                break;
            }
        }

        if (is_allocated == 0)
        {
            for (ii = 0; ii < nof_instances; ++ii)
            {
                inst_to_allocate = index + ii;
                SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.instance.
                                allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL, &inst_to_allocate));
            }

            *instance = index;
            break;
        }
    }

    if (is_allocated != 0)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Failed to allocate %d consequent FlexE instances", nof_instances);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_instance_free(
    int unit,
    int speed,
    int instance)
{
    int nof_instances;
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    nof_instances = FLEXE_PHY_NOF_INSTANCES(speed);

    for (ii = instance; ii < (instance + nof_instances); ++ii)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.instance.free_single(unit, ii));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_lphy_alloc(
    int unit,
    int flags,
    int speed,
    int start_pos,
    int end_pos,
    SHR_BITDCL * lphy_bitmap)
{
    int nof_lphys;
    int nof_free_lphys = 0;
    int nof_allocated_lphys = 0;
    int index;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    nof_lphys = speed / 50000;

    start_pos = MAX(start_pos, 0);
    end_pos = MIN(end_pos, dnx_data_nif.flexe.nof_flexe_lphys_get(unit) - 1);

    /*
     * Check if there are enought free LPHYs in the range of [start_pos, end_pos].
     */
    for (index = start_pos; index <= end_pos; ++index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.lphy.is_allocated(unit, index, &is_allocated));

        if (is_allocated == 0)
        {
            ++nof_free_lphys;
        }
    }

    if (nof_lphys > nof_free_lphys)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Number of free LPHYs(%d) less than required(%d)", nof_free_lphys, nof_lphys);
    }

    /*
     * At this point we are sure that we can allocate the required number of LPHYs from the range.
     */
    for (index = start_pos; index <= end_pos; ++index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.lphy.is_allocated(unit, index, &is_allocated));

        if (is_allocated == 0)
        {
            if (!(flags & DNX_ALGO_FLEXE_CORE_LPHY_ALLOC_CHECK_ONLY))
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.lphy.
                                allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL, &index));
            }

            SHR_BITSET(lphy_bitmap, index);

            ++nof_allocated_lphys;
            if (nof_allocated_lphys == nof_lphys)
            {
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_lphy_free(
    int unit,
    SHR_BITDCL * lphy_bitmap)
{
    int lphy;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Free LPHY according to bitmap.
     */
    SHR_BIT_ITER(lphy_bitmap, dnx_data_nif.flexe.nof_flexe_lphys_get(unit), lphy)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.lphy.free_single(unit, lphy));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_mac_ts_alloc(
    int unit,
    int nof_timeslots,
    SHR_BITDCL * ts_bitmap)
{
    int nof_free_timeslots = 0;
    int timeslot_allocated = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.
                    mac_timeslot.nof_free_elements_get(unit, &nof_free_timeslots));

    if (nof_timeslots > nof_free_timeslots)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Number of free LPHYs(%d) less than required(%d)",
                     nof_free_timeslots, nof_timeslots);
    }

    for (index = 0; index < nof_timeslots; ++index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.
                        mac_timeslot.allocate_single(unit, 0, NULL, &timeslot_allocated));
        SHR_BITSET(ts_bitmap, timeslot_allocated);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_mac_ts_free(
    int unit,
    SHR_BITDCL * ts_bitmap)
{
    int timeslot;

    SHR_FUNC_INIT_VARS(unit);

    SHR_BIT_ITER(ts_bitmap, dnx_data_nif.flexe.nof_mac_timeslots_get(unit), timeslot)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.mac_timeslot.free_single(unit, timeslot));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_sar_rx_ts_alloc(
    int unit,
    int nof_timeslots,
    SHR_BITDCL * ts_bitmap)
{
    int nof_free_timeslots = 0;
    int timeslot_allocated = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.
                    sar_rx_timeslot.nof_free_elements_get(unit, &nof_free_timeslots));

    if (nof_timeslots > nof_free_timeslots)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Number of free LPHYs(%d) less than required(%d)",
                     nof_free_timeslots, nof_timeslots);
    }

    for (index = 0; index < nof_timeslots; ++index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.
                        sar_rx_timeslot.allocate_single(unit, 0, NULL, &timeslot_allocated));
        SHR_BITSET(ts_bitmap, timeslot_allocated);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_sar_rx_ts_free(
    int unit,
    SHR_BITDCL * ts_bitmap)
{
    int timeslot;

    SHR_FUNC_INIT_VARS(unit);

    SHR_BIT_ITER(ts_bitmap, dnx_data_nif.flexe.nof_sar_timeslots_get(unit), timeslot)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.sar_rx_timeslot.free_single(unit, timeslot));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_sar_tx_ts_alloc(
    int unit,
    int nof_timeslots,
    SHR_BITDCL * ts_bitmap)
{
    int nof_free_timeslots = 0;
    int timeslot_allocated = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.
                    sar_tx_timeslot.nof_free_elements_get(unit, &nof_free_timeslots));

    if (nof_timeslots > nof_free_timeslots)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Number of free LPHYs(%d) less than required(%d)",
                     nof_free_timeslots, nof_timeslots);
    }

    for (index = 0; index < nof_timeslots; ++index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.
                        sar_tx_timeslot.allocate_single(unit, 0, NULL, &timeslot_allocated));
        SHR_BITSET(ts_bitmap, timeslot_allocated);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_sar_tx_ts_free(
    int unit,
    SHR_BITDCL * ts_bitmap)
{
    int timeslot;

    SHR_FUNC_INIT_VARS(unit);

    SHR_BIT_ITER(ts_bitmap, dnx_data_nif.flexe.nof_sar_timeslots_get(unit), timeslot)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.sar_tx_timeslot.free_single(unit, timeslot));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_rateadpt_ts_alloc(
    int unit,
    int nof_timeslots,
    SHR_BITDCL * ts_bitmap)
{
    int nof_free_timeslots = 0;
    int timeslot_allocated = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.
                    rateadpt_timeslot.nof_free_elements_get(unit, &nof_free_timeslots));

    if (nof_timeslots > nof_free_timeslots)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Number of free rate adapter timeslots(%d) less than required(%d)",
                     nof_free_timeslots, nof_timeslots);
    }

    for (index = 0; index < nof_timeslots; ++index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.
                        rateadpt_timeslot.allocate_single(unit, 0, NULL, &timeslot_allocated));
        SHR_BITSET(ts_bitmap, timeslot_allocated);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_rateadpt_ts_free(
    int unit,
    SHR_BITDCL * ts_bitmap)
{
    int timeslot;

    SHR_FUNC_INIT_VARS(unit);

    SHR_BIT_ITER(ts_bitmap, dnx_data_nif.flexe.max_nof_slots_get(unit), timeslot)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.rateadpt_timeslot.free_single(unit, timeslot));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

#undef _ERR_MSG_MODULE_NAME
