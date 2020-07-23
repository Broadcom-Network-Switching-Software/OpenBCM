/*! \file bcm56780_a0_fp_common_pdd.c
 *
 * APIs to intsall/uninstall to configurations to h/w
 * memories/registers to create/destroy the IFP/EFP/VFP
 * PDD for Trident4-X9 device(56780_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56780_a0_fp.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_pdd_internal.h>
#include <bcmfp/bcmfp_cth_pdd.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56780_a0_fp_common_pdd_profile_entry_get(int unit,
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
    uint32_t sec_bitmap[BCMFP_MAX_PDD_WORDS] = {0};
    uint8_t start_bit, end_bit, num_bits;

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

        if ((stage_id == BCMFP_STAGE_ID_INGRESS) ||
            (stage_id == BCMFP_STAGE_ID_LOOKUP)  ||
            (stage_id == BCMFP_STAGE_ID_EGRESS)) {

            /*
             * VFP has 2 sections, one MSB(S1) and one LSB(S0) aligned.
             * EFP, IFP has 3 sections, one MSB(S2) and 2 LSB(S0, S1) aligned.
             */

            start_bit = stage->pdd_hw_info->section_hw_info->section_start_bit[0];
            if (stage_id == BCMFP_STAGE_ID_LOOKUP) {
                end_bit = stage->pdd_hw_info->section_hw_info->section_end_bit[0];
            } else {
                end_bit = stage->pdd_hw_info->section_hw_info->section_end_bit[1];
            }
            num_bits = end_bit - start_bit + 1;

            SHR_BITCOPY_RANGE(sec_bitmap, 0, cont_bitmap, start_bit, num_bits);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmdrd_pt_field_set(unit,
                                     mem_sid,
                                     prof_entry,
                                     LSB_SECTION_BITMAPf,
                                     sec_bitmap));

            if (stage_id == BCMFP_STAGE_ID_LOOKUP) {
                start_bit = stage->pdd_hw_info->section_hw_info->section_start_bit[1];
                end_bit = stage->pdd_hw_info->section_hw_info->section_end_bit[1];
            } else {
                start_bit = stage->pdd_hw_info->section_hw_info->section_start_bit[2];
                end_bit = stage->pdd_hw_info->section_hw_info->section_end_bit[2];
            }
            num_bits = end_bit - start_bit + 1;

            SHR_BITCOPY_RANGE(sec_bitmap, 0, cont_bitmap, start_bit, num_bits);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmdrd_pt_field_set(unit,
                                     mem_sid,
                                     prof_entry,
                                     SECTION_BITMAPf,
                                     sec_bitmap));

        } else {
            /*
             * EM  and FT have 3 sections all MSB aligned.
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmdrd_pt_field_set(unit,
                                     mem_sid,
                                     prof_entry,
                                     SECTION_BITMAPf,
                                     cont_bitmap));
        }

        SHR_IF_ERR_VERBOSE_EXIT
             (bcmfp_pdd_section_info_get(unit,
                           stage_id,
                           pdd_id,
                           0,
                           pdd_type,
                           &section_info));

        if ((stage_id == BCMFP_STAGE_ID_INGRESS) ||
            (stage_id == BCMFP_STAGE_ID_EGRESS)) {
            value = section_info.section_offset[1];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmdrd_pt_field_set(unit,
                                     mem_sid,
                                     prof_entry,
                                     LSB_SECTION1_OFFSETf,
                                     &value));

        } else if ((stage_id == BCMFP_STAGE_ID_EXACT_MATCH) ||
                   (stage_id == BCMFP_STAGE_ID_FLOW_TRACKER)) {
            value = section_info.section_offset[0];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmdrd_pt_field_set(unit,
                                     mem_sid,
                                     prof_entry,
                                     SECTION0_OFFSETf,
                                     &value));

            value = section_info.section_offset[1];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmdrd_pt_field_set(unit,
                                     mem_sid,
                                     prof_entry,
                                     SECTION1_OFFSETf,
                                     &value));
        }

        /* Set AUX data in profile table */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmdrd_pt_field_set(unit,
                                 mem_sid,
                                 prof_entry,
                                 AUX_PDD_DATAf,
                                 aux_data));
    }

exit:
    SHR_FUNC_EXIT();
}
