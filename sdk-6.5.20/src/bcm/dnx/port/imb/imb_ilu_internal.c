/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/cosq/flow_control/cosq_fc.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include "imb_ilu_internal.h"
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/portmod/portmod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dbal/dbal.h>
#include "imb_ile_internal.h"

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/** The number of pools which the polling is expected to complete within */
#define IMB_ILU_INTERNAL_HRF_COUNTER_POLLING_NOF_POLLS        (100)
/** Time-out for HRF counter polling [usec] */
#define IMB_ILU_INTERNAL_HRF_COUNTER_POLLING_TIME_OUT_USEC    (20)
/**
 * See .h file
 */
int
imb_ilu_internal_inband_ilkn_fc_init(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    int tx_cal_len,
    int rx_cal_len,
    int llfc_mode,
    int calendar_mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ILU_PARAMETERS, &entry_handle_id));
    /*
     * Set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
    /*
     * Set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REC_LLFC_EN, INST_SINGLE,
                                 ((llfc_mode == DNX_FC_INB_ILKN_CAL_LLFC_MODE_DISABLE) ? 0 : 1));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GEN_LLFC_FROM_ILKN_STATUS, INST_SINGLE,
                                 ((llfc_mode == DNX_FC_INB_ILKN_CAL_LLFC_MODE_DISABLE) ? 0 : 1));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GEN_LLFC_FROM_RX_DATA_HRF, INST_SINGLE,
                                 ((llfc_mode == DNX_FC_INB_ILKN_CAL_LLFC_MODE_DISABLE) ? 0 : 1));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GEN_LLFC_FROM_ILKN_STATUS, INST_SINGLE,
                                 ((llfc_mode == DNX_FC_INB_ILKN_CAL_LLFC_MODE_DISABLE) ? 0 : 1));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GEN_LLFC_ON_CH_0, INST_SINGLE,
                                 ((llfc_mode == DNX_FC_INB_ILKN_CAL_LLFC_MODE_CH_0_ONLY) ? 1 : 0));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REC_LLFC_ON_CH_0, INST_SINGLE,
                                 ((llfc_mode == DNX_FC_INB_ILKN_CAL_LLFC_MODE_CH_0_ONLY) ? 1 : 0));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GEN_LLFC_EVERY_16_CHS, INST_SINGLE,
                                 ((llfc_mode == DNX_FC_INB_ILKN_CAL_LLFC_MODE_EVERY_16_CH) ? 1 : 0));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GEN_LLFC_EVERY_16_CHS, INST_SINGLE,
                                 ((llfc_mode == DNX_FC_INB_ILKN_CAL_LLFC_MODE_EVERY_16_CH) ? 1 : 0));

    if ((calendar_mode == DNX_FC_CAL_MODE_TX_ENABLE) || (calendar_mode == DNX_FC_CAL_MODE_TX_RX_ENABLE))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_CAL_LEN, INST_SINGLE, (uint32) tx_cal_len);
    }

    if ((calendar_mode == DNX_FC_CAL_MODE_RX_ENABLE) || (calendar_mode == DNX_FC_CAL_MODE_TX_RX_ENABLE))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CHFC_REC_CAL_LEN, INST_SINGLE,
                                     (uint32) rx_cal_len);
    }

    /*
     * Commit the value
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
imb_ilu_internal_port_stop_pm_from_cfc_llfc_enable_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

/** Set HW to stop transmission according LLFC signal received from CFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ILU_REC_LLFC_STOP_PM_FROM_CFC, &entry_handle_id));
    /*
     * Set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
    /*
     * Set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    /*
     * Commit the value
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
imb_ilu_internal_fc_reset_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 in_reset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set HW to put in/out of reset all ILUs */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_ILU_RESET, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_RESET, INST_SINGLE, in_reset);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get number of enabled ports in ilkn core (input excluded).
 *
 * See .h file
 */
int
imb_ilu_port_nof_active_ilkn_ports_in_core_get(
    int unit,
    bcm_port_t port,
    int *nof_active_port)
{
    bcm_port_t master_port;
    bcm_port_t port_iter;
    bcm_pbmp_t ilkn_ports;
    int enable;
    SHR_FUNC_INIT_VARS(unit);

    *nof_active_port = 0;

    /*
     * Get ILKN master ports on core
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_ports_in_core_get(unit, port, &ilkn_ports));

    /*
     * Exclude input port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
    _SHR_PBMP_PORT_REMOVE(ilkn_ports, master_port);

    /*
     *  Count number of active ports on core
     */
    BCM_PBMP_ITER(ilkn_ports, port_iter)
    {
        SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, port_iter, 0, &enable));
        if (enable)
        {
            (*nof_active_port)++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ilu_is_extended_memory_used(
    int unit,
    bcm_port_t port,
    uint8 *is_extended_memory_used)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    int nof_segments;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_nof_segments_get(unit, port, &nof_segments));

    *is_extended_memory_used = ((ilkn_access_info.port_in_core == 0) &&
                                (nof_segments == dnx_data_nif.ilkn.segments_max_nof_get(unit))) ? TRUE : FALSE;

