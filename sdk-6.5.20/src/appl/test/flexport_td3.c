/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Flexport test is to check the correctness of flexport operation under
 * ipmc and l3uc traffic background. Ports are divided into two groups:
 * ports receiving IPMC packets and ports receiving L3UC packets.
 * Each port in IPMC group can receive L2 or L3 copies. While each port
 * in L3UC receiving L3UC packets. Test calculates expected rates based
 * on port config and oversub ratio and checks against it.
 * Configuration parameters passed from CLI:\n"
 *
 * Checks:
 * 1. Packet counts on each port.
 * 2. Packet integrity per port per replication.
 * 3. Port rate.
 *
 * Test Limitations
 * 1. FloodCnt automatically computed in the test supports packet sizes upto
 *    1518B and NumCopy=2. Any deviations may result in packet drops for LR
 *    configs or false rate errors being reported. For custom configs the user is
 *    encouraged to tweak the FloodCnt value as per their needs.
 *
 * Configuration parameters passed from CLI:
 * PktSize : Packet size in bytes not including HG2 header. Set to 0 for worst
 *           case packet sizes (145B for Ethernet and 76B for HG2). Set to 1 for
 *           random packet sizes.
 * FloodCnt: Number of packets sent from CPU to flood each stream (packet swill).
 *           Set to 0 for test to automatically calculate.
 * IpmcNumL2Ports: Number of ports receiving L2 copies in each stream (2 by default).
 * IpmcNumL3Ports:  Number of ports receiving L3 copies in each stream (2 by default).
 * IpmcNumRep: Number of same-port replications on ports receiving . Set to 0 for
 *         random.
 * MaxNumCells: Max number of cells for random packet sizes. Default = 4.
 *              Set to 0 for random.
 * RateCalcInterval: Interval in seconds over which swill is allowed to
 *          run (10s by default).
 */

#include <appl/diag/system.h>
#include <shared/alloc.h>
#include <shared/bsl.h>

#include <soc/cm.h>
#include <soc/dma.h>
#include <soc/drv.h>
#include <soc/dcb.h>
#include <soc/cmicm.h>
#include <soc/cmic.h>

#include <sal/types.h>
#include <appl/diag/parse.h>
#include <bcm/port.h>
#include <bcm/vlan.h>
#include <bcm/link.h>
#include <bcm/ipmc.h>
#include <bcm/multicast.h>

#include "testlist.h"
#include "gen_pkt.h"
#include "streaming_lib.h"

#define MAC_DA {0x01,0x00,0x5e,0x02,0x03,0x04}
#define MAC_SA {0x00,0x00,0x00,0x00,0x00,0x01}
#define VLAN 0x2
#define DROP_VLAN 0xabc
#define SIP 0x02030405
#define DIP 0xe0020304
#define FIRST_MC_GROUP 0x2000001
#define FIRST_INTF_ID 0x2
#define TTL 5

#define MAX_NUM_TSC 65
#define NUM_LANES_PER_TSC 4
#define MAX_PKTS_PER_STREAM 200

#define PKT_SIZE_PARAM_DEFAULT 0
#define FLOOD_PKT_CNT_PARAM_DEFAULT 0
#define NUM_L2_COPY_PARAM_DEFAULT 2
#define NUM_L3_COPY_PARAM_DEFAULT 6
#define NUM_REP_PARAM_DEFAULT 3
#define MAX_NUM_CELLS_PARAM_DEFAULT 4
#define RATE_CALC_INTERVAL_PARAM_DEFAULT 10
#define RATE_TOLERANCE_LR_PARAM_DEFAULT 2
#define RATE_TOLERANCE_OV_PARAM_DEFAULT 5
#define CHECK_INTEGRITY_PARAM_DEFAULT 1
#define CHECK_RATE_PARAM_DEFAULT 1
#define CHECK_MIB_PARAM_DEFAULT 0
#define MAX_L3_VLANS_PER_TEST 1000
#define MAX_NUM_IPMC_STREAMS 10
#define MAX_NUM_REP_PER_PORT 10
#define NO_SPEED_CHANGE_PARAM_DEFAULT 1
#define EMULATION_PARAM_DEFAULT 0
#define CLEAR_NEW_PORT_CNT_PARAM_DEFAULT 0
#define TEST_COVERITY_PBMP_WID 256

#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_L3) && \
           (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)) && \
           !defined(NO_SAL_APPL) && (!defined(__KERNEL__))

typedef struct flexport_mc_s {
    pbmp_t ipmc_pbmp;
    uint32 ipmc_num_streams;
    uint32 ipmc_port_list[SOC_MAX_NUM_PORTS];
    uint32 ipmc_port_list_size;
    int    ipmc_stream[MAX_NUM_IPMC_STREAMS][SOC_MAX_NUM_PORTS];
    uint32 ipmc_num_rep[SOC_MAX_NUM_PORTS];
    uint32 ipmc_port_l3_copy_en[SOC_MAX_NUM_PORTS];
    uint32 ipmc_tx_vlan[MAX_NUM_IPMC_STREAMS];
    uint32 ipmc_exp_vlan[MAX_NUM_IPMC_STREAMS];
    uint8  ipmc_exp_mac_addr[MAX_NUM_IPMC_STREAMS][NUM_BYTES_MAC_ADDR];
    uint32 ipmc_drop_vlan[MAX_NUM_IPMC_STREAMS];
    uint64 *ipmc_exp_rate;
    uint64 *ipmc_tpkt_start;
    uint64 *ipmc_tpkt_end;
    uint32 ipmc_rand_pkt_sizes[TARGET_CELL_COUNT];

    uint32 pkt_size_param;
    uint32 flood_pkt_cnt_param;
    uint32 ipmc_num_l2_ports_param;
    uint32 ipmc_num_l3_ports_param;
    uint32 check_integrity_param;
    uint32 check_rate_param;
    uint32 check_mib_param;
    uint32 ipmc_num_rep_param;
    uint32 max_num_cells_param;
    uint32 rate_calc_interval_param;
    uint32 rate_tolerance_lr_param;
    uint32 rate_tolerance_os_param;
    uint32 flex_no_spd_ch_param;
    uint32 emulation_param;
    uint32 loopback_mode_param;
    uint32 clear_new_port_cnt_param;
    uint32 num_ports_per_stream;

    uint32 bad_input;
    int test_fail;
    uint32 pkt_seed;

    uint32 debug_param;
    uint32 ipmc_port_rep_vlan[SOC_MAX_NUM_PORTS][MAX_NUM_REP_PER_PORT];
    uint32 ipmc_port_vlan_cnt[SOC_MAX_NUM_PORTS];
    uint32 ipmc_cpu_rx_vlan_cnt[MAX_L3_VLANS_PER_TEST];
    uint32 ipmc_next_vlan;
    uint32 ipmc_next_drop_vlan;
    uint32 ipmc_next_intf_id;
    uint32 ipmc_stream_mc_group[MAX_NUM_IPMC_STREAMS];

    int    env_linkscan_enable;
    uint32 env_counter_flags;
    int    env_counter_interval;
    pbmp_t env_counter_pbm;

    pbmp_t flex_lport_dn_pbmp;  /* pbmp for down-ports */
    pbmp_t flex_lport_up_pbmp;  /* pbmp for up-ports */
    pbmp_t flex_lport_ch_pbmp;  /* pbmp for speed_change_lane_unchange-ports */
    char   *flex_tsc_mode[MAX_NUM_TSC];
    uint32 flex_num_del;
    uint32 flex_num_add;
    bcm_port_resource_t flex_resource_del[SOC_MAX_NUM_PORTS];
    bcm_port_resource_t flex_resource_add[SOC_MAX_NUM_PORTS];

    pbmp_t l3uc_pbmp;
    pbmp_t l3uc_vlan_pbmp;

    /* chip dependent test parameters. */
    int    test_lport_lo_ipmc, test_lport_hi_ipmc;
    int    test_lport_lo_l3uc, test_lport_hi_l3uc;
} flexport_mc_t;


typedef struct flexport_l3uc_rate_calc_t {
    uint32 ovs_ratio_x1000[SOC_MAX_NUM_PIPES];
    uint32 rand_pkt_sizes[SOC_MAX_NUM_PORTS][TARGET_CELL_COUNT];
    uint64 stime[SOC_MAX_NUM_PORTS];
    uint64 exp_rate[SOC_MAX_NUM_PORTS];
    uint64 tpkt_start[SOC_MAX_NUM_PORTS];
    uint64 tbyt_start[SOC_MAX_NUM_PORTS];
} flexport_l3uc_rate_calc_t;


static flexport_mc_t *flexport_parray[SOC_MAX_NUM_DEVICES];
static flexport_l3uc_rate_calc_t *rate_calc_parray[SOC_MAX_NUM_DEVICES];


const char flexport_mc_test_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nDocumentation too long to be displayed with -pedantic compiler\n";
#else
    "Flexport test is to check the correctness of flexport operation under\n"
    "ipmc and l3uc traffic background. Ports are divided into two groups:\n"
    "ports receiving IPMC packets and ports receiving L3UC packets. \n"
    "Each port in IPMC group can receive L2 or L3 copies. While each port\n"
    "in L3UC receiving L3UC packets. Test calculates expected rates based\n"
    "on port config and oversub ratio and checks against it.\n"
    "Configuration parameters passed from CLI:\n"
    "PktSize: Packet size in bytes. Set it to 0 for worst case packet sizes.\n"
    "         Set it to 1 for random packet sizes.\n"
    "FloodCnt: Number of packets in each swill. Set it to 0 will let the\n"
    "          test calculate the number of packets that can be sent to\n"
    "          achieve a lossless swirl at full rate. Set to 0 by default.\n"
    " IpmcNumL2Ports: Number of ports receiving L2 copies (2 by default).\n"
    " IpmcNumL3Ports: Number of ports receiving L3 copies (2 by default).\n"
    " IpmcNumRep: Number of replications per port. Set to 0 for random.\n"
    " MaxNumCells: Max number of cells for random packet sizes. Default = 4.\n"
    "              Set to 0 for random.\n"
    " RateCalcInterval: Interval in seconds over which swill is allowed to\n"
    "                   run (10s by default).\n";
#endif

const char flexport_mc_td3_supported_mode[] =
    "              1x100G, 1x40G\n"
    "              2x50G, 2x40G, 2x20G,\n"
    "              20G+2x10G, 2x10G+20G,\n"
    "              50G+2x25G, 2x25G+50G,\n"
    "              4x25G, 4x10G and 4x1G\n"
    "              The supported mode/speed in HiGig2 encapsulation are:\n"
    "              1x106G, 1x42G,\n"
    "              2x53G, 2x42G, 2x21G,\n"
    "              4x27G, and 4x11G\n"
    ;



/*
 * Function:
 *      flexport_print_ports
 * Purpose:
 *      Print ports in port bmp.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pbmp: port bitmap.
 *      str : string ptr for print title.
 *
 * Returns:
 *     Nothing.
 */
static void
flexport_print_ports(int unit, pbmp_t pbmp, char *str)
{
    int p, cnt = 0;
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\n%s"), str));
    PBMP_ITER(pbmp, p) {
        if (cnt++ % 10 == 0) {
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\n")));
        }
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "%3d "), p));
    }
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\n")));
}

/*
 * Function:
 *      flexport_set_chip_dependent_params
 * Purpose:
 *      Set chip dependent parameters.
 * Parameters:
 *
 * Returns:
 *     Nothing
 */
static void
flexport_set_chip_dependent_params(int unit)
{
    flexport_mc_t *flex_p = flexport_parray[unit];

    /* MAVERICK2 */
    if (SOC_IS_MAVERICK2(unit)) {
        flex_p->test_lport_lo_ipmc = 1;
        flex_p->test_lport_hi_ipmc = 32;
        flex_p->test_lport_lo_l3uc = 41;
        flex_p->test_lport_hi_l3uc = 64;
    }
    /* TRIDENT3 */
    else if (SOC_IS_TRIDENT3(unit)) {
        flex_p->test_lport_lo_ipmc = 1;
        flex_p->test_lport_hi_ipmc = 32;
        flex_p->test_lport_lo_l3uc = 64;
        flex_p->test_lport_hi_l3uc = 128;
    }
    else {
        flex_p->test_lport_lo_ipmc = 1;
        flex_p->test_lport_hi_ipmc = 32;
        flex_p->test_lport_lo_l3uc = 64;
        flex_p->test_lport_hi_l3uc = 128;
    }
}

/*
 * Function:
 *      flexport_is_ipmc_port
 * Purpose:
 *      Check if physial port is belonging to ipmc group.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      phy_port:  physical port number.
 *
 * Returns:
 *     Return true if port is belonging to impc port_range,
 *     otherwise return false.
 */
static int
flexport_is_ipmc_port(int unit, int phy_port)
{
    flexport_mc_t *flex_p = flexport_parray[unit];
    return (phy_port >= flex_p->test_lport_lo_ipmc &&
            phy_port <= flex_p->test_lport_hi_ipmc);
}

/*
 * Function:
 *      flexport_is_l3uc_port
 * Purpose:
 *      Check if physial port is belonging to l3uc group.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      phy_port:  physical port number.
 *
 * Returns:
 *     Return true if port is belonging to l3uc port_range,
 *     otherwise return false.
 */
static int
flexport_is_l3uc_port(int unit, int phy_port)
{
    flexport_mc_t *flex_p = flexport_parray[unit];
    return (phy_port >= flex_p->test_lport_lo_l3uc &&
            phy_port <= flex_p->test_lport_hi_l3uc);
}


/*
 * Function:
 *      flexport_parse_test_params
 * Purpose:
 *      Parse CLI parameters, create test structure and flag bad inputs.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     Nothing
 * Notes:
 *      flex_p->bad_input set from here - tells test to crash out in case
 *      CLI input combination is invalid.
 */
