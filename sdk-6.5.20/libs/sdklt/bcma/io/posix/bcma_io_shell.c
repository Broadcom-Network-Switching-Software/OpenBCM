/*! \file bcma_io_shell.c
 *
 * POSIX shell command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <bsl/bsl.h>

#include <bcma/io/bcma_io_file.h>
#include <bcma/io/bcma_io_shell.h>

#define MAX_BUF_SIZE 256

static int
io_shell_pipe(const char *cmd)
{
    FILE *s;
    size_t len;
    char *cmd_str;
    char buf[MAX_BUF_SIZE];
    int rv;

    if (!cmd) {
        return -1;
    }
    /* Add room for stderr redirection */
    len = strlen(cmd) + 16;
    cmd_str = malloc(len);
    if (!cmd_str) {
        return -1;
    }
    /* Build the command to run */
    snprintf(cmd_str, len, "%s 2>&1", cmd);

    s = popen(cmd_str, "r");
    if (s == NULL) {
        free(cmd_str);
        return -1;
    }

    while ((rv = bcma_io_file_read(s, buf, 1, MAX_BUF_SIZE - 1)) > 0) {
        buf[rv] = '\0';
        cli_out("%s", buf);
    }

    pclose(s);
    free(cmd_str);

    return 0;
}

int
bcma_io_shell(const char *cmd, const char *ifile, const char *ofile)
{
    size_t len;
    char *cmd_str;
    char *iredir;
    char *oredir;
    int rv = 0;

    if (!cmd) {
        cmd = getenv("SHELL");
        if (!cmd) {
            cmd = "/bin/sh";
        }
        return system(cmd);
    }

    /* Add room for redirection operators */
    len = strlen(cmd) + 16;

    if (ifile) {
        len += strlen(ifile);
        iredir = " <";
    } else {
        ifile = "";
        iredir = "";
    }

    if (ofile) {
        len += strlen(ofile);
        oredir = " >";
    } else {
        ofile = "";
        oredir = "";
    }

    cmd_str = malloc(len);
    if (!cmd_str) {
        return -1;
    }

    /* Build command line and execute it */
    snprintf(cmd_str, len, "%s%s%s%s%s", cmd, iredir, ifile, oredir, ofile);

    rv = io_shell_pipe(cmd_str);

    free(cmd_str);

    return rv;
}
