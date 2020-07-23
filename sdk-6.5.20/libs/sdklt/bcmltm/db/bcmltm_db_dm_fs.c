/*! \file bcmltm_db_dm_fs.c
 *
 * Logical Table Manager - Field Selection for Direct Mapped Tables.
 *
 * This file contains routines to construct information for
 * field selections in direct map logical tables.
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
#include <shr/shr_util.h>
#include <sal/sal_libc.h>
#include <bsl/bsl.h>

#include <bcmlrd/bcmlrd_types.h>

#include <bcmltm/bcmltm_lta_intf_internal.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_wb_apic_internal.h>
#include <bcmltm/bcmltm_wb_fs_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>

#include "bcmltm_db_field_select.h"
#include "bcmltm_db_md_util.h"
#include "bcmltm_db_dm_field.h"
#include "bcmltm_db_dm_fixed_field.h"
#include "bcmltm_db_dm_xfrm.h"
#include "bcmltm_db_validate.h"
#include "bcmltm_db_dm_apic.h"
#include "bcmltm_db_dm_fs.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Add API Cache working buffer block.
 *
 * This routine adds the API Cache working buffer block for the
 * specified logical table ID.
 *
 * The API Cache working buffer block is required for the
 * LTM field select driver implementation.  It is used to hold a
 * working copy the of the API fields (for input and output).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
apic_wb_block_add(int unit,
                  bcmlrd_sid_t sid)
{
    uint32_t max_fields;
    uint32_t max_field_elems;
    uint32_t num_fields;
    unsigned int alloc_size;
    bcmltm_field_idx_count_t *fields = NULL;
    bcmltm_wb_handle_t *wb_handle = NULL;
    bcmltm_wb_block_id_t wb_block_id;

    SHR_FUNC_ENTER(unit);

    /* Get count of field IDs and field elements (fid,idx) */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_count_get(unit, sid,
                                   &max_fields, &max_field_elems));
    if (max_field_elems == 0) {
        SHR_EXIT();
    }

    /* Allocate field idx count array */
    alloc_size = sizeof(*fields) * max_fields;
    SHR_ALLOC(fields, alloc_size, "bcmltmDbDmFieldIdxCountArr");
    SHR_NULL_CHECK(fields, SHR_E_MEMORY);
    sal_memset(fields, 0, alloc_size);

    /* Get list of field IDs and index count */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_list_get(unit, sid,
                                  max_fields, fields, &num_fields));

    /* Add API Cache working buffer block */
    wb_handle = bcmltm_wb_lt_handle_get(unit, sid);
    SHR_IF_ERR_EXIT
        (bcmltm_wb_block_apic_add(unit,
                                  num_fields, fields,
                                  wb_handle, &wb_block_id, NULL));
 exit:
    SHR_FREE(fields);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add Field Selections working buffer block.
 *
 * This routine adds the Field Selections working buffer block for the
 * specified logical table ID.
 *
 * The Field Selections working buffer block is required for the
 * LTM field select driver implementation.  It is used to hold
 * the selection map index.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] selection_list Field selection list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fs_wb_block_add(int unit,
                bcmlrd_sid_t sid,
                const bcmltm_db_field_selection_list_t *selection_list)
{
    uint32_t num_selections;
    unsigned int alloc_size;
    bcmltm_field_selection_id_t *sel_ids = NULL;
    bcmltm_wb_handle_t *wb_handle = NULL;
    bcmltm_wb_block_id_t wb_block_id;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    if ((selection_list == NULL) ||
        (selection_list->num_selections == 0)) {
        SHR_EXIT();
    }

    num_selections = selection_list->num_selections;

    /* Allocate list array */
    alloc_size = sizeof(*sel_ids) * num_selections;
    SHR_ALLOC(sel_ids, alloc_size, "bcmltmDbDmFieldSelectionIdArr");
    SHR_NULL_CHECK(sel_ids, SHR_E_MEMORY);
    sal_memset(sel_ids, 0, alloc_size);

    /* Get list of field selection IDs */
    for (i = 0; i < num_selections; i++) {
        sel_ids[i] = selection_list->selections[i]->selection_id;
    }

    /* Add Field Selections working buffer block */
    wb_handle = bcmltm_wb_lt_handle_get(unit, sid);
    SHR_IF_ERR_EXIT
        (bcmltm_wb_block_fs_add(unit,
                                num_selections, sel_ids,
                                wb_handle, &wb_block_id, NULL));

 exit:
    SHR_FREE(sel_ids);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the entry index from any destination field in a transform.
 *
 * This routine gets the map entry index (PT entry index or PT op)
 * for the given transform from the first available destination field.
 * This information is used when the destination field is not relevant
 * (i.e. all destination fields are assumed to be in the same PT entry op).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] entry LRD transform map entry.
 * \param [out] entry_idx PT entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_pt_entry_idx_get(int unit,
                      bcmlrd_sid_t sid,
                      const bcmlrd_map_entry_t *entry,
                      uint16_t *entry_idx)
{
    bcmlrd_map_entry_type_t entry_type = entry->entry_type;
    const bcmltd_xfrm_handler_t *lta_handler = NULL;
    const bcmltd_field_list_t *field_list;

    SHR_FUNC_ENTER(unit);

    /* Get handler information */
    lta_handler = bcmltm_lta_intf_xfrm_handler_get(unit,
                                            entry->u.xfrm.desc->handler_id);

    /* Sanity check */
    if (lta_handler == NULL) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s(ltid=%d): "
                              "Transform Handler is NULL\n"),
                   table_name, sid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get the right list for destination PT fields */
    if (BCMLTM_DB_MAP_ENTRY_TYPE_IS_FWD_XFRM(entry_type)) {
        field_list = lta_handler->arg->rfield_list;
    } else {
        field_list = lta_handler->arg->field_list;
    }

    /* Expect at least one destination field */
    if (field_list->field_num <= 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Use any field (first) */
    *entry_idx = field_list->field_array[0].entry_idx;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the entry index for given map entry.
 *
 * This routine gets the map entry index (PT entry index or PT op)
 * for the given map entry.
 *
 * A virtual field selector is associated to a PT-op and thus
 * the PT entry index is needed to identify the offset.
 * The PT entry index for a virtual selector can be obtained from
 * any of the selectee map entries within the selector group.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] entry LRD map entry in virtual selector.
 * \param [out] entry_idx PT entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fs_virtual_selector_pt_entry_idx_get(int unit,
                                     bcmlrd_sid_t sid,
                                     const bcmlrd_map_entry_t *entry,
                                     uint16_t *entry_idx)
{
    bcmlrd_map_entry_type_t entry_type = entry->entry_type;

    SHR_FUNC_ENTER(unit);

    if (BCMLTM_DB_MAP_ENTRY_TYPE_IS_DM(entry_type) ||
        BCMLTM_DB_MAP_ENTRY_TYPE_IS_FIXED(entry_type)) {
        *entry_idx = entry->desc.entry_idx;
    } else if (BCMLTM_DB_MAP_ENTRY_TYPE_IS_XFRM(entry_type)) {
        SHR_IF_ERR_EXIT
            (xfrm_pt_entry_idx_get(unit, sid, entry, entry_idx));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get working buffer offset for the virtual field selector.
 *
 * This function gets the working buffer offset for the virtual
 * field selector.
 *
 * A virtual field selector is per-PT, per-PT-op.  Its selector value
 * resides in the PTM working buffer block (unlike logical field selector
 * where the selector value resides in the API Cache working buffer block).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] selection Field selection group.
 * \param [out] offset Virtual selector field working buffer offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fs_virtual_selector_offset_get(int unit,
                               bcmlrd_sid_t sid,
                               const bcmltm_db_dm_arg_t *dm_arg,
                               const bcmltm_db_field_selection_t *selection,
                               uint32_t *offset)
{
    const bcmltm_wb_block_t *ptm_wb_block = NULL;
    const bcmltm_db_field_selection_map_t *selection_map;
    const bcmltm_db_map_entry_list_t *map_entries;
    const bcmltm_db_map_entry_t *map_entry = NULL;
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t sm_idx;
    bcmdrd_sid_t ptid;      /* PT ID */
    uint32_t pt_fid;        /* Virtual destination selector ID */
    uint16_t pt_entry_idx;  /* PT entry index */

    SHR_FUNC_ENTER(unit);

    /*
     * A virtual selector clause is associated to a (PT,PT-op).
     *
     * As such, all maps part of given virtual selector clause
     * share the same destination PT ID and the same PT op (pt_entry_idx).
     *
     * It is assumed that the generated LRD data is correct.
     */

    /* Find first map entry selectee */
    for (sm_idx = 0; sm_idx < selection->num_maps; sm_idx++) {
        selection_map = selection->maps[sm_idx];
        map_entries = selection_map->map_entry_list;
        map = map_entries->map;

        if (map_entries->num_entries > 0) {
            map_entry = &map_entries->entries[0];
            break;
        }
    }

    /* Sanity check */
    if (map_entry == NULL) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Map entry not found: "
                              "%s(ltid=%d) selection_id=%d selector_fid=%d\n"),
                   table_name, sid,
                   selection->selection_id, selection->selector_fid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get LRD group and entry */
    group = bcmltm_db_map_lrd_group_get(map, map_entry);
    entry = bcmltm_db_map_lrd_entry_get(map, map_entry);

    ptid = group->dest.id;
    pt_fid = selection->selector_fid;
    SHR_IF_ERR_EXIT
        (fs_virtual_selector_pt_entry_idx_get(unit, sid, entry,
                                              &pt_entry_idx));

    /* Find PTM working buffer block */
    ptm_wb_block = bcmltm_db_dm_ptm_wb_block_find(unit, sid,
                                                  dm_arg->pt_info, ptid);
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ptm_field_lt_offset_get(ptm_wb_block,
                                           pt_fid,
                                           BCMLTM_FIELD_TYPE_KEY,
                                           BCMLTM_FIELD_DIR_IN, /* n/a */
                                           pt_entry_idx,
                                           offset));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy given field selection metadata.
 *
 * This routine destroys the given field selection metadata.
 *
 * \param [in] fsm Field selection metadata to destroy.
 */
