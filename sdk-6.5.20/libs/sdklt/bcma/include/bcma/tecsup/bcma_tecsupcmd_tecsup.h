/*! \file bcma_tecsupcmd_tecsup.h
 *
 * CLI command 'techsupport' description.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TECSUPCMD_TECSUP_H
#define BCMA_TECSUPCMD_TECSUP_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_TECSUPCMD_TECSUP_DESC \
    "Collect diagnostics and debug information."

/*! Syntax for CLI command. */
#define BCMA_TECSUPCMD_TECSUP_SYNOP \
    "config [mode=builtin|script] [path=<string>]\n" \
    "dump <feature> [<category> ...]\n" \
    "list <feature> [<category> ...]"

/*! Help for CLI command. */
#define BCMA_TECSUPCMD_TECSUP_HELP \
    "The techsupport command allows an end user to collect diagnostics\n" \
    "and debug information per switch feature.\n" \
    "\n" \
    "The 'config' subcommand without any parameters will show the\n" \
    "current collection mode along with the supported categories and\n" \
    "features.\n" \
    "The techsupport information is collected based on a set of CLI\n" \
    "commands, and by default a built-in set of commands will be used.\n" \
    "Use the 'mode' and 'path' parameters to collect information based\n" \
    "on a set of external script files instead.\n" \
    "\n" \
    "The 'dump' subcommand collects the diagnostics and debug\n" \
    "information based on the specified feature and category.  Note\n" \
    "that the features and categories are device and variant specific.\n" \
    "If a specified feature/category is not supported on the device\n" \
    "variant, an error will be returned.\n" \
    "If <feature> is '*', all features will be dumped.\n" \
    "If no <category> is given, all categories will be dumped.\n" \
    "\n" \
    "The 'list' subcommand shows the list of commands used to collect\n" \
    "the diagnostics and debug information."

/*! Examples for CLI command. */
#define BCMA_TECSUPCMD_TECSUP_EXAMPLES \
    "config\n" \
    "config mode=script path=/tmp/script/\n" \
    "dump *\n" \
    "list *"

/*!
 * \brief CLI 'techsupport' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_tecsupcmd_tecsup(bcma_cli_t *cli, bcma_cli_args_t *args);

/*!
 * \brief Clean up the resource used in the 'techsupport' command.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_tecsupcmd_tecsup_cleanup(bcma_cli_t *cli);

#endif /* BCMA_TECSUPCMD_TECSUP_H */
