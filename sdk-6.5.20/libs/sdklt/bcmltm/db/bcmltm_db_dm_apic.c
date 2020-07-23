/*! \file bcmltm_db_dm_apic.c
 *
 * Logical Table Manager - Information for API Cache Fields.
 *
 * This file contains interfaces to construct information for
 * copying logical fields between the API input/output field list
 * and the API Cache.
 *
 * The metadata created is used for the following LTM driver flow:
 * (field select opcode driver version)
 *     API <-> API Cache <-> [VALIDATE, XFRM] <-> PTM
 *
 * This driver flow can be applied to tables with or without field selections.
 *
 * The API cache field lists contains the union of the logical
 * fields in:
 *   - Direct field maps
 *   - Field transforms (source fields)
 *
 * The API cache field metadata is constructed using
 * the direct field map metadata and the transform metadata.
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
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>

#include "bcmltm_db_util.h"
#include "bcmltm_db_map.h"
#include "bcmltm_db_field_select.h"
#include "bcmltm_db_md_util.h"
#include "bcmltm_db_dm_util.h"
#include "bcmltm_db_dm_field.h"
#include "bcmltm_db_dm_xfrm.h"
#include "bcmltm_db_dm_apic.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*!
 * \brief API Cache Field List Specifiers.
 *
 * This structure contains information to indicate the desired
 * type of field map list to construct information for.
 *
 * It is used as an input argument to routines to pass various
 * data and help reduce the number of input function arguments.
 */
typedef struct {
    /*! Indicates desired selector map index for fields. */
    uint32_t select_map_index;

    /*! Indicates key or value field. */
    bcmltm_field_type_t field_type;

    /*!
     * Indicates what keys to include in returning information.
     * Relevant only for key fields on IwA LTs.
     */
    bcmltm_field_key_type_t key_type;
} bcmltm_db_apic_field_spec_t;


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Resize field maps array.
 *
 * This utility routine resizes the field maps array in the
 * given list to a smaller size, if necessary,
 * in order to free unused memory.
 *
 * If a resize takes place, the previous array is freed.
 *
 * \param [in] unit Unit number.
 * \param [in] max_array Max field maps allocated in array.
 * \param [in,out] list List of field maps.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
