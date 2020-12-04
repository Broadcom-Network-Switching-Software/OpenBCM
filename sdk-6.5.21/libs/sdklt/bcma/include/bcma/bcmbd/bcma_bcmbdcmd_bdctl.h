/*! \file bcma_bcmbdcmd_bdctl.h
 *
 * BCMBD CLI 'bdctl' command
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_BDCTL_H
#define BCMA_BCMBDCMD_BDCTL_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMBDCMD_BDCTL_DESC  "Base driver control."

/*! Syntax for CLI command. */
#define BCMA_BCMBDCMD_BDCTL_SYNOP \
    "[attach|detach|reset|init|start|stop]"

/*!
 * \brief CLI 'bdctl' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmbdcmd_bdctl(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMBDCMD_BDCTL_H */
