/*! \file bcmlrd_field_idx_count_get.c
 *
 * Get the number of elements for a given table and field.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>


/*******************************************************************************
 * Public functions
 */


uint32_t
bcmlrd_field_idx_count_get(int unit, bcmlrd_sid_t sid, bcmlrd_fid_t fid)
{
    uint32_t idx_count = 0;
    uint32_t depth;
    uint32_t width;
    bcmlrd_field_def_t fdef;
    int rv;

    rv = bcmlrd_table_field_def_get(unit, sid, fid, &fdef);
    if (rv != SHR_E_NONE) {
        return 0;
    }

    /*
     * The field indexes are defined by either of these parameters:
     *
     * a) Depth of the field
     *    The field is defined as an array in the LT definition file.
     *    The depth indicates the number of indexes for the field.
     *
     * b) Width of the field
     *    The width of the field defines the number of indexes
     *    needed to represent all the bits needed by the field
     *    (each field element holds up to 64 bits).
     *
     * Double dimensional arrays are not supported.  This means
     * that a wide field (width > 64 bits) cannot be defined
     * as an array in the table definition.
     */

    depth = fdef.depth;
    width = fdef.width;

    /* Check for invalid case:  array of wide fields */
    if ((depth > 0) && (width > BCM_FIELD_SIZE_BITS)) {
        /* Array of wide field (>64 bits) is not supported */
        return 0;
    }

    if (depth > 0) {
        idx_count = depth;
    } else {
        idx_count = bcmltd_field_idx_count_get(width);
    }

    return idx_count;
}

