/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DBAL_INTERNAL_H_INCLUDED
#define DBAL_INTERNAL_H_INCLUDED

#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_str.h>
#include <soc/dnx/swstate/auto_generated/access/dbal_access.h>
#include <soc/dnx/swstate/auto_generated/types/dbal_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include "dbal_db_init.h"
#include <src/soc/dnx/flush/flush_internal.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>
#include <src/soc/dnx/dbal/dbal_formula_cb.h>
#include <src/soc/dnx/dbal/dbal_init/auto_generated/dbal_init_cb_formula.h>
#include <soc/dnxc/dnxc_verify.h>

uint8 dbal_is_j2c_a0(
    int unit);
uint8 dbal_is_jr2_a0(
    int unit);
uint8 dbal_is_jr2_b0(
    int unit);
uint8 dbal_is_q2a_a0(
    int unit);
uint8 dbal_is_q2a_b0(
    int unit);
uint8 dbal_is_j2p_a0(
    int unit);

#define DBAL_IS_J2C_A0                               (dbal_is_j2c_a0(unit))
#define DBAL_IS_JR2_A0                                (dbal_is_jr2_a0(unit))
#define DBAL_IS_JR2_B0                                (dbal_is_jr2_b0(unit))
#define DBAL_IS_Q2A_A0                                (dbal_is_q2a_a0(unit))
#define DBAL_IS_Q2A_B0                                (dbal_is_q2a_b0(unit))
#define DBAL_IS_J2P_A0                               (dbal_is_j2p_a0(unit))

uint8 dbal_is_dnx2(
    int unit);

#define DBAL_IS_DNX2(unit)                           (dbal_is_dnx2(unit))


#define DBAL_RUNTIME_VERIFICATIONS_ENABLED(_unit) DNXC_VERIFY_ALLOWED_GET(_unit)


#define DBAL_VERIFICATIONS(_expr)                \
if(DBAL_RUNTIME_VERIFICATIONS_ENABLED(unit))     \
{                                                \
  SHR_IF_ERR_EXIT(_expr);                        \
}


#define BSL_SOURCE_DBALACCESS_DNX_CHK \
    (BSL_LAYER_ENC(bslLayerSocdnx) | BSL_SOURCE_ENC(bslSourceDbalaccessdnx) | BSL_SEVERITY_ENC(bslSeverityInfo))
#define BSL_SOURCE_DBAL_DNX_CHK \
    (BSL_LAYER_ENC(bslLayerSocdnx) | BSL_SOURCE_ENC(bslSourceDbaldnx) | BSL_SEVERITY_ENC(bslSeverityInfo))

#define GET_FIELD_TYPE(_field_id, _field_type)                                                                          \
do                                                                                                                      \
{                                                                                                                       \
    if  ((_field_id) < DBAL_NOF_FIELDS)                                                                                 \
    {                                                                                                                   \
        (*_field_type) = dbal_field_id_to_type[(_field_id)];                                                            \
    }                                                                                                                   \
    else                                                                                                                \
    {                                                                                                                   \
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.field_type.get(unit, (_field_id - DBAL_NOF_FIELDS), (_field_type)));   \
    }                                                                                                                   \
} while (0)


typedef enum
{
    
    ALL_KEY_FIELDS_MUST_EXIST = SAL_BIT(1),

    
    RESULT_FIELD_MUST_EXIST = SAL_BIT(2),

    
    RESULT_FIELD_NOT_ALLOWED = SAL_BIT(3),

    
    CORE_COULD_BE_NOT_INITIALIZED = SAL_BIT(4),

    
    VALIDATE_ALLOCATOR_FIELD = SAL_BIT(5),

    
    CORE_ID_MUST_BE_SET_TO_ONE_CORE_ONLY = SAL_BIT(6)
} dbal_action_validation_flags_e;


#define DBAL_DUMP(meta, log, file)                                \
do                                                                \
{                                                                 \
    if (log)                                                      \
    {                                                             \
        LOG_CLI( (BSL_META("%s"),meta) );                         \
    }                                                             \
    if (file != NULL)                                             \
    {                                                             \
        sal_fwrite(meta, strlen(meta), 1, file);                  \
    }                                                             \
} while (0);

#define DBAL_ITERATE_OVER_CORES(entry_handle, core_id)                              \
    int first_core = 0, nof_cores = DBAL_MAX_NUM_OF_CORES;                          \
    if(DBAL_MAX_NUM_OF_CORES > 1)                                                         \
    {                                                                               \
        if (entry_handle->table->core_mode == DBAL_CORE_MODE_SBC)                   \
        {                                                                           \
            nof_cores = 1;                                                          \
            if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_MDB)       \
            {                                                                       \
                nof_cores = DBAL_MAX_NUM_OF_CORES;                                  \
            }                                                                       \
        }                                                                           \
        else if ((entry_handle->core_id != DBAL_CORE_ALL) &&                        \
                 (entry_handle->core_id != DBAL_CORE_NOT_INTIATED))                 \
        {                                                                           \
            first_core = entry_handle->core_id;                                     \
            nof_cores = 1;                                                          \
        }                                                                           \
    }                                                                               \
    else if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_MDB)          \
    {                                                                               \
        first_core = 0;                                                             \
        nof_cores = entry_handle->table->nof_physical_tables;                       \
    }                                                                               \
    for(core_id = first_core; core_id < first_core + nof_cores; core_id++)


