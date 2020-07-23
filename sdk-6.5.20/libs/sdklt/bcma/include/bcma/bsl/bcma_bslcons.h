/*! \file bcma_bslcons.h
 *
 * Console sink.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BSLCONS_H
#define BCMA_BSLCONS_H

/*!
 * \brief Initialize console sink.
 *
 * \retval 0 No errors
 */
int
bcma_bslcons_init(void);

/*!
 * \brief Check if console is enabled.
 *
 * \retval Non-zero Console is enabled.
 * \retval 0 Console is not enabled.
 */
int
bcma_bslcons_is_enabled(void);

/*!
 * \brief Enable or disable console output.
 *
 * Log messages will be dropped unconditionally if the console is not
 * enabled.
 *
 * \param [in] enable Set to 0 to disable console output.
 *
 * \return Previous enable value.
 */
int
bcma_bslcons_enable(int enable);

/*!
 * \brief Check if CLI-only console mode is active.
 *
 * \retval Non-Zero CLI-only console mode is active.
 * \retval 0 Console is not enabled.
 */
extern int
bcma_bslcons_cli_only_get(void);

/*!
 * \brief Enter or exit CLI-only console mode.
 *
 * Non-CLI log messages will be dropped unconditionally if CLI-only
 * mode is active.
 *
 * CLI log messages will still be dropped in CLI-only mode if the
 * console is not enabled (see \ref bcma_bslcons_enable).
 *
 * \param [in] new_val Set to non-zero to enter CLI-only console mode.
 *
 * \return Previous CLI-only value.
 */
extern int
bcma_bslcons_cli_only_set(int new_val);

#endif /* BCMA_BSLCONS_H */
