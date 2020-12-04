/*! \file bcma_bcmltcmd_lt_complete.c
 *
 * Completion support for logical table access command in CLI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/bcmlt/bcma_bcmltcmd_complete.h>

#include "bcmltcmd_complete_internal.h"

int
bcma_bcmltcmd_lt_complete(bcma_cli_t *cli, bcma_cli_tokens_t *ctoks,
                          const char *text, int start, int end,
                          bcma_clirlc_t *complete)
{
    return bcmltcmd_complete(cli, ctoks, false, text, start, end, complete);
}
