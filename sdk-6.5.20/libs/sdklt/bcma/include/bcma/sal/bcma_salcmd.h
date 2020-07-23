/*! \file bcma_salcmd.h
 *
 * SAL debug CLI commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_SALCMD_H
#define BCMA_SALCMD_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Add default set of CLI commands for SAL debug.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern void
bcma_salcmd_add_sal_cmds(bcma_cli_t *cli);

#endif /* BCMA_SALCMD_H */
