/*! \file bcma_bcmbdcmd_sbusdma.c
 *
 * DMA FIFO instance commands in CLI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_util.h>
#include <shr/shr_pb.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_sbusdma.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_sbusdma.h>

#define BSL_LOG_MODULE          BSL_LS_APPL_SBUSDMA

/*!
 * Size of buffer allocated to store SBUS OPCODE header.
 */
#define SBUS_HEADER_MAX_WORDS   4

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

static int
sbusdma_read(int unit, bcmdrd_sid_t sid, int pipe, int index, int count)
{
    int i;
    uint64_t *buf = NULL, paddr;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmbd_sbusdma_data_t dma_data;
    bcmbd_sbusdma_work_t dma_work;
    uint32_t header[SBUS_HEADER_MAX_WORDS], hd_size = 0, entry_size;
    shr_pb_t *pb;

    SHR_FUNC_ENTER(unit);

    /* Initialize FIFO DMA work */
    pt_dyn_info.index = index;
    pt_dyn_info.tbl_inst = pipe;
    SHR_IF_ERR_EXIT
        (bcmbd_pt_cmd_hdr_get(unit,
                              sid,
                              &pt_dyn_info,
                              NULL,
                              BCMBD_PT_CMD_READ,
                              SBUS_HEADER_MAX_WORDS,
                              header,
                              &hd_size));
    entry_size = bcmdrd_pt_entry_wsize(unit, sid);

    buf = bcmdrd_hal_dma_alloc(unit, entry_size * count * 4, "bcmaBcmbdSbusdmaRead",
                               &paddr);

    SHR_NULL_CHECK(buf, SHR_E_MEMORY);

    /* Prepare dma_data */
    sal_memset(&dma_data, 0, sizeof(bcmbd_sbusdma_data_t));
    dma_data.start_addr = header[1];
    dma_data.data_width = entry_size;
    dma_data.op_code = header[0];
    dma_data.op_count = count;
    dma_data.buf_paddr = paddr;
    dma_data.pend_clocks = bcmbd_mor_clks_read_get(unit, sid);

    /* Prepare dma_work */
    sal_memset(&dma_work, 0, sizeof(bcmbd_sbusdma_work_t));
    dma_work.data = &dma_data;
    dma_work.items = 1;
    dma_work.flags = SBUSDMA_WF_READ_CMD;

    SHR_IF_ERR_EXIT
        (bcmbd_sbusdma_light_work_init(unit, &dma_work));

    SHR_IF_ERR_EXIT
        (bcmbd_sbusdma_work_execute(unit, &dma_work));

    pb = shr_pb_create();
    shr_pb_printf(pb, "Read data:\n");
    for (i = 0; i < count; i++) {
        shr_pb_printf(pb, "Entry %d:\n", i + index);
        dump_data(pb, (uint32_t *)buf + i * entry_size, entry_size);
    }
    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);

