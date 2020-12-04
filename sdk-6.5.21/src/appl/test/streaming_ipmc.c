/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Streaming test to check TDM table and programming. Each port loops traffic
 * back to itself using port bridging and higig lookup for HG ports. Test test
 * calculates expected rates based on port config and oversub ratio and checks
 * against it.
 * Configuration parameters passed from CLI:
 * PktSize: Packet size in bytes. Set to 0 for worst case packet sizes on all
 *          ports (145B for ENET, 76B for HG2). Set to 1 for random packet sizes
 * FloodCnt: Number of packets in each swill. Setting this to 0 will let the
 *           test calculate the number of packets that can be sent to achieve
 *           a lossless swirl at full rate. Set to 0 by default.
 * Setup and Test Sequence:
 * 1. Group all active front panel ports into groups of
 *    (NumL2Copy + NumL3Copy+ 1) ports. Each group constitutes 1 IPMC "stream".
 *    All ports in a stream should have the same line rate, although they may
 *    have different oversub ratios.
 * 2. Let us assume a stream has n ports P0, P1 ... Pn. The ports should be
 *    arranged such that the P1 is the slowest port, i.e. has the highest oversub
 *    ratio and the highest number of same port replications.
 * 3. Turn off all flow control, set MAC loopback on all ports and disable TTL
 *    decrement by setting EGR_IPMC_CFG2.DISABLE_TTL_DECREMENT=1.
 * 4. Randomly decide whether each receiving port in the stream (P1, P2, ... Pn)
 *    should receive L2 or L3 copies. If port P1 receives L2 copies, the IPMC
 *    stream is designated an "L2 stream". If it receives L3 copies the stream
 *    is designated an "L3 stream".
 * 5. For each stream:
 *      a. Place port P0 on VLAN0. Create an empty VLAN called DROP_VLAN with
 *         no ports
 *      b. Add all ports receiving L2 copies to VLAN1 and multicast group MC0
 *         using bcm_multicast_egress_add(). This programs the L2MC table.
 *      c. Add the following VLAN translations:
 *          i. For L2 streams: Port=P2, P3 ... Pn OldVlan=VLAN1 NewVlan=DROP_VLAN
 *          ii. For L3 streams: Port=P1, P2 ... Pn OldVlan=VLAN1 NewVlan=DROP_VLAN
 *          iii. Port=P0 OldVlan=VLAN0 NewVlan=VLAN1
 *          iv. For L2 streams: Port=P1 OldVlan=VLAN1 NewVlan=DROP_VLAN
 *      d. For each port Pk receiving L3 copies, let the number of L3 copies received at a port be m (=NumRep+1).
 *          i. Create m VLANs (VLANk0, VLANk1, .... VLANk(m-1)), each with port Pk
 *          ii. For L3 streams: If k=1, VLAN2=VLANk0
 *          iii. For L3 streams: Create the following VLAN translation: Port=Pk OldVlan=VLAN2 NewVlan=VLAN0
 *          iv. For L2 streams: Create the following VLAN translation: Port=Pk OldVlan= VLANk0, VLANk1, ... VLANk(m-1)) NewVlan=DROP_VLAN
 *          v. Create m L3 interfaces I0, I1 ... I(m-1) with VLANk0, VLANk1, .... VLANk(m-1)
 *          vi. Add interface to multicat group MC1 using bcm_multicast_egress_add().
 *          vii. Program IPMC table with necessary SIP, DIP and multicast group MC1.
 * 6. Send packets from CPU port on VLAN0 of each stream to flood the stream.
 *    If FloodCnt=0, the test will determine the number of packets to send based.
 *    These values are obtained empirically through trial and error for LR ports
 *    for NumCopy=2 and adjusted for oversub configs based on oversub ratio.
 *    Embed the packet size and random seed used to generate each packet in the
 *    payload.
 * 7. Allow the packets to swill for a fixed amount of time decided by RunTime.
 * 8. Snoop back all packets in each stream to the CPU using the following sequence:
 *      a. Program CPU_CONTROL_0.UVLAN_TOCPU=1. This sends all packets with an unknown VLAN to CPU.
 *      b. Invalidate VLAN0 for all streams by setting VALID=0 in the VLAN table
 * 9. Decode each received packet to obtain the random seed and packet size.
 *    Reconstruct the expected packet and check the received packet for integrity.
 * Checks:
 * 1. Packet counts on each port.
 * 2. No packet drops on any stream for LR configs.
 * 3. All packets in a given stream snooped back to CPU port and checked for integrity.
 * 
 * Test Limitations
 * 1. FloodCnt automatically computed in the test supports packet sizes upto
 *    1518B and NumCopy=2. Any deviations may result in packet drops for LR
 *    configs or false rate errors being reported. For custom configs the user is
 *    encouraged to tweak the FloodCnt value as per their needs.
 *
 * PktSize : Packet size in bytes not including HG2 header. Set to 0 for worst
 *           case packet sizes (145B for Ethernet and 76B for HG2). Set to 1 for
 *           random packet sizes.
 * FloodCnt: Number of packets sent from CPU to flood each stream (packet swill).
 *           Set to 0 for test to automatically calculate.
 * NumL2Copy: Number of ports receiving L2 copies in each stream (2 by default).
 * NumL3Copy:  Number of ports receiving L3 copies in each stream (2 by default).
 * NumRep: Number of same-port replications on ports receiving . Set to 0 for
 *         random.
 * MaxNumRep: Max number of replications if NumRep=0. Set both MaxNumRep and
 *            NumRep to 0 for no same port replication.
 * MaxNumCells: Max number of cells for random packet sizes. Default = 4.
 *              Set to 0 for random.
 * RunTime: Interval in seconds over which swill is allowed to
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

#define TARGET_PKT_COUNT 100
#define MAX_FP_PORTS 130

#define TTL 5

#define DROP_VLAN 0xabc
#define FIRST_MC_GROUP 0x2000001
#define FIRST_INTF_ID 0x2

#define SIP 0x02030405
#define DIP 0xe0020304

#define MAX_PKTS_PER_STREAM 30
#define PKT_COUNT_CHECK_TOL 5

#define PKT_SIZE_PARAM_DEFAULT 0
#define FLOOD_PKT_CNT_PARAM_DEFAULT 0
#define NUM_L2_COPY_PARAM_DEFAULT 2
#define NUM_L3_COPY_PARAM_DEFAULT 2
#define NUM_REP_PARAM_DEFAULT 1
#define MAX_NUM_REP_PARAM_DEFAULT 3
#define RUN_TIME_PARAM_DEFAULT 10
#define MAX_NUM_CELLS_PARAM_DEFAULT 4
#define SPEEDUP_FACTOR_PARAM_DEFAULT 1
#define CPU_PORT 0

