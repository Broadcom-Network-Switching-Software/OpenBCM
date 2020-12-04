/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * The l3uc test checks TDM and L3 unicast functionality and performance by
 * streaming IPv4/IPv6 packets on all ports at maximum rate. All ports are
 * configured in MAC loopback mode and each port is paired with a same speed
 * port. The test calculates the number of packets needed to saturate the ports
 * and send the IPv4/IPv6 packets from the CPU to each port pairs initially.
 * Then the packets are L3 switched between the port pairs indefinitely. The
 * MAC DA, MAC SA, and VLAN are changed every time a packet is L3 switched. Once
 * the traffic reaches steady state, rate calculation is done by dividing the
 * transmit packet count changes and transmit byte count changes over a
 * programmable interval. The rates are checked against expected rates based on
 * port configuration and oversubscription ratio. Finally, packet integrity
 * check is achieved by redirecting the packets back to the CPU and compared
 * against expected packets.
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
 * ChkPktInteg: Set to 0 to disable packet integrity checks, 1 to enable
                (default).
 * MaxNumCells: Max number of cells for random packet sizes. Default = 4. Set
 *              to 0 for random.
 * IPv6: Set to 1 to send IPv6 packets, 0 to send IPv4 packets (default).
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

#define MAC_DA {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc}
#define MAC_SA {0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54}
#define VLAN 0x0a00
#define TTL 255
#define IPV4_ADDR 0xc0a80001
#define IPV6_ADDR { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                    0x00, 0x00, 0x00, 0x00, 0xc0, 0xa8, 0x00, 0x01 };

#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_L3) && \
         (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))

#define PKT_SIZE_PARAM_DEFAULT 0
#define FLOOD_PKT_CNT_PARAM_DEFAULT 0
#define RATE_CALC_INTERVAL_PARAM_DEFAULT 10
#define RATE_TOLERANCE_LR_PARAM_DEFAULT 1
#define RATE_TOLERANCE_OV_PARAM_DEFAULT 3
#define CHECK_PACKET_INTEGRITY_PARAM_DEFAULT 1
#define MAX_NUM_CELLS_PARAM_DEFAULT 4
#define IPV6_PARAM_DEFAULT 0
#define SPEEDUP_FACTOR_PARAM_DEFAULT 1

typedef struct l3uc_s {
    uint32 pkt_size_param;
    uint32 flood_pkt_cnt_param;
    uint32 rate_calc_interval_param;
    uint32 rate_tolerance_lr_param;
    uint32 rate_tolerance_ov_param;
    uint32 check_packet_integrity_param;
    uint32 max_num_cells_param;
    uint32 loopback_mode_param;
    uint32 scaling_factor_param;
    uint32 ipv6_param;
    uint32 dst_port[SOC_MAX_NUM_PORTS];
    uint32 bad_input;
    int test_fail;
    uint32 pkt_seed;
    uint32 **rand_pkt_sizes;
} l3uc_t;

static l3uc_t *l3uc_parray[SOC_MAX_NUM_DEVICES];
/* static rate_calc_t *rate_calc_parray[SOC_MAX_NUM_DEVICES]; */

const char l3uc_test_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nDocumentation too long to be displayed with -pedantic compiler\n";
#else
    "L3UC test usage:\n"
    " \n"
    "PktSize:     Packet size in bytes. Set to 1 for random packet sizes.\n"
    "             Set to 0 (default) for worst case packet sizes on all ports\n"
    "             (145B for ENET, 76B for HG2).\n"
    "FloodCnt:    Number of packets swirl between each port pair.\n"
    "             Set to 0 (default) for a lossless swirl at full rate.\n"
    "RateCalcInt: Interval in seconds over which rate is to be calculated.\n"
    "TolLr:       Rate tolerance percentage for line rate ports.\n"
    "             (1 percentage by default)\n"
    "TolOv:       Rate tolerance percentage for oversubscribed ports.\n"
    "             (3 percentage by default).\n"
    "ChkPktInteg: Set to 0 to disable packet integrity checks.\n"
    "             Set to 1 to enable (default).\n"
    "MaxNumCells: Maximum number of cells for random packet sizes.\n"
    "             Set to 0 for random cell sizes. (default is 4)\n"
    "LoopbackMode: Loopback mode. Set to 1 for MAC loopback, 2 for PHY loopback.\n"
    "              (default is MAC loopback)\n"
    "IPv6:        Set to 1 to send IPv6 packets; 0 to send IPv4 (default).\n"
    ;
