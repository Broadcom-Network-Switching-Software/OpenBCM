/** \file imb_ethu.c
 *
 *  Common Ethernet ports procedures for DNX.
 */
/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <include/bcm_int/dnx/port/imb/imb_ethu.h>

#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <soc/drv.h> /** CDPORT_BLOCK */
#include <soc/cmic.h>
#include <soc/util.h>
#include <soc/mcm/memregs.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <soc/portmod/portmod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esb.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/stat/stif/stif_mgmt.h>
#include <bcm_int/dnx/cosq/egress/esb.h>
#include <bcm_int/common/link.h>
#include "imb_utils.h"
#include <soc/sand/sand_signals.h>
#include <bcm_int/dnx_dispatch.h>

#include "imb_ethu_internal.h"
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#ifdef INCLUDE_XFLOW_MACSEC
#include <dnx/dnx_sec.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_macsec.h>
#endif

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/**
 * \brief - NIF ETH ports that have RMCs
 */
#define IMB_ETHU_IS_RX_RMC_PORT(unit, port_info) (DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info) ||                  \
                                                  DNX_ALGO_PORT_TYPE_IS_ETH_L1(unit, port_info))

/**
 * \brief - NIF ETH ports that send credits to EGQ block (ESB queues)
 */
#define IMB_ETHU_IS_TX_CREDIT_PORT(unit, port_info) (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info) ||               \
                                                     DNX_ALGO_PORT_TYPE_IS_ETH_L1(unit, port_info))

static int imb_ethu_port_priority_config_enable_set(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc_arr,
    int nof_rmcs,
    int enable);

/**
 * \brief - Disable traffic for all lanes that belong to the given ETHU :
 *
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] ethu_id - ethu id to be disabled
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
imb_ethu_tx_data_disable_traffic(
    int unit,
    bcm_core_t core,
    int ethu_id)
{
    int first_lane = 0, nof_lanes_in_ethu;
    int first_lane_in_core = 0;
    int lane_idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_ethu_id_lane_info_get(unit, core, ethu_id, &first_lane, &nof_lanes_in_ethu));
    first_lane_in_core = first_lane % dnx_data_nif.phys.nof_phys_per_core_get(unit);
    /*
     * Disable all lanes in the given ETHU
     */
    for (lane_idx = 0; lane_idx < nof_lanes_in_ethu; lane_idx++)
    {
        SHR_IF_ERR_EXIT(imb_ethu_tx_data_to_pm_enable_hw_set(unit, core, first_lane_in_core + lane_idx, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the ETHU in the specified Scheduler
 *
 * see .h file
 */
int
imb_ethu_scheduler_config_set(
    int unit,
    bcm_core_t core,
    uint32 ethu_id,
    bcm_port_nif_scheduler_t sch_prio,
    int weight)
{
    uint32 dbal_sch_prio, dbal_sch_weight;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * convert bcm sch_prio to DBAL sch_prio
     */
    SHR_IF_ERR_EXIT(imb_common_dbal_sch_prio_from_bcm_sch_prio_get(unit, sch_prio, &dbal_sch_prio));
    /*
     * convert to DBAL weight from reguler int (0-4)
     */
    SHR_IF_ERR_EXIT(imb_common_dbal_sch_weight_get(unit, weight, &dbal_sch_weight));

    SHR_IF_ERR_EXIT(imb_ethu_scheduler_config_hw_set(unit, core, ethu_id, dbal_sch_prio, dbal_sch_weight));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the requested weight in the low priority RMC
 *        scheduler. this scheduler is performing RR on all RMCs
 *        with traffic, and each RMC has more than one bit in
 *        the RR. the weight is actually the number of bits in
 *        the RR.
 *
 *
 * see .h file
 */
int
imb_ethu_rmc_low_prio_scheduler_config_set(
    int unit,
    bcm_port_t port,
    int rmc_id,
    int weight)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    uint32 dbal_sch_weight;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    /*
     * convert to DBAL weight from regular int (0-4)
     */
    SHR_IF_ERR_EXIT(imb_common_dbal_sch_weight_get(unit, weight, &dbal_sch_weight));
    /*
 */
    SHR_IF_ERR_EXIT(imb_ethu_rmc_low_prio_scheduler_config_hw_set
                    (unit, ethu_info.core, ethu_info.ethu_id_in_core, rmc_id, dbal_sch_weight));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get port ETHU active indication in the specified
 *        scheduler.
 * see .h file
 */
int
imb_ethu_port_scheduler_active_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_prio,
    uint32 *active)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    uint32 weight;
    uint32 dbal_sch_prio;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    SHR_IF_ERR_EXIT(imb_common_dbal_sch_prio_from_bcm_sch_prio_get(unit, sch_prio, &dbal_sch_prio));
    SHR_IF_ERR_EXIT(imb_ethu_scheduler_config_hw_get
                    (unit, ethu_info.core, ethu_info.ethu_id_in_core, dbal_sch_prio, &weight));

    *active = weight ? 1 : 0;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the logical FIFO threshold after overflow.
 *   see imb_ethu_thr_after_ovf_set
 */
int
imb_ethu_port_thr_after_ovf_set(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    SHR_IF_ERR_EXIT(imb_ethu_port_rmc_thr_after_ovf_hw_set
                    (unit, ethu_info.core, ethu_info.ethu_id_in_core, rmc->rmc_id, rmc->thr_after_ovf));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - map RMC to a specific lane in the ethu.
 * see imb_ethu_rmc_to_lane_map.
 */
int
imb_ethu_port_rmc_to_lane_map(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc,
    int is_map_valid)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    SHR_IF_ERR_EXIT(imb_ethu_rmc_to_lane_map_hw_set
                    (unit, ethu_info.core, ethu_info.ethu_id_in_core, is_map_valid ? ethu_info.first_lane_in_port : 0,
                     rmc));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable / disable egress flush. Egress flush means
 *        Continuously granting credits to the port in order to
 *        free occupied resources.
 *
 * see .h file
 */
int
imb_ethu_port_tx_egress_flush_set(
    int unit,
    bcm_port_t port,
    int flush_enable)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    SHR_IF_ERR_EXIT(imb_ethu_tx_egress_flush_hw_set(unit, ethu_info.core, ethu_info.lane_in_core, flush_enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Flush packets in ETHU RX if flush available and reset the port
 */
static int
imb_ethu_port_rx_flush_and_reset(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    dnx_algo_port_rmc_info_t rmc;
    int nof_priority_groups = dnx_data_nif.eth.priority_groups_nof_get(unit);
    int priority_group;
    uint8 weight_arr_set = FALSE;
    uint32 nof_bits_per_sec;
    uint32 time_usec;
    uint32 dbal_sch_prio;
    uint32 counter;
    uint32 priority_weight[DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF];
    int rmc_empty = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    /*
     * store weight in NIF scheduler per priority in local array
     */
    for (priority_group = 0; priority_group < nof_priority_groups; ++priority_group)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, priority_group, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC id not valid
             */
            continue;
        }

        SHR_IF_ERR_EXIT(imb_common_dbal_sch_prio_from_bcm_sch_prio_get(unit, rmc.sch_priority, &dbal_sch_prio));
        SHR_IF_ERR_EXIT(imb_ethu_scheduler_config_hw_get
                        (unit, ethu_info.core, ethu_info.ethu_id_in_core, dbal_sch_prio,
                         &priority_weight[priority_group]));

    }
    weight_arr_set = TRUE;

    /*
     * Run flush sequence for each of the RMCs
     */
    for (priority_group = 0; priority_group < nof_priority_groups; ++priority_group)
    {
        /*
         * get logical fifo info
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, priority_group, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC id not valid
             */
            continue;
        }

       /**
         * Flush sequence
         */
        /*
         * Disable Data to RMC from PM
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_rx_drop_data_hw_set
                        (unit, ethu_info.core, ethu_info.ethu_id_in_core, rmc.rmc_id, TRUE));

        /*
         * In case SOP entered the fifo, we need to wait until all the packet
         * will enter.
         * Worst case is maximum packet size on a minimum bandwidth port (1Gbps).
         */
        nof_bits_per_sec = dnx_data_nif.eth.packet_size_max_get(unit) * UTILEX_NOF_BITS_IN_CHAR;
        time_usec = UTILEX_DIV_ROUND_UP(nof_bits_per_sec, 1000);
        sal_usleep(time_usec);

        if (dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_rmc_flush_support))
        {
            /*
             * Here we have to check if there are entries in this RMC, if empty Do not Flush!
             */
            SHR_IF_ERR_EXIT(imb_ethu_port_rx_rmc_counter_get
                            (unit, ethu_info.core, ethu_info.ethu_id_in_core, rmc.rmc_id, &counter));

            rmc_empty = (counter == 0 ? TRUE : FALSE);

        }
        else
        {
            /** Check if RMC is empty */
            SHR_IF_ERR_EXIT(imb_ethu_internal_rmc_level_get(unit, port, rmc.rmc_id, &counter));
            rmc_empty = (counter == 0 ? TRUE : FALSE);
        }

        if (!rmc_empty)
        {
            /*
             * Get DBAL sched priority
             */
            SHR_IF_ERR_EXIT(imb_common_dbal_sch_prio_from_bcm_sch_prio_get(unit, rmc.sch_priority, &dbal_sch_prio));

            /*
             * Mask the priority in the NIF scheduler round-robin, to disable RD operation during Flush
             */
            SHR_IF_ERR_EXIT(imb_ethu_scheduler_config_hw_set
                            (unit, ethu_info.core, ethu_info.ethu_id_in_core, dbal_sch_prio,
                             DBAL_ENUM_FVAL_NIF_SCH_WEIGHT_WEIGHT_0));

            if (dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_rmc_flush_support))
            {
                /*
                 * Enable flush contexts in NMG
                 */

                SHR_IF_ERR_EXIT(imb_ethu_port_rx_nmg_flush_context_enable_hw_set
                                (unit, ethu_info.core, ethu_info.ethu_id_in_core, dbal_sch_prio, TRUE));

                /*
                 * Enable Flush mode in NMG
                 */
                SHR_IF_ERR_EXIT(imb_ethu_port_rx_nmg_flush_mode_enable_hw_set(unit, ethu_info.core, TRUE));

                /*
                 * Enable flush mode for RMC
                 */
                SHR_IF_ERR_EXIT(imb_ethu_port_rx_rmc_flush_mode_enable_hw_set
                                (unit, ethu_info.core, ethu_info.ethu_id_in_core, rmc.rmc_id, TRUE));

                /*
                 * Start flushing
                 */
                SHR_IF_ERR_EXIT(imb_ethu_port_rx_flush_enable_hw_set
                                (unit, ethu_info.core, ethu_info.ethu_id_in_core, TRUE));

                /*
                 * Wait until RMC counter is empty
                 */
                SHR_IF_ERR_EXIT(imb_ethu_port_rx_rmc_counter_polling
                                (unit, ethu_info.core, ethu_info.ethu_id_in_core, rmc.rmc_id));
            }
            else
            {
                /*
                 * reset RMC entry in NMG - to zero the RMC data counter
                 */
                SHR_IF_ERR_EXIT(imb_ethu_port_rx_nmg_reset_hw_set(unit, port, rmc.rmc_id, IMB_COMMON_IN_RESET));

            }
        }

        /**
         * Reset the RMC
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_rx_reset_hw_set(unit, port, rmc.rmc_id, IMB_COMMON_IN_RESET));

        if (!rmc_empty)
        {
            if (dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_rmc_flush_support))
            {
                /**
                 * Disable flush sequence
                 */
                /*
                 * Disable flush mode for RMC
                 */
                SHR_IF_ERR_EXIT(imb_ethu_port_rx_rmc_flush_mode_enable_hw_set
                                (unit, ethu_info.core, ethu_info.ethu_id_in_core, rmc.rmc_id, FALSE));

                /*
                 * Stop flushing
                 */
                SHR_IF_ERR_EXIT(imb_ethu_port_rx_flush_enable_hw_set
                                (unit, ethu_info.core, ethu_info.ethu_id_in_core, FALSE));

                /*
                 * Disable Flush mode in NMG
                 */
                SHR_IF_ERR_EXIT(imb_ethu_port_rx_nmg_flush_mode_enable_hw_set(unit, ethu_info.core, FALSE));

                /*
                 * Disable flush contexts in NMG
                 */

                SHR_IF_ERR_EXIT(imb_ethu_port_rx_nmg_flush_context_enable_hw_set
                                (unit, ethu_info.core, ethu_info.ethu_id_in_core, dbal_sch_prio, FALSE));
            }

            /*
             * Restore the priority in the NIF scheduler round-robin
             */
            SHR_IF_ERR_EXIT(imb_ethu_scheduler_config_hw_set
                            (unit, ethu_info.core, ethu_info.ethu_id_in_core, dbal_sch_prio,
                             priority_weight[priority_group]));
        }

        /*
         * Enable Data to RMC from PM
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_rx_drop_data_hw_set
                        (unit, ethu_info.core, ethu_info.ethu_id_in_core, rmc.rmc_id, FALSE));
    }

exit:
    /*
     * Restore weight in NIF scheduler per priority
     */
    if ((SHR_GET_CURRENT_ERR() != BCM_E_NONE) && (weight_arr_set))
    {
        for (priority_group = 0; priority_group < nof_priority_groups; ++priority_group)
        {
            SHR_IF_ERR_RETURN(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, priority_group, &rmc));
            if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
            {
                /*
                 * RMC id not valid
                 */
                continue;
            }

            SHR_IF_ERR_RETURN(imb_common_dbal_sch_prio_from_bcm_sch_prio_get(unit, rmc.sch_priority, &dbal_sch_prio));
            SHR_IF_ERR_RETURN(imb_ethu_scheduler_config_hw_set
                              (unit, ethu_info.core, ethu_info.ethu_id_in_core, dbal_sch_prio,
                               priority_weight[priority_group]));
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Reset the priority group in the scheduler.
 *   Since the sch_priority is common for all ports in the ETHU, need
 *   to update the reset only if our port is the only port in the ETHU
 *   which has this sch_priority.
 */
static int
imb_ethu_port_rx_nmg_reset_per_sch_prio_set(
    int unit,
    bcm_port_t port,
    uint32 sch_priority,
    int in_reset)
{
    bcm_pbmp_t ethu_ports;
    bcm_port_t port_i;
    dnx_algo_port_rmc_info_t rmc;
    uint32 max_rmcs_per_port = dnx_data_nif.eth.priority_groups_nof_get(unit);
    int rmc_index = 0, is_enabled = 0;
    int is_multiple_ports_have_same_sch_priority = 0;
    uint32 dbal_sch_prio = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETH ports on ETHU
     */
    SHR_IF_ERR_EXIT(imb_ethu_master_ports_get(unit, port, &ethu_ports));
    /** Exclude self port */
    BCM_PBMP_PORT_REMOVE(ethu_ports, port);
    /*
     * Search for other ports in the ETHU that have the same sch_priority
     */
    BCM_PBMP_ITER(ethu_ports, port_i)
    {
        SHR_IF_ERR_EXIT(imb_ethu_port_enable_get(unit, port_i, &is_enabled));
        if (!is_enabled)
        {
            /*
             * If the other ports on the ETHU are not enabled the fifo information will be set
             *  but  the NMG won't be put out of reset
             *  (with other words imb_ethu_port_rx_nmg_reset_per_sch_prio_hw_set won't be executed).
             *  Continue the optimization check only if the current port is enabled
             */
            continue;
        }

        for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port_i, 0, rmc_index, &rmc));
            if ((rmc.rmc_id != IMB_COSQ_RMC_INVALID) && (rmc.sch_priority == sch_priority))
            {
                is_multiple_ports_have_same_sch_priority = 1;
                break;
            }
        }

        if (is_multiple_ports_have_same_sch_priority)
        {
            break;
        }
    }

    /*
     * Update the reset only if our port is the only port in the ETHU
     * which has this sch_priority.
     */
    if (is_multiple_ports_have_same_sch_priority == 0)
    {
        SHR_IF_ERR_EXIT(imb_common_dbal_sch_prio_from_bcm_sch_prio_get(unit, sch_priority, &dbal_sch_prio));
        SHR_IF_ERR_EXIT(imb_ethu_port_rx_nmg_reset_per_sch_prio_hw_set(unit, port, dbal_sch_prio, in_reset));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Reset priority groups in the scheduler
 *   Configure the specific sch_priority, or all sch_priorities
 *   of the port in case sch_priority=IMB_COSQ_ALL_SCH_PRIO.
 *
 * see .h file
 */
int
imb_ethu_port_rx_nmg_reset_sch_prio_set(
    int unit,
    bcm_port_t port,
    uint32 sch_priority,
    int in_reset)
{
    dnx_algo_port_rmc_info_t rmc;
    uint32 max_rmcs_per_port = dnx_data_nif.eth.priority_groups_nof_get(unit);
    int rmc_index = 0;
    bcm_port_nif_scheduler_t sch_priority_to_reset;
    SHR_FUNC_INIT_VARS(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc));
        sch_priority_to_reset = (sch_priority == IMB_COSQ_ALL_SCH_PRIO) ? rmc.sch_priority : sch_priority;
        if ((rmc.rmc_id != IMB_COSQ_RMC_INVALID) && (rmc.sch_priority == sch_priority_to_reset))
        {
            SHR_IF_ERR_EXIT(imb_ethu_port_rx_nmg_reset_per_sch_prio_set(unit, port, sch_priority_to_reset, in_reset));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable / disable RMCs of the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - flags
 * \param [in] enable - enable indication
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
imb_ethu_port_rmc_enable_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable)
{

    SHR_FUNC_INIT_VARS(unit);

    if (enable)
    {
        /*
         * take the port out of reset in ETHU RX
         */
        if (dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_nif_scheduler_context_per_rmc_support))
        {
            SHR_IF_ERR_EXIT(imb_ethu_port_rx_nmg_reset_hw_set(unit, port, IMB_COSQ_ALL_RMCS, IMB_COMMON_OUT_OF_RESET));
        }
        else
        {
            SHR_IF_ERR_EXIT(imb_ethu_port_rx_nmg_reset_sch_prio_set
                            (unit, port, IMB_COSQ_ALL_SCH_PRIO, IMB_COMMON_OUT_OF_RESET));
        }
        SHR_IF_ERR_EXIT(imb_ethu_port_rx_reset_hw_set(unit, port, IMB_COSQ_ALL_RMCS, IMB_COMMON_OUT_OF_RESET));

    }
    else
    {
        /** disable */

        /*
         * Flush if available and reset the port in ETHU (RMC)
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_rx_flush_and_reset(unit, port));

        /*
         * reset the port in ETHU (NMG)
         */
        if (dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_nif_scheduler_context_per_rmc_support))
        {
            SHR_IF_ERR_EXIT(imb_ethu_port_rx_nmg_reset_hw_set(unit, port, IMB_COSQ_ALL_RMCS, IMB_COMMON_IN_RESET));
        }
        else
        {
            SHR_IF_ERR_EXIT(imb_ethu_port_rx_nmg_reset_sch_prio_set
                            (unit, port, IMB_COSQ_ALL_SCH_PRIO, IMB_COMMON_IN_RESET));
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable / disable the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - flags
 * \param [in] enable - enable indication
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
imb_ethu_port_enable_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable)
{
    dnx_algo_port_info_s port_info;
    int loopback;
    int sif_instance_core, sif_instance_id;
    uint8 is_connected = FALSE;
    int rv;
    bcm_port_t port_i;
    bcm_pbmp_t pbmp_same_if;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, port, DNX_ALGO_PORT_CHANNELS_F_NON_L1_ONLY, &pbmp_same_if));

    if (enable)
    {
        if (IMB_ETHU_IS_TX_CREDIT_PORT(unit, port_info))
        {
            /*
             * stop the egr flush
             */
            SHR_IF_ERR_EXIT(imb_ethu_port_tx_egress_flush_set(unit, port, 0));
        }

        /*
         * Configure TXI logic and IRDY threshold
         */
        if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support)
            && DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_esb_port_txi_config_set(unit, port, TRUE));
        }

        /*
         * reset the port in ETHU TX
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_tx_nmg_reset_hw_set(unit, port, IMB_COMMON_IN_RESET));
        SHR_IF_ERR_EXIT(imb_ethu_port_tx_reset_hw_set(unit, port, IMB_COMMON_IN_RESET));

        /*
         * Reset the credit counter before ETHU Tx is out of reset
         */
        if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_l1_mismatch_rate_support)
            && DNX_ALGO_PORT_TYPE_IS_ETH_L1(unit, port_info))
        {
            SHR_IF_ERR_EXIT(imb_ethu_port_l1_tx_credit_counter_reset_hw_set(unit, port, IMB_COMMON_IN_RESET));
            SHR_IF_ERR_EXIT(imb_ethu_port_l1_tx_credit_counter_reset_hw_set(unit, port, IMB_COMMON_OUT_OF_RESET));
        }
        /*
         * take the port out of reset in ETHU TX
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_tx_nmg_reset_hw_set(unit, port, IMB_COMMON_OUT_OF_RESET));
        SHR_IF_ERR_EXIT(imb_ethu_port_tx_reset_hw_set(unit, port, IMB_COMMON_OUT_OF_RESET));

        /** take macsec port out of reset */
