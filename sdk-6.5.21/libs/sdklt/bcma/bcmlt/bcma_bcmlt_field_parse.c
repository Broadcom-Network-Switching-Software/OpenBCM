/*! \file bcma_bcmlt_field_parse.c
 *
 * Field parse and format functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_bitop.h>
#include <shr/shr_pb_format.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_token.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

/* Supported field format type in field_format function. */
#define FIELD_FMT_TYPE_RAW          0
#define FIELD_FMT_TYPE_YAML         1

/*
 * Remove a specified character from the given string.
 */
static void
str_rm_chr(char *src, char c)
{
    if (src) {
        char *dst = src;

        do {
            if (*src != c) {
                *dst++ = *src;
            }
        } while (*src++ != '\0');
    }
}

/*
 * Convert the input field symbol string to upper case except for
 * field PT_ID which is ended in lower case 'm' or 'r'.
 */
static void
field_symbol_string_check(const char *field, char **field_val)
{
    char *str = *field_val;

    /* Take the string as-is the symbol string which is case ignored. */
    sal_strupr(str);

    /*
     * Exceptions:
     *   Field PT_ID is ended in 'm' or 'r'.
     *   Field PT_ID_DATA is "INVALID_PT" or ended in 'm' or 'r' as PT_ID.
     */
    if (sal_strcmp(field, "PT_ID") == 0 ||
        sal_strcmp(field, "PT_ID_DATA") == 0) {
        int idx = sal_strlen(str) - 1;
        char c = str[idx];

        if (c == 'M' || c == 'R') {
            str[idx] = sal_tolower(c);
        }
    }
}

/*
 * Field array assignment string field_arg should be in format of:
 * <field>[<r1>]=<v2>{,<field>[<r2>]=<v2>,...}, where ri = <i> | <i1>-<i2>
 * For example: field1[1]=3,field1[2-4]=5
 * Field array qualifier string field arg is in format of:
 * <field>[<r1>]<qlfr><v2>, where <qlfr> can be ==,!=, >=, <=, > or <.
 * For example: field1[3]>=5
 * The 'qualifier' argument is use to check whether the field_arg is
 * a qualifier string or a pure value assignment string.
 */
static int
field_array_parse(char *field_arg, const char *name,
                  const bcma_cli_parse_enum_t *qualifier,
                  bcma_bcmlt_field_array_t *arr, int is_str)
{
    int rv = 0;
    int len;
    int idx = -1, start = -1, end = -1;
    char *tok, *s = NULL;

    if (arr == NULL) {
        return -1;
    }
    if ((is_str && arr->u.strs == NULL) || (!is_str && arr->u.vals == NULL)) {
        return -1;
    }
    if (qualifier && sal_strchr(field_arg, ',') != NULL) {
        /*
         * The qualifier field does not support multiple field array assignment
         * in a single token separated by commas, in case different qualifiers
         * are specified and can not be stored in a single qualifier struct
         * in each bcma_bcmlt_field_t struct.
         */
        return -1;
    }

    len = sal_strlen(name);

    /* Tokenize the array field argument by ',' (if any) */
    tok = sal_strtok_r(field_arg, ",", &s);
    while (tok) {
        /* Parse <field>[<r>]=<v> or qualifiers such as <field>[<r>]==<v> */
        char *s1, *s2;
        char *p;
        char ch;

        if (qualifier == NULL) {
            p = sal_strchr(tok, '=');
        } else {
            p = sal_strstr(tok, qualifier->name);
        }

        if (p == NULL) {
            rv = -1;
            break;
        }
        s1 = tok;
        s2 = (qualifier == NULL) ? p + 1 : p + sal_strlen(qualifier->name);
        *p = '\0';
        if (*s1 == '\0' || *s2 == '\0') {
            rv = -1;
            break;
        }
        /* Check the field name for each token */
        if (sal_strncasecmp(s1, name, len) != 0) {
            rv = -1;
            break;
        }
        ch = s1[len];
        if (ch != '[') {
            rv = -1;
            break;
        }
        /* Parse field array index range in <field>[r] */
        if (bcma_bcmlt_field_parse_array_index(s1, &start, &end) < 0) {
            rv = -1;
            break;
        }
        /* Check the parse result of the range of array indices */
        if (start >= arr->size || end >= arr->size || start > end) {
            rv = -1;
            break;
        }
        /* Set parsed result to elements of array field */
        for (idx = start; idx <= end; idx++) {
            arr->elements[idx] = true;
            if (is_str) {
                field_symbol_string_check(name, &s2);
                arr->u.strs[idx] = s2;
            } else {
                /* Remove underscore in s2 if any. */
                str_rm_chr(s2, '_');
                if (bcma_cli_parse_uint64(s2, &arr->u.vals[idx]) < 0) {
                    rv = -1;
                    break;
                }
            }
        }

        tok = sal_strtok_r(NULL, ",", &s);
    }

    return rv;
}

