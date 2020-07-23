/*! \file bcma_testcase_traffic_flex.c
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
#include <bcma/test/util/bcma_testutil_vlan.h>
#include <bcma/test/util/bcma_testutil_port.h>
#include <bcma/test/util/bcma_testutil_stat.h>
#include <bcma/test/util/bcma_testutil_packet.h>
#include <bcma/test/util/bcma_testutil_traffic.h>
#include <bcma/test/testcase/bcma_testcase_traffic_flex.h>


static const shr_mac_t default_src_mac = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
static const shr_mac_t default_dst_mac = {0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb};

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define TRAFFIC_FLEX_CPU_PORT                   (0)
#define TRAFFIC_FLEX_VLAN_ID_BASE               (100)
#define TRAFFIC_FLEX_VLAN_PROFILE_ID_BASE       (0)
#define TRAFFIC_FLEX_MAX_NUM_VLANS              (50)
#define TRAFFIC_FLEX_L2_MC_GROUP_ID_BASE        (0)
#define TRAFFIC_FLEX_DEFAULT_NETIF_ID           (1)
#define TRAFFIC_FLEX_DEFAULT_RUNNING_TIME       (10)
#define TRAFFIC_FLEX_DEFAULT_POLLING_INTERVAL   (2)
#define TRAFFIC_FLEX_DEFAULT_PACKET_LENGTH      (1514)
#define TRAFFIC_FLEX_DEFAULT_PACKET_PATTERN     (0x0)
#define TRAFFIC_FLEX_DEFAULT_PACKET_ETHERTYPE   (0xffff)
#define TRAFFIC_FLEX_DEFAULT_PACKET_PATTERN_INC (0)
#define TRAFFIC_FLEX_DEFAULT_INJECT_COUNT       (1)
#define TRAFFIC_FLEX_DEFAULT_RUN_UNTIL_END      (FALSE)
#define TRAFFIC_FLEX_TX_CHAN                    (0)
#define TRAFFIC_FLEX_RX_CHAN                    (1)
#define TRAFFIC_FLEX_PKT_MAX_FRAME_SIZE         (1536)
#define TRAFFIC_FLEX_RATE_SCALING_FACTOR        (1)
#define TRAFFIC_FLEX_DEFAULT_CHECK_RX           (TRUE)
#define TRAFFIC_FLEX_DEFAULT_CHECK_SPEED        (TRUE)
#define TRAFFIC_FLEX_DEFAULT_CHECK_SPEED_MARGIN (5) /* pass tolerance is 5% */
#define TRAFFIC_FLEX_DEFAULT_RX_CHECK_WAIT      (50) /* 10 sec */

#define TRAFFIC_FLEX_MAX_NUM_PMS                (20)
#define TRAFFIC_FLEX_MAX_STR_SIZE               (32)
#define TRAFFIC_FLEX_MAX_PRINT_NUM_PKT          (10)


#define TRAFFIC_FLEX_PARAM_HELP \
    "  PortList             - Logical port list to be tested\n"\
    "  PhysicalPortListDown - Physical port list for down ports\n"\
    "  PhysicalPortListUp   - Physical port list for up ports\n"\
    "  LoopBack             - Loopback mode when running this testing (MAC / PHY / EXT)\n"\
    "  CounterPollInterval  - Time interval between each counter polling (in sec)\n"\
    "  TimeInSeconds        - Total testing time (in sec)\n"\
    "  RxCheckWaitTime      - Wait time for RX data check (in sec)\n"\
    "  Length               - Packet length\n"\
    "  Pattern              - Packet pattern filled into payload (4-byte)\n"\
    "  PatternIncrement     - Packet pattern increment\n"\
    "  Count                - Packet count\n"\
    "  Run2End              - Keep testing until the end\n"\
    "  CheckData            - Redirect packet back to CPU and check data content\n"\
    "  CheckLineRate        - Check if traffic speed meets line rate\n"\
    "  PMMode               - Flex PM Mode (4x50G / 2x100G / 1x200G / 1x400G / etc) \n\n"\
    "  TestMode             - Test mode: 0->traffic before/after flex; 1->traffic before flex; 2->traffic after flex\n"\
    "  VlanMode             - VLAN mode: 0->flex with no background traffic; 1->flex with background traffic\n"\
    "\nExample:\n"\
    "    Run traffic testing for 64B packet without DataCheck\n"\
    "      tr 514 pl=1-8 l=64 cd=0 clr=0 cpi=10 pmm=1x200G \n"\
    "    Run traffic testing in phy-loopback mode \n"\
    "      tr 514 pl=2,4,7-10 rm=phy pmm=1x400G \n"\
    "    Run traffic testing in external-loopback mode \n"\
    "      tr 514 pl=2,4,7-10 rm=ext pmm=1x400G \n"\
    "    Run traffic testing with specified up/down ports \n"\
    "      tr 514 pl=1-4 ppld=1,9 pplu=1,5,9,13 pmm=1x200G\n"\
    "    Run traffic testing for 10 sec, polling interval is 2 sec (it would check counter 5 times)\n"\
    "      tr 514 pl=1-4 tis=10 cpi=2 pmm=1x400G\n"\
    "    Run traffic testing with 64-byte packet, payload is filled with 0xffffffff repeatedly\n"\
    "      tr 514 pl=1-4 l=64 p=0xffffffff pmm=1x400G\n"\
    "    Run traffic testing with 64-byte packet, payload is filled with 0x00000001, 00000002...\n"\
    "      tr 514 pl=1-4 l=64 p=0x00000001 pi=1 pmm=1x400G\n"\
    "    Run traffic testing by injecting 10 packets into each testing port\n"\
    "      tr 514 pl=1-4 c=10 pmm=1x400G\n"\
    "    Run traffic testing until the end even if there is some port pair traffic stopped\n"\
    "      tr 514 pl=1-4 r2e=true pmm=1x400G\n"\
    "    Run traffic testing on emulator, require dm=2\n"\
    "      tr 514 pl=1-4 cpi=5 cd=1 clr=0 c=1 pmm=1x400G dm=2\n"\
    "    Run traffic testing on bcmsim, require cpi=10\n"\
    "      tr 514 pl=1-4 cpi=10 cd=0 clr=0 c=1 pmm=1x400G\n"\
    "    Run traffic testing with backgound traffic on non-flexed active ports\n"\
    "      tr 514 pl=1-4 cpi=5 cd=0 clr=1 c=200 pmm=1x200G tm=0 vm=1 ppld=1,9 pplu=1,5,9,13\n"



/* Loopback mode */
static bcma_cli_parse_enum_t testcase_traffic_lb_str[] = {
    {"MAC", LB_TYPE_MAC},
    {"PHY", LB_TYPE_PHY},
    {"EXT", LB_TYPE_EXT},
    {NULL,   0}
};

/* L2 action */
typedef enum traffic_flex_l2_action_e {
    /* setup loop by connecting each two adjacent ports */
    L2_CONNECT = 0,

    /* redirect all packet back to cpu */
    L2_REDIRECT,

    /* clean l2 setup */
    L2_CLEAN
} traffic_flex_l2_action_t;

/* PM mode */
typedef enum traffic_flex_pm_mode_e {
    PM_MODE_0 = 0,
    PM_MODE_4X10G,
    PM_MODE_1X40G,
    PM_MODE_2X40G,
    PM_MODE_4X25G,
    PM_MODE_2X25G_1X50G,
    PM_MODE_1X50G_2X25G,
    PM_MODE_2X50G,
    PM_MODE_4X50G,
    PM_MODE_1X100G,
    PM_MODE_2X100G,
    PM_MODE_4X100G,
    PM_MODE_1X200G,
    PM_MODE_2X200G,
    PM_MODE_1X400G,
    PM_MODE_1X100G_2X50G,
    PM_MODE_2X50G_1X100G,
    PM_MODE_UNCHANGE,
    PM_MODE_RAND,
    PM_MODE_COUNT
} traffic_flex_pm_mode_t;

/* PM mode string to value map */
static bcma_cli_parse_enum_t traffic_flex_pm_mode_map[] = {
    {"0G",          PM_MODE_0},
    {"4x10G",       PM_MODE_4X10G},
    {"1x40G",       PM_MODE_1X40G},
    {"2x40G",       PM_MODE_2X40G},
    {"4x25G",       PM_MODE_4X25G},
    {"2x25G_1x50G", PM_MODE_2X25G_1X50G},
    {"1x50G_2x25G", PM_MODE_1X50G_2X25G},
    {"2x50G",       PM_MODE_2X50G},
    {"4x50G",       PM_MODE_4X50G},
    {"1x100G",      PM_MODE_1X100G},
    {"2x100G",      PM_MODE_2X100G},
    {"4x100G",      PM_MODE_4X100G},
    {"1x200G",      PM_MODE_1X200G},
    {"2x200G",      PM_MODE_2X200G},
    {"1x400G",      PM_MODE_1X400G},
    {"1x100G_2x50G",PM_MODE_1X100G_2X50G},
    {"2x50G_1x100G",PM_MODE_2X50G_1X100G},
    {"RAND",        PM_MODE_RAND},
    {NULL,          0}
};

/* VLAN mode */
typedef enum traffic_flex_vlan_mode_e {
    /* Assigned vlan in a dynamic flavor */
    VLAN_MODE_VID_BY_PPORT = 0,

    /* Assigned vlan in a fixed flavor */
    VLAN_MODE_VID_BY_LPORT = 1,
} traffic_flex_vlan_mode_t;

/* Test mode */
typedef enum traffic_flex_test_mode_e {
    /* Send traffic before and after flexport operation */
    TEST_MODE_TRAFFIC_BEFORE_AFTER_FLEX = 0,

    /* Send traffic before flexport operation */
    TEST_MODE_TRAFFIC_BEFORE_FLEX = 1,

    /* Send traffic after flexport operation */
    TEST_MODE_TRAFFIC_AFTER_FLEX = 2
} traffic_flex_test_mode_t;

