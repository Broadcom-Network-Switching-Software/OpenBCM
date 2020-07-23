/*! \file bcm56780_a0_fp_em_qual_db.c
 *
 * IFP/EFP/VFP qualifier configuration DB initialization function
 * for Trident4-X9(56780_A0).
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
#include <bcmfp/bcmfp_keygen_api.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmfp/bcmfp_strings_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56780_a0_tile_hcf.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56780_a0_fp_em_qual_db_init(int unit,
                               bcmfp_stage_t *stage)
{
    uint16_t qual_idx = 0;
    uint16_t cont_idx = 0;
    uint16_t inst_idx = 0;
    uint16_t index = 0;
    bcmltd_fid_t fid = 0;
    bcmltd_sid_t sid = BCMLTD_SID_INVALID;
    bcmfp_qual_flags_bmp_t qual_flags;
    bcmfp_keygen_qual_cfg_info_db_t *db = NULL;
    BCMFP_KEYGEN_QUAL_CFG_DECL;
    const bcmlrd_table_pcm_info_t *pcm_info = NULL;
    const bcmlrd_field_cont_info_t *qual_cont_info = NULL;
    const bcmlrd_cont_info_t *cont_info = NULL;
    bcmfp_keygen_ext_section_t section_id;
    uint8_t cont_width = 0;
    uint8_t chunk_width = 0;
    uint16_t chunk_cont_id = 0;
    uint16_t chunk_offset = 0;
    uint8_t chunk_idx = 0;
    uint8_t section_width = 0;
    const bcmlrd_field_data_t *field_data = NULL;
    uint16_t array_depth = 0;
    uint16_t array_idx = 0;
    uint16_t start_cont_idx = 0;
    uint16_t end_cont_idx = 0;
    bcmfp_qualifier_t qual_id = 0;
    uint8_t num_conts_per_array_element = 0;
    bcm56780_a0_tile_id_t tile_id = 0;
    uint8_t gran = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_fp_common_presel_qual_db_init(unit, stage));

    if (stage->tbls.rule_tbl != NULL ) {
        sid = stage->tbls.rule_tbl->sid;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (stage->stage_id == BCMFP_STAGE_ID_EXACT_MATCH) {
       tile_id = BCMDRD_BCM56780_A0_TILE_IFTA90;
    } else if (stage->stage_id == BCMFP_STAGE_ID_FLOW_TRACKER) {
       tile_id = BCMDRD_BCM56780_A0_TILE_IFTA80;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_pcm_conf_get(unit, sid, &pcm_info));
    if (!pcm_info->field_count || pcm_info->field_info == NULL) {
        /* For any FP stage to operate rule LT is mandatory. */
        LOG_ERROR(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Rule LT is missing for %s "
                 "stage\n"), bcmfp_stage_string(stage->stage_id)));
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    BCMFP_ALLOC(stage->kgn_qual_cfg_info_db,
                sizeof(bcmfp_keygen_qual_cfg_info_db_t),
                "bcmfpKeyGenQualCfgInfoDb");
    db = stage->kgn_qual_cfg_info_db;
    sal_memset(&qual_flags, 0, sizeof(bcmfp_qual_flags_bmp_t));

    for (qual_idx = 0; qual_idx < pcm_info->field_count; qual_idx++) {
        qual_cont_info = pcm_info->field_info[qual_idx].cont_info;
        if (qual_cont_info == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,
                        "%s stage doest not support %s qualifier\n"),
                        bcmfp_stage_string(stage->stage_id),
                        pcm_info->field_info[qual_idx].name));
            fid = pcm_info->field_info[qual_idx].id;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56780_a0_fp_fixed_qual_init(unit, stage, fid));
            continue;
        }

        fid = pcm_info->field_info[qual_idx].id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_get(unit, sid, fid, &field_data));
        if (field_data->depth) {
            array_depth = field_data->depth;
        } else {
            array_depth = 1;
        }
        if (qual_cont_info->count % array_depth) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        num_conts_per_array_element = (qual_cont_info->count / array_depth);
        fid = stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier;
        for (inst_idx = 0; inst_idx < qual_cont_info->instances; inst_idx++) {
            index = inst_idx * qual_cont_info->count;
            if (qual_cont_info->info[index].width == 0) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            for (array_idx = 0; array_idx < array_depth; array_idx++) {
                start_cont_idx =
                      (index + (num_conts_per_array_element * array_idx));
                end_cont_idx =
                       (start_cont_idx + num_conts_per_array_element);
                chunk_idx = 0;
                BCMFP_KEYGEN_QUAL_CFG_INIT(unit);
                for (cont_idx = start_cont_idx;
                     cont_idx < end_cont_idx;
                     cont_idx++) {
                    cont_info = &(qual_cont_info->info[cont_idx]);
                    if (cont_info->width == 0) {
                        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                    }
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmfp_tile_l0_section_gran_get(unit,
                                                        tile_id,
                                                        cont_info->section_id,
                                                        &gran));
                    if (gran == 16) {
                        section_id = BCMFP_KEYGEN_EXT_SECTION_L1E16;
                        section_width = 16;
                    } else if (gran == 4) {
                        section_id = BCMFP_KEYGEN_EXT_SECTION_L1E4;
                        section_width = 4;
                    } else if (gran == 32) {
                        section_id = BCMFP_KEYGEN_EXT_SECTION_L1E32;
                        section_width = 32;
                    } else {
                        SHR_ERR_EXIT(SHR_E_INTERNAL);
                    }

                    cont_width = cont_info->width;
                    chunk_offset = cont_info->bit_offset;
                    while (cont_width != 0) {
                        chunk_cont_id = (chunk_offset /
                                         section_width);
                        chunk_width = (section_width -
                                      (chunk_offset %
                                       section_width));
                        if (cont_width < chunk_width) {
                            chunk_width = cont_width;
                        }
                        BCMFP_KEYGEN_QUAL_CFG_OFFSET_ADD(chunk_idx,
                                                         section_id,
                                                         chunk_cont_id,
                                                         chunk_offset,
                                                         chunk_width);
                        chunk_idx++;
                        cont_width -= chunk_width;
                        chunk_offset += chunk_width;
                    }
               }
               qual_id = fid + array_idx;
               BCMFP_KEYGEN_QUAL_CFG_INSERT(unit, db, qual_id, qual_flags);
           }
        }
    }
exit:
    SHR_FUNC_EXIT();

}
