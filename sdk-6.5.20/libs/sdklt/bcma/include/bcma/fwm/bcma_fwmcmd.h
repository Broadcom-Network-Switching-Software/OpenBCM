/*! \file bcma_fwmcmd.h
 *
 * CLI command for Firmware Management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_FWMCMD_H
#define BCMA_FWMCMD_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Add default set of CLI commands for Firmware Management.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_fwmcmd_add_cmds(bcma_cli_t *cli);

#endif /* BCMA_FWMCMD_H */
