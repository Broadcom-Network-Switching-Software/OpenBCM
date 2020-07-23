/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * L2UC Streaming test to check basic L2 packet switching. The test pairs up
 * same speed ports and creates L2UC traffic swills. Test also
 * calculates expected rates based on port config and oversub ratio, measures
 * actual rate over a period of time and conducts rate checks. Finally all
 * packets in each swill will be redirected to the CPU and checked for integrity.
 *
 * Configuration parameters passed from CLI:
 * PktSize: Packet size in bytes. Set to 0 for worst case packet sizes on all
 *          ports (145B for ENET, 76B for HG2). Set to 1 for random packet sizes
 * FloodCnt: Number of packets in each swill. Setting this to 0 will let the
 *           test calculate the number of packets that can be sent to achieve
 *           a lossless swirl at full rate. Set to 0 by default.
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

#include "testlist.h"
#include "gen_pkt.h"
#include "streaming_lib.h"

#define MAC_DA {0x12,0x34,0x56,0x78,0x9a,0xbc}
#define MAC_SA {0xfe,0xdc,0xba,0x98,0x76,0x54}
#define VLAN 0x0a00

#define XLATE_VLAN0 0x0100
#define XLATE_VLAN1 0x0400

#define MAX_FP_PORTS 130
#define MAX_NUM_CELLS_PARAM_DEFAULT 4

#define HG2_MY_MODID 100
#define MODPORT_MAP_OFFSET 256

#define NULL_DEST 999

#define PKT_SIZE_PARAM_DEFAULT 0
#define FLOOD_PKT_CNT_PARAM_DEFAULT 0
#define RATE_CALC_INTERVAL_PARAM_DEFAULT 10
#define RATE_TOLERANCE_LR_PARAM_DEFAULT 1
#define RATE_TOLERANCE_OV_PARAM_DEFAULT 3
#define CHECK_PACKET_INTEGRITY_PARAM_DEFAULT 1
#define SPEEDUP_FACTOR_PARAM_DEFAULT 1
#define NUM_LPORT_TAB_ENTRIES 256
#define NUM_PORT_TAB_ENTRIES 136

#if defined(BCM_ESW_SUPPORT) && \
  (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))

typedef struct l2uc_s {
    uint32 num_fp_ports;
    uint32 num_streams;
    int *port_speed;
    int *port_list;
    int *dest_port;
    uint32 *stream;
    int *hg_stream;
    uint32 *modid;
    uint32 *port_oversub;
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
    uint32 rate_calc_interval_param;
    uint32 rate_tolerance_lr_param;
    uint32 rate_tolerance_ov_param;
    uint32 check_packet_integrity_param;
    uint32 max_num_cells_param;
    uint32 scaling_factor_param;
    uint32 bad_input;
    int test_fail;
    uint32 pkt_seed;
} l2uc_t;

static l2uc_t *l2uc_parray[SOC_MAX_NUM_DEVICES];


/*
 * Function:
 *      l2uc_lossless_flood_cnt
 * Purpose:
 *      Calculates number of packets that need to be sent to a port for a
 *      lossless swirl
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pkt_size : Packet size in bytes
 *      port: Test port no
 * Returns:
 *     Number of packets needed for lossless flooding
 */
static uint32
l2uc_lossless_flood_cnt(int unit, int port_idx, int hg_en)
{
    uint32 port, num_fp_ports, pkt_size, flood_cnt;
    uint32 *rand_pkt_size;
    int param_flood_cnt, param_pkt_size;
    l2uc_t *l2uc_p = l2uc_parray[unit];

    port = l2uc_p->port_list[port_idx];
    num_fp_ports = l2uc_p->num_fp_ports;
    rand_pkt_size = l2uc_p->rand_pkt_sizes[port_idx];
    param_flood_cnt = l2uc_p->flood_pkt_cnt_param;
    param_pkt_size = l2uc_p->pkt_size_param;

    if (param_flood_cnt == 0) {
        if (param_pkt_size == 0) {
            pkt_size = stream_get_wc_pkt_size(unit, hg_en);
        } else {
            pkt_size = param_pkt_size;
        }
        flood_cnt = stream_get_ll_flood_cnt(unit, port, pkt_size,
                                            rand_pkt_size);
        if (num_fp_ports > 0) {
            flood_cnt = (flood_cnt * MAX_FP_PORTS) / num_fp_ports;
            if (flood_cnt % 2 == 1) {
                flood_cnt++;
            }
        }
    } else {
        flood_cnt = param_flood_cnt;
    }

    return(flood_cnt);
}

/*
 * Function:
 *      l2uc_chk_port_rate
 * Purpose:
 *      Check actual port rates against expected port rates.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     SOC_E_XXXX
 */
