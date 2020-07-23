/*! \file bcmlrd_unit_match_id_db_ptr_get.c
 *
 * Return the pointer to the function that retrieves the match id data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_internal.h>

bcmlrd_match_id_db_get_t
bcmlrd_unit_match_id_db_get_ptr[BCMDRD_CONFIG_MAX_UNITS];

/* get a mapping configuration for a unit */
bcmlrd_match_id_db_get_t
bcmlrd_unit_match_id_db_ptr_get(int unit)
{
    bcmlrd_match_id_db_get_t ptr = NULL;

    if (unit >= 0 && unit < COUNTOF(bcmlrd_unit_match_id_db_get_ptr)) {
        ptr = bcmlrd_unit_match_id_db_get_ptr[unit];
    }

    return ptr;
}
