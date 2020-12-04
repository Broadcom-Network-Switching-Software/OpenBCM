/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <bcm/types.h>
#include <bcm/field.h>
#include <bcm/flexctr.h>
#include <bcmltd/chip/bcmltd_str.h>

#include <bcm_int/control.h>
#include <bcm_int/ltsw/field_int.h>
#include <bcm_int/ltsw/flowtracker_int.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/field.h>
#include <bcm_int/ltsw/field_destination.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/mbcm/field.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/policer.h>
#include <bcm_int/ltsw/mbcm/flowtracker.h>

#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <bcmlt/bcmlt.h>

extern char *
bcmint_field_qual_name(bcm_field_qualify_t qid);
extern int
bcmint_field_qualifier_udf_params_get(
    int unit,
    char *lt_field_name,
    uint32_t qual_idx,
    bool *udf,
    uint8_t *cont);

#define BSL_LOG_MODULE BSL_LS_BCM_FLOWTRACKER


/**************************************************************
 *                 PRIVATE functions                          *
 */

/*
 * This routine is used for extracting EM key from EM entry with
 * help of EM group key offsets information.
 */
static void
bcm_ltsw_flowtracker_em_entry_key_get(
                    int unit,
                    bcm_int_flowtracker_em_group_info_t *em_grp_info,
                    uint8_t num_entry_words,
                    uint32_t *em_entry,
                    uint32_t *em_key)
{
    int      num_bits;
    int      i;

    for (i = 0, num_bits = 0; i < em_grp_info->num_base_entries; i++) {
        shr_bitop_range_copy(em_key, num_bits, /* DST */
                        em_entry, em_grp_info->key_start_offset[i], /* SRC */
                        em_grp_info->key_width[i] /* SIZE */);
        num_bits += em_grp_info->key_width[i];
    }

}

/*
 * This routine is used for extracting flow tuple values from EM key
 * using EM group qualifiers' offsets/width information.
 */
static void
bcm_ltsw_flowtracker_em_key_tuple_get(
                    int unit,
                    bcm_int_flowtracker_em_group_info_t *em_grp_info,
                    uint32_t *em_key,
                    bcm_flowtracker_flow_entry_t *flow_entry)
{
    int         i, j;
    int         qual_offset;
    uint32_t    tuple_values[4];

    flow_entry->num_tuples = em_grp_info->num_quals;
    for (i = 0; i < em_grp_info->num_quals; i++) {
        qual_offset = 0;
        flow_entry->tuple_qualifiers[i] = em_grp_info->qual_info[i].qual;
        sal_memset(tuple_values, 0, sizeof(tuple_values));
        for (j = 0; j < em_grp_info->qual_info[i].num_qual_parts; j++) {
            shr_bitop_range_copy(tuple_values, qual_offset, /* DST */
                em_key, em_grp_info->qual_info[i].part_info[j].offset, /* SRC */
                em_grp_info->qual_info[i].part_info[j].width /* SIZE */);

            qual_offset += em_grp_info->qual_info[i].part_info[j].width;
            if (qual_offset > 128) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                        (BSL_META_U(unit, "Error: Tuple's width"
                                    " greater than 128 %d. Qual %d\n"),
                        qual_offset,em_grp_info->qual_info[i].qual));
                break;
            }
        }
        flow_entry->tuple_values[i].word1 = tuple_values[0];
        flow_entry->tuple_values[i].word2 = tuple_values[1];
        flow_entry->tuple_values[i].word3 = tuple_values[2];
        flow_entry->tuple_values[i].word4 = tuple_values[3];
    }
}

/*
 * This routine is useful for extracting following information.
 *       1) Number of base entries the EM key occupies.
 *       2) Start of key within each base entry.
 *       3) End of key within each base entry.
 *       4) Width of key within each base entry (end - start + 1)
 */