static void
flexport_parse_test_params(int unit, args_t *a)
{
    parse_table_t parse_table;
    flexport_mc_t *flex_p = flexport_parray[unit];

    flex_p->bad_input = 0;

    flex_p->pkt_size_param          = PKT_SIZE_PARAM_DEFAULT;
    flex_p->flood_pkt_cnt_param     = FLOOD_PKT_CNT_PARAM_DEFAULT;
    flex_p->max_num_cells_param     = MAX_NUM_CELLS_PARAM_DEFAULT;
    flex_p->ipmc_num_l2_ports_param = NUM_L2_COPY_PARAM_DEFAULT;
    flex_p->ipmc_num_l3_ports_param = NUM_L3_COPY_PARAM_DEFAULT;
    flex_p->ipmc_num_rep_param      = NUM_REP_PARAM_DEFAULT;
    flex_p->rate_calc_interval_param= RATE_CALC_INTERVAL_PARAM_DEFAULT;
    flex_p->rate_tolerance_lr_param = RATE_TOLERANCE_LR_PARAM_DEFAULT;
    flex_p->rate_tolerance_os_param = RATE_TOLERANCE_OV_PARAM_DEFAULT;
    flex_p->check_integrity_param   = CHECK_INTEGRITY_PARAM_DEFAULT;
    flex_p->check_rate_param        = CHECK_RATE_PARAM_DEFAULT;
    flex_p->check_mib_param         = CHECK_MIB_PARAM_DEFAULT;
    flex_p->flex_no_spd_ch_param    = NO_SPEED_CHANGE_PARAM_DEFAULT;
    flex_p->emulation_param         = EMULATION_PARAM_DEFAULT;
    flex_p->loopback_mode_param     = BCM_PORT_LOOPBACK_MAC;
    flex_p->clear_new_port_cnt_param= CLEAR_NEW_PORT_CNT_PARAM_DEFAULT;
    flex_p->debug_param             = 0;

    /*Parse CLI opts */

    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table, "PktSize", PQ_INT|PQ_DFL, 0,
                    &(flex_p->pkt_size_param), NULL);
    parse_table_add(&parse_table, "FloodCnt", PQ_INT|PQ_DFL, 0,
                    &(flex_p->flood_pkt_cnt_param), NULL);
    parse_table_add(&parse_table, "IpmcNumL2Ports", PQ_INT|PQ_DFL, 0,
                    &(flex_p->ipmc_num_l2_ports_param), NULL);
    parse_table_add(&parse_table, "IpmcNumL3Ports", PQ_INT|PQ_DFL, 0,
                    &(flex_p->ipmc_num_l3_ports_param), NULL);
    parse_table_add(&parse_table, "IpmcNumRep", PQ_INT|PQ_DFL, 0,
                    &(flex_p->ipmc_num_rep_param), NULL);
    parse_table_add(&parse_table, "RateToleranceLR", PQ_INT|PQ_DFL, 0,
                    &(flex_p->rate_tolerance_lr_param), NULL);
    parse_table_add(&parse_table, "RateToleranceOS", PQ_INT|PQ_DFL, 0,
                    &(flex_p->rate_tolerance_os_param), NULL);
    parse_table_add(&parse_table, "RateCalcInterval", PQ_INT|PQ_DFL, 0,
                    &(flex_p->rate_calc_interval_param), NULL);
    parse_table_add(&parse_table, "CheckIntegrity", PQ_INT|PQ_DFL, 0,
                    &(flex_p->check_integrity_param), NULL);
    parse_table_add(&parse_table, "CheckRate", PQ_INT|PQ_DFL, 0,
                    &(flex_p->check_rate_param), NULL);
    parse_table_add(&parse_table, "CheckMib", PQ_INT|PQ_DFL, 0,
                    &(flex_p->check_mib_param), NULL);
    parse_table_add(&parse_table, "ClrNewPortCnt", PQ_INT|PQ_DFL, 0,
                    &(flex_p->clear_new_port_cnt_param), NULL);
    parse_table_add(&parse_table, "DebugParam", PQ_INT|PQ_DFL, 0,
                    &(flex_p->debug_param), NULL);

    if (parse_arg_eq(a, &parse_table) < 0 || ARG_CNT(a) != 0) {
        test_msg("%s", flexport_mc_test_usage);
        cli_out("\n%s\n", flexport_mc_td3_supported_mode);
        test_error(unit, "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        flex_p->bad_input = 1;
        parse_arg_eq_done(&parse_table);
    } else {
        cli_out("\n");
        cli_out("------------- PRINTING TEST PARAMS ------------------\n");
        cli_out("PktSize          = %0d\n", flex_p->pkt_size_param);
        cli_out("FloodCnt         = %0d\n", flex_p->flood_pkt_cnt_param);
        cli_out("IpmcNumL3Ports   = %0d\n", flex_p->ipmc_num_l3_ports_param);
        cli_out("IpmcNumRep       = %0d\n", flex_p->ipmc_num_rep_param);
        cli_out("MaxNumCells      = %0d\n", flex_p->max_num_cells_param);
        cli_out("RateToleranceLR  = %0d\n", flex_p->rate_tolerance_lr_param);
        cli_out("RateToleranceOS  = %0d\n", flex_p->rate_tolerance_os_param);
        cli_out("RateCalcInterval = %0d\n", flex_p->rate_calc_interval_param);
        cli_out("CheckIntegrity   = %0d\n", flex_p->check_integrity_param);
        cli_out("CheckRate        = %0d\n", flex_p->check_rate_param);
        cli_out("FlexNoSpeedChange= %0d\n", flex_p->flex_no_spd_ch_param);
        cli_out("ClrNewPortCnt    = %0d\n", flex_p->clear_new_port_cnt_param);
        cli_out("DebugParam       = %0d\n", flex_p->debug_param);
        cli_out("-----------------------------------------------------\n");
    }

    /* loopback_mode */
    if ((flex_p->loopback_mode_param != BCM_PORT_LOOPBACK_MAC) &&
        (flex_p->loopback_mode_param != BCM_PORT_LOOPBACK_PHY) &&
        (flex_p->loopback_mode_param != BCM_PORT_LOOPBACK_PHY_REMOTE) &&
        (flex_p->loopback_mode_param != BCM_PORT_LOOPBACK_NONE)) {
        /*
         *   0 - no loopback; only to be used with chip external loopbacks.
         *   1 - MAC outer loopback.
         *   2 - PCS local loopback, close to PMD boundary.
         *   3 - PMD local loopback, close to AFE.
         */
        test_error(unit,"*ERROR: Loopback mode must be either 0, 1, 2 or 3\n");
        flex_p->bad_input = 1;
    }

    if (flex_p->max_num_cells_param == 0) {
        /* coverity[dont_call : FALSE] */
        flex_p->max_num_cells_param = (sal_rand() % (MTU_CELL_CNT - 1)) + 1;
    }

    if (flex_p->pkt_size_param == 0) {
        flex_p->pkt_size_param = 256;
        cli_out("Using default packet sizes - %0dB", flex_p->pkt_size_param);
    } else if (flex_p->pkt_size_param == 1) {
        cli_out("Using random packet sizes\n");
    } else if (flex_p->pkt_size_param < MIN_PKT_SIZE) {
        test_error(unit,"\n*ERROR: Packet size cannot be lower than %0dB\n",
                   MIN_PKT_SIZE);
        flex_p->bad_input = 1;
    } else if (flex_p->pkt_size_param > MTU) {
        test_error(unit, "\n%s higher than %0dB (Device MTU)\n",
                   "*ERROR: Packet size cannot be ", MTU);
        flex_p->bad_input = 1;
    }

    if (flex_p->flood_pkt_cnt_param == 0) {
        cli_out("FloodCnt=0, test will automatically calculate number of"
                " packets to flood each port\n");
    }

    if (flex_p->ipmc_num_rep_param > MAX_NUM_REP_PER_PORT) {
        flex_p->ipmc_num_rep_param = MAX_NUM_REP_PER_PORT;
        cli_out("Max replications supported by this test = %0d,"
                "forcing number of replications to %0d\n",
                MAX_NUM_REP_PER_PORT, MAX_NUM_REP_PER_PORT);
    }

    /* total number of ports in each ipmc group */
    flex_p->num_ports_per_stream = flex_p->ipmc_num_l2_ports_param +
                                   flex_p->ipmc_num_l3_ports_param;
    cli_out("initial NumPortsIpmc = %0d,\n", flex_p->num_ports_per_stream);
}

/*
 * Function:
 *      flexport_ipmc_set_port_property
 * Purpose:
 *      Set port_list, stream, port_l3_copy_en, rand_pkt_size.
 *      Also call set_exp_rates.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing.
 */
static void
flexport_ipmc_set_port_property(int unit)
{
    int i, j, p, port_idx, pkt_size;
    flexport_mc_t *flex_p = flexport_parray[unit];

    cli_out("\n[ipmc] set_port_property()\n");

    flex_p->ipmc_num_streams = 0;
    /* coverity[dont_call : FALSE] */
    flex_p->pkt_seed = sal_rand();

    flex_p->ipmc_port_list_size = 0;
    PBMP_ITER(flex_p->ipmc_pbmp, p) {
        flex_p->ipmc_port_list[flex_p->ipmc_port_list_size++] = p;
    }

    /* assign ports for stream 0 by picking up the first n ports */
    i = 0;
    for (j = 0; j < flex_p->num_ports_per_stream; j++) {
        port_idx = j; /* index of port in ipmc_port_list */
        flex_p->ipmc_stream[i][j] = port_idx;
    }
    flex_p->ipmc_num_streams = 1;

    /* port_l3_copy_en[]: all ports except P0 and P1 receive L3 copies.
     * P0 and P1 are used for generating traffic.
     */
    for (i = 0; i < flex_p->ipmc_port_list_size; i++) {
        p = flex_p->ipmc_port_list[i];
        flex_p->ipmc_port_l3_copy_en[p] = i < 2 ? 0 : 1;
    }

    /* num_rep[]: number of L3 replications of each port */
    for (i = 0; i < flex_p->ipmc_port_list_size; i++) {
        p = flex_p->ipmc_port_list[i];
        flex_p->ipmc_num_rep[p] = 0;
        if (flex_p->ipmc_port_l3_copy_en[p] == 1) {
            if (flex_p->ipmc_num_rep_param == 0) {
                /* coverity[dont_call : FALSE] */
                flex_p->ipmc_num_rep[p] = sal_rand() % (MAX_NUM_REP_PER_PORT + 1);
            } else {
                flex_p->ipmc_num_rep[p] = flex_p->ipmc_num_rep_param;
            }
        }
    }

    /* rand_pkt_sizes[][] */
    p = flex_p->ipmc_port_list[0];
    for (j = 0; j < TARGET_CELL_COUNT; j++) {
        do {
            /* coverity[dont_call : FALSE] */
            pkt_size = (sal_rand() % (MTU - MIN_PKT_SIZE + 1)) + MIN_PKT_SIZE;
        } while (stream_get_pkt_cell_cnt(unit, pkt_size, p) >
                                         flex_p->max_num_cells_param);
        flex_p->ipmc_rand_pkt_sizes[j] = pkt_size;
    }

    {
        /* print */
        int port_idx;
        cli_out("\n=========== IPMC STREAM INFO ============\n");
        cli_out("\t[%s]%4s %s/%s\n", "idx", "port", "type", "num_cpy");

        for (i = 0; i < flex_p->ipmc_num_streams; i++) {
            cli_out("%4d ", i);
            for (j = 0; j < flex_p->num_ports_per_stream; j++) {
                port_idx = flex_p->ipmc_stream[i][j];
                p = flex_p->ipmc_port_list[port_idx];
                cli_out("\t[%d]%4d ", j, p);
                if (flex_p->ipmc_port_l3_copy_en[p] == 0) {
                    cli_out("%s/%d", "L2", 0);
                } else {
                    cli_out("%s/%d", "L3", (flex_p->ipmc_num_rep[p] + 1));
                }
            }
            cli_out("\n");
        }
        cli_out("\nipmc_num_ports/ipmc_num_avail_ports = %0d / %0d\n",
                flex_p->num_ports_per_stream, flex_p->ipmc_port_list_size);
    }
}

/*
 * Function:
 *      flexport_ipmc_add_l3_intf
 * Purpose:
 *      This function actually sets up the IPMC packet flow. Please refer to
 *      items 4-6 of the "Test Setup" section of the test description at the
 *      beginning of this file.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      port: physical port.
 *      mc_group: multicast group id.
 *      dst_vlan: used for vlan translation.
 *
 * Returns:
 *     Nothing.
 */
