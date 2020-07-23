
/** \file imb_cdu.c
 *
 *  Ethernet ports procedures for DNX.
 */
/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** allow memregs.h include excplictly */
#ifdef _SOC_MEMREGS_H
#undef _SOC_MEMREGS_H
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <soc/util.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <soc/cmic.h>
#include <soc/mcm/memregs.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/port/imb/imb_cdu.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <bcm_int/common/link.h>
#include <bcm_int/dnx/port/flexe/flexe_core.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/phy/phymod_sim.h>
#include <soc/dnx/pll/pll.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pll.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#include <soc/dnx/dbal/dbal.h>
#include <src/bcm/dnx/port/port_utils.h>
#include "imb_utils.h"
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <soc/dnxc/dnxc_port.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>
#include <soc/sand/sand_signals.h>
#include "../../../../../include/bcm_int/dnx/port/imb/imb_ethu.h"
#include "imb_ethu_internal.h"
#include "imb_cdu_internal.h"

#include <bcm_int/dnx/port/port_flexe.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/* Static function declarations */
static int imb_cdu_port_macro_config(
    int unit,
    int pm_in_cdu_index,
    const imb_create_info_t * imb_info,
    portmod_pm_create_info_t * pm_info);

static int imb_cdu_port_pm_id_get(
    int unit,
    bcm_port_t port,
    int *pm_id);

int imb_cdu_port_tx_power_down_set(
    int unit,
    bcm_port_t port,
    int power_down);

/**
 * \brief - call back function to portmod to reset port credit
 */
int
imb_cdu_portmod_soft_reset(
    int unit,
    soc_port_t port,
    portmod_call_back_action_type_t action)
{
    dnx_algo_port_ethu_access_info_t cdu_info;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &cdu_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, TRUE))
    {
        switch (action)
        {
            case portmodCallBackActionTypeDuring:
                SHR_IF_ERR_EXIT(imb_ethu_port_credit_tx_reset(unit, port));
                break;
            case portmodCallBackActionTypePre:
                if (dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_gear_shifter_exists))
                {
                    SHR_IF_ERR_EXIT(imb_cdu_rx_gear_shift_reset_set
                                    (unit, cdu_info.core, cdu_info.ethu_id_in_core, cdu_info.first_lane_in_port, 1));
                }
                break;
            case portmodCallBackActionTypePost:
                if (dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_gear_shifter_exists))
                {
                    SHR_IF_ERR_EXIT(imb_cdu_rx_gear_shift_reset_set
                                    (unit, cdu_info.core, cdu_info.ethu_id_in_core, cdu_info.first_lane_in_port, 0));
                }
                break;

            default:
                break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD threshold value for all priorities for all rmc's
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] cdu_id_in_core - cdu id
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
imb_cdu_prd_threshold_all_rmc_threshold_set(
    int unit,
    bcm_core_t core,
    int cdu_id_in_core)
{
    uint32 nof_priority_groups, priority_index;
    int nof_rmcs_per_port, rmc_index;

    SHR_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.prd.nof_priorities_get(unit);
    nof_rmcs_per_port = dnx_data_nif.eth.ethu_logical_fifo_nof_get(unit);
    for (rmc_index = 0; rmc_index < nof_rmcs_per_port; ++rmc_index)
    {
        for (priority_index = 0; priority_index < nof_priority_groups; priority_index++)
        {
            SHR_IF_ERR_EXIT(imb_cdu_prd_threshold_hw_set
                            (unit, core, cdu_id_in_core, rmc_index, priority_index,
                             dnx_data_nif.prd.cdu_rmc_threshold_max_get(unit)));
        }
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set PRD threshold value per priority per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority - priority for the threshold
 * \param [in] threshold - threshold value
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
imb_cdu_prd_threshold_set(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 threshold)
{
    dnx_algo_port_rmc_info_t rmc;
    uint32 rmc_fifo_size;
    dnx_algo_port_ethu_access_info_t cdu_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &cdu_info));

    SHR_IF_ERR_EXIT(dnx_cosq_prd_threshold_rmc_get(unit, port, priority, &rmc));

    rmc_fifo_size = rmc.last_entry - rmc.first_entry + 1;

    if (threshold > rmc_fifo_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Threshold %d for priority %d of port %d is higher than the fifo size %d\n",
                     threshold, priority, port, rmc_fifo_size);
    }

    SHR_IF_ERR_EXIT(imb_cdu_prd_threshold_hw_set
                    (unit, cdu_info.core, cdu_info.ethu_id_in_core, rmc.rmc_id, priority, threshold));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get Threshold value for the given priority
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority - priority to get threshold
 * \param [in] threshold - returned threshold
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
imb_cdu_prd_threshold_get(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 *threshold)
{
    dnx_algo_port_rmc_info_t rmc;
    dnx_algo_port_ethu_access_info_t cdu_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &cdu_info));

    SHR_IF_ERR_EXIT(dnx_cosq_prd_threshold_rmc_get(unit, port, priority, &rmc));

    SHR_IF_ERR_EXIT(imb_cdu_prd_threshold_hw_get
                    (unit, cdu_info.core, cdu_info.ethu_id_in_core, rmc.rmc_id, priority, threshold));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Initialize CDU:
 * 1. Initialize CDU Database.
 * 2. Set some Reg values to get to desired SW start point. 3.
 * 3. Initialize PM init info for PM below and call
 * port_macro_add()
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_info - IMB info required for
 *        Initialization.
 * \param [out] imb_specific_info - IMB specific info to be
 *        initialized.
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
imb_cdu_init(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    int pm_i, rmc, nof_rmcs, nof_cdus_per_core, cdu_id;
    bcm_core_t core;
    portmod_pm_create_info_t pm_info;
    portmod_default_user_access_t *user_acc;
    SHR_FUNC_INIT_VARS(unit);

    /**
     * 1. Initialize CDU DB
     */
    user_acc = sal_alloc(sizeof(portmod_default_user_access_t), "CDU user access");
    SHR_NULL_CHECK(user_acc, _SHR_E_MEMORY, "user_access");
    SHR_IF_ERR_EXIT(portmod_default_user_access_t_init(unit, user_acc));
    PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET(user_acc);
    user_acc->unit = unit;
    user_acc->blk_id = dnx_drv_cdport_block(unit, imb_info->inst_id);
    user_acc->mutex = sal_mutex_create("pm mutex");
    SHR_NULL_CHECK(user_acc->mutex, _SHR_E_MEMORY, "user_access->mutex");
    imb_specific_info->cdu.user_acc = user_acc;

    /**
     * 2. initialize register values
     */
    if (!sw_state_is_warm_boot(unit))
    {
        nof_rmcs = dnx_data_nif.eth.ethu_logical_fifo_nof_get(unit);
        nof_cdus_per_core = dnx_data_nif.eth.cdu_nof_per_core_get(unit);
        core = imb_info->inst_id >= nof_cdus_per_core ? 1 : 0;
        cdu_id = imb_info->inst_id % nof_cdus_per_core;
        /*
         * Disable data to Portmacro
         */
        SHR_IF_ERR_EXIT(imb_ethu_tx_data_disable_traffic(unit, core, cdu_id));

        /*
         * Take PM out of reset
         */
        SHR_IF_ERR_EXIT(imb_cdu_pm_reset(unit, core, cdu_id, IMB_COMMON_OUT_OF_RESET));

        if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_clock_power_down))
        {
            /** Power down the unit */
            SHR_IF_ERR_EXIT(imb_ethu_power_down_set(unit, core, cdu_id, TRUE));
        }

        if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_ilkn_cdu_fec_separate_clock_power_down))
        {
            /** Power down the FEC domain of the unit */
            SHR_IF_ERR_EXIT(imb_cdu_ilkn_logic_power_down_set(unit, core, cdu_id, TRUE));
        }
        if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_tx_separate_clock_power_down))
        {
            /** Power down TX */
            SHR_IF_ERR_EXIT(imb_cdu_tx_power_down_set(unit, core, cdu_id, TRUE));
        }
        /*
         * Clear all Schedulers
         */
        SHR_IF_ERR_EXIT(imb_ethu_scheduler_config_set(unit, core, cdu_id, bcmPortNifSchedulerLow, 0));
        SHR_IF_ERR_EXIT(imb_ethu_scheduler_config_set(unit, core, cdu_id, bcmPortNifSchedulerHigh, 0));
        if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
        {
            SHR_IF_ERR_EXIT(imb_ethu_scheduler_config_set(unit, core, cdu_id, bcmPortNifSchedulerTDM, 0));
        }

        if (dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_rmc_flush_support))
        {
            /*
             * Set Scheduler cnt decrement threshold
             */
            SHR_IF_ERR_EXIT(imb_ethu_sch_cnt_dec_thres_hw_set(unit, core, cdu_id, 0));
        }
        /*
         * Set all RMCs threshold after overflow to 0. set All RMCs Flush/stop bit
         */
        for (rmc = 0; rmc < nof_rmcs; ++rmc)
        {
            SHR_IF_ERR_EXIT(imb_ethu_port_rmc_thr_after_ovf_hw_set(unit, core, cdu_id, rmc, 0));
            if (dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_rmc_flush_support))
            {
                SHR_IF_ERR_EXIT(imb_ethu_port_rx_rmc_flush_mode_enable_hw_set(unit, core, cdu_id, rmc, 0));
            }
        }
        /*
         * Enable EEE for the CDU - this does not actually enable the EEE for the ports, but if this bit is not set,
         * EEE will not work
         */
        SHR_IF_ERR_EXIT(imb_cdu_eee_enable_set(unit, core, cdu_id, 1));

        SHR_IF_ERR_EXIT(imb_cdu_iddq_enable_set(unit, core, cdu_id, 0));

        /*
         * Set PRD bypass mode to 0.
         * This is a debug feature to bypass the PRD parser, and it is turned on by default.
         * it should not be exposed as API.
         * this will enable working with the PRD later on.
         */
        SHR_IF_ERR_EXIT(imb_ethu_prd_bypass_hw_set(unit, core, cdu_id, 0));

        /*
         * Disable masking of TDM indication from parser. CLU/CDU has a feature to mask TDM indication received from PP
         * parser. This mechanism was added when PP team was not sure they would complete the work required for TDM
         * indication. Since the work was eventually completed, it should be disabled. Set
         * RX_PORT_MAP_MASK_PARSER_TDM_BITf in CLU_RX_PORT_MAP_MASK_PARSER_TDM to '0'.
         */
        if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
        {
            SHR_IF_ERR_EXIT(imb_ethu_tdm_mask_hw_set(unit, core, cdu_id, 0));
        }
        if (dnx_data_nif.prd.feature_get(unit, dnx_data_nif_prd_is_rx_rmc_threshold_need_configuration))
        {
            SHR_IF_ERR_EXIT(imb_cdu_prd_threshold_all_rmc_threshold_set(unit, core, cdu_id));
        }
    }

    /**
     * 3. initialize portmod_pm_create_info_t
     */
    for (pm_i = 0; pm_i < dnx_data_nif.eth.nof_pms_in_cdu_get(unit); pm_i++)
    {
        SHR_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
        SHR_IF_ERR_EXIT(imb_cdu_port_macro_config(unit, pm_i, imb_info, &pm_info));

        /*
         * Initialize specific pm8x50 info - after pm8x50 is implemented
         */
        SHR_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));
    }

