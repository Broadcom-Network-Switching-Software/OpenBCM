/*! \file bcmfp_grp_selcode.c
 *
 * APIs for selcode algorithm.
 *
 * Selcode algorithm is used to allocate selcodes for the given QSET.
 * bcmfp_allocate_extractors is the top function to use. In addition
 * to this function and all the other required utility functions to
 * allocate selcodes are part of this file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/bcmfp_grp_selcode.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static int
bcmfp_group_selectors_init(int unit, bcmfp_group_t *fg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    fg->ext_codes[0].intraslice = BCMFP_EXT_SELCODE_DONT_USE;
    if (fg->flags & BCMFP_GROUP_SPAN_DOUBLE_SLICE) {
        if (fg->flags & BCMFP_GROUP_INTRASLICE_DOUBLEWIDE) {
            fg->ext_codes[2].intraslice = BCMFP_EXT_SELCODE_DONT_USE;
        } else {
            fg->ext_codes[1].intraslice = BCMFP_EXT_SELCODE_DONT_USE;
        }
    }

    fg->ext_codes[0].secondary = BCMFP_EXT_SELCODE_DONT_USE;
    if (fg->flags & BCMFP_GROUP_SPAN_DOUBLE_SLICE) {
        if (fg->flags & BCMFP_GROUP_INTRASLICE_DOUBLEWIDE) {
            fg->ext_codes[1].secondary = BCMFP_EXT_SELCODE_DONT_USE;
            fg->ext_codes[2].secondary = BCMFP_EXT_SELCODE_DONT_USE;
        }
    }

exit:

    SHR_FUNC_EXIT();
}

static int
bcmfp_group_qualifiers_free(int unit, bcmfp_group_t *fg, int idx)
{
    bcmfp_group_qual_offset_info_t *q_arr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fg, SHR_E_PARAM);
    if ((idx < 0) || (idx >= BCMFP_ENTRY_PARTS_MAX)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    q_arr = &(fg->qual_arr[idx]);

    SHR_FREE(q_arr->qid_arr);
    SHR_FREE(q_arr->offset_arr);

    q_arr->size = 0;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_selcode_qual_arr_free(int unit,
                            bcmfp_qual_cfg_info_t ***f_qual_arr,
                            uint16_t qual_count)
{
    int   qual_idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(f_qual_arr, SHR_E_PARAM);

    if (*f_qual_arr) {
        for (qual_idx = 0; qual_idx < qual_count ; qual_idx++) {
            if ((*f_qual_arr)[qual_idx]) {
                SHR_FREE((*f_qual_arr)[qual_idx]->qual_cfg_arr);
                SHR_FREE((*f_qual_arr)[qual_idx]);
            }
        }
        SHR_FREE(*f_qual_arr);
    }

exit:

    SHR_FUNC_EXIT();
}

static int
bcmfp_qual_info_cmp(const void *a, const void *b)
{
    bcmfp_qual_cfg_info_t **first = NULL;
    bcmfp_qual_cfg_info_t **second = NULL;

    first = (bcmfp_qual_cfg_info_t **)a;
    second = (bcmfp_qual_cfg_info_t **)b;

    if ((*first)->num_qual_cfg < (*second)->num_qual_cfg) {
        return (-1);
    } else if ((*first)->num_qual_cfg > (*second)->num_qual_cfg) {
        return (1);
    }
    return (0);
}


static int
bcmfp_selector_update_cmp(const void *a, const void *b)
{
    bcmfp_qual_cfg_t *first = NULL;
    bcmfp_qual_cfg_t *second = NULL;

    first = (bcmfp_qual_cfg_t *)a;
    second = (bcmfp_qual_cfg_t *)b;

    if (first->update_count <
            second->update_count) {
        return (-1);
    } else if (first->update_count >
            second->update_count) {
        return (1);
    }
    return (0);
}

static int
bcmfp_qualifiers_info_get(int unit,
                          bcmfp_stage_t *stage,
                          bcmfp_qset_t *qset_req,
                          bcmfp_qual_cfg_info_t ***f_qual_arr,
                          uint16_t qual_arr_size)
{
    uint16_t idx = 0;
    uint16_t arr_idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(qset_req, SHR_E_PARAM);
    SHR_NULL_CHECK(f_qual_arr, SHR_E_PARAM);

    /* Allocate & initialize group select codes array. */
    BCMFP_ALLOC((*f_qual_arr),
                (qual_arr_size * sizeof(bcmfp_qual_cfg_info_t *)),
                "bcmfpFieldQuals");

    /* Copy individual qualifiers information from the requested qset */
    SHR_BIT_ITER(qset_req->w, BCMFP_QUALIFIERS_COUNT, idx) {
        /* Qualifiers support check. */
        SHR_NULL_CHECK(stage->qual_cfg_info_db->qual_cfg_info[idx],
                       SHR_E_RESOURCE);

        /*
         * Make a copy of the qualifier combination for sorting
         * instead of using pointer to the data in stage
         * this will avoid sorting of qualifiers in stage
         * and make group create independent of previously
         * created groups.
         */
        BCMFP_ALLOC(((*f_qual_arr)[arr_idx]),
                    (sizeof(bcmfp_qual_cfg_info_t)),
                    "bcmfpFieldQualsCopy");

        (*f_qual_arr)[arr_idx]->qual =
                   stage->qual_cfg_info_db->qual_cfg_info[idx]->qual;
        (*f_qual_arr)[arr_idx]->num_qual_cfg =
                   stage->qual_cfg_info_db->qual_cfg_info[idx]->num_qual_cfg;

        /* Allocate memory for qualifer config array */
        BCMFP_ALLOC(((*f_qual_arr)[arr_idx]->qual_cfg_arr),
                    (stage->qual_cfg_info_db->qual_cfg_info[idx]->num_qual_cfg *
                     sizeof(bcmfp_qual_cfg_t)),
                    "bcmfpFieldQualsArray");

        /* Copy qual info from stage fc to new memory allocated */
        sal_memcpy((*f_qual_arr)[arr_idx]->qual_cfg_arr,
                   stage->qual_cfg_info_db->qual_cfg_info[idx]->qual_cfg_arr,
                   (stage->qual_cfg_info_db->qual_cfg_info[idx]->num_qual_cfg *
                    sizeof(bcmfp_qual_cfg_t)));

        arr_idx++;
        if (arr_idx == (qual_arr_size)) {
            break;
        }
    }

    /*
     * Qualifier array elements are sorted so less frequently present
     * qualifiers appear first.
     */
    sal_qsort((*f_qual_arr), qual_arr_size, sizeof(bcmfp_qual_cfg_info_t *),
              bcmfp_qual_info_cmp);

    SHR_FUNC_EXIT();

