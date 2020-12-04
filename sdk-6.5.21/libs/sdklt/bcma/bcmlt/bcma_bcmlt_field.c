/*! \file bcma_bcmlt_field.c
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
#include <sal/sal_alloc.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

/* Free dynamic allocated array field memory */
static void
field_array_free(bcma_bcmlt_field_t *field)
{
    bcma_bcmlt_field_array_t *arr = field->u.array;

    if (arr == NULL) {
        return;
    }
    if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
        sal_free(arr->u.strs);
    } else {
        sal_free(arr->u.vals);
    }
    if (field->flags & BCMA_BCMLT_FIELD_F_ARRAY) {
        sal_free(arr->elements);
    }
    sal_free(arr);
    field->u.array = NULL;
}

/* Allocate memory for array field in array_size */
static int
field_array_alloc(bcma_bcmlt_field_t *field, int array_size)
{
    bcma_bcmlt_field_array_t *arr;

    arr = sal_alloc(sizeof(bcma_bcmlt_field_array_t), "bcmaLtFieldArray");
    if (arr == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Fail to allocate array field %s\n"),
                   field->name));
        return -1;
    }
    sal_memset(arr, 0, sizeof(*arr));

    if (field->flags & BCMA_BCMLT_FIELD_F_ARRAY) {
        /* Information for scalar array or symbol strings array field */
        arr->elements = sal_alloc(sizeof(bool) * array_size, "bcmaLtArrElem");
        if (arr->elements == NULL) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Fail to allocate array field %s info\n"),
                       field->name));
            sal_free(arr);
            return -1;
        }
        sal_memset(arr->elements, 0, sizeof(bool) * array_size);
    }

    if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
        /* Symbol strings array field */
        arr->u.strs = sal_alloc(sizeof(const char *) * array_size,
                                "bcmaLtArrStrs");
        if (arr->u.strs == NULL) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Fail to allocate symbol array field %s\n"),
                       field->name));
            sal_free(arr->elements);
            sal_free(arr);
            return -1;
        }
        sal_memset(arr->u.strs, 0, sizeof(const char *) * array_size);
    } else {
        /* Scalar array field or wide field */
        arr->u.vals = sal_alloc(sizeof(uint64_t) * array_size, "bcmaLtArrVals");
        if (arr->u.vals == NULL) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Fail to allocate value array field %s\n"),
                       field->name));
            sal_free(arr->elements);
            sal_free(arr);
            return -1;
        }
        sal_memset(arr->u.vals, 0, sizeof(uint64_t) * array_size);
    }

    arr->size = array_size;

    field->u.array = arr;

    return 0;
}

int
bcma_bcmlt_field_init(bcma_bcmlt_field_t *field, bcmlt_field_def_t *df)
{
    /* Free previously used field data (if any) */
    bcma_bcmlt_field_array_free(field);

    sal_memset(field, 0, sizeof(*field));

    field->name = df->name;
    field->width = df->width;
    sal_memcpy(&field->dflt, df, sizeof(field->dflt));

    /* Set field flags by default field attributes */
    field->flags = bcma_bcmlt_field_flags(df);

    /* Allocate memory for array-type field */
    if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
        /* Sanity check */
        bool is_wide = (field->flags & BCMA_BCMLT_FIELD_F_WIDE) ? true : false;
        if (df->elements < 1 || (df->elements == 1 && is_wide)) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Invalid attribute "
                                "(w:%"PRIu32" d:%"PRIu32" e:%"PRIu32")"
                                " of field %s\n"),
                       df->width, df->depth, df->elements,
                       field->name));
            return -1;
        }
        return field_array_alloc(field, df->elements);
    }

    return 0;
}

int
bcma_bcmlt_field_array_free(bcma_bcmlt_field_t *field)
{
    if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
        field_array_free(field);
    }
    return 0;
}
