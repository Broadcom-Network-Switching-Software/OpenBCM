/*! \file bcma_testcase_traffic_latency.c
 *
 *  Test case for latency traffic.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <sal/sal_sleep.h>
#include <sal/sal_time.h>
#include <sal/sal_sem.h>
#include <bcmbd/bcmbd_ts.h>
#include <bcmlt/bcmlt.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmpkt/bcmpkt_buf.h>
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
#include <bcma/test/testcase/bcma_testcase_traffic_latency.h>
#include <bcmlrd/chip/bcmlrd_id.h>
#include <bcmlrd/bcmlrd_table.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define TRAFFIC_LATENCY_PARAM_HELP \
    "  PortList      - The ports to be tested\n"\
    "                  (one port for phy loopback and 2 ports for external loopback)\n"\
    "  PortTraffic   - Generate traffic with 2 ports\n"\
    "  PortTimeStamp - Generate timestamp with 2 ports for latency calculation\n"\
    "  RunMode       - Loopback mode when running this testing (PHY / EXT)\n"\
    "  LAtencyType   - Latency type that the user wants to measure (I2I / E2E / E2I)\n"\
    "  Length        - Packet length\n"\
    "  Count         - Packet count\n"\
    "  CheckLineRate - Check if traffic speed meets line rate\n\n"\
    "\nExample:\n"\
    "    Run testing at default single port with PHY loopback\n"\
    "      tr 507\n"\
    "    Run testing at port 20 with PHY loopback\n"\
    "      tr 507 pl=20\n"\
    "    Run testing at port 20, and generate traffic with port 1, 4\n"\
    "      tr 507 pl=20 pt=1,4\n"\
    "    Run testing at port 20, and generate timestamp with port 1, 4\n"\
    "      tr 507 pl=20 pts=1,4\n"\
    "    Run testing with external cable connected between port 20, 21\n"\
    "      tr 507 pl=20,21 rm=ext\n"\
    "    Run testing to measure the latency from egress to ingress\n"\
    "      tr 507 lat=E2I\n"\
    "    Run testing by injecting 2000 packets with 70-byte length\n"\
    "      tr 507 c=2000 l=70\n"\
    "    Run testing without checking traffic to meet line rate or not\n"\
    "      tr 507 c=2000 l=70 clr=false\n"

/* L2 action */
typedef enum traffic_latency_l2_action_e {
    L2_CONNECT = 0, /* setup L2 entries by connecting each two adjacent ports */
    L2_REDIRECT,    /* redirect all packets from the loop to the sender */
    L2_CLEAN        /* clean l2 setup */
} traffic_latency_l2_action_t;

static bcma_cli_parse_enum_t testcase_traffic_lb_str[] = {
    {"PHY", LB_TYPE_PHY},
    {"EXT", LB_TYPE_EXT},
    {NULL,   0}
};

/*! Latency type */
typedef enum traffic_lat_type_e {
    LAT_TYPE_INGRESS_TO_INGRESS = 0,
    LAT_TYPE_EGRESS_TO_EGRESS,
    LAT_TYPE_EGRESS_TO_INGRESS
} traffic_lat_type_t;

static bcma_cli_parse_enum_t testcase_traffic_lat_str[] = {
    {"I2I", LAT_TYPE_INGRESS_TO_INGRESS},
    {"E2E", LAT_TYPE_EGRESS_TO_EGRESS},
    {"E2I", LAT_TYPE_EGRESS_TO_INGRESS},
    {NULL,   0}
};

/* default value */
static const shr_mac_t src_mac_inc = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
static const shr_mac_t dst_mac_inc = {0x66, 0x77, 0x00, 0x00, 0x00, 0x00};

#define TRAFFIC_LATENCY_CPU_PORT                   (0)
#define TRAFFIC_LATENCY_VLAN_ID_BASE               (100)
#define TRAFFIC_LATENCY_VLAN_PROFILE_ID_BASE       (1)
#define TRAFFIC_LATENCY_DEFAULT_NETIF_ID           (1)
#define TRAFFIC_LATENCY_DEFAULT_RUNNING_TIME       (3)
#define TRAFFIC_LATENCY_DEFAULT_POLLING_INTERVAL   (1)
#define TRAFFIC_LATENCY_DEFAULT_RUNNING_MODE       (LB_TYPE_PHY)
#define TRAFFIC_LATENCY_DEFAULT_LATENCY_TYPE       (LAT_TYPE_INGRESS_TO_INGRESS)
#define TRAFFIC_LATENCY_DEFAULT_PACKET_LENGTH      (1514)
#define TRAFFIC_LATENCY_DEFAULT_PACKET_PATTERN     (0xa5a4a3a2)
#define TRAFFIC_LATENCY_DEFAULT_PACKET_ETHERTYPE   (0xffff)
#define TRAFFIC_LATENCY_DEFAULT_PACKET_PATTERN_INC (0)
#define TRAFFIC_LATENCY_DEFAULT_INJECT_COUNT       (600)
#define TRAFFIC_LATENCY_DEFAULT_CHECK_SPEED        (TRUE)
#define TRAFFIC_LATENCY_DEFAULT_CHECK_SPEED_MARGIN (5) /* pass tolerance is 5% */
#define TRAFFIC_LATENCY_DEFAULT_GROUP_PKTCNT       (500)
#define TRAFFIC_LATENCY_DEFAULT_GROUP_RXCHECK_WAIT (2)   /* 2 sec */
#define TRAFFIC_LATENCY_TX_CHAN                    (0)
#define TRAFFIC_LATENCY_RX_CHAN                    (1)

