/*! \file bcma_bcmpktcmd_pkttrace.c
 *
 * BCMPKT TRACE commands in CLI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_alloc.h>
#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcmpkt/bcmpkt_trace.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_pkttrace.h>
#include "bcma_bcmpktcmd_internal.h"

#define BSL_LOG_MODULE BSL_LS_APPL_PACKET

typedef struct px_trace_s {
    int src_port;
    int len;
    int netif_id;
} px_trace_t;

static void
trace_data_dump(const uint8_t *data, int size, shr_pb_t *pb)
{
    int idx;

    if (data == NULL) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("NULL pointer\n")));
        return;
    }

    shr_pb_printf(pb, "Trace Raw Data - Data length: %d\n", size);
    if (size == 0) {
        return;
    }

    for (idx = 0;  idx < size; idx++) {
        if ((idx & 0xf) == 0) {
            shr_pb_printf(pb, "%04x: ", idx);
        }
        if ((idx & 0xf) == 8) {
            shr_pb_printf(pb, "- ");
        }
        shr_pb_printf(pb, "%02x ", data[idx]);
        if ((idx & 0xf) == 0xf) {
            shr_pb_printf(pb, "\n");
        }
    }
    if ((idx & 0xf) != 0) {
        shr_pb_printf(pb, "\n");
    }
}

static int
trace_trigger_param_check(px_trace_t *px_cfg, char *data)
{
    int len;

    if ((px_cfg->src_port == PX_NOT_CONFIGURED) ||
        (px_cfg->netif_id == PX_NOT_CONFIGURED)) {
        return SHR_E_FAIL;
    }
    if (data == NULL) {
        return SHR_E_FAIL;
    }
    len = sal_strlen(data);
    if (px_cfg->len == PX_NOT_CONFIGURED) {
        if (len > 64) {
            px_cfg->len = len;
        } else {
            px_cfg->len = 64;
        }
    } else {
        if (len > 1518 && px_cfg->len > 1518) {
            px_cfg->len = 1518;
        } else if (len < px_cfg->len) {
            if (px_cfg->len < 64) {
                px_cfg->len = 64;
            }
        } else {
            if (len < 64) {
                px_cfg->len = 64;
            }
        }
    }

    return SHR_E_NONE;
}

static int
do_trace(int unit, char *pkt, px_trace_t *px_cfg, uint32_t opts, int parse_only,
         shr_pb_t *pb)
{
    int rv;
    bcmpkt_trace_pkt_t trace_pkt;
    bcmpkt_trace_data_t trace_data;
    uint32_t len = 0;
    uint32_t size;
    uint8_t *data = NULL;
    uint32_t trace_data_len = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&trace_pkt, 0, sizeof(bcmpkt_trace_pkt_t));

    rv = bcmpkt_trace_data_len_get(unit, &trace_data_len);
    if (SHR_FAILURE(rv) && (rv != SHR_E_UNAVAIL))  {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    if (trace_data_len) {
        data = sal_alloc(sizeof(uint8_t) * trace_data_len, "bcmaBcmpktTrace");
        if (data == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }

    if (parse_only) {
        rv = bcma_bcmpkt_load_data_from_istr(pkt, data, trace_data_len, &size);
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (size < trace_data_len) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Trace data length should be %d bytes\n"),
                      trace_data_len));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        rv = bcmpkt_trace_fields_dump(unit, data, pb);
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    } else {
        rv = trace_trigger_param_check(px_cfg, pkt);
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        trace_pkt.pkt = sal_alloc(px_cfg->len, "bcmaBcmpktTrace");
        if (trace_pkt.pkt == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        rv = bcma_bcmpkt_load_data_from_istr(pkt, trace_pkt.pkt,
                                             (uint32_t)px_cfg->len, &len);
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        trace_pkt.options = opts;
        trace_pkt.netif_id = px_cfg->netif_id;
        trace_pkt.port = px_cfg->src_port;
        trace_pkt.len = px_cfg->len;

        trace_data.buf_size = trace_data_len;
        trace_data.buf = data;
        rv = bcmpkt_trace_data_collect(unit, &trace_pkt, &trace_data);
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        if (!trace_data_len) {
            SHR_EXIT();
        }

        trace_data_dump(trace_data.buf, trace_data.len, pb);

        rv = bcmpkt_trace_fields_dump(unit, trace_data.buf, pb);
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    if (data != NULL) {
        sal_free(data);
    }
    if (trace_pkt.pkt != NULL) {
        sal_free(trace_pkt.pkt);
    }

    SHR_FUNC_EXIT();
}

static int
trace_cmds(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv, parse_only = 0;
    int lrn = 0, noifp = 0, drop = 0;
    uint32_t trace_opts = 0;
    shr_pb_t *pb = NULL;
    int unit = cli->cmd_unit;
    char *pkt = NULL;
    px_trace_t px_cfg;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);

    px_cfg.src_port = PX_NOT_CONFIGURED;
    px_cfg.len = PX_NOT_CONFIGURED;
    px_cfg.netif_id = PX_NOT_CONFIGURED;

    bcma_cli_parse_table_add(&pt, "Port", "int",
                             &px_cfg.src_port, NULL);
    bcma_cli_parse_table_add(&pt, "Len", "int",
                             &px_cfg.len, NULL);
    bcma_cli_parse_table_add(&pt, "NetifID", "int",
                             &px_cfg.netif_id, NULL);
    bcma_cli_parse_table_add(&pt, "Data", "str",
                             &pkt, NULL);
    bcma_cli_parse_table_add(&pt, "PArseOnly", "bool",
                             &parse_only, NULL);
    bcma_cli_parse_table_add(&pt, "LeaRN", "bool",
                             &lrn, NULL);
    bcma_cli_parse_table_add(&pt, "NoIFP", "bool",
                             &noifp, NULL);
    bcma_cli_parse_table_add(&pt, "DropTm", "bool",
                             &drop, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid arg: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    if (lrn) {
        trace_opts |= BCMPKT_TRACE_OPT_LEARN;
    }
    if (noifp) {
        trace_opts |= BCMPKT_TRACE_OPT_NO_IFP;
    }
    if (drop) {
        trace_opts |= BCMPKT_TRACE_OPT_DROP_TM;
    }

    pb = shr_pb_create();

    rv = do_trace(unit, pkt, &px_cfg, trace_opts, parse_only, pb);

    cli_out("%s", shr_pb_str(pb));

    shr_pb_destroy(pb);
    bcma_cli_parse_table_done(&pt);

    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_PARAM) {
            return BCMA_CLI_CMD_USAGE;
        }
        return BCMA_CLI_CMD_FAIL;
    }
    return BCMA_CLI_CMD_OK;
}

int
bcma_bcmpktcmd_pkttrace(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    if (cli == NULL || args == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (BCMA_CLI_ARG_CNT(args) == 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    return trace_cmds(cli, args);
}
