/*! \file bcma_testcase_traffic_pair_ext.c
 *  Pair traffic extension
 *  Extend the support for test case for pair traffic.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_sleep.h>
#include <sal/sal_assert.h>
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
#include <bcma/test/util/bcma_testutil_vlan.h>
#include <bcma/test/util/bcma_testutil_port.h>
#include <bcma/test/util/bcma_testutil_stat.h>
#include <bcma/test/util/bcma_testutil_packet.h>
#include <bcma/test/util/bcma_testutil_tm.h>
#include <bcma/test/util/bcma_testutil_traffic.h>
#include <bcma/test/testcase/bcma_testcase_traffic_pair.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>
#include <bcmtm/bcmtm_utils.h>


#define BSL_LOG_MODULE BSL_LS_APPL_TEST

static const shr_mac_t default_src_mac = {0x00, 0x11, 0x22, 0x33, 0x44, 0x00};
static const shr_mac_t default_dst_mac_u = {0x66, 0x77, 0x88, 0x99, 0xaa, 0x00};
static const shr_mac_t default_dst_mac_m = {0x01, 0x77, 0x88, 0x99, 0xaa, 0xbb};

#define NUM_PORTS 160

#define TRAFFIC_PAIR_VLAN_ID_BASE               (100)
#define TRAFFIC_PAIR_VLAN_PROFILE_ID_BASE       (0)
#define TRAFFIC_PAIR_L2_MC_GROUP_ID_BASE        (0)
#define TRAFFIC_PAIR_DEFAULT_NETIF_ID           (1)
#define TRAFFIC_PAIR_DEFAULT_RUNNING_TIME       (30)
#define TRAFFIC_PAIR_DEFAULT_POLLING_INTERVAL   (5)
#define TRAFFIC_PAIR_DEFAULT_PACKET_LENGTH      (1514)
#define TRAFFIC_PAIR_DEFAULT_PACKET_PATTERN     (0xa5a4a3a2)
#define TRAFFIC_PAIR_DEFAULT_PACKET_ETHERTYPE   (0xffff)
#define TRAFFIC_PAIR_DEFAULT_PACKET_PATTERN_INC (0)
#define TRAFFIC_PAIR_DEFAULT_INJECT_COUNT       (10)
#define TRAFFIC_PAIR_DEFAULT_RUN_UNTIL_END      (0)
#define TRAFFIC_PAIR_TX_CHAN                    (0)
#define TRAFFIC_PAIR_RX_CHAN                    (1)
#define TRAFFIC_PAIR_PKT_MAX_FRAME_SIZE         (9416)
#define TRAFFIC_PAIR_DEFAULT_CHECK_RX           (1)
#define TRAFFIC_PAIR_DEFAULT_RX_CHECK_WAIT      (1000000) /* 1 sec */
#define TRAFFIC_PAIR_CPU_PORT                   (0)
#define TRAFFIC_PAIR_DEFAULT_CHECK_PORT_BW      (1)
#define TRAFFIC_PAIR_DEFAULT_CHECK_Q_BW         (1)

#define TRAFFIC_PAIR_ITER(_pbmp0_, _pbmp1_, _port0_, _port1_)       \
    do {                                                            \
        _port0_ = 0;                                                \
        _port1_ = 0;                                                \
        BCMDRD_PBMP_ITER(_pbmp0_, _port0_) {                        \
            for (; _port1_ < BCMDRD_CONFIG_MAX_PORTS; _port1_++) {  \
                if (BCMDRD_PBMP_MEMBER(_pbmp1_, _port1_)) {         \
                    break;                                          \
                }                                                   \
            }                                                       \
            if (_port1_ == BCMDRD_CONFIG_MAX_PORTS) {               \
                SHR_ERR_EXIT(SHR_E_INTERNAL);                \
            }

#define TRAFFIC_PAIR_ITER_END(_port0_, _port1_)                     \
            _port1_++;                                              \
        }                                                           \
    } while (0)

#define TRAFFIC_PAIR_EXT_PARAM_HELP \
    "  PortList  - Port list to be tested \n"\
    "  RunMode   - Loopback mode when running this testing (MAC / PHY / AUTO)\n"\
    "              AUTO - Auto detect the externally connected link partner, \n"\
    "              could be same or a different port. Use MAC loopback mode for \n" \
    "              unconnected ports. \n"\
    "  CounterPollInterval - Time interval between each counter polling (in sec)\n"\
    "  TimeInSeconds - Traffic run time (in sec)\n"\
    "  Length    - Packet length\n"\
    "  LengthMIN    - Minimum Packet length\n"\
    "  LengthMAX    - Maximim Packet length\n"\
    "  Length Mode - Packet length mode \n"\
    "                FIXED - Fixed Length \n"\
    "                RAND  - Random Length in range LengthMIN to LengthMAX \n"\
    "  Pattern   - Packet pattern filled into payload (4-byte)\n"\
    "  PatternIncrement - Packet pattern increment \n"\
    "  CountMode - Mode to determine packet count per port(Auto / User).\n" \
    "              AUTO - Test auto calculates numbe rof packets to be injected \n"\
    "                     for line rate traffic. \n" \
    "              USER - Use packet count provided by user. \n"\
    "  Count     - Packet count \n"\
    "  FlowType  - Packet flow type used for traffic switching.(L2UC / L2MC) \n"\
    "  AllQueues - Use all Queues or only Queue 0 of each port. \n"\
    "  CheckData - Perform data integrity check at the end. \n"\
    "  CheckPortBw - Perform port bandwidth check at the end. \n"\
    "  CheckQueueBw - Perform queue bandwidth check at the end. \n"\
    "  MeterEnable - Enable random port metering. \n"\
    "  DynamicMeter - Dynamically apply and modify metering on ports during traffic. \n"\
    "                 Metering is removed before the actual test run. \n"\
    "                 Cannot be enabled along with meterEnable. \n"\
    "  CheckPostLink - Check link down status after testing\n"\
    "  Run2End   - Keep testing until the end\n\n"\
    "  TestMode   - Split test run into multiple plases \n"\
    "               COMPLETE run the complete test (default) \n"\
    "               TX_START Start the traffic and exit test without check \n"\
    "               TX_STOP Stop the traffic and run checks    \n"\
    "\nExample:\n"\
    "    Run traffic testing with default port list\n"\
    "      tr 572\n"\
    "    Run traffic testing in auto link-partner detect mode (port list: 2, 4, 7, 8, 9, 10)\n"\
    "      tr 572 pl=2,4,7-10 rm=auto\n"\
    "    Run traffic testing for 30 sec, polling interval is 5 sec (it would check counter 6 times)\n"\
    "    If teh CPI is too high, measured bandwidth deviation will be higher.\n"\
    "      tr 572 tis=30 cpi=5\n"\
    "    Run traffic testing with 64-byte packet, payload is filled with 0xffffffff repeatedly\n"\
    "      tr 572 l=64 p=0xffffffff\n"\
    "    Run traffic testing with 64-byte packet, payload is filled with 0x00000001, 00000002...\n"\
    "      tr 572  l=64 p=0x00000001 pi=1\n"\
    "    Run traffic testing by injecting 10 packets into each testing port\n"\
    "      tr 572 cm=user c=10\n"\
    "    Run traffic testing with MC traffic only on Queue 0 \n"\
    "      tr 572 ft=L2MC aq=false \n"\
    "    Run traffic testing with data integrity check disabled. \n"\
    "      tr 572 l=191 cd=false \n"\
    "    Run traffic testing with random port metering enabled \n"\
    "      tr 572 me=true \n"\
    "    Run traffic testing with dynamic port metering \n"\
    "      tr 572 dm=true \n"\
    "    Run traffic testing until the end even if there is some port pair traffic stopped\n"\
    "      tr 572 r2e=true\n"\
    "    Run traffic test with random packet size in range 64-256B\n"\
    "      tr 572 lm=rand lmin=64 lmax=256 \n"\
    "   Run traffic test to start traffic and exit without any checks \n"\
    "       tr 572 tm=tx_start \n"\
    "   Run traffic test to stop the traffic and run the checks \n"\
    "       Assumption is, traffic is tarted using same parameters to traffic"\
    "        start and tm=tx_start beore this. \n" \
    "       tr 572 tm=tx_stop \n"

