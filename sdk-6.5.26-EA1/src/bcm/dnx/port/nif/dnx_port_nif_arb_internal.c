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
#include "dnx_port_nif_calendar_internal.h"
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/port/imb/imb_ethu.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stif.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_enum_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_nif_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_nif_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/* Static functions */
/* General */

/* dbal */
static shr_error_e dnx_port_arb_mac_mode_config_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *mac_mode);
static shr_error_e dnx_port_stop_data_table_id_get(
    int unit,
    bcm_port_t logical_port,
    dbal_tables_e * table_id);
static shr_error_e dnx_port_map_destination_table_id_get(
    int unit,
    bcm_port_t logical_port,
    dbal_tables_e * table_id);
static shr_error_e dnx_port_init_credits_table_id_get(
    int unit,
    bcm_port_t logical_port,
    dbal_tables_e * table_id);
static shr_error_e dnx_port_arb_tmac_msw_credits_set(
    int unit,
    bcm_port_t logical_port,
    int credits);

shr_error_e
dnx_port_arb_tx_context_get(
    int unit,
    int logical_port,
    int *arb_context)
{
    dnx_algo_port_info_s port_info;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check arbiter context type 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1)))
    {
        /*
         * Get ETHU access info
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, logical_port, &ethu_info));
        *arb_context = ethu_info.lane_in_core;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, FALSE, FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, logical_port, arb_context));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "logical port %d is invalid (not an eth nor otn).", logical_port);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_stop_data_set(
    int unit,
    bcm_port_t logical_port,
    int enable)
{
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    uint32 entry_handle_id;
    int arb_context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(dnx_port_stop_data_table_id_get(unit, logical_port, &table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /*
     * Get arbiter context
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_tx_context_get(unit, logical_port, &arb_context));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ARB_CONTEXT, arb_context);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STOP_TRAFFIC, INST_SINGLE, enable);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_rx_pm_default_min_pkt_size_set(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_RX_PM_MIN_PKT_SIZE, &entry_handle_id));

    /*
     * set rx pm default minimal packet size
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_PKT_SIZE_SET, INST_SINGLE,
                                 dnx_data_nif.arb.rx_pm_default_min_pkt_size_get(unit));

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_disable_datapath_set(
    int unit,
    bcm_port_t logical_port,
    int disable_datapath)
{
    uint32 entry_handle_id;
    int arb_context, core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_PORT_DISABLE_DATAPATH, &entry_handle_id));

    /*
     * Get arbiter context
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, logical_port, 0, &core, &arb_context));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ARB_CONTEXT, arb_context);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_DATAPATH, INST_SINGLE, disable_datapath);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_map_destination_set(
    int unit,
    bcm_port_t logical_port,
    dnx_port_map_destination_e mapping_type)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;
    int arb_context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(dnx_port_map_destination_table_id_get(unit, logical_port, &table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /*
     * Get arbiter context
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_tx_context_get(unit, logical_port, &arb_context));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ARB_CONTEXT, arb_context);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ARB_MAP_DESTINATION, INST_SINGLE, mapping_type);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_map_destination_get(
    int unit,
    bcm_port_t logical_port,
    dnx_port_map_destination_e * mapping_type)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;
    int arb_context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get arbiter context
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_tx_context_get(unit, logical_port, &arb_context));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(dnx_port_map_destination_table_id_get(unit, logical_port, &table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ARB_CONTEXT, arb_context);

    /*
     * set value fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ARB_MAP_DESTINATION, INST_SINGLE, mapping_type);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_port_mac_mode_config_set(
    int unit,
    bcm_port_t logical_port,
    uint32 mac_mode)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, logical_port, &ethu_info));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_MAC_MODE_CONFIG, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU_ID, ethu_info.ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_MODE_CONFIG_ID, ethu_info.mac_mode_config_id);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_MODE_CONFIG, INST_SINGLE, mac_mode);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_mac_mode_get(
    int unit,
    bcm_port_t logical_port,
    int *mac_mode)
{

    bcm_pbmp_t port_phys;
    int nof_phys, first_phy, first_lane_local;
    uint32 current_mac_mode;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get the needed parameters of the port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &port_phys));
    BCM_PBMP_COUNT(port_phys, nof_phys);
    _SHR_PBMP_FIRST(port_phys, first_phy);
    first_lane_local = first_phy % dnx_data_nif.eth.mac_mode_config_lanes_nof_get(unit);

    SHR_IF_ERR_EXIT(dnx_port_arb_mac_mode_config_get(unit, logical_port, &current_mac_mode));

    SHR_IF_ERR_EXIT(imb_ethu_port_mac_mode_calc(unit, current_mac_mode, first_lane_local, nof_phys, mac_mode));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_mac_mode_config_nof_ports_get(
    int unit,
    bcm_port_t logical_port,
    int *mac_mode_config_nof_ports)
{
    bcm_pbmp_t mac_ports;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_mac_mode_config_ports_get(unit, logical_port, &mac_ports));
    BCM_PBMP_COUNT(mac_ports, *mac_mode_config_nof_ports);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_link_list_sections_required_get(
    int unit,
    int speed,
    int *nof_sections)
{
    SHR_FUNC_INIT_VARS(unit);

    if (speed == 0)
    {
        *nof_sections = 0;
    }
    else if (speed <= dnx_data_nif.arb.min_port_speed_for_link_list_section_get(unit))
    {
        *nof_sections = 1;
    }
    else
    {
        *nof_sections = UTILEX_DIV_ROUND_UP(speed, dnx_data_nif.arb.link_list_sections_denominator_get(unit)) * 2;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_pm_port_reset(
    int unit,
    bcm_port_t logical_port,
    uint32 direction,
    int in_reset)
{
    int arb_context;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get arbiter context
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_tx_context_get(unit, logical_port, &arb_context));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_PM_PORT_CONTROL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ARB_CONTEXT, arb_context);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DIRECTION,
                               ((direction ==
                                 ARB_TX_DIRECTION) ? DBAL_ENUM_FVAL_CONNECTION_DIRECTION_TX :
                                DBAL_ENUM_FVAL_CONNECTION_DIRECTION_RX));

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_RESET, INST_SINGLE, in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_tmac_port_reset(
    int unit,
    bcm_port_t logical_port,
    uint32 direction,
    int in_reset)
{
    int arb_context;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (direction == ARB_TX_DIRECTION)
    {
        /*
         * Get arbiter context
         */
        SHR_IF_ERR_EXIT(dnx_port_arb_tx_context_get(unit, logical_port, &arb_context));

        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_TMAC_PORT_CONTROL, &entry_handle_id));

        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ARB_CONTEXT, arb_context);

        /*
         * set value fields
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_RESET, INST_SINGLE, in_reset ?
                                     DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                     DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);

        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_tmac_credits_calc_and_set(
    int unit,
    bcm_port_t logical_port,
    int speed)
{
    int credits;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * The credits are equal with the nof link lists
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_link_list_sections_required_get(unit, speed, &credits));
    SHR_IF_ERR_EXIT(dnx_port_arb_tmac_msw_credits_set(unit, logical_port, credits));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_credits_init_set(
    int unit,
    bcm_port_t logical_port,
    int enable)
{
    int arb_context;
    dbal_tables_e table_id;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get arbiter context
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_tx_context_get(unit, logical_port, &arb_context));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(dnx_port_init_credits_table_id_get(unit, logical_port, &table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ARB_CONTEXT, arb_context);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDITS_INIT, INST_SINGLE, enable);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_port_arb_sif_enable_set(
    int unit,
    int sif_port_id,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_TX_SIF_CONFIG, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SIF_PORT_ID, sif_port_id);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_ENABLE, INST_SINGLE, enable);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_sif_port_set(
    int unit,
    int sif_port_id,
    int offset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_TX_SIF_CONFIG, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SIF_PORT_ID, sif_port_id);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET, INST_SINGLE, offset);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_sif_reset_set(
    int unit,
    int sif_port_id,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_TX_SIF_CONFIG, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SIF_PORT_ID, sif_port_id);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET_ASYNC_FIFO, INST_SINGLE, enable);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * General static functions
 */

