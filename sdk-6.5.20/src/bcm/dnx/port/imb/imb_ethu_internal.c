/** \file imb_ethu_internal.c
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
#include <soc/drv.h> /** CDPORT_BLOCK */
#include <soc/cmic.h>
#include <soc/mcm/memregs.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress_port.h>
#include <soc/portmod/portmod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_imb_types.h>

#include "imb_ethu_internal.h"

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/**
 *
 * see .h file
 */

int
imb_ethu_tdm_mask_hw_set(
    int unit,
    int core,
    int ethu_id,
    uint32 value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_RX_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);

    /*
     *  Set value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MASK_TDM_INDICATION, INST_SINGLE, value);
    /*
     * Commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * See imb_ethu_internal.h
 */
int
imb_ethu_tx_data_to_pm_enable_hw_set(
    int unit,
    bcm_core_t core,
    int lane,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_TX_LANE_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CORE, lane);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STOP_DATA, INST_SINGLE, enable ? 0 : 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the ETHU in the specified Scheduler
 *
 * see .h file
 */
int
imb_ethu_scheduler_config_hw_set(
    int unit,
    bcm_core_t core,
    uint32 ethu_id,
    uint32 sch_prio,
    uint32 sch_weight)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_SCHEDULER_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_PRIORITY, sch_prio);

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_WEIGHT, INST_SINGLE, sch_weight);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the ETHU in the specified Scheduler
 *
 * see .h file
 */
int
imb_ethu_rmc_low_prio_scheduler_config_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int rmc_id,
    int sch_weight)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_RX_RMC_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOW_PRIO_WEIGHT, INST_SINGLE, sch_weight);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the logical FIFO threshold after overflow.
 * after the fifo reaches overflow, it will not resume writing
 * until fifo level will get below this value.
 *
 * see .h file
 */
int
imb_ethu_port_rmc_thr_after_ovf_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int rmc_id,
    int thr_after_ovf)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THR_AFTER_OVF, INST_SINGLE, thr_after_ovf);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
int
imb_ethu_prd_bypass_hw_set(
    int unit,
    int core,
    int ethu_id_in_core,
    uint32 is_bypass)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * Set Core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * Set ETHU id
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id_in_core);
    /*
     *  Set Bypass mode
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_BYPASS, INST_SINGLE, is_bypass);
    /*
     * Commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - Get ETHU configuration in specified scheduler
 *
 * see .h file
 */
int
imb_ethu_scheduler_config_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    uint32 sch_prio,
    uint32 *weight)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_SCHEDULER_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_PRIORITY, sch_prio);

    /*
     * request value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SCH_WEIGHT, INST_SINGLE, weight);

    /*
     * commit the request
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the port in the NMG
 * see .h file
 */
int
imb_ethu_sch_cnt_dec_thres_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int threshold)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_SCH_CNT_DEC_THRESHOLD, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);

    /*
     * request value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_CNT_DEC_THRESHOLD, INST_SINGLE, threshold);

    /*
     * Commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the port in the NMG
 * see .h file
 */