/* Parameter */
typedef struct traffic_latency_param_s {
    /*
     * The post list to be tested
     *  a) phy-loopback needs only 1 port
     *  b) ext-loopback needs 2 ports
     */
    uint32_t port_list[2];

    /* Use these two ports to create the traffic in a loop */
    uint32_t port_traffic[2];

    /*
     * Use these two ports as sender and receiver
     * to add timestamp for latency calculation
     */
    uint32_t port_timestamp[2];

    /* Total number of the used ports during this test */
    uint32_t total_port_count;

    /* Total used port list */
    uint32_t total_port_list[6];

    /* Loopback type */
    testutil_port_lb_type_t lb_type;

    /* Latency type */
    traffic_lat_type_t lat_type;

    /* Packet length */
    uint32_t length;

    /* Inject packet count */
    uint32_t inject_cnt;

    /* The packet nubmer in one group */
    uint32_t group_pktnum;

    /* Group count */
    uint32_t group_cnt;

    /* Whether to check speed meet line rate */
    int need_check_speed;

    /* Thread of timesync */
    shr_thread_t thread_timesync;

    /* Unit id */
    int unit;
} traffic_latency_param_t;

static int
traffic_latency_select(int unit)
{
    return true;
}

static int
traffic_latency_parse_portlist(traffic_latency_param_t *tra_param,
                               const char *user_str,
                               uint32_t (*port_list)[2],
                               uint32_t port_list_num,
                               bcmdrd_pbmp_t *ret_user_pbmp,
                               uint32_t **ret_need_def_port_list,
                               uint32_t *ret_need_def_port_num,
                               const char *msg_str)
{
    uint32_t i;
    uint32_t *parse_list = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);
    SHR_NULL_CHECK(ret_user_pbmp, SHR_E_PARAM);
    SHR_NULL_CHECK(port_list, SHR_E_PARAM);
    SHR_NULL_CHECK(ret_need_def_port_list, SHR_E_PARAM);
    SHR_NULL_CHECK(ret_need_def_port_num, SHR_E_PARAM);
    SHR_NULL_CHECK(msg_str, SHR_E_PARAM);

    if (port_list_num > 2) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (user_str != NULL) {
        /*
         * parse user_str if the user has specified the port list
         *   ==> then write the result into port_list and ret_user_pbmp
         */
        uint32_t parse_count = 0;

        SHR_ALLOC(parse_list, sizeof(uint32_t) * BCMDRD_CONFIG_MAX_PORTS,
                  "bcmaTestCaseLatencyParseList");
        SHR_NULL_CHECK(parse_list, SHR_E_MEMORY);

        SHR_IF_ERR_EXIT
            (bcma_testutil_parse_port_list(user_str, parse_list,
                                           BCMDRD_CONFIG_MAX_PORTS,
                                           &parse_count));
        if (parse_count != port_list_num) {
            cli_out("Needs %"PRIu32" ports to %s\n", port_list_num, msg_str);
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        for (i = 0; i < port_list_num; i++) {
            BCMDRD_PBMP_PORT_ADD(*ret_user_pbmp, parse_list[i]);
            (*port_list)[i] = parse_list[i];
            tra_param->total_port_count++;
        }
    } else {
        /* Otherwise, write the port into ret_need_def_port_ptr */
        for (i = 0; i < port_list_num; i++) {
            ret_need_def_port_list[(*ret_need_def_port_num)++] = &((*port_list)[i]);
        }
    }

exit:
    SHR_FREE(parse_list);
    SHR_FUNC_EXIT();
}

