/*! \file bcma_testcase_traffic_snake.c
 *
 *  Test case for snake traffic.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_sleep.h>
#include <sal/sal_time.h>
#include <sal/sal_assert.h>
#include <sal/sal_sem.h>
#include <bcmlt/bcmlt.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcma/test/util/bcma_testutil_l2.h>
#include <bcma/test/util/bcma_testutil_vlan.h>
#include <bcma/test/util/bcma_testutil_port.h>
#include <bcma/test/util/bcma_testutil_stat.h>
#include <bcma/test/util/bcma_testutil_packet.h>
#include <bcma/test/util/bcma_testutil_traffic.h>
#include <bcma/test/testcase/bcma_testcase_traffic_snake.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define TRAFFIC_SNAKE_PARAM_HELP \
    "  PortList      - Port list to be tested\n"\
    "  SnakePath     - Loop path type (INCreasing / DECreasing / TWOways)\n"\
    "  RunMode       - Loopback mode when running this testing (MAC / PHY / EXT)\n"\
    "  CounterPollInterval - Time interval between each counter polling (in sec)\n"\
    "  TimeInSeconds       - Total testing time (in sec)\n"\
    "  Length        - Packet length\n"\
    "  Pattern       - Packet pattern filled into payload (4-byte)\n"\
    "  PatternIncrement - Packet pattern increment\n"\
    "  Count         - Packet count\n"\
    "  Run2End       - Keep testing until the end\n"\
    "  CheckData     - Redirect packet back to CPU and check data content\n"\
    "  CheckLineRate - Check if raffic speed meets line rate\n\n"\
    "\nExample:\n"\
    "    Run traffic testing with default port list\n"\
    "      tr 39\n"\
    "    Run traffic testing (loop 1 -> 2 -> 3 -> 4 -> 1 and 4 -> 3 -> 2 -> 1 -> 4)\n"\
    "      tr 39 pl=1-4\n"\
    "    Run traffic testing (loop 4 -> 2 -> 1 -> 3 -> 4 and 3 -> 1 -> 2 -> 4 -> 3)\n"\
    "      tr 39 pl=4,2,1,3\n"\
    "    Run traffic testing with only decreasing loop (loop 3 -> 1 -> 2 -> 4 -> 3)\n"\
    "      tr 39 pl=4,2,1,3 sp=dec\n"\
    "    Run traffic testing in external-loopback mode\n"\
    "      tr 39 pl=4,2,1,3 rm=ext\n"\
    "    Run traffic testing for 10 sec, polling interval is 2 sec (it would check counter 5 times)\n"\
    "      tr 39 pl=4,2,1,3 tis=10 cpi=2\n"\
    "    Run traffic testing with 64-byte packet, payload is filled with 0xffffffff repeatedly\n"\
    "      tr 39 pl=4,2,1,3 l=64 p=0xffffffff\n"\
    "    Run traffic testing with 64-byte packet, payload is filled with 0x00000001, 00000002...\n"\
    "      tr 39 pl=4,2,1,3 l=64 p=0x00000001 pi=1\n"\
    "    Run traffic testing by injecting 10 packets into each testing port\n"\
    "      tr 39 pl=4,2,1,3 c=10\n"\
    "    Run traffic testing until the end even if there is some port traffic stopped\n"\
    "      tr 39 pl=4,2,1,3 r2e=true\n"\
    "    Run traffic testing without checking rx packet and line rate\n"\
    "      tr 39 pl=4,2,1,3 cd=false clr=false\n"

/* L2 action */
typedef enum traffic_snake_l2_action_e {
    L2_CONNECT = 0, /* setup loop by connecting each two adjacent ports */
    L2_REDIRECT,    /* redirect all packet back to cpu */
    L2_CLEAN        /* clean l2 setup */
} traffic_snake_l2_action_t;

/* Snake path type */
typedef enum traffic_snake_path_type_e {
    SNAKE_PATH_INC = 0,
    SNAKE_PATH_DEC,
    SNAKE_PATH_TWO
} traffic_snake_path_type_t;

static bcma_cli_parse_enum_t testcase_traffic_lb_str[] = {
    {"MAC", LB_TYPE_MAC},
    {"PHY", LB_TYPE_PHY},
    {"EXT", LB_TYPE_EXT},
    {NULL,   0}
};

