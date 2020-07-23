/*! \file bcmltm_db_ltm.c
 *
 * Logical Table Manager - Information for LTM Managed Tables.
 *
 * This file contains routines to construct information for
 * LTM managed tables, such as TABLE_ LTs.
 *
 * This information is derived from the LRD LTM Managed Table map groups.
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

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_wb_ltm_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>

#include "bcmltm_db_md_util.h"
#include "bcmltm_db_ltm.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/* LTM Key fields Min and Max bits (this should the Table ID) */
#define LTID_INDEX_MINBIT        0
#define LTID_INDEX_MAXBIT       31

/*!
 * \brief Field Map.
 *
 * Field map information.
 */
typedef struct ltm_map_field_s {
    /*! Field ID. */
    uint32_t fid;

    /*! The index of the field in an array. */
    uint32_t idx;

    /*! The minimum value for this field. */
    uint64_t minimum_value;

    /*! The maximum value for this field. */
    uint64_t maximum_value;
} ltm_map_field_t;

/*!
 * \brief LT map entries information for LTM managed map groups.
 *
 * This structure is used to hold information parsed from the LRD
 * map entries that are relevant for the LTM group.
 */
typedef struct ltm_map_entries_s {
    /*! Number of total key field elements (fid, idx). */
    uint32_t num_key_fields;

    /*! Array of fields. */
    ltm_map_field_t *key_field;

    /*! Number of total field elements (fid, idx). */
    uint32_t num_value_fields;

    /*! Array of fields. */
    ltm_map_field_t *value_field;

} ltm_map_entries_t;


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Cleanup LTM group map entries information.
 *
 * Free any internal memory allocated during the parsing of the
 * LTM group map entries.
 *
 * \param [in] lt_map LT mapping information to clean.
 */
static void
ltm_map_entries_cleanup(ltm_map_entries_t *lt_map)
{
    SHR_FREE(lt_map->key_field);
    SHR_FREE(lt_map->value_field);
    lt_map->key_field = NULL;
    lt_map->value_field = NULL;

    return;
}