static bcma_cli_parse_enum_t testcase_traffic_lb_str[] = {
    {"MAC", LB_TYPE_MAC},
    {"PHY", LB_TYPE_PHY},
    {"AUTO", LB_TYPE_AUTO},
    {NULL,   0}
};

static bcma_cli_parse_enum_t testcase_traffic_pkt_flow_str[] = {
    {"L2_UC", FLOW_TYPE_L2UC},
    {"L2_MC", FLOW_TYPE_L2MC},
    {NULL,   0}
};

static bcma_cli_parse_enum_t testutil_traffic_cnt_mode_str[] = {
    {"AUTO", CNT_MODE_AUTO},
    {"USER", CNT_MODE_USER},
    {NULL, 0}
};

static bcma_cli_parse_enum_t testutil_traffic_len_mode_str[] = {
    {"FIXED", LEN_MODE_FIXED},
    {"RAND", LEN_MODE_RANDOM},
    {NULL, 0}
};
static bcma_cli_parse_enum_t testutil_traffic_test_mode_str[] = {
    {"COMPLETE", TEST_MODE_COMPLETE},
    {"TX_START", TEST_MODE_TX_START},
    {"TX_STOP",  TEST_MODE_TX_STOP},
    {NULL, 0}
};


/* Parameter */
typedef struct traffic_pair_ext_param_s {
    /* Port bitmap */
    bcmdrd_pbmp_t pbmp;

    /* Port bitmap with external link in AUTO mode */
    bcmdrd_pbmp_t ext_link_pbmp;

    /* Loopback type. */
    testutil_port_lb_type_t lb_type;

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
    uint32_t inject_cnt[BCMDRD_CONFIG_MAX_PORTS];

    /* Run until end even if error happen */
    int run_2_end;

    /* Packet flow Type. */
    testutil_traffic_pkt_flow_type_t flow_type;

    /* Whether enable all queues or only one. */
    int all_queues;

    /* Count Mode. */
    testutil_traffic_cnt_mode_t cnt_mode;

   /* Whether to send packet back to CPU after unlock loop. */
    int check_rx;

    /* Port count */
    uint32_t port_cnt;

    /* Test Result */
    bool test_result;

    /* Link partner. */
    int lp_port[BCMDRD_CONFIG_MAX_PORTS];

    /* Enable random port shaping */
    int meter_en;

    /* Enable Dynamic port metering */
    int dyn_meter;

    /* Port final expected rate in Kbps */
    uint64_t port_rate[BCMDRD_CONFIG_MAX_PORTS];

    /* Packet length mode. */
    testutil_traffic_len_mode_t len_mode;

    /* Min packet length */
    int min_len;

    /* Max packet length */
    int max_len;

    /* Check port bandwidth enable */
    int check_port_bw;

    /* Check port bandwidth enable */
    int check_queue_bw;

    /* Test mode. */
    testutil_traffic_test_mode_t test_mode;

    /* Configure and start traffic */
    bool config_n_send;

    /* Stop traffic and check */
    bool stop_n_check;

    /* Length of mac oversize packet */
    int oversize_pkt[BCMDRD_CONFIG_MAX_PORTS];

    /* Check link status after testing */
    int check_post_link;

} traffic_pair_ext_param_t;

static int
traffic_pair_select(int unit)
{
    return true;
}

static int
traffic_pair_parse_param(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                         traffic_pair_ext_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    bool pt_is_init = false;
    char *parse_pbmp = NULL;
    int num_pkts, port;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    /* setup parse table */
    bcma_cli_parse_table_init(cli, &pt);
    pt_is_init = true;

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PortList", "str", &parse_pbmp, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "RunMode", "enum", &(param->lb_type),
                                  &testcase_traffic_lb_str));
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
        (bcma_cli_parse_table_add(&pt, "CountMode", "enum",
                                  &param->cnt_mode, &testutil_traffic_cnt_mode_str));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Count", "int",
                                  &num_pkts, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Run2End", "bool",
                                  &param->run_2_end, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "FlowType", "enum",
                                  &param->flow_type, &testcase_traffic_pkt_flow_str));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "AllQueues", "bool",
                                  &param->all_queues, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "MeterEnable", "bool",
                                  &param->meter_en, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "DynamicMeter", "bool",
                                  &param->dyn_meter, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "CheckData", "bool",
                                  &param->check_rx, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "CheckPortBw", "bool",
                                  &param->check_port_bw, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "CheckQueueBw", "bool",
                                  &param->check_queue_bw, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "LengthMode", "enum",
                                  &param->len_mode, &testutil_traffic_len_mode_str));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "LengthMIN", "int",
                                  &param->min_len, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "LengthMAX", "int",
                                  &param->max_len, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "TestMode", "enum",
                                  &param->test_mode, &testutil_traffic_test_mode_str));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "CheckPostLink", "int",
                                  &param->check_post_link, NULL));

    /* parse start */
    if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (BCMA_CLI_ARG_CNT(a) != 0) {
        cli_out("%s: extra options starting with \"%s\"\n",
                BCMA_CLI_ARG_CMD(a), BCMA_CLI_ARG_CUR(a));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (param->meter_en == 1 && param->dyn_meter == 1) {
        param->dyn_meter = 0;
        cli_out("MeterEnable and DynamicMeter cannot be enabled simultaneously."
                "Meter Enable takes precedence \n");
    }
    if (show) {
        /* just show parsing result and exit function */
        bcma_cli_parse_table_show(&pt, NULL);
        SHR_EXIT();
    }
    BCMDRD_PBMP_CLEAR(param->pbmp);

    if (parse_pbmp != NULL) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_parse_port_bmp(parse_pbmp, &(param->pbmp)));
    } else {
        SHR_IF_ERR_EXIT
            (bcma_testutil_default_port_bmp_get(unit,
                BCMDRD_PORT_TYPE_FPAN | BCMDRD_PORT_TYPE_MGMT, &(param->pbmp)));
    }
    BCMDRD_PBMP_ITER(param->pbmp, port) {
        param->inject_cnt[port] = num_pkts;
    }

