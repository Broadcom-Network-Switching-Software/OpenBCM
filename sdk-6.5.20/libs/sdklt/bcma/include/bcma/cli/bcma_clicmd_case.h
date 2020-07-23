/*! \file bcma_clicmd_case.h
 *
 * CLI 'case' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_CASE_H
#define BCMA_CLICMD_CASE_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_CASE_DESC \
    "Execute command based on string match."

/*! Syntax for CLI command. */
#define BCMA_CLICMD_CASE_SYNOP \
    "<key> <value> <command> [<value> <command> ... ] [* <command>]"

/*! Help for CLI command. */
#define BCMA_CLICMD_CASE_HELP \
    "Execute a command based on matching a key value. Command lines of\n" \
    "more than one word must be quoted. Different commands must be\n" \
    "separated by semicolon."

/*! Examples for CLI command */
#define BCMA_CLICMD_CASE_EXAMPLES \
    "$key a \"echo 1; echo 2\" b \"echo 3\""

/*!
 * \brief CLI 'case' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_case(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_CLICMD_CASE_H */
