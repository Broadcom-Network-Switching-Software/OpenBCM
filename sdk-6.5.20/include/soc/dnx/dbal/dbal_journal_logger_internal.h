/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef INCLUDE_SOC_DNX_DBAL_DBAL_JOURNAL_LOGGER_INTERNAL_H_
#define INCLUDE_SOC_DNX_DBAL_DBAL_JOURNAL_LOGGER_INTERNAL_H_

#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>

typedef struct
{

    uint8 disable_logger;

    dbal_tables_e user_log_locked_table;

    dbal_tables_e internal_log_locked_table;

    FILE *dbal_file;

    dbal_logger_mode_e logger_mode;

} dbal_journal_logger_info_t;

shr_error_e dbal_journal_logger_table_lock(
    int unit,
    dbal_tables_e table_id);

shr_error_e dbal_journal_logger_table_mode_set(
    int unit,
    dbal_logger_mode_e mode);

shr_error_e dnx_dbal_journal_logger_status_print(
    int unit,
    sh_sand_control_t * sand_control);

int dbal_journal_logger_is_enable(
    int unit,
    dbal_tables_e table_id);

shr_error_e dbal_journal_logger_init(
    int unit);

shr_error_e dbal_journal_logger_file_open(
    int unit,
    char *file_name);

shr_error_e dbal_journal_logger_file_close(
    int unit);

#endif