static bcma_cli_parse_enum_t traffic_snake_path_type_str[] = {
    {"INCreasing", SNAKE_PATH_INC},
    {"DECreasing", SNAKE_PATH_DEC},
    {"TWOways", SNAKE_PATH_TWO},
    {NULL,   0}
};

/* default value */
static const shr_mac_t src_mac_inc = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
static const shr_mac_t dst_mac_inc = {0x66, 0x77, 0x00, 0x00, 0x00, 0x00};
static const shr_mac_t src_mac_dec = {0x00, 0x11, 0x22, 0x33, 0x44, 0x56};
static const shr_mac_t dst_mac_dec = {0x66, 0x88, 0x00, 0x00, 0x00, 0x00};

#define TRAFFIC_SNAKE_CPU_PORT                   (0)
#define TRAFFIC_SNAKE_VLAN_ID_BASE               (100)
#define TRAFFIC_SNAKE_VLAN_PROFILE_ID_BASE       (1)
#define TRAFFIC_SNAKE_DEFAULT_NETIF_ID           (1)
#define TRAFFIC_SNAKE_DEFAULT_RUNNING_TIME       (10)
#define TRAFFIC_SNAKE_DEFAULT_POLLING_INTERVAL   (2)
#define TRAFFIC_SNAKE_DEFAULT_RUNNING_MODE       (LB_TYPE_MAC)
#define TRAFFIC_SNAKE_DEFAULT_SNAKE_PATH         (SNAKE_PATH_TWO)
#define TRAFFIC_SNAKE_DEFAULT_PACKET_LENGTH      (1514)
#define TRAFFIC_SNAKE_DEFAULT_PACKET_PATTERN     (0xa5a4a3a2)
#define TRAFFIC_SNAKE_DEFAULT_PACKET_ETHERTYPE   (0xffff)
#define TRAFFIC_SNAKE_DEFAULT_PACKET_PATTERN_INC (0)
#define TRAFFIC_SNAKE_DEFAULT_INJECT_COUNT       (600)
#define TRAFFIC_SNAKE_DEFAULT_RUN_UNTIL_END      (FALSE)
#define TRAFFIC_SNAKE_DEFAULT_CHECK_RX           (TRUE)
#define TRAFFIC_SNAKE_DEFAULT_CHECK_SPEED        (TRUE)
#define TRAFFIC_SNAKE_DEFAULT_CHECK_SPEED_MARGIN (5) /* pass tolerance is 5% */
#define TRAFFIC_SNAKE_DEFAULT_GROUP_PKTCNT       (500)
#define TRAFFIC_SNAKE_DEFAULT_GROUP_RXCHECK_WAIT (2)   /* 2 sec */
#define TRAFFIC_SNAKE_TX_CHAN                    (0)
#define TRAFFIC_SNAKE_RX_CHAN                    (1)

/* Parameter */
typedef struct traffic_snake_param_s {
    /* Path of testing port */
    uint32_t port_path[BCMDRD_CONFIG_MAX_PORTS];

    /* Count of testing port */
    uint32_t port_count;

    /* Loopback type */
    testutil_port_lb_type_t lb_type;

    /* Snake path type */
    traffic_snake_path_type_t snake_path;

    /* Running time (in sec) */
    uint32_t runtime;

    /* Polling interval (in sec) */
    uint32_t interval;

    /* Packet length */
    uint32_t length;

    /* Packet pattern */
    uint32_t pattern;

    /* Packet pattern increment */
    uint32_t pattern_inc;

    /* Inject packet count */
    uint32_t inject_cnt;

    /* The packet nubmer in one group */
    uint32_t group_pktnum;

    /* group count */
    uint32_t group_cnt;

    /* Run until end even if error happen */
    int run_2_end;

    /* Whether to check packet back to CPU after unlock loop */
    int need_check_rx;

    /* Whether to check speed meet line rate */
    int need_check_speed;
} traffic_snake_param_t;

static int
traffic_snake_select(int unit)
{
    return true;
}