#ifdef INCLUDE_XFLOW_MACSEC
        if (dnx_data_macsec.general.feature_get(unit, dnx_data_macsec_general_is_macsec_supported))
        {
            /** toggle soft reset */
            SHR_IF_ERR_EXIT(dnx_xflow_macsec_port_soft_reset_set(unit, port, IMB_COMMON_IN_RESET));
            SHR_IF_ERR_EXIT(dnx_xflow_macsec_port_soft_reset_set(unit, port, IMB_COMMON_OUT_OF_RESET));
        }
#endif

        if (IMB_ETHU_IS_RX_RMC_PORT(unit, port_info))
        {
            /*
             * take the port out of reset in ETHU RX
             */
            SHR_IF_ERR_EXIT(imb_ethu_port_rmc_enable_set(unit, port, flags, enable));
        }

        /*
         * override the egress credits
         */
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            /*
             * Wait for NIF to finish transmitting initial credits
             */
            sal_usleep(10);

            SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_credit_default_set(unit, port));
        }

        /*
         * enable the port in Portmod
         */
        SHR_IF_ERR_EXIT(imb_er_portmod_port_enable_set(unit, port, 0, enable));

        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            /*
             * Enable Qpairs
             */
            _SHR_PBMP_ITER(pbmp_same_if, port_i)
            {
                SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, port_i, enable, FALSE));
            }
        }
    }
    else
    {

        /*
         * Disable Qpairs
         */
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            _SHR_PBMP_ITER(pbmp_same_if, port_i)
            {
                SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, port_i, enable, FALSE));
            }
        }
        if (IMB_ETHU_IS_RX_RMC_PORT(unit, port_info))
        {
            /*
             * reset the port in ETHU RX
             */
            SHR_IF_ERR_EXIT(imb_ethu_port_rmc_enable_set(unit, port, flags, enable));

        }

        /*
         * flush the egress
         */
        if (IMB_ETHU_IS_TX_CREDIT_PORT(unit, port_info))
        {
            SHR_IF_ERR_EXIT(imb_ethu_port_tx_egress_flush_set(unit, port, 1));
        }

        /** soft reset macsec port */
#ifdef INCLUDE_XFLOW_MACSEC
        if (dnx_data_macsec.general.feature_get(unit, dnx_data_macsec_general_is_macsec_supported))
        {
            /** toggle soft reset */
            SHR_IF_ERR_EXIT(dnx_xflow_macsec_port_soft_reset_set(unit, port, IMB_COMMON_IN_RESET));
        }
#endif

        /** special handling for STIF port */
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info))
        {
            /*
             * reset the port in ETHU TX
             */
            SHR_IF_ERR_EXIT(imb_ethu_port_tx_nmg_reset_hw_set(unit, port, IMB_COMMON_IN_RESET));
            SHR_IF_ERR_EXIT(imb_ethu_port_tx_reset_hw_set(unit, port, IMB_COMMON_IN_RESET));

            /*
             * Check if the port is already connected to SIF, if yes, the credits in the sif controller should be reset
             */
            SHR_IF_ERR_EXIT(dnx_stif_mgmt_logical_port_is_connected
                            (unit, port, &sif_instance_core, &sif_instance_id, &is_connected));

            if (is_connected)
            {
                SHR_IF_ERR_EXIT(dnx_stif_mgmt_credit_reset(unit, sif_instance_core, sif_instance_id));
            }
        }

        /*
         * disable the port in portmod
         */
        SHR_IF_ERR_EXIT(imb_er_portmod_port_enable_set(unit, port, 0, enable));

        /*
         * Configure TXI logic and IRDY threshold
         */
        if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support)
            && DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_esb_port_txi_config_set(unit, port, FALSE));
        }
    }

    DNX_ERR_RECOVERY_SUPPRESS(unit);
    SHR_IF_ERR_EXIT(imb_port_loopback_get(unit, port, &loopback));
    rv = _bcm_linkscan_available(unit);
    if ((loopback == BCM_PORT_LOOPBACK_MAC) && (rv == BCM_E_NONE))
    {
        SHR_IF_ERR_EXIT(_bcm_link_force(unit, port, TRUE, enable));
    }
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get port enable indication
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable indication
 *
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ethu_port_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    /*
     * it is enough to get the indication from Portmod
     */

    SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, port, 0, enable));

    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable / disable Tx data to the PM.
 *      see imb_ethu_tx_data_to_pm_enable_hw_set
 */