exit:
    SHR_FUNC_EXIT;
}
/**
* \brief - Set ILKN Burst Editor configuration.
* see .h file
*/
int
imb_ilu_ibe_config(
    int unit,
    bcm_port_t port,
    uint32 enable_padding,
    uint32 burst_max,
    uint32 burst_min,
    uint8 is_extended_mem_used)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    dbal_enum_value_field_ilkn_max_burst_size_e dbal_max_burst_size_val;
    dbal_enum_value_field_ilkn_min_burst_size_e dbal_min_burst_size_val;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    /*
     * adjust min and max to dbal values
     */
    switch (burst_max)
    {
        case 256:
            dbal_max_burst_size_val = DBAL_ENUM_FVAL_ILKN_MAX_BURST_SIZE_MAX_BURST_SIZE_256_BYTES;
            break;
        case 512:
            dbal_max_burst_size_val = DBAL_ENUM_FVAL_ILKN_MAX_BURST_SIZE_MAX_BURST_SIZE_512_BYTES;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid burst size %d\n", burst_max);
    }

    switch (burst_min)
    {
        case 64:
            dbal_min_burst_size_val = DBAL_ENUM_FVAL_ILKN_MIN_BURST_SIZE_MIN_BURST_SIZE_64_BYTES;
            break;
        case 128:
            dbal_min_burst_size_val = DBAL_ENUM_FVAL_ILKN_MIN_BURST_SIZE_MIN_BURST_SIZE_128_BYTES;
            break;
        case 256:
            dbal_min_burst_size_val = DBAL_ENUM_FVAL_ILKN_MIN_BURST_SIZE_MIN_BURST_SIZE_256_BYTES;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid burst size %d\n", burst_min);
    }

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_IBE_CONFIG, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_ENABLE_PACKET_PADDING,
                                 ilkn_access_info.port_in_core, enable_padding);
    if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_support_ilu_burst_max))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BURST_MAX, INST_SINGLE, dbal_max_burst_size_val);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BURST_MIN, INST_SINGLE, dbal_min_burst_size_val);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(imb_ilu_hrf_tx_extended_mem_set(unit, port, is_extended_mem_used));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
imb_ilu_ibe_reset(
    int unit,
    bcm_port_t port,
    uint8 in_reset)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_IBE_CONFIG, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_RESET,
                                 ilkn_access_info.port_in_core, in_reset ?
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
* \brief - parser wrap reset
* see .h file
*/
int
imb_ilu_parser_wrap_reset(
    int unit,
    bcm_port_t port,
    uint8 in_reset)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_RX_CONFIG, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PARSER_WRAP_RESET, INST_SINGLE, in_reset ?
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
* \brief - HRF RX PRD reset
* see .h file
*/
int
imb_ilu_prd_reset(
    int unit,
    bcm_port_t port,
    uint8 in_reset)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    int hrf_id, nof_hrf_per_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_HRF_RX_CONFIGURATION, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    nof_hrf_per_port = dnx_data_nif.ilkn.ilkn_rx_hrf_nof_get(unit) / dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);

    for (hrf_id = ilkn_access_info.port_in_core; hrf_id < dnx_data_nif.ilkn.ilkn_rx_hrf_nof_get(unit);
         hrf_id += nof_hrf_per_port)
    {
        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
        /*
         * set value fields
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_RESET, hrf_id, in_reset ?
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
* \brief - Set RX HRF configuration
* see .h file
*/
int
imb_ilu_hrf_rx_config_set(
    int unit,
    bcm_port_t port,
    uint8 is_extended_mem_used,
    uint8 enable_interleaving,
    uint8 drop_small_packets)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    int thresh;
    int hrf_id, nof_hrf_per_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    nof_hrf_per_port = dnx_data_nif.ilkn.ilkn_rx_hrf_nof_get(unit) / dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);

    for (hrf_id = ilkn_access_info.port_in_core; hrf_id < dnx_data_nif.ilkn.ilkn_rx_hrf_nof_get(unit);
         hrf_id += nof_hrf_per_port)
    {

        thresh =
            (hrf_id < 2) ? dnx_data_nif.ilkn.data_rx_hrf_size_get(unit) : dnx_data_nif.ilkn.tdm_rx_hrf_size_get(unit);
        if (is_extended_mem_used)
        {
            thresh *= 2;
        }
        thresh = (thresh * 8) / 10;

        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_HRF_RX_CONFIGURATION, &entry_handle_id));
        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
        /*
         * set value fields
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_AFTER_OVERFLOW, hrf_id, thresh);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_INTERLEAVING, hrf_id,
                                     enable_interleaving);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_SMALL_PACKETS, hrf_id, drop_small_packets);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_EXTENDED_MEMORY, hrf_id / 2,
                                     is_extended_mem_used);

        if (!enable_interleaving)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_DISABLE_GENERATE_ERR, hrf_id, 1);
        }

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
* \brief - reset RX HRF
* see .h file
*/
int
imb_ilu_hrf_rx_segmentation_reset(
    int unit,
    bcm_port_t port,
    uint8 is_complete,
    uint8 in_reset)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    if (is_complete)
    {
        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_HRF_RX_CONFIGURATION, &entry_handle_id));
        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
        /*
         * set value fields
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEGMENTATION_COMPLETE_RESET, INST_SINGLE,
                                     in_reset ? DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                     DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_HRF_RX_CTRL, &entry_handle_id));

    for (ilkn_hrf_type = DBAL_ENUM_FVAL_ILKN_HRF_TYPE_DATA; ilkn_hrf_type < DBAL_NOF_ENUM_ILKN_HRF_TYPE_VALUES;
         ++ilkn_hrf_type)
    {
        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_access_info.port_in_core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_HRF_TYPE, ilkn_hrf_type);
        /*
         * set value fields
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEGMENTATION_HRF_RESET, INST_SINGLE, in_reset ?
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

/*
 * \brief - Set TX HRF configuration
 * see .h file
 */
