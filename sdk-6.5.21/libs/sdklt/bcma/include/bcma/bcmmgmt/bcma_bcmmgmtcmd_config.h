/*! \file bcma_bcmmgmtcmd_config.h
 *
 * CLI 'dev' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMMGMTCMD_CONFIG_H
#define BCMA_BCMMGMTCMD_CONFIG_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMMGMTCMD_CONFIG_DESC \
    "Load and inspect configuration file."

/*! Syntax for CLI command. */
#define BCMA_BCMMGMTCMD_CONFIG_SYNOP \
    "load|status\n" \
    "get <table>.<field>"

/*! Help for CLI command. */
#define BCMA_BCMMGMTCMD_CONFIG_HELP \
    "The main purpose of this command is to load the SDK configuration\n" \
    "file before the system manager is started. This is often useful\n" \
    "for board bring-up.\n" \
    "The 'status' command will check whether a configuration file has\n" \
    "been loaded or not.\n" \
    "The 'get'command can be used to read entries from the loaded\n" \
    "configuration. Note that table and field names are case-sensitive."

/*! Examples for CLI command. */
#define BCMA_BCMMGMTCMD_CONFIG_EXAMPLES \
    "load\n" \
    "get DEVICE_CONFIG.CHIP_DEBUG"

/*!
 * \brief CLI 'config' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmmgmtcmd_config(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMMGMTCMD_CONFIG_H */
