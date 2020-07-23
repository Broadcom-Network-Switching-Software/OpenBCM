/*! \file spm_info.c
 *
 * Strength Profile manager info LT support.
 *
 * These functions provide support for the Strength Profile
 * resource LT CTHs for user examination of strength profile physical
 * index usage.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <sal/sal_assert.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_table_constants.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmlrd_ptrm.h>
#include <bcmptm/bcmptm_spm_internal.h>
#include <bcmptm/bcmptm_spm.h>
#include <bcmptm/bcmptm_uft.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>

#include "sbr_device.h"
#include "sbr_internal.h"
#include "sbr_util.h"
#include "spm_util.h"

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_UFT

static bcmptm_spm_resource_root_t *spm_resource_root_all[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Perform SP resource info LT lookup by index.
 *
 * Strength Profile LTs have a corresponding resource LT to allow
 * inspection of the current entry usage state.  This function
 * provides the common implementation for retrieving this information
 * for a given LT index.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  lt_index        SPT resource LT index value.
 * \param [in]  lt_record       Pointer to the SPM record structure.
 * \param [in]  ltr_info        Pointer to bcmptm_spm_lt_resource_info_t.
 * \param [in]  lt_fields       Pointer to bcmptm_spm_lt_fields_t structure.
 * \param [out] out             LTD output field set.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
static int
spm_resource_lookup_core(int unit,
                         uint32_t lt_index,
                         bcmptm_spm_lt_record_t *lt_record,
                         bcmptm_spm_lt_resource_info_t *ltr_info,
                         bcmptm_spm_lt_fields_t *spm_lt_fields,
                         bcmltd_fields_t *out)
{
    bcmltd_field_t *lta_out_field;
    bcmptm_spm_entry_state_t spm_state;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_spm_user_lt_index_state(unit, lt_record, spm_lt_fields,
                                        lt_index, &spm_state));

    /* Append key field output */
    SHR_NULL_CHECK(out, SHR_E_INTERNAL);
    lta_out_field = out->field[out->count];
    lta_out_field->data = spm_state;  
    lta_out_field->id = ltr_info->value_fid;
    lta_out_field->idx = 0;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Perform TRAVERSE call for SP resource LTs.
 *
 * Strength Profile LTs have a corresponding resource LT to allow
 * inspection of the current entry usage state.  This function
 * provides the common implementation for TRAVERSE first and next.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  op_arg          Operation arguments.
 * \param [in]  in              LTD input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
