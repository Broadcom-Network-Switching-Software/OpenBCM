/*! \file bcmlrd_enum_scalar_to_symbol.c
 *
 * Implementation of bcmlrd_enum_scalar_to_symbol().
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
bcmlrd_enum_scalar_to_symbol(int unit,
                             uint32_t sid,
                             uint32_t fid,
                             uint64_t value,
                             const char **symbol)
{
    const bcmltd_enum_type_t *enum_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_enum_symbol_type(unit, sid, fid, &enum_type));
    SHR_IF_ERR_EXIT
        (bcmlrd_enum_scalar_lookup(enum_type, value, symbol));

 exit:
    SHR_FUNC_EXIT();
}

