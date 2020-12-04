/*! \file bcma_bcmbdcmd_cmicd.h
 *
 * BCMBD CMICd CLI commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_CMICD_H
#define BCMA_BCMBDCMD_CMICD_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Add default set of CLI commands for BCMBD CMICd devices.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern void
bcma_bcmbdcmd_add_cmicd_cmds(bcma_cli_t *cli);

#endif /* BCMA_BCMBDCMD_CMICD_H */
