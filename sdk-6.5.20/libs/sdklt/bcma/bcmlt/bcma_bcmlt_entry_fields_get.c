/*! \file bcma_bcmlt_entry_fields_get.c
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
bcma_bcmlt_entry_fields_get(bcma_bcmlt_entry_info_t *ei)
{
    int rv;
    int idx;
    const char *fname;
    bcmlt_field_data_info_t finfo;
    uint32_t fhdl = BCMLT_INVALID_FIELD_SEARCH_HDL;

    if (ei == NULL) {
        return -1;
    }

    /* Clean up fields first */
    for (idx = 0; idx < ei->szfields; idx++) {
        bcma_bcmlt_field_t *field = &ei->fields[idx];
        bcma_bcmlt_field_array_free(field);
    }
    sal_memset(ei->fields, 0, sizeof(bcma_bcmlt_field_t) * ei->szfields);
    ei->num_fields = 0;

    while ((rv = bcmlt_entry_field_traverse(ei->eh, &fname, &finfo, &fhdl)) ==
           SHR_E_NONE) {
        /*
         * Find the corresponding default field information from
         * the field name.
         */
        for (idx = 0; idx < ei->num_def_fields; idx++) {
            bcmlt_field_def_t *def_field = &ei->def_fields[idx];
            bcma_bcmlt_field_t *field = &ei->fields[idx];

            /* Collect the field value into field structure. */
            if (sal_strcmp(fname, def_field->name) == 0) {
                if (field->name == NULL) {
                    bcma_bcmlt_field_init(field, def_field);
                }
                if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
                    bcma_bcmlt_field_array_t *arr = field->u.array;

                    if (field->flags & BCMA_BCMLT_FIELD_F_WIDE) {
                        arr->u.vals[finfo.idx] = finfo.val;
                    } else {
                        if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
                            arr->u.strs[finfo.idx] = finfo.sym_val;
                        } else {
                            arr->u.vals[finfo.idx] = finfo.val;
                        }
                        arr->elements[finfo.idx] = true;
                    }
                } else {
                    if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
                        field->u.str = finfo.sym_val;
                    } else {
                        field->u.val = finfo.val;
                    }
                }
                break;
            }
        }
    }
    if (rv != SHR_E_NOT_FOUND) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to traverse fields in table %s entry: "
                            "%s (error code %d).\n"),
                   ei->name, shr_errmsg(rv), rv));
        return -1;
    }

    ei->num_fields = ei->num_def_fields;

    return 0;
}