#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_L3) && \
      (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))

typedef struct ipmc_s {
    uint32 num_fp_ports;
    uint32 num_streams;
    int *port_speed;
    int *port_list;
    int **stream;
    uint32 *num_rep;
    uint32 *l3_copy;
    uint32 *tx_vlan;
    uint32 *exp_vlan;
    uint8 (*exp_mac_addr)[NUM_BYTES_MAC_ADDR];
    uint32 *port_oversub;
    uint32 oversub_config;
    uint32 *port_used;
    uint32 num_pipes;
    uint32 total_chip_bw;
    uint32 bw_per_pipe;
    uint32 *ovs_ratio_x1000;
    uint64 *exp_rate;
    uint64 *tpkt_start;
    uint64 *tpkt_end;
    uint32 **rand_pkt_sizes;
    uint32 pkt_size_param;
    uint32 flood_pkt_cnt_param;
    uint32 num_l2_copy_param;
    uint32 num_l3_copy_param;
    uint32 check_packet_integrity_param;
    uint32 num_rep_param;
    uint32 max_num_rep_param;
    uint32 max_num_cells_param;
    uint32 run_time_param;
    uint32 scaling_factor_param;
    uint32 bad_input;
    int test_fail;
    uint32 pkt_seed;
    uint32 num_copy;
} ipmc_t;