/*
 * Format field scalar value according to the display mode.
 */
static const char *
field_value_format(shr_pb_t *pb, uint64_t val,
                   bcma_bcmlt_field_display_mode_t field_disp_mode)
{
    if (field_disp_mode == BCMA_BCMLT_DISP_MODE_DEC) {
        if (val <= 0xffffffff) {
            /* Display decimal for single-word scalar */
            shr_pb_printf(pb, "%"PRIu64, val);
        } else {
            shr_pb_format_uint64(pb, NULL, &val, 1, 0);
        }
    } else {
        shr_pb_format_uint64(pb, NULL, &val, 1, 0);
        if (field_disp_mode == BCMA_BCMLT_DISP_MODE_HEX_DEC) {
            if (val >= 10 && val <= 0xffffffff) {
                /*
                 * Display both decimal and hexadecimal
                 * for single-word scalar
                 */
                shr_pb_printf(pb, "(%"PRIu64")", val);
            }
        }
    }
    return shr_pb_str(pb);
}

/*
 * Format strings in field array structure to make the indices with the same
 * symbol string or scalar value in the same group to the print buffer.
 * An example of the format is field[a,b,c-d]=<SYM_STR> or field[a-b,c,d]=<val>
 */
static const char *
field_array_format(shr_pb_t *pb, const char *prefix, const char *name,
                   bcma_bcmlt_field_array_t *arr, int is_str,
                   bcma_bcmlt_display_mode_t disp_mode,
                   const char *delim)
{
    int idx, sidx = -1;

    /*
     * Format the valid elements in an array field to be:
     * field[r1]=v1,field[r2]=v2,...,where r1 = <idx> | <idx1>-<idx2>
     */
    for (idx = 0; idx < arr->size; idx++) {
        if (!arr->elements[idx]) {
            continue;
        }
        if (sidx != -1) {
            shr_pb_printf(pb, "%c%s", disp_mode.array_delim, prefix);
        }
        sidx = idx;
        /* Get a continuous range of array elements with the same value. */
        while (++idx < arr->size && arr->elements[idx]) {
            if (is_str) {
                if (arr->u.strs[sidx] != arr->u.strs[idx]) {
                    break;
                }
            } else {
                if (arr->u.vals[sidx] != arr->u.vals[idx]) {
                    break;
                }
            }
        }
        idx--;
        /* Format field name and indices range */
        if (sidx == idx) {
            shr_pb_printf(pb, "%s[%d]%s", name, sidx, delim);
        } else {
            shr_pb_printf(pb, "%s[%d-%d]%s", name, sidx, idx, delim);
        }
        /* Format field value */
        if (is_str) {
            shr_pb_printf(pb, "%s", arr->u.strs[sidx]);
        } else {
            field_value_format(pb, arr->u.vals[sidx], disp_mode.array);
        }
    }
    return shr_pb_str(pb);
}

