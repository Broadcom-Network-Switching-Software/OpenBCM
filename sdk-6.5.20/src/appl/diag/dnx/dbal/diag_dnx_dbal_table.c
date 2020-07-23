/** \file diag_dnx_dbal_table.c
 *
 * Main diagnostics for dbal applications All CLI commands, that
 * are related to DBAL tables, are gathered in this file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGDBALDNX

/*************
 * INCLUDES  *
 */
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <appl/diag/diag.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include "diag_dnx_dbal_internal.h"
#include <appl/diag/shell.h>
#include <appl/diag/cmdlist.h>
#include <appl/diag/bslenable.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/sand/sand_aux_access.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <soc/sand/sand_signals.h>
#include <sal/core/boot.h>
#include "diag_dnx_dbal.h"
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_rhlist.h>
#include <shared/utilex/utilex_str.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>

/*************
 * DEFINES   *
 */

#define DNX_DBAL_FILE_TYPE_UNKNOWN  0
#define DNX_DBAL_FILE_TYPE_XML      1
#define DNX_DBAL_FILE_TYPE_DCSV     2

/*************
 * TYPEDEFS  *
 */
extern const char *strcaseindex(
    const char *s,
    const char *sub);

/*
 * basic dbal entry information
 */
typedef struct
{
    dbal_tables_e table_id;
    CONST dbal_logical_table_t *table;
    uint32 priority;
    uint32 access_id;
    uint32 entry_handle_id;
} entry_struct_t;

/*
 * a structure to keep parsed value from file
 * can be used to store key and mask
 */
typedef struct
{
    sh_sand_param_u param1;
    sh_sand_param_u param2;
} multi_param_struct_t;

/*************
 * FUNCTIONS *
 */

 /*
  * LOCAL DIAG PACK:
  * {
  */
/**********************************************************************************************************************
 *  DBAL DIAGNOSTIC PACK:
 *  STRUCTURE:
 *  MAIN MENU CMD: define under sh_dnx_dbal_cmds
 *     TABLE - SUB MENU defined under sh_dnx_dbal_table_cmds
 *     ENTRY - SUB MENU defined under sh_dnx_dbal_entry_cmds
 *     TYPES - SUB MENU defined under sh_dnx_dbal_field_types_cmds
 *     LABELSDUMP
 *     LOGSEVERITY
 *     HANDLESSTATUS
 */

/**********************************************************************************************************************
 *  DBAL DIAGNOSTIC PACK:
 *  TABLE SUB MENU function & infrastructure Definitions - START
 *  STURCTURE:
 *  1. cmd function definition
 *  2. cmd option and man table
 *  3. TABLE SUB MENU cmd table
 */
static shr_error_e
diag_dbal_table_list_dump(
    int unit,
    dbal_labels_e label,
    dbal_maturity_level_e maturity_level,
    dbal_table_status_e status,
    dbal_access_method_e access_method,
    int min_num_operations,
    int is_image_specific,
    int is_dirty,
    int payload_size,
    sh_sand_control_t * sand_control)
{
    int table_id, jj, nof_tables = 0;
    int char_count = 0;
    uint8 to_print;
    CONST dbal_logical_table_t *table;
    char *str;
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE];

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    str = buffer_to_print;

    char_count = sal_sprintf(str, "%s", "LOGICAL TABLES");
    str += char_count;

    if (label != DBAL_LABEL_NONE)
    {
        char_count = sal_sprintf(str, ", with %s %s", "LABEL:", dbal_label_to_string(unit, label));
        str += char_count;
    }
    if (maturity_level != DBAL_NOF_MATURITY_LEVELS)
    {
        char_count = sal_sprintf(str, ", with %s %s", "MATURITY:", dbal_maturity_level_to_string(unit, maturity_level));
        str += char_count;
    }
    if (status != DBAL_NOF_TABLE_STATUS)
    {
        char_count = sal_sprintf(str, ", with %s %s", "STATUS:", dbal_table_status_to_string(unit, status));
        str += char_count;
    }

    if (access_method != DBAL_NOF_ACCESS_METHODS)
    {
        char_count = sal_sprintf(str, ", with %s %s", "ACCESS:", dbal_access_method_to_string(unit, access_method));
        str += char_count;
    }
    PRT_TITLE_SET("%s", buffer_to_print);

    PRT_COLUMN_ADD("Table Name");
    PRT_COLUMN_ADD("Access Type");
    PRT_COLUMN_ADD("Maturity");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Status");
    PRT_COLUMN_ADD("Sets");
    PRT_COLUMN_ADD("Gets");
    PRT_COLUMN_ADD("Payload Size");
    PRT_COLUMN_ADD("Key Size");
    PRT_COLUMN_ADD("Image Specific");

    for (table_id = 0; table_id < dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit); table_id++)
    {
        dbal_table_status_e table_status;
        to_print = TRUE;

        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

        if ((table_status == DBAL_TABLE_NOT_INITIALIZED))
        {
            continue;
        }

        if (label != DBAL_LABEL_NONE)
        {
            for (jj = 0; jj < table->nof_labels; jj++)
            {
                if ((table->table_labels[jj] == label))
                {
                    break;
                }
            }
            if (jj == table->nof_labels)
            {
                to_print = FALSE;
            }
        }

        if (is_image_specific)
        {
            if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_IMAGE_SPECIFIC_TABLE))
            {
                to_print = FALSE;
            }
        }

        if (is_dirty)
        {
            if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_TABLE_DIRTY))
            {
                to_print = FALSE;
            }
        }

        if (min_num_operations != 0)
        {
            int nof_get_operations = 0, nof_set_operations = 0;
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_set_operations.get(unit, table_id, &nof_set_operations));
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_get_operations.get(unit, table_id, &nof_get_operations));
            if ((nof_set_operations + nof_get_operations) < min_num_operations)
            {
                to_print = FALSE;
            }
        }
        if (payload_size != 0)
        {
            if (table->max_payload_size < payload_size)
            {
                to_print = FALSE;
            }
        }

        if ((maturity_level != DBAL_NOF_MATURITY_LEVELS) && (table->maturity_level != maturity_level))
        {
            to_print = FALSE;
        }

        if ((access_method != DBAL_NOF_ACCESS_METHODS) && (table->access_method != access_method))
        {
            to_print = FALSE;
        }

        if ((status != DBAL_NOF_TABLE_STATUS) && (table_status != status))
        {
            to_print = FALSE;
        }

        if (to_print)
        {
            int nof_get_operations, nof_set_operations;

            nof_tables++;

            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_set_operations.get(unit, table_id, &nof_set_operations));
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_get_operations.get(unit, table_id, &nof_get_operations));
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", table->table_name);
            PRT_CELL_SET("%s", dbal_access_method_to_string(unit, table->access_method));
            if (table->maturity_level == DBAL_MATURITY_LOW)
            {
                PRT_CELL_SET("Low");
            }
            else if (table->maturity_level == DBAL_MATURITY_PARTIALLY_FUNCTIONAL)
            {
                PRT_CELL_SET("Medium");
            }
            else
            {
                PRT_CELL_SET("High");
            }
            PRT_CELL_SET("%s", dbal_table_type_to_string(unit, table->table_type));
            PRT_CELL_SET("%s", dbal_table_status_to_string(unit, table_status));
            PRT_CELL_SET("%d", nof_set_operations);
            PRT_CELL_SET("%d", nof_get_operations);
            PRT_CELL_SET("%d", table->max_payload_size);
            PRT_CELL_SET("%d", table->key_size);
            PRT_CELL_SET("%s",
                         (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_IMAGE_SPECIFIC_TABLE) ? "YES" : "NO"));

        }
    }

    PRT_INFO_ADD("Total Tables: %d", nof_tables);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Return the next table id that matches to the input string,
