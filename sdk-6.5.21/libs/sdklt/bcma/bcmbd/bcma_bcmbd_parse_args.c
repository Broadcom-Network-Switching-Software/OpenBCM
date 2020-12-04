/*! \file bcma_bcmbd_parse_args.c
 *
 * Implementation of BCMBD CLI command parse functions.
 *
 * Tokenize CLI arguments into BCMBD command tokens.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/bcmbd/bcma_bcmbd.h>

const char *
bcma_bcmbd_parse_args(bcma_cli_args_t *args, bcma_cli_tokens_t *ctoks, int max)
{
    return bcma_cli_parse_tokens(args, ctoks, max);
}
