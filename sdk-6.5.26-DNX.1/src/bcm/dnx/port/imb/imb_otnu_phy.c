/** \file imb_otnu_phy.c
 *
 *  OTN physical ports procedures for DNX.
 *
 */
/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <sal/core/boot.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/port/imb/imb_otnu_phy.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#include "imb_utils.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include "imb_framer_internal.h"
#include "imb_ethu_internal.h"
#include "imb_cdu_internal.h"
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/recovery/generic_state_journal.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#define IMB_OTNU_PHY_PORT_PAM4_BITMUX_EVEN_ODD 0
#define IMB_OTNU_PHY_PORT_PAM4_BITMUX_MSB20_LSB20 1
#define IMB_OTNU_PHY_PORT_PAM4_LANE_ORDER_NORMAL 0
#define IMB_OTNU_PHY_PORT_PAM4_LANE_ORDER_REVERSE 1

int imb_cdu_port_tx_power_down_set(
    int unit,
    bcm_port_t port,
    int power_down);

/**
 * \brief - get port macro id by logical port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] pm_id - port macro id
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_otnu_phy_port_pm_id_get(
    int unit,
    bcm_port_t port,
    int *pm_id)
{
    int first_phy_port;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0x0, &first_phy_port));
    SHR_IF_ERR_EXIT(portmod_phy_pm_id_get(unit, first_phy_port, pm_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - initialize Portmod core info before calling
 *        portmod_core_add
 *
 * \param [in] unit - chip unit id.
 * \param [in] pm_id - PM index
 * \param [in] add_info - portmod add info
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_otnu_phy_portmod_add_info_core_config(
    int unit,
    int pm_id,
    portmod_port_add_info_t * add_info)
{
    int i, nof_pm_lanes;
    int pm_lower_bound, pm_upper_bound;
    soc_dnxc_lane_map_db_map_t *lane2serdes = NULL;
    bcm_pbmp_t pm_phys;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Configure lane map info. Get the lane map info from swstate
     */
    pm_phys = dnx_data_nif.eth.pm_properties_get(unit, pm_id)->phys;
    _SHR_PBMP_FIRST(pm_phys, pm_lower_bound);
    _SHR_PBMP_LAST(pm_phys, pm_upper_bound);
    nof_pm_lanes = pm_upper_bound - pm_lower_bound + 1;
    SHR_ALLOC(lane2serdes, nof_pm_lanes * sizeof(soc_dnxc_lane_map_db_map_t), "Lane to Serdes Map", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_pm_lane_to_serdes_map_get
                    (unit, DNX_ALGO_LANE_MAP_NIF_SIDE, pm_lower_bound, pm_upper_bound, lane2serdes));

    for (i = 0; i < nof_pm_lanes; i++)
    {
        /*
         * Coverity:
         * pm_phys is getting from dnx-data which is constant
         * so 'pm_upper_bound - pm_lower_bound + 1' won't exceed number of lanes of the PM
         */
         /* coverity[overrun-local : FALSE]  */
        add_info->init_config.lane_map[0].lane_map_rx[i] = lane2serdes[i].rx_id;
        add_info->init_config.lane_map[0].lane_map_tx[i] = lane2serdes[i].tx_id;
    }
    add_info->init_config.lane_map[0].num_of_lanes = nof_pm_lanes;
    /*
     * Alway overwrite the lane map
     */
    add_info->init_config.lane_map_overwrite = 1;
    /*
     * The following configurations don't need to be overwritten.
     */
    add_info->init_config.polarity_overwrite = 0;
    add_info->init_config.fw_load_method_overwrite = 0;
    add_info->init_config.ref_clk_overwrite = 0;

    PORTMOD_PORT_ADD_F_SKIP_SPEED_INIT_SET(add_info);
    PORTMOD_PORT_ADD_F_AUTONEG_CONFIG_SKIP_SET(add_info);
    PORTMOD_PORT_ADD_F_RX_SRIP_CRC_SET(add_info);
    /*
     * Set OTN flag
     */
    PORTMOD_PORT_ADD_F_OTN_CONFIG_SET(add_info);

    if (dnx_data_port.static_add.nif_fw_crc_check_get(unit))
    {
        PORTMOD_PORT_ADD_F_BYPASS_FW_CRC_CHECK_CLR(add_info);
    }
    else
    {
        PORTMOD_PORT_ADD_F_BYPASS_FW_CRC_CHECK_SET(add_info);
    }

    if (dnx_data_port.static_add.nif_fw_load_verify_get(unit))
    {
        PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_SET(add_info);
    }
    else
    {
        PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_CLR(add_info);
    }

    add_info->interface_config.interface = SOC_PORT_IF_OTN;

exit:
    SHR_FREE(lane2serdes);
    SHR_FUNC_EXIT;
}

/**
 * \brief - initialize Portmod add_info before calling
 *        portmod_port_add
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] add_info - portmod add info
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_otnu_phy_portmod_add_info_config(
    int unit,
    bcm_port_t port,
    portmod_port_add_info_t * add_info)
{
    int pm_id;
    int is_flr_fw_support;

    SHR_FUNC_INIT_VARS(unit);

    /** Initialize core based configuruation */
    SHR_IF_ERR_EXIT(imb_otnu_phy_port_pm_id_get(unit, port, &pm_id));
    SHR_IF_ERR_EXIT(imb_otnu_phy_portmod_add_info_core_config(unit, pm_id, add_info));

    /** Initialize port based configuruation */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, (bcm_pbmp_t *) & (add_info->phys)));

    /** Get port num of lanes */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &add_info->interface_config.port_num_lanes));

    /** Update the FLR flag */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_fw_support_get(unit, port, &is_flr_fw_support));
    if (is_flr_fw_support)
    {
        PORTMOD_PORT_ADD_F_FLR_SUPPORT_SET(add_info);
    }
    else
    {
        PORTMOD_PORT_ADD_F_FLR_SUPPORT_CLR(add_info);
    }
    /*
     * Set Bypass flag
     */
    PORTMOD_PORT_ADD_F_PCS_BYPASSED_SET(add_info);
    /** Enable TXPI mode */
    add_info->init_config.txpi_mode = 1;
    add_info->init_config.txpi_sdm_scheme = dnx_data_nif.phys.txpi_sdm_scheme_get(unit);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Rollback for portmod_port_add
 */
