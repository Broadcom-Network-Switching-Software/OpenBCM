/*! \file bcmfp_keygen_pmux.c
 *
 * Post Mux Qualifier allocation APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "bcmfp_keygen_md.h"

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

STATIC int
bcmfp_keygen_pmux_alloc(int unit,
                       bcmfp_keygen_cfg_t *keygen_cfg,
                       bcmfp_keygen_md_t *keygen_md)
{
    uint8_t idx = 0;
    uint8_t part = 0;
    uint8_t num_parts = 0;
    uint8_t found_part = FALSE;
    uint8_t num_qual_cfg = 0;
    uint16_t qual_cfg_id = 0;
    uint8_t pmux_idx = 0;
    bcmfp_qualifier_t pmux_qual;
    bcmfp_keygen_ext_sel_t *pmux_sel = NULL;
    bcmfp_keygen_qual_flags_bmp_t *qual_flags = NULL;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmfp_keygen_qual_cfg_info_t **qual_cfg_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    num_parts = keygen_cfg->num_parts;
    qual_cfg_info = keygen_cfg->qual_cfg_info_db->qual_cfg_info;
    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        pmux_qual = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_cfg = qual_cfg_info[pmux_qual]->qual_cfg_arr;
        qual_flags = &(qual_cfg->qual_flags);
        if (!SHR_BITGET(qual_flags->w, BCMFP_KEYGEN_QUAL_FLAGS_PMUX)) {
            continue;
        }
        if (qual_cfg->pri_ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_DISABLE) {
            keygen_md->pmux_info[pmux_idx].pmux_qual = pmux_qual;
            keygen_md->pmux_info[pmux_idx].pmux_part = 0;
            keygen_md->pmux_info[pmux_idx].pmux_qual_cfg = qual_cfg;
            pmux_idx++;
            continue;
        }
        if (pmux_sel == NULL) {
            BCMFP_ALLOC(pmux_sel,
                sizeof(bcmfp_keygen_ext_sel_t) * num_parts,
                "bcmfpPostMuxQualSecSel");
            for (part = 0; part < num_parts; part++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_keygen_ext_codes_init(unit,
                                  &(pmux_sel[part])));
            }
        }

        found_part = FALSE;
        num_qual_cfg = qual_cfg_info[pmux_qual]->num_qual_cfg;
        for (qual_cfg_id = 0;
             qual_cfg_id < num_qual_cfg;
             qual_cfg_id++) {
             if (SHR_BITGET(qual_cfg[qual_cfg_id].qual_flags.w,
                 BCMFP_KEYGEN_QUAL_FLAGS_NOT_IN_HALF) &&
                 keygen_cfg->mode == BCMFP_KEYGEN_MODE_HALF) {
                 continue;
            }
            switch (qual_cfg[qual_cfg_id].pri_ctrl_sel) {
                case BCMFP_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT:
                    if (pmux_sel[0].ipbm_source ==
                            BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                        pmux_sel[0].ipbm_source =
                            qual_cfg[qual_cfg_id].pri_ctrl_sel_val;
                        part = 0;
                        found_part = TRUE;
                        break;
                    } else if (pmux_sel[0].ipbm_source ==
                               qual_cfg[qual_cfg_id].pri_ctrl_sel_val) {
                        part = 0;
                        found_part = TRUE;
                        break;
                    }
                    break;
                case BCMFP_KEYGEN_EXT_CTRL_SEL_DST_A:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].dst_cont_a_sel ==
                            BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                            pmux_sel[part].dst_cont_a_sel =
                                qual_cfg[qual_cfg_id].pri_ctrl_sel_val;
                            found_part = TRUE;
                            break;
                        } else if (pmux_sel[part].dst_cont_a_sel ==
                            qual_cfg[qual_cfg_id].pri_ctrl_sel_val) {
                            found_part = TRUE;
                            break;
                        }
                    }
                    break;
                case BCMFP_KEYGEN_EXT_CTRL_SEL_DST_B:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].dst_cont_b_sel ==
                            BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                            pmux_sel[part].dst_cont_b_sel =
                                qual_cfg[qual_cfg_id].pri_ctrl_sel_val;
                            found_part = TRUE;
                            break;
                        } else if (pmux_sel[part].dst_cont_b_sel ==
                            qual_cfg[qual_cfg_id].pri_ctrl_sel_val) {
                            found_part = TRUE;
                            break;
                        }
                    }
                    break;
                case BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_0:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].src_dest_cont_0_sel ==
                            BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                            pmux_sel[part].src_dest_cont_0_sel =
                                qual_cfg[qual_cfg_id].pri_ctrl_sel_val;
                            found_part = TRUE;
                            break;
                        } else if (pmux_sel[part].src_dest_cont_0_sel ==
                            qual_cfg[qual_cfg_id].pri_ctrl_sel_val) {
                            found_part = TRUE;
                            break;
                        }
                    }
                    break;
                case BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_1:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].src_dest_cont_1_sel ==
                            BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                            pmux_sel[part].src_dest_cont_1_sel =
                                qual_cfg[qual_cfg_id].pri_ctrl_sel_val;
                            found_part = TRUE;
                            break;
                        } else if (pmux_sel[part].src_dest_cont_1_sel ==
                            qual_cfg[qual_cfg_id].pri_ctrl_sel_val) {
                            found_part = TRUE;
                            break;
                        }
                    }
                    break;
                case BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_C:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].class_id_cont_c_sel ==
                            BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                            pmux_sel[part].class_id_cont_c_sel =
                                qual_cfg[qual_cfg_id].pri_ctrl_sel_val;
                            found_part = TRUE;
                            break;
                        } else if (pmux_sel[part].class_id_cont_c_sel ==
                            qual_cfg[qual_cfg_id].pri_ctrl_sel_val) {
                            found_part = TRUE;
                            break;
                        }
                    }
                    break;
                case BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_D:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].class_id_cont_d_sel ==
                            BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                            pmux_sel[part].class_id_cont_d_sel =
                                qual_cfg[qual_cfg_id].pri_ctrl_sel_val;
                            found_part = TRUE;
                            break;
                        } else if (pmux_sel[part].class_id_cont_d_sel ==
                            qual_cfg[qual_cfg_id].pri_ctrl_sel_val) {
                            found_part = TRUE;
                            break;
                        }
                    }
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            if (found_part == TRUE) {
                break;
            }
        }
        if (found_part == FALSE) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        } else {
            keygen_md->pmux_info[pmux_idx].pmux_qual = pmux_qual;
            keygen_md->pmux_info[pmux_idx].pmux_part = part;
            keygen_md->pmux_info[pmux_idx].pmux_qual_cfg =
                                   &(qual_cfg[qual_cfg_id]);
           pmux_idx++;
       }
    }
exit:
    SHR_FREE(pmux_sel);
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_pmux_control_set(int unit,
                             bcmfp_qualifier_t qual,
                             bcmfp_keygen_md_t *keygen_md,
                             bcmfp_keygen_cfg_t *keygen_cfg)
{
    uint8_t idx = 0;
    uint8_t pmux_num = 0;
    uint8_t pmux_part = 0;
    bcmfp_keygen_ext_sel_t *ext_codes = NULL;
    bcmfp_keygen_pmux_info_t *pmux_info = NULL;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_keygen_pmux_info_get(unit,
                            qual,
                            keygen_md->num_pmux_qual,
                            keygen_md->pmux_info,
                            &pmux_info));

    pmux_part = pmux_info->pmux_part;
    qual_cfg = pmux_info->pmux_qual_cfg;
    ext_codes = &(keygen_md->ext_codes[pmux_part]);
    for (idx = 0; idx < qual_cfg->num_chunks; idx++) {
        pmux_num = qual_cfg->e_params[idx].sec_val;
        switch (qual_cfg->e_params[idx].section) {
            case BCMFP_KEYGEN_EXT_SECTION_IPBM:
                ext_codes->ipbm_source = qual_cfg->pri_ctrl_sel_val;
                ext_codes->ipbm_present = 1;
                break;
            case BCMFP_KEYGEN_EXT_SECTION_DROP:
            case BCMFP_KEYGEN_EXT_SECTION_NAT_NEEDED:
            case BCMFP_KEYGEN_EXT_SECTION_NAT_DST_REALMID:
            case BCMFP_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_HIT:
            case BCMFP_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_HIT:
            case BCMFP_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASS_ID_BITS_0_3:
            case BCMFP_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASS_ID_BITS_4_7:
            case BCMFP_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASS_ID_BITS_8_11:
            case BCMFP_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASS_ID_BITS_0_3:
            case BCMFP_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASS_ID_BITS_4_7:
            case BCMFP_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASS_ID_BITS_8_11:
            case BCMFP_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_0_3:
            case BCMFP_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_4_7:
            case BCMFP_KEYGEN_EXT_SECTION_PKT_RESOLUTION:
            case BCMFP_KEYGEN_EXT_SECTION_INT_PRI:
            case BCMFP_KEYGEN_EXT_SECTION_INT_CN:
            case BCMFP_KEYGEN_EXT_SECTION_IFP_DROP_VECTOR:
            case BCMFP_KEYGEN_EXT_SECTION_MH_OPCODE:
            case BCMFP_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB:
            case BCMFP_KEYGEN_EXT_SECTION_KEY_TYPE:
                keygen_md->ext_codes[pmux_part].pmux_sel[pmux_num] = 1;
                break;
            case BCMFP_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->src_dest_cont_0_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            case BCMFP_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->src_dest_cont_1_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            case BCMFP_KEYGEN_EXT_SECTION_DST_CONTAINER_A:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->dst_cont_a_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            case BCMFP_KEYGEN_EXT_SECTION_DST_CONTAINER_B:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->dst_cont_b_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            case BCMFP_KEYGEN_EXT_SECTION_CLASS_ID_C:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->class_id_cont_c_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            case BCMFP_KEYGEN_EXT_SECTION_CLASS_ID_D:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->class_id_cont_d_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_pmux_info_get(int unit,
                    bcmfp_qualifier_t qual,
                    uint8_t num_pmux_qual,
                    bcmfp_keygen_pmux_info_t *pmux_info_arr,
                    bcmfp_keygen_pmux_info_t **pmux_info)
{
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pmux_info, SHR_E_PARAM);
    SHR_NULL_CHECK(pmux_info_arr, SHR_E_PARAM);

    *pmux_info = NULL;
    for (idx = 0; idx < num_pmux_qual; idx++) {
        if (pmux_info_arr[idx].pmux_qual == qual) {
            *pmux_info = &(pmux_info_arr[idx]);
        }
    }

    if (pmux_info == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_pmux_qual_offset_get(int unit,
                                 bcmfp_qualifier_t qual_id,
                                 bcmfp_keygen_md_t *keygen_md,
                                 bcmfp_keygen_cfg_t *keygen_cfg,
                                 bcmfp_keygen_qual_offset_t *qual_offset)
{
     uint8_t idx;
     bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
     bcmfp_keygen_pmux_info_t *pmux_info = NULL;

     SHR_FUNC_ENTER(unit);

     SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
     SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);
     SHR_NULL_CHECK(qual_offset, SHR_E_PARAM);

     sal_memset(qual_offset, 0, sizeof(bcmfp_keygen_qual_offset_t));

     SHR_IF_ERR_VERBOSE_EXIT
         (bcmfp_keygen_pmux_info_get(unit,
                             qual_id,
                             keygen_md->num_pmux_qual,
                             keygen_md->pmux_info,
                             &pmux_info));
     SHR_IF_ERR_VERBOSE_EXIT
         (bcmfp_keygen_pmux_control_set(unit, qual_id,
                             keygen_md, keygen_cfg));
     qual_cfg = pmux_info->pmux_qual_cfg;
     for (idx = 0; idx < qual_cfg->num_chunks; idx++) {
         qual_offset->offset[idx] = qual_cfg->e_params[idx].offset;
         qual_offset->width[idx] = qual_cfg->e_params[idx].width;
         qual_offset->num_offsets++;
     }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_pmux_process(int unit,
                         bcmfp_keygen_cfg_t *keygen_cfg,
                         bcmfp_keygen_md_t *keygen_md)
{
    uint8_t idx = 0;
    uint8_t num_pmux_qual = 0;
    bcmfp_qualifier_t pmux_qual;
    bcmfp_keygen_qual_flags_bmp_t *qual_flags = NULL;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmfp_keygen_qual_cfg_info_t **qual_cfg_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    qual_cfg_info = keygen_cfg->qual_cfg_info_db->qual_cfg_info;
    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        pmux_qual = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_cfg = qual_cfg_info[pmux_qual]->qual_cfg_arr;
        qual_flags = &(qual_cfg->qual_flags);
        if (!SHR_BITGET(qual_flags->w, BCMFP_KEYGEN_QUAL_FLAGS_PMUX)) {
            continue;
        }
        num_pmux_qual++;
    }

    if (num_pmux_qual == 0) {
        SHR_EXIT();
    }

    BCMFP_ALLOC(keygen_md->pmux_info,
                num_pmux_qual * sizeof(bcmfp_keygen_pmux_info_t),
                "bcmfpGrpPostMuxQualInfo");
    keygen_md->num_pmux_qual = num_pmux_qual;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_keygen_pmux_alloc(unit, keygen_cfg, keygen_md));
exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(keygen_md->pmux_info);
    }
    SHR_FUNC_EXIT();
}