apic_field_maps_resize(int unit,
                       uint32_t max_array,
                       bcmltm_field_map_list_t *list)
{
    bcmltm_field_map_t *field_maps = NULL;
    bcmltm_field_map_t *new_field_maps = NULL;
    unsigned int alloc_size;

    SHR_FUNC_ENTER(unit);

    field_maps = list->field_maps;

    /* Sanity check */
    if (list->num_fields > max_array) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Nothing to do if number of maps matches allocated max maps */
    if (list->num_fields == max_array) {
        SHR_EXIT();
    }

    /* Free array if there are no maps */
    if (list->num_fields == 0) {
        SHR_FREE(field_maps);
        list->field_maps = NULL;
        SHR_EXIT();
    }

    /* Reallocate smaller array and copy array elements */
    alloc_size = sizeof(*new_field_maps) * list->num_fields;
    SHR_ALLOC(new_field_maps, alloc_size, "bcmltmDmFieldMaps");
    SHR_NULL_CHECK(new_field_maps, SHR_E_MEMORY);
    sal_memcpy(new_field_maps, field_maps, alloc_size);

    /* Free old array and return new array */
    SHR_FREE(field_maps);
    list->field_maps = new_field_maps;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find field map for given field ID and field index.
 *
 * This routine finds and returns the field map that matches the
 * field ID and field index in the given field list.
 *
 * \param [in] list Field list to search.
 * \param [in] field_id Field ID to find.
 * \param [in] field_idx Field index to find.
 *
 * \retval Pointer to matching field map, if found.
 * \retval NULL, if not found.
 */
static const bcmltm_field_map_t *
apic_field_find(const bcmltm_field_map_list_t *list,
                uint32_t field_id,
                uint32_t field_idx)
{
    const bcmltm_field_map_t *field_map = NULL;
    uint32_t num_fields;
    uint32_t idx;

    if (list == NULL) {
        return NULL;
    }

    num_fields = list->num_fields;
    for (idx = 0; idx < num_fields; idx++) {
        field_map = &list->field_maps[idx];

        if ((field_map->field_id == field_id) &&
            (field_map->field_idx == field_idx)) {
            return field_map;
        }
    }

    return NULL;
}

/*!
 * \brief Add API cache field maps metadata from direct field maps.
 *
 * This routine adds the API cache field maps metadata from the
 * given direct field map list.
 *
 * Assumption: array in field list is large enough to hold
 * all possible logical field elements, there is no array size check.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] list Field map list to add field maps to.
 * \param [in] from_list Direct field map list to copy from.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
apic_direct_field_maps_add(int unit,
                           bcmlrd_sid_t sid,
                           bcmltm_field_map_list_t *list,
                           const bcmltm_field_map_list_t *from_list)
{
    const bcmltm_field_map_t *from_field_map = NULL;
    bcmltm_field_map_t *to_field_map = NULL;
    uint32_t from_num_fields;
    uint32_t from_idx;

    SHR_FUNC_ENTER(unit);

    if (from_list == NULL) {
        /* No field maps, nothing to do */
        SHR_EXIT();
    }

    /*
     * Copy field map.
     *
     * Only one field element copy (fid,idx) is required.
     *
     * The direct field maps metadata is constructed so that
     * a given field element (fid,idx) only appears once in
     * the field map array.  The other field maps for the same
     * field (fid,idx) are placed along the link list.
     *
     * Due to this construction and the assumption that
     * the direct field maps metadata (this routine) is processed
     * before the field transforms, there is no need to check that
     * the field element (fid,idx) is already in the list.
     *
     * Rule: It is a requirement for LTM to supply output fields
     * with the field indexes in order.  Since the resulting list
     * can be used by LTM to return the output fields, this list
     * must follow this rule.
     *
     * This logic assumes that the field indexes in the source list
     * are already in order, so there is no need to check or sort the list.
     */
    from_num_fields = from_list->num_fields;
    for (from_idx = 0; from_idx < from_num_fields; from_idx++) {
        from_field_map = &from_list->field_maps[from_idx];

        /* Add new field map to list */
        to_field_map = &list->field_maps[list->num_fields];
        list->num_fields++;

        /* Fill data */
        to_field_map->field_id = from_field_map->field_id;
        to_field_map->field_idx = from_field_map->field_idx;
        to_field_map->apic_offset = from_field_map->apic_offset;
        to_field_map->pt_changed_offset = from_field_map->pt_changed_offset;
        to_field_map->default_value = from_field_map->default_value;
        to_field_map->minimum_value = from_field_map->minimum_value;
        to_field_map->maximum_value = from_field_map->maximum_value;
        to_field_map->flags = from_field_map->flags;

        /* Not applicable */
        to_field_map->wbc.buffer_offset = BCMLTM_WB_OFFSET_INVALID;
        to_field_map->wbc.maxbit = 0;
        to_field_map->wbc.minbit = 0;
        to_field_map->next_map = NULL;
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add direct field maps metadata for given list type.
 *
 * This routine adds the API cache field maps metadata from
 * the given direct field maps for the specified field list criteria.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] list Field map list to add field maps to.
 * \param [in] fspec Specifies type of field list to add.
 * \param [in] apic_arg API cache arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
apic_direct_field_list_add(int unit,
                           bcmlrd_sid_t sid,
                           bcmltm_field_map_list_t *list,
                           const bcmltm_db_apic_field_spec_t *fspec,
                           const bcmltm_db_dm_apic_arg_t *apic_arg)
{
    const bcmltm_field_select_mapping_t *from_mapping = NULL;
    const bcmltm_field_map_list_t *from_list = NULL;
    uint32_t select_map_index;
    bcmltm_field_type_t field_type;
    bcmltm_field_key_type_t key_type;
    uint32_t fm_idx;

    SHR_FUNC_ENTER(unit);

    if (apic_arg->direct_field_maps == NULL) {
        /* No fields, nothing to do */
        SHR_EXIT();
    }

    select_map_index = fspec->select_map_index;
    field_type = fspec->field_type;
    key_type = fspec->key_type;

    /* Get corresponding field mapping information to parse */
    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        if (key_type == BCMLTM_FIELD_KEY_TYPE_IWA_REQ) {
            from_mapping = apic_arg->direct_field_maps->req_keys;
        } else if (key_type == BCMLTM_FIELD_KEY_TYPE_IWA_ALLOC) {
            from_mapping = apic_arg->direct_field_maps->alloc_keys;
        } else {
            from_mapping = apic_arg->direct_field_maps->in_keys;
        }
    } else {
        from_mapping = apic_arg->direct_field_maps->values;
    }

    if (from_mapping == NULL) {
        /* No fields, nothing to do */
        SHR_EXIT();
    }

    /* Find index of map list that corresponds to the desired select map */
    fm_idx = select_map_index - 1;

    /* Sanity check */
    if (fm_idx >= from_mapping->num_maps) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    from_list = from_mapping->field_map_list[fm_idx];

    /* Add field maps in list */
    SHR_IF_ERR_EXIT
        (apic_direct_field_maps_add(unit, sid, list, from_list));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add API cache field maps metadata from transform field list.
 *
 * This routine adds the API cache field maps metadata from the
 * given transform field list.
 *
 * Assumption: array in field list is large enough to hold
 * all possible logical field elements, there is no array size check.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] list Field map list to add field maps to.
 * \param [in] from_list Transform field map list.  This list must
 *                       be the logical field list (source).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