#endif


/*
 * Function:
 *      l3uc_parse_test_params
 * Purpose:
 *      Parse CLI parameters, create test structure and flag bad inputs.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     Nothing
 * Notes:
 *      l3uc_p->bad_input set from here - tells test to crash out in case
 *      CLI input combination is invalid.
 */
static void
l3uc_parse_test_params(int unit, args_t *a)
{
    parse_table_t parse_table;
    l3uc_t *l3uc_p = l3uc_parray[unit];
    uint32 min_pkt_size = l3uc_p->ipv6_param ? MIN_IPV6_PKT_SIZE : MIN_PKT_SIZE;

    /* default parameter value */
    l3uc_p->bad_input = 0;
    l3uc_p->pkt_size_param = PKT_SIZE_PARAM_DEFAULT;
    l3uc_p->flood_pkt_cnt_param = FLOOD_PKT_CNT_PARAM_DEFAULT;
    l3uc_p->rate_calc_interval_param = RATE_CALC_INTERVAL_PARAM_DEFAULT;
    l3uc_p->rate_tolerance_lr_param = RATE_TOLERANCE_LR_PARAM_DEFAULT;
    l3uc_p->rate_tolerance_ov_param = RATE_TOLERANCE_OV_PARAM_DEFAULT;
    l3uc_p->check_packet_integrity_param
                            = CHECK_PACKET_INTEGRITY_PARAM_DEFAULT;
    l3uc_p->max_num_cells_param = MAX_NUM_CELLS_PARAM_DEFAULT;
    l3uc_p->loopback_mode_param = BCM_PORT_LOOPBACK_MAC;
    l3uc_p->ipv6_param = IPV6_PARAM_DEFAULT;
    l3uc_p->scaling_factor_param = SPEEDUP_FACTOR_PARAM_DEFAULT;

    /*Parse CLI opts */
    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table, "PktSize", PQ_INT|PQ_DFL, 0,
                    &(l3uc_p->pkt_size_param), NULL);
    parse_table_add(&parse_table, "FloodCnt", PQ_INT|PQ_DFL, 0,
                    &(l3uc_p->flood_pkt_cnt_param), NULL);
    parse_table_add(&parse_table, "RateCalcInt", PQ_INT|PQ_DFL, 0,
                    &(l3uc_p->rate_calc_interval_param), NULL);
    parse_table_add(&parse_table, "TolLr", PQ_INT|PQ_DFL, 0,
                    &(l3uc_p->rate_tolerance_lr_param), NULL);
    parse_table_add(&parse_table, "TolOv", PQ_INT|PQ_DFL, 0,
                    &(l3uc_p->rate_tolerance_ov_param), NULL);
    parse_table_add(&parse_table, "ChkPktInteg", PQ_INT|PQ_DFL, 0,
                    &(l3uc_p->check_packet_integrity_param), NULL);
    parse_table_add(&parse_table, "MaxNumCells", PQ_INT|PQ_DFL, 0,
                    &(l3uc_p->max_num_cells_param), NULL);
    parse_table_add(&parse_table, "LoopbackMode", PQ_INT|PQ_DFL, 0,
                    &l3uc_p->loopback_mode_param, NULL);
    parse_table_add(&parse_table, "IPv6", PQ_INT|PQ_DFL, 0,
                    &(l3uc_p->ipv6_param), NULL);
    parse_table_add(&parse_table, "ScalingFactor", PQ_INT|PQ_DFL, 0,
                    &(l3uc_p->scaling_factor_param), NULL);

    if (parse_arg_eq(a, &parse_table) < 0 || ARG_CNT(a) != 0) {
        cli_out("%s", l3uc_test_usage);
        test_error(unit, "\n*ERROR PARSING ARGS\n");
    }

    cli_out("\n");
    cli_out("------------- PRINTING TEST PARAMS ------------------\n");
    cli_out("PktSize      = %0d\n", l3uc_p->pkt_size_param);
    cli_out("RateCalcInt  = %0d\n", l3uc_p->rate_calc_interval_param);
    cli_out("FloodCnt     = %0d\n", l3uc_p->flood_pkt_cnt_param);
    cli_out("TolLr        = %0d\n", l3uc_p->rate_tolerance_lr_param);
    cli_out("TolOv        = %0d\n", l3uc_p->rate_tolerance_ov_param);
    cli_out("ChkPktInteg  = %0d\n", l3uc_p->check_packet_integrity_param);
    cli_out("MaxNumCells  = %0d\n", l3uc_p->max_num_cells_param);
    cli_out("LoopbackMode = %0d\n", l3uc_p->loopback_mode_param);
    cli_out("IPv6         = %0d\n", l3uc_p->ipv6_param);
    cli_out("ScalingFactor= %0d\n", l3uc_p->scaling_factor_param);
    cli_out("-----------------------------------------------------\n");

    if (l3uc_p->max_num_cells_param == 0) {
        /* coverity[dont_call : FALSE] */
        l3uc_p->max_num_cells_param = (sal_rand() % (MTU_CELL_CNT - 1)) + 1;
    }

    if (l3uc_p->pkt_size_param == 0) {
        cli_out
            ("\nUsing worst case packet sizes - 145B for Ethernet "
             "and 76B (64B + 12B hg-hdr) for HG2");
    } else if (l3uc_p->pkt_size_param == 1) {
        cli_out("\nUsing random packet sizes");
    } else if (l3uc_p->pkt_size_param < min_pkt_size) {
        test_error(unit,"\n*ERROR: Packet size cannot be lower than %0dB\n",
                min_pkt_size);
        l3uc_p->bad_input = 1;
    } else if (l3uc_p->pkt_size_param > MTU) {
        test_error(unit,"\n*ERROR: Packet size higher than %0dB (Device MTU)\n",
                MTU);
        l3uc_p->bad_input = 1;
    }

    if (l3uc_p->flood_pkt_cnt_param == 0) {
        cli_out("\nFloodCnt=0, test will automatically calculate number of"
                " packets to flood each port");
    }
}