int
imb_ilu_hrf_tx_config_set(
    int unit,
    bcm_port_t port,
    uint32 burst_max,
    uint8 is_extended_mem_used)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    int nof_credits;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    /*
     * Set Number of credits to send to EGQ
     */
    nof_credits = dnx_data_nif.ilkn.tx_hrf_credits_get(unit);
    if (is_extended_mem_used)
    {
        nof_credits *= 2;
    }
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_HRF_TX_CONFIGURATION, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_CREDITS_TO_EGQ,
                                 ilkn_access_info.port_in_core, nof_credits);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Set TX HRF extended memory configuration
 * see .h file
 */
int
imb_ilu_hrf_tx_extended_mem_set(
    int unit,
    bcm_port_t port,
    uint8 is_extended_mem_used)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_HRF_TX_CONFIGURATION, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_EXTENDED_MEM, INST_SINGLE, is_extended_mem_used);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - ReSet RX HRF controller
 * see .h file
 */
int
imb_ilu_hrf_controller_rx_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type,
    uint8 in_reset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_HRF_RX_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_HRF_TYPE, ilkn_hrf_type);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTROLLER_RESET, INST_SINGLE, in_reset ?
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
 * \brief - ReSet TX HRF controller
 * see .h file
 */
int
imb_ilu_hrf_controller_tx_reset(
    int unit,
    bcm_port_t port,
    uint8 in_reset)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_HRF_TX_CONFIGURATION, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_CONTROLLER_RESET,
                                 ilkn_access_info.port_in_core,
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
 * \brief - ReSet RX and TX HRF controller
 * see .h file
 */
int
imb_ilu_hrf_controller_rx_reset(
    int unit,
    bcm_port_t port,
    uint8 in_reset)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    /*
     * Reset Rx HRFs controllers
     */
    for (ilkn_hrf_type = DBAL_ENUM_FVAL_ILKN_HRF_TYPE_DATA; ilkn_hrf_type < DBAL_NOF_ENUM_ILKN_HRF_TYPE_VALUES;
         ++ilkn_hrf_type)
    {
        SHR_IF_ERR_EXIT(imb_ilu_hrf_controller_rx_hw_set
                        (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, ilkn_hrf_type, in_reset));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - ReSet TX scheduler
 * see .h file
 */
int
imb_ilu_tx_nmg_reset(
    int unit,
    bcm_port_t port,
    uint8 in_reset)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_SCHEDULER_TX_RESET, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_access_info.port_in_core);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NMG_RESET,
                                 INST_SINGLE,
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
 * \brief - Activate both RX/TX configurations if one port with 4 segments is used
 * see .h file
 */
int
imb_ilu_active_segment_set(
    int unit,
    bcm_port_t port,
    uint8 use_half_nof_segments)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_SET_ACTIVE_SEGMENTS, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_HALF_SEGMENTS, INST_SINGLE,
                                 use_half_nof_segments);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - set number of segments to be used by the port
 * see .h file
 */
