/*! \file bcmlrd_field_def_get.c
 *
 * Get the field definition for the given table and map.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_internal.h>

static bcmltd_field_acc_t
field_access_get(uint32_t flags)
{
    return (flags & BCMLRD_FIELD_F_READ_ONLY) ?
        BCMLTD_FIELD_ACCESS_READ_ONLY :
        BCMLTD_FIELD_ACCESS_READ_WRITE;
}

/*!
 * \brief Get field definition information by given table field data
 *        and map field data.
 *
 * This routine gets the field definition information for mapped fields.
 * The given field ID must be a valid mapped field.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  fid             Field ID.
 * \param [in]  tbl_field       Table Field data.
 * \param [in]  map_field       Map Field data.
 * \param [out] field_def       Field Def data.
 *
 * \retval 0 OK
 * \retval <0 ERROR
 */
static int
bcmlrd_core_field_def_get(int unit,
                          const bcmlrd_fid_t fid,
                          const bcmlrd_table_rep_t *tbl,
                          const bcmlrd_map_t *map,
                          bcmlrd_field_def_t *field_def)
{
    const bcmlrd_field_data_t *map_field;
    const bcmltd_field_rep_t *tbl_field;

    map_field = &map->field_data->field[fid];
    tbl_field = &tbl->field[fid];

    sal_memset(field_def, 0, sizeof(*field_def));

    field_def->id = fid;
    field_def->name = tbl_field->name;
    field_def->key = !!(tbl_field->flags & BCMLRD_FIELD_F_KEY);
    field_def->symbol = !!(tbl_field->flags & BCMLRD_FIELD_F_ENUM);
    field_def->array = true; /* treat every field as an array */
    field_def->width = (map_field->width > 0) ? map_field->width:
                                                tbl_field->width;
    field_def->depth = map_field->depth;
    if (field_def->depth > 0) {
        /* number of elements of an array field is depth */
        field_def->elements = field_def->depth;
    } else {
        /* number of elements of a scalar field depends on width */
        field_def->elements =
            (field_def->width + (BCM_FIELD_SIZE_BITS - 1)) / BCM_FIELD_SIZE_BITS;
    }

    /* field access flags may be from the logical table definition
       or the field mapping */
    field_def->access = field_access_get(tbl_field->flags | map_field->flags);

    if (tbl_field->width == 1) {
        field_def->dtag = BCMLT_FIELD_DATA_TAG_BOOL;
        field_def->min.is_true = map_field->min->is_true;
        field_def->def.is_true = map_field->def->is_true;
        field_def->max.is_true = map_field->max->is_true;
    } else if (tbl_field->width <= 8) {
        field_def->dtag = BCMLT_FIELD_DATA_TAG_U8;
        field_def->min.u8 = map_field->min->u8;
        field_def->def.u8 = map_field->def->u8;
        field_def->max.u8 = map_field->max->u8;
    } else if (tbl_field->width <= 16) {
        field_def->dtag = BCMLT_FIELD_DATA_TAG_U16;
        field_def->min.u16 = map_field->min->u16;
        field_def->def.u16 = map_field->def->u16;
        field_def->max.u16 = map_field->max->u16;
    } else if (tbl_field->width <= 32) {
        field_def->dtag = BCMLT_FIELD_DATA_TAG_U32;
        field_def->min.u32 = map_field->min->u32;
        field_def->def.u32 = map_field->def->u32;
        field_def->max.u32 = map_field->max->u32;
    } else if (tbl_field->width <= 64) {
        field_def->dtag = BCMLT_FIELD_DATA_TAG_U64;
        field_def->min.u64 = map_field->min->u64;
        field_def->def.u64 = map_field->def->u64;
        field_def->max.u64 = map_field->max->u64;
    } else {
        field_def->dtag = BCMLT_FIELD_DATA_TAG_RAW;
        field_def->min.binary = map_field->min->binary;
        field_def->def.binary = map_field->def->binary;
        field_def->max.binary = map_field->max->binary;
    }

    return SHR_E_NONE;
}

int
bcmlrd_field_def_get(int unit,
                     const bcmlrd_fid_t fid,
                     const bcmlrd_table_rep_t *tbl,
                     const bcmlrd_map_t *map,
                     bcmlrd_field_def_t *field_def)
{
    int rv = SHR_E_UNAVAIL;

    /* Get field definition for mapped field */
    if (!bcmlrd_field_is_unmapped_internal(map, fid)) {
        rv = bcmlrd_core_field_def_get(unit, fid, tbl, map, field_def);
    }

    return rv;
}
