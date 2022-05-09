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
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <soc/portmod/portmod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mib.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_bitstream.h>
#include "imb_ethu_internal.h"
#include "imb_cdu_internal.h"

#include "../../../../../include/bcm_int/dnx/port/imb/imb_ethu.h"
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

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
imb_cdu_tx_start_thr_hw_set(
    int unit,
    bcm_core_t core,
    uint32 cdu_id,
    int first_lane_in_port,
    uint32 start_thr)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_TX_LANE_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CDU, first_lane_in_port);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_THR, INST_SINGLE, start_thr);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the MAC mode on a specific mac_mode_config interface.
 * see .h file
 */
int
imb_cdu_port_mac_mode_config_set(
    int unit,
    bcm_port_t port,
    int mac_mode,
    int enable)
{
    dnx_algo_port_ethu_access_info_t cdu_info;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get CDU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &cdu_info));

    /*
     * alooc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_MAC_MODE_CONFIG, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_MODE_CONFIG_ID, cdu_info.mac_mode_config_id);

    /*
     * set value field
     */
    if (enable)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_MODE_CONFIG, INST_SINGLE, mac_mode);
    }
    else
    {
        /*
         * return to default value - ignore mac_mode argument
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_MODE_CONFIG, INST_SINGLE,
                                     DBAL_ENUM_FVAL_NIF_MAC_MODE_FOUR_PORTS);
    }

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the the MAC mode config in the CDU
 * see .h file
 */
int
imb_cdu_port_mac_mode_config_get(
    int unit,
    bcm_port_t port,
    int *mac_mode)
{
    dnx_algo_port_ethu_access_info_t cdu_info;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get CDU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &cdu_info));

    /*
     * alooc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_MAC_MODE_CONFIG, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_MODE_CONFIG_ID, cdu_info.mac_mode_config_id);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAC_MODE_CONFIG, INST_SINGLE, (uint32 *) mac_mode);

    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable / disable the MAC in the CDU
 * see .h file
 */
int
imb_cdu_port_mac_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * skip assignment to dbal table in case feature is not supported
     */
    if (dnx_data_nif.dbal.feature_get(unit, dnx_data_nif_dbal_active_mac_configure))
    {
        dnx_algo_port_ethu_access_info_t cdu_info;
        uint32 entry_handle_id;

        /*
         * Get CDU access info
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &cdu_info));

        /*
         * alooc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_MAC_CTRL, &entry_handle_id));

        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.ethu_id_in_core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_ID, cdu_info.mac_id);

        /*
         * Set value
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);

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
 * \brief - Reset the Async unit in the CDU
 *
 * see .h file
 */
int
imb_cdu_port_async_unit_reset(
    int unit,
    bcm_port_t port,
    int in_reset)
{
    dnx_algo_port_ethu_access_info_t cdu_info;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get CDU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &cdu_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_MAC_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_ID, cdu_info.mac_id);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_ASYNC_UNIT, INST_SINGLE,
                                 in_reset ? DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
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
 * \brief - Reset the port PM.
 * see .h file
 */
int
imb_cdu_pm_reset(
    int unit,
    int core,
    int cdu_id,
    int in_reset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_PM_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    /*
     * set value fields
     * Note: taking all lanes out-of-reset even if there is only one active lane on the CDU,
     * since the per lane pwrdown is not taking lane mpa into account.
     * e.g. if lane 0 on the chip is connected to rx of lane 1 and tx of lane 2, then when power down lane 0,
     * it can affect two different ports.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_LANES_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_CDU_PM_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_CDU_PM_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    /**
     * set PM_RESET
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_RESET, INST_SINGLE,
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
 * \brief - Enable EEE for the CDU
 * see .h file
 */
int
imb_cdu_eee_enable_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_PM_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEE_ENABLE, INST_SINGLE, enable);
    /*
     * commit value
     */
    /*
     * SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
     */
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable iddq for the CDU
 * see .h file
 */
int
imb_cdu_iddq_enable_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_PM_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IDDQ, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - map NIF port to EGQ interface
 *
 * see .h file
 */
int
imb_cdu_internal_nif_egq_interface_map(
    int unit,
    bcm_core_t core,
    uint32 nif_id,
    uint32 egq_if,
    int enable)
{

    uint32 entry_handle_id;
    uint32 egq_if_max_val;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_EGQ_INTERFACE_MAP, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_ID, nif_id);

    /*
     * get field max valus DBAL table handle
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                    (unit, DBAL_TABLE_NIF_EGQ_INTERFACE_MAP, DBAL_FIELD_EGQ_INTERFACE, FALSE, 0, 0,
                     DBAL_PREDEF_VAL_MAX_VALUE, &egq_if_max_val));

    /*
     * for mapping enable set user value, for mapping disable set the maximal value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_INTERFACE, INST_SINGLE,
                                 (enable ? egq_if : egq_if_max_val));

    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
int
imb_cdu_rx_gear_shift_reset_set(
    int unit,
    bcm_core_t core,
    uint32 cdu_id,
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
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_GEAR_SHIFT_RESET, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, first_lane_in_port);

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

/**
 * \brief - set PRD thresholds per priority. based on the
 *        priority given from the parser, the packet is sent to
 *        the correct RMC. in each RMC there is a thresholds per
 *        priority which is mapped to it.
 *
 * see .h file
 */
int
imb_cdu_prd_threshold_hw_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
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
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_THRESHOLDS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU_ID, cdu_id);
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
imb_cdu_prd_threshold_hw_get(
    int unit,
    bcm_core_t core,
    int cdu_id,
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
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_CDU_THRESHOLDS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU_ID, cdu_id);
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
 * \brief - Configure power down of ILKN FEC of CDU
 *
 */
