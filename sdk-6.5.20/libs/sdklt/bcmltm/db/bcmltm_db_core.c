/*! \file bcmltm_db_core.c
 *
 * Logical Table Manager - Core functions.
 *
 * This file contains routines that provide general information
 * for logical tables derived from the LRD table definition and maps.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <bsl/bsl.h>

#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/chip/bcmlrd_limits.h>

#include <bcmltm/bcmltm_types.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>
#include <bcmltm/bcmltm_db_types_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*
 * Flags used in field_[count|list]_get() to indicate desired fields to return.
 *
 * KEY         - Key fields
 * VALUE       - Value fields
 * MAPPED_ONLY - Only mapped fields (if not specified, then get
 *               all defined fields, mapped and unmapped)
 */
#define FIELD_F_KEY            0x00000001
#define FIELD_F_VALUE          0x00000002
#define FIELD_F_MAPPED_ONLY    0x00000004


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the table map type.
 *
 * This function gets the map type for the given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] type Table map type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
map_type_get(int unit, bcmlrd_sid_t sid, bcmltm_table_map_type_t *type)
{
    bcmltm_db_map_group_count_t group_count;
    uint32_t dm_count;
    uint32_t cth_count;
    uint32_t ltm_count;
    uint32_t unique_count = 0;

    SHR_FUNC_ENTER(unit);

    *type = BCMLTM_TABLE_MAP_NONE;

    /* Get group map counts */
    SHR_IF_ERR_EXIT
        (bcmltm_db_map_group_count_get(unit, sid, &group_count));

    dm_count = group_count.dm;
    cth_count = group_count.cth;
    ltm_count = group_count.ltm;

    /* Sanity checks */
    /*
     * Rule:
     * - A table can contain groups of only 1 kind of these: DM, CTH, LTM.
     * - A table can have several PT groups (direct PT maps).
     * - A table can have only 1 CTH group.
     * - A table can have only 1 LTM group.
     */
    if (dm_count > 0) {
        unique_count++;
    }
    if (cth_count > 0) {
        unique_count++;
    }
    if (ltm_count > 0) {
        unique_count++;
    }
    if (unique_count > 1) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LT table cannot have more than 1 kind of "
                              "map group: %s(ltid=%d) "
                              "dm_count=%d cth_count=%d ltm_count=%d\n"),
                   table_name, sid, dm_count, cth_count, ltm_count));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Check that CTH and LTM map groups are no more than 1 */
    if (cth_count > 1) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LT table cannot have more than 1 CTH group: "
                              "%s(ltid=%d) cth_count=%d\n"),
                   table_name, sid, cth_count));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (ltm_count > 1) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LT table cannot have more than 1 LTM group: "
                              "%s(ltid=%d) ltm_count=%d\n"),
                   table_name, sid, ltm_count));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (ltm_count > 0) {
        *type = BCMLTM_TABLE_MAP_LTM;
    } else if (cth_count > 0) {
        *type = BCMLTM_TABLE_MAP_CTH;
    } else if (dm_count > 0) {
        *type = BCMLTM_TABLE_MAP_DIRECT;
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the supported opcodes.
 *
 * This routine returns the supported opcodes given the logical table map.
 *
 * \param [in] map LRD map.
 *
 * \retval Supported table opcodes.
 */
static bcmltm_opcodes_t
map_opcodes_get(const bcmlrd_map_t *map)
{
    bcmltm_opcodes_t opcodes = 0x0;

    if (map->entry_ops & BCMLRD_MAP_TABLE_ENTRY_OPERATION_INSERT) {
        opcodes |= BCMLTM_OPCODE_FLAG_INSERT;
    }
    if (map->entry_ops & BCMLRD_MAP_TABLE_ENTRY_OPERATION_LOOKUP) {
        opcodes |= BCMLTM_OPCODE_FLAG_LOOKUP;
    }
    if (map->entry_ops & BCMLRD_MAP_TABLE_ENTRY_OPERATION_UPDATE) {
        opcodes |= BCMLTM_OPCODE_FLAG_UPDATE;
    }
    if (map->entry_ops & BCMLRD_MAP_TABLE_ENTRY_OPERATION_DELETE) {
        opcodes |= BCMLTM_OPCODE_FLAG_DELETE;
    }
    if (map->entry_ops & BCMLRD_MAP_TABLE_ENTRY_OPERATION_TRAVERSE) {
        opcodes |= BCMLTM_OPCODE_FLAG_TRAVERSE;
    }

    return opcodes;
}

/*!
 * \brief Get LTM min, max, and size for given LRD table attribute values.
 *
 * LRD outputs one of these:
 *     SIZE
 *     MIN/MAX
 *
 * LTM needs all three, min, max, and size for its metadata construction.
 * This routine gets all three values from the LRD data.
 *
 * Input values are the LRD attribute values.
 * Output values are the LTM adjusted values.
 *
 * \param [in,out] min Table minimum limit value.
 * \param [in,out] max  Table maximum limit value.
 * \param [in,out] size Table limit size.
 */
static void
attr_limits_get(uint32_t *min, uint32_t *max, uint32_t *size)
{
    uint32_t amin, amax, asize;

    amin = *min;
    amax = *max;
    asize = *size;

    if (asize != 0) {
        /*
         * Set MIN/MAX based on SIZE if these are zero.
         * If non-zero, assume LRD data is correct (no check needed).
         */
        if ((amin == 0) && (amax == 0)) {
            /* Set min starting at zero, and max accordingly */
            amax = asize - 1;
        }
    } else {
        asize = amax - amin + 1;
    }

    *min = amin;
    *max = amax;
    *size = asize;

    return;
}

/*!
 * \brief Get the min, max, and size for the table PT SID selector.
 *
 * This routine gets the min, max, and size limits for the PT SID selector
 * on a a logical table.
 *
 * PT SID selectors are specified through table transforms.
 * Note that size of the PT SID selectors must be the same across all
 * table transforms (assumes FLTG checks this condition is met).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] min Minimum value for PT SID selector.
 * \param [out] max Maximum value for PT SID selector.
 * \param [out] size Size of PT SID selector (zero if there is no PT select).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pt_sid_sel_limits_get(int unit, bcmlrd_sid_t sid,
                      uint32_t *min, uint32_t *max, uint32_t *size)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    const bcmltd_xfrm_handler_t *xfrm = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    uint32_t num_sid_sel = 0;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);

    *min = *max = *size = 0;

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    /* Find PT SID selector (specified through table transforms) */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];
        /* Skip non-PT groups */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        /*
         * Check map entry
         *
         * It is only necessary to find one entry per group
         * since PT SID selection is one per PT group.
         */
        for (entry_idx = 0; (entry_idx < group->entries) && !found;
             entry_idx++) {
            entry = &group->entry[entry_idx];

            switch(entry->entry_type) {
            case BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER:
                xfrm = bcmltm_lta_intf_xfrm_handler_get(unit,
                                                        entry->u.xfrm.desc->handler_id);
                if (xfrm != NULL) {
                    if (xfrm->arg->tables > 0) {
                        found = TRUE;
                        num_sid_sel = xfrm->arg->tables;
                    }
                }
                break;

            default:
                break;
            }
        }

        /*
         * If found, no need to check other groups since the number
         * of selected tables must be the same across table transforms in
         * other PT groups.
         */
        if (found) {
            break;
        }
    }

    if (num_sid_sel > 0) {
        *max = num_sid_sel - 1;
        *size = num_sid_sel;
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if logical field is to be processed based on flags.
 *
 * This helper routine checks whether the given logical field is to be
 * processed or skipped based on the specified flags as follows:
 *   FIELD_F_KEY         - Key fields
 *   FIELD_F_VALUE       - Value fields
 *   FIELD_F_MAPPED_ONLY - Only mapped fields (if not specified, then get
 *                         all defined fields, mapped and unmapped)
 *
 * \param [in] fid Logical field ID.
 * \param [in] tbl LRD table representation
 * \param [in] map LRD map.
 * \param [in] flags Indicates desired field type to process.
 *
 * \retval TRUE Field is to be processed based on flags.
 * \retval FALSE Field is to be skipped based on flags.
 */
static bool
field_process(uint32_t fid,
              const bcmlrd_table_rep_t *tbl, const bcmlrd_map_t *map,
              uint32_t flags)
{
    bool process = TRUE;

    /*
     * Field: Key or Value
     * Need to check LRD table_rep because the LRD map
     * does not retain the KEY flag if field is unmapped.
     */
    if (tbl->field[fid].flags & BCMLRD_FIELD_F_KEY) {  /* Key field */
        /* Skip key field if not requested */
        if (!(flags & FIELD_F_KEY)) {
            process = FALSE;
        }
    } else {  /* Value field */
        /* Skip value field if not requested */
        if (!(flags & FIELD_F_VALUE)) {
            process = FALSE;
        }
    }

    /* Skip unmapped fields if only mapped fields are requested */
    if ((flags & FIELD_F_MAPPED_ONLY) &&
        bcmlrd_map_field_is_unmapped(map, fid)) {
        process = FALSE;
    }

    return process;
}

/*!
 * \brief Get number of field id and idx count for a given logical table.
 *
 * This core routine returns the count of logical fields IDs and
 * the total number of field elements (fid,idx) for the requested
 * types of field (indicated by 'flags') for the specified logical table.
 *
 * Notes:
 *   num_fid       - This is the number of logical fields
 *                   and does not include the indexes.
 *                   It represents the number of LT field IDs (fid).
 *
 *   num_fid_idx   - This is the number of the unique logical field
 *                   elements for a given table.  A field element
 *                   is identified by the field ID and index (fid,idx).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] flags Specifies requested fields FIELD_F_xxx.
 * \param [out] num_fid Number of field IDs (fid).
 * \param [out] num_fid_idx Number of field elements (fid,idx).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_count_get(int unit, bcmlrd_sid_t sid,
                uint32_t flags,
                uint32_t *num_fid, uint32_t *num_fid_idx)
{
    const bcmlrd_table_rep_t *tbl = NULL;
    const bcmlrd_map_t *map = NULL;
    uint32_t fid_count = 0;
    uint32_t fid_idx_count = 0;
    uint32_t fid;
    uint32_t idx_count;

    SHR_FUNC_ENTER(unit);

    *num_fid = *num_fid_idx = 0;

    /* Get LRD information */
    SHR_IF_ERR_EXIT
        (bcmltm_db_lrd_table_get(sid, &tbl));
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    for (fid = 0; fid < tbl->fields; fid++) {
        /* Check whether to process or skip field */
        if (!field_process(fid, tbl, map, flags)) {
            continue;
        }

        idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);

        fid_idx_count += idx_count;
        fid_count++;
    }

    *num_fid = fid_count;
    *num_fid_idx = fid_idx_count;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get list of field IDs and index count for a given logical table.
 *
 * This core routine returns list of field IDs and associated index count
 * for the requested types of field (indicated by 'flags') for
 * the specified logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] flags Specifies requested fields FIELD_F_xxx.
 * \param [in] list_max Size of allocated list.
 * \param [out] list List of field IDs and index count.
 * \param [out] num Number of fields returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_list_get(int unit, bcmlrd_sid_t sid, uint32_t flags,
               uint32_t list_max,
               bcmltm_fid_idx_t *list, uint32_t *num)
{
    const bcmlrd_table_rep_t *tbl = NULL;
    const bcmlrd_map_t *map = NULL;
    uint32_t fid_count = 0;
    uint32_t fid;
    uint32_t idx_count;

    SHR_FUNC_ENTER(unit);

    /* Get LRD information */
    SHR_IF_ERR_EXIT
        (bcmltm_db_lrd_table_get(sid, &tbl));
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    /*
     * The logical field IDs are always [0..(max_num_fields-1)]
     * by design, where max_num_fields is the total number of
     * fields defined in a table.
     */
    for (fid = 0; fid < tbl->fields; fid++) {
        /* Check whether to process or skip field */
        if (!field_process(fid, tbl, map, flags)) {
            continue;
        }

        if (fid_count >= list_max) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);

        list[fid_count].fid = fid;
        list[fid_count].idx_count = idx_count;

        fid_count++;
    }

    *num = fid_count;

 exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_lrd_table_get(bcmlrd_sid_t sid,
                        const bcmlrd_table_rep_t **tbl)
{
    /* Get LRD information */
    *tbl = bcmlrd_table_get(sid);
    if (*tbl == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM could not get LRD table representation: "
                            "ltid=%d\n"),
                   sid));
        return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}

