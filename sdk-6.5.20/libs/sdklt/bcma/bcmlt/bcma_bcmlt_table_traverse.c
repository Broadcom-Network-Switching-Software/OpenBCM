/*! \file bcma_bcmlt_table_traverse.c
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

#include <bcma/bcmlt/bcma_bcmlt.h>

int
bcma_bcmlt_table_traverse(int unit, const char *lt_name, uint32_t attr,
                          bcmlt_field_def_t *fields, uint32_t num_fields,
                          bcma_bcmlt_traverse_f cb, void *cookie)
{
    int rv;
    int cnt = 0;
    uint32_t fcnt = 0;
    bcma_bcmlt_entry_info_t *ei;

    /*
     * Use a new entry to make sure no fields are added
     * to the entry for traverse.
     */
    ei = bcma_bcmlt_entry_info_create(unit, lt_name, false, attr, NULL,
                                      fields, num_fields);
    if (ei == NULL) {
        cli_out("%sFail to init table %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, lt_name);
        return SHR_E_FAIL;
    }

    /* Set entry attribute */
    if (ei->attr != 0) {
        bcmlt_entry_attrib_set(ei->eh, ei->attr);
    }

    while ((rv = bcmlt_entry_commit(ei->eh,
                                    BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

        if (ei->attr & BCMLT_ENT_ATTR_TRAVERSE_DONE) {
            break;
        }

        if (cb) {
            if (cb(cookie, ei, cnt) < 0) {
                /* Stop traversing on error. */
                break;
            }
        }
        cnt++;

        /* Only traverse once if the LT contains no key. */
        if (ei->num_key_fields == 0) {
            break;
        }

        /*
         * Calling to traverse with an empty entry (entry contains no fields)
         * implies to traverse from get_first. Need to ensure the entry is
         * not empty when calling to traverse get_next.
         */
        rv = bcmlt_entry_field_count(ei->eh, &fcnt);
        if (SHR_SUCCESS(rv) && fcnt == 0) {
            cli_out("%sEntry contains no fields while traversing to "
                    "get_next.\n", BCMA_CLI_CONFIG_ERROR_STR);
            break;
        }
    }

    bcma_bcmlt_entry_info_destroy(ei);

    return (cnt > 0) ? cnt : rv;
}