static bcm_error_t
l2uc_chk_port_rate(int unit)
{
    int i, port;
    bcm_error_t rv = BCM_E_NONE;
    stream_rate_t *rate_calc;
    l2uc_t *l2uc_p = l2uc_parray[unit];

    rate_calc = (stream_rate_t *)
                 sal_alloc(sizeof(stream_rate_t), "rate_calc");
    if (rate_calc == NULL) {
        test_error(unit, "Failed to allocate memory for rate_calc\n");
        return BCM_E_FAIL;
    }
    sal_memset(rate_calc, 0, sizeof(stream_rate_t));

    rate_calc->mode         = 0; /* check act_rate against config_rate */
    rate_calc->pkt_size     = l2uc_p->pkt_size_param;
    rate_calc->interval_len = l2uc_p->rate_calc_interval_param;
    rate_calc->tolerance_lr = l2uc_p->rate_tolerance_lr_param;
    rate_calc->tolerance_os = l2uc_p->rate_tolerance_ov_param;
    rate_calc->scaling_factor = l2uc_p->scaling_factor_param;

    SOC_PBMP_CLEAR(rate_calc->pbmp);
    for (i = 0; i < l2uc_p->num_fp_ports; i++) {
        port = l2uc_p->port_list[i];
        if (port < SOC_MAX_NUM_PORTS) {
            SOC_PBMP_PORT_ADD(rate_calc->pbmp, port);
            /* src_port */
            rate_calc->src_port[port] = port;
        }
    }

    rv = stream_chk_port_rate(unit, PBMP_PORT_ALL(unit), rate_calc);

    sal_free(rate_calc);
    return rv;
}

/*
 * Function:
 *      l2uc_chk_pkt_integrity
 * Purpose:
 *      Redirect all packets back to CPU and check packet integrity
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     SOC_E_XXXX
 */
