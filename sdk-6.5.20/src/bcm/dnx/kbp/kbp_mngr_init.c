/** \file kbp_mngr_init.c
 *
 *  kbp procedures for DNX that should be used during KBP mngr init.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_KBP_MNGR

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm/types.h>
#include <shared/util.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <bcm_int/dnx/kbp/kbp_connectivity.h>
#include <bcm_int/dnx/kbp/kbp_rop.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dbal/dbal_dynamic.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include "kbp_mngr_internal.h"
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <sal/appl/sal.h>
#include <bcm_int/dnx/field/field_key.h>

#include <sal/core/boot.h>
#include <bcm_int/dnx/kbp/auto_generated/dbx_pre_compiled_kbp_auto_generated_xml_parser.h>

/*
 * }
 */
#if defined(INCLUDE_KBP)
/*
 * DEFINEs
 * {
 */

/*
 * Hash tables must take a power of two for table size.
 * This macro is used to convert an arbitrary size into containing legitimate hash table size.
 * It finds the smallest power of two which is larger than or equal to the input size.
 */
#define KBP_FWD_TCAM_ACCESS_HASH_TABLE_SIZE_CONVERT(_size) \
        (1 << utilex_log2_round_up(_size))

/*
 * Multicast mode 1
 * Will set the following to KBP, ACL algo mode 3, critical fields (VRF & DIP)
 * and KBP_DB_MC_TYPE1.
 */
#define KBP_DB_MC_MODE1 1

/*
 * Multicast mode 2
 * Will set the following to KBP, ACL algo mode 3 and KBP_DB_MC_TYPE2.
 */
#define KBP_DB_MC_MODE2 2

/*
 * }
 */
/*
 * GLOBALs
 * {
 */
extern generic_kbp_app_data_t *Kbp_app_data[SOC_MAX_NUM_DEVICES];
extern kbp_warmboot_t Kbp_warmboot_data[SOC_MAX_NUM_DEVICES];

/*
 * }
 */
/*
 * FUNCTIONs
 * {
 */

/**
 * Convert DNX (DBAL) DB type for KBP DB type
 */
static shr_error_e
kbp_mngr_dbal_db_type_to_kbp_db_type(
    int unit,
    dbal_table_type_e type,
    int *kbp_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case DBAL_TABLE_TYPE_LPM:
            *kbp_type = KBP_DB_LPM;
            break;
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_BY_ID:
            *kbp_type = KBP_DB_ACL;
            break;
        case DBAL_TABLE_TYPE_EM:
            *kbp_type = KBP_DB_EM;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported DBAL table type to KBP %d\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Convert ResultLocation from string to enum
 */
static shr_error_e
kbp_mngr_valid_result_location_parse(
    int unit,
    char *result_location,
    uint8 *valid_location)
{
    SHR_FUNC_INIT_VARS(unit);

    if (sal_strcmp(result_location, "FWD") == 0)
    {
        *valid_location = DBAL_DEFINE_KBP_RESULT_LOCATION_FWD;
    }
    else if (sal_strcmp(result_location, "RPF") == 0)
    {
        *valid_location = DBAL_DEFINE_KBP_RESULT_LOCATION_RPF;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid result location: %s. Valid are FWD and RPF\n", result_location);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Convert given integer to KBP priority define
 */
static shr_error_e
kbp_mngr_kbp_meta_priority_get(
    int unit,
    int meta_priority,
    int *kbp_meta_priority)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(kbp_meta_priority, _SHR_E_INTERNAL, "kbp_meta_priority");

    switch (meta_priority)
    {
        case 0:
            *kbp_meta_priority = KBP_ENTRY_META_PRIORITY_0;
            break;
        case 1:
            *kbp_meta_priority = KBP_ENTRY_META_PRIORITY_1;
            break;
        case 2:
            *kbp_meta_priority = KBP_ENTRY_META_PRIORITY_2;
            break;
        case 3:
            *kbp_meta_priority = KBP_ENTRY_META_PRIORITY_3;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported META PRIORITY value %d\n", meta_priority);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Convert DBAL key field type to KBP key field type
 */
shr_error_e
kbp_mngr_dbal_key_field_type_to_kbp_key_field_type(
    int unit,
    uint32 dbal_key_field_type,
    int *kbp_key_field_type)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(kbp_key_field_type, _SHR_E_INTERNAL, "kbp_key_field_type");

    switch (dbal_key_field_type)
    {
        case DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_INVALID:
            *kbp_key_field_type = KBP_KEY_FIELD_INVALID;
            break;
        case DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_PREFIX:
            *kbp_key_field_type = KBP_KEY_FIELD_PREFIX;
            break;
        case DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_EM:
            *kbp_key_field_type = KBP_KEY_FIELD_EM;
            break;
        case DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_RANGE:
            *kbp_key_field_type = KBP_KEY_FIELD_RANGE;
            break;
        case DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_TERNARY:
            *kbp_key_field_type = KBP_KEY_FIELD_TERNARY;
            break;
        case DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_HOLE:
            *kbp_key_field_type = KBP_KEY_FIELD_HOLE;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported DBAL KBP_KEY_FIELD %d\n", dbal_key_field_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Transforms KBP key field type to DBAL key field type
 */
shr_error_e
kbp_mngr_kbp_key_field_type_to_dbal_key_field_type(
    int unit,
    uint32 kbp_key_field_type,
    uint32 *dbal_key_field_type)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dbal_key_field_type, _SHR_E_INTERNAL, "dbal_key_field_type");

    switch (kbp_key_field_type)
    {
        case KBP_KEY_FIELD_TERNARY:
            *dbal_key_field_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_TERNARY;
            break;
        case KBP_KEY_FIELD_PREFIX:
            *dbal_key_field_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_PREFIX;
            break;
        case KBP_KEY_FIELD_EM:
            *dbal_key_field_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_EM;
            break;
        case KBP_KEY_FIELD_RANGE:
            *dbal_key_field_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_RANGE;
            break;
        case KBP_KEY_FIELD_HOLE:
            *dbal_key_field_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_HOLE;
            break;
        case KBP_KEY_FIELD_INVALID:
            *dbal_key_field_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_INVALID;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid KBP_KEY_FIELD %d\n", kbp_key_field_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * API for retrieving regular and optimized AD DB result sizes.
 * Optimized KBP result is used with FWD tables only when result type with value 0 is present.
 * An optimized AD DB is used for it. The goal is to include only the relevant fields in order to improve capacity.
 * For all other result types a regular AD DB is used.
 */
static shr_error_e
kbp_mngr_result_info_get(
    int unit,
    const dbal_logical_table_t * table,
    uint32 *regular_result_size,
    uint32 *optimized_result_size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(regular_result_size, _SHR_E_INTERNAL, "regular_result_size");
    SHR_NULL_CHECK(optimized_result_size, _SHR_E_INTERNAL, "optimized_result_size");

    if (DNX_KBP_USE_OPTIMIZED_RESULT && DNX_KBP_IS_TABLE_USED_FOR_FWD(table->table_type)
        && SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        int result_type_index;

        *regular_result_size = 0;
        *optimized_result_size = 0;
        for (result_type_index = 0; result_type_index < table->nof_result_types; result_type_index++)
        {
            if (table->multi_res_info[result_type_index].result_type_hw_value[0] ==
                DNX_KBP_OPTIMIZED_RESULT_TYPE_HW_VALUE)
            {
                int result_field_index;
                multi_res_info_t *res_info = &table->multi_res_info[result_type_index];

                /*
                 * Optimized result was found.
                 * It's size should be all the fields that are not result type and padding.
                 */
                for (result_field_index = 0; result_field_index < res_info->nof_result_fields; result_field_index++)
                {
                    dbal_fields_e field_id = res_info->results_info[result_field_index].field_id;
                    if (!DNX_KBP_FIELD_OMMITTED_FROM_OPTIMIZED_RESULT(field_id))
                    {
                        *optimized_result_size += res_info->results_info[result_field_index].field_nof_bits;
                    }
                }
            }
            else
            {
                /** Regular result was found */
                *regular_result_size = table->max_payload_size;
            }
            if ((*regular_result_size != 0) && (*optimized_result_size != 0))
            {
                /** Optimized and regular result types found */
                break;
            }
        }
    }
    else
    {
        /** Optimized result is not enabled or table is not used for forwarding or no multiple result types */
        *regular_result_size = table->max_payload_size;
        *optimized_result_size = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Gets the DB key segments info related to a DB
 *   \param [in] unit - Relevant unit.
 *   \param [in] db_id - KBP DB ID.
 *   \param [in] index - Segment index to get.
 *   \param [out] segment_type - Returned segment type in KBP define; Returned if not NULL.
 *   \param [out] segment_size - Returned segment size in bytes; Returned if not NULL.
 *   \param [out] segment_name - Returned segment name; Returned if not NULL.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
shr_error_e
kbp_mngr_db_key_segment_get(
    int unit,
    uint32 db_id,
    int index,
    uint32 *segment_type,
    uint32 *segment_size,
    char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES])
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (segment_type == NULL && segment_size == NULL && segment_name == NULL)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_KEY_SEGMENT_INFO, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_ID, db_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_SEGMENT_INDEX, index);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (segment_name)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_SEGMENT_NAME, INST_SINGLE, (uint8 *) segment_name));
    }

    if (segment_size)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_SEGMENT_SIZE, INST_SINGLE, segment_size));
    }

    if (segment_type)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_SEGMENT_TYPE, INST_SINGLE, segment_type));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Sets the DB key segments info related to a DB
 *   \param [in] unit - Relevant unit.
 *   \param [in] db_id - KBP DB ID.
 *   \param [in] index - Segment index to get.
 *   \param [in] segment_type - Input segment type in KBP define; Set if not NULL.
 *   \param [in] segment_size - Input segment size in bytes; Set if not NULL.
 *   \param [in] segment_name - Input segment name; Set if not NULL.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
shr_error_e
kbp_mngr_db_key_segment_set(
    int unit,
    uint32 db_id,
    int index,
    uint32 *segment_type,
    uint32 *segment_size,
    char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES])
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (segment_type == NULL && segment_size == NULL && segment_name == NULL)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_KEY_SEGMENT_INFO, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_ID, db_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_SEGMENT_INDEX, index);

    if (segment_name)
    {
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_SEGMENT_NAME, INST_SINGLE,
                                        (uint8 *) segment_name);
    }

    if (segment_size)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEGMENT_SIZE, INST_SINGLE, *segment_size);
    }

    if (segment_type)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEGMENT_TYPE, INST_SINGLE, *segment_type);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Gets the master key segments info related to an opcode
 *   \param [in] unit - Relevant unit.
 *   \param [in] opcode_id - KBP opcode ID.
 *   \param [in] index - Segment index to get.
 *   \param [out] segment_type - Returned segment type in KBP define; Returned if not NULL.
 *   \param [out] segment_size - Returned segment size in bytes; Returned if not NULL.
 *   \param [out] segment_name - Returned segment name; Returned if not NULL.
 *   \param [out] overlay_offset - Returned segment overlay offset in bytes; Returned if not NULL.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
shr_error_e
kbp_mngr_master_key_segment_get(
    int unit,
    uint32 opcode_id,
    int index,
    uint32 *segment_type,
    uint32 *segment_size,
    char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES],
    uint32 *overlay_offset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (segment_type == NULL && segment_size == NULL && segment_name == NULL && overlay_offset == NULL)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_MASTER_KEY_SEGMENT_INFO, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_SEGMENT_INDEX, index);

    SHR_SET_CURRENT_ERR(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
    {
        /** Entry is not found. Return 0 for all requested fields. */
        if (segment_name)
        {
            segment_name[0] = '\0';
        }
        if (segment_size)
        {
            *segment_size = 0;
        }
        if (segment_type)
        {
            *segment_type = 0;
        }
        if (overlay_offset)
        {
            *overlay_offset = 0;
        }
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        SHR_EXIT();
    }

    if (segment_name)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_SEGMENT_NAME, INST_SINGLE, (uint8 *) segment_name));
    }

    if (segment_size)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_SEGMENT_SIZE, INST_SINGLE, segment_size));
    }

    if (segment_type)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_SEGMENT_TYPE, INST_SINGLE, segment_type));
    }

    if (overlay_offset)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_SEGMENT_OVERLAY_OFFSET, INST_SINGLE, overlay_offset));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Sets the master key segments info related to an opcode
 *   \param [in] unit - Relevant unit.
 *   \param [in] opcode_id - KBP opcode ID.
 *   \param [in] index - Segment index to get.
 *   \param [in] segment_type - Input segment type in KBP define; Set if not NULL.
 *   \param [in] segment_size - Input segment size in bytes; Set if not NULL.
 *   \param [in] segment_name - Input segment name; Set if not NULL.
 *   \param [in] overlay_offset - Input segment overlay offset in bytes; Set if not NULL.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
shr_error_e
kbp_mngr_master_key_segment_set(
    int unit,
    uint32 opcode_id,
    int index,
    uint32 *segment_type,
    uint32 *segment_size,
    char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES],
    uint32 *overlay_offset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (segment_type == NULL && segment_size == NULL && segment_name == NULL && overlay_offset == NULL)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_MASTER_KEY_SEGMENT_INFO, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_SEGMENT_INDEX, index);

    if (segment_name)
    {
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_SEGMENT_NAME, INST_SINGLE,
                                        (uint8 *) segment_name);
    }

    if (segment_size)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEGMENT_SIZE, INST_SINGLE, *segment_size);
    }

    if (segment_type)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEGMENT_TYPE, INST_SINGLE, *segment_type);
    }

    if (overlay_offset)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEGMENT_OVERLAY_OFFSET, INST_SINGLE,
                                     *overlay_offset);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Parse segments indices from CSV string to index array. Used in OPCODE XML parsing.
 */
static shr_error_e
kbp_mngr_parse_segments_to_array(
    int unit,
    char *segment_string,
    uint8 *segment_array,
    uint8 *nof_segments)
{
    char segment[2];
    char delimiter1 = ',';
    char delimiter2 = ' ';
    int str_len = 0;
    int char_pos = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(segment_string, _SHR_E_INTERNAL, "segment_string");
    SHR_NULL_CHECK(segment_array, _SHR_E_INTERNAL, "segment_array");
    SHR_NULL_CHECK(nof_segments, _SHR_E_INTERNAL, "nof_segments");

    *nof_segments = 0;
    str_len = sal_strlen(segment_string);
    for (char_pos = 0; char_pos < str_len; char_pos++)
    {
        if (segment_string[char_pos] == delimiter1 || segment_string[char_pos] == delimiter2)
        {
            continue;
        }
        else
        {
            segment[0] = segment_string[char_pos];
            segment[1] = 0;
            segment_array[(*nof_segments)] = sal_atoi(segment);
            (*nof_segments)++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   This API checks the table name and if split RPF is not enabled and the table name indicates RPF table,
*   the RPF table name is replaced with the name of the corresponding FWD table.
*
*   In non-split RPF KBP manager will disable the RPF table and instead use a clone of the FWD table for the RPF lookup.
*
*   \param [in/out] db_name - The KBP DB name.
*  \return
*    \retval None
*  \remark
*    None
*/
void
kbp_mngr_rpf_table_name_update(
    int unit,
    char db_name[DBAL_MAX_STRING_LENGTH])
{
    if (!kbp_mngr_split_rpf_in_use(unit))
    {
        if (!sal_strncmp(db_name, "KBP_IPV4_UNICAST_PRIVATE_LPM_RPF", DBAL_MAX_STRING_LENGTH))
        {
            sal_strncpy(db_name, "KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD", DBAL_MAX_STRING_LENGTH);
        }
        else if (!sal_strncmp(db_name, "KBP_IPV6_UNICAST_PRIVATE_LPM_RPF", DBAL_MAX_STRING_LENGTH))
        {
            sal_strncpy(db_name, "KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD", DBAL_MAX_STRING_LENGTH);
        }
    }
}

/**
* \brief
*   Maps FWD and ACL contexts to KBP opcode ID.
*   \param [in] unit - Relevant unit.
*   \param [in] fwd_context - Forwarding context (only from IFWD2 stage and only HW value).
*   \param [in] acl_context - ACL context.
*   \param [in] opcode_id - The KBP opcode ID used by the contexts.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e
kbp_mngr_context_to_opcode_set(
    int unit,
    uint8 fwd_context,
    uint8 acl_context,
    uint8 opcode_id)
{
    uint32 entry_handle_id;
    uint32 fwd_context_enum_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_FWD2_CONTEXT_ID, fwd_context, &fwd_context_enum_value));

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Updating opcode %s (%d) with context %s (%d) and ACL context %d\n"),
                                 dbal_enum_id_type_to_string(unit, DBAL_FIELD_TYPE_DEF_KBP_FWD_OPCODE, opcode_id),
                                 opcode_id,
                                 dbal_enum_id_type_to_string(unit, DBAL_FIELD_TYPE_DEF_FWD2_CONTEXT_ID,
                                                             fwd_context_enum_value), fwd_context, acl_context));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_MAPPING, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID, fwd_context_enum_value);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACL_CONTEXT, acl_context);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_OPCODE_ID, INST_SINGLE, opcode_id);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Validate that for specific fwc_ctx_id the opcode_id is configured as expected
* \param [in]  unit - The Unit number.
* \param [in]  fwd_ctx_id - fwd_context.
* \param [int] opcode_id - The expected opcode ID according to definitions (XML).
* \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
* \remark
*   None
* \see
*   shr_error_e
*/
static shr_error_e
kbp_mngr_fwd_context_to_opcode_hw_verify_update(
    int unit,
    int fwd_ctx_id,
    int acl_ctx_id,
    int opcode_id,
    uint8 is_standard_image)
{
    uint32 entry_handle_id;
    uint32 configured_opcode_id;
    uint32 lookup_bmp;
    uint32 fwd_ctx_id_enum_value;
    uint32 opcode_id_enum_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_MAPPING, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_FWD2_CONTEXT_ID, fwd_ctx_id, &fwd_ctx_id_enum_value));
    SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_KBP_FWD_OPCODE, opcode_id, &opcode_id_enum_value));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID, fwd_ctx_id_enum_value);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACL_CONTEXT, acl_ctx_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_KBP_OPCODE_ID, INST_SINGLE, &configured_opcode_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KBP_OPCODE_INFO, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, (uint32) opcode_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, &lookup_bmp);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Validate if the context is mapped to the expected opcode */
    if (opcode_id != configured_opcode_id)
    {
        /** Lookup bitmap indicates configured forwarding lookup */
        if (lookup_bmp && is_standard_image)
        {
            /** It is required for FWD contexts to be mapped correctly to opcodes with FWD lookups */
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "context %s (%d) configured to opcode %d expected to be configured to opcode %s (%d).\n",
                         dbal_enum_id_type_to_string(unit, DBAL_FIELD_TYPE_DEF_FWD2_CONTEXT_ID, fwd_ctx_id_enum_value),
                         fwd_ctx_id, configured_opcode_id,
                         dbal_enum_id_type_to_string(unit, DBAL_FIELD_TYPE_DEF_KBP_FWD_OPCODE, opcode_id_enum_value),
                         opcode_id);
        }
        /** Lookup bitmap indicates no forwarding lookups */
        else
        {
            /** FWD context has not been configured in pemla. Update it for opcodes with no FWD lookups */
            SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_set(unit, fwd_ctx_id, acl_ctx_id, opcode_id));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Validate the context attached to the opcode has KBP lookups configured in pemla.
