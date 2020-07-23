/** \file imb_feu_phy.c
 *
 *  FlexE physical ports procedures for DNX.
 */
/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <sal/core/boot.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/port/imb/imb_feu_phy.h>
#include <bcm_int/dnx/port/imb/imb_cdu.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>

#include "imb_utils.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include "imb_feu_internal.h"
#include "imb_ethu_internal.h"
#include "imb_cdu_internal.h"
#include <bcm_int/dnx/port/flexe/flexe_core.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

int imb_cdu_port_tx_power_down_set(
    int unit,
    bcm_port_t port,
    int power_down);

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
    SHR_IF_ERR_EXIT(imb_feu_internal_clock_enable_set(unit, 0));
    /*
     * Select the clock
     */
    SHR_IF_ERR_EXIT(imb_feu_internal_clock_source_set(unit, clock_source));
    /*
     * Enable the clock
     */
    SHR_IF_ERR_EXIT(imb_feu_internal_clock_enable_set(unit, 1));
exit:
    SHR_FUNC_EXIT;
}

int
imb_feu_phy_init(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    SHR_FUNC_INIT_VARS(unit);
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
    if (flexe_core_is_active)
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
    uint32 cal[DNX_DATA_MAX_NIF_FLEXE_NOF_CLIENTS];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_active_get(unit, &is_active));

    if (!is_active)
    {
        /*
         * Get flexewp into soft init (set to 1)
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_flexe_blocks_soft_init_enable(unit, 1));
        /*
         * Release soft reset for FLEXEWP
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_flexe_blocks_soft_reset(unit, IMB_COMMON_IN_RESET));
        SHR_IF_ERR_EXIT(imb_feu_internal_flexe_blocks_soft_reset(unit, IMB_COMMON_OUT_OF_RESET));
        /*
         * Release sbus reset for FLEXEWP
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_flexe_blocks_sbus_reset(unit, IMB_COMMON_IN_RESET));
        SHR_IF_ERR_EXIT(imb_feu_internal_flexe_blocks_sbus_reset(unit, IMB_COMMON_OUT_OF_RESET));
        /*
         * enable flexe blocks
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_blocks_enable_set(unit, 1));
        /*
         * Get flexewp out of soft init (set to 0)
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_flexe_blocks_soft_init_enable(unit, 0));
        /*
         * Select the clock source for FEU
         */
        clock_src = dnx_data_nif.flexe.flexe_core_clock_source_pm_get(unit);
        SHR_IF_ERR_EXIT(imb_feu_phy_clock_source_set(unit, clock_src));
        /*
         * Configure NB TX delay
         */
        tx_delay = dnx_data_nif.flexe.nb_tx_delay_get(unit);
        SHR_IF_ERR_EXIT(imb_feu_internal_nb_tx_delay_set(unit, tx_delay));
        /*
         * Enable tx credit mechanisms for Rx valid in case of local fault
         */
        if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_rx_valid_in_lf))
        {
            SHR_IF_ERR_EXIT(imb_feu_internal_tx_credit_for_rx_valid_enable_set(unit, 1));
        }
        /*
         * Configure NB TX FIFO threshold for first read.
         */
        tx_threshold = dnx_data_nif.flexe.nb_tx_fifo_th_for_first_read_get(unit);
        SHR_IF_ERR_EXIT(imb_feu_internal_nb_tx_fifo_threshold_set(unit, tx_threshold));
        /*
         * Set FlexE core out of reset
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_flexe_core_reset(unit, IMB_COMMON_IN_RESET));
        SHR_IF_ERR_EXIT(imb_feu_internal_flexe_core_reset(unit, IMB_COMMON_OUT_OF_RESET));
        /*
         * Set Tiny MAC out of reset
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_tiny_mac_reset(unit, IMB_COMMON_IN_RESET));
        SHR_IF_ERR_EXIT(imb_feu_internal_tiny_mac_reset(unit, IMB_COMMON_OUT_OF_RESET));
        /*
         * Init FlexE core
         */
        if (!SAL_BOOT_PLISIM)
        {
            SHR_IF_ERR_EXIT(flexe_core_init(unit));
        }

        SHR_IF_ERR_EXIT(imb_feu_internal_default_pm_id_and_pcs_port_get(unit, &pm_id, &pcs_port));
        for (i = 0; i < dnx_data_nif.flexe.nof_pcs_get(unit); ++i)
        {
            /** Reset all ports RX in FlexEWP NB */
            SHR_IF_ERR_EXIT(imb_feu_internal_nb_port_rx_reset_set(unit, i, 1));
            /** Config default PCS port to FlexE core port mapping */
            SHR_IF_ERR_EXIT(imb_feu_internal_flexe_core_to_pcs_port_map_set(unit, i, pm_id, pcs_port));
            /*
             * Reset all ports TX in FlexEWP NB to avoid Tx FIFO is stuck.
             * If the Rx valid can be kept during local fault, there is no
             * need to touch the Tx reset.
             */
            if (!dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_rx_valid_in_lf))
            {
                for (pm_index = 0; pm_index < dnx_data_nif.flexe.nof_pms_per_feu_get(unit); ++pm_index)
                {
                    SHR_IF_ERR_EXIT(imb_feu_internal_nb_port_tx_reset_set(unit, pm_index, i, IMB_COMMON_IN_RESET));
                }
            }
        }
        /*
         * Set SB RX calendar to default value
         */
        nof_clients = dnx_data_nif.flexe.nof_clients_get(unit);
        for (i = 0; i < nof_clients; ++i)
        {
            cal[i] = DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;
        }
        SHR_IF_ERR_EXIT(imb_feu_internal_calendar_slots_config(unit, IMB_FEU_SB_RX_CALENDAR, 0, nof_clients, cal));
        SHR_IF_ERR_EXIT(imb_feu_internal_calendar_slots_config(unit, IMB_FEU_SB_RX_CALENDAR, 1, nof_clients, cal));
        /*
         * Enable flexe led which is inside flexe wrapper
         * that is relied on PM0 tsc clock.
         */
        if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_is_led_supported))
        {
            SHR_IF_ERR_EXIT(imb_feu_internal_led_enable_set(unit, 1));
        }
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

    if (dnx_data_nif.flexe.flexe_core_clock_source_pm_get(unit) == pm_id)
    {
        PORTMOD_PORT_ADD_F_FLEXE_TVCO_26G_CONFIG_SET(add_info);
    }
    else
    {
        PORTMOD_PORT_ADD_F_FLEXE_TVCO_26G_CONFIG_CLR(add_info);
    }
    PORTMOD_PORT_ADD_F_FLEXE_CONFIG_SET(add_info);

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
    SHR_FUNC_INIT_VARS(unit);

    /** Initialize core based configuruation */
    SHR_IF_ERR_EXIT(imb_feu_phy_port_pm_id_get(unit, port, &pm_id));
    SHR_IF_ERR_EXIT(imb_feu_phy_portmod_add_info_core_config(unit, pm_id, add_info));

    /** Initialize port based configuruation */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, (bcm_pbmp_t *) & (add_info->phys)));

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
    int pm_id;
    portmod_port_add_info_t add_info;
    int flexe_clock_src_pm, is_flexe_clock_src_pm_initialized;
    int core_add_flags = 0;
    int cdu_id, core, nof_cdus_per_core, ethu_index;
    int phy_map_granularity;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_phy_port_pm_id_get(unit, port, &pm_id));

    if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_clock_power_down) &&
        dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_tx_separate_clock_power_down))
    {
        /** Power up TX */
        SHR_IF_ERR_EXIT(imb_cdu_port_tx_power_down_set(unit, port, FALSE));
    }

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
        SHR_IF_ERR_EXIT(portmod_core_add(unit, flexe_clock_src_pm, core_add_flags, &add_info));
    }

    /** Configure Portmod */
    SHR_IF_ERR_EXIT(portmod_port_add_info_t_init(unit, &add_info));
    SHR_IF_ERR_EXIT(imb_feu_phy_portmod_add_info_config(unit, port, &add_info));
    PORTMOD_CORE_ADD_FORCE_MAC_SET(core_add_flags);
    SHR_IF_ERR_EXIT(portmod_core_add(unit, pm_id, core_add_flags, &add_info));
    SHR_IF_ERR_EXIT(portmod_port_add(unit, port, &add_info));
    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, 0));
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
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_core_port_get(unit, port, &flexe_core_port));
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
        SHR_IF_ERR_EXIT(imb_feu_internal_default_pm_id_and_pcs_port_get(unit, &pm_id, &pcs_port));
    }
    /*
     * Configure HW
     */
    SHR_IF_ERR_EXIT(imb_feu_internal_flexe_core_port_enable_set(unit, flexe_core_port, enable));
    SHR_IF_ERR_EXIT(imb_feu_internal_flexe_core_to_pcs_port_map_set(unit, flexe_core_port, pm_id, pcs_port));
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
    SHR_IF_ERR_EXIT(imb_feu_internal_pcs_port_enable_set(unit, pm_id, pcs_port, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure FlexE Northbound wrapper
 *
 * \param [in] unit - chip unit id
 * \param [in] port - Logical port ID
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
    int enable)
{
    int flexe_core_port;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get flexe core port from DB.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_core_port_get(unit, port, &flexe_core_port));
    /*
     * Reset FlexE core port in Wrapper
     */
    SHR_IF_ERR_EXIT(imb_feu_internal_nb_port_rx_reset_set(unit, flexe_core_port, IMB_COMMON_IN_RESET));
    /*
     * Configure FlexE core port to PCS port mapping in Wrapper
     */
    SHR_IF_ERR_EXIT(imb_feu_phy_flexe_core_to_pcs_port_map_set(unit, port, enable));
    /*
     * Enable PCS port in Wrapper
     */
    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_pcs_port_enable_support))
    {
        SHR_IF_ERR_EXIT(imb_feu_phy_pcs_port_enable_set(unit, port, enable));
    }
    if (enable)
    {
        /*
         * Take FlexE core port out of reset in Wrapper
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_nb_port_rx_reset_set(unit, flexe_core_port, IMB_COMMON_OUT_OF_RESET));
    }
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
    int speed, flexe_core_port;
    int enable, pm_id;
    dnx_algo_port_ethu_access_info_t ethu_info;

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
    SHR_IF_ERR_EXIT(portmod_port_remove(unit, port));
    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * Disable FlexE mode for PCS port
     */
    SHR_IF_ERR_EXIT(imb_feu_internal_flexe_mode_enable_set
                    (unit, ethu_info.ethu_id_in_core, ethu_info.first_lane_in_port, 0));
    /*
     * Disable wrapper NB
     */
    SHR_IF_ERR_EXIT(imb_feu_phy_nb_wrapper_config_set(unit, port, 0));
    /*
     * Free FlexE core port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, 0, &speed));
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_core_port_get(unit, port, &flexe_core_port));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_free(unit, speed, flexe_core_port));
    /*
     * Clear channel map in FlexE core
     */
    SHR_IF_ERR_EXIT(flexe_core_channel_map_set(unit, BCM_PORT_FLEXE_RX | BCM_PORT_FLEXE_TX, flexe_core_port, 0xF));
    /*
     * Build calendar
     */
    SHR_IF_ERR_EXIT(imb_feu_phy_calendar_set(unit, 0, 0));

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
 * \param [in] flexe_core_port - FlexE core port
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
    int flexe_core_port,
    int speed)
{
    int has_speed, is_with_id;
    int orig_speed = -1, flexe_core_port_tmp = -1;
    dnx_algo_port_ethu_access_info_t ethu_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, 0, &has_speed));

    if (has_speed)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, 0, &orig_speed));
        /*
         * Get orignal flexe core port from DB.
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_core_port_get(unit, port, &flexe_core_port_tmp));
        /*
         * Handle speed change scenario and flexe core port change scenario
         */
        if (((orig_speed != speed) || (flexe_core_port != flexe_core_port_tmp)) &&
            (flexe_core_port_tmp != DNX_ALGO_PORT_INVALID))
        {
            /*
             * If there is speed change, need to free the resource
             */
            SHR_IF_ERR_EXIT(imb_feu_phy_nb_wrapper_config_set(unit, port, 0));
            /*
             * Free FlexE core port
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_free(unit, orig_speed, flexe_core_port_tmp));
            /*
             * Clear the DB.
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_core_port_set(unit, port, DNX_ALGO_PORT_INVALID));
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
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_core_port_get(unit, port, &flexe_core_port_tmp));

    if (flexe_core_port_tmp == DNX_ALGO_PORT_INVALID)
    {
        /*
         * Allocate FlexE core port.
         */
        is_with_id = (flexe_core_port == DNX_ALGO_PORT_INVALID) ? 0 : 1;
        flexe_core_port_tmp = flexe_core_port;
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_alloc(unit, speed, is_with_id, &flexe_core_port_tmp));
        /*
         * Store the flexe core port in DB.
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_core_port_set(unit, port, flexe_core_port_tmp));
        /*
         * Configure NB wrapper, which should be configured prior to PCS configuration.
         */
        SHR_IF_ERR_EXIT(imb_feu_phy_nb_wrapper_config_set(unit, port, 1));
        /*
         * Enable FlexE mode for PCS port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
        SHR_IF_ERR_EXIT(imb_feu_internal_flexe_mode_enable_set
                        (unit, ethu_info.ethu_id_in_core, ethu_info.first_lane_in_port, 1));
        /*
         * Configure FlexE Default instatnce configuration
         */
        SHR_IF_ERR_EXIT(flexe_core_phy_default_config_set(unit, flexe_core_port_tmp, speed));
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

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_phy_pcs_port_get(unit, port, &pcs_port));
    SHR_IF_ERR_EXIT(imb_feu_phy_port_pm_id_get(unit, port, &pm_index));

    if (enable && !dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_rx_valid_in_lf))
    {
        /*
         * NB Tx FIFO might be stuck if there is no Rx valid during Local fault,
         * therefore we need to reset the Tx FIFO before enabling SyncE or 200G WAR,
         * otherwise, the FlexE overhead cannot be synced.
         *
         * If Rx valid can be kept during local fault, the Tx FIFO should never be
         * stuck, so there is no need to reset the NB Tx FIFO.
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_nb_port_tx_reset_set(unit, pm_index, pcs_port, IMB_COMMON_IN_RESET));
        SHR_IF_ERR_EXIT(imb_feu_internal_nb_port_tx_reset_set(unit, pm_index, pcs_port, IMB_COMMON_OUT_OF_RESET));
    }
    /*
     * Enable / Disable port in Portmod
     */
    SHR_IF_ERR_EXIT(imb_er_portmod_port_enable_set(unit, port, 0, enable));

    if (!enable && !dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_rx_valid_in_lf))
    {
        SHR_IF_ERR_EXIT(imb_feu_internal_nb_port_tx_reset_set(unit, pm_index, pcs_port, IMB_COMMON_IN_RESET));
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
    SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, port, 0, enable));

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
    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, logical_port, &speed_config));
    /*
     * Get FlexE core port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_core_port_get(unit, logical_port, &flexe_core_port));

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
                SHR_IF_ERR_EXIT(imb_feu_internal_nb_cal_original_tdm_slots_nof_get
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
    uint32 slots_per_phy_port[DNX_DATA_MAX_NIF_FLEXE_NOF_PCS] = { 0 };
    uint32 calendar[DNX_DATA_MAX_NIF_FLEXE_NOF_PCS] = { 0 };
    int nof_phy_ports = 0, speed, has_speed, need_to_double_slots = 0;
    bcm_port_t logical_port, flexe_core_port = 0;
    bcm_pbmp_t phy_ports;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get all the FlexE physical ports
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_PHY,
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT, &phy_ports));

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
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_core_port_get(unit, logical_port, &flexe_core_port));
        /*
         * Get FlexE phy port speed configuration
         */
        SHR_IF_ERR_EXIT(imb_feu_phy_calendar_double_slot_is_required_get
                        (unit, logical_port, port, flags, &need_to_double_slots));

        slots_per_phy_port[flexe_core_port] =
            (need_to_double_slots ? 2 : 1) * speed / dnx_data_nif.flexe.phy_speed_granularity_get(unit);
    }
    /*
     * Build uniform calendar for flexe core port
     */
    nof_phy_ports = dnx_data_nif.flexe.nof_pcs_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_cal_simple_fixed_len_cal_build
                    (unit, slots_per_phy_port, nof_phy_ports, nof_phy_ports, nof_phy_ports, calendar));

    SHR_IF_ERR_EXIT(imb_feu_internal_calendar_config(unit, IMB_FEU_NB_RX_CALENDAR, nof_phy_ports, calendar));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure alternative ports for FlexE PHY ports.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] alter_ports - alternative ports
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
imb_feu_phy_alternative_port_set(
    int unit,
    bcm_port_t port,
    const bcm_pbmp_t * alter_ports)
{
    int flexe_core_port;
    uint32 alter_flexe_core_ports[1] = { 0 };
    bcm_port_t alter_port;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get all the flexe core ports for alternative ports.
     */
    _SHR_PBMP_ITER(*alter_ports, alter_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_core_port_get(unit, alter_port, &flexe_core_port));
        SHR_BITSET(alter_flexe_core_ports, flexe_core_port);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_core_port_get(unit, port, &flexe_core_port));
    SHR_IF_ERR_EXIT(imb_feu_internal_alternative_port_set(unit, flexe_core_port, alter_flexe_core_ports[0]));
exit:
    SHR_FUNC_EXIT;
}
