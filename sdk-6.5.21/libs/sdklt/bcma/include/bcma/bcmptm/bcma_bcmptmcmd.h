/*! \file bcma_bcmptmcmd.h
 *
 * CLI commands for PTM component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPTMCMD_H
#define BCMA_BCMPTMCMD_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Add default set of CLI commands for the PTM API.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_bcmptmcmd_add_cmds(bcma_cli_t *cli);

#endif /* BCMA_BCMPTMCMD_H */
