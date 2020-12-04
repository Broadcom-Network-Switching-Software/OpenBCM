/*! \file bcmcfg_table_set_internal.c
 *
 * Device configuration handle helper.
 * This file contains functions to set fields in the config tables.
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
 * Set value for the given field in config table.
 */
int
bcmcfg_table_set_internal(int unit,
                          bcmltd_sid_t sid,
                          bcmltd_sid_t fid,
                          size_t start_idx,
                          size_t count,
                          uint64_t *field,
                          size_t *actual)
{
    const bcmltd_table_rep_t *lt;
    bcmcfg_config_data_t *user_config;
    size_t i;
    size_t out;
    size_t actual_size = 0;
    uint32_t offset;
    uint32_t last_idx;
    uint32_t depth;

    SHR_FUNC_ENTER(unit);
    /* Make sure unit is in range. */
    if (!bcmdrd_dev_exists(unit)) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    if (bcmcfg_lt_init_is_done(unit)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                   "Logical table configuration is done already.\n")));
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    lt = bcmltd_table_get(sid);
    /* Make sure table SID is in range. */
    if (lt == NULL) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("Table not found.\n")));
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    if (!(lt->flags & BCMLTD_TABLE_F_CONFIG)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Table is not config LT.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Sanity check for fid. */
    if (fid >= lt->fields) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("Field not found.\n")));
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcfg_tbl_user_config_get(unit, sid, &user_config));

    /* Allocate memory for config data if not allocated. */
    if (user_config == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcfg_config_table_field_alloc(unit, sid, &user_config));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcfg_tbl_user_config_set(unit, sid, user_config));
    }

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmcfg_tbl_user_config_hook(unit, sid, fid, field[0]));

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

    /* Set the value for given fid. */
    for (out = 0, i = start_idx + offset; i < last_idx; out++, i++) {
        if (out < count) {
            user_config[i].index = start_idx + out;
            user_config[i].value = field[out];
            user_config[i].config_src = BCMCFG_CONFIG_SRC_TYPE_API;
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
