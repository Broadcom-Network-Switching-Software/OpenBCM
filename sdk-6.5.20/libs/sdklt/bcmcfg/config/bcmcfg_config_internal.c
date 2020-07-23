/*! \file bcmcfg_config_internal.c
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
#include <sal/sal_libc.h>
#include <bcmissu/issu_api.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmltd/chip/bcmltd_limits.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd_config.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_DEV

/*! Config Table Data. */
static bcmcfg_config_data_t *config_table_data[BCMDRD_CONFIG_MAX_UNITS][BCMLTD_TABLE_COUNT];
static bcmlrd_variant_t config_variant[BCMDRD_CONFIG_MAX_UNITS];
static bool config_lt_initialized[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */
void
bcmcfg_lt_init_done(int unit)
{
    config_lt_initialized[unit] = true;
    return;
}

bool
bcmcfg_lt_init_is_done(int unit)
{
    return config_lt_initialized[unit];
}

bool
bcmcfg_lt_init_is_issu(int unit)
{
    return bcmissu_is_active();
}

/*
 * Initialize config table information.
 */
int
bcmcfg_config_table_init(int unit, bool keep)
{
    uint32_t sid;
    uint32_t fid;
    uint32_t fidx;
    uint32_t idx_count;
    const bcmltd_table_rep_t *tbl;
    bool reader_src;
    bcmcfg_config_data_t *config = NULL;

    for (sid = 0; sid < BCMLTD_TABLE_COUNT; sid++) {
        if (config_table_data[unit][sid] == NULL) {
            continue;
        }

        tbl = bcmltd_table_get(sid);
        if (tbl == NULL) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META("Table not found.\n")));
            continue;
        }

        reader_src = false;
        for (fid = 0; fid < tbl->fields; fid++) {
            uint32_t first_idx;
            int tmp_rv;

            tmp_rv = bcmcfg_tbl_user_config_field_index_get(unit, sid, fid,
                                                       0, &first_idx);
            if (SHR_FAILURE(tmp_rv)) {
                continue;
            }

            tmp_rv = bcmltd_field_depth_get(sid, fid, &idx_count);
            if (SHR_FAILURE(tmp_rv) ||
                idx_count == 0) {

                continue;
            }

            for (fidx = 0; fidx < idx_count; fidx++) {
                uint32_t index = first_idx + fidx;

                config = &config_table_data[unit][sid][index];

                if (config->locus == NULL) {
                    continue;
                }
                if (keep &&
                    config->config_src == BCMCFG_CONFIG_SRC_TYPE_READER) {
                    reader_src = true;
                    continue;
                }

                sal_free((void *)config->locus);
                config->locus = NULL;
            }
        }

        /* Keep the reader src config data if 'keep' flag is true. */
        if (keep && reader_src) {
            continue;
        }

        sal_free(config_table_data[unit][sid]);
        config_table_data[unit][sid] = NULL;
    }

    sal_memset(&config_variant[unit], 0, sizeof(config_variant[unit]));
    config_lt_initialized[unit] = false;

    return SHR_E_NONE;
}

/*
 * Allocate memory for config data for the given unit.
 */
int
bcmcfg_config_table_field_alloc(int unit,
                                bcmltd_sid_t sid,
                                bcmcfg_config_data_t **data_ptr)
{
    bcmcfg_config_data_t *config;
    const bcmltd_table_rep_t *lt;
    size_t size = 0;
    size_t num = 0;
    uint32_t depth;
    uint32_t i;
    int rv;

    SHR_FUNC_ENTER(unit);

    lt = bcmltd_table_get(sid);
    /* Make sure table SID is in range. */
    if (lt == NULL) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("Table not found.\n")));
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Calculate allocation size.
     */
    for (i = 0; i < lt->fields; i++) {
        rv = bcmltd_field_depth_get(sid, i, &depth);
        if (SHR_FAILURE(rv)) {
            continue;
        }

        num += depth;
    }
    size = num * sizeof(bcmcfg_config_data_t);
    config = sal_alloc(size, "bcmcfgConfigTableFieldAlloc");
    if (config == NULL) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("Error allocating memory for config data.\n")));
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }

    sal_memset(config , 0, size);
    *data_ptr = config;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Set the variant mapping when the per-unit config_variant changes
 * from BCMLRD_VARIANT_T_NONE to the specified variant.
 */
