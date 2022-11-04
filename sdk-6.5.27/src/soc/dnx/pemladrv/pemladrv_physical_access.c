
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#include "pemladrv_physical_access.h"
#ifdef LINK_TO_SVERILOG

#if 0
int
pemladrv_physical_read(
    int unit,
    unsigned int block_identifier,
    unsigned int is_reg,
    unsigned int mem_address,
    unsigned int mem_width_in_bits,
    unsigned int reserve,
    void *entry_data)
{
    return 0;
}

int
pemladrv_physical_write(
    int unit,
    unsigned int block_identifier,
    unsigned int is_reg,
    unsigned int mem_address,
    unsigned int mem_width_in_bits,
    unsigned int reserve,
    void *entry_data)
{
    return 0;
}
#endif
#endif
