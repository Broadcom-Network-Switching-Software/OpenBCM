/*! \file bcma_tecsupcmd.h
 *
 * CLI tecsup commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TECSUPCMD_H
#define BCMA_TECSUPCMD_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Add default set of CLI commands for tecsup.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_tecsupcmd_add_cmds(bcma_cli_t *cli);

#endif /* BCMA_TECSUPCMD_H */
