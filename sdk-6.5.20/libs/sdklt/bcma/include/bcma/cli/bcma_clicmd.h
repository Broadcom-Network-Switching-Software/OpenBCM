/*! \file bcma_clicmd.h
 *
 * CLI core commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_H
#define BCMA_CLICMD_H

#include <bcma/cli/bcma_cli.h>

/*! Environment variable name for CLI "if" command error handling. */
#define BCMA_CLICMD_IFERROR "_iferror"

/*! Environment variable name for CLI loop/for/each commands error handling. */
#define BCMA_CLICMD_LOOPERROR "_looperror"

/*! Environment variable name for CLI "rcload" command error handling. */
#define BCMA_CLICMD_RCERROR "_rcerror"

/*!
 * Environment variable name for whether to alias rcload to CLI command
 * exception handler.
 */
#define BCMA_CLICMD_RCLOAD "_rcload"

/*!
 * \brief Add default set of CLI commands.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_clicmd_add_basic_cmds(bcma_cli_t *cli);

/*!
 * \brief Add CLI script commands.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_clicmd_add_script_cmds(bcma_cli_t *cli);

/*!
 * \brief Add CLI rc commands.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_clicmd_add_rc_cmds(bcma_cli_t *cli);

/*!
 * \brief Add CLI directory commands.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_clicmd_add_dir_cmds(bcma_cli_t *cli);

/*!
 * \brief Add CLI system commands.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_clicmd_add_system_cmds(bcma_cli_t *cli);

#endif /* BCMA_CLICMD_H */