static shr_error_e
imb_otnu_phy_port_portmod_port_add_er_rollback(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    int *port = NULL;

    SHR_FUNC_INIT_VARS(unit);

    port = (int *) metadata;

    SHR_IF_ERR_EXIT(portmod_port_remove(unit, *port));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Rollback for portmod_port_remove
 */
static shr_error_e
imb_otnu_phy_port_portmod_port_remove_er_rollback(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    int *port = NULL;
    portmod_port_add_info_t add_info;
    int pm_id;
    int core_add_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    port = (int *) metadata;

    SHR_IF_ERR_EXIT(imb_otnu_phy_port_pm_id_get(unit, *port, &pm_id));

    /** Configure Portmod */
    SHR_IF_ERR_EXIT(portmod_port_add_info_t_init(unit, &add_info));
    SHR_IF_ERR_EXIT(imb_otnu_phy_portmod_add_info_config(unit, *port, &add_info));
    PORTMOD_CORE_ADD_SKIP_MAC_SET(core_add_flags);
    SHR_IF_ERR_EXIT(portmod_core_add(unit, pm_id, core_add_flags, &add_info));
    SHR_IF_ERR_EXIT(portmod_port_add(unit, *port, &add_info));
    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, *port, 0, 0));
exit:
    SHR_FUNC_EXIT;
}

static int
imb_otnu_phy_port_mlane_group_enable_set(
    int unit,
    int port,
    int nof_lanes,
    int *lanes,
    int enable,
    int *status)
{
    int i;
    uint64 mlane_members;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OTNU_MLANE_GROUP_CTRL, &entry_handle_id));
    /*
     * set value fields
     */
    COMPILER_64_ZERO(mlane_members);
    if (enable)
    {
        for (i = 0; i < nof_lanes; i++)
        {
            COMPILER_64_BITSET(mlane_members, lanes[i]);
        }
    }
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_MLANE_GROUP_MEMBERS, INST_SINGLE, mlane_members);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MLANE_GROUP_ENABLE, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (status)
    {
        *status = enable;
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
imb_otnu_phy_port_tx_lane_map_set(
    int unit,
    int pm_id,
    int pm_internal_lane,
    int pmux_idx)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OTNU_TX_PMUX, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_ID, pm_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_INTERNAL_LANE, pm_internal_lane);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_MUX_SEL, INST_SINGLE, pmux_idx);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
