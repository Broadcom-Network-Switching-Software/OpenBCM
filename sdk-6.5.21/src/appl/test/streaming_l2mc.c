/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * The goal of this test is to check basic multicast and broadcast features of
 * the chip.
 * Test Sequence:
 * 1. Group all active front panel ports into groups of (NumCopy + 1) ports.
 *    Each group constitutes 1 multicast "stream". All ports in a stream should
 *    either be Ethernet or HG2 and have the same line rate, although they may
 *    have different oversub ratios.
 * 2. Let us assume a stream has n ports P0, P1 ... Pn. The ports should be
 *    arranged such that the P1 is the slowest port, i.e. has the highest
 *    oversub ratio.
 * 3. Turn off all flow control, set MAC loopback on all ports and set
 *    IGNORE_MY_MODID to 1 on ING_CONFIG_64.
 * 4. For Ethernet streams:
 *      a.  Place all ports in the stream on a single VLAN, say VLAN0.
 *      b.  Place ports P0 and P1 on a second VLAN, say VLAN1.
 *      c.  Create a 3rd VLAN, say VLAN3 with no ports on it.
 *      d.  Add the following VLAN translations:
 *          i.  Port=P0 OldVlan=VLAN1 NewVlan=VLAN0
 *          ii. Port=P1 OldVlan=VLAN0 NewVlan=VLAN1
 *          iii.    Ports=P2, P3 ... Pn OldVlan=VLAN0 NewVlan=VLAN2
 * 5. For HG2 streams:
 *      a.  Place port P0 on VLAN0.
 *      b.  Place ports P0, P1 ... Pn on VLAN1.
 *      c.  Place all ports in the stream except P0, i.e. P1, P2 ... Pn on VLAN2
 *      d.  Create 3 entries, say L2MC0, L2MC1 and L2MC2.
 *          i.  L2MC0: PORT_BITMAP = P0
 *          ii. L2MC1: PORT_BITMAP = P0
 *          iii.    L2MC2: PORT_BITMAP = P1, P2 ... Pn
 *      e.  Add the following IFP entries:
 *          i.  Qualifier: OuterVlan=VLAN0, Actions: OuterVlanNew=VLAN1, RedirectMcast=L2MC0
 *          ii. Qualifier: OuterVlan=VLAN2, Actions: OuterVlanNew=VLAN1, RedirectMcast=L2MC1
 *          iii.Qualifier: OuterVlan=VLAN1, Actions: OuterVlanNew=VLAN2, RedirectMcast=L2MC2
 * 6. Send packets from CPU port on VLAN0 of each stream to flood the stream.
 *    If FloodCnt=0, the test will determine the number of packets to send based.
 *    These values are obtained empirically through trial and error for LR ports
 *    for NumCopy=2 and adjusted for oversub configs based on oversub ratio.
 *    Embed the packet size and random seed used to generate each packet in the
 *    payload.
 * 7. Allow the packets to swill for a fixed amount of time decided by RateCalcInt.
 * 8. Snoop back all packets in each stream to the CPU using the following sequence:
 *      a.  Program CPU_CONTROL_0.UVLAN_TOCPU=1. This sends all packets with an
 *          unknown VLAN to CPU.
 *      b.  Invalidate VLAN0 for Ethernet Streams and VLAN1 for HG2 streams by
 *          setting VALID=0 in the VLAN table
 * 9. Decode each received packet to obtain the random seed and packet size.
 *    Reconstruct the expected packet and check the received packet for integrity.
 *
 * Configuration parameters passed from CLI:
 * PktSize: Packet size in bytes. Set to 0 for worst case packet sizes on all
 *          ports (145B for ENET, 76B for HG2). Set to 1 for random packet sizes
 * FloodCnt: Number of packets in each swill. Setting this to 0 will let the
 *           test calculate the number of packets that can be sent to achieve
 *           a lossless swirl at full rate. Set to 0 by default.
 * NumCopy: Number of multicast copies in each stream (2 by default).
 * RateCalcInt: Interval in seconds over which rate is to be calculated
 * TolLr: Rate tolerance percentage for linerate ports (1% by default).
 * TolOv: Rate tolerance percentage for oversubscribed ports (3% by default).
 * ChkPktInteg: Set to 0 to disable packet integrity checks, 1 to enable (default).
 * MaxNumCells: Max number of cells for random packet sizes. Default = 4. Set
 *              to 0 for random.
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

#include "testlist.h"
#include "gen_pkt.h"
#include "streaming_lib.h"

#define MAC_DA {0x01,0x00,0x0c,0xde,0xff,0x00}
#define MAC_SA {0xfe,0xdc,0xba,0x98,0x76,0x54}
#define VLAN 0x100

#define MAX_FP_PORTS 130
#define MAX_NUM_CELLS_PARAM_DEFAULT 4

#define FLOOD_CNT_25G 9
#define FLOOD_CNT_40G 21
#define FLOOD_CNT_50G 28
#define FLOOD_CNT_100G 50

#define MAX_PKTS_PER_STREAM 500

#define PKT_SIZE_PARAM_DEFAULT 0
#define FLOOD_PKT_CNT_PARAM_DEFAULT 0
#define NUM_COPY_PARAM_DEFAULT 2
#define RATE_CALC_INTERVAL_PARAM_DEFAULT 10
#define RATE_TOLERANCE_LR_PARAM_DEFAULT 1
#define RATE_TOLERANCE_OV_PARAM_DEFAULT 3
#define CHECK_PACKET_INTEGRITY_PARAM_DEFAULT 1
#define SPEEDUP_FACTOR_PARAM_DEFAULT 1
#define NUM_LPORT_TAB_ENTRIES 256
#define NUM_PORT_TAB_ENTRIES 136
#define CPU_PORT 0

#if defined(BCM_ESW_SUPPORT) && \
   (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))

typedef struct l2mc_s {

    uint32 num_fp_ports;
    uint32 num_streams;
    int *port_speed;
    int *port_list;
    int **stream;
    uint32 *tx_vlan;
    int *hg_stream;
    uint32 *port_oversub;
    uint32 oversub_config;
    uint32 *port_used;
    uint32 num_pipes;
    uint32 total_chip_bw;
    uint32 bw_per_pipe;
    uint32 *ovs_ratio_x1000;
    uint64 *rate;
    uint64 *exp_rate;
    uint64 *tpkt_start;
    uint64 *tpkt_end;
    uint64 *tbyt_start;
    uint64 *tbyt_end;
    uint32 **rand_pkt_sizes;
    uint32 pkt_size_param;
    uint32 flood_pkt_cnt_param;
    uint32 num_copy_param;
    uint32 rate_calc_interval_param;
    uint32 rate_tolerance_lr_param;
    uint32 rate_tolerance_ov_param;
    uint32 check_packet_integrity_param;
    uint32 max_num_cells_param;
    uint32 scaling_factor_param;
    uint32 bad_input;
    int test_fail;
    uint32 pkt_seed;
} l2mc_t;