exit:

    if (SHR_FUNC_ERR())
    {
        if (user_acc != NULL)
        {
            if (user_acc->mutex != NULL)
            {
                sal_mutex_destroy(user_acc->mutex);
            }

            sal_free(user_acc);
            imb_specific_info->cdu.user_acc = NULL;
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - De-initialize the CDU. main task is to free
 *        dynamically allocated memory.
 *
 * \param [in] unit - chip unit id.
 * \param [in] imb_info - IMB info IMB info required for
 *        de-init.
 * \param [out] imb_specific_info - IMB specific info to be
 *        de-initialized.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * Should only be called during de-init process.
 * \see
 *   * None
 */
int
imb_cdu_deinit(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    portmod_default_user_access_t *user_acc;
    SHR_FUNC_INIT_VARS(unit);

    user_acc = imb_specific_info->cdu.user_acc;

    if (user_acc != NULL)
    {
        if (user_acc->mutex != NULL)
        {
            sal_mutex_destroy(user_acc->mutex);
        }

        sal_free(user_acc);
        imb_specific_info->cdu.user_acc = NULL;
    }

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
int
imb_cdu_portmod_add_info_config(
    int unit,
    bcm_port_t port,
    portmod_port_add_info_t * add_info)
{
    int i, nof_phys, pm_id;
    int pm_lower_bound, pm_upper_bound;
    soc_dnxc_lane_map_db_map_t lane2serdes[DNX_DATA_MAX_NIF_ETH_NOF_LANES_IN_CDU];

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_add_info_t_init(unit, add_info));

    /*
     * Configure lane map info. Get the lane map info from swstate
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_imb_ethu_port_pm_boundary_get(unit, port, &pm_lower_bound, &pm_upper_bound));
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_pm_lane_to_serdes_map_get
                    (unit, DNX_ALGO_LANE_MAP_NIF_SIDE, pm_lower_bound, pm_upper_bound, lane2serdes));

    for (i = 0; i < dnx_data_nif.eth.nof_lanes_in_cdu_get(unit); i++)
    {
        add_info->init_config.lane_map[0].lane_map_rx[i] = lane2serdes[i].rx_id;
        add_info->init_config.lane_map[0].lane_map_tx[i] = lane2serdes[i].tx_id;
    }
    add_info->init_config.lane_map[0].num_of_lanes = dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);
    add_info->init_config.lane_map_overwrite = 1; /** we always overwrite lane map*/
    add_info->init_config.polarity_overwrite = 0; /** same as in pm create, we don't need to overwrite*/
    add_info->init_config.fw_load_method_overwrite = 0; /** same as in pm create, we don't need to overwrite*/
    add_info->init_config.ref_clk_overwrite = 0; /** same as in pm create, we don't need to overwrite*/
    /** get port phys */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, (bcm_pbmp_t *) & (add_info->phys)));
    /** get Max speed */
    PORTMOD_PBMP_COUNT(add_info->phys, nof_phys);
    add_info->interface_config.max_speed = dnx_data_nif.eth.max_speed_get(unit, nof_phys)->speed;

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

    SHR_IF_ERR_EXIT(imb_cdu_port_pm_id_get(unit, port, &pm_id));

    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_is_supported))
    {
        if ((dnx_data_nif.flexe.flexe_core_clock_source_pm_get(unit) == pm_id)
            && (dnx_data_nif.flexe.flexe_mode_get(unit) != 0))
        {
            PORTMOD_PORT_ADD_F_FLEXE_TVCO_26G_CONFIG_SET(add_info);
        }
        else
        {
            PORTMOD_PORT_ADD_F_FLEXE_TVCO_26G_CONFIG_CLR(add_info);
        }
    }

    add_info->interface_config.interface = SOC_PORT_IF_NIF_ETH;

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
imb_cdu_port_pm_id_get(
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
 * \brief - this function will configure a struct of type portmod_pm_create_info_t
 *          before it is sent to portmod_port_macro_add
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_info - imb info to retrive CDU id
 * \param [in] pm_in_cdu_index - pm index inside cdu
 * \param [in] pm_info - pm configuration sturct
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
imb_cdu_port_macro_config(
    int unit,
    int pm_in_cdu_index,
    const imb_create_info_t * imb_info,
    portmod_pm_create_info_t * pm_info)
{
    bcm_port_t phy;
    bcm_pbmp_t pm_pbmp;
    int is_sim;
    int pm_index;
    int lane;
    uint32 rx_polarity, tx_polarity;
    phymod_lane_map_t lane_map = { 0 };
    DNX_SERDES_REF_CLOCK_TYPE ref_clk = DNX_SERDES_NOF_REF_CLOCKS;
    int lcpll;
    phymod_dispatch_type_t sim_type;
    int single_vco_restriction_disabled;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_nif.global.nof_lcplls_get(unit) > 1)
    {
        lcpll = imb_info->inst_id >= dnx_data_nif.eth.cdu_nof_per_core_get(unit) ? 1 : 0;
    }
    else
    {
        /** For devices w/o LCPLL, use fixed index 0 to retrieve ref clock from DNX-DATA */
        lcpll = 0;
    }
    /** update type and phys */
    pm_info->type = portmodDispatchTypePm8x50;
    pm_index = dnx_data_nif.eth.ethu_properties_get(unit, imb_info->inst_id)->pms[pm_in_cdu_index];
    pm_pbmp = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phys;
    BCM_PBMP_ITER(pm_pbmp, phy)
    {
        BCM_PBMP_PORT_ADD(pm_info->phys, phy);
        /** lane polarity get */
        rx_polarity = dnx_data_nif.phys.polarity_get(unit, (int) phy)->rx_polarity;
        tx_polarity = dnx_data_nif.phys.polarity_get(unit, (int) phy)->tx_polarity;
        lane = (int) phy % dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);
        pm_info->pm_specific_info.pm8x50.polarity.rx_polarity |= ((rx_polarity & 0x1) << lane);
        pm_info->pm_specific_info.pm8x50.polarity.tx_polarity |= ((tx_polarity & 0x1) << lane);
    }

    ref_clk = dnx_data_pll.general.nif_pll_get(unit, lcpll)->out_freq;
    if (ref_clk == DNX_SERDES_REF_CLOCK_BYPASS)
    {
        ref_clk = dnx_data_pll.general.nif_pll_get(unit, lcpll)->in_freq;
    }
    SHR_IF_ERR_EXIT(dnx_to_phymod_ref_clk(unit, ref_clk, &(pm_info->pm_specific_info.pm8x50.ref_clk)));

    /** As we do not know the speeds of the ports on the PM yet,we set vco to null */
    pm_info->pm_specific_info.pm8x50.tvco = portmodVCOInvalid;
    pm_info->pm_specific_info.pm8x50.ovco = portmodVCOInvalid;

    /** update access struct with cdu info */
    SHR_IF_ERR_EXIT(phymod_access_t_init(&(pm_info->pm_specific_info.pm8x50.access.access)));
    pm_info->pm_specific_info.pm8x50.access.access.user_acc = imb_info->imb_specific_info.cdu.user_acc;
    pm_info->pm_specific_info.pm8x50.access.access.addr = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phy_addr;
    pm_info->pm_specific_info.pm8x50.access.access.bus = NULL;  /* Use default bus */
    pm_info->pm_specific_info.pm8x50.access.access.tvco_pll_index =
        dnx_data_nif.eth.pm_properties_get(unit, pm_index)->tvco_pll_index;

    sim_type = dnx_data_nif.simulator.cdu_type_get(unit);
    SHR_IF_ERR_EXIT(soc_physim_check_sim
                    (unit, sim_type, &(pm_info->pm_specific_info.pm8x50.access.access),
                     (imb_info->inst_id << 16), &is_sim));

    if (is_sim)
    {
        pm_info->pm_specific_info.pm8x50.fw_load_method = phymodFirmwareLoadMethodNone;
        /** enable clause45 just for sim - used by phy sim */
        PHYMOD_ACC_F_CLAUSE45_SET(&pm_info->pm_specific_info.pm8x50.access.access);
        PHYMOD_ACC_F_PHYSIM_SET(&pm_info->pm_specific_info.pm8x50.access.access);
    }
    else
    {
        pm_info->pm_specific_info.pm8x50.fw_load_method = dnx_data_port.static_add.nif_fw_load_method_get(unit);
    }

    pm_info->pm_specific_info.pm8x50.lane_map = lane_map; /** lane map soc property will be read at a later stage, we pass an empty lane map for now*/

    pm_info->pm_specific_info.pm8x50.external_fw_loader = NULL; /**Tells Portmod to use default external loader */
    pm_info->pm_specific_info.pm8x50.portmod_mac_soft_reset = imb_cdu_portmod_soft_reset;
    pm_info->pm_specific_info.pm8x50.portmod_egress_buffer_reset = imb_ethu_port_credit_tx_reset;
    SHR_IF_ERR_EXIT(soc_dnxc_single_pll_restriction_disable_get(unit, &single_vco_restriction_disabled));
    pm_info->pm_specific_info.pm8x50.single_vco_used_by_ports = 0;
    if (single_vco_restriction_disabled == FALSE &&
        dnx_data_nif.eth.pm_properties_get(unit, pm_index)->special_pll_check)
    {
        pm_info->pm_specific_info.pm8x50.single_vco_used_by_ports = 1;
        pm_info->pm_specific_info.pm8x50.clock_buffer_disable_required = 1;
    }
    if (dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_pm8x50_bw_update))
    {
        pm_info->pm_specific_info.pm8x50.clock_buffer_bw_update_required = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get all CDU ports on the same MAC
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] mac_ports - bitmap of ports on the same mac
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
imb_cdu_mac_ports_get(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t * mac_ports)
{
    bcm_port_t port_i;
    bcm_pbmp_t cdu_ports;
    dnx_algo_port_ethu_access_info_t cdu_info, port_i_cdu_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * clear output pbmp
     */
    BCM_PBMP_CLEAR(*mac_ports);

    /*
     * get port and CDU info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &cdu_info));
    SHR_IF_ERR_EXIT(imb_ethu_pm_master_ports_get(unit, port, &cdu_ports));

    /*
     * iterate over all CDU ports and check if MAC is is the same as given port
     */
    BCM_PBMP_ITER(cdu_ports, port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port_i, &port_i_cdu_info));
        if (port_i_cdu_info.mac_id == cdu_info.mac_id)
        {
            BCM_PBMP_PORT_ADD(*mac_ports, port_i);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - get all CDU ports on the same MAC mode config
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] mac_ports - bitmap of ports on the same mac
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
imb_cdu_mac_mode_config_ports_get(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t * mac_ports)
{
    bcm_port_t port_i;
    bcm_pbmp_t cdu_ports;
    dnx_algo_port_ethu_access_info_t cdu_info, port_i_cdu_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * clear output pbmp
     */
    BCM_PBMP_CLEAR(*mac_ports);

    /*
     * get port and CDU info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &cdu_info));
    SHR_IF_ERR_EXIT(imb_ethu_pm_master_ports_get(unit, port, &cdu_ports));

    /*
     * iterate over all CDU ports and check if MAC is is the same as given port
     */
    BCM_PBMP_ITER(cdu_ports, port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port_i, &port_i_cdu_info));
        if (port_i_cdu_info.mac_mode_config_id == cdu_info.mac_mode_config_id)
        {
            BCM_PBMP_PORT_ADD(*mac_ports, port_i);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static int
imb_cdu_mac_nof_ports_get(
    int unit,
    bcm_port_t port,
    int *mac_nof_ports)
{
    bcm_pbmp_t mac_ports;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_mac_ports_get(unit, port, &mac_ports));
    BCM_PBMP_COUNT(mac_ports, *mac_nof_ports);

exit:
    SHR_FUNC_EXIT;
}