static ipmc_t *ipmc_parray[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *      ipmc_parse_test_params
 * Purpose:
 *      Parse CLI parameters, create test structure and flag bad inputs.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     Nothing
 * Notes:
 *      ipmc_p->bad_input set from here - tells test to crash out in case
 *      CLI input combination is invalid.
 */
static void
ipmc_parse_test_params(int unit, args_t *a)
{
    parse_table_t parse_table;
    ipmc_t *ipmc_p = ipmc_parray[unit];

    const char tr513_test_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nDocumentation too long to be displayed with -pedantic compiler\n";
#else
    "Streaming test to check TDM table and programming. Each port loops traffic\n"
    "back to itself using port bridging and higig lookup for HG ports. Test test\n"
    "calculates expected rates based on port config and oversub ratio and checks\n"
    "against it.\n"
    "Configuration parameters passed from CLI:\n"
    "PktSize: Packet size in bytes. Set to 0 for worst case packet sizes on all\n"
    "        ports (145B for ENET, 76B for HG2). Set to 1 for random packet sizes\n"
    "FloodCnt: Number of packets in each swill. Setting this to 0 will let the\n"
    "        test calculate the number of packets that can be sent to achieve\n"
    "        a lossless swirl at full rate. Set to 0 by default.\n"
    "Setup and Test Sequence:\n"
    "1. Group all active front panel ports into groups of\n"
    "    (NumL2Copy + NumL3Copy+ 1) ports. Each group constitutes 1 IPMC \"stream\".\n"
    "    All ports in a stream should have the same line rate, although they may\n"
    "    have different oversub ratios.\n"
    "2. Let us assume a stream has n ports P0, P1 ... Pn. The ports should be\n"
    "    arranged such that the P1 is the slowest port, i.e. has the highest oversub\n"
    "    ratio and the highest number of same port replications.\n"
    "3. Turn off all flow control, set MAC loopback on all ports and disable TTL\n"
    "    decrement by setting EGR_IPMC_CFG2.DISABLE_TTL_DECREMENT=1.\n"
    "4. Randomly decide whether each receiving port in the stream (P1, P2, ... Pn)\n"
    "    should receive L2 or L3 copies. If port P1 receives L2 copies, the IPMC\n"
    "    stream is designated an \"L2 stream\". If it receives L3 copies the stream\n"
    "    is designated an \"L3 stream\".\n"
    "5. For each stream:\n"
    "    a. Place port P0 on VLAN0. Create an empty VLAN called DROP_VLAN with\n"
    "        no ports\n"
    "    b. Add all ports receiving L2 copies to VLAN1 and multicast group MC0\n"
    "        using bcm_multicast_egress_add(). This programs the L2MC table.\n"
    "    c. Add the following VLAN translations:\n"
    "        i. For L2 streams: Port=P2, P3 ... Pn OldVlan=VLAN1 NewVlan=DROP_VLAN\n"
    "        ii. For L3 streams: Port=P1, P2 ... Pn OldVlan=VLAN1 NewVlan=DROP_VLAN\n"
    "        iii. Port=P0 OldVlan=VLAN0 NewVlan=VLAN1\n"
    "        iv. For L2 streams: Port=P1 OldVlan=VLAN1 NewVlan=DROP_VLAN\n"
    "    d. For each port Pk receiving L3 copies, let the number of L3 copies received at a port be m (=NumRep+1).\n"
    "        i. Create m VLANs (VLANk0, VLANk1, .... VLANk(m-1)), each with port Pk\n"
    "        ii. For L3 streams: If k=1, VLAN2=VLANk0\n"
    "        iii. For L3 streams: Create the following VLAN translation: Port=Pk OldVlan=VLAN2 NewVlan=VLAN0\n"
    "        iv. For L2 streams: Create the following VLAN translation: Port=Pk OldVlan= VLANk0, VLANk1, ... VLANk(m-1)) NewVlan=DROP_VLAN\n"
    "        v. Create m L3 interfaces I0, I1 ... I(m-1) with VLANk0, VLANk1, .... VLANk(m-1)\n"
    "        vi. Add interface to multicat group MC1 using bcm_multicast_egress_add().\n"
    "        vii. Program IPMC table with necessary SIP, DIP and multicast group MC1.\n"
    "6. Send packets from CPU port on VLAN0 of each stream to flood the stream.\n"
    "    If FloodCnt=0, the test will determine the number of packets to send based.\n"
    "    These values are obtained empirically through trial and error for LR ports\n"
    "    for NumCopy=2 and adjusted for oversub configs based on oversub ratio.\n"
    "    Embed the packet size and random seed used to generate each packet in the\n"
    "    payload.\n"
    "7. Allow the packets to swill for a fixed amount of time decided by RunTime.\n"
    "8. Snoop back all packets in each stream to the CPU using the following sequence:\n"
    "    a. Program CPU_CONTROL_0.UVLAN_TOCPU=1. This sends all packets with an unknown VLAN to CPU.\n"
    "    b. Invalidate VLAN0 for all streams by setting VALID=0 in the VLAN table\n"
    "9. Decode each received packet to obtain the random seed and packet size.\n"
    "    Reconstruct the expected packet and check the received packet for integrity.\n"
    "Checks:\n"
    "1. Packet counts on each port.\n"
    "2. No packet drops on any stream for LR configs.\n"
    "3. All packets in a given stream snooped back to CPU port and checked for integrity.\n"
    "\n"
    " Test Limitations\n"
    " 1. FloodCnt automatically computed in the test supports packet sizes upto\n"
    "    1518B and NumCopy=2. Any deviations may result in packet drops for LR\n"
    "    configs or false rate errors being reported. For custom configs the user is\n"
    "    encouraged to tweak the FloodCnt value as per their needs.\n"
    "\n"
    " PktSize : Packet size in bytes not including HG2 header. Set to 0 for worst\n"
    "           case packet sizes (145B for Ethernet and 76B for HG2). Set to 1 for\n"
    "           random packet sizes.\n"
    " FloodCnt: Number of packets sent from CPU to flood each stream (packet swill).\n"
    "           Set to 0 for test to automatically calculate.\n"
    " NumL2Copy: Number of ports receiving L2 copies in each stream (2 by default).\n"
    " NumL3Copy:  Number of ports receiving L3 copies in each stream (2 by default).\n"
    " NumRep: Number of same-port replications on ports receiving . Set to 0 for\n"
    "         random.\n"
    " MaxNumRep: Max number of replications if NumRep=0. Set both MaxNumRep and\n"
    "            NumRep to 0 for no same port replication.\n"
    " MaxNumCells: Max number of cells for random packet sizes. Default = 4.\n"
    "              Set to 0 for random.\n"
    " RunTime: Interval in seconds over which swill is allowed to\n"
    "          run (10s by default).\n";
#endif
    ipmc_p->bad_input = 0;

    ipmc_p->pkt_size_param = PKT_SIZE_PARAM_DEFAULT;
    ipmc_p->flood_pkt_cnt_param = FLOOD_PKT_CNT_PARAM_DEFAULT;
    ipmc_p->num_l2_copy_param = NUM_L2_COPY_PARAM_DEFAULT;
    ipmc_p->num_l3_copy_param = NUM_L3_COPY_PARAM_DEFAULT;
    ipmc_p->num_rep_param = NUM_REP_PARAM_DEFAULT;
    ipmc_p->max_num_rep_param = MAX_NUM_REP_PARAM_DEFAULT;
    ipmc_p->max_num_cells_param = MAX_NUM_CELLS_PARAM_DEFAULT;
    ipmc_p->run_time_param = RUN_TIME_PARAM_DEFAULT;
    ipmc_p->scaling_factor_param = SPEEDUP_FACTOR_PARAM_DEFAULT;

    /*Parse CLI opts */

    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table, "PktSize", PQ_INT|PQ_DFL, 0,
                    &(ipmc_p->pkt_size_param), NULL);
    parse_table_add(&parse_table, "FloodCnt", PQ_INT|PQ_DFL, 0,
                    &(ipmc_p->flood_pkt_cnt_param), NULL);
    parse_table_add(&parse_table, "NumL2Copy", PQ_INT|PQ_DFL, 0,
                    &(ipmc_p->num_l2_copy_param), NULL);
    parse_table_add(&parse_table, "NumL3Copy", PQ_INT|PQ_DFL, 0,
                    &(ipmc_p->num_l3_copy_param), NULL);
    parse_table_add(&parse_table, "NumRep", PQ_INT|PQ_DFL, 0,
                    &(ipmc_p->num_rep_param), NULL);
    parse_table_add(&parse_table, "MaxNumRep", PQ_INT|PQ_DFL, 0,
                    &(ipmc_p->max_num_rep_param), NULL);
    parse_table_add(&parse_table, "MaxNumCells", PQ_INT|PQ_DFL, 0,
                    &(ipmc_p->max_num_cells_param), NULL);
    parse_table_add(&parse_table, "RunTime", PQ_INT|PQ_DFL, 0,
                    &(ipmc_p->run_time_param), NULL);
    parse_table_add(&parse_table, "ScalingFactor", PQ_INT|PQ_DFL, 0,
                    &(ipmc_p->scaling_factor_param), NULL);

    if (parse_arg_eq(a, &parse_table) < 0 || ARG_CNT(a) != 0) {
        test_msg("%s", tr513_test_usage);
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        ipmc_p->bad_input = 1;
        parse_arg_eq_done(&parse_table);
    } else {
        cli_out("\n");
        cli_out("------------- PRINTING TEST PARAMS ------------------\n");
        cli_out("PktSize      = %0d\n", ipmc_p->pkt_size_param);
        cli_out("FloodCnt     = %0d\n", ipmc_p->flood_pkt_cnt_param);
        cli_out("NumL2Copy    = %0d\n", ipmc_p->num_l2_copy_param);
        cli_out("NumL3Copy    = %0d\n", ipmc_p->num_l3_copy_param);
        cli_out("NumRep       = %0d\n", ipmc_p->num_rep_param);
        cli_out("MaxNumRep    = %0d\n", ipmc_p->max_num_rep_param);
        cli_out("MaxNumCells  = %0d\n", ipmc_p->max_num_cells_param);
        cli_out("RunTime      = %0d\n", ipmc_p->run_time_param);
        cli_out("ScalingFactor= %0d\n", ipmc_p->scaling_factor_param);
        cli_out("-----------------------------------------------------\n");
    }

    if (ipmc_p->max_num_cells_param == 0) {
        /* coverity[dont_call : FALSE] */
        ipmc_p->max_num_cells_param = (sal_rand() % (MTU_CELL_CNT - 1)) + 1;
    }

    if (ipmc_p->pkt_size_param == 0) {
        cli_out
            ("\nUsing worst case packet sizes - 145B for Ethernet "
             "and 76B (64B + 12B hg-hdr) for HG2");
    } else if (ipmc_p->pkt_size_param == 1) {
        cli_out("\nUsing random packet sizes");
    } else if (ipmc_p->pkt_size_param < MIN_PKT_SIZE) {
        test_error(unit,"\n*ERROR: Packet size cannot be lower than %0dB\n",
                MIN_PKT_SIZE);
        ipmc_p->bad_input = 1;
    } else if (ipmc_p->pkt_size_param > MTU) {
        test_error(unit,"\n*ERROR: Packet size cannot be higher than %0dB (Device MTU)\n",
                MTU);
        ipmc_p->bad_input = 1;
    }

    if (ipmc_p->flood_pkt_cnt_param == 0) {
        cli_out("\nFloodCnt=0, test will automatically calculate number of"
                " packets to flood each port");
    }

    if (ipmc_p->num_rep_param > MAX_NUM_REP_PARAM_DEFAULT) {
        ipmc_p->max_num_rep_param = MAX_NUM_REP_PARAM_DEFAULT;
        cli_out("\nMax replications supported by this test = %0d,"
                "forcing number of replications to %0d",
                MAX_NUM_REP_PARAM_DEFAULT, MAX_NUM_REP_PARAM_DEFAULT);
    }

    ipmc_p->num_copy = ipmc_p->num_l2_copy_param + ipmc_p->num_l3_copy_param;
}

