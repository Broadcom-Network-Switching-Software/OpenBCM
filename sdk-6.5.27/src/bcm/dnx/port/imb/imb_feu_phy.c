/** \file imb_feu_phy.c
 *
 *  FlexE physical ports procedures for DNX.
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
#include <bcm_int/dnx/port/imb/imb_feu_phy.h>
#include <bcm_int/dnx/port/imb/imb_cdu.h>
#include <bcm_int/dnx/port/imb/imb_otnu_phy.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_calendar.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <bcm_int/dnx/algo/flexe_core/algo_flexe_core.h>

#include "imb_utils.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_led.h>
#include "imb_framer_internal.h"
#include "imb_ethu_internal.h"
#include "imb_cdu_internal.h"
#include <bcm_int/dnx/port/flexe/flexe_core.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/recovery/generic_state_journal.h>

#include "../nif/dnx_port_nif_arb_internal.h"

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

int imb_cdu_port_tx_power_down_set(
    int unit,
    bcm_port_t port,
    int power_down);

static int imb_feu_phy_port_transcode_set(
    int unit,
    bcm_port_t port,
    int is_rx,
    int speed,
    int is_am_transparent,
    int enable);

/**
 * \brief - Select the clock source for FEU.
 *
 * \param [in] unit - chip unit id
 * \param [in] clock_source - The clock source
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
imb_feu_phy_clock_source_set(
    int unit,
    int clock_source)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Disable the clock first
     */
    SHR_IF_ERR_EXIT(imb_framer_internal_clock_enable_set(unit, 0));
    /*
     * Select the clock
     */
    SHR_IF_ERR_EXIT(imb_framer_internal_clock_source_set(unit, clock_source));
    /*
     * Enable the clock
     */
    SHR_IF_ERR_EXIT(imb_framer_internal_clock_enable_set(unit, 1));
exit:
    SHR_FUNC_EXIT;
}