static int
imb_cdu_mac_mode_config_nof_ports_get(
    int unit,
    bcm_port_t port,
    int *mac_mode_config_nof_ports)
{
    bcm_pbmp_t mac_ports;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_mac_mode_config_ports_get(unit, port, &mac_ports));
    BCM_PBMP_COUNT(mac_ports, *mac_mode_config_nof_ports);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - calculated the new CDU MAC mode.
 * Suitable for both add and remove sequences
 *
 * \param [in] unit - chip unit id
 * \param [in] current_mac_mode - current_mac_mode
 * \param [in] first_lane_local - first_lane_local
 * \param [in] nof_phys - nof_phys
 * \param [out] mac_mode - MAC operating mode
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
imb_cdu_port_mac_mode_calc(
    int unit,
    int current_mac_mode,
    int first_lane_local,
    int nof_phys,
    int *mac_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (nof_phys)
    {
        case 8:
            *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_ONE_PORT_FOR_TWO_MACS;
            break;
        case 4:
            *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_ONE_PORT;
            break;
        case 2:
            switch (current_mac_mode)
            {
                case DBAL_ENUM_FVAL_NIF_MAC_MODE_FOUR_PORTS:
                    if (first_lane_local == 0)
                    {
                        *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_2_3;
                    }
                    else
                    {
                        *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_1_2;
                    }
                    break;
                case DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_1_2:
                    if (first_lane_local == 0)
                    {
                        *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_TWO_PORTS;
                    }
                    else
                    {
                        *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_1_2;
                    }
                    break;
                case DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_2_3:
                    if (first_lane_local == 0)
                    {
                        *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_2_3;
                    }
                    else
                    {
                        *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_TWO_PORTS;
                    }
                    break;
                case DBAL_ENUM_FVAL_NIF_MAC_MODE_TWO_PORTS:
                    *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_TWO_PORTS;
                    break;
                case DBAL_ENUM_FVAL_NIF_MAC_MODE_ONE_PORT:
                    *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_TWO_PORTS;
                    break;
                default:
                    break;
            }
            break;
        case 1:
            switch (current_mac_mode)
            {
                case DBAL_ENUM_FVAL_NIF_MAC_MODE_FOUR_PORTS:
                    *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_FOUR_PORTS;
                    break;
                case DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_1_2:
                    if ((first_lane_local == 0) || (first_lane_local == 1))
                    {
                        *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_1_2;
                    }
                    else if ((first_lane_local == 2) || (first_lane_local == 3))
                    {
                        *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_FOUR_PORTS;
                    }
                    break;
                case DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_2_3:
                    if ((first_lane_local == 0) || (first_lane_local == 1))
                    {
                        *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_FOUR_PORTS;
                    }
                    else if ((first_lane_local == 2) || (first_lane_local == 3))
                    {
                        *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_2_3;
                    }
                    break;
                case DBAL_ENUM_FVAL_NIF_MAC_MODE_TWO_PORTS:
                    if ((first_lane_local == 0) || (first_lane_local == 1))
                    {
                        *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_1_2;
                    }
                    else if ((first_lane_local == 2) || (first_lane_local == 3))
                    {
                        *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_2_3;
                    }
                    break;
                case DBAL_ENUM_FVAL_NIF_MAC_MODE_ONE_PORT:
                    *mac_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_FOUR_PORTS;
                    break;
                default:
                    break;
            }
            break;
        default:
            SHR_ERR_EXIT(BCM_E_INTERNAL, "Invalid number of phys %d \n", nof_phys);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the appropriate MAC operating mode from all
 *        enabled ports on the MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] mac_mode - MAC operating mode
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
imb_cdu_port_mac_mode_get(
    int unit,
    bcm_port_t port,
    int *mac_mode)
{

    bcm_pbmp_t port_phys;
    int nof_phys, first_phy, first_lane_local;
    int current_mac_mode;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get the needed parameters of the port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &port_phys));
    BCM_PBMP_COUNT(port_phys, nof_phys);
    _SHR_PBMP_FIRST(port_phys, first_phy);
    first_lane_local = first_phy % dnx_data_nif.eth.mac_mode_config_lanes_nof_get(unit);

    SHR_IF_ERR_EXIT(imb_cdu_port_mac_mode_config_get(unit, port, &current_mac_mode));

    SHR_IF_ERR_EXIT(imb_cdu_port_mac_mode_calc(unit, current_mac_mode, first_lane_local, nof_phys, mac_mode));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - map NIF port to EGQ interface
 *
 * \param [in] unit   - chip unit id
 * \param [in] port   - logical port
 * \param [in] enable - 1:enable mapping, 0:disable mapping
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_cdu_port_egq_interface_map(
    int unit,
    bcm_port_t port,
    int enable)
{
    dnx_algo_port_ethu_access_info_t cdu_info;
    dnx_algo_port_info_s port_info;
    int egq_if;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
    {
        /*
         * Get CDU access info
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &cdu_info));

        /*
         * get port egress interface
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egq_if));

        /*
         * Call internal function
         */
        SHR_IF_ERR_EXIT(imb_cdu_internal_nif_egq_interface_map
                        (unit, cdu_info.core, cdu_info.lane_in_core, egq_if, enable));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add new port to the CDU. config all CDU settings and
 *         call Portmod API for PM configurations
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port to be attched to the CDU
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
imb_cdu_port_attach(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    int mac_mode = 0;
    int is_master_port;
    int average_ipg_in_bits;
    portmod_port_add_info_t add_info;
    portmod_pause_control_t pause_control;
    portmod_rx_control_t rx_ctrl;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, port, &is_master_port));

    if (is_master_port)
    {
        if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_clock_power_down))
        {
            /** Power up the unit */
            SHR_IF_ERR_EXIT(imb_ethu_port_power_down_set(unit, port, FALSE));

            if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_tx_separate_clock_power_down))
            {
                /** Power up TX */
                SHR_IF_ERR_EXIT(imb_cdu_port_tx_power_down_set(unit, port, FALSE));
            }
        }

        /*
         * 1.  Enable TX data to PM
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_tx_data_to_pm_enable_set(unit, port, 1));

        /*
         * calculate new mac mode
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_mac_mode_get(unit, port, &mac_mode));

        /*
         * 5. Set ETHU port mode
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_mac_mode_config_set(unit, port, mac_mode, 1));
        SHR_IF_ERR_EXIT(imb_cdu_port_mac_enable_set(unit, port, 1));
    }

    /*
     * 7.  Call Portmod API
 */
    SHR_IF_ERR_EXIT(imb_cdu_portmod_add_info_config(unit, port, &add_info));
    SHR_IF_ERR_EXIT(imb_er_portmod_port_add(unit, port, &add_info));

    if (is_master_port)
    {
        DNX_ERR_RECOVERY_SUPPRESS(unit);
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, 0));
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);

        /*
         * Reset Async unit
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_async_unit_reset(unit, port, IMB_COMMON_OUT_OF_RESET));

        /*
         * Pause - to be consistent with legacy disable TX and enable RX
         */
        SHR_IF_ERR_EXIT(portmod_pause_control_t_init(unit, &pause_control));
        pause_control.tx_enable = FALSE;
        pause_control.rx_enable = TRUE;
        DNX_ERR_RECOVERY_SUPPRESS(unit);
        SHR_IF_ERR_EXIT(portmod_port_pause_control_set(unit, port, &pause_control));
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);
        /*
         * Control Frames - to be consistent with legacy control frames (
         * including PFC frames wih ether type 0x8808) should be dropped in CDMAC
         */
        rx_ctrl.flags = PORTMOD_MAC_PASS_CONTROL_FRAME;
        rx_ctrl.pass_control_frames = FALSE;
        DNX_ERR_RECOVERY_SUPPRESS(unit);
        SHR_IF_ERR_EXIT(portmod_port_rx_control_set(unit, port, &rx_ctrl));
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    }

    if (is_master_port && (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support) == FALSE))
    {
        /*
         * Map NIF port to EGQ interface
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_egq_interface_map(unit, port, TRUE));
    }

    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        /** Mask EGQ credit if L1 port */
        SHR_IF_ERR_EXIT(imb_ethu_port_credit_mask_set
                        (unit, port, (DNX_ALGO_PORT_TYPE_IS_ETH_L1(unit, port_info) ? 1 : 0)));
        /** Configure average IPG for L1 port */
        if (DNX_ALGO_PORT_TYPE_IS_ETH_L1(unit, port_info))
        {
            average_ipg_in_bits = dnx_data_nif.flexe.average_ipg_for_l1_get(unit);
            SHR_IF_ERR_EXIT(portmod_port_tx_average_ipg_set(unit, port, average_ipg_in_bits));
        }
    }

    /*
     * 2. Configure the SCH
     * 7. Map RMC to physical port
     * 8. Map physical port to RMC
     * 9. Set RX threshold after overflow
     * 4. Reset ports in NMG
     * 11. Reset ports in CDU
     *
     * --> These stages are done during port level tuning.
     * it is important all tuning will be done before the port is enabled!
 */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove a port from the CDU. first calls Portmos API
 *        to remove from portmod DB, then configures all CDU
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
imb_cdu_port_detach(
    int unit,
    bcm_port_t port)
{
    int mac_nof_ports = 0, mac_mode_config_nof_ports = 0, nof_channels = 0;
    int pm_id;
    phymod_autoneg_control_t an;
    bcm_pbmp_t ethu_ports;
    int port_count;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Disable AN before detaching port
     */
    phymod_autoneg_control_t_init(&an);
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    SHR_IF_ERR_EXIT(imb_port_autoneg_get(unit, port, 0, &an));
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    if (an.enable)
    {
        an.enable = 0;
        SHR_IF_ERR_EXIT(imb_port_autoneg_set(unit, port, 0, &an));
    }
    /*
     * Get PM ID before removing the port in portmod
     */
    SHR_IF_ERR_EXIT(portmod_port_pm_id_get(unit, port, &pm_id));
    /*
     * 13.  Call Portmod API
     */
    SHR_IF_ERR_EXIT(imb_er_portmod_port_remove(unit, port));
    /*
     * get number of ports on the MAC
     */
    SHR_IF_ERR_EXIT(imb_cdu_mac_nof_ports_get(unit, port, &mac_nof_ports));
    /*
     * get number of ports on the MAC mode config
     */
    SHR_IF_ERR_EXIT(imb_cdu_mac_mode_config_nof_ports_get(unit, port, &mac_mode_config_nof_ports));
    /*
     * get number of channels
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, port, &nof_channels));

    if (nof_channels == 1)
    {
        /*
         * 4. Reset ports in NMG
         * 11. Reset ports in CDU
         * 2. Configure the SCH
         * 9. Set RX threshold after overflow
         * 8. Map physical port to RMC
         * 7. Map RMC to physical port
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_remove_rmc_lane_unmap(unit, port));

        /*
         * stop the egr flush
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_tx_egress_flush_set(unit, port, 0));

        if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support) == FALSE)
        {
            /*
             * Unmap NIF port
             */
            SHR_IF_ERR_EXIT(imb_cdu_port_egq_interface_map(unit, port, FALSE));
        }
    }

    if ((mac_nof_ports == 1) && (nof_channels == 1))
    {
        /*
         * 6. Reset Async unit
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_async_unit_reset(unit, port, IMB_COMMON_IN_RESET));
        /*
         * 5. Set CDU port mode
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_mac_enable_set(unit, port, 0));
    }

    if (mac_mode_config_nof_ports == 1)
    {
        /*
         * disable MAC mode mode when last channel in that mac mode config unit.
         */
        SHR_IF_ERR_EXIT(imb_cdu_port_mac_mode_config_set(unit, port, 0, 0));
    }

    if (nof_channels == 1)
    {
        /*
         * 1.  Disable TX data to PM
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_tx_data_to_pm_enable_set(unit, port, 0));
    }

    if (nof_channels == 1 && dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_clock_power_down))
    {
        /** last port on the interface */

        SHR_IF_ERR_EXIT(imb_ethu_master_ports_get(unit, port, &ethu_ports));
        _SHR_PBMP_COUNT(ethu_ports, port_count);

        if (port_count == 1)
        {
            /** Last port on the unit */

            /** Power down the unit */
            SHR_IF_ERR_EXIT(imb_ethu_port_power_down_set(unit, port, TRUE));

            if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_tx_separate_clock_power_down) &&
                dnx_data_nif.flexe.flexe_mode_get(unit) == DNX_FLEXE_MODE_DISABLED)
            {
                /** In FlexE mode, do not power down TX */

                /** Power down TX */
                SHR_IF_ERR_EXIT(imb_cdu_port_tx_power_down_set(unit, port, TRUE));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable/ disable eee for the port.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] enable - enable indication
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_port_eee_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    portmod_eee_t eee;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * enable EEE in Portmod
     */
    SHR_IF_ERR_EXIT(portmod_port_eee_get(unit, port, &eee));
    eee.enable = enable;
    SHR_IF_ERR_EXIT(portmod_port_eee_set(unit, port, &eee));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get EEE enable indication for the port_update_control
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] enable - enable indication
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_port_eee_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    portmod_eee_t eee;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * we can take the indication directly from Pormod
     */
    SHR_IF_ERR_EXIT(portmod_port_eee_get(unit, port, &eee));
    *enable = eee.enable;