exit:
    if (*f_qual_arr) {
        SHR_FREE(((*f_qual_arr)[arr_idx]));
    }
    (void)bcmfp_selcode_qual_arr_free(unit, f_qual_arr, arr_idx);
    SHR_FUNC_EXIT();
}

static int
bcmfp_selector_diff(int unit,
                    bcmfp_ext_codes_t *sel_arr,
                    int part_idx,
                    bcmfp_qual_cfg_t *selector,
                    uint8_t *diff_count)
{
    uint8_t count = 0;
    bcmfp_ext_codes_t *tcam_part = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(sel_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(selector, SHR_E_PARAM);
    SHR_NULL_CHECK(diff_count, SHR_E_PARAM);

    count = 0;
    tcam_part = sel_arr + part_idx;

    /* Qualifier is available in the second part of intraslice entry */
    if (selector->intraslice) {
        if (BCMFP_EXT_SELCODE_DONT_USE == tcam_part->intraslice) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    } else {
        if (BCMFP_EXT_SELCODE_DONT_USE != tcam_part->intraslice) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }

    /* Qualifier is available only in the secondary part of a paired
       entry */

    if (selector->secondary) {
        if (BCMFP_EXT_SELCODE_DONT_USE == tcam_part->secondary) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }

    /* Primary slice selector. */
    switch (selector->e_params[0].section) {
        case BCMFP_EXT_SECTION_DISABLE:
            break;
        case BCMFP_EXT_SECTION_FPF1:
            if (tcam_part->fpf1 == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->fpf1 != selector->e_params[0].sec_val) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_SECTION_FPF2:
            if (tcam_part->fpf2 == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->fpf2 != selector->e_params[0].sec_val) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_SECTION_FPF3:
            if (tcam_part->fpf3 == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->fpf3 != selector->e_params[0].sec_val) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_SECTION_FPF4:
            if (tcam_part->fpf4 == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->fpf4 != selector->e_params[0].sec_val) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Secondary slice selector. */
    /* Per slice selectors are inserted into primary slice only. */
    if ((selector->intraslice) && (0 != part_idx)) {
        tcam_part = sel_arr + (part_idx - 1);
    }

    switch (selector->pri_ctrl_sel) {
        case BCMFP_EXT_CTRL_SEL_DISABLE:
            break;
        case BCMFP_EXT_CTRL_SEL_SRC:
            if (tcam_part->src_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->src_sel != selector->pri_ctrl_sel_val) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_IP_HEADER:
            if (tcam_part->ip_header_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->ip_header_sel !=
                       selector->pri_ctrl_sel_val) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY1_CLASS_ID:
            if (tcam_part->egr_key1_classid_sel ==
                BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key1_classid_sel !=
                       selector->pri_ctrl_sel_val) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY2_CLASS_ID:
            if (tcam_part->egr_key2_classid_sel ==
                BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key2_classid_sel !=
                       selector->pri_ctrl_sel_val) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY3_CLASS_ID:
            if (tcam_part->egr_key3_classid_sel ==
                BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key3_classid_sel !=
                       selector->pri_ctrl_sel_val) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY4_CLASS_ID:
            if (tcam_part->egr_key4_classid_sel ==
                BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key4_classid_sel !=
                       selector->pri_ctrl_sel_val) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY6_CLASS_ID:
            if (tcam_part->egr_key6_classid_sel ==
                BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key6_classid_sel !=
                       selector->pri_ctrl_sel_val) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY7_CLASS_ID:
            if (tcam_part->egr_key7_classid_sel ==
                BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key7_classid_sel !=
                       selector->pri_ctrl_sel_val) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY8_CLASS_ID:
            if (tcam_part->egr_key8_classid_sel ==
                BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key8_classid_sel !=
                       selector->pri_ctrl_sel_val) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY4_DVP:
            if (tcam_part->egr_key4_dvp_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key4_dvp_sel !=
                       selector->pri_ctrl_sel_val) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY8_DVP:
            if (tcam_part->egr_key8_dvp_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key8_dvp_sel !=
                       selector->pri_ctrl_sel_val) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY4_MDL:
            if (tcam_part->egr_key4_mdl_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                count++;
            } else if (tcam_part->egr_key4_mdl_sel !=
                       selector->pri_ctrl_sel_val) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_IP_ADDR:
            if (tcam_part->egr_ip_addr_sel == BCMFP_EXT_SELCODE_DONT_CARE) {
                ++count;
            } else if (tcam_part->egr_ip_addr_sel !=
                       selector->pri_ctrl_sel_val) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Tertiary slice selector */
    switch (selector->sec_ctrl_sel) {
        case BCMFP_EXT_CTRL_SEL_DISABLE:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    *diff_count = count;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_qual_selectors_sort(int unit,
                          bcmfp_ext_codes_t *candidate,
                          uint8_t parts_count,
                          bcmfp_qual_cfg_info_t *f_qual)
{
    uint8_t idx = 0;
    uint8_t conf_idx = 0;
    uint8_t update_count_min = 0;
    uint8_t update_count = 0;
    int rv = 0;
    bcmfp_qual_cfg_t *selector = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(candidate, SHR_E_PARAM);
    SHR_NULL_CHECK(f_qual, SHR_E_PARAM);

    /* Don't sort if only one configuration is available. */
    if (f_qual->num_qual_cfg <= 1) {
        SHR_EXIT();
    }

    /* Get number of updates required for each qualifier configuration. */
    for (conf_idx = 0; conf_idx < f_qual->num_qual_cfg; conf_idx++) {
        update_count_min = -1;
        selector = &(f_qual->qual_cfg_arr[conf_idx]);

        for (idx = 0; idx < parts_count; idx++) {
            update_count = 0;

            /* Test if selector can be inserted in a specific tcam part. */
            rv = bcmfp_selector_diff(unit, candidate, idx,
                                      selector, &update_count);
            if (SHR_FAILURE(rv)) {
                continue;
            }

            if (update_count < update_count_min) {
                update_count_min = update_count;
            }
        }
        /* Keep minimum number of required updates. */
        selector->update_count  =  update_count_min;
    }

    /* Qualifier configurations sort based on number of updates required.  */
    sal_qsort(f_qual->qual_cfg_arr,
              f_qual->num_qual_cfg,
              sizeof(bcmfp_qual_cfg_t),
              bcmfp_selector_update_cmp);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_selector_insert(int unit,
                      bcmfp_ext_codes_t *sel_arr,
                      int part_idx,
                      bcmfp_qual_cfg_t *selector)
{
    bcmfp_ext_codes_t *tcam_part = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(sel_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(selector, SHR_E_PARAM);

    tcam_part = sel_arr + part_idx;

    /* Primary slice selector. */
    switch (selector->e_params[0].section) {
        case BCMFP_EXT_SECTION_DISABLE:
            break;
        case BCMFP_EXT_SECTION_FPF1:
            tcam_part->fpf1 = selector->e_params[0].sec_val;
            break;
        case BCMFP_EXT_SECTION_FPF2:
            tcam_part->fpf2 = selector->e_params[0].sec_val;
            break;
        case BCMFP_EXT_SECTION_FPF3:
            tcam_part->fpf3 = selector->e_params[0].sec_val;
            break;
        case BCMFP_EXT_SECTION_FPF4:
            tcam_part->fpf4 = selector->e_params[0].sec_val;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Per slice selectors are inserted into primary slice only. */
    if ((selector->intraslice) && (0 != part_idx)) {
        tcam_part = sel_arr + (part_idx - 1);
    }

    /* Secondary slice selector. */
    switch (selector->pri_ctrl_sel) {
        case BCMFP_EXT_CTRL_SEL_DISABLE:
            break;
        case BCMFP_EXT_CTRL_SEL_SRC:
            tcam_part->src_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_IP_HEADER:
            tcam_part->ip_header_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY1_CLASS_ID:
            tcam_part->egr_key1_classid_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY2_CLASS_ID:
            tcam_part->egr_key2_classid_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY3_CLASS_ID:
            tcam_part->egr_key3_classid_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY4_CLASS_ID:
            tcam_part->egr_key4_classid_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY6_CLASS_ID:
            tcam_part->egr_key6_classid_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY7_CLASS_ID:
            tcam_part->egr_key7_classid_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY8_CLASS_ID:
            tcam_part->egr_key8_classid_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY4_DVP:
            tcam_part->egr_key4_dvp_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY8_DVP:
            tcam_part->egr_key8_dvp_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_KEY4_MDL:
            tcam_part->egr_key4_mdl_sel = selector->pri_ctrl_sel_val;
            break;
        case BCMFP_EXT_CTRL_SEL_EGR_IP_ADDR:
            tcam_part->egr_ip_addr_sel = selector->pri_ctrl_sel_val;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Tertiary slice selector */
    switch (selector->sec_ctrl_sel) {
        case BCMFP_EXT_CTRL_SEL_DISABLE:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_qual_offsets_copy(int unit,
                        bcmfp_group_t *fg,
                        uint8_t parts_count,
                        uint16_t qual_count,
                        bcmfp_qual_cfg_info_t **f_qual_arr,
                        uint8_t *qual_conf_idx)
{
    bcmfp_qual_cfg_t                 *qual_config  = NULL;
    bcmfp_group_qual_offset_info_t   *q_arr        = NULL;
    bcmfp_qset_t                     qset_assigned;
    int                              qual_idx = 0;
    int                              chunk_idx = 0;
    int                              part_idx = 0;
    uint8_t                          conf_idx = 0;
    uint8_t                          diff = 0;
    bcmfp_qualifier_t                *qual_arr = NULL;
    bcmfp_qual_offset_info_t         *offset_arr = NULL;
    uint16_t                         size = 0;
    int                              rv = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fg, SHR_E_PARAM);
    SHR_NULL_CHECK(f_qual_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(qual_conf_idx, SHR_E_PARAM);

    BCMFP_QSET_INIT(qset_assigned);

    /* Get currently assigned qualifiers. */
    for (part_idx = 0; part_idx < parts_count; part_idx++) {
        q_arr = &(fg->qual_arr[part_idx]);
        for (qual_idx = 0; qual_idx < q_arr->size; qual_idx++) {
            BCMFP_QSET_ADD(qset_assigned, q_arr->qid_arr[qual_idx]);
        }
    }

    for (part_idx = 0; part_idx < parts_count; part_idx++) {
        /* Initialization. */
        offset_arr = NULL;
        qual_arr = NULL;

        /* Get qualifiers array pointer. */
        q_arr = &(fg->qual_arr[part_idx]);
        size = q_arr->size;

        for (qual_idx = 0; qual_idx < qual_count; qual_idx++) {
            /* Check if qualifier was already assigned. */
            if (BCMFP_QSET_TEST(qset_assigned, f_qual_arr[qual_idx]->qual)) {
                continue;
            }

            /* Get qualifier configuration index. */
            conf_idx = qual_conf_idx[qual_idx];
            /* Get qualifier configuration for the index. */
            qual_config = f_qual_arr[qual_idx]->qual_cfg_arr + conf_idx;
            /* Test if selector was inserted in a specific tcam part. */
            rv = bcmfp_selector_diff(unit, fg->ext_codes, part_idx,
                                     qual_config,
                                     &diff);
            if (SHR_FAILURE(rv) || diff) {
                continue;
            }
            size++;
        }

        if (size == 0)  {
            continue;
        }
        /* Allocate qualifiers id and offsets array. */
        BCMFP_ALLOC(qual_arr,
                   (size * sizeof (bcmfp_qualifier_t)),
                   "bcmfGrpQualId");

        BCMFP_ALLOC(offset_arr,
                   (size * sizeof (bcmfp_qual_offset_info_t)),
                   "bcmfpGrpQualOffset");

        /*
         * Copy whatever qualifiers the group already have into the
         * new array.
         */
        if (q_arr->size) {
            sal_memcpy(qual_arr, q_arr->qid_arr,
                    q_arr->size * sizeof (bcmfp_qualifier_t));
            sal_memcpy(offset_arr, q_arr->offset_arr,
                    q_arr->size * sizeof(bcmfp_qual_offset_info_t));
        }
        size = q_arr->size;

        /* Free old group qualifiers. */
        SHR_IF_ERR_EXIT(bcmfp_group_qualifiers_free(unit, fg, part_idx));

        /* Now let the group point to the new and bigger array */
        q_arr->qid_arr = qual_arr;
        q_arr->offset_arr = offset_arr;
        q_arr->size = size;

        for (qual_idx = 0; qual_idx < qual_count; qual_idx++) {
            /* Check if qualifier was already assigned. */
            if (BCMFP_QSET_TEST(qset_assigned, f_qual_arr[qual_idx]->qual)) {
                continue;
            }

            /* Get qualifier configuration index. */
            conf_idx = qual_conf_idx[qual_idx];
            /* Get qualifier configuration for the index. */
            qual_config = f_qual_arr[qual_idx]->qual_cfg_arr + conf_idx;
            /* Test if selector was inserted in a specific tcam part. */
            rv = bcmfp_selector_diff(unit,
                                     fg->ext_codes,
                                     part_idx,
                                     qual_config,
                                     &diff);

            if (SHR_FAILURE(rv) || diff) {
                continue;
            }
            q_arr->qid_arr[q_arr->size] = f_qual_arr[qual_idx]->qual;
            q_arr->offset_arr[q_arr->size].num_offsets =
                                           qual_config->num_chunks;
            for (chunk_idx = 0;
                 chunk_idx < qual_config->num_chunks;
                 chunk_idx++) {
                q_arr->offset_arr[q_arr->size].offset[chunk_idx] =
                    qual_config->e_params[chunk_idx].offset;
                q_arr->offset_arr[q_arr->size].width[chunk_idx] =
                    qual_config->e_params[chunk_idx].width;
            }
            q_arr->size++;
            BCMFP_QSET_ADD(qset_assigned, f_qual_arr[qual_idx]->qual);
        }
    }

    SHR_FUNC_EXIT();

exit:
    SHR_FREE(qual_arr);
    SHR_FREE(offset_arr);
    SHR_FUNC_EXIT();
}


static int
bcmfp_qual_selectors_insert(int unit,
                            bcmfp_ext_codes_t *candidate,
                            uint8_t parts_count,
                            bcmfp_qual_cfg_t *selector)
{
    uint8_t idx = 0;
    uint8_t selected_part = 0;
    uint8_t update_count_min = 0;
    uint8_t update_count = 0;
    int     rv = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(candidate, SHR_E_PARAM);
    SHR_NULL_CHECK(selector, SHR_E_PARAM);

    update_count_min = -1;
    selected_part = -1;

    for (idx = 0; idx < parts_count; idx++) {
        update_count = 0;

        /* Test if selector can be inserted in a specific tcam part. */
        rv = bcmfp_selector_diff(unit,
                                 candidate,
                                 idx,
                                 selector,
                                 &update_count);
        if (SHR_FAILURE(rv)) {
            continue;
        }

        if (update_count < update_count_min) {
            update_count_min = update_count;
            selected_part = idx;
        }
    }

    /* Check if any tcam part can accommodate all the required selectors. */
    if ((uint8_t)-1 == selected_part) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    /* Insert selector configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_selector_insert(unit,
                               candidate,
                               selected_part,
                               selector));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_selcode_allocate(int unit,
                             bcmfp_stage_t *stage,
                             bcmfp_group_t *fg,
                             int qual_count,
                             uint8_t parts_count)
{
    bcmfp_qual_cfg_info_t **f_qual_arr = NULL;
    bcmfp_qual_cfg_t      *qual_config = NULL;
    uint8_t               *qual_conf_idx = NULL;
    bcmfp_ext_codes_t     *sel_arr = NULL;
    bcmfp_ext_codes_t     *candidate = NULL;
    uint8_t               conf_idx = 0;
    int                   qual_idx = 0;
    int                   sel_entry_sz = 0;
    int                   rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    f_qual_arr    = NULL;
    qual_config   = NULL;
    qual_conf_idx = NULL;
    sel_arr       = NULL;
    candidate     = NULL;

    SHR_NULL_CHECK(fg, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    /* Update group selectors based on group flags. */
    SHR_IF_ERR_EXIT(bcmfp_group_selectors_init(unit, fg));

    /* Get qualifiers configuration.          */
    /* NOTE: Must free f_qual_arr at the end. */
    if (qual_count) {
        SHR_IF_ERR_EXIT
            (bcmfp_qualifiers_info_get(unit,
                                       stage,
                                       &fg->qset,
                                       &f_qual_arr,
                                       qual_count));
        /* Allocate and initialize qualifier configurations iterators. */
        BCMFP_ALLOC(qual_conf_idx,
                    (qual_count * sizeof(uint8_t)),
                    "bcmfpFieldQualCfg");
    }

    /* Allocate and initialize select codes status before           */
    /* individual qualifier configuration was applied.              */
    /* Candidate select codes are in the last element of the array. */
    sel_entry_sz = parts_count * sizeof(bcmfp_ext_codes_t);

    BCMFP_ALLOC(sel_arr,
               ((1 + qual_count) * sel_entry_sz),
                "bcmfpFieldGrpSelectCodes");

    /* Initialize first qualifier select codes to group select codes. */
    candidate = sel_arr + (qual_count * parts_count);
    sal_memcpy(candidate, fg->ext_codes, sel_entry_sz);

    /* Initialize first qualifier select codes to a group defaults */
    /* Check to avoid valgrind error in qual_count 0 case */
    if (sel_arr != candidate) {
        sal_memcpy(sel_arr, candidate, sel_entry_sz);
    }
    /* Main select codes selection loop over caller requested qualifier set. */
    for (qual_idx = 0; qual_idx < qual_count;) {

        /* Get qualifier selcodes configuration index. */
        conf_idx = qual_conf_idx[qual_idx];

        /*
         * Sort qualifier configurations so minimum
         * change configuration attempted first.
         */
        if (0 == conf_idx) {
            SHR_IF_ERR_EXIT
                (bcmfp_qual_selectors_sort(unit,
                                           candidate,
                                           parts_count,
                                           f_qual_arr[qual_idx]));
        }

        /*
         * Set default return value, in case all configuration
         * options were exhausted.
         */
        rv = SHR_E_RESOURCE;

        /* Loop over possible qualifier configurations. */
        for (;conf_idx < f_qual_arr[qual_idx]->num_qual_cfg; conf_idx++)  {

            /* Get qualifier configuration for the index. */
            qual_config = f_qual_arr[qual_idx]->qual_cfg_arr + conf_idx;

            /* Skip conflicting configurations. */
            if ((uint8_t) -1 ==
                    qual_config->update_count) {
                continue;
            }

            /*
             *  Attempt to accommodate specific qualifier
             *  configuration into candidate select codes.
             */
            rv = bcmfp_qual_selectors_insert(unit,
                                             candidate,
                                             parts_count,
                                             qual_config);

            if (SHR_SUCCESS(rv) || (SHR_E_RESOURCE != rv)) {
                break;
            }
        }

        /*
         * If code failed to insert qualifier configuration
         * attempt a previous qualifier in the list using alternative
         * configuration.
         */
        if (SHR_FAILURE(rv)) {
            if ((qual_idx == 0) || (SHR_E_RESOURCE != rv)) {
                break;
            }
            /* Previous qualifier has to use next available configuration. */
            while (qual_idx > 0) {
                /* This qualifier configuration will be re-selected. */
                qual_conf_idx[qual_idx] = 0;
                /* Back to the previous qualifier. */
                qual_idx--;
                /* Check select codes were modified by qualifier. */
                qual_config = f_qual_arr[qual_idx]->qual_cfg_arr +
                              qual_conf_idx[qual_idx];
                /* If modified -> reverse changes and proceed to next config.*/
                if ((0 == qual_idx) ||
                    (0 != qual_config->update_count)) {
                    /* Go to the next configuration */
                    qual_conf_idx[qual_idx]++;
                    /* Restore original select codes state. */
                    sal_memcpy(candidate,
                               sel_arr + (qual_idx * parts_count),
                               sel_entry_sz);
                    break;
                }
            }
        } else {
            /* Preserve current qualifier configuration index. */
            qual_conf_idx[qual_idx] = conf_idx;
            /* Proceed to the next qualifier. */
            qual_idx++;
            /* Set selection base line for the next qualifier */
            if (qual_idx < qual_count) {
                sal_memcpy(sel_arr + (qual_idx * parts_count),
                        candidate, sel_entry_sz);
            }
        }
    }
    if (SHR_SUCCESS(rv)) {
        if (qual_count) {
            /* Copy selected codes to the group configuration. */
            sal_memcpy(fg->ext_codes, candidate, sel_entry_sz);
            /* Copy qualifiers offset to group tcam parts. */
            rv = bcmfp_qual_offsets_copy(unit,
                                         fg,
                                         parts_count,
                                         qual_count,
                                         f_qual_arr,
                                         qual_conf_idx);
        }
    }
    SHR_ERR_EXIT(rv);

exit:
    SHR_FREE(qual_conf_idx);
    SHR_FREE(sel_arr);
    (void)bcmfp_selcode_qual_arr_free(unit, &f_qual_arr, qual_count);
    SHR_FUNC_EXIT();
}

int
bcmfp_group_selcode_assign(int unit,
                           bcmfp_group_t *fg,
                           bcmfp_stage_t *stage,
                           int qual_count,
                           uint8_t parts_count,
                           uint8_t reset_flag)
{
    int idx = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fg, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    /*
     * Clear the group's slice Qsets.
     * Selcodes need not be cleared if this function is called from group_set
     */
    for (idx = 0; (idx < BCMFP_ENTRY_PARTS_MAX) && reset_flag ; idx++) {
        bcmfp_ext_codes_init(unit, &(fg->ext_codes[idx]));
        SHR_IF_ERR_EXIT
            (bcmfp_group_qualifiers_free(unit, fg, idx));
    }

    /* Allocate select codes based on group flags. */
    rv = bcmfp_group_selcode_allocate(unit,
                                      stage,
                                      fg,
                                      qual_count,
                                      parts_count);

    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_slice_mode_to_group_mode(int unit,
                                     bcmfp_group_slice_mode_t grp_slice_mode,
                                     bcmfp_group_mode_t *grp_mode)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(grp_mode, SHR_E_PARAM);

    switch (grp_slice_mode) {
        case BCMFP_GROUP_SLICE_MODE_L2_SINGLE_WIDE:
        case BCMFP_GROUP_SLICE_MODE_L3_SINGLE_WIDE:
        case BCMFP_GROUP_SLICE_MODE_L3_ANY_SINGLE_WIDE:
            *grp_mode = BCMFP_GROUP_MODE_SINGLE;
            break;

        case BCMFP_GROUP_SLICE_MODE_L3_DOUBLE_WIDE:
        case BCMFP_GROUP_SLICE_MODE_L3_ANY_DOUBLE_WIDE:
        case BCMFP_GROUP_SLICE_MODE_L3_ALT_DOUBLE_WIDE:
            *grp_mode = BCMFP_GROUP_MODE_DBLINTER;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ext_codes_init(int unit, bcmfp_ext_codes_t *ext_codes)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ext_codes, SHR_E_PARAM);

    sal_memset(ext_codes,
               BCMFP_EXT_SELCODE_DONT_CARE,
               sizeof(bcmfp_ext_codes_t));

    ext_codes->keygen_index = -1;
    ext_codes->ipbm_source = -1;
    ext_codes->ltid = -1;
    ext_codes->default_policy_enable = 0;

exit:
    SHR_FUNC_EXIT();
}