/*
 * Function:
 *      ipmc_set_lowest_stream_port
 * Purpose:
 *      Within each stream, put the port with the lowest speed
 *      in the 2nd position.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
static void
ipmc_set_lowest_stream_port(int unit)
{
    int i, j, tmp, port, port_speed, min_speed, min_speed_idx;
    ipmc_t *ipmc_p = ipmc_parray[unit];

    for (i = 0; i < ipmc_p->num_streams; i++) {
        min_speed_idx = 1;
        port = ipmc_p->port_list[ipmc_p->stream[i][min_speed_idx]];
        bcm_port_speed_get(unit, port, &min_speed);

        for (j = 0; j < (ipmc_p->num_copy + 1); j++) {
            port = ipmc_p->port_list[ipmc_p->stream[i][j]];
            bcm_port_speed_get(unit, port, &port_speed);
            if (port_speed < min_speed) {
                min_speed = port_speed;
                min_speed_idx = j;
            }
        }
        if (min_speed_idx != 1) {
            tmp = ipmc_p->stream[i][1];
            ipmc_p->stream[i][1] =  ipmc_p->stream[i][min_speed_idx];
            ipmc_p->stream[i][min_speed_idx] = tmp;
        }
    }
}

/*
 * Function:
 *      ipmc_set_port_property
 * Purpose:
 *      Set port_list, port_speed, port_oversub arrays and l3_copy.
 *      Also call set_exp_rates.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing.
 */
static void
ipmc_set_port_property(int unit)
{
    int i, j, k, p, picked_port;
    uint32 temp, pkt_size, ports_found = 0, ports_picked = 0,
           ports_accounted_for = 0, stream_possible = 0;
    soc_info_t *si = &SOC_INFO(unit);
    ipmc_t *ipmc_p = ipmc_parray[unit];

    ipmc_p->num_fp_ports = 0;
    ipmc_p->num_streams = 0;
    ipmc_p->oversub_config = 0;

    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        ipmc_p->num_fp_ports++;
    }

    ipmc_p->port_list =
        (int *)sal_alloc(ipmc_p->num_fp_ports * sizeof(int),
                         "port_list");
    ipmc_p->port_speed =
        (int *)sal_alloc(ipmc_p->num_fp_ports * sizeof(int),
                         "port_speed_array");
    ipmc_p->port_oversub =
        (uint32 *) sal_alloc(ipmc_p->num_fp_ports * sizeof(uint32),
                             "port_oversub_array");

    ipmc_p->l3_copy =
        (uint32 *) sal_alloc(ipmc_p->num_fp_ports * sizeof(uint32),
                             "l3_copy");
    ipmc_p->rand_pkt_sizes =
        (uint32 **) sal_alloc(ipmc_p->num_fp_ports * sizeof(uint32 *),
                              "rand_pkt_sizes_array*");

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        ipmc_p->rand_pkt_sizes[i] =
            (uint32 *) sal_alloc(TARGET_PKT_COUNT * sizeof(uint32),
                                "rand_pkt_sizes_array");
    }

    ipmc_p->num_rep =
        (uint32 *) sal_alloc(ipmc_p->num_fp_ports * sizeof(uint32),
                              "num_rep_array");

    ipmc_p->stream =
        (int **) sal_alloc(ipmc_p->num_fp_ports * sizeof(int *),
                              "stream_array*");

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        ipmc_p->stream[i] =
            (int *) sal_alloc((ipmc_p->num_copy + 1) * sizeof(int),
                                "stream_array");
    }
    ipmc_p->port_used =
        (uint32 *) sal_alloc(ipmc_p->num_fp_ports * sizeof(uint32),
                             "ipmc_p->port_used_array");

    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        ipmc_p->port_list[i] = p;
        i++;
    }

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        ipmc_p->port_speed[i] =
            si->port_speed_max[ipmc_p->port_list[i]];

        switch(ipmc_p->port_speed[i]) {
            case 11000: ipmc_p->port_speed[i] = 10600; break;
            case 27000: ipmc_p->port_speed[i] = 26500; break;
            case 42000: ipmc_p->port_speed[i] = 42400; break;
            default: break;
        }
    }

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        if (SOC_PBMP_MEMBER(si->oversub_pbm, ipmc_p->port_list[i]) &&
            (!(SOC_PBMP_MEMBER(si->management_pbm, ipmc_p->port_list[i])))) {
            ipmc_p->oversub_config = 1;
            ipmc_p->port_oversub[i] = 1;
        } else {
            ipmc_p->port_oversub[i] = 0;
        }
    }

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        ipmc_p->port_used[i] = 0;
    }

    while (ports_accounted_for < ipmc_p->num_fp_ports) {
        do {
            i = sal_rand() % ipmc_p->num_fp_ports;
        }while (ipmc_p->port_used[i] != 0);
        stream_possible = 0;
        ports_found = 0;
        for (j = 0;j < ipmc_p->num_fp_ports; j++) {
            if((ipmc_p->port_used[j] == 0)
                    && (ipmc_p->port_speed[i]
                                == ipmc_p->port_speed[j])
                    && (i!=j)) {
                ports_found++;
            }

            if (ports_found >= ipmc_p->num_copy) {
                stream_possible = 1;
            }
        }

        ports_accounted_for++;
        if(stream_possible == 1) {
            ipmc_p->port_used[i] = 1;
            ports_picked = 0;
            ipmc_p->stream[ipmc_p->num_streams][0] = i;
            while (ports_picked < ipmc_p->num_copy) {
                do {
                    picked_port = sal_rand() % ipmc_p->num_fp_ports;
                }while((ipmc_p->port_speed[i] !=
                                ipmc_p->port_speed[picked_port])
                        || (ipmc_p->port_used[picked_port] != 0));

                ports_picked++;
                ipmc_p->port_used[picked_port] = 1;
                ipmc_p->stream[ipmc_p->num_streams][ports_picked]
                                                            = picked_port;
                ports_accounted_for++;
            }
            ipmc_p->num_streams++;
        }
    }
    ipmc_set_lowest_stream_port(unit);

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        ipmc_p->l3_copy[i] = 0;
    }
    for (i = 0; i < ipmc_p->num_streams; i++) {
        for (j = 0; j < ipmc_p->num_l3_copy_param; j++) {
            do {
                k = (sal_rand() % ipmc_p->num_copy) + 1;
            } while(ipmc_p->l3_copy[ipmc_p->stream[i][k]] == 1);

            ipmc_p->l3_copy[ipmc_p->stream[i][k]] = 1;
        }
    }

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        if (ipmc_p->l3_copy[i] == 1) {
            if (ipmc_p->num_rep_param == 0) {
                ipmc_p->num_rep[i] = sal_rand() %
                                     (ipmc_p->max_num_rep_param + 1);
            } else {
                ipmc_p->num_rep[i] = ipmc_p->num_rep_param;
            }
        }
        else {
            ipmc_p->num_rep[i] = 0;
        }
    }

    for (i = 0; i < ipmc_p->num_streams; i++) {
        for (j = 0; j < (ipmc_p->num_copy + 1); j++) {
            if (ipmc_p->l3_copy[ipmc_p->stream[i][1]] == 0) {
                ipmc_p->num_rep[ipmc_p->stream[i][j]] = 0;
            } else if (ipmc_p->num_rep[ipmc_p->stream[i][j]] >
                            ipmc_p->num_rep[ipmc_p->stream[i][1]]) {
                temp = ipmc_p->num_rep[ipmc_p->stream[i][j]];
                ipmc_p->num_rep[ipmc_p->stream[i][j]]
                                = ipmc_p->num_rep[ipmc_p->stream[i][1]];
                ipmc_p->num_rep[ipmc_p->stream[i][1]] = temp;
            }
        }
    }

    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        for (j = 0; j < TARGET_PKT_COUNT; j++) {
            do {
                /* coverity[dont_call : FALSE] */
                pkt_size = (sal_rand() % (MTU - MIN_PKT_SIZE + 1)) + MIN_PKT_SIZE;
            } while (stream_get_pkt_cell_cnt(unit, pkt_size, ipmc_p->port_list[i]) >
                            ipmc_p->max_num_cells_param);
            ipmc_p->rand_pkt_sizes[i][j] = pkt_size;
        }
    }

    {
        /* print */
        int port_idx;
        cli_out("\n=========== STREAM INFO ============\n");
        cli_out("%4s %4s \t[%s]%4s %s/%s\n",
                "strm", "type", "j", "port", "type", "num_l3cpy");

        for (i = 0; i < ipmc_p->num_streams; i++) {
            cli_out("%4d ", i);
            if (ipmc_p->l3_copy[ipmc_p->stream[i][1]] == 0) {
                cli_out("%4s ", "L2");
            } else {
                cli_out("%4s ", "L3");
            }
            for (j = 0; j < (ipmc_p->num_copy + 1); j++) {
                port_idx = ipmc_p->stream[i][j];
                cli_out("\t[%d]%4d ", j, ipmc_p->port_list[port_idx]);
                if (ipmc_p->l3_copy[port_idx] == 0) {
                    cli_out("%s/%d", "L2", 0);
                } else {
                    cli_out("%s/%d", "L3", (ipmc_p->num_rep[port_idx] + 1));
                }
            }
            cli_out("\n");
        }
    }
}

