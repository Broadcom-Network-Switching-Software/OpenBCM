/*! \file bcma_bcmltcmd_config.h
 *
 * CLI 'config' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMLTCMD_CONFIG_H
#define BCMA_BCMLTCMD_CONFIG_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMLTCMD_CONFIG_DESC \
    "List device configuration."

/*! Syntax for CLI command. */
#define BCMA_BCMLTCMD_CONFIG_SYNOP \
    "[*]"

/*! Help for CLI command. */
#define BCMA_BCMLTCMD_CONFIG_HELP \
    "List the Software component configuration and the configuration\n" \
    "of the current device by default. If \'*\' is specified, the\n" \
    "configuration for all attached device units will be listed."

/*!
 * \brief CLI 'config' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmltcmd_config(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMLTCMD_CONFIG_H */