int
imb_ethu_port_rx_nmg_reset_hw_set(
    int unit,
    bcm_port_t port,
    int rmc_id,
    int in_reset)
{
    dnx_algo_port_rmc_info_t rmc;
    dnx_algo_port_ethu_access_info_t ethu_info;
    uint32 entry_handle_id;
    int ii, nof_rmcs_to_reset, rmc_id_to_reset, nof_priority_groups;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.eth.priority_groups_nof_get(unit);
    nof_rmcs_to_reset = (rmc_id == IMB_COSQ_ALL_RMCS) ? nof_priority_groups : 1;
    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    /*
     * go over all logical FIFOs of the port
     */
    for (ii = 0; ii < nof_rmcs_to_reset; ++ii)
    {
        /*
         * get logical fifo information
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, ii, &rmc));
        rmc_id_to_reset = (rmc_id == IMB_COSQ_ALL_RMCS) ? rmc.rmc_id : rmc_id;
        if (rmc_id_to_reset == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }

        /*
         * set logical fifo key
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc_id_to_reset);
        /*
         * set reset indication
         */
        dbal_entry_value_field32_set(unit,
                                     entry_handle_id,
                                     DBAL_FIELD_NMG_RESET, INST_SINGLE,
                                     in_reset ?
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

/**
 * \brief - Reset the context counter in the scheduler
 *
 * see .h file
 */
int
imb_ethu_port_rx_nmg_reset_per_sch_prio_hw_set(
    int unit,
    bcm_port_t port,
    uint32 sch_prio,
    int in_reset)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_SCHEDULER_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_PRIORITY, sch_prio);

    /*
     * set reset indication
     */
    dbal_entry_value_field32_set(unit,
                                 entry_handle_id,
                                 DBAL_FIELD_NMG_RESET, INST_SINGLE,
                                 in_reset ?
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

/**
 * \brief - Reset the port tx in the NMG
 * see .h file
 */
int
imb_ethu_port_tx_nmg_reset_hw_set(
    int unit,
    bcm_port_t port,
    int in_reset)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_TX_LANE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    /*
     * set lane we want to reset as key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CORE, ethu_info.lane_in_core);
    /*
     * set reset indication
     */
    dbal_entry_value_field32_set(unit,
                                 entry_handle_id,
                                 DBAL_FIELD_NMG_RESET, INST_SINGLE,
                                 in_reset ?
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

/**
 * \brief - Reset the port in the ETHU.
 *
 * see .h file
 */
int
imb_ethu_port_tx_reset_hw_set(
    int unit,
    bcm_port_t port,
    int in_reset)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * Reset the port in TX
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_TX_LANE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CORE, ethu_info.lane_in_core);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE,
                                 in_reset ?
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

/**
 * \brief - Reset the port in the ETHU.
 *
 * see .h file
 */
int
imb_ethu_port_rx_reset_hw_set(
    int unit,
    bcm_port_t port,
    int rmc_id,
    int in_reset)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    dnx_algo_port_rmc_info_t rmc;
    uint32 entry_handle_id;
    int ii, nof_rmcs_to_reset, rmc_id_to_reset, nof_priority_groups;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.eth.priority_groups_nof_get(unit);
    nof_rmcs_to_reset = (rmc_id == IMB_COSQ_ALL_RMCS) ? nof_priority_groups : 1;
    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * Reset the port in RX - port is represented by logical FIFOs
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);

    for (ii = 0; ii < nof_rmcs_to_reset; ++ii)
    {
        /*
         * get logical fifo info
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, ii, &rmc));
        rmc_id_to_reset = (rmc_id == IMB_COSQ_ALL_RMCS) ? rmc.rmc_id : rmc_id;
        if (rmc_id_to_reset == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC id not valid
             */
            continue;
        }

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc_id_to_reset);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE,
                                     in_reset ?
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

int
imb_ethu_tx_egress_flush_hw_set(
    int unit,
    bcm_core_t core,
    int lane,
    int flush_enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_TX_LANE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CORE, lane);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLUSH, INST_SINGLE, flush_enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Common function to set boolean fields in DBAL_TABLE_NIF_ETHU_RX_RMC_CTRL.
 */
static int
imb_ethu_port_rx_rmc_ctrl_enable_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int rmc_id,
    dbal_fields_e dbal_field,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Enable the field for the port in RX - port is represented by logical FIFOs
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_RX_RMC_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc_id);

    /*
     * Set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field, INST_SINGLE, enable ? TRUE : FALSE);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_port_rx_drop_data_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int rmc_id,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_port_rx_rmc_ctrl_enable_hw_set
                    (unit, core, ethu_id_in_core, rmc_id, DBAL_FIELD_DROP_DATA, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_port_rx_rmc_flush_mode_enable_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int rmc_id,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ethu_port_rx_rmc_ctrl_enable_hw_set
                    (unit, core, ethu_id_in_core, rmc_id, DBAL_FIELD_FLUSH_RMC, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_port_rx_nmg_flush_context_enable_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    uint32 sch_priority,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_SCHEDULER_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_PRIORITY, sch_priority);

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLUSH_CONTEXT, INST_SINGLE, enable ? TRUE : FALSE);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_port_rx_nmg_flush_mode_enable_hw_set(
    int unit,
    bcm_core_t core,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_RX_CORE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLUSH_ENABLE, INST_SINGLE, enable ? TRUE : FALSE);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_port_rx_flush_enable_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_RX_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id_in_core);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLUSH_ENABLE, INST_SINGLE, enable ? TRUE : FALSE);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_port_rx_rmc_counter_get(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int rmc_id,
    uint32 *counter)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_RX_RMC_COUNTER, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id_in_core);

    /*
     * Set The RMC to count
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, INST_SINGLE, rmc_id);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Reset DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_ETHU_RX_RMC_COUNTER, entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id_in_core);

    /*
     * Get field value
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COUNTER, INST_SINGLE, counter);

    /*
     * Get entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_port_rx_rmc_counter_polling(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int rmc_id)
{
    uint32 entry_handle_id;
    sal_usecs_t time_out;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_RX_RMC_COUNTER, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id_in_core);

    /*
     * Set The RMC to count
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, INST_SINGLE, rmc_id);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Reset DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_ETHU_RX_RMC_COUNTER, entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id_in_core);
    /*
     * Wait for RMC packets counter to be empty.
     */
    time_out = 100000;
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, time_out, 1000000, entry_handle_id, DBAL_FIELD_COUNTER, 0));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the RMC priority
 *
 * see .h file
 */
int
imb_ethu_internal_port_rmc_scheduler_config_set(
    int unit,
    bcm_port_t port,
    int rmc_id,
    bcm_port_nif_scheduler_t sch_prio)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    uint32 is_tdm_priority = 0, is_high_priority = 0, is_low_priority = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_tdm_priority = (sch_prio == bcmPortNifSchedulerTDM) ? 1 : 0;
    is_high_priority = (sch_prio == bcmPortNifSchedulerHigh) ? 1 : 0;
    is_low_priority = (sch_prio == bcmPortNifSchedulerLow) ? 1 : 0;
    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_RX_RMC_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc_id);
    /*
     * set RMC priority
     */
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm_priority);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_HIGH, INST_SINGLE, is_high_priority);
    if (dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_is_rmc_low_priority_needs_configuration))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LOW, INST_SINGLE, is_low_priority);
    }
    /*
     * The channel differentiate between high and low priority in the IRE.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, INST_SINGLE, is_high_priority ? 1 : 0);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - map physical lane in the ETHU + PRD priority to a
 *        specific RMC (logical FIFO)
 *
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] ethu_id - ethu index inside the core
 * \param [in] first_lane_in_port - port first lane index in the ETHU
 * \param [in] rmc - logical fifo info
 * \param [in] is_map_valid - map valid indication: TRUE => map,
 *        FALSE=> unmap
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
imb_ethu_lane_to_rmc_map(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int first_lane_in_port,
    dnx_algo_port_rmc_info_t * rmc,
    int is_map_valid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * set this RMC in RMCs bitmap for the lane
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_RX_LANE_TO_RMC_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERNAL_LANE, first_lane_in_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc->rmc_id);

    /*
     * enable the RMC for the lane
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTIVE, INST_SINGLE, is_map_valid);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - map physical lane in the ETHU + PRD priority to a
 *        specific RMC (logical FIFO)
 *
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] ethu_id - ethu index inside the core
 * \param [in] first_lane_in_port - port first lane index in the ETHU
 * \param [in] rmc - logical fifo info
 * \param [in] is_map_valid - map valid indication: TRUE => map,
 *        FALSE=> unmap
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
imb_ethu_internal_lane_rx_priority_to_rmc_map(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int first_lane_in_port,
    dnx_algo_port_rmc_info_t * rmc,
    int is_map_valid)
{
    uint32 entry_handle_id;
    int rmc_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * if map is invalid, map all priorities to RMC 0
     */
    rmc_id = is_map_valid ? rmc->rmc_id : 0;
    /*
     * map RMC to lane + PRD priority
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_RX_PRIORITY_TO_RMC_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERNAL_LANE, first_lane_in_port);

    /*
     * RMC can be mapped to more than one PRD priority of the lane (even to all priorities), so each priority is
     * checked independently
     */
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_0)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, INST_SINGLE, rmc_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_VALID, INST_SINGLE, is_map_valid);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_1)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, INST_SINGLE, rmc_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_VALID, INST_SINGLE, is_map_valid);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_2)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, INST_SINGLE, rmc_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_VALID, INST_SINGLE, is_map_valid);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_3)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, INST_SINGLE, rmc_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_VALID, INST_SINGLE, is_map_valid);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_TDM)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, DBAL_DEFINE_NIF_PRD_PRIORITY_TDM);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, INST_SINGLE, rmc_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_VALID, INST_SINGLE, is_map_valid);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - map physical lane in the ETHU + PRD priority to a
 *        specific RMC (logical FIFO)
 *  see imb_ethu_lane_to_rmc_map
 */