apic_xfrm_field_maps_add(int unit,
                         bcmlrd_sid_t sid,
                         bcmltm_field_map_list_t *list,
                         const bcmltm_lta_select_field_list_t *from_list)
{
    const bcmltm_lta_select_field_map_t *from_field_map = NULL;
    bcmltm_field_map_t *to_field_map = NULL;
    uint32_t from_num_fields;
    uint32_t from_idx;
    uint32_t field_id;
    uint32_t field_idx;
    uint32_t idx_count = 0;
    uint64_t *default_values = NULL;
    uint64_t *minimum_values = NULL;
    uint64_t *maximum_values = NULL;
    uint32_t prev_field_id = (uint32_t) -1;

    SHR_FUNC_ENTER(unit);

    if (from_list == NULL) {
        /* No field maps, nothing to do */
        SHR_EXIT();
    }

    /*
     * Add field map from transform field list.
     *
     * Only one field element copy (fid,idx) is required.
     *
     * Add a field map only if the field element is not
     * in the list.  A field element may be already in the list
     * from either a direct field map or from previous transforms.
     *
     * Rule: It is a requirement for LTM to supply output fields
     * with the field indexes in order.  Since the resulting list
     * can be used by LTM to return the output fields, this list
     * must follow this rule.
     *
     * This logic assumes that the field indexes in the source list
     * are already in order, so there is no need to check or sort the list.
     */
    from_num_fields = from_list->num_maps;
    for (from_idx = 0; from_idx < from_num_fields; from_idx++) {
        from_field_map = &from_list->lta_select_field_maps[from_idx];

        field_id = from_field_map->field_id;
        field_idx = from_field_map->field_idx;

        /* Skip if field element is already in list */
        if (apic_field_find(list, field_id, field_idx) != NULL) {
            continue;
        }

        /*
         * If field is new, find default, minimum,
         * and maximum values.
         *
         * The following logic tries to optimize time
         * and avoid getting the default, minimum, maximum values
         * again for the same field ID if the previous iteration
         * already has that data.
         */
        if (prev_field_id != field_id) {
            /* Get number of logical field indices */
            idx_count = bcmlrd_field_idx_count_get(unit, sid, field_id);

            /* All fields has at least 1 index count for a mapped entry */
            if (idx_count < 1) {
                const char *table_name;
                const char *field_name;

                table_name = bcmltm_lt_table_sid_to_name(unit, sid);
                field_name = bcmltm_lt_field_fid_to_name(unit, sid, field_id);
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Invalid field index count of zero: "
                                      "%s(ltid=%d) %s(fid=%d)\n"),
                           table_name, sid, field_name, field_id));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }

            /* Get default, min, max value arrays */
            SHR_FREE(default_values);
            SHR_IF_ERR_EXIT
                (bcmltm_db_array_uint64_alloc(idx_count, &default_values));

            SHR_FREE(minimum_values);
            SHR_IF_ERR_EXIT
                (bcmltm_db_array_uint64_alloc(idx_count, &minimum_values));

            SHR_FREE(maximum_values);
            SHR_IF_ERR_EXIT
                (bcmltm_db_array_uint64_alloc(idx_count, &maximum_values));

            SHR_IF_ERR_EXIT
                (bcmltm_db_field_def_min_max_get(unit, sid,
                                                 field_id, idx_count,
                                                 default_values,
                                                 minimum_values,
                                                 maximum_values));
            prev_field_id = field_id;
        }

        /* Sanity check */
        if (field_idx >= idx_count) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* Add new field map to list */
        to_field_map = &list->field_maps[list->num_fields];
        list->num_fields++;

        /* Fill data */
        to_field_map->field_id = field_id;
        to_field_map->field_idx = field_idx;
        to_field_map->apic_offset = from_field_map->apic_offset;
        to_field_map->pt_changed_offset = from_field_map->pt_changed_offset;
        to_field_map->default_value = default_values[field_idx];
        to_field_map->minimum_value = minimum_values[field_idx];
        to_field_map->maximum_value = maximum_values[field_idx];
        to_field_map->flags = from_field_map->flags;

        /* Not applicable */
        to_field_map->wbc.buffer_offset = BCMLTM_WB_OFFSET_INVALID;
        to_field_map->wbc.maxbit = 0;
        to_field_map->wbc.minbit = 0;
        to_field_map->next_map = NULL;
    }

 exit:
    SHR_FREE(maximum_values);
    SHR_FREE(minimum_values);
    SHR_FREE(default_values);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add transform field maps metadata for given list type.
 *
 * This routine adds the API cache field maps metadata from
 * the given transforms list for the specified field list criteria.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] list Field map list to add field maps to.
 * \param [in] fspec Specifies type of field list to add.
 * \param [in] apic_arg API cache arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
