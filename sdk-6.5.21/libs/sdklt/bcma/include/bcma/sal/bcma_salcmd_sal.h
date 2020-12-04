/*! \file bcma_salcmd_sal.h
 *
 * CLI 'sal' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_SALCMD_SAL_H
#define BCMA_SALCMD_SAL_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_SALCMD_SAL_DESC \
    "SAL debug access."

/*! Syntax for CLI command. */
#define BCMA_SALCMD_SAL_SYNOP \
    "alloc trace|notrace|track|status|dump|fill|nofill [-|<prefix>]"

/*! Help for CLI command. */
#define BCMA_SALCMD_SAL_HELP \
    "Track or trace memory allocations through the SAL API.\n\n" \
    "Traced allocations will be shown in real time according\n" \
    "to the specified trace prefix list. Special prefix string\n" \
    "'*' means all and '-' means off.\n\n" \
    "The trace function also supports an exclusion list of\n" \
    "prefix strings (notrace).\n\n" \
    "Tracked allocations will be shown via the dump command\n" \
    "according to the specified track prefix.\n\n" \
    "A tilde (~) in the prefix will match any uppercase character.\n" \
    "This feature can be used to match a prefix which is a\n" \
    "sub-string of another prefix. For example 'bcmlt~' will\n" \
    "match 'bcmltHdl', but not 'bcmltmInfo', while 'bcmlt' will\n" \
    "match both.\n" \
    "Allocated and freed memory will be filled with a fixed\n"  \
    "pattern by default. Use nofill/fill to turn this off/on."

/*! Examples for CLI command. */
#define BCMA_SALCMD_SAL_EXAMPLES \
    "alloc status\n" \
    "alloc trace *\n" \
    "alloc trace bcmpc\n" \
    "alloc trace sal,shr\n" \
    "alloc trace -\n" \
    "alloc notrace cli\n" \
    "alloc track bcmpc\n" \
    "alloc nofill\n" \
    "alloc dump"

/*!
 * \brief CLI 'sal' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_salcmd_sal(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_SALCMD_SAL_H */