/*
 * Function:
 *      ipmc_set_up_streams
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
ipmc_set_up_streams(int unit)
{
    int i, j, k, p;
    pbmp_t pbm, ubm;
    uint32 vlan = VLAN;
    uint32 vlan_0 = 0, vlan_1 = 0, vlan_2 = 0;
    uint32 l3_stream;
    bcm_multicast_t mc_group;
    uint32 mc_flags = 0;
    bcm_l3_intf_t l3_intf;
    bcm_if_t intf_id;
    bcm_if_t encap_id;
    uint32 intf_flags = 0;
    uint8 intf_mac[] = {0x00,0x00,0x00,0x00,0x00,0x00};
    uint8 mac_0[NUM_BYTES_MAC_ADDR];
    bcm_ipmc_addr_t ipmc_addr;
    uint32 ipmc_flags = 0;
    uint8 mac_sa[] = MAC_SA;
    bcm_gport_t gport;
    ipmc_t *ipmc_p = ipmc_parray[unit];

    ipmc_p->tx_vlan = (uint32*) sal_alloc(ipmc_p->num_streams * sizeof(uint32),
                                          "tx_vlan");

    ipmc_p->exp_vlan = (uint32*) sal_alloc(ipmc_p->num_streams * sizeof(uint32),
                                          "exp_vlan");

    ipmc_p->exp_mac_addr = sal_alloc(ipmc_p->num_streams *
                                        sizeof(uint8) * NUM_BYTES_MAC_ADDR,
                                                "exp_mac_addr");
    BCM_PBMP_CLEAR(ubm);

    bcm_vlan_destroy_all(unit);
    bcm_vlan_control_set(unit, bcmVlanTranslate, TRUE);
    (void) bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE);
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        (void) bcm_port_control_set(unit, p, bcmPortControlIP4, TRUE);
        (void) bcm_port_control_set(unit, p, bcmPortControlIP6, TRUE);
        (void) bcm_port_control_set(unit, p, bcmSwitchL3McIdxRetType, TRUE);
        (void) bcm_port_control_set(unit, p,
                                    bcmSwitchIpmcReplicationSharing, TRUE);
    }

    bcm_ipmc_enable(unit, TRUE);
    bcm_vlan_create(unit, (bcm_vlan_t)(DROP_VLAN));

    mc_group = (bcm_multicast_t)(FIRST_MC_GROUP);
    mc_flags = BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID;
    intf_id  = (bcm_if_t)(FIRST_INTF_ID);
    intf_flags |= BCM_L3_WITH_ID;
    ipmc_flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;

    for (i = 0; i < ipmc_p->num_streams; i++) {
        if (ipmc_p->l3_copy[ipmc_p->stream[i][1]] == 0) {
            l3_stream = 0;
        } else {
            l3_stream = 1;
        }
        (void) bcm_multicast_create(unit, mc_flags, &mc_group);
        bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
        BCM_PBMP_CLEAR(pbm);
        BCM_PBMP_PORT_ADD(pbm, ipmc_p->port_list[ipmc_p->stream[i][0]]);
        bcm_vlan_port_add(unit, (bcm_vlan_t)(vlan), pbm, ubm);
        vlan_0 = vlan;
        vlan++;

        bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
        BCM_PBMP_CLEAR(pbm);
        for (j = 0; j < (ipmc_p->num_copy + 1); j++) {
            if (ipmc_p->l3_copy[ipmc_p->stream[i][j]] == 0) {
                BCM_PBMP_PORT_ADD(pbm, ipmc_p->port_list[ipmc_p->stream[i][j]]);
                BCM_GPORT_LOCAL_SET(gport,
                                    ipmc_p->port_list[ipmc_p->stream[i][j]]);
                bcm_multicast_egress_add(unit, mc_group, gport,(bcm_if_t)(-1));
                if ((j > 0 && l3_stream == 1) || (j > 1 && l3_stream == 0)) {
                    bcm_vlan_translate_add(unit,
                                    ipmc_p->port_list[ipmc_p->stream[i][j]],
                                    (bcm_vlan_t)(vlan),
                                    (bcm_vlan_t)(DROP_VLAN), 0);
                }
            }
        }
        bcm_vlan_port_add(unit, (bcm_vlan_t)(vlan), pbm, ubm);
        vlan_1 = vlan;
        vlan++;

        bcm_vlan_translate_add(unit, ipmc_p->port_list[ipmc_p->stream[i][0]],
                               (bcm_vlan_t)(vlan_0), (bcm_vlan_t)(vlan_1), 0);

        if (l3_stream == 0) {
            bcm_vlan_translate_add(unit, ipmc_p->port_list[ipmc_p->stream[i][1]],
                               (bcm_vlan_t)(vlan_1), (bcm_vlan_t)(vlan_0), 0);
        } else {
            bcm_vlan_translate_add(unit, ipmc_p->port_list[ipmc_p->stream[i][1]],
                               (bcm_vlan_t)(vlan_1), (bcm_vlan_t)(DROP_VLAN), 0);
        }

        for (j = 0; j < (ipmc_p->num_copy + 1); j++) {
            if (ipmc_p->l3_copy[ipmc_p->stream[i][j]] == 1) {
                for (k = 0;
                     k < (ipmc_p->num_rep[ipmc_p->stream[i][j]] + 1);
                     k++) {
                    BCM_PBMP_CLEAR(pbm);
                    BCM_PBMP_PORT_ADD(pbm,
                                      ipmc_p->port_list[ipmc_p->stream[i][j]]);
                    bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
                    bcm_vlan_port_add(unit, (bcm_vlan_t)(vlan), pbm, ubm);
                    intf_mac[NUM_BYTES_MAC_ADDR - 1] = intf_id;
                    if (l3_stream == 1 && j == 1 && k == 0) {
                        vlan_2 = vlan;
                        bcm_vlan_translate_add(unit,
                                    ipmc_p->port_list[ipmc_p->stream[i][1]],
                                    (bcm_vlan_t)(vlan_2),
                                    (bcm_vlan_t)(vlan_0), 0);
                        sal_memcpy(mac_0, intf_mac, 6);
                    }
                    else {
                        bcm_vlan_translate_add(unit,
                                    ipmc_p->port_list[ipmc_p->stream[i][j]],
                                    (bcm_vlan_t)(vlan),
                                    (bcm_vlan_t)(DROP_VLAN), 0);
                    }
                    bcm_l3_intf_t_init(&l3_intf);
                    sal_memcpy(l3_intf.l3a_mac_addr, intf_mac, 6);
                    l3_intf.l3a_vid = vlan;
                    l3_intf.l3a_intf_id = intf_id;
                    l3_intf.l3a_flags = intf_flags;
                    bcm_l3_intf_create(unit, &l3_intf);
                    bcm_multicast_l3_encap_get(unit, mc_group,
                                       ipmc_p->port_list[ipmc_p->stream[i][j]],
                                       intf_id, &encap_id);
                    BCM_GPORT_LOCAL_SET(gport,
                                    ipmc_p->port_list[ipmc_p->stream[i][j]]);
                    bcm_multicast_egress_add(unit, mc_group, gport, encap_id);
                    vlan++;
                    intf_id++;
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

        ipmc_p->tx_vlan[i] = vlan_0;

        if (l3_stream == 0) {
            ipmc_p->exp_vlan[i] = vlan_1;
            sal_memcpy(ipmc_p->exp_mac_addr[i], mac_sa, 6);
        } else {
            ipmc_p->exp_vlan[i] = vlan_2;
            sal_memcpy(ipmc_p->exp_mac_addr[i], mac_0, 6);
        }
    }
}

/*
 * Function:
 *      ipmc_lossless_flood_cnt
 * Purpose:
 *      Calculates number of packets that need to be sent to a port for a
 *      lossless swirl.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pkt_size : Packet size in bytes
 *      port: Test port no
 * Returns:
 *     Number of packets needed for lossless flooding
 */
