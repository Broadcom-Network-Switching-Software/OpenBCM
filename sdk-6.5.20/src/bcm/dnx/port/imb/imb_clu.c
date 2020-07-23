
/** \file imb_clu.c
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
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <sal/core/boot.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <soc/cmic.h>
#include <soc/mcm/memregs.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/port/imb/imb_clu.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <soc/dnx/pll/pll.h>
#include <soc/phy/phymod_sim.h>
#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pll.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_port_types.h>

#include <include/bcm_int/dnx/port/imb/imb_ethu.h>
#include "imb_ethu_internal.h"
#include "imb_clu_internal.h"
#include "imb_utils.h"
#include <soc/dnx/dbal/dbal.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*!
 * imb_clu_portmod_soft_reset
 *
 * @brief external reset function
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  action          - when is this callback called
 */
int
imb_clu_portmod_soft_reset(
    int unit,
    int port,
    portmod_call_back_action_type_t action)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, TRUE))
    {
        switch (action)
        {
            case portmodCallBackActionTypeDuring:
                SHR_IF_ERR_EXIT(imb_ethu_port_credit_tx_reset(unit, port));
                break;
            case portmodCallBackActionTypePre:
                SHR_IF_ERR_EXIT(imb_clu_rx_gear_shift_reset_hw_set
                                (unit, ethu_info.imb_type_id, ethu_info.first_lane_in_port, 1));
                break;
            case portmodCallBackActionTypePost:
                SHR_IF_ERR_EXIT(imb_clu_rx_gear_shift_reset_hw_set
                                (unit, ethu_info.imb_type_id, ethu_info.first_lane_in_port, 0));
                break;

            default:
                break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set PM TSC reference clock source and enable reference clock output to other TSC according to master/slave mode
 *
 * \param [in] unit - chip unit id
 * \param [in] pm_in_clu_index - pm index inside clu*
 * \param [in] imb_info - imb info to retrive CLU id

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
imb_clu_pm_tsc_refclock_set(
    int unit,
    int pm_in_clu_index,
    const imb_create_info_t * imb_info)
{

    uint32 pm_index;
    bcm_pbmp_t pm_pbmp;
    int pm_id;
    int first_phy;
    int is_master;
    int tsc_ref_in, tsc_ref_out;

    SHR_FUNC_INIT_VARS(unit);

    pm_index = dnx_data_nif.eth.ethu_properties_get(unit, imb_info->inst_id)->pms[pm_in_clu_index];
    pm_pbmp = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phys;
    is_master = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->is_master;

    BCM_PBMP_ITER(pm_pbmp, first_phy)
    {
        break;
    }

    /*
     * For master PMs select PAD (REF_IN == 0) ; For slave select LCREF (REF_IN == 1)
     */
    tsc_ref_in = is_master ? 0 : 1;

    /*
     * For master enable LCREFOUT (REF_OUT == 1)
     */
    tsc_ref_out = is_master ? 1 : 0;

    SHR_IF_ERR_EXIT(portmod_phy_pm_id_get(unit, first_phy, &pm_id));
    SHR_IF_ERR_EXIT(portmod_pm_tsc_refclock_set(unit, pm_id, tsc_ref_in, tsc_ref_out));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - this function will configure a struct of type portmod_pm_create_info_t for CLU
 *          before it is sent to portmod_port_macro_add
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_info - imb info to retrive CLU id
 * \param [in] pm_in_clu_index - pm index inside clu
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
imb_clu_port_macro_config(
    int unit,
    int pm_in_clu_index,
    const imb_create_info_t * imb_info,
    portmod_pm_create_info_t * pm_info)
{
    bcm_port_t phy;
    bcm_pbmp_t pm_pbmp;
    int is_sim;
    int pm_index;
    int lane;
    uint32 rx_polarity, tx_polarity;
    DNX_SERDES_REF_CLOCK_TYPE ref_clk = DNX_SERDES_NOF_REF_CLOCKS;
    int lcpll;
    int nof_pms_in_clu;
    phymod_lane_map_t lane_map = { 0 };
    phymod_dispatch_type_t sim_type;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * CLU supported devices (J2C and above) does not contain LCPLLs
     * LCPLL index is used to retrieve REF CLOCK (IN/OUT) from DNX DATA
 */
    lcpll = 0;

    /** update type and phys */
    pm_info->type = portmodDispatchTypePm4x25;
    pm_index = dnx_data_nif.eth.ethu_properties_get(unit, imb_info->inst_id)->pms[pm_in_clu_index];
    nof_pms_in_clu = dnx_data_nif.eth.nof_pms_in_clu_get(unit);
    pm_pbmp = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phys;
    BCM_PBMP_ITER(pm_pbmp, phy)
    {
        BCM_PBMP_PORT_ADD(pm_info->phys, phy);
        /** lane polarity get */
        rx_polarity = dnx_data_nif.phys.polarity_get(unit, (int) phy)->rx_polarity;
        tx_polarity = dnx_data_nif.phys.polarity_get(unit, (int) phy)->tx_polarity;
        lane = (int) phy % (dnx_data_nif.eth.nof_lanes_in_clu_get(unit) / nof_pms_in_clu);
        pm_info->pm_specific_info.pm4x25.polarity.rx_polarity |= ((rx_polarity & 0x1) << lane);
        pm_info->pm_specific_info.pm4x25.polarity.tx_polarity |= ((tx_polarity & 0x1) << lane);
    }

    ref_clk = dnx_data_pll.general.nif_pll_get(unit, lcpll)->out_freq;
    if (ref_clk == DNX_SERDES_REF_CLOCK_BYPASS)
    {
        ref_clk = dnx_data_pll.general.nif_pll_get(unit, lcpll)->in_freq;
    }
    SHR_IF_ERR_EXIT(dnx_to_phymod_ref_clk(unit, ref_clk, &(pm_info->pm_specific_info.pm4x25.ref_clk)));

    pm_info->pm_specific_info.pm4x25.portmod_mac_soft_reset = imb_clu_portmod_soft_reset; /**Used when soft reset require external method.*/

    pm_info->pm_specific_info.pm4x25.external_fw_loader = NULL; /**Tells Portmod to use default external loader */

    pm_info->pm_specific_info.pm4x25.rescal = -1;               /**Calibration for the Portmacros on a chip (usually is not required)*/

    /** update access struct with clu info */
    SHR_IF_ERR_EXIT(phymod_access_t_init(&(pm_info->pm_specific_info.pm4x25.access)));
    pm_info->pm_specific_info.pm4x25.access.user_acc =
        &((portmod_default_user_access_t *) imb_info->imb_specific_info.clu.user_acc)[pm_in_clu_index];
    pm_info->pm_specific_info.pm4x25.access.addr = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phy_addr;
    pm_info->pm_specific_info.pm4x25.access.bus = NULL; /* Use default bus */
    pm_info->pm_specific_info.pm4x25.lane_map = lane_map;

    sim_type = dnx_data_nif.simulator.clu_type_get(unit);
    SHR_IF_ERR_EXIT(soc_physim_check_sim
                    (unit, sim_type, &(pm_info->pm_specific_info.pm4x25.access), (imb_info->inst_id << 16), &is_sim));

    if (is_sim)
    {
        pm_info->pm_specific_info.pm4x25.fw_load_method = phymodFirmwareLoadMethodNone;
    }
    else
    {
        pm_info->pm_specific_info.pm4x25.fw_load_method = dnx_data_port.static_add.nif_fw_load_method_get(unit);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize CLU:
 * 1. Initialize CLU Database.
 * 2. Set some Reg values to get to desired SW start point
 * 3. Initialize PM init info for PMs and call port_macro_add()
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_info - IMB info required for
 *        initialization.
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
imb_clu_init(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    int pm_i;
    int clu_id, ethu_id, rmc, nof_rmcs;
    portmod_pm_create_info_t pm_info;
    bcm_core_t core = 0;
    uint32 nof_pms_in_clu = dnx_data_nif.eth.nof_pms_in_clu_get(unit);
    portmod_default_user_access_t *user_acc;
    uint32 pm_index;

    int i;
    SHR_FUNC_INIT_VARS(unit);

    /**
     * 1. Initialize CLU DB
     */
    ethu_id = imb_info->inst_id;
    clu_id = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type_index;

    user_acc = sal_alloc(nof_pms_in_clu * sizeof(portmod_default_user_access_t), "CLU user access");
    SHR_NULL_CHECK(user_acc, _SHR_E_MEMORY, "user_access");

    for (i = 0; i < nof_pms_in_clu; i++)
    {
        pm_index = clu_id * nof_pms_in_clu + i;
        SHR_IF_ERR_EXIT(portmod_default_user_access_t_init(unit, &user_acc[i]));
        PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((&user_acc[i]));
        user_acc[i].unit = unit;
        user_acc[i].blk_id = dnx_drv_clport_block(unit, pm_index);
        user_acc[i].mutex = sal_mutex_create("pm mutex");
        SHR_NULL_CHECK(user_acc[i].mutex, _SHR_E_MEMORY, "user_access->mutex");
    }

    imb_specific_info->clu.user_acc = user_acc;
    /**
     * 2. initialize register values
     */
    if (!dnx_drv_soc_warm_boot(unit))
    {
        nof_rmcs = dnx_data_nif.eth.ethu_logical_fifo_nof_get(unit);

        /*
         * Disable data to Portmacro
         */
        SHR_IF_ERR_EXIT(imb_ethu_tx_data_disable_traffic(unit, core, ethu_id));

        if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_clock_power_down))
        {
            /** Power down the unit */
            SHR_IF_ERR_EXIT(imb_ethu_power_down_set(unit, core, ethu_id, TRUE));
        }

        /*
         * Clear all Schedulers
         */
        SHR_IF_ERR_EXIT(imb_ethu_scheduler_config_set(unit, core, ethu_id, bcmPortNifSchedulerLow, 0));
        SHR_IF_ERR_EXIT(imb_ethu_scheduler_config_set(unit, core, ethu_id, bcmPortNifSchedulerHigh, 0));
        if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
        {
            SHR_IF_ERR_EXIT(imb_ethu_scheduler_config_set(unit, core, ethu_id, bcmPortNifSchedulerTDM, 0));
        }

        if (dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_rmc_flush_support))
        {
            /*
             * Set Scheduler cnt decrement threshold
             */
            SHR_IF_ERR_EXIT(imb_ethu_sch_cnt_dec_thres_hw_set(unit, core, ethu_id, 0));
        }
        /*
         * Set all RMCs threshold after overflow to 0. set All RMCs Flush/stop bit
         */
        for (rmc = 0; rmc < nof_rmcs; ++rmc)
        {
            SHR_IF_ERR_EXIT(imb_ethu_port_rmc_thr_after_ovf_hw_set(unit, core, ethu_id, rmc, 0));
            if (dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_rmc_flush_support))
            {
                SHR_IF_ERR_EXIT(imb_ethu_port_rx_rmc_flush_mode_enable_hw_set(unit, core, ethu_id, rmc, 0));
            }
        }

        /*
         * Set PRD bypass mode to 0.
         * This is a debug feature to bypass the PRD parser, and it is turned on by default.
         * it should not be exposed as API.
         * this will enable working with the PRD later on.
         */
        SHR_IF_ERR_EXIT(imb_ethu_prd_bypass_hw_set(unit, core, ethu_id, 0));
        /*
         * Disable masking of TDM indication from parser. CLU/CDU has a feature to mask TDM indication received from PP
         * parser. This mechanism was added when PP team was not sure they would complete the work required for TDM
         * indication. Since the work was eventually completed, it should be disabled. Set
         * RX_PORT_MAP_MASK_PARSER_TDM_BITf in CLU_RX_PORT_MAP_MASK_PARSER_TDM to '0'.
         */
        if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
        {
            SHR_IF_ERR_EXIT(imb_ethu_tdm_mask_hw_set(unit, core, ethu_id, 0));
        }
    }

    /**
     * initialize portmod_pm_create_info_t
     */
    for (pm_i = 0; pm_i < nof_pms_in_clu; pm_i++)
    {
        SHR_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
        SHR_IF_ERR_EXIT(imb_clu_port_macro_config(unit, pm_i, imb_info, &pm_info));

        /*
         * Initialize specific pm4x25 info - after pm4x25 is implemented
         */
        SHR_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));
    }

    /*
     * Power up PMs and set reference clock source according to master/slave mode
     */
    if (!dnx_drv_soc_warm_boot(unit))
    {
        for (pm_i = 0; pm_i < nof_pms_in_clu; pm_i++)
        {
            SHR_IF_ERR_EXIT(imb_clu_rx_pm_reset_hw_set(unit, clu_id, pm_i, IMB_COMMON_OUT_OF_RESET));
            SHR_IF_ERR_EXIT(imb_clu_pm_tsc_refclock_set(unit, pm_i, imb_info));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - De-initialize the CLU. main task is to free
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
imb_clu_deinit(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    int i;
    uint32 nof_pms_in_clu = dnx_data_nif.eth.nof_pms_in_clu_get(unit);
    portmod_default_user_access_t *user_acc;
    SHR_FUNC_INIT_VARS(unit);

    user_acc = imb_specific_info->clu.user_acc;

    if (user_acc != NULL)
    {
        for (i = 0; i < nof_pms_in_clu; i++)
        {
            if (user_acc[i].mutex != NULL)
            {
                sal_mutex_destroy(user_acc[i].mutex);
            }
        }

        sal_free(user_acc);
        imb_specific_info->clu.user_acc = NULL;
    }

    SHR_FUNC_EXIT;
}

static int
imb_clu_port_nof_lanes_config_get(
    int unit,
    int port_num_lanes,
    _shr_port_if_t * port_interface,
    int *default_speed,
    int *max_speed)
{

    SHR_FUNC_INIT_VARS(unit);

    switch (port_num_lanes)
    {
        case 1:
        {
            *port_interface = BCM_PORT_IF_KR;
            *default_speed = 10000;
            *max_speed = 25000;
            break;
        }
        case 2:
        {
            *port_interface = BCM_PORT_IF_KR2;
            *default_speed = 40000;
            *max_speed = 50000;
            break;
        }
        case 4:
        {
            *port_interface = BCM_PORT_IF_KR4;
            *default_speed = 40000;
            *max_speed = 100000;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(BCM_E_NOT_FOUND, "unknown number of lanes %d \n", port_num_lanes);
        }
    }
exit:
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
imb_clu_portmod_add_info_config(
    int unit,
    bcm_port_t port,
    portmod_port_add_info_t * add_info)
{
    int i;
    int pm_lower_bound, pm_upper_bound;
    int nof_lanes_in_pm;
    _shr_port_if_t port_interface;
    int default_speed, max_speed;
    soc_dnxc_lane_map_db_map_t *lane2serdes = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_add_info_t_init(unit, add_info));

    nof_lanes_in_pm = dnx_data_nif.eth.nof_clu_lanes_in_pm_get(unit);

    lane2serdes = sal_alloc(nof_lanes_in_pm * sizeof(soc_dnxc_lane_map_db_map_t), "lane2serdes");

    /*
     * Configure lane map info. Get the lane map info from swstate
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_imb_ethu_port_pm_boundary_get(unit, port, &pm_lower_bound, &pm_upper_bound));
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_pm_lane_to_serdes_map_get
                    (unit, DNX_ALGO_LANE_MAP_NIF_SIDE, pm_lower_bound, pm_upper_bound, lane2serdes));

    for (i = 0; i < nof_lanes_in_pm; i++)
    {
        add_info->init_config.lane_map[0].lane_map_rx[i] = lane2serdes[i].rx_id;
        add_info->init_config.lane_map[0].lane_map_tx[i] = lane2serdes[i].tx_id;
    }

    add_info->init_config.lane_map[0].num_of_lanes = nof_lanes_in_pm;
    add_info->init_config.lane_map_overwrite = 1; /** we always overwrite lane map*/
    add_info->init_config.polarity_overwrite = 0; /** same as in pm create, we don't need to overwrite*/

    /** get port phys */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, (bcm_pbmp_t *) & (add_info->phys)));
    /*
     * Configuraiton Flags
 */
    PORTMOD_PORT_ADD_F_AUTONEG_CONFIG_SKIP_SET(add_info);
    PORTMOD_PORT_ADD_F_RX_SRIP_CRC_SET(add_info);

    /*
     * run 1G at 25 VCO
 */
    add_info->init_config.serdes_1000x_at_25g_vco = 1;
    /*
     * run 10G at 25 VCO
 */
    add_info->init_config.serdes_10g_at_25g_vco = 1;

    add_info->init_config.pll_divider_req = 165;
    add_info->interface_config.pll_divider_req = 165;

    /** enable cl72 for AN  */
    add_info->init_config.an_cl72 = 1;

    /*
     * Disable link training
     */
    add_info->link_training_en = 0;

    /*
     * get port PHYs
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &add_info->interface_config.port_num_lanes));

    /*
     * Get the configuration values according to the nof_lanes
     */
    SHR_IF_ERR_EXIT(imb_clu_port_nof_lanes_config_get
                    (unit, add_info->interface_config.port_num_lanes, &port_interface, &default_speed, &max_speed));
    /*
     * Set interface operating mode
 */
    add_info->interface_config.interface = port_interface;
    /*
     * Interface rate.
     * This sets the default and will be overridden by resource API settings.
 */
    add_info->interface_config.speed = default_speed;
    /*
     * max speed is defined by number of lanes
 */
    add_info->interface_config.max_speed = max_speed;

    /*
     * Flags Config
     */
    add_info->interface_config.flags |= PHYMOD_INTF_F_DONT_TURN_OFF_PLL;

    /*
     *  Enable/Disable firmware load CRC verification
     */
    if (dnx_data_port.static_add.nif_fw_crc_check_get(unit))
    {
        PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_SET(add_info);
    }
    else
    {
        PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_CLR(add_info);
    }

exit:
    sal_free(lane2serdes);
    SHR_FUNC_EXIT;
}

