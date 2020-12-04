/*! \file bcma_testcmd.h
 *
 * Test CLI command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCMD_H
#define BCMA_TESTCMD_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Add default set of CLI commands for Test component.
 *
 * \param[in] cli CLI object
 */
extern void
bcma_testcmd_add_cmds(bcma_cli_t *cli);

#endif /* BCMA_TESTCMD_H */