int
imb_ethu_port_lane_to_rmc_map(
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
    SHR_IF_ERR_EXIT(imb_ethu_lane_to_rmc_map
                    (unit, ethu_info.core, ethu_info.ethu_id_in_core, ethu_info.first_lane_in_port, rmc, is_map_valid));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - map PRD priority to a
 *        specific RMC (logical FIFO)
 */
int
imb_ethu_internal_port_rx_priority_to_rmc_map(
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
    SHR_IF_ERR_EXIT(imb_ethu_internal_lane_rx_priority_to_rmc_map
                    (unit, ethu_info.core, ethu_info.ethu_id_in_core, ethu_info.first_lane_in_port, rmc, is_map_valid));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - map RMC (logical FIFO) to a physical lane in the ETHU
 *
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] ethu_id - ethu index inside the core
 * \param [in] first_ethu_lane - port first lane index in the ethu
 * \param [in] rmc - logical fifo info.
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
imb_ethu_rmc_to_lane_map_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int first_ethu_lane,
    dnx_algo_port_rmc_info_t * rmc)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc->rmc_id);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERNAL_LANE, INST_SINGLE, first_ethu_lane);

    /*
     * set memory boundaries
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MLF_FIRST_ENTRY, INST_SINGLE, rmc->first_entry);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MLF_LAST_ENTRY, INST_SINGLE, rmc->last_entry);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_internal_port_gen_pfc_from_rmc_enable_set(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int rmc,
    uint32 priority,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Use LLFC threshold to produce PFC  */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_GEN_PFC_FROM_LLFC_BITMAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID, ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, priority);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PFC_FROM_LLFC_BITMAP_TYPE_A_ENABLE, INST_SINGLE,
                                 enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PFC_FROM_LLFC_BITMAP_TYPE_B_ENABLE, INST_SINGLE,
                                 enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PFC_FROM_LLFC_BITMAP_TYPE_C_ENABLE, INST_SINGLE,
                                 enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PFC_FROM_LLFC_BITMAP_TYPE_D_ENABLE, INST_SINGLE,
                                 enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Use PFC threshold to produce PFC (same configuration as LLFC) */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_FC_ETHU_GEN_PFC_FROM_RX_FIFOS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID, ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_internal_port_gen_pfc_from_rmc_enable_get(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int rmc,
    uint32 priority,
    uint32 *enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");

    /** Check the table used to enable PFC threshold to produce PFC (same configuration as LLFC) */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_GEN_PFC_FROM_RX_FIFOS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID, ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_internal_port_gen_llfc_from_rmc_enable_set(
    int unit,
    bcm_core_t core,
    uint32 lane_in_core,
    int rmc,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set HW to enable the RMC of the relevant lane to generate LLFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_GEN_LLFC_FROM_RX_FIFOS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CORE, lane_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_internal_port_stop_pm_from_cfc_llfc_enable_set(
    int unit,
    bcm_core_t core,
    uint32 lane_in_core,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set HW to stop transmission according LLFC signal received from CFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_REC_LLFC_STOP_PM_FROM_CFC, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CORE, lane_in_core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_internal_port_stop_pm_from_pfc_enable_set(
    int unit,
    bcm_core_t core,
    uint32 ethu_id_in_core,
    int pfc,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set HW to stop transmission according received PFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_REC_PFC_TO_STOP_PM_BITMAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID, ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, pfc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BMP_A_PFC_TO_STOP_NIF_EN, INST_SINGLE, enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BMP_B_PFC_TO_STOP_NIF_EN, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_internal_port_stop_pm_from_pfc_enable_get(
    int unit,
    bcm_core_t core,
    uint32 ethu_id_in_core,
    int pfc,
    uint32 *enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get from HW enable indication for received PFC to stop transmission on a port */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_REC_PFC_TO_STOP_PM_BITMAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID, ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, pfc);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BMP_A_PFC_TO_STOP_NIF_EN, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_internal_port_gen_pfc_from_llfc_thresh_enable_set(
    int unit,
    bcm_core_t core,
    uint32 ethu_id_in_core,
    uint32 rmc,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set to HW enable indication for generation of PFC from LLFC RMC threshold
     *  All priorities are set the same.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_GEN_PFC_FROM_LLFC_THRESH, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID, ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_internal_port_gen_pfc_from_llfc_thresh_enable_get(
    int unit,
    bcm_core_t core,
    uint32 ethu_id_in_core,
    uint32 rmc,
    uint32 *enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get from HW enable indication for generation of PFC from LLFC RMC threshold
     *  All RMCs of the port and all priorities are set the same, so it is enough to
     *  get the indication for the first priority.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_GEN_PFC_FROM_LLFC_THRESH, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID, ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_internal_port_fc_rx_qmlf_threshold_set(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int rmc,
    uint32 flags,
    uint32 threshold)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the handle to the relevant table and set the key fields */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_RX_QMLF_THRESHOLD, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID, ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);

    if (flags & BCM_COSQ_THRESHOLD_ETH_PORT_LLFC)
    {
        /*
         * Set RX PORT FIFO Link Level Flow Control generation thresholds
         */
        if (flags & BCM_COSQ_THRESHOLD_SET)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLFC_THRESHOLD_SET, INST_SINGLE, threshold);
        }

        if (flags & BCM_COSQ_THRESHOLD_CLEAR)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLFC_THRESHOLD_CLR, INST_SINGLE, threshold);
        }
    }

    if (flags & BCM_COSQ_THRESHOLD_ETH_PORT_PFC)
    {
        /*
         * Set RX PORT FIFO Priority Flow Control generation thresholds
         */
        if (flags & BCM_COSQ_THRESHOLD_SET)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PFC_THRESHOLD_SET, INST_SINGLE, threshold);
        }
        if (flags & BCM_COSQ_THRESHOLD_CLEAR)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PFC_THRESHOLD_CLR, INST_SINGLE, threshold);
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_internal_port_fc_rx_qmlf_threshold_get(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int rmc,
    uint32 flags,
    uint32 *threshold)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the handle to the relevant table and set the key fields */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_RX_QMLF_THRESHOLD, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID, ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);

    if (flags & BCM_COSQ_THRESHOLD_ETH_PORT_LLFC)
    {
        /*
         * Set RX PORT FIFO Link Level Flow Control generation thresholds
         */
        if (flags & BCM_COSQ_THRESHOLD_SET)
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LLFC_THRESHOLD_SET, INST_SINGLE, threshold);
        }

        if (flags & BCM_COSQ_THRESHOLD_CLEAR)
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LLFC_THRESHOLD_CLR, INST_SINGLE, threshold);
        }
    }

    if (flags & BCM_COSQ_THRESHOLD_ETH_PORT_PFC)
    {
        /*
         * Set RX PORT FIFO Priority Flow Control generation thresholds
         */
        if (flags & BCM_COSQ_THRESHOLD_SET)
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PFC_THRESHOLD_SET, INST_SINGLE, threshold);
        }
        if (flags & BCM_COSQ_THRESHOLD_CLEAR)
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PFC_THRESHOLD_CLR, INST_SINGLE, threshold);
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_internal_fc_reset_set(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    uint32 in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set HW to put in/out of reset all ETHUs on both cores */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_RESET, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID, ethu_id_in_core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_RESET, INST_SINGLE, in_reset);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_internal_llfc_from_glb_rsc_enable_set(
    int unit,
    bcm_core_t core,
    int lane_in_core,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set HW to enable/disable the generation of LLFC based on a signal for Global resources for all ETHUs */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_GEN_LLFC_FROM_GLB_RCS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CORE, lane_in_core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_internal_pfc_rec_priority_map(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    uint32 nif_priority,
    uint32 egq_priority)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set HW to map the NIF priority to the EGQ priority for the specified ETHU */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_REC_PFC_BITMAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID, ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CFC_PRIO, egq_priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIO, nif_priority);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BITMAP_TYPE_A_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BITMAP_TYPE_B_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ethu_internal_pfc_rec_priority_unmap(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set HW to unmap all priorities for the specified ETHU */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_REC_PFC_BITMAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID, ethu_id_in_core);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CFC_PRIO, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIO, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BITMAP_TYPE_A_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BITMAP_TYPE_B_ENABLE, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - map STIF instance to NIF port
 *
 * see .h file
 */
