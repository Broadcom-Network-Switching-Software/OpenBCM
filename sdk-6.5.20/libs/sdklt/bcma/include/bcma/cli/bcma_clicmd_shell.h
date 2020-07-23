/*! \file bcma_clicmd_shell.h
 *
 * CLI 'shell' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_SHELL_H
#define BCMA_CLICMD_SHELL_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_SHELL_DESC \
    "Launch system shell."

/*!
 * \brief CLI 'shell' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_shell(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_CLICMD_SHELL_H */
