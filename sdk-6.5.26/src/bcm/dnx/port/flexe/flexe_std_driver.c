/** \file flexe_std_driver.c
 *
 * Functions for FlexE adapter layer
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_FLEXE

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <sal/core/thread.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/error.h>

#include <shared/util.h>
#include <flexe_drv.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_flexe_core_access.h>
#include <bcm_int/dnx/algo/flexe_core/algo_flexe_core.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <bcm_int/dnx/port/flexe/flexe_std.h>

#include <shared/pbmp.h>

#include "flexe_common.h"
/* } */

/*************
 * DEFINES   *
 *************/
/* { */
/* } */

/*************
 *  MACROS   *
 *************/
/* { */
#define FLEXE_CORE_FLAGS_VERIFY(_flags) \
    SHR_MASK_VERIFY(_flags, (BCM_PORT_FLEXE_RX | BCM_PORT_FLEXE_TX), _SHR_E_PARAM, "unexpected flags.\n")

#define FLEXE_CORE_GROUP_VERIFY(_group) \
    SHR_RANGE_VERIFY(_group, 0, dnx_data_nif.flexe.nof_groups_get(unit) - 1, _SHR_E_PARAM, "Group ID is out of range.\n")

#define FLEXE_CORE_PORT_VERIFY(_port) \
    SHR_RANGE_VERIFY(_port, 0, dnx_data_nif.flexe.nof_pcs_get(unit) - 1, _SHR_E_PARAM, "FlexE core port ID is out of range.\n")

#define FLEXE_CORE_NOF_PORTS_VERIFY(_nof_ports) \
    SHR_RANGE_VERIFY(_nof_ports, 1, dnx_data_nif.flexe.nof_pcs_get(unit), _SHR_E_PARAM, "Number of ports is out of range.\n")

#define FLEXE_CORE_FLEXE_CHANNEL_VERIFY(_channel) \
    SHR_RANGE_VERIFY(_channel, 0, dnx_data_nif.flexe.nof_clients_get(unit) - 1, _SHR_E_PARAM, "Client ID is out of range.\n")

#define FLEXE_CORE_NOF_TIMESLOTS_VERIFY(_num_slots) \
    SHR_RANGE_VERIFY(_num_slots, 1, dnx_data_nif.flexe.max_nof_slots_get(unit), _SHR_E_PARAM, "Numbre of timeslots is out of range.\n")

#define FLEXE_CORE_SAR_CHANNEL_VERIFY(_channel) \
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(_channel)

#define FLEXE_CORE_SAR_NOF_TIMESLOTS_VERIFY(_num_slots) \
    SHR_RANGE_VERIFY(_num_slots, 1, dnx_data_nif.sar_client.nof_sar_timeslots_get(unit), _SHR_E_PARAM, "Numbre of timeslots is out of range.\n")

#define FLEXE_CORE_MAC_CHANNEL_VERIFY(_channel) \
    SHR_RANGE_VERIFY(_channel, 0, dnx_data_nif.mac_client.nof_clients_get(unit) - 1, _SHR_E_PARAM, "Client ID is out of range.\n")

#define FLEXE_CORE_MAC_NOF_TIMESLOTS_VERIFY(_nof_slots) \
    SHR_RANGE_VERIFY(_nof_slots, 1, dnx_data_nif.mac_client.nof_mac_timeslots_get(unit), _SHR_E_PARAM, "Numbre of timeslots is out of range.\n")

#define FLEXE_CORE_BOOL_VERIFY(_en) \
    SHR_RANGE_VERIFY(_en, 0, 1, _SHR_E_PARAM, "Value out of range which should be a bool.\n")

#define FLEXE_CORE_CALENDAR_ID_VERIFY(_cal_id) \
    SHR_RANGE_VERIFY(_cal_id, 0, 1, _SHR_E_PARAM, "Calendar ID is out of range.\n")

#define FLEXE_CORE_BITMAP_TO_ARRAY(_bitmap, _max, _array, _pos) \
{ \
    int __ii__; \
    SHR_BIT_ITER(_bitmap, _max, __ii__) \
    { \
        _array[_pos++] = __ii__; \
    } \
}

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/*
 * Global struct to store FlexE core port info
 */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

static shr_error_e flexe_std_oh_client_id_clear(
    int unit,
    int cal_id,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports);

shr_error_e _flexe_std_oh_logical_phy_id_set(
    int unit,
    int flexe_core_port,
    int speed,
    int logical_phy_id);

static shr_error_e flexe_std_oh_phymap_set(
    int unit,
    int flexe_core_port,
    int logical_phy_id,
    int enable);

/*
 * See .h file
 */
shr_error_e
flexe_std_init(
    int unit)
{
#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
    return soc_flexe_std_init(unit);
#else
    /*
     * FlexE registers are not simulated in PLISIM
     */
    return _SHR_E_NONE;
#endif
}

/*
 * See .h file
 * DBG driver uses this function to release dynamic memories.
 * Not necessary for STD driver. Dummy function.
 */
shr_error_e
flexe_std_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for flexe_std_group_phy_remove API.
 *
 */
static shr_error_e
flexe_std_group_phy_remove_verify(
    int unit,
    uint32 flags,
    int group_index,
    soc_pbmp_t flexe_core_port_bmp)
{
    int flexe_core_port = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify flags */
    FLEXE_CORE_FLAGS_VERIFY(flags);

    /** Verify group ID */
    FLEXE_CORE_GROUP_VERIFY(group_index);

    _SHR_PBMP_ITER(flexe_core_port_bmp, flexe_core_port)
    {
        FLEXE_CORE_PORT_VERIFY(flexe_core_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for flexe_std_group_phy_add API.
 *
 */
static shr_error_e
flexe_std_group_phy_add_verify(
    int unit,
    uint32 flags,
    int group_index,
    soc_pbmp_t flexe_core_port_bmp,
    const int *logical_phy_id,
    const flexe_core_port_info_t * port_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flexe_std_group_phy_remove_verify(unit, flags, group_index, flexe_core_port_bmp));

    /** Null checks */
    SHR_NULL_CHECK(logical_phy_id, _SHR_E_PARAM, "logical_phy_id");
    SHR_NULL_CHECK(port_info, _SHR_E_PARAM, "port_info");

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Add PHYs for bypass mode.
 *
 */
static shr_error_e
flexe_std_group_bypass_phy_add(
    int unit,
    uint32 flags,
    int group_index,
    soc_pbmp_t * flexe_core_port_bmp,
    const flexe_core_port_info_t * port_info)
{
    int flexe_core_port;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(*flexe_core_port_bmp, flexe_core_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.is_bypass.set(unit, flexe_core_port, 1));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - sort the flexe core port according to the
 *          logical PHY ID.
 *
 */
static shr_error_e
flexe_std_flexe_demux_pre_and_latter_lphy_get(
    int unit,
    int group_index,
    int logical_phy_id,
    int *pre_lphy,
    int *latter_lphy)
{
    int is_latter_lphy_found = 0, flexe_core_port;
    uint32 logical_phy_id_node;
    sw_state_ll_node_t node;
    SHR_BITDCL lphy_bitmap_tmp[_SHR_BITDCLSIZE(DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_LPHYS)];

    SHR_FUNC_INIT_VARS(unit);

    *pre_lphy = 0;
    *latter_lphy = dnx_data_nif.flexe.nof_flexe_lphys_get(unit) - 1;

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.flexe_ports.get_first(unit, group_index, &node));

    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.
                        flexe_ports.node_key(unit, group_index, node, (uint32 *) &logical_phy_id_node));
        /*
         * From 1 base to 0 base
         */
        logical_phy_id_node = logical_phy_id_node - 1;
        SHR_BITCLR_RANGE(lphy_bitmap_tmp, 0, dnx_data_nif.flexe.nof_flexe_lphys_get(unit));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.
                        flexe_ports.node_value(unit, group_index, node, (uint32 *) &flexe_core_port));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.
                        demux_lphy_bitmap.bit_range_read(unit, flexe_core_port, 0, 0,
                                                         dnx_data_nif.flexe.nof_flexe_lphys_get(unit),
                                                         lphy_bitmap_tmp));
        if (logical_phy_id_node < logical_phy_id)
        {
            *pre_lphy = shr_bitop_last_bit_get(lphy_bitmap_tmp, dnx_data_nif.flexe.nof_flexe_lphys_get(unit));
        }
        else if (!is_latter_lphy_found)
        {
            *latter_lphy = shr_bitop_first_bit_get(lphy_bitmap_tmp, dnx_data_nif.flexe.nof_flexe_lphys_get(unit));
            is_latter_lphy_found = 1;
        }
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.flexe_ports.next_node(unit, group_index, node, &node));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add PHYs for bypass mode.
 *
 */
