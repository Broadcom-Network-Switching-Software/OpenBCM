/*! \file bcmlrd_field_min_get.c
 *
 * Get the min value for the given table and field.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmlrd/bcmlrd_internal.h>
#include <bcmlrd/bcmlrd_table.h>

int
bcmlrd_field_min_get(int unit,
                         const bcmlrd_sid_t sid,
                         const bcmlrd_fid_t fid,
                         const uint32_t  num_alloc,
                         uint64_t* field_def,
                         uint32_t* num_actual)
{
    return bcmlrd_field_data_get_by_type(unit, sid, fid,
                                         BCMLRD_FIELD_DATA_TYPE_MIN,
                                         num_alloc, field_def, num_actual);
}

