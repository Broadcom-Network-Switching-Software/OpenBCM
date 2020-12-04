/** \file diag_dnx_error_recovery.c
 *
 * Main diagnostics for Error Recovery applications All CLI commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_ERRORRECOVERY

/*************
 * INCLUDES  *
 */
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_err_recovery_manager_common.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <src/appl/diag/dnx/dbal/diag_dnx_dbal.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include <soc/dnx/dbal/dbal_journal_logger_internal.h>
#include "diag_dnx_error_recovery.h"

/*************
 * TYPEDEFS  *
 */

/**
 * \brief - enable the error recovery feature
 */
static shr_error_e
sh_dnx_err_rec_enable(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_err_recovery_common_enable_disable(unit, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - disable the error recovery feature
 */
static shr_error_e
sh_dnx_err_rec_disable(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_err_recovery_common_enable_disable(unit, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - deinit error recovery diagnistics, reset it to default value
 */
shr_error_e
sh_dnx_deinit_err_rec(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_err_recovery_common_init(unit));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - starts the rollback journal
 */
static shr_error_e
sh_dnx_err_rec_rollback_jrnl_start(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - commit the rollback journal
 */
static shr_error_e
sh_dnx_err_rec_rollback_jrnl_commit(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - rollback the rollback journal
 */
static shr_error_e
sh_dnx_err_rec_rollback_jrnl_rollback(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - suppress the rollback journal
 */
static shr_error_e
sh_dnx_err_rec_rollback_jrnl_suppress(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DNX_ERR_RECOVERY_SUPPRESS(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - unsuppress the rollback journal
 */
static shr_error_e
sh_dnx_err_rec_rollback_jrnl_unsuppress(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DNX_ERR_RECOVERY_UNSUPPRESS(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print the journal
 */
static shr_error_e
sh_dnx_err_rec_transaction_swstate_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_print_journal(unit, DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print the journal
 */
static shr_error_e
sh_dnx_err_rec_transaction_dbal_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_print_journal(unit, DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - journal turn logger on
 */
static shr_error_e
sh_dnx_err_rec_transaction_swstate_logger(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_logger_state_change(unit, DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - journal turn logger on
 */
static shr_error_e
sh_dnx_err_rec_transaction_dbal_logger(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 print_status = FALSE;
    char *table_name = NULL, *file_name = NULL, *mode = NULL;
    dbal_tables_e table_id = DBAL_NOF_TABLES;
    uint8 is_logging;

    SHR_FUNC_INIT_VARS(unit);

    dnx_dbal_journal_logger_state_get(unit, &is_logging);

    if (!is_logging)
    {
        SH_SAND_GET_STR("Table", table_name);
        SH_SAND_GET_STR("FILENaMe", file_name);
        SH_SAND_GET_BOOL("Status", print_status);
        SH_SAND_GET_STR("Mode", mode);

        if (!ISEMPTY(file_name))
        {
            if (sal_strcasecmp("close", file_name) == 0)
            {
                SHR_IF_ERR_EXIT(dbal_journal_logger_file_close(unit));
                LOG_CLI((BSL_META(" logger filed closed.\n")));
                SHR_EXIT();
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_journal_logger_file_open(unit, file_name));
                SHR_EXIT();
            }
        }

        if (!ISEMPTY(table_name))
        {
            if (sal_strcasecmp("off", table_name) == 0)
            {
                SHR_IF_ERR_EXIT(dbal_journal_logger_table_lock(unit, DBAL_TABLE_EMPTY));
                LOG_CLI((BSL_META(" logger released.\n")));
                SHR_EXIT();
            }

            if (dbal_logical_table_string_to_id(unit, table_name, &table_id) == _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "No table found matching to string %s \n", table_name);
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_journal_logger_table_lock(unit, table_id));
                LOG_CLI((BSL_META("Logger is Locked on table %s with severity info\n"),
                         dbal_logical_table_to_string(unit, table_id)));
                SHR_EXIT();
            }
        }

        if (!ISEMPTY(mode))
        {
            if (sal_strcasecmp("write", mode) == 0)
            {
                SHR_IF_ERR_EXIT(dbal_journal_logger_table_mode_set(unit, DBAL_LOGGER_MODE_WRITE_ONLY));
                LOG_CLI((BSL_META(" logger mode set to write only.\n")));
            }

            if (sal_strcasecmp("regular", mode) == 0)
            {
                SHR_IF_ERR_EXIT(dbal_journal_logger_table_mode_set(unit, DBAL_LOGGER_MODE_REGULAR));
                LOG_CLI((BSL_META(" logger mode set to regular.\n")));
            }
        }

        if (print_status == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_dbal_journal_logger_status_print(unit, sand_control));
            SHR_EXIT();
        }
    }
/*enables dbal journal logging */
    SHR_IF_ERR_EXIT(dnx_rollback_journal_logger_state_change(unit, DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print the journal
 */
static shr_error_e
sh_dnx_err_rec_compare_swstate_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_state_comparison_print_journal(unit, DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print the journal
 */
static shr_error_e
sh_dnx_err_rec_compare_dbal_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_state_comparison_print_journal(unit, DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - journal turn logger on
 */
static shr_error_e
sh_dnx_err_rec_compare_swstate_logger(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_state_comparison_journal_logger_state_change(unit, DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - journal turn logger on
 */
static shr_error_e
sh_dnx_err_rec_compare_dbal_logger(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_state_comparison_journal_logger_state_change(unit, DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - starts the comparison journal
 */
static shr_error_e
sh_dnx_err_rec_comparison_jrnl_start(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_state_comparison_start(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - end the comparison journal
 */
static shr_error_e
sh_dnx_err_rec_comparison_jrnl_end(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_state_comparison_end_and_compare(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - suppress the rollback journal
 */
static shr_error_e
sh_dnx_err_rec_comparison_jrnl_suppress(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_comparison_journal_tmp_suppress(unit, TRUE));
    SHR_IF_ERR_EXIT(dnx_dbal_journal_comparison_tmp_suppress_unsafe(unit, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - unsuppress the rollback journal
 */
static shr_error_e
sh_dnx_err_rec_comparison_jrnl_unsuppress(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_comparison_journal_tmp_suppress(unit, FALSE));
    SHR_IF_ERR_EXIT(dnx_dbal_journal_comparison_tmp_suppress_unsafe(unit, FALSE));

exit:
    SHR_FUNC_EXIT;
}

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
/**
 * \brief - turn on rollback api testing
 */
static shr_error_e
sh_dnx_err_rec_rollback_jrnl_test_mode_on(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_er_regress_testing_api_test_mode_change(unit, TRUE);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - turn off rollback api testing
 */
static shr_error_e
sh_dnx_err_rec_rollback_jrnl_test_mode_off(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_er_regress_testing_api_test_mode_change(unit, FALSE);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

sh_sand_man_t sh_dnx_err_rec_transaction_jrnl_man = {
    .brief = "Rollback journal menu",
    .full = NULL
};

sh_sand_man_t sh_dnx_err_rec_comparison_jrnl_man = {
    .brief = "Comparison journal menu",
    .full = NULL
};

sh_sand_man_t sh_dnx_err_rec_man = {
    .brief = "Error recovery menu",
    .full = "Error recovery menu"
};

static sh_sand_man_t sh_dnx_err_rec_enable_man = {
    .brief = "Error recovery enable",
    .full = "Error recovery enable",
    .deinit_cb = sh_dnx_deinit_err_rec
};

static sh_sand_man_t sh_dnx_err_rec_disble_man = {
    .brief = "Error recovery disable",
    .full = "Error recovery disable",
    .deinit_cb = sh_dnx_deinit_err_rec
};

static sh_sand_man_t sh_dnx_err_rec_rollback_jrnl_start_man = {
    .brief = "Start a new rollback journal transaction",
    .full = "Start a new rollback journal transaction. Nesting transactions is permitted."
};

static sh_sand_man_t sh_dnx_err_rec_rollback_jrnl_commit_man = {
    .brief = "Commit the current rollback journal transaction",
    .full = "Commit the current rollback journal transaction. No rollback is being done."
};

static sh_sand_man_t sh_dnx_err_rec_rollback_jrnl_rollback_man = {
    .brief = "Rollback the current rollback journal transaction",
    .full = "Rollback the current rollback journal transaction. All changes during the transaction will be rolled back."
};

static sh_sand_man_t sh_dnx_err_rec_rollback_jrnl_swstate_man = {
    .brief = "SW STATE rollback journal commands",
    .full = "SW STATE rollback journal commands"
};

static sh_sand_man_t sh_dnx_err_rec_rollback_jrnl_dbal_man = {
    .brief = "DBAL rollback journal commands",
    .full = "DBAL rollback journal commands"
};

static sh_sand_man_t sh_dnx_err_rec_rollback_jrnl_suppress_man = {
    .brief = "Suppresses the rollback journal",
    .full =
        "Temporary suppresses the current rollback journal transaction. No operations are journaled during this period."
};

static sh_sand_man_t sh_dnx_err_rec_rollback_jrnl_unsuppress_man = {
    .brief = "Unsuppresses the rollback journal",
    .full =
        "Unsuppresses the temporary current rollback journal transaction. Journaling is resumed for the current transaction."
};

static sh_sand_man_t sh_dnx_err_rec_comparison_jrnl_start_man = {
    .brief = "Start a new comparison journal transaction",
    .full = "Start a new comparison journal transaction. Nesting transactions is permitted."
};

static sh_sand_man_t sh_dnx_err_rec_comparison_jrnl_end_man = {
    .brief = "End and compare the current comparison journal transaction",
    .full = "End and compare the current comparison journal transaction. Differences are logged."
};

static sh_sand_man_t sh_dnx_err_rec_comparison_jrnl_swstate_man = {
    .brief = "SW STATE comparison journal commands",
    .full = "SW STATE comparison journal commands"
};

static sh_sand_man_t sh_dnx_err_rec_comparison_jrnl_dbal_man = {
    .brief = "DBAL comparison journal commands",
    .full = "DBAL comparison journal commands"
};

static sh_sand_man_t sh_dnx_err_rec_comparison_jrnl_suppress_man = {
    .brief = "Suppresses the comparison journal",
    .full =
        "Temporary suppresses the current comparison journal transaction. No operations are journaled during this period."
};

static sh_sand_man_t sh_dnx_err_rec_comparison_jrnl_unsuppress_man = {
    .brief = "Unsuppresses the comparison journal",
    .full =
        "Unsuppresses the temporary current comparison journal transaction. Journaling is resumed for the current transaction."
};

static sh_sand_man_t sh_dnx_err_rec_jrnl_print_man = {
    .brief = "Print the contents of the current journal transaction",
    .full = "Print the contents of the current journal transaction."
};

static sh_sand_man_t sh_dnx_err_rec_jrnl_logger_man = {
    .brief = "Log contents of the current journal transaction as they are being inserted",
    .full = "Log contents of the current journal transaction as they are being inserted",
    .examples = "table=EGRESS_PP_PORT\n" "mode=Write\n" "status"
};

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING

static sh_sand_man_t sh_dnx_err_rec_rollback_jrnl_rollback_test_man = {
    .brief = "Rollback toggle current test mode",
    .full = "Allow or disallow rollback journal regression testing for each API opting-in for error recovery"
};

/**
 * \brief DNX Rollback journal test command pack
 */
sh_sand_cmd_t sh_dnx_err_rec_transaction_test_cmds[] = {
    {"ON", sh_dnx_err_rec_rollback_jrnl_test_mode_on, NULL, NULL, &sh_dnx_err_rec_rollback_jrnl_rollback_test_man},
    {"OFF", sh_dnx_err_rec_rollback_jrnl_test_mode_off, NULL, NULL, &sh_dnx_err_rec_rollback_jrnl_rollback_test_man},
    {NULL}
};

#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */
/**
 * \brief DNX Rollback journal command pack
 */

sh_sand_cmd_t sh_dnx_err_rec_transaction_swstate[] = {
    {"PRinT", sh_dnx_err_rec_transaction_swstate_print, NULL, NULL, &sh_dnx_err_rec_jrnl_print_man},
    {"LoGger", sh_dnx_err_rec_transaction_swstate_logger, NULL, NULL, &sh_dnx_err_rec_jrnl_logger_man, NULL, NULL,
     SH_CMD_SKIP_EXEC},
    {NULL}
};

static sh_sand_option_t dnx_dbal_journal_logger_options[] = {
    {"Table", SAL_FIELD_TYPE_STR, "assign logger level to specific table", ""},
    {"Status", SAL_FIELD_TYPE_BOOL, "show status", "No"},
    {"Mode", SAL_FIELD_TYPE_STR, "write / regular", ""},
    {"FILENaMe", SAL_FIELD_TYPE_STR, "Filename", "", NULL, NULL, SH_SAND_ARG_QUIET},
    {NULL}
};

sh_sand_cmd_t sh_dnx_err_rec_transaction_dbal[] = {
    {"PRinT", sh_dnx_err_rec_transaction_dbal_print, NULL, NULL, &sh_dnx_err_rec_jrnl_print_man},
    {"LoGger", sh_dnx_err_rec_transaction_dbal_logger, NULL, dnx_dbal_journal_logger_options,
     &sh_dnx_err_rec_jrnl_logger_man, NULL, NULL},
    {NULL}
};

sh_sand_cmd_t sh_dnx_err_rec_compare_swstate[] = {
    {"PRinT", sh_dnx_err_rec_compare_swstate_print, NULL, NULL, &sh_dnx_err_rec_jrnl_print_man},
    {"LoGger", sh_dnx_err_rec_compare_swstate_logger, NULL, NULL, &sh_dnx_err_rec_jrnl_logger_man, NULL, NULL,
     SH_CMD_SKIP_EXEC},
    {NULL}
};

sh_sand_cmd_t sh_dnx_err_rec_compare_dbal[] = {
    {"PRinT", sh_dnx_err_rec_compare_dbal_print, NULL, NULL, &sh_dnx_err_rec_jrnl_print_man},
    {"LoGger", sh_dnx_err_rec_compare_dbal_logger, NULL, NULL, &sh_dnx_err_rec_jrnl_logger_man, NULL, NULL,
     SH_CMD_SKIP_EXEC},
    {NULL}
};

sh_sand_cmd_t sh_dnx_err_rec_transaction_cmds[] = {
  /**********************************************************************************************************
   * CMD_NAME *     CMD_ACTION              * Next *        Options                   *       MAN           *
   *          *                             * Level*                                  *                     *
   *          *                             * CMD  *                                  *                     *
 */
    {"START", sh_dnx_err_rec_rollback_jrnl_start, NULL, NULL, &sh_dnx_err_rec_rollback_jrnl_start_man},
    {"SuSPenD", sh_dnx_err_rec_rollback_jrnl_suppress, NULL, NULL, &sh_dnx_err_rec_rollback_jrnl_suppress_man},
    {"ReSuMe", sh_dnx_err_rec_rollback_jrnl_unsuppress, NULL, NULL, &sh_dnx_err_rec_rollback_jrnl_unsuppress_man},
    {"CoMmit", sh_dnx_err_rec_rollback_jrnl_commit, NULL, NULL, &sh_dnx_err_rec_rollback_jrnl_commit_man, NULL, NULL,
     SH_CMD_SKIP_EXEC},
    {"RoLLBaCK", sh_dnx_err_rec_rollback_jrnl_rollback, NULL, NULL, &sh_dnx_err_rec_rollback_jrnl_rollback_man},
    {"SWSTate", NULL, sh_dnx_err_rec_transaction_swstate, NULL, &sh_dnx_err_rec_rollback_jrnl_swstate_man},
    {"DBaL", NULL, sh_dnx_err_rec_transaction_dbal, NULL, &sh_dnx_err_rec_rollback_jrnl_dbal_man},
#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    {"Test_MoDe", NULL, sh_dnx_err_rec_transaction_test_cmds, NULL, &sh_dnx_err_rec_rollback_jrnl_rollback_test_man},
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */
    {NULL}
};

/**
 * \brief DNX Comparison journal command pack
 */
sh_sand_cmd_t sh_dnx_err_rec_comparison_cmds[] = {
  /**********************************************************************************************************
   * CMD_NAME *     CMD_ACTION              * Next *        Options                   *       MAN           *
   *          *                             * Level*                                  *                     *
   *          *                             * CMD  *                                  *                     *
 */
    {"START", sh_dnx_err_rec_comparison_jrnl_start, NULL, NULL, &sh_dnx_err_rec_comparison_jrnl_start_man},
    {"SuSPenD", sh_dnx_err_rec_comparison_jrnl_suppress, NULL, NULL, &sh_dnx_err_rec_comparison_jrnl_suppress_man},
    {"ReSuMe", sh_dnx_err_rec_comparison_jrnl_unsuppress, NULL, NULL, &sh_dnx_err_rec_comparison_jrnl_unsuppress_man},
    {"END", sh_dnx_err_rec_comparison_jrnl_end, NULL, NULL, &sh_dnx_err_rec_comparison_jrnl_end_man},
    {"SWSTate", NULL, sh_dnx_err_rec_compare_swstate, NULL, &sh_dnx_err_rec_comparison_jrnl_swstate_man},
    {"DBaL", NULL, sh_dnx_err_rec_compare_dbal, NULL, &sh_dnx_err_rec_comparison_jrnl_dbal_man},
    {NULL}
};

sh_sand_cmd_t sh_dnx_err_rec_cmds[] = {
  /**********************************************************************************************************
   * CMD_NAME *     CMD_ACTION              * Next *        Options                   *       MAN           *
   *          *                             * Level*                                  *                     *
   *          *                             * CMD  *                                  *                     *
 */
    {"ENABLE", sh_dnx_err_rec_enable, NULL, NULL, &sh_dnx_err_rec_enable_man},
    {"DISABLE", sh_dnx_err_rec_disable, NULL, NULL, &sh_dnx_err_rec_disble_man},
    {NULL}
};
