/*! \file bcma_sys_conf_cli_gets.c
 *
 * Implementation of the CLI command input function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <bcma/io/bcma_io_term.h>
#include <bcma/editline/bcma_readline.h>
#include <bcma/cli/bcma_cli_ctrlc.h>

#include <bcma/sys/bcma_sys_conf.h>

static int (*io_gets)(const char *prompt, int max, char *buf);

/*******************************************************************************
 *
 * CLI input function
 */

#define CTRLC_MARK      "^C"

/*
 * Ctrl-C readline wrapper context
 */
typedef struct rl_ctrlc_s {
    const char *prompt;
    char *str;
} rl_ctrlc_t;

/*
 * Ctrl-C readline wrapper
 */
static int
readline_ctrlc(void *data)
{
    rl_ctrlc_t *rl_ctrlc = (rl_ctrlc_t *)data;

    rl_catch_signals = 0;
    rl_ctrlc->str = readline(rl_ctrlc->prompt);
    if (rl_ctrlc->str == NULL) {
        return BCMA_CLI_CMD_EXIT;
    }
    return BCMA_CLI_CMD_OK;
}

/*
 * Readline-based terminal input
 */
static int
readline_gets(const char *prompt, int max, char *buf)
{
    char *str;
    int len;
    int rv;
    rl_ctrlc_t rl_ctrlc;
    bcma_cli_t *cli = bcma_readline_context_get();

    rl_ctrlc.prompt = prompt;
    rl_ctrlc.str = NULL;
    rv = bcma_cli_ctrlc_exec(cli, readline_ctrlc, &rl_ctrlc, 0);
    str = rl_ctrlc.str;
    if (rv == BCMA_CLI_CMD_INTR) {
        /* Ctrl-C was pressed */
        cli_out("%s\n", CTRLC_MARK);
        if (str) {
            bcma_rl_free(str);
        }
        str = NULL;
    }
    if (str == NULL) {
        buf[0] = '\0';
        return rv;
    }

    /* Log the command input from readline to support the CLI 'log' command. */
    LOG_INFO(BSL_LS_APPL_ECHO, (BSL_META("%s%s\n"), prompt, str));

    len = sal_strlen(str) + 1;
    sal_memcpy(buf, str, len > max ? max : len);
    buf[max - 1] = '\0';
    bcma_rl_free(str);
    return BCMA_CLI_CMD_OK;
}

/*
 * Stream-based terminal input
 */
static int
stream_gets(const char *prompt, int max, char *buf)
{
    char *str;
    int len;
    static bool show_prompt = false;
    static bool echo_cmd = false;

    if (show_prompt) {
        bcma_io_term_write(prompt, sal_strlen(prompt));
    }

    str = bcma_io_term_gets(buf, max);
    if (str == NULL) {
        buf[0] = '\0';
        return BCMA_CLI_CMD_EXIT;
    }

    /* Check run time option for stream input */
    if (sal_strchr("+-", str[0])) {
        if (sal_strcmp(str + 1, "prompt\n") == 0) {
            show_prompt = (str[0] == '+');
            buf[0] = '\0';
            return BCMA_CLI_CMD_OK;
        } else if (sal_strcmp(str + 1, "echo\n") == 0) {
            echo_cmd = (str[0] == '+');
            buf[0] = '\0';
            return BCMA_CLI_CMD_OK;
        }
    }

    if (echo_cmd) {
        bcma_io_term_write(str, sal_strlen(str));
    }

    len = sal_strlen(str);

    /* Log the command input from stream to support the CLI 'log' command. */
    LOG_INFO(BSL_LS_APPL_ECHO, (BSL_META("%s%s"), prompt, str));

    /* Continue to read input if the command is not ending */
    while ((len + 1) == max && str[len - 1] != '\n') {
        str = bcma_io_term_gets(buf, max);
        if (str == NULL) {
            break;
        }
        len += sal_strlen(str);
        if (echo_cmd) {
            bcma_io_term_write(str, sal_strlen(str));
        }
        LOG_INFO(BSL_LS_APPL_ECHO, (BSL_META("%s"), str));
    }

    /* Remove the last carriage return read from stream input */
    if (len > 0 && len < max && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Call-back function of CLI command input gets.
 */
static int
cli_cmd_get(bcma_cli_t *cli, const char *prompt, int max, char *buf)
{
    int rv, len;
    char *str = buf;
    const char *p = prompt;

    bcma_readline_context_set(cli);

    while ((rv = io_gets(p, max, str)) == BCMA_CLI_CMD_OK) {
        if (((int)sal_strlen(buf) + 1) >= max) {
            cli_out("%sCommand-line too long\n", BCMA_CLI_CONFIG_ERROR_STR);
            buf[0] = '\0';
            return BCMA_CLI_CMD_BAD_ARG;
        }

        len = sal_strlen(str);
        /* Check for command line continuation */
        if (len >= 1 && str[len - 1] == '\\') {
            /*
             * Perform a continuation prompt if command line ends
             * in a backslash for continuation.
             */
            p = "? ";
            str = str + len - 1;
            max = max - len + 1;
            continue;
        }
        return BCMA_CLI_CMD_OK;
    }
    return rv;
}

/*******************************************************************************
 *
 * Public functions
 */

bcma_cli_gets_f
bcma_sys_conf_cli_gets(bcma_sys_conf_t *sc)
{
    io_gets = readline_gets;
    if (!bcma_io_term_is_tty()) {
        io_gets = stream_gets;
    }
    return cli_cmd_get;
}