#define DBAL_FIELD_ERR_HANDLE(is_error, string_to_save)                                           \
do                                                                                                \
{                                                                                                 \
    if(is_error)                                                                                  \
    {                                                                                             \
        if(SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE) &&  \
           entry_handle->cur_res_type != DBAL_RESULT_TYPE_NOT_INITIALIZED)\
        {                                                                                             \
            LOG_ERROR(BSL_LOG_MODULE,(BSL_META_U(unit,"%s - field %s table %s res type %s\n\n"),                          \
            string_to_save, dbal_field_to_string(unit, field_id), entry_handle->table->table_name,                        \
                                      entry_handle->table->multi_res_info[entry_handle->cur_res_type].result_type_name)); \
        }                                                                                             \
        else                                                                                          \
        {                                                                                             \
            LOG_ERROR(BSL_LOG_MODULE,(BSL_META_U(unit,"%s - field %s table %s\n\n"),                  \
            string_to_save, dbal_field_to_string(unit, field_id), entry_handle->table->table_name));  \
        }                                                                                             \
        entry_handle->error_info.field_id = field_id;                                                 \
        entry_handle->error_info.error_exists = _SHR_E_INTERNAL;                                        \
        if(!dbal_image_name_is_std_1(unit))                                                           \
        {                                                                                             \
            entry_handle->error_info.error_exists = _SHR_E_UNAVAIL;                                   \
        }                                                                                             \
        return;                                                                                       \
    }                                                                                                 \
} while (0);

#define DBAL_FIELD_ERR_HANDLE_NO_RETURN(error_code, string_to_save)                                           \
do                                                                                                \
{                                                                                                 \
    if(error_code)                                                                                  \
    {                                                                                             \
        LOG_ERROR(BSL_LOG_MODULE,(BSL_META_U(unit,"%s - field %s table %s\n\n"),                  \
        string_to_save, dbal_field_to_string(unit, field_id), entry_handle->table->table_name));  \
        entry_handle->error_info.field_id = field_id;                                             \
        entry_handle->error_info.error_exists = error_code;                                       \
    }                                                                                             \
} while (0);

#define  DBAL_MASKED_TABLE_TYPE(table_type)                                                     \
                          ((table_type != DBAL_TABLE_TYPE_TCAM) &&                              \
                          (table_type != DBAL_TABLE_TYPE_TCAM_DIRECT) &&                        \
                          (table_type != DBAL_TABLE_TYPE_TCAM_BY_ID) &&                        \
                          (table_type != DBAL_TABLE_TYPE_LPM))


#define DBAL_MASK_ENTRY_VALID_TABLE_CHECK(table_type, access_method)                            \
do                                                                                              \
{                                                                                               \
    DBAL_FIELD_ERR_HANDLE(DBAL_MASKED_TABLE_TYPE(table_type),                                   \
                          "Illegal use for mask APIs in table type");                           \
} while (0);


#define DBAL_FIELD_ERR_RETURN_HANDLE(is_error, string_to_save)                                  \
do                                                                                                \
{                                                                                                 \
    if(is_error)                                                                                  \
    {                                                                                             \
        LOG_ERROR(BSL_LOG_MODULE,(BSL_META_U(unit,"%s - field %s table %s\n\n"),                  \
        string_to_save, dbal_field_to_string(unit, field_id), entry_handle->table->table_name));  \
        entry_handle->error_info.field_id = field_id;                                             \
        entry_handle->error_info.error_exists = _SHR_E_INTERNAL;                                  \
        return _SHR_E_INTERNAL;                                                                   \
    }                                                                                             \
} while (0);

#define DBAL_ACCESS_PRINTS_HL                                   ("HL-----")
#define DBAL_ACCESS_PRINTS_MDB                                  ("MDB----")
#define DBAL_ACCESS_PRINTS_SW                                   ("SW-----")
#define DBAL_ACCESS_PRINTS_PEMLA                                ("PEML---")
#define DBAL_ACCESS_PRINTS_TCAM_CS                              ("TCAM_CS")
#define DBAL_ACCESS_PRINTS_KBP                                  ("KBP----")


