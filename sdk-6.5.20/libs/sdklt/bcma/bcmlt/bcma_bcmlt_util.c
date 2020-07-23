/*! \file bcma_bcmlt_util.c
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
#include <shr/shr_pb_format.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/io/bcma_io_term.h>
#include <bcma/bcmlt/bcma_bcmlt.h>

#define DEFAULT_WINDOW_COLUMNS     80
#define STRS_ARRAY_SIZE_THRESHOLD  128

/*
 * Return formatted string of field access type.
 */
static char *
field_access_str(bcmlt_field_acc_t acc)
{
    switch (acc) {
    case BCMLT_FIELD_ACCESS_READ_WRITE:
        return "R/W";
    case BCMLT_FIELD_ACCESS_READ_ONLY:
        return "R/O";
    default:
        return "N/A";
    }
}

const char *
bcma_bcmlt_entry_field_default_info_format(shr_pb_t *pb, const char *pf_str,
                                           int unit, const char *tbl_name,
                                           bcmlt_field_def_t *df)
{
    const char *prefix = pf_str ? pf_str : "";

    if (pb == NULL || df == NULL) {
        return "<N/A>\n";
    }

    /* Field width. */
    shr_pb_printf(pb, "%sWidth: %d bit%s\n",
                  prefix, df->width, df->width > 1 ? "s" : "");

    /* Display field default, minimum and maximum values for scalar fields. */
    if (!df->symbol) {
        shr_pb_printf(pb, "%sValue (default, min, max): ", prefix);
        shr_pb_format_uint64(pb, "", &df->def, 1, 0);
        shr_pb_format_uint64(pb, ", ", &df->min, 1, 0);
        shr_pb_format_uint64(pb, ", ", &df->max, 1, 0);
        shr_pb_printf(pb, "\n");
    }

    /* Field attribute: key, symbol, array, wide-field. */
    shr_pb_printf(pb, "%sAttribute: ", prefix);
    shr_pb_printf(pb, "%s", field_access_str(df->access));
    if (df->key) {
        shr_pb_printf(pb, ", key");
    }
    if (df->symbol) {
        shr_pb_printf(pb, ", symbol");
    }
    if (df->depth > 0) {
        shr_pb_printf(pb, ", array[%d]", df->depth);
    }
    if (df->width > 64) {
        shr_pb_printf(pb, ", wide-field[%d]", df->elements);
    }
    shr_pb_printf(pb, "\n");

    /* Get symbol strings of a symbol field. */
    if (df->symbol) {
        const char **sym_strs;
        uint32_t elems;

        shr_pb_printf(pb, "%sSymbol: ", prefix);
        sym_strs = bcma_bcmlt_entry_field_symbol_strs_get(unit, tbl_name,
                                                          df->name, &elems);
        if (sym_strs) {
            shr_pb_printf(pb, "%u\n", elems);
            bcma_bcmlt_format_strs_array(pb, "          ", sym_strs, elems);
            sal_free(sym_strs);
        } else {
            shr_pb_printf(pb, "N/A\n");
        }

        /* The symbol field default value. */
        shr_pb_printf(pb, "%sDefault value: %s\n", prefix, df->sym_def);
    }

    return shr_pb_str(pb);
}

/*
 * Format the strings in array strs in row x column matrix.
 * The columns are calculated dynamically according to the current
 * window width and the maximum string length in the array. If the
 * array size is too large, the calculation will be skipped and the
 * strings will be listed in one column per line.
 */
void
bcma_bcmlt_format_strs_array(shr_pb_t *pb, const char * prefix,
                             const char **strs, int cnt)
{
    int len, maxlen = 0;
    const char *pfix = prefix ? prefix : "";
    int idx, row, rows, col, cols, cw = 0;
    int win_cols, win_rows;

    if (cnt > STRS_ARRAY_SIZE_THRESHOLD) {
        /*
         * Optimization for large array size to avoid the column width
         * calculation. Just print each string per line.
         */
        cols = 1;
    } else {
        /* Determine column count from the maximum string length. */
        for (idx = 0; idx < cnt; idx++) {
            len = sal_strlen(strs[idx]);
            if (len > maxlen) {
                maxlen = len;
            }
        }
        cw = maxlen + 3;
        if (bcma_io_term_winsize_get(&win_cols, &win_rows) < 0) {
            win_cols = DEFAULT_WINDOW_COLUMNS;
        }
        cols = (win_cols - sal_strlen(pfix)) / cw;
        if (cols <= 0) {
            cols = 1;
        }
    }
    rows = (cnt + cols - 1) / cols;

    for (row = 0; row < rows; row++) {
        shr_pb_printf(pb, "%s", pfix);
        for (col = 0; col < cols; col++) {
            idx = row + col * rows;
            if (idx >= cnt) {
                break;
            }
            if (cols == 1) {
                shr_pb_printf(pb, "%s", strs[idx]);
            } else {
                shr_pb_printf(pb, "%-*s", cw, strs[idx]);
            }
        }
        shr_pb_printf(pb, "\n");
    }
}

const char **
bcma_bcmlt_entry_field_symbol_strs_get(int unit, const char *tbl_name,
                                       const char *field_name,
                                       uint32_t *elems)
{
    int rv;
    bcmlt_entry_handle_t eh;
    const char **sstrs;
    uint32_t cnt;

    if (tbl_name == NULL || field_name == NULL) {
        return NULL;
    }

    rv = bcmlt_entry_allocate(unit, tbl_name, &eh);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META_U(unit, "%s: "BCMA_BCMLT_ERRMSG_FMT"\n"),
                   tbl_name, BCMA_BCMLT_ERRMSG(rv)));
        return NULL;
    }
    /* Get symbol strings array size. */
    rv = bcmlt_entry_field_symbol_info_get(eh, field_name, 0, NULL, &cnt);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META_U(unit, "%s-%s: "BCMA_BCMLT_ERRMSG_FMT"\n"),
                   tbl_name, field_name, BCMA_BCMLT_ERRMSG(rv)));
        bcmlt_entry_free(eh);
        return NULL;
    }
    sstrs = sal_alloc(sizeof(const char *) * cnt, "bcmaBcmltFieldSymStrs");
    if (sstrs == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META_U(unit, "Fail to allocate symbol strings\n")));
        bcmlt_entry_free(eh);
        return NULL;
    }
    /* Get symbol strings array. */
    rv = bcmlt_entry_field_symbol_info_get(eh, field_name, cnt, sstrs, &cnt);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META_U(unit, "%s-%s: "BCMA_BCMLT_ERRMSG_FMT"\n"),
                   tbl_name, field_name, BCMA_BCMLT_ERRMSG(rv)));
        sal_free(sstrs);
        bcmlt_entry_free(eh);
        return NULL;
    }
    bcmlt_entry_free(eh);
    *elems = cnt;
    return sstrs;
}
