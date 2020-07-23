/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * The purpose of this test is to measure the latency of the chip according to RFC2544. Since this
 * test uses 1588 timestamps to measure the latency and timestamping is added at the TSC, this
 * test requires one external cable to loop back the packets between a port pair being measured.
 * Thus, the speeds of the port pair have to be the same as the port speed being measured.
 * In order to cover all available port speeds, the cable may need to be moved to port pairs of
 * different speed and the chip may need to be rebooted into a different TDM.
 *
 * First, this test creates an infinite loop in an swirling port. After the CPU has deposited enough
 * packets into the swirl to achieve line rate, the packets will be redirected to the port pair via egress
 * VLAN translate and L2 switching. Then, the packets get ingress or egress time stamped twiced at
 * the externally connected port pair. Finally, the packets are redirected back to the CPU via
 * egress VLAN translate and L2 switching. The CPU checks the packet integrity and computes the
 * latencies by substracting the second timestamp with the first timestamp of each packet
 *
 * Configuration parameters passed from CLI:
 * IngEgrTs: Set to 0 for ingress timestamping (default), 1 to egress timestamping.
 * PktSize: Packet size in bytes.
 * FloodCnt: Number of packets in each swill.
 * SwirlInt: Interval in seconds over which rate is to be calculated
 * ChkPktInteg: Set to 0 to disable packet integrity checks, 1 to enable (default).
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
#include <bcm/time.h>

#include "testlist.h"
#include "gen_pkt.h"
#include "streaming_lib.h"

#define MAC_DA {0x12,0x34,0x56,0x78,0x9a,0xbc}
#define MAC_SA {0xfe,0xdc,0xba,0x98,0x76,0x54}
#define PKT_VLAN 0xa

#define XLATE_VLAN0 0xb
#define XLATE_VLAN1 0xc
#define CPU_VLAN 0xd

#define MAX_FP_PORTS 130

#define NULL_DEST 999

#define EGR_TS_PARAM_DEFAULT 0
#define PKT_SIZE_PARAM_DEFAULT 64
#define FLOOD_PKT_CNT_PARAM_DEFAULT 100
#define SWIRL_INTERVAL_PARAM_DEFAULT 10
#define SWIRL_PORT_PARAM_DEFAULT 1
#define TIMESTAMP_PORT1_PARAM_DEFAULT 5
#define TIMESTAMP_PORT2_PARAM_DEFAULT 9
#define CHECK_PACKET_INTEGRITY_PARAM_DEFAULT 1

#if defined(BCM_ESW_SUPPORT) && \
  (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))

typedef struct latency_s {
    uint32 num_fp_ports;
    uint32 num_streams;
    int *port_speed;
    int *port_list;
    int *dest_port;
    uint32 *stream;
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
    uint32 egr_ts_param;
    uint32 pkt_size_param;
    uint32 flood_pkt_cnt_param;
    uint32 swirl_interval_param;
    uint32 swirl_port_param;
    uint32 timestamp_port1_param;
    uint32 timestamp_port2_param;
    uint32 check_packet_integrity_param;
    uint32 bad_input;
    int test_fail;
    uint32 pkt_seed;
} latency_t;

static latency_t *latency_parray[SOC_MAX_NUM_DEVICES];


/*
 * Function:
 *      latency_chk_port_rate
 * Purpose:
 *      Check actual port rates against expected port rates.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     SOC_E_XXXX
 */
