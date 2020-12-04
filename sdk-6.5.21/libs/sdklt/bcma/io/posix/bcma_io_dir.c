/*! \file bcma_io_dir.c
 *
 * Functions related to directory.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <bcma/io/bcma_io_shell.h>
#include <bcma/io/bcma_io_dir.h>

char *
bcma_io_dir_pwd(char *buf, int size)
{
    if (buf == NULL || size <= 0) {
        return NULL;
    }

    /* Leave one character space to include the trailing slash */
    if (getcwd(buf, size - 1) == NULL) {
        return NULL;
    }

    if (buf[strlen(buf) - 1] != '/') {
        strcat(buf, "/");
    }

    return buf;
}

int
bcma_io_dir_cd(const char *path)
{
    int rv;
    char *s = NULL;

    if (path == NULL) {
        /* Change to home directory if path is not specified. */
        path = getenv("HOME");
        if (path == NULL) {
            path = "/";
        } else {
            s = strdup(path);
            path = (s == NULL) ? "/" : s;
        }
    }

    rv = chdir(path);

    if (s) {
        free(s);
    }

    return rv;
}

int
bcma_io_dir_mkdir(const char *path)
{
    if (path == NULL) {
        return -1;
    }

    return mkdir(path, 0777);
}

int
bcma_io_dir_rmdir(const char *path)
{
    if (path == NULL) {
        return -1;
    }

    return rmdir(path);
}

int
bcma_io_dir_ls(const char *name, const char *flags)
{
    int rv, sz;
    char *cmd;
    char *c = "";

    if (name == NULL) {
        return -1;
    }

    sz = strlen(name);
    if (flags) {
        sz += strlen(flags);
    }
    sz += 16;

    cmd = malloc(sz);
    if (cmd == NULL) {
        return -1;
    }

    if (strchr(name, ' ')) {
        /*
         * Add extra quotes to the input name passed to the shell command for
         * supporting spaces in the input name.
         * By default double-quotes are added. If the input name is already
         * started in double-quote, single-quotes will be added instead, e.g.:
         * "split name" => '"split name"'
         * 'split name' => "'split name'"
         * split\ name => "split\ name"
         */
        if (name[0] == '"') {
            c = "'";
        } else {
            c = "\"";
        }
    }
    rv = snprintf(cmd, sz, "ls %s %s%s%s", flags ? flags : "", c, name, c);
    if (rv >= sz) {
        free(cmd);
        return -1;
    }

    rv = bcma_io_shell(cmd, NULL, NULL);

    free(cmd);

    return (rv == -1) ? -1 : 0;
}