static uint32
ipmc_lossless_flood_cnt(int unit, int port)
{
    uint32 pkt_size, flood_cnt = 0;
    int param_flood_cnt, param_pkt_size;
    ipmc_t *ipmc_p = ipmc_parray[unit];

    param_flood_cnt = ipmc_p->flood_pkt_cnt_param;
    param_pkt_size = ipmc_p->pkt_size_param;

    if (param_flood_cnt == 0) {
        if (param_pkt_size == 0) {
            pkt_size = ENET_WC_PKT_SIZE;
        } else {
            pkt_size = param_pkt_size;
        }
        flood_cnt = stream_get_ll_flood_cnt(unit, port, pkt_size, NULL);
    } else {
        flood_cnt = param_flood_cnt;
    }

    if (flood_cnt > MAX_PKTS_PER_STREAM) {
        flood_cnt = MAX_PKTS_PER_STREAM;
    }
    return (flood_cnt);
}

/*
 * Function:
 *      ipmc_send_pkts
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
ipmc_send_pkts(int unit)
{
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    uint32 pkt_size = 0, pkt_count = 0, flood_cnt = 0;
    int i, port;
    uint32 use_random_packet_sizes = 0;
    ipmc_t *ipmc_p = ipmc_parray[unit];
    stream_pkt_t *tx_pkt;

    tx_pkt = sal_alloc(sizeof(stream_pkt_t), "tx_pkt");
    sal_memset(tx_pkt, 0, sizeof(stream_pkt_t));

    if (ipmc_p->pkt_size_param == 1) {
        use_random_packet_sizes = 1;
    }

    cli_out("\n==================================================\n");
    cli_out("\nSending packets ...\n\n");
    for (i = 0; i < ipmc_p->num_streams; i++) {
        port = ipmc_p->port_list[ipmc_p->stream[i][0]];
        flood_cnt = ipmc_lossless_flood_cnt(unit, port);
        if (ipmc_p->pkt_size_param == 0) {
            pkt_size = ENET_WC_PKT_SIZE;
        } else {
            pkt_size = ipmc_p->pkt_size_param;
        }

        tx_pkt->port = port;
        tx_pkt->num_pkt = flood_cnt;
        tx_pkt->pkt_seed = ipmc_p->pkt_seed + i;
        tx_pkt->pkt_size = pkt_size;
        tx_pkt->rand_pkt_size_en = use_random_packet_sizes;
        tx_pkt->rand_pkt_size = NULL;
        tx_pkt->tx_vlan = ipmc_p->tx_vlan[i];
        sal_memcpy(tx_pkt->mac_da, mac_da, NUM_BYTES_MAC_ADDR);
        sal_memcpy(tx_pkt->mac_sa, mac_sa, NUM_BYTES_MAC_ADDR);
        if (tx_pkt->rand_pkt_size_en) {
            tx_pkt->rand_pkt_size = ipmc_p->rand_pkt_sizes[port];
        }
        tx_pkt->l3_en   = 1;
        tx_pkt->ipv6_en = 0;
        tx_pkt->ip_da   = DIP;
        tx_pkt->ip_sa   = SIP;
        tx_pkt->ttl     = TTL;
        stream_tx_pkt(unit, tx_pkt);

        /* print */
        cli_out("flood_cnt for stream %0d = %0d\n", i, tx_pkt->cnt_pkt);
        pkt_count += tx_pkt->cnt_pkt;
    }

    cli_out("\nTotal packet sent: %0d\n\n", pkt_count);
    sal_free(tx_pkt);
}

