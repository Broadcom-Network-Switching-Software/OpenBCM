/*! \file bcma_testcase_lb_mac.c
 *
 *  Test case for MAC loopback.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/testcase/bcma_testcase_lb_mac.h>
#include "bcma_testcase_lb_internal.h"

static int
mac_lb_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a, uint32_t flags, void **bp)
{
    return bcma_testcase_lb_parser(unit, cli, a, flags, bp, LB_TYPE_MAC);
}

static bcma_test_proc_t mac_lb_proc[] = {
    {
        .desc = "setup cpu port\n",
        .cb = bcma_testutil_port_cpu_setup_cb,
    },
    {
        .desc = "create VLAN and add ports into VLAN\n",
        .cb = bcma_testcase_lb_vlan_set_cb,
    },
    {
        .desc = "set ports operating mode and MAC loopback, wait link up\n",
        .cb = bcma_testcase_lb_port_set_cb,
    },
    {
        .desc = "add a l2 forward entry, the destination is CPU\n",
        .cb = bcma_testcase_lb_l2_forward_add_cb,
    },
    {
        .desc = "TX/RX test\n",
        .cb = bcma_testcase_lb_txrx_cb,
    },
    {
        .desc = "cleanup l2 forward and vlan configuration\n",
        .cb = bcma_testcase_lb_cleanup_cb,
    },
    {
        .desc = "Test End Checkers\n",
        .cb = bcma_testutil_traffic_check_cb,
    }
};

static bcma_test_op_t mac_lb_op = {
    .select = bcma_testcase_lb_select,
    .parser = mac_lb_parser,
    .help = bcma_testcase_lb_help,
    .recycle = bcma_testcase_lb_recycle,
    .procs = mac_lb_proc,
    .proc_count = COUNTOF(mac_lb_proc)
};

bcma_test_op_t *
bcma_testcase_mac_lb_op_get(void)
{
    return &mac_lb_op;
}