apic_xfrm_field_list_add(int unit,
                         bcmlrd_sid_t sid,
                         bcmltm_field_map_list_t *list,
                         const bcmltm_db_apic_field_spec_t *fspec,
                         const bcmltm_db_dm_apic_arg_t *apic_arg)
{
    const bcmltm_db_dm_xfrm_list_t *xfrm_list = NULL;
    const bcmltm_db_dm_xfrm_t *xfrm = NULL;
    const bcmltm_lta_select_field_list_t *from_list = NULL;
    uint32_t select_map_index;
    bcmltm_field_type_t field_type;
    bcmltm_field_key_type_t key_type;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    if (apic_arg->xfrms == NULL) {
        /* No fields, nothing to do */
        SHR_EXIT();
    }

    select_map_index = fspec->select_map_index;
    field_type = fspec->field_type;
    key_type = fspec->key_type;

    /*
     * Use the field lists of reverse transforms.
     *
     * The usage of the reverse transforms results
     * in creating lists that contains all fields including
     * the read-only fields.  This is intentional since
     * there is only one metadata list used for both directions,
     * input and output, and therefore needs to have all fields.
     *
     * Note that this works on the established rule that
     * the input API list should not contain read-only fields.
     * This condition should be checked and flagged by the upper layer
     * component.
     */
    if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        xfrm_list = apic_arg->xfrms->rev_keys;
    } else {
        xfrm_list = apic_arg->xfrms->rev_values;
    }

    if (xfrm_list == NULL) {
        /* No transforms, nothing to do */
        SHR_EXIT();
    }

    /* Process each transform in list */
    for (idx = 0; idx < xfrm_list->num_xfrms; idx++) {
        xfrm = xfrm_list->xfrms[idx];

        /* Get the logical field list of the transform */
        from_list = xfrm->out_fields;

        /* Check for matching select map index */
        if (from_list->select_map_index != select_map_index) {
            /* Select map index does not match, skip transform */
            continue;
        }

        /* Do further checks on key field type */
        if (field_type == BCMLTM_FIELD_TYPE_KEY) {
            /*
             * Skip key transform if:
             * - Request is required keys AND transform has allocatable keys
             * - Request is allocatable keys AND transform has required keys
             */
            if ((key_type == BCMLTM_FIELD_KEY_TYPE_IWA_REQ) &&
                (xfrm->is_alloc_keys)) {
                continue;
            } else if ((key_type == BCMLTM_FIELD_KEY_TYPE_IWA_ALLOC) &&
                       (!xfrm->is_alloc_keys)) {
                continue;
            }
        }

        /* Add field maps in list */
        SHR_IF_ERR_EXIT
            (apic_xfrm_field_maps_add(unit, sid, list, from_list));
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create API cache field maps metadata.
 *
 * This routine creates the array of field maps metadata
 * for the specified field list type.
 *
 * The field maps metadata is populated differently based
 * on the specified field list type.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] list Field map list to add created field maps to.
 * \param [in] fspec Specifies type of field list to add.
 * \param [in] apic_arg API cache arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