int
bcmltm_db_map_group_count_get(int unit, bcmlrd_sid_t sid,
                              bcmltm_db_map_group_count_t *count)
{
    bcmlrd_map_t const *map = NULL;
    bcmlrd_table_kind_t group_kind;
    uint32_t group_idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(count, 0, sizeof(*count));

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlrd_map_get(unit, sid, &map), SHR_E_UNAVAIL);
    if (map == NULL) {
        /* Table has no maps */
        SHR_EXIT();
    }

    /* Find out number of Table Commit maps */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group_kind = map->group[group_idx].dest.kind;

        if (group_kind == BCMLRD_MAP_PHYSICAL) {
            count->dm++;
        } else if (group_kind == BCMLRD_MAP_CUSTOM) {
            count->cth++;
        } else if (group_kind == BCMLRD_MAP_LTM) {
            count->ltm++;
        } else if (group_kind == BCMLRD_MAP_VALIDATION) {
            count->fv++;
        } else if (group_kind == BCMLRD_MAP_TABLE_COMMIT) {
            count->tc++;
        }
    }

    /* Sanity checks */

    /* Only 1 Custom Table Handler is supported */
    if (count->cth > 1) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "%s(ltid=%d) "
                             "has %d Custom Table Handlers defined, "
                             "only 1 Custom Table Handler is supported "
                             "per LT.\n"),
                  table_name, sid, count->cth));
        /* Reset to max allowed */
        count->cth = 1;
    }

    /* Only 1 LTM group is supported */
    if (count->ltm > 1) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "%s(ltid=%d) "
                             "has %d LTM map groups defined, "
                             "only 1 LTM group is supported per LT.\n"),
                  table_name, sid, count->ltm));
        /* Reset to max allowed */
        count->ltm = 1;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_table_attr_parse(int unit, bcmlrd_sid_t sid,
                           bcmltm_table_attr_t *attr)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_table_attr_t *tbl_attr = NULL;
    const bcmlrd_field_data_t *field_data = NULL;
    uint32_t attr_idx, lfid, num_attrs;
    uint32_t ro_cnt = 0;
    bool index = FALSE;
    bool mem_param = FALSE;
    uint32_t index_min = 0, index_max = 0, index_size = 0;
    uint32_t mem_param_min = 0, mem_param_max = 0, mem_param_size = 0;
    uint32_t sid_sel_min = 0, sid_sel_max = 0, sid_sel_size = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(attr, 0, sizeof(*attr));

    /* Set default attributes */
    attr->type = BCMLTM_TABLE_TYPE_NONE;
    attr->map_type = BCMLTM_TABLE_MAP_NONE;
    attr->mode = BCMLTM_TABLE_MODE_MODELED;
    attr->flag = 0x0;
    attr->index_min = 0;
    attr->index_max = 0;
    attr->index_size = 0;
    attr->mem_param_min = 0;
    attr->mem_param_max = 0;
    attr->mem_param_size = 0;
    attr->global_ltid = BCMLTD_SID_INVALID;
    attr->opcodes = 0x0;

    /* Get table type */
    SHR_IF_ERR_EXIT
        (bcmltm_db_core_table_type_get(unit, sid, &attr->type));

    /* Get table map type */
    SHR_IF_ERR_EXIT
        (map_type_get(unit, sid, &attr->map_type));

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Set table_min, max for LTM group table */
    if (BCMLTM_TABLE_MAP_IS_LTM(attr->map_type)) {
        uint32_t num_table = bcmltm_db_table_count_get();
        index_min = 0;
        index_max = num_table - 1;
        index = TRUE;
    }

    num_attrs = map->table_attr->attributes;
    for (attr_idx = 0; attr_idx < num_attrs; attr_idx++) {
        tbl_attr = &map->table_attr->attr[attr_idx];
        switch(tbl_attr->key) {
        case BCMLRD_MAP_TABLE_ATTRIBUTE_INTERACTIVE:
            /* Get table operating mode attribute */
            if (tbl_attr->value == TRUE) {
                attr->mode = BCMLTM_TABLE_MODE_INTERACTIVE;
            } else {
                attr->mode = BCMLTM_TABLE_MODE_MODELED;
            }
            break;
        case BCMLRD_MAP_TABLE_ATTRIBUTE_TABLE_MIN_INDEX:
            index_min = tbl_attr->value;
            index = TRUE;
            break;
        case BCMLRD_MAP_TABLE_ATTRIBUTE_TABLE_MAX_INDEX:
            index_max = tbl_attr->value;
            index = TRUE;
            break;
        case BCMLRD_MAP_TABLE_ATTRIBUTE_TABLE_SIZE:
            index_size = tbl_attr->value;
            index = TRUE;
            break;
        case BCMLRD_MAP_TABLE_ATTRIBUTE_PORT_MIN_INDEX:
        case BCMLRD_MAP_TABLE_ATTRIBUTE_INSTANCE_MIN_INDEX:
            mem_param_min = tbl_attr->value;
            mem_param = TRUE;
            break;
        case BCMLRD_MAP_TABLE_ATTRIBUTE_PORT_MAX_INDEX:
        case BCMLRD_MAP_TABLE_ATTRIBUTE_INSTANCE_MAX_INDEX:
            mem_param_max = tbl_attr->value;
            mem_param = TRUE;
            break;
        case BCMLRD_MAP_TABLE_ATTRIBUTE_PORT_SIZE:
        case BCMLRD_MAP_TABLE_ATTRIBUTE_INSTANCE_SIZE:
            mem_param_size = tbl_attr->value;
            mem_param = TRUE;
            break;
        case BCMLRD_MAP_TABLE_ATTRIBUTE_SHARED_INDEX:
            attr->flag |= BCMLTM_TABLE_FLAG_OVERLAY;
            attr->global_ltid = tbl_attr->value;
            break;
        default:
            break;
        }
    }

    /* Get PT SID selector limits */
    SHR_IF_ERR_EXIT
        (pt_sid_sel_limits_get(unit, sid,
                               &sid_sel_min, &sid_sel_max,
                               &sid_sel_size));

    /*
     * If there are no table limits specified, table has no key.
     * In this case, set index size to 1 because the entry
     * still needs to be tracked.
     */
    if (!index && !mem_param && (sid_sel_size == 0)) {
        index = TRUE;
        index_size = 1;
    }
    if (index) {
        attr_limits_get(&index_min, &index_max, &index_size);
    }
    if (mem_param) {
        attr_limits_get(&mem_param_min, &mem_param_max, &mem_param_size);
    }

    attr->index_min = index_min;
    attr->index_max = index_max;
    attr->index_size = index_size;
    attr->mem_param_min = mem_param_min;
    attr->mem_param_max = mem_param_max;
    attr->mem_param_size = mem_param_size;
    attr->sid_sel_min = sid_sel_min;
    attr->sid_sel_max = sid_sel_max;
    attr->sid_sel_size = sid_sel_size;

    for (lfid = 0; lfid < map->field_data->fields; lfid++) {
        field_data = &map->field_data->field[lfid];
        if (field_data->flags & BCMLRD_FIELD_F_READ_ONLY) {
            ro_cnt++;
        }
    }

    if (ro_cnt == map->field_data->fields) {
        attr->flag |= BCMLTM_TABLE_FLAG_READ_ONLY;
    }

    attr->opcodes = map_opcodes_get(map);

 exit:
    SHR_FUNC_EXIT();
}