static int
traffic_latency_parse_param(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                            traffic_latency_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    bool pt_is_init = false;
    char *port_list_user = NULL, *port_traffic_user = NULL,
         *port_timestamp_user = NULL;
    bcmdrd_pbmp_t pbmp_user, pbmp_default;
    uint32_t i, port;
    uint32_t need_default_port_count = 0;
    uint32_t *need_default_port_ptr[BCMDRD_CONFIG_MAX_PORTS] = {NULL};

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    /* setup parse table */
    bcma_cli_parse_table_init(cli, &pt);
    pt_is_init = true;

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PortList", "str",
                                  &port_list_user, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PortTraffic", "str",
                                  &port_traffic_user, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PortTimeStamp", "str",
                                  &port_timestamp_user, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "RunMode", "enum",
                                  &(param->lb_type),
                                  &testcase_traffic_lb_str));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "LAtencyType", "enum",
                                  &(param->lat_type),
                                  &testcase_traffic_lat_str));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Length", "int",
                                  &param->length, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Count", "int",
                                  &param->inject_cnt, NULL));

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

    /* check loopback mode */
    if (param->lb_type != LB_TYPE_PHY && param->lb_type != LB_TYPE_EXT) {
        cli_out("Latency test only support PHY or EXT loopback.\n");
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* translate port list from the user */
    BCMDRD_PBMP_CLEAR(pbmp_user);
    param->total_port_count = 0;
    need_default_port_count = 0;

    if (param->lb_type == LB_TYPE_PHY) {
        SHR_IF_ERR_EXIT
            (traffic_latency_parse_portlist
                (param, port_list_user, &(param->port_list), 1,
                 &pbmp_user, need_default_port_ptr, &need_default_port_count,
                 "be tested with PHY loopback"));
    } else {
        SHR_IF_ERR_EXIT
            (traffic_latency_parse_portlist
                (param, port_list_user, &(param->port_list), 2,
                 &pbmp_user, need_default_port_ptr, &need_default_port_count,
                 "be tested with EXT loopback"));
    }

    SHR_IF_ERR_EXIT
        (traffic_latency_parse_portlist
            (param, port_traffic_user, &(param->port_traffic), 2,
             &pbmp_user, need_default_port_ptr, &need_default_port_count,
             "generate traffic"));

    SHR_IF_ERR_EXIT
        (traffic_latency_parse_portlist
            (param, port_timestamp_user, &(param->port_timestamp), 2,
             &pbmp_user, need_default_port_ptr, &need_default_port_count,
             "generate timestamp"));


    /* for the port that user not specified, assign it by default */
    SHR_IF_ERR_EXIT
        (bcma_testutil_default_port_bmp_get(unit, BCMDRD_PORT_TYPE_FPAN,
                                            &pbmp_default));
    i = 0;
    BCMDRD_PBMP_ITER(pbmp_default, port) {
        if (i == need_default_port_count) {
            break;
        }
        if (BCMDRD_PBMP_MEMBER(pbmp_user, port)) {
            continue;
        }
        *(need_default_port_ptr[i]) = port;
        param->total_port_count++;
        i++;
    }

    /* check the final port number and list */
    if ((param->lb_type == LB_TYPE_PHY && param->total_port_count != 5) ||
        (param->lb_type == LB_TYPE_EXT && param->total_port_count != 6)) {
        cli_out("Incorrect port number: %d\n", param->total_port_count);
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    i = 0;
    param->total_port_list[i++] = param->port_traffic[0];
    param->total_port_list[i++] = param->port_traffic[1];
    param->total_port_list[i++] = param->port_timestamp[0];
    param->total_port_list[i++] = param->port_list[0];
    if (param->lb_type == LB_TYPE_EXT) {
        param->total_port_list[i++] = param->port_list[1];
    }
    param->total_port_list[i++] = param->port_timestamp[1];

exit:
    if (pt_is_init == true) {
        bcma_cli_parse_table_done(&pt);
    }
    SHR_FUNC_EXIT();
}

static int
traffic_latency_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                       uint32_t flags, void **bp)
{
    traffic_latency_param_t *tra_param = NULL;
    bool show = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_ALLOC(tra_param, sizeof(traffic_latency_param_t),
              "bcmaTestCaseTrafficPairPrm");
    SHR_NULL_CHECK(tra_param, SHR_E_MEMORY);

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    /* init */
    sal_memset(tra_param, 0, sizeof(traffic_latency_param_t));
    tra_param->unit = unit;
    tra_param->lb_type = TRAFFIC_LATENCY_DEFAULT_RUNNING_MODE;
    tra_param->lat_type = TRAFFIC_LATENCY_DEFAULT_LATENCY_TYPE;
    tra_param->length = TRAFFIC_LATENCY_DEFAULT_PACKET_LENGTH;
    tra_param->need_check_speed = TRAFFIC_LATENCY_DEFAULT_CHECK_SPEED;
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_traffic_default_pktcnt_get(unit,
                                                      &tra_param->inject_cnt));
    if (tra_param->inject_cnt == 0) {
        tra_param->inject_cnt = TRAFFIC_LATENCY_DEFAULT_INJECT_COUNT;
    }

    /* parse */
    SHR_IF_ERR_EXIT
        (traffic_latency_parse_param(unit, cli, a, tra_param, show));
    if (show == true) {
        SHR_FREE(tra_param);
        SHR_EXIT();
    }

    /*
     * calculate the number of groups
     *  ex. assume that we need to inject 1200 packets
     *    ==> so we have three groups
     *     group A (0~499)     uses dst mac{0x66, 0x77, 0x00, 0x00, 0x00, 0x00}
     *     group B (500~999)   uses dst mac{0x66, 0x77, 0x00, 0x00, 0x00, 0x01}
     *     group C (1000~1199) uses dst mac{0x66, 0x77, 0x00, 0x00, 0x00, 0x02}
     */

    tra_param->group_pktnum = TRAFFIC_LATENCY_DEFAULT_GROUP_PKTCNT;
    if (tra_param->length > TRAFFIC_LATENCY_DEFAULT_PACKET_LENGTH) {
        /*
         * If the packet len is less than 1514, we keep 500 packets in each group
         * For the jumbo frame, decrease the packet number in the group to ease CPU pressure
         */
        tra_param->group_pktnum *= TRAFFIC_LATENCY_DEFAULT_PACKET_LENGTH;
        tra_param->group_pktnum /= tra_param->length;
    }
    tra_param->group_cnt = ((tra_param->inject_cnt - 1) /
                             tra_param->group_pktnum) + 1;
    if (tra_param->group_cnt > 256) {
        cli_out("The number of packets cannot exceed %"PRIu32"\n",
                tra_param->group_pktnum * 256);
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (tra_param->lb_type == LB_TYPE_PHY) {
        cli_out("Tested port: %d ", tra_param->port_list[0]);
    } else {
        cli_out("Tested port: %d %d ", tra_param->port_list[0],
                                       tra_param->port_list[1]);
    }
    cli_out("Traffic port: %d %d ", tra_param->port_traffic[0],
                                    tra_param->port_traffic[1]);
    cli_out("Timestamp port: %d %d\n", tra_param->port_timestamp[0],
                                       tra_param->port_timestamp[1]);

    *bp = (void *)tra_param;

exit:
    if (SHR_FUNC_ERR()) {
        if (tra_param != NULL) {
            if (tra_param->thread_timesync != NULL) {
                shr_thread_stop(tra_param->thread_timesync, 0);
                tra_param->thread_timesync = NULL;
            }
            sal_free(tra_param);
        }
    }
    SHR_FUNC_EXIT();
}

