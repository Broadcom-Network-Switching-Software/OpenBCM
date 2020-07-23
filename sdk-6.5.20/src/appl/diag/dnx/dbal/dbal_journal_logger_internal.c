/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * dbal_journal_logger_internal.c
 */

#include <soc/dnx/dbal/dbal.h>
#include <sal/appl/sal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <sys/stat.h>
#include <dirent.h>
#include <dbal_journal_logger_internal.h>

#define BSL_LOG_MODULE BSL_LS_SOCDNX_ERRORRECOVERY

dbal_journal_logger_info_t logger_info[BCM_MAX_NUM_UNITS] = { {0}
};

shr_error_e
dbal_journal_logger_table_lock(
    int unit,
    dbal_tables_e table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    logger_info[unit].user_log_locked_table = table_id;

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_journal_logger_table_mode_set(
    int unit,
    dbal_logger_mode_e mode)
{
    SHR_FUNC_INIT_VARS(unit);

    if (mode >= DBAL_NOF_LOGGER_MODES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mode %d is out of range\n", mode);
    }

    logger_info[unit].logger_mode = mode;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_journal_logger_status_print(
    int unit,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Logger status");

    PRT_COLUMN_ADD("File");
    PRT_COLUMN_ADD("Mode");
    PRT_COLUMN_ADD("Table locked");
    PRT_COLUMN_ADD("Internal locked");
    PRT_COLUMN_ADD("Flags");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("%s", (logger_info[unit].dbal_file != NULL) ? "ON" : "OFF");
    if (logger_info[unit].logger_mode == DBAL_LOGGER_MODE_REGULAR)
    {
        PRT_CELL_SET("Regular");
    }
    else
    {
        PRT_CELL_SET("Write only");
    }
    if (logger_info[unit].user_log_locked_table != DBAL_TABLE_EMPTY)
    {
        PRT_CELL_SET("%s", dbal_logical_table_to_string(unit, logger_info[unit].user_log_locked_table));
    }
    else
    {
        PRT_CELL_SET("%s", "NONE");
    }

    if (logger_info[unit].internal_log_locked_table != DBAL_TABLE_EMPTY)
    {
        PRT_CELL_SET("%s", dbal_logical_table_to_string(unit, logger_info[unit].internal_log_locked_table));
    }
    else
    {
        PRT_CELL_SET("%s", "NONE");
    }

    PRT_CELL_SET("%s", logger_info[unit].disable_logger ? "Disabled" : "None");

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_journal_logger_init(
    int unit)
{
    CONST dnx_data_dbal_diag_loggerInfo_t *journal_logger_info;

    SHR_FUNC_INIT_VARS(unit);

    /** logger is not locked on any table */
    logger_info[unit].disable_logger = 1;
    logger_info[unit].user_log_locked_table = DBAL_TABLE_EMPTY;
    logger_info[unit].internal_log_locked_table = DBAL_TABLE_EMPTY;
    logger_info[unit].logger_mode = DBAL_LOGGER_MODE_REGULAR;

    journal_logger_info = dnx_data_dbal.diag.loggerInfo_get(unit);

    if (journal_logger_info->file_name)
    {
        logger_info[unit].dbal_file = sal_fopen(journal_logger_info->file_name, "a");
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_journal_logger_file_open(
    int unit,
    char *file_name)
{
    SHR_FUNC_INIT_VARS(unit);

    if (logger_info[unit].dbal_file == NULL)
    {
        logger_info[unit].dbal_file = sal_fopen(file_name, "a");
        DBAL_DUMP("FILE OPENED\n\n", 0, logger_info[unit].dbal_file);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "File already open!\n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_journal_logger_file_close(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (logger_info[unit].dbal_file != NULL)
    {
        DBAL_DUMP("FILE CLOSED\n\n", 0, logger_info[unit].dbal_file);
        sal_fclose(logger_info[unit].dbal_file);
        logger_info[unit].dbal_file = NULL;
    }
    SHR_FUNC_EXIT;
}

int
dbal_journal_logger_is_enable(
    int unit,
    dbal_tables_e table_id)
{
    /**
     *  Logger is enabled only if all four conditions are set:
     * 1. called from main thread
     * 2. user logger lock is disabled or set to current table
     * 3. internal logger lock is disabled or set to current table
     * 4. global logger disable flag is off
     */
    if (!logger_info[unit].disable_logger)
    {
        if ((logger_info[unit].user_log_locked_table == DBAL_TABLE_EMPTY) ||
            (table_id == logger_info[unit].user_log_locked_table))
        {
            if ((logger_info[unit].internal_log_locked_table == DBAL_TABLE_EMPTY) ||
                (table_id == logger_info[unit].internal_log_locked_table))
            {
                if (sal_thread_self() == sal_thread_main_get())
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}
