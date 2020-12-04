/*! \file bcma_salcmd_backtrace.c
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_sysrq.h>

#include <shr/shr_exception.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/sal/bcma_salcmd_backtrace.h>

/* Backtrace control parameters. */
static struct backtrace_params_s {

    /*
     * Only show backtrace if this string matches the function name
     * from the caller. Matches any function name if NULL.
     */
    char *func;

    /*
     * Only show backtrace if this string matches the file name from
     * the caller. Matches any file name if NULL.
     */
    char *file;

    /*
     * Only show backtrace if this value matches the line number from
     * the caller. Matches any line number if zero.
     */
    int line;

    /*
     * Only show backtrace if this value matches the error code from
     * the caller. Matches any error code if zero.
     */
    int err_code;

    /*
     * Number of backtraces to show. This number is decremented every
     * time a backtrace is shown. Use -1 to show unlimited backtraces.
     */
    int trig;

    /*
     * Number of backtraces to skip. This number is decremented every
     * time a backtrace is requested. Once this value reaches zero,
     * subsequent backtrace requests will be honored according to the
     * value of 'trig' above.
     */
    int skip;

    /*
     * Request external debugger to provide backtrace. An external
     * debugger can normally provide a more accurate and detailed
     * backtrace, but it is more time consuming and may not even be
     * available.
     */
    int dbg;

} backtrace_params;

static void
show_backtrace(int unit, int err_code,
               const char *file, int line, const char *func)
{
    struct backtrace_params_s *btp = &backtrace_params;

    if (btp->func && func && sal_strstr(func, btp->func) == NULL) {
        return;
    }
    if (btp->file && file && sal_strstr(file, btp->file) == NULL) {
        return;
    }
    if (btp->line && line != btp->line) {
        return;
    }
    if (btp->err_code && err_code != btp->err_code) {
        return;
    }
    if (btp->skip > 0) {
        btp->skip--;
    } else if (btp->trig != 0) {
        if (btp->trig > 0) {
            btp->trig--;
        }
        sal_sysrq(SAL_SYSRQ_BACKTRACE, btp->dbg);
    }
}

static void
show_params(struct backtrace_params_s *btp)
{
    char *func = btp->func;
    char *file = btp->file;

    if (func == NULL || *func == '\0') {
        func = "<none>";
    }
    if (file == NULL || *file == '\0') {
        file = "<none>";
    }
    cli_out("%-16s %s\n", "Function name:", func);
    cli_out("%-16s %s\n", "File name:", file);
    cli_out("%-16s %d\n", "Line number:", btp->line);
    cli_out("%-16s %d\n", "Error code:", btp->err_code);
    cli_out("%-16s %d\n", "Trigger count:", btp->trig);
    cli_out("%-16s %d\n", "Skip count:", btp->skip);
    cli_out("%-16s %d\n", "Use debugger:", btp->dbg);
}

static void
func_reset(struct backtrace_params_s *btp)
{
    if (btp->func) {
        sal_free(btp->func);
        btp->func = NULL;
    }
}

static void
file_reset(struct backtrace_params_s *btp)
{
    if (btp->file) {
        sal_free(btp->file);
        btp->file = NULL;
    }
}

int
bcma_salcmd_backtrace(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    struct backtrace_params_s *btp = &backtrace_params;
    bcma_cli_parse_table_t pt;
    char *func;
    char *file;

    if (BCMA_CLI_ARG_CNT(args) == 0) {
        show_params(btp);
        return BCMA_CLI_CMD_OK;
    }

    func = btp->func;
    file = btp->file;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "FUNC", "str", &func, NULL);
    bcma_cli_parse_table_add(&pt, "File", "str", &file, NULL);
    bcma_cli_parse_table_add(&pt, "Line", "int", &btp->line, NULL);
    bcma_cli_parse_table_add(&pt, "ErrorCode", "int", &btp->err_code, NULL);
    bcma_cli_parse_table_add(&pt, "Skip", "int", &btp->skip, NULL);
    bcma_cli_parse_table_add(&pt, "Trig", "int", &btp->trig, NULL);
    bcma_cli_parse_table_add(&pt, "UseDebugger", "bool", &btp->dbg, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    if (func && func != btp->func) {
        func_reset(btp);
        btp->func = sal_strdup(func);
    }

    if (file && file != btp->file) {
        file_reset(btp);
        btp->file = sal_strdup(file);
    }

    bcma_cli_parse_table_done(&pt);

    shr_exception_handler_set(show_backtrace);

    return BCMA_CLI_CMD_OK;
}

int
bcma_salcmd_backtrace_cleanup(bcma_cli_t *cli)
{
    struct backtrace_params_s *btp = &backtrace_params;

    func_reset(btp);
    file_reset(btp);

    return 0;
}
