/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/esw/keygen_md.h>
#include <stdio.h>

int
bcmi_keygen_ext_codes_regenerate(int unit,
                                 uint8 num_parts,
                                 bcmi_keygen_cfg_t *keygen_cfg,
                                 bcmi_keygen_oper_t *keygen_oper)
{
    uint8 part = 0;
    bcmi_keygen_qual_offset_info_t *qual_offset_info = NULL;
    bcmi_keygen_md_t *keygen_md = NULL;
    bcmi_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmi_keygen_ext_cfg_t *ext_cfg1 = NULL;
    bcmi_keygen_ext_cfg_t *ext_cfg2 = NULL;
    uint16 ext_idx = 0;
    bcmi_keygen_qual_offset_t *offset_arr = NULL;
    int g = 0, p = 0, l = 0;
    int g1 = 0, g2 = 0;
    uint16 ext_num = 0;
    int level = 0;
    uint16 idx1 = 0, idx2 = 0;
    uint16 ext_idx1 = 0, ext_idx2 = 0;
    bcmi_keygen_ext_field_info_t *finfo = NULL;
    bcmi_keygen_qual_flags_bmp_t *qual_flags = NULL;
    bcmi_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmi_keygen_qual_cfg_info_t **qual_cfg_info = NULL;
    bcm_field_qualify_t qid = 0;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_oper, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    if (keygen_cfg->ext_ctrl_sel_info == NULL ||
        keygen_oper->qual_offset_info == NULL) {
        BCMI_KEYGEN_EXIT();
    }

    BCMI_KEYGEN_ALLOC(keygen_md,
                      sizeof(bcmi_keygen_md_t),
                      "bcmiKeygenMd");

    BCMI_KEYGEN_IF_ERR_EXIT
        (bcmi_keygen_ext_cfg_alloc(unit, keygen_cfg, keygen_md));

    keygen_cfg->prev_qual_info_arr = keygen_cfg->qual_info_arr;
    keygen_cfg->prev_qual_info_count =
	                       keygen_cfg->qual_info_count;

    BCMI_KEYGEN_IF_ERR_EXIT
        (bcmi_keygen_acl_attrs_update(unit, keygen_cfg, keygen_md));

    for (part = 0; part < keygen_cfg->num_parts; part++) {
        BCMI_KEYGEN_IF_ERR_EXIT
             (bcmi_keygen_ext_codes_validate(unit,
                                             part,
                                             keygen_cfg,
                                             keygen_md));
    }

    /* Fill the extractors section values in all levels and all parts. */
    for (idx1 = 0 ; idx1 < keygen_cfg->ext_ctrl_sel_info_count; idx1++) {
        if (keygen_cfg->ext_ctrl_sel_info[idx1].ctrl_sel !=
            BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER) {
            continue;
        }
        for (level = 1;
             level <= keygen_md->num_ext_levels;
             level++) {
            for (ext_idx = 0;
                ext_idx < keygen_md->ext_cfg_db->conf_size[level];
                ext_idx++) {
                ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] +
                           ext_idx;
                g = BCMI_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
                p = BCMI_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
                l = BCMI_KEYGEN_EXT_ID_LEVEL_GET(ext_cfg->ext_id);
                ext_num = BCMI_KEYGEN_EXT_ID_NUM_GET(ext_cfg->ext_id);
                if (g == keygen_cfg->ext_ctrl_sel_info[idx1].gran &&
                    p == keygen_cfg->ext_ctrl_sel_info[idx1].part &&
                    l == keygen_cfg->ext_ctrl_sel_info[idx1].level &&
                    ext_num == keygen_cfg->ext_ctrl_sel_info[idx1].ext_num) {
                    BCMI_KEYGEN_ALLOC(finfo,
                                      sizeof(bcmi_keygen_ext_field_info_t),
                                      "bcmiKeygenFinfo");
                    finfo->sec_val =
                        keygen_cfg->ext_ctrl_sel_info[idx1].ctrl_sel_val;
                    finfo->ext_cfg = ext_cfg;
                    ext_cfg->finfo = finfo;
                    finfo = NULL;
                }
            }
        }
    }

    /* Rule out unused extractors in final level. */
    level = keygen_md->num_ext_levels;
    qual_cfg_info = keygen_cfg->qual_cfg_info_db->qual_cfg_info;
    for (part = 0; part < num_parts; part++) {
        qual_offset_info = &(keygen_oper->qual_offset_info[part]);
        for (idx1 = 0; idx1 < qual_offset_info->size; idx1++) {
            qid = qual_offset_info->qid_arr[idx1];
            qual_cfg = qual_cfg_info[qid]->qual_cfg_arr;
            qual_flags = &(qual_cfg->qual_flags);
            if (SHR_BITGET(qual_flags->w, BCMI_KEYGEN_QUAL_FLAGS_PMUX)) {
                continue;
            }
            for (ext_idx = 0;
                 ext_idx < keygen_md->ext_cfg_db->conf_size[level];
                 ext_idx++) {
                 ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] +
                            ext_idx;
                 p = BCMI_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
                 if (p != part) {
                     continue;
                 }
                 if (ext_cfg->finfo == NULL) {
                     continue;
                 }
                 _func_rv = bcmi_keygen_ext_validate(unit, p, keygen_md, ext_cfg);
                 if (BCMI_KEYGEN_FUNC_VAL_IS(BCM_E_CONFIG)) {
                     continue;
                 }
                 BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(_func_rv);
                 offset_arr = &(qual_offset_info->offset_arr[idx1]);
                 for (idx2 = 0; idx2 < BCMI_KEYGEN_QUAL_OFFSETS_MAX; idx2++) {
                     if (offset_arr->width[idx2] == 0) {
                         continue;
                     }
                     if (offset_arr->offset[idx2] >= ext_cfg->offset &&
                         offset_arr->offset[idx2] < (ext_cfg->offset + g)) {
                         ext_cfg->in_use = TRUE;
                     }
                 }
            }
        }
    }

    /*
     * Nothing to do if all first level extractors are directly connected
     * to the final key.
     */
    if (keygen_md->num_ext_levels == 1) {
        BCMI_KEYGEN_IF_ERR_EXIT
            (bcmi_keygen_ext_codes_create(unit,
                                          keygen_cfg,
                                          keygen_md,
                                          keygen_oper));
        BCMI_KEYGEN_EXIT();
    }

    /* Rule out unused extractors in all other levels till the first level. */
    for (level = keygen_md->num_ext_levels; level >= 1; level--) {
        for (ext_idx1 = 0;
            ext_idx1 < keygen_md->ext_cfg_db->conf_size[level];
            ext_idx1++) {
            ext_cfg1 =  keygen_md->ext_cfg_db->ext_cfg[level] +
                        ext_idx1;

            if (ext_cfg1->in_use == FALSE) {
                continue;
            }

            g1 = BCMI_KEYGEN_EXT_ID_GRAN_GET(ext_cfg1->ext_id);
            for (ext_idx2 = 0;
                ext_idx2 < keygen_md->ext_cfg_db->conf_size[level-1];
                ext_idx2++) {
                ext_cfg2 =  keygen_md->ext_cfg_db->ext_cfg[level-1] +
                            ext_idx2;

                if (ext_cfg1->in_sec != ext_cfg2->out_sec) {
                    continue;
                }
                if (ext_cfg2->finfo == NULL) {
                    continue;
                }

                g2 = BCMI_KEYGEN_EXT_ID_GRAN_GET(ext_cfg2->ext_id);

                finfo = ext_cfg1->finfo;

                if (((finfo->sec_val * g1) >= ext_cfg2->offset) &&
                    (finfo->sec_val * g1) < (ext_cfg2->offset + g2)) {
                    ext_cfg2->in_use = TRUE;
                }
            }
        }
    }

    /* Generate the actual extractors in use in keygen_oper */
    BCMI_KEYGEN_IF_ERR_EXIT
        (bcmi_keygen_ext_codes_create(unit,
                                      keygen_cfg,
                                      keygen_md,
                                      keygen_oper));