exit:
    if (pt_is_init == true) {
        bcma_cli_parse_table_done(&pt);
    }
    SHR_FUNC_EXIT();
}

static int
traffic_pair_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                    uint32_t flags, void **bp)
{
    traffic_pair_ext_param_t *tra_param = NULL;
    bool show = false;
    int count_of_pbmp = 0, port;
    int port_speed;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_ALLOC(tra_param, sizeof(traffic_pair_ext_param_t),
              "bcmaTestCaseTrafficPairPrm");
    SHR_NULL_CHECK(tra_param, SHR_E_MEMORY);

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    /* init */
    BCMDRD_PBMP_CLEAR(tra_param->pbmp);
    tra_param->lb_type = LB_TYPE_MAC;
    tra_param->runtime = TRAFFIC_PAIR_DEFAULT_RUNNING_TIME;
    tra_param->interval = TRAFFIC_PAIR_DEFAULT_POLLING_INTERVAL;
    tra_param->length = TRAFFIC_PAIR_DEFAULT_PACKET_LENGTH;
    tra_param->pattern = TRAFFIC_PAIR_DEFAULT_PACKET_PATTERN;
    tra_param->pattern_inc = TRAFFIC_PAIR_DEFAULT_PACKET_PATTERN_INC;
    tra_param->run_2_end = TRAFFIC_PAIR_DEFAULT_RUN_UNTIL_END;
    tra_param->min_len = TRAFFIC_PAIR_DEFAULT_PACKET_LENGTH;
    tra_param->max_len = TRAFFIC_PAIR_DEFAULT_PACKET_LENGTH;
    tra_param->flow_type = FLOW_TYPE_L2UC;
    tra_param->all_queues = 1;
    tra_param->meter_en = 0;
    tra_param->dyn_meter = 0;
    tra_param->cnt_mode = CNT_MODE_AUTO;
    tra_param->len_mode = LEN_MODE_FIXED;
    tra_param->check_rx = TRAFFIC_PAIR_DEFAULT_CHECK_RX;
    tra_param->check_port_bw = TRAFFIC_PAIR_DEFAULT_CHECK_PORT_BW;
    tra_param->check_queue_bw = TRAFFIC_PAIR_DEFAULT_CHECK_Q_BW;
    tra_param->port_cnt = 0;
    tra_param->test_mode = TEST_MODE_COMPLETE;
    tra_param->config_n_send = true;
    tra_param->stop_n_check = true;
    tra_param->check_post_link = TRUE;

    /* parse */
    SHR_IF_ERR_EXIT
        (traffic_pair_parse_param(unit, cli, a, tra_param, show));
    if (show == true) {
        SHR_FREE(tra_param);
        SHR_EXIT();
    }

   /* check port number in pbmp. */
    BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
        count_of_pbmp++;
        tra_param->lp_port[port] = -1;
        /* Retrieve port_speed in Mbps */
        SHR_IF_ERR_EXIT
            (bcmtm_lport_max_speed_get(unit, port, &port_speed));
        /* Convert port rate from Mbps to Kbps */
        tra_param->port_rate[port] = port_speed * 1000 * 1000;
    }
    tra_param->port_cnt = count_of_pbmp;

    if (tra_param->cnt_mode == CNT_MODE_AUTO) {
        if (tra_param->len_mode == LEN_MODE_FIXED) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_tm_num_inject_pkst_line_rate_get(unit,
                    tra_param->pbmp,
                    tra_param->length, tra_param->inject_cnt));
        } else {
            /* Random packet length */
            SHR_IF_ERR_EXIT
                (bcma_testutil_tm_num_inject_pkst_line_rate_get(unit,
                    tra_param->pbmp,
                    (tra_param->min_len + tra_param->max_len) / 2,
                    tra_param->inject_cnt));
        }
    }

    /* check other parameter */
    if (tra_param->runtime == 0 || tra_param->interval == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if ( tra_param->test_mode == TEST_MODE_TX_START) {
        tra_param->stop_n_check = false;
    }
    if ( tra_param->test_mode == TEST_MODE_TX_STOP) {
        tra_param->config_n_send = false;
    }

    *bp = (void *)tra_param;

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(tra_param);
    }
    SHR_FUNC_EXIT();
}

static void
traffic_pair_help(int unit, void *bp)
{
    cli_out("%s", TRAFFIC_PAIR_EXT_PARAM_HELP);
}

/*
 *  For each port pair, we assign different VLAN ID
 *   ex. given pbmp0=0x13 pbmp1=0x2c
 *     ==> pair (0, 2) assign vid = 100
 *         pair (1, 3) assign vid = 101
 *         pair (4, 5) assign vid = 102
 */