/*
 * Function:
 *      l3uc_set_port_property
 * Purpose:
 *      Set rand_pkt_sizes arrays.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pbmp - Device port bitmap
 *      pkt_size_param - Packet size in bytes
 *      max_num_cells_param - Maximum number of cells
 *
 * Returns:
 *     Nothing
 */
static void
l3uc_set_port_property(int unit, pbmp_t pbmp, int pkt_size_param,
                         int max_num_cells_param)
{
    int p, j;
    uint32 pkt_size;
    l3uc_t *l3uc_p = l3uc_parray[unit];

    l3uc_p->rand_pkt_sizes =
        (uint32 **) sal_alloc(SOC_MAX_NUM_PORTS * sizeof(uint32 *),
                              "rand_pkt_sizes_array*");

    for (p = 0; p < SOC_MAX_NUM_PORTS; p++) {
        l3uc_p->rand_pkt_sizes[p] =
            (uint32 *) sal_alloc(TARGET_CELL_COUNT * sizeof(uint32),
                                "rand_pkt_sizes_array");
    }

    PBMP_ITER(pbmp, p) {
        if (p < SOC_MAX_NUM_PORTS) {
            for (j = 0; j < TARGET_CELL_COUNT; j++) {
                do {
                    /* coverity[dont_call : FALSE] */
                    pkt_size = (sal_rand() % (MTU - MIN_PKT_SIZE + 1)) +
                               MIN_PKT_SIZE;
                } while (stream_get_pkt_cell_cnt(unit, pkt_size, p) >
                         max_num_cells_param);
                l3uc_p->rand_pkt_sizes[p][j] = pkt_size;
            }
        }
    }
}

