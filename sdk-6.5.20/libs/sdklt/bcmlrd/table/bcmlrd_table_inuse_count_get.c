/*! \file bcmlrd_table_count_get.c
 *
 * Get the current in-use entry count of a LT.
 *
 * Value provided via registered callback to LTM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd_config.h>
#include <bcmlrd/bcmlrd_table.h>

/*******************************************************************************
 * Private variables
 */
static bcmlrd_table_inuse_count_get_cb lt_inuse_cb[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public Functions
 */
void
bcmlrd_table_inuse_count_get_register(int unit,
                          bcmlrd_table_inuse_count_get_cb lt_inuse_get_cb)
{
    lt_inuse_cb[unit] = lt_inuse_get_cb;
}

int
bcmlrd_table_inuse_count_get(int unit,
                             uint32_t trans_id,
                             bcmltd_sid_t ltid,
                             uint32_t *inuse_count)
{
    if (lt_inuse_cb[unit] == NULL) {
        return SHR_E_UNAVAIL;
    }

    return lt_inuse_cb[unit](unit, trans_id, ltid, inuse_count);
}
