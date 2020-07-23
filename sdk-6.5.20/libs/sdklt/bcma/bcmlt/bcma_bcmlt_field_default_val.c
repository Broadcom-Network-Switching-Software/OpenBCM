/*! \file bcma_bcmlt_field_default_val.c
 *
 * Functions based on bcmlt library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <sal/sal_libc.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

int
bcma_bcmlt_field_default_val_set(bcma_bcmlt_field_t *field)
{
    bcmlt_field_def_t *df;

    if (!field) {
        return -1;
    }
    df = &field->dflt;
    if (df->name != field->name) {
        return -1;
    }

    if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
        bcma_bcmlt_field_array_t *arr = field->u.array;
        int idx;

        if (field->flags & BCMA_BCMLT_FIELD_F_WIDE) {
            arr->u.vals[0] = df->def;
        } else if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
            /* Set default value to each element in the symbol array. */
            for (idx = 0; idx < arr->size; idx++) {
                arr->u.strs[idx] = df->sym_def;
                arr->elements[idx] = true;
            }
        } else {
            /* Set default value to each element in the scalar array. */
            for (idx = 0; idx < arr->size; idx++) {
                arr->u.vals[idx] = df->def;
                arr->elements[idx] = true;
            }
        }
    } else if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
        field->u.str = df->sym_def;
    } else {
        field->u.val = df->def;
    }

    return 0;
}

int
bcma_bcmlt_field_default_val_cmp(bcma_bcmlt_field_t *field)
{
    bcmlt_field_def_t *df;

    if (!field) {
        return -1;
    }
    df = &field->dflt;
    if (df->name != field->name) {
        return -1;
    }

    if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
        bcma_bcmlt_field_array_t *arr = field->u.array;
        int idx;

        if (field->flags & BCMA_BCMLT_FIELD_F_WIDE) {
            if (arr->u.vals[0] != df->def) {
                return -1;
            }
        } else if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
            /* Compare default value to each element in the symbol array. */
            for (idx = 0; idx < arr->size; idx++) {
                if (arr->elements[idx] != true ||
                    arr->u.strs[idx] != df->sym_def) {
                    return -1;
                }
            }
        } else {
            /* Compare default value to each element in the scalar array. */
            for (idx = 0; idx < arr->size; idx++) {
                if (arr->elements[idx] != true ||
                    arr->u.vals[idx] != df->def) {
                    return -1;
                }
            }
        }
    } else if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
        if (field->u.str != df->sym_def) {
            return -1;
        }
    } else {
        if (field->u.val != df->def) {
            return -1;
        }
    }

    return 0;
}