imb_otnu_phy_port_tx_lane_map_get(
    int unit,
    int pm_id,
    int pm_internal_lane,
    int *pmux_idx)
{
    uint32 entry_handle_id;
    uint32 pmux_index_get;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OTNU_TX_PMUX, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_ID, pm_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_INTERNAL_LANE, pm_internal_lane);
    /*
     * Get field value
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PORT_MUX_SEL, INST_SINGLE, &pmux_index_get);
    /*
     * Get the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *pmux_idx = pmux_index_get;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
imb_otnu_phy_port_tx_lane_map_clear(
    int unit,
    int pm_id,
    int pm_internal_lane)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OTNU_TX_PMUX, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_ID, pm_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_INTERNAL_LANE, pm_internal_lane);
    /*
     * Clear the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
imb_otnu_phy_port_rx_lane_map_set(
    int unit,
    int framer_pm,
    int framer_pm_lane,
    int pmux_index)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OTNU_RX_PMUX, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FRAMER_PM, framer_pm);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FRAMER_PM_LANE, framer_pm_lane);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_MUX_SEL, INST_SINGLE, pmux_index);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
imb_otnu_phy_port_rx_lane_map_get(
    int unit,
    int framer_pm,
    int framer_pm_lane,
    int *pmux_index)
{
    uint32 entry_handle_id;
    uint32 pmux_index_get;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OTNU_RX_PMUX, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FRAMER_PM, framer_pm);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FRAMER_PM_LANE, framer_pm_lane);
    /*
     * Get field value
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PORT_MUX_SEL, INST_SINGLE, &pmux_index_get);
    /*
     * Get the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *pmux_index = pmux_index_get;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
imb_otnu_phy_port_rx_lane_map_clear(
    int unit,
    int framer_pm,
    int framer_pm_lane)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OTNU_RX_PMUX, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FRAMER_PM, framer_pm);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FRAMER_PM_LANE, framer_pm_lane);
    /*
     * Clear the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
imb_otnu_phy_port_lane_map_cfg(
    int unit,
    int pm_id,
    int pm_lane,
    int flags,
    int pam4_enable,
    int pam4_bitmux,
    int pam4_lane_order)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (flags & IMB_COMMON_DIRECTION_RX)
    {
        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OTNU_CTRL, &entry_handle_id));

        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_ID, pm_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_LANE, pm_lane);

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DIRECTION, DBAL_ENUM_FVAL_CONNECTION_DIRECTION_RX);
        /*
         * set value fields
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PAM4_ENABLE, INST_SINGLE, pam4_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PAM4_BITMUX, INST_SINGLE, pam4_bitmux);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PAM4_LANE_ORDER, INST_SINGLE, pam4_lane_order);

        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Clear the handle before re-use
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_OTNU_CTRL, entry_handle_id));
    }

    if (flags & IMB_COMMON_DIRECTION_TX)
    {

        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OTNU_CTRL, &entry_handle_id));

        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_ID, pm_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_LANE, pm_lane);

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DIRECTION, DBAL_ENUM_FVAL_CONNECTION_DIRECTION_TX);
        /*
         * set value fields
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PAM4_ENABLE, INST_SINGLE, pam4_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PAM4_BITMUX, INST_SINGLE, pam4_bitmux);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PAM4_LANE_ORDER, INST_SINGLE, pam4_lane_order);

        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
imb_otnu_phy_port_lane_map_clear(
    int unit,
    bcm_port_t port)
{
    int phy_lanes[DNX_DATA_MAX_NIF_OTN_NOF_LANES] = { 0 };
    int i, nof_phys;
    int pm_id, pm_lane, pm_internal_lane, pm_internal_lane_1;
    int framer_pm, framer_pm_port, framer_pm_port_1;
    int pmux_idx, pmux_idx_get, pmux_idx_1, pmux_idx_get_1;
    int nof_lanes_per_group, nof_groups;
    int nof_logical_lanes, logical_otn_lane;
    int is_pam4 = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_otn_logical_lane_get(unit, port, &nof_logical_lanes, &logical_otn_lane));
    if (nof_logical_lanes == 0)
    {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_otn_lanes_get(unit, port, DNX_DATA_MAX_NIF_OTN_NOF_LANES, phy_lanes, &nof_phys));
    is_pam4 = nof_logical_lanes == nof_phys ? 0 : 1;

    nof_groups = dnx_data_nif.otn.nof_groups_get(unit);
    nof_lanes_per_group = dnx_data_nif.otn.nof_lanes_per_group_get(unit);
    if (is_pam4)
    {
        for (i = 0; i < nof_phys; i++)
        {
            pm_id = phy_lanes[i] / nof_lanes_per_group;
            pm_lane = phy_lanes[i] % nof_lanes_per_group;
            pm_internal_lane = pm_lane * 2;
            pm_internal_lane_1 = pm_internal_lane + 1;

            framer_pm_port = pm_internal_lane % nof_lanes_per_group;
            framer_pm_port_1 = pm_internal_lane_1 % nof_lanes_per_group;

            pmux_idx = pm_id * 3 + (pm_internal_lane / nof_lanes_per_group) * 2;
            pmux_idx_1 = pm_id * 3 + (pm_internal_lane_1 / nof_lanes_per_group) * 2;
            for (framer_pm = 0; framer_pm < nof_groups; framer_pm++)
            {
                SHR_IF_ERR_EXIT(imb_otnu_phy_port_rx_lane_map_get(unit, framer_pm, framer_pm_port, &pmux_idx_get));
                if (pmux_idx_get == pmux_idx)
                {
                    break;
                }
            }
            if (framer_pm == nof_groups)
            {
                SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
            }
            SHR_IF_ERR_EXIT(imb_otnu_phy_port_rx_lane_map_clear(unit, framer_pm, framer_pm_port));
            for (framer_pm = 0; framer_pm < nof_groups; framer_pm++)
            {
                SHR_IF_ERR_EXIT(imb_otnu_phy_port_rx_lane_map_get(unit, framer_pm, framer_pm_port_1, &pmux_idx_get_1));
                if (pmux_idx_get_1 == pmux_idx_1)
                {
                    break;
                }
            }
            if (framer_pm == nof_groups)
            {
                SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
            }
            SHR_IF_ERR_EXIT(imb_otnu_phy_port_rx_lane_map_clear(unit, framer_pm, framer_pm_port_1));

            SHR_IF_ERR_EXIT(imb_otnu_phy_port_tx_lane_map_clear(unit, pm_id, pm_internal_lane));
            SHR_IF_ERR_EXIT(imb_otnu_phy_port_tx_lane_map_clear(unit, pm_id, pm_internal_lane_1));

            SHR_IF_ERR_EXIT(imb_otnu_phy_port_lane_map_cfg
                            (unit, pm_id, pm_lane, IMB_COMMON_DIRECTION_RX | IMB_COMMON_DIRECTION_TX, 0,
                             IMB_OTNU_PHY_PORT_PAM4_BITMUX_EVEN_ODD, IMB_OTNU_PHY_PORT_PAM4_LANE_ORDER_NORMAL));
        }
    }
    else
    {
        for (i = 0; i < nof_phys; i++)
        {
            pm_id = phy_lanes[i] / nof_lanes_per_group;
            pm_lane = phy_lanes[i] % nof_lanes_per_group;
            pm_internal_lane = pm_lane * 2;
            framer_pm_port = pm_lane;
            pmux_idx = pm_id * 3 + 1;
            for (framer_pm = 0; framer_pm < nof_groups; framer_pm++)
            {
                SHR_IF_ERR_EXIT(imb_otnu_phy_port_rx_lane_map_get(unit, framer_pm, framer_pm_port, &pmux_idx_get));
                if (pmux_idx_get == pmux_idx)
                {
                    break;
                }
            }
            if (framer_pm == nof_groups)
            {
                SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
            }
            SHR_IF_ERR_EXIT(imb_otnu_phy_port_rx_lane_map_clear(unit, framer_pm, framer_pm_port));
            SHR_IF_ERR_EXIT(imb_otnu_phy_port_tx_lane_map_clear(unit, pm_id, pm_internal_lane));

            SHR_IF_ERR_EXIT(imb_otnu_phy_port_lane_map_cfg
                            (unit, pm_id, pm_lane, IMB_COMMON_DIRECTION_RX | IMB_COMMON_DIRECTION_TX, 0,
                             IMB_OTNU_PHY_PORT_PAM4_BITMUX_EVEN_ODD, IMB_OTNU_PHY_PORT_PAM4_LANE_ORDER_NORMAL));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static int
imb_otnu_phy_port_pm_lane_reset_set(
    int unit,
    int pm_id,
    int lane_id,
    int in_reset)
{
    int reset;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OTNU_PM_LANE_RESET, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_ID, pm_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_LANE, lane_id);
    /*
     * set value fields
     */
    reset =
        in_reset ? DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
        DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_RESET, INST_SINGLE, reset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_RESET, INST_SINGLE, reset);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
imb_otnu_phy_port_reset_set(
    int unit,
    int port,
    int in_reset)
{
    int pm_id, pm_lane, i, nof_lanes, nof_otn_lanes, mlane_enabled = 0;
    int lanes[DNX_DATA_MAX_NIF_OTN_NOF_LANES] = { 0 };
    int otn_lanes[DNX_DATA_MAX_NIF_OTN_NOF_LANES] = { 0 };
    int out_reset = IMB_COMMON_OUT_OF_RESET == in_reset ? 1 : 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_otnu_phy_port_pm_id_get(unit, port, &pm_id));
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_otn_lanes_get(unit, port, DNX_DATA_MAX_NIF_OTN_NOF_LANES, lanes, &nof_lanes));
    if (out_reset)
    {
        SHR_IF_ERR_EXIT(imb_otnu_phy_port_lane_map_get
                        (unit, port, 0, DNX_DATA_MAX_NIF_OTN_NOF_LANES, otn_lanes, &nof_otn_lanes));
        SHR_IF_ERR_EXIT(imb_otnu_phy_port_mlane_group_enable_set
                        (unit, port, nof_otn_lanes, otn_lanes, 1, &mlane_enabled));
        sal_usleep(1000);
    }
    for (i = 0; i < nof_lanes; i++)
    {
        pm_lane = lanes[i] % dnx_data_nif.eth.nof_cdu_lanes_in_pm_get(unit);
        SHR_IF_ERR_EXIT(imb_otnu_phy_port_pm_lane_reset_set(unit, pm_id, pm_lane, in_reset));
    }
    if (out_reset)
    {
        sal_usleep(1000);
    }