static int
traffic_snake_parse_param(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                          traffic_snake_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    bool pt_is_init = false;
    char *parse_path = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    /* setup parse table */
    bcma_cli_parse_table_init(cli, &pt);
    pt_is_init = true;

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PortList", "str",
                                  &parse_path, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "RunMode", "enum",
                                  &(param->lb_type),
                                  &testcase_traffic_lb_str));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "SnakePath", "enum",
                                  &(param->snake_path),
                                  &traffic_snake_path_type_str));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "CounterPollInterval", "int",
                                  &param->interval, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "TimeInSeconds", "int",
                                  &param->runtime, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Length", "int",
                                  &param->length, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Pattern", "hex",
                                  &param->pattern, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PatternIncrement", "hex",
                                  &param->pattern_inc, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Count", "int",
                                  &param->inject_cnt, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Run2End", "bool",
                                  &param->run_2_end, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "CheckData", "bool",
                                  &param->need_check_rx, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "CheckLineRate", "bool",
                                  &param->need_check_speed, NULL));

    /* parse start */
    if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (BCMA_CLI_ARG_CNT(a) != 0) {
        cli_out("%s: extra options starting with \"%s\"\n",
                BCMA_CLI_ARG_CMD(a), BCMA_CLI_ARG_CUR(a));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (show) {
        /* just show parsing result and exit function */
        bcma_cli_parse_table_show(&pt, NULL);
        SHR_EXIT();
    }

    /* translate port list */
    if (parse_path != NULL) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_parse_port_list(parse_path, param->port_path,
                                           BCMDRD_CONFIG_MAX_PORTS,
                                           &(param->port_count)));
    } else {
        SHR_IF_ERR_EXIT
            (bcma_testutil_default_port_list_get(unit, BCMDRD_PORT_TYPE_FPAN,
                                                 param->port_path,
                                                 BCMDRD_CONFIG_MAX_PORTS,
                                                 &(param->port_count)));
    }

exit:
    if (pt_is_init == true) {
        bcma_cli_parse_table_done(&pt);
    }
    SHR_FUNC_EXIT();
}

static int
traffic_snake_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                     uint32_t flags, void **bp)
{
    traffic_snake_param_t *tra_param = NULL;
    bool show = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_ALLOC(tra_param, sizeof(traffic_snake_param_t),
              "bcmaTestCaseTrafficPairPrm");
    SHR_NULL_CHECK(tra_param, SHR_E_MEMORY);

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    /* init */
    tra_param->port_count = 0;
    tra_param->lb_type = TRAFFIC_SNAKE_DEFAULT_RUNNING_MODE;
    tra_param->snake_path = TRAFFIC_SNAKE_DEFAULT_SNAKE_PATH;
    tra_param->runtime = TRAFFIC_SNAKE_DEFAULT_RUNNING_TIME;
    tra_param->interval = TRAFFIC_SNAKE_DEFAULT_POLLING_INTERVAL;
    tra_param->length = TRAFFIC_SNAKE_DEFAULT_PACKET_LENGTH;
    tra_param->pattern = TRAFFIC_SNAKE_DEFAULT_PACKET_PATTERN;
    tra_param->pattern_inc = TRAFFIC_SNAKE_DEFAULT_PACKET_PATTERN_INC;
    tra_param->run_2_end = TRAFFIC_SNAKE_DEFAULT_RUN_UNTIL_END;
    tra_param->need_check_rx = TRAFFIC_SNAKE_DEFAULT_CHECK_RX;
    tra_param->need_check_speed = TRAFFIC_SNAKE_DEFAULT_CHECK_SPEED;

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_traffic_default_pktcnt_get(unit,
                                                      &tra_param->inject_cnt));
    if (tra_param->inject_cnt == 0) {
        tra_param->inject_cnt = TRAFFIC_SNAKE_DEFAULT_INJECT_COUNT;
    }

    /* parse */
    SHR_IF_ERR_EXIT
        (traffic_snake_parse_param(unit, cli, a, tra_param, show));
    if (show == true) {
        SHR_FREE(tra_param);
        SHR_EXIT();
    }

    /* check */
    if (tra_param->runtime == 0 || tra_param->interval == 0 ||
        tra_param->inject_cnt == 0 || tra_param->port_count < 2) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (tra_param->lb_type == LB_TYPE_EXT && (tra_param->port_count % 2 != 0)) {
        cli_out("The port number must be even when running mode is external\n");
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * calculate the number of groups
     *  ex. assume that we need to inject 1200 packets
     *    ==> so we have three groups
     *     group A (0~499)     uses dst mac{0x66, 0x77, 0x00, 0x00, 0x00, 0x00}
     *     group B (500~999)   uses dst mac{0x66, 0x77, 0x00, 0x00, 0x00, 0x01}
     *     group C (1000~1199) uses dst mac{0x66, 0x77, 0x00, 0x00, 0x00, 0x02}
     */
    tra_param->group_pktnum = TRAFFIC_SNAKE_DEFAULT_GROUP_PKTCNT;
    if (tra_param->length > TRAFFIC_SNAKE_DEFAULT_PACKET_LENGTH) {
        /*
         * If the packet len is less than 1514, we keep 500 packets in each group
         * For the jumbo frame, decrease the packet number in the group to ease CPU pressure
         */
        tra_param->group_pktnum *= TRAFFIC_SNAKE_DEFAULT_PACKET_LENGTH;
        tra_param->group_pktnum /= tra_param->length;
    }
    tra_param->group_cnt = ((tra_param->inject_cnt - 1) /
                             tra_param->group_pktnum) + 1;
    if (tra_param->group_cnt > 256) {
        cli_out("The number of packets cannot exceed %"PRIu32"\n",
                tra_param->group_pktnum * 256);
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *bp = (void *)tra_param;

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(tra_param);
    }
    SHR_FUNC_EXIT();
}