static int
traffic_pair_vlan_set_cb(int unit, void *bp, uint32_t option)
{
    traffic_pair_ext_param_t *tra_param = (traffic_pair_ext_param_t *)bp;
    int vid, pid, port;
    bcmdrd_pbmp_t ubmp, bmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    if (tra_param->config_n_send == true) {
        BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
            vid = TRAFFIC_PAIR_VLAN_ID_BASE + port;
            pid = TRAFFIC_PAIR_VLAN_PROFILE_ID_BASE + port;

            SHR_IF_ERR_EXIT
                (bcma_testutil_vlan_create(unit, vid, pid));

            BCMDRD_PBMP_CLEAR(ubmp);
            BCMDRD_PBMP_CLEAR(bmp);
            BCMDRD_PBMP_PORT_ADD(bmp, port);
            if ((tra_param->lp_port[port] != port) &&
                (tra_param->lp_port[port] != -1)) {
                BCMDRD_PBMP_PORT_ADD(bmp, tra_param->lp_port[port]);
            }
            if (tra_param->check_rx == TRUE) {
                BCMDRD_PBMP_PORT_ADD(bmp, TRAFFIC_PAIR_CPU_PORT);
            }
            SHR_IF_ERR_EXIT
                (bcma_testutil_vlan_port_add(unit, vid, bmp, ubmp, false));
            SHR_IF_ERR_EXIT
                (bcma_testutil_vlan_stp_set(unit, vid, bmp,
                                            BCMA_TESTUTIL_STP_FORWARD));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/* generate mac address from base mac + port id */
static void
traffic_pair_mac_addr_generate(const shr_mac_t base, shr_port_t port,
                               shr_mac_t mac)
{
    uint8_t port_id[2];

    sal_memcpy(mac, base, sizeof(shr_mac_t));

    port_id[0] = port & 0xff;
    port_id[1] = (port >> 8) & 0xff;

    sal_memset((mac + SHR_MAC_ADDR_LEN - 1), port_id[0], sizeof(uint8_t));
    if (port_id[1] != 0) {
        sal_memset((mac + SHR_MAC_ADDR_LEN - 2), port_id[1], sizeof(uint8_t));
    }
}

static int
traffic_pair_forward_set_cb(int unit, void *bp, uint32_t option)
{
    traffic_pair_ext_param_t *tra_param = (traffic_pair_ext_param_t *)bp;
    int vid, gid, port;
    shr_mac_t dst_mac;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    if (tra_param->config_n_send == true) {
        BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
            if (tra_param->flow_type == FLOW_TYPE_L2UC) {
                traffic_pair_mac_addr_generate(default_dst_mac_u, port, dst_mac);
            } else {
                traffic_pair_mac_addr_generate(default_dst_mac_m, port, dst_mac);
            }

            vid = TRAFFIC_PAIR_VLAN_ID_BASE + port;
            gid = TRAFFIC_PAIR_L2_MC_GROUP_ID_BASE + port;

            if (tra_param->flow_type == FLOW_TYPE_L2UC) {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_l2_unicast_add(unit, port, dst_mac, vid, 0));
            } else {
                bcmdrd_pbmp_t pbmp;
                BCMDRD_PBMP_CLEAR(pbmp);
                BCMDRD_PBMP_PORT_ADD(pbmp, port);
                SHR_IF_ERR_EXIT
                    (bcma_testutil_l2_multicast_add(unit, gid, pbmp,
                                                    dst_mac, vid, 0));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
traffic_pair_port_set_cb(int unit, void *bp, uint32_t option)
{
    traffic_pair_ext_param_t *tra_param = (traffic_pair_ext_param_t *)bp;
    uint64_t rpkt;
    bcmdrd_dev_type_t pkt_dev_type;
    int netif_id, pktdev_id;
    int vid, port, rx_port;
    bcmpkt_packet_t *packet = NULL;
    uint64_t link;
    int oversize_pkt;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (tra_param->stop_n_check == true && tra_param->config_n_send == false) {
        /* Test mode will not work for external port to port
         * connection */
        BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
            tra_param->lp_port[port] = port;
        }
    }
    if (tra_param->config_n_send == true) {
        if (tra_param->lb_type == LB_TYPE_AUTO) {
            /* Auto-detect link partner. */
            SHR_IF_ERR_EXIT
                (bcma_testutil_packet_dev_init
                    (unit, TRAFFIC_PAIR_TX_CHAN, TRAFFIC_PAIR_RX_CHAN,
                     TRAFFIC_PAIR_PKT_MAX_FRAME_SIZE,
                     1000,
                     &netif_id, &pktdev_id));
            SHR_IF_ERR_EXIT
                (bcmpkt_dev_type_get(unit, &pkt_dev_type));
            BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_stat_clear(unit, port));
            }

            /* For AUTO mode, detect external link first. */
            SHR_IF_ERR_EXIT
                (bcma_testutil_port_setup(unit, LB_TYPE_AUTO, tra_param->pbmp));

            BCMDRD_PBMP_CLEAR(tra_param->ext_link_pbmp);
            BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
                vid = TRAFFIC_PAIR_VLAN_ID_BASE + port;
                if (tra_param->lp_port[port] != -1) {
                    continue;
                }

                SHR_IF_ERR_EXIT
                    (bcma_testutil_port_link_status_get(unit, port, &link));
                if (!link) {
                    cli_out("failed: Port %"PRId32" does not link up\n", port);
                    continue;
                }

                SHR_IF_ERR_EXIT
                    (bcmpkt_alloc(pktdev_id, tra_param->length, BCMPKT_BUF_F_TX, &packet));
                SHR_IF_ERR_EXIT
                    (bcma_testutil_packet_fill
                        (packet->data_buf, tra_param->length,
                         default_src_mac, default_dst_mac_u, true, vid,
                         TRAFFIC_PAIR_DEFAULT_PACKET_ETHERTYPE,
                         tra_param->pattern, tra_param->pattern_inc));
                SHR_IF_ERR_EXIT
                    (bcmpkt_fwd_port_set(pkt_dev_type, port, packet));
                SHR_IF_ERR_EXIT
                    (bcmpkt_tx(unit, TRAFFIC_PAIR_DEFAULT_NETIF_ID, packet));
                sal_sleep(1);
                BCMDRD_PBMP_ITER(tra_param->pbmp, rx_port) {
                    rpkt = 0;
                    SHR_IF_ERR_EXIT
                        (bcma_testutil_stat_get(unit, rx_port,
                                            SNMP_IF_DOT1P_IN_FRAMES, &rpkt));
                    if (rpkt != 0) {
                        tra_param->lp_port[port] = rx_port;
                        tra_param->lp_port[rx_port] = port;
                        BCMDRD_PBMP_PORT_ADD(tra_param->ext_link_pbmp, port);
                        BCMDRD_PBMP_PORT_ADD(tra_param->ext_link_pbmp, rx_port);
                        cli_out("External link detectected for port: %d with port: %d \n",
                            port, rx_port);
                        /* Clear stats only on port and link partner for speed-up*/
                        SHR_IF_ERR_EXIT
                            (bcma_testutil_stat_clear(unit, port));
                        SHR_IF_ERR_EXIT
                            (bcma_testutil_stat_clear(unit, rx_port));
                        break;
                    }
                }
                bcmpkt_free(pktdev_id, packet);
                packet = NULL;
            }
            (void)bcma_testutil_packet_dev_cleanup(unit);
        }

        if (tra_param->lb_type != LB_TYPE_AUTO) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_port_setup(unit, tra_param->lb_type,
                                          tra_param->pbmp));
            BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
                tra_param->lp_port[port] = port;
            }
        } else {
            bcmdrd_pbmp_t lb_pbmp;

            BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
                if (tra_param->lp_port[port] == -1) {
                    tra_param->lp_port[port] = port;
                }
            }

            BCMDRD_PBMP_ASSIGN(lb_pbmp, tra_param->pbmp);
            BCMDRD_PBMP_REMOVE(lb_pbmp, tra_param->ext_link_pbmp);

            SHR_IF_ERR_EXIT
                (bcma_testutil_port_setup(unit, LB_TYPE_MAC, lb_pbmp));
        }
    }

    if (tra_param->len_mode == LEN_MODE_FIXED) {
        oversize_pkt = tra_param->length;
    } else {
        oversize_pkt = tra_param->max_len;
    }
    BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
        /* Store the old oversize packet value. */
        SHR_IF_ERR_EXIT
            (bcma_testutil_port_oversize_pkt_get(unit, port,
                                                 &tra_param->oversize_pkt[port]));
        if (oversize_pkt > tra_param->oversize_pkt[port]) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_port_oversize_pkt_set(unit, port, oversize_pkt));
        }
    }