#define DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(is_start, sevirity, type, log_action)            \
do                                                                                      \
{                                                                                       \
    int rv = bsl_check(bslLayerSocdnx, bslSourceDbalaccessdnx, bslSeverityInfo, unit);  \
    if (rv)                                                                             \
    {                                                                                   \
        if(dbal_logger_is_enable(unit, entry_handle->table_id))                             \
        {                                                                               \
            if (is_start)                                                                   \
            {                                                                               \
                LOG_CLI((BSL_META("\n")));                                                  \
            }                                                                                                     \
            LOG_CLI((BSL_META("---------------------------------ACCESS-%s------------------------------------\n"), is_start? type:"-------"));  \
            if(is_start)                                                                    \
            {                                                                               \
                LOG_CLI((BSL_META("Action: %s\n"), (log_action==0)?"Commit":(log_action==1)?"Get":"Clear"));  \
            }                                                                               \
            if (!is_start )                                                                 \
            {                                                                               \
                LOG_CLI((BSL_META("\n")));                                                  \
            }                                                                               \
        }                                                                               \
    }                                                                                   \
}                                                                                       \
while (0)

#define DBAL_PRINT_FRAME_FOR_API_PRINTS(is_start, sevirity)                             \
do                                                                                      \
{                                                                                       \
    int rv = bsl_check(bslLayerSocdnx, bslSourceDbaldnx, bslSeverityInfo, unit);\
    if (rv)                                                                             \
    {                                                                                   \
        if (is_start)                                                                   \
        {                                                                               \
            LOG_CLI((BSL_META("\n")));                                                  \
        }                                                                               \
        LOG_CLI((BSL_META("**************************************API**************************************\n")));  \
        if (!is_start)                                                                  \
        {                                                                               \
            LOG_CLI((BSL_META("\n")));                                                  \
        }                                                                               \
    }                                                                                   \
}                                                                                       \
while (0)


#define DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_ALLOC(_entry_handle) \
do \
{ \
    if ((_entry_handle).user_output_info == NULL) \
    { \
        DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_ALLOC_FORCE((_entry_handle)); \
    } \
} while (0)


#define DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_ALLOC_FORCE(_entry_handle) \
do \
{ \
    (_entry_handle).user_output_info = \
        sal_alloc(sizeof(dbal_user_output_info_t) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE, \
                  (_entry_handle).table->table_name); \
        sal_memset((_entry_handle).user_output_info, 0x0, \
                   sizeof(dbal_user_output_info_t) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE); \
} while (0)


#define DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_COPY(_src_entry_handle, _dst_entry_handle) \
do \
{ \
    if ((_src_entry_handle).user_output_info != NULL) \
    { \
        if ((_dst_entry_handle).user_output_info == NULL) \
        { \
            DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_ALLOC_FORCE((_dst_entry_handle)); \
        } \
        sal_memcpy((_dst_entry_handle).user_output_info, (_src_entry_handle).user_output_info, \
                   sizeof(dbal_user_output_info_t) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE); \
    } \
} while (0)


#define DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_FREE(_entry_handle) \
do \
{ \
    if ((_entry_handle).user_output_info != NULL) \
    { \
        sal_free((_entry_handle).user_output_info); \
        (_entry_handle).user_output_info = NULL; \
    } \
} while (0)



#define DBAL_COLLECTION_PRINTS_MODE              0

#define DBAL_COLLECTION_PRINTS(_expr)            \
if(DBAL_COLLECTION_PRINTS_MODE)                  \
{                                                \
  LOG_CLI(_expr);                                \
}

#define DBAL_COLLECTION_NOF_ENTRIES_IN_POOL(index)      (DBAL_COLLECTION_POOL_SIZE? (index)%DBAL_COLLECTION_POOL_SIZE:0)

#define DBAL_COLLECTION_POOL_ID_GET(index)              (DBAL_COLLECTION_POOL_SIZE? (index)/DBAL_COLLECTION_POOL_SIZE:0)

#define DBAL_COLLECTION_IS_POOL_FULL(index)             (DBAL_COLLECTION_POOL_SIZE?(((index+1))%DBAL_COLLECTION_POOL_SIZE) == 0:0)

#define DBAL_HANDLE_IS_COLLECTION_MODE(_entry_handle)   (_entry_handle->handle_id > DBAL_SW_NOF_ENTRY_HANDLES)

#define DBAL_HANDLE_POOL_ID_GET(_entry_handle)          DBAL_COLLECTION_POOL_ID_GET(_entry_handle->handle_id- DBAL_SW_NOF_ENTRY_HANDLES)

#define DBAL_HANDLE_IS_POOL_FULL(_entry_handle)         DBAL_COLLECTION_IS_POOL_FULL(_entry_handle->handle_id- DBAL_SW_NOF_ENTRY_HANDLES)

#define DBAL_COLLECTION_SIGNAL_DESTROY                  100
#define DBAL_COLLECTION_SIGNAL_PROCESS_POOL             0

typedef struct
{
    int unit;
    int mutex_id;
} dbal_collection_worker_thread_params_t;


#define DBAL_SW_STATE_TABLES dbal_db.sw_tables


#define DBAL_SW_STATE_TBL_PROP dbal_db.tables_properties


#define DBAL_SW_STATE_MDB_PHY_TBL dbal_db.mdb_phy_tables


#define DBAL_DYNAMIC_TBL_IN_SW_STATE dbal_db.dbal_dynamic_tables