apic_field_maps_create(int unit,
                       bcmlrd_sid_t sid,
                       bcmltm_field_map_list_t *list,
                       const bcmltm_db_apic_field_spec_t *fspec,
                       const bcmltm_db_dm_apic_arg_t *apic_arg)
{
    bcmltm_field_map_t *field_maps = NULL;
    uint32_t max_fields;
    uint32_t max_field_elems;
    unsigned int alloc_size;

    SHR_FUNC_ENTER(unit);

    list->num_fields = 0;
    list->field_maps = NULL;

    /* Get max count of field IDs and field elements (fid,idx) */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_count_get(unit, sid,
                                   &max_fields, &max_field_elems));
    if (max_field_elems == 0) {
        SHR_EXIT();
    }

    /* Allocate field maps array */
    alloc_size = sizeof(*field_maps) * max_field_elems;
    SHR_ALLOC(list->field_maps, alloc_size, "bcmltmDmFieldMaps");
    SHR_NULL_CHECK(list->field_maps, SHR_E_MEMORY);
    sal_memset(list->field_maps, 0, alloc_size);

    /* Add fields in direct field maps */
    SHR_IF_ERR_EXIT
        (apic_direct_field_list_add(unit, sid,
                                    list, fspec, apic_arg));


    /* Add fields in transforms */
    SHR_IF_ERR_EXIT
        (apic_xfrm_field_list_add(unit, sid,
                                  list, fspec, apic_arg));

    /* Resize field maps array */
    SHR_IF_ERR_EXIT
        (apic_field_maps_resize(unit, max_field_elems, list));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the field map list metadata.
 *
 * This routine creates the direct field map list metadata for
 * the given map entry list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fspec Specifies type of field list to create.
 * \param [in] apic_arg API cache arguments.
 * \param [out] field_list_ptr Returning pointer to field map list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
apic_field_map_list_create(int unit,
                           bcmlrd_sid_t sid,
                           const bcmltm_db_apic_field_spec_t *fspec,
                           const bcmltm_db_dm_apic_arg_t *apic_arg,
                           bcmltm_field_map_list_t **field_list_ptr)
{
    bcmltm_field_map_list_t *field_list = NULL;
    const bcmltm_db_dm_arg_t *dm_arg;
    bcmltm_table_type_t table_type;
    bcmltm_field_type_t field_type;
    bcmltm_field_key_type_t key_type;
    uint32_t index_absent_offset = BCMLTM_WB_OFFSET_INVALID;

    SHR_FUNC_ENTER(unit);

    dm_arg = apic_arg->dm_arg;
    table_type = dm_arg->attr->type;
    field_type = fspec->field_type;
    key_type = fspec->key_type;

    /*
     * This metadata object should always be created even if
     * the field count is zero.  This is expected by the corresponding
     * FA node function logic.
     */

    /* Allocate */
    SHR_ALLOC(field_list, sizeof(*field_list), "bcmltmDmFieldMapList");
    SHR_NULL_CHECK(field_list, SHR_E_MEMORY);
    sal_memset(field_list, 0, sizeof(*field_list));

    /*
     * Get index absent offset.
     * Valid only for Index with Allocation LT and
     * on allocatable key fields.
     */
    if (BCMLTM_TABLE_TYPE_IS_ALLOC_INDEX(table_type) &&
        (field_type == BCMLTM_FIELD_TYPE_KEY) &&
        (key_type == BCMLTM_FIELD_KEY_TYPE_IWA_ALLOC)) {
        index_absent_offset = dm_arg->lt_pvt_wb_offsets->index_absent;
    }

    /* Fill data */
    field_list->index_absent_offset = index_absent_offset;

    /* Create field maps */
    SHR_IF_ERR_EXIT
        (apic_field_maps_create(unit, sid,
                                field_list, fspec, apic_arg));

    *field_list_ptr = field_list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_field_map_list_destroy(field_list);
        *field_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the field mapping metadata.
 *
 * Create the field mapping metadata for given field selection group
 * and field type.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] apic_arg API cache arguments.
 * \param [in] selection Field selection group.
 * \param [in] field_type Field type.
 * \param [in] key_type Key field type.
 * \param [out] field_mapping_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