/*
 * Function:
 *      l3uc_set_up_ports
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
l3uc_set_up_ports(int unit, pbmp_t pbmp)
{
    int i, p;
    lport_tab_entry_t lport_tab_entry;
    port_tab_entry_t port_tab_entry;
    soc_field_t ihg_lookup_fields[] = {
        HYBRID_MODE_ENABLEf,
        USE_MH_PKT_PRIf,
        USE_MH_VIDf,
        HG_LOOKUP_ENABLEf,
        REMOVE_MH_SRC_PORTf
    };
    uint32 ihg_lookup_values[] = { 0x0, 0x1, 0x1, 0x1, 0x0 };

    cli_out("\nEnabling HG_LOOKUP on HG ports");
    if (SOC_REG_IS_VALID(unit, IHG_LOOKUPr)) {
        PBMP_ITER(pbmp, p) {
            if (p < SOC_MAX_NUM_PORTS && IS_HG_PORT(unit, p)) {
                soc_reg_fields32_modify(unit, IHG_LOOKUPr, p, 5,
                                        ihg_lookup_fields, ihg_lookup_values);
            }
        }
    } else {
        cli_out("\n*ERROR, invalid reg %s\n", SOC_REG_NAME(unit, IHG_LOOKUPr));
    }

    cli_out("\nEnabling Port bridging");
    if (SOC_MEM_IS_VALID(unit, LPORT_TABm)) {
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
    } else {
        cli_out("\n*ERROR, invalid mem %s\n", SOC_MEM_NAME(unit, LPORT_TABm));
    }

    if (SOC_MEM_IS_VALID(unit, PORT_TABm)) {
        for (i = 0; i < soc_mem_index_max(unit, PORT_TABm); i++) {
            (void) soc_mem_read(unit, PORT_TABm, COPYNO_ALL, i,
                         port_tab_entry.entry_data);
            soc_mem_field32_set(unit, PORT_TABm, port_tab_entry.entry_data,
                                PORT_BRIDGEf, 0x1);
            (void) soc_mem_write(unit, PORT_TABm, COPYNO_ALL, i,
                              port_tab_entry.entry_data);
        }
    } else {
        cli_out("\n*ERROR, invalid mem %s\n", SOC_MEM_NAME(unit, PORT_TABm));
    }
}

/*
 * Function:
 *      l3uc_lossless_flood_cnt
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
l3uc_lossless_flood_cnt(int unit, int port)
{
    uint32 pkt_size, flood_cnt = 0;
    int param_flood_cnt, param_pkt_size;
    l3uc_t *l3uc_p = l3uc_parray[unit];

    param_flood_cnt = l3uc_p->flood_pkt_cnt_param;
    param_pkt_size = l3uc_p->pkt_size_param;

    if (param_flood_cnt == 0) {
        if (param_pkt_size == 0) {           
            pkt_size = stream_get_wc_pkt_size(unit, IS_HG_PORT(unit, port));
        } else {
            pkt_size = param_pkt_size;
        }
        flood_cnt = stream_get_ll_flood_cnt(unit, port, pkt_size, NULL);
    } else {
        flood_cnt = param_flood_cnt;
    }

    return (flood_cnt);
}

/*
 * Function:
 *      l3uc_set_up_streams
 * Purpose:
 *      VLAN programming for l3uc. Each port is put on an unique VLAN.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     BCM_E_XXXX
 */
