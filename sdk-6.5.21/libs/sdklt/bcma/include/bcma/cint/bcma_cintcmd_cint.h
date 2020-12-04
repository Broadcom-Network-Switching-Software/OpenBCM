/*! \file bcma_cintcmd_cint.h
 *
 * CLI 'cint' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CINTCMD_CINT_H
#define BCMA_CINTCMD_CINT_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CINTCMD_CINT_DESC \
    "Starts CINT prompt, used to execute BCM LT API's."

/*!
 * \brief CLI 'cint' command implementation.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_cintcmd_cint(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_CINTCMD_CINT_H */
