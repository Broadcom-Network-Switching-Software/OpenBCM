/*! \file bcmfp_keygen_ext_core.c
 *
 * Core Algorithmic APIS to allocate extractors.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "bcmfp_keygen_md.h"

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

#define BCMFP_KEYGEN_EXT_FINFO_COUNT (1024 * 1)

static bcmfp_keygen_ext_field_info_t *finfo_free_pool[BCMDRD_CONFIG_MAX_UNITS];

static bcmfp_keygen_ext_field_info_t finfo_pool[BCMDRD_CONFIG_MAX_UNITS]
                                               [BCMFP_KEYGEN_EXT_FINFO_COUNT];

static uint8_t debug_enable = 0;
static uint16_t finfo_free_count = 0;
static uint16_t finfo_used_count = 0;

#define DUMP_FINFO_COUNT(free) {  \
    if (free) {                   \
        if (debug_enable) {       \
            finfo_free_count++;   \
            finfo_used_count--;   \
        }                         \
    } else {                      \
        if (debug_enable) {       \
            finfo_free_count--;   \
            finfo_used_count++;   \
        }                         \
    }                             \
}

#define SP2 "  "
#define SP4 "    "
#define SP6 "      "
#define SP8 "        "

static int
bcmfp_keygen_ext_cfg_dump(int unit,
                          bcmfp_keygen_md_t *keygen_md,
                          bcmfp_keygen_ext_cfg_t *ext_cfg)
{
    int g = 0, p = 0, l = 0;
    uint16_t ext_num = 0;
    uint8_t idx = 0;
    bcmfp_keygen_ext_cfg_t *onehot_ext_cfg = NULL;
    bcmfp_keygen_ext_cfg_t *conflict_ext_cfg = NULL;
    bcmfp_keygen_ext_cfg_t *parent_ext_cfg = NULL;
    uint16_t bit_count = 0;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;

    SHR_FUNC_ENTER(unit);

    if (ext_cfg == NULL) {
       LOG_DEBUG(BSL_LOG_MODULE,
                (BSL_META_U(unit, "NULL extractor configuration\n")));
    }

    bit_count = (sizeof(uint32_t) * BCMFP_KEYGEN_EXT_FIELD_WORD_COUNT * 8);
    g = BCMFP_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
    p = BCMFP_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
    l = BCMFP_KEYGEN_EXT_ID_LEVEL_GET(ext_cfg->ext_id);
    ext_num = BCMFP_KEYGEN_EXT_ID_NUM_GET(ext_cfg->ext_id);
    LOG_DEBUG(BSL_LOG_MODULE,
             (BSL_META_U(unit, "[Id - %d, Part - %d, Level - %d]\n"),
             ext_num, p, l));

    LOG_DEBUG(BSL_LOG_MODULE,
             (BSL_META_U(unit, "%sInSection - %d\n"), SP2, ext_cfg->in_sec));

    LOG_DEBUG(BSL_LOG_MODULE,
             (BSL_META_U(unit, "%sGranularity- %d\n"), SP2, g));

    finfo = ext_cfg->finfo;

    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
             "%sChunk Section ID - %d\n"), SP2, finfo->sec_val));

    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
             "%sChunk Size - %d\n"), SP2, finfo->size));

    for (idx = 0; idx < BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS; idx++) {
        if (ext_cfg->out_sec[idx] == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
            continue;
        }
        LOG_DEBUG(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "%sOutSection-%d - %d\n"),
                  SP2, idx, ext_cfg->out_sec[idx]));

        LOG_DEBUG(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "%sOffset - %d\n"),
                  SP4, ext_cfg->offset[idx]));

        if (SHR_BITNULL_RANGE(&finfo->req_bmp[idx][0], 0, bit_count) == 0) {
            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                     "%sreq_bmp - 0x%X\n"), SP4, finfo->req_bmp[idx][0]));
            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                     "%sext_bmp - 0x%X\n"), SP4, finfo->ext_bmp[idx][0]));
        }

        if (ext_cfg->onehot_ext_id[idx] != 0) {

            SHR_IF_ERR_EXIT
                (bcmfp_keygen_ext_cfg_get(unit,
                                          keygen_md,
                                          ext_cfg->onehot_ext_id[idx],
                                          &onehot_ext_cfg));
            LOG_DEBUG(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "%sONEHOT_EXT - Exists\n"), SP4));

            g = BCMFP_KEYGEN_EXT_ID_GRAN_GET(onehot_ext_cfg->ext_id);
            p = BCMFP_KEYGEN_EXT_ID_PART_GET(onehot_ext_cfg->ext_id);
            l = BCMFP_KEYGEN_EXT_ID_LEVEL_GET(onehot_ext_cfg->ext_id);
            ext_num = BCMFP_KEYGEN_EXT_ID_NUM_GET(onehot_ext_cfg->ext_id);

            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                      "%s[Id - %d, Part - %d, Level - %d]\n"),
                      SP6, ext_num, p, l));
            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                      "%sInsection - %d\n"), SP8, onehot_ext_cfg->in_sec));
            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                     "%sGranularity - %d\n"), SP8, g));
            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                     "%sOutSection - %d\n"), SP8, onehot_ext_cfg->out_sec[0]));
            onehot_ext_cfg = NULL;
        }

        if (ext_cfg->conflict_ext_id[idx] != 0) {
            SHR_IF_ERR_EXIT
                (bcmfp_keygen_ext_cfg_get(unit,
                                          keygen_md,
                                          ext_cfg->conflict_ext_id[idx],
                                          &conflict_ext_cfg));
            LOG_DEBUG(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "%sCONFLICT_EXT - Exists\n"), SP4));

            g = BCMFP_KEYGEN_EXT_ID_GRAN_GET(conflict_ext_cfg->ext_id);
            p = BCMFP_KEYGEN_EXT_ID_PART_GET(conflict_ext_cfg->ext_id);
            l = BCMFP_KEYGEN_EXT_ID_LEVEL_GET(conflict_ext_cfg->ext_id);
            ext_num = BCMFP_KEYGEN_EXT_ID_NUM_GET(conflict_ext_cfg->ext_id);

            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                      "%s[Id - %d, Part - %d, Level - %d]\n"),
                      SP6, ext_num, p, l));
            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                      "%sInsection - %d\n"), SP8, conflict_ext_cfg->in_sec));
            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                     "%sGranularity - %d\n"), SP8, g));
            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                     "%sOutSection - %d\n"), SP8,
                     conflict_ext_cfg->out_sec[0]));
            conflict_ext_cfg = NULL;
        }
    }

    if (ext_cfg->parent_ext_id != 0) {
        SHR_IF_ERR_EXIT
            (bcmfp_keygen_ext_cfg_get(unit,
                                      keygen_md,
                                      ext_cfg->parent_ext_id,
                                      &parent_ext_cfg));
        LOG_DEBUG(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "%sPARENT_EXT - Exists\n"), SP2));

        g = BCMFP_KEYGEN_EXT_ID_GRAN_GET(parent_ext_cfg->ext_id);
        p = BCMFP_KEYGEN_EXT_ID_PART_GET(parent_ext_cfg->ext_id);
        l = BCMFP_KEYGEN_EXT_ID_LEVEL_GET(parent_ext_cfg->ext_id);
        ext_num = BCMFP_KEYGEN_EXT_ID_NUM_GET(parent_ext_cfg->ext_id);

        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                  "%s[Id - %d, Part - %d, Level - %d]\n"),
                  SP4, ext_num, p, l));
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                  "%sInsection - %d\n"), SP6, parent_ext_cfg->in_sec));
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                 "%sGranularity - %d\n"), SP6, g));
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                 "%sOutSection - %d\n"), SP6, parent_ext_cfg->out_sec[0]));
        parent_ext_cfg = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_keygen_ext_debug_info_print_3(int unit,
                                    uint8_t part,
                                    uint8_t level,
                                    bcmfp_keygen_ext_section_t section,
                                    bcmfp_keygen_cfg_t *keygen_cfg,
                                    bcmfp_keygen_md_t *keygen_md,
                                    bcmfp_keygen_oper_t *keygen_oper)
{
    int p = 0, l = 0;
    uint16_t ext_idx = 0;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    LOG_DEBUG(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Debug information of "
              "Part - %d Level - %d Section - %d\n"),
              part, level, section));

    for (ext_idx = 0;
        ext_idx < keygen_md->ext_cfg_db->conf_size[level + 1];
        ext_idx++) {
        ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level + 1] + ext_idx;
        p = BCMFP_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
        l = BCMFP_KEYGEN_EXT_ID_LEVEL_GET(ext_cfg->ext_id);
        if ((p != part) || (l != (level + 1)) ||
            (ext_cfg->in_use != TRUE) ||
            (section != ext_cfg->in_sec)) {
           continue;
        }
        SHR_IF_ERR_EXIT
            (bcmfp_keygen_ext_cfg_dump(unit, keygen_md, ext_cfg));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_keygen_ext_debug_info_print_2(int unit,
                                    uint8_t part,
                                    uint8_t level,
                                    bcmfp_keygen_cfg_t *keygen_cfg,
                                    bcmfp_keygen_md_t *keygen_md,
                                    bcmfp_keygen_oper_t *keygen_oper)
{
    uint8_t sec_idx = 0;
    bcmfp_keygen_ext_section_t section;

    SHR_FUNC_ENTER(unit);

    for (sec_idx = 0;
         sec_idx < BCMFP_KEYGEN_EXT_SECTION_COUNT;
         sec_idx++) {

        section = keygen_md->sections[part][level][sec_idx];

        if (section == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
            continue;
        }
        SHR_IF_ERR_EXIT
            (bcmfp_keygen_ext_debug_info_print_3(unit,
                                                  part,
                                                  level,
                                                  section,
                                                  keygen_cfg,
                                                  keygen_md,
                                                  keygen_oper));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_keygen_ext_debug_info_print_1(int unit,
                                    uint8_t part,
                                    bcmfp_keygen_cfg_t *keygen_cfg,
                                    bcmfp_keygen_md_t *keygen_md,
                                    bcmfp_keygen_oper_t *keygen_oper)
{
    uint8_t level = 0;

    SHR_FUNC_ENTER(unit);


    for (level = 0; level < keygen_md->num_ext_levels; level++) {
        SHR_IF_ERR_EXIT
            (bcmfp_keygen_ext_debug_info_print_2(unit,
                                                 part,
                                                 level,
                                                 keygen_cfg,
                                                 keygen_md,
                                                 keygen_oper));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_keygen_ext_debug_info_print(int unit,
                                  bcmfp_keygen_cfg_t *keygen_cfg,
                                  bcmfp_keygen_md_t *keygen_md,
                                  bcmfp_keygen_oper_t *keygen_oper)
{
    uint8_t part = 0;

    SHR_FUNC_ENTER(unit);

    for (part = 0; part < keygen_cfg->num_parts; part++) {
        SHR_IF_ERR_EXIT
            (bcmfp_keygen_ext_debug_info_print_1(unit,
                                                 part,
                                                 keygen_cfg,
                                                 keygen_md,
                                                 keygen_oper));
    }

exit:
    SHR_FUNC_EXIT();
}
static int
bcmfp_keygen_bit_stream_get(int unit,
                     SHR_BITDCL *buffer,
                     uint16_t start_bit,
                     uint16_t size,
                     uint16_t  *bs_count,
                     uint16_t *bs_offset,
                     uint16_t *bs_width)
{
    int offset = -1;
    uint16_t bit = 0;
    uint16_t width = 0;
    uint16_t end_bit = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(bs_count, SHR_E_PARAM);
    SHR_NULL_CHECK(bs_width, SHR_E_PARAM);
    SHR_NULL_CHECK(bs_offset, SHR_E_PARAM);

    width = 0;
    offset = -1;
    *bs_count = 0;
    end_bit = start_bit + size;
    for (bit = start_bit; bit < end_bit; bit++)  {
        if (SHR_BITGET(buffer, bit)) {
           if (offset == -1) {
               offset = bit;
           }
           width++;
           if (bit == end_bit - 1) {
               bs_offset[*bs_count] = offset;
               bs_width[*bs_count] = width;
               (*bs_count)++;
           }
        } else {
           if (offset != -1) {
               bs_offset[*bs_count] = offset;
               bs_width[*bs_count] = width;
               (*bs_count)++;
               offset = -1;
               width = 0;
           }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

#define ORDER_TYPE_ASCENDING     (1 << 0)
#define ORDER_TYPE_DESCENDING    (1 << 1)
#define ORDER_TYPE_END_OF_LIST   (1 << 2)
#define ORDER_TYPE_START_OF_LIST (1 << 3)

static int
bcmfp_keygen_ext_finfo_node_add(int unit,
                  uint8_t order_type,
                  bcmfp_keygen_ext_field_info_t *in_node,
                  bcmfp_keygen_ext_field_info_t **head_node)
{
    bcmfp_keygen_ext_field_info_t *temp_node1 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_node2 = NULL;


    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in_node, SHR_E_PARAM);
    SHR_NULL_CHECK(head_node, SHR_E_PARAM);

    in_node->next = NULL;
    if (*head_node == NULL) {
        *head_node = in_node;
        SHR_EXIT();
    }

    if (order_type == ORDER_TYPE_ASCENDING) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    } else if (order_type == ORDER_TYPE_DESCENDING) {
        temp_node1 = temp_node2 = *head_node;
        while (temp_node1 != NULL) {
             if (temp_node1->req_bit_count <
                 in_node->req_bit_count) {
                 in_node->next = temp_node1;
                 if (temp_node1 == temp_node2) {
                     *head_node = in_node;
                     break;
                 }
                 temp_node2->next = in_node;
                 break;
             }
             temp_node2 = temp_node1;
             temp_node1 = temp_node1->next;
        }
        if (temp_node1 == NULL) {
            in_node->next = NULL;
            temp_node2->next = in_node;
        }
    } else if (order_type == ORDER_TYPE_END_OF_LIST) {
        temp_node1 = *head_node;
        while (temp_node1->next != NULL) {
            temp_node1 = temp_node1->next;
        }
        temp_node1->next = in_node;
    } else if (order_type == ORDER_TYPE_START_OF_LIST) {
        in_node->next = *head_node;
        *head_node = in_node;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

static void
bcmfp_keygen_ext_finfo_release(int unit,
                             bcmfp_keygen_ext_field_info_t *finfo)
{
    bcmfp_keygen_ext_field_info_t *temp_finfo = NULL;

    while (finfo != NULL) {
        temp_finfo = finfo;
        finfo = finfo->next;
        sal_memset(temp_finfo, 0, sizeof(bcmfp_keygen_ext_field_info_t));
        temp_finfo->next = finfo_free_pool[unit];
        finfo_free_pool[unit] = temp_finfo;
        DUMP_FINFO_COUNT(1);
    }
}

static int
bcmfp_keygen_qual_id_count_get(int unit,
                            bcmfp_keygen_md_t *keygen_md,
                            bcmfp_keygen_cfg_t *keygen_cfg,
                            uint8_t *qual_id_count)
{
    int bit_count = 0;
    uint8_t idx = 0;
    uint8_t f_idx = 0;
    uint8_t finfo_idx = 0;
    uint8_t part = 0;
    uint8_t sec_val = 0;
    uint16_t qual_cfg_id = 0;
    uint16_t ctrl_sel_val = 0;
    uint16_t field_offset = 0;
    uint16_t field_width = 0;
    bcmfp_keygen_ext_section_t section;
    bcmfp_keygen_ext_ctrl_sel_t ctrl_sel;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;
    bcmfp_qualifier_t qual_id = BCMFP_QUALIFIERS_COUNT;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmfp_keygen_qual_cfg_info_t *qual_cfg_info = NULL;
    bcmfp_keygen_pmux_info_t *pmux_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(qual_id_count, SHR_E_PARAM);

    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        qual_cfg_id = keygen_md->qual_cfg_id_arr[idx];
        qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_cfg_info = keygen_md->qual_cfg_info[qual_id];
        qual_cfg = &(qual_cfg_info->qual_cfg_arr[qual_cfg_id]);
        if (SHR_BITGET(qual_cfg->qual_flags.w, BCMFP_KEYGEN_QUAL_FLAGS_PMUX)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_keygen_pmux_info_get(unit,
                                            qual_id,
                                            keygen_md->num_pmux_qual,
                                            keygen_md->pmux_info,
                                            &pmux_info));
            qual_id_count[pmux_info->pmux_part] += 1;
            continue;
        }
        field_offset = 0;
        for (f_idx = 0; f_idx < qual_cfg->num_chunks; f_idx++) {
            section = qual_cfg->e_params[f_idx].section;
            sec_val = qual_cfg->e_params[f_idx].sec_val;
            ctrl_sel = qual_cfg->pri_ctrl_sel;
            ctrl_sel_val = qual_cfg->pri_ctrl_sel_val;
            if (keygen_cfg->qual_info_arr[idx].partial == TRUE) {
                field_width = qual_cfg->e_params[f_idx].width;
                bit_count = 0;
                SHR_BITCOUNT_RANGE(keygen_cfg->qual_info_arr[idx].bitmap,
                                   bit_count, field_offset, field_width);
                field_offset += qual_cfg->e_params[f_idx].width;
                if (bit_count == 0) {
                    continue;
                }
            }
            for (finfo_idx = 0;
                finfo_idx < BCMFP_KEYGEN_EXT_FIELDS_MAX;
                finfo_idx++) {
                if (keygen_md->finfo[finfo_idx].section ==
                    BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
                    break;
                }
                if (section == keygen_md->finfo[finfo_idx].section &&
                    sec_val == keygen_md->finfo[finfo_idx].sec_val &&
                    ctrl_sel == keygen_md->finfo[finfo_idx].pri_ctrl_sel &&
                    ctrl_sel_val ==
                    keygen_md->finfo[finfo_idx].pri_ctrl_sel_val) {
                    finfo = &(keygen_md->finfo[finfo_idx]);
                    break;
                }
            }
            SHR_IF_ERR_VERBOSE_EXIT
                ((finfo == NULL) ? SHR_E_INTERNAL : SHR_E_NONE);
            part = finfo->part;
            qual_id_count[part]++;
            finfo = NULL;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_keygen_finfo_qual_info_update(int unit,
                             bcmfp_keygen_ext_field_info_t *parent_finfo,
                             bcmfp_keygen_ext_field_info_t *child_finfo,
                             uint16_t child_finfo_start_offset,
                             uint16_t parent_field_offset,
                             uint16_t parent_field_width)
{
    uint8_t qual_info_idx = 0;
    bcmfp_finfo_qual_info_t parent_qual_info;
    bcmfp_finfo_qual_info_t child_qual_info;
    uint16_t field_endbit = 0;
    bool qual_found = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(parent_finfo, SHR_E_PARAM);
    SHR_NULL_CHECK(child_finfo, SHR_E_PARAM);

    for (qual_info_idx = 0;
         qual_info_idx < parent_finfo->qual_info_count;
         qual_info_idx++) {

        qual_found = FALSE;

        parent_qual_info = parent_finfo->qual_info[qual_info_idx];

        if (parent_field_offset + parent_field_width <=
           (parent_qual_info.finfo_field_offset +
            parent_qual_info.finfo_field_width)) {
            field_endbit = parent_field_offset + parent_field_width;
        } else {
            field_endbit = (parent_qual_info.finfo_field_offset +
                            parent_qual_info.finfo_field_width);
        }

        /*
         * case -1 finfo_field_offset is in between parent_field_offset and
         * (parent_field_offset + width)
         */
        if ((parent_qual_info.finfo_field_offset >=
             parent_field_offset) &&
            (parent_qual_info.finfo_field_offset <
            (parent_field_offset + parent_field_width))) {

            child_qual_info.qual_id = parent_qual_info.qual_id;

            child_qual_info.finfo_field_offset =
                  (child_finfo_start_offset +
                  (parent_qual_info.finfo_field_offset -
                  parent_field_offset));

            child_qual_info.qual_field_offset =
                            parent_qual_info.qual_field_offset;

            child_qual_info.finfo_field_width =
                  (field_endbit - parent_qual_info.finfo_field_offset);
            qual_found = TRUE;

        } else if (parent_qual_info.finfo_field_offset <
                   parent_field_offset &&
                   parent_qual_info.finfo_field_offset +
                   parent_qual_info.finfo_field_width >
                   parent_field_offset) {
            /*
             * case-2: finfo_field_offset < parent_field_offset but the
             * qualifier maps to the chunk in between parent_field_offset
             * and parent_field_width
             */
            child_qual_info.qual_id = parent_qual_info.qual_id;

            child_qual_info.finfo_field_offset = child_finfo_start_offset;

            child_qual_info.qual_field_offset =
                            (parent_qual_info.qual_field_offset +
                            (parent_field_offset -
                            parent_qual_info.finfo_field_offset));

            child_qual_info.finfo_field_width =
                            (field_endbit - parent_field_offset);

            qual_found = TRUE;
        }

        if (qual_found) {
            child_finfo->qual_info[child_finfo->qual_info_count] =
                                                child_qual_info;
            child_finfo->qual_info_count++;
        }
    }

    /* Report error if no qualifiers are mapped to child_finfo */
    if (child_finfo->qual_info_count == 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_keygen_qual_finfo_db_update(int unit,
                           bcmfp_keygen_ext_field_info_t *finfo,
                           uint8_t out_sec_idx,
                           bcmfp_keygen_ext_field_info_t **qual_finfo_db_arr,
                           bcmfp_qualifier_t *qid_arr,
                           uint32_t *qid_arr_idx)
{
    uint8_t qual_info_idx = 0;
    bcmfp_finfo_qual_info_t qual_info;
    bcmfp_qualifier_t qual_id = BCMFP_QUALIFIERS_COUNT;
    uint8_t f_offset = 0;
    uint8_t f_width = 0;
    uint16_t temp_idx = 0;
    bcmfp_keygen_ext_field_info_t *temp_finfo3 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo4 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo_insert = NULL;
    bool inserted = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(finfo, SHR_E_PARAM);
    SHR_NULL_CHECK(qual_finfo_db_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(qid_arr, SHR_E_PARAM);

    for (qual_info_idx = 0;
         qual_info_idx < finfo->qual_info_count;
         qual_info_idx++) {

        inserted = FALSE;
        qual_info = finfo->qual_info[qual_info_idx];
        qual_id = qual_info.qual_id;
        f_offset = qual_info.finfo_field_offset;
        f_width = qual_info.finfo_field_width;
        /*
         * Make a copy of temp_finfo2 before inserting as this finfo
         * can be used by multiple qualifiers
         */
        temp_finfo_insert = finfo_free_pool[unit];
        if (temp_finfo_insert == NULL)  {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        finfo_free_pool[unit] = temp_finfo_insert->next;
        DUMP_FINFO_COUNT(0);
        sal_memcpy(temp_finfo_insert,
                   finfo,
                   sizeof(bcmfp_keygen_ext_field_info_t));
        temp_finfo_insert->next = NULL;

        /* Update the req_bmp in temp_finfo to the req_qualifier_bmp */
        sal_memset(temp_finfo_insert->req_bmp,
                   0,
                   sizeof(temp_finfo_insert->req_bmp));

        SHR_BITCOPY_RANGE(temp_finfo_insert->req_bmp[0],
                          f_offset,
                          finfo->req_bmp[out_sec_idx],
                          f_offset,
                          f_width);
        temp_finfo_insert->qual_field_offset = qual_info.qual_field_offset;

        /*
         * update the qual_finfo_db and insert temp_finfo's per
         * qualifier finfo's are to be inserted in ascending order
         * of qual_field_offset
         */
        for (temp_idx = 0 ; temp_idx < *qid_arr_idx; temp_idx++) {

            if (qid_arr[temp_idx]==qual_id) {

                /* New node has to be at the head_node of the list. */
                temp_finfo3 = qual_finfo_db_arr[temp_idx];
                if (temp_finfo3->qual_field_offset >
                    temp_finfo_insert->qual_field_offset) {
                    temp_finfo_insert->next = temp_finfo3;
                    qual_finfo_db_arr[temp_idx] = temp_finfo_insert;
                    inserted = TRUE;
                    break;
                }

                /* New node has to be some where in the middle of the list. */
                temp_finfo4 = qual_finfo_db_arr[temp_idx];
                while (temp_finfo3 != NULL) {
                    if (temp_finfo3->qual_field_offset >
                        temp_finfo_insert->qual_field_offset) {
                        temp_finfo4->next = temp_finfo_insert;
                        temp_finfo_insert->next = temp_finfo3;
                        break;
                    }
                    temp_finfo4 = temp_finfo3;
                    temp_finfo3 = temp_finfo3->next;
                }

                /* New node has to be at the end of the list. */
                if (temp_finfo3 == NULL) {
                    temp_finfo4->next = temp_finfo_insert;
                }
                inserted = TRUE;
                break;
            }
        }

        if (inserted == FALSE) {
            /* New node is the first node in the list. */
            qid_arr[*qid_arr_idx] = qual_id;
            qual_finfo_db_arr[*qid_arr_idx] = temp_finfo_insert;
            *qid_arr_idx = *qid_arr_idx + 1;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_keygen_ext_fields_get_last(int unit,
                            uint8_t level,
                            bcmfp_keygen_md_t *keygen_md,
                            bcmfp_keygen_cfg_t *keygen_cfg,
                            bcmfp_keygen_ext_field_info_t **qual_finfo_db_arr,
                            bcmfp_qualifier_t *qid_arr)
{
    uint8_t temp_level = 0;
    uint32_t ext_idx = 0;
    uint32_t qid_arr_idx = 0;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    uint8_t out_sec_level = 0;
    bcmfp_keygen_ext_section_t out_sec = BCMFP_KEYGEN_EXT_SECTION_COUNT;
    bcmfp_keygen_ext_field_info_t *temp_finfo1 = NULL;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);

    /*
     * Iterate over ext_cfg to identify the ext whose output
     * sec is the final key.
     */
    for (temp_level = 1; temp_level <= level; temp_level++) {
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[temp_level];
            ext_idx++) {

            ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[temp_level] + ext_idx;
            if (ext_cfg->in_use == FALSE) {
                continue;
            }

            for (idx = 0; idx < BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS; idx++) {
                out_sec = ext_cfg->out_sec[idx];
                if (out_sec == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
                    continue;
                }

                out_sec_level =
                    keygen_md->ext_cfg_db->sec_cfg[out_sec]->level;
                if (level != out_sec_level) {
                    continue;
                }
                if ((temp_finfo1 = ext_cfg->finfo) == NULL) {
                    continue;
                }
                /*
                 * Loop through all qualifiers mapped to temp_finfo1 and
                 * update qual_finfo_db_arr
                 */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_keygen_qual_finfo_db_update(unit,
                                                       temp_finfo1,
                                                       idx,
                                                       qual_finfo_db_arr,
                                                       qid_arr,
                                                       &qid_arr_idx));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_keygen_qual_offset_info_init(int unit,
                         bcmfp_keygen_oper_t *keygen_oper,
                         bcmfp_keygen_ext_field_info_t **qual_finfo_db_arr,
                         bcmfp_qualifier_t *qid_arr)
{
    uint16_t idx = 0;
    bcmfp_keygen_ext_field_info_t *temp_finfo1 = NULL;
    uint8_t offset_idx = 0;
    bcmfp_qualifier_t qual_id = BCMFP_QUALIFIERS_COUNT;
    uint8_t qid_found[BCMFP_KEYGEN_ENTRY_PARTS_MAX] = { FALSE };
    uint8_t qid_idx = 0;
    bcmfp_keygen_qual_offset_t *offset_info = NULL;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    uint8_t bs_idx = 0;
    uint16_t bs_count = 0;
    uint16_t bs_width[BCMFP_KEYGEN_EXT_FIELD_OFFSET_COUNT] = { 0 };
    uint16_t bs_offset[BCMFP_KEYGEN_EXT_FIELD_OFFSET_COUNT] = { 0 };
    uint8_t part;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; ;idx++) {
        if (qual_finfo_db_arr[idx] == NULL) {
            /* end of the qual_finfo_db_arr */
            break;
        } else {
            temp_finfo1 = qual_finfo_db_arr[idx];
        }
        offset_idx = 0;
        qual_id = qid_arr[idx];
        sal_memset(qid_found,
                   FALSE,
                   sizeof(uint8_t) * BCMFP_KEYGEN_ENTRY_PARTS_MAX);
        while (temp_finfo1 != NULL) {
            part = temp_finfo1->part;
            if (qid_found[part] == FALSE) {
                qid_found[part] = TRUE;
            }
            qid_idx = keygen_oper->qual_offset_info[part].size;
            offset_info =
                &(keygen_oper->qual_offset_info[part].offset_arr[qid_idx]);
            keygen_oper->qual_offset_info[part].qid_arr[qid_idx] = qual_id;
            ext_cfg = temp_finfo1->ext_cfg;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_keygen_bit_stream_get(unit,
                                             temp_finfo1->req_bmp[0],
                                             0,
                                             temp_finfo1->size,
                                             &bs_count,
                                             bs_offset,
                                             bs_width));
            for (bs_idx = 0; bs_idx < bs_count; bs_idx++) {
                offset_info->offset[offset_idx] =
                             ext_cfg->offset[0] + bs_offset[bs_idx];
                offset_info->width[offset_idx] = bs_width[bs_idx];
                offset_info->num_offsets++;
                offset_idx++;
            }
            temp_finfo1 = temp_finfo1->next;
        }
        if (qual_finfo_db_arr[idx] != NULL) {
            (void)bcmfp_keygen_ext_finfo_release(unit, qual_finfo_db_arr[idx]);
            qual_finfo_db_arr[idx] = NULL;
        }
        for (part = 0; part < BCMFP_KEYGEN_ENTRY_PARTS_MAX; part++) {
            if (qid_found[part] == TRUE) {
                keygen_oper->qual_offset_info[part].size++;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_keygen_qual_offset_create(int unit,
                                bcmfp_keygen_cfg_t *keygen_cfg,
                                bcmfp_keygen_oper_t *keygen_oper,
                                bcmfp_keygen_md_t *keygen_md)
{
    uint8_t qual_id_count[BCMFP_KEYGEN_ENTRY_PARTS_MAX];
    uint8_t parts_count = 0;
    uint16_t qual_cfg_id = 0;
    uint8_t part = 0;
    uint8_t level = 0;
    uint16_t idx = 0;
    bcmfp_qualifier_t qual_id = BCMFP_QUALIFIERS_COUNT;
    bcmfp_keygen_qual_cfg_info_t *qual_cfg_info = NULL;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmfp_keygen_qual_offset_t *qual_offset = NULL;
    bcmfp_keygen_pmux_info_t *pmux_info = NULL;
    bcmfp_qualifier_t *qid_arr = NULL;
    bcmfp_keygen_qual_offset_t *offset_arr = NULL;
    bcmfp_keygen_qual_offset_info_t *pmuxqual_offset_info = NULL;
    bcmfp_keygen_ext_field_info_t **temp_qual_finfo_db_arr = NULL;
    bcmfp_qualifier_t  *temp_qid_arr = NULL;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    parts_count = keygen_cfg->num_parts;
    size = (sizeof(uint8_t) * BCMFP_KEYGEN_ENTRY_PARTS_MAX);
    sal_memset(qual_id_count, 0, size);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_keygen_qual_id_count_get(unit,
                                        keygen_md,
                                        keygen_cfg,
                                        qual_id_count));

    size = (sizeof(bcmfp_keygen_qual_offset_info_t) * parts_count);
    BCMFP_ALLOC(keygen_oper->qual_offset_info,
                size,
                "bcmfpKeyGenOperQualOffsetInfo");

    size = (sizeof(bcmfp_keygen_ext_field_info_t *) *
            BCMFP_KEYGEN_EXT_FIELDS_MAX);
    BCMFP_ALLOC(temp_qual_finfo_db_arr,
                size,
                "bcmfpKeyGenTempQualFinfoDBArr");

    size = (sizeof(bcmfp_qualifier_t) *
            BCMFP_KEYGEN_EXT_FIELDS_MAX);
    BCMFP_ALLOC(temp_qid_arr,
                size,
                "bcmfpKeyGenTempQualIDArr");

    for (part = 0; part < parts_count; part++) {

        if (qual_id_count[part] == 0) {
            keygen_oper->qual_offset_info[part].qid_arr = NULL;
            keygen_oper->qual_offset_info[part].offset_arr = NULL;
            continue;
        }

        qid_arr = NULL;
        offset_arr = NULL;
        size = (keygen_cfg->qual_info_count * sizeof(bcmfp_qualifier_t));
        BCMFP_ALLOC(qid_arr, size, "bcmfpGrpQualArray");
        keygen_oper->qual_offset_info[part].qid_arr = qid_arr;

        size = (keygen_cfg->qual_info_count *
                sizeof(bcmfp_keygen_qual_offset_t));
        BCMFP_ALLOC(offset_arr, size, "bcmfpGrpQualOffsetArray");
        keygen_oper->qual_offset_info[part].offset_arr = offset_arr;
    }


    /* first extract the offsets for all PMUX qualifiers */
    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        qual_cfg_id = keygen_md->qual_cfg_id_arr[idx];
        qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_cfg_info = keygen_md->qual_cfg_info[qual_id];
        qual_cfg = &(qual_cfg_info->qual_cfg_arr[qual_cfg_id]);
        if (SHR_BITGET(qual_cfg->qual_flags.w,
            BCMFP_KEYGEN_QUAL_FLAGS_PMUX)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_keygen_pmux_info_get(unit,
                                            qual_id,
                                            keygen_md->num_pmux_qual,
                                            keygen_md->pmux_info,
                                            &pmux_info));
            pmuxqual_offset_info =
                &(keygen_oper->qual_offset_info[pmux_info->pmux_part]);
            qual_offset =
                &(pmuxqual_offset_info->offset_arr[pmuxqual_offset_info->size]);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_keygen_pmux_qual_offset_get(unit,
                                                   qual_id,
                                                   keygen_md,
                                                   keygen_cfg,
                                                   qual_offset));
            pmuxqual_offset_info->qid_arr[pmuxqual_offset_info->size] = qual_id;
            pmuxqual_offset_info->size++;
        }
    }

    /*
     * go through all the extractos whose out_sec is the final key and
     * extract the finfo's of each of these extractors on a qual_id basis
 */
    level = keygen_md->num_ext_levels;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_keygen_ext_fields_get_last(unit,
                                          level,
                                          keygen_md,
                                          keygen_cfg,
                                          temp_qual_finfo_db_arr,
                                          temp_qid_arr));
    /*
     * compute the offsets from qual_finfo_db_arr and
     * push them into keygen_oper->qual_offset_info[part]
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_keygen_qual_offset_info_init(unit,
                                            keygen_oper,
                                            temp_qual_finfo_db_arr,
                                            temp_qid_arr));
exit:
    SHR_FREE(temp_qual_finfo_db_arr);
    SHR_FREE(temp_qid_arr);
    SHR_FUNC_EXIT();
}

static int
bcmfp_keygen_ext_fields_get_first(int unit,
                               bcmfp_keygen_cfg_t *keygen_cfg,
                               bcmfp_keygen_md_t *keygen_md)
{
    int bit_count = 0;
    int bit_count1 = 0;
    int bit_count2 = 0;
    uint16_t req_bmp_size = 0;
    uint8_t idx = 0;
    uint8_t f_idx = 0;
    uint8_t i = 0, j = 0;
    uint8_t field_count = 0;
    uint8_t finfo_idx = 0;
    uint8_t f_width = 0;
    uint8_t sec_val = 0;
    uint8_t found = FALSE;
    uint16_t qual_cfg_id = 0;
    uint8_t q_bit = 0;
    uint8_t f_bit = 0;
    uint16_t f_offset = 0;
    uint32_t finfo_size = 0;
    uint16_t ctrl_sel_val = 0;
    uint16_t field_offset = 0;
    uint16_t field_width = 0;
    uint32_t qual_flags = 0;
    uint32_t finfo_flags = 0;
    bcmfp_keygen_ext_section_t section;
    bcmfp_keygen_ext_ctrl_sel_t ctrl_sel;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;
    bcmfp_keygen_ext_field_info_t temp_finfo;
    bcmfp_qualifier_t qual_id = BCMFP_QUALIFIERS_COUNT;
    bcmfp_keygen_qual_cfg_t *qual_cfg = NULL;
    bcmfp_keygen_qual_cfg_info_t *qual_cfg_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    finfo_size = (sizeof(bcmfp_keygen_ext_field_info_t) *
                  BCMFP_KEYGEN_EXT_FIELDS_MAX);
    sal_memset(keygen_md->finfo, 0, finfo_size);

    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {
        qual_cfg_id = keygen_md->qual_cfg_id_arr[idx];
        qual_id = keygen_cfg->qual_info_arr[idx].qual_id;
        qual_flags = keygen_cfg->qual_info_arr[idx].flags;
        qual_cfg_info = keygen_md->qual_cfg_info[qual_id];
        qual_cfg = &(qual_cfg_info->qual_cfg_arr[qual_cfg_id]);
        if (SHR_BITGET(qual_cfg->qual_flags.w,
                       BCMFP_KEYGEN_QUAL_FLAGS_PMUX)) {
            continue;
        }
        field_offset = 0;
        for (f_idx = 0; f_idx < qual_cfg->num_chunks; f_idx++) {
            section = qual_cfg->e_params[f_idx].section;
            sec_val = qual_cfg->e_params[f_idx].sec_val;
            ctrl_sel = qual_cfg->pri_ctrl_sel;
            ctrl_sel_val = qual_cfg->pri_ctrl_sel_val;
            if (keygen_cfg->qual_info_arr[idx].partial == TRUE) {
                bit_count = 0;
                field_width = qual_cfg->e_params[f_idx].width;
                SHR_BITCOUNT_RANGE(keygen_cfg->qual_info_arr[idx].bitmap,
                                   bit_count,
                                   field_offset,
                                   field_width);
                field_offset += qual_cfg->e_params[f_idx].width;
                if (bit_count == 0) {
                    continue;
                }
            } else {
                field_offset += qual_cfg->e_params[f_idx].width;
            }
            for (finfo_idx = 0;
                 finfo_idx < BCMFP_KEYGEN_EXT_FIELDS_MAX;
                 finfo_idx++) {
                 finfo = &(keygen_md->finfo[finfo_idx]);
                 if ((finfo->section == section &&
                      finfo->sec_val == sec_val &&
                      finfo->pri_ctrl_sel == ctrl_sel &&
                      finfo->pri_ctrl_sel_val == ctrl_sel_val)) {
                      found = TRUE;
                 }
                 /* Ignore 'found' if this flag is set */
                 if (qual_flags & BCMFP_KEYGEN_QUAL_F_USE_FINFO_NEW) {
                    found = FALSE;
                 }
                 if ((found == TRUE) ||
                    (finfo->section == BCMFP_KEYGEN_EXT_SECTION_DISABLE)) {
                     break;
                 }
                 continue;
            }
            finfo_flags = 0;
            finfo_flags |=
                  (qual_flags & BCMFP_KEYGEN_QUAL_F_USE_L2_EXT_GRAN16) ?
                   BCMFP_KEYGEN_EXT_FIELD_USE_L2_GRAN16: finfo_flags;
            finfo_flags |=
                  (qual_flags & BCMFP_KEYGEN_QUAL_F_USE_L2_EXT_GRAN8) ?
                   BCMFP_KEYGEN_EXT_FIELD_USE_L2_GRAN8: finfo_flags;
            if (found == FALSE) {
                finfo->flags = finfo_flags;
                finfo->size = qual_cfg->e_params[f_idx].size;
                finfo->section = section;
                finfo->sec_val = sec_val;
                finfo->pri_ctrl_sel = ctrl_sel;
                finfo->pri_ctrl_sel_val = ctrl_sel_val;
                finfo->part = -1;
                sal_memset(finfo->ext_bmp, 0, sizeof(finfo->ext_bmp));
                finfo->qual_info_count = 0;
                field_count++;
            }

            if (field_count > BCMFP_KEYGEN_EXT_FIELDS_MAX) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }

            f_offset = (qual_cfg->e_params[f_idx].offset %
                        qual_cfg->e_params[f_idx].size);
            f_width = qual_cfg->e_params[f_idx].width;

            /*
             * return error if qual_info_count exceeds
             * BCMFP_KEYGEN_MAX_QUAL_PER_FINFO.
             */
            if (finfo->qual_info_count >= BCMFP_KEYGEN_MAX_QUAL_PER_FINFO) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            /* update qual_info */
            finfo->qual_info[finfo->qual_info_count].qual_id = qual_id;
            finfo->qual_info[finfo->qual_info_count].qual_field_offset =
                                                     field_offset - f_width;
            finfo->qual_info[finfo->qual_info_count].finfo_field_width =
                                                     f_width;
            finfo->qual_info[finfo->qual_info_count].finfo_field_offset =
                                                     f_offset;
            finfo->qual_info_count++;

            if (keygen_cfg->qual_info_arr[idx].partial == TRUE) {
                q_bit = field_offset - f_width;
                for (f_bit = f_offset;
                     f_bit < (f_offset + f_width);
                     f_bit++) {
                    if (SHR_BITGET(
                        keygen_cfg->qual_info_arr[idx].bitmap, q_bit)) {
                        SHR_BITSET(finfo->req_bmp[0], f_bit);
                    }
                    q_bit++;
                }
            } else {
                SHR_BITSET_RANGE(finfo->req_bmp[0],
                                (int)f_offset,
                                (int)f_width);
            }
            if (qual_flags & BCMFP_KEYGEN_QUAL_F_EXTRACT_TO_MODULO_32_OFFSET) {
                if (f_idx == 0) {
                    finfo->flags |=
                        BCMFP_KEYGEN_EXT_FIELD_EXTRACT_TO_MODULO_32_OFFSET;
                }
            }
            if (qual_flags & BCMFP_KEYGEN_QUAL_F_STICKY_CONTAINERS) {
                if (f_idx != 0) {
                    keygen_md->finfo[finfo_idx - 1].sticky = finfo;
                    finfo->flags |= BCMFP_KEYGEN_EXT_FIELD_IS_STICKY;
                }
            }
            found = FALSE;

        }
    }

    if (keygen_cfg->flags &
        BCMFP_KEYGEN_CFG_FLAGS_SERIAL_QUAL_CONTAINERS) {
        SHR_EXIT();
    }

    /* Sort the finfo List */
    for (i = 0; i < field_count; i++) {
        if (keygen_md->finfo[i].flags & BCMFP_KEYGEN_EXT_FIELD_IS_STICKY) {
            continue;
        }
        for (j = (i + 1); j < field_count; j++) {
            if (keygen_md->finfo[j].flags &
                           BCMFP_KEYGEN_EXT_FIELD_IS_STICKY) {
                continue;
            }
            if (keygen_md->finfo[j].size > keygen_md->finfo[i].size) {
                sal_memcpy(&temp_finfo, &keygen_md->finfo[i],
                           sizeof(bcmfp_keygen_ext_field_info_t));
                sal_memcpy(&keygen_md->finfo[i], &keygen_md->finfo[j],
                           sizeof(bcmfp_keygen_ext_field_info_t));
                sal_memcpy(&keygen_md->finfo[j], &temp_finfo,
                           sizeof(bcmfp_keygen_ext_field_info_t));
            }
        }
    }

    j = 0;
    field_width = keygen_md->finfo[0].size;
    req_bmp_size =
        (sizeof(uint32_t) * BCMFP_KEYGEN_EXT_FIELD_WORD_COUNT * 8);
    for (i = 0; i < field_count; i++) {
        if (keygen_md->finfo[i].flags & BCMFP_KEYGEN_EXT_FIELD_IS_STICKY) {
            continue;
        }
        for (j = (i + 1); j < field_count; j++) {
            if (field_width != keygen_md->finfo[j].size) {
                field_width = keygen_md->finfo[i].size;
                break;
            }
            bit_count1 = 0;
            bit_count2 = 0;
            SHR_BITCOUNT_RANGE(keygen_md->finfo[i].req_bmp[0],
                               bit_count1,
                               0,
                               req_bmp_size);
            SHR_BITCOUNT_RANGE(keygen_md->finfo[j].req_bmp[0],
                               bit_count2,
                               0,
                               req_bmp_size);
            if (bit_count2 > bit_count1) {
                sal_memcpy(&temp_finfo, &keygen_md->finfo[j],
                           sizeof(bcmfp_keygen_ext_field_info_t));
                sal_memcpy(&keygen_md->finfo[j], &keygen_md->finfo[i],
                           sizeof(bcmfp_keygen_ext_field_info_t));
                sal_memcpy(&keygen_md->finfo[i], &temp_finfo,
                           sizeof(bcmfp_keygen_ext_field_info_t));
            }
        }
    }

    j = 0;
    field_width = keygen_md->finfo[0].size;
    for (i = 0; i < field_count; i++) {
        if (keygen_md->finfo[i].flags & BCMFP_KEYGEN_EXT_FIELD_IS_STICKY) {
            continue;
        }
        if (field_width != keygen_md->finfo[i].size) {
            field_width = keygen_md->finfo[i].size;
            j = i;
        }
        if (keygen_md->finfo[i].pri_ctrl_sel !=
                                BCMFP_KEYGEN_EXT_CTRL_SEL_DISABLE) {
            if (j != i) {
                sal_memcpy(&temp_finfo, &keygen_md->finfo[j],
                           sizeof(bcmfp_keygen_ext_field_info_t));
                sal_memcpy(&keygen_md->finfo[j], &keygen_md->finfo[i],
                           sizeof(bcmfp_keygen_ext_field_info_t));
                sal_memcpy(&keygen_md->finfo[i], &temp_finfo,
                           sizeof(bcmfp_keygen_ext_field_info_t));
            }
            j++;
        }
    }

    for (i = 0; i < field_count; i++) {
       LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                 "Section=%d, SecValue=%d, req_bmp=%d\n"),
                 keygen_md->finfo[i].section,
                 keygen_md->finfo[i].sec_val,
                 keygen_md->finfo[i].req_bmp[0][0]));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_keygen_ext_fields_validate(int unit,
                                uint8_t part,
                                uint8_t level,
                                bcmfp_keygen_ext_section_t section,
                                bcmfp_keygen_md_t *keygen_md)
{
    uint8_t equal = 0;
    uint8_t ext_part = 0;
    uint8_t temp_level = 0;
    uint16_t ext_idx = 0;
    uint16_t bit_count = 0;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo1 = NULL;
    uint8_t out_sec_idx = 0;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);

    bit_count = (BCMFP_KEYGEN_EXT_FIELD_WORD_COUNT * sizeof(uint32_t) * 8);
    for (temp_level = 1; temp_level <= level; temp_level++) {
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[temp_level];
            ext_idx++) {
            ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[temp_level] + ext_idx;
            ext_part =  BCMFP_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
            if (ext_cfg->in_use == FALSE || ext_part != part) {
                continue;
            }
            for (idx = 0; idx < BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS; idx++) {
                if (ext_cfg->out_sec[idx] == section) {
                    out_sec_idx = idx;
                    break;
                }
            }
            if (idx == BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS) {
                continue;
            }

            temp_finfo1 = ext_cfg->finfo;
            if (temp_finfo1 == NULL) {
                if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                    continue;
                } else {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
            equal = SHR_BITEQ_RANGE(temp_finfo1->req_bmp[out_sec_idx],
                                    temp_finfo1->ext_bmp[out_sec_idx],
                                    0,
                                    bit_count);
            if (!equal) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Notes:
 * Default behavior of this function is to allow any granularity
 * if finfo flags are not set
 */
static int
bcmfp_keygen_finfo_flags_verify(int unit,
                          uint8_t level,
                          uint8_t gran,
                          bcmfp_keygen_ext_field_info_t *finfo)
{
    uint8_t g_idx = 0;
    uint8_t index = 0;
    uint8_t found = TRUE;
    uint8_t exp_level = 0;
    uint8_t exp_gran[BCMFP_KEYGEN_EXT_GRANULAR_ARR_SIZE] = {0};

    if (finfo->flags & BCMFP_KEYGEN_EXT_FIELD_USE_L2_GRAN16) {
        exp_level = 1;
        exp_gran[index] = 16;
        index++;
    }
    if (finfo->flags & BCMFP_KEYGEN_EXT_FIELD_USE_L2_GRAN8) {
        exp_level = 1;
        exp_gran[index] = 8;
        index++;
    }

    if ((exp_level == level) && (index != 0)) {
        found = FALSE;
        for (g_idx = 0; g_idx < index; g_idx++) {
            if (exp_gran[g_idx] == gran) {
                found = TRUE;
                break;
            }
        }
    }

    return found;
}

static int
bcmfp_keygen_section_fields_create(int unit,
                            uint8_t part,
                            uint8_t level,
                            bcmfp_keygen_ext_section_t section,
                            uint8_t gran,
                            uint8_t num_ext,
                            bcmfp_keygen_cfg_t *keygen_cfg,
                            bcmfp_keygen_md_t *keygen_md)
{
    int count = 0;
    uint8_t idx = 0;
    uint8_t chunk = 0;
    uint8_t num_chunks = 0;
    uint8_t order_type = 0;
    uint8_t temp_level = 0;
    uint8_t flags_check = FALSE;
    uint16_t ext_idx = 0;
    uint16_t ext_offset = 0;
    uint16_t self_field_offset = 0;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    uint16_t parent_field_offset = 0;
    uint16_t parent_field_width = 0;
    bcmfp_keygen_ext_section_cfg_t *sec_cfg = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo1 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo2 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo3 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo4 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo_list = NULL;
    uint8_t out_sec_idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    for (temp_level = 1; temp_level <= level; temp_level++) {
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[temp_level];
            ext_idx++) {
            ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[temp_level] + ext_idx;
            if (ext_cfg->in_use == FALSE) {
                continue;
            }
            for (idx = 0; idx < BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS; idx++) {
                if (ext_cfg->out_sec[idx] == section) {
                    out_sec_idx = idx;
                    break;
                }
            }
            if (idx == BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS) {
                continue;
            }
            temp_finfo1 = ext_cfg->finfo;
            if (temp_finfo1 == NULL) {
                if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                    continue;
                } else {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
            if (temp_finfo1->size < gran) {
                continue;
            }
            flags_check = bcmfp_keygen_finfo_flags_verify(unit,
                                                          level,
                                                          gran,
                                                          temp_finfo1);
            if (flags_check == FALSE) {
               continue;
            }
            num_chunks = temp_finfo1->size / gran;
            ext_offset = ext_cfg->offset[out_sec_idx];
            for (chunk = 0; chunk < num_chunks; chunk++) {
                SHR_BITCOUNT_RANGE(temp_finfo1->req_bmp[out_sec_idx],
                                   count,
                                   chunk * gran, gran);
                if (count == 0) {
                    continue;
                }

                if (SHR_BITEQ_RANGE(temp_finfo1->req_bmp[out_sec_idx],
                                    temp_finfo1->ext_bmp[out_sec_idx],
                                    chunk * gran, gran)) {
                    continue;
                }
                temp_finfo2 = finfo_free_pool[unit];
                if (temp_finfo2 == NULL) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                finfo_free_pool[unit] = temp_finfo2->next;
                DUMP_FINFO_COUNT(0);
                temp_finfo2->flags = temp_finfo1->flags;
                temp_finfo2->section = section;
                temp_finfo2->sec_val = (ext_offset + (chunk * gran)) / gran;
                temp_finfo2->pri_ctrl_sel = temp_finfo1->pri_ctrl_sel;
                temp_finfo2->pri_ctrl_sel_val = temp_finfo1->pri_ctrl_sel_val;
                temp_finfo2->size = gran;
                temp_finfo2->extracted = FALSE;
                temp_finfo2->req_bit_count = count;
                parent_field_offset = chunk * gran;
                temp_finfo2->qual_info_count = 0;
                self_field_offset = 0;
                parent_field_width = temp_finfo2->size;
                SHR_BITCOPY_RANGE(temp_finfo2->req_bmp[0],
                                  0,
                                  temp_finfo1->req_bmp[out_sec_idx],
                                  chunk * gran,
                                  gran);
                SHR_BITCOPY_RANGE(temp_finfo1->ext_bmp[out_sec_idx],
                                  chunk * gran,
                                  temp_finfo1->req_bmp[out_sec_idx],
                                  chunk * gran,
                                  gran);
                temp_finfo2->prev = temp_finfo1;
                temp_finfo2->prev_offset = chunk * gran;
                temp_finfo2->next = NULL;
                temp_finfo2->part = part;

                /* update the qual_info for temp_finfo2 */
                /*
                 * extract all the qualifiers mapped between
                 * parent_field_offset and
                 * parent_field_offset+finfo2->size
                 */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_keygen_finfo_qual_info_update(unit,
                                                         temp_finfo1,
                                                         temp_finfo2,
                                                         self_field_offset,
                                                         parent_field_offset,
                                                         parent_field_width));

                /*
                 * Add the new finfo node to the list in the sorted
                 * (descending) order.
                 */
                sec_cfg = keygen_md->ext_cfg_db->sec_cfg[section];
                if (sec_cfg->flags & BCMFP_KEYGEN_EXT_SECTION_PASS_THRU) {
                    order_type = ORDER_TYPE_END_OF_LIST;
                } else {
                    order_type = ORDER_TYPE_DESCENDING;
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_keygen_ext_finfo_node_add(unit,
                                                     order_type,
                                                     temp_finfo2,
                                                     &temp_finfo_list));
            }
        }
    }

    /*
     * Keep only num_ext number of nodes in the finfo list and add the
     * remaining nodes to free list.
     */
    temp_finfo3 = temp_finfo_list;
    temp_finfo2 = NULL;
    idx = 0;
    while (temp_finfo3 != NULL) {
        idx++;
        temp_finfo2 = temp_finfo3;
        temp_finfo3 = temp_finfo3->next;
        if (idx == num_ext) {
            break;
        }
    }

    while (temp_finfo3 != NULL) {
        temp_finfo4 = temp_finfo3;
        temp_finfo3 = temp_finfo3->next;
        ext_cfg = temp_finfo4->prev->ext_cfg;
        for (idx = 0; idx < BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS; idx++) {
            if (ext_cfg->out_sec[idx] == section) {
                out_sec_idx = idx;
                break;
            }
        }
        SHR_BITCLR_RANGE(temp_finfo4->prev->ext_bmp[out_sec_idx],
                         temp_finfo4->prev_offset,
                         temp_finfo4->size);
        sal_memset(temp_finfo4, 0, sizeof(bcmfp_keygen_ext_field_info_t));
        temp_finfo4->next = finfo_free_pool[unit];
        finfo_free_pool[unit] = temp_finfo4;
        DUMP_FINFO_COUNT(1);
    }

    /* Add the list of chunks to the section in given level and part. */
    if (temp_finfo2 != NULL) {
        temp_finfo2->next =
                     keygen_md->ext_finfo_db[part][level].finfo[section];
        keygen_md->ext_finfo_db[part][level].finfo[section] =
                                                  temp_finfo_list;
    }

    SHR_FUNC_EXIT();
exit:
    SHR_FUNC_EXIT();
}

static void
bcmfp_keygen_ext_section_finfo_reset(int unit,
                                    uint8_t part,
                                    uint8_t level,
                                    bcmfp_keygen_ext_section_t section,
                                    bcmfp_keygen_md_t *keygen_md)
{
    bcmfp_keygen_ext_field_info_t *finfo = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo = NULL;

    finfo = keygen_md->ext_finfo_db[part][level].finfo[section];
    while (finfo != NULL) {
        temp_finfo = finfo;
        finfo = finfo->next;

        SHR_BITCLR_RANGE(temp_finfo->prev->ext_bmp[0],
                temp_finfo->prev_offset,
                temp_finfo->size);
    }

    (void)bcmfp_keygen_ext_finfo_release(unit,
            keygen_md->ext_finfo_db[part][level].finfo[section]);

    keygen_md->ext_finfo_db[part][level].finfo[section] = NULL;
}

static int
bcmfp_keygen_section_fields_create_type1(int unit,
                                         uint8_t part,
                                         uint8_t level,
                                         bcmfp_keygen_ext_section_t section,
                                         bcmfp_keygen_cfg_t *keygen_cfg,
                                         bcmfp_keygen_md_t *keygen_md)
{
    int src_offset = 0;
    int dst_offset = 0;
    int num_bits = 0;
    uint8_t chunk = 0;
    uint8_t temp_level = 0;
    uint8_t gran = 0;
    uint8_t num_chunks = 0;
    uint16_t idx = 0;
    uint16_t ext_idx = 0;
    uint16_t num_extractors = 0;
    uint32_t chunk_bmp = 0;
    uint32_t section_bmp[20] = { 0 };
    uint8_t chunk_offset = 0;
    uint8_t required_extractors = 0;
    uint8_t required_extractors_gran[4];
    uint8_t extractors_1b_count = 0;
    uint8_t extractors_2b_count = 0;
    uint8_t extractors_4b_count = 0;
    uint16_t new_node_offset = 0;
    uint16_t old_node_offset = 0;
    uint16_t self_field_offset = 0;
    uint16_t parent_field_offset = 0;
    uint16_t parent_field_width = 0;
    uint8_t new_node_size = 0;
    uint8_t old_node_size = 0;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo1 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo2 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo_list = NULL;
    bcmfp_keygen_ext_section_gran_info_t *section_gran_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    for (temp_level = 1; temp_level <= level; temp_level++) {
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[temp_level];
            ext_idx++) {
            ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[temp_level] + ext_idx;
            if (ext_cfg->in_use == FALSE) {
                continue;
            }
            if (ext_cfg->out_sec[ext_cfg->out_sec_idx] != section) {
                continue;
            }
            temp_finfo1 = ext_cfg->finfo;
            if (temp_finfo1 == NULL) {
                if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                    continue;
                } else {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
            dst_offset = ext_cfg->offset[0];
            src_offset = 0;
            num_bits = temp_finfo1->size;
            SHR_BITCOPY_RANGE(section_bmp,
                              dst_offset,
                              temp_finfo1->req_bmp[0],
                              src_offset,
                              num_bits);
        }
    }

    section_gran_info = &(keygen_md->section_gran_info[section]);
    for (idx = 0;
         idx < section_gran_info->out_gran_info_count;
         idx++) {
        gran = section_gran_info->out_gran_info[idx].gran;
        num_extractors = section_gran_info->out_gran_info[idx].num_extractors;
        if (gran == 1) {
            extractors_1b_count = num_extractors;
        } else if (gran == 2) {
            extractors_2b_count = num_extractors;
        } else if (gran == 4) {
            extractors_4b_count = num_extractors;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    num_chunks = (sizeof(uint32_t) * 8 * 20) / 4;
    for (chunk = 0; chunk < num_chunks; chunk++) {
        chunk_bmp = 0;
        SHR_BITCOPY_RANGE(&chunk_bmp, 0, section_bmp, chunk * 4, 4);
        if ((chunk_bmp & 0xF) == 0) {
            continue;
        }
        switch (chunk_bmp) {
            case 0x1:
            case 0x2:
            case 0x4:
            case 0x8:
                required_extractors = 1;
                if (extractors_1b_count) {
                    required_extractors_gran[0] = 1;
                } else if (extractors_2b_count) {
                    required_extractors_gran[0] = 2;
                } else if (extractors_4b_count) {
                    required_extractors_gran[0] = 4;
                } else {
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }
                break;
            case 0x3:
            case 0xC:
                required_extractors = 1;
                if (extractors_2b_count) {
                    required_extractors_gran[0] = 2;
                } else if (extractors_1b_count >= 2) {
                    required_extractors = 2;
                    required_extractors_gran[0] = 1;
                    required_extractors_gran[1] = 1;
                } else if (extractors_4b_count) {
                    required_extractors_gran[0] = 4;
                } else {
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }
                break;
            case 0x5:
            case 0x6:
            case 0x9:
            case 0xA:
                required_extractors = 2;
                if (extractors_1b_count >= 2) {
                    required_extractors_gran[0] = 1;
                    required_extractors_gran[1] = 1;
                } else if (extractors_2b_count >= 2) {
                    required_extractors_gran[0] = 2;
                    required_extractors_gran[1] = 2;
                } else if (extractors_4b_count) {
                    required_extractors = 1;
                    required_extractors_gran[0] = 4;
                } else {
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }
                break;
            case 0x7:
            case 0xB:
                required_extractors = 2;
                if (extractors_2b_count &&
                    extractors_1b_count) {
                    required_extractors_gran[0] = 2;
                    required_extractors_gran[1] = 1;
                } else if (extractors_2b_count >= 2) {
                    required_extractors_gran[0] = 2;
                    required_extractors_gran[1] = 2;
                } else if (extractors_4b_count) {
                    required_extractors = 1;
                    required_extractors_gran[0] = 4;
                } else {
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }
                break;
            case 0xD:
            case 0xE:
                required_extractors = 2;
                if (extractors_2b_count &&
                    extractors_1b_count) {
                    required_extractors_gran[0] = 1;
                    required_extractors_gran[1] = 2;
                } else if (extractors_2b_count >= 2) {
                    required_extractors_gran[0] = 2;
                    required_extractors_gran[1] = 2;
                } else if (extractors_4b_count) {
                    required_extractors = 1;
                    required_extractors_gran[0] = 4;
                } else {
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }
                break;
            case 0xF:
                if (extractors_4b_count) {
                    required_extractors = 1;
                    required_extractors_gran[0] = 4;
                } else if (extractors_2b_count >= 2) {
                    required_extractors = 2;
                    required_extractors_gran[0] = 2;
                    required_extractors_gran[1] = 2;
                } else if (extractors_2b_count &&
                           extractors_1b_count >= 2) {
                    required_extractors = 3;
                    required_extractors_gran[0] = 2;
                    required_extractors_gran[1] = 1;
                    required_extractors_gran[2] = 1;
                } else {
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }
                break;
            default:
                SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        for (idx = 0; idx < required_extractors; idx++) {
             gran = required_extractors_gran[idx];
             for (chunk_offset = 0;
                 chunk_offset < (4 / gran);
                 chunk_offset++) {
                 if (chunk_bmp &
                    ((1 << gran) - 1) << (chunk_offset * gran)) {
                     break;
                 }
             }
             if (chunk_offset == (4 / gran)) {
                 SHR_ERR_EXIT(SHR_E_INTERNAL);
             }
             temp_finfo2 = finfo_free_pool[unit];
             if (temp_finfo2 == NULL) {
                 SHR_ERR_EXIT(SHR_E_INTERNAL);
             }
             finfo_free_pool[unit] = temp_finfo2->next;
             DUMP_FINFO_COUNT(0);
             temp_finfo2->section = section;
             temp_finfo2->sec_val =
                 ((chunk * 4) + (chunk_offset * gran)) / gran;
             temp_finfo2->size = gran;
             temp_finfo2->extracted = FALSE;
             temp_finfo2->prev = NULL;
             temp_finfo2->next = NULL;
             temp_finfo2->part = part;
             temp_finfo2->qual_info_count = 0;
             num_bits = 0;
             SHR_BITCOUNT_RANGE(&chunk_bmp, num_bits,
                                chunk_offset * gran, gran);

             temp_finfo2->req_bit_count = num_bits;
             SHR_BITCOPY_RANGE(temp_finfo2->req_bmp[0],
                               0,
                               &chunk_bmp,
                               chunk_offset * gran, gran);
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_keygen_ext_finfo_node_add(unit,
                                                  ORDER_TYPE_DESCENDING,
                                                  temp_finfo2,
                                                  &temp_finfo_list));
             if (gran == 1) {
                 extractors_1b_count--;
             } else if (gran == 2) {
                 extractors_2b_count--;
             } else if (gran == 4) {
                 extractors_4b_count--;
             } else {
                 SHR_ERR_EXIT(SHR_E_INTERNAL);
             }
             SHR_BITCLR_RANGE(&chunk_bmp, chunk_offset * gran, gran);
        }
    }

    /* Update the l0 section of all new finfo nodes. */
    temp_finfo2 = temp_finfo_list;
    while (temp_finfo2 != NULL) {
        for (temp_level = 1; temp_level <= level; temp_level++) {
            for (ext_idx = 0;
                ext_idx < keygen_md->ext_cfg_db->conf_size[temp_level];
                ext_idx++) {
                ext_cfg = (keygen_md->ext_cfg_db->ext_cfg[temp_level] +
                           ext_idx);
                if (ext_cfg->in_use == FALSE) {
                    continue;
                }
                if (ext_cfg->out_sec[ext_cfg->out_sec_idx] != section) {
                    continue;
                }
                temp_finfo1 = ext_cfg->finfo;
                if (temp_finfo1 == NULL) {
                    if (SHR_BITGET(ext_cfg->ext_attrs.w,
                        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                        continue;
                    } else {
                        SHR_ERR_EXIT(SHR_E_INTERNAL);
                    }
                }
                new_node_offset = temp_finfo2->sec_val * temp_finfo2->size;
                new_node_size = temp_finfo2->size;
                old_node_offset = ext_cfg->offset[0];
                old_node_size = temp_finfo1->size;
                if (((new_node_offset >= old_node_offset) &&
                    ((new_node_offset + new_node_size)) <=
                    (old_node_offset + old_node_size)) ||
                    ((new_node_offset <= old_node_offset) &&
                    ((new_node_offset + new_node_size)) >=
                    (old_node_offset + old_node_size))) {

                    temp_finfo2->pri_ctrl_sel = temp_finfo1->pri_ctrl_sel;
                    temp_finfo2->pri_ctrl_sel_val =
                                 temp_finfo1->pri_ctrl_sel_val;
                    if (old_node_offset >= new_node_offset) {
                        parent_field_offset = 0;
                        self_field_offset = old_node_offset - new_node_offset;
                        parent_field_width = old_node_size;
                    } else {
                        parent_field_offset =
                               new_node_offset - old_node_offset;
                        self_field_offset = 0;
                        parent_field_width = new_node_size;
                    }
                    /* update the qual_info */
                     SHR_IF_ERR_VERBOSE_EXIT
                         (bcmfp_keygen_finfo_qual_info_update(unit,
                                                       temp_finfo1,
                                                       temp_finfo2,
                                                       self_field_offset,
                                                       parent_field_offset,
                                                       parent_field_width));
                }
            }
        }
        temp_finfo2 = temp_finfo2->next;
    }

    keygen_md->ext_finfo_db[part][level].finfo[section] = temp_finfo_list;

    SHR_FUNC_EXIT();

exit:
    (void)bcmfp_keygen_ext_finfo_release(unit, temp_finfo_list);
    SHR_FUNC_EXIT();
}

static int
bcmfp_keygen_ext_cfg_gran_set(int unit,
                             bcmfp_keygen_ext_cfg_t *ext_cfg,
                             uint8_t *gran_reset)
{
    uint8_t gran = 0;
    uint8_t attr_idx = 0;
    uint8_t ext_attr_count = 0;
    uint8_t gran_assigned = FALSE;
    bcmfp_keygen_ext_attr_t ext_attr;
    bcmfp_keygen_ext_attr_t ext_attr_arr[] =
               { BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_1,
                 BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_2,
                 BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_4,
                 BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_8,
                 BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_16,
                 BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_32,
               };

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ext_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(gran_reset, SHR_E_PARAM);

    ext_attr_count = sizeof(ext_attr_arr) /
                     sizeof(bcmfp_keygen_ext_attr_t);
    *gran_reset = FALSE;
    gran = BCMFP_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
    BCMFP_KEYGEN_EXT_GRAN_TO_ATTR_IDX(gran, attr_idx);
    attr_idx++;
    for (; attr_idx < ext_attr_count; attr_idx++) {
        ext_attr = ext_attr_arr[attr_idx];
        if (SHR_BITGET(ext_cfg->ext_attrs.w, ext_attr)) {
            BCMFP_KEYGEN_EXT_ATTR_IDX_TO_GRAN(attr_idx, gran);
            BCMFP_KEYGEN_EXT_ID_GRAN_SET(ext_cfg->ext_id, gran);
            gran_assigned = TRUE;
            break;
        }
    }
    if (gran_assigned == TRUE) {
        SHR_EXIT();
    }
    for (attr_idx = 0; attr_idx < ext_attr_count; attr_idx++) {
        ext_attr = ext_attr_arr[attr_idx];
        if (SHR_BITGET(ext_cfg->ext_attrs.w, ext_attr)) {
            BCMFP_KEYGEN_EXT_ATTR_IDX_TO_GRAN(attr_idx, gran);
            BCMFP_KEYGEN_EXT_ID_GRAN_SET(ext_cfg->ext_id, gran);
            *gran_reset = TRUE;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_keygen_section_fields_create_onehot(int unit,
                                   uint8_t part,
                                   uint8_t level,
                                   bcmfp_keygen_ext_section_t section,
                                   bcmfp_keygen_cfg_t *keygen_cfg,
                                   bcmfp_keygen_md_t *keygen_md)
{
    uint8_t order_type = 0;
    uint8_t temp_level = 0;
    uint8_t gran = 0;
    uint16_t ext_idx = 0;
    uint16_t self_field_offset = 0;
    uint16_t parent_field_offset = 0;
    uint16_t parent_field_width = 0;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmfp_keygen_ext_section_cfg_t *sec_cfg = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo1 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo2 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo_list = NULL;
    uint32_t onehot_ext_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    for (temp_level = 1; temp_level <= level; temp_level++) {
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[temp_level];
            ext_idx++) {
            ext_cfg =
                (keygen_md->ext_cfg_db->ext_cfg[temp_level] + ext_idx);
            if (ext_cfg->in_use == FALSE) {
                continue;
            }
            if (ext_cfg->out_sec[ext_cfg->out_sec_idx] != section) {
                continue;
            }
            temp_finfo1 = ext_cfg->finfo;
            if (temp_finfo1 == NULL) {
                if (SHR_BITGET(ext_cfg->ext_attrs.w,
                    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
                    continue;
                } else {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
            temp_finfo2 = finfo_free_pool[unit];
            if (temp_finfo2 == NULL) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            gran = BCMFP_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
            finfo_free_pool[unit] = temp_finfo2->next;
            DUMP_FINFO_COUNT(0);
            temp_finfo2->flags = temp_finfo1->flags;
            temp_finfo2->section = section;
            temp_finfo2->sec_val =
                 ((ext_cfg->offset[ext_cfg->out_sec_idx] + gran) / gran);
            temp_finfo2->pri_ctrl_sel = temp_finfo1->pri_ctrl_sel;
            temp_finfo2->pri_ctrl_sel_val = temp_finfo1->pri_ctrl_sel_val;
            temp_finfo2->size = gran;
            temp_finfo2->extracted = FALSE;
            temp_finfo2->req_bit_count = gran;
            temp_finfo2->part = part;
            /*
             * Since this section is ONEHOT section extractor ID to be used
             * by this finfo chunk is fixed and should not use any other
             * extractor than this.
             */
            onehot_ext_id = ext_cfg->onehot_ext_id[ext_cfg->out_sec_idx];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_keygen_ext_cfg_get(unit,
                                          keygen_md,
                                          onehot_ext_id,
                                          &(temp_finfo2->ext_cfg)));
            parent_field_offset = 0;
            self_field_offset = 0;
            temp_finfo2->qual_info_count = 0;
            parent_field_width = temp_finfo2->size;
            SHR_BITCOPY_RANGE(temp_finfo2->req_bmp[0], 0,
                              temp_finfo1->req_bmp[0], 0, gran);
            SHR_BITCOPY_RANGE(temp_finfo1->ext_bmp[0], 0,
                              temp_finfo1->req_bmp[0], 0, gran);
            temp_finfo2->prev = temp_finfo1;
            temp_finfo2->prev_offset = 0;
            temp_finfo2->next = NULL;

            /* update qual_info */
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_keygen_finfo_qual_info_update(unit,
                                                      temp_finfo1,
                                                      temp_finfo2,
                                                      self_field_offset,
                                                      parent_field_offset,
                                                      parent_field_width));
            /*
             * Add the new finfo node to the list in the sorted
             * (descending) order.
             */
            sec_cfg = keygen_md->ext_cfg_db->sec_cfg[section];
            if (sec_cfg->flags & BCMFP_KEYGEN_EXT_SECTION_PASS_THRU) {
                order_type = ORDER_TYPE_END_OF_LIST;
            } else {
                order_type = ORDER_TYPE_DESCENDING;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_keygen_ext_finfo_node_add(unit, order_type,
                                 temp_finfo2, &temp_finfo_list));
        }
    }

    /* Add the list of chunks to the section in given level and part. */
    if (temp_finfo2 != NULL) {
        temp_finfo2->next =
                     keygen_md->ext_finfo_db[part][level].finfo[section];
        keygen_md->ext_finfo_db[part][level].finfo[section] = temp_finfo_list;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_keygen_section_fields_create_type3(int unit,
                                         uint8_t part,
                                         uint8_t level,
                                         bcmfp_keygen_ext_section_t section,
                                         bcmfp_keygen_cfg_t *keygen_cfg,
                                         bcmfp_keygen_md_t *keygen_md)
{
    uint8_t idx = 0;
    uint8_t gran = 0;
    uint8_t gran_count = 0;
    uint8_t num_ext = 0;
    bcmfp_keygen_ext_section_gran_t *gran_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    gran_count =
        keygen_md->section_gran_info[section].out_gran_info_count;
    gran_info = keygen_md->section_gran_info[section].out_gran_info;
    /*
     * gran_count will always be one. For loop is there just for
     * future enhancements.
     */
    for (idx = 0; idx < gran_count; idx++) {
        gran = gran_info[idx].gran;
        num_ext = gran_info[idx].num_extractors;
        /*
         * Divide the unextracted bits of the section into chunks of
         * given granularity.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_keygen_section_fields_create(unit,
                                                part,
                                                level,
                                                section,
                                                gran,
                                                num_ext,
                                                keygen_cfg,
                                                keygen_md));
     }
     /*
      * Verify whether all the required bits in the section are part
      * of section containers. Otherwise resource error.
      */
     SHR_IF_ERR_VERBOSE_EXIT
         (bcmfp_keygen_ext_fields_validate(unit,
                                           part,
                                           level,
                                           section,
                                           keygen_md));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_keygen_section_fields_create_type2(int unit,
                                    uint8_t part, uint8_t level,
                                    bcmfp_keygen_ext_section_t section,
                                    bcmfp_keygen_cfg_t *keygen_cfg,
                                    bcmfp_keygen_md_t *keygen_md)
{
    int rv;
    uint16_t ext_idx = 0;
    int p = 0;
    uint8_t gran_reset = FALSE;
    uint16_t ext_cfg_combinations = 0;
    uint16_t ext_cfg_combinations_tried = 0;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    ext_cfg_combinations =
        keygen_md->ext_cfg_db->sec_cfg[section]->ext_cfg_combinations;
    while (TRUE) {
        rv = bcmfp_keygen_section_fields_create_type3(unit, part, level,
                                                      section, keygen_cfg,
                                                      keygen_md);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);
        if (SHR_SUCCESS(rv)) {
            break;
        }
        ext_cfg_combinations_tried++;
        if (ext_cfg_combinations ==
            ext_cfg_combinations_tried) {
            break;
        }
        for (ext_idx = 0;
            ext_idx < keygen_md->ext_cfg_db->conf_size[level + 1];
            ext_idx++) {
            ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level + 1] + ext_idx;
            p = BCMFP_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
            if (p != part || section != ext_cfg->in_sec) {
                continue;
            }
            if (!SHR_BITGET(ext_cfg->ext_attrs.w,
                BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN)) {
                continue;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_keygen_ext_cfg_gran_set(unit, ext_cfg, &gran_reset));
            if (gran_reset == TRUE) {
                continue;
            }
            break;
        }
        (void)bcmfp_keygen_ext_section_gran_info_init(unit, keygen_md);
        (void)bcmfp_keygen_ext_section_finfo_reset(unit, part, level,
                                                  section, keygen_md);
    }

exit:
    SHR_FUNC_EXIT();
}


/* Create containers in the given section. Number of containers
 * depends on the containers pushed from previous hierarchial level
 * sections to this section and outgoing muxers of the section.
 * Note: This function assumes that in a section either single gran
 * extractor or multi-gran extractor are present and not both.
 * Also, It is assumed that only one multi-gran extractor is
 * present in the section. In future, if a section has both types
 * of extractors or more then one multi-gran extractor, then
 * function will be required to be enhanced.
 */
static int
bcmfp_keygen_ext_section_fields_get(int unit, uint8_t part, uint8_t level,
                                   bcmfp_keygen_ext_section_t section,
                                   bcmfp_keygen_cfg_t *keygen_cfg,
                                   bcmfp_keygen_md_t *keygen_md)
{
    uint32_t sec_flags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    /* If section has 1, 2 and 4 bit granular out going
     * muxers then containers creation is different.
     */
    sec_flags = keygen_md->ext_cfg_db->sec_cfg[section]->flags;
    if ((sec_flags & BCMFP_KEYGEN_EXT_SECTION_WITH_GRAN_1) &&
        (sec_flags & BCMFP_KEYGEN_EXT_SECTION_WITH_GRAN_2) &&
        (sec_flags & BCMFP_KEYGEN_EXT_SECTION_WITH_GRAN_4)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_keygen_section_fields_create_type1(unit, part, level,
                                      section, keygen_cfg, keygen_md));
     } else if (sec_flags & BCMFP_KEYGEN_EXT_SECTION_ONEHOT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_keygen_section_fields_create_onehot(unit, part, level,
                                      section, keygen_cfg, keygen_md));
    } else if (sec_flags & BCMFP_KEYGEN_EXT_SECTION_MULTI_GRAN) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_keygen_section_fields_create_type2(unit, part, level,
                                      section, keygen_cfg, keygen_md));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_keygen_section_fields_create_type3(unit, part, level,
                                      section, keygen_cfg, keygen_md));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Given the current hierarchiel level, create containers in
 * all sections of next hierarchiel level.
 */
static int
bcmfp_keygen_ext_fields_get_next(int unit,
                              uint8_t part,
                              uint8_t level,
                              bcmfp_keygen_cfg_t *keygen_cfg,
                              bcmfp_keygen_md_t *keygen_md)
{
    uint8_t sec_idx = 0;
    bcmfp_keygen_ext_section_t section;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    for (sec_idx = 0; sec_idx < BCMFP_KEYGEN_EXT_SECTION_COUNT; sec_idx++) {
        section = keygen_md->sections[part][level + 1][sec_idx];
        if (section == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
            continue;
        }

        /* Create conatiners in the given section. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_keygen_ext_section_fields_get(unit,
                                                 part,
                                                 level + 1,
                                                 section,
                                                 keygen_cfg,
                                                 keygen_md));
    }

exit:
    SHR_FUNC_EXIT();
}

static void
bcmfp_keygen_ext_finfo_db_free(int unit,
                 bcmfp_keygen_ext_field_info_t *finfo_db)
{
    uint16_t size = 0;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;
    bcmfp_keygen_ext_field_info_t *next_finfo = NULL;
    bcmfp_keygen_ext_field_info_t *sticky_finfo = NULL;

    finfo = finfo_db;
    size = sizeof(bcmfp_keygen_ext_field_info_t);
    while (finfo != NULL) {
        next_finfo = finfo->next;
        sticky_finfo = finfo->sticky;
        sal_memset(finfo, 0, size);
        finfo->next = finfo_free_pool[unit];
        finfo_free_pool[unit] = finfo;
        DUMP_FINFO_COUNT(1);
        finfo = sticky_finfo;
        while (finfo != NULL) {
            sticky_finfo = finfo->sticky;
            sal_memset(finfo, 0, size);
            finfo->next = finfo_free_pool[unit];
            finfo_free_pool[unit] = finfo;
            DUMP_FINFO_COUNT(1);
            finfo = sticky_finfo;
        }
        finfo = next_finfo;
    }
}

/* Section needs to be validated when containers from sections in
 * previous level are pushed to it by a multiplexer.A container
 * can be pushed to a section if sections drain bits are greater
 * than fill bits plus number of bits to be pushed to section in
 * next level from this container.
 */
static int
bcmfp_keygen_ext_section_validate(int unit,
                                  uint8_t part,
                                  bcmfp_keygen_ext_section_t section,
                                  bcmfp_keygen_ext_cfg_t *ext_cfg,
                                  bcmfp_keygen_ext_field_info_t *finfo,
                                  bcmfp_keygen_cfg_t *keygen_cfg,
                                  bcmfp_keygen_md_t *keygen_md)
{
    uint8_t gran = 0;
    uint8_t level = 0;
    uint16_t fill_bits = 0;
    uint16_t drain_bits = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ext_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(finfo, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);

    level = keygen_md->ext_cfg_db->sec_cfg[section]->level;
    if (level == keygen_md->num_ext_levels) {
        gran = BCMFP_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
        fill_bits = keygen_md->ext_cfg_db->sec_cfg[section]->fill_bits;
        drain_bits = keygen_md->ext_cfg_db->sec_cfg[section]->drain_bits;
        if ((fill_bits + gran) > drain_bits) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
        SHR_EXIT();
    }

    ext_cfg->finfo = finfo;
    finfo->ext_cfg = ext_cfg;
    ext_cfg->in_use = TRUE;
    finfo->extracted = TRUE;

    SHR_IF_ERR_EXIT
        (bcmfp_keygen_ext_section_fields_get(unit,
                                             part,
                                             level,
                                             section,
                                             keygen_cfg,
                                             keygen_md));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_keygen_ext_sections_validate(int unit,
                                   uint8_t part,
                                   bcmfp_keygen_ext_cfg_t *ext_cfg,
                                   bcmfp_keygen_ext_field_info_t *finfo,
                                   bcmfp_keygen_cfg_t *keygen_cfg,
                                   bcmfp_keygen_md_t *keygen_md)
{
    int rv = SHR_E_NONE;
    uint8_t idx = 0;
    uint8_t idx1 = 0;
    bcmfp_keygen_ext_section_t out_sec = BCMFP_KEYGEN_EXT_SECTION_COUNT;
    bcmfp_keygen_ext_cfg_t *conflict_ext_cfg = NULL;
    uint8_t conflict_ext_out_sec_idx = 0;
    bcmfp_keygen_ext_section_t conflict_ext_out_sec =
                               BCMFP_KEYGEN_EXT_SECTION_COUNT;
    uint16_t bit_count = 0;
    bcmfp_keygen_ext_field_info_t *finfo_temp = NULL;
    uint8_t level = 0;
    uint8_t level_count = 0;
    uint16_t ext_idx = 0;
    bcmfp_keygen_ext_cfg_t *ext_cfg_temp = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ext_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(finfo, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);

    bit_count = (BCMFP_KEYGEN_EXT_FIELD_WORD_COUNT * sizeof(uint32_t) * 8);
    for (idx = 0; idx < BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS; idx++) {
        out_sec = ext_cfg->out_sec[idx];
        if (out_sec == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
            continue;
        }
        if (idx) {
            SHR_BITXOR_RANGE(finfo->req_bmp[idx - 1],
                             finfo->ext_bmp[idx - 1],
                             0,
                             bit_count,
                             finfo->req_bmp[idx]);
            SHR_BITAND_RANGE(finfo->req_bmp[idx - 1],
                             finfo->ext_bmp[idx - 1],
                             0,
                             bit_count,
                             finfo->req_bmp[idx - 1]);
        }
        /*
         * If the muxer has mutually exclusive muxer and that is in use,
         * then the muxer cannot be used.
         */
        if (ext_cfg->conflict_ext_id[idx]) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_keygen_ext_cfg_get(unit,
                                          keygen_md,
                                          ext_cfg->conflict_ext_id[idx],
                                          &conflict_ext_cfg));

            conflict_ext_out_sec_idx = conflict_ext_cfg->out_sec_idx;
            conflict_ext_out_sec =
                conflict_ext_cfg->out_sec[conflict_ext_out_sec_idx];
            /*
             * if the out sections on conflict_ext and ext are the
             * same try other out_sec of ext if it exists
             */
            if (conflict_ext_cfg->in_use == TRUE &&
                conflict_ext_out_sec == out_sec) {
                /*
                 * the extractor cant be used only if the out_sec of ext
                 * matches with the out_sec of conflicting extractor
                 */
                rv = SHR_E_RESOURCE;
                continue;
            }
        }
        ext_cfg->out_sec_idx = idx;
        rv = bcmfp_keygen_ext_section_validate(unit,
                                               part,
                                               out_sec,
                                               ext_cfg,
                                               finfo,
                                               keygen_cfg,
                                               keygen_md);
        if (rv == SHR_E_RESOURCE) {
            continue;
        }
        SHR_IF_ERR_EXIT(rv);
        break;
    }

    /*
     * if any of the the out_sections of the ext can't be used,
     * return resource error
     */
    if (idx == BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS) {
        /*
         * Copy the split req_bmp across multiple out sections
         * into first out section back for fresh trial with
         * another extractor.
         */
        for (idx = 1;
             idx < BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS;
             idx++) {
             SHR_BITOR_RANGE(finfo->req_bmp[0],
                             finfo->req_bmp[idx],
                             0,
                             bit_count,
                             finfo->req_bmp[0]);
             SHR_BITCLR_RANGE(finfo->req_bmp[idx], 0, bit_count);
        }
    }

    for (idx = 0; idx < BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS; idx++) {

        out_sec = ext_cfg->out_sec[idx];
        if (out_sec == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
            continue;
        }
        level = keygen_md->ext_cfg_db->sec_cfg[out_sec]->level;
        finfo_temp = keygen_md->ext_finfo_db[part][level].finfo[out_sec];
        (void)bcmfp_keygen_ext_finfo_db_free(unit, finfo_temp);
        keygen_md->ext_finfo_db[part][level].finfo[out_sec] = NULL;
        level_count = keygen_md->ext_cfg_db->sec_cfg[out_sec]->level;
        for (level = 1; level <= level_count; level++) {
            for (ext_idx = 0;
                 ext_idx < keygen_md->ext_cfg_db->conf_size[level];
                 ext_idx++) {
                ext_cfg_temp = keygen_md->ext_cfg_db->ext_cfg[level] +
                               ext_idx;
                for (idx1 = 0;
                     idx1 < BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS;
                     idx1++) {
                    if (ext_cfg_temp->out_sec[idx1] != out_sec) {
                        continue;
                    }
                    finfo_temp = ext_cfg_temp->finfo;
                    if (finfo_temp != NULL) {
                        sal_memset(finfo_temp->ext_bmp[idx1],
                                   0,
                                   sizeof(finfo_temp->ext_bmp[idx1]));
                    }
                }
            }
        }
    }

    ext_cfg->finfo = NULL;
    finfo->ext_cfg = NULL;
    ext_cfg->in_use = FALSE;
    finfo->extracted = FALSE;

    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_keygen_ext_offset_validate(int unit, uint8_t level,
                                bcmfp_keygen_md_t *keygen_md,
                                bcmfp_keygen_ext_cfg_t *in_ext_cfg)
{
    uint16_t ext_idx = 0;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(in_ext_cfg, SHR_E_PARAM);

    for (ext_idx = 0;
        ext_idx < keygen_md->ext_cfg_db->conf_size[level + 1];
        ext_idx++) {
        ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level + 1] + ext_idx;
        if (ext_cfg->in_sec != in_ext_cfg->out_sec[0]) {
            continue;
        }
        if ((ext_cfg->offset[0] % 32) != 0) {
            SHR_ERR_EXIT(SHR_E_CONFIG);
        } else {
            break;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/* Allocate muxers to a bus containers in the given
 * section of a mux hierarchiel level.
 */
static int
bcmfp_keygen_ext_alloc_4(int unit, uint8_t part, uint8_t level,
                      bcmfp_keygen_ext_section_t section,
                      bcmfp_keygen_cfg_t *keygen_cfg,
                      bcmfp_keygen_md_t *keygen_md,
                      bcmfp_keygen_ext_field_info_t *finfo,
                      uint16_t sticky_head_finfo_ext_num)
{
    int rv;
    int g = 0, p = 0, l = 0;
    uint16_t ext_idx = 0;
    uint16_t ext_num = 0;
    bcmfp_keygen_ext_section_t out_sec;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(finfo, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    /* Loop through all outgoing muxers of the section. */
    for (ext_idx = 0;
        ext_idx < keygen_md->ext_cfg_db->conf_size[level + 1];
        ext_idx++) {
        ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level + 1] + ext_idx;
        g = BCMFP_KEYGEN_EXT_ID_GRAN_GET(ext_cfg->ext_id);
        p = BCMFP_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
        l = BCMFP_KEYGEN_EXT_ID_LEVEL_GET(ext_cfg->ext_id);
        if ((p != part) || (l != (level + 1)) ||
            (ext_cfg->in_use == TRUE) ||
            (section != ext_cfg->in_sec)) {
           continue;
        }

        /*
         * Sometimes all bits extracted by an extractor(going directly
         * to final key section) cannot be part of the final key due
         * to key size restrictions.
         */
        if (SHR_BITGET(ext_cfg->ext_attrs.w,
                BCMFP_KEYGEN_EXT_ATTR_PARTIAL_GRAN)) {
            if (ext_cfg->valid_bitmap < finfo->req_bmp[0][0]) {
                continue;
            }
        } else {
            if (g != finfo->size) {
                continue;
            }
        }

        /*
         * Extractor is already allocated means this section is
         * ONEHOT section and extractor need not be allocated
         * again.
         */
        if (finfo->ext_cfg != NULL) {
            if (ext_cfg->ext_id != finfo->ext_cfg->ext_id) {
                continue;
            }
        }
        /* Validate the muxer to know whether it can be used or not. */
        rv = bcmfp_keygen_ext_validate(unit, p, keygen_md, ext_cfg);
        if (rv == SHR_E_CONFIG) {
            continue;
        }
        SHR_IF_ERR_EXIT(rv);

        if (finfo->flags &
            BCMFP_KEYGEN_EXT_FIELD_EXTRACT_TO_MODULO_32_OFFSET) {
            rv = bcmfp_keygen_ext_offset_validate(unit, level + 1,
                                                  keygen_md, ext_cfg);
            if (rv == SHR_E_CONFIG) {
                continue;
            }
            SHR_IF_ERR_EXIT(rv);
        }
        if (finfo->flags &
            BCMFP_KEYGEN_EXT_FIELD_EXTRACT_AFTER_STICKY_HEAD) {
            ext_num = BCMFP_KEYGEN_EXT_ID_NUM_GET(ext_cfg->ext_id);
            if (ext_num <= sticky_head_finfo_ext_num) {
                continue;
            }
        }
        /* Validate the muxer output section to check whether it can
         * hold the new container or not.
         */
        rv = bcmfp_keygen_ext_sections_validate(unit, part, ext_cfg, finfo,
                                                keygen_cfg, keygen_md);
        if (rv == SHR_E_RESOURCE) {
            continue;
        }
        SHR_IF_ERR_EXIT(rv);

        /* Update the muxer out section fill_bits. fill_bits is the number
         * bits the section is currently holding. Number of bits a section
         * can hold depends on the number of outgoing muxers of that
         * section.
         */
        out_sec = ext_cfg->out_sec[ext_cfg->out_sec_idx];
        keygen_md->ext_cfg_db->sec_cfg[out_sec]->fill_bits += g;

        /* Link the muxer and finfo node, mark that muxer is used and
         * container as extracted.
         */
        ext_cfg->finfo = finfo;
        finfo->ext_cfg = ext_cfg;
        ext_cfg->in_use = TRUE;
        finfo->extracted = TRUE;

        (void)bcmfp_keygen_ext_cfg_dump(unit, keygen_md, ext_cfg);
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Allocate muxers to all bus containers in the given
 * section of a mux hierarchiel level.
 */
static int
bcmfp_keygen_ext_alloc_3(int unit,
                      uint8_t part,
                      uint8_t level,
                      bcmfp_keygen_ext_section_t section,
                      bcmfp_keygen_cfg_t *keygen_cfg,
                      bcmfp_keygen_md_t *keygen_md)
{
    uint16_t sticky_head_finfo_ext_num = 0;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;
    bcmfp_keygen_ext_field_info_t *sticky_finfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    finfo = keygen_md->ext_finfo_db[part][level].finfo[section];
    /* Loop through all containers in the section */
    while (finfo != NULL) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_keygen_ext_alloc_4(unit, part, level, section,
                                     keygen_cfg, keygen_md, finfo,
                                     sticky_head_finfo_ext_num));
        /* Container is not able to be extracted by any muxer, due to
         * lack of resources.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            ((finfo->extracted == FALSE) ? SHR_E_RESOURCE : SHR_E_NONE);
        /* Allocate muxers to sticky finfo nodes in the same muxer
         * hierarchy.
         */
        sticky_finfo = finfo->sticky;
        while (sticky_finfo != NULL) {
            sticky_head_finfo_ext_num =
                BCMFP_KEYGEN_EXT_ID_NUM_GET(finfo->ext_cfg->ext_id);
            sticky_finfo->flags |=
                BCMFP_KEYGEN_EXT_FIELD_EXTRACT_AFTER_STICKY_HEAD;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_keygen_ext_alloc_4(unit, part,
                                level, section, keygen_cfg, keygen_md,
                                sticky_finfo, sticky_head_finfo_ext_num));
            /* Container is not able to be extracted by any muxer, due to
             * lack of resources.
             */
            SHR_IF_ERR_VERBOSE_EXIT
                ((sticky_finfo->extracted == FALSE) ? SHR_E_RESOURCE :
                                                      SHR_E_NONE);
            sticky_finfo = sticky_finfo->sticky;
        }
        /* if container is extracted successfully, then go to the next
         * container.
         */
        finfo = finfo->next;
        sticky_head_finfo_ext_num = 0;
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/* find the max number of output sections an extractor has */
static int
bcmfp_keygen_max_out_sec_count(int unit,
                               int part,
                               int level,
                               bcmfp_keygen_ext_section_t section,
                               bcmfp_keygen_sorted_sec_idx_t *temp_sec_idx_info,
                               bcmfp_keygen_md_t *keygen_md)
{
    uint8_t max_out_sec = 0;
    uint8_t ext_out_sec = 0;
    uint8_t idx = 0;
    uint16_t ext_idx =0;
    int p = 0 , l = 0;
    bcmfp_keygen_ext_cfg_t *ext_cfg = NULL;
    bcmfp_keygen_ext_section_t out_sec;

    SHR_FUNC_ENTER(unit);

    for (ext_idx = 0;
        ext_idx < keygen_md->ext_cfg_db->conf_size[level + 1];
        ext_idx++) {
        ext_cfg =  keygen_md->ext_cfg_db->ext_cfg[level + 1] + ext_idx;
        ext_out_sec = 0;
        p = BCMFP_KEYGEN_EXT_ID_PART_GET(ext_cfg->ext_id);
        l = BCMFP_KEYGEN_EXT_ID_LEVEL_GET(ext_cfg->ext_id);
        if ((p != part) || (l != (level + 1)) ||
            (section != ext_cfg->in_sec)) {
           continue;
        }

        for (idx = 0; idx < BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS; idx++) {
            out_sec = ext_cfg->out_sec[idx];
            if (out_sec == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
                continue;
            }
            ext_out_sec++;
        }

        if (ext_out_sec > max_out_sec) {
            max_out_sec = ext_out_sec;
        }

    }
    temp_sec_idx_info->max_out_sec = max_out_sec;
    SHR_FUNC_EXIT();
}

/* sort the section indices based on the unique max output sections */
static int
bcmfp_keygen_sort_sec_indices(int unit,
                              uint8_t part,
                              uint8_t level,
                              bcmfp_keygen_sorted_sec_idx_t **sec_idx_info,
                              bcmfp_keygen_md_t *keygen_md)
{
    uint8_t sec_idx = 0;
    bcmfp_keygen_ext_section_t section;
    bcmfp_keygen_sorted_sec_idx_t *temp_sec_idx_info = NULL;
    bcmfp_keygen_sorted_sec_idx_t *temp_sec_idx_info2 = NULL;
    bcmfp_keygen_sorted_sec_idx_t *temp_sec_idx_info3 = NULL;

    SHR_FUNC_ENTER(unit);

     for (sec_idx = 0;
          sec_idx < BCMFP_KEYGEN_EXT_SECTION_COUNT;
          sec_idx++) {

         section = keygen_md->sections[part][level][sec_idx];
         if (section == BCMFP_KEYGEN_EXT_SECTION_DISABLE &&
            keygen_md->ext_finfo_db[part][level].finfo[sec_idx]) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
         }
         if (section == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
             continue;
         }

         temp_sec_idx_info = NULL;

         BCMFP_ALLOC(temp_sec_idx_info,
                sizeof(bcmfp_keygen_sorted_sec_idx_t),
                "bcmfpKeyGenTempSecIdxList");

         temp_sec_idx_info->sec_idx = sec_idx;

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmfp_keygen_max_out_sec_count(unit,
                                             part,
                                             level,
                                             section,
                                             temp_sec_idx_info,
                                             keygen_md));

         if (*sec_idx_info == NULL) {
             /* Section is the first one to be inserted in the list */
             *sec_idx_info = temp_sec_idx_info;
             continue;
         }

         if (temp_sec_idx_info->max_out_sec <
             (*sec_idx_info)->max_out_sec) {
             /*
              * New section idx to be inserted at the head of the
              * list.
              */
             temp_sec_idx_info->next = *sec_idx_info;
             *sec_idx_info = temp_sec_idx_info;
             continue;
         }

         /*
          * New section idx has to be somewhere in the middle of
          * the list.
          */
         temp_sec_idx_info2 = *sec_idx_info;
         temp_sec_idx_info3 = *sec_idx_info;
         while (temp_sec_idx_info3 != NULL) {
             if (temp_sec_idx_info3->max_out_sec >
                 temp_sec_idx_info->max_out_sec) {
                 temp_sec_idx_info2->next = temp_sec_idx_info;
                 temp_sec_idx_info->next = temp_sec_idx_info3;
                 break;
             }
             temp_sec_idx_info2 = temp_sec_idx_info3;
             temp_sec_idx_info3 = temp_sec_idx_info3->next;
         }

         /* New sec idx has to be at the end of the list */
         if (temp_sec_idx_info3 == NULL) {
             temp_sec_idx_info2->next = temp_sec_idx_info;
         }
     }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Allocate muxers to all bus containers in given level of the
 * muxer hierarchy.
 */
static int
bcmfp_keygen_ext_alloc_2(int unit,
                         uint8_t part,
                         uint8_t level,
                         bcmfp_keygen_cfg_t *keygen_cfg,
                         bcmfp_keygen_md_t *keygen_md)
{
    uint8_t sec_idx = 0;
    bcmfp_keygen_ext_section_t section;
    bcmfp_keygen_sorted_sec_idx_t *sec_idx_info= NULL;
    bcmfp_keygen_sorted_sec_idx_t *temp_sec_idx_info = NULL;
    bcmfp_keygen_sorted_sec_idx_t *temp_sec_idx_info2 = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_keygen_sort_sec_indices(unit,
                                       part,
                                       level,
                                       &sec_idx_info,
                                       keygen_md));

    temp_sec_idx_info = sec_idx_info;
    /*
     * sec_idx_finfo contains the section indices to be used in sorted order
     * Here, the priority is given to sections whose extractors' unique output
     * sections count is less.
     * This ensures that the sections with fewer output sections
     * are processed first and sections with multiple output setions
     * are processed after that
     */
    while (temp_sec_idx_info != NULL) {
        sec_idx = temp_sec_idx_info->sec_idx;
        section = keygen_md->sections[part][level][sec_idx];
        /* Allocate muxers to all bus containers in the given
         * section of of mux hierarchiel level.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_keygen_ext_alloc_3(unit, part, level,
                           section, keygen_cfg, keygen_md));
        temp_sec_idx_info = temp_sec_idx_info->next;
    }
exit:
    while (sec_idx_info != NULL) {
        temp_sec_idx_info2 = sec_idx_info->next;
        SHR_FREE(sec_idx_info);
        sec_idx_info = temp_sec_idx_info2;
    }
    SHR_FUNC_EXIT();
}

/* Release finfo nodes used in a given level of muxer hierarchy
 * to finfo pool.
 */
static int
bcmfp_keygen_ext_section_finfo_db_free(int unit, uint8_t part,
                  uint8_t level, bcmfp_keygen_md_t *keygen_md)
{
    bcmfp_keygen_ext_section_t section;
    bcmfp_keygen_ext_field_info_t *finfo_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);

    for (section = 0; section < BCMFP_KEYGEN_EXT_SECTION_COUNT; section++) {
        finfo_db = keygen_md->ext_finfo_db[part][level].finfo[section];
        (void)bcmfp_keygen_ext_finfo_db_free(unit, finfo_db);
        keygen_md->ext_finfo_db[part][level].finfo[section] = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Allocate finfo nodes from finfo pool for all containers
 * in input bus to given muxer hierarchy.
 */
static int
bcmfp_keygen_ext_finfo_add(int unit,
                        uint8_t part,
                        bcmfp_keygen_md_t *keygen_md)
{
    uint8_t idx = 0;
    bcmfp_keygen_ext_section_t section;
    bcmfp_keygen_ext_field_info_t *temp_finfo1 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo2 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo3 = NULL;
    bcmfp_keygen_ext_field_info_t *temp_finfo4 = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);

    for (idx = 0; idx < BCMFP_KEYGEN_EXT_FIELDS_MAX; idx++) {
       if (keygen_md->finfo[idx].flags &
           BCMFP_KEYGEN_EXT_FIELD_IS_STICKY) {
           continue;
       }
       temp_finfo1 = &(keygen_md->finfo[idx]);
       section = temp_finfo1->section;
       if (section == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
          break;
       }
       if (temp_finfo1->part != part)  {
           continue;
       }
       temp_finfo2 = finfo_free_pool[unit];
       SHR_IF_ERR_VERBOSE_EXIT
           ((temp_finfo2 == NULL) ? SHR_E_INTERNAL : SHR_E_NONE);
       finfo_free_pool[unit] = finfo_free_pool[unit]->next;
       DUMP_FINFO_COUNT(0);
       sal_memcpy(temp_finfo2, temp_finfo1,
                  sizeof(bcmfp_keygen_ext_field_info_t));
       temp_finfo2->next = NULL;
       temp_finfo2->sticky = NULL;
       if (keygen_md->ext_finfo_db[part][0].finfo[section] == NULL) {
           keygen_md->ext_finfo_db[part][0].finfo[section] = temp_finfo2;
       } else {
           temp_finfo3 = keygen_md->ext_finfo_db[part][0].finfo[section];
           while (temp_finfo3->next != NULL) {
               temp_finfo3 = temp_finfo3->next;
           }
           temp_finfo3->next = temp_finfo2;
       }
       while (temp_finfo1->sticky != NULL) {
           temp_finfo4 = finfo_free_pool[unit];
           SHR_IF_ERR_VERBOSE_EXIT
               ((temp_finfo4 == NULL) ? SHR_E_INTERNAL : SHR_E_NONE);
           finfo_free_pool[unit] = finfo_free_pool[unit]->next;
           DUMP_FINFO_COUNT(0);
           sal_memcpy(temp_finfo4, temp_finfo1->sticky,
                      sizeof(bcmfp_keygen_ext_field_info_t));
           temp_finfo4->next = NULL;
           temp_finfo4->sticky = NULL;
           temp_finfo2->sticky = temp_finfo4;
           temp_finfo2 = temp_finfo4;
           temp_finfo1 = temp_finfo1->sticky;
       }
    }

exit:
    SHR_FUNC_EXIT();
}

/* Allocate muxers to all containers in one mux hierarchy(i.e. one part). */
static int
bcmfp_keygen_ext_alloc_1(int unit,
                         uint8_t part,
                         bcmfp_keygen_cfg_t *keygen_cfg,
                         bcmfp_keygen_md_t *keygen_md)
{
    uint8_t level = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    /*
     * Release all finfo nodes used in the previous run of this function
     * for the given mux hierarchy.
     */
    for (level = 0; level < keygen_md->num_ext_levels; level++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_keygen_ext_section_finfo_db_free(unit,
                                                    part,
                                                    level,
                                                    keygen_md));
    }

    /*
     * Reset all the muxer configurations to default values. These muxer
     * configurations might have been modified in the previous run of this
     * finction for the given mux hierarchy.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_keygen_ext_cfg_reset_part(unit, part, keygen_cfg, keygen_md));

    /*
     * Get the finfo nodes to hold containers in the first level of the
     * mux hierarchy.
     * Note: keygen_md->finfo will also hold the containers in first level
     * of the mux hierarchy. But copies of those are created before muxers
     * are allocated to have single unmodified copy of containers in first
     * level of mux hierarchy.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_keygen_ext_finfo_add(unit, part, keygen_md));

    /*
     * Validate the mux controls to be used for different conatiners.
     * Sometimes same mux control might be required for two different
     * containers. In such cases one container shouldbe moved to other
     * mux hierarchy.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_keygen_ext_codes_validate(unit,
                                         part,
                                         keygen_cfg,
                                         keygen_md));

    /* Allocate muxers in all levels of mux hierarchy to all containers. */
    for (level = 0; level < keygen_md->num_ext_levels; level++) {
        /*
         * Allocate muxers to all containers in a given level of
         * muxer hierarchy.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_keygen_ext_alloc_2(unit,
                                      part,
                                      level,
                                      keygen_cfg,
                                      keygen_md));

        /* Allocate conatiners in next level bus of mux hierarchy. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_keygen_ext_fields_get_next(unit,
                                              part,
                                              level,
                                              keygen_cfg,
                                              keygen_md));
    }

exit:
    SHR_FUNC_EXIT();
}

/* All finfo nodes in the pool will be initialized to zeros and
 * each node will be linked to its previous node.
 */
static void
bcmfp_keygen_ext_finfo_pool_init(int unit)
{
    uint16_t idx = 0;

    sal_memset(&finfo_pool[unit], 0,
        sizeof(bcmfp_keygen_ext_field_info_t) * BCMFP_KEYGEN_EXT_FINFO_COUNT);
    for (idx = 0; idx < (BCMFP_KEYGEN_EXT_FINFO_COUNT - 1); idx++) {
         finfo_pool[unit][idx].next = &(finfo_pool[unit][idx + 1]);
    }
    finfo_pool[unit][BCMFP_KEYGEN_EXT_FINFO_COUNT - 1].next = NULL;
    finfo_free_pool[unit] = &(finfo_pool[unit][0]);
    finfo_free_count = 1024;
    finfo_used_count = 0;
}

/* finfo pool has BCMFP_KEYGEN_EXT_FINFO_COUNT number of finfo
 * nodes.All these finfo nodes will be in one single linked
 * list. finfo nodes are taken out of this linked list on
 * demand for use and will be given back to the linked list
 * once if is not in use. At the end of the algorithm finfo
 * pool will be verified to see all the finfo nodes are given
 * back to avoid unintentional shortage of finfo nodes after
 * multiple runs of the algorithm.
 */
static int
bcmfp_keygen_ext_finfo_pool_verify(int unit)
{
    uint16_t idx = 0;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;

    SHR_FUNC_ENTER(unit);

    finfo = finfo_free_pool[unit];
    while (finfo != NULL) {
        idx++;
        finfo = finfo->next;
    }

    if (idx != BCMFP_KEYGEN_EXT_FINFO_COUNT) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/* Validate the mux configuration. ACL attributes are compared
 * against muxer attributes to confirm the muxer can be used or
 * not.
 */
int
bcmfp_keygen_ext_validate(int unit,
                       uint8_t part,
                       bcmfp_keygen_md_t *keygen_md,
                       bcmfp_keygen_ext_cfg_t *ext_cfg)
{
    uint8_t p = part;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_md, SHR_E_PARAM);
    SHR_NULL_CHECK(ext_cfg, SHR_E_PARAM);

    /* If muxer is already in use when groups QSET is first
     * time created. so should not use such extractors in
     * QSET update.
     */
    if (SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    /* HALF will have half the key size of base key size.
     * So all the muxers which are muxing bits to the LSB
     * of base key cannot be used in HALF mode.
     */
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_MODE_HALF) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_HALF)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    /* ASET wide mode has lesser key size than base key size,
     * inorder to support wider policy. So in ASET wide mode some
     * bits in final key are unused. So all the muxers which
     * are muxing bits to those unused bit positions cannot be
     * used in ASET wide mode.
     */
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_MODE_SINGLE_ASET_WIDE) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_SINGLE_ASET_WIDE)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    /* Post mux qualifiers occupies bits in the final key at fixed
     * offsets. So muxers which are muxing bits to post mux qualifier
     * portions in final key cannot be used if the post mux qualifier
     * is part of the groups QSET.
     */
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_IPBM) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_MSB)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0_HM) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0_HM)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1_HM) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1_HM)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB_HM) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_MSB_HM)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_DST_CONTAINER_A) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_DST_CONTAINER_B) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_DROP) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DROP)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASS_ID_BITS_0_3) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASS_ID_BITS_0_3)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASS_ID_BITS_4_7) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASS_ID_BITS_4_7)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASS_ID_BITS_8_11) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASS_ID_BITS_8_11)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_0_3) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_0_3)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_4_7) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_4_7)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_8_11) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_8_11)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_HIT) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_HIT)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_HIT) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_HIT)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_NAT_NEEDED) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_NAT_NEEDED)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_NAT_DST_REALM_ID) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_NAT_DST_REALM_ID)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_PKT_RESOLUTION) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_CLASS_ID_C) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_C)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_CLASS_ID_D) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_D)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_INT_PRI) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_PRI)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_INT_CN) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_CN)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_IFP_DROP_VECTOR) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IFP_DROP_VECTOR)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_MH_OPCODE) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_MH_OPCODE)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (SHR_BITGET(keygen_md->acl_attrs[p].w,
        BCMFP_KEYGEN_ACL_ATTR_PMUX_KEY_TYPE) &&
        SHR_BITGET(ext_cfg->ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_KEY_TYPE)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * The function checks whether the max of the qualifier offsets
 * (or the key width) exceeds the keygen_cfg->qual_max_offset.
 * The function returns resource error is the check fails
 */
static int
bcmfp_keygen_qual_offset_check(int unit,
                               bcmfp_keygen_oper_t *keygen_oper,
                               bcmfp_keygen_cfg_t *keygen_cfg)
{
    uint16_t qual_idx = 0, off_idx = 0, ksz = 0;
    bcmfp_keygen_qual_offset_t *offset_arr = NULL;
    bcmfp_keygen_qual_offset_info_t *qual_offset_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_oper,SHR_E_PARAM);

    qual_offset_info = keygen_oper->qual_offset_info;
    SHR_NULL_CHECK(qual_offset_info, SHR_E_PARAM);

    for (qual_idx = 0; qual_idx < qual_offset_info->size; qual_idx++) {
        offset_arr = &qual_offset_info->offset_arr[qual_idx];
        SHR_NULL_CHECK(offset_arr, SHR_E_PARAM);
        for (off_idx = 0; off_idx < offset_arr->num_offsets; off_idx++) {
            if (ksz <= offset_arr->offset[off_idx]) {
                ksz = (offset_arr->offset[off_idx] +
                        offset_arr->width[off_idx]);
            }
        }
    }

    if (ksz > keygen_cfg->qual_max_offset) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

exit:
    SHR_FUNC_EXIT();
}