int
imb_feu_phy_init(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    int tx_delay, i, tx_threshold;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_clock_independent) && !sw_state_is_warm_boot(unit))
    {
        tx_threshold = dnx_data_nif.flexe.nb_tx_fifo_th_for_first_read_get(unit);
        SHR_IF_ERR_EXIT(imb_framer_internal_nb_tx_fifo_threshold_set(unit, tx_threshold));
        /*
         * Init TX delay for all PMs
         */
        for (i = 0; i < dnx_data_nif.framer.nof_pms_per_framer_get(unit); ++i)
        {
            tx_delay = dnx_data_nif.flexe.nb_tx_delay_table_get(unit, i)->val;
            SHR_IF_ERR_EXIT(imb_framer_internal_nb_tx_delay_per_pm_set(unit, i, tx_delay));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_feu_phy_deinit(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    uint8 flexe_core_is_active;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_active_get(unit, &flexe_core_is_active));
    /*
     * Deinit FlexE core
     */
    if (flexe_core_is_active && !dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_clock_independent))
    {
        if (!SAL_BOOT_PLISIM)
        {
            SHR_IF_ERR_EXIT(flexe_core_deinit(unit));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - FEU PHY post init after the TSC clock is ready
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_id - imb ID
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
imb_feu_phy_post_init(
    int unit,
    int imb_id)
{
    int tx_delay, clock_src, tx_threshold;
    uint8 is_active;
    int i, nof_clients, pm_id, pcs_port, pm_index;
    uint32 cal[DNX_DATA_MAX_NIF_MAC_CLIENT_NOF_CLIENTS];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_active_get(unit, &is_active));

    if (!is_active)
    {
        /*
         * Get flexewp into soft init (set to 1)
         */
        SHR_IF_ERR_EXIT(imb_framer_internal_flexe_blocks_soft_init_enable(unit, 1));
        /*
         * Release soft reset for FLEXEWP
         */
        SHR_IF_ERR_EXIT(imb_framer_internal_flexe_blocks_soft_reset(unit, IMB_COMMON_IN_RESET));
        SHR_IF_ERR_EXIT(imb_framer_internal_flexe_blocks_soft_reset(unit, IMB_COMMON_OUT_OF_RESET));
        /*
         * Release sbus reset for FLEXEWP
         */
        SHR_IF_ERR_EXIT(imb_framer_internal_flexe_blocks_sbus_reset(unit, IMB_COMMON_IN_RESET));
        SHR_IF_ERR_EXIT(imb_framer_internal_flexe_blocks_sbus_reset(unit, IMB_COMMON_OUT_OF_RESET));
        /*
         * enable flexe blocks
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_blocks_enable_set(unit, 1));
        /*
         * Get flexewp out of soft init (set to 0)
         */
        SHR_IF_ERR_EXIT(imb_framer_internal_flexe_blocks_soft_init_enable(unit, 0));
        /*
         * Select the clock source for FEU
         */
        clock_src = dnx_data_nif.flexe.flexe_core_clock_source_pm_get(unit);
        SHR_IF_ERR_EXIT(imb_feu_phy_clock_source_set(unit, clock_src));
        /*
         * Configure NB TX delay
         */
        tx_delay = dnx_data_nif.flexe.nb_tx_delay_get(unit);
        SHR_IF_ERR_EXIT(imb_framer_internal_nb_tx_delay_set(unit, tx_delay));
        /*
         * Enable tx credit mechanisms for Rx valid in case of local fault
         */
        if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_rx_valid_in_lf))
        {
            SHR_IF_ERR_EXIT(imb_framer_internal_tx_credit_for_rx_valid_enable_set(unit, 1));
        }
        /*
         * Configure NB TX FIFO threshold for first read.
         */
        tx_threshold = dnx_data_nif.flexe.nb_tx_fifo_th_for_first_read_get(unit);
        SHR_IF_ERR_EXIT(imb_framer_internal_nb_tx_fifo_threshold_set(unit, tx_threshold));
        /*
         * Set FlexE core out of reset
         */
        SHR_IF_ERR_EXIT(imb_framer_internal_flexe_core_reset(unit, IMB_COMMON_IN_RESET));
        SHR_IF_ERR_EXIT(imb_framer_internal_flexe_core_reset(unit, IMB_COMMON_OUT_OF_RESET));
        /*
         * Set Tiny MAC out of reset
         */
        SHR_IF_ERR_EXIT(imb_framer_internal_tiny_mac_reset(unit, IMB_COMMON_IN_RESET));
        SHR_IF_ERR_EXIT(imb_framer_internal_tiny_mac_reset(unit, IMB_COMMON_OUT_OF_RESET));
        /*
         * Init FlexE core
         */
        if (!SAL_BOOT_PLISIM)
        {
            SHR_IF_ERR_EXIT(flexe_core_init(unit));
        }

        SHR_IF_ERR_EXIT(imb_framer_internal_default_pm_id_and_pcs_port_get(unit, &pm_id, &pcs_port));
        for (i = 0; i < dnx_data_nif.flexe.nof_pcs_get(unit); ++i)
        {
            /** Reset all ports RX in FlexEWP NB */
            SHR_IF_ERR_EXIT(imb_framer_internal_nb_port_rx_reset_set(unit, i, 1));
            /** Config default PCS port to FlexE core port mapping */
            SHR_IF_ERR_EXIT(imb_framer_internal_flexe_core_to_pcs_port_map_set(unit, i, pm_id, pcs_port));
            /*
             * Reset all ports TX in FlexEWP NB to avoid Tx FIFO is stuck.
             * If the Rx valid can be kept during local fault, there is no
             * need to touch the Tx reset.
             */
            if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_toggle_tx_reset_in_nb))
            {
                for (pm_index = 0; pm_index < dnx_data_nif.framer.nof_pms_per_framer_get(unit); ++pm_index)
                {
                    SHR_IF_ERR_EXIT(imb_framer_internal_nb_port_tx_reset_set(unit, pm_index, i, IMB_COMMON_IN_RESET));
                }
            }
        }
        /*
         * Set SB RX calendar to default value
         */
        nof_clients = dnx_data_nif.mac_client.nof_clients_get(unit);
        for (i = 0; i < nof_clients; ++i)
        {
            cal[i] = DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;
        }
        SHR_IF_ERR_EXIT(imb_framer_internal_calendar_slots_config
                        (unit, IMB_FRAMER_SB_RX_CALENDAR, 0, nof_clients, cal));
        SHR_IF_ERR_EXIT(imb_framer_internal_calendar_slots_config
                        (unit, IMB_FRAMER_SB_RX_CALENDAR, 1, nof_clients, cal));
        /*
         * Enable flexe led which is inside flexe wrapper
         * that is relied on PM0 tsc clock.
         */
        if (dnx_data_led.general.feature_get(unit, dnx_data_led_general_flexe_in_scan_chain_is_adjusted))

        {
            SHR_IF_ERR_EXIT(imb_framer_internal_led_enable_set(unit, 1));
        }
        /*
         * Enable LF/RF/LI ENC for L1 switching
         */
        SHR_IF_ERR_EXIT(imb_framer_internal_lf_rf_li_encap_enable_set(unit, 1));
        /*
         * Mark FlexE core is active
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_active_set(unit, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

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
imb_feu_phy_port_pm_id_get(
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
imb_feu_phy_portmod_add_info_core_config(
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

    add_info->interface_config.interface = SOC_PORT_IF_NIF_ETH;

    if (!dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_clock_independent))
    {
        if (dnx_data_nif.flexe.flexe_core_clock_source_pm_get(unit) == pm_id)
        {
            PORTMOD_PORT_ADD_F_FLEXE_TVCO_26G_CONFIG_SET(add_info);
        }
        else
        {
            PORTMOD_PORT_ADD_F_FLEXE_TVCO_26G_CONFIG_CLR(add_info);
        }
        PORTMOD_PORT_ADD_F_FLEXE_CONFIG_SET(add_info);
    }
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
imb_feu_phy_portmod_add_info_config(
    int unit,
    bcm_port_t port,
    portmod_port_add_info_t * add_info)
{
    int pm_id;
    int ext_txpi_enable, is_flr_fw_support;

    SHR_FUNC_INIT_VARS(unit);

    /** Initialize core based configuruation */
    SHR_IF_ERR_EXIT(imb_feu_phy_port_pm_id_get(unit, port, &pm_id));
    SHR_IF_ERR_EXIT(imb_feu_phy_portmod_add_info_core_config(unit, pm_id, add_info));

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
    /** Get TXPI mode */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ext_txpi_enable_get(unit, port, &ext_txpi_enable));
    add_info->init_config.txpi_mode = ext_txpi_enable;
    if (ext_txpi_enable)
    {
        add_info->init_config.txpi_sdm_scheme = dnx_data_nif.phys.txpi_sdm_scheme_get(unit);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Rollback for portmod_port_add
 */
static shr_error_e
imb_feu_phy_port_portmod_port_add_er_rollback(
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
imb_feu_phy_port_portmod_port_remove_er_rollback(
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
    int flexe_clock_src_pm, is_flexe_clock_src_pm_initialized;

    SHR_FUNC_INIT_VARS(unit);

    port = (int *) metadata;

    SHR_IF_ERR_EXIT(imb_feu_phy_port_pm_id_get(unit, *port, &pm_id));

    /** Init PM0 core only when adding first flexe phy port on PM1 */
    flexe_clock_src_pm = dnx_data_nif.flexe.flexe_core_clock_source_pm_get(unit);
    SHR_IF_ERR_EXIT(portmod_pm_is_initialized(unit, flexe_clock_src_pm, &is_flexe_clock_src_pm_initialized));
    if (!is_flexe_clock_src_pm_initialized && (flexe_clock_src_pm != pm_id))
    {
        SHR_IF_ERR_EXIT(portmod_port_add_info_t_init(unit, &add_info));
        SHR_IF_ERR_EXIT(imb_feu_phy_portmod_add_info_core_config(unit, flexe_clock_src_pm, &add_info));
        PORTMOD_PORT_ADD_F_SKIP_SPEED_INIT_CLR(&add_info);
        add_info.tvco = portmodVCO26P562G;
        PORTMOD_CORE_ADD_FORCE_MAC_SET(core_add_flags);
        SHR_IF_ERR_EXIT(portmod_core_add(unit, flexe_clock_src_pm, core_add_flags, &add_info));
    }

    /** Configure Portmod */
    SHR_IF_ERR_EXIT(portmod_port_add_info_t_init(unit, &add_info));
    SHR_IF_ERR_EXIT(imb_feu_phy_portmod_add_info_config(unit, *port, &add_info));
    PORTMOD_CORE_ADD_FORCE_MAC_SET(core_add_flags);
    SHR_IF_ERR_EXIT(portmod_core_add(unit, pm_id, core_add_flags, &add_info));
    SHR_IF_ERR_EXIT(portmod_port_add(unit, *port, &add_info));
    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, *port, 0, 0));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure the links status mux for HW
 *    Linkscan. The link mux is from regular link
 *    status from PM.
 */
static int
imb_feu_phy_port_link_status_mux_set(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_ethu_access_info_t ethu_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * Configure link status MUX
     */
    SHR_IF_ERR_EXIT(imb_cdu_hw_linkscan_link_mux_set(unit, ethu_info.core, ethu_info.ethu_id_in_core,
                                                     ethu_info.first_lane_in_port, IMB_PORT_LINK_STATUS_MUX_FLEXE));
exit:
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
imb_feu_phy_port_attach(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    int pm_id, client_channel;
    portmod_port_add_info_t add_info;
    int flexe_clock_src_pm, is_flexe_clock_src_pm_initialized;
    int core_add_flags = 0;
    int cdu_id, core, nof_cdus_per_core, ethu_index;
    int phy_map_granularity;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_phy_port_pm_id_get(unit, port, &pm_id));

    if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_clock_power_down) &&
        dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_tx_separate_clock_power_down))
    {
        /** Power up TX */
        SHR_IF_ERR_EXIT(imb_cdu_port_tx_power_down_set(unit, port, FALSE));
    }
    if (!dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_clock_independent))
    {
        /** Init PM0 core only when adding first flexe phy port on PM1 */
        flexe_clock_src_pm = dnx_data_nif.flexe.flexe_core_clock_source_pm_get(unit);
        SHR_IF_ERR_EXIT(portmod_pm_is_initialized(unit, flexe_clock_src_pm, &is_flexe_clock_src_pm_initialized));
        if (!is_flexe_clock_src_pm_initialized && (flexe_clock_src_pm != pm_id))
        {
            if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_clock_power_down) &&
                dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_tx_separate_clock_power_down))
            {
                phy_map_granularity = dnx_data_nif.eth.phy_map_granularity_get(unit);
                ethu_index = dnx_data_nif.eth.phy_map_get(unit, dnx_data_nif.eth.nof_lanes_in_cdu_get(unit) *
                                                          flexe_clock_src_pm / phy_map_granularity)->ethu_index;
                nof_cdus_per_core = dnx_data_nif.eth.cdu_nof_per_core_get(unit);
                core = ethu_index >= nof_cdus_per_core ? 1 : 0;
                cdu_id = ethu_index % nof_cdus_per_core;

                /** Power up TX */
                SHR_IF_ERR_EXIT(imb_cdu_tx_power_down_set(unit, core, cdu_id, FALSE));
            }
            SHR_IF_ERR_EXIT(portmod_port_add_info_t_init(unit, &add_info));
            SHR_IF_ERR_EXIT(imb_feu_phy_portmod_add_info_core_config(unit, flexe_clock_src_pm, &add_info));
            PORTMOD_PORT_ADD_F_SKIP_SPEED_INIT_CLR(&add_info);
            add_info.tvco = portmodVCO26P562G;
            PORTMOD_CORE_ADD_FORCE_MAC_SET(core_add_flags);

            DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(portmod_core_add
                                                       (unit, flexe_clock_src_pm, core_add_flags, &add_info));
        }
    }

    /** Configure Portmod */
    SHR_IF_ERR_EXIT(portmod_port_add_info_t_init(unit, &add_info));
    SHR_IF_ERR_EXIT(imb_feu_phy_portmod_add_info_config(unit, port, &add_info));
    PORTMOD_CORE_ADD_FORCE_MAC_SET(core_add_flags);

    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(portmod_core_add(unit, pm_id, core_add_flags, &add_info));
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(portmod_port_add(unit, port, &add_info));

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        &imb_feu_phy_port_portmod_port_add_er_rollback, FALSE));

    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, 0));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, 1))
    {
        /*
         * Drop packet in arbiter for L1 ETH port
         */
        SHR_IF_ERR_EXIT(dnx_port_arb_disable_datapath_set(unit, port, TRUE));
        /*
         * Bypass OAM for L1 ETH
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));
        DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(flexe_core_oam_bypass_enable_set
                                                   (unit, client_channel, BCM_PORT_FLEXE_TX, 1));
    }
    /*
     * Configure Link status MUX for HW linkscan
     */
    if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_framer_phy_hw_linkscan_supported))
    {
        SHR_IF_ERR_EXIT(imb_feu_phy_port_link_status_mux_set(unit, port));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the PCS port ID in the PM
 *
 * \param [in] unit - chip unit id
 * \param [in] port - the logical port ID
 * \param [out] pcs_port - PCS port ID
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
imb_feu_phy_pcs_port_get(
    int unit,
    bcm_port_t port,
    int *pcs_port)
{
    int pm_id, phy;
    bcm_pbmp_t port_phys, pm_phys;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * get PM ID
     */
    SHR_IF_ERR_EXIT(imb_feu_phy_port_pm_id_get(unit, port, &pm_id));
    /*
     * get PM phys
     */
    pm_phys = dnx_data_nif.eth.pm_properties_get(unit, pm_id)->phys;
    /*
     * get all nif logical ports for our specific device core (0 or 1)
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &port_phys));

    *pcs_port = 0;
    _SHR_PBMP_ITER(pm_phys, phy)
    {
        if (_SHR_PBMP_MEMBER(port_phys, phy))
        {
            break;
        }
        ++(*pcs_port);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable or disable FlexE core port to PCS port
 *    mapping
 *
 * \param [in] unit - chip unit id
 * \param [in] port - the logical port to be removed
 * \param [in] enable - Enable or disable the mapping
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
imb_feu_phy_flexe_core_to_pcs_port_map_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    int pm_id, flexe_core_port, pcs_port;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the relevant info
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, port, &flexe_core_port));
    if (flexe_core_port == DNX_ALGO_PORT_INVALID)
    {
        SHR_EXIT();
    }

    if (enable)
    {
        SHR_IF_ERR_EXIT(imb_feu_phy_pcs_port_get(unit, port, &pcs_port));
        SHR_IF_ERR_EXIT(imb_feu_phy_port_pm_id_get(unit, port, &pm_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_framer_internal_default_pm_id_and_pcs_port_get(unit, &pm_id, &pcs_port));
    }
    /*
     * Configure HW
     */
    SHR_IF_ERR_EXIT(imb_framer_internal_flexe_core_port_enable_set(unit, flexe_core_port, enable));
    SHR_IF_ERR_EXIT(imb_framer_internal_flexe_core_to_pcs_port_map_set(unit, flexe_core_port, pm_id, pcs_port));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Map PCS port to FlexE core port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - the logical port
 * \param [in] enable - enable or disable
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
imb_feu_phy_pcs_to_flexe_core_port_map_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    int pm_id, flexe_core_port = 0, pcs_port;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the relevant info
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, port, &flexe_core_port));
    if (flexe_core_port == DNX_ALGO_PORT_INVALID)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(imb_feu_phy_pcs_port_get(unit, port, &pcs_port));
    SHR_IF_ERR_EXIT(imb_feu_phy_port_pm_id_get(unit, port, &pm_id));
    /*
     * Configure HW
     */
    SHR_IF_ERR_EXIT(imb_framer_internal_pcs_to_flexe_core_port_map_set
                    (unit, pm_id, pcs_port, (enable ? flexe_core_port : 0)));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Allocate memory for FlexE physical port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - the logical port
 * \param [in] speed - port speed
 * \param [in] enable - enable or disable
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
imb_feu_phy_pcs_port_fifo_set(
    int unit,
    bcm_port_t port,
    int speed,
    int enable)
{
    int pm_id, fifo_size = 0, pcs_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_phy_pcs_port_get(unit, port, &pcs_port));
    SHR_IF_ERR_EXIT(imb_feu_phy_port_pm_id_get(unit, port, &pm_id));
    if (enable)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_pcs_port_fifo_size_get(unit, speed, &fifo_size));
    }
    /*
     * Configure HW
     */
    SHR_IF_ERR_EXIT(imb_framer_internal_pcs_port_fifo_size_set(unit, pm_id, pcs_port, fifo_size));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Enable or disable PCS port in Wrapper
 *
 * \param [in] unit - chip unit id
 * \param [in] port - the logical port to be removed
 * \param [in] enable - Enable or disable
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
imb_feu_phy_pcs_port_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    int pm_id, pcs_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_phy_pcs_port_get(unit, port, &pcs_port));
    SHR_IF_ERR_EXIT(imb_feu_phy_port_pm_id_get(unit, port, &pm_id));
    /*
     * Configure HW
     */
    SHR_IF_ERR_EXIT(imb_framer_internal_pcs_port_enable_set(unit, pm_id, pcs_port, enable));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure msbus format for L1 ETH and flexe
 *
 * \param [in] unit - chip unit id
 * \param [in] port - the logical port to be removed
 * \param [in] enable - Enable or disable msbus mode
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
imb_feu_phy_pcs_msbus_mode_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    int pm_id, pcs_port;
    int l1_eth_en, flexe_en;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_phy_pcs_port_get(unit, port, &pcs_port));
    SHR_IF_ERR_EXIT(imb_feu_phy_port_pm_id_get(unit, port, &pm_id));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (enable)
    {
        l1_eth_en = DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE) ? 0 : 1;
        flexe_en = DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE) ? 1 : 0;
    }
    else
    {
        l1_eth_en = 0;
        flexe_en = 0;
    }
    /*
     * Configure HW
     */
    SHR_IF_ERR_EXIT(imb_framer_internal_pcs_msbus_mode_enable_set(unit, pm_id, pcs_port, l1_eth_en, flexe_en));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure FlexE Northbound wrapper
 *
 * \param [in] unit - chip unit id
 * \param [in] port - Logical port ID
 * \param [in] flags - IMB flexe flags
 * \param [in] speed - Logical port speed
 * \param [in] enable - Enable or disable
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_feu_phy_nb_wrapper_config_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int speed,
    int enable)
{
    int flexe_core_port, is_am_transparent;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get flexe core port from DB.
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, port, &flexe_core_port));
    /*
     * Reset FlexE core port in Wrapper
     */
    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_nb_rx_port_reset_support))
    {
        SHR_IF_ERR_EXIT(imb_framer_internal_nb_port_rx_reset_set(unit, flexe_core_port, IMB_COMMON_IN_RESET));
    }
    /*
     * Configure FlexE core port to PCS port mapping in Wrapper
     */
    SHR_IF_ERR_EXIT(imb_feu_phy_flexe_core_to_pcs_port_map_set(unit, port, enable));
    /*
     * Configure PCS port to FlexE core port mapping in Wrapper
     */
    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_pcs_to_flexe_port_map_support))
    {
        SHR_IF_ERR_EXIT(imb_feu_phy_pcs_to_flexe_core_port_map_set(unit, port, enable));
    }
    /*
     * Configure FIFO size
     */
    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_nb_dynamic_async_fifo))
    {
        SHR_IF_ERR_EXIT(imb_feu_phy_pcs_port_fifo_set(unit, port, speed, enable));
    }
    /*
     * Configure default transcode for both Rx and Tx
     */
    if (dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_l1_eth_supported))
    {
        is_am_transparent = IMB_FLEXE_AM_TRANSPARENT_GET(flags) ? 1 : 0;
        SHR_IF_ERR_EXIT(imb_feu_phy_port_transcode_set(unit, port, 0, speed, is_am_transparent, enable));
        SHR_IF_ERR_EXIT(imb_feu_phy_port_transcode_set(unit, port, 1, speed, is_am_transparent, enable));
    }
    /*
     * Enable PCS port in Wrapper
     */
    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_pcs_port_enable_support))
    {
        SHR_IF_ERR_EXIT(imb_feu_phy_pcs_port_enable_set(unit, port, enable));
    }
    if (enable && dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_nb_rx_port_reset_support))
    {
        /*
         * Take FlexE core port out of reset in Wrapper
         */
        SHR_IF_ERR_EXIT(imb_framer_internal_nb_port_rx_reset_set(unit, flexe_core_port, IMB_COMMON_OUT_OF_RESET));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure Framer PHY busa mapping for L1_ETH/FLEXE_PHY/MGMT
 *
 */
static int
imb_feu_phy_busa_mapping_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable)
{
    int flexe_core_port, instance_id;
    dnx_algo_port_info_s port_info;
    flexe_core_busa_channel_map_info_t busa_channel_map;
    bcm_port_phy_fec_t fec_type;

    SHR_FUNC_INIT_VARS(unit);

    flexe_core_busa_channel_map_info_t_init(unit, &busa_channel_map);
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, port, &flexe_core_port));
    busa_channel_map.flexe_core_port = flexe_core_port;

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, FALSE))
    {
        /*
         * For FlexE phy, configure FLEXE PHY <-> instance mapping
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_to_instance_id_get(unit, flexe_core_port, &instance_id));
        busa_channel_map.local_port = instance_id;
        busa_channel_map.is_flexe_phy = 1;
    }
    else
    {
        /*
         * For L1_ETH/MGMT_INTF/UNAWARE, configure FRAMER_PHY <-> CHANNEL mapping
         */
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, TRUE)
            || DNX_ALGO_PORT_TYPE_IS_FLEXE_UNAWARE(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &busa_channel_map.local_port));
            busa_channel_map.is_eth = 1;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_framer_mgmt_channel_get(unit, port, &busa_channel_map.local_port));
            busa_channel_map.is_mgmt_intf = 1;
        }
    }
    /*
     * Get FEC type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_fec_type_get(unit, port, &fec_type));
    busa_channel_map.is_fec = (fec_type != bcmPortPhyFecNone);
    /*
     * Check if AM is transparent
     */
    if (enable)
    {
        busa_channel_map.is_transparent = IMB_FLEXE_AM_TRANSPARENT_GET(flags) ? 1 : 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_am_transparent_get
                        (unit, flexe_core_port, &busa_channel_map.is_transparent));
    }
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(flexe_core_busa_channel_map_set
                                               (unit, BCM_PORT_FLEXE_RX | BCM_PORT_FLEXE_TX, &busa_channel_map,
                                                enable));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Remove a port from the FEU_PHY. first calls Portmos API
 *        to remove from portmod DB, then configures all FEU_PHY
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
imb_feu_phy_port_detach(
    int unit,
    bcm_port_t port)
{
    int speed, flexe_core_port, has_speed;
    int enable, pm_id, instance_id, channel;
    int is_flr_fw_support = 0;
    uint32 flags;
    dnx_algo_port_info_s port_info;
    flexe_core_rate_adpt_info_t rate_adpt_info;
    bcm_port_flr_link_config_t link_config;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify if the port is enabled
     */
    SHR_IF_ERR_EXIT(imb_feu_phy_port_enable_get(unit, port, &enable));
    if (enable)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Cannot detach active port %d", port);
    }
    /*
     * Get PM ID before removing the port in portmod
     */
    SHR_IF_ERR_EXIT(portmod_port_pm_id_get(unit, port, &pm_id));
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(portmod_port_remove(unit, port));

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        &imb_feu_phy_port_portmod_port_remove_er_rollback, FALSE));

    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, 0, &has_speed));
    if (has_speed)
    {
        /*
         * Disable Msbus mode for PCS port
         */
        SHR_IF_ERR_EXIT(imb_feu_phy_pcs_msbus_mode_enable_set(unit, port, 0));
        /*
         * Disable wrapper NB
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, 0, &speed));
        SHR_IF_ERR_EXIT(imb_feu_phy_nb_wrapper_config_set(unit, port, 0, speed, 0));

        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        /*
         * Clear Framer PHY to Framer channel mapping
         */
        SHR_IF_ERR_EXIT(imb_feu_phy_busa_mapping_set(unit, port, 0, 0));

        if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, FALSE))
        {
            /*
             * Get flexe core port and instance id info
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, port, &flexe_core_port));
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_to_instance_id_get(unit, flexe_core_port, &instance_id));
            /*
             * Clear the default FlexE default configuration
             */
            DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(flexe_core_phy_default_config_set
                                                       (unit, flexe_core_port, instance_id, speed, 0));
        }
        /*
         * Clear rate adapter for L1 ETH and Unware
         */
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, 1)
            || DNX_ALGO_PORT_TYPE_IS_FLEXE_UNAWARE(unit, port_info))
        {
            /*
             * Remove 1588 configuration for L1 ETH
             */
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, 1))
            {
                flags = (BCM_PORT_FLEXE_RX | BCM_PORT_FLEXE_TX);
                SHR_IF_ERR_EXIT(imb_feu_phy_port_l1_eth_1588_enable_set(unit, port, flags, 0));
            }
            /*
             * Remove rate adapter configuration
             */
            flexe_core_rate_adpt_info_t_init(unit, &rate_adpt_info);
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &channel));
            SHR_IF_ERR_EXIT(flexe_core_rateadpt_get(unit, channel, &rate_adpt_info));
            DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(flexe_core_rateadpt_delete(unit, channel, &rate_adpt_info));
        }
        /*
         * Remove flexe core port in DB
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_remove(unit, port));
        /*
         * Build calendar
         */
        SHR_IF_ERR_EXIT(imb_feu_phy_calendar_set(unit, 0, 0));
        /*
         * Clear FLR resources
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_fw_support_get(unit, port, &is_flr_fw_support));
        if (is_flr_fw_support && dnx_data_nif.flr.feature_get(unit, dnx_data_nif_flr_is_advanced_flr_supported))
        {
            SHR_IF_ERR_EXIT(imb_otnu_phy_port_lane_map_set(unit, port, 0, NULL, 0));
            /*
             * Clear FLR link mask configuration
             */
            sal_memset(&link_config, 0, sizeof(bcm_port_flr_link_config_t));
            link_config.link_mask =
                (BCM_PORT_FLR_LOS_DOWN_MASK | BCM_PORT_FLR_CDR_OOL_DOWN_MASK | BCM_PORT_FLR_CDR_LOCK_DOWN_MASK);
            SHR_IF_ERR_EXIT(imb_otnu_phy_port_flr_link_config_set(unit, port, -1, &link_config));
        }
    }
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
imb_feu_phy_port_resource_default_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_port_resource_t * resource)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_port_resource_default_get(unit, port, flags, resource));
    /*
     * Specific resource for FEU PHY
     */
    if (resource->base_flexe_instance == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->base_flexe_instance = -1;
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - FlexE core port related configuration
 *
 * \param [in] unit - chip unit id
 * \param [in] port - Logical port ID
 * \param [in] flags - IMB flexe flags
 * \param [in] instance_id - FlexE instance ID
 * \param [in] speed - port speed
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
imb_feu_phy_flexe_core_port_config_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int instance_id,
    int speed)
{
    int instance_id_tmp = -1;
    int orig_speed = -1, flexe_core_port_tmp = -1;
    int orig_am_transparent, am_transparent;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    am_transparent = IMB_FLEXE_AM_TRANSPARENT_GET(flags) ? 1 : 0;
    /*
     * Get orignal flexe core port from DB.
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, port, &flexe_core_port_tmp));
    /*
     * Handle speed change scenario and flexe core port change scenario
     */
    if (flexe_core_port_tmp != DNX_ALGO_PORT_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_speed_get(unit, flexe_core_port_tmp, &orig_speed));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_am_transparent_get(unit, flexe_core_port_tmp, &orig_am_transparent));
        /*
         * Get instance ID
         */
        if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, FALSE))
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_to_instance_id_get(unit, flexe_core_port_tmp, &instance_id_tmp));
        }
        if ((orig_speed != speed) || (instance_id_tmp != instance_id) || (orig_am_transparent != am_transparent))
        {
            /*
             * Clear Framer PHY to Framer channel mapping
             */
            SHR_IF_ERR_EXIT(imb_feu_phy_busa_mapping_set(unit, port, flags, 0));
            /*
             * Clear FlexE Default instance configuration
             */
            if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, FALSE))
            {
                SHR_IF_ERR_EXIT(flexe_core_phy_default_config_set
                                (unit, flexe_core_port_tmp, instance_id_tmp, speed, 0));
            }
            /*
             * If there is speed change, need to free the resource
             */
            SHR_IF_ERR_EXIT(imb_feu_phy_nb_wrapper_config_set(unit, port, flags, orig_speed, 0));
            /*
             * Remove flexe core port in DB
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_remove(unit, port));
        }
    }
    /*
     * Configure FlexE core port configuration for the
     * following scenarios:
     *    1. Configure the port speed for the first time.
     *    2. There is speed change.
     *    3. There is flexe core port change
     * For all the scenarios, the flexe_core_port in SW DB is
     * initial value, -1.
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, port, &flexe_core_port_tmp));

    if (flexe_core_port_tmp == DNX_ALGO_PORT_INVALID)
    {
        /*
         * Add FlexE core port in DB
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_add(unit, port, speed, instance_id, am_transparent));
        /*
         * Configure NB wrapper, which should be configured prior to PCS configuration.
         */
        SHR_IF_ERR_EXIT(imb_feu_phy_nb_wrapper_config_set(unit, port, flags, speed, 1));
        /*
         * Enable Msbus mode for PCS port
         */
        SHR_IF_ERR_EXIT(imb_feu_phy_pcs_msbus_mode_enable_set(unit, port, 1));
        /*
         * Configure Framer PHY to Framer channel mapping
         */
        SHR_IF_ERR_EXIT(imb_feu_phy_busa_mapping_set(unit, port, flags, 1));
        /*
         * Configure FlexE Default instance configuration
         */
        if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, FALSE))
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, port, &flexe_core_port_tmp));
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_to_instance_id_get(unit, flexe_core_port_tmp, &instance_id_tmp));
            SHR_IF_ERR_EXIT(flexe_core_phy_default_config_set(unit, flexe_core_port_tmp, instance_id_tmp, speed, 1));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - control enable/ disable FlexE PHY port
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
imb_feu_phy_port_enable_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable)
{
    int pcs_port, pm_index;
    int is_flr_fw_support, is_advanced_flr_support;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_phy_pcs_port_get(unit, port, &pcs_port));
    SHR_IF_ERR_EXIT(imb_feu_phy_port_pm_id_get(unit, port, &pm_index));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_fw_support_get(unit, port, &is_flr_fw_support));
    is_advanced_flr_support = is_flr_fw_support
        && dnx_data_nif.flr.feature_get(unit, dnx_data_nif_flr_is_advanced_flr_supported);

    if (enable && dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_toggle_tx_reset_in_nb))
    {
        /*
         * NB Tx FIFO might be stuck if there is no Rx valid during Local fault,
         * therefore we need to reset the Tx FIFO before enabling SyncE or 200G WAR,
         * otherwise, the FlexE overhead cannot be synced.
         *
         * If Rx valid can be kept during local fault, the Tx FIFO should never be
         * stuck, so there is no need to reset the NB Tx FIFO.
         */
        SHR_IF_ERR_EXIT(imb_framer_internal_nb_port_tx_reset_set(unit, pm_index, pcs_port, IMB_COMMON_IN_RESET));
        SHR_IF_ERR_EXIT(imb_framer_internal_nb_port_tx_reset_set(unit, pm_index, pcs_port, IMB_COMMON_OUT_OF_RESET));
    }
    if (!enable && is_advanced_flr_support)
    {
        SHR_IF_ERR_EXIT(imb_otnu_phy_port_enable_set(unit, port, IMB_PORT_ENABLE_F_SKIP_PORTMOD, 0));
    }
    /*
     * Enable / Disable port in Portmod
     */
    SHR_IF_ERR_EXIT(imb_er_portmod_port_enable_set(unit, port, 0, enable));

    if (!enable && dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_toggle_tx_reset_in_nb))
    {
        SHR_IF_ERR_EXIT(imb_framer_internal_nb_port_tx_reset_set(unit, pm_index, pcs_port, IMB_COMMON_IN_RESET));
    }
    if (enable && is_advanced_flr_support)
    {
        SHR_IF_ERR_EXIT(imb_otnu_phy_port_enable_set(unit, port, IMB_PORT_ENABLE_F_SKIP_PORTMOD, 1));
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
imb_feu_phy_port_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the indication from Portmod
     */
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(portmod_port_enable_get(unit, port, 0, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Check if the TDM slots needs to be
 *    doubled
 *
 * \param [in] unit - chip unit id
 * \param [in] logical_port - logical port
 * \param [in] adjust_port - the port whose TDM slot to be adjusted
 * \param [in] flags - Indicate to use single or double slot
 * \param [out] is_required - If doubling slots is required
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
imb_feu_phy_calendar_double_slot_is_required_get(
    int unit,
    bcm_port_t logical_port,
    bcm_port_t adjust_port,
    uint32 flags,
    int *is_required)
{
    int flexe_core_port, nof_ori_slots;
    portmod_speed_config_t speed_config;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get FlexE phy port speed configuration
     */
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(portmod_port_speed_config_get(unit, logical_port, &speed_config));
    /*
     * Get FlexE core port
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, logical_port, &flexe_core_port));

    if ((speed_config.num_lane == 2) && (speed_config.speed == 50000)
        && (speed_config.fec != PORTMOD_PORT_PHY_FEC_NONE))
    {
        if (dnx_data_nif.flexe.nb_tdm_slot_allocation_mode_get(unit) == DNX_FLEXE_ALLOC_MODE_DYNAMIC)
        {
            if (IMB_FLEXE_DOUBLE_NB_TDM_SLOT_GET(flags) && (logical_port == adjust_port))
            {
                /*
                 * Double the slots for the specific port
                 */
                *is_required = 1;
            }
            else if (IMB_FLEXE_SINGLE_NB_TDM_SLOT_GET(flags) && (logical_port == adjust_port))
            {
                /*
                 * Single slot
                 */
                *is_required = 0;
            }
            else
            {
                /*
                 * No flag is set, get the original value
                 */
                SHR_IF_ERR_EXIT(imb_framer_internal_nb_cal_original_tdm_slots_nof_get
                                (unit, flexe_core_port, &nof_ori_slots));
                *is_required = (nof_ori_slots == 2) ? 1 : 0;
            }
        }
        else if (dnx_data_nif.flexe.nb_tdm_slot_allocation_mode_get(unit) == DNX_FLEXE_ALLOC_MODE_DOUBLE)
        {
            *is_required = 1;
        }
        else
        {
            /*
             * DNX_FLEXE_ALLOC_MODE_SINGLE
             */
            *is_required = 0;
        }
    }
    else
    {
        *is_required = 0;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the Sch calender for FlexE physical port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - port id
 * \param [in] flags - port resource flags
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
imb_feu_phy_calendar_set(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    uint32 *calendar = NULL, calendar_length;
    int cal_to_config = -1;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_is_shared_arch))
    {
        /*
         * Get calendar's length
         */
        SHR_IF_ERR_EXIT(dnx_port_calendar_length_get(unit, DNX_PORT_NIF_FLEXE_PHY_CALENDAR, &calendar_length));

        /*
         * Allocating arrays memory
         */
        SHR_ALLOC_SET_ZERO_ERR_EXIT(calendar, calendar_length * sizeof(uint32),
                                    "Calendar to set slots per port", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(dnx_port_calendar_build_only
                        (unit, DNX_PORT_NIF_FLEXE_PHY_CALENDAR, &calendar_length, calendar));
    }
    else
    {
        bcm_pbmp_t phy_ports, l1_eth_ports;
        bcm_port_t logical_port, flexe_core_port = 0;
        int nof_phy_ports = 0, speed, has_speed, need_to_double_slots = 0;
        uint32 slots_per_phy_port[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_CORE_PORTS] = { 0 };

        /*
         * Allocating arrays memory
         */
        SHR_ALLOC_SET_ZERO_ERR_EXIT(calendar, DNX_DATA_MAX_NIF_FLEXE_NB_CAL_NOF_SLOTS * sizeof(uint32),
                                    "Calendar to set slots per port", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        /*
         * Get all the FlexE physical ports
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_PHY,
                         DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT, &phy_ports));
        /*
         * Get L1 ETH ports
         */
        if (dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_l1_eth_supported))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                            (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH_L1,
                             DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT, &l1_eth_ports));
            BCM_PBMP_OR(phy_ports, l1_eth_ports);
        }

        BCM_PBMP_ITER(phy_ports, logical_port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, 0, &has_speed));
            if (!has_speed)
            {
                continue;
            }
            /*
             * Get FlexE phy port speed
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, 0, &speed));
            /*
             * Get FlexE core port
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, logical_port, &flexe_core_port));
            /*
             * Get FlexE phy port speed configuration
             */
            SHR_IF_ERR_EXIT(imb_feu_phy_calendar_double_slot_is_required_get
                            (unit, logical_port, port, flags, &need_to_double_slots));
            slots_per_phy_port[flexe_core_port] =
                (need_to_double_slots ? 2 : 1) * speed / dnx_data_nif.flexe.nb_cal_granularity_get(unit);
        }

        /*
         * Build uniform calendar for flexe core port
         */
        nof_phy_ports = dnx_data_nif.flexe.nof_flexe_core_ports_get(unit);
        calendar_length = dnx_data_nif.flexe.nb_cal_nof_slots_get(unit);
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_cal_build
                        (unit, slots_per_phy_port, nof_phy_ports, calendar_length, calendar));

    }

    SHR_IF_ERR_EXIT(imb_framer_internal_shadow_calendar_config
                    (unit, IMB_FRAMER_NB_RX_CALENDAR, calendar_length, calendar, &cal_to_config));
    SHR_IF_ERR_EXIT(imb_framer_internal_calendar_switch(unit, IMB_FRAMER_NB_RX_CALENDAR, cal_to_config));

    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_nb_tx_cal_support))
    {
        SHR_IF_ERR_EXIT(imb_framer_internal_shadow_calendar_config
                        (unit, IMB_FRAMER_NB_TX_CALENDAR, calendar_length, calendar, &cal_to_config));
        SHR_IF_ERR_EXIT(imb_framer_internal_calendar_switch(unit, IMB_FRAMER_NB_TX_CALENDAR, cal_to_config));
    }