exit:
    if (out_reset && mlane_enabled)
    {
        SHR_IF_ERR_EXIT(imb_otnu_phy_port_mlane_group_enable_set
                        (unit, port, nof_otn_lanes, otn_lanes, 0, &mlane_enabled));
    }
    SHR_FUNC_EXIT;
}

int
imb_otnu_phy_init(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

int
imb_otnu_phy_deinit(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add new port to the FEU_PHY. config all FEU PHY settings and
 *         call Portmod API for PM configurations
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port to be attched to the FEU_PHY
 * \param [in] flags - not used
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_otnu_phy_port_attach(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    int pm_id;
    portmod_port_add_info_t add_info;
    int core_add_flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_otnu_phy_port_pm_id_get(unit, port, &pm_id));
    if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_clock_power_down) &&
        dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_tx_separate_clock_power_down))
    {
        /** Power up TX */
        SHR_IF_ERR_EXIT(imb_cdu_port_tx_power_down_set(unit, port, FALSE));
    }
    /** Configure Portmod */
    SHR_IF_ERR_EXIT(portmod_port_add_info_t_init(unit, &add_info));
    SHR_IF_ERR_EXIT(imb_otnu_phy_portmod_add_info_config(unit, port, &add_info));
    PORTMOD_CORE_ADD_SKIP_MAC_SET(core_add_flags);
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(portmod_core_add(unit, pm_id, core_add_flags, &add_info));
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(portmod_port_add(unit, port, &add_info));
    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        &imb_otnu_phy_port_portmod_port_add_er_rollback, FALSE));
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, 0));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove a port from the OTU_PHY. first calls Portmos API
 *        to remove from portmod DB, then configures all OTU_PHY
 *        settings.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_otnu_phy_port_detach(
    int unit,
    bcm_port_t port)
{
    int enable, pm_id;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify if the port is enabled
     */
    SHR_IF_ERR_EXIT(imb_otnu_phy_port_enable_get(unit, port, &enable));
    if (enable)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Cannot detach active port %d", port);
    }
    /*
     * Get PM ID before removing the port in portmod
     */
    SHR_IF_ERR_EXIT(portmod_port_pm_id_get(unit, port, &pm_id));
    SHR_IF_ERR_EXIT(imb_otnu_phy_port_lane_map_set(unit, port, 0, NULL, 0));
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(portmod_port_remove(unit, port));
    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        &imb_otnu_phy_port_portmod_port_remove_er_rollback, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get recommended default values of resource members for given port with speed
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - not used
 * \param [out] resource - each memeber other than speed with BCM_PORT_RESOURCE_DEFAULT_REQUEST
 *                         will be filled with default value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_otnu_phy_port_resource_default_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_port_resource_t * resource)
{
    int is_flr_fw_support;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");

    if ((resource->flags != 0) || (resource->physical_port != 0) || (resource->encap != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "port %d: flags, physical_port and encap fields of bcm_port_resource_t are not in use. They should all be initialized to 0",
                     port);
    }

    if ((resource->port != 0) && (resource->port != port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "port %d: resource->port should be 0 or equal to port passed to the API", port);
    }

    if (resource->speed == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "port %d: there is no given default value for speed. speed is mandatory as input to this API",
                     port);
    }

    /*
     * in case user didn't configure the FEC already - give default value
     */
    if (resource->fec_type == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->fec_type = bcmPortPhyFecNone;
    }

    /*
     * Get the FLR firmware status. FLR firmware doesn't support Link traning and Force ES
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_fw_support_get(unit, port, &is_flr_fw_support));
    /*
     * in case user didn't configure CL72
     *    - enable it if FLR firmware is not downloaded
     *    - disable it if FLR firmware is downloaded
     */
    if (resource->link_training == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->link_training = is_flr_fw_support ? 0 : 1;
    }
    if ((resource->link_training == 1) && is_flr_fw_support)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "port %d: Link training and FLR cannot be enabled together. \r\n", port);
    }

    /*
     * get default lane_config
     */
    if (resource->phy_lane_config == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->phy_lane_config = 0;
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(resource->phy_lane_config);
    }

    resource->port = port;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - control enable/ disable OTN PHY port
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] flags - flags
 * \param [in] enable - enable / disable indication
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_otnu_phy_port_enable_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable)
{
    uint8 is_xpmd_enabled;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_port_xpmd_is_enabled(unit, port, &is_xpmd_enabled));
    if (!enable)
    {
        SHR_IF_ERR_EXIT(imb_otnu_phy_port_reset_set(unit, port, IMB_COMMON_IN_RESET));
        if (is_xpmd_enabled)
        {
            SHR_IF_ERR_EXIT(imb_cdu_port_xpmd_lane_enable
                            (unit, port, IMB_COMMON_DIRECTION_RX | IMB_COMMON_DIRECTION_TX, 0));
        }
    }
    /*
     * Enable / Disable port in Portmod
     */
    if (!(flags & IMB_PORT_ENABLE_F_SKIP_PORTMOD))
    {
        SHR_IF_ERR_EXIT(imb_er_portmod_port_enable_set(unit, port, 0, enable));
    }

    sal_usleep(1000);

    if (enable)
    {
        if (is_xpmd_enabled)
        {
            SHR_IF_ERR_EXIT(imb_cdu_port_xpmd_lane_enable
                            (unit, port, IMB_COMMON_DIRECTION_RX | IMB_COMMON_DIRECTION_TX, 1));
        }
        SHR_IF_ERR_EXIT(imb_otnu_phy_port_reset_set(unit, port, IMB_COMMON_IN_RESET));
        SHR_IF_ERR_EXIT(imb_otnu_phy_port_reset_set(unit, port, IMB_COMMON_OUT_OF_RESET));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get enable status of the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - returned enable status
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_otnu_phy_port_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    /*
     * Get the indication from Portmod
     */
    SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, port, 0, enable));

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get port link state
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] clear_status - indication to clear latch down
 *        status
 * \param [out] link_state - link state info
 *
 * \return
 *   int - see_SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_otnu_phy_port_link_state_get(
    int unit,
    bcm_port_t port,
    int clear_status,
    bcm_port_link_state_t * link_state)
{
    SHR_FUNC_INIT_VARS(unit);

    link_state->status = BCM_PORT_LINK_STATUS_UP;
    SHR_IF_ERR_EXIT(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get link status.
 *
 *  see .h file
 */
int
imb_otnu_phy_port_link_get(
    int unit,
    int port,
    int flags,
    int *link)
{
    SHR_FUNC_INIT_VARS(unit);

    *link = 1;
    SHR_IF_ERR_EXIT(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - sync the MAC status according to the PHY status.
 *
 *  see .h file
 */
int
imb_otnu_phy_port_link_up_mac_update(
    int unit,
    bcm_port_t port,
    int link)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the OTN port in loopback.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] loopback - loopback type:
 * 0 => no loopback.
 * 2 => PHY loopback (TX->RX, inside the PM)
 * 3 => PHY Remote (RX->TX, inside the PM)
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_otnu_phy_port_loopback_set(
    int unit,
    bcm_port_t port,
    int loopback)
{
    portmod_loopback_mode_t portmod_loopback;
    int curr_loopback, enable, is_symmetric;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Since the lane map for PM8x50 has been moved to PMD, PHY/REMOTE/MAC
     * loopback cannot be configured when lane map is not symmetric.
     */
    if (loopback == BCM_PORT_LOOPBACK_PHY || loopback == BCM_PORT_LOOPBACK_PHY_REMOTE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_is_symmetric_get(unit, port, &is_symmetric));
        if (!is_symmetric)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "Lane Map for port %d is not symmetric. Configuring PHY/REMOTE loopback on non-symmetric lane swapped port is forbidden, please adjust the lane map",
                         port);
        }
    }

    SHR_IF_ERR_EXIT(imb_port_loopback_get(unit, port, &curr_loopback));
    if (curr_loopback == loopback)
    {
        /*
         * Nothing to do
         */
        SHR_EXIT();
    }

    /*
     * Get Enable status to retrieve after setting loopack
     */
    SHR_IF_ERR_EXIT(imb_port_enable_get(unit, port, &enable));

    /*
     * Disable port before closing loopback
     */
    SHR_IF_ERR_EXIT(imb_port_enable_set(unit, port, 0, 0));

    sal_usleep(10000);

    if (curr_loopback != BCM_PORT_LOOPBACK_NONE)
    {
        /*
         * Open current loopback
         */
        SHR_IF_ERR_EXIT(imb_portmod_loopback_from_bcm_loopback_get(unit, port, curr_loopback, &portmod_loopback));
        SHR_IF_ERR_EXIT(portmod_port_loopback_set(unit, port, portmod_loopback, 0));
    }
    if (loopback != BCM_PORT_LOOPBACK_NONE)
    {
        /*
         * Close requested loopback
         */
        SHR_IF_ERR_EXIT(imb_portmod_loopback_from_bcm_loopback_get(unit, port, loopback, &portmod_loopback));
        SHR_IF_ERR_EXIT(portmod_port_loopback_set(unit, port, portmod_loopback, 1));
    }

    sal_usleep(10000);
    if (enable)
    {
        SHR_IF_ERR_EXIT(imb_port_enable_set(unit, port, 0, 1));
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get loopback type on the port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] loopback - loopback type. see loopback_set for
 *        loopback types
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_otnu_phy_port_loopback_get(
    int unit,
    bcm_port_t port,
    int *loopback)
{
    int rv = 0, lb_enabled = 0;
    portmod_loopback_mode_t portmod_loopback;
    SHR_FUNC_INIT_VARS(unit);

    *loopback = 0;
    /*
     * Iterate over all portmod loopback types
     */
    for (portmod_loopback = portmodLoopbackMacOuter; portmod_loopback != portmodLoopbackCount; ++portmod_loopback)
    {
        rv = portmod_port_loopback_get(unit, port, portmod_loopback, &lb_enabled);
        if (rv == _SHR_E_UNAVAIL)
        {
            /*
             * Portmod loopback type is not supported for PM type
             */
            continue;
        }
        SHR_IF_ERR_EXIT(rv);
        if (lb_enabled)
        {
            SHR_IF_ERR_EXIT(imb_bcm_loopback_from_portmod_loopback_get(unit, port, portmod_loopback, loopback));
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set ilkn logical lane order
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - Flags to indicate the tx or rx direction.
 * \param [in] lanes - lane_order
 * \param [in] nof_lanes - lane_order array size
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
int
imb_otnu_phy_port_lane_map_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    const int *lanes,
    int nof_lanes)
{
    int phy_lanes[DNX_DATA_MAX_NIF_OTN_NOF_LANES] = { 0 };
    int i, nof_phys;
    portmod_speed_config_t port_speed_config;
    int pm_id, pm_lane, pm_internal_lane, pm_internal_lane_1;
    int framer_pm, framer_pm_lane, framer_pm_1, framer_pm_lane_1;
    int pmux_idx;
    int nof_lanes_per_group;

    SHR_FUNC_INIT_VARS(unit);

    if (nof_lanes == 0)
    {
        SHR_IF_ERR_EXIT(imb_otnu_phy_port_lane_map_clear(unit, port));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_otn_lanes_get(unit, port, DNX_DATA_MAX_NIF_OTN_NOF_LANES, phy_lanes, &nof_phys));
    SHR_IF_ERR_EXIT(portmod_speed_config_t_init(unit, &port_speed_config));
    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &port_speed_config));

    nof_lanes_per_group = dnx_data_nif.otn.nof_lanes_per_group_get(unit);
    if (DNXC_PORT_PHY_SPEED_IS_PAM4(port_speed_config.speed))
    {
        for (i = 0; i < nof_phys; i++)
        {
            pm_id = phy_lanes[i] / nof_lanes_per_group;
            pm_lane = phy_lanes[i] % nof_lanes_per_group;
            pm_internal_lane = pm_lane * 2;
            pm_internal_lane_1 = pm_internal_lane + 1;
            framer_pm = lanes[i * 2] / nof_lanes_per_group;
            framer_pm_lane = lanes[i * 2] % nof_lanes_per_group;
            framer_pm_1 = lanes[i * 2 + 1] / nof_lanes_per_group;
            framer_pm_lane_1 = lanes[i * 2 + 1] % nof_lanes_per_group;
            pmux_idx = pm_id * 3 + (pm_internal_lane / nof_lanes_per_group) * 2;
            SHR_IF_ERR_EXIT(imb_otnu_phy_port_rx_lane_map_set(unit, framer_pm, framer_pm_lane, pmux_idx));
            pmux_idx = pm_id * 3 + (pm_internal_lane_1 / nof_lanes_per_group) * 2;
            SHR_IF_ERR_EXIT(imb_otnu_phy_port_rx_lane_map_set(unit, framer_pm_1, framer_pm_lane_1, pmux_idx));

            switch (pm_internal_lane)
            {
                case 0:
                    pmux_idx = lanes[i * 2] / nof_lanes_per_group;
                    break;
                case 2:
                case 4:
                case 6:
                    pmux_idx = (lanes[i * 2] - (pm_internal_lane % nof_lanes_per_group)) / 4 + 1;
                    break;
                case 8:
                case 10:
                case 12:
                case 14:
                    pmux_idx = (lanes[i * 2] - (pm_internal_lane % nof_lanes_per_group)) / 4;
                    break;
                default:
                    SHR_IF_ERR_EXIT(_SHR_E_PARAM);
            }
            SHR_IF_ERR_EXIT(imb_otnu_phy_port_tx_lane_map_set(unit, pm_id, pm_internal_lane, pmux_idx));
            switch (pm_internal_lane_1)
            {
                case 1:
                case 3:
                case 5:
                case 7:
                case 9:
                case 11:
                case 13:
                case 15:
                    pmux_idx = (lanes[i * 2 + 1] - (pm_internal_lane_1 % nof_lanes_per_group)) / nof_lanes_per_group;
                    break;
                default:
                    SHR_IF_ERR_EXIT(_SHR_E_PARAM);
            }
            SHR_IF_ERR_EXIT(imb_otnu_phy_port_tx_lane_map_set(unit, pm_id, pm_internal_lane_1, pmux_idx));

            SHR_IF_ERR_EXIT(imb_otnu_phy_port_lane_map_cfg
                            (unit, pm_id, pm_lane, IMB_COMMON_DIRECTION_RX | IMB_COMMON_DIRECTION_TX, 1,
                             IMB_OTNU_PHY_PORT_PAM4_BITMUX_MSB20_LSB20, IMB_OTNU_PHY_PORT_PAM4_LANE_ORDER_NORMAL));
        }
    }
    else
    {
        for (i = 0; i < nof_phys; i++)
        {
            pm_id = phy_lanes[i] / nof_lanes_per_group;
            pm_lane = phy_lanes[i] % nof_lanes_per_group;
            pm_internal_lane = pm_lane * 2;
            framer_pm = lanes[i] / nof_lanes_per_group;
            framer_pm_lane = lanes[i] % nof_lanes_per_group;
            pmux_idx = pm_id * 3 + 1;
            SHR_IF_ERR_EXIT(imb_otnu_phy_port_rx_lane_map_set(unit, framer_pm, framer_pm_lane, pmux_idx));
            switch (pm_internal_lane)
            {
                case 0:
                    pmux_idx = lanes[i] / nof_lanes_per_group;
                    break;
                case 2:
                case 4:
                case 6:
                    pmux_idx = (lanes[i] - pm_internal_lane / 2) / 4;
                    break;
                case 8:
                case 10:
                case 12:
                case 14:
                    pmux_idx = (lanes[i] - pm_internal_lane / 2) / 4 + 1;
                    break;
                default:
                    SHR_IF_ERR_EXIT(_SHR_E_PARAM);
            }
            SHR_IF_ERR_EXIT(imb_otnu_phy_port_tx_lane_map_set(unit, pm_id, pm_internal_lane, pmux_idx));

            SHR_IF_ERR_EXIT(imb_otnu_phy_port_lane_map_cfg
                            (unit, pm_id, pm_lane, IMB_COMMON_DIRECTION_RX | IMB_COMMON_DIRECTION_TX, 0,
                             IMB_OTNU_PHY_PORT_PAM4_BITMUX_EVEN_ODD, IMB_OTNU_PHY_PORT_PAM4_LANE_ORDER_NORMAL));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get ilkn logical lane order
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - Flags to indicate the tx or rx direction.
 * \param [in] nof_max_lanes - max lane order size
 * \param [out] lanes - lane_order
 * \param [out] nof_actual_lanes - lane_order array size
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */

int
imb_otnu_phy_port_lane_map_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int nof_max_lanes,
    int *lanes,
    int *nof_actual_lanes)
{
    dnx_algo_lane_map_type_e type;
    soc_dnxc_lane_map_db_map_t lane2serdes;
    int phy_lanes[DNX_DATA_MAX_NIF_OTN_NOF_LANES] = { 0 };
    int i, nof_phys;
    bcm_pbmp_t port_lanes_bmp;
    portmod_speed_config_t port_speed_config;
    int pm_id, pm_lane, pm_internal_lane, pm_internal_lane_1;
    int framer_pm, framer_pm_lane, framer_pm_lane_1;
    int pmux_idx, pmux_idx_get, pmux_idx_1, pmux_idx_get_1;
    int tx_lane, rx_lane, tx_lane_1, rx_lane_1;
    int nof_lanes_per_group, nof_groups;

    SHR_FUNC_INIT_VARS(unit);

    *nof_actual_lanes = 0;
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &port_lanes_bmp));
    nof_phys = 0;
    BCM_PBMP_ITER(port_lanes_bmp, i)
    {
        type = DNX_ALGO_LANE_MAP_NIF_SIDE;
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_lane_id_to_serdes_map_get(unit, type, i, &lane2serdes));
        phy_lanes[nof_phys++] = lane2serdes.rx_id;
    }
    SHR_IF_ERR_EXIT(portmod_speed_config_t_init(unit, &port_speed_config));
    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &port_speed_config));

    nof_groups = dnx_data_nif.otn.nof_groups_get(unit);
    nof_lanes_per_group = dnx_data_nif.otn.nof_lanes_per_group_get(unit);
    if (DNXC_PORT_PHY_SPEED_IS_PAM4(port_speed_config.speed))
    {
        for (i = 0; i < nof_phys; i++)
        {
            pm_id = phy_lanes[i] / nof_lanes_per_group;
            pm_lane = phy_lanes[i] % nof_lanes_per_group;
            pm_internal_lane = pm_lane * 2;
            pm_internal_lane_1 = pm_internal_lane + 1;

            framer_pm_lane = pm_internal_lane % nof_lanes_per_group;
            framer_pm_lane_1 = pm_internal_lane_1 % nof_lanes_per_group;

            pmux_idx = pm_id * 3 + (pm_internal_lane / nof_lanes_per_group) * 2;
            pmux_idx_1 = pm_id * 3 + (pm_internal_lane_1 / nof_lanes_per_group) * 2;
            for (framer_pm = 0; framer_pm < nof_groups; framer_pm++)
            {
                SHR_IF_ERR_EXIT(imb_otnu_phy_port_rx_lane_map_get(unit, framer_pm, framer_pm_lane, &pmux_idx_get));
                if (pmux_idx_get == pmux_idx)
                {
                    break;
                }
            }
            if (framer_pm == nof_groups)
            {
                SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
            }
            rx_lane = framer_pm * nof_lanes_per_group + framer_pm_lane;
            for (framer_pm = 0; framer_pm < nof_groups; framer_pm++)
            {
                SHR_IF_ERR_EXIT(imb_otnu_phy_port_rx_lane_map_get(unit, framer_pm, framer_pm_lane_1, &pmux_idx_get_1));
                if (pmux_idx_get_1 == pmux_idx_1)
                {
                    break;
                }
            }
            if (framer_pm == nof_groups)
            {
                SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
            }
            rx_lane_1 = framer_pm * nof_lanes_per_group + framer_pm_lane_1;

            SHR_IF_ERR_EXIT(imb_otnu_phy_port_tx_lane_map_get(unit, pm_id, pm_internal_lane, &pmux_idx_get));
            switch (pm_internal_lane)
            {
                case 0:
                    tx_lane = pmux_idx_get * nof_lanes_per_group;
                    break;
                case 2:
                case 4:
                case 6:
                    tx_lane = (pmux_idx_get - 1) * 4 + pm_internal_lane % nof_lanes_per_group;
                    break;
                case 8:
                case 10:
                case 12:
                case 14:
                    tx_lane = pmux_idx_get * 4 + pm_internal_lane % nof_lanes_per_group;
                    break;
                default:
                    SHR_IF_ERR_EXIT(_SHR_E_PARAM);
            }
            SHR_IF_ERR_EXIT(imb_otnu_phy_port_tx_lane_map_get(unit, pm_id, pm_internal_lane_1, &pmux_idx_get_1));
            switch (pm_internal_lane_1)
            {
                case 1:
                case 3:
                case 5:
                case 7:
                case 9:
                case 11:
                case 13:
                case 15:
                    tx_lane_1 = pmux_idx_get_1 * nof_lanes_per_group + (pm_internal_lane_1 % nof_lanes_per_group);
                    break;
                default:
                    SHR_IF_ERR_EXIT(_SHR_E_PARAM);
            }
            if (tx_lane != rx_lane && tx_lane_1 != rx_lane_1)
            {
                SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
            }
            lanes[i * 2] = rx_lane;
            lanes[i * 2 + 1] = rx_lane_1;
            *nof_actual_lanes += 2;
        }
    }
    else
    {
        for (i = 0; i < nof_phys; i++)
        {
            pm_id = phy_lanes[i] / nof_lanes_per_group;
            pm_lane = phy_lanes[i] % nof_lanes_per_group;
            pm_internal_lane = pm_lane * 2;
            framer_pm_lane = pm_lane;
            pmux_idx = pm_id * 3 + 1;
            for (framer_pm = 0; framer_pm < nof_groups; framer_pm++)
            {
                SHR_IF_ERR_EXIT(imb_otnu_phy_port_rx_lane_map_get(unit, framer_pm, framer_pm_lane, &pmux_idx_get));
                if (pmux_idx_get == pmux_idx)
                {
                    break;
                }
            }
            if (framer_pm == nof_groups)
            {
                SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
            }
            rx_lane = framer_pm * nof_lanes_per_group + framer_pm_lane;
            SHR_IF_ERR_EXIT(imb_otnu_phy_port_tx_lane_map_get(unit, pm_id, pm_internal_lane, &pmux_idx_get));
            switch (pm_internal_lane)
            {
                case 0:
                    tx_lane = pmux_idx_get * nof_lanes_per_group;
                    break;
                case 2:
                case 4:
                case 6:
                    tx_lane = pmux_idx_get * 4 + pm_internal_lane / 2;
                    break;
                case 8:
                case 10:
                case 12:
                case 14:
                    tx_lane = (pmux_idx_get - 1) * 4 + pm_internal_lane / 2;
                    break;
                default:
                    SHR_IF_ERR_EXIT(_SHR_E_PARAM);
            }
            if (tx_lane != rx_lane)
            {
                SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
            }
            lanes[i] = rx_lane;
            *nof_actual_lanes += 1;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate the Speed, FEC, nof_lane some limitations for NIF OTN ports
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] fec_type - fec type
 * \param [in] speed - speed
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
int
imb_otnu_phy_port_fec_speed_validate(
    int unit,
    bcm_port_t port,
    bcm_port_phy_fec_t fec_type,
    int speed)
{
    int nof_lanes, is_valid;
    int idx, core;
    bcm_pbmp_t phys;
    int pm_imb_type;
    portmod_dispatch_type_t pm_type = portmodDispatchTypeCount;
    int first_phy, ethu_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
    BCM_PBMP_COUNT(phys, nof_lanes);
    _SHR_PBMP_FIRST(phys, first_phy);
    SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, first_phy, &ethu_id));
    pm_imb_type = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type;
    SHR_IF_ERR_EXIT(dnx_algo_port_imb_type_to_pm_type_get(unit, pm_imb_type, 0, (int *) &pm_type));

    is_valid = FALSE;
    /**
     * Go over all supported NIF interfaces for the device
     * and mark the port configuration as valid only if it passes all the checks
     */
    for (idx = 0; idx < dnx_data_nif.otn.supported_interfaces_info_get(unit)->key_size[0]; idx++)
    {
        if (dnx_data_nif.otn.supported_interfaces_get(unit, idx)->is_valid)
        {
            if (pm_type == dnx_data_nif.otn.supported_interfaces_get(unit, idx)->pm_dispatch_type)
            {
                if (speed == dnx_data_nif.otn.supported_interfaces_get(unit, idx)->speed)
                {
                    /**
                     * Fec type is protected by soc_dnxc_port_resource_validate when in set API
                     * The fec type validity skip can be applied only in port resource default request API
                     * where only the speed should be validate
                     */
                    if (fec_type == dnx_data_nif.otn.supported_interfaces_get(unit, idx)->fec_type
                        || fec_type == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
                    {
                        if (nof_lanes == dnx_data_nif.otn.supported_interfaces_get(unit, idx)->nof_lanes)
                        {
                            is_valid = TRUE;
                            break;
                        }
                    }
                }
            }
        }
    }
    if (!is_valid)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "port %d: The combination of speed %d and fec type %d and nof_lanes %d is not supported! \n ",
                     port, speed, fec_type, nof_lanes);
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure the FLR timer into HW
 */