static int
bcmfp_keygen_ext_alloc_start(int unit,
                             bcmfp_keygen_cfg_t *keygen_cfg,
                             bcmfp_keygen_oper_t *keygen_oper,
                             bcmfp_keygen_md_t *keygen_md)
{
    int rv;
    uint8_t idx = 0;
    uint8_t part = 0;
    uint16_t size = 0;
    uint8_t field_extracted = FALSE;
    bcmfp_keygen_ext_section_t section;
    bcmfp_keygen_ext_field_info_t *finfo = NULL;
    bcmfp_keygen_ext_field_info_t *sticky_finfo = NULL;

    SHR_FUNC_ENTER(unit);

    bcmfp_keygen_ext_finfo_pool_init(unit);

    /* Initialize the bits_extracted in each granularity of each part. */
    size = (sizeof(uint32_t) * BCMFP_KEYGEN_ENTRY_PARTS_MAX *
                               BCMFP_KEYGEN_EXT_GRAN_COUNT);
    sal_memset(keygen_md->bits_extracted, 0, size);

    /*
     * Get all the containers, in the input bus, relevant to candidate
     * qualifier configurations. Each container will occupy one finfo
     * in keygen_md->finfo array.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_keygen_ext_fields_get_first(unit, keygen_cfg, keygen_md));

    /*
     * if mode is single, all containers needs to be extracted with lone
     * multiplexer hierarchy. Otherwise containers needs to distributed
     * and extracted by different multiplexer hierarchies. finfo->part
     * will hold the multiplexer hierarchy to be used to extract that
     * container.
     */
    if (keygen_cfg->num_parts == 1) {
        for (idx = 0; idx < BCMFP_KEYGEN_EXT_FIELDS_MAX; idx++) {
            finfo = &(keygen_md->finfo[idx]);
            section = finfo->section;
            if (section == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
                break;
            }
            finfo->part = 0;
            sticky_finfo = finfo->sticky;
            while (sticky_finfo != NULL) {
                sticky_finfo->part = finfo->part;
                sticky_finfo = sticky_finfo->sticky;
            }
        }
    } else {
        /* Distribute the containers across different mux hierarchies. */
        for (idx = 0; idx < BCMFP_KEYGEN_EXT_FIELDS_MAX; idx++) {
            if (keygen_md->finfo[idx].flags &
                           BCMFP_KEYGEN_EXT_FIELD_IS_STICKY) {
                continue;
            }
            finfo = &(keygen_md->finfo[idx]);
            field_extracted = FALSE;
            section = finfo->section;
            if (section == BCMFP_KEYGEN_EXT_SECTION_DISABLE) {
                break;
            }
            /*
             * Initialize the bitmap of already tried mux hierarchies to zero
             * before start selecting mux hierarchy for the container.
             */
            keygen_md->tried_part_bmp[0] = 0;
            /*
             * Loop through the maximum number of mux hierarchies available
             * for the given keygen configuration.
             */
            for (part = 0; part < keygen_cfg->num_parts; part++) {
                /*
                 * Get one mux hierarchy. keygen_md->bits_extracted tracks
                 * number of containers extracted in each granularity in each
                 * mux hierarchy. So to select the right mux hiereachy for the
                 * container, it starts hierarchy that extracted least number
                 * of such containers. If it fails, then the next hierarchy
                 * with least number of such containers extracted is used.
                 * It will continue till the right hierarchy is selected for
                 * the container or all hierarchies are tried.
                 * Note: While assigning a mux hierarchy for a container,mux
                 * hierarchy of already extracted containers are not changed.
                 */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_keygen_ext_part_get(unit,
                                               keygen_cfg,
                                               keygen_md,
                                               finfo));
                /*
                 * part doesnt get assigned to finfo for cases where
                 * all the parts that the finfo's section if tried before
                 * and there is no other part to assign
                 */
                if (finfo->part < 0) {
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }

                LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                          "---------->Trying Section=%d, "
                          "SecValue=%d, part=%d\n"),
                          finfo->section, finfo->sec_val, finfo->part));

                /*
                 * Allocate muxers in selected hierarchy for all the containers
                 * which are already part of that hiereachy along with new
                 * container.
                 */
                rv = bcmfp_keygen_ext_alloc_1(unit,
                                              finfo->part,
                                              keygen_cfg,
                                              keygen_md);
                /*
                 * If the allocation fails with resource error, then try
                 * assigining different hierarchy for the container.
                 */
                if (rv == SHR_E_RESOURCE) {
                   finfo->part = -1;
                   continue;
                }
                /* Not expecting any other error codes. */
                SHR_IF_ERR_EXIT(rv);

                /*
                 * Mux hierarchy got assigned to the container
                 * successfully.
                 */
                field_extracted = TRUE;
                if (finfo->part < 0) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                keygen_md->bits_extracted[finfo->part][0] += finfo->size;
                break;
            }
            /*
             * field_extracted is set to FALSE means container does not fit
             * in any of the mux hierarchies.
             */
            SHR_IF_ERR_VERBOSE_EXIT
                ((field_extracted == FALSE) ? SHR_E_RESOURCE : SHR_E_NONE);
        }
    }

    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
              "---------->Trying all chunks together\n")));
    /*
     * All containers are successfully distributed across fifferent
     * mux hierarchies. Now go through each mux hierarchy and assign
     * muxers for all containers in it.
     */
    for (part = 0; part < keygen_cfg->num_parts; part++) {
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmfp_keygen_ext_alloc_1(unit, part, keygen_cfg, keygen_md));
    }

    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
              "---------->Extractor allocation passed\n")));

    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
              "---------->Offset creation started\n")));

    /* Assign the offsets in the final key for all the qualifiers. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_keygen_qual_offset_create(unit,
                                         keygen_cfg,
                                         keygen_oper,
                                         keygen_md));
    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
              "---------->Offset creation passed\n")));


    /*
     * Check if the key size/(max key offset) is within the
     * keygen_cfg->qual_max_offset
     */
    if (keygen_cfg->flags & BCMFP_KEYGEN_CFG_FLAGS_QUAL_MAX_OFFSET) {
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                  "---------->Qualifiers max offset "
                  "validation started\n")));
        rv = bcmfp_keygen_qual_offset_check(unit,
                                            keygen_oper,
                                            keygen_cfg);
        if (rv == SHR_E_RESOURCE) {
            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                      "---------->Qualifiers max offset "
                      "validation failed\n")));
            /*
             * clear the keygen_oper qual_offset_info and exit
             * with resource error
             */
            for (part = 0; part < keygen_cfg->num_parts; part++) {
                SHR_FREE(keygen_oper->qual_offset_info[part].qid_arr);
                SHR_FREE(keygen_oper->qual_offset_info[part].offset_arr);
            }
            SHR_FREE(keygen_oper->qual_offset_info);
            SHR_ERR_EXIT(rv);
        }
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
                  "---------->Qualifiers max offset "
                  "validation passed\n")));
    }

    /*
     * If the BCMFP_KEYGEN_EXT_ATTR_PUSH_TO_PARENT_EXT extractor
     * flag is set the ext_cfg->finfo and ext_cfg->in_use are to
     * be pushed to the ext_cfg->parent_ext_id. This is used for
     * cases where the pass through ext_cfg and parent_ext_cfg
     * should be the same. This has to be called before
     * ext_codes_create.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_keygen_ext_cfg_copy_childtoparent(unit,
                                                 keygen_cfg,
                                                 keygen_md));

    /*
     * Generate muxer controls fin all mux hierarchies.
     * These mux controls will be installed in hardware.
     */
    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
              "---------->Selcodes creation started\n")));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_keygen_ext_codes_create(unit,
                                       keygen_cfg,
                                       keygen_md,
                                       keygen_oper));

    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit,
              "---------->Selcodes creation passed\n")));
