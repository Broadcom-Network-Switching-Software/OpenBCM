/*! \file bcmltm_db_field_select.c
 *
 * Logical Table Manager - Information for Field Selection.
 *
 * This file contains routines to construct information for
 * field selections.
 *
 * This information is derived from the LRD Field Selector maps.
 * and is used to construct various LTM metadata.
 *
 * The metadata created is used for the following LTM driver flow:
 * (field select opcode driver version)
 *     API <-> API Cache <-> [VALIDATE, XFRM] <-> PTM
 *
 * This driver flow can be applied to tables with or without field selections.
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
#include <bcmlrd/bcmlrd_enum.h>

#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>

#include "bcmltm_db_map.h"
#include "bcmltm_db_field_select.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the selector value for given field selector.
 *
 * This routine gets the local selector value for given field selector.
 * The local selector value is the value that is provided by
 * the API during the operation.  This local value is what needs to
 * be set in the LTM metadata.
 *
 * The LRD provides a local selector value in all cases.
 *
 * \param [in] selector LRD field selector.
 * \param [out] selector_value Returning local selector value.
 */
static inline void
local_selector_value_get(const bcmlrd_field_selector_t *selector,
                         uint64_t *selector_value)
{
    *selector_value = selector->selector_value;
}


/*!
 * \brief Find field selection map for a given selector value.
 *
 * This routine searches the given field selection for
 * a field selection map that matches the provided selector value.
 *
 * It returns the matching field selection map if found.
 * Otherwise, it returns NULL.
 *
 * \param [in] selection Field selection to search.
 * \param [in] selector_value Field selector value to find.
 *
 * \retval Pointer to matching field selection map, if found.
 * \retval NULL, if not found.
 */
static bcmltm_db_field_selection_map_t *
field_selection_map_find(const bcmltm_db_field_selection_t *selection,
                         uint64_t selector_value)
{
    bcmltm_db_field_selection_map_t *selection_map = NULL;
    uint32_t i;

    if (selection == NULL) {
        return NULL;
    }

    /* Find matching selector value in maps */
    for (i = 0; i < selection->num_maps; i++) {
        if (selection->maps[i]->selector_value == selector_value) {
            selection_map = selection->maps[i];
            break;
        }
    }

    return selection_map;
}

/*!
 * \brief  Destroy given field selection map.
 *
 * This routine deallocates any memory allocated for the
 * field selection map.
 *
 * \param [in] selection_map Field selection map to destroy.
 */
static void
field_selection_map_destroy(bcmltm_db_field_selection_map_t *selection_map)
{
    if (selection_map == NULL) {
        return;
    }

    bcmltm_db_map_entry_list_destroy(selection_map->map_entry_list);

    SHR_FREE(selection_map);

    return;
}

