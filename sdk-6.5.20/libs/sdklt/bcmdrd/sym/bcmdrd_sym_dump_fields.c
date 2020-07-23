/*! \file bcmdrd_sym_dump_fields.c
 *
 * Dump symbol field information into a string.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <shr/shr_pb.h>

#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_sym_dump.h>

#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 1
/*!
 * \brief Print bit range string.
 *
 * \param [in,out] pb Print buffer.
 * \param [in] size Size of ou
 * \param [in] minbit First bit in bit range.
 * \param [in] maxbit Last bit in bit range.
 *
 * \retval 0 No errors
 */
static int
show_bit_range(shr_pb_t *pb, int minbit, int maxbit)
{
    if (minbit == maxbit) {
        shr_pb_printf(pb, "<%d>", minbit);
    } else {
        shr_pb_printf(pb, "<%d:%d>", maxbit, minbit);
    }
    return 0;
}

/*!
 * \brief Compare two strings from qsort.
 *
 * Note that the input parameters are not strings, but pointers to
 * strings.
 *
 * \param [in] e1 Pointer to first string.
 * \param [in] e2 Pointer to second string.
 *
 * \return Same as standard strcmp function.
 */
static int
qsort_strcmp(const void *e1, const void *e2)
{
    const char *s1 = *(const char **)e1;
    const char *s2 = *(const char **)e2;

    return sal_strcmp(s1, s2);
}

/*!
 * \brief Sort field string.
 *
 * Create a copy of the field string and split it into separate
 * strings for each field. Sort the string array using qsort and write
 * the sorted list back to the print buffer.
 *
 * The last field in the fields string is expected to be followed by a
 * delimiter as well.
 *
 * In case of errors, the fields string will remain unchanged.
 *
 * \param [in] pb Print buffer containing unsorted fields string.
 * \param [in] start Offset of the fields string in the print buffer.
 * \param [in] fcnt Number of fields in the fields string.
 * \param [in] delimiter Delimiter between each field.
 *
 * \retval 0 No errors.
 */
static int
sort_fields(shr_pb_t *pb, int start, int fcnt, char delimiter)
{
    int idx;
    char *ptr, *dstr;
    const char *fstr;
    const char **str_array;

    if (fcnt <= 0) {
        /* Nothing to sort */
        return 0;
    }

    /* Create a copy of the fields string in the print buffer */
    fstr = shr_pb_str(pb);
    dstr = sal_strdup(&fstr[start]);
    if (dstr) {
        /* Split fields string into separate strings for each field */
        str_array = sal_alloc(fcnt * sizeof(char *), "bcmdrdDumpFld");
        if (str_array) {
            fstr = dstr;
            for (idx = 0; idx < fcnt; idx++) {
                str_array[idx] = fstr;
                ptr = sal_strchr(fstr, delimiter);
                if (ptr) {
                    *ptr = '\0';
                    fstr = ptr + 1;
                }
            }
            /* Sort strng array */
            sal_qsort(str_array, fcnt, sizeof(char *), qsort_strcmp);
            /* Rewind print buffer and write back sprted field strings */
            shr_pb_mark(pb, start);
            shr_pb_reset(pb);
            for (idx = 0; idx < fcnt; idx++) {
                shr_pb_printf(pb, "%s%c", str_array[idx], delimiter);
            }
            sal_free(str_array);
        }
        sal_free(dstr);
    }
    return 0;
}
#endif

int
bcmdrd_sym_dump_fields(shr_pb_t *pb, const bcmdrd_symbol_t *symbol,
                       const char **fnames, uint32_t *data, uint32_t flags,
                       bcmdrd_symbol_filter_cb_f fcb, void *fcb_cookie,
                       bcmdrd_symbol_custom_filter_cb_f ccb, void *ccb_cookie)
{
#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 0
    return 0;
#else
    int mval;
    int start;
    char *indent;
    char delimiter;
    bcmdrd_sym_field_info_t finfo;
    uint32_t val[BCMDRD_MAX_PT_WSIZE];
    int idx = 0;
    int fcnt = 0;
    int size;

    /* Save starting point */
    start = shr_pb_mark(pb, -1);

    /* Indentation before each field */
    indent = (flags & BCMDRD_SYM_DUMP_SINGLE_LINE) ? "" : "\t";

    /* Delimiter between each field */
    delimiter = (flags & BCMDRD_SYM_DUMP_SINGLE_LINE) ? ',' : '\n';

    BCMDRD_SYM_FIELDS_ITER_BEGIN(symbol->fields, finfo, fnames) {

        /* Create a reset point for skipping zeros */
        shr_pb_mark(pb, -1);

        /* Use real field name when available */
        if (finfo.name) {
            if (ccb) {
                if (ccb(symbol, finfo.name, finfo.flags, data, ccb_cookie) != 0) {
                    continue;
                }
            }
            if (fcb && finfo.view >= 0) {
                if (fcb(symbol, fnames, fnames[finfo.view], fcb_cookie) != 0) {
                    continue;
                }
            }
            shr_pb_printf(pb, "%s%s", indent, finfo.name);
        } else {
            shr_pb_printf(pb, "%sfield%d", indent, idx++);
        }

        /* Create bit range string */
        if (flags & BCMDRD_SYM_DUMP_BIT_RANGE) {
            show_bit_range(pb, finfo.minbit, finfo.maxbit);
        }

        if (data) {
            /* Get normalized field value */
            sal_memset(val, 0, sizeof(val));
            if (symbol->flags & BCMDRD_SYMBOL_FLAG_BIG_ENDIAN) {
                size = BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index);
                bcmdrd_field_be_get(data, BCMDRD_BYTES2WORDS(size),
                                    finfo.minbit, finfo.maxbit, val);
            } else {
                bcmdrd_field_get(data, finfo.minbit, finfo.maxbit, val);
            }

            /* Create field value string */
            mval = COUNTOF(val) - 1;
            while (mval && val[mval] == 0) {
                mval--;
            }

            /* Optionally skip fields with all zeros */
            if (flags & BCMDRD_SYM_DUMP_SKIP_ZEROS) {
                if (mval == 0 && val[mval] == 0) {
                    shr_pb_reset(pb);
                    continue;
                }
            }

            if (mval == 0 && val[mval] < 10) {
                /* Values less thans 10 in decimal only */
                shr_pb_printf(pb, "=%"PRIu32, val[0]);
            } else if (mval == 0 && (flags & BCMDRD_SYM_DUMP_EXT_HEX)) {
                /* Single-word values in both decimal and hexadecimal */
                shr_pb_printf(pb, "=0x%"PRIx32"(%"PRIu32")", val[0], val[0]);
            } else {
                /* Anything else is shown as a hex-string */
                shr_pb_printf(pb, "=0x%"PRIx32"", val[mval]);
                while (mval--) {
                    shr_pb_printf(pb, "_%08"PRIx32"", val[mval]);
                }
            }
        }

        /* Add field delimiter */
        shr_pb_printf(pb, "%c", delimiter);

        fcnt++;

    } BCMDRD_SYM_FIELDS_ITER_END();

    if (flags & BCMDRD_SYM_DUMP_ALPHA_SORT) {
        sort_fields(pb, start, fcnt, delimiter);
    }

    if (flags & BCMDRD_SYM_DUMP_SINGLE_LINE) {
        if (fcnt) {
            /* Remove last delimiter */
            shr_pb_mark(pb, shr_pb_mark(pb, -1) - 1);
            shr_pb_reset(pb);
        }
        shr_pb_printf(pb, "\n");
    }

    return 0;
#endif /* BCMDRD_CONFIG_INCLUDE_FIELD_INFO */
}