static void
traffic_latency_help(int unit, void *bp)
{
    cli_out("%s", TRAFFIC_LATENCY_PARAM_HELP);
}

static int
traffic_latency_vlan_set_cb(int unit, void *bp, uint32_t option)
{
    traffic_latency_param_t *tra_param = (traffic_latency_param_t *)bp;
    int vid, pid;
    uint32_t i;
    bool need_tag_action;
    bcmdrd_pbmp_t pbmp, ubmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    BCMDRD_PBMP_CLEAR(pbmp);
    BCMDRD_PBMP_PORT_ADD(pbmp, TRAFFIC_LATENCY_CPU_PORT);
    for (i = 0; i < tra_param->total_port_count; i++) {
        BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->total_port_list[i]);
    }
    BCMDRD_PBMP_ASSIGN(ubmp, pbmp);

    /* create vlan for each testing port */
    vid = TRAFFIC_LATENCY_VLAN_ID_BASE;
    pid = TRAFFIC_LATENCY_VLAN_PROFILE_ID_BASE;
    for (i = 0; i < tra_param->total_port_count; i++) {
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
    vid = TRAFFIC_LATENCY_VLAN_ID_BASE;
    for (i = 0; i < tra_param->total_port_count; i++) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_port_vlan_set
                (unit, tra_param->total_port_list[i], vid));
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
traffic_latency_port_set_cb(int unit, void *bp, uint32_t option)
{
    traffic_latency_param_t *tra_param = (traffic_latency_param_t *)bp;
    bool ts_s_i = false, ts_s_e = false;
    bool ts_t_i = false, ts_t_e = false;
    bool ts_r_i = false, ts_r_e = false;
    bcmdrd_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    /*
     * After port_traffic[0] and port_traffic[1] generate traffic,
     *  the traffic would be redirected to ...
     *    -->  port_timestamp[0] (sender)
     *    -->  port_list[0][1]   (tested port)
     *    -->  port_timestamp[1] (receiver)
     *
     * According to the different latency type, it needs to add the timestamp
     * at ingress or egress side.
     */

    switch (tra_param->lat_type) {
        case LAT_TYPE_INGRESS_TO_INGRESS:
            /*
             *          |<-----latency-------->|
             * sender --i------> tested port --i------> receiver
             */
            ts_s_i = true;
            ts_t_i = true;
            break;
        case LAT_TYPE_EGRESS_TO_EGRESS:
            /*
             *              |<-----latency-------->|
             * sender ------e--> tested port ------e--> receiver
             */
            ts_t_e = true;
            ts_r_e = true;
            break;
        case LAT_TYPE_EGRESS_TO_INGRESS:
            /*
             *              |<-----latency---->|
             * sender ------e--> tested port --i------> receiver
             */
            ts_t_e = true;
            ts_t_i = true;
            break;
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_port_timestamp_enable
            (unit, tra_param->port_timestamp[0], ts_s_i, ts_s_e));
    if (tra_param->lb_type == LB_TYPE_PHY) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_port_timestamp_enable
                (unit, tra_param->port_list[0], ts_t_i, ts_t_e));
    } else {
        SHR_IF_ERR_EXIT
            (bcma_testutil_port_timestamp_enable
                (unit, tra_param->port_list[0], false, ts_t_e));
        SHR_IF_ERR_EXIT
            (bcma_testutil_port_timestamp_enable
                (unit, tra_param->port_list[1], ts_t_i, false));
    }
    SHR_IF_ERR_EXIT
        (bcma_testutil_port_timestamp_enable
            (unit, tra_param->port_timestamp[1], ts_r_i, ts_r_e));

    /* Tested port : setup loopback and wait link up */
    BCMDRD_PBMP_CLEAR(pbmp);
    BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->port_list[0]);
    if (tra_param->lb_type == LB_TYPE_EXT) {
        BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->port_list[1]);
    }
    SHR_IF_ERR_EXIT
        (bcma_testutil_port_setup(unit, tra_param->lb_type, pbmp));

    /* For the other port, all phy-loopback and wait link up */
    BCMDRD_PBMP_CLEAR(pbmp);
    BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->port_traffic[0]);
    BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->port_traffic[1]);
    BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->port_timestamp[0]);
    BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->port_timestamp[1]);
    SHR_IF_ERR_EXIT
        (bcma_testutil_port_setup(unit, LB_TYPE_PHY, pbmp));

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_latency_l2_forward(int unit, traffic_latency_param_t *tra_param,
                           traffic_latency_l2_action_t action,
                           unsigned int delay_sec)
{
    uint32_t g;
    shr_mac_t dst_mac;
    int v_traffic_0, v_traffic_1, v_sender, v_testport, v_receiver;
    int p_traffic_0, p_traffic_1, p_sender, p_testport, p_receiver;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    /* get vlan id for different port role */
    v_traffic_0 = TRAFFIC_LATENCY_VLAN_ID_BASE;
    v_traffic_1 = TRAFFIC_LATENCY_VLAN_ID_BASE + 1;
    v_sender = TRAFFIC_LATENCY_VLAN_ID_BASE + 2;
    if (tra_param->lb_type == LB_TYPE_PHY) {
        v_testport = TRAFFIC_LATENCY_VLAN_ID_BASE + 3;
        v_receiver = TRAFFIC_LATENCY_VLAN_ID_BASE + 4;
    } else {
        v_testport = TRAFFIC_LATENCY_VLAN_ID_BASE + 4;
        v_receiver = TRAFFIC_LATENCY_VLAN_ID_BASE + 5;
    }

    /* get port id for different port role */
    p_traffic_0 = tra_param->port_traffic[0];
    p_traffic_1 = tra_param->port_traffic[1];
    p_sender = tra_param->port_timestamp[0];
    p_testport = tra_param->port_list[0];
    p_receiver = tra_param->port_timestamp[1];

    /* setup 5 forwadring entries */
    if (action == L2_CONNECT || action ==  L2_CLEAN) {

        sal_memcpy(dst_mac, dst_mac_inc, sizeof(shr_mac_t));

        for (g = 0; g < tra_param->group_cnt; g++) {
            if (action == L2_CONNECT) {
                /* loop between p_traffic_0 and p_traffic_1 */
                SHR_IF_ERR_EXIT
                    (bcma_testutil_l2_unicast_add(unit,
                                                  p_traffic_1,
                                                  dst_mac, v_traffic_0, 0));
                SHR_IF_ERR_EXIT
                    (bcma_testutil_l2_unicast_add(unit,
                                                  p_traffic_0,
                                                  dst_mac, v_traffic_1, 0));

                /* p_sender --> p_testport --> p_receiver --> cpu port */
                SHR_IF_ERR_EXIT
                    (bcma_testutil_l2_unicast_add(unit,
                                                  p_testport,
                                                  dst_mac, v_sender, 0));
                SHR_IF_ERR_EXIT
                    (bcma_testutil_l2_unicast_add(unit,
                                                  p_receiver,
                                                  dst_mac, v_testport, 0));
                SHR_IF_ERR_EXIT
                    (bcma_testutil_l2_unicast_add(unit,
                                                  TRAFFIC_LATENCY_CPU_PORT,
                                                  dst_mac, v_receiver, 0));
            } else {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_l2_unicast_delete(unit,
                                                     p_traffic_1,
                                                     dst_mac, v_traffic_0, 0));
                SHR_IF_ERR_EXIT
                    (bcma_testutil_l2_unicast_delete(unit,
                                                     p_traffic_0,
                                                     dst_mac, v_traffic_1, 0));
                SHR_IF_ERR_EXIT
                    (bcma_testutil_l2_unicast_delete(unit,
                                                     p_testport,
                                                     dst_mac, v_sender, 0));
                SHR_IF_ERR_EXIT
                    (bcma_testutil_l2_unicast_delete(unit,
                                                     p_receiver,
                                                     dst_mac, v_testport, 0));
                SHR_IF_ERR_EXIT
                    (bcma_testutil_l2_unicast_delete(unit,
                                                     TRAFFIC_LATENCY_CPU_PORT,
                                                     dst_mac, v_receiver, 0));
            }
            /* increase dst mac for the next group */
            dst_mac[SHR_MAC_ADDR_LEN - 1]++;
            sal_sleep(delay_sec);
        }
    } else if (action == L2_REDIRECT) {

        /* redirect the packet from traffic loop to the sender */
        sal_memcpy(dst_mac, dst_mac_inc, sizeof(shr_mac_t));

        for (g = 0; g < tra_param->group_cnt; g++) {

            SHR_IF_ERR_EXIT
                (bcma_testutil_l2_unicast_update(unit, p_sender,
                                                 dst_mac, v_traffic_1, 0));

            /* increase dst mac for the next group */
            dst_mac[SHR_MAC_ADDR_LEN - 1]++;
            sal_sleep(delay_sec);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_latency_l2_forward_add_cb(int unit, void *bp, uint32_t option)
{
    return traffic_latency_l2_forward
               (unit, (traffic_latency_param_t *)bp, L2_CONNECT, 0);
}

static void
traffic_latency_timesync_thread(shr_thread_t tc, void *arg)
{
    traffic_latency_param_t *tra_param = (traffic_latency_param_t *)arg;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    /* use the general logical table to update TOD */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(tra_param->unit, "DEVICE_TS_TOD", &entry_hdl));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL), SHR_E_EXISTS);

    while (1) {
        uint64_t timestamp;
        uint64_t secs, nano_secs;

        SHR_IF_ERR_EXIT
            (bcmbd_ts_timestamp_nanosec_get(tra_param->unit, 1, &timestamp));

        secs = timestamp / SECOND_NSEC;
        nano_secs = timestamp % SECOND_NSEC;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_clear(entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "ADJUST", timestamp));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "TOD_SEC", secs));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "TOD_NSEC", nano_secs));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                                BCMLT_PRIORITY_NORMAL));


        shr_thread_sleep(tc, 50);
        if (shr_thread_stopping(tc)) {
            break;
        }
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
}