int
imb_ethu_port_tx_data_to_pm_enable_set(
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
    /*
     * call direct function
     */
    SHR_IF_ERR_EXIT(imb_ethu_tx_data_to_pm_enable_hw_set(unit, ethu_info.core, ethu_info.lane_in_core, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable / disable port loopback
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] loopback - loopback type
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
imb_ethu_port_loopback_set(
    int unit,
    bcm_port_t port,
    int loopback)
{
    portmod_loopback_mode_t portmod_loopback;
    int curr_loopback, is_symmetric;
    int rv;
    int link, is_linkscan_available;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Since the lane map for PM8x50 has been moved to PMD, PHY/REMOTE
     * loopback cannot be configured when lane map is not symmetric.
     */
    if (loopback == BCM_PORT_LOOPBACK_PHY || loopback == BCM_PORT_LOOPBACK_PHY_REMOTE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_is_symmetric_get(unit, port, &is_symmetric));
        if (!is_symmetric)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "Lane Map for port %d is not symmetric. Configuring PHY/REMOTE loopback on non-symmetric lane swapped port is forbidden, please adjust the lane map or use MAC loopback",
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

    rv = _bcm_linkscan_available(unit);
    if (rv == BCM_E_NONE)
    {
        is_linkscan_available = TRUE;
    }
    else
    {
        is_linkscan_available = FALSE;
    }

    if (curr_loopback != BCM_PORT_LOOPBACK_NONE)
    {
        /*
         * Open current loopback
         */
        SHR_IF_ERR_EXIT(imb_portmod_loopback_from_bcm_loopback_get(unit, port, curr_loopback, &portmod_loopback));
        SHR_IF_ERR_EXIT(portmod_port_loopback_set(unit, port, portmod_loopback, 0));
        if ((is_linkscan_available == TRUE) && (curr_loopback == BCM_PORT_LOOPBACK_MAC))
        {
            SHR_IF_ERR_EXIT(_bcm_link_force(unit, port, FALSE, FALSE));
        }

    }
    if (loopback != BCM_PORT_LOOPBACK_NONE)
    {
        /*
         * Close requested loopback
         */
        SHR_IF_ERR_EXIT(imb_portmod_loopback_from_bcm_loopback_get(unit, port, loopback, &portmod_loopback));
        SHR_IF_ERR_EXIT(portmod_port_loopback_set(unit, port, portmod_loopback, 1));
        if ((is_linkscan_available == TRUE) && (loopback == BCM_PORT_LOOPBACK_MAC))
        {
            SHR_IF_ERR_EXIT(imb_port_enable_get(unit, port, &link));
            SHR_IF_ERR_EXIT(_bcm_link_force(unit, port, TRUE, link));
        }
        /*
         * If in adapter - in case of PHY loopback perform also MAC loopback in Portmod (set LOCAL_LPBK bit) 
         */
#ifdef ADAPTER_SERVER_MODE
        if (loopback == BCM_PORT_LOOPBACK_PHY)
        {
            SHR_IF_ERR_EXIT(portmod_port_loopback_set(unit, port, portmodLoopbackMacOuter, 1));
            if (is_linkscan_available == TRUE)
            {
                SHR_IF_ERR_EXIT(imb_port_enable_get(unit, port, &link));
                SHR_IF_ERR_EXIT(_bcm_link_force(unit, port, TRUE, link));
            }
        }
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get loopback type on the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] loopback - loopback type
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
imb_ethu_port_loopback_get(
    int unit,
    bcm_port_t port,
    int *loopback)
{
    int rv = 0, lb_enabled = 0;
    portmod_loopback_mode_t portmod_loopback;
    SHR_FUNC_INIT_VARS(unit);

    *loopback = 0;

#ifdef ADAPTER_SERVER_MODE
    portmod_loopback = portmodLoopbackPhyGloopPMD;
    rv = portmod_port_loopback_get(unit, port, portmod_loopback, &lb_enabled);
    if (lb_enabled)
    {
        SHR_IF_ERR_EXIT(imb_bcm_loopback_from_portmod_loopback_get(unit, port, portmod_loopback, loopback));
        SHR_EXIT();
    }
#endif

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

int
imb_ethu_port_gen_pfc_from_rmc_enable_set(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 enable)
{
    int rmc;
    bcm_pbmp_t rmcs;
    dnx_algo_port_ethu_access_info_t ethu_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Determine the ETHU access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_access_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_pbmp_get(unit, port, 0, &rmcs));
    _SHR_PBMP_ITER(rmcs, rmc)
    {
        /** Set HW to enable all RMCs of the port to produce PFC for the specified priority */
        SHR_IF_ERR_EXIT(imb_ethu_internal_port_gen_pfc_from_rmc_enable_set
                        (unit, ethu_access_info.core, ethu_access_info.ethu_id_in_core, rmc, priority, enable));
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_ethu_port_gen_pfc_from_rmc_enable_get(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 *enable)
{
    int rmc;
    bcm_pbmp_t rmcs;
    dnx_algo_port_ethu_access_info_t ethu_access_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");

    /** Determine the ETHU access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_access_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_pbmp_get(unit, port, 0, &rmcs));

    /** Check HW if the RMCs of the port are enabled to produce PFC for the specified priority */
    /** Get the Enable field of the first RMC only, the rest have the same value */
    _SHR_PBMP_FIRST(rmcs, rmc);
    SHR_IF_ERR_EXIT(imb_ethu_internal_port_gen_pfc_from_rmc_enable_get
                    (unit, ethu_access_info.core, ethu_access_info.ethu_id_in_core, rmc, priority, enable));

exit:
    SHR_FUNC_EXIT;
}

int
imb_ethu_port_gen_llfc_from_rmc_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    int rmc;
    bcm_pbmp_t rmcs;
    int lane_in_core, global_lane;
    bcm_pbmp_t lanes;
    dnx_algo_port_ethu_access_info_t ethu_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Determine the ETHU access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_access_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_pbmp_get(unit, port, 0, &rmcs));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &lanes));

    /** Mapping of port's RMCs(all lanes) to generate LLFC */
    _SHR_PBMP_ITER(lanes, global_lane)
    {
        /** Convert global lane to lane in core */
        lane_in_core = global_lane - dnx_data_nif.phys.nof_phys_per_core_get(unit) * ethu_access_info.core;
        _SHR_PBMP_ITER(rmcs, rmc)
        {
            SHR_IF_ERR_EXIT(imb_ethu_internal_port_gen_llfc_from_rmc_enable_set
                            (unit, ethu_access_info.core, lane_in_core, rmc, enable));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_ethu_port_stop_pm_from_cfc_llfc_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    int lane_in_core, global_lane;
    bcm_pbmp_t lanes;
    dnx_algo_port_ethu_access_info_t ethu_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Determine the CUD access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_access_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &lanes));

    /** Set HW to stop transmission on all lanes of the port according LLFC signal received from CFC */
    _SHR_PBMP_ITER(lanes, global_lane)
    {
        /** Convert global lane to lane in core */
        lane_in_core = global_lane - dnx_data_nif.phys.nof_phys_per_core_get(unit) * ethu_access_info.core;
        SHR_IF_ERR_EXIT(imb_ethu_internal_port_stop_pm_from_cfc_llfc_enable_set
                        (unit, ethu_access_info.core, lane_in_core, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

int
imb_ethu_port_stop_pm_from_pfc_enable_set(
    int unit,
    bcm_port_t port,
    int pfc,
    uint32 enable)
{
    int pfc_internal = 0;
    dnx_algo_port_ethu_access_info_t ethu_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Determine the CUD access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_access_info));

    /** Standard case */
    pfc_internal = pfc;

    /** In Jericho 2 the last two ETHUs of each core have one bit shift left so the bitmap configuration is changed.
      * The implications here are that the mapping at CFC also needs to be shifted, causing the last priority of the last
      * NIF port to be lost and the first priority of the cdu=(nof_cdus-2) to be unused */
    if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_pfc_rec_cdu_shift)
        && (ethu_access_info.ethu_id_in_core >= (dnx_data_nif.eth.cdu_nof_per_core_get(unit) - 2)))
    {
        pfc_internal = ((pfc == 0) ? (BCM_COS_COUNT - 1) : (pfc - 1));
    }

    /** Set HW to stop transmission on the port according to received PFC signal */
    SHR_IF_ERR_EXIT(imb_ethu_internal_port_stop_pm_from_pfc_enable_set
                    (unit, ethu_access_info.core, ethu_access_info.ethu_id_in_core, pfc_internal, enable));

exit:
    SHR_FUNC_EXIT;
}

int
imb_ethu_port_stop_pm_from_pfc_enable_get(
    int unit,
    bcm_port_t port,
    int pfc,
    uint32 *enable)
{
    int pfc_internal = 0;
    dnx_algo_port_ethu_access_info_t ethu_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Determine the CUD access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_access_info));

    /** Standard case */
    pfc_internal = pfc;

    /** In Jericho 2 the last two ETHUs of each core have one bit shift left so the bitmap configuration is changed.
      * The implications here are that the mapping at CFC also needs to be shifted, causing the last priority of the last
      * NIF port to be lost and the first priority of the cdu=(nof_cdus-2) to be unused */
    if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_pfc_rec_cdu_shift)
        && (ethu_access_info.ethu_id_in_core >= (dnx_data_nif.eth.cdu_nof_per_core_get(unit) - 2)))
    {
        pfc_internal = ((pfc == 0) ? (BCM_COS_COUNT - 1) : (pfc - 1));
    }

    /** Get from HW enable indication for received PFC to stop transmission on a port */
    SHR_IF_ERR_EXIT(imb_ethu_internal_port_stop_pm_from_pfc_enable_get
                    (unit, ethu_access_info.core, ethu_access_info.ethu_id_in_core, pfc_internal, enable));

exit:
    SHR_FUNC_EXIT;
}