* \param [in]  unit - The Unit number.
* \param [in]  context_index - The context to disable.
* \param [in]  opcode_id - The context to disable.
* \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
* \remark
*   None
* \see
*   shr_error_e
*/
static shr_error_e
kbp_mngr_context_lookups_validate(
    int unit,
    uint32 context_index,
    uint32 opcode_id)
{
    uint32 entry_handle_id;
    uint32 key_size[2] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, context_index);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_0);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE, &key_size[0]);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE, &key_size[1]);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if ((key_size[0] == 0) && (key_size[1] == 0))
    {
        uint32 fwd_ctx_id_enum_value;
        uint32 opcode_id_enum_value;

        SHR_IF_ERR_EXIT(dbal_fields_enum_value_get
                        (unit, DBAL_FIELD_FWD2_CONTEXT_ID, context_index, &fwd_ctx_id_enum_value));
        SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_KBP_FWD_OPCODE, opcode_id, &opcode_id_enum_value));

        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Context %s (%d) attached to opcode %s (%d) has no KBP lookups configured.\n",
                     dbal_enum_id_type_to_string(unit, DBAL_FIELD_TYPE_DEF_FWD2_CONTEXT_ID, fwd_ctx_id_enum_value),
                     context_index, dbal_enum_id_type_to_string(unit, DBAL_FIELD_TYPE_DEF_KBP_FWD_OPCODE,
                                                                opcode_id_enum_value), opcode_id);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Reads the contexts that related to this opcode from XML and verify that HW is configured according to it.
*   Disables FWD lookups in FLP for opcodes in ACL only mode.
*/
static shr_error_e
kbp_mngr_opcode_to_context_read(
    int unit,
    void *opcode_node,
    int opcode_id,
    uint8 disable_fwd,
    uint8 is_standard_image)
{
    void *iter_top;
    void *iter;
    uint32 entry_handle_id;
    uint32 fwd_ctx_enum_id[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE] = { 0 };
    uint32 acl_ctx_id[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE] = { 0 };
    int ctx_index = 0;
    int nof_ctxs = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    iter_top = dbx_xml_child_get_first(opcode_node, "FwdContexts");
    if (iter_top == NULL)
    {
        /** no context related to this opcode currently it is OK. need to think about it */
        SHR_EXIT();
    }

    RHDATA_ITERATOR(iter, iter_top, "Context")
    {
        char ctx_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };

        RHDATA_GET_STR_STOP(iter, "Name", ctx_name);

        /** Receive the ctx enum value */
        SHR_IF_ERR_EXIT(dbal_enum_type_string_to_id
                        (unit, DBAL_FIELD_TYPE_DEF_FWD2_CONTEXT_ID, ctx_name, &fwd_ctx_enum_id[ctx_index]));
        /*
         * Receive the context hw value.
         * For static/predefines contexts, the FWD context and the ACL context are identical.
         */
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                        (unit, DBAL_FIELD_FWD2_CONTEXT_ID, fwd_ctx_enum_id[ctx_index], &acl_ctx_id[ctx_index]));

        SHR_IF_ERR_EXIT(kbp_mngr_fwd_context_to_opcode_hw_verify_update
                        (unit, acl_ctx_id[ctx_index], acl_ctx_id[ctx_index], opcode_id, is_standard_image));

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "opcode %s (%d) related to context %s (%d) ACL context %d\n"),
                                     dbal_enum_id_type_to_string(unit, DBAL_FIELD_TYPE_DEF_KBP_FWD_OPCODE, opcode_id),
                                     opcode_id, ctx_name, acl_ctx_id[ctx_index], acl_ctx_id[ctx_index]));

        ctx_index++;
    }

    nof_ctxs = ctx_index;
    if (nof_ctxs > DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Opcode %d is mapped to %d contexts, which is more than the available %d\n",
                     opcode_id, nof_ctxs, DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);

    /** Update the opcode info with the forwarding contexts */
    for (ctx_index = 0; ctx_index < nof_ctxs; ctx_index++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_FWD_CONTEXT, ctx_index,
                                     fwd_ctx_enum_id[ctx_index]);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_OPCODE_FWD_CONTEXTS, INST_SINGLE, nof_ctxs);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (disable_fwd)
    {
        /** disables the pd order for FWD context in KBP in case of ACL only mode.
          * when disabled it means that the FWD lookups will not process in the FLP */
        for (ctx_index = 0; ctx_index < nof_ctxs; ctx_index++)
        {
            SHR_IF_ERR_EXIT(kbp_mngr_pd_order_disable(unit, fwd_ctx_enum_id[ctx_index]));
        }
    }
    else if (!SAL_BOOT_PLISIM)
    {
        /** Validate every context attached to this opcode has KBP lookups configured on real device */
        for (ctx_index = 0; ctx_index < nof_ctxs; ctx_index++)
        {
            SHR_IF_ERR_EXIT(kbp_mngr_context_lookups_validate(unit, acl_ctx_id[ctx_index], opcode_id));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Used to set the lookup info opcode.
 */
shr_error_e
kbp_mngr_opcode_lookup_set(
    int unit,
    uint8 opcode_id,
    uint8 lookup_id,
    uint8 lookup_type,
    uint8 lookup_db,
    uint8 result_id,
    uint8 nof_key_segments,
    uint8 key_segment_index[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP])
{
    uint32 entry_handle_id;
    uint32 used_segments_bmp = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Validate lookup segment indices */
    for (index = 0; index < nof_key_segments; index++)
    {
        if (key_segment_index[index] >= DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid key segment index %d\n", key_segment_index[index]);
        }
        if (utilex_bitstream_test_bit(&used_segments_bmp, key_segment_index[index]))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "found repeating key segment index %d\n", key_segment_index[index]);
        }
        SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&used_segments_bmp, key_segment_index[index]));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ID, lookup_id);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE, lookup_db);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_TYPE, INST_SINGLE, lookup_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_ID, INST_SINGLE, result_id);

    /** Set lookup segment indices */
    for (index = 0; index < nof_key_segments; index++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_SEGMENT_IDS, index,
                                     key_segment_index[index]);
    }
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, nof_key_segments);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * API for validating KBP opcode result size and table result type sizes are aligned.
 * A table returning response to an opcode result need to have all of its result type sizes equal to the opcode result size.
 * All KBP tables need to follow this restriction.
 * Unaligned table result sizes and opcode result sizes would lead to issues with traffic, entry management and diagnostics.
 * Optimized results are not validated.
 */
static shr_error_e
kbp_mngr_table_result_sizes_validate(
    int unit,
    const char *opcode_name,
    int res_size,
    uint8 res_index,
    const dbal_logical_table_t * table,
    dbal_tables_e dbal_table_id,
    uint8 is_standard_image)
{
    int res_type_index = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Skip validation for KBP RPF result when split RPF is not enabled in standard_1 and the table is IPv4/6 FWD */
    if (!kbp_mngr_split_rpf_in_use(unit) && /** RPF result index */ (res_index == 1) && is_standard_image &&
        ((dbal_table_id == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD) ||
         (dbal_table_id == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD)))
    {
        SHR_EXIT();
    }

    for (res_type_index = 0; res_type_index < table->nof_result_types; res_type_index++)
    {
        /** Skip validation for optimized result types */
        if (DNX_KBP_USE_OPTIMIZED_RESULT && SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE) &&
            (table->multi_res_info[res_type_index].result_type_hw_value[0] == DNX_KBP_OPTIMIZED_RESULT_TYPE_HW_VALUE))
        {
            continue;
        }

        if (table->multi_res_info[res_type_index].entry_payload_size != res_size)
        {
            if ((res_size - table->multi_res_info[res_type_index].entry_payload_size) < 0)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "\n\nOpcode \"%s\" has result %d configured to %d bits.\n"
                             "Table \"%s\" at result %d has result type %s configured to %d bits.\n"
                             "The KBP result could not fit the table result.\n",
                             opcode_name, res_index, res_size, table->table_name, res_index,
                             table->multi_res_info[res_type_index].result_type_name,
                             table->multi_res_info[res_type_index].entry_payload_size);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "\n\nOpcode \"%s\" has result %d configured to %d bits.\n"
                             "Table \"%s\" at result %d has result type \"%s\" configured to %d bits.\n"
                             "Unaligned results would lead to incorrect behavior.\n"
                             "Please add padding result field with size %d bits to the result type in order to align it to %d bits.\n",
                             opcode_name, res_index, res_size, table->table_name, res_index,
                             table->multi_res_info[res_type_index].result_type_name,
                             table->multi_res_info[res_type_index].entry_payload_size,
                             (res_size - table->multi_res_info[res_type_index].entry_payload_size), res_size);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Used to set the result info (offset and size) per opcode. The padding is set to 0 and will be updated in sync.
 */
