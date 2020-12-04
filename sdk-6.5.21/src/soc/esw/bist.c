/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/error.h>
#include <soc/drv.h>

/* No longer used - preserved for historical reasons */
int
soc_bist(int unit, soc_mem_t *mems, int num_mems, int timeout_msec)
{
    return SOC_E_NONE;
}
