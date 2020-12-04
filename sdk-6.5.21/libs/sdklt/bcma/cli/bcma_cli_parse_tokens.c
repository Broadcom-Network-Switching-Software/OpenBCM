/*! \file bcma_cli_parse_tokens.c
 *
 * This module parses CLI argument structure to command tokens structure.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bsl/bsl.h>

#include <bcma/cli/bcma_cli_parse.h>


const char *
bcma_cli_parse_tokens(bcma_cli_args_t *args, bcma_cli_tokens_t *ctoks, int max)
{
    int idx;
    bcma_cli_tokens_t *ctok = ctoks;
    const char *arg;
    char *tmpstr;

    sal_memset(ctoks, 0, max * sizeof(*ctoks));

    /* For all arguments */
    for (idx = 0; BCMA_CLI_ARG_CNT(args); idx++, ctok++) {

        arg = BCMA_CLI_ARG_GET(args);

        /* Check for argument overrun */
        if ((idx + 1) >= max) {
            cli_out("%stoo many arguments (>%d)\n",
                    BCMA_CLI_CONFIG_ERROR_STR, max);
            return arg;
        }

        /* Parse each individual argument with '=' into ctok */
        if (bcma_cli_tokenize(arg, ctok, "=") < 0) {
            return arg;
        }
        if (ctok->argc == 2) {
            /*
             * If two tokens are found, we parse a second time in
             * order to parse e.g. "flags=f1,f2,f3" into a single
             * token structure.
             */
            tmpstr = sal_alloc(BCMA_CLI_CONFIG_INPUT_BUFFER_MAX,
                               "bcmaCliParseArgs");
            if (tmpstr == NULL) {
                return arg;
            }
            sal_snprintf(tmpstr, BCMA_CLI_CONFIG_INPUT_BUFFER_MAX, "%s,%s",
                         ctok->argv[0], ctok->argv[1]);

            /* Parse second argument with ',' into ctok */
            if (bcma_cli_tokenize(tmpstr, ctok, ",") < 0) {
                sal_free(tmpstr);
                return arg;
            }
            sal_free(tmpstr);
        }
        else if (ctok->argc != 1) {
            /* Number of tokens must be 1 or 2 */
            return arg;
        }
    }
    /* All args successfully parsed */
    return NULL;
}