static void
flexport_ipmc_add_l3_intf(int unit, int port, bcm_multicast_t mc_group,
                 bcm_vlan_t dst_vlan)
{
    pbmp_t pbm, ubm;
    bcm_vlan_t vlan;
    bcm_l3_intf_t l3_intf;
    bcm_if_t intf_id;
    bcm_if_t encap_id;
    uint32 intf_flags = 0;
    uint8  intf_mac[] = {0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_gport_t gport;
    uint32 port_vlan_cnt;
    flexport_mc_t *flex_p = flexport_parray[unit];

    /* init */
    intf_flags |= BCM_L3_WITH_ID;
    BCM_PBMP_CLEAR(ubm);

    /* create vlan */
    vlan = (bcm_vlan_t) flex_p->ipmc_next_vlan++;
    bcm_vlan_create(unit, vlan);
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_PORT_ADD(pbm, port);
    bcm_vlan_port_add(unit, vlan, pbm, ubm);

    /* create vlan_translate */
    bcm_vlan_translate_add(unit, port, vlan, dst_vlan, 0);

    /* update test variable */
    port_vlan_cnt = flex_p->ipmc_port_vlan_cnt[port]++;
    flex_p->ipmc_port_rep_vlan[port][port_vlan_cnt] = (uint32) vlan;

    /* create l3_intf */
    bcm_l3_intf_t_init(&l3_intf);
    intf_id = (bcm_if_t) flex_p->ipmc_next_intf_id++;
    intf_mac[NUM_BYTES_MAC_ADDR - 1] = intf_id;
    intf_mac[NUM_BYTES_MAC_ADDR - 2] = intf_id >> 8;
    sal_memcpy(l3_intf.l3a_mac_addr, intf_mac, 6);
    l3_intf.l3a_vid = vlan;
    l3_intf.l3a_intf_id = intf_id;
    l3_intf.l3a_flags = intf_flags;
    bcm_l3_intf_create(unit, &l3_intf);

    /* add l3_intf into mc_group */
    bcm_multicast_l3_encap_get(unit, mc_group, port, intf_id, &encap_id);
    BCM_GPORT_LOCAL_SET(gport, port);
    bcm_multicast_egress_add(unit, mc_group, gport, encap_id);
}

/*
 * Function:
 *      flexport_ipmc_print_vlan_info
 * Purpose:
 *      Print IPMC vlan info.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing.
 */
static void
flexport_ipmc_print_vlan_info(int unit)
{
    int i, j, port_idx;
    int port, cnt, vlan_cnt;
    flexport_mc_t *flex_p = flexport_parray[unit];

    cli_out("\n=========== port vlan info ============\n");
    cli_out("%4s %4s %4s %4s %4s %4s \t%4s\n",
            "strm", "idx", "tsc", "port", "type", "vlan_cnt", "vlan_id");

    for (i = 0; i < flex_p->ipmc_num_streams; i++) {
        for (j = 0; j < flex_p->num_ports_per_stream; j++) {
            port_idx = flex_p->ipmc_stream[i][j];
            port     = flex_p->ipmc_port_list[port_idx];
            if (flex_p->ipmc_port_l3_copy_en[port] == -1) continue;
            cli_out("%4d ", i);
            cli_out("%4d %4d %4d ", j, (port-1)/4, port);
            if (flex_p->ipmc_port_l3_copy_en[port] == 0) {
                cli_out("%4s ", "L2");
            } else {
                cli_out("%4s ", "L3");
            }
            vlan_cnt = flex_p->ipmc_port_vlan_cnt[port];
            cli_out("%4d \t", vlan_cnt);
            for (cnt = 0; cnt < vlan_cnt; cnt++) {
                cli_out("%4d  ", flex_p->ipmc_port_rep_vlan[port][cnt]);
            }
            cli_out("\n");
        }
        cli_out("\n");
    }
}

/*
 * Function:
 *      flexport_ipmc_set_up_streams
 * Purpose:
 *      This function actually sets up the IPMC packet flow. Please refer to
 *      items 4-6 of the "Test Setup" section of the test description at the
 *      beginning of this file.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing.
 */
static void
flexport_ipmc_set_up_streams(int unit)
{
    int i, j, k;
    pbmp_t pbm, ubm;
    bcm_vlan_t vlan, vlan_0 = 0, vlan_1 = 0, dst_vlan, drop_vlan;
    /* uint32 vlan, vlan_0 = 0, vlan_1 = 0; */
    bcm_multicast_t mc_group;
    uint32 mc_flags = 0;
    bcm_if_t intf_id;
    bcm_ipmc_addr_t ipmc_addr;
    uint32 ipmc_flags = 0;
    uint8 mac_sa[] = MAC_SA;
    int port;
    bcm_gport_t gport;
    uint32 port_vlan_cnt;
    flexport_mc_t *flex_p = flexport_parray[unit];

    /* init vlan setup */
    bcm_vlan_destroy_all(unit);
    bcm_vlan_control_set(unit, bcmVlanTranslate, TRUE);
    BCM_PBMP_CLEAR(ubm);
    flex_p->ipmc_next_vlan = VLAN + SOC_MAX_NUM_PORTS + 10;
    flex_p->ipmc_next_drop_vlan = DROP_VLAN;

    (void) bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE);
    PBMP_ITER(flex_p->ipmc_pbmp, port) {
        (void) bcm_port_control_set(unit, port, bcmPortControlIP4, TRUE);
        (void) bcm_port_control_set(unit, port, bcmPortControlIP6, TRUE);
        (void) bcm_port_control_set(unit, port, bcmSwitchL3McIdxRetType, TRUE);
        (void) bcm_port_control_set(unit, port,
                                    bcmSwitchIpmcReplicationSharing, TRUE);
    }

    bcm_ipmc_enable(unit, TRUE);
    mc_group = (bcm_multicast_t)(FIRST_MC_GROUP);
    mc_flags = BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID;
    intf_id  = (bcm_if_t)(FIRST_INTF_ID);
    flex_p->ipmc_next_intf_id = (uint32) intf_id;
    ipmc_flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;

    for (i = 0; i < flex_p->ipmc_num_streams; i++) {
        (void) bcm_multicast_create(unit, mc_flags, &mc_group);
        flex_p->ipmc_stream_mc_group[i] = mc_group;

        /* create drop_vlan */
        drop_vlan = (bcm_vlan_t) flex_p->ipmc_next_drop_vlan++;
        bcm_vlan_create(unit, drop_vlan);
        flex_p->ipmc_drop_vlan[i] = (uint32) drop_vlan;
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                    "\nstream = %0d, mc_group = 0x%0x, drop_vlan = %0d\n"),
                    i, mc_group, (uint32) drop_vlan));

        /* vlan_0 for P0 */
        vlan = (bcm_vlan_t) flex_p->ipmc_next_vlan++;
        bcm_vlan_create(unit, vlan);
        BCM_PBMP_CLEAR(pbm);
        port = flex_p->ipmc_port_list[flex_p->ipmc_stream[i][0]];
        BCM_PBMP_PORT_ADD(pbm, port);
        bcm_vlan_port_add(unit, vlan, pbm, ubm);
        vlan_0 = vlan;

        port_vlan_cnt = flex_p->ipmc_port_vlan_cnt[port]++;
        flex_p->ipmc_port_rep_vlan[port][port_vlan_cnt] = (uint32) vlan;

        /* vlan_1 for ports receiving l2 copies, P0,P1,...,Pn */
        vlan = (bcm_vlan_t) flex_p->ipmc_next_vlan++;
        bcm_vlan_create(unit, vlan);
        BCM_PBMP_CLEAR(pbm);
        for (j = 0; j < flex_p->num_ports_per_stream; j++) {
            port = flex_p->ipmc_port_list[flex_p->ipmc_stream[i][j]];
            if (flex_p->ipmc_port_l3_copy_en[port] == 0) {
                BCM_PBMP_PORT_ADD(pbm, port);

                port_vlan_cnt = flex_p->ipmc_port_vlan_cnt[port]++;
                flex_p->ipmc_port_rep_vlan[port][port_vlan_cnt] = (uint32) vlan;

                BCM_GPORT_LOCAL_SET(gport, port);
                bcm_multicast_egress_add(unit, mc_group, gport,(bcm_if_t)(-1));
            }
        }
        bcm_vlan_port_add(unit, vlan, pbm, ubm);
        vlan_1 = vlan;

        /* vlan_xlate: P0: vlan_0 -> vlan_1 */
        port = flex_p->ipmc_port_list[flex_p->ipmc_stream[i][0]];
        bcm_vlan_translate_add(unit, port, vlan_0, vlan_1, 0);

        for (j = 1; j < flex_p->num_ports_per_stream; j++) {
            port = flex_p->ipmc_port_list[flex_p->ipmc_stream[i][j]];
            if (flex_p->ipmc_port_l3_copy_en[port] == 0) {
                dst_vlan = (j == 1 ) ? vlan_0 : drop_vlan;
                bcm_vlan_translate_add(unit, port, vlan_1, dst_vlan, 0);
            }
        }

        /* [L3 copy] */
        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            if (flexport_is_ipmc_port(unit, port)) {
                if (port == flex_p->ipmc_port_list[flex_p->ipmc_stream[i][0]] ||
                    port == flex_p->ipmc_port_list[flex_p->ipmc_stream[i][1]])
                    continue;
                for (k = 0; k < (flex_p->ipmc_num_rep[port] + 1); k++) {
                    flexport_ipmc_add_l3_intf(unit, port, mc_group, drop_vlan);
                }
            }
        }

        bcm_ipmc_addr_t_init(&ipmc_addr);
        ipmc_addr.mc_ip_addr = DIP;
        ipmc_addr.s_ip_addr = SIP;
        ipmc_addr.vid = vlan_1;
        ipmc_addr.group = mc_group;
        ipmc_addr.flags = ipmc_flags;
        bcm_ipmc_add(unit, &ipmc_addr);
        mc_group++;

        flex_p->ipmc_tx_vlan[i]  = (uint32) vlan_0;
        flex_p->ipmc_exp_vlan[i] = (uint32) vlan_1;
        sal_memcpy(flex_p->ipmc_exp_mac_addr[i], mac_sa, 6);

        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                    "\nstream=%0d, tx_vlan = %0d, rx_vlan = %0d\n"),
                    i, flex_p->ipmc_tx_vlan[i], flex_p->ipmc_exp_vlan[i]));
    }

    /* print vlan info */
    flexport_ipmc_print_vlan_info(unit);
}

/*
 * Function:
 *     flexport_get_port_pkt_size
 * Purpose:
 *     Get the packet size of given port.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      port - Device port number
 *
 * Returns:
 *      Port packet size.
 */
static uint32
flexport_get_port_pkt_size(int unit, int port)
{
    uint32 pkt_size;
    flexport_mc_t *flex_p = flexport_parray[unit];

    if (flex_p->pkt_size_param == 0) {
        pkt_size = stream_get_wc_pkt_size(unit, IS_HG_PORT(unit, port));
    } else {
        pkt_size = flex_p->pkt_size_param;
    }

    return (pkt_size);
}

/*
 * Function:
 *     flexport_get_lossless_flood_cnt
 * Purpose:
 *     Calculates no of packets to be set for the given port
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      port - Device port number
 *
 * Returns:
 *     flood packet count for the given port.
 */
static uint32
flexport_get_lossless_flood_cnt(int unit, int port)
{
    uint32 pkt_size, flood_cnt = 0;
    flexport_mc_t *flex_p = flexport_parray[unit];

    if (flex_p->flood_pkt_cnt_param == 0) {
        pkt_size = flexport_get_port_pkt_size(unit, port);
        flood_cnt = stream_get_ll_flood_cnt(unit, port, pkt_size, NULL);
    } else {
        flood_cnt = flex_p->flood_pkt_cnt_param;
    }

    if (flood_cnt > MAX_PKTS_PER_STREAM) {
        flood_cnt = MAX_PKTS_PER_STREAM;
    }

    return (flood_cnt);
}

/*
 * Function:
 *      flexport_ipmc_send_pkts
 * Purpose:
 *      Send packets from CPU to create a swirl on each stream. Please refer
 *      to item 6 of the "Test Setup" portion of the test description at the
 *      beginning of this file.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
static void
flexport_ipmc_send_pkts(int unit)
{
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    uint32 pkt_size = 0, pkt_count = 0, flood_cnt = 0;
    int i, port;
    uint32 use_random_packet_sizes = 0;
    flexport_mc_t *flex_p = flexport_parray[unit];
    stream_pkt_t *tx_pkt;

    tx_pkt = sal_alloc(sizeof(stream_pkt_t), "tx_pkt");
    sal_memset(tx_pkt, 0, sizeof(stream_pkt_t));

    if (flex_p->pkt_size_param == 1) {
        use_random_packet_sizes = 1;
    }

    cli_out("\n==================================================\n");
    cli_out("\nSending packets ...\n\n");
    for (i = 0; i < flex_p->ipmc_num_streams; i++) {
        port = flex_p->ipmc_port_list[flex_p->ipmc_stream[i][0]];
        flood_cnt = flexport_get_lossless_flood_cnt(unit, port);
        pkt_size  = (flex_p->pkt_size_param == 0) ?
                    ENET_WC_PKT_SIZE : flex_p->pkt_size_param;

        tx_pkt->l3_en    = 1;
        tx_pkt->ipv6_en  = 0;
        tx_pkt->ip_da    = DIP;
        tx_pkt->ip_sa    = SIP;
        tx_pkt->ttl      = TTL;
        tx_pkt->port     = port;
        tx_pkt->num_pkt  = flood_cnt;
        tx_pkt->pkt_seed = flex_p->pkt_seed + i;
        tx_pkt->pkt_size = pkt_size;
        tx_pkt->tx_vlan  = flex_p->ipmc_tx_vlan[i];
        sal_memcpy(tx_pkt->mac_da, mac_da, NUM_BYTES_MAC_ADDR);
        sal_memcpy(tx_pkt->mac_sa, mac_sa, NUM_BYTES_MAC_ADDR);
        tx_pkt->rand_pkt_size_en = use_random_packet_sizes;
        tx_pkt->rand_pkt_size    = NULL;
        if (tx_pkt->rand_pkt_size_en) {
            tx_pkt->rand_pkt_size = flex_p->ipmc_rand_pkt_sizes;
        }

        stream_tx_pkt(unit, tx_pkt);

        pkt_count += tx_pkt->cnt_pkt;
        cli_out("flood_cnt for stream %0d = %0d\n", i, tx_pkt->cnt_pkt);
    }

    cli_out("\nTotal packet sent: %0d\n\n", pkt_count);
    sal_free(tx_pkt);
}

/*
 * Function:
 *      flexport_ipmc_chk_rate
 * Purpose:
 *      Check actual port rates against expected port rates.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     SOC_E_XXXX
 */
static bcm_error_t
flexport_ipmc_chk_rate(int unit)
{
    int i, j, port, port_idx;
    bcm_error_t rv = BCM_E_NONE;
    stream_rate_t *rate_calc;
    flexport_mc_t *flex_p = flexport_parray[unit];

    /* check mib counters */
    if (flex_p->check_mib_param == 1) {
        if (stream_chk_mib_counters(unit, flex_p->ipmc_pbmp, 0) != BCM_E_NONE) {
            flex_p->test_fail = 1;
        }
    }

    /* check rate */
    if(!flex_p->check_rate_param) return rv;
    rate_calc = (stream_rate_t *)
                 sal_alloc(sizeof(stream_rate_t), "rate_calc");
    if (rate_calc == NULL) {
        test_error(unit, "Failed to allocate memory for rate_calc\n");
        return BCM_E_FAIL;
    }
    sal_memset(rate_calc, 0, sizeof(stream_rate_t));

    rate_calc->mode         = 1;
    rate_calc->pkt_size     = flex_p->pkt_size_param;
    rate_calc->interval_len = flex_p->rate_calc_interval_param;
    rate_calc->tolerance_lr = flex_p->rate_tolerance_lr_param;
    rate_calc->tolerance_os = flex_p->rate_tolerance_os_param;

    SOC_PBMP_CLEAR(rate_calc->pbmp);
    for (i = 0; i < flex_p->ipmc_num_streams; i++) {
        for (j = 0; j < flex_p->num_ports_per_stream; j++) {
            port_idx = flex_p->ipmc_stream[i][j];
            port = flex_p->ipmc_port_list[port_idx];
            if (port < SOC_MAX_NUM_PORTS && port < TEST_COVERITY_PBMP_WID) {
                if (flex_p->ipmc_port_l3_copy_en[port] == -1) continue;
                /* pbmp */
                SOC_PBMP_PORT_ADD((rate_calc->pbmp), port);
                /* num_l3_rep */
                if (flex_p->ipmc_port_l3_copy_en[port] == 0) {
                    rate_calc->num_rep[port] = 0;
                } else {
                    rate_calc->num_rep[port] = flex_p->ipmc_num_rep[port] + 1;
                }
                /* src_port */
                rate_calc->src_port[port] =
                        flex_p->ipmc_port_list[flex_p->ipmc_stream[i][0]];
            }
        }
    }

    rv = stream_chk_port_rate(unit, flex_p->ipmc_pbmp, rate_calc);
    if (rv != BCM_E_NONE) flex_p->test_fail = 1;

    sal_free(rate_calc);
    return rv;
}