int
imb_ilu_port_nof_segments_set(
    int unit,
    bcm_port_t port,
    int nof_segments)
{
    uint8 is_extended_memory_used = 0;
    int enable_interleaving;
    uint32 burst_max;
    int nof_active_port;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    SHR_IF_ERR_EXIT(imb_ilu_is_extended_memory_used(unit, port, &is_extended_memory_used));

    SHR_IF_ERR_EXIT(dnx_algo_port_is_ingress_interleave_get(unit, port, &enable_interleaving));
    burst_max = dnx_data_nif.ilkn.properties_get(unit, ilkn_access_info.ilkn_id)->burst_max;

    /*
     * Take HRF controller in reset
     */
    SHR_IF_ERR_EXIT(imb_ilu_hrf_controller_tx_reset(unit, port, IMB_COMMON_IN_RESET));
    SHR_IF_ERR_EXIT(imb_ilu_hrf_controller_rx_reset(unit, port, IMB_COMMON_IN_RESET));

    /*
     * HRF RX Segmentation Reset
     */
    SHR_IF_ERR_EXIT(imb_ilu_port_nof_active_ilkn_ports_in_core_get(unit, port, &nof_active_port));
    SHR_IF_ERR_EXIT(imb_ilu_hrf_rx_segmentation_reset(unit, port, (nof_active_port == 0) ? 1 : 0, IMB_COMMON_IN_RESET));

    /*
     * HRF RX configuration
     */
    SHR_IF_ERR_EXIT(imb_ilu_hrf_rx_config_set(unit, port, is_extended_memory_used, enable_interleaving, 1));

    /*
     * HRF TX configuration
     */
    SHR_IF_ERR_EXIT(imb_ilu_hrf_tx_config_set(unit, port, burst_max, is_extended_memory_used));

    SHR_IF_ERR_EXIT(imb_ilu_hrf_tx_extended_mem_set(unit, port, is_extended_memory_used));

    /*
     * Take HRF controller out of reset
     */
    SHR_IF_ERR_EXIT(imb_ilu_hrf_controller_tx_reset(unit, port, IMB_COMMON_OUT_OF_RESET));
    SHR_IF_ERR_EXIT(imb_ilu_hrf_controller_rx_reset(unit, port, IMB_COMMON_OUT_OF_RESET));
    /*
     * Take HRF RX Segmentation out of Reset
     */
    SHR_IF_ERR_EXIT(imb_ilu_hrf_rx_segmentation_reset(unit, port, 1, IMB_COMMON_OUT_OF_RESET));

    /*
     * Set Active Segment for ilkn big port
     */
    if (ilkn_access_info.port_in_core == 0)
    {
        SHR_IF_ERR_EXIT(imb_ilu_active_segment_set(unit, port, is_extended_memory_used ? 0 : 1));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Init all ilkn ports in the ilkn core scheduler
 *
 * see .h file
 */
int
imb_ilu_port_scheduler_config_hw_init(
    int unit,
    uint32 ilkn_core_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_PORT_SCHEDULER_CTRL, &entry_handle_id));

    /*
     * Loop over all ILKN_PORT_ID
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    /*
     * Clear value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MAX_SCH_WEIGHT_TDM, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MAX_SCH_WEIGHT_HIGH, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_WEIGHT_LOW, INST_SINGLE,
                                 DBAL_ENUM_FVAL_NIF_SCH_WEIGHT_WEIGHT_0);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the ILU port in the specified Scheduler
 *
 * see .h file
 */
int
imb_ilu_port_scheduler_config_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    int is_max_sch_weight_tdm,
    int is_max_sch_weight_high,
    dbal_enum_value_field_nif_sch_weight_e dbal_sch_weight_low)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_PORT_SCHEDULER_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MAX_SCH_WEIGHT_TDM, INST_SINGLE,
                                 is_max_sch_weight_tdm);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MAX_SCH_WEIGHT_HIGH, INST_SINGLE,
                                 is_max_sch_weight_high);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_WEIGHT_LOW, INST_SINGLE, dbal_sch_weight_low);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get scheduler weights of ILU port in NMG Scheduler
 *
 * see .h file
 */
int
imb_ilu_port_scheduler_config_hw_get(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    uint32 *sch_weight_tdm,
    uint32 *sch_weight_high,
    dbal_enum_value_field_nif_sch_weight_e * dbal_sch_weight_low)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_PORT_SCHEDULER_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);

    /*
     * request value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_MAX_SCH_WEIGHT_TDM, INST_SINGLE, sch_weight_tdm);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_MAX_SCH_WEIGHT_HIGH, INST_SINGLE, sch_weight_high);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SCH_WEIGHT_LOW, INST_SINGLE, dbal_sch_weight_low);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the ILU in the specified Scheduler
 *
 * see .h file
 */
int
imb_ilu_core_scheduler_config_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 is_max_sch_weight_tdm,
    uint32 is_max_sch_weight_high,
    dbal_enum_value_field_nif_sch_weight_e dbal_sch_weight_low)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_SCHEDULER_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MAX_SCH_WEIGHT_TDM, INST_SINGLE,
                                 is_max_sch_weight_tdm);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MAX_SCH_WEIGHT_HIGH, INST_SINGLE,
                                 is_max_sch_weight_high);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_WEIGHT_LOW, INST_SINGLE, dbal_sch_weight_low);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve the ILU in the specified Scheduler
 *
 * see .h file
 */
