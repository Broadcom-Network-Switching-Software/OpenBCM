/*! \file bcmltm_db_wide_field.c
 *
 * Logical Table Manager - Information for internal validation field maps.
 *
 * This file contains interfaces to construct information for
 * wide  field lists for logical tables.
 *
 * Wide field maps examine API input value fields for coherency of
 * wide fields (scalar fields of > 64 bits).  The requirement is that
 * all or none of the field elements must be present in the input.
 *
 * These checks are applicable to INSERT and UPDATE opcodes.
 *
 * The wide field check can be applied to tables with or without
 * field selections.
 *
 * These checks replace the former "standard validator" LTA validate
 * function which was implicitly added by FLTG logic.
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

#include <bcmltm/bcmltm_lta_intf_internal.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_db_table_internal.h>

#include "bcmltm_db_map.h"
#include "bcmltm_db_field_select.h"
#include "bcmltm_db_md_util.h"
#include "bcmltm_db_dm_util.h"
#include "bcmltm_db_wide_field.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Resize fields list.
 *
 * This utility routine resizes the given wide fields list
 * to a smaller size, if necessary, in order to free unused memory.
 *
 * If a resize takes place, the previous map array is freed.
 *
 * \param [in] unit Unit number.
 * \param [in] max_fields Max fields allocated in array.
 * \param [in] num_fields Actual number of fields.
 * \param [in,out] wide_field_list_ptr Returning pointer to resized array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
wide_fields_resize(int unit,
                   uint32_t max_fields,
                   uint32_t num_fields,
                   bcmltm_wide_field_list_t **wide_field_list_ptr)
{
    bcmltm_wide_field_list_t *wide_fields = NULL;
    bcmltm_wide_field_list_t *new_wide_fields = NULL;
    unsigned int alloc_size;

    SHR_FUNC_ENTER(unit);

    wide_fields = *wide_field_list_ptr;

    /* Sanity check */
    if (num_fields > max_fields) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Nothing to do if number of fields matches allocated max fields */
    if (num_fields == max_fields) {
        SHR_EXIT();
    }

    /* Free array if there are no field maps */
    if (num_fields == 0) {
        SHR_FREE(wide_fields);
        *wide_field_list_ptr = NULL;
        SHR_EXIT();
    }

    /*
     * Reallocate smaller array and copy array elements.
     */
    alloc_size =  sizeof(*wide_fields) +
        sizeof(bcmltm_wide_field_t) * num_fields;
    SHR_ALLOC(new_wide_fields, alloc_size, "bcmltmWideFieldList");
    SHR_NULL_CHECK(new_wide_fields, SHR_E_MEMORY);
    sal_memcpy(new_wide_fields, wide_fields, alloc_size);

    /* Free old array and return new array */
    SHR_FREE(wide_fields);
    *wide_field_list_ptr = new_wide_fields;

 exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief Create the wide field mapping metadata.
 *
 * This routine creates the wide field mapping metadata for
 * given logical table's value fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] info_ptr Returning pointer to wide field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmltm_db_wide_field_info_create(int unit,
                                 bcmlrd_sid_t sid,
                                 bcmltm_wide_field_list_t **info_ptr)
{
    uint32_t num_fid;      /* Number of API facing fields (fid) */
    uint32_t num_fid_idx;  /* Total fields elements (fid, idx) */
    uint32_t fid_list_count;
    bcmltm_fid_idx_t *fid_list = NULL;
    uint32_t wide_field_count = 0;
    bcmltm_wide_field_t *wide_field = NULL;
    bcmltm_wide_field_list_t *wide_field_list = NULL;
    uint32_t fid;
    uint32_t alloc_size;
    uint32_t i;
    bcmltm_table_field_info_t field_info;

    SHR_FUNC_ENTER(unit);

    *info_ptr = NULL;

    /* Get number of API facing LT value fields and field elements(fid,idx) */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_value_count_get(unit, sid,
                                         &num_fid, &num_fid_idx));

    if (num_fid == 0) {
        SHR_EXIT();
    }

    /* Get list of value field IDs and index count */
    alloc_size = sizeof(*fid_list) * num_fid;
    SHR_ALLOC(fid_list, alloc_size, "bcmltmLtmFidIdxArr");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, alloc_size);
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_value_list_get(unit, sid,
                                        num_fid, fid_list, &fid_list_count));

    /* Make structure big enough for all fid.  Resize after. */
    alloc_size = sizeof(*wide_field_list) +
        sizeof(*wide_field) * num_fid;
    SHR_ALLOC(wide_field_list, alloc_size, "bcmltmWideFieldList");
    SHR_NULL_CHECK(wide_field_list, SHR_E_MEMORY);
    sal_memset(wide_field_list, 0, alloc_size);

    for (i = 0; i < fid_list_count; i++) {
        fid = fid_list[i].fid;

        if (fid_list[i].idx_count > 1) {
            /* Potential wide field */
            SHR_IF_ERR_EXIT
                (bcmltm_db_table_field_info_get(unit, sid, fid, &field_info));
            if (field_info.array_depth == 1) {
                /* Wide field */
                wide_field =
                    &(wide_field_list->wide_field[wide_field_count]);

                /* Add fid to wide list */
                wide_field->field_id = fid;
                wide_field->num_field_idx = fid_list[i].idx_count;
                wide_field_count++;
            }
        }
    }

    wide_field_list->num_wide_fields = wide_field_count;

    /* Resize fields array */
    SHR_IF_ERR_EXIT
        (wide_fields_resize(unit, num_fid_idx, wide_field_count,
                            &wide_field_list));

    *info_ptr = wide_field_list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_wide_field_info_destroy(wide_field_list);
        *info_ptr = NULL;
    }
    SHR_FREE(fid_list);

    SHR_FUNC_EXIT();
}

void
bcmltm_db_wide_field_info_destroy(bcmltm_wide_field_list_t *info)
{
    if (info == NULL) {
        return;
    }

    SHR_FREE(info);

    return;
}
