/*! \file bcmltm_db_tc.c
 *
 * Logical Table Manager - Information for Table Commit Interfaces.
 *
 * This file contains routines to construct information for
 * table commit interfaces in logical tables.
 *
 * This information is derived from the LRD Table Commit map groups.
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

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>

#include "bcmltm_db_tc.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*!
 * \brief LT map entries information for Table Commit groups.
 *
 * This structure is used to hold information parsed from the LRD
 * map entries that are relevant for the Table Commit Handler groups.
 */
typedef struct tc_map_entries_s {
    /*! Number of Table Commit handlers. */
    uint32_t num_handlers;

    /*! Array of Table Commit handler ids. */
    uint32_t *handler_id;
} tc_map_entries_t;


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Count number of Table Commit entries.
 *
 * Count number of Table Commit entries.
 *
 * \param [in] unit Unit number.
 * \param [in] map LRD map.
 * \param [out] count Number of Table Commit entries.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
tc_map_entries_count_get(int unit,
                         const bcmlrd_map_t *map,
                         uint32_t *count)
{
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    uint32_t num = 0;

    *count = 0;

    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Check for Table Commit kind */
        if (group->dest.kind != BCMLRD_MAP_TABLE_COMMIT) {
            continue;
        }

        /* Process each map entry */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];
            if (entry->entry_type == BCMLRD_MAP_ENTRY_TABLE_COMMIT_HANDLER) {
                num++;
            }
        }
    }

    *count = num;

    return SHR_E_NONE;
}

/*!
 * \brief Cleanup LT Table Commit map entries information.
 *
 * Free any internal memory allocated during the parsing of the
 * Table Commit map entries.
 *
 * \param [in] lt_map LT mapping information to clean.
 */
static void
tc_map_entries_cleanup(tc_map_entries_t *lt_map)
{
    SHR_FREE(lt_map->handler_id);
    lt_map->handler_id = NULL;

    return;
}