int
imb_cdu_ilkn_logic_power_down_set(
    int unit,
    bcm_core_t core_id,
    int cdu_id,
    int power_down)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_ILKN_LOGIC_POWER_DOWN, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU_ID, cdu_id);
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

/**
 * \brief - Configure power down of  CDU TX
 *
 */
int
imb_cdu_tx_power_down_set(
    int unit,
    bcm_core_t core_id,
    int cdu_id,
    int power_down)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_TX_POWER_DOWN, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU_ID, cdu_id);
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

/**
 * \brief - Set PM's refclk termination correction.
 * see .h file
 */
int
imb_cdu_refclk_termination_set(
    int unit,
    int core,
    int cdu_id,
    int is_sw_correction)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_PM_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);

    /**
     * set refclk termination
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RTERM_200, INST_SINGLE, (is_sw_correction != 0));
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
imb_cdu_cdmac_block_enable(
    int unit,
    int cdmac_block,
    int enable)
{
    int *cdmac_block_p;
    uint8 *block_valid_p;
    int blk = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Mark cdmac block as valid/non-valid */
    SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, CDMAC_BLOCK_INT, &cdmac_block_p));
    blk = cdmac_block_p[cdmac_block];

    SHR_IF_ERR_EXIT(soc_info_uint8_address_get(unit, BLOCK_VALID, &block_valid_p));
    block_valid_p[blk] = enable ? 1 : 0;

exit:
    SHR_FUNC_EXIT;
}

