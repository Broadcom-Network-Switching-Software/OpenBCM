/*! \file bcma_sys_conf_rcload.c
 *
 * Implementation of the CLI auto-run script while the system starts up.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>

#include <bcma/io/bcma_io_file.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_clicmd_rcload.h>

#include <bcma/sys/bcma_sys_conf.h>

#define MAX_SCRIPT_FILE_LEN     255

/* Buffer to store the specified CLI auto-run script */
static char autorun_script[MAX_SCRIPT_FILE_LEN + 1] = {0};

/* Default auto-run script */
static char *def_autorun_script = BCMA_SYS_CONF_RCLOAD_SCRIPT;

int
bcma_sys_conf_rcload_run(bcma_sys_conf_t *sc)
{
    int rv = BCMA_CLI_CMD_OK;
    char *filename = NULL;

    if (sc == NULL || sc->cli == NULL) {
        return -1;
    }

    if (autorun_script[0] == '\0') {
        /* Test if default auto-run script exists */
        bcma_io_file_handle_t fh;
        if ((fh = bcma_io_file_open(def_autorun_script, "r")) != NULL) {
            bcma_io_file_close(fh);
            filename = def_autorun_script;
        }
    } else {
        filename = autorun_script;
    }

    if (filename) {
        LOG_INFO(BSL_LS_APPL_SHELL,
                 (BSL_META("Auto-running %s\n"), filename));
        rv = bcma_clicmd_rcload_file(sc->cli, NULL, filename, false);
        if (rv == BCMA_CLI_CMD_NOT_FOUND) {
            LOG_WARN(BSL_LS_APPL_SHELL,
                     (BSL_META("File not found: %s\n"), filename));
        }
    }

    return rv;
}

int
bcma_sys_conf_rcload_set(const char *rc_file)
{
    size_t len;

    if (rc_file) {
        len = sal_strlen(rc_file);
        if (len == 0 || len > MAX_SCRIPT_FILE_LEN) {
            return -1;
        }
        sal_memcpy(autorun_script, rc_file, len + 1);
    }

    return 0;
}
