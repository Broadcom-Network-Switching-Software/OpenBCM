/*! \file bcmlrd_field_data_get_by_type.c
 *
 * Get field data by field data type.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_internal.h>
#include <bcmlrd/bcmlrd_table.h>

int
bcmlrd_field_data_get_by_type(int unit,
                              const bcmlrd_sid_t sid,
                              const bcmlrd_fid_t fid,
                              const bcmlrd_field_data_type_t type,
                              const uint32_t  num_alloc,
                              uint64_t *field_data,
                              uint32_t *num_actual)
{
    int rv;
    bcmlrd_field_def_t fdef;

    rv = bcmlrd_table_field_def_get(unit, sid, fid, &fdef);
    if (rv == SHR_E_NONE) {
        rv = bcmlrd_field_data_get_by_def_type(&fdef, type,
                                               num_alloc,
                                               field_data,
                                               num_actual);
    }

    return rv;
}
