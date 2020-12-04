/*! \file bcma_bcmdicmd.h
 *
 * CLI commands for firmware loader.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMDICMD_H
#define BCMA_BCMDICMD_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Add CLI commands for firmware loader.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_bcmdicmd_add_cmds(bcma_cli_t *cli);

#endif /* BCMA_BCMDICMD_H */
