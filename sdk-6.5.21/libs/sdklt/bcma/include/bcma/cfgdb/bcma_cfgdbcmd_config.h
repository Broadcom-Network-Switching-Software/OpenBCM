/*! \file bcma_cfgdbcmd_config.h
 *
 * CLI 'config' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CFGDBCMD_CONFIG_H
#define BCMA_CFGDBCMD_CONFIG_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CFGDBCMD_CONFIG_DESC \
    "Configure Management interface."

/*! Syntax for CLI command. */
#define BCMA_CFGDBCMD_CONFIG_SYNOP \
    "=\n" \
    "<var_name>=<value> | <var_name> <value>\n" \
    "add <var_name>=<value> | <var_name> <value>\n" \
    "clear\n" \
    "delete [pattern=<substring>] [<var_name>]\n" \
    "dump\n" \
    "refresh\n" \
    "save [filename=<filename>] [pattern=<substring>] [append=yes]\n" \
    "show [<substring>]"

/*! Help for CLI command. */
#define BCMA_CFGDBCMD_CONFIG_HELP \
    "If <substring> in format pattern=<substring> starts with '^',\n" \
    "the pattern matches only when string starts with <substring>.\n" \
    "For example:\n" \
    "\"mem\" will match \"memory\" and \"socmem\", but \"^mem\" will only\n" \
    "match \"memory\".\n\n" \
    "Wildcard character '*' is allowed in <var_name> for config delete.\n"


/*!
 * \brief CLI configuration management variables manipulation.
 *
 * This function can handle various operations of configuration management
 * variables. It is able to show/set/delete variables of configuration
 * management interface, and save/load configuration variables to/from file.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \retval BCMA_CLI_CMD_OK Command completed successfully
 * \retval BCMA_CLI_CMD_USAGE Invalid command syntax
 * \retval BCMA_CLI_CMD_FAIL Command failed
 */
extern int
bcma_cfgdbcmd_config(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_CFGDBCMD_CONFIG_H */