static bcm_error_t
l2uc_chk_pkt_integrity(int unit)
{
    int i, port, port_idx, vlan_id;
    int param_pkt_seed;
    uint32 flood_cnt;
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    stream_integrity_t *pkt_intg;
    bcm_error_t rv = BCM_E_NONE;
    l2uc_t *l2uc_p = l2uc_parray[unit];

    param_pkt_seed = l2uc_p->pkt_seed;

    pkt_intg = sal_alloc(sizeof(stream_integrity_t), "pkt_intg");
    if (pkt_intg == NULL) {
        test_error(unit, "Failed to allocate memory for pkt_intg\n");
        return BCM_E_FAIL;
    }
    sal_memset(pkt_intg, 0, sizeof(stream_integrity_t));

    pkt_intg->type = PKT_TYPE_L2;
    SOC_PBMP_CLEAR(pkt_intg->rx_pbmp);
    for (i = 0; i < l2uc_p->num_streams; i++) {
        port_idx = l2uc_p->dest_port[l2uc_p->stream[i]];
        port = l2uc_p->port_list[port_idx];
        if (port < SOC_MAX_NUM_PORTS) {
            /* rx_pbmp */
            SOC_PBMP_PORT_ADD(pkt_intg->rx_pbmp, port);
            /* rx_vlan */
            if (l2uc_p->hg_stream[i] == 1) {
                vlan_id = VLAN + i;
            } else {
                vlan_id = XLATE_VLAN0 + i;
            }
            pkt_intg->rx_vlan[port] = vlan_id;
            /* tx_vlan */
            if (l2uc_p->hg_stream[i] == 1) {
                vlan_id = VLAN + i;
            } else {
                vlan_id = XLATE_VLAN1 + i;
            }
            pkt_intg->tx_vlan[port] = vlan_id;
            /* port_flood_cnt */
            flood_cnt = l2uc_lossless_flood_cnt(unit, port_idx,
                                                l2uc_p->hg_stream[i]);
            pkt_intg->port_flood_cnt[port] = flood_cnt * 2;
            /* port_pkt_seed */
            pkt_intg->port_pkt_seed[port] = param_pkt_seed + i;
            /* mac_da, mac_sa */
            sal_memcpy(pkt_intg->mac_da[port], mac_da, NUM_BYTES_MAC_ADDR);
            sal_memcpy(pkt_intg->mac_sa[port], mac_sa, NUM_BYTES_MAC_ADDR);
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
 *      l2uc_parse_test_params
 * Purpose:
 *      Parse CLI parameters, create test structure and flag bad inputs.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     Nothing
 * Notes:
 *      l2uc_p->bad_input set from here - tells test to crash out in case
 *      CLI input combination is invalid.
 */
static void
l2uc_parse_test_params(int unit, args_t *a)
{
    parse_table_t parse_table;
    l2uc_t *l2uc_p = l2uc_parray[unit];

    const char tr510_test_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nDocumentation too long to be displayed with -pedantic compiler\n";
#else
    "\nL2UC Streaming test to check basic L2 packet switching. The test pairs up\n"
    "same speed ports and creates L2UC traffic swills. Test also\n"
    "calculates expected rates based on port config and oversub ratio, measures\n"
    "actual rate over a period of time and conducts rate checks. Finally all\n"
    "packets in each swill will be redirected to the CPU and checked for integrity.\n"
    "\n"
    "Configuration parameters passed from CLI:\n"
    "PktSize: Packet size in bytes. Set to 0 for worst case packet sizes on all\n"
    "         ports (145B for ENET, 76B for HG2). Set to 1 for random packet sizes\n"
    "FloodCnt: Number of packets in each swill. Setting this to 0 will let the\n"
    "          test calculate the number of packets that can be sent to achieve\n"
    "          a lossless swirl at full rate. Set to 0 by default.\n"
    "RateCalcInt: Interval in seconds over which rate is to be calculated\n"
    "TolLr: Rate tolerance percentage for linerate ports (1 percentage by default).\n"
    "TolOv: Rate tolerance percentage for oversubscribed ports (3 percentage by default).\n"
    "ChkPktInteg: Set to 0 to disable packet integrity checks, 1 to enable (default).\n"
    "MaxNumCells: Max number of cells for random packet sizes. Default = 4. Set\n"
    "             to 0 for random.\n";
#endif

    l2uc_p->bad_input = 0;

    l2uc_p->pkt_size_param = PKT_SIZE_PARAM_DEFAULT;
    l2uc_p->flood_pkt_cnt_param = FLOOD_PKT_CNT_PARAM_DEFAULT;
    l2uc_p->rate_calc_interval_param = RATE_CALC_INTERVAL_PARAM_DEFAULT;
    l2uc_p->rate_tolerance_lr_param = RATE_TOLERANCE_LR_PARAM_DEFAULT;
    l2uc_p->rate_tolerance_ov_param = RATE_TOLERANCE_OV_PARAM_DEFAULT;
    l2uc_p->check_packet_integrity_param
                            = CHECK_PACKET_INTEGRITY_PARAM_DEFAULT;
    l2uc_p->max_num_cells_param = MAX_NUM_CELLS_PARAM_DEFAULT;
    l2uc_p->scaling_factor_param = SPEEDUP_FACTOR_PARAM_DEFAULT;

    /*Parse CLI opts */

    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table, "PktSize", PQ_INT|PQ_DFL, 0,
                    &(l2uc_p->pkt_size_param), NULL);
    parse_table_add(&parse_table, "FloodCnt", PQ_INT|PQ_DFL, 0,
                    &(l2uc_p->flood_pkt_cnt_param), NULL);
    parse_table_add(&parse_table, "RateCalcInt", PQ_INT|PQ_DFL, 0,
                    &(l2uc_p->rate_calc_interval_param), NULL);
    parse_table_add(&parse_table, "TolLr", PQ_INT|PQ_DFL, 0,
                    &(l2uc_p->rate_tolerance_lr_param), NULL);
    parse_table_add(&parse_table, "TolOv", PQ_INT|PQ_DFL, 0,
                    &(l2uc_p->rate_tolerance_ov_param), NULL);
    parse_table_add(&parse_table, "ChkPktInteg", PQ_INT|PQ_DFL, 0,
                    &(l2uc_p->check_packet_integrity_param), NULL);
    parse_table_add(&parse_table, "MaxNumCells", PQ_INT|PQ_DFL, 0,
                    &(l2uc_p->max_num_cells_param), NULL);
    parse_table_add(&parse_table, "ScalingFactor", PQ_INT|PQ_DFL, 0,
                    &(l2uc_p->scaling_factor_param), NULL);

    if (parse_arg_eq(a, &parse_table) < 0 || ARG_CNT(a) != 0) {
        test_msg("%s", tr510_test_usage);
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        l2uc_p->bad_input = 1;
        parse_arg_eq_done(&parse_table);
    } else {
        cli_out("\n");
        cli_out("------------- PRINTING TEST PARAMS ------------------\n");
        cli_out("PktSize      = %0d\n", l2uc_p->pkt_size_param);
        cli_out("RateCalcInt  = %0d\n", l2uc_p->rate_calc_interval_param);
        cli_out("FloodCnt     = %0d\n", l2uc_p->flood_pkt_cnt_param);
        cli_out("TolLr        = %0d\n", l2uc_p->rate_tolerance_lr_param);
        cli_out("TolOv        = %0d\n", l2uc_p->rate_tolerance_ov_param);
        cli_out("ChkPktInteg  = %0d\n", l2uc_p->check_packet_integrity_param);
        cli_out("MaxNumCells  = %0d\n", l2uc_p->max_num_cells_param);
        cli_out("ScalingFactor= %0d\n", l2uc_p->scaling_factor_param);
        cli_out("-----------------------------------------------------\n");
    }

    if (l2uc_p->max_num_cells_param == 0) {
        /* coverity[dont_call : FALSE] */
        l2uc_p->max_num_cells_param = (sal_rand() % (MTU_CELL_CNT - 1)) + 1;
    }

    if (l2uc_p->pkt_size_param == 0) {
        cli_out
            ("\nUsing worst case packet sizes - 145B for Ethernet "
             "and 76B (64B + 12B hg-hdr) for HG2");
    } else if (l2uc_p->pkt_size_param == 1) {
        cli_out("\nUsing random packet sizes");
    } else if (l2uc_p->pkt_size_param < MIN_PKT_SIZE) {
        test_error(unit,"\n*ERROR: Packet size cannot be lower than %0dB\n",
                MIN_PKT_SIZE);
        l2uc_p->bad_input = 1;
    } else if (l2uc_p->pkt_size_param > MTU) {
        test_error(unit,"\n*ERROR: Packet size cannot be higher than %0dB (Device MTU)\n",
                MTU);
        l2uc_p->bad_input = 1;
    }

    if (l2uc_p->flood_pkt_cnt_param == 0) {
        cli_out("\nFloodCnt=0, test will automatically calculate number of"
                " packets to flood each port");
    }
}


/*
 * Function:
 *      l2uc_set_port_property
 * Purpose:
 *      Set port_list, port_speed and port_oversub arrays. Also call set_exp_rates
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Safe packet size.
 */
static void
l2uc_set_port_property(int unit)
{
    int p, i, j, paired_port;
    uint32 pkt_size, can_be_paired = 0;
    soc_info_t *si = &SOC_INFO(unit);
    l2uc_t *l2uc_p = l2uc_parray[unit];

    l2uc_p->num_fp_ports = 0;
    l2uc_p->num_streams = 0;

    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        l2uc_p->num_fp_ports++;
    }

    l2uc_p->port_list =
        (int *)sal_alloc(l2uc_p->num_fp_ports * sizeof(int),
                         "port_list");
    l2uc_p->port_speed =
        (int *)sal_alloc(l2uc_p->num_fp_ports * sizeof(int),
                         "port_speed_array");
    l2uc_p->dest_port =
        (int *)sal_alloc(l2uc_p->num_fp_ports * sizeof(int),
                          "dest_port_array");
    l2uc_p->stream =
        (uint32 *)sal_alloc(l2uc_p->num_fp_ports * sizeof(uint32),
                          "stream");
    l2uc_p->modid =
        (uint32 *)sal_alloc(l2uc_p->num_fp_ports * sizeof(int),
                          "modid_array");
    l2uc_p->hg_stream =
        (int *)sal_alloc(l2uc_p->num_fp_ports * sizeof(int),
                         "hg_stream_array");
    l2uc_p->port_oversub =
        (uint32 *) sal_alloc(l2uc_p->num_fp_ports * sizeof(uint32),
                             "port_oversub_array");
    l2uc_p->rand_pkt_sizes =
        (uint32 **) sal_alloc(l2uc_p->num_fp_ports * sizeof(uint32 *),
                              "rand_pkt_sizes_array*");

    for (i = 0; i < l2uc_p->num_fp_ports; i++) {
        l2uc_p->rand_pkt_sizes[i] =
            (uint32 *) sal_alloc(TARGET_CELL_COUNT * sizeof(uint32),
                                "rand_pkt_sizes_array");
    }

    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        l2uc_p->port_list[i] = p;
        i++;
    }

    for (i = 0; i < l2uc_p->num_fp_ports; i++) {
        l2uc_p->port_speed[i] =
            si->port_speed_max[l2uc_p->port_list[i]];

        switch(l2uc_p->port_speed[i]) {
            case 11000: l2uc_p->port_speed[i] = 10600; break;
            case 27000: l2uc_p->port_speed[i] = 26500; break;
            case 42000: l2uc_p->port_speed[i] = 42400; break;
            default: break;
        }
    }

    for (i = 0; i < l2uc_p->num_fp_ports; i++) {
        if (SOC_PBMP_MEMBER(si->oversub_pbm, l2uc_p->port_list[i]) &&
            (!(SOC_PBMP_MEMBER(si->management_pbm, l2uc_p->port_list[i])))) {
            l2uc_p->port_oversub[i] = 1;
        } else {
            l2uc_p->port_oversub[i] = 0;
        }
    }

    for (i = 0; i < l2uc_p->num_fp_ports; i++) {
        l2uc_p->dest_port[i] = NULL_DEST;
        l2uc_p->hg_stream[i] = 0;
    }

    /* a trick to accelerate port grouping, if same speed eth and hg */
    for (i = 0; i < l2uc_p->num_fp_ports; i++) {
        if (IS_HG_PORT(unit, l2uc_p->port_list[i])) {
            l2uc_p->port_speed[i] |= 0x1;
        }
    }

    /* set src and dst ports for each stream */
    for (i = 0; i < l2uc_p->num_fp_ports; i++) {
        can_be_paired = 0;
        if(l2uc_p->dest_port[i] == NULL_DEST) {
            for (j = 0;j < l2uc_p->num_fp_ports; j++) {
                if((l2uc_p->dest_port[j] == NULL_DEST)
                        && (l2uc_p->port_speed[i]
                                    == l2uc_p->port_speed[j])
                        && (i!=j)) {
                    can_be_paired = 1;
                }
            }

            if(can_be_paired) {
                do {
                    paired_port = sal_rand() % l2uc_p->num_fp_ports;
                }while((l2uc_p->port_speed[i] !=
                                l2uc_p->port_speed[paired_port])
                        || (l2uc_p->dest_port[paired_port] != NULL_DEST)
                                                        || (i == paired_port));

                l2uc_p->dest_port[i] = paired_port;
                l2uc_p->dest_port[paired_port] = i;
                l2uc_p->stream[l2uc_p->num_streams] = i;
                if (IS_HG_PORT(unit, l2uc_p->port_list[i])) {
                    l2uc_p->hg_stream[l2uc_p->num_streams] = 1;
                    l2uc_p->modid[i] = l2uc_p->num_streams;
                    l2uc_p->modid[paired_port] = l2uc_p->num_streams;
                }
                l2uc_p->num_streams++;
            }
        }
    }

    /* restore hg speed */
    for (i = 0; i < l2uc_p->num_fp_ports; i++) {
        if (IS_HG_PORT(unit, l2uc_p->port_list[i])) {
            l2uc_p->port_speed[i] &= ~0x1;
        }
    }

    /* random packet size */
    for (i = 0; i < l2uc_p->num_fp_ports; i++) {
        for (j = 0; j < TARGET_CELL_COUNT; j++) {
            do {
                /* coverity[dont_call : FALSE] */
                pkt_size = (sal_rand() % (MTU - MIN_PKT_SIZE + 1)) + MIN_PKT_SIZE;
            } while (stream_get_pkt_cell_cnt(unit, pkt_size, l2uc_p->port_list[i]) >
                            l2uc_p->max_num_cells_param);
            l2uc_p->rand_pkt_sizes[i][j] = pkt_size;
        }
    }

    {
        /* print */
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\n==== STREAM INFO (VERBOSE) =====\n")));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\n%8s%8s%8s%8s%8s%8s\n"),
                             "idx", "port", "dst", "speed", "ovsb", "HiGig"));
        for (i = 0; i < l2uc_p->num_fp_ports; i++) {
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "%8d%8d%8d%7dG"),
                                 i,
                                 l2uc_p->port_list[i],
                                 l2uc_p->port_list[l2uc_p->dest_port[i]],
                                 l2uc_p->port_speed[i]/1000));
            if (SOC_PBMP_MEMBER(si->oversub_pbm, l2uc_p->port_list[i])) {
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "%8s"), "OS"));
            } else {
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "%8s"), "--"));
            }
            if (IS_HG_PORT(unit, l2uc_p->port_list[i])) {
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "%8s"), "HG"));
            } else {
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "%8s"), "--"));
            }
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\n")));
        }
    }
}