* (next refers to the curr_table_id) if the subsstring is fully
* match to the table name  than is_full_match=1 use
* curr_table_id = -1 to start form the first existing table
*/
cmd_result_t
diag_dbal_table_string_to_next_table_id_get(
    int unit,
    char *substr_match,
    dbal_tables_e curr_table_id,
    dbal_tables_e * table_id,
    int *is_full_match)
{
    int iter;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    *table_id = dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit);
    *is_full_match = 0;

    curr_table_id++;

    if ((substr_match == NULL) || (curr_table_id > dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input substr_match not exists or table_id too big\n");
    }

    for (iter = curr_table_id; iter < dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit); iter++)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, iter, &table));

        if (!sal_strcasecmp(substr_match, table->table_name))
        {
            *is_full_match = 1;
            *table_id = iter;
            break;
        }

        if (strcaseindex(table->table_name, substr_match))
        {
            *table_id = iter;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** returns the table ID that matches to the string, if there is more than one table ID that matches to the string
 *  dump all table IDs that related if not unique table was found return table_id = DBAL_NOF_DYNAMIC_AND_STATIC_TABLES */
shr_error_e
diag_dbal_table_from_string(
    int unit,
    char *table_name,
    dbal_tables_e * table_id,
    sh_sand_control_t * sand_control)
{
    int is_full_match = 0;
    dbal_tables_e first_table_id;
    int nof_tables_found = 0;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Possible tables");

    PRT_COLUMN_ADD("Table Name");

    diag_dbal_table_string_to_next_table_id_get(unit, table_name, -1, table_id, &is_full_match);
    if ((*table_id) == dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        LOG_CLI((BSL_META("No matching tables found\n\n")));
        SHR_EXIT();
    }

    first_table_id = (*table_id);

    while ((*table_id) != dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        nof_tables_found++;

        /** full match - use this table */
        if (is_full_match)
        {
            SHR_EXIT();
        }
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", dbal_logical_table_to_string(unit, (*table_id)));
        diag_dbal_table_string_to_next_table_id_get(unit, table_name, (*table_id), table_id, &is_full_match);
    }

    /** only one table found use it */
    if (nof_tables_found == 1)
    {
        (*table_id) = first_table_id;
        SHR_EXIT();
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
* \brief
* return the capacity estimation for a specific table. for direct tables the estimation is according to the key size. for
* non-direct it depends on the HW and more parameters. there is only exception for LIF tables. if the
* estimated_nof_entries returns 0 it means that number of entries cannot be estimated.
 */

shr_error_e
diag_dnx_dbal_table_capacity_estimate(
    int unit,
    dbal_tables_e table_id,
    int result_type,
    int *estimated_nof_entries)
{
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    if (table->access_method == DBAL_ACCESS_METHOD_KBP)
    {
        uint32 db_id;
        uint32 num_of_entries;
        uint32 estimated_capacity = 0;

        SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, table_id, &db_id, NULL));
        SHR_IF_ERR_EXIT(kbp_mngr_db_capacity_get(unit, db_id, &num_of_entries, &estimated_capacity));
        *estimated_nof_entries = estimated_capacity;
    }
    else if (DBAL_TABLE_IS_NONE_DIRECT(table))
    {
        if (table->access_method == DBAL_ACCESS_METHOD_MDB)
        {
            /** Do not include core_id field for DPC tables in the key size  */
            int key_size = table->key_size - table->core_id_nof_bits;
            int payload_size;

            /** calculating the effective key size for KAPs, since the table->key_size is the max key size  */
            if (table->table_type == DBAL_TABLE_TYPE_LPM)
            {
                int ii;
                key_size = 0;
                for (ii = 0; ii < table->nof_key_fields; ii++)
                {
                    key_size += table->keys_info[ii].field_nof_bits;
                }
            }

            /** Do not include result type that is mapped to SW in the payload size  */
            payload_size = table->multi_res_info[result_type].entry_payload_size;
            if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
            {
                payload_size = payload_size - table->multi_res_info[0].results_info[0].field_nof_bits;
            }

            SHR_IF_ERR_EXIT(mdb_get_capacity_estimate(unit, table->physical_db_id[0], key_size, payload_size,
                                                      table->app_id, table->app_id_size, estimated_nof_entries));
        }
        else
        {
            (*estimated_nof_entries) = table->max_capacity;
        }
    }
    else
    {
        (*estimated_nof_entries) = table->max_capacity;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_file_entry_prepare_for_commit(
    int unit,
    entry_struct_t * entry)
{

    int core = DBAL_CORE_ALL;
    dbal_entry_action_flags_e commit_flags = DBAL_COMMIT;

    SHR_FUNC_INIT_VARS(unit);

    if (DBAL_TABLE_IS_TCAM(entry->table))
    {
        if (DBAL_TABLE_IS_TCAM_BY_ID(entry->table, 1))
        {
            if (entry->table->access_method == DBAL_ACCESS_METHOD_MDB)
            {
                SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create
                                (unit, core, entry->table_id, entry->priority, &(entry->access_id)));
                SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry->entry_handle_id, entry->access_id));
            }
            else if (entry->table->access_method == DBAL_ACCESS_METHOD_KBP)
            {
                SHR_IF_ERR_EXIT(dbal_entry_attribute_set
                                (unit, entry->entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, entry->priority));

                if (entry->access_id == 0xffffffff)
                {
                    /** Access id not received or not legal */
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal access id\n");
                }
                SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry->entry_handle_id, entry->access_id));
            }
            else
            {
                commit_flags |= DBAL_COMMIT_OVERRIDE_DEFAULT;
                if (entry->access_id == 0xffffffff)
                {
                    /** Access id not received or not legal */
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal access id\n");
                }
                SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry->entry_handle_id, entry->access_id));

            }
        }
        else
        {
            if (commit_flags == DBAL_COMMIT)
            {
                if (entry->table->access_method == DBAL_ACCESS_METHOD_MDB)
                {
                    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create
                                    (unit, core, entry->table_id, entry->priority, &(entry->access_id)));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry->entry_handle_id, entry->access_id));
                }
            }

        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* comit xml/dcsv file dbal entries results
 */
static shr_error_e
dbal_file_entry_result_commit(
    int unit,
    char *name,
    char *value,
    entry_struct_t * entry)
{
    dbal_fields_e field_id;
    uint32 res_type = 0;
    char cmd[] = "";
    dbal_tables_e table_id = entry->table_id;
    dbal_field_print_type_e field_print_type;
    multi_param_struct_t parsed_param;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, name, &field_id));

    if (field_id == DBAL_FIELD_RESULT_TYPE)
    {

        field_print_type = DBAL_FIELD_PRINT_TYPE_STR;
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_fields_print_type_get(unit, field_id, &field_print_type));
    }

    /** Parse rule value, from string to any sal_type */
    SHR_IF_ERR_EXIT(sh_sand_value_get
                    (unit, dbal_field_type_to_sal_field_type[field_print_type], value, &(parsed_param.param1),
                     &dbal_test_enum_table[0]));

    DBAL_ENTRY_COMMIT_VALUE_FIELD(unit, (entry->entry_handle_id), field_id, res_type, 0, (&parsed_param));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* comit xml/dcsv file dbal entries keys
* handle
 */
