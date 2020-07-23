/*! \file bcmltcmd_pt_reset.c
 *
 * "pt reset" command in CLI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <shr/shr_error.h>
#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_pt.h>

#include <bcmptm/bcmptm.h>

#include <bcma/cli/bcma_cli_ctrlc.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

#include "bcmltcmd_internal.h"

#define BSL_LOG_MODULE BSL_LS_APPL_BCMLT


/*
 * Cookie for bcma_bcmlt_tables_search() call-back function
 * pt_reset()
 */
typedef struct tbl_search_reset_data_s {
    int unit;
    uint32_t attr;
    bool aborted;
} tbl_search_reset_data_t;

/*
 * Structure to store the key field position in bcma_bcmlt_entry_info_t.
 */
typedef struct kfld_info_s {
    /* The field index of "BCMLT_PT_INDEX". */
    int fld_idx;

    /* The field index of "BCMLT_PT_INSTANCE" or "BCMLT_PT_PORT". */
    int fld_inst;
} kfld_info_t;

/*!
 * \brief Initialize the fields buffer of the entry.
 *
 * \param [in] ei Entry information.
 * \param [in] reset True to reset the entry to default.
 *                   False to clear the entry to 0s.
 * \param [out] kfld_info Key field information in the entry buffer.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_FAIL Fail to initialize.
 */
