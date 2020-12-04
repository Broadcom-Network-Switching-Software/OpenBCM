/*! \file bcmlrd_enum_symbol_type.c
 *
 * Return the LTL symbol (enum) type data for a given table and field
 * is the field is an enum.
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
#include <bcmlrd/bcmlrd_map.h>
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
bcmlrd_enum_symbol_type(int unit,
                        uint32_t sid,
                        uint32_t fid,
                        const bcmltd_enum_type_t **enum_type_p)
{
    const bcmlrd_field_data_t *field;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_get(unit, sid, fid, &field));

    if (!field->edata) {
        /* Not an LTL enum */
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    } else if (enum_type_p == NULL) {
        /* Illegal pointer. */
        SHR_ERR_EXIT(SHR_E_PARAM);
    } else {
        /* OK */
        *enum_type_p = field->edata;
    }

exit:
    SHR_FUNC_EXIT();
}

