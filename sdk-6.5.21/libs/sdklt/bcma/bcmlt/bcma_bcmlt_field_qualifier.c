/*! \file bcma_bcmlt_field_qualifier.c
 *
 * Functions entry fields qualifier support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

/* Matching qualifiers for > operator */
#define GT_QLFRS    (BCMA_BCMLT_FIELD_QLFR_GT | BCMA_BCMLT_FIELD_QLFR_GE | \
                     BCMA_BCMLT_FIELD_QLFR_NE)

/* Matching qualifiers for < operator */
#define LT_QLFRS    (BCMA_BCMLT_FIELD_QLFR_LT | BCMA_BCMLT_FIELD_QLFR_LE | \
                     BCMA_BCMLT_FIELD_QLFR_NE)

/* Matching qualifiers for == operator */
#define EQ_QLFRS    (BCMA_BCMLT_FIELD_QLFR_EQ | \
                     BCMA_BCMLT_FIELD_QLFR_GE | BCMA_BCMLT_FIELD_QLFR_LE)

/* Enum for the supported qualifiers */
static bcma_cli_parse_enum_t field_qualifier_enum[] = {
    {"==", BCMA_BCMLT_FIELD_QLFR_EQ},
    {"!=", BCMA_BCMLT_FIELD_QLFR_NE},
    {">=", BCMA_BCMLT_FIELD_QLFR_GE},
    {"<=", BCMA_BCMLT_FIELD_QLFR_LE},
    {">",  BCMA_BCMLT_FIELD_QLFR_GT},
    {"<",  BCMA_BCMLT_FIELD_QLFR_LT},
    {NULL, 0}
};

/*
 * Comparison for wide field values
 */
static uint32_t
wide_val_cmp(uint64_t *val1, uint64_t *val2, int numel)
{
    int idx;

    for (idx = numel-1; idx >= 0; idx--) {
        if (val1[idx] > val2[idx]) {
            return GT_QLFRS;
        }
        if (val1[idx] < val2[idx]) {
            return LT_QLFRS;
        }
    }
    return EQ_QLFRS;
}

/*
 * Comparison for uint64 scalar values
 */
static uint32_t
scalar_val_cmp(uint64_t val1, uint64_t val2)
{
    if (val1 > val2) {
        return GT_QLFRS;
    }
    if (val1 < val2) {
        return LT_QLFRS;
    }
    return EQ_QLFRS;
}

/*
 * Comparison for symbol string fields
 */
static uint32_t
sym_string_cmp(const char *str1, const char *str2)
{
    if (sal_strcasecmp(str1, str2) == 0) {
        return BCMA_BCMLT_FIELD_QLFR_EQ;
    }
    return BCMA_BCMLT_FIELD_QLFR_NE;
}

/*
 * Comparison for target field and the qualifier field according to
 * the field data type
 */
static uint32_t
field_val_cmp(bcma_bcmlt_field_t *field, bcma_bcmlt_field_t *qlfr_field)
{
    int qv = 0;

    if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
        bcma_bcmlt_field_array_t *arr1 = field->u.array;
        bcma_bcmlt_field_array_t *arr2 = qlfr_field->u.array;

        if (field->flags & BCMA_BCMLT_FIELD_F_WIDE) {
            qv = wide_val_cmp(arr1->u.vals, arr2->u.vals, arr1->size);
        } else {
            int idx;

            qv = 0xffffffff;
            for (idx = 0; idx < arr1->size; idx++) {
                if (arr2->elements[idx]) {
                    if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
                        qv &= sym_string_cmp(arr1->u.strs[idx],
                                             arr2->u.strs[idx]);
                    } else {
                        qv &= scalar_val_cmp(arr1->u.vals[idx],
                                             arr2->u.vals[idx]);
                    }
                }
            }
            if ((qv & BCMA_BCMLT_FIELD_QLFR_EQ) == 0) {
                qv |= BCMA_BCMLT_FIELD_QLFR_NE;
            }
        }
    } else {
        if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
            qv = sym_string_cmp(field->u.str, qlfr_field->u.str);
        } else {
            qv = scalar_val_cmp(field->u.val, qlfr_field->u.val);
        }
    }

    return qv;
}

const bcma_cli_parse_enum_t *
bcma_bcmlt_field_qualifier_enum_get(void)
{
    return field_qualifier_enum;
}

bool
bcma_bcmlt_field_qualifier(bcma_bcmlt_field_t *field,
                           bcma_bcmlt_field_t *qlfr_field)
{
    const bcma_cli_parse_enum_t *qlfr;

    if (field == NULL || qlfr_field == NULL) {
        return true;
    }

    qlfr = qlfr_field->qlfr;
    if (qlfr == NULL) {
        return true;
    }

    if ((qlfr->val & field_val_cmp(field, qlfr_field)) == 0) {
        return false;
    }

    return true;
}
