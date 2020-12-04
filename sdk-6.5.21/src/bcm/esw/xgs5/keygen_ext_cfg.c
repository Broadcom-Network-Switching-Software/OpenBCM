/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/esw/keygen_md.h>

int
bcmi_keygen_ext_cfg_db_free(int unit, bcmi_keygen_ext_cfg_db_t *ext_cfg_db)
{
    int level;
    int ext_sec;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(ext_cfg_db, BCM_E_PARAM);

    /* Free extractor sec_cfg memory. */
    for (ext_sec = BCMI_KEYGEN_EXT_SECTION_L1E32;
         ext_sec < BCMI_KEYGEN_EXT_SECTION_COUNT;
         ext_sec++) {
        BCMI_KEYGEN_FREE(ext_cfg_db->sec_cfg[ext_sec]);
    }

    /* Free extractors configuration memory for all levels. */
    for (level = 1; level < BCMI_KEYGEN_EXT_LEVEL_COUNT; level++) {
        BCMI_KEYGEN_FREE(ext_cfg_db->ext_cfg[level]);
    }

    BCMI_KEYGEN_FREE(ext_cfg_db);

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_cfg_reset(int unit,
                        bcmi_keygen_cfg_t *keygen_cfg,
                        bcmi_keygen_md_t *keygen_md)
{
    bcmi_keygen_ext_cfg_db_t *orig_ext_info = NULL; /* Extractor info original list. */
    int alloc_sz;                            /* Memory allocation size.       */
    int level;                               /* Extractor hierarcy level.     */
    int num_sec;                             /* Number of Sections.           */
    bcmi_keygen_ext_section_t ext_sel;         /* Extractor selector type.      */
    bcmi_keygen_ext_section_cfg_t *org_ext_secs;     /* Original extractor sec_cfg.  */
    bcmi_keygen_ext_cfg_db_t **ext_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    /* Get extractor information for the selected extractor mode. */
    orig_ext_info = keygen_cfg->ext_cfg_db;
    /* Confirm original extractor info list is initialized. */
    if (NULL == orig_ext_info) {
        BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
    }

    ext_info = &(keygen_md->ext_cfg_db);
    if (*ext_info == NULL) {
        BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
    }

    /* Set the extractor mode. */
    (*ext_info)->mode = keygen_cfg->ext_cfg_db->mode;

    /* Copy the extractor hierarchy section list for the selected mode. */
    for (level = 1; level < keygen_md->num_ext_levels; level++) {
        alloc_sz = orig_ext_info->conf_size[level];

        /* Confirm extractor configuration has a valid size. */
        if (0 == alloc_sz) {
            BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
        }

        if ((*ext_info)->ext_cfg[level] == NULL) {
            BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
        }

        /* Copy extractor configuration for current level. */
        sal_memcpy((*ext_info)->ext_cfg[level], orig_ext_info->ext_cfg[level],
            alloc_sz * sizeof(bcmi_keygen_ext_cfg_t));

        /* Copy extractor configuration size information. */
        (*ext_info)->conf_size[level] = orig_ext_info->conf_size[level];
    }

    /* Copy extractor sec_cfg information for the selected HW slice mode. */
    if ((NULL != orig_ext_info) && (orig_ext_info->num_sec > 0)) {

        /* Initialize number of sec_cfg information in new extractor info. */
        (*ext_info)->num_sec = orig_ext_info->num_sec;

        /*
         * Get number of extractor sec_cfg initialized for current extractor
         * mode from the new extractor information structure.
         */
        num_sec = (*ext_info)->num_sec;

        /* Iterate over valid extractors and copy their configuration. */
        for (ext_sel = BCMI_KEYGEN_EXT_SECTION_L1E32;
             ext_sel < BCMI_KEYGEN_EXT_SECTION_COUNT; ext_sel++) {

            org_ext_secs = orig_ext_info->sec_cfg[ext_sel];
            if (NULL != org_ext_secs) {

                if ((*ext_info)->sec_cfg[ext_sel] == NULL) {
                    BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
                }

                /* Copy extractor information. */
                sal_memcpy((*ext_info)->sec_cfg[ext_sel], org_ext_secs,
                    sizeof(bcmi_keygen_ext_section_cfg_t));

                /* Keep track of how many sec_cfg are yet to be copied. */
                num_sec--;
            }
        }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_cfg_reset_part(int unit,
                             uint8 part,
                             bcmi_keygen_cfg_t *keygen_cfg,
                             bcmi_keygen_md_t *keygen_md)
{
    uint8 level = 0;
    uint8 ext_part = 0;
    uint8 multi_gran_present = FALSE;
    uint16 ext_idx = 0;
    bcmi_keygen_ext_section_t section;
    bcmi_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmi_keygen_ext_section_cfg_t *sec_cfg = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    for (section = BCMI_KEYGEN_EXT_SECTION_L1E32;
         section < BCMI_KEYGEN_EXT_SECTION_COUNT; section++) {
         sec_cfg = keygen_md->ext_cfg_db->sec_cfg[section];
         if (sec_cfg != NULL) {
             sec_cfg->fill_bits = 0;
         }
    }

    for (level = 0; level <= keygen_md->num_ext_levels; level++) {
        for (ext_idx = 0;
             ext_idx < keygen_md->ext_cfg_db->conf_size[level];
             ext_idx++) {
            ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
            ext_part = BCMI_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
            if (part != ext_part) {
                continue;
            }
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                sec_cfg = keygen_md->ext_cfg_db->sec_cfg[ext_cfg->in_sec];
                sec_cfg->fill_bits +=
                             BCMI_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
                continue;
            }
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN)) {
                multi_gran_present = TRUE;
                if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_1)) {
                    BCMI_KEYGEN_EXT_ID_GRAN_SET(ext_cfg->ext_id, 1);
                } else if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_2)) {
                    BCMI_KEYGEN_EXT_ID_GRAN_SET(ext_cfg->ext_id, 2); 
                } else if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_4)) {
                    BCMI_KEYGEN_EXT_ID_GRAN_SET(ext_cfg->ext_id, 4); 
                } else if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_8)) {
                    BCMI_KEYGEN_EXT_ID_GRAN_SET(ext_cfg->ext_id, 8); 
                } else if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_16)) {
                    BCMI_KEYGEN_EXT_ID_GRAN_SET(ext_cfg->ext_id, 16); 
                } else if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_32)) {
                    BCMI_KEYGEN_EXT_ID_GRAN_SET(ext_cfg->ext_id, 32); 
                } else {
                    BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_INTERNAL);
                }
            }
            ext_cfg->in_use = FALSE;
            ext_cfg->finfo = NULL;
        }
    }
    if (multi_gran_present == TRUE) {
        (void)bcmi_keygen_ext_section_gran_info_init(unit, keygen_md);
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_cfg_update(int unit,
                        bcmi_keygen_cfg_t *keygen_cfg,
                        bcmi_keygen_md_t *keygen_md)
{
    uint8 level = 0;
    uint8 ext_gran = 0;
    uint8 ext_part = 0;
    uint16 ext_num = 0;
    uint16 idx = 0;
    uint16 ext_id = 0;
    uint16 pt_ext_id = 0;
    bcmi_keygen_ext_section_t out_section;
    bcmi_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmi_keygen_ext_section_cfg_t *sec_cfg = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);

    if (keygen_cfg->ext_ctrl_sel_info == NULL ||
        keygen_cfg->ext_ctrl_sel_info_count == 0) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_NONE);
    }

    /* ext_ctrl_sel_info can be NULL in case of QSET update
     * if earlier calls to keygen has no qualifiers present
     * in keygen configuration.
     */
    for (idx = 0; idx < keygen_cfg->ext_ctrl_sel_info_count; idx++) {
        if (keygen_cfg->ext_ctrl_sel_info[idx].ctrl_sel ==
                        BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER) {
            level = keygen_cfg->ext_ctrl_sel_info[idx].level;
            for (ext_id = 0;
                 ext_id < keygen_md->ext_cfg_db->conf_size[level];
                 ext_id++) {
                ext_cfg = keygen_md->ext_cfg_db->ext_cfg[level] + ext_id;
                ext_part = BCMI_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
                ext_gran = BCMI_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
                ext_num = BCMI_KEYGEN_EXT_ID_NUM_GET(ext_cfg->ext_id);

                if (ext_part == keygen_cfg->ext_ctrl_sel_info[idx].part &&
                    ext_num == keygen_cfg->ext_ctrl_sel_info[idx].ext_num) {

                    if (SHR_BITGET((ext_cfg)->ext_attrs.w,
                                BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN)) {
                        ext_gran = keygen_cfg->ext_ctrl_sel_info[idx].gran;
                        BCMI_KEYGEN_EXT_ID_GRAN_SET(ext_cfg->ext_id, ext_gran);
                    }
                    if (ext_gran != keygen_cfg->ext_ctrl_sel_info[idx].gran) {
                        continue;
                    }
                    ext_cfg->in_use = TRUE;
                    SHR_BITSET(ext_cfg->ext_attrs.w,
                         BCMI_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE);
                    out_section = ext_cfg->out_sec;
                    sec_cfg = keygen_md->ext_cfg_db->sec_cfg[out_section];
                    if (sec_cfg->flags & BCMI_KEYGEN_EXT_SECTION_PASS_THRU) {
                        for (pt_ext_id = 0;
                             (pt_ext_id <
                             keygen_md->ext_cfg_db->conf_size[level + 1]);
                             pt_ext_id++) {
                             ext_cfg =
                                 (keygen_md->ext_cfg_db->ext_cfg[level + 1] +
                                  pt_ext_id);
                             ext_part =
                                 BCMI_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
                             ext_gran =
                                 BCMI_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
                             ext_num =
                                 BCMI_KEYGEN_EXT_ID_NUM_GET(ext_cfg->ext_id);
                             if ((ext_part ==
                                  keygen_cfg->ext_ctrl_sel_info[idx].part) &&
                                 (ext_gran ==
                                  keygen_cfg->ext_ctrl_sel_info[idx].gran) &&
                                 (ext_num ==
                                  keygen_cfg->ext_ctrl_sel_info[idx].ext_num)) {
                                 ext_cfg->in_use = TRUE;
                                 SHR_BITSET(ext_cfg->ext_attrs.w,
                                            BCMI_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE);
                             }
                        }
                    }
                }
            }
        }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_section_drain_bits_update(int unit,
                       bcmi_keygen_md_t *keygen_md)
{
    uint8 level = 0;
    uint8 ext_gran = 0;
    uint8 ext_part = 0;
    uint16 idx = 0;
    uint16 ext_id = 0;
    uint16 attr_enum_count = 0;
    bcmi_keygen_ext_section_t in_section;
    bcmi_keygen_ext_section_t out_section;
    bcmi_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmi_keygen_ext_section_cfg_t *sec_cfg = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);

    /* Number of enumerations in extractor attributes and acl attributes
     * are same and have 1-to-1 mapping.
     */
    attr_enum_count = BCMI_KEYGEN_EXT_ATTR_COUNT;
    for (level = 0; level < keygen_md->num_ext_levels; level++) {
        for (ext_id = 0;
            ext_id < keygen_md->ext_cfg_db->conf_size[level + 1];
            ext_id++) {
            ext_cfg = keygen_md->ext_cfg_db->ext_cfg[level + 1] + ext_id;
            ext_gran = BCMI_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
            ext_part = BCMI_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
            for (idx = 0; idx < attr_enum_count; idx++) {
                if (SHR_BITGET(keygen_md->acl_attrs[ext_part].w, idx) &&
                    SHR_BITGET(ext_cfg->ext_attrs.w, idx)) {
                    in_section = ext_cfg->in_sec;
                    sec_cfg = keygen_md->ext_cfg_db->sec_cfg[in_section];
                    sec_cfg->drain_bits -= ext_gran;
                    if (level == (keygen_md->num_ext_levels - 1)) {
                        out_section = ext_cfg->out_sec;
                        sec_cfg = keygen_md->ext_cfg_db->sec_cfg[out_section];
                        sec_cfg->drain_bits -= ext_gran;
                    }
                    break;
                }
            }
        }
    }

    /* Reduce drain bits in final level. */
    level = keygen_md->num_ext_levels;
    for (ext_id = 0;
        ext_id < keygen_md->ext_cfg_db->conf_size[level];
        ext_id++) {
        ext_cfg = keygen_md->ext_cfg_db->ext_cfg[level] + ext_id;
        ext_gran = BCMI_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);

        if ((ext_cfg->in_use == TRUE) &&
                (SHR_BITGET(ext_cfg->ext_attrs.w,
                            BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN))) {
            out_section = ext_cfg->out_sec;
            sec_cfg = keygen_md->ext_cfg_db->sec_cfg[out_section];
            sec_cfg->drain_bits -= ext_gran;
        }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_cfg_alloc(int unit,
                        bcmi_keygen_cfg_t *keygen_cfg,
                        bcmi_keygen_md_t *keygen_md)
{
    int alloc_sz;                            /* Memory allocation size.       */
    int idx;                                 /* Iterator Index.               */
    int level;                               /* Extractor hierarcy level.     */
    int num_sec;                             /* Number of Sections.           */
    bcmi_keygen_ext_section_t ext_sel;         /* Extractor selector type.      */
    bcmi_keygen_ext_section_cfg_t *org_ext_secs;     /* Original extractor sec_cfg.  */
    bcmi_keygen_ext_cfg_db_t *orig_ext_info = NULL; /* Extractor info original list. */
    bcmi_keygen_ext_cfg_db_t **ext_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);

    ext_info = &(keygen_md->ext_cfg_db);

    /* Get extractor information for the selected extractor mode. */
    orig_ext_info = keygen_cfg->ext_cfg_db;

    /* Confirm original extractor info list is initialized. */
    if (NULL == orig_ext_info) {
        BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
    }

    /* Allocate extractor info memory. */
    BCMI_KEYGEN_ALLOC(*ext_info, sizeof(bcmi_keygen_ext_cfg_db_t), "IFP ext info");

    /* Set the extractor mode. */
    (*ext_info)->mode = keygen_cfg->ext_cfg_db->mode;

    keygen_md->num_ext_levels = orig_ext_info->num_ext_levels - 1;
    /* Copy the extractor hierarchy section list for the selected mode. */
    for (level = 1; level < orig_ext_info->num_ext_levels; level++) {
        alloc_sz = orig_ext_info->conf_size[level];
        /* Confirm extractor configuration has a valid size. */
        if (0 == alloc_sz) {
            BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);
        }
        /* Allocate memory for extractor configurations in input bus list. */
        BCMI_KEYGEN_ALLOC((*ext_info)->ext_cfg[level],
            (alloc_sz * sizeof(bcmi_keygen_ext_cfg_t)), "IFP ext config");
        /* Copy extractor configuration for current level. */
        sal_memcpy((*ext_info)->ext_cfg[level], orig_ext_info->ext_cfg[level],
            alloc_sz * sizeof(bcmi_keygen_ext_cfg_t));
        /* Copy extractor configuration size information. */
        (*ext_info)->conf_size[level] = orig_ext_info->conf_size[level];
    }

    /* Copy extractor sec_cfg information for the selected HW slice mode. */
    if (orig_ext_info->num_sec > 0) {
        /* Initialize number of sec_cfg information in new extractor info. */
        (*ext_info)->num_sec = orig_ext_info->num_sec;
        /*
         * Get number of extractor sec_cfg initialized for current extractor
         * mode from the new extractor information structure.
         */
        num_sec = (*ext_info)->num_sec;
        /* Iterate over valid extractors and copy their configuration. */
        for (ext_sel = BCMI_KEYGEN_EXT_SECTION_L1E32;
             ext_sel < BCMI_KEYGEN_EXT_SECTION_COUNT; ext_sel++) {
            org_ext_secs = orig_ext_info->sec_cfg[ext_sel];
            if (NULL != org_ext_secs) {
                /* Allocate memory for extractor sec_cfg. */
                BCMI_KEYGEN_ALLOC((*ext_info)->sec_cfg[ext_sel],
                    sizeof(bcmi_keygen_ext_section_cfg_t), "IFP Ext section");
                /* Copy extractor information. */
                sal_memcpy((*ext_info)->sec_cfg[ext_sel], org_ext_secs,
                    sizeof(bcmi_keygen_ext_section_cfg_t));
                /* Keep track of how many sec_cfg are yet to be copied. */
                num_sec--;
            }
        }
    }

    /* If preparing keygen for QSET update. */
    if (keygen_cfg->flags & BCMI_KEYGEN_CFG_FLAGS_QSET_UPDATE) {
        BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
            bcmi_keygen_ext_cfg_update(unit, keygen_cfg, keygen_md));
    }
    BCMI_KEYGEN_FUNC_EXIT();
