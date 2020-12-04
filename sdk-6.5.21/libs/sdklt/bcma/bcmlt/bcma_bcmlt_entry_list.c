/*! \file bcma_bcmlt_entry_list.c
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
#include <shr/shr_pb_local.h>

#include <sal/sal_libc.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

/* Field flag used in entry_list() to filter the fields to be displayed */
#define ALL_FIELDS          0
#define KEY_FIELDS          BCMA_BCMLT_FIELD_F_KEY
#define NON_KEY_FIELDS      (~BCMA_BCMLT_FIELD_F_KEY)

/*
 * Get fields value for the specified table.
 */
static int
entry_fields_get(bcma_bcmlt_entry_info_t *ei, bool key_only)
{
    int idx, rv = 0;
    int fcnt = 0;
    char *key_prefix = "BCMLT_PT_";
    int len = sal_strlen(key_prefix);

    if (ei->attr & BCMLT_ENT_ATTR_FILTER_DEFAULTS) {
        rv = bcma_bcmlt_entry_fields_get(ei);
    } else {
        /* Get all fields values of a table entry. */
        for (idx = 0; idx < ei->num_def_fields; idx++) {
            bcmlt_field_def_t *def_field = &ei->def_fields[idx];
            bcma_bcmlt_field_t *field = &ei->fields[fcnt];

            /*
             * Skip PT key fields that start with "BCMLT_PT_" because
             * these are not real physical table fields, so they should be
             * omitted from the PT output.
             */
            if (ei->pt && def_field->key &&
                sal_strncmp(def_field->name, key_prefix, len) == 0) {
                continue;
            }

            /* List for key fields only. */
            if (key_only && !def_field->key) {
                continue;
            }

            if (bcma_bcmlt_field_init(field, def_field) != 0) {
                return -1;
            }
            /* Accept E_NOT_FOUND for non key fields. */
            if (bcma_bcmlt_entry_field_get(ei, field, !def_field->key) != 0) {
                rv = -1;
            }
            fcnt++;
        }
        ei->num_fields = fcnt;
    }

    return rv;
}

/*
 * List fields value for the specified table.
 */
static int
entry_list(shr_pb_t *pb, bcma_bcmlt_entry_info_t *ei, const char *prefix,
           uint32_t fflag, bcma_bcmlt_display_mode_t disp_mode,
           const char *(*ffmt_fn)(shr_pb_t *, const char *prefix,
                                  bcma_bcmlt_field_t *,
                                  bcma_bcmlt_display_mode_t))
{
    bool is_diff = false;
    int idx, opt_diff;

    shr_pb_mark(pb, -1);
    opt_diff = BCMA_BCMLT_ENTRY_OP_OPT_ENT_DIFF |
               BCMA_BCMLT_ENTRY_OP_OPT_FLD_DIFF;

    for (idx = 0; idx < ei->num_fields; idx++) {
        bcma_bcmlt_field_t *field = &ei->fields[idx];

        /* Skip unused fields */
        if (field->name == NULL) {
            continue;
        }

        /* List for the specified fields only */
        if (fflag != ALL_FIELDS &&
            (fflag & KEY_FIELDS) != (field->flags & BCMA_BCMLT_FIELD_F_KEY)) {
            continue;
        }

        /* Check if the field is different from default value */
        if (ei->opts & opt_diff) {
            if (bcma_bcmlt_field_default_val_cmp(field) != 0) {
                is_diff = true;
            } else {
                if (ei->opts & BCMA_BCMLT_ENTRY_OP_OPT_FLD_DIFF) {
                    /* Skip the field with default value */
                    continue;
                }
            }
        }

        /*
         * Print the prefix for once if the fields are displayed in
         * single-line style
         */
        if (prefix && disp_mode.field_delim != '\n') {
            shr_pb_printf(pb, "%s", prefix);
            prefix = NULL;
        }
        /* Field format for name and value */
        ffmt_fn(pb, prefix, field, disp_mode);
    }

    /*
     * Replace the last field delimiter with newline if the fields are
     * displayed in single-line style
     */
    if (disp_mode.field_delim != '\n') {
        shr_pb_replace_last(pb, '\n');
    }

    /* Clear output if nothing is different from the default */
    if ((ei->opts & opt_diff) && !is_diff) {
        shr_pb_reset(pb);
    }

    return 0;
}

int
bcma_bcmlt_entry_list(bcma_bcmlt_entry_info_t *ei, const char *prefix,
                      bool key_only, bcma_bcmlt_display_mode_t disp_mode,
                      shr_pb_t *pb)
{
    int rv;
    uint32_t fflag;

    if (!ei) {
        return -1;
    }

    rv = entry_fields_get(ei, key_only);
    if (rv == 0) {
        SHR_PB_LOCAL_DECL(pb);

        fflag = key_only ? KEY_FIELDS : ALL_FIELDS;
        entry_list(pb, ei, prefix, fflag, disp_mode,
                   bcma_bcmlt_field_format);

        /* Display output if pb was NULL */
        SHR_PB_LOCAL_DONE_VIA_FUNC(cli_out);
    }
    return rv;
}

int
bcma_bcmlt_entry_list_yaml(bcma_bcmlt_entry_info_t *ei, const char *prefix,
                           bool key_only, bcma_bcmlt_display_mode_t disp_mode,
                           shr_pb_t *pb)
{
    int rv;

    if (!ei) {
        return -1;
    }

    rv = entry_fields_get(ei, key_only);
    if (rv == 0) {
        SHR_PB_LOCAL_DECL(pb);

        if (ei->num_key_fields == 0) {
            /* List all fields for LTs with no key fields */
            entry_list(pb, ei, prefix, ALL_FIELDS, disp_mode,
                       bcma_bcmlt_field_format_yaml);
        } else {
            char nlvl[32];

            sal_snprintf(nlvl, sizeof(nlvl), "%s    ", prefix ? prefix : "");
            /* List key fields */
            shr_pb_printf(pb, "%s?\n", prefix ? prefix : "");
            entry_list(pb, ei, nlvl, KEY_FIELDS, disp_mode,
                       bcma_bcmlt_field_format_yaml);
            /* List non-key fields */
            shr_pb_printf(pb, "%s:\n", prefix ? prefix : "");
            entry_list(pb, ei, nlvl, NON_KEY_FIELDS, disp_mode,
                       bcma_bcmlt_field_format_yaml);
        }

        /* Display output if pb was NULL */
        SHR_PB_LOCAL_DONE_VIA_FUNC(cli_out);
    }
    return rv;
}
