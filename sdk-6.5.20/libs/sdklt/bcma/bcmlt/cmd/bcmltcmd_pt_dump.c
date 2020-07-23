/*! \file bcmltcmd_pt_dump.c
 *
 * "pt dump" command in CLI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <shr/shr_debug.h>
#include <shr/shr_pb.h>

#include <bcmdrd/bcmdrd_pt.h>

#include <bcmptm/bcmptm.h>

#include <bcma/cli/bcma_cli_ctrlc.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

#include "bcmltcmd_internal.h"

#define BSL_LOG_MODULE BSL_LS_APPL_BCMLT

/*
 * Cookie for bcma_bcmlt_tables_search() call-back function
 * pt_dump()
 */
typedef struct tbl_search_dump_data_s {
    int unit;
    bcma_bcmlt_display_mode_t disp_mode;
    uint32_t attr;
    uint32_t opts;
    bool aborted;
} tbl_search_dump_data_t;

/*!
 * \brief List PT key fields.
 *
 * \param [in] ei Entry information.
 * \param [in] prefix Prefix string to be displayed.
 * \param [in] disp_mode Field display mode.
 * \param [out] pb Print buffer.
 *
 * \retval 0 Success.
 */
static int
pt_key_list(bcma_bcmlt_entry_info_t *ei, const char *prefix,
            bcma_bcmlt_display_mode_t disp_mode, shr_pb_t *pb)
{
    int idx;

    /* The key fields are displayed in a single line */
    disp_mode.array_delim = ',';
    disp_mode.field_delim = ' ';

    shr_pb_printf(pb, "%sEntry: ", prefix ? prefix : "");
    for (idx = 0; idx < ei->num_fields; idx++) {
        bcma_bcmlt_field_t *field = &ei->fields[idx];

        bcma_bcmlt_field_format(pb, NULL, field, disp_mode);
    }
    /* Replace the last field delimiter with newline */
    shr_pb_replace_last(pb, '\n');

    return 0;
}

/*!
 * \brief Get the default field attributes of the given field name.
 *
 * \param [in] ei Entry information.
 * \param [in] field_name Field name.
 * \param [out] field Field buffer.
 *
 * \retval 0 Success.
 * \retval -1 Fail to get the field attribute.
 */
static int
field_attr_get(bcma_bcmlt_entry_info_t *ei, const char *field_name,
               bcma_bcmlt_field_t *field)
{
    int idx;

    for (idx = 0; idx < ei->num_def_fields; idx++) {
        bcmlt_field_def_t *df = &ei->def_fields[idx];

        if (sal_strcmp(field_name, df->name) == 0) {
            if (bcma_bcmlt_field_init(field, df) != 0) {
                return -1;
            }
        }
    }

    return 0;
}

/*!
 * \brief Dump physical table contents.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_name Table name to be dumped.
 * \param [in] attr Entry attributes.
 * \param [in] fields Default fields array of table.
 * \param [in] num_fields Number of default fields array of table.
 * \param [in] disp_mode Field display mode.
 * \param [in] opts Dump options.
 * \param [out] aborted Set if the operation is aborted by Ctrl-C.
 * \param [out] pt_cnt Total number of dumped entries.
 *
 * \return SHR_E_xxx.
 */
