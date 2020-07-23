/*! \file bcma_cli_bshell.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLI_BSHELL_H
#define BCMA_CLI_BSHELL_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Execute a CLI command.
 *
 * This function allows a CLI command to be executed from the context
 * of a CINT script or a debugger such as gdb.
 *
 * \param [in] unit Unit number on which to execute the command.
 * \param [in] cmd Command as it would have been entered in the CLI.
 *
 * \retval BCMA_CLI_CMD_OK if no errors.
 */
extern int
bcma_cli_bshell(int unit, char *cmd);

/*!
 * \brief Initialize bshell context.
 *
 * The bshell function must be associated with a CLI object before it
 * can be used.
 *
 * \param [in] cli CLI to process bshell commands.
 */
extern void
bcma_cli_bshell_init(bcma_cli_t *cli);

#endif /* BCMA_CLI_BSHELL_H */
