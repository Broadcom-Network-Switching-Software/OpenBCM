/*! \file bcma_clicmd_rcload.h
 *
 * CLI 'rcload' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_RCLOAD_H
#define BCMA_CLICMD_RCLOAD_H

#include <bcma/cli/bcma_cli.h>

/*! Default file extension of rc script file. */
#ifndef BCMA_CLICMD_RCLOAD_FILE_EXT
#define BCMA_CLICMD_RCLOAD_FILE_EXT ".soc"
#endif

/*! Brief description for CLI command. */
#define BCMA_CLICMD_RCLOAD_DESC \
    "Load commands from a file."

/*! Syntax for CLI command. */
#define BCMA_CLICMD_RCLOAD_SYNOP \
    "<File> [<Parameter> ...]"

/*! Help for CLI command. */
#define BCMA_CLICMD_RCLOAD_HELP \
    "Load commands from a file until the file is complete or an error\n" \
    "occurs. If optional parameters are listed after <File>, they will be\n" \
    "pushed as local variables ($1, $2, ...) for the file processing.\n" \
    "If one of the script commands fails, the execution will stop,\n" \
    "unless the environment variable '_rcerror' is set to 0."

/*! Help for CLI command. */
#define BCMA_CLICMD_RCLOAD_HELP_2 \
    "For platforms where FTP is used, the user name, password and host\n" \
    "may be specified as: \"user%password@host:\" as a prefix to the\n" \
    "file name."

/*! Examples for CLI command. */
#define BCMA_CLICMD_RCLOAD_EXAMPLES \
    "file param1 20"

/*!
 * \brief CLI 'rcload' command implementation.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_rcload(bcma_cli_t *cli, bcma_cli_args_t *args);

/*!
 * \brief Execute CLI commands in a file.
 *
 * This function will create (push) a new environment variable scope and
 * execute CLI commands in the file \c filename. After command execution is
 * complete, the new environment will be destroyed (popped).
 * Arguments in \c args will be added as local environment variables
 * '$1', '$2', etc. in the new scope.
 * If \c auto_ext is TRUE, the default file extension
 * (\ref BCMA_CLICMD_RCLOAD_FILE_EXT) will be appended to \c filename if
 * \c filename does not have the default file extension.
 * eg. \c filename 'rc' will become 'rc.soc' if \c auto_ext is TRUE.
 * The script file will not be executed if environment variable \ref
 * BCMA_CLICMD_RCLOAD is set to 0.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 * \param [in] filename Script file name.
 * \param [in] auto_ext Append default file extension to \c filename if TRUE.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 * \return BCMA_CLI_CMD_NOT_FOUND if file not found or $CLI_RCLOAD is set to 0.
 */
extern int
bcma_clicmd_rcload_file(bcma_cli_t *cli, bcma_cli_args_t *args,
                        const char *filename, bool auto_ext);

#endif /* BCMA_CLICMD_RCLOAD_H */
