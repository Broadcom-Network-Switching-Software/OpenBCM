/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/esw/keygen_md.h>

STATIC int
bcmi_keygen_qual_cfg_info_free(int unit,
                             bcmi_keygen_qual_cfg_info_t **qual_cfg_info)
{
    bcm_field_qualify_t qual_id;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    if (qual_cfg_info) {
        for (qual_id = 0; qual_id < BCM_FIELD_QUALIFY_MAX; qual_id++) {
            if (qual_cfg_info[qual_id]) {
                BCMI_KEYGEN_FREE(qual_cfg_info[qual_id]->qual_cfg_arr);
                BCMI_KEYGEN_FREE(qual_cfg_info[qual_id]);
            }
        }
    }

    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_qual_cfg_info_db_free(int unit,
                  bcmi_keygen_qual_cfg_info_db_t *qual_cfg_info_db)
{
    bcmi_keygen_qual_cfg_info_t **qual_cfg_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(qual_cfg_info_db, BCM_E_NONE);

    /* Free bus qids sec_cfg memory. */
    qual_cfg_info = qual_cfg_info_db->qual_cfg_info;
    bcmi_keygen_qual_cfg_info_free(unit, qual_cfg_info);

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_md_free(int unit, bcmi_keygen_cfg_t *keygen_cfg, bcmi_keygen_md_t *keygen_md)
{
    uint8 level = 0;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_NONE);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_NONE);

    BCMI_KEYGEN_FREE(keygen_md->qual_cfg_id_arr);
    BCMI_KEYGEN_FREE(keygen_md->pmux_info);

    (void)bcmi_keygen_qual_cfg_info_free(unit, keygen_md->qual_cfg_info);
    for (level = 0; level < BCMI_KEYGEN_EXT_LEVEL_COUNT; level++) {
        if (keygen_md->qual_cfg_info_db[level] == NULL) {
            continue;
        }
        (void)bcmi_keygen_qual_cfg_info_db_free(unit,
                         keygen_md->qual_cfg_info_db[level]);
         BCMI_KEYGEN_FREE(keygen_md->qual_cfg_info_db[level]);
    }

    if (keygen_md->ext_cfg_db != NULL) {
        BCMI_KEYGEN_IF_ERR_EXIT
            (bcmi_keygen_ext_cfg_db_free(unit, keygen_md->ext_cfg_db));
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_qual_cfg_is_valid(int unit,
                            bcm_field_qualify_t qual_id,
                            bcmi_keygen_cfg_t *keygen_cfg,
                            bcmi_keygen_qual_cfg_t *qual_cfg,
                            uint16 qual_cfg_max_size,
                            uint8 *qual_cfg_is_valid)
{
    uint8 idx = 0;
    uint8 qual_cfg_chunk_id = 0;
    uint16 bit = 0;
    uint16 start_bit = 0;
    uint16 end_bit = 0;
    uint16 width = 0;
    uint32 *qual_bitmap = NULL;
    uint32 qual_bitmap1[BCMI_KEYGEN_QUAL_BITMAP_MAX_WORDS] = {0};
    uint32 qual_bitmap2[BCMI_KEYGEN_QUAL_BITMAP_MAX_WORDS] = {0};

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(qual_cfg, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(qual_cfg_is_valid, BCM_E_PARAM);

    *qual_cfg_is_valid = 0;

    if (keygen_cfg->mode == BCMI_KEYGEN_MODE_SINGLE) {
       if (SHR_BITGET(qual_cfg->qual_flags.w,
                      BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_SINGLE)) {
           BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_NONE);
       }
    }

    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        if (keygen_cfg->qual_info_arr[idx].qual_id == qual_id &&
            keygen_cfg->qual_info_arr[idx].partial == TRUE) {
            qual_bitmap = keygen_cfg->qual_info_arr[idx].bitmap;
            break;
        }
    }

    if (qual_bitmap == NULL) {
       sal_memset(qual_bitmap2, 0, sizeof(qual_bitmap2));
       for (bit = 0; bit < qual_cfg_max_size; bit++) {
           SHR_BITSET(qual_bitmap2, bit);
       }
       qual_bitmap = qual_bitmap2;
    }

    start_bit = 0;
    end_bit = 0;
    width = 0;
    sal_memset(qual_bitmap1, 0, sizeof(qual_bitmap1));
    for (qual_cfg_chunk_id = 0;
         qual_cfg_chunk_id < qual_cfg->num_chunks;
         qual_cfg_chunk_id++) {
        start_bit += width;
        width = qual_cfg->e_params[qual_cfg_chunk_id].width;
        end_bit += width;
        for (bit = start_bit; bit < end_bit; bit++) {
            SHR_BITSET(qual_bitmap1, bit);
        }
    }

    *qual_cfg_is_valid = TRUE;
    SHR_BIT_ITER(qual_bitmap, qual_cfg_max_size - 1, bit) {
       if (!SHR_BITGET(qual_bitmap1, bit)) {
          *qual_cfg_is_valid = FALSE;
          break;
       }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_qual_cfg_info_alloc(int unit,
                              bcmi_keygen_cfg_t *keygen_cfg,
                              bcmi_keygen_md_t *keygen_md)
{
    uint8 qual_cfg_is_valid = FALSE;
    uint8 qual_cfg_count = 0;
    uint8 valid_qual_cfg_count = 0;
    uint8 qual_cfg_id = 0;
    uint16 idx = 0;
    uint16 qual_cfg_max_size = 0;
    uint32 qual_cfg_id_bmp = 0;
    bcm_field_qualify_t qual_id;
    bcmi_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmi_keygen_qual_cfg_t *qual_cfg_arr = NULL;
    bcmi_keygen_qual_cfg_info_t **qual_cfg_info1 = NULL;
    bcmi_keygen_qual_cfg_info_t *qual_cfg_info2 = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    qual_cfg_info1 = keygen_cfg->qual_cfg_info_db->qual_cfg_info;

    BCMI_KEYGEN_IF_ERR_EXIT((qual_cfg_info1 == NULL) ? BCM_E_INTERNAL : BCM_E_NONE);

    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {

        qual_id =  keygen_cfg->qual_info_arr[idx].qual_id;

        if (qual_cfg_info1[qual_id] == NULL) {
            continue;
        }

        BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
            bcmi_keygen_qual_cfg_max_size_get(unit,
                                        keygen_cfg,
                                        qual_id,
                                        &qual_cfg_max_size));

        valid_qual_cfg_count = 0;
        qual_cfg_id_bmp = 0;
        for (qual_cfg_id = 0;
             qual_cfg_id < qual_cfg_info1[qual_id]->num_qual_cfg;
             qual_cfg_id++) {

            qual_cfg = &(qual_cfg_info1[qual_id]->qual_cfg_arr[qual_cfg_id]);
            qual_cfg_is_valid = FALSE;
            BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
                bcmi_keygen_qual_cfg_is_valid(unit,
                                            qual_id,
                                            keygen_cfg,
                                            qual_cfg,
                                            qual_cfg_max_size,
                                            &qual_cfg_is_valid));

            if (qual_cfg_is_valid == TRUE) {
                SHR_BITSET(&qual_cfg_id_bmp, qual_cfg_id);
                valid_qual_cfg_count++;
            }
        }

        BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
            (valid_qual_cfg_count == 0) ? BCM_E_CONFIG : BCM_E_NONE);

        qual_cfg_info2 = NULL;
        BCMI_KEYGEN_ALLOC(qual_cfg_info2,
                  sizeof(bcmi_keygen_qual_cfg_info_t),
                  "BCMI_KEYGEN: Group Qual Config Information");
        keygen_md->qual_cfg_info[qual_id] = qual_cfg_info2;

        qual_cfg_arr = NULL;
        BCMI_KEYGEN_ALLOC(qual_cfg_arr,
                  valid_qual_cfg_count * sizeof(bcmi_keygen_qual_cfg_t),
                  "BCMI_KEYGEN:Group Qual Configuration array");
        qual_cfg_info2->num_qual_cfg = valid_qual_cfg_count;
        qual_cfg_info2->qual_cfg_arr = qual_cfg_arr;

        qual_cfg_count = 0;
        for (qual_cfg_id = 0;
             qual_cfg_id < qual_cfg_info1[qual_id]->num_qual_cfg;
             qual_cfg_id++) {

            qual_cfg =
                &(qual_cfg_info1[qual_id]->qual_cfg_arr[qual_cfg_id]);

            if (SHR_BITGET(&qual_cfg_id_bmp, qual_cfg_id)) {
                qual_cfg =
                    &(qual_cfg_info1[qual_id]->qual_cfg_arr[qual_cfg_id]);
                sal_memcpy(&(qual_cfg_arr[qual_cfg_count]),
                           qual_cfg, sizeof(bcmi_keygen_qual_cfg_t));
                qual_cfg_count++;
            }
        }
    }

exit:
    if (BCMI_KEYGEN_FUNC_ERR()) {
        (void)bcmi_keygen_qual_cfg_info_free(unit, keygen_md->qual_cfg_info);
    }

    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_qual_cfg_info_db_alloc(int unit,
                                 bcmi_keygen_cfg_t *keygen_cfg,
                                 bcmi_keygen_md_t *keygen_md)
{
    uint8 level = 0;
    uint16 idx = 0;
    bcm_field_qualify_t qual_id;
    bcmi_keygen_qual_cfg_t *qual_cfg_arr = NULL;
    bcmi_keygen_qual_cfg_info_t *qual_cfg_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    for (level = 0; level < BCMI_KEYGEN_EXT_LEVEL_COUNT; level++) {
        keygen_md->qual_cfg_info_db[level] = NULL;
    }

    BCMI_KEYGEN_ALLOC(keygen_md->qual_cfg_info_db[0],
                sizeof(bcmi_keygen_qual_cfg_info_db_t),
                "BCMI_KEYGEN:Group Qualifer Config Information DB");

    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
        if (keygen_cfg->qual_cfg_info_db->qual_cfg_info[qual_id] == NULL) {
            continue;
        }
        BCMI_KEYGEN_ALLOC(qual_cfg_info,
                    sizeof(bcmi_keygen_qual_cfg_info_t),
                    "BCMI_KEYGEN:Group Qual Config Information");
        keygen_md->qual_cfg_info_db[0]->qual_cfg_info[qual_id] = qual_cfg_info;
        BCMI_KEYGEN_ALLOC(qual_cfg_arr,
                    sizeof(bcmi_keygen_qual_cfg_t),
                    "BCMI_KEYGEN:Group Qual Config");
        qual_cfg_info->qual_cfg_arr = qual_cfg_arr;
        qual_cfg_info->num_qual_cfg = 1;
        qual_cfg_info = NULL;
        qual_cfg_arr = NULL;
    }

exit:
    if (BCMI_KEYGEN_FUNC_ERR()) {
        (void)bcmi_keygen_qual_cfg_info_db_free(unit,
                          keygen_md->qual_cfg_info_db[0]);
        BCMI_KEYGEN_FREE(keygen_md->qual_cfg_info_db[0]);
    }
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_qual_cfg_id_array_alloc(int unit,
                                  bcmi_keygen_cfg_t *keygen_cfg,
                                  bcmi_keygen_md_t *keygen_md)
{
    uint16 idx = 0;
    bcm_field_qualify_t qual_id;
    bcmi_keygen_qual_cfg_t *qual_cfg = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    BCMI_KEYGEN_ALLOC(keygen_md->qual_cfg_id_arr,
                sizeof(uint8) * keygen_cfg->qual_info_count,
                "Group Qual Config ID Tracker");

    keygen_md->qual_cfg_combinations = 1;
    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
        if (keygen_md->qual_cfg_info[qual_id] == NULL) {
            continue;
        }
        qual_cfg =
            &(keygen_md->qual_cfg_info[qual_id]->qual_cfg_arr[0]);
        if (SHR_BITGET(qual_cfg->qual_flags.w,
            BCMI_KEYGEN_QUAL_FLAGS_PMUX)) {
            continue;
        }
        keygen_md->qual_cfg_combinations *=
              keygen_md->qual_cfg_info[qual_id]->num_qual_cfg;
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_acl_attrs_update(int unit,
                             bcmi_keygen_cfg_t *keygen_cfg,
                             bcmi_keygen_md_t *keygen_md)
{
    uint8 part = 0;
    uint16 idx = 0;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    if (keygen_cfg->ext_ctrl_sel_info == NULL ||
        keygen_cfg->ext_ctrl_sel_info_count == 0) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_NONE);
    }

    for (idx = 0; idx < keygen_cfg->ext_ctrl_sel_info_count; idx++) {
        part = keygen_cfg->ext_ctrl_sel_info[idx].part;
        if (keygen_cfg->ext_ctrl_sel_info[idx].ctrl_sel ==
            BCMI_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT) {
            SHR_BITSET(keygen_md->acl_attrs[part].w,
                       BCMI_KEYGEN_ACL_ATTR_PMUX_IPBM);
        }
        if (keygen_cfg->ext_ctrl_sel_info[idx].ctrl_sel ==
            BCMI_KEYGEN_EXT_CTRL_SEL_PMUX) {
            switch (keygen_cfg->ext_ctrl_sel_info[idx].ctrl_sel_val) {
                case 0:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMI_KEYGEN_ACL_ATTR_PMUX_DROP);
                    break;
                case 1:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMI_KEYGEN_ACL_ATTR_PMUX_NAT_NEEDED);
                    break;
                case 2:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMI_KEYGEN_ACL_ATTR_PMUX_NAT_DST_REALM_ID);
                    break;
                case 3:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMI_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_HIT);
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMI_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_HIT);
                    break;
                case 4:
                    if (keygen_cfg->mode == BCMI_KEYGEN_MODE_SINGLE) {
                        SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB_HM);
                    } else {
                        SHR_BITSET(keygen_md->acl_attrs[part].w,
                                  BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB);
                    }
                    break;
                case 12:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0_HM);
                    break;
                case 14:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0);
                    break;
                case 11:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1_HM);
                    break;
                case 13:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                               BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1);
                    break;
                case 8:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                        BCMI_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_0_3);
                    break;
                case 9:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                        BCMI_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_4_7);
                    break;
                case 10:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                        BCMI_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_8_11);
                    break;
                case 5:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                        BCMI_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_0_3);
                    break;
                case 6:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                        BCMI_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_4_7);
                    break;
                case 7:
                    SHR_BITSET(keygen_md->acl_attrs[part].w,
                        BCMI_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_8_11);
                    break;
                default:
                    BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
            }
        }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_acl_attrs_init(int unit,
                           bcmi_keygen_cfg_t *keygen_cfg,
                           bcmi_keygen_md_t *keygen_md)
{
    uint8 idx = 0;
    uint8 chunk = 0;
    bcmi_keygen_ext_section_t pmux_section;
    bcmi_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmi_keygen_pmux_info_t *pmux_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    if ((keygen_cfg->flags & BCMI_KEYGEN_CFG_FLAGS_QSET_UPDATE)) {
        BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
            bcmi_keygen_acl_attrs_update(unit, keygen_cfg, keygen_md));
    }

    for (idx = 0; idx < keygen_md->num_pmux_qual; idx++) {
        pmux_info = &(keygen_md->pmux_info[idx]);
        qual_cfg = pmux_info->pmux_qual_cfg;
        for (chunk = 0; chunk < qual_cfg->num_chunks; chunk++) {
            pmux_section = qual_cfg->e_params[chunk].section;
            if (pmux_section ==
                BCMI_KEYGEN_EXT_SECTION_IPBM) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMI_KEYGEN_ACL_ATTR_PMUX_IPBM);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_DROP) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMI_KEYGEN_ACL_ATTR_PMUX_DROP);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_NAT_NEEDED) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMI_KEYGEN_ACL_ATTR_PMUX_NAT_NEEDED);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_NAT_DST_REALMID) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMI_KEYGEN_ACL_ATTR_PMUX_NAT_DST_REALM_ID);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0 &&
                       keygen_cfg->mode == BCMI_KEYGEN_MODE_SINGLE) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0_HM);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1 &&
                       keygen_cfg->mode == BCMI_KEYGEN_MODE_SINGLE) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1_HM);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB &&
                       keygen_cfg->mode == BCMI_KEYGEN_MODE_SINGLE) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB_HM);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_HIT) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMI_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_HIT);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_HIT) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                           BCMI_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_HIT);
            } else if (pmux_section ==
                     BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_0_3) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMI_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_0_3);
            } else if (pmux_section ==
                     BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_4_7) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMI_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_4_7);
            } else if (pmux_section ==
                     BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_8_11) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMI_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_8_11);
            } else if (pmux_section ==
                     BCMI_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_0_3) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMI_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_0_3);
            } else if (pmux_section ==
                     BCMI_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_4_7) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMI_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_4_7);
            } else if (pmux_section ==
                     BCMI_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_8_11) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMI_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_8_11);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_A) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMI_KEYGEN_ACL_ATTR_PMUX_DST_CONTAINER_A);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_B) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMI_KEYGEN_ACL_ATTR_PMUX_DST_CONTAINER_B);
            } else if (pmux_section ==
                     BCMI_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_0_3) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMI_KEYGEN_ACL_ATTR_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3);
            } else if (pmux_section ==
                     BCMI_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_4_7) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                    BCMI_KEYGEN_ACL_ATTR_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_PKT_RESOLUTION) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                                   BCMI_KEYGEN_ACL_ATTR_PMUX_PKT_RESOLUTION);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_CLASS_ID_C) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                                   BCMI_KEYGEN_ACL_ATTR_PMUX_CLASS_ID_C);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_CLASS_ID_D) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                                   BCMI_KEYGEN_ACL_ATTR_PMUX_CLASS_ID_D);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_INT_PRI) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                                   BCMI_KEYGEN_ACL_ATTR_PMUX_INT_PRI);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_IFP_DROP_VECTOR) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                                   BCMI_KEYGEN_ACL_ATTR_PMUX_IFP_DROP_VECTOR);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_MH_OPCODE) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                                   BCMI_KEYGEN_ACL_ATTR_PMUX_MH_OPCODE);
            } else if (pmux_section ==
                       BCMI_KEYGEN_EXT_SECTION_LTID) {
                SHR_BITSET(keygen_md->acl_attrs[pmux_info->pmux_part].w,
                                   BCMI_KEYGEN_ACL_ATTR_PMUX_LTID);
            } else {
                BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
            }
            continue;
        }
    }

    if (keygen_cfg->mode == BCMI_KEYGEN_MODE_SINGLE) {
        SHR_BITSET(keygen_md->acl_attrs[0].w, BCMI_KEYGEN_ACL_ATTR_MODE_HALF);
    }

    if (keygen_cfg->mode == BCMI_KEYGEN_MODE_SINGLE_ASET_WIDE) {
        SHR_BITSET(keygen_md->acl_attrs[0].w,
                   BCMI_KEYGEN_ACL_ATTR_MODE_SINGLE_ASET_WIDE);
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_md_alloc(int unit,
                   bcmi_keygen_cfg_t *keygen_cfg,
                   bcmi_keygen_md_t **keygen_md)
{
    uint8 part = 0;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    BCMI_KEYGEN_ALLOC(*keygen_md, sizeof(bcmi_keygen_md_t), "Group MetaData");

    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
        (bcmi_keygen_qual_cfg_info_alloc(unit, keygen_cfg, *keygen_md));

    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
        (bcmi_keygen_qual_cfg_info_db_alloc(unit, keygen_cfg, *keygen_md));

    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
        (bcmi_keygen_ext_cfg_alloc(unit, keygen_cfg, *keygen_md));

    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
        (bcmi_keygen_qual_cfg_id_array_alloc(unit, keygen_cfg, *keygen_md));

    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
        (bcmi_keygen_ext_sections_get(unit, keygen_cfg, *keygen_md));

    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
        (bcmi_keygen_pmux_process(unit, keygen_cfg, *keygen_md));

    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
        (bcmi_keygen_acl_attrs_init(unit, keygen_cfg, *keygen_md));

    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
        (bcmi_keygen_ext_section_gran_info_init(unit, *keygen_md));

    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
        (bcmi_keygen_ext_section_drain_bits_update(unit, *keygen_md));

    for (part = 0; part < keygen_cfg->num_parts; part++) {
        BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT
            (bcmi_keygen_ext_codes_init(unit, &((*keygen_md)->ext_codes[part])));
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_cfg_md_process(int unit,
                         bcmi_keygen_cfg_t *keygen_cfg,
                         bcmi_keygen_oper_t *keygen_oper,
                         bcmi_keygen_md_t *keygen_md)
{
    uint8 *qual_cfg_id_arr = NULL;
    uint16 idx = 0;
    uint16 first_non_pmux_qual_idx = 0;
    uint8 all_pmux_qset = TRUE;
    uint32 qual_cfg_combinations = 0;
    uint32 qual_cfg_combinations_tried = 0;
    bcm_field_qualify_t qual_id = bcmFieldQualifyCount;
    bcmi_keygen_qual_cfg_t *qual_cfg1 = NULL;
    bcmi_keygen_qual_cfg_t *qual_cfg2 = NULL;
    bcmi_keygen_qual_cfg_info_t **qual_cfg_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    qual_cfg_info = keygen_md->qual_cfg_info;
    qual_id = keygen_cfg->qual_info_arr[0].qual_id;
    qual_cfg_id_arr = keygen_md->qual_cfg_id_arr;
    qual_cfg_combinations = keygen_md->qual_cfg_combinations;

    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_cfg1 = &(qual_cfg_info[qual_id]->qual_cfg_arr[0]);
        if (!SHR_BITGET(qual_cfg1->qual_flags.w,
                       BCMI_KEYGEN_QUAL_FLAGS_PMUX)) {
             first_non_pmux_qual_idx = idx;
             all_pmux_qset = FALSE;
             break;
        }
    }

    if (all_pmux_qset == TRUE) {
        BCMI_KEYGEN_RETURN_VAL_EXIT
            (bcmi_keygen_ext_alloc(unit,
                                   keygen_cfg,
                                   keygen_oper,
                                   keygen_md));
    }

    while (TRUE) {
        _func_rv =
            bcmi_keygen_ext_alloc(unit, keygen_cfg, keygen_oper, keygen_md);
        if (BCMI_KEYGEN_FAILURE(_func_rv) && (_func_rv != BCM_E_RESOURCE)) {
            BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(_func_rv);
        } else if (BCMI_KEYGEN_SUCCESS(_func_rv)) {
           break;
        }

        qual_cfg_combinations_tried++;

        /* all combinations are tried , but still no success. */
        if (qual_cfg_combinations_tried == qual_cfg_combinations) {
            BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(_func_rv);
        }

        /* Identify the next configuration of each qualifer to try with. */
        qual_id = keygen_cfg->qual_info_arr[first_non_pmux_qual_idx].qual_id;

        if (qual_cfg_id_arr[first_non_pmux_qual_idx] ==
            qual_cfg_info[qual_id]->num_qual_cfg - 1) {

           qual_cfg_id_arr[first_non_pmux_qual_idx] = 0;

           idx = first_non_pmux_qual_idx + 1;
           while (idx < keygen_cfg->qual_info_count) {
               qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
               qual_cfg2 = &(qual_cfg_info[qual_id]->qual_cfg_arr[0]);
               if (SHR_BITGET(qual_cfg2->qual_flags.w,
                              BCMI_KEYGEN_QUAL_FLAGS_PMUX)) {
                   idx++;
                   continue;
               }
               if (qual_cfg_id_arr[idx] <
                   qual_cfg_info[qual_id]->num_qual_cfg - 1) {
                   qual_cfg_id_arr[idx]++;
                   break;
               } else {
                   qual_cfg_id_arr[idx] = 0;
               }
               idx++;
           }
        } else {
           qual_cfg_id_arr[first_non_pmux_qual_idx]++;
        }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_qual_cfg_max_size_get(int unit,
                            bcmi_keygen_cfg_t *keygen_cfg,
                            bcm_field_qualify_t qual_id,
                            uint16 *qual_cfg_max_size)
{
    uint8 qual_cfg_id = 0;
    uint8 qual_cfg_chunk_id = 0;
    uint16 qual_cfg_size = 0;
    bcmi_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmi_keygen_qual_cfg_info_t *qual_cfg_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(qual_cfg_max_size, BCM_E_PARAM);

    *qual_cfg_max_size = 0;

    qual_cfg_info = keygen_cfg->qual_cfg_info_db->qual_cfg_info[qual_id];

    for (qual_cfg_id = 0;
         qual_cfg_id < qual_cfg_info->num_qual_cfg;
         qual_cfg_id++) {
         qual_cfg = &(qual_cfg_info->qual_cfg_arr[qual_cfg_id]);
         qual_cfg_size = 0;
         for (qual_cfg_chunk_id = 0;
              qual_cfg_chunk_id < qual_cfg->num_chunks;
              qual_cfg_chunk_id++) {
              qual_cfg_size += qual_cfg->e_params[qual_cfg_chunk_id].width;
         }

         if (*qual_cfg_max_size < qual_cfg_size) {
             *qual_cfg_max_size = qual_cfg_size;
         }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_keysize_get(int unit,
                        bcmi_keygen_cfg_t *keygen_cfg,
                        uint16 *rkey_size)
{
    int key_size = 0;
    uint8 chunk = 0;
    uint16 idx = 0;
    bcm_field_qualify_t qual_id;
    bcmi_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmi_keygen_qual_cfg_info_t *qual_cfg_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(rkey_size, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    *rkey_size = 0;
    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_cfg_info = keygen_cfg->qual_cfg_info_db->qual_cfg_info[qual_id];
        if (qual_cfg_info == NULL) {
            continue;
        }
        qual_cfg = &(qual_cfg_info->qual_cfg_arr[0]);
        if (qual_cfg == NULL) {
            continue;
        }
        if (qual_cfg->pri_ctrl_sel != BCMI_KEYGEN_EXT_CTRL_SEL_DISABLE) {
            continue;
        }
        if (SHR_BITGET(qual_cfg->qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_PMUX)) {
            continue;
        }
        if (keygen_cfg->qual_info_arr[idx].partial == TRUE) {
            key_size = 0;
            SHR_BITCOUNT_RANGE(keygen_cfg->qual_info_arr[idx].bitmap, key_size,
                         0, sizeof(uint32) * BCMI_KEYGEN_QUAL_BITMAP_MAX_WORDS);
            *rkey_size += key_size;
        } else {
            for (chunk = 0; chunk < qual_cfg->num_chunks; chunk++) {
                *rkey_size += qual_cfg->e_params[chunk].width;
            }
        }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_oper_free(int unit,
                      uint16 num_parts,
                      bcmi_keygen_oper_t *keygen_oper)
{
    uint16 part = 0;
    bcmi_keygen_qual_offset_info_t *offset_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    if (keygen_oper == NULL) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_NONE);
    }

    if (keygen_oper->qual_offset_info == NULL &&
        keygen_oper->ext_ctrl_sel_info == NULL) {
        BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(BCM_E_NONE);
    }

    if (keygen_oper->qual_offset_info != NULL) {
        for (part = 0; part < num_parts; part++) {
            offset_info = &(keygen_oper->qual_offset_info[part]);
            BCMI_KEYGEN_FREE(offset_info->qid_arr);
            BCMI_KEYGEN_FREE(offset_info->offset_arr);
        }
        BCMI_KEYGEN_FREE(keygen_oper->qual_offset_info);
    }

    if (keygen_oper->ext_ctrl_sel_info != NULL) {
        BCMI_KEYGEN_FREE(keygen_oper->ext_ctrl_sel_info);
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_qual_cfg_t_init(int unit, bcmi_keygen_qual_cfg_t *qual_cfg)
{
    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(qual_cfg, BCM_E_PARAM);

    sal_memset(qual_cfg, 0, sizeof(bcmi_keygen_qual_cfg_t));

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_qual_cfg_insert(int unit,
                 bcm_field_qualify_t qual_id,
                 bcmi_keygen_qual_cfg_t *new_qual_cfg,
                 bcmi_keygen_qual_cfg_info_db_t *qual_cfg_info_db)
{
    uint8 qual_cfg_info_db_alloc = 0;
    uint8 qual_cfg_info_alloc = 0;
    uint8 chunk = 0;
    uint8 num_qual_cfg = 0;
    bcmi_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmi_keygen_qual_cfg_t *qual_cfg_arr = NULL;
    bcmi_keygen_qual_cfg_info_t *qual_cfg_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(new_qual_cfg, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(qual_cfg_info_db, BCM_E_PARAM);

    qual_cfg_info = qual_cfg_info_db->qual_cfg_info[qual_id];
    if (qual_cfg_info == NULL) {
        BCMI_KEYGEN_ALLOC(qual_cfg_info,
                    sizeof(bcmi_keygen_qual_cfg_info_t),
                    "BCMI_KEYGEN:Qualifier Config Info");
        qual_cfg_info_db->qual_cfg_info[qual_id] = qual_cfg_info;
        qual_cfg_info->qual = qual_id;
        qual_cfg_info_alloc = 1;
    }

    num_qual_cfg = qual_cfg_info->num_qual_cfg + 1;

    BCMI_KEYGEN_ALLOC(qual_cfg_arr,
                num_qual_cfg * sizeof(bcmi_keygen_qual_cfg_t),
                "BCMI_KEYGEN:Qualifier config");

    sal_memcpy(qual_cfg_arr,
               qual_cfg_info->qual_cfg_arr,
               sizeof(bcmi_keygen_qual_cfg_t) * (num_qual_cfg - 1));

    qual_cfg = &(qual_cfg_arr[num_qual_cfg - 1]);

    for (chunk = 0; chunk < new_qual_cfg->num_chunks; chunk++) {
        if (BCMI_KEYGEN_EXT_SECTION_L1E32 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 32;
        } else if (BCMI_KEYGEN_EXT_SECTION_L1E16 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 16;
        } else if (BCMI_KEYGEN_EXT_SECTION_L1E8 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 8;
        } else if (BCMI_KEYGEN_EXT_SECTION_L1E4 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 4;
        } else if (BCMI_KEYGEN_EXT_SECTION_L1E2 ==
            new_qual_cfg->e_params[chunk].section) {
            new_qual_cfg->e_params[chunk].size = 2;
        }
    }

    sal_memcpy(qual_cfg, new_qual_cfg, sizeof(bcmi_keygen_qual_cfg_t));

    BCMI_KEYGEN_FREE(qual_cfg_info->qual_cfg_arr);

    qual_cfg_info->qual_cfg_arr = qual_cfg_arr;
    qual_cfg_info->num_qual_cfg++;

    BCMI_KEYGEN_FUNC_EXIT();

exit:
    BCMI_KEYGEN_FREE(qual_cfg_arr);
    if (qual_cfg_info_db_alloc) {
        BCMI_KEYGEN_FREE(qual_cfg_info_db);
    }
    if (qual_cfg_info_alloc) {
        BCMI_KEYGEN_FREE(qual_cfg_info);
    }
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_cfg_process(int unit,
                        bcmi_keygen_cfg_t *keygen_cfg,
                        bcmi_keygen_oper_t *keygen_oper)
{
    bcmi_keygen_md_t *keygen_md = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    if (keygen_cfg->qual_info_count == 0) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_NONE);
    }

    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(bcmi_keygen_md_alloc(unit, keygen_cfg, &keygen_md));

    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
        bcmi_keygen_cfg_md_process(unit, keygen_cfg, keygen_oper, keygen_md));
exit:
    (void)bcmi_keygen_md_free(unit, keygen_cfg, keygen_md);
    BCMI_KEYGEN_FREE(keygen_md);
    BCMI_KEYGEN_FUNC_EXIT();
}
