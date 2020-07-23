/*! \file bcma_testcase_traffic_pair.c
 *
 *  Test case for pair traffic.
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
#include <bcma/test/util/bcma_testutil_traffic.h>
#include <bcma/test/testcase/bcma_testcase_traffic_pair.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

static const shr_mac_t default_src_mac = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
static const shr_mac_t default_dst_mac_u = {0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb};
static const shr_mac_t default_dst_mac_m = {0x01, 0x77, 0x88, 0x99, 0xaa, 0xbb};

#define TRAFFIC_PAIR_VLAN_ID_BASE               (100)
#define TRAFFIC_PAIR_VLAN_PROFILE_ID_BASE       (0)
#define TRAFFIC_PAIR_L2_MC_GROUP_ID_BASE        (0)
#define TRAFFIC_PAIR_DEFAULT_NETIF_ID           (1)
#define TRAFFIC_PAIR_DEFAULT_RUNNING_TIME       (10)
#define TRAFFIC_PAIR_DEFAULT_POLLING_INTERVAL   (2)
#define TRAFFIC_PAIR_DEFAULT_PACKET_LENGTH      (1514)
#define TRAFFIC_PAIR_DEFAULT_PACKET_PATTERN     (0xa5a4a3a2)
#define TRAFFIC_PAIR_DEFAULT_PACKET_ETHERTYPE   (0xffff)
#define TRAFFIC_PAIR_DEFAULT_PACKET_PATTERN_INC (0)
#define TRAFFIC_PAIR_DEFAULT_INJECT_COUNT       (1)
#define TRAFFIC_PAIR_DEFAULT_RUN_UNTIL_END      (FALSE)
#define TRAFFIC_PAIR_TX_CHAN                    (0)
#define TRAFFIC_PAIR_RX_CHAN                    (1)

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

#define TRAFFIC_PAIR_PARAM_HELP \
    "  PortList  - Port list to be tested (select adjacent two port as pair)\n"\
    "  PortListA - Port list for 1st port of the pair\n"\
    "  PortListB - Port list for 2nd port of the pair\n"\
    "  RunMode   - Loopback mode when running this testing (MAC / PHY / EXT)\n"\
    "  CounterPollInterval - Time interval between each counter polling (in sec)\n"\
    "  TimeInSeconds - Total testing time (in sec)\n"\
    "  Length    - Packet length\n"\
    "  Pattern   - Packet pattern filled into payload (4-byte)\n"\
    "  PatternIncrement - Packet pattern increment\n"\
    "  Count     - Packet count\n"\
    "  CheckPostLink - Check link down status after testing\n"\
    "  Run2End   - Keep testing until the end\n\n"\
    "\nExample:\n"\
    "    Run traffic testing with default port list\n"\
    "      tr 72\n"\
    "    Run traffic testing in phy-loopback mode (port pair [2, 4], [7, 8], [9, 10])\n"\
    "      tr 72 pl=2,4,7-10 rm=phy\n"\
    "    Run traffic testing in external-loopback mode (port pair [2, 8], [4, 9], [7, 10])\n"\
    "      tr 72 pla=2,4,7 plb=8-10 rm=ext\n"\
    "    Run traffic testing for 10 sec, polling interval is 2 sec (it would check counter 5 times)\n"\
    "      tr 72 pla=2,4,7 plb=8-10 tis=10 cpi=2\n"\
    "    Run traffic testing with 64-byte packet, payload is filled with 0xffffffff repeatedly\n"\
    "      tr 72 pla=2,4,7 plb=8-10 l=64 p=0xffffffff\n"\
    "    Run traffic testing with 64-byte packet, payload is filled with 0x00000001, 00000002...\n"\
    "      tr 72 pla=2,4,7 plb=8-10 l=64 p=0x00000001 pi=1\n"\
    "    Run traffic testing by injecting 10 packets into each testing port\n"\
    "      tr 72 pla=2,4,7 plb=8-10 c=10\n"\
    "    Run traffic testing until the end even if there is some port pair traffic stopped\n"\
    "      tr 72 pla=2,4,7 plb=8-10 r2e=true\n"

static bcma_cli_parse_enum_t testcase_traffic_lb_str[] = {
    {"MAC", LB_TYPE_MAC},
    {"PHY", LB_TYPE_PHY},
    {"EXT", LB_TYPE_EXT},
    {NULL,   0}
};

/* Parameter */
typedef struct traffic_pair_param_s {
    /* Port bitmap for the 1st member in the pair */
    bcmdrd_pbmp_t pbmp0;

    /* Port bitmap for the 2nd member in the pair */
    bcmdrd_pbmp_t pbmp1;

    /* Port pair count */
    uint32_t pair_cnt;

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
    uint32_t inject_cnt;

    /* Check link status after testing */
    int check_post_link;

    /* Run until end even if error happen */
    int run_2_end;
} traffic_pair_param_t;