int
imb_ilu_core_scheduler_config_hw_get(
    int unit,
    uint32 ilkn_core_id,
    uint32 *is_max_sch_weight_tdm,
    uint32 *is_max_sch_weight_high,
    dbal_enum_value_field_nif_sch_weight_e * dbal_sch_weight_low)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_SCHEDULER_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);

    /*
     * set value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_MAX_SCH_WEIGHT_TDM, INST_SINGLE,
                               is_max_sch_weight_tdm);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_MAX_SCH_WEIGHT_HIGH, INST_SINGLE,
                               is_max_sch_weight_high);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SCH_WEIGHT_LOW, INST_SINGLE, dbal_sch_weight_low);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the counter decrement threshold value for flush mechanism
 * see .h file
 */
int
imb_ilu_sch_cnt_dec_thres_hw_set(
    int unit,
    uint32 ilkn_core_id,
    int threshold)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_SCH_CNT_DEC_THRESHOLD, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);

    /*
     * request value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILU_SCH_CNT_DEC_THRESHOLD, INST_SINGLE, threshold);

    /*
     * Commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure low priority weight on the ILU Data HRF scheduler
 *
 * see .h file
 */
int
imb_ilu_internal_port_low_prio_scheduler_config_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    dbal_enum_value_field_nif_sch_weight_e dbal_sch_weight_low)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_RX_HRF_CTRL, &entry_handle_id));

    /*
     * choose the required Data HRF
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);

    /*
     * set low priority weight for ILKN port
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOW_PRIO_WEIGHT, INST_SINGLE, dbal_sch_weight_low);

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
 *   Configure low/high priority for Data HRF
 *
 * see .h file
 */
int
imb_ilu_internal_port_hrf_scheduler_config_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    int is_high)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_RX_HRF_CTRL, &entry_handle_id));

    /*
     * choose the required Data HRF
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);

    /*
     * set Data HRF low/high priority
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_HIGH, INST_SINGLE, is_high);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set IS_TDM per ILKN channel
 * see .h file
 */
int
imb_ilu_internal_port_channel_tdm_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    uint32 channel_id,
    int is_tdm)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_TDM, &entry_handle_id));
    /*
     * Set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL_ID, channel_id);

    /*
     * Set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);

    /*
     * Commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set Rx drop data for ILU flush sequence.
 * see .h file
 */
int
imb_ilu_internal_port_rx_drop_data_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type,
    int is_drop_data)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_HRF_RX_CTRL, &entry_handle_id));
    /*
     * Set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_HRF_TYPE, ilkn_hrf_type);

    /*
     * Set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_DATA, INST_SINGLE, is_drop_data);

    /*
     * Commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the scheduler to flush mode for HRF type in ILU.
 * see .h file
 */
int
imb_ilu_internal_port_rx_nmg_flush_context_enable_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 sch_priority,
    int flush_context_enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_RX_SCHEDULER_FLUSH, &entry_handle_id));
    /*
     * Set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_PRIORITY, sch_priority);

    /*
     * Set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLUSH_CONTEXT, INST_SINGLE, flush_context_enable);

    /*
     * Commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable flush mode in scheduler
 * see .h file
 */
int
imb_ilu_internal_port_rx_nmg_flush_mode_enable_hw_set(
    int unit,
    int flush_enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_RX_FLUSH, &entry_handle_id));

    /*
     * Set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLUSH_ENABLE, INST_SINGLE, flush_enable);

    /*
     * Commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set HRF into flush mode.
 * see .h file
 */
int
imb_ilu_internal_port_rx_hrf_flush_mode_enable_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    uint32 ilkn_hrf_type,
    int flush)
{
    uint32 entry_handle_id;
    int temp_port, temp_hrf_type;
    int flush_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_RX_HRF_FLUSH, &entry_handle_id));
    /*
     * Set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);

    /*
     * Set all bits in the bitmap
     * the requested HRF set to flush
     * all other HRFs set to !flush
     */
    for (temp_port = 0; temp_port < dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit); temp_port++)
    {
        for (temp_hrf_type = DBAL_ENUM_FVAL_ILKN_HRF_TYPE_DATA; temp_hrf_type < DBAL_NOF_ENUM_ILKN_HRF_TYPE_VALUES;
             temp_hrf_type++)
        {

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, temp_port);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_HRF_TYPE, temp_hrf_type);

            /*
             * Set value fields
             */
            if (flush)
            {
                flush_value = (temp_port == ilkn_port_id && temp_hrf_type == ilkn_hrf_type ? flush : !flush);
            }
            else
            {
                flush_value = flush;
            }
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLUSH_HRF, INST_SINGLE, flush_value);

            /*
             * Commit the value
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable flush in ILU
 * see .h file
 */
