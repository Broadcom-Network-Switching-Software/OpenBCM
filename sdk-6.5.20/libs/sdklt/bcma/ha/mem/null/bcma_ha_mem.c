/*! \file bcma_ha_mem.c
 *
 * HA memory driver based on POSIX shm API..
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcma/ha/bcma_ha.h>

/*******************************************************************************
 * Public functions
 */

int
bcma_ha_mem_init(int unit, int size, bool warm, int inst)
{
    return SHR_E_UNAVAIL;
}

int
bcma_ha_mem_cleanup(int unit, bool warm_next)
{
    return SHR_E_UNAVAIL;
}

int
bcma_ha_mem_name_get(int unit, int buf_size, char *buf)
{
    return SHR_E_UNAVAIL;
}