static shr_error_e
dbal_file_entry_key_commit(
    int unit,
    char *name,
    char *value,
    char *mask,
    entry_struct_t * entry)
{
    dbal_fields_e field_id;
    int mask_present;
    dbal_field_print_type_e field_print_type;
    multi_param_struct_t parsed_param;

    SHR_FUNC_INIT_VARS(unit);

    mask_present = ((mask != NULL) && (mask[0] != 0));

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, name, &field_id));

    SHR_IF_ERR_EXIT(dbal_fields_print_type_get(unit, field_id, &field_print_type));

    /** Parse rule value, from string to any sal_type */
    SHR_IF_ERR_EXIT(sh_sand_value_get
                    (unit, dbal_field_type_to_sal_field_type[field_print_type], value, &(parsed_param.param1),
                     &dbal_test_enum_table[0]));

    if (mask_present && (DBAL_TABLE_IS_TCAM(entry->table) || (entry->table->table_type == DBAL_TABLE_TYPE_LPM)))
    {
        /** Get Key fields Params with mask */
        SHR_IF_ERR_EXIT(sh_sand_value_get
                        (unit, dbal_field_type_to_sal_field_type[field_print_type], mask, &(parsed_param.param2),
                         &dbal_test_enum_table[0]));
        DBAL_ENTRY_COMMIT_KEY_FIELD_WITH_MASK(unit, (entry->entry_handle_id), field_id, (&parsed_param));
    }
    else
    {
        /** Get Key fields Params */
        DBAL_ENTRY_COMMIT_KEY_FIELD(unit, (entry->entry_handle_id), field_id, (&parsed_param));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* parsing xml file dbal entries results and setting
* them in in entry handle for commit
 */
static shr_error_e
dbal_xml_file_entry_results_update(
    int unit,
    void *node,
    entry_struct_t * entry,
    sh_sand_control_t * sand_control)
{
    void *res_sub;
    void *cur_sub;
    char name[RHSTRING_MAX_SIZE];
    char value[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(name, 0, sizeof(name));
    sal_memset(value, 0, sizeof(value));

    res_sub = dbx_xml_child_get_first(node, "Result");

    if (res_sub == NULL)
    {
        SHR_EXIT();
    }

    RHDATA_ITERATOR(cur_sub, res_sub, "Field")
    {
        RHDATA_GET_STR_CONT(cur_sub, "Name", name);
        RHDATA_GET_STR_CONT(cur_sub, "Value", value);

        SHR_IF_ERR_EXIT(dbal_file_entry_result_commit(unit, name, value, entry));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* parsing xml file dbal entries keys amd setting them into entry
* handle
 */
static shr_error_e
dbal_xml_file_entry_keys_update(
    int unit,
    void *node,
    entry_struct_t * entry,
    sh_sand_control_t * sand_control)
{
    void *key_sub;
    void *cur_sub;
    char name[RHSTRING_MAX_SIZE];
    char value[RHSTRING_MAX_SIZE];
    char mask[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(name, 0, sizeof(name));
    sal_memset(value, 0, sizeof(value));
    sal_memset(mask, 0, sizeof(mask));

    if ((key_sub = dbx_xml_child_get_first(node, "Key")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Empty key was provided \n");
    }

    RHDATA_ITERATOR(cur_sub, key_sub, "Field")
    {
        RHDATA_GET_STR_CONT(cur_sub, "Name", name);
        RHDATA_GET_STR_CONT(cur_sub, "Value", value);
        RHDATA_GET_STR_DEF_NULL(cur_sub, "Mask", mask);

        SHR_IF_ERR_EXIT(dbal_file_entry_key_commit(unit, name, value, mask, entry));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* parsing xml file dbal entries and commiting them into dbal
 */
static shr_error_e
dbal_xml_file_entry_parse(
    int unit,
    void *node,
    entry_struct_t * entry,
    sh_sand_control_t * sand_control)
{
    void *cur_sub;
    dbal_entry_action_flags_e commit_flags = DBAL_COMMIT | DBAL_COMMIT_OVERRIDE_DEFAULT;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, entry->table_id, &(entry->entry_handle_id)));

    RHDATA_ITERATOR(cur_sub, node, "Entry")
    {

        RHDATA_GET_INT_DEF(cur_sub, "Priority", entry->priority, 0);
        RHDATA_GET_INT_DEF(cur_sub, "AccessId", entry->access_id, 0xffffffff);

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, entry->table_id, entry->entry_handle_id));

        SHR_IF_ERR_EXIT(dbal_file_entry_prepare_for_commit(unit, entry));

        SHR_IF_ERR_EXIT(dbal_xml_file_entry_keys_update(unit, cur_sub, entry, sand_control));

        SHR_IF_ERR_EXIT(dbal_xml_file_entry_results_update(unit, cur_sub, entry, sand_control));

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry->entry_handle_id, commit_flags));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* parsing xml file and populating dbal tables.
* entry structure can have additonal attributes such as priority
* and accessid (for TCAM tbles). Also TCAM fields can have a
* mask attribute.
* The file structure is as following:
*
*\<Root\>
*    \<Entries Table="SOME_DB"\>
*        \<Entry\>
*            \<Key\>
*                \<Field Name="KEY1" Value="1" /\>
*                \<Field Name="KEY2" Value="2"/\>
*            \</Key\>
*            \<Result\>
*                \<Field Name="RES1" Value="3"/\>
*            \</Result\>
*        \</Entry\>
*        \<Entry\>
*            \<Key\>
*                \<Field Name="KEY1" Value="2" /\>
*                \<Field Name="KEY2" Value="3"/\>
*            \</Key\>
*            \<Result\>
*                \<Field Name="RES1" Value="4"/\>
*            \</Result\>
*        \</Entry\>
*    \</Entries\>
*    \<Entries Table="SOME_TCAM_DB"\>
*        \<\<Entry Priority="1"\>\>
*            \<Key\>
*                \<Field Name="KEY1" Value="1" Mask="1" /\>
*                \<Field Name="KEY2" Value="2" Mask="0" /\>
*            \</Key\>
*            \<Result\>
*                \<Field Name="RES1" Value="3"/\>
*            \</Result\>
*        \</Entry\>
*    \</Entries\>
*    \<Entries Table="SOME_TCAM_DB"\>
*        \<\<Entry AccessId="1"\>\>
*            \<Key\>
*                \<Field Name="KEY1" Value="1" Mask="1" /\>
*                \<Field Name="KEY2" Value="2" Mask="0" /\>
*            \</Key\>
*        \</Entry\>
*    \</Entries\>
*\</Root\>\
 */

shr_error_e
dbal_populate_tables_from_xml_file(
    int unit,
    char *filename,
    sh_sand_control_t * sand_control)
{
    void *curTop, *cur_sub;
    entry_struct_t entry;
    char table_name[RHNAME_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    if ((curTop = dbx_file_get_xml_top(unit, filename, "Root", CONF_OPEN_CURRENT_LOC)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Cannot find input xml file in: %s \n", filename);
    }

    RHDATA_ITERATOR(cur_sub, curTop, "Entries")
    {
        sal_memset(&entry, 0, sizeof(entry));
        sal_memset(table_name, 0, sizeof(table_name));
        RHDATA_GET_STR_CONT(cur_sub, "Table", table_name);

        SHR_IF_ERR_EXIT(diag_dbal_table_from_string(unit, table_name, &(entry.table_id), sand_control));

        if (entry.table_id == dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
        {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, entry.table_id, &(entry.table)));
        DIAG_DBAL_TABLE_STATUS_VALIDATE(entry.table_id);

        SHR_IF_ERR_EXIT(dbal_xml_file_entry_parse(unit, cur_sub, &entry, sand_control));
    }

exit:
    if (curTop != NULL)
    {
        dbx_xml_top_close(curTop);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_dcsv_file_get_value_from_string(
    int unit,
    char *find_str,
    char **fields_list,
    char **values_list,
    uint32 num_of_values,
    sh_sand_param_u * val,
    sh_sand_control_t * sand_control)
{
    int i;
    dbal_fields_e field_id;
    dbal_field_print_type_e field_print_type;
    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < num_of_values; i++)
    {
        if (sal_strcasecmp(find_str, fields_list[i]) == 0)
        {
            if ((sal_strcasecmp("Priority", find_str) == 0) || (sal_strcasecmp("AccessId", find_str) == 0))
            {
                SHR_IF_ERR_EXIT(utilex_str_stoul(values_list[i], &(val->val_uint32)));
                SHR_EXIT();
            }
            else if (sal_strcasecmp("N/A", values_list[i]) != 0)
            {

                SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, fields_list[i], &field_id));
                if (field_id == DBAL_FIELD_RESULT_TYPE)
                {

                    field_print_type = DBAL_FIELD_PRINT_TYPE_STR;
                }
                else
                {
                    SHR_IF_ERR_EXIT(dbal_fields_print_type_get(unit, field_id, &field_print_type));
                }
                SHR_IF_ERR_EXIT(sh_sand_value_get
                                (unit, dbal_field_type_to_sal_field_type[field_print_type], values_list[i], val,
                                 &dbal_test_enum_table[0]));
                SHR_EXIT();
            }
        }
    }
    SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_dcsv_file_entry_populate(
    int unit,
    char *field,
    char *value,
    char *mask,
    entry_struct_t * entry,
    sh_sand_control_t * sand_control)
{
    uint8 is_key;
    dbal_fields_e field_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field, &field_id));
    SHR_IF_ERR_EXIT(dbal_tables_field_is_key_get(unit, entry->table_id, field_id, &is_key));
    if (is_key == TRUE)
    {
        SHR_IF_ERR_EXIT(dbal_file_entry_key_commit(unit, field, value, mask, entry));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_file_entry_result_commit(unit, field, value, entry));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_dcsv_file_entry_parse(
    int unit,
    entry_struct_t * entry,
    char **fields_list,
    char **values_list,
    uint32 num_of_values,
    sh_sand_control_t * sand_control)
{
    int i;
    shr_error_e rv;
    sh_sand_param_u val;
    dbal_entry_action_flags_e commit_flags = DBAL_COMMIT | DBAL_COMMIT_OVERRIDE_DEFAULT;
    char *mask;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_dcsv_file_get_value_from_string(unit, "Priority", fields_list, values_list, num_of_values, &val,
                                              sand_control);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv == _SHR_E_NOT_FOUND)
    {
        val.val_uint32 = 0;
    }
    entry->priority = val.val_uint32;

    rv = dbal_dcsv_file_get_value_from_string(unit, "AccessId", fields_list, values_list, num_of_values, &val,
                                              sand_control);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv == _SHR_E_NOT_FOUND)
    {
        val.val_uint32 = 0xffffffff;
    }
    entry->access_id = val.val_uint32;

    for (i = 0; i < num_of_values; i++)
    {
        if (sal_strcasecmp("Priority", fields_list[i]) == 0 || sal_strcasecmp("AccessId", fields_list[i]) == 0
            || sal_strcasecmp("Mask", fields_list[i]) == 0 || sal_strcasecmp("N/A", values_list[i]) == 0)
        {
            continue;
        }
        mask = NULL;
        if ((i + 1 < num_of_values) && sal_strcasecmp("Mask", fields_list[i + 1]) == 0)
        {
            mask = values_list[i + 1];
        }
        SHR_IF_ERR_EXIT(dbal_dcsv_file_entry_populate(unit, fields_list[i], values_list[i], mask, entry, sand_control));

    }
    SHR_IF_ERR_EXIT(dbal_file_entry_prepare_for_commit(unit, entry));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry->entry_handle_id, commit_flags));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_populate_tables_from_dcsv_file(
    int unit,
    char *filename,
    sh_sand_control_t * sand_control)
{
    char line[SH_SAND_MAX_TOKEN_SIZE];
    char table_name[RHNAME_MAX_SIZE];
    char *tmp_str;
    int expect_table_header = 0;
    uint32 num_columns_fields = 0;
    uint32 num_columns_values = 0;
    char **fields_list = NULL;
    char **values_list = NULL;
    entry_struct_t entry;
    shr_error_e rv = _SHR_E_NONE;

    FILE *file;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&entry, 0, sizeof(entry_struct_t));


    if ((file = sal_fopen(filename, "rt")) == NULL)
    {
        SHR_ERR_EXIT(rv, "Cannot open file\n");
    }

    while (sal_fgets(line, SH_SAND_MAX_TOKEN_SIZE - 1, file))
    {
        tmp_str = line;

        /*
         * skip leading whitespace
         */
        utilex_str_clean(&tmp_str);
        utilex_str_white_spaces_remove(tmp_str);

        /*
         * ignore comment or empty lines
         */
        if (tmp_str[0] == '#' || tmp_str[0] == 0)
        {
            continue;
        }

        /*
         * this line is expected to be headrs of AppDB
         */
        if (expect_table_header)
        {
            if ((fields_list = utilex_str_split(tmp_str, ",", 20, &num_columns_fields)) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to parse dscv table fields string: %s\n", line);
            }
            expect_table_header = 0;
            continue;
        }

        /*
         * beginning of new table
         */
        if (sal_strncmp("Table=", tmp_str, strlen("Table=")) == 0)
        {

            /*
             * free previous table handle if allocated before
             */
            if (entry.table_id)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry.entry_handle_id));
            }

            sal_memset(table_name, 0, sizeof(table_name));
            sal_strcpy(table_name, tmp_str + sal_strlen("Table="));

            utilex_str_split_free(fields_list, num_columns_fields);
            fields_list = NULL;

            sal_memset(&entry, 0, sizeof(entry_struct_t));

            SHR_IF_ERR_EXIT(diag_dbal_table_from_string(unit, table_name, &(entry.table_id), sand_control));

            if (entry.table_id == dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
            {
                SHR_EXIT();
            }

            SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, entry.table_id, &(entry.table)));
            DIAG_DBAL_TABLE_STATUS_VALIDATE(entry.table_id);

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, entry.table_id, &(entry.entry_handle_id)));

            /*
             * next line will be table header with fields names
             */
            expect_table_header = 1;
            continue;
        }

        /*
         * parse and insert entries
         */
        if ((values_list = utilex_str_split(tmp_str, ",", 20, &num_columns_values)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to parse dcsv string: %s\n", line);
        }

        if (num_columns_values != num_columns_fields)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Inconsistent number of columns in: %s\n", line);
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, entry.table_id, entry.entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_dcsv_file_entry_parse
                        (unit, &entry, fields_list, values_list, num_columns_values, sand_control));

        utilex_str_split_free(values_list, num_columns_values);
        values_list = NULL;

    }