#define DBAL_DYNAMIC_XML_TBL_IN_SW_STATE dbal_db.dbal_dynamic_xml_tables


#define DBAL_SW_STATE_FIELD_PROP dbal_db.dbal_fields


#define DBAL_SW_STATE_PPMC_RES_TYPE dbal_db.dbal_ppmc_table_res_types


#define DBAL_DYNAMIC_XML_TABLES_ENABLED !dbal_image_name_is_std_1(unit)


#define DBAL_FIELD_ARR_PREFIX_SIZE_CALCULATE(max_field_size, field_size_in_table) \
   (max_field_size - field_size_in_table)


#define DBAL_FIELD_ARR_PREFIX_VALUE_CALCULATE(arr_prefix_table_entry_width, arr_prefix_table_entry_value, arr_prefix_size) \
    (arr_prefix_table_entry_value >> (arr_prefix_table_entry_width - arr_prefix_size))

extern const uint32 zero_buffer_to_compare[DBAL_MAX_SIZE_FOR_GLOBAL_ZERO_BUFFER];


uint8 dbal_is_initiated(
    int unit);

shr_error_e dbal_internal_init(
    int unit);


shr_error_e dbal_internal_deinit(
    int unit);


shr_error_e dbal_collection_init(
    int unit);


void dbal_status_set(
    int unit,
    dbal_status_e status);


shr_error_e dbal_status_get(
    int unit,
    dbal_status_e * status);

uint8 dbal_image_name_is_std_1(
    int unit);


void dbal_pemla_status_set(
    int unit,
    dbal_status_e status);


shr_error_e dbal_pemla_status_get(
    int unit,
    dbal_status_e * status);


void dbal_initiated_reset(
    int unit);


shr_error_e dbal_action_prints(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_action_flags_e flags,
    dbal_actions_e action);


shr_error_e dbal_action_skip_check(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_actions_e * action);


shr_error_e dbal_action_start(
    int unit,
    uint32 handle_id,
    dbal_entry_handle_t ** entry_handle,
    dbal_entry_action_flags_e flags,
    dbal_actions_e * action);


shr_error_e dbal_action_finish(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_action_flags_e flags,
    dbal_actions_e action);


shr_error_e dbal_entry_handle_take_internal(
    int unit,
    dbal_tables_e table_id,
    uint32 *entry_handle_id,
    uint8 operation);


shr_error_e dbal_entry_handle_copy_internal(
    int unit,
    uint32 src_handle_id,
    uint32 *dst_handle_id);


shr_error_e dbal_entry_handle_get_internal(
    int unit,
    uint32 entry_handle_id,
    dbal_entry_handle_t ** entry_handle);


void dbal_entry_handle_update_result(
    int unit,
    uint32 orig_entry_handle_id,
    uint32 new_entry_handle_id);


void dbal_entry_key_field_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val,
    uint32 *field_mask,
    dbal_pointer_type_e pointer_type);


void dbal_entry_key_field_range_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 min_val,
    uint32 max_val);


void dbal_entry_value_field_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int in_inst_id,
    dbal_pointer_type_e pointer_type,
    uint32 *field_val,
    uint32 *field_mask);


void dbal_entry_value_field_unset_field(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int in_inst_id);


void dbal_entry_value_field_request(
    int unit,
    dbal_pointer_type_e pointer_type,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int in_inst_id,
    void *field_val,
    void *field_mask);


shr_error_e dbal_entry_enum_hw_value_or_key_field_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_fields_e field_id,
    uint32 *field_val,
    uint32 *field_mask,
    dbal_pointer_type_e pointer_type);


shr_error_e dbal_entry_key_field_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_fields_e field_id,
    uint32 *field_val,
    uint32 *field_mask,
    dbal_pointer_type_e pointer_type);


shr_error_e dbal_entry_value_field_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_fields_e field_id,
    int in_inst_id,
    uint32 *field_val,
    uint32 *field_mask,
    dbal_pointer_type_e pointer_type);


shr_error_e dbal_entry_attribute_request_internal(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 attr_type,
    uint32 *attr_val);

shr_error_e dbal_iterator_key_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_condition_types_e key_condition,
    uint32 *field_value,
    uint32 *field_mask);


shr_error_e dbal_iterator_value_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_condition_types_e key_condition,
    uint32 *field_value,
    uint32 *field_mask);


shr_error_e dbal_iterator_common_validation(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *action_skip);


shr_error_e dbal_iterator_action_add(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_fields_e field_id,
    dbal_iterator_action_types_e action_type,
    uint32 *field_value,
    uint32 *field_mask);


shr_error_e dbal_iterator_increment_by_allocator(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_key_value_validity_e * is_valid_entry);


shr_error_e dbal_iterator_init_handle_info(
    int unit,
    dbal_entry_handle_t * entry_handle);


shr_error_e dbal_entry_print(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 format);


int dbal_logger_is_enable(
    int unit,
    dbal_tables_e table_id);


