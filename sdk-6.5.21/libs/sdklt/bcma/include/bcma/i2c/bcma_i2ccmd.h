/*! \file bcma_i2ccmd.h
 *
 * CLI 'i2c' command for serial I2C control.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_I2CCMD_H
#define BCMA_I2CCMD_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Add default set of CLI commands for i2c.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_i2ccmd_add_cmds(bcma_cli_t *cli);

#endif /* BCMA_I2CCMD_H */