shr_error_e
kbp_mngr_opcode_result_set(
    int unit,
    uint8 opcode_id,
    uint8 result_id,
    uint8 offset,
    uint8 size)
{
    uint32 entry_handle_id;
    uint8 is_valid;
    uint8 get_offset;
    uint8 get_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_RESULT_INFO, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_RESULT_ID, result_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &is_valid));

    if (is_valid)
    {
        /** Another lookup is added to this result; perform parameters validation */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Validating existing result %d for opcode %d, offset %d, size %d\n"), result_id,
                     opcode_id, offset, size));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_OFFSET, INST_SINGLE, &get_offset));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_SIZE, INST_SINGLE, &get_size));
        if ((offset != get_offset) || (size != get_size))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The new result parameters for result %d do not match the currently configured ones\nNew offset, size = %d, %d; Current offset, size = %d, %d\n",
                         result_id, offset, size, get_offset, get_size);
        }
    }
    else
    {
        /** Configure the result */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Configuring result %d for opcode %d, offset %d, size %d\n"), result_id,
                     opcode_id, offset, size));
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_RESULT_PADDING, INST_SINGLE, 0);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_RESULT_OFFSET, INST_SINGLE, offset);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_RESULT_SIZE, INST_SINGLE, size);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Trim the prefix from a key segment name for non-standard images.
*   Expecting the prefix to match the image name. Do nothing if image name is not found as prefix.
* \param [in]  unit - The Unit number.
* \param [in]  key_name - The key segment name.
* \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
* \remark
*   None
* \see
*   shr_error_e
*/
static shr_error_e
kbp_mngr_opcode_key_trim_image_name(
    int unit,
    char *key_name)
{
    char *image_name;
    int prefix_name_len;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_name, _SHR_E_INTERNAL, "key_name");

    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_get(unit, &image_name));

    prefix_name_len = sal_strnlen(image_name, DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES);

    /** Compare only the size of the image name */
    if (sal_strncasecmp(image_name, key_name, prefix_name_len) == 0)
    {
        int new_i = 0;
        /** Offset to the valid part of the name */
        int orig_i = prefix_name_len + 1;

        /** Perform trimming */
        for (; orig_i < DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES; new_i++, orig_i++)
        {
            key_name[new_i] = key_name[orig_i];
        }
        /** Fill the remaining with '\0' because it will be stored in dbal */
        while (new_i < DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES)
        {
            key_name[new_i++] = '\0';
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Parse the opcode valid indication from string. Expecting 0/1/DNXDATA.
*   Returns TRUE or FALSE.
* \param [in]  unit - The Unit number.
* \param [in]  data - The data string. Should be 0/1/DNXDATA.
* \param [out] valid - The valid indication.
* \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
* \remark
*   None
* \see
*   shr_error_e
*/
static shr_error_e
kbp_mngr_opcode_valid_parse(
    int unit,
    char *data,
    uint8 *valid)
{
    const uint32 *get_value = NULL;
    SHR_FUNC_INIT_VARS(unit);

    if (!sal_strncmp("0", data, 1))
    {
        *valid = FALSE;
    }
    else if (!sal_strncmp("1", data, 1))
    {
        *valid = TRUE;
    }
    else
    {
        get_value = dnxc_data_utils_generic_value_get(unit, data);
        if (get_value == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Parsing opcode valid indication \"%s\" cannot be resolved\n", data);
        }
        *valid = (uint8) *get_value;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Parse the information for a single opcode and update KBP_OPCODE_INFO according it.
*
*   In case of non-split RPF, this API performs the following updates to the opcode configuration:
*   1. RPF lookups will be done to the FWD tables instead of to the RPF tables.
*   2. The above change will cause KBP manager to create clone tables of the FWD tables for the RPF lookup.
*   3. RPF tables will not be used and will not be created.
*
* \param [in]  unit - The Unit number.
* \param [in]  opcode_node - The node to an opcode in the opcode catalogue.
* \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
* \remark
*   None
* \see
*   shr_error_e
*/
static shr_error_e
kbp_mngr_opcode_read(
    int unit,
    void *opcode_node,
    uint8 is_standard_image)
{
    void *iter_top;
    void *iter;
    int index;

    int opcode_id;
    char opcode_name[DBAL_MAX_STRING_LENGTH] = { 0 };
    char opcode_valid_str[DBAL_MAX_STRING_LENGTH] = { 0 };
    uint8 opcode_valid = FALSE;
    uint32 lookup_bmp = 0;
    uint32 result_bmp = 0;
    uint8 nof_public_lookups = 0;
    uint8 nof_opcodes;
    uint8 is_allocated;
    sw_state_htbl_init_info_t fwd_tcam_access_hash_info;

    int key_id[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
    int nof_keys = 0;

    uint32 entry_handle_id;
    uint32 lkp_entry_handle_id;

    /** Indicates that opcode DBs should be configured */
    uint8 disable_fwd_db = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));

    RHDATA_GET_STR_STOP(opcode_node, "Name", opcode_name);
    RHDATA_GET_INT_STOP(opcode_node, "OpCode", opcode_id);

    /** Get opcode valid indication and parse it to TRUE or FALSE */
    if ((!SAL_BOOT_PLISIM) || (dnx_data_elk.general.device_type_get(unit)))
    {
        RHDATA_GET_STR_STOP(opcode_node, "Valid", opcode_valid_str);
    }
    else
    {
        RHDATA_GET_STR_STOP(opcode_node, "ValidForAdapter", opcode_valid_str);
    }
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_valid_parse(unit, opcode_valid_str, &opcode_valid));

    /** Opcode is not valid. Disable FWD DBs configuration. */
    if (!opcode_valid)
    {
        disable_fwd_db = TRUE;
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_NAME, INST_ALL, (uint8 *) opcode_name);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Parse the related FWD contexts to this opcode and disable unused FWD lookups in FLP */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_to_context_read(unit, opcode_node, opcode_id, disable_fwd_db, is_standard_image));

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Opcode %s (%d) is %s\n"), opcode_name, opcode_id,
                                 (disable_fwd_db ? "configured for ACL only" : "enabled")));

    iter_top = dbx_xml_child_get_first(opcode_node, "MasterKey");
    if (iter_top == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find KBP opcode master key configuration (opcode %d)\n", opcode_id);
    }

    /** Parse master key */
    index = 0;
    RHDATA_ITERATOR(iter, iter_top, "Field")
    {
        int key_size = 0;
        char key_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };
        /** Set all segment types to EM */
        uint32 segment_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_EM;
        uint32 overlay_offset = BYTES2BITS(DNX_KBP_OVERLAY_FIELD_INVALID);

        RHDATA_GET_STR_STOP(iter, "Name", key_name);
        RHDATA_GET_INT_STOP(iter, "Segment", key_id[index]);
        RHDATA_GET_INT_STOP(iter, "BitSize", key_size);
        RHDATA_GET_INT_DEF(iter, "OverlayOffset", overlay_offset, BYTES2BITS(DNX_KBP_OVERLAY_FIELD_INVALID));

        /** Convert the size from bits to bytes */
        key_size = BITS2BYTES(key_size);
        overlay_offset = BITS2BYTES(overlay_offset);

        if (!is_standard_image)
        {
            /** Shorten the key name by removing the image name prefix */
            SHR_IF_ERR_EXIT(kbp_mngr_opcode_key_trim_image_name(unit, key_name));
        }

        /** Set the parsed master key segment */
        SHR_IF_ERR_EXIT(kbp_mngr_master_key_segment_set
                        (unit, opcode_id, index, &segment_type, (uint32 *) &key_size, key_name, &overlay_offset));
        index++;
    }
    nof_keys = index;

    if (nof_keys > DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "The number of key segments (%d) read from the opcodes definition XML exceeds the maximum available number of segments %d\n",
                     nof_keys, DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, nof_keys);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &lkp_entry_handle_id));

    /** In case Opcode's FWD functionality is disabled do not load FWd DB's, ACL only mode*/
    if (!disable_fwd_db)
    {
        uint32 res_size = 0;
        uint32 res_offset = 0;

        /** Parse used DBs */
        index = 0;
        RHDATA_ITERATOR(iter, opcode_node, "AppDB")
        {
            char res_index[16] = { 0 };
            char segments[16] = { 0 };
            int app_db_segment_index = 0;
            uint32 db_id = 0;
            uint8 lookup_type = DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FREE;
            char app_db_name[DBAL_MAX_STRING_LENGTH] = { 0 };
            uint8 app_db_result_index = 0;
            uint8 app_db_segment_id[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
            uint8 app_db_nof_segments = 0;
            dbal_tables_e dbal_table_id;
            const dbal_logical_table_t *table;

            RHDATA_GET_STR_STOP(iter, "Name", app_db_name);

            /*
             * The below API is performing manual update of the opcode RPF lookups.
             * In case of non-split RPF the relevant FWD table will be set to the RPF lookup.
             */
            kbp_mngr_rpf_table_name_update(unit, app_db_name);

            SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, app_db_name, &dbal_table_id));
            SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, dbal_table_id, &db_id, NULL));

            RHDATA_GET_STR_STOP(iter, "ResultLocation", res_index);
            SHR_IF_ERR_EXIT(kbp_mngr_valid_result_location_parse(unit, res_index, &app_db_result_index));

            SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

            /*
             * Handle lookup types and result sizes and offsets.
             * Size is set according to the table max payload size.
             * Offset is set according to the sum of the previous result sizes.
             */
            res_offset += res_size;

            if (is_standard_image && (app_db_result_index == DBAL_DEFINE_KBP_RESULT_LOCATION_RPF) &&
                ((dbal_table_id == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD) ||
                 (dbal_table_id == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF) ||
                 (dbal_table_id == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD) ||
                 (dbal_table_id == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF)))
            {
                lookup_type = DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_RPF;
                res_size = DNX_KBP_RESULT_SIZE_RPF;
            }
            else
            {
                lookup_type = DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD;
                SHR_IF_ERR_EXIT(dnx_kbp_valid_result_width(unit, table->max_payload_size, &res_size));
            }

            /** Validate table result sizes are aligned to the related opcode result size. */
            SHR_IF_ERR_EXIT(kbp_mngr_table_result_sizes_validate
                            (unit, opcode_name, res_size, app_db_result_index, table, dbal_table_id,
                             is_standard_image));

            RHDATA_GET_STR_STOP(iter, "Segments", segments);
            SHR_IF_ERR_EXIT(kbp_mngr_parse_segments_to_array(unit, segments, app_db_segment_id, &app_db_nof_segments));

            /** Get the AppDb segments info from the opcode (master key) and validate them */
            for (app_db_segment_index = 0; app_db_segment_index < app_db_nof_segments; app_db_segment_index++)
            {
                int key_index = 0;
                /** Find the segment in the opcode (master key) */
                for (key_index = 0; key_index < nof_keys; key_index++)
                {
                    if (key_id[key_index] == app_db_segment_id[app_db_segment_index])
                    {
                        /** Master key segment found */
                        break;
                    }
                }
                if (key_index >= nof_keys)
                {
                    /** Non-existing segment index */
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "AppDb %s on search %d has a segment index %d that does not exist in opcode (%d) %s\n",
                                 app_db_name, index, app_db_segment_id[app_db_segment_index], opcode_id, opcode_name);
                }
            }

            dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
            dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_ID, (uint8) index);

            if (utilex_bitstream_test_bit(&lookup_bmp, index))
            {
                /** If we have a second lookup on the same index, we have a public lookup */
                lookup_type = (lookup_type == DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD) ?
                    DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD_PUBLIC : DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_RPF_PUBLIC;
            }
            else
            {
                utilex_bitstream_set_bit(&lookup_bmp, index);
            }

            utilex_bitstream_set_bit(&result_bmp, app_db_result_index);

            /** Update lookup info */
            SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_set
                            (unit, opcode_id, (uint8) index, lookup_type, db_id, app_db_result_index,
                             app_db_nof_segments, app_db_segment_id));

            /** Update result info */
            SHR_IF_ERR_EXIT(kbp_mngr_opcode_result_set(unit, opcode_id, app_db_result_index, res_offset, res_size));

            index++;
        }
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, lookup_bmp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, result_bmp);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_NOF_PUBLIC_LOOKUPS, INST_SINGLE, nof_public_lookups);

    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Get the number of valid opcodes */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KBP_GENERAL_INFO, entry_handle_id));
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_NOF_OPCODES, INST_SINGLE, &nof_opcodes);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** updating the number of OPCODES in the KBP */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_OPCODES, INST_SINGLE, nof_opcodes + 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Initialize IPv4/6 MC for internal storing of access IDs */
    SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.is_init(unit, &is_allocated));
    if (!is_allocated)
    {
        SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.init(unit));
    }
    if (opcode_id == DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV4_MULTICAST_PRIVATE_W_UC_RPF)
    {
        sal_memset(&fwd_tcam_access_hash_info, 0, sizeof(fwd_tcam_access_hash_info));
        fwd_tcam_access_hash_info.expected_nof_elements =
            KBP_FWD_TCAM_ACCESS_HASH_TABLE_SIZE_CONVERT(dnx_data_elk.application.ipv4_mc_hash_table_size_get(unit));
        fwd_tcam_access_hash_info.max_nof_elements =
            KBP_FWD_TCAM_ACCESS_HASH_TABLE_SIZE_CONVERT(dnx_data_elk.application.ipv4_mc_hash_table_size_get(unit));
        fwd_tcam_access_hash_info.hash_function = NULL;
        fwd_tcam_access_hash_info.print_cb_name = "kbp_ipv4_fwd_tcam_access_mapping_htb_entry_print";

        SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.key_2_entry_id_hash.create(unit, DNX_KBP_TCAM_HASH_TABLE_INDEX_IPV4_MC,
                                                                       &fwd_tcam_access_hash_info));
    }
    if (opcode_id == DBAL_ENUM_FVAL_KBP_FWD_OPCODE_IPV6_MULTICAST_PRIVATE_W_UC_RPF)
    {
        sal_memset(&fwd_tcam_access_hash_info, 0, sizeof(fwd_tcam_access_hash_info));
        fwd_tcam_access_hash_info.expected_nof_elements =
            KBP_FWD_TCAM_ACCESS_HASH_TABLE_SIZE_CONVERT(dnx_data_elk.application.ipv6_mc_hash_table_size_get(unit));
        fwd_tcam_access_hash_info.max_nof_elements =
            KBP_FWD_TCAM_ACCESS_HASH_TABLE_SIZE_CONVERT(dnx_data_elk.application.ipv6_mc_hash_table_size_get(unit));
        fwd_tcam_access_hash_info.hash_function = NULL;
        fwd_tcam_access_hash_info.print_cb_name = "kbp_ipv6_fwd_tcam_access_mapping_htb_entry_print";

        SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.key_2_entry_id_hash.create(unit, DNX_KBP_TCAM_HASH_TABLE_INDEX_IPV6_MC,
                                                                       &fwd_tcam_access_hash_info));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Update the padding of the results of a certain opcode.
