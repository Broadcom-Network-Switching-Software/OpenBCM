/*! \file bcmltd_table_name_to_idx.c
 *
 * Return the index of the given base table name.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmltd/bcmltd_internal.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmltd/chip/bcmltd_limits.h>

/*******************************************************************************
 * Private functions
 */

static int bcmltd_num_base_tables;

/*!
 * \brief Count the number of base tables.
 *
 * Count the number of BASE tables (tables that are available for
 * mapping for every device. The array of table definition data
 * structures are in alphabetical order for BASE tables, followed by
 * each variant, also in alphabetical order. Because the array starts
 * with BASE, is compact, and is followed by variant tables, also in
 * alphabetical order, is it sufficient to find the end of the BASE
 * table list by stopping when either the ordering is violated (the
 * end ordering of a BASE table immediately followed by by the start
 * ordering of the immediately following variant) or a NULL table
 * pointer (where the variant is not compiled in). In the corner case
 * where an immediately following variant table is ordered after the
 * last base table, the calculated number will be larger, but this is
 * not expected to be a problem because bcmltd_table_name_to_idx() is
 * primarily an existence test for base tables in during configuration
 * where it is not uncommon for a table to be defined but not mapped
 * for a device, and such a corner case table would behave similarly.
 *
 * \retval Number of base tables
 */
static int
bcmltd_num_base_tables_init(void)
{
    int idx = 0;
    int v;
    const bcmltd_table_rep_t *tbl0;
    const bcmltd_table_rep_t *tbl1;

    for (idx = 1; idx < BCMLTD_TABLE_COUNT; idx++) {
        tbl0 = bcmltd_table_get(idx-1);
        tbl1 = bcmltd_table_get(idx);

        if (tbl0 == NULL || tbl1 == NULL) {
            break;
        }

        v = sal_strcmp(tbl0->name, tbl1->name);

        if (v >= 0) {
            break;
        }
    }

    return idx;
}

/*******************************************************************************
 * Public functions
 */

/* Binary search table to return index of base tables only. */
int
bcmltd_table_name_to_idx(const char *table_name)
{
    int bot = 0;
    int top;
    int idx = 0;
    int v = 0;
    const bcmltd_table_rep_t *tbl;

    if (bcmltd_num_base_tables == 0) {
        bcmltd_num_base_tables = bcmltd_num_base_tables_init();
    }

    top = bcmltd_num_base_tables - 1;
    while (top >= bot) {
        idx = (bot+top)/2; /* pick midpoint */
        tbl = bcmltd_table_get(idx);
        if (tbl == NULL) {
            return SHR_E_UNAVAIL;
        }
        v = sal_strcmp(table_name, tbl->name);
        if (v == 0) {
            break;
        } else if (v < 0) {
            /* must be below idx, so move top just below idx */
            top = idx-1;
        } else {
            /* must be above idx, so move bottom just above idx */
            bot = idx+1;
        }
    }

    return (v == 0) ?
        idx :
        SHR_E_UNAVAIL;
}