shr_error_e dbal_key_buffer_validate(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_key_value_validity_e * is_valid);


shr_error_e dbal_verify_result_type_is_from_table(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 *is_from_table);


shr_error_e dbal_entry_handle_phy_entry_align(
    int unit,
    uint32 payload_offset,
    uint8 shift_left,
    dbal_entry_handle_t * entry_handle);


shr_error_e dbal_entry_handle_is_field_set(
    int unit,
    int entry_handle_id,
    dbal_fields_e field_id,
    int in_inst_id);


shr_error_e dbal_entry_handle_value_fields_parse(
    int unit,
    dbal_entry_handle_t * entry_handle);


void dbal_entry_handle_attribute_parse(
    int unit,
    dbal_entry_handle_t * entry_handle);


dbal_status_e dbal_mngr_status_get(
    int unit);


shr_error_e dbal_field_from_buffer_get(
    int unit,
    dbal_table_field_info_t * table_field_info,
    dbal_fields_e field_id,
    uint32 *buffer,
    uint32 *returned_field_value);


shr_error_e dbal_field_physical_value_from_buffer_get(
    int unit,
    dbal_table_field_info_t * table_field_info,
    dbal_fields_e field_id,
    uint32 *buffer,
    uint32 *returned_field_value);


shr_error_e dbal_field_mask_from_buffer_get(
    int unit,
    dbal_table_field_info_t * table_field_info,
    dbal_fields_e field_id,
    uint32 *buffer,
    uint32 *returned_field_value);


shr_error_e dbal_field_in_table_printable_string_get(
    int unit,
    dbal_fields_e field_id,
    dbal_tables_e table_id,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    int result_type_idx,
    uint8 is_key,
    uint8 is_full_buffer,
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE]);

shr_error_e dbal_field_printable_string_get(
    int unit,
    dbal_fields_e field_id,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    multi_res_info_t * result_info,
    uint8 is_full_buffer,
    dbal_fields_e parent_field,
    int size,
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE]);

shr_error_e dbal_lpm_prefix_length_to_mask(
    int unit,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_lpm_mask_to_prefix_length(
    int unit,
    dbal_entry_handle_t * entry_handle);



shr_error_e dbal_field_types_init_illegal_value_cb_assign(
    int unit,
    dbal_field_types_defs_e field_type,
    dbal_field_types_basic_info_t * field_types_info);

shr_error_e dbal_fields_nof_occupied_dynamic_fields_get(
    int unit,
    int *nof_occupied_fields);

shr_error_e dbal_field_types_enum_name_to_hw_value_get(
    int unit,
    dbal_field_types_defs_e field_type,
    char enum_value_name[DBAL_MAX_STRING_LENGTH],
    uint32 *hw_value);


shr_error_e dbal_field_types_info_get(
    int unit,
    dbal_field_types_defs_e field_type,
    CONST dbal_field_types_basic_info_t ** field_type_info);

shr_error_e dbal_field_arr_prefix_decode(
    int unit,
    dbal_table_field_info_t * table_field,
    uint32 field_val,
    uint32 *ret_field_val,
    uint8 verbose);

shr_error_e dbal_fields_encode_decode_bitwise_not(
    int unit,
    uint32 *field_val,
    uint32 max_nof_bits,
    uint32 *field_val_out);

shr_error_e dbal_fields_field_types_info_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_types_basic_info_t ** field_type_info);

uint8 dbal_fields_is_field_encoded(
    int unit,
    dbal_fields_e field_id);


shr_error_e dbal_field_encode_handle_rv(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int is_key,
    dbal_fields_e field_id,
    shr_error_e ret_val);


shr_error_e dbal_fields_field_encode(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e parent_field_id,
    int field_size,
    uint32 *field_val,
    uint32 *field_value_out);


shr_error_e dbal_fields_field_parent_encode_custom_val(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e parent_field_id,
    uint32 encode_mask_val,
    uint32 *field_mask_val,
    uint32 *parent_field_full_mask);

shr_error_e dbal_fields_encode_valid_ind(
    int unit,
    uint32 *field_val,
    uint32 max_nof_bits,
    uint32 *field_val_out);

shr_error_e dbal_fields_decode_valid_ind(
    int unit,
    uint32 *field_val,
    uint32 curr_nof_bits,
    uint32 *field_val_out);

shr_error_e dbal_fields_field_decode(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e parent_field_id,
    int field_size,
    uint32 *field_val,
    uint32 *field_value_out);

shr_error_e dbal_fields_is_basic_type(
    int unit,
    dbal_fields_e field_id,
    uint8 *is_basic_type);

shr_error_e dbal_fields_of_same_instance(
    int unit,
    dbal_fields_e field_id_1,
    dbal_fields_e field_id_2,
    uint8 *is_same_instance);


int dbal_fields_is_parent_field(
    int unit,
    dbal_fields_e field_id);

shr_error_e dbal_fields_sub_field_info_get_internal(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e parent_field_id,
    dbal_sub_field_info_t * sub_field_info);

