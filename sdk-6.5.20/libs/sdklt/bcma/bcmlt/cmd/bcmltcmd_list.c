/*! \file bcmltcmd_list.c
 *
 * Sub-command 'list' of table access commands in CLI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <shr/shr_debug.h>
#include <bcma/cli/bcma_cli_ctrlc.h>
#include <bcma/bcmlt/bcma_bcmlt.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlrd/bcmlrd_client.h>
#include "bcmltcmd_internal.h"

#define DESC_PREFIX_LEN_MAX         1024
/*! Maximum length of PT map information string. */
#define BCMLT_MAPPING_STR_LEN_MAX 400

#define BSL_LOG_MODULE BSL_LS_APPL_BCMLT

/*!
 * \brief Arguments for table list callback function.
 *
 * This structure is used to store unit and flag for the
 * callback function used to list the LT table and field.
 * This would be passed as cookie information for the callback function.
 */
typedef struct table_list_args_s {
    /* Device unit number */
    int unit;

    /* Option to indicate the level of information */
    int cb_flag;

    /* True if aborted by Ctrl-C signal */
    bool aborted;

} table_list_args_t;

/* Format the PT information for each LT field. */
static void
field_pt_info(shr_pb_t *pb,
              const char *pf_str,
              int unit,
              const char *tbl_name,
              const char *fld_name)
{
    const char *prefix = pf_str ? pf_str : "";
    int rv;
    uint32_t idx;
    uint32_t ltid;
    uint32_t lfid;
    uint32_t num_entries = 0, ret_entries;
    bcmlrd_pt_map_info_t *pt_maps = NULL;

    do {
        if (pb == NULL ||
            fld_name == NULL ||
            tbl_name == NULL) {
            break;
        }

        rv = bcmlt_table_field_id_get_by_name(unit,
                                              tbl_name,
                                              fld_name,
                                              &ltid,
                                              &lfid);
        if (SHR_FAILURE(rv)) {
            break;
        }

        /* Get number of mapped entries. */
        rv = bcmlrd_field_pt_map_info_num_get(unit,
                                              ltid,
                                              lfid,
                                              &num_entries);
        if (SHR_FAILURE(rv) || num_entries == 0) {
            break;
        }

        pt_maps = sal_alloc(sizeof(bcmlrd_pt_map_info_t) * num_entries,
                            "bcmaBcmltPtMapInfos");
        sal_memset(pt_maps, 0x0, sizeof(bcmlrd_pt_map_info_t) * num_entries);

        rv = bcmlrd_field_pt_map_info_get(unit,
                                          ltid,
                                          lfid,
                                          num_entries,
                                          &ret_entries,
                                          pt_maps);
        if (SHR_FAILURE(rv)) {
            break;
        }

        for (idx = 0; idx < ret_entries; idx++) {
            bcmlrd_pt_map_info_t *map = &pt_maps[idx];
            uint32_t pt_tid = map->pt_id;
            uint32_t pt_fid = map->field.field_id;
            const char *pt_name = NULL;
            char dest[BCMLT_MAPPING_STR_LEN_MAX];

            if (map->int_field_type &&
                sal_strlen(map->int_field_type) != 0) {
                shr_pb_printf(pb, "%sHandler: %s (%s)\n",
                              prefix,
                              map->map_type_name,
                              map->int_field_type);
            } else {
                shr_pb_printf(pb, "%sHandler: %s\n",
                              prefix,
                              map->map_type_name);
            }

            pt_name = bcmdrd_pt_sid_to_name(unit, pt_tid);
            if (bcmlrd_field_is_magic_field(unit, pt_fid)) {
                const char *magic_name = NULL;
                magic_name = bcmlrd_field_magic_field_name_get(unit, pt_fid);
                sal_snprintf(dest, sizeof(dest), "%s.%s", pt_name, magic_name);
            } else if (bcmlrd_field_is_internal_field(unit, pt_fid)) {
                sal_snprintf(dest, sizeof(dest), "%s", map->field.sym);
            } else {
                bcmdrd_sym_field_info_t finfo;
                rv = bcmdrd_pt_field_info_get(unit, pt_tid, pt_fid, &finfo);
                if (SHR_FAILURE(rv)) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "Failed to get PT %s field for "
                                            "LT map: %s.%s\n"),
                                 pt_name, tbl_name, fld_name));
                    break;
                }
                sal_snprintf(dest, sizeof(dest), "%s.%sf", pt_name, finfo.name);
            }
            shr_pb_printf(pb, "%s  Map: %s\n", prefix, dest);
            if ((!sal_strcmp(map->map_type_name, "Direct")) ||
                (!sal_strcmp(map->map_type_name, "Transform")) ||
                (!sal_strcmp(map->map_type_name, "Table Transform"))) {
                shr_pb_printf(pb, "%s  Bit (min, max): %d, %d\n",
                              prefix, map->field.minbit, map->field.maxbit);
            }
        }
        if (SHR_FAILURE(rv)) {
            break;
        }
    } while (0);

    if (pt_maps) {
        sal_free(pt_maps);
    }

    return;
}