exit:
    SHR_FREE(calendar);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get two-step PTP PHY timesync tx info for flex phy port.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [out] tx_info - timesync tx info
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
imb_feu_phy_port_timesync_tx_info_get(
    int unit,
    bcm_port_t port,
    bcm_port_timesync_tx_info_t * tx_info)
{
    int core_port;
    uint32 valid;
    dnx_algo_port_info_s port_info;
    portmod_fifo_status_t info;
    uint64 timestamp;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
    {
        SHR_IF_ERR_EXIT(portmod_port_timesync_tx_info_get(unit, port, &info));
        COMPILER_64_SET(tx_info->timestamp, info.timestamps_in_fifo_hi, info.timestamps_in_fifo);
        tx_info->sequence_id = info.sequence_id;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, port, &core_port));
        SHR_IF_ERR_EXIT(flexe_core_ptp_2step_tx_info_get(unit, core_port, &valid, &timestamp));
        if (valid)
        {
            COMPILER_64_COPY(tx_info->timestamp, timestamp);
        }
        else
        {
            SHR_IF_ERR_EXIT(_SHR_E_EMPTY);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Config MIB counter max size for L1 ETH.
 */
int
imb_feu_phy_port_cntmaxsize_set(
    int unit,
    bcm_port_t port,
    int val)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, TRUE))
    {
        SHR_IF_ERR_EXIT(portmod_port_cntmaxsize_set(unit, port, val));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Max size configuration is not supported!\n");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get MIB counter max size for L1 ETH.
 */
int
imb_feu_phy_port_cntmaxsize_get(
    int unit,
    bcm_port_t port,
    int *val)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, TRUE))
    {
        SHR_IF_ERR_EXIT(portmod_port_cntmaxsize_get(unit, port, val));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Max size configuration is not supported!\n");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure Transcode configuration for
 *    L1 ETH and FlexE phy.
 *    If map type is bcmPortControlOtnMapNone, it means the L1 ETH
 *    is map to FlexE.
 */
static int
imb_feu_phy_port_transcode_set(
    int unit,
    bcm_port_t port,
    int is_rx,
    int speed,
    int is_am_transparent,
    int enable)
{
    int src_is_cl82 = TRUE, dst_is_cl82 = TRUE;
    int pcs_port, pm_index;
    imb_framer_xcode_config_t xcode_cfg;
    dnx_algo_port_info_s port_info;
    bcm_port_phy_fec_t fec_type;

    SHR_FUNC_INIT_VARS(unit);

    if (speed <= 25000)
    {
        /*
         * The 10G and 25G port is cl49 encaped.
         * For GFP and IMP, it is cl82 encaped.
         */
        src_is_cl82 = is_rx ? FALSE : !is_am_transparent;
        dst_is_cl82 = is_rx ? !is_am_transparent : FALSE;
    }
    sal_memset(&xcode_cfg, 0, sizeof(imb_framer_xcode_config_t));

    if (enable)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        SHR_IF_ERR_EXIT(dnx_algo_port_fec_type_get(unit, port, &fec_type));
        /*
         * Transcode configure for L1 ETH to FlexE client or OTN map
         */
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, TRUE)
            || DNX_ALGO_PORT_TYPE_IS_FRAMER_MGMT(unit, port_info))
        {
            xcode_cfg.xcode_shift_en = (src_is_cl82 != dst_is_cl82);
            /*
             * Special handling for 50G/100G BMP/GMP
             */
            if ((speed == 50000 || speed == 100000) && is_am_transparent)
            {
                xcode_cfg.trans_am_en = 1;
            }
            else
            {
                xcode_cfg.xcode_en = 1;
                xcode_cfg.xcode_cl82_en = ((is_rx && dst_is_cl82) || (!is_rx && src_is_cl82));
            }
            /*
             * Special handling for 25G BMP Tx
             */
            if (!is_rx && (speed == 25000) && (fec_type != bcmPortPhyFecNone) && is_am_transparent)
            {
                xcode_cfg.am_comp_en = 1;
            }
        }
    }
    SHR_IF_ERR_EXIT(imb_feu_phy_pcs_port_get(unit, port, &pcs_port));
    SHR_IF_ERR_EXIT(imb_feu_phy_port_pm_id_get(unit, port, &pm_index));
    /*
     * Configure HW
     */
    SHR_IF_ERR_EXIT(imb_framer_internal_port_transcode_set(unit, pm_index, pcs_port, is_rx, &xcode_cfg));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure 1588 configurations for
 *    L1 ETH port
 */