apic_field_mapping_create(int unit,
                          bcmlrd_sid_t sid,
                          const bcmltm_db_dm_apic_arg_t *apic_arg,
                          const bcmltm_db_field_selection_t *selection,
                          bcmltm_field_type_t field_type,
                          bcmltm_field_key_type_t key_type,
                          bcmltm_field_select_mapping_t **field_mapping_ptr)
{
    bcmltm_field_select_mapping_t *field_mapping = NULL;
    const bcmltm_db_field_selection_map_t *selection_map = NULL;
    bcmltm_db_apic_field_spec_t fspec;
    uint32_t num_maps;
    uint32_t alloc_size;
    uint32_t map_index_offset = BCMLTM_WB_OFFSET_INVALID;
    uint32_t map_index;
    uint32_t fm_idx;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    num_maps = selection->num_maps;

    /* Allocate */
    alloc_size = sizeof(*field_mapping) +
        sizeof(bcmltm_field_map_list_t *) * num_maps;
    SHR_ALLOC(field_mapping, alloc_size, "bcmltmDmFieldSelectMapping");
    SHR_NULL_CHECK(field_mapping, SHR_E_MEMORY);
    sal_memset(field_mapping, 0, alloc_size);

    /* Get selection map index offset */
    if (selection->selection_id != BCMLTM_FIELD_SELECTION_ID_UNCOND) {
        SHR_IF_ERR_EXIT
            (bcmltm_wb_lt_fs_map_index_offset_get_by_sid(unit, sid,
                                              selection->selection_id,
                                              &map_index_offset));
    }

    /*
     * Fill data
     *
     * Note:
     * PT suppress is not applicable when this metadata
     * is used for copying fields API <-> API Cache.
     *
     * The PT suppress status is handled in the phase
     *   API Cache <-> [XFRM] <-> PTM
     */
    field_mapping->pt_suppress_offset = BCMLTM_WB_OFFSET_INVALID;
    field_mapping->map_index_offset = map_index_offset;
    field_mapping->num_maps = num_maps;

    /* Create field list for each select map in the selection group */
    fspec.field_type = field_type;
    fspec.key_type = key_type;
    for (i = 0; i < num_maps; i++) {
        selection_map = selection->maps[i];

        map_index = selection_map->map_index;
        if (map_index > num_maps) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        fspec.select_map_index = map_index;

        fm_idx = map_index - 1;
        SHR_IF_ERR_EXIT
            (apic_field_map_list_create(unit, sid,
                                        &fspec, apic_arg,
                        &(field_mapping->field_map_list[fm_idx])));
    }

    *field_mapping_ptr = field_mapping;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_field_mapping_destroy(field_mapping);
        *field_mapping_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Add API cache field maps metadata from field map list.
 *
 * This is a generic routine that adds the API cache field maps metadata
 * from the given source field map list.  A field map is added to the list
 * only if this is a new field element (id, idx).
 *
 * Assumption: array in field list is large enough to hold
 * all possible logical field elements, there is no array size check.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] list Field map list to add field maps to.
 * \param [in] from_list Source field map list to copy from.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
