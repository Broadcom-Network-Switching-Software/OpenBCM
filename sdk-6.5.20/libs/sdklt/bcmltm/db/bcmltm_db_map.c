/*! \file bcmltm_db_map.c
 *
 * Logical Table Manager - Utility interfaces for LRD maps.
 *
 * This file contains routines to get information from the LRD map database.
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

#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>

#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>

#include "bcmltm_db_map.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Sanity check for transform table.
 *
 * This routine performs sanity checks for transform table information
 * in the given transform handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] entry LRD transform map entry to check.
 * \param [in] handler LRD Transform handler.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_table_check(int unit,
                 bcmlrd_sid_t sid,
                 const bcmlrd_map_entry_t *entry,
                 const bcmltd_xfrm_handler_t *handler)
{
    bcmlrd_map_entry_type_t entry_type = entry->entry_type;
    uint32_t table_count;
    const uint32_t *table_array;
    bool field_table_sel;

    SHR_FUNC_ENTER(unit);

    /*
     * Sanity check: Transform table
     * The following checks applies only to key transforms (table selection
     * is ignored if present in value transforms):
     * - If (table count > 0) then table array should not be NULL.
     * - If (table count > 0) then destination fields must contain __TABLE_SEL.
     * - If destination field contains __TABLE_SEL, then (table_count > 0).
     */

    /* Skip check if transform is not a key transform */
    if (!BCMLTM_DB_MAP_ENTRY_TYPE_IS_KEY_XFRM(entry_type)) {
        SHR_EXIT();
    }

    table_count = handler->arg->tables;
    table_array = handler->arg->table;
    field_table_sel = bcmltm_db_xfrm_field_find(unit, entry, FALSE,
                                                BCMLRD_FIELD_TABLE_SEL);

    if ((table_count > 0) && (table_array == NULL)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s(ltid=%d): "
                              "Transform table array is NULL "
                              "(table_count=%d)\n"),
                   bcmltm_lt_table_sid_to_name(unit, sid), sid,
                   table_count));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if ((table_count > 0) && (!field_table_sel)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s(ltid=%d): "
                              "Transform table missing dst field __TABLE_SEL"
                              "(table_count=%d)\n"),
                   bcmltm_lt_table_sid_to_name(unit, sid), sid,
                   table_count));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (field_table_sel && (table_count == 0)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s(ltid=%d): "
                              "Transform dst field __TABLE_SEL requires "
                              "table array\n"),
                   bcmltm_lt_table_sid_to_name(unit, sid), sid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_map_entry_list_init(int unit, bcmlrd_sid_t sid,
                              uint32_t max_entries,
                              bcmltm_db_map_entry_list_t *list)
{
    bcmltm_db_map_entry_t *entries = NULL;
    uint32_t size;

    SHR_FUNC_ENTER(unit);

    sal_memset(list, 0, sizeof(*list));

    if (max_entries > 0) {
        /* Allocate lrd map array */
        size = sizeof(*entries) * max_entries;
        SHR_ALLOC(entries, size, "bcmltmDbMapEntries");
        SHR_NULL_CHECK(entries, SHR_E_MEMORY);
        sal_memset(entries, 0, size);
    }

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &list->map));
    list->max_entries = max_entries;
    list->num_entries = 0;
    list->entries = entries;

 exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(entries);
        list->entries = NULL;
        bcmltm_db_map_entry_list_cleanup(list);
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_db_map_entry_list_cleanup(bcmltm_db_map_entry_list_t *list)
{
    if (list == NULL) {
        return;
    }

    SHR_FREE(list->entries);
    list->entries = NULL;

    return;
}

int
bcmltm_db_map_entry_list_create(int unit,
                                bcmlrd_sid_t sid,
                                uint32_t max_entries,
                                bcmltm_db_map_entry_list_t **list_ptr)
{
    bcmltm_db_map_entry_list_t *list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(list, sizeof(*list), "bcmltmDbMapEntryList");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));

    SHR_IF_ERR_EXIT
        (bcmltm_db_map_entry_list_init(unit,
                                       sid,
                                       max_entries,
                                       list));
    *list_ptr = list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_map_entry_list_destroy(list);
        *list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_db_map_entry_list_destroy(bcmltm_db_map_entry_list_t *list)
{
    bcmltm_db_map_entry_list_cleanup(list);
    SHR_FREE(list);

    return;
}

