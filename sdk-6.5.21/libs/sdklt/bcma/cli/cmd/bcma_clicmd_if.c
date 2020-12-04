/*! \file bcma_clicmd_if.c
 *
 * CLI 'if' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_var.h>

#include <bcma/cli/bcma_clicmd.h>
#include <bcma/cli/bcma_clicmd_if.h>

/*
 * Check the condition result of string and return the boolean result.
 * If string 'c' is numeric, the boolean result is TRUE only if numeric value
 * of 'c' is not 0. Otherwise string 'c' will be took as a command line to
 * execute, and boolean result will be TRUE only if the execution result
 * is successful.
 */
static int
condition_get(bcma_cli_t *cli, const char *c)
{
    int op_not = FALSE;
    int val, cond;

    /* Process for boolean operator NOT */
    while (*c == '!') {
        c++;
        op_not = !op_not;
    }

    if (bcma_cli_parse_int(c, &val) < 0) {
        cond = (bcma_cli_cmd_process(cli, c) == BCMA_CLI_CMD_OK) ? TRUE : FALSE;
    } else  {
        cond = (val != 0) ? TRUE : FALSE;
    }

    return op_not ? !cond : cond;
}

int
bcma_clicmd_if(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = 0, cond = 0;
    int op_and = FALSE, op_or = FALSE;
    const char *c;
    int iferr_stop = bcma_cli_var_bool_get(cli, BCMA_CLICMD_IFERROR, TRUE);

    /* Check condition after if command. */
    do {
        c = BCMA_CLI_ARG_GET(args);
        if (c == NULL) {
            cli_out("%s: missing test condition\n", BCMA_CLI_ARG_CMD(args));
            return BCMA_CLI_CMD_USAGE;
        }

        /*
         * If 'cond' is FALSE and followed by an AND boolean operator,
         * the condition after AND will be ignored. Similarly, if 'cond' is
         * TRUE and followd by an OR boolean operator, the condition
         * after OR will be ignored. Since these ignored condition won't
         * change the condtion result.
         */
        if ((!op_and && !op_or) || (cond && op_and) || (!cond && op_or)) {
            rv = condition_get(cli, c);
            if (op_and) {
                cond = cond && rv;
            } else if (op_or) {
                cond = cond || rv;
            } else {
                cond = rv;
            }
        }

        c = BCMA_CLI_ARG_GET(args);
        if (c == NULL) {
            return BCMA_CLI_CMD_OK;
        }

        /* Check if boolean operator AND or OR exists. */
        op_and = (sal_strcmp(c, "&&") == 0) ? TRUE : FALSE;
        if (!op_and) {
            op_or = (sal_strcmp(c, "||") == 0) ? TRUE : FALSE;
        } else {
            op_or = FALSE;
        }
    } while (op_and || op_or);

    rv = BCMA_CLI_CMD_OK;

    if (cond) {
        /* Execute commands before 'else'. */
        do {
            /* Running command list until 'else' */
            if (sal_strcasecmp(c, "else") == 0) {
                /* Discard arguments after else when condition is TRUE. */
                BCMA_CLI_ARG_DISCARD(args);
                return rv;
            }

            rv = bcma_cli_cmd_process(cli, c);
            if (rv != BCMA_CLI_CMD_OK && iferr_stop) {
                return rv;
            }
        } while ((c = BCMA_CLI_ARG_GET(args)) != NULL);
    } else {
        /* Find commands after 'else' */
        while (sal_strcasecmp(c, "else") != 0) {
            c = BCMA_CLI_ARG_GET(args);
            if (c == NULL) {
                return rv;
            }
        }

        /* Execute commands after 'else'. */
        while ((c = BCMA_CLI_ARG_GET(args)) != NULL) {
            rv = bcma_cli_cmd_process(cli, c);
            if (rv != BCMA_CLI_CMD_OK && iferr_stop) {
                return rv;
            }
        }
    }

    return rv;
}