/*
 * Function:
 *      l2uc_set_up_ports
 * Purpose:
 *      Programs port table to set unique modid for each HG2 port. Also
 *      programs ING_VLAN_TAG_ACTION_PROFILEm to remove the inner tag for
 *      double tagged packets.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
static void
l2uc_set_up_ports(int unit)
{
    int p;
    port_tab_entry_t port_tab_entry;
    ing_vlan_tag_action_profile_entry_t ing_vlan_tag_action_profile_entry;

    if (SOC_MEM_IS_VALID(unit, PORT_TABm)) {
        PBMP_ITER(PBMP_PORT_ALL(unit), p) {
            if (IS_HG_PORT(unit, p)) {
                (void) soc_mem_read(unit, PORT_TABm, COPYNO_ALL,
                             p, port_tab_entry.entry_data);
                soc_mem_field32_set(unit, PORT_TABm, port_tab_entry.entry_data,
                                    MY_MODIDf, (HG2_MY_MODID + p));
                (void) soc_mem_write(unit, PORT_TABm, COPYNO_ALL, p,
                                port_tab_entry.entry_data);
            }
        }
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
}

/*
 * Function:
 *      set_modport_map_index_upper
 * Purpose:
 *      Set MODPORT_MAP_SEL.MODPORT_MAP_INDEX_UPPER to a given value
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      port : Device Port no
 *      index : Value for MODPORT_MAP_INDEX_UPPER
 *
 * Returns:
 *     Nothing.
 */