static int
traffic_pair_select(int unit)
{
    return true;
}

static int
traffic_pair_parse_param(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                         traffic_pair_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    bool pt_is_init = false;
    char *parse_pbmp = NULL, *parse_pbmp_a = NULL, *parse_pbmp_b = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    /* setup parse table */
    bcma_cli_parse_table_init(cli, &pt);
    pt_is_init = true;

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PortList", "str", &parse_pbmp, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PortListA", "str", &parse_pbmp_a, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PortListB", "str", &parse_pbmp_b, NULL));
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
        (bcma_cli_parse_table_add(&pt, "CheckPostLink", "int",
                                  &param->check_post_link, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Count", "int",
                                  &param->inject_cnt, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Run2End", "bool",
                                  &param->run_2_end, NULL));
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

    /* Check pbmp input by user */
    if (parse_pbmp != NULL && (parse_pbmp_a != NULL || parse_pbmp_b != NULL)) {
        cli_out("cannot assign \"PortList\" and \"PortListA, PortListB\"\
                 at the same time\n");
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* translate pbmp */
    if (parse_pbmp_a == NULL && parse_pbmp_b == NULL) {
        /* assign pbmp0/1 by pairing two adjacent ports in parse_pbmp */
        bcmdrd_pbmp_t pbmp;
        int port_1 = 0, port_2 = 0;

        BCMDRD_PBMP_CLEAR(param->pbmp0);
        BCMDRD_PBMP_CLEAR(param->pbmp1);

        if (parse_pbmp != NULL) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_parse_port_bmp(parse_pbmp, &pbmp));
        } else {
            SHR_IF_ERR_EXIT
                (bcma_testutil_default_port_bmp_get
                    (unit, BCMDRD_PORT_TYPE_FPAN, &pbmp));
        }

        for (port_1 = 0; port_1 < BCMDRD_CONFIG_MAX_PORTS; port_1++) {
            if (BCMDRD_PBMP_MEMBER(pbmp, port_1)) {
                bool find_partner = false;

                for (port_2 = port_1 + 1; port_2 < BCMDRD_CONFIG_MAX_PORTS;
                     port_2++) {

                    if (BCMDRD_PBMP_MEMBER(pbmp, port_2)) {
                        find_partner = true;
                        break;
                    }
                }

                if (find_partner == false) {
                    cli_out("Warning: port %"PRId32" is isolated, "
                            "would not be tested\n", port_1);
                    break;
                } else {
                    BCMDRD_PBMP_PORT_ADD(param->pbmp0, port_1);
                    BCMDRD_PBMP_PORT_ADD(param->pbmp1, port_2);
                    port_1 = port_2; /* find the next pair */
                }
            }
        }

    } else {
        /* assign pbmp0/1 from parse_pbmp_a/b accordingly */
        if (parse_pbmp_a != NULL) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_parse_port_bmp(parse_pbmp_a, &param->pbmp0));
        }
        if (parse_pbmp_b != NULL) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_parse_port_bmp(parse_pbmp_b, &param->pbmp1));
        }
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
    traffic_pair_param_t *tra_param = NULL;
    bool show = false;
    int count_of_pbmp0 = 0, count_of_pbmp1 = 0, port;
    bcmdrd_pbmp_t pbmp_overlay;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_ALLOC(tra_param, sizeof(traffic_pair_param_t),
              "bcmaTestCaseTrafficPairPrm");
    SHR_NULL_CHECK(tra_param, SHR_E_MEMORY);

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    /* init */
    BCMDRD_PBMP_CLEAR(tra_param->pbmp0);
    BCMDRD_PBMP_CLEAR(tra_param->pbmp1);
    tra_param->lb_type = LB_TYPE_MAC;
    tra_param->runtime = TRAFFIC_PAIR_DEFAULT_RUNNING_TIME;
    tra_param->interval = TRAFFIC_PAIR_DEFAULT_POLLING_INTERVAL;
    tra_param->length = TRAFFIC_PAIR_DEFAULT_PACKET_LENGTH;
    tra_param->pattern = TRAFFIC_PAIR_DEFAULT_PACKET_PATTERN;
    tra_param->pattern_inc = TRAFFIC_PAIR_DEFAULT_PACKET_PATTERN_INC;
    tra_param->inject_cnt = TRAFFIC_PAIR_DEFAULT_INJECT_COUNT;
    tra_param->run_2_end = TRAFFIC_PAIR_DEFAULT_RUN_UNTIL_END;
    tra_param->check_post_link = TRUE;
    tra_param->pair_cnt = 0;

    /* parse */
    SHR_IF_ERR_EXIT
        (traffic_pair_parse_param(unit, cli, a, tra_param, show));
    if (show == true) {
        SHR_FREE(tra_param);
        SHR_EXIT();
    }

    /* check port number in pbmp0 and pbmp1 should be the same */
    BCMDRD_PBMP_ITER(tra_param->pbmp0, port) {
        count_of_pbmp0++;
    }
    BCMDRD_PBMP_ITER(tra_param->pbmp1, port) {
        count_of_pbmp1++;
    }
    if (count_of_pbmp0 != count_of_pbmp1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (count_of_pbmp0 == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    tra_param->pair_cnt = count_of_pbmp0;

    /* check pbmp0 and pbmp1 cannot overlay */
    BCMDRD_PBMP_ASSIGN(pbmp_overlay, tra_param->pbmp0);
    BCMDRD_PBMP_AND(pbmp_overlay, tra_param->pbmp1);
    if (BCMDRD_PBMP_NOT_NULL(pbmp_overlay)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* check other parameter */
    if (tra_param->runtime == 0 || tra_param->interval == 0 ||
        tra_param->inject_cnt == 0) {
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
traffic_pair_help(int unit, void *bp)
{
    cli_out("%s", TRAFFIC_PAIR_PARAM_HELP);
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
    traffic_pair_param_t *tra_param = (traffic_pair_param_t *)bp;
    int vid = TRAFFIC_PAIR_VLAN_ID_BASE;
    int pid = TRAFFIC_PAIR_VLAN_PROFILE_ID_BASE;
    int gid = TRAFFIC_PAIR_L2_MC_GROUP_ID_BASE;
    int port_0, port_1;
    bool need_l2_mc;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_feature_get
            (unit, BCMA_TESTUTIL_FT_NEED_L2_MC_FOR_VLAN_FLOOD, &need_l2_mc));

    TRAFFIC_PAIR_ITER(tra_param->pbmp0, tra_param->pbmp1, port_0, port_1) {

        bcmdrd_pbmp_t pbmp, ubmp;

        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_create(unit, vid, pid));

        BCMDRD_PBMP_CLEAR(ubmp);
        BCMDRD_PBMP_CLEAR(pbmp);
        BCMDRD_PBMP_PORT_ADD(pbmp, port_0);
        BCMDRD_PBMP_PORT_ADD(pbmp, port_1);
        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_port_add(unit, vid, pbmp, ubmp, true));

        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_stp_set(unit, vid, pbmp,
                                        BCMA_TESTUTIL_STP_FORWARD));

        if (need_l2_mc == true) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_l2_multicast_add(unit, gid, pbmp,
                                                default_dst_mac_m, vid, 0));
        }

        vid++;
        pid++;
        gid++;
    } TRAFFIC_PAIR_ITER_END(port_0, port_1);

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_pair_port_set_cb(int unit, void *bp, uint32_t option)
{
    traffic_pair_param_t *tra_param = (traffic_pair_param_t *)bp;
    bcmdrd_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    BCMDRD_PBMP_ASSIGN(pbmp, tra_param->pbmp0);
    BCMDRD_PBMP_OR(pbmp, tra_param->pbmp1);

    SHR_IF_ERR_EXIT
        (bcma_testutil_port_setup(unit, tra_param->lb_type, pbmp));

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_pair_txrx_cb(int unit, void *bp, uint32_t option)
{
    traffic_pair_param_t *tra_param = (traffic_pair_param_t *)bp;
    int port_0, port_1;
    int vid = TRAFFIC_PAIR_VLAN_ID_BASE;
    bcmdrd_dev_type_t pkt_dev_type;
    uint32_t curtime;
    const char *runmode_str = "\0";
    uint32_t idx = 0, pair_itr = 0;
    int netif_id, pktdev_id;
    bool counter_checker_create = false, counter_checker_start = false;
    bool test_result = false;
    bool need_l2_mc;
    bcmpkt_packet_t *packet = NULL;
    bcmpkt_packet_t *packet_clone = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_feature_get
            (unit, BCMA_TESTUTIL_FT_NEED_L2_MC_FOR_VLAN_FLOOD, &need_l2_mc));

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
             tra_param->length + 4, BCMPKT_BPOOL_BCOUNT_DEF,
             &netif_id, &pktdev_id));

    /* allocate packet buffer */
    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &pkt_dev_type));

    SHR_IF_ERR_EXIT
        (bcmpkt_alloc(pktdev_id, tra_param->length, BCMPKT_BUF_F_TX, &packet));

    SHR_NULL_CHECK(packet, SHR_E_INTERNAL);

    /* setup counter checker */
    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_create(unit));

    counter_checker_create = true;

    TRAFFIC_PAIR_ITER(tra_param->pbmp0, tra_param->pbmp1, port_0, port_1) {

        char show_msg[TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN];

        sal_snprintf(show_msg, TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN,
                     "Pair %3"PRId32", %3"PRId32" :", port_0, port_1);
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_counter_checker_add
                (unit, port_0, true, true, 0, show_msg));

    } TRAFFIC_PAIR_ITER_END(port_0, port_1);

    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_start(unit));

    counter_checker_start = true;

    /* clone packet to iterate each port pair and send packet */
    SHR_IF_ERR_EXIT
        (bcmpkt_packet_clone(pktdev_id, packet, &packet_clone));

    pair_itr = 0;
    TRAFFIC_PAIR_ITER(tra_param->pbmp0, tra_param->pbmp1, port_0, port_1) {

        uint32_t port_itr;
        uint32_t inject_itr;

        /* clear stat */
        SHR_IF_ERR_EXIT
            (bcma_testutil_stat_clear(unit, port_0));
        SHR_IF_ERR_EXIT
            (bcma_testutil_stat_clear(unit, port_1));

        /* inject packet to port 1 and port 2 */
        for (port_itr = 0; port_itr < 2; port_itr++) {

            int inject_port;
            inject_port = (port_itr == 0) ? port_0 : port_1;

            for (inject_itr = 0; inject_itr < tra_param->inject_cnt;
                 inject_itr++) {

                const shr_mac_t *dst_mac;

                if (need_l2_mc == true) {
                    dst_mac = &default_dst_mac_m;
                } else {
                    dst_mac = &default_dst_mac_u;
                }

                /* free previous and allocate new databuf for clone packet */
                if (packet_clone->data_buf) {
                    SHR_IF_ERR_EXIT
                        (bcmpkt_data_buf_free(pktdev_id,
                                              packet_clone->data_buf));
                    packet_clone->data_buf = NULL;
                }
                SHR_IF_ERR_EXIT
                    (bcmpkt_data_buf_copy(pktdev_id, packet->data_buf,
                                          &packet_clone->data_buf));

                /* fill pattern */
                SHR_IF_ERR_EXIT
                    (bcma_testutil_packet_fill
                        (packet_clone->data_buf, tra_param->length,
                         default_src_mac, *dst_mac, true, vid,
                         TRAFFIC_PAIR_DEFAULT_PACKET_ETHERTYPE,
                         tra_param->pattern, tra_param->pattern_inc));

                SHR_IF_ERR_EXIT
                    (bcmpkt_fwd_port_set(pkt_dev_type, inject_port,
                                         packet_clone));

                /* tx */
                SHR_IF_ERR_EXIT
                    (bcmpkt_tx(unit, TRAFFIC_PAIR_DEFAULT_NETIF_ID,
                               packet_clone));
             }
        }

        /* vlan id increase */
        vid++;
        pair_itr++;
    } TRAFFIC_PAIR_ITER_END(port_0, port_1);

    bcmpkt_free(pktdev_id, packet_clone);
    packet_clone = NULL;

    /* iterate each port pair to poll counter */
    curtime = 0;
    while (curtime < tra_param->runtime) {
        sal_sleep(tra_param->interval);

        curtime += tra_param->interval;
        if (curtime > tra_param->runtime) {
            curtime = tra_param->runtime;
        }

        cli_out("TRAFFIC: running... (%"PRId32"%% complete)\n",
                (curtime * 100) / tra_param->runtime);

        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_counter_checker_execute(unit, true,
                                                           &test_result));

        if (test_result == false && tra_param->run_2_end == FALSE) {
            /* return immediately if user dont want to run to end */
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

    if (test_result == false) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
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

    (void)bcma_testutil_packet_dev_cleanup(unit);
    SHR_FUNC_EXIT();
}