int
imb_ethu_internal_port_to_stif_instance_map(
    int unit,
    int core,
    int sif_instance_id,
    int ethu_id_in_core,
    int first_lane_in_port,
    int ethu_select)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_STIF_INSTANCE_MAP, &entry_handle_id));
    /** set key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STIF_INSTANCE, sif_instance_id);
    /** ETHU_SELECT selects CLU/ETHU */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_ETHU_SELECT, INST_SINGLE, ethu_select);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, INST_SINGLE, first_lane_in_port);
    /** commit value */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See imb_ethu_internal.h
 */
int
imb_ethu_internal_rmc_level_get(
    int unit,
    bcm_port_t port,
    uint32 rmc_id,
    uint32 *fifo_level)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc_id);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIFO_LEVEL, INST_SINGLE, fifo_level);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get PRD drop counter value, per RMC
 *
 * see .h file
 */
int
imb_ethu_prd_drop_count_hw_get(
    int unit,
    bcm_port_t port,
    uint32 rmc,
    uint64 *count)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_COUNTERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    /*
     * request for value field
     */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_DROP_COUNTER, INST_SINGLE, count);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - internal use only, Reset the port credit tx lane ctrl.
 */
static int
imb_ethu_port_tx_lane_ctrl_credit_reset(
    int unit,
    dnx_algo_port_ethu_access_info_t ethu_info,
    bcm_pbmp_t phys,
    int value)
{
    int phy, lane;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_TX_LANE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);

    BCM_PBMP_ITER(phys, phy)
    {
        lane = phy % dnx_data_nif.phys.nof_phys_per_core_get(unit);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CORE, lane);
        /*
         * set value field 0
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_RESET, INST_SINGLE, value);
        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the port credit in the ETHU.
 *
 * see .h file
 */
int
imb_ethu_port_credit_tx_reset(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    bcm_pbmp_t phys;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imbm.credit_tx_reset_mutex.take(unit, sal_mutex_FOREVER));

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
    /*
     * Reset the port credit value in TX
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_TX_CREDIT_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_VALUE, INST_SINGLE, 0);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Reset the port credit in TX - sequence 0 1 0
     */
    SHR_IF_ERR_EXIT(imb_ethu_port_tx_lane_ctrl_credit_reset(unit, ethu_info, phys, 0));
    SHR_IF_ERR_EXIT(imb_ethu_port_tx_lane_ctrl_credit_reset(unit, ethu_info, phys, 1));
    SHR_IF_ERR_EXIT(imb_ethu_port_tx_lane_ctrl_credit_reset(unit, ethu_info, phys, 0));

exit:
    SHR_IF_ERR_CONT(imbm.credit_tx_reset_mutex.give(unit));
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get NIF RX fifo status
 *
 * see .h file
 */
int
imb_ethu_internal_port_rx_fifo_status_get(
    int unit,
    bcm_port_t port,
    int rmc_id,
    uint32 *max_occupancy,
    uint32 *fifo_level)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_RX_FIFO_STATUS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc_id);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_OCCUPANCY, INST_SINGLE, max_occupancy);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIFO_LEVEL, INST_SINGLE, fifo_level);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get NIF_TX fifo status
 *
 * see .h file
 */