int
imb_ethu_port_gen_pfc_from_llfc_thresh_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    int prio_group;
    uint32 nof_prio_groups_supported;
    dnx_algo_port_rmc_info_t rmc_info;
    dnx_algo_port_ethu_access_info_t ethu_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Determine the CUD access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_access_info));

    /** Determine the number of possible priority groups and get the relevant RMC ID for each of them */
    nof_prio_groups_supported = dnx_data_nif.eth.priority_groups_nof_get(unit);
    for (prio_group = 0; prio_group < nof_prio_groups_supported; ++prio_group)
    {
        /** Get logical FIFO information and set HW for each RMC of the port*/
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, prio_group, &rmc_info));
        if (rmc_info.rmc_id != -1)
        {
            /** Set the threshold value to HW */
            SHR_IF_ERR_EXIT(imb_ethu_internal_port_gen_pfc_from_llfc_thresh_enable_set
                            (unit, ethu_access_info.core, ethu_access_info.ethu_id_in_core, rmc_info.rmc_id, enable));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

int
imb_ethu_port_gen_pfc_from_llfc_thresh_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    int prio_group;
    uint32 nof_prio_groups_supported;
    dnx_algo_port_rmc_info_t rmc_info;
    dnx_algo_port_ethu_access_info_t ethu_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Determine the CUD access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_access_info));

    /** Determine the number of possible priority groups and get the relevant RMC ID for each of them */
    nof_prio_groups_supported = dnx_data_nif.eth.priority_groups_nof_get(unit);
    for (prio_group = 0; prio_group < nof_prio_groups_supported; ++prio_group)
    {
        /** Get logical FIFO information and set HW for each RMC of the port*/
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, prio_group, &rmc_info));
        if (rmc_info.rmc_id != -1)
        {
            /** Set the threshold value to HW */
            SHR_IF_ERR_EXIT(imb_ethu_internal_port_gen_pfc_from_llfc_thresh_enable_get
                            (unit, ethu_access_info.core, ethu_access_info.ethu_id_in_core, rmc_info.rmc_id, enable));

            /** We only need to get the indication for the first RMC
             * of the port, because all are set the same */
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static int
imb_ethu_port_fc_rx_qmlf_threshold_set_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 *threshold,
    uint8 is_set)
{
    dnx_algo_port_rmc_info_t rmc;
    int rmc_fifo_size = 0;
    int nof_prio_groups_supported;
    int prio_group;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold");

    /*
     * Validate threshold flags
     */
    if (!(flags & (BCM_COSQ_THRESHOLD_SET | BCM_COSQ_THRESHOLD_CLEAR)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Unit %d, Need to set at least one of flags BCM_COSQ_THRESHOLD_SET BCM_COSQ_THRESHOLD_CLEAR\n",
                     unit);
    }

    if (!(flags & (BCM_COSQ_THRESHOLD_ETH_PORT_LLFC | BCM_COSQ_THRESHOLD_ETH_PORT_PFC)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Unit %d, Need to set one of flags BCM_COSQ_THRESHOLD_ETH_PORT_LLFC BCM_COSQ_THRESHOLD_ETH_PORT_PFC\n",
                     unit);
    }

    if ((flags & BCM_COSQ_THRESHOLD_ETH_PORT_LLFC) && (flags & BCM_COSQ_THRESHOLD_ETH_PORT_PFC))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Unit %d, Can not set both of flags BCM_COSQ_THRESHOLD_ETH_PORT_LLFC BCM_COSQ_THRESHOLD_ETH_PORT_PFC\n",
                     unit);
    }

    /** Check the port type, thresholds are only for Ethernet ports */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (!(DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, FALSE)))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Unit %d, Thresholds can only be set for Ethernet ports. \n", unit);
    }
    if (!(DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info)))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Unit %d, Thresholds can only be set for ingress TM ports. \n", unit);
    }

    if (is_set)
    {
        /** Determine the RMC size. The threshold can be of any size up to the RMC size */
        nof_prio_groups_supported = dnx_data_nif.eth.priority_groups_nof_get(unit);
        for (prio_group = 0; prio_group < nof_prio_groups_supported; ++prio_group)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, prio_group, &rmc));
            if (rmc.rmc_id == -1)
            {
                /** RMC is invalid */
                continue;
            }
            rmc_fifo_size = rmc.last_entry - rmc.first_entry + 1;
        }

        if (*threshold > rmc_fifo_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Unit %d, Unsupported threshold value! Must be of any size up to the RMC size: %d\n", unit,
                         rmc_fifo_size);
        }
    }
    else
    {
        /** Verify that not more than one FIFO priority is specified to get threshold for */
        SHR_NOF_SET_BITS_IN_RANGE_VERIFY(flags,
                                         (BCM_COSQ_THRESHOLD_ETH_PORT_LOW_FIFO | BCM_COSQ_THRESHOLD_ETH_PORT_HIGH_FIFO |
                                          BCM_COSQ_THRESHOLD_ETH_PORT_TDM_FIFO), 0, 1, _SHR_E_PARAM,
                                         "Can not set more than one flag for FIFO priority\n");
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_ethu_port_fc_rx_qmlf_threshold_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 threshold)
{
    int prio_group;
    uint32 nof_prio_groups_supported;
    dnx_algo_port_rmc_info_t rmc_info;
    dnx_algo_port_ethu_access_info_t ethu_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(imb_ethu_port_fc_rx_qmlf_threshold_set_get_verify(unit, port, flags, &threshold, TRUE));

    /** Determine the CUD access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_access_info));

    /** Determine the number of possible priority groups and get the relevant RMC ID for each of them */
    nof_prio_groups_supported = dnx_data_nif.eth.priority_groups_nof_get(unit);
    for (prio_group = 0; prio_group < nof_prio_groups_supported; ++prio_group)
    {
        /** Get logical FIFO information */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, prio_group, &rmc_info));
        if (rmc_info.rmc_id != -1)
        {
            if (((rmc_info.sch_priority == bcmPortNifSchedulerLow)
                 && (_SHR_IS_FLAG_SET(flags, BCM_COSQ_THRESHOLD_ETH_PORT_LOW_FIFO)))
                || ((rmc_info.sch_priority == bcmPortNifSchedulerHigh)
                    && (_SHR_IS_FLAG_SET(flags, BCM_COSQ_THRESHOLD_ETH_PORT_HIGH_FIFO)))
                || ((rmc_info.sch_priority == bcmPortNifSchedulerTDM)
                    && (_SHR_IS_FLAG_SET(flags, BCM_COSQ_THRESHOLD_ETH_PORT_TDM_FIFO)))
                ||
                (!(flags &
                   (BCM_COSQ_THRESHOLD_ETH_PORT_TDM_FIFO | BCM_COSQ_THRESHOLD_ETH_PORT_LOW_FIFO |
                    BCM_COSQ_THRESHOLD_ETH_PORT_HIGH_FIFO))))
            {
                       /** Set the threshold value to HW */
                SHR_IF_ERR_EXIT(imb_ethu_internal_port_fc_rx_qmlf_threshold_set
                                (unit, ethu_access_info.core, ethu_access_info.ethu_id_in_core, rmc_info.rmc_id, flags,
                                 threshold));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_ethu_port_fc_rx_qmlf_threshold_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 *threshold)
{
    int prio_group;
    uint32 nof_prio_groups_supported;
    dnx_algo_port_rmc_info_t rmc_info;
    dnx_algo_port_ethu_access_info_t ethu_access_info;
    int thresh_get[DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF];
    int thresh_temp = -1;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(imb_ethu_port_fc_rx_qmlf_threshold_set_get_verify(unit, port, flags, threshold, FALSE));

    /** Determine the CUD access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_access_info));
    /** Determine the number of possible priority groups */
    nof_prio_groups_supported = dnx_data_nif.eth.priority_groups_nof_get(unit);

    if (!
        (flags &
         (BCM_COSQ_THRESHOLD_ETH_PORT_TDM_FIFO | BCM_COSQ_THRESHOLD_ETH_PORT_LOW_FIFO |
          BCM_COSQ_THRESHOLD_ETH_PORT_HIGH_FIFO)))
    {
         /** The threshold requested is for all RMCs of the port, meaning it should be the same all FIFOs (High, Low and TDM)
           * Get the threshold value of each of the FIFO priorities and make sure they are the same.
           * Get the relevant RMC ID for each of the priority groups */
        for (prio_group = 0; prio_group < nof_prio_groups_supported; ++prio_group)
        {
            /*
             * Init thresholds array with invalid value
             */
            thresh_get[prio_group] = -1;
             /** Get logical FIFO information */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, prio_group, &rmc_info));
            if (rmc_info.rmc_id != -1)
            {
                /** Get the threshold value of the RMC for current prio */
                SHR_IF_ERR_EXIT(imb_ethu_internal_port_fc_rx_qmlf_threshold_get
                                (unit, ethu_access_info.core, ethu_access_info.ethu_id_in_core, rmc_info.rmc_id,
                                 flags, (uint32 *) &thresh_get[prio_group]));
            }
        }

          /** Make sure the threshold value of each of the FIFO priorities is the same */
        for (prio_group = 0; prio_group < nof_prio_groups_supported; ++prio_group)
        {
             /** Get the first value that is different from zero */
            if ((thresh_temp == -1) && (thresh_get[prio_group] != 0))
            {
                thresh_temp = thresh_get[prio_group];
            }

              /** Compare the values to the first non-zero and make sure they are the same */
            if ((thresh_get[prio_group] != thresh_temp) && (thresh_get[prio_group] != -1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Unit %d, Thresholds for High prio, Low prio and TDM FIFOs are different, please specify which threshold to get.\n",
                             unit);
            }
        }

          /** If we did not get an error from the loop above, the thresholds are the same and we can return the first non-zero value */
        *threshold = thresh_temp;
    }
    else
    {
       /** The threshold requested is for a specific FIFO
         * Get the relevant RMC ID for each of the priority groups */
        for (prio_group = 0; prio_group < nof_prio_groups_supported; ++prio_group)
        {
               /** Get logical FIFO information */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, prio_group, &rmc_info));
            if (rmc_info.rmc_id != -1)
            {
                if (((rmc_info.sch_priority == bcmPortNifSchedulerLow)
                     && (_SHR_IS_FLAG_SET(flags, BCM_COSQ_THRESHOLD_ETH_PORT_LOW_FIFO)))
                    || ((rmc_info.sch_priority == bcmPortNifSchedulerHigh)
                        && (_SHR_IS_FLAG_SET(flags, BCM_COSQ_THRESHOLD_ETH_PORT_HIGH_FIFO)))
                    || ((rmc_info.sch_priority == bcmPortNifSchedulerTDM)
                        && (_SHR_IS_FLAG_SET(flags, BCM_COSQ_THRESHOLD_ETH_PORT_TDM_FIFO))))
                {
                         /** If a FIFO is specified, get the threshold value of the first RMC only, the rest have the same value */
                    SHR_IF_ERR_EXIT(imb_ethu_internal_port_fc_rx_qmlf_threshold_get
                                    (unit, ethu_access_info.core, ethu_access_info.ethu_id_in_core, rmc_info.rmc_id,
                                     flags, (uint32 *) &thresh_temp));
                    break;
                }
            }
        }

        /** Get the first value that is different from zero */
        if (thresh_temp == -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unit %d, Threshold not found - no RMC allocated for the specified priority.\n",
                         unit);
        }
        else
        {
            /** If we did not get an error, we can return the value */
            *threshold = thresh_temp;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_ethu_fc_reset_set(
    int unit,
    const imb_create_info_t * imb_info,
    uint32 in_reset)
{
    int nof_ethus_per_core, ethu_id_in_core;
    bcm_core_t core;
    SHR_FUNC_INIT_VARS(unit);

    nof_ethus_per_core = dnx_data_nif.eth.ethu_nof_per_core_get(unit);
    core = imb_info->inst_id >= nof_ethus_per_core ? 1 : 0;
    ethu_id_in_core = imb_info->inst_id % nof_ethus_per_core;

    /** Set HW to put in/out of reset all ETHUs */
    SHR_IF_ERR_EXIT(imb_ethu_internal_fc_reset_set(unit, core, ethu_id_in_core, in_reset));

exit:
    SHR_FUNC_EXIT;
}

int
imb_ethu_llfc_from_glb_rsc_enable_set(
    int unit,
    const imb_create_info_t * imb_info,
    uint32 enable)
{
    int nof_ethus_per_core, ethu_id_in_core, lane;
    int lane_in_core, first_lane_in_ethu, nof_lanes_in_ethu;
    bcm_core_t core;
    SHR_FUNC_INIT_VARS(unit);

    nof_ethus_per_core = dnx_data_nif.eth.ethu_nof_per_core_get(unit);
    core = imb_info->inst_id >= nof_ethus_per_core ? 1 : 0;
    ethu_id_in_core = imb_info->inst_id % nof_ethus_per_core;
    SHR_IF_ERR_EXIT(dnx_algo_ethu_id_lane_info_get
                    (unit, core, ethu_id_in_core, &first_lane_in_ethu, &nof_lanes_in_ethu));

    /** Set HW to enable/disable the generation of LLFC based on a signal for Global resources for all lanes of the ETHU */
    for (lane = first_lane_in_ethu; lane < (first_lane_in_ethu + nof_lanes_in_ethu); lane++)
    {
        lane_in_core = lane % dnx_data_nif.phys.nof_phys_per_core_get(unit);
        SHR_IF_ERR_EXIT(imb_ethu_internal_llfc_from_glb_rsc_enable_set(unit, core, lane_in_core, enable));
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_ethu_pfc_rec_priority_map(
    int unit,
    int imb_id,
    uint32 nif_priority,
    uint32 egq_priority)
{
    imb_create_info_t imb_info;
    uint32 nif_priority_internal = 0;
    uint32 egq_priority_internal = 0;
    int nof_ethus_per_core, ethu_id_in_core;
    bcm_core_t core;
    SHR_FUNC_INIT_VARS(unit);

    /** Get IMB info  and verify parameters */
    SHR_IF_ERR_EXIT(imbm.imb.get(unit, imb_id, &imb_info));

    nof_ethus_per_core = dnx_data_nif.eth.ethu_nof_per_core_get(unit);
    core = imb_info.inst_id >= nof_ethus_per_core ? 1 : 0;
    ethu_id_in_core = imb_info.inst_id % nof_ethus_per_core;

    /** Standard case */
    nif_priority_internal = nif_priority;
    egq_priority_internal = egq_priority;

    /** In Jericho 2 the last two ETHUs of each core have one bit shift left so the bitmap configuration is changed.
      * The implications here are that the mapping at CFC also needs to be shifted, causing the last priority of the last
      * NIF port to be lost and the first priority of the cdu=(nof_cdus-2) to be unused */
    if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_pfc_rec_cdu_shift)
        && (ethu_id_in_core >= (dnx_data_nif.eth.cdu_nof_per_core_get(unit) - 2)))
    {
        nif_priority_internal = nif_priority;
        egq_priority_internal = ((egq_priority == 0) ? (BCM_COS_COUNT - 1) : (egq_priority - 1));
    }

    /** Set mapping to HW*/
    SHR_IF_ERR_EXIT(imb_ethu_internal_pfc_rec_priority_map
                    (unit, core, ethu_id_in_core, nif_priority_internal, egq_priority_internal));

exit:
    SHR_FUNC_EXIT;
}

int
imb_ethu_pfc_rec_priority_unmap(
    int unit,
    int imb_id)
{
    imb_create_info_t imb_info;
    int nof_ethus_per_core, ethu_id_in_core;
    bcm_core_t core;
    SHR_FUNC_INIT_VARS(unit);

    /** Get IMB info  and verify parameters */
    SHR_IF_ERR_EXIT(imbm.imb.get(unit, imb_id, &imb_info));

    nof_ethus_per_core = dnx_data_nif.eth.ethu_nof_per_core_get(unit);
    core = imb_info.inst_id >= nof_ethus_per_core ? 1 : 0;
    ethu_id_in_core = imb_info.inst_id % nof_ethus_per_core;

    /** Unmap the specified ETHU in HW for all priorities */
    SHR_IF_ERR_EXIT(imb_ethu_internal_pfc_rec_priority_unmap(unit, core, ethu_id_in_core));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set local fault enable value per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable value (1 for enabled)
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
imb_ethu_port_local_fault_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    portmod_local_fault_control_t control;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_local_fault_control_get(unit, port, &control));
    control.enable = enable;

    SHR_IF_ERR_EXIT(portmod_port_local_fault_control_set(unit, port, &control));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Get local fault enable value per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable value (1 for enabled)
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
imb_ethu_port_local_fault_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    portmod_local_fault_control_t control;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_local_fault_control_get(unit, port, &control));
    *enable = control.enable;

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Set remote fault enable value per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable value (1 for enabled)
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
imb_ethu_port_remote_fault_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    portmod_remote_fault_control_t control;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_remote_fault_control_get(unit, port, &control));
    control.enable = enable;

    SHR_IF_ERR_EXIT(portmod_port_remote_fault_control_set(unit, port, &control));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Get remote fault enable value per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable value (1 for enabled)
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
imb_ethu_port_remote_fault_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    portmod_remote_fault_control_t control;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_remote_fault_control_get(unit, port, &control));
    *enable = control.enable;