static int
imb_otnu_phy_port_flr_link_timer_set(
    int unit,
    bcm_core_t core,
    int lane_in_core,
    uint32 timer)
{
    uint32 tick_count, tick_unit;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_flr_timer_to_tick_get(unit, timer, &tick_unit, &tick_count));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FLR_LINK_CONFIG, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CORE, lane_in_core);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TIMER_TICK_UNIT, INST_SINGLE, tick_unit);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TIMER_COUNT, INST_SINGLE, tick_count);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure the FLR timer from HW
 */
static int
imb_otnu_phy_port_flr_link_timer_get(
    int unit,
    bcm_core_t core,
    int lane_in_core,
    uint32 *timer)
{
    uint32 tick_count, tick_unit;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FLR_LINK_CONFIG, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CORE, lane_in_core);
    /*
     * Get field value
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TIMER_TICK_UNIT, INST_SINGLE, &tick_unit);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TIMER_COUNT, INST_SINGLE, &tick_count);
    /*
     * Get the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_algo_port_flr_tick_to_timer_get(unit, tick_unit, tick_count, timer));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure the FLR link mask into HW
 */
static int
imb_otnu_phy_port_flr_link_mask_set(
    int unit,
    bcm_core_t core,
    int lane_in_core,
    uint32 link_mask)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FLR_LINK_CONFIG, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CORE, lane_in_core);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MASK_LOS_DOWN, INST_SINGLE,
                                 (link_mask & BCM_PORT_FLR_LOS_DOWN_MASK) ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MASK_CDR_UNLOCK, INST_SINGLE,
                                 (link_mask & BCM_PORT_FLR_CDR_LOCK_DOWN_MASK) ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MASK_CDR_OOL, INST_SINGLE,
                                 (link_mask & BCM_PORT_FLR_CDR_OOL_DOWN_MASK) ? 1 : 0);
    /*
     * Always disable los up mask.
     * If los up is masked, the FLR state machine will be unable
     * to escape faulty status
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MASK_LOS_UP, INST_SINGLE, 0);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get the FLR link mask from HW
 */
