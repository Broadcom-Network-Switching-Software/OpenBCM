/*! \file bcmlrd_enum_symbol_name.c
 *
 * Return the LTL type name string for a given table and field if the
 * field is an enum.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_internal.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_enum.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLRD_CLIENT

/*******************************************************************************
 * Public functions
 */

int
bcmlrd_enum_symbol_name(int unit,
                        uint32_t sid,
                        uint32_t fid,
                        const char **name)
{
    const bcmltd_enum_type_t *enum_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_enum_symbol_type(unit, sid, fid, &enum_type));

    if (name == NULL) {
        /* Illegal pointer. */
        SHR_ERR_EXIT(SHR_E_PARAM);
    } else if (enum_type == NULL) {
        /* Should not happen unless there is an implementation error. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    } else {
        /* OK */
        *name = enum_type->name;
    }

exit:
    SHR_FUNC_EXIT();
}

