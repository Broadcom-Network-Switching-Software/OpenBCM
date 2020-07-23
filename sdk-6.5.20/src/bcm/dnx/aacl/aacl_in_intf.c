/** \file aacl_in_intf.c
 *
 *  aacl procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_AACL_MNGR
/*
 * Include files.
 * {
 */

#include <bcm_int/dnx/aacl/aacl_in_intf.h>
#include <shared/util.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include "aacl_out_intf.h"

/** When real feature exists this include goes to a full file, when feature is not supported this include goes to
 *  stub file  */
#ifdef DNX_AACL_OPERATIONAL
#include "aacl_mngr.h"
#endif
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */
/*
 * }
 */

/*
 * GLOBALs
 * {
 */
/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

/**
* See .h
*/
#ifdef DNX_AACL_OPERATIONAL

/*
 * Array to get the aacl mngr field type from dbal field id
 */
aacl_field_type aacl_field_type_from_dbal_field[BCM_MAX_NUM_UNITS][AACL_MAX_GROUP_ID][AACL_MAX_NUM_OF_FILEDS];

/** Array to map aacl mngr field_id to dbal_field_id */
dbal_fields_e aacl_field_to_dbal_field_map[BCM_MAX_NUM_UNITS][AACL_MAX_GROUP_ID][AACL_MAX_NUM_OF_FILEDS];

/*
 * Function which converts the AACL mngr errors to SDK errors
 */
static shr_error_e
dnx_aacl_error_translate(
    aacl_error aacl_err)
{
    shr_error_e translated_err;

    switch (aacl_err)
    {
        case AACL_ERR_NONE:
            translated_err = _SHR_E_NONE;
            break;
        case AACL_ERR_PARAM:
            translated_err = _SHR_E_PARAM;
            break;
        case AACL_ERR_MEMORY:
            translated_err = _SHR_E_MEMORY;
            break;
        case AACL_ERR_FULL:
            translated_err = _SHR_E_FULL;
            break;
        default:
            translated_err = _SHR_E_INTERNAL;
    }

    return translated_err;
}

/*
 * Macro to guard the AACL mngr functions
 */
#define DNX_IF_AACL_ERR_EXIT(A)                                                                                              \
    do                                                                                                              \
    {                                                                                                               \
        aacl_error __tmp_status = A;                                                                                \
        if (__tmp_status != AACL_ERR_NONE)                                                                                 \
        {                                                                                                           \
            SHR_IF_ERR_EXIT(dnx_aacl_error_translate(__tmp_status));                          \
        }                                                                                                           \
    }                                                                                                               \
    while(0)

/*
 * Funtion to calculate the prefix length from the mask array
 */
static shr_error_e
dnx_aacl_calculate_prefix_length(
    int unit,
    uint32 *payload_mask,
    uint32 max_key_size_in_bits,
    uint32_t * ret_prefix_length)
{
    int uint32_index;
    uint32 max_key_size_in_uint32 = BITS2WORDS(max_key_size_in_bits);
    uint32 prefix_length = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (uint32_index = max_key_size_in_uint32 - 1; uint32_index >= 0; uint32_index--)
    {
        /*
         * If the prefix is full or if this is the MSB, ignore the prefix MSB masking
         */
        if (payload_mask[uint32_index] == SAL_UINT32_MAX)
        {
            prefix_length += SAL_UINT32_NOF_BITS;
        }
        else
        {
            prefix_length += SAL_UINT32_NOF_BITS - (utilex_msb_bit_on(~payload_mask[uint32_index]) + 1);
            break;
        }
    }

    *ret_prefix_length = prefix_length;
exit:
    SHR_FUNC_EXIT;
}
/*
 * Function to assign initial aacl field types to invalid
 */