int
bcmltm_db_map_entry_add(bcmltm_db_map_entry_list_t *list,
                        uint32_t group_idx,
                        uint32_t entry_idx)
{
    uint32_t map_idx;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* If already in list, do not add */
    if (bcmltm_db_map_entry_find(list, group_idx, entry_idx) != NULL) {
        SHR_EXIT();
    }

    /* Assume group and entry indexes are valid */
    map_idx = list->num_entries;
    if (map_idx >= list->max_entries) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Map entries exceeds max list size max=%d\n"),
                   list->max_entries));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    list->entries[map_idx].group_idx = group_idx;
    list->entries[map_idx].entry_idx = entry_idx;
    list->num_entries++;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_map_entry_list_add(bcmltm_db_map_entry_list_t *list,
                             const bcmltm_db_map_entry_list_t *from_list)
{
    bcmltm_db_map_entry_t *from_entry;
    uint32_t from_idx;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (from_list == NULL) {
        SHR_EXIT();
    }

    for (from_idx = 0; from_idx < from_list->num_entries; from_idx++) {
        from_entry = &from_list->entries[from_idx];

        SHR_IF_ERR_EXIT
            (bcmltm_db_map_entry_add(list,
                                     from_entry->group_idx,
                                     from_entry->entry_idx));
    }

 exit:
    SHR_FUNC_EXIT();
}

const bcmltm_db_map_entry_t *
bcmltm_db_map_entry_find(const bcmltm_db_map_entry_list_t *list,
                         uint32_t group_idx,
                         uint32_t entry_idx)
{
    const bcmltm_db_map_entry_t *map_entry;
    uint32_t idx;

    if ((list == NULL) || (list->num_entries == 0)) {
        return NULL;
    }

    for (idx = 0; idx < list->num_entries; idx++) {
        map_entry = &list->entries[idx];

        if ((map_entry->group_idx == group_idx) &&
            (map_entry->entry_idx == entry_idx)) {
            return map_entry;
        }
    }

    return NULL;
}

int
bcmltm_db_map_entries_count_get(int unit,
                                bcmlrd_sid_t sid,
                                uint32_t *count)
{
    bcmlrd_map_t const *map = NULL;
    uint32_t max_entries = 0;
    uint32_t group_idx;

    SHR_FUNC_ENTER(unit);

    *count = 0;

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlrd_map_get(unit, sid, &map), SHR_E_UNAVAIL);
    if (map == NULL) {
        /* Table has no maps */
        SHR_EXIT();
    }

    /* Get total count of map entries in LRD map */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        max_entries += map->group[group_idx].entries;
    }

    *count = max_entries;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_field_def_min_max_get(int unit,
                                bcmlrd_sid_t sid,
                                uint32_t fid,
                                uint32_t idx_count,
                                uint64_t *default_values,
                                uint64_t *minimum_values,
                                uint64_t *maximum_values)
{
    uint32_t default_count;
    uint32_t minimum_count;
    uint32_t maximum_count;

    SHR_FUNC_ENTER(unit);

    /* Get SW default values for field */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_default_get(unit, sid, fid,
                                  idx_count, default_values, &default_count));
    if (idx_count != default_count) {
        const char *table_name;
        const char *field_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid index count for field defaults: "
                              "%s(ltid=%d) %s(fid=%d) "
                              "expected=%d actual=%d\n"),
                   table_name, sid, field_name, fid,
                   idx_count, default_count));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get SW minimum values for field */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_min_get(unit, sid, fid,
                              idx_count, minimum_values, &minimum_count));
    if (idx_count != minimum_count) {
        const char *table_name;
        const char *field_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid index count for field minimums: "
                              "%s(ltid=%d) %s(fid=%d) "
                              "expected=%d actual=%d\n"),
                   table_name, sid, field_name, fid,
                   idx_count, minimum_count));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get SW maximum values for field */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_max_get(unit, sid, fid,
                              idx_count, maximum_values, &maximum_count));
    if (idx_count != maximum_count) {
        const char *table_name;
        const char *field_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid index count for field maximums: "
                              "%s(ltid=%d) %s(fid=%d) "
                              "expected=%d actual=%d\n"),
                   table_name, sid, field_name, fid,
                   idx_count, maximum_count));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

bool
bcmltm_db_map_dst_field_find(int unit,
                             bcmlrd_sid_t sid,
                             bcmdrd_sid_t ptid,
                             uint32_t fid)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    bcmlrd_map_entry_type_t entry_type;
    uint32_t group_idx;
    uint32_t entry_idx;

    /* Get LRD mapping */
    if (SHR_FAILURE(bcmlrd_map_get(unit, sid, &map))) {
        return FALSE;
    }

    /* Check group map */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Skip non-PT groups */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        /* Skip non-matching PT ID (if specified) */
        if ((ptid != BCMDRD_INVALID_ID) && (group->dest.id != ptid)) {
            continue;
        }

        /* Check map entry */
        for (entry_idx = 0; (entry_idx < group->entries); entry_idx++) {
            entry = &group->entry[entry_idx];
            entry_type = entry->entry_type;

            if (BCMLTM_DB_MAP_ENTRY_TYPE_IS_DM(entry_type) ||
                BCMLTM_DB_MAP_ENTRY_TYPE_IS_FIXED(entry_type)) {
                /* Direct field map or fixed field map */
                if (entry->desc.field_id == fid) {
                    return TRUE;
                }
            } else if (BCMLTM_DB_MAP_ENTRY_TYPE_IS_XFRM(entry_type)) {
                /* Transform field map */
                if (bcmltm_db_xfrm_field_find(unit, entry, 0, fid)) {
                    return TRUE;
                }
            }

            /* Ignore other map entry types */
        }
    }

    return FALSE;
}