exit:
    if (buf) {
        bcmdrd_hal_dma_free(unit, entry_size * count * 4, buf, paddr);
        buf = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
sbusdma_write(int unit, bcmdrd_sid_t sid, int pipe, int index, char *data)
{
    uint64_t *buf = NULL, paddr;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmbd_sbusdma_data_t dma_data;
    bcmbd_sbusdma_work_t dma_work;
    uint32_t header[SBUS_HEADER_MAX_WORDS], hd_size = 0, entry_size;
    shr_pb_t *pb;

    SHR_FUNC_ENTER(unit);

    /* Initialize FIFO DMA work */
    pt_dyn_info.index = index;
    pt_dyn_info.tbl_inst = pipe;
    SHR_IF_ERR_EXIT
        (bcmbd_pt_cmd_hdr_get(unit,
                              sid,
                              &pt_dyn_info,
                              NULL,
                              BCMBD_PT_CMD_WRITE,
                              SBUS_HEADER_MAX_WORDS,
                              header,
                              &hd_size));

    entry_size = bcmdrd_pt_entry_wsize(unit, sid);
    buf = bcmdrd_hal_dma_alloc(unit, entry_size * 4, "bcmaBcmbdSbusdmaWrite", &paddr);
    SHR_NULL_CHECK(buf, SHR_E_RESOURCE);

    sal_memset(buf, 0, entry_size);

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_array_uint32(data, entry_size, (uint32_t *)buf));

    pb = shr_pb_create();
    shr_pb_printf(pb, "Write data:\n");
    dump_data(pb, (uint32_t *)buf, entry_size);
    LOG_VERBOSE(BSL_LS_APPL_SBUSDMA, (BSL_META("%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);

    /* Prepare dma_data */
    sal_memset(&dma_data, 0, sizeof(bcmbd_sbusdma_data_t));
    dma_data.start_addr = header[1];
    dma_data.data_width = entry_size;
    dma_data.op_code = header[0];
    dma_data.op_count = 1;
    dma_data.buf_paddr = paddr;
    dma_data.pend_clocks = bcmbd_mor_clks_write_get(unit, sid);

    /* Prepare dma_work */
    sal_memset(&dma_work, 0, sizeof(bcmbd_sbusdma_work_t));
    dma_work.data = &dma_data;
    dma_work.items = 1;
    dma_work.flags = SBUSDMA_WF_WRITE_CMD;

    SHR_IF_ERR_EXIT
        (bcmbd_sbusdma_light_work_init(unit, &dma_work));

    SHR_IF_ERR_EXIT
        (bcmbd_sbusdma_work_execute(unit, &dma_work));

exit:
    if (buf) {
        bcmdrd_hal_dma_free(unit, entry_size * 4, buf, paddr);
        buf = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
sbusdma_push(int unit, bcmdrd_sid_t sid, int pipe, int index, char *data)
{
    uint64_t *buf = NULL, paddr;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmbd_sbusdma_data_t dma_data;
    bcmbd_sbusdma_work_t dma_work;
    uint32_t header[SBUS_HEADER_MAX_WORDS], hd_size = 0, entry_size;
    shr_pb_t *pb;

    SHR_FUNC_ENTER(unit);

    /* Initialize FIFO DMA work */
    pt_dyn_info.index = index;
    pt_dyn_info.tbl_inst = pipe;
    SHR_IF_ERR_EXIT
        (bcmbd_pt_cmd_hdr_get(unit,
                              sid,
                              &pt_dyn_info,
                              NULL,
                              BCMBD_PT_CMD_PUSH,
                              SBUS_HEADER_MAX_WORDS,
                              header,
                              &hd_size));

    entry_size = bcmdrd_pt_entry_wsize(unit, sid);
    buf = bcmdrd_hal_dma_alloc(unit, entry_size * 4, "bcmaBcmbdSbusdmaPush", &paddr);
    SHR_NULL_CHECK(buf, SHR_E_RESOURCE);

    sal_memset(buf, 0, entry_size);

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_array_uint32(data, entry_size, (uint32_t *)buf));

    pb = shr_pb_create();
    shr_pb_printf(pb, "Push data:\n");
    dump_data(pb, (uint32_t *)buf, entry_size);
    LOG_VERBOSE(BSL_LS_APPL_SBUSDMA, (BSL_META("%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);

    /* Prepare dma_data */
    sal_memset(&dma_data, 0, sizeof(bcmbd_sbusdma_data_t));
    dma_data.start_addr = header[1];
    dma_data.data_width = entry_size;
    dma_data.op_code = header[0];
    dma_data.op_count = 1;
    dma_data.buf_paddr = paddr;
    dma_data.pend_clocks = bcmbd_mor_clks_write_get(unit, sid);

    /* Prepare dma_work */
    sal_memset(&dma_work, 0, sizeof(bcmbd_sbusdma_work_t));
    dma_work.data = &dma_data;
    dma_work.items = 1;
    dma_work.flags = SBUSDMA_WF_WRITE_CMD;

    SHR_IF_ERR_EXIT
        (bcmbd_sbusdma_light_work_init(unit, &dma_work));

    SHR_IF_ERR_EXIT
        (bcmbd_sbusdma_work_execute(unit, &dma_work));

exit:
    if (buf) {
        bcmdrd_hal_dma_free(unit, entry_size * 4, buf, paddr);
        buf = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
sbusdma_read_cmd(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cur_unit;
    int rv;
    char *sid_name = NULL;
    bcmdrd_sid_t sid;
    int pipe = 0;
    int count = -1;
    int index = 0;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Memory", "str", &sid_name, NULL);
    bcma_cli_parse_table_add(&pt, "Pipe", "int", &pipe, NULL);
    bcma_cli_parse_table_add(&pt, "InDeX", "int", &index, NULL);
    bcma_cli_parse_table_add(&pt, "Count", "int", &count, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        return BCMA_CLI_CMD_USAGE;
    }

    if (sid_name == NULL || pipe < 0 || count < 0 || index < 0) {
        cli_out("Invalid options: mem - %s, pipe %d, index %d, count %d\n",
                (sid_name == NULL) ? "NULL" : sid_name, pipe, index, count);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcmdrd_pt_name_to_sid(unit, sid_name, &sid);
    if (SHR_FAILURE(rv)) {
        cli_out("Does not support %s. \n", sid_name);
        return BCMA_CLI_CMD_FAIL;
    }

    return sbusdma_read(unit, sid, pipe, index, count);
}

static int
sbusdma_write_cmd(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cur_unit;
    int rv;
    char *sid_name = NULL;
    bcmdrd_sid_t sid;
    int pipe = 0;
    int index = 0;
    char *data = NULL;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Memory", "str", &sid_name, NULL);
    bcma_cli_parse_table_add(&pt, "Pipe", "int", &pipe, NULL);
    bcma_cli_parse_table_add(&pt, "InDeX", "int", &index, NULL);
    bcma_cli_parse_table_add(&pt, "DATA", "str", &data, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        return BCMA_CLI_CMD_USAGE;
    }

    if (sid_name == NULL || pipe < 0) {
        cli_out("Invalid options: mem - %s, pipe %d, index %d\n",
                (sid_name == NULL) ? "NULL" : sid_name, pipe, index);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcmdrd_pt_name_to_sid(unit, sid_name, &sid);
    if (SHR_FAILURE(rv)) {
        cli_out("Does not support %s. \n", sid_name);
        return BCMA_CLI_CMD_FAIL;
    }

    return sbusdma_write(unit, sid, pipe, index, data);
}

static int
sbusdma_push_cmd(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cur_unit;
    int rv;
    char *sid_name = NULL;
    bcmdrd_sid_t sid;
    int pipe = 0;
    int index = 0;
    char *data = NULL;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Memory", "str", &sid_name, NULL);
    bcma_cli_parse_table_add(&pt, "Pipe", "int", &pipe, NULL);
    bcma_cli_parse_table_add(&pt, "InDeX", "int", &index, NULL);
    bcma_cli_parse_table_add(&pt, "DATA", "str", &data, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        return BCMA_CLI_CMD_USAGE;
    }

    if (sid_name == NULL || pipe < 0) {
        cli_out("Invalid options: mem - %s, pipe %d, index %d\n",
                (sid_name == NULL) ? "NULL" : sid_name, pipe, index);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcmdrd_pt_name_to_sid(unit, sid_name, &sid);
    if (SHR_FAILURE(rv)) {
        cli_out("Does not support %s. \n", sid_name);
        return BCMA_CLI_CMD_FAIL;
    }

    return sbusdma_push(unit, sid, pipe, index, data);
}

int
bcma_bcmbdcmd_sbusdma(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char *cmd = NULL;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Read", cmd,  ' ')) {
        return sbusdma_read_cmd(cli, args);
    }

    if (bcma_cli_parse_cmp("Write", cmd,  ' ')) {
        return sbusdma_write_cmd(cli, args);
    }

    if (bcma_cli_parse_cmp("Push", cmd,  ' ')) {
        return sbusdma_push_cmd(cli, args);
    }

    cli_out("%sUnknown SBUSDMA command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;

}