static int
pt_dump(int unit, const char *pt_name, uint32_t attr,
        bcmlt_field_def_t *fields, uint32_t num_fields,
        bcma_bcmlt_display_mode_t disp_mode, uint32_t opts,
        bool *aborted, int *pt_cnt)
{
    int cnt = 0;
    bcma_bcmlt_entry_info_t *ei = NULL;
    bcmdrd_sid_t sid;
    int idx, idx_min, idx_max, inst, inst_min, inst_max;
    int num_key_flds = 1, fidx;
    char *key_flds[] = { "BCMLT_PT_INDEX", NULL };
    bool ctrlc_aborted = false;
    shr_pb_t *pb = NULL;

    SHR_FUNC_ENTER(unit);

    cli_out("Table [%s]:\n", pt_name);

    /* Get table information */
    if (SHR_FAILURE(bcmdrd_pt_name_to_sid(unit, pt_name, &sid))) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Invalid pt: %s.\n"),
                  pt_name));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    inst_min = 0;
    inst_max = bcmdrd_pt_num_tbl_inst(unit, sid) - 1;
    idx_min = bcmptm_pt_index_min(unit, sid);
    idx_max = bcmptm_pt_index_max(unit, sid);
    if (idx_min < 0 || idx_max < 0) {
        /* Do nothing if the table does not exist from PTM perspective */
        SHR_EXIT();
    }

    /* Create an entry */
    ei = bcma_bcmlt_entry_info_create(unit, pt_name, true, attr, NULL,
                                      fields, num_fields);
    if (ei == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Failed to init table %s\n"),
                  pt_name));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Set dump options */
    ei->opts = opts;

    /* Set entry attribute */
    if (ei->attr != 0) {
        bcmlt_entry_attrib_set(ei->eh, ei->attr);
    }

    /* Get the name of the second key field */
    if (inst_max > 0) {
        if (bcmdrd_pt_is_port_reg(unit, sid) ||
            bcmdrd_pt_is_soft_port_reg(unit, sid)) {
            key_flds[1] = "BCMLT_PT_PORT";
        } else {
            key_flds[1] = "BCMLT_PT_INSTANCE";
        }
        num_key_flds++;
    }

    /* Create key fields */
    if (bcma_bcmlt_entry_fields_create(ei, ei->num_def_fields) < 0) {
        bcma_bcmlt_entry_info_destroy(ei);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Failed to create fields for table %s\n"),
                  pt_name));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    pb = shr_pb_create();
    for (idx = idx_min; idx <= idx_max && !ctrlc_aborted; idx++) {
        for (inst = inst_min; inst <= inst_max; inst++) {
            size_t key_len;

            if (bcma_cli_ctrlc_break()) {
                ctrlc_aborted = true;
                break;
            }
            if (!bcmdrd_pt_index_valid(unit, sid, inst, idx)) {
                continue;
            }

            /* Clear print buffer */
            shr_pb_mark(pb, 0);
            shr_pb_reset(pb);

            ei->num_fields = num_key_flds;
            for (fidx = 0; fidx < num_key_flds; fidx++) {
                if (field_attr_get(ei, key_flds[fidx], &ei->fields[fidx]) < 0) {
                    SHR_ERR_EXIT(SHR_E_FAIL);
                }
                if (fidx == 0) {
                    ei->fields[fidx].u.val = idx;
                } else {
                    ei->fields[fidx].u.val = inst;
                }
                bcma_bcmlt_entry_field_add(ei, &ei->fields[fidx]);
            }

            /* Add key fields into print buffer */
            if (pt_key_list(ei, "  ", disp_mode, pb) < 0) {
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
            key_len = sal_strlen(shr_pb_str(pb));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_pt_entry_commit(ei->eh, BCMLT_PT_OPCODE_GET,
                                       BCMLT_PRIORITY_NORMAL));

            /* Add data fields into print buffer */
            if (bcma_bcmlt_entry_list(ei, "    ", 0, disp_mode, pb) < 0) {
                SHR_ERR_EXIT(SHR_E_FAIL);
            }

            /* Skip entries which contain the default value when dump diff */
            if (key_len == sal_strlen(shr_pb_str(pb))) {
                continue;
            }

            cli_out("%s", shr_pb_str(pb));
            cnt++;
        }
    }

exit:
    if (aborted) {
        *aborted = ctrlc_aborted;
    }
    if (pt_cnt) {
        *pt_cnt = cnt;
    }
    if (ei) {
        bcma_bcmlt_entry_info_destroy(ei);
    }
    shr_pb_destroy(pb);

    SHR_FUNC_EXIT();
}

/*
 * Callback function of bcma_bcmlt_tables_search().
 * dump the matched table and list fields values of the dumped entry.
 */
static int
pt_match(void *cookie, const char *name,
         bcmlt_field_def_t *fields, uint32_t num_fields)
{
    tbl_search_dump_data_t *ptd = (tbl_search_dump_data_t *)cookie;
    int unit = ptd->unit;
    uint32_t attr = ptd->attr;
    uint32_t opts = ptd->opts;
    int rv;
    int cnt;

    rv = pt_dump(unit, name, attr, fields, num_fields, ptd->disp_mode, opts,
                 &ptd->aborted, &cnt);

    if (SHR_FAILURE(rv)) {
        cli_out("  "BCMA_BCMLT_ERRMSG_FMT"\n", BCMA_BCMLT_ERRMSG(rv));
    } else {
        cli_out("  %d entr%s dumped.\n",
                cnt, (cnt != 1) ? "ies" : "y");
    }

    return ptd->aborted ? -1 : 0;
}

int
bcmltcmd_pt_dump(bcma_cli_t *cli, bcma_cli_args_t *args,
                 bcma_bcmlt_display_mode_t disp_mode)
{
    int unit = cli->cmd_unit;
    tbl_search_dump_data_t ptd;
    uint32_t attr, opt_flags;
    char buf[16], *opts, opt;

    /* Check for attributes. */
    attr = bcmltcmd_parse_attr(args);

    /* Check for dump options. */
    opt_flags = 0;
    if ((opts = bcmltcmd_parse_options(args, sizeof(buf), buf)) != NULL) {
        while ((opt = *opts++) != '\0') {
            if (opt == 'd') {
                /* List fields with non-default value only. */
                opt_flags = BCMA_BCMLT_ENTRY_OP_OPT_FLD_DIFF;
            } else if (opt == 'n') {
                /* List all fields of the entry with non-default value. */
                opt_flags = BCMA_BCMLT_ENTRY_OP_OPT_ENT_DIFF;
            } else {
                cli_out("%sOption '%c' is not supported.\n",
                        BCMA_CLI_CONFIG_ERROR_STR, opt);
                return -1;
            }
        }
    }

    sal_memset(&ptd, 0, sizeof(ptd));
    ptd.unit = unit;
    ptd.disp_mode = disp_mode;
    ptd.attr = attr;
    ptd.opts = opt_flags;

    bcmltcmd_tables_match(cli, args, true, pt_match, &ptd);

    return ptd.aborted ? BCMA_CLI_CMD_INTR : BCMA_CLI_CMD_OK;
}