static bcm_error_t
latency_chk_port_rate(int unit)
{
    int port;
    bcm_error_t rv = BCM_E_NONE;
    stream_rate_t *rate_calc;
    latency_t *latency_p = latency_parray[unit];

    rate_calc = (stream_rate_t *)
                 sal_alloc(sizeof(stream_rate_t), "rate_calc");
    if (rate_calc == NULL) {
        test_error(unit, "Failed to allocate memory for rate_calc\n");
        return BCM_E_FAIL;
    }
    sal_memset(rate_calc, 0, sizeof(stream_rate_t));

    rate_calc->mode         = 1; /* check act_rate by tx_rate */
    rate_calc->pkt_size     = latency_p->pkt_size_param;
    rate_calc->interval_len = latency_p->swirl_interval_param;

    SOC_PBMP_CLEAR(rate_calc->pbmp);
    port = latency_p->swirl_port_param;
    SOC_PBMP_PORT_ADD(rate_calc->pbmp, port);
    /* src_port */
    rate_calc->src_port[port] = port;

    rv = stream_chk_port_rate(unit, PBMP_PORT_ALL(unit), rate_calc);

    sal_free(rate_calc);
    return rv;
}

/*
 * Function:
 *      latency_chk_pkt_integrity
 * Purpose:
 *      Redirect all packets back to CPU and check packet integrity
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     SOC_E_XXXX
 */
static bcm_error_t
latency_chk_pkt_integrity(int unit)
{
    int port;
    int param_pkt_seed;
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    stream_integrity_t *pkt_intg;
    bcm_error_t rv = BCM_E_NONE;
    latency_t *latency_p = latency_parray[unit];

    param_pkt_seed = latency_p->pkt_seed;

    pkt_intg = sal_alloc(sizeof(stream_integrity_t), "pkt_intg");
    if (pkt_intg == NULL) {
        test_error(unit, "Failed to allocate memory for pkt_intg\n");
        return BCM_E_FAIL;
    }
    sal_memset(pkt_intg, 0, sizeof(stream_integrity_t));

    pkt_intg->type = PKT_TYPE_L2_TS;
    SOC_PBMP_CLEAR(pkt_intg->rx_pbmp);
    port = latency_p->timestamp_port1_param;
     /* rx_pbmp */
     SOC_PBMP_PORT_ADD(pkt_intg->rx_pbmp, port);
     /* rx_vlan */
     pkt_intg->rx_vlan[port] = CPU_VLAN;
     /* tx_vlan */
     pkt_intg->tx_vlan[port] = CPU_VLAN;
     /* port_flood_cnt */
     pkt_intg->port_flood_cnt[port] = latency_p->flood_pkt_cnt_param;
     /* port_pkt_seed */
     pkt_intg->port_pkt_seed[port] = param_pkt_seed;
     /* mac_da, mac_sa */
     sal_memcpy(pkt_intg->mac_da[port], mac_da, NUM_BYTES_MAC_ADDR);
     sal_memcpy(pkt_intg->mac_sa[port], mac_sa, NUM_BYTES_MAC_ADDR);

    if (stream_chk_pkt_integrity(unit, pkt_intg) != BCM_E_NONE) {
        rv = BCM_E_FAIL;
    }

    sal_free(pkt_intg);
    return rv;
}

/*
 * Function:
 *      latency_parse_test_params
 * Purpose:
 *      Parse CLI parameters, create test structure and flag bad inputs.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     Nothing
 * Notes:
 *      latency_p->bad_input set from here - tells test to crash out in case
 *      CLI input combination is invalid.
 */