static shr_error_e
dnx_aacl_assign_initial_type(
    int unit,
    int group_id,
    dbal_tables_e overflow_dbal_id)
{
    int field_index = 0;
    dbal_fields_e next_field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_next_field_get(unit, overflow_dbal_id, DBAL_FIELD_EMPTY, 1, 0, &next_field_id));

    while (next_field_id != DBAL_FIELD_EMPTY)
    {
        aacl_field_type_from_dbal_field[unit][group_id][field_index] = AACL_FIELD_TYPE_INVALID;
        field_index++;
        SHR_IF_ERR_EXIT(dbal_tables_next_field_get(unit, overflow_dbal_id, next_field_id, 1, 0, &next_field_id));
    }
exit:
    SHR_FUNC_EXIT;

}

/*
 * Function to assign aacl field types (em, prefix) to index1/index2 dbal field ids
 */
static shr_error_e
dnx_aacl_assign_em_prefix_type(
    int unit,
    int group_id,
    dbal_tables_e index_dbal_id,
    dbal_tables_e overflow_dbal_id)
{
    int found = 0;
    int overflow_field_index = 0;
    dbal_fields_e index_next_field_id;
    dbal_fields_e overflow_next_field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_next_field_get(unit, index_dbal_id, DBAL_FIELD_EMPTY, 1, 0, &index_next_field_id));

    /*
     * Field_id in index1/index2 tables is em/prefix fields 
     */
    while (index_next_field_id != DBAL_FIELD_EMPTY)
    {
        found = 0;
        SHR_IF_ERR_EXIT(dbal_tables_next_field_get
                        (unit, overflow_dbal_id, DBAL_FIELD_EMPTY, 1, 0, &overflow_next_field_id));

        while (overflow_next_field_id != DBAL_FIELD_EMPTY)
        {
            if (index_next_field_id == overflow_next_field_id)
            {
                found = 1;
                break;
            }
            overflow_field_index++;
            SHR_IF_ERR_EXIT(dbal_tables_next_field_get
                            (unit, overflow_dbal_id, overflow_next_field_id, 1, 0, &overflow_next_field_id));

        }
        if (found == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unable to map index field id in overflow fields \n");
        }

        aacl_field_type_from_dbal_field[unit][group_id][overflow_field_index] = AACL_FIELD_TYPE_PREFIX;

        SHR_IF_ERR_EXIT(dbal_tables_next_field_get
                        (unit, index_dbal_id, index_next_field_id, 1, 0, &index_next_field_id));

    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function to assign aacl range/ternary field types
 */
static shr_error_e
dnx_aacl_assign_range_ternary_type(
    int unit,
    int group_id,
    dbal_tables_e overflow_dbal_id)
{
    int field_index = 0;
    dbal_fields_e next_field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_next_field_get(unit, overflow_dbal_id, DBAL_FIELD_EMPTY, 1, 0, &next_field_id));

    while (next_field_id != DBAL_FIELD_EMPTY)
    {
        if (aacl_field_type_from_dbal_field[unit][group_id][field_index] == AACL_FIELD_TYPE_INVALID)
        {
            SHR_IF_ERR_EXIT(dbal_table_field_is_ranged_get(unit, overflow_dbal_id, next_field_id, &is_ranged));
            if (is_ranged)
            {
                aacl_field_type_from_dbal_field[unit][group_id][iter] = AACL_FIELD_TYPE_RANGE;
            }
            else
            {
                /*
                 * As we already assigned the em/prefix types remaining field types are ternary
                 */
                aacl_field_type_from_dbal_field[unit][group_id][iter] = AACL_FIELD_TYPE_TERNARY;
            }
        }
        field_index++;
        SHR_IF_ERR_EXIT(dbal_tables_next_field_get(unit, overflow_dbal_id, next_field_id, 1, 0, &next_field_id));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_aacl_aasign_dbal_field_types(
    int unit,
    int group_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Assign initial field type to invalid type for all fields 
     */
    SHR_IF_ERR_EXIT(dnx_aacl_assign_initial_type(unit, group_id,
                                                 aacl_group_to_dbal_info[unit][group_id].overflow_dbal_id));

    /*
     * Field_id in index1/index2 tables is em/prefix fields 
     */
    SHR_IF_ERR_EXIT(dnx_aacl_assign_em_prefix_type(unit, group_id,
                                                   aacl_group_to_dbal_info[unit][group_id].index1_dbal_id,
                                                   aacl_group_to_dbal_info[unit][group_id].overflow_dbal_id));

    SHR_IF_ERR_EXIT(dnx_aacl_assign_em_prefix_type(unit, group_id,
                                                   aacl_group_to_dbal_info[unit][group_id].index2_dbal_id,
                                                   aacl_group_to_dbal_info[unit][group_id].overflow_dbal_id));

    /*
     * Assign the field type to range/ternary from the dbal field ids 
     */
    SHR_IF_ERR_EXIT(dnx_aacl_assign_range_ternary_type(unit, group_id,
                                                       aacl_group_to_dbal_info[unit][group_id].overflow_dbal_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_field_id_from_dbal_field_id_get(
    int unit,
    int group_id,
    dbal_fields_e field_id uint32 *aacl_mngr_field_id)
{
    int iter;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < AACL_MAX_NUM_OF_FILEDS; iter++)
    {
        if (aacl_field_to_dbal_field_map[unit][group_id][iter] == field_id)
            break;
    }
    if (iter == AACL_MAX_NUM_OF_FILEDS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unable to map AACL mngr field id to DBAL field id \n");
    }
    *aacl_mngr_field_id = iter;

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function to get the aacl mngr field type from dbal field id
 */

shr_error_e
dnx_aacl_field_type_get(
    int unit,
    int group_id,
    dbal_fields_e field_id,
    enum aacl_field_type *type)
{
    int field_index = 0;
    int found = 0;
    dbal_fields_e next_field_id;
    dbal_tables_e overflow_dbal_id;

    SHR_FUNC_INIT_VARS(unit);

    overflow_dbal_id = aacl_group_to_dbal_info[unit][group_id].overflow_dbal_id;

    SHR_IF_ERR_EXIT(dbal_tables_next_field_get(unit, overflow_dbal_id, DBAL_FIELD_EMPTY, 1, 0, &next_field_id));

    while (next_field_id != DBAL_FIELD_EMPTY)
    {
        if (next_field_id == field_id)
        {
            found = 1;
            *type = aacl_field_type_from_dbal_field[unit][group_id][iter];
            break;
        }
        field_index++;
        SHR_IF_ERR_EXIT(dbal_tables_next_field_get(unit, overflow_dbal_id, next_field_id, 1, 0, &next_field_id));
    }
    if (found == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unable to get AACL mngr field type from DBAL field id \n");
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_init(
    int unit)
{
    aacl_mngr_out_intf_cb out_cbs;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Assigning the callback function pointers to write tcam/lpm entries to HW
     */
    out_cbs.tcam_entry_add = dnx_aacl_tcam_entry_add;
    out_cbs.tcam_entry_delete = dnx_aacl_tcam_entry_delete;
    out_cbs.tcam_entry_payload_update = dnx_aacl_tcam_entry_payload_update;
    out_cbs.lpm_entry_add = dnx_aacl_lpm_entry_add;
    out_cbs.lpm_entry_delete = dnx_aacl_lpm_entry_delete;
    out_cbs.lpm_entry_payload_update = dnx_aacl_lpm_entry_payload_update;

    DNX_IF_AACL_ERR_EXIT(aacl_mngr_init(unit, &out_cbs));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_IF_AACL_ERR_EXIT(aacl_mngr_deinit(unit));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_is_enabled(
    int unit,
    uint8 *is_enabled)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_enabled = 1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_group_create(
    int unit,
    uint32 *group_id,
    char *group_name,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(group_id, _SHR_E_INTERNAL, "group_id");
    SHR_NULL_CHECK(group_name, _SHR_E_INTERNAL, "group_name");

    DNX_IF_AACL_ERR_EXIT(aacl_mngr_group_create(unit, group_id, group_name, flags));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_group_dbal_tables_connect(
    int unit,
    uint32 group_id,
    dbal_tables_e compressed_dbal_id,
    dbal_tables_e overflow_dbal_id,
    dbal_tables_e index1_dbal_id,
    dbal_tables_e index2_dbal_id)
{
    const dbal_logical_table_t *table;
    int field_index;
    uint32 field_id;
    enum aacl_field_type type;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Bookkeep the dbal table_id per group
 */
    aacl_group_to_dbal_info[unit][group_id].compressed_dbal_id = compressed_dbal_id;
    aacl_group_to_dbal_info[unit][group_id].overflow_dbal_id = overflow_dbal_id;
    aacl_group_to_dbal_info[unit][group_id].index1_dbal_id = index1_dbal_id;
    aacl_group_to_dbal_info[unit][group_id].index2_dbal_id = index2_dbal_id;

    /*
     * Assign the dbal field types
     */
    SHR_IF_ERR_EXIT(dnx_aacl_aasign_dbal_field_types(unit, group_id));

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, overflow_dbal_id, &table));

    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        SHR_IF_ERR_EXIT(dnx_aacl_field_type_get(unit, group_id, table->keys_info[field_index].field_id, &type));

        DNX_IF_AACL_ERR_EXIT(aacl_mngr_field_add(unit, group_id,
                                                 &field_id,
                                                 dbal_field_to_string(unit, table->keys_info[field_index].field_id),
                                                 type, table->keys_info[field_index].field_nof_bits));

        /*
         * Structure to book keep /map the aacl_mngr field ids to dbal field ids
         */
        aacl_field_to_dbal_field_map[unit][group_id][field_id] = table->keys_info[field_index].field_id;
    }
    DNX_IF_AACL_ERR_EXIT(aacl_mngr_group_lock(unit, group_id));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_entry_create(
    int unit,
    uint32 group_id,
    uint32 priority,
    uint32 *entry_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_id, _SHR_E_INTERNAL, "entry_id");

    DNX_IF_AACL_ERR_EXIT(aacl_mngr_entry_create(unit, group_id, priority, entry_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_key_field_masked_set(
    int unit,
    uint32 entry_id,
    dbal_fields_e field_id,
    uint32 *field_value,
    uint32 *field_mask)
{
    uint32 aacl_mngr_field_id;
    uint32 prefix_length;
    uint32 field_size;
    enum aacl_field_type type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(field_value, _SHR_E_INTERNAL, "field_value");
    SHR_NULL_CHECK(field_mask, _SHR_E_INTERNAL, "field_mask");

    SHR_IF_ERR_EXIT(dnx_aacl_field_type_get(unit, group_id, field_id, &type));

    aacl_mngr_field_id = dnx_aacl_field_id_from_dbal_field_id_get(group_id, field_id);

    switch (type)
    {
        case AACL_FIELD_TYPE_TERNARY:
        {
            DNX_IF_AACL_ERR_EXIT(aacl_mngr_entry_add_ternary
                                 (unit, group_id, entry_id, aacl_mngr_field_id, field_value, field_mask));
            break;
        }

        case AACL_FIELD_TYPE_PREFIX:
        {
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, aacl_group_to_dbal_info[unit][group_id].overflow_dbal_id,
                                                       field_id, 1, 0, 0, &field_size));

            SHR_IF_ERR_EXIT(dnx_aacl_calculate_prefix_length(unit, field_mask, field_size, &prefix_length));
            DNX_IF_AACL_ERR_EXIT(aacl_mngr_entry_add_prefix(unit, group_id, entry_id,
                                                            aacl_mngr_field_id, field_value, prefix_length));
            break;
        }

        case AACL_FIELD_TYPE_EM:
        {
            DNX_IF_AACL_ERR_EXIT(aacl_mngr_entry_add_em(unit, group_id, entry_id, aacl_mngr_field_id, field_value));
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown AACL field type \n");
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_key_field_range_set(
    int unit,
    uint32 entry_id,
    dbal_fields_e field_id,
    uint32 min_value,
    uint32 max_value)
{
    uint32 aacl_mngr_field_id;
    SHR_FUNC_INIT_VARS(unit);
    aacl_mngr_field_id = dnx_aacl_field_id_from_dbal_field_id_get(group_id, field_id);
    DNX_IF_AACL_ERR_EXIT(aacl_mngr_entry_add_range(unit, entry_id, aacl_mngr_field_id, min_value, max_value));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_result_field_set(
    int unit,
    uint32 entry_id,
    dbal_fields_e field_id,
    uint32 *result_value)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(result_value, _SHR_E_INTERNAL, "result_value");
    DNX_IF_AACL_ERR_EXIT(_aacl_mngr_entry_result_field_set(unit, entry_id, field_id, result_value));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_entry_install(
    int unit,
    uint32 entry_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_IF_AACL_ERR_EXIT(aacl_mngr_entry_commit(unit, entry_id));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_batch_collect(
    int unit,
    uint32 group_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_IF_AACL_ERR_EXIT(aacl_mngr_batch_collect(unit, group_id));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_batch_commit(
    int unit,
    uint32 group_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_IF_AACL_ERR_EXIT(aacl_mngr_batch_commit(unit, group_id));
exit:
    SHR_FUNC_EXIT;
}

#else

shr_error_e
dnx_aacl_is_enabled(
    int unit,
    uint8 *is_enabled)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_enabled = 0;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
shr_error_e dnx_aacl_init(int unit){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "AACL unavailable\n");exit:SHR_FUNC_EXIT;}
shr_error_e dnx_aacl_deinit(int unit){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "AACL unavailable\n");exit:SHR_FUNC_EXIT;}
shr_error_e dnx_aacl_group_create(int unit, uint32 *group_id, char *group_name, uint32 flags) {SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "AACL unavailable\n");exit:SHR_FUNC_EXIT;}
shr_error_e dnx_aacl_group_dbal_tables_connect(int unit, uint32 group_id, dbal_tables_e compressed_dbal_id, dbal_tables_e overflow_dbal_id, dbal_tables_e index1_dbal_id, dbal_tables_e index2_dbal_id) {SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "AACL unavailable\n");exit:SHR_FUNC_EXIT;}
shr_error_e dnx_aacl_entry_create(int unit, uint32 group_id, uint32 priority, uint32 *entry_id){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "AACL unavailable\n");exit:SHR_FUNC_EXIT;}
shr_error_e dnx_aacl_key_field_masked_set(int unit, uint32 entry_id, dbal_fields_e field_id, uint32 *field_value, uint32 *field_mask) {SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "AACL unavailable\n");exit:SHR_FUNC_EXIT;}
shr_error_e dnx_aacl_key_field_range_set(int unit, uint32 entry_id, dbal_fields_e field_id, uint32 min_value, uint32 max_value){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "AACL unavailable\n");exit:SHR_FUNC_EXIT;}
shr_error_e dnx_aacl_result_field_set(int unit, uint32 entry_id, dbal_fields_e field_id, uint32 *result_value){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "AACL unavailable\n");exit:SHR_FUNC_EXIT;}
shr_error_e dnx_aacl_entry_install(int unit, uint32 entry_id){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "AACL unavailable\n");exit:SHR_FUNC_EXIT;}
shr_error_e dnx_aacl_batch_collect(int unit, uint32 group_id){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "AACL unavailable\n");exit:SHR_FUNC_EXIT;}
shr_error_e dnx_aacl_batch_commit(int unit, uint32 group_id) {SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "AACL unavailable\n");exit:SHR_FUNC_EXIT;}
/* *INDENT-ON* */

#endif
