/*! \file bcmcfg_lt_config_range_check.c
 *
 * Config value range check for the LT config fields.
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
#include <bcmlrd/bcmlrd_map.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_INIT

/*******************************************************************************
 * Public functions
 */
int
bcmcfg_lt_config_range_check(int unit, bool warm)
{
    uint32_t sid;
    uint32_t fid;
    uint32_t fidx;
    uint32_t idx_count;
    uint32_t minimum_count;
    uint32_t maximum_count;
    uint64_t *minimum_value = NULL;
    uint64_t *maximum_value = NULL;
    bcmcfg_config_data_t *user_config = NULL;
    const bcmlrd_map_t *map = NULL;
    size_t nfields;
    int error = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    for (sid = 0; sid < BCMLTD_TABLE_COUNT; sid++) {
        const bcmlrd_table_rep_t *tbl;

        tbl = bcmltd_table_get(sid);
        if (!tbl || !(tbl->flags & BCMLTD_TABLE_F_CONFIG)) {
            continue;
        }

        rv = bcmlrd_map_get(unit, sid, &map);
        if (SHR_FAILURE(rv)) {
            continue;
        }

        SHR_IF_ERR_EXIT(
            bcmcfg_tbl_user_config_get(unit, sid, &user_config));
        if (user_config == NULL) {
            continue;
        }

        SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, sid, &nfields));

        for (fid = 0; fid < nfields; fid++) {
            uint32_t first_idx;
            int tmp_rv;

            tmp_rv = bcmcfg_tbl_user_config_field_index_get(unit, sid, fid,
                                                       0, &first_idx);
            if (SHR_FAILURE(tmp_rv)) {
                continue;
            }

            idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);
            if (idx_count == 0) {
               continue;
            }

            SHR_ALLOC(minimum_value, idx_count * sizeof(uint64_t), "bcmcfgConfigRangeCheckMin");
            SHR_NULL_CHECK(minimum_value, SHR_E_MEMORY);
            SHR_ALLOC(maximum_value, idx_count * sizeof(uint64_t), "bcmcfgConfigRangeCheckMax");
            SHR_NULL_CHECK(maximum_value, SHR_E_MEMORY);

            SHR_IF_ERR_EXIT
                (bcmlrd_field_min_get(unit, sid, fid,
                                      idx_count,
                                      minimum_value,
                                      &minimum_count));

            SHR_IF_ERR_EXIT
                (bcmlrd_field_max_get(unit, sid, fid,
                                      idx_count,
                                      maximum_value,
                                      &maximum_count));

            for (fidx = 0; fidx < idx_count; fidx++) {
                uint32_t index = first_idx + fidx;

                if (user_config[index].value < minimum_value[fidx] ||
                    user_config[index].value > maximum_value[fidx]) {
                    bcmltd_field_def_t field;

                    SHR_IF_ERR_EXIT(
                        bcmlrd_field_def_get(unit, fid, tbl, map, &field));

                    if (user_config[index].locus) {
                        LOG_ERROR
                            (BSL_LOG_MODULE,
                             (BSL_META("%s:%d:%d: %s.%s value(%" PRIu64 ") "
                                       "out of range[%" PRIu64 ":%" PRIu64 "]\n"),
                              user_config[index].locus,
                              user_config[index].line,
                              user_config[index].column,
                              tbl->name, field.name, user_config[index].value,
                              minimum_value[fidx], maximum_value[fidx]));
                        error++;
                    } else {
                        /* This field is not specified in yaml file.
                         * Default value is out of range.
                         */
                        LOG_WARN
                            (BSL_LOG_MODULE,
                             (BSL_META("%s.%s value(%" PRIu64 ") "
                                       "out of range[%" PRIu64 ":%" PRIu64 "]\n"),
                              tbl->name, field.name, user_config[index].value,
                              minimum_value[fidx], maximum_value[fidx]));
                    }
                }
            }
            SHR_FREE(minimum_value);
            SHR_FREE(maximum_value);
        }
    }

    if (error) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    if (SHR_FUNC_ERR()) {
        if (minimum_value) {
            SHR_FREE(minimum_value);
        }
        if (maximum_value) {
            SHR_FREE(maximum_value);
        }
    }
    SHR_FUNC_EXIT();
}