exit:
    if (file != NULL)
    {
        utilex_str_split_free(fields_list, num_columns_fields);
        utilex_str_split_free(values_list, num_columns_values);
        sal_fclose(file);
    }
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  indentify the file type based on its extension
 */

static int
diag_dbal_file_get_type(
    char *filename)
{
    char **tokens;
    uint32 realtokens = 0;
    char *extension;
    int ret = DNX_DBAL_FILE_TYPE_UNKNOWN;

    if ((tokens = utilex_str_split(filename, ".", 10, &realtokens)) == NULL)
    {
        LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(0, "Failed to split:%s\n"), filename));
        return ret;
    }

    if (realtokens > 1)
    {
        extension = tokens[realtokens - 1];
        if (!sal_strcmp(extension, "dcsv") || !sal_strcmp(extension, "txt"))
            ret = DNX_DBAL_FILE_TYPE_DCSV;
        else if (!sal_strcmp(extension, "xml"))
            ret = DNX_DBAL_FILE_TYPE_XML;
    }

    utilex_str_split_free(tokens, realtokens);
    return ret;
}

/**
* \brief
*  read commit list from file
*  input arg should be valid file path.
 */

static shr_error_e
diag_dbal_table_populate(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *name;
    shr_error_e rv = _SHR_E_NONE;
    int filetype;

    SHR_FUNC_INIT_VARS(unit);


    SH_SAND_GET_STR("filename", name);

    if (ISEMPTY(name))
    {
        SHR_ERR_EXIT(rv, "File name does not exist \n");
    }

    filetype = diag_dbal_file_get_type(name);
    if (filetype == DNX_DBAL_FILE_TYPE_DCSV)
    {
        SHR_IF_ERR_EXIT(dbal_populate_tables_from_dcsv_file(unit, name, sand_control));
    }
    else if (filetype == DNX_DBAL_FILE_TYPE_XML)
    {
        SHR_IF_ERR_EXIT(dbal_populate_tables_from_xml_file(unit, name, sand_control));
    }
    else
    {
        SHR_ERR_EXIT(rv, "Unsupported file type\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* dump logical tables.
* input arg should be valid label_name / table_name.
* dump all logical tables names that related to specific label_id,
* if label_id = DBAL_LABEL_NONE dump all tables.
* if table_name match to more than one table name dump brief information of all matching tables
* if full match valid table_name exist dump extended table information
 */
static shr_error_e
diag_dbal_table_info(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int min_num_operations = 0, is_image_specific = 0, is_dirty = 0, payload_size = 0;
    dbal_tables_e table_id;
    dbal_labels_e label;
    dbal_maturity_level_e maturity_level;
    dbal_access_method_e access_method;
    dbal_table_status_e table_status;
    char *table_name = NULL, *label_name = NULL, *maturity_str = NULL, *table_status_str = NULL, *access_method_str =
        NULL;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("table", table_name);
    SH_SAND_GET_STR("label", label_name);
    SH_SAND_GET_STR("maturity", maturity_str);
    SH_SAND_GET_STR("status", table_status_str);
    SH_SAND_GET_STR("access", access_method_str);
    SH_SAND_GET_INT32("operations", min_num_operations);
    SH_SAND_GET_INT32("image_specific", is_image_specific);
    SH_SAND_GET_INT32("DIRTY", is_dirty);
    SH_SAND_GET_INT32("size", payload_size);

    if (ISEMPTY(table_name))
    {
        if (ISEMPTY(label_name))
        {
            label = DBAL_LABEL_NONE;
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_label_string_to_id(unit, label_name, &label));
        }

        if (ISEMPTY(maturity_str))
        {
            maturity_level = DBAL_NOF_MATURITY_LEVELS;
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_maturity_level_string_to_id(unit, maturity_str, &maturity_level));
        }

        if (ISEMPTY(access_method_str))
        {
            access_method = DBAL_NOF_ACCESS_METHODS;
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_access_method_string_to_id(unit, access_method_str, &access_method));
        }

        if (ISEMPTY(table_status_str))
        {
            table_status = DBAL_NOF_TABLE_STATUS;
        }
        else
        {
            if (sal_strcasecmp(table_status_str, "initialized") == 0)
            {
                table_status = DBAL_TABLE_INITIALIZED;
            }
            else if (sal_strcasecmp(table_status_str, "hwerr") == 0)
            {
                table_status = DBAL_TABLE_HW_ERROR;
            }
            else if (sal_strcasecmp(table_status_str, "NotInit") == 0)
            {
                table_status = DBAL_TABLE_NOT_INITIALIZED;
            }
            else if (sal_strcasecmp(table_status_str, "incompatibleImage") == 0)
            {
                table_status = DBAL_TABLE_INCOMPATIBLE_IMAGE;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown table status %s options untouched, NotInit, Init, api, hwerr\n",
                             table_status_str);
            }
        }

        SHR_IF_ERR_EXIT(diag_dbal_table_list_dump
                        (unit, label, maturity_level, table_status, access_method, min_num_operations,
                         is_image_specific, is_dirty, payload_size, sand_control));
    }
    else
    {
        SHR_IF_ERR_EXIT(diag_dbal_table_from_string(unit, table_name, &table_id, sand_control));
        if (table_id == dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
        {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(diag_dbal_logical_table_dump(unit, table_id, 1, sand_control));
        SHR_EXIT();
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dbal_table_indication(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    CONST dbal_logical_table_t *table;
    dbal_tables_e table_id;
    dbal_table_indications_e indication_type;
    int value = 0;
    char *table_name = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("table", table_name);
    SH_SAND_GET_ENUM("indication", indication_type);
    SH_SAND_GET_BOOL("value", value);

    if (ISEMPTY(table_name))
    {
        LOG_CLI((BSL_META("Table must be set\n")));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(diag_dbal_table_from_string(unit, table_name, &table_id, sand_control));
    if (table_id != dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
        if (indication_type == DBAL_NOF_TABLE_IND)
        {
            SHR_IF_ERR_EXIT(diag_dbal_table_indications_dump(unit, table_id, table, sand_control));
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_tables_indication_set(unit, table_id, indication_type, value));
            LOG_CLI((BSL_META("Table indication was updated\n")));
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static void
diag_dbal_table_iterator_rule_string_parse(
    char *rule_str,
    dbal_condition_types_e * cond,
    char **rule_val_str)
{
    if (rule_str[0] == '=')
    {
        *cond = DBAL_CONDITION_EQUAL_TO;
        *rule_val_str = rule_str + 1;
    }
    else if ((rule_str[0] == '!') && (rule_str[1] == '='))
    {
        *cond = DBAL_CONDITION_NOT_EQUAL_TO;
        *rule_val_str = rule_str + 2;
    }
    else if (rule_str[0] == '>')
    {
        *cond = DBAL_CONDITION_BIGGER_THAN;
        *rule_val_str = rule_str + 1;
    }
    else if (rule_str[0] == '<')
    {
        *cond = DBAL_CONDITION_LOWER_THAN;
        *rule_val_str = rule_str + 1;
    }
    else if (sal_strcmp(rule_str, "odd") == 0)
    {
        *cond = DBAL_CONDITION_IS_ODD;
    }
    else if (sal_strcmp(rule_str, "even") == 0)
    {
        *cond = DBAL_CONDITION_IS_EVEN;
    }
    else
    {
        *cond = DBAL_CONDITION_NONE;
        *rule_val_str = NULL;
    }
}

static shr_error_e
diag_dbal_table_cmd_iterator_rule_get(
    int unit,
    char *keyword,
    sal_field_type_e * type_p,
    uint32 *id_p,
    void **ext_ptr_p)
{
    dbal_fields_e field_id;
    shr_error_e rc;

    SHR_FUNC_INIT_VARS(unit);

    rc = dbal_field_string_to_id_no_error(unit, keyword, &field_id);

    if (type_p != NULL)
    {
        *type_p = SAL_FIELD_TYPE_STR;
    }
    if (id_p != NULL)
    {
        if (rc != _SHR_E_NOT_FOUND)
        {
            *id_p = field_id;
        }
        else
        {
            *id_p = DBAL_FIELD_EMPTY;
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dbal_table_iterator_rules_read_and_set(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e table_id,
    int *nof_rules,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    CONST dbal_logical_table_t *table;
    uint8 is_key = TRUE, hit_bit_rule = FALSE;
    int num_of_rules = 0;
    char *rule_str;
    char *rule_val_str = "0";
    char *rule_name_str;
    dbal_condition_types_e condition = DBAL_CONDITION_NONE;
    dbal_fields_e field_id;
    dbal_field_print_type_e field_print_type;
    sh_sand_param_u parsed_value;
    sh_sand_arg_t *sand_arg;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /** Add iterator Rules */
    if (sand_control)
    {
        SH_SAND_GET_ITERATOR(sand_arg)
        {
            int strnlen_rule_name_str;
            field_id = SH_SAND_GET_ID(sand_arg);
            rule_str = SH_SAND_ARG_STR(sand_arg);
            rule_name_str = SH_SAND_GET_NAME(sand_arg);
            strnlen_rule_name_str = sal_strnlen(rule_name_str, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);

            /**Parse rule as string, convert rule condition to DBAL condition ENUM*/
            diag_dbal_table_iterator_rule_string_parse(rule_str, &condition, &rule_val_str);
            if (condition == DBAL_CONDITION_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot parse rule for field %s. rule %s\n",
                             dbal_field_to_string(unit, field_id), rule_str);
            }

            /*
             * handle attribute rule
             */
            if (field_id == DBAL_FIELD_EMPTY)
            {
                if (sal_strncasecmp(rule_name_str, "HITBIT", strnlen_rule_name_str) == 0)
                {
                    uint32 attr_val;
                    uint32 attr_type =
                        (DBAL_ENTRY_ATTR_HIT_GET | DBAL_ENTRY_ATTR_HIT_PRIMARY | DBAL_ENTRY_ATTR_HIT_SECONDARY);

                    SHR_IF_ERR_EXIT(utilex_str_stoul(rule_val_str, &attr_val));

                    dbal_iterator_attribute_rule_add(unit, entry_handle_id, attr_type, attr_val);
                    num_of_rules++;
                    hit_bit_rule = TRUE;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot parse rule for attribute %s. Supportes addtibute: HITBIT\n",
                                 rule_name_str);
                }
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_fields_print_type_get(unit, field_id, &field_print_type));
                dbal_builds_enum_sand_table(unit, field_id);

                                /** Parse rule value, from string to any sal_type */
                SHR_IF_ERR_EXIT(sh_sand_value_get(unit, dbal_field_type_to_sal_field_type[field_print_type],
                                                  rule_val_str, &parsed_value, &dbal_test_enum_table[0]));

                SHR_IF_ERR_EXIT(dbal_tables_field_is_key_get(unit, table_id, field_id, &is_key));
                                /** Add rule */
                if (field_print_type == DBAL_FIELD_PRINT_TYPE_MAC)
                {
                    if (is_key == TRUE)
                    {
                        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr8_rule_add
                                        (unit, entry_handle_id, field_id, condition, parsed_value.mac_addr, NULL));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                                        (unit, entry_handle_id, field_id, 0, condition, parsed_value.mac_addr, NULL));
                    }
                }
                else if (field_print_type == DBAL_FIELD_PRINT_TYPE_IPV6)
                {
                    if (is_key == TRUE)
                    {
                        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr8_rule_add
                                        (unit, entry_handle_id, field_id, condition, parsed_value.ip6_addr, NULL));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                                        (unit, entry_handle_id, field_id, 0, condition, parsed_value.ip6_addr, NULL));
                    }
                }
                else
                {
                    if (is_key == TRUE)
                    {
                        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                                        (unit, entry_handle_id, field_id, condition, parsed_value.array_uint32, NULL));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                        (unit, entry_handle_id, field_id, 0, condition, parsed_value.array_uint32,
                                         NULL));
                    }
                }
                num_of_rules++;
            }
        }
    }

    if (!hit_bit_rule)
    {
        uint32 hitbit_en;
        SHR_IF_ERR_EXIT(dbal_tables_indication_get(unit, table_id, DBAL_TABLE_IND_IS_HITBIT_EN, &hitbit_en));
        if (hitbit_en)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_attribute_action_add
                            (unit, entry_handle_id,
                             (DBAL_ENTRY_ATTR_HIT_PRIMARY | DBAL_ENTRY_ATTR_HIT_SECONDARY | DBAL_ENTRY_ATTR_HIT_GET)));
        }
    }

    *nof_rules = num_of_rules;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - clear a full DBAL table
 */
