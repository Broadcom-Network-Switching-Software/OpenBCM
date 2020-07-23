/*! \file bcma_testcase_pkt_tx.c
 *
 *  Test case for packet I/O Tx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_sleep.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/testcase/bcma_testcase_pkt_tx.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcma/bcmpkt/bcma_bcmpkt_test.h>
#include "bcma_testcase_pkt_internal.h"
#include <bcma/test/util/bcma_testutil_traffic.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define PKT_TX_PARAM_HELP \
    "  LengthStart     - Starting packet length\n"\
    "  LengthEnd       - Ending packet length\n"\
    "  LengthInc       - Increasing step of packet length\n"\
    "  SendCount       - The number of packets to be sent for each packet length\n"\

#define PKT_TX_EXAMPLE \
    "\nExamples:\n"\
    "  tr 40 ls=128 le=512 li=128 sc=50000\n"\

static void
pkt_tx_print_help(int unit, void *bp)
{
    cli_out("%s", PKT_TX_PARAM_HELP);
    cli_out("%s", PKT_TX_EXAMPLE);

    return;
}

static int
pkt_tx_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a, uint32_t flags,
              void **bp)
{
    bcma_bcmpkt_test_tx_param_t *tx_param = NULL;
    bool show = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    tx_param = sal_alloc(sizeof(bcma_bcmpkt_test_tx_param_t),
                         "bcmaTestCasePktTxPrm");
    if (tx_param == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(tx_param, 0, sizeof(bcma_bcmpkt_test_tx_param_t));
    *bp = (void *)tx_param;

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    SHR_IF_ERR_EXIT
        (bcma_bcmpkt_test_tx_parse_args(unit, cli, a, tx_param, show));
exit:
    SHR_FUNC_EXIT();
}

static int
pkt_tx_port_lb_set_cb(int unit, void *bp, uint32_t option)
{
    bcma_bcmpkt_test_tx_param_t *tx_param = (bcma_bcmpkt_test_tx_param_t *)bp;
    bcmdrd_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tx_param, SHR_E_PARAM);

    BCMDRD_PBMP_CLEAR(pbmp);
    BCMDRD_PBMP_PORT_ADD(pbmp, tx_param->port);

    SHR_IF_ERR_EXIT
        (bcma_testutil_port_setup(unit, LB_TYPE_MAC ,pbmp));

exit:
    SHR_FUNC_EXIT();
}

static int
pkt_tx_port_lb_cleanup_cb(int unit, void *bp, uint32_t option)
{
    bcma_bcmpkt_test_tx_param_t *tx_param = (bcma_bcmpkt_test_tx_param_t *)bp;
    bcmdrd_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tx_param, SHR_E_PARAM);

    BCMDRD_PBMP_CLEAR(pbmp);
    BCMDRD_PBMP_PORT_ADD(pbmp, tx_param->port);

    SHR_IF_ERR_EXIT
        (bcma_testutil_port_clear(unit, LB_TYPE_MAC, pbmp, TRUE));

exit:
    SHR_FUNC_EXIT();
}

static int
pkt_tx_packet_generate_cb(int unit, void *bp, uint32_t option)
{
    bcma_bcmpkt_test_tx_param_t *tx_param = (bcma_bcmpkt_test_tx_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tx_param, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_bcmpkt_test_packet_generate(unit, &tx_param->packet,
                                          tx_param->len_start,
                                          tx_param->len_end, tx_param->port));
exit:
    SHR_FUNC_EXIT();
}

static int
pkt_tx_traffic_dpath_create_cb(int unit, void *bp, uint32_t option)
{
    bcma_bcmpkt_test_tx_param_t *tx_param = (bcma_bcmpkt_test_tx_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tx_param, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_bcmpkt_test_dpath_create(unit, tx_param->port, -1,
                                       tx_param->ifp_cfg));

exit:
    SHR_FUNC_EXIT();
}

static int
pkt_tx_traffic_dpath_destroy_cb(int unit, void *bp, uint32_t option)
{
    bcma_bcmpkt_test_tx_param_t *tx_param = (bcma_bcmpkt_test_tx_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tx_param, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_bcmpkt_test_dpath_destroy(unit, tx_param->port, tx_param->ifp_cfg));

exit:
    SHR_FUNC_EXIT();
}

static int
pkt_tx_run_cb(int unit, void *bp, uint32_t option)
{
    bcma_bcmpkt_test_tx_param_t *tx_param = (bcma_bcmpkt_test_tx_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tx_param, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcma_bcmpkt_test_tx_run(unit, tx_param));

exit:
    SHR_FUNC_EXIT();
}

static void
pkt_tx_recycle_cb(int unit, void *bp)
{
    bcma_bcmpkt_test_tx_param_t *tx_param = (bcma_bcmpkt_test_tx_param_t *)bp;

    if (!tx_param) {
        return;
    }
    if (tx_param->packet != NULL) {
        bcmpkt_free(tx_param->packet->unit, tx_param->packet);
        tx_param->packet = NULL;
    }
    if (tx_param->ifp_cfg != NULL) {
        sal_free(tx_param->ifp_cfg);
        tx_param->ifp_cfg = NULL;
    }

    return;
}

static bcma_test_proc_t pkt_tx_proc[] = {
    {
        .desc = "cleanup packet I/O device\n",
        .cb = bcma_testcase_pkt_dev_cleanup_cb,
    },
    {
        .desc = "init packet I/O device\n",
        .cb = bcma_testcase_pkt_dev_init_cb,
    },
    {
        .desc = "set port loopback mode, wait link up\n",
        .cb = pkt_tx_port_lb_set_cb,
    },
    {
        .desc = "generate a packet for the test\n",
        .cb = pkt_tx_packet_generate_cb,
    },
    {
        .desc = "create traffic data path\n",
        .cb = pkt_tx_traffic_dpath_create_cb,
    },
    {
        .desc = "run packet I/O Tx test\n",
        .cb = pkt_tx_run_cb,
    },
    {
        .desc = "destroy traffic data path\n",
        .cb = pkt_tx_traffic_dpath_destroy_cb,
    },
    {
        .desc = "clean port loopback mode\n",
        .cb = pkt_tx_port_lb_cleanup_cb,
    },
    {
        .desc = "cleanup packet I/O device\n",
        .cb = bcma_testcase_pkt_dev_cleanup_cb,
    }
};

static bcma_test_op_t pkt_tx_op = {
    .select = bcma_testcase_pkt_select,
    .parser = pkt_tx_parser,
    .help = pkt_tx_print_help,
    .recycle = pkt_tx_recycle_cb,
    .procs = pkt_tx_proc,
    .proc_count = COUNTOF(pkt_tx_proc)
};

bcma_test_op_t *
bcma_testcase_pkt_tx_op_get(void)
{
    return &pkt_tx_op;
}