static int
pt_info_format(shr_pb_t *pb, int unit, const char *tbl_name, uint32_t *count)
{
    uint32_t pt_count = 0;
    uint32_t pt_max_count = 0;
    const char **pt_names = NULL;
    unsigned int size = 0;

    SHR_FUNC_ENTER(unit);

    do {
        SHR_IF_ERR_EXIT(
            bcmlt_table_pt_name_max_num_get(unit, tbl_name, &pt_max_count));

        if (pt_max_count > 0) {
            size = sizeof(const char *) * pt_max_count;
            SHR_ALLOC(pt_names, size, "bcmaBcmltPTInfo");
            SHR_NULL_CHECK(pt_names, SHR_E_MEMORY);
            sal_memset(pt_names, 0, size);

            SHR_IF_ERR_EXIT(
                bcmlt_table_pt_name_get(unit, tbl_name, pt_max_count, pt_names,
                                        &pt_count));
        }

        /* Format PT information. */
        shr_pb_printf(pb, "  PT Reference:\n");
        if (pt_count) {
            bcma_bcmlt_format_strs_array(pb, "    ", pt_names, pt_count);
        } else {
            shr_pb_printf(pb, "    n/a\n");
        }

        *count = pt_count;
    } while (0);

exit:
    SHR_FREE(pt_names);

    SHR_FUNC_EXIT();
}

static void
display_description(shr_pb_t *pb, int unit,
                    const char *tbl_name,
                    const char *fld_name,
                    const char *prefix)
{
    const char *desc = NULL;
    const char *desc_prefix = "Description: ";
    const char *ptr;
    int rv;
    int indent_len;

    if (fld_name == NULL) {
        rv = bcmlt_table_desc_get(unit, tbl_name, &desc);
    } else {
        rv = bcmlt_field_desc_get(unit, tbl_name, fld_name, &desc);
    }

    if (rv != SHR_E_NONE) {
        return;
    }

    shr_pb_printf(pb, "%s%s", prefix, desc_prefix);
    indent_len = sal_strlen(prefix) + sal_strlen(desc_prefix);

    /* Handle NULL description. */
    if (desc == NULL || desc[0] == '\0') {
        shr_pb_printf(pb, "<none>\n");
        return;
    }

    ptr = desc;
    while (*ptr) {
        /* Skip the ending new line */
        if (*ptr == '\n' && *(ptr + 1) == '\0') {
            break;
        }
        shr_pb_printf(pb, "%c", *ptr);
        /* Pad indent spaces for new line */
        if (*ptr == '\n') {
            shr_pb_printf(pb, "%*s", indent_len, "");
            /* Skip the spaces after new line */
            while (*(ptr + 1) == ' ') {
                ptr++;
            }
        }
        ptr++;
    }
    shr_pb_printf(pb, "\n");
}

/*
 * Format the specified table and its fields to the print buffer.
 */