/* Parameter */
typedef struct traffic_flex_param_s {
    /* Logical port bitmap for traffic ports */
    bcmdrd_pbmp_t pbmp_all;

    /* Logical port bitmap for macsec traffic ports */
    bcmdrd_pbmp_t pbmp_msall;

    /* Logical port bitmap for unchanged ports */
    bcmdrd_pbmp_t pbmp_unchanged;

    /* Physical port bitmap for down ports */
    bcmdrd_pbmp_t phy_pbmp_down;

    /* Physical port bitmap for up ports */
    bcmdrd_pbmp_t phy_pbmp_up;

    /* Oversubscribed ports */
    bcmdrd_pbmp_t pbmp_oversub;

    /* Port pair count */
    uint32_t port_cnt;

    /* Loopback type. */
    testutil_port_lb_type_t lb_type;

    /* Running time (in sec) */
    uint32_t runtime;

    /* Polling interval (in sec) */
    uint32_t interval;

    /* RX check wait time (in sec) */
    uint32_t rx_check_wait;

    /* Packet length */
    uint32_t length;

    /* Packet pattern */
    uint32_t pattern;

    /* Packet pattern increment */
    uint32_t pattern_inc;

    /* Inject packet count */
    uint32_t inject_cnt;

    /* Run until end even if error happen */
    int run_2_end;

    /* Whether to check packet back to CPU after unlock loop */
    int need_check_rx;

    /* Whether to check speed meet line rate */
    int need_check_speed;

    /* Rate scaling factor */
    int rate_scaling_factor;

    /* PM mode to be flexed */
    traffic_flex_pm_mode_t cfg_pm_mode;

    /* PM mode for flexing in enum value*/
    traffic_flex_pm_mode_t pm_mode[TRAFFIC_FLEX_MAX_NUM_PMS];

    /* PM mode for flexing in string */
    char pm_mode_str[TRAFFIC_FLEX_MAX_NUM_PMS][TRAFFIC_FLEX_MAX_STR_SIZE];

    /* Test mode */
    int test_mode;

    /* VLAN mode */
    int vlan_mode;

    /* Flag to indicate whether or not before flex operation */
    bool is_before_flex;

    /* Logical pbmp for vlan setup/clean */
    bcmdrd_pbmp_t vlan_pbmp;

    /* Logical pbmp for test internal usage */
    bcmdrd_pbmp_t valid_pbmp;

} traffic_flex_param_t;

static int
traffic_flex_get_l2p_pmap(int unit, uint32_t map_len, uint32_t *l2p_map)
{
    bcmdrd_pbmp_t lport_bmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(l2p_map, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_testutil_default_port_bmp_get
         (unit, BCMDRD_PORT_TYPE_FPAN, &lport_bmp));
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_flex_l2p_map_get
         (unit, lport_bmp, map_len, l2p_map));

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_flex_print_port_info(int unit, bcmdrd_pbmp_t pbmp)
{
    uint32_t l2p_map[BCMDRD_CONFIG_MAX_PORTS];
    uint32_t lport_speed[BCMDRD_CONFIG_MAX_PORTS];
    uint32_t idx = 0, lport;

    SHR_FUNC_ENTER(unit);

    sal_memset(l2p_map, 0, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint32_t));
    SHR_IF_ERR_EXIT
        (traffic_flex_get_l2p_pmap(unit, BCMDRD_CONFIG_MAX_PORTS, l2p_map));

    sal_memset(lport_speed, 0, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint32_t));
    SHR_IF_ERR_EXIT
        (bcma_testutil_default_port_speed_list_get
         (unit, BCMDRD_CONFIG_MAX_PORTS, lport_speed));

    BCMDRD_PBMP_ITER(pbmp, lport) {
        cli_out("port_info: idx=%2d, lport=%3d, pport=%3d, speed=%0dG\n",
                idx++, lport, l2p_map[lport], lport_speed[lport] / 1000);
    }

exit:
    SHR_FUNC_EXIT();
}



static void
traffic_flex_print_pbmp(int unit, bcmdrd_pbmp_t pbmp, char *desc)
{
    uint32_t lport;

    if (desc != NULL) cli_out("%s\n", desc);
    BCMDRD_PBMP_ITER(pbmp, lport) {
        cli_out("%3d, ", lport);
    }
    cli_out("\n");
}

static void
traffic_flex_print_lb_type(int unit, testutil_port_lb_type_t lb_type)
{
    uint32_t map_len;
    const char *lb_type_str = "\0";

    map_len = COUNTOF(testcase_traffic_lb_str);
    lb_type_str = bcma_testutil_id2name(testcase_traffic_lb_str, map_len,
            lb_type);
    if (lb_type_str != NULL) {
        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit, "lb_type mode = %s\n"), lb_type_str));
    } else {
        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit, "lb_type mode = %s\n"), "*"));
    }
}

static int
traffic_flex_select(int unit)
{
    return true;
}





static int
traffic_flex_parse_param(bcma_cli_t *cli, bcma_cli_args_t *a,
        traffic_flex_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    bool pt_is_init = false;
    char *parse_lgc_pbmp = NULL, *parse_unchanged_lgc_pbmp = NULL,
         *parse_phy_pbmp_down = NULL, *parse_phy_pbmp_up = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    /* setup parse table */
    bcma_cli_parse_table_init(cli, &pt);
    pt_is_init = true;

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PortList", "str",
                                  &parse_lgc_pbmp, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "UnchangedPortList", "str",
                                  &parse_unchanged_lgc_pbmp, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PhysicalPortListUp", "str",
                                  &parse_phy_pbmp_up, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PhysicalPortListDown", "str",
                                  &parse_phy_pbmp_down, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "LoopBack", "enum", &(param->lb_type),
                                  &testcase_traffic_lb_str));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "CounterPollInterval", "int",
                                  &param->interval, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "TimeInSeconds", "int",
                                  &param->runtime, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "RxCheckWaitTime", "int",
                                  &param->rx_check_wait, NULL));
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
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PMMode", "enum",
                                  &(param->cfg_pm_mode),
                                  &traffic_flex_pm_mode_map));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "RateScalingFactor", "int",
                                  &param->rate_scaling_factor, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "TestMode", "int",
                                  &param->test_mode, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "VlanMode", "int",
                                  &param->vlan_mode, NULL));

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

    /* translate pbmp */
    if (parse_lgc_pbmp != NULL) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_parse_port_bmp(parse_lgc_pbmp, &param->pbmp_all));
    }

    if (parse_unchanged_lgc_pbmp != NULL) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_parse_port_bmp(parse_unchanged_lgc_pbmp,
                                          &param->pbmp_unchanged));
    }
    if (parse_phy_pbmp_down != NULL) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_parse_port_bmp(parse_phy_pbmp_down,
                                          &param->phy_pbmp_down));
    }
    if (parse_phy_pbmp_up != NULL) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_parse_port_bmp(parse_phy_pbmp_up,
                                          &param->phy_pbmp_up));
    }

    /* vlan_mode && vlan_pbmp */
    if (param->vlan_mode == VLAN_MODE_VID_BY_LPORT) {
        uint32_t lport;
        bcmdrd_pbmp_t pbmp;

        BCMDRD_PBMP_CLEAR(pbmp);
        for (lport = 1; lport <= TRAFFIC_FLEX_MAX_NUM_VLANS; lport++) {
            BCMDRD_PBMP_PORT_ADD(pbmp, lport);
        }
        BCMDRD_PBMP_ASSIGN(param->vlan_pbmp, pbmp);
    }

exit:
    if (pt_is_init == true) {
        bcma_cli_parse_table_done(&pt);
    }
    SHR_FUNC_EXIT();
}

