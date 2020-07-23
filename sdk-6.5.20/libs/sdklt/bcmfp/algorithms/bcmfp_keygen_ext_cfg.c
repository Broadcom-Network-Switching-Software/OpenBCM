/*! \file bcmfp_keygen_ext_cfg.c
 *
 * Utility functions to operate on Group
 * extractor configurations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "bcmfp_keygen_md.h"

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_keygen_ext_cfg_db_free(int unit, bcmfp_keygen_ext_cfg_db_t *ext_cfg_db)
{
    int level;
    int ext_sec;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ext_cfg_db, SHR_E_PARAM);

    /* Free extractor sec_cfg memory. */
    for (ext_sec = BCMFP_KEYGEN_EXT_SECTION_L1E32;
         ext_sec < BCMFP_KEYGEN_EXT_SECTION_COUNT;
         ext_sec++) {
        SHR_FREE(ext_cfg_db->sec_cfg[ext_sec]);
    }

    /* Free extractors configuration memory for all levels. */
    for (level = 1; level < BCMFP_KEYGEN_EXT_LEVEL_COUNT; level++) {
        SHR_FREE(ext_cfg_db->ext_cfg[level]);
    }

    SHR_FREE(ext_cfg_db);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_cfg_reset(int unit,
                        bcmfp_keygen_cfg_t *keygen_cfg,
                        bcmfp_keygen_md_t *keygen_md)
{
    bcmfp_keygen_ext_cfg_db_t *orig_ext_info = NULL; /* Extractor info original list. */
    int alloc_sz;                            /* Memory allocation size.       */
    int level;                               /* Extractor hierarcy level.     */
    int num_sec;                             /* Number of Sections.           */
    bcmfp_keygen_ext_section_t ext_sel;         /* Extractor selector type.      */
    bcmfp_keygen_ext_section_cfg_t *org_ext_secs;     /* Original extractor sec_cfg.  */
    bcmfp_keygen_ext_cfg_db_t **ext_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    /* Get extractor information for the selected extractor mode. */
    orig_ext_info = keygen_cfg->ext_cfg_db;
    /* Confirm original extractor info list is initialized. */
    if (NULL == orig_ext_info) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    ext_info = &(keygen_md->ext_cfg_db);
    if (*ext_info == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Set the extractor mode. */
    (*ext_info)->mode = keygen_cfg->ext_cfg_db->mode;

    /* Copy the extractor hierarchy section list for the selected mode. */
    for (level = 1; level < keygen_md->num_ext_levels; level++) {
        alloc_sz = orig_ext_info->conf_size[level];

        /* Confirm extractor configuration has a valid size. */
        if (0 == alloc_sz) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }

        if ((*ext_info)->ext_cfg[level] == NULL) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* Copy extractor configuration for current level. */
        sal_memcpy((*ext_info)->ext_cfg[level], orig_ext_info->ext_cfg[level],
            alloc_sz * sizeof(bcmfp_keygen_ext_cfg_t));

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
        for (ext_sel = BCMFP_KEYGEN_EXT_SECTION_L1E32;
             ext_sel < BCMFP_KEYGEN_EXT_SECTION_COUNT; ext_sel++) {

            org_ext_secs = orig_ext_info->sec_cfg[ext_sel];
            if (NULL != org_ext_secs) {

                if ((*ext_info)->sec_cfg[ext_sel] == NULL) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }

                /* Copy extractor information. */
                sal_memcpy((*ext_info)->sec_cfg[ext_sel], org_ext_secs,
                    sizeof(bcmfp_keygen_ext_section_cfg_t));

                /* Keep track of how many sec_cfg are yet to be copied. */
                num_sec--;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_cfg_reset_part(int unit,
                             uint8_t part,
                             bcmfp_keygen_cfg_t *keygen_cfg,
                             bcmfp_keygen_md_t *keygen_md)
{
    uint8_t level = 0;
    uint8_t ext_part = 0;
    uint8_t multi_gran_present = FALSE;
    uint16_t ext_idx = 0;
    bcmfp_keygen_ext_section_t section;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmfp_keygen_ext_section_cfg_t *sec_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    for (section = BCMFP_KEYGEN_EXT_SECTION_L1E32;
         section < BCMFP_KEYGEN_EXT_SECTION_COUNT; section++) {
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
            ext_part = BCMFP_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
            if (part != ext_part) {
                continue;
            }
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                sec_cfg = keygen_md->ext_cfg_db->sec_cfg[ext_cfg->in_sec];
                sec_cfg->fill_bits +=
                             BCMFP_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
                continue;
            }
            if (SHR_BITGET(ext_cfg->ext_attrs.w,
                BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN)) {
                multi_gran_present = TRUE;
                if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_1)) {
                    BCMFP_KEYGEN_EXT_ID_GRAN_SET(ext_cfg->ext_id, 1);
                } else if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_2)) {
                    BCMFP_KEYGEN_EXT_ID_GRAN_SET(ext_cfg->ext_id, 2);
                } else if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_4)) {
                    BCMFP_KEYGEN_EXT_ID_GRAN_SET(ext_cfg->ext_id, 4);
                } else if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_8)) {
                    BCMFP_KEYGEN_EXT_ID_GRAN_SET(ext_cfg->ext_id, 8);
                } else if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_16)) {
                    BCMFP_KEYGEN_EXT_ID_GRAN_SET(ext_cfg->ext_id, 16);
                } else if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_32)) {
                    BCMFP_KEYGEN_EXT_ID_GRAN_SET(ext_cfg->ext_id, 32);
                } else {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
            ext_cfg->in_use = FALSE;
            ext_cfg->finfo = NULL;
        }
    }
    if (multi_gran_present == TRUE) {
        (void)bcmfp_keygen_ext_section_gran_info_init(unit, keygen_md);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_cfg_update(int unit,
                        bcmfp_keygen_cfg_t *keygen_cfg,
                        bcmfp_keygen_md_t *keygen_md)
{
    uint8_t level = 0;
    uint8_t ext_gran = 0;
    uint8_t ext_part = 0;
    uint16_t ext_num = 0;
    uint16_t idx = 0;
    uint16_t ext_id = 0;
    uint16_t pt_ext_id = 0;
    bcmfp_keygen_ext_section_t out_section;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmfp_keygen_ext_section_cfg_t *sec_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);

    if (keygen_cfg->ext_ctrl_sel_info == NULL ||
        keygen_cfg->ext_ctrl_sel_info_count == 0) {
        SHR_EXIT();
    }

    /* ext_ctrl_sel_info can be NULL in case of QSET update
     * if earlier calls to keygen has no qualifiers present
     * in keygen configuration.
     */
    for (idx = 0; idx < keygen_cfg->ext_ctrl_sel_info_count; idx++) {
        if (keygen_cfg->ext_ctrl_sel_info[idx].ctrl_sel ==
                        BCMFP_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER) {
            level = keygen_cfg->ext_ctrl_sel_info[idx].level;
            for (ext_id = 0;
                 ext_id < keygen_md->ext_cfg_db->conf_size[level];
                 ext_id++) {
                ext_cfg = keygen_md->ext_cfg_db->ext_cfg[level] + ext_id;
                ext_part = BCMFP_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
                ext_gran = BCMFP_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
                ext_num = BCMFP_KEYGEN_EXT_ID_NUM_GET(ext_cfg->ext_id);
                if (ext_part == keygen_cfg->ext_ctrl_sel_info[idx].part &&
                    ext_gran == keygen_cfg->ext_ctrl_sel_info[idx].gran &&
                    ext_num == keygen_cfg->ext_ctrl_sel_info[idx].ext_num) {
                    ext_cfg->in_use = TRUE;
                    SHR_BITSET(ext_cfg->ext_attrs.w,
                         BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE);
                    out_section = ext_cfg->out_sec[0];
                    sec_cfg = keygen_md->ext_cfg_db->sec_cfg[out_section];
                    if (sec_cfg->flags & BCMFP_KEYGEN_EXT_SECTION_PASS_THRU) {
                        for (pt_ext_id = 0;
                             (pt_ext_id <
                             keygen_md->ext_cfg_db->conf_size[level + 1]);
                             pt_ext_id++) {
                             ext_cfg =
                                 (keygen_md->ext_cfg_db->ext_cfg[level + 1] +
                                  pt_ext_id);
                             ext_part =
                                 BCMFP_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
                             ext_gran =
                                 BCMFP_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
                             ext_num =
                                 BCMFP_KEYGEN_EXT_ID_NUM_GET(ext_cfg->ext_id);
                             if ((ext_part ==
                                  keygen_cfg->ext_ctrl_sel_info[idx].part) &&
                                 (ext_gran ==
                                  keygen_cfg->ext_ctrl_sel_info[idx].gran) &&
                                 (ext_num ==
                                  keygen_cfg->ext_ctrl_sel_info[idx].ext_num)) {
                                 ext_cfg->in_use = TRUE;
                                 SHR_BITSET(ext_cfg->ext_attrs.w,
                                            BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE);
                             }
                        }
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_section_drain_bits_update(int unit,
                       bcmfp_keygen_md_t *keygen_md)
{
    uint8_t level = 0;
    uint8_t ext_gran = 0;
    uint8_t ext_part = 0;
    uint16_t idx = 0;
    uint16_t ext_id = 0;
    uint16_t attr_enum_count = 0;
    bcmfp_keygen_ext_section_t in_section;
    bcmfp_keygen_ext_section_t out_section;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmfp_keygen_ext_section_cfg_t *sec_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);

    /* Number of enumerations in extractor attributes and acl attributes
     * are same and have 1-to-1 mapping.
     */
    attr_enum_count = BCMFP_KEYGEN_EXT_ATTR_COUNT;
    for (level = 0; level < keygen_md->num_ext_levels; level++) {
        for (ext_id = 0;
            ext_id < keygen_md->ext_cfg_db->conf_size[level + 1];
            ext_id++) {
            ext_cfg = keygen_md->ext_cfg_db->ext_cfg[level + 1] + ext_id;
            ext_gran = BCMFP_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
            ext_part = BCMFP_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
            for (idx = 0; idx < attr_enum_count; idx++) {
                if (SHR_BITGET(keygen_md->acl_attrs[ext_part].w, idx) &&
                    SHR_BITGET(ext_cfg->ext_attrs.w, idx)) {
                    in_section = ext_cfg->in_sec;
                    sec_cfg = keygen_md->ext_cfg_db->sec_cfg[in_section];
                    sec_cfg->drain_bits -= ext_gran;
                    if (level == (keygen_md->num_ext_levels - 1)) {
                        out_section = ext_cfg->out_sec[0];
                        sec_cfg = keygen_md->ext_cfg_db->sec_cfg[out_section];
                        sec_cfg->drain_bits -= ext_gran;
                    }
                    break;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_cfg_alloc(int unit,
                        bcmfp_keygen_cfg_t *keygen_cfg,
                        bcmfp_keygen_md_t *keygen_md)
{
    int alloc_sz;                            /* Memory allocation size.       */
    int idx;                                 /* Iterator Index.               */
    int level;                               /* Extractor hierarcy level.     */
    int num_sec;                             /* Number of Sections.           */
    bcmfp_keygen_ext_section_t ext_sel;         /* Extractor selector type.      */
    bcmfp_keygen_ext_section_cfg_t *org_ext_secs;     /* Original extractor sec_cfg.  */
    bcmfp_keygen_ext_cfg_db_t *orig_ext_info = NULL; /* Extractor info original list. */
    bcmfp_keygen_ext_cfg_db_t **ext_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);

    ext_info = &(keygen_md->ext_cfg_db);

    /* Get extractor information for the selected extractor mode. */
    orig_ext_info = keygen_cfg->ext_cfg_db;

    /* Confirm original extractor info list is initialized. */
    if (NULL == orig_ext_info) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate extractor info memory. */
    BCMFP_ALLOC(*ext_info, sizeof(bcmfp_keygen_ext_cfg_db_t), "bcmfpIngExtCfgDb");

    /* Set the extractor mode. */
    (*ext_info)->mode = keygen_cfg->ext_cfg_db->mode;

    keygen_md->num_ext_levels = orig_ext_info->num_ext_levels - 1;
    /* Copy the extractor hierarchy section list for the selected mode. */
    for (level = 1; level < orig_ext_info->num_ext_levels; level++) {
        alloc_sz = orig_ext_info->conf_size[level];
        /* Confirm extractor configuration has a valid size. */
        if (0 == alloc_sz) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        /* Allocate memory for extractor configurations in input bus list. */
        BCMFP_ALLOC((*ext_info)->ext_cfg[level],
            (alloc_sz * sizeof(bcmfp_keygen_ext_cfg_t)), "bcmfpIngExtCfg");
        /* Copy extractor configuration for current level. */
        sal_memcpy((*ext_info)->ext_cfg[level], orig_ext_info->ext_cfg[level],
            alloc_sz * sizeof(bcmfp_keygen_ext_cfg_t));
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
        for (ext_sel = BCMFP_KEYGEN_EXT_SECTION_L1E32;
             ext_sel < BCMFP_KEYGEN_EXT_SECTION_COUNT; ext_sel++) {
            org_ext_secs = orig_ext_info->sec_cfg[ext_sel];
            if (NULL != org_ext_secs) {
                /* Allocate memory for extractor sec_cfg. */
                BCMFP_ALLOC((*ext_info)->sec_cfg[ext_sel],
                    sizeof(bcmfp_keygen_ext_section_cfg_t), "bcmfpIngExtSecCfg");
                /* Copy extractor information. */
                sal_memcpy((*ext_info)->sec_cfg[ext_sel], org_ext_secs,
                    sizeof(bcmfp_keygen_ext_section_cfg_t));
                /* Keep track of how many sec_cfg are yet to be copied. */
                num_sec--;
            }
        }
    }

    /* If preparing keygen for QSET update. */
    if (keygen_cfg->flags & BCMFP_KEYGEN_CFG_FLAGS_QSET_UPDATE) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_keygen_ext_cfg_update(unit, keygen_cfg, keygen_md));
    }
    SHR_FUNC_EXIT();
exit:
    if (ext_info != NULL) {
        if (*ext_info != NULL) {
            for (idx = 1; idx < BCMFP_KEYGEN_EXT_LEVEL_COUNT; idx++) {
                SHR_FREE((*ext_info)->ext_cfg[idx]);
            }
            for (idx = 0; idx < BCMFP_KEYGEN_EXT_SECTION_COUNT; idx++) {
                SHR_FREE((*ext_info)->sec_cfg[idx]);
            }
            SHR_FREE(*ext_info);
        }
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_sections_get(int unit,
                           bcmfp_keygen_cfg_t *keygen_cfg,
                           bcmfp_keygen_md_t *keygen_md)
{
    uint8_t part = 0;
    uint8_t level = 0;
    uint16_t ext_idx = 0;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    for (level = 1; level <= keygen_md->num_ext_levels; level++) {
        for (ext_idx = 0;
             ext_idx < keygen_md->ext_cfg_db->conf_size[level];
             ext_idx++) {
             ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
             part = BCMFP_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
             keygen_md->sections[part][level -1][ext_cfg->in_sec] =
                                                      ext_cfg->in_sec;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_cfg_t_init(int unit, bcmfp_keygen_ext_cfg_t *ext_cfg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ext_cfg, SHR_E_PARAM);
    sal_memset(ext_cfg, 0, sizeof(bcmfp_keygen_ext_cfg_t));
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_ext_cfg_add(int unit,
                 uint8_t level,
                 bcmfp_keygen_ext_cfg_t *new_ext_cfg,
                 bcmfp_keygen_ext_cfg_db_t *ext_cfg_db)
{
    uint32_t size = 0;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(new_ext_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(ext_cfg_db, SHR_E_PARAM);

    size = (ext_cfg_db->conf_size[level] + 1)  * sizeof(bcmfp_keygen_ext_cfg_t);
    BCMFP_ALLOC(ext_cfg, size, "bcmfpIngExtCfg");

    if (ext_cfg_db->ext_cfg[level] != NULL) {
        size -= sizeof(bcmfp_keygen_ext_cfg_t);
        sal_memcpy(ext_cfg, ext_cfg_db->ext_cfg[level], size);
        SHR_FREE(ext_cfg_db->ext_cfg[level]);
    } else {
        ext_cfg_db->num_ext_levels++;
    }

    ext_cfg_db->ext_cfg[level] = ext_cfg;
    ext_cfg_db->ext_cfg[level][ext_cfg_db->conf_size[level]] = *new_ext_cfg;
    ext_cfg_db->conf_size[level]++;
    SHR_FUNC_EXIT();

exit:
    SHR_FREE(ext_cfg);
    SHR_FREE(ext_cfg_db);
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_keygen_ext_section_cfg_add(int unit,
                     uint8_t level,
                     bcmfp_keygen_ext_cfg_t *new_ext_cfg,
                     bcmfp_keygen_ext_cfg_db_t *ext_cfg_db)
{
    uint16_t gran = 0;
    uint8_t num_gran = 0;
    uint32_t size = 0;
    bcmfp_keygen_ext_section_t in_section;
    bcmfp_keygen_ext_section_cfg_t *sec_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(new_ext_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(ext_cfg_db, SHR_E_PARAM);

    in_section = new_ext_cfg->in_sec;
    sec_cfg = ext_cfg_db->sec_cfg[in_section];
    gran = BCMFP_KEYGEN_EXT_ID_GRAN_GET(new_ext_cfg->ext_id);
    if (sec_cfg == NULL) {
        size = sizeof(bcmfp_keygen_ext_section_cfg_t);
        BCMFP_ALLOC(sec_cfg, size, "bcmfpIngExtSecCfg");
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
        BCMFP_KEYGEN_EXT_ATTR_PASS_THRU)) {
        sec_cfg->flags |= BCMFP_KEYGEN_EXT_SECTION_PASS_THRU;
    }
    if (SHR_BITGET(new_ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN)) {
        if (SHR_BITGET(new_ext_cfg->ext_attrs.w,
            BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_1)) {
            num_gran += 1;
        }
        if (SHR_BITGET(new_ext_cfg->ext_attrs.w,
            BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_2)) {
            num_gran += 1;
        }
        if (SHR_BITGET(new_ext_cfg->ext_attrs.w,
            BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_4)) {
            num_gran += 1;
        }
        if (SHR_BITGET(new_ext_cfg->ext_attrs.w,
            BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_8)) {
            num_gran += 1;
        }
        if (SHR_BITGET(new_ext_cfg->ext_attrs.w,
            BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_16)) {
            num_gran += 1;
        }
        if (SHR_BITGET(new_ext_cfg->ext_attrs.w,
            BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_32)) {
            num_gran += 1;
        }
        sec_cfg->ext_cfg_combinations *= num_gran;
        sec_cfg->flags |= BCMFP_KEYGEN_EXT_SECTION_MULTI_GRAN;
    }
    sec_cfg->level = (level - 1);
    SHR_FUNC_EXIT();

exit:
    SHR_FREE(sec_cfg);
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_cfg_insert(int unit,
                     bcmfp_keygen_ext_cfg_t *ext_cfg,
                     bcmfp_keygen_ext_cfg_db_t *ext_cfg_db)
{
    uint8_t level = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ext_cfg, SHR_E_PARAM);

    level = BCMFP_KEYGEN_EXT_ID_LEVEL_GET(ext_cfg->ext_id);

    if (level < BCMFP_KEYGEN_EXT_LEVEL_COUNT) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_keygen_ext_cfg_add(unit, level, ext_cfg, ext_cfg_db));
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_keygen_ext_section_cfg_add(unit, level, ext_cfg, ext_cfg_db));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_section_gran_info_init(int unit,
                              bcmfp_keygen_md_t *keygen_md)
{
    int rv;
    uint8_t idx = 0;
    uint8_t part = 0;
    uint8_t gran = 0;
    uint8_t level = 0;
    uint8_t found = 0;
    uint16_t size = 0;
    uint8_t *gran_info_count = 0;
    uint16_t ext_idx = 0;
    bcmfp_keygen_ext_section_t in_sec;
    bcmfp_keygen_ext_section_t out_sec;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmfp_keygen_ext_section_cfg_t *sec_cfg = NULL;
    bcmfp_keygen_ext_section_gran_t *gran_info = NULL;
    bcmfp_keygen_ext_section_gran_info_t *section_gran_info = NULL;
    uint8_t out_sec_idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    size = sizeof(bcmfp_keygen_ext_section_gran_info_t) *
                           BCMFP_KEYGEN_EXT_SECTION_COUNT;
    sal_memset(keygen_md->section_gran_info, 0, size);

    for (level = 1; level < (keygen_md->num_ext_levels + 1); level++) {
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[level];
            ext_idx++) {
            ext_cfg = keygen_md->ext_cfg_db->ext_cfg[level] + ext_idx;
            part = BCMFP_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
            gran = BCMFP_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
            in_sec = ext_cfg->in_sec;
            sec_cfg = keygen_md->ext_cfg_db->sec_cfg[in_sec];
            if (gran == 32) {
                sec_cfg->flags |= BCMFP_KEYGEN_EXT_SECTION_WITH_GRAN_32;
            } else if (gran == 16) {
                sec_cfg->flags |= BCMFP_KEYGEN_EXT_SECTION_WITH_GRAN_16;
            } else if (gran == 8) {
                sec_cfg->flags |= BCMFP_KEYGEN_EXT_SECTION_WITH_GRAN_8;
            } else if (gran == 4) {
                sec_cfg->flags |= BCMFP_KEYGEN_EXT_SECTION_WITH_GRAN_4;
            } else if (gran == 2) {
                sec_cfg->flags |= BCMFP_KEYGEN_EXT_SECTION_WITH_GRAN_2;
            } else if (gran == 1) {
                sec_cfg->flags |= BCMFP_KEYGEN_EXT_SECTION_WITH_GRAN_1;
            }

            rv = bcmfp_keygen_ext_validate(unit, part, keygen_md, ext_cfg);
            if (rv == SHR_E_CONFIG) {
               continue;
            }
            SHR_IF_ERR_VERBOSE_EXIT(rv);
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
            for (out_sec_idx = 0;
                 out_sec_idx < BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS;
                 out_sec_idx++) {
                out_sec = ext_cfg->out_sec[out_sec_idx];
                if (out_sec == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
                   continue;
                }
                if (ext_cfg->onehot_ext_id[out_sec_idx] != 0) {
                    keygen_md->ext_cfg_db->sec_cfg[out_sec]->flags |=
                                       BCMFP_KEYGEN_EXT_SECTION_ONEHOT;
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
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_sec_dbit_set(int unit,
                    bcmfp_keygen_ext_section_t sec,
                    int drain_bits,
                    bcmfp_keygen_ext_cfg_db_t *ext_cfg_db)
{
    int size = 0;
    bcmfp_keygen_ext_section_cfg_t *sec_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    if ((ext_cfg_db == NULL) ||
        (sec >= BCMFP_KEYGEN_EXT_SECTION_COUNT)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sec_cfg = ext_cfg_db->sec_cfg[sec];
    if (sec_cfg == NULL) {
        size = sizeof(bcmfp_keygen_ext_section_cfg_t);
        BCMFP_ALLOC(sec_cfg, size, "bcmfpIngExtSecCfg");
        sec_cfg->sec = sec;
        ext_cfg_db->sec_cfg[sec] = sec_cfg;
    }

    sec_cfg->drain_bits = drain_bits;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_sec_parent_set(int unit,
                    bcmfp_keygen_ext_section_t sec,
                    bcmfp_keygen_ext_section_t parent_sec,
                    bcmfp_keygen_ext_cfg_db_t *ext_cfg_db)
{
    bcmfp_keygen_ext_section_cfg_t *sec_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    if ((ext_cfg_db == NULL) ||
        (sec >= BCMFP_KEYGEN_EXT_SECTION_COUNT) ||
        (parent_sec >= BCMFP_KEYGEN_EXT_SECTION_COUNT)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sec_cfg = ext_cfg_db->sec_cfg[sec];
    if (sec_cfg == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    sec_cfg->parent_sec = parent_sec;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_cfg_db_cleanup(int unit,
                               bcmfp_keygen_ext_cfg_db_t *ext_cfg_db)
{
    uint8_t level = 0;
    bcmfp_keygen_ext_section_t section;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ext_cfg_db, SHR_E_PARAM);

    for (level = 0; level < BCMFP_KEYGEN_EXT_LEVEL_COUNT; level++) {
        SHR_FREE(ext_cfg_db->ext_cfg[level]);
    }

    for (section = 0; section < BCMFP_KEYGEN_EXT_SECTION_COUNT; section++) {
        SHR_FREE(ext_cfg_db->sec_cfg[section]);
    }

exit:
    SHR_FUNC_EXIT();
}
