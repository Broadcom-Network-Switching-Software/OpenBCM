/*! \file bcma_salcmd_backtrace.h
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_SALCMD_BACKTRACE_H
#define BCMA_SALCMD_BACKTRACE_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_SALCMD_BACKTRACE_DESC  \
    "Enable backtrace on error."

/*! Syntax for CLI command. */
#define BCMA_SALCMD_BACKTRACE_SYNOP \
    "[Trig=<cnt>] [Skip=<cnt>] [File=<name>] [Line=<num>] \\\n" \
    "[FUNC=<name>] [UseDebugger=0|1]"

/*! Help for CLI command. */
#define BCMA_SALCMD_BACKTRACE_HELP \
    "Request backtrace when a driver error is encountered.\n" \
    "\n" \
    "Options:\n" \
    "Trig        - Number of backtraces to show. This number is decremented\n" \
    "              every time a backtrace is shown. Use -1 to show unlimited\n" \
    "              backtraces.\n" \
    "Skip        - Number of backtraces to skip. This number is decremented\n" \
    "              every time a backtrace is requested. Once this value\n" \
    "              reaches zero, subsequent backtrace requests will be\n" \
    "              honored according to the value of the Trig parameter.\n" \
    "File        - Only show backtrace if this string matches the file name\n" \
    "              from the caller. Matches any file name if empty string.\n" \
    "Line        - Only show backtrace if this value matches the line number\n" \
    "              from the caller. Matches any line number if zero.\n" \
    "ErrorCode   - Only show backtrace if this value matches the error code\n" \
    "              from the caller. Matches any error code if zero.\n" \
    "FUNC        - Only show backtrace if this string matches the function\n" \
    "              name from the caller. Matches any name if empty string.\n" \
    "UseDebugger - Request external debugger to provide backtrace.\n" \
    "              An external debugger can normally provide a more accurate\n" \
    "              and detailed backtrace, but it is more time consuming and\n" \
    "              may not even be available.\n" \
    "\n" \
    "If no options are given, the current parameters will be shown."

/*! Examples for CLI command. */
#define BCMA_SALCMD_BACKTRACE_EXAMPLES \
    "trig=1\n" \
    "trig=-1 func=l2_insert\n" \
    "trig=1 skip=6 file=_init.c"

/*!
 * \brief CLI 'backtrace' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_salcmd_backtrace(bcma_cli_t *cli, bcma_cli_args_t *args);

/*!
 * \brief CLI 'backtrace' command cleanup.
 *
 * \param [in] cli CLI object
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_salcmd_backtrace_cleanup(bcma_cli_t *cli);

#endif /* BCMA_SALCMD_BACKTRACE_H */