/*
 * Format the field array in YAML format:
 *     field_name: [val1, val2,...valn]
 */
static const char *
field_array_format_yaml(shr_pb_t *pb, const char *prefix, const char *name,
                        bcma_bcmlt_field_array_t *arr, int is_str,
                        bcma_bcmlt_display_mode_t disp_mode,
                        const char *delim)
{
    int idx;

    shr_pb_printf(pb, "%s%s[", name, delim);
    for (idx = 0; idx < arr->size; idx++) {
        if (idx) {
            shr_pb_printf(pb, ", ");
        }
        /* Format field value */
        if (is_str) {
            shr_pb_printf(pb, "%s", arr->u.strs[idx]);
        } else {
            field_value_format(pb, arr->u.vals[idx], disp_mode.array);
        }
    }
    shr_pb_printf(pb, "]");

    return shr_pb_str(pb);
}

/*
 * Format entry field name/value according to the display mode and
 * the format type.
 */
static const char *
field_format(shr_pb_t *pb, const char *prefix, bcma_bcmlt_field_t *field,
             bcma_bcmlt_display_mode_t disp_mode, int fmt_type)
{
    const char *delim = field->qlfr ? field->qlfr->name : "=";
    const char *(*field_array_fmt_fn)(shr_pb_t *, const char *, const char *,
                                      bcma_bcmlt_field_array_t *, int,
                                      bcma_bcmlt_display_mode_t, const char *);

    field_array_fmt_fn = field_array_format;
    if (fmt_type == FIELD_FMT_TYPE_YAML) {
        delim = ": ";
        field_array_fmt_fn = field_array_format_yaml;
    }

    if (!prefix) {
        prefix = "";
    }
    shr_pb_printf(pb, "%s", prefix);

    if (disp_mode.array_delim != '\n') {
        prefix = "";
    }

    if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
        bcma_bcmlt_field_array_t *arr = field->u.array;

        if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
            /* Symbol array field */
            field_array_fmt_fn(pb, prefix, field->name, arr, 1,
                               disp_mode, delim);
        } else if (field->flags & BCMA_BCMLT_FIELD_F_ARRAY) {
            /* Scalar array field */
            field_array_fmt_fn(pb, prefix, field->name, arr, 0,
                               disp_mode, delim);
        } else {
            /* Wide field */
            shr_pb_printf(pb, "%s%s", field->name, delim);
            shr_pb_format_uint64(pb, NULL, arr->u.vals, arr->size,
                                 field->width);
        }
    } else {
        /* Non-array field */
        shr_pb_printf(pb, "%s%s", field->name, delim);
        if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
            shr_pb_printf(pb, "%s", field->u.str);
        } else {
            field_value_format(pb, field->u.val, disp_mode.scalar);
        }
    }
    shr_pb_printf(pb, "%c", disp_mode.field_delim);
    return shr_pb_str(pb);
}

/*
 * Evaluate the field_value width in bits. Only support in HEX format.
 */
static int
field_value_width_get(char *field_val)
{
    if (field_val[0] =='0' &&
        (field_val[1] == 'x' || field_val[1] == 'X')) {
        char *ptr = field_val + 2;

        while (*ptr == '0') {
            ptr++;
        }
        if (*ptr) {
            return 4 * sal_strlen(ptr);
        }
        return 0;
    }

    return -1;
}