/*
 * Function:
 *      flexport_ipmc_chk_integrity_l3
 * Purpose:
 *      Redirect all packets back to CPU and check packet integrity
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pkt_intg - ptr to packet integrity struct, containing all necessary
 *                 parameters needed for integrity check.
 *
 * Returns:
 *     SOC_E_XXXX
 */
static bcm_error_t
flexport_ipmc_chk_integrity_l3(int unit, stream_integrity_t *pkt_intg)
{
    bcm_error_t rv = BCM_E_NONE;
    uint8 *rx_pkt, *ref_pkt;
    int i, j, k, port, port_speed, chk_fail = 0, stream_cnt = 0;
    int rst_vlan;
    dv_t *dv_rx;
    uint32 vlan;
    uint32 pkt_size;
    uint32 port_exp_pkt_cnt, port_act_pkt_cnt, pkt_cnt_sum = 0;
    uint32 header_size = 0;
    uint32 rx_vlan;
    flexport_mc_t *flex_p = flexport_parray[unit];

    cli_out("\n==================================================\n");
    cli_out("IPMC Checking Packet Integrity ...\n\n");
    /* initialize */
    soc_dma_header_size_get(unit, &header_size);

    dv_rx   = soc_dma_dv_alloc(unit, DV_RX, 3);
    ref_pkt = sal_dma_alloc(MTU * sizeof(uint8), "ref_packet");
    rx_pkt  = sal_dma_alloc(MTU * sizeof(uint8) + header_size, "rx_pkt");
    if (dv_rx == NULL || ref_pkt == NULL || rx_pkt == NULL) {
        soc_dma_dv_free(unit, dv_rx);
        sal_dma_free(ref_pkt);
        sal_dma_free(rx_pkt);
        test_error(unit, "Failed to allocate memory for integrity check\n");
        return BCM_E_FAIL;
    }

    /* config all channels as RX */
    soc_dma_chan_cos_ctrl_set(unit, RX_CHAN, 1, 0xffffffff);
    soc_dma_chan_cos_ctrl_set(unit, RX_CHAN, 2, 0xffffffff);

    /* config unmatched VLAN packet to be forwarded to CPU */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CPU_CONTROL_0r, 0,
                                               UVLAN_TOCPUf, 0x1));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CPU_CONTROL_1r, 0,
                                               L3_MTU_FAIL_TOCPUf, 0x1));

    /* check packet integrity for each port */
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "%4s %4s %4s %8s %8s\n"),
                         "idx", "port", "spd", "tx_pkt", "rx_pkt"));
    PBMP_ITER(pkt_intg->rx_pbmp, port) {
        if (port < SOC_MAX_NUM_PORTS) {
            /* check integrity of each packet */
            port_exp_pkt_cnt = pkt_intg->port_flood_cnt[port];
            port_act_pkt_cnt = 0;
            vlan = pkt_intg->rx_vlan[port];

            for (j = 0; j < port_exp_pkt_cnt; j++) {
                rst_vlan = 0;
                if (j == 0) {
                    if (pkt_intg->type == PKT_TYPE_L3UC) {
                        rst_vlan = 2; /* L3 packet*/
                    } else {
                        rst_vlan = 1; /* L2 packet*/
                    }
                }
                /* receive one packet from CPU though rx_pkt_dma */
                if (stream_pktdma_rx(unit, rst_vlan, vlan, dv_rx, rx_pkt,
                                     MTU + header_size) != BCM_E_NONE) {
                    break;
                }
                port_act_pkt_cnt ++;

                /* record rx_vlan */
                rx_vlan = (rx_pkt[14+header_size] << 8) +
                          rx_pkt[15+header_size];
                flex_p->ipmc_cpu_rx_vlan_cnt[rx_vlan]++;

                /* generate reference packet */
                if (stream_gen_ref_pkt(unit, port, pkt_intg, rx_pkt, ref_pkt,
                                       &pkt_size) != BCM_E_NONE) {
                    cli_out("*ERROR, failed to generate ref_pkt "
                            "for integrity check, port %d\n",
                            port);
                    chk_fail = 1;
                    break;
                }

                /* check packet content (payload) */
                for (k = 0; k < (pkt_size - NUM_BYTES_CRC); k++) {
                    if (rx_pkt[k + header_size] != ref_pkt[k]) {
                        /* mac_sa and vlan are used for rep identification */
                        if (( 6 <= k && k <= 11) || /* mac_sa */
                            (14 <= k && k <= 15))   /* vlan   */
                            continue;

                        cli_out("*ERROR: Packet corruption, "
                                "Stream %d, Port %3d, Packet %d, "
                                "pkt_size=%0d, header_size=%0d, k=%0d, "
                                "rx_pkt[%0d]=0x%0x, ref_pkt[%0d]=0x%0x\n",
                                stream_cnt, port, j,
                                pkt_size, header_size, k,
                                k + header_size, rx_pkt[k + header_size],
                                k, ref_pkt[k]);

                        cli_out("-- Expected Packet:");
                        tgp_print_pkt(ref_pkt, pkt_size);
                        cli_out("-- Received Packet:");
                        tgp_print_pkt(rx_pkt, pkt_size+header_size);

                        chk_fail = 1;
                        break;
                    }
                }
            }

            /* disable forwarding pkt to cpu for current port */
            if (pkt_intg->type == PKT_TYPE_L3UC) {
                stream_set_l3mtu_valid(unit, vlan);
            } else {
                stream_set_vlan_valid(unit, vlan);
            }

            /* check packet count */
            bcm_port_speed_get(unit, port, &port_speed);
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "%4d %4d %3dG %8d %8d "),
                                 stream_cnt, port, port_speed/1000,
                                 port_exp_pkt_cnt, port_act_pkt_cnt));
            if (port_act_pkt_cnt == 0) {
                chk_fail = 1;
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "*WARNING")));
            } else if (port_act_pkt_cnt < port_exp_pkt_cnt &&
                !(SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), port))) {
                chk_fail = 1;
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "*WARNING")));
            }
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\n")));

            pkt_cnt_sum += port_act_pkt_cnt;
            stream_cnt++;
        }
    }

    /* check l3 copy: at least one pkt of each l3 replication is received. */
    {
        int vlan_num, port_idx, pkt_cnt, k;
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "Check Port L3 Replication Reception\n")));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "%4s %4s %6s %8s\n"),
                             "port", "rep", "vlan", "pkt_cnt"));

        for (i = 0; i < flex_p->ipmc_num_streams; i++) {
            for (j = 0; j < flex_p->num_ports_per_stream; j++) {
                port_idx = flex_p->ipmc_stream[i][j];
                port     = flex_p->ipmc_port_list[port_idx];
                if (flex_p->ipmc_port_l3_copy_en[port] != 1) continue;

                for (k = 0; k < flex_p->ipmc_port_vlan_cnt[port]; k++){
                    vlan_num= flex_p->ipmc_port_rep_vlan[port][k];
                    pkt_cnt = flex_p->ipmc_cpu_rx_vlan_cnt[vlan_num];
                    LOG_INFO(BSL_LS_APPL_TESTS,
                             (BSL_META_U(unit, "%4d %4d %4d %4d"),
                                         port, k, vlan_num, pkt_cnt));
                    if (flex_p->ipmc_cpu_rx_vlan_cnt[vlan_num] == 0) {
                        chk_fail = 1;
                        LOG_INFO(BSL_LS_APPL_TESTS,
                                 (BSL_META_U(unit,
                                    "\t*ERROR, no l3 packet received")));
                    }
                    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\n")));
                }
            }
        }
    }

    cli_out("Total packets received: %d\n", pkt_cnt_sum);
    if (chk_fail == 0) {
        cli_out("\n**** PACKET INTEGRITY CHECK PASSED *****\n");
    } else {
        cli_out("\n**** PACKET INTEGRITY CHECK FAILED *****\n");
        rv = BCM_E_FAIL;
    }

    soc_dma_abort_dv(unit, dv_rx);
    soc_dma_dv_free(unit, dv_rx);
    sal_dma_free(rx_pkt);
    sal_dma_free(ref_pkt);

    return rv;
}

/*
 * Function:
 *      flexport_ipmc_chk_integrity
 * Purpose:
 *      Redirect all packets back to CPU and check packet integrity
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
static bcm_error_t
flexport_ipmc_chk_integrity(int unit)
{
    int i, port;
    uint32 flood_cnt;
    uint8 mac_da[] = MAC_DA;
    stream_integrity_t *pkt_intg;
    bcm_error_t rv = BCM_E_NONE;
    flexport_mc_t *flex_p = flexport_parray[unit];

    if (!flex_p->check_integrity_param) return rv;

    pkt_intg = sal_alloc(sizeof(stream_integrity_t), "pkt_intg");
    if (pkt_intg == NULL) {
        test_error(unit, "Failed to allocate memory for pkt_intg\n");
        return BCM_E_FAIL;
    }
    sal_memset(pkt_intg, 0, sizeof(stream_integrity_t));

    pkt_intg->type = PKT_TYPE_IPMC;
    pkt_intg->ipv6_en = 0;
    SOC_PBMP_CLEAR(pkt_intg->rx_pbmp);
    for (i = 0; i < flex_p->ipmc_num_streams; i++) {
        port = flex_p->ipmc_port_list[flex_p->ipmc_stream[i][0]];
        if (port < SOC_MAX_NUM_PORTS && port < TEST_COVERITY_PBMP_WID) {
            /* rx_pbmp */
            SOC_PBMP_PORT_ADD(pkt_intg->rx_pbmp, port);

            /* rx_vlan: used to forward pkt to CPU */
            pkt_intg->rx_vlan[port] = flex_p->ipmc_drop_vlan[i];

            /* tx_vlan: used to re-generate ref_pkt */
            pkt_intg->tx_vlan[port] = flex_p->ipmc_exp_vlan[i];

            /* port_flood_cnt */
            /* flood_cnt = flexport_get_lossless_flood_cnt(unit, port); */
            flood_cnt = flex_p->num_ports_per_stream *
                        flex_p->ipmc_num_l3_ports_param * 30;
            pkt_intg->port_flood_cnt[port] = flood_cnt;

            /* port_pkt_seed */
            pkt_intg->port_pkt_seed[port] = flex_p->pkt_seed + i;

            /* mac_da, mac_sa */
            sal_memcpy(pkt_intg->mac_da[port], mac_da, NUM_BYTES_MAC_ADDR);
            sal_memcpy(pkt_intg->mac_sa[port], flex_p->ipmc_exp_mac_addr[i],
                       NUM_BYTES_MAC_ADDR);

            /* ip_da, ip_sa */
            pkt_intg->ip_da[port] = DIP;
            pkt_intg->ip_sa[port] = SIP;
        }
    }

    if (flexport_ipmc_chk_integrity_l3(unit, pkt_intg) != BCM_E_NONE) {
        flex_p->test_fail = 1;
        rv = BCM_E_FAIL;
    }

    sal_free(pkt_intg);
    return rv;
}

/*
 * Function:
 *      flexport_call_bcm_api
 * Purpose:
 *      Call the bcm_port_resource_multi_set API
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     SOC_E_XXXX
 */
static bcm_error_t
flexport_call_bcm_api(int unit)
{
    flexport_mc_t *flex_p = flexport_parray[unit];
    int i;
    bcm_port_t lport;
    uint32 nports;
    bcm_port_resource_t *resource;
    bcm_error_t rv;

    nports = flex_p->flex_num_del + flex_p->flex_num_add;
    resource = (bcm_port_resource_t *) sal_alloc(nports *
                                                 sizeof(bcm_port_resource_t),
                                                 "flexport_resource_array");
    sal_memset(resource, 0, sizeof(bcm_port_resource_t));

    for (i = 0; i < flex_p->flex_num_del; i ++) {
        resource[i] = flex_p->flex_resource_del[i];
    }
    for (i = 0; i < flex_p->flex_num_add; i ++) {
        resource[flex_p->flex_num_del + i] = flex_p->flex_resource_add[i];
    }

    PBMP_ITER(flex_p->flex_lport_dn_pbmp, lport) {
        if (lport < SOC_MAX_NUM_PORTS) {
            rv = bcm_port_enable_set(unit, lport, 0);
            if (BCM_FAILURE(rv)) {
                cli_out("\nbcm_port_disable_set %0d: %s",
                        lport, bcm_errmsg(rv));
                sal_free(resource);
                return rv;
            }
        }
    }

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nCalling bcm_port_resource_multi:")));
    for (i = 0; i < nports; i ++) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nresource[%3d]: pport = %3d, "),
                             i, resource[i].physical_port));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "lport  = %3d, "),
                             resource[i].port));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "speed = %3dG, "),
                             resource[i].speed/1000));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "lanes = %0d, "),
                             resource[i].lanes));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "encap = %0d"),
                             resource[i].encap));
    }
    rv = bcm_port_resource_multi_set(unit, nports, resource);
    if (BCM_FAILURE(rv)) {
        cli_out("\nbcm_port_resource_multi_set: %s", bcm_errmsg(rv));
        sal_free(resource);
        return rv;
    }

    cli_out("\nWait 20s for TSC configuration changes to take effect\n");
    sal_sleep(20);

    PBMP_ITER(flex_p->flex_lport_up_pbmp, lport) {
        if (lport < SOC_MAX_NUM_PORTS) {
            rv = bcm_port_enable_set(unit, lport, 1);
            if (BCM_FAILURE(rv)) {
                cli_out("\nbcm_port_enable_set %0d: %s", lport, bcm_errmsg(rv));
                sal_free(resource);
                return rv;
            }
        }
    }

    sal_free(resource);

    return BCM_E_NONE;
}