static int
imb_otnu_phy_port_flr_link_mask_get(
    int unit,
    bcm_core_t core,
    int lane_in_core,
    uint32 *link_mask)
{
    uint32 mask_los, mask_cdr_lock, mask_ool;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FLR_LINK_CONFIG, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CORE, lane_in_core);
    /*
     * Get value fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MASK_LOS_DOWN, INST_SINGLE, &mask_los);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MASK_CDR_UNLOCK, INST_SINGLE, &mask_cdr_lock);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MASK_CDR_OOL, INST_SINGLE, &mask_ool);
    /*
     * Get the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (mask_los)
    {
        *link_mask |= BCM_PORT_FLR_LOS_DOWN_MASK;
    }
    if (mask_cdr_lock)
    {
        *link_mask |= BCM_PORT_FLR_CDR_LOCK_DOWN_MASK;
    }
    if (mask_ool)
    {
        *link_mask |= BCM_PORT_FLR_CDR_OOL_DOWN_MASK;
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the FLR link mask and
 *    link stable timer
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port ID
 * \param [in] config - FLR link mask and timer
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_otnu_phy_port_flr_link_config_set(
    int unit,
    bcm_port_t port,
    const bcm_port_flr_link_config_t * config)
{
    dnx_algo_port_ethu_access_info_t access_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &access_info));
    SHR_IF_ERR_EXIT(imb_otnu_phy_port_flr_link_mask_set
                    (unit, access_info.core, access_info.lane_in_core, config->link_mask));
    SHR_IF_ERR_EXIT(imb_otnu_phy_port_flr_link_timer_set
                    (unit, access_info.core, access_info.lane_in_core, config->link_stable_timer));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get the FLR link mask and
 *    link stable timer
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port ID
 * \param [in] config - FLR link mask and timer
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_otnu_phy_port_flr_link_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_flr_link_config_t * config)
{
    dnx_algo_port_ethu_access_info_t access_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &access_info));
    SHR_IF_ERR_EXIT(imb_otnu_phy_port_flr_link_mask_get
                    (unit, access_info.core, access_info.lane_in_core, &config->link_mask));
    SHR_IF_ERR_EXIT(imb_otnu_phy_port_flr_link_timer_get
                    (unit, access_info.core, access_info.lane_in_core, &config->link_stable_timer));
exit:
    SHR_FUNC_EXIT;
}
