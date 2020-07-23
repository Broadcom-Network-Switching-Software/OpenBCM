/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/esw/keygen_md.h>
#include <bcm_int/esw/field.h>

STATIC int
bcmi_keygen_pmux_alloc(int unit,
                       bcmi_keygen_cfg_t *keygen_cfg,
                       bcmi_keygen_md_t *keygen_md)
{
    uint8 idx = 0;
    uint8 part = 0;
    uint8 num_parts = 0;
    uint8 found_part = FALSE;
    uint8 num_qual_cfg = 0;
    uint8 qual_cfg_id = 0;
    uint8 pmux_idx = 0;
    bcm_field_qualify_t pmux_qual;
    bcmi_keygen_ext_sel_t *pmux_sel = NULL;
    bcmi_keygen_qual_flags_bmp_t *qual_flags = NULL;
    bcmi_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmi_keygen_qual_cfg_info_t **qual_cfg_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    num_parts = keygen_cfg->num_parts;
    qual_cfg_info = keygen_cfg->qual_cfg_info_db->qual_cfg_info;
    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        pmux_qual = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_cfg = qual_cfg_info[pmux_qual]->qual_cfg_arr;
        qual_flags = &(qual_cfg->qual_flags);
        if (!SHR_BITGET(qual_flags->w, BCMI_KEYGEN_QUAL_FLAGS_PMUX)) {
            continue;
        }
        if (qual_cfg->pri_ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_DISABLE ||
            qual_cfg->pri_ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT) {
            keygen_md->pmux_info[pmux_idx].pmux_qual = pmux_qual;
            keygen_md->pmux_info[pmux_idx].pmux_part = 0;
            keygen_md->pmux_info[pmux_idx].pmux_qual_cfg = qual_cfg;
            pmux_idx++;
            continue;
        }
        if (pmux_sel == NULL) {
            BCMI_KEYGEN_ALLOC(pmux_sel,
                sizeof(bcmi_keygen_ext_sel_t) * num_parts,
                "Post mux qualifiers secondary selectors");
            for (part = 0; part < num_parts; part++) {
                BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
                    (bcmi_keygen_ext_codes_init(unit,
                                  &(pmux_sel[part])));
            }
        }

        found_part = FALSE;
        num_qual_cfg = qual_cfg_info[pmux_qual]->num_qual_cfg;
        for (qual_cfg_id = 0;
             qual_cfg_id < num_qual_cfg;
             qual_cfg_id++) {
             if (SHR_BITGET(qual_cfg[qual_cfg_id].qual_flags.w,
                 BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_SINGLE) &&
                 keygen_cfg->mode == BCMI_KEYGEN_MODE_SINGLE) {
                 continue;
            }
            switch (qual_cfg[qual_cfg_id].pri_ctrl_sel) {
                case BCMI_KEYGEN_EXT_CTRL_SEL_DST_A:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].dst_cont_a_sel ==
                            BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
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
                case BCMI_KEYGEN_EXT_CTRL_SEL_DST_B:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].dst_cont_b_sel ==
                            BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
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
                case BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].src_dest_cont_0_sel ==
                            BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
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
                case BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].src_dest_cont_1_sel ==
                            BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
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
                case BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].class_id_cont_c_sel ==
                            BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
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
                case BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D:
                    for (part = 0; part < num_parts; part++) {
                        if (pmux_sel[part].class_id_cont_d_sel ==
                            BCMI_KEYGEN_EXT_SELCODE_DONT_CARE) {
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
                    BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_INTERNAL);
            }
            if (found_part == TRUE) {
                break;
            }
        }
        if (found_part == FALSE) {
            BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_RESOURCE);
        } else {
            keygen_md->pmux_info[pmux_idx].pmux_qual = pmux_qual;
            keygen_md->pmux_info[pmux_idx].pmux_part = part;
            keygen_md->pmux_info[pmux_idx].pmux_qual_cfg =
                                   &(qual_cfg[qual_cfg_id]);
           pmux_idx++;
       }
    }