*   whenever a result ID does not begin after the previous one has ended, we need to pad the result ID
*   to make them continuous.
*   If we have a hole between the result IDs (dummy result ID) we use it for padding, giving every hole
*   at least one byte of padding.
*   \param [in] unit - Relevant unit.
*   \param [in] opcode_id - The opcode_id for which we calculate the padding
*   \param [in] result_bmp - The bitmap of all result types used
*                            (all ones mut start from the first bit and be consecutive).
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    Intended to be called from kbp_mngr_opcode_info_update.
*  \see
*    shr_error_e
*/
static shr_error_e
kbp_mngr_opcode_info_update_result_padding_calc(
    int unit,
    uint8 opcode_id,
    uint32 result_bmp)
{
    uint32 res_entry_handle_id;
    uint8 result_id;
    uint8 result_size[DNX_KBP_MAX_NOF_RESULTS] = { 0 };
    uint8 result_offset[DNX_KBP_MAX_NOF_RESULTS] = { 0 };
    uint32 buffer_size = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_RESULT_INFO, &res_entry_handle_id));

    /** Get resutl size and offset */
    for (result_id = 0; result_id < DNX_KBP_MAX_NOF_RESULTS; result_id++)
    {
        if (utilex_bitstream_test_bit(&result_bmp, result_id))
        {
            dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
            dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_RESULT_ID, result_id);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, res_entry_handle_id, DBAL_GET_ALL_FIELDS));

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, res_entry_handle_id, DBAL_FIELD_RESULT_OFFSET, INST_SINGLE,
                             &result_offset[result_id]));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, res_entry_handle_id, DBAL_FIELD_RESULT_SIZE, INST_SINGLE, &result_size[result_id]));

        }
    }

    /** Calculate paddings */
    for (result_id = 0; result_id < DNX_KBP_MAX_NOF_RESULTS; result_id++)
    {
        if (utilex_bitstream_test_bit(&result_bmp, result_id))
        {
            int padding = 0;

            /*
             * If the result has zero size, we pad it with extra offset we may have from a future result.
             * We assume zero size means that the result is a dummy result.
             * If we have multiple consecutive dummy results we give all but the last 1 byte padding (minimum)
             */
            if ((result_size[result_id] == 0) && (result_id != (DNX_KBP_MAX_NOF_RESULTS - 1)))
            {
                if (utilex_bitstream_test_bit(&result_bmp, result_id + 1) == 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error, dummy result for opcode %d at the end of the bitmap. "
                                 "last result %d, result bitmap 0x%x\n", opcode_id, result_id, result_bmp);
                }
                if (result_size[result_id + 1] == 0)
                {
                    padding = BYTES2BITS(DNX_KBP_RESULT_SIZE_MIN_BYTES);
                }
                else
                {
                    padding = result_offset[result_id + 1] - buffer_size;
                }
                if (padding <= 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Error, not enough space for padding dummy results before "
                                 "result_id %d in OPCODE ID %d (offset %d)\n",
                                 result_id, opcode_id, result_offset[result_id + 1]);
                }
                /*
                 * result_size + padding form the result length of a single result (in this case result_size is 0).
                 * The result length cannot be bigger than 128b.
                 * The excess padding will be moved to the next result.
                 */
                if (padding > DNX_KBP_MAX_RESULT_DATA_LEN)
                {
                    padding = DNX_KBP_MAX_RESULT_DATA_LEN;
                }
                buffer_size += padding;
            }
            else
            {
                /*
                 * Calculate the required result padding according to the difference
                 * in the offset and size between results
                 */
                padding = result_offset[result_id] - buffer_size;
                if (padding < 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Error, not enough offset for result_id %d in opcode_id %d. "
                                 "Offset %d, previous result_id ends in %d.\n",
                                 result_id, opcode_id, result_offset[result_id], buffer_size);
                }
                /*
                 * result_size + padding form the result length of a single result.
                 * The result length cannot be bigger than 128b.
                 */
                if ((padding + result_size[result_id]) > DNX_KBP_MAX_RESULT_DATA_LEN)
                {
                    if (result_id == 0)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, cannot pad result_id %d in opcode_id %d enough. "
                                     "Offset %d, previous result_id ends in %d. Missing %d bits. "
                                     "LSB offset of result_id 0 must be at least %d.\n",
                                     result_id, opcode_id, result_offset[result_id], buffer_size,
                                     (padding + result_size[result_id] - DNX_KBP_MAX_RESULT_DATA_LEN),
                                     (dnx_data_field.kbp.max_payload_size_per_opcode_get(unit) -
                                      DNX_KBP_MAX_RESULT_DATA_LEN));
                    }
                    else if (result_size[result_id - 1] != 0)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, cannot pad result_id %d in opcode_id %d enough. "
                                     "Offset %d, previous result_id ends in %d. Missing %d bits. "
                                     "Difference between the LSB offsets of consecutive results can be at most %d.\n",
                                     result_id, opcode_id, result_offset[result_id], buffer_size,
                                     (padding + result_size[result_id] - DNX_KBP_MAX_RESULT_DATA_LEN),
                                     DNX_KBP_MAX_RESULT_DATA_LEN);
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error, cannot pad result_id %d in opcode_id %d enough. "
                                     "Offset %d, previous result_id ends in %d. Missing %d bits.\n",
                                     result_id, opcode_id, result_offset[result_id], buffer_size,
                                     (padding + result_size[result_id] - DNX_KBP_MAX_RESULT_DATA_LEN));
                    }

                }
                buffer_size += padding + result_size[result_id];
            }

            if (padding < 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error, reached negative padding for opcode_id %d, reuslt_id %d, "
                             "offset %d, padding %d\n", opcode_id, result_id, result_offset[result_id], padding);
            }

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Update result %d padding for opcode %d to %d\n"), result_id, opcode_id,
                         padding));

            dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
            dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_RESULT_ID, result_id);

            dbal_entry_value_field8_set(unit, res_entry_handle_id, DBAL_FIELD_RESULT_PADDING, INST_SINGLE, padding);
            dbal_entry_value_field8_set(unit, res_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, res_entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Adds a dummy_lookup to an opcode that has no lookups.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] opcod_id - Handle to the DB or OPCODE table.
 *   \param [in] acl_conmtext - Handle to the DB or OPCODE table.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_opcode_without_lookup_add_dummy_lookup(
    int unit,
    uint32 opcode_id,
    uint8 acl_context)
{
    uint32 entry_handle_id_opcode;
    uint32 lkp_entry_handle_id;
    uint32 result_entry_handle_id;
    uint32 aligner_entry_handle_id;
    uint32 kbr_entry_handle_id;
    uint32 result_bmp;
    uint32 lookup_bmp;
    dbal_enum_value_field_kbp_kbr_idx_e kbp_kbr_kbp = DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_0;
    dbal_enum_value_field_field_key_e kbp_kbr_fwd;
    int lookup_result_id = 0;
    uint32 result_size_bytes = 1;
    uint32 aligner_size_get;
    kbp_mngr_key_segment_t key_segments;
    uint32 ffc_bitmap[2] = { 0 };
    uint32 dummy_db_id = DNX_KBP_DUMMY_HOLE_DB_ID;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(kbp_mngr_key_segment_t_init(unit, &key_segments));
    key_segments.nof_bytes = result_size_bytes;
    sal_strncpy_s(key_segments.name, "ZERO", sizeof(key_segments.name));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id_opcode));
    dbal_entry_key_field8_set(unit, entry_handle_id_opcode, DBAL_FIELD_OPCODE_ID, opcode_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_opcode, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id_opcode, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, &result_bmp));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id_opcode, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, &lookup_bmp));
    if ((result_bmp != 0) || (lookup_bmp != 0))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Opcode %d shouldn't have any lookups, but lookup bitmap 0x%x, result bitmap 0x%x.\n",
                     opcode_id, lookup_bmp, result_bmp);
    }
    /** Get the first KBP key.*/
    SHR_IF_ERR_EXIT(dnx_field_key_acl_to_fwd2_key_id_convert(unit, kbp_kbr_kbp, &kbp_kbr_fwd));

    /*
     * Set the dummy lookup.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id_opcode, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE,
                                 0x1 << lookup_result_id);
    dbal_entry_value_field32_set(unit, entry_handle_id_opcode, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE,
                                 0x1 << lookup_result_id);
    dbal_entry_value_field8_set(unit, entry_handle_id_opcode, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &lkp_entry_handle_id));
    dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_ID, lookup_result_id);
    dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
    dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE, dummy_db_id);
    dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_RESULT_ID, INST_SINGLE, lookup_result_id);
    dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_TYPE, INST_SINGLE,
                                DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_ACL);
    dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, lkp_entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_RESULT_INFO, &result_entry_handle_id));
    dbal_entry_key_field8_set(unit, result_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    dbal_entry_key_field8_set(unit, result_entry_handle_id, DBAL_FIELD_RESULT_ID, lookup_result_id);
    dbal_entry_value_field8_set(unit, result_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
    dbal_entry_value_field8_set(unit, result_entry_handle_id, DBAL_FIELD_RESULT_SIZE, INST_SINGLE,
                                BYTES2BITS(result_size_bytes));
    dbal_entry_value_field8_set(unit, result_entry_handle_id, DBAL_FIELD_RESULT_PADDING, INST_SINGLE, 0);
    dbal_entry_value_field8_set(unit, result_entry_handle_id, DBAL_FIELD_RESULT_OFFSET, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, result_entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_opcode, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segments_add_internal
                    (unit, opcode_id, DNX_KBP_INVALID_SEGMENT_ID, 1, &key_segments));

    /*
     * Set the aligner.
     * Note we set the aligner to the 0 key, which we never set for ACL at the moment.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING, &aligner_entry_handle_id));
    dbal_entry_key_field32_set(unit, aligner_entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, acl_context);
    dbal_entry_key_field32_set(unit, aligner_entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, kbp_kbr_kbp);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, aligner_entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, aligner_entry_handle_id, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE, &aligner_size_get));
    if (aligner_size_get != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Opcode %d has no lookup, but first key aligner size isn't zero (%d).\n",
                     opcode_id, aligner_size_get);
    }
    dbal_entry_value_field32_set(unit, aligner_entry_handle_id, DBAL_FIELD_ACL_KEY_SIZE, INST_SINGLE,
                                 result_size_bytes);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, aligner_entry_handle_id, DBAL_COMMIT));

    /*
     * Add one FFC to the KBR.
     * We don't care about the content of the FFC.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_KBR_INFO, &kbr_entry_handle_id));
    dbal_entry_key_field32_set(unit, kbr_entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, acl_context);
    dbal_entry_key_field32_set(unit, kbr_entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, kbp_kbr_fwd);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, kbr_entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, kbr_entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap));
    ffc_bitmap[0] |= 1;
    dbal_entry_value_field_arr32_set(unit, kbr_entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, kbr_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Takes an opcode. for predefined (static) opcodes, looks at the user defined (dynamic) opcodes that are based
 *   on it as a group together with a predefined opcode.
 *   If at lease one the the opcodes in the group uses a KBP lookup, sets ACL_RESOLUTION_ENABLE to true for all of the
 *   fwd contexts.
 *   If some of the contexts in the group use KBP lookups but some don't, those that don't are added with a dummy lookup.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] opcod_id - Handle to the DB or OPCODE table. Only handles predefined opcodes.
 *   \param [out] dummy_lookup_added - Returns if we added a dummy lookup to an opcode.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_opcode_acl_resolution_enable_update(
    int unit,
    uint32 opcode_id,
    uint8 *dummy_lookup_added)
{
    int all_opcodes_have_lookups = TRUE;
    int no_opcode_has_lookup = TRUE;
    int mapping_found = FALSE;
    uint8 opcode_is_dynamic;
    uint8 fwd_nof_contexts;
    kbp_mngr_fwd_acl_context_mapping_t fwd_acl_ctx_mapping[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    int fwd_ctx_index;
    int acl_context;
    uint8 derived_opcode;
    uint32 entry_handle_id;
    uint32 entry_handle_id_enabler;
    uint32 result_bmp;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *dummy_lookup_added = FALSE;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_is_dynamic(unit, opcode_id, &opcode_is_dynamic));
    if (opcode_is_dynamic)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Opcode %d is dynamic, this function only takes static opcodes.\n", opcode_id);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));

    /*
     * Use the opcode to context mapping and the context to opcode mapping to get all of the derived opcodes
     * and check if they all use KBP lookup, they all don't use KBP lookups or some do and some don't.
     */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_to_contexts_get(unit, opcode_id, &fwd_nof_contexts, fwd_acl_ctx_mapping));
    for (fwd_ctx_index = 0; fwd_ctx_index < fwd_nof_contexts; fwd_ctx_index++)
    {
        uint32 fwd_context = fwd_acl_ctx_mapping[fwd_ctx_index].fwd_context;
        for (acl_context = 0; acl_context < DNX_KBP_NOF_FWD_CONTEXTS; acl_context++)
        {
            rv = kbp_mngr_context_to_opcode_translate(unit, fwd_context, acl_context, &derived_opcode);
            if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_IF_ERR_EXIT(rv);
                if (derived_opcode < opcode_id)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Opcode %d is static and connected to opcode %d, but expecting all "
                                 "dynamic opcodes to be larger then static opcodes. "
                                 "FWD_context %d, ACL context %d.\n",
                                 opcode_id, derived_opcode, fwd_context, acl_context);
                }
                SHR_IF_ERR_EXIT(kbp_mngr_opcode_is_dynamic(unit, derived_opcode, &opcode_is_dynamic));
                if ((!opcode_is_dynamic) && (derived_opcode != opcode_id))
                {

                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Opcode %d is static and yet it is derived from another opcode %d."
                                 "fwd_context %d acl_context %d.\n",
                                 derived_opcode, opcode_id, fwd_context, acl_context);
                }
                dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, derived_opcode);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, &result_bmp));
                if (result_bmp == 0)
                {
                    all_opcodes_have_lookups = FALSE;
                }
                else
                {
                    no_opcode_has_lookup = FALSE;
                }
                mapping_found = TRUE;
            }
        }
    }

    if (!mapping_found)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Opcode %d has no fwd context mapped to a valid opcode.\n", opcode_id);
    }
    if (all_opcodes_have_lookups && no_opcode_has_lookup)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Opcode %d illogical result reached.\n", opcode_id);
    }

    /*
     * If some opcodes have KBP lookups and others don't, provide those that don't with a dummy lookup.
     */
    if (!all_opcodes_have_lookups && !no_opcode_has_lookup)
    {

        for (fwd_ctx_index = 0; fwd_ctx_index < fwd_nof_contexts; fwd_ctx_index++)
        {
            uint32 fwd_context = fwd_acl_ctx_mapping[fwd_ctx_index].fwd_context;
            for (acl_context = 0; acl_context < DNX_KBP_NOF_FWD_CONTEXTS; acl_context++)
            {
                rv = kbp_mngr_context_to_opcode_translate(unit, fwd_context, acl_context, &derived_opcode);
                if (rv != _SHR_E_NOT_FOUND)
                {
                    SHR_IF_ERR_EXIT(rv);
                    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, derived_opcode);
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, &result_bmp));
                    if (result_bmp == 0)
                    {
                        /*
                         * Set A new lookup and set a byte to the aligner.
                         */
                        SHR_IF_ERR_EXIT(kbp_mngr_opcode_without_lookup_add_dummy_lookup
                                        (unit, derived_opcode, acl_context));
                        *dummy_lookup_added = TRUE;
                    }
                }
            }
        }
        if (*dummy_lookup_added == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Opcode without KBP lookup found in the first search but not the second.\n");
        }
    }

    /*
     * If at least one opcode has KBP lookup, set ACL_RESOLUTION_ENABLE to TRUE for all of the FWD contexts.
     */
    if (!no_opcode_has_lookup)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INGRESS_IRPP_FWD2_CONTEXT_PROPERTIES, &entry_handle_id_enabler));
        for (fwd_ctx_index = 0; fwd_ctx_index < fwd_nof_contexts; fwd_ctx_index++)
        {
            uint32 fwd_context = fwd_acl_ctx_mapping[fwd_ctx_index].fwd_context;
            uint32 fwd_context_enum_value;

            SHR_IF_ERR_EXIT(dbal_fields_enum_value_get
                            (unit, DBAL_FIELD_FWD2_CONTEXT_ID, fwd_context, &fwd_context_enum_value));
            dbal_entry_key_field32_set(unit, entry_handle_id_enabler, DBAL_FIELD_FWD2_CONTEXT_ID,
                                       fwd_context_enum_value);
            dbal_entry_value_field32_set(unit, entry_handle_id_enabler, DBAL_FIELD_ACL_RESOLUTION_ENABLE, INST_SINGLE,
                                         TRUE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_enabler, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Sets ACL_RESULUTION_ENABLE according to whether or not the opcode uses lookup.
 *   \param [in] unit - Relevant unit.
 *   \param [out] dummy_lookup_added - Returns if we added a dummy lookup to an opcode.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_opcode_all_acl_resolution_enable_update(
    int unit,
    uint8 *dummy_lookup_added)
{
    uint32 entry_handle_id;
    uint8 dummy_lookup_added_int;
    uint8 is_valid;
    int is_end;
    uint8 opcode_id;
    uint8 opcode_is_dynamic;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *dummy_lookup_added = FALSE;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                     &is_valid));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, &opcode_id));
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_is_dynamic(unit, opcode_id, &opcode_is_dynamic));
        /*
         * We call the check for static opcodes only, as for each static opcode,
         * we also handle the derived dynamic opcodes.
         */
        if (!opcode_is_dynamic)
        {
            SHR_IF_ERR_EXIT(kbp_mngr_opcode_acl_resolution_enable_update(unit, opcode_id, &dummy_lookup_added_int));
            if (dummy_lookup_added_int)
            {
                *dummy_lookup_added = TRUE;
            }
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Updates all valid opcodes lookups and results and introduce dummy lookups
 *   \param [in] unit - Relevant unit.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_opcode_info_update(
    int unit,
    uint8 *is_dummy_needed)
{
    uint32 entry_handle_id;
    uint32 lkp_entry_handle_id;
    uint32 db_entry_handle_id;
    int is_end;
    int index;
    uint8 nof_db_segments;
    uint8 is_valid = TRUE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *is_dummy_needed = FALSE;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &lkp_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &db_entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                     &is_valid));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint8 opcode_id;
        uint8 result_id;
        uint8 db_id;
        uint32 lookup_bmp = 0;
        uint32 result_bmp = 0;
        uint8 nof_public_lookups;

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, &opcode_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, &lookup_bmp));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, &result_bmp));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_NOF_PUBLIC_LOOKUPS, INST_SINGLE, &nof_public_lookups));

        /** Iterate over all results and find holes in them */
        for (result_id = 0; result_id < DNX_KBP_MAX_NOF_RESULTS - nof_public_lookups; result_id++)
        {
            uint8 lookup_id = result_id + nof_public_lookups;

            /** Check for invalid lookup */
            if (!utilex_bitstream_test_bit(&result_bmp, result_id))
            {
                /** Hole was found; create dummy lookup for it */
                if (utilex_bitstream_have_one_in_range(&result_bmp, result_id + 1, DNX_KBP_MAX_NOF_RESULTS - 1))
                {
                    dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
                    dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_ID, lookup_id);

                    dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
                    dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE,
                                                DNX_KBP_DUMMY_HOLE_DB_ID);
                    dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_RESULT_ID, INST_SINGLE,
                                                result_id);
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, lkp_entry_handle_id, DBAL_COMMIT));

                    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&lookup_bmp, lookup_id));
                    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&result_bmp, result_id));

                    *is_dummy_needed = TRUE;
                }
            }
            else
            {
                /*
                 * Lookup is valid
                 * Update the opcode master key field types
                 * Only do so for range fields
                 */
                dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
                dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_ID, lookup_id);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, lkp_entry_handle_id, DBAL_GET_ALL_FIELDS));

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE, &db_id));

                dbal_entry_key_field8_set(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, db_id);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, db_entry_handle_id, DBAL_GET_ALL_FIELDS));

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                (unit, db_entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE,
                                 &nof_db_segments));
                for (index = 0; index < nof_db_segments; index++)
                {
                    uint32 db_key_type;
                    uint32 db_segment_index;
                    char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };

                    /** Get the DB key segment type */
                    SHR_IF_ERR_EXIT(kbp_mngr_db_key_segment_get(unit, db_id, index, &db_key_type, NULL, segment_name));
                    if (db_key_type == DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_RANGE)
                    {
                        /** Get the DB key segment index that is mapped to the master key segments */
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                        (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_SEGMENT_IDS, index,
                                         &db_segment_index));

                        LOG_VERBOSE(BSL_LOG_MODULE,
                                    (BSL_META_U(unit, "Update opcode %d master key segment %d, name %s from DB %d\n"),
                                     opcode_id, db_segment_index, segment_name, db_id));

                        /** Update the master key field type */
                        SHR_IF_ERR_EXIT(kbp_mngr_master_key_segment_set
                                        (unit, opcode_id, db_segment_index, &db_key_type, NULL, NULL, NULL));
                    }
                }
            }
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, lookup_bmp);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, result_bmp);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Update padding
         */
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_info_update_result_padding_calc(unit, opcode_id, result_bmp));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validates the number of lookups per each opcode.
 *   \param [in] unit - Relevant unit.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_opcode_lookups_validate(
    int unit)
{
    uint32 entry_handle_id;
    uint32 lkp_entry_handle_id;
    uint8 is_valid = TRUE;
    uint8 error = FALSE;
    uint32 xl_db;
    int is_end;
    int nof_valid_lookups = DNX_KBP_MAX_NOF_LOOKUPS;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Look for XL_DBs */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                     &is_valid));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_XL_DB, INST_SINGLE, &xl_db));

        if (xl_db)
        {
            nof_valid_lookups = MAX_NUMBER_OF_LOOKUPS_WHEN_XL_DB_EXISTS;
            break;
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    if (xl_db)
    {
        /** Validate opcode lookups */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KBP_OPCODE_INFO, entry_handle_id));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &lkp_entry_handle_id));

        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                         &is_valid));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

        while (!is_end)
        {
            uint32 lookup_bmp = 0;
            int nof_lookups = 0;

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, &lookup_bmp));

            /*
             * The expectation is that the lookup_bmp is already updated with the dummy lookups.
             * There should be no holes inside the lookup_bmp.
             */
            nof_lookups = utilex_bitstream_get_nof_on_bits(&lookup_bmp, 1);
            /*
             * When XL_DBs are in use, all opcodes are limited to maximum 6 lookups.
             * Only print error message and continue checking the other opcodes.
             * Return error when all opcodes are checked.
             */
            if (nof_lookups > nof_valid_lookups)
            {
                uint32 opcode_id = 0;
                char opcode_name[DBAL_MAX_STRING_LENGTH] = { 0 };

                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, &opcode_id));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                                (unit, entry_handle_id, DBAL_FIELD_OPCODE_NAME, INST_SINGLE, (uint8 *) opcode_name));

                LOG_CLI((BSL_META("Opcode %d %s has %d lookups and only %d are available when XL_DBs are in use\n"),
                         opcode_id, opcode_name, nof_lookups, MAX_NUMBER_OF_LOOKUPS_WHEN_XL_DB_EXISTS));
                error = TRUE;
            }

            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }

        if (error)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Opcodes number of lookups validation failed\n");
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * setting the OPCODE master key in KBP according to table definitions
 *   \param [in] unit - Relevant unit.
 *   \param [in] entry_handle_id - entry handle ID.
 *   \param [in] key - kbp key pointer
 *   \param [in] nof_segments - number of segments.
 *   \param [in] instruction_id - relevant instruction id.
 *   \param [in/out] master_key_nof_bytes - number of bytes in master key.
 *   \param [in] is_overlay_field - indication whether to add the segment as master key or as an overlay.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_set_master_key(
    int unit,
    uint32 opcode_id,
    kbp_key_t_p key,
    uint32 nof_segments,
    uint32 instruction_id,
    uint32 *master_key_nof_bytes,
    uint8 is_overlay_field)
{
    int index;

    SHR_FUNC_INIT_VARS(unit);

    if (master_key_nof_bytes)
    {
        *master_key_nof_bytes = 0;
    }
    for (index = 0; index < nof_segments; index++)
    {
        uint32 overlay_offset;
        uint32 field_nof_bytes;
        char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };
        int kbp_segment_type;

        SHR_IF_ERR_EXIT(kbp_mngr_master_key_segment_get
                        (unit, opcode_id, index, (uint32 *) &kbp_segment_type, &field_nof_bytes, segment_name,
                         &overlay_offset));

        if (!is_overlay_field && !DNX_KBP_KEY_IS_OVERLAY_FIELD(overlay_offset))
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Adding master key segment %d, name %s, size %d type %d\n"), index,
                         segment_name, field_nof_bytes, kbp_segment_type));

            SHR_IF_ERR_EXIT(kbp_mngr_dbal_key_field_type_to_kbp_key_field_type(unit, kbp_segment_type,
                                                                               &kbp_segment_type));
            DNX_KBP_TRY_PRINT(kbp_key_add_field(key, segment_name, BYTES2BITS(field_nof_bytes), kbp_segment_type),
                              "Instruction %d", instruction_id);

            *master_key_nof_bytes += field_nof_bytes;
        }
        else if (is_overlay_field && DNX_KBP_KEY_IS_OVERLAY_FIELD(overlay_offset))
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Adding overlay field %d, name %s, size %d type %d offset %d\n"), index,
                         segment_name, field_nof_bytes, kbp_segment_type, overlay_offset));

            SHR_IF_ERR_EXIT(kbp_mngr_dbal_key_field_type_to_kbp_key_field_type(unit, kbp_segment_type,
                                                                               &kbp_segment_type));

            DNX_KBP_TRY_PRINT(kbp_key_overlay_field(key, segment_name, BYTES2BITS(field_nof_bytes), kbp_segment_type,
                                                    BYTES2BITS(overlay_offset)), "Instruction %d", instruction_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Sync all Valid LTRs (opcodes) to KBP device.
 *   Each entry in KBP_OPCODE_INFO contains all information for creating KBP LTR.
 *   Create a KBP instruction for all valid entries in KBP_OPCODE_INFO and add the lookup DBs to that instruction.
 *   \param [in] unit - Relevant unit.
 *   \param [in] smt_id - KBP thread ID 0 or 1. In case SMT is not enabled, this parameter is irrelevant.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_opcode_sync(
    int unit,
    uint32 smt_id)
{
    int retval = 0;
    uint32 entry_handle_id;
    uint32 lkp_entry_handle_id;
    uint32 res_entry_handle_id;
    uint32 db_entry_handle_id;
    uint32 lookup_bmp = 0;
    uint32 result_bmp = 0;
    int is_end;
    int stat_enable;
    uint8 is_valid = TRUE;
    struct kbp_device *device = NULL;
    uint8 kbp_context_fwd_master_key_valid[DNX_KBP_FWD_CONTEXT_LAST] = { 0 };
    uint8 context_index;
    uint8 nof_valid_opcodes = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (kbp_mngr_smt_enabled(unit))
    {
        if (smt_id >= DNX_KBP_MAX_NOF_SMTS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid SMT ID %d\n", smt_id);
        }
        device = Kbp_app_data[unit]->smt_p[DNX_KBP_INST_MASTER][smt_id];
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\nCreating instructions for thread %d\n"), smt_id));
    }
    else
    {
        /** OP compatibility */
        smt_id = 0;
        device = Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER];
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &lkp_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_RESULT_INFO, &res_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &db_entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                     &is_valid));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint32 opcode_id = 0;
        uint32 instruction_id = 0;
        uint32 nof_segments = 0;
        int index;
        kbp_key_t_p key;
        kbp_instruction_t_p inst_p;
        char opcode_name[DBAL_MAX_STRING_LENGTH] = { 0 };
        uint32 ad_width[DNX_KBP_MAX_NOF_LOOKUPS] = { 0 };
        uint32 res_padding[DNX_KBP_MAX_NOF_LOOKUPS] = { 0 };
        uint8 rpf_result[DNX_KBP_MAX_NOF_RESULTS] = { 0 };
        uint32 core = 0;
        uint32 master_key_nof_bytes = 0;
        uint32 result_buffer_nof_bytes = 0;

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, &opcode_id));
        instruction_id = opcode_id;

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, &lookup_bmp));

        if (lookup_bmp == 0)
        {
            /** in case the opcode has no lookups no need to configure it in the KBP */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            continue;
        }
        else
        {
            /** Mark the opcode contexts as used */
            uint32 nof_opcode_fwd_context;
            uint32 opcode_fwd_context_id;

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_NOF_OPCODE_FWD_CONTEXTS, INST_SINGLE,
                             &nof_opcode_fwd_context));

            /** Enable aligner for this opcode contexts */
            for (context_index = 0; context_index < nof_opcode_fwd_context; context_index++)
            {
                /** Context is retrieved as enum. Convert it to HW value. */
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OPCODE_FWD_CONTEXT, context_index,
                                 &opcode_fwd_context_id));
                SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                                (unit, DBAL_FIELD_FWD2_CONTEXT_ID, opcode_fwd_context_id, &opcode_fwd_context_id));

                kbp_context_fwd_master_key_valid[opcode_fwd_context_id] = TRUE;
            }
        }

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_OPCODE_NAME, INST_SINGLE, (uint8 *) opcode_name));

        if (!sw_state_is_warm_boot(unit))
        {
            inst_p = NULL;

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_segments));

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "\nCreating new Instruction ID %d opcode (%d) %s\n"), instruction_id,
                         opcode_id, opcode_name));

            DNX_KBP_TRY_PRINT(kbp_instruction_init
                              (device, instruction_id, opcode_id, &inst_p), "Instruction %d", instruction_id);

            DNX_KBP_TRY_PRINT(kbp_key_init(device, &key), "Instruction %d", instruction_id);

            SHR_IF_ERR_EXIT(kbp_mngr_set_master_key
                            (unit, opcode_id, key, nof_segments, instruction_id, &master_key_nof_bytes, FALSE));

            /*
             * Pad the key to a multiplier of 80 bits.
             */
            if ((master_key_nof_bytes % DNX_KBP_MASTER_KEY_BYTE_GRANULARITY) != 0)
            {
                unsigned int padding_size_bytes =
                    DNX_KBP_MASTER_KEY_BYTE_GRANULARITY - (master_key_nof_bytes % DNX_KBP_MASTER_KEY_BYTE_GRANULARITY);
                DNX_KBP_TRY_PRINT(kbp_key_add_field
                                  (key, "PADD_80b", BYTES2BITS(padding_size_bytes), KBP_KEY_FIELD_HOLE),
                                  "Padding key for Instruction %d", instruction_id);

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Adding master key padding segment, name PADD_80b, size %d type %d\n"),
                             padding_size_bytes, KBP_KEY_FIELD_HOLE));
            }
            SHR_IF_ERR_EXIT(kbp_mngr_set_master_key(unit, opcode_id, key, nof_segments, instruction_id, NULL, TRUE));

            DNX_KBP_TRY_PRINT(kbp_instruction_set_key(inst_p, key), "Instruction %d", instruction_id);

            for (index = 0; index < DNX_KBP_MAX_NOF_LOOKUPS; index++)
            {
                if (utilex_bitstream_test_bit(&lookup_bmp, index))
                {
                    uint32 db_id = 0;
                    uint32 result_index = 0;
                    kbp_db_handles_t db_handle;
                    uint32 lookup_type;
                    uint32 table_id;

                    dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
                    dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_ID, (uint8) index);
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lkp_entry_handle_id, DBAL_GET_ALL_FIELDS));

                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE, &db_id));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, lkp_entry_handle_id, DBAL_FIELD_RESULT_ID, INST_SINGLE, &result_index));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_TYPE, INST_SINGLE, &lookup_type));

                    dbal_entry_key_field8_set(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, db_id);
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, db_entry_handle_id, DBAL_GET_ALL_FIELDS));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, db_entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, &table_id));

                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Setting lookup DB (%d) %s search index %d result location %d\n"),
                                 db_id, dbal_logical_table_to_string(unit, table_id), index, result_index));
                    /** Read DB handles from SW state */
                    SHR_IF_ERR_EXIT(KBP_ACCESS.db_handles_info.get(unit, db_id, &db_handle));

                    retval = kbp_instruction_add_db(inst_p, db_handle.db_p, result_index);
                    if (dnx_kbp_error_translation(retval) != _SHR_E_NONE)
                    {
                        DNX_KBP_TRY_PRINT(kbp_instruction_install(inst_p), "Failed Instruction %d", instruction_id);
                        SHR_ERR_EXIT(dnx_kbp_error_translation(retval),
                                     "Instruction %d failed in kbp_instruction_add_db() failed: %s\n",
                                     instruction_id, kbp_get_status_string(retval));
                    }
                }
            }

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, &result_bmp));

            for (index = 0; index < DNX_KBP_MAX_NOF_RESULTS; index++)
            {
                if (utilex_bitstream_test_bit(&result_bmp, index))
                {
                    dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
                    dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_RESULT_ID, (uint8) index);
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, res_entry_handle_id, DBAL_GET_ALL_FIELDS));

                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, res_entry_handle_id, DBAL_FIELD_RESULT_PADDING, INST_SINGLE,
                                     &res_padding[index]));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, res_entry_handle_id, DBAL_FIELD_RESULT_SIZE, INST_SINGLE, &ad_width[index]));

                    /** Do validation of the result size and padding */
                    if (ad_width[index] + res_padding[index] <= 0)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Result %d has no size (%db) and no padding (%db).\n",
                                     index, ad_width[index], res_padding[index]);
                    }
                    if (ad_width[index] + res_padding[index] > DNX_KBP_MAX_RESULT_DATA_LEN)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Result %d exceeds the maximum allowed data length %db: size (%db) + padding (%db).\n",
                                     index, DNX_KBP_MAX_RESULT_DATA_LEN, ad_width[index], res_padding[index]);
                    }

                    /** Convert padding and size from bits to bytes */
                    res_padding[index] = BITS2BYTES(res_padding[index]);
                    ad_width[index] = BITS2BYTES(ad_width[index]);

                    result_buffer_nof_bytes += res_padding[index] + ad_width[index];
                }
            }

            DNX_KBP_TRY_PRINT(kbp_instruction_install(inst_p), "Instruction %d", instruction_id);

            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
            {
                if (dnx_kbp_interface_enabled(unit))
                {
                    dnx_kbp_opcode_map_t opcode_map = { 0 };

                    /** opcode request reply configurations */
                    opcode_map.opcode = opcode_id;
                    opcode_map.tx_type = DNX_KBP_OPCODE_TYPE_REQUEST;
                    opcode_map.tx_size = master_key_nof_bytes;
                    opcode_map.rx_type = DNX_KBP_OPCODE_TYPE_REPLY;
                    opcode_map.rx_size = result_buffer_nof_bytes + 1;
                    SHR_IF_ERR_EXIT(dnx_kbp_opcode_map_set(unit, core, &opcode_map));

                    DNX_KBP_TRY_PRINT(kbp_instruction_set_opcode(inst_p, opcode_id), "Instruction %d", instruction_id);
                }
            }

            /** set KBP_INST_PROP_RESULT_DATA_LEN */
            for (index = 0; index < DNX_KBP_MAX_NOF_LOOKUPS; index++)
            {
                if (utilex_bitstream_test_bit(&result_bmp, index))
                {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Result property set index %d length %dB (pad %dB res %dB)\n"),
                                 index, (ad_width[index] + res_padding[index]), res_padding[index], ad_width[index]));
                    DNX_KBP_TRY_PRINT(kbp_instruction_set_property
                                      (inst_p, KBP_INST_PROP_RESULT_DATA_LEN, index, rpf_result[index],
                                       (ad_width[index] + res_padding[index])), "Instruction %d", instruction_id);
                }
            }
        }
        else
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "\nRefreshing Instruction ID %d opcode (%d) %s\n"), instruction_id, opcode_id,
                         opcode_name));
            SHR_IF_ERR_EXIT(KBP_ACCESS.instruction_info.inst_p.get(unit, instruction_id, smt_id, &inst_p));
            DNX_KBP_TRY_PRINT(kbp_instruction_refresh_handle(device, inst_p, &inst_p), "Instruction %d",
                              instruction_id);
        }

        /** save instruction handles in SW state */
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
        SHR_IF_ERR_EXIT(KBP_ACCESS.instruction_info.inst_p.set(unit, instruction_id, smt_id, inst_p));
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SW_STATE));

        nof_valid_opcodes++;

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    /** Check if the kbp configuration has at least 1 valid opcode, if not return error */
    /** don't do this check if kbp stat exists, because it is being configured outside DNX SDK */
    SHR_IF_ERR_EXIT(dnx_kbp_stat_interface_enable_get(unit, &stat_enable));
    if ((nof_valid_opcodes == 0) && (stat_enable == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Trying to perform KBP sync with no valid opcode");
    }

    /** Disable the KBP lookups for all contexts not used by KBP. */
    for (context_index = 0; context_index < DNX_KBP_FWD_CONTEXT_LAST; context_index++)
    {
        if (!kbp_context_fwd_master_key_valid[context_index])
        {
            SHR_IF_ERR_EXIT(kbp_mngr_lookups_disable(unit, context_index));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Restore the KBP instruction handles from KBP swstate during warmboot
 *   \param [in] unit - Relevant unit.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_opcode_handles_restore(
    int unit)
{
    int smt_id;
    int nof_smts;
    int instruction_id;
    kbp_instruction_t_p inst_p;
    struct kbp_device *device = NULL;
    SHR_FUNC_INIT_VARS(unit);

    nof_smts = (kbp_mngr_smt_enabled(unit) ? DNX_KBP_MAX_NOF_SMTS : 1);

    for (smt_id = 0; smt_id < nof_smts; smt_id++)
    {
        device = (kbp_mngr_smt_enabled(unit) ? Kbp_app_data[unit]->smt_p[DNX_KBP_INST_MASTER][smt_id] :
                  Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER]);

        for (instruction_id = 0; instruction_id < DNX_KBP_NOF_INSTRUCTIONS; instruction_id++)
        {
            SHR_IF_ERR_EXIT(KBP_ACCESS.instruction_info.inst_p.get(unit, instruction_id, smt_id, &inst_p));

            if (inst_p != NULL)
            {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "\nRefreshing Instruction ID %d SMT %d\n"), instruction_id, smt_id));
                DNX_KBP_TRY_PRINT(kbp_instruction_refresh_handle(device, inst_p, &inst_p), "Instruction %d",
                                  instruction_id);

                /** save instruction handles in SW state */
                SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
                SHR_IF_ERR_EXIT(KBP_ACCESS.instruction_info.inst_p.set(unit, instruction_id, smt_id, inst_p));
                SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Configure dummy DB for hole lookups in KBP_DB_INFO
 * This API is called only once during kbp manager sync
 */
static shr_error_e
kbp_mngr_dummy_db_configure(
    int unit)
{
    uint32 entry_handle_id;
    uint32 segment_bytes = 1;
    uint32 segment_type = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_HOLE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_DB_ID, DNX_KBP_DUMMY_HOLE_DB_ID);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_KEY_WIDTH, INST_SINGLE, 80);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_RESULT_WIDTH, INST_SINGLE, 32);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_OPTIMIZED_WIDTH, INST_SINGLE, 0);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DB_TYPE, INST_SINGLE, DBAL_TABLE_TYPE_TCAM_BY_ID);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALGO_MODE, INST_SINGLE,
                                 DNX_KBP_DB_PROPERTIES_ALGORITHMIC_MODE_DEFAULT_INDICATION);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_CAPACITY, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPTIMIZED_CAPACITY, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_XL_DB, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCALITY, INST_SINGLE, 0);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_CLONE_DB, INST_SINGLE, 0);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, DBAL_TABLE_EMPTY);

    SHR_IF_ERR_EXIT(kbp_mngr_db_key_segment_set
                    (unit, DNX_KBP_DUMMY_HOLE_DB_ID, 0, &segment_type, &segment_bytes, "HOLE"));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Creation of ad entry handles for the zero size AD databases.
 * Here we will create only one zero size ad_entry which we keep associating to all the entries,
 * which expects the payload to be zero.
 * This creation need to be done only for Databases which has zero size AD databases.
 */