static int
traffic_latency_txrx_cb(int unit, void *bp, uint32_t option)
{
    traffic_latency_param_t *tra_param = (traffic_latency_param_t *)bp;
    bcmdrd_dev_type_t pkt_dev_type;
    uint32_t curtime;
    uint32_t inject_itr;
    bool test_result = false;
    bool first_polling = true;
    bcmpkt_packet_t *packet = NULL;
    bcmpkt_packet_t *packet_clone = NULL;
    uint64_t target_speed = 0;
    bool counter_checker_create = false, counter_checker_start = false;
    bool latency_checker_create = false;
    bool unmapped = false;
    int netif_id, pktdev_id;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    char show_msg[TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN];
    shr_mac_t dst_mac;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    /* setup packet device */
    SHR_IF_ERR_EXIT
        (bcma_testutil_packet_dev_init
            (unit, TRAFFIC_LATENCY_TX_CHAN, TRAFFIC_LATENCY_RX_CHAN,
             tra_param->length + 4 + (TESTUTIL_LATENCY_TIMESTAMP_SHIMLEN * 2),
             BCMPKT_BPOOL_BCOUNT_DEF,
             &netif_id, &pktdev_id));

    /* allocate packet buffer */
    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &pkt_dev_type));

    SHR_IF_ERR_EXIT
        (bcmpkt_alloc(pktdev_id, tra_param->length, BCMPKT_BUF_F_TX, &packet));

    SHR_NULL_CHECK(packet, SHR_E_INTERNAL);

    /* create counter and determine target speed (minimum port speed * 95%) */
    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_create(unit));

    counter_checker_create = true;

    if (tra_param->need_check_speed == TRUE) {
        uint64_t speed_0, speed_1;

        SHR_IF_ERR_EXIT
            (bcma_testutil_port_speed_get
                (unit, tra_param->port_traffic[0], &speed_0));
        SHR_IF_ERR_EXIT
            (bcma_testutil_port_speed_get
                (unit, tra_param->port_traffic[1], &speed_1));

        cli_out("Port %d speed : ", tra_param->port_traffic[0]);
        bcma_testutil_stat_show(speed_0, 1, true);
        cli_out("b/s\n");

        cli_out("Port %d speed : ", tra_param->port_traffic[1]);
        bcma_testutil_stat_show(speed_1, 1, true);
        cli_out("b/s\n");

        target_speed = (speed_0 < speed_1)? speed_0: speed_1;
        target_speed = target_speed *
                       (100 - TRAFFIC_LATENCY_DEFAULT_CHECK_SPEED_MARGIN) / 100;

        cli_out("  ==> Target speed : ");
        bcma_testutil_stat_show(target_speed, 1, true);
        cli_out("b/s\n\n");
    }

    sal_snprintf(show_msg, TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN,
                 "Port %3"PRId32" :", tra_param->port_traffic[0]);

    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_add
            (unit, tra_param->port_traffic[0], true, true,
             target_speed, show_msg));

    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_start(unit));
    counter_checker_start = true;

    /* clone packet and inject them into swirl loop */
    sal_memcpy(dst_mac, dst_mac_inc, sizeof(shr_mac_t));

    SHR_IF_ERR_EXIT
        (bcmpkt_packet_clone(pktdev_id, packet, &packet_clone));

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
                 src_mac_inc, dst_mac, false, 0,
                 TRAFFIC_LATENCY_DEFAULT_PACKET_ETHERTYPE,
                 TRAFFIC_LATENCY_DEFAULT_PACKET_PATTERN,
                 TRAFFIC_LATENCY_DEFAULT_PACKET_PATTERN_INC));

        SHR_IF_ERR_EXIT
            (bcmpkt_fwd_port_set(pkt_dev_type, tra_param->port_traffic[0],
                                 packet_clone));

        /* tx */
        SHR_IF_ERR_EXIT
            (bcmpkt_tx(unit, TRAFFIC_LATENCY_DEFAULT_NETIF_ID, packet_clone));
    }

    bcmpkt_free(pktdev_id, packet_clone);
    packet_clone = NULL;

    /* counter polling */
    curtime = 0;
    first_polling = true;
    while (curtime < TRAFFIC_LATENCY_DEFAULT_RUNNING_TIME) {
        sal_sleep(TRAFFIC_LATENCY_DEFAULT_POLLING_INTERVAL);

        curtime += TRAFFIC_LATENCY_DEFAULT_POLLING_INTERVAL;

        cli_out("TRAFFIC: running... (%"PRId32"%% complete)\n",
                (curtime * 100) / TRAFFIC_LATENCY_DEFAULT_RUNNING_TIME);

        if (tra_param->need_check_speed == TRUE && first_polling == false) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_traffic_counter_checker_execute(unit, true,
                                                               &test_result));
        } else {
            SHR_IF_ERR_EXIT
                (bcma_testutil_traffic_counter_checker_execute(unit, false,
                                                               &test_result));
        }

        if (test_result == false) {
            /* return immediately if user dont want to run to end */
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        first_polling = false;
    }

    if (test_result == false) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_is_unmapped(unit, DEVICE_TS_TODt, DEVICE_TS_TODt_HW_UPDATEf, &unmapped));
    if (unmapped) {
        /* run timesync thread */
        tra_param->thread_timesync = shr_thread_start("bcmalatencytimesyncthread",
                                                      SAL_THREAD_PRIO_DEFAULT,
                                                      traffic_latency_timesync_thread,
                                                      (void *)tra_param);
        SHR_NULL_CHECK(tra_param->thread_timesync, SHR_E_MEMORY);
        sal_sleep(1); /* wait a second to run the thread */
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(tra_param->unit, "DEVICE_TS_TOD", &entry_hdl));
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                BCMLT_PRIORITY_NORMAL), SHR_E_EXISTS);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_clear(entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "HW_UPDATE", 1));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                                BCMLT_PRIORITY_NORMAL));
    }

    /* prepare letency checker */
    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_latency_checker_create
            (unit, netif_id, tra_param->length, tra_param->inject_cnt));
    latency_checker_create = true;

    /*
     * redirect packets from swirl loop
     * into the timestamped port pair per 2 second
     */
    cli_out("redirecting all packets (%d groups) to check timestamp....\n",
            tra_param->group_cnt);

    SHR_IF_ERR_EXIT
        (traffic_latency_l2_forward(unit, tra_param, L2_REDIRECT,
                                    TRAFFIC_LATENCY_DEFAULT_GROUP_RXCHECK_WAIT));

    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_latency_checker_execute(unit, 0, &test_result));

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
    if (latency_checker_create == true) {
        bcma_testutil_traffic_latency_checker_destroy(unit);
    }
    (void)bcma_testutil_packet_dev_cleanup(unit);
    SHR_FUNC_EXIT();
}

