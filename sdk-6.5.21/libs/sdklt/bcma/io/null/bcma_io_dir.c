/*! \file bcma_io_dir.c
 *
 * Functions related to directory.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/io/bcma_io_dir.h>

char *
bcma_io_dir_pwd(char *buf, int size)
{
    return ((char *)0);
}

int
bcma_io_dir_cd(const char *path)
{
    return -1;
}

int
bcma_io_dir_mkdir(const char *path)
{
    return -1;
}

int
bcma_io_dir_rmdir(const char *path)
{
    return -1;
}

int
bcma_io_dir_ls(const char *name, const char *flags)
{
    return -1;
}
