/*! \file bcma_bcmpccmd.h
 *
 * CLI core commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPCCMD_H
#define BCMA_BCMPCCMD_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Add default set of CLI commands for the PC API.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_bcmpccmd_add_cmds(bcma_cli_t *cli);

/*!
 * \brief Add debug set of CLI commands for the PC API.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_bcmpccmd_add_dbg_cmds(bcma_cli_t *cli);

/*!
 * \brief Add the CLI commands for internal PHY register access.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_bcmpccmd_add_phy_cmds(bcma_cli_t *cli);

/*!
 * \brief Add the CLI commands for port diagnostics.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_bcmpccmd_add_diag_cmds(bcma_cli_t *cli);

#endif /* BCMA_BCMPCCMD_H */
