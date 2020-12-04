/*! \file bcma_bcmltcmd_config.c
 *
 * CLI 'config' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_alloc.h>

#include <bcmcfg/bcmcfg_comp_scanner_traverse.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmlrd/bcmlrd_table.h>

#include <bcma/bcmlt/bcma_bcmlt.h>
#include <bcma/bcmlt/bcma_bcmltcmd_config.h>

#define INDENT_WIDTH            4

/* User data for bcma_bcmlt_table_traverse() callback function. */
typedef struct traverse_cookie_s {
    bcma_bcmlt_display_mode_t disp_mode;
    int indent;
} traverse_cookie_t;

/*
 * Callback function for bcma_bcmlt_table_traverse().
 */
static int
entry_list_yaml(void *cookie, bcma_bcmlt_entry_info_t *tei, int entry_no)
{
    traverse_cookie_t *trvs_data = (traverse_cookie_t *)cookie;
    bcma_bcmlt_display_mode_t disp_mode = trvs_data->disp_mode;
    int indent = trvs_data->indent;
    char prefix[32];

    if (entry_no == 0) {
        cli_out("%*s%s:\n", indent, "", tei->name);
    }
    sal_snprintf(prefix, sizeof(prefix), "%*s", indent + INDENT_WIDTH, "");
    return bcma_bcmlt_entry_list_yaml(tei, prefix, false, disp_mode, NULL);
}

/*
 * Display CONFIG LTs in YAML format.
 */
static int
config_lt_show(int unit)
{
    int rv = 0;
    unsigned int idx;
    bcmlrd_sid_t *table_sid;
    size_t count;
    const bcmlrd_table_rep_t *tbl;
    bcma_bcmlt_entry_info_t *ei;
    traverse_cookie_t trvs_data;
    int indent = INDENT_WIDTH;

    if (SHR_FAILURE(bcmlrd_table_count_get(unit, &count)) || count <= 0) {
        return -1;
    }
    table_sid = sal_alloc(sizeof(*table_sid) * count, "bcmaBcmltConfig");
    if (table_sid == NULL) {
        return -1;
    }
    if (SHR_FAILURE(bcmlrd_table_list_get(unit, count, table_sid, &count))) {
        sal_free(table_sid);
        return -1;
    }

    cli_out("---\n");
    cli_out("# %s\n", bcmdrd_dev_name(unit));
    cli_out("device:\n");
    cli_out("%*s%d:\n", indent, "", unit);

    trvs_data.disp_mode.scalar = BCMA_BCMLT_DISP_MODE_DEC;
    trvs_data.disp_mode.array = BCMA_BCMLT_DISP_MODE_DEC;
    trvs_data.disp_mode.array_delim = ',';
    trvs_data.disp_mode.field_delim = '\n';
    trvs_data.indent = indent + INDENT_WIDTH;

    for (idx = 0; idx < count; idx++) {
        tbl = bcmlrd_table_get(table_sid[idx]);
        if (!tbl || (tbl->flags & BCMLTD_TABLE_F_CONFIG) == 0) {
            continue;
        }
        ei = bcma_bcmlt_entry_info_create(unit, tbl->name, 0, 0, NULL, NULL, 0);
        if (ei == NULL) {
            cli_out("%sFailed to create entry information for table %s\n",
                    BCMA_CLI_CONFIG_ERROR_STR, tbl->name);
            rv = -1;
            break;
        }
        bcma_bcmlt_table_traverse(ei->unit, ei->name, ei->attr,
                                  ei->def_fields, ei->num_def_fields,
                                  entry_list_yaml, &trvs_data);
        bcma_bcmlt_entry_info_destroy(ei);
    }

    cli_out("...\n");

    sal_free(table_sid);

    return rv;
}

/*
 * Callback function for bcmcfg_comp_scanner_traverse().
 */
static int
sw_comp_node(void *user_data, const char *key, int array_idx, int indent,
             uint32_t *data, uint32_t array)
{
    uint32_t idx;
    int indent_width = (indent + 1) * INDENT_WIDTH;
    shr_pb_t *pb = (shr_pb_t *)user_data;
    const char *prefix = "";

    if (array_idx == 0) {
        prefix = "- ";
    } else if (array_idx > 0) {
        prefix = "  ";
    }

    shr_pb_printf(pb, "%*s%s%s:", indent_width, "", prefix, key);
    if (data) {
        /* Scalar node */
        if (array == 0) {
            shr_pb_printf(pb, " %"PRIu32, *data);
        } else {
            shr_pb_printf(pb, " [");
            for (idx = 0; idx < array; idx++, data++) {
                shr_pb_printf(pb, "%s%"PRIu32, idx > 0 ? ", " : "", *data);
            }
            shr_pb_printf(pb, "]");
        }
    }
    shr_pb_printf(pb, "\n");

    return indent;
}

/*
 * Display SW component configuration in YAML format.
 */
static int
sw_comp_config_show(void)
{
    shr_pb_t *pb = shr_pb_create();

    shr_pb_printf(pb, "---\n");
    shr_pb_printf(pb, "# Software component configuration\n");
    shr_pb_printf(pb, "component:\n");
    bcmcfg_comp_scanner_traverse(sw_comp_node, pb);
    shr_pb_printf(pb, "...\n");

    cli_out("%s", shr_pb_str(pb));

    shr_pb_destroy(pb);

    return 0;
}

int
bcma_bcmltcmd_config(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int u;
    int unit = cli->cmd_unit;
    char *arg = BCMA_CLI_ARG_GET(args);

    if (arg != NULL) {
        if (sal_strcmp(arg, "*") == 0) {
            unit = -1;
        } else {
            cli_out("%sInvalid argument: %s\n",
                    BCMA_CLI_CONFIG_ERROR_STR, arg);
            return BCMA_CLI_CMD_FAIL;
        }
    }

    sw_comp_config_show();

    for (u = 0; u < BCMDRD_CONFIG_MAX_UNITS; u++) {
        if (bcmdrd_dev_exists(u) && (unit == -1 || unit == u)) {
            config_lt_show(u);
        }
    }

    return BCMA_CLI_CMD_OK;
}