static shr_error_e
flexe_std_group_flexe_phy_add(
    int unit,
    uint32 flags,
    int group_index,
    soc_pbmp_t * flexe_core_port_bmp,
    const int *logical_phy_id,
    const flexe_core_port_info_t * port_info)
{
    SHR_BITDCL group_instances[_SHR_BITDCLSIZE(DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_INSTANCES)];
    SHR_BITDCL demux_lphy_bitmap[_SHR_BITDCLSIZE(DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_LPHYS)];
    SHR_BITDCL mux_lphy_bitmap[_SHR_BITDCLSIZE(DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_LPHYS)];
    int flexe_core_ports_arr[DNX_DATA_MAX_NIF_FLEXE_NOF_PCS] = { 0 };
    int logical_phy_id_arr[DNX_DATA_MAX_NIF_FLEXE_NOF_PCS] = { 0 };
    uint8 demux_lphys[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_LPHYS];
    uint8 mux_lphys[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_LPHYS];
    sw_state_ll_node_t node;
    uint8 lphy;
    int instance = 0;
    int nof_ports = 0;
    int ii, pos;
    int is_group_exist;
    int pre_lphy;
    int latter_lphy;
    int flexe_core_port;
    int is_alloc_success;
    int existed_group_id;
    uint32 logical_phy_id_plus_1;
    uint32 nof_rx_elements = 0, nof_tx_elements = 0;
    bcm_pbmp_t flexe_core_ports_tmp;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(flexe_std_group_phy_add_verify
                           (unit, flags, group_index, *flexe_core_port_bmp, logical_phy_id, port_info));
    /*
     * Start allocate LPHY for Demux, to be optimized
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.flexe_ports.get_first(unit, group_index, &node));
    is_group_exist = DNX_SW_STATE_LL_IS_NODE_VALID(node) ? 1 : 0;
    /*
     * If the group already exists, need to check if there is enough
     * space for the new PHY.
     */
    if (is_group_exist)
    {
        /*
         * Get the previous and latter demux logical PHY
         */
        _SHR_PBMP_FIRST(*flexe_core_port_bmp, flexe_core_port);
        SHR_IF_ERR_EXIT(flexe_std_flexe_demux_pre_and_latter_lphy_get
                        (unit, group_index, logical_phy_id[flexe_core_port], &pre_lphy, &latter_lphy));
        /*
         * Check if there is enough logical PHY
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_demux_lphy_alloc(unit, DNX_ALGO_FLEXE_CORE_LPHY_ALLOC_CHECK_ONLY,
                                                             port_info->speed, pre_lphy, latter_lphy, NULL,
                                                             &is_alloc_success));
        /*
         * If allocation failed, need to free all the logical PHYs in the group
         * and re-alloc again.
         */
        if (!is_alloc_success)
        {
            /*
             * Check if there is Rx timeslots in Logical phys
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.clients_rx.nof_elements(unit, group_index, &nof_rx_elements));
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.clients_tx.nof_elements(unit, group_index, &nof_tx_elements));
            if (nof_rx_elements || nof_tx_elements)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Please clear all the time slots in the group. \n");
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_core_ports_get(unit, group_index, &flexe_core_ports_tmp));
                _SHR_PBMP_REMOVE(flexe_core_ports_tmp, *flexe_core_port_bmp);
                SHR_IF_ERR_EXIT(flexe_std_group_phy_remove
                                (unit, BCM_PORT_FLEXE_RX | BCM_PORT_FLEXE_TX, group_index, flexe_core_ports_tmp));
            }
        }
    }
    if (!is_group_exist || (is_group_exist && !is_alloc_success))
    {
        /*
         * In this scenario, needs to sort all the FlexE core ports
         * as per logical PHY ID
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_ports_sort_get
                        (unit, group_index, flexe_core_ports_arr, logical_phy_id_arr, &nof_ports));
    }
    else
    {
        /*
         * For other scenario, only add one member to the FlexE group
         */
        flexe_core_ports_arr[0] = flexe_core_port;
        logical_phy_id_arr[0] = logical_phy_id[flexe_core_port];
        nof_ports = 1;
    }
    SHR_BITCLR_RANGE(group_instances, 0, dnx_data_nif.flexe.nof_flexe_instances_get(unit));
    /*
     * Allocate DEMUX logical PHYs and update DB
     */
    for (ii = 0; ii < nof_ports; ++ii)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_phy_add
                        (unit, flexe_core_ports_arr[ii], flexe_core_ports_arr[ii], port_info->speed,
                         (flexe_drv_serdes_rate_mode_e) (port_info->serdes_rate_mode)));
        /*
         * Get the previous and latter demux logical PHY
         */
        SHR_IF_ERR_EXIT(flexe_std_flexe_demux_pre_and_latter_lphy_get
                        (unit, group_index, logical_phy_id_arr[ii], &pre_lphy, &latter_lphy));
        /*
         * Allocate Logical PHY for Demux
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_demux_lphy_alloc
                        (unit, 0, port_info->speed, pre_lphy, latter_lphy, demux_lphy_bitmap, NULL));
        /*
         * Update bitmap
         */
        SHR_BITSET(group_instances, flexe_core_ports_arr[ii]);
        /*
         * Update DB
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.group.set(unit, flexe_core_ports_arr[ii], group_index));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.
                        serdes_rate.set(unit, flexe_core_ports_arr[ii], port_info->serdes_rate_mode));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.
                        instance.set(unit, flexe_core_ports_arr[ii], flexe_core_ports_arr[ii]));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.
                        demux_lphy_bitmap.bit_range_write(unit, flexe_core_ports_arr[ii], 0, 0,
                                                          dnx_data_nif.flexe.nof_flexe_lphys_get(unit),
                                                          demux_lphy_bitmap));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.
                        is_100g.set(unit, flexe_core_ports_arr[ii], (port_info->speed >= FLEXE_PHY_SPEED_100G)));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.is_bypass.set(unit, flexe_core_ports_arr[ii], 0));
    }

    /*
     * Sort lphy according to instance ID
     */
    pos = 0;
    sal_memset(demux_lphys, FLEXE_INVALID_PORT_ID, sizeof(demux_lphys));
    sal_memset(mux_lphys, FLEXE_INVALID_PORT_ID, sizeof(mux_lphys));

    if (flags & BCM_PORT_FLEXE_RX)
    {
        pos = 0;
        SHR_BIT_ITER(group_instances, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.demux_lphy_bitmap.bit_range_read(unit, instance, 0, 0,
                                                                                         dnx_data_nif.
                                                                                         flexe.nof_flexe_lphys_get
                                                                                         (unit), demux_lphy_bitmap));
            SHR_BIT_ITER(demux_lphy_bitmap, dnx_data_nif.flexe.nof_flexe_lphys_get(unit), lphy)
            {
                demux_lphys[pos++] = lphy;
            }
        }
        SHR_IF_ERR_EXIT(soc_flexe_std_group_rx_add(unit, group_index, group_instances, demux_lphys, port_info->speed));
    }

    if (flags & BCM_PORT_FLEXE_TX)
    {
        pos = 0;
        SHR_BIT_ITER(group_instances, dnx_data_nif.flexe.nof_flexe_instances_get(unit), instance)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.mux_lphy_bitmap.bit_range_read(unit, instance, 0, 0,
                                                                                       dnx_data_nif.
                                                                                       flexe.nof_flexe_lphys_get(unit),
                                                                                       mux_lphy_bitmap));
            SHR_BIT_ITER(mux_lphy_bitmap, dnx_data_nif.flexe.nof_flexe_lphys_get(unit), lphy)
            {
                mux_lphys[pos++] = lphy;
            }
        }
        SHR_IF_ERR_EXIT(soc_flexe_std_group_tx_add(unit, group_index, group_instances, mux_lphys, port_info->speed));
    }

    for (ii = 0; ii < nof_ports; ++ii)
    {
        /*
         * Make logical PHY ID to 1 base in DB to avoid SW state issue
         */
        logical_phy_id_plus_1 = logical_phy_id_arr[ii] + 1;
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.flexe_ports.add(unit, group_index, &logical_phy_id_plus_1,
                                                                     (uint32 *) &flexe_core_ports_arr[ii]));

        /*
         * Update overhead
         */
        SHR_IF_ERR_EXIT(_flexe_std_oh_logical_phy_id_set
                        (unit, flexe_core_ports_arr[ii], port_info->speed, logical_phy_id_arr[ii]));
    }
    /*
     * Get existed group ID
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_index_to_id_get(unit, group_index, &existed_group_id));
    for (ii = 0; ii < nof_ports; ++ii)
    {
        /*
         * Update PHYMAP
         */
        SHR_IF_ERR_EXIT(flexe_std_oh_phymap_set(unit, flexe_core_ports_arr[ii], logical_phy_id_arr[ii], 1));
        /*
         * Update group ID.
         * 1. If existed group id is -1, meaning the group id is not configured,
         *    so setting it to "1" - default value.
         * 2. If existed group id is not -1, meaning the group id has been configured,
         *    so setting it to the existed value for all the instances
         */
        if (existed_group_id == -1)
        {
            SHR_IF_ERR_EXIT(flexe_std_oh_group_id_set(unit, flexe_core_ports_arr[ii], 1));
        }
        else
        {
            SHR_IF_ERR_EXIT(flexe_std_oh_group_id_set(unit, flexe_core_ports_arr[ii], existed_group_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add phys to FlexE group
 *
 */
shr_error_e
flexe_std_group_phy_add(
    int unit,
    uint32 flags,
    int group_index,
    soc_pbmp_t flexe_core_port_bmp,
    const int *logical_phy_id,
    const flexe_core_port_info_t * port_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(flexe_std_group_phy_add_verify
                           (unit, flags, group_index, flexe_core_port_bmp, logical_phy_id, port_info));

    if (port_info->is_bypass)
    {
        SHR_IF_ERR_EXIT(flexe_std_group_bypass_phy_add(unit, flags, group_index, &flexe_core_port_bmp, port_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(flexe_std_group_flexe_phy_add
                        (unit, flags, group_index, &flexe_core_port_bmp, logical_phy_id, port_info));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove PHYs for bypass mode.
 *
 */
static shr_error_e
flexe_std_group_bypass_phy_remove(
    int unit,
    uint32 flags,
    int group_index,
    soc_pbmp_t * flexe_core_port_bmp)
{
    int flexe_core_port;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(*flexe_core_port_bmp, flexe_core_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.is_bypass.set(unit, flexe_core_port, 0));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove PHYs for FlexE mode.
 *
 */
static shr_error_e
flexe_std_group_flexe_phy_remove(
    int unit,
    uint32 flags,
    int group_index,
    soc_pbmp_t * flexe_core_port_bmp)
{
    SHR_BITDCL group_phys[_SHR_BITDCLSIZE(DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_INSTANCES)];
    SHR_BITDCL group_demux_lphys[_SHR_BITDCLSIZE(DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_LPHYS)];
    SHR_BITDCL demux_lphy_bitmap[_SHR_BITDCLSIZE(DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_LPHYS)];
    SHR_BITDCL group_mux_lphys[_SHR_BITDCLSIZE(DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_LPHYS)];
    SHR_BITDCL mux_lphy_bitmap[_SHR_BITDCLSIZE(DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_LPHYS)];
    sw_state_ll_node_t node;
    int phy_speed_in_group = 0;
    int flexe_core_port = 0;
    int logical_phy_id = 0;
    int instance = 0;
    uint8 found = 0;
    uint32 logical_phy_id_plus_1;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(flexe_std_group_phy_remove_verify(unit, flags, group_index, *flexe_core_port_bmp));

    SHR_BITCLR_RANGE(group_phys, 0, dnx_data_nif.flexe.nof_flexe_instances_get(unit));
    SHR_BITCLR_RANGE(group_demux_lphys, 0, dnx_data_nif.flexe.nof_flexe_lphys_get(unit));
    SHR_BITCLR_RANGE(group_mux_lphys, 0, dnx_data_nif.flexe.nof_flexe_lphys_get(unit));

    _SHR_PBMP_ITER(*flexe_core_port_bmp, flexe_core_port)
    {
        /*
         * Get phy speed of FlexE ports in group. All ports in a group should have the same speed.
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.phy_speed.get(unit, flexe_core_port, &phy_speed_in_group));
        break;
    }
    _SHR_PBMP_ITER(*flexe_core_port_bmp, flexe_core_port)
    {
        /*
         * Clear FlexE overhead
         */
        SHR_IF_ERR_EXIT(_flexe_std_oh_logical_phy_id_set(unit, flexe_core_port, phy_speed_in_group, 1));
        SHR_IF_ERR_EXIT(flexe_std_oh_group_id_set(unit, flexe_core_port, 1));

        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));
        SHR_BITSET(group_phys, instance);

        SHR_BITCLR_RANGE(demux_lphy_bitmap, 0, dnx_data_nif.flexe.nof_flexe_lphys_get(unit));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.demux_lphy_bitmap.bit_range_read
                        (unit, flexe_core_port, 0, 0, dnx_data_nif.flexe.nof_flexe_lphys_get(unit), demux_lphy_bitmap));

        SHR_BITOR_RANGE(group_demux_lphys, demux_lphy_bitmap, 0, dnx_data_nif.flexe.nof_flexe_lphys_get(unit),
                        group_demux_lphys);

        SHR_BITCLR_RANGE(mux_lphy_bitmap, 0, dnx_data_nif.flexe.nof_flexe_lphys_get(unit));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.mux_lphy_bitmap.bit_range_read
                        (unit, flexe_core_port, 0, 0, dnx_data_nif.flexe.nof_flexe_lphys_get(unit), mux_lphy_bitmap));

        SHR_BITOR_RANGE(group_mux_lphys, mux_lphy_bitmap, 0, dnx_data_nif.flexe.nof_flexe_lphys_get(unit),
                        group_mux_lphys);
    }

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_group_rx_remove
                        (unit, group_index, group_phys, group_demux_lphys, phy_speed_in_group));
    }

    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_group_tx_remove(unit, group_phys, group_mux_lphys, phy_speed_in_group));
    }

    SHR_BITCLR_RANGE(demux_lphy_bitmap, 0, dnx_data_nif.flexe.nof_flexe_lphys_get(unit));
    _SHR_PBMP_ITER(*flexe_core_port_bmp, flexe_core_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.demux_lphy_bitmap.bit_range_read
                        (unit, flexe_core_port, 0, 0, dnx_data_nif.flexe.nof_flexe_lphys_get(unit), demux_lphy_bitmap));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_demux_lphy_free(unit, demux_lphy_bitmap));
        /*
         * Update PHYMAP
         */
        SHR_IF_ERR_EXIT(flexe_std_oh_phymap_set(unit, flexe_core_port, 0, 0));

        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.group.set(unit, flexe_core_port, -1));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.serdes_rate.set(unit, flexe_core_port, 0));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.demux_lphy_bitmap.bit_range_clear(unit, flexe_core_port, 0,
                                                                                      dnx_data_nif.
                                                                                      flexe.nof_flexe_lphys_get(unit)));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.logical_phy_id.get(unit, flexe_core_port, &logical_phy_id));

        /*
         * Remove FlexE port node from group.
         */
        logical_phy_id_plus_1 = logical_phy_id + 1;
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.
                        flexe_ports.find(unit, group_index, &node, &logical_phy_id_plus_1, &found));
        if (found)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.flexe_ports.remove_node(unit, group_index, node));
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Remove PHY from FlexE group
 *
 */