int
imb_feu_phy_port_l1_eth_1588_enable_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable)
{
    flexe_core_l1_eth_1588_info_t eth_1588_info;
    int curr_enable, mac1_channel, mac2_channel, count;
    int client_channel;
    uint32 directions[] = { BCM_PORT_FLEXE_RX, BCM_PORT_FLEXE_TX };
    flexe_core_rate_adpt_info_t rate_adpt_info;

    SHR_FUNC_INIT_VARS(unit);

    for (count = 0; count < COUNTOF(directions); ++count)
    {
        if (!(flags & directions[count]))
        {
            continue;
        }
        /*
         * Check if the 1588 has been enabled for this direction.
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_l1_eth_1588_enable_get(unit, port, directions[count], &curr_enable));
        if (enable == curr_enable)
        {
            continue;
        }
        /*
         * Get original MAC channels from DB
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_l1_eth_1588_mac_channel_get(unit, port, &mac1_channel, &mac2_channel));
        if (enable)
        {
            /*
             * If no MAC channel is allocated
             */
            if (mac1_channel == -1)
            {
                SHR_IF_ERR_EXIT(dnx_algo_l1_eth_1588_mac_channel_alloc_and_set
                                (unit, port, &mac1_channel, &mac2_channel));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_l1_eth_1588_mac_channel_remove
                            (unit, port, directions[count], mac1_channel, mac2_channel));
        }
        /*
         * Configure Framer
         */
        eth_1588_info.mac1_channel = mac1_channel;
        eth_1588_info.mac2_channel = mac2_channel;
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &eth_1588_info.ptp_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, 0, &eth_1588_info.speed));
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &eth_1588_info.eth_channel));
        DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(flexe_core_l1_eth_1588_enable_set
                                                   (unit, directions[count], &eth_1588_info, enable));
        /*
         * Update rate adapter Bus Type
         */
        if (directions[count] == BCM_PORT_FLEXE_TX)
        {
            flexe_core_rate_adpt_info_t_init(unit, &rate_adpt_info);
            rate_adpt_info.bus_type = enable ? flexe_core_rateadpt_adj_shift_bypass : flexe_core_rateadpt_normal;
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, 0, &rate_adpt_info.speed));
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));
            SHR_IF_ERR_EXIT(flexe_core_rateadpt_add(unit, client_channel, &rate_adpt_info));
        }
        /*
         * Update DB
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_l1_eth_1588_enable_set(unit, port, directions[count], enable));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get 1588 configurations for
 *    L1 ETH port
 */
int
imb_feu_phy_port_l1_eth_1588_enable_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get status from DB
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_l1_eth_1588_enable_get(unit, port, flags, enable));
exit:
    SHR_FUNC_EXIT;
}
