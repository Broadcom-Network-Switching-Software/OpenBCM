/*! \file bcm56880_a0_fp_common_pdd.c
 *
 * APIs to intsall/uninstall to configurations to h/w
 * memories/registers to create/destroy the IFP/EFP/VFP
 * PDD for Trident4 device(56880_A0).
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
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_pdd_internal.h>
#include <bcmfp/bcmfp_cth_pdd.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56880_a0_fp_common_pdd_profile_entry_get(int unit,
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
    bcmfp_stage_t *stage = NULL;
    bcmfp_pdd_section_info_t section_info;
    uint32_t cont_bitmap[BCMFP_MAX_PDD_WORDS];
    bool is_valid_pdd_oper = FALSE;
    uint32_t *prof_entry = NULL;
    bcmdrd_sid_t mem_sid;
    uint32_t value = 0;
    uint32_t def_cont_bitmap[BCMFP_MAX_PDD_WORDS];
    bcmfp_action_data_t *pdd_data = NULL;
    uint32_t aux_data[1] = { 0 };
    uint8_t keygen_parts = 0;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);
    SHR_NULL_CHECK(pdd_profile, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmfp_pdd_oper_status_get(unit,
                              stage_id,
                              pdd_id,
                              pdd_type,
                              &is_valid_pdd_oper));

    if (is_valid_pdd_oper == FALSE) {
        SHR_EXIT();
    }

    size = sizeof(uint32_t) * BCMFP_MAX_PDD_WORDS;
    sal_memset(def_cont_bitmap, 0, size);

    if (stage_id == BCMFP_STAGE_ID_INGRESS) {
        mem_sid = IFTA100_T4T_00_PDD_PROFILE_TABLE_0m;
    } else if (stage_id == BCMFP_STAGE_ID_EGRESS) {
        mem_sid = EFTA30_T4T_00_PDD_PROFILE_TABLE_0m;
    } else if (stage_id == BCMFP_STAGE_ID_LOOKUP) {
        mem_sid = IFTA40_T4T_01_PDD_PROFILE_TABLE_0m;
    } else if (stage_id == BCMFP_STAGE_ID_EXACT_MATCH) {
        mem_sid = IFTA90_E2T_00_PDD_PROFILE_TABLE_0m;
    } else if (stage_id == BCMFP_STAGE_ID_FLOW_TRACKER) {
        mem_sid = IFTA80_E2T_00_PDD_PROFILE_TABLE_0m;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 opinfo->flags,
                                 &keygen_parts));
    pdd_data = pdd_config->action_data;

    while (pdd_data != NULL) {
        if (((stage->action_cfg_db->action_cfg[pdd_data->action]->flags)
             & BCMFP_ACTION_IN_AUX_CONTAINER)) {
            if (pdd_data->action ==
                stage->tbls.pdd_tbl->aux_drop_code_fid) {
                value = pdd_data->param;
                bcmdrd_field_set(aux_data, 0, 7, &value);
            } else if (pdd_data->action ==
                stage->tbls.pdd_tbl->aux_drop_priority_fid) {
                value = pdd_data->param;
                bcmdrd_field_set(aux_data, 8, 11, &value);
            } else if (pdd_data->action ==
                stage->tbls.pdd_tbl->aux_trace_id_fid) {
                value = pdd_data->param;
                bcmdrd_field_set(aux_data, 12, 17, &value);
            } else if (pdd_data->action ==
                stage->tbls.pdd_tbl->aux_flex_ctr_valid_fid) {
                value = pdd_data->param;
                bcmdrd_field_set(aux_data, 18, 18, &value);
            } else {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
        pdd_data = pdd_data->next;
    }

    for (part = 0; part < keygen_parts; part++) {
        /*
         * Write data to all parts as PTRM-TCAM considers
         * PDD profiles as global resource. And in TD4,
         * multiwide policy is not supported. so copy part
         * 0 information to all other parts.
         */
        prof_entry = pdd_profile[part];

        sal_memset(prof_entry, 0, size);
        sal_memset(cont_bitmap, 0, size);
        SHR_IF_ERR_VERBOSE_EXIT
             (bcmfp_pdd_bitmap_get(unit,
                                   stage_id,
                                   pdd_id,
                                   0,
                                   pdd_type,
                                   cont_bitmap));
        /*
         * Check if container bitmap is 0, if 0, dont write
         * anything in this part to HW.
         */
        if (sal_memcmp(cont_bitmap, def_cont_bitmap, size) == 0) {
            continue;
        }

        bcmdrd_pt_field_set(unit,
                            mem_sid,
                            prof_entry,
                            SECTION_BITMAPf,
                            cont_bitmap);

        SHR_IF_ERR_VERBOSE_EXIT
             (bcmfp_pdd_section_info_get(unit,
                                         stage_id,
                                         pdd_id,
                                         0,
                                         pdd_type,
                                         &section_info));

        value = section_info.section_offset[0];
        bcmdrd_pt_field_set(unit,
                            mem_sid,
                            prof_entry,
                            SECTION0_OFFSETf,
                            &value);

        value = section_info.section_offset[1];
        bcmdrd_pt_field_set(unit,
                            mem_sid,
                            prof_entry,
                            SECTION1_OFFSETf,
                            &value);

        if (stage_id == BCMFP_STAGE_ID_INGRESS ||
            stage_id == BCMFP_STAGE_ID_EGRESS ||
            stage_id == BCMFP_STAGE_ID_EXACT_MATCH ||
            stage_id == BCMFP_STAGE_ID_FLOW_TRACKER) {
            value = section_info.section_offset[2];
            bcmdrd_pt_field_set(unit,
                                mem_sid,
                                prof_entry,
                                SECTION2_OFFSETf,
                                &value);
        }

        /* Set AUX data in profile table */
        bcmdrd_pt_field_set(unit,
                            mem_sid,
                            prof_entry,
                            AUX_PDD_DATAf,
                            aux_data);
    }

exit:
   SHR_FUNC_EXIT();
}