shr_error_e
flexe_std_group_phy_remove(
    int unit,
    uint32 flags,
    int group_index,
    soc_pbmp_t flexe_core_port_bmp)
{
    int is_bypass = 0, flexe_core_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(flexe_std_group_phy_remove_verify(unit, flags, group_index, flexe_core_port_bmp));

    _SHR_PBMP_ITER(flexe_core_port_bmp, flexe_core_port)
    {
        /*
         * Get bypass flag
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.is_bypass.get(unit, flexe_core_port, &is_bypass));
        break;
    }
    if (is_bypass)
    {
        SHR_IF_ERR_EXIT(flexe_std_group_bypass_phy_remove(unit, flags, group_index, &flexe_core_port_bmp));
    }
    else
    {
        SHR_IF_ERR_EXIT(flexe_std_group_flexe_phy_remove(unit, flags, group_index, &flexe_core_port_bmp));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
flexe_std_port_ts_mask_to_global(
    int unit,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports,
    int is_demux,
    SHR_BITDCL * global_ts_bitmap,
    uint32 *global_ts_array)
{
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_LPHYS)];
    int nof_lhpys_per_port;
    int nof_slots_in_lphy;
    int slot_in_lphy;
    int slot_index;
    int phy_speed = 0;
    int lphy;
    int ii;
    int is_100g;
    int global_ts_pos;
    int lphy_index;
    int ts_num;
    int lphy_array[FLEXE_CORE_NOF_LPHYS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.phy_speed.get(unit, flexe_core_port_array[0], &phy_speed));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.is_100g.get(unit, flexe_core_port_array[0], &is_100g));

    /*
     * Get nof lphys per port
     */
    nof_lhpys_per_port = phy_speed / dnx_data_nif.flexe.phy_speed_granularity_get(unit);
    nof_slots_in_lphy =
        dnx_data_nif.flexe.phy_speed_granularity_get(unit) / dnx_data_nif.flexe.client_speed_granularity_get(unit,
                                                                                                             bcmPortFlexePhySlot5G)->val;

    SHR_BITCLR_RANGE(global_ts_bitmap, 0, FLEXE_CORE_NOF_TIMESLOTS);

    ts_num = 0;
    for (ii = 0; ii < nof_flexe_core_ports; ++ii)
    {
        SHR_BITCLR_RANGE(lphy_bitmap, 0, dnx_data_nif.flexe.nof_flexe_lphys_get(unit));

        if (is_demux)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.demux_lphy_bitmap.bit_range_read
                            (unit, flexe_core_port_array[ii], 0, 0, dnx_data_nif.flexe.nof_flexe_lphys_get(unit),
                             lphy_bitmap));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.mux_lphy_bitmap.bit_range_read
                            (unit, flexe_core_port_array[ii], 0, 0, dnx_data_nif.flexe.nof_flexe_lphys_get(unit),
                             lphy_bitmap));
        }
        lphy_index = 0;
        SHR_BIT_ITER(lphy_bitmap, dnx_data_nif.flexe.nof_flexe_lphys_get(unit), lphy)
        {
            lphy_array[lphy_index++] = lphy;
        }
        for (lphy_index = 0; lphy_index < nof_lhpys_per_port; ++lphy_index)
        {
            for (slot_in_lphy = 0; slot_in_lphy < nof_slots_in_lphy; ++slot_in_lphy)
            {
                slot_index = ii * nof_lhpys_per_port * nof_slots_in_lphy;
                slot_index += lphy_index * nof_slots_in_lphy + slot_in_lphy;

                if (SHR_BITGET(ts_mask, slot_index))
                {
                    /*
                     * for 100G phy, the timeslots are arranged as 0~4/10~14/5~9/15~19
                     */
                    if (!is_demux && is_100g && ((slot_index % 20) >= 5) && ((slot_index % 20) < 10))
                    {
                        global_ts_pos = lphy_array[lphy_index + 1] * nof_slots_in_lphy + slot_in_lphy - 5;
                    }
                    else if (!is_demux && is_100g && ((slot_index % 20) >= 10) && ((slot_index % 20) < 15))
                    {
                        global_ts_pos = lphy_array[lphy_index - 1] * nof_slots_in_lphy + slot_in_lphy + 5;
                    }
                    else
                    {
                        global_ts_pos = lphy_array[lphy_index] * nof_slots_in_lphy + slot_in_lphy;
                    }
                    SHR_BITSET(global_ts_bitmap, global_ts_pos);
                    global_ts_array[ts_num++] = global_ts_pos;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify function for flexe_std_client_add/flexe_std_client_delete API.
 *
 */
static shr_error_e
flexe_std_client_add_remove_verify(
    int unit,
    int client_channel,
    uint32 flags,
    int cal_id,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify client ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify flags */
    FLEXE_CORE_FLAGS_VERIFY(flags);

    /** Verify calendar ID */
    FLEXE_CORE_CALENDAR_ID_VERIFY(cal_id);

    /** Null checks */
    SHR_NULL_CHECK(flexe_core_port_array, _SHR_E_PARAM, "flexe_core_port_array");

    /** Verify NOF ports */
    FLEXE_CORE_NOF_PORTS_VERIFY(nof_flexe_core_ports);

    for (ii = 0; ii < nof_flexe_core_ports; ++ii)
    {
        /** Verify FlexE core port ID */
        FLEXE_CORE_PORT_VERIFY(flexe_core_port_array[ii]);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add FlexE BusA client
 *
 */
shr_error_e
flexe_std_client_add(
    int unit,
    int client_channel,
    uint32 flags,
    int cal_id,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports)
{
    SHR_BITDCL demux_global_ts_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)];
    SHR_BITDCL mux_global_ts_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)];
    uint32 global_ts_array[FLEXE_CORE_NOF_TIMESLOTS] = { 0 };
    int is_bypass = 0;
    int phy_speed = 0;
    int group = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(flexe_std_client_add_remove_verify
                           (unit, client_channel, flags, cal_id, flexe_core_port_array, ts_mask, nof_flexe_core_ports));

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(flexe_std_port_ts_mask_to_global
                        (unit, flexe_core_port_array, ts_mask, nof_flexe_core_ports, 1, demux_global_ts_bitmap,
                         global_ts_array));
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(flexe_std_port_ts_mask_to_global
                        (unit, flexe_core_port_array, ts_mask, nof_flexe_core_ports, 0, mux_global_ts_bitmap,
                         global_ts_array));
    }

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.is_bypass.get(unit, flexe_core_port_array[0], &is_bypass));

    if (is_bypass)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.phy_speed.get(unit, flexe_core_port_array[0], &phy_speed));

        if (flags & BCM_PORT_FLEXE_RX)
        {
            SHR_IF_ERR_EXIT(soc_flexe_std_rx_phy_bypass_set(unit, flexe_core_port_array[0], 1, client_channel));
        }

        if (flags & BCM_PORT_FLEXE_TX)
        {
            SHR_IF_ERR_EXIT(soc_flexe_std_tx_phy_bypass_set
                            (unit, flexe_core_port_array[0], phy_speed, 1, client_channel));
        }

        SHR_IF_ERR_EXIT(soc_flexe_std_channel_bypass_set(unit, client_channel, 1));

        SHR_EXIT();
    }

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_channel_rx_add(unit, client_channel, demux_global_ts_bitmap, cal_id));
    }

    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_channel_tx_add
                        (unit, client_channel, mux_global_ts_bitmap, global_ts_array, cal_id));
    }

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.group.get(unit, flexe_core_port_array[0], &group));

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.clients_rx.add_last(unit, group, &client_channel));
    }

    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.clients_tx.add_last(unit, group, &client_channel));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete FlexE busA client
 *
 */
shr_error_e
flexe_std_client_delete(
    int unit,
    int client_channel,
    uint32 flags,
    int cal_id,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports)
{
    SHR_BITDCL demux_global_ts_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)];
    SHR_BITDCL mux_global_ts_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)];
    uint32 global_ts_array[FLEXE_CORE_NOF_TIMESLOTS] = { 0 };
    sw_state_ll_node_t node;
    int is_bypass = 0;
    int phy_speed = 0;
    int node_val = 0;
    int group = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(flexe_std_client_add_remove_verify
                           (unit, client_channel, flags, cal_id, flexe_core_port_array, ts_mask, nof_flexe_core_ports));

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(flexe_std_port_ts_mask_to_global
                        (unit, flexe_core_port_array, ts_mask, nof_flexe_core_ports, 1, demux_global_ts_bitmap,
                         global_ts_array));
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(flexe_std_port_ts_mask_to_global
                        (unit, flexe_core_port_array, ts_mask, nof_flexe_core_ports, 0, mux_global_ts_bitmap,
                         global_ts_array));
    }

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.is_bypass.get(unit, flexe_core_port_array[0], &is_bypass));

    if (is_bypass)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.phy_speed.get(unit, flexe_core_port_array[0], &phy_speed));

        if (flags & BCM_PORT_FLEXE_RX)
        {
            SHR_IF_ERR_EXIT(soc_flexe_std_rx_phy_bypass_set(unit, flexe_core_port_array[0], 0, client_channel));
        }

        if (flags & BCM_PORT_FLEXE_TX)
        {
            SHR_IF_ERR_EXIT(soc_flexe_std_tx_phy_bypass_set
                            (unit, flexe_core_port_array[0], phy_speed, 0, client_channel));
        }

        SHR_IF_ERR_EXIT(soc_flexe_std_channel_bypass_set(unit, client_channel, 0));

        SHR_EXIT();
    }

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_channel_rx_del(unit, client_channel, demux_global_ts_bitmap, cal_id));
    }

    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_channel_tx_del
                        (unit, client_channel, mux_global_ts_bitmap, global_ts_array, cal_id));

        SHR_IF_ERR_EXIT(flexe_std_oh_client_id_clear
                        (unit, cal_id, flexe_core_port_array, ts_mask, nof_flexe_core_ports));
    }

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.group.get(unit, flexe_core_port_array[0], &group));

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.clients_rx.get_first(unit, group, &node));

        while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.clients_rx.node_value(unit, group, node, &node_val));

            if (node_val == client_channel)
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.clients_rx.remove_node(unit, group, node));
                break;
            }

            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.clients_rx.next_node(unit, group, node, &node));
        }
    }

    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.clients_tx.get_first(unit, group, &node));

        while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.clients_tx.node_value(unit, group, node, &node_val));

            if (node_val == client_channel)
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.clients_tx.remove_node(unit, group, node));
                break;
            }

            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.clients_tx.next_node(unit, group, node, &node));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add FlexE BusB client
 *
 */
shr_error_e
flexe_std_mac_client_add(
    int unit,
    int client_channel,
    int speed)
{
    int cur_nof_slots, nof_slots;
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_TINYMAC_NOF_TIMESLOTS)];

    SHR_FUNC_INIT_VARS(unit);

    /** Verify client ID */
    FLEXE_CORE_MAC_CHANNEL_VERIFY(client_channel);

    nof_slots =
        UTILEX_MAX((speed / dnx_data_nif.flexe.client_speed_granularity_get(unit, bcmPortFlexePhySlot5G)->val), 1);
    /** Verify number of slots */
    FLEXE_CORE_MAC_NOF_TIMESLOTS_VERIFY(nof_slots);

    if (client_channel >= dnx_data_nif.mac_client.nof_normal_clients_get(unit))
    {
        /*
         * Special clients configured during init
         */
        SHR_EXIT();
    }

    SHR_BITCLR_RANGE(tsmap, 0, FLEXE_TINYMAC_NOF_TIMESLOTS);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.mac_client.timeslot_bitmap.bit_range_read
                    (unit, client_channel, 0, 0, FLEXE_TINYMAC_NOF_TIMESLOTS, tsmap));

    cur_nof_slots = 0;
    SHR_BITCOUNT_RANGE(tsmap, cur_nof_slots, 0, FLEXE_TINYMAC_NOF_TIMESLOTS);

    if ((cur_nof_slots != 0) && (cur_nof_slots != nof_slots))
    {
        SHR_IF_ERR_EXIT(flexe_std_mac_client_delete(unit, client_channel, cur_nof_slots));
    }

    if (cur_nof_slots != nof_slots)
    {
        SHR_BITCLR_RANGE(tsmap, 0, FLEXE_TINYMAC_NOF_TIMESLOTS);

        /*
         * Allocate RX timeslots
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_mac_ts_alloc(unit, nof_slots, tsmap));

        /*
         * Add channel mapping in TinyMAC (bus b)
         */
        SHR_IF_ERR_EXIT(soc_flexe_std_client_channel_add(unit, client_channel, tsmap));

        /*
         * Save data
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.mac_client.timeslot_bitmap.bit_range_write
                        (unit, client_channel, 0, 0, FLEXE_TINYMAC_NOF_TIMESLOTS, tsmap));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete FlexE BusB client
 *
 */
shr_error_e
flexe_std_mac_client_delete(
    int unit,
    int client_channel,
    int speed)
{
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_TINYMAC_NOF_TIMESLOTS)];

    SHR_FUNC_INIT_VARS(unit);

    /** Verify client ID */
    FLEXE_CORE_MAC_CHANNEL_VERIFY(client_channel);

    if (client_channel >= dnx_data_nif.mac_client.nof_normal_clients_get(unit))
    {
        /*
         * Special clients (80/81) were configured during init
         */
        SHR_EXIT();
    }

    SHR_BITCLR_RANGE(tsmap, 0, FLEXE_TINYMAC_NOF_TIMESLOTS);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.mac_client.timeslot_bitmap.bit_range_read
                    (unit, client_channel, 0, 0, FLEXE_TINYMAC_NOF_TIMESLOTS, tsmap));

    /*
     * Remove channel mapping in TinyMAC (bus b)
     */
    SHR_IF_ERR_EXIT(soc_flexe_std_client_channel_del(unit, client_channel, tsmap));

    /*
     * Free timeslots
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_mac_ts_free(unit, tsmap));

    SHR_BITCLR_RANGE(tsmap, 0, FLEXE_TINYMAC_NOF_TIMESLOTS);

    /** restore data */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.mac_client.timeslot_bitmap.bit_range_write
                    (unit, client_channel, 0, 0, FLEXE_TINYMAC_NOF_TIMESLOTS, tsmap));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add FlexE BusC client
 *
 */
