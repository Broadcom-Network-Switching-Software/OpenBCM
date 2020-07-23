/*! \file bcmltm_wb_lta.c
 *
 * Logical Table Manager Working Buffer for LTA interfaces.
 *
 * This file contains routines to manage the working buffer blocks
 * used for operations that require to assemble data for the LTA
 * interfaces:
 *     Custom Table Handler
 *     Field Transforms
 *     Field Validations
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

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>
#include <bcmltm/bcmltm_wb_internal.h>
#include <bcmltm/bcmltm_wb_lta_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE       BSL_LS_BCMLTM_METADATA


/* Sizes (in words) for the LTA fields 'param' section */
#define PARAM_WSIZE      (BCMLTM_BYTES2WORDS(sizeof(bcmltd_fields_t)))

/* Sizes (in words) for the LTA fields 'ptrs' section */
#define PTRS_WSIZE(_n)   (BCMLTM_BYTES2WORDS(sizeof(bcmltd_field_t*) * _n))

/* Sizes (in words) for the LTA fields 'field' array section */
#define FIELD_WSIZE(_n)  (BCMLTM_BYTES2WORDS(sizeof(bcmltd_field_t)) * _n)


/* Checks that type is a valid LTA block type */
#define BLOCK_LTA_TYPE_VALID(_type)                        \
    ((_type == BCMLTM_WB_BLOCK_TYPE_LTA_CTH) ||            \
     (_type == BCMLTM_WB_BLOCK_TYPE_LTA_XFRM) ||           \
     (_type == BCMLTM_WB_BLOCK_TYPE_LTA_VAL))


/*
 * Check that block type is valid.  Function exits if invalid.
 */
#define LTA_BLOCK_TYPE_CHECK(_b)                                    \
    do {                                                            \
        if (!BLOCK_LTA_TYPE_VALID(_b->type)) {                      \
            LOG_ERROR(BSL_LOG_MODULE,                               \
                      (BSL_META("LTM invalid LTA working buffer "   \
                                "LTA block type: "                  \
                                "block_type=%d\n"),                 \
                       _b->type));                                  \
            return SHR_E_INTERNAL;                                  \
        }                                                           \
    } while (0)

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get working buffer LTA fields offsets.
 *
 * This function gets the LTA fields offsets for the given number of
 * fields elements (fid, idx) with specified base offset.
 *
 * \param [in] base_offset Base offset of LTA fields section.
 * \param [in] num_fields Number of field elements (fid, idx).
 * \param [out] offsets Returning LTA fields offsets.
 *
 * \retval Size, in words, of LTA fields section.
 */
static uint32_t
lta_fields_offsets_get(uint32_t base_offset,
                       uint32_t num_fields,
                       bcmltm_wb_lta_fields_offsets_t *offsets)
{
    uint32_t wsize = 0;
    uint32_t param_wsize;
    uint32_t ptrs_wsize;
    uint32_t field_wsize;

    /*
     * WB LTA fields section layout
     *
     * Memory is allocated for the given number of fields.
     *
     * LTA Fields:
     *     +------------------------------+
     *     | Param:                       |
     *     |     bcmltd_fields_t          |
     *     |                              |
     *     |------------------------------|
     *     | Ptrs:                        |
     *     |     (bcmltd_field_t*) * n    |
     *     |                              |
     *     | Array of pointers to fields  |
     *     |------------------------------|
     *     | Field:                       |
     *     |     bcmltd_field_t * n       |
     *     |                              |
     *     | Array of bcmltm_field_t      |
     *     +------------------------------+
     *
     * Where "n" is the number of field elements (fid, idx).
     */

    sal_memset(offsets, 0, sizeof(*offsets));

    param_wsize = PARAM_WSIZE;
    ptrs_wsize = PTRS_WSIZE(num_fields);
    field_wsize = FIELD_WSIZE(num_fields);

    offsets->param = base_offset;
    if (num_fields > 0) {
        /*
         * If number of fields is zero, nothing is allocated
         * for the 'ptrs' and 'field' array sections.
         */
        offsets->ptrs = offsets->param + param_wsize;
        offsets->field = offsets->ptrs + ptrs_wsize;
    }

    wsize = param_wsize + ptrs_wsize + field_wsize;

    return wsize;
}


/*!
 * \brief Get working buffer size for a LTA fields section.
 *
 * This function gets the working buffer size of the LTA fields section
 * for the given number fields.
 *
 * \param [in] num_fields Number of fields.
 *
 * \retval Size, in words, of a LTA fields section.
 */
