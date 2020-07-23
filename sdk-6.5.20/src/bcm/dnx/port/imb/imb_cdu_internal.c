/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <shared/shrextend/shrextend_debug.h>
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
 * \brief - get all ilkn master logical ports on a CDU
 *
 * see .h file
 */
int
imb_cdu_ilkn_master_ports_get(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t * cdu_ilkn_ports)
{
    dnx_algo_port_ethu_access_info_t cdu_info;
    int i, pm_i, nof_ilkn_lanes_on_cdu, is_over_fabric;
    bcm_port_t ilkn_port_i;
    bcm_pbmp_t all_ilkn_ports, cdu_phys, pm_phys, ilkn_lanes;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * clear output pbmp
     */
    BCM_PBMP_CLEAR(*cdu_ilkn_ports);
    /*
     * get port cdu info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &cdu_info));
    /*
     * get CDU lanes
     */
    BCM_PBMP_CLEAR(cdu_phys);
    for (i = 0; i < dnx_data_nif.eth.nof_pms_in_cdu_get(unit); i++)
    {
        pm_i = dnx_data_nif.eth.ethu_properties_get(unit, cdu_info.ethu_id)->pms[i];
        pm_phys = dnx_data_nif.eth.pm_properties_get(unit, pm_i)->phys;
        BCM_PBMP_OR(cdu_phys, pm_phys);
    }
    /*
     * get all ilkn logical ports on our specific device core (0 or 1)
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, cdu_info.core, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                     &all_ilkn_ports));
    BCM_PBMP_ITER(all_ilkn_ports, ilkn_port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, ilkn_port_i, 0, &ilkn_lanes));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, ilkn_port_i, &is_over_fabric));
        /*
         * check if ilkn port is in CDU range.
         * Note that ilkn-over-fabric might overlap the same links as the CDU so need to confirm ilkn is not over fabric
         */
        BCM_PBMP_AND(ilkn_lanes, cdu_phys);
        BCM_PBMP_COUNT(ilkn_lanes, nof_ilkn_lanes_on_cdu);
        if ((nof_ilkn_lanes_on_cdu > 0) && (!is_over_fabric))
        {
            BCM_PBMP_PORT_ADD(*cdu_ilkn_ports, ilkn_port_i);
        }
    }

exit:
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
        dbal_entry_value_field_predefine_value_set(unit,
                                                   entry_handle_id, DBAL_FIELD_MAC_MODE_CONFIG, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_DEFAULT_VALUE);
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
 * \brief - Get the the MAC mode in the CDU
 * see .h file
 */
int
imb_cdu_port_mac_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
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
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_MAC_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_info.core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_info.ethu_id_in_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_ID, cdu_info.mac_id);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, (uint32 *) enable);

    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

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

#undef _ERR_MSG_MODULE_NAME