static shr_error_e
kbp_mngr_create_zero_size_entry_handles(
    int unit)
{
    kbp_db_handles_t db_handles;
    kbp_status res;
    uint32 entry_handle_id;
    int is_end;
    uint8_t value[8] = { 0, };
    uint8 is_valid = TRUE;
    uint8 db_has_zero_size_ad[1];
    uint32 db_id;
    dbal_tables_e dbal_table_id;
    enum kbp_device_type device_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_kbp_init_device_type_get(unit, &device_type));

    /** zero size AD DBs are supported only on OP2 */
    if (device_type == KBP_DEVICE_OP)
    {
        SHR_EXIT();
    }

    /** Iterate through all dbal KBP tables */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                     &is_valid));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_DB_HAS_ZERO_SIZE_AD, INST_SINGLE, db_has_zero_size_ad));

        if (*db_has_zero_size_ad)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_DB_ID, &db_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, &dbal_table_id));

            if (dnx_data_elk.application.db_property_get(unit, dbal_table_id)->zero_size_ad == FALSE)
            {
                const dbal_logical_table_t *table;
                SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error in configuration: Table %s was not allowed to have 0b AD DB\n",
                             table->table_name);
            }

            /** creating the zero size ad_entry for the databases */
            SHR_IF_ERR_EXIT(KBP_ACCESS.db_handles_info.get(unit, db_id, &db_handles));

            res = kbp_ad_db_add_entry(db_handles.ad_db_zero_size_p, value, &db_handles.ad_entry_zero_size_p);
            if (dnx_kbp_error_translation(res) != _SHR_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, " Error: zero size kbp_ad_db_add_entry Failed\n");
            }

            /** save the zero size ad_entry to swstate */
            SHR_IF_ERR_EXIT(KBP_ACCESS.db_handles_info.set(unit, db_id, &db_handles));
            /** save the zero size ad_entry to dbal handles */
            SHR_IF_ERR_EXIT(dbal_tables_table_access_info_set
                            (unit, dbal_table_id, DBAL_ACCESS_METHOD_KBP, (void *) &db_handles));

        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Enables the dummy DB, when it is needed.
 */
static shr_error_e
kbp_mngr_dummy_db_enable(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_DB_ID, DNX_KBP_DUMMY_HOLE_DB_ID);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** Returns the capacity values for a given table */