bool
bcmltm_db_xfrm_field_find(int unit,
                          const bcmlrd_map_entry_t *entry,
                          bool src,
                          uint32_t fid)
{
    const bcmltd_xfrm_handler_t *xfrm = NULL;
    bcmlrd_map_entry_type_t entry_type;
    const uint32_t *fields;
    uint32_t num_fields;
    uint32_t fidx;

    entry_type = entry->entry_type;
    if (!BCMLTM_DB_MAP_ENTRY_TYPE_IS_XFRM(entry_type)) {
        return FALSE;
    }

    /* Transform Handler */
    xfrm = bcmltm_lta_intf_xfrm_handler_get(unit,
                                            entry->u.xfrm.desc->handler_id);

    /* Sanity check */
    if (xfrm == NULL) {
        return FALSE;
    }

    /* Forward transform */
    if (BCMLTM_DB_MAP_ENTRY_TYPE_IS_FWD_XFRM(entry_type)) {
        if (src) {
            fields = xfrm->arg->field;
            num_fields = xfrm->arg->fields;
        } else {
            fields = xfrm->arg->rfield;
            num_fields = xfrm->arg->rfields;
        }
    } else { /* Reverse transform */
        if (src) {
            fields = xfrm->arg->rfield;
            num_fields = xfrm->arg->rfields;
        } else {
            fields = xfrm->arg->field;
            num_fields = xfrm->arg->fields;
        }
    }

    /* Look for field ID */
    for (fidx = 0; fidx < num_fields; fidx++) {
        if (fields[fidx] == fid) {
            return TRUE;
        }
    }

    return FALSE;
}

bool
bcmltm_db_xfrm_all_field_find(int unit,
                              bcmlrd_sid_t sid,
                              bool src,
                              uint32_t fid)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    bcmlrd_map_entry_type_t entry_type;
    uint32_t group_idx;
    uint32_t entry_idx;
    bool found;

    /* Get LRD mapping */
    if (SHR_FAILURE(bcmlrd_map_get(unit, sid, &map))) {
        return FALSE;
    }

    /* Process each map group */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Skip non-PT groups */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        /* Process each map entry */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];
            entry_type = entry->entry_type;

            if (!BCMLTM_DB_MAP_ENTRY_TYPE_IS_XFRM(entry_type)) {
                continue;
            }

            found = bcmltm_db_xfrm_field_find(unit, entry, src, fid);
            if (found) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

int
bcmltm_db_xfrm_check(int unit,
                     bcmlrd_sid_t sid,
                     const bcmlrd_map_entry_t *entry,
                     const bcmltd_xfrm_handler_t **lta_handler)
{
    const bcmltd_xfrm_handler_t *handler = NULL;
    bcmlrd_map_entry_type_t entry_type;

    SHR_FUNC_ENTER(unit);

    *lta_handler = NULL;

    /* Sanity check: Transform map entry type */
    entry_type = entry->entry_type;
    if (!BCMLTM_DB_MAP_ENTRY_TYPE_IS_XFRM(entry_type)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s(ltid=%d): "
                              "Map entry type is not transform type "
                              "(entry_type=%d)\n"),
                   bcmltm_lt_table_sid_to_name(unit, sid), sid,
                   entry_type));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Sanity check: Transform handler cannot be NULL */
    handler = bcmltm_lta_intf_xfrm_handler_get(unit,
                                               entry->u.xfrm.desc->handler_id);
    if (handler == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s(ltid=%d): "
                              "Transform handler is NULL\n"),
                   bcmltm_lt_table_sid_to_name(unit, sid), sid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Sanity check: Transform handler argument cannot be NULL */
    if (handler->arg == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s(ltid=%d): "
                              "Transform handler arg is NULL\n"),
                   bcmltm_lt_table_sid_to_name(unit, sid), sid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Sanity check: transform table */
    SHR_IF_ERR_EXIT
        (xfrm_table_check(unit, sid, entry, handler));

    *lta_handler = handler;

 exit:
    SHR_FUNC_EXIT();
}