shr_error_e
flexe_std_sar_client_add(
    int unit,
    int client_channel,
    uint32 flags,
    int speed)
{
    int cur_ts_num, ts_num;
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)];

    SHR_FUNC_INIT_VARS(unit);

    /** Verify client ID */
    FLEXE_CORE_SAR_CHANNEL_VERIFY(client_channel);

    /** Verify flags */
    FLEXE_CORE_FLAGS_VERIFY(flags);

    /** Verify number of slots */
    ts_num = speed / dnx_data_nif.flexe.client_speed_granularity_get(unit, bcmPortFlexePhySlot5G)->val;
    FLEXE_CORE_SAR_NOF_TIMESLOTS_VERIFY(ts_num);

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_BITCLR_RANGE(tsmap, 0, FLEXE_SAR_NOF_TIMESLOTS);

        /** get data */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.sar_client.rx_timeslot_bitmap.bit_range_read
                        (unit, client_channel, 0, 0, dnx_data_nif.sar_client.nof_sar_timeslots_get(unit), tsmap));

        cur_ts_num = 0;
        SHR_BITCOUNT_RANGE(tsmap, cur_ts_num, 0, FLEXE_CORE_NOF_TIMESLOTS);

        if ((cur_ts_num != 0) && (cur_ts_num != ts_num))
        {
            SHR_IF_ERR_EXIT(flexe_std_sar_client_delete(unit, client_channel, BCM_PORT_FLEXE_RX, cur_ts_num));
        }

        if (cur_ts_num != ts_num)
        {
            SHR_BITCLR_RANGE(tsmap, 0, FLEXE_SAR_NOF_TIMESLOTS);

            /*
             * Allocate RX timeslots
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_sar_rx_ts_alloc(unit, ts_num, tsmap));

            /*
             * Add SAR channel mapping
             */
            SHR_IF_ERR_EXIT(soc_flexe_std_sar_channel_rx_add(unit, client_channel, tsmap));

            /*
             * Save data
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.sar_client.rx_timeslot_bitmap.bit_range_write
                            (unit, client_channel, 0, 0, dnx_data_nif.sar_client.nof_sar_timeslots_get(unit), tsmap));
        }
    }

    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_BITCLR_RANGE(tsmap, 0, FLEXE_SAR_NOF_TIMESLOTS);

        /** get data */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.sar_client.tx_timeslot_bitmap.bit_range_read
                        (unit, client_channel, 0, 0, dnx_data_nif.sar_client.nof_sar_timeslots_get(unit), tsmap));

        cur_ts_num = 0;
        SHR_BITCOUNT_RANGE(tsmap, cur_ts_num, 0, FLEXE_CORE_NOF_TIMESLOTS);

        if ((cur_ts_num != 0) && (cur_ts_num != ts_num))
        {
            SHR_IF_ERR_EXIT(flexe_std_sar_client_delete(unit, client_channel, BCM_PORT_FLEXE_TX, cur_ts_num));
        }

        if (cur_ts_num != ts_num)
        {
            SHR_BITCLR_RANGE(tsmap, 0, FLEXE_SAR_NOF_TIMESLOTS);

            /*
             * Allocate TX timeslots
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_sar_tx_ts_alloc(unit, ts_num, tsmap));

            /*
             * Add SAR channel mapping
             */
            SHR_IF_ERR_EXIT(soc_flexe_std_sar_channel_tx_add(unit, client_channel, tsmap));

            /*
             * Save data
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.sar_client.tx_timeslot_bitmap.bit_range_write
                            (unit, client_channel, 0, 0, dnx_data_nif.sar_client.nof_sar_timeslots_get(unit), tsmap));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete FlexE BusC client
 *
 */
shr_error_e
flexe_std_sar_client_delete(
    int unit,
    int client_channel,
    uint32 flags,
    int speed)
{
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)];

    SHR_FUNC_INIT_VARS(unit);

    /** Verify client ID */
    FLEXE_CORE_SAR_CHANNEL_VERIFY(client_channel);

    /** Verify flags */
    FLEXE_CORE_FLAGS_VERIFY(flags);

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_BITCLR_RANGE(tsmap, 0, FLEXE_SAR_NOF_TIMESLOTS);

        /** get data */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.sar_client.rx_timeslot_bitmap.bit_range_read
                        (unit, client_channel, 0, 0, dnx_data_nif.sar_client.nof_sar_timeslots_get(unit), tsmap));

        /*
         * Remove SAR channel mapping
         */
        SHR_IF_ERR_EXIT(soc_flexe_std_sar_channel_rx_del(unit, client_channel, tsmap));

        /*
         * Free RX timeslots
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_sar_rx_ts_free(unit, tsmap));

        SHR_BITCLR_RANGE(tsmap, 0, FLEXE_SAR_NOF_TIMESLOTS);

        /** restore data */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.sar_client.rx_timeslot_bitmap.bit_range_write
                        (unit, client_channel, 0, 0, dnx_data_nif.sar_client.nof_sar_timeslots_get(unit), tsmap));
    }

    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_BITCLR_RANGE(tsmap, 0, FLEXE_SAR_NOF_TIMESLOTS);

        /** get data */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.sar_client.tx_timeslot_bitmap.bit_range_read
                        (unit, client_channel, 0, 0, dnx_data_nif.sar_client.nof_sar_timeslots_get(unit), tsmap));

        /*
         * Delete SAR channel mapping
         */
        SHR_IF_ERR_EXIT(soc_flexe_std_sar_channel_tx_del(unit, client_channel, tsmap));

        /*
         * Free TX timeslots
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_sar_tx_ts_free(unit, tsmap));

        SHR_BITCLR_RANGE(tsmap, 0, FLEXE_SAR_NOF_TIMESLOTS);

        /** restore data */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.sar_client.tx_timeslot_bitmap.bit_range_write
                        (unit, client_channel, 0, 0, dnx_data_nif.sar_client.nof_sar_timeslots_get(unit), tsmap));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure FlexE sar channel map
 *
 */