/**
 * \brief
 *      Returns the capacity values for a given table.
 *      In case non-default initial capacity is provided, the capacity values equal the provided initial capacity.
 *      In case default initial capacity is provided, the capacity values are taken from DNX data.
 *      Regular result capacity is set only if the regular result is valid.
 *      Optimized result capacity  is set only if the optimized result is valid.
 *      Table size is set to the biggest of the two - regular and optimized result capacity.
 * \param [in]  unit - Relevant unit.
 * \param [in]  table_id - DBAL table ID.
 * \param [in]  initial_capacity - Initial capacity provided by SDK.
 * \param [in]  regular_result_valid - Indicates if the table is using regular result.
 * \param [in]  optimized_result_valid - Indicates if the table is using optimized result.
 * \param [out] table_size - The returned initial capacity of the table.
 * \param [out] regular_capacity - The returned regular result capacity.
 * \param [out] optimized_capacity - The returned optimized result capacity.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
kbp_mngr_db_size_and_capacity_get(
    int unit,
    dbal_tables_e table_id,
    uint32 initial_capacity,
    uint8 regular_result_valid,
    uint8 optimized_result_valid,
    uint32 *table_size,
    uint32 *regular_capacity,
    uint32 *optimized_capacity)
{
    int set_size;
    int set_reg_capacity;
    int set_opt_capacity;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(table_size, _SHR_E_INTERNAL, "table_size");
    SHR_NULL_CHECK(regular_capacity, _SHR_E_INTERNAL, "regular_capacity");
    SHR_NULL_CHECK(optimized_capacity, _SHR_E_INTERNAL, "optimized_capacity");

    if (initial_capacity != DNX_KBP_DB_PROPERTIES_MIN_CAPACITY_DEFAULT_INDICATION)
    {
        /** Initial capacity is provided; Set the sizes to this value */
        set_size = set_reg_capacity = set_opt_capacity = initial_capacity;
    }
    else
    {
        /** No initial capacity provided; Read values from DNX data */
        set_reg_capacity = dnx_data_elk.application.db_property_get(unit, table_id)->initial_regular_capacity;
        set_opt_capacity = dnx_data_elk.application.db_property_get(unit, table_id)->initial_optimized_capacity;

        /** Set the DB size to the biggest of the two result capacities */
        set_size = (set_reg_capacity > set_opt_capacity) ? set_reg_capacity : set_opt_capacity;
    }

    /** Set the capacity values */
    *table_size = set_size;
    if (regular_result_valid)
    {
        *regular_capacity = set_reg_capacity;
    }
    if (optimized_result_valid)
    {
        *optimized_capacity = set_opt_capacity;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize a single KBP_DB_INFO table from DBAL table.
 * \param [in] unit - Relevant unit.
 * \param [in] db_id - KBP DB ID.
 * \param [in] clone_id - cloned KBP DB ID, 0 means not cloned.
 * \param [in] table_id - DBAL table ID.
 * \param [in] counters_enable - if counters enable for the table (only ACLs).
 * \param [in] initial_capacity - initial capacity of the DB, 0 means default.
 * \param [in] algo_mode - algo mode of the DB, default will not set any value on this property and use KBP default.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
kbp_mngr_db_info_configure(
    int unit,
    uint8 db_id,
    uint8 clone_id,
    dbal_tables_e table_id,
    uint8 counters_enable,
    uint32 initial_capacity,
    int algo_mode)
{
    uint32 entry_handle_id;
    uint32 key_width = 0;
    uint32 result_width = 0;
    uint32 opt_result_width = 0;
    int key_index;
    const dbal_logical_table_t *table;
    uint8 is_public = FALSE;
    uint8 db_has_zero_size_ad = FALSE;
    uint32 public_field = DBAL_NOF_FIELDS;
    uint32 table_size;
    uint32 regular_result_capacity = 0;
    uint32 optimized_result_capacity = 0;
    int xl_db = 0;
    int locality = 0;
    enum kbp_device_type device_type;
    int nof_valid_segments = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (table->nof_key_fields == 0)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "DB %d is defined with no key segments", db_id);
    }

    if (counters_enable && (table->table_type != DBAL_TABLE_TYPE_TCAM_BY_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Counters are supported only for TCAM_BY_ID table type %s",
                     dbal_table_type_to_string(unit, table->table_type));
    }

    if ((algo_mode != DNX_KBP_DB_PROPERTIES_ALGORITHMIC_MODE_DEFAULT_INDICATION)
        && (table->table_type != DBAL_TABLE_TYPE_TCAM_BY_ID) && (table->table_type != DBAL_TABLE_TYPE_TCAM))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "algo_mode other than -1 only applicable for TCAM tables %d %s", algo_mode,
                     dbal_table_type_to_string(unit, table->table_type));
    }

    /** Check if there is a need to use the optimized result */
    SHR_IF_ERR_EXIT(kbp_mngr_result_info_get(unit, table, &result_width, &opt_result_width));

    SHR_IF_ERR_EXIT(dnx_kbp_valid_key_width(unit, table->key_size, &key_width));
    SHR_IF_ERR_EXIT(dnx_kbp_valid_result_width(unit, result_width, &result_width));
    SHR_IF_ERR_EXIT(dnx_kbp_valid_result_width(unit, opt_result_width, &opt_result_width));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_DB_ID, db_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, table_id);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_COUNTERS_EN, INST_SINGLE, counters_enable);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_KEY_WIDTH, INST_SINGLE, key_width);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_RESULT_WIDTH, INST_SINGLE, result_width);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_OPTIMIZED_WIDTH, INST_SINGLE, opt_result_width);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DB_TYPE, INST_SINGLE, table->table_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALGO_MODE, INST_SINGLE, algo_mode);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_CLONE_DB, INST_SINGLE, clone_id);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, FALSE);

    /** Capacity properties */
    SHR_IF_ERR_EXIT(kbp_mngr_db_size_and_capacity_get
                    (unit, table_id, initial_capacity, (result_width ? TRUE : FALSE), (opt_result_width ? TRUE : FALSE),
                     &table_size, &regular_result_capacity, &optimized_result_capacity));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, table_size);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_CAPACITY, INST_SINGLE,
                                 regular_result_capacity);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPTIMIZED_CAPACITY, INST_SINGLE,
                                 optimized_result_capacity);

    /** DB properties */
    SHR_IF_ERR_EXIT(dnx_kbp_init_device_type_get(unit, &device_type));
    xl_db = (device_type == KBP_DEVICE_OP) ? 0 : dnx_data_elk.application.db_property_get(unit, table_id)->xl_db;
    /** Locality is not available for OP */
    locality = (device_type == KBP_DEVICE_OP) ? 0 : dnx_data_elk.application.db_property_get(unit, table_id)->locality;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_XL_DB, INST_SINGLE, xl_db);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCALITY, INST_SINGLE, locality);

    
    if (sal_strstr(table->table_name, "PRIVATE"))
    {
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_META_PRIORITY, INST_SINGLE, 0);
    }
    else if (sal_strstr(table->table_name, "PUBLIC"))
    {
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_META_PRIORITY, INST_SINGLE, 1);
        if (sal_strstr(table->table_name, "IPV4"))
        {
            is_public = TRUE;
            public_field = DBAL_FIELD_IPV4_PUBLIC_ENABLED;
        }
        else if (sal_strstr(table->table_name, "IPV6"))
        {
            is_public = TRUE;
            public_field = DBAL_FIELD_IPV6_PUBLIC_ENABLED;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Public KBP tables need to be IPv4 or IPv6\n");
        }
    }
    for (key_index = 0; key_index < table->nof_key_fields; key_index++)
    {
        int key_index_packed;
        int key_is_packed_with_nof_next_fields = 0;
        uint32 size_total_bits = table->keys_info[key_index].field_nof_bits;
        uint32 field_nof_bytes;
        uint32 segment_type;
        char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = DNX_KBP_KEY_SEGMENT_NOT_CONFIGURED;

        /*
         * Do not add fields that are packed with previous fields..
         */
        if (SHR_BITGET(table->keys_info[key_index].field_indication_bm, DBAL_FIELD_IND_IS_PACKED))
        {
            continue;
        }
        /*
         * Find if the key is packed with future keys, and combine into a single .
         */
        for (key_index_packed = key_index + 1; key_index_packed < table->nof_key_fields; key_index_packed++)
        {
            if (SHR_BITGET(table->keys_info[key_index_packed].field_indication_bm, DBAL_FIELD_IND_IS_PACKED))
            {
                key_is_packed_with_nof_next_fields++;
                size_total_bits += table->keys_info[key_index_packed].field_nof_bits;
            }
            else
            {
                break;
            }
        }

        if (BYTES2BITS(BITS2BYTES(size_total_bits)) != size_total_bits)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "DB %d has segment (including %d key fields) with size %d, "
                         "size must be in complete bytes.\n", db_id, key_index_packed - key_index, size_total_bits);
        }

        field_nof_bytes = BITS2BYTES(size_total_bits);
        segment_type = (SHR_BITGET(table->keys_info[key_index].field_indication_bm, DBAL_FIELD_IND_IS_RANGED)) ?
            DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_RANGE : DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_EM;

        /** Initialize the segment names to "NOT_CONFIGURED". They will be updated after the table is validated. */
        SHR_IF_ERR_EXIT(kbp_mngr_db_key_segment_set
                        (unit, db_id, nof_valid_segments, &segment_type, &field_nof_bytes, segment_name));
        nof_valid_segments++;
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, nof_valid_segments);

    /** OP2 RPF databases with split RPF in use, we need to create the 0b AD databases for loose RPF scenarios */
    if (kbp_mngr_split_rpf_in_use(unit))
    {
        db_has_zero_size_ad = (device_type == KBP_DEVICE_OP) ? FALSE :
            dnx_data_elk.application.db_property_get(unit, table_id)->zero_size_ad;
    }
    else
    {
        db_has_zero_size_ad = FALSE;
    }
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DB_HAS_ZERO_SIZE_AD, INST_SINGLE,
                                db_has_zero_size_ad);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (is_public == TRUE)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KBP_GENERAL_INFO, entry_handle_id));
        dbal_entry_value_field8_set(unit, entry_handle_id, public_field, INST_SINGLE, TRUE);
    }

    /** Set optimized table indication in order to improve insertion rates. Should be valid only for LPM tables */
    if (dnx_data_elk.application.db_property_get(unit, table_id)->dbal_none_direct_optimized)
    {
        SHR_IF_ERR_EXIT(dbal_tables_indication_set(unit, table_id, DBAL_TABLE_IND_IS_NONE_DIRECT_OPTIMIZED, TRUE));
    }

    /** set app_id to the table */
    SHR_IF_ERR_EXIT(dbal_tables_app_id_set(unit, table_id, db_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Update the key segments of a given DB using the opcode and lookup info.
 *   All DBs are created without key segment names.
 *   They need to be updated using the master key segment names in order to correctly perform device lock.
 * \param [in]  unit - The Unit number.
 * \param [in]  entry_handle_id - Handle to the OPCODE table.
 * \param [in]  lkp_entry_handle_id - Handle to the OPCODE LOOKUP table that will be used to update the segments.
 * \param [in]  db_id - The DB ID in KBP_DB_INFO of the DB that has its segments updated.
 * \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
static shr_error_e
kbp_mngr_db_segment_names_update(
    int unit,
    uint32 entry_handle_id,
    uint32 lkp_entry_handle_id,
    uint8 db_id)
{
    int index;
    uint8 opcode_id;
    uint32 db_entry_handle_id;
    uint32 nof_master_key_segments;
    uint8 nof_db_segments;
    uint32 db_segment_index[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
    char segment_name[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY][DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { {0} };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, &opcode_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_master_key_segments));
    for (index = 0; index < nof_master_key_segments; index++)
    {
        SHR_IF_ERR_EXIT(kbp_mngr_master_key_segment_get(unit, opcode_id, index, NULL, NULL, segment_name[index], NULL));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &db_entry_handle_id));
    dbal_entry_key_field8_set(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, db_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, db_entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, db_entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_db_segments));
    for (index = 0; index < nof_db_segments; index++)
    {
        /** Get the DB key segment index that is mapped to the master key segments */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_SEGMENT_IDS, index, &db_segment_index[index]));
    }

    for (index = 0; index < nof_db_segments; index++)
    {
        /** Update all DB segment names */
        SHR_IF_ERR_EXIT(kbp_mngr_db_key_segment_set
                        (unit, db_id, index, NULL, NULL, segment_name[db_segment_index[index]]));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Create a clone DB using the information from the original DB.
 *   The newly cloned DB key segments will not be set. They need to be updated after the clone is created.
 * \param [in]  unit - The Unit number.
 * \param [in]  original_db_id - The ID in KBP_DB_INFO of the DB that is being cloned.
 * \param [out] clone_db_id - The ID of the new cloned DB in KBP_DB_INFO.
 * \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
static shr_error_e
kbp_mngr_db_clone_create(
    int unit,
    uint8 original_db_id,
    uint8 *clone_db_id)
{
    uint8 db_id;
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &entry_handle_id));

    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_DB_ID, original_db_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, &dbal_table_id));
    SHR_IF_ERR_EXIT(kbp_mngr_db_create_internal(unit, dbal_table_id, original_db_id, FALSE, 0,
                                                DNX_KBP_DB_PROPERTIES_ALGORITHMIC_MODE_DEFAULT_INDICATION, &db_id));

    *clone_db_id = db_id;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See kbp_mngr_create() in kbp_mngr.h
 */