static l2mc_t *l2mc_parray[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *      l2mc_print_port_config
 * Purpose:
 *      Print port config
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
static void
l2mc_print_port_config(int unit)
{
    int i, port;
    pbmp_t pbmp;
    l2mc_t *l2mc_p = l2mc_parray[unit];

    SOC_PBMP_CLEAR(pbmp);
    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        port = l2mc_p->port_list[i];
        if (port < SOC_MAX_NUM_PORTS) {
            SOC_PBMP_PORT_ADD(pbmp, port);
        }
    }
    stream_print_port_config(unit, pbmp);
}

/*
 * Function:
 *      l2mc_parse_test_params
 * Purpose:
 *      Parse CLI parameters, create test structure and flag bad inputs.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     Nothing
 * Notes:
 *      l2mc_p->bad_input set from here - tells test to crash out in case
 *      CLI input combination is invalid.
 */
static void
l2mc_parse_test_params(int unit, args_t *a)
{
    parse_table_t parse_table;
    l2mc_t *l2mc_p = l2mc_parray[unit];

    const char tr511_test_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nDocumentation too long to be displayed with -pedantic compiler\n";
#else
    "The goal of this test is to check basic multicast and broadcast features of\n"
    "the chip.\n"
    "Test Sequence:\n"
    "1. Group all active front panel ports into groups of (NumCopy + 1) ports.\n"
    "    Each group constitutes 1 multicast \"stream\". All ports in a stream should\n"
    "    either be Ethernet or HG2 and have the same line rate, although they may\n"
    "    have different oversub ratios.\n"
    "2. Let us assume a stream has n ports P0, P1 ... Pn. The ports should be\n"
    "    arranged such that the P1 is the slowest port, i.e. has the highest\n"
    "    oversub ratio.\n"
    "3. Turn off all flow control, set MAC loopback on all ports and set\n"
    "    IGNORE_MY_MODID to 1 on ING_CONFIG_64.\n"
    "4. For Ethernet streams:\n"
    "    a.  Place all ports in the stream on a single VLAN, say VLAN0.\n"
    "    b.  Place ports P0 and P1 on a second VLAN, say VLAN1.\n"
    "    c.  Create a 3rd VLAN, say VLAN3 with no ports on it.\n"
    "    d.  Add the following VLAN translations:\n"
    "        i.  Port=P0 OldVlan=VLAN1 NewVlan=VLAN0\n"
    "        ii. Port=P1 OldVlan=VLAN0 NewVlan=VLAN1\n"
    "        iii.    Ports=P2, P3 ... Pn OldVlan=VLAN0 NewVlan=VLAN2\n"
    "5. For HG2 streams:\n"
    "    a.  Place port P0 on VLAN0.\n"
    "    b.  Place ports P0, P1 ... Pn on VLAN1.\n"
    "    c.  Place all ports in the stream except P0, i.e. P1, P2 ... Pn on VLAN2\n"
    "    d.  Create 3 entries, say L2MC0, L2MC1 and L2MC2.\n"
    "        i.  L2MC0: PORT_BITMAP = P0\n"
    "        ii. L2MC1: PORT_BITMAP = P0\n"
    "        iii.    L2MC2: PORT_BITMAP = P1, P2 ... Pn\n"
    "    e.  Add the following IFP entries:\n"
    "        i.  Qualifier: OuterVlan=VLAN0, Actions: OuterVlanNew=VLAN1, RedirectMcast=L2MC0\n"
    "        ii. Qualifier: OuterVlan=VLAN2, Actions: OuterVlanNew=VLAN1, RedirectMcast=L2MC1\n"
    "        iii.    Qualifier: OuterVlan=VLAN1, Actions: OuterVlanNew=VLAN2, RedirectMcast=L2MC2\n"
    "6. Send packets from CPU port on VLAN0 of each stream to flood the stream.\n"
    "    If FloodCnt=0, the test will determine the number of packets to send based.\n"
    "    These values are obtained empirically through trial and error for LR ports\n"
    "    for NumCopy=2 and adjusted for oversub configs based on oversub ratio.\n"
    "    Embed the packet size and random seed used to generate each packet in the\n"
    "    payload.\n"
    "7. Allow the packets to swill for a fixed amount of time decided by RateCalcInt.\n"
    "8. Snoop back all packets in each stream to the CPU using the following sequence:\n"
    "    a.  Program CPU_CONTROL_0.UVLAN_TOCPU=1. This sends all packets with an\n"
    "        unknown VLAN to CPU.\n"
    "    b.  Invalidate VLAN0 for Ethernet Streams and VLAN1 for HG2 streams by\n"
    "        setting VALID=0 in the VLAN table\n"
    "9. Decode each received packet to obtain the random seed and packet size.\n"
    "    Reconstruct the expected packet and check the received packet for integrity.\n"
    "\n"
    "Configuration parameters passed from CLI:\n"
    "PktSize: Packet size in bytes. Set to 0 for worst case packet sizes on all\n"
    "         ports (145B for ENET, 76B for HG2). Set to 1 for random packet sizes\n"
    "FloodCnt: Number of packets in each swill. Setting this to 0 will let the\n"
    "          test calculate the number of packets that can be sent to achieve\n"
    "          a lossless swirl at full rate. Set to 0 by default.\n"
    "NumCopy: Number of multicast copies in each stream (2 by default).\n"
    "RateCalcInt: Interval in seconds over which rate is to be calculated\n"
    "TolLr: Rate tolerance percentage for linerate ports (1 percentage by default).\n"
    "TolOv: Rate tolerance percentage for oversubscribed ports (3 percentage by default).\n"
    "ChkPktInteg: Set to 0 to disable packet integrity checks, 1 to enable (default).\n"
    "MaxNumCells: Max number of cells for random packet sizes. Default = 4. Set\n"
    "             to 0 for random.\n";
#endif

    l2mc_p->bad_input = 0;

    l2mc_p->pkt_size_param = PKT_SIZE_PARAM_DEFAULT;
    l2mc_p->flood_pkt_cnt_param = FLOOD_PKT_CNT_PARAM_DEFAULT;
    l2mc_p->num_copy_param = NUM_COPY_PARAM_DEFAULT;
    l2mc_p->rate_calc_interval_param = RATE_CALC_INTERVAL_PARAM_DEFAULT;
    l2mc_p->rate_tolerance_lr_param = RATE_TOLERANCE_LR_PARAM_DEFAULT;
    l2mc_p->rate_tolerance_ov_param = RATE_TOLERANCE_OV_PARAM_DEFAULT;
    l2mc_p->check_packet_integrity_param
                            = CHECK_PACKET_INTEGRITY_PARAM_DEFAULT;
    l2mc_p->max_num_cells_param = MAX_NUM_CELLS_PARAM_DEFAULT;
    l2mc_p->scaling_factor_param = SPEEDUP_FACTOR_PARAM_DEFAULT;

    /*Parse CLI opts */

    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table, "PktSize", PQ_INT|PQ_DFL, 0,
                    &(l2mc_p->pkt_size_param), NULL);
    parse_table_add(&parse_table, "FloodCnt", PQ_INT|PQ_DFL, 0,
                    &(l2mc_p->flood_pkt_cnt_param), NULL);
    parse_table_add(&parse_table, "NumCopy", PQ_INT|PQ_DFL, 0,
                    &(l2mc_p->num_copy_param), NULL);
    parse_table_add(&parse_table, "RateCalcInt", PQ_INT|PQ_DFL, 0,
                    &(l2mc_p->rate_calc_interval_param), NULL);
    parse_table_add(&parse_table, "TolLr", PQ_INT|PQ_DFL, 0,
                    &(l2mc_p->rate_tolerance_lr_param), NULL);
    parse_table_add(&parse_table, "TolOv", PQ_INT|PQ_DFL, 0,
                    &(l2mc_p->rate_tolerance_ov_param), NULL);
    parse_table_add(&parse_table, "ChkPktInteg", PQ_INT|PQ_DFL, 0,
                    &(l2mc_p->check_packet_integrity_param), NULL);
    parse_table_add(&parse_table, "MaxNumCells", PQ_INT|PQ_DFL, 0,
                    &(l2mc_p->max_num_cells_param), NULL);
    parse_table_add(&parse_table, "ScalingFactor", PQ_INT|PQ_DFL, 0,
                    &(l2mc_p->scaling_factor_param), NULL);

    if (parse_arg_eq(a, &parse_table) < 0 || ARG_CNT(a) != 0) {
        test_msg("%s", tr511_test_usage);
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        l2mc_p->bad_input = 1;
        parse_arg_eq_done(&parse_table);
    } else {
        cli_out("\n");
        cli_out("------------- PRINTING TEST PARAMS ------------------\n");
        cli_out("PktSize      = %0d\n", l2mc_p->pkt_size_param);
        cli_out("FloodCnt     = %0d\n", l2mc_p->flood_pkt_cnt_param);
        cli_out("NumCopy      = %0d\n", l2mc_p->num_copy_param);
        cli_out("RateCalcInt  = %0d\n", l2mc_p->rate_calc_interval_param);
        cli_out("TolLr        = %0d\n", l2mc_p->rate_tolerance_lr_param);
        cli_out("TolOv        = %0d\n", l2mc_p->rate_tolerance_ov_param);
        cli_out("ChkPktInteg  = %0d\n", l2mc_p->check_packet_integrity_param);
        cli_out("MaxNumCells  = %0d\n", l2mc_p->max_num_cells_param);
        cli_out("ScalingFactor= %0d\n", l2mc_p->scaling_factor_param);
        cli_out("-----------------------------------------------------\n");
    }

    if (l2mc_p->max_num_cells_param == 0) {
        /* coverity[dont_call : FALSE] */
        l2mc_p->max_num_cells_param = (sal_rand() % (MTU_CELL_CNT - 1)) + 1;
    }

    if (l2mc_p->pkt_size_param == 0) {
        cli_out
            ("\nUsing worst case packet sizes - 145B for Ethernet "
             "and 76B (64B + 12B hg-hdr) for HG2");
    } else if (l2mc_p->pkt_size_param == 1) {
        cli_out("\nUsing random packet sizes");
    } else if (l2mc_p->pkt_size_param < MIN_PKT_SIZE) {
        test_error(unit,"\n*ERROR: Packet size cannot be lower than %0dB\n",
                MIN_PKT_SIZE);
        l2mc_p->bad_input = 1;
    } else if (l2mc_p->pkt_size_param > MTU) {
        test_error(unit,"\n*ERROR: Packet size cannot be higher than %0dB (Test Limitation)\n",
                MTU);
        l2mc_p->bad_input = 1;
    }

    if (l2mc_p->flood_pkt_cnt_param == 0) {
        cli_out("\nFloodCnt=0, test will automatically calculate number of"
                " packets to flood each port");
    }
}