int
imb_ethu_internal_port_tx_fifo_status_get(
    int unit,
    bcm_port_t port,
    uint32 *max_occupancy,
    uint32 *fifo_level,
    uint32 *pm_credits)
{
    uint32 entry_handle_id;
    int dbal_ethu_key_type;
    int ethu_id = 0;
    dnx_algo_port_ethu_access_info_t ethu_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle and parse additional table keys
     */
    if (ethu_info.imb_type == imbDispatchTypeImb_cdu)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_TX_FIFO_STATUS, &entry_handle_id));
        dbal_ethu_key_type = DBAL_FIELD_CDU;
        ethu_id = ethu_info.ethu_id_in_core;
    }
    else if (ethu_info.imb_type == imbDispatchTypeImb_clu)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CLU_TX_FIFO_STATUS, &entry_handle_id));
        dbal_ethu_key_type = DBAL_FIELD_CLU;
        ethu_id = ethu_info.ethu_id_in_core - dnx_data_nif.eth.cdu_nof_get(unit);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported pm_type=%d for port=%d", ethu_info.imb_type, port);
    }

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, dbal_ethu_key_type, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERNAL_LANE, ethu_info.first_lane_in_port);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_OCCUPANCY, INST_SINGLE, max_occupancy);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIFO_LEVEL, INST_SINGLE, fifo_level);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PM_CREDITS, INST_SINGLE, pm_credits);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set EGQ credit mask for ETHU/CLU ports
 *
 * see .h file
 */
int
imb_ethu_port_credit_mask_set(
    int unit,
    bcm_port_t port,
    int value)
{
    int first_phy;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU/CLU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));
    /*
     * Configure the port credit mask
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_EGQ_CREDIT_MASK, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT_ID, first_phy);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_CREDIT_MASK, INST_SINGLE, value);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable PRD hard stage per RMC. this effectively
 *        enable the PRD feature.
 *
 * see .h file
 */
int
imb_ethu_prd_hard_stage_enable_hw_set(
    int unit,
    bcm_port_t port,
    uint32 rmc,
    uint32 enable)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_HARD_STAGE, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get enable indication for the PRD hard stage per RMC
 *
 * see .h file
 */
int
imb_ethu_prd_hard_stage_enable_hw_get(
    int unit,
    bcm_port_t port,
    uint32 rmc,
    uint32 *enable)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE_HARD_STAGE, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the TM priority map (PRD hard stage). add the
 *        the map priority value to be returned per TC + DP
 *        values
 *
 * see .h file
 */
int
imb_ethu_prd_map_tm_tc_dp_hw_set(
    int unit,
    bcm_port_t port,
    uint32 tc,
    uint32 dp,
    uint32 priority,
    uint32 is_tdm)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_TM_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information from TM priority map (PRD hard
 *        stage). get the priority given for a combination of
 *        TC+DP.
 *
 * see .h file
 */
int
imb_ethu_prd_map_tm_tc_dp_hw_get(
    int unit,
    bcm_port_t port,
    uint32 tc,
    uint32 dp,
    uint32 *priority,
    uint32 *is_tdm)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_TM_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    else
    {
        *is_tdm = 0;
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the IP priority table (PRD hard stage). set
 *        priority value for a specific DSCP.
 *
 * see .h file
 */
int
imb_ethu_prd_map_ip_dscp_hw_set(
    int unit,
    bcm_port_t port,
    uint32 dscp,
    uint32 priority,
    uint32 is_tdm)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_IP_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DSCP, dscp);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information from IP priority table (PRD hard
 *        stage). get the priority returned for a specific DSCP
 *
 * See .h file
 */
int
imb_ethu_prd_map_ip_dscp_hw_get(
    int unit,
    bcm_port_t port,
    uint32 dscp,
    uint32 *priority,
    uint32 *is_tdm)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_IP_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DSCP, dscp);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    else
    {
        *is_tdm = 0;
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the MPLS priority table (PRD hard stage). set
 *        priority value for a specific EXP value.
 *
 * see .h file
 */
int
imb_ethu_prd_map_mpls_exp_hw_set(
    int unit,
    bcm_port_t port,
    uint32 exp,
    uint32 priority,
    uint32 is_tdm)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_MPLS_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EXP, exp);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information from the MPLS priority table. get
 *        the priority returned for a specific EXP value
 *
 * see .h file
 */
int
imb_ethu_prd_map_mpls_exp_hw_get(
    int unit,
    bcm_port_t port,
    uint32 exp,
    uint32 *priority,
    uint32 *is_tdm)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_MPLS_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EXP, exp);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    else
    {
        *is_tdm = 0;
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the ETH (VLAN) Priority table (PRD hard
 *        stage). set a priority value to match a specific
 *        PCP+DEI combination
 *
 * see .h file
 */
int
imb_ethu_prd_map_eth_pcp_dei_hw_set(
    int unit,
    bcm_port_t port,
    uint32 pcp,
    uint32 dei,
    uint32 priority,
    uint32 is_tdm)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_VLAN_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP, pcp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DEI, dei);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - get information from the ETH (VLAN) priority table
 *        (PRD hard stage). get the priority returned for a
 *        specific combination of PCP+DEI
 *
 * see .h file
 */
int
imb_ethu_prd_map_eth_pcp_dei_hw_get(
    int unit,
    bcm_port_t port,
    uint32 pcp,
    uint32 dei,
    uint32 *priority,
    uint32 *is_tdm)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_VLAN_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP, pcp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DEI, dei);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    else
    {
        *is_tdm = 0;
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set configurable ether type to a ether type code.
 *        the ether type codes are meaningful in the PRD soft
 *        stage (TCAM).
 *
 * See .h file
 */
int
imb_ethu_prd_custom_ether_type_hw_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_val)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ethu access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_CONFIGURABLE_ETHER_TYPE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_VAL, INST_SINGLE, ether_type_val);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the ether type value mapped to a specific ether
 *        type code, out of the configurable ether types (codes
 *        1-6).
 *
 * see .h file
 */
