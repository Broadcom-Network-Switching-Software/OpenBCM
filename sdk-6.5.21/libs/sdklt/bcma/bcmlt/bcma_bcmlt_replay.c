/*! \file bcma_bcmlt_replay.c
 *
 * Logical table replay API.
 *
 * The LT replay API utilizes the BCMLT replay engine and implements
 * the "replay" functionality.
 *
 * The replay API provides default callback for the BCMLT replay API to
 * let the replay engine read the LT operations from a file created
 * by the LT capture API.
 * Default replay action is also provided by the replay API to decode the
 * captured data to an entry-based or a transaction-based operation in
 * CLI text command fomrat.
 *
 * Note that the replay API will start separate thread for individual unit.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>

#include <shr/shr_debug.h>
#include <shr/shr_thread.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcma/io/bcma_io_file.h>
#include <bcma/bcmlt/bcma_bcmlt.h>
#include <bcma/bcmlt/bcma_bcmlt_enum_map.h>
#include <bcma/bcmlt/bcma_bcmlt_capture_replay.h>

#define BSL_LOG_MODULE BSL_LS_APPL_BCMLT

/*******************************************************************************
 * Local definitions
 */

/*
 * Replay action configuration for default implementaion of BCMLT replay action
 * callback replay_action() which decodes the replay information to CLI LT
 * commands.
 */
typedef struct replay_action_cfg_s {

    /*
     * If not NULL, the decoded CLI LT commands will be written to the
     * file handle.
     */
    bcma_io_file_handle_t ofh;

    /*
     * The decoded CLI LT commands will be displayed on console if set to true.
     */
    bool console_output;

    /*
     * If set to true, the decoded CLI commands will be in raw staged result.
     * Otherwise the the CLI friendly command will be generated for the replay
     * to run without errors.
     * Note that raw=0 is supported only in dry-run mode (submit=0).
     */
    bool raw;

} replay_action_cfg_t;

/* Replay information */
typedef struct replay_info_s {

    /* Unit number */
    int unit;

    /*
     * File handle to read data for BCMLT replay engine.
     * This is used for default bcmlt_replay_cb_t 'cb' implementaion.
     */
    bcma_io_file_handle_t ifh;

    /*
     * Callback function struct for BCMLT replay engine.
     * The default implementation here is replay_read() which reads data from
     * file handle 'ifh'.
     */
    bcmlt_replay_cb_t cb;

    /*
     * Replay action callback function for BCMLT replay engine.
     * The default implementaion here is replay_action() which decodes the
     * replay information to CLI LT commands.
     */
    bcmlt_replay_action_f act;

    /*
     * Replay action configuration which is only applicable when 'act' is
     * default implementation replay_action().
     */
    replay_action_cfg_t *act_cfg;

    /*
     * The original capture timeing will be reference in replay if set to true.
     */
    bool timing;

    /*
     * The replay will take effect if set to true.
     */
    bool submit;

    /*
     * Handle of the replay thread.
     */
    shr_thread_t th;

    /*
     * Callback function when the replay thread is done.
     */
    bcma_bcmlt_replay_done_f rp_done_cb;

} replay_info_t;

static replay_info_t *replay_info[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*
 * Format the field information of the replayed data.
 */
static int
fields_show(shr_pb_t *pb, bcmlt_entry_info_t *bcmlt_ei, bool physical)
{
    int rv;
    int idx;
    bcma_bcmlt_entry_info_t *ei;
    bcma_bcmlt_display_mode_t disp_mode;

    disp_mode.scalar = BCMA_BCMLT_DISP_MODE_HEX;
    disp_mode.array = BCMA_BCMLT_DISP_MODE_HEX;
    disp_mode.array_delim = ',';
    disp_mode.field_delim = ' ';

    /*
     * When the LT replay engine is launched in "real-time verbose"
     * mode, a call to bcma_bcmlt_entry_info_create will block in
     * bcma_bcmlt_tables_search if executing an "lt traverse" command,
     * because the this command uses bcma_bcmlt_tables_search as
     * well. Instead we call bcma_bcmlt_entry_info_create_exact_name
     * to avoid the blocking issue.
     */
    ei = bcma_bcmlt_entry_info_create_exact_name(bcmlt_ei->unit,
                                                 bcmlt_ei->table_name,
                                                 physical, 0, NULL, NULL, 0);
    if (ei == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to create entry information for "
                            "table %s.\n"), bcmlt_ei->table_name));
        return -1;
    }

    /* Free the unused entry handle */
    bcmlt_entry_free(ei->eh);

    /* Set the entry handle from BCMLT */
    ei->eh = bcmlt_ei->entry_hdl;

    rv = bcma_bcmlt_entry_fields_get(ei);

    /* Unset the entry handle to avoid being freed in the destroy function. */
    ei->eh = 0;

    if (rv < 0) {
        bcma_bcmlt_entry_info_destroy(ei);
        return -1;
    }

    /* Format the assigned value for fields. */
    for (idx = 0; idx < ei->num_def_fields; idx++) {
        bcma_bcmlt_field_t *field = &ei->fields[idx];

        if (field->name) {
            bcma_bcmlt_field_format(pb, NULL, field, disp_mode);
        }
    }
    /* Remove the last field delimiter */
    shr_pb_replace_last(pb, 0);

    bcma_bcmlt_entry_info_destroy(ei);
    return 0;
}