static int
spm_resource_traverse_core(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    uint32_t ltid, sp_ltid;
    uint16_t lt_index = 0;
    bcmptm_spm_resource_root_t *resource_root;
    bcmptm_spm_lt_resource_info_t *ltr_info;
    bcmltd_field_t *lta_out_field, *key_field;
    bcmptm_spm_lt_fields_t *spm_lt_fields;

    SHR_FUNC_ENTER(unit);

    resource_root = spm_resource_root_all[unit];
    SHR_NULL_CHECK(resource_root, SHR_E_UNAVAIL);

    ltid = arg->sid;
    ltr_info = bcmptm_spm_lt_resource_find(resource_root, ltid);
    SHR_NULL_CHECK(ltr_info, SHR_E_UNAVAIL);

    sp_ltid = ltr_info->sp_ltid;
    SHR_NULL_CHECK(resource_root->spm_lt_record, SHR_E_UNAVAIL);
    spm_lt_fields =
        bcmptm_spm_lt_fields_find(resource_root->spm_lt_record,
                                  sp_ltid);
    SHR_NULL_CHECK(spm_lt_fields, SHR_E_UNAVAIL);

    if (in == NULL) {
        lt_index = 0;
    } else {
        /* Retrieve LT index value from LTA input */
        if (in->count != 1) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        key_field = in->field[0];
        if (key_field->id != ltr_info->key_fid) {
            /* Wrong ID */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        lt_index = key_field->data;

        /* Traverse to next key */
        lt_index++;

        if (lt_index >= spm_lt_fields->lt_index_max) {
            /* End of table reached */
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

    /* Append key field output */
    SHR_NULL_CHECK(out, SHR_E_INTERNAL);
    out->count = 0;
    lta_out_field = out->field[0];
    lta_out_field->data = lt_index;
    lta_out_field->id = ltr_info->key_fid;
    lta_out_field->idx = 0;
    out->count++;

    SHR_IF_ERR_EXIT
        (spm_resource_lookup_core(unit, lt_index,
                                  resource_root->spm_lt_record,
                                  ltr_info, spm_lt_fields, out));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcmptm_spm_resource_info_init(int unit, bool warm,
                              bcmptm_spm_lt_record_t *spm_lt_record)
{
    uint32_t ix, spm_num, spri_ix;
    bcmltd_sid_t sid;
    bcmptm_spm_resource_root_t *resource_root = NULL;
    bcmptm_spm_lt_resource_info_t *ltr_info;
    const bcmlrd_hmf_t *hmf_map;
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_table_rep_t *tbl = NULL;
    const bcmptm_sbr_lt_str_pt_list_t *lt_str_pt_list;
    uint32_t spri_lt_count;
    uint32_t alloc_size;
    uint32_t fid;
    int rv, key_num, value_num;

    SHR_FUNC_ENTER(unit);

    if (spm_resource_root_all[unit] != NULL) {
        /* Reinitializing without shutdown */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Count resource LTs */
    spri_lt_count = 0;
    lt_str_pt_list = spm_lt_record->lt_str_pt_list;
    spm_num = lt_str_pt_list->lt_str_pt_info_count;

    for (ix = 0; ix < spm_num; ix++) {
        hmf_map =
            lt_str_pt_list->lt_str_pt_info[ix].hw_entry_info->l2p_map_info.map;
        if (hmf_map != NULL) {
            /* If this SPLT has a resource info LT */
            if ((hmf_map->resource_sid != BCMLTD_INVALID_LT) &&
                (hmf_map->resource_sid != 0)) { 
                spri_lt_count++;
            }
        }
    }

    if (spri_lt_count == 0) {
        /* No LTs mapped, nothing to do. */
        SHR_EXIT();
    }

    /* Allocate resource info struture space */
    alloc_size = sizeof(bcmptm_spm_resource_root_t) +
        sizeof(bcmptm_spm_lt_resource_info_t) * spri_lt_count;
    SHR_ALLOC(resource_root, alloc_size, "bcmptmSpmResourceRoot");
    SHR_NULL_CHECK(resource_root, SHR_E_MEMORY);
    sal_memset(resource_root, 0, alloc_size);

    /* Populate records */
    resource_root->spm_lt_record = spm_lt_record;
    resource_root->resource_lt_count = spri_lt_count;

    spri_ix = 0;
    for (ix = 0; ix < spm_num; ix++) {
        hmf_map =
            lt_str_pt_list->lt_str_pt_info[ix].hw_entry_info->l2p_map_info.map;
        if (hmf_map != NULL) {
            /* If this SPLT has a resource info LT */
            if (hmf_map->resource_sid == BCMLTD_INVALID_LT) {
                continue;
            }
        }

        ltr_info = &(resource_root->resource_lt[spri_ix]);
        ltr_info->ltid = hmf_map->resource_sid;
        ltr_info->sp_ltid = hmf_map->sid;

        /* Get LRD information */
        tbl = bcmlrd_table_get(ltr_info->ltid);
        SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

        sid = ltr_info->ltid;
        rv = bcmlrd_map_get(unit, sid, &map);
        if (SHR_SUCCESS(rv) && map) {
            /*
             * The logical field IDs are always [0..(max_num_fields-1)]
             * by design, where max_num_fields is the total number of
             * fields defined in a table.
             */
            key_num = value_num = 0;
            for (fid = 0; fid < tbl->fields; fid++) {
                if (bcmlrd_map_field_is_unmapped(map, fid)) {
                    /* Skip unmapped fields */
                    continue;
                }

                if (tbl->field[fid].flags & BCMLRD_FIELD_F_KEY) {
                    key_num++;
                    ltr_info->key_fid = fid;
                } else {
                    value_num++;
                    ltr_info->value_fid = fid;
                }
            }

            /* SPRI LTs have one key and one value */
            if ((key_num != 1) || (value_num != 1)) {
                /* Improper SP resource LT format. */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        }

        /* Advance resource LT index */
        spri_ix++;
    }

    spm_resource_root_all[unit] = resource_root;

exit:
    if (SHR_FUNC_ERR()) {
        bcmptm_spm_resource_info_cleanup(unit, warm);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_spm_resource_info_cleanup(int unit, bool warm)
{
    SHR_FREE(spm_resource_root_all[unit]);

    return SHR_E_NONE;
}

int
bcmptm_spm_resource_info_validate(int unit,
                                  bcmlt_opcode_t opcode,
                                  const bcmltd_fields_t *in,
                                  const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    switch (opcode) {
        case BCMLT_OPCODE_LOOKUP:
        case BCMLT_OPCODE_TRAVERSE:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_spm_resource_info_lookup(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg)
{
    uint32_t ltid, sp_ltid;
    uint16_t lt_index = 0;
    bcmptm_spm_resource_root_t *resource_root;
    bcmptm_spm_lt_resource_info_t *ltr_info;
    bcmltd_field_t *key_field;
    bcmptm_spm_lt_fields_t *spm_lt_fields;

    SHR_FUNC_ENTER(unit);

    resource_root = spm_resource_root_all[unit];
    SHR_NULL_CHECK(resource_root, SHR_E_UNAVAIL);

    ltid = arg->sid;
    ltr_info = bcmptm_spm_lt_resource_find(resource_root, ltid);
    SHR_NULL_CHECK(ltr_info, SHR_E_UNAVAIL);

    sp_ltid = ltr_info->sp_ltid;
    SHR_NULL_CHECK(resource_root->spm_lt_record, SHR_E_UNAVAIL);
    spm_lt_fields =
        bcmptm_spm_lt_fields_find(resource_root->spm_lt_record,
                                  sp_ltid);
    SHR_NULL_CHECK(spm_lt_fields, SHR_E_UNAVAIL);

    SHR_NULL_CHECK(in, SHR_E_INTERNAL);

    /* Retrieve LT index value from LTA input */
    if (in->count != 1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    key_field = in->field[0];
    if (key_field->id != ltr_info->key_fid) {
        /* Wrong ID */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    lt_index = key_field->data;

    /* Prepare LTA output */
    SHR_NULL_CHECK(out, SHR_E_INTERNAL);
    out->count = 0;

    SHR_IF_ERR_EXIT
        (spm_resource_lookup_core(unit, lt_index,
                                  resource_root->spm_lt_record,
                                  ltr_info, spm_lt_fields, out));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_spm_resource_info_first(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    if (in != NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (spm_resource_traverse_core(unit, op_arg, in, out, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_spm_resource_info_next(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    /* Traverse next must have key field(s) */
    SHR_NULL_CHECK(in, SHR_E_INTERNAL);

    SHR_IF_ERR_EXIT
        (spm_resource_traverse_core(unit, op_arg, in, out, arg));

exit:
    SHR_FUNC_EXIT();
}