static void
set_modport_map_index_upper(int unit, int port, uint32 index)
{
    if (SOC_REG_IS_VALID(unit, MODPORT_MAP_SELr)) {
        soc_reg_field32_modify(unit, MODPORT_MAP_SELr, port,
                               MODPORT_MAP_INDEX_UPPERf, index);
    } else {
        cli_out("\n*ERROR, invalid reg %s\n",
                SOC_REG_NAME(unit, MODPORT_MAP_SELr));
    }
}

/*
 * Function:
 *      set_modport_map_sw
 * Purpose:
 *      Program MODPORT_MAP_SW to set destination port for a given HG2 port
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      modid : Modid for HG2 port
 *      index_upper: MODPORT_MAP_SEL.MODPORT_MAP_INDEX_UPPER
 *      dest0: Dest device port
 *
 * Returns:
 *     Nothing.
 */
static void
set_modport_map_sw(int unit, uint32 modid, uint32 index_upper, uint32 dest0)
{
    modport_map_sw_entry_t modport_map_sw_entry;
    uint32 index;

    if (SOC_MEM_IS_VALID(unit, MODPORT_MAP_SWm)) {
        index = (index_upper * MODPORT_MAP_OFFSET) + modid;

        (void) soc_mem_read(unit, MODPORT_MAP_SWm, COPYNO_ALL, index,
                     modport_map_sw_entry.entry_data);
        soc_mem_field32_set(unit, MODPORT_MAP_SWm,
                            modport_map_sw_entry.entry_data, DEST0f, dest0);
        soc_mem_field32_set(unit, MODPORT_MAP_SWm,
                            modport_map_sw_entry.entry_data, ENABLE0f, 0x1);
        (void) soc_mem_write(unit, MODPORT_MAP_SWm, COPYNO_ALL, index,
                      modport_map_sw_entry.entry_data);
    } else {
        cli_out("\n*ERROR, invalid mem %s\n",
                SOC_MEM_NAME(unit, MODPORT_MAP_SWm));
    }
}

