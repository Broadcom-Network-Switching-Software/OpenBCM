/*! \file bcma_cfgdbcmd.h
 *
 * CLI Configuration database commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CFGDBCMD_H
#define BCMA_CFGDBCMD_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Add CLI configuration database commands.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_cfgdbcmd_add_cmds(bcma_cli_t *cli);

#endif /* BCMA_CFGDBCMD_H */
