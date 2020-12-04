/*! \file bcma_envcmd.h
 *
 * CLI Environment commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_ENVCMD_H
#define BCMA_ENVCMD_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Add default set of CLI commands for environment variables.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_envcmd_add_cmds(bcma_cli_t *cli);

#endif /* BCMA_ENVCMD_H */
