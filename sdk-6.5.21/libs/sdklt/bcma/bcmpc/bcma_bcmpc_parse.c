/*! \file bcma_bcmpc_parse.c
 *
 * Implementation of BCMPC CLI command parse functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/bcmpc/bcma_bcmpc.h>

static int
do_parse_desc(const char *str, bcma_cli_tokens_t *ctok,
              bcma_bcmpc_parse_desc_t *desc)
{
    /*
     * The string format to be parsed is:
     * <reg_name>|<reg_addr>[.<lane_index>][.<pll_index>]
     * reg_addr = 0x1NNNN when devad = 1, register address is PMD
     * reg_addr = 0xNNNN when devad = 0, register address is PCS
     */
    bcma_bcmpc_parse_desc_t_init(desc);

    if (bcma_cli_tokenize(str, ctok, ".") < 0) {
        return -1;
    }
    if (ctok->argc > 3) {
        return -1;
    }

    if (bcma_cli_parse_is_int(ctok->argv[0])) {
        /* address */
        if (bcma_cli_parse_uint32(ctok->argv[0], &desc->name32)) {
            return -1;
        }
    } else {
        /* register symbol */
        sal_strlcpy(desc->name, ctok->argv[0], sizeof(desc->name));
    }
    /* lane index */
    if (ctok->argc == 2) {
        if (bcma_cli_parse_uint32(ctok->argv[1], &desc->lane_index) < 0) {
            return -1;
        }
    }
    /* pll index */
    if (ctok->argc == 3) {
        if (bcma_cli_parse_uint32(ctok->argv[2], &desc->pll_index) < 0) {
            return -1;
        }
    }

    return 0;
}

int
bcma_bcmpc_parse_desc(const char *str, bcma_bcmpc_parse_desc_t *desc)
{
    int rv;
    bcma_cli_tokens_t *ctok;

    if (str == NULL || desc == NULL) {
        return -1;
    }

    ctok = sal_alloc(sizeof(*ctok), "bcmaCliPcParseCtok");
    if (ctok == NULL) {
        return -1;
    }

    rv = do_parse_desc(str, ctok, desc);

    sal_free(ctok);

    return rv;
}

int
bcma_bcmpc_parse_error(const char *desc, const char *arg)
{
    if (arg) {
        cli_out("%sunable to parse %s%sargument '%s'\n",
                BCMA_CLI_CONFIG_ERROR_STR, desc, desc[0] ? " " : "", arg);
    }
    else {
        cli_out("%smissing %s argument\n",
                BCMA_CLI_CONFIG_ERROR_STR, desc);
    }
    return -1;
}

const char *
bcma_bcmpc_parse_args(bcma_cli_args_t *args, bcma_cli_tokens_t *ctoks, int max)
{
    int idx;
    bcma_cli_tokens_t *ctok = ctoks;
    const char *arg;

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

        if (ctok->argc != 1 && ctok->argc != 2) {
            /* Number of tokens must be 1 or 2 */
            return arg;
        }
    }
    /* All args successfully parsed */
    return NULL;
}
