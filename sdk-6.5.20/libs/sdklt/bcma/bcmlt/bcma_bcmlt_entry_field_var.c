/*! \file bcma_bcmlt_entry_field_var.c
 *
 * Functions related to environment variables for entry fields.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_internal.h>

#include <shr/shr_pb.h>
#include <shr/shr_pb_format.h>

#include <bcma/cli/bcma_cli_tvar.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

/* Tag value used for CLI variables of entry fields */
#define LT_VAR_TAG              SAL_SIG_DEF(0, 0, 'L', 'T')

/*
 * Get environment variable 'var_name' with LT_VAR_TAG tag.
 */
static const char *
lt_var_get(void *ev_cookie, const char *var_name)
{
    bcma_cli_t *cli = (bcma_cli_t *)ev_cookie;
    uint32_t tag = 0;
    const char *val;

    val = bcma_cli_tvar_get(cli, var_name, &tag);
    if (tag == LT_VAR_TAG) {
        return val;
    }
    return NULL;
}

/*
 * Set print buffer 'pb' contents to environment variable 'var_name'
 * with LT_VAR_TAG tag.
 */
static int
lt_var_set(void *ev_cookie, const char *var_name, shr_pb_t *pb)
{
    bcma_cli_t *cli = (bcma_cli_t *)ev_cookie;

    if (sal_strlen(shr_pb_str(pb)) > 0) {
        return bcma_cli_tvar_set(cli, var_name, shr_pb_str(pb),
                                 LT_VAR_TAG, TRUE);
    }
    return -1;
}

/*
 * Set field value to print buffer 'pb'.
 * The index value 'idx' will only be referenced for the element of array field.
 */
static void
field_val_str_set(bcma_bcmlt_field_t *field, int idx, shr_pb_t *pb)
{
    if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
        if (field->flags & BCMA_BCMLT_FIELD_F_ARRAY) {
            shr_pb_printf(pb, "%s", field->u.array->u.strs[idx]);
        } else {
            shr_pb_printf(pb, "%s", field->u.str);
        }
    } else {
        if (field->flags & BCMA_BCMLT_FIELD_F_WIDE) {
            shr_pb_format_uint64(pb, NULL, &field->u.array->u.vals[0],
                                 field->u.array->size, 0);
        } else {
            if (field->flags & BCMA_BCMLT_FIELD_F_ARRAY) {
                shr_pb_format_uint64(pb, NULL, &field->u.array->u.vals[idx],
                                     1, 0);
            } else {
                shr_pb_format_uint64(pb, NULL, &field->u.val, 1, 0);
            }
        }
    }
}

int
bcma_bcmlt_entry_field_var_set(bcma_bcmlt_entry_info_t *ei,
                               bcma_bcmlt_field_t *field, int idx)
{
    int array_name_size;
    char *array_name = NULL;
    int start = -1, end = -1, aidx;
    int rv = 0;
    shr_pb_t *pb = NULL;

    if (!ei || !field) {
        return -1;
    }

    /* Get elements indices range of array field to set for variables */
    if (field->flags & BCMA_BCMLT_FIELD_F_ARRAY) {
        if (idx >= field->u.array->size) {
            return -1;
        }
        if (idx < 0) {
            /* Set all elements of the array field */
            start = 0;
            end = field->u.array->size - 1;
        } else {
            start = idx;
            end = idx;
        }
    }

    /* Sanity check for field name has two continuous underscores */
    if (sal_strstr(field->name, "__")) {
        cli_out("%sFailed to set environment variable for field %s in "
                "table %s (field name has continuous underscores)\n",
                BCMA_CLI_CONFIG_ERROR_STR, field->name, ei->name);
        return -1;
    }

    pb = shr_pb_create();

    if (field->flags & BCMA_BCMLT_FIELD_F_ARRAY) {
        array_name_size = sal_strlen(field->name) + 8;
        array_name = sal_alloc(array_name_size, "bcmaBcmltEnvVar");
        if (array_name == NULL) {
            shr_pb_destroy(pb);
            return -1;
        }
        /* The variable name for element n of array field is <field>__n */
        for (aidx = start; aidx <= end && aidx >= 0; aidx++) {
            shr_pb_mark(pb, 0);
            shr_pb_reset(pb);
            sal_snprintf(array_name, array_name_size, "%s__%d",
                         field->name, aidx);
            field_val_str_set(field, aidx, pb);
            rv += lt_var_set(ei->ev_cookie, array_name, pb);
        }
    } else {
        /*
         * Assign 0 to array index parameter 'idx' which will not be referenced
         * for non-array fields.
         */
        field_val_str_set(field, 0, pb);
        rv = lt_var_set(ei->ev_cookie, field->name, pb);
    }

    if (array_name) {
        sal_free(array_name);
    }
    shr_pb_destroy(pb);

    return rv < 0 ? -1 : 0;
}

int
bcma_bcmlt_entry_field_var_append(bcma_bcmlt_entry_info_t *ei,
                                  bcma_bcmlt_field_t *field)
{
    const char *ov;
    shr_pb_t *pb = NULL;
    int rv;

    if (!ei || !field) {
        return -1;
    }

    /* Do not support fields with array attribute */
    if (field->flags & BCMA_BCMLT_FIELD_F_ARRAY) {
        return -1;
    }

    pb = shr_pb_create();

    ov = lt_var_get(ei->ev_cookie, field->name);
    if (ov) {
        shr_pb_printf(pb, "%s,", ov);
    }
    field_val_str_set(field, 0, pb);
    rv = lt_var_set(ei->ev_cookie, field->name, pb);

    shr_pb_destroy(pb);

    return rv;
}

int
bcma_bcmlt_entry_field_var_append_group(bcma_bcmlt_entry_info_t *ei,
                                        bcma_bcmlt_field_t **fields,
                                        int num_fields,
                                        const char *var_name)
{
    const char *ov;
    int rv, idx;
    shr_pb_t *pb = NULL;

    if (!ei || !fields || !var_name || num_fields <= 0) {
        return -1;
    }

    pb = shr_pb_create();

    ov = lt_var_get(ei->ev_cookie, var_name);
    if (ov) {
        shr_pb_printf(pb, "%s,", ov);
    }

    shr_pb_printf(pb, "{");
    for (idx = 0; idx < num_fields; idx++) {
        bcma_bcmlt_field_t *field = fields[idx];

        /* Do not support fields with array attribute */
        if (field->flags & BCMA_BCMLT_FIELD_F_ARRAY) {
            shr_pb_destroy(pb);
            return -1;
        }
        if (idx > 0) {
            shr_pb_printf(pb, ":");
        }
        field_val_str_set(field, 0, pb);
    }
    shr_pb_printf(pb, "}");

    rv = lt_var_set(ei->ev_cookie, var_name, pb);

    shr_pb_destroy(pb);

    return rv;
}

int
bcma_bcmlt_entry_field_vars_unset(bcma_bcmlt_entry_info_t *ei)
{
    bcma_cli_t *cli = (bcma_cli_t *)ei->ev_cookie;

    if (!cli) {
        return -1;
    }
    return bcma_cli_tvar_unset_by_tag(cli, LT_VAR_TAG, TRUE);
}