static int
lta_fields_wsize_get(uint32_t num_fields)
{
    bcmltm_wb_lta_fields_offsets_t offsets;

    return lta_fields_offsets_get(0, num_fields, &offsets);
}


/*!
 * \brief Add an LTA block type to the working buffer handle.
 *
 * This function adds an LTA working buffer block required for LTA
 * interfaces such as Custom Table Handlers, Field Transforms and
 * Field Validations.
 *
 * Valid PTM block types are:
 *     BCMLTM_WB_BLOCK_TYPE_LTA_CTH
 *     BCMLTM_WB_BLOCK_TYPE_LTA_XFRM
 *     BCMLTM_WB_BLOCK_TYPE_LTA_VAL
 *
 * \param [in] unit Unit number.
 * \param [in] type Working buffer block type BCMLTM_WB_BLOCK_TYPE_LTA_xxx
 * \param [in] in_num_fields Number of input fields elements (fid, idx).
 * \param [in] in_key_num_fields Number of input key fields elements (fid, idx).
 * \param [in] out_num_fields Number of output fields elements (fid, idx).
 * \param [in,out] handle Working buffer handle.
 * \param [out] block_id Returning ID for new working buffer block.
 * \param [out] block_ptr Optional return of pointer to working buffer block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_wb_block_lta_add(int unit,
                        bcmltm_wb_block_type_t type,
                        uint32_t in_num_fields,
                        uint32_t in_key_num_fields,
                        uint32_t out_num_fields,
                        bcmltm_wb_handle_t *handle,
                        bcmltm_wb_block_id_t *block_id,
                        bcmltm_wb_block_t **block_ptr)
{
    bcmltm_wb_block_t *block = NULL;
    uint32_t in_fields_wsize;
    uint32_t in_key_fields_wsize;
    uint32_t out_fields_wsize;
    uint32_t block_wsize;
    uint32_t in_base_offset;
    uint32_t in_key_base_offset;
    uint32_t out_base_offset;

    SHR_FUNC_ENTER(unit);

    /*
     * Working Buffer LTA Block Layout
     *
     *    +----------------------+
     *    | Input Fields         |
     *    |                      |
     *    +----------------------+
     *    | Input Key Fields     |
     *    | (Optional)           |
     *    |----------------------|
     *    | Output Fields        |
     *    |                      |
     *    +----------------------+
     *
     * Each section is large enough to hold the number of fields required
     * for the input and the output fields.
     */

    /* Check for handle */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_handle_validate(handle));

    /* Check for valid block type */
    if (!BLOCK_LTA_TYPE_VALID(type)) {
        const char *table_name;
        const char *table_sid_type;

        table_name = bcmltm_table_sid_to_name(unit,
                                              handle->sid, handle->logical);
        table_sid_type = bcmltm_table_sid_type_str(handle->logical);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM invalid LTA working buffer block type: "
                              "%s(%s=%d) block_type=%d\n"),
                   table_name, table_sid_type, handle->sid, type));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Sections sizes */
    in_fields_wsize = lta_fields_wsize_get(in_num_fields);
    in_key_fields_wsize = lta_fields_wsize_get(in_key_num_fields);
    out_fields_wsize = lta_fields_wsize_get(out_num_fields);
    block_wsize = in_fields_wsize + in_key_fields_wsize + out_fields_wsize;

    /* Add block */
    block = bcmltm_wb_block_add(block_wsize, type, handle);
    if (block == NULL) {
        const char *table_name;
        const char *table_sid_type;

        table_name = bcmltm_table_sid_to_name(unit,
                                              handle->sid, handle->logical);
        table_sid_type = bcmltm_table_sid_type_str(handle->logical);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not allocate LTA IO "
                              "working buffer block: "
                              "%s(%s=%d) "
                              "in_num_fields=%d out_num_fields=%d\n"),
                   table_name, table_sid_type, handle->sid,
                   in_num_fields, out_num_fields));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Base offsets for sections */
    in_base_offset     = block->base_offset;
    in_key_base_offset = in_base_offset + in_fields_wsize;
    out_base_offset    = in_key_base_offset + in_key_fields_wsize;

    /* Fill block information */
    block->u.lta.in_num_fields = in_num_fields;
    block->u.lta.in_key_num_fields = in_key_num_fields;
    block->u.lta.out_num_fields = out_num_fields;
    (void)lta_fields_offsets_get(in_base_offset, in_num_fields,
                                 &block->u.lta.offsets.in);
    (void)lta_fields_offsets_get(in_key_base_offset, in_key_num_fields,
                                 &block->u.lta.offsets.in_key);
    (void)lta_fields_offsets_get(out_base_offset, out_num_fields,
                                 &block->u.lta.offsets.out);

    *block_id = block->id;
    if (block_ptr != NULL) {
        *block_ptr = block;
    }

 exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */
int
bcmltm_wb_block_lta_cth_add(int unit,
                            uint32_t num_fields,
                            bcmltm_wb_handle_t *handle,
                            bcmltm_wb_block_id_t *block_id,
                            bcmltm_wb_block_t **block_ptr)
{
    /*
     * Number of input and output fields are the same
     * for Custom Table Handler.
     */
    return bcmltm_wb_block_lta_add(unit, BCMLTM_WB_BLOCK_TYPE_LTA_CTH,
                                   num_fields, 0, num_fields,
                                   handle, block_id, block_ptr);
}


int
bcmltm_wb_block_lta_xfrm_add(int unit,
                             uint32_t in_num_fields,
                             uint32_t in_key_num_fields,
                             uint32_t out_num_fields,
                             bcmltm_wb_handle_t *handle,
                             bcmltm_wb_block_id_t *block_id,
                             bcmltm_wb_block_t **block_ptr)
{
    return bcmltm_wb_block_lta_add(unit, BCMLTM_WB_BLOCK_TYPE_LTA_XFRM,
                                   in_num_fields, in_key_num_fields, out_num_fields,
                                   handle, block_id, block_ptr);
}


int
bcmltm_wb_block_lta_val_add(int unit,
                            uint32_t in_num_fields,
                            bcmltm_wb_handle_t *handle,
                            bcmltm_wb_block_id_t *block_id,
                            bcmltm_wb_block_t **block_ptr)
{
    /* Field validation only requires input fields */
    return bcmltm_wb_block_lta_add(unit, BCMLTM_WB_BLOCK_TYPE_LTA_VAL,
                                   in_num_fields, 0, 0,
                                   handle, block_id, block_ptr);
}


int
bcmltm_wb_lta_fields_offsets_get(const bcmltm_wb_block_t *wb_block,
                                 int field_type,
                                 bcmltm_wb_lta_fields_offsets_t *offsets)
{
    const bcmltm_wb_lta_fields_offsets_t *lta_offsets;

    /* Check for valid block type */
    LTA_BLOCK_TYPE_CHECK(wb_block);

    if (field_type == BCMLTM_WB_LTA_INPUT_FIELDS) {
        lta_offsets = &wb_block->u.lta.offsets.in;
    } else if (field_type == BCMLTM_WB_LTA_INPUT_KEY_FIELDS) {
        lta_offsets = &wb_block->u.lta.offsets.in_key;
    } else {
        lta_offsets = &wb_block->u.lta.offsets.out;
    }

    sal_memcpy(offsets, lta_offsets, sizeof(*lta_offsets));

    return SHR_E_NONE;
}


int
bcmltm_wb_lta_field_map_offset_get(const bcmltm_wb_block_t *wb_block,
                                   int field_type,
                                   uint32_t field_idx,
                                   uint32_t *offset)
{
    uint32_t num_fields;
    const bcmltm_wb_lta_fields_offsets_t *lta_offsets;

    /* Check for valid block type */
    LTA_BLOCK_TYPE_CHECK(wb_block);

    if (field_type == BCMLTM_WB_LTA_INPUT_FIELDS) {
        num_fields = wb_block->u.lta.in_num_fields;
        lta_offsets = &wb_block->u.lta.offsets.in;
    } else if (field_type == BCMLTM_WB_LTA_INPUT_KEY_FIELDS) {
        num_fields = wb_block->u.lta.in_key_num_fields;
        lta_offsets = &wb_block->u.lta.offsets.in_key;
    } else {
        num_fields = wb_block->u.lta.out_num_fields;
        lta_offsets = &wb_block->u.lta.offsets.out;
    }

    if (field_idx >= num_fields) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid working buffer field index: "
                            "index (%d) exceeds max allocated fields (%d)\n"),
                   field_idx, num_fields));
        return SHR_E_INTERNAL;
    }

    *offset = lta_offsets->field + FIELD_WSIZE(field_idx);

    return SHR_E_NONE;
}