/*
 * Function:
 *      l2mc_set_lowest_stream_port
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
l2mc_set_lowest_stream_port(int unit)
{
    int i, j, tmp, port, port_speed, min_speed, min_speed_idx;
    int param_idx;
    l2mc_t *l2mc_p = l2mc_parray[unit];

    param_idx = 1;
    for (i = 0; i < l2mc_p->num_streams; i++) {
        min_speed_idx = param_idx;
        port = l2mc_p->port_list[l2mc_p->stream[i][min_speed_idx]];
        bcm_port_speed_get(unit, port, &min_speed);

        for (j = 0; j < (l2mc_p->num_copy_param + 1); j++) {
            port = l2mc_p->port_list[l2mc_p->stream[i][j]];
            bcm_port_speed_get(unit, port, &port_speed);
            if (port_speed < min_speed) {
                min_speed = port_speed;
                min_speed_idx = j;
            }
        }
        if (min_speed_idx != param_idx) {
            tmp = l2mc_p->stream[i][param_idx];
            l2mc_p->stream[i][param_idx] =  l2mc_p->stream[i][min_speed_idx];
            l2mc_p->stream[i][min_speed_idx] = tmp;
        }
    }
}

/*
 * Function:
 *      l2mc_set_port_property_arrays
 * Purpose:
 *      Set port_list, port_speed
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing.
 */