/*
 * Function:
 *      flexport_ipmc_update_stream
 * Purpose:
 *      This function actually sets up the IPMC packet flow. Please refer to
 *      items 4-6 of the "Test Setup" section of the test description at the
 *      beginning of this file.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing.
 */
static void
flexport_ipmc_update_stream(int unit)
{
    int i, j, stream_id, port, port_idx, port_exist;
    flexport_mc_t *flex_p = flexport_parray[unit];

    stream_id = 0;
    PBMP_ITER(flex_p->ipmc_pbmp, port) {
        if (SOC_PBMP_MEMBER(flex_p->flex_lport_up_pbmp, port)) {
            /* if port exists in stream, continue */
            port_exist = 0;
            i = stream_id;
            for (j = 0; j < flex_p->num_ports_per_stream; j++) {
                if (port == flex_p->ipmc_port_list[flex_p->ipmc_stream[i][j]]) {
                    port_exist = 1;
                    break;
                }
            }
            if (port_exist) continue;

            /* add port to stream */
            port_exist = 0;
            for (j = 0; j < flex_p->ipmc_port_list_size; j++) {
                if (port == flex_p->ipmc_port_list[j]) {
                    port_exist = 1;
                    break;
                }
            }
            if (port_exist) {
                port_idx = j;
            } else {
                port_idx = flex_p->ipmc_port_list_size++;
                flex_p->ipmc_port_list[port_idx] = port;
            }
            flex_p->ipmc_stream[i][flex_p->num_ports_per_stream++] = port_idx;
            flex_p->ipmc_port_l3_copy_en[port] = 1;

            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                        "[IPMC] add lport %3d to mc stream\n"), port));
        }
    }
    PBMP_ITER(flex_p->flex_lport_dn_pbmp, port) {
        if (flexport_is_ipmc_port(unit, port) &&
            !SOC_PBMP_MEMBER(flex_p->flex_lport_up_pbmp, port)) {
            flex_p->ipmc_port_l3_copy_en[port] = -1;

            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                        "[IPMC] remove lport %3d from mc stream\n"), port));
        }
    }

    /* print */
    flexport_ipmc_print_vlan_info(unit);
}

/*
 * Function:
 *      flexport_flex_ports
 * Purpose:
 *      flex ports.
 *        -- do flex by calling bcm apis.
 *        -- set test env for up-ports.
 *        -- send traffic
 *        -- measure rate
 *        --
 *        --
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      port_pbmp: port bitmap for down ports.
 *
 * Returns:
 *     BCM_E_XXXX
 */
static bcm_error_t
flexport_flex_ports(int unit)
{
    pbmp_t start_traffic_pbmp;
    flexport_mc_t *flex_p = flexport_parray[unit];
    uint64 val64;
    int rv = BCM_E_NONE;

    /* call bcm apis. */
    if (flex_p->flex_num_del + flex_p->flex_num_add > 0) {
        stream_set_lpbk(unit, flex_p->flex_lport_dn_pbmp,
                        BCM_PORT_LOOPBACK_NONE);
        flexport_call_bcm_api(unit);
        stream_set_lpbk(unit, flex_p->flex_lport_up_pbmp,
                        flex_p->loopback_mode_param);
    }

    /* set test env for up-ports */
    SOC_PBMP_CLEAR(start_traffic_pbmp);
    start_traffic_pbmp = flex_p->flex_lport_up_pbmp;
    BCM_PBMP_OR(start_traffic_pbmp, flex_p->flex_lport_ch_pbmp);

    /* clear counter */
    if (flex_p->clear_new_port_cnt_param) {
        COMPILER_64_ZERO(val64);
        BCM_IF_ERROR_RETURN(soc_counter_set_by_port(unit, start_traffic_pbmp,
                                                    val64));
    }

    /* turn off fc for up_ports */
    stream_turn_off_fc(unit, flex_p->flex_lport_up_pbmp);

    return rv;
}

/*
 * Function:
 *      flexport_get_tsc_speed_by_mode
 * Purpose:
 *      Get tsc speed list by converting tsc mode from string to int.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      tsc: TSC #
 *      mode: target port speed/mode
 *      speed: prt to tsc speed list
 *
 * Returns:
 *     BCM_E_XXXX
 */
static bcm_error_t
flexport_get_tsc_speed_by_mode(int unit, uint32 tsc, char *mode, int* speed)
{
    int rv = BCM_E_NONE;
    int i;

    for (i = 0; i < NUM_LANES_PER_TSC; i++) speed[i] = 0;

    if (sal_strstr(mode, "4x0G")) {
        return rv;
    } else if (sal_strstr(mode, "1x100G") || sal_strstr(mode, "1x106G")) {
        speed[0] = 100000;
    } else if (sal_strstr(mode, "50G+2x25G")) {
        speed[0] = 50000; speed[2] = 25000; speed[3] = 25000;
    } else if (sal_strstr(mode, "2x25G+50G")) {
        speed[0] = 25000; speed[1] = 25000; speed[2] = 50000;
    } else if (sal_strstr(mode, "20G+2x10G")) {
        speed[0] = 20000; speed[2] = 10000; speed[3] = 10000;
    } else if (sal_strstr(mode, "2x10G+20G")) {
        speed[0] = 10000; speed[1] = 10000; speed[2] = 20000;
    } else if (sal_strstr(mode, "40G+2x10G")) {
        speed[0] = 40000; speed[2] = 10000; speed[3] = 10000;
    } else if (sal_strstr(mode, "2x10G+40G")) {
        speed[0] = 10000; speed[1] = 10000; speed[2] = 40000;
    } else if (sal_strstr(mode, "1x50G") || sal_strstr(mode, "1x53G")) {
        speed[0] = 50000;
    } else if (sal_strstr(mode, "1x40G") || sal_strstr(mode, "1x42G")) {
        speed[0] = 40000;
    } else if (sal_strstr(mode, "1x25G") || sal_strstr(mode, "1x27G")) {
        speed[0] = 25000;
    } else if (sal_strstr(mode, "1x20G") || sal_strstr(mode, "1x21G")) {
        speed[0] = 20000;
    } else if ((sal_strstr(mode, "1x10G") || sal_strstr(mode, "1x11G"))) {
        speed[0] = 10000;
    } else if (sal_strstr(mode, "2x50G") || sal_strstr(mode, "2x53G")) {
        speed[0] = 50000; speed[2] = 50000;
    } else if (sal_strstr(mode, "2x40G") || sal_strstr(mode, "2x42G")) {
        speed[0] = 40000; speed[2] = 40000;
    } else if (sal_strstr(mode, "2x25G") || sal_strstr(mode, "2x27G")) {
        speed[0] = 25000; speed[2] = 25000;
    } else if (sal_strstr(mode, "2x20G") || sal_strstr(mode, "2x21G")) {
        speed[0] = 20000; speed[2] = 20000;
    } else if (sal_strstr(mode, "2x10G") || sal_strstr(mode, "2x11G")) {
        speed[0] = 10000; speed[2] = 10000;
    } else if (sal_strstr(mode, "40G+20G")) {
        speed[0] = 40000; speed[2] = 20000;
    } else if (sal_strstr(mode, "20G+40G")) {
        speed[0] = 20000; speed[2] = 40000;
    } else if (sal_strstr(mode, "4x25G") || sal_strstr(mode, "4x27G")) {
        speed[0] = 25000; speed[1] = 25000; speed[2] = 25000; speed[3] = 25000;
    } else if (sal_strstr(mode, "4x10G") || sal_strstr(mode, "4x11G")) {
        speed[0] = 10000; speed[1] = 10000; speed[2] = 10000; speed[3] = 10000;
    } else if (sal_strstr(mode, "4x1G")) {
        speed[0] = 1000; speed[1] = 1000; speed[2] = 1000; speed[3] = 1000;
    } else if (sal_strstr(mode, "10G_25G_mix")) {
        int subport_bmp = sal_rand() % 16;
        speed[0] = 25000; speed[1] = 25000; speed[2] = 25000; speed[3] = 25000;
        if (subport_bmp & 1) speed[0] = 10000;
        if (subport_bmp & 2) speed[1] = 10000;
        if (subport_bmp & 4) speed[2] = 10000;
        if (subport_bmp & 8) speed[3] = 10000;
    } else {
        test_error(unit, "*ERROR: Mode %s is not supported\n", mode);
    }

    return rv;
}

/*
 * Function:
 *      flexport_get_tsc_encap_by_mode
 * Purpose:
 *      Get tsc encap (HiGig or Enthernet) by tsc mode.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      tsc: TSC #
 *      mode: target port speed/mode
 *
 * Returns:
 *     BCM_E_XXXX
 */
static bcm_error_t
flexport_get_tsc_encap_by_mode(int unit, uint32 tsc, char *mode, int *encap)
{
    int rv = BCM_E_NONE;
    int encap_enum = BCM_PORT_ENCAP_IEEE;

    if (sal_strstr(mode, "11G") || sal_strstr(mode, "21G") ||
        sal_strstr(mode, "27G") || sal_strstr(mode, "53G") ||
        sal_strstr(mode, "42G") || sal_strstr(mode, "106G") ||
        sal_strstr(mode, "127G")) {
        encap_enum = BCM_PORT_ENCAP_HIGIG2;
    } else {
        encap_enum = BCM_PORT_ENCAP_IEEE;
    }
    (*encap) = encap_enum;

    return rv;
}

/*
 * Function:
 *      flexport_get_speed_lanes
 * Purpose:
 *      Converts hex string to 4-bit binary map
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      num_subport: number of subports
 *      speed: pointer to a port speed array
 *      lanes: pointer to a lane array
 *
 * Returns:
 *     Nothing
 */
static void
flexport_get_speed_lanes(int unit, int num_subport, int *speed, int *lanes)
{
    int i;

    for (i = 0; i < num_subport; i++) {
        switch(speed[i]) {
            case   1000 : lanes[i] = 1; break;
            case  10000 : lanes[i] = 1; break;
            case  20000 : lanes[i] = 2; break;
            case  25000 : lanes[i] = 1; break;
            case  40000 : lanes[i] = ((i==0) && (speed[2] == 0))? 4 : 2; break;
            case  50000 : lanes[i] = 2; break;
            case 100000 : lanes[i] = 4; break;
            default     : lanes[i] = 0; break;
        }
    }
}

/*
 * Function:
 *      flexport_assign_lport_by_pport
 * Purpose:
 *      Find an available logical port number for given physical up-port.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pport: physical port number.
 *
 * Returns:
 *     Return -1 if there is no available logical port.
 *     Otherwise return a valid unused logial port number.
 */
static int
flexport_assign_lport_by_pport(int unit, int pport)
{
    int lport = -1;

    /* Maverick2: mv2 has flexed p2l mapping, 32 lport per halfpipe. */
    if (SOC_IS_MAVERICK2(unit)) {
        lport = ((pport >= 1  && pport <= 32) ||
                 (pport >= 41 && pport <= 64)) ? pport : -1;
    }
    else if (SOC_IS_HELIX5(unit)) {
        lport = (pport <= 68) ? (pport) : (-1);
    }
    else if (SOC_IS_HURRICANE4(unit)) {
        lport = (pport <= 64) ? (pport) : (-1);
    }
    /* Trident3: td3 has fixed p2l mapping */
    else if (SOC_IS_TRIDENT3(unit)) {
        lport = (pport > 64) ? (2 + pport) : (pport);
    }
    /* default */
    else {
        lport = (pport > 64) ? (2 + pport) : (pport);
    }

    return lport;
}

/*
 * Function:
 *      flexport_fill_resource_per_tsc
 * Purpose:
 *      Prepare port_dn_pbmp, port_up_pbmp and port_ch_pbmp for one TSC.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      num_subport: number of subports
 *      first_phy_port: first physical port number of the TSC
 *      encap: Ethernet or HiGig2
 *
 * Returns:
 *     SOC_E_XXXX
 */
