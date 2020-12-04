/**
 * \file diag_dnx_dbal_signals_db_init.c
 * 
 *
 * Main functions for init the dbal signals tables DB
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

/*************
 * INCLUDES  *
 */

#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/types.h>
#include <shared/bsl.h>
#include <shared/util.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_rhlist.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <soc/sand/sand_aux_access.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/dnx/swstate/auto_generated/access/dbal_access.h>
#include <include/shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/kbp/kbp_common.h>
#include "diag_dnx_dbal_signals_db_init.h"

/*************
 *  DEFINES  *
 */
#define DBAL_SIGNALS_DB_INVALID        (-1)
#define DBAL_SIGNALS_NOF_XML_FILES     (10)
#define DBAL_SIGNALS_DB_INIT_DEFAULT_VAL_TEMP   (0x12345678)

/** DBAL define path can SOC_CHIP_STRING(unit) since J2C also read from the same path it is used as following */
#define DBAL_SIGNALS_FILE_PATH                  "jericho_2"

#define DBAL_SIGNALS_INIT_COPY_N_STR(dest, src, length) \
do                                                      \
{                                                       \
    sal_strncpy_s(dest, src, length);                   \
}while (0);

/*************
 *  GLOBALS  *
 */

dbal_signals_table_t signals_table_info[DBAL_NOF_TABLES];

/**
 * \brief
 * DBAL Last Hit XMLs files list
 */
char *signals_xml_files_list[DBAL_SIGNALS_NOF_XML_FILES];

/*************
 * FUNCTIONS *
 */
/**
 * \brief
 * The function init the xml files list and init it to EMPTY string
 */
