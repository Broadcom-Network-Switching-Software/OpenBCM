/*! \file bcma_sys_conf_cli_history_add.c
 *
 * Implementation of the CLI command history support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <bcma/editline/bcma_readline.h>

#include <bcma/sys/bcma_sys_conf.h>

/*******************************************************************************
 *
 * Private functions
 */

/*
 * Expand command from history.
 */
static int
cmd_history_expand(int max, char *str)
{
    int rv;
    char *expansion;

    rv = history_expand(str, &expansion);
    if (rv) {
        cli_out("%s%s\n",
                rv < 0 ? BCMA_CLI_CONFIG_ERROR_STR : "", expansion);
        if (rv == 1) {
            /* Expand successfully */
            sal_strlcpy(str, expansion, max);
        }
    }
    if (expansion) {
        bcma_rl_free(expansion);
    }
    return rv;
}

/*
 * Add command string to input history.
 */
static void
cmd_history_add(char *str)
{
    char *p = str;
    char *cmd_last;

    /* Remove the heading spaces if any. */
    while (p && sal_isspace(*p)) {
        p++;
    }
    /* Do no add empty string to history. */
    if (!p || !*p) {
        return;
    }
    /* Do not add the last duplicate command to history */
    cmd_last = bcma_editline_history_get(-1);
    if (cmd_last && sal_strcmp(p, cmd_last) == 0) {
        return;
    }
    add_history(p);
}

/*
 * Call-back function for CLI command history add.
 */
static void
cli_cmd_history_add(int max, char *str)
{
    int rv;

    rv = cmd_history_expand(max, str);
    if (rv < 0 || rv == 2) {
        /* Error in expansion or the returned line should not be executed */
        return;
    }
    /* Add command to history */
    cmd_history_add(str);
}

/*******************************************************************************
 *
 * Public functions
 */

bcma_cli_history_add_f
bcma_sys_conf_cli_history_add(bcma_sys_conf_t *sc)
{
    return cli_cmd_history_add;
}
