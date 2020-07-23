/*! \file bcmltx_std_table_validator.c
 *
 * Built-in Field Validation functions -
 *
 * 1) Input field list validation (deprecated, now in LTM core)
 * 2) Port bitmap array validation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bsl/bsl.h>

#include <bcmdrd/bcmdrd_field.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmltx/bcmltx_std_table_validator.h>
#include <bcmltx/bcmltx_util.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

int
bcmltx_std_table_validator(int unit,
                           bcmlt_opcode_t opcode,
                           const bcmltd_fields_t *in,
                           const bcmltd_field_val_arg_t *arg)
{
#ifdef STD_TABLE_VALIDATOR_ENABLE
    uint32_t aix, fid;
    uint32_t found_count = 0;
    bcmltx_field_list_t *field_ptr;
    const bcmlrd_table_rep_t *tbl = NULL;
    const bcmlrd_map_t *map = NULL;
    bcmlrd_sid_t sid;
    uint32_t idx, idx_count;

    SHR_FUNC_ENTER(unit);

    if ((arg->field == NULL) ||
        (arg->fields == 0) ||
        (arg->comp_data == NULL)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Invalid field validation information\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    sid = arg->comp_data->sid;
    tbl = bcmlrd_table_get(sid);
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));
    if ((tbl == NULL) || (map == NULL)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not get LRD table,map representation: "
                              "sid=%d\n"),
                   sid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Wide Field */
    for (idx = 0; idx < arg->fields; idx++) {
        fid = arg->field[idx];
        found_count = 0;

        idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);
        for (aix = 0; aix < idx_count; aix++) {
            field_ptr = bcmltx_lta_find_field(in, fid, aix);
            if (field_ptr == NULL) {
                /* Missing array element in list */
                if (fid < tbl->fields) {
                    /* Field validation failure */
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                          "LTX %s: Missing field value"
                                          " for field %s[%d]\n"),
                                 tbl->name,
                                 tbl->field[fid].name,
                                 aix));
                }
            } else {
                found_count++;
            }
        }
        if (found_count != 0 &&
            found_count != idx_count) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    /* READ ONLY */
    if ((opcode == BCMLT_OPCODE_INSERT) ||
        (opcode == BCMLT_OPCODE_UPDATE)) {

        uint32_t flags;
        uint32_t error = 0;

        for (idx = 0; idx < arg->fields; idx++) {
            fid = arg->field[idx];

            field_ptr = bcmltx_lta_find_field(in, fid, 0);
            if (field_ptr != NULL) {

                flags = map->field_data->field[fid].flags;
                if (flags & BCMLRD_FIELD_F_READ_ONLY) {
                    error++;
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                          "LTX %s: Read only field %s\n"),
                                 tbl->name,
                                 tbl->field[fid].name));
                }
            }
        }

        if (error > 0) {
            SHR_ERR_EXIT(SHR_E_ACCESS);
        }
    }

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_NONE;
#endif
}