static void
latency_parse_test_params(int unit, args_t *a)
{
    parse_table_t parse_table;
    latency_t *latency_p = latency_parray[unit];

    const char tr510_test_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nDocumentation too long to be displayed with -pedantic compiler\n";
#else
    "\nLatency test to check basic L2 packet switching. The test pairs up\n"
    "same speed ports and creates L2UC traffic swills. Test also\n"
    "calculates expected rates based on port config and oversub ratio, measures\n"
    "actual rate over a period of time and conducts rate checks. Finally all\n"
    "packets in each swill will be redirected to the CPU and checked for integrity.\n"
    "\n"
    "Configuration parameters passed from CLI:\n"
    "IngEgrTs: Set to 0 for ingress timestamping (default), 1 to egress timestamping.\n"
    "PktSize: Packet size in bytes.\n"
    "FloodCnt: Number of packets in each swill.\n"
    "SwirlInt: Interval in seconds over which rate is to be calculated\n"
    "ChkPktInteg: Set to 0 to disable packet integrity checks, 1 to enable (default).\n";
#endif

    latency_p->bad_input = 0;

    latency_p->egr_ts_param = EGR_TS_PARAM_DEFAULT;
    latency_p->pkt_size_param = PKT_SIZE_PARAM_DEFAULT;
    latency_p->flood_pkt_cnt_param = FLOOD_PKT_CNT_PARAM_DEFAULT;
    latency_p->swirl_interval_param = SWIRL_INTERVAL_PARAM_DEFAULT;
    latency_p->swirl_port_param = SWIRL_PORT_PARAM_DEFAULT;
    latency_p->timestamp_port1_param = TIMESTAMP_PORT1_PARAM_DEFAULT;
    latency_p->timestamp_port2_param = TIMESTAMP_PORT2_PARAM_DEFAULT;
    latency_p->check_packet_integrity_param
                            = CHECK_PACKET_INTEGRITY_PARAM_DEFAULT;

    /*Parse CLI opts */

    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table, "EgrTs", PQ_INT|PQ_DFL, 0,
                    &(latency_p->egr_ts_param), NULL);
    parse_table_add(&parse_table, "PktSize", PQ_INT|PQ_DFL, 0,
                    &(latency_p->pkt_size_param), NULL);
    parse_table_add(&parse_table, "FloodCnt", PQ_INT|PQ_DFL, 0,
                    &(latency_p->flood_pkt_cnt_param), NULL);
    parse_table_add(&parse_table, "SwirlInt", PQ_INT|PQ_DFL, 0,
                    &(latency_p->swirl_interval_param), NULL);
    parse_table_add(&parse_table, "SwirlPort", PQ_INT|PQ_DFL, 0,
                    &(latency_p->swirl_port_param), NULL);
    parse_table_add(&parse_table, "TimeStampPort1", PQ_INT|PQ_DFL, 0,
                    &(latency_p->timestamp_port1_param), NULL);
    parse_table_add(&parse_table, "TimeStampPort2", PQ_INT|PQ_DFL, 0,
                    &(latency_p->timestamp_port2_param), NULL);
    parse_table_add(&parse_table, "ChkPktInteg", PQ_INT|PQ_DFL, 0,
                    &(latency_p->check_packet_integrity_param), NULL);

    if (parse_arg_eq(a, &parse_table) < 0 || ARG_CNT(a) != 0) {
        test_msg("%s", tr510_test_usage);
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        latency_p->bad_input = 1;
        parse_arg_eq_done(&parse_table);
    } else {
        cli_out("\n");
        cli_out("------------- PRINTING TEST PARAMS ------------------\n");
        cli_out("EgrTS          = %0d\n", latency_p->egr_ts_param);
        cli_out("PktSize        = %0d\n", latency_p->pkt_size_param);
        cli_out("SwirlInt       = %0d\n", latency_p->swirl_interval_param);
        cli_out("SwirlPort      = %0d\n", latency_p->swirl_port_param);
        cli_out("TimeStampPort1 = %0d\n", latency_p->timestamp_port1_param);
        cli_out("TimeStampPort2 = %0d\n", latency_p->timestamp_port2_param);
        cli_out("FloodCnt       = %0d\n", latency_p->flood_pkt_cnt_param);
        cli_out("ChkPktInteg    = %0d\n", latency_p->check_packet_integrity_param);
        cli_out("-----------------------------------------------------\n");
    }

    if (latency_p->pkt_size_param < MIN_PKT_SIZE) {
        test_error(unit,"\n*ERROR: Packet size cannot be lower than %0dB\n",
                MIN_PKT_SIZE);
        latency_p->bad_input = 1;
    } else if (latency_p->pkt_size_param > MTU) {
        test_error(unit,"\n*ERROR: Packet size cannot be higher than %0dB (Device MTU)\n",
                MTU);
        latency_p->bad_input = 1;
    }
}