exit:
    if (packet != NULL) {
        bcmpkt_free(pktdev_id, packet);
        packet = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
traffic_pair_copy_to_cpu_for_pkt_integrity_check(int unit, bcmdrd_pbmp_t pbmp,
                                                 int flow_type, int *lp_port,
                                                 bool *result)
{
    bcmdrd_pbmp_t mc_pbmp;
    int vid, gid, port;
    shr_mac_t dst_mac;
    bool test_result = true;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(result, SHR_E_PARAM);
    *result = true;
    BCMDRD_PBMP_ITER(pbmp, port) {
        vid = TRAFFIC_PAIR_VLAN_ID_BASE + port;
        gid = TRAFFIC_PAIR_L2_MC_GROUP_ID_BASE + port;

        if (flow_type == FLOW_TYPE_L2UC) {
            traffic_pair_mac_addr_generate(default_dst_mac_u, port, dst_mac);
            SHR_IF_ERR_EXIT
                (bcma_testutil_l2_unicast_update(unit, TRAFFIC_PAIR_CPU_PORT,
                                                 dst_mac, vid, 0));
        } else {
            traffic_pair_mac_addr_generate(default_dst_mac_m, port, dst_mac);
            BCMDRD_PBMP_CLEAR(mc_pbmp);
            BCMDRD_PBMP_PORT_ADD(mc_pbmp, TRAFFIC_PAIR_CPU_PORT);
            SHR_IF_ERR_EXIT
                (bcma_testutil_l2_multicast_update(unit, gid, mc_pbmp,
                                                   dst_mac, vid, 0));
        }

        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_rxdata_checker_execute_per_port(unit,
                                        lp_port[port],
                                        TRAFFIC_PAIR_DEFAULT_RX_CHECK_WAIT,
                                        &test_result));
        if (test_result == false) {
            *result = false;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
traffic_pair_dynamic_meter(int unit, bcmdrd_pbmp_t pbmp)
{
    int port;
    int i, meter_perc;
    int dyn_meter_loop_count = 10;
    SHR_FUNC_ENTER(unit);

    /* Program a random value of metering for each port for loop count. */
    for (i = 0; i < dyn_meter_loop_count; i++) {
        cli_out("Dynamic metering on all ports, loop: %d \n", i);
        BCMDRD_PBMP_ITER(pbmp, port) {
            /* Random number between 1 & 100 */
            /* coverity[dont_call : FALSE] */
            meter_perc = (sal_rand() % 100) + 1;
            SHR_IF_ERR_EXIT
                (bcma_testutil_tm_port_meter(unit, port, meter_perc));
            sal_sleep(1);
        }
    }
    /* Clean up dynamic metering config. */
    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_tm_port_meter_cleanup(unit, port));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_pair_txrx_cb(int unit, void *bp, uint32_t option)
{
    traffic_pair_ext_param_t *tra_param = (traffic_pair_ext_param_t *)bp;
    int port, vid;
    bcmdrd_dev_type_t pkt_dev_type;
    uint32_t curtime;
    const char *runmode_str = "\0";
    uint32_t idx = 0;
    int netif_id, pktdev_id;
    bool counter_checker_create = false, counter_checker_start = false;
    bool rxdata_checker_create = false, rxdata_checker_start = false;
    bool test_result = false;
    bool perq_test_result = false;
    shr_mac_t dst_mac, src_mac;
    int pri_cfi, pri, cfi;
    bool queue_bw_checker_create = false, queue_bw_checker_start = false;
    int j;
    int num_uc_q = 1, num_mc_q = 1, num_qs = 1;
    int error_tolerance = 1;
    bool tm_lossless;
    int total_pkts = 0;
    bcmpkt_packet_t *packet = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);
    BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
        /* Retrieve number of UC/MC queues using any port */
        SHR_IF_ERR_EXIT
            (bcmtm_lport_num_ucq_get(unit, port, &num_uc_q));
        SHR_IF_ERR_EXIT
            (bcmtm_lport_num_mcq_get(unit, port, &num_mc_q));
        break;
    }

    if (tra_param->flow_type == FLOW_TYPE_L2MC && (num_mc_q == 0)) {
        cli_out("Flow Type set to L2MC in TM mode with 0 MC Queues \n");
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (tra_param->all_queues) {
        if (tra_param->flow_type == FLOW_TYPE_L2UC) {
            num_qs = num_uc_q;
        } else if (tra_param->flow_type == FLOW_TYPE_L2MC) {
            num_qs = num_mc_q;
        }
    }

    BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
        if (tra_param->inject_cnt[port] % num_qs != 0) {
            tra_param->inject_cnt[port] =
                ((tra_param->inject_cnt[port] / num_qs) + 1) * num_qs;
        }
        if (tra_param->config_n_send == true) {
            cli_out("Inject count of port : %d is %d \n", port,
                                                      tra_param->inject_cnt[port]);
        }
        total_pkts += tra_param->inject_cnt[port];
    }
    if (tra_param->config_n_send == true) {
        cli_out("Total pkt count: %d \n", total_pkts);
    }

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
            (unit, TRAFFIC_PAIR_TX_CHAN, TRAFFIC_PAIR_RX_CHAN,
             TRAFFIC_PAIR_PKT_MAX_FRAME_SIZE, BCMPKT_BPOOL_BCOUNT_DEF,
             &netif_id, &pktdev_id));

    /* alloacte packet buffer */
    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &pkt_dev_type));

    if (tra_param->len_mode == LEN_MODE_FIXED) {
        SHR_IF_ERR_EXIT
            (bcmpkt_alloc(pktdev_id, tra_param->length, BCMPKT_BUF_F_TX,
                          &packet));
    } else {
        SHR_IF_ERR_EXIT
            (bcmpkt_alloc(pktdev_id, tra_param->max_len, BCMPKT_BUF_F_TX,
                          &packet));
    }

    SHR_NULL_CHECK(packet, SHR_E_INTERNAL);

    if (tra_param->stop_n_check == true) {
        /* setup counter checker */
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_counter_checker_create(unit));
        counter_checker_create = true;

        /* setup queue bandwidth checker */
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_queue_bw_checker_create(unit));
        SHR_IF_ERR_EXIT
            (bcma_testutil_ctr_egr_perq_stat_create(unit, tra_param->pbmp));
        queue_bw_checker_create = true;

        SHR_IF_ERR_EXIT
            (bcma_testutil_tm_mmu_thd_mode_is_lossless(unit, &tm_lossless));
        if (tm_lossless == true) {
            /* Increase error tolerance in lossless mode to accommodate for Pause
             * frames sharing OBM space.*/
            error_tolerance = 5;
        }
        if (tra_param->len_mode == LEN_MODE_RANDOM) {
            /* In case of random mode for smaller packet sizes in the oversub range,
             * bandwidth predictability is not accurate, hence increasing error
             * tolerance */
            error_tolerance = 5;
            if (tra_param->all_queues) {
                /* BW distribution becomes much worse when multiple queue traffic is
                 * used. Queue bandwidth check is disabled in that case.*/
                tra_param->check_queue_bw = 0;
            }
        }

        BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
            char show_msg[TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN];
            /* Number of unicast queues based on current mode */
            SHR_IF_ERR_EXIT
                (bcmtm_lport_num_ucq_get(unit, port, &num_uc_q));
            SHR_IF_ERR_EXIT
                (bcmtm_lport_num_mcq_get(unit, port, &num_mc_q));

            sal_snprintf(show_msg, TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN,
                         "Port %3"PRId32" :", port);
            SHR_IF_ERR_EXIT
                (bcma_testutil_traffic_counter_checker_add
                    (unit, port, true, true,
                     (tra_param->port_rate[port] * (100 - error_tolerance) / 100),
                     show_msg));

            if (tra_param->flow_type == FLOW_TYPE_L2UC) {
                /* UC queue bandwidth check */
                if (tra_param->all_queues) {
                    for (j = 0; j < num_uc_q; j++) {
                        sal_snprintf(show_msg, TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN,
                                     "Port %3"PRId32" UC Queue: %d:", port, j);
                        SHR_IF_ERR_EXIT
                            (bcma_testutil_traffic_queue_bw_checker_add
                                (unit, port, j, TRAFFIC_Q_TYPE_UC,
                                 (tra_param->port_rate[port] /  num_uc_q),
                                 error_tolerance,
                                 20, show_msg));
                    }
                } else {
                    sal_snprintf(show_msg, TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN,
                        "Port %3"PRId32" UC Queue: 0:", port);
                    SHR_IF_ERR_EXIT
                        (bcma_testutil_traffic_queue_bw_checker_add
                            (unit, port, 0, TRAFFIC_Q_TYPE_UC,
                             tra_param->port_rate[port], error_tolerance,
                             20, show_msg));
                }
            } else {
                if (tra_param->all_queues) {
                    for (j = 0; j < num_mc_q; j++) {
                        sal_snprintf(show_msg, TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN,
                                     "Port %3"PRId32" MC Queue: %d:", port, j);
                        SHR_IF_ERR_EXIT
                            (bcma_testutil_traffic_queue_bw_checker_add
                                (unit, port, j, TRAFFIC_Q_TYPE_MC,
                                 (tra_param->port_rate[port] / num_mc_q),
                                 error_tolerance,
                                 20, show_msg));
                    }
                } else {
                    sal_snprintf(show_msg, TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN,
                        "Port %3"PRId32" MC Queue: 0:", port);
                    SHR_IF_ERR_EXIT
                        (bcma_testutil_traffic_queue_bw_checker_add
                            (unit, port, 0, TRAFFIC_Q_TYPE_MC,
                             tra_param->port_rate[port],
                             error_tolerance,
                             20, show_msg));
                }
            }
        }


        if (tra_param->config_n_send == false) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_traffic_counter_checker_start_no_stat_clear(unit));
        } else {
            SHR_IF_ERR_EXIT
                (bcma_testutil_traffic_counter_checker_start(unit));
        }
        counter_checker_start = true;

        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_queue_bw_checker_start(unit));
        queue_bw_checker_start = true;

        if (tra_param->check_rx == TRUE) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_traffic_rxdata_checker_create
                    (unit, netif_id, pktdev_id, TRAFFIC_RXDATA_CHECKER_MODE_PORT));
            rxdata_checker_create = true;
        }
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_tm_global_drop_stat_clear(unit));

    if (tra_param->config_n_send == true) {
        /* clear stat */
        BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_stat_clear(unit, port));
            SHR_IF_ERR_EXIT
                (bcma_testutil_tm_stat_clear(unit, port));
        }
    }

    BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
        int inject_itr;
        int num_packets = tra_param->inject_cnt[port];
        int pkt_length;

        traffic_pair_mac_addr_generate(default_src_mac, port, src_mac);
        if (tra_param->flow_type == FLOW_TYPE_L2UC) {
            traffic_pair_mac_addr_generate(default_dst_mac_u, port, dst_mac);
        } else {
            traffic_pair_mac_addr_generate(default_dst_mac_m, port, dst_mac);
        }

        vid = TRAFFIC_PAIR_VLAN_ID_BASE + port;

        for (inject_itr = 0; inject_itr < num_packets; inject_itr++) {
            if (tra_param->len_mode == LEN_MODE_FIXED) {
                pkt_length = tra_param->length;
            } else {
                /* random */
                /* coverity[dont_call : FALSE] */
                pkt_length = (sal_rand() %
                              (tra_param->max_len - tra_param->min_len)) +
                              tra_param->min_len;
            }

            if (tra_param->all_queues == 1) {
                pri_cfi = (inject_itr % num_qs);
            } else {
                pri_cfi = 0;
            }
            pri = pri_cfi % 8;
            cfi = pri_cfi / 8;

            if (packet == NULL) {
                if (tra_param->len_mode == LEN_MODE_FIXED) {
                    SHR_IF_ERR_EXIT
                        (bcmpkt_alloc(pktdev_id, tra_param->length,
                                      BCMPKT_BUF_F_TX, &packet));
                } else {
                    SHR_IF_ERR_EXIT
                        (bcmpkt_alloc(pktdev_id, tra_param->max_len,
                                      BCMPKT_BUF_F_TX, &packet));
                }
                SHR_NULL_CHECK(packet, SHR_E_INTERNAL);
            }

            SHR_IF_ERR_EXIT
                (bcma_testutil_packet_fill
                    (packet->data_buf, pkt_length,
                     src_mac, dst_mac, true, ((pri << 13) + (cfi << 12) + vid),
                     TRAFFIC_PAIR_DEFAULT_PACKET_ETHERTYPE,
                     tra_param->pattern, tra_param->pattern_inc));

            SHR_IF_ERR_EXIT
                (bcmpkt_fwd_port_set(pkt_dev_type, port, packet));

            if (tra_param->check_rx == TRUE && tra_param->stop_n_check == true) {
                    SHR_IF_ERR_EXIT
                        (bcma_testutil_traffic_rxdata_checker_add
                               (unit, tra_param->lp_port[port], packet->data_buf));
            }

            if (tra_param->config_n_send == true) {
                SHR_IF_ERR_EXIT
                    (bcmpkt_tx(unit, TRAFFIC_PAIR_DEFAULT_NETIF_ID, packet));
            }

            if (packet != NULL) {
                bcmpkt_free(pktdev_id, packet);
                packet = NULL;
            }
        }
    }

    if (tra_param->stop_n_check == true) {
        /* Change metering rate and remove after starting traffic,
         * before further checks. */
        if (tra_param->dyn_meter) {
            SHR_IF_ERR_EXIT
                (traffic_pair_dynamic_meter(unit, tra_param->pbmp));
        }
        curtime = 0;
        while (curtime < tra_param->runtime) {
            sal_sleep(tra_param->interval);

            curtime += tra_param->interval;
            if (curtime > tra_param->runtime) {
                curtime = tra_param->runtime;
            }

            cli_out("TRAFFIC: running... (%"PRId32"%% complete)\n",
                    (curtime * 100) / tra_param->runtime);
            if (curtime >=  tra_param->runtime) {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_traffic_counter_checker_execute(unit,
                                                                   tra_param->check_port_bw,
                                                                   &test_result));
                if (tra_param->check_port_bw) {
                    SHR_IF_ERR_EXIT
                        (bcma_testutil_traffic_check_result_disp_add(unit,
                                        test_result, "Port Bandwidth Check"));
                }

                SHR_IF_ERR_EXIT
                    (bcma_testutil_traffic_queue_bw_checker_execute(unit,
                                                                    tra_param->check_queue_bw,
                                                                    &perq_test_result));
                if (tra_param->check_queue_bw) {
                    SHR_IF_ERR_EXIT
                        (bcma_testutil_traffic_check_result_disp_add(unit,
                                        perq_test_result, "Queue Bandwidth Check"));
                }
            } else {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_traffic_counter_checker_execute(unit, false,
                                                                   &test_result));
                SHR_IF_ERR_EXIT
                    (bcma_testutil_traffic_queue_bw_checker_execute(unit, false,
                                                                    &perq_test_result));
            }

            if (test_result == false || perq_test_result == false) {

                if (tra_param->run_2_end == 0) {
                    /* return immediately if user dont want to run to end */
                    /* Display all check result */
                    SHR_IF_ERR_EXIT
                        (bcma_testutil_traffic_check_result_disp_add(unit, false,
                                                                  "Test Result"));
                    SHR_IF_ERR_EXIT
                        (bcma_testutil_traffic_check_result_display(unit));
                    SHR_ERR_EXIT(SHR_E_FAIL);
                } else {
                    /* Set test result to false and continue */
                    tra_param->test_result = false;
                }
            }
        }

        /* redirect all packet back to CPU and check */
        if (tra_param->check_rx == TRUE) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_traffic_rxdata_checker_start(unit));
            rxdata_checker_start = true;

            LOG_VERBOSE(BSL_LOG_MODULE,
                 (BSL_META("drop all the packets and copy to cpu\n")));

            SHR_IF_ERR_EXIT
                (traffic_pair_copy_to_cpu_for_pkt_integrity_check(unit,
                                                              tra_param->pbmp,
                                                              tra_param->flow_type,
                                                              tra_param->lp_port,
                                                              &test_result));
            if (test_result == false) {
                tra_param->test_result = false;
            } else {
                tra_param->test_result = true;
            }
            SHR_IF_ERR_EXIT
                (bcma_testutil_traffic_check_result_disp_add(unit,
                                                         test_result,
                                                         "Data Integrity Check"));
        }
    }