static int
imb_clu_rx_pm_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    SHR_IF_ERR_EXIT(imb_clu_rx_pm_enable_hw_set(unit, ethu_info.imb_type_id, ethu_info.pm_id_in_ethu, enable));

exit:
    SHR_FUNC_EXIT;
}

static int
imb_clu_rx_pm_port_mode_set(
    int unit,
    bcm_port_t port,
    int mode,
    int enable)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    mode = (enable) ? mode : DBAL_PREDEF_VAL_DEFAULT_VALUE;
    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    SHR_IF_ERR_EXIT(imb_clu_rx_pm_port_mode_hw_set(unit, ethu_info.imb_type_id, ethu_info.pm_id_in_ethu, mode));

exit:
    SHR_FUNC_EXIT;
}

static int
imb_clu_rx_pm_aligner_reset_set(
    int unit,
    bcm_port_t port,
    int reset)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    SHR_IF_ERR_EXIT(imb_clu_rx_pm_aligner_reset_hw_set(unit, ethu_info.imb_type_id, ethu_info.pm_id_in_ethu, reset));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the appropriate Port operating mode from all
 *        enabled ports on the MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] port_mode - Port operating mode
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
imb_clu_rx_pm_port_mode_calc(
    int unit,
    bcm_port_t port,
    int *port_mode)
{
    int nof_phys, port_i_nof_phys, check_3_port_mode, nof_phys_for_3_ports_check;
    bcm_port_t port_i;
    bcm_pbmp_t ethu_ports;
    dnx_algo_port_ethu_access_info_t ethu_info, port_i_clu_info;
    SHR_FUNC_INIT_VARS(unit);

    check_3_port_mode = 0;

    /*
     * get port nof phys
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &nof_phys));

    switch (nof_phys)
    {
        case 4:
            *port_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_ONE_PORT;
            break;
        case 2:
            /*
             * set the MAC mode to 2 ports mode, and mark to check for 3 ports mode
             */
            *port_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_TWO_PORTS;
            check_3_port_mode = 1;
            /*
             * 3 port mode means on the same quad we have ports with 2 lanes ad ports with single lane
             */
            nof_phys_for_3_ports_check = 1;
            break;
        case 1:
            /*
             * set the MAC mode to 4 ports mode, and mark to check for 3 ports mode
             */
            *port_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_FOUR_PORTS;
            check_3_port_mode = 1;
            /*
             * 3 port mode means on the same quad we have ports with 2 lanes ad ports with single lane
             */
            nof_phys_for_3_ports_check = 2;
            break;
        default:
            SHR_ERR_EXIT(BCM_E_INTERNAL, "Invalid number of phys %d for port %d \n", nof_phys, port);
    }

    if (check_3_port_mode)
    {
        /**check if we are in 3 ports mode*/
        /*
         * get port info
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
        SHR_IF_ERR_EXIT(imb_ethu_pm_master_ports_get(unit, port, &ethu_ports));
        /*
         * iterate over all ports on the MAC to see if we are in three ports mode
         */
        BCM_PBMP_PORT_REMOVE(ethu_ports, port);
        BCM_PBMP_ITER(ethu_ports, port_i)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port_i, &port_i_nof_phys));
            /*
             * check if port_i number of phys matches 3 ports mode
             */
            if (port_i_nof_phys == nof_phys_for_3_ports_check)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port_i, &port_i_clu_info));

                if (((nof_phys > port_i_nof_phys)
                     && (ethu_info.first_lane_in_port > port_i_clu_info.first_lane_in_port))
                    || ((nof_phys < port_i_nof_phys)
                        && (ethu_info.first_lane_in_port < port_i_clu_info.first_lane_in_port)))
                {
                    *port_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_1_2;
                    break;
                }
                else
                {
                    *port_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_THREE_PORTS_0_2_3;
                    break;
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update the PM port mode for both ADD and REMOVE sequences
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
static int
imb_clu_port_mode_update(
    int unit,
    bcm_port_t port)
{
    int nof_port_lanes, first_lane_local = 0, current_mode = portmodPortModeCount, new_port_mode = portmodPortModeCount;
    bcm_pbmp_t phys;
    portmod_port_mode_info_t port_mode_info;
    dnx_algo_port_ethu_access_info_t ethu_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_mode_get(unit, port, &port_mode_info));
    current_mode = port_mode_info.cur_mode;

    /*
     * Get port add/remove info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    first_lane_local = ethu_info.first_lane_in_port % dnx_data_nif.eth.nof_clu_lanes_in_pm_get(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
    BCM_PBMP_COUNT(phys, nof_port_lanes);

    switch (nof_port_lanes)
    {
        case 4:
            new_port_mode = portmodPortModeSingle;
            break;
        case 2:
            switch (current_mode)
            {
                case portmodPortModeQuad:
                    if (first_lane_local == 0)
                    {
                        new_port_mode = portmodPortModeTri023;
                    }
                    else
                    {
                        new_port_mode = portmodPortModeTri012;
                    }
                    break;
                case portmodPortModeTri012:
                    if (first_lane_local == 0)
                    {
                        new_port_mode = portmodPortModeDual;
                    }
                    else
                    {
                        new_port_mode = portmodPortModeTri012;
                    }
                    break;
                case portmodPortModeTri023:
                    if (first_lane_local == 0)
                    {
                        new_port_mode = portmodPortModeTri023;
                    }
                    else
                    {
                        new_port_mode = portmodPortModeDual;
                    }
                    break;
                case portmodPortModeDual:
                    new_port_mode = portmodPortModeDual;
                    break;
                case portmodPortModeSingle:
                    new_port_mode = portmodPortModeDual;
                    break;
                default:
                    SHR_ERR_EXIT(BCM_E_INTERNAL, "Invalid mode %d for port %d \n", current_mode, port);
            }
            break;
        case 1:
            switch (current_mode)
            {
                case portmodPortModeQuad:
                    new_port_mode = portmodPortModeQuad;
                    break;
                case portmodPortModeTri012:
                    if ((first_lane_local == 0) || (first_lane_local == 1))
                    {
                        new_port_mode = portmodPortModeTri012;
                    }
                    else
                    {
                        new_port_mode = portmodPortModeQuad;
                    }
                    break;
                case portmodPortModeTri023:
                    if ((first_lane_local == 0) || (first_lane_local == 1))
                    {
                        new_port_mode = portmodPortModeQuad;
                    }
                    else
                    {
                        new_port_mode = portmodPortModeTri023;
                    }
                    break;
                case portmodPortModeDual:
                    if ((first_lane_local == 0) || (first_lane_local == 1))
                    {
                        new_port_mode = portmodPortModeTri012;
                    }
                    else
                    {
                        new_port_mode = portmodPortModeTri023;
                    }
                    break;
                case portmodPortModeSingle:
                    new_port_mode = portmodPortModeQuad;
                    break;
                default:
                    SHR_ERR_EXIT(BCM_E_INTERNAL, "Invalid mode %d for port %d \n", current_mode, port);
            }
            break;
        default:
            SHR_ERR_EXIT(BCM_E_INTERNAL, "Invalid number nof ports lanes %d for port %d \n", nof_port_lanes, port);
    }

    port_mode_info.cur_mode = new_port_mode;
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    SHR_IF_ERR_EXIT(portmod_port_mode_set(unit, port, &port_mode_info));
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add new port to the CLU. config all CLU settings and
 *         call Portmod API for PM configurations
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port to be attched to the CLU
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
imb_clu_port_attach(
    int unit,
    bcm_port_t port,
    uint32 flags)
{

    portmod_port_add_info_t add_info;
    dnx_algo_port_ethu_access_info_t ethu_info;
    int port_mode = DBAL_ENUM_FVAL_NIF_MAC_MODE_ONE_PORT;
    int is_master_port;
    int average_ipg_in_bits;
    portmod_pause_control_t pause_control;
    portmod_pfc_control_t pfc_control;
    portmod_rx_control_t rx_ctrl;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, port, &is_master_port));

    /*
     * get port ethu info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    if (is_master_port)
    {

        if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_clock_power_down))
        {
            /** Power up the unit */
            SHR_IF_ERR_EXIT(imb_ethu_port_power_down_set(unit, port, FALSE));
        }

        /*
         *  Enable TX data to PM
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_tx_data_to_pm_enable_set(unit, port, 1));

        /*
         *  The following configurations should be done per PM
 */
        /*
         *  Set CLU port mode - Enable CLU
         */

        if (dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_is_rx_port_mode_needs_configuration))
        {
            SHR_IF_ERR_EXIT(imb_clu_rx_pm_enable_set(unit, port, TRUE /* Enable */ ));
        }

        /*
         * Set the port mode to the Portmacro.
         */
        SHR_IF_ERR_EXIT(imb_clu_rx_pm_port_mode_calc(unit, port, &port_mode));

        SHR_IF_ERR_EXIT(imb_clu_rx_pm_port_mode_set(unit, port, port_mode, TRUE /* Enable */ ));
        /*
         * Take the correct aligner unit out of reset
         */
        SHR_IF_ERR_EXIT(imb_clu_rx_pm_aligner_reset_set(unit, port, IMB_COMMON_OUT_OF_RESET));
    }

    

    /*
     * Call Portmod Port Add
 */
    SHR_IF_ERR_EXIT(imb_portmod_add_info_config(unit, port, &add_info));
    SHR_IF_ERR_EXIT(imb_er_portmod_port_add(unit, port, &add_info));

    if (is_master_port)
    {
        DNX_ERR_RECOVERY_SUPPRESS(unit);
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, 0));
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);

        /*
         * Set the PM mode each time in case it needs to be updated
         */
        SHR_IF_ERR_EXIT(imb_clu_port_mode_update(unit, port));

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
        /*
         * PFC - to enable generation of PFC set refresh timer
         */
        SHR_IF_ERR_EXIT(portmod_pfc_control_t_init(unit, &pfc_control));
        pfc_control.refresh_timer = 0xc000;
        DNX_ERR_RECOVERY_SUPPRESS(unit);
        SHR_IF_ERR_EXIT(portmod_port_pfc_control_set(unit, port, &pfc_control));
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);
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
            DNX_ERR_RECOVERY_SUPPRESS(unit);
            SHR_IF_ERR_EXIT(portmod_port_tx_average_ipg_set(unit, port, average_ipg_in_bits));
            DNX_ERR_RECOVERY_UNSUPPRESS(unit);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove a port from the CLU. first calls Portmos API
 *        to remove from portmod DB, then configures all CLU
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
imb_clu_port_detach(
    int unit,
    bcm_port_t port)
{
    int clu_nof_eth_ports = 0, nof_channels = 0;
    dnx_algo_port_ethu_access_info_t ethu_info;
    bcm_pbmp_t ethu_ports;
    int port_count;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * get port ethu info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    /*
     * Set the PM mode each time in case it needs to be updated
     */
    SHR_IF_ERR_EXIT(imb_clu_port_mode_update(unit, port));

    /*
     *  Call Portmod API
     */
    SHR_IF_ERR_EXIT(imb_er_portmod_port_remove(unit, port));
    /*
     * get number of eth ports on CDU before disabling anything common to all ports in CDU
     */
    SHR_IF_ERR_EXIT(imb_ethu_pm_nof_master_ports_get(unit, port, &clu_nof_eth_ports));

    /*
     * get number of channels
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, port, &nof_channels));

    if ((clu_nof_eth_ports == 1) && (nof_channels == 1))
    {
        /*
         *  Reset Async unit
         */
        SHR_IF_ERR_EXIT(imb_clu_rx_pm_aligner_reset_set(unit, port, IMB_COMMON_IN_RESET));
        /*
         *  Disable CLU port mode
         */
        SHR_IF_ERR_EXIT(imb_clu_rx_pm_port_mode_set(unit, port, 0, FALSE /* Disable */ ));
        /*
         *  Disable PM
         */
        if (dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_is_rx_port_mode_needs_configuration))
        {
            SHR_IF_ERR_EXIT(imb_clu_rx_pm_enable_set(unit, port, FALSE /* Disable */ ));
        }
        
    }

    if (nof_channels == 1)
    {
        /*
         *  Disable TX data to PM
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_tx_data_to_pm_enable_set(unit, port, FALSE /* Disable */ ));

        /*
         * Unmap RMCs
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_remove_rmc_lane_unmap(unit, port));

        /*
         * stop the egr flush
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_tx_egress_flush_set(unit, port, 0));
    }

    if ((clu_nof_eth_ports == 1) && (nof_channels == 1))
    {
        /*
         *  Disable CLU port mode
         */
        SHR_IF_ERR_EXIT(imb_clu_rx_pm_port_mode_set(unit, port, 0, FALSE /* Disable */ ));
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
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static int
imb_clu_port_resource_default_get_verify(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_port_resource_t * resource)
{
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
imb_clu_port_resource_default_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_port_resource_t * resource)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(imb_clu_port_resource_default_get_verify(unit, port, flags, resource));

    /*
     * in case user didn't configure the FEC already - give default value
     */
    if (resource->fec_type == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->fec_type = bcmPortPhyFecNone;
    }

    /*
     * in case user didn't configure CL72 - by default it is enabled
     */
    if (resource->link_training == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->link_training = 1;
    }

    /*
     * set default lane_config
     */
    if (resource->phy_lane_config == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        /** clear all the fields in lane config */
        resource->phy_lane_config = 0;

        /** enable DFE */
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(resource->phy_lane_config);

        /** set media type to backplane */
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(resource->phy_lane_config,
                                                     BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_BACKPLANE);

        /** enable CL72 restart timeout */
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_SET(resource->phy_lane_config);
    }

    resource->port = port;

