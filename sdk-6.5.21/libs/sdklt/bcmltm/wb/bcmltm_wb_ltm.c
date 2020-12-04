/*! \file bcmltm_wb_ltm.c
 *
 * Logical Table Manager Working Buffer for LTM Internal Tables.
 *
 * This file contains routines to manage the working buffer block
 * used for operations that require to assemble data for
 * LTM internal table key fields.
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
#include <bcmltm/bcmltm_wb_ltm_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE       BSL_LS_BCMLTM_METADATA

/* Size (in words) of one information element (uint32_t) */
#define  KEY_WSIZE         1


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get working buffer LTM key value offsets.
 *
 * This function gets the LTM internal table key value offsets
 * with specified base offset.
 *
 * \param [in] base_offset Base offset of LTM key values.
 * \param [out] offsets Returning LTM key value offsets.
 *
 * \retval Size, in words, of LTM key values section.
 */
static uint32_t
ltm_offsets_get(uint32_t base_offset,
                bcmltm_wb_ltm_offsets_t *offsets)
{
    uint32_t wsize = 0;

    /*
     * WB LTM key values section layout
     *
     *     +----------------------+
     *     | LTID                 |
     *     |                      |
     *     |----------------------|
     *     | LT Field ID          |
     *     |                      |
     *     |----------------------|
     *     | Field Select Group   |
     *     |                      |
     *     |----------------------|
     *     | Resource Info Index  |
     *     |                      |
     *     +----------------------+
     */

    sal_memset(offsets, 0, sizeof(*offsets));

    offsets->key_value_offsets[BCMLTM_TABLE_KEYS_LTID] = base_offset;
    offsets->key_value_offsets[BCMLTM_TABLE_KEYS_LT_FIELD_ID] =
        offsets->key_value_offsets[BCMLTM_TABLE_KEYS_LTID] + KEY_WSIZE;
    offsets->key_value_offsets[BCMLTM_TABLE_KEYS_SELECT_GROUP] =
        offsets->key_value_offsets[BCMLTM_TABLE_KEYS_LT_FIELD_ID] + KEY_WSIZE;
    offsets->key_value_offsets[BCMLTM_TABLE_KEYS_RESOURCE_INFO] =
        offsets->key_value_offsets[BCMLTM_TABLE_KEYS_SELECT_GROUP] + KEY_WSIZE;

    wsize = KEY_WSIZE * BCMLTM_TABLE_KEYS_COUNT;

    return wsize;
}


/*!
 * \brief Get working buffer size for a LTM internal section.
 *
 * This function gets the working buffer size of the LTM key values section.
 *
 * \retval Size, in words, of a LTM key values section.
 */
static int
ltm_wsize_get(void)
{
    bcmltm_wb_ltm_offsets_t offsets;

    return ltm_offsets_get(0, &offsets);
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_wb_block_ltm_add(int unit,
                        bcmltm_wb_handle_t *handle,
                        bcmltm_wb_block_id_t *block_id,
                        bcmltm_wb_block_t **block_ptr)
{
    bcmltm_wb_block_t *block = NULL;
    uint32_t block_wsize;

    SHR_FUNC_ENTER(unit);

    /*
     * WB LTM key values section layout
     *
     *     +----------------------+
     *     | LTID                 |
     *     |                      |
     *     |----------------------|
     *     | LT Field ID          |
     *     |                      |
     *     |----------------------|
     *     | Field Select         |
     *     |                      |
     *     +----------------------+
     */

    /* Check for handle */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_handle_validate(handle));

    /* Sections sizes */
    block_wsize = ltm_wsize_get();

    /* Add block */
    block = bcmltm_wb_block_add(block_wsize,
                                BCMLTM_WB_BLOCK_TYPE_LTM, handle);
    if (block == NULL) {
        const char *table_name;
        const char *table_sid_type;

        table_name = bcmltm_table_sid_to_name(unit,
                                              handle->sid, handle->logical);
        table_sid_type = bcmltm_table_sid_type_str(handle->logical);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not allocate LTM "
                              "working buffer block: "
                              "%s(%s=%d)\n"),
                   table_name, table_sid_type, handle->sid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }


    /* Fill block information */
    (void)ltm_offsets_get(block->base_offset,
                          &(block->u.ltm.offsets));

    *block_id = block->id;
    if (block_ptr != NULL) {
        *block_ptr = block;
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_wb_ltm_offsets_get(const bcmltm_wb_block_t *wb_block,
                          bcmltm_wb_ltm_offsets_t *offsets)
{
    /* Check for valid block type */
    if (wb_block->type != BCMLTM_WB_BLOCK_TYPE_LTM) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid working buffer block type: "
                            "expected=%d actual=%d\n"),
                   BCMLTM_WB_BLOCK_TYPE_LTM, wb_block->type));
        return SHR_E_INTERNAL;
    }

    sal_memcpy(offsets, &wb_block->u.ltm.offsets,
               sizeof(bcmltm_wb_ltm_offsets_t));

    return SHR_E_NONE;
}