/*
 * Function:
 *      l2uc_set_up_streams
 * Purpose:
 *      Program switch to set up L2UC streams between port pairs. For Ethernet
 *      ports this involves setting up VLAN translations and L2 entries to get
 *      a packet flow of CPU -> P0 (Vlan0, Xlate_Vlan0) -> P1(Xlate_vlan1) -> P0 ...
 *      For HG2 ports the modport map table is programmed such that packets
 *      coming from P0 are directed to P1 and packets coming from P1 are directed
 *      to P0. Packets coming from the CPU are broadcast to both P0 and P1. The
 *      HG2 flow is CPU ->P0,P1 and P0->P1->P0 and P1->P0->P1.
 *
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 *
 * Returns:
 *     Nothing
 */
static void
l2uc_set_up_streams(int unit)
{
    int i;
    pbmp_t pbm, ubm;
    uint32 vlan = VLAN;
    uint32 xlate_vlan0 = XLATE_VLAN0;
    uint32 xlate_vlan1 = XLATE_VLAN1;
    uint32 *port_accounted_for;
    l2uc_t *l2uc_p = l2uc_parray[unit];
    bcm_l2_addr_t l2_addr;
    bcm_mac_t mac_da = MAC_DA;

    port_accounted_for = (uint32*)sal_alloc(l2uc_p->num_fp_ports
                                        * sizeof(uint32), "port_accounted_for");

    for (i = 0; i < l2uc_p->num_fp_ports; i++) {
        port_accounted_for[i] = 0;
    }

    BCM_PBMP_CLEAR(ubm);
    bcm_vlan_destroy_all(unit);
    bcm_vlan_control_set(unit, bcmVlanTranslate, TRUE);

    for (i = 0; i < l2uc_p->num_fp_ports; i++) {
        if (l2uc_p->dest_port[i] != NULL_DEST
            && port_accounted_for[i] == 0
            && port_accounted_for[l2uc_p->dest_port[i]] == 0) {
            if (IS_HG_PORT(unit, l2uc_p->port_list[i])) {
                BCM_PBMP_CLEAR(pbm);
                bcm_vlan_create(unit, (bcm_vlan_t) (vlan));
                BCM_PBMP_PORT_ADD(pbm, l2uc_p->port_list[i]);
                BCM_PBMP_PORT_ADD(pbm, l2uc_p->port_list[l2uc_p->
                                                                dest_port[i]]);
                bcm_vlan_port_add(unit, (bcm_vlan_t) (vlan), pbm, ubm);
                port_accounted_for[i] = 1;
                port_accounted_for[l2uc_p->dest_port[i]] = 1;

                set_modport_map_index_upper(unit, l2uc_p->port_list[i], 0x1);
                set_modport_map_sw(unit, l2uc_p->modid[i],
                                   0x0, l2uc_p->port_list[i]);
                set_modport_map_index_upper(unit, l2uc_p->port_list[l2uc_p->
                                                            dest_port[i]], 0x0);
                set_modport_map_sw(unit, l2uc_p->modid[l2uc_p->dest_port[i]],
                                   0x1, l2uc_p->port_list[l2uc_p->dest_port[i]]);

                bcm_l2_addr_t_init(&l2_addr, mac_da, (bcm_vlan_t)(vlan));
                l2_addr.port = 1;
                l2_addr.modid = l2uc_p->modid[i];
                l2_addr.flags = 0;
                bcm_l2_addr_add(unit, &l2_addr);
            } else {
                bcm_vlan_create(unit, (bcm_vlan_t) (vlan));
                bcm_vlan_create(unit, (bcm_vlan_t) (xlate_vlan0));
                bcm_vlan_create(unit, (bcm_vlan_t) (xlate_vlan1));
                BCM_PBMP_CLEAR(pbm);
                BCM_PBMP_PORT_ADD(pbm, l2uc_p->port_list[i]);
                BCM_PBMP_PORT_ADD(pbm, l2uc_p->port_list[l2uc_p->
                                                                dest_port[i]]);
                bcm_vlan_port_add(unit, (bcm_vlan_t) (vlan), pbm, ubm);
                BCM_PBMP_CLEAR(pbm);
                BCM_PBMP_PORT_ADD(pbm, l2uc_p->port_list[l2uc_p->
                                                                dest_port[i]]);
                bcm_vlan_port_add(unit, (bcm_vlan_t) (xlate_vlan0), pbm, ubm);
                BCM_PBMP_CLEAR(pbm);
                BCM_PBMP_PORT_ADD(pbm, l2uc_p->port_list[i]);
                bcm_vlan_port_add(unit, (bcm_vlan_t) (xlate_vlan1), pbm, ubm);

                bcm_vlan_translate_add(unit, l2uc_p->port_list[i],
                                       (bcm_vlan_t)(vlan),
                                       (bcm_vlan_t)(xlate_vlan0), 0);
                bcm_vlan_translate_add(unit, l2uc_p->port_list[i],
                                       (bcm_vlan_t)(xlate_vlan1),
                                       (bcm_vlan_t)(xlate_vlan0), 0);
                bcm_vlan_translate_add(unit, l2uc_p->port_list[l2uc_p->
                                                                dest_port[i]],
                                       (bcm_vlan_t)(vlan),
                                       (bcm_vlan_t)(xlate_vlan1), 0);
                bcm_vlan_translate_add(unit, l2uc_p->port_list[l2uc_p->
                                                                dest_port[i]],
                                       (bcm_vlan_t)(xlate_vlan0),
                                       (bcm_vlan_t)(xlate_vlan1), 0);
                bcm_l2_addr_t_init(&l2_addr, mac_da, (bcm_vlan_t)(xlate_vlan0));
                l2_addr.port = l2uc_p->port_list[l2uc_p->dest_port[i]];
                l2_addr.flags = 0;
                bcm_l2_addr_add(unit, &l2_addr);

                bcm_l2_addr_t_init(&l2_addr, mac_da, (bcm_vlan_t)(xlate_vlan1));
                l2_addr.port = l2uc_p->port_list[i];
                l2_addr.flags = 0;
                bcm_l2_addr_add(unit, &l2_addr);

                port_accounted_for[i] = 1;
                port_accounted_for[l2uc_p->dest_port[i]] = 1;
            }

            xlate_vlan0++;
            xlate_vlan1++;
            vlan++;
        }
    }
}