static bcm_error_t
l3uc_set_up_streams(int unit)
{
    int p, j;
    int src_port, dst_port;
    int l2_id;
    pbmp_t pbm, ubm;
    bcm_l2_station_t l2_station;
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t l3_egress;
    bcm_if_t egr_obj_id;
    bcm_l3_host_t l3_host;
    bcm_field_qset_t f_qset;
    bcm_field_group_t f_group;
    bcm_field_entry_t f_entry;
    l3uc_t *l3uc_p = l3uc_parray[unit];
    bcm_mac_t nhop_mac = MAC_DA;
    bcm_mac_t router_mac = MAC_SA;
    bcm_vlan_t vlan = VLAN;
    bcm_ip_t ipv4_da = IPV4_ADDR;
    bcm_ip6_t ipv6_da = IPV6_ADDR;

    BCM_PBMP_CLEAR(ubm);
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, 0, bcmPortControlIP4, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, 0, bcmPortControlIP6, 1));
    BCM_IF_ERROR_RETURN(bcm_field_init(unit));
    BCM_FIELD_QSET_INIT(f_qset);
    BCM_FIELD_QSET_ADD(f_qset, bcmFieldQualifyOuterVlanId);
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, f_qset,
                                               BCM_FIELD_GROUP_PRIO_ANY,
                                               &f_group));

    PBMP_ITER(PBMP_E_ALL(unit), p) {
        if (p < SOC_MAX_NUM_PORTS) {
            src_port = p;
            dst_port = l3uc_p->dst_port[p];
            nhop_mac[2] = dst_port;
            router_mac[2] = src_port;
            vlan = VLAN + src_port;
            ipv4_da = IPV4_ADDR | (src_port << 8);
            ipv6_da[14] = src_port;

            /* Create VLAN */
            BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vlan));
            BCM_PBMP_CLEAR(pbm);
            BCM_PBMP_PORT_ADD(pbm, src_port);
            BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbm, ubm));

            /* Add L2 Station */
            bcm_l2_station_t_init(&l2_station);
            sal_memcpy(l2_station.dst_mac, nhop_mac, 6);
            sal_memcpy(l2_station.dst_mac_mask, "ff:ff:ff:ff:ff:ff", 6);
            l2_station.vlan = vlan;
            l2_station.vlan_mask = 0xfff;
            l2_station.flags |= BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6;
            BCM_IF_ERROR_RETURN(bcm_l2_station_add(unit, &l2_id, &l2_station));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "\nAdded L2 Station: ")));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "mac %2x:%2x:%2x:%2x:%2x:%2x, vlan 0x%x"),
                                 l2_station.dst_mac[5], l2_station.dst_mac[4],
                                 l2_station.dst_mac[3], l2_station.dst_mac[2],
                                 l2_station.dst_mac[1], l2_station.dst_mac[0],
                                 l2_station.vlan));

            /* Create L3 Interfaces */
            bcm_l3_intf_t_init(&l3_intf);
            l3_intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
            sal_memcpy(l3_intf.l3a_mac_addr, router_mac, 6);
            l3_intf.l3a_vid = vlan;
            BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "\nAdded L3 Interface: intf %0d, "),
                                 l3_intf.l3a_intf_id));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "mac %2x:%2x:%2x:%2x:%2x:%2x, vlan 0x%x"),
                                 l3_intf.l3a_mac_addr[5], l3_intf.l3a_mac_addr[4],
                                 l3_intf.l3a_mac_addr[3], l3_intf.l3a_mac_addr[2],
                                 l3_intf.l3a_mac_addr[1], l3_intf.l3a_mac_addr[0],
                                 l3_intf.l3a_vid));

            /* Create Next Hop Egress Object */
            bcm_l3_egress_t_init(&l3_egress);
            sal_memcpy(l3_egress.mac_addr, nhop_mac, 6);
            l3_egress.intf = l3_intf.l3a_intf_id;
            l3_egress.port = src_port;
            BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress,
                                                     &egr_obj_id));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "\nAdded L3 Next Hop: nhop_id %0d, intf %0d, "),
                                 egr_obj_id, l3_egress.intf));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "mac %2x:%2x:%2x:%2x:%2x:%2x, port %0d"),
                                 l3_egress.mac_addr[5], l3_egress.mac_addr[4],
                                 l3_egress.mac_addr[3], l3_egress.mac_addr[2],
                                 l3_egress.mac_addr[1], l3_egress.mac_addr[0],
                                 l3_egress.port));

            /* Associate IP Address with Egress Object */
            bcm_l3_host_t_init(&l3_host);
            if (l3uc_p->ipv6_param) {
                l3_host.l3a_flags = BCM_L3_IP6;
                sal_memcpy(l3_host.l3a_ip6_addr, ipv6_da, 16);
            } else {
                l3_host.l3a_ip_addr = ipv4_da;
            }
            l3_host.l3a_intf = egr_obj_id;
            BCM_IF_ERROR_RETURN(bcm_l3_host_add(unit, &l3_host));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "\nAdded L3 Table: nhop_id %0d, ip "),
                                 egr_obj_id));
            if (l3uc_p->ipv6_param) {
                for (j = 0; j < 16; j++) {
                    LOG_INFO(BSL_LS_APPL_TESTS,
                             (BSL_META_U(unit, "%0d."),
                                         l3_host.l3a_ip6_addr[j]));
                }
            } else {
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "%0d.%0d.%0d.%0d"),
                                     (l3_host.l3a_ip_addr) >> 24 & 0xff,
                                     (l3_host.l3a_ip_addr) >> 16 & 0xff,
                                     (l3_host.l3a_ip_addr) >> 8 & 0xff,
                                     l3_host.l3a_ip_addr & 0xff));
            }

            /* Program FP to L3 Switch */
            {
                BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, f_group, &f_entry));
                BCM_IF_ERROR_RETURN(bcm_field_qualify_OuterVlanId(unit, f_entry,
                                                           VLAN + dst_port, 0xfff));
                BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, f_entry,
                                                         bcmFieldActionL3Switch,
                                                         egr_obj_id, 0));
                BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, f_entry));
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "\nAdded FP L3 Switch vlan 0x%x to "),
                                     VLAN + dst_port));
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "nhop_id %0d\n"), egr_obj_id));
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      l3uc_send_pkts
 * Purpose:
 *      Send packets to flood VLANs and create a swirl on each port.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
