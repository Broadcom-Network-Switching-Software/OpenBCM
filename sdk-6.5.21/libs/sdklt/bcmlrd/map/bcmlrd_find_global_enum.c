/*! \file bcmlrd_find_global_enum.c
 *
 * Return the enum scalar value for the given table and field based
 * on the global ENUM_NAME_T scalar value.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_enum.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLRD_TABLE

/*******************************************************************************
 * Public functions
 */

int
bcmlrd_find_global_enum(int unit, uint32_t sid, uint32_t fid,
                        uint64_t lvalue, uint64_t *gvalue)
{
    const char *sym = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_enum_scalar_to_symbol(unit, sid, fid, lvalue, &sym));

    SHR_IF_ERR_EXIT
        (bcmlrd_enum_symbol_to_scalar_by_type(unit,
                                              "ENUM_NAME_T",
                                              sym,
                                              gvalue));

 exit:
    SHR_FUNC_EXIT();
}