/*
 * Function:
 *      l2uc_send_pkts
 * Purpose:
 *      Send packets to flood create swills on each port pair.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
static void
l2uc_send_pkts(int unit)
{
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    int i, port;
    int hg_en;
    uint32 flood_cnt, pkt_size, num_pkts_tx, pkt_count = 0;
    uint32 use_random_packet_sizes = 0;
    l2uc_t *l2uc_p = l2uc_parray[unit];
    stream_pkt_t *tx_pkt;

    tx_pkt = sal_alloc(sizeof(stream_pkt_t), "tx_pkt");
    sal_memset(tx_pkt, 0, sizeof(stream_pkt_t));

    if (l2uc_p->pkt_size_param == 1) {
        use_random_packet_sizes = 1;
    }

    cli_out("\n==================================================\n");
    cli_out("\nSending packets ...\n\n");
    for (i = 0; i < l2uc_p->num_streams; i++) {
        hg_en = l2uc_p->hg_stream[i] == 1 ? 1: 0;
        if (l2uc_p->pkt_size_param == 0) {
            pkt_size = stream_get_wc_pkt_size(unit, hg_en);
        } else {
            pkt_size = l2uc_p->pkt_size_param;
        }

        port = l2uc_p->dest_port[l2uc_p->stream[i]];
        flood_cnt = l2uc_lossless_flood_cnt(unit, port, l2uc_p->hg_stream[i]);
        if (l2uc_p->hg_stream[i] == 1) {
            num_pkts_tx = 2 * flood_cnt;
        } else {
            num_pkts_tx = flood_cnt;
        }

        tx_pkt->port     = port;
        tx_pkt->num_pkt  = num_pkts_tx;
        tx_pkt->pkt_seed = l2uc_p->pkt_seed + i;
        tx_pkt->pkt_size = pkt_size;
        tx_pkt->rand_pkt_size_en = use_random_packet_sizes;
        tx_pkt->rand_pkt_size = NULL;
        tx_pkt->tx_vlan = VLAN + i;
        sal_memcpy(tx_pkt->mac_da, mac_da, NUM_BYTES_MAC_ADDR);
        sal_memcpy(tx_pkt->mac_sa, mac_sa, NUM_BYTES_MAC_ADDR);
        if (tx_pkt->rand_pkt_size_en) {
            tx_pkt->rand_pkt_size = l2uc_p->rand_pkt_sizes[port];
        }
        stream_tx_pkt(unit, tx_pkt);

        /* print */
        cli_out("Stream %0d, pkt_flood_count = %0d\n", i, tx_pkt->cnt_pkt);
        pkt_count += tx_pkt->cnt_pkt;
    }

    cli_out("\nTotal packets sent: %d\n\n", pkt_count);
    sal_free(tx_pkt);
}

