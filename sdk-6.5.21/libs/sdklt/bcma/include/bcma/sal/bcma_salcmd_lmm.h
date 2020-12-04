/*! \file bcma_salcmd_lmm.h
 *
 * CLI 'lmm' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_SALCMD_LMM_H
#define BCMA_SALCMD_LMM_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_SALCMD_LMM_DESC  \
    "Memory usage statistics from Local Memory Manager."

/*! Syntax for CLI command. */
#define BCMA_SALCMD_LMM_SYNOP \
    "stats [<ID-string1> [<ID-string2>] ...]"

/*!
 * \brief CLI 'lmm' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_salcmd_lmm(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_SALCMD_LMM_H */
