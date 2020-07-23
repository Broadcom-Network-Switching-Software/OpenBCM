/*! \file bcma_clirlc_cmd.c
 *
 * This module supports the auto-completion function of the CLI command.
 *
 * The auto-completion function complies the completion support in
 * GNU readline library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bsl/bsl.h>

#include <bcma/clirlc/bcma_clirlc.h>

static bcma_clirlc_cmd_t *compcmd_root = NULL;

static bcma_clirlc_cmd_t *
complete_cmd_lookup(const char *name)
{
    bcma_clirlc_cmd_t *compcmd;

    if (name == NULL) {
        return NULL;
    }

    compcmd = compcmd_root;
    while (compcmd) {
        if (sal_strcasecmp(compcmd->name, name) == 0) {
            break;
        }
        compcmd = compcmd->next;
    }

    return compcmd;
}

static int
complete_var_init(bcma_clirlc_t *complete)
{
    sal_memset(complete, 0, sizeof(*complete));

    complete->append_char = -1;

    return 0;
}

static char *
command_match(const char *text, int state)
{
    static bcma_cli_cmd_list_t *cl;
    static int len;
    bcma_cli_t *cli = (bcma_cli_t *)bcma_readline_context_get();
    bcma_cli_command_t *cmd;
    char *cmd_name;

    if (state == 0) {
        cl = cli->command_root;
        len = sal_strlen(text);
    }

    while (cl) {
        cmd = cl->cmd;
        if (cmd == NULL) {
            /* Should never get here */
            continue;
        }
        if (!bcma_cli_cmd_enabled(cli, cl->feature)) {
            cl = cl->next;
            continue;
        }
        if (text && sal_strncmp(cmd->name, text, len) != 0) {
            cl = cl->next;
            continue;
        }
        cmd_name = cmd->name;
        cl = cl->next;
        /*
         * The returned string will be freed within the readline library,
         * so we need to ensure that strdup uses the corresponding memory
         * allocation function.
         */
        return bcma_rl_strdup(cmd_name);
    }
    return NULL;
}

static char **
command_completion(const char *text, int start, int end)
{
    bcma_cli_tokens_t *ctoks;
    bcma_cli_t *cli = (bcma_cli_t *)bcma_readline_context_get();
    char **matches = NULL;

    if (cli == NULL) {
        return NULL;
    }

    rl_attempted_completion_over = 1;
    rl_completion_append_character = ' ';

    ctoks = sal_alloc(sizeof(*ctoks), "bcmaCliCmdCompleteTokens");
    if (ctoks == NULL) {
        return NULL;
    }

    /* Tokenize the current input text by space. */
    if (bcma_cli_tokenize(rl_line_buffer, ctoks, " ") < 0) {
        sal_free(ctoks);
        return NULL;
    }

    if (ctoks->argc <= 0 || (ctoks->argc == 1 && start != end)) {
        matches = rl_completion_matches(text, command_match);
        if (matches == NULL) {
            /* Show available commands if no matches. */
            cli_out("\n");
            bcma_cli_show_cmds_avail(cli, NULL);
            rl_on_new_line();
        }
    } else {
        int rv, usage_show = 0;
        bcma_cli_command_t *clicmd;

        rv = bcma_cli_cmd_lookup(cli, ctoks->argv[0], &clicmd);
        if (rv == BCMA_CLI_CMD_OK) {
            bcma_clirlc_cmd_t *compcmd;

            compcmd = complete_cmd_lookup(ctoks->argv[0]);
            if (compcmd && compcmd->func) {
                bcma_clirlc_t complete, *cv = &complete;

                complete_var_init(cv);

                rv = compcmd->func(cli, ctoks, text, start, end, cv);
                if (rv == BCMA_CLI_CMD_OK) {
                    if (cv->entry_func) {
                        matches = rl_completion_matches(text, cv->entry_func);
                    }
                    if (cv->append_char >= 0) {
                        rl_completion_append_character = cv->append_char;
                    }
                } else if (rv == BCMA_CLI_CMD_USAGE) {
                    usage_show = 1;
                }
            } else if (ctoks->argc == 1) {
                usage_show = 1;
            }

            /* Show command usage. */
            if (usage_show) {
                cli_out("\n");
                bcma_cli_cmd_usage_show(cli, clicmd);
                rl_on_new_line();
            }
        }
    }

    sal_free(ctoks);

    return matches;
}

int
bcma_clirlc_init(void)
{
    char wb[8], *ptr = wb;
    char *wbc = "^@=,";

    sal_memset(wb, 0, sizeof(wb));

    rl_attempted_completion_function = command_completion;

    /* Check if the necessary word break characters is set. */
    while (*wbc) {
        if (sal_strchr(rl_basic_word_break_characters, *wbc) == NULL) {
            *ptr++ = *wbc;
        }
        wbc++;
    }

    if (ptr != wb) {
        static char bwbc[32];

        sal_snprintf(bwbc, sizeof(bwbc) - 1, "%s%s",
                     rl_basic_word_break_characters, wb);
        rl_basic_word_break_characters = bwbc;
    }

    return 0;
}

int
bcma_clirlc_add_cmd(bcma_clirlc_cmd_t *compcmd)
{
    bcma_clirlc_cmd_t *p;

    if (compcmd == NULL) {
        compcmd_root = NULL;
        return 0;
    }

    /* Do not reinstall the same complete command structure */
    for (p = compcmd_root; p; p = p->next) {
        if(p == compcmd) {
            /* Already installed */
            return 0;
        }
    }

    compcmd->next = compcmd_root;
    compcmd_root = compcmd;

    return 0;
}

void
bcma_clirlc_info_show(const char *msg)
{
    if (msg) {
        cli_out("\n%s", msg);
        rl_on_new_line();
    }
}