/*
 * DBAL static functions
 */

static shr_error_e
dnx_port_stop_data_table_id_get(
    int unit,
    bcm_port_t logical_port,
    dbal_tables_e * table_id)
{
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check arbiter context type 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1)))
    {
        *table_id = DBAL_TABLE_NIF_ARB_PM_PORT_STOP_TRAFFIC;

    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, FALSE, FALSE))
    {
        *table_id = DBAL_TABLE_NIF_ARB_TMAC_PORT_STOP_TRAFFIC;
    }
    else
    {
        *table_id = DBAL_TABLE_EMPTY;
        SHR_ERR_EXIT(_SHR_E_PARAM, "logical port %d is invalid (not an eth nor otn).", logical_port);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_map_destination_table_id_get(
    int unit,
    bcm_port_t logical_port,
    dbal_tables_e * table_id)
{
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check arbiter context type 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1)))
    {
        *table_id = DBAL_TABLE_NIF_ARB_PM_PORT_MAP_DESTINATION;

    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, FALSE, FALSE))
    {
        *table_id = DBAL_TABLE_NIF_ARB_TMAC_PORT_MAP_DESTINATION;
    }
    else
    {
        *table_id = DBAL_TABLE_EMPTY;
        SHR_ERR_EXIT(_SHR_E_PARAM, "logical port %d is invalid (not an eth nor otn).", logical_port);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_init_credits_table_id_get(
    int unit,
    bcm_port_t logical_port,
    dbal_tables_e * table_id)
{
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check arbiter context type 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1)))
    {
        *table_id = DBAL_TABLE_NIF_ARB_PM_CREDITS_CONFIG;

    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, FALSE, FALSE))
    {
        *table_id = DBAL_TABLE_NIF_ARB_TMAC_CREDITS_CONFIG;
    }
    else
    {
        *table_id = DBAL_TABLE_EMPTY;
        SHR_ERR_EXIT(_SHR_E_PARAM, "logical port %d is invalid (not an eth nor otn).", logical_port);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_arb_mac_mode_config_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *mac_mode)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, logical_port, &ethu_info));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_MAC_MODE_CONFIG, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU_ID, ethu_info.ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_MODE_CONFIG_ID, ethu_info.mac_mode_config_id);

    /*
     * set value fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAC_MODE_CONFIG, INST_SINGLE, mac_mode);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_global_powerdown_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_GLOBAL_POWER_DOWN, &entry_handle_id));

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POWER_DOWN_EN, INST_SINGLE, enable);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_pm_powerdown_set(
    int unit,
    int cdu_id,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_PM_POWER_DOWN, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU_ID, cdu_id);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POWER_DOWN_EN, INST_SINGLE, enable);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_tmac_powerdown_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_TMAC_POWER_DOWN, &entry_handle_id));

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POWER_DOWN_EN, INST_SINGLE, enable);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_arb_tmac_msw_credits_set(
    int unit,
    bcm_port_t logical_port,
    int credits)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;
    int arb_context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get arbiter context
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_tx_context_get(unit, logical_port, &arb_context));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(dnx_port_init_credits_table_id_get(unit, logical_port, &table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ARB_CONTEXT, arb_context);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_CREDITS, INST_SINGLE, credits);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_enable_dynamic_memories_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_ENABLE_MEMORIES, &entry_handle_id));

    /*
     * Enabling OFT memories
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INIT_MEMORY, INST_SINGLE, 1);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_pm_credits_hw_set(
    int unit,
    bcm_port_t logical_port,
    int credits_val)
{
    int arb_context;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get arbiter context
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_tx_context_get(unit, logical_port, &arb_context));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_PM_CREDITS_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ARB_CONTEXT, arb_context);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDITS_VALUE, INST_SINGLE, credits_val);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    DBAL_HANDLE_FREE(unit, entry_handle_id);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_PM_CREDITS_CONFIG, &entry_handle_id));

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDITS_VALUE_SET_ENABLE, INST_SINGLE, 1);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDITS_VALUE_SET_ENABLE, INST_SINGLE, 0);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_rx_calendar_ids_get(
    int unit,
    bcm_port_t logical_port,
    bcm_pbmp_t * calendars_to_be_set)
{
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Rx scheduler calendar should always recalculated
     */
    _SHR_PBMP_PORT_ADD(*calendars_to_be_set, DNX_PORT_NIF_ARB_RX_SCH_CALENDAR);

    /*
     * Check if port is eth or tmac
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0))
    {
        _SHR_PBMP_PORT_ADD(*calendars_to_be_set, DNX_PORT_NIF_ARB_RX_QPM_CALENDAR);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_tx_calendar_ids_get(
    int unit,
    bcm_port_t logical_port,
    bcm_pbmp_t * calendars_to_be_set)
{
    int core, nif_port;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get nif port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, logical_port, 0, &core, &nif_port));

    /*
     * Check if port is eth or tmac
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0))
    {
        _SHR_PBMP_PORT_ADD(*calendars_to_be_set,
                           (DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR +
                            (nif_port / dnx_data_nif.eth.nof_lanes_in_cdu_get(unit))));
    }
    else
    {
        _SHR_PBMP_PORT_ADD(*calendars_to_be_set, DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_init_all_calendars(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize arb calendars.
     */
    SHR_IF_ERR_EXIT(dnx_port_calendar_init(unit, DNX_PORT_NIF_ARB_RX_QPM_CALENDAR));
    SHR_IF_ERR_EXIT(dnx_port_calendar_init(unit, DNX_PORT_NIF_ARB_RX_SCH_CALENDAR));
    SHR_IF_ERR_EXIT(dnx_port_calendar_init(unit, DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR));
    SHR_IF_ERR_EXIT(dnx_port_calendar_init(unit, DNX_PORT_NIF_ARB_TX_PPM1_CALENDAR));
    SHR_IF_ERR_EXIT(dnx_port_calendar_init(unit, DNX_PORT_NIF_ARB_TX_PPM2_CALENDAR));
    SHR_IF_ERR_EXIT(dnx_port_calendar_init(unit, DNX_PORT_NIF_ARB_TX_PPM3_CALENDAR));
    SHR_IF_ERR_EXIT(dnx_port_calendar_init(unit, DNX_PORT_NIF_ARB_TX_PPM4_CALENDAR));
    SHR_IF_ERR_EXIT(dnx_port_calendar_init(unit, DNX_PORT_NIF_ARB_TX_PPM5_CALENDAR));
    SHR_IF_ERR_EXIT(dnx_port_calendar_init(unit, DNX_PORT_NIF_ARB_TX_PPM6_CALENDAR));
    SHR_IF_ERR_EXIT(dnx_port_calendar_init(unit, DNX_PORT_NIF_ARB_TX_PPM7_CALENDAR));
    SHR_IF_ERR_EXIT(dnx_port_calendar_init(unit, DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_tx_bypass_fifo_reset_set(
    int unit,
    int in_reset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_FIFO_RESET, &entry_handle_id));

    /*
     * Enabling OFT memories
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_RESET, INST_SINGLE, in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_tmac_start_tx_threshold_hw_set(
    int unit,
    bcm_port_t logical_port,
    int threshold_value)
{
    uint32 entry_handle_id;
    int arb_context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get arbiter context
     */
    SHR_IF_ERR_EXIT(dnx_port_arb_tx_context_get(unit, logical_port, &arb_context));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_TX_TMAC_THRESHOLD, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ARB_CONTEXT, arb_context);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_THRESHOLD, INST_SINGLE, threshold_value);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