static int
traffic_latency_cleanup_cb(int unit, void *bp, uint32_t option)
{
    traffic_latency_param_t *tra_param = (traffic_latency_param_t *)bp;
    bcmdrd_pbmp_t pbmp;
    uint32_t i;
    int vid = TRAFFIC_LATENCY_VLAN_ID_BASE;
    bool need_tag_action;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);


    /* Tested port : restore port setting */
    BCMDRD_PBMP_CLEAR(pbmp);
    BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->port_list[0]);
    if (tra_param->lb_type == LB_TYPE_EXT) {
        BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->port_list[1]);
    }
    SHR_IF_ERR_EXIT
        (bcma_testutil_port_clear(unit, tra_param->lb_type, pbmp, TRUE));

    /* For the other port, restore port setting */
    BCMDRD_PBMP_CLEAR(pbmp);
    BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->port_traffic[0]);
    BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->port_traffic[1]);
    BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->port_timestamp[0]);
    BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->port_timestamp[1]);
    SHR_IF_ERR_EXIT
        (bcma_testutil_port_clear(unit, LB_TYPE_PHY, pbmp, TRUE));

    /* disable timestamp */
    SHR_IF_ERR_EXIT
        (bcma_testutil_port_timestamp_disable(unit,
                                              tra_param->port_timestamp[0]));
    SHR_IF_ERR_EXIT
        (bcma_testutil_port_timestamp_disable(unit,
                                              tra_param->port_timestamp[1]));
    SHR_IF_ERR_EXIT
        (bcma_testutil_port_timestamp_disable(unit,
                                              tra_param->port_list[0]));
    if (tra_param->lb_type == LB_TYPE_EXT) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_port_timestamp_disable(unit,
                                                  tra_param->port_list[1]));
    }

    /* cleanup vlan setting */
    BCMDRD_PBMP_CLEAR(pbmp);
    BCMDRD_PBMP_PORT_ADD(pbmp, TRAFFIC_LATENCY_CPU_PORT);
    for (i = 0; i < tra_param->total_port_count; i++) {
        BCMDRD_PBMP_PORT_ADD(pbmp, tra_param->total_port_list[i]);
    }
    for (i = 0; i < tra_param->total_port_count; i++) {

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
        (traffic_latency_l2_forward(unit, (traffic_latency_param_t *)bp,
                                    L2_CLEAN, 0));

exit:
    SHR_FUNC_EXIT();
}

