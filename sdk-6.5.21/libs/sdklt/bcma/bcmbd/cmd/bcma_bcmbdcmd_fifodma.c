/*! \file bcma_bcmbdcmd_fifodma.c
 *
 * DMA FIFO instance commands in CLI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shr/shr_thread.h>
#include <sal/sal_sleep.h>
#include <shr/shr_types.h>
#include <shr/shr_pb.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_fifodma.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcmmgmt/bcmmgmt.h>

#include <bcma/bcmbd/bcma_bcmbdcmd_fifodma.h>

#define BSL_LOG_MODULE          BSL_LS_APPL_FIFODMA

/*! Maximum unit supported. */
#define MAX_UNITS               BCMDRD_CONFIG_MAX_UNITS

#define FIFODMA_CHAN_NUM        12
/* Triger threshold. (Percentages)*/
#define FIFODMA_THRESHOLD       50

/*!
 * Size of buffer allocated to store SBUS OPCODE header.
 */
#define SBUS_HEADER_MAX_WORDS   4

/* Each chan has a work. */
typedef struct fifodma_work_s {
    bcmdrd_sid_t sid;

    uint32_t pipe;

    /* Entry Size */
    uint32_t entry_wsize;

    /* Eviction threshold */
    uint32_t threshold;

    /* FIFO DMA start */
    bool start;

    /* Stop FIFO DMA */
    bool stop;

    /* FIFO DMA ticks */
    uint16_t ticks;

    /* Number of entries. */
    uint16_t count;

    /*! Eviction buffer */
    uint32_t *buffer;

    /*! DMA FIFO work item. */
    bcmbd_fifodma_work_t work;

    /*! DMA FIFO data item. */
    bcmbd_fifodma_data_t data;

} fifodma_work_t;

/* DMA FIFO thread control. */
static shr_thread_t thread_ctrl[MAX_UNITS];
/*! True means the FIFODMA channel interrupt triggered. */
static bool new_intr[MAX_UNITS][FIFODMA_CHAN_NUM];
/* DMA FIFO works. */
static fifodma_work_t works[MAX_UNITS][FIFODMA_CHAN_NUM];

static void
fifodma_intr_cb(int unit, void *data)
{
    int chan = *(int *)data;

    if (chan < FIFODMA_CHAN_NUM && chan >= 0) {
        new_intr[unit][chan] =  true;
        shr_thread_wake(thread_ctrl[unit]);
    }
}

static void
dump_data(shr_pb_t *pb, uint32_t *data, uint32_t size)
{
    uint32_t i;

    for (i = 0; i < size; i++) {
        shr_pb_printf(pb, "%08x ", data[i]);
        if (i % 4 == 3) {
            shr_pb_printf(pb, "\n");
        }
    }
    shr_pb_printf(pb, "\n");
}

static void
data_process(int unit, int chan)
{
    uint32_t offset = 0;
    uint32_t num = 0;
    uint32_t *entry;
    int rv;
    shr_pb_t *pb;
    fifodma_work_t *work = &works[unit][chan];

    work->ticks++;

    if (!work->start) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Channel %d does not run.\n"),
                     chan));
        return ;
    } else if (work->stop) {
        bcmbd_fifodma_stop(unit, chan);
        work->stop = false;
        work->buffer = NULL;
        work->start = false;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Channel %d stopped.\n"),
                     chan));
        return ;
    }

    /* Get evicted entries */
    rv = bcmbd_fifodma_pop(unit, chan, 0, &offset, &num);

    LOG_VERBOSE(BSL_LOG_MODULE,(BSL_META_U(unit,
                "FIFO DMA: Evicted entries = %u, offset = %u\n"),
                num, offset));
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_EMPTY) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Eviction FIFO %d is empty.\n"),
                         chan));
        } else {
            cli_out("FIFO DMA channel %d pop failure - rv = %d\n", chan, rv);
        }
        return ;
    }

    pb = shr_pb_create();
    shr_pb_printf(pb, "Chan %d:\n", chan);
    /* Process evicted entries */
    while (num--) {
        work->count++;
        entry = work->buffer + (offset * work->entry_wsize);
        offset = (offset + 1) % work->data.num_entries;
        dump_data(pb, entry, work->entry_wsize);
        LOG_INFO(BSL_LOG_MODULE,(BSL_META_U(unit, "\n%s"), shr_pb_str(pb)));

        shr_pb_reset(pb);
    }
    shr_pb_destroy(pb);

}

static void
fifodma_handler(shr_thread_t tc, void *arg)
{
    int unit = *(int *)arg;
    int chan;

    while (1) {
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        if (shr_thread_stopping(tc)) {
            break;
        }

        for (chan = 0; chan < FIFODMA_CHAN_NUM; chan++) {
            if (new_intr[unit][chan]) {
                new_intr[unit][chan] = false;
                data_process(unit, chan);
            }
        }
    }
}

