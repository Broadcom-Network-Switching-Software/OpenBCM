/*! \file bcm56880_a0_fp_common_qual_db.c
 *
 * IFP/EFP/VFP qualifier configuration DB initialization function
 * for Trident4(56880_A0).
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
#include <bcmfp/bcmfp_keygen_api.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmfp/bcmfp_strings_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmdrd/chip/bcm56880_a0_tile_hcf.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56880_a0_fp_fixed_qual_init(int unit,
                               bcmfp_stage_t *stage,
                               uint32_t fid)
{
    bool unmapped = false;
    uint16_t cont_id[20] = {0};
    uint16_t num_cont = 0;
    uint16_t cont_idx = 0;
    uint8_t cont_width = 0;
    uint8_t chunk_width = 0;
    uint16_t chunk_cont_id = 0;
    uint16_t chunk_offset = 0;
    uint8_t chunk_idx = 0;
    uint8_t section_width = 0;
    uint16_t start_cont_idx = 0;
    uint16_t end_cont_idx = 0;
    bcmfp_qual_flags_bmp_t qual_flags;
    bcmltd_sid_t sid = BCMLTD_SID_INVALID;
    bcmfp_keygen_qual_cfg_info_db_t *db = NULL;
    BCMFP_KEYGEN_QUAL_CFG_DECL;
    bcmfp_keygen_ext_section_t section_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    sid = stage->tbls.rule_tbl->sid;

    unmapped = false;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_is_unmapped(unit, sid, fid, &unmapped));
    if (unmapped == true) {
        SHR_EXIT();
    }

    db = stage->kgn_qual_cfg_info_db;

    switch (stage->stage_id) {
    case BCMFP_STAGE_ID_INGRESS:
        if (fid == stage->tbls.rule_tbl->hash_a0_fid) {
            num_cont = 1;
            cont_id[0] = 146;
            cont_width = 16;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E16;
            section_width = 16;
            db->qual_name[fid] = "HASH_A0";
        } else if (fid == stage->tbls.rule_tbl->hash_a1_fid) {
            num_cont = 1;
            cont_id[0] = 147;
            cont_width = 16;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E16;
            section_width = 16;
            db->qual_name[fid] = "HASH_A1";
        } else if (fid == stage->tbls.rule_tbl->hash_b0_fid) {
            num_cont = 1;
            cont_id[0] = 148;
            cont_width = 16;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E16;
            section_width = 16;
            db->qual_name[fid] = "HASH_B0";
        } else if (fid == stage->tbls.rule_tbl->hash_b1_fid) {
            num_cont = 1;
            cont_id[0] = 149;
            cont_width = 16;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E16;
            section_width = 16;
            db->qual_name[fid] = "HASH_B1";
        } else if (fid == stage->tbls.rule_tbl->hash_c0_fid) {
            num_cont = 1;
            cont_id[0] = 150;
            cont_width = 16;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E16;
            section_width = 16;
            db->qual_name[fid] = "HASH_C0";
        } else if (fid == stage->tbls.rule_tbl->hash_c1_fid) {
            num_cont = 1;
            cont_id[0] = 151;
            cont_width = 16;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E16;
            section_width = 16;
            db->qual_name[fid] = "HASH_C1";
        } else if (fid == stage->tbls.rule_tbl->drop_data_fid) {
            num_cont = 5;
            cont_id[0] = 100;
            cont_id[1] = 101;
            cont_id[2] = 102;
            cont_id[3] = 103;
            cont_id[4] = 104;
            cont_width = 4;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E4;
            section_width = 4;
            db->qual_name[fid] = "DROP_DATA";
        } else if (fid == stage->tbls.rule_tbl->drop_decision_fid) {
            num_cont = 1;
            cont_id[0] = 105;
            cont_width = 4;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E4;
            section_width = 4;
            db->qual_name[fid] = "DROP_DECESION";
        } else if (fid == stage->tbls.rule_tbl->trace_vector_fid) {
            num_cont = 12;
            cont_id[0] = 106;
            cont_id[1] = 107;
            cont_id[2] = 108;
            cont_id[3] = 109;
            cont_id[4] = 110;
            cont_id[5] = 111;
            cont_id[6] = 112;
            cont_id[7] = 113;
            cont_id[8] = 114;
            cont_id[9] = 115;
            cont_id[10] = 116;
            cont_id[11] = 117;
            cont_width = 4;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E4;
            section_width = 4;
            db->qual_name[fid] = "TRACE_VECTOR";
        } else {
            SHR_EXIT();
        }
        break;
    case BCMFP_STAGE_ID_EGRESS:
        if (fid == stage->tbls.rule_tbl->drop_data_fid) {
            num_cont = 5;
            cont_id[0] = 64;
            cont_id[1] = 65;
            cont_id[2] = 66;
            cont_id[3] = 67;
            cont_id[4] = 68;
            cont_width = 4;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E4;
            section_width = 4;
        } else if (fid == stage->tbls.rule_tbl->drop_decision_fid) {
            num_cont = 1;
            cont_id[0] = 69;
            cont_width = 4;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E4;
            section_width = 4;
        } else if (fid == stage->tbls.rule_tbl->trace_vector_fid) {
            num_cont = 12;
            cont_id[0] = 70;
            cont_id[1] = 71;
            cont_id[2] = 72;
            cont_id[3] = 73;
            cont_id[4] = 74;
            cont_id[5] = 75;
            cont_id[6] = 76;
            cont_id[7] = 77;
            cont_id[8] = 78;
            cont_id[9] = 79;
            cont_id[10] = 80;
            cont_id[11] = 81;
            cont_width = 4;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E4;
            section_width = 4;
        } else {
            SHR_EXIT();
        }
        break;
    case BCMFP_STAGE_ID_LOOKUP:
        if (fid == stage->tbls.rule_tbl->drop_data_fid) {
            num_cont = 5;
            cont_id[0] = 60;
            cont_id[1] = 61;
            cont_id[2] = 62;
            cont_id[3] = 63;
            cont_id[4] = 64;
            cont_width = 4;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E4;
            section_width = 4;
        } else if (fid == stage->tbls.rule_tbl->drop_decision_fid) {
            num_cont = 1;
            cont_id[0] = 65;
            cont_width = 4;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E4;
            section_width = 4;
        } else if (fid == stage->tbls.rule_tbl->trace_vector_fid) {
            num_cont = 12;
            cont_id[0] = 66;
            cont_id[1] = 67;
            cont_id[2] = 68;
            cont_id[3] = 69;
            cont_id[4] = 70;
            cont_id[5] = 71;
            cont_id[6] = 72;
            cont_id[7] = 73;
            cont_id[8] = 74;
            cont_id[9] = 75;
            cont_id[10] = 76;
            cont_id[11] = 77;
            cont_width = 4;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E4;
            section_width = 4;
        } else {
            SHR_EXIT();
        }
        break;
    case BCMFP_STAGE_ID_EXACT_MATCH:
    case BCMFP_STAGE_ID_FLOW_TRACKER:
        if (fid == stage->tbls.rule_tbl->hash_a0_fid) {
            num_cont = 1;
            cont_id[0] = 130;
            cont_width = 16;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E16;
            section_width = 16;
        } else if (fid == stage->tbls.rule_tbl->hash_a1_fid) {
            num_cont = 1;
            cont_id[0] = 131;
            cont_width = 16;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E16;
            section_width = 16;
        } else if (fid == stage->tbls.rule_tbl->hash_b0_fid) {
            num_cont = 1;
            cont_id[0] = 132;
            cont_width = 16;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E16;
            section_width = 16;
        } else if (fid == stage->tbls.rule_tbl->hash_b1_fid) {
            num_cont = 1;
            cont_id[0] = 133;
            cont_width = 16;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E16;
            section_width = 16;
        } else if (fid == stage->tbls.rule_tbl->hash_c0_fid) {
            num_cont = 1;
            cont_id[0] = 134;
            cont_width = 16;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E16;
            section_width = 16;
        } else if (fid == stage->tbls.rule_tbl->hash_c1_fid) {
            num_cont = 1;
            cont_id[0] = 135;
            cont_width = 16;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E16;
            section_width = 16;
        } else if (fid == stage->tbls.rule_tbl->drop_decision_fid) {
            num_cont = 1;
            cont_id[0] = 100;
            cont_width = 4;
            section_id = BCMFP_KEYGEN_EXT_SECTION_L1E4;
            section_width = 4;
        } else {
            SHR_EXIT();
        }
        break;
    default:
        break;
    }

    if (cont_width == 0) {
        SHR_EXIT();
    }

    fid = stage->tbls.rule_tbl->qualifiers_fid_info[fid].qualifier;
    start_cont_idx = 0;
    end_cont_idx = num_cont;
    chunk_idx = 0;
    BCMFP_KEYGEN_QUAL_CFG_INIT(unit);
    sal_memset(&qual_flags, 0, sizeof(bcmfp_qual_flags_bmp_t));
    for (cont_idx = start_cont_idx;
         cont_idx < end_cont_idx;
         cont_idx++) {
        chunk_cont_id = cont_id[cont_idx];
        chunk_offset = (chunk_cont_id * section_width);
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
    }
    BCMFP_KEYGEN_QUAL_CFG_INSERT(unit, db, fid, qual_flags);

exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_fp_common_qual_db_init(int unit,
                                bcmfp_stage_t *stage)
{
    bool unmapped = false;
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
    bcm56880_a0_tile_id_t tile_id = 0;
    uint8_t gran = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_fp_common_presel_qual_db_init(unit, stage));

    if (stage->tbls.rule_tbl != NULL ) {
        sid = stage->tbls.rule_tbl->sid;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (stage->stage_id == BCMFP_STAGE_ID_INGRESS) {
       tile_id = BCMDRD_BCM56880_A0_TILE_IFTA100;
    } else if (stage->stage_id == BCMFP_STAGE_ID_EGRESS) {
       tile_id = BCMDRD_BCM56880_A0_TILE_EFTA30;
    } else if (stage->stage_id == BCMFP_STAGE_ID_LOOKUP) {
       tile_id = BCMDRD_BCM56880_A0_TILE_IFTA40;
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
                (bcm56880_a0_fp_fixed_qual_init(unit, stage, fid));
            continue;
        }

        fid = pcm_info->field_info[qual_idx].id;

        unmapped = false;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_is_unmapped(unit, sid, fid, &unmapped));
        if (unmapped == true) {
            continue;
        }

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
        db->qual_name[fid] = pcm_info->field_info[qual_idx].name;
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

    BCMFP_KEYGEN_QUAL_CFG_INIT(unit);
    BCMFP_KEYGEN_QUAL_CFG_OFFSET_ADD(0,
                                     BCMFP_KEYGEN_EXT_SECTION_KEY_TYPE,
                                     0,
                                     0,
                                     4);
    qual_id = stage->tbls.rule_tbl->key_type_fid;
    fid = stage->tbls.rule_tbl->qualifiers_fid_info[qual_id].qualifier;
    SHR_BITSET(qual_flags.w, BCMFP_KEYGEN_QUAL_FLAGS_PMUX);
    BCMFP_KEYGEN_QUAL_CFG_INSERT(unit, db, fid, qual_flags);

exit:
    SHR_FUNC_EXIT();
}
