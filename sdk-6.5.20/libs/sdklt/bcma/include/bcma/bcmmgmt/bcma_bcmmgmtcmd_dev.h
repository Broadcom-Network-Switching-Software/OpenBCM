/*! \file bcma_bcmmgmtcmd_dev.h
 *
 * CLI 'dev' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMMGMTCMD_DEV_H
#define BCMA_BCMMGMTCMD_DEV_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMMGMTCMD_DEV_DESC  "Perform a full system stop/start."

/*! Syntax for CLI command. */
#define BCMA_BCMMGMTCMD_DEV_SYNOP "stop|start|restart|status [all]"

/*! Help for CLI command. */
#define BCMA_BCMMGMTCMD_DEV_HELP \
    "Add \'all\' for the whole system operation.\n" \
    "For example, 'restart all' includes reloading the configuration file,\n" \
    "restarting the core components and all devices.\n" \
    "Otherwise only the current device is operated on."

/*!
 * \brief CLI 'dev' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmmgmtcmd_dev(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMMGMTCMD_DEV_H */
