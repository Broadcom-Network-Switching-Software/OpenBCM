/*! \file bcma_bslcmd.h
 *
 * CLI core commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BSLCMD_H
#define BCMA_BSLCMD_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Add default set of BSL commands to the given CLI.
 *
 * \param [in] cli CLI object for which to add BSL commands.
 *
 * \return Always 0.
 */
extern int
bcma_bslcmd_add_cmds(bcma_cli_t *cli);

#endif /* BCMA_BSLCMD_H */