exit:
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
imb_clu_port_tx_start_thr_set(
    int unit,
    bcm_port_t port,
    int speed)
{
    int start_thr = -1;
    int idx;
    dnx_algo_port_ethu_access_info_t clu_info;
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
                    DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info) ?
                    dnx_data_nif.eth.start_tx_threshold_table_get(unit, idx)->start_thr_for_l1
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

    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &clu_info));
    SHR_IF_ERR_EXIT(imb_clu_tx_start_thr_hw_set(unit, clu_info.imb_type_id, clu_info.first_lane_in_port, start_thr));

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
imb_clu_prd_threshold_set(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 threshold)
{
    dnx_algo_port_rmc_info_t rmc;
    uint32 rmc_fifo_size;
    dnx_algo_port_ethu_access_info_t clu_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &clu_info));

    SHR_IF_ERR_EXIT(dnx_cosq_prd_threshold_rmc_get(unit, port, priority, &rmc));

    rmc_fifo_size = rmc.last_entry - rmc.first_entry + 1;

    if (threshold > rmc_fifo_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Threshold %d for priority %d of port %d is higher than the fifo size %d\n",
                     threshold, priority, port, rmc_fifo_size);
    }
    SHR_IF_ERR_EXIT(imb_clu_prd_threshold_hw_set(unit, clu_info.imb_type_id, rmc.rmc_id, priority, threshold));
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
imb_clu_prd_threshold_get(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 *threshold)
{
    dnx_algo_port_rmc_info_t rmc;
    dnx_algo_port_ethu_access_info_t clu_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &clu_info));

    SHR_IF_ERR_EXIT(dnx_cosq_prd_threshold_rmc_get(unit, port, priority, &rmc));

    SHR_IF_ERR_EXIT(imb_clu_prd_threshold_hw_get(unit, clu_info.imb_type_id, rmc.rmc_id, priority, threshold));
