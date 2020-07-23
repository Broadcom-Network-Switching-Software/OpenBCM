/*! \file bcma_cli_unit.h
 *
 * Functions about unit information operation in CLI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLI_UNIT_H
#define BCMA_CLI_UNIT_H

#include <bcma/cli/bcma_cli.h>

extern int
/*!
 * \brief Set current unit in the CLI.
 *
 * If the CLI is operating on multiple units, setting the unit number
 * will append the unit number to the CLI prompt.
 *
 * \param [in] cli CLI object.
 * \param [in] unit Unit number.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
bcma_cli_unit_set(bcma_cli_t *cli, int unit);

/*!
 * \brief Install call-back vectors for unit management.
 *
 * An application can add support for unit management by installing a
 * set of call-back functions.
 *
 * Please refer to \ref bcma_cli_unit_cb_t for more information about each
 * call-back function.
 *
 * \param [in] cli CLI object.
 * \param [in] unit_cb Call-back functions.
 * \param [in] cookie Pass-thru data for call-back functions.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_unit_cb_set(bcma_cli_t *cli,
                     const bcma_cli_unit_cb_t *unit_cb, void *cookie);

/*!
 * \brief Wrapper for CLI maximum units number call-back function.
 *
 * This function will ensure that parameters are validated and that a
 * default behavior is defined if no call-back is installed.
 *
 * \param [in] cli CLI object.
 *
 * \retval Please refer to \ref bcma_cli_unit_max_f.
 */
extern int
bcma_cli_unit_max(bcma_cli_t *cli);

/*!
 * \brief Wrapper for CLI unit number validation call-back function.
 *
 * This function will ensure that parameters are validated and that a
 * default behavior is defined if no call-back is installed.
 *
 * \param [in] cli CLI object.
 * \param [in] unit Unit number.
 *
 * \retval Please refer to \ref bcma_cli_unit_valid_f.
 */
extern int
bcma_cli_unit_valid(bcma_cli_t *cli, int unit);

/*!
 * \brief Wrapper for CLI unit feature call-back function.
 *
 * This function will ensure that parameters are validated and that a
 * default behavior is defined if no call-back is installed.
 *
 * \param [in] cli CLI object.
 * \param [in] unit Unit number.
 * \param [in] feature Feature to check for.
 *
 * \retval Please refer to \ref bcma_cli_unit_feature_enabled_f.
 */
extern int
bcma_cli_unit_feature_enabled(bcma_cli_t *cli, int unit, int feature);

#endif /* BCMA_CLI_UNIT_H */