shr_error_e
flexe_std_sar_channel_map_set(
    int unit,
    uint32 flags,
    const flexe_core_sar_channel_map_info_t * map_info,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify flags */
    FLEXE_CORE_FLAGS_VERIFY(flags);

    /** Verify SAR channel ID */
    FLEXE_CORE_SAR_CHANNEL_VERIFY(map_info->sar_channel);

    /** Verify enable */
    FLEXE_CORE_BOOL_VERIFY(enable);

    if (enable != 0)
    {
        /** Verify ILKN channel ID */
        SHR_RANGE_VERIFY(map_info->peer_channel, 0, 511, _SHR_E_PARAM, "ILKN channel ID is out of range.\n");
    }

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_sar_channel_map_rx_set
                        (unit, map_info->sar_channel, map_info->peer_channel, enable));
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_sar_channel_map_tx_set
                        (unit, map_info->sar_channel, map_info->peer_channel, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for flexe_std_66b_switch_set API.
 *
 */
static shr_error_e
flexe_std_66b_switch_set_verify(
    int unit,
    int src_channel,
    int dest_channel,
    int index,
    int enable)
{
    int max_channel_id;

    SHR_FUNC_INIT_VARS(unit);

    max_channel_id = dnx_data_nif.mac_client.channel_base_get(unit) + dnx_data_nif.mac_client.nof_clients_get(unit);

    /** Verify src channel ID */
    SHR_RANGE_VERIFY(src_channel, 0, max_channel_id - 1, _SHR_E_PARAM, "Source channel ID is out of range.\n");

    /** Verify enable */
    FLEXE_CORE_BOOL_VERIFY(enable);

    if (enable != 0)
    {
        /** Verify dest channel ID */
        SHR_RANGE_VERIFY(dest_channel, 0, max_channel_id - 1, _SHR_E_PARAM,
                         "Destination channel ID is out of range.\n");
    }

    /** Verify index */
    SHR_RANGE_VERIFY(index, 0, FLEXE_66BSWITCH_NOF_SUBSYS - 1, _SHR_E_PARAM, "Index is out of range.\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure 66B switch
 *
 */
shr_error_e
flexe_std_66b_switch_set(
    int unit,
    const flexe_core_66bswitch_channel_info_t * src_info,
    const flexe_core_66bswitch_channel_info_t * dest_info,
    int index,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(flexe_std_66b_switch_set_verify(unit, src_info->channel, dest_info->channel, index, enable));

    SHR_IF_ERR_EXIT(soc_flexe_std_66bsw_set(unit, src_info->channel, dest_info->channel, index, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure PHY instance number
 *
 */
shr_error_e
_flexe_std_oh_logical_phy_id_set(
    int unit,
    int flexe_core_port,
    int speed,
    int logical_phy_id)
{
    int ii;
    uint8 instance_id;

    SHR_FUNC_INIT_VARS(unit);

    FLEXE_CORE_PORT_VERIFY(flexe_core_port);

    for (ii = 0; ii < FLEXE_PHY_NOF_INSTANCES(speed); ++ii)
    {
        SHR_IF_ERR_EXIT(flexe_common_logical_phy_id_to_instance_num_get(unit, speed, logical_phy_id, ii, &instance_id));

        SHR_IF_ERR_EXIT(soc_flexe_std_oh_logical_phy_id_set(unit, flexe_core_port + ii, instance_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure PHY instance number
 *
 */
shr_error_e
flexe_std_oh_logical_phy_id_set(
    int unit,
    int flexe_core_port,
    int speed,
    int logical_phy_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(_flexe_std_oh_logical_phy_id_set(unit, flexe_core_port, speed, logical_phy_id));
    /*
     * Update PHYMAP
     */
    SHR_IF_ERR_EXIT(flexe_std_oh_phymap_set(unit, flexe_core_port, logical_phy_id, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for flexe_std_oh_logical_phy_id_get API.
 *
 */
static shr_error_e
flexe_std_oh_logical_phy_id_get_verify(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *logical_phy_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE core port ID */
    FLEXE_CORE_PORT_VERIFY(flexe_core_port);

    /** Verify flags */
    FLEXE_CORE_FLAGS_VERIFY(flags);

    /** Null checks */
    SHR_NULL_CHECK(logical_phy_id, _SHR_E_PARAM, "logical_phy_id");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PHY instance number
 *
 */
shr_error_e
flexe_std_oh_logical_phy_id_get(
    int unit,
    int flexe_core_port,
    int speed,
    uint32 flags,
    int *logical_phy_id)
{
    int instance_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(flexe_std_oh_logical_phy_id_get_verify(unit, flexe_core_port, flags, logical_phy_id));

    SHR_IF_ERR_EXIT(soc_flexe_std_oh_logical_phy_id_get(unit, flexe_core_port, flags, &instance_id));

    SHR_IF_ERR_EXIT(flexe_common_instance_num_to_logical_phy_id_get(unit, speed, instance_id, logical_phy_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure group ID
 *
 */
shr_error_e
flexe_std_oh_group_id_set(
    int unit,
    int flexe_core_port,
    int group_id)
{
    int ii;
    int instance = 0;
    int nof_instances = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE core port ID */
    FLEXE_CORE_PORT_VERIFY(flexe_core_port);

    /** Verify group ID - support 20 bits */
    SHR_MASK_VERIFY(group_id, ((0x1 << 20) - 1), _SHR_E_PARAM, "group_id out of range.\n")
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.nof_instances.get(unit, flexe_core_port, &nof_instances));

    for (ii = 0; ii < nof_instances; ++ii)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_oh_group_id_set(unit, instance + ii, group_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get group ID
 *
 */
shr_error_e
flexe_std_oh_group_id_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *group_id)
{
    int instance = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE core port ID */
    FLEXE_CORE_PORT_VERIFY(flexe_core_port);

    /** Verify flags */
    FLEXE_CORE_FLAGS_VERIFY(flags);

    /** Null checks */
    SHR_NULL_CHECK(group_id, _SHR_E_PARAM, "group_id");

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));

    SHR_IF_ERR_EXIT(soc_flexe_std_oh_group_id_get(unit, instance, flags, group_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for flexe_std_oh_client_id_set/flexe_std_oh_client_id_get API.
 *
 */
static shr_error_e
flexe_std_oh_client_id_set_get_verify(
    int unit,
    int cal_id,
    int nof_ports,
    const int *flexe_core_port_array,
    int nof_slots,
    const int *calendar_slots)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify calendar ID */
    FLEXE_CORE_CALENDAR_ID_VERIFY(cal_id);

    /** Null checks */
    SHR_NULL_CHECK(flexe_core_port_array, _SHR_E_PARAM, "flexe_core_port_array");
    SHR_NULL_CHECK(calendar_slots, _SHR_E_PARAM, "calendar_slots");

    /** Verify number of ports */
    FLEXE_CORE_NOF_PORTS_VERIFY(nof_ports);

    for (ii = 0; ii < nof_ports; ++ii)
    {
        /** Verify FlexE core port ID */
        FLEXE_CORE_PORT_VERIFY(flexe_core_port_array[ii]);
    }

    /** Verify number of slots */
    FLEXE_CORE_NOF_TIMESLOTS_VERIFY(nof_slots);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure CLIENT id
 *
 */
shr_error_e
flexe_std_oh_client_id_set(
    int unit,
    int cal_id,
    int nof_ports,
    const int *flexe_core_port_array,
    int nof_slots,
    const int *calendar_slots)
{
    int ii, jj, index, slot = 0;
    int is_100g;
    int instance = 0;
    int nof_instances = 0;
    int slots_per_instance;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(flexe_std_oh_client_id_set_get_verify
                           (unit, cal_id, nof_ports, flexe_core_port_array, nof_slots, calendar_slots));

    for (ii = 0; ii < nof_slots; ++ii)
    {
        /** Verify client ID,  support 16 bits */
    SHR_MASK_VERIFY(calendar_slots[ii], ((0x1 << 16) - 1), _SHR_E_PARAM, "Client ID out of range.\n")}

    for (index = 0; index < nof_ports; ++index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port_array[index], &instance));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.
                        nof_instances.get(unit, flexe_core_port_array[index], &nof_instances));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.is_100g.get(unit, flexe_core_port_array[index], &is_100g));

        slots_per_instance = is_100g ? 20 : 10;

        for (ii = 0; ii < nof_instances; ++ii)
        {
            for (jj = 0; (jj < slots_per_instance) && (slot < nof_slots); ++jj, ++slot)
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_oh_client_id_set(unit, instance + ii, cal_id, jj, calendar_slots[slot]));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get CLIENT id
 *
 */
shr_error_e
flexe_std_oh_client_id_get(
    int unit,
    int cal_id,
    uint32 flags,
    int nof_ports,
    const int *flexe_core_port_array,
    int nof_slots,
    int *calendar_slots)
{
    int ii, jj, index, slot = 0;
    int is_100g;
    int instance = 0;
    int nof_instances = 0;
    int slots_per_instance;
    uint32 flag_mask = BCM_PORT_FLEXE_TX | BCM_PORT_FLEXE_RX;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(flexe_std_oh_client_id_set_get_verify
                           (unit, cal_id, nof_ports, flexe_core_port_array, nof_slots, calendar_slots));

    /*
     * Mask flags
     */
    flags = flags & flag_mask;

    for (index = 0; index < nof_ports; ++index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port_array[index], &instance));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.
                        nof_instances.get(unit, flexe_core_port_array[index], &nof_instances));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.is_100g.get(unit, flexe_core_port_array[index], &is_100g));

        slots_per_instance = is_100g ? 20 : 10;

        for (ii = 0; ii < nof_instances; ++ii)
        {
            for (jj = 0; (jj < slots_per_instance) && (slot < nof_slots); ++jj, ++slot)
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_oh_client_id_get
                                (unit, flags, instance + ii, cal_id, jj, &calendar_slots[slot]));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for flexe_std_oh_client_id_clear API.
 *
 */
static shr_error_e
flexe_std_oh_client_id_clear_verify(
    int unit,
    int cal_id,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify calendar ID */
    FLEXE_CORE_CALENDAR_ID_VERIFY(cal_id);

    /** Null checks */
    SHR_NULL_CHECK(flexe_core_port_array, _SHR_E_PARAM, "flexe_core_port_array");

    /** Verify number of ports */
    FLEXE_CORE_NOF_PORTS_VERIFY(nof_flexe_core_ports);

    for (ii = 0; ii < nof_flexe_core_ports; ++ii)
    {
        /** Verify FlexE core port ID */
        FLEXE_CORE_PORT_VERIFY(flexe_core_port_array[ii]);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - clear the client id in overhead
 *
 */
static shr_error_e
flexe_std_oh_client_id_clear(
    int unit,
    int cal_id,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports)
{
    int ii, jj, index, slot = 0;
    int is_100g = 10;
    int instance = 0;
    int nof_instances = 0;
    int slots_per_instance;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(flexe_std_oh_client_id_clear_verify
                           (unit, cal_id, flexe_core_port_array, ts_mask, nof_flexe_core_ports));

    for (index = 0; index < nof_flexe_core_ports; ++index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port_array[index], &instance));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.
                        nof_instances.get(unit, flexe_core_port_array[index], &nof_instances));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.is_100g.get(unit, flexe_core_port_array[index], &is_100g));

        slots_per_instance = is_100g ? 20 : 10;

        for (ii = 0; ii < nof_instances; ++ii)
        {
            for (jj = 0; jj < slots_per_instance; ++jj, ++slot)
            {
                if (SHR_BITGET(ts_mask, slot))
                {
                    SHR_IF_ERR_EXIT(soc_flexe_std_oh_client_id_set(unit, instance + ii, cal_id, jj, 0));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure phymap
 *
 * If enable == 1:
 *     Set logical PHY ID for all instances in the group;
 *     Clear old logical PHY ID for all instances in the group;
 * If enable == 0:
 *     Clear old logical PHY ID for all instances in the group;
 *     Clear all logical PHY ID for given port;
 *
 */
static shr_error_e
flexe_std_oh_phymap_set(
    int unit,
    int flexe_core_port,
    int logical_phy_id,
    int enable)
{
    int ii, loop;
    int port;
    int group = 0;
    int phy_id;
    int phy_speed = 0;
    int instance = 0;
    uint8 instance_id = 0;
    int nof_instances = 0;
    sw_state_ll_node_t node;
    int logical_phy_id_2_clr = 0;
    int tmp_logical_phy_id = 0;
    uint32 logical_phy_id_plus_1;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE core port ID */
    FLEXE_CORE_PORT_VERIFY(flexe_core_port);

    /** Verify enable */
    FLEXE_CORE_BOOL_VERIFY(enable);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.group.get(unit, flexe_core_port, &group));
    if (group == -1)
    {
        /*
         * If the FlexE phy doesn't belong to any group,
         * no need to configure phymap
         */
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.nof_instances.get(unit, flexe_core_port, &nof_instances));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.logical_phy_id.get(unit, flexe_core_port, &logical_phy_id_2_clr));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.phy_speed.get(unit, flexe_core_port, &phy_speed));
    /*
     * Loop up to two cycles:
     *     cycle 1: set the new logical_phy_id;
     *     cycle 2: clear the old logical_phy_id (if enable and old logical_phy_id not equal to new logical_phy_id);
     */
    for (loop = 0; loop <= (enable && (logical_phy_id_2_clr != logical_phy_id)); ++loop)
    {
        phy_id = (enable && (loop == 0)) ? logical_phy_id : logical_phy_id_2_clr;

        SHR_IF_ERR_EXIT(flexe_common_logical_phy_id_to_instance_num_get(unit, phy_speed, phy_id, 0, &instance_id));

        if (instance_id != 0)
        {
            /*
             * Write to all instances which belong to this group
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.flexe_ports.get_first(unit, group, &node));

            while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.
                                flexe_ports.node_value(unit, group, node, (uint32 *) &port));
                SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, port, &instance));

                for (ii = 0; ii < nof_instances; ++ii)
                {
                    SHR_IF_ERR_EXIT(soc_flexe_std_oh_phymap_update
                                    (unit, instance + ii, instance_id, nof_instances, !enable || (loop == 1)));
                }

                SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.flexe_ports.next_node(unit, group, node, &node));
            }
        }
        /*
         * Update SW state for the 1st round
         */
        if (loop == 0)
        {
            /*
             * if enable && old logical_phy_id == new logical_phy_id:
             *     duplicated configuration, likely called  from group_phy_add, do not update SW state
             */
            if (enable && (logical_phy_id_2_clr != logical_phy_id))
            {
                uint8 found = 0;

                logical_phy_id_plus_1 = logical_phy_id_2_clr + 1;
                SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.
                                flexe_ports.find(unit, group, &node, &logical_phy_id_plus_1, &found));
                if (found)
                {
                    /*
                     * If found then replace the node with new key.
                     * Should always found as a FlexE phy should always be in a group.
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.
                                    flexe_ports.node_value(unit, group, node, (uint32 *) &port));
                    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.flexe_ports.remove_node(unit, group, node));
                }
                logical_phy_id_plus_1 = logical_phy_id + 1;
                SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.
                                flexe_ports.add(unit, group, &logical_phy_id_plus_1, (uint32 *) &port));

            }
        }
    }

    /*
     * IF enable ==0 then clear all logical PHY IDs for the given port
     */
    if (enable == 0)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));

        for (ii = 0; ii < nof_instances; ++ii)
        {
            SHR_IF_ERR_EXIT(soc_flexe_std_oh_phymap_clear(unit, instance + ii));
        }
    }
    else
    {
        /*
         * Update phymap for flexe_core_port with instance ID from other FlexE ports in the same group
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.flexe_ports.get_first(unit, group, &node));

        while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.
                            flexe_ports.node_key(unit, group, node, (uint32 *) &tmp_logical_phy_id));
            tmp_logical_phy_id = tmp_logical_phy_id - 1;
            SHR_IF_ERR_EXIT(flexe_common_logical_phy_id_to_instance_num_get
                            (unit, phy_speed, tmp_logical_phy_id, 0, &instance_id));
            if (instance_id != 0)
            {
                for (ii = 0; ii < nof_instances; ++ii)
                {
                    SHR_IF_ERR_EXIT(soc_flexe_std_oh_phymap_update(unit, instance + ii, instance_id, nof_instances, 0));
                }
            }
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.group.flexe_ports.next_node(unit, group, node, &node));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for flexe_std_oh_cr/ca/c_bit_get API.
 *
 */
static shr_error_e
flexe_std_oh_cr_ca_c_bit_get_verify(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *cal_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE core port ID */
    FLEXE_CORE_PORT_VERIFY(flexe_core_port);

    /** Verify flags */
    FLEXE_CORE_FLAGS_VERIFY(flags);

    /** Null checks */
    SHR_NULL_CHECK(cal_id, _SHR_E_PARAM, "cal_id");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure CR bit
 *
 */
shr_error_e
flexe_std_oh_cr_bit_set(
    int unit,
    int flexe_core_port,
    int cal_id)
{
    int ii;
    int instance = 0;
    int nof_instances = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE core port ID */
    FLEXE_CORE_PORT_VERIFY(flexe_core_port);

    /** Verify calendar ID */
    FLEXE_CORE_CALENDAR_ID_VERIFY(cal_id);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.nof_instances.get(unit, flexe_core_port, &nof_instances));

    for (ii = 0; ii < nof_instances; ++ii)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_oh_cr_bit_set(unit, instance + ii, cal_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get CR bit
 *
 */
shr_error_e
flexe_std_oh_cr_bit_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *cal_id)
{
    int instance = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(flexe_std_oh_cr_ca_c_bit_get_verify(unit, flexe_core_port, flags, cal_id));

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));

    SHR_IF_ERR_EXIT(soc_flexe_std_oh_cr_bit_get(unit, instance, flags, cal_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure CA bit
 *
 */
shr_error_e
flexe_std_oh_ca_bit_set(
    int unit,
    int flexe_core_port,
    int cal_id)
{
    int ii;
    int instance = 0;
    int nof_instances = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE core port ID */
    FLEXE_CORE_PORT_VERIFY(flexe_core_port);

    /** Verify calendar ID */
    FLEXE_CORE_CALENDAR_ID_VERIFY(cal_id);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.nof_instances.get(unit, flexe_core_port, &nof_instances));

    for (ii = 0; ii < nof_instances; ++ii)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_oh_ca_bit_set(unit, instance + ii, cal_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get CA bit
 *
 */
shr_error_e
flexe_std_oh_ca_bit_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *cal_id)
{
    int instance = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(flexe_std_oh_cr_ca_c_bit_get_verify(unit, flexe_core_port, flags, cal_id));

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));

    SHR_IF_ERR_EXIT(soc_flexe_std_oh_ca_bit_get(unit, instance, flags, cal_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure C bit
 *
 */
shr_error_e
flexe_std_oh_c_bit_set(
    int unit,
    int flexe_core_port,
    int cal_id)
{
    int ii;
    int instance = 0;
    int nof_instances = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE core port ID */
    FLEXE_CORE_PORT_VERIFY(flexe_core_port);

    /** Verify calendar ID */
    FLEXE_CORE_CALENDAR_ID_VERIFY(cal_id);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.nof_instances.get(unit, flexe_core_port, &nof_instances));

    for (ii = 0; ii < nof_instances; ++ii)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_oh_c_bit_set(unit, instance + ii, cal_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get C bit
 *
 */
shr_error_e
flexe_std_oh_c_bit_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *cal_id)
{
    int instance = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(flexe_std_oh_cr_ca_c_bit_get_verify(unit, flexe_core_port, flags, cal_id));

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));

    SHR_IF_ERR_EXIT(soc_flexe_std_oh_c_bit_get(unit, instance, flags, cal_id));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure SC bit
 *
 */
shr_error_e
flexe_std_oh_sc_bit_set(
    int unit,
    int flexe_core_port,
    int sync_config)
{
    int ii;
    int instance = 0;
    int nof_instances = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE core port ID */
    FLEXE_CORE_PORT_VERIFY(flexe_core_port);

    /** Verify calendar ID */
    FLEXE_CORE_BOOL_VERIFY(sync_config);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.nof_instances.get(unit, flexe_core_port, &nof_instances));

    for (ii = 0; ii < nof_instances; ++ii)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_oh_sc_bit_set(unit, instance + ii, sync_config));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get SC bit
 *
 */
shr_error_e
flexe_std_oh_sc_bit_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *sync_config)
{
    int instance = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE core port ID */
    FLEXE_CORE_PORT_VERIFY(flexe_core_port);

    /** Verify flags */
    FLEXE_CORE_FLAGS_VERIFY(flags);

    /** Null checks */
    SHR_NULL_CHECK(sync_config, _SHR_E_PARAM, "sync_config");

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));

    SHR_IF_ERR_EXIT(soc_flexe_std_oh_sc_bit_get(unit, instance, flags, sync_config));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure active cal in MUX
 *
 */
shr_error_e
flexe_std_active_calendar_set(
    int unit,
    int flexe_core_port,
    int cal_id)
{
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_LPHYS)];

    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE core port ID */
    FLEXE_CORE_PORT_VERIFY(flexe_core_port);

    /** Verify calendar ID */
    FLEXE_CORE_CALENDAR_ID_VERIFY(cal_id);

    SHR_BITCLR_RANGE(lphy_bitmap, 0, FLEXE_CORE_NOF_INSTANCES);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.mux_lphy_bitmap.bit_range_read(unit, flexe_core_port, 0, 0,
                                                                               dnx_data_nif.
                                                                               flexe.nof_flexe_lphys_get(unit),
                                                                               lphy_bitmap));

    SHR_IF_ERR_EXIT(soc_flexe_std_active_calendar_set(unit, lphy_bitmap, cal_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get active cal
 *
 */
shr_error_e
flexe_std_active_calendar_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *cal_id)
{
    int instance = 0;
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_LPHYS)];

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(flexe_std_oh_cr_ca_c_bit_get_verify(unit, flexe_core_port, flags, cal_id));

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));

    SHR_BITCLR_RANGE(lphy_bitmap, 0, FLEXE_CORE_NOF_INSTANCES);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.mux_lphy_bitmap.bit_range_read(unit, flexe_core_port, 0, 0,
                                                                               dnx_data_nif.
                                                                               flexe.nof_flexe_lphys_get(unit),
                                                                               lphy_bitmap));

    SHR_IF_ERR_EXIT(soc_flexe_std_active_calendar_get(unit, instance, lphy_bitmap, flags, cal_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Group Alarms
 *    Bit 0:  GIDM_ALM
 *    Bit 1:  phy_num mismatch
 *    Bit 2:  LOF
 *    Bit 3:  LOM
 *    Bit 4:  RPF
 *    Bit 5:  OH1_BLOCK_ALM
 *    Bit 6:  C_BIT_ALM
 *    Bit 7:  PMM
 *    Bit 8:  CCAM
 *    Bit 9:  CCBM
 *    Bit 10: FlexE_LOGA (deskew alram)
 *    Bit 11: CRC
 *    Bit 12: SC mismatch
 *    Bit 13: Unequipped alarm
 *
 */
shr_error_e
flexe_std_oh_alarm_status_get(
    int unit,
    int flexe_core_port,
    uint16 *alarm_status)
{
    int ii;
    int instance = 0;
    int nof_instances = 0;
    int group_index;
    uint8 deskew_alarm = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE core port ID */
    FLEXE_CORE_PORT_VERIFY(flexe_core_port);

    /** Null checks */
    SHR_NULL_CHECK(alarm_status, _SHR_E_PARAM, "alarm_status");

    *alarm_status = 0;

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.nof_instances.get(unit, flexe_core_port, &nof_instances));

    if (nof_instances == 0)
    {
        /** port not added to any group yet */
        SHR_IF_ERR_EXIT(soc_flexe_std_oh_alarm_status_get(unit, flexe_core_port, alarm_status));
    }
    else
    {
        for (ii = 0; ii < nof_instances; ++ii)
        {
            SHR_IF_ERR_EXIT(soc_flexe_std_oh_alarm_status_get(unit, instance + ii, alarm_status));
        }
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.group.get(unit, flexe_core_port, &group_index));
        SHR_IF_ERR_EXIT(soc_flexe_std_oh_deskew_alarm_get(unit, group_index, &deskew_alarm));
        if (deskew_alarm)
        {
            *alarm_status |= FLEXE_CORE_OH_ALARM_GROUP_DESKEW;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the time slots bitmap if there is
 *     mismatch happens.
 *
 */
shr_error_e
flexe_std_oh_cal_mismatch_ts_get(
    int unit,
    int flexe_core_port,
    bcm_port_flexe_time_slot_t * time_slots)
{
    SHR_BITDCL ts_per_instance[_SHR_BITDCLSIZE(FLEXE_NOF_TIMESLOTS_PER_INSTANCE)];
    int nof_slots_per_instance;
    int nof_instances = 0;
    int instance = 0;
    int is_100g = 0;
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE core port ID */
    FLEXE_CORE_PORT_VERIFY(flexe_core_port);

    /** Null checks */
    SHR_NULL_CHECK(time_slots, _SHR_E_PARAM, "time_slots");

    SHR_BITCLR_RANGE(*time_slots, 0, BCM_PORT_FLEXE_MAX_NOF_SLOTS);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.nof_instances.get(unit, flexe_core_port, &nof_instances));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.is_100g.get(unit, flexe_core_port, &is_100g));

    nof_slots_per_instance = (is_100g) ? FLEXE_NOF_TIMESLOTS_PER_INSTANCE : FLEXE_NOF_TIMESLOTS_PER_INSTANCE / 2;

    for (ii = 0; ii < nof_instances; ++ii)
    {
        SHR_BITCLR_RANGE(ts_per_instance, 0, FLEXE_NOF_TIMESLOTS_PER_INSTANCE);
        SHR_IF_ERR_EXIT(soc_flexe_std_oh_cal_mismatch_ts_get(unit, instance + ii, ts_per_instance));
        SHR_BITCOPY_RANGE((SHR_BITDCL *) time_slots, ii * nof_slots_per_instance, ts_per_instance, 0,
                          nof_slots_per_instance);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get OAM Alarms
 *    Bit 0: RX_LPI
 *    Bit 1: RX_CS_LF
 *    Bit 2: RX_CS_RF
 *    Bit 3: BASE_OAM_LOS
 *    Bit 4: RX_SDBIP
 *    Bit 5: RX_CRC
 *    Bit 6: RX_RDI
 *    Bit 7: RX_PERIOD_MISMATCH
 *    Bit 8: SDREI
 *    Bit 9: SFBIP
 *    Bit 10: SFREI
 *    Bit 11: LOCAL FAULT
 *    Bit 12: REMOTE FAULT
 *
 */
shr_error_e
flexe_std_oam_alarm_status_get(
    int unit,
    int client_channel,
    uint16 *alarms_status)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(alarms_status, _SHR_E_PARAM, "alarms_status");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_alarm_status_get(unit, client_channel, alarms_status));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable FlexE OAM alarms
 *
 */
shr_error_e
flexe_std_oam_alarm_enable_set(
    int unit,
    int client_channel,
    int alarm_type,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify enable */
    FLEXE_CORE_BOOL_VERIFY(enable);

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_alarm_enable_set(unit, client_channel, alarm_type, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure OAM base period for Rx and Tx
 *
 */
shr_error_e
flexe_std_oam_base_period_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 base_period)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify flags */
    FLEXE_CORE_FLAGS_VERIFY(flags);

    if (base_period > 3)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "base_period not supported: %d.\r\n", base_period);
    }

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_base_period_set(unit, client_channel, flags, base_period));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_oam_base_period_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *base_period)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify flags */
    FLEXE_CORE_FLAGS_VERIFY(flags);

    /** Null checks */
    SHR_NULL_CHECK(base_period, _SHR_E_PARAM, "base_period");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_base_period_get(unit, client_channel, flags, base_period));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable/Disable Base OAM insert
 */
shr_error_e
flexe_std_oam_base_insert_enable_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_base_insert_enable_set(unit, client_channel, enable));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_oam_base_insert_enable_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_base_insert_enable_get(unit, client_channel, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable/Disable OAM bypass functionality
 */
shr_error_e
flexe_std_oam_bypass_enable_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify flags */
    FLEXE_CORE_FLAGS_VERIFY(flags);

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_bypass_enable_set(unit, client_channel, flags, enable));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_oam_bypass_enable_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify flags */
    FLEXE_CORE_FLAGS_VERIFY(flags);

    /** Null checks */
    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_bypass_enable_get(unit, client_channel, flags, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable/Disable OAM bypass functionality for SAR client
 */
shr_error_e
flexe_std_sar_oam_bypass_enable_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify flags */
    FLEXE_CORE_FLAGS_VERIFY(flags);

    SHR_IF_ERR_EXIT(soc_flexe_std_sar_oam_bypass_enable_set(unit, client_channel, flags, enable));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_sar_oam_bypass_enable_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify flags */
    FLEXE_CORE_FLAGS_VERIFY(flags);

    /** Null checks */
    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");

    SHR_IF_ERR_EXIT(soc_flexe_std_sar_oam_bypass_enable_get(unit, client_channel, flags, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable/Disable OAM local fault insert
 */
shr_error_e
flexe_std_oam_local_fault_insert_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_local_fault_insert_set(unit, client_channel, enable));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_oam_local_fault_insert_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_local_fault_insert_get(unit, client_channel, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable/Disable OAM local fault insert
 */
shr_error_e
flexe_std_oam_remote_fault_insert_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_remote_fault_insert_set(unit, client_channel, enable));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_oam_remote_fault_insert_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_remote_fault_insert_get(unit, client_channel, enable));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Enable/Disable OAM rdi insert
 */
shr_error_e
flexe_std_oam_rdi_insert_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_rdi_insert_set(unit, client_channel, enable));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get OAM rdi insert enable status
 */
shr_error_e
flexe_std_oam_rdi_insert_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_rdi_insert_get(unit, client_channel, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SD BIP8 alarm block number
 */
shr_error_e
flexe_std_oam_sd_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify block num, 27 bits [59:33] */
    SHR_MASK_VERIFY(block_num, ((0x1 << 27) - 1), _SHR_E_PARAM, "block_num too big.\n");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sd_alm_block_num_set(unit, client_channel, block_num));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_oam_sd_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(block_num, _SHR_E_PARAM, "block_num");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sd_alm_block_num_get(unit, client_channel, block_num));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SD BIP8 error set threshold
 */
shr_error_e
flexe_std_oam_sd_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify threshold, 17 bits [32:16] */
    SHR_MASK_VERIFY(threshold, ((0x1 << 17) - 1), _SHR_E_PARAM, "threshold too big.\n");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sd_alm_trigger_thr_set(unit, client_channel, threshold));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_oam_sd_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sd_alm_trigger_thr_get(unit, client_channel, threshold));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SD BIP8 error clear threshold
 */
shr_error_e
flexe_std_oam_sd_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify threshold, 16 bits [15:0] */
    SHR_MASK_VERIFY(threshold, ((0x1 << 16) - 1), _SHR_E_PARAM, "threshold too big.\n");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sd_alm_clear_thr_set(unit, client_channel, threshold));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_oam_sd_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sd_alm_clear_thr_get(unit, client_channel, threshold));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SD BEI alarm block number
 */
shr_error_e
flexe_std_oam_sd_bei_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify block num, 27 bits [59:33] */
    SHR_MASK_VERIFY(block_num, ((0x1 << 27) - 1), _SHR_E_PARAM, "block_num too big.\n");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sd_bei_alm_block_num_set(unit, client_channel, block_num));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_oam_sd_bei_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(block_num, _SHR_E_PARAM, "block_num");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sd_bei_alm_block_num_get(unit, client_channel, block_num));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SD BEI error set threshold
 */
shr_error_e
flexe_std_oam_sd_bei_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify threshold, 17 bits [32:16] */
    SHR_MASK_VERIFY(threshold, ((0x1 << 17) - 1), _SHR_E_PARAM, "threshold too big.\n");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sd_bei_alm_trigger_thr_set(unit, client_channel, threshold));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_oam_sd_bei_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sd_bei_alm_trigger_thr_get(unit, client_channel, threshold));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SD BEI error clear threshold
 */
