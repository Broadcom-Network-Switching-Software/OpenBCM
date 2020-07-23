/*! \file bcma_testcase_traffic_max_power.c
 *
 *  Test case for max power.
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
#include <bcma/test/util/bcma_testutil_power.h>
#include <bcma/test/util/bcma_testutil_port.h>
#include <bcma/test/util/bcma_testutil_stat.h>
#include <bcma/test/util/bcma_testutil_packet.h>
#include <bcma/test/testcase/bcma_testcase_traffic_max_power.h>
#include <bcma/test/util/bcma_testutil_traffic.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define TRAFFIC_MAX_POWER_PARAM_HELP \
    "  PortList         - List of all ports to be tested\n"\
    "  MirrorPortList   - List of mirror ports to be tested\n"\
    "  PortListOversub  - List of oversubscribed ports\n"\
    "  EnableDefip      - Enable l3 defip hit\n"\
    "  EnableIfp        - Enable ifp hit (if ifp is disabled, ingress flex counter is also disabled)\n"\
    "  EnableEfp        - Enable efp hit (if efp is disabled, egress flex counter is also disabled)\n"\
    "  EnableFlexCtr    - Enable both ingress and egress flex counter\n"\
    "  EnableRndHdr     - Enable pkt header randomization \n"\
    "  PortListOversub  - List of oversubscribed ports\n"\
    "  LengthMode       - Packet length mode (WorstCase / RANDom / USERdefined)\n"\
    "  LengthPattern    - Pkt length repeating pattern; valid only when LengthMode=USERdefined. Ex.: LengthPattern=201,202,180\n\n"\
    "  Length           - Packet length (only for user-defined mode)\n"\
    "  Count            - Packet count  (only for user-defined and worst-case mode)\n"\
    "  MaxNumCells      - Limit of cell number per packet\n"\
    "  CounterPollInterval - Time interval between each counter polling (in sec)\n"\
    "  TimeInSeconds    - Total testing time (in sec)\n"\
    "  Run2End          - Keep testing until the end\n"\
    "  CheckData        - Redirect packet back to CPU and check data content\n"\
    "  CheckLineRate    - Check if traffic speed meets line rate\n"\
    "  TestPhase        - 1: init traffic only 2: check bandwidth and check packet if CheckData is true\n"\
    "  LoopbackMode     - Loopback Mode 1.MAC 2.PHY 3.EXT; default=MAC\n"\
    "  LpmLevel         - Alpm level. DevSpecific specifications\n"\
    "  Topology         - Port traffic topology. DevSpecific specifications\n"\
    "  PktFlow          - Pkt Flow type: L2, L3, MPLS, VxLAN. DevSpecific specifications\n"\
    "\nExample:\n"\
    "    Run streaming testing with smallest packet length (i.e. in worst case, it is default mode)\n"\
    "      tr 517 pl=1-4\n"\
    "    Run streaming testing with some ports operated as mirror port\n"\
    "      tr 517 pl=1-3 plm=4\n"\
    "    Run streaming testing with packet length is random\n"\
    "      tr 517 pl=1-4 LengthMode=random\n"\
    "    Run streaming testing with packet length is random (but cells limit is up to 20)\n"\
    "      tr 517 pl=1-4 LengthMode=random MaxNumCells=20\n"\
    "    Run streaming testing with packet length is 1200 (packet count is determined by switch capability)\n"\
    "      tr 517 pl=1-4 LengthMode=userdefined Length=1200\n"\
    "    Run streaming testing with packet length is 1200 and count is 10\n"\
    "      tr 517 pl=1-4 LengthMode=userdefined Length=1200 Count=10\n"\
    "    Run sreaming testing for 10 sec, polling interval is 2 sec (it would check counter 5 times)\n"\
    "      tr 517 pl=1-4 tis=10 cpi=2\n"\
    "    Run sreaming testing until the end even if there is some port traffic stopped\n"\
    "      tr 517 pl=1-4 r2e=true\n"\
    "    Run sreaming testing without checking rx packet and line rate\n"\
    "      tr 517 pl=1-4 cd=false clr=false\n"\
    "    Run the test in PHY mode with length pattern 192,195,193 ...\n"\
    "      tr 517 PortList=1-271 Count=1 LengthMode=USERpattern LengthPattern=192,195,193 LoopbackMode=PHY\n"




static bcma_cli_parse_enum_t traffic_power_pktlen_mode_str[] = {
    {"WorstCase", POWER_PKTLEN_WORSTCASE},
    {"RANDom", POWER_PKTLEN_RANDOM},
    {"USERdefined", POWER_PKTLEN_USERDEFINED},
    {"USERpattern", POWER_PKTLEN_PATTERN},
    {NULL,   0}
};

static int seed=0;

#define TRAFFIC_MAX_POWER_DEFAULT_RUNNING_TIME       (10)
#define TRAFFIC_MAX_POWER_DEFAULT_POLLING_INTERVAL   (5)
#define TRAFFIC_MAX_POWER_DEFAULT_CELL_MAXLIMIT      (4)
#define TRAFFIC_MAX_POWER_DEFAULT_PKTLEN_MODE        (POWER_PKTLEN_WORSTCASE)
#define TRAFFIC_MAX_POWER_DEFAULT_RUN_UNTIL_END      (FALSE)
#define TRAFFIC_MAX_POWER_DEFAULT_CHECK_RX           (TRUE)
#define TRAFFIC_MAX_POWER_DEFAULT_CHECK_SPEED        (TRUE)
#define TRAFFIC_MAX_POWER_DEFAULT_CONFIG_N_SEND      (TRUE)
#define TRAFFIC_MAX_POWER_DEFAULT_CHECK_N_STOP       (TRUE)
#define TRAFFIC_MAX_POWER_DEFAULT_ENABLE_DEFIP       (TRUE)
#define TRAFFIC_MAX_POWER_DEFAULT_ENABLE_DLB         (TRUE)
#define TRAFFIC_MAX_POWER_DEFAULT_ENABLE_MIRROR      (TRUE)
#define TRAFFIC_MAX_POWER_DEFAULT_ENABLE_IFP         (TRUE)
#define TRAFFIC_MAX_POWER_DEFAULT_ENABLE_RDB         (TRUE)
#define TRAFFIC_MAX_POWER_DEFAULT_ENABLE_ALPM        (FALSE)
#define TRAFFIC_MAX_POWER_DEFAULT_ENABLE_EFP         (TRUE)
#define TRAFFIC_MAX_POWER_DEFAULT_ENABLE_FLEX_CTR    (TRUE)
#define TRAFFIC_MAX_POWER_DEFAULT_ENABLE_RND_HDR     (1)



static bcma_cli_parse_enum_t testcase_power_lb_str[] = {
    {"MAC", LB_TYPE_MAC},
    {"PHY", LB_TYPE_PHY},
    {"EXT", LB_TYPE_EXT},
    {NULL,   0}
};

static int
get_int_pattern(const char *str, int *pattern_size, int *length_pattern);


static int
traffic_max_power_select(int unit)
{
    return true;
}


static int
traffic_max_power_parse_param(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                              traffic_max_power_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    bool pt_is_init = false;
    uint32_t parse_pktlen = 0, parse_pktcnt = 0;
    char *parse_pbmp_all = NULL, *parse_pbmp_oversub = NULL, *parse_mirror_pbmp = NULL;
    bcmdrd_pbmp_t pbmp_temp;
    int port, test_phase = 0;
    int pattern_size, length_pattern[10];
    uint32_t pkt_len, pkt_cnt, i;
    char *parse_length_pattern = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    /* setup parse table */
    bcma_cli_parse_table_init(cli, &pt);
    pt_is_init = true;

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PortList", "str",
                                  &parse_pbmp_all, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "MirrorPortList", "str",
                                  &parse_mirror_pbmp, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PortListOversub", "str",
                                  &parse_pbmp_oversub, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "LengthMode", "enum",
                                  &(param->pkt_len_mode),
                                  &traffic_power_pktlen_mode_str));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Length", "int",
                                  &parse_pktlen, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Count", "int",
                                  &parse_pktcnt, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "LoopbackMode", "enum", &(param->loopback_mode),
                                  &testcase_power_lb_str));

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
        (bcma_cli_parse_table_add(&pt, "EnableDefip", "bool",
                                  &param->enable_defip, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "EnableDlb", "bool",
                                  &param->enable_dlb, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "EnableMirror", "bool",
                                  &param->enable_mirror, NULL));
   SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "EnableRdb", "bool",
                                  &param->enable_rdb, NULL));
   SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "EnableAlpm", "bool",
                                  &param->enable_alpm, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "EnableIfp", "bool",
                                  &param->enable_ifp, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "EnableEfp", "bool",
                                  &param->enable_efp, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "EnableFlexCtr", "bool",
                                  &param->enable_flex_ctr, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "EnableRndHdr", "int",
                                  &param->enable_rnd_hdr, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "EnableBmp", "hex",
                                  &param->enable_bmp, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "CheckData", "bool",
                                  &param->need_check_rx, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "CheckLineRate", "bool",
                                  &param->need_check_speed, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "TestPhase", "int",
                                  &test_phase, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "LengthPattern", "str",
                                  &parse_length_pattern, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "LpmLevel", "int",
                                  &param->lpm_level, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Topology", "int",
                                  &param->topology, NULL));

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PktFlow", "int",
                                  &param->pkt_flow, NULL));

    /* parse start */
    if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    if (BCMA_CLI_ARG_CNT(a) != 0) {
        cli_out("%s: extra options starting with \"%s\"\n",
                BCMA_CLI_ARG_CMD(a), BCMA_CLI_ARG_CUR(a));
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
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
                (bcma_testutil_default_port_bmp_get
                    (unit, BCMDRD_PORT_TYPE_FPAN, &(param->pbmp_all)));
    }

    if (parse_pbmp_oversub != NULL) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_parse_port_bmp(parse_pbmp_oversub,
                                          &(param->pbmp_oversub)));
    }

    if (parse_mirror_pbmp != NULL) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_parse_port_bmp(parse_mirror_pbmp,
                                          &(param->mirror_pbmp)));
    } else {
        BCMDRD_PBMP_CLEAR(param->mirror_pbmp);
    }

    BCMDRD_PBMP_ASSIGN(pbmp_temp, param->pbmp_all);
    BCMDRD_PBMP_OR(pbmp_temp, param->pbmp_oversub);
    BCMDRD_PBMP_XOR(pbmp_temp, param->pbmp_all);
    if (BCMDRD_PBMP_NOT_NULL(pbmp_temp)) {
        cli_out("PortListOversub need to be the subset of PortList\n");
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
/*parse_pktcnt = 10;*/

    /* calculate packet length/count per testing port */
    if (param->pkt_len_mode == POWER_PKTLEN_RANDOM) {
        if (parse_pktcnt != 0 || parse_pktlen != 0) {
            cli_out("Cannot assign Length or Count "
                    "when length mode is random\n");
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        BCMDRD_PBMP_ITER(param->pbmp_all, port) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_drv_traffic_random_packet_list_get
                    (unit, param->pkt_cell_limit, TRAFFIC_MAX_POWER_PKT_CNT_MAX,
                     param->pkt_len[port], &(param->pkt_cnt[port])));
        }
    } else if (param->pkt_len_mode == POWER_PKTLEN_PATTERN) {
        /* If LengthPattern defined then overwrite pkt sizes */
        if (parse_length_pattern != NULL) {
            if (get_int_pattern(parse_length_pattern,
                               &pattern_size, length_pattern) == -1) {
                cli_out("get_int_pattern() returned with error\n");
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
            }
        } else {
            cli_out("LengthMode=USERpattern should specify"
                    "LengthPattern=? param also\n");
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        if (parse_pktcnt != 0) {
            pkt_cnt = parse_pktcnt;
        } else {
            SHR_IF_ERR_EXIT
                (bcma_testutil_drv_traffic_fixed_packet_list_get
                    (unit, length_pattern[0], &pkt_cnt));
        }

        BCMDRD_PBMP_ITER(param->pbmp_all, port) {
            param->pkt_cnt[port] = pkt_cnt;
            for (i = 0; i < pkt_cnt; i++) {
                param->pkt_len[port][i] = length_pattern[i % pattern_size];
            }
        }
    } else {
        BCMDRD_PBMP_ITER(param->pbmp_all, port) {
            /* get packet length */
            if (param->pkt_len_mode == POWER_PKTLEN_WORSTCASE) {
                if (parse_pktlen != 0) {
                    cli_out("Cannot assign Length "
                            "when length mode is worst-case\n");
                    SHR_IF_ERR_EXIT(SHR_E_PARAM);
                }
                SHR_IF_ERR_EXIT
                    (bcma_testutil_drv_traffic_worstcase_pktlen_get
                        (unit, port, &pkt_len));
            } else if (param->pkt_len_mode == POWER_PKTLEN_USERDEFINED) {
                if (parse_pktlen == 0) {
                    cli_out("Need to assign Length "
                            "when length mode is user-defined\n");
                    SHR_IF_ERR_EXIT(SHR_E_PARAM);
                }
                pkt_len = parse_pktlen;
            } else {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL); /* should not happen */
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

    if (test_phase == 1) {
        param->config_n_send = true;
        param->check_n_stop  = false;
        param->need_check_rx = false;
        /* coverity[dont_call] */
        seed = sal_rand();
    }
    if (test_phase == 2) {
        param->config_n_send = false;
        param->check_n_stop = true;
        param->need_check_rx = true;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
             (BSL_META_U(unit, "config_n_send = %0d, check_n_stop= %0d\n"),
              param->config_n_send, param->check_n_stop));
exit:
    if (pt_is_init == true) {
        bcma_cli_parse_table_done(&pt);
    }
    SHR_FUNC_EXIT();
}


static void
print_test_params(int unit, void *bp)
{
    traffic_max_power_param_t *tra_param = (traffic_max_power_param_t *)bp;

    cli_out("======= TR 517 TEST PARAMETERS =============\n");
    cli_out("  loopback_mode    =%0d\n", tra_param->loopback_mode);
    cli_out("  pkt_len_mode     =%0d\n", tra_param->pkt_len_mode);
    cli_out("  pkt_cell_limit   =%0d\n", tra_param->pkt_cell_limit);
    cli_out("  runtime          =%0d\n", tra_param->runtime);
    cli_out("  interval         =%0d\n", tra_param->interval);
    cli_out("  run_2_end        =%0d\n", tra_param->run_2_end);
    cli_out("  config_n_send    =%0d\n", tra_param->config_n_send);
    cli_out("  check_n_stop     =%0d\n", tra_param->check_n_stop);
    cli_out("  need_check_rx    =%0d\n", tra_param->need_check_rx);
    cli_out("  need_check_speed =%0d\n", tra_param->need_check_speed);
    cli_out("  enable_defip     =%0d\n", tra_param->enable_defip);
    cli_out("  enable_dlb       =%0d\n", tra_param->enable_dlb);
    cli_out("  enable_mirror    =%0d\n", tra_param->enable_mirror);
    cli_out("  enable_ifp       =%0d\n", tra_param->enable_ifp);
    cli_out("  enable_rdb       =%0d\n", tra_param->enable_rdb);
    cli_out("  enable_alpm       =%0d\n", tra_param->enable_alpm);
    cli_out("  enable_efp       =%0d\n", tra_param->enable_efp);
    cli_out("  enable_flex_ctr  =%0d\n", tra_param->enable_flex_ctr);
    cli_out("  enable_rnd_hdr   =%0d\n", tra_param->enable_rnd_hdr);
    cli_out("  enable_bmp       =0x%"PRIx32"\n", tra_param->enable_bmp);
    cli_out("  enable_defip     =%0d\n", tra_param->enable_defip);
    cli_out("  lpm_level        =%0d\n", tra_param->lpm_level);
    cli_out("  topology         =%0d\n", tra_param->topology);
    cli_out("  pkt_flow         =%0d\n", tra_param->pkt_flow);
    cli_out("==================================================\n");
}

static int
traffic_max_power_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                      uint32_t flags, void **bp)
{
    traffic_max_power_param_t *tra_param = NULL;
    bool show = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_ALLOC(tra_param, sizeof(traffic_max_power_param_t),
              "bcmaTestCaseTrafficPairPrm");
    SHR_NULL_CHECK(tra_param, SHR_E_MEMORY);

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    /* init */
    BCMDRD_PBMP_CLEAR(tra_param->pbmp_all);
    BCMDRD_PBMP_CLEAR(tra_param->pbmp_oversub);
    tra_param->pkt_len_mode = TRAFFIC_MAX_POWER_DEFAULT_PKTLEN_MODE;
    tra_param->runtime = TRAFFIC_MAX_POWER_DEFAULT_RUNNING_TIME;
    tra_param->interval = TRAFFIC_MAX_POWER_DEFAULT_POLLING_INTERVAL;
    tra_param->pkt_cell_limit = TRAFFIC_MAX_POWER_DEFAULT_CELL_MAXLIMIT;
    tra_param->run_2_end = TRAFFIC_MAX_POWER_DEFAULT_RUN_UNTIL_END;
    tra_param->need_check_rx = TRAFFIC_MAX_POWER_DEFAULT_CHECK_RX;
    tra_param->need_check_speed = TRAFFIC_MAX_POWER_DEFAULT_CHECK_SPEED;
    tra_param->enable_defip = TRAFFIC_MAX_POWER_DEFAULT_ENABLE_DEFIP;
    tra_param->enable_dlb = TRAFFIC_MAX_POWER_DEFAULT_ENABLE_DLB;
    tra_param->enable_mirror = TRAFFIC_MAX_POWER_DEFAULT_ENABLE_MIRROR;
    tra_param->enable_ifp = TRAFFIC_MAX_POWER_DEFAULT_ENABLE_IFP;
    tra_param->enable_rdb = TRAFFIC_MAX_POWER_DEFAULT_ENABLE_RDB;
    tra_param->enable_alpm = TRAFFIC_MAX_POWER_DEFAULT_ENABLE_ALPM;
    tra_param->enable_efp = TRAFFIC_MAX_POWER_DEFAULT_ENABLE_EFP;
    tra_param->enable_flex_ctr = TRAFFIC_MAX_POWER_DEFAULT_ENABLE_FLEX_CTR;
    tra_param->enable_rnd_hdr = TRAFFIC_MAX_POWER_DEFAULT_ENABLE_RND_HDR;
    tra_param->lpm_level      = -1;
    tra_param->topology       = -1;
    tra_param->pkt_flow       = -1;
    tra_param->enable_bmp = 0xffffffff;
    tra_param->config_n_send = TRAFFIC_MAX_POWER_DEFAULT_CONFIG_N_SEND;
    tra_param->check_n_stop = TRAFFIC_MAX_POWER_DEFAULT_CHECK_N_STOP;
    tra_param->loopback_mode = LB_TYPE_MAC;
    sal_memset(tra_param->pkt_len, 0, sizeof(tra_param->pkt_len));
    sal_memset(tra_param->pkt_cnt, 0, sizeof(tra_param->pkt_cnt));

    /* parse */
    SHR_IF_ERR_EXIT
        (traffic_max_power_parse_param(unit, cli, a, tra_param, show));
    if (show == true) {
        SHR_FREE(tra_param);
        SHR_EXIT();
    }

    /* check */
    if (tra_param->runtime == 0 || tra_param->interval == 0 ||
        BCMDRD_PBMP_IS_NULL(tra_param->pbmp_all)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    *bp = (void *)tra_param;

    /* Call chip specific power init
     * It can process the parsed params and also parse more chip specific params
     */
    SHR_IF_ERR_EXIT(bcma_testutil_power_init(unit, tra_param));

    print_test_params(unit, tra_param);

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(tra_param);
    }
    SHR_FUNC_EXIT();
}

static int
get_int_pattern(const char *str, int *pattern_size, int *length_pattern)
{
    char ch;
    const char *cptr = str;
    int length = -1;
    int indx;

    if (str == NULL) {
        return -1;
    }

    indx = 0;
    do {
        ch = *cptr++;
        if (ch >= '0' && ch <= '9') {
            if (length < 0) {
                length = 0;
            }
            length = (length * 10) + (ch - '0');
        } else {
            if (ch == ',' || ch == 0) {
                length_pattern[indx] = length;
                indx ++;
                if (length < 0) {
                    return -1;
                }
                length = -1;
            } else {
                return -1;
            }
        }

    } while (ch != 0);

    *pattern_size = indx;

    return 0;
}



static void
traffic_max_power_help(int unit, void *bp)
{
    cli_out("%s", TRAFFIC_MAX_POWER_PARAM_HELP);
    /* chip specific help */
    bcma_testutil_power_help(unit);
}

static int
traffic_max_power_set_cb(int unit, void *bp, uint32_t option)
{
    traffic_max_power_param_t *tra_param = (traffic_max_power_param_t *)bp;

    SHR_FUNC_ENTER(unit);

    if (tra_param->config_n_send) {
        SHR_IF_ERR_EXIT(bcma_testutil_power_config_set(unit, tra_param));
    }

exit:
    SHR_FUNC_EXIT();
}


static int
traffic_max_power_port_set_cb(int unit, void *bp, uint32_t option)
{
    int port;
    traffic_max_power_param_t *tra_param = (traffic_max_power_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);
    if (tra_param->config_n_send) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_port_setup(unit, tra_param->loopback_mode,
                                      tra_param->pbmp_all));
        SHR_IF_ERR_EXIT
            (bcma_testutil_port_setup(unit, tra_param->loopback_mode,
                                      tra_param->mirror_pbmp));
        BCMDRD_PBMP_ITER(tra_param->mirror_pbmp, port) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_port_mac_control_set(unit, port, false, true));
        }
        SHR_IF_ERR_EXIT(bcma_testutil_power_port_set(unit, bp));
    }