exit:
    if (packet != NULL) {
        bcmpkt_free(pktdev_id, packet);
        packet = NULL;
    }

    if (counter_checker_start == true) {
        bcma_testutil_traffic_counter_checker_stop(unit);
    }
    if (counter_checker_create == true) {
        bcma_testutil_traffic_counter_checker_destroy(unit);
    }

    if (queue_bw_checker_start == true) {
        bcma_testutil_traffic_queue_bw_checker_stop(unit);
    }
    if (queue_bw_checker_create == true) {
        bcma_testutil_traffic_queue_bw_checker_destroy(unit);
        bcma_testutil_ctr_egr_perq_stat_destroy(unit, tra_param->pbmp);
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
traffic_pair_l2_entry_cleanup(int unit, int port, int flow_type)
{
    int vid, gid;
    shr_mac_t dst_mac;
    bcmdrd_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);

    vid = TRAFFIC_PAIR_VLAN_ID_BASE + port;
    gid = TRAFFIC_PAIR_L2_MC_GROUP_ID_BASE + port;

    if (flow_type == FLOW_TYPE_L2UC) {
        traffic_pair_mac_addr_generate(default_dst_mac_u, port, dst_mac);
    } else {
        traffic_pair_mac_addr_generate(default_dst_mac_m, port, dst_mac);
    }

    if (flow_type == FLOW_TYPE_L2UC) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_l2_unicast_delete(unit, port, dst_mac, vid, 0));
    } else {
        BCMDRD_PBMP_CLEAR(pbmp);
        BCMDRD_PBMP_PORT_ADD(pbmp, port);
        SHR_IF_ERR_EXIT
            (bcma_testutil_l2_multicast_delete(unit, gid, pbmp,
                                               dst_mac, vid, 0));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_pair_cleanup_cb(int unit, void *bp, uint32_t option)
{
    traffic_pair_ext_param_t *tra_param = (traffic_pair_ext_param_t *)bp;
    int vid, port, oversize_pkt;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (tra_param->stop_n_check == true) {
        if (tra_param->lb_type != LB_TYPE_AUTO) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_port_clear(unit, tra_param->lb_type,
                                          tra_param->pbmp,
                                          tra_param->check_post_link));
        } else {
            /*
             * For AUTO mode, some ports have external link, others will
             * set to MAC loopback.
             */

            bcmdrd_pbmp_t lb_pbmp;

            BCMDRD_PBMP_ASSIGN(lb_pbmp, tra_param->pbmp);
            BCMDRD_PBMP_REMOVE(lb_pbmp, tra_param->ext_link_pbmp);

            /* Clear MAC loopback and remove ports from link scan. */
            SHR_IF_ERR_EXIT
                (bcma_testutil_port_clear(unit, LB_TYPE_MAC, lb_pbmp,
                                          tra_param->check_post_link));

            /* Remove ports from link scan for external link. */
            SHR_IF_ERR_EXIT
                (bcma_testutil_port_clear(unit, LB_TYPE_EXT,
                                          tra_param->ext_link_pbmp,
                                          tra_param->check_post_link));
        }

        BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
            vid = TRAFFIC_PAIR_VLAN_ID_BASE + port;
            SHR_IF_ERR_EXIT
                (bcma_testutil_vlan_destroy(unit, vid));
            SHR_IF_ERR_EXIT
                (traffic_pair_l2_entry_cleanup(unit, port, tra_param->flow_type));
            SHR_IF_ERR_EXIT
                (bcma_testutil_tm_port_meter_cleanup(unit, port));

            SHR_IF_ERR_EXIT
                (bcma_testutil_port_oversize_pkt_get(unit, port, &oversize_pkt));
            if (oversize_pkt != tra_param->oversize_pkt[port]) {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_port_oversize_pkt_set(unit, port,
                                                tra_param->oversize_pkt[port]));
            }
        }

        SHR_IF_ERR_EXIT
            (bcma_testutil_drv_qos_one_to_one_map_destroy(unit, tra_param->pbmp));

    }