static shr_error_e
diag_dbal_table_clear(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_tables_e table_id;
    char *table_name = NULL;
    uint32 entry_handle_id;
    int nof_rules = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("table", table_name);

    if (ISEMPTY(table_name))
    {
        /**dbal_tables_list_dump(unit, DBAL_LABEL_NONE, sand_control);*/
        DIAG_DBAL_HEADER_DUMP("Example: dbal table Clear Table=<table_name>", "\0");
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(diag_dbal_table_from_string(unit, table_name, &table_id, sand_control));
    if (table_id == dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    SHR_IF_ERR_EXIT(diag_dbal_table_iterator_rules_read_and_set
                    (unit, entry_handle_id, table_id, &nof_rules, args, sand_control));

    if (nof_rules > 0)
    {
        SHR_IF_ERR_EXIT(dbal_iterator_entry_action_add(unit, entry_handle_id, DBAL_ITER_ACTION_DELETE));
        SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   dump all the entries of a logical table in a xml friendly format
 */
static shr_error_e
diag_dbal_table_print_table_in_xml_format(
    int unit,
    dbal_tables_e table_id,
    uint8 export_to_file,
    sh_sand_control_t * sand_control)
{
    CONST dbal_logical_table_t *table;
    dbal_table_status_e table_status;
    dbal_entry_handle_t *entry_handle;
    int entry_col_id, key_col_id, result_col_id;
    int iter;
    uint32 entry_handle_id;
    int is_end, entry_counter = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));

    if ((table->maturity_level <= DBAL_MATURITY_PARTIALLY_FUNCTIONAL) || (table_status != DBAL_TABLE_INITIALIZED))
    {
        SHR_EXIT();
    }
    if (export_to_file)
    {
        LOG_CLI((BSL_META(".")));
    }

    PRT_TITLE_SET("%s", table->table_name);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &entry_col_id, "entry");
    PRT_COLUMN_ADDX(PRT_XML_CHILD, entry_col_id, &key_col_id, "key_field");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, key_col_id, NULL, "key_value");
    PRT_COLUMN_ADDX(PRT_XML_CHILD, entry_col_id, &result_col_id, "result_field");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, result_col_id, NULL, "result_value");

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint32 curr_res_type_index = 0;

        /*
         * Entry
         */
        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
        {
            dbal_field_data_t field_data;
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                                    DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                                    field_data.field_val));
            curr_res_type_index = field_data.field_val[0];
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

        PRT_CELL_SET("%d", entry_counter);
        for (iter = 0; (iter < entry_handle->nof_key_fields) || (iter < entry_handle->nof_result_fields); iter++)
        {
            char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
            if (iter > 0)
            {
                /*
                 * Same entry, new line
                 */
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("");
            }
            if (iter < entry_handle->nof_key_fields)
            {
                char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
                uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
                const char *field_name = dbal_field_to_string(unit, table->keys_info[iter].field_id);

                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id,
                                                                      table->keys_info[iter].field_id, field_val));

                SHR_IF_ERR_EXIT(dbal_field_in_table_printable_string_get(unit, table->keys_info[iter].field_id,
                                                                         table_id, field_val, NULL, 0, TRUE, FALSE,
                                                                         buffer));

                PRT_CELL_SET("%s", field_name);
                PRT_CELL_SET("%s", buffer);
            }
            else
            {
                /*
                 * Out of keys.
                 */
                PRT_CELL_SKIP(2);
            }

            if (iter < table->multi_res_info[curr_res_type_index].nof_result_fields)
            {
                const char *field_name = dbal_field_to_string(unit, entry_handle->value_field_ids[iter]);
                dbal_field_data_t field_data;

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                                        table->
                                                                        multi_res_info[curr_res_type_index].results_info
                                                                        [iter].field_id, INST_SINGLE,
                                                                        field_data.field_val));

                SHR_IF_ERR_EXIT(dbal_field_in_table_printable_string_get
                                (unit, table->multi_res_info[curr_res_type_index].results_info[iter].field_id, table_id,
                                 field_data.field_val, NULL, curr_res_type_index, FALSE, FALSE, buffer));
                PRT_CELL_SET("%s", field_name);
                PRT_CELL_SET("%s", buffer);
            }
            else
            {
                /*
                 * Out of results.
                 */
                PRT_CELL_SKIP(2);
            }
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        entry_counter++;
    }

    if (entry_counter > 0)
    {
        /*
         * Only print the table if any entry was added to it.
         */
        PRT_INFO_ADD("Total Entries: %d", entry_counter);
        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * dump all the non-default entries in xml format
 */
static shr_error_e
diag_dbal_table_xml_format_dump(
    int unit,
    sh_sand_control_t * sand_control)
{
    char *table_name;
    dbal_tables_e table_id;

    /*
     * If file name is given,  the printing is expected to  go to a file.
     * In that case, print some status to the console since the iteration
     * may take a long time.
     */
    char *filename;
    uint8 export_to_file;
    dbal_tables_e start_table = 0;
    dbal_tables_e end_table = dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("table", table_name);
    SH_SAND_GET_STR("file", filename);
    export_to_file = (!ISEMPTY(filename));

    if (sal_strncmp(table_name, "ALL", sal_strlen(table_name)) == 0)
    {
        if (!export_to_file)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "dbal table dump with ALL option must have filename\n");
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(diag_dbal_table_from_string(unit, table_name, &table_id, sand_control));
        if (table_id != dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
        {
            start_table = table_id;
            end_table = table_id + 1;
        }
        else
        {
            SHR_EXIT();
        }
    }

    if (export_to_file)
    {
        LOG_CLI((BSL_META("DBAL dumps tables to %s ...\n."), filename));
    }

    for (table_id = start_table; table_id < end_table; table_id++)
    {
        SHR_IF_ERR_EXIT(diag_dbal_table_print_table_in_xml_format(unit, table_id, export_to_file, sand_control));
    }

    if (export_to_file)
    {
        LOG_CLI((BSL_META("\nDBAL dump - Done.\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Prints all the entries of a table with ID table_id.
 *   Printing format is compact - each entry in a single line.
 */
shr_error_e
diag_dbal_table_entries_print(
    int unit,
    dbal_tables_e table_id,
    uint8 dump_only_key_fields,
    uint8 dump_default_entries,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    CONST dbal_logical_table_t *table;
    int iter, is_end, res_type_index, first_res_index = 0;
    uint32 entry_handle_id;
    int nof_rules = 0;
    dbal_entry_handle_t *entry_handle;
    int entry_counter[DBAL_MAX_NUMBER_OF_RESULT_TYPES] = { 0 };
    uint32 hitbit_en = FALSE;
    dbal_table_status_e table_status;
    /*
     * Up to DBAL_MAX_NUMBER_OF_RESULT_TYPES sub-tables
     */
    PRT_INIT_VARS_PL(DBAL_MAX_NUMBER_OF_RESULT_TYPES);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    DIAG_DBAL_TABLE_STATUS_VALIDATE(table_id);

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));
    if (table_status == DBAL_TABLE_HW_ERROR)
    {
        LOG_CLI((BSL_META("table has HW error cannot perform operation\n\n")));
        SHR_EXIT();
    }

    /** check if the requested table support hitbit */
    SHR_IF_ERR_EXIT(dbal_tables_indication_get(unit, table_id, DBAL_TABLE_IND_IS_HITBIT_EN, &hitbit_en));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    /**DBAL_ITER_MODE_ALL  */
    if (dump_default_entries)
    {
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    }

    SHR_IF_ERR_EXIT(diag_dbal_table_iterator_rules_read_and_set
                    (unit, entry_handle_id, table_id, &nof_rules, args, sand_control));
    /*
     * Skip the RESULT_TYPE as a column
     */
    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        first_res_index = 1;
    }

    /*
     * Print title and columns for each sub-table
     */
    for (res_type_index = 0; res_type_index < table->nof_result_types; res_type_index++)
    {
        PRT_TITLE_SET_PL(res_type_index, "Entries dump table %s", table->table_name);

        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
        {
            PRT_INFO_ADD_PL(res_type_index, "Result type  %s", table->multi_res_info[res_type_index].result_type_name);
        }

        PRT_COLUMN_ADD_FLEX_PL(res_type_index, PRT_FLEX_ASCII, "#");

        /*
         * For TCAM tables - add the access id
         */
        if (DBAL_TABLE_IS_TCAM_BY_ID(table, 0))
        {
            PRT_COLUMN_ADD_FLEX_PL(res_type_index, PRT_FLEX_ASCII, "ACCESS_ID");
        }

        /*
         * Key columns
         */
        for (iter = 0; iter < table->nof_key_fields; iter++)
        {
            PRT_COLUMN_ADD_FLEX_PL(res_type_index, PRT_FLEX_ASCII, "%s",
                                   dbal_field_to_string(unit, table->keys_info[iter].field_id));
        }

        if (!dump_only_key_fields)
        {
            PRT_COLUMN_ADD_FLEX_PL(res_type_index, PRT_FLEX_ASCII, "");
            /*
             * Result columns
             */
            for (iter = first_res_index; iter < table->multi_res_info[res_type_index].nof_result_fields; iter++)
            {
                if (SHR_BITGET
                    (table->multi_res_info[res_type_index].results_info[iter].field_indication_bm,
                     DBAL_FIELD_IND_IS_VALID_INDICATION))
                {
                    PRT_COLUMN_ADD_FLEX_PL(res_type_index, PRT_FLEX_ASCII, "%s (include valid bit)",
                                           dbal_field_to_string(unit,
                                                                table->multi_res_info[res_type_index].
                                                                results_info[iter].field_id));
                }
                else
                {
                    PRT_COLUMN_ADD_FLEX_PL(res_type_index, PRT_FLEX_ASCII, "%s",
                                           dbal_field_to_string(unit,
                                                                table->multi_res_info[res_type_index].
                                                                results_info[iter].field_id));
                }
            }
        }

        if (hitbit_en)
        {
            PRT_COLUMN_ADD_FLEX_PL(res_type_index, PRT_FLEX_ASCII, "HITBIT");
        }
    }

    /*
     * Use iterator to print all entries
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        /*
         * Read the result type index of this entry
         */
        uint32 curr_res_type_index = 0;
        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
        {
            dbal_field_data_t field_data;
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                                    DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                                    field_data.field_val));
            curr_res_type_index = field_data.field_val[0];
        }

        /*
         * New row for the entry
         */
        PRT_ROW_ADD_PL(curr_res_type_index, PRT_ROW_SEP_NONE);
        PRT_CELL_SET_PL(curr_res_type_index, "%d", entry_counter[curr_res_type_index]);

        /*
         * For TCAM tables - add the access id
         */
        if (DBAL_TABLE_IS_TCAM_BY_ID(table, 0))
        {
            uint32 entry_hw_id;
            SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_get(unit, entry_handle_id, &entry_hw_id));
            PRT_CELL_SET_PL(curr_res_type_index, "%d", entry_hw_id);
        }

        /*
         * Add keys for the entry
         */
        for (iter = 0; iter < entry_handle->nof_key_fields; iter++)
        {
            char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
            uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
            uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

            if ((DBAL_TABLE_IS_TCAM(table)) || (table->table_type == DBAL_TABLE_TYPE_LPM))
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get(unit, entry_handle_id,
                                                                             table->keys_info[iter].field_id,
                                                                             field_val, field_mask));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id,
                                                                      table->keys_info[iter].field_id, field_val));
            }

            if (entry_handle->key_field_ids[iter] == DBAL_FIELD_CORE_ID)
            {
                    /** in this case the core ID will be used as a separate parameter and not part of the key */
                if ((entry_handle->core_id == DBAL_CORE_ALL) || (table->core_mode == DBAL_CORE_MODE_SBC))
                {
                    sal_sprintf(buffer, "ALL");
                }
                else
                {
                    sal_sprintf(buffer, " %d ", entry_handle->core_id);
                }
            }
            else
            {
                if ((DBAL_TABLE_IS_TCAM(table)) || (table->table_type == DBAL_TABLE_TYPE_LPM))
                {
                    SHR_IF_ERR_EXIT(dbal_field_in_table_printable_string_get(unit, table->keys_info[iter].field_id,
                                                                             table_id, field_val, field_mask, 0,
                                                                             TRUE, FALSE, buffer));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dbal_field_in_table_printable_string_get(unit, table->keys_info[iter].field_id,
                                                                             table_id, field_val, NULL, 0, TRUE,
                                                                             FALSE, buffer));
                }
            }
            PRT_CELL_SET_PL(curr_res_type_index, "%s", buffer);
        }

        if (!dump_only_key_fields)
        {
            PRT_CELL_SET_PL(curr_res_type_index, "");
            /*
             * Add results for the entry
             */
            for (iter = first_res_index; iter < table->multi_res_info[curr_res_type_index].nof_result_fields; iter++)
            {
                dbal_field_data_t field_data;
                char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
                int rv;

                rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                             table->
                                                             multi_res_info[curr_res_type_index].results_info[iter].
                                                             field_id, INST_SINGLE, field_data.field_val);

                /** when the field has valid indication it can return not_found, in this case no need to dump info*/
                if (rv == _SHR_E_NONE)
                {
                    SHR_IF_ERR_EXIT(dbal_field_in_table_printable_string_get
                                    (unit, table->multi_res_info[curr_res_type_index].results_info[iter].field_id,
                                     table_id, field_data.field_val, NULL, entry_handle->cur_res_type, FALSE, FALSE,
                                     buffer));
                    PRT_CELL_SET_PL(curr_res_type_index, "%s", buffer);
                }
                else
                {
                    PRT_CELL_SET_PL(curr_res_type_index, "----");
                }
            }
        }
        /*
         * For MDB tables - add the hitbit
         */
        if (hitbit_en)
        {
            if (!SAL_BOOT_PLISIM)
            {
                uint32 hitbit;
                SHR_IF_ERR_EXIT(dbal_entry_attribute_get(unit, entry_handle_id, DBAL_ENTRY_ATTR_HIT_GET, &hitbit));
                PRT_CELL_SET_PL(curr_res_type_index, "%s",
                                dbal_mdb_hitbit_to_string(unit, table->physical_db_id[0], hitbit));
            }
            else
            {
                PRT_CELL_SET_PL(curr_res_type_index, "N/A");
            }
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        entry_counter[curr_res_type_index]++;
    }

    /*
     * Add Total entries per table
     */
    for (res_type_index = 0; res_type_index < table->nof_result_types; res_type_index++)
    {
        PRT_INFO_ADD_PL(res_type_index, "Total Entries: %d", entry_counter[res_type_index]);
        PRT_COMMITX_PL(res_type_index);

        if ((table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC) && (entry_counter[res_type_index] == 0))
        {
            if (table->hl_mapping_multi_res[res_type_index].is_default_non_standard)
            {
                LOG_CLI((BSL_META("\n NOTE, 0 entries found for this result type. "
                                  "but the default entry is non-standard (means that there are entries in HW different than 0\n"
                                  "to see default entries use: dbal table dump table=xxx default\n")));
            }
        }
    }

