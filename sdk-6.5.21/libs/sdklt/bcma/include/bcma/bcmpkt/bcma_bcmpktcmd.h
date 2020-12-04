/*! \file bcma_bcmpktcmd.h
 *
 * CLI commands for packet I/O access.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPKTCMD_H
#define BCMA_BCMPKTCMD_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Add CLI commands for packet I/O access.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_bcmpktcmd_add_cmds(bcma_cli_t *cli);

#endif /* BCMA_BCMPKTCMD_H */