static void
default_fields_format(shr_pb_t *pb, uint32_t flag,
                      int unit, const char *tbl_name,
                      bcmlt_field_def_t *fields, uint32_t num_fields)
{
    uint32_t idx, num_key_fields;
    bcmlt_field_def_t *df;
    uint32_t pt_count = 0;
    int rv = SHR_E_NONE;
    const char *prefix = "        ";

    shr_pb_printf(pb, "%s\n", tbl_name);

    /* Display description if requested. */
    if (flag & BCMA_BCMLT_TBL_LIST_OPT_DESC) {
        display_description(pb, unit, tbl_name, NULL, "  ");
    }

    /* Display PT information if requested. */
    if (flag & BCMA_BCMLT_TBL_LIST_OPT_FIELD_PT_INFO) {
        rv = pt_info_format(pb, unit, tbl_name, &pt_count);
        if (SHR_FAILURE(rv)) {
            return;
        }
    }

    /* Get the number key fields */
    num_key_fields = 0;
    for (idx = 0; idx < num_fields; idx++) {
        df = &fields[idx];
        if (df->key) {
            num_key_fields++;
        }
    }

    shr_pb_printf(pb, "  %d field%s (%d key-type field%s):\n",
                  (int)num_fields, num_fields > 1 ? "s" : "",
                  (int)num_key_fields, num_key_fields > 1 ? "s" : "");

    for (idx = 0; idx < num_fields; idx++) {
        df = &fields[idx];
        shr_pb_printf(pb, "    %s", df->name);
        if (df->key && (flag & BCMA_BCMLT_TBL_LIST_OPT_FIELD_INFO) == 0) {
            /* Flag the key fields when '-l' option is not specified */
            shr_pb_printf(pb, " (key)");
        }
        shr_pb_printf(pb, "\n");
        /* Display description if requested. */
        if (flag & BCMA_BCMLT_TBL_LIST_OPT_DESC) {
            display_description(pb, unit, tbl_name, df->name, prefix);
        }
        if (flag & BCMA_BCMLT_TBL_LIST_OPT_FIELD_INFO) {
            bcma_bcmlt_entry_field_default_info_format(pb, prefix,
                                                       unit, tbl_name, df);
        }
        /* Display PT information if requested. */
        if ((flag & BCMA_BCMLT_TBL_LIST_OPT_FIELD_PT_INFO) && pt_count != 0) {
            field_pt_info(pb, prefix, unit, tbl_name, df->name);
        }
    }

    return;
}

/*
 * bcma_bcmlt_tables_search callback function.
 * List table name, fields names and fields information.
 */
static int
table_list(void *cookie, const char *tbl_name,
           bcmlt_field_def_t *fields, uint32_t num_fields)
{
    table_list_args_t *args;
    int unit, flag;
    shr_pb_t *pb;

    args = (table_list_args_t *)cookie;
    unit = args->unit;
    flag = args->cb_flag;

    if (bcma_cli_ctrlc_break()) {
        args->aborted = true;
        return -1;
    }

    pb = shr_pb_create();

    default_fields_format(pb, flag, unit,
                          tbl_name, fields, num_fields);

    cli_out("%s", shr_pb_str(pb));

    shr_pb_destroy(pb);

    return 0;
}

/*
 * bcma_bcmlt_tables_search callback function.
 * List table name in raw.
 */
static int
table_list_raw(void *cookie, const char *tbl_name,
               bcmlt_field_def_t *fields, uint32_t num_fields)
{
    table_list_args_t *args = (table_list_args_t *)cookie;

    if (bcma_cli_ctrlc_break()) {
        args->aborted = true;
        return -1;
    }

    cli_out("%s\n", tbl_name);

    return 0;
}

int
bcmltcmd_tables_list(bcma_cli_t *cli, bcma_cli_args_t *args, bool physical)
{
    int unit = cli->cmd_unit;
    char *opts;
    char buf[16], opt;
    bcma_bcmlt_table_info_f cb = table_list;
    table_list_args_t list_args;

    /* Initialize arguments for table list callback function. */
    sal_memset(&list_args, 0, sizeof(list_args));
    list_args.unit = unit;

    /* Check for list options. */
    if ((opts = bcmltcmd_parse_options(args, sizeof(buf), buf)) != NULL) {
        bool tbl_name_only = false;
        while ((opt = *opts++) != '\0') {
            if (opt == 'b') {
                /* List table names only. */
                tbl_name_only = true;
            } else if (opt == 'l') {
                list_args.cb_flag |= BCMA_BCMLT_TBL_LIST_OPT_FIELD_INFO;
            } else if (opt == 'm') {
                list_args.cb_flag |= BCMA_BCMLT_TBL_LIST_OPT_FIELD_PT_INFO;
            } else if (opt == 'd') {
                list_args.cb_flag |= BCMA_BCMLT_TBL_LIST_OPT_DESC;
            } else {
                cli_out("%sOption '%c' is not supported.\n",
                        BCMA_CLI_CONFIG_ERROR_STR, opt);
                return -1;
            }
        }
        if (list_args.cb_flag & BCMA_BCMLT_TBL_LIST_OPT_FIELD_INFO ||
            list_args.cb_flag & BCMA_BCMLT_TBL_LIST_OPT_DESC ||
            list_args.cb_flag & BCMA_BCMLT_TBL_LIST_OPT_FIELD_PT_INFO) {
            if (tbl_name_only) {
                cli_out("Warning : Ignoring option 'b'\n");
            }
        } else if (tbl_name_only) {
            cb = table_list_raw;
        }
    }

    bcmltcmd_tables_match(cli, args, physical, cb, &list_args);

    return list_args.aborted ? BCMA_CLI_CMD_INTR : BCMA_CLI_CMD_OK;
}