apic_field_maps_add(int unit,
                    bcmlrd_sid_t sid,
                    bcmltm_field_map_list_t *list,
                    const bcmltm_field_map_list_t *from_list)
{
    const bcmltm_field_map_t *from_field_map = NULL;
    bcmltm_field_map_t *to_field_map = NULL;
    uint32_t from_num_fields;
    uint32_t from_idx;
    uint32_t field_id;
    uint32_t field_idx;

    SHR_FUNC_ENTER(unit);

    if (from_list == NULL) {
        /* No field maps, nothing to do */
        SHR_EXIT();
    }

    /*
     * Copy field maps
     *
     * It is a requirement for LTM to supply output fields
     * with the field indexes in order.  Since the resulting list
     * can be used by LTM to return the output fields, this list
     * must follow this rule.
     *
     * This logic assumes that the field indexes in the source list
     * are already in order, so there is no need to check or sort the list.
     */
    from_num_fields = from_list->num_fields;
    for (from_idx = 0; from_idx < from_num_fields; from_idx++) {
        from_field_map = &from_list->field_maps[from_idx];

        field_id = from_field_map->field_id;
        field_idx = from_field_map->field_idx;

        /* Skip if field element is already in list */
        if (apic_field_find(list, field_id, field_idx) != NULL) {
            continue;
        }

        /* Add new field map to list */
        to_field_map = &list->field_maps[list->num_fields];
        list->num_fields++;

        /* Copy data */
        sal_memcpy(to_field_map, from_field_map, sizeof(*to_field_map));
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create API cache field maps metadata for all value fields.
 *
 * This routine creates the array of API cache field maps metadata
 * for all the value fields, regardless of the selection group.
 *
 * It uses the provided list of API cache fields information
 * (created per field selection group), and adds all the maps
 * into a single master list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] list Field map list to add created field maps to.
 * \param [in] apic_info_list List of API cache fields information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
apic_all_value_field_maps_create(int unit,
                                 bcmlrd_sid_t sid,
                                 bcmltm_field_map_list_t *list,
         const bcmltm_db_dm_apic_field_info_list_t *apic_info_list)
{
    bcmltm_field_map_t *field_maps = NULL;
    uint32_t max_fields;
    uint32_t max_field_elems;
    unsigned int alloc_size;
    const bcmltm_db_dm_apic_field_info_t *apic_info = NULL;
    const bcmltm_field_select_mapping_t *from_mapping = NULL;
    const bcmltm_field_map_list_t *from_list = NULL;
    uint32_t i;
    uint32_t map_idx;

    SHR_FUNC_ENTER(unit);

    list->num_fields = 0;
    list->field_maps = NULL;

    /* Get max count of field IDs and field elements (fid,idx) */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_count_get(unit, sid,
                                   &max_fields, &max_field_elems));
    if (max_field_elems == 0) {
        SHR_EXIT();
    }

    /* Allocate field maps array */
    alloc_size = sizeof(*field_maps) * max_field_elems;
    SHR_ALLOC(list->field_maps, alloc_size, "bcmltmDmFieldMaps");
    SHR_NULL_CHECK(list->field_maps, SHR_E_MEMORY);
    sal_memset(list->field_maps, 0, alloc_size);

    /* Add field maps from each API cache value field information group */
    for (i = 0; i < apic_info_list->num_apics; i++) {
        apic_info = apic_info_list->apics[i];

        if ((apic_info == NULL) || (apic_info->values == NULL)) {
            continue;
        }

        /* Add field maps from each map view */
        from_mapping = apic_info->values;
        for (map_idx = 0; map_idx < from_mapping->num_maps; map_idx++) {
            from_list = from_mapping->field_map_list[map_idx];
            SHR_IF_ERR_EXIT
                (apic_field_maps_add(unit, sid, list, from_list));

        }
    }

    /* Resize field maps array */
    SHR_IF_ERR_EXIT
        (apic_field_maps_resize(unit, max_field_elems, list));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create API cache field map list metadata for all value fields.
 *
 * This routine creates the API cache field map list metadata for
 * for all the value fields, regardless of the selection group.
 *
 * It uses the provided list of API cache fields information
 * (created per field selection group), and adds all the maps
 * into a single master list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] apic_info_list List of API cache fields information.
 * \param [out] field_list_ptr Returning pointer to field map list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
