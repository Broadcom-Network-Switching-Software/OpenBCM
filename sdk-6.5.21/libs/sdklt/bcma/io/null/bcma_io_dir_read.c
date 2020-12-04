/*! \file bcma_io_dir_read.c
 *
 * Functions related to directory read.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/io/bcma_io_dir.h>

bcma_io_dir_handle_t
bcma_io_dir_open(const char *path)
{
    return ((bcma_io_dir_handle_t)0);
}

int
bcma_io_dir_close(bcma_io_dir_handle_t dh)
{
    return -1;
}

int
bcma_io_dir_read(bcma_io_dir_handle_t dh, int sz, char *name)
{
    return -1;
}
