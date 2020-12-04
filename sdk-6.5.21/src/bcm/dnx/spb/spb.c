/** \file src/bcm/dnx/spb/spb.c
 * 
 *
 * Sram Packet Buffer configuration implementation
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/spb/spb.h>
#include <soc/mem.h> 
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_spb.h>
/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/**
 * \brief - set spb dynamic configuration
 * 
 * \param [in] unit - unit number
 * \param [in] enable_data_path - enable data path
 * \param [in] max_buffers_threshold - max buffers threshold per 
 *        packet
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_spb_dynamic_configuration_set(
    int unit,
    uint8 enable_data_path,
    uint32 max_buffers_threshold)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_DYNAMIC_CONFIGURATION, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_DATA_PATH, INST_SINGLE, enable_data_path);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_MAX_BUFFERS_THRESHOLD, INST_SINGLE, max_buffers_threshold);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_spb_context_mru_default_get_f(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field32_set(unit, SPB_CONTEXT_MRUm, entry_data, MAX_SIZEf, 0x2fff);
    soc_mem_field32_set(unit, SPB_CONTEXT_MRUm, entry_data, MAX_ORG_SIZEf,
                        dnx_data_ingr_congestion.info.max_jumbo_packet_size_get(unit));
    soc_mem_field32_set(unit, SPB_CONTEXT_MRUm, entry_data, MIN_SIZEf, 0x20);
    soc_mem_field32_set(unit, SPB_CONTEXT_MRUm, entry_data, MIN_ORG_SIZEf, 0x20);

    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_spb_pdm_fpc_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PDM_INIT, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FPC_INIT, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PDM_INIT, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FPC_INIT, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PDM_INIT, entry_handle_id));
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, 1000000, 1000000, entry_handle_id, DBAL_FIELD_FPC_INIT_STATUS, 0));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PDM_INIT, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FPC_ALLOCATE_EN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_spb_drop_convert_priority_to_dbal(
    int unit,
    uint32 priority,
    dbal_enum_value_field_sram_packet_buffer_drop_priority_e * dbal_priority)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (priority)
    {
        case 0:
            *dbal_priority = DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_HIGH;
            break;
        case 1:
            *dbal_priority = DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_MID;
            break;
        case 2:
            *dbal_priority = DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_LOW;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal priority value %d\n", priority);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_spb_drop_convert_priority_from_dbal(
    int unit,
    dbal_enum_value_field_sram_packet_buffer_drop_priority_e dbal_priority,
    uint32 *priority)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (dbal_priority)
    {
        case DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_HIGH:
            *priority = 0;
            break;
        case DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_MID:
            *priority = 1;
            break;
        case DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_LOW:
            *priority = 2;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal priority value %d\n", dbal_priority);
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_spb_drop_threshold_set(
    int unit,
    int priority,
    uint32 value)
{

    uint32 entry_handle_id;
    dbal_fields_e dbal_priority[] =
        { DBAL_FIELD_THRESHOLD_HIGH_PRIORITY, DBAL_FIELD_THRESHOLD_MID_PRIORITY, DBAL_FIELD_THRESHOLD_LOW_PRIORITY };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_DROP_THRESHOLDS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_priority[priority], INST_SINGLE, value);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_spb_drop_threshold_get(
    int unit,
    int priority,
    uint32 *value)
{

    uint32 entry_handle_id;
    dbal_fields_e dbal_priority[] =
        { DBAL_FIELD_THRESHOLD_HIGH_PRIORITY, DBAL_FIELD_THRESHOLD_MID_PRIORITY, DBAL_FIELD_THRESHOLD_LOW_PRIORITY };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_DROP_THRESHOLDS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, dbal_priority[priority], INST_SINGLE, value);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_spb_drop_reassembly_context_profile_map_set(
    int unit,
    int core,
    uint32 reassembly_context,
    uint32 profile_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_DROP_CONTEXT_PROFILE_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REASSEMBLY_CONTEXT, reassembly_context);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, INST_SINGLE, profile_id);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_spb_drop_reassembly_context_profile_map_get(
    int unit,
    int core,
    uint32 reassembly_context,
    uint32 *profile_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_DROP_CONTEXT_PROFILE_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REASSEMBLY_CONTEXT, reassembly_context);

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, INST_SINGLE, profile_id);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_spb_drop_profile_priority_map_set(
    int unit,
    uint32 profile_id,
    uint32 tc,
    uint32 dp,
    uint32 priority)
{
    dbal_enum_value_field_sram_packet_buffer_drop_priority_e dbal_priority;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_spb_drop_convert_priority_to_dbal(unit, priority, &dbal_priority));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_DROP_PRIORITY_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_PACKET_BUFFER_DROP_PRIORITY, INST_SINGLE,
                                 (uint32) dbal_priority);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_spb_drop_profile_priority_map_get(
    int unit,
    uint32 profile_id,
    uint32 tc,
    uint32 dp,
    uint32 *priority)
{
    dbal_enum_value_field_sram_packet_buffer_drop_priority_e dbal_priority;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_DROP_PRIORITY_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SRAM_PACKET_BUFFER_DROP_PRIORITY, INST_SINGLE,
                               (uint32 *) &dbal_priority);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_spb_drop_convert_priority_from_dbal(unit, dbal_priority, priority));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - init spb drop threshold values
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_spb_drop_thresholds_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_DROP_THRESHOLDS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_HIGH_PRIORITY, INST_SINGLE,
                                 dnx_data_ingr_congestion.sram_buffer.drop_tresholds_get(unit, 0)->value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_MID_PRIORITY, INST_SINGLE,
                                 dnx_data_ingr_congestion.sram_buffer.drop_tresholds_get(unit, 1)->value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_LOW_PRIORITY, INST_SINGLE,
                                 dnx_data_ingr_congestion.sram_buffer.drop_tresholds_get(unit, 2)->value);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - init OCB last in chain
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_spb_ocb_last_in_chain_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 ocb_index;
    uint32 nof_ocb_blocks = dnx_data_spb.ocb.nof_blocks_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_OCB_LAST_IN_CHAIN, &entry_handle_id));

    for (ocb_index = 0; ocb_index < nof_ocb_blocks; ++ocb_index)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OCB_INDEX, ocb_index);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LAST_IN_CHAIN, INST_SINGLE,
                                     dnx_data_spb.ocb.last_in_chain_get(unit, ocb_index)->is_last_in_chain);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_spb_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_spb_dynamic_configuration_set(unit, 1, 48));
    SHR_IF_ERR_EXIT(dnx_spb_pdm_fpc_init(unit));
    SHR_IF_ERR_EXIT(dnx_spb_drop_thresholds_init(unit));
    if (dnx_data_spb.ocb.feature_get(unit, dnx_data_spb_ocb_blocks_in_ring))
    {
        SHR_IF_ERR_EXIT(dnx_spb_ocb_last_in_chain_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_spb_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}
