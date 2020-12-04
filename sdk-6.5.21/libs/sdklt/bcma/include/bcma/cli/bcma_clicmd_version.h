/*! \file bcma_clicmd_version.h
 *
 * CLI 'version' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_VERSION_H
#define BCMA_CLICMD_VERSION_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_VERSION_DESC \
    "Show version information."

/*!
 * \brief CLI command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_version(bcma_cli_t *cli, bcma_cli_args_t *args);

/*!
 * \brief Set version string.
 *
 * Update string to be printed by the CLI \c version command.
 *
 * No local copy of the version string is created, so the provided
 * version string is presumed to static or const.
 *
 * \param [in] vstr Version string.
 *
 * \retval 0 No errors
 */
extern int
bcma_clicmd_version_string_set(const char *vstr);

/*!
 * \brief Get version string.
 *
 * \return Version string.
 */
extern const char *
bcma_clicmd_version_string_get(void);

#endif /* BCMA_CLICMD_VERSION_H */