exit:
    for (res_type_index = 0; res_type_index < table->nof_result_types; res_type_index++)
    {
        PRT_FREE_PL(res_type_index);
    }

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dbal_entries_print_by_attribute(
    int unit,
    dbal_labels_e label,
    uint8 export_to_file,
    sh_sand_control_t * sand_control)
{
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    int is_image_specific = 0, is_dirty = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("image_specific", is_image_specific);
    SH_SAND_GET_INT32("DIRTY", is_dirty);

    SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                    (unit, DBAL_TABLE_EMPTY, label, DBAL_NOF_ACCESS_METHODS, DBAL_PHYSICAL_TABLE_NONE,
                     DBAL_TABLE_TYPE_NONE, &table_id));

    while (table_id != DBAL_TABLE_EMPTY)
    {
        int to_print = TRUE;
        CONST dbal_logical_table_t *table;

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
        if (is_image_specific)
        {
            if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_IMAGE_SPECIFIC_TABLE))
            {
                to_print = FALSE;
            }
        }

        if (is_dirty)
        {
            if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_TABLE_DIRTY))
            {
                to_print = FALSE;
            }
        }

        if (to_print)
        {
            if (!export_to_file)
            {
                SHR_IF_ERR_EXIT(diag_dbal_table_entries_print(unit, table_id, 0, 0, NULL, NULL));
            }
            else
            {
                SHR_IF_ERR_EXIT(diag_dbal_table_print_table_in_xml_format
                                (unit, table_id, export_to_file, sand_control));
            }
        }
        SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                        (unit, table_id, label, DBAL_NOF_ACCESS_METHODS, DBAL_PHYSICAL_TABLE_NONE, DBAL_TABLE_TYPE_NONE,
                         &table_id));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