static void
traffic_snake_help(int unit, void *bp)
{
    cli_out("%s", TRAFFIC_SNAKE_PARAM_HELP);
}

static int
traffic_snake_vlan_set_cb(int unit, void *bp, uint32_t option)
{
    traffic_snake_param_t *tra_param = (traffic_snake_param_t *)bp;
    int vid, pid;
    uint32_t port;
    bool need_tag_action;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    /* create vlan for each testing port */
    vid = TRAFFIC_SNAKE_VLAN_ID_BASE;
    pid = TRAFFIC_SNAKE_VLAN_PROFILE_ID_BASE;
    for (port = 0; port < tra_param->port_count; port++) {
        bcmdrd_pbmp_t pbmp, ubmp;
        int port_prev, port_next;

        /* pbmp and ubmp = self + prev + next + CPU port */
        port_prev = (port + tra_param->port_count - 1) % tra_param->port_count;
        port_next = (port + 1) % tra_param->port_count;
        BCMDRD_PBMP_CLEAR(pbmp);
        BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->port_path[port]);
        BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->port_path[port_prev]);
        BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->port_path[port_next]);
        BCMDRD_PBMP_PORT_ADD(pbmp, TRAFFIC_SNAKE_CPU_PORT);
        BCMDRD_PBMP_ASSIGN(ubmp, pbmp);

        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_create(unit, vid, pid));
        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_port_add(unit, vid, pbmp, ubmp, true));
        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_stp_set(unit, vid, pbmp,
                                        BCMA_TESTUTIL_STP_FORWARD));
        vid++;
        pid++;
    }

    /* iterate each port, assign different port-based vid */
    vid = TRAFFIC_SNAKE_VLAN_ID_BASE;
    for (port = 0; port < tra_param->port_count; port++) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_port_vlan_set(unit, tra_param->port_path[port],
                                              vid));
        vid++;
    }

    /* untag action, should be "ADD" because our inject packet is untagged */
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_feature_get
            (unit, BCMA_TESTUTIL_FT_NEED_TAG_ACTION_FOR_PVID,
             &need_tag_action));

    if (need_tag_action == true) {
        SHR_IF_ERR_EXIT
          (bcma_testutil_vlan_untag_action_add(
              unit, BCMA_TESTUTIL_VLAN_TAG_ACTION_ADD));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_snake_port_set_cb(int unit, void *bp, uint32_t option)
{
    traffic_snake_param_t *tra_param = (traffic_snake_param_t *)bp;
    uint32_t i;
    bcmdrd_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    BCMDRD_PBMP_CLEAR(pbmp);
    for (i = 0; i < tra_param->port_count; i++) {
        BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->port_path[i]);
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_port_setup(unit, tra_param->lb_type, pbmp));

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_snake_l2_forward(int unit, traffic_snake_param_t *tra_param,
                         traffic_snake_l2_action_t action,
                         unsigned int delay_sec)
{
    uint32_t p, g;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    /*
     * loop with increasing order
     *    ex. port_path = port 0, 1, 2, 3
     *   (if running mode is external, user needs to connect [0, 1] and [2, 3])
     *
     *         use l2 to forward  0 --> 1 (skip if external)
     *                            1 --> 2
     *                            2 --> 3 (skip if external)
     *                            3 --> 0
     */
    if (tra_param->snake_path == SNAKE_PATH_INC ||
        tra_param->snake_path == SNAKE_PATH_TWO) {

        shr_mac_t dst_mac;
        sal_memcpy(dst_mac, dst_mac_inc, sizeof(shr_mac_t));

        for (g = 0; g < tra_param->group_cnt; g++) {

            int vid = TRAFFIC_SNAKE_VLAN_ID_BASE;
            for (p = 0; p < tra_param->port_count; p++) {
                if ((tra_param->lb_type != LB_TYPE_EXT) || (p % 2 == 1)) {
                    int next_port;

                    if (p == (tra_param->port_count - 1)) {
                        next_port = tra_param->port_path[0];
                    } else {
                        next_port = tra_param->port_path[p + 1];
                    }

                    switch (action) {
                        case L2_CONNECT:
                            SHR_IF_ERR_EXIT
                                (bcma_testutil_l2_unicast_add
                                    (unit, next_port, dst_mac, vid, 0));
                            break;
                        case L2_REDIRECT:
                            SHR_IF_ERR_EXIT
                                (bcma_testutil_l2_unicast_update
                                    (unit, TRAFFIC_SNAKE_CPU_PORT, dst_mac,
                                     vid, 0));
                            break;
                        case L2_CLEAN:
                            SHR_IF_ERR_EXIT
                                (bcma_testutil_l2_unicast_delete
                                    (unit, next_port, dst_mac, vid, 0));
                            break;
                        default:
                            SHR_IF_ERR_EXIT(SHR_E_PARAM);

                    }
                }
                vid++;
            }

            /* increase dst mac for the next group */
            dst_mac[SHR_MAC_ADDR_LEN - 1]++;
            sal_sleep(delay_sec);
        }
    }

    /* loop with decreasing order
     *    ex. port_path = port 0, 1, 2, 3
     *   (if running mode is external, user needs to connect [0, 1] and [2, 3])
     *
     *         use l2 to forward  0 --> 3
     *                            1 --> 0 (skip if external)
     *                            2 --> 1
     *                            3 --> 2 (skip if external)
     */
    if (tra_param->snake_path == SNAKE_PATH_DEC ||
        tra_param->snake_path == SNAKE_PATH_TWO) {

        shr_mac_t dst_mac;
        sal_memcpy(dst_mac, dst_mac_dec, sizeof(shr_mac_t));

        for (g = 0; g < tra_param->group_cnt; g++) {

            int vid = TRAFFIC_SNAKE_VLAN_ID_BASE;
            for (p = 0; p < tra_param->port_count; p++) {
                if ((tra_param->lb_type != LB_TYPE_EXT) || (p % 2 == 0)) {
                    int next_port;

                    if (p == 0) {
                        next_port = tra_param->port_path[tra_param->port_count - 1];
                    } else {
                        next_port = tra_param->port_path[p - 1];
                    }

                    switch (action) {
                        case L2_CONNECT:
                            SHR_IF_ERR_EXIT
                                (bcma_testutil_l2_unicast_add
                                    (unit, next_port, dst_mac, vid, 0));
                            break;
                        case L2_REDIRECT:
                            SHR_IF_ERR_EXIT
                                (bcma_testutil_l2_unicast_update
                                    (unit, TRAFFIC_SNAKE_CPU_PORT, dst_mac,
                                     vid, 0));
                            break;
                        case L2_CLEAN:
                            SHR_IF_ERR_EXIT
                                (bcma_testutil_l2_unicast_delete
                                    (unit, next_port, dst_mac, vid, 0));
                            break;
                        default:
                            SHR_IF_ERR_EXIT(SHR_E_PARAM);
                    }
                }
                vid++;
            }
            /* increase dst mac for the next group */
            dst_mac[SHR_MAC_ADDR_LEN - 1]++;
            sal_sleep(delay_sec);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_snake_l2_forward_add_cb(int unit, void *bp, uint32_t option)
{
    return traffic_snake_l2_forward
               (unit, (traffic_snake_param_t *)bp, L2_CONNECT, 0);
}

static int
traffic_snake_txrx_cb(int unit, void *bp, uint32_t option)
{
    traffic_snake_param_t *tra_param = (traffic_snake_param_t *)bp;
    bcmdrd_dev_type_t pkt_dev_type;
    uint32_t curtime;
    const char *runmode_str = "\0";
    uint32_t idx = 0, direction_itr = 0;
    bool test_result = false;
    bool first_polling = true;
    bcmpkt_packet_t *packet = NULL;
    bcmpkt_packet_t *packet_clone = NULL;
    uint64_t target_speed = 0;
    bool counter_checker_create = false, counter_checker_start = false;
    bool rxdata_checker_create = false, rxdata_checker_start = false;
    int netif_id, pktdev_id;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    /* init */
    while (testcase_traffic_lb_str[idx].name != NULL) {
        if (tra_param->lb_type ==
            (testutil_port_lb_type_t)testcase_traffic_lb_str[idx].val) {
            runmode_str = testcase_traffic_lb_str[idx].name;
            break;
        }
        idx++;
    }
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Start testing (mode = %s)\n"), runmode_str));

    /* setup packet device */
    SHR_IF_ERR_EXIT
        (bcma_testutil_packet_dev_init
            (unit, TRAFFIC_SNAKE_TX_CHAN, TRAFFIC_SNAKE_RX_CHAN,
             tra_param->length + 4, BCMPKT_BPOOL_BCOUNT_DEF,
             &netif_id, &pktdev_id));

    /* allocate packet buffer */
    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &pkt_dev_type));

    SHR_IF_ERR_EXIT
        (bcmpkt_alloc(pktdev_id, tra_param->length, BCMPKT_BUF_F_TX, &packet));

    SHR_NULL_CHECK(packet, SHR_E_INTERNAL);

    /* create counter and rxdata checker */
    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_create(unit));

    counter_checker_create = true;

    if (tra_param->need_check_rx == TRUE) {

        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_rxdata_checker_create
                (unit, netif_id, pktdev_id, TRAFFIC_RXDATA_CHECKER_MODE_ANY));

        rxdata_checker_create = true;
    }

    /* determine target speed : minimun port speed * 95% */
    if (tra_param->need_check_speed == TRUE) {
        for (idx = 0; idx < tra_param->port_count; idx++) {
            uint32_t port;
            uint64_t speed_64;

            port = tra_param->port_path[idx];

            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(unit, "PC_PORT", &entry_hdl));

            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                                    BCMLT_PRIORITY_NORMAL));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_get(entry_hdl, "SPEED", &speed_64));

            bcmlt_entry_free(entry_hdl);
            entry_hdl = BCMLT_INVALID_HDL;

            /* if retruned speed = 40000
             *    ==> 40000 Mb/s = 40000 * 1000 * 1000 b/s
             */
            speed_64 = speed_64 * 1000 * 1000;
            cli_out("Port %d speed : ", port);
            bcma_testutil_stat_show(speed_64, 1, true);
            cli_out("b/s\n");

            if (idx == 0 || speed_64 < target_speed) {
                target_speed = speed_64;
            }
        }
        target_speed = target_speed *
                       (100 - TRAFFIC_SNAKE_DEFAULT_CHECK_SPEED_MARGIN) / 100;
        cli_out("  ==> Target speed : ");
        bcma_testutil_stat_show(target_speed, 1, true);
        cli_out("b/s\n\n");
    }


    /* setup counter checker */
    for (idx = 0; idx < tra_param->port_count; idx++) {
        /*
         *  Assumed that port list 0, 1, 2, 3
         *     ==> increasing loop : 0 --> 1 --> 2 --> 3 --> 0
         *         decreasing loop : 3 --> 2 --> 1 --> 0 --> 3
         *
         *  We need to check :
         *             port    0       1        2         3
         *                  TX  RX   TX  RX   TX  RX   TX  RX
         *
         *  (PHY/MAC/EXT)    V   V    V   V    V   V    V   V
         *   [TWO way]
         *
         *   (PHY/MAC)
         *  [INC/DECREASE]   V   V    V   V    V   V    V   V
         *
         *     (EXT)
         *   [INCREASE]      V            V    V            V
         *
         *     (EXT)
         *   [DECREASE]          V    V            V    V
         */

        bool need_check_tx = true, need_check_rx = true;
        char show_msg[TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN];
        int port = tra_param->port_path[idx];

        if (tra_param->lb_type == LB_TYPE_EXT) {
            if (tra_param->snake_path == SNAKE_PATH_INC) {
                if (idx % 2 == 0) {
                    need_check_rx = false;
                } else {
                    need_check_tx = false;
                }
            } else if (tra_param->snake_path == SNAKE_PATH_DEC) {
                if (idx % 2 == 0) {
                    need_check_tx = false;
                } else {
                    need_check_rx = false;
                }
            }
        }

        sal_snprintf(show_msg, TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN,
                     "Port %3"PRId32" :", port);

        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_counter_checker_add
                (unit, port, need_check_rx, need_check_tx,
                 target_speed, show_msg));
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_start(unit));
    counter_checker_start = true;

    /* clone packet and inject them into two forwarding loops
     *   (0: increasing, 1 : decreasing)
     */
    SHR_IF_ERR_EXIT
        (bcmpkt_packet_clone(pktdev_id, packet, &packet_clone));

    for (direction_itr = 0; direction_itr < 2; direction_itr++) {

        uint32_t inject_itr;
        uint32_t inject_port;
        const shr_mac_t *src_mac;
        shr_mac_t dst_mac;

        if (tra_param->snake_path == SNAKE_PATH_INC && direction_itr == 1) {
            continue;
        }
        if (tra_param->snake_path == SNAKE_PATH_DEC && direction_itr == 0) {
            continue;
        }

        if (direction_itr == 0) {
            src_mac = &src_mac_inc;
            sal_memcpy(dst_mac, dst_mac_inc, sizeof(shr_mac_t));
            inject_port = tra_param->port_path[0];
        } else {
            src_mac = &src_mac_dec;
            sal_memcpy(dst_mac, dst_mac_dec, sizeof(shr_mac_t));
            inject_port = tra_param->port_path[tra_param->port_count - 1];
        }

        for (inject_itr = 0; inject_itr < tra_param->inject_cnt; inject_itr++) {

            /* increase dst mac addr per 500 packets */
            if (inject_itr % tra_param->group_pktnum == 0 &&
                inject_itr / tra_param->group_pktnum >= 1) {
                dst_mac[SHR_MAC_ADDR_LEN - 1]++;
            }

            /* free previous and allocate new databuf for clone packet */
            if (packet_clone->data_buf) {
                SHR_IF_ERR_EXIT
                    (bcmpkt_data_buf_free(pktdev_id, packet_clone->data_buf));
                packet_clone->data_buf = NULL;
            }
            SHR_IF_ERR_EXIT
                (bcmpkt_data_buf_copy(pktdev_id, packet->data_buf,
                                      &packet_clone->data_buf));

            /* fill pattern */
            SHR_IF_ERR_EXIT
                (bcma_testutil_packet_fill
                    (packet_clone->data_buf, tra_param->length,
                     *src_mac, dst_mac, false, 0,
                     TRAFFIC_SNAKE_DEFAULT_PACKET_ETHERTYPE,
                     tra_param->pattern, tra_param->pattern_inc));

            if (tra_param->need_check_rx == TRUE) {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_traffic_rxdata_checker_add
                        (unit, -1, packet_clone->data_buf));
            }

            SHR_IF_ERR_EXIT
                (bcmpkt_fwd_port_set(pkt_dev_type, inject_port, packet_clone));

            /* tx */
            SHR_IF_ERR_EXIT
                (bcmpkt_tx(unit, TRAFFIC_SNAKE_DEFAULT_NETIF_ID, packet_clone));
        }
    }
    bcmpkt_free(pktdev_id, packet_clone);
    packet_clone = NULL;

    /* counter polling */
    curtime = 0;
    first_polling = true;
    while (curtime < tra_param->runtime) {
        sal_sleep(tra_param->interval);

        curtime += tra_param->interval;
        if (curtime > tra_param->runtime) {
            curtime = tra_param->runtime;
        }
        cli_out("TRAFFIC: running... (%"PRId32"%% complete)\n",
                (curtime * 100) / tra_param->runtime);

        if (tra_param->need_check_speed == TRUE && first_polling == false) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_traffic_counter_checker_execute(unit, true,
                                                               &test_result));
        } else {
            SHR_IF_ERR_EXIT
                (bcma_testutil_traffic_counter_checker_execute(unit, false,
                                                               &test_result));
        }

        if (test_result == false && tra_param->run_2_end == FALSE) {
            /* return immediately if user dont want to run to end */
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        first_polling = false;
    }

    if (test_result == false) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* redirect each group of packets back to CPU per 2 second */
    if (tra_param->need_check_rx == TRUE) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_rxdata_checker_start(unit));

        rxdata_checker_start = true;

        cli_out("redirecting all packets (%d groups) back to cpu....\n",
                tra_param->group_cnt);

        SHR_IF_ERR_EXIT
            (traffic_snake_l2_forward(unit, tra_param, L2_REDIRECT,
                                      TRAFFIC_SNAKE_DEFAULT_GROUP_RXCHECK_WAIT));

        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_rxdata_checker_execute(unit, 0,
                                                          &test_result));

        if (test_result == false) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (packet != NULL) {
        bcmpkt_free(pktdev_id, packet);
        packet = NULL;
    }
    if (packet_clone != NULL) {
        bcmpkt_free(pktdev_id, packet_clone);
        packet_clone = NULL;
    }
    if (counter_checker_start == true) {
        bcma_testutil_traffic_counter_checker_stop(unit);
    }
    if (counter_checker_create == true) {
        bcma_testutil_traffic_counter_checker_destroy(unit);
    }
    if (rxdata_checker_start == true) {
        bcma_testutil_traffic_rxdata_checker_stop(unit);
    }
    if (rxdata_checker_create == true) {
        bcma_testutil_traffic_rxdata_checker_destroy(unit);
    }
    (void)bcma_testutil_packet_dev_cleanup(unit);
    SHR_FUNC_EXIT();
}

