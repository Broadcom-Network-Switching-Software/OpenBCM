/*! \file bcm56880_a0_fp_common_presel_qual_db.c
 *
 * IFP/EFP/VFP preselection qualifier configuration DB
 * initialization function for Trident4(56880_A0).
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

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

STATIC int
bcm56880_a0_fp_presel_matchid_quals_init(int unit,
                                         bcmfp_stage_t *stage)
{
    bcmltd_fid_t fid = 0;
    bcmltd_fid_t match_fid = 0;
    bcmfp_qual_flags_bmp_t qual_flags;
    bcmfp_keygen_qual_cfg_info_db_t *db = NULL;
    const bcmlrd_match_id_db_t *qual_info = NULL;
    uint8_t offset_id = 0;
    uint16_t base_bit_offset = 0;
    uint16_t bit_offset = 0;
    uint16_t curr_bit_offset = 0;
    int32_t start_offset = -1;
    uint16_t curr_offset = 0;
    uint16_t match_id_len = 0;
    uint32_t bit_mask = 1;
    uint16_t width = 0;
    uint16_t cont_id = 0;
    bcmltd_sid_t sid = BCMLTD_SID_INVALID;
    BCMFP_KEYGEN_QUAL_CFG_DECL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    /* Match ID is supported in IFP and EFP only */
    if (!(stage->stage_id == BCMFP_STAGE_ID_INGRESS ||
        stage->stage_id == BCMFP_STAGE_ID_LOOKUP ||
        stage->stage_id == BCMFP_STAGE_ID_EGRESS ||
        stage->stage_id == BCMFP_STAGE_ID_EXACT_MATCH ||
        stage->stage_id == BCMFP_STAGE_ID_FLOW_TRACKER)) {
        SHR_EXIT();
    }

    if (stage->tbls.pse_tbl != NULL ) {
        sid = stage->tbls.pse_tbl->sid;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    db = stage->kgn_presel_qual_cfg_info_db;

    sal_memset(&qual_flags, 0, sizeof(bcmfp_qual_flags_bmp_t));

    if (stage->stage_id == BCMFP_STAGE_ID_EGRESS) {
        base_bit_offset = 64 * 4;
    } else if (stage->stage_id == BCMFP_STAGE_ID_LOOKUP) {
        base_bit_offset = 60 * 4;
    } else {
        base_bit_offset = 100 * 4;
    }

    for (fid = 0; fid < stage->tbls.pse_tbl->fid_count; fid++) {

        if (!(SHR_BITGET(
                    stage->tbls.pse_tbl->qualifiers_fid_info[fid].flags.w,
                    BCMFP_QUALIFIER_FID_FLAGS_MATCH_ID)
            && SHR_BITGET(
                    stage->tbls.pse_tbl->qualifiers_fid_info[fid].flags.w,
                    BCMFP_QUALIFIER_FID_FLAGS_DATA))) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_match_id_db_get(unit, sid, fid, &qual_info));

        SHR_NULL_CHECK(qual_info, SHR_E_UNAVAIL);

        match_fid = stage->tbls.pse_tbl->qualifiers_fid_info[fid].qualifier;

        /* Initialize in 8 bit container bus */
        match_id_len = qual_info->pmaxbit - qual_info->pminbit + 1;
        offset_id = 0;

        BCMFP_KEYGEN_QUAL_CFG_INIT(unit);
        for (curr_offset = 0; curr_offset <= match_id_len; curr_offset++) {
            curr_bit_offset = base_bit_offset + qual_info->pminbit
                              + curr_offset;
            if ((qual_info->match_mask & (bit_mask << curr_offset)) != 0) {
                if (start_offset == -1) {
                    start_offset = curr_offset;
                }
            } else {
                if (start_offset != -1) {
                    width = curr_offset - start_offset;
                    bit_offset = base_bit_offset + qual_info->pminbit
                                 + start_offset;
                    cont_id = bit_offset / 8;
                    /*
                     * consecutive chunk of 1's end here,
                     * add qualifier offsets
                     */
                    BCMFP_KEYGEN_QUAL_CFG_OFFSET_ADD(offset_id,
                                 BCMFP_KEYGEN_EXT_SECTION_L1E8,
                                                       cont_id,
                                                    bit_offset,
                                                        width);
                    offset_id++;
                    start_offset = -1;
                    width = 0;

                }
            }
            if (((curr_bit_offset +1 ) % 8) == 0) {
                /*
                 * end of the 8 bit container -
                 * push the existing offset into the DB
                 */
                if (start_offset != -1) {
                    width = curr_offset - start_offset + 1;
                    bit_offset = base_bit_offset + qual_info->pminbit
                                 + start_offset;
                    cont_id = bit_offset / 8;
                    /*
                     * consecutive chunk of 1's end here ,
                     * add qualifier offsets
                     */
                    BCMFP_KEYGEN_QUAL_CFG_OFFSET_ADD(offset_id,
                                 BCMFP_KEYGEN_EXT_SECTION_L1E8,
                                                       cont_id,
                                                    bit_offset,
                                                        width);
                    offset_id++;
                    start_offset = -1;
                    width = 0;
                }

            }
        }
        BCMFP_KEYGEN_QUAL_CFG_INSERT(unit, db, match_fid, qual_flags);

        /* Initialize in 4 bit container bus */
        offset_id = 0;
        match_id_len = qual_info->pmaxbit - qual_info->pminbit + 1;

        BCMFP_KEYGEN_QUAL_CFG_INIT(unit);
        for (curr_offset = 0; curr_offset <= match_id_len; curr_offset++) {
            curr_bit_offset = base_bit_offset + qual_info->pminbit
                              + curr_offset;
            if ((qual_info->match_mask & (bit_mask << curr_offset)) != 0) {
                if (start_offset == -1) {
                    start_offset = curr_offset;
                }
            } else {
                if (start_offset != -1) {
                    width = curr_offset - start_offset;
                    bit_offset = base_bit_offset + qual_info->pminbit
                                 + start_offset;
                    cont_id = bit_offset / 4;
                    /*
                     * consecutive chunk of 1's end here ,
                     * add qualifier offsets
                     */
                    BCMFP_KEYGEN_QUAL_CFG_OFFSET_ADD(offset_id,
                                 BCMFP_KEYGEN_EXT_SECTION_L1E8,
                                                       cont_id,
                                                    bit_offset,
                                                         width);
                    offset_id++;
                    start_offset = -1;
                    width = 0;

                }
            }
            if (((curr_bit_offset +1 ) % 4) == 0) {
                /*
                 * end of the 4 bit container -
                 * push the existing offset into the DB
                 */
                if (start_offset != -1) {
                    width = curr_offset - start_offset + 1;
                    bit_offset = base_bit_offset + qual_info->pminbit
                                 + start_offset;
                    cont_id = bit_offset / 4;
                    /*
                     * consecutive chunk of 1's end here ,
                     * add qualifier offsets
                     */
                    BCMFP_KEYGEN_QUAL_CFG_OFFSET_ADD(offset_id,
                                 BCMFP_KEYGEN_EXT_SECTION_L1E4,
                                                       cont_id,
                                                    bit_offset,
                                                        width);
                    offset_id++;
                    start_offset = -1;
                    width = 0;
                }

            }
        }
        BCMFP_KEYGEN_QUAL_CFG_INSERT(unit, db, match_fid, qual_flags);

    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56880_a0_fp_presel_fixed_qual_init(int unit,
                                      bcmfp_stage_t *stage,
                                      uint32_t fid)
{
    bool unmapped = false;
    uint16_t cont_id[10] = {0};
    uint16_t num_cont = 0;
    uint16_t cont_idx = 0;
    uint16_t bit_offset = 0;
    uint8_t cont_width = 0;
    bcmfp_qual_flags_bmp_t qual_flags;
    bcmltd_sid_t sid = BCMLTD_SID_INVALID;
    bcmfp_keygen_qual_cfg_info_db_t *db = NULL;
    BCMFP_KEYGEN_QUAL_CFG_DECL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    sid = stage->tbls.pse_tbl->sid;

    unmapped = false;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_is_unmapped(unit, sid, fid, &unmapped));
    if (unmapped == true) {
        SHR_EXIT();
    }

    db = stage->kgn_presel_qual_cfg_info_db;
    sal_memset(&qual_flags, 0, sizeof(bcmfp_qual_flags_bmp_t));

    switch (stage->stage_id) {
    case BCMFP_STAGE_ID_INGRESS:
    case BCMFP_STAGE_ID_EXACT_MATCH:
    case BCMFP_STAGE_ID_FLOW_TRACKER:
        if (fid == stage->tbls.pse_tbl->drop_decision_fid) {
            num_cont = 1;
            cont_id[0] = 112;
            cont_width = 4;
        } else {
            SHR_EXIT();
        }
        break;
    case BCMFP_STAGE_ID_EGRESS:
        if (fid == stage->tbls.pse_tbl->drop_decision_fid) {
            num_cont = 1;
            cont_id[0] = 80;
            cont_width = 4;
        } else {
            SHR_EXIT();
        }
        break;
    case BCMFP_STAGE_ID_LOOKUP:
        if (fid == stage->tbls.pse_tbl->drop_decision_fid) {
            num_cont = 1;
            cont_id[0] = 72;
            cont_width = 4;
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

    fid = stage->tbls.pse_tbl->qualifiers_fid_info[fid].qualifier;

    BCMFP_KEYGEN_QUAL_CFG_INIT(unit);
    for (cont_idx = 0;
         cont_idx < num_cont;
         cont_idx++) {
        bit_offset = (cont_id[cont_idx] * cont_width);
        BCMFP_KEYGEN_QUAL_CFG_OFFSET_ADD(cont_idx,
                                         BCMFP_KEYGEN_EXT_SECTION_L1E4,
                                         cont_id[cont_idx],
                                         bit_offset,
                                         cont_width);
    }
    BCMFP_KEYGEN_QUAL_CFG_INSERT(unit, db, fid, qual_flags);


    BCMFP_KEYGEN_QUAL_CFG_INIT(unit);
    for (cont_idx = 0;
         cont_idx < num_cont;
         cont_idx++) {
        bit_offset = (cont_id[cont_idx] * cont_width);
        BCMFP_KEYGEN_QUAL_CFG_OFFSET_ADD(cont_idx,
                                         BCMFP_KEYGEN_EXT_SECTION_L1E8,
                                         (cont_id[cont_idx] / 2),
                                         bit_offset,
                                         cont_width);
    }
    BCMFP_KEYGEN_QUAL_CFG_INSERT(unit, db, fid, qual_flags);


exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_fp_common_presel_qual_db_init(int unit,
                                       bcmfp_stage_t *stage)
{

    uint16_t qual_idx = 0;
    uint16_t cont_idx = 0;
    bcmltd_fid_t fid = 0;
    bcmltd_sid_t sid = BCMLTD_SID_INVALID;
    bcmfp_qual_flags_bmp_t qual_flags;
    bcmfp_keygen_qual_cfg_info_db_t *db = NULL;
    BCMFP_KEYGEN_QUAL_CFG_DECL;
    const bcmlrd_table_pcm_info_t *pcm_info = NULL;
    const bcmlrd_field_cont_info_t *qual_cont_info = NULL;
    const bcmlrd_cont_info_t *cont_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (stage->tbls.pse_tbl != NULL ) {
        sid = stage->tbls.pse_tbl->sid;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_pcm_conf_get(unit, sid, &pcm_info));
    if (!pcm_info->field_count || pcm_info->field_info == NULL) {
        /* For any FP stage to operate presel LT is mandatory. */
        LOG_ERROR(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Presel LT is missing for %s "
                 "stage\n"), bcmfp_stage_string(stage->stage_id)));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    BCMFP_ALLOC(stage->kgn_presel_qual_cfg_info_db,
                sizeof(bcmfp_keygen_qual_cfg_info_db_t),
                "bcmfpKeyGenPreselQualCfgInfoDb");
    db = stage->kgn_presel_qual_cfg_info_db;
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
            /*
             * Drop decision container info is initialized in
             * below function. Mactc ID qualifiers also doesnt
             * contain conatinaer info, these will be initialized
             * in bcm56880_a0_fp_presel_matchid_quals_init function.
             */
            if (fid == stage->tbls.pse_tbl->drop_decision_fid) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcm56880_a0_fp_presel_fixed_qual_init(unit, stage, fid));
            }
            continue;
        }
        BCMFP_KEYGEN_QUAL_CFG_INIT(unit);
        for (cont_idx = 0;
            cont_idx < qual_cont_info->count;
            cont_idx++) {
            cont_info = &(qual_cont_info->info[cont_idx]);
            BCMFP_KEYGEN_QUAL_CFG_OFFSET_ADD(cont_idx,
                               BCMFP_KEYGEN_EXT_SECTION_L1E4,
                               cont_info->cont_id,
                               cont_info->bit_offset,
                               cont_info->width);
        }
        fid = pcm_info->field_info[qual_idx].id;
        fid = stage->tbls.pse_tbl->qualifiers_fid_info[fid].qualifier;
        BCMFP_KEYGEN_QUAL_CFG_INSERT(unit, db, fid, qual_flags);
    }

    for (qual_idx = 0; qual_idx < pcm_info->field_count; qual_idx++) {
        qual_cont_info = pcm_info->field_info[qual_idx].cont_info;
        if (qual_cont_info == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,
                        "%s stage presel doest not support %s qualifier\n"),
                        bcmfp_stage_string(stage->stage_id),
                        pcm_info->field_info[qual_idx].name));
            continue;
        }
        BCMFP_KEYGEN_QUAL_CFG_INIT(unit);
        for (cont_idx = 0;
            cont_idx < qual_cont_info->count;
            cont_idx++) {
            cont_info = &(qual_cont_info->info[cont_idx]);
            BCMFP_KEYGEN_QUAL_CFG_OFFSET_ADD(cont_idx,
                               BCMFP_KEYGEN_EXT_SECTION_L1E8,
                               (cont_info->cont_id / 2),
                               cont_info->bit_offset,
                               cont_info->width);
        }
        fid = pcm_info->field_info[qual_idx].id;
        fid = stage->tbls.pse_tbl->qualifiers_fid_info[fid].qualifier;
        BCMFP_KEYGEN_QUAL_CFG_INSERT(unit, db, fid, qual_flags);
    }

    (void) bcm56880_a0_fp_presel_matchid_quals_init(unit, stage);
exit:
    SHR_FUNC_EXIT();
}