int
imb_cdu_port_cdmac_valid_enable(
    int unit,
    bcm_port_t port,
    int enable)
{
    bcm_pbmp_t nif_phys;
    bcm_pbmp_t cdmac_block_pbmp;
    int phy;
    int cdmac_block, cdu_block;
    bcm_pbmp_t pm_pmbp, cdmac_pbmp;
    int phy_id, is_active = 0;
    int iter_cnt, nof_mac_lanes;
    int cdmac_disabled;
    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(cdmac_block_pbmp);
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &nif_phys));
    nof_mac_lanes = dnx_data_nif.eth.mac_lanes_nof_get(unit);
    BCM_PBMP_ITER(nif_phys, phy)
    {
        cdmac_block = phy / nof_mac_lanes;
        BCM_PBMP_PORT_ADD(cdmac_block_pbmp, cdmac_block);
    }

    if (enable)
    {
        BCM_PBMP_ITER(cdmac_block_pbmp, cdmac_block)
        {
            SHR_IF_ERR_EXIT(imb_cdu_cdmac_block_enable(unit, cdmac_block, 1));
        }
    }
    else
    {
        BCM_PBMP_ITER(cdmac_block_pbmp, cdmac_block)
        {
            BCM_PBMP_CLEAR(cdmac_pbmp);
            cdu_block = cdmac_block / dnx_data_nif.eth.cdu_mac_nof_get(unit);
            pm_pmbp = dnx_data_nif.eth.pm_properties_get(unit, cdu_block)->phys;
            if (dnx_data_nif.eth.cdu_mac_nof_get(unit) == 1)
            {
                BCM_PBMP_ASSIGN(cdmac_pbmp, pm_pmbp);
            }
            else if (dnx_data_nif.eth.cdu_mac_nof_get(unit) == 2)
            {
                iter_cnt = 0;
                BCM_PBMP_ITER(pm_pmbp, phy_id)
                {
                    iter_cnt++;
                    if (cdmac_block % 2 == 0)
                    {
                        if (iter_cnt <= nof_mac_lanes)
                        {
                            BCM_PBMP_PORT_ADD(cdmac_pbmp, phy_id);
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        if (iter_cnt > nof_mac_lanes)
                        {
                            BCM_PBMP_PORT_ADD(cdmac_pbmp, phy_id);
                        }
                    }
                }
            }

            BCM_PBMP_REMOVE(cdmac_pbmp, nif_phys);
            cdmac_disabled = 1;
            BCM_PBMP_ITER(cdmac_pbmp, phy_id)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_active_get(unit, phy_id, &is_active));
                if (is_active)
                {
                    cdmac_disabled = 0;
                }
            }
            if (cdmac_disabled)
            {
                SHR_IF_ERR_EXIT(imb_cdu_cdmac_block_enable(unit, cdmac_block, 0));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * See .h file
 */
int
imb_cdu_port_xpmd_is_enabled(
    int unit,
    bcm_port_t port,
    uint8 *is_xpmd_enabled)
{
    bcm_pbmp_t phys;
    int first_phy, ethu_id, core;
    imb_dispatch_type_t imb_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
    _SHR_PBMP_FIRST(phys, first_phy);

    SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, first_phy, &ethu_id));

    imb_type = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type;

    *is_xpmd_enabled = ((dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_support_xpmd_if))
                        &&(imb_type == imbDispatchTypeImb_cdu
                           || imb_type == imbDispatchTypeImb_cdu_shr)) ? TRUE : FALSE;

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
imb_cdu_port_xpmd_lane_enable(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable)
{
    uint32 entry_handle_id;
    int iter_phy, lane, ethu_id_in_core, core, ethu_id, cdu_id;
    bcm_pbmp_t phys;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    if (flags & IMB_COMMON_DIRECTION_RX)
    {
        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_XPMD_CTRL, &entry_handle_id));

        _SHR_PBMP_ITER(phys, iter_phy)
        {

            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_lane_info_get(unit, core, iter_phy, &ethu_id_in_core, &lane));
            SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, iter_phy, &ethu_id));

            cdu_id = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type_index;
            /*
             * set key fields
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DIRECTION,
                                       DBAL_ENUM_FVAL_CONNECTION_DIRECTION_RX);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CDU, lane);
            /*
             * set value fields
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_ENABLE, INST_SINGLE, enable);

            /*
             * commit the value
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        /*
         * Clear the handle before re-use
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_CDU_XPMD_CTRL, entry_handle_id));
    }

    if (flags & IMB_COMMON_DIRECTION_TX)
    {

        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_XPMD_CTRL, &entry_handle_id));

        _SHR_PBMP_ITER(phys, iter_phy)
        {

            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_lane_info_get(unit, core, iter_phy, &ethu_id_in_core, &lane));
            SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, iter_phy, &ethu_id));

            cdu_id = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type_index;
            /*
             * set key fields
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DIRECTION,
                                       DBAL_ENUM_FVAL_CONNECTION_DIRECTION_TX);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CDU, lane);
            /*
             * set value fields
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_ENABLE, INST_SINGLE, enable);

            /*
             * commit the value
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