static int
traffic_flex_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
        uint32_t flags, void **bp)
{
    traffic_flex_param_t *tra_param = NULL;
    bool show = false;
    int port_cnt = 0, port;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_ALLOC(tra_param, sizeof(traffic_flex_param_t),
            "bcmaTestCaseTrafficFlexPrm");
    SHR_NULL_CHECK(tra_param, SHR_E_MEMORY);
    sal_memset(tra_param, 0, sizeof(traffic_flex_param_t));

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    /* init */
    tra_param->lb_type = LB_TYPE_MAC;
    tra_param->runtime = TRAFFIC_FLEX_DEFAULT_RUNNING_TIME;
    tra_param->interval = TRAFFIC_FLEX_DEFAULT_POLLING_INTERVAL;
    tra_param->rx_check_wait = TRAFFIC_FLEX_DEFAULT_RX_CHECK_WAIT;
    tra_param->length = TRAFFIC_FLEX_DEFAULT_PACKET_LENGTH;
    tra_param->pattern = TRAFFIC_FLEX_DEFAULT_PACKET_PATTERN;
    tra_param->pattern_inc = TRAFFIC_FLEX_DEFAULT_PACKET_PATTERN_INC;
    tra_param->inject_cnt = TRAFFIC_FLEX_DEFAULT_INJECT_COUNT;
    tra_param->run_2_end = TRAFFIC_FLEX_DEFAULT_RUN_UNTIL_END;
    tra_param->need_check_rx = TRAFFIC_FLEX_DEFAULT_CHECK_RX;
    tra_param->need_check_speed = TRAFFIC_FLEX_DEFAULT_CHECK_SPEED;
    tra_param->rate_scaling_factor = TRAFFIC_FLEX_RATE_SCALING_FACTOR;
    tra_param->port_cnt = 0;
    tra_param->cfg_pm_mode = PM_MODE_1X400G;
    tra_param->test_mode = TEST_MODE_TRAFFIC_AFTER_FLEX;
    tra_param->vlan_mode = VLAN_MODE_VID_BY_PPORT;
    tra_param->is_before_flex = true;
    BCMDRD_PBMP_CLEAR(tra_param->pbmp_all);
    BCMDRD_PBMP_CLEAR(tra_param->pbmp_unchanged);
    BCMDRD_PBMP_CLEAR(tra_param->phy_pbmp_down);
    BCMDRD_PBMP_CLEAR(tra_param->phy_pbmp_up);
    BCMDRD_PBMP_CLEAR(tra_param->pbmp_oversub);
    BCMDRD_PBMP_CLEAR(tra_param->vlan_pbmp);
    BCMDRD_PBMP_CLEAR(tra_param->valid_pbmp);

    /* parse */
    SHR_IF_ERR_EXIT
        (traffic_flex_parse_param(cli, a, tra_param, show));
    if (show == true) {
        SHR_FREE(tra_param);
        SHR_EXIT();
    }

    /* check number of testing ports, should be greater than 0 */
    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
        port_cnt++;
    }
    if (port_cnt == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    tra_param->port_cnt = port_cnt;
    SHR_IF_ERR_EXIT(traffic_flex_print_port_info(unit, tra_param->pbmp_all));

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
traffic_flex_help(int unit, void *bp)
{
    cli_out("%s", TRAFFIC_FLEX_PARAM_HELP);
}

static int
traffic_flex_get_lport_vid_pid(int unit, traffic_flex_param_t *tra_param,
        uint32_t lport, uint32_t *vid, uint32_t *pid)
{
    uint32_t vlan_vid, vlan_pid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);
    SHR_NULL_CHECK(vid, SHR_E_PARAM);
    SHR_NULL_CHECK(pid, SHR_E_PARAM);

    if (tra_param->vlan_mode == VLAN_MODE_VID_BY_LPORT) {
        vlan_vid = lport + TRAFFIC_FLEX_VLAN_ID_BASE;
        vlan_pid = lport + TRAFFIC_FLEX_VLAN_PROFILE_ID_BASE;
    } else {
        uint32_t l2p_map[BCMDRD_CONFIG_MAX_PORTS];

        sal_memset(l2p_map, 0, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint32_t));
        SHR_IF_ERR_EXIT
            (traffic_flex_get_l2p_pmap(unit, BCMDRD_CONFIG_MAX_PORTS, l2p_map));

        vlan_vid = l2p_map[lport] + TRAFFIC_FLEX_VLAN_ID_BASE;
        vlan_pid = l2p_map[lport] + TRAFFIC_FLEX_VLAN_PROFILE_ID_BASE;
    }

    *vid = vlan_vid;
    *pid = vlan_pid;

exit:
    SHR_FUNC_EXIT();
}

static void
traffic_flex_pipe_macsec_config_set(int unit,void *bp,int port)
{
    bcmlt_entry_handle_t entry_hdl_1 = BCMLT_INVALID_HDL;
    const char *l2_bank[2] = {"EFTA30_E2T_00_BANK0", "EFTA30_E2T_00_BANK1"};
    traffic_flex_param_t *tra_param = (traffic_flex_param_t *)bp;
    uint32_t vid, pid;
    uint64_t macsec_block_id, macsec_enabled;
    int rv,lv;
    uint64_t mc_en;

    uint64_t mac_addr[8] = { 0x66778899aabb,0x66778899aabb, 0x66778899aabb, 0x66778899aabb,
        0x66778899aabb, 0x66778899aabb, 0x66778899aabb, 0x66778899aabb};

    SHR_FUNC_ENTER(unit);
    mc_en = 1;


    LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "SEC_PORT_INFO : lport=%0d\n"),
             port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "SEC_PORT_INFO", &entry_hdl_1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_1, "PORT_ID",
                               port));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get
         (entry_hdl_1, "SEC_BLOCK_ID", &macsec_block_id));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get
         (entry_hdl_1, "SEC", &macsec_enabled));

    bcmlt_entry_free(entry_hdl_1);

    if (macsec_enabled == 1){

        BCMDRD_PBMP_PORT_ADD(tra_param->pbmp_msall, port);
        cli_out("STEP: %s\n", "macsec_pipe_set");
        cli_out("STEP: %s\n", "configuring macsec 0-5 for mgmt pkts ");

        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "SEC_PORT_INFO : lport=%0d, SEC_BLOCK_ID=%"PRIu64", SEC=%"PRIu64"\n"),
                 port, macsec_block_id, macsec_enabled));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "SEC_DECRYPT_PORT", &entry_hdl_1));

        rv =  (bcmlt_entry_field_add(entry_hdl_1, "PORT_ID",
                    port));
        rv = (bcmlt_entry_field_array_add(entry_hdl_1, "MGMT_DST_MAC",0,
                    &mc_en,1));
        rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_INSERT,
                    BCMLT_PRIORITY_NORMAL));

        bcmlt_entry_free(entry_hdl_1);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "SEC_DECRYPT_MGMT", &entry_hdl_1));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl_1, "SEC_BLOCK_ID",
                                   macsec_block_id));
        lv =
            (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(entry_hdl_1, "DST_MAC",0,
                                         mac_addr,7));

        if (lv == SHR_E_NOT_FOUND)
        {
            rv =
                (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_INSERT,
                                    BCMLT_PRIORITY_NORMAL));
        }
        else
        {
            rv =
                (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_UPDATE,
                                    BCMLT_PRIORITY_NORMAL));
        }
        bcmlt_entry_free(entry_hdl_1);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "SEC_ENCRYPT", &entry_hdl_1));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl_1, "SEC_BLOCK_ID",
                                   macsec_block_id));
        lv =
            (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl_1, "SVTAG_ETHERTYPE",
                                   0x88));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl_1, "SVTAG_ETHERTYPE_MASK",
                                   0xFF));

        if (lv == SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_INSERT,
                                    BCMLT_PRIORITY_NORMAL));
        }
        else
        {
            rv =
                (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_UPDATE,
                                    BCMLT_PRIORITY_NORMAL));
        }

        bcmlt_entry_free(entry_hdl_1);
        rv = (bcmlt_entry_allocate(unit, "SEC_ENCRYPT_MACSEC_SC_POLICY", &entry_hdl_1));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "SEC_BLOCK_ID", macsec_block_id));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "SEC_ENCRYPT_MACSEC_SC_POLICY_ID", 0x0));
        lv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "SEC_BLOCK_ID", macsec_block_id));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "SEC_ENCRYPT_MACSEC_SC_POLICY_ID", 0x0));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "MTU", 1518));
        rv = (bcmlt_entry_field_symbol_add(entry_hdl_1, "UNSECURED_DATA_MODE", "ALLOW"));
        if (lv == SHR_E_NOT_FOUND)
            rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
        else
            rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "rv_ SEC_ENCRYPT_MACSEC_SC_POLICY: =%0d\n"), rv));
        bcmlt_entry_free(entry_hdl_1);
        traffic_flex_get_lport_vid_pid(unit, tra_param, port, &vid, &pid);
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "lport :  =%0d , vid : =%0d ,pid :=%0d \n"),
                 port,vid,pid));

        /*  ingress processing */
        /*  Insert PORT_PROPERTY (shared by all vlan id) */

        rv = bcmlt_entry_allocate(unit, "IDEV_CONFIG_TABLEm", &entry_hdl_1);
        rv = bcmlt_entry_field_add(entry_hdl_1, "BCMLT_PT_INDEX", port);
        rv = bcmlt_entry_field_add(entry_hdl_1, "SYSTEM_PORT", port);
        rv = bcmlt_entry_field_add(entry_hdl_1, "PARSER0_CTRL_ID", 0x4);
        rv = bcmlt_entry_field_add(entry_hdl_1, "DEVICE_PORT", port);
        rv = bcmlt_entry_field_add(entry_hdl_1, "ING_PP_PORT", port);
        rv = bcmlt_entry_field_add(entry_hdl_1, "CONFIG_DEVICE_PORT_TYPE", 1);
        rv = bcmlt_pt_entry_commit(entry_hdl_1, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL);
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv_IDEV_CONFIG_TABLEm : =%0d\n"),
                 rv));
        bcmlt_entry_free(entry_hdl_1);
        rv = bcmlt_entry_allocate(unit, "EPRE_EDEV_CONFIG_DEVICE_PORT_TABLEm", &entry_hdl_1);
        rv = bcmlt_entry_field_add(entry_hdl_1, "BCMLT_PT_INDEX", port);
        rv = bcmlt_entry_field_add(entry_hdl_1, "DEVICE_OPQ_CMD1", 0x1);
        rv = bcmlt_entry_field_add(entry_hdl_1, "L2_OIF", port);
        rv = bcmlt_pt_entry_commit(entry_hdl_1, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL);
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv_EPRE_EDEV_CONFIG_DEVICE_PORT_TABLEm : =%0d\n"),
                 rv));
        bcmlt_entry_free(entry_hdl_1);

        /* Bcm_cmd "lt EGR_L2_OIF update L2_OIF=$dst_port EDIT_CTRL_ID=0x2 TPID_0=0x8100 TPID_1=0x9100 STRENGTH_PRFL_IDX=0x3 EGR_PORT_GROUP_ID=$dst_port L2_PROCESS_CTRL=0x1" */
        rv = (bcmlt_entry_allocate(unit, "EGR_L2_OIF", &entry_hdl_1));
        rv = bcmlt_entry_field_add(entry_hdl_1, "L2_OIF", port);
        lv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
        rv = bcmlt_entry_field_add(entry_hdl_1, "EDIT_CTRL_ID", 0x2);
        rv = bcmlt_entry_field_add(entry_hdl_1, "TPID_0", 0x8100);
        rv = bcmlt_entry_field_add(entry_hdl_1, "TPID_1", 0x9100);
        rv = bcmlt_entry_field_add(entry_hdl_1, "STRENGTH_PRFL_IDX", 0x3);
        rv = bcmlt_entry_field_add(entry_hdl_1, "EGR_PORT_GROUP_ID", port);
        rv = bcmlt_entry_field_add(entry_hdl_1, "L2_PROCESS_CTRL", 0x1);
        rv = bcmlt_entry_field_add(entry_hdl_1, "L2_OIF", port);
        if (lv == SHR_E_NOT_FOUND)
            rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
        else
            rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "rv_EGR_L2_OIF : =%0d\n"), rv));
        bcmlt_entry_free(entry_hdl_1);


        rv = (bcmlt_entry_allocate(unit, "DEVICE_EM_TILE", &entry_hdl_1));
        rv = (bcmlt_entry_field_symbol_add(entry_hdl_1, "DEVICE_EM_TILE_ID", "EFTA30_E2T_00"));
        rv = (bcmlt_entry_field_symbol_add(entry_hdl_1, "mode", "EFTA30_E2T_00_MODE0"));
        rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry_hdl_1);
        rv = (bcmlt_entry_allocate(unit, "DEVICE_EM_GROUP", &entry_hdl_1));
        rv = (bcmlt_entry_field_symbol_add(entry_hdl_1, "DEVICE_EM_GROUP_ID", "EFTA30_E2T_00_MODE0_GROUP0"));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "NUM_BANKS", 2));
        rv = (bcmlt_entry_field_array_symbol_add(entry_hdl_1, "DEVICE_EM_BANK_ID", 0, l2_bank, 2));
        rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry_hdl_1);
        rv = (bcmlt_entry_allocate(unit, "TABLE_CONTROL", &entry_hdl_1));
        rv = (bcmlt_entry_field_symbol_add(entry_hdl_1, "TABLE_ID", "EGR_VLAN_TRANSLATION_SVTAG_ASSIGN_TABLE"));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "MAX_ENTRIES", 16384));
        rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry_hdl_1);

        rv = (bcmlt_entry_allocate(unit, "EGR_VLAN_TRANSLATION_SVTAG_ASSIGN_TABLE", &entry_hdl_1));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "VFI", vid));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "GROUP_ID", port));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "PRESERVE_OTAG", 1));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "PRESERVE_ITAG", 0));
        lv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "VFI", vid));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "GROUP_ID", port));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "PRESERVE_OTAG", 1));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "PRESERVE_ITAG", 0));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "SVTAG_OFFSET", 2));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "SVTAG_PKT_TYPE", 0));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "SVTAG_SC_INDEX", 0));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "STRENGTH_PRFL_IDX", 0xf));

        if (lv == SHR_E_NOT_FOUND)
            rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
        else
            rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv_EGR_VLAN_TRANSLATION_SVTAG_ASSIGN_TABLE : =%0d\n"),
                 rv));
        bcmlt_entry_free(entry_hdl_1);


        rv = (bcmlt_entry_allocate(unit,"ING_L2_IIF_TABLE", &entry_hdl_1));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "L2_IIF", port));
        lv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "L2_IIF", port));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "VLAN_TAG_PRESERVE_CTRL", 0x1));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "VLAN_TO_VFI_MAPPING_ENABLE", 0x0));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "VFI", vid));
        if (lv == SHR_E_NOT_FOUND)
            rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
        else
            rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "rv_ING_L2_IIF_TABLE : =%0d\n"), rv));
        bcmlt_entry_free(entry_hdl_1);

        rv = (bcmlt_entry_allocate(unit, "EGR_VXLT_SVTAG_ASSIGN_TABLE_STR_PROFILE", &entry_hdl_1));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "EGR_VXLT_SVTAG_ASSIGN_TABLE_STR_PROFILE_INDEX", 0xf));
        lv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "EGR_VXLT_SVTAG_ASSIGN_TABLE_STR_PROFILE_INDEX", 0xf));
        if (lv == SHR_E_NOT_FOUND)
            rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
        else
            rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "rv_EGR_VXLT_SVTAG_ASSIGN_TABLE_STR_PROFILE : =%0d\n"), rv));
        bcmlt_entry_free(entry_hdl_1);

        rv = (bcmlt_entry_allocate(unit, "R_EGR_SVTAG_SIGNATURE", &entry_hdl_1));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "VALUE", 0x88));
        lv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "VALUE", 0x88));
        if (lv == SHR_E_NOT_FOUND) {
            rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
        }

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "rv_R_EGR_SVTAG_SIGNATURE: =%0d\n"), rv));
        bcmlt_entry_free(entry_hdl_1);
    }
