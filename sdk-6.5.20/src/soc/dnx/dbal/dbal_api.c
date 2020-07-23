/**
 * \file dbal_api.c
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * System wide Logical Table Manager.
 *
 * To be used for:
 *   Access of physical table
 *   Access of pure software tables
 *   Activation of access procedutes (dispatcher) which is
 *     equivalent to 'MBCM and arad_pp_dbal' on SDK6 for JR1.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

#include "dbal_internal.h"

#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>

extern uint32 G_dbal_field_full_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];


#define DBAL_ENTRY_VALUE_FIELD_SET_INST(unit, entry_handle_id, field_id, inst_id, pointer_type)                 \
do                                                                                                              \
{                                                                                                               \
    if (inst_id == INST_ALL)                                                                                    \
    {                                                                                                           \
        dbal_entry_value_field_set(unit, entry_handle_id, field_id, INST_ALL, pointer_type, field_val_u32, field_mask_u32);\
    }                                                                                                           \
    else                                                                                                        \
    {                                                                                                           \
        dbal_entry_value_field_set(unit, entry_handle_id, field_id, inst_id, pointer_type, field_val_u32, field_mask_u32);\
    }                                                                                                           \
}                                                                                                               \
while (0)



shr_error_e
dbal_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_INIT);

    if (!dbal_is_initiated(unit))
    {
        SHR_IF_ERR_EXIT(dbal_field_types_alloc(unit));
        SHR_IF_ERR_EXIT(dbal_internal_init(unit));
        dbal_status_set(unit, DBAL_STATUS_DBAL_INIT_DONE);
        if (!sw_state_is_warm_boot(unit))
        {
            SHR_IF_ERR_EXIT(dbal_db.init(unit));
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DBAL init is called but DBAL is already initiated\n");
    }

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_INIT);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dbal_is_initiated(unit))
    {
        dbal_status_set(unit, DBAL_STATUS_NOT_INITIALIZED);
        SHR_IF_ERR_EXIT(dbal_internal_deinit(unit));
        SHR_IF_ERR_EXIT(dbal_field_types_dealloc(unit));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DBAL deinit is called but DBAL is not initiated\n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_device_init_done(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    dbal_status_set(unit, DBAL_STATUS_DEVICE_INIT_DONE);

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_access_method_init(
    int unit,
    dbal_access_method_e access_method)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            SHR_IF_ERR_EXIT(dbal_mdb_init(unit));
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            SHR_IF_ERR_EXIT(dbal_sw_access_init(unit));
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            SHR_IF_ERR_EXIT(dbal_pemla_init(unit));
            break;

        case DBAL_ACCESS_METHOD_KBP:
        case DBAL_ACCESS_METHOD_TCAM_CS:
        case DBAL_ACCESS_METHOD_HARD_LOGIC:
        
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "ilegal access method %d \n", access_method);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_access_method_deinit(
    int unit,
    dbal_access_method_e access_method)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            SHR_IF_ERR_EXIT(dbal_mdb_deinit(unit));
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            SHR_IF_ERR_EXIT(dbal_sw_access_deinit(unit));
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            SHR_IF_ERR_EXIT(dbal_pemla_deinit(unit));
            break;

        case DBAL_ACCESS_METHOD_KBP:
        case DBAL_ACCESS_METHOD_TCAM_CS:
        case DBAL_ACCESS_METHOD_HARD_LOGIC:
        
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "ilegal access method %d \n", access_method);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_logger_table_lock(
    int unit,
    dbal_tables_e table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_logger_table_user_lock(unit, table_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_logger_table_mode_set(
    int unit,
    dbal_logger_mode_e mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_logger_table_mode_set_internal(unit, mode));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_entry_handle_take_macro(
    int unit,
    dbal_tables_e table_id,
    uint32 *entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_entry_handle_take_internal(unit, table_id, entry_handle_id, DBAL_HANDLE_TAKE_ALLOC));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_clear_macro(
    int unit,
    dbal_tables_e table_id,
    uint32 entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_entry_handle_take_internal(unit, table_id, &entry_handle_id, DBAL_HANDLE_TAKE_CLEAR));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_copy_macro(
    int unit,
    uint32 src_handle_id,
    uint32 *dst_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_entry_handle_copy_internal(unit, src_handle_id, dst_handle_id));
exit:
    SHR_FUNC_EXIT;
}



void
dbal_entry_key_field_predefine_value_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_field_predefine_value_type_e value_type)
{
    uint32 field_val_u32[1] = { 0 };
    int rc;
    uint32 *field_mask_u32 = G_dbal_field_full_mask;
    dbal_entry_handle_t *entry_handle;

    rc = dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle);
    DBAL_FIELD_ERR_HANDLE((rc), "handle not taken");
    rc = dbal_fields_predefine_value_get(unit, entry_handle_id, field_id, 1, value_type, field_val_u32);
    DBAL_FIELD_ERR_HANDLE((rc), "predefine value get failed");

    dbal_entry_key_field_set(unit, entry_handle_id, field_id, field_val_u32, field_mask_u32,
                             DBAL_POINTER_TYPE_ARR_UINT32);
}

void
dbal_entry_value_field_predefine_value_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    dbal_field_predefine_value_type_e value_type)
{
    uint32 field_val_u32[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 *field_mask_u32 = G_dbal_field_full_mask;
    int rc;
    int internal_inst_id = inst_id;
    dbal_entry_handle_t *entry_handle;

    if (inst_id == INST_ALL)
    {
        
        internal_inst_id = 0;
    }
    if (inst_id == INST_SINGLE)
    {
        internal_inst_id = 0;
    }

    rc = dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle);
    DBAL_FIELD_ERR_HANDLE((rc), "handle not taken");

    rc = dbal_fields_predefine_value_get(unit, entry_handle_id, field_id + internal_inst_id, 0, value_type,
                                         field_val_u32);
    DBAL_FIELD_ERR_HANDLE((rc), "predefine value get failed");

    DBAL_ENTRY_VALUE_FIELD_SET_INST(unit, entry_handle_id, field_id, inst_id, DBAL_POINTER_TYPE_ARR_UINT32);
}

void
dbal_entry_key_field8_range_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 min_val,
    uint8 max_val)
{
    uint32 min_val_as32, max_val_as32;

    min_val_as32 = min_val;
    if (min_val == (DBAL_RANGE_ALL & 0xff))
    {
        min_val_as32 = DBAL_RANGE_ALL;
    }

    max_val_as32 = max_val;
    if (max_val == (DBAL_RANGE_ALL & 0xff))
    {
        max_val_as32 = DBAL_RANGE_ALL;
    }
    dbal_entry_key_field_range_set(unit, entry_handle_id, field_id, min_val_as32, max_val_as32);
}

void
dbal_entry_key_field16_range_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint16 min_val,
    uint16 max_val)
{
    uint32 min_val_as32, max_val_as32;

    min_val_as32 = min_val;
    if (min_val == (DBAL_RANGE_ALL & 0xffff))
    {
        min_val_as32 = DBAL_RANGE_ALL;
    }

    max_val_as32 = max_val;
    if (max_val == (DBAL_RANGE_ALL & 0xffff))
    {
        max_val_as32 = DBAL_RANGE_ALL;
    }
    dbal_entry_key_field_range_set(unit, entry_handle_id, field_id, min_val_as32, max_val_as32);
}

void
dbal_entry_key_field32_range_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 min_val,
    uint32 max_val)
{
    dbal_entry_key_field_range_set(unit, entry_handle_id, field_id, min_val, max_val);
}

void
dbal_entry_key_field8_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 field_val)
{
    uint32 field_val_u32[1];
    uint32 *field_mask_u32;

    field_val_u32[0] = field_val;
    field_mask_u32 = G_dbal_field_full_mask;

    dbal_entry_key_field_set(unit, entry_handle_id, field_id, field_val_u32, field_mask_u32, DBAL_POINTER_TYPE_UINT8);
}

void
dbal_entry_key_field16_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint16 field_val)
{
    uint32 field_val_u32[1];
    uint32 *field_mask_u32;

    field_val_u32[0] = field_val;
    field_mask_u32 = G_dbal_field_full_mask;

    dbal_entry_key_field_set(unit, entry_handle_id, field_id, field_val_u32, field_mask_u32, DBAL_POINTER_TYPE_UINT16);
}

void
dbal_entry_key_field32_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 field_val)
{
    uint32 field_val_u32[1];
    uint32 *field_mask_u32;

    field_val_u32[0] = field_val;
    field_mask_u32 = G_dbal_field_full_mask;
    dbal_entry_key_field_set(unit, entry_handle_id, field_id, field_val_u32, field_mask_u32, DBAL_POINTER_TYPE_UINT32);
}

void
dbal_entry_key_field_arr8_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val)
{
    int rv;
    dbal_entry_handle_t *entry_handle;
    uint32 field_val_u32[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 *field_mask_u32;

    rv = dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle);
    DBAL_FIELD_ERR_HANDLE((rv), "handle not taken");

    rv = dbal_fields_transform_arr8_to_arr32(unit, entry_handle->table_id, field_id, TRUE, 0, 0, field_val,
                                             field_val_u32, NULL, NULL);
    DBAL_FIELD_ERR_HANDLE((rv), "Error transform arr8 to arr32");

    field_mask_u32 = G_dbal_field_full_mask;
    dbal_entry_key_field_set(unit, entry_handle_id, field_id, field_val_u32, field_mask_u32,
                             DBAL_POINTER_TYPE_ARR_UINT8);
}

void
dbal_entry_key_field_arr32_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val)
{
    uint32 *field_val_u32;
    uint32 *field_mask_u32;

    field_val_u32 = field_val;
    field_mask_u32 = G_dbal_field_full_mask;

    dbal_entry_key_field_set(unit, entry_handle_id, field_id, field_val_u32, field_mask_u32,
                             DBAL_POINTER_TYPE_ARR_UINT32);
}

void
dbal_entry_key_field64_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint64 field_val)
{
    uint32 field_val_u32[2];
    uint32 *field_mask_u32;

    field_val_u32[0] = COMPILER_64_LO(field_val);
    field_val_u32[1] = COMPILER_64_HI(field_val);

    field_mask_u32 = G_dbal_field_full_mask;

    dbal_entry_key_field_set(unit, entry_handle_id, field_id, field_val_u32, field_mask_u32, DBAL_POINTER_TYPE_UINT64);
}



void
dbal_entry_key_field8_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 field_val,
    uint8 field_mask)
{
    dbal_entry_handle_t *entry_handle;
    uint32 field_val_u32[1], field_mask_u32[1];
    int rv;

    rv = dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle);
    DBAL_FIELD_ERR_HANDLE((rv), "handle not taken");

    DBAL_MASK_ENTRY_VALID_TABLE_CHECK(entry_handle->table->table_type, entry_handle->table->access_method);

    field_val_u32[0] = field_val;
    field_mask_u32[0] = field_mask;

    dbal_entry_key_field_set(unit, entry_handle_id, field_id, field_val_u32, field_mask_u32, DBAL_POINTER_TYPE_UINT8);
}

void
dbal_entry_key_field16_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint16 field_val,
    uint16 field_mask)
{
    dbal_entry_handle_t *entry_handle;
    uint32 field_val_u32[1], field_mask_u32[1];
    int rv;

    rv = dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle);
    DBAL_FIELD_ERR_HANDLE((rv), "handle not taken");

    DBAL_MASK_ENTRY_VALID_TABLE_CHECK(entry_handle->table->table_type, entry_handle->table->access_method);

    field_val_u32[0] = field_val;
    field_mask_u32[0] = field_mask;

    dbal_entry_key_field_set(unit, entry_handle_id, field_id, field_val_u32, field_mask_u32, DBAL_POINTER_TYPE_UINT16);
}

void
dbal_entry_key_field32_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 field_val,
    uint32 field_mask)
{
    dbal_entry_handle_t *entry_handle;
    uint32 field_val_u32[1], field_mask_u32[1];
    int rv;

    rv = dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle);
    DBAL_FIELD_ERR_HANDLE((rv), "handle not taken");

    DBAL_MASK_ENTRY_VALID_TABLE_CHECK(entry_handle->table->table_type, entry_handle->table->access_method);

    field_val_u32[0] = field_val;
    field_mask_u32[0] = field_mask;

    dbal_entry_key_field_set(unit, entry_handle_id, field_id, field_val_u32, field_mask_u32, DBAL_POINTER_TYPE_UINT32);
}

void
dbal_entry_key_field_arr8_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val,
    uint8 *field_mask)
{
    int rv;
    dbal_entry_handle_t *entry_handle;
    uint32 field_val_u32[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 field_mask_u32[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    rv = dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle);
    DBAL_FIELD_ERR_HANDLE((rv), "handle not taken");

    DBAL_MASK_ENTRY_VALID_TABLE_CHECK(entry_handle->table->table_type, entry_handle->table->access_method);

    rv = dbal_fields_transform_arr8_to_arr32(unit, entry_handle->table_id, field_id, TRUE, 0, 0, field_val,
                                             field_val_u32, field_mask, field_mask_u32);
    DBAL_FIELD_ERR_HANDLE((rv), "Error transform arr8 to arr32");

    dbal_entry_key_field_set(unit, entry_handle_id, field_id, field_val_u32, field_mask_u32,
                             DBAL_POINTER_TYPE_ARR_UINT8);
}

void
dbal_entry_key_field_arr32_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val,
    uint32 *field_mask)
{
    dbal_entry_handle_t *entry_handle;
    uint32 *field_val_u32;
    uint32 *field_mask_u32;
    int rv;

    rv = dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle);
    DBAL_FIELD_ERR_HANDLE((rv), "handle not taken");

    DBAL_MASK_ENTRY_VALID_TABLE_CHECK(entry_handle->table->table_type, entry_handle->table->access_method);

    field_val_u32 = field_val;
    field_mask_u32 = field_mask;

    dbal_entry_key_field_set(unit, entry_handle_id, field_id, field_val_u32, field_mask_u32,
                             DBAL_POINTER_TYPE_ARR_UINT32);
}

void
dbal_entry_key_field64_masked_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint64 field_val,
    uint64 field_mask)
{
    dbal_entry_handle_t *entry_handle;
    uint32 field_val_u32[2];
    uint32 field_mask_u32[2];
    int rv;

    rv = dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle);
    DBAL_FIELD_ERR_HANDLE((rv), "handle not taken");

    DBAL_MASK_ENTRY_VALID_TABLE_CHECK(entry_handle->table->table_type, entry_handle->table->access_method);

    field_val_u32[0] = COMPILER_64_LO(field_val);
    field_val_u32[1] = COMPILER_64_HI(field_val);
    field_mask_u32[0] = COMPILER_64_LO(field_mask);
    field_mask_u32[1] = COMPILER_64_HI(field_mask);

    dbal_entry_key_field_set(unit, entry_handle_id, field_id, field_val_u32, field_mask_u32, DBAL_POINTER_TYPE_UINT64);
}

shr_error_e
dbal_entry_handle_key_field_range_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *min_val,
    uint32 *max_val)
{
    uint32 nof_entries = 0;
    dbal_entry_handle_t *entry_handle;
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    if (entry_handle->table->access_method != DBAL_ACCESS_METHOD_KBP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "supported only for KBP tables");
    }

    if (entry_handle->handle_status != DBAL_HANDLE_STATUS_ACTION_PREFORMED)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "dbal_field_get() not preformed on the handle - buffer empty\n");
    }

    for (ii = 0; ii < entry_handle->table->nof_key_fields; ii++)
    {
        if (field_id == entry_handle->table->keys_info[ii].field_id)
        {
            if (SHR_BITGET(entry_handle->table->keys_info[ii].field_indication_bm, DBAL_FIELD_IND_IS_RANGED))
            {
                nof_entries = entry_handle->key_field_ranges[ii];
                break;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Requested field %s doesn't have range",
                             dbal_field_to_string(unit, field_id));
            }
        }
    }

    if (ii == entry_handle->table->nof_key_fields)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "field %s not found in table ", dbal_field_to_string(unit, field_id));
    }

    SHR_IF_ERR_EXIT(dbal_entry_key_field_get(unit, entry_handle, field_id, min_val, NULL, DBAL_POINTER_TYPE_UINT32));

    *max_val = *min_val;
    if (nof_entries)
    {
        *max_val += nof_entries - 1;
    }

exit:
    SHR_FUNC_EXIT;
}



void
dbal_entry_value_field8_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 field_val)
{
    uint32 field_val_u32[1] = { 0 };
    uint32 *field_mask_u32 = G_dbal_field_full_mask;

    field_val_u32[0] = field_val;
    DBAL_ENTRY_VALUE_FIELD_SET_INST(unit, entry_handle_id, field_id, inst_id, DBAL_POINTER_TYPE_UINT8);
}

void
dbal_entry_value_field16_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint16 field_val)
{
    uint32 field_val_u32[1] = { 0 };
    uint32 *field_mask_u32 = G_dbal_field_full_mask;

    field_val_u32[0] = field_val;
    DBAL_ENTRY_VALUE_FIELD_SET_INST(unit, entry_handle_id, field_id, inst_id, DBAL_POINTER_TYPE_UINT16);
}

void
dbal_entry_value_field32_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 field_val)
{
    uint32 field_val_u32[1] = { 0 };
    uint32 *field_mask_u32 = G_dbal_field_full_mask;

    field_val_u32[0] = field_val;
    DBAL_ENTRY_VALUE_FIELD_SET_INST(unit, entry_handle_id, field_id, inst_id, DBAL_POINTER_TYPE_UINT32);
}

void
dbal_entry_value_field_arr8_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 *field_val)
{
    int rv;
    dbal_entry_handle_t *entry_handle;
    uint32 field_val_u32[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 *field_mask_u32 = G_dbal_field_full_mask;

    rv = dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle);
    DBAL_FIELD_ERR_HANDLE((rv), "handle not taken");

    rv = dbal_fields_transform_arr8_to_arr32(unit, entry_handle->table_id, field_id, FALSE, entry_handle->cur_res_type,
                                             inst_id, field_val, field_val_u32, NULL, NULL);
    DBAL_FIELD_ERR_HANDLE((rv), "Error transform arr8 to arr32");

    DBAL_ENTRY_VALUE_FIELD_SET_INST(unit, entry_handle_id, field_id, inst_id, DBAL_POINTER_TYPE_ARR_UINT8);
}

void
dbal_entry_value_field_arr32_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val)
{
    uint32 *field_val_u32;
    uint32 *field_mask_u32 = G_dbal_field_full_mask;

    field_val_u32 = field_val;
    DBAL_ENTRY_VALUE_FIELD_SET_INST(unit, entry_handle_id, field_id, inst_id, DBAL_POINTER_TYPE_ARR_UINT32);
}

void
dbal_entry_value_field64_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint64 field_val)
{
    uint32 field_val_u32[2];
    uint32 *field_mask_u32 = G_dbal_field_full_mask;

    field_val_u32[0] = COMPILER_64_LO(field_val);
    field_val_u32[1] = COMPILER_64_HI(field_val);

    DBAL_ENTRY_VALUE_FIELD_SET_INST(unit, entry_handle_id, field_id, inst_id, DBAL_POINTER_TYPE_UINT64);
}

void
dbal_entry_value_field_unset(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id)
{
    dbal_entry_value_field_unset_field(unit, entry_handle_id, field_id, inst_id);
}

void
dbal_value_field8_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 *field_val)
{
    DNX_INIT_TIME_ANALYZER_DBAL_START(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_REQUEST);
    dbal_entry_value_field_request(unit, DBAL_POINTER_TYPE_UINT8, entry_handle_id, field_id, inst_id, field_val, NULL);
    DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_REQUEST);
}

void
dbal_value_field16_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint16 *field_val)
{
    DNX_INIT_TIME_ANALYZER_DBAL_START(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_REQUEST);
    dbal_entry_value_field_request(unit, DBAL_POINTER_TYPE_UINT16, entry_handle_id, field_id, inst_id, field_val, NULL);
    DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_REQUEST);
}

void
dbal_value_field32_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val)
{
    DNX_INIT_TIME_ANALYZER_DBAL_START(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_REQUEST);
    dbal_entry_value_field_request(unit, DBAL_POINTER_TYPE_UINT32, entry_handle_id, field_id, inst_id, field_val, NULL);
    DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_REQUEST);
}

void
dbal_value_field_arr8_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 *field_val)
{
    DNX_INIT_TIME_ANALYZER_DBAL_START(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_REQUEST);
    dbal_entry_value_field_request(unit, DBAL_POINTER_TYPE_ARR_UINT8, entry_handle_id, field_id, inst_id, field_val,
                                   NULL);
    DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_REQUEST);
}

void
dbal_value_field_arr32_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val)
{
    DNX_INIT_TIME_ANALYZER_DBAL_START(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_REQUEST);
    dbal_entry_value_field_request(unit, DBAL_POINTER_TYPE_ARR_UINT32, entry_handle_id, field_id, inst_id, field_val,
                                   NULL);
    DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_REQUEST);
}

void
dbal_value_field64_request(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint64 *field_val)
{
    DNX_INIT_TIME_ANALYZER_DBAL_START(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_REQUEST);
    dbal_entry_value_field_request(unit, DBAL_POINTER_TYPE_UINT64, entry_handle_id, field_id, inst_id, field_val, NULL);
    DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_REQUEST);
}

shr_error_e
dbal_entry_attribute_set(
    int unit,
    uint32 entry_handle_id,
    uint32 attr_type,
    uint32 attr_val)
{
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    if (attr_type == DBAL_ENTRY_ATTR_PRIORITY)
    {
        if (DBAL_TABLE_IS_TCAM(entry_handle->table) && (entry_handle->table->access_method == DBAL_ACCESS_METHOD_KBP))
        {
            entry_handle->phy_entry.prefix_length = attr_val;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "priority set supported only for KBP TCAM table\n");
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal attribute set type %x", attr_type);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_attribute_get(
    int unit,
    uint32 entry_handle_id,
    uint32 attr_type,
    uint32 *attr_val)
{
    dbal_entry_handle_t *entry_handle;
    uint32 age_state_not_updated = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    if (attr_type == DBAL_ENTRY_ATTR_PRIORITY)
    {
        if (DBAL_TABLE_IS_TCAM(entry_handle->table) && (entry_handle->table->access_method == DBAL_ACCESS_METHOD_KBP))
        {
            *attr_val = entry_handle->phy_entry.prefix_length;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "priority set supported only for KBP TCAM table\n");
        }
    }
    else if (attr_type == DBAL_ENTRY_ATTR_AGE)
    {
        age_state_not_updated = dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_age_state_not_updated);
        if (age_state_not_updated)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "DBAL_ENTRY_ATTR_AGE is not supported");
        }
        if (!dbal_logical_table_is_mact(unit, entry_handle->table_id))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal use for AGE API in table %s", entry_handle->table->table_name);
        }
        (*attr_val) = entry_handle->phy_entry.age;
    }
    else if (attr_type & DBAL_ENTRY_ATTR_HIT_GET)
    {
        if (entry_handle->table->access_method != DBAL_ACCESS_METHOD_MDB)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal use for hitbit API in access method %s",
                         dbal_access_method_to_string(unit, entry_handle->table->access_method));
        }
        (*attr_val) = entry_handle->phy_entry.hitbit & DBAL_PHYSICAL_KEY_PRIMARY_AND_SECONDARY_HITBIT_ACCESSED;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal attribute get type %s", dbal_entry_attr_to_string(unit, attr_type));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_attribute_request(
    int unit,
    uint32 entry_handle_id,
    uint32 attr_type,
    uint32 *attr_val)
{
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    SHR_IF_ERR_EXIT(dbal_entry_attribute_request_internal(unit, entry_handle, attr_type, attr_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_related_table_get(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e * table_id)
{
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    *table_id = entry_handle->table_id;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_key_field_arr32_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val)
{
    int field_size = 0;
    uint32 field_val_arr[1] = { 0 };
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, entry_handle->table_id, field_id, TRUE, 0, 0, &field_size));
    if (field_size <= 32)
    {
        
        SHR_IF_ERR_EXIT(dbal_entry_key_field_get
                        (unit, entry_handle, field_id, field_val_arr, NULL, DBAL_POINTER_TYPE_ARR_UINT32));
        *field_val = field_val_arr[0];
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_key_field_get
                        (unit, entry_handle, field_id, field_val, NULL, DBAL_POINTER_TYPE_ARR_UINT32));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_key_field_arr8_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val)
{
    uint32 field_val_u32[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    SHR_IF_ERR_EXIT(dbal_entry_key_field_get
                    (unit, entry_handle, field_id, field_val_u32, NULL, DBAL_POINTER_TYPE_ARR_UINT8));

    SHR_IF_ERR_EXIT(dbal_fields_transform_arr32_to_arr8
                    (unit, entry_handle->table_id, field_id, TRUE, 0, 0, field_val_u32, field_val, NULL, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_key_field_arr8_masked_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val,
    uint8 *field_mask)
{
    uint32 field_val_u32[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 field_mask_u32[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    if (DBAL_MASKED_TABLE_TYPE(entry_handle->table->table_type))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal use for mask APIs in table type table %s",
                     entry_handle->table->table_name);
    }

    SHR_IF_ERR_EXIT(dbal_entry_key_field_get
                    (unit, entry_handle, field_id, field_val_u32, field_mask_u32, DBAL_POINTER_TYPE_ARR_UINT8));

    SHR_IF_ERR_EXIT(dbal_fields_transform_arr32_to_arr8
                    (unit, entry_handle->table_id, field_id, TRUE, 0, 0, field_val_u32, field_val, field_mask_u32,
                     field_mask));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_key_field_arr32_masked_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 field_val[],
    uint32 field_mask[])
{
    int field_size = 0;
    uint32 field_val_arr[1] = { 0 };
    uint32 field_mask_arr[1] = { 0 };
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    if (DBAL_MASKED_TABLE_TYPE(entry_handle->table->table_type))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal use for mask APIs in table type table %s",
                     entry_handle->table->table_name);
    }

    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, entry_handle->table_id, field_id, TRUE, 0, 0, &field_size));
    if (field_size <= 32)
    {
        
        SHR_IF_ERR_EXIT(dbal_entry_key_field_get
                        (unit, entry_handle, field_id, field_val_arr, field_mask_arr, DBAL_POINTER_TYPE_ARR_UINT32));
        *field_val = field_val_arr[0];
        *field_mask = field_mask_arr[0];
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_key_field_get
                        (unit, entry_handle, field_id, field_val, field_mask, DBAL_POINTER_TYPE_ARR_UINT32));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_value_field8_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 *field_val)
{
    uint32 field_val_u32[1];
    shr_error_e rv;
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    rv = dbal_entry_value_field_get(unit, entry_handle, field_id, inst_id, field_val_u32, NULL,
                                    DBAL_POINTER_TYPE_UINT8);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    SHR_IF_ERR_EXIT_NO_MSG(rv);

    *field_val = field_val_u32[0] & 0xFF;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_value_field16_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint16 *field_val)
{
    uint32 field_val_u32[1];
    shr_error_e rv;
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    rv = dbal_entry_value_field_get(unit, entry_handle, field_id, inst_id, field_val_u32, NULL,
                                    DBAL_POINTER_TYPE_UINT16);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    SHR_IF_ERR_EXIT_NO_MSG(rv);

    *field_val = field_val_u32[0] & 0xFFFF;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_value_field32_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val)
{
    uint32 field_val_u32[1] = { 0 };
    shr_error_e rv;
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    rv = dbal_entry_value_field_get(unit, entry_handle, field_id, inst_id, field_val_u32, NULL,
                                    DBAL_POINTER_TYPE_UINT32);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    SHR_IF_ERR_EXIT_NO_MSG(rv);
    *field_val = field_val_u32[0];

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_value_field_arr8_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint8 *field_val)
{
    dbal_entry_handle_t *entry_handle;
    uint32 field_val_u32[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    rv = dbal_entry_value_field_get(unit, entry_handle, field_id, inst_id, field_val_u32, NULL,
                                    DBAL_POINTER_TYPE_ARR_UINT8);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    SHR_IF_ERR_EXIT_NO_MSG(rv);

    SHR_IF_ERR_EXIT(dbal_fields_transform_arr32_to_arr8
                    (unit, entry_handle->table_id, field_id, FALSE, entry_handle->cur_res_type, inst_id, field_val_u32,
                     field_val, NULL, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_value_field_arr32_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint32 *field_val)
{
    uint32 *field_val_u32;
    shr_error_e rv;
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    field_val_u32 = field_val;
    rv = dbal_entry_value_field_get(unit, entry_handle, field_id, inst_id, field_val_u32, NULL,
                                    DBAL_POINTER_TYPE_ARR_UINT32);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    SHR_IF_ERR_EXIT_NO_MSG(rv);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_value_field64_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    uint64 *field_val)
{
    uint32 field_val_u32[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    shr_error_e rv;
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    rv = dbal_entry_value_field_get(unit, entry_handle, field_id, inst_id, field_val_u32, NULL,
                                    DBAL_POINTER_TYPE_UINT64);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    SHR_IF_ERR_EXIT_NO_MSG(rv);

    COMPILER_64_ZERO(*field_val);
    COMPILER_64_SET(*field_val, field_val_u32[1], field_val_u32[0]);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_outlif_resolve_outlif_res_type(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int jj, cur_res_type;
    uint8 found = FALSE;
    uint32 field_value[1] = { 0 };
    dbal_table_field_info_t table_field_info;

    SHR_FUNC_INIT_VARS(unit);

    for (cur_res_type = 0; (cur_res_type < entry_handle->table->nof_result_types) && (found == FALSE); cur_res_type++)
    {
        if (entry_handle->table->multi_res_info[cur_res_type].is_disabled)
        {
            
            continue;
        }

        SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, entry_handle->table_id,
                                                   DBAL_FIELD_RESULT_TYPE, 0, cur_res_type, 0, &table_field_info));
        
        table_field_info.bits_offset_in_buffer =
            entry_handle->table->max_payload_size - table_field_info.field_nof_bits;
        SHR_IF_ERR_EXIT(dbal_field_from_buffer_get
                        (unit, &table_field_info, DBAL_FIELD_RESULT_TYPE, entry_handle->phy_entry.payload,
                         field_value));
        for (jj = 0; jj < entry_handle->table->multi_res_info[cur_res_type].result_type_nof_hw_values; jj++)
        {
            if (entry_handle->table->multi_res_info[cur_res_type].result_type_hw_value[jj] == field_value[0])
            {
                entry_handle->cur_res_type = cur_res_type;
                found = TRUE;
                break;
            }
        }
    }
    if (found != TRUE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_entry_handle_key_payload_update(
    int unit,
    uint32 entry_handle_id,
    uint32 *key_buffer,
    uint32 *key_mask_buffer,
    int core_id,
    int payload_size,
    uint32 *payload_buffer)
{
    int ii;
    int copy_length, read_start_bit, write_start_bit;
    dbal_entry_handle_t *entry_handle;
    uint8 res_type_resolution_needed = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        
        if ((entry_handle->table->access_method != DBAL_ACCESS_METHOD_MDB)
            && (entry_handle->table->access_method != DBAL_ACCESS_METHOD_KBP))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Cannot perform dbal_entry_handle_key_payload_update on non MDB tables with result type. table %s",
                         entry_handle->table->table_name);
        }
        else if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Cannot perform dbal_entry_handle_key_payload_update on MDB tables with result type in SW state. table %s",
                         entry_handle->table->table_name);
        }
        else
        {
            if (payload_buffer)
            {
                res_type_resolution_needed = TRUE;
            }
            copy_length = UTILEX_MIN(payload_size, entry_handle->table->max_payload_size);
            write_start_bit = entry_handle->table->max_payload_size - copy_length;
            read_start_bit = payload_size - copy_length;
        }
    }
    else
    {
        entry_handle->cur_res_type = 0;
        copy_length = UTILEX_MIN(payload_size, entry_handle->table->multi_res_info[0].entry_payload_size);
        write_start_bit = entry_handle->table->multi_res_info[0].entry_payload_size - copy_length;
        read_start_bit = payload_size - copy_length;
    }
    for (ii = 0; ii < entry_handle->table->nof_key_fields; ii++)
    {
        if (entry_handle->table->keys_info[ii].field_id == DBAL_FIELD_CORE_ID)
        {
            entry_handle->core_id = core_id;
        }
        else
        {
            entry_handle->key_field_ids[ii] = entry_handle->table->keys_info[ii].field_id;
        }
    }

    
    entry_handle->phy_entry.key_size = entry_handle->table->key_size;
    if (key_buffer)
    {
        entry_handle->nof_key_fields = entry_handle->table->nof_key_fields;
        if (DBAL_TABLE_IS_KEY_MASK_REQUIRED(entry_handle->table))
        {
            if (key_mask_buffer == NULL)
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range
                                (entry_handle->phy_entry.k_mask, 0, entry_handle->table->key_size - 1));
            }
            else
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                (key_mask_buffer, 0, entry_handle->table->key_size,
                                 &entry_handle->phy_entry.k_mask[0]));
            }
        }
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (key_buffer, 0, entry_handle->table->key_size, &entry_handle->phy_entry.key[0]));
    }

    
    if (payload_buffer)
    {
        uint32 temp_payload_buffer[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (payload_buffer, read_start_bit, copy_length, temp_payload_buffer));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (temp_payload_buffer, write_start_bit, copy_length, entry_handle->phy_entry.payload));
    }
    entry_handle->phy_entry.payload_size = entry_handle->table->max_payload_size;
    if (res_type_resolution_needed)
    {
        int rv;
        uint32 temp_buffer[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };

        
        if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_KBP)
        {
            rv = dbal_kbp_res_type_resolution(unit, entry_handle, copy_length);
        }
        else if (dbal_table_is_in_lif(entry_handle->table))
        {
            rv = dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                (unit, *entry_handle->phy_entry.key, entry_handle->core_id,
                 entry_handle->table->physical_db_id[0], NULL, (uint32 *) &entry_handle->cur_res_type, NULL);
        }
        else if (dbal_table_is_out_lif(entry_handle->table))
        {
            rv = dbal_outlif_resolve_outlif_res_type(unit, entry_handle);
        }
        else
        {
            rv = dbal_mdb_res_type_resolution(unit, entry_handle, FALSE);
        }

        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
        
        sal_memset(entry_handle->phy_entry.payload, 0, sizeof(entry_handle->phy_entry.payload));
        copy_length = UTILEX_MIN(payload_size, DBAL_RES_INFO.entry_payload_size);
        write_start_bit = DBAL_RES_INFO.entry_payload_size - copy_length;
        read_start_bit = payload_size - copy_length;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(payload_buffer, read_start_bit, copy_length, temp_buffer));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (temp_buffer, write_start_bit, copy_length, entry_handle->phy_entry.payload));
    }
    entry_handle->get_all_fields = TRUE;
    entry_handle->handle_status = DBAL_HANDLE_STATUS_ACTION_PREFORMED;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_access_id_by_key_get(
    int unit,
    uint32 entry_handle_id,
    uint32 *entry_access_id,
    dbal_entry_action_flags_e flags)
{
    dbal_entry_handle_t *entry_handle;
    dbal_actions_e action = DBAL_ACTION_ENTRY_GET_ACCESS_ID;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_action_start(unit, entry_handle_id, &entry_handle, flags, &action));
    if (action == DBAL_ACTION_SKIP)
    {
        SHR_EXIT();
    }

    rv = dbal_actions_access_access_id_by_key_get(unit, entry_handle);
    if (rv != _SHR_E_NONE)
    {
        SHR_SET_CURRENT_ERR(rv);
        SHR_EXIT();
    }

    (*entry_access_id) = entry_handle->phy_entry.entry_hw_id;

    SHR_IF_ERR_EXIT(dbal_action_finish(unit, entry_handle, flags, action));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_default_get(
    int unit,
    uint32 entry_handle_id,
    int result_type)
{
    dbal_logical_table_t *table;
    dbal_fields_e field_id;
    int field_index = 0;
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    table = entry_handle->table;

    if (result_type > table->nof_result_types)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ilegal result type %d. max values if %d", result_type, table->nof_result_types - 1);
    }

    for (field_index = 0; field_index < table->multi_res_info[result_type].nof_result_fields; field_index++)
    {
        field_id = table->multi_res_info[0].results_info[field_index].field_id;
        if (table->multi_res_info[0].results_info[field_index].permission != DBAL_PERMISSION_READONLY)
        {
            dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, field_id, INST_SINGLE,
                                                       DBAL_PREDEF_VAL_DEFAULT_VALUE);
        }
    }

    entry_handle->get_all_fields = 1;
    entry_handle->handle_status = DBAL_HANDLE_STATUS_ACTION_PREFORMED;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_get(
    int unit,
    uint32 entry_handle_id,
    dbal_entry_action_flags_e flags)
{
    int rv = _SHR_E_NONE;
    dbal_entry_handle_t *entry_handle;
    dbal_actions_e action = DBAL_ACTION_ENTRY_GET;
    dbal_logical_table_t *table = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_DBAL_START(unit, DNX_INIT_TIME_ANALYZER_DBAL_GET);

    SHR_IF_ERR_EXIT(dbal_action_start(unit, entry_handle_id, &entry_handle, flags, &action));
    if (action == DBAL_ACTION_SKIP)
    {
        SHR_EXIT();
    }

    table = entry_handle->table;

    
    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED) &&
        !SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_DEFAULT_NON_STANDARD))
    {
        int is_exists = FALSE;

        SHR_IF_ERR_EXIT(dbal_actions_optimized_is_exists(unit, entry_handle, &is_exists));

        if (!is_exists)
        {
            
            sal_memset(&entry_handle->phy_entry.payload, 0, DBAL_TABLE_BUFFER_IN_BYTES(table));
            sal_memset(&entry_handle->phy_entry.p_mask, 0, DBAL_TABLE_BUFFER_IN_BYTES(table));
            if (DBAL_TABLE_IS_NONE_DIRECT(table))
            {
                
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            }
        }
        else
        {
            rv = dbal_actions_access_entry_get(unit, entry_handle, NULL, 0);
        }
    }
    else
    {
        rv = dbal_actions_access_entry_get(unit, entry_handle, NULL, 0);
    }

    if (rv == _SHR_E_NOT_FOUND)
    {
        
        int rc = dbal_logger_table_internal_unlock(unit, entry_handle->table_id);
        if (rc != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(rc,
                         "Entry was not found, but an error occurred later inside dbal_logger_table_internal_unlock\n");
        }
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

    dbal_entry_handle_attribute_parse(unit, entry_handle);

    if (flags & DBAL_GET_ALL_FIELDS)
    {
        entry_handle->handle_status = DBAL_HANDLE_STATUS_ACTION_PREFORMED;
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_fields_parse(unit, entry_handle));
    }

    SHR_IF_ERR_EXIT(dbal_action_finish(unit, entry_handle, flags, action));

exit:
    DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_GET);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_commit(
    int unit,
    uint32 entry_handle_id,
    dbal_entry_action_flags_e flags)
{
    dbal_entry_handle_t *entry_handle;
    dbal_actions_e action = DBAL_ACTION_ENTRY_COMMIT;

    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_DBAL_START(unit, DNX_INIT_TIME_ANALYZER_DBAL_SET);

    SHR_IF_ERR_EXIT(dbal_action_start(unit, entry_handle_id, &entry_handle, flags, &action));
    if (action == DBAL_ACTION_SKIP)
    {
        SHR_EXIT();
    }

    
    if (entry_handle_id >= DBAL_SW_NOF_ENTRY_HANDLES)
    {
        if (action != DBAL_ACTION_ENTRY_COMMIT)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "only entry commit action supported for collection %s %d\n",
                         entry_handle->table->table_name, action);
        }
        entry_handle->handle_status = DBAL_HANDLE_STATUS_ACTION_COLLECTED;
        SHR_IF_ERR_EXIT(dbal_action_finish(unit, entry_handle, flags, action));

        if (DBAL_HANDLE_IS_POOL_FULL(entry_handle))
        {
            
            SHR_IF_ERR_EXIT(dbal_collection_worker_thread_start(unit, DBAL_HANDLE_POOL_ID_GET(entry_handle)));
        }
    }
    else
    {
        switch (action)
        {
            case DBAL_ACTION_ENTRY_COMMIT_RANGE:
                
                SHR_IF_ERR_EXIT(dbal_entry_range_action_process(unit, entry_handle, action));
                break;
            case DBAL_ACTION_ENTRY_COMMIT:
                SHR_IF_ERR_EXIT(dbal_entry_commit_normal(unit, entry_handle));
                break;
            case DBAL_ACTION_ENTRY_COMMIT_UPDATE:
                SHR_IF_ERR_EXIT(dbal_entry_commit_update(unit, entry_handle));
                break;
            case DBAL_ACTION_ENTRY_COMMIT_FORCE:
                SHR_IF_ERR_EXIT(dbal_entry_commit_force(unit, entry_handle));
                break;
            default:
                break;
        }
        SHR_IF_ERR_EXIT(dbal_action_finish(unit, entry_handle, flags, action));
    }

exit:
    DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_SET);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_access_id_set(
    int unit,
    uint32 entry_handle_id,
    uint32 entry_access_id)
{
    dbal_entry_handle_t *entry_handle;
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    table = entry_handle->table;

    if (DBAL_TABLE_IS_TCAM(table))
    {
        if (table->access_method == DBAL_ACCESS_METHOD_KBP && (table->table_type == DBAL_TABLE_TYPE_TCAM))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "access ID set not supported for KBP TCAM tables %s \n", table->table_name);
        }

        if (((table->access_method != DBAL_ACCESS_METHOD_MDB) && (table->access_method != DBAL_ACCESS_METHOD_KBP)) &&
            (entry_access_id >= table->max_capacity))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Table %s max capacity is %d, entry_hw_id is too big: %d \n",
                         table->table_name, table->max_capacity, entry_access_id);
        }

        entry_handle->phy_entry.entry_hw_id = entry_access_id;
        entry_handle->access_id_set = 1;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "access ID set supported only for TCAM table\n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_access_id_get(
    int unit,
    uint32 entry_handle_id,
    uint32 *entry_access_id)
{
    dbal_entry_handle_t *entry_handle;
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    table = entry_handle->table;

    if (DBAL_TABLE_IS_TCAM(table))
    {
        *entry_access_id = entry_handle->phy_entry.entry_hw_id;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "access ID get supported only for TCAM table\n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_clear(
    int unit,
    uint32 entry_handle_id,
    dbal_entry_action_flags_e flags)
{
    dbal_entry_handle_t *entry_handle;
    dbal_actions_e action = DBAL_ACTION_ENTRY_CLEAR;

    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_DBAL_START(unit, DNX_INIT_TIME_ANALYZER_DBAL_ENTRY_CLEAR);

    SHR_IF_ERR_EXIT(dbal_action_start(unit, entry_handle_id, &entry_handle, flags, &action));
    if (action == DBAL_ACTION_SKIP)
    {
        SHR_EXIT();
    }

    if (action == DBAL_ACTION_ENTRY_CLEAR)
    {
        SHR_IF_ERR_EXIT(dbal_actions_access_entry_clear(unit, entry_handle));
    }
    else if (action == DBAL_ACTION_ENTRY_CLEAR_RANGE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_range_action_process(unit, entry_handle, action));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown action %d\n", action);
    }

    SHR_IF_ERR_EXIT(dbal_action_finish(unit, entry_handle, flags, action));

exit:
    DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_ENTRY_CLEAR);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_result_type_update(
    int unit,
    uint32 entry_handle_id,
    int new_result_type,
    dbal_result_type_update_mode_e mode)
{
    dbal_logical_table_t *table;
    dbal_entry_handle_t *entry_handle;
    dbal_entry_handle_t *update_entry_handle;
    int field_index;
    dbal_fields_e field_id;
    dbal_fields_e new_field_id;
    int result_type;
    uint32 update_entry_handle_id;
    dbal_table_field_info_t field_info, new_field_info;
    dbal_fields_e sub_field_id;
    uint32 sub_field_val;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    table = entry_handle->table;
    result_type = entry_handle->cur_res_type;

    if (DBAL_TABLE_IS_TCAM(table))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "API does not support TCAM tables");
    }
    if (new_result_type > table->nof_result_types)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal result type %d. Max result type value is %d", new_result_type,
                     table->nof_result_types - 1);
    }
    if (new_result_type == result_type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "New result type given matches the current result type %d", new_result_type);
    }

    
    if ((mode == DBAL_RESULT_TYPE_UPDATE_MODE_STRICT) || (mode == DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE_DEFAULT))
    {
        for (field_index = 0; field_index < table->multi_res_info[result_type].nof_result_fields; field_index++)
        {
            field_id = table->multi_res_info[result_type].results_info[field_index].field_id;
            
            sal_memset(&field_info, 0x0, sizeof(dbal_table_field_info_t));
            SHR_IF_ERR_EXIT_EXCEPT_IF(dbal_tables_field_info_get_no_err
                                      (unit, entry_handle->table_id, field_id, 0, new_result_type, INST_SINGLE,
                                       &field_info), _SHR_E_NOT_FOUND);

            
            if (!field_info.field_nof_bits)
            {
                if (mode == DBAL_RESULT_TYPE_UPDATE_MODE_STRICT)
                {
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                                 "Requested new result type in table %s is missing field %s from original result type, forbidden in STRICT mode\n",
                                 entry_handle->table->table_name, dbal_field_to_string(unit, field_id));
                }
                else if (mode == DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE_DEFAULT)
                {
                    uint32 default_field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
                    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

                    
                    SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get
                                    (unit, entry_handle_id, field_id, 0, DBAL_PREDEF_VAL_DEFAULT_VALUE,
                                     default_field_value));

                    
                    SHR_IF_ERR_EXIT(dbal_field_physical_value_from_buffer_get(unit,
                                                                              &entry_handle->table->multi_res_info
                                                                              [result_type].results_info[field_index],
                                                                              field_id, entry_handle->phy_entry.payload,
                                                                              field_value));

                    if (sal_memcmp(field_value, default_field_value, field_info.field_nof_bits))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "All fields which only exist in old result type must have default value in loose default mode (field name: %s)\n",
                                     entry_handle->table->table_name);
                    }
                }
            }
        }
    }

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, entry_handle->table_id, &update_entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, update_entry_handle_id, &update_entry_handle));

    dbal_entry_value_field32_set(unit, update_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, new_result_type);

    
    for (field_index = 0; field_index < table->multi_res_info[new_result_type].nof_result_fields; field_index++)
    {
        uint8 is_valid_value = FALSE;
        uint32 value_to_use;

        new_field_id = table->multi_res_info[new_result_type].results_info[field_index].field_id;
        if (new_field_id == DBAL_FIELD_RESULT_TYPE)
        {
            continue;
        }
        
        sal_memset(&field_info, 0x0, sizeof(dbal_table_field_info_t));
        SHR_IF_ERR_EXIT_EXCEPT_IF(dbal_tables_field_info_get_no_err
                                  (unit, entry_handle->table_id, new_field_id, 0, result_type, INST_SINGLE,
                                   &field_info), _SHR_E_NOT_FOUND);
        
        if (field_info.field_nof_bits)
        {
            uint32 val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
            field_id = field_info.field_id;
            sub_field_val = 0;

            
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE, val));
            SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode_no_err(unit, field_id, val, &sub_field_id, &sub_field_val));
            value_to_use = (sub_field_id == DBAL_FIELD_EMPTY) ? val[0] : sub_field_val;
            sal_memset(&new_field_info, 0x0, sizeof(dbal_table_field_info_t));
            SHR_IF_ERR_EXIT_EXCEPT_IF(dbal_tables_field_info_get_no_err
                                      (unit, entry_handle->table_id, field_id, 0, new_result_type, INST_SINGLE,
                                       &new_field_info), _SHR_E_NOT_FOUND);

            
            if ((sub_field_id == DBAL_FIELD_EMPTY) || (sub_field_id == new_field_id))
            {
                
                SHR_IF_ERR_EXIT(dbal_value_field32_validate(unit, &value_to_use, &new_field_info, &is_valid_value));
            }
        }

        
        if (is_valid_value)
        {
            dbal_entry_value_field_arr32_set(unit, update_entry_handle_id, new_field_id, INST_SINGLE, &value_to_use);
            if (update_entry_handle->error_info.error_exists)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failed updating field %s\n", dbal_field_to_string(unit, new_field_id));
            }
        }
        else
        {
            dbal_entry_value_field_predefine_value_set(unit, update_entry_handle_id, new_field_id, INST_SINGLE,
                                                       DBAL_PREDEF_VAL_DEFAULT_VALUE);
        }
    }

    dbal_entry_handle_update_result(unit, update_entry_handle_id, entry_handle_id);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_table_clear(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id;
    dbal_entry_handle_t *entry_handle;
    dbal_actions_e action = DBAL_ACTION_TABLE_CLEAR;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_DBAL_START(unit, DNX_INIT_TIME_ANALYZER_DBAL_CLEAR);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_action_start(unit, entry_handle_id, &entry_handle, 0, &action));
    if (action == DBAL_ACTION_SKIP)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_actions_access_table_clear(unit, entry_handle));

    SHR_IF_ERR_EXIT(dbal_action_finish(unit, entry_handle, 0, action));

exit:
    DBAL_FUNC_FREE_VARS;
    DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_CLEAR);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_bulk_mode_set(
    int unit,
    dbal_access_method_e access_method,
    dbal_bulk_module_e bulk_module,
    int collect_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
        {
            if (bulk_module != DBAL_BULK_MODULE_MDB_LEM_FLUSH)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "bulk_module not supported %d \n", bulk_module);
            }
            if (collect_mode)
            {
                SHR_IF_ERR_EXIT(dbal_flush_mode_collect(unit));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_flush_mode_normal(unit));
            }
            break;
        }
        case DBAL_ACCESS_METHOD_KBP:
        {
            
            if (bulk_module == DBAL_BULK_MODULE_KBP_FWD)
            {
                SHR_IF_ERR_EXIT(dnx_kbp_caching_enabled_set(unit, FALSE, collect_mode));
            }
            else if (bulk_module == DBAL_BULK_MODULE_KBP_ACL)
            {
                SHR_IF_ERR_EXIT(dnx_kbp_caching_enabled_set(unit, TRUE, collect_mode));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "bulk_module not supported %d \n", bulk_module);
            }
            break;
        }

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "access_method not supported %s \n",
                         dbal_access_method_to_string(unit, access_method));
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_bulk_mode_get(
    int unit,
    dbal_access_method_e access_method,
    dbal_bulk_module_e bulk_module,
    int *collect_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
        {
            if (bulk_module != DBAL_BULK_MODULE_MDB_LEM_FLUSH)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "bulk_module not supported %d \n", bulk_module);
            }
            SHR_IF_ERR_EXIT(dbal_flush_mode_get(unit, collect_mode));
            break;
        }
        case DBAL_ACCESS_METHOD_KBP:
        {
            
            if (bulk_module == DBAL_BULK_MODULE_KBP_FWD)
            {
                SHR_IF_ERR_EXIT(dnx_kbp_caching_enabled_get(unit, FALSE, collect_mode));
            }
            else if (bulk_module == DBAL_BULK_MODULE_KBP_ACL)
            {
                SHR_IF_ERR_EXIT(dnx_kbp_caching_enabled_get(unit, TRUE, collect_mode));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "bulk_module not supported %d \n", bulk_module);
            }
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "access_method not supported %s \n",
                         dbal_access_method_to_string(unit, access_method));
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_bulk_commit(
    int unit,
    dbal_access_method_e access_method,
    dbal_bulk_module_e bulk_module)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
        {
            if (bulk_module == DBAL_BULK_MODULE_MDB_LEM_FLUSH)
            {
                SHR_IF_ERR_EXIT(dbal_flush_start(unit, TRUE, TRUE, 0));
            }
            else if (bulk_module == DBAL_BULK_MODULE_MDB_LEM_NON_BLOCKING_FLUSH)
            {
                SHR_IF_ERR_EXIT(dbal_flush_start(unit, TRUE, TRUE, 1));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "bulk_module not supported %d \n", bulk_module);
            }

            break;
        }
        case DBAL_ACCESS_METHOD_KBP:
        {
            int enabled;
#if defined(INCLUDE_KBP)
            int is_acl;
            if (bulk_module == DBAL_BULK_MODULE_KBP_FWD)
            {
                is_acl = FALSE;
            }
            else if (bulk_module == DBAL_BULK_MODULE_KBP_ACL)
            {
                is_acl = TRUE;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "bulk_module not supported %d \n", bulk_module);
            }
#else
            if ((bulk_module != DBAL_BULK_MODULE_KBP_FWD) && (bulk_module != DBAL_BULK_MODULE_KBP_ACL))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "bulk_module not supported %d \n", bulk_module);
            }
#endif
            SHR_IF_ERR_EXIT(dbal_bulk_mode_get(unit, DBAL_ACCESS_METHOD_KBP, bulk_module, &enabled));
            if (!enabled)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP bulk_module not enabled %d \n", bulk_module);
            }
#if defined(INCLUDE_KBP)
            SHR_IF_ERR_EXIT(dbal_kbp_access_cache_commit(unit, is_acl));
#endif
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "access_method not supported %s \n",
                         dbal_access_method_to_string(unit, access_method));
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_bulk_clear(
    int unit,
    dbal_access_method_e access_method,
    dbal_bulk_module_e bulk_module)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            if (bulk_module != DBAL_BULK_MODULE_MDB_LEM_FLUSH)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "bulk_module not supported %d \n", bulk_module);
            }
            SHR_IF_ERR_EXIT(dbal_flush_db_clear(unit));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "access_method not supported %s \n",
                         dbal_access_method_to_string(unit, access_method));
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

int
dbal_is_collection_mode_enabled(
    int unit)
{
    if (dnx_data_dbal.db_init.dbal_collection_pool_size_get(unit))
    {
        return 1;
    }
    return 0;
}

shr_error_e
dbal_collection_trigger_set(
    int unit,
    dbal_collection_trigger_type_e trigger_type)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_collection_trigger_set_internal(unit, trigger_type));

exit:
    SHR_FUNC_EXIT;
}