/*
 * Function:
 *      ipmc_set_up_ports
 * Purpose:
 *      Disable TTL decrement and TTL check. This makes sure the stream does not
 *      die out.
 *
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
static void
ipmc_set_up_ports(int unit)
{
    int p;
    if (SOC_REG_IS_VALID(unit, EGR_IPMC_CFG2r)) {
        PBMP_ITER(PBMP_PORT_ALL(unit), p) {
            soc_reg_field32_modify(unit, EGR_IPMC_CFG2r, p,
                                DISABLE_TTL_DECREMENTf, 0x1);
            soc_reg_field32_modify(unit, EGR_IPMC_CFG2r, p,
                                DISABLE_TTL_CHECKf, 0x1);
        }
    } else {
        cli_out("\n*ERROR, invalid reg %s\n",
                SOC_REG_NAME(unit, EGR_IPMC_CFG2r));
    }
}

/*
 * Function:
 *      ipmc_chk_port_rate
 * Purpose:
 *      Check actual port rates against expected port rates.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     SOC_E_XXXX
 */
static bcm_error_t
ipmc_chk_port_rate(int unit)
{
    int i, j, port, port_idx;
    bcm_error_t rv = BCM_E_NONE;
    stream_rate_t *rate_calc;
    ipmc_t *ipmc_p = ipmc_parray[unit];

    rate_calc = (stream_rate_t *)
                 sal_alloc(sizeof(stream_rate_t), "rate_calc");
    if (rate_calc == NULL) {
        test_error(unit, "Failed to allocate memory for rate_calc\n");
        return BCM_E_FAIL;
    }
    sal_memset(rate_calc, 0, sizeof(stream_rate_t));

    rate_calc->mode         = 1;
    rate_calc->pkt_size     = ipmc_p->pkt_size_param;
    rate_calc->interval_len = ipmc_p->run_time_param;
    rate_calc->tolerance_lr = PKT_COUNT_CHECK_TOL;
    rate_calc->tolerance_os = PKT_COUNT_CHECK_TOL;
    rate_calc->scaling_factor = ipmc_p->scaling_factor_param;

    SOC_PBMP_CLEAR(rate_calc->pbmp);
    for (i = 0; i < ipmc_p->num_streams; i++) {
        for (j = 0; j < (ipmc_p->num_copy + 1); j++) {
            port_idx = ipmc_p->stream[i][j];
            port = ipmc_p->port_list[port_idx];
            if (port < SOC_MAX_NUM_PORTS) {
                /* pbmp */
                SOC_PBMP_PORT_ADD((rate_calc->pbmp), port);
                /* num_l3_rep */
                if (ipmc_p->l3_copy[port_idx] == 0) {
                    rate_calc->num_rep[port] = 0;
                } else {
                    rate_calc->num_rep[port] = ipmc_p->num_rep[port_idx] + 1;
                }
                /* src_port */
                rate_calc->src_port[port] =
                        ipmc_p->port_list[ipmc_p->stream[i][0]];
            }
        }
    }

    rv = stream_chk_port_rate(unit, PBMP_PORT_ALL(unit), rate_calc);

    sal_free(rate_calc);
    return rv;
}