exit:
    if (entry_hdl_1 != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl_1);
    }

}


static void
traffic_flex_pipe_macsec_config_delete(int unit,void *bp)
{
    bcmlt_entry_handle_t entry_hdl_1 = BCMLT_INVALID_HDL;
    traffic_flex_param_t *tra_param = (traffic_flex_param_t *)bp;
    uint32_t vid, pid;
    int port;
    uint64_t macsec_block_id, macsec_enabled;
    int rv,lv;
    bcmdrd_pbmp_t pbmp_all;

    SHR_FUNC_ENTER(unit);
    BCMDRD_PBMP_ASSIGN(pbmp_all, tra_param->pbmp_msall);
    BCMDRD_PBMP_ITER(pbmp_all, port) {
    cli_out("STEP: %s\n", "macsec_pipe_delete");
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "SEC_PORT_INFO : lport=%0d\n"),
                 port));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "SEC_PORT_INFO", &entry_hdl_1));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl_1, "PORT_ID",
                                   port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get
             (entry_hdl_1, "SEC_BLOCK_ID", &macsec_block_id));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get
             (entry_hdl_1, "SEC", &macsec_enabled));

        bcmlt_entry_free(entry_hdl_1);

        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "SEC_PORT_INFO : lport=%0d, SEC_BLOCK_ID=%"PRIu64", SEC=%"PRIu64"\n"),
                 port, macsec_block_id, macsec_enabled));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "SEC_DECRYPT_PORT", &entry_hdl_1));

        rv =  (bcmlt_entry_field_add(entry_hdl_1, "PORT_ID",
                    port));
        rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_DELETE,
                    BCMLT_PRIORITY_NORMAL));

        bcmlt_entry_free(entry_hdl_1);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "SEC_DECRYPT_MGMT", &entry_hdl_1));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl_1, "SEC_BLOCK_ID",
                                   macsec_block_id));
        lv =
            (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL));

        bcmlt_entry_free(entry_hdl_1);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "SEC_ENCRYPT", &entry_hdl_1));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl_1, "SEC_BLOCK_ID",
                                   macsec_block_id));
        lv =
            (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL));

        bcmlt_entry_free(entry_hdl_1);
        rv = (bcmlt_entry_allocate(unit, "SEC_ENCRYPT_MACSEC_SC_POLICY", &entry_hdl_1));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "SEC_BLOCK_ID", macsec_block_id));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "SEC_ENCRYPT_MACSEC_SC_POLICY_ID", 0x0));
        lv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

        bcmlt_entry_free(entry_hdl_1);

        traffic_flex_get_lport_vid_pid(unit, tra_param, port, &vid, &pid);
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "lport :  =%0d , vid : =%0d ,pid :=%0d \n"),
                 port,vid,pid));

        /*  ingress processing */
        /*  Insert PORT_PROPERTY (shared by all vlan id) */

        bcmlt_entry_free(entry_hdl_1);

        rv = (bcmlt_entry_allocate(unit, "EGR_VLAN_TRANSLATION_SVTAG_ASSIGN_TABLE", &entry_hdl_1));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "VFI", vid));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "GROUP_ID", port));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "PRESERVE_OTAG", 1));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "PRESERVE_ITAG", 0));
        lv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "VFI", vid));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "GROUP_ID", port));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "PRESERVE_OTAG", 1));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "PRESERVE_ITAG", 0));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "SVTAG_OFFSET", 2));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "SVTAG_PKT_TYPE", 2));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "SVTAG_SC_INDEX", 1));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "STRENGTH_PRFL_IDX", 0xf));

        if (lv == SHR_E_EXISTS)
            rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv_EGR_VLAN_TRANSLATION_SVTAG_ASSIGN_TABLE : =%0d\n"),
                 rv));
        bcmlt_entry_free(entry_hdl_1);


        rv = (bcmlt_entry_allocate(unit,"ING_L2_IIF_TABLE", &entry_hdl_1));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "L2_IIF", port));
        lv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "L2_IIF", port));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "VLAN_TAG_PRESERVE_CTRL", 0x1));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "VLAN_TO_VFI_MAPPING_ENABLE", 0x0));
        rv = (bcmlt_entry_field_add(entry_hdl_1, "VFI", vid));
        if (lv == SHR_E_EXISTS)
            rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry_hdl_1);
    rv = (bcmlt_entry_allocate(unit, "EGR_VXLT_SVTAG_ASSIGN_TABLE_STR_PROFILE", &entry_hdl_1));
    rv = (bcmlt_entry_field_add(entry_hdl_1, "EGR_VXLT_SVTAG_ASSIGN_TABLE_STR_PROFILE_INDEX", 0xf));
    lv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
    rv = (bcmlt_entry_field_add(entry_hdl_1, "EGR_VXLT_SVTAG_ASSIGN_TABLE_STR_PROFILE_INDEX", 0xf));
    if (lv == SHR_E_EXISTS)
        rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

    bcmlt_entry_free(entry_hdl_1);
    rv = (bcmlt_entry_allocate(unit, "R_EGR_SVTAG_SIGNATURE", &entry_hdl_1));
    rv = (bcmlt_entry_field_add(entry_hdl_1, "VALUE", 0x88));
    lv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
    rv = (bcmlt_entry_field_add(entry_hdl_1, "VALUE", 0x88));
    if (lv == SHR_E_EXISTS)
        rv = (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry_hdl_1);
    }