exit:
    SHR_FUNC_EXIT();
}


static int
traffic_max_power_txrx_cb(int unit, void *bp, uint32_t option)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcma_testutil_power_txrx(unit, bp));

exit:
    SHR_FUNC_EXIT();
}


static int
traffic_max_power_cleanup_cb(int unit, void *bp, uint32_t option)
{
    traffic_max_power_param_t *tra_param = (traffic_max_power_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (tra_param->check_n_stop) {
        /* restore port setting */
        LOG_VERBOSE(BSL_LOG_MODULE,
             (BSL_META("restore port setting\n")));

        SHR_IF_ERR_EXIT
            (bcma_testutil_port_clear(unit, tra_param->loopback_mode,
                                      tra_param->pbmp_all, TRUE));
    }

    cli_out("traffic_max_power_cleanup_cb() FINISHED \n");
exit:
    SHR_FUNC_EXIT();
}

static bcma_test_proc_t traffic_max_power_proc[] = {
    {
        .desc = "setup ports operating mode, wait link up\n",
        .cb = traffic_max_power_port_set_cb,
    },
    {
        .desc = "create max power scenario\n",
        .cb = traffic_max_power_set_cb,
    },
    {
        .desc = "TX/RX test\n",
        .cb = traffic_max_power_txrx_cb,
    },
    {
        .desc = "cleanup port and vlan configuration\n",
        .cb = traffic_max_power_cleanup_cb,
    }
};

static void
traffic_max_power_recycle(int unit, void *bp)
{
    return;
}

static bcma_test_op_t traffic_max_power_op = {
    .select = traffic_max_power_select,
    .parser = traffic_max_power_parser,
    .help = traffic_max_power_help,
    .recycle = traffic_max_power_recycle,
    .procs = traffic_max_power_proc,
    .proc_count = COUNTOF(traffic_max_power_proc)
};

bcma_test_op_t *
bcma_testcase_traffic_max_power_op_get(void)
{
    return &traffic_max_power_op;
}
