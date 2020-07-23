/*! \file bcma_testcase_traffic_stream.c
 *
 *  Test case for streaming traffic.
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
#include <sal/sal_sem.h>
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
#include <bcma/test/testcase/bcma_testcase_traffic_stream.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define TRAFFIC_STREAM_PARAM_HELP \
    "  PortList         - List of all ports to be tested\n"\
    "  PortListOversub  - List of oversubscribed ports\n"\
    "  LengthMode       - Packet length mode (WorstCase / RANDom / USERdefined)\n"\
    "  Length           - Packet length (only for user-defined mode)\n"\
    "  Count            - Packet count  (only for user-defined and worst-case mode)\n"\
    "  MaxNumCells      - Limit of cell number per packet\n"\
    "  CounterPollInterval - Time interval between each counter polling (in sec)\n"\
    "  TimeInSeconds       - Total testing time (in sec)\n"\
    "  Run2End          - Keep testing until the end\n"\
    "  CheckData        - Redirect packet back to CPU and check data content\n"\
    "  CheckLineRate    - Check if raffic speed meets line rate\n\n"\
    "\nExample:\n"\
    "    Run streaming testing with default port list\n"\
    "      tr 501\n"\
    "    Run streaming testing with smallest packet length (i.e. in worst case, it is default mode)\n"\
    "      tr 501 pl=1-4,7,10\n"\
    "    Run streaming testing with some ports operated in oversubscribed mode\n"\
    "      tr 501 pl=1-4,7,10 plo=4,7\n"\
    "    Run streaming testing with packet length is ramdom\n"\
    "      tr 501 pl=1-4,7,10 LengthMode=random\n"\
    "    Run streaming testing with packet length is ramdom (but cells limit is up to 20)\n"\
    "      tr 501 pl=1-4,7,10 LengthMode=random MaxNumCells=20\n"\
    "    Run streaming testing with packet length is 1200 (packet count is determined by switch capability)\n"\
    "      tr 501 pl=1-4,7,10 LengthMode=userdefined Length=1200\n"\
    "    Run streaming testing with packet length is 1200 and count is 10\n"\
    "      tr 501 pl=1-4,7,10 LengthMode=userdefined Length=1200 Count=10\n"\
    "    Run sreaming testing for 10 sec, polling interval is 2 sec (it would check counter 5 times)\n"\
    "      tr 501 pl=1-4,7,10 tis=10 cpi=2\n"\
    "    Run sreaming testing until the end even if there is some port traffic stopped\n"\
    "      tr 501 pl=1-4,7,10 r2e=true\n"\
    "    Run sreaming testing without checking rx packet and line rate\n"\
    "      tr 501 pl=1-4,7,10 cd=false clr=false\n"

/* packet length mode */
typedef enum traffic_stream_pktlen_mode_e {
    STREAM_PKTLEN_WORSTCASE = 0,
    STREAM_PKTLEN_RANDOM,
    STREAM_PKTLEN_USERDEFINED
} traffic_stream_pktlen_mode_t;

static bcma_cli_parse_enum_t traffic_stream_pktlen_mode_str[] = {
    {"WorstCase", STREAM_PKTLEN_WORSTCASE},
    {"RANDom", STREAM_PKTLEN_RANDOM},
    {"USERdefined", STREAM_PKTLEN_USERDEFINED},
    {NULL,   0}
};

/* default value */
static const shr_mac_t default_src_mac = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
static const shr_mac_t default_dst_mac = {0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb};

#define TRAFFIC_STREAM_CPU_PORT                   (0)
#define TRAFFIC_STREAM_VLAN_ID_BASE               (100)
#define TRAFFIC_STREAM_VLAN_PROFILE_ID_BASE       (1)
#define TRAFFIC_STREAM_DEFAULT_NETIF_ID           (1)
#define TRAFFIC_STREAM_DEFAULT_RUNNING_TIME       (10)
#define TRAFFIC_STREAM_DEFAULT_POLLING_INTERVAL   (2)
#define TRAFFIC_STREAM_DEFAULT_CELL_MAXLIMIT      (4)
#define TRAFFIC_STREAM_DEFAULT_PKTLEN_MODE        (STREAM_PKTLEN_WORSTCASE)
#define TRAFFIC_STREAM_DEFAULT_PACKET_ETHERTYPE   (0xffff)
#define TRAFFIC_STREAM_DEFAULT_PACKET_PATTERN     (0xa5a4a3a2)
#define TRAFFIC_STREAM_DEFAULT_PACKET_PATTERN_INC (0)
#define TRAFFIC_STREAM_DEFAULT_RUN_UNTIL_END      (FALSE)
#define TRAFFIC_STREAM_DEFAULT_CHECK_RX           (TRUE)
#define TRAFFIC_STREAM_DEFAULT_CHECK_SPEED        (TRUE)
#define TRAFFIC_STREAM_DEFAULT_CHECK_SPEED_MARGIN (5) /* pass tolerance is 5% */
#define TRAFFIC_STREAM_DEFAULT_RX_CHECK_WAIT      (10*1000000) /* 10 sec */

