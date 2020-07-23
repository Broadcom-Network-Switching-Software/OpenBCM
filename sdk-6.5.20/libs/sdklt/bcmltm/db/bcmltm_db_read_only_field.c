/*! \file bcmltm_db_read_only_field.c
 *
 * Logical Table Manager - Information for internal validation field maps.
 *
 * This file contains interfaces to construct information for
 * read-only field lists for logical tables.
 *
 * Read-only field lists are used to check API input value fields do not
 * assign to a field which is not writeable.
 *
 * These checks are applicable to INSERT and UPDATE opcodes.
 *
 * The read-only field checks are applicable only to tables without
 * field selections.  Field selection logic checks for read-only fields
 * as part of the copy to API cache.
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

#include "bcmltm_db_map.h"
#include "bcmltm_db_field_select.h"
#include "bcmltm_db_md_util.h"
#include "bcmltm_db_dm_util.h"
#include "bcmltm_db_read_only_field.h"


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
 * This utility routine resizes the given read-only fields list
 * to a smaller size, if necessary, in order to free unused memory.
 *
 * If a resize takes place, the previous map array is freed.
 *
 * \param [in] unit Unit number.
 * \param [in] max_fields Max fields allocated in array.
 * \param [in] num_fields Actual number of fields.
 * \param [in,out] ro_field_list_ptr Returning pointer to resized array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ro_fields_resize(int unit,
                 uint32_t max_fields,
                 uint32_t num_fields,
                 bcmltm_ro_field_list_t **ro_field_list_ptr)
{
    bcmltm_ro_field_list_t *ro_fields = NULL;
    bcmltm_ro_field_list_t *new_ro_fields = NULL;
    unsigned int alloc_size;

    SHR_FUNC_ENTER(unit);

    ro_fields = *ro_field_list_ptr;

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
        SHR_FREE(ro_fields);
        *ro_field_list_ptr = NULL;
        SHR_EXIT();
    }

    /*
     * Reallocate smaller array and copy array elements.
     */
    alloc_size =  sizeof(*ro_fields) +
        sizeof(bcmltm_field_spec_t) * num_fields;
    SHR_ALLOC(new_ro_fields, alloc_size, "bcmltmReadOnlyFieldList");
    SHR_NULL_CHECK(new_ro_fields, SHR_E_MEMORY);
    sal_memcpy(new_ro_fields, ro_fields, alloc_size);

    /* Free old array and return new array */
    SHR_FREE(ro_fields);
    *ro_field_list_ptr = new_ro_fields;

 exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief Create the read-only field mapping metadata.
 *
 * This routine creates the read-only field mapping metadata for
 * given logical table's value fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] info_ptr Returning pointer to ro field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmltm_db_read_only_field_info_create(int unit,
                                      bcmlrd_sid_t sid,
                                      bcmltm_ro_field_list_t **info_ptr)
{
    uint32_t num_fid;      /* Number of API facing fields (fid) */
    uint32_t num_fid_idx;  /* Total fields elements (fid, idx) */
    uint32_t fid_list_count;
    bcmltm_fid_idx_t *fid_list = NULL;
    uint32_t ro_field_count = 0;
    bcmltm_field_spec_t *ro_field_spec = NULL;
    bcmltm_ro_field_list_t *ro_field_list = NULL;
    uint32_t fid;
    uint32_t alloc_size;
    uint32_t i, idx;
    bool is_read_only;

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

    /* Make structure big enough for all (fid, fidx).  Resize after. */
    alloc_size = sizeof(*ro_field_list) +
        sizeof(*ro_field_spec) * num_fid_idx;
    SHR_ALLOC(ro_field_list, alloc_size, "bcmltmReadOnlyFieldList");
    SHR_NULL_CHECK(ro_field_list, SHR_E_MEMORY);
    sal_memset(ro_field_list, 0, alloc_size);

    for (i = 0; i < fid_list_count; i++) {
        fid = fid_list[i].fid;

        SHR_IF_ERR_EXIT
            (bcmltm_db_field_is_read_only(unit, sid, fid, &is_read_only));
        if (is_read_only) {
            for (idx = 0; idx < fid_list[i].idx_count; idx++) {
                ro_field_spec =
                    &(ro_field_list->field_specs[ro_field_count]);

                /* Add fid to read-only list */
                ro_field_spec->field_id = fid;
                ro_field_spec->field_idx = idx;
                ro_field_count++;
            }
        }
    }

    ro_field_list->num_field_specs = ro_field_count;

    /* Resize fields array */
    SHR_IF_ERR_EXIT
        (ro_fields_resize(unit, num_fid_idx, ro_field_count,
                          &ro_field_list));

    *info_ptr = ro_field_list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_read_only_field_info_destroy(ro_field_list);
        *info_ptr = NULL;
    }
    SHR_FREE(fid_list);

    SHR_FUNC_EXIT();
}

void
bcmltm_db_read_only_field_info_destroy(bcmltm_ro_field_list_t *info)
{
    if (info == NULL) {
        return;
    }

    SHR_FREE(info);

    return;
}
