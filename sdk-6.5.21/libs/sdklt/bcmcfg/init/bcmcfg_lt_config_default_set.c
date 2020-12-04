/*! \file bcmcfg_lt_config_default_set.c
 *
 * Load map default value if not configured by config file.
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
bcmcfg_lt_config_default_set(int unit, bool warm)
{
    uint32_t sid;
    uint32_t fid;
    uint32_t fidx;
    uint32_t idx_count;
    uint32_t default_count;
    uint64_t *default_value = NULL;
    bcmcfg_config_data_t *user_config = NULL;
    const bcmltd_table_rep_t *tbl;
    const bcmlrd_map_t *map = NULL;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    for (sid = 0; sid < BCMLTD_TABLE_COUNT; sid++) {

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

        for (fid = 0; fid < tbl->fields; fid++) {
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
            SHR_ALLOC(default_value, idx_count * sizeof(uint64_t), "bcmcfgConfigDefaultSet");
            SHR_NULL_CHECK(default_value, SHR_E_MEMORY);
            SHR_IF_ERR_EXIT
                (bcmlrd_field_default_get(unit, sid, fid,
                                          idx_count,
                                          default_value,
                                          &default_count));

            for (fidx = 0; fidx < idx_count; fidx++) {
                uint32_t index = first_idx + fidx;

                if (!user_config ||
                    !(user_config[index].config_src == BCMCFG_CONFIG_SRC_TYPE_READER ||
                      user_config[index].config_src == BCMCFG_CONFIG_SRC_TYPE_API)) {
                    SHR_IF_ERR_EXIT(
                        bcmcfg_table_set_internal(unit, sid, fid, fidx, 1,
                                                  default_value, NULL));
                }
            }

            SHR_FREE(default_value);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (default_value) {
            SHR_FREE(default_value);
        }
    }
    SHR_FUNC_EXIT();
}