exit:
    SHR_FUNC_EXIT();
}

static int
traffic_pair_checks_cb(int unit, void *bp, uint32_t option)
{
    bool test_result;
    traffic_pair_ext_param_t *tra_param = (traffic_pair_ext_param_t *)bp;
    int port;
    int rv;
    uint64_t value = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);
    test_result = true;
    if (tra_param->stop_n_check == true) {
        BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_traffic_src_dst_packet_count_check(unit, port,
                                        tra_param->lp_port[port], &test_result));
            if (test_result == false) {
                tra_param->test_result = false;
            }
        }
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_check_result_disp_add(unit, test_result,
                                               "TxRx Packet Count Check"));
        test_result = true;
        BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
            value = 0;
            SHR_IF_ERR_EXIT
                (bcma_testutil_tm_total_queue_pkt_drop_get(unit, port, &value));
            if (value != 0) {
                tra_param->test_result = false;
                test_result = false;
            }
        }
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_check_result_disp_add(unit, test_result,
                                               "MMU Queue Drop Count Check"));
        test_result = true;
        BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
            value = 0;
            SHR_IF_ERR_EXIT
                (bcma_testutil_tm_ing_pkt_drop_get(unit, port, &value));
            if (value != 0) {
                tra_param->test_result = false;
                test_result = false;
            }
        }
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_check_result_disp_add(unit, test_result,
                                               "MMU SrcPort Drop Count Check"));
        test_result = true;
        BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
            value = 0;
            if (bcmtm_obm_port_validation(unit, port)) {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_tm_obm_pkt_drop_get(unit, port, &value));
            }
            if (value != 0) {
                tra_param->test_result = false;
                test_result = false;
            }
        }
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_check_result_disp_add(unit, test_result,
                                               "OBM Drop Count Check"));
        SHR_IF_ERR_EXIT
            (bcma_testutil_tm_global_drop_count_check(unit, &test_result));
        if (test_result == false) {
            tra_param->test_result = false;
        }
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_check_result_disp_add(unit, test_result,
                                               "MMU Global Drop Count Check"));
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_port_error_packets_check(unit, tra_param->pbmp,
                                                            &test_result));
        if (test_result == false) {
            tra_param->test_result = false;
        }
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_check_result_disp_add(unit, test_result,
                                               "MAC Error Packet Check"));
        rv = bcma_testutil_drv_check_health(unit, &test_result);
        if (rv == SHR_E_UNAVAIL) {
            /* Ignore if check health is not available for a device */
            rv = SHR_E_NONE;
        } else {
            if (test_result == false) {
                tra_param->test_result = false;
            }
        }
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_check_result_disp_add(unit, test_result,
                                               "Check Health"));
        if (test_result == false) {
            tra_param->test_result = false;
        }

        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_check_result_disp_add(unit,
                            tra_param->test_result, "Test Result"));
        /* Display all check result */
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_check_result_display(unit));
        if (tra_param->test_result == false) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();

}