shr_error_e
kbp_mngr_db_create_internal(
    int unit,
    dbal_tables_e table_id,
    uint8 clone_id,
    uint8 counters_enable,
    uint32 initial_capacity,
    int algo_mode,
    uint8 *db_id)
{
    uint32 entry_handle_id, nof_dbs;
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    KBP_COMPILATION_CHECK;

    DNX_KBP_BLOCK_API_IF_DEVICE_IS_LOCKED();

    /** updating the number of DBs in the KBP */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_GENERAL_INFO, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NOF_DBS, INST_SINGLE, &nof_dbs);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    nof_dbs++;
    SHR_IF_ERR_EXIT(kbp_mngr_db_info_configure
                    (unit, nof_dbs, clone_id, table_id, counters_enable, initial_capacity, algo_mode));

    /** updating the number of DBs in the KBP */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_DBS, INST_SINGLE, nof_dbs);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (db_id != NULL)
    {
        *db_id = nof_dbs;
    }

    /*
     * If the device is locked, update status to indicate that the SW has been changed after device lock
     * and that sync need to be redone in order to apply the updated configurations.
     * Currently it's not allowed to perform sync and lock more than once
     * and trying to apply new configurations after device lock will return error.
     */
    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
    if (kbp_mngr_status == DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
    {
        SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_CONFIG_UPDATED));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate the segments of a DB to the master key segments of an opcode, which perform lookup to this DB.
 *   All DB key segments need to have the same type and size as the segments from the master key they are mapped to
 *   in order for the DB to be valid.
 *   This function also returns error indication if the DB needs to have its segments updated
 *   and if a clone DB need to be used.
 * \param [in]  unit - The Unit number.
 * \param [in]  entry_handle_id - Handle to the OPCODE table.
 * \param [in]  lkp_entry_handle_id - Handle to the OPCODE LOOKUP table.
 * \param [in]  db_id - The DB ID in KBP_DB_INFO of the DB that is being validated.
 * \param [in]  key_type - The type of each opcode master key segment.
 * \param [in]  key_size - The size of each opcode master key segment.
 * \param [in]  key_name - The name of each opcode master key segment.
 * \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *    _SHR_E_EXISTS    - Indicates that the DB exists and its segments need to be updated.
 *    _SHR_E_NOT_FOUND - Indicates that the DB does not match the segments and a clone need to be used.
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
static shr_error_e
kbp_mngr_db_info_validate(
    int unit,
    uint32 entry_handle_id,
    uint32 lkp_entry_handle_id,
    uint8 db_id,
    uint32 key_type[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP],
    uint32 key_size[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP],
    char key_name[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP][DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES])
{
    int index;
    uint32 db_entry_handle_id;
    uint8 db_update = TRUE;
    uint8 db_found = TRUE;
    uint8 nof_db_segments;
    uint32 db_key_index[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP];

    uint32 db_key_type;
    uint32 db_key_size;
    char db_key_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &db_entry_handle_id));
    dbal_entry_key_field8_set(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, db_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, db_entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, db_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &db_update));
    /** If DB is already valid, no need to update; invert -> valid = !update; !valid = update */
    db_update = !db_update;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, db_entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_db_segments));

    for (index = 0; index < nof_db_segments; index++)
    {
        /** Get the DB key segment index that is mapped to the master key segments */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_SEGMENT_IDS, index, &db_key_index[index]));
        SHR_IF_ERR_EXIT(kbp_mngr_db_key_segment_get(unit, db_id, index, &db_key_type, &db_key_size, db_key_name));

        if (db_update == TRUE)
        {
            if (sal_strcmp(db_key_name, DNX_KBP_KEY_SEGMENT_NOT_CONFIGURED) != 0)
            {
                /** DB segment name is configured; DB is initialized */
                db_update = FALSE;
            }
        }
        if (db_found == TRUE)
        {
            if (sal_strcmp(db_key_name, key_name[db_key_index[index]]) != 0)
            {
                /** DB segment name does not match the opcode (master key) segment name */
                db_found = FALSE;
            }
        }
        if ((db_update == FALSE) && (db_found == FALSE))
        {
            /** DB is valid and does not match */
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
    }

    if (db_update == TRUE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EXISTS);
        SHR_EXIT();
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Update all valid forwarding DB info using information from opcode info.
 *   Update key segment names, db width and decide if a clone needs to be created for a DB.
 *   \param [in] unit - Relevant unit.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_db_info_update(
    int unit)
{
    uint32 entry_handle_id;
    uint32 db_entry_handle_id;
    uint32 lkp_entry_handle_id;
    int is_end;
    uint8 is_valid = TRUE;
    uint8 is_standard_image = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));

    /** Create DUMMY DB for HOLE lookups */
    SHR_IF_ERR_EXIT(kbp_mngr_dummy_db_configure(unit));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &db_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &lkp_entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                     &is_valid));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        int lookup_index;
        int key_index;
        uint8 opcode_id;
        uint32 nof_keys;
        uint32 lookup_bmp[1] = { 0 };
        uint8 db_id;
        uint32 key_type[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY] = { 0 };
        uint32 key_size[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY] = { 0 };
        char key_name[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY][DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { {0} };

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, &opcode_id));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_keys));

        for (key_index = 0; key_index < nof_keys; key_index++)
        {
            SHR_IF_ERR_EXIT(kbp_mngr_master_key_segment_get
                            (unit, opcode_id, key_index, &key_type[key_index], &key_size[key_index],
                             key_name[key_index], NULL));
        }

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, lookup_bmp));

        /** Iterate over all valid lookups and validate the lookup DB */
        for (lookup_index = 0; lookup_index < DNX_KBP_MAX_NOF_LOOKUPS; lookup_index++)
        {
            uint8 is_hole_lookup = FALSE;

            dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
            dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_ID, (uint8) lookup_index);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, lkp_entry_handle_id, DBAL_GET_ALL_FIELDS));

            if (!utilex_bitstream_test_bit(lookup_bmp, lookup_index))
            {
                if (utilex_bitstream_have_one_in_range(lookup_bmp, lookup_index + 1, DNX_KBP_MAX_NOF_LOOKUPS - 1))
                {
                    /*
                     * Current lookup is not valid, but is followed by at least one valid lookup.
                     * This indicates hole and will be configured at later stage.
                     */

                    is_hole_lookup = TRUE;
                }
                else
                {
                    /** No other valid lookups. Can exit the loop */
                    break;
                }
            }
            if (!is_hole_lookup)
            {
                shr_error_e res;

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE, &db_id));
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Validating DB ID %d on opcode %d lookup index %d - "), db_id, opcode_id,
                             lookup_index));
                res =
                    kbp_mngr_db_info_validate(unit, entry_handle_id, lkp_entry_handle_id, db_id, key_type, key_size,
                                              key_name);

                if (res == _SHR_E_EXISTS)
                {
                    uint8 lookup_type = DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FREE;

                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "update\n")));
                    /** Update the DB with segments from the opcode master key */
                    SHR_IF_ERR_EXIT(kbp_mngr_db_segment_names_update
                                    (unit, entry_handle_id, lkp_entry_handle_id, db_id));

                    /** Mark the DB as valid */
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                    (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_TYPE, INST_SINGLE, &lookup_type));

                    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KBP_DB_INFO, db_entry_handle_id));
                    dbal_entry_key_field8_set(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, db_id);
                    dbal_entry_value_field8_set(unit, db_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);

                    if (is_standard_image)
                    {
                        /** Set the ad_width for DBs on FWD lookup to 64b in order to perform the lookup correctly */
                        if (lookup_type == DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD ||
                            lookup_type == DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD_PUBLIC)
                        {
                            dbal_entry_value_field32_set(unit, db_entry_handle_id, DBAL_FIELD_RESULT_WIDTH, INST_SINGLE,
                                                         DNX_KBP_RESULT_SIZE_FWD);
                        }
                    }

                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, db_entry_handle_id, DBAL_COMMIT));
                }
                else if (res == _SHR_E_NOT_FOUND)
                {
                    /*
                     * Search for other DBs, that are mapped to the same DBAL table (search for existing clones).
                     * Create new clone if no matching DB is found
                     */
                    int db_is_end;
                    uint8 clone_db_id;
                    dbal_tables_e dbal_table;
                    dbal_tables_e clone_dbal_table;

                    /** Get the DBAL table mapped to the lookup DB */
                    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KBP_DB_INFO, db_entry_handle_id));
                    dbal_entry_key_field8_set(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, db_id);
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, db_entry_handle_id, DBAL_GET_ALL_FIELDS));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                    (unit, db_entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, &dbal_table));

                    SHR_IF_ERR_EXIT(dbal_iterator_init
                                    (unit, db_entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
                    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, db_entry_handle_id, &db_is_end));

                    while (!db_is_end)
                    {
                        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get
                                        (unit, db_entry_handle_id, DBAL_FIELD_DB_ID, &clone_db_id));
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                        (unit, db_entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE,
                                         &clone_dbal_table));

                        if ((db_id != clone_db_id) && (dbal_table == clone_dbal_table))
                        {
                            /** A different DB to the same DBAL table (possible clone). Validate segments. */
                            res =
                                kbp_mngr_db_info_validate(unit, entry_handle_id, lkp_entry_handle_id, clone_db_id,
                                                          key_type, key_size, key_name);
                            if (res == _SHR_E_NONE)
                            {
                                /** Existing clone DB found */
                                break;
                            }
                        }
                        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, db_entry_handle_id, &db_is_end));
                    }
                    if (res == _SHR_E_NOT_FOUND)
                    {
                        /*
                         * All appropriate DBs were checked and no matching clone DB was found.
                         * Create new clone DB.
                         */
                        SHR_IF_ERR_EXIT(kbp_mngr_db_clone_create(unit, db_id, &clone_db_id));
                        SHR_IF_ERR_EXIT(kbp_mngr_db_segment_names_update
                                        (unit, entry_handle_id, lkp_entry_handle_id, clone_db_id));

                        /** Mark the clone as valid */
                        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_KBP_DB_INFO, db_entry_handle_id));
                        dbal_entry_key_field8_set(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, clone_db_id);
                        dbal_entry_value_field8_set(unit, db_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
                        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, db_entry_handle_id, DBAL_COMMIT));

                        /** Update the opcode DB lookup */
                        dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
                        dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE,
                                                    clone_db_id);
                        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, lkp_entry_handle_id, DBAL_COMMIT));
                        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "new clone DB ID %d created\n"), clone_db_id));
                    }
                    else if (res == _SHR_E_NONE)
                    {
                        /*
                         * Matching clone DB was found.
                         * Update the lookup info.
                         */
                        dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);
                        dbal_entry_value_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE,
                                                    clone_db_id);
                        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, lkp_entry_handle_id, DBAL_COMMIT));
                        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "use clone DB ID %d\n"), clone_db_id));
                    }
                    else
                    {
                        SHR_ERR_EXIT(res, "kbp_mngr_db_info_validate failed\n");
                    }
                }
                else if (res != _SHR_E_NONE)
                {
                    SHR_ERR_EXIT(res, "kbp_mngr_db_info_validate failed\n");
                }
                else
                {
                    /** In case of no error, the DB is valid and the segments match the segments in the master key */
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "found\n")));
                }
            }
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Sync all Valid DBs to KBP device.
 *   Each entry in KBP_DB_INFO contains all information for creating a KBP DB.
 *   Create a KBP DB or clone DB for all valid entries in KBP_DB_INFO.
 *   \param [in] unit - Relevant unit.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_db_sync(
    int unit)
{
    uint32 entry_handle_id;
    int is_end;
    uint8 is_valid = TRUE;
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    dbal_table_type_e db_type = 0;
    uint32 db_id = 0, ad_db_id = 0, clone_db = 0;
    uint32 table_size = 0, regular_capacity = 0, optimized_capacity = 0;
    uint32 key_width = 0, result_width = 0, opt_result_width = 0;
    uint32 nof_segments = 0;
    uint32 xl_db = 0, locality = 0, algo_mode = 0, meta_priority = 0;
    uint8 counters_enable = FALSE;
    uint8 db_has_zero_size_ad = FALSE;
    char *db_name = NULL;
    struct kbp_device *device = NULL;

    int index;
    int kbp_db_type = 0;
    kbp_key_t_p key;
    kbp_db_handles_t db_handles;
    uint32 caching_bmp = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    device = Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER];

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                     &is_valid));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &is_valid));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_ENTRY_COUNTERS_EN, INST_SINGLE, &counters_enable));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_DB_TYPE, INST_SINGLE, &db_type));

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_DB_ID, &db_id));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, &dbal_table_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_CLONE_DB, INST_SINGLE, &clone_db));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_WIDTH, INST_SINGLE, &result_width));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OPTIMIZED_WIDTH, INST_SINGLE, &opt_result_width));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, &table_size));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_CAPACITY, INST_SINGLE, &regular_capacity));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OPTIMIZED_CAPACITY, INST_SINGLE, &optimized_capacity));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_KEY_WIDTH, INST_SINGLE, &key_width));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_META_PRIORITY, INST_SINGLE, &meta_priority));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_segments));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_ALGO_MODE, INST_SINGLE, &algo_mode));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_XL_DB, INST_SINGLE, &xl_db));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_LOCALITY, INST_SINGLE, &locality));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_DB_HAS_ZERO_SIZE_AD, INST_SINGLE, &db_has_zero_size_ad));

        db_name = dbal_logical_table_to_string(unit, dbal_table_id);

        sal_memset(&db_handles, 0, sizeof(db_handles));

        SHR_IF_ERR_EXIT(kbp_mngr_dbal_db_type_to_kbp_db_type(unit, db_type, &kbp_db_type));

        /** Set the caching indication to FWD or ACL */
        if (DNX_KBP_IS_TABLE_USED_FOR_FWD(db_type))
        {
            caching_bmp |= DNX_KBP_CACHING_BMP_FWD;
        }
        else if (DNX_KBP_IS_TABLE_USED_FOR_ACL(db_type))
        {
            caching_bmp |= DNX_KBP_CACHING_BMP_ACL;
        }

        if (clone_db == 0)
        {
            /** not a clone - init the DB */

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U
                         (unit, "\nCreating new DB ID (%d) %s type %d, size %d, key_width %db result_width %db\n"),
                         db_id, db_name, db_type, table_size, key_width, result_width));

            DNX_KBP_TRY_PRINT(kbp_db_init(device, kbp_db_type, db_id, table_size, &db_handles.db_p), "DB %d", db_id);
            DNX_KBP_TRY_PRINT(kbp_db_set_property(db_handles.db_p, KBP_PROP_DESCRIPTION, db_name), "DB %d", db_id);

            /** Enabling External Counters on ACL database */
            if (counters_enable)
            {
                DNX_KBP_TRY_PRINT(kbp_db_set_property(db_handles.db_p, KBP_PROP_ENABLE_COUNTERS, 1), "DB %d", db_id);
            }

            /** Initialize regular AD DB according to result size present */
            if (result_width)
            {
                DNX_KBP_TRY_PRINT(kbp_ad_db_init
                                  (device, ad_db_id, regular_capacity, result_width,
                                   &db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR]), "DB %d AD DB %d", db_id,
                                  ad_db_id);
                DNX_KBP_TRY_PRINT(kbp_db_set_ad(db_handles.db_p, db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR]),
                                  "DB %d AD DB %d", db_id, ad_db_id);
                ad_db_id++;
            }
            /** Initialize optimized AD DB according to optimized indication and optimized result size present */
            if (DNX_KBP_USE_OPTIMIZED_RESULT && opt_result_width)
            {
                db_handles.opt_result_size = opt_result_width;
                DNX_KBP_TRY_PRINT(kbp_ad_db_init
                                  (device, ad_db_id, optimized_capacity, opt_result_width,
                                   &db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED]), "DB %d AD DB %d", db_id,
                                  ad_db_id);
                DNX_KBP_TRY_PRINT(kbp_db_set_ad(db_handles.db_p, db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED]),
                                  "DB %d AD DB %d", db_id, ad_db_id);
                ad_db_id++;
            }

            if (db_has_zero_size_ad)
            {
                DNX_KBP_TRY_PRINT(kbp_ad_db_init
                                  (device, ad_db_id, 32 * 1024, 0, &db_handles.ad_db_zero_size_p),
                                  "DB %d AD DB %d", db_id, ad_db_id);
                DNX_KBP_TRY_PRINT(kbp_db_set_ad(db_handles.db_p, db_handles.ad_db_zero_size_p),
                                  "DB %d AD DB %d", db_id, ad_db_id);
                ad_db_id++;
            }

            /** Set the caching indication to allow caching for this table (allowed for all non-cloned DBs) */
            caching_bmp |= DNX_KBP_CACHING_BMP_ALLOWED;
        }
        else
        {
            /** Create clone */

            kbp_db_handles_t cloned_db_handles;
            /** Read DB handles from SW state */
            SHR_IF_ERR_EXIT(KBP_ACCESS.db_handles_info.get(unit, clone_db, &cloned_db_handles));

            DNX_KBP_TRY_PRINT(kbp_db_clone(cloned_db_handles.db_p, db_id, &(db_handles.db_p)),
                              "Orig DB %d Clone DB %d", clone_db, db_id);

            if (result_width)
            {
                db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR] =
                    cloned_db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR];
            }
            if (DNX_KBP_USE_OPTIMIZED_RESULT && opt_result_width)
            {
                db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED] =
                    cloned_db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED];
            }

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "\nDB ID %d Cloned; new DB ID %d, %p\n"), clone_db, db_id,
                         (void *) db_handles.db_p));
        }

        DNX_KBP_TRY_PRINT(kbp_key_init(device, &key), "DB %d", db_id);

        /** setting the DB key in KBP according to table definitions */
        for (index = 0; index < nof_segments; index++)
        {
            uint32 field_nof_bytes;
            char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };
            int kbp_segment_type;

            SHR_IF_ERR_EXIT(kbp_mngr_db_key_segment_get
                            (unit, db_id, index, (uint32 *) &kbp_segment_type, &field_nof_bytes, segment_name));
            SHR_IF_ERR_EXIT(kbp_mngr_dbal_key_field_type_to_kbp_key_field_type
                            (unit, kbp_segment_type, &kbp_segment_type));

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Adding Segment %d, name %s, size %d type %d\n"), index,
                         segment_name, field_nof_bytes, kbp_segment_type));

            DNX_KBP_TRY_PRINT(kbp_key_add_field(key, segment_name, BYTES2BITS(field_nof_bytes), kbp_segment_type),
                              "DB %d %s %d", db_id, segment_name, index);
        }

        DNX_KBP_TRY_PRINT(kbp_db_set_key(db_handles.db_p, key), "DB %d", db_id);

        /** Meta priority */
        if (meta_priority != 0)
        {
            int kbp_meta_priority = 0;
            /** Convert meta priority to KBP value, using hard coded mapping */
            SHR_IF_ERR_EXIT(kbp_mngr_kbp_meta_priority_get(unit, meta_priority, &kbp_meta_priority));
            DNX_KBP_TRY_PRINT(kbp_db_set_property(db_handles.db_p, KBP_PROP_ENTRY_META_PRIORITY, kbp_meta_priority),
                              "DB %d Meta prio %d", db_id, kbp_meta_priority);
        }

        if (kbp_db_type == KBP_DB_ACL)
        {
            /** Priority */
            DNX_KBP_TRY_PRINT(kbp_db_set_property(db_handles.db_p, KBP_PROP_MIN_PRIORITY, DNX_KBP_MIN_ACL_PRIORITY),
                              "DB %d Prio %d", db_id, DNX_KBP_MIN_ACL_PRIORITY);
        }

        /** Algorithmic property */
        if (algo_mode != DBAL_ENUM_FVAL_ALGO_MODE_DEFAULT)
        {
            DNX_KBP_TRY_PRINT(kbp_db_set_property(db_handles.db_p, KBP_PROP_ALGORITHMIC, algo_mode),
                              "DB %d Algo mode %d", db_id, algo_mode);
        }

        /** Set the DBs to extra large */
        if (xl_db != 0)
        {
            /*
             * If any LPM DB is configured as XL, they will all default to XL. Only 4 XL DBs are supported
             */
            DNX_KBP_TRY_PRINT(kbp_db_set_property(db_handles.db_p, KBP_PROP_XL_DB, xl_db), "DB %d XL_DB %d", db_id,
                              xl_db);
        }

        /** Set locality for RPF Tables */
        if (locality != 0)
        {
            DNX_KBP_TRY_PRINT(kbp_db_set_property(db_handles.db_p, KBP_PROP_LOCALITY, locality),
                              "DB %d Locality %d", db_id, locality);
        }

        /*
         * When large multicast mode=1 is enabled, set the following db properties to KBP
         */
        if ((dnx_data_elk.application.ipv4_large_mc_enable_get(unit) == KBP_DB_MC_MODE1 &&
             dbal_table_id == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD) ||
            (dnx_data_elk.application.ipv6_large_mc_enable_get(unit) == KBP_DB_MC_MODE1 &&
             dbal_table_id == DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD))
        {
            /*
             * Setting critical segment to KBP. Since the DIP in the multicast is fully
             * specified bits set this as highest critical segment
             * and VRF is next critical segment.
             */
            if (dbal_table_id == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD)
            {
                DNX_KBP_TRY_PRINT(kbp_key_set_critical_field(key, "IPV4_DIP"),
                                  "Critical segment IPV4_DIP DB %d", db_id);
            }
            else
            {
                DNX_KBP_TRY_PRINT(kbp_key_set_critical_field(key, "IPV6_DIP"),
                                  "Critical segment IPV6_DIP DB %d", db_id);
            }
            DNX_KBP_TRY_PRINT(kbp_key_set_critical_field(key, "VRF"), "Critical segment VRF DB %d", db_id);

            DNX_KBP_TRY_PRINT(kbp_db_set_property
                              (db_handles.db_p, KBP_PROP_ALGORITHMIC, DBAL_ENUM_FVAL_ALGO_MODE_NET_ACL),
                              "DB %d Algo mode 3", db_id);
            DNX_KBP_TRY_PRINT(kbp_db_set_property(db_handles.db_p, KBP_PROP_MC_DB, KBP_DB_MC_TYPE1), "DB %d", db_id);
        }

        /*
         * When large multicast mode=2 is enabled, set the following db properties to KBP
         */
        if ((dnx_data_elk.application.ipv4_large_mc_enable_get(unit) == KBP_DB_MC_MODE2 &&
             dbal_table_id == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD) ||
            (dnx_data_elk.application.ipv6_large_mc_enable_get(unit) == KBP_DB_MC_MODE2 &&
             dbal_table_id == DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD))
        {
            DNX_KBP_TRY_PRINT(kbp_db_set_property
                              (db_handles.db_p, KBP_PROP_ALGORITHMIC, DBAL_ENUM_FVAL_ALGO_MODE_NET_ACL),
                              "DB %d Algo mode 3", db_id);
        }

        /** Set the caching bitmap; In warmboot the caching bitmap will be retrieved from the swstate */
        db_handles.caching_bmp = caching_bmp;
        db_handles.cloned_db_id = clone_db;
        db_handles.associated_dbal_table_id = dbal_table_id;

        /** save DB handles in SW state */
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
        SHR_IF_ERR_EXIT(KBP_ACCESS.db_handles_info.set(unit, db_id, &db_handles));
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SW_STATE));

        if ((db_id != DNX_KBP_DUMMY_HOLE_DB_ID) && (clone_db == 0))
        {
            /** app_id set in the table configuration step */
            /** save DB handles in DBAL*/
            SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
            SHR_IF_ERR_EXIT(dbal_tables_table_access_info_set
                            (unit, dbal_table_id, DBAL_ACCESS_METHOD_KBP, (void *) &db_handles));
            SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    /** In case a DB init failure, print the DB configuration */
    if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE)
    {
        LOG_CLI((BSL_META("\nDB name %s | DBAL table ID %d | DBAL table type %d\n"), db_name, dbal_table_id, db_type));
        LOG_CLI((BSL_META("DB ID %d | Clone ID %d | Number of segments %d\n"), db_id, clone_db, nof_segments));
        LOG_CLI((BSL_META("Key size %d | Regular result size %d | Optimized result size %d\n"),
                 key_width, result_width, opt_result_width));
        LOG_CLI((BSL_META("Table size %d | Regular capacity %d | Optimized capacity %d\n"),
                 table_size, regular_capacity, optimized_capacity));
        LOG_CLI((BSL_META("Properties:\n\tXL DB %d\n\tLocality %d\n\tAlgo mode %d\n\tMeta prio %d\n\tCounters %d\n"),
                 xl_db, locality, algo_mode, meta_priority, counters_enable));
    }

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Restore the KBP DB handles from KBP swstate during warmboot
 *   \param [in] unit - Relevant unit.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_db_handles_restore(
    int unit)
{
    int db_id;
    kbp_db_handles_t db_handles;
    struct kbp_device *device = Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER];
    SHR_FUNC_INIT_VARS(unit);

    /** Skip Dummy DB (db_id=0) */
    for (db_id = 1; db_id < DNX_KBP_NOF_DBS; db_id++)
    {
        SHR_IF_ERR_EXIT(KBP_ACCESS.db_handles_info.get(unit, db_id, &db_handles));

        /** Refresh DB handles only if handles were created before warmboot. */
        if (db_handles.db_p != NULL)
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\nRefreshing DB ID %d\n"), db_id));
            DNX_KBP_TRY_PRINT(kbp_db_refresh_handle(device, db_handles.db_p, &db_handles.db_p), "DB %d", db_id);

            if (db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR] != NULL)
            {
                DNX_KBP_TRY_PRINT(kbp_ad_db_refresh_handle
                                  (device, db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR],
                                   &db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR]), "DB %d Regular AD_DB", db_id);
            }
            if (db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED] != NULL)
            {
                DNX_KBP_TRY_PRINT(kbp_ad_db_refresh_handle
                                  (device, db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED],
                                   &db_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED]), "DB %d Optimized AD_DB", db_id);
            }

            if (db_handles.ad_db_zero_size_p != NULL)
            {
                DNX_KBP_TRY_PRINT(kbp_ad_db_refresh_handle
                                  (device, db_handles.ad_db_zero_size_p,
                                   &db_handles.ad_db_zero_size_p), "DB %d Zero size AD_DB", db_id);
            }

            if (db_handles.cloned_db_id == 0)
            {
                /** set app_id to the table */
                SHR_IF_ERR_EXIT(dbal_tables_app_id_set(unit, db_handles.associated_dbal_table_id, db_id));
                /** save DB handles in DBAL*/
                SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
                SHR_IF_ERR_EXIT(dbal_tables_table_access_info_set
                                (unit, db_handles.associated_dbal_table_id, DBAL_ACCESS_METHOD_KBP,
                                 (void *) &db_handles));
                SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
            }
        }

        /** save DB handles in SW state */
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
        SHR_IF_ERR_EXIT(KBP_ACCESS.db_handles_info.set(unit, db_id, &db_handles));
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Restore the DBAL app_id for KBP tables from KBP_DB_INFO during warmboot.
 *   API is called regardles whether KBP sync is performed or not.
 *   The API takes advantage of the fact that all configured DBs have sequential IDs.
 *   \param [in] unit - Relevant unit.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_db_app_id_restore(
    int unit)
{
    uint8 db_id;
    uint32 entry_handle_id, nof_dbs;
    uint32 table_id, clone_db;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take the number of DBs in the KBP */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_GENERAL_INFO, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NOF_DBS, INST_SINGLE, &nof_dbs);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &entry_handle_id));

    /** Valid DB start from ID=1. The last valid DB has ID=nof_dbs. */
    for (db_id = 1; db_id <= nof_dbs; db_id++)
    {
        dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_DB_ID, db_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, &table_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CLONE_DB, INST_SINGLE, &clone_db);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        /** Restore the app_id only for DBs that are not clones. */
        if (clone_db == 0)
        {
            /** set app_id to the table */
            SHR_IF_ERR_EXIT(dbal_tables_app_id_set(unit, table_id, db_id));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   read the KBP opcode definitions XML and updates the KBP SW info.
 *   the original XML defined by the opcode XML is created by it by the magic push.
 *   during this process all the opcodes are being loaded to the kbp mnr.
 *   \param [in] unit - Relevant unit.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
static shr_error_e
kbp_mngr_fwd_opcode_info_init(
    int unit)
{
    void *catalogue = NULL;
    void *opcode;
    char *image_name;
    char *opcodes_path = NULL;
    uint8 is_standard_image = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(opcodes_path, DBAL_MAX_PRINTABLE_BUFFER_SIZE, "opcodes_path", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));
    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_get(unit, &image_name));
    DNX_KBP_OPCODES_XML_FILE_PATH_FROM_IMAGE_NAME(opcodes_path, image_name);

    catalogue = dbx_pre_compiled_kbp_top_get(unit, opcodes_path, "KbpOpcodesDbCatalogue", CONF_OPEN_PER_DEVICE);
    if (catalogue == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find KBP configuration xml file: %s\n", opcodes_path);
    }

    /** reading each opcode and updating the info */
    RHDATA_ITERATOR(opcode, catalogue, "Opcode")
    {
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_read(unit, opcode, is_standard_image));
    }