static void
l2mc_set_port_property_arrays(int unit)
{
    int p;
    int picked_port;
    int i, j;
    uint32 pkt_size;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 stream_possible = 0;
    uint32 ports_found = 0;
    uint32 ports_picked = 0;
    uint32 ports_accounted_for = 0;
    l2mc_t *l2mc_p = l2mc_parray[unit];

    l2mc_p->num_fp_ports = 0;
    l2mc_p->num_streams = 0;
    l2mc_p->oversub_config = 0;

    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        l2mc_p->num_fp_ports++;
    }

    l2mc_p->port_list =
        (int *)sal_alloc(l2mc_p->num_fp_ports * sizeof(int),
                         "port_list");
    l2mc_p->port_speed =
        (int *)sal_alloc(l2mc_p->num_fp_ports * sizeof(int),
                         "port_speed_array");
    l2mc_p->hg_stream =
        (int *)sal_alloc(l2mc_p->num_fp_ports * sizeof(int),
                         "hg_stream_array");
    l2mc_p->port_oversub =
        (uint32 *) sal_alloc(l2mc_p->num_fp_ports * sizeof(uint32),
                             "port_oversub_array");
    l2mc_p->rand_pkt_sizes =
        (uint32 **) sal_alloc(l2mc_p->num_fp_ports * sizeof(uint32 *),
                              "rand_pkt_sizes_array*");

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        l2mc_p->rand_pkt_sizes[i] =
            (uint32 *) sal_alloc(TARGET_CELL_COUNT * sizeof(uint32),
                                "rand_pkt_sizes_array");
    }
    l2mc_p->stream =
        (int **) sal_alloc(l2mc_p->num_fp_ports * sizeof(int *),
                              "stream_array*");

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        l2mc_p->stream[i] =
            (int *) sal_alloc((l2mc_p->num_copy_param + 1) * sizeof(int),
                                "stream_array");
    }
    l2mc_p->port_used =
        (uint32 *) sal_alloc(l2mc_p->num_fp_ports * sizeof(uint32),
                             "l2mc_p->port_used_array");

    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        l2mc_p->port_list[i] = p;
        i++;
    }

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        l2mc_p->port_speed[i] =
            si->port_speed_max[l2mc_p->port_list[i]];

        switch(l2mc_p->port_speed[i]) {
            case 11000: l2mc_p->port_speed[i] = 10600; break;
            case 27000: l2mc_p->port_speed[i] = 26500; break;
            case 42000: l2mc_p->port_speed[i] = 42400; break;
            default: break;
        }
    }

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        if (SOC_PBMP_MEMBER(si->oversub_pbm, l2mc_p->port_list[i])
            &&
            (!(SOC_PBMP_MEMBER(si->management_pbm, l2mc_p->port_list[i])))) {
            l2mc_p->oversub_config = 1;
            l2mc_p->port_oversub[i] = 1;
        } else {
            l2mc_p->port_oversub[i] = 0;
        }
    }

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        l2mc_p->port_used[i] = 0;
        l2mc_p->hg_stream[i] = 0;
    }

    /* a trick to accelerate port grouping, if same speed eth and hg */
    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        if (IS_HG_PORT(unit, l2mc_p->port_list[i])) {
            l2mc_p->port_speed[i] |= 0x1;
        }
    }

    while (ports_accounted_for < l2mc_p->num_fp_ports) {
        do {
            i = sal_rand() % l2mc_p->num_fp_ports;
        }while (l2mc_p->port_used[i] != 0);
        stream_possible = 0;
        ports_found = 0;
        for (j = 0;j < l2mc_p->num_fp_ports; j++) {
            if((l2mc_p->port_used[j] == 0) &&
               (l2mc_p->port_speed[i] == l2mc_p->port_speed[j]) &&
               (i != j)) {
                ports_found++;
            }

            if (ports_found >= l2mc_p->num_copy_param) {
                stream_possible = 1;
            }
        }

        ports_accounted_for++;
        if(stream_possible == 1) {
            l2mc_p->port_used[i] = 1;
            ports_picked = 0;
            l2mc_p->stream[l2mc_p->num_streams][0] = i;
            if (IS_HG_PORT(unit, l2mc_p->port_list[i])) {
                l2mc_p->hg_stream[l2mc_p->num_streams] = 1;
            }
            while (ports_picked < l2mc_p->num_copy_param) {
                do {
                    picked_port = sal_rand() % l2mc_p->num_fp_ports;
                }while((l2mc_p->port_speed[i] !=
                                l2mc_p->port_speed[picked_port])
                        || (l2mc_p->port_used[picked_port] != 0));

                ports_picked++;
                l2mc_p->port_used[picked_port] = 1;
                l2mc_p->stream[l2mc_p->num_streams][ports_picked]
                                                            = picked_port;
                ports_accounted_for++;
            }
            l2mc_p->num_streams++;
        }
    }

    /* restore hg speed */
    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        if (IS_HG_PORT(unit, l2mc_p->port_list[i])) {
            l2mc_p->port_speed[i] &= ~0x1;
        }
    }
    l2mc_set_lowest_stream_port(unit);

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        for (j = 0; j < TARGET_CELL_COUNT; j++) {
            do {
                /* coverity[dont_call : FALSE] */
                pkt_size = (sal_rand() % (MTU - MIN_PKT_SIZE + 1)) + MIN_PKT_SIZE;
            } while (stream_get_pkt_cell_cnt(unit, pkt_size,
                                             l2mc_p->port_list[i]) >
                     l2mc_p->max_num_cells_param);
            l2mc_p->rand_pkt_sizes[i][j] = pkt_size;
        }
    }

    {
        /* print */
        l2mc_print_port_config(unit);
        cli_out("\n=========== STREAM INFO ============\n");
        cli_out("\n%8s %8s", "Stream", "Port");
        for (i = 0; i < l2mc_p->num_streams; i++) {
            cli_out("\n%8d ", i);
            for (j = 0; j < (l2mc_p->num_copy_param + 1); j++) {
                cli_out("%8d", l2mc_p->port_list[l2mc_p->stream[i][j]]);
            }
        }
    }
}

/*
 * Function:
 *      fp_chg_vlan_and_redirect
 * Purpose:
 *      Creates entry pointed to by l2mc_index in the L2MC table with
 *      PORT_BITMAP=redirect_pbmp
 *      Create the following FP actions:
 *          Qualifier: OuterVlan=old_vlan
 *          Actions: OuterVlanNew=VLAN1, RedirectMcast=l2mc_index
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      old_vlan : Old VLAN of incoming packet
 *      new_vlan: New VLAN after IFP modifies packet
 *      fg : Pointer to Field group for IFP entry
 *      l2mc_index: Index in L2MC table where entry is to be created
 *      redirect_pbmp: PORT_BITMAP field of L2MC entry created
 * Returns:
 *     Nothing
 */