#define TRAFFIC_STREAM_PKT_CNT_MAX                (400)
#define TRAFFIC_STREAM_PKT_SIZE_MAX               (1536)
#define TRAFFIC_STREAM_PKTDEV_CHAN_TX             (0)
#define TRAFFIC_STREAM_PKTDEV_CHAN_RX             (1)

/* Parameter */
typedef struct traffic_stream_param_s {
    /* All testing ports */
    bcmdrd_pbmp_t pbmp_all;

    /* Oversubscribed ports */
    bcmdrd_pbmp_t pbmp_oversub;

    /* Packet length mode */
    traffic_stream_pktlen_mode_t pkt_len_mode;

    /* Packet length array for each testing port */
    uint32_t pkt_len[BCMDRD_CONFIG_MAX_PORTS][TRAFFIC_STREAM_PKT_CNT_MAX];

    /* Packet count array for each testing port  */
    uint32_t pkt_cnt[BCMDRD_CONFIG_MAX_PORTS];

    /* Upper limit of cell numbers per packet */
    uint32_t pkt_cell_limit;

    /* Running time (in sec) */
    uint32_t runtime;

    /* Polling interval (in sec) */
    uint32_t interval;

    /* Run until end even if error happen */
    int run_2_end;

    /* Whether to check packet back to CPU after unlock loop */
    int need_check_rx;

    /* Whether to check speed meet line rate */
    int need_check_speed;
} traffic_stream_param_t;

static int
traffic_stream_select(int unit)
{
    bool no_tdm_coding = false;

    bcma_testutil_drv_feature_get
        (unit, BCMA_TESTUTIL_FT_NEED_NOT_TDM_CODING_ALGO, &no_tdm_coding);

    if (no_tdm_coding == true) {
        return false;
    } else {
        return true;
    }
}