/*
 * Format the entry information of the replayed data.
 */
static int
entry_format(shr_pb_t *pb, const char *prefix, bcmlt_entry_info_t *bcmlt_ei,
             int *opcode, bool *logical)
{
    int rv;
    uint32_t attr = 0;
    bool physical;
    const char *opcstr;
    int opc;

    rv = bcmlt_entry_attrib_get(bcmlt_ei->entry_hdl, &attr);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to get table %s entry attribute: "
                            "%s (%d).\n"),
                   bcmlt_ei->table_name, shr_errmsg(rv), rv));
        return -1;
    }

    opcstr = bcma_bcmlt_opcode_format_by_entry_info(bcmlt_ei, &opc, &physical);
    if (opcstr == NULL) {
        return -1;
    }

    if (prefix != NULL) {
        shr_pb_printf(pb, "%s", prefix);
    }
    shr_pb_printf(pb, "%s", bcmlt_ei->table_name);
    if (attr != 0) {
        int num_attrs = 0, match = 0;
        const bcma_cli_parse_enum_t *attrs = bcma_bcmlt_ent_attr_enum_get();

        shr_pb_printf(pb, " attr=");
        while (attrs[num_attrs].name != NULL) {
            if ((attr & attrs[num_attrs].val) != 0) {
                if (match) {
                    shr_pb_printf(pb, ",");
                }
                shr_pb_printf(pb, "%s", attrs[num_attrs].name);
                match = 1;
            }
            num_attrs++;
        };
    }
    shr_pb_printf(pb, " %s ", opcstr);
    if (fields_show(pb, bcmlt_ei, physical) != 0) {
        return -1;
    }

    if (opcode) {
        *opcode = opc;
    }
    if (logical) {
        *logical = !physical;
    }

    return 0;
}

/*
 * Format the transaction type of the CLI command for the replayed operation.
 */
static int
command_trans_type_format(shr_pb_t *pb, const char *prefix,
                          bcmlt_transaction_info_t *bcmlt_ti)
{
    if (prefix != NULL) {
        shr_pb_printf(pb, "%s", prefix);
    }

    /* Format transaction type */
    if (bcmlt_ti) {
        shr_pb_printf(pb, "tr=1 tt=%s",
                      bcma_bcmlt_trans_type_format(bcmlt_ti->type));
    } else {
        shr_pb_printf(pb, "tr=0");
    }

    /* Only synchronous mode is supported in the replay. */
    shr_pb_printf(pb, " a=0");

    return 0;
}

/*
 * Format the CLI command of the replayed operation.
 */
static int
command_format(shr_pb_t * pb, const char *prefix, bcmlt_entry_info_t *bcmlt_ei)
{
    int rv;
    bcmlt_unified_opcode_t uopc;

    rv = bcmlt_entry_oper_get(bcmlt_ei->entry_hdl, &uopc);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to get table %s entry op code: "
                            "%s (%d).\n"),
                   bcmlt_ei->table_name, shr_errmsg(rv), rv));
        return -1;
    }
    if (prefix != NULL) {
        shr_pb_printf(pb, "%s", prefix);
    }
    shr_pb_printf(pb, "%d:%s", bcmlt_ei->unit, uopc.pt ? "pt" : "lt");

    return 0;
}

/*
 * Format the entry replayed data to human-readable CLI command.
 */