shr_error_e dbal_fields_encode_prefix(
    int unit,
    uint32 *field_val,
    uint32 prefix_val,
    uint32 value_length,
    uint32 total_field_len,
    uint32 *field_val_out);

shr_error_e dbal_fields_decode_prefix(
    int unit,
    uint32 *field_val,
    uint32 value_length,
    uint32 *field_val_out);

shr_error_e dbal_fields_encode_suffix(
    int unit,
    uint32 *field_val,
    uint32 suffix_val,
    uint32 field_length,
    uint32 total_field_len,
    uint32 *field_val_out);


shr_error_e dbal_fields_sub_field_match(
    int unit,
    dbal_fields_e parent_field_id,
    dbal_fields_e sub_field_id,
    uint8 *is_found);

shr_error_e dbal_fields_max_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 *max_value);

shr_error_e dbal_fields_min_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 *min_value);

shr_error_e dbal_fields_const_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 *const_value,
    uint8 *const_value_valid);

shr_error_e dbal_fields_has_illegal_values(
    int unit,
    dbal_fields_e field_id,
    uint8 *has_illegals);

shr_error_e dbal_fields_print_type_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_print_type_e * field_print_type);

shr_error_e dbal_fields_encode_type_get(
    int unit,
    dbal_fields_e field_id,
    dbal_value_field_encode_types_e * encode_type);

shr_error_e dbal_fields_transform_arr32_to_arr8(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int res_type_index,
    int inst_id,
    uint32 *field_val_u32,
    uint8 *field_val,
    uint32 *field_mask_u32,
    uint8 *field_mask);

shr_error_e dbal_fields_transform_arr8_to_arr32(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int res_type_index,
    int inst_id,
    uint8 *field_val,
    uint32 *field_val_u32,
    uint8 *field_mask,
    uint32 *field_mask_u32);

uint32 dbal_fields_instance_idx_get(
    int unit,
    dbal_fields_e field_id);

shr_error_e dbal_field_string_update(
    int unit,
    dbal_fields_e field_id,
    char field_name[DBAL_MAX_STRING_LENGTH]);

shr_error_e dbal_field_string_to_indexes_init(
    int unit);


shr_error_e dbal_tables_table_get_internal(
    int unit,
    dbal_tables_e table_id,
    dbal_logical_table_t ** table);


shr_error_e dbal_tables_field_index_in_table_get_no_err(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    int *field_index_in_table);

shr_error_e dbal_tables_table_create_dynamic(
    int unit,
    char *parent_dir,
    dbal_logical_table_t * tables_info);

shr_error_e dbal_fields_nof_occupied_dynamic_tables_get(
    int unit,
    int *nof_occupied_tables);


shr_error_e dbal_tables_table_get(
    int unit,
    dbal_tables_e table_id,
    CONST dbal_logical_table_t ** table);


shr_error_e dbal_tables_entry_counter_update(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries_to_update,
    uint8 is_add_operation);

shr_error_e dbal_tables_max_key_value_get(
    int unit,
    dbal_tables_e table_id,
    uint32 *key_buffer);

shr_error_e dbal_tables_field_info_get_internal(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    dbal_table_field_info_t ** field_info,
    int *field_index_in_table,
    dbal_fields_e * parent_field_id);

shr_error_e dbal_tables_update_hw_error(
    int unit,
    dbal_tables_e table_id);

shr_error_e dbal_tables_nof_hw_error_tables_get(
    int unit,
    int *not_tables);

shr_error_e dbal_tables_porting_info_get(
    int unit,
    hl_porting_info_t ** hl_porting_info);

shr_error_e dbal_tables_groups_info_get(
    int unit,
    hl_groups_info_t ** hl_groups_info);

shr_error_e dbal_tables_group_info_get(
    int unit,
    dbal_hw_entity_group_e hw_entity_group_id,
    hl_group_info_t ** group_info);


uint8 dbal_tables_is_merge_entries_supported(
    int unit,
    dbal_tables_e table_id);


uint8 dbal_tables_is_non_direct(
    int unit,
    dbal_tables_e table_id);

 uint8
    dbal_tables_is_tcam(
    int unit,
    dbal_tables_e table_id);


shr_error_e dbal_tables_sizes_get(
    int unit,
    dbal_tables_e table_id,
    int *key_size,
    int *max_pld_size);


shr_error_e dbal_tables_optimize_commit_mode_set(
    int unit,
    dbal_tables_e table_id,
    uint32 mode);


shr_error_e dbal_tables_dirty_mode_set(
    int unit,
    dbal_tables_e table_id,
    uint32 mode);


shr_error_e dbal_tables_protection_enable_set(
    int unit,
    dbal_tables_e table_id,
    uint32 mode);


shr_error_e dbal_field_types_alloc(
    int unit);


shr_error_e dbal_field_types_dealloc(
    int unit);


