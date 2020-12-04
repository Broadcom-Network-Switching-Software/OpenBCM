/*! \file bcmfp_group.c
 *
 * FP group related APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdi/bcmdi.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_types_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_group_parts_count(int unit,
                        bool is_presel,
                        uint32_t group_flags,
                        uint8_t *part_count)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(part_count, SHR_E_PARAM);

    /* Check group flags settings and determine number of TCAM entry parts. */
    if (group_flags & BCMFP_GROUP_SPAN_SINGLE_SLICE) {
        if ((group_flags & BCMFP_GROUP_INTRASLICE_DOUBLEWIDE))  {
            if (is_presel)  {
                /* ISDW use one presel */
                *part_count = 1;
            } else {
                *part_count = 2;
            }
        } else {
            *part_count = 1;
        }
    } else if (group_flags & BCMFP_GROUP_SPAN_DOUBLE_SLICE) {
        if ((group_flags & BCMFP_GROUP_INTRASLICE_DOUBLEWIDE))  {
            *part_count = 4;
        } else {
            *part_count = 2;
        }
    } else if (group_flags & BCMFP_GROUP_SPAN_TRIPLE_SLICE) {
        *part_count = 3;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_qual_bitmap_get(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_id_t group_id,
                            bcmfp_qualifier_t qual,
                            uint32_t *qual_bitmap)
{
    uint32_t qual_size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(qual_bitmap, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmfp_qual_data_size_get(unit, stage_id, qual, &qual_size));

    SHR_BITSET_RANGE(qual_bitmap, 0, qual_size);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_action_res_id_allocate(int unit,
                                   int tbl_inst,
                                   bcmfp_stage_id_t stage_id,
                                   uint8_t *action_res_id,
                                   uint32_t group_prio)
{
    int rv;
    uint8_t id = 0;
    uint8_t num_action_res_ids = 0;
    bcmfp_stage_id_t shr_stage_id = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_stage_t *shr_stage = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmfp_group_oper_info_t *shr_group_oper_info = NULL;
    uint16_t idx = 0;
    bcmfp_group_action_res_id_bmp_t action_res_id_bmp;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action_res_id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   BCMFP_GROUP_ID_ALL,
                                   &group_oper_info));

    *action_res_id = 0;
    num_action_res_ids = 0;
    sal_memset(&action_res_id_bmp, 0,
                     sizeof(bcmfp_group_action_res_id_bmp_t));

    for (idx = 0; idx < stage->misc_info->num_groups; idx++)  {
        if (group_oper_info[idx].valid == FALSE) {
            continue;
        }

        /*
         * For global mode(tbl_inst=-1), the same action_res_id has to be free
         * for all the pipes, to be used in group creation. So in global mode
         * set the  pbmp with all the action_res_ids in use, irrespective of the
         * instances. This pbmp contains the action_res_id in use for both EM
         * and IFP */
        if ((tbl_inst == -1) ||
            (group_oper_info[idx].tbl_inst == -1) ||
            (group_oper_info[idx].tbl_inst == tbl_inst)) {
            /*
             * if any other group with the same priority exists.
             * allocate the same action_res_id of the matching group
             * to the incoming group. Valid only for non HASH stages.
             * Priority is not valid for EM groups and all groups
             * are created with same priority.
             */
            if (!(BCMFP_STAGE_FLAGS_TEST(stage,
                        BCMFP_STAGE_ENTRY_TYPE_HASH))) {
                if (group_oper_info[idx].group_prio == group_prio) {
                    *action_res_id = group_oper_info[idx].action_res_id;
                    SHR_EXIT();
                }
            }

            if (!SHR_BITGET(action_res_id_bmp.w,
                            group_oper_info[idx].action_res_id)) {
                SHR_BITSET(action_res_id_bmp.w,
                           group_oper_info[idx].action_res_id);
                num_action_res_ids++;
            }
        }
    }

    /*
     * Loop through all other stages with which this stage is sharing the
     * action resolution id space.
     */
    for (shr_stage_id = 0;
         shr_stage_id < BCMFP_STAGE_ID_COUNT;
         shr_stage_id++) {
        if ((stage->misc_info->shared_action_res_id_stage_id_bmp &
            (1 << shr_stage_id)) == 0) {
            continue;
        }
        rv = bcmfp_stage_get(unit, shr_stage_id, &shr_stage);
        if (SHR_SUCCESS(rv) && (shr_stage)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_group_oper_info_get(unit,
                                           shr_stage_id,
                                           BCMFP_GROUP_ID_ALL,
                                           &shr_group_oper_info));
           for (idx = 0; idx < shr_stage->misc_info->num_groups; idx++)  {
               if (shr_group_oper_info[idx].valid == FALSE) {
                   continue;
               }
               if ((tbl_inst == -1) ||
                   (shr_group_oper_info[idx].tbl_inst == -1) ||
                   (shr_group_oper_info[idx].tbl_inst == tbl_inst)) {
                   if (!SHR_BITGET(action_res_id_bmp.w,
                       shr_group_oper_info[idx].action_res_id)) {
                       SHR_BITSET(action_res_id_bmp.w,
                                  shr_group_oper_info[idx].action_res_id);
                       num_action_res_ids++;
                   }
               }
           }
        }
        shr_stage = NULL;
        shr_group_oper_info = NULL;
    }

    if (num_action_res_ids == stage->num_group_action_res_ids) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    for (id = 0; id < stage->num_group_action_res_ids; id++) {
        if (!SHR_BITGET(action_res_id_bmp.w, id)) {
            *action_res_id = id;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_selcode_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_group_oper_info_t *group_oper_info,
                        bcmptm_rm_tcam_fp_group_selcode_t *selcode)
{
    uint8_t  part = 0;
    uint8_t  parts_count = 0;
    bcmfp_ext_codes_t  *ext_ptr;
    bcmptm_rm_tcam_fp_group_selcode_t  *sel_ptr;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(group_oper_info, SHR_E_PARAM);
    SHR_NULL_CHECK(selcode, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 group_oper_info->flags,
                                &parts_count));

    /* Fill in selcodes to be pass for entry insertion */
    sal_memset(selcode, -1,
       (sizeof(bcmptm_rm_tcam_fp_group_selcode_t) * BCMPTM_TCAM_FP_MAX_PARTS));

    for (part = 0; part < parts_count; part++) {
        sel_ptr = &selcode[part];
        ext_ptr = &group_oper_info->ext_codes[part];
        sel_ptr->intraslice = ext_ptr->intraslice;
        sel_ptr->fpf2       = ext_ptr->fpf2;
        sel_ptr->fpf3       = ext_ptr->fpf3;
        if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_KEY_TYPE_FIXED)) {
            sel_ptr->egr_key1_classid_sel = ext_ptr->egr_key1_classid_sel;
            sel_ptr->egr_key2_classid_sel = ext_ptr->egr_key2_classid_sel;
            sel_ptr->egr_key3_classid_sel = ext_ptr->egr_key3_classid_sel;
            sel_ptr->egr_key4_classid_sel = ext_ptr->egr_key4_classid_sel;
            sel_ptr->egr_key6_classid_sel = ext_ptr->egr_key6_classid_sel;
            sel_ptr->egr_key7_classid_sel = ext_ptr->egr_key7_classid_sel;
            sel_ptr->egr_key8_classid_sel = ext_ptr->egr_key8_classid_sel;
            sel_ptr->egr_key4_dvp_sel     = ext_ptr->egr_key4_dvp_sel;
            sel_ptr->egr_key8_dvp_sel     = ext_ptr->egr_key8_dvp_sel;
            sel_ptr->egr_key4_mdl_sel     = ext_ptr->egr_key4_mdl_sel;
            sel_ptr->egr_ip_addr_sel      = ext_ptr->egr_ip_addr_sel;
        } else {
            sel_ptr->src_sel       = ext_ptr->src_sel;
            sel_ptr->ip_header_sel = ext_ptr->ip_header_sel;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_combined_selcode_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmfp_group_t *fg)
{
    uint8_t idx = 0;
    uint8_t  part = 0;
    uint8_t  parts_count = 0;
    bcmfp_ext_codes_t  *ext_ptr;
    bcmfp_ext_codes_t  *sel_ptr;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    if (!BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_KEY_TYPE_FIXED)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                  stage->stage_id,
                                  BCMFP_GROUP_ID_ALL,
                                  &group_oper_info));

    parts_count = (fg->group_mode == BCMFP_GROUP_MODE_SINGLE) ? 1 : 2;

    for (idx = 0; idx < stage->misc_info->num_groups; idx++) {
        if (group_oper_info[idx].valid == FALSE) {
            continue;
        }
        if ((group_oper_info[idx].group_id == fg->group_id) ||
                (group_oper_info[idx].tbl_inst != fg->pipe_id) ||
                (group_oper_info[idx].group_mode != fg->group_mode) ||
                (group_oper_info[idx].group_prio != fg->group_prio)) {
            continue;
        }

        /* Fill in selcodes to be pass for entry insertion */
        for (part = 0; part < parts_count; part++) {
            sel_ptr = &fg->ext_codes[part];
            ext_ptr = &(group_oper_info[idx].ext_codes[part]);

            if (sel_ptr->egr_key1_classid_sel != -1 &&
                    ext_ptr->egr_key1_classid_sel != -1 &&
                    sel_ptr->egr_key1_classid_sel !=
                    ext_ptr->egr_key1_classid_sel) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key1_classid_sel != -1) {
                sel_ptr->egr_key1_classid_sel = ext_ptr->egr_key1_classid_sel;
            }

            if (sel_ptr->egr_key2_classid_sel != -1 &&
                    ext_ptr->egr_key2_classid_sel != -1 &&
                    sel_ptr->egr_key2_classid_sel !=
                    ext_ptr->egr_key2_classid_sel) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key2_classid_sel != -1) {
                sel_ptr->egr_key2_classid_sel = ext_ptr->egr_key2_classid_sel;
            }

            if (sel_ptr->egr_key3_classid_sel != -1 &&
                    ext_ptr->egr_key3_classid_sel != -1 &&
                    sel_ptr->egr_key3_classid_sel !=
                    ext_ptr->egr_key3_classid_sel) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key3_classid_sel != -1) {
                sel_ptr->egr_key3_classid_sel = ext_ptr->egr_key3_classid_sel;
            }

            if (sel_ptr->egr_key4_classid_sel != -1 &&
                    ext_ptr->egr_key4_classid_sel != -1 &&
                    sel_ptr->egr_key4_classid_sel !=
                    ext_ptr->egr_key4_classid_sel) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key4_classid_sel != -1) {
                sel_ptr->egr_key4_classid_sel = ext_ptr->egr_key4_classid_sel;
            }

            if (sel_ptr->egr_key6_classid_sel != -1 &&
                    ext_ptr->egr_key6_classid_sel != -1 &&
                    sel_ptr->egr_key6_classid_sel !=
                    ext_ptr->egr_key6_classid_sel) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key6_classid_sel != -1) {
                sel_ptr->egr_key6_classid_sel = ext_ptr->egr_key6_classid_sel;
            }

            if (sel_ptr->egr_key7_classid_sel != -1 &&
                    ext_ptr->egr_key7_classid_sel != -1 &&
                    sel_ptr->egr_key7_classid_sel !=
                    ext_ptr->egr_key7_classid_sel) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key7_classid_sel != -1) {
                sel_ptr->egr_key7_classid_sel = ext_ptr->egr_key7_classid_sel;
            }

            if (sel_ptr->egr_key8_classid_sel != -1 &&
                    ext_ptr->egr_key8_classid_sel != -1 &&
                    sel_ptr->egr_key8_classid_sel !=
                    ext_ptr->egr_key8_classid_sel) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key8_classid_sel != -1) {
                sel_ptr->egr_key8_classid_sel = ext_ptr->egr_key8_classid_sel;
            }

            if (sel_ptr->egr_key4_dvp_sel != -1 &&
                    ext_ptr->egr_key4_dvp_sel != -1 &&
                    sel_ptr->egr_key4_dvp_sel !=
                    ext_ptr->egr_key4_dvp_sel) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key4_dvp_sel != -1) {
                sel_ptr->egr_key4_dvp_sel     = ext_ptr->egr_key4_dvp_sel;
            }

            if (sel_ptr->egr_key8_dvp_sel != -1 &&
                    ext_ptr->egr_key8_dvp_sel != -1 &&
                    sel_ptr->egr_key8_dvp_sel !=
                    ext_ptr->egr_key8_dvp_sel) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key8_dvp_sel != -1) {
                sel_ptr->egr_key8_dvp_sel     = ext_ptr->egr_key8_dvp_sel;
            }

            if (sel_ptr->egr_key4_mdl_sel != -1 &&
                    ext_ptr->egr_key4_mdl_sel != -1 &&
                    sel_ptr->egr_key4_mdl_sel !=
                    ext_ptr->egr_key4_mdl_sel) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_key4_mdl_sel != -1) {
                sel_ptr->egr_key4_mdl_sel     = ext_ptr->egr_key4_mdl_sel;
            }

            if (sel_ptr->egr_ip_addr_sel != -1 &&
                    ext_ptr->egr_ip_addr_sel != -1 &&
                    sel_ptr->egr_ip_addr_sel !=
                    ext_ptr->egr_ip_addr_sel) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            } else if (ext_ptr->egr_ip_addr_sel != -1) {
                sel_ptr->egr_ip_addr_sel      = ext_ptr->egr_ip_addr_sel;
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/* Below function can be enhanced to do all presel related checks
 * for all relevant stages
 */
int bcmfp_group_presel_entry_validate(int unit,
                                      int tbl_inst,
                                      bcmfp_stage_id_t stage_id,
                                      uint8_t lookup_id,
                                      uint8_t num_presel_ids)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    uint16_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmfp_stage_get(unit,
                                    stage_id,
                                    &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                  stage_id,
                                  BCMFP_GROUP_ID_ALL,
                                  &group_oper_info));

    for (idx = 0; idx < stage->misc_info->num_groups; idx++)  {
        if (group_oper_info[idx].valid == FALSE) {
            continue;
        }
        /* Two groups with default presel cannot have same lookup_id/slice_id */
        if (group_oper_info[idx].tbl_inst == tbl_inst) {
            if ((group_oper_info[idx].slice_id == lookup_id)
                && (group_oper_info[idx].flags & BCMFP_GROUP_WITH_DEFAULT_PRESEL)
                && (0 == num_presel_ids)) {
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_mode_to_flags_get(int unit,
                              bcmfp_stage_t *stage,
                              bcmfp_group_mode_t group_mode,
                              uint32_t *group_flags)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(group_flags, SHR_E_PARAM);

    if (BCMFP_STAGE_FLAGS_TEST(stage,
                               BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
        *group_flags = BCMFP_GROUP_SPAN_SINGLE_SLICE;
    } else {
        if (group_mode == BCMFP_GROUP_MODE_HALF ||
            group_mode == BCMFP_GROUP_MODE_SINGLE ||
            group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_NARROW ||
            group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_WIDE) {
            *group_flags = BCMFP_GROUP_SPAN_SINGLE_SLICE;
        } else if (group_mode == BCMFP_GROUP_MODE_DBLINTRA) {
            *group_flags = (BCMFP_GROUP_INTRASLICE_DOUBLEWIDE |
                            BCMFP_GROUP_SPAN_SINGLE_SLICE);
        } else if (group_mode == BCMFP_GROUP_MODE_DBLINTER) {
            *group_flags = BCMFP_GROUP_SPAN_DOUBLE_SLICE;
        } else if (group_mode == BCMFP_GROUP_MODE_TRIPLE) {
            *group_flags = BCMFP_GROUP_SPAN_TRIPLE_SLICE;
        } else if (group_mode == BCMFP_GROUP_MODE_QUAD) {
            *group_flags = (BCMFP_GROUP_SPAN_DOUBLE_SLICE |
                            BCMFP_GROUP_INTRASLICE_DOUBLEWIDE);
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
exit:
    SHR_FUNC_EXIT();
}