exit:SHR_FUNC_EXIT;

}

/**
 * \brief - Set port advertise abilities
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in]  num_ability     - number of ability the port will advertise
 * \param [in]  abilities       - local advertisement for each ability
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
imb_cdu_port_autoneg_ability_advert_set(
    int unit,
    bcm_port_t port,
    int num_ability,
    const bcm_port_speed_ability_t * abilities)
{
    portmod_port_speed_ability_t *an_abilities = NULL;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(an_abilities, num_ability * sizeof(portmod_port_speed_ability_t), "port an abilities", "%s%s%s\r\n",
              EMPTY, EMPTY, EMPTY);
    sal_memset(an_abilities, 0, num_ability * sizeof(portmod_port_speed_ability_t));
    /*
     * According to DS, 20G is not supported"
     */
    for (i = 0; i < num_ability; i++)
    {
        if (abilities[i].speed == 20000)
        {
            SHR_ERR_EXIT(SOC_E_CONFIG, "Port %d: abilities[%d] CAN'T support 20000 speed\n", port, i);
        }
    }
    imb_portmod_an_ability_from_bcm_an_ability_get(num_ability, abilities, an_abilities);

    SHR_IF_ERR_EXIT(portmod_port_autoneg_ability_advert_set(unit, port, num_ability, an_abilities));