shr_error_e dbal_per_table_mutex_reserve(
    int unit,
    dbal_tables_e table_id,
    uint8 *mutex_id);


shr_error_e dbal_per_table_mutex_release(
    int unit,
    dbal_tables_e table_id,
    uint8 mutex_id);

shr_error_e dbal_per_table_mutex_info_get(
    int unit,
    uint8 mutex_id,
    CONST dbal_specific_table_protection_info_t ** protect_info);

shr_error_e dbal_tables_learning_enable(
    int unit,
    dbal_tables_e table_id,
    uint32 hitbit);


shr_error_e dbal_tables_hitbit_enable(
    int unit,
    dbal_tables_e table_id,
    uint32 hitbit);


shr_error_e dbal_tables_validation_set(
    int unit,
    dbal_tables_e table_id,
    uint32 mode);


shr_error_e dbal_tables_collection_mode_set(
    int unit,
    dbal_tables_e table_id,
    uint32 mode);


shr_error_e dbal_tables_result_type_restore(
    int unit,
    dbal_tables_e table_id);

shr_error_e dbal_mdb_table_entry_add(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_mdb_table_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_mdb_table_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_mdb_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_mdb_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_mdb_table_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_mdb_table_iterator_deinit(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_mdb_table_access_id_by_key_get(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_kbp_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 payload_size);
shr_error_e dbal_mdb_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 resolve_from_sw);
uint8 dbal_table_is_in_lif(
    CONST dbal_logical_table_t * table);
uint8 dbal_table_is_out_lif(
    CONST dbal_logical_table_t * table);
uint8 dbal_logical_table_is_mact(
    int unit,
    dbal_tables_e table_id);
shr_error_e dbal_physical_table_app_id_is_mact(
    int unit,
    dbal_physical_tables_e dbal_physical_table,
    uint32 app_id,
    uint32 *is_mact);
uint8 dbal_logical_table_is_out_rif_allocator(
    CONST dbal_logical_table_t * table);
uint8 dbal_logical_table_is_fec(
    int unit,
    CONST dbal_logical_table_t * table);

shr_error_e dbal_hl_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_hl_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_hl_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_hl_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_hl_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_hl_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_hl_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_hl_range_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries);

shr_error_e dbal_hl_range_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries);


shr_error_e dbal_hl_entry_default_entry_build(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * default_handle);

shr_error_e dbal_key_or_value_condition_check(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 is_key,
    dbal_access_condition_info_t * access_condition,
    int nof_conditions,
    uint8 *is_passed);

shr_error_e dbal_hl_offset_calculate(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_fields_e mapped_field_id,
    uint32 default_val,
    dbal_offset_encode_info_t * encode_info,
    uint32 *calc_index);

shr_error_e dbal_hl_register_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int result_type_get);

shr_error_e dbal_hl_memory_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int result_type_get);

shr_error_e dbal_hl_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_sw_table_nof_entries_calculate(
    int unit,
    dbal_logical_table_t * table,
    uint32 *num_of_entries);

shr_error_e dbal_sw_table_hl_iterator_init(
    int unit,
    dbal_tables_e table_id,
    uint32 key_size,
    dbal_sw_table_iterator_t * sw_iterator);
shr_error_e dbal_hl_reg_default_mode_get(
    int unit,
    soc_reg_t reg,
    int *is_standard);
shr_error_e dbal_hl_reg_default_get(
    int unit,
    soc_reg_t reg,
    int copyno,
    int index,
    uint32 *entry);
shr_error_e dbal_hl_is_entry_default(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *is_default);


shr_error_e dbal_sw_table_direct_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_direct_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int res_type_get,
    int *resolved_res_type);
shr_error_e dbal_sw_table_direct_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_direct_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_hash_entry_add(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_hash_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *resolved_res_type);
shr_error_e dbal_sw_table_hash_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_hash_iterator_init(
    int unit,
    dbal_tables_e table_id,
    dbal_sw_table_iterator_t * sw_iterator);