/*
 * Function:
 *      latency_enable_ingress_egress_timestamp
 * Purpose:
 *      Enable ingress timestamp generation for the timstamp ports
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     SOC_E_XXXX
 */
bcm_error_t
latency_enable_ingress_egress_timestamp(int unit)
{
    uint32 rval;
    latency_t *latency_p = latency_parray[unit];

    /* Enable Timestamp Generation */
    start_cmic_timesync(unit);
    READ_CMIC_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, &rval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_COUNTER_CONFIG_SELECTr, &rval,
                                ENABLE_COMMON_CONTROLf, 1);
    WRITE_CMIC_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, rval);

    /* Enable Ingress Timestamp for timestamp port 1 and 2 */
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, latency_p->timestamp_port1_param,
                                bcmPortControlPacketTimeStampInsertRx, !latency_p->egr_ts_param));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, latency_p->timestamp_port1_param,
                                bcmPortControlPacketTimeStampInsertTx, latency_p->egr_ts_param));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, latency_p->timestamp_port1_param,
                                bcmPortControlPacketTimeStampRxId, latency_p->timestamp_port1_param));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, latency_p->timestamp_port2_param,
                                bcmPortControlPacketTimeStampInsertRx, !latency_p->egr_ts_param));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, latency_p->timestamp_port2_param,
                                bcmPortControlPacketTimeStampInsertTx, latency_p->egr_ts_param));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, latency_p->timestamp_port2_param,
                                bcmPortControlPacketTimeStampRxId, latency_p->timestamp_port2_param));
    return BCM_E_NONE;
}

/*
 * Function:
 *      latency_set_port_property
 * Purpose:
 *      Set port_list, port_speed and port_oversub arrays. Also call set_exp_rates
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Safe packet size.
 */
