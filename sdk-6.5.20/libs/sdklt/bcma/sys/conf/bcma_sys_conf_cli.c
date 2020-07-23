/*! \file bcma_sys_conf_cli.c
 *
 * Basic initialization of SDK core libraries.
 *
 * The intention is to avoid code duplication across multiple
 * SDK-based applications, but the code may also serve as a template
 * for a customer-equivalent.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_assert.h>
#include <sal/sal_sysrq.h>

#include <bcma/io/bcma_io_term.h>
#include <bcma/io/bcma_io_sig.h>
#include <bcma/io/bcma_io_ctrlc.h>
#include <bcma/io/bcma_io_file.h>
#include <bcma/env/bcma_env.h>
#include <bcma/env/bcma_envcmd.h>
#include <bcma/cli/bcma_cli_var.h>
#include <bcma/cli/bcma_cli_tvar.h>
#include <bcma/cli/bcma_cli_ctrlc.h>
#include <bcma/cli/bcma_cli_redir.h>
#include <bcma/cli/bcma_cli_bshell.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_clicmd.h>
#include <bcma/cli/bcma_clicmd_rcload.h>
#include <bcma/editline/bcma_readline.h>
#include <bcma/editline/bcma_editlinecmd.h>

#include <bcma/sys/bcma_sys_conf.h>

/*******************************************************************************
 *
 * For use by SDK-version of readline
 */

/*
 * Console hooks required by SDK-version of readline
 */
static bcma_editline_io_cb_t el_io_cb = {
    bcma_io_term_read,
    bcma_io_term_write,
    bcma_io_term_mode_set,
    bcma_io_term_winsize_get,
    bcma_io_send_sigint,
    bcma_io_send_sigquit
};

/*******************************************************************************
 *
 * CLI environment variables
 */

static const char *
var_get(void *cookie, const char *name)
{
    return bcma_env_var_get((bcma_env_handle_t)cookie, name);
}

static int
var_set(void *cookie, const char *name, const char *value, int local)
{
    if (value == NULL) {
        return bcma_env_var_unset((bcma_env_handle_t)cookie, name,
                                  local, !local);
    }

    return bcma_env_var_set((bcma_env_handle_t)cookie, name, value, local);
}

static int
var_scope_push(void *cookie)
{
    return bcma_env_scope_push((bcma_env_handle_t)cookie);
}

static int
var_scope_pop(void *cookie)
{
    return bcma_env_scope_pop((bcma_env_handle_t)cookie);
}

static bcma_cli_var_cb_t var_cb = {
    .var_get = var_get,
    .var_set = var_set,
    .var_scope_push = var_scope_push,
    .var_scope_pop = var_scope_pop
};

/*******************************************************************************
 *
 * CLI environment variables with tag support
 */

typedef struct unset_trvs_cb_data_s {
    uint32_t tag;
    int cnt;
} unset_trvs_cb_data_t;

static int
tvar_unset_by_tag_traverse_cb(void *user_data, const char *name,
                              const char *value, uint32_t *tag)
{
    unset_trvs_cb_data_t *cb_data = (unset_trvs_cb_data_t *)user_data;

    /* Delete the variable with the same tag */
    if (cb_data->tag == *tag) {
        cb_data->cnt++;
        return BCMA_ENV_VAR_TRAVERSE_DELETE;
    }
    return 0;
}

static const char *
tvar_get(void *cookie, const char *name, uint32_t *tag)
{
    const char *value;

    value = bcma_env_var_get((bcma_env_handle_t)cookie, name);
    if (value && tag) {
        bcma_env_var_tag_get((bcma_env_handle_t)cookie, name, tag);
    }
    return value;
}

static int
tvar_set(void *cookie, const char *name, const char *value, uint32_t tag,
         int local)
{
    int rv;

    if (value == NULL) {
        return bcma_env_var_unset((bcma_env_handle_t)cookie, name,
                                  local, !local);
    }

    rv = bcma_env_var_set((bcma_env_handle_t)cookie, name, value, local);
    rv += bcma_env_var_tag_set((bcma_env_handle_t)cookie, name, tag, local);

    return rv < 0 ? -1 : 0;
}

