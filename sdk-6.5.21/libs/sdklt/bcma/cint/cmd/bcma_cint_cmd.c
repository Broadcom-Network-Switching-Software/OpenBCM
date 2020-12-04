/*
 * $Id: bcma_cint_cmd.c
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * bcma_cint_cmd.c
 */
#include <sal_config.h>
#include <bsl/bsl.h>

#include "bcma_cint_sdk_atomics.h"
#include "../core/cint_interpreter.h"
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include "../core/cint_porting.h"
#include <bcma/io/bcma_io_file.h>
#include <bcma/cli/bcma_cli_ctrlc.h>
#include <bcma/cli/bcma_cli_bshell.h>

#include <bcma/cint/bcma_cint.h>
#include <bcma/cint/bcma_cintcmd_cint.h>

/* bcma_cint_bcmlt_data.c */
extern cint_data_t bcma_cint_data;

/* bcm_cint_stdclib_data.c */
extern cint_data_t stdclib_cint_data;

/* Custom functions */

static int
bshell(int unit, char *cmd)
{
    return bcma_cli_bshell(unit, cmd);
}
CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                  bshell,
                  int,int,unit,0,0,
                  char*,char,cmd,1,0);

static cint_function_t __cint_cmd_functions[] =
    {
        CINT_FWRAPPER_ENTRY(bshell),
        CINT_ENTRY_LAST
    };

static cint_constants_t bcma_cint_cmd_constants[] =
{
    { "TRUE", 1 },
    { "FALSE", 0 },
    { "true", 1 },
    { "false", 0 },
    { NULL }
};

cint_data_t cmd_cint_data =
{
    NULL,
    __cint_cmd_functions,
    NULL,
    NULL,
    NULL,
    bcma_cint_cmd_constants,
    NULL
};


static int
bcma_cint_sdk_data_init(void)
{
    cint_interpreter_add_data(&bcma_cint_sdk_data, NULL);
    cint_interpreter_add_data(&stdclib_cint_data, NULL);
    cint_interpreter_add_data(&bcma_cint_data, NULL);
    cint_interpreter_add_data(&cmd_cint_data, NULL);
    return 0;
}

static int
bcma_cint_event_handler(void* cookie, cint_interpreter_event_t event)
{
    switch (event)
    {
        case cintEventReset:
            /*
             * Interpreter has reset. Re-register our data
             */
            bcma_cint_sdk_data_init();
            break;
        default:
            /* Nothing */
            break;
    }
    return 0;
}

#if CINT_CONFIG_INCLUDE_XINCLUDE == 1
static void
bcma_cint_init_path(void)
{
    char *path = NULL;
    path = getenv("CINT_INCLUDE_PATH");

    if (path) {
        cint_interpreter_include_set(path);
    }
}
#endif

static int
cmd_cint_initialize(void)
{
    static int init = 0;
    if (init == 0) {
        cint_interpreter_init();
        cint_interpreter_event_register(bcma_cint_event_handler, NULL);
#if CINT_CONFIG_INCLUDE_XINCLUDE == 1
        bcma_cint_init_path();
#endif
        bcma_cint_sdk_data_init();
#ifdef INCLUDE_LIB_C_UNIT
        c_unit_initialize(c_unit_context_set);
#endif
        init = 1;
    }
    return 0;
}

/* Data for local Ctrl-C trap */
typedef struct do_cint_parse_s {
    void *handle;
    const char *prompt;
    int argc;
    char **argv;
} do_cint_parse_t;

static int
do_cint_parse_ctrlc(void *data)
{
    do_cint_parse_t *cparse = (do_cint_parse_t *)data;

    cint_interpreter_parse(cparse->handle, cparse->prompt,
                           cparse->argc, cparse->argv);

    return 0;
}

static int
do_cint_parse(bcma_cli_t *cli, void *handle, const char *prompt,
              int argc, char **argv)
{
    int rv;
    do_cint_parse_t cint_parse, *cparse = &cint_parse;

    cparse->handle = handle;
    cparse->prompt = prompt;
    cparse->argc = argc;
    cparse->argv = argv;
    /*
     * Use local Ctrl-C trap to ensure that the cint shell still runs
     * when Ctrl-C is pressed.
     */
    do {
        rv = bcma_cli_ctrlc_exec(cli, do_cint_parse_ctrlc, cparse, 1);
    } while (rv == BCMA_CLI_CMD_INTR);

    return 0;
}

void
bcma_cint_fatal_error(char *msg)
{
    CINT_PRINTF("%s",msg);
}

int
bcma_cintcmd_cint(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    static bool cint_active;
    char *str;
    int cmp;
    int argc = 0;
    char *argv[16];

    if (cint_active) {
        CINT_PRINTF("Error: cint cannot be called recursively.\n");
        return 0;
    }
    cint_active = true;

    cmd_cint_initialize();

    str = BCMA_CLI_ARG_CUR(args);
    if (str) {
        cmp = sal_strcmp(str, "allow_file_info");
        if (!cmp) {
            argv[argc] = BCMA_CLI_ARG_GET(args);
            argc++;
        }
    }

    if (BCMA_CLI_ARG_CUR(args)) {
        /* Load file */
#if CINT_CONFIG_INCLUDE_STDLIB == 1
        FILE *fp;
#else
        bcma_io_file_handle_t fp;
#endif
        str = BCMA_CLI_ARG_GET(args);

        if ((fp = CINT_FOPEN(str, "r")) != NULL) {

            sal_memset(&argv[argc], 0,
                    (sizeof(argv)-(argc*sizeof(char *))));
            while ((str = BCMA_CLI_ARG_GET(args))) {
                if (argc < 16) {
                    argv[argc] = str;
                }
                argc++;
            }

            if ( argc <= 16 ) {
                do_cint_parse(cli, fp, NULL, argc, argv);
            } else {
                cli_out("Error: cannot pass more than 16 "
                        "arguments, excluding the filename.\n");
            }
            CINT_FCLOSE(fp);
        } else {
            cli_out("Error: could not open file '%s'\n", str);
        }
    } else {
        cli_out("Entering C Interpreter. Type 'exit;' to quit.\n\n");
        do_cint_parse(cli, NULL, "cint> ", argc, argv);
    }

    cint_active = false;

    return 0;
}
