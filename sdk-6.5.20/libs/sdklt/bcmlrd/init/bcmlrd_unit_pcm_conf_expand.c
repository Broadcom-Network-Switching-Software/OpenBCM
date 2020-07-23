/*! \file bcmlrd_unit_pcm_conf_expand.c
 *
 * Return a mapping configuration for the given unit.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <sal/sal_alloc.h>
#include <bsl/bsl.h>
#include <bcmlrd/bcmlrd_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLRD_TABLE

/*******************************************************************************
 * Variables
 */

const bcmlrd_pcm_conf_compact_rep_t *
bcmlrd_unit_pcm_conf_compact[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

int
bcmlrd_unit_pcm_conf_expand(int unit)
{
    const bcmlrd_pcm_conf_compact_rep_t *compact_conf = NULL;
    const bcmlrd_table_pcm_info_t *pcm;
    bcmlrd_pcm_conf_rep_t *conf;
    uint32_t num_pcm;
    uint32_t i;

    if (unit >= 0 && unit < COUNTOF(bcmlrd_unit_pcm_conf)) {
        compact_conf = bcmlrd_unit_pcm_conf_compact[unit];
    }

    if (compact_conf == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "PCM configuration not available.")));
        return SHR_E_UNAVAIL;
    }

    if (bcmlrd_unit_pcm_conf[unit] != NULL) {
        sal_free(bcmlrd_unit_pcm_conf[unit]);
        bcmlrd_unit_pcm_conf[unit] = NULL;
    }

    conf = sal_alloc(sizeof(bcmlrd_pcm_conf_rep_t), "bcmlrdPcmConfExpand");
    if (conf == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "No memory for PCM configurations.")));
        return SHR_E_MEMORY;
    }
    sal_memset(conf, 0 , sizeof(bcmlrd_pcm_conf_rep_t));

    num_pcm = compact_conf->num_pcm;
    sal_strncpy(conf->name, compact_conf->name, sizeof(conf->name) - 1);

    for (i = 0; i < num_pcm; i++) {
        pcm = compact_conf->pcm[i];
        if (pcm == NULL) {
            /* PCM can be NULL if the module gets excluded. */
            break;
        }
        if (pcm->src_id >= BCMLRD_TABLE_COUNT) {
            /* LTID cannot be greater than table count. */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Incorrect PCM data.")));
            sal_free(conf);
            return SHR_E_INTERNAL;
        }

        conf->pcm[pcm->src_id] = pcm;
    }

    bcmlrd_unit_pcm_conf[unit] = conf;

    return SHR_E_NONE;
}