int
imb_ethu_prd_custom_ether_type_hw_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_val)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ethu access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_CONFIGURABLE_ETHER_TYPE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_VAL, INST_SINGLE, ether_type_val);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the offset base for the TCAM key. it means the
 *        offsets that compose the key will start from this
 *        offset base. the offset base have 3 options:
 * 0=>start of packet
 * 1=>end of eth header
 * 2=>end of header after eth header.
 *
 * see .h file
 */
int
imb_ethu_prd_tcam_entry_key_offset_base_hw_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_base)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ethu access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_ETHER_TYPE_CODE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET_BASE, INST_SINGLE, offset_base);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the current offset base for the given ether
 *        code.
 *
 * see .h file
 */
int
imb_ethu_prd_tcam_entry_key_offset_base_hw_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *offset_base)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ethu access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_ETHER_TYPE_CODE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OFFSET_BASE, INST_SINGLE, offset_base);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set ether type size (in bytes) for the given ether
 *        type code. the ether type size is only used if the
 *        offset base for the key is "end of header after eth
 *        header"
 *
 * see .h file
 */
int
imb_ethu_prd_ether_type_size_hw_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_size)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ethu access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_ETHER_TYPE_CODE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_SIZE, INST_SINGLE, ether_type_size);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the ether type currently set for a specific
 *        ether type code
 *
 * see .h file
 */
int
imb_ethu_prd_ether_type_size_hw_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_size)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ethu access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_ETHER_TYPE_CODE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_SIZE, INST_SINGLE, ether_type_size);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the PRD soft stage (TCAM) key. there is a key
 *        per ether type (total of 16 keys). the key is build of
 *        4 offsets given in the packet header. from each
 *        offset, 8 bits are taken to create a total of 32 bit.
 *        when comparing to the TCAM entries, the ether type
 *        code joins the key to create 36bit key:
 *        |ether type code|offset 4|offset 3|offset 2|offset 1|
 *        -----------------------------------------------------
 *        35              31       23       15       7        0
 *
 * see .h file
 */
int
imb_ethu_prd_tcam_entry_key_offset_hw_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 offset_value)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ethu access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_TCAM_KEY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET_INDEX, offset_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET, INST_SINGLE, offset_value);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information about the TCAM key for a specific
 *        ether type.
 *
 * see .h file
 */
int
imb_ethu_prd_tcam_entry_key_offset_hw_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 *offset_value)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ethu access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_TCAM_KEY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET_INDEX, offset_index);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OFFSET, INST_SINGLE, offset_value);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the PRD soft stage TCAM table. there are 32
 *        entries in the table. if there is a hit, the priority
 *        for the packet will be taken from the TCAM entry
 *        information
 *
 * see .h file
 */

int
imb_ethu_prd_tcam_entry_hw_set(
    int unit,
    bcm_port_t port,
    uint32 entry_index,
    const dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ethu access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_TCAM, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_ID, entry_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, INST_SINGLE,
                                 entry_info->ether_code_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE_MASK, INST_SINGLE,
                                 entry_info->ether_code_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_OFFSETS_VALUE, INST_SINGLE,
                                 entry_info->offset_array_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_OFFSETS_MASK, INST_SINGLE,
                                 entry_info->offset_array_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, entry_info->priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM, INST_SINGLE, entry_info->tdm);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, entry_info->is_entry_valid);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information from the PRD soft stage (TCAM)
 *        table. get specific entry information
 *
 * see .h file
 */
int
imb_ethu_prd_tcam_entry_hw_get(
    int unit,
    bcm_port_t port,
    uint32 entry_index,
    dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ethu access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_TCAM, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_ID, entry_index);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, INST_SINGLE,
                               &entry_info->ether_code_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE_MASK, INST_SINGLE,
                               &entry_info->ether_code_mask);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENTRY_OFFSETS_VALUE, INST_SINGLE,
                               &entry_info->offset_array_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENTRY_OFFSETS_MASK, INST_SINGLE,
                               &entry_info->offset_array_mask);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, &entry_info->priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TDM, INST_SINGLE, &entry_info->tdm);
    }
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &entry_info->is_entry_valid);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set Control Frame properties to be recognized by the
 *        PRD parser. if a control frame is identified, it
 *        automatically gets the highest priority (3). each
 *        packet is compared against all control frame
 *        properties of the ETHU
 *
 * see .h file
 */
int
imb_ethu_prd_control_frame_hw_set(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    const bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_CONTROL_FRAME, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, control_frame_index);
    /*
     * set value field
     */
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_MAC_DA_VALUE, INST_SINGLE,
                                 control_frame_config->mac_da_val);
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_MAC_DA_MASK, INST_SINGLE,
                                 control_frame_config->mac_da_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, INST_SINGLE,
                                 control_frame_config->ether_type_code);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE_MASK, INST_SINGLE,
                                 control_frame_config->ether_type_code_mask);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get Control Frame properties recognized by the
 *        parser.
 *
 * see .h file
 */
int
imb_ethu_prd_control_frame_hw_get(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_CONTROL_FRAME, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, control_frame_index);
    /*
     * request for value field
     */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_MAC_DA_VALUE, INST_SINGLE,
                               &control_frame_config->mac_da_val);
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_MAC_DA_MASK, INST_SINGLE,
                               &control_frame_config->mac_da_mask);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, INST_SINGLE,
                               &control_frame_config->ether_type_code);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE_MASK, INST_SINGLE,
                               &control_frame_config->ether_type_code_mask);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set MPLS special label properties. if one of the
 *        MPLS labels is identified as special label and its
 *        value match one of the special label values, priority
 *        for the packet is taken from the special label
 *        properties.
 *
 * see .h file
 */
int
imb_ethu_prd_mpls_special_label_hw_set(
    int unit,
    bcm_port_t port,
    uint32 special_label_index,
    const bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_MPLS_SPECIAL_LABEL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, special_label_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LABEL_VALUE, INST_SINGLE, label_config->label_value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, label_config->priority);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, label_config->is_tdm);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get current configuration of MPLS special label
 *
 * see .h file
 */
int
imb_ethu_prd_mpls_special_label_hw_get(
    int unit,
    bcm_port_t port,
    uint32 special_label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_MPLS_SPECIAL_LABEL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, special_label_index);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LABEL_VALUE, INST_SINGLE, &label_config->label_value);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, &label_config->priority);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, &label_config->is_tdm);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
