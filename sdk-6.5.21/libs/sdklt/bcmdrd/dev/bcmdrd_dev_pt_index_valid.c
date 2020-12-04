/*! \file bcmdrd_dev_index_valid.c
 *
 * DRD functions for setting and dispatching device-specific physical table
 * index valid functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_dev.h>

static bcmdrd_dev_pt_index_valid_f index_valid_func[BCMDRD_CONFIG_MAX_UNITS];

int
bcmdrd_dev_pt_index_valid_func_set(int unit, bcmdrd_dev_pt_index_valid_f func)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    index_valid_func[unit] = func;
    return SHR_E_NONE;
}

int
bcmdrd_dev_pt_index_valid(int unit, bcmdrd_sid_t sid, int tbl_inst, int tbl_idx,
                          bool *valid)
{
    bcmdrd_dev_pt_index_valid_f func;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    func = index_valid_func[unit];
    if (func == NULL) {
        return SHR_E_UNAVAIL;
    }
    return func(unit, sid, tbl_inst, tbl_idx, valid);
}