exit:
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
imb_ethu_port_link_state_get(
    int unit,
    bcm_port_t port,
    int clear_status,
    bcm_port_link_state_t * link_state)
{
    int sw_latch_down, hw_latch_down;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * link up indication
     */
    SHR_IF_ERR_EXIT(portmod_port_link_get(unit, port, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, &link_state->status));
    /*
     * get latch down from DB
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_latch_down_get(unit, port, &sw_latch_down));
    /*
     * latch down indication
     */
    SHR_IF_ERR_EXIT(portmod_port_link_latch_down_get(unit,
                                                     port,
                                                     clear_status ? PORTMOD_PORT_LINK_LATCH_DOWN_F_CLEAR : 0,
                                                     &hw_latch_down));

    link_state->latch_down = sw_latch_down || hw_latch_down;

    if (clear_status)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_latch_down_set(unit, port, 0));
    }
    else
    {
        /*
         * dont count on the register to keep the value. save in SW DB just in case
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_latch_down_set(unit, port, link_state->latch_down));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get all master logical ports on a PM
 *
 * see .h file
 */
int
imb_ethu_pm_master_ports_get(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t * ethu_ports)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    int first_phy;
    bcm_port_t port_i;
    bcm_pbmp_t all_ports, pm_phys, all_flexe_phy_ports;
    int pm_i;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * clear output pbmp
     */
    BCM_PBMP_CLEAR(*ethu_ports);
    /*
     * get port ethu info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * get ETHU lanes
     */
    BCM_PBMP_CLEAR(pm_phys);

    pm_i = dnx_data_nif.eth.ethu_properties_get(unit, ethu_info.ethu_id)->pms[ethu_info.pm_id_in_ethu];
    pm_phys = dnx_data_nif.eth.pm_properties_get(unit, pm_i)->phys;
    /*
     * get all nif logical ports for our specific device core (0 or 1)
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, ethu_info.core, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                     &all_ports));
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, ethu_info.core, DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_PHY, 0, &all_flexe_phy_ports));
    BCM_PBMP_OR(all_ports, all_flexe_phy_ports);

    BCM_PBMP_ITER(all_ports, port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port_i, 0, &first_phy));
        /*
         * check if port first phy is in ETHU range
         */
        if (BCM_PBMP_MEMBER(pm_phys, first_phy))
        {
            BCM_PBMP_PORT_ADD(*ethu_ports, port_i);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get all master logical ports on a ETHU
 */
int
imb_ethu_master_ports_get(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t * ethu_ports)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    int first_phy;
    int lane;
    bcm_port_t port_i;
    bcm_pbmp_t all_ports, ethu_phys;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * clear output pbmp
     */
    BCM_PBMP_CLEAR(*ethu_ports);
    /*
     * get port ethu info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * get ETHU lanes
     */
    BCM_PBMP_CLEAR(ethu_phys);
    for (lane = ethu_info.first_lane_in_ethu; lane < ethu_info.first_lane_in_ethu + ethu_info.nof_lanes_in_ethu; ++lane)
    {
        BCM_PBMP_PORT_ADD(ethu_phys, lane);
    }

    /*
     * get all nif logical ports for our specific device core
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, ethu_info.core, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                     &all_ports));
    BCM_PBMP_ITER(all_ports, port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port_i, 0, &first_phy));
        /*
         * check if port first phy is in ETHU range
         */
        if (BCM_PBMP_MEMBER(ethu_phys, first_phy))
        {
            BCM_PBMP_PORT_ADD(*ethu_ports, port_i);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get number of ports on a port's ETHU
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] ethu_nof_ports - number of ports on the ETHU
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
imb_ethu_pm_nof_master_ports_get(
    int unit,
    bcm_port_t port,
    int *ethu_nof_ports)
{
    bcm_pbmp_t ethu_ports;
    int ethu_pm_nof_ports;

    SHR_FUNC_INIT_VARS(unit);
    *ethu_nof_ports = 0;

    SHR_IF_ERR_EXIT(imb_ethu_pm_master_ports_get(unit, port, &ethu_ports));

    BCM_PBMP_COUNT(ethu_ports, ethu_pm_nof_ports);

    *ethu_nof_ports = ethu_pm_nof_ports;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - map SIF instance (port) into NIF port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port. if port==BCM_PORT_INVALID, disconnect the sif instance from the NIF
 * \param [in] instace_id - statistics interface instance (port) id
 * \param [in] instace_core - the core that the sif instance belong to.
 * \param [in] connect - TRUE to connect stif instance to port, FALSE to disconnect
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
imb_ethu_port_to_stif_instance_map_set(
    int unit,
    bcm_port_t port,
    int instace_id,
    int instace_core,
    int connect)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    int ethu_id_in_core = 0;
    int first_lane_in_port = 0, ethu_select = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** if connect, get the ethu info, if disconnect, ethu params
     *  initiate to zero */
    if (connect)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
        if (ethu_info.core != instace_core)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "core mismatch between {logical port=%d,core=%d} and {sif port=%d, core=%d}",
                         port, ethu_info.core, instace_id, instace_core);
        }
        ethu_id_in_core = ethu_info.ethu_id_in_core;
        first_lane_in_port = ethu_info.first_lane_in_port;
        ethu_select = dnx_data_stif.port.ethu_id_stif_valid_get(unit, ethu_info.ethu_id)->hw_select_value;
    }
    SHR_IF_ERR_EXIT(imb_ethu_internal_port_to_stif_instance_map
                    (unit, instace_core, instace_id, ethu_id_in_core, first_lane_in_port, ethu_select));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get number of ports from a ETHU on the specified
 *        scheduler
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] sch_prio - Scheduler. see
 *        bcm_port_nif_scheduler_t
 * \param [out] nof_ports - how many ports from the ETHU use this
 *        scheduler.
 *
 * \return
 *   int - seee _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_ethu_scheduler_nof_ports_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_prio,
    int *nof_ports)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    bcm_pbmp_t ethu_ports;
    dnx_algo_port_rmc_info_t rmc;
    bcm_port_t port_i;
    int ii, ports_counter, nof_priority_groups;
    SHR_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.eth.priority_groups_nof_get(unit);
    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    /*
     * Get ETHU ports bitmap
     */
    SHR_IF_ERR_EXIT(imb_ethu_master_ports_get(unit, port, &ethu_ports));

    ports_counter = 0;
    BCM_PBMP_ITER(ethu_ports, port_i)
    {
        for (ii = 0; ii < nof_priority_groups; ++ii)
        {
            /*
             * Get logical fifo information
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port_i, 0, ii, &rmc));
            if ((rmc.rmc_id != IMB_COSQ_RMC_INVALID) && (rmc.sch_priority == sch_prio))
            {
                ++ports_counter;
            }
        }
    }
    *nof_ports = ports_counter;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the port's ETHU as active in the specified
 *        scheduler
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] sch_prio - scheduler. see
 *        bcm_port_nif_scheduler_t
 * \param [in] is_active - active indication to set.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * the active indication will be translated to the
 *   bandwidth each scheduler will get for the ETHU as
 *   follows:
 *   * TDM/High scheduler - there are 2 bits per ETHU, but
 *   whenever the ETHU is active in the scheduler both bits will
 *   be set.
 *   * Low scheduler - there are 4 bits per ETHU. the weight will
 *   be determined according to ETHU bandwidth - for every 100G
 *   the ETHU is carrying, one bit will be added in the
 *   scheduler. (when the ETHU BW is less then 100G, there will
 *   be one bit set in the scheduler)
 * \see
 *   * None
 */
static int
imb_ethu_port_scheduler_active_set(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_prio,
    uint32 is_active)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    uint32 curr_active;
    int nof_ports = 0, weight = 0, nof_channels = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    if (sch_prio == bcmPortNifSchedulerLow)
    {
        /*
         * Get the weight for the low priority scheduler
         */
        SHR_IF_ERR_EXIT(dnx_port_algo_imb_ethu_scheduler_weight_get(unit, port, is_active, &weight));
        SHR_IF_ERR_EXIT(imb_ethu_scheduler_config_set
                        (unit, ethu_info.core, ethu_info.ethu_id_in_core, sch_prio, weight));
    }
    else        /* High / TDM scheduler */
    {
        SHR_IF_ERR_EXIT(imb_ethu_port_scheduler_active_get(unit, port, sch_prio, &curr_active));
        if (is_active == curr_active)
        {
            /*
             * Nothing to do
             */
            SHR_EXIT();
        }
        else if (is_active)
        {
            /*
             * This is the first port in the ETHU that needs this priority
             */
            /*
             * for TDM / High priority - we always set the weight to the max BW - 4 bits per ETHU.
             * If the ETHU supports less bits, it will take only the supported bits.
             */
            SHR_IF_ERR_EXIT(imb_ethu_scheduler_config_set
                            (unit, ethu_info.core, ethu_info.ethu_id_in_core, sch_prio,
                             dnx_data_nif.scheduler.nof_weight_bits_get(unit)));
        }
        else if (is_active == 0)
        {
            /*
             * before disabling this ETHU in the High / TDM scheduler, we need to check if other
             * ports on the ETHU need this scheduler
             */
            SHR_IF_ERR_EXIT(imb_ethu_scheduler_nof_ports_get(unit, port, sch_prio, &nof_ports));
            /*
             * Get nof channels on the port
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, port, &nof_channels));
            if ((nof_ports == 1) && (nof_channels == 1))
            {
                SHR_IF_ERR_EXIT(imb_ethu_scheduler_config_set
                                (unit, ethu_info.core, ethu_info.ethu_id_in_core, sch_prio, 0));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure ETHU scheduler
 *
 * \param [in] unit   - chip unit id
 * \param [in] port   - logical port
 * \param [in] enable - 0:disable port, 1:enable port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *  * None
 */
int
imb_ethu_port_scheduler_config_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    uint32 max_rmcs_per_port;
    int rmc_index;
    dnx_algo_port_rmc_info_t rmc;
    int weight = 0;

    SHR_FUNC_INIT_VARS(unit);
    max_rmcs_per_port = dnx_data_nif.eth.priority_groups_nof_get(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }

        /** Set the port's ETHU as active in the specified scheduler */
        SHR_IF_ERR_EXIT(imb_ethu_port_scheduler_active_set(unit, port, rmc.sch_priority, enable));

        /** Set RMC priority (Low, Priority, TDM) */
        SHR_IF_ERR_EXIT(imb_ethu_internal_port_rmc_scheduler_config_set(unit, port, rmc.rmc_id, rmc.sch_priority));

        if (rmc.sch_priority == bcmPortNifSchedulerLow)
        {
            /** Configure RMC weight in Low priority scheduler */
            SHR_IF_ERR_EXIT(dnx_port_algo_imb_rmc_scheduler_weight_get(unit, port, &weight));
            SHR_IF_ERR_EXIT(imb_ethu_rmc_low_prio_scheduler_config_set(unit, port, rmc.rmc_id, weight));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate there are enough free RMC IDs for the new port priority
 *   configuration.
 *
 *   If number of new priority groups <= current number of priority groups
 *   then there are enough free RMC IDs.
 *   If number of new priority groups > current number of priority groups
 *   then need to check the difference against the free elements in the
 *   resource manager.
 */
static int
imb_ethu_port_priority_config_nof_free_rmc_validate(
    int unit,
    bcm_port_t port,
    int nof_new_priority_groups)
{
    int ii, max_nof_prio_groups;
    dnx_algo_port_rmc_info_t rmc;
    int nof_current_priority_groups = 0;
    int nof_free_rmc_ids = 0;
    int nof_total_priority_groups_change = 0;
    SHR_FUNC_INIT_VARS(unit);

    max_nof_prio_groups = dnx_data_nif.eth.priority_groups_nof_get(unit);

    for (ii = 0; ii < max_nof_prio_groups; ++ii)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, ii, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }

        ++nof_current_priority_groups;
    }

    nof_total_priority_groups_change = nof_new_priority_groups - nof_current_priority_groups;
    if (nof_total_priority_groups_change > 0)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_imb_rmc_id_nof_free_elements_get(unit, port, &nof_free_rmc_ids));
        if (nof_free_rmc_ids < nof_total_priority_groups_change)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Not enough free RMC IDs. Required %d, but currently have only %d.\n",
                         nof_total_priority_groups_change, nof_free_rmc_ids);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - helper function to unmap the RMCs
 *        from the physical lanes.
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
imb_ethu_port_remove_rmc_lane_unmap(
    int unit,
    bcm_port_t port)
{
    int ii, nof_priority_groups;
    dnx_algo_port_rmc_info_t rmc_arr[DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF];
    SHR_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.eth.priority_groups_nof_get(unit);

    for (ii = 0; ii < nof_priority_groups; ++ii)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, ii, &rmc_arr[ii]));
        if (rmc_arr[ii].rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        /*
         * set the RMC to invalid values
         */
        rmc_arr[ii].first_entry = 0;
        rmc_arr[ii].last_entry = 0;
        rmc_arr[ii].prd_priority = IMB_PRD_PRIORITY_ALL;
        rmc_arr[ii].thr_after_ovf = 0;
    }

    SHR_IF_ERR_EXIT(imb_ethu_port_priority_config_enable_set(unit, port, rmc_arr, nof_priority_groups, 0));

    for (ii = 0; ii < nof_priority_groups; ++ii)
    {
        if (rmc_arr[ii].rmc_id != IMB_COSQ_RMC_INVALID)
        {
            /** Free RMC ID in resource manager */
            SHR_IF_ERR_EXIT(dnx_algo_port_imb_rmc_id_free(unit, port, rmc_arr[ii].rmc_id));
        }

        rmc_arr[ii].rmc_id = IMB_COSQ_RMC_INVALID;
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_set(unit, port, 0, ii, &rmc_arr[ii]));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the priority group configuration into SW db.
 * priority group translate to a RMC (logical fifo)
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - local id
 * \param [in] priority_config - priority group specification
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * bcm_port_prio_config_t
 */
static int
imb_ethu_port_priority_config_db_update(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config)
{
    int ii, nof_prio_groups, prev_last_entry, rmc_fifo_size;
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    nof_prio_groups = priority_config->nof_priority_groups;

    prev_last_entry = -1;
    for (ii = 0; ii < nof_prio_groups; ++ii)
    {
        /*
         * copy logical fifo priorities from user configuration
         */
        rmc.sch_priority = priority_config->priority_groups[ii].sch_priority;
        rmc.prd_priority = priority_config->priority_groups[ii].source_priority;
        /*
         * allocate RMC id
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_imb_rmc_id_alloc(unit, port, &rmc.rmc_id));
        /*
         * get first end last entry of the RMC
         */
        SHR_IF_ERR_EXIT(dnx_port_algo_imb_rmc_entry_range_get
                        (unit, port, priority_config->priority_groups[ii].num_of_entries, prev_last_entry,
                         nof_prio_groups, &rmc.first_entry, &rmc.last_entry));
        prev_last_entry = rmc.last_entry;

        /*
         * get threshold after ovf
         */
        rmc_fifo_size = rmc.last_entry - rmc.first_entry + 1;
        SHR_IF_ERR_EXIT(dnx_algo_port_imb_threshold_after_ovf_get(unit, port, rmc_fifo_size, &rmc.thr_after_ovf));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_set(unit, port, 0, ii, &rmc));
    }
exit:
    SHR_FUNC_EXIT;
}

static int
imb_ethu_port_priority_config_enable_set(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc_arr,
    int nof_rmcs,
    int enable)
{
    int ii;
    int port_has_speed;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * before we start the sequence, we need to make sure all the RMC FIFOs are empty, and reset them
     */
    for (ii = 0; ii < nof_rmcs; ++ii)
    {
        if (rmc_arr[ii].rmc_id == IMB_COSQ_RMC_INVALID)
        {
            continue;
        }

        /*
         * reset RMCs of the port
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_rmc_enable_set(unit, port, 0, FALSE /** disable */ ));
    }

    /*
     * configure scheduler
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, 0, &port_has_speed));
    if (port_has_speed)
    {
        SHR_IF_ERR_EXIT(imb_ethu_port_scheduler_config_set(unit, port, enable));
    }

    /*
     * change the priority configuration
     */
    for (ii = 0; ii < nof_rmcs; ++ii)
    {
        if (rmc_arr[ii].rmc_id == IMB_COSQ_RMC_INVALID)
        {
            continue;
        }

        /*
         * Map RMC to physical port
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_rmc_to_lane_map(unit, port, &rmc_arr[ii], enable));
        /*
         * Map physical port to RMC
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_lane_to_rmc_map(unit, port, &rmc_arr[ii], enable));
        /*
         * Map priorities to RMC
         */
        SHR_IF_ERR_EXIT(imb_ethu_internal_port_rx_priority_to_rmc_map(unit, port, &rmc_arr[ii], enable));
        /*
         * Set RX threshold after overflow
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_thr_after_ovf_set(unit, port, &rmc_arr[ii]));

    }
    if (enable)
    {
        /*
         * take the RMCs out of reset
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_rmc_enable_set(unit, port, 0, TRUE /** enable */ ));

    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Map PRD priority to RMC
 */