exit:
    SHR_FREE(an_abilities);
    SHR_FUNC_EXIT;

}

/**
 * \brief - Get port advertise abilities
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in]  max_num_ability   - the max num of autoneg ability this port can advertise
 * \param [out]  abilities       - indicate the ability this port can advertise
 * \param [out]  actual_num_ability   - the actual num of ability that this port can advertise
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
imb_cdu_port_autoneg_ability_advert_get(
    int unit,
    bcm_port_t port,
    int max_num_ability,
    bcm_port_speed_ability_t * abilities,
    int *actual_num_ability)
{
    portmod_port_speed_ability_t *an_abilities = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(an_abilities, max_num_ability * sizeof(portmod_port_speed_ability_t), "port an abilities", "%s%s%s\r\n",
              EMPTY, EMPTY, EMPTY);
    sal_memset(an_abilities, 0, max_num_ability * sizeof(portmod_port_speed_ability_t));

    SHR_IF_ERR_EXIT(portmod_port_autoneg_ability_advert_get
                    (unit, port, max_num_ability, an_abilities, actual_num_ability));

    imb_bcm_an_ability_from_portmod_an_ability_get(*actual_num_ability, an_abilities, abilities);

exit:
    SHR_FREE(an_abilities);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the local speed abilities
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in]  max_num_ability   - the max num of autoneg ability this port can advertise
 * \param [out]  abilities       - indicate the ability this port can advertise
 * \param [out]  actual_num_ability   - the actual num of ability that this port can advertise
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
imb_cdu_port_speed_ability_local_get(
    int unit,
    bcm_port_t port,
    int max_num_ability,
    bcm_port_speed_ability_t * abilities,
    int *actual_num_ability)
{
    portmod_port_speed_ability_t *an_abilities = NULL;
    portmod_port_speed_ability_t *raw_abilities = NULL;
    int raw_num_abilities = 0;
    int i = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(an_abilities, max_num_ability * sizeof(portmod_port_speed_ability_t), "port an abilities",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    sal_memset(an_abilities, 0, max_num_ability * sizeof(portmod_port_speed_ability_t));
    SHR_ALLOC(raw_abilities, max_num_ability * sizeof(portmod_port_speed_ability_t), "raw port abilities",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    sal_memset(raw_abilities, 0, max_num_ability * sizeof(portmod_port_speed_ability_t));

    SHR_IF_ERR_EXIT(portmod_port_speed_ability_local_get
                    (unit, port, max_num_ability, raw_abilities, &raw_num_abilities));

    /*
     * Filter out speed combinations suggested in "imb_cdu_fec_speed_validate"
     */
    *actual_num_ability = 0;
    for (i = 0; i < raw_num_abilities; i++)
    {
        if ((raw_abilities[i].speed == 20000) || (raw_abilities[i].speed == 12000) ||
            (raw_abilities[i].speed == 50000 && raw_abilities[i].num_lanes == 1 &&
             raw_abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_NONE) ||
            (raw_abilities[i].speed == 200000 && raw_abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_544) ||
            (raw_abilities[i].speed == 200000 && raw_abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_NONE) ||
            (raw_abilities[i].speed == 200000 && raw_abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_RS_272) ||
            (raw_abilities[i].speed == 100000 && raw_abilities[i].num_lanes == 2
             && raw_abilities[i].fec_type == PORTMOD_PORT_PHY_FEC_NONE))
        {
            continue;
        }
        else
        {
            sal_memcpy(&an_abilities[*actual_num_ability], &raw_abilities[i], sizeof(portmod_port_speed_ability_t));
            (*actual_num_ability) += 1;
        }
    }
    imb_bcm_an_ability_from_portmod_an_ability_get(*actual_num_ability, an_abilities, abilities);