static int
bcm_ltsw_flowtracker_field_em_base_entries_key_offsets_extract(
    int unit,
    bcm_int_flowtracker_em_group_info_t *em_grp_info
    )
{
    bcm_int_flowtracker_em_group_qual_info_t *qual_info = NULL;
    int total_key_width = 0, rem_key_width = 0;
    int num_base_entries = 0;
    uint16_t key_start_offset[FT_EM_KEY_MAX_NUM_BASE_ENTRIES];
    uint16_t key_end_offset[FT_EM_KEY_MAX_NUM_BASE_ENTRIES];
    int i, j, temp = 0;
    uint16_t biggest_offset = 0, biggest_width = 0;
    uint8_t num_quals = em_grp_info->num_quals;
    uint16_t base_entry_size = 0;
    uint16_t key_space_avl[FT_EM_KEY_MAX_NUM_BASE_ENTRIES];
    uint16_t key_type_base_valid_size[FT_EM_KEY_MAX_NUM_BASE_ENTRIES];
    SHR_FUNC_ENTER(unit);

    /*
     * Find the biggest offset and its corresponding width.
     * Sum of it will be the total key width.
     */
    for (i = 0; i < num_quals; i++) {
        qual_info = &(em_grp_info->qual_info[i]);
        for (j = 0; j < qual_info->num_qual_parts; j++) {
            if (qual_info->part_info[j].offset > biggest_offset) {
                biggest_offset = qual_info->part_info[j].offset;
                biggest_width  = qual_info->part_info[j].width;
            }
        }
    }
    total_key_width = biggest_offset + biggest_width;

    rem_key_width = total_key_width;


    /* Get the chip specific em base entry info */
    SHR_IF_ERR_VERBOSE_EXIT(
            mbcm_ltsw_flowtracker_base_entry_info_get(unit,
                &base_entry_size, key_space_avl, key_type_base_valid_size));

    /*
     * This loop fits the key into multiple base entries based on
     * space available for putting key in each base entry
     * and as part of that process calculates the number of base entries
     * used for the key portion.
     */
    while (rem_key_width > 0) {
        temp = rem_key_width;
        /*
         * Key width gets reduced by space available for key in
         * that base entry.
         */
        rem_key_width -= key_space_avl[num_base_entries];
        /*
         * Key start offset will be
         * (number of base entries passed * size of one base entry) +
         * num bits used for base valid/key_type, since base_valid/key_type
         * are always at the start of a base entry.
         */
        key_start_offset[num_base_entries] =
            (num_base_entries * base_entry_size) +
             key_type_base_valid_size[num_base_entries];

        if (rem_key_width <= 0) {
            /*
             * Utilized the key only upto what was required
             * to fill the remaining key.
             */
            key_end_offset[num_base_entries] =
                key_start_offset[num_base_entries] + temp - 1;
        } else {
            /*
             * Utilized the key fully.
             */
            key_end_offset[num_base_entries] =
                key_start_offset[num_base_entries] +
                key_space_avl[num_base_entries] - 1;
        }
        num_base_entries++;
    }

    if (num_base_entries > FT_EM_KEY_MAX_NUM_BASE_ENTRIES) {
        /* Not possible. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    em_grp_info->num_base_entries = num_base_entries;
    for (i = 0; i < num_base_entries; i++) {
        em_grp_info->key_start_offset[i] = key_start_offset[i];
        em_grp_info->key_end_offset[i] = key_end_offset[i];
        em_grp_info->key_width[i] = key_end_offset[i] - key_start_offset[i] + 1;
    }
exit:
    SHR_FUNC_EXIT();
}

/**************************************************************
 *                 PUBLIC functions                          *
 */

/* Extracts the flow tuple from EM entry using the EM group information. */
void
bcm_ltsw_flowtracker_flow_tuple_extract(
                            int unit,
                            bcm_int_flowtracker_em_group_info_t *em_grp_info,
                            uint8_t num_entry_words,
                            uint32_t *em_entry,
                            bcm_flowtracker_flow_entry_t *flow_entry
                            )
{
    uint32_t em_key[FLOWTRACKER_ENTRY_DATA_WORDS_MAX];

    /* First extract the EM key alone from the entry */
    bcm_ltsw_flowtracker_em_entry_key_get(unit, em_grp_info, num_entry_words,
                                          em_entry, em_key);

    /* Extract the flow tuple from the EM key */
    bcm_ltsw_flowtracker_em_key_tuple_get(unit, em_grp_info,
                                          em_key, flow_entry);
}

/*
 * Looks up FP info LTs using the EM group ID to find out
 *    1) number of qualifiers
 *    2) qualifier list
 *    3) number of qualifier parts
 *    4) list of qualifier parts's offsets and widths
 * Logic was taken from bcm_ltsw_field_entry_dump used for "fp show" CLI.
 */
int
bcm_ltsw_flowtracker_field_em_group_info_get(
    int unit,
    int group_id,
    bcm_int_flowtracker_em_group_info_t *em_grp_info
    )
{
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmlt_entry_handle_t grp_info_templ = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t grp_info_qual_templ[4] = {BCMLT_INVALID_HDL};
    int ct = 0;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcm_field_qual_map_t *lt_qual_map = NULL;
    bcmlt_field_def_t *fld_defs_ref = NULL;
    bcmlt_field_def_t *fld_defs_iter = NULL;
    bcmlt_field_def_t *fld_defs_qual_info = NULL;
    int iter = 0;
    uint64_t *qual_value_num_offset_buf = NULL;
    uint64_t *qual_value_offset_buf = NULL;
    uint64_t *qual_value_width_buf = NULL;
    uint32_t num_element = 0;
    int map_count= 0;
    uint8_t id = 0;
    bcm_field_action_params_t params;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    uint64_t num_offsets = 0;
    uint8_t part_id = 0;
    uint64_t num_parts = 0;
    uint32_t offset = 0, width = 0;
    uint16_t offset_index = 0, width_index = 0;
    char *num_offset_field_name = NULL;
    char *offset_field_name = NULL;
    char *width_field_name = NULL;
    char *lt_field_name = NULL;
    bcm_field_qset_t grp_qset;
    bcmi_ltsw_field_stage_t stage_id = bcmiFieldStageFlowTracker;
    int curr_qual_idx = 0;
    uint8_t cont = 0;
    bool udf = FALSE;
    int i = 0,start = 0, end = 0, tmp = 0;

    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    if (em_grp_info->em_group_info_valid == true) {
        /* Info already present. Simply return. */
        SHR_EXIT();
    }

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id,
                                           &stage_info));



    BCMINT_FIELD_MEM_ALLOC(fld_defs_qual_info,
                           sizeof(bcmlt_field_def_t),
                           "field definitions");

    tbls_info =  stage_info->tbls_info;


    sal_memset(&params, 0 ,sizeof(bcm_field_action_params_t));


    /* Allocate and Read grp info template of source entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &grp_info_templ,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL,
                                      tbls_info->group_info_sid,
                                      tbls_info->group_info_key_fid,
                                      0, group_id, NULL,
                                      NULL,
                                      0,
                                      0, 0,
                                      0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(grp_info_templ,
                               tbls_info->group_info_num_parts_fid,
                               &num_parts));

    if (num_parts > 1 ) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Error: Number of partitions"
                            " greater than 1.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Allocate and Read grp info qual template of source entry */
    for (part_id = 1; part_id <= num_parts; part_id++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_lt_entry_commit(unit, &grp_info_qual_templ[part_id - 1],
                                          BCMLT_OPCODE_LOOKUP,
                                          BCMLT_PRIORITY_NORMAL,
                                          tbls_info->group_info_partition_sid,
                                          tbls_info->group_info_partition_key_fid,
                                          0, group_id, NULL,
                                          tbls_info->group_info_common_partition_key_fid,
                                          part_id,
                                          0, 0,
                                          0));
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmint_field_group_oper_lookup(unit,
                                       group_id,
                                       stage_info,
                                       &group_oper));

    BCM_FIELD_QSET_INIT(grp_qset);
    FP_HA_GROUP_OPER_QSET_ARR_GET(unit, group_oper, grp_qset);

    em_grp_info->num_quals = 0;
    for (ct = 0; ct < group_oper->qual_cnt; ct++) {
        if (BCMINT_FIELD_IS_PSEUDO_QUAL((int)group_oper->qset_arr[ct])) {
            continue;
        }
        curr_qual_idx = em_grp_info->num_quals;
        /* Only count non pseudo qualifiers */
        em_grp_info->num_quals = em_grp_info->num_quals + 1;
        offset_index = 0;
        width_index = 0;

        em_grp_info->qual_info[curr_qual_idx].qual = group_oper->qset_arr[ct];

       /* Find lt map from db for given qualifier in group qset */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_qual_map_find(unit, group_oper->qset_arr[ct],
                                        stage_info, 0, 0, &map_count,
                                        &lt_qual_map));
        sal_memset(fld_defs_qual_info, 0, sizeof(bcmlt_field_def_t));

        BCMINT_FIELD_MEM_ALLOC
            (fld_defs_ref,
            sizeof(bcmlt_field_def_t) * (lt_qual_map->num_maps),
            "field definitions");

        fld_defs_iter = fld_defs_ref;

        for (iter = (lt_qual_map->num_maps - 1); iter >=0 ; iter--) {
            if (FALSE == bcmint_field_map_supported(unit, stage_info,
                (lt_qual_map->map[iter]))) {
                fld_defs_iter++;
                continue;
            }

            lt_field_name = NULL;
            /* Get Field Qualifier name for group template */
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_field_lt_qual_name_update(unit, 0, 0,
                   stage_info, (const bcm_field_qset_t *) &grp_qset,
                   &(lt_qual_map->map[iter]),
                   &lt_field_name));

            /* Retreive field definition for provided lt field in map */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit,
                    tbls_info->rule_sid,
                    lt_field_name, fld_defs_iter));
            if (0 == fld_defs_iter->elements) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }

            /* UDF specific handling */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_qualifier_udf_params_get(unit,
                                                       lt_field_name,
                                                       group_oper->qset_arr[ct],
                                                       &udf,
                                                       &cont));

            for (part_id = 1; part_id <= num_parts; part_id++) {

                BCMINT_FIELD_MEM_ALLOC
                    (num_offset_field_name,
                    (strlen(lt_field_name) + 15),
                    "qualifier field num offset");
                BCMINT_FIELD_MEM_ALLOC
                    (offset_field_name,
                    (strlen(lt_field_name) + 15),
                    "qualifier field offsets");
                BCMINT_FIELD_MEM_ALLOC
                    (width_field_name,
                    (strlen(lt_field_name) + 15),
                    "qualifier field widths");
                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_field_qual_info_lt_name_get(
                        unit,
                        lt_field_name,
                        num_offset_field_name,
                        offset_field_name,
                        width_field_name));

                /* Retreive field definition for provided lt field in map */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_field_def_get(unit,
                        tbls_info->group_info_partition_sid,
                        num_offset_field_name, fld_defs_qual_info));
                if (fld_defs_qual_info->elements > 1) {
                    BCMINT_FIELD_MEM_ALLOC
                        (qual_value_num_offset_buf,
                        sizeof(uint64_t) * (fld_defs_qual_info->elements),
                        "qualifier num offset");

                    num_element = fld_defs_qual_info->elements;

                    /* Retreive from qualifier value */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_get(
                                             grp_info_qual_templ[part_id-1],
                                             num_offset_field_name,
                                             0, qual_value_num_offset_buf,
                                             num_element,
                                             &num_element));
                    num_offsets = qual_value_num_offset_buf[cont];

                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(grp_info_qual_templ[part_id-1],
                                           num_offset_field_name,
                                           &num_offsets));
                }

                /* Retreive field definition for provided lt field in map */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_field_def_get(unit,
                        tbls_info->group_info_partition_sid,
                        offset_field_name, fld_defs_qual_info));

                BCMINT_FIELD_MEM_ALLOC
                    (qual_value_offset_buf,
                    sizeof(uint64_t) * (fld_defs_qual_info->elements),
                    "qualifier offset");

                num_element = fld_defs_qual_info->elements;

                /* Retreive from qualifier value */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_get(grp_info_qual_templ[part_id-1],
                                             offset_field_name,
                                             0, qual_value_offset_buf,
                                             num_element,
                                             &num_element));

                /* Retreive field definition for provided lt field in map */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_field_def_get(unit,
                        tbls_info->group_info_partition_sid,
                        width_field_name, fld_defs_qual_info));

                BCMINT_FIELD_MEM_ALLOC
                    (qual_value_width_buf,
                    sizeof(uint64_t) * (fld_defs_qual_info->elements),
                    "qualifier width");

                num_element = fld_defs_qual_info->elements;

                /* Retreive from qualifier value */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_get(grp_info_qual_templ[part_id-1],
                                             width_field_name,
                                             0, qual_value_width_buf,
                                             num_element,
                                             &num_element));

                em_grp_info->qual_info[curr_qual_idx].num_qual_parts = num_offsets;

                start = 0;
                end = num_offsets;
                if (udf) {
                    for (i = 0; i < cont; i++) {
                        start += qual_value_num_offset_buf[i];
                    }
                    end = start + qual_value_num_offset_buf[cont];
                }
                tmp = 0;
                for (id = start; id < end; id++) {
                     offset = COMPILER_64_LO(qual_value_offset_buf[id]);
                     width = COMPILER_64_LO(qual_value_width_buf[id]);
                     em_grp_info->qual_info[curr_qual_idx].part_info[tmp].offset = offset;
                     em_grp_info->qual_info[curr_qual_idx].part_info[tmp].width = width;
                     offset_index++;
                     width_index++;
                     tmp++;
                }
                BCMINT_FIELD_MEM_FREE(num_offset_field_name);
                BCMINT_FIELD_MEM_FREE(offset_field_name);
                BCMINT_FIELD_MEM_FREE(width_field_name);
                BCMINT_FIELD_MEM_FREE(qual_value_offset_buf);
                BCMINT_FIELD_MEM_FREE(qual_value_num_offset_buf);
                BCMINT_FIELD_MEM_FREE(qual_value_width_buf);
            }
            fld_defs_iter++;
        } /* end of for */

        BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcm_ltsw_flowtracker_field_em_base_entries_key_offsets_extract(
                                                                unit,
                                                                em_grp_info));
    /* All info collected for EM group. Set valid = true */
    em_grp_info->em_group_info_valid = true;
exit:
    if (BCMLT_INVALID_HDL != grp_info_templ) {
        (void) bcmlt_entry_free(grp_info_templ);
    }
    for (iter = 0; iter < 4; iter++) {
        if (BCMLT_INVALID_HDL != grp_info_qual_templ[iter]) {
            (void) bcmlt_entry_free(grp_info_qual_templ[iter]);
        }
    }
    if (fld_defs_ref != NULL) {
        BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    }
    if (qual_value_width_buf != NULL) {
        BCMINT_FIELD_MEM_FREE(qual_value_width_buf);
    }
    if (num_offset_field_name != NULL) {
        BCMINT_FIELD_MEM_FREE(num_offset_field_name);
    }
    if (offset_field_name != NULL) {
        BCMINT_FIELD_MEM_FREE(offset_field_name);
    }
    if (width_field_name != NULL) {
        BCMINT_FIELD_MEM_FREE(width_field_name);
    }
    if (qual_value_offset_buf != NULL) {
        BCMINT_FIELD_MEM_FREE(qual_value_offset_buf);
    }
    if (qual_value_num_offset_buf != NULL) {
        BCMINT_FIELD_MEM_FREE(qual_value_num_offset_buf);
    }
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}