uint32_t
bcmltm_db_table_count_get(void)
{
    return BCMLRD_TABLE_COUNT;
}

int
bcmltm_db_table_prepopulate_get(int unit,
                                bcmlrd_sid_t sid,
                                bool *prepopulate)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_table_attr_t *tbl_attr = NULL;
    uint32_t num_attrs;
    uint32_t attr_idx;

    SHR_FUNC_ENTER(unit);

    *prepopulate = FALSE;

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    num_attrs = map->table_attr->attributes;
    for (attr_idx = 0; attr_idx < num_attrs; attr_idx++) {
        tbl_attr = &map->table_attr->attr[attr_idx];

        if (tbl_attr->key == BCMLRD_MAP_TABLE_ATTRIBUTE_PREPOPULATE) {
            if (tbl_attr->value == 0) {
                *prepopulate = FALSE;
            } else {
                *prepopulate = TRUE;
            }
            break;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_field_count_get(int unit, bcmlrd_sid_t sid,
                          uint32_t *num_fid, uint32_t *num_fid_idx)
{
    uint32_t flags = FIELD_F_KEY | FIELD_F_VALUE | FIELD_F_MAPPED_ONLY;

    return field_count_get(unit, sid, flags, num_fid, num_fid_idx);
}

int
bcmltm_db_field_key_count_get(int unit, bcmlrd_sid_t sid,
                              uint32_t *num_fid, uint32_t *num_fid_idx)
{
    uint32_t flags = FIELD_F_KEY | FIELD_F_MAPPED_ONLY;

    return field_count_get(unit, sid, flags, num_fid, num_fid_idx);
}

int
bcmltm_db_field_value_count_get(int unit, bcmlrd_sid_t sid,
                                uint32_t *num_fid, uint32_t *num_fid_idx)
{
    uint32_t flags = FIELD_F_VALUE | FIELD_F_MAPPED_ONLY;

    return field_count_get(unit, sid, flags, num_fid, num_fid_idx);
}

int
bcmltm_db_field_list_get(int unit, bcmlrd_sid_t sid,
                         uint32_t list_max,
                         bcmltm_fid_idx_t *list, uint32_t *num)
{
    uint32_t flags = FIELD_F_KEY | FIELD_F_VALUE | FIELD_F_MAPPED_ONLY;

    return field_list_get(unit, sid, flags, list_max, list, num);
}

int
bcmltm_db_field_key_list_get(int unit, bcmlrd_sid_t sid,
                             uint32_t list_max,
                             bcmltm_fid_idx_t *list, uint32_t *num)
{
    uint32_t flags = FIELD_F_KEY | FIELD_F_MAPPED_ONLY;

    return field_list_get(unit, sid, flags, list_max, list, num);
}

int
bcmltm_db_field_value_list_get(int unit, bcmlrd_sid_t sid,
                               uint32_t list_max,
                               bcmltm_fid_idx_t *list, uint32_t *num)
{
    uint32_t flags = FIELD_F_VALUE | FIELD_F_MAPPED_ONLY;

    return field_list_get(unit, sid, flags, list_max, list, num);
}

int
bcmltm_db_gfid_to_fid(int unit,
                      bcmltd_sid_t sid,
                      bcmltd_gfid_t gfid,
                      bcmltd_fid_t *fid)
{
    return bcmlrd_fid_get(unit, sid, gfid, fid);
}

int
bcmltm_db_fid_to_gfid(int unit,
                      bcmltd_sid_t sid,
                      bcmltd_fid_t fid,
                      bcmltd_gfid_t *gfid)
{
    return bcmlrd_gfid_get(unit, sid, fid, gfid);
}

int
bcmltm_db_fid_max_get(int unit, bcmlrd_sid_t sid,
                      bcmlrd_fid_t *fid_max)
{
    const bcmlrd_table_rep_t *tbl;

    SHR_FUNC_ENTER(unit);

    *fid_max = 0;

    /* Get LRD information */
    SHR_IF_ERR_EXIT
        (bcmltm_db_lrd_table_get(sid, &tbl));

    /* Return unavail if table has no logical fields */
    if (tbl->fields == 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *fid_max = tbl->fields - 1;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_field_to_fidx_min_max(int unit, uint32_t fid,
                                uint16_t minbit, uint16_t maxbit,
                                uint32_t list_max,
                                uint16_t *fidx_minbit,
                                uint16_t *fidx_maxbit,
                                uint32_t *num_fidx)
{
    uint32_t width;
    uint32_t idx;
    uint32_t idx_count;
    uint32_t last_idx;

    width = maxbit - minbit + 1;
    idx_count = bcmltd_field_idx_count_get(width);

    if (list_max < idx_count) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM field to field index array: "
                              "array count too small: "
                              "fid=%d max=%d actual=%d\n"),
                   fid, list_max, idx_count));
        return SHR_E_INTERNAL;
    }

    last_idx = idx_count - 1;
    for (idx = 0; idx < idx_count; idx++) {
        /* Minbit on field index element */
        fidx_minbit[idx] = minbit + (BCMLTM_FIELD_SIZE_BITS * idx);

        /* Maxbit on last field index element takes field maxbit */
        if (idx == last_idx) {
            fidx_maxbit[idx] = maxbit;
        } else {
            fidx_maxbit[idx] = fidx_minbit[idx] + (BCMLTM_FIELD_SIZE_BITS - 1);
        }
    }

    *num_fidx = idx_count;

    return SHR_E_NONE;
}