static void
l2mc_fp_change_vlan_redirect(int unit, bcm_vlan_t old_vlan, bcm_vlan_t new_vlan,
                        bcm_field_group_t *fg, uint32 l2mc_index,
                        soc_pbmp_t *redirect_pbmp) {
    bcm_field_entry_t fe;
    l2mc_entry_t l2mc_entry;

    if (SOC_MEM_IS_VALID(unit, L2MCm)) {
        (void) soc_mem_read(unit, L2MCm, COPYNO_ALL, l2mc_index,
                            l2mc_entry.entry_data);
        soc_mem_field32_set(unit, L2MCm, l2mc_entry.entry_data,
                            VALIDf, 0x1);
        soc_mem_pbmp_field_set(unit, L2MCm, l2mc_entry.entry_data,
                               PORT_BITMAPf, redirect_pbmp);
        (void) soc_mem_write(unit, L2MCm, COPYNO_ALL, l2mc_index, 
                             l2mc_entry.entry_data);
    } else {
        cli_out("\n*ERROR, invalid mem %s\n", SOC_MEM_NAME(unit, L2MCm));
    }

    (void) bcm_field_entry_create(unit, *fg, &fe);
    bcm_field_qualify_OuterVlan(unit, fe, old_vlan, (bcm_vlan_t)(0xffff));
    bcm_field_action_add(unit, fe, bcmFieldActionOuterVlanNew,
                                                (uint32)(new_vlan), 0);
    bcm_field_action_add(unit, fe, bcmFieldActionRedirectMcast, l2mc_index, 0);
    (void) bcm_field_entry_install(unit, fe);
}

/*
 * Function:
 *      l2mc_set_up_streams
 * Purpose:
 *      This function actually sets up the L2MC packet flow. Please refer to
 *      items 3-6 of the "Test Setup" section of the test description at the
 *      beginning of this file.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing.
 */
static void
l2mc_set_up_streams(int unit)
{
    int i, j;
    pbmp_t pbm, ubm;
    soc_pbmp_t redirect_pbmp;
    uint32 vlan = VLAN;
    uint32 vlan_0, vlan_1;
    uint32 l2mc_index = 1;
    bcm_field_group_t fg;
    bcm_field_qset_t qset;
    l2mc_t *l2mc_p = l2mc_parray[unit];

    l2mc_p->tx_vlan = (uint32*) sal_alloc(l2mc_p->num_streams * sizeof(uint32),
                                          "tx_vlan");
    BCM_PBMP_CLEAR(ubm);
    bcm_vlan_destroy_all(unit);
    bcm_vlan_control_set(unit, bcmVlanTranslate, TRUE);

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlan);
    (void) bcm_field_group_create(unit, qset, l2mc_index, &fg);

    (void) bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1);

    for (i = 0; i < l2mc_p->num_streams; i++) {
        if (l2mc_p->hg_stream[i] == 0) {
            /*Ethernet Stream*/
            BCM_PBMP_CLEAR(pbm);
            bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
            for (j = 0; j < l2mc_p->num_copy_param + 1; j++) {
                BCM_PBMP_PORT_ADD(pbm, l2mc_p->port_list[l2mc_p->stream[i][j]]);
            }
            bcm_vlan_port_add(unit, (bcm_vlan_t)(vlan), pbm, ubm);
            vlan_0 = vlan;
            vlan++;
            bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
            BCM_PBMP_CLEAR(pbm);
            BCM_PBMP_PORT_ADD(pbm, l2mc_p->port_list[l2mc_p->stream[i][0]]);
            BCM_PBMP_PORT_ADD(pbm, l2mc_p->port_list[l2mc_p->stream[i][1]]);
            bcm_vlan_port_add(unit, (bcm_vlan_t)(vlan), pbm, ubm);
            vlan_1 = vlan;
            vlan++;
            bcm_vlan_translate_add(unit, l2mc_p->port_list[l2mc_p->stream[i][0]],
                                   vlan_1, vlan_0, 0);
            bcm_vlan_translate_add(unit, l2mc_p->port_list[l2mc_p->stream[i][1]],
                                   vlan_0, vlan_1, 0);

            for (j = 2; j < (l2mc_p->num_copy_param + 1); j++) {
                bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
                bcm_vlan_translate_add(unit,
                                       l2mc_p->port_list[l2mc_p->stream[i][j]],
                                       vlan_0, vlan, 0);
                vlan++;
            }
            l2mc_p->tx_vlan[i] = vlan_0;
        } else {
            /*HG2 Stream*/
            BCM_PBMP_CLEAR(pbm);
            bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
            BCM_PBMP_PORT_ADD(pbm, l2mc_p->port_list[l2mc_p->stream[i][0]]);
            bcm_vlan_port_add(unit, (bcm_vlan_t)(vlan), pbm, ubm);
            vlan_0 = vlan;
            vlan++;
            bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
            BCM_PBMP_CLEAR(pbm);
            for (j = 0; j < l2mc_p->num_copy_param + 1; j++) {
                BCM_PBMP_PORT_ADD(pbm, l2mc_p->port_list[l2mc_p->stream[i][j]]);
            }
            bcm_vlan_port_add(unit, (bcm_vlan_t)(vlan), pbm, ubm);
            vlan_1 = vlan;
            vlan++;

            SOC_PBMP_CLEAR(redirect_pbmp);
            SOC_PBMP_PORT_ADD(redirect_pbmp,
                              l2mc_p->port_list[l2mc_p->stream[i][0]]);
            l2mc_fp_change_vlan_redirect(unit, vlan_0, vlan_1, &fg,
                                    l2mc_index, &redirect_pbmp);
            l2mc_index++;

            BCM_PBMP_CLEAR(pbm);
            bcm_vlan_create(unit, (bcm_vlan_t)(vlan));
            for (j = 1; j < l2mc_p->num_copy_param + 1; j++) {
                BCM_PBMP_PORT_ADD(pbm, l2mc_p->port_list[l2mc_p->stream[i][j]]);
            }
            bcm_vlan_port_add(unit, (bcm_vlan_t)(vlan), pbm, ubm);
            l2mc_fp_change_vlan_redirect(unit, vlan, vlan_1, &fg,
                                    l2mc_index, &redirect_pbmp);
            l2mc_index++;
            SOC_PBMP_CLEAR(redirect_pbmp);
            for (j = 1; j < l2mc_p->num_copy_param + 1; j++) {
                SOC_PBMP_PORT_ADD(redirect_pbmp,
                                  l2mc_p->port_list[l2mc_p->stream[i][j]]);
            }
            l2mc_fp_change_vlan_redirect(unit, vlan_1, vlan, &fg,
                                    l2mc_index, &redirect_pbmp);
            l2mc_index++;
            vlan++;
            l2mc_p->tx_vlan[i] = vlan_0;
        }
    }
}

