/*! \file bcmlrd_find_local_enum.c
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
#include <bcmlrd/bcmlrd_conf.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLRD_TABLE

/*******************************************************************************
 * Public functions
 */

int
bcmlrd_find_local_enum(int unit, uint32_t sid, uint32_t fid,
                       uint64_t gvalue, uint64_t *lvalue)
{
    const char *sym = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get enum symbol of global enum value by ENUM_NAME_T enum. */
    SHR_IF_ERR_EXIT
        (bcmlrd_enum_scalar_to_symbol_by_type(unit,
                                              "ENUM_NAME_T",
                                              gvalue,
                                              &sym));
    SHR_IF_ERR_EXIT
        (bcmlrd_resolve_enum(unit, sid, fid, sym, lvalue));

 exit:
    SHR_FUNC_EXIT();
}