exit:
    SHR_FREE(raw_abilities);
    SHR_FREE(an_abilities);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the remote abilities
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in]  max_num_ability   - the max num of autoneg ability this port can advertise
 * \param [out]  abilities       - indicate the ability this port can advertise
 * \param [out]  actual_num_ability   - the actual num of ability that this port can advertise
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
imb_cdu_port_autoneg_ability_remote_get(
    int unit,
    bcm_port_t port,
    int max_num_ability,
    bcm_port_speed_ability_t * abilities,
    int *actual_num_ability)
{
    portmod_port_speed_ability_t *an_abilities = NULL;
    phymod_autoneg_status_t an_status;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_autoneg_status_get(unit, port, &an_status));

    if (!an_status.enabled)
    {
        SHR_ERR_EXIT(_SHR_E_DISABLED, "AN is not enabled for port %d! \n", port);
    }
    else if (!an_status.locked)
    {
        SHR_ERR_EXIT(_SHR_E_BUSY, "AN status is not locked for port %d! \n", port);
    }
    else
    {
        SHR_ALLOC(an_abilities, max_num_ability * sizeof(portmod_port_speed_ability_t), "port an abilities",
                  "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        sal_memset(an_abilities, 0, max_num_ability * sizeof(portmod_port_speed_ability_t));

        SHR_IF_ERR_EXIT(portmod_port_autoneg_ability_remote_get
                        (unit, port, max_num_ability, an_abilities, actual_num_ability));
        imb_bcm_an_ability_from_portmod_an_ability_get(*actual_num_ability, an_abilities, abilities);
    }

exit:
    SHR_FREE(an_abilities);
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
imb_cdu_port_resource_default_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_port_resource_t * resource)
{
    bcm_pbmp_t phys;
    int num_lane;
    int lane_rate;
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
     *  calculate lane rate
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
    BCM_PBMP_COUNT(phys, num_lane);
    lane_rate = resource->speed / num_lane;

    /*
     * in case user didn't configure the FEC already - give default value
     */
    if (resource->fec_type == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {

        switch (lane_rate)
        {
            case 10000:
            case 12000:
            case 20000:
            case 25000:
                resource->fec_type = bcmPortPhyFecNone;
                break;
            case 50000:

                if ((num_lane == 8) || (num_lane == 4))
                {
                    /*
                     * For 400G,200G return 544 2xN
                     */
                    resource->fec_type = bcmPortPhyFecRs544_2xN;
                }
                else
                {
                    resource->fec_type = bcmPortPhyFecRs544;
                }

                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "port %d: Invalid speed (%d) and number of lanes (%d) configuration",
                             port, resource->speed, num_lane);
        }
    }

    /*
     * in case user didn't configure CL72 - by default it is enabled
     */
    if (resource->link_training == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->link_training = 1;
    }

    /*
     * get default lane_config
     */
    if (resource->phy_lane_config == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->phy_lane_config = 0;
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(resource->phy_lane_config,
                                                     SOC_DNXC_PORT_LANE_CONFIG_MEDIA_TYPE_BACKPLANE_COPPER_TRACE);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_SCRAMBLING_DISABLE_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_SET(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_PREC_EN_CLEAR(resource->phy_lane_config);

        /*
         * FORCE_ES, FORCE_NS must be clear for {NRZ} and for {PAM4 with link training}
         */
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_CLEAR(resource->phy_lane_config);

        if (DNXC_PORT_PHY_SPEED_IS_PAM4(lane_rate) && (resource->link_training == 0))
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_SET(resource->phy_lane_config);
        }

        /*
         * internal fields that will be set in resource_set before calling portmod. Not exposed to user.
         */
        PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_CLEAR(resource->phy_lane_config);
        PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_CLEAR(resource->phy_lane_config);
        /*
         * this field should always be 0 in normal operation, can be changed only for debug.
         */
        PORTMOD_PORT_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_SET(resource->phy_lane_config, 0);

    }

    resource->port = port;

exit:
    SHR_FUNC_EXIT;
}