exit:
    BCMI_KEYGEN_FREE(pmux_sel);
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_pmux_info_get(int unit,
                    bcm_field_qualify_t qual,
                    uint8 num_pmux_qual,
                    bcmi_keygen_pmux_info_t *pmux_info_arr,
                    bcmi_keygen_pmux_info_t **pmux_info)
{
    uint8 idx = 0;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(pmux_info, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(pmux_info_arr, BCM_E_PARAM);

    *pmux_info = NULL;
    for (idx = 0; idx < num_pmux_qual; idx++) {
        if (pmux_info_arr[idx].pmux_qual == qual) {
            *pmux_info = &(pmux_info_arr[idx]);
        }
    }

    if (pmux_info == NULL) {
        BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_NOT_FOUND);
    }
exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_pmux_control_set(int unit,
                             bcm_field_qualify_t qual,
                             bcmi_keygen_md_t *keygen_md,
                             bcmi_keygen_cfg_t *keygen_cfg)
{
    uint8 idx = 0;
    uint8 pmux_num = 0;
    uint8 pmux_part = 0;
    bcmi_keygen_ext_sel_t *ext_codes = NULL;
    bcmi_keygen_pmux_info_t *pmux_info = NULL;
    bcmi_keygen_qual_cfg_t *qual_cfg = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
        bcmi_keygen_pmux_info_get(unit,
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
            case BCMI_KEYGEN_EXT_SECTION_IPBM:
                ext_codes->ipbm_source = qual_cfg->pri_ctrl_sel_val;
                ext_codes->ipbm_present = 1;
                break;
            case BCMI_KEYGEN_EXT_SECTION_DROP:
            case BCMI_KEYGEN_EXT_SECTION_NAT_NEEDED:
            case BCMI_KEYGEN_EXT_SECTION_NAT_DST_REALMID:
            case BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_HIT:
            case BCMI_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_HIT:
            case BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_0_3:
            case BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_4_7:
            case BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_8_11:
            case BCMI_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_0_3:
            case BCMI_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_4_7:
            case BCMI_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_8_11:
            case BCMI_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_0_3:
            case BCMI_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_4_7:
            case BCMI_KEYGEN_EXT_SECTION_PKT_RESOLUTION:
            case BCMI_KEYGEN_EXT_SECTION_INT_PRI:
            case BCMI_KEYGEN_EXT_SECTION_IFP_DROP_VECTOR:
            case BCMI_KEYGEN_EXT_SECTION_MH_OPCODE:
            case BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB:
                keygen_md->ext_codes[pmux_part].pmux_sel[pmux_num] = 1;
                break;
            case BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->src_dest_cont_0_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            case BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->src_dest_cont_1_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            case BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_A:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->dst_cont_a_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            case BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_B:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->dst_cont_b_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            case BCMI_KEYGEN_EXT_SECTION_CLASS_ID_C:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->class_id_cont_c_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            case BCMI_KEYGEN_EXT_SECTION_CLASS_ID_D:
                ext_codes->pmux_sel[pmux_num] = 1;
                ext_codes->class_id_cont_d_sel = qual_cfg->pri_ctrl_sel_val;
                break;
            case BCMI_KEYGEN_EXT_SECTION_LTID:
                break;
            default:
                BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_INTERNAL);
        }
    }
exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_pmux_qual_offset_get(int unit,
                                 bcm_field_qualify_t qual_id,
                                 bcmi_keygen_md_t *keygen_md,
                                 bcmi_keygen_cfg_t *keygen_cfg,
                                 bcmi_keygen_qual_offset_t *qual_offset)
{
     uint8 idx;
     bcmi_keygen_qual_cfg_t *qual_cfg = NULL;
     bcmi_keygen_pmux_info_t *pmux_info = NULL;

     BCMI_KEYGEN_FUNC_ENTER(unit);

     BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
     BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);
     BCMI_KEYGEN_NULL_CHECK(qual_offset, BCM_E_PARAM);

     sal_memset(qual_offset, 0, sizeof(bcmi_keygen_qual_offset_t));

     BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
         (bcmi_keygen_pmux_info_get(unit,
                             qual_id,
                             keygen_md->num_pmux_qual,
                             keygen_md->pmux_info,
                             &pmux_info));
     BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
         (bcmi_keygen_pmux_control_set(unit, qual_id,
                             keygen_md, keygen_cfg));
     qual_cfg = pmux_info->pmux_qual_cfg;
     for (idx = 0; idx < qual_cfg->num_chunks; idx++) {
         qual_offset->offset[idx] = qual_cfg->e_params[idx].offset;
         qual_offset->width[idx] = qual_cfg->e_params[idx].width;
         qual_offset->num_offsets++;
     }
exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_pmux_process(int unit,
                         bcmi_keygen_cfg_t *keygen_cfg,
                         bcmi_keygen_md_t *keygen_md)
{
    uint8 idx = 0;
    uint8 num_pmux_qual = 0;
    bcm_field_qualify_t pmux_qual;
    bcmi_keygen_qual_flags_bmp_t *qual_flags = NULL;
    bcmi_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmi_keygen_qual_cfg_info_t **qual_cfg_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    qual_cfg_info = keygen_cfg->qual_cfg_info_db->qual_cfg_info;
    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        pmux_qual = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_cfg = qual_cfg_info[pmux_qual]->qual_cfg_arr;
        qual_flags = &(qual_cfg->qual_flags);
        if (!SHR_BITGET(qual_flags->w, BCMI_KEYGEN_QUAL_FLAGS_PMUX)) {
            continue;
        }
        num_pmux_qual++;
    }

    if (num_pmux_qual == 0) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_NONE);
    }

    BCMI_KEYGEN_ALLOC(keygen_md->pmux_info,
                num_pmux_qual * sizeof(bcmi_keygen_pmux_info_t),
                "Groups Post Mux Qualifiers Information");
    keygen_md->num_pmux_qual = num_pmux_qual;

    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
        (bcmi_keygen_pmux_alloc(unit, keygen_cfg, keygen_md));
exit:
    if (BCMI_KEYGEN_FUNC_ERR()) {
        BCMI_KEYGEN_FREE(keygen_md->pmux_info);
    }
    BCMI_KEYGEN_FUNC_EXIT();
}
