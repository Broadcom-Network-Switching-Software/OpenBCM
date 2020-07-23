/*! \file bcmfp_keygen_ext_ctrl.c
 *
 * Utility APIs to generate secondary selectors.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "bcmfp_keygen_md.h"

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

STATIC int
bcmfp_keygen_ext_code_part_get(int unit,
                              bcmfp_keygen_md_t *keygen_md,
                              bcmfp_keygen_ext_ctrl_sel_t ctrl_sel,
                              uint8_t ctrl_sel_val,
                              int8_t *part)
{
    uint8_t idx = 0;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(part, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);

    *part = -1;
    for (idx = 0; idx < BCMFP_KEYGEN_EXT_FIELDS_MAX; idx++) {
       finfo = &(keygen_md->finfo[idx]);
       if (finfo->section == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
          break;
       }
       if (finfo->part == -1) {
           continue;
       }
       if (finfo->pri_ctrl_sel == ctrl_sel &&
           finfo->pri_ctrl_sel_val == ctrl_sel_val) {
           *part = finfo->part;
           break;
       }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_cfg_get(int unit,
                         bcmfp_keygen_md_t *keygen_md,
                         uint32_t ext_id,
                         bcmfp_keygen_ext_cfg_t **ext_cfg)
{
    uint8_t level = 0;
    uint16_t ext_idx = 0;
    bcmfp_keygen_ext_cfg_t *temp_ext_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(ext_cfg, SHR_E_PARAM);

    *ext_cfg = NULL;
    /* Get the level of the extractor to be found. */
    level = BCMFP_KEYGEN_EXT_ID_LEVEL_GET(ext_id);
    /* Go through all extractors in that level. */
    for (ext_idx = 0;
        ext_idx < keygen_md->ext_cfg_db->conf_size[level];
        ext_idx++) {
        temp_ext_cfg =
            keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
        /* Stop if extractor configuration is found. */
        if (ext_id == temp_ext_cfg->ext_id) {
            *ext_cfg = temp_ext_cfg;
            break;
        }
    }

    if (*ext_cfg == NULL) {
       LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                 "Extractor configuration for extractor ID:%x "
                 "is not found.\n"), ext_id));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_codes_init(int unit, bcmfp_keygen_ext_sel_t *ext_codes)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ext_codes, SHR_E_PARAM);

    sal_memset(ext_codes, BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE, sizeof(bcmfp_keygen_ext_sel_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_cfg_copy_childtoparent(int unit,
                                        bcmfp_keygen_cfg_t *keygen_cfg,
                                        bcmfp_keygen_md_t *keygen_md)
{
    bcmfp_keygen_ext_cfg_t *parent_ext_cfg = NULL;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    uint8_t level = 0;
    uint16_t ext_idx = 0;
    uint16_t parent_finfo_count = 0;
    uint16_t idx = 0;
    bcmfp_keygen_ext_field_info_t **parent_finfo_db_arr = NULL;

    SHR_FUNC_ENTER(unit);

    BCMFP_ALLOC(parent_finfo_db_arr,
                (sizeof(bcmfp_keygen_ext_field_info_t*) *
                 BCMFP_KEYGEN_EXT_FIELDS_MAX),
                "bcmfpKeyGenTempParentFinfoDBArr");

    for (level = 1; level < (keygen_md->num_ext_levels + 1); level++) {
        parent_finfo_count = 0;
        /*
         * First loop through the ext's which have the PUSH_TO_PARENT_EXT
         * attr set and make a note of parent_ext->finfo's
         */
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[level];
            ext_idx++) {
            ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                   BCMFP_KEYGEN_EXT_ATTR_PUSH_TO_PARENT_EXT)) {
                SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_keygen_ext_cfg_get(unit,
                                          keygen_md,
                                          ext_cfg->parent_ext_id,
                                          &parent_ext_cfg));
                if (parent_ext_cfg->in_use == TRUE) {
                    parent_finfo_db_arr[parent_finfo_count] =
                                        parent_ext_cfg->finfo;
                    parent_finfo_count++;
                }
            }
        }
        /*
         * Now copy the finfo's in parent_finfo_db_arr to parent_ext_cfg->finfo
         * in order. Its important to maintain the order in which finfo's
         * are copied to the parent_ext_cfg
         */
        idx = 0;
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[level];
            ext_idx++) {
            ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                   BCMFP_KEYGEN_EXT_ATTR_PUSH_TO_PARENT_EXT)) {
                SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_keygen_ext_cfg_get(unit,
                                          keygen_md,
                                          ext_cfg->parent_ext_id,
                                          &parent_ext_cfg));
                parent_ext_cfg->in_use = ext_cfg->in_use;
                if (ext_cfg->in_use == TRUE) {
                    /*
                     * Return error for this case where garbage finfo
                     * gets copied to parent_ext_cfg
                     */
                    if (idx >= parent_finfo_count) {
                        SHR_ERR_EXIT(SHR_E_INTERNAL);
                    }
                    parent_ext_cfg->finfo = parent_finfo_db_arr[idx];
                    idx++;
                }
            }
        }

    }
