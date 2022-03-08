/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/** \file algo_flexe_core.c
 *
 *  FlexE core resource manager.
 *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*
 * Include files.
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_flexe_core_access.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/flexe_core/algo_flexe_core.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
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

    if ((dnx_data_nif.framer.gen_get(unit) == DNX_ALGO_FLEXE_GEN_1)
        && dnx_data_dev_init.general.flexe_core_drv_select_get(unit))
    {
        /*
         * FlexE core Mux LPHY allocation manager create
         */
        create_data.first_element = 0;
        create_data.nof_elements = dnx_data_nif.flexe.nof_flexe_lphys_get(unit);
        sal_strncpy_s(create_data.name, "FLEXE_CORE_MUX_LPHY_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.mux_lphy.create(unit, &create_data, NULL));

        /*
         * FlexE core Demux LPHY allocation manager create
         */
        create_data.first_element = 0;
        create_data.nof_elements = dnx_data_nif.flexe.nof_flexe_lphys_get(unit);
        sal_strncpy_s(create_data.name, "FLEXE_CORE_DEMUX_LPHY_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.demux_lphy.create(unit, &create_data, NULL));

        /*
         * FlexE core MAC timeslots allocation manager create
         */
        create_data.first_element = 0;
        create_data.nof_elements = dnx_data_nif.mac_client.nof_mac_timeslots_get(unit);
        sal_strncpy_s(create_data.name, "FLEXE_CORE_MAC_TIMESLOTS_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.mac_timeslot.create(unit, &create_data, NULL));

        /*
         * FlexE core SAR timeslots allocation manager create
         */
        create_data.first_element = 0;
        create_data.nof_elements = dnx_data_nif.sar_client.nof_sar_timeslots_get(unit);
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
    }
    /*
     * FlexE core instance allocation manager create
     */
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_nif.flexe.nof_flexe_instances_get(unit);
    sal_strncpy_s(create_data.name, "FLEXE_CORE_INSTANCE_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.instance.create(unit, &create_data, NULL));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Select the flexe core driver.
 */
static shr_error_e
dnx_algo_flexe_core_driver_mode_set(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_nif.framer.gen_get(unit) == DNX_ALGO_FLEXE_GEN_1)
    {
        if (dnx_data_dev_init.general.flexe_core_drv_select_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.drv_type.set(unit, flexe_coreDispatchTypeFlexe_std));
        }
        else
        {
#if defined(INCLUDE_FLEXE_DBG)
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.drv_type.set(unit, flexe_coreDispatchTypeFlexe_dbg));
#else
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "DBG driver is not compiled!\n");
#endif
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.drv_type.set(unit, flexe_coreDispatchTypeFlexe_std_gen2));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_port_db_init(
    int unit,
    int flexe_core_port)
{
    dnx_algo_flexe_core_port_info_t flexe_core_port_db;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&flexe_core_port_db, 0, sizeof(dnx_algo_flexe_core_port_info_t));
    flexe_core_port_db.logical_phy_id = -1;
    flexe_core_port_db.group = -1;
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.set(unit, flexe_core_port, &flexe_core_port_db));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_init(
    int unit)
{
    sw_state_ll_init_info_t port_init_info;
    sw_state_ll_init_info_t client_init_info;
    int group, flexe_core_port;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create SW State instance
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.init(unit));

    /*
     * Allocate SW STATE DBs
     */
    if ((dnx_data_nif.framer.gen_get(unit) == DNX_ALGO_FLEXE_GEN_1)
        && dnx_data_dev_init.general.flexe_core_drv_select_get(unit))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.alloc(unit));

        sal_memset(&port_init_info, 0, sizeof(port_init_info));
        port_init_info.max_nof_elements = dnx_data_nif.flexe.nof_pcs_get(unit) + 1;
        port_init_info.expected_nof_elements = dnx_data_nif.flexe.nof_pcs_get(unit) + 1;
        port_init_info.key_cmp_cb_name = "sw_state_sorted_list_cmp32";

        sal_memset(&client_init_info, 0, sizeof(client_init_info));
        client_init_info.max_nof_elements = dnx_data_nif.flexe.nof_clients_get(unit) + 1;
        client_init_info.expected_nof_elements = dnx_data_nif.flexe.nof_clients_get(unit) + 1;

        /*
         * Init flexe_ports sorted link list in group
         */
        for (group = 0; group < dnx_data_nif.flexe.nof_groups_get(unit); ++group)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.flexe_ports.create_empty(unit, group, &port_init_info));
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.clients_rx.create_empty(unit, group, &client_init_info));
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.clients_tx.create_empty(unit, group, &client_init_info));
        }
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.mac_client.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.sar_client.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.flexe_client.alloc(unit));

    /** Init flexe core port DB */
    for (flexe_core_port = 0; flexe_core_port < dnx_data_nif.flexe.nof_flexe_core_ports_get(unit); ++flexe_core_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_db_init(unit, flexe_core_port));
    }
    /** Alloc Manager create */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_alloc_manager_create(unit));

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_driver_mode_set(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_mux_lphy_alloc(
    int unit,
    int speed,
    SHR_BITDCL * lphy_bitmap)
{
    int nof_lphys;
    int nof_free_lphys = 0;
    int nof_allocated_lphys = 0;
    int index;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_BITCLR_RANGE(lphy_bitmap, 0, DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_LPHYS);

    nof_lphys = speed / dnx_data_nif.flexe.phy_speed_granularity_get(unit);
    /*
     * Check if there are enought free LPHYs in the range of [start_pos, end_pos].
     */
    for (index = 0; index < dnx_data_nif.flexe.nof_flexe_lphys_get(unit); ++index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.mux_lphy.is_allocated(unit, index, &is_allocated));

        if (is_allocated == 0)
        {
            ++nof_free_lphys;
        }
    }
    if (nof_lphys > nof_free_lphys)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Number of free Mux LPHYs(%d) less than required(%d)", nof_free_lphys, nof_lphys);
    }
    /*
     * At this point we are sure that we can allocate the required number of LPHYs from the range.
     */
    for (index = 0; index < dnx_data_nif.flexe.nof_flexe_lphys_get(unit); ++index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.mux_lphy.is_allocated(unit, index, &is_allocated));

        if (is_allocated == 0)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.mux_lphy.
                            allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL, &index));
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
dnx_algo_flexe_core_mux_lphy_free(
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
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.mux_lphy.free_single(unit, lphy, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_demux_lphy_alloc(
    int unit,
    int flags,
    int speed,
    int start_pos,
    int end_pos,
    SHR_BITDCL * lphy_bitmap,
    int *is_success)
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
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.demux_lphy.is_allocated(unit, index, &is_allocated));

        if (is_allocated == 0)
        {
            ++nof_free_lphys;
        }
    }

    if (flags & DNX_ALGO_FLEXE_CORE_LPHY_ALLOC_CHECK_ONLY)
    {
        *is_success = (nof_lphys > nof_free_lphys) ? 0 : 1;
    }
    else
    {
        if (nof_lphys > nof_free_lphys)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Number of free LPHYs(%d) less than required(%d)", nof_free_lphys, nof_lphys);
        }
        SHR_BITCLR_RANGE(lphy_bitmap, 0, DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_LPHYS);
        /*
         * At this point we are sure that we can allocate the required number of LPHYs from the range.
         */
        for (index = start_pos; index <= end_pos; ++index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.demux_lphy.is_allocated(unit, index, &is_allocated));

            if (is_allocated == 0)
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.demux_lphy.
                                allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL, &index));
                SHR_BITSET(lphy_bitmap, index);

                ++nof_allocated_lphys;
                if (nof_allocated_lphys == nof_lphys)
                {
                    break;
                }
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
dnx_algo_flexe_core_demux_lphy_free(
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
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.demux_lphy.free_single(unit, lphy, NULL));
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

    SHR_BIT_ITER(ts_bitmap, dnx_data_nif.mac_client.nof_mac_timeslots_get(unit), timeslot)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.mac_timeslot.free_single(unit, timeslot, NULL));
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

    SHR_BIT_ITER(ts_bitmap, dnx_data_nif.sar_client.nof_sar_timeslots_get(unit), timeslot)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.sar_rx_timeslot.free_single(unit, timeslot, NULL));
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

    SHR_BIT_ITER(ts_bitmap, dnx_data_nif.sar_client.nof_sar_timeslots_get(unit), timeslot)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.sar_tx_timeslot.free_single(unit, timeslot, NULL));
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
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.rateadpt_timeslot.free_single(unit, timeslot, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_port_to_instance_id_get(
    int unit,
    int flexe_core_port,
    int *instance_id)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify flexe core port */
    if (flexe_core_port < 0 || flexe_core_port >= dnx_data_nif.flexe.nof_flexe_core_ports_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE core port index is out of range.\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, instance_id));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_port_to_instance_id_set(
    int unit,
    int flexe_core_port,
    int instance_id)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify flexe core port */
    if (flexe_core_port < 0 || flexe_core_port >= dnx_data_nif.flexe.nof_flexe_core_ports_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE core port index is out of range.\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.set(unit, flexe_core_port, instance_id));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_port_nof_instances_set(
    int unit,
    int flexe_core_port,
    int nof_instances)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify flexe core port */
    if (flexe_core_port < 0 || flexe_core_port >= dnx_data_nif.flexe.nof_flexe_core_ports_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE core port index is out of range.\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.nof_instances.set(unit, flexe_core_port, nof_instances));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_port_nof_instances_get(
    int unit,
    int flexe_core_port,
    int *nof_instances)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify flexe core port */
    if (flexe_core_port < 0 || flexe_core_port >= dnx_data_nif.flexe.nof_flexe_core_ports_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE core port index is out of range.\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.nof_instances.get(unit, flexe_core_port, nof_instances));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_instance_id_alloc(
    int unit,
    int speed,
    int is_with_id,
    int *instance_id,
    int *nof_instances)
{
    int i, index, nof_elements = 0;
    const int *instance_id_array = NULL;
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get flexe core ports array from DNX DATA
     */
    for (i = 0; i < dnx_data_nif.flexe.nof_phy_speeds_get(unit); i++)
    {
        if (speed == dnx_data_nif.flexe.phy_info_get(unit, i)->speed)
        {
            instance_id_array = dnx_data_nif.flexe.phy_info_get(unit, i)->instance_id;
            nof_elements = dnx_data_nif.flexe.phy_info_get(unit, i)->nof_instances;
            break;
        }
    }
    if (i == dnx_data_nif.flexe.nof_phy_speeds_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The speed is not supported.\r\n");
    }
    *nof_instances = 0;
    for (i = 0; i < dnx_data_nif.flexe.nof_flexe_instances_get(unit); i++)
    {
        if (is_with_id && (*instance_id != instance_id_array[i]))
        {
            /*
             * If "is_with_id" is set, check if the specified instance_id is legal.
             */
            continue;
        }
        /*
         * Coverity:
         * If the instance_id_array is NULL, the codes should return in
         * the previous line.
         */
         /* coverity[var_deref_op : FALSE]  */
        *instance_id = instance_id_array[i];
        if (*instance_id != -1)
        {
            for (index = 0; index < nof_elements; ++index)
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.
                                instance.is_allocated(unit, *instance_id + index, &is_allocated));
                if (is_allocated)
                {
                    break;
                }
            }
            if (!is_allocated)
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.
                                instance.allocate_several(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, nof_elements, NULL,
                                                          instance_id));
                *nof_instances = nof_elements;
                break;
            }
        }
    }
    if (i == dnx_data_nif.flexe.nof_flexe_instances_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Instance ID allocation failed.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_instance_id_free(
    int unit,
    int instance_id,
    int nof_instance)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.resource_alloc.instance.free_several(unit, nof_instance, instance_id, NULL));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_port_speed_set(
    int unit,
    int flexe_core_port,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify flexe core port */
    if (flexe_core_port < 0 || flexe_core_port >= dnx_data_nif.flexe.nof_flexe_core_ports_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE core port index is out of range.\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.phy_speed.set(unit, flexe_core_port, speed));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_port_speed_get(
    int unit,
    int flexe_core_port,
    int *speed)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify flexe core port */
    if (flexe_core_port < 0 || flexe_core_port >= dnx_data_nif.flexe.nof_flexe_core_ports_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE core port index is out of range.\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.phy_speed.get(unit, flexe_core_port, speed));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_port_instances_get(
    int unit,
    int flexe_core_port,
    uint32 *instances)
{
    int nof_instances, base_instance, i;

    SHR_FUNC_INIT_VARS(unit);

    /** verify flexe core port */
    if (flexe_core_port < 0 || flexe_core_port >= dnx_data_nif.flexe.nof_flexe_core_ports_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE core port index is out of range.\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.nof_instances.get(unit, flexe_core_port, &nof_instances));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &base_instance));

    *instances = 0;
    for (i = 0; i < nof_instances; ++i)
    {
        SHR_BITSET(instances, base_instance + i);
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_sar_cell_mode_get(
    int unit,
    int client_channel,
    int is_rx,
    int *cell_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify flexe core port */
    if (client_channel < 0 || client_channel >= dnx_data_nif.sar_client.nof_clients_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "SAR channel ID is out of range.\n");
    }
    if (is_rx)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.sar_client.rx_cell_mode.get(unit, client_channel, cell_mode));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.sar_client.tx_cell_mode.get(unit, client_channel, cell_mode));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_sar_cell_mode_set(
    int unit,
    int client_channel,
    int is_rx,
    int cell_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify flexe core port */
    if (client_channel < 0 || client_channel >= dnx_data_nif.sar_client.nof_clients_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "SAR channel ID is out of range.\n");
    }
    if (is_rx)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.sar_client.rx_cell_mode.set(unit, client_channel, cell_mode));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.sar_client.tx_cell_mode.set(unit, client_channel, cell_mode));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_sar_cal_granularity_calculate(
    int unit,
    int speed,
    bcm_port_flexe_phy_slot_mode_t * granularity)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Find a proper granularity
     */
    for (*granularity = bcmPortFlexePhySlot5G; *granularity < bcmPortFlexePhySlotModeCount; ++(*granularity))
    {
        if ((speed % dnx_data_nif.flexe.client_speed_granularity_get(unit, *granularity)->val) == 0)
        {
            break;
        }
    }
    if (*granularity == bcmPortFlexePhySlotModeCount)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid SAR speed %d.\n", speed);
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_client_slot_mode_set(
    int unit,
    int client_channel,
    int is_rx,
    bcm_port_flexe_phy_slot_mode_t slot_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify flexe core port */
    if (client_channel < 0 || client_channel >= dnx_data_nif.flexe.nof_clients_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "SAR channel ID is out of range.\n");
    }
    if (is_rx)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.flexe_client.rx_slot_mode.set(unit, client_channel, slot_mode));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.flexe_client.tx_slot_mode.set(unit, client_channel, slot_mode));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_client_slot_mode_get(
    int unit,
    int client_channel,
    int is_rx,
    bcm_port_flexe_phy_slot_mode_t * slot_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify flexe core port */
    if (client_channel < 0 || client_channel >= dnx_data_nif.flexe.nof_clients_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "SAR channel ID is out of range.\n");
    }
    if (is_rx)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.flexe_client.rx_slot_mode.get(unit, client_channel, slot_mode));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.flexe_client.tx_slot_mode.get(unit, client_channel, slot_mode));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

#undef _ERR_MSG_MODULE_NAME
