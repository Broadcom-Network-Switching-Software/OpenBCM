/*! \file bcma_cli_token.c
 *
 * CLI token functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <bcma/cli/bcma_cli_token.h>

static int
cmd_split(char *str, char *argv[], int max, const char *delim)
{
    int idx, argc = 0;
    char* ptr;

    assert(str);
    assert(argv);

    /* Default delimiter is space */
    if (delim == NULL) {
        delim = " ";
    }

    /*
     * Break the input string up into chunks delimited by
     * characters in the delimiter string.
     */
    ptr = str;
    for(idx = 0; idx < max && *ptr; idx++) {
        while (*ptr && sal_strchr(delim, *ptr)) {
            ptr++;
        }
        if (*ptr == 0) {
            break;
        }
        argv[idx] = ptr;
        argc++;
        while (*ptr && !sal_strchr(delim, *ptr)) {
            ptr++;
        }
        if (*ptr) {
            *ptr++ = 0;
        }
    }

    return argc;
}

int
bcma_cli_tokenize(const char *str, bcma_cli_tokens_t *tok, const char *delim)
{
    assert(tok);

    /* Clear the tokens structure */
    sal_memset(tok, 0, sizeof(*tok));

    /* Do not modify source string */
    sal_strlcpy(tok->str, str, sizeof(tok->str));

    tok->argc = cmd_split(tok->str, tok->argv,
                          BCMA_CLI_CONFIG_ARGS_CNT_MAX, delim);

    return 0;
}