exit:
    (void)bcmfp_keygen_ext_debug_info_print(unit,
                                            keygen_cfg,
                                            keygen_md,
                                            keygen_oper);
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_ext_alloc(int unit,
                       bcmfp_keygen_cfg_t *keygen_cfg,
                       bcmfp_keygen_oper_t *keygen_oper,
                       bcmfp_keygen_md_t *keygen_md)
{
    int rv;
    uint8_t part;
    uint8_t level;

    SHR_FUNC_ENTER(unit);

    rv = bcmfp_keygen_ext_alloc_start(unit,
                                      keygen_cfg,
                                      keygen_oper,
                                      keygen_md);

    /* Clean up also if allocation fails */
    for (part = 0; part < keygen_cfg->num_parts; part++) {

         /* Release all finfo nodes used from the finfo pool. */
         for (level = 0; level < keygen_md->num_ext_levels; level++) {
             (void)bcmfp_keygen_ext_section_finfo_db_free(unit,
                                                          part,
                                                          level,
                                                          keygen_md);
         }

         /* Clear extractor configurations to default values. */
         (void)bcmfp_keygen_ext_cfg_reset_part(unit,
                                               part,
                                               keygen_cfg,
                                               keygen_md);
    }

    /*
     * Verify finfo pool to make sure that all finfo nodes are freed
     * back to pool.
     */
    SHR_IF_ERR_EXIT
        (bcmfp_keygen_ext_finfo_pool_verify(unit));

    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}
