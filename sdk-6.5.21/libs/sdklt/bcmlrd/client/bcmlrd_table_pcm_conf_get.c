/*! \file bcmlrd_table_pcm_conf_get.c
 *
 * Get the PCM configuration for the given table.
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
bcmlrd_table_pcm_conf_get(int unit,
                          bcmlrd_sid_t sid,
                          const bcmlrd_table_pcm_info_t **pcm_info)
{
    bcmlrd_pcm_conf_rep_t *pcm_conf;
    const bcmlrd_table_pcm_info_t *pcm;

    SHR_FUNC_ENTER(unit);

    pcm_conf = bcmlrd_unit_pcm_conf_get(unit);
    if (pcm_conf == NULL) {
        /* Should have a configuration for this unit */
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Unit PCM configuration not available")));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    pcm = pcm_conf->pcm[sid];
    if (pcm == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Table PCM configuration not available")));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *pcm_info = pcm;
exit:
    SHR_FUNC_EXIT();
}