static int
fifodma_stop(int unit, int chan)
{
    fifodma_work_t *work = &works[unit][chan];

    if (work->start) {
        work->stop = true;
        fifodma_intr_cb(unit, &chan);
    }

    return BCMA_CLI_CMD_OK;
}

static void
fifodma_cleanup(int unit, void *cb_data)
{
    int rv;
    int chan;

    if (thread_ctrl[unit] != NULL) {
        for (chan = 0; chan < FIFODMA_CHAN_NUM; chan++) {
            fifodma_stop(unit, chan);
        }

        rv = shr_thread_stop(thread_ctrl[unit], 2 * SECOND_USEC);
        if (SHR_FAILURE(rv)) {
            cli_out("fifodma_handler could not stop. (%d)\n", rv);
        } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "FIFODMA thread stopped.\n")));

        }
        thread_ctrl[unit] = NULL;
    }
}

/*
 * Interrupt should be enabled by reg set/get.
 * Example for enable ETRAP pipe 0 instant 0 interrupt:
 *  dsh -c "set INTR_CONFIG_MASKr 0x1"
 */
static int
fifodma_start(int unit, int chan)
{
    int rv;
    fifodma_work_t *fifo_work = &works[unit][chan];

    if (fifo_work->start) {
        cli_out("Channel %d is running.\n", chan);
        return BCMA_CLI_CMD_FAIL;
    }

    if (!fifo_work->entry_wsize) {
        cli_out("Channel %d is not configured.\n", chan);
        return BCMA_CLI_CMD_FAIL;
    }

    if (thread_ctrl[unit] == NULL) {
        thread_ctrl[unit] = shr_thread_start("bcmaFifoDmaHandler",
                                             SAL_THREAD_PRIO_DEFAULT,
                                             fifodma_handler,
                                             &unit);

        if (thread_ctrl[unit] == NULL) {
            cli_out("Could not start fifodma test handler thread.\n");
            return BCMA_CLI_CMD_FAIL;
        }

        /* Register a shutdown callback function for fifodma cleanup. */
        rv = bcmmgmt_shutdown_cb_register(fifodma_cleanup, NULL);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit,
                                  "Failed to create shutdown callback (%s)\n"),
                       shr_errmsg(rv)));
            return BCMA_CLI_CMD_FAIL;
        }
    }

    rv = bcmbd_fifodma_start(unit, chan, &fifo_work->work);
    if (SHR_FAILURE(rv)) {
        cli_out("Channel %d start failed. (%d)\n", chan, rv);
        return BCMA_CLI_CMD_FAIL;
    }
    LOG_VERBOSE(BSL_LOG_MODULE,(BSL_META_U(unit,
              "DMA Fifo started: FIFO Chan - %d, Opcode = 0x%08x\n"),
               chan, fifo_work->data.op_code));
    fifo_work->buffer = fifo_work->work.buf_cache;
    fifo_work->start = true;

    return BCMA_CLI_CMD_OK;
}

static int
fifodma_poll(int unit, int chan)
{
    fifodma_work_t *work = &works[unit][chan];

    if (work->buffer == NULL || thread_ctrl[unit] == NULL) {
        cli_out("Channel %d is not running!\n", chan);
        return BCMA_CLI_CMD_FAIL;
    }

    fifodma_intr_cb(unit, &chan);

    return BCMA_CLI_CMD_OK;
}

