/*! \file bcmltm_db_table.c
 *
 * Logical Table Manager - Logical Table Information.
 *
 * This file contains interfaces to provide information on
 * logical tables tailored for the TABLE LTs.  However,
 * these interfaces can be used of other purposes as well.
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

#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_enum.h>
#include <bcmlrd/bcmlrd_client.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_db_internal.h>
#include <bcmltm/bcmltm_db_table_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>

#include "bcmltm_db_map.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the number of values for a given logical field.
 *
 * This routine gets the number of values for the specified logical
 * table and field ID.
 *
 * If the field is an enumeration, the count is retrieved from the
 * the LRD enum data.
 *
 * Otherwise, the count is calculated using the minimum and maximum
 * field values with the assumption that the field values are sequential.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Logical field ID.
 * \param [out] count Field values count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_values_count_get(int unit,
                       bcmlrd_sid_t sid,
                       bcmlrd_fid_t fid,
                       uint32_t *count)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_field_data_t *field_data = NULL;
    uint64_t minimum_value;
    uint64_t maximum_value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    if (fid >= map->field_data->fields) {
        const char *table_name;
        const char *field_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "LTM: Invalid field ID"
                              "%s(ltid=%d) %s(fid=%d)\n"),
                   table_name, sid, field_name, fid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    field_data = &map->field_data->field[fid];

    /* Determine if field is enum */
    if (field_data->flags & BCMLRD_FIELD_F_ENUM) {
        /* Sanity check */
        if (field_data->edata == NULL) {
            const char *table_name;
            const char *field_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM: Unexpected null enum data"
                                  "%s(ltid=%d) %s(fid=%d)\n"),
                       table_name, sid, field_name, fid));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        *count = field_data->edata->num_sym;
    } else {
        /* Assume sequential values for non-enumeration field */
        SHR_IF_ERR_EXIT
            (bcmltm_db_field_scalar_values_get(unit, sid, fid,
                                               &minimum_value,
                                               &maximum_value,
                                               NULL));

        *count = (maximum_value - minimum_value + 1);
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get maximum number of entries based on logical key fields.
 *
 * This routine gets the maximum number of entries which may be installed for
 * the specified logical table based on its logical key field limits.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [out] entry_maximum Maximum number of entries.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
table_entry_maximum_logical_get(int unit, bcmlrd_sid_t sid,
                                uint32_t *entry_maximum)
{
    uint32_t num_fid;
    uint32_t num_fid_idx;
    uint32_t fid;
    uint32_t fid_count;
    uint32_t idx_count;
    unsigned int size;
    bcmltm_fid_idx_t *fid_idx = NULL;
    uint32_t values_count = 0;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    *entry_maximum = 1;

    /* Get count of key field IDs and field elements (fid,idx) */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_key_count_get(unit, sid, &num_fid, &num_fid_idx));
    if (num_fid_idx == 0) {
        SHR_EXIT();
    }

    /* Allocate field id/idx array */
    size = sizeof(*fid_idx) * num_fid;
    SHR_ALLOC(fid_idx, size, "bcmltmKeyFidIdxArr");
    SHR_NULL_CHECK(fid_idx, SHR_E_MEMORY);
    sal_memset(fid_idx, 0, size);

    /* Get list of key field IDs and index count */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_key_list_get(unit, sid,
                                      num_fid, fid_idx, &fid_count));

    /* Get minimum and maximum logical field limits */
    for (i = 0; i < fid_count; i++) {
        fid = fid_idx[i].fid;
        idx_count = fid_idx[i].idx_count;

        /*
         * Sanity check: index count must be 1 for mapped key fields.
         *
         * Key fields for Index LTs can only be scalars (wide fields
         * and field arrays are not allowed).
         *
         * This limitation is due to the following:
         * - PT keys in Index LTs must fit within a 32-bit value.
         *   (this is the size allocated in the PTM WB for keys).
         *   Since logical key fields may be directly mapped to PT keys,
         *   this size restriction reduces the chances of a logical
         *   key being too large to fit in the PTM WB.
         * - The notion of a key field array does not fit the definition
         *   of an array.  An array implies that elements can be absent
         *   during input.  However, key fields are mandatory (with the
         *   exception of IwA LTs).
         */
        
        if (idx_count != 1) {
            const char *table_name;
            const char *field_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM: Invalid index count for key field "
                                  "%s(ltid=%d) %s(fid=%d) "
                                  "expected=1 actual=%d\n"),
                       table_name, sid, field_name, fid, idx_count));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_EXIT
            (field_values_count_get(unit, sid, fid, &values_count));

        *entry_maximum *= values_count;
    }

 exit:
    SHR_FREE(fid_idx);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get maximum number of entries for a logical table.
 *
 * This routine gets the maximum number of entries which may be installed for
 * the specified logical table.
 *
 * In order to support this maximum size, reconfiguration of resources
 * such as hash banks, UFT, TCAM banks, etc., may be required (this is
 * the case for resizable tables, as described below).
 *
 * The entry limit (TABLE_INFO.ENTRY_LIMIT) for a table reflects
 * the greatest number of entries under the current table configuration.
 * This entry limit may be smaller than this entry_maximum.
 * This is the case for tables where the resource limit can be
 * resized by some reconfiguration.
 *
 * For fixed sizes tables, the entry limit is the same
 * as this entry_maximum.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] attr Table attributes.
 * \param [out] entry_maximum Maximum number of logical entries.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