static int
tvar_unset_by_tag(void *cookie, uint32_t tag, int local)
{
    unset_trvs_cb_data_t cb_data;

    sal_memset(&cb_data, 0, sizeof(cb_data));
    cb_data.tag = tag;
    bcma_env_var_traverse((bcma_env_handle_t)cookie, local ? 1 : 0,
                          tvar_unset_by_tag_traverse_cb, &cb_data);
    return cb_data.cnt;
}

static bcma_cli_tvar_cb_t tvar_cb = {
    .tvar_get = tvar_get,
    .tvar_set = tvar_set,
    .tvar_unset_by_tag = tvar_unset_by_tag
};

/*******************************************************************************
 *
 * CLI Ctrl-C handling
 */

static int
ctrlc_enable_set(void *cookie, int mode)
{
    return bcma_io_ctrlc_enable_set(mode);
}

static int
ctrlc_exec(void *cookie, int (*func)(void *), void *data,
           int (*ctrlc_sig_cb)(void *), void *cb_data, int rv_intr)
{
    return bcma_io_ctrlc_exec(func, data, ctrlc_sig_cb, cb_data, rv_intr);
}

static bcma_cli_ctrlc_cb_t bsh_ctrlc_cb = {
    .ctrlc_enable_set = ctrlc_enable_set,
    .ctrlc_exec = ctrlc_exec,
};

static bcma_cli_ctrlc_cb_t dsh_ctrlc_cb = {
    /* Support Ctrl-C enable/disable in main shell only. */
    .ctrlc_enable_set = NULL,
    .ctrlc_exec = ctrlc_exec,
};

/*******************************************************************************
 *
 * CLI I/O redirection handling
 */

#ifndef REDIR_FILE_PREFIX
#define REDIR_FILE_PREFIX       "/tmp/bcmcli."
#endif
#define REDIR_FILE_TEMPLATE     "XXXXXX"

#define REDIR_LINE_SIZE_MAX     1024

/* Data for local Ctrl-C trap */
typedef struct redir_flush_s {
    bcma_io_file_handle_t ofile;
    size_t bufsz;
    char *buf;
} redir_flush_t;

static int redir_enable;
static char redir_input_name[] = REDIR_FILE_PREFIX "in." REDIR_FILE_TEMPLATE;
static char redir_output_name[] = REDIR_FILE_PREFIX "out." REDIR_FILE_TEMPLATE;

static void
redir_init_tempfile_name(char *file_name)
{
    char *ptr = sal_strrchr(file_name, '.');

    if (ptr == NULL || sal_strlen(ptr) < sizeof(REDIR_FILE_TEMPLATE)) {
        /* Should never get here */
        assert(0);
        return;
    }
    sal_memcpy(ptr + 1, REDIR_FILE_TEMPLATE, sizeof(REDIR_FILE_TEMPLATE));
}

static int
redir_enable_set(void *cookie, int enable)
{
    /* Update status */
    redir_enable = (enable > 0) ? 1 : 0;

    if (enable > 0) {
        /* Create temporary files for redirection */
        bcma_io_file_mktemp(redir_input_name);
        bcma_io_file_mktemp(redir_output_name);
    } else if (enable < 0) {
        /* Reset the temporary files */
        bcma_io_file_delete(redir_input_name);
        bcma_io_file_delete(redir_output_name);
        redir_init_tempfile_name(redir_input_name);
        redir_init_tempfile_name(redir_output_name);
    }

    return 0;
}

/*
 * Use local Ctrl-C trap to ensure that temporary buffer is freed and
 * redirection files are deleted even if Ctrl-C is pressed while the
 * output file is being dumped.
 */
static int
redir_flush_ctrlc(void *data)
{
    redir_flush_t *rf = (redir_flush_t *)data;

    /* Dump output file line by line */
    while (bcma_io_file_gets(rf->ofile, rf->buf, rf->bufsz - 1) != NULL) {
        if (bcma_cli_ctrlc_break()) {
            break;
        }
        cli_out("%s", rf->buf);
    }

    return 0;
}

