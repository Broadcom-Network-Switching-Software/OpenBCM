/*! \file bcma_bcmbdcmd_dev.h
 *
 * BCMBD device CLI commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_DEV_H
#define BCMA_BCMBDCMD_DEV_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Add default set of CLI commands for BCMBD devices.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern void
bcma_bcmbdcmd_add_dev_cmds(bcma_cli_t *cli);

#endif /* BCMA_BCMBDCMD_DEV_H */
