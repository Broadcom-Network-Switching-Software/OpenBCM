/** \file imb_clu_internal.c
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
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <soc/portmod/portmod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_debug.h>

#include "imb_ethu_internal.h"
#include "imb_clu_internal.h"

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

int
imb_clu_rx_pm_enable_hw_set(
    int unit,
    int clu_id,
    int pm_id,
    uint32 value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CLU_PM_CTRL, &entry_handle_id));
    /*
     * Set CLU id
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU, clu_id);
    /*
     * Set PM id in CLU
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_IN_CLU, pm_id);
    /*
     *  Set value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_ENABLE, INST_SINGLE, value);
    /*
     * Commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

int
imb_clu_rx_pm_port_mode_hw_set(
    int unit,
    int clu_id,
    int pm_id,
    uint32 value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CLU_PM_CTRL, &entry_handle_id));
    /*
     * Set CLU id
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU, clu_id);
    /*
     * Set PM id in CLU
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_IN_CLU, pm_id);
    /*
     *  Set value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_PORTS_CONFIG, INST_SINGLE, value);
    /*
     * Commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
imb_clu_rx_pm_aligner_reset_hw_set(
    int unit,
    int clu_id,
    int pm_id,
    uint32 reset)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_nif_reset_reverse_logic_e dbal_reset;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CLU_PM_CTRL, &entry_handle_id));
    /*
     * Set CLU id
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU, clu_id);
    /*
     * Set PM id in CLU
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_IN_CLU, pm_id);
    /*
     *  Set value
     */
    dbal_reset =
        reset ? DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET : DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALIGNER_SHIFT_UNIT_RESET, INST_SINGLE, dbal_reset);
    /*
     * Commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
imb_clu_rx_pm_reset_hw_set(
    int unit,
    int clu_id,
    int pm_id,
    uint32 in_reset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CLU_PM_CTRL, &entry_handle_id));
    /*
     * Set CLU id
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU, clu_id);
    /*
     * Set PM id in CLU
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_IN_CLU, pm_id);

    /*
     * set value fields
     * Note: taking all lanes out-of-reset even if there is only one active lane on the CLU,
     * since the per lane pwrdown is not taking lane mpa into account.
     * e.g. if lane 0 on the chip is connected to rx of lane 1 and tx of lane 2, then when power down lane 0,
     * it can affect two different ports.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU_PM_LANES_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_CLU_PM_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_CLU_PM_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    /*
     * Commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set TX start threshold. this threshold represent the
 *        number of 64B words to be accumulated in the MLF
 *        before transmitting towards the PM. This is to prevent
 *        TX MAC starvation and is important for systems with
 *        oversubscription.
 *
 *  see .h file
 */
int
imb_clu_tx_start_thr_hw_set(
    int unit,
    uint32 clu_id,
    int first_lane_in_port,
    uint32 start_thr)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CLU_TX_LANE_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU, clu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CLU, first_lane_in_port);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_THR, INST_SINGLE, start_thr);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset GearShift
 *
 * \param [in] unit - chip unit id
 * \param [in] clu_id - clu index
 * \param [in] first_lane_in_port - first_lane_in_port (which extracted from the logical port)
 * \param [in] reset - '1': reset, '0': out of reset
 *
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
imb_clu_rx_gear_shift_reset_hw_set(
    int unit,
    uint32 clu_id,
    int first_lane_in_port,
    int reset)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_nif_reset_reverse_logic_e dbal_reset;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CLU_GEAR_SHIFT_RESET, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU, clu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CLU, first_lane_in_port);

    /*
     * set value field
     */
    dbal_reset =
        reset ? DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET : DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GS_RESET, INST_SINGLE, dbal_reset);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
imb_clu_rx_prd_port_profile_map_hw_set(
    int unit,
    uint32 clu_id,
    int first_lane_in_port,
    int prd_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CLU_PORT_PROFILE_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU, clu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CLU, first_lane_in_port);

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, INST_SINGLE, prd_profile);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD thresholds per priority. based on the
 *        priority given from the parser, the packet is sent to
 *        the correct RMC. in each RMC there is a thresholds per
 *        priority which is mapped to it.
 *
 * see .h file
 */
int
imb_clu_prd_threshold_hw_set(
    int unit,
    int clu_id,
    uint32 rmc,
    uint32 priority,
    uint32 threshold)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CLU_THRESHOLDS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU_ID, clu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE, threshold);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the current threshold for a specific priority in
 *        a specific RMC.
 *
 * see .h file
 */
int
imb_clu_prd_threshold_hw_get(
    int unit,
    int clu_id,
    uint32 rmc,
    uint32 priority,
    uint32 *threshold)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CLU_THRESHOLDS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU_ID, clu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, rmc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE, threshold);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable EEE for the CLU 
 * see .h file 
 */
int
imb_clu_eee_enable_set(
    int unit,
    int clu_id,
    int pm_id,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CLU_PM_CTRL, &entry_handle_id));
    /*
     * Set CLU id
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU, clu_id);
    /*
     * Set PM id in CLU
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_IN_CLU, pm_id);

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEE_ENABLE, INST_SINGLE, !!enable);
    /*
     * Commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