exit:
    SHR_FUNC_EXIT;

}
int
imb_clu_prd_threshold_max_get(
    int unit,
    bcm_port_t port,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    *threshold = dnx_data_nif.prd.rmc_threshold_max_get(unit);

    SHR_FUNC_EXIT;
}

int
imb_clu_prd_port_profile_map_set(
    int unit,
    bcm_port_t port,
    uint32 prd_profile)
{
    dnx_algo_port_ethu_access_info_t clu_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &clu_info));

    /*
     * Save the mapping in sw state
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.prd.port_to_profile_map.set(unit, port, prd_profile));

    /*
     * Set the mapping in hw
     */
    SHR_IF_ERR_EXIT(imb_clu_rx_prd_port_profile_map_hw_set
                    (unit, clu_info.imb_type_id, clu_info.first_lane_in_port, prd_profile));
exit:
    SHR_FUNC_EXIT;
}

int
imb_clu_prd_port_profile_map_get(
    int unit,
    bcm_port_t port,
    uint32 *prd_profile_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.prd.port_to_profile_map.get(unit, port, prd_profile_p));

exit:
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
imb_clu_port_phy_control_validate(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * We need to filter phy controls based on phy types. For this to happen we are using
     * PM dispatch types. In this case here we are interested to filter some phy controls that are
     * not valid for portmodDispatchTypePm4x25.
     */
    switch (type)
    {
            /*
             * Sepcial handle for PRBs function, as port control PRBs function needs to
             * call the soc layer PRBs function to deal with Per Lane Gport.
             * Therefore we must ensure the PRBs is supported by soc layer common API.
             */
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
        case BCM_PORT_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE:
        case BCM_PORT_PHY_CONTROL_LP_TX_PRECODER_ENABLE:
        case BCM_PORT_PHY_CONTROL_RX_HIGH_FREQ_PEAK_FILTER:
        case BCM_PORT_PHY_CONTROL_RX_TAP6:
        case BCM_PORT_PHY_CONTROL_RX_ADAPTATION_RESUME:
        case BCM_PORT_PHY_CONTROL_FEC_CORRUPTION_PERIOD:
        case BCM_PORT_PHY_CONTROL_FLEXE_50G_NOFEC_20K_AM_SPACING_ENABLE:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "PHY control type %d is not supported on Falcon PHY.\r\n", type);
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
imb_clu_port_autoneg_ability_verify(
    int unit,
    bcm_port_t port,
    int num_ability,
    const bcm_port_speed_ability_t * abilities)
{
    int is_ability_valid = 0;
    bcm_port_autoneg_mode_t pre_an_mode = bcmPortAnModeCount;
    uint32 pre_resolved_num_lanes = 0;
    bcm_port_phy_fec_t pre_fec_type = bcmPortPhyFecInvalid;
    bcm_port_medium_t pre_medium = BCM_PORT_MEDIUM_NONE;
    bcm_port_phy_channel_t pre_channel = bcmPortPhyChannelCount;
    bcm_port_phy_pause_t pre_pause = bcmPortPhyPauseCount;
    int nof_lanes;
    int i, ii;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &nof_lanes));

    for (i = 0; i < num_ability; ++i)
    {
        /** Only one AN mode */
        /** Only one medium type */
        /** Only one pause type */
        /** Only one channel type */
        /** FEC should be either enabled or disabled among given abilities */
        if (i > 0)
        {
            if ((pre_an_mode != abilities[i].an_mode) || (pre_channel != abilities[i].channel)
                || (pre_medium != abilities[i].medium) || (pre_pause != abilities[i].pause)
                || (pre_resolved_num_lanes != abilities[i].resolved_num_lanes)
                || ((pre_fec_type != abilities[i].fec_type) && (pre_fec_type == bcmPortPhyFecNone)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "All autoneg abilities adverted should have the same AN mode, FEC enable status, media type, pause type and channel type.\r\n");
            }
        }
        else
        {
            if (abilities[i].pause > bcmPortPhyPauseSymm)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid pause type %d.\r\n", abilities[i].pause);
            }
            if (abilities[i].resolved_num_lanes != nof_lanes)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "resolved_num_lanes should equals to port's number of lanes.\r\n");
            }
            pre_an_mode = abilities[i].an_mode;
            pre_fec_type = abilities[i].fec_type;
            pre_medium = abilities[i].medium;
            pre_pause = abilities[i].pause;
            pre_channel = abilities[i].channel;
            pre_resolved_num_lanes = abilities[i].resolved_num_lanes;
        }

        /** each of the abilities should be member of the AN ability table */
        is_ability_valid = 0;
        for (ii = 0; ii < dnx_data_nif.eth.supported_an_abilities_info_get(unit)->key_size[0]; ++ii)
        {
            if (dnx_data_nif.eth.supported_an_abilities_get(unit, ii)->is_valid)
            {
                if (abilities[i].an_mode == dnx_data_nif.eth.supported_an_abilities_get(unit, ii)->an_mode)
                {
                    if (abilities[i].speed == dnx_data_nif.eth.supported_an_abilities_get(unit, ii)->speed)
                    {
                        if (abilities[i].resolved_num_lanes ==
                            dnx_data_nif.eth.supported_an_abilities_get(unit, ii)->nof_lanes)
                        {
                            if (abilities[i].fec_type ==
                                dnx_data_nif.eth.supported_an_abilities_get(unit, ii)->fec_type)
                            {
                                if (abilities[i].medium ==
                                    dnx_data_nif.eth.supported_an_abilities_get(unit, ii)->medium)
                                {
                                    if ((abilities[i].channel < bcmPortPhyChannelAll) || (abilities[i].speed != 25000))
                                    {
                                        is_ability_valid = 1;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (!is_ability_valid)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The autoneg ability[%d] is not supported by CLU.\r\n", i);
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Set port advertise abilities
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] num_ability     - number of ability the port will advertise
 * \param [in] abilities       - local advertisement for each ability
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
imb_clu_port_autoneg_ability_advert_set(
    int unit,
    bcm_port_t port,
    int num_ability,
    const bcm_port_speed_ability_t * abilities)
{
    portmod_port_ability_t *an_abilities = NULL;
    uint32_t port_dynamic_state = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(an_abilities, sizeof(portmod_port_ability_t), "port an abilities", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    sal_memset(an_abilities, 0, sizeof(portmod_port_ability_t));

    imb_portmod_legacy_an_ability_from_bcm_an_ability_get(num_ability, abilities, an_abilities);

    SHR_IF_ERR_EXIT(portmod_port_ability_advert_set(unit, port, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, an_abilities));

    port_dynamic_state = abilities[0].an_mode << 16;
    PORTMOD_PORT_AUTONEG_MODE_UPDATED_SET(port_dynamic_state);
    SHR_IF_ERR_EXIT(portmod_port_update_dynamic_state(unit, port, port_dynamic_state));

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
imb_clu_port_autoneg_ability_advert_get(
    int unit,
    bcm_port_t port,
    int max_num_ability,
    bcm_port_speed_ability_t * abilities,
    int *actual_num_ability)
{
    portmod_port_ability_t *an_abilities = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(an_abilities, sizeof(portmod_port_ability_t), "port an abilities", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    sal_memset(an_abilities, 0, sizeof(portmod_port_ability_t));

    SHR_IF_ERR_EXIT(portmod_port_ability_advert_get(unit, port, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, an_abilities));

    SHR_IF_ERR_EXIT(imb_bcm_an_ability_from_portmod_legacy_an_ability_get(unit, port,
                                                                          max_num_ability, an_abilities,
                                                                          actual_num_ability, abilities));

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
imb_clu_port_speed_ability_local_get(
    int unit,
    bcm_port_t port,
    int max_num_ability,
    bcm_port_speed_ability_t * abilities,
    int *actual_num_ability)
{
    portmod_port_ability_t *an_abilities = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(an_abilities, sizeof(portmod_port_ability_t), "port an abilities", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    sal_memset(an_abilities, 0, sizeof(portmod_port_ability_t));

    SHR_IF_ERR_EXIT(portmod_port_ability_local_get(unit, port, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, an_abilities));

    SHR_IF_ERR_EXIT(imb_bcm_local_an_ability_from_portmod_legacy_an_ability_get(unit, port,
                                                                                max_num_ability, an_abilities,
                                                                                actual_num_ability, abilities));

exit:
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
imb_clu_port_autoneg_ability_remote_get(
    int unit,
    bcm_port_t port,
    int max_num_ability,
    bcm_port_speed_ability_t * abilities,
    int *actual_num_ability)
{
    portmod_port_ability_t *an_abilities = NULL;
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
        SHR_ALLOC(an_abilities, sizeof(portmod_port_ability_t), "port an abilities", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        sal_memset(an_abilities, 0, sizeof(portmod_port_ability_t));

        SHR_IF_ERR_EXIT(portmod_port_ability_remote_get(unit, port, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, an_abilities));

        SHR_IF_ERR_EXIT(imb_bcm_an_ability_from_portmod_legacy_an_ability_get(unit, port,
                                                                              max_num_ability, an_abilities,
                                                                              actual_num_ability, abilities));
    }

exit:
    SHR_FREE(an_abilities);
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
imb_clu_port_eee_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    portmod_eee_t eee;
    bcm_port_t port_i;
    bcm_pbmp_t ethu_ports;
    uint32 nof_ports_enabled = 0;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    SHR_IF_ERR_EXIT(imb_ethu_pm_master_ports_get(unit, port, &ethu_ports));
    /*
     * iterate over all ports on the PM to see how many ports in PM eee enabled
     */
    BCM_PBMP_ITER(ethu_ports, port_i)
    {
        SHR_IF_ERR_EXIT(portmod_port_eee_get(unit, port_i, &eee));
        if (eee.enable)
        {
            ++nof_ports_enabled;
        }
    }
    SHR_IF_ERR_EXIT(portmod_port_eee_get(unit, port, &eee));
    if (enable && (0 == nof_ports_enabled))
    {
        SHR_IF_ERR_EXIT(imb_clu_eee_enable_set(unit, ethu_info.imb_type_id, ethu_info.pm_id_in_ethu, enable));
    }
    else if (!enable && (1 == nof_ports_enabled) && !!eee.enable)
    {
        SHR_IF_ERR_EXIT(imb_clu_eee_enable_set(unit, ethu_info.imb_type_id, ethu_info.pm_id_in_ethu, enable));
    }
    else
    {
        /** do nothing */
    }
    /*
     * enable EEE in Portmod
     */
    eee.enable = !!enable;
    SHR_IF_ERR_EXIT(portmod_port_eee_set(unit, port, &eee));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get EEE enable indication
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
imb_clu_port_eee_enable_get(
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
exit:
    SHR_FUNC_EXIT;
}