static bcm_error_t
flexport_fill_resource_per_tsc(int unit, int num_subport, int first_phy_port,
                               int *speed, int encap)
{
    soc_info_t *si;
    flexport_mc_t *flex_p = flexport_parray[unit];
    int i, resource_idx;
    int tsc = (num_subport > 0) ? ((first_phy_port - 1) / num_subport) : -1;
    int cur_speed[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int cur_lanes[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int cur_encap = BCM_PORT_ENCAP_IEEE;
    int lanes[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int lanes_changed = 0;
    bcm_port_t lport, pport;
    int rv;

    si = &SOC_INFO(unit);

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nFlexing TSC_%02d, port[%0d..%0d] to ["),
             tsc, first_phy_port, first_phy_port + num_subport - 1));
    for (i = 0; i < num_subport; i++) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, " %0dG"), speed[i] / 1000));
    }
    if (encap == BCM_PORT_ENCAP_IEEE) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "] Ethernet")));
    } else if (encap == BCM_PORT_ENCAP_HIGIG2) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "] Higig2")));
    }

    lport = si->port_p2l_mapping[first_phy_port];
    if (lport != -1 && IS_HG2_ENABLED_PORT(unit, lport)) {
        cur_encap = BCM_PORT_ENCAP_HIGIG2;
    }

    for (i = 0; i < num_subport; i++) {
        pport = first_phy_port + i;
        lport = si->port_p2l_mapping[pport];
        if (lport != -1) {
            rv = bcm_port_speed_get(unit, lport, &cur_speed[i]);
            if (BCM_FAILURE(rv) && rv != BCM_E_PORT) {
                cli_out("\nbcm_port_speed_get %0d %0d: %s",
                        lport, rv, bcm_errmsg(rv));
                return rv;
            }
        }
    }

    flexport_get_speed_lanes(unit, num_subport, cur_speed, cur_lanes);
    flexport_get_speed_lanes(unit, num_subport, speed, lanes);
    for (i = 0; i < num_subport; i++) {
        if (lanes[i] != cur_lanes[i]) {
            lanes_changed = 1;
            break;
        }
    }

    if (lanes_changed == 0 && encap == cur_encap &&
        flex_p->flex_no_spd_ch_param == 0) {
        for (i = 0; i < num_subport; i++) {
            if (speed[i] != cur_speed[i]) {
                pport = first_phy_port + i;
                lport = si->port_p2l_mapping[pport];
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                    "\nTSC_%02d, Changing pport %0d, from %0dG to %0dG"),
                    tsc, pport, cur_speed[i] / 1000, speed[i] / 1000));
                rv = bcm_port_speed_set(unit, lport, speed[i]);
                if (BCM_FAILURE(rv) && rv != BCM_E_PORT) {
                    cli_out("\nbcm_port_speed_set %0d %0d: %s",
                            lport, speed[i], bcm_errmsg(rv));
                    return rv;
                }
                SOC_PBMP_PORT_ADD(flex_p->flex_lport_ch_pbmp, lport);
            }
        }
        return BCM_E_NONE;
    }

    for (i = 0; i < num_subport; i++) {
        pport = first_phy_port + i;
        lport = si->port_p2l_mapping[pport];
        if (lport != -1) {
            if (cur_speed[i] > 0 &&
                (speed[i] != cur_speed[i] || encap != cur_encap ||
                 lanes[i] != cur_lanes[i])) {
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit,
                            "\nTSC_%02d, Removing pport %0d, speed %0dG"),
                            tsc, pport, cur_speed[i] / 1000));
                resource_idx = flex_p->flex_num_del++;
                bcm_port_resource_t_init(
                    &flex_p->flex_resource_del[resource_idx]);
                flex_p->flex_resource_del[resource_idx].physical_port = -1;
                flex_p->flex_resource_del[resource_idx].port = lport;
                SOC_PBMP_PORT_ADD(flex_p->flex_lport_dn_pbmp, lport);
            }
        }
    }

    for (i = 0; i < num_subport; i++) {
        if (speed[i] > 0 &&
            (speed[i] != cur_speed[i] || encap != cur_encap ||
             lanes[i] != cur_lanes[i])) {
            pport = first_phy_port + i;
            /* lport = (pport > 64) ? (2 + pport) : (pport); */
            lport = flexport_assign_lport_by_pport(unit, pport);
            if(lport == -1) {
                cli_out("\nERROR : Unable to find valid logical port for"
                                              " physical port %0d", pport);
                return BCM_E_INTERNAL;
            }
            resource_idx = flex_p->flex_num_add++;
            bcm_port_resource_t_init(
                &flex_p->flex_resource_add[resource_idx]);
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                     "\nTSC_%02d, Adding   pport %0d, speed %0dG,  lane %0d"),
                     tsc, pport, speed[i] / 1000, lanes[i]));
            flex_p->flex_resource_add[resource_idx].physical_port = pport;
            flex_p->flex_resource_add[resource_idx].port  = lport;
            flex_p->flex_resource_add[resource_idx].speed =
                                      stream_get_exact_speed(speed[i], encap);
            flex_p->flex_resource_add[resource_idx].lanes = lanes[i];
            flex_p->flex_resource_add[resource_idx].encap = encap;
            SOC_PBMP_PORT_ADD(flex_p->flex_lport_up_pbmp, lport);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      flexport_cfg_resource_per_tsc
 * Purpose:
 *      Configure resource for one tsc by tsc mode.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      tsc: TSC #
 *      mode: target port speed/mode
 *
 * Returns:
 *     BCM_E_XXXX
 */
static bcm_error_t
flexport_cfg_resource_per_tsc(int unit, uint32 tsc, char *mode)
{
    int rv = BCM_E_NONE;
    int speed[NUM_LANES_PER_TSC];
    int num_subport = 4;
    int encap = BCM_PORT_ENCAP_IEEE;

    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                "\nFlexing TSC_%02d to %s"), tsc, mode));

    flexport_get_tsc_speed_by_mode(unit, tsc, mode, speed);
    flexport_get_tsc_encap_by_mode(unit, tsc, mode, &encap);

    rv = flexport_fill_resource_per_tsc(unit, num_subport,
                                        tsc * NUM_LANES_PER_TSC + 1,
                                        speed, encap);
    if (BCM_FAILURE(rv)) {
        cli_out("\nflex_tsc %0d: %s", tsc , bcm_errmsg(rv));
    }

    return rv;
}

/*
 * Function:
 *      flexport_is_ovsb_cfg
 * Purpose:
 *      Configure resource for flexports.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     BCM_E_XXXX
 */
static int
flexport_is_ovsb_cfg(int unit)
{
    int is_ovsb = 0;
    int port_speed, port;

    PBMP_ITER(PBMP_E_ALL(unit), port) {
        if ((port < SOC_MAX_NUM_PORTS) &&
            (flexport_is_ipmc_port(unit, port) ||
             flexport_is_l3uc_port(unit, port)) ) {
            bcm_port_speed_get(unit, port, &port_speed);
            if (SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), port) && port_speed > 0) {
                is_ovsb = 1;
                break;
            }
        }
    }

    return  is_ovsb;
}

/*
 * Function:
 *      flexport_rand_tsc_mode
 * Purpose:
 *      Randomly select TSCs for flexing.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      min_tsc: lowest tsc number.
 *      max_tsc: highest tsc number.
 *      num_req_tsc: number of required tsc.
 *      tsc_selected: ptr to tsc selected status. 1:selected, 0:available.
 *
 * Returns:
 *     BCM_E_XXXX
 */
static void
flexport_rand_tsc_mode(int unit, int min_tsc, int max_tsc, int num_req_tsc,
                       uint32 *tsc_selected)
{
    int tsc_cnt = 0, tsc = 1, num_tsc, num_avail_tsc;

    num_avail_tsc = max_tsc - min_tsc + 1;
    if (min_tsc >= 0 && min_tsc < MAX_NUM_TSC &&
        max_tsc >= 0 && max_tsc < MAX_NUM_TSC &&
        num_avail_tsc > 0 && num_req_tsc > 0) {
        num_tsc = (num_req_tsc <= num_avail_tsc) ? num_req_tsc : num_avail_tsc;
        for (tsc_cnt = 0; tsc_cnt < num_tsc; tsc_cnt++) {
            do {
                /* coverity[dont_call : FALSE] */
                tsc = (sal_rand() % num_avail_tsc) + min_tsc;
            } while (tsc_selected[tsc % MAX_NUM_TSC]);
            tsc_selected[tsc] = 1;
        }
    }
}

/*
 * Function:
 *      flexport_cfg_test_case
 * Purpose:
 *      Configure resource for flexports.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     BCM_E_XXXX
 */
static void
flexport_cfg_test_case(int unit, int case_num)
{
    int i;
    int tsc_cnt;
    uint32 tsc_selected[MAX_NUM_TSC];
    int tsc_lo, tsc_hi, tsc_mid;
    int tsc_lo_ipmc = -1, tsc_hi_ipmc = -1;
    int tsc_lo_l3uc = -1, tsc_hi_l3uc = -1;
    flexport_mc_t *flex_p = flexport_parray[unit];

    for (i = 0; i < MAX_NUM_TSC; i++) flex_p->flex_tsc_mode[i] = NULL;

    if (SOC_IS_MAVERICK2(unit)) {
        tsc_lo  = 0;
        tsc_hi  = 19;
        tsc_mid = (tsc_lo + tsc_hi + 1) / 2;

        tsc_lo_ipmc = 0;
        tsc_hi_ipmc = 7;
        tsc_lo_l3uc = 10;
        tsc_hi_l3uc = 14;
    } else if (SOC_IS_HELIX5(unit)) {
        /* Only test TSCf */
        tsc_lo  = 17;
        tsc_hi  = 19;
        tsc_mid = (tsc_lo + tsc_hi + 1) / 2;
    } else if (SOC_IS_HURRICANE4(unit)) {
        /* Only test TSCf */
        tsc_lo  = 14;
        tsc_hi  = 14;
        tsc_mid = (tsc_lo + tsc_hi + 1) / 2;
    } else if (SOC_IS_TRIDENT3X(unit)) {
        tsc_lo  = 0;
        tsc_hi  = 31;
        tsc_mid = (tsc_lo + tsc_hi + 1) / 2;
        tsc_hi  = 30; /* ignore the last PM due to 2nd mgmt port */
    } else {
        tsc_lo  = 0;
        tsc_hi  = 31;
        tsc_mid = (tsc_lo + tsc_hi + 1) / 2;
    }

    if (tsc_lo_ipmc == -1 || tsc_hi_ipmc == -1 ||
        tsc_lo_l3uc == -1 || tsc_hi_l3uc == -1) {
        if (tsc_lo_ipmc == -1) tsc_lo_ipmc = tsc_lo;
        if (tsc_hi_ipmc == -1) tsc_hi_ipmc = tsc_mid - 1;
        if (tsc_lo_l3uc == -1) tsc_lo_l3uc = tsc_mid;
        if (tsc_hi_l3uc == -1) tsc_hi_l3uc = tsc_hi;
    }

    if (case_num == 0) {
        if (SOC_IS_MAVERICK2(unit)) {
            /* Maverick 2 has mixed 10G/25G PM config */
            for (i = tsc_lo_ipmc; i <= tsc_hi_ipmc ; i++)
                flex_p->flex_tsc_mode[i] = "10G_25G_mix";
            for (i = tsc_lo_l3uc; i <= tsc_hi_l3uc ; i++)
                flex_p->flex_tsc_mode[i] = "10G_25G_mix";
        }
        else {
            /* oversub */
            if(flexport_is_ovsb_cfg(unit)) {
                for (i = tsc_lo_ipmc; i <= tsc_hi_ipmc ; i++)
                    flex_p->flex_tsc_mode[i] = "4x25G";
                for (i = tsc_lo_l3uc; i <= tsc_hi_l3uc ; i++)
                    flex_p->flex_tsc_mode[i] = "4x25G";
            }
            /* linerate */
            else {
                if (tsc_mid > 0) {
                    for (i = tsc_lo_ipmc; i <= tsc_hi_ipmc ; i++)
                        flex_p->flex_tsc_mode[i] = ((i % tsc_mid) < 6) ?
                                                   "4x25G" : "4x0G";
                    for (i = tsc_lo_l3uc; i <= tsc_hi_l3uc ; i++)
                        flex_p->flex_tsc_mode[i] = ((i % tsc_mid) < 6) ?
                                                   "4x25G" : "4x0G";
                }
            }
        }

        if ((!flexport_is_ovsb_cfg(unit)) ||
            (flexport_is_ovsb_cfg(unit) && SOC_IS_MAVERICK2(unit))) {
            for (i = tsc_lo; i <= tsc_hi ; i++) {
                if (!((i >= tsc_lo_ipmc && i <= tsc_hi_ipmc) ||
                      (i >= tsc_lo_l3uc && i <= tsc_hi_l3uc)))
                    flex_p->flex_tsc_mode[i] = "4x0G";
            }
        }
    }
    else if (case_num == 1) {
        /* randomly select tsc for flexport */
        sal_memset(tsc_selected, 0, sizeof(uint32) * MAX_NUM_TSC);
        flexport_rand_tsc_mode(unit, 1, tsc_hi_ipmc, 6, tsc_selected);
        flexport_rand_tsc_mode(unit, tsc_lo_l3uc, tsc_hi_l3uc, 6, tsc_selected);

        /* oversub */
        if(flexport_is_ovsb_cfg(unit)) {
            /* ipmc */
            tsc_cnt = 0;
            for (i = tsc_lo_ipmc; i <= tsc_hi_ipmc; i++) {
                if (tsc_selected[i] == 0) continue;
                tsc_cnt++;
                switch (tsc_cnt) {
                   case 1: flex_p->flex_tsc_mode[i] = "1x50G";   break;
                   case 2: flex_p->flex_tsc_mode[i] = "1x100G"; break;
                   case 3: flex_p->flex_tsc_mode[i] = "2x50G";  break;
                   case 4: flex_p->flex_tsc_mode[i] = "4x25G";  break;
                   case 5: flex_p->flex_tsc_mode[i] = "2x40G";  break;
                   case 6: flex_p->flex_tsc_mode[i] = "1x40G";  break;
                   default:flex_p->flex_tsc_mode[i] = NULL;    break;
               }
            }
            /* l3uc */
            tsc_cnt = 0;
            for (i = tsc_lo_l3uc; i <= tsc_hi_l3uc; i++) {
                if (tsc_selected[i] == 0 || i == 22) {
                    flex_p->flex_tsc_mode[i] = "4x0G";
                    continue;
                }
                tsc_cnt++;
                switch (tsc_cnt) {
                   case 1: flex_p->flex_tsc_mode[i] = "1x100G"; break;
                   case 2: flex_p->flex_tsc_mode[i] = "2x53G";  break;
                   case 3: flex_p->flex_tsc_mode[i] = "4x25G";  break;
                   case 4: flex_p->flex_tsc_mode[i] = "4x27G";  break;
                   case 5: flex_p->flex_tsc_mode[i] = "1x40G";  break;
                   default:flex_p->flex_tsc_mode[i] = "4x0G";   break;
               }
            }
        }
        /* linerate */
        else {
            /* ipmc: The 1st tsc is reserved for traffic gen */
            tsc_cnt = 0;
            for (i = tsc_lo_ipmc + 1; i <= tsc_hi_ipmc; i++) {
                if (tsc_selected[i] == 0) {
                    flex_p->flex_tsc_mode[i] = "4x0G";
                    continue;
                }
                tsc_cnt++;
                switch (tsc_cnt) {
                   case 1: flex_p->flex_tsc_mode[i] = NULL;        break;
                   case 2: flex_p->flex_tsc_mode[i] = "2x25G+50G"; break;
                   case 3: flex_p->flex_tsc_mode[i] = "4x0G";      break;
                   case 4: flex_p->flex_tsc_mode[i] = "2x50G";     break;
                   case 5: flex_p->flex_tsc_mode[i] = "2x40G";     break;
                   case 6: flex_p->flex_tsc_mode[i] = "1x50G";     break;
                   default:flex_p->flex_tsc_mode[i] = "4x0G";      break;
               }
            }

            /* l3uc */
            tsc_cnt = 0;
            for (i = tsc_lo_l3uc + 1; i <= tsc_hi_l3uc; i++) {
                if (tsc_selected[i] == 0) {
                    flex_p->flex_tsc_mode[i] = "4x0G";
                    continue;
                }
                tsc_cnt++;
                switch (tsc_cnt) {
                   /* case 1: flex_p->flex_tsc_mode[i] = "4x27G";    break; */
                   case 1: flex_p->flex_tsc_mode[i] = "4x25G";    break;
                   case 2: flex_p->flex_tsc_mode[i] = "50G+2x25G";break;
                   case 3: flex_p->flex_tsc_mode[i] = "2x40G";    break;
                   /* case 4: flex_p->flex_tsc_mode[i] = "1x53G";    break; */
                   case 4: flex_p->flex_tsc_mode[i] = "1x50G";    break;
                   case 5: flex_p->flex_tsc_mode[i] = "2x50G";    break;
                   case 6: flex_p->flex_tsc_mode[i] = "1x40G";    break;
                   default:flex_p->flex_tsc_mode[i] = "4x0G";     break;
               }
            }
        }
    }

    /* print */
    cli_out("\nFlexport Config: case_num = %0d\n", case_num);
    cli_out("%4s\t%s\n","Tsc ", "TscMode");
    for (i = 0; i < MAX_NUM_TSC; i++) {
        if (flex_p->flex_tsc_mode[i] != NULL)
            cli_out("%4d, \t%s\n", i, flex_p->flex_tsc_mode[i]);
    }

}