shr_error_e
flexe_std_oam_sd_bei_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify threshold, 16 bits [15:0] */
    SHR_MASK_VERIFY(threshold, ((0x1 << 16) - 1), _SHR_E_PARAM, "threshold too big.\n");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sd_bei_alm_clear_thr_set(unit, client_channel, threshold));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_oam_sd_bei_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sd_bei_alm_clear_thr_get(unit, client_channel, threshold));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SF BIP8 alarm block number
 */
shr_error_e
flexe_std_oam_sf_bip8_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify block num, 27 bits [59:33] */
    SHR_MASK_VERIFY(block_num, ((0x1 << 27) - 1), _SHR_E_PARAM, "block_num too big.\n");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sf_bip8_alm_block_num_set(unit, client_channel, block_num));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_oam_sf_bip8_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(block_num, _SHR_E_PARAM, "block_num");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sf_bip8_alm_block_num_get(unit, client_channel, block_num));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SF BIP8 error set threshold
 */
shr_error_e
flexe_std_oam_sf_bip8_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify threshold, 17 bits [32:16] */
    SHR_MASK_VERIFY(threshold, ((0x1 << 17) - 1), _SHR_E_PARAM, "threshold too big.\n");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sf_bip8_alm_trigger_thr_set(unit, client_channel, threshold));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_oam_sf_bip8_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sf_bip8_alm_trigger_thr_get(unit, client_channel, threshold));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SF BIP8 error clear threshold
 */