/*!
 * \brief Get the LTM group mapping information for a given LT.
 *
 * Get the LTM group mapping information for a given table ID.
 *
 * This routine parses the LRD map group information for:
 *     Map Group Kind: BCMLRD_MAP_LTM
 *
 * This support table definitions with only only 1 LTM group.
 *
 * This routine allocates memory so caller must call corresponding routine
 * to free memory when this is no longer required.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] lt_map Returning LT mapping information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltm_map_entries_parse(int unit, bcmlrd_sid_t sid,
                      ltm_map_entries_t *lt_map)
{
    const bcmlrd_map_t *map = NULL;
    uint32_t num_fid;      /* Number of API facing fields (fid) */
    uint32_t num_fid_idx;  /* Total fields elements (fid, idx) */
    uint32_t fid_list_count;
    bcmltm_fid_idx_t *fid_list = NULL;
    ltm_map_field_t *key_field = NULL;
    uint32_t key_field_idx = 0 ;
    ltm_map_field_t *value_field = NULL;
    uint32_t value_field_idx = 0 ;
    unsigned int size;
    uint32_t i;
    uint32_t fid;
    uint32_t idx;
    uint32_t idx_count;
    uint64_t *minimum_value = NULL;
    uint32_t minimum_count;
    uint64_t *maximum_value = NULL;
    uint32_t maximum_count;
    bcmlrd_field_def_t field_def;


    SHR_FUNC_ENTER(unit);

    sal_memset(lt_map, 0, sizeof(*lt_map));

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    /* Get number of API facing LT fields and field elements(fid,idx) */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_count_get(unit, sid,
                                   &num_fid, &num_fid_idx));

    /* Get list of field IDs and index count */
    size = sizeof(*fid_list) * num_fid;
    if (size == 0) {
        /* No FIDs */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fid_list, size, "bcmltmLtmFidIdxArr");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, size);
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_list_get(unit, sid,
                                  num_fid, fid_list, &fid_list_count));

    /* Allocate field IDs array */
    size = sizeof(*key_field) * num_fid_idx;
    if (size == 0) {
        /* No FIDs */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(key_field, size, "bcmltmLtmKeyMapFields");
    SHR_NULL_CHECK(key_field, SHR_E_MEMORY);
    sal_memset(key_field, 0, size);

    size = sizeof(*value_field) * num_fid_idx;
    SHR_ALLOC(value_field, size, "bcmltmLtmValueMapFields");
    SHR_NULL_CHECK(value_field, SHR_E_MEMORY);
    sal_memset(value_field, 0, size);

    /* Get field map information */
    for (i = 0; i < fid_list_count; i++) {
        fid = fid_list[i].fid;
        idx_count = fid_list[i].idx_count;

        SHR_IF_ERR_EXIT
            (bcmlrd_table_field_def_get(unit, sid, fid, &field_def));

        /* All fields has at least 1 index count for mapped fields */
        if (idx_count < 1) {
            const char *table_name;
            const char *field_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "LTM group: invalid "
                                  "field index count (should be > 0) "
                                  "%s(ltid=%d) %s(fid=%d) count=%d\n"),
                       table_name, sid, field_name, fid, idx_count));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* Get SW minimum value for field */
        SHR_ALLOC(minimum_value, sizeof(*minimum_value) * idx_count,
                  "bcmltmLtmFieldMinVals");
        SHR_NULL_CHECK(minimum_value, SHR_E_MEMORY);
        sal_memset(minimum_value, 0, sizeof(*minimum_value) * idx_count);
        SHR_IF_ERR_EXIT
            (bcmlrd_field_min_get(unit, sid, fid,
                                  idx_count,
                                  minimum_value,
                                  &minimum_count));
        if (idx_count != minimum_count) {
            const char *table_name;
            const char *field_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Invalid index count for field minimums: "
                                  "%s(ltid=%d) %s(fid=%d) "
                                  "expected=%d actual=%d\n"),
                       table_name, sid, field_name, fid,
                       idx_count, minimum_count));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* Get SW maximum value for field */
        SHR_ALLOC(maximum_value, sizeof(*maximum_value) * idx_count,
                  "bcmltmLtmFieldMaxVals");
        SHR_NULL_CHECK(maximum_value, SHR_E_MEMORY);
        sal_memset(maximum_value, 0, sizeof(*maximum_value) * idx_count);
        SHR_IF_ERR_EXIT
            (bcmlrd_field_max_get(unit, sid, fid,
                                  idx_count,
                                  maximum_value,
                                  &maximum_count));
        if (idx_count != maximum_count) {
            const char *table_name;
            const char *field_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            field_name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Invalid index count for field maximums: "
                                  "%s(ltid=%d) %s(fid=%d) "
                                  "expected=%d actual=%d\n"),
                       table_name, sid, field_name, fid,
                       idx_count, maximum_count));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        for (idx = 0; idx < idx_count; idx++) {
            if (i >= num_fid_idx) {
                const char *table_name;

                table_name = bcmltm_lt_table_sid_to_name(unit, sid);
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "LTM group: current "
                                      "field count exceeds max count "
                                      "%s(ltid=%d) "
                                      "count=%d max_count=%d\n"),
                           table_name, sid, (i+1), num_fid_idx));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }

            if (field_def.key) {
                key_field[key_field_idx].fid = fid;
                key_field[key_field_idx].idx = idx;
                key_field[key_field_idx].minimum_value =
                    minimum_value[idx];
                key_field[key_field_idx].maximum_value =
                    maximum_value[idx];
                key_field_idx++;
            } else {
                value_field[value_field_idx].fid = fid;
                value_field[value_field_idx].idx = idx;
                value_field[value_field_idx].minimum_value =
                    minimum_value[idx];
                value_field[value_field_idx].maximum_value =
                    maximum_value[idx];
                value_field_idx++;
            }
        }

        SHR_FREE(maximum_value);
        SHR_FREE(minimum_value);
    }

    lt_map->key_field        = key_field;
    lt_map->num_key_fields   = key_field_idx;
    lt_map->value_field      = value_field;
    lt_map->num_value_fields = value_field_idx;

 exit:
    SHR_FREE(maximum_value);
    SHR_FREE(minimum_value);
    SHR_FREE(fid_list);

    if (SHR_FUNC_ERR()) {
        SHR_FREE(key_field);
        lt_map->key_field = NULL;

        SHR_FREE(value_field);
        lt_map->value_field = NULL;

        ltm_map_entries_cleanup(lt_map);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get working buffer offset for given LTM table key type.
 *
 * This function gets the working buffer offset for the specified
 * LTM table key type.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] key Table key type.
 * \param [in] offsets LTM working buffer key offsets.
 * \param [out] offset Returning working buffer offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltm_key_offset_get(int unit,
                   uint32_t sid,
                   bcmltm_table_keys_t key,
                   bcmltm_wb_ltm_offsets_t *offsets,
                   uint32_t *offset)
{
    bool valid = FALSE;

    SHR_FUNC_ENTER(unit);

    *offset = BCMLTM_WB_OFFSET_INVALID;

    switch (sid) {
    case TABLE_STATSt:
        switch (key) {
        case BCMLTM_TABLE_KEYS_LTID:
            valid = TRUE;
            break;
        default:
            break;
        }
        break;
    case TABLE_INFOt:
        switch (key) {
        case BCMLTM_TABLE_KEYS_LTID:
            valid = TRUE;
            break;
        default:
            break;
        }
        break;
    case TABLE_FIELD_INFOt:
        switch (key) {
        case BCMLTM_TABLE_KEYS_LTID:
        case BCMLTM_TABLE_KEYS_LT_FIELD_ID:
            valid = TRUE;
            break;
        default:
            break;
        }
        break;
    case TABLE_FIELD_SELECTt:
        switch (key) {
        case BCMLTM_TABLE_KEYS_LTID:
        case BCMLTM_TABLE_KEYS_LT_FIELD_ID:
        case BCMLTM_TABLE_KEYS_SELECT_GROUP:
            valid = TRUE;
            break;
        default:
            break;
        }
        break;
    case TABLE_CONTROLt:
        switch (key) {
        case BCMLTM_TABLE_KEYS_LTID:
            valid = TRUE;
            break;
        default:
            break;
        }
        break;
    case TABLE_RESOURCE_INFOt:
        switch (key) {
        case BCMLTM_TABLE_KEYS_LTID:
        case BCMLTM_TABLE_KEYS_RESOURCE_INFO:
            valid = TRUE;
            break;
        default:
            break;
        }
        break;
    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
        break;
    }

    if (valid) {
        *offset = offsets->key_value_offsets[key];
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Select the correct LTM key field offset for a given key field.
 *
 * Map the LTM tables key fields to the WB key field offset.
 *
 * \param [in] unit Unit number.
 * \param [in] sid LT ID for LTM internal table.
 * \param [in] fid Field ID for LTM internal table key.
 * \param [in] offsets LTM working buffer key values offsets.
 * \param [out] offset Returning pointer to LTM key field WB offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltm_key_field_to_offset(int unit,
                        uint32_t sid,
                        uint32_t fid,
                        bcmltm_wb_ltm_offsets_t *offsets,
                        uint32_t *offset)
{
    bcmltm_table_keys_t key = BCMLTM_TABLE_KEYS_COUNT;

    SHR_FUNC_ENTER(unit);

    *offset = BCMLTM_WB_OFFSET_INVALID;

    switch (sid) {
    case TABLE_STATSt:
        switch (fid) {
        case TABLE_STATSt_TABLE_IDf:
            key = BCMLTM_TABLE_KEYS_LTID;
            break;
        default:
            break;
        }
        break;
    case TABLE_INFOt:
        switch (fid) {
        case TABLE_INFOt_TABLE_IDf:
            key = BCMLTM_TABLE_KEYS_LTID;
            break;
        default:
            break;
        }
        break;
    case TABLE_FIELD_INFOt:
        switch (fid) {
        case TABLE_FIELD_INFOt_TABLE_IDf:
            key = BCMLTM_TABLE_KEYS_LTID;
            break;
        case TABLE_FIELD_INFOt_FIELD_IDf:
            key = BCMLTM_TABLE_KEYS_LT_FIELD_ID;
            break;
        default:
            break;
        }
        break;
    case TABLE_FIELD_SELECTt:
        switch (fid) {
        case TABLE_FIELD_SELECTt_TABLE_IDf:
            key = BCMLTM_TABLE_KEYS_LTID;
            break;
        case TABLE_FIELD_SELECTt_FIELD_IDf:
            key = BCMLTM_TABLE_KEYS_LT_FIELD_ID;
            break;
        case TABLE_FIELD_SELECTt_GROUPf:
            key = BCMLTM_TABLE_KEYS_SELECT_GROUP;
            break;
        default:
            break;
        }
        break;
    case TABLE_CONTROLt:
        switch (fid) {
        case TABLE_CONTROLt_TABLE_IDf:
            key = BCMLTM_TABLE_KEYS_LTID;
            break;
        default:
            break;
        }
        break;
    case TABLE_RESOURCE_INFOt:
        switch (fid) {
        case TABLE_RESOURCE_INFOt_TABLE_IDf:
            key = BCMLTM_TABLE_KEYS_LTID;
            break;
        case TABLE_RESOURCE_INFOt_RESOURCE_INFOf:
            key = BCMLTM_TABLE_KEYS_RESOURCE_INFO;
            break;
        default:
            break;
        }
        break;
    
    default:
        *offset = BCMLTM_WB_OFFSET_INVALID;
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (ltm_key_offset_get(unit, sid,
                            key, offsets, offset));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given FA LTM internal table metadata.
 *
 * Destroy the given FA LTM internal table metadata.
 *
 * \param [in] ltm FA LTM internal metadata to destroy.
 */
static void
fa_ltm_destroy(bcmltm_fa_ltm_t *ltm)
{
    SHR_FREE(ltm);

    return;
}

/*!
 * \brief Create the FA LTM internal metadata.
 *
 * Create the FA LTM key fields metadata for the given table.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] offsets LTM working buffer key values offsets.
 * \param [out] index_ptr Returning pointer FA LTM metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fa_ltm_create(int unit,
              uint32_t sid,
              bcmltm_wb_ltm_offsets_t *offsets,
              bcmltm_fa_ltm_t **ltm_ptr)
{
    bcmltm_fa_ltm_t *ltm = NULL;
    uint32_t koix;
    bcmltm_table_keys_t key;
    uint32_t key_offset;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(ltm, sizeof(*ltm), "bcmltmFaLtm");
    SHR_NULL_CHECK(ltm, SHR_E_MEMORY);
    sal_memset(ltm, 0, sizeof(*ltm));

    /* Fill data */
    for (koix = 0; koix < BCMLTM_TABLE_KEYS_COUNT; koix++) {
        key = koix;
        SHR_IF_ERR_EXIT
            (ltm_key_offset_get(unit, sid,
                                key, offsets, &key_offset));
        ltm->key_offset[koix] = key_offset;
    }

    *ltm_ptr = ltm;

 exit:
    if (SHR_FUNC_ERR()) {
        fa_ltm_destroy(ltm);
        *ltm_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the field map metadata for given map information.
 *
 * Get the field map metadata for given map information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid LT ID for LTM internal table.
 * \param [in] map_info Field map information from LRD.
 * \param [in] offset LT Index working buffer offset.
 * \param [out] field_map Field map to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
key_field_map_get(int unit,
                  uint32_t sid,
                  const ltm_map_field_t *map_info,
                  uint32_t offset,
                  bcmltm_field_map_t *field_map)
{
    uint32_t field_flags = 0x0;

    SHR_FUNC_ENTER(unit);

    /* Get field properties */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_properties_get(unit, sid, map_info->fid,
                                        &field_flags));

    field_map->field_id = map_info->fid;
    field_map->field_idx = map_info->idx;
    field_map->minimum_value = map_info->minimum_value;
    field_map->maximum_value = map_info->maximum_value;
    field_map->wbc.buffer_offset = offset;
    
    field_map->wbc.maxbit = LTID_INDEX_MAXBIT;
    field_map->wbc.minbit = LTID_INDEX_MINBIT;
    field_map->flags = field_flags;

    /* These members are not applicable for LTM based LTs */
    field_map->apic_offset = BCMLTM_WB_OFFSET_INVALID;
    field_map->pt_changed_offset = BCMLTM_WB_OFFSET_INVALID;
    field_map->default_value = 0;
    field_map->next_map = NULL;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the list of key field maps metadata.
 *
 * Create the list of key field maps metadata.
 *
 * \param [in] unit Unit number.
 * \param [in] sid LT ID for LTM internal table.
 * \param [in] map_info LRD Field mapping information list.
 * \param [in] offsets LTM working buffer key values offsets.
 * \param [out] field_maps_ptr Returning pointer to list of field maps.
 * \param [out] num_fields Number of field maps returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
key_field_maps_create(int unit,
                      uint32_t sid,
                      const ltm_map_entries_t *map_info,
                      bcmltm_wb_ltm_offsets_t *offsets,
                      bcmltm_field_map_t **field_maps_ptr,
                      uint32_t *num_fields)
{
    bcmltm_field_map_t *field_maps = NULL;
    bcmltm_field_map_t *field_map = NULL;
    unsigned int size;
    uint32_t offset;
    uint32_t field_idx;
    uint32_t field_max_count;

    SHR_FUNC_ENTER(unit);

    *num_fields = 0;
    *field_maps_ptr = NULL;

    /* Get total number of key fields */
    field_max_count = map_info->num_key_fields;

    /*
     * Sanity check:
     * LTM based table cannot have more than the number of defined
     * table key types.
     */
    if ((field_max_count < 1) || (field_max_count > BCMLTM_TABLE_KEYS_COUNT)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate field_map */
    size = sizeof(*field_maps) * field_max_count;
    SHR_ALLOC(field_maps, size, "bcmltmLtmFieldMaps");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, size);

    /*
     * Process field map information list
     */
    for (field_idx = 0; field_idx < field_max_count; field_idx++) {
        field_map = &field_maps[field_idx];
        SHR_IF_ERR_EXIT
            (ltm_key_field_to_offset(unit,
                                     sid,
                                     map_info->key_field[field_idx].fid,
                                     offsets,
                                     &offset));
        SHR_IF_ERR_EXIT
            (key_field_map_get(unit,
                               sid,
                               &map_info->key_field[field_idx],
                               offset,
                               field_map));
    }

    *num_fields = field_idx;
    *field_maps_ptr = field_maps;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_field_maps_destroy(field_max_count, field_maps);
        *num_fields = 0;
        *field_maps_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the key field list metadata.
 *
 * Create the key field list metadata for given table ID.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid LT ID for LTM internal table.
 * \param [in] map_info LRD Field mapping information list.
 * \param [in] offsets LTM working buffer key values offsets.
 * \param [out] field_list_ptr Returning pointer to field list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
key_field_list_create(int unit,
                      uint32_t sid,
                      const ltm_map_entries_t *map_info,
                      bcmltm_wb_ltm_offsets_t *offsets,
                      bcmltm_field_map_list_t **field_list_ptr)
{
    bcmltm_field_map_list_t *field_list = NULL;
    uint32_t num_fields;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(field_list, sizeof(*field_list), "bcmltmLtmFieldList");
    SHR_NULL_CHECK(field_list, SHR_E_MEMORY);
    sal_memset(field_list, 0, sizeof(*field_list));

    /* Get field maps */
    SHR_IF_ERR_EXIT
        (key_field_maps_create(unit, sid,
                               map_info, offsets,
                               &field_list->field_maps,
                               &num_fields));
    field_list->num_fields = num_fields;

    /* Not applicable */
    field_list->index_absent_offset = BCMLTM_WB_OFFSET_INVALID;

    *field_list_ptr = field_list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_field_map_list_destroy(field_list);
        *field_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the key field mapping metadata.
 *
 * This routine creates the key fields mapping metadata for given table ID.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid LT ID for LTM internal table.
 * \param [in] map_info LRD Field mapping information list.
 * \param [in] offsets LTM working buffer key values offsets.
 * \param [out] field_mapping_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
key_mapping_create(int unit,
                   uint32_t sid,
                   const ltm_map_entries_t *map_info,
                   bcmltm_wb_ltm_offsets_t *offsets,
                   bcmltm_field_select_mapping_t **field_mapping_ptr)
{
    bcmltm_field_select_mapping_t *field_mapping = NULL;
    uint32_t alloc_size;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    alloc_size = sizeof(*field_mapping) + sizeof(bcmltm_field_map_list_t *);
    SHR_ALLOC(field_mapping, alloc_size, "bcmltmLtmFieldSelectMapping");
    SHR_NULL_CHECK(field_mapping, SHR_E_MEMORY);
    sal_memset(field_mapping, 0, alloc_size);
    field_mapping->map_index_offset = BCMLTM_WB_OFFSET_INVALID;
    field_mapping->num_maps = 1;

    /* Fill data */
    SHR_IF_ERR_EXIT
        (key_field_list_create(unit, sid,
                               map_info, offsets,
                               &(field_mapping->field_map_list[0])));

    *field_mapping_ptr = field_mapping;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_field_mapping_destroy(field_mapping);
        *field_mapping_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the list of field spec metadata.
 *
 * Destroy the list of field spec metadata.
 *
 * \param [in] field_spec Field spec metadata to destroy.
 */
static void
field_spec_destroy(bcmltm_field_spec_t *field_spec)
{
    SHR_FREE(field_spec);

    return;
}

/*!
 * \brief Create the list of field spec metadata.
 *
 * Create the list of field spec metadata
 *
 * \param [in] unit Unit number.
 * \param [in] lt_map LTM Group mapping information.
 * \param [out] field_spec_ptr Returning pointer to list of field specs.
 * \param [out] num_fields Number of field specs returned in list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_spec_create(int unit,
                  const ltm_map_entries_t *lt_map,
                  bcmltm_field_spec_t **field_spec_ptr,
                  uint32_t *num_fields)
{
    bcmltm_field_spec_t *field_spec = NULL;
    unsigned int size;
    uint32_t i;
    uint32_t field_max_count;

    SHR_FUNC_ENTER(unit);

    *num_fields = 0;
    *field_spec_ptr = NULL;

    /* Get total number of fields */
    field_max_count = lt_map->num_value_fields;
    if (field_max_count == 0) {
        SHR_EXIT();
    }

    /* Allocate field_map */
    size = sizeof(*field_spec) * field_max_count;
    SHR_ALLOC(field_spec, size, "bcmltmLtmFieldSpecs");
    SHR_NULL_CHECK(field_spec, SHR_E_MEMORY);
    sal_memset(field_spec, 0, size);

    /* Fill data */
    for (i = 0; i < field_max_count ; i++) {
        field_spec[i].field_id  = lt_map->value_field[i].fid;
        field_spec[i].field_idx = lt_map->value_field[i].idx;
    }

    *num_fields = field_max_count;
    *field_spec_ptr = field_spec;

 exit:
    if (SHR_FUNC_ERR()) {
        field_spec_destroy(field_spec);
        *num_fields = 0;
        *field_spec_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the given field specifier list metadata.
 *
 * Destroy the given field specifier list metadata.
 *
 * \param [in] flist Field specifier list to destroy.
 */
static void
field_spec_list_destroy(bcmltm_field_spec_list_t *flist)
{
    if (flist == NULL) {
        return;
    }

    field_spec_destroy(flist->field_specs);

    SHR_FREE(flist);

    return;
}

/*!
 * \brief Create the field specifier list metadata for given table.
 *
 * Create the field specifier list metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] map LTM Group mapping information.
 * \param [in] offsets LTM working buffer key values offsets.
 * \param [out] flist Returning pointer for field list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
field_spec_list_create(int unit,
                       uint32_t sid,
                       const ltm_map_entries_t *lt_map,
                       bcmltm_wb_ltm_offsets_t *offsets,
                       bcmltm_field_spec_list_t **flist_ptr)
{
    bcmltm_field_spec_list_t *flist = NULL;
    uint32_t koix;
    bcmltm_table_keys_t key;
    uint32_t key_offset;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(flist, sizeof(*flist), "bcmltmLtmFieldSpecList");
    SHR_NULL_CHECK(flist, SHR_E_MEMORY);
    sal_memset(flist, 0, sizeof(*flist));

    /* Fill data */
    /* LTM offsets for key fields */
    for (koix = 0; koix < BCMLTM_TABLE_KEYS_COUNT; koix++) {
        key = koix;
        SHR_IF_ERR_EXIT
            (ltm_key_offset_get(unit, sid,
                                key, offsets, &key_offset));
        flist->key_offset[koix] = key_offset;
    }

    /* Get field specifier list */
    SHR_IF_ERR_EXIT
        (field_spec_create(unit, lt_map,
                           &flist->field_specs,
                           &flist->num_field_specs));

    *flist_ptr = flist;

 exit:
    if (SHR_FUNC_ERR()) {
        field_spec_list_destroy(flist);
        *flist_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_ltm_info_create(int unit, bcmlrd_sid_t sid,
                          bcmltm_db_ltm_info_t **info_ptr)
{
    bcmltm_db_ltm_info_t *info = NULL;
    ltm_map_entries_t map_entries;
    bcmltm_wb_block_t *wb_block = NULL;
    bcmltm_wb_handle_t *wb_handle = NULL;
    bcmltm_wb_ltm_offsets_t ltm_offsets;

    SHR_FUNC_ENTER(unit);

    /* Obtain LT map entries information */
    SHR_IF_ERR_EXIT
        (ltm_map_entries_parse(unit, sid, &map_entries));

    /* Allocate */
    SHR_ALLOC(info, sizeof(*info), "bcmltmDbLtmInfo");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    /* Add LT Index Working Buffer block */
    wb_handle = bcmltm_wb_lt_handle_get(unit, sid);
    SHR_IF_ERR_EXIT
        (bcmltm_wb_block_ltm_add(unit, wb_handle,
                                 &info->wb_block_id,
                                 &wb_block));
    SHR_IF_ERR_EXIT
        (bcmltm_wb_ltm_offsets_get(wb_block,
                                   &ltm_offsets));

    /* Create LT Index */
    SHR_IF_ERR_EXIT
        (fa_ltm_create(unit, sid, &ltm_offsets, &info->fa_ltm));

    /* Create Key Field Map list */
    SHR_IF_ERR_EXIT
        (key_mapping_create(unit, sid, &map_entries,
                            &ltm_offsets, &info->ltm_key));

    /*
     * Create LTM group Field Spec List
     */
    SHR_IF_ERR_EXIT
        (field_spec_list_create(unit, sid, &map_entries,
                                &ltm_offsets, &info->fs_list));

    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_ltm_info_destroy(info);
        *info_ptr = NULL;
    }

    ltm_map_entries_cleanup(&map_entries);

    SHR_FUNC_EXIT();
}

void
bcmltm_db_ltm_info_destroy(bcmltm_db_ltm_info_t *info)
{
    if (info == NULL) {
        return;
    }

    /* Destroy Field field spec list */
    field_spec_list_destroy(info->fs_list);

    /* Destroy Key Field Map list */
    bcmltm_db_field_mapping_destroy(info->ltm_key);

    /* Destroy LT Index */
    fa_ltm_destroy(info->fa_ltm);

    SHR_FREE(info);

    return;
}