static void
fs_fsm_destroy(bcmltm_field_select_maps_t *fsm)
{
    if (fsm == NULL) {
        return;
    }

    SHR_FREE(fsm);

    return;
}

/*!
 * \brief Create field selection metadata.
 *
 * This routine creates the field selection metadata for the given
 * selection group in logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] selection Field selection group.
 * \param [out] fsm_ptr Returning pointer to field selection metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fs_fsm_create(int unit,
              bcmlrd_sid_t sid,
              const bcmltm_db_dm_arg_t *dm_arg,
              const bcmltm_db_field_selection_t *selection,
              bcmltm_field_select_maps_t **fsm_ptr)
{
    bcmltm_field_select_maps_t *fsm = NULL;
    bcmltm_select_map_t *sm;
    unsigned int alloc_size;
    uint32_t num_maps;
    bcmltm_field_selection_id_t selection_id;
    uint32_t selector_fid;
    bcmltm_field_selection_id_t parent_selection_id;
    uint32_t sfo = BCMLTM_WB_OFFSET_INVALID;  /* Select field offset */
    uint32_t vsfo = BCMLTM_WB_OFFSET_INVALID;  /* Virtual select field offset */
    uint32_t mio = BCMLTM_WB_OFFSET_INVALID;  /* Map index offset */
    uint32_t psmio = BCMLTM_WB_OFFSET_INVALID;  /* Parent map index offset */
    uint32_t parent_map_index = 0;  /* Parent map index */
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    *fsm_ptr = NULL;

    /* Unconditional selection group (level 0) does not have select map */
    if (selection->level == BCMLTM_FIELD_SELECTION_LEVEL_UNCOND) {
        SHR_EXIT();
    }

    num_maps = selection->num_maps;

    selection_id = selection->selection_id;
    selector_fid = selection->selector_fid;
    parent_selection_id = selection->parent_selection_id;
    parent_map_index = selection->parent_map_index;

    /* Allocate info struct */
    alloc_size = sizeof(*fsm) + (sizeof(bcmltm_select_map_t) * num_maps);
    SHR_ALLOC(fsm, alloc_size, "bcmltmDbDmFieldSelectMaps");
    SHR_NULL_CHECK(fsm, SHR_E_MEMORY);
    sal_memset(fsm, 0, alloc_size);

    /* Get offsets */
    if (selector_fid == BCMLRD_FIELD_SELECTOR) {
        /* Selector is virtual field, offset is in PTM block */
        SHR_IF_ERR_EXIT
            (fs_virtual_selector_offset_get(unit, sid,
                                            dm_arg, selection,
                                            &vsfo));
    } else {
        /* Selector is logical field, offset is in APIC block */
        SHR_IF_ERR_EXIT
            (bcmltm_wb_lt_apic_field_offset_get_by_sid(unit, sid,
                                                       selector_fid, 0,
                                                       &sfo));
    }

    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_fs_map_index_offset_get_by_sid(unit, sid,
                                                     selection_id,
                                                     &mio));

    if (parent_selection_id != BCMLRD_INVALID_SELECTOR_INDEX) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_lt_fs_map_index_offset_get_by_sid(unit, sid,
                                                         parent_selection_id,
                                                         &psmio));
    }

    /* Fill data */
    fsm->select_field_offset = sfo;
    fsm->pt_subentry_offset = vsfo;
    fsm->map_index_offset = mio;
    fsm->parent_select_offset = psmio;
    fsm->parent_select_map_index = parent_map_index;
    fsm->num_maps = num_maps;

    for (i = 0; i < num_maps; i++) {
        sm = &fsm->select_maps[i];
        sm->selector_field_value = selection->maps[i]->selector_value;
        sm->map_index = selection->maps[i]->map_index;
    }

    *fsm_ptr = fsm;

 exit:
    if (SHR_FUNC_ERR()) {
        fs_fsm_destroy(fsm);
        *fsm_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy given field selection information.
 *
 * This routine destroys the given field selection information.
 *
 * \param [in] fs Field selection information to destroy.
 */
static void
fs_destroy(bcmltm_db_dm_fs_t *fs)
{
    if (fs == NULL) {
        return;
    }

    /* Destroy API Cache Field metadata */
    bcmltm_db_dm_apic_field_info_destroy(fs->apic_fields);
    fs->apic_fields = NULL;

    /* Destroy Field Validation metadata */
    bcmltm_db_validate_info_destroy(fs->field_validations);
    fs->field_validations = NULL;

    /* Destroy Field Transform metadata */
    bcmltm_db_dm_xfrm_info_destroy(fs->xfrms);
    fs->xfrms = NULL;

    /* Destroy Fixed Fields Mapping metadata */
    bcmltm_db_dm_fixed_field_info_destroy(fs->fixed_fields);
    fs->fixed_fields = NULL;

    /* Destroy Direct Field Mapping metadata */
    bcmltm_db_dm_field_map_info_destroy(fs->field_maps);
    fs->field_maps = NULL;

    /* Destroy Field Selection Maps metadata */
    fs_fsm_destroy(fs->fsm);
    fs->fsm = NULL;

    return;
}

/*!
 * \brief Create field selection information.
 *
 * This routine creates the field selection information for the given
 * selection group in logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] selection_list Field selection list.
 * \param [in] selection Field selection group to create info for.
 * \param [out] fs Returning field selection information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fs_create(int unit,
          bcmlrd_sid_t sid,
          const bcmltm_db_dm_arg_t *dm_arg,
          const bcmltm_db_field_selection_list_t *selection_list,
          const bcmltm_db_field_selection_t *selection,
          bcmltm_db_dm_fs_t *fs)
{
    bcmltm_db_dm_apic_arg_t apic_arg;

    SHR_FUNC_ENTER(unit);

    sal_memset(fs, 0, sizeof(*fs));

    fs->selection_id = selection->selection_id;
    fs->selector_type = selection->selector_type;
    fs->level = selection->level;

    /* Create Field Selection Maps metadata */
    SHR_IF_ERR_EXIT
        (fs_fsm_create(unit, sid, dm_arg, selection, &fs->fsm));

    /* Create Direct Field Mapping metadata */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_map_info_create(unit, sid, dm_arg,
                                            selection,
                                            &fs->field_maps));

    /* Create Fixed Field Mapping metadata */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_field_info_create(unit, sid, dm_arg,
                                              selection,
                                              &fs->fixed_fields));

    /* Create Field Transform metadata */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_xfrm_info_create(unit, sid, dm_arg,
                                       selection,
                                       &fs->xfrms));

    /* Create Field Validation metadata */
    SHR_IF_ERR_EXIT
        (bcmltm_db_validate_info_create(unit, sid,
                                        selection,
                                        &fs->field_validations));

    /* Create API Cache Field metadata */
    sal_memset(&apic_arg, 0, sizeof(apic_arg));
    apic_arg.dm_arg = dm_arg;
    apic_arg.direct_field_maps = fs->field_maps;
    apic_arg.xfrms = fs->xfrms;
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_apic_field_info_create(unit, sid, &apic_arg,
                                             selection,
                                             &fs->apic_fields));

 exit:
    if (SHR_FUNC_ERR()) {
        fs_destroy(fs);
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_dm_fs_info_create(int unit, bcmlrd_sid_t sid,
                            const bcmltm_db_dm_arg_t *dm_arg,
                            bcmltm_db_dm_fs_info_t **info_ptr)
{
    bcmltm_db_dm_fs_info_t *info = NULL;
    bcmltm_db_dm_fs_t *fs = NULL;
    bcmltm_db_field_selection_list_t *selection_list = NULL;
    const bcmltm_db_field_selection_t *selection = NULL;
    unsigned int alloc_size;
    uint32_t num_selections = 0;
    uint32_t level;
    uint32_t s_idx;
    uint32_t fs_idx = 0;

    SHR_FUNC_ENTER(unit);

    *info_ptr = NULL;

    /* Get field selections data */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_selection_list_create(unit, sid,
                                               &selection_list));
    if ((selection_list == NULL) ||
        (selection_list->num_selections == 0)) {
        SHR_EXIT();
    }

    num_selections = selection_list->num_selections;

    /* Add API Cache working buffer block */
    SHR_IF_ERR_EXIT
        (apic_wb_block_add(unit, sid));

    /* Add Field Selections working buffer block */
    SHR_IF_ERR_EXIT
        (fs_wb_block_add(unit, sid, selection_list));

    /* Allocate info struct */
    SHR_ALLOC(info, sizeof(*info), "bcmltmDbDmFsInfo");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    /* Allocate selections array */
    alloc_size = sizeof(bcmltm_db_dm_fs_t) * num_selections;
    SHR_ALLOC(info->selections, alloc_size, "bcmltmDbDmFsArr");
    SHR_NULL_CHECK(info->selections, SHR_E_MEMORY);
    sal_memset(info->selections, 0, alloc_size);

    info->num_levels = selection_list->num_levels;
    info->num_with_selectors = selection_list->num_with_selectors;
    info->num_selections = num_selections;

    /*
     * Create information for each field selection group.
     * Place selections in level order.
     */
    for (level = 0; level < selection_list->num_levels; level++) {
        for (s_idx = 0; s_idx < num_selections; s_idx++) {
            selection = selection_list->selections[s_idx];
            if (selection->level != level) {
                continue;
            }

            fs = &info->selections[fs_idx];
            SHR_IF_ERR_EXIT
                (fs_create(unit, sid, dm_arg,
                           selection_list, selection, fs));
            fs_idx++;
        }
    }

    /* Sanity check */
    if (fs_idx != num_selections) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_dm_fs_info_destroy(info);
        *info_ptr = NULL;
    }

    bcmltm_db_field_selection_list_destroy(selection_list);

    SHR_FUNC_EXIT();
}

void
bcmltm_db_dm_fs_info_destroy(bcmltm_db_dm_fs_info_t *info)
{
    bcmltm_db_dm_fs_t *fs = NULL;
    uint32_t i;

    if (info == NULL) {
        return;
    }

    /* Destroy each selection */
    for (i = 0; i < info->num_selections; i++) {
        fs = &info->selections[i];
        fs_destroy(fs);
    }

    SHR_FREE(info->selections);
    SHR_FREE(info);

    return;
}

const bcmltm_db_dm_fs_t *
bcmltm_db_dm_fs_find(const bcmltm_db_dm_fs_info_t *info,
                     bcmltm_field_selection_id_t selection_id)
{
    const bcmltm_db_dm_fs_t *fs = NULL;
    uint32_t i;

    if ((info == NULL) || (info->num_selections == 0)) {
        return NULL;
    }

    for (i = 0; i < info->num_selections; i++) {
        fs = &info->selections[i];
        if (fs->selection_id == selection_id) {
            return fs;
        }
    }

    return NULL;
}