int
imb_ilu_internal_port_rx_core_flush_enable_hw_set(
    int unit,
    uint32 ilkn_core_id,
    int flush_enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_RX_CORE_FLUSH, &entry_handle_id));
    /*
     * Set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);

    /*
     * Set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLUSH_ENABLE, INST_SINGLE, flush_enable);

    /*
     * Commit the value
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
imb_ilu_port_rx_hrf_counter_get(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type,
    uint32 *counter)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_RX_HRF_COUNTER, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);

    /*
     * Set The HRF to count
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, INST_SINGLE, ilkn_port_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_HRF_TYPE, INST_SINGLE, ilkn_hrf_type);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Reset DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_ILU_RX_HRF_COUNTER, entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);

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
imb_ilu_internal_port_rx_hrf_counter_polling(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type)
{
    uint32 entry_handle_id;
    sal_usecs_t time_out;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_RX_HRF_COUNTER, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);

    /*
     * Set The HRF to count
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, INST_SINGLE, ilkn_port_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_HRF_TYPE, INST_SINGLE, ilkn_hrf_type);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Reset DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_ILU_RX_HRF_COUNTER, entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    /*
     * Wait for HRF packets counter to be empty.
     */
    time_out = IMB_ILU_INTERNAL_HRF_COUNTER_POLLING_TIME_OUT_USEC;
    SHR_IF_ERR_EXIT(dnxcmn_polling
                    (unit, time_out, IMB_ILU_INTERNAL_HRF_COUNTER_POLLING_NOF_POLLS, entry_handle_id,
                     DBAL_FIELD_COUNTER, 0));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ilu_internal_nmg_rx_hrf_reset(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type,
    uint8 in_reset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_SCHEDULER_RX_HRF_RESET, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_HRF_TYPE, ilkn_hrf_type);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NMG_RESET,
                                 INST_SINGLE,
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
 * See .h file
 */
int
imb_ilu_internal_nmg_rx_core_reset(
    int unit,
    uint32 ilkn_core_id,
    dbal_enum_value_field_nif_sch_priority_e dbal_sch_prio,
    uint8 in_reset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_SCHEDULER_RX_CORE_RESET, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_PRIORITY, dbal_sch_prio);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NMG_RESET,
                                 INST_SINGLE,
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
 * \brief - set TX start threshold. this threshold represent the
 *        number of memory entries to be accumulated in the MLF
 *        before transmitting towards the PM. This is to prevent
 *        TX MAC starvation and is important for systems with
 *        oversubscription.
 *
 *  see .h file
 */
int
imb_ilu_tx_start_thr_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    uint32 start_thr)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_HRF_TX_CONFIGURATION, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_TX_TRESHOLD, ilkn_port_id, start_thr);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable or disable the ILKN padding feature
 *
 *  see .h file
 */
int
imb_ilu_internal_padding_enable_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_IBE_CONFIG, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_ENABLE_PACKET_PADDING, ilkn_port_id, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the ILKN padding feature enable status
 *
 *  see .h file
 */
int
imb_ilu_internal_padding_enable_hw_get(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    uint32 *enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_IBE_CONFIG, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PORT_ENABLE_PACKET_PADDING, ilkn_port_id, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable PRD hard stage per HRF. this effectively
 *        enable the PRD feature.
 *
 * see .h file
 */
int
imb_ilu_prd_hard_stage_enable_hw_set(
    int unit,
    int ilkn_core_id,
    uint32 hrf,
    uint32 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HRF, hrf);
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
 * \brief - Get HRF RX fifo status
 *
 * see .h file
 */
int
imb_ilu_internal_port_rx_fifo_status_get(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type,
    uint32 *max_occupancy,
    uint32 *fifo_level)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_HRF_RX_FIFO_STATUS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_HRF_TYPE, ilkn_hrf_type);
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
 * \brief - Get HRF TX fifo status
 *
 * see .h file
 */
int
imb_ilu_internal_port_tx_fifo_status_get(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    uint32 *max_occupancy,
    uint32 *fifo_level)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_HRF_TX_FIFO_STATUS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
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
 * \brief - get enable indication for the PRD hard stage per HRF
 *
 * see .h file
 */
int
imb_ilu_prd_hard_stage_enable_hw_get(
    int unit,
    int ilkn_core_id,
    uint32 hrf,
    uint32 *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HRF, hrf);
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
 * \brief - set ITMH TC + DP offsets for the PRD.
 *
 * see .h file
 */
int
imb_ilu_prd_itmh_offsets_hw_set(
    int unit,
    int ilkn_id,
    uint32 tc_offset,
    uint32 dp_offset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_id);
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
 * \brief - get ITMH TC and DP offstes in PRD
 *
 * see .h file
 */
int
imb_ilu_prd_itmh_offsets_hw_get(
    int unit,
    int ilkn_id,
    uint32 *tc_offset,
    uint32 *dp_offset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_id);
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
imb_ilu_prd_ftmh_offsets_hw_set(
    int unit,
    int ilkn_id,
    uint32 tc_offset,
    uint32 dp_offset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_id);
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
imb_ilu_prd_ftmh_offsets_hw_get(
    int unit,
    int ilkn_id,
    uint32 *tc_offset,
    uint32 *dp_offset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_id);
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
imb_ilu_prd_mpls_special_label_enable_hw_set(
    int unit,
    int ilkn_id,
    uint32 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_id);
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
imb_ilu_prd_mpls_special_label_enable_hw_get(
    int unit,
    int ilkn_id,
    uint32 *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_id);
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
 * \brief - set PRD Parser wether to ignore indication of IP
 *        dscp even if packet is identified as IP.
 *
 * see .h file
 */
