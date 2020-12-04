/*! \file bcmltm_wb_lt_pvt.c
 *
 * Logical Table Manager Working Buffer for LT Private Internal Elements.
 *
 * This file contains routines to manage the working buffer block
 * used for operations that require to assemble data for the
 * private internal elements of a LT.
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

#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>
#include <bcmltm/bcmltm_wb_internal.h>
#include <bcmltm/bcmltm_wb_lt_pvt_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE       BSL_LS_BCMLTM_METADATA

/* Size (in words) of one information internal element (uint32_t) */
#define  INT_ELEM_WSIZE         1


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get working buffer LT internal element offsets.
 *
 * This function gets the LT private internal element offsets
 * with specified base offset.
 *
 * \param [in] base_offset Base offset of LT private internal element section.
 * \param [out] offset Returning LT internal element offsets.
 *
 * \retval Size, in words, of LT internal element section.
 */
static uint32_t
lt_pvt_offsets_get(uint32_t base_offset,
                   bcmltm_wb_lt_pvt_offsets_t *offsets)
{
    uint32_t wsize = 0;
    uint32_t offset = base_offset;

    /*
     * WB LT Private Internal Element section layout
     *
     *     +----------------------+
     *     | Track Index          |
     *     |                      |
     *     |----------------------|
     *     | Index Absent         |
     *     |                      |
     *     |----------------------|
     *     | PT Suppress          |
     *     |                      |
     *     +----------------------+
     */

    sal_memset(offsets, 0, sizeof(*offsets));

    offsets->track_index = offset;
    offset += INT_ELEM_WSIZE;

    offsets->index_absent = offset;
    offset += INT_ELEM_WSIZE;

    offsets->pt_suppress = offset;
    offset += INT_ELEM_WSIZE;

    wsize = offset - base_offset;

    return wsize;
}

/*!
 * \brief Get working buffer size for a LT private internal element section.
 *
 * This function gets the working buffer size of the LT
 * private internal element section.
 *
 * \retval Size, in words, of a LT private section.
 */
static int
lt_pvt_wsize_get(void)
{
    bcmltm_wb_lt_pvt_offsets_t offsets;

    return lt_pvt_offsets_get(0, &offsets);
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_wb_block_lt_pvt_add(int unit,
                           bcmltm_wb_handle_t *handle,
                           bcmltm_wb_block_id_t *block_id,
                           bcmltm_wb_block_t **block_ptr)
{
    bcmltm_wb_block_t *block = NULL;
    uint32_t block_wsize;

    SHR_FUNC_ENTER(unit);

    /*
     * Working Buffer LT Private Layout
     *
     *    +----------------------+
     *    | Internal Elements    |
     *    |                      |
     *    +----------------------+
 */

    /* Check for handle */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_handle_validate(handle));

    /* Sections sizes */
    block_wsize = lt_pvt_wsize_get();

    /* Add block */
    block = bcmltm_wb_block_add(block_wsize,
                                BCMLTM_WB_BLOCK_TYPE_LT_PVT, handle);
    if (block == NULL) {
        const char *table_name;
        const char *table_sid_type;

        table_name = bcmltm_table_sid_to_name(unit,
                                              handle->sid, handle->logical);
        table_sid_type = bcmltm_table_sid_type_str(handle->logical);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not allocate LT Private "
                              "working buffer block: "
                              "%s(%s=%d)\n"),
                   table_name, table_sid_type, handle->sid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }


    /* Fill block information */
    (void)lt_pvt_offsets_get(block->base_offset,
                             &block->u.lt_pvt.offsets);

    *block_id = block->id;
    if (block_ptr != NULL) {
        *block_ptr = block;
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_wb_lt_pvt_offsets_get(const bcmltm_wb_block_t *wb_block,
                             bcmltm_wb_lt_pvt_offsets_t *offsets)
{
    /* Check for valid block type */
    if (wb_block->type != BCMLTM_WB_BLOCK_TYPE_LT_PVT) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid working buffer block type: "
                            "expected=%d actual=%d\n"),
                   BCMLTM_WB_BLOCK_TYPE_LT_PVT, wb_block->type));
        return SHR_E_INTERNAL;
    }

    sal_memcpy(offsets, &wb_block->u.lt_pvt.offsets, sizeof(*offsets));

    return SHR_E_NONE;
}