exit:
    if (keygen_md != NULL) {
        if (keygen_md->ext_cfg_db != NULL) {
            for (level = 1;
                level <= keygen_md->num_ext_levels;
                level++) {
                for (ext_idx1 = 0;
                    ext_idx1 < keygen_md->ext_cfg_db->conf_size[level];
                    ext_idx1++) {
                    ext_cfg1 =  keygen_md->ext_cfg_db->ext_cfg[level] +
                                ext_idx1;
                    if (ext_cfg1->finfo != NULL) {
                        BCMI_KEYGEN_FREE(ext_cfg1->finfo);
                        ext_cfg1->finfo = NULL;
                    }
                }
            }
            bcmi_keygen_ext_cfg_db_free(unit, keygen_md->ext_cfg_db);
        }
    }
    BCMI_KEYGEN_FREE(keygen_md);
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_ext_code_part_get(int unit,
                              bcmi_keygen_md_t *keygen_md,
                              bcmi_keygen_ext_ctrl_sel_t ctrl_sel,
                              uint8 ctrl_sel_val,
                              int8 *part)
{
    uint8 idx = 0;
    bcmi_keygen_ext_field_info_t *finfo = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(part, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);

    *part = -1;
    for (idx = 0; idx < BCMI_KEYGEN_EXT_FIELDS_MAX; idx++) {
       finfo = &(keygen_md->finfo[idx]);
       if (finfo->section == BCMI_KEYGEN_EXT_SECTION_DISABLE) {
          break;
       }
       if (finfo->part == -1) {
           continue;
       }
       if (finfo->pri_ctrl_sel[0] == ctrl_sel &&
           finfo->pri_ctrl_sel_val[0] == ctrl_sel_val) {
           *part = finfo->part;
           break;
       }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_codes_init(int unit, bcmi_keygen_ext_sel_t *ext_codes)
{
    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(ext_codes, BCM_E_PARAM);

    sal_memset(ext_codes, BCMI_KEYGEN_EXT_SELCODE_DONT_CARE, sizeof(bcmi_keygen_ext_sel_t));

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_codes_create(int unit,
                             bcmi_keygen_cfg_t *keygen_cfg,
                             bcmi_keygen_md_t *keygen_md,
                             bcmi_keygen_oper_t *keygen_oper)
{
    uint8 part = 0;
    uint8 level = 0;
    uint16 idx = 0;
    uint16 pmux_idx = 0;
    uint16 ext_idx = 0;
    uint16 num_ext_codes = 0;
    bcmi_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmi_keygen_ext_sel_t *ext_codes = NULL;
    bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_info = NULL;
    bcmi_keygen_ext_field_info_t *finfo = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_oper, BCM_E_PARAM);

    for (level = 1; level < (keygen_md->num_ext_levels + 1); level++) {
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[level];
            ext_idx++) {
            ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                   BCMI_KEYGEN_EXT_ATTR_PASS_THRU)) {
                continue;
            }
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                continue;
            }
            if (ext_cfg->in_use == TRUE) {
                num_ext_codes++;
            }
        }
    }

    for (part = 0; part < keygen_cfg->num_parts; part++) {
        ext_codes = &(keygen_md->ext_codes[part]);
        if (ext_codes->aux_tag_a_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->aux_tag_b_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->aux_tag_c_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->aux_tag_d_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->ttl_fn_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->tcp_fn_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->tos_fn_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->class_id_cont_a_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->class_id_cont_b_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->class_id_cont_c_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->class_id_cont_d_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->src_dest_cont_0_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->src_dest_cont_1_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->src_cont_a_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->src_cont_b_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->dst_cont_a_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->dst_cont_b_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->ipbm_present != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->normalize_l3_l4_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->normalize_mac_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->udf_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->alt_ttl_fn_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->ltid_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        for (pmux_idx = 0; pmux_idx < 15; pmux_idx++) {
            if (ext_codes->pmux_sel[pmux_idx] == 1) {
                num_ext_codes++;
            }
        }
    }

    if (num_ext_codes == 0) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_NONE);
    }

    keygen_oper->ext_ctrl_sel_info_count = num_ext_codes;
    BCMI_KEYGEN_ALLOC(keygen_oper->ext_ctrl_sel_info,
                      sizeof(bcmi_keygen_ext_ctrl_sel_info_t) * num_ext_codes,
                      "Keygen Extractor codes");
    ext_ctrl_info = keygen_oper->ext_ctrl_sel_info;
    for (level = 1; level < (keygen_md->num_ext_levels + 1); level++) {
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[level];
            ext_idx++) {
            ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                   BCMI_KEYGEN_EXT_ATTR_PASS_THRU)) {
                continue;
            }
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                continue;
            }
            if (ext_cfg->in_use == TRUE) {
                ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                if (SHR_BITGET(ext_cfg->ext_attrs.w,
                        BCMI_KEYGEN_EXT_ATTR_RESERVED_FOR_ALIGNMENT)) {
                    ext_ctrl_info[idx].ctrl_sel_val = 0;
                } else {
                    finfo = ext_cfg->finfo;
                    ext_ctrl_info[idx].ctrl_sel_val = finfo->sec_val;
                }
                ext_ctrl_info[idx].ext_num =
                    BCMI_KEYGEN_EXT_ID_NUM_GET(ext_cfg->ext_id);
                ext_ctrl_info[idx].part =
                    BCMI_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
                ext_ctrl_info[idx].level =
                    BCMI_KEYGEN_EXT_ID_LEVEL_GET(ext_cfg->ext_id);
                ext_ctrl_info[idx].gran =
                    BCMI_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
                idx++;
            }
        }
    }
    for (part = 0; part < keygen_cfg->num_parts; part++) {
        ext_codes = &(keygen_md->ext_codes[part]);
        if (ext_codes->aux_tag_a_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->aux_tag_a_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->aux_tag_b_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->aux_tag_b_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->aux_tag_c_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->aux_tag_c_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->aux_tag_d_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->aux_tag_d_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->ttl_fn_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_TTL_FN;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->ttl_fn_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->tcp_fn_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_TCP_FN;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->tcp_fn_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->tos_fn_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_TOS_FN;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->tos_fn_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->class_id_cont_a_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->class_id_cont_a_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->class_id_cont_b_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->class_id_cont_b_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->class_id_cont_c_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->class_id_cont_c_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->class_id_cont_d_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->class_id_cont_d_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->src_dest_cont_0_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->src_dest_cont_0_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->src_dest_cont_1_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->src_dest_cont_1_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->src_cont_a_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->src_cont_a_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->src_cont_b_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->src_cont_b_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->dst_cont_a_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_DST_A;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->dst_cont_a_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->dst_cont_b_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_DST_B;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->dst_cont_b_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->ipbm_present != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->ipbm_source;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->normalize_l3_l4_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_L3_L4_NORM;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->normalize_l3_l4_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->normalize_mac_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_MAC_NORM;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->normalize_mac_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->udf_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_UDF;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->udf_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->ltid_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_LTID;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->ltid_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        for (pmux_idx = 0; pmux_idx < 15; pmux_idx++) {
            if (ext_codes->pmux_sel[pmux_idx] == 1) {
                ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_PMUX;
                ext_ctrl_info[idx].ctrl_sel_val = pmux_idx;
                ext_ctrl_info[idx].part = part;
                idx++;
            }
        }
        if (ext_codes->alt_ttl_fn_sel != BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_ALT_TTL_FN;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->alt_ttl_fn_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
    }

    BCMI_KEYGEN_IF_ERR_EXIT((idx != num_ext_codes) ? BCM_E_INTERNAL : BCM_E_NONE);

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_codes_validate(int unit,
                               uint8 part,
                               bcmi_keygen_cfg_t *keygen_cfg,
                               bcmi_keygen_md_t *keygen_md)
{
    uint8 idx = 0;
    bcmi_keygen_ext_field_info_t *finfo = NULL;
    bcmi_keygen_ext_ctrl_sel_info_t *keygen_ext_code = NULL;
    bcmi_keygen_ext_sel_t *grp_ext_code = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
        bcmi_keygen_ext_codes_init(unit, &(keygen_md->ext_codes[part])));
    for (idx = 0; idx < keygen_cfg->ext_ctrl_sel_info_count; idx++) {
        keygen_ext_code = &(keygen_cfg->ext_ctrl_sel_info[idx]);
        grp_ext_code = &(keygen_md->ext_codes[keygen_ext_code->part]);
        if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A) {
            grp_ext_code->aux_tag_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B) {
            grp_ext_code->aux_tag_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C) {
            grp_ext_code->aux_tag_c_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D) {
            grp_ext_code->aux_tag_d_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_TCP_FN) {
            grp_ext_code->tcp_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_TOS_FN) {
            grp_ext_code->tos_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_TTL_FN) {
            grp_ext_code->ttl_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A) {
            grp_ext_code->class_id_cont_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B) {
            grp_ext_code->class_id_cont_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C) {
            grp_ext_code->class_id_cont_c_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D) {
            grp_ext_code->class_id_cont_d_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A) {
            grp_ext_code->src_cont_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B) {
            grp_ext_code->src_cont_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_DST_A) {
            grp_ext_code->dst_cont_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_DST_B) {
            grp_ext_code->dst_cont_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0) {
            grp_ext_code->src_dest_cont_0_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1) {
            grp_ext_code->src_dest_cont_1_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT) {
            grp_ext_code->ipbm_present = 1;
            grp_ext_code->ipbm_source = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_L3_L4_NORM) {
            grp_ext_code->normalize_l3_l4_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_MAC_NORM) {
            grp_ext_code->normalize_mac_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_UDF) {
            grp_ext_code->udf_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_ALT_TTL_FN) {
            grp_ext_code->alt_ttl_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_LTID) {
            grp_ext_code->ltid_sel = keygen_ext_code->ctrl_sel_val;
        } else {
            continue;
        }
    }
    for (idx = 0; idx < BCMI_KEYGEN_EXT_FIELDS_MAX; idx++) {
       finfo = &(keygen_md->finfo[idx]);
       if (finfo->section == BCMI_KEYGEN_EXT_SECTION_DISABLE) {
          break;
       }
       if (finfo->part != part) {
           continue;
       }
       switch (finfo->pri_ctrl_sel[0]) {
           case BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A:
               if (keygen_md->ext_codes[part].aux_tag_a_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].aux_tag_a_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].aux_tag_a_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B:
               if (keygen_md->ext_codes[part].aux_tag_b_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].aux_tag_b_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].aux_tag_b_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C:
               if (keygen_md->ext_codes[part].aux_tag_c_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].aux_tag_c_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].aux_tag_c_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D:
               if (keygen_md->ext_codes[part].aux_tag_d_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].aux_tag_d_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].aux_tag_d_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A:
               if (keygen_md->ext_codes[part].class_id_cont_a_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].class_id_cont_a_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].class_id_cont_a_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B:
               if (keygen_md->ext_codes[part].class_id_cont_b_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].class_id_cont_b_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].class_id_cont_b_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C:
               if (keygen_md->ext_codes[part].class_id_cont_c_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].class_id_cont_c_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].class_id_cont_c_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D:
               if (keygen_md->ext_codes[part].class_id_cont_d_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].class_id_cont_d_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].class_id_cont_d_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_TCP_FN:
               if (keygen_md->ext_codes[part].tcp_fn_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].tcp_fn_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].tcp_fn_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_TTL_FN:
               if (keygen_md->ext_codes[part].ttl_fn_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].ttl_fn_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].ttl_fn_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_TOS_FN:
               if (keygen_md->ext_codes[part].tos_fn_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].tos_fn_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].tos_fn_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A:
               if (keygen_md->ext_codes[part].src_cont_a_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].src_cont_a_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].src_cont_a_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B:
               if (keygen_md->ext_codes[part].src_cont_b_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].src_cont_b_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].src_cont_b_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_DST_A:
               if (keygen_md->ext_codes[part].dst_cont_a_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].dst_cont_a_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].dst_cont_a_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_DST_B:
               if (keygen_md->ext_codes[part].dst_cont_b_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].dst_cont_b_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].dst_cont_b_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0:
               if (keygen_md->ext_codes[part].src_dest_cont_0_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].src_dest_cont_0_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].src_dest_cont_0_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1:
               if (keygen_md->ext_codes[part].src_dest_cont_1_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].src_dest_cont_1_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].src_dest_cont_1_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT:
               if (keygen_md->ext_codes[0].ipbm_source ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[0].ipbm_source =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[0].ipbm_source !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_L3_L4_NORM:
               if (keygen_md->ext_codes[part].normalize_l3_l4_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].normalize_l3_l4_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].normalize_l3_l4_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_MAC_NORM:
               if (keygen_md->ext_codes[part].normalize_mac_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].normalize_mac_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].normalize_mac_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_UDF:
               if (keygen_md->ext_codes[part].udf_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].udf_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].udf_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_ALT_TTL_FN:
               if (keygen_md->ext_codes[part].alt_ttl_fn_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].alt_ttl_fn_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].alt_ttl_fn_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_LTID:
               if (keygen_md->ext_codes[part].ltid_sel ==
                   BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].ltid_sel =
                                  finfo->pri_ctrl_sel_val[0];
               } else if (keygen_md->ext_codes[part].ltid_sel !=
                          finfo->pri_ctrl_sel_val[0]) {
                   BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_RESOURCE);
               }
               break;
           case BCMI_KEYGEN_EXT_CTRL_SEL_DISABLE:
               break;
           default:
               BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
       }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_part_get(int unit,
                         bcmi_keygen_cfg_t *keygen_cfg,
                         bcmi_keygen_md_t *keygen_md,
                         bcmi_keygen_ext_field_info_t *finfo)
{
    int8 part = -1;
    uint8 idx = 0;
    uint8 found_untried_part = FALSE;
    uint16 bits_extracted = 0;
    bcmi_keygen_ext_field_info_t *sticky_finfo = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(finfo, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    if (keygen_cfg->flags & BCMI_KEYGEN_CFG_FLAGS_SERIAL_MUX_ALLOC) {
        for (idx = 0; idx < keygen_cfg->num_parts; idx++) {
            if (!(keygen_md->tried_part_bmp[0] & (1 << idx))) {
                part = idx;
                break;
            }
        }
    } else {
        if (finfo->pri_ctrl_sel[0] != BCMI_KEYGEN_EXT_CTRL_SEL_DISABLE) {
            BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
                (bcmi_keygen_ext_code_part_get(unit, keygen_md,
                                               finfo->pri_ctrl_sel[0],
                                               finfo->pri_ctrl_sel_val[0],
                                               &part));
        }

        if (part != -1 && !(keygen_md->tried_part_bmp[0] & (1 << part))) {
            keygen_md->tried_part_bmp[0] |= (1 << part);
            finfo->part = part;
            BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_NONE);
        }

        for (idx = 0; idx < keygen_cfg->num_parts; idx++) {
            if (keygen_md->tried_part_bmp[0] & (1 << idx)) {
                continue;
            }
            if (!found_untried_part) {
                found_untried_part = TRUE;
                part = idx;
                bits_extracted = keygen_md->bits_extracted[idx][0];
                continue;
            }
            if (bits_extracted > keygen_md->bits_extracted[idx][0]) {
                part = idx;
                bits_extracted = keygen_md->bits_extracted[idx][0];
            }
        }
    }

    if (part != -1) {
        keygen_md->tried_part_bmp[0] |= (1 << part);
        finfo->part = part;
    } else {
        BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
    }

exit:
    sticky_finfo = finfo->sticky;
    while (sticky_finfo != NULL) {
        sticky_finfo->part = finfo->part;
        sticky_finfo = sticky_finfo->sticky;
    }
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_part_incr(int unit,
                          bcmi_keygen_cfg_t *keygen_cfg,
                          bcmi_keygen_md_t *keygen_md,
                          bcmi_keygen_ext_section_t section)
{
    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    if (keygen_md->next_part_id[0] == (keygen_cfg->num_parts - 1)) {
        keygen_md->next_part_id[0] = 0;
    } else {
        if (keygen_md->tried_part_bmp[0] &
           (1 << keygen_md->next_part_id[0])) {
            keygen_md->next_part_id[0]++;
        }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();

}

int
bcmi_keygen_ext_ctrl_sel_validate(int unit,
                               bcmi_keygen_cfg_t *keygen_cfg,
                               bcmi_keygen_md_t *keygen_md)
{
    uint8 idx = 0;
    uint8 qual_cfg_id = 0;
    bcm_field_qualify_t qual_id = bcmFieldQualifyCount;
    bcmi_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmi_keygen_qual_cfg_info_t *qual_cfg_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    sal_memset(keygen_md->pri_ctrl_sel_count, 0,
               (sizeof(uint8) * BCMI_KEYGEN_EXT_CTRL_SEL_COUNT));
    sal_memset(keygen_md->pri_ctrl_sel_values, 0,
               (sizeof(uint32) * BCMI_KEYGEN_EXT_CTRL_SEL_COUNT));
    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        qual_cfg_id = keygen_md->qual_cfg_id_arr[idx];
        qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_cfg_info = keygen_md->qual_cfg_info[qual_id];
        if (qual_cfg_info == NULL) {
            continue;
        }
        qual_cfg = &(qual_cfg_info->qual_cfg_arr[qual_cfg_id]);
        if (SHR_BITGET(qual_cfg->qual_flags.w,
            BCMI_KEYGEN_QUAL_FLAGS_PMUX)) {
            continue;
        }
        if (qual_cfg->pri_ctrl_sel ==
            BCMI_KEYGEN_EXT_CTRL_SEL_DISABLE) {
            continue;
        }
        /*
         * Check whether same primary control sel value was used
         * by given qset.
         */
        if (keygen_md->pri_ctrl_sel_values[qual_cfg->pri_ctrl_sel]
            & (1 << qual_cfg->pri_ctrl_sel_val)) {
            continue;
        }
        keygen_md->pri_ctrl_sel_values[qual_cfg->pri_ctrl_sel] |= (1 << qual_cfg->pri_ctrl_sel_val);
        keygen_md->pri_ctrl_sel_count[qual_cfg->pri_ctrl_sel] += 1;
    }
    for (idx = 0; idx < BCMI_KEYGEN_EXT_CTRL_SEL_COUNT; idx++) {
        if (keygen_md->pri_ctrl_sel_count[idx] > keygen_cfg->num_parts) {
            BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_RESOURCE);
        }
    }
exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