static int
imb_ethu_port_rx_priority_to_rmc_config_set(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc_arr,
    int nof_rmcs)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * change the priority mapping
     */
    for (ii = 0; ii < nof_rmcs; ++ii)
    {
        if (rmc_arr[ii].rmc_id == IMB_COSQ_RMC_INVALID)
        {
            continue;
        }
        /*
         * Map priorities to RMC
         */
        SHR_IF_ERR_EXIT(imb_ethu_internal_port_rx_priority_to_rmc_map(unit, port, &rmc_arr[ii], TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate imb_ethu_port_priority_config_set params
 */
static int
imb_ethu_port_priority_config_verify(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config)
{
    int nof_prio_groups_supported, ii, nof_entries, prd_priority_mask;
    int prio_found[DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF] = { 0 };
    bcm_port_nif_scheduler_t sch_prio;
    SHR_FUNC_INIT_VARS(unit);

    nof_prio_groups_supported = dnx_data_nif.eth.priority_groups_nof_get(unit);

    if (priority_config->nof_priority_groups > nof_prio_groups_supported)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of priority groups %d is not supported\n",
                     priority_config->nof_priority_groups);
    }
    for (ii = 0; ii < priority_config->nof_priority_groups; ++ii)
    {
        nof_entries = priority_config->priority_groups[ii].num_of_entries;
        sch_prio = priority_config->priority_groups[ii].sch_priority;
        prd_priority_mask = priority_config->priority_groups[ii].source_priority;

        /*
         * Supported values for num_of_entries is -1 or > dnx_data_nif.eth.priority_group_nof_entries_min
         */
        if (nof_entries < dnx_data_nif.eth.priority_group_nof_entries_min_get(unit) && nof_entries != -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Number of entries %d is invalid\n", nof_entries);
        }
        if (prio_found[sch_prio])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Having two priority groups with the same scheduler priority is not supported\n");
        }
        prio_found[sch_prio] = 1;
        if (prd_priority_mask == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Priority group %d is not mapped to any source priority\n", ii);
        }
        else if (prd_priority_mask & (~IMB_PRD_PRIORITY_ALL))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Priority group %d source priority is invalid\n", ii);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#define DNX_PORT_IMB_ETHU_SCH_PRIO_INVALID (bcmPortNifSchedulerTDM+1)

/**
 * \brief - Get default priority groups configuration
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] default_rmc_arr - array of RMC configurations to be filled
 *                                with default values.
 * \param [out] nof_default_rmc - length of default_rmc array
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
imb_ethu_port_priority_config_default_get(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * default_rmc_arr,
    int *nof_default_rmc)
{
    int ii;
    int min_sch_prio_index = 0;
    int min_sch_prio = DNX_PORT_IMB_ETHU_SCH_PRIO_INVALID;
    uint32 nof_prio_groups_supported = dnx_data_nif.eth.priority_groups_nof_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    *nof_default_rmc = 0;

    /*
     * Search for miminum configured scheduler priority in the port
     */
    for (ii = 0; ii < nof_prio_groups_supported; ++ii)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, ii, &default_rmc_arr[ii]));
        if (default_rmc_arr[ii].rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /** RMC is invalid */
            continue;
        }

        /*
         * Set all prd_priority to 0, for all group priorities.
         * Later in this function the minimum group priority will
         * be assigned with all the priorities.
         */
        default_rmc_arr[ii].prd_priority = 0;

        /** update minimum scheduler priority */
        if (default_rmc_arr[ii].sch_priority < min_sch_prio)
        {
            min_sch_prio = default_rmc_arr[ii].sch_priority;
            min_sch_prio_index = ii;
        }

        /** count number of RMCs */
        ++(*nof_default_rmc);
    }

    /*
     * by default, only min scheduler should be with all PRD priorities
     */
    default_rmc_arr[min_sch_prio_index].prd_priority = IMB_PRD_PRIORITY_ALL;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get priority groups configuration from data stored
 *          in sw state.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] rmc_arr - array of RMC configurations to be filled
 *                                with user values.
 * \param [out] nof_rmc - length of rmc array
 */
static int
imb_ethu_port_priority_config_user_get(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc_arr,
    int *nof_rmc)
{
    int ii;
    uint32 nof_prio_groups_supported = dnx_data_nif.eth.priority_groups_nof_get(unit);
    int nof_prio_groups = 0;

    SHR_FUNC_INIT_VARS(unit);

    *nof_rmc = 0;

    /*
     * read from DB in order to call configuration function with RMC array
     */
    for (ii = 0; ii < nof_prio_groups_supported; ++ii)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, ii, &rmc_arr[ii]));
        if (rmc_arr[ii].rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /** RMC is invalid */
            break;
        }
        ++nof_prio_groups;
    }
    *nof_rmc = nof_prio_groups;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the given priority groups configuration
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority_config - priority groups configuration
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
imb_ethu_port_priority_config_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config)
{
    int prd_enable_mode = bcmCosqIngressPortDropEnableHardAndSoftStage;

    dnx_algo_port_rmc_info_t rmc_arr[DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF];
    int nof_rmc = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * verify data is correct
     */
    SHR_IF_ERR_EXIT(imb_ethu_port_priority_config_verify(unit, port, priority_config));

    /*
     * Validate there are enough free RMC IDs for the new priority configuration
     */
    SHR_IF_ERR_EXIT(imb_ethu_port_priority_config_nof_free_rmc_validate
                    (unit, port, priority_config->nof_priority_groups));

    /*
     * remove current RMC mapping
     */
    SHR_IF_ERR_EXIT(imb_ethu_port_remove_rmc_lane_unmap(unit, port));

    /*
     * update new configuration on SW DB
     */
    SHR_IF_ERR_EXIT(imb_ethu_port_priority_config_db_update(unit, port, priority_config));

    
    if (!soc_is(unit, J2P_DEVICE))
    {
        /*
         * Write configuration to HW according to PRD mode:
         * PRD is disabled - write default configurations to HW. User configurations
         *                   will be written when PRD will be enabled.
         * PRD is enabled - write user configurations to HW.
         */

        SHR_IF_ERR_EXIT(imb_ethu_prd_enable_get(unit, port, &prd_enable_mode));

        if (prd_enable_mode == bcmCosqIngressPortDropDisable)
        {
            /** Get default priority configuration */
            SHR_IF_ERR_EXIT(imb_ethu_port_priority_config_default_get(unit, port, rmc_arr, &nof_rmc));
        }
        else
        {
            /** Get user priority configuration which is stored in sw state db */
            SHR_IF_ERR_EXIT(imb_ethu_port_priority_config_user_get(unit, port, rmc_arr, &nof_rmc));
        }
    }
    else
    {
        /** Get default priority configuration */
        SHR_IF_ERR_EXIT(imb_ethu_port_priority_config_default_get(unit, port, rmc_arr, &nof_rmc));
    }

    /*
     * Set configuration to HW
     */
    SHR_IF_ERR_EXIT(imb_ethu_port_priority_config_enable_set(unit, port, rmc_arr, nof_rmc, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the given priority groups configuration
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] priority_config - priority groups configuration
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
imb_ethu_port_priority_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_prio_config_t * priority_config)
{
    int ii, nof_priority_groups;
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.eth.priority_groups_nof_get(unit);

    priority_config->nof_priority_groups = 0;

    for (ii = 0; ii < nof_priority_groups; ++ii)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, ii, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        priority_config->priority_groups[ii].sch_priority = rmc.sch_priority;
        priority_config->priority_groups[ii].source_priority = rmc.prd_priority;
        priority_config->priority_groups[ii].num_of_entries = rmc.last_entry - rmc.first_entry + 1;
        ++priority_config->nof_priority_groups;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get ETHU current bandwidth
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] is_port_include - indication whether to include /
 *        exclude specified port in the BW calculation
 * \param [out] bandwidth - BW calculated
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
imb_ethu_bandwidth_get(
    int unit,
    bcm_port_t port,
    int is_port_include,
    int *bandwidth)
{
    bcm_pbmp_t ethu_ports;
    bcm_port_t port_i;
    int bw_sum, port_speed;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_master_ports_get(unit, port, &ethu_ports));

    bw_sum = 0;
    if (!is_port_include)
    {
        /*
         * don't include the current port in the calculation (means port is in process of being removed)
         */
        BCM_PBMP_PORT_REMOVE(ethu_ports, port);
    }
    BCM_PBMP_ITER(ethu_ports, port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port_i, 0, &port_speed));
        bw_sum += port_speed;
    }
    *bandwidth = bw_sum;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See imb_ethu.h
 */