static void
entry_commit_replay(bcmlt_entry_handle_t eh, const char *prefix,
                    bool verbose, bcma_io_file_handle_t fh, bool raw)
{
    int rv;
    bcmlt_entry_info_t bcmlt_ei;
    shr_pb_t *pb;
    int opcode;
    bool logical;
    bool cmd_out = true;

    if (eh == BCMLT_INVALID_HDL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("bcmlt_entry_handle_t eh is NULL.\n")));
        return;
    }

    rv = bcmlt_entry_info_get(eh, &bcmlt_ei);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to get entry info: %s (%d).\n"),
                   shr_errmsg(rv), rv));
        return;
    }

    pb = shr_pb_create();
    if (pb == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to create print buffer object.\n")));
        return;
    }

    rv += command_format(pb, prefix, &bcmlt_ei);
    rv += command_trans_type_format(pb, " ", NULL);
    rv += entry_format(pb, " ", &bcmlt_ei, &opcode, &logical);
    shr_pb_printf(pb, "\n");

    if (!raw) {
        if (logical && opcode == BCMLT_OPCODE_TRAVERSE) {
            uint32_t fcnt;
            rv = bcmlt_entry_field_count(bcmlt_ei.entry_hdl, &fcnt);
            if (SHR_SUCCESS(rv) && fcnt != 0) {
                /* Skip the traverse opcode with fields specified */
                cmd_out = false;
            }
        }
    }

    if (SHR_SUCCESS(rv)) {
        if (cmd_out) {
            if (verbose) {
                cli_out("%s", shr_pb_str(pb));
            }
            if (fh) {
                bcma_io_file_puts(fh, shr_pb_str(pb));
                bcma_io_file_flush(fh);
            }
        }
    } else {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to format the replayed command for "
                            "entry commit.\n")));
    }
    shr_pb_destroy(pb);

    /* Check for entry status */
    rv = bcmlt_ei.status;
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_NOT_FOUND &&
            logical && opcode == BCMLT_OPCODE_TRAVERSE) {
            /* End of traversing */
            return;
        }
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Invalid entry status for %s: %s (%d).\n"),
                   bcmlt_ei.table_name, shr_errmsg(rv), rv));
    }
}

/*
 * Format the transaction replayed data to human-readable CLI command.
 */
static void
transaction_commit_replay(bcmlt_transaction_hdl_t th, const char *prefix,
                          bool verbose, bcma_io_file_handle_t fh)
{
    int rv;
    uint32_t idx;
    bcmlt_transaction_info_t trans_info, *ti = &trans_info;
    shr_pb_t *pb;

    rv = bcmlt_transaction_info_get(th, ti);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to get transaction info: %s (%d).\n"),
                   shr_errmsg(rv), rv));
        return;
    }

    pb = shr_pb_create();
    if (pb == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to create print buffer object.\n")));
        return;
    }

    for (idx = 0; idx < ti->num_entries; idx++) {
        bcmlt_entry_info_t entry_info;

        rv = bcmlt_transaction_entry_num_get(th, idx, &entry_info);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Failed to get the #%d entry info "
                                "in transaction: %s (%d).\n"),
                       idx + 1, shr_errmsg(rv), rv));
            if (ti->type == BCMLT_TRANS_TYPE_BATCH) {
                continue;
            } else {
                /* All or none operation for atomic transaction type. */
                break;
            }
        }
        /*
         * Except for transactions containing only a single entry,
         * always format the transaction mode command in continuous
         * command mode to support a transaction with large amount of entries
         * that might exceed the CLI input size using only one single line
         * command to commit the transaction.
         */
        command_format(pb, prefix, &entry_info);
        if (idx == 0) {
            /*
             * Only need to format the transaction parameters once
             * in continuous mode.
             */
            command_trans_type_format(pb, " ", ti);
        }
        /* Ends the continuous mode at the last entry. */
        shr_pb_printf(pb, " ccm=%d", idx == (ti->num_entries -1) ? 0 : 1);
        entry_format(pb, " ", &entry_info, NULL, NULL);
        shr_pb_printf(pb, "\n");
    }
    if (verbose) {
        cli_out("%s", shr_pb_str(pb));
    }
    if (fh) {
        bcma_io_file_puts(fh, shr_pb_str(pb));
        bcma_io_file_flush(fh);
    }

    shr_pb_destroy(pb);
}

static void
replay_action(int unit, bcmlt_object_hdl_t *object)
{
    replay_info_t *ri = replay_info[unit];
    replay_action_cfg_t *cfg;

    if (ri == NULL || ri->act_cfg == NULL) {
        return;
    }
    cfg = ri->act_cfg;

    if (object == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "bcmlt_object_hdl_t object is NULL.\n")));
        return;
    }

    if (object->entry) {
        entry_commit_replay(object->hdl.entry, NULL,
                            cfg->console_output, cfg->ofh, cfg->raw);
    } else {
        transaction_commit_replay(object->hdl.trans, NULL,
                                  cfg->console_output, cfg->ofh);
    }
}

/*
 * Callback function from bcmlt_capture_replay() to read data for replay.
 */
static uint32_t
replay_read(void *fd, void *buffer, uint32_t nbyte)
{
    int rb;
    replay_info_t *ri = (replay_info_t *)fd;

    /*
     * Return 0 to stop reading and return from bcmlt_capture_replay gracefully
     * if the application is requesting to stop the replay engine.
     */
    if (shr_thread_stopping(ri->th)) {
        return 0;
    }

    rb = bcma_io_file_read(ri->ifh, buffer, 1, nbyte);
    return (rb < 0) ? 0 : rb;
}

