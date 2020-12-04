/*! \file bcma_bcmpccmd_pcmmu.h
 *
 * CLI 'pcmmu' command for CMICd
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPCCMD_PCMMU_H
#define BCMA_BCMPCCMD_PCMMU_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMPCCMD_PCMMU_DESC  "MMU debug for Port Control."

/*! Syntax for CLI command. */
#define BCMA_BCMPCCMD_PCMMU_SYNOP "[manual|auto] [update]"

/*! Help for CLI command. */
#define BCMA_BCMPCCMD_PCMMU_HELP \
    "Configures and/or initiates an MMU callback."

/*!
 * \brief CLI 'pcmmu' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmpccmd_pcmmu(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMPCCMD_PCMMU_H */