static int
fifodma_status(int unit)
{
    int chan;
    shr_pb_t *pb;

    pb = shr_pb_create();
    shr_pb_printf(pb, "FIFODMA status: \n");

    for (chan = 0; chan < FIFODMA_CHAN_NUM; chan++) {
        fifodma_work_t *work = &works[unit][chan];
        if (work->entry_wsize) {
            shr_pb_printf(pb,
                " %d: sid %s pipe %d threshold %d ticks %d count %d %srunning\n",
                chan,
                bcmdrd_pt_sid_to_name(unit, work->sid),
                work->pipe,
                work->threshold,
                work->ticks,
                work->count,
                (work->start) ? "" : "not ");
         }
    }

    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

static int
fifo_work_init(int unit, int chan, int pipe, bcmdrd_sid_t sid, int size,
               int threshold)
{
    int rv;
    bcmbd_fifodma_work_t *work;
    bcmbd_fifodma_data_t *data;
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_sym_info_t sinfo = {0};
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t header[SBUS_HEADER_MAX_WORDS], hd_size = 0;
    fifodma_work_t *fifodma_work = &works[unit][chan];

    if (fifodma_work->start) {
        cli_out("FIFO channel %d is running. Changes not allowed.\n", chan);
        return BCMA_CLI_CMD_FAIL;
    }
    sal_memset(fifodma_work, 0, sizeof(*fifodma_work));
    fifodma_work->sid = sid;
    fifodma_work->pipe = pipe;
    fifodma_work->threshold = threshold;
    work = &fifodma_work->work;
    data = &fifodma_work->data;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (!cinfo) {
        cli_out("bcmdrd_dev_chip_info_get failed\n");
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcmdrd_pt_info_get(unit, sid, &sinfo);
    if (SHR_FAILURE(rv)) {
        cli_out("bcmdrd_pt_info_get failed (%d)\n", rv);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Initialize FIFO DMA work */
    pt_dyn_info.index = 0;
    pt_dyn_info.tbl_inst = pipe;
    rv = bcmbd_pt_cmd_hdr_get(unit,
                              sid,
                              &pt_dyn_info,
                              NULL,
                              BCMBD_PT_CMD_POP,
                              SBUS_HEADER_MAX_WORDS,
                              header,
                              &hd_size);
    if (SHR_FAILURE(rv)) {
        cli_out("bcmbd_pt_cmd_hdr_get failed (%d)\n", rv);
        return BCMA_CLI_CMD_FAIL;
    }

    data->op_code = header[0];
    data->start_addr = header[1];
    data->data_width = sinfo.entry_wsize;
    data->num_entries = size;
    data->thresh_entries = (threshold *size) / 100;
    work->data = data;
    work->cb = fifodma_intr_cb;
    work->cb_data = &chan;
    work->flags = FIFODMA_WF_INT_MODE;
    fifodma_work->entry_wsize = sinfo.entry_wsize;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Configured - chan %d sid %d pipe %d size %d threshold %d\n"),
                 chan, sid, pipe, size, threshold));

    return BCMA_CLI_CMD_OK;
}

static int
fifodma_chan_config(bcma_cli_t *cli, bcma_cli_args_t *args, int chan)
{
    int unit = cli->cur_unit;
    int rv;
    char *sid_name = NULL;
    bcmdrd_sid_t sid;
    int pipe = -1;
    int pipe_num = -1;
    int size = 1024;
    static int threshold = FIFODMA_THRESHOLD;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Memory", "str", &sid_name, NULL);
    bcma_cli_parse_table_add(&pt, "SiZe", "int", &size, NULL);
    bcma_cli_parse_table_add(&pt, "Pipe", "int", &pipe, NULL);
    bcma_cli_parse_table_add(&pt, "THReshold", "int", &threshold, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        return BCMA_CLI_CMD_USAGE;
    }

    if (sid_name == NULL || pipe < 0 || threshold < 1 || threshold > 100) {
        cli_out("Invalid options: sid - %s, chan %d, pipe %d\n",
                (sid_name == NULL) ? "NULL" : sid_name, chan, pipe);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcmdrd_pt_name_to_sid(unit, sid_name, &sid);
    if (SHR_FAILURE(rv)) {
        cli_out("Does not support %s. \n", sid_name);
        return BCMA_CLI_CMD_FAIL;
    }

    pipe_num = bcmdrd_pt_num_pipe_inst(unit, sid);
    if (pipe_num < 0) {
        cli_out("Could not get pipe number for %s. \n", sid_name);
        return BCMA_CLI_CMD_FAIL;
    }

    return fifo_work_init(unit, chan, pipe, sid, size, threshold);
}

int
bcma_bcmbdcmd_fifodma(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cur_unit;
    char *cmd = NULL;
    char *arg = NULL;
    int chan;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("STatus", cmd,  ' ')) {
        return fifodma_status(unit);
    }

    /* Channel base commands. */
    if (!(arg = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }
    chan = sal_strtol(arg, &arg, 0);
    if (*arg != 0) {
        return BCMA_CLI_CMD_USAGE;
    }
    if (chan < 0 || chan >= FIFODMA_CHAN_NUM) {
        cli_out("Invalid channel %d\n", chan);
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_cli_parse_cmp("ChanConfig", cmd,  ' ')) {
        return fifodma_chan_config(cli, args, chan);
    }

    if (bcma_cli_parse_cmp("START", cmd,  ' ')) {
        return fifodma_start(unit, chan);
    }

    if (bcma_cli_parse_cmp("STOP", cmd,  ' ')) {
        return fifodma_stop(unit, chan);
    }

    if (bcma_cli_parse_cmp("Poll", cmd,  ' ')) {
        return fifodma_poll(unit, chan);
    }

    cli_out("%sUnknown FIFODMA command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;

}