/*
 * Function:
 *      flexport_cfg_resource
 * Purpose:
 *      Configure resource for flexports.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     BCM_E_XXXX
 */
static bcm_error_t
flexport_cfg_resource(int unit)
{
    int i, rv = BCM_E_NONE;
    char *mode;
    flexport_mc_t *flex_p = flexport_parray[unit];

    /* init */
    flex_p->flex_num_del = 0;
    flex_p->flex_num_add = 0;

    SOC_PBMP_CLEAR(flex_p->flex_lport_dn_pbmp);
    SOC_PBMP_CLEAR(flex_p->flex_lport_up_pbmp);
    SOC_PBMP_CLEAR(flex_p->flex_lport_ch_pbmp);

    for (i = 0; i < MAX_NUM_TSC; i++) {
        mode = flex_p->flex_tsc_mode[i];
        if (mode != NULL) {
            BCM_IF_ERROR_RETURN(flexport_cfg_resource_per_tsc(unit, i, mode));
        }
    }

    /* print */
    flexport_print_ports(unit, flex_p->flex_lport_dn_pbmp, "Down Ports");
    flexport_print_ports(unit, flex_p->flex_lport_up_pbmp, "UP Ports");
    flexport_print_ports(unit, flex_p->flex_lport_ch_pbmp, "Changed Ports");

    return rv;
}


/*
 * Function:
 *      flexport_l3uc_set_port_property
 * Purpose:
 *      Set rand_pkt_sizes, port_oversub, and exp_rates arrays.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pbmp - Device port bitmap
 *      pkt_size_param - Packet size in bytes
 *      max_num_cells_param - Maximum number of cells
 *
 * Returns:
 *     BCM_E_XXXX
 */
static bcm_error_t
flexport_l3uc_set_port_property(int unit, pbmp_t pbmp, int pkt_size_param,
                         int max_num_cells_param)
{
  int p, j, pkt_size;
  stream_rate_t *stream_rate_calc_p;
  flexport_l3uc_rate_calc_t *rate_calc_p = rate_calc_parray[unit];
  flexport_mc_t *flex_p = flexport_parray[unit];

  stream_rate_calc_p = sal_alloc(sizeof(stream_rate_t), "streaming_rate");
  sal_memset(stream_rate_calc_p, 0, sizeof(stream_rate_t));

  stream_rate_calc_p->mode         = 0;
  stream_rate_calc_p->tolerance_lr = flex_p->rate_tolerance_lr_param;
  stream_rate_calc_p->tolerance_os = flex_p->rate_tolerance_os_param;
  SOC_PBMP_CLEAR(stream_rate_calc_p->pbmp);
  PBMP_ITER(PBMP_E_ALL(unit), p) {
      if (p < SOC_MAX_NUM_PORTS && p < TEST_COVERITY_PBMP_WID) {
          stream_rate_calc_p->pkt_size = flexport_get_port_pkt_size(unit, p);
          SOC_PBMP_PORT_ADD(stream_rate_calc_p->pbmp, p);
      }
  }

  if (SOC_FAILURE(stream_calc_exp_rate_by_rx(unit, rate_calc_p->exp_rate,
                                             stream_rate_calc_p))) {
      cli_out("\n*ERROR, stream_calc_exp_rate_by_rx failed.\n");
      sal_free(stream_rate_calc_p);
      return BCM_E_FAIL;
  }
  sal_free(stream_rate_calc_p);

  PBMP_ITER(pbmp, p) {
      if (p < SOC_MAX_NUM_PORTS) {
          for (j = 0; j < TARGET_CELL_COUNT; j++) {
              do {
                  /* coverity[dont_call : FALSE] */
                  pkt_size = (sal_rand() % (MTU - MIN_PKT_SIZE + 1)) +
                             MIN_PKT_SIZE;
              } while (stream_get_pkt_cell_cnt(unit, pkt_size, p) >
                       max_num_cells_param);
              rate_calc_p->rand_pkt_sizes[p][j] = pkt_size;
          }
      }
  }

  return BCM_E_NONE;
}

/*
 * Function:
 *      flexport_l3uc_set_up_ports
 * Purpose:
 *      Enable port bridging and HiGig lookup for HG2 ports
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pbmp - Device port bitmap
 *
 * Returns:
 *     Nothing
 */
static void
flexport_l3uc_set_up_ports(int unit, pbmp_t pbmp)
{
    int i, p;
    lport_tab_entry_t lport_tab_entry;
    port_tab_entry_t port_tab_entry;
    soc_field_t ihg_lookup_fields[] =
        { HYBRID_MODE_ENABLEf, USE_MH_PKT_PRIf, USE_MH_VIDf, HG_LOOKUP_ENABLEf,
        REMOVE_MH_SRC_PORTf
    };
    uint32 ihg_lookup_values[] = { 0x0, 0x1, 0x1, 0x1, 0x0 };

    cli_out("\nEnabling HG_LOOKUP on HG ports");

    PBMP_ITER(pbmp, p) {
        if (p < SOC_MAX_NUM_PORTS && IS_HG_PORT(unit, p)) {
            soc_reg_fields32_modify(unit, IHG_LOOKUPr, p, 5,
                                    ihg_lookup_fields, ihg_lookup_values);
        }
    }

    cli_out("\nEnabling Port bridging");

    for (i = 0; i < soc_mem_index_max(unit, LPORT_TABm); i++) {
        (void) soc_mem_read(unit, LPORT_TABm, COPYNO_ALL, i,
                     lport_tab_entry.entry_data);
        soc_mem_field32_set(unit, LPORT_TABm, lport_tab_entry.entry_data,
                            ALLOW_SRC_MODf, 0x1);
        soc_mem_field32_set(unit, LPORT_TABm, lport_tab_entry.entry_data,
                            PORT_BRIDGEf, 0x1);
        (void) soc_mem_write(unit, LPORT_TABm, COPYNO_ALL, i,
                      lport_tab_entry.entry_data);
    }

    for (i = 0; i < soc_mem_index_max(unit, PORT_TABm); i++) {
        (void) soc_mem_read(unit, PORT_TABm, COPYNO_ALL, i,
                     port_tab_entry.entry_data);
        soc_mem_field32_set(unit, PORT_TABm, port_tab_entry.entry_data,
                            PORT_BRIDGEf, 0x1);
        (void) soc_mem_write(unit, PORT_TABm, COPYNO_ALL, i,
                          port_tab_entry.entry_data);
    }
}

/*
 * Function:
 *      flexport_l3uc_set_up_streams
 * Purpose:
 *      VLAN programming. Each port is put on an unique VLAN.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pbmp - Device port bitmap
 *      vlan_bmp - bitmap of device ports that have vlan set up
 *
 * Returns:
 *     vlan_pbmp - bitmap of device ports that have vlan set up.
 */
static pbmp_t
flexport_l3uc_set_up_streams(int unit, pbmp_t pbmp, pbmp_t vlan_pbmp)
{
    int p;
    pbmp_t pbm, ubm;
    bcm_vlan_t vlan;

    BCM_PBMP_CLEAR(ubm);

    PBMP_ITER(pbmp, p) {
        if (p < SOC_MAX_NUM_PORTS) {
            vlan = VLAN + p;
            if (SOC_PBMP_MEMBER(vlan_pbmp, p)) {
                stream_set_vlan(unit, vlan, 1);
            } else {
                BCM_PBMP_CLEAR(pbm);
                bcm_vlan_create(unit, vlan);
                BCM_PBMP_PORT_ADD(pbm, p);
                bcm_vlan_port_add(unit, vlan, pbm, ubm);
            }
        }
    }

    SOC_PBMP_OR(vlan_pbmp, pbmp);

    return vlan_pbmp;
}

/*
 * Function:
 *     flexport_l3uc_send_pkts
 * Purpose:
 *     Sends packet for all the ports in the port bitmap
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pbmp - port bitmap
 *      pkt_size_param -
 *      pkt_seed -
 *
 * Returns:
 *     flood packet count for the given port.
 */
static void
flexport_l3uc_send_pkts(int unit, pbmp_t pbmp, uint32 pkt_size_param,
                   uint32 pkt_seed)
{
	int port;
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
	uint32 pkt_size;
    stream_pkt_t *tx_pkt;
	flexport_l3uc_rate_calc_t *rate_calc_p = rate_calc_parray[unit];

    tx_pkt = sal_alloc(sizeof(stream_pkt_t), "tx_pkt");
    sal_memset(tx_pkt, 0, sizeof(stream_pkt_t));

    cli_out("\n==================================================\n");
    cli_out("\nSending packets ...\n\n");
    PBMP_ITER(pbmp, port) {
        if (port < SOC_MAX_NUM_PORTS) {
/*             if (pkt_size_param == 0) {
                pkt_size = stream_get_wc_pkt_size(unit, IS_HG_PORT(unit, port));
            } else {
                pkt_size = pkt_size_param;
            } */
            pkt_size = flexport_get_port_pkt_size(unit, port);

            tx_pkt->port = port;
            tx_pkt->num_pkt = flexport_get_lossless_flood_cnt(unit, port);
            tx_pkt->pkt_seed = pkt_seed;
            tx_pkt->pkt_size = pkt_size;
    		cli_out("\nSending packets on port %0d size %0d num %0d.",
                    port, pkt_size, tx_pkt->num_pkt);
			if(pkt_size_param == 1) {
              tx_pkt->rand_pkt_size_en = 1;
              tx_pkt->rand_pkt_size = rate_calc_p->rand_pkt_sizes[port];
            } else {
              tx_pkt->rand_pkt_size_en = 0;
              tx_pkt->rand_pkt_size = NULL;
            }
            tx_pkt->tx_vlan = VLAN + port;
            sal_memcpy(tx_pkt->mac_da, mac_da, NUM_BYTES_MAC_ADDR);
            sal_memcpy(tx_pkt->mac_sa, mac_sa, NUM_BYTES_MAC_ADDR);
            stream_tx_pkt(unit, tx_pkt);
        }
    }
	sal_free(tx_pkt);
}

/*
 * Function:
 *      flexport_l3uc_start_rate_record
 * Purpose:
 *      Get current time, packet count & byte count needed to calculate port
 *      rate
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pbmp - Device port bitmap to be used
 *      emulation_param - 1: test run in emulator platform
 *                        0: test run is actual hardware(chip)
 *
 * Returns:
 *     SOC_E_XXXX
 */
static bcm_error_t
flexport_l3uc_start_rate_record(int unit, pbmp_t pbmp, uint32 emulation_param)
{
    flexport_l3uc_rate_calc_t *rate_calc_p = rate_calc_parray[unit];

    return record_rate_information(unit, emulation_param, pbmp,
                                   rate_calc_p->stime,
                                   rate_calc_p->tpkt_start,
                                   rate_calc_p->tbyt_start);

}


/*
 * Function:
 *      flexport_l3uc_chk_rate_run
 * Purpose:
 *      Check rates against expected rates.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pbmp - Device port bitmap to be checked
 *      tolerance_lr - Percentage of rate tolerance for line rate ports
 *      tolerance_os - Percentage of rate tolerance for oversub ports
 *
 * Returns:
 *     SOC_E_XXXX
 */
static bcm_error_t
flexport_l3uc_chk_rate_run(int unit, pbmp_t pbmp, uint32 tolerance_lr,
             uint32 tolerance_os, uint32 emulation_param)
{
    uint64 *etime;
    uint64 *tpkt_end;
    uint64 *tbyt_end;
    uint64 *rate;
    flexport_l3uc_rate_calc_t *rate_calc_p = rate_calc_parray[unit];
    bcm_error_t rv = BCM_E_NONE;

    etime = (uint64 *) sal_alloc(SOC_MAX_NUM_PORTS * sizeof(uint64),
                                      "etime");
    tpkt_end = (uint64 *) sal_alloc(SOC_MAX_NUM_PORTS * sizeof(uint64),
                                    "tpkt_end");
    tbyt_end = (uint64 *) sal_alloc(SOC_MAX_NUM_PORTS * sizeof(uint64),
                                    "tbyt_end");
    rate = (uint64 *) sal_alloc(SOC_MAX_NUM_PORTS * sizeof(uint64),
                                "rate");

    sal_memset(etime,    0, sizeof(uint64) * SOC_MAX_NUM_PORTS);
    sal_memset(tpkt_end, 0, sizeof(uint64) * SOC_MAX_NUM_PORTS);
    sal_memset(tbyt_end, 0, sizeof(uint64) * SOC_MAX_NUM_PORTS);
    sal_memset(rate,     0, sizeof(uint64) * SOC_MAX_NUM_PORTS);

    rv = record_rate_information(unit, emulation_param, pbmp, etime,
                                     tpkt_end, tbyt_end);

    calc_act_port_rate(unit, pbmp, rate_calc_p->stime, etime,
                       rate_calc_p->tpkt_start, tpkt_end,
                       rate_calc_p->tbyt_start, tbyt_end, rate);

    rv = compare_rates(unit, pbmp, tolerance_lr, tolerance_os,
                  rate_calc_p->exp_rate, rate);

    if (rv != BCM_E_NONE) {
        test_error(unit, "********** [L3UC] RATE CHECK FAILED ***********\n");
    } else {
        cli_out("\n********** [L3UC] RATE CHECK PASSED ***********");
    }

    sal_free(etime);
    sal_free(tpkt_end);
    sal_free(tbyt_end);
    sal_free(rate);
    return rv;
}