exit:
    SHR_FREE(opcodes_path);
    dbx_xml_top_close(catalogue);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Runs over all DBAL tables and configure according to the information the KBP SW tables.
 * \param [in] unit - Relevant unit.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
kbp_mngr_fwd_db_info_init(
    int unit)
{
    dbal_tables_e table_iter;
    uint8 db_id = 0;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                    (unit, DBAL_TABLE_EMPTY, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_KBP, DBAL_PHYSICAL_TABLE_NONE,
                     DBAL_TABLE_TYPE_NONE, &table_iter));
    while (table_iter != DBAL_TABLE_EMPTY)
    {
        db_id++;
        SHR_IF_ERR_EXIT(kbp_mngr_db_info_configure
                        (unit, db_id, 0, table_iter, FALSE, 0,
                         DNX_KBP_DB_PROPERTIES_ALGORITHMIC_MODE_DEFAULT_INDICATION));
        SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                        (unit, table_iter, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_KBP, DBAL_PHYSICAL_TABLE_NONE,
                         DBAL_TABLE_TYPE_NONE, &table_iter));
    }

    /** updating the number of DBs in the KBP */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_GENERAL_INFO, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_DBS, INST_SINGLE, db_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Updating FWD Applications DB in DBAL SW tables.
*   tables are read from DBAL and the KBP related properties are set in SW
*   LTRs (Opcodes) are read from XML and set in SW
*   \param [in] unit - Relevant unit.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
static shr_error_e
kbp_mngr_fwd_applications_info_init(
    int unit)
{
    enum kbp_device_type device_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_kbp_init_device_type_get(unit, &device_type));

    if (device_type != KBP_DEVICE_OP2 &&
        (dnx_data_elk.application.ipv4_large_mc_enable_get(unit) ||
         dnx_data_elk.application.ipv6_large_mc_enable_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "IPv4/IPv6 large multicast is applicable for only OP2 \n");
    }

    SHR_IF_ERR_EXIT(kbp_mngr_fwd_db_info_init(unit));
    SHR_IF_ERR_EXIT(kbp_mngr_fwd_opcode_info_init(unit));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
kbp_mngr_device_set_properties_and_lock(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Associate slave devices if any are present */
    if (DNX_KBP_IS_SECOND_KBP_EXIST)
    {
        DNX_KBP_TRY(kbp_device_set_property
                    (Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER], KBP_DEVICE_ADD_BROADCAST, DNX_KBP_INST_SLAVE,
                     Kbp_app_data[unit]->device_p[DNX_KBP_INST_SLAVE]));
    }

    DNX_KBP_TRY(kbp_device_set_property
                (Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER], KBP_DEVICE_PRE_CLEAR_ABS, 1));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "\nPerforming device Lock %p\n"),
                 (void *) (Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER])));

    DNX_KBP_TRY(kbp_device_lock(Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER]));
    SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED));
    SHR_IF_ERR_EXIT(KBP_ACCESS.is_device_locked.set(unit, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See kbp_mngr_wb_init() in kbp_mngr.h
 */
shr_error_e
kbp_mngr_wb_init_internal(
    int unit)
{
    int is_device_locked_in_swstate = FALSE;
    kbp_warmboot_t *warmboot_data = &Kbp_warmboot_data[unit];
    SHR_FUNC_INIT_VARS(unit);

    if (!sw_state_is_warm_boot(unit))
    {
        /** Failsafe: This function is supposed to be called only during warmboot. */
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP warmboot restore sequence called outside of warmboot\n");
    }

    SHR_IF_ERR_EXIT(KBP_ACCESS.is_device_locked.get(unit, &is_device_locked_in_swstate));

    if (is_device_locked_in_swstate == FALSE)
    {
        /** Restore the the app_id of all KBP DBAL tables and exit. */
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_DBAL));
        SHR_IF_ERR_EXIT(kbp_mngr_db_app_id_restore(unit));
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_DBAL));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_DBAL));

    /** Update KBP status 'in warmboot.' */
    SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_WB_IN_PROCESS));

    /** Restore device state */
    DNX_KBP_TRY(kbp_device_restore_state(Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER],
                                         warmboot_data->kbp_file_read,
                                         warmboot_data->kbp_file_write, warmboot_data->kbp_file_fp));

    /** Restore DB handles. */
    SHR_IF_ERR_EXIT(kbp_mngr_db_handles_restore(unit));

    /** Restore instruction handles. */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_handles_restore(unit));

    /** Set device properties and perform device lock. */
    SHR_IF_ERR_EXIT(kbp_mngr_device_set_properties_and_lock(unit));

    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_DBAL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See kbp_mngr_sync() in kbp_mngr.h
 */
shr_error_e
kbp_mngr_sync_internal(
    int unit)
{
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;
    uint32 smt_id;
    int smt_id_start;
    int smt_id_end;
    uint8 dummy_lookup_used_in_no_lookup_opcode;
    uint8 dummy_lookup_used_in_holes;
    int stif_enabled = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    smt_id_start = 0;
    smt_id_end = (kbp_mngr_smt_enabled(unit) ? DNX_KBP_MAX_NOF_SMTS : 1);

    if (sw_state_is_warm_boot(unit))
    {
        /** Failsafe: This function is NOT supposed to be called during warmboot. */
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP sync sequence called during warmboot\n");
    }

    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));

    /** for now sync and lock is performed only once */
    if (kbp_mngr_status == DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_BEFORE_INIT)
    {
        /** Suppose to indicate that KBP mngr was never initialized. */
        SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP manager has not performed any initializations\n");
    }
    else if (kbp_mngr_status == DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_CONFIG_UPDATED)
    {
        /** KBP device sync already performed. Deinit and init device in order to continue. */
        LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "KBP is being reconfigured. All KBP entries will be deleted.\n")));
        SHR_IF_ERR_EXIT(dnx_kbp_device_deinit(unit));
        SHR_IF_ERR_EXIT(dnx_kbp_device_init(unit));
    }
    else if (kbp_mngr_status == DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
    {
        /** No changes detected after device lock. Do nothing */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_SYNC_IN_PROCESS));

    /** Update DB configuration tables. */
    SHR_IF_ERR_EXIT(kbp_mngr_db_info_update(unit));
    /*
     * Set the ACL_RESOLUTION_ENABLE enablers, and add dummy lookups to opcodes that don't KBP lookups but are either
     * the parent or the child of an opcode that does.
     */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_all_acl_resolution_enable_update(unit, &dummy_lookup_used_in_no_lookup_opcode));

    /** Update the opcode configuration tables. */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_info_update(unit, &dummy_lookup_used_in_holes));
    if (dummy_lookup_used_in_no_lookup_opcode || dummy_lookup_used_in_holes)
    {
        /** If dummy DB is used, mark the dummy DB as enabled. */
        SHR_IF_ERR_EXIT(kbp_mngr_dummy_db_enable(unit));
    }

    /** Perform DB and opcode sync. */
    SHR_IF_ERR_EXIT(kbp_mngr_db_sync(unit));
    for (smt_id = smt_id_start; smt_id < smt_id_end; smt_id++)
    {
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_sync(unit, smt_id));
    }

    /** Validate the number of lookups used by each opcode and output meaningful error. */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookups_validate(unit));

    /** Validate ELK ports are configured on real device. */
    SHR_IF_ERR_EXIT(dnx_kbp_stat_interface_enable_get(unit, &stif_enabled));
    if (!SAL_BOOT_PLISIM && !dnx_data_elk.connectivity.blackhole_get(unit)
        && !dnx_kbp_interface_enabled(unit) && !stif_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "No ELK ports were configured. KBP interface is not initialized.\n");
    }

    /** Set device properties and perform device lock. */
    SHR_IF_ERR_EXIT(kbp_mngr_device_set_properties_and_lock(unit));

    /** Create the 0b ad_entry handles for databases. */
    if (kbp_mngr_split_rpf_in_use(unit))
    {
        SHR_IF_ERR_EXIT(kbp_mngr_create_zero_size_entry_handles(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See kbp_mngr_init() in kbp_mngr.h
 */
shr_error_e
kbp_mngr_init_internal(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "KBP manager init\n")));
    if (!sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(KBP_ACCESS.init(unit));
        SHR_IF_ERR_EXIT(KBP_COMMON_ACCESS.init(unit));
        /** Reading all KBP data from XML (updating DBAL SW tables) */
        SHR_IF_ERR_EXIT(kbp_mngr_fwd_applications_info_init(unit));
        SHR_IF_ERR_EXIT(kbp_mngr_status_update(unit, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_FWD_CONFIG_DONE));
    }

    /** No configuration is required in warmboot. All configurations should already be done in normal boot */

exit:
    SHR_FUNC_EXIT;
}

#else
/* *INDENT-OFF* */
/** dummy prototype to return error if KBP not compiled */

#define DUMMY_PROTOTYPE_ERR_MSG(__func_name__)                                                      \
        SHR_FUNC_INIT_VARS(unit);                                                                   \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "cannot perform %s, KBP lib is not compiled", __func_name__); \
        exit:                                                                                       \
        SHR_FUNC_EXIT;

shr_error_e kbp_mngr_db_create_internal(int unit, dbal_tables_e table_id, uint8 clone_id, uint8 counters_enable, uint32 initial_capacity, int algo_mode, uint8 *db_id){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_db_create_internal")}
shr_error_e kbp_mngr_init_internal(int unit){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_init_internal")}
shr_error_e kbp_mngr_sync_internal(int unit){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_sync_internal")}
shr_error_e kbp_mngr_wb_init_internal(int unit){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_wb_init_internal")}
shr_error_e kbp_mngr_db_key_segment_set(int unit, uint32 db_id, int index, uint32 *segment_type, uint32 *segment_size, char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES]){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_db_key_segment_set")}
shr_error_e kbp_mngr_db_key_segment_get(int unit, uint32 db_id, int index, uint32 *segment_type, uint32 *segment_size, char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES]){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_db_key_segment_get")}
shr_error_e kbp_mngr_master_key_segment_set(int unit, uint32 opcode_id, int index, uint32 *segment_type, uint32 *segment_size, char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES], uint32 *overlay_offset){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_master_key_segment_set")}
shr_error_e kbp_mngr_master_key_segment_get(int unit, uint32 opcode_id, int index, uint32 *segment_type, uint32 *segment_size, char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES], uint32 *overlay_offset){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_master_key_segment_get")}
shr_error_e kbp_mngr_context_to_opcode_set(int unit, uint8 fwd_context, uint8 acl_context,uint8 opcode_id){DUMMY_PROTOTYPE_ERR_MSG("kbp_mngr_context_to_opcode_set")}
/* *INDENT-ON* */
#endif
