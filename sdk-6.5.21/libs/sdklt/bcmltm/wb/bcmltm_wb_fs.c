/*! \file bcmltm_wb_fs.c
 *
 * Logical Table Manager Working Buffer for Field Selections.
 *
 * This file contains routines to manage the working buffer block
 * used for operations that require field selections.
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

#include <bcmlrd/bcmlrd_types.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>
#include <bcmltm/bcmltm_wb_internal.h>
#include <bcmltm/bcmltm_wb_fs_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE    BSL_LS_BCMLTM_METADATA

/* Sizes (in words) for a field select map index (uint32_t) */
#define MAP_INDEX_WSIZE    1

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get working buffer size for the Field Selection Map Index section.
 *
 * This function gets the working buffer size of the Field Selection Map
 * Index section required to hold the given number of field selections.
 *
 * \param [in] num_sels Number of field selections.
 *
 * \retval Size, in words, of the Field Selection Map Index section.
 */
static uint32_t
fs_map_indexes_wsize_get(uint32_t num_sels)
{
    /*
     * The Field Selection Map Index section contains
     * one map index element for each field selection
     */
    return (MAP_INDEX_WSIZE * num_sels);
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_wb_block_fs_add(int unit,
                       uint32_t num_sels,
                       const bcmltm_field_selection_id_t *list,
                       bcmltm_wb_handle_t *handle,
                       bcmltm_wb_block_id_t *block_id,
                       bcmltm_wb_block_t **block_ptr)
{
    bcmltm_wb_block_t *block = NULL;
    uint32_t block_wsize;
    bcmltm_field_selection_id_t *sel_id_list = NULL;
    unsigned int size;

    SHR_FUNC_ENTER(unit);

    /*
     * Working Buffer Field Selection Block Layout
     *
     *    +-------------------------------------+
     *    | Field Selection Map Index           |
     *    |                                     |
     *    | List of field selection map indexes |
     *    |     map_index * n                   |
     *    |                                     |
     *    +-------------------------------------+
     *
     * Where "n" is the number of field selections.
     *
     * The offsets for each map index in the
     * Field Selection Map Index section
     * will be assigned sequentially using
     * the order of the given field selection IDs.
     */

    /* Check for handle */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_handle_validate(handle));

    /* Sections sizes */
    block_wsize = fs_map_indexes_wsize_get(num_sels);

    /* Add block */
    block = bcmltm_wb_block_add(block_wsize,
                                BCMLTM_WB_BLOCK_TYPE_FS,
                                handle);
    if (block == NULL) {
        const char *table_name;
        const char *table_sid_type;

        table_name = bcmltm_table_sid_to_name(unit,
                                              handle->sid, handle->logical);
        table_sid_type = bcmltm_table_sid_type_str(handle->logical);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not allocate Field Selection "
                              "working buffer block: "
                              "%s(%s=%d) num_sels=%d\n"),
                   table_name, table_sid_type, handle->sid, num_sels));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Copy field select list information */
    size = sizeof(*sel_id_list) * num_sels;
    if (size == 0) {
        /* No field selections */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(sel_id_list, size, "bcmltmWbFsFieldSelectionIdArr");
    SHR_NULL_CHECK(sel_id_list, SHR_E_MEMORY);
    sal_memcpy(sel_id_list, list, size);

    /* Fill block information */
    block->u.fs.map_index_base_offset = block->base_offset;
    block->u.fs.num_sels = num_sels;
    block->u.fs.sel_id_list = sel_id_list;

    *block_id = block->id;
    if (block_ptr != NULL) {
        *block_ptr = block;
    }

 exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(sel_id_list);
        if (block != NULL) {
            block->u.fs.sel_id_list = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

int
bcmltm_wb_fs_map_index_offset_get(const bcmltm_wb_block_t *wb_block,
                                  bcmltm_field_selection_id_t sel_id,
                                  uint32_t *offset)
{
    const bcmltm_wb_block_fs_t *fs = NULL;
    uint32_t i;

    /* Check for valid block type */
    if (wb_block->type != BCMLTM_WB_BLOCK_TYPE_FS) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid Field Selection working buffer "
                            "block type: "
                            "block_type=%d\n"),
                   wb_block->type));
        return SHR_E_INTERNAL;
    }

    fs = &wb_block->u.fs;

    /* Find matching field selection ID */
    for (i = 0; i < fs->num_sels; i++) {
        if (fs->sel_id_list[i] == sel_id) {
            break;
        }
    }

    /* Check field selection ID was found */
    if (i >= fs->num_sels) {
        /* Field selection ID not found */
        return SHR_E_NOT_FOUND;
    }

    *offset = fs->map_index_base_offset + (MAP_INDEX_WSIZE * i);

    return SHR_E_NONE;
}
