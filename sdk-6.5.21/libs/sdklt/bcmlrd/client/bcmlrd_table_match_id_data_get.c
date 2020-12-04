/*! \file bcmlrd_table_match_id_data_get.c
 *
 * Get the match id information for given match id spec.
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
#include <bcmlrd/bcmlrd_client.h>
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
bcmlrd_table_match_id_data_get(int unit,
                               const char *spec,
                               const bcmlrd_match_id_db_t **info)
{
    const bcmlrd_match_id_db_info_t *match_id_info;
    uint32_t id;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(spec, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    match_id_info = bcmlrd_unit_match_id_data_all_info_get(unit);
    if (match_id_info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *info = NULL;

    for (id = 0; id < match_id_info->num_entries; id++) {
        if (!sal_strcmp(match_id_info->db[id].name, spec)) {
            *info = &match_id_info->db[id];
            break;
        }
    }

    if (*info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