static int
traffic_pair_cleanup_cb(int unit, void *bp, uint32_t option)
{
    traffic_pair_param_t *tra_param = (traffic_pair_param_t *)bp;
    bcmdrd_pbmp_t pbmp;
    int port_0, port_1;
    int vid = TRAFFIC_PAIR_VLAN_ID_BASE;
    int gid = TRAFFIC_PAIR_L2_MC_GROUP_ID_BASE;
    bool need_l2_mc;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_feature_get
            (unit, BCMA_TESTUTIL_FT_NEED_L2_MC_FOR_VLAN_FLOOD, &need_l2_mc));

    BCMDRD_PBMP_ASSIGN(pbmp, tra_param->pbmp0);
    BCMDRD_PBMP_OR(pbmp, tra_param->pbmp1);

    SHR_IF_ERR_EXIT
        (bcma_testutil_port_clear(unit, tra_param->lb_type, pbmp,
                                  tra_param->check_post_link));

    TRAFFIC_PAIR_ITER(tra_param->pbmp0, tra_param->pbmp1, port_0, port_1) {

        BCMDRD_PBMP_CLEAR(pbmp);
        BCMDRD_PBMP_PORT_ADD(pbmp, port_0);
        BCMDRD_PBMP_PORT_ADD(pbmp, port_1);

        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_port_remove(unit, vid, pbmp));

        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_destroy(unit, vid));

        if (need_l2_mc == true) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_l2_multicast_delete(unit, gid, pbmp,
                                                   default_dst_mac_m, vid, 0));
        }

        vid++;
        gid++;
    } TRAFFIC_PAIR_ITER_END(port_0, port_1);

exit:
    SHR_FUNC_EXIT();
}

static bcma_test_proc_t traffic_pair_proc[] = {
    {
        .desc = "setup cpu port\n",
        .cb = bcma_testutil_port_cpu_setup_cb,
    },
    {
        .desc = "create VLAN and add ports into VLAN\n",
        .cb = traffic_pair_vlan_set_cb,
    },
    {
        .desc = "setup ports operating mode, wait link up\n",
        .cb = traffic_pair_port_set_cb,
    },
    {
        .desc = "TX/RX test\n",
        .cb = traffic_pair_txrx_cb,
    },
    {
        .desc = "cleanup port and vlan configuration\n",
        .cb = traffic_pair_cleanup_cb,
    },
    {
        .desc = "Test End Checkers\n",
        .cb = bcma_testutil_traffic_check_cb,
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
bcma_testcase_traffic_pair_op_get(void)
{
    return &traffic_pair_op;
}