int
imb_ethu_prd_bypass_get(
    int unit,
    int core,
    int ethu_id_in_core,
    uint32 *is_bypass)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * Set Core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * Set ETHU id
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id_in_core);
    /*
     *  Get Bypass mode
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRD_BYPASS, INST_SINGLE, is_bypass);
    /*
     * Commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - set one of the TPID values per port profile to be recognized
 *        by the PRD parser. if a packet TPID is not recognized,
 *        the packet will get the default priority. each port
 *        can have four TPID values.
 *
 * see .h file
 */
int
imb_ethu_prd_tpid_hw_set(
    int unit,
    bcm_core_t core,
    uint32 ethu_id,
    uint32 prd_profile,
    uint32 tpid_index,
    uint32 tpid_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_TPID, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, tpid_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID, INST_SINGLE, tpid_value);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the TPID value per port profile to be recognized
 *        by the PRD parser. if a packet TPID is not recognized,
 *        the packet will get the default priority. each port
 *        can have four TPID values.
 *
 * see .h file
 */
int
imb_ethu_prd_tpid_hw_get(
    int unit,
    bcm_core_t core,
    uint32 ethu_id,
    uint32 prd_profile,
    uint32 tpid_index,
    uint32 *tpid_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_TPID, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, tpid_index);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TPID, INST_SINGLE, tpid_value);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD Parser whether to ignore indication of IP
 *        dscp even if packet is identified as IP.
 *
 * see .h file
 */
int
imb_ethu_prd_ignore_ip_dscp_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 ignore_ip_dscp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_IP_DSCP, INST_SINGLE, ignore_ip_dscp);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get from PRD parser indication whether it ignores IP
 *        DSCP
 *
 * see .h file
 */
int
imb_ethu_prd_ignore_ip_dscp_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 *ignore_ip_dscp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_IP_DSCP, INST_SINGLE, ignore_ip_dscp);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD parser to ignore MPLS EXP even if packet is
 *        identified as MPLS
 *
 * see .h file
 */
int
imb_ethu_prd_ignore_mpls_exp_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 ignore_mpls_exp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_MPLS_EXP, INST_SINGLE, ignore_mpls_exp);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get from PRD parser indication whether it ignores
 *        MPLS EXP
 *
 * see .h file
 */
int
imb_ethu_prd_ignore_mpls_exp_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 *ignore_mpls_exp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_MPLS_EXP, INST_SINGLE, ignore_mpls_exp);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD Parser whether to ignore inner tag PCP DEI
 *        indication even if packet is identified as double
 *        tagged
 *
 * see .h file
 */
int
imb_ethu_prd_ignore_inner_tag_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 ignore_inner_tag)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_INNER_ETH_TAG, INST_SINGLE, ignore_inner_tag);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get from PRD parser indication whether it ignores
 *        inner VLAN tag
 *
 * see .h file
 */
int
imb_ethu_prd_ignore_inner_tag_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 *ignore_inner_tag)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_INNER_ETH_TAG, INST_SINGLE, ignore_inner_tag);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD Parser whether to ignore outer tag PCP DEI
 *        indication even if packet is identified as VLAN tagged
 *
 * see .h file
 */
int
imb_ethu_prd_ignore_outer_tag_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 ignore_outer_tag)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_OUTER_ETH_TAG, INST_SINGLE, ignore_outer_tag);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get from PRD parser indication whether it ignores
 *        outer VLAN tag
 *
 * see .h file
 */
int
imb_ethu_prd_ignore_outer_tag_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 *ignore_outer_tag)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_OUTER_ETH_TAG, INST_SINGLE, ignore_outer_tag);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default priority for the PRD hard stage parser.
 *        if the packet is not identified as IP/MPLS/double
 *        tagged/single tagged or if the relevant indications
 *        are set to ignore, the default priority will be given
 *        to the packet. (the priority from the hard stage can
 *        be later overridden by the priority from the soft
 *        stage.
 *
 * see .h file
 */
int
imb_ethu_prd_default_priority_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 default_priority)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_PRIORITY, INST_SINGLE, default_priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get default priority given in the PRD hard stage
 *        parser.
 *
 * see .h file
 */
int
imb_ethu_prd_default_priority_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 *default_priority)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DEFAULT_PRIORITY, INST_SINGLE, default_priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable PRD soft stage per port. PRD soft stage is a
 *        TCAM for which the user configures both the table and
 *        the key. if there is a hit in the TCAM, the priority
 *        from the TCAM entry will override the priority from
 *        the hard stage
 *
 * see .h file
 */
int
imb_ethu_prd_soft_stage_enable_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 enable_eth,
    uint32 enable_tm)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOFT_STAGE_ENABLE_ETH, INST_SINGLE, enable_eth);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOFT_STAGE_ENABLE_TM, INST_SINGLE, enable_tm);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get enable indication for PRD soft stage
 *
 * see .h file
 */
int
imb_ethu_prd_soft_stage_enable_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 *enable_eth,
    uint32 *enable_tm)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SOFT_STAGE_ENABLE_ETH, INST_SINGLE, enable_eth);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SOFT_STAGE_ENABLE_TM, INST_SINGLE, enable_tm);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set outer tag size for the port. if port is port
 *        Extender, the outer tag size should be set to 8B,
 *        otherwise 4B
 *
 * see .h file
 */
int
imb_ethu_prd_outer_tag_size_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 outer_tag_size)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TAG_SIZE, INST_SINGLE, outer_tag_size);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get current outer tag size for the port
 *
 * see .h file
 */
int
imb_ethu_prd_outer_tag_size_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 *outer_tag_size)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUTER_TAG_SIZE, INST_SINGLE, outer_tag_size);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD port type, according to the header type of
 *        the port
 *
 * see .h file
 */
int
imb_ethu_prd_port_type_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 prd_port_type)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_TYPE, INST_SINGLE, prd_port_type);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get PRD port type for the port
 *
 * see .h file
 */
int
imb_ethu_prd_port_info_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 *prd_port_info)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PORT_TYPE, INST_SINGLE, prd_port_info);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set ITMH TC + DP offsets for the PRD.
 *
 * see .h file
 */
