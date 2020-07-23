/*! \file bcma_clicmd_cd.h
 *
 * CLI 'cd' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_CD_H
#define BCMA_CLICMD_CD_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_CD_DESC \
    "Change current working directory."

/*! Syntax for CLI command. */
#define BCMA_CLICMD_CD_SYNOP \
    "[<directory>]"

/*! Help for CLI command. */
#define BCMA_CLICMD_CD_HELP \
    "Change current working directory. <directory> must be of the\n" \
    "form \"usr%password@host:<path>\" or \"/flash/<directory>\" for\n" \
    "FTP to remote system or local flash disk respectively.\n" \
    "If the target directory does not exist, this command may still\n" \
    "succeed, but attempts to access files may fail.\n"

/*!
 * \brief CLI 'cd' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_cd(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_CLICMD_CD_H */