static void
l3uc_send_pkts(int unit)
{
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    int port, i = 0;
    uint32 pkt_size = 0,  pkt_count = 0, flood_cnt = 0;
    uint8 src_port;
    uint8 dst_port;
    uint32 use_random_packet_sizes = 0;
    l3uc_t *l3uc_p = l3uc_parray[unit];
    stream_pkt_t *tx_pkt;

    tx_pkt = sal_alloc(sizeof(stream_pkt_t), "tx_pkt");
    sal_memset(tx_pkt, 0, sizeof(stream_pkt_t));

    if (l3uc_p->pkt_size_param == 1) {
        use_random_packet_sizes = 1;
    }

    cli_out("\n==================================================\n");
    cli_out("\nSending packets ...\n\n");
    PBMP_ITER(PBMP_E_ALL(unit), port) {
        if (port < SOC_MAX_NUM_PORTS) {
            i++;
            src_port = port;
            dst_port = l3uc_p->dst_port[port];
            if (i % 2 == 0) {
                flood_cnt = 0;
                continue;
            } else {
                flood_cnt = l3uc_lossless_flood_cnt(unit, port);
            }

            if (l3uc_p->pkt_size_param == 0) {
                pkt_size = stream_get_wc_pkt_size(unit, IS_HG_PORT(unit, port));
            } else {
                pkt_size = l3uc_p->pkt_size_param;
            }

            tx_pkt->port = port;
            tx_pkt->num_pkt = flood_cnt;
            tx_pkt->pkt_seed = l3uc_p->pkt_seed + port;
            tx_pkt->pkt_size = pkt_size;
            tx_pkt->rand_pkt_size_en = use_random_packet_sizes;
            tx_pkt->rand_pkt_size = NULL;
            tx_pkt->tx_vlan = VLAN + src_port;
            sal_memcpy(tx_pkt->mac_da, mac_da, NUM_BYTES_MAC_ADDR);
            sal_memcpy(tx_pkt->mac_sa, mac_sa, NUM_BYTES_MAC_ADDR);
            tx_pkt->mac_da[2] = dst_port;
            tx_pkt->mac_sa[2] = src_port;
            if (tx_pkt->rand_pkt_size_en) {
                tx_pkt->rand_pkt_size = l3uc_p->rand_pkt_sizes[port];
            }
            tx_pkt->l3_en   = 1;
            tx_pkt->ipv6_en = l3uc_p->ipv6_param;
            tx_pkt->ip_da   = IPV4_ADDR | (dst_port << 8);
            tx_pkt->ip_sa   = IPV4_ADDR | (src_port << 8);
            tx_pkt->ttl     = TTL;
            stream_tx_pkt(unit, tx_pkt);

            /* print */
            cli_out("flood_cnt for stream %0d = %0d\n", i/2, tx_pkt->cnt_pkt);
            pkt_count += tx_pkt->cnt_pkt;
        }
    }
    cli_out("\nTotal packet sent: %d\n\n", pkt_count);
    sal_free(tx_pkt);
}

/*
 * Function:
 *      l3uc_chk_port_rate
 * Purpose:
 *      Check actual port rates against expected port rates.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     SOC_E_XXXX
 */
