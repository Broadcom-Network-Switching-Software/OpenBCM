/*! \file bcmcfg_table_get_internal.c
 *
 * Device configuration handle helper.
 * This file contains functions to get fields in the config tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/chip/bcmlrd_limits.h>
#include <bcmdrd/bcmdrd_dev.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_DEV

/*******************************************************************************
 * Public functions
 */

/*
 * Get config table values.
 */
int
bcmcfg_table_get_internal(int unit,
                          bcmltd_sid_t sid,
                          bcmltd_sid_t fid,
                          size_t start_idx,
                          size_t count,
                          uint64_t *field,
                          size_t *actual)
{
    bcmcfg_config_data_t *user_config;
    size_t i;
    size_t out;
    size_t actual_size = 0;
    const bcmltd_table_rep_t *lt;
    uint32_t offset;
    uint32_t last_idx;
    uint32_t depth;

    SHR_FUNC_ENTER(unit);

    /* Make sure unit is in range. */
    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (!bcmcfg_lt_init_is_done(unit)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Logical Table configuration is not ready yet.\n")));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (sid >= BCMLRD_TABLE_COUNT) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    lt = bcmltd_table_get(sid);
    /* Make sure table SID is in range. */
    if (lt == NULL) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("Table not found.\n")));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (!(lt->flags & BCMLTD_TABLE_F_CONFIG)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Table is not config LT.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (field == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("The field buffer is NULL.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (fid >= lt->fields) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Field is out of range.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmcfg_tbl_user_config_get(unit, sid, &user_config));

    /* Get first index(offset) for this field. */
    SHR_IF_ERR_EXIT(
        bcmcfg_tbl_user_config_field_index_get(unit,
                                               sid,
                                               fid,
                                               0,
                                               &offset));

    SHR_IF_ERR_EXIT(
        bcmltd_field_depth_get(sid, fid, &depth));
    last_idx = offset + depth;

    for (out = 0, i = start_idx + offset; i < last_idx; i++, out++) {
        if (out < count) {
            /* If no data, default is zero. */
            *field++ = user_config ? user_config[i].value : 0;
            actual_size++;
        } else {
            break;
        }
    }

    /* Return how many elements were written if requested. */
    if (actual) {
        *actual = actual_size;
    }

exit:
    SHR_FUNC_EXIT();
}
