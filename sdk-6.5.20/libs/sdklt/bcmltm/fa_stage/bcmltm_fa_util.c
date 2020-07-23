/*! \file bcmltm_fa_util.c
 *
 * Logical Table Manager Field Adaptation
 *
 * Utility functions for field manipulations
 *
 * This file contains the LTM FA stage utilities, including copying
 * API field values to/from the Working Buffer.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_util.h>

#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmlrd/bcmlrd_table.h>

#include <bcmltm/bcmltm_pt.h>
#include <bcmltm/bcmltm_fa_util_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_FIELDADAPTATION

/*******************************************************************************
 * Private functions
 */

/*******************************************************************************
 * Public functions
 */

bool
bcmltm_field_fit_check(bcmltm_wb_coordinate_t *wbc, uint32_t *field_val)
{
    uint32_t    mask;    /* Value mask                      */
    uint16_t    len;     /* Bits in field                   */
    int         idx;     /* Iteration index.                */


    len = wbc->maxbit - wbc->minbit + 1;
    idx = (len - 1) >> 5;
    len = len % 32;

    if (((idx % 2) == 0) && (field_val[idx + 1] != 0)) {
        /* Set bits in high word beyond range of field */
        return FALSE;
    }

    if (!len) {
       return TRUE;
    }

    mask = (1 << len) - 1;
    if((field_val[idx] & ~mask) != 0) {
        return (FALSE);
    }
    return (TRUE);
}

bcmltm_field_list_t *
bcmltm_api_find_field(bcmltm_field_list_t *in_fields,
                      uint32_t api_field_id,
                      uint32_t field_idx)
{
    bcmltm_field_list_t *flist = in_fields;

    while (flist != NULL) {
        if ((flist->id == api_field_id) &&
            (flist->idx == field_idx)) {
            return flist;
        }
        flist = flist->next;
    }

    return NULL;
}

void
bcmltm_field_value_into_wbc(bcmltm_wb_coordinate_t *wbc,
                            uint32_t *field_val,
                            uint32_t *ltm_buffer)
{
    uint32_t *entry_base;

    if ((wbc->minbit == BCMLTM_FIELD_BIT_POSITION_INVALID) ||
        (wbc->maxbit == BCMLTM_FIELD_BIT_POSITION_INVALID)) {
        return;
    }
    entry_base = ltm_buffer + wbc->buffer_offset;

    bcmdrd_field_set(entry_base,
                     wbc->minbit, wbc->maxbit,
                     field_val);
}

void
bcmltm_field_value_from_wbc(bcmltm_wb_coordinate_t *wbc,
                            uint32_t *field_val,
                            uint32_t *ltm_buffer)
{
    uint32_t *entry_base;

    if ((wbc->minbit == BCMLTM_FIELD_BIT_POSITION_INVALID) ||
        (wbc->maxbit == BCMLTM_FIELD_BIT_POSITION_INVALID)) {
        return;
    }

    entry_base = ltm_buffer + wbc->buffer_offset;

    bcmdrd_field_get(entry_base,
                     wbc->minbit, wbc->maxbit,
                     field_val);

    if ((wbc->maxbit - wbc->minbit) < 32) {
        /* Avoid trash in the upper word.  Pending uint64_t handling. */
        field_val[1] = 0;
    }
}

void
bcmltm_field_value_into_buffer(bcmltm_field_map_t *field_map,
                               uint32_t *field_val,
                               uint32_t *ltm_buffer)
{
    bcmltm_field_value_into_wbc(&(field_map->wbc),
                                field_val, ltm_buffer);
}

void
bcmltm_field_value_from_buffer(bcmltm_field_map_t *field_map,
                               uint32_t *field_val,
                               uint32_t *ltm_buffer)
{
    bcmltm_field_value_from_wbc(&(field_map->wbc),
                                field_val, ltm_buffer);
}

void
bcmltm_field_map_into_buffer(bcmltm_field_map_t *field_map,
                             uint32_t *ltm_buffer)
{
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    bcmltm_field_list_t *field_data;
    uint32_t *ptr_math;

    ptr_math = ltm_buffer + field_map->apic_offset;
    field_data = (bcmltm_field_list_t *)ptr_math;

    shr_uint64_to_uint32_array(field_data->data, field_val);
    bcmltm_field_value_into_wbc(&(field_map->wbc),
                                field_val, ltm_buffer);
}

void
bcmltm_field_map_from_buffer(bcmltm_field_map_t *field_map,
                             uint32_t *ltm_buffer)
{
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    bcmltm_field_list_t *field_data;
    uint32_t *ptr_math;

    ptr_math = ltm_buffer + field_map->apic_offset;
    field_data = (bcmltm_field_list_t *)ptr_math;

    field_val[0] = field_val[1] = 0;
    bcmltm_field_value_from_wbc(&(field_map->wbc),
                                field_val, ltm_buffer);
    shr_uint32_array_to_uint64(field_val, &(field_data->data));
    field_data->flags = 0;
    field_data->id = field_map->field_id;
    field_data->idx = field_map->field_idx;
}

void
bcmltm_field_map_lta_from_buffer(bcmltm_field_map_t *field_map,
                                 uint32_t *ltm_buffer)
{
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    bcmltm_field_list_t *field_data;
    uint32_t *ptr_math;

    ptr_math = ltm_buffer + field_map->apic_offset;
    field_data = (bcmltm_field_list_t *)ptr_math;

    field_val[0] = field_val[1] = 0;
    bcmltm_field_value_from_wbc(&(field_map->wbc),
                                field_val, ltm_buffer);
    shr_uint32_array_to_uint64(field_val, &(field_data->data));
    field_data->flags = 0;
    field_data->id = field_map->field_id;
    field_data->idx = field_map->field_idx;
}

bcmltm_field_list_t *
bcmltm_lta_find_field(const bcmltd_fields_t *lta_fields,
                      uint32_t field_id,
                      uint32_t field_idx)
{
    int fix, max_fields;
    bcmltd_field_t **lta_fields_ptrs;

    lta_fields_ptrs = lta_fields->field;
    max_fields = lta_fields->count;

    for (fix = 0; fix < max_fields; fix++) {
        if ((lta_fields_ptrs[fix]->id == field_id) &&
            (lta_fields_ptrs[fix]->idx == field_idx)) {
            return lta_fields_ptrs[fix];
        }
    }

    return NULL;
}