int
imb_ilu_prd_ignore_ip_dscp_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 ignore_ip_dscp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
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
 * \brief - get from PRD parser indication wether it ignores IP
 *        DSCP
 *
 * see .h file
 */
int
imb_ilu_prd_ignore_ip_dscp_hw_get(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 *ignore_ip_dscp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);

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
imb_ilu_prd_ignore_mpls_exp_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 ignore_mpls_exp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
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
 * \brief - get from PRD parser indication wether it ignores
 *        MPLS EXP
 *
 * see .h file
 */
int
imb_ilu_prd_ignore_mpls_exp_hw_get(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 *ignore_mpls_exp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);

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
 * \brief - set PRD Parser wether to ignore inner tag PCP DEI
 *        indication even if packet is identified as double
 *        tagged
 *
 * see .h file
 */
int
imb_ilu_prd_ignore_inner_tag_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 ignore_inner_tag)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
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
 * \brief - get from PRD parser indication wether it ignores
 *        inner VLAN tag
 *
 * see .h file
 */
int
imb_ilu_prd_ignore_inner_tag_hw_get(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 *ignore_inner_tag)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);

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
 * \brief - set PRD Parser wether to ignore outer tag PCP DEI
 *        indication even if packet is identified as VLAN tagged
 *
 * see .h file
 */
int
imb_ilu_prd_ignore_outer_tag_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 ignore_outer_tag)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
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
 * \brief - get from PRD parser indication wether it ignores
 *        outer VLAN tag
 *
 * see .h file
 */
int
imb_ilu_prd_ignore_outer_tag_hw_get(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 *ignore_outer_tag)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);

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
 *        be later overriden by the priority from the soft
 *        stage.
 *
 * see .h file
 */
int
imb_ilu_prd_default_priority_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 default_priority)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
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
imb_ilu_prd_default_priority_hw_get(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 *default_priority)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
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
imb_ilu_prd_soft_stage_enable_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 enable_eth,
    uint32 enable_tm)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
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
imb_ilu_prd_soft_stage_enable_hw_get(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 *enable_eth,
    uint32 *enable_tm)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
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
 *        extander, the outer tag size should be set to 8B,
 *        otherwise 4B
 *
 * see .h file
 */
int
imb_ilu_prd_outer_tag_size_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 outer_tag_size)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
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
imb_ilu_prd_outer_tag_size_hw_get(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 *outer_tag_size)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
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
 *        the ilkn port
 *
 * see .h file
 */
int
imb_ilu_prd_port_type_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 prd_port_type)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
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
 * \brief - get PRD port type for the ilkn port
 *
 * see .h file
 */
int
imb_ilu_prd_port_type_hw_get(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 *prd_port_type)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PORT_TYPE, INST_SINGLE, prd_port_type);
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
 *        the map priorty value to be returned per TC + DP
 *        values
 *
 * see .h file
 */
int
imb_ilu_prd_map_tm_tc_dp_hw_set(
    int unit,
    bcm_port_t port,
    uint32 tc,
    uint32 dp,
    uint32 priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_TM_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
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
 *        stage). get the priority given for a conbination of
 *        TC+DP.
 *
 * see .h file
 */
int
imb_ilu_prd_map_tm_tc_dp_hw_get(
    int unit,
    bcm_port_t port,
    uint32 tc,
    uint32 dp,
    uint32 *priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_TM_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
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
imb_ilu_prd_map_ip_dscp_hw_set(
    int unit,
    bcm_port_t port,
    uint32 dscp,
    uint32 priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_IP_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DSCP, dscp);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
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
 *        stage). get the priorty returned for a specific DSCP
 *
 * See .h file
 */
int
imb_ilu_prd_map_ip_dscp_hw_get(
    int unit,
    bcm_port_t port,
    uint32 dscp,
    uint32 *priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_IP_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DSCP, dscp);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the ILKN (VLAN) Prioroity table (PRD hard
 *        stage). set a priority value to match a spcific
 *        PCP+DEI combination
 *
 * see .h file
 */
int
imb_ilu_prd_map_eth_pcp_dei_hw_set(
    int unit,
    bcm_port_t port,
    uint32 pcp,
    uint32 dei,
    uint32 priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_VLAN_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP, pcp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DEI, dei);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - get information from the ILKN (VLAN) priority table
 *        (PRD hard stage). get the priorty returned for a
 *        specific combination of PCP+DEI
 *
 * see .h file
 */
