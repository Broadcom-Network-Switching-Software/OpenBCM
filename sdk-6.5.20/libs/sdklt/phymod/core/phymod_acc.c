/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod_acc.h>

int
phymod_acc_check(const phymod_access_t *pa)
{
    if (pa == 0 ||
        PHYMOD_ACC_BUS(pa) == 0 ||
        PHYMOD_ACC_BUS(pa)->read == 0 ||
        PHYMOD_ACC_BUS(pa)->write == 0 ||
        PHYMOD_ACC_BUS(pa)->mem_read == 0 ||
        PHYMOD_ACC_BUS(pa)->mem_write == 0) {
        return -1;
    }
    return 0;
}

int
phymod_bus_read(const phymod_access_t *pa, uint32_t reg, uint32_t *data)
{
    /* Read raw PHY data */
    return PHYMOD_ACC_BUS(pa)->read(PHYMOD_ACC_USER_ACC(pa),
                                    PHYMOD_ACC_BUS_ADDR(pa),
                                    reg, data);
}

int
phymod_mem_read(const phymod_access_t *pa, phymod_mem_type_t mem_type, uint32_t mem_index, uint32_t* val)
{
    /* Read raw PHY data */
    return PHYMOD_ACC_BUS(pa)->mem_read(PHYMOD_ACC_USER_ACC(pa),
                                    mem_type,
                                    mem_index,
                                    val);
}


int
phymod_bus_write(const phymod_access_t *pa, uint32_t reg, uint32_t data)
{
    /* Write raw PHY data */
    return PHYMOD_ACC_BUS(pa)->write(PHYMOD_ACC_USER_ACC(pa),
                                     PHYMOD_ACC_BUS_ADDR(pa),
                                     reg, data);
}

int
phymod_mem_write(const phymod_access_t *pa, phymod_mem_type_t mem_type, uint32_t mem_index, uint32_t* data)
{
    /* Read raw PHY data */
    return PHYMOD_ACC_BUS(pa)->mem_write(PHYMOD_ACC_USER_ACC(pa),
                                    mem_type,
                                    mem_index,
                                    data);
}


int
phymod_is_write_disabled(const phymod_access_t *pa, uint32_t *data)
{
    if((PHYMOD_ACC_BUS(pa)->is_write_disabled) == NULL) { *data = 0; } else {
        return PHYMOD_ACC_BUS(pa)->is_write_disabled(PHYMOD_ACC_USER_ACC(pa),
                                     data);
    }
    return 0;
}