int
imb_ethu_prd_drop_count_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_priority,
    uint64 *count)
{
    dnx_algo_port_rmc_info_t rmc;
    uint32 max_rmcs_per_port, rmc_found = 0;
    int rmc_index;
    SHR_FUNC_INIT_VARS(unit);

    max_rmcs_per_port = dnx_data_nif.eth.priority_groups_nof_get(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc));
        if ((rmc.rmc_id != IMB_COSQ_RMC_INVALID) && (rmc.sch_priority == sch_priority))
        {
            SHR_IF_ERR_EXIT(imb_ethu_prd_drop_count_hw_get(unit, port, rmc.rmc_id, count));
            rmc_found = 1;
            break;
        }
    }

    if (!rmc_found)
    {
        COMPILER_64_SET(*count, 0, 0);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Get NIF RX fifo status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] sch_priority - nif schedule priority
 * \param [out] max_occupancy - indicate the MAX fullness level of the fifo
 * \param [out] fifo_level - indicate the current fullness level of the fifo
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
imb_ethu_port_rx_fifo_status_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_priority,
    uint32 *max_occupancy,
    uint32 *fifo_level)
{

    dnx_algo_port_rmc_info_t rmc_info;
    uint32 max_rmcs_per_port;
    int rmc_index, rmc_found = 0;
    SHR_FUNC_INIT_VARS(unit);

    max_rmcs_per_port = dnx_data_nif.eth.priority_groups_nof_get(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc_info));
        if ((rmc_info.rmc_id != IMB_COSQ_RMC_INVALID) && (rmc_info.sch_priority == sch_priority))
        {
            SHR_IF_ERR_EXIT(imb_ethu_internal_port_rx_fifo_status_get
                            (unit, port, rmc_info.rmc_id, max_occupancy, fifo_level));
            rmc_found = 1;
            break;
        }
    }
    if (!rmc_found)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Sch Priority %d is not mapped to any of the port's priority group\n", sch_priority);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get NIF TX fifo status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] max_occupancy - indicate the MAX fullness level of the fifo
 * \param [out] fifo_level - indicate the current fullness level of the fifo
 * \param [out] pm_credits - indicate the credits from PM tx buffer
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
imb_ethu_port_tx_fifo_status_get(
    int unit,
    bcm_port_t port,
    uint32 *max_occupancy,
    uint32 *fifo_level,
    uint32 *pm_credits)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_internal_port_tx_fifo_status_get(unit, port, max_occupancy, fifo_level, pm_credits));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD hard stage priority map.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] map - map type:
 *  see bcm_cosq_ingress_port_drop_map_t.
 * \param [in] key - key index for the map (depends on the map
 *        type)
 * \param [in] priority - priority value to set to the key in
 *        the map.
 * \param [in] is_tdm - TDM signal
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
imb_ethu_prd_map_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 priority,
    uint32 is_tdm)
{
    uint32 dp, tc, dscp, pcp, dei, exp;
    SHR_FUNC_INIT_VARS(unit);

    switch (map)
    {
        case bcmCosqIngressPortDropTmTcDpPriorityTable:
            IMB_COSQ_PRD_TM_KEY_TC_GET(key, tc);
            IMB_COSQ_PRD_TM_KEY_DP_GET(key, dp);
            SHR_IF_ERR_EXIT(imb_ethu_prd_map_tm_tc_dp_hw_set(unit, port, tc, dp, priority, is_tdm));
            break;
        case bcmCosqIngressPortDropIpDscpToPriorityTable:
            IMB_COSQ_PRD_IP_KEY_DSCP_GET(key, dscp);
            SHR_IF_ERR_EXIT(imb_ethu_prd_map_ip_dscp_hw_set(unit, port, dscp, priority, is_tdm));
            break;
        case bcmCosqIngressPortDropEthPcpDeiToPriorityTable:
            IMB_COSQ_PRD_ETH_KEY_PCP_GET(key, pcp);
            IMB_COSQ_PRD_ETH_KEY_DEI_GET(key, dei);
            SHR_IF_ERR_EXIT(imb_ethu_prd_map_eth_pcp_dei_hw_set(unit, port, pcp, dei, priority, is_tdm));
            break;
        case bcmCosqIngressPortDropMplsExpToPriorityTable:
            IMB_COSQ_PRD_MPLS_KEY_EXP_GET(key, exp);
            SHR_IF_ERR_EXIT(imb_ethu_prd_map_mpls_exp_hw_set(unit, port, exp, priority, is_tdm));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD Map %d\n", map);
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get priority value mapped to a key in one of the PRD
 *        priority tables
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] map - type of map. see
 *        bcm_cosq_ingress_port_drop_map_t
 * \param [in] key - key index for the map (depends on the map
 *        type)
 * \param [out] priority - returned priority value
 * \param [out] is_tdm - TDM signal
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ethu_prd_map_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 *priority,
    uint32 *is_tdm)
{
    uint32 dp, tc, dscp, pcp, dei, exp;
    SHR_FUNC_INIT_VARS(unit);

    *is_tdm = 0;
    switch (map)
    {
        case bcmCosqIngressPortDropTmTcDpPriorityTable:
            IMB_COSQ_PRD_TM_KEY_TC_GET(key, tc);
            IMB_COSQ_PRD_TM_KEY_DP_GET(key, dp);
            SHR_IF_ERR_EXIT(imb_ethu_prd_map_tm_tc_dp_hw_get(unit, port, tc, dp, priority, is_tdm));
            break;
        case bcmCosqIngressPortDropIpDscpToPriorityTable:
            IMB_COSQ_PRD_IP_KEY_DSCP_GET(key, dscp);
            SHR_IF_ERR_EXIT(imb_ethu_prd_map_ip_dscp_hw_get(unit, port, dscp, priority, is_tdm));
            break;
        case bcmCosqIngressPortDropEthPcpDeiToPriorityTable:
            IMB_COSQ_PRD_ETH_KEY_PCP_GET(key, pcp);
            IMB_COSQ_PRD_ETH_KEY_DEI_GET(key, dei);
            SHR_IF_ERR_EXIT(imb_ethu_prd_map_eth_pcp_dei_hw_get(unit, port, pcp, dei, priority, is_tdm));
            break;
        case bcmCosqIngressPortDropMplsExpToPriorityTable:
            IMB_COSQ_PRD_MPLS_KEY_EXP_GET(key, exp);
            SHR_IF_ERR_EXIT(imb_ethu_prd_map_mpls_exp_hw_get(unit, port, exp, priority, is_tdm));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD Map %d\n", map);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set configurable ether type to be recognized by the
 *        PRD Parser
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code to recognize
 *        the new ether type (1-6)
 * \param [in] ether_type_val - ether type value
 *
 * \return
 *   int  -see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ethu_prd_custom_ether_type_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_val)
{
    uint32 ether_type_code_max;
    SHR_FUNC_INIT_VARS(unit);

    ether_type_code_max = dnx_data_nif.prd.custom_ether_type_code_max_get(unit);

    SHR_IF_ERR_EXIT(imb_ethu_prd_custom_ether_type_hw_set(unit, port, ether_type_code - 1, ether_type_val));

    if (ether_type_code == ether_type_code_max)
    {
        /*
         * The last configurable ether type (with ether type code 7) is saved for TM port types. the way to save it is
         * to always configure ether type code 6 and ether type code 7 to the same value. so if the user want to
         * configure ether type code 6, we copy the same value to ether type code 7
         */
        SHR_IF_ERR_EXIT(imb_ethu_prd_custom_ether_type_hw_set(unit, port, ether_type_code, ether_type_val));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get ether type mapped to a specific configurable
 *        ether type  code
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - configurable ether type code to
 *        get (1-6)
 * \param [in] ether_type_val - eter type value mapped to the
 *        given ether type code.
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
imb_ethu_prd_custom_ether_type_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_prd_custom_ether_type_hw_get(unit, port, ether_type_code - 1, ether_type_val));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set an entry to the TCAM table in the PRD soft stage
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] key_index - index in the TCAM
 * \param [in] entry_info - info of the TCAM entry
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
imb_ethu_prd_flex_key_entry_set(
    int unit,
    bcm_port_t port,
    uint32 key_index,
    const dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_prd_tcam_entry_hw_set(unit, port, key_index, entry_info));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get an entry from the PRD soft stage TCAM table
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] key_index - index in the TCAM table
 * \param [in] entry_info - returned entry info.
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
imb_ethu_prd_flex_key_entry_get(
    int unit,
    bcm_port_t port,
    uint32 key_index,
    dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_prd_tcam_entry_hw_get(unit, port, key_index, entry_info));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set control frame properties to be recognized by the
 *        PRD. if a control packet is recognized (each packet is
 *        compared against all control planes defined for the
 *        ETHU), it gets the highest priority
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] control_frame_index - index of control frame to
 *        configure (0-3)
 * \param [in] control_frame_config - control frame properties
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
imb_ethu_prd_control_frame_set(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    const bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_prd_control_frame_hw_set(unit, port, control_frame_index, control_frame_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get Control frame properties recognized by the PRD
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] control_frame_index - index of control frame to
 *        get
 * \param [out] control_frame_config - returned control frame
 *        properties
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
imb_ethu_prd_control_frame_get(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_prd_control_frame_hw_get(unit, port, control_frame_index, control_frame_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set MPLS special label properties. the special
 *        labels are compared globally against all the MPLS ports
 *        of the same ETHU.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] label_index - label index to set (0-3)
 * \param [in] label_config - special label configuration.
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
imb_ethu_prd_mpls_special_label_set(
    int unit,
    bcm_port_t port,
    uint32 label_index,
    const bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_prd_mpls_special_label_hw_set(unit, port, label_index, label_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get MPLS special label properties
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] label_index - label index to get (0-3)
 * \param [out] label_config - label properties
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
imb_ethu_prd_mpls_special_label_get(
    int unit,
    bcm_port_t port,
    uint32 label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_prd_mpls_special_label_hw_get(unit, port, label_index, label_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set all fixed ether types sizes. (in bytes). this
 *        function is called only once per port, when the PRD is
 *        enabled (fixed ether types should always have the
 *        same size). the configurable ether types sizes are set
 *        by the user, see API
 *        imb_ethu_prd_flex_key_construct_set
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code
 * \param [in] ether_type_size - ether type size
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
imb_ethu_prd_ether_type_size_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_prd_ether_type_size_hw_set(unit, port, ether_type_code, ether_type_size));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get ether types sizes. (in bytes).
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code
 * \param [out] ether_type_size - ether type size
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
imb_ethu_prd_ether_type_size_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_prd_ether_type_size_hw_get(unit, port, ether_type_code, ether_type_size));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable PRD Hard stage per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable indication
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
imb_ethu_prd_hard_stage_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    uint32 max_rmcs_per_port;
    int rmc_index;
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    max_rmcs_per_port = dnx_data_nif.eth.priority_groups_nof_get(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        SHR_IF_ERR_EXIT(imb_ethu_prd_hard_stage_enable_hw_set(unit, port, rmc.rmc_id, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get enable indication per port for the PRD hard
 *        stage
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable indication
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
imb_ethu_prd_hard_stage_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    uint32 max_rmcs_per_port;
    int rmc_index;
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    max_rmcs_per_port = dnx_data_nif.eth.priority_groups_nof_get(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        SHR_IF_ERR_EXIT(imb_ethu_prd_hard_stage_enable_hw_get(unit, port, rmc.rmc_id, enable));
        break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set TPID value for the port profile
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tpid_index - tpid index (0-3). per port there can
 *        be 4 identified TPIDs
 * \param [in] tpid_value - TPID value
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
imb_ethu_prd_tpid_set(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 tpid_value)
{
    uint32 prd_profile;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    /*
     * Get prd profile for the given port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

    SHR_IF_ERR_EXIT(imb_ethu_prd_tpid_hw_set
                    (unit, ethu_info.core, ethu_info.ethu_id_in_core, prd_profile, tpid_index, tpid_value));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get TPID value for the port profile
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tpid_index - tpid index (0-3). per port there can
 *        be 4 identified TPIDs
 * \param [in] tpid_value - TPID value
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
imb_ethu_prd_tpid_get(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 *tpid_value)
{
    uint32 prd_profile;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    /*
     * Get prd profile for the given port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

    SHR_IF_ERR_EXIT(imb_ethu_prd_tpid_hw_get
                    (unit, ethu_info.core, ethu_info.ethu_id_in_core, prd_profile, tpid_index, tpid_value));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set different PRD properties
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] property - PRD property to set:
 *  see enum imb_prd_properties_t.
 * \param [in] val - value of the property to set.
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
imb_ethu_prd_properties_set(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 val)
{
    uint32 prd_profile;
    dnx_algo_port_ethu_access_info_t ethu_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get prd profile for the given port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    switch (property)
    {
        case imbImbPrdIgnoreIpDscp:
            SHR_IF_ERR_EXIT(imb_ethu_prd_ignore_ip_dscp_hw_set
                            (unit, ethu_info.core, ethu_info.ethu_id_in_core, prd_profile, val));
            break;
        case imbImbPrdIgnoreMplsExp:
            SHR_IF_ERR_EXIT(imb_ethu_prd_ignore_mpls_exp_hw_set
                            (unit, ethu_info.core, ethu_info.ethu_id_in_core, prd_profile, val));
            break;
        case imbImbPrdIgnoreInnerTag:
            SHR_IF_ERR_EXIT(imb_ethu_prd_ignore_inner_tag_hw_set
                            (unit, ethu_info.core, ethu_info.ethu_id_in_core, prd_profile, val));
            break;
        case imbImbPrdIgnoreOuterTag:
            SHR_IF_ERR_EXIT(imb_ethu_prd_ignore_outer_tag_hw_set
                            (unit, ethu_info.core, ethu_info.ethu_id_in_core, prd_profile, val));
            break;
        case imbImbPrdDefaultPriority:
            SHR_IF_ERR_EXIT(imb_ethu_prd_default_priority_hw_set
                            (unit, ethu_info.core, ethu_info.ethu_id_in_core, prd_profile, val));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD property %d\n", property);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get PRD property value
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] property - PRD property to get.
 * see enum imb_prd_properties_t.
 * \param [out] val - returned property value
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
imb_ethu_prd_properties_get(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 *val)
{
    uint32 prd_profile;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get prd profile for the given port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    switch (property)
    {
        case imbImbPrdIgnoreIpDscp:
            SHR_IF_ERR_EXIT(imb_ethu_prd_ignore_ip_dscp_hw_get
                            (unit, ethu_info.core, ethu_info.ethu_id_in_core, prd_profile, val));
            break;
        case imbImbPrdIgnoreMplsExp:
            SHR_IF_ERR_EXIT(imb_ethu_prd_ignore_mpls_exp_hw_get
                            (unit, ethu_info.core, ethu_info.ethu_id_in_core, prd_profile, val));
            break;
        case imbImbPrdIgnoreInnerTag:
            SHR_IF_ERR_EXIT(imb_ethu_prd_ignore_inner_tag_hw_get
                            (unit, ethu_info.core, ethu_info.ethu_id_in_core, prd_profile, val));
            break;
        case imbImbPrdIgnoreOuterTag:
            SHR_IF_ERR_EXIT(imb_ethu_prd_ignore_outer_tag_hw_get
                            (unit, ethu_info.core, ethu_info.ethu_id_in_core, prd_profile, val));
            break;
        case imbImbPrdDefaultPriority:
            SHR_IF_ERR_EXIT(imb_ethu_prd_default_priority_hw_get
                            (unit, ethu_info.core, ethu_info.ethu_id_in_core, prd_profile, val));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD property %d\n", property);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - enable/disable PRD for the port. this call should be
 *        last, after the PRD is already configured.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable_mode - enable modes for the PRD:
 * bcmCosqIngressPortDropDisable
 * bcmCosqIngressPortDropEnableHardStage
 * bcmCosqIngressPortDropEnableHardAndSoftStage
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * it is required to call header type set before enabling
 *     the PRD.
 * \see
 *   * None
 */
int
imb_ethu_prd_enable_set(
    int unit,
    bcm_port_t port,
    int enable_mode)
{
    uint32 en_soft_stage_eth, en_soft_stage_tm;
    uint32 is_eth_port = 0, is_tm_port = 0;
    uint32 port_type = 0;
    uint32 outer_tag_size = 0;
    int tc_offset = 0;
    int dp_offset = 0;
    uint32 prd_profile;
    dnx_algo_port_ethu_access_info_t ethu_info;
    dnx_algo_port_rmc_info_t rmc_arr[DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF];
    int nof_rmc = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get prd profile for the given port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    if (enable_mode == bcmCosqIngressPortDropDisable)
    {
        /*
         * Disable Hard stage
         */
        SHR_IF_ERR_EXIT(imb_ethu_prd_hard_stage_enable_set(unit, port, 0));

        /*
         * Disable Soft Stage
         */
        SHR_IF_ERR_EXIT(imb_ethu_prd_soft_stage_enable_hw_set
                        (unit, ethu_info.core, ethu_info.ethu_id_in_core, prd_profile, 0, 0));

        /*
         * Since PRD is now disabled, need to set the port priority
         * configuration to default state.
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_priority_config_default_get(unit, port, rmc_arr, &nof_rmc));
        SHR_IF_ERR_EXIT(imb_ethu_port_rx_priority_to_rmc_config_set(unit, port, rmc_arr, nof_rmc));
    }
    else
    {
        /*
         * Since PRD was disabled until now, the user's port priority
         * configurations was written only to sw state db.
         * Since now PRD will be enabled, we want to write the user's
         * configuration into HW.
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_priority_config_user_get(unit, port, rmc_arr, &nof_rmc));
        SHR_IF_ERR_EXIT(imb_ethu_port_rx_priority_to_rmc_config_set(unit, port, rmc_arr, nof_rmc));

        /*
         * set TC/DP offsets for ITMH/FTMH
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_itmh_offsets_get(unit, port, &tc_offset, &dp_offset));
        SHR_IF_ERR_EXIT(imb_ethu_prd_itmh_offsets_hw_set(unit, port, tc_offset, dp_offset));

        SHR_IF_ERR_EXIT(dnx_cosq_prd_ftmh_offsets_get(unit, port, &tc_offset, &dp_offset));
        SHR_IF_ERR_EXIT(imb_ethu_prd_ftmh_offsets_hw_set(unit, port, tc_offset, dp_offset));

        /*
         * Get is port extender
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_outer_tag_size_get(unit, port, &outer_tag_size));
        SHR_IF_ERR_EXIT(imb_ethu_prd_outer_tag_size_hw_set
                        (unit, ethu_info.core, ethu_info.ethu_id_in_core, prd_profile, outer_tag_size));

        /*
         * Set Ether type sizes for fixed Ether types
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_ether_type_size_set(unit, port));

        /*
         * Set prd port type
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_port_type_get(unit, port, &port_type, &is_eth_port, &is_tm_port));
        SHR_IF_ERR_EXIT(imb_ethu_prd_port_type_hw_set
                        (unit, ethu_info.core, ethu_info.ethu_id_in_core, prd_profile, port_type));

        /*
         * Enable soft stage
         */
        en_soft_stage_eth = ((enable_mode == bcmCosqIngressPortDropEnableHardAndSoftStage) && (is_eth_port)) ? 1 : 0;
        en_soft_stage_tm = ((enable_mode == bcmCosqIngressPortDropEnableHardAndSoftStage) && (is_tm_port)) ? 1 : 0;
        SHR_IF_ERR_EXIT(imb_ethu_prd_soft_stage_enable_hw_set
                        (unit, ethu_info.core, ethu_info.ethu_id_in_core, prd_profile, en_soft_stage_eth,
                         en_soft_stage_tm));

        /*
         * Enable hard stage
         */
        SHR_IF_ERR_EXIT(imb_ethu_prd_hard_stage_enable_set(unit, port, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get PRD enable mode per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable_mode - PRD enable modes:
 *  bcmCosqIngressPortDropDisable
 *  bcmCosqIngressPortDropEnableHardStage
 *  bcmCosqIngressPortDropEnableHardAndSoftStage
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
imb_ethu_prd_enable_get(
    int unit,
    bcm_port_t port,
    int *enable_mode)
{
    uint32 is_eth_port = 0, is_tm_port = 0, port_type, en_hard_stage = 0, en_soft_stage_eth, en_soft_stage_tm;
    uint32 prd_profile;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    SHR_IF_ERR_EXIT(imb_ethu_prd_hard_stage_enable_get(unit, port, &en_hard_stage));

    if (en_hard_stage)
    {
        /*
         * Get prd profile for the given port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));
        /*
         * The expectation is that eth soft stage will only be set for eth ports, and tm soft stage will only be set
         * for tm ports. but we check for the port type just in case
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_port_type_get(unit, port, &port_type, &is_eth_port, &is_tm_port));

        SHR_IF_ERR_EXIT(imb_ethu_prd_soft_stage_enable_hw_get
                        (unit, ethu_info.core, ethu_info.ethu_id_in_core, prd_profile, &en_soft_stage_eth,
                         &en_soft_stage_tm));

        if ((en_soft_stage_eth && is_eth_port) || (en_soft_stage_tm && is_tm_port))
        {
            *enable_mode = bcmCosqIngressPortDropEnableHardAndSoftStage;
        }
        else
        {
            *enable_mode = bcmCosqIngressPortDropEnableHardStage;
        }
    }
    else
    {
        *enable_mode = bcmCosqIngressPortDropDisable;
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_ethu_prd_tcam_entry_key_offset_base_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_base)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_prd_tcam_entry_key_offset_base_hw_set(unit, port, ether_type_code, offset_base));

exit:
    SHR_FUNC_EXIT;

}
int
imb_ethu_prd_tcam_entry_key_offset_base_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *offset_base)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_prd_tcam_entry_key_offset_base_hw_get(unit, port, ether_type_code, offset_base));

exit:
    SHR_FUNC_EXIT;

}

int
imb_ethu_prd_tcam_entry_key_offset_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 offset_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_prd_tcam_entry_key_offset_hw_set(unit, port, ether_type_code, offset_index, offset_value));

exit:
    SHR_FUNC_EXIT;

}

int
imb_ethu_prd_tcam_entry_key_offset_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 *offset_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_prd_tcam_entry_key_offset_hw_get(unit, port, ether_type_code, offset_index, offset_value));

exit:
    SHR_FUNC_EXIT;
}
/**
 * See imb_ethu.h
 */
int
imb_ethu_port_fec_speed_validate(
    int unit,
    bcm_port_t port,
    bcm_port_phy_fec_t fec_type,
    int speed)
{
    int nof_lanes, idx;
    int is_valid, master_port;
    portmod_dispatch_type_t pm_type;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Get number of lanes for the port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &nof_lanes));
    /** get portmod master port in case interface is channalized (portmod layer handles its own master) */
    SHR_IF_ERR_EXIT(portmod_port_pm_type_get(unit, port, &master_port, &pm_type));

    is_valid = FALSE;
    /**
     * Go over all supported NIF interfaces for the device
     * and mark the port configuration as valid only if it passes all the checks
     */
    for (idx = 0; idx < dnx_data_nif.eth.supported_interfaces_info_get(unit)->key_size[0]; idx++)
    {
        if (dnx_data_nif.eth.supported_interfaces_get(unit, idx)->is_valid)
        {
            if (pm_type == dnx_data_nif.eth.supported_interfaces_get(unit, idx)->dispatch_type)
            {
                if (speed == dnx_data_nif.eth.supported_interfaces_get(unit, idx)->speed)
                {
                    if (nof_lanes == dnx_data_nif.eth.supported_interfaces_get(unit, idx)->nof_lanes)
                    {
                        /**
                         * Fec type is protected by soc_dnxc_port_resource_validate when in set API
                         * The fec type validity skip can be applied only in port resource default request API
                         * where only the speed should be validate
                         */
                        if (fec_type == dnx_data_nif.eth.supported_interfaces_get(unit, idx)->fec_type
                            || fec_type == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
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
                     "port %d: The combination of FEC type %d, speed %d and lane numbers %d is not supported! \n ",
                     port, fec_type, speed, nof_lanes);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See imb_ethu.h
 */
int
imb_ethu_pfc_deadlock_counters_reset(
    int unit,
    const imb_create_info_t * imb_info)
{
    int nof_ethus_per_core, ethu_id_in_core, lane;
    int lane_in_core, first_lane_in_ethu, nof_lanes_in_ethu;
    bcm_core_t core;
    SHR_FUNC_INIT_VARS(unit);

    nof_ethus_per_core = dnx_data_nif.eth.ethu_nof_per_core_get(unit);
    core = imb_info->inst_id >= nof_ethus_per_core ? 1 : 0;
    ethu_id_in_core = imb_info->inst_id % nof_ethus_per_core;
    SHR_IF_ERR_EXIT(dnx_algo_ethu_id_lane_info_get
                    (unit, core, ethu_id_in_core, &first_lane_in_ethu, &nof_lanes_in_ethu));

    for (lane = first_lane_in_ethu; lane < (first_lane_in_ethu + nof_lanes_in_ethu); lane++)
    {
        /**
         * Reset PFC counters
         */
        lane_in_core = lane % dnx_data_nif.phys.nof_phys_per_core_get(unit);
        SHR_IF_ERR_EXIT(imb_ethu_internal_pfc_deadlock_counters_reset(unit, core, lane_in_core));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset per port instrumentation counters for PFC deadlock detection mechanism
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ethu_port_pfc_deadlock_counters_reset(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    /**
     * Reset PFC counters
     */
    SHR_IF_ERR_EXIT(imb_ethu_internal_pfc_deadlock_counters_reset(unit, ethu_info.core, ethu_info.lane_in_core));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset per ETHU instrumentation counters
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_info - IMB info
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ethu_instru_counters_reset(
    int unit,
    const imb_create_info_t * imb_info)
{
    int nof_ethus_per_core, ethu_id_in_core;
    bcm_core_t core;
    SHR_FUNC_INIT_VARS(unit);

    nof_ethus_per_core = dnx_data_nif.eth.ethu_nof_per_core_get(unit);
    core = imb_info->inst_id >= nof_ethus_per_core ? 1 : 0;
    ethu_id_in_core = imb_info->inst_id % nof_ethus_per_core;

    /** Set HW to put in/out of reset all instrumentation counters of the ETHU */
    SHR_IF_ERR_EXIT(imb_ethu_internal_instru_counters_reset(unit, core, ethu_id_in_core));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the debug info for PFC deadlock detection mechanism
 *
 * \param [in] unit - chip unit id
 * \param [in] ethu_id - ETHU ID
 * \param [out] ethu_port_num - port number internal for the ETHU ID
 *              of the port that triggered the PFC deadlock detection
 * \param [out] pfc_num - PFC that triggered the PFC deadlock detection
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ethu_pfc_deadlock_debug_info_get(
    int unit,
    int ethu_id,
    int *ethu_port_num,
    int *pfc_num)
{
    int ethu_core, ethu_id_in_core;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * Get ETHU core and ETHU in core
     */
    ethu_id_in_core = ethu_id % (dnx_data_nif.eth.ethu_nof_get(unit) / dnx_data_device.general.nof_cores_get(unit));
    ethu_core = ethu_id / (dnx_data_nif.eth.ethu_nof_get(unit) / dnx_data_device.general.nof_cores_get(unit));

    /**
     * Get the debug info
     */
    SHR_IF_ERR_EXIT(imb_ethu_internal_pfc_deadlock_debug_info_get
                    (unit, ethu_core, ethu_id_in_core, ethu_port_num, pfc_num));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the max PFC duration for PFC deadlock detection mechanism
 *
 * \param [in] unit - chip unit id
 * \param [in] ethu_id - ETHU ID
 * \param [in] max_duration - max duration of the PFC
 *             that will trigger the PFC deadlock recovery
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ethu_pfc_deadlock_max_duration_set(
    int unit,
    int ethu_id,
    uint32 max_duration)
{
    int ethu_core, ethu_id_in_core;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * Get ETHU core and ETHU in core
     */
    ethu_id_in_core = ethu_id % (dnx_data_nif.eth.ethu_nof_get(unit) / dnx_data_device.general.nof_cores_get(unit));
    ethu_core = ethu_id / (dnx_data_nif.eth.ethu_nof_get(unit) / dnx_data_device.general.nof_cores_get(unit));

    /**
     * Set max_duration
     */
    SHR_IF_ERR_EXIT(imb_ethu_internal_pfc_deadlock_max_duration_set(unit, ethu_core, ethu_id_in_core, max_duration));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the max PFC duration for PFC deadlock detection mechanism
 *
 * \param [in] unit - chip unit id
 * \param [in] ethu_id - ETHU ID
 * \param [out] max_duration - max duration of the PFC
 *             that will trigger the PFC deadlock recovery
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ethu_pfc_deadlock_max_duration_get(
    int unit,
    int ethu_id,
    uint32 *max_duration)
{
    int ethu_core, ethu_id_in_core;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * Get ETHU core and ETHU in core
     */
    ethu_id_in_core = ethu_id % (dnx_data_nif.eth.ethu_nof_get(unit) / dnx_data_device.general.nof_cores_get(unit));
    ethu_core = ethu_id / (dnx_data_nif.eth.ethu_nof_get(unit) / dnx_data_device.general.nof_cores_get(unit));

    /**
     * Get max_duration
     */
    SHR_IF_ERR_EXIT(imb_ethu_internal_pfc_deadlock_max_duration_get(unit, ethu_core, ethu_id_in_core, max_duration));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure start tx for RMC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tx_thr - start tx for RMC
 * \param [in] enable - Enable or disable start tx
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
imb_ethu_port_rx_start_tx_thr_set(
    int unit,
    bcm_port_t port,
    int tx_thr,
    int enable)
{
    int i, nof_priority_groups;
    dnx_algo_port_rmc_info_t rmc;

    SHR_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.flexe.priority_groups_nof_get(unit);
    for (i = 0; i < nof_priority_groups; ++i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, i, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        SHR_IF_ERR_EXIT(imb_ethu_internal_port_rx_start_tx_thr_set(unit, port, rmc.rmc_id, tx_thr, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the credit counter for RMC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tx_speed - The speed for target TMC
 * \param [in] enable - Enable or disable start tx
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
imb_ethu_port_rx_credit_counter_set(
    int unit,
    bcm_port_t port,
    int tx_speed,
    int enable)
{
    int credit_val, i, nof_priority_groups;
    dnx_algo_port_rmc_info_t rmc;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_imb_feu_tx_credit_val_get(unit, tx_speed, &credit_val));
    nof_priority_groups = dnx_data_nif.flexe.priority_groups_nof_get(unit);
    for (i = 0; i < nof_priority_groups; ++i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, i, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        SHR_IF_ERR_EXIT(imb_ethu_port_rx_credit_counter_hw_set(unit, port, rmc.rmc_id, credit_val, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See imb_ethu.h
 */

int
imb_ethu_port_l1_mismatch_rate_rx_config_set(
    int unit,
    bcm_port_t port,
    int tx_speed,
    int enable)
{
    int idx, rx_speed, tx_thr, is_found = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, DNX_ALGO_PORT_SPEED_F_RX, &rx_speed));
    /*
     * Check if the speed combination is legal
     */
    for (idx = 0; idx < dnx_data_nif.flexe.l1_mismatch_rate_table_info_get(unit)->key_size[0]; idx++)
    {
        if ((rx_speed == dnx_data_nif.flexe.l1_mismatch_rate_table_get(unit, idx)->eth_speed) &&
            (tx_speed == dnx_data_nif.flexe.l1_mismatch_rate_table_get(unit, idx)->client_speed))
        {
            is_found = 1;
            break;
        }
    }
    if (is_found)
    {
        /*
         * Configure credit counter
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_rx_credit_counter_set(unit, port, tx_speed, enable));
        /*
         * Configure start tx
         */
        if (rx_speed < tx_speed)
        {
            tx_thr = dnx_data_nif.flexe.l1_mismatch_rate_table_get(unit, idx)->tx_thr;
            SHR_IF_ERR_EXIT(imb_ethu_port_rx_start_tx_thr_set(unit, port, tx_thr, enable));
        }
    }
    else if (!dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_disable_l1_mismatch_rate_checking))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "L1 mismatch rate switching doesn't support client speed %d and eth speed %d.\n",
                     tx_speed, rx_speed);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure power down of ETHU
 */
int
imb_ethu_port_power_down_set(
    int unit,
    bcm_port_t port,
    int power_down)
{
    dnx_algo_port_ethu_access_info_t ethu_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    SHR_IF_ERR_EXIT(imb_ethu_power_down_set(unit, ethu_info.core, ethu_info.ethu_id_in_core, power_down));

exit:
    SHR_FUNC_EXIT;
}
