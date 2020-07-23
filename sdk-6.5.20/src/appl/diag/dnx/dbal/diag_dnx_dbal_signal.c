/** \file diag_dnx_dbal_signal.c
 *
 * Main diagnostics for dbal applications All CLI commands, that
 * are related to DBAL signals, are gathered in this file.
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

#include "diag_dnx_dbal.h"
#include "diag_dnx_dbal_signals_db_init.h"

/*************
 * TYPEDEFS  *
 */

/*************
 * FUNCTIONS *
 */

/**
 * fields values for random values test
 */
uint32 key_field_val[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
uint32 key_signal_val[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
uint32 key_field_mask[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
uint32 res_field_val[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
uint32 res_field_mask[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

extern dbal_signals_table_t signals_table_info[DBAL_NOF_TABLES];
/*************
* FUNCTIONS *
 */

 /*
  * LOCAL DIAG PACK:
  * {
  */
/**********************************************************************************************************************
 *  DBAL DIAGNOSTIC PACK:
 *  SIGNAL SUB MENU function & infrastructure Definitions - START
 *  STURCTURE:
 *  1. cmd function definition
 *  2. cmd option and man table
 *  3. TABLE SUB MENU cmd table
 */

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
diag_dbal_signal_table_info(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    CONST dbal_logical_table_t *table;
    dbal_signals_stage_info_t *signals_stage_info;
    dbal_tables_e table_id;
    char *table_name = NULL;
    int key_index, stage_index;
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
    char *str;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("table", table_name);

    if (ISEMPTY(table_name))
    {
        DIAG_DBAL_HEADER_DUMP("Example: dbal SIGnal Info TaBLe=<table_name>", "\0");
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(diag_dbal_table_from_string(unit, table_name, &table_id, sand_control));
    if (table_id == dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    str = buffer_to_print;
    sal_sprintf(str, "Signal info for table: %s", table->table_name);
    DIAG_DBAL_HEADER_DUMP(str, "\0");

    for (stage_index = 0; stage_index < signals_table_info[table_id].nof_stages; stage_index++)
    {
        signals_stage_info = &signals_table_info[table_id].signals_stage_info[stage_index];
        PRT_TITLE_SET("Signal info for Stage: %s", signals_stage_info->stage_name);
        /*
         * Key column
         */
        PRT_COLUMN_ADD("Key Fields");
        /*
         * Key columns
         */
        for (key_index = 0; key_index < table->nof_key_fields; key_index++)
        {
            PRT_COLUMN_ADD("%s", dbal_field_to_string(unit, table->keys_info[key_index].field_id));
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("Key Signals");
        for (key_index = 0; key_index < table->nof_key_fields; key_index++)
        {
            if (table->keys_info[key_index].field_id != DBAL_FIELD_CORE_ID)
            {
                PRT_CELL_SET("%s", signals_stage_info->signals_key_info[key_index].signal_name);
            }
            else
            {
                PRT_CELL_SET("CORE_ID");
            }
        }
        PRT_COMMITX;
    }

    LOG_CLI((BSL_META("\n")));

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Prints all the entries of a table with ID table_id. for the
 *   rquested stages.
 *   Printing format is compact - each entry in a single line.
 */
shr_error_e
diag_dbal_signal_table_entries_print(
    int unit,
    dbal_tables_e table_id,
    char *stage_name,
    uint8 dump_all_stages,
    int core_id,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE] = { 0 };
    CONST dbal_logical_table_t *table;
    dbal_signals_stage_info_t *signals_stage_info;
    int key_index, stage_index;
    uint32 entry_handle_id;
    dbal_entry_handle_t *entry_handle;
    dnx_pp_stage_e stage_id;
    dbal_field_data_t *val_fields_array = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    val_fields_array =
        sal_alloc(sizeof(dbal_field_data_t) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE, "Result fields");
    if (NULL == val_fields_array)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "alloc failed Result fields");
    }

    if (!dump_all_stages)
    {
        SHR_IF_ERR_EXIT(dnx_pp_stage_string_to_id(unit, stage_name, &stage_id));
    }
    else
    {
        stage_id = DNX_PP_STAGE_NOF;
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    for (stage_index = 0; stage_index < signals_table_info[table_id].nof_stages; stage_index++)
    {
        signals_stage_info = &signals_table_info[table_id].signals_stage_info[stage_index];
        if ((signals_stage_info->stage_id == stage_id) || (stage_id == DNX_PP_STAGE_NOF))
        {
            LOG_CLI((BSL_META("\nSignals For Stage %s:\n"), signals_stage_info->stage_name));

            for (key_index = 0; key_index < table->nof_key_fields; key_index++)
            {
                if (table->keys_info[key_index].field_id != DBAL_FIELD_CORE_ID)
                {

                    /** Receive the value of the context signal. Skip the current stage if the context signal is not found */
                    SHR_IF_ERR_EXIT(dpp_dsig_read
                                    (unit, DBAL_CORE_ALL, dnx_pp_stage_block_name(unit, signals_stage_info->stage_id),
                                     signals_stage_info->stage_name, NULL,
                                     signals_stage_info->signals_key_info[key_index].signal_name,
                                     (uint32 *) &key_signal_val[key_index],
                                     BITS2BYTES(table->keys_info[key_index].field_nof_bits)));
                    LOG_CLI((BSL_META("Signal: %s Value: %x\n"),
                             signals_stage_info->signals_key_info[key_index].signal_name,
                             key_signal_val[key_index][0]));
                    dbal_entry_key_field_arr32_set(unit, entry_handle_id, table->keys_info[key_index].field_id,
                                                   key_signal_val[key_index]);
                    dbal_field_in_table_printable_string_get(unit, table->keys_info[key_index].field_id, table_id,
                                                             key_signal_val[key_index], NULL, 0, TRUE, FALSE, buffer);
                }
                else
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, table->keys_info[key_index].field_id, core_id);
                    if (core_id == DBAL_CORE_ALL)
                    {
                        LOG_CLI((BSL_META("Signal: CORE_ID Value: DBAL_CORE_ALL\n")));
                    }
                    else
                    {
                        LOG_CLI((BSL_META("Signal: CORE_ID Value: %x\n"), core_id));
                    }
                    dbal_field_in_table_printable_string_get(unit, table->keys_info[key_index].field_id, table_id,
                                                             (uint32 *) &core_id, NULL, 0, TRUE, FALSE, buffer);
                }

            }
            LOG_CLI((BSL_META("\n")));
            /*
             * read the full entry 
             */
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

            SHR_IF_ERR_EXIT(diag_dbal_entry_print_entry(unit, entry_handle_id));
        }
    }

exit:
    if (val_fields_array != NULL)
    {
        sal_free(val_fields_array);
    }

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* dump table entries that related to a specific table in 
* specific stages. input parameter "table name" "stage name"
 */
static shr_error_e
diag_dbal_signal_table_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_tables_e table_id = 0;
    char *table_name;
    char *stage_name;
    int is_dump_all_stages;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("table", table_name);
    SH_SAND_GET_STR("stage", stage_name);
    SH_SAND_GET_BOOL("all", is_dump_all_stages);
    SH_SAND_GET_INT32("Core", core_id);

    if (ISEMPTY(table_name))
    {
        DIAG_DBAL_HEADER_DUMP("Example: dbal SIGnal DuMP TaBLe=<table_name> Stage=<stage name>/ALL", "\0");
        SHR_EXIT();
    }

    if (ISEMPTY(stage_name))
    {
        is_dump_all_stages = TRUE;
    }

    SHR_IF_ERR_EXIT(diag_dbal_table_from_string(unit, table_name, &table_id, sand_control));
    if (table_id != dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        SHR_IF_ERR_EXIT(diag_dbal_signal_table_entries_print
                        (unit, table_id, stage_name, is_dump_all_stages, core_id, args, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_option_t diag_dbal_signal_table_list_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "dbal table name", ""},
    {"SIGnal", SAL_FIELD_TYPE_STR, "dbal table related signal", ""},
    {NULL}
};

static sh_sand_man_t diag_dbal_signal_table_list_man = {
    .brief = "dump all list of tables info related to specific table name/signal",
    .full = "dump all list of tables info related to specific table name/signal,",
    .synopsis = "[SIGnal=<signal name>] [TaBLe=<tabel name>]",
    .examples = "SIGnal=Out_PP_Port TaBLe=EGRESS_PP_PORT"
};

static sh_sand_option_t diag_dbal_signal_table_dump_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "dbal table name", ""},
    {"STaGe", SAL_FIELD_TYPE_STR, "Dump only specific stage fields", ""},
    {"ALL", SAL_FIELD_TYPE_BOOL, "Dump all stages table entries", "false"},
    {NULL}
};

static sh_sand_man_t diag_dbal_signal_table_dump_man = {
    .brief = "dump table entry match to requested stage signals value",
    .full = NULL,
    .synopsis = "TaBLe=<tabel name> STaGe=<stage_name>",
    .examples = "TaBLe=EGRESS_PP_PORT STaGe=IPMF1\n" "TaBLe=EGRESS_PP_PORT ALL"
};

sh_sand_cmd_t diag_dbal_signal_cmds[] = {
  /**********************************************************************************************************
   * CMD_NAME *     CMD_ACTION              * Next *        Options                   *       MAN           *
   *          *                             * Level*                                  *                     *
   *          *                             * CMD  *                                  *                     *
 */
    {"Info", diag_dbal_signal_table_info, NULL, diag_dbal_signal_table_list_options, &diag_dbal_signal_table_list_man,},
    {"DuMP", diag_dbal_signal_table_dump, NULL, diag_dbal_signal_table_dump_options, &diag_dbal_signal_table_dump_man,},
    {NULL}
};