/*!
 * \brief Add field selection map to given field selection.
 *
 * This routine adds a new field selection map into the given
 * field selection.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in,out] selection Field selection to add new map to.
 * \param [in] selector_value Selector value.
 * \param [in] max_entries Max map entries to allocate in selection map.
 * \param [out] selection_map_ptr Returning pointer to new selection map.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_selection_map_add(int unit,
                        bcmlrd_sid_t sid,
                        bcmltm_db_field_selection_t *selection,
                        uint64_t selector_value,
                        uint32_t max_entries,
                        bcmltm_db_field_selection_map_t **selection_map_ptr)
{
    bcmltm_db_field_selection_map_t *selection_map = NULL;
    unsigned int size;

    SHR_FUNC_ENTER(unit);

    if (selection->num_maps >= selection->max_maps) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s(ltid=%d): "
                              "Field selection maps exceeds max list size "
                              "max=%d\n"),
                   table_name, sid,
                   selection->max_maps));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate field selection map struct */
    size = sizeof(*selection_map);
    SHR_ALLOC(selection_map, size, "bcmltmDbFieldSelectionMap");
    SHR_NULL_CHECK(selection_map, SHR_E_MEMORY);
    sal_memset(selection_map, 0, size);

    /* Allocate for max map entries */
    SHR_IF_ERR_EXIT
        (bcmltm_db_map_entry_list_create(unit, sid,
                                         max_entries,
                                         &selection_map->map_entry_list));

    selection_map->selector_value = selector_value;
    selection_map->map_index = 0;  /* Assigned later */

    /* Add to list */
    selection->maps[selection->num_maps] = selection_map;
    selection->num_maps++;

    *selection_map_ptr = selection_map;

 exit:
    if (SHR_FUNC_ERR()) {
        field_selection_map_destroy(selection_map);
        *selection_map_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Find field selection for a given selection ID.
 *
 * This routine searches the given field selection list for
 * a field selection that matches the provided field selection ID.
 *
 * It returns the matching field selection if found.
 * Otherwise, it returns NULL.
 *
 * \param [in] list Field selections list to search
 * \param [in] selection_id Field selection ID to find.
 *
 * \retval Pointer to matching field selection, if found.
 * \retval NULL, if not found.
 */
static bcmltm_db_field_selection_t *
field_selection_find(const bcmltm_db_field_selection_list_t *list,
                     bcmltm_field_selection_id_t selection_id)
{
    bcmltm_db_field_selection_t *selection = NULL;
    uint32_t i;

    for (i = 0; i < list->num_selections; i++) {
        if (list->selections[i]->selection_id == selection_id) {
            selection = list->selections[i];
            break;
        }
    }

    return selection;
}

/*!
 * \brief Get the field selector type.
 *
 * This routine returns the field selector type for the given
 * selector field ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] selector_fid Selector field ID.
 * \param [out] selector_type Selector type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_selector_type_get(int unit,
                        bcmlrd_sid_t sid,
                        bcmltm_field_selection_id_t selection_id,
                        uint32_t selector_fid,
                        bcmltm_field_selector_type_t *selector_type)
{
    bool is_key;

    SHR_FUNC_ENTER(unit);

    /*
     * Selector type:
     * - Unconditional group is NONE
     * - Virtual selector is KEY
     * - Logical field is KEY or VALUE (depending on field type)
     */
    if (selection_id == BCMLTM_FIELD_SELECTION_ID_UNCOND) {
        *selector_type = BCMLTM_FIELD_SELECTOR_TYPE_NONE;
    } else if (selector_fid == BCMLRD_FIELD_SELECTOR) {
        *selector_type = BCMLTM_FIELD_SELECTOR_TYPE_KEY;
    } else {
        /* Assumes field selector is a logical field */
        SHR_IF_ERR_EXIT
            (bcmltm_db_field_is_key(unit, sid, selector_fid,
                                    &is_key));
        if (is_key) {
            *selector_type = BCMLTM_FIELD_SELECTOR_TYPE_KEY;
        } else {
            *selector_type = BCMLTM_FIELD_SELECTOR_TYPE_VALUE;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Add field selection to given field selection list.
 *
 * This routine adds a new field selection group into the given
 * field selection list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in,out] list Field selections list to add new selection to.
 * \param [in] selection_id Field selection ID.
 * \param [in] selector_fid Selector field ID.
 * \param [in] max_maps Max maps to allocate in selection.
 * \param [out] selection_ptr Returning pointer to new selection.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_selection_add(int unit,
                    bcmlrd_sid_t sid,
                    bcmltm_db_field_selection_list_t *list,
                    bcmltm_field_selection_id_t selection_id,
                    uint32_t selector_fid,
                    uint32_t max_maps,
                    bcmltm_db_field_selection_t **selection_ptr)
{
    bcmltm_db_field_selection_t *selection = NULL;
    const bcmlrd_field_selector_t *selector = NULL;
    unsigned int size;
    bcmltm_field_selection_id_t parent_id = BCMLRD_INVALID_SELECTOR_INDEX;
    uint32_t parent_fid = 0;
    uint64_t parent_value = 0;
    bcmltm_field_selector_type_t selector_type = 0;
    uint64_t local_selector_value;

    SHR_FUNC_ENTER(unit);

    if (list->num_selections >= list->max_selections) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s(ltid=%d): "
                              "Field selections exceeds max list size "
                              "max=%d\n"),
                   table_name, sid,
                   list->max_selections));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate field selection struct */
    size = sizeof(*selection);
    SHR_ALLOC(selection, size, "bcmltmDbFieldSelection");
    SHR_NULL_CHECK(selection, SHR_E_MEMORY);
    sal_memset(selection, 0, size);

    /* Allocate for max selection maps */
    size = sizeof(bcmltm_db_field_selection_map_t *) * max_maps;
    if (size == 0) {
        /* No maps */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(selection->maps, size, "bcmltmDbFieldSelectionMaps");
    SHR_NULL_CHECK(selection->maps, SHR_E_MEMORY);
    sal_memset(selection->maps, 0, size);

    /* Get selector type */
    SHR_IF_ERR_EXIT
        (field_selector_type_get(unit, sid,
                                 selection_id, selector_fid,
                                 &selector_type));

    /* Get parent information (assume LRD data is valid) */
    if (selection_id != BCMLRD_INVALID_SELECTOR_INDEX) {
        selector = &list->selector_data->field_selector[selection_id];
        parent_id = selector->selection_parent;

        /* Check if selection has a parent */
        if (parent_id != BCMLRD_INVALID_SELECTOR_INDEX) {
            /* Get selector value */
            local_selector_value_get(selector, &local_selector_value);

            parent_fid = selector->selector_id;
            parent_value = local_selector_value;
        }
    }

    selection->selection_id = selection_id;
    selection->selector_type = selector_type;
    selection->selector_fid = selector_fid;
    selection->parent_selection_id = parent_id;
    selection->parent_selector_fid = parent_fid;
    selection->parent_selector_value = parent_value;
    selection->parent_map_index = 0;  /* Assigned later */
    selection->level = 0;  /* Assigned later */
    selection->max_maps = max_maps;
    selection->num_maps = 0;

    /* Add to list */
    list->selections[list->num_selections] = selection;
    list->num_selections++;

    /* Update count for selection with selector */
    if (selection_id != BCMLTM_FIELD_SELECTION_ID_UNCOND) {
        list->num_with_selectors++;
    }

    *selection_ptr = selection;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_field_selection_destroy(selection);
        *selection_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the map index for given selection value.
 *
 * This routine returns the map index corresponding to the
 * selector value for the given field selection group.
 *
 * \param [in] selection Field selection.
 * \param [in] selector_value Field selector value to get map index for.
 *
 * \retval Map index for selector value.
 */
static uint32_t
field_selection_map_index_get(const bcmltm_db_field_selection_t *selection,
                              uint64_t selector_value)
{
    const bcmltm_db_field_selection_map_t *selection_map;
    uint32_t map_index = 0;

    /* Find matching selector value in maps */
    selection_map = field_selection_map_find(selection, selector_value);
    if (selection_map != NULL) {
        map_index = selection_map->map_index;
    }

    return map_index;
}

/*!
 * \brief Get the map index for given selection value by selection ID.
 *
 * This routine returns the map index corresponding to the
 * selector value for the given field selection ID.
 *
 * \param [in] list Field selection list.
 * \param [in] selection_id Field selection ID.
 * \param [in] selector_value Field selector value to get map index for.
 *
 * \retval Map index for selector value.
 */
static uint32_t
field_selection_map_index_get_by_id(
                              const bcmltm_db_field_selection_list_t *list,
                              bcmltm_field_selection_id_t selection_id,
                              uint64_t selector_value)
{
    const bcmltm_db_field_selection_t *selection = NULL;
    uint32_t map_index = 0;

    /* Get field selection */
    selection = field_selection_find(list, selection_id);
    map_index = field_selection_map_index_get(selection, selector_value);

    return map_index;
}

/*!
 * \brief Add a selected map entry into field selection list.
 *
 * This routine adds the given selected map entry into
 * the corresponding location within the field selection list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in,out] list Field selection list.
 * \param [in] selector LRD field selector entry to add.
 * \param [in] max_maps Max maps to allocate within a selection.
 * \param [in] max_entries Max map entries to allocate within a selection map.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_selector_map_entry_add(int unit,
                             bcmlrd_sid_t sid,
                             bcmltm_db_field_selection_list_t *list,
                             const bcmlrd_field_selector_t *selector,
                             uint32_t max_maps,
                             uint32_t max_entries)
{
    bcmltm_db_field_selection_t *selection = NULL;
    bcmltm_db_field_selection_map_t *selection_map = NULL;
    bcmltm_field_selection_id_t selection_id;  /* Field selection ID */
    uint32_t selector_fid;     /* Selector field ID */
    uint64_t selector_value;  /* Selector value */
    uint32_t group_idx;  /* Group index of LRD map entry */
    uint32_t entry_idx;  /* Entry index of LRD map entry */
    uint64_t local_selector_value;

    SHR_FUNC_ENTER(unit);

    /* Get selector value */
    local_selector_value_get(selector, &local_selector_value);

    selection_id = selector->selection_parent;
    selector_fid = selector->selector_id;
    selector_value = local_selector_value;
    group_idx = selector->group_index;
    entry_idx = selector->entry_index;

    /* Get field selection */
    selection = field_selection_find(list, selection_id);
    if (selection == NULL) {
        SHR_IF_ERR_EXIT
            (field_selection_add(unit,
                                 sid,
                                 list,
                                 selection_id,
                                 selector_fid,
                                 max_maps,
                                 &selection));
    }

    /* Get field selection map */
    selection_map = field_selection_map_find(selection,
                                             selector_value);
    if (selection_map == NULL) {
        SHR_IF_ERR_EXIT
            (field_selection_map_add(unit,
                                     sid,
                                     selection,
                                     selector_value,
                                     max_entries,
                                     &selection_map));
    }

    /* Add map entry */
    SHR_IF_ERR_EXIT
        (bcmltm_db_map_entry_add(selection_map->map_entry_list,
                                 group_idx,
                                 entry_idx));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Complete field selections hierarchy.
 *
 * This routine adds any missing parent selections or maps for nested
 * selections in order to complete the selection hierarchy.
 *
 * Since only select entries of type BCMLRD_FIELD_SELECTOR_TYPE_MAP_ENTRY
 * are processed, a parent entry may be not part of the initial parsed
 * selection list (this occurs when the parent select entry
 * type is BCMLRD_FIELD_SELECTOR_TYPE_FIELD).  In this case,
 * the corresponding selection group and map is added to the list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in,out] list Field selection list to complete nested selections.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_selection_hierarchy_complete(int unit,
                                   bcmlrd_sid_t sid,
                                   bcmltm_db_field_selection_list_t *list)
{
    bcmltm_db_field_selection_t *selection = NULL;
    bcmltm_db_field_selection_t *parent_selection = NULL;
    bcmltm_db_field_selection_map_t *parent_selection_map = NULL;
    uint32_t max_selection_maps;
    uint32_t max_entries;
    uint32_t s_idx;

    SHR_FUNC_ENTER(unit);

    /* Use max count */
    max_selection_maps = list->selector_data->num_field_selector;
    max_entries = list->selector_data->num_field_selector;

    /*
     * Make sure there is a selection group for the parent.
     *
     * If this is missing (this is possible if the selector
     * type is _FIELD), then add the selection map to complete the
     * selection hierarchy.
     */
    for (s_idx = 0; s_idx < list->num_selections; s_idx++) {
        selection = list->selections[s_idx];

        /* Check for nested selection */
        if (selection->parent_selection_id == BCMLRD_INVALID_SELECTOR_INDEX) {
            continue;
        }

        /* Check for selection group */
        parent_selection =
            field_selection_find(list,
                                 selection->parent_selection_id);
        if (parent_selection == NULL) {
            SHR_IF_ERR_EXIT
                (field_selection_add(unit, sid, list,
                                     selection->parent_selection_id,
                                     selection->parent_selector_fid,
                                     max_selection_maps,
                                     &parent_selection));
        }

        /* Check for selection map */
        parent_selection_map =
            field_selection_map_find(parent_selection,
                                     selection->parent_selector_value);
        if (parent_selection_map == NULL) {
            SHR_IF_ERR_EXIT
                (field_selection_map_add(unit,
                                         sid,
                                         parent_selection,
                                         selection->parent_selector_value,
                                         max_entries,
                                         &parent_selection_map));
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse given LRD field selector data.
 *
 * This routine parses the given LRD map field selector data and
 * populate the field selection list accordingly.
 *
 * Note that only the field selector type
 * BCMLRD_FIELD_SELECTOR_TYPE_MAP_ENTRY is processed since this
 * carries the map entry information (_ROOT and _FIELD are ignored).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in,out] list Field selection list to populate.
 * \param [in] selector_data LRD field selector data to parse.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_selector_parse(int unit,
                     bcmlrd_sid_t sid,
                     bcmltm_db_field_selection_list_t *list,
                     const bcmlrd_field_selector_data_t *selector_data)
{
    const bcmlrd_field_selector_t *selector = NULL;
    uint32_t max_selection_maps;
    uint32_t max_entries;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    /* Check if there is anything to parse */
    if ((selector_data == NULL) ||
        (selector_data->num_field_selector == 0)) {
        SHR_EXIT();
    }

    /* Use max count */
    max_selection_maps = selector_data->num_field_selector;
    max_entries = selector_data->num_field_selector;

    /* Process LRD select data */
    for (i = 0; i < selector_data->num_field_selector; i++) {
        selector = &selector_data->field_selector[i];

        /* Parse only those selected map entries */
        if (selector->selector_type !=
            BCMLRD_FIELD_SELECTOR_TYPE_MAP_ENTRY) {
            continue;
        }

        /* Add map entry to corresponding field selection list */
        SHR_IF_ERR_EXIT
            (field_selector_map_entry_add(unit,
                                          sid,
                                          list,
                                          selector,
                                          max_selection_maps,
                                          max_entries));
    }

    /* Complete selection hierarchy if needed */
    SHR_IF_ERR_EXIT
        (field_selection_hierarchy_complete(unit, sid, list));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the level for the given field selection ID.
 *
 * This routine gets the level corresponding to the given
 * field selection ID.
 *
 * The selection level helps identify which group of map entries
 * needs to be processed first. Selections are processed from
 * level 0 to level n.
 *
 *   Level 0: no field selection.
 *   Level 1: has 0 parents.
 *   ...
 *   Level n: has (n-1) parents.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] selector_data LRD field selector data to parse.
 * \param [in] selection_id Field selection ID to get level for.
 * \param [out] level Field selection level.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_selection_level_get(int unit,
                          bcmlrd_sid_t sid,
                          const bcmlrd_field_selector_data_t *selector_data,
                          bcmltm_field_selection_id_t selection_id,
                          uint32_t *level)
{
    const bcmlrd_field_selector_t *selector;
    uint32_t selection_parent;
    uint32_t max_parents = 0;
    uint32_t num_parents = 0;

    SHR_FUNC_ENTER(unit);

    *level = 0;

    /* Check for special no field selector group */
    if (selection_id == BCMLTM_FIELD_SELECTION_ID_UNCOND) {
        *level = BCMLTM_FIELD_SELECTION_LEVEL_UNCOND;
        SHR_EXIT();
    }

    /* Sanity check: LRD selector data is required at this point */
    if ((selector_data == NULL) ||
        (selector_data->num_field_selector == 0)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    max_parents = selector_data->num_field_selector;

    selector = &selector_data->field_selector[selection_id];
    selection_parent = selector->selection_parent;

    /* Count number of parents for this field selection */
    while (selection_parent != BCMLRD_INVALID_SELECTOR_INDEX) {
        num_parents++;
        selector = &selector_data->field_selector[selection_parent];
        selection_parent = selector->selection_parent;

        /* Sanity check */
        if (num_parents > max_parents) {
            const char *table_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "%s(ltid=%d): "
                                  "Unable to get field selection level, "
                                  "parent count exceeds limit "
                                  "(selection_id=%d max_parents=%d)\n"),
                       table_name, sid,
                       selection_id, max_parents));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    *level = num_parents + 1;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Assign field selection levels.
 *
 * This routine assigns the levels for all the field selections in
 * the given field selection list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in,out] list Field selection list to assign level for.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_selection_level_assign(int unit,
                             bcmlrd_sid_t sid,
                             bcmltm_db_field_selection_list_t *list)
{
    bcmltm_db_field_selection_t *selection;
    uint32_t level = 0;
    uint32_t max_level = 0;
    uint32_t s_idx;

    SHR_FUNC_ENTER(unit);

    for (s_idx = 0; s_idx < list->num_selections; s_idx++) {
        selection = list->selections[s_idx];

        SHR_IF_ERR_EXIT
            (field_selection_level_get(unit, sid,
                                       list->selector_data,
                                       selection->selection_id,
                                       &level));
        /*
         * Sanity check
         *
         * If selector type is KEY, level should be 1.
         *
         * This is expected by the opcodes implementation logic:
         * - all key field select maps are handled first, then
         * - all the value field select maps are handled later (per level).
         */
        if ((selection->selector_type == BCMLTM_FIELD_SELECTOR_TYPE_KEY) &&
            (level != BCMLTM_FIELD_SELECTION_LEVEL_KEY_SELECTOR)) {
            const char *table_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "%s(ltid=%d): "
                                  "Field selection level for Key selector "
                                  "is not %d "
                                  "(selection_id=%d level=%d)\n"),
                       table_name, sid,
                       BCMLTM_FIELD_SELECTION_LEVEL_KEY_SELECTOR,
                       selection->selection_id, level));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        selection->level = level;
        if (level > max_level) {
            max_level = level;
        }
    }

    list->num_levels = max_level + 1;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Assign field selection map indexes.
 *
 * This routine assigns the map indexes for all the field selection maps
 * the given field selection list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in,out] list Field selection list to assign map indexes for.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_selection_map_index_assign(int unit,
                                 bcmlrd_sid_t sid,
                                 bcmltm_db_field_selection_list_t *list)
{
    bcmltm_db_field_selection_t *selection;
    bcmltm_db_field_selection_map_t *selection_map;
    uint32_t map_index;
    uint32_t s_idx;
    uint32_t sm_idx;

    /* Assign the map indexes for the selection */
    for (s_idx = 0; s_idx < list->num_selections; s_idx++) {
        selection = list->selections[s_idx];

        for (sm_idx = 0; sm_idx < selection->num_maps; sm_idx++) {
            selection_map = selection->maps[sm_idx];
            map_index = sm_idx + 1;
            selection_map->map_index = map_index;
        }
    }

    /* If applicable, assign the map index corresponding to its parent */
    for (s_idx = 0; s_idx < list->num_selections; s_idx++) {
        selection = list->selections[s_idx];

        if (selection->parent_selection_id == BCMLRD_INVALID_SELECTOR_INDEX) {
            map_index = 0;
        } else {
            map_index = field_selection_map_index_get_by_id(list,
                                            selection->parent_selection_id,
                                            selection->parent_selector_value);
        }

        selection->parent_map_index = map_index;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Check if LRD map entry is part of a field selection.
 *
 * This routine checks if given LRD map entry is part of a selection.
 * A map entry is part of a selection if there is a matching
 * (group,entry) in the LRD selector data.
 *
 * \param [in] selector_data LRD field selector data.
 * \param [in] group_idx LRD group index to check.
 * \param [in] entry_idx LRD entry index to check.
 *
 * \retval TRUE, if map entry is part of a field selection.
 * \retval FALSE, if map entry is not part of a field selection (unconditional).
 */
static bool
map_entry_is_selected(const bcmlrd_field_selector_data_t *selector_data,
                      uint32_t group_idx,
                      uint32_t entry_idx)
{
    const bcmlrd_field_selector_t *selector = NULL;
    uint32_t i;

    /* If there is no selection, map entry is not part of any selection */
    if ((selector_data == NULL) ||
        (selector_data->num_field_selector == 0)) {
        return FALSE;
    }

    for (i = 0; i < selector_data->num_field_selector; i++) {
        selector = &selector_data->field_selector[i];

        /* Skip non map entry type */
        if (selector->selector_type != BCMLRD_FIELD_SELECTOR_TYPE_MAP_ENTRY) {
            continue;
        }

        /* Check for matching map group/entry index */
        if ((selector->group_index == group_idx) &&
            (selector->entry_index == entry_idx)) {
            return TRUE;
        }
    }

    return FALSE;
}

/*!
 * \brief Add the unconditional field selection group to given list.
 *
 * This routine adds the unconditional field selection group into the given
 * field selection list.
 *
 * The unconditional field selection group contains all those map entries
 * with no field selector.
 *
 * This additional group is helpful during later stages of
 * the metadata construction.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in,out] list Field selection list to add selection to.
 * \param [in] lrd_map LRD map.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_selection_uncond_add(int unit,
                           bcmlrd_sid_t sid,
                           bcmltm_db_field_selection_list_t *list,
                           const bcmlrd_map_t *lrd_map)
{
    const bcmlrd_field_selector_data_t *selector_data = list->selector_data;
    const bcmlrd_map_group_t *group = NULL;
    bcmltm_db_field_selection_t *selection = NULL;
    bcmltm_db_field_selection_map_t *selection_map = NULL;
    uint32_t max_maps = 1;  /* This selection group contains only 1 map view */
    uint64_t selector_value = 0;
    uint32_t max_entries = 0;
    uint32_t group_idx;
    uint32_t entry_idx;

    SHR_FUNC_ENTER(unit);

    /* Add a new selection group for unconditional maps */
    SHR_IF_ERR_EXIT
        (field_selection_add(unit, sid,
                             list,
                             BCMLTM_FIELD_SELECTION_ID_UNCOND,
                             0, /* N/A */
                             max_maps,
                             &selection));

    /* Get max number of map entries in LRD map */
    SHR_IF_ERR_EXIT
        (bcmltm_db_map_entries_count_get(unit, sid,
                                         &max_entries));

    /* Add a new selection map for unconditional maps */
    SHR_IF_ERR_EXIT
        (field_selection_map_add(unit, sid,
                                 selection,
                                 selector_value,
                                 max_entries,
                                 &selection_map));

    /* Add all map entries that are unconditional */
    for (group_idx = 0; group_idx < lrd_map->groups; group_idx++) {
        group = &lrd_map->group[group_idx];

        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            /* Skip maps that are part of a field selection */
            if (map_entry_is_selected(selector_data, group_idx, entry_idx)) {
                continue;
            }

            SHR_IF_ERR_EXIT
                (bcmltm_db_map_entry_add(selection_map->map_entry_list,
                                         group_idx,
                                         entry_idx));
        }
    }

 exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_field_selection_list_create(int unit,
                                      bcmlrd_sid_t sid,
                                      bcmltm_db_field_selection_list_t
                                      **list_ptr)
{
    bcmltm_db_field_selection_list_t *list = NULL;
    const bcmlrd_map_t *map = NULL;
    uint32_t max_selections;
    unsigned int size;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(list, sizeof(*list), "bcmltmDbFieldSelectionList");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));

    list->selector_data = NULL;
    list->num_levels = 0;
    list->max_selections = 0;
    list->num_with_selectors = 0;
    list->num_selections = 0;
    list->selections = NULL;
    *list_ptr = list;

    /*
     * Currently, the LTM field select opcode driver is used
     * only if there are active selectors (with map entries).
     * If there is no selection data or no active selector,
     * this function returns an empty list since the information
     * is not used for MD construction.
     *
     * The LTM field select opcode driver is generic and can be
     * used regardless of whether the table has any
     * active selectors (select with map entries) or not.
     *
     * If it is later decided to use the field select opcode driver
     * for any LT (LT with select or no select),
     * this function needs to be adjusted to return
     * at least the unconditional selection group.
     */

    /* Get LRD field select */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));
    list->selector_data = map->sel;

    /* Return if there is no selection data */
    if ((list->selector_data == NULL) ||
        (list->selector_data->num_field_selector == 0)) {
        SHR_EXIT();
    }

    max_selections = list->selector_data->num_field_selector;

    /* Allocate for max selections */
    size = sizeof(bcmltm_db_field_selection_t *) * max_selections;
    if (size == 0) {
        /* No selections */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(list->selections, size, "bcmltmDbFieldSelections");
    SHR_NULL_CHECK(list->selections, SHR_E_MEMORY);
    sal_memset(list->selections, 0, size);
    list->max_selections = max_selections;

    /* Parse all selected map entries */
    SHR_IF_ERR_EXIT
        (field_selector_parse(unit, sid, list, list->selector_data));

    /*
     * If there are no field selector groups, just return.
     *
     * There is no need to create additional information since
     * the LTM no-field-selection logic flow is used for this case.
     */
    if (list->num_with_selectors == 0) {
        SHR_EXIT();
    }

    /*
     * Add unconditional field selection group.
     *
     * This unconditional field selection group contains all
     * those map entries with no field selector.
     */
    SHR_IF_ERR_EXIT
        (field_selection_uncond_add(unit, sid, list, map));

    /* Assign selection levels */
    SHR_IF_ERR_EXIT
        (field_selection_level_assign(unit, sid, list));

    /* Assign selection map indexes */
    SHR_IF_ERR_EXIT
        (field_selection_map_index_assign(unit, sid, list));

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_field_selection_list_destroy(list);
        *list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_db_field_selection_list_destroy(bcmltm_db_field_selection_list_t *list)
{
    uint32_t i;

    if (list == NULL) {
        return;
    }

    /* Destroy field selections */
    for (i = 0; i < list->num_selections; i++) {
        bcmltm_db_field_selection_destroy(list->selections[i]);
    }

    SHR_FREE(list->selections);
    SHR_FREE(list);

    return;
}

int
bcmltm_db_field_selection_me_create(int unit,
                                    bcmlrd_sid_t sid,
                                    const bcmltm_db_map_entry_list_t *list,
                                    bcmltm_db_field_selection_t **selection_ptr)
{
    bcmltm_db_field_selection_t *selection = NULL;
    bcmltm_db_field_selection_map_t *selection_map = NULL;
    unsigned int alloc_size;
    uint32_t max_maps = 1;  /* This selection group contains only 1 map view */
    uint64_t selector_value = 0;
    const bcmltm_db_map_entry_t *map_entry;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    *selection_ptr = NULL;

    if (list == NULL) {
        SHR_EXIT();
    }

    /* Allocate field selection struct */
    alloc_size = sizeof(*selection);
    SHR_ALLOC(selection, alloc_size, "bcmltmDbFieldSelection");
    SHR_NULL_CHECK(selection, SHR_E_MEMORY);
    sal_memset(selection, 0, alloc_size);

    /* Allocate for max selection maps */
    alloc_size = sizeof(bcmltm_db_field_selection_map_t *) * max_maps;
    SHR_ALLOC(selection->maps, alloc_size, "bcmltmDbFieldSelectionMaps");
    SHR_NULL_CHECK(selection->maps, SHR_E_MEMORY);
    sal_memset(selection->maps, 0, alloc_size);

    selection->selection_id = BCMLTM_FIELD_SELECTION_ID_UNCOND;
    selection->selector_type = BCMLTM_FIELD_SELECTOR_TYPE_NONE;
    selection->selector_fid = 0;
    selection->parent_selection_id = BCMLRD_INVALID_SELECTOR_INDEX;
    selection->parent_selector_fid = 0;
    selection->parent_selector_value = 0;
    selection->parent_map_index = 0;
    selection->level = 0;
    selection->max_maps = max_maps;
    selection->num_maps = 0;

    /* Add a new selection map */
    SHR_IF_ERR_EXIT
        (field_selection_map_add(unit, sid,
                                 selection,
                                 selector_value,
                                 list->num_entries,
                                 &selection_map));
    selection_map->map_index = 1;

    /* Add all map entries in the given list */
    for (idx = 0; idx < list->num_entries; idx++) {
        map_entry = &list->entries[idx];
        SHR_IF_ERR_EXIT
            (bcmltm_db_map_entry_add(selection_map->map_entry_list,
                                     map_entry->group_idx,
                                     map_entry->entry_idx));
    }

    *selection_ptr = selection;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_field_selection_destroy(selection);
        *selection_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief  Destroy given field selection.
 *
 * This routine deallocates any memory allocated for the
 * field selection.
 *
 * \param [in] selection Field selection to destroy.
 */
void
bcmltm_db_field_selection_destroy(bcmltm_db_field_selection_t *selection)
{
    uint32_t i;

    if (selection == NULL) {
        return;
    }

    for (i = 0; i < selection->num_maps; i++) {
        field_selection_map_destroy(selection->maps[i]);
    }

    SHR_FREE(selection->maps);
    SHR_FREE(selection);

    return;
}

void
bcmltm_db_field_selection_list_dump(const bcmltm_db_field_selection_list_t
                                    *list,
                                    shr_pb_t *pb)
{
    const bcmltm_db_field_selection_t *selection;
    const bcmltm_db_field_selection_map_t *selection_map;
    const bcmltm_db_map_entry_list_t *map_entries;
    uint32_t s_idx, sm_idx, me_idx;
    SHR_PB_LOCAL_DECL(pb);

    if (list == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "Field Selection List\n");
    shr_pb_printf(pb,
                  "  num_levels=%d  num_with_selectors=%d num_selections=%d\n",
                  list->num_levels,
                  list->num_with_selectors,
                  list->num_selections);

    for (s_idx = 0; s_idx < list->num_selections; s_idx++) {
        selection = list->selections[s_idx];

        shr_pb_printf(pb, "  [%3d]: "
                      "selection_id=%d selector_fid=%d level=%d num_maps=%d\n",
                      s_idx,
                      selection->selection_id,
                      selection->selector_fid,
                      selection->level,
                      selection->num_maps);
        shr_pb_printf(pb,
                      "         parent_selection_id=%d parent_selector_fid=%d\n",
                      selection->parent_selection_id,
                      selection->parent_selector_fid);
        shr_pb_printf(pb, "         parent_selector_value=%" PRIu64 " "
                      "parent_map_index=%d\n",
                      selection->parent_selector_value,
                      selection->parent_map_index);

        shr_pb_printf(pb, "         Selection maps:\n");
        for (sm_idx = 0; sm_idx < selection->num_maps; sm_idx++) {
            selection_map = selection->maps[sm_idx];
            map_entries = selection_map->map_entry_list;
            shr_pb_printf(pb, "         [%d]: "
                          "selector_value=%" PRIu64
                          " map_index=%d num_map_entries=%d\n",
                          sm_idx,
                          selection_map->selector_value,
                          selection_map->map_index,
                          map_entries->num_entries);

            shr_pb_printf(pb, "               Map Entries: (group,entry)\n");
            shr_pb_printf(pb, "                 ");
            for (me_idx = 0; me_idx < map_entries->num_entries; me_idx++) {
                shr_pb_printf(pb, "(%d,%d) ",
                              map_entries->entries[me_idx].group_idx,
                              map_entries->entries[me_idx].entry_idx);
            }
            shr_pb_printf(pb, "\n");
        }
        shr_pb_printf(pb, "\n");
    }

    SHR_PB_LOCAL_DONE();
}