table_entry_maximum_get(int unit, bcmlrd_sid_t sid,
                        const bcmltm_table_attr_t *attr,
                        uint32_t *entry_maximum)
{
    int rv;
    bcmltm_table_type_t table_type = attr->type;

    SHR_FUNC_ENTER(unit);

    *entry_maximum = 0;

    /*
     * Maximum number of logical entries is calculated as follows:
     *
     * 1) If LRD attribute entry_limit is present, use attr value.
     *
     * 2) Else, use default logic:
     *    - Index tables: use the logical field limits.
     *    - Keyed tables: use the PT (or destination) limits.
     */
    rv = bcmlrd_map_table_attr_get(unit, sid,
                                   BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT,
                                   entry_maximum);
    if (SHR_FAILURE(rv)) {
        if (rv != SHR_E_UNAVAIL) {
            SHR_ERR_EXIT(rv);
        }

        /* Attribute not available, use default logic */
        if (BCMLTM_TABLE_TYPE_IS_INDEX(table_type)) {
            SHR_IF_ERR_EXIT
                (table_entry_maximum_logical_get(unit, sid, entry_maximum));
        } else if (BCMLTM_TABLE_TYPE_IS_KEYED(table_type)) {
            *entry_maximum = BCMLTM_KEYED_LT_ENTRY_MAX;
        } else {
            const char *table_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM: Invalid table type for entry maximum "
                                  "%s(ltid=%d) type=%d\n"),
                       table_name, sid, table_type));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the field minimum, maximum, and default values.
 *
 * This routine gets the minimum, maximum, and default values for
 * the specified logical field.
 *
 * This only applies for narrow scalar and array fields.
 * On wide fields (greater than 64-bit), the values are set to zero.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Logical field ID.
 * \param [in] width Field width.
 * \param [out] min Minimum value.
 * \param [out] max Maximum value.
 * \param [out] default_value Default value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_values_get(int unit, bcmlrd_sid_t sid,
                 bcmlrd_fid_t fid, uint32_t width,
                 uint64_t *min, uint64_t *max, uint64_t *default_value)
{
    SHR_FUNC_ENTER(unit);

    *min = *max = *default_value = 0;

    if (width <= 64) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_field_scalar_values_get(unit, sid, fid,
                                               min, max, default_value));
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get corresponding enum value in TABLE_FIELD_INFO.SYMBOL_DEFAULT.
 *
 * This routine gets the corresponding enum value for the type
 * specified for TABLE_FIELD_INFO.SYMBOL_DEFAULT.
 *
 * The field TABLE_FIELD_INFO.SYMBOL_DEFAULT is an enum that is
 * composed of all possible enum symbols that can comprise all
 * LTL enum type default values.
 *
 * Since the enum value assigned to TABLE_FIELD_INFO.SYMBOL_DEFAULT
 * may not be the same as the value declared in the target table
 * enum file, the following conversion process needs to take place:
 *
 *   target-enum-value
 *       -> enum-symbol
 *            -> TABLE_FIELD_INFO.SYMBOL_DEFAULT-enum-value
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] fid Logical field ID.
 * \param [in] local_enum Local enum value.
 * \param [out] symbol_default_enum TABLE_FIELD_INFO.SYMBOL_DEFAULT enum value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_info_symbol_default_enum_get(int unit,
                                   bcmlrd_sid_t sid,
                                   bcmlrd_fid_t fid,
                                   uint64_t local_enum,
                                   uint64_t *symbol_default_enum)
{
    const char *sym = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get the enum symbol corresponding to the target enum value */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit,
                                      sid,
                                      fid,
                                      local_enum,
                                      &sym));

    /*
     * Get the enum value assigned in TABLE_FIELD_INFO.SYMBOL_DEFAULT
     * that corresponds to the symbol.
     */
    SHR_IF_ERR_EXIT
        (bcmlrd_resolve_enum(unit,
                             TABLE_FIELD_INFOt,
                             TABLE_FIELD_INFOt_SYMBOL_DEFAULTf,
                             sym,
                             symbol_default_enum));

 exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_table_info_get(int unit, bcmlrd_sid_t sid,
                         bcmltm_table_info_t *info)
{
    const bcmltm_table_attr_t *attr;
    uint32_t num_fields;
    uint32_t num_keys;
    uint32_t num_fid_idx;
    uint32_t entry_maximum;
    size_t num_resource_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmltm_db_table_attr_get(unit, sid, &attr));

    SHR_IF_ERR_EXIT
        (bcmltm_db_field_count_get(unit, sid, &num_fields, &num_fid_idx));
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_key_count_get(unit, sid, &num_keys, &num_fid_idx));
    SHR_IF_ERR_EXIT
        (table_entry_maximum_get(unit, sid, attr, &entry_maximum));
    SHR_IF_ERR_EXIT
        (bcmlrd_map_table_attr_count_get(unit,
                                     sid,
                                     BCMLRD_MAP_TABLE_ATTRIBUTE_RESOURCE_INFO,
                                     &num_resource_info));

    info->type = attr->type;
    info->map_type = attr->map_type;
    info->mode = attr->mode;
    info->read_only = !!(attr->flag & BCMLTM_TABLE_FLAG_READ_ONLY);
    info->num_keys = num_keys;
    info->num_fields = num_fields;
    info->entry_maximum = entry_maximum;
    info->num_resource_info = num_resource_info;
    info->opcodes = attr->opcodes;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_table_resource_info_get(int unit,
                                  bcmlrd_sid_t sid,
                                  uint32_t idx,
                                  bcmltm_table_resource_info_t *info)
{
    size_t num_resource_info;
    uint32_t attr_value;

    SHR_FUNC_ENTER(unit);

    sal_memset(info, 0, sizeof(*info));

    SHR_IF_ERR_EXIT
        (bcmlrd_map_table_attr_count_get(unit,
                                     sid,
                                     BCMLRD_MAP_TABLE_ATTRIBUTE_RESOURCE_INFO,
                                     &num_resource_info));
    if (idx >= num_resource_info) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_map_table_attr_get_by_idx(unit,
                                      sid,
                                      BCMLRD_MAP_TABLE_ATTRIBUTE_RESOURCE_INFO,
                                      idx,
                                      &attr_value));
    info->ri_sid = attr_value;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_table_field_info_get(int unit, bcmlrd_sid_t sid,
                               bcmlrd_fid_t fid,
                               bcmltm_table_field_info_t *info)
{
    bcmlrd_field_def_t field_def;
    uint64_t min, max, default_value;
    bool is_selector;
    uint32_t select_group_count;
    bool is_read_only = FALSE;

    SHR_FUNC_ENTER(unit);

    sal_memset(info, 0, sizeof(*info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_def_get(unit, sid, fid, &field_def));

    SHR_IF_ERR_EXIT
        (field_values_get(unit, sid, fid, field_def.width,
                          &min, &max, &default_value));

    /*
     * If field is enum, the target table enum value
     * needs to be converted to the corresponding enum value
     * assigned in TABLE_FIELD_INFO.SYMBOL_DEFAULT.
     */
    if (field_def.symbol) {
        SHR_IF_ERR_EXIT
            (field_info_symbol_default_enum_get(unit, sid, fid,
                                                default_value,
                                                &default_value));
    }

    SHR_IF_ERR_EXIT
        (bcmltm_db_field_is_read_only(unit, sid, fid, &is_read_only));

    SHR_IF_ERR_EXIT
        (bcmltm_db_field_is_selector(unit, sid, fid, &is_selector));

    SHR_IF_ERR_EXIT
        (bcmltm_db_field_select_group_count_get(unit, sid, fid,
                                                &select_group_count));

    info->min_limit = min;
    info->max_limit = max;
    info->default_value = default_value;
    info->key = field_def.key;
    info->alloc_key_field = bcmltm_db_field_key_is_alloc(unit, sid, fid);
    info->read_only = is_read_only;
    info->symbol = field_def.symbol;
    info->array_depth = field_def.depth;
    info->elements = field_def.elements;
    info->width = field_def.width;
    info->selector = is_selector;
    info->num_select_group = select_group_count;

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_db_table_field_info_get_by_gfid(int unit, bcmlrd_sid_t sid,
                                       bcmltd_gfid_t gfid,
                                       bcmltm_table_field_info_t *info)
{
    bcmlrd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    /* Convert global to local field ID */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmltm_db_gfid_to_fid(unit, sid, gfid, &fid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmltm_db_table_field_info_get(unit, sid, fid, info));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_table_field_select_get(int unit, bcmlrd_sid_t sid,
                                 bcmlrd_fid_t fid,
                                 uint32_t group,
                                 bcmltm_table_field_select_t *info)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_field_selector_data_t *select_data = NULL;
    const bcmlrd_field_selector_t *field_select = NULL;
    bcmltd_fid_t selector_fid;
    bcmltd_gfid_t selector_gfid;
    uint32_t i;
    bool is_virtual;
    bool is_enum;

    SHR_FUNC_ENTER(unit);

    sal_memset(info, 0, sizeof(*info));

    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    select_data = map->sel;
    if (select_data == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    for (i = 0; i < select_data->num_field_selector; i++) {
        field_select = &select_data->field_selector[i];

        /* Match field ID and group number */
        if ((field_select->field_id == fid) &&
            (field_select->group == group)) {

            selector_fid = field_select->selector_id;

            /* Check if field selector is a virtual field */
            SHR_IF_ERR_EXIT
                (bcmltm_db_field_is_virtual(unit, selector_fid, &is_virtual));

            if (is_virtual) {
                /*
                 * Skip virtual field selector.
                 *
                 * A virtual field selector does not select which fields
                 * are valid or not.  It is use to select where
                 * logical fields map to.  As such, this type of
                 * selector is ignored.
                 */
                continue;
            }

            /*
             * Not virtual: field must be logical since a selector cannot
             * be a HW field.
             */

            SHR_IF_ERR_EXIT
                (bcmltm_db_fid_to_gfid(unit, sid, selector_fid, &selector_gfid));

            SHR_IF_ERR_EXIT
                (bcmltm_db_field_is_enum(unit, sid, selector_fid,
                                         &is_enum));

            info->selector_fid = selector_fid;
            info->selector_gfid = selector_gfid;
            if (is_enum) {
                info->selector_scalar = FALSE;

                SHR_IF_ERR_EXIT
                    (bcmlrd_find_global_enum(unit,
                                             sid,
                                             selector_fid,
                                             field_select->selector_value,
                                             &info->selector_enum_value));

            } else {
                info->selector_scalar = TRUE;
                info->selector_scalar_value = field_select->selector_value;
            }

            break;
        }
    }

    if (i >= select_data->num_field_selector) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_table_field_select_get_by_gfid(int unit, bcmlrd_sid_t sid,
                                         bcmltd_gfid_t gfid,
                                         uint32_t group,
                                         bcmltm_table_field_select_t *info)
{
    bcmlrd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    /* Convert global to local field ID */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmltm_db_gfid_to_fid(unit, sid, gfid, &fid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmltm_db_table_field_select_get(unit, sid, fid, group, info));

 exit:
    SHR_FUNC_EXIT();
}

