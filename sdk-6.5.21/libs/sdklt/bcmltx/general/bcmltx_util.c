/*! \file bcmltx_util.c
 *
 * Built-in utility functions for other bcmltx implementations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltx/bcmltx_util.h>

/*******************************************************************************
 * Public functions
 */
bcmltx_field_list_t *
bcmltx_lta_find_field(const bcmltd_fields_t *lta_fields,
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

int
bcmltx_field_data_to_uint32_array(bcmltx_field_list_t *field,
                                  uint32_t *field_val)
{
    field_val[0] = field->data & 0xffffffff;
    field_val[1] = (field->data >> 32) & 0xffffffff;
    return SHR_E_NONE;
}