/*
 * Function:
 *      l2mc_lossless_flood_cnt
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
l2mc_lossless_flood_cnt(int unit, int port_speed, int stream_hg_en)
{
    uint32 flood_cnt = 0;
    int param_flood_cnt;
    l2mc_t *l2mc_p = l2mc_parray[unit];

    param_flood_cnt = l2mc_p->flood_pkt_cnt_param;

    if (param_flood_cnt == 0) {
        switch (port_speed) {
            case 10000: flood_cnt = 14; break;
            case 10600: flood_cnt = 14; break;
            case 25000: flood_cnt = 14; break;
            case 26500: flood_cnt = 25; break;
            case 40000: flood_cnt = 20; break;
            case 42400: flood_cnt = 38; break;
            case 50000: flood_cnt = 24; break;
            case 53000: flood_cnt = 45; break;
            case 100000: flood_cnt = 50; break;
            case 106000: flood_cnt = 94; break;
            default: flood_cnt = MAX_PKTS_PER_STREAM; break;
        }
    } else {
        flood_cnt = param_flood_cnt;
    }

    if (stream_hg_en == 1) {
        flood_cnt = 2 * flood_cnt;
    }
    if (flood_cnt > MAX_PKTS_PER_STREAM) {
        flood_cnt = MAX_PKTS_PER_STREAM;
    }

    return (flood_cnt);
}

/*
 * Function:
 *      l2mc_send_pkts
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
l2mc_send_pkts(int unit)
{
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    int i, port;
    uint32 pkt_size, flood_cnt, pkt_cnt_sum = 0;
    uint32 use_random_packet_sizes = 0;
    l2mc_t *l2mc_p = l2mc_parray[unit];
    stream_pkt_t *tx_pkt;

    tx_pkt = sal_alloc(sizeof(stream_pkt_t), "tx_pkt");
    sal_memset(tx_pkt, 0, sizeof(stream_pkt_t));

    if (l2mc_p->pkt_size_param == 1) {
        use_random_packet_sizes = 1;
    }

    cli_out("\n==================================================");
    cli_out("\nSending packets ...\n");
    for (i = 0; i < l2mc_p->num_streams; i++) {
        port = l2mc_p->stream[i][0];
        if (l2mc_p->pkt_size_param == 0) {
            if (l2mc_p->hg_stream[i] == 1) {
                pkt_size = HG2_WC_PKT_SIZE;
            } else {
                pkt_size = ENET_WC_PKT_SIZE;
            }
        } else {
            pkt_size = l2mc_p->pkt_size_param;
        }
        flood_cnt = l2mc_lossless_flood_cnt(unit,
                            l2mc_p->port_speed[port],
                            l2mc_p->hg_stream[i]);

        tx_pkt->port = l2mc_p->port_list[port];
        tx_pkt->num_pkt = flood_cnt;
        tx_pkt->pkt_seed = l2mc_p->pkt_seed + i;
        tx_pkt->pkt_size = pkt_size;
        tx_pkt->rand_pkt_size_en = use_random_packet_sizes;
        tx_pkt->rand_pkt_size = NULL;
        tx_pkt->tx_vlan = l2mc_p->tx_vlan[i];
        sal_memcpy(tx_pkt->mac_da, mac_da, NUM_BYTES_MAC_ADDR);
        sal_memcpy(tx_pkt->mac_sa, mac_sa, NUM_BYTES_MAC_ADDR);
        if (tx_pkt->rand_pkt_size_en) {
            tx_pkt->rand_pkt_size = l2mc_p->rand_pkt_sizes[port];
        }
        stream_tx_pkt(unit, tx_pkt);
        /* print */
        cli_out("Stream %0d, pkt_flood_count = %0d\n", i, tx_pkt->cnt_pkt);
        pkt_cnt_sum += tx_pkt->cnt_pkt;
    }

    cli_out("\nTotal packets sent: %0d\n\n", pkt_cnt_sum);
    sal_free(tx_pkt);
}

/*
 * Function:
 *      l2mc_set_up_ports
 * Purpose:
 *      1. Enable IFP for CPU port
 *      2. Program ING_VLAN_TAG_ACTION_PROFILE to remove inner tag for double
 *         tagged packets
 *      3. Program MMU to not accept any packets from ports l2mc_p->stream[i][2..n]
 *         by setting THDI_INPUT_PORT_XON_ENABLES.INPUT_PORT_RX_ENABLE=0.
 *         This prevents the number of packets in each swill from multiplying.
 *      4. Set IGNORE_MY_MODID to 1 to prevent dropping HG2 packets after looping
 *         back.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
static void
l2mc_set_up_ports(int unit)
{
    int i, j;
    port_tab_entry_t port_tab_entry;
    l2mc_t *l2mc_p = l2mc_parray[unit];
    ing_vlan_tag_action_profile_entry_t ing_vlan_tag_action_profile_entry;
    uint64 ing_config;

    if (SOC_MEM_IS_VALID(unit, PORT_TABm)) {
        (void) soc_mem_read(unit, PORT_TABm, COPYNO_ALL,
                        CPU_PORT, port_tab_entry.entry_data);
        soc_mem_field32_set(unit, PORT_TABm, port_tab_entry.entry_data,
                            FILTER_ENABLEf, 0x1);
        (void) soc_mem_write(unit, PORT_TABm, COPYNO_ALL, CPU_PORT,
                        port_tab_entry.entry_data);
    } else {
        cli_out("\n*ERROR, invalid mem %s\n", SOC_MEM_NAME(unit, PORT_TABm));
    }

    if (SOC_MEM_IS_VALID(unit, ING_VLAN_TAG_ACTION_PROFILEm)) {
        (void) soc_mem_read(unit, ING_VLAN_TAG_ACTION_PROFILEm, COPYNO_ALL,
                     0, ing_vlan_tag_action_profile_entry.entry_data);
        soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm,
                            ing_vlan_tag_action_profile_entry.entry_data,
                            DT_ITAG_ACTIONf, 0x3);
        (void) soc_mem_write(unit, ING_VLAN_TAG_ACTION_PROFILEm, COPYNO_ALL,
                     0, ing_vlan_tag_action_profile_entry.entry_data);

    } else {
        cli_out("\n*ERROR, invalid mem %s\n",
                SOC_MEM_NAME(unit, ING_VLAN_TAG_ACTION_PROFILEm));
    }

    if (SOC_REG_IS_VALID(unit, THDI_INPUT_PORT_XON_ENABLESr)) {
        for (i = 0; i < l2mc_p->num_streams; i++) {
            for (j = 2; j < (l2mc_p->num_copy_param + 1); j++) {
                soc_reg_field32_modify(unit, THDI_INPUT_PORT_XON_ENABLESr,
                                       l2mc_p->port_list[l2mc_p->stream[i][j]],
                                       INPUT_PORT_RX_ENABLEf, 0x0);
            }
        }
    } else {
        cli_out("\n*ERROR, invalid reg %s\n",
                SOC_REG_NAME(unit, THDI_INPUT_PORT_XON_ENABLESr));
    }

    if (SOC_REG_IS_VALID(unit, ING_CONFIG_64r)) {
        (void) soc_reg_get(unit, ING_CONFIG_64r, REG_PORT_ANY, 0, &ing_config);
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &ing_config,
                              IGNORE_MY_MODIDf, 0x1);
        (void) soc_reg_set(unit, ING_CONFIG_64r, REG_PORT_ANY, 0, ing_config);
    } else {
        cli_out("\n*ERROR, invalid reg %s\n",
                SOC_REG_NAME(unit, ING_CONFIG_64r));
    }
}

/*
 * Function:
 *      l2mc_chk_port_rate
 * Purpose:
 *      Check actual port rates against expected port rates.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     SOC_E_XXXX
 */
