/*! \file bcmpmac_drv_util.c
 *
 * Utilities for the PMAC driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcmpmac/bcmpmac_drv.h>
#include <bcmpmac/bcmpmac_util_internal.h>


/*******************************************************************************
* Public functions
 */

int
bcmpmac_port_mode_get(const bcmpmac_port_mode_map_t *modes, int mode_num,
                      uint32_t *lmap, uint32_t *mval)
{
    int idx, rv;

    for (idx = 0; idx < mode_num; idx++) {
        rv = sal_memcmp(modes[idx].lane_map, lmap, sizeof(modes[idx].lane_map));
        if (rv == 0) {
            *mval = modes[idx].port_mode;
            return SHR_E_NONE;
        }
    }

    return SHR_E_PARAM;
}

bcmpmac_drv_op_f
bcmpmac_drv_op_func_get(const bcmpmac_drv_op_hdl_t *ops, int op_num,
                        const char *op_name)
{
    int idx;

    for (idx = 0; idx < op_num; idx++) {
        if (sal_strcmp(op_name, ops[idx].name) == 0) {
            return ops[idx].func;
        }
    }

    return NULL;
}
