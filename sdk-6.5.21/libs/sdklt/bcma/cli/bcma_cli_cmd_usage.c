/*! \file bcma_cli_cmd_usage.c
 *
 * Display usage of CLI command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <shr/shr_pb.h>

#include <bsl/bsl.h>

#include <bcma/cli/bcma_cli.h>

#include "cli_internal.h"

/*!
 * \brief Print multi-line text with prefix.
 *
 * \param [in] str String to be prefix-printed
 * \param [in] prefix Prefix string applied to each line of text
 *
 * \retval 0 On success
 */
static int
cli_prefix_print(const char *str, const char *prefix)
{
    char buf[80];
    const char *ptr = str;
    const char *line = str;
    int max, len, prefix_len;
    bool empty_prefix = false;
    bool line_cont;

    if (str == NULL || prefix == NULL) {
        return -1;
    }
    prefix_len = sal_strlen(prefix);
    max = sizeof(buf) - prefix_len - 2;

    if (max < 0) {
        return -1;
    }

    while (*ptr) {
        len = ptr - line;
        if (len >= max || *ptr++ == '\n') {
            if (len == max) {
                /* Check for new line or continuous line in boundary case. */
                if (*ptr == '\n') {
                    ptr++;
                } else if (*ptr == '\\' && *(ptr + 1) == '\n') {
                    ptr += 2;
                    len += 1;
                }
            }
            line_cont = (len > 0 && line[len - 1] == '\\' && line[len] == '\n');
            if (line_cont) {
                /* Remove the backslash (\) as well. */
                len--;
            }
            sal_memcpy(buf, line, len);
            buf[len] = '\0';
            if (empty_prefix) {
                cli_out("%*c%s\n", prefix_len, ' ', buf);
            } else {
                cli_out("%s%s\n", prefix, buf);
            }
            empty_prefix = line_cont;
            line = ptr;
        }
    }
    if (empty_prefix) {
        cli_out("%*c%s\n", prefix_len, ' ', line);
    } else {
        cli_out("%s%s\n", prefix, line);
    }

    return 0;
}

int
bcma_cli_cmd_usage_show(bcma_cli_t *cli, bcma_cli_command_t *cmd)
{
    int idx;
    char prefix[32];

    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    if (cmd == NULL) {
        return -1;
    }

    cli_out("\n  SUMMARY:\n\n     %s\n", cmd->desc);
    cli_out("\n  USAGE:\n\n");
    sal_snprintf(prefix, sizeof(prefix)-1, "     %s ", cmd->name);
    cli_prefix_print(cmd->synop, prefix);
    cli_out("\n");
    if (cmd->help[0]) {
        sal_strcpy(prefix, "     ");
        for (idx = 0; idx < COUNTOF(cmd->help) && cmd->help[idx]; idx++) {
            cli_prefix_print(cmd->help[idx], prefix);
        }
        cli_out("\n");
    }
    if (cmd->examples) {
        char *incmd;
        const char *examples;
        shr_pb_t *pb;
        /*
         * Some system commands are executed by redirection.
         * These commands will not occur at the beginning of the command
         * syntax and will be represented by "%s" in the command examples.
         */
        incmd = sal_strstr(cmd->examples, "%s");
        if (incmd) {
            char *src = cmd->examples;
            pb = shr_pb_create();
            while (*src) {
                if (*src == '%' && *(src + 1) == 's') {
                    /* Replace with command name */
                    shr_pb_printf(pb, "%s", cmd->name);
                    src += 2;
                } else {
                    shr_pb_printf(pb, "%c", *src++);
                }
            }
            sal_strcpy(prefix, "     ");
            examples = shr_pb_str(pb);
        } else {
            sal_snprintf(prefix, sizeof(prefix)-1, "     %s ", cmd->name);
            examples = cmd->examples;
        }
        cli_out("  EXAMPLES:\n\n");
        cli_prefix_print(examples, prefix);
        cli_out("\n");
        if (incmd) {
            shr_pb_destroy(pb);
        }
    }

    return 0;
}

int
bcma_cli_cmd_usage_brief_show(bcma_cli_t *cli, bcma_cli_command_t *cmd)
{
    char prefix[32];

    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    if (cmd == NULL) {
        return -1;
    }

    cli_out("\n  USAGE:\n\n");
    sal_snprintf(prefix, sizeof(prefix)-1, "     %s ", cmd->name);
    cli_prefix_print(cmd->synop, prefix);
    cli_out("\n");
    cli_out("  Use \"help %s\" for more information.\n\n", cmd->name);

    return 0;
}