static bcm_error_t
l2mc_chk_port_rate(int unit)
{
    int i, j, port, port_idx;
    bcm_error_t rv = BCM_E_NONE;
    stream_rate_t *rate_calc;
    l2mc_t *l2mc_p = l2mc_parray[unit];

    rate_calc = (stream_rate_t *)
                 sal_alloc(sizeof(stream_rate_t), "rate_calc");
    if (rate_calc == NULL) {
        test_error(unit, "Failed to allocate memory for rate_calc\n");
        return BCM_E_FAIL;
    }
    sal_memset(rate_calc, 0, sizeof(stream_rate_t));

    rate_calc->mode         = 0; /* check act_rate against config_rate */
    rate_calc->pkt_size     = l2mc_p->pkt_size_param;
    rate_calc->interval_len = l2mc_p->rate_calc_interval_param;
    rate_calc->tolerance_lr = l2mc_p->rate_tolerance_lr_param;
    rate_calc->tolerance_os = l2mc_p->rate_tolerance_ov_param;
    rate_calc->scaling_factor = l2mc_p->scaling_factor_param;

    SOC_PBMP_CLEAR(rate_calc->pbmp);
    for (i = 0; i < l2mc_p->num_streams; i++) {
        for (j = 0; j < (l2mc_p->num_copy_param + 1); j++) {
            port_idx = l2mc_p->stream[i][j];
            port = l2mc_p->port_list[port_idx];
            if (port < SOC_MAX_NUM_PORTS) {
                /* pbmp */
                SOC_PBMP_PORT_ADD((rate_calc->pbmp), port);
                /* num_rep */
/*                 if (l2mc_p->l3_copy[port_idx] == 0) {
                    rate_calc->num_rep[port] = 0;
                } else {
                    rate_calc->num_rep[port] = l2mc_p->num_rep[port_idx] + 1;
                } */
                /* src_port */
                rate_calc->src_port[port] =
                        l2mc_p->port_list[l2mc_p->stream[i][1]];
            }
        }
    }

    rv = stream_chk_port_rate(unit, PBMP_PORT_ALL(unit), rate_calc);

    sal_free(rate_calc);
    return rv;
}

/*
 * Function:
 *      l2mc_hg_disable_vlan_ports
 * Purpose:
 *      Workaround to diasble vlan PORT_BITMAP,
 *      to avoid forwarding between vlan ports after vlan disable
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      disable - disable vlan ports
 *
 * Returns:
 *     Nothing
 */
static void
l2mc_hg_disable_vlan_ports(int unit, int disable)
{
    int i, vlan, param_num_stream, stream_hg_en;
    l2mc_t *l2mc_p = l2mc_parray[unit];
    vlan_2_tab_entry_t vlan_2_tab_entry;
    soc_pbmp_t pbm;

    if (!SOC_IS_TOMAHAWKX(unit)) {
        return;
    }

    param_num_stream = l2mc_p->num_streams;
    for (i = 0; i < param_num_stream; i++) {
        stream_hg_en = l2mc_p->hg_stream[i];
        if (!stream_hg_en) {
            continue;
        }

        vlan = l2mc_p->tx_vlan[i] + 1;
        (void) soc_mem_read(unit, VLAN_2_TABm, COPYNO_ALL, vlan,
                            vlan_2_tab_entry.entry_data);
        if (disable) {
            SOC_PBMP_CLEAR(pbm);
        } else {
            (void) soc_mem_pbmp_field_get(unit, VLAN_2_TABm,
                vlan_2_tab_entry.entry_data, ING_PORT_BITMAPf, &pbm);
        }
        (void) soc_mem_pbmp_field_set(unit, VLAN_2_TABm,
            vlan_2_tab_entry.entry_data, PORT_BITMAPf, &pbm);
        (void) soc_mem_write(unit, VLAN_2_TABm, COPYNO_ALL, vlan,
                             vlan_2_tab_entry.entry_data);
    }
}