/*!
 * \brief Get the Table Commit mapping information for a given LT.
 *
 * Get the LT Table Commit mapping information for a given table ID.
 *
 * This routine parses the LRD entry map information for:
 *     Map Group Kind: BCMLRD_MAP_TABLE_COMMIT
 *     Map Entry Type: BCMLRD_MAP_ENTRY_TABLE_COMMIT_HANDLER
 *
 * This routine allocates memory so caller must call corresponding routine
 * to free memory when this is no longer required.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] lt_map Returning LT mapping information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
tc_map_entries_parse(int unit, bcmlrd_sid_t sid,
                     tc_map_entries_t *lt_map)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    uint32_t total_num_tc = 0;
    uint32_t *handler_id = NULL;
    uint32_t handler_idx = 0;
    unsigned int size;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_map, 0, sizeof(*lt_map));

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Count number of Table Commit entries */
    SHR_IF_ERR_EXIT
        (tc_map_entries_count_get(unit, map, &total_num_tc));
    if (total_num_tc == 0) {
        SHR_EXIT();
    }

    /* Allocate handler ids array */
    size = sizeof(*handler_id) * total_num_tc;
    SHR_ALLOC(handler_id, size, "bcmltmTableCommitHandlerIdArr");
    SHR_NULL_CHECK(handler_id, SHR_E_MEMORY);
    sal_memset(handler_id, 0, size);

    /* For each map group */
    for (group_idx = 0; group_idx < map->groups; group_idx++) {
        group = &map->group[group_idx];

        /* Process Table Commit kind */
        if (group->dest.kind != BCMLRD_MAP_TABLE_COMMIT) {
            continue;
        }

        /* Process each map entry */
        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];

            switch(entry->entry_type) {
            case BCMLRD_MAP_ENTRY_TABLE_COMMIT_HANDLER:
                if (handler_idx >= total_num_tc) {
                    const char *table_name;

                    table_name = bcmltm_lt_table_sid_to_name(unit, sid);
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "LTM Table Commit Handler: "
                                          "current table commit entry count "
                                          "exceeds max count %s(ltid=%d) "
                                          "count=%d max_count=%d\n"),
                               table_name, sid,
                               (handler_idx+1), total_num_tc));
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }

                handler_id[handler_idx] = entry->u.handler_id;
                handler_idx++;
                break;
            default:
                break;
            }
        }
    }

    lt_map->num_handlers = handler_idx;
    lt_map->handler_id = handler_id;

 exit:
    if (SHR_FUNC_ERR()) {
        if (lt_map->handler_id == NULL) {
            SHR_FREE(handler_id);
        }
        tc_map_entries_cleanup(lt_map);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the Table Commit list metadata.
 *
 * Destroy the Table Commit list metadata.
 *
 * \param [in] tc_list Table Commit list to destroy.
 */
static void
table_commit_list_destroy(bcmltm_table_commit_list_t *tc_list)
{
    if (tc_list == NULL) {
        return;
    }

    SHR_FREE(tc_list->handlers);
    SHR_FREE(tc_list);

    return;
}

/*!
 * \brief Create the Table Commit list metadata for given table.
 *
 * Create the Table Commit list metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_map Table Commit mapping information.
 * \param [out] tc_list Returning pointer for Table Commit list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
table_commit_list_create(int unit,
                         const tc_map_entries_t *lt_map,
                         bcmltm_table_commit_list_t **tc_list_ptr)
{
    bcmltm_table_commit_list_t *tc_list = NULL;
    const bcmltd_table_commit_handler_t **handlers = NULL;
    uint32_t num_handlers;
    unsigned int size;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    *tc_list_ptr = NULL;

    num_handlers = lt_map->num_handlers;
    if (num_handlers == 0) {
        SHR_EXIT();
    }

    /* Allocate */
    SHR_ALLOC(tc_list, sizeof(*tc_list), "bcmltmTableCommitList");
    SHR_NULL_CHECK(tc_list, SHR_E_MEMORY);
    sal_memset(tc_list, 0, sizeof(*tc_list));

    size = sizeof(*handlers) * num_handlers;
    SHR_ALLOC(handlers, size, "bcmltmTableCommitHandlers");
    SHR_NULL_CHECK(handlers, SHR_E_MEMORY);
    sal_memset(handlers, 0, size);

    for (i = 0; i < num_handlers; i++) {
        handlers[i] =
            bcmltm_lta_intf_table_commit_handler_get(unit,
                                                     lt_map->handler_id[i]);
    }

    tc_list->handlers = handlers;
    tc_list->num_handlers = num_handlers;

    *tc_list_ptr = tc_list;

 exit:
    if (SHR_FUNC_ERR()) {
        if ((tc_list == NULL) || (tc_list->handlers == NULL)) {
            SHR_FREE(handlers);
        }
        table_commit_list_destroy(tc_list);
        *tc_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_tc_info_create(int unit, bcmlrd_sid_t sid,
                         bcmltm_db_tc_info_t **info_ptr)
{
    bcmltm_db_tc_info_t *info = NULL;
    tc_map_entries_t map_entries;

    SHR_FUNC_ENTER(unit);

    /* Obtain LT map entries information */
    SHR_IF_ERR_EXIT
        (tc_map_entries_parse(unit, sid, &map_entries));

    /* Allocate */
    SHR_ALLOC(info, sizeof(*info), "bcmltmDbTcInfo");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    /* Create LT info for Table Commit handlers */
    SHR_IF_ERR_EXIT
        (table_commit_list_create(unit, &map_entries, &info->tc_list));
    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_tc_info_destroy(info);
        *info_ptr = NULL;
    }

    tc_map_entries_cleanup(&map_entries);

    SHR_FUNC_EXIT();
}

void
bcmltm_db_tc_info_destroy(bcmltm_db_tc_info_t *info)
{
    if (info == NULL) {
        return;
    }

    /* Destroy Table Commit info */
    table_commit_list_destroy(info->tc_list);

    SHR_FREE(info);

    return;
}