shr_error_e
flexe_std_oam_sf_bip8_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify threshold, 16 bits [15:0] */
    SHR_MASK_VERIFY(threshold, ((0x1 << 16) - 1), _SHR_E_PARAM, "threshold too big.\n");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sf_bip8_alm_clear_thr_set(unit, client_channel, threshold));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_oam_sf_bip8_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sf_bip8_alm_clear_thr_get(unit, client_channel, threshold));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SF BEI alarm block number
 */
shr_error_e
flexe_std_oam_sf_bei_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify block num, 27 bits [59:33] */
    SHR_MASK_VERIFY(block_num, ((0x1 << 27) - 1), _SHR_E_PARAM, "block_num too big.\n");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sf_bei_alm_block_num_set(unit, client_channel, block_num));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_oam_sf_bei_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(block_num, _SHR_E_PARAM, "block_num");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sf_bei_alm_block_num_get(unit, client_channel, block_num));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SF BEI error set threshold
 */
shr_error_e
flexe_std_oam_sf_bei_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify threshold, 17 bits [32:16] */
    SHR_MASK_VERIFY(threshold, ((0x1 << 17) - 1), _SHR_E_PARAM, "threshold too big.\n");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sf_bei_alm_trigger_thr_set(unit, client_channel, threshold));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_oam_sf_bei_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sf_bei_alm_trigger_thr_get(unit, client_channel, threshold));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SF BEI error clear threshold
 */
shr_error_e
flexe_std_oam_sf_bei_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Verify threshold, 16 bits [15:0] */
    SHR_MASK_VERIFY(threshold, ((0x1 << 16) - 1), _SHR_E_PARAM, "threshold too big.\n");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sf_bei_alm_clear_thr_set(unit, client_channel, threshold));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_std_oam_sf_bei_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_sf_bei_alm_clear_thr_get(unit, client_channel, threshold));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get OAM BIP8 counter
 */
shr_error_e
flexe_std_oam_bip8_counter_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_bip8_counter_get(unit, client_channel, val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get OAM BEI counter
 */
shr_error_e
flexe_std_oam_bei_counter_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_bei_counter_get(unit, client_channel, val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get OAM packet
 */
shr_error_e
flexe_std_oam_pkt_count_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_pkt_count_get(unit, client_channel, val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get OAM base packet
 */
shr_error_e
flexe_std_oam_base_pkt_count_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE channel ID */
    FLEXE_CORE_FLEXE_CHANNEL_VERIFY(client_channel);

    /** Null checks */
    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_base_pkt_count_get(unit, client_channel, val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure default FlexE phy settings
 */
shr_error_e
flexe_std_phy_default_config_set(
    int unit,
    int flexe_core_port,
    int local_port,
    int speed,
    int enable)
{
    int index, lphy, nof_lphys;
    uint8 lphys[FLEXE_CORE_NOF_LPHYS];
    SHR_BITDCL mux_lphy_bitmap[_SHR_BITDCLSIZE(DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_LPHYS)];
    flexe_drv_port_speed_mode_e rate;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.mux_lphy_bitmap.bit_range_read(unit, flexe_core_port, 0, 0,
                                                                               dnx_data_nif.
                                                                               flexe.nof_flexe_lphys_get(unit),
                                                                               mux_lphy_bitmap));
    if (enable)
    {
        shr_bitop_range_count(mux_lphy_bitmap, 0, dnx_data_nif.flexe.nof_flexe_lphys_get(unit), &nof_lphys);
        /*
         * Allocate the MUX Logical phys when there is speed change
         * Otherwise, there might be re-allocation
         */
        if (nof_lphys * dnx_data_nif.flexe.phy_speed_granularity_get(unit) != speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_mux_lphy_alloc(unit, speed, mux_lphy_bitmap));
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.mux_lphy_bitmap.bit_range_write
                            (unit, flexe_core_port, 0, 0, dnx_data_nif.flexe.nof_flexe_lphys_get(unit),
                             mux_lphy_bitmap));
            SHR_IF_ERR_EXIT(soc_flexe_std_phy_add
                            (unit, flexe_core_port, flexe_core_port, speed, FLEXE_DRV_SERDES_RATE_53G));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_phy_del(unit, flexe_core_port, speed));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_mux_lphy_free(unit, mux_lphy_bitmap));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.mux_lphy_bitmap.bit_range_clear(unit, flexe_core_port, 0,
                                                                                    dnx_data_nif.
                                                                                    flexe.nof_flexe_lphys_get(unit)));
    }
    /*
     * Configure FlexE MUX logical PHYs
     */
    index = 0;
    sal_memset(lphys, FLEXE_INVALID_PORT_ID, FLEXE_CORE_NOF_LPHYS);
    SHR_BIT_ITER(mux_lphy_bitmap, dnx_data_nif.flexe.nof_flexe_lphys_get(unit), lphy)
    {
        lphys[index++] = lphy;
    }
    SHR_IF_ERR_EXIT(soc_flexe_std_mux_lphy_config_set(unit, flexe_core_port, lphys, speed, enable));
    /*
     * Configure FlexE ENV
     */
    rate = FLEXE_PORT_SPEED_TO_RATE(speed);
    SHR_IF_ERR_EXIT(soc_flexe_std_env_set(unit, flexe_core_port, rate, 0, enable));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Allocated time slots in rate adapter
 */
shr_error_e
flexe_std_rateadpt_add(
    int unit,
    int client_channel,
    const flexe_core_rate_adpt_info_t * rate_adpt_info)
{
    int cur_ts_num = 0, ts_num;

    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)];

    SHR_FUNC_INIT_VARS(unit);

    SHR_BITCLR_RANGE(tsmap, 0, FLEXE_CORE_NOF_TIMESLOTS);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.flexe_client.timeslot_bitmap.bit_range_read
                    (unit, client_channel, 0, 0, dnx_data_nif.flexe.max_nof_slots_get(unit), tsmap));

    SHR_BITCOUNT_RANGE(tsmap, cur_ts_num, 0, FLEXE_CORE_NOF_TIMESLOTS);

    ts_num = rate_adpt_info->speed / dnx_data_nif.flexe.client_speed_granularity_get(unit, bcmPortFlexePhySlot5G)->val;
    if (cur_ts_num != 0)
    {
        if (cur_ts_num == ts_num)
        {
            /**
            * Done
            */
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(flexe_std_rateadpt_delete(unit, client_channel, NULL));
            SHR_BITCLR_RANGE(tsmap, 0, FLEXE_CORE_NOF_TIMESLOTS);
        }
    }

    /*
     * Allocate timeslots in rate adapter
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_rateadpt_ts_alloc(unit, ts_num, tsmap));

    /*
     * Add channel mapping to rate adapter
     */
    SHR_IF_ERR_EXIT(soc_flexe_std_mux_rateadp_channel_add(unit, client_channel, tsmap));

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.flexe_client.timeslot_bitmap.bit_range_write
                    (unit, client_channel, 0, 0, dnx_data_nif.flexe.max_nof_slots_get(unit), tsmap));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Delete time slots in rate adapter
 */
shr_error_e
flexe_std_rateadpt_delete(
    int unit,
    int client_channel,
    const flexe_core_rate_adpt_info_t * rate_adpt_info)
{
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)];

    SHR_FUNC_INIT_VARS(unit);

    SHR_BITCLR_RANGE(tsmap, 0, FLEXE_CORE_NOF_TIMESLOTS);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.flexe_client.timeslot_bitmap.bit_range_read
                    (unit, client_channel, 0, 0, dnx_data_nif.flexe.max_nof_slots_get(unit), tsmap));

    /*
     * Delete channel mapping in rate adapter
     */
    SHR_IF_ERR_EXIT(soc_flexe_std_mux_rateadp_channel_del(unit, client_channel, tsmap));

    /*
     * Free timeslots in rate adapter
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_rateadpt_ts_free(unit, tsmap));

    SHR_BITCLR_RANGE(tsmap, 0, FLEXE_CORE_NOF_TIMESLOTS);

    /** restore data */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.flexe_client.timeslot_bitmap.bit_range_write
                    (unit, client_channel, 0, 0, dnx_data_nif.flexe.max_nof_slots_get(unit), tsmap));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure the OAM alarm collection time step
 */
shr_error_e
flexe_std_oam_alarm_collection_timer_step_set(
    int unit,
    int step)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify step, 6 bits [13:8] */
    SHR_MASK_VERIFY(step, ((0x1 << 6) - 1), _SHR_E_PARAM, "step too big.\n");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_alarm_collection_timer_step_set(unit, step));
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Get the OAM alarm collection time step
 */
shr_error_e
flexe_std_oam_alarm_collection_timer_step_get(
    int unit,
    int *step)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Null checks */
    SHR_NULL_CHECK(step, _SHR_E_PARAM, "step");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_alarm_collection_timer_step_get(unit, step));
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure number of steps for oam alarm collection period
 */
shr_error_e
flexe_std_oam_alarm_collection_step_count_set(
    int unit,
    int step_count)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify step_count, 4 bits [3:0] */
    SHR_MASK_VERIFY(step_count, ((0x1 << 4) - 1), _SHR_E_PARAM, "step_count too big.\n");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_alarm_collection_step_count_set(unit, step_count));
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Get number of steps for oam alarm collection period
 */
shr_error_e
flexe_std_oam_alarm_collection_step_count_get(
    int unit,
    int *step_count)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Null checks */
    SHR_NULL_CHECK(step_count, _SHR_E_PARAM, "step_count");

    SHR_IF_ERR_EXIT(soc_flexe_std_oam_alarm_collection_step_count_get(unit, step_count));
exit:
    SHR_FUNC_EXIT;

}
/*
 * \brief - Configure SAR cell mode in FlexE core
 */
shr_error_e
flexe_std_sar_cell_mode_set(
    int unit,
    int cell_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_sar_cell_mode_set(unit, cell_mode));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get SAR cell mode in FlexE core
 */
shr_error_e
flexe_std_sar_cell_mode_get(
    int unit,
    int *cell_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_sar_cell_mode_get(unit, cell_mode));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Set channel map in FlexE core
 */
shr_error_e
flexe_std_busa_channel_map_set(
    int unit,
    uint32 flags,
    const flexe_core_busa_channel_map_info_t * channel_map_info,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_tx_channel_map_table_set
                        (unit, channel_map_info->flexe_core_port, enable ? channel_map_info->local_port : 0xF));
    }
    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_rx_channel_map_table_set
                        (unit, channel_map_info->flexe_core_port, enable ? channel_map_info->local_port : 0xF));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - The local fault status for FlexE client
 */
shr_error_e
flexe_std_client_local_fault_get(
    int unit,
    int client_channel,
    int *local_fault_status)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_client_local_fault_get(unit, client_channel, local_fault_status));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - The Remote fault status for FlexE client
 */
shr_error_e
flexe_std_client_remote_fault_get(
    int unit,
    int client_channel,
    int *remote_fault_status)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_client_remote_fault_get(unit, client_channel, remote_fault_status));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - The local fault status for FlexE SAR client
 */
