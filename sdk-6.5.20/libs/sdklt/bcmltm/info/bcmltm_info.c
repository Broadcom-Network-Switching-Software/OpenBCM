/*! \file bcmltm_info.c
 *
 * Logical Table Manager - Low level table information functions.
 *
 * This file contains routines that provide low level
 * properties information for logical tables derived
 * from the LRD table definition and maps.
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

#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_info_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*******************************************************************************
 * Public functions
 */

int
bcmltm_info_table_type_get(int unit,
                           bcmlrd_sid_t sid,
                           bcmltm_table_type_t *type)
{
    const bcmlrd_table_rep_t *tbl = NULL;
    uint32_t lrd_type;
    uint32_t tbl_flags;

    SHR_FUNC_ENTER(unit);

    *type = BCMLTM_TABLE_TYPE_NONE;

    tbl = bcmlrd_table_get(sid);
    if (tbl == NULL) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not get LRD table representation: "
                              "%s(ltid=%d)\n"),
                   table_name, sid));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (tbl->flags & BCMLRD_TABLE_F_TYPE_MAPPED) {
        SHR_IF_ERR_EXIT
            (bcmlrd_map_table_attr_get(unit, sid,
                                       BCMLRD_MAP_TABLE_ATTRIBUTE_TABLE_TYPE,
                                       &tbl_flags));
    } else {
        tbl_flags = tbl->flags;
    }

    lrd_type = tbl_flags & BCMLTD_TABLE_F_TYPE_MASK;

    if (lrd_type == BCMLTD_TABLE_F_TYPE_INDEX) {
        /* Configuration tables are a type of index table */
        if (tbl_flags & BCMLTD_TABLE_F_CONFIG) {
            *type = BCMLTM_TABLE_TYPE_CONFIG;
        } else if (tbl_flags & BCMLTD_TABLE_F_TYPE_INDEX_ALLOC) {
            *type = BCMLTM_TABLE_TYPE_ALLOC_INDEX;
        } else {
            *type = BCMLTM_TABLE_TYPE_SIMPLE_INDEX;
        }
    } else if (lrd_type == BCMLTD_TABLE_F_TYPE_TCAM) {
        *type = BCMLTM_TABLE_TYPE_TCAM;
    } else if (lrd_type == BCMLTD_TABLE_F_TYPE_HASH) {
        *type = BCMLTM_TABLE_TYPE_HASH;
    } else {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Undefined LRD table type: "
                              "%s(ltid=%d) lrd_table_flags=0x%x\n"),
                   table_name, sid, tbl->flags));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_info_table_map_type_get(int unit,
                               bcmlrd_sid_t sid,
                               bcmltm_table_map_type_t *type)
{
    const bcmlrd_map_t *map = NULL;
    bcmlrd_table_kind_t group_kind;
    uint32_t group_idx;

    SHR_FUNC_ENTER(unit);

    *type = BCMLTM_TABLE_MAP_NONE;

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    /* Find out table map type based on group kind */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group_kind = map->group[group_idx].dest.kind;

        /*
         * Assume LRD map data is correct and only one map type
         * specification is allowed in a logical table.
         */
        if (group_kind == BCMLRD_MAP_PHYSICAL) {
            *type = BCMLTM_TABLE_MAP_DIRECT;
            break;
        } else if (group_kind == BCMLRD_MAP_CUSTOM) {
            *type = BCMLTM_TABLE_MAP_CTH;
            break;
        } else if (group_kind == BCMLRD_MAP_LTM) {
            *type = BCMLTM_TABLE_MAP_LTM;
            break;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_info_table_mode_get(int unit,
                           bcmlrd_sid_t sid,
                           bcmltm_table_mode_t *mode)
{
    int rv;
    uint32_t value;

    SHR_FUNC_ENTER(unit);

    /* Set default: modeled */
    *mode = BCMLTM_TABLE_MODE_MODELED;

    /* Get attribute for table operating mode */
    rv = bcmlrd_map_table_attr_get(unit, sid,
                                   BCMLRD_MAP_TABLE_ATTRIBUTE_INTERACTIVE,
                                   &value);
    if (SHR_SUCCESS(rv)) {
        if (value == TRUE) {
            *mode = BCMLTM_TABLE_MODE_INTERACTIVE;
        } else {
            *mode = BCMLTM_TABLE_MODE_MODELED;
        }
    } else {
        /* Return error other than those caused by attribute not present */
        if ((rv != SHR_E_UNAVAIL) && (rv != SHR_E_NOT_FOUND)) {
            SHR_ERR_EXIT(rv);
        }
        /* Return default mode if attribute is absent */
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_info_table_cth_handler_id_get(int unit,
                                     bcmlrd_sid_t sid,
                                     uint32_t *handler_id)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    bool handler_found = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    /* For each map group */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Skip non Custom Table Handler group kind */
        if (group->dest.kind != BCMLRD_MAP_CUSTOM) {
            continue;
        }

        /* Process map entries until table handler entry is found */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];

            if (entry->entry_type == BCMLRD_MAP_ENTRY_TABLE_HANDLER) {
                *handler_id = entry->u.handler_id;
                handler_found = TRUE;
                break;
            }
        }

        break;  /* Support 1 custom table handler entry per LT */
    }

    if (!handler_found) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_info_table_entry_limit_handler_id_get(int unit,
                                             bcmlrd_sid_t sid,
                                             uint32_t *handler_id)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    bool handler_found = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    /* For each map group */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Skip non table entry limit group kind */
        if (group->dest.kind != BCMLRD_MAP_TABLE_ENTRY_LIMIT) {
            continue;
        }

        /* Process map entries until handler entry is found */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];

            if (entry->entry_type ==
                BCMLRD_MAP_ENTRY_TABLE_ENTRY_LIMIT_HANDLER) {
                *handler_id = entry->u.handler_id;
                handler_found = TRUE;
                break;
            }
        }

        break;
    }

    if (!handler_found) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

 exit:
    SHR_FUNC_EXIT();
}