exit:
    if (entry_hdl_1 != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl_1);
    }
}


static int
traffic_flex_vlan_set(int unit, void *bp, uint32_t option)
{
    traffic_flex_param_t *tra_param = (traffic_flex_param_t *)bp;
    uint32_t vid, pid;
    bool need_tag_action;
    bcmdrd_pbmp_t pbmp, ubmp, pbmp_all;
    int port;
    int rv;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;


    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    cli_out("STEP: %s\n", "vlan_set");

    BCMDRD_PBMP_ASSIGN(pbmp_all, tra_param->valid_pbmp);
    BCMDRD_PBMP_ASSIGN(pbmp, tra_param->valid_pbmp);
    BCMDRD_PBMP_PORT_ADD(pbmp, TRAFFIC_FLEX_CPU_PORT);
    BCMDRD_PBMP_ASSIGN(ubmp, pbmp);

    /* create vlan for each testing port */
    BCMDRD_PBMP_ITER(pbmp_all, port) {
        traffic_flex_get_lport_vid_pid(unit, tra_param, port, &vid, &pid);



        rv = (bcma_testutil_vlan_check(unit, vid));

        if (rv == SHR_E_NOT_FOUND)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "setup vlan: lport=%0d, vid=%0d, pid=%0d\n"),
                     port, vid, pid));

            SHR_IF_ERR_EXIT
                (bcma_testutil_vlan_create(unit, vid, pid));
            SHR_IF_ERR_EXIT
                (bcma_testutil_vlan_port_add(unit, vid, pbmp, ubmp, true));
            SHR_IF_ERR_EXIT
                (bcma_testutil_vlan_stp_set(unit, vid, pbmp,
                                            BCMA_TESTUTIL_STP_FORWARD));
            traffic_flex_pipe_macsec_config_set(unit,bp,port);
        }
        else {
            SHR_IF_ERR_EXIT
                (bcma_testutil_vlan_port_add(unit, vid, pbmp, ubmp, true));
            traffic_flex_pipe_macsec_config_set(unit,bp,port);
        }
    }

    /* iterate each port, assign different port-based vid */
    BCMDRD_PBMP_ITER(pbmp_all, port) {
        traffic_flex_get_lport_vid_pid(unit, tra_param, port, &vid, &pid);
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "setup port-based vid, lport=%0d, vid=%0d\n"),
                 port, vid));
        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_port_vlan_set(unit, port, vid));
    }

    /* untag action, should be "ADD" because our inject packet is untagged */
    if ((tra_param->vlan_mode == VLAN_MODE_VID_BY_PPORT) ||
            (tra_param->vlan_mode == VLAN_MODE_VID_BY_LPORT &&
             tra_param->is_before_flex)) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_drv_feature_get
             (unit, BCMA_TESTUTIL_FT_NEED_TAG_ACTION_FOR_PVID,
              &need_tag_action));

        if (need_tag_action == true) {
            cli_out("setup vlan: vlan_untag_action=%0d\n", need_tag_action);
            SHR_IF_ERR_EXIT
                (bcma_testutil_vlan_untag_action_add
                 (unit, BCMA_TESTUTIL_VLAN_TAG_ACTION_ADD));
        }
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
traffic_flex_vlan_set_wrap_cb(int unit, void *bp, uint32_t option)
{
    traffic_flex_param_t *tra_param = (traffic_flex_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    if (tra_param->vlan_mode == VLAN_MODE_VID_BY_LPORT) {
        if (tra_param->is_before_flex) {
            BCMDRD_PBMP_ASSIGN(tra_param->valid_pbmp, tra_param->vlan_pbmp);
            SHR_IF_ERR_EXIT
                (traffic_flex_vlan_set(unit, bp, option));
        }
    }
    else {
        BCMDRD_PBMP_ASSIGN(tra_param->valid_pbmp, tra_param->pbmp_all);
        if (!tra_param->is_before_flex)
            BCMDRD_PBMP_XOR(tra_param->valid_pbmp, tra_param->pbmp_unchanged);
        SHR_IF_ERR_EXIT
            (traffic_flex_vlan_set(unit, bp, option));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_flex_port_set_cb(int unit, void *bp, uint32_t option)
{
    traffic_flex_param_t *tra_param = (traffic_flex_param_t *)bp;
    bcmdrd_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    cli_out("STEP: %s\n", "lb_port_set");
    traffic_flex_print_lb_type(unit, tra_param->lb_type);

    BCMDRD_PBMP_ASSIGN(pbmp, tra_param->pbmp_all);
    if (tra_param->vlan_mode == VLAN_MODE_VID_BY_LPORT) {
        if (!tra_param->is_before_flex)
            BCMDRD_PBMP_XOR(pbmp, tra_param->pbmp_unchanged);
    }

    traffic_flex_print_pbmp(unit, pbmp, "pbmp for setting lb" );

    /* set port loopback mode */
    SHR_IF_ERR_EXIT
        (bcma_testutil_port_setup(unit, tra_param->lb_type, pbmp));

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_flex_l2_unicast_delete(int unit, int port, const shr_mac_t mac,
        int vid, int copy2cpu)
{
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_info_t trans_info;
    bcmlt_entry_handle_t entry_hdl;
    uint64_t mac_addr = 0, tmp;
    int idx;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < 6; idx++) {
        tmp = mac[idx];
        mac_addr <<= 8;
        mac_addr |= tmp;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    /* Delete L2_HOST_TABLE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L2_HOST_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VFI", vid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "MAC_ADDR", mac_addr));

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_DELETE,
                                     entry_hdl));

    /* Submit transaction. */
    SHR_IF_ERR_EXIT
        (bcmlt_transaction_commit(trans_hdl, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_info_get(trans_hdl, &trans_info));
    if (SHR_FAILURE(trans_info.status)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}



static int
traffic_flex_l2_forward(int unit, traffic_flex_param_t *tra_param,
        traffic_flex_l2_action_t action)
{
    uint32_t vid, pid;
    int port, port_cnt = 0, num_ports = 0;
    int port_list[BCMDRD_CONFIG_MAX_PORTS];
    int next_port_list[BCMDRD_CONFIG_MAX_PORTS];
    int port_paired[BCMDRD_CONFIG_MAX_PORTS];
    uint32_t l2p_map[BCMDRD_CONFIG_MAX_PORTS];
    uint32_t lport_speed[BCMDRD_CONFIG_MAX_PORTS];
    int i, j;
    bcmdrd_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    sal_memset(l2p_map, 0, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint32_t));
    SHR_IF_ERR_EXIT
        (traffic_flex_get_l2p_pmap(unit, BCMDRD_CONFIG_MAX_PORTS, l2p_map));

    sal_memset(lport_speed, 0, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint32_t));
    SHR_IF_ERR_EXIT
        (bcma_testutil_default_port_speed_list_get
         (unit, BCMDRD_CONFIG_MAX_PORTS, lport_speed));

    /* l2 forwarding rule
     *      p1 -> p2 -> p1
     *      p3 -> p4 -> p3
     *      p5 -> p6 -> p5
     */
    sal_memset(port_list, 0, sizeof(int) * BCMDRD_CONFIG_MAX_PORTS);
    sal_memset(next_port_list, 0, sizeof(int) * BCMDRD_CONFIG_MAX_PORTS);
    sal_memset(port_paired, 0, sizeof(int) * BCMDRD_CONFIG_MAX_PORTS);

    BCMDRD_PBMP_ASSIGN(pbmp, tra_param->valid_pbmp);
    BCMDRD_PBMP_ITER(pbmp, port) {
        port_list[num_ports++] = port;
    }
    for (port_cnt = 0; port_cnt < num_ports; port_cnt++) {
        if (port_cnt % 2 == 0) {
            if ((port_cnt + 1) < num_ports) {
                next_port_list[port_cnt] = port_list[port_cnt + 1];
            }
        } else {
            next_port_list[port_cnt] = port_list[port_cnt - 1];
        }
    }
    /* Pair same speed ports */
    for (i = 0; i < num_ports; i++) {
        if (port_paired[i]) continue;
        for (j = i + 1; j < num_ports; j++) {
            if (lport_speed[port_list[i]] == lport_speed[port_list[j]] &&
                    port_paired[j] == 0) {
                port_paired[i] = 1;
                port_paired[j] = 1;
                next_port_list[i] = port_list[j];
                next_port_list[j] = port_list[i];
                break;
            }
        }
    }

    for (port_cnt = 0; port_cnt < num_ports; port_cnt++) {
        int curr_port, next_port;
        switch (action) {
            case L2_CONNECT:
                curr_port = port_list[port_cnt];
                next_port = next_port_list[port_cnt];
                traffic_flex_get_lport_vid_pid(unit, tra_param,
                        curr_port, &vid, &pid);
                LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "L2_CONNECT: vid=%0d, src_port=%0d, \
                                    dst_port=%0d\n"),
                         vid, curr_port, next_port));
                SHR_IF_ERR_EXIT
                    (bcma_testutil_l2_unicast_add
                     (unit, next_port, default_dst_mac, vid, 0));
                break;
            case L2_REDIRECT:
                curr_port = port_list[port_cnt];
                next_port = TRAFFIC_FLEX_CPU_PORT;
                traffic_flex_get_lport_vid_pid(unit, tra_param,
                        curr_port, &vid, &pid);
                LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "L2_REDIRECT: vid=%0d, src_port=%0d, \
                                    dst_port=%0d\n"),
                         vid, curr_port, next_port));
                SHR_IF_ERR_EXIT
                    (bcma_testutil_l2_unicast_update
                     (unit, next_port, default_dst_mac, vid, 0));
                break;
            case L2_CLEAN:
                curr_port = port_list[port_cnt];
                next_port = next_port_list[port_cnt];
                traffic_flex_get_lport_vid_pid(unit, tra_param,
                        curr_port, &vid, &pid);
                LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "L2_CLEAN: vid=%0d, src_port=%0d, \
                                    dst_port=%0d\n"),
                         vid, curr_port, next_port));
                if (tra_param->is_before_flex) {
                    SHR_IF_ERR_EXIT
                        (traffic_flex_l2_unicast_delete
                         (unit, next_port, default_dst_mac, vid, 0));
                }
                else
                {
                    SHR_IF_ERR_EXIT
                        (bcma_testutil_l2_unicast_delete
                         (unit, next_port, default_dst_mac, vid, 0));
                }
                break;
            default:
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}