* dump all entries that related to a specific table.
* this function is using iterator to dump all the entries.
* input parameter "table name"
 */
static shr_error_e
diag_dbal_table_entries_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_tables_e table_id = 0;
    char *table_name;
    char *filename;
    char *label_str;
    int dump_key_only;
    int dump_default_entries;
    dbal_labels_e label = DBAL_LABEL_NONE;
    int is_image_specific = 0, is_dirty = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("table", table_name);
    SH_SAND_GET_BOOL("key_only", dump_key_only);
    SH_SAND_GET_STR("label", label_str);
    SH_SAND_GET_BOOL("default", dump_default_entries);
    SH_SAND_GET_STR("file", filename);
    SH_SAND_GET_INT32("image_specific", is_image_specific);
    SH_SAND_GET_INT32("DIRTY", is_dirty);

    /** if no general attribute requested table name nust exists */
    if (ISEMPTY(label_str) && (is_image_specific == 0) && (is_dirty == 0))
    {
        if (ISEMPTY(table_name))
        {
            DIAG_DBAL_HEADER_DUMP("table name must exists: dbal table dump Table=<table_name>", "\0");
            SHR_EXIT();
        }
    }
    else
    {
        if (!ISEMPTY(label_str))
        {
            SHR_IF_ERR_EXIT(dbal_label_string_to_id(unit, label_str, &label));
        }
    }

    /** if we have one of the attributes, print multiple tables.. */
    if (label != DBAL_LABEL_NONE || is_image_specific || is_dirty)
    {
        SHR_IF_ERR_EXIT(diag_dbal_entries_print_by_attribute(unit, label, (!ISEMPTY(filename)), sand_control));
        SHR_EXIT();
    }

    if ((sal_strncmp(table_name, "ALL", sal_strlen(table_name)) == 0) || (!ISEMPTY(filename)))
    {
        /** All tables dump requested */
        SHR_IF_ERR_EXIT(diag_dbal_table_xml_format_dump(unit, sand_control));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(diag_dbal_table_from_string(unit, table_name, &table_id, sand_control));
    if (table_id != dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        SHR_IF_ERR_EXIT(diag_dbal_table_entries_print
                        (unit, table_id, dump_key_only, dump_default_entries, args, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 * Generate dynamically "dbal table populate" example .
 */
static shr_error_e
diag_dbal_table_example_generate(
    int unit,
    int flags,
    struct sh_sand_cmd_s *sh_sand_cmd,
    rhlist_t * examples_list)
{
    char cmd[2][DBAL_MAX_EXTRA_LONG_STRING_LENGTH];
    char *sdk_dir;

    SHR_FUNC_INIT_VARS(unit);

    sdk_dir = getenv("SDK");
    if (sdk_dir == NULL)
    {
        sdk_dir = ".";
    }

    sal_sprintf(cmd[0], "FILENaMe=%s/tools/sand/db/jericho2_a0/dbalPopulateExample.xml", sdk_dir);
    sal_sprintf(cmd[1], "FILENaMe=%s/tools/sand/db/jericho2_a0/dbalPopulateExample.dcsv", sdk_dir);
    SHR_CLI_EXIT_IF_ERR(sh_sand_example_add(unit, flags, sh_sand_cmd, examples_list, cmd[0]), "");
    SHR_CLI_EXIT_IF_ERR(sh_sand_example_add(unit, flags, sh_sand_cmd, examples_list, cmd[1]), "");

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_option_t diag_dbal_table_populate_options[] = {
    {"FILENaMe", SAL_FIELD_TYPE_STR, "dbal populate file", ""},
    {NULL}
};

static sh_sand_man_t diag_dbal_table_populate_man = {
    .brief = "Populate dbal tables from file",
    .full = NULL,
    .synopsis = "FILENaMe=<file path>",
    .example_generate_cb = diag_dbal_table_example_generate
};

static sh_sand_option_t diag_dbal_table_list_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "dbal table name", "", NULL, NULL, SH_SAND_ARG_FREE},
    {"LaBeL", SAL_FIELD_TYPE_STR, "dbal label name", ""},
    {"MaTuRity", SAL_FIELD_TYPE_STR, "dbal maturity level", ""},
    {"StaTuS", SAL_FIELD_TYPE_STR, "dbal table status", ""},
    {"access", SAL_FIELD_TYPE_STR, "dbal table access", ""},
    {"operations", SAL_FIELD_TYPE_INT32, "min operations", "0"},
    {"size", SAL_FIELD_TYPE_INT32, "min payload size", "0"},
    {"image_specific", SAL_FIELD_TYPE_INT32, "is table is valid only for this image", "0"},
    {"dirty", SAL_FIELD_TYPE_INT32, "is table was touched (commit/clear)", "0"},
    {NULL}
};

static sh_sand_man_t diag_dbal_table_list_man = {
    .brief = "dump all list of tables info related to specific table name/label/maturity/status/access/image_specific",
    .full = "dump all list of tables info related to specific table name/label/maturity/status,"
        "label can be from the list of labels, maturity level can be Medium/High status can be NotInit/Init/hwerr/incompatibleImage.",
    .synopsis = "[MaTuRity=<maturity level>] [LaBeL=<label name>] [status=<status>] [access=<access_method>]"
        "[TaBLe=<tabel name>][dirty=<0/1>][Size=<1000>]",
    .examples =
        "LaBeL=L3 table=IPv4_mul\n" "TaBLe=INGRESS_PP_PORT\n" "access=HARD_LOGIC table=KLEAP_FWD12\n" "size=1000\n"
        "MaTuRity=HIGH table=TRAP_INGRESS\n" "image_specific=1 dirty=1\n" "operations=1000 table=FIELD_PMF\n"
        "status=hwerr access=PEMLA"
};

static sh_sand_option_t diag_dbal_table_clear_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "dbal table name", ""},
    {NULL}
};

static sh_sand_man_t diag_dbal_table_clear_man = {
    .brief = "delete all entries from the table",
    .full = NULL,
    .synopsis = "TaBLe=<tabel name>",
    .examples = "table=IPV4_UNICAST_PRIVATE_LPM_FORWARD\n" "table=PRT_VIRTUAL_PORT_TCAM"
};

static sh_sand_option_t diag_dbal_table_dump_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "dbal table name", ""},
    {"Key_Only", SAL_FIELD_TYPE_BOOL, "Dump only key fields", "false"},
    {"Label", SAL_FIELD_TYPE_STR, "Dump tables entries that related to label", ""},
    {"default", SAL_FIELD_TYPE_BOOL, "Dump default entries", "false"},
    {"image_specific", SAL_FIELD_TYPE_INT32, "is table is valid only for this image", "0"},
    {"dirty", SAL_FIELD_TYPE_INT32, "is table was touched (commit/clear)", "0"},
    {NULL}
};