int
imb_ethu_prd_itmh_offsets_hw_set(
    int unit,
    bcm_port_t port,
    uint32 tc_offset,
    uint32 dp_offset)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_OFFSET, INST_SINGLE, tc_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_DP_OFFSET, INST_SINGLE, dp_offset);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get ITMH TC and DP offsets in PRD
 *
 * see .h file
 */
int
imb_ethu_prd_itmh_offsets_hw_get(
    int unit,
    bcm_port_t port,
    uint32 *tc_offset,
    uint32 *dp_offset)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_OFFSET, INST_SINGLE, tc_offset);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ITMH_DP_OFFSET, INST_SINGLE, dp_offset);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set FTMH  TC + DP offsets for the PRD
 *
 * see .h file
 */
int
imb_ethu_prd_ftmh_offsets_hw_set(
    int unit,
    bcm_port_t port,
    uint32 tc_offset,
    uint32 dp_offset)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_TC_OFFSET, INST_SINGLE, tc_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_DP_OFFSET, INST_SINGLE, dp_offset);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get FTMH TC + DP offsets in PRD
 *
 * see .h file
 */
int
imb_ethu_prd_ftmh_offsets_hw_get(
    int unit,
    bcm_port_t port,
    uint32 *tc_offset,
    uint32 *dp_offset)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_TC_OFFSET, INST_SINGLE, tc_offset);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_DP_OFFSET, INST_SINGLE, dp_offset);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable checking for MPLS special label, if packet is
 *        identified as MPLS
 *
 * see .h file
 */
int
imb_ethu_prd_mpls_special_label_enable_hw_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_SPECIAL_LABEL_ENABLE, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get enable indication for MPLS special label.
 *
 * see .h file
 */
int
imb_ethu_prd_mpls_special_label_enable_hw_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ETHU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MPLS_SPECIAL_LABEL_ENABLE, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief - Reset per port instrumentation counters for PFC deadlock detection mechanism
*
* see .h file
*/
int
imb_ethu_internal_pfc_deadlock_counters_reset(
    int unit,
    bcm_core_t core,
    uint32 lane_in_core)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set HW to reset the instrumentation counters for PFC deadlock detection mechanism */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_INSTRU_PFC_CNTS_RESET, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CORE, lane_in_core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET_CNTS, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /** Take out of reset */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET_CNTS, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief - Reset per ETHU instrumentation counters
*
* see .h file
*/
int
imb_ethu_internal_instru_counters_reset(
    int unit,
    bcm_core_t core,
    int ethu_in_core)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set HW to reset the NIF instrumentation counters per ETHU */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_INSTRU_CNTS_RESET, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID, ethu_in_core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INSTRU_RESET, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /** Take out of reset */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INSTRU_RESET, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief - Get from HW the debug info for PFC deadlock detection mechanism
*
* see .h file
*/
int
imb_ethu_internal_pfc_deadlock_debug_info_get(
    int unit,
    bcm_core_t core,
    int ethu_in_core,
    int *ethu_port_num,
    int *pfc_num)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get from HW the debug info for PFC deadlock detection mechanism */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_PFC_DEADLOCK_BREAKING_MECH, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID, ethu_in_core);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PORT_NUM, INST_SINGLE, (uint32 *) ethu_port_num);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PFC_NUM, INST_SINGLE, (uint32 *) pfc_num);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief - Get from HW the max PFC duration for PFC deadlock detection mechanism
*
* see .h file
*/
int
imb_ethu_internal_pfc_deadlock_max_duration_get(
    int unit,
    bcm_core_t core,
    int ethu_in_core,
    uint32 *max_duration)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get from HW the max PFC duration for PFC deadlock detection mechanism */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_PFC_DEADLOCK_BREAKING_MECH, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID, ethu_in_core);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PFC_MAX_DURATION, INST_SINGLE, max_duration);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Set to HW the max PFC duration for PFC deadlock detection mechanism
 *
 * see .h file
 */
int
imb_ethu_internal_pfc_deadlock_max_duration_set(
    int unit,
    bcm_core_t core,
    int ethu_in_core,
    uint32 max_duration)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set to HW the max PFC duration for PFC deadlock detection mechanism */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ETHU_PFC_DEADLOCK_BREAKING_MECH, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID, ethu_in_core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PFC_MAX_DURATION, INST_SINGLE, max_duration);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure start TX for ETHU RMC
 *
 * see .h file
 */
int
imb_ethu_internal_port_rx_start_tx_thr_set(
    int unit,
    bcm_port_t port,
    int rmc_id,
    uint32 start_tx,
    int enable)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_TX_EN, INST_SINGLE, enable);
    if (enable)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_TX_THR, INST_SINGLE, start_tx);
    }
    else
    {
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id,
                                                   DBAL_FIELD_START_TX_THR, INST_SINGLE, DBAL_PREDEF_VAL_DEFAULT_VALUE);
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the L1 TX credit counter
 *
 * see .h file
 */
int
imb_ethu_port_l1_tx_credit_counter_reset_hw_set(
    int unit,
    bcm_port_t port,
    int in_reset)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_L1_TX_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TMC, ethu_info.lane_in_core);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_RSTN, INST_SINGLE, in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure Rx credit counter for L1 mismatch rate
 *
 * see .h file
 */
int
imb_ethu_port_rx_credit_counter_hw_set(
    int unit,
    bcm_port_t port,
    int rmc_id,
    int credit_val,
    int enable)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_RX_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MISMATCH_RATE_CREDIT_VAL, INST_SINGLE, credit_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_PENDING_CNT_INC_EN, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_ETHU_RX_RMC_CTRL, entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ethu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU, ethu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MISMATCH_RATE_CREDIT_EN, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MISMATCH_RATE_CREDIT_RESET, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure power down of ETHU
 */
int
imb_ethu_power_down_set(
    int unit,
    bcm_core_t core_id,
    int ethu_id,
    int power_down)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETHU_POWER_DOWN, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHU_ID, ethu_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POWER_DOWN, INST_SINGLE, power_down);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