static int
traffic_snake_cleanup_cb(int unit, void *bp, uint32_t option)
{
    traffic_snake_param_t *tra_param = (traffic_snake_param_t *)bp;
    bcmdrd_pbmp_t pbmp;
    uint32_t i;
    int vid = TRAFFIC_SNAKE_VLAN_ID_BASE;
    bool need_tag_action;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    BCMDRD_PBMP_CLEAR(pbmp);
    for (i = 0; i < tra_param->port_count; i++) {
        BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->port_path[i]);
    }

    /* restore port setting */
    SHR_IF_ERR_EXIT
        (bcma_testutil_port_clear(unit, tra_param->lb_type, pbmp, TRUE));

    /* cleanup vlan setting */
    BCMDRD_PBMP_PORT_ADD(pbmp, TRAFFIC_SNAKE_CPU_PORT);
    for (i = 0; i < tra_param->port_count; i++) {

        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_port_remove(unit, vid, pbmp));

        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_destroy(unit, vid));

        vid++;
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_feature_get
            (unit, BCMA_TESTUTIL_FT_NEED_TAG_ACTION_FOR_PVID,
             &need_tag_action));

    if (need_tag_action == true) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_untag_action_remove(unit));
    }

    /* cleanup l2 setting */
    SHR_IF_ERR_EXIT
        (traffic_snake_l2_forward(unit, (traffic_snake_param_t *)bp,
                                  L2_CLEAN, 0));