/*
 * imb_cdu_phys_get
 *
 * @brief Get phys associated with imb instance
 *
 * @param [in]  unit             - unit id
 * @param [in]  imb_id           - imb instance id
 * @param [out]  phys            - phys associated with imb instance
 */
int
imb_cdu_phys_get(
    int unit,
    int imb_id,
    bcm_pbmp_t * phys)
{

    int pm_index;
    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_CLEAR(*phys);
    pm_index = dnx_data_nif.eth.ethu_properties_get(unit, imb_id)->pms[0];
    *phys = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phys;

    SHR_FUNC_EXIT;
}

/**
 * \brief - set the TX start threshold for the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] speed - port interface rate
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
imb_cdu_port_tx_start_thr_set(
    int unit,
    bcm_port_t port,
    int speed)
{
    int start_thr = -1;
    int idx;
    dnx_algo_port_ethu_access_info_t cdu_info;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get global TX threshold definition
     */
    start_thr = dnx_data_nif.global.start_tx_threshold_global_get(
    unit);
    if (start_thr < 0)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        /*
         * Global TX threshold not defined.
         * Lookup TX threshold in table.
         */
        for (idx = 0; idx < dnx_data_nif.eth.start_tx_threshold_table_info_get(unit)->key_size[0]; idx++)
        {
            if (speed <= dnx_data_nif.eth.start_tx_threshold_table_get(unit, idx)->speed)
            {
                start_thr =
                    DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info) ? dnx_data_nif.eth.start_tx_threshold_table_get(unit,
                                                                                                              idx)->start_thr_for_l1
                    : dnx_data_nif.eth.start_tx_threshold_table_get(unit, idx)->start_thr;

                if (start_thr == 0)
                {
                    /** If the value is 0, means the there is no valid start_thr for the current speed */
                    continue;
                }
                else
                {
                    break;
                }
            }
        }

        if (start_thr < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported speed %d (port %d)\n", speed, port);
        }
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &cdu_info));
    SHR_IF_ERR_EXIT(imb_cdu_tx_start_thr_hw_set
                    (unit, cdu_info.core, cdu_info.ethu_id_in_core, cdu_info.first_lane_in_port, start_thr));

exit:
    SHR_FUNC_EXIT;
}

int
imb_cdu_prd_threshold_max_get(
    int unit,
    bcm_port_t port,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    *threshold = dnx_data_nif.prd.cdu_rmc_threshold_max_get(unit);

    SHR_FUNC_EXIT;
}