int
bcmltm_db_field_scalar_values_get(int unit,
                                  bcmlrd_sid_t sid,
                                  bcmlrd_fid_t fid,
                                  uint64_t *min, uint64_t *max,
                                  uint64_t *default_value)
{
    uint32_t actual_count;

    SHR_FUNC_ENTER(unit);

    if (min != NULL) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_min_get(unit, sid, fid,
                                  1, min, &actual_count));
        if (actual_count != 1) {
            const char *table_name;
            const char *field_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM: Invalid index count for field minimum "
                                  "%s(ltid=%d) %s(fid=%d) "
                                  "expected=1 actual=%d\n"),
                       table_name, sid, field_name, fid, actual_count));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    if (max != NULL) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_max_get(unit, sid, fid,
                                  1, max, &actual_count));
        if (actual_count != 1) {
            const char *table_name;
            const char *field_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM: Invalid index count for field maximum "
                                  "%s(ltid=%d) %s(fid=%d) "
                                  "expected=1 actual=%d\n"),
                       table_name, sid, field_name, fid, actual_count));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    if (default_value != NULL) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_default_get(unit, sid, fid,
                                      1, default_value, &actual_count));
        if (actual_count != 1) {
            const char *table_name;
            const char *field_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM: Invalid index count for field default "
                                  "%s(ltid=%d) %s(fid=%d) "
                                  "expected=1 actual=%d\n"),
                       table_name, sid, field_name, fid, actual_count));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_field_is_key(int unit,
                       bcmlrd_sid_t sid,
                       bcmlrd_fid_t fid,
                       bool *is_key)
{
    const bcmlrd_table_rep_t *tbl;

    SHR_FUNC_ENTER(unit);

    *is_key = FALSE;

    /* Get LRD information */
    SHR_IF_ERR_EXIT
        (bcmltm_db_lrd_table_get(sid, &tbl));

    if (fid >= tbl->fields) {
        const char *table_name;
        const char *field_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM: Invalid logical field ID "
                              "%s(ltid=%d) %s(fid=%d)\n"),
                   table_name, sid, field_name, fid));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *is_key = !!(tbl->field[fid].flags & BCMLRD_FIELD_F_KEY);

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_field_is_read_only(int unit, bcmlrd_sid_t sid,
                             bcmlrd_fid_t fid,
                             bool *is_read_only)
{
    const bcmlrd_table_rep_t *tbl = NULL;
    const bcmlrd_map_t *map = NULL;
    const bcmltd_field_rep_t *tbl_field;
    const bcmlrd_field_data_t *map_field;
    bool is_key = FALSE;

    SHR_FUNC_ENTER(unit);

    *is_read_only = FALSE;

    
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_is_key(unit, sid, fid, &is_key));
    if (is_key) {
         SHR_EXIT();
    }

    /* Get LRD information */
    SHR_IF_ERR_EXIT
        (bcmltm_db_lrd_table_get(sid, &tbl));
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    if (fid >= tbl->fields) {
        const char *table_name;
        const char *field_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM: Invalid logical field ID "
                              "%s(ltid=%d) %s(fid=%d)\n"),
                   table_name, sid, field_name, fid));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * Field access flags may be from the logical table definition
     * or the field mapping.
     */
    tbl_field = &tbl->field[fid];
    map_field = &map->field_data->field[fid];

    if ((tbl_field->flags & BCMLRD_FIELD_F_READ_ONLY) ||
        (map_field->flags & BCMLRD_FIELD_F_READ_ONLY)) {
        *is_read_only = TRUE;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_field_is_enum(int unit, bcmlrd_sid_t sid,
                        bcmlrd_fid_t fid,
                        bool *is_enum)
{
    const bcmlrd_table_rep_t *tbl;

    SHR_FUNC_ENTER(unit);

    *is_enum = FALSE;

    /* Get LRD information */
    tbl = bcmlrd_table_get(sid);
    if (tbl == NULL) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not get LRD table representation: "
                              "%s(ltid=%d)\n"),
                   table_name, sid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (fid >= tbl->fields) {
        const char *table_name;
        const char *field_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM: Invalid logical field ID "
                              "%s(ltid=%d) %s(fid=%d)\n"),
                   table_name, sid, field_name, fid));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *is_enum = !!(tbl->field[fid].flags & BCMLRD_FIELD_F_ENUM);

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_field_is_virtual(int unit, uint32_t fid,
                           bool *is_virtual)
{
    SHR_FUNC_ENTER(unit);

    *is_virtual = FALSE;

    if ((fid == BCMLRD_FIELD_INDEX) ||
        (fid == BCMLRD_FIELD_PORT) ||
        (fid == BCMLRD_FIELD_ENTRY_PRIORITY) ||
        (fid == BCMLRD_FIELD_INSTANCE) ||
        (fid == BCMLRD_FIELD_TABLE_SEL) ||
        (fid == BCMLRD_FIELD_TRACK_INDEX) ||
        (fid == BCMLRD_FIELD_PT_SUPPRESS) ||
        (fid == BCMLRD_FIELD_SELECTOR) ||
        (fid == BCMLRD_FIELD_LDTYPE)) {
        *is_virtual = TRUE;
    }

    SHR_FUNC_EXIT();
}

