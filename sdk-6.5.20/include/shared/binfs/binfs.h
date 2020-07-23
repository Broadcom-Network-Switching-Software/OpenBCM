/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom Built-in binary file system.
 */

#ifndef __BINFS_H___
#define __BINFS_H___

typedef struct {
    const char *name;
    const uint8 *data;
    int len;
} binfs_file_t;

extern const binfs_file_t *binfs_file_list[];
extern int
binfs_file_data_get(char *filename, const uint8 **data, int *len);


#endif /* __BINFS_H___ */