static void
replay_action_config_cleanup(replay_action_cfg_t *cfg)
{
    if (cfg) {
        if (cfg->ofh) {
            bcma_io_file_close(cfg->ofh);
        }
        sal_free(cfg);
    }
}

static int
replay_action_config_init(bcma_bcmlt_replay_params_t *rparams,
                          replay_action_cfg_t **act_cfg)
{
    replay_action_cfg_t *cfg = NULL;

    if (rparams->text_file || rparams->console_output) {
        cfg = sal_alloc(sizeof(*cfg), "bcmaBcmltRelayActCfg");
        if (cfg == NULL) {
            return SHR_E_MEMORY;
        }
        sal_memset(cfg, 0, sizeof(*cfg));
        /* Open the file to be written to. */
        if (rparams->text_file) {
            cfg->ofh = bcma_io_file_open(rparams->text_file, "w");
            if (cfg->ofh == NULL) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("Failed to open file %s to write to.\n"),
                           rparams->text_file));
                replay_action_config_cleanup(cfg);
                return SHR_E_FAIL;
            }
        }
        cfg->console_output = rparams->console_output;
        cfg->raw = rparams->raw;
    }

    *act_cfg = cfg;

    return SHR_E_NONE;
}

static void
replay_info_cleanup(replay_info_t *ri)
{
    if (ri) {
        if (ri->ifh) {
            bcma_io_file_close(ri->ifh);
        }
        replay_action_config_cleanup(ri->act_cfg);
        sal_free(ri);
    }
}

static int
replay_info_init(int unit, bcma_bcmlt_replay_params_t *rparams,
                 replay_info_t ** rep_info)
{
    replay_info_t *ri = NULL;

    SHR_FUNC_ENTER(unit);

    if (rparams->cap_file == NULL && rparams->custom_replay_cb == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    ri = sal_alloc(sizeof(*ri), "bcmaBcmltReplayInfo");
    if (ri == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(ri, 0, sizeof(*ri));

    /* Open the captured file. */
    if (rparams->cap_file) {
        ri->ifh = bcma_io_file_open(rparams->cap_file, "r");
        if (ri->ifh == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Failed to open log file %s to replay.\n"),
                       rparams->cap_file));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

    /* Set replay action to default support function if not customized */
    if (rparams->custom_replay_act) {
        ri->act = rparams->custom_replay_act;
    } else {
        SHR_IF_ERR_EXIT
            (replay_action_config_init(rparams, &ri->act_cfg));
        ri->act = ri->act_cfg ? replay_action : NULL;
    }

    /* Set replay action to default support function if not customized */
    if (rparams->custom_replay_cb) {
        sal_memcpy(&ri->cb, rparams->custom_replay_cb, sizeof(ri->cb));
    } else {
        ri->cb.fd = ri;
        ri->cb.read = replay_read;
    }

    ri->unit = unit;
    ri->timing = rparams->timing;
    ri->submit = rparams->submit;

    *rep_info = ri;

exit:
    if (SHR_FUNC_ERR()) {
        replay_info_cleanup(ri);
    }
    SHR_FUNC_EXIT();
}

static void
replay_thread(shr_thread_t th, void *arg)
{
    int rv;
    replay_info_t *ri = (replay_info_t *)arg;

    rv = bcmlt_capture_replay(ri->timing, ri->act, !ri->submit, &ri->cb);
    if (ri->rp_done_cb) {
        ri->rp_done_cb(ri->unit, rv);
    }
}

/*******************************************************************************
 * Public functions
 */

int
bcma_bcmlt_replay_thread_start(int unit, bcma_bcmlt_replay_params_t *rparams,
                               bcma_bcmlt_replay_done_f rp_done_cb)
{
    replay_info_t *ri = NULL;
    char th_name[32];

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    if (rparams == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (replay_info[unit] != NULL) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    SHR_IF_ERR_EXIT
        (replay_info_init(unit, rparams, &ri));
    ri->rp_done_cb = rp_done_cb;

    sal_snprintf(th_name, sizeof(th_name), "bcmaBcmltReplay.%d", unit);
    ri->th = shr_thread_start(th_name, -1, replay_thread, ri);
    if (ri->th == NULL) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    replay_info[unit] = ri;

exit:
    if (SHR_FUNC_ERR()) {
        replay_info_cleanup(ri);
    }
    SHR_FUNC_EXIT();
}

int
bcma_bcmlt_replay_thread_stop(int unit)
{
    int rv;
    replay_info_t *ri;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    ri = replay_info[unit];
    if (ri == NULL) {
        return SHR_E_NOT_FOUND;
    }

    rv = shr_thread_stop(ri->th, 500 * MSEC_USEC);

    replay_info[ri->unit] = NULL;
    replay_info_cleanup(ri);

    return rv;
}