static int
redir_cmd_done(void *cookie)
{
    bcma_cli_t *cli = (bcma_cli_t *)cookie;
    int rv = 0;
    redir_flush_t redir_flush, *rf = &redir_flush;

    if (redir_enable) {
        /* Repurpose any existing output as input */
        if (bcma_io_file_move(redir_output_name, redir_input_name) != 0) {
            rv = -1;
        }
        /* Recreate an output file for the following command */
        redir_init_tempfile_name(redir_output_name);
        bcma_io_file_mktemp(redir_output_name);
    } else {
        /* Dump output and remove temporary files */
        rf->ofile = bcma_io_file_open(redir_output_name, "r");
        if (rf->ofile) {
            /* Allocate temporary message buffer */
            rf->bufsz = REDIR_LINE_SIZE_MAX;
            rf->buf = sal_alloc(rf->bufsz, "bcmaSysCliRedir");
            if (rf->buf) {
                sal_memset(rf->buf, 0, rf->bufsz);
                /* Dump most recent CLI command output */
                (void)bcma_cli_ctrlc_exec(cli, redir_flush_ctrlc, rf, 1);
                sal_free(rf->buf);
            }
            bcma_io_file_close(rf->ofile);
        }
        if (bcma_io_file_delete(redir_input_name) != 0) {
            rv = -1;
        }
        if (bcma_io_file_delete(redir_output_name) != 0) {
            rv = -1;
        }
        /*
         * To ensure retrieving a unique temporary file name, reinitialize
         * the temporary file name after the file is deleted.
         */
        redir_init_tempfile_name(redir_input_name);
        redir_init_tempfile_name(redir_output_name);
    }

    return rv;
}

static const char *
redir_input_name_get(void *cookie)
{
    return redir_input_name;
}

static const char *
redir_output_name_get(void *cookie)
{
    return redir_output_name;
}

static bcma_cli_redir_cb_t redir_cb = {
    redir_enable_set,
    redir_cmd_done,
    redir_input_name_get,
    redir_output_name_get
};

static int
redir_out_hook(bsl_meta_t *meta, const char *format, va_list args)
{
    bcma_io_file_handle_t ofile;

    /* We only redirect output from the CLI */
    if (redir_enable == 0 || meta->source != BSL_SRC_SHELL) {
        return -1;
    }

    /* Append log message to output file */
    ofile = bcma_io_file_open(redir_output_name, "a");
    if (ofile) {
        bcma_io_file_vprintf(ofile, format, args);
        bcma_io_file_close(ofile);
    }

    return 0;
}

/*******************************************************************************
 *
 * CLI command exception handling
 */

static int
cmd_exception(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *filename = BCMA_CLI_ARG_GET(args);

    return bcma_clicmd_rcload_file(cli, args, filename, true);
}

/*******************************************************************************
 *
 * Debug shell command
 */

/* Local copy of debug shell handle to launch debug shell. */
static bcma_cli_t *dsh;

static int
debugshell_run_ctrlc(void *data)
{
    bcma_cli_t *cli = (bcma_cli_t *)data;

    bcma_cli_cmd_loop(cli);

    return 0;
}

static int
bcma_clicmd_debugshell(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *opt;

    /* Sanity check */
    if (!dsh || !cli) {
        return BCMA_CLI_CMD_FAIL;
    }

    /* Sync current unit and command unit for debug shell */
    dsh->cur_unit = cli->cur_unit;
    dsh->cmd_unit = cli->cmd_unit;

    opt = BCMA_CLI_ARG_GET(args);
    if (opt && sal_strcmp(opt, "-c") == 0) {
        /* Execute debug command */
        bcma_cli_cmd_process(dsh, BCMA_CLI_ARG_GET(args));
    } else {
        /* Run debug shell */
        int rv;

        /*
         * Use local Ctrl-C trap to ensure that the debug shell still runs
         * when Ctrl-C is pressed.
         */
        do {
            rv = bcma_cli_ctrlc_exec(dsh, debugshell_run_ctrlc, dsh, 1);
        } while (rv == BCMA_CLI_CMD_INTR);
    }

    return BCMA_CLI_CMD_OK;
}

/* CLI system command set */
static bcma_cli_command_t cmd_debugshell = {
    .name = "DebugSHell",
    .func = bcma_clicmd_debugshell,
    .desc = "Debug shell.",
    .synop = "[-c <command>]",
    .help = {
        "Without any options, this command will launch the debug shell.\n" \
        "With the -c option, the debug shell will execute the specified\n" \
        "command and return."
    },
    .examples = "\n" \
                "-c \"get CMIC_DEV_REV_IDr\""
};

/*******************************************************************************
 *
 * Assert handler
 */