int
bcmltm_db_field_is_selector(int unit, bcmlrd_sid_t sid,
                            bcmlrd_fid_t fid,
                            bool *is_selector)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_field_selector_data_t *select_data = NULL;
    const bcmlrd_field_selector_t *field_select = NULL;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    *is_selector = FALSE;

    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    select_data = map->sel;
    if (select_data == NULL) {
        SHR_EXIT();
    }

    for (i = 0; i < select_data->num_field_selector; i++) {
        field_select = &select_data->field_selector[i];

        if (field_select->selector_id == fid) {
            *is_selector = TRUE;
            break;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_field_select_group_count_get(int unit, bcmlrd_sid_t sid,
                                       bcmlrd_fid_t fid,
                                       uint32_t *num_group)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_field_selector_data_t *select_data = NULL;
    const bcmlrd_field_selector_t *field_select = NULL;
    uint32_t i;
    uint32_t group_count = 0;
    uint32_t group_max = 0;
    bool is_virtual = FALSE;

    SHR_FUNC_ENTER(unit);

    *num_group = 0;

    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    select_data = map->sel;
    if (select_data == NULL) {
        SHR_EXIT();
    }

    /*
     * Get the number of selection groups for a given field.
     *
     * The LRD selection groups are assigned in a compact numbering
     * space [0..group_max].
     *
     * For the group count, use the largest selection group number
     * 'group_max' plus 1, rather than counting the valid groups.
     * This is to avoid double-counting for the case explained below.
     *
     * Note:
     * More than one selector entry may have the same
     * selection group number.
     *
     * This happens when a selector value contains several
     * maps for the same logical field.
     * Example:
     *      select S1 {
     *          0: A: PT1.F1; --> Field A group is assigned 0
     *          1: A: PT1.F2, --> Field A group is assigned 1
     *             A: PT2.F3; --> Field A group is also assigned 1
     *          2: B: PT1.F4;
     *      }
     * The returning group count for field A should be 2, for
     * group numbers 0 and 1, but not 3.
     */
    for (i = 0; i < select_data->num_field_selector; i++) {
        field_select = &select_data->field_selector[i];

        /* Skip ROOT selector entry */
        if (field_select->selector_type == BCMLRD_FIELD_SELECTOR_TYPE_ROOT) {
            continue;
        }

        /*
         * Skip invalid selector group.
         *
         * Invalid group numbers are assigned to those field selections
         * where the field is always valid (field is specified in all
         * selector values).  From the logical perspective, the field
         * is always present, and therefore, not subject to any selection.
         *
         * This is the case where the select construct is used
         * to select where the logical field maps to, but it does not
         * make a field present or not (virtual field selector is one case).
         */
        if (field_select->group == BCMLRD_INVALID_SELECTOR_INDEX) {
            continue;
        }

        /*
         * Skip virtual field selector.
         *
         * A virtual field selector does not select which fields
         * are valid or not.
         *
         * NOTE: Currently, fields that part of a virtual field selector
         * are assigned a valid group.
         * This check can be removed once the LRD data
         * sets the INVALID group number for fields using
         * virtual field selectors (skip will be triggered by
         * the invalid selector group check above).
         */
        SHR_IF_ERR_EXIT
            (bcmltm_db_field_is_virtual(unit, field_select->selector_id,
                                        &is_virtual));
        if (is_virtual) {
            continue;
        }

        if (field_select->field_id == fid) {
            group_count++;

            /* Record largest selection group number */
            if (field_select->group > group_max) {
                group_max = field_select->group;
            }
        }
    }

    if (group_count > 0) {
        *num_group = group_max + 1;
    }

 exit:
    SHR_FUNC_EXIT();
}

bool
bcmltm_db_field_key_pt_index_mapped(int unit, bcmlrd_sid_t sid,
                                    uint32_t fid)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;

    /* Get LRD mapping */
    if (SHR_FAILURE(bcmlrd_map_get(unit, sid, &map))) {
        return FALSE;
    }

    /* For each map group */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];
        /* Skip non-PT groups */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        /* For each map entry */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];

            /* Check direct mapped destination field */
            if (entry->entry_type == BCMLRD_MAP_ENTRY_MAPPED_KEY) {
                if ((entry->u.mapped.src.field_id == fid) &&
                    (entry->desc.field_id == BCMLRD_FIELD_INDEX)) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

bool
bcmltm_db_field_key_is_alloc(int unit, bcmlrd_sid_t sid, uint32_t fid)
{
    int rv;
    bcmltm_table_type_t table_type;

    rv = bcmltm_db_core_table_type_get(unit, sid, &table_type);
    if (SHR_FAILURE(rv)) {
        return FALSE;
    }

    /* Allocatable (optional) keys are only applicable on IwA LT */
    if (BCMLTM_TABLE_TYPE_IS_ALLOC_INDEX(table_type)) {
        /* Field is allocatable if it maps to a PT index */
        if (bcmltm_db_field_key_pt_index_mapped(unit, sid, fid)) {
            return TRUE;
        }
    }

    return FALSE;
}

bool
bcmltm_db_xfrm_key_is_alloc(const bcmltm_table_attr_t *attr,
                            int xfrm_type,
                            const bcmltd_transform_arg_t *xfrm_arg)
{
    const uint32_t *fields;
    uint32_t num_fields;
    uint32_t fidx;

    /* Allocatable (i.e. optional) keys are only applicable on IwA LT */
    if (!BCMLTM_TABLE_TYPE_IS_ALLOC_INDEX(attr->type)) {
        return FALSE;
    }

    /* Valid only on key transforms */
    if ((xfrm_type != BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM) &&
        (xfrm_type != BCMLTM_DB_XFRM_TYPE_REV_KEY_TRANSFORM)) {
        return FALSE;
    }

    /* Destination field list (PT) based on forward or reverse transform */
    if (xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM) {
        fields = xfrm_arg->rfield;
        num_fields = xfrm_arg->rfields;
    } else {
        fields = xfrm_arg->field;
        num_fields = xfrm_arg->fields;
    }

    /* Search field list */
    for (fidx = 0; fidx < num_fields; fidx++) {
        if (fields[fidx] == BCMLRD_FIELD_INDEX) {
            /* Transform contains allocatable (optional) key */
            return TRUE;
        }
    }

    return FALSE;
}