static bcma_test_proc_t traffic_latency_proc[] = {
    {
        .desc = "setup cpu port\n",
        .cb = bcma_testutil_port_cpu_setup_cb,
    },
    {
        .desc = "create VLAN and add ports into VLAN\n",
        .cb = traffic_latency_vlan_set_cb,
    },
    {
        .desc = "setup ports operating mode, wait link up\n",
        .cb = traffic_latency_port_set_cb,
    },
    {
        .desc = "add l2 forward entry\n",
        .cb = traffic_latency_l2_forward_add_cb,
    },
    {
        .desc = "TX/RX test\n",
        .cb = traffic_latency_txrx_cb,
    },
    {
        .desc = "cleanup port and vlan configuration\n",
        .cb = traffic_latency_cleanup_cb,
    },
    {
        .desc = "Test End Checkers\n",
        .cb = bcma_testutil_traffic_check_cb,
    }
};

static void
traffic_latency_recycle(int unit, void *bp)
{
    traffic_latency_param_t *tra_param = (traffic_latency_param_t *)bp;

    if (tra_param != NULL && tra_param->thread_timesync != NULL) {
        /* if testing is interrupted by ctrl-c, need to stop timesync thread */
        if (SHR_FAILURE(shr_thread_stop(tra_param->thread_timesync, 0))) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Timesync thread cannot exit.\n")));
        }
        tra_param->thread_timesync = NULL;
    }
    return;
}

static bcma_test_op_t traffic_latency_op = {
    .select = traffic_latency_select,
    .parser = traffic_latency_parser,
    .help = traffic_latency_help,
    .recycle = traffic_latency_recycle,
    .procs = traffic_latency_proc,
    .proc_count = COUNTOF(traffic_latency_proc)
};

bcma_test_op_t *
bcma_testcase_traffic_latency_op_get(void)
{
    return &traffic_latency_op;
}