exit:
    if (ext_info != NULL) {
        if (*ext_info != NULL) {
            for (idx = 1; idx < BCMI_KEYGEN_EXT_LEVEL_COUNT; idx++) {
                BCMI_KEYGEN_FREE((*ext_info)->ext_cfg[idx]);
            }
            for (idx = 0; idx < BCMI_KEYGEN_EXT_SECTION_COUNT; idx++) {
                BCMI_KEYGEN_FREE((*ext_info)->sec_cfg[idx]);
            }
            BCMI_KEYGEN_FREE(*ext_info);
        }
    }
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_sections_get(int unit,
                           bcmi_keygen_cfg_t *keygen_cfg,
                           bcmi_keygen_md_t *keygen_md)
{
    uint8 part = 0;
    uint8 level = 0;
    uint16 ext_idx = 0;
    bcmi_keygen_ext_cfg_t *ext_cfg = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(keygen_cfg, BCM_E_PARAM);

    for (level = 1; level <= keygen_md->num_ext_levels; level++) {
        for (ext_idx = 0;
             ext_idx < keygen_md->ext_cfg_db->conf_size[level];
             ext_idx++) {
             ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
             part = BCMI_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
             keygen_md->sections[part][level -1][ext_cfg->in_sec] =
                                                      ext_cfg->in_sec;
        }
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_cfg_t_init(int unit, bcmi_keygen_ext_cfg_t *ext_cfg)
{
    BCMI_KEYGEN_FUNC_ENTER(unit);
    BCMI_KEYGEN_NULL_CHECK(ext_cfg, BCM_E_PARAM);
    sal_memset(ext_cfg, 0, sizeof(bcmi_keygen_ext_cfg_t));
exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_ext_cfg_add(int unit,
                 uint8 level,
                 bcmi_keygen_ext_cfg_t *new_ext_cfg,
                 bcmi_keygen_ext_cfg_db_t *ext_cfg_db)
{
    uint32 size = 0;
    bcmi_keygen_ext_cfg_t *ext_cfg = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(new_ext_cfg, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(ext_cfg_db, BCM_E_PARAM);

    size = (ext_cfg_db->conf_size[level] + 1)  * sizeof(bcmi_keygen_ext_cfg_t);
    BCMI_KEYGEN_ALLOC(ext_cfg, size, "Extractor Config");

    if (ext_cfg_db->ext_cfg[level] != NULL) {
        size -= sizeof(bcmi_keygen_ext_cfg_t);
        sal_memcpy(ext_cfg, ext_cfg_db->ext_cfg[level], size);
        BCMI_KEYGEN_FREE(ext_cfg_db->ext_cfg[level]);
    } else {
        ext_cfg_db->num_ext_levels++;
    }

    ext_cfg_db->ext_cfg[level] = ext_cfg;
    ext_cfg_db->ext_cfg[level][ext_cfg_db->conf_size[level]] = *new_ext_cfg;
    ext_cfg_db->conf_size[level]++;
    BCMI_KEYGEN_FUNC_EXIT();

exit:
    BCMI_KEYGEN_FREE(ext_cfg);
    BCMI_KEYGEN_FREE(ext_cfg_db);
    BCMI_KEYGEN_FUNC_EXIT();
}

STATIC int
bcmi_keygen_ext_section_cfg_add(int unit,
                     uint8 level,
                     bcmi_keygen_ext_cfg_t *new_ext_cfg,
                     bcmi_keygen_ext_cfg_db_t *ext_cfg_db)
{
    uint8 gran = 0;
    uint8 num_gran = 0;
    uint32 size = 0;
    bcmi_keygen_ext_section_t in_section;
    bcmi_keygen_ext_section_cfg_t *sec_cfg = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(new_ext_cfg, BCM_E_PARAM);
    BCMI_KEYGEN_NULL_CHECK(ext_cfg_db, BCM_E_PARAM);

    in_section = new_ext_cfg->in_sec;
    sec_cfg = ext_cfg_db->sec_cfg[in_section];
    gran = BCMI_KEYGEN_EXT_ID_GRAN_GET(new_ext_cfg->ext_id);
    if (sec_cfg == NULL) {
        size = sizeof(bcmi_keygen_ext_section_cfg_t);
        BCMI_KEYGEN_ALLOC(sec_cfg, size, "Extractor Section Config");
        sec_cfg->sec = new_ext_cfg->in_sec;
        sec_cfg->parent_sec = new_ext_cfg->in_sec;
        sec_cfg->drain_bits = gran;
        sec_cfg->ext_cfg_combinations = 1;
        ext_cfg_db->num_sec++;
        ext_cfg_db->sec_cfg[in_section] = sec_cfg;
    } else {
        sec_cfg->drain_bits += gran;
    }
    if (SHR_BITGET(new_ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_PASS_THRU)) {
        sec_cfg->flags |= BCMI_KEYGEN_EXT_SECTION_PASS_THRU;
    }
    if (SHR_BITGET(new_ext_cfg->ext_attrs.w,
        BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN)) {
        if (SHR_BITGET(new_ext_cfg->ext_attrs.w,
            BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_1)) {
            num_gran += 1;
        }
        if (SHR_BITGET(new_ext_cfg->ext_attrs.w,
            BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_2)) {
            num_gran += 1;
        }
        if (SHR_BITGET(new_ext_cfg->ext_attrs.w,
            BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_4)) {
            num_gran += 1;
        }
        if (SHR_BITGET(new_ext_cfg->ext_attrs.w,
            BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_8)) {
            num_gran += 1;
        }
        if (SHR_BITGET(new_ext_cfg->ext_attrs.w,
            BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_16)) {
            num_gran += 1;
        }
        if (SHR_BITGET(new_ext_cfg->ext_attrs.w,
            BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_32)) {
            num_gran += 1;
        }
        sec_cfg->ext_cfg_combinations *= num_gran;
        sec_cfg->flags |= BCMI_KEYGEN_EXT_SECTION_MULTI_GRAN;
    }
    BCMI_KEYGEN_FUNC_EXIT();

exit:
    BCMI_KEYGEN_FREE(sec_cfg);
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_cfg_insert(int unit,
                     uint8 level,
                     bcmi_keygen_ext_cfg_t *ext_cfg,
                     bcmi_keygen_ext_cfg_db_t *ext_cfg_db)
{
    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(ext_cfg, BCM_E_PARAM);

    if (level < BCMI_KEYGEN_EXT_LEVEL_COUNT) {
        BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
            bcmi_keygen_ext_cfg_add(unit, level, ext_cfg, ext_cfg_db));
    }

    BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(
        bcmi_keygen_ext_section_cfg_add(unit, level, ext_cfg, ext_cfg_db));

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_section_gran_info_init(int unit,
                              bcmi_keygen_md_t *keygen_md)
{
    uint8 idx = 0;
    uint8 part = 0;
    uint8 gran = 0;
    uint8 level = 0;
    uint8 found = 0;
    uint16 size = 0;
    uint8 *gran_info_count = 0;
    uint16 ext_idx = 0;
    bcmi_keygen_ext_section_t in_sec;
    bcmi_keygen_ext_section_t out_sec;
    bcmi_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmi_keygen_ext_section_cfg_t *sec_cfg = NULL;
    bcmi_keygen_ext_section_gran_t *gran_info = NULL;
    bcmi_keygen_ext_section_gran_info_t *section_gran_info = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(keygen_md, BCM_E_PARAM);
    size = sizeof(bcmi_keygen_ext_section_gran_info_t) *
                           BCMI_KEYGEN_EXT_SECTION_COUNT;
    sal_memset(keygen_md->section_gran_info, 0, size);

    for (level = 1; level < (keygen_md->num_ext_levels + 1); level++) {
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[level];
            ext_idx++) {
            ext_cfg = keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
            part = BCMI_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
            gran = BCMI_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
            in_sec = ext_cfg->in_sec;
            out_sec = ext_cfg->out_sec;
            sec_cfg = keygen_md->ext_cfg_db->sec_cfg[in_sec];
            if (gran == 32) {
                sec_cfg->flags |= BCMI_KEYGEN_EXT_SECTION_WITH_GRAN_32;
            } else if (gran == 16) {
                sec_cfg->flags |= BCMI_KEYGEN_EXT_SECTION_WITH_GRAN_16;
            } else if (gran == 8) {
                sec_cfg->flags |= BCMI_KEYGEN_EXT_SECTION_WITH_GRAN_8;
            } else if (gran == 4) {
                sec_cfg->flags |= BCMI_KEYGEN_EXT_SECTION_WITH_GRAN_4;
            } else if (gran == 2) {
                sec_cfg->flags |= BCMI_KEYGEN_EXT_SECTION_WITH_GRAN_2;
            } else if (gran == 1) {
                sec_cfg->flags |= BCMI_KEYGEN_EXT_SECTION_WITH_GRAN_1;
            }
            _func_rv =
                bcmi_keygen_ext_validate(unit, part, keygen_md, ext_cfg);
            if (BCMI_KEYGEN_FUNC_VAL_IS(BCM_E_CONFIG)) {
               continue;
            }
            BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(_func_rv);
            section_gran_info = &(keygen_md->section_gran_info[in_sec]);
            gran_info = &section_gran_info->out_gran_info[0];
            gran_info_count = &section_gran_info->out_gran_info_count;
            found = FALSE;
            for (idx = 0; idx < *gran_info_count; idx++) {
                if (gran_info[idx].gran == gran) {
                    found = TRUE;
                    gran_info[idx].num_extractors++;
                    break;
                }
            }
            if (found == FALSE) {
                gran_info[*gran_info_count].gran = gran;
                gran_info[*gran_info_count].num_extractors++;
                (*gran_info_count)++;
            }
           found = FALSE;
           section_gran_info = &(keygen_md->section_gran_info[out_sec]);
           gran_info = &section_gran_info->in_gran_info[0];
           gran_info_count = &section_gran_info->in_gran_info_count;
            for (idx = 0; idx < *gran_info_count; idx++) {
                if (gran_info[idx].gran == gran) {
                    found = TRUE;
                    gran_info[idx].num_extractors++;
                    break;
                }
            }
            if (found == FALSE) {
                gran_info[*gran_info_count].gran = gran;
                gran_info[*gran_info_count].num_extractors++;
                (*gran_info_count)++;
            }
         }
    }

    BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_NONE);

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_sec_dbit_set(int unit,
                    bcmi_keygen_ext_section_t sec,
                    int drain_bits,
                    bcmi_keygen_ext_cfg_db_t *ext_cfg_db)
{
    int size = 0;
    bcmi_keygen_ext_section_cfg_t *sec_cfg = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    if ((ext_cfg_db == NULL) ||
        (sec >= BCMI_KEYGEN_EXT_SECTION_COUNT)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    sec_cfg = ext_cfg_db->sec_cfg[sec];
    if (sec_cfg == NULL) {
        size = sizeof(bcmi_keygen_ext_section_cfg_t);
        BCMI_KEYGEN_ALLOC(sec_cfg, size, "Extractor Section Config");
        sec_cfg->sec = sec;
        ext_cfg_db->sec_cfg[sec] = sec_cfg;
    }

    sec_cfg->drain_bits = drain_bits;

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_sec_parent_set(int unit,
                    bcmi_keygen_ext_section_t sec,
                    bcmi_keygen_ext_section_t parent_sec,
                    bcmi_keygen_ext_cfg_db_t *ext_cfg_db)
{
    bcmi_keygen_ext_section_cfg_t *sec_cfg = NULL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    if ((ext_cfg_db == NULL) ||
        (sec >= BCMI_KEYGEN_EXT_SECTION_COUNT) ||
        (parent_sec >= BCMI_KEYGEN_EXT_SECTION_COUNT)) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    sec_cfg = ext_cfg_db->sec_cfg[sec];
    if (sec_cfg == NULL) {
        BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_PARAM);
    }
    sec_cfg->parent_sec = parent_sec;

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

int
bcmi_keygen_ext_cfg_db_cleanup(int unit,
                               bcmi_keygen_ext_cfg_db_t *ext_cfg_db)
{
    uint8 level = 0;
    bcmi_keygen_ext_section_t section;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_NULL_CHECK(ext_cfg_db, BCM_E_PARAM);

    for (level = 0; level < BCMI_KEYGEN_EXT_LEVEL_COUNT; level++) {
        BCMI_KEYGEN_FREE(ext_cfg_db->ext_cfg[level]);
    }

    for (section = 0; section < BCMI_KEYGEN_EXT_SECTION_COUNT; section++) {
        BCMI_KEYGEN_FREE(ext_cfg_db->sec_cfg[section]);
    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}
