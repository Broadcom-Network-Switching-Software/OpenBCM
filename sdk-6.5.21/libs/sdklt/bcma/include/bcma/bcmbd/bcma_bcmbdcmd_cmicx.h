/*! \file bcma_bcmbdcmd_cmicx.h
 *
 * BCMBD CMICx CLI commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_CMICX_H
#define BCMA_BCMBDCMD_CMICX_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Add default set of CLI commands for BCMBD CMICx devices.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern void
bcma_bcmbdcmd_add_cmicx_cmds(bcma_cli_t *cli);

#endif /* BCMA_BCMBDCMD_CMICX_H */
