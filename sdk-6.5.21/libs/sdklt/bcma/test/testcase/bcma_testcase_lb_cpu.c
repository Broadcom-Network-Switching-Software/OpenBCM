/*! \file bcma_testcase_lb_cpu.c
 *
 *  Test case for CPU loopback.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>

#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmpkt/bcmpkt_buf.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/testcase/bcma_testcase_lb_cpu.h>
#include "bcma_testcase_lb_internal.h"

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

static int
cpu_lb_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a, uint32_t flags, void **bp)
{
    return bcma_testcase_lb_parser(unit, cli, a, flags, bp, LB_TYPE_CPU);
}

static int
cpu_lb_test_all_chan(int unit, lb_param_t *lb_param)
{
    int tx_chan, rx_chan, init_done = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcma_testcase_lb_txrx_init(unit, lb_param));
    init_done = 1;

    /* Iterate all chan */
    for (tx_chan = 0; tx_chan < LB_CMC_CHAN_NUM; tx_chan++) {
        for (rx_chan = 0; rx_chan < LB_CMC_CHAN_NUM; rx_chan++) {
            if (rx_chan == tx_chan) {
                continue;
            }

            SHR_IF_ERR_EXIT
                (bcma_testcase_lb_txrx(unit, lb_param, tx_chan, rx_chan));
        }
    }

exit:
    if (init_done) {
        bcma_testcase_lb_txrx_cleanup(unit, lb_param);
    }

    SHR_FUNC_EXIT();
}

static int
cpu_lb_txrx_cb(int unit, void *bp, uint32_t option)
{
    lb_param_t *lb_param = (lb_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);


    SHR_IF_ERR_EXIT
        (cpu_lb_test_all_chan(unit, lb_param));

exit:

    SHR_FUNC_EXIT();
}

static bcma_test_proc_t cpu_lb_proc[] = {
    {
        .desc = "setup cpu port\n",
        .cb = bcma_testutil_port_cpu_setup_cb,
    },
    {
        .desc = "create default VLAN and add CPU into it\n",
        .cb = bcma_testcase_lb_vlan_set_cb,
    },
    {
        .desc = "configure a l2 user entry for copying to CPU\n",
        .cb = bcma_testcase_lb_l2_forward_add_cb,
    },
    {
        .desc = "TX/RX test\n",
        .cb = cpu_lb_txrx_cb,
    },
    {
        .desc = "delete the l2 user entry and VLAN configuration\n",
        .cb = bcma_testcase_lb_cleanup_cb,
    },
    {
        .desc = "Test End Checkers\n",
        .cb = bcma_testutil_traffic_check_cb,
    }
};

static bcma_test_op_t cpu_lb_op = {
    .select = bcma_testcase_lb_select,
    .parser = cpu_lb_parser,
    .help = bcma_testcase_lb_help,
    .recycle = bcma_testcase_lb_recycle,
    .procs = cpu_lb_proc,
    .proc_count = COUNTOF(cpu_lb_proc)
};

bcma_test_op_t *
bcma_testcase_cpu_lb_op_get(void)
{
    return &cpu_lb_op;
}