static int
entry_fields_init(bcma_bcmlt_entry_info_t *ei, bool reset,
                  kfld_info_t *kfld_info)
{
    int idx;

    kfld_info->fld_idx = -1;
    kfld_info->fld_inst = -1;
    ei->num_fields = ei->num_def_fields;
    for (idx = 0; idx < ei->num_def_fields; idx++) {
        bcma_bcmlt_field_t *fld = &ei->fields[idx];
        bcmlt_field_def_t *df = &ei->def_fields[idx];

        if (bcma_bcmlt_field_init(fld, df) != 0) {
            return SHR_E_FAIL;
        }

        if (reset) {
            /* Skip wide-field, because its default value is not available */
            if (fld->flags & BCMA_BCMLT_FIELD_F_WIDE) {
                continue;
            }
            if (bcma_bcmlt_field_default_val_set(fld) != 0) {
                return SHR_E_FAIL;
            }
        }

        if (bcma_bcmlt_entry_field_add(ei, fld) != 0) {
            return SHR_E_FAIL;
        }

        if (fld->flags & BCMA_BCMLT_FIELD_F_KEY &&
            sal_strncmp(df->name, "BCMLT_PT_", 9) == 0) {
            if (sal_strcmp(df->name + 9, "INDEX") == 0) {
                kfld_info->fld_idx = idx;
            } else {
                kfld_info->fld_inst = idx;
            }
        }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Set the table entries to default or all 0s.
 *
 * \param [in] ei Entry information.
 * \param [in] reset True to reset the table to default.
 *                   False to clear the table to 0s.
 * \param [out] aborted Set if the operation is aborted by Ctrl-C.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_FAIL Fail to initialize.
 */
static int
pt_set(bcma_bcmlt_entry_info_t *ei, bool reset, bool *aborted)
{
    int unit = ei->unit;
    const char *pt_name = ei->name;
    bcmdrd_sid_t sid;
    int idx, idx_min, idx_max, inst, inst_min, inst_max;
    kfld_info_t kfld_info;
    bool ctrlc_aborted = false;

    SHR_FUNC_ENTER(unit);

    /* Get table infomation */
    if (SHR_FAILURE(bcmdrd_pt_name_to_sid(unit, pt_name, &sid))) {
        cli_out("%sInvalid pt: %s.\n",
                BCMA_CLI_CONFIG_ERROR_STR, pt_name);
        SHR_EXIT();
    }

    inst_min = 0;
    inst_max = bcmdrd_pt_num_tbl_inst(unit, sid) - 1;
    idx_min = bcmptm_pt_index_min(unit, sid);
    idx_max = bcmptm_pt_index_max(unit, sid);
    if (idx_min < 0 || idx_max < 0) {
        /* Do nothing if the table does not exist from PTM perspective */
        SHR_EXIT();
    }

    /* Initialize field buffer */
    SHR_IF_ERR_EXIT
        (entry_fields_init(ei, reset, &kfld_info));

    /* Set table entries */
    for (idx = idx_min; idx <= idx_max && !ctrlc_aborted; idx++) {
        for (inst = inst_min; inst <= inst_max; inst++) {
            if (bcma_cli_ctrlc_break()) {
                ctrlc_aborted = true;
                SHR_ERR_EXIT(SHR_E_FAIL);
            }

            if (!bcmdrd_pt_index_valid(unit, sid, inst, idx)) {
                continue;
            }

            if (kfld_info.fld_idx >= 0) {
                ei->fields[kfld_info.fld_idx].u.val = idx;
                bcma_bcmlt_entry_field_add(ei, &ei->fields[kfld_info.fld_idx]);
            }
            if (kfld_info.fld_inst >= 0) {
                ei->fields[kfld_info.fld_inst].u.val = inst;
                bcma_bcmlt_entry_field_add(ei, &ei->fields[kfld_info.fld_inst]);
            }

            SHR_IF_ERR_EXIT
                (bcmlt_pt_entry_commit(ei->eh, BCMLT_PT_OPCODE_SET,
                                       BCMLT_PRIORITY_NORMAL));
        }
    }

exit:
    if (aborted) {
        *aborted = ctrlc_aborted;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Reset physical table contents.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_name Table name to be reset.
 * \param [in] attr Entry attributes.
 * \param [in] fields Default fields array of table.
 * \param [in] num_fields Number of default fields array of table.
 * \param [out] aborted Set if the operation is aborted by Ctrl-C.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failed to reset the table.
 */
static int
pt_reset(int unit, const char *pt_name, uint32_t attr,
        bcmlt_field_def_t *fields, uint32_t num_fields,
        bool *aborted)
{
    bcma_bcmlt_entry_info_t *ei = NULL;

    SHR_FUNC_ENTER(unit);

    /* Create an entry */
    ei = bcma_bcmlt_entry_info_create(unit, pt_name, true, attr, NULL,
                                      fields, num_fields);
    if (ei == NULL) {
        cli_out("%sFail to init table %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, pt_name);
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Set entry attribute */
    if (ei->attr != 0) {
        bcmlt_entry_attrib_set(ei->eh, ei->attr);
    }

    /* Create entry fields */
    if (bcma_bcmlt_entry_fields_create(ei, ei->num_def_fields) < 0) {
        cli_out("%sFail to create table fields %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, pt_name);
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Clear table entries first */
    SHR_IF_ERR_EXIT
        (pt_set(ei, false, aborted));

    /* Set table entries to default */
    SHR_IF_ERR_EXIT
        (pt_set(ei, true, aborted));

exit:
    if (ei) {
        bcma_bcmlt_entry_info_destroy(ei);
    }

    SHR_FUNC_EXIT();
}

/*
 * Callback function of bcma_bcmlt_tables_search().
 */
static int
pt_match(void *cookie, const char *name,
         bcmlt_field_def_t *fields, uint32_t num_fields)
{
    tbl_search_reset_data_t *ptd = cookie;
    int rv, unit = ptd->unit;
    uint32_t attr = ptd->attr;

    rv = pt_reset(unit, name, attr, fields, num_fields, &ptd->aborted);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFail to reset %s.\n",
                BCMA_CLI_CONFIG_ERROR_STR, name);
    } else {
        cli_out("%s is reset.\n", name);
    }

    return ptd->aborted ? -1 : 0;
}

int
bcmltcmd_pt_reset(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cmd_unit;
    tbl_search_reset_data_t ptd;
    uint32_t attr;

    /* Check for attributes. */
    attr = bcmltcmd_parse_attr(args);

    /* Need explicitly specifiy the table-match-string */
    if (BCMA_CLI_ARG_CNT(args) == 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    sal_memset(&ptd, 0, sizeof(ptd));
    ptd.unit = unit;
    ptd.attr = attr;
    bcmltcmd_tables_match(cli, args, true, pt_match, &ptd);

    return ptd.aborted ? BCMA_CLI_CMD_INTR : BCMA_CLI_CMD_OK;
}