static int
traffic_flex_l2_forward_add(int unit, void *bp, uint32_t option)
{
    traffic_flex_param_t *tra_param = (traffic_flex_param_t *)bp;
    bcmdrd_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (tra_param->vlan_mode == VLAN_MODE_VID_BY_LPORT) {
        if (tra_param->is_before_flex) {
            BCMDRD_PBMP_ASSIGN(pbmp, tra_param->vlan_pbmp);
        } else {
            BCMDRD_PBMP_CLEAR(pbmp);
        }
    } else {
        BCMDRD_PBMP_ASSIGN(pbmp, tra_param->pbmp_all);
        if (!tra_param->is_before_flex)
            BCMDRD_PBMP_XOR(pbmp, tra_param->pbmp_unchanged);
    }

    cli_out("traffic_flex_l2_forward(): L2_CONNECT\n");
    BCMDRD_PBMP_ASSIGN(tra_param->valid_pbmp, pbmp);
    SHR_IF_ERR_EXIT
        (traffic_flex_l2_forward(unit, tra_param, L2_CONNECT));

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_flex_l2_forward_redirect(int unit, void *bp, uint32_t option)
{
    traffic_flex_param_t *tra_param = (traffic_flex_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    BCMDRD_PBMP_ASSIGN(tra_param->valid_pbmp, tra_param->pbmp_all);

    cli_out("traffic_flex_l2_forward(): L2_REDIRECT\n");
    SHR_IF_ERR_EXIT
        (traffic_flex_l2_forward(unit, tra_param, L2_REDIRECT));

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_flex_l2_forward_clean(int unit, void *bp, uint32_t option)
{
    traffic_flex_param_t *tra_param = (traffic_flex_param_t *)bp;
    bcmdrd_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (tra_param->vlan_mode == VLAN_MODE_VID_BY_LPORT) {
        if (tra_param->is_before_flex) {
            BCMDRD_PBMP_CLEAR(pbmp);
        } else {
            BCMDRD_PBMP_ASSIGN(pbmp, tra_param->vlan_pbmp);
        }
    } else {
        if (tra_param->is_before_flex) {
            BCMDRD_PBMP_ASSIGN(pbmp, tra_param->pbmp_all);
            BCMDRD_PBMP_XOR(pbmp, tra_param->pbmp_unchanged);
        } else {
            BCMDRD_PBMP_ASSIGN(pbmp, tra_param->pbmp_all);
        }
    }

    cli_out("traffic_flex_l2_forward(): L2_CLEAN\n");
    BCMDRD_PBMP_ASSIGN(tra_param->valid_pbmp, pbmp);
    SHR_IF_ERR_EXIT
        (traffic_flex_l2_forward(unit, tra_param, L2_CLEAN));

    if ((tra_param->is_before_flex == false)) {
        traffic_flex_pipe_macsec_config_delete(unit,bp);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_flex_set_expected_rate(int unit,
        traffic_flex_param_t *tra_param,
        uint32_t array_len, uint64_t *expected_rate)
{
    int port, port2, port_cnt;
    bcmdrd_pbmp_t pbmp_tmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);
    SHR_NULL_CHECK(expected_rate, SHR_E_PARAM);

    if (tra_param->need_check_speed == TRUE) {
        /* set original target speed */
        SHR_IF_ERR_EXIT
            (bcma_testutil_drv_traffic_expeceted_rate_get
             (unit, tra_param->pbmp_all, tra_param->pbmp_oversub,
              tra_param->length + 4, array_len, expected_rate));

        /* adjust exp_rate of paired port to the min */
        port_cnt = 0;
        BCMDRD_PBMP_ASSIGN(pbmp_tmp, tra_param->pbmp_all);
        BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
            BCMDRD_PBMP_PORT_REMOVE(pbmp_tmp, port);
            if (port_cnt++ % 2 == 1) continue;

            BCMDRD_PBMP_ITER(pbmp_tmp, port2) {
                if (expected_rate[port] > expected_rate[port2])
                    expected_rate[port] = expected_rate[port2];
                if (expected_rate[port2] > expected_rate[port])
                    expected_rate[port2] = expected_rate[port];
                break;
            }
        }

        /* set rate margin: expected_rate * 95% */
        BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
            cli_out("exp_rate: lport=%3d, ", port);
            cli_out("inject %0d packets with pkt_size = %0dB, ",
                    tra_param->inject_cnt, tra_param->length);
            cli_out("target_speed : ");
            bcma_testutil_stat_show(expected_rate[port], 1, true);
            cli_out("b/s, ");

            /* set rate margin: expected_rate * 95% */
            expected_rate[port] = (expected_rate[port] *
                    (100 - TRAFFIC_FLEX_DEFAULT_CHECK_SPEED_MARGIN)) / 100;

            cli_out("target_min_speed : ");
            bcma_testutil_stat_show(expected_rate[port], 1, true);
            cli_out("b/s \n");
        }
    }

exit:
    SHR_FUNC_EXIT();
}


static void
traffic_flex_databuf_edit_print(int unit, bcmpkt_data_buf_t *buf,bcmpkt_data_buf_t *buf1, shr_pb_t *pb, int macsec_en)
{
    uint8_t *pbuf;
    uint8_t *pbuf1;
    uint32_t idx;
    uint32_t len;



    pbuf = buf->data;
    bcmpkt_trim(buf1, 0);
    pbuf1 = buf1->data;
    shr_pb_printf(pb,"packet source content:\n");
    for (idx = 0; idx < buf->data_len; idx++) {
        shr_pb_printf(pb,"%02x ", pbuf[idx]);
    }
    shr_pb_printf(pb, "\n----------------------------------------------------------------------------------\n");
    shr_pb_printf(pb,"\n");
    len = buf->data_len ;
    len = len - 4 ;
    shr_pb_printf(pb,"packet exp content:\n");
    if (macsec_en){
        bcmpkt_put(buf1, len);
        for (idx = 0; idx < len; idx++) {
            if (idx > 11)
            {
                pbuf1[idx] = pbuf[idx+4];
            }
            else
            {
                pbuf1[idx] = pbuf[idx];
            }
            shr_pb_printf(pb,"%02x ", pbuf1[idx]);
        }
    }
    else {
        bcmpkt_put(buf1, buf->data_len);
        for (idx = 0; idx < buf->data_len; idx++) {
            pbuf1[idx] = pbuf[idx];
            shr_pb_printf(pb,"%02x ", pbuf1[idx]);
        }
    }

    shr_pb_printf(pb, "\n----------------------------------------------------------------------------------\n");
    shr_pb_printf(pb,"\n");
}

static int
traffic_flex_txrx_cb(int unit, void *bp, uint32_t option)
{
    traffic_flex_param_t *tra_param = (traffic_flex_param_t *)bp;
    int port, port_cnt = 0, num_ports = 0;
    bcmdrd_dev_type_t pkt_dev_type;
    uint32_t curtime;
    uint32_t idx = 0, num_pkt = 0;
    int netif_id, pktdev_id;
    int macsec_en;
    int rv ;
    int check_on;
    bool counter_checker_create = false, counter_checker_start = false;
    bool rxdata_checker_create = false, rxdata_checker_start = false;
    bool test_result = false;
    bool first_polling = true;
    bcmpkt_packet_t **packet_list = {NULL};
    uint64_t *expected_rate = NULL;
    bcmdrd_pbmp_t pbmp, chk_pbmp,ms_pbmp;
    shr_pb_t *pb;


    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    cli_out("STEP: %s\n", "txrx");

    BCMDRD_PBMP_ASSIGN(pbmp, tra_param->valid_pbmp);
    BCMDRD_PBMP_ASSIGN(ms_pbmp, tra_param->pbmp_msall);
    BCMDRD_PBMP_ITER(pbmp, port) {
        num_ports++;
    }
    num_pkt = num_ports * tra_param->inject_cnt;

    /* setup packet device */
    SHR_IF_ERR_EXIT
        (bcma_testutil_packet_dev_init
         (unit, TRAFFIC_FLEX_TX_CHAN, TRAFFIC_FLEX_RX_CHAN,
          TRAFFIC_FLEX_PKT_MAX_FRAME_SIZE,
          (tra_param->need_check_rx == TRUE) ? (num_pkt * 2) : num_pkt,
          &netif_id, &pktdev_id));

    /* alloacte packet buffer */
    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &pkt_dev_type));


    SHR_ALLOC(packet_list, num_pkt * sizeof(bcmpkt_packet_t *),
            "bcmaTestCaseTrafficFlexPkt");
    SHR_NULL_CHECK(packet_list, SHR_E_MEMORY);
    sal_memset(packet_list, 0, num_pkt * sizeof(bcmpkt_packet_t *));

    /* determine target speed : minimun port speed * 95% */
    SHR_ALLOC(expected_rate, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint64_t),
            "bcmaTestCaseTrafficFlexExpRate");
    SHR_NULL_CHECK(expected_rate, SHR_E_MEMORY);
    sal_memset(expected_rate, 0, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint64_t));
    traffic_flex_set_expected_rate(unit, tra_param, BCMDRD_CONFIG_MAX_PORTS,
            expected_rate);

    /* create counter checker */
    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_create(unit));

    counter_checker_create = true;

    BCMDRD_PBMP_ASSIGN(chk_pbmp, tra_param->valid_pbmp);
    BCMDRD_PBMP_OR(chk_pbmp, tra_param->pbmp_unchanged);
    BCMDRD_PBMP_ITER(chk_pbmp, port) {
        char show_msg[TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN];

        sal_snprintf(show_msg, TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN,
                "lport %3"PRId32" : ",
                port);
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_counter_checker_add
             (unit, port, true, true, expected_rate[port], show_msg));
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_start(unit));
    counter_checker_start = true;

    /* create rxdata checker */
    if (tra_param->need_check_rx == TRUE) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_rxdata_checker_create
             (unit, netif_id, pktdev_id, TRAFFIC_RXDATA_CHECKER_MODE_ANY));
        rxdata_checker_create = true;
    }

    /* iterate each port to send packet */
    cli_out("txrx: port_cnt=%0d, pkt_per_port=%0d, total_pkts=%0d\n",
            num_ports, tra_param->inject_cnt, num_pkt);

    /* clear stat */
    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT(bcma_testutil_stat_clear(unit, port));
    }

    bcma_testutil_packet_init();
    /* inject packet to port */
    port_cnt = 0;
    BCMDRD_PBMP_ITER(pbmp, port) {
        uint32_t inject_itr;
        if (BCMDRD_PBMP_MEMBER(ms_pbmp, port)) {
            macsec_en = 1;
        }
        else
        {
            macsec_en = 0;
        }
        cli_out("sent %0d pkts to lport %0d\n", tra_param->inject_cnt, port);
        /* bcmpkt_alloc limitation: 1024 */
        for (inject_itr = 0; inject_itr < tra_param->inject_cnt; inject_itr++) {
            idx = port_cnt * tra_param->inject_cnt + inject_itr;
            SHR_IF_ERR_EXIT
                (bcmpkt_alloc(pktdev_id, tra_param->length+4, BCMPKT_BUF_F_TX,
                              &(packet_list[idx])));
        }

        for (inject_itr = 0; inject_itr < tra_param->inject_cnt; inject_itr++) {
            bcmpkt_packet_t *packet;
            bcmpkt_packet_t *packet_clone = NULL;
            uint32_t pattern = tra_param->pattern;



            /* coverity[dont_call : FALSE] */
            pattern = (pattern == 0) ? (uint32_t)sal_rand() : pattern;

            idx = port_cnt * tra_param->inject_cnt + inject_itr;
            packet = packet_list[idx];

            if (tra_param->inject_cnt <= TRAFFIC_FLEX_MAX_PRINT_NUM_PKT)
                cli_out("tx: lport=%0d, port_pkt_idx=%0d, total_pkt_idx=%0d\n",
                        port, inject_itr, idx);

            /* to maintain same pkt lenghts between macsec enabled and disabled ports */
            if (macsec_en)
            {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_packet_fill_svtag
                     (packet->data_buf, (tra_param->length+4),
                      default_src_mac, default_dst_mac, false,1,0x88000801, 0,
                      TRAFFIC_FLEX_DEFAULT_PACKET_ETHERTYPE,
                      pattern, tra_param->pattern_inc));
            }
            else
            {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_packet_fill_svtag
                     (packet->data_buf, (tra_param->length),
                      default_src_mac, default_dst_mac, false,0,0x88000801, 0,
                      TRAFFIC_FLEX_DEFAULT_PACKET_ETHERTYPE,
                      pattern, tra_param->pattern_inc));

            }

            if (tra_param->need_check_rx == TRUE) {

                SHR_IF_ERR_EXIT
                    (bcmpkt_alloc(pktdev_id, tra_param->length, BCMPKT_BUF_F_TX, &packet_clone));
                SHR_NULL_CHECK(packet_clone, SHR_E_INTERNAL);
                pb = shr_pb_create();
                traffic_flex_databuf_edit_print(unit, packet->data_buf,packet_clone->data_buf, pb,macsec_en);
                LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "%s\n"), shr_pb_str(pb)));
                shr_pb_destroy(pb);

                SHR_IF_ERR_EXIT
                    ( bcma_testutil_traffic_rxdata_checker_add
                      (unit, -1, packet_clone->data_buf));

                rv = bcmpkt_free(pktdev_id, packet_clone);
                cli_out("release_clone_status : RV_value=%0d, SHR_E_NONE=%0d\n",
                        rv, SHR_E_NONE);
                packet_clone = NULL;
            }

            SHR_IF_ERR_EXIT
                (bcmpkt_fwd_port_set(pkt_dev_type, port, packet));

            SHR_IF_ERR_EXIT
                (bcmpkt_tx(unit, TRAFFIC_FLEX_DEFAULT_NETIF_ID, packet));


        }

        for (inject_itr = 0; inject_itr < tra_param->inject_cnt; inject_itr++) {
            idx = port_cnt * tra_param->inject_cnt + inject_itr;
            if (packet_list[idx] != NULL) {
                rv = bcmpkt_free(pktdev_id, packet_list[idx]);
                cli_out("release_packet_list : RV_value=%0d, idx=%0d\n",
                        rv, idx);
                packet_list[idx] = NULL;
            }
        }

        port_cnt++;
    }

    /* iterate each port pair to poll counter */
    cli_out("TRAFFIC: runtime=%0d, interval=%0d\n",
            tra_param->runtime, tra_param->interval);
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
                ( bcma_testutil_traffic_counter_checker_execute
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
    check_on = 1;
    /* as there will be background traffic -- no checking till the end for TEST_MODE_TRAFFIC_BEFORE_AFTER_FLEX */
    if ((tra_param->is_before_flex) && (tra_param->test_mode == TEST_MODE_TRAFFIC_BEFORE_AFTER_FLEX))
        check_on = 0;
    /* redirect all packet back to CPU and check */
    if ((tra_param->need_check_rx == TRUE) && (check_on == 1)) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_rxdata_checker_start(unit));

        rxdata_checker_start = true;

        /* udpate all vlan member to CPU */
        SHR_IF_ERR_EXIT
            (traffic_flex_l2_forward_redirect(unit, bp, option));

        /* check */
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_rxdata_checker_execute
             (unit, (tra_param->rx_check_wait * 1000000), &test_result));

        if (test_result == false) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

    }