/*
 * Function:
 *      flexport_l3uc_chk_counter
 * Purpose:
 *      Check rate for ports in L3UC categories.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     SOC_E_XXXX
 */
static void
flexport_l3uc_chk_rate(int unit, pbmp_t pbmp)
{
    flexport_mc_t *flex_p = flexport_parray[unit];

    /* check mib counter */
    if (flex_p->check_mib_param == 1) {
        if (stream_chk_mib_counters(unit, pbmp, 0) != BCM_E_NONE) {
            flex_p->test_fail = 1;
        }
    }

    /* check rate */
    if (flex_p->check_rate_param == 1) {
        cli_out("\nWait 2s for traffic to stabilize");
        sal_sleep(2);
        /* start rate measurement */
        flexport_l3uc_start_rate_record(unit, pbmp, flex_p->emulation_param);
        cli_out("\nMeasuring Rate over a period of %0ds",
                flex_p->rate_calc_interval_param);
        sal_sleep(flex_p->rate_calc_interval_param);

        /* start rate checking */
        if (flexport_l3uc_chk_rate_run(unit, pbmp,
                       flex_p->rate_tolerance_lr_param,
                       flex_p->rate_tolerance_os_param,
                       flex_p->emulation_param) != BCM_E_NONE) {
            flex_p->test_fail = 1;
        }
    }
}

/*
 * Function:
 *      flexport_l3uc_chk_integrity
 * Purpose:
 *      Check integrity for ports in L3UC categories.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     SOC_E_XXXX
 */
static void
flexport_l3uc_chk_integrity(int unit, pbmp_t pbmp)
{
    int p;
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    flexport_mc_t *flex_p = flexport_parray[unit];
    stream_integrity_t *pkt_intg;
    int rv = BCM_E_NONE;

    cli_out("\nWait 2s for traffic to stabilize");
    sal_sleep(2);

    /* check integrity */
    if (flex_p->check_integrity_param != 0) {
        pkt_intg = sal_alloc(sizeof(stream_integrity_t), "pkt integrity");
        sal_memset(pkt_intg, 0, sizeof(stream_integrity_t));

        pkt_intg->rx_pbmp = pbmp;
        PBMP_ITER(pbmp, p) {
            if (p < SOC_MAX_NUM_PORTS) {
                pkt_intg->port_pkt_seed[p] = flex_p->pkt_seed;
                pkt_intg->port_flood_cnt[p] =
                    flexport_get_lossless_flood_cnt(unit, p);
                pkt_intg->rx_vlan[p] = VLAN + p;
                pkt_intg->tx_vlan[p] = VLAN + p;
                sal_memcpy(pkt_intg->mac_da[p], mac_da, NUM_BYTES_MAC_ADDR);
                sal_memcpy(pkt_intg->mac_sa[p], mac_sa, NUM_BYTES_MAC_ADDR);
            }
		}

        rv = stream_chk_pkt_integrity(unit, pkt_intg);
        if (rv != BCM_E_NONE) {
            flex_p->test_fail = 1;
        }
        sal_free(pkt_intg);
    }
}

/*
 * Function:
 *      flexport_l3uc_update_stream
 * Purpose:
 *      flex ports.
 *        -- do flex by calling bcm apis.
 *        -- set test env for up-ports.
 *        -- send traffic
 *        -- measure rate
 *        --
 *        --
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      port_pbmp: port bitmap for down ports.
 *
 * Returns:
 *     BCM_E_XXXX
 */
static bcm_error_t
flexport_l3uc_update_stream(int unit, pbmp_t pbmp)
{
    flexport_mc_t *flex_p = flexport_parray[unit];
    int rv = BCM_E_NONE;

    /* set port property: rand_pkt_size */
    flexport_l3uc_set_port_property(unit, pbmp,
                             flex_p->pkt_size_param,
                             flex_p->max_num_cells_param);
    /* enable port bridging for loopback */
    flexport_l3uc_set_up_ports(unit, flex_p->flex_lport_up_pbmp);

    /* set stream */
    flex_p->l3uc_vlan_pbmp = flexport_l3uc_set_up_streams(unit, pbmp,
                                           flex_p->l3uc_vlan_pbmp);

    /* send traffic */
    flexport_l3uc_send_pkts(unit, pbmp, flex_p->pkt_size_param,
                            flex_p->pkt_seed);

    return rv;
}



/*
 * Function:
 *      flexport_divide_ports_for_l3uc_ipmc
 * Purpose:
 *      Divide ports into IPMC and L3UC categories.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     BCM_E_XXXX
 */
static void
flexport_divide_ports_for_l3uc_ipmc(int unit)
{
    int p;
    flexport_mc_t *flex_p = flexport_parray[unit];

    cli_out("\nDivide ports into l3uc and ipmc groups\n");
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                "SOC_MAX_NUM_PORTS %3d\n"), SOC_MAX_NUM_PORTS));
    BCM_PBMP_CLEAR(flex_p->ipmc_pbmp);
    BCM_PBMP_CLEAR(flex_p->l3uc_pbmp);

    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        if (flexport_is_ipmc_port(unit, p)) {
            BCM_PBMP_PORT_ADD(flex_p->ipmc_pbmp, p);
        } else if (flexport_is_l3uc_port(unit, p)) {
            BCM_PBMP_PORT_ADD(flex_p->l3uc_pbmp, p);
        }
    }

    /* print */
    flexport_print_ports(unit, flex_p->ipmc_pbmp,
                             "Ports reserved for IMPC traffic");
    flexport_print_ports(unit, flex_p->l3uc_pbmp,
                             "Ports reserved for L3UC traffic");
}

/*
 * Function:
 *      flexport_l3uc_init_env
 * Purpose:
 *      Test init function. Parse CLI params and do necessary init.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     SOC_E_XXXX
 */
static int
flexport_l3uc_init_env(int unit, args_t *a, void **pa)
{
    flexport_mc_t *flex_p = flexport_parray[unit];

    cli_out("\nCalling flexport_l3uc_init_env");

    /* counter */
    BCM_IF_ERROR_RETURN(soc_counter_status(unit, &flex_p->env_counter_flags,
                                                 &flex_p->env_counter_interval,
                                                 &flex_p->env_counter_pbm));
    if (flex_p->env_counter_interval > 0) {
        cli_out("\nDisabling counter collection");
        soc_counter_stop(unit);
    }

    /* linkscan */
    BCM_IF_ERROR_RETURN(bcm_linkscan_enable_get(unit,
                        &flex_p->env_linkscan_enable));
    if (flex_p->env_linkscan_enable) {
        cli_out("\nDisabling linkscan");
        BCM_IF_ERROR_RETURN(bcm_linkscan_enable_set(unit, 0));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      flexport_run_flex
 * Purpose:
 *      flex ports.
 *        -- do flex by calling bcm apis.
 *        -- set test env for up-ports.
 *        -- send traffic
 *        -- measure rate
 *        --
 *        --
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      case_num: flex case number. 0 is reserved for init.
 *
 * Returns:
 *     BCM_E_XXXX
 */
static void
flexport_run_flex(int unit, int case_num)
{
    pbmp_t pbm;
    flexport_mc_t *flex_p = flexport_parray[unit];

    /* config tsc mode for flexports */
    flexport_cfg_test_case(unit, case_num);

    /* prepare resource struct for flexports */
    flexport_cfg_resource(unit);

    /* cal bcm api to flex ports */
    flexport_flex_ports(unit);

    /* print: for non-initial flex, only print up/changed ports */
    if (case_num == 0) {
        pbm = PBMP_PORT_ALL(unit);
    } else {
        pbm = flex_p->flex_lport_up_pbmp;
        SOC_PBMP_OR(pbm, flex_p->flex_lport_ch_pbmp);
    }
    stream_print_port_config(unit, pbm);
}

/*
 * Function:
 *      flexport_td3_test_init
 * Purpose:
 *      Test init function. Parse CLI params and do necessary init.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
flexport_td3_test_init(int unit, args_t *a, void **pa)
{
    flexport_mc_t *flex_p;
    flexport_l3uc_rate_calc_t *rate_calc_p;

    flex_p = flexport_parray[unit];
    flex_p = sal_alloc(sizeof(flexport_mc_t), "ipmc_test");
    sal_memset(flex_p, 0, sizeof(flexport_mc_t));
    flexport_parray[unit] = flex_p;


    rate_calc_p = rate_calc_parray[unit];
    rate_calc_p = sal_alloc(sizeof(flexport_l3uc_rate_calc_t), "rac_calc_p");
    sal_memset(rate_calc_p, 0, sizeof(flexport_l3uc_rate_calc_t));
    rate_calc_parray[unit] = rate_calc_p;


    stream_print_port_config(unit, PBMP_PORT_ALL(unit));
    cli_out("\n==================================================");
    cli_out("\nCalling flexport_test_init ...\n");
    flexport_parse_test_params(unit, a);
    flexport_l3uc_init_env(unit, a, pa);
    flexport_set_chip_dependent_params(unit);

    flex_p->test_fail = 0;

    if (flex_p->bad_input == 1 ||
        flex_p->test_fail == 1) {
        goto done;
    }

    stream_set_mac_lpbk(unit, PBMP_PORT_ALL(unit));
    stream_turn_off_cmic_mmu_bkp(unit);
    stream_turn_off_fc(unit, PBMP_PORT_ALL(unit));

done:
    return 0;
}

/*
 * Function:
 *      flexport_td3_test
 * Purpose:
 *      Set up ports/streams and send packets.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
flexport_td3_test(int unit, args_t *a, void *pa)
{
    int p;
    pbmp_t pbm;
    flexport_mc_t *flex_p = flexport_parray[unit];

    if (flex_p->bad_input == 1 ||
        flex_p->test_fail == 1) {
        goto done;
    }

    cli_out("\n==================================================");
    cli_out("\nCalling flexport_test ... \n");
    /* init port config */
    flexport_run_flex(unit, 0);

    /* divide ports into l3uc usage and impc usage */
    flexport_divide_ports_for_l3uc_ipmc(unit);

    /* 1.1 set traffic for ipmc */
    flexport_ipmc_set_port_property(unit);
    flexport_ipmc_set_up_streams(unit);
    flexport_ipmc_send_pkts(unit);

    /* 1.2 set traffic for l3uc */
    flexport_l3uc_update_stream(unit, flex_p->l3uc_pbmp);

    /* 1.3 check port rate for l3uc and ipmc */
    flexport_l3uc_chk_rate(unit, flex_p->l3uc_pbmp);
    flexport_ipmc_chk_rate(unit);

    /* 2 flex port */
    flexport_run_flex(unit, 1);

    /* 3.1 update ipmc stream */
    flexport_ipmc_update_stream(unit);

    /* 3.2 update l3uc stream*/
    BCM_PBMP_CLEAR(pbm);
    PBMP_ITER(flex_p->flex_lport_up_pbmp, p) {
        if (flexport_is_l3uc_port(unit, p)) SOC_PBMP_PORT_ADD(pbm, p);
    }
    PBMP_ITER(flex_p->flex_lport_ch_pbmp, p) {
        if (flexport_is_l3uc_port(unit, p)) SOC_PBMP_PORT_ADD(pbm, p);
    }
    flexport_l3uc_update_stream(unit, pbm);

    /* 4.1 check counter, rate, integrity for l3uc */
    flexport_l3uc_chk_rate(unit, pbm);
    flexport_l3uc_chk_integrity(unit, pbm);

    /* 4.2 check counter, rate, integrity for ipmc */
    flexport_ipmc_chk_rate(unit);
    flexport_ipmc_chk_integrity(unit);


done:
    return 0;
}

/*
 * Function:
 *      flexport_td3_test_cleanup
 * Purpose:
 *      Do test end checks and free all allocated memory.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
flexport_td3_test_cleanup(int unit, void *pa)
{
    int rv;
    flexport_mc_t *flex_p = flexport_parray[unit];
    flexport_l3uc_rate_calc_t *rate_calc_p = rate_calc_parray[unit];

    if (flex_p->bad_input == 1) {
        flex_p->test_fail = 1;
        goto done;
    }
    cli_out("\nCalling flexport_test_cleanup");

done:

    /* reset env to initial state */
    if (flex_p->env_linkscan_enable) {
        cli_out("\nEnabling linkscan");
        BCM_IF_ERROR_RETURN(bcm_linkscan_mode_set_pbm(unit,
                            PBMP_PORT_ALL(unit), BCM_LINKSCAN_MODE_SW));
        BCM_IF_ERROR_RETURN(bcm_linkscan_enable_set(unit, 1));
    }
    if (flex_p->env_counter_interval > 0) {
        cli_out("\nEnabling counter collection with interval %0d",
                flex_p->env_counter_interval);
        BCM_IF_ERROR_RETURN(soc_counter_start(unit, flex_p->env_counter_flags,
                                                 flex_p->env_counter_interval,
                                                 PBMP_PORT_ALL(unit)));
    }
    bcm_vlan_init(unit);

    /* print */
    rv = (flex_p->test_fail == 1) ? BCM_E_FAIL : BCM_E_NONE;
    cli_out("\n==================================================");
    cli_out("\n==================================================");
    if (flex_p->test_fail == 1) {
        cli_out("\n[Flexport test failed]\n\n");
    } else {
        cli_out("\n[Flexport test passed]\n\n");
    }

    /* free mem */
    sal_free(rate_calc_p);
    sal_free(flex_p->ipmc_exp_rate);
    sal_free(flex_p->ipmc_tpkt_start);
    sal_free(flex_p->ipmc_tpkt_end);
    sal_free(flex_p);

    return rv;
}

#endif /* BCM_ESW_SUPPORT */
