/*! \file bcmltm_wb_apic.c
 *
 * Logical Table Manager Working Buffer for API Cache.
 *
 * This file contains routines to manage the working buffer block
 * used for operations that require to copy logical fields
 * between the input/output API field list and the API field cache.
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
#include <bcmltm/bcmltm_wb_apic_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE    BSL_LS_BCMLTM_METADATA

/* Sizes (in words) for a field element (fid, idx) */
#define FIELD_WSIZE       (BCMLTM_BYTES2WORDS(sizeof(bcmltm_field_list_t)))

/* Sizes (in words) for 'n' number of field elements (fid, idx) */
#define FIELDS_WSIZE(_n)  (FIELD_WSIZE * (_n))


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get field elements count.
 *
 * This function gets the total number of field elements (fid, idx)
 * for the given field list.
 *
 * \param [in] num_fields Number of fields in list.
 * \param [in] list List of field index counts.
 *
 * \retval Total number of field elements (fid, idx)
 */
static uint32_t
apic_field_elem_count_get(uint32_t num_fields,
                          const bcmltm_field_idx_count_t *list)
{
    uint32_t i;
    uint32_t num_field_elems = 0;

    /* Get total number of field elements (fid, idx)*/
    for (i = 0; i < num_fields; i++) {
        num_field_elems += list[i].idx_count;
    }

    return num_field_elems;
}

/*!
 * \brief Get working buffer size for the API Cache Fields section.
 *
 * This function gets the working buffer size of the API Cache Fields
 * section required to hold the given number of field elements (fid, idx).
 *
 * \param [in] num_field_elems Number of field elements to allocate.
 *
 * \retval Size, in words, of the API Cache Fields section.
 */
static uint32_t
apic_fields_wsize_get(uint32_t num_field_elems)
{
    return FIELDS_WSIZE(num_field_elems);
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_wb_block_apic_add(int unit,
                         uint32_t num_fields,
                         const bcmltm_field_idx_count_t *list,
                         bcmltm_wb_handle_t *handle,
                         bcmltm_wb_block_id_t *block_id,
                         bcmltm_wb_block_t **block_ptr)
{
    bcmltm_wb_block_t *block = NULL;
    uint32_t block_wsize;
    uint32_t num_field_elems = 0;
    bcmltm_field_idx_count_t *fields = NULL;
    unsigned int size;

    SHR_FUNC_ENTER(unit);

    /*
     * Working Buffer API Cache Block Layout
     *
     *    +-----------------------------------+
     *    | API Fields:                       |
     *    |                                   |
     *    | List of field elements (fid, idx) |
     *    |     bcmltm_field_list_t * n       |
     *    |                                   |
     *    +-----------------------------------+
     *
     * Where "n" is the number of field elements (fid, idx).
     *
     * The offsets for each field element 'bcmltm_field_list_t'
     * will be assigned sequentially using the order of the given
     * field IDs and corresponding index count (0..max_idx).
     */

    /* Check for handle */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_handle_validate(handle));

    /* Sections sizes */
    num_field_elems = apic_field_elem_count_get(num_fields, list);
    block_wsize = apic_fields_wsize_get(num_field_elems);

    /* Add block */
    block = bcmltm_wb_block_add(block_wsize,
                                BCMLTM_WB_BLOCK_TYPE_APIC,
                                handle);
    if (block == NULL) {
        const char *table_name;
        const char *table_sid_type;

        table_name = bcmltm_table_sid_to_name(unit,
                                              handle->sid, handle->logical);
        table_sid_type = bcmltm_table_sid_type_str(handle->logical);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM could not allocate API Cache "
                              "working buffer block: "
                              "%s(%s=%d) num_fields=%d\n"),
                   table_name, table_sid_type, handle->sid, num_fields));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Copy field list information: field ID and index count */
    size = sizeof(*fields) * num_fields;
    if (size == 0) {
        /* No fields */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fields, size, "bcmltmWbApicFieldIdxCountArr");
    SHR_NULL_CHECK(fields, SHR_E_MEMORY);
    sal_memcpy(fields, list, size);

    /* Fill block information */
    block->u.apic.field_base_offset = block->base_offset;
    block->u.apic.num_field_elems = num_field_elems;
    block->u.apic.field_elem_wsize = FIELD_WSIZE;
    block->u.apic.num_fields = num_fields;
    block->u.apic.fields = fields;

    *block_id = block->id;
    if (block_ptr != NULL) {
        *block_ptr = block;
    }

 exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(fields);
        if (block != NULL) {
            block->u.apic.fields = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

int
bcmltm_wb_apic_field_offset_get(const bcmltm_wb_block_t *wb_block,
                                uint32_t field_id,
                                uint32_t field_idx,
                                uint32_t *offset)
{
    const bcmltm_wb_block_apic_t *apic = NULL;
    const bcmltm_field_idx_count_t *field = NULL;
    uint32_t field_elem_count = 0;
    uint32_t i;

    /* Check for valid block type */
    if (wb_block->type != BCMLTM_WB_BLOCK_TYPE_APIC) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("LTM invalid API Cache working buffer "
                            "block type: "
                            "block_type=%d\n"),
                   wb_block->type));
        return SHR_E_INTERNAL;
    }

    apic = &wb_block->u.apic;

    /* Find matching field ID */
    for (i = 0; i < apic->num_fields; i++) {
        field = &apic->fields[i];

        if (field->fid == field_id) {
            break;
        }

        /* Accumulate count of previous field elements */
        field_elem_count += field->idx_count;
    }

    /* Check field ID was found */
    if (i >= apic->num_fields) {
        /* Field ID not found */
        return SHR_E_NOT_FOUND;
    }

    /* Check field index range */
    if (field_idx >= field->idx_count) {
        return SHR_E_NOT_FOUND;
    }

    field_elem_count += field_idx;

    /* Sanity check */
    if (field_elem_count >= apic->num_field_elems) {
        return SHR_E_INTERNAL;
    }

    *offset = apic->field_base_offset + FIELDS_WSIZE(field_elem_count);

    return SHR_E_NONE;
}