static int
traffic_stream_parse_param(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                           traffic_stream_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    bool pt_is_init = false;
    uint32_t parse_pktlen = 0, parse_pktcnt = 0;
    char *parse_pbmp_all = NULL, *parse_pbmp_oversub = NULL;
    bcmdrd_pbmp_t pbmp_temp;
    int port;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    /* setup parse table */
    bcma_cli_parse_table_init(cli, &pt);
    pt_is_init = true;

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PortList", "str",
                                  &parse_pbmp_all, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PortListOversub", "str",
                                  &parse_pbmp_oversub, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "LengthMode", "enum",
                                  &(param->pkt_len_mode),
                                  &traffic_stream_pktlen_mode_str));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Length", "int",
                                  &parse_pktlen, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Count", "int",
                                  &parse_pktcnt, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "MaxNumCells", "int",
                                  &param->pkt_cell_limit, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "CounterPollInterval", "int",
                                  &param->interval, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "TimeInSeconds", "int",
                                  &param->runtime, NULL));

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

    /* check pbmp_oversub is the subset of pbmp_all */
    if (parse_pbmp_all != NULL) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_parse_port_bmp(parse_pbmp_all,
                                          &(param->pbmp_all)));
    } else {
        SHR_IF_ERR_EXIT
            (bcma_testutil_default_port_bmp_get(unit, BCMDRD_PORT_TYPE_FPAN,
                                                &(param->pbmp_all)));
    }

    if (parse_pbmp_oversub != NULL) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_parse_port_bmp(parse_pbmp_oversub,
                                          &(param->pbmp_oversub)));
    }
    BCMDRD_PBMP_ASSIGN(pbmp_temp, param->pbmp_all);
    BCMDRD_PBMP_OR(pbmp_temp, param->pbmp_oversub);
    BCMDRD_PBMP_XOR(pbmp_temp, param->pbmp_all);
    if (BCMDRD_PBMP_NOT_NULL(pbmp_temp)) {
        cli_out("PortListOversub need to be the subset of PortList\n");
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* calculate packet length/count per testing port */
    if (param->pkt_len_mode == STREAM_PKTLEN_RANDOM) {

        if (parse_pktcnt != 0 || parse_pktlen != 0) {
            cli_out("Cannot assign Length or Count "
                    "when length mode is random\n");
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        BCMDRD_PBMP_ITER(param->pbmp_all, port) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_drv_traffic_random_packet_list_get
                    (unit, param->pkt_cell_limit, TRAFFIC_STREAM_PKT_CNT_MAX,
                     param->pkt_len[port], &(param->pkt_cnt[port])));
        }
    } else {
        BCMDRD_PBMP_ITER(param->pbmp_all, port) {

            uint32_t pkt_len, pkt_cnt, i;

            /* get packet length */
            if (param->pkt_len_mode == STREAM_PKTLEN_WORSTCASE) {
                if (parse_pktlen != 0) {
                    cli_out("Cannot assign Length "
                            "when length mode is worst-case\n");
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                SHR_IF_ERR_EXIT
                    (bcma_testutil_drv_traffic_worstcase_pktlen_get
                        (unit, port, &pkt_len));
            } else if (param->pkt_len_mode == STREAM_PKTLEN_USERDEFINED) {
                if (parse_pktlen == 0) {
                    cli_out("Need to assign Length "
                            "when length mode is user-defined\n");
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                pkt_len = parse_pktlen;
            } else {
                SHR_ERR_EXIT(SHR_E_INTERNAL); /* should not happen */
            }

            /* get packet count */
            if (parse_pktcnt != 0) {
                pkt_cnt = parse_pktcnt;
            } else {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_drv_traffic_fixed_packet_list_get
                        (unit, pkt_len, &pkt_cnt));

            }

            /* fill it into param */
            param->pkt_cnt[port] = pkt_cnt;
            for (i = 0; i < pkt_cnt; i++) {
                param->pkt_len[port][i] = pkt_len;
            }
        }
    }

exit:
    if (pt_is_init == true) {
        bcma_cli_parse_table_done(&pt);
    }
    SHR_FUNC_EXIT();
}