static int
bcmcfg_config_variant_set(int unit, bcmlrd_variant_t variant)
{
    int rv = SHR_E_INTERNAL;

    if (config_variant[unit] == BCMLRD_VARIANT_T_NONE) {
        /* Variant not set yet. */
        rv = bcmlrd_variant_set(unit, variant);
        if (SHR_SUCCESS(rv)) {
            config_variant[unit] = variant;
        }
    } else if (config_variant[unit] != variant) {
        /* Variant changing illegally. */
        rv = SHR_E_PARAM;
    } else if (config_variant[unit] == variant) {
        /* Setting the current value again is OK. */
        rv = SHR_E_NONE;
    }

    return rv;
}


/*
 * Get field array index.
 */
int
bcmcfg_tbl_user_config_field_index_get(int unit,
                                       bcmltd_sid_t sid,
                                       bcmltd_fid_t fid,
                                       uint32_t fidx,
                                       uint32_t *field_index)
{
    int rv = SHR_E_NONE;
    uint32_t i;
    uint32_t index;
    const bcmltd_table_rep_t *lt = bcmltd_table_get(sid);

    /* Make sure table SID is in range. */
    if (lt == NULL) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META("Table not found.\n")));
        rv = SHR_E_UNAVAIL;
    }

    if (!(lt->flags & BCMLTD_TABLE_F_CONFIG)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Table is not config LT.\n")));
        rv = SHR_E_PARAM;
    }

    if (SHR_SUCCESS(rv)) {
        index = 0;
        for (i = 0; i < fid; i++) {
            uint32_t depth;

            rv = bcmltd_field_depth_get(sid, i, &depth);
            if (SHR_FAILURE(rv)) {
                continue;
            }
            index += depth;
        }

        index += fidx;

        *field_index = index;
    }

    return rv;
}

/*
 * Get user config data.
 */
int
bcmcfg_tbl_user_config_get(int unit,
                           bcmltd_sid_t sid,
                           bcmcfg_config_data_t **user_config)
{
    if (sid > BCMLTD_TABLE_COUNT) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "sid is out of range:%u\n"),
                  sid));
        return SHR_E_PARAM;
    }

    *user_config = config_table_data[unit][sid];
    return SHR_E_NONE;
}

/*
 * Config hook for special config variable handling.
 */
int
bcmcfg_tbl_user_config_hook(int unit,
                            bcmltd_sid_t sid,
                            bcmltd_fid_t fid,
                            uint64_t value)
{
    int rv = SHR_E_NONE;
    bcmlrd_variant_t variant;

    if (sid == DEVICE_CONFIGt && fid == DEVICE_CONFIGt_VARIANTf) {
        variant = bcmlrd_variant_from_device_variant_t(value);
        /* Ignore BCMLRD_VARIANT_T_NONE with no error. This is
         * typically a default config value if not otherwise set in
         * the mapping.
         */
        if (variant != BCMLRD_VARIANT_T_NONE) {
            rv = bcmcfg_config_variant_set(unit, variant);
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                      "Unavailable variant\n")));
            rv = SHR_E_PARAM;
        }
    }

    return rv;
}

/*
 * Set user config data.
 */
int
bcmcfg_tbl_user_config_set(int unit,
                           bcmltd_sid_t sid,
                           bcmcfg_config_data_t *user_config)
{
    if (sid > BCMLTD_TABLE_COUNT) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("sid is out of range:%u\n"),
                  sid));
        return SHR_E_PARAM;
    }

    if (user_config == NULL) {
        /* Invalid config. */
        return SHR_E_PARAM;
    }

    config_table_data[unit][sid] = user_config;
    return SHR_E_NONE;
}