exit:
    SHR_FREE(parent_finfo_db_arr);
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_codes_create(int unit,
                             bcmfp_keygen_cfg_t *keygen_cfg,
                             bcmfp_keygen_md_t *keygen_md,
                             bcmfp_keygen_oper_t *keygen_oper)
{
    uint8_t part = 0;
    uint8_t level = 0;
    uint16_t idx = 0;
    uint16_t pmux_idx = 0;
    uint16_t ext_idx = 0;
    uint16_t num_ext_codes = 0;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmfp_keygen_ext_sel_t *ext_codes = NULL;
    bcmfp_keygen_ext_ctrl_sel_info_t *ext_ctrl_info = NULL;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_oper, SHR_E_PARAM);

    for (level = 1; level < (keygen_md->num_ext_levels + 1); level++) {
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[level];
            ext_idx++) {
            ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                   BCMFP_KEYGEN_EXT_ATTR_PASS_THRU)) {
                continue;
            }
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                continue;
            }
            if (ext_cfg->in_use == TRUE) {
                num_ext_codes++;
            }
        }
    }

    for (part = 0; part < keygen_cfg->num_parts; part++) {
        ext_codes = &(keygen_md->ext_codes[part]);
        if (ext_codes->aux_tag_a_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->aux_tag_b_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->aux_tag_c_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->aux_tag_d_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->ttl_fn_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->tcp_fn_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->tos_fn_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->class_id_cont_a_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->class_id_cont_b_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->class_id_cont_c_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->class_id_cont_d_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->src_dest_cont_0_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->src_dest_cont_1_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->src_cont_a_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->src_cont_b_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->dst_cont_a_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->dst_cont_b_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->ipbm_present != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->normalize_l3_l4_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->normalize_mac_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->udf_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        if (ext_codes->alt_ttl_fn_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            num_ext_codes++;
        }
        for (pmux_idx = 0; pmux_idx < 15; pmux_idx++) {
            if (ext_codes->pmux_sel[pmux_idx] == 1) {
                num_ext_codes++;
            }
        }
    }

    SHR_IF_ERR_EXIT((num_ext_codes == 0) ? SHR_E_INTERNAL : SHR_E_NONE);

    keygen_oper->ext_ctrl_sel_info_count = num_ext_codes;
    BCMFP_ALLOC(keygen_oper->ext_ctrl_sel_info,
                      sizeof(bcmfp_keygen_ext_ctrl_sel_info_t) * num_ext_codes,
                      "bcmfpKeyGenExtCtrlSelInfo");
    ext_ctrl_info = keygen_oper->ext_ctrl_sel_info;
    for (level = 1; level < (keygen_md->num_ext_levels + 1); level++) {
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[level];
            ext_idx++) {
            ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                   BCMFP_KEYGEN_EXT_ATTR_PASS_THRU)) {
                continue;
            }
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                continue;
            }
            if (ext_cfg->in_use == TRUE) {
                ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                finfo = ext_cfg->finfo;
                ext_ctrl_info[idx].ctrl_sel_val = finfo->sec_val;
                ext_ctrl_info[idx].ext_num =
                    BCMFP_KEYGEN_EXT_ID_NUM_GET(ext_cfg->ext_id);
                ext_ctrl_info[idx].part =
                    BCMFP_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
                ext_ctrl_info[idx].level =
                    BCMFP_KEYGEN_EXT_ID_LEVEL_GET(ext_cfg->ext_id);
                ext_ctrl_info[idx].gran =
                    BCMFP_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
                idx++;
            }
        }
    }

    for (part = 0; part < keygen_cfg->num_parts; part++) {
        ext_codes = &(keygen_md->ext_codes[part]);
        if (ext_codes->aux_tag_a_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->aux_tag_a_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->aux_tag_b_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->aux_tag_b_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->aux_tag_c_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->aux_tag_c_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->aux_tag_d_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->aux_tag_d_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->ttl_fn_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_TTL_FN;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->ttl_fn_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->tcp_fn_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_TCP_FN;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->tcp_fn_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->tos_fn_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_TOS_FN;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->tos_fn_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->class_id_cont_a_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_A;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->class_id_cont_a_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->class_id_cont_b_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_B;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->class_id_cont_b_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->class_id_cont_c_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_C;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->class_id_cont_c_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->class_id_cont_d_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_D;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->class_id_cont_d_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->src_dest_cont_0_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_0;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->src_dest_cont_0_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->src_dest_cont_1_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_1;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->src_dest_cont_1_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->src_cont_a_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_A;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->src_cont_a_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->src_cont_b_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_B;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->src_cont_b_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->dst_cont_a_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_DST_A;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->dst_cont_a_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->dst_cont_b_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_DST_B;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->dst_cont_b_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->ipbm_present != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->ipbm_source;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->normalize_l3_l4_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_L3_L4_NORM;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->normalize_l3_l4_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->normalize_mac_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_MAC_NORM;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->normalize_mac_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (ext_codes->udf_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_UDF;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->udf_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        for (pmux_idx = 0; pmux_idx < 15; pmux_idx++) {
            if (ext_codes->pmux_sel[pmux_idx] == 1) {
                ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_PMUX;
                ext_ctrl_info[idx].ctrl_sel_val = pmux_idx;
                ext_ctrl_info[idx].part = part;
                idx++;
            }
        }
        if (ext_codes->alt_ttl_fn_sel != BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
            ext_ctrl_info[idx].ctrl_sel = BCMFP_KEYGEN_EXT_CTRL_SEL_ALT_TTL_FN;
            ext_ctrl_info[idx].ctrl_sel_val = ext_codes->alt_ttl_fn_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
    }

    SHR_IF_ERR_EXIT((idx != num_ext_codes) ? SHR_E_INTERNAL : SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_codes_validate(int unit,
                               uint8_t part,
                               bcmfp_keygen_cfg_t *keygen_cfg,
                               bcmfp_keygen_md_t *keygen_md)
{
    uint8_t idx = 0;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;
    bcmfp_keygen_ext_ctrl_sel_info_t *keygen_ext_code = NULL;
    bcmfp_keygen_ext_sel_t *grp_ext_code = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_keygen_ext_codes_init(unit, &(keygen_md->ext_codes[part])));
    for (idx = 0; idx < keygen_cfg->ext_ctrl_sel_info_count; idx++) {
        keygen_ext_code = &(keygen_cfg->ext_ctrl_sel_info[idx]);
        grp_ext_code = &(keygen_md->ext_codes[keygen_ext_code->part]);
        if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A) {
            grp_ext_code->aux_tag_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B) {
            grp_ext_code->aux_tag_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C) {
            grp_ext_code->aux_tag_c_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D) {
            grp_ext_code->aux_tag_d_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_TCP_FN) {
            grp_ext_code->tcp_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_TOS_FN) {
            grp_ext_code->tos_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_TTL_FN) {
            grp_ext_code->ttl_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_A) {
            grp_ext_code->class_id_cont_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_B) {
            grp_ext_code->class_id_cont_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_C) {
            grp_ext_code->class_id_cont_c_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_D) {
            grp_ext_code->class_id_cont_d_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_A) {
            grp_ext_code->src_cont_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_B) {
            grp_ext_code->src_cont_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_DST_A) {
            grp_ext_code->dst_cont_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_DST_B) {
            grp_ext_code->dst_cont_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_0) {
            grp_ext_code->src_dest_cont_0_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_1) {
            grp_ext_code->src_dest_cont_1_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT) {
            grp_ext_code->ipbm_present = 1;
            grp_ext_code->ipbm_source = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_L3_L4_NORM) {
            grp_ext_code->normalize_l3_l4_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_MAC_NORM) {
            grp_ext_code->normalize_mac_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_UDF) {
            grp_ext_code->udf_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_ALT_TTL_FN) {
            grp_ext_code->alt_ttl_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else {
            continue;
        }
    }
    for (idx = 0; idx < BCMFP_KEYGEN_EXT_FIELDS_MAX; idx++) {
       finfo = &(keygen_md->finfo[idx]);
       if (finfo->section == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
          break;
       }
       if (finfo->part != part) {
           continue;
       }
       switch (finfo->pri_ctrl_sel) {
           case BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A:
               if (keygen_md->ext_codes[part].aux_tag_a_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].aux_tag_a_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].aux_tag_a_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B:
               if (keygen_md->ext_codes[part].aux_tag_b_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].aux_tag_b_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].aux_tag_b_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C:
               if (keygen_md->ext_codes[part].aux_tag_c_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].aux_tag_c_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].aux_tag_c_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D:
               if (keygen_md->ext_codes[part].aux_tag_d_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].aux_tag_d_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].aux_tag_d_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_A:
               if (keygen_md->ext_codes[part].class_id_cont_a_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].class_id_cont_a_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].class_id_cont_a_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_B:
               if (keygen_md->ext_codes[part].class_id_cont_b_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].class_id_cont_b_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].class_id_cont_b_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_C:
               if (keygen_md->ext_codes[part].class_id_cont_c_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].class_id_cont_c_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].class_id_cont_c_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_D:
               if (keygen_md->ext_codes[part].class_id_cont_d_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].class_id_cont_d_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].class_id_cont_d_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_TCP_FN:
               if (keygen_md->ext_codes[part].tcp_fn_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].tcp_fn_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].tcp_fn_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_TTL_FN:
               if (keygen_md->ext_codes[part].ttl_fn_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].ttl_fn_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].ttl_fn_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_TOS_FN:
               if (keygen_md->ext_codes[part].tos_fn_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].tos_fn_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].tos_fn_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_A:
               if (keygen_md->ext_codes[part].src_cont_a_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].src_cont_a_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].src_cont_a_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_B:
               if (keygen_md->ext_codes[part].src_cont_b_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].src_cont_b_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].src_cont_b_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_DST_A:
               if (keygen_md->ext_codes[part].dst_cont_a_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].dst_cont_a_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].dst_cont_a_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_DST_B:
               if (keygen_md->ext_codes[part].dst_cont_b_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].dst_cont_b_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].dst_cont_b_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_0:
               if (keygen_md->ext_codes[part].src_dest_cont_0_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].src_dest_cont_0_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].src_dest_cont_0_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_1:
               if (keygen_md->ext_codes[part].src_dest_cont_1_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].src_dest_cont_1_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].src_dest_cont_1_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT:
               if (keygen_md->ext_codes[0].ipbm_source ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[0].ipbm_source =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[0].ipbm_source !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_L3_L4_NORM:
               if (keygen_md->ext_codes[part].normalize_l3_l4_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].normalize_l3_l4_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].normalize_l3_l4_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_MAC_NORM:
               if (keygen_md->ext_codes[part].normalize_mac_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].normalize_mac_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].normalize_mac_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_UDF:
               if (keygen_md->ext_codes[part].udf_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].udf_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].udf_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_ALT_TTL_FN:
               if (keygen_md->ext_codes[part].alt_ttl_fn_sel ==
                   BCMFP_KEYGEN_EXT_SELCODE_DONT_CARE) {
                   keygen_md->ext_codes[part].alt_ttl_fn_sel =
                                  finfo->pri_ctrl_sel_val;
               } else if (keygen_md->ext_codes[part].alt_ttl_fn_sel !=
                          finfo->pri_ctrl_sel_val) {
                   SHR_ERR_EXIT(SHR_E_RESOURCE);
               }
               break;
           case BCMFP_KEYGEN_EXT_CTRL_SEL_DISABLE:
               break;
           default:
               SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
       }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_part_get(int unit,
                         bcmfp_keygen_cfg_t *keygen_cfg,
                         bcmfp_keygen_md_t *keygen_md,
                         bcmfp_keygen_ext_field_info_t *finfo)
{
    int8_t part = -1;
    uint8_t idx = 0;
    uint8_t found_untried_part = FALSE;
    uint16_t bits_extracted = 0;
    bcmfp_keygen_ext_field_info_t *sticky_finfo = NULL;
    bcmfp_keygen_ext_section_t section;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(finfo, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    section = finfo->section;

    /* To check if finfo->section is included in the part which gets selected,
     * keygen_md->sections[part][level0] is checked.
     * Since part is allocated for level 0 finfo's,
     * the check below is done for level 0
     */

    if (keygen_cfg->flags & BCMFP_KEYGEN_CFG_FLAGS_SERIAL_MUX_ALLOC) {
        for (idx = 0; idx < keygen_cfg->num_parts; idx++) {
            if (!(keygen_md->tried_part_bmp[0] & (1 << idx)) &&
                (section == keygen_md->sections[idx][0][section])) {
                part = idx;
                break;
            }
        }
    } else {
        if (finfo->pri_ctrl_sel != BCMFP_KEYGEN_EXT_CTRL_SEL_DISABLE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_keygen_ext_code_part_get(unit, keygen_md,
                                               finfo->pri_ctrl_sel,
                                               finfo->pri_ctrl_sel_val,
                                               &part));
        }

        if (part != -1 && !(keygen_md->tried_part_bmp[0] & (1 << part)) &&
            (section == keygen_md->sections[part][0][section])) {
            keygen_md->tried_part_bmp[0] |= (1 << part);
            finfo->part = part;
            SHR_EXIT();
        }

        for (idx = 0; idx < keygen_cfg->num_parts; idx++) {
            if (keygen_md->tried_part_bmp[0] & (1 << idx) ||
               (section != keygen_md->sections[idx][0][section])) {
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
      finfo->part = -1;
    }

exit:
    sticky_finfo = finfo->sticky;
    while (sticky_finfo != NULL) {
        sticky_finfo->part = finfo->part;
        sticky_finfo = sticky_finfo->sticky;
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_part_incr(int unit,
                          bcmfp_keygen_cfg_t *keygen_cfg,
                          bcmfp_keygen_md_t *keygen_md,
                          bcmfp_keygen_ext_section_t section)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    if (keygen_md->next_part_id[0] == (keygen_cfg->num_parts - 1)) {
        keygen_md->next_part_id[0] = 0;
    } else {
        if (keygen_md->tried_part_bmp[0] &
           (1 << keygen_md->next_part_id[0])) {
            keygen_md->next_part_id[0]++;
        }
    }

exit:
    SHR_FUNC_EXIT();

}
