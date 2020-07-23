/*! \file bcm56990_a0_fp_em_pdd.c
 *
 * API to fetch PDD profiles needs to be installed in HW.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56990_a0_fp.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56990_a0_enum.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_pdd_internal.h>
#include <bcmfp/bcmfp_cth_pdd.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56990_a0_fp_em_pdd_profile_entry_get(int unit,
                                        uint32_t trans_id,
                                        bcmfp_stage_id_t stage_id,
                                        bcmfp_pdd_id_t pdd_id,
                                        bcmfp_group_id_t group_id,
                                        bcmfp_group_oper_info_t *opinfo,
                                        bcmfp_pdd_oper_type_t pdd_type,
                                        uint32_t **pdd_profile,
                                        bcmfp_pdd_config_t *pdd_config)
{
    uint8_t part = 0;
    uint32_t cont_bitmap[BCMFP_MAX_PDD_WORDS];
    bool is_valid_pdd_oper = FALSE;
    uint8_t num_parts = 0;
    uint32_t *prof_entry = NULL;
    bcmdrd_sid_t mem_sid;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);
    SHR_NULL_CHECK(pdd_profile, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmfp_pdd_oper_status_get(unit,
                                  stage_id,
                                  pdd_id,
                                  pdd_type,
                                  &is_valid_pdd_oper));

    if (is_valid_pdd_oper == FALSE) {
        SHR_EXIT();
    }

    mem_sid = EXACT_MATCH_ACTION_PROFILEm;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_num_parts_get(unit,
                                 stage_id,
                                 pdd_id,
                                 pdd_type,
                                 &num_parts));

    size = (sizeof(uint32_t) * BCMFP_MAX_PDD_WORDS);
    for (part = 0; part < num_parts; part++) {

        prof_entry = pdd_profile[part];

        sal_memset(prof_entry, 0, size);

        SHR_IF_ERR_VERBOSE_EXIT
             (bcmfp_pdd_bitmap_get(unit,
                                   stage_id,
                                   pdd_id,
                                   part,
                                   pdd_type,
                                   cont_bitmap));
        bcmdrd_pt_field_set(unit,
                            mem_sid,
                            prof_entry,
                            ACTION_SET_BITMAPf,
                            cont_bitmap);
    }

exit:
    SHR_FUNC_EXIT();
}