/*
 * Function:
 *      l2uc_test_init
 * Purpose:
 *      Test init function. Parse CLI params and do necessary init.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
l2uc_test_init(int unit, args_t *a, void **pa)
{
    l2uc_t *l2uc_p;

    l2uc_p = l2uc_parray[unit];
    l2uc_p = sal_alloc(sizeof(l2uc_t), "l2uc_test");
    sal_memset(l2uc_p, 0, sizeof(l2uc_t));
    l2uc_parray[unit] = l2uc_p;
    cli_out("\nCalling l2uc_test_init");
    l2uc_parse_test_params(unit, a);

    l2uc_p->test_fail = 0;
    if (l2uc_p->bad_input == 1) {
        goto done;
    }

    /* set mac loopback on all ports */
    stream_set_mac_lpbk(unit, PBMP_PORT_ALL(unit));
    /* turn off cmic to mmu backpressure */
    stream_turn_off_cmic_mmu_bkp(unit);
    /* turn off flow control */
    stream_turn_off_fc(unit, PBMP_PORT_ALL(unit));
    /* coverity[dont_call : FALSE] */
    l2uc_p->pkt_seed = sal_rand();

done:
    return 0;
}

/*
 * Function:
 *      l2uc_test
 * Purpose:
 *      Set up ports/streams and send packets.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
l2uc_test(int unit, args_t *a, void *pa)
{
    l2uc_t *l2uc_p = l2uc_parray[unit];

    if (l2uc_p->bad_input == 1) {
        goto done;
    }

    cli_out("\nCalling l2uc_test");
    l2uc_set_port_property(unit);
    l2uc_set_up_ports(unit);
    l2uc_set_up_streams(unit);
    l2uc_send_pkts(unit);

    /* check counter */
    if (stream_chk_mib_counters(unit, PBMP_PORT_ALL(unit), 0) != BCM_E_NONE) {
        l2uc_p->test_fail = 1;
    }
    /* check rate */
    if (l2uc_chk_port_rate(unit) != BCM_E_NONE) {
        l2uc_p->test_fail = 1;
    }
    /* check integrity */
    if (l2uc_p->check_packet_integrity_param != 0) {
        if (l2uc_chk_pkt_integrity(unit) != BCM_E_NONE) {
            l2uc_p->test_fail = 1;
        }
    }

done:
    return 0;
}

/*
 * Function:
 *      l2uc_test_cleanup
 * Purpose:
 *      Do test end checks and free all allocated memory.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
l2uc_test_cleanup(int unit, void *pa)
{
    l2uc_t *l2uc_p = l2uc_parray[unit];
    int i, rv;

    if (l2uc_p->bad_input == 1) {
        goto done;
    }
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nCalling l2uc_test_cleanup\n")));

    sal_free(l2uc_p->port_speed);
    sal_free(l2uc_p->port_list);
    sal_free(l2uc_p->dest_port);
    sal_free(l2uc_p->stream);
    sal_free(l2uc_p->hg_stream);
    sal_free(l2uc_p->modid);
    sal_free(l2uc_p->port_oversub);
    sal_free(l2uc_p->ovs_ratio_x1000);
    sal_free(l2uc_p->rate);
    sal_free(l2uc_p->exp_rate);
    sal_free(l2uc_p->tpkt_start);
    sal_free(l2uc_p->tpkt_end);
    sal_free(l2uc_p->tbyt_start);
    sal_free(l2uc_p->tbyt_end);

    for (i = 0; i < l2uc_p->num_fp_ports; i++) {
        sal_free(l2uc_p->rand_pkt_sizes[i]);
    }
    sal_free(l2uc_p->rand_pkt_sizes);

done:
    if (l2uc_p->bad_input == 1) {
        l2uc_p->test_fail = 1;
    }

    if (l2uc_p->test_fail == 1) {
        rv = BCM_E_FAIL;
    } else {
        rv = BCM_E_NONE;
    }

    cli_out("\n==================================================");
    cli_out("\n==================================================");
    if (l2uc_p->test_fail == 1) {
        cli_out("\n[L2UC test failed]\n\n");
    } else {
        cli_out("\n[L2UC test passed]\n\n");
    }

    sal_free(l2uc_p);

    return rv;
}
#endif /* BCM_ESW_SUPPORT */
