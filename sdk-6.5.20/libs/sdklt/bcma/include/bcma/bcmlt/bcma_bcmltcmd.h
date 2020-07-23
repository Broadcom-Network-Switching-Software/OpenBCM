/*! \file bcma_bcmltcmd.h
 *
 * CLI commands for logical and physical table access.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMLTCMD_H
#define BCMA_BCMLTCMD_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Add CLI commands for logical and physical table access.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_bcmltcmd_add_cmds(bcma_cli_t *cli);

/*!
 * \brief Add CLI commands for logical tables application.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_bcmltcmd_add_appl_cmds(bcma_cli_t *cli);

#endif /* BCMA_BCMLTCMD_H */
