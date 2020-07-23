/*! \file bcmlrd_custom_tables_count_get.c
 *
 * Get the count of logical tables for the given combination of
 * component name and handler name.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <bsl/bsl.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLRD_TABLE

/*******************************************************************************
 * Public functions
 */

int
bcmlrd_custom_tables_count_get(int unit,
                               const char *component_name,
                               const char *handler_name,
                               size_t *count)
{
    const bcmlrd_dev_comp_t *conf;
    const bcmlrd_comp_desc_t *desc;
    const bcmlrd_comp_hdl_t *hdl;
    const bcmlrd_map_conf_rep_t *map_conf;
    uint32_t cid;
    uint32_t hid;
    uint32_t tid;
    bcmlrd_sid_t tbl;
    int found = 0;
    size_t num_tbl = 0;

    SHR_FUNC_ENTER(unit);
    if ((unit < 0) || (unit >= BCMDRD_CONFIG_MAX_UNITS)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid unit.\n")));
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if ((count == NULL) || (component_name == NULL) ||
        (handler_name == NULL)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "NULL input parameters.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    map_conf = bcmlrd_unit_conf_get(unit);
    if (map_conf == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Map conf not available.\n")));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    conf = bcmlrd_unit_comp_conf_get(unit);
    if (conf == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Component conf not available.\n")));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    for (cid = 0; cid < conf->num_comp; cid++) {
        desc = &conf->desc[cid];
        if (sal_strcasecmp(desc->comp_name, component_name)) {
            continue;
        }

        for (hid = 0; hid < desc->num_hdl; hid++) {
            hdl = &desc->hdl[hid];
            if (!sal_strcasecmp(hdl->hdl_name, handler_name)) {
                found = 1;
                /* Parse through tables to filter out tables
                 * that are conditionally removed. */
                for (tid = 0; tid < hdl->num_tbl; tid++) {
                    tbl = hdl->tbl[tid];
                    if (map_conf->map[tbl] != NULL) {
                        num_tbl++;
                    }
                }
                *count = num_tbl;
            }
        }
    }

    if (found == 0) {
        *count = 0;
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}