static shr_error_e
diag_dnx_dbal_signals_db_file_list_init(
    int unit)
{
    int file_index;

    SHR_FUNC_INIT_VARS(unit);

    for (file_index = 0; file_index < DBAL_SIGNALS_NOF_XML_FILES; file_index++)
    {
        signals_xml_files_list[file_index] = NULL;
        SHR_ALLOC(signals_xml_files_list[file_index], RHFILE_MAX_SIZE, "signals_xml_files_list", "%s%s%s\r\n", EMPTY,
                  EMPTY, EMPTY);
        sal_strncpy_s(signals_xml_files_list[file_index], EMPTY, RHFILE_MAX_SIZE);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function free the allocated memory for xml files list
 */
static void
diag_dnx_dbal_signals_db_file_list_deinit(
    int unit)
{
    int file_index;
    for (file_index = 0; file_index < DBAL_SIGNALS_NOF_XML_FILES; file_index++)
    {
        SHR_FREE(signals_xml_files_list[file_index]);
    }
}

/**
 * \brief
 * The function fills the field from table interface DB
 * called by: dbal_db_init_table_read_interface
 */
static shr_error_e
diag_dnx_dbal_signals_db_init_table_read_key_field(
    int unit,
    void *cur_field_node,
    dbal_signals_table_key_field_info_t * field_db)
{
    SHR_FUNC_INIT_VARS(unit);

    RHDATA_GET_XSTR_STOP(cur_field_node, "Name", field_db->signal_name, DBAL_MAX_STRING_LENGTH);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function fills the field from table interface DB
 * called by: dbal_db_init_table_read_interface
 */
static shr_error_e
diag_dnx_dbal_signals_db_init_table_read_result_field(
    int unit,
    void *cur_field_node,
    table_signals_db_result_field_info_t * field_db)
{
    SHR_FUNC_INIT_VARS(unit);

    RHDATA_GET_XSTR_STOP(cur_field_node, "Name", field_db->result_field_name, DBAL_MAX_STRING_LENGTH);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function adds the table interface part read from XML to
 * the dbal table DB.
 * The interface is common to all logical tables type
 * Read by : diag_dnx_dbal_signals_db_init_table_add
 */
static shr_error_e
diag_dnx_dbal_signals_db_init_table_add_interface_to_table(
    int unit,
    dbal_signals_table_t * table_entry,
    table_signals_db_struct_t * table_params)
{
    int stage_index, key_index, field_index;

    SHR_FUNC_INIT_VARS(unit);

    /** Name - Printing Name */
    sal_strncpy_s(table_entry->table_name, table_params->table_name, sizeof(table_params->table_name));

    /** Stages  */
    if (table_params->nof_stages > MAX_NUM_SIGNALS_DIAG_STAGES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s has more stages than max allowed.\n", table_params->table_name);
    }

    if (table_params->nof_stages == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s has 0 stages min 1 stage needed.\n", table_params->table_name);
    }

    SHR_ALLOC_SET_ZERO(table_entry->signals_stage_info, table_params->nof_stages * sizeof(dbal_signals_stage_info_t),
                       "stage info allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);

    for (stage_index = 0; stage_index < table_params->nof_stages; stage_index++)
    {
        dbal_signals_stage_info_t *signals_stage_info = &table_entry->signals_stage_info[stage_index];
        table_signals_db_stage_info_struct_t *table_signals_stage_info = &table_params->signals_stage_info[stage_index];

        sal_strncpy_s(signals_stage_info->stage_name, table_signals_stage_info->stage_name,
                      sizeof(table_signals_stage_info->stage_name));
        SHR_IF_ERR_EXIT(dnx_pp_stage_string_to_id(unit, signals_stage_info->stage_name, &signals_stage_info->stage_id));

        /** Key Fields */
        if (table_signals_stage_info->nof_key_fields > DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s has more key fields than max allowed.\n", table_params->table_name);
        }

        SHR_ALLOC_SET_ZERO(signals_stage_info->signals_key_info,
                           table_signals_stage_info->nof_key_fields * sizeof(dbal_signals_table_key_field_info_t),
                           "key fields info allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);

        for (key_index = 0; key_index < table_signals_stage_info->nof_key_fields; key_index++)
        {
            dbal_signals_table_key_field_info_t *signals_key_info =
                &table_entry->signals_stage_info[stage_index].signals_key_info[key_index];
            dbal_signals_table_key_field_info_t *table_params_key_info =
                &table_params->signals_stage_info[stage_index].signals_key_info[key_index];

            sal_strncpy_s(signals_key_info->signal_name, table_params_key_info->signal_name,
                          sizeof(table_params_key_info->signal_name));
        }
        table_entry->signals_stage_info[stage_index].nof_key_fields =
            table_params->signals_stage_info[stage_index].nof_key_fields;

        SHR_ALLOC_SET_ZERO(signals_stage_info->result_field_info,
                           table_signals_stage_info->nof_result_fields * sizeof(dbal_signals_table_result_field_info_t),
                           "result fields info allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);

        for (field_index = 0; field_index < table_signals_stage_info->nof_result_fields; field_index++)
        {
            dbal_fields_e field_id;
            dbal_signals_table_result_field_info_t *signals_field_info =
                &table_entry->signals_stage_info[stage_index].result_field_info[field_index];
            table_signals_db_result_field_info_t *table_params_field_info =
                &table_params->signals_stage_info[stage_index].result_field_info[field_index];
            SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, table_params_field_info->result_field_name, &field_id));
            signals_field_info->field_id = field_id;

            
        }
        table_entry->signals_stage_info[stage_index].nof_result_fields = table_signals_stage_info->nof_result_fields;

    }
    table_entry->nof_stages = table_params->nof_stages;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function adds a logical table read from XML to the dbal
 * tables DB.
 * Main functionality is translating the string from XML to
 * corresponding values
 */
shr_error_e
diag_dnx_dbal_signals_db_init_table_add(
    int unit,
    table_signals_db_struct_t * table_params,
    dbal_signals_table_t * table_info)
{
    dbal_signals_table_t *table_entry;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    /** find the field id according to its name */
    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_params->table_name, &table_id));
    table_entry = table_info + table_id;

    /** set interface data */
    SHR_IF_ERR_EXIT(diag_dnx_dbal_signals_db_init_table_add_interface_to_table(unit, table_entry, table_params));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dnx_dbal_signals_db_init_table_read_interface(
    int unit,
    void *interface_node,
    table_signals_db_struct_t * table_param)
{
    int field_counter, stage_counter;
    void *curKey, *curResult, *curStage, *MultipleStages, *curSub;
    table_signals_db_stage_info_struct_t *stage_param;

    SHR_FUNC_INIT_VARS(unit);

    curKey = dbx_xml_child_get_first(interface_node, "Key");
    if (curKey == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cant find Key section in table:%s\n", table_param->table_name);
    }

    stage_counter = 0;
    curStage = dbx_xml_child_get_first(curKey, "Stage");
    if (curStage != NULL)
    {
        stage_param = &table_param->signals_stage_info[stage_counter];
        RHDATA_GET_XSTR_STOP(curStage, "Name", stage_param->stage_name, DBAL_MAX_STRING_LENGTH);
        field_counter = 0;
        RHDATA_ITERATOR(curSub, curStage, "Signal")
        {
            SHR_IF_ERR_EXIT(diag_dnx_dbal_signals_db_init_table_read_key_field
                            (unit, curSub, &stage_param->signals_key_info[field_counter]));
            field_counter++;
        }
        stage_param->nof_key_fields = field_counter;
        table_param->nof_stages = 1;
    }
    else
    {
        MultipleStages = dbx_xml_child_get_first(curKey, "MultipleStages");
        if (MultipleStages == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Can't find Stage section (Stage / MultipleStages) in table:%s\n",
                         table_param->table_name);
        }

        RHDATA_ITERATOR(curStage, MultipleStages, "Stage")
        {
            stage_param = &table_param->signals_stage_info[stage_counter];
            field_counter = 0;
            RHDATA_GET_XSTR_STOP(curStage, "Name", stage_param->stage_name, DBAL_MAX_STRING_LENGTH);

            RHDATA_ITERATOR(curSub, curStage, "Signal")
            {
                SHR_IF_ERR_EXIT(diag_dnx_dbal_signals_db_init_table_read_key_field
                                (unit, curSub, &stage_param->signals_key_info[field_counter]));
                field_counter++;
            }
            stage_param->nof_key_fields = field_counter;
            stage_counter++;
        }
        table_param->nof_stages = stage_counter;
    }

    field_counter = 0;
    curResult = dbx_xml_child_get_first(interface_node, "Result");
    if (curResult == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cant find Result section in table:%s\n", table_param->table_name);
    }

    stage_counter = 0;
    curStage = dbx_xml_child_get_first(curResult, "Stage");
    if (curStage != NULL)
    {
        stage_param = &table_param->signals_stage_info[stage_counter];
        RHDATA_GET_XSTR_STOP(curStage, "Name", stage_param->stage_name, DBAL_MAX_STRING_LENGTH);

        field_counter = 0;
        RHDATA_ITERATOR(curSub, curStage, "Field")
        {
            SHR_IF_ERR_EXIT(diag_dnx_dbal_signals_db_init_table_read_result_field
                            (unit, curSub, &stage_param->result_field_info[field_counter]));
            field_counter++;
        }
        stage_param->nof_result_fields = field_counter;
    }
    else
    {
        MultipleStages = dbx_xml_child_get_first(curResult, "MultipleStages");
        if (MultipleStages != NULL)
        {
            RHDATA_ITERATOR(curStage, MultipleStages, "Stage")
            {
                stage_param = &table_param->signals_stage_info[stage_counter];
                field_counter = 0;
                RHDATA_GET_XSTR_STOP(curStage, "Name", stage_param->stage_name, DBAL_MAX_STRING_LENGTH);

                RHDATA_ITERATOR(curSub, curStage, "Field")
                {
                    SHR_IF_ERR_EXIT(diag_dnx_dbal_signals_db_init_table_read_result_field
                                    (unit, curSub, &stage_param->result_field_info[field_counter]));
                    field_counter++;
                }
                stage_param->nof_result_fields = field_counter;
                stage_counter++;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dnx_dbal_signals_db_init_tables(
    int unit)
{
    int file_idx, nof_files;
    void *curTop, *cur, *curInterface;
    char **file_list_ptr;
    char db_path[RHFILE_MAX_SIZE];
    table_signals_db_struct_t *cur_table_param = NULL;

    SHR_FUNC_INIT_VARS(unit);
    if (diag_dnx_dbal_signals_db_file_list_init(unit) != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Cant allocate file list array\n");
    }
    file_list_ptr = &signals_xml_files_list[0];

    sal_memset(&signals_table_info, 0x0, (sizeof(dbal_signals_table_t) * DBAL_NOF_TABLES));

    SHR_ALLOC(cur_table_param, sizeof(table_signals_db_struct_t),
              "logical_tables,cur_table_param", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Read signals xml DB */
    sal_strncpy_s(db_path, DB_INIT_SIGNALS_PATH, sizeof(db_path));
    nof_files = dbx_file_get_xml_files_from_dir(DBAL_SIGNALS_FILE_PATH, db_path, file_list_ptr);

    for (file_idx = 0; file_idx < nof_files; file_idx++)
    {
        char file_path[RHFILE_MAX_SIZE];

        char file_name[RHFILE_MAX_SIZE];
        int file_name_len;

        sal_strncpy_s(file_path, db_path, sizeof(file_path));
        sal_strncat_s(file_path, "/", RHFILE_MAX_SIZE);
        DBAL_SIGNALS_INIT_COPY_N_STR(file_name, signals_xml_files_list[file_idx], RHFILE_MAX_SIZE);
        file_name_len = sal_strnlen(file_name, sizeof(file_name));
        if ((sal_strnlen(file_path, sizeof(file_path)) + file_name_len) > (sizeof(file_path) - 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Size of file_path (container) is %d. This is too small to contain both file_path (%d) AND file_name (%d). Quit\n",
                         (int) sizeof(file_path), (int) sal_strnlen(file_path, sizeof(file_path)), file_name_len);
        }
        sal_strncat(file_path, file_name, file_name_len + 1);
        curTop = dbx_file_get_xml_top(unit, file_path, "TblDbCatalog", CONF_OPEN_PER_DEVICE);
        if (curTop == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find dbal signals tables xml file: %s\n", file_path);
        }

        /** Run all over XML fields and add it to the fields table */
        RHDATA_ITERATOR(cur, curTop, "TblDB")
        {
            /** clear the table_signals_db_struct_t which is common */
            sal_memset(cur_table_param, 0x0, sizeof(table_signals_db_struct_t));
            RHDATA_GET_XSTR_STOP(cur, "Name", cur_table_param->table_name, DBAL_MAX_STRING_LENGTH);
            curInterface = dbx_xml_child_get_first(cur, "TblDbInterface");
            if (curInterface == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Can't find TblDbInterface section in table:%s\n",
                             cur_table_param->table_name);
            }
            SHR_IF_ERR_EXIT(diag_dnx_dbal_signals_db_init_table_read_interface(unit, curInterface, cur_table_param));

            SHR_IF_ERR_EXIT(diag_dnx_dbal_signals_db_init_table_add(unit, cur_table_param, signals_table_info));
        }
        dbx_xml_top_close(curTop);
    }

exit:
    SHR_FREE(cur_table_param);
    diag_dnx_dbal_signals_db_file_list_deinit(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dnx_dbal_signals_db_deinit_tables(
    int unit)
{
    dbal_signals_stage_info_t *signals_stage_info;
    int table_index, stage_index;

    SHR_FUNC_INIT_VARS(unit);

    for (table_index = 0; table_index < DBAL_NOF_TABLES; table_index++)
    {
        if (signals_table_info[table_index].nof_stages > 0)
        {
            for (stage_index = 0; stage_index < signals_table_info[table_index].nof_stages; stage_index++)
            {
                signals_stage_info = &signals_table_info[table_index].signals_stage_info[stage_index];
                SHR_FREE(signals_stage_info->signals_key_info);
                SHR_FREE(signals_stage_info->result_field_info);
            }
            SHR_FREE(signals_table_info[table_index].signals_stage_info);
        }
        signals_table_info[table_index].nof_stages = 0;
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