int
imb_ilu_prd_map_eth_pcp_dei_hw_get(
    int unit,
    bcm_port_t port,
    uint32 pcp,
    uint32 dei,
    uint32 *priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_VLAN_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP, pcp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DEI, dei);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
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
imb_ilu_prd_map_mpls_exp_hw_set(
    int unit,
    bcm_port_t port,
    uint32 exp,
    uint32 priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_MPLS_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EXP, exp);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
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
imb_ilu_prd_map_mpls_exp_hw_get(
    int unit,
    bcm_port_t port,
    uint32 exp,
    uint32 *priority)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_MPLS_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EXP, exp);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD thresholds per priority. based on the
 *        priority given from the parser, the packet is sent to
 *        the correct HRF. in each HRF there are thresholds per
 *        priority which are mapped to it.
 *
 * see .h file
 */
int
imb_ilu_prd_threshold_hw_set(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 threshold)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_THRESHOLDS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_access_info.port_in_core);
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
 *        a specific HRF.
 *
 * see .h file
 */
int
imb_ilu_prd_threshold_hw_get(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 *threshold)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_THRESHOLDS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_access_info.port_in_core);
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
 * \brief - set one of the TPID values per port profile to be recognized
 *        by the PRD parser. if a packet TPID is not recognized,
 *        the packet will get the default priority. each port
 *        can have four TPID values.
 *
 * see .h file
 */
int
imb_ilu_prd_tpid_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 tpid_index,
    uint32 tpid_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_TPID, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
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
imb_ilu_prd_tpid_hw_get(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 tpid_index,
    uint32 *tpid_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_TPID, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_port_id);
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
 * \brief - get PRD drop counter value, per HRF
 *
 * see .h file
 */
int
imb_ilu_prd_drop_count_hw_get(
    int unit,
    bcm_port_t port,
    uint32 is_tdm,
    uint64 *count)
{
    uint32 entry_handle_id;
    uint64 overflow_count;
    int nof_ports_in_core, hrf;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    nof_ports_in_core = dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_HRF_OVFL_COUNTERS, &entry_handle_id));
    /*
     * set key fields
     */
    hrf = (is_tdm * nof_ports_in_core) + ilkn_access_info.port_in_core;
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HRF, hrf);
    /*
     * request for value field
     */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_DROP_COUNTER, INST_SINGLE, &overflow_count);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (!is_tdm)
    {
        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_COUNTERS, &entry_handle_id));
        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_access_info.port_in_core);

        /*
         * request for value field
         */
        dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_DROP_COUNTER, INST_SINGLE, count);
        /*
         * commit value
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        *count += overflow_count;
    }

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
imb_ilu_prd_tcam_entry_key_offset_base_hw_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_base)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_ETHER_TYPE_CODE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
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
imb_ilu_prd_tcam_entry_key_offset_base_hw_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *offset_base)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_ETHER_TYPE_CODE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
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
imb_ilu_prd_ether_type_size_hw_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_size)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_ETHER_TYPE_CODE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
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
imb_ilu_prd_ether_type_size_hw_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_size)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_ETHER_TYPE_CODE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
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
imb_ilu_prd_tcam_entry_key_offset_hw_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 offset_value)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_TCAM_KEY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
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
imb_ilu_prd_tcam_entry_key_offset_hw_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 *offset_value)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_TCAM_KEY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
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
imb_ilu_prd_tcam_entry_hw_set(
    int unit,
    bcm_port_t port,
    uint32 entry_index,
    const dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_TCAM, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
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
imb_ilu_prd_tcam_entry_hw_get(
    int unit,
    bcm_port_t port,
    uint32 entry_index,
    dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_TCAM, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
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
 *        automatically gets the higest priroity (3). each
 *        packet is compared against all control frame
 *        properties of the ilu
 *
 * see .h file
 */
int
imb_ilu_prd_control_frame_hw_set(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    const bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_CONTROL_FRAME, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
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
imb_ilu_prd_control_frame_hw_get(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_CONTROL_FRAME, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
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
imb_ilu_prd_mpls_special_label_hw_set(
    int unit,
    bcm_port_t port,
    uint32 special_label_index,
    const bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_MPLS_SPECIAL_LABEL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
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
imb_ilu_prd_mpls_special_label_hw_get(
    int unit,
    bcm_port_t port,
    uint32 special_label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_MPLS_SPECIAL_LABEL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
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

/**
 * \brief - set configurable ether type to a ether type code.
 *        the ether type codes are meaningful in the PRD soft
 *        stage (TCAM).
 *
 * See .h file
 */
int
imb_ilu_prd_custom_ether_type_hw_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_val)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ilu access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_CONFIGURABLE_ETHER_TYPE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
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
imb_ilu_prd_custom_ether_type_hw_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_val)
{
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ilu access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_ILU_CONFIGURABLE_ETHER_TYPE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
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
 * \brief - Configure power down of ILE
 */
int
imb_ilu_power_down_set(
    int unit,
    int ilkn_core_id,
    int power_down)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_POWER_DOWN, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_core_id);
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