static void
latency_set_port_property(int unit)
{
    int p, i, j, paired_port;
    uint32 can_be_paired = 0;
    soc_info_t *si = &SOC_INFO(unit);
    latency_t *latency_p = latency_parray[unit];

    latency_p->num_fp_ports = 0;
    latency_p->num_streams = 0;

    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        latency_p->num_fp_ports++;
    }

    latency_p->port_list =
        (int *)sal_alloc(latency_p->num_fp_ports * sizeof(int),
                         "port_list");
    latency_p->port_speed =
        (int *)sal_alloc(latency_p->num_fp_ports * sizeof(int),
                         "port_speed_array");
    latency_p->dest_port =
        (int *)sal_alloc(latency_p->num_fp_ports * sizeof(int),
                          "dest_port_array");
    latency_p->stream =
        (uint32 *)sal_alloc(latency_p->num_fp_ports * sizeof(uint32),
                          "stream");
    latency_p->port_oversub =
        (uint32 *) sal_alloc(latency_p->num_fp_ports * sizeof(uint32),
                           "port_oversub_array");

    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        latency_p->port_list[i] = p;
        i++;
    }

    for (i = 0; i < latency_p->num_fp_ports; i++) {
        latency_p->port_speed[i] =
            si->port_speed_max[latency_p->port_list[i]];

        switch(latency_p->port_speed[i]) {
            case 11000: latency_p->port_speed[i] = 10600; break;
            case 27000: latency_p->port_speed[i] = 26500; break;
            case 42000: latency_p->port_speed[i] = 42400; break;
            default: break;
        }
    }

    for (i = 0; i < latency_p->num_fp_ports; i++) {
        if (SOC_PBMP_MEMBER(si->oversub_pbm, latency_p->port_list[i]) &&
            (!(SOC_PBMP_MEMBER(si->management_pbm, latency_p->port_list[i])))) {
            latency_p->port_oversub[i] = 1;
        } else {
            latency_p->port_oversub[i] = 0;
        }
    }

    for (i = 0; i < latency_p->num_fp_ports; i++) {
        latency_p->dest_port[i] = NULL_DEST;
    }

    /* set src and dst ports for each stream */
    for (i = 0; i < latency_p->num_fp_ports; i++) {
        can_be_paired = 0;
        if(latency_p->dest_port[i] == NULL_DEST) {
            for (j = 0;j < latency_p->num_fp_ports; j++) {
                if((latency_p->dest_port[j] == NULL_DEST)
                        && (latency_p->port_speed[i]
                                    == latency_p->port_speed[j])
                        && (i!=j)) {
                    can_be_paired = 1;
                }
            }

            if(can_be_paired) {
                do {
                    paired_port = sal_rand() % latency_p->num_fp_ports;
                }while((latency_p->port_speed[i] !=
                                latency_p->port_speed[paired_port])
                        || (latency_p->dest_port[paired_port] != NULL_DEST)
                                                        || (i == paired_port));

                latency_p->dest_port[i] = paired_port;
                latency_p->dest_port[paired_port] = i;
                latency_p->stream[latency_p->num_streams] = i;
                latency_p->num_streams++;
            }
        }
    }

    {
        /* print */
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\n==== STREAM INFO (VERBOSE) =====\n")));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\n%8s%8s%8s%8s%8s%8s\n"),
                             "idx", "port", "dst", "speed", "ovsb", "HiGig"));
        for (i = 0; i < latency_p->num_fp_ports; i++) {
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "%8d%8d%8d%7dG"),
                                 i,
                                 latency_p->port_list[i],
                                 latency_p->port_list[latency_p->dest_port[i]],
                                 latency_p->port_speed[i]/1000));
            if (SOC_PBMP_MEMBER(si->oversub_pbm, latency_p->port_list[i])) {
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "%8s"), "OS"));
            } else {
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "%8s"), "--"));
            }
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "%8s"), "--"));
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\n")));
        }
    }
}

/*
 * Function:
 *      latency_set_up_streams
 * Purpose:
 *      Program switch to set up L2UC streams between port pairs. For Ethernet
 *      ports this involves setting up VLAN translations and L2 entries to get
 *      a packet flow of CPU -> P0 (Vlan0, Xlate_Vlan0) -> P1(Xlate_vlan1) -> P0 ...
 *
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 *
 * Returns:
 *     SOC_E_XXXX
 */
