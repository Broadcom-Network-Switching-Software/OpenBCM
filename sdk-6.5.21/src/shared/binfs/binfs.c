/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Built-in binrary files.
 *
 * binfs includes
 * 1. A script to generate binfs data base. ($SDK/tools/genbinfs.py)
 * 2. Built-in file access APIs.
 */

#include <shared/error.h>
#include <shared/bsl.h>
#include <sal/types.h>
#include <sal/core/libc.h>
#include <shared/binfs/binfs.h>

int
binfs_file_data_get(char *name, const uint8 **data, int *len) {

   const binfs_file_t *binfs;
   int i;

   LOG_VERBOSE(BSL_LS_SOC_COMMON,
              (BSL_META("Get binfs %s\n"), name));

   i = 0;
   binfs = binfs_file_list[i];

   while (binfs) {

       if (sal_strncmp(name, binfs->name, sal_strlen(binfs->name)) == 0)
       {
            *data = binfs->data;
            *len = binfs->len;
            return _SHR_E_NONE;
       }
       i ++;
       binfs = binfs_file_list[i];
   }

   return _SHR_E_NOT_FOUND;
}