exit:
    if (packet_list != NULL) {
        for (idx = 0; idx < num_pkt; idx++) {
            if (packet_list[idx] != NULL) {
                bcmpkt_free(pktdev_id, packet_list[idx]);
                packet_list[idx] = NULL;
            }
        }
        SHR_FREE(packet_list);
        packet_list = NULL;
    }
    if (expected_rate != NULL) {
        SHR_FREE(expected_rate);
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
traffic_flex_txrx_wrap_cb(int unit, void *bp, uint32_t option , int aft_cleanup )
{
    traffic_flex_param_t *tra_param = (traffic_flex_param_t *)bp;
    bcmdrd_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    BCMDRD_PBMP_ASSIGN(pbmp, tra_param->pbmp_all);
    if (tra_param->vlan_mode == VLAN_MODE_VID_BY_LPORT) {
        if (!tra_param->is_before_flex) {
            BCMDRD_PBMP_XOR(pbmp, tra_param->pbmp_unchanged);
        }
    }
    if (!tra_param->is_before_flex) {
        BCMDRD_PBMP_XOR(pbmp, tra_param->pbmp_unchanged);
    }

    if (tra_param->is_before_flex && aft_cleanup) {
        BCMDRD_PBMP_ASSIGN(pbmp, tra_param->pbmp_unchanged);
    }

    BCMDRD_PBMP_ASSIGN(tra_param->valid_pbmp, pbmp);

    if (BCMDRD_PBMP_NOT_NULL(pbmp))
        SHR_IF_ERR_EXIT(traffic_flex_txrx_cb(unit, bp, option));
exit:
    SHR_FUNC_EXIT();
}

static int
traffic_flex_set_pm_mode(int unit,
        traffic_flex_param_t *tra_param)
{
    uint32_t pm, map_len;
    const char *name = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    /* Set PM mode */
    if (tra_param->cfg_pm_mode == PM_MODE_RAND) {
        for (pm = 0; pm < TRAFFIC_FLEX_MAX_NUM_PMS; pm++) {
            tra_param->pm_mode[pm] = PM_MODE_1X400G;
        }
    } else {
        for (pm = 0; pm < TRAFFIC_FLEX_MAX_NUM_PMS; pm++) {
            tra_param->pm_mode[pm] = tra_param->cfg_pm_mode;
        }
    }

    /* Set PM mode string */
    map_len = COUNTOF(traffic_flex_pm_mode_map);
    for (pm = 0; pm < TRAFFIC_FLEX_MAX_NUM_PMS; pm++) {
        name = bcma_testutil_id2name(traffic_flex_pm_mode_map, map_len,
                tra_param->pm_mode[pm]);
        if (name !=  NULL) {
            sal_strncpy(tra_param->pm_mode_str[pm], name,
                    TRAFFIC_FLEX_MAX_STR_SIZE - 1);
        } else {
            sal_strcpy(tra_param->pm_mode_str[pm], "*");
        }

        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit, "FlexPmMode = %0d, FlexPmMode_str=%s\n"),
                 tra_param->cfg_pm_mode, tra_param->pm_mode_str[0]));
    }
    cli_out("FlexPmMode=%0d, FlexPmMode_str=%s\n",
            tra_param->cfg_pm_mode, tra_param->pm_mode_str[0]);

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_flex_do_flex(int unit, void *bp, uint32_t option)
{
    traffic_flex_param_t *tra_param = (traffic_flex_param_t *)bp;
    uint32_t lport, port_cnt = 0;
    bcmdrd_pbmp_t lgc_pbmp_down, lgc_pbmp_up;
    uint32_t last_lport = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (traffic_flex_set_pm_mode(unit, tra_param));

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_flex_do_flex
         (unit, tra_param->pbmp_all,
          tra_param->phy_pbmp_down, tra_param->phy_pbmp_up,
          TRAFFIC_FLEX_MAX_NUM_PMS, TRAFFIC_FLEX_MAX_STR_SIZE,
          (char *)&(tra_param->pm_mode_str[0][0]),
          &lgc_pbmp_down, &lgc_pbmp_up));

    traffic_flex_print_pbmp(unit, tra_param->pbmp_all, "pbmp for pbmp_all before flex");
    traffic_flex_print_pbmp(unit, tra_param->phy_pbmp_down, "pbmp for phy_pbmp_down");
    traffic_flex_print_pbmp(unit, tra_param->phy_pbmp_up, "pbmp for phy_pbmp_up");
    traffic_flex_print_pbmp(unit, lgc_pbmp_down, "pbmp for lgc_pbmp_down");
    traffic_flex_print_pbmp(unit, lgc_pbmp_up, "pbmp for lgc_pbmp_up");
    BCMDRD_PBMP_XOR(tra_param->pbmp_all, lgc_pbmp_down);
    BCMDRD_PBMP_OR(tra_param->pbmp_all, lgc_pbmp_up);
    traffic_flex_print_pbmp(unit, tra_param->pbmp_all, "pbmp for pbmp_all after flex");

    BCMDRD_PBMP_ITER(tra_param->pbmp_all, lport) {
        port_cnt++;
        last_lport = lport;
    }
    if (port_cnt % 2 == 1) {
        BCMDRD_PBMP_PORT_REMOVE(tra_param->pbmp_all, last_lport);
        port_cnt = port_cnt - 1;
    }
    tra_param->port_cnt = port_cnt;

    SHR_IF_ERR_EXIT(traffic_flex_print_port_info(unit, tra_param->pbmp_all));

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_flex_cleanup_vlan(int unit, void *bp, uint32_t option)
{
    traffic_flex_param_t *tra_param = (traffic_flex_param_t *)bp;
    bcmdrd_pbmp_t pbmp, pbmp_all, valid_pbmp;
    int port;
    uint32_t vid, pid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (tra_param->vlan_mode == VLAN_MODE_VID_BY_LPORT) {
        BCMDRD_PBMP_CLEAR(valid_pbmp);
        if (!tra_param->is_before_flex) {
            valid_pbmp = tra_param->vlan_pbmp;
        }
    }
    else
    {
        if (tra_param->is_before_flex) {
            BCMDRD_PBMP_ASSIGN(valid_pbmp, tra_param->pbmp_all);
            BCMDRD_PBMP_XOR(valid_pbmp, tra_param->pbmp_unchanged);
        }
        else
        {
            BCMDRD_PBMP_ASSIGN(valid_pbmp, tra_param->pbmp_all);
        }
    }


    BCMDRD_PBMP_ASSIGN(pbmp_all, valid_pbmp);
    BCMDRD_PBMP_ASSIGN(pbmp, valid_pbmp);
    BCMDRD_PBMP_PORT_ADD(pbmp, TRAFFIC_FLEX_CPU_PORT);

    BCMDRD_PBMP_ITER(pbmp_all, port) {
        traffic_flex_get_lport_vid_pid(unit, tra_param, port, &vid, &pid);
        cli_out("cleanup vlan: lport %0d, vid=%0d\n", port, vid);
        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_port_remove(unit, vid, pbmp));
        SHR_IF_ERR_EXIT
            (bcma_testutil_vlan_destroy(unit, vid));
    }