bcm_error_t
latency_set_up_streams(int unit)
{
    pbmp_t pbm, ubm;
    latency_t *latency_p = latency_parray[unit];
    bcm_l2_addr_t l2_addr;
    bcm_mac_t mac_da = MAC_DA;
    port_tab_entry_t port_tab_entry;

    if (!SOC_MEM_IS_VALID(unit, PORT_TABm)) {
        uint32 entry[SOC_MAX_MEM_WORDS];
        soc_pbmp_t pbmp;

        SOC_PBMP_PORT_SET(pbmp, latency_p->swirl_port_param);
        sal_memset(entry, 0, sizeof(port_bridge_bmap_entry_t));
        soc_mem_pbmp_field_set(unit, PORT_BRIDGE_BMAPm, &entry, BITMAPf, &pbmp);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, PORT_BRIDGE_BMAPm, COPYNO_ALL, 0, entry));
    } else {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm, COPYNO_ALL, latency_p->swirl_port_param,
                            port_tab_entry.entry_data));
        soc_mem_field32_set(unit, PORT_TABm, port_tab_entry.entry_data, PORT_BRIDGEf, 1);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, PORT_TABm, COPYNO_ALL, latency_p->swirl_port_param,
                            port_tab_entry.entry_data));
    }

    BCM_PBMP_CLEAR(ubm);
    bcm_vlan_destroy_all(unit);
    bcm_vlan_control_set(unit, bcmVlanTranslate, TRUE);

    bcm_vlan_create(unit, (bcm_vlan_t) (PKT_VLAN));
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_PORT_ADD(pbm, latency_p->swirl_port_param);
    bcm_vlan_port_add(unit, (bcm_vlan_t) (PKT_VLAN), pbm, ubm);
    /* set mac loopback on the swriling port */
    stream_set_mac_lpbk(unit, pbm);

    bcm_vlan_create(unit, (bcm_vlan_t) (XLATE_VLAN0));
    bcm_vlan_create(unit, (bcm_vlan_t) (XLATE_VLAN1));
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_PORT_ADD(pbm, latency_p->timestamp_port1_param);
    BCM_PBMP_PORT_ADD(pbm, latency_p->timestamp_port2_param);
    bcm_vlan_port_add(unit, (bcm_vlan_t) (XLATE_VLAN0), pbm, ubm);
    bcm_vlan_port_add(unit, (bcm_vlan_t) (XLATE_VLAN1), pbm, ubm);

    bcm_vlan_create(unit, (bcm_vlan_t) (CPU_VLAN));
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_PORT_ADD(pbm, 0);
    bcm_vlan_port_add(unit, (bcm_vlan_t) (CPU_VLAN), pbm, ubm);

    bcm_vlan_translate_egress_add(unit, latency_p->timestamp_port1_param,
                            (bcm_vlan_t)(XLATE_VLAN0),
                            (bcm_vlan_t)(XLATE_VLAN1), 0);
    bcm_vlan_translate_egress_add(unit, latency_p->timestamp_port1_param,
                            (bcm_vlan_t)(XLATE_VLAN1),
                            (bcm_vlan_t)(CPU_VLAN), 0);

    bcm_l2_addr_t_init(&l2_addr, mac_da, (bcm_vlan_t)(XLATE_VLAN0));
    l2_addr.port = latency_p->timestamp_port1_param;
    l2_addr.flags = 0;
    bcm_l2_addr_add(unit, &l2_addr);

    bcm_l2_addr_t_init(&l2_addr, mac_da, (bcm_vlan_t)(XLATE_VLAN1));
    l2_addr.port = latency_p->timestamp_port1_param;
    l2_addr.flags = 0;
    bcm_l2_addr_add(unit, &l2_addr);

    bcm_l2_addr_t_init(&l2_addr, mac_da, (bcm_vlan_t)(CPU_VLAN));
    l2_addr.port = 0;
    l2_addr.flags = 0;
    bcm_l2_addr_add(unit, &l2_addr);
    return BCM_E_NONE;
}

/*
 * Function:
 *      latency_send_pkts
 * Purpose:
 *      Send packets to flood the swirl port.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
static void
latency_send_pkts(int unit)
{
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    latency_t *latency_p = latency_parray[unit];
    stream_pkt_t *tx_pkt;

    tx_pkt = sal_alloc(sizeof(stream_pkt_t), "tx_pkt");
    sal_memset(tx_pkt, 0, sizeof(stream_pkt_t));

    cli_out("\nSending %0d packets to swirl port %0d\n",
              latency_p->flood_pkt_cnt_param, latency_p->swirl_port_param);

    tx_pkt->port = latency_p->swirl_port_param;
    tx_pkt->num_pkt  = latency_p->flood_pkt_cnt_param;
    tx_pkt->pkt_seed = latency_p->pkt_seed;
    tx_pkt->pkt_size = latency_p->pkt_size_param;
    tx_pkt->tx_vlan = PKT_VLAN;
    sal_memcpy(tx_pkt->mac_da, mac_da, NUM_BYTES_MAC_ADDR);
    sal_memcpy(tx_pkt->mac_sa, mac_sa, NUM_BYTES_MAC_ADDR);
    stream_tx_pkt(unit, tx_pkt);

    sal_free(tx_pkt);
}

/*
 * Function:
 *      latency_test_init
 * Purpose:
 *      Test init function. Parse CLI params and do necessary init.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
latency_test_init(int unit, args_t *a, void **pa)
{
    latency_t *latency_p;

    latency_p = latency_parray[unit];
    latency_p = sal_alloc(sizeof(latency_t), "latency_test");
    sal_memset(latency_p, 0, sizeof(latency_t));
    latency_parray[unit] = latency_p;
    cli_out("\nCalling latency_test_init");
    latency_parse_test_params(unit, a);

    latency_p->test_fail = 0;
    if (latency_p->bad_input == 1) {
        goto done;
    }

    /* coverity[dont_call : FALSE] */
    latency_p->pkt_seed = sal_rand();