static void
sdk_assert_handler(const char *expr, const char *file, int line)
{
    char buf[1];

    cli_out("ERROR: Assertion failed: (%s) at %s:%d\n", expr, file, line);
    sal_sysrq(SAL_SYSRQ_BACKTRACE, 0);

    cli_out("ERROR: Continue or quit [C/q]: ");
    if (bcma_io_term_read(buf, sizeof(buf)) > 0) {
        if (sal_tolower(buf[0]) == 'c' || buf[0] == '\n') {
            cli_out("WARNING: Correct behavior no longer guaranteed.\n");
            return;
        }
    }

    sal_sysrq(SAL_SYSRQ_ABORT, 0);
}

static int
bcma_cli_add_assert_handler(void)
{
    sal_assert_set(sdk_assert_handler);
    return 0;
}

/*******************************************************************************
 *
 * Common CLI initialization
 */

static int
basic_cli_init(bcma_cli_t *cli, bcma_cli_ctrlc_cb_t *ctrlc_cb)
{
    /* Add basic CLI commands */
    bcma_clicmd_add_basic_cmds(cli);
    bcma_clicmd_add_script_cmds(cli);
    bcma_clicmd_add_rc_cmds(cli);
    bcma_clicmd_add_dir_cmds(cli);
    bcma_clicmd_add_system_cmds(cli);

    /* Add support for Ctrl-C exceptions in CLI */
    bcma_cli_ctrlc_cb_set(cli, ctrlc_cb, NULL);

    /* Add support for redirection in CLI */
    bcma_cli_redir_cb_set(cli, &redir_cb, cli);

    /* Add support for command exceptions in CLI */
    bcma_cli_cmd_exception_set(cli, cmd_exception);

    /* Enable Ctrl-D to exit */
    bcma_cli_eof_exit_set(cli, true);

    return 0;
}

/*******************************************************************************
 *
 * Public functions
 */

int
bcma_sys_conf_cli_init(bcma_sys_conf_t *sc)
{
    bcma_cli_gets_f io_gets = bcma_sys_conf_cli_gets(sc);
    bcma_cli_history_add_f io_history_add = bcma_sys_conf_cli_history_add(sc);

    /* Initialize SDK-version of readline */
    bcma_editline_init(&el_io_cb, NULL);

    /* Create basic CLI */
    sc->cli = bcma_cli_create();
    bcma_cli_input_cb_set(sc->cli, "BCMLT", io_gets, io_history_add);
    bcma_cli_prompt_prefix_cb_set(sc->cli, sc->cli_prompt_prefix_cb);
    basic_cli_init(sc->cli, &bsh_ctrlc_cb);

    /* Add support for CLI environment variables and tagged variables */
    sc->eh = bcma_env_create(0);
    bcma_cli_var_cb_set(sc->cli, &var_cb, sc->eh);
    bcma_cli_tvar_cb_set(sc->cli, &tvar_cb, sc->eh);
    bcma_envcmd_add_cmds(sc->cli);

    /* Add editline commands support */
    bcma_editlinecmd_add_cmds(sc->cli);

    /* Create debug shell */
    sc->dsh = bcma_cli_create();
    dsh = sc->dsh;
    bcma_cli_input_cb_set(sc->dsh, "sdklt", io_gets, io_history_add);
    bcma_cli_prompt_prefix_cb_set(sc->dsh, sc->cli_prompt_prefix_cb);
    basic_cli_init(sc->dsh, &dsh_ctrlc_cb);

    /* Debug shell uses same environment as the main CLI */
    bcma_cli_var_cb_set(sc->dsh, &var_cb, sc->eh);
    bcma_envcmd_add_cmds(sc->dsh);

    /* Add editline commands support for debug shell */
    bcma_editlinecmd_add_cmds(sc->dsh);

    bcma_cli_add_command(sc->cli, &cmd_debugshell, 0);

    /* Create assert handler to skip core dump */
    bcma_cli_add_assert_handler();

    bcma_cli_bshell_init(sc->cli);

    return 0;
}

int
bcma_sys_conf_cli_cleanup(bcma_sys_conf_t *sc)
{
    /* Clean up SDK-version of readline */
    bcma_editline_cleanup();

    /* Clean up environment */
    bcma_env_destroy(sc->eh);

    /* Free remaining CLI resources */
    bcma_cli_destroy(sc->dsh);
    bcma_cli_destroy(sc->cli);

    return 0;
}

int
bcma_sys_conf_cli_redir_bsl(bsl_meta_t *meta, const char *format, va_list args)
{
    return redir_out_hook(meta, format, args);
}