/**
 * \brief - Check if phy control is supported for current PM.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] type - Phy Control type
 * \param [in] is_set - if this fuction is called by set function
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
imb_cdu_port_phy_control_validate(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    int is_set)
{
    bcm_port_resource_t resource;
    dnx_algo_port_info_s port_info;
    bcm_pbmp_t phys;
    int nof_lanes;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Sepcial handle for PRBs function, as port control PRBs function needs to
     * call the soc layer PRBs function to deal with Per Lane Gport.
     * Therefore we must ensure the PRBs is supported by soc layer common API.
     */
    switch (type)
    {
        case BCM_PORT_PHY_CONTROL_PRBS_POLYNOMIAL:
        case BCM_PORT_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        case BCM_PORT_PHY_CONTROL_PRBS_RX_INVERT_DATA:
        case BCM_PORT_PHY_CONTROL_PRBS_TX_ENABLE:
        case BCM_PORT_PHY_CONTROL_PRBS_RX_ENABLE:
        case BCM_PORT_PHY_CONTROL_PRBS_RX_STATUS:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "PHY control type %d is not supported, please use bcm_port_control_set/get instead.\r\n",
                         type);
            break;
        }
        case BCM_PORT_PHY_CONTROL_LINK_TRAINING_INIT_TX_FIR_POST:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "PHY control type %d is not supported on Blackhawk PHY.\r\n", type);
            break;
        }
        case BCM_PORT_PHY_CONTROL_FLEXE_50G_NOFEC_20K_AM_SPACING_ENABLE:
        {
            SHR_IF_ERR_EXIT(imb_common_port_resource_get(unit, port, &resource));
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
            _SHR_PBMP_COUNT(phys, nof_lanes);
            if (!port_info.is_flexe_phy)
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                             "PHY control type %d set is supported on Blackhawk PHY but only for FlexE ports.\r\n",
                             type);
            }
            else
            {
                if (resource.speed != 50000 || nof_lanes != 2 || resource.fec_type != bcmPortPhyFecNone)
                {
                    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                                 "PHY control type %d set is supported only on FlexE PHY port with 50G, 2 lanes and no FEC.\r\n",
                                 type);
                }
            }

            break;
        }
        /** The soc layer will verify the rest types */
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Validate that the MAC mode transition is legal
 *
 * \param [in] unit - chip unit id.
 * \param [in] port_to_add - logical port to be added
 * \param [in] port_to_add_phys - phys of the port to be added
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * needed only because of described in
 * dnx_data_nif.features.feature_get(unit, dnx_data_nif_features_pm_mode_0122_invalid
 * \see
 *   * None
 */
int
imb_cdu_port_mac_mode_validate(
    int unit,
    bcm_port_t port_to_add,
    bcm_pbmp_t port_to_add_phys)
{
    int port_to_add_first_phy, port_to_add_mac_id, port_i;
    int port_i_enable_status = FALSE, mac_has_enabled_ports = FALSE;
    int first_lane_local, nof_port_lanes;
    bcm_pbmp_t ports_on_same_mac;
    int current_mac_mode = DBAL_NOF_ENUM_NIF_MAC_MODE_VALUES, new_mac_mode = DBAL_NOF_ENUM_NIF_MAC_MODE_VALUES;
    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_FIRST(port_to_add_phys, port_to_add_first_phy);

    port_to_add_mac_id = port_to_add_first_phy / dnx_data_nif.eth.mac_mode_config_lanes_nof_get(unit);
    first_lane_local = port_to_add_first_phy % dnx_data_nif.eth.mac_mode_config_lanes_nof_get(unit);
    BCM_PBMP_COUNT(port_to_add_phys, nof_port_lanes);

    SHR_IF_ERR_EXIT(dnx_algo_mac_mode_config_id_ports_get(unit, port_to_add_mac_id, &ports_on_same_mac));

    /**
     * Check if any enabled ports on the MAC
     * and get the MAC PM mode
     * If no ports on the MAC skip checks
     */
    BCM_PBMP_ITER(ports_on_same_mac, port_i)
    {
        SHR_IF_ERR_EXIT(imb_cdu_port_mac_mode_config_get(unit, port_i, &current_mac_mode));
        SHR_IF_ERR_EXIT(imb_ethu_port_enable_get(unit, port_i, &port_i_enable_status));
        if (port_i_enable_status)
        {
            mac_has_enabled_ports = TRUE;
            break;
        }
    }

    if (mac_has_enabled_ports)
    {
        SHR_IF_ERR_EXIT(imb_cdu_port_mac_mode_calc
                        (unit, current_mac_mode, first_lane_local, nof_port_lanes, &new_mac_mode));

        if (new_mac_mode == DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_1_2
            && current_mac_mode != DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_1_2)
        {

            SHR_ERR_EXIT(SOC_E_CONFIG,
                         "Current PM mode is NOT 0122 and driver was about to switch it to triple mode (1-lane port, 1-lane port, 2-lane port) which is invalid."
                         " \n You can create this configuration only statically for this device or by disabling all ports on the relevant MAC!\n");
        }

        if (current_mac_mode == DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_1_2
            && new_mac_mode != DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_1_2)
        {
            SHR_ERR_EXIT(SOC_E_CONFIG,
                         "Current PM mode is 0122 (1-lane port, 1-lane port, 2-lane port) and the driver was about to force it to switch out of it, which is invalid."
                         " \n You can create this configuration only statically for this device or by disabling all ports on the relevant MAC!\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Implement here all the required CDU related checks
 * for ports to be added at an early stage
 * before they are added to any SW DB
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port to be added
 * \param [in] phys - phys of the port to be added
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * needed only because of described in
 * dnx_data_nif.features.feature_get(unit, dnx_data_nif_features_pm_mode_0122_invalid
 * \see
 *   * None
 */
int
imb_cdu_port_pre_add_validate(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t phys)
{

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_nif.features.feature_get(unit, dnx_data_nif_features_pm_mode_0122_invalid))
    {
        SHR_IF_ERR_EXIT(imb_cdu_port_mac_mode_validate(unit, port, phys));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify input AN abilities.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] num_ability - number of abilities
 * \param [in] abilities - array of abilities
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
imb_cdu_port_autoneg_ability_verify(
    int unit,
    bcm_port_t port,
    int num_ability,
    const bcm_port_speed_ability_t * abilities)
{
    int nof_lanes;
    int speed;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the number of lane of the port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &nof_lanes));
    /*
     * Get the speed of the port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, 0, &speed));

    /*
     * Before setting AN, user must configure the port with force-speed using bcm_port_resource_set API.
     * AN can negotiate the speed up to the speed that was set in the last call to bcm_port_resource_set for the same port.
     */
    for (i = 0; i < num_ability; ++i)
    {
        if (abilities[i].speed > speed)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "abilities.speed(%d) exceeds port's maximum speed(%d).\r\n",
                         abilities[i].speed, speed);
        }
        if (abilities[i].resolved_num_lanes > nof_lanes)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "abilities.resolved_num_lanes(%d) exceeds port's number of lanes(%d).\r\n",
                         abilities[i].resolved_num_lanes, nof_lanes);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Perform the full ber projection test.
 *   Configures, collectes are returns the values.
 * \param [in] unit - The unit number.
 * \param [in] port - The port.
 * \param [in] lane - The lane.
 * \param [in] ber_proj - BER test parametes.
 * \param [in] max_errcnt - Maximumm number of lanes to test.
 * \param [out] errcnt_array - The result array.
 * \param [out] actual_errcnt - Number of valid elements in errcnt_array

 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_cdu_port_post_ber_proj_get(
    int unit,
    bcm_port_t port,
    int lane,
    const bcm_port_ber_proj_params_t * ber_proj,
    int max_errcnt,
    bcm_port_ber_proj_analyzer_errcnt_t * errcnt_array,
    int *actual_errcnt)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_port_post_ber_proj_get
                    (unit, port, lane, ber_proj, max_errcnt, errcnt_array, actual_errcnt));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure TX power down in CDUs of this port
 */
int
imb_cdu_port_tx_power_down_set(
    int unit,
    bcm_port_t port,
    int power_down)
{
    dnx_algo_port_ethu_access_info_t access_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &access_info));

    SHR_IF_ERR_EXIT(imb_cdu_tx_power_down_set(unit, access_info.core, access_info.ethu_id_in_core, power_down));

exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