done:
    return 0;
}

/*
 * Function:
 *      latency_test
 * Purpose:
 *      Set up ports/streams and send packets.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
latency_test(int unit, args_t *a, void *pa)
{
    latency_t *latency_p = latency_parray[unit];

    if (latency_p->bad_input == 1) {
        goto done;
    }

    cli_out("\nCalling latency_test");
    /* turn off cmic to mmu backpressure */
    stream_turn_off_cmic_mmu_bkp(unit);
    /* turn off flow control */
    stream_turn_off_fc(unit, PBMP_PORT_ALL(unit));
    latency_set_port_property(unit);
    if (latency_enable_ingress_egress_timestamp(unit) != BCM_E_NONE) {
        latency_p->test_fail = 1;
    }
    if (latency_set_up_streams(unit) != BCM_E_NONE) {
        latency_p->test_fail = 1;
    }
    latency_send_pkts(unit);

    /* check counter */
    if (stream_chk_mib_counters(unit, PBMP_PORT_ALL(unit), 0) != BCM_E_NONE) {
        latency_p->test_fail = 1;
    }
    /* check rate */
    if (latency_chk_port_rate(unit) != BCM_E_NONE) {
        latency_p->test_fail = 1;
    }
    bcm_vlan_translate_egress_add(unit, latency_p->swirl_port_param,
                            (bcm_vlan_t)(PKT_VLAN),
                            (bcm_vlan_t)(XLATE_VLAN0), 0);
    /* check integrity */
    if (latency_p->check_packet_integrity_param != 0) {
        if (latency_chk_pkt_integrity(unit) != BCM_E_NONE) {
            latency_p->test_fail = 1;
        }
    }

done:
    return 0;
}

/*
 * Function:
 *      latency_test_cleanup
 * Purpose:
 *      Do test end checks and free all allocated memory.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
latency_test_cleanup(int unit, void *pa)
{
    latency_t *latency_p = latency_parray[unit];
    int rv;

    if (latency_p->bad_input == 1) {
        goto done;
    }
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nCalling latency_test_cleanup\n")));

    sal_free(latency_p->port_speed);
    sal_free(latency_p->port_list);
    sal_free(latency_p->dest_port);
    sal_free(latency_p->stream);
    sal_free(latency_p->port_oversub);
    sal_free(latency_p->ovs_ratio_x1000);
    sal_free(latency_p->rate);
    sal_free(latency_p->exp_rate);
    sal_free(latency_p->tpkt_start);
    sal_free(latency_p->tpkt_end);
    sal_free(latency_p->tbyt_start);
    sal_free(latency_p->tbyt_end);

done:
    if (latency_p->bad_input == 1) {
        latency_p->test_fail = 1;
    }

    if (latency_p->test_fail == 1) {
        rv = BCM_E_FAIL;
    } else {
        rv = BCM_E_NONE;
    }

    cli_out("\n==================================================");
    cli_out("\n==================================================");
    if (latency_p->test_fail == 1) {
        cli_out("\n[Latency test failed]\n\n");
    } else {
        cli_out("\n[Latency test passed]\n\n");
    }

    sal_free(latency_p);

    return rv;
}
#endif /* BCM_ESW_SUPPORT */
