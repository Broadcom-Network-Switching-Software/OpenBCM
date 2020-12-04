/*! \file bcma_io_dir_read.c
 *
 * Functions related to directory read.
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
#include <sys/types.h>
#include <dirent.h>

#include <bcma/io/bcma_io_dir.h>

bcma_io_dir_handle_t
bcma_io_dir_open(const char *path)
{
    if (path == NULL) {
        return NULL;
    }
    if (strlen(path) == 0) {
        return NULL;
    }

    return (bcma_io_dir_handle_t)opendir(path);
}

int
bcma_io_dir_close(bcma_io_dir_handle_t dh)
{
    if (dh == NULL) {
        return -1;
    }

    return closedir((DIR *)dh);
}

int
bcma_io_dir_read(bcma_io_dir_handle_t dh, int sz, char *name)
{
    DIR *dir = dh;
    struct dirent *dirent = NULL;
    int nlen;

    dirent = readdir(dir);
    if (!dirent) {
        return -1;
    }

    nlen = strlen(dirent->d_name);
    if (nlen >= sz) {
        return -1;
    }

    memcpy(name, dirent->d_name, nlen + 1);

    return 0;
}
