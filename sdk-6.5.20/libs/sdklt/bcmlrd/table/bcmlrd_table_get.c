/*! \file bcmlrd_table_get.c
 *
 * Get table rep for given SID.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_internal.h>

const bcmlrd_table_rep_t *
bcmlrd_table_get(bcmlrd_sid_t sid)
{
    return bcmltd_table_get(sid);
}