exit:
    SHR_FUNC_EXIT();
}

static bcma_test_proc_t traffic_snake_proc[] = {
    {
        .desc = "setup cpu port\n",
        .cb = bcma_testutil_port_cpu_setup_cb,
    },
    {
        .desc = "create VLAN and add ports into VLAN\n",
        .cb = traffic_snake_vlan_set_cb,
    },
    {
        .desc = "setup ports operating mode, wait link up\n",
        .cb = traffic_snake_port_set_cb,
    },
    {
        .desc = "add l2 forward entry\n",
        .cb = traffic_snake_l2_forward_add_cb,
    },
    {
        .desc = "TX/RX test\n",
        .cb = traffic_snake_txrx_cb,
    },
    {
        .desc = "cleanup port and vlan configuration\n",
        .cb = traffic_snake_cleanup_cb,
    },
    {
        .desc = "Test End Checkers\n",
        .cb = bcma_testutil_traffic_check_cb,
    }
};

static void
traffic_snake_recycle(int unit, void *bp)
{
    /* tra_param would be freed in testdb_run_teardown */
    return;
}

static bcma_test_op_t traffic_snake_op = {
    .select = traffic_snake_select,
    .parser = traffic_snake_parser,
    .help = traffic_snake_help,
    .recycle = traffic_snake_recycle,
    .procs = traffic_snake_proc,
    .proc_count = COUNTOF(traffic_snake_proc)
};

bcma_test_op_t *
bcma_testcase_traffic_snake_op_get(void)
{
    return &traffic_snake_op;
}
