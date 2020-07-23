/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include "pem_physical_access.h" 
#include <string.h>
#include <assert.h>
#include <stdlib.h>





#ifndef DUNE_BCM 

int dpp_dsig_read(int unit, int core, char *block, char *from, char *to, char *name, uint32 *value, size);
{
  int array_entry;
   unit = 0;
   core = 0;

  
   for (array_entry = 0; array_entry < DSIG_MAX_SIZE_UINT32; ++array_entry)
     value[array_entry] = rand();
}

#endif 



