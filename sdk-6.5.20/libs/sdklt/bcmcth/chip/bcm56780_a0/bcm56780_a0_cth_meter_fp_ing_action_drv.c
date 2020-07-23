/*! \file bcm56780_a0_cth_meter_fp_ing_action_drv.c
 *
 * This file contains bcm56780_a0 chip specific attributes
 * for IFP meter action template LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <bcmdrd_config.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmcth/bcmcth_meter_fp_ing_action.h>
#include <bcmcth/bcmcth_meter_sysm.h>
#include "bcm56780_a0_cth_meter_fp_ing_action_tbls.h"

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_METER

/*******************************************************************************
 * Private functions
 */

static int bcm56780_a0_meter_fp_ing_action_db_init(int unit)
{
    uint16_t    idx = 0, lt_idx = 0, cont_idx = 0;
    uint32_t    pdd_fid = 0;
    bcmltd_fid_t fid = 0;
    bcmcth_meter_action_drv_t *drv = NULL;
    const bcmlrd_table_pcm_info_t *pcm_info = NULL;
    const bcmlrd_field_pdd_info_t *action_pdd_info = NULL;
    bcmcth_meter_pdd_cont_info_t *cont_info = NULL;

    SHR_FUNC_ENTER(unit);

    drv = bcm56780_a0_cth_meter_ing_action_drv_get(unit);
    if (drv == NULL) {
        return SHR_E_UNAVAIL;
    }

    for (lt_idx = 0; lt_idx < drv->num_action_lts; lt_idx++) {
        cont_info = drv->pdd_lt_info->cont_info;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_pcm_conf_get(unit,
                                       drv->action_lt_info[lt_idx].ltid,
                                       &pcm_info));

        for (idx = 0; idx < pcm_info->field_count; idx++) {
            fid = pcm_info->field_info[idx].id;
            action_pdd_info = pcm_info->field_info[idx].pdd_info;
            if (action_pdd_info == NULL) {
                continue;
            }

            pdd_fid = drv->action_lt_info[lt_idx].fid_info[fid].map_fid;
            cont_info[pdd_fid].num_conts = action_pdd_info->count;
            for (cont_idx = 0; cont_idx < action_pdd_info->count; cont_idx++) {
                cont_info[pdd_fid].cont_ids[cont_idx] =
                    action_pdd_info->info[cont_idx].phy_cont_id;
                cont_info[pdd_fid].cont_width[cont_idx] =
                    action_pdd_info->info[cont_idx].phy_cont_size;
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/* Physical table and field attributes. */
bcmcth_meter_action_pdd_pt_info_t bcm56780_a0_meter_fp_ing_action_pdd_pt_info = {
    .pt_id = IFP_METER_PDD_PROFILE_TABLEm,
    .pt_fid = SECTION_BITMAPf,
    .num_profiles = 32,
    .pdd_bmp_size = 20,
};

bcmcth_meter_action_sbr_pt_info_t bcm56780_a0_meter_fp_ing_action_sbr_pt_info = {
    .pt_id = IFP_METER_SBR_PROFILE_TABLE_0m,
    .pt_fid = STRENGTHf,
    .bits_per_pri = 3,
    .num_profiles = 32,
};

uint32_t bcm56780_a0_meter_fp_ing_action_table[] = {
    IFP_METER_COLOR_TABLE_0m,
    IFP_METER_COLOR_TABLE_1m,
    IFP_METER_COLOR_TABLE_2m,
    IFP_METER_COLOR_TABLE_3m,
};

bcmcth_meter_action_pt_info_t bcm56780_a0_meter_fp_ing_action_pt_info = {
    .pt_id = bcm56780_a0_meter_fp_ing_action_table,
    .pt_width = 40,
};

bcmcth_meter_action_drv_t bcm56780_a0_meter_fp_ing_action_drv = {
    .init = &bcm56780_a0_meter_fp_ing_action_db_init,
    .num_action_lts = 0,
    .pdd_lt_info = NULL,
    .pdd_pt_info = &bcm56780_a0_meter_fp_ing_action_pdd_pt_info,
    .sbr_lt_info = NULL,
    .sbr_pt_info = &bcm56780_a0_meter_fp_ing_action_sbr_pt_info,
    .action_lt_info = NULL,
    .action_pt_info = &bcm56780_a0_meter_fp_ing_action_pt_info,
};

bcmcth_meter_action_drv_t *
bcm56780_a0_cth_meter_ing_action_drv_get(int unit)
{
    return &bcm56780_a0_meter_fp_ing_action_drv;
}

