/*! \file bcmltd_field_idx_count_get.c
 *
 * Return the number of array elements for the given field width.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_table.h>

/*******************************************************************************
 * Public functions
 */

uint32_t
bcmltd_field_idx_count_get(uint32_t width)
{
    uint32_t idx_count;

    idx_count = (width + (BCM_FIELD_SIZE_BITS - 1)) / BCM_FIELD_SIZE_BITS;

    return idx_count;
}