apic_all_value_field_map_list_create(int unit,
                                     bcmlrd_sid_t sid,
                  const bcmltm_db_dm_apic_field_info_list_t *apic_info_list,
                                     bcmltm_field_map_list_t **field_list_ptr)
{
    bcmltm_field_map_list_t *field_list = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * This metadata object should always be created even if
     * the field count is zero.  This is expected by the corresponding
     * FA node function logic.
     */

    /* Allocate */
    SHR_ALLOC(field_list, sizeof(*field_list), "bcmltmDmFieldMapList");
    SHR_NULL_CHECK(field_list, SHR_E_MEMORY);
    sal_memset(field_list, 0, sizeof(*field_list));

    /* Fill data */
    field_list->index_absent_offset = BCMLTM_WB_OFFSET_INVALID;

    /* Create field maps */
    SHR_IF_ERR_EXIT
        (apic_all_value_field_maps_create(unit, sid,
                                          field_list, apic_info_list));

    *field_list_ptr = field_list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_field_map_list_destroy(field_list);
        *field_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_dm_apic_field_info_create(int unit,
                              bcmlrd_sid_t sid,
                              const bcmltm_db_dm_apic_arg_t *apic_arg,
                              const bcmltm_db_field_selection_t *selection,
                              bcmltm_db_dm_apic_field_info_t **info_ptr)
{
    bcmltm_db_dm_apic_field_info_t *info = NULL;
    bcmltm_table_type_t table_type;

    SHR_FUNC_ENTER(unit);

    *info_ptr = NULL;

    /* Allocate info struct */
    SHR_ALLOC(info, sizeof(*info), "bcmltmDbDmFieldInfo");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    /* Get table type */
    table_type = apic_arg->dm_arg->attr->type;

    /* Create Key Fields */
    SHR_IF_ERR_EXIT
        (apic_field_mapping_create(unit, sid, apic_arg,
                                   selection,
                                   BCMLTM_FIELD_TYPE_KEY,
                                   BCMLTM_FIELD_KEY_TYPE_ALL,
                                   &info->keys));

    /* Create Required and Allocatable Key Fields for IwA LTs */
    if (BCMLTM_TABLE_TYPE_IS_ALLOC_INDEX(table_type)) {
        SHR_IF_ERR_EXIT
            (apic_field_mapping_create(unit, sid, apic_arg,
                                       selection,
                                       BCMLTM_FIELD_TYPE_KEY,
                                       BCMLTM_FIELD_KEY_TYPE_IWA_REQ,
                                       &info->req_keys));

        SHR_IF_ERR_EXIT
            (apic_field_mapping_create(unit, sid, apic_arg,
                                       selection,
                                       BCMLTM_FIELD_TYPE_KEY,
                                       BCMLTM_FIELD_KEY_TYPE_IWA_ALLOC,
                                       &info->alloc_keys));
    }

    /* Create Value Fields */
    SHR_IF_ERR_EXIT
        (apic_field_mapping_create(unit, sid, apic_arg,
                                   selection,
                                   BCMLTM_FIELD_TYPE_VALUE,
                                   0, /* N/A */
                                   &info->values));

    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_dm_apic_field_info_destroy(info);
        *info_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_db_dm_apic_field_info_destroy(bcmltm_db_dm_apic_field_info_t *info)
{
    if (info == NULL) {
        return;
    }

    /* Destroy Field Mappings */
    bcmltm_db_field_mapping_destroy(info->values);
    bcmltm_db_field_mapping_destroy(info->alloc_keys);
    bcmltm_db_field_mapping_destroy(info->req_keys);
    bcmltm_db_field_mapping_destroy(info->keys);

    SHR_FREE(info);

    return;
}

int
bcmltm_db_dm_apic_all_value_field_mapping_create(int unit,
                                                 bcmlrd_sid_t sid,
                   const bcmltm_db_dm_apic_field_info_list_t *apic_info_list,
                   bcmltm_field_select_mapping_t **field_mapping_ptr)
{
    bcmltm_field_select_mapping_t *field_mapping = NULL;
    uint32_t num_maps;
    uint32_t alloc_size;

    SHR_FUNC_ENTER(unit);

    /*
     * This metadata contains all the value fields,
     * regardless of whether the field is part of a selection group or not.
     *
     * As such, it contains only 1 map with an invalid
     * map index offset (i.e. unconditional).
     */
    num_maps = 1;

    /* Allocate */
    alloc_size = sizeof(*field_mapping) +
        sizeof(bcmltm_field_map_list_t *) * num_maps;
    SHR_ALLOC(field_mapping, alloc_size, "bcmltmDmFieldSelectMapping");
    SHR_NULL_CHECK(field_mapping, SHR_E_MEMORY);
    sal_memset(field_mapping, 0, alloc_size);

    /*
     * Fill data
     */
    field_mapping->pt_suppress_offset = BCMLTM_WB_OFFSET_INVALID;
    field_mapping->map_index_offset = BCMLTM_WB_OFFSET_INVALID;
    field_mapping->num_maps = num_maps;

    /* Create field map list */
    SHR_IF_ERR_EXIT
        (apic_all_value_field_map_list_create(unit, sid,
                                              apic_info_list,
                                &field_mapping->field_map_list[0]));

    *field_mapping_ptr = field_mapping;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_field_mapping_destroy(field_mapping);
        *field_mapping_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}