static int
traffic_pair_pri_queue_map_setup_cb(int unit, void *bp, uint32_t option)
{
    traffic_pair_ext_param_t *tra_param = (traffic_pair_ext_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (tra_param->config_n_send == true) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_drv_qos_one_to_one_map_create(unit, tra_param->pbmp));
    }

exit:
    SHR_FUNC_EXIT();
}


static int
traffic_pair_add_mmu_setup_cb(int unit, void *bp, uint32_t option)
{
    traffic_pair_ext_param_t *tra_param = (traffic_pair_ext_param_t *)bp;
    int port;
    uint64_t port_exp_rate[BCMDRD_CONFIG_MAX_PORTS] = {0};
    int meter_perc;
    int meter_port;
    uint64_t meter_rate;
    int length;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (tra_param->config_n_send == true) {
        if (tra_param->len_mode == LEN_MODE_FIXED) {
            length = tra_param->length;
        } else {
            length = (tra_param->min_len + tra_param->max_len) / 2;
        }
        SHR_IF_ERR_EXIT
            (bcma_testutil_drv_traffic_expeceted_rate_get(unit, tra_param->pbmp,
                                                          tra_param->pbmp, length,
                                                          BCMDRD_CONFIG_MAX_PORTS,
                                                          port_exp_rate));

        if (tra_param->meter_en == 1) {
            BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
                /* Randomly apply/not apply port metering. */
                /* coverity[dont_call : FALSE] */
                meter_port = (sal_rand() % 2);
                if (meter_port == 1) {
                    /* Random number between 1 & 100 */
                    /* coverity[dont_call : FALSE] */
                    meter_perc = (sal_rand() % 100) + 1;
                    SHR_IF_ERR_EXIT
                        (bcma_testutil_tm_port_meter(unit, port, meter_perc));
                    meter_rate = (tra_param->port_rate[port] * meter_perc) / 100;
                    tra_param->port_rate[port] = MIN(meter_rate, port_exp_rate[port]);
                } else {
                    tra_param->port_rate[port] = port_exp_rate[port] ;
                }
            }
        } else {
            BCMDRD_PBMP_ITER(tra_param->pbmp, port) {
                tra_param->port_rate[port] = port_exp_rate[port];
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static bcma_test_proc_t traffic_pair_proc[] = {
    {
        .desc = "setup cpu port\n",
        .cb = bcma_testutil_port_cpu_setup_cb,
    },
    {
        .desc = "Setup ports operating mode, wait link up\n",
        .cb = traffic_pair_port_set_cb,
    },
    {
        .desc = "Create VLAN and add ports into VLAN\n",
        .cb = traffic_pair_vlan_set_cb,
    },
    {
        .desc = "Create Forwarding path\n",
        .cb = traffic_pair_forward_set_cb,
    },
    {
        .desc = "Create Queue mapping\n",
        .cb = traffic_pair_pri_queue_map_setup_cb,
    },
    {
        .desc = "Additional MMu setup\n",
        .cb = traffic_pair_add_mmu_setup_cb,
    },
    {
        .desc = "TX/RX test\n",
        .cb = traffic_pair_txrx_cb,
    },
    {
        .desc = "Cleanup port and vlan configuration\n",
        .cb = traffic_pair_cleanup_cb,
    },
    {
        .desc = "Test End Checkers\n",
        .cb = traffic_pair_checks_cb,
    }
};

static void
traffic_pair_recycle(int unit, void *bp)
{
    /* tra_param would be freed in testdb_run_teardown */
    return;
}

static bcma_test_op_t traffic_pair_op = {
    .select = traffic_pair_select,
    .parser = traffic_pair_parser,
    .help = traffic_pair_help,
    .recycle = traffic_pair_recycle,
    .procs = traffic_pair_proc,
    .proc_count = COUNTOF(traffic_pair_proc)
};

bcma_test_op_t *
bcma_testcase_traffic_pair_ext_op_get(void)
{
    return &traffic_pair_op;
}