shr_error_e
flexe_std_sar_client_local_fault_get(
    int unit,
    int client_channel,
    int *local_fault_status)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_sar_client_local_fault_get(unit, client_channel, local_fault_status));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - The Remote fault status for FlexE SAR client
 */
shr_error_e
flexe_std_sar_client_remote_fault_get(
    int unit,
    int client_channel,
    int *remote_fault_status)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_sar_client_remote_fault_get(unit, client_channel, remote_fault_status));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Set FlexE 1588 packet header
 */
shr_error_e
flexe_std_1588_pkt_header_set(
    int unit,
    uint32 flags,
    uint32 val)
{
    uint64 orig_mac = COMPILER_64_INIT(0, 0), new_mac;
    uint32 reg_val;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Config MAC
     */
    if (flags &
        (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_DEST_MAC_LO | FLEXE_CORE_1588_SRC_MAC_HI |
         FLEXE_CORE_1588_SRC_MAC_LO | FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
    {
        /*
         * Get original MAC
         */
        if (flags & FLEXE_CORE_1588_RX)
        {
            if (flags & (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_DEST_MAC_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_rx_dest_mac_get(unit, &orig_mac));
            }
            else if (flags & (FLEXE_CORE_1588_SRC_MAC_HI | FLEXE_CORE_1588_SRC_MAC_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_rx_src_mac_get(unit, &orig_mac));
            }
            else if (flags & (FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_over_eth_rx_uni_dest_mac_get(unit, &orig_mac));
            }
        }
        else if (flags & FLEXE_CORE_1588_TX)
        {
            if (flags & (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_DEST_MAC_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_tx_dest_mac_get(unit, &orig_mac));
            }
            else if (flags & (FLEXE_CORE_1588_SRC_MAC_HI | FLEXE_CORE_1588_SRC_MAC_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_tx_src_mac_get(unit, &orig_mac));
            }
            else if (flags & (FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_over_eth_tx_uni_dest_mac_get(unit, &orig_mac));
            }
        }
        COMPILER_64_SET(new_mac, 0, 0xFFFFFF);

        if (flags & (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_SRC_MAC_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI))
        {
            COMPILER_64_AND(orig_mac, new_mac);
            COMPILER_64_SET(new_mac, 0, val);
            COMPILER_64_SHL(new_mac, 24);
        }
        else if (flags &
                 (FLEXE_CORE_1588_DEST_MAC_LO | FLEXE_CORE_1588_SRC_MAC_LO | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
        {
            COMPILER_64_SHL(new_mac, 24);
            COMPILER_64_AND(orig_mac, new_mac);
            COMPILER_64_SET(new_mac, 0, val);
        }

        COMPILER_64_OR(new_mac, orig_mac);
        /*
         * Configure new MAC
         */
        if (flags & FLEXE_CORE_1588_RX)
        {
            if (flags & (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_DEST_MAC_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_rx_dest_mac_set(unit, new_mac));
            }
            else if (flags & (FLEXE_CORE_1588_SRC_MAC_HI | FLEXE_CORE_1588_SRC_MAC_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_rx_src_mac_set(unit, new_mac));
            }
            else if (flags & (FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_over_eth_rx_uni_dest_mac_set(unit, new_mac));
            }
        }
        else if (flags & FLEXE_CORE_1588_TX)
        {
            if (flags & (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_DEST_MAC_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_tx_dest_mac_set(unit, new_mac));
            }
            else if (flags & (FLEXE_CORE_1588_SRC_MAC_HI | FLEXE_CORE_1588_SRC_MAC_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_tx_src_mac_set(unit, new_mac));
            }
            else if (flags & (FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_over_eth_tx_uni_dest_mac_set(unit, new_mac));
            }
        }
    }
    else
    {
        /*
         * IP configuration
         */
        if (flags & FLEXE_CORE_1588_RX)
        {
            SHR_IF_ERR_EXIT(soc_flexe_std_1588_over_ipv4_rx_uni_dip_get(unit, &reg_val));
        }
        else
        {
            SHR_IF_ERR_EXIT(soc_flexe_std_1588_over_ipv4_tx_uni_dip_get(unit, &reg_val));
        }
        if (flags & FLEXE_CORE_1588_OVER_IPV4_UNI_DIP_HI)
        {
            reg_val &= 0xffff;
            reg_val |= ((val & 0xffff) << 16);
        }
        else
        {
            reg_val &= 0xffff0000;
            reg_val |= (val & 0xffff);
        }
        if (flags & FLEXE_CORE_1588_RX)
        {
            SHR_IF_ERR_EXIT(soc_flexe_std_1588_over_ipv4_rx_uni_dip_set(unit, reg_val));
        }
        else
        {
            SHR_IF_ERR_EXIT(soc_flexe_std_1588_over_ipv4_tx_uni_dip_set(unit, reg_val));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get FlexE 1588 packet header
 */
shr_error_e
flexe_std_1588_pkt_header_get(
    int unit,
    uint32 flags,
    uint32 *val)
{
    uint64 orig_mac = COMPILER_64_INIT(0, 0), mac_mask;
    uint32 reg_val;

    SHR_FUNC_INIT_VARS(unit);

    if (flags &
        (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_DEST_MAC_LO | FLEXE_CORE_1588_SRC_MAC_HI |
         FLEXE_CORE_1588_SRC_MAC_LO | FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
    {
        /*
         * Get original MAC
         */
        if (flags & FLEXE_CORE_1588_RX)
        {
            if (flags & (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_DEST_MAC_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_rx_dest_mac_get(unit, &orig_mac));
            }
            else if (flags & (FLEXE_CORE_1588_SRC_MAC_HI | FLEXE_CORE_1588_SRC_MAC_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_rx_src_mac_get(unit, &orig_mac));
            }
            else if (flags & (FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_over_eth_rx_uni_dest_mac_get(unit, &orig_mac));
            }
        }
        else if (flags & FLEXE_CORE_1588_TX)
        {
            if (flags & (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_DEST_MAC_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_tx_dest_mac_get(unit, &orig_mac));
            }
            else if (flags & (FLEXE_CORE_1588_SRC_MAC_HI | FLEXE_CORE_1588_SRC_MAC_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_tx_src_mac_get(unit, &orig_mac));
            }
            else if (flags & (FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO))
            {
                SHR_IF_ERR_EXIT(soc_flexe_std_1588_over_eth_tx_uni_dest_mac_get(unit, &orig_mac));
            }
        }
        COMPILER_64_SET(mac_mask, 0, 0xFFFFFF);
        if (flags & (FLEXE_CORE_1588_DEST_MAC_HI | FLEXE_CORE_1588_SRC_MAC_HI | FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI))
        {
            COMPILER_64_SHR(orig_mac, 24);
        }
        COMPILER_64_AND(orig_mac, mac_mask);
        *val = COMPILER_64_LO(orig_mac);
    }
    else
    {
        /*
         * Get unicast IP
         */
        if (flags & FLEXE_CORE_1588_RX)
        {
            SHR_IF_ERR_EXIT(soc_flexe_std_1588_over_ipv4_rx_uni_dip_get(unit, &reg_val));
        }
        else
        {
            SHR_IF_ERR_EXIT(soc_flexe_std_1588_over_ipv4_tx_uni_dip_get(unit, &reg_val));
        }
        if (flags & FLEXE_CORE_1588_OVER_IPV4_UNI_DIP_HI)
        {
            *val = ((reg_val >> 16) & 0xffff);
        }
        else
        {
            *val = (reg_val & 0xffff);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Set FlexE oh/oam packet MAC
 */
shr_error_e
flexe_std_oh_oam_pkt_mac_set(
    int unit,
    uint32 flags,
    uint32 val)
{
    uint64 orig_mac = COMPILER_64_INIT(0, 0), new_mac;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get original MAC
     */
    if (flags & (FLEXE_CORE_OH_OAM_DEST_MAC_HI | FLEXE_CORE_OH_OAM_DEST_MAC_LO))
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_oh_oam_dest_mac_get(unit, &orig_mac));
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_oh_oam_src_mac_get(unit, &orig_mac));
    }
    COMPILER_64_SET(new_mac, 0, 0xFFFFFF);

    if (flags & (FLEXE_CORE_OH_OAM_DEST_MAC_HI | FLEXE_CORE_OH_OAM_SRC_MAC_HI))
    {
        COMPILER_64_AND(orig_mac, new_mac);
        COMPILER_64_SET(new_mac, 0, val);
        COMPILER_64_SHL(new_mac, 24);
    }
    else
    {
        COMPILER_64_SHL(new_mac, 24);
        COMPILER_64_AND(orig_mac, new_mac);
        COMPILER_64_SET(new_mac, 0, val);
    }
    COMPILER_64_OR(new_mac, orig_mac);
    /*
     * Configure new MAC
     */
    if (flags & (FLEXE_CORE_OH_OAM_DEST_MAC_HI | FLEXE_CORE_OH_OAM_DEST_MAC_LO))
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_oh_oam_dest_mac_set(unit, new_mac));
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_oh_oam_src_mac_set(unit, new_mac));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get FlexE oh/oam packet MAC
 */
shr_error_e
flexe_std_oh_oam_pkt_mac_get(
    int unit,
    uint32 flags,
    uint32 *val)
{
    uint64 orig_mac = COMPILER_64_INIT(0, 0), mac_mask;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get original MAC
     */
    if (flags & (FLEXE_CORE_OH_OAM_DEST_MAC_HI | FLEXE_CORE_OH_OAM_DEST_MAC_LO))
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_oh_oam_dest_mac_get(unit, &orig_mac));
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_oh_oam_src_mac_get(unit, &orig_mac));
    }
    COMPILER_64_SET(mac_mask, 0, 0xFFFFFF);
    if (flags & (FLEXE_CORE_OH_OAM_DEST_MAC_HI | FLEXE_CORE_OH_OAM_SRC_MAC_HI))
    {
        COMPILER_64_SHR(orig_mac, 24);
    }
    COMPILER_64_AND(orig_mac, mac_mask);
    *val = COMPILER_64_LO(orig_mac);
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Enable/disable FlexE BAS OAM to external processor
 */
shr_error_e
flexe_std_bas_oam_to_ext_processor_set(
    int unit,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_bas_oam_to_ext_processor_set(unit, enable));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get FlexE BAS OAM to external processor enable status
 */
shr_error_e
flexe_std_bas_oam_to_ext_processor_get(
    int unit,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_bas_oam_to_ext_processor_get(unit, enable));
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Get 1588 2-step tx timestamp for FlexE phy
 */
shr_error_e
flexe_std_ptp_2step_tx_info_get(
    int unit,
    int flexe_core_port,
    uint32 *valid,
    uint64 *timestamp)
{
    int instance = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(valid, _SHR_E_PARAM, "valid");
    SHR_NULL_CHECK(timestamp, _SHR_E_PARAM, "timestamp");

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.instance.get(unit, flexe_core_port, &instance));
    SHR_IF_ERR_EXIT(soc_flexe_std_ptp_2step_tx_info_get(unit, instance, valid, timestamp));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Configure the Jitter tolerance level for SAR client
 */
shr_error_e
flexe_std_sar_jitter_fifo_level_set(
    int unit,
    int client_channel,
    const flexe_core_sar_jitter_fifo_level_t * fifo_level)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_sar_jitter_fifo_level_set(unit, client_channel, fifo_level));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get the Jitter tolerance level for SAR client
 */
shr_error_e
flexe_std_sar_jitter_fifo_level_get(
    int unit,
    int client_channel,
    flexe_core_sar_jitter_fifo_level_t * fifo_level)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_sar_jitter_fifo_level_get(unit, client_channel, fifo_level));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Enable/disable SAR client
 */
shr_error_e
flexe_std_sar_enable_set(
    int unit,
    int client_channel,
    uint32 flags,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get SAR client enable/disable status
 */
shr_error_e
flexe_std_sar_enable_get(
    int unit,
    int client_channel,
    uint32 flags,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get 66b switching info
 */
shr_error_e
flexe_std_66b_switch_get(
    int unit,
    const flexe_core_66bswitch_channel_info_t * src_info,
    flexe_core_66bswitch_channel_info_t * dest_info,
    int *nof_dest)
{
    SHR_FUNC_INIT_VARS(unit);

    *nof_dest = 0;

    SHR_FUNC_EXIT;
}
/**
 * \brief - Get 66b switching info from dest info
 */
shr_error_e
flexe_std_66b_switch_reverse_get(
    int unit,
    const flexe_core_66bswitch_channel_info_t * dest_info,
    flexe_core_66bswitch_channel_info_t * src_info,
    int *nof_src)
{
    SHR_FUNC_INIT_VARS(unit);

    *nof_src = 0;

    SHR_FUNC_EXIT;
}

/* } */
