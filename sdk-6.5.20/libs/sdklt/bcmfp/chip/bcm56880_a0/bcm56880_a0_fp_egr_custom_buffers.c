/*! \file bcm56880_a0_fp_egr_custom_buffers.c
 *
 * APIs to intsall/uninstall to configurations to h/w
 * memories/registers to create/destroy FP egr group for
 * Trident4 device(56880_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56880_a0_fp.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmptm/bcmptm_uft.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_pdd_internal.h>
#include <bcmfp/bcmfp_cth_pdd.h>
#include <bcmfp/bcmfp_ptm_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static uint32_t kap_entry[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];

STATIC int
bcm56880_a0_egr_sbr_profile_install(int unit,
                                   uint32_t trans_id,
                                   bool default_entry,
                                   void *group_opinfo,
                                   bcmfp_stage_t *stage,
                                   bcmfp_buffers_t *buffers)
{
    int index = 0;
    uint32_t *prof_entry = NULL;
    bcmdrd_sid_t egr_sbr_profile_sid[2] = { 0 };
    bcmfp_group_oper_info_t *opinfo = NULL;
    size_t size = 0;
    int pipe_id = -1;
    bcmltd_sid_t tbl_id;
    bcmfp_ext_codes_t *ext_codes = NULL;
    uint8_t idx = 0;
    uint8_t start_idx = 0;
    uint8_t end_idx = 0;
    uint32_t *profile_data = NULL;
    uint8_t parts_count = 0;
    uint8_t part = 0;
    uint8_t sid_idx = 0;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(buffers, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    prof_entry = kap_entry[unit];
    size = (sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 opinfo->flags,
                                 &parts_count));
    tbl_id = stage->tbls.sbr_tbl->sid;
    ext_codes = &(opinfo->ext_codes[0]);
    pipe_id = opinfo->tbl_inst;

    egr_sbr_profile_sid[0] = EFTA30_SBR_PROFILE_TABLE_0m;
    egr_sbr_profile_sid[1] = EFTA30_SBR_PROFILE_TABLE_0_EXTm;

    if (default_entry == TRUE) {
        index = opinfo->def_sbr_id;
    } else {
        index = opinfo->sbr_id;
    }

    start_idx =
        ((parts_count -1) * stage->sbr_hw_info->sbr_profile_sid_columns);
    end_idx = (start_idx + stage->sbr_hw_info->sbr_profile_sid_columns);
    for (idx = start_idx ; idx < end_idx; idx++) {
        sal_memset(prof_entry, 0, size);

        if (TRUE == default_entry) {
            profile_data = buffers->dsbr[idx];
        } else {
            profile_data = buffers->sbr[idx];
        }

        bcmdrd_pt_field_set(unit,
                            egr_sbr_profile_sid[sid_idx],
                            prof_entry,
                            DATAf,
                            profile_data);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_index_insert(unit,
                                    trans_id,
                                    pipe_id,
                                    tbl_id,
                                    egr_sbr_profile_sid[sid_idx],
                                    prof_entry,
                                    index));
        sid_idx++;
    }

    for (part = 0; part < parts_count; part++) {
        ext_codes = &(opinfo->ext_codes[part]);
        if (TRUE == default_entry) {
            ext_codes->default_sbr_prof_index = (uint32_t)index;
        } else {
            ext_codes->sbr_prof_index = (uint32_t)index;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56880_a0_egr_sbr_profile_uninstall(int unit,
                                     uint32_t trans_id,
                                     bool default_entry,
                                     void *group_opinfo,
                                     bcmfp_stage_t *stage)
{
    int index = 0;
    bcmdrd_sid_t egr_sbr_profile_sid[2] = { 0 };
    bcmfp_group_oper_info_t *opinfo = NULL;
    int pipe_id = -1;
    bcmltd_sid_t tbl_id;
    bcmfp_ext_codes_t *ext_codes = NULL;
    uint8_t idx = 0;
    uint8_t parts_count = 0;
    uint8_t part = 0;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 opinfo->flags,
                                 &parts_count));
    tbl_id = stage->tbls.sbr_tbl->sid;
    ext_codes = &(opinfo->ext_codes[0]);
    pipe_id = opinfo->tbl_inst;

    egr_sbr_profile_sid[0] = EFTA30_SBR_PROFILE_TABLE_0m;
    egr_sbr_profile_sid[1] = EFTA30_SBR_PROFILE_TABLE_0_EXTm;

    if (default_entry == TRUE) {
        index = opinfo->def_sbr_id;
    } else {
        index = opinfo->sbr_id;
    }

    for (idx = 0; idx < stage->sbr_hw_info->sbr_profile_sid_columns;
        idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_index_delete(unit,
                                    trans_id,
                                    pipe_id,
                                    tbl_id,
                                    egr_sbr_profile_sid[idx],
                                    index));
    }

    for (part = 0; part < parts_count; part++) {
        ext_codes = &(opinfo->ext_codes[part]);
        if (TRUE == default_entry) {
            ext_codes->default_sbr_prof_index = 0;
        } else {
            ext_codes->sbr_prof_index = 0;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_fp_egr_profiles_install(int unit,
                                   uint32_t stage_id,
                                   uint32_t trans_id,
                                   uint32_t flags,
                                   bcmfp_group_id_t group_id,
                                   bcmfp_group_oper_info_t *opinfo,
                                   bcmfp_buffers_t *buffers)
{
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffers, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (flags & BCMFP_GROUP_INSTALL_SBR_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56880_a0_egr_sbr_profile_install(unit,
                                                trans_id,
                                                FALSE,
                                                (void *)opinfo,
                                                stage,
                                                buffers));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_fp_egr_profiles_uninstall(int unit,
                                     uint32_t stage_id,
                                     uint32_t trans_id,
                                     uint32_t flags,
                                     bcmfp_group_id_t group_id,
                                     bcmfp_group_oper_info_t *opinfo)
{
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (flags & BCMFP_GROUP_INSTALL_SBR_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56880_a0_egr_sbr_profile_uninstall(unit,
                                                  trans_id,
                                                  FALSE,
                                                  (void *)opinfo,
                                                  stage));
    }

exit:
    SHR_FUNC_EXIT();
}