exit:
    SHR_FUNC_EXIT();
}

static int
traffic_flex_cleanup_cb(int unit, void *bp, uint32_t option)
{
    traffic_flex_param_t *tra_param = (traffic_flex_param_t *)bp;
    bcmdrd_pbmp_t pbmp;
    bool need_tag_action;
    /* bool after_flex_cleanup, before_flex_cleanup; */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);
    cli_out("STEP: %s\n", "cleanup");

    /* restore port setting */
    cli_out("cleanup port lb\n");
    BCMDRD_PBMP_ASSIGN(pbmp, tra_param->pbmp_all);
    if (tra_param->vlan_mode == VLAN_MODE_VID_BY_LPORT) {
        if (tra_param->is_before_flex) {
            BCMDRD_PBMP_XOR(pbmp, tra_param->pbmp_unchanged);
        } else {
            BCMDRD_PBMP_OR(pbmp, tra_param->pbmp_unchanged);
        }
    }
    else
    {
        if (tra_param->is_before_flex) {
            BCMDRD_PBMP_XOR(pbmp, tra_param->pbmp_unchanged);
        }
        else {
            BCMDRD_PBMP_OR(pbmp, tra_param->pbmp_unchanged);
        }
    }

    traffic_flex_print_pbmp(unit, pbmp, "pbmp for lb cleanup");
    SHR_IF_ERR_EXIT
        (bcma_testutil_port_clear(unit, tra_param->lb_type, pbmp, TRUE));

    if (!tra_param->is_before_flex) {
        /* cleanup vlan setting */
        cli_out("cleanup port vlan\n");
        SHR_IF_ERR_EXIT
            (traffic_flex_cleanup_vlan(unit, bp, option));
    }

    /* cleanup vlan untag action */
    if ((tra_param->vlan_mode == VLAN_MODE_VID_BY_PPORT && !tra_param->is_before_flex ) ||
            (tra_param->vlan_mode == VLAN_MODE_VID_BY_LPORT &&
             !tra_param->is_before_flex)) {
        cli_out("cleanup vlan_untag\n");
        SHR_IF_ERR_EXIT
            (bcma_testutil_drv_feature_get
             (unit, BCMA_TESTUTIL_FT_NEED_TAG_ACTION_FOR_PVID,
              &need_tag_action));

        if (need_tag_action == true) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_vlan_untag_action_remove(unit));
        }
    }

    /* cleanup l2 setting */
    cli_out("cleanup port l2 forwarding\n");
    SHR_IF_ERR_EXIT
        (traffic_flex_l2_forward_clean(unit, bp, option));

exit:
    SHR_FUNC_EXIT();
}

static int
traffic_flex_traffic_wrap(int unit, void *bp, uint32_t option)
{
    traffic_flex_param_t *tra_param = (traffic_flex_param_t *)bp;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    /* Traffic routine */
    SHR_IF_ERR_EXIT(traffic_flex_vlan_set_wrap_cb(unit, bp, option));
    SHR_IF_ERR_EXIT(traffic_flex_port_set_cb(unit, bp, option));
    SHR_IF_ERR_EXIT(traffic_flex_l2_forward_add(unit, bp, option));
    SHR_IF_ERR_EXIT(traffic_flex_txrx_wrap_cb(unit, bp, option,0));
    SHR_IF_ERR_EXIT(traffic_flex_cleanup_cb(unit, bp, option));
    if (tra_param->is_before_flex) {
        SHR_IF_ERR_EXIT(traffic_flex_txrx_wrap_cb(unit, bp, option,1));
    }

exit:

    SHR_FUNC_EXIT();
}

static int
traffic_flex_flexport_wrap(int unit, void *bp, uint32_t option)
{
    traffic_flex_param_t *tra_param = (traffic_flex_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    /* Before flex */
    if (tra_param->test_mode == TEST_MODE_TRAFFIC_BEFORE_AFTER_FLEX ||
            tra_param->test_mode == TEST_MODE_TRAFFIC_BEFORE_FLEX) {
        cli_out("[stage]: before flex\n");
        tra_param->is_before_flex = true;
        SHR_IF_ERR_EXIT(traffic_flex_traffic_wrap(unit, bp, option));
    }

    /* Do flex */
    if (tra_param->test_mode == TEST_MODE_TRAFFIC_BEFORE_AFTER_FLEX ||
            tra_param->test_mode == TEST_MODE_TRAFFIC_AFTER_FLEX) {
        cli_out("[stage]: do flex\n");
        SHR_IF_ERR_EXIT(traffic_flex_do_flex(unit, bp, option));
    }

    /* After flex */
    if (tra_param->test_mode == TEST_MODE_TRAFFIC_BEFORE_AFTER_FLEX ||
            tra_param->test_mode == TEST_MODE_TRAFFIC_AFTER_FLEX) {
        cli_out("[stage]: after flex\n");
        tra_param->is_before_flex = false;
        SHR_IF_ERR_EXIT(traffic_flex_traffic_wrap(unit, bp, option));
    }

exit:

    SHR_FUNC_EXIT();
}


static bcma_test_proc_t traffic_flex_proc[] = {
    {
        .desc = "setup cpu port\n",
        .cb = bcma_testutil_port_cpu_setup_cb,
    },
    {
        .desc = "flex port down/up, and send traffic\n",
        .cb = traffic_flex_flexport_wrap,
    }
};

static void
traffic_flex_recycle(int unit, void *bp)
{
    /* tra_param would be freed in testdb_run_teardown */
    return;
}

static bcma_test_op_t traffic_flex_op = {
    .select = traffic_flex_select,
    .parser = traffic_flex_parser,
    .help = traffic_flex_help,
    .recycle = traffic_flex_recycle,
    .procs = traffic_flex_proc,
    .proc_count = COUNTOF(traffic_flex_proc)
};

bcma_test_op_t *
bcma_testcase_traffic_flex_op_get(void)
{
    return &traffic_flex_op;
}