static bcm_error_t
l3uc_chk_port_rate(int unit)
{
    int port;
    bcm_error_t rv = BCM_E_NONE;
    stream_rate_t *rate_calc;
    l3uc_t *l3uc_p = l3uc_parray[unit];

    rate_calc = (stream_rate_t *)
                 sal_alloc(sizeof(stream_rate_t), "rate_calc");
    if (rate_calc == NULL) {
        test_error(unit, "Failed to allocate memory for rate_calc\n");
        return BCM_E_FAIL;
    }
    sal_memset(rate_calc, 0, sizeof(stream_rate_t));

    rate_calc->mode         = 0; /* check act_rate against config_rate */
    rate_calc->pkt_size     = l3uc_p->pkt_size_param;
    rate_calc->interval_len = l3uc_p->rate_calc_interval_param;
    rate_calc->tolerance_lr = l3uc_p->rate_tolerance_lr_param;
    rate_calc->tolerance_os = l3uc_p->rate_tolerance_ov_param;
    rate_calc->scaling_factor = l3uc_p->scaling_factor_param;

    SOC_PBMP_CLEAR(rate_calc->pbmp);
    PBMP_ITER(PBMP_E_ALL(unit), port) {
        if (port < SOC_MAX_NUM_PORTS) {
            SOC_PBMP_PORT_ADD(rate_calc->pbmp, port);
        }
    }

    rv = stream_chk_port_rate(unit, PBMP_PORT_ALL(unit), rate_calc);

    sal_free(rate_calc);
    return rv;
}

/*
 * Function:
 *      l3uc_chk_pkt_integrity
 * Purpose:
 *      Redirect all packets back to CPU and check packet integrity
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
static bcm_error_t
l3uc_chk_pkt_integrity(int unit)
{
    int idx = 0, port, dst_port, src_port, is_even;
    int param_pkt_seed;
    uint32 flood_cnt;
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    stream_integrity_t *pkt_intg;
    bcm_error_t rv = BCM_E_NONE;
    l3uc_t *l3uc_p = l3uc_parray[unit];

    param_pkt_seed = l3uc_p->pkt_seed;

    pkt_intg = sal_alloc(sizeof(stream_integrity_t), "pkt_intg");
    if (pkt_intg == NULL) {
        test_error(unit, "Failed to allocate memory for pkt_intg\n");
        return BCM_E_FAIL;
    }
    sal_memset(pkt_intg, 0, sizeof(stream_integrity_t));

    pkt_intg->type = PKT_TYPE_L3UC;
    pkt_intg->ipv6_en = (l3uc_p->ipv6_param > 0) ? 1 : 0;
    SOC_PBMP_CLEAR(pkt_intg->rx_pbmp);
    PBMP_ITER(PBMP_E_ALL(unit), port) {
        if (port < SOC_MAX_NUM_PORTS) {
            is_even = (idx % 2 == 0) ? (1) : (0);
            src_port = port;
            dst_port = l3uc_p->dst_port[port];
            /* rx_pbmp */
            if (is_even == 1) {
                SOC_PBMP_PORT_ADD(pkt_intg->rx_pbmp, port);
            }
            /* rx_vlan: used to forward pkt to CPU */
            pkt_intg->rx_vlan[port] = VLAN + dst_port;
            /* tx_vlan: used to re-generate ref_pkt */
            pkt_intg->tx_vlan[port] = VLAN + src_port;
            /* port_flood_cnt */
            if (is_even) {
                flood_cnt = l3uc_lossless_flood_cnt(unit, port);
            } else {
                flood_cnt = 0;
            }
            pkt_intg->port_flood_cnt[port] = flood_cnt;
            /* port_pkt_seed */
            pkt_intg->port_pkt_seed[port] = param_pkt_seed + port;
            /* mac_da, mac_sa */
            mac_da[2] = dst_port;
            mac_sa[2] = src_port;
            sal_memcpy(pkt_intg->mac_da[port], mac_da, NUM_BYTES_MAC_ADDR);
            sal_memcpy(pkt_intg->mac_sa[port], mac_sa, NUM_BYTES_MAC_ADDR);
            /* ip_da, ip_sa */
            pkt_intg->ip_da[port] = IPV4_ADDR | (dst_port << 8);
            pkt_intg->ip_sa[port] = IPV4_ADDR | (src_port << 8);

            idx++;
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
 *      l3uc_test_init
 * Purpose:
 *      Test init function. Parse CLI params and do necessary init.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