/*
 * Function:
 *      l2mc_chk_pkt_integrity
 * Purpose:
 *      Redirect all packets back to CPU and check packet integrity
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
static bcm_error_t
l2mc_chk_pkt_integrity(int unit)
{
    int i, port, port_speed, stream_hg_en, vlan_id;
    int param_num_stream, param_pkt_seed;
    int *param_port_list, **param_stream, *param_port_speed,
        *param_hg_stream;
    uint32 flood_cnt;
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    stream_integrity_t *pkt_intg;
    bcm_error_t rv = BCM_E_NONE;
    l2mc_t *l2mc_p = l2mc_parray[unit];

    param_num_stream = l2mc_p->num_streams;
    param_pkt_seed = l2mc_p->pkt_seed;
    param_port_list = l2mc_p->port_list;
    param_stream = l2mc_p->stream;
    param_port_speed = l2mc_p->port_speed;
    param_hg_stream = l2mc_p->hg_stream;

    pkt_intg = sal_alloc(sizeof(stream_integrity_t), "pkt_intg");
    if (pkt_intg == NULL) {
        test_error(unit, "Failed to allocate memory for pkt_intg\n");
        return BCM_E_FAIL;
    }
    sal_memset(pkt_intg, 0, sizeof(stream_integrity_t));

    pkt_intg->type = PKT_TYPE_L2;
    SOC_PBMP_CLEAR(pkt_intg->rx_pbmp);
    for (i = 0; i < param_num_stream; i++) {
        port = param_port_list[param_stream[i][0]];
        port_speed = param_port_speed[param_stream[i][0]];
        stream_hg_en = param_hg_stream[i];
        if (port < SOC_MAX_NUM_PORTS) {
            /* rx_pbmp */
            SOC_PBMP_PORT_ADD(pkt_intg->rx_pbmp, port);

            /* rx_vlan: used to forward pkt to CPU */
            if (stream_hg_en == 1) {
                vlan_id = l2mc_p->tx_vlan[i] + 1;
            } else {
                vlan_id = l2mc_p->tx_vlan[i];
            }
            pkt_intg->rx_vlan[port] = vlan_id;

            /* tx_vlan: used to re-generate ref_pkt */
            vlan_id = l2mc_p->tx_vlan[i] + 1;
            pkt_intg->tx_vlan[port] = vlan_id;

            /* port_flood_cnt */
            flood_cnt = l2mc_lossless_flood_cnt(unit, port_speed, stream_hg_en);
            if (stream_hg_en == 0) {
                flood_cnt = 2 * flood_cnt;
            }
            pkt_intg->port_flood_cnt[port] = flood_cnt;

            /* port_pkt_seed */
            pkt_intg->port_pkt_seed[port] = param_pkt_seed + i;

            /* mac_da, mac_sa */
            sal_memcpy(pkt_intg->mac_da[port], mac_da, NUM_BYTES_MAC_ADDR);
            sal_memcpy(pkt_intg->mac_sa[port], mac_sa, NUM_BYTES_MAC_ADDR);
        }
    }

    l2mc_hg_disable_vlan_ports(unit, 1);
    if (stream_chk_pkt_integrity(unit, pkt_intg) != BCM_E_NONE) {
        rv = BCM_E_FAIL;
    }
    l2mc_hg_disable_vlan_ports(unit, 0);

    sal_free(pkt_intg);
    return rv;
}

/*
 * Function:
 *      l2mc_test_init
 * Purpose:
 *      Test init function. Parse CLI params and do necessary init.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
l2mc_test_init(int unit, args_t *a, void **pa)
{
    l2mc_t *l2mc_p;

    l2mc_p = l2mc_parray[unit];
    l2mc_p = sal_alloc(sizeof(l2mc_t), "l2mc_test");
    sal_memset(l2mc_p, 0, sizeof(l2mc_t));
    l2mc_parray[unit] = l2mc_p;
    cli_out("\n==================================================");
    cli_out("\nCalling l2mc_test_init ...\n");
    l2mc_parse_test_params(unit, a);

    l2mc_p->test_fail = 0;

    if (l2mc_p->bad_input == 1) {
        goto done;
    }

    stream_set_mac_lpbk(unit, PBMP_PORT_ALL(unit));
    stream_turn_off_cmic_mmu_bkp(unit);
    stream_turn_off_fc(unit, PBMP_PORT_ALL(unit));
    /* coverity[dont_call : FALSE] */
    l2mc_p->pkt_seed = sal_rand();

done:
    return 0;
}

/*
 * Function:
 *      l2mc_test
 * Purpose:
 *      Set up ports/streams and send packets.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
l2mc_test(int unit, args_t *a, void *pa)
{
    l2mc_t *l2mc_p = l2mc_parray[unit];

    if (l2mc_p->bad_input == 1) {
        goto done;
    }

    cli_out("\n==================================================");
    cli_out("\nCalling l2mc_test ... \n");
    l2mc_set_port_property_arrays(unit);
    l2mc_set_up_ports(unit);
    l2mc_set_up_streams(unit);
    l2mc_send_pkts(unit);

    /* check counter */
    if (stream_chk_mib_counters(unit, PBMP_PORT_ALL(unit), 0) != BCM_E_NONE) {
        l2mc_p->test_fail = 1;
    }
    /* check rate */
    if (l2mc_chk_port_rate(unit) != BCM_E_NONE) {
        l2mc_p->test_fail = 1;
    }
    /* check integrity */
    if (l2mc_chk_pkt_integrity(unit) != BCM_E_NONE) {
        l2mc_p->test_fail = 1;
    }

done:
    return 0;
}

/*
 * Function:
 *      l2mc_test_cleanup
 * Purpose:
 *      Do test end checks and free all allocated memory.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
l2mc_test_cleanup(int unit, void *pa)
{
    int i, rv;
    l2mc_t *l2mc_p = l2mc_parray[unit];

    if (l2mc_p->bad_input == 1) {
        goto done;
    }
    cli_out("\nCalling l2mc_test_cleanup");

    /* free memory */
    sal_free(l2mc_p->port_speed);
    sal_free(l2mc_p->port_list);
    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        sal_free(l2mc_p->stream[i]);
    }
    sal_free(l2mc_p->stream);
    sal_free(l2mc_p->tx_vlan);
    sal_free(l2mc_p->hg_stream);
    sal_free(l2mc_p->port_oversub);
    sal_free(l2mc_p->ovs_ratio_x1000);
    sal_free(l2mc_p->rate);
    sal_free(l2mc_p->exp_rate);
    sal_free(l2mc_p->tpkt_start);
    sal_free(l2mc_p->tpkt_end);
    sal_free(l2mc_p->tbyt_start);
    sal_free(l2mc_p->tbyt_end);

    for (i = 0; i < l2mc_p->num_fp_ports; i++) {
        sal_free(l2mc_p->rand_pkt_sizes[i]);
    }
    sal_free(l2mc_p->rand_pkt_sizes);

done:
    if (l2mc_p->bad_input == 1) {
        l2mc_p->test_fail = 1;
    }

    if (l2mc_p->test_fail == 1) {
        rv = BCM_E_FAIL;
    }
    else {
        rv = BCM_E_NONE;
    }

    cli_out("\n==================================================");
    cli_out("\n==================================================");
    if (l2mc_p->test_fail == 1) {
        cli_out("\n[L2MC test failed]\n\n");
    } else {
        cli_out("\n[L2MC test passed]\n\n");
    }
    sal_free(l2mc_p);

    return rv;
}
#endif /* BCM_ESW_SUPPORT */
