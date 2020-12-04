/*! \file bcma_pciecmd.h
 *
 * CLI pcie commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_PCIECMD_H
#define BCMA_PCIECMD_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Add default set of CLI commands for PCIe.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_pciecmd_add_cmds(bcma_cli_t *cli);

#endif /* BCMA_PCIECMD_H */