static int
traffic_stream_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                      uint32_t flags, void **bp)
{
    traffic_stream_param_t *tra_param = NULL;
    bool show = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_ALLOC(tra_param, sizeof(traffic_stream_param_t),
              "bcmaTestCaseTrafficPairPrm");
    SHR_NULL_CHECK(tra_param, SHR_E_MEMORY);

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    /* init */
    BCMDRD_PBMP_CLEAR(tra_param->pbmp_all);
    BCMDRD_PBMP_CLEAR(tra_param->pbmp_oversub);
    tra_param->pkt_len_mode = TRAFFIC_STREAM_DEFAULT_PKTLEN_MODE;
    tra_param->runtime = TRAFFIC_STREAM_DEFAULT_RUNNING_TIME;
    tra_param->interval = TRAFFIC_STREAM_DEFAULT_POLLING_INTERVAL;
    tra_param->pkt_cell_limit = TRAFFIC_STREAM_DEFAULT_CELL_MAXLIMIT;
    tra_param->run_2_end = TRAFFIC_STREAM_DEFAULT_RUN_UNTIL_END;
    tra_param->need_check_rx = TRAFFIC_STREAM_DEFAULT_CHECK_RX;
    tra_param->need_check_speed = TRAFFIC_STREAM_DEFAULT_CHECK_SPEED;
    sal_memset(tra_param->pkt_len, 0, sizeof(tra_param->pkt_len));
    sal_memset(tra_param->pkt_cnt, 0, sizeof(tra_param->pkt_cnt));

    /* parse */
    SHR_IF_ERR_EXIT
        (traffic_stream_parse_param(unit, cli, a, tra_param, show));
    if (show == true) {
        SHR_FREE(tra_param);
        SHR_EXIT();
    }

    /* check */
    if (tra_param->runtime == 0 || tra_param->interval == 0 ||
        BCMDRD_PBMP_IS_NULL(tra_param->pbmp_all)) {
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
traffic_stream_help(int unit, void *bp)
{
    cli_out("%s", TRAFFIC_STREAM_PARAM_HELP);
}

static int
traffic_stream_vlan_set_cb(int unit, void *bp, uint32_t option)
{
    traffic_stream_param_t *tra_param = (traffic_stream_param_t *)bp;
    int vid, pid, port;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    /* create vlan for each testing port */
    vid = TRAFFIC_STREAM_VLAN_ID_BASE;
    pid = TRAFFIC_STREAM_VLAN_PROFILE_ID_BASE;
    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {

        bcmdrd_pbmp_t pbmp, ubmp;

        BCMDRD_PBMP_CLEAR(ubmp);
        BCMDRD_PBMP_CLEAR(pbmp);
        BCMDRD_PBMP_PORT_ADD(pbmp, port);

        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_create(unit, vid, pid));
        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_port_add(unit, vid, pbmp, ubmp, false));
        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_stp_set(unit, vid, pbmp,
                                        BCMA_TESTUTIL_STP_FORWARD));

        vid++;
        pid++;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_stream_port_set_cb(int unit, void *bp, uint32_t option)
{
    traffic_stream_param_t *tra_param = (traffic_stream_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_testutil_port_setup(unit, LB_TYPE_MAC, tra_param->pbmp_all));
exit:
    SHR_FUNC_EXIT();
}

static int
traffic_stream_txrx_cb(int unit, void *bp, uint32_t option)
{
    traffic_stream_param_t *tra_param = (traffic_stream_param_t *)bp;
    bcmdrd_dev_type_t pkt_dev_type;
    uint64_t *expeceted_rate = NULL;
    bool counter_checker_create = false, counter_checker_start = false;
    bool rxdata_checker_create = false, rxdata_checker_start = false;
    bool test_result = false, first_polling = true;
    int vid, netif_id, pktdev_id, port, pkt_cnt_all;
    uint32_t curtime, idx;
    bcmpkt_packet_t *packet = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    /* setup packet device */
    pkt_cnt_all = 0;
    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
        pkt_cnt_all += tra_param->pkt_cnt[port];
    }
    SHR_IF_ERR_EXIT
        (bcma_testutil_packet_dev_init
            (unit, TRAFFIC_STREAM_PKTDEV_CHAN_TX, TRAFFIC_STREAM_PKTDEV_CHAN_RX,
             TRAFFIC_STREAM_PKT_SIZE_MAX,
             (tra_param->need_check_rx == TRUE) ? pkt_cnt_all * 2 : pkt_cnt_all,
             &netif_id, &pktdev_id));

    /* determine target speed : minimun port speed * 95% */
    SHR_ALLOC(expeceted_rate, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint64_t),
              "bcmaTestCaseTrafficStreamExpRate");
    SHR_NULL_CHECK(expeceted_rate, SHR_E_MEMORY);
    sal_memset(expeceted_rate, 0, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint64_t));

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_traffic_expeceted_rate_get
            (unit, tra_param->pbmp_all, tra_param->pbmp_oversub,
             0, BCMDRD_CONFIG_MAX_PORTS, expeceted_rate));

    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
        expeceted_rate[port] =
            (expeceted_rate[port] *
             (100 - TRAFFIC_STREAM_DEFAULT_CHECK_SPEED_MARGIN)) / 100;
    }

    /* print information */
    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
        cli_out("port %d traget speed : ", port);
        bcma_testutil_stat_show(expeceted_rate[port], 1, true);
        cli_out("b/s ,");
        cli_out("inject %d packtes.\n  (size :", tra_param->pkt_cnt[port]);
        for (idx = 0; idx < tra_param->pkt_cnt[port]; idx++) {
            cli_out(" %d", tra_param->pkt_len[port][idx]);
        }
        cli_out(")\n");
    }

    /* setup counter checker */
    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_create(unit));

    counter_checker_create = true;

    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {

        char show_msg[TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN];

        sal_snprintf(show_msg, TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN,
                     "Port %3"PRId32" :", port);

        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_counter_checker_add
                (unit, port, true, true, expeceted_rate[port], show_msg));
    }

    /* inject packet, and setup rxdataa checker if need */
    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &pkt_dev_type));

    if (tra_param->need_check_rx == TRUE) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_rxdata_checker_create
                (unit, netif_id, pktdev_id, TRAFFIC_RXDATA_CHECKER_MODE_PORT));
        rxdata_checker_create = true;
    }

    vid = TRAFFIC_STREAM_VLAN_ID_BASE;

    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {

        for (idx = 0; idx < tra_param->pkt_cnt[port]; idx++) {

            SHR_IF_ERR_EXIT
                (bcmpkt_alloc(pktdev_id, tra_param->pkt_len[port][idx],
                              BCMPKT_BUF_F_TX, &packet));

            SHR_NULL_CHECK(packet, SHR_E_INTERNAL);

            SHR_IF_ERR_EXIT
                (bcma_testutil_packet_fill
                    (packet->data_buf, tra_param->pkt_len[port][idx],
                     default_src_mac, default_dst_mac, true, vid,
                     TRAFFIC_STREAM_DEFAULT_PACKET_ETHERTYPE,
                     TRAFFIC_STREAM_DEFAULT_PACKET_PATTERN,
                     TRAFFIC_STREAM_DEFAULT_PACKET_PATTERN_INC));

            if (tra_param->need_check_rx == TRUE) {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_traffic_rxdata_checker_add
                        (unit, port, packet->data_buf));
            }

            SHR_IF_ERR_EXIT
                (bcmpkt_fwd_port_set(pkt_dev_type, port, packet));

            SHR_IF_ERR_EXIT
                (bcmpkt_tx(unit, TRAFFIC_STREAM_DEFAULT_NETIF_ID, packet));

            if (packet != NULL) {
                bcmpkt_free(pktdev_id, packet);
                packet = NULL;
            }
        }
        vid++;
    }

    /* counter polling */

    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_start(unit));
    counter_checker_start = true;

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
                (bcma_testutil_traffic_counter_checker_execute
                    (unit, true, &test_result));
        } else {
            SHR_IF_ERR_EXIT
                (bcma_testutil_traffic_counter_checker_execute
                    (unit, false, &test_result));
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

    /* redirect all packet back to CPU and check */
    if (tra_param->need_check_rx == TRUE) {
        bcmdrd_pbmp_t pbmp, ubmp;

        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_rxdata_checker_start(unit));

        rxdata_checker_start = true;

        /* udpate all vlan member to CPU */
        BCMDRD_PBMP_CLEAR(ubmp);
        BCMDRD_PBMP_CLEAR(pbmp);
        BCMDRD_PBMP_PORT_ADD(pbmp, TRAFFIC_STREAM_CPU_PORT);
        vid = TRAFFIC_STREAM_VLAN_ID_BASE;
        BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_vlan_port_update(unit, vid, pbmp, ubmp));
            vid++;
        }

        /* check */
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_rxdata_checker_execute
                (unit, TRAFFIC_STREAM_DEFAULT_RX_CHECK_WAIT, &test_result));

        if (test_result == false) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    if (packet != NULL) {
        bcmpkt_free(pktdev_id, packet);
        packet = NULL;
    }
    if (expeceted_rate != NULL) {
        SHR_FREE(expeceted_rate);
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
traffic_stream_cleanup_cb(int unit, void *bp, uint32_t option)
{
    traffic_stream_param_t *tra_param = (traffic_stream_param_t *)bp;
    int vid, port;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    /* restore port setting */
    SHR_IF_ERR_EXIT
        (bcma_testutil_port_clear(unit, LB_TYPE_MAC, tra_param->pbmp_all, TRUE));

    /* cleanup vlan setting */
    vid = TRAFFIC_STREAM_VLAN_ID_BASE;
    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
        bcmdrd_pbmp_t pbmp;
        BCMDRD_PBMP_CLEAR(pbmp);
        BCMDRD_PBMP_PORT_ADD(pbmp, port);

        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_port_remove(unit, vid, pbmp));

        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_destroy(unit, vid));

        vid++;
    }

exit:
    SHR_FUNC_EXIT();
}

static bcma_test_proc_t traffic_stream_proc[] = {
    {
        .desc = "create VLAN and add ports into VLAN\n",
        .cb = traffic_stream_vlan_set_cb,
    },
    {
        .desc = "setup ports operating mode, wait link up\n",
        .cb = traffic_stream_port_set_cb,
    },
    {
        .desc = "TX/RX test\n",
        .cb = traffic_stream_txrx_cb,
    },
    {
        .desc = "cleanup port and vlan configuration\n",
        .cb = traffic_stream_cleanup_cb,
    }
};

static void
traffic_stream_recycle(int unit, void *bp)
{
    return;
}

static bcma_test_op_t traffic_stream_op = {
    .select = traffic_stream_select,
    .parser = traffic_stream_parser,
    .help = traffic_stream_help,
    .recycle = traffic_stream_recycle,
    .procs = traffic_stream_proc,
    .proc_count = COUNTOF(traffic_stream_proc)
};

bcma_test_op_t *
bcma_testcase_traffic_stream_op_get(void)
{
    return &traffic_stream_op;
}