int
bcma_bcmlt_field_parse_array_index(char *str, int *sidx, int *eidx)
{
    char *ptr, *cptr;
    char ch;
    int idx = -1, start = -1, end = -1;

    if (str == NULL || sidx == NULL || eidx == NULL) {
        return -1;
    }

    *sidx = -1;
    *eidx = -1;

    /* Parse the string index specified in [a] or [a-b] */
    ptr = sal_strchr(str, '[');
    if (ptr == NULL) {
        return -1;
    }
    cptr = ptr + 1;
    do {
        ch = *cptr++;
        if (ch >= '0' && ch <= '9') {
            if (idx == -1) {
                idx = 0;
            }
            idx = (idx * 10) + (ch - '0');
        } else if (ch == '-') {
            if (idx == -1) {
                return -1;
            }
            start = idx;
            idx = -1;
        } else if (ch == ']') {
            if (idx == -1) {
                return -1;
            }
            if (start == -1) {
                start = idx;
            }
            if (end != -1) {
                return -1;
            }
            end = idx;
        } else if (ch != 0) {
            return -1;
        }
    } while (ch != 0);

    if (idx == -1 || start == -1 || end == -1) {
        return -1;
    }

    *ptr = '\0';
    *sidx = start;
    *eidx = end;

    return 0;
}

int
bcma_bcmlt_field_parse(char *field_arg, bcma_bcmlt_field_t *field)
{
    char *field_val = field_arg;
    char *ptr;
    bool is_symstr;
    const bcma_cli_parse_enum_t *qualifier;

    if (!field_arg || !field) {
        return -1;
    }

    is_symstr = field->flags & BCMA_BCMLT_FIELD_F_SYMBOL;
    qualifier = field->qlfr;
    if (qualifier && is_symstr) {
        /* Only support == and != for symbol field qualifiers */
        if (qualifier->val != BCMA_BCMLT_FIELD_QLFR_EQ &&
            qualifier->val != BCMA_BCMLT_FIELD_QLFR_NE) {
            return -1;
        }
    }

    /*
     * Check for default value setting format:
     *     <field_name>=* or qualifiers like <field_name>==*
     */
    ptr = field_arg + sal_strlen(field->name);
    if (qualifier) {
        ptr += sal_strlen(qualifier->name);
    } else {
        ptr += 1;
    }
    if (sal_strcmp(ptr, "*") == 0) {
        return bcma_bcmlt_field_default_val_set(field);
    }

    if ((field->flags & BCMA_BCMLT_FIELD_F_ARRAY) == 0) {
        /*
         * field_arg is in format of <field_name>=<field_val> or
         * qualifiers like <field_name>==<field_val>
         */
        field_val = ptr;
    }

    if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
        bcma_bcmlt_field_array_t *arr = field->u.array;

        if (field->flags & BCMA_BCMLT_FIELD_F_WIDE) {
            int numel;

            numel = bcma_cli_parse_array_uint64(field_val,
                                                arr->size, arr->u.vals);
            return numel > 0 ? 0 : -1;
        } else {
            /* Parse scalar array or symbol strings array */
            return field_array_parse(field_val, field->name,
                                     qualifier, arr, is_symstr);
        }
    }

    /* Non-array field */

    if (is_symstr) {
        field_symbol_string_check(field->name, &field_val);
        field->u.str = field_val;
        return 0;
    }

    /* Remove underscore in field_val if any. */
    str_rm_chr(field_val, '_');

    /* Return error if the value for uint64 parser is greater than 64-bit. */
    if (field_value_width_get(field_val) > 64) {
        return -1;
    }

    /* Use uint64 parser by default */
    if (bcma_cli_parse_uint64(field_val, &field->u.val) < 0) {
        return -1;
    }

    return 0;
}

const char *
bcma_bcmlt_field_format(shr_pb_t *pb, const char *prefix,
                        bcma_bcmlt_field_t *field,
                        bcma_bcmlt_display_mode_t disp_mode)
{
    if (!field) {
        return "N/A";
    }
    return field_format(pb, prefix, field, disp_mode, FIELD_FMT_TYPE_RAW);
}

const char *
bcma_bcmlt_field_format_yaml(shr_pb_t *pb, const char *prefix,
                             bcma_bcmlt_field_t *field,
                             bcma_bcmlt_display_mode_t disp_mode)
{
    if (!field) {
        return "";
    }
    return field_format(pb, prefix, field, disp_mode, FIELD_FMT_TYPE_YAML);
}
