/*! \file bcmltm_db_util.c
 *
 * Logical Table Manager - Utility interfaces.
 *
 * This file contains basic utility interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <bsl/bsl.h>

#include "bcmltm_db_util.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_array_uint64_alloc(uint32_t count, uint64_t **values_ptr)
{
    uint64_t *values = NULL;
    unsigned int alloc_size;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    *values_ptr = NULL;

    if (count == 0) {
        SHR_EXIT();
    }

    alloc_size = sizeof(*values) * count;
    SHR_ALLOC(values, alloc_size, "bcmltmVal64Arr");
    SHR_NULL_CHECK(values, SHR_E_MEMORY);
    sal_memset(values, 0, alloc_size);

    *values_ptr = values;

 exit:
    SHR_FUNC_EXIT();
}