shr_error_e dbal_sw_table_hash_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_table_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_sw_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_tcam_cs_entry_add(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_tcam_cs_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_tcam_cs_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_tcam_cs_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_tcam_cs_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_tcam_cs_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_tcam_cs_iterator_deinit(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_tcam_cs_stage_info_get(
    int unit,
    dbal_stage_e dbal_stage,
    dbal_tcam_cs_stage_info_t * stage_info);

shr_error_e dbal_pemla_table_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_pemla_table_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_pemla_table_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_pemla_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_pemla_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_pemla_table_iterator_deinit(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_pemla_table_iterator_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);


shr_error_e dbal_kbp_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_kbp_entry_add(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_kbp_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_kbp_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_kbp_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_kbp_table_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_kbp_table_iterator_deinit(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_kbp_access_id_by_key_get(
    int unit,
    dbal_entry_handle_t * entry_handle);
shr_error_e dbal_kbp_access_cache_commit(
    int unit,
    int is_acl);


shr_error_e dbal_logger_table_user_lock(
    int unit,
    dbal_tables_e table_id);

shr_error_e dbal_logger_table_mode_set_internal(
    int unit,
    dbal_logger_mode_e mode);


shr_error_e dbal_logger_table_internal_lock(
    int unit,
    dbal_tables_e table_id);


void dbal_logger_internal_disable_set(
    int unit);
void dbal_logger_internal_disable_clear(
    int unit);


shr_error_e dbal_logger_table_internal_unlock(
    int unit,
    dbal_tables_e table_id);


void dbal_logger_info_get_internal(
    int unit,
    CONST dbal_logger_info_t ** logger_info);

shr_error_e dbal_mutexes_init(
    int unit);

shr_error_e dbal_logger_init(
    int unit);


shr_error_e dbal_logger_deinit(
    int unit);


shr_error_e dbal_handles_init(
    int unit);


shr_error_e dbal_handles_deinit(
    int unit);


shr_error_e dbal_logger_file_open(
    int unit,
    char *file_name);


shr_error_e dbal_logger_file_get(
    int unit,
    FILE ** dbal_file);

shr_error_e dbal_logger_file_close(
    int unit);


shr_error_e dbal_logger_user_locked_table_get(
    int unit,
    dbal_tables_e * table_id);


shr_error_e dbal_entry_default_values_add(
    int unit,
    dbal_entry_handle_t * entry_handle);



shr_error_e dbal_actions_access_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_actions_access_iterator_destroy(
    int unit,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_actions_access_iterator_get_next(
    int unit,
    uint32 entry_handle_id,
    int *is_end,
    uint8 *continue_iterating);

shr_error_e dbal_actions_access_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);


shr_error_e dbal_action_access_pre_access(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_action_access_type_e action_access_type,
    dbal_action_access_func_e action_access_func);


shr_error_e dbal_action_access_error_recovery_invalidate(
    int unit,
    dbal_action_access_func_e action_access_func);


shr_error_e dbal_action_access_post_access(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * get_entry_handle,
    dbal_action_access_type_e action_access_type,
    dbal_action_access_func_e action_access_func,
    int rv);


shr_error_e dbal_entry_commit_normal(
    int unit,
    dbal_entry_handle_t * entry_handle);


shr_error_e dbal_entry_commit_update(
    int unit,
    dbal_entry_handle_t * entry_handle);


shr_error_e dbal_entry_commit_force(
    int unit,
    dbal_entry_handle_t * entry_handle);


shr_error_e dbal_entry_range_action_process(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_actions_e action);


shr_error_e dbal_actions_access_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle);


shr_error_e dbal_actions_access_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * get_entry_handle,
    int is_sw_get);


shr_error_e dbal_actions_access_entry_commit(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * get_handle);


shr_error_e dbal_actions_access_access_id_by_key_get(
    int unit,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_actions_access_result_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle);


shr_error_e dbal_db_init_table_mutual_interface_validation(
    int unit,
    dbal_tables_e table_id,
    dbal_logical_table_t * table);


shr_error_e dbal_db_init_table_mdb_validation(
    int unit,
    dbal_tables_e table_id,
    dbal_logical_table_t * table);


shr_error_e dbal_entry_direct_is_default(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *is_default);


shr_error_e dbal_flush_mode_collect(
    int unit);


shr_error_e dbal_flush_mode_get(
    int unit,
    int *collect_mode);


shr_error_e dbal_iterator_info_get(
    int unit,
    int entry_handle_id,
    dbal_iterator_info_t ** iterator_info);


shr_error_e dbal_flush_mode_normal(
    int unit);


shr_error_e dbal_merge_entries(
    int unit,
    dbal_entry_handle_t * prim_entry,
    dbal_entry_handle_t * secondary_entry);


typedef shr_error_e(
    *dbal_dynamic_xml_add_f) (
    int unit,
    xml_node top_node);


shr_error_e dnx_dynamic_xml_tables_update(
    int unit,
    xml_node top_node);


shr_error_e dbal_dynamic_updates_dispatcher(
    int unit,
    char *parent_dir,
    dbal_dynamic_xml_add_f dbal_dynamic_xml_add,
    char *top_node_name);



shr_error_e dbal_pemla_init(
    int unit);

shr_error_e dbal_pemla_deinit(
    int unit);

shr_error_e dbal_sw_access_init(
    int unit);

shr_error_e dbal_sw_access_deinit(
    int unit);

shr_error_e dbal_mdb_init(
    int unit);

shr_error_e dbal_mdb_deinit(
    int unit);


shr_error_e dbal_value_field32_validate(
    int unit,
    uint32 *field_val,
    dbal_table_field_info_t * table_field_info,
    uint8 *is_valid_value);


shr_error_e dbal_actions_optimized_is_exists(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *is_entry_exists);


shr_error_e dbal_collection_worker_thread_start(
    int unit,
    int pool_id);

shr_error_e dbal_collection_trigger_set_internal(
    int unit,
    dbal_collection_trigger_type_e trigger_type);
#endif
