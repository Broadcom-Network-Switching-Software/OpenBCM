/**
 * \file nif_tune.c $Id$ NIF Tuning procedures for DNX. this 
 *       file is meant for internal functions related to tuning.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <shared/bslenum.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx_dispatch.h>

/*
 * }
 */

/**
 * \brief - set the default priority configuration for every new 
 *        port. this configuration can be later overriden by
 *        bcm_port_prioroity_config_set API.
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
dnx_port_tune_port_add_priority_config(
    int unit,
    bcm_port_t port)
{
    bcm_port_prio_config_t priority_config;
    dnx_algo_port_info_s port_info;
    int is_master_port;
    dnx_algo_port_if_tdm_mode_e if_tdm_mode;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, port, &is_master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, FALSE) && is_master_port &&
        DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_if_tdm_mode_get(unit, port, &if_tdm_mode));

        switch (if_tdm_mode)
        {
            case DNX_ALGO_PORT_IF_NO_TDM:
            {
                /*
                 * by default, there should be only 1 priority group in the Low scheduler with all PRD priorities
                 */
                priority_config.nof_priority_groups = 1;
                priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerLow;
                priority_config.priority_groups[0].source_priority = IMB_PRD_PRIORITY_ALL;
                /*
                 * take all entries to a single RMC (max fifo size)
                 */
                priority_config.priority_groups[0].num_of_entries = -1;
                break;
            }
            case DNX_ALGO_PORT_IF_TDM_ONLY:
            {
                /*
                 * by default, there should be only 1 priority group in the Low scheduler with all PRD priorities
                 */
                priority_config.nof_priority_groups = 1;
                priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerTDM;
                priority_config.priority_groups[0].source_priority = IMB_PRD_PRIORITY_ALL;
                /*
                 * take all entries to a single RMC (max fifo size)
                 */
                priority_config.priority_groups[0].num_of_entries = -1;
                break;
            }
            case DNX_ALGO_PORT_IF_TDM_HYBRID:
            {
                /*
                 * by default, there should be only 1 priority group in the Low scheduler with all PRD priorities
                 */
                priority_config.nof_priority_groups = 2;
                priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerLow;
                priority_config.priority_groups[0].source_priority = IMB_PRD_PRIORITY_ALL & ~BCM_PORT_F_PRIORITY_TDM;
                priority_config.priority_groups[1].sch_priority = bcmPortNifSchedulerTDM;
                priority_config.priority_groups[1].source_priority = BCM_PORT_F_PRIORITY_TDM;
                /*
                 * Equal division (max fifo size)
                 */
                priority_config.priority_groups[0].num_of_entries = -1;
                priority_config.priority_groups[1].num_of_entries = -1;
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected interface tdm mode %d\n", if_tdm_mode);
                break;
            }

        }

        SHR_IF_ERR_EXIT(bcm_dnx_port_priority_config_set(unit, port, &priority_config));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_ETH_L1(unit, port_info) && is_master_port)
    {
        /*
         * L1 ports must set to TDM priority
         */
        priority_config.nof_priority_groups = 1;
        priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerTDM;
        priority_config.priority_groups[0].source_priority = IMB_PRD_PRIORITY_ALL;
        /*
         * take all entries to a single RMC (max fifo size)
         */
        priority_config.priority_groups[0].num_of_entries = -1;
        SHR_IF_ERR_EXIT(bcm_dnx_port_priority_config_set(unit, port, &priority_config));
    }

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, FALSE, TRUE) && is_master_port)
    {
        int master_port;
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
        /*
         * Set ILKN scheduler priority to be Low by default
         */
        if (port == master_port)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_port_control_set(unit, port, bcmPortControlRxPriority, bcmPortNifSchedulerLow));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the default phy control configuration for every new
 *        port. this configuration can be later overriden by
 *        bcm_port_phy_control_set API.
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
dnx_port_tune_port_add_phy_control_config(
    int unit,
    bcm_port_t port)
{
    bcm_port_phy_control_t phy_control;
    uint32 value;
    int first_phy;
    int ethu_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));
    SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, first_phy, &ethu_id));

    if (dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type == imbDispatchTypeImb_clu)
    {
        phy_control = BCM_PORT_PHY_CONTROL_LINK_TRAINING_INIT_TX_FIR_POST;
        value = 25;

        SHR_IF_ERR_EXIT(bcm_dnx_port_phy_control_set(unit, port, phy_control, value));
    }

exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