static sh_sand_enum_t sand_dbal_table_indications_enum_table[] = {
    {"hibit", DBAL_TABLE_IND_IS_HITBIT_EN, "hitbit enabled"},
    {"hook_active", DBAL_TABLE_IND_IS_HOOK_ACTIVE, "table has hook (not supported yet)"},
    {"range_set", DBAL_TABLE_IND_RANGE_SET_SUPPORTED, "range set supported (not supported yet)"},
    {"learn_enabled", DBAL_TABLE_IND_IS_LEARNING_EN, "learn enabled"},
    {"commit_mode_opt", DBAL_TABLE_IND_IS_NONE_DIRECT_OPTIMIZED, "none direct commit mode"},
    {"Dirty", DBAL_TABLE_IND_IS_TABLE_DIRTY, "is table touched"},
    {"Protection", DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED, "thread safe table"},
    {"Validation", DBAL_TABLE_IND_NO_VALIDATIONS, "No table validations"},
    {"Collect_mode", DBAL_TABLE_IND_COLLECT_MODE, "asynchronous commit mode"},
    {"EMPTY", DBAL_NOF_TABLE_IND, "default"},
    {NULL}
};

static sh_sand_option_t diag_dbal_table_indication_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "dbal table name", ""},
    {"INDiCation", SAL_FIELD_TYPE_ENUM, "the table indication to update", "EMPTY",
     (void *) sand_dbal_table_indications_enum_table},
    {"Value", SAL_FIELD_TYPE_BOOL, "the value related to the indication to update", "false"},
    {NULL}
};

static sh_sand_man_t diag_dbal_table_dump_man = {
    .brief = "Dump all table entries or dump partial entries by using rules",
    .full =
        "Dump all table entries or dump partial entries by using rules (=,>,<). User can add multiple rules, each rules related to a field in the table. DBAL will perform AND between the rules",
    .synopsis = "[TaBLe=<tabel name>] [LaBeL=<label name>] [image_specific=<0/1>] [dirty=<0/1>]",
    .examples = "table=INGRESS_PP_PORT\n" "TaBLe=INGRESS_PP_PORT Key_Only\n"
        "label=QOS_PHB\n"
        "TaBLe=QOS_INGRESS_PHB_INTERNAL_MAP_ENTRIES QOS_INT_MAP_ID=\">4\" QOS_INT_MAP_ID=\"<6\"\n"
        "TaBLe=IN_AC_INFO_DB HITBIT=\"=0\"\n" "TaBLe=IN_AC_INFO_DB HITBIT=\"=1\"\n"
        "image_specific=1 dirty=1\n" "TaBLe=KLEAP_FWD12_ALIGNER_MAPPING default"
};

static sh_sand_man_t diag_dbal_table_indication_man = {
    .brief = "table indications manue. allow to dump or update table indications",
    .full = "table indication are properties of the table. not all indications can be set.",
    .synopsis = "[Indication=<table indication>] [Value=<true/false>]",
    .examples =
        "table=BIER_BFER_TERMINATION\n"
        "table=BIER_BFER_TERMINATION indication=commit_mode_opt value=true\n"
        "table=BIER_BFER_TERMINATION indication=commit_mode_opt value=false"
};

sh_sand_cmd_t diag_dbal_table_cmds[] = {
  /**********************************************************************************************************
   * CMD_NAME *     CMD_ACTION              * Next *        Options                   *       MAN           *
   *          *                             * Level*                                  *                     *
   *          *                             * CMD  *                                  *                     *
 */
    {"Info", diag_dbal_table_info, NULL, diag_dbal_table_list_options, &diag_dbal_table_list_man,
     NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"CLear", diag_dbal_table_clear, NULL, diag_dbal_table_clear_options, &diag_dbal_table_clear_man,
     diag_dbal_table_cmd_iterator_rule_get},
    {"DuMP", diag_dbal_table_entries_dump, NULL, diag_dbal_table_dump_options, &diag_dbal_table_dump_man,
     diag_dbal_table_cmd_iterator_rule_get, NULL, SH_CMD_NO_XML_VERIFY},
    {"INDication", diag_dbal_table_indication, NULL, diag_dbal_table_indication_options,
     &diag_dbal_table_indication_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"PoPuLate", diag_dbal_table_populate, NULL, diag_dbal_table_populate_options, &diag_dbal_table_populate_man,
     NULL, NULL, SH_CMD_SKIP_EXEC},
    {NULL}
};