l3uc_test_init(int unit, args_t *a, void **pa)
{
    int i, p, src_port;
    l3uc_t *l3uc_p;

    l3uc_p = l3uc_parray[unit];
    l3uc_p = sal_alloc(sizeof(l3uc_t), "l3uc_test");
    sal_memset(l3uc_p, 0, sizeof(l3uc_t));
    l3uc_parray[unit] = l3uc_p;

    cli_out("\nCalling l3uc_test_init");
    l3uc_parse_test_params(unit, a);

    for (p = 0; p < SOC_MAX_NUM_PORTS; p++) {
        l3uc_p->dst_port[p] = 0;
    }

    i = 0;
    src_port = 0;
    PBMP_ITER(PBMP_E_ALL(unit), p) {
        if (p < SOC_MAX_NUM_PORTS) {
            if (i % 2 == 0) {
                src_port = p;
            } else {
                l3uc_p->dst_port[src_port] = p;
                l3uc_p->dst_port[p] = src_port;
            }
            i++;
        }
    }

    l3uc_p->test_fail = 0;

    return BCM_E_NONE;
}


/*
 * Function:
 *      l3uc_test
 * Purpose:
 *      Set up ports/streams and send packets.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
l3uc_test(int unit, args_t *a, void *pa)
{
    int rv = BCM_E_NONE;
    l3uc_t *l3uc_p = l3uc_parray[unit];

    if (l3uc_p->bad_input) {
        goto done;
    }

    cli_out("\nCalling l3uc_test");

    stream_set_lpbk(unit, PBMP_PORT_ALL(unit), l3uc_p->loopback_mode_param);
    stream_turn_off_cmic_mmu_bkp(unit);
    stream_turn_off_fc(unit, PBMP_PORT_ALL(unit));
    l3uc_p->pkt_seed = sal_rand();
    l3uc_set_port_property(unit, PBMP_PORT_ALL(unit), l3uc_p->pkt_size_param,
                             l3uc_p->max_num_cells_param);
    l3uc_set_up_ports(unit, PBMP_PORT_ALL(unit));
    l3uc_set_up_streams(unit);
    l3uc_send_pkts(unit);

    /* check counter */
    if (stream_chk_mib_counters(unit, PBMP_PORT_ALL(unit), 0) != BCM_E_NONE) {
        l3uc_p->test_fail = 1;
    }
    /* check rate */
    if (l3uc_chk_port_rate(unit) != BCM_E_NONE) {
        l3uc_p->test_fail = 1;
    }
    /* check integrity */
    if (l3uc_chk_pkt_integrity(unit) != BCM_E_NONE) {
        l3uc_p->test_fail = 1;
    }


done:
    return rv;
}


/*
 * Function:
 *      l3uc_test_cleanup
 * Purpose:
 *      Do test end checks and free all allocated memory.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
l3uc_test_cleanup(int unit, void *pa)
{
    int i;
    bcm_error_t rv = BCM_E_NONE;
    l3uc_t *l3uc_p = l3uc_parray[unit];

    cli_out("\nCalling l3uc_test_cleanup");

    if (l3uc_p->bad_input == 1) {
        l3uc_p->test_fail = 1;
    }
    if (l3uc_p->test_fail == 1) {
        rv = BCM_E_FAIL;
    }

    cli_out("\n==================================================");
    cli_out("\n==================================================");
    if (l3uc_p->test_fail == 1) {
        cli_out("\n[L3UC test failed]\n\n");
    } else {
        cli_out("\n[L3UC test passed]\n\n");
    }

    /* free mem */
    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        sal_free(l3uc_p->rand_pkt_sizes[i]);
    }
    sal_free(l3uc_p->rand_pkt_sizes);
    sal_free(l3uc_p);
    bcm_vlan_init(unit);
    return rv;
}
#endif /* BCM_ESW_SUPPORT && INCLUDE_L3 */