/*
 * Function:
 *      ipmc_chk_pkt_integrity
 * Purpose:
 *      Redirect all packets back to CPU and check packet integrity
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
static bcm_error_t
ipmc_chk_pkt_integrity(int unit)
{
    int i, port, vlan_id;
    int param_num_stream, param_pkt_seed;
    int *param_port_list, **param_stream;
    uint32 flood_cnt;
    uint8 mac_da[] = MAC_DA;
    /* uint8 mac_sa[] = MAC_SA; */
    stream_integrity_t *pkt_intg;
    bcm_error_t rv = BCM_E_NONE;
    ipmc_t *ipmc_p = ipmc_parray[unit];

    param_num_stream = ipmc_p->num_streams;
    param_pkt_seed = ipmc_p->pkt_seed;
    param_port_list = ipmc_p->port_list;
    param_stream = ipmc_p->stream;

    pkt_intg = sal_alloc(sizeof(stream_integrity_t), "pkt_intg");
    if (pkt_intg == NULL) {
        test_error(unit, "Failed to allocate memory for pkt_intg\n");
        return BCM_E_FAIL;
    }
    sal_memset(pkt_intg, 0, sizeof(stream_integrity_t));

    pkt_intg->type = PKT_TYPE_IPMC;
    pkt_intg->ipv6_en = 0;
    SOC_PBMP_CLEAR(pkt_intg->rx_pbmp);
    for (i = 0; i < param_num_stream; i++) {
        port = param_port_list[param_stream[i][0]];
        if (port < SOC_MAX_NUM_PORTS) {
            /* rx_pbmp */
            SOC_PBMP_PORT_ADD(pkt_intg->rx_pbmp, port);
            /* rx_vlan: used to forward pkt to CPU */
            vlan_id = ipmc_p->tx_vlan[i];
            pkt_intg->rx_vlan[port] = vlan_id;
            /* tx_vlan: used to re-generate ref_pkt */
            vlan_id = ipmc_p->exp_vlan[i];
            pkt_intg->tx_vlan[port] = vlan_id;
            /* port_flood_cnt */
            flood_cnt = ipmc_lossless_flood_cnt(unit, port);
            pkt_intg->port_flood_cnt[port] = flood_cnt;
            /* port_pkt_seed */
            pkt_intg->port_pkt_seed[port] = param_pkt_seed + i;
            /* mac_da, mac_sa */
            sal_memcpy(pkt_intg->mac_da[port], mac_da, NUM_BYTES_MAC_ADDR);
            sal_memcpy(pkt_intg->mac_sa[port], ipmc_p->exp_mac_addr[i], NUM_BYTES_MAC_ADDR);
            /* ip_da, ip_sa */
            pkt_intg->ip_da[port] = DIP;
            pkt_intg->ip_sa[port] = SIP;
        }
    }

    if (stream_chk_pkt_integrity(unit, pkt_intg) != BCM_E_NONE) {
        rv = BCM_E_FAIL;
    }

    sal_free(pkt_intg);
    return rv;
}

/*
 * Function:
 *      ipmc_free_all_memory
 * Purpose:
 *      Free all allocated memory.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
static void
ipmc_free_all_memory(int unit)
{
    int i;
    ipmc_t *ipmc_p = ipmc_parray[unit];

    sal_free(ipmc_p->port_speed);
    sal_free(ipmc_p->port_list);
    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        sal_free(ipmc_p->stream[i]);
    }
    sal_free(ipmc_p->stream);
    sal_free(ipmc_p->num_rep);
    sal_free(ipmc_p->l3_copy);
    sal_free(ipmc_p->tx_vlan);
    sal_free(ipmc_p->exp_vlan);
    sal_free(ipmc_p->exp_mac_addr);
    sal_free(ipmc_p->port_oversub);
    sal_free(ipmc_p->port_used);
    sal_free(ipmc_p->ovs_ratio_x1000);
    sal_free(ipmc_p->exp_rate);
    sal_free(ipmc_p->tpkt_start);
    sal_free(ipmc_p->tpkt_end);
    for (i = 0; i < ipmc_p->num_fp_ports; i++) {
        sal_free(ipmc_p->rand_pkt_sizes[i]);
    }
    sal_free(ipmc_p->rand_pkt_sizes);
}

/*
 * Function:
 *      ipmc_test_init
 * Purpose:
 *      Test init function. Parse CLI params and do necessary init.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
ipmc_test_init(int unit, args_t *a, void **pa)
{
    ipmc_t *ipmc_p;

    ipmc_p = ipmc_parray[unit];
    ipmc_p = sal_alloc(sizeof(ipmc_t), "ipmc_test");
    sal_memset(ipmc_p, 0, sizeof(ipmc_t));
    ipmc_parray[unit] = ipmc_p;

    stream_print_port_config(unit, PBMP_PORT_ALL(unit));
    cli_out("\n==================================================");
    cli_out("\nCalling ipmc_test_init ...\n");
    ipmc_parse_test_params(unit, a);

    ipmc_p->test_fail = 0;

    if (ipmc_p->bad_input == 1) {
        goto done;
    }

    stream_set_mac_lpbk(unit, PBMP_PORT_ALL(unit));
    stream_turn_off_cmic_mmu_bkp(unit);
    stream_turn_off_fc(unit, PBMP_PORT_ALL(unit));
    /* coverity[dont_call : FALSE] */
    ipmc_p->pkt_seed = sal_rand();

done:
    return 0;
}

/*
 * Function:
 *      ipmc_test
 * Purpose:
 *      Set up ports/streams and send packets.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
ipmc_test(int unit, args_t *a, void *pa)
{
    ipmc_t *ipmc_p = ipmc_parray[unit];

    if (ipmc_p->bad_input == 1) {
        goto done;
    }

    cli_out("\n==================================================");
    cli_out("\nCalling ipmc_test ... \n");
    ipmc_set_port_property(unit);
    ipmc_set_up_ports(unit);
    ipmc_set_up_streams(unit);
    ipmc_send_pkts(unit);

    /* check counter */
    if (stream_chk_mib_counters(unit, PBMP_PORT_ALL(unit), 0) != BCM_E_NONE) {
        ipmc_p->test_fail = 1;
    }
    /* check rate */
    if (ipmc_chk_port_rate(unit) != BCM_E_NONE) {
        ipmc_p->test_fail = 1;
    }
    /* check integrity */
    if (ipmc_chk_pkt_integrity(unit) != BCM_E_NONE) {
        ipmc_p->test_fail = 1;
    }

done:
    return 0;
}

/*
 * Function:
 *      ipmc_test_cleanup
 * Purpose:
 *      Do test end checks and free all allocated memory.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
ipmc_test_cleanup(int unit, void *pa)
{
    int rv;
    ipmc_t *ipmc_p = ipmc_parray[unit];

    if (ipmc_p->bad_input == 1) {
        goto done;
    }
    cli_out("\nCalling ipmc_test_cleanup");

done:
    if (ipmc_p->bad_input == 1) {
        ipmc_p->test_fail = 1;
    }

    if (ipmc_p->test_fail == 1) {
        rv = BCM_E_FAIL;
    }
    else {
        rv = BCM_E_NONE;
    }

    cli_out("\n==================================================");
    cli_out("\n==================================================");
    if (ipmc_p->test_fail == 1) {
        cli_out("\n[IPMC test failed]\n\n");
    } else {
        cli_out("\n[IPMC test passed]\n\n");
    }

    ipmc_free_all_memory(unit);
    sal_free(ipmc_p);

    return rv;
}

#endif /* BCM_ESW_SUPPORT && INCLUDE_L3 */
