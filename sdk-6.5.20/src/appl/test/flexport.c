/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * The flexport test checks the flexport and TDM functionalities by streaming
 * L2UC packets on all ports at maximum rate. All ports are configured in
 * MAC or PHY loopback mode and each port is associated with one VLAN. The test
 * calculates the number of packets needed to saturate each port and send the
 * L2UC packets with the port's VLAN tag from the CPU to each port initially.
 * Then the packets DLF and flood to the VLAN which contains one port. Port
 * bridging is enabled to allow the DLF packet to go back to its ingress port.
 * Thus, the packets keep looping back within each port indefinitely. The
 * flexport functionality is checked by flexing one or more port macros to
 * a different mode/speed/encapsulation. The port macros and mode/speed/
 * encapsulation can be specify either by command line or through a series of
 * config.bcm files. Once the traffic reaches steady state, rate and packet
 * integrity are checked on the flex ports. Then, traffic will be stopped on the
 * flex ports before calling the BCM FlexPort API to performance the flexport
 * operation. Traffic on the non-flex ports will not be interrupted. The rate
 * calculation is done by dividing the transmit packet count changes and
 * transmit byte count changes over a programmable interval. The rates are
 * checked against expected rates based on port configuration and
 * oversubscription ratio. Packet integrity check is achieved by disabling the
 * VLAN so the packets go back to the CPU as unknown VLAN. The packets are
 * compared against expected packets to ensure packet integrity.
 *
 * Configuration parameters passed from CLI:
 * PktSize: Packet size in bytes. Set to 0 for worst case packet sizes on all
 *          ports (145B for ENET, 76B for HG2). Set to 1 for random packet sizes
 * FloodCnt: Number of packets in each swirl. Setting this to 0 will let the
 *           test calculate the number of packets that can be sent to achieve
 *           a lossless swirl at full rate. Set to 0 by default.
 * RateCalcInt: Interval in seconds over which rate is to be calculated
 * TolLr: Rate tolerance percentage for line rate ports (1% by default).
 * TolOv: Rate tolerance percentage for oversubscribed ports (3% by default).
 * ChkPktInteg: Set to 0 to disable packet integrity checks, 1 to enable
                (default).
 * MaxNumCells: Max number of cells for random packet sizes. Default = 4. Set
 *              to 0 for random.
 * LoopbackMode: Loopback mode. Set to 1 for MAC loopback, 2 for PHY loopback.
 *               (default is MAC loopback).
 * ConfigFile: Config files for each succession flexport operation.
 *             (default is none)
 * Tsc<num>: TSC to be flexed and port mode to flex to, e.g. Tsc5=4x10G.
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

#define PKT_SIZE_PARAM_DEFAULT 0
#define FLOOD_PKT_CNT_PARAM_DEFAULT 0
#define RATE_CALC_INTERVAL_PARAM_DEFAULT 10
#define RATE_TOLERANCE_LR_PARAM_DEFAULT 1
#define RATE_TOLERANCE_OV_PARAM_DEFAULT 3
#define CHECK_PACKET_INTEGRITY_PARAM_DEFAULT 1
#define EMULATION_PARAM_DEFAULT 0
#define MAX_NUM_CELLS_PARAM_DEFAULT 4
#define NO_SPEED_CHANGE_PARAM_DEFAULT 1

#define TCP_CMD_BUF_LEN 512

#define MAC_DA {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc}
#define MAC_SA {0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54}
#define TPID 0x8100
#define VLAN 0x0a00


#if defined(BCM_ESW_SUPPORT) && \
           (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)) && \
           !defined(NO_SAL_APPL) && (!defined(__KERNEL__))

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_TSC_NUM 65

/* TSC Info */
#define TDMV_NUM_TSC(unit)       (SOC_IS_TOMAHAWK2(unit) ? 64 : 32)
#define NUM_PORT_MODULES(unit)   (TDMV_NUM_TSC(unit) + 1)
#define TDMV_NUM_TSC_LANES 4

/* Encap types */
#define PM_ENCAP__HIGIG2 999
#define PM_ENCAP__ETHRNT 998

enum port_state_e {
    PORT_STATE__DISABLED    = 0x0,
    PORT_STATE__LINERATE    = 0x1,
    PORT_STATE__OVERSUB     = 0x2,
    PORT_STATE__COMBINE     = 0x3,
    PORT_STATE__LINERATE_HG = 0x5,
    PORT_STATE__OVERSUB_HG  = 0x6,
    PORT_STATE__COMBINE_HG  = 0x7,
    PORT_STATE__MANAGEMENT  = 0x9,
    PORT_STATE__MUTABLE     = 0x11,
    PORT_STATE__CARDINAL    = 0x19
};

/* Speed set (Gbps) */
typedef enum {
    SPEED_UI_0        = 0,
    SPEED_UI_1G       = 1,
    SPEED_UI_2p5G     = 2,
    SPEED_UI_10G      = 10,
    SPEED_UI_20G      = 20,
    SPEED_UI_21G_DUAL = 21,
    SPEED_UI_25G      = 25,
    SPEED_UI_40G      = 40,
    SPEED_UI_41G_DUAL = 41,
    SPEED_UI_42G      = 42,
    SPEED_UI_50G      = 50,
    SPEED_UI_100G     = 100,
    SPEED_UI_120G     = 120
} port_speed_t;

typedef struct flexport_s {
    pbmp_t vlan_pbmp;
    pbmp_t port_down_pbmp;
    pbmp_t port_up_pbmp;
    pbmp_t speed_change_pbmp;
    uint32 pkt_size_param;
    uint32 flood_pkt_cnt_param;
    uint32 rate_calc_interval_param;
    uint32 rate_tolerance_lr_param;
    uint32 rate_tolerance_ov_param;
    uint32 check_packet_integrity_param;
    uint32 emulation_param;
    char *server_name_param;
    uint32 socket_num_param;
    uint32 max_num_cells_param;
    uint32 loopback_mode_param;
    uint32 no_speed_change_param;
    uint32 init_tsc_shim;
    uint32 flex_by_command_line;
    char *config_file_name;
    char *tsc_param[MAX_TSC_NUM];
    uint32 num_del;
    uint32 num_add;
    bcm_port_resource_t del_resource[SOC_MAX_NUM_PORTS];
    bcm_port_resource_t add_resource[SOC_MAX_NUM_PORTS];
    int last_assigned_port[SOC_MAX_NUM_PIPES];
    int flex_tsc_map[MAX_TSC_NUM];
    int linkscan_enable;
    uint32 counter_flags;
    int counter_interval;
    pbmp_t counter_pbm;
    uint32 bad_input;
    uint32 test_fail;
    uint32 pkt_seed;
    int sockfd;
    char tsc_cfg_buffer[12][TCP_CMD_BUF_LEN];
    int tsc_cfg_cmds;
    int tsc_cfg_ptr;
    int clear_new_port_counts;
} flexport_t;

typedef struct rate_calc_s {
    uint32 ovs_ratio_x1000[SOC_MAX_NUM_PIPES];
    uint32 rand_pkt_sizes[SOC_MAX_NUM_PORTS][TARGET_CELL_COUNT];
    uint64 stime[SOC_MAX_NUM_PORTS];
    uint64 exp_rate[SOC_MAX_NUM_PORTS];
    uint64 tpkt_start[SOC_MAX_NUM_PORTS];
    uint64 tbyt_start[SOC_MAX_NUM_PORTS];
} rate_calc_t;

static flexport_t *flexport_parray[SOC_MAX_NUM_DEVICES];
static rate_calc_t *rate_calc_parray[SOC_MAX_NUM_DEVICES];

static const char flexport_test_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nDocumentation too long to be displayed with -pedantic compiler\n";
#else
    "\nFlexPort test usage:\n"
    " \n"
    "PktSize:      Packet size in bytes. Set to 1 for random packet sizes.\n"
    "              Set to 0 (default) for worst case packet sizes on all ports\n"
    "              (145B for ENET, 76B for HG2).\n"
    "FloodCnt:     Number of packets in each swirl between.\n"
    "              Set to 0 (default) for a lossless swirl at full rate.\n"
    "RateCalcInt:  Interval in seconds over which rate is to be calculated.\n"
    "TolLr:        Rate tolerance percentage for line rate ports.\n"
    "              (1% by default)\n"
    "TolOv:        Rate tolerance percentage for oversubscribed ports.\n"
    "              (3% by default).\n"
    "ChkPktInteg:  Set to 0 to disable packet integrity checks.\n"
    "              Set to 1 to enable (default).\n"
    "Emulation:    Set to 1 to send TSC clock commands for emulation.\n"
    "              Set to 0 to disable (default).\n"
    "ServerName:   Name (Linux) of the TSC clock server.\n"
    "              Required for emulation\n."
    "SocketNum:    Socket number of the TSC clock server.\n"
    "              Required for emulation\n."
    "MaxNumCells:  Maximum number of cells for random packet sizes.\n"
    "              Set to 0 for random cell sizes. (default is 4)\n"
    "LoopbackMode: Loopback mode. Set to 1 for MAC loopback, 2 for PHY loopback.\n"
    "              (default is MAC loopback)\n"
    "ConfigFile:   Config files for each succession flexport operation.\n"
    "              (default is none)\n"
    "Tsc<num>:     TSC to be flexed and port mode to flex to, e.g. Tsc5=4x10G\n"
    "              The supported mode/speed in Ethernet encapsulation are:\n"
    "NoSpeedChg:   Do not call the bcm_port_speed change. (default)\n"
    ;
#endif

static const char td2p_supported_mode[] =
    "              1x100G, 1x120G, 10x10G, 12x10G,\n"
    "              1x40G, 1x20G, 1x10G, 2x20G, 2x10G,\n"
    "              20G+2x10G, 2x10G+20G,\n"
    "              4x10G, 4x2.5G, and 4x1G\n"
    "              The supported mode/speed in HiGig2 encapsulation are:\n"
    "              1x106G, 1x127G, 10x11G, 12x11G,\n"
    "              1x42G, 1x21G, 1x11G, 2x21G, and 4x11G\n"
    ;

static const char th2_supported_mode[] =
    "              1x100G, 1x40G\n"
    "              2x50G, 2x40G, 2x20G,\n"
    "              20G+2x10G, 2x10G+20G,\n"
    "              50G+2x25G, 2x25G+50G,\n"
    "              4x25G, and 4x10G\n"
    "              The supported mode/speed in HiGig2 encapsulation are:\n"
    "              1x106G, 1x42G,\n"
    "              2x53G, 2x42G, 2x21G,\n"
    "              4x27G, and 4x11G\n"
    ;

static const char td3_supported_mode[] =
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
 *      flexport_p->bad_input set from here - tells test to crash out in case
 *      CLI input combination is invalid.
 */

static void
flexport_parse_test_params(int unit, args_t *a)
{
    int i;
    parse_table_t parse_table;
    flexport_t *flexport_p = flexport_parray[unit];

    flexport_p->bad_input = 0;

    flexport_p->pkt_size_param = PKT_SIZE_PARAM_DEFAULT;
    flexport_p->flood_pkt_cnt_param = FLOOD_PKT_CNT_PARAM_DEFAULT;
    flexport_p->rate_calc_interval_param = RATE_CALC_INTERVAL_PARAM_DEFAULT;
    flexport_p->rate_tolerance_lr_param = RATE_TOLERANCE_LR_PARAM_DEFAULT;
    flexport_p->rate_tolerance_ov_param = RATE_TOLERANCE_OV_PARAM_DEFAULT;
    flexport_p->check_packet_integrity_param
                            = CHECK_PACKET_INTEGRITY_PARAM_DEFAULT;
    flexport_p->emulation_param = EMULATION_PARAM_DEFAULT;
    flexport_p->socket_num_param = 0;
    flexport_p->max_num_cells_param = MAX_NUM_CELLS_PARAM_DEFAULT;
    flexport_p->loopback_mode_param = BCM_PORT_LOOPBACK_MAC;
    flexport_p->no_speed_change_param = NO_SPEED_CHANGE_PARAM_DEFAULT;
    flexport_p->init_tsc_shim = 0;
    flexport_p->flex_by_command_line = 0;
    flexport_p->clear_new_port_counts = 0;

    /*Parse CLI opts */

    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table, "PktSize", PQ_INT|PQ_DFL, 0,
                    &flexport_p->pkt_size_param, NULL);
    parse_table_add(&parse_table, "FloodCnt", PQ_INT|PQ_DFL, 0,
                    &flexport_p->flood_pkt_cnt_param, NULL);
    parse_table_add(&parse_table, "RateCalcInt", PQ_INT|PQ_DFL, 0,
                    &flexport_p->rate_calc_interval_param, NULL);
    parse_table_add(&parse_table, "TolLr", PQ_INT|PQ_DFL, 0,
                    &flexport_p->rate_tolerance_lr_param, NULL);
    parse_table_add(&parse_table, "TolOv", PQ_INT|PQ_DFL, 0,
                    &flexport_p->rate_tolerance_ov_param, NULL);
    parse_table_add(&parse_table, "ChkPktInteg", PQ_INT|PQ_DFL, 0,
                    &flexport_p->check_packet_integrity_param, NULL);
    parse_table_add(&parse_table, "Emulation", PQ_INT|PQ_DFL, 0,
                    &flexport_p->emulation_param, NULL);
    parse_table_add(&parse_table, "ServerName", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->server_name_param, NULL);
    parse_table_add(&parse_table, "SocketNum", PQ_INT|PQ_DFL, 0,
                    &flexport_p->socket_num_param, NULL);
    parse_table_add(&parse_table, "MaxNumCells", PQ_INT|PQ_DFL, 0,
                    &flexport_p->max_num_cells_param, NULL);
    parse_table_add(&parse_table, "LoopbackMode", PQ_INT|PQ_DFL, 0,
                    &flexport_p->loopback_mode_param, NULL);
    parse_table_add(&parse_table, "ConfigFile", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->config_file_name, NULL);
    parse_table_add(&parse_table, "Tsc0", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[0], NULL);
    parse_table_add(&parse_table, "Tsc1", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[1], NULL);
    parse_table_add(&parse_table, "Tsc2", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[2], NULL);
    parse_table_add(&parse_table, "Tsc3", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[3], NULL);
    parse_table_add(&parse_table, "Tsc4", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[4], NULL);
    parse_table_add(&parse_table, "Tsc5", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[5], NULL);
    parse_table_add(&parse_table, "Tsc6", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[6], NULL);
    parse_table_add(&parse_table, "Tsc7", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[7], NULL);
    parse_table_add(&parse_table, "Tsc8", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[8], NULL);
    parse_table_add(&parse_table, "Tsc9", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[9], NULL);
    parse_table_add(&parse_table, "Tsc10", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[10], NULL);
    parse_table_add(&parse_table, "Tsc11", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[11], NULL);
    parse_table_add(&parse_table, "Tsc12", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[12], NULL);
    parse_table_add(&parse_table, "Tsc13", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[13], NULL);
    parse_table_add(&parse_table, "Tsc14", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[14], NULL);
    parse_table_add(&parse_table, "Tsc15", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[15], NULL);
    parse_table_add(&parse_table, "Tsc16", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[16], NULL);
    parse_table_add(&parse_table, "Tsc17", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[17], NULL);
    parse_table_add(&parse_table, "Tsc18", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[18], NULL);
    parse_table_add(&parse_table, "Tsc19", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[19], NULL);
    parse_table_add(&parse_table, "Tsc20", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[20], NULL);
    parse_table_add(&parse_table, "Tsc21", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[21], NULL);
    parse_table_add(&parse_table, "Tsc22", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[22], NULL);
    parse_table_add(&parse_table, "Tsc23", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[23], NULL);
    parse_table_add(&parse_table, "Tsc24", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[24], NULL);
    parse_table_add(&parse_table, "Tsc25", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[25], NULL);
    parse_table_add(&parse_table, "Tsc26", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[26], NULL);
    parse_table_add(&parse_table, "Tsc27", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[27], NULL);
    parse_table_add(&parse_table, "Tsc28", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[28], NULL);
    parse_table_add(&parse_table, "Tsc29", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[29], NULL);
    parse_table_add(&parse_table, "Tsc30", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[30], NULL);
    parse_table_add(&parse_table, "Tsc31", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[31], NULL);
    parse_table_add(&parse_table, "Tsc32", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[32], NULL);
    parse_table_add(&parse_table, "Tsc33", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[33], NULL);
    parse_table_add(&parse_table, "Tsc34", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[34], NULL);
    parse_table_add(&parse_table, "Tsc35", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[35], NULL);
    parse_table_add(&parse_table, "Tsc36", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[36], NULL);
    parse_table_add(&parse_table, "Tsc37", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[37], NULL);
    parse_table_add(&parse_table, "Tsc38", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[38], NULL);
    parse_table_add(&parse_table, "Tsc39", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[39], NULL);
    parse_table_add(&parse_table, "Tsc40", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[40], NULL);
    parse_table_add(&parse_table, "Tsc41", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[41], NULL);
    parse_table_add(&parse_table, "Tsc42", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[42], NULL);
    parse_table_add(&parse_table, "Tsc43", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[43], NULL);
    parse_table_add(&parse_table, "Tsc44", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[44], NULL);
    parse_table_add(&parse_table, "Tsc45", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[45], NULL);
    parse_table_add(&parse_table, "Tsc46", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[46], NULL);
    parse_table_add(&parse_table, "Tsc47", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[47], NULL);
    parse_table_add(&parse_table, "Tsc48", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[48], NULL);
    parse_table_add(&parse_table, "Tsc49", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[49], NULL);
    parse_table_add(&parse_table, "Tsc50", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[50], NULL);
    parse_table_add(&parse_table, "Tsc51", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[51], NULL);
    parse_table_add(&parse_table, "Tsc52", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[52], NULL);
    parse_table_add(&parse_table, "Tsc53", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[53], NULL);
    parse_table_add(&parse_table, "Tsc54", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[54], NULL);
    parse_table_add(&parse_table, "Tsc55", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[55], NULL);
    parse_table_add(&parse_table, "Tsc56", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[56], NULL);
    parse_table_add(&parse_table, "Tsc57", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[57], NULL);
    parse_table_add(&parse_table, "Tsc58", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[58], NULL);
    parse_table_add(&parse_table, "Tsc59", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[59], NULL);
    parse_table_add(&parse_table, "Tsc60", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[60], NULL);
    parse_table_add(&parse_table, "Tsc61", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[61], NULL);
    parse_table_add(&parse_table, "Tsc62", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[62], NULL);
    parse_table_add(&parse_table, "Tsc63", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[63], NULL);
    parse_table_add(&parse_table, "NoSpeedChg", PQ_INT|PQ_DFL, 0,
                    &flexport_p->no_speed_change_param, NULL);
    parse_table_add(&parse_table, "InitTscShim", PQ_INT|PQ_DFL, 0,
                    &flexport_p->init_tsc_shim, NULL);
    parse_table_add(&parse_table, "ClrNewPortCounts", PQ_INT|PQ_DFL, 0,
                    &flexport_p->clear_new_port_counts, NULL);

    if (parse_arg_eq(a, &parse_table) < 0 || ARG_CNT(a) != 0) {
        if (SOC_IS_TRIDENT2X(unit)) {
            cli_out("%s%s", flexport_test_usage, td2p_supported_mode);
        }
        if (SOC_IS_TOMAHAWK2(unit)) {
            cli_out("%s%s", flexport_test_usage, th2_supported_mode);
        }
        if (SOC_IS_TRIDENT3X(unit)) {
            cli_out("%s%s", flexport_test_usage, td3_supported_mode);
        }
        test_error(unit, "\n*ERROR PARSING ARGS\n");
    }

    cli_out("\n ------------- PRINTING TEST PARAMS ------------------");
    cli_out("\nPktSize      = %0d", flexport_p->pkt_size_param);
    cli_out("\nRateCalcInt  = %0d", flexport_p->rate_calc_interval_param);
    cli_out("\nFloodCnt     = %0d", flexport_p->flood_pkt_cnt_param);
    cli_out("\nTolLr        = %0d", flexport_p->rate_tolerance_lr_param);
    cli_out("\nTolOv        = %0d", flexport_p->rate_tolerance_ov_param);
    cli_out("\nChkPktInteg  = %0d", flexport_p->check_packet_integrity_param);
    cli_out("\nEmulation    = %0d", flexport_p->emulation_param);
    cli_out("\nMaxNumCells  = %0d", flexport_p->max_num_cells_param);
    cli_out("\nLoopbackMode = %0d", flexport_p->loopback_mode_param);
    cli_out("\nConfigFile   = %s",  flexport_p->config_file_name);
    cli_out("\nClrNewPortCounts   = %0d",  flexport_p->clear_new_port_counts);
    for (i = 0; i < MAX_TSC_NUM; i++) {
       if (flexport_p->tsc_param[i] != NULL) {
           cli_out("\nTsc%2d       = %s", i, flexport_p->tsc_param[i]);
           if(i >= TDMV_NUM_TSC(unit)) {
               flexport_p->bad_input = 1;
           }
           flexport_p->flex_by_command_line = 1;
       }
    }
    if (flexport_p->flex_by_command_line == 0) {
        cli_out("\nTsc<num>    = (null)");
    }
    cli_out("\nNoSpeeedChg   = %0d", flexport_p->no_speed_change_param);
    cli_out("\nInitTscShim   = %0d", flexport_p->init_tsc_shim);
    cli_out("\n -----------------------------------------------------");

    if (flexport_p->max_num_cells_param == 0) {
        /* coverity[dont_call : FALSE] */
        flexport_p->max_num_cells_param = (sal_rand() % (MTU_CELL_CNT - 1)) + 1;
    }

    if (flexport_p->pkt_size_param == 0) {
      if(SOC_IS_TRIDENT3X(unit))
        cli_out
            ("\nUsing worst case packet sizes - 64B for Ethernet and HG2");
      else
        cli_out
            ("\nUsing worst case packet sizes - 145B for Ethernet and "
             "76B for HG2");
    } else if (flexport_p->pkt_size_param == 1) {
        cli_out("\nUsing random packet sizes");
    } else if (flexport_p->pkt_size_param < MIN_PKT_SIZE) {
        test_error(unit,"*ERROR: Packet size cannot be lower than %0dB\n",
                MIN_PKT_SIZE);
        flexport_p->bad_input = 1;
    } else if (flexport_p->pkt_size_param > MTU) {
        test_error(unit,"*ERROR: Packet size higher than %0dB (Device MTU)\n",
                MTU);
        flexport_p->bad_input = 1;
    }

    if (flexport_p->flood_pkt_cnt_param == 0) {
        cli_out("\nFloodCnt=0, automatically calculate number of packets to "
                "flood each port");
    }

    if ((flexport_p->loopback_mode_param != BCM_PORT_LOOPBACK_MAC) &&
        (flexport_p->loopback_mode_param != BCM_PORT_LOOPBACK_PHY) &&
        (flexport_p->loopback_mode_param != BCM_PORT_LOOPBACK_PHY_REMOTE) &&
        (flexport_p->loopback_mode_param != BCM_PORT_LOOPBACK_NONE)) {
        /*
         *   0 - no loopback; only to be used with chip external loopbacks.
         *   1 - MAC outer loopback.
         *   2 - PCS local loopback, close to PMD boundary.
         *   3 - PMD local loopback, close to AFE.
         */
        test_error(unit,"*ERROR: Loopback mode must be either 0, 1, 2 or 3\n");
        flexport_p->bad_input = 1;
    }

    if ((flexport_p->config_file_name != NULL &&
         flexport_p->flex_by_command_line != 0) ||
        (flexport_p->config_file_name == NULL &&
         flexport_p->flex_by_command_line == 0)) {
        test_error(unit,"*ERROR: Please specify ConfigFile or Tsc\n");
        flexport_p->bad_input = 1;
    }

    if (flexport_p->emulation_param &&
        (flexport_p->server_name_param == NULL ||
         flexport_p->socket_num_param == 0)) {
        test_error(unit,"*ERROR: ServerName and SocketNum must be specified\n");
        flexport_p->bad_input = 1;
    }

}


static int
connectToSocket(int portno, char* serverName)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server = NULL;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cli_out("\nconnectToSocket : ERROR opening socket\n");
        return -1;
    }
    sal_memset((char *) &serv_addr, 0, sizeof(serv_addr));

    server = gethostbyname(serverName);
    if (server == NULL) {
        cli_out("\nconnectToSocket : ERROR no such host '%s'\n", serverName);
        close(sockfd);
        return -1;
    }
    sal_memcpy((char *) &serv_addr.sin_addr.s_addr, server->h_addr_list[0],
            server->h_length);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cli_out("\nconnectToSocket : ERROR connecting\n");
        close(sockfd);
        return -1;
    } else {
        cli_out("\nconnectToSocket : Connected to '%s' %0d\n", serverName, portno);
    }
    return sockfd;
}


static bcm_error_t
sendDataToSocket(int sockfd, char* msg)
{
    int rv;
    rv = write(sockfd, msg, strlen(msg));
    if (rv < 0) {
         cli_out("\nsendDataToSocket : ERROR writing to socket\n");
         return -1;
    }
    cli_out("\nsendDataToSocket : %s \n", msg);
    return BCM_E_NONE;
}


/*
 * Function:
 *      tdm_chomp
 * Purpose:
 *      Removes newline from portmap strings
 * Parameters:
 *      str: Pointer to a string
 *
 * Returns:
 *     Nothing
 */

static
void tdm_chomp(char *str)
{
    while (*str && *str != '\n' && *str != '\r') {
        str++;
    }
    *str = 0;
}


/*
 * Function:
 *      convert_hex2bin
 * Purpose:
 *      Converts hex string to 4-bit binary map
 * Parameters:
 *      str: single hex digit string
 *      bmp: 4 bit bitmap
 *
 * Returns:
 *     Nothing
 */

static
void convert_hex2bin(char str, unsigned char bmp[4]){
    char str_tmp[2];
    long num;
    str_tmp[0]=str; str_tmp[1]='\0';
    num = strtol(str_tmp, NULL, 16);

    switch(num){
        case 0:
                bmp[3]=0; bmp[2]=0; bmp[1]=0; bmp[0]=0;
                break;
        case 1:
                bmp[3]=0; bmp[2]=0; bmp[1]=0; bmp[0]=1;
                break;
        case 2:
                bmp[3]=0; bmp[2]=0; bmp[1]=1; bmp[0]=0;
                break;
        case 3:
                bmp[3]=0; bmp[2]=0; bmp[1]=1; bmp[0]=1;
                break;
        case 4:
                bmp[3]=0; bmp[2]=1; bmp[1]=0; bmp[0]=0;
                break;
        case 5:
                bmp[3]=0; bmp[2]=1; bmp[1]=0; bmp[0]=1;
                break;
        case 6:
                bmp[3]=0; bmp[2]=1; bmp[1]=1; bmp[0]=0;
                break;
        case 7:
                bmp[3]=0; bmp[2]=1; bmp[1]=1; bmp[0]=1;
                break;
        case 8:
                bmp[3]=1; bmp[2]=0; bmp[1]=0; bmp[0]=0;
                break;
        case 9:
                bmp[3]=1; bmp[2]=0; bmp[1]=0; bmp[0]=1;
                break;
        case 0xa:
                bmp[3]=1; bmp[2]=0; bmp[1]=1; bmp[0]=0;
                break;
        case 0xb:
                bmp[3]=1; bmp[2]=0; bmp[1]=1; bmp[0]=1;
                break;
        case 0xc:
                bmp[3]=1; bmp[2]=1; bmp[1]=0; bmp[0]=0;
                break;
        case 0xd:
                bmp[3]=1; bmp[2]=1; bmp[1]=0; bmp[0]=1;
                break;
        case 0xe:
                bmp[3]=1; bmp[2]=1; bmp[1]=1; bmp[0]=0;
                break;
        case 0xf:
                bmp[3]=1; bmp[2]=1; bmp[1]=1; bmp[0]=1;
                break;
        default:
                bmp[3]=0; bmp[2]=0; bmp[1]=0; bmp[0]=0;
                break;
    }
}


/*
 * Function:
 *      get_lanes
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
get_lanes(int unit, int num_subport, int *speed, int *lanes)
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
            case 100000 : lanes[i] = ((i==0) && (SOC_IS_TRIDENT2X(unit))) ?
                                                      10 : 4; break;
            default     : lanes[i] = 0; break;
        }
    }
}


static void
get_per_pipe_log_port_range(int unit, int pport,
              int *log_start, int *log_end, int *pipe) {

    if(SOC_IS_TOMAHAWK2(unit)) {
        *pipe = (pport - 1) / 64;
        *log_start = (*pipe) * 34;
        *log_end = (*pipe) * 34 + 34 - 1;
    } else {
        *pipe = 0;
        *log_start = 0;
        *log_end = SOC_MAX_NUM_PORTS;
    }
}

/*
 * Function:
 *      get_logical_port_no
 * Purpose:
 *      assign logical port number for physical port
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pport: physical port number
 *
 * Returns:
 *      logical port number
 */
static int
get_logical_port_no(int unit, int pport){
    soc_info_t *si;
    int l, lport;
    int lstart, lend, pipe;
    flexport_t *flexport_p = flexport_parray[unit];

    si = &SOC_INFO(unit);
    lport = -1;
    get_per_pipe_log_port_range(unit, pport, &lstart, &lend, &pipe);

  if(SOC_IS_TRIDENT3X(unit)) {
    lport = (pport > 64) ? (2 + pport) : (pport);
  } else {
    if(flexport_p->last_assigned_port[pipe] == -1) {
	    flexport_p->last_assigned_port[pipe] = lstart;
    }

    for (l = (flexport_p->last_assigned_port[pipe] + 1);
         l <= lend && l < SOC_MAX_NUM_PORTS; l++) {
        if(si->port_l2p_mapping[l] == -1) {
            flexport_p->last_assigned_port[pipe] = l;
            lport = l;
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "\nAssigned logical port")));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, " %0d to physical port %0d"),
                                 l, pport));
            break;
        }
    }
  }
  return lport;
}

static int
config_tsc_shim(int unit, int tsc_num, int num_subport, int *speed, int encap)
{
  int i, lane_speed = 0, vco_sel = 0, higig = 0, tsc_lane_mode = 0;
  int lanes[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  int update_tscf = 0;
  char buffer[TCP_CMD_BUF_LEN];
  flexport_t *flexport_p = flexport_parray[unit];

  get_lanes(unit, num_subport, speed, lanes);
  for(i = 0; i < num_subport; i++) {
    if(speed[i] > 0) {
      update_tscf = 1;
      switch (speed[i]) {
        case 100000:
            lane_speed = 12; vco_sel = 1; tsc_lane_mode =  1;
            update_tscf = 1;
            break;
        case  50000:
            lane_speed = 12; vco_sel = 1; tsc_lane_mode =  5;
            update_tscf = 1;
            break;
        case  40000:
            lane_speed = (lanes[i] == 4) ? 5 : 10;
            vco_sel = 0;
            tsc_lane_mode =  (lanes[i] == 4) ? 1 : 5;
            update_tscf = 1;
            break;
        case  25000:
            lane_speed = 12; vco_sel = 1; tsc_lane_mode = 15;
            update_tscf = 1;
             break;
        case  20000:
            lane_speed =  5; vco_sel = 0; tsc_lane_mode =  5;
            update_tscf = 1;
            break;
        case  10000:
            lane_speed =  5; vco_sel = 0; tsc_lane_mode = 15;
            update_tscf = 1;
            break;
        default:
            test_error(unit, "*ERROR: Speed %0dG is not supported\n",
                       speed[i] / 1000);
            break;
      }
      sal_memset(buffer, 0, TCP_CMD_BUF_LEN);
      sprintf(buffer, "config_tscf_lane TscNum %0d LaneNum %0d LaneSpeed %0d\n",
             tsc_num, i, lane_speed);
      sal_strcpy(flexport_p->tsc_cfg_buffer[flexport_p->tsc_cfg_ptr++], buffer);
      flexport_p->tsc_cfg_cmds = 1;
    }
  }

  higig = (encap == BCM_PORT_ENCAP_HIGIG2) ? 1 : 0;
  /* Now, correct the TSC lane mode if the port macro is configured */
  /* as a tri-port. */
  if ((speed[0] > 0) && (speed[0] == speed[1]) && (speed[0] != speed[2])) {
          tsc_lane_mode = 7;
  }
  if ((speed[2] > 0) && (speed[2] == speed[3]) && (speed[0] != speed[2])) {
          tsc_lane_mode = 13;
  }

  if (update_tscf) {
      sal_memset(buffer, 0, TCP_CMD_BUF_LEN);
      sprintf(buffer, "config_tscf TscNum %0d VcoSel %0d Higig %0d TscLaneMode %0d HddcNum %0d\n",
              tsc_num, vco_sel, higig, tsc_lane_mode, tsc_num);
      sal_strcpy(flexport_p->tsc_cfg_buffer[flexport_p->tsc_cfg_ptr++], buffer);
      flexport_p->tsc_cfg_cmds = 1;
  }

  for (i = 0; i < 12; i++) {
      if (flexport_p->tsc_cfg_buffer[i][0] != '\0') {
          if (BCM_E_NONE != sendDataToSocket(flexport_p->sockfd,
                                flexport_p->tsc_cfg_buffer[i])) {
              return BCM_E_INTERNAL;
          }
          sal_memset(flexport_p->tsc_cfg_buffer[i], 0, TCP_CMD_BUF_LEN);
      }
  }
  flexport_p->tsc_cfg_cmds = 0;
  flexport_p->tsc_cfg_ptr = 0;

  return BCM_E_NONE;
}

/*
 * Function:
 *      flex_tsc
 * Purpose:
 *      Prepare speed_change_pbmp, port_down_pbmp, port_up_pbmp for one TSC
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
flex_tsc(int unit, int num_subport, int first_phy_port, int *speed, int encap)
{
    soc_info_t *si;
    flexport_t *flexport_p = flexport_parray[unit];
    int i;
    int cur_speed[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int cur_lanes[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int cur_encap = BCM_PORT_ENCAP_IEEE;
    int lanes[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int lanes_changed = 0;
    int exact_speed[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    bcm_port_t lport, pport;
    int rv;

    si = &SOC_INFO(unit);

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nFlexing port %0d..%0d to"),
             first_phy_port, first_phy_port + num_subport - 1));
    for (i = 0; i < num_subport; i++) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, " %0dG"), speed[i] / 1000));
    }
    if (encap == BCM_PORT_ENCAP_IEEE) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, " Ethernet")));
    } else if (encap == BCM_PORT_ENCAP_HIGIG2) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, " Higig2")));
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

    get_lanes(unit, num_subport, cur_speed, cur_lanes);
    get_lanes(unit, num_subport, speed, lanes);
    for (i = 0; i < num_subport; i++) {
        if (lanes[i] != cur_lanes[i]) {
            lanes_changed = 1;
            break;
        }
    }

    if (lanes_changed == 0 && encap == cur_encap &&
        flexport_p->no_speed_change_param == 0) {
        for (i = 0; i < num_subport; i++) {
            exact_speed[i] = stream_get_exact_speed(speed[i], encap);
            if (exact_speed[i] != cur_speed[i]) {
                pport = first_phy_port + i;
                lport = si->port_p2l_mapping[pport];
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "\nChanging port %0d from %0d G to %0d G"),
                                            pport, cur_speed[i] / 1000,
                                            exact_speed[i] / 1000));
                rv = bcm_port_speed_set(unit, lport, exact_speed[i]);
                if (BCM_FAILURE(rv) && rv != BCM_E_PORT) {
                    cli_out("\nbcm_port_speed_set %0d %0d: %s",
                            lport, exact_speed[i], bcm_errmsg(rv));
                    return rv;
                }
                SOC_PBMP_PORT_ADD(flexport_p->speed_change_pbmp, lport);
            }
        }
        return BCM_E_NONE;
    }

    for (i = 0; i < num_subport; i++) {
        pport = first_phy_port + i;
        lport = si->port_p2l_mapping[pport];
        if (lport != -1) {
            exact_speed[i] = stream_get_exact_speed(speed[i], encap);
            if (cur_speed[i] > 0 &&
                (exact_speed[i] != cur_speed[i] || encap != cur_encap ||
                 lanes[i] != cur_lanes[i])) {
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "\nRemoving %0d G port %0d"),
                                     cur_speed[i] / 1000, pport));
                bcm_port_resource_t_init(
                    &flexport_p->del_resource[flexport_p->num_del]);
                flexport_p->del_resource[flexport_p->num_del].physical_port = -1;
                flexport_p->del_resource[flexport_p->num_del].port = lport;
                flexport_p->num_del++;
                SOC_PBMP_PORT_ADD(flexport_p->port_down_pbmp, lport);
            }
        }
    }

    for (i = 0; i < num_subport; i++) {
        exact_speed[i] = stream_get_exact_speed(speed[i], encap);
        if (speed[i] > 0 &&
            (exact_speed[i] != cur_speed[i] || encap != cur_encap ||
             lanes[i] != cur_lanes[i])) {
            pport = first_phy_port + i;
            lport = get_logical_port_no(unit, pport);
            if(lport == -1) {
                cli_out("\nERROR : Unable to find valid logical port for"
                                              " physical port %0d", pport);
                return BCM_E_INTERNAL;
            }
            bcm_port_resource_t_init(
                &flexport_p->add_resource[flexport_p->num_add]);
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "\nAdding %0d G port %0d lane %0d"),
                                 exact_speed[i] / 1000, pport, lanes[i]));
            flexport_p->add_resource[flexport_p->num_add].physical_port = pport;
            flexport_p->add_resource[flexport_p->num_add].port  = lport;
            flexport_p->add_resource[flexport_p->num_add].speed = exact_speed[i];
            flexport_p->add_resource[flexport_p->num_add].lanes = lanes[i];
            flexport_p->add_resource[flexport_p->num_add].encap = encap;
            flexport_p->num_add++;
            SOC_PBMP_PORT_ADD(flexport_p->port_up_pbmp, lport);
        }
    }
    flexport_p->flex_tsc_map[(first_phy_port - 1) / num_subport] = 1;

    return BCM_E_NONE;
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
    flexport_t *flexport_p = flexport_parray[unit];
    int i;
    bcm_port_t lport;
    uint32 nports;
    bcm_port_resource_t *resource;
    bcm_error_t rv;

    nports = flexport_p->num_del + flexport_p->num_add;
    resource = (bcm_port_resource_t *) sal_alloc(nports *
                                                 sizeof(bcm_port_resource_t),
                                                 "flexport_resource_array");
    sal_memset(resource, 0, sizeof(bcm_port_resource_t));

    for (i = 0; i < flexport_p->num_del; i ++) {
        resource[i] = flexport_p->del_resource[i];
    }
    for (i = 0; i < flexport_p->num_add; i ++) {
        resource[flexport_p->num_del + i] = flexport_p->add_resource[i];
    }

    PBMP_ITER(flexport_p->port_down_pbmp, lport) {
        if (lport < SOC_MAX_NUM_PORTS) {
            rv = bcm_port_enable_set(unit, lport, 0);
            if (BCM_FAILURE(rv)) {
                cli_out("\nbcm_port_disable_set %0d: %s", lport, bcm_errmsg(rv));
                sal_free(resource);
                return rv;
            }
        }
    }

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nCalling bcm_port_resource_multi:")));
    for (i = 0; i < nports; i ++) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nresource[%0d]: physical_port = %0d"),
                             i, resource[i].physical_port));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nresource[%0d]: port  = %0d"),
                             i, resource[i].port));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nresource[%0d]: speed = %0d"),
                             i, resource[i].speed));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nresource[%0d]: lanes = %0d"),
                             i, resource[i].lanes));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nresource[%0d]: encap = %0d"),
                             i, resource[i].encap));
    }
    rv = bcm_port_resource_multi_set(unit, nports, resource);
    if (BCM_FAILURE(rv)) {
        cli_out("\nbcm_port_resource_multi_set: %s", bcm_errmsg(rv));
        sal_free(resource);
        return rv;
    }

    cli_out("\nWait 20s for TSC configuration changes to take effect\n");
    sal_sleep(20);

    PBMP_ITER(flexport_p->port_up_pbmp, lport) {
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
 *      flexport_by_config_file
 * Purpose:
 *      Load a BCM config file and flex accordingly
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      fname: filename of the bcm config file
 *
 * Returns:
 *     BCM_E_XXXX
 */

static bcm_error_t
flexport_by_config_file(int unit, char *fname)
{
    int port_speed;
    int *port_speeds;
    int *port_states;
    int traffic[MAX_TSC_NUM];
    char line[256], line_cpy1[256], line_cpy2[32], line_cpy3[32];
    char *port_str[32], *port_data[32], *l2p_str[16];
    char *split1 = NULL, *split2 = NULL, *split3 = NULL;
    const char *dic_portmap, *dic_pbmp_xe, *dic_pbmp_os, *dic_dev_bw;
    const char *dic_higig;
    int i, j, port_idx, l2p_idx, port_lgcID, port_data_idx, tmp_port, tmp_speed;
    int port_phyID, highest_lgcID = 0, clk_freq, lgc_2_phy_portmap[SOC_MAX_NUM_PORTS];
    int port_lanenum = 0;
    unsigned char os_bmp[SOC_MAX_NUM_PORTS], xe_bmp[SOC_MAX_NUM_PORTS],
                  bmp_4bit[4];
    long int hg_bmp=0;
    FILE *fp;
    soc_info_t *si;
    int p;
    char eth_hig_str[3], line_ovsb_str[4];
    int speed[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int encap;
    int num_subport;
    int rv = BCM_E_NONE;

    port_speeds = (int *) sal_alloc(sizeof(int) * SOC_MAX_NUM_PORTS,
                                    "port_speeds");
    port_states = (int *) sal_alloc(sizeof(int) * SOC_MAX_NUM_PORTS,
                                    "port_states");
    sal_memset(port_speeds, 0, sizeof(int) * SOC_MAX_NUM_PORTS);
    sal_memset(port_states, 0, sizeof(int) * SOC_MAX_NUM_PORTS);

    si = &SOC_INFO(unit);
    cli_out("\nCurrent bcm config:");
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        if (p < SOC_MAX_NUM_PORTS) {
            bcm_port_speed_get(unit, p, &port_speed);
            if (SOC_PBMP_MEMBER(PBMP_E_ALL(unit), p)) {
                sal_sprintf(eth_hig_str, "ETH");
            } else {
                sal_sprintf(eth_hig_str, "HG2");
            }
            if (SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), p)) {
                sal_sprintf(line_ovsb_str, "OVSB");
            } else {
                sal_sprintf(line_ovsb_str, "LINE");
            }
            cli_out("\nDev Port %3d, Phy Port %3d, %3d G %s %s",
                    p, si->port_l2p_mapping[p], port_speed / 1000,
                    eth_hig_str, line_ovsb_str);
        }
    }

    /* Open bcm config file */
    fp = sal_fopen(fname, "r");
    if (fp == NULL) {
        test_error(unit, "*ERROR: Cannot open bcm config file: '%s'\n", fname);
        rv = BCM_E_FAIL;
        goto done;
    } else {
        cli_out("\nLoading bcm config file: '%s'", fname);
    }

    /* Dictionary */
    dic_portmap = "portmap_";
    dic_pbmp_xe = "pbmp_xport_xe";
    dic_pbmp_os = "pbmp_oversubscribe";
    dic_dev_bw  = "device_bandwidth";
    dic_higig   = "bmp_higig";

    /* Initialize parameters */
    for (i=0; i < SOC_MAX_NUM_PORTS; i++) { port_states[i] = 0; }
    for (i=1; i < SOC_MAX_NUM_PORTS; i++) { port_speeds[i] = SPEED_UI_0; }
    port_speeds[0] = SPEED_UI_10G; port_speeds[129] = SPEED_UI_1G;
    for (i=0; i < MAX_TSC_NUM; i++) { traffic[i] = PM_ENCAP__ETHRNT; }
    for (i=0; i < SOC_MAX_NUM_PORTS; i++) { lgc_2_phy_portmap[i] = 9999; }
    for (i=0; i < SOC_MAX_NUM_PORTS; i++) { os_bmp[i] = 0; xe_bmp[i] = 0; }

    while (sal_fgets(line, sizeof(line), fp)) {
        if (sal_strstr(line, "#") != line) {
            if (sal_strlen(line) > 256) {
                sal_fclose(fp);
                rv = BCM_E_FAIL;
                goto done;
            }
            sal_strcpy(line_cpy1, line);
            split1 = sal_strtok(line_cpy1,"=");
            port_idx = 0;
            while (split1) {
                port_str[port_idx++] = split1;
                split1 = sal_strtok(NULL,"=");
            }
            tdm_chomp(port_str[1]);
            if (sal_strstr(line, dic_portmap)) {
                if (sal_strlen(port_str[1]) > 32) {
                    sal_fclose(fp);
                    rv = BCM_E_FAIL;
                    goto done;
                }
                sal_strcpy(line_cpy2, port_str[1]);
                split2 = sal_strtok(line_cpy2,":");
                port_data_idx = 0;
                while (split2) {
                    port_data[port_data_idx++] = split2;
                    split2 = sal_strtok(NULL,":");
                }
                tmp_port = sal_atoi(port_data[0]);
                tmp_speed = sal_atoi(port_data[1]);
                if (tmp_port<129 && tmp_port>0) {
                    port_speeds[tmp_port] = tmp_speed;
                    port_states[tmp_port-1] = 9;
                }
                if (sal_strlen(port_str[0]) > 32) {
                    sal_fclose(fp);
                    rv = BCM_E_FAIL;
                    goto done;
                }
                sal_strcpy(line_cpy3, port_str[0]);
                split3 = sal_strtok(line_cpy3,"_");
                l2p_idx = 0;
                while (split3) {
                    l2p_str[l2p_idx++] = split3;
                    split3 = sal_strtok(NULL,"_");
                }
                port_lgcID = sal_atoi(l2p_str[1]);
                if (port_lgcID > 0 && port_lgcID < SOC_MAX_NUM_PORTS) {
                    lgc_2_phy_portmap[port_lgcID] = tmp_port;
                    highest_lgcID = (port_lgcID>highest_lgcID) ?
                                    (port_lgcID) : (highest_lgcID);
                }
            }
            if (sal_strstr(line, dic_dev_bw)) {
                clk_freq = sal_atoi(port_str[1]);
                clk_freq /= 1000;
                /* Check clock frequency */
                if (clk_freq != si->frequency) {
                    test_error(unit, "*ERROR: Cannot change freq from %0d to %0d\n",
                                      si->frequency, clk_freq);
                }
            }
            if (sal_strstr(line, dic_pbmp_xe)) {
                if (sal_strlen(port_str[1]) > 32) {
                    sal_fclose(fp);
                    rv = BCM_E_FAIL;
                    goto done;
                }
                sal_strcpy(line_cpy2, port_str[1]);
                split2 = sal_strtok(line_cpy2,"x");
                port_data_idx=0;
                while (split2) {
                    port_data[port_data_idx++] = split2;
                    split2 = sal_strtok(NULL,"x");
                }
                /* Potentially 128 bit, so cannot just strtol entire string */
                j = 0;
                for (i = (strlen(port_data[1])-1); i >= 0; i--) {
                    if ((j * 4 + 3) < SOC_MAX_NUM_PORTS) {
                        convert_hex2bin(port_data[1][i], bmp_4bit);
                        xe_bmp[j * 4    ] = bmp_4bit[0];
                        xe_bmp[j * 4 + 1] = bmp_4bit[1];
                        xe_bmp[j * 4 + 2] = bmp_4bit[2];
                        xe_bmp[j * 4 + 3] = bmp_4bit[3];
                        j++;
                    }
                }
            }
            if (sal_strstr(line, dic_pbmp_os)) {
                if (sal_strlen(port_str[1]) > 32) {
                    sal_fclose(fp);
                    rv = BCM_E_FAIL;
                    goto done;
                }
                sal_strcpy(line_cpy2, port_str[1]);
                split2 = sal_strtok(line_cpy2, "x");
                port_data_idx = 0;
                while (split2) {
                    port_data[port_data_idx++] = split2;
                    split2 = sal_strtok(NULL, "x");
                }
                /* Potentially 128 bit, so cannot just strtol entire string */
                j = 0;
                for (i = (strlen(port_data[1]) - 1); i >= 0; i--) {
                    if ((j * 4 + 3) < 128) {
                        convert_hex2bin(port_data[1][i], bmp_4bit);
                        os_bmp[j * 4    ] = bmp_4bit[0];
                        os_bmp[j * 4 + 1] = bmp_4bit[1];
                        os_bmp[j * 4 + 2] = bmp_4bit[2];
                        os_bmp[j * 4 + 3] = bmp_4bit[3];
                        j++;
                    }
                }
            }
            if (sal_strstr(line, dic_higig)) {
                if (sal_strlen(port_str[1]) > 32) {
                    sal_fclose(fp);
                    rv = BCM_E_FAIL;
                    goto done;
                }
                sal_strcpy(line_cpy2, port_str[1]);
                split2 = sal_strtok(line_cpy2, "x");
                port_data_idx = 0;
                while (split2) {
                    port_data[port_data_idx++] = split2;
                    split2 = sal_strtok(NULL, "x");
                }
                hg_bmp = strtol(port_data[1], NULL, 16);
            }
        }
    }

    /* Set port state: oversub, linerate, combined */
    for (i = 1; i < (SOC_MAX_NUM_PORTS - 1); i++) {
        port_phyID = lgc_2_phy_portmap[i];
        if (port_phyID > 0 && port_phyID < (SOC_MAX_NUM_PORTS - 1)) {
            if (port_states[port_phyID - 1] == 9 && xe_bmp[i] == 1) {
                /* linerate/oversub */
                port_states[port_phyID - 1] = (os_bmp[i] == 1) ?
                    (PORT_STATE__OVERSUB) : (PORT_STATE__LINERATE);

                /* Combined */
                if (SOC_IS_TRIDENT2X(unit)) {
                    switch (port_speeds[port_phyID]) {
                        case SPEED_UI_120G     : port_lanenum = 12; break;
                        case SPEED_UI_100G     : port_lanenum = 10; break;
                        case SPEED_UI_42G      :
                        case SPEED_UI_40G      : port_lanenum =  4; break;
                        case SPEED_UI_21G_DUAL :
                        case SPEED_UI_20G      : port_lanenum =  2; break;
                        case SPEED_UI_10G      :
                        case SPEED_UI_2p5G     :
                        case SPEED_UI_1G       : port_lanenum =  1; break;
                        default                : port_lanenum =  0; break;
                    }
                }
                if (SOC_IS_TOMAHAWK2(unit) || SOC_IS_TRIDENT3X(unit)) {
                    switch (port_speeds[port_phyID]) {
                        case SPEED_UI_100G     :
                        case SPEED_UI_40G      : port_lanenum =  4; break;
                        case SPEED_UI_50G      :
                        case SPEED_UI_41G_DUAL :
                        case SPEED_UI_20G      : port_lanenum =  2; break;
                        case SPEED_UI_25G      :
                        case SPEED_UI_10G      : port_lanenum =  1; break;
                        default                : port_lanenum =  0; break;
                    }
                }
                for (j = 1; j < port_lanenum; j++){
                    port_states[port_phyID + j - 1] = PORT_STATE__COMBINE;
                }
            }
        }
    }

    /* Set tsc type: higig, ethernet */
    if (hg_bmp > 0) {
        for (i = 0; i < TDMV_NUM_TSC(unit); i++) {
            if ((((hg_bmp >> i) & 0x1) == 0x1)) {
                traffic[i] = PM_ENCAP__HIGIG2;
            }
        }
    }

    /* Transfer port state to TDM-5 specification */
    for (i = 1; i < (SOC_MAX_NUM_PORTS - 7); i++) {
        j = (i - 1) / TDMV_NUM_TSC_LANES;
        if (j < TDMV_NUM_TSC(unit)) {
            if (traffic[j] == PM_ENCAP__HIGIG2) {
                switch (port_states[i - 1]) {
                case PORT_STATE__LINERATE:
                    port_states[i-1] = PORT_STATE__LINERATE_HG;
                    break;
                case PORT_STATE__OVERSUB:
                    port_states[i-1] = PORT_STATE__OVERSUB_HG;
                    break;
                default:
                    break;
                }
            }
        }
    }

    sal_fclose(fp);

    for (i = 1; i < (SOC_MAX_NUM_PORTS - 1); i++) {
        port_phyID = lgc_2_phy_portmap[i];
        if (port_phyID > 0 && port_phyID < (SOC_MAX_NUM_PORTS - 1)) {
            if ((port_states[port_phyID-1] == PORT_STATE__LINERATE_HG) ||
                (port_states[port_phyID-1] == PORT_STATE__OVERSUB_HG)) {
                switch (port_speeds[port_phyID]) {
                    case 120: port_speeds[port_phyID] = 127; break;
                    case 100: port_speeds[port_phyID] = 106; break;
                    case 50:  port_speeds[port_phyID] = 53;  break;
                    case 40:  port_speeds[port_phyID] = 42;  break;
                    case 25:  port_speeds[port_phyID] = 27;  break;
                    case 20:  port_speeds[port_phyID] = 21;  break;
                    case 10:  port_speeds[port_phyID] = 11;  break;
                    default: break;
                }
            }
            if ((port_states[port_phyID-1] == PORT_STATE__LINERATE) ||
                (port_states[port_phyID-1] == PORT_STATE__OVERSUB)) {
                sal_sprintf(eth_hig_str, "ETH");
            } else {
                sal_sprintf(eth_hig_str, "HG2");
            }
            if ((port_states[port_phyID-1] == PORT_STATE__OVERSUB_HG) ||
                (port_states[port_phyID-1] == PORT_STATE__OVERSUB)) {
                sal_sprintf(line_ovsb_str, "OVSB");
            } else {
                sal_sprintf(line_ovsb_str, "LINE");
            }
            cli_out("\nDev Port %3d, Phy Port %3d, %3d G %s %s",
                    i, port_phyID, port_speeds[port_phyID],
                    eth_hig_str, line_ovsb_str);
        }
    }

    for (i = 0; i < TDMV_NUM_TSC(unit); i++) {
        port_idx = i * TDMV_NUM_TSC_LANES + 1;
        num_subport = 4;
        if (SOC_IS_TRIDENT2X(unit) && port_speeds[port_idx] >= 100) {
            num_subport = 12;
            i += 2;
        }
        for (j = 0; j < num_subport; j++) {
            speed[j] = port_speeds[port_idx + j] * 1000;
        }
        if (traffic[i] == PM_ENCAP__HIGIG2) {
            encap = BCM_PORT_ENCAP_HIGIG2;
        } else {
            encap = BCM_PORT_ENCAP_IEEE;
        }

        rv = flex_tsc(unit, num_subport, port_idx, speed, encap);
        if (BCM_FAILURE(rv)) {
            cli_out("\nflex_tsc %0d: %s", port_idx, bcm_errmsg(rv));
            break;
        }
    }

done:
    sal_free(port_speeds);
    sal_free(port_states);
    return rv;
}

/*
 * Function:
 *      flexport_by_tsc
 * Purpose:
 *      Flex one TSC according to the command line inputs
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      tsc: TSC #
 *      mode: target port speed/mode
 *
 * Returns:
 *     BCM_E_XXXX
 */

static bcm_error_t
flexport_by_tsc(int unit, uint32 tsc, char *mode)
{
    int rv = BCM_E_NONE;
    int speed[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int num_subport = 4;
    int encap_enum = BCM_PORT_ENCAP_IEEE;

    cli_out("\nFlexing TSC %d to %s", tsc, mode);

    if (sal_strstr(mode, "4x0G") || sal_strstr(mode, "12x0G")) {
        cli_out("\nShutting down TSC %d", tsc);
    } else if (sal_strstr(mode, "1x100G") || sal_strstr(mode, "1x106G")) {
        speed[0] = 100000;
        if (SOC_IS_TRIDENT2X(unit)) {
            num_subport = 12;
        }
    } else if (SOC_IS_TRIDENT2X(unit) &&
               (sal_strstr(mode, "1x120G") || sal_strstr(mode, "1x126G"))) {
        speed[0] = 120000;
        num_subport = 12;
    } else if (SOC_IS_TRIDENT2X(unit) &&
               (sal_strstr(mode, "10x10G") || sal_strstr(mode, "10x11G"))) {
        speed[0] = 10000; speed[1] = 10000; speed[2] = 10000; speed[3] = 10000;
        speed[4] = 10000; speed[5] = 10000; speed[6] = 10000; speed[7] = 10000;
        speed[8] = 10000; speed[9] = 10000;
        num_subport = 12;
    } else if (SOC_IS_TRIDENT2X(unit) &&
               (sal_strstr(mode, "12x10G") || sal_strstr(mode, "12x11G"))) {
        speed[0] = 10000; speed[1] = 10000; speed[2] = 10000; speed[3] = 10000;
        speed[4] = 10000; speed[5] = 10000; speed[6] = 10000; speed[7] = 10000;
        speed[8] = 10000; speed[9] = 10000; speed[10] = 10000; speed[11] = 10000;
        num_subport = 12;
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
    } else if (SOC_IS_TRIDENT2X(unit) &&
               (sal_strstr(mode, "4x2.5G"))) {
        speed[0] = 2500; speed[1] = 2500; speed[2] = 2500; speed[3] = 2500;
    } else if (sal_strstr(mode, "4x1G")) {
        speed[0] = 1000; speed[1] = 1000; speed[2] = 1000; speed[3] = 1000;
    } else {
        test_error(unit, "*ERROR: Mode %s is not supported\n", mode);
    }

    if (sal_strstr(mode, "11G") || sal_strstr(mode, "21G") ||
        sal_strstr(mode, "27G") || sal_strstr(mode, "53G") ||
        sal_strstr(mode, "42G") || sal_strstr(mode, "106G") ||
        sal_strstr(mode, "127G")) {
        encap_enum = BCM_PORT_ENCAP_HIGIG2;
    } else {
        encap_enum = BCM_PORT_ENCAP_IEEE;
    }

    rv = flex_tsc(unit, num_subport, tsc * TDMV_NUM_TSC_LANES + 1, speed,
                  encap_enum);
    if (BCM_FAILURE(rv)) {
        cli_out("\nflex_tsc %0d: %s",
                tsc * TDMV_NUM_TSC_LANES + 1, bcm_errmsg(rv));
    }

    return rv;
}


/*
 * Function:
 *     lossless_flood_cnt
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
lossless_flood_cnt(int unit, int port)
{
    uint32 pkt_size, flood_cnt = 0;
    int param_flood_cnt, param_pkt_size;
    flexport_t *flexport_p = flexport_parray[unit];

    param_flood_cnt = flexport_p->flood_pkt_cnt_param;
    param_pkt_size = flexport_p->pkt_size_param;

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

static int
get_packet_size(int unit, int port)
{
    int pkt_size, flood_cnt, i;
    rate_calc_t *rate_calc_p = rate_calc_parray[unit];
    flexport_t *flexport_p = flexport_parray[unit];

    pkt_size = flexport_p->pkt_size_param;
    if(pkt_size == 0) {
            if (IS_HG_PORT(unit, port)) {
                pkt_size = HG2_WC_PKT_SIZE;
            } else {
                pkt_size = ENET_WC_PKT_SIZE;
            }
    }

    if (pkt_size == 1) {
        if (flexport_p->flood_pkt_cnt_param == 0) {
            flood_cnt = lossless_flood_cnt(unit, port);
        } else {
            flood_cnt = flexport_p->flood_pkt_cnt_param;
        }
        for(i = 0; i< flood_cnt; i++) {
            pkt_size += rate_calc_p->rand_pkt_sizes[port][i];
        }
        pkt_size = pkt_size / flood_cnt;
    }
    return pkt_size;
}

/*
 * Function:
 *      set_port_property_arrays
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
set_port_property_arrays(int unit, pbmp_t pbmp, int pkt_size_param,
                         int max_num_cells_param)
{
  int p, j, pkt_size;
  stream_rate_t *stream_rate_calc_p;
  rate_calc_t *rate_calc_p = rate_calc_parray[unit];
  flexport_t *flexport_p = flexport_parray[unit];

  stream_rate_calc_p = sal_alloc(sizeof(stream_rate_t), "streaming_rate");
  sal_memset(stream_rate_calc_p, 0, sizeof(stream_rate_t));

  stream_rate_calc_p->mode              = 0;
  stream_rate_calc_p->emulation_param   = flexport_p->emulation_param;
  stream_rate_calc_p->tolerance_lr      = flexport_p->rate_tolerance_lr_param;
  stream_rate_calc_p->tolerance_os      = flexport_p->rate_tolerance_ov_param;
  SOC_PBMP_CLEAR(stream_rate_calc_p->pbmp);
  PBMP_ITER(PBMP_E_ALL(unit), p) {
      if (p < SOC_MAX_NUM_PORTS) {
          stream_rate_calc_p->pkt_size          = get_packet_size(unit, p);
          SOC_PBMP_PORT_ADD(stream_rate_calc_p->pbmp, p);
      }
  }

  if (SOC_FAILURE(stream_calc_exp_rate_by_rx(unit, rate_calc_p->exp_rate, stream_rate_calc_p))) {
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
              } while (stream_get_pkt_cell_cnt(unit, pkt_size, p) > max_num_cells_param);
              rate_calc_p->rand_pkt_sizes[p][j] = pkt_size;
          }
      }
  }

  return BCM_E_NONE;
}

/*
 * Function:
 *      set_up_ports
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
set_up_ports(int unit, pbmp_t pbmp)
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
 *      set_up_streams
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
set_up_streams(int unit, pbmp_t pbmp, pbmp_t vlan_pbmp)
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
                vlan = VLAN + p;
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
 *     send_pkts
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
send_pkts(int unit, pbmp_t pbmp, uint32 pkt_size_param, uint32 pkt_seed)
{
	int port;
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
	uint32 pkt_size;
    stream_pkt_t *tx_pkt;
	flexport_t *flexport_p = flexport_parray[unit];
	rate_calc_t *rate_calc_p = rate_calc_parray[unit];

    tx_pkt = sal_alloc(sizeof(stream_pkt_t), "tx_pkt");
    sal_memset(tx_pkt, 0, sizeof(stream_pkt_t));

    cli_out("\n==================================================\n");
    cli_out("\nSending packets ...\n\n");
    PBMP_ITER(pbmp, port) {
        if (port < SOC_MAX_NUM_PORTS) {
            if (flexport_p->pkt_size_param == 0) {
                pkt_size = stream_get_wc_pkt_size(unit, IS_HG_PORT(unit, port));
            } else {
                pkt_size = flexport_p->pkt_size_param;
            }

            tx_pkt->port = port;
            tx_pkt->num_pkt = lossless_flood_cnt(unit, port);
            tx_pkt->pkt_seed = pkt_seed;
            tx_pkt->pkt_size = pkt_size;
    		cli_out("\nSending packets on port %0d size %0d num %0d.", port, pkt_size, tx_pkt->num_pkt);
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
 *      start_rate_measurement
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
start_rate_measurement(int unit, pbmp_t pbmp, uint32 emulation_param)
{
    rate_calc_t *rate_calc_p = rate_calc_parray[unit];

    return record_rate_information(unit, emulation_param, pbmp, rate_calc_p->stime,
                                 rate_calc_p->tpkt_start, rate_calc_p->tbyt_start);

}

/*
 * Function:
 *      check_rates
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
check_rates(int unit, pbmp_t pbmp, uint32 tolerance_lr,
             uint32 tolerance_os, uint32 emulation_param)
{
    uint64 *etime;
    uint64 *tpkt_end;
    uint64 *tbyt_end;
    uint64 *rate;
    rate_calc_t *rate_calc_p = rate_calc_parray[unit];
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
        test_error(unit, "********** RATE CHECK FAILED ***********\n");
    } else {
        cli_out("\n********** RATE CHECK PASSED ***********");
    }

    sal_free(etime);
    sal_free(tpkt_end);
    sal_free(tbyt_end);
    sal_free(rate);
    return rv;
}

/*
 * Function:
 *      flexport_test_init
 * Purpose:
 *      Test init function. Parse CLI params and do necessary init.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     SOC_E_XXXX
 */

int
flexport_test_init(int unit, args_t *a, void **pa)
{
    flexport_t *flexport_p;
    rate_calc_t *rate_calc_p;
    int i, tsc, sub_port, pport, lport;
    int encap;
    int num_subport = 4;
    int speed[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    flexport_p = flexport_parray[unit];
    flexport_p = sal_alloc(sizeof(flexport_t), "flexport_test");
    sal_memset(flexport_p, 0, sizeof(flexport_t));
    flexport_parray[unit] = flexport_p;

    rate_calc_p = rate_calc_parray[unit];
    rate_calc_p = sal_alloc(sizeof(rate_calc_t), "streaming_library");
    sal_memset(rate_calc_p, 0, sizeof(rate_calc_t));
    rate_calc_parray[unit] = rate_calc_p;

    cli_out("\nCalling flexport_test_init");
    flexport_parse_test_params(unit, a);

    flexport_p->test_fail = 0;

    if(flexport_p->emulation_param) {
        start_cmic_timesync(unit);
    }

    BCM_IF_ERROR_RETURN(soc_counter_status(unit, &flexport_p->counter_flags,
                                                 &flexport_p->counter_interval,
                                                 &flexport_p->counter_pbm));
    if (flexport_p->counter_interval > 0) {
        cli_out("\nDisabling counter collection");
        soc_counter_stop(unit);
    }
    BCM_IF_ERROR_RETURN(bcm_linkscan_enable_get(unit,
                                                &flexport_p->linkscan_enable));
    if (flexport_p->linkscan_enable) {
        cli_out("\nDisabling linkscan");
        BCM_IF_ERROR_RETURN(bcm_linkscan_enable_set(unit, 0));
    }

    if (flexport_p->emulation_param) {
        /* The server address and port number changes in each session.   */
        /* So, the image must be recompiled after changing the following */
        /* code before every session.                                    */
        flexport_p->sockfd = connectToSocket(flexport_p->socket_num_param,
                                             flexport_p->server_name_param);

        /* Initialize tsc_cfg_buffer*/
        for (i = 0; i < 12; i++) {
            sal_memset(flexport_p->tsc_cfg_buffer[i], 0, TCP_CMD_BUF_LEN);
        }
        flexport_p->tsc_cfg_cmds = 0;
        flexport_p->tsc_cfg_ptr = 0;

        /* Perform TSC config based on SOC_INFO*/
        if(flexport_p->init_tsc_shim) {
            for(tsc = 0; tsc < TDMV_NUM_TSC(unit); tsc++) {
                encap = BCM_PORT_ENCAP_IEEE;
	            for(sub_port = 0; sub_port < num_subport; sub_port++) {
	                pport = 1 + (tsc * 4) + sub_port;
	                lport = SOC_INFO(unit).port_p2l_mapping[pport];

                    speed[sub_port] = 0;
	                if(lport == -1) continue;

                    if(IS_HG_PORT(unit, lport)) {
                        encap = BCM_PORT_ENCAP_HIGIG2;
                    }

	                BCM_IF_ERROR_RETURN(bcm_port_speed_get(unit, lport, &speed[sub_port]));
                }
                config_tsc_shim(unit, tsc, num_subport, speed, encap);
            }
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      flexport_test
 * Purpose:
 *      Set up ports/streams and send packets.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     SOC_E_XXXX
 */

int
flexport_test(int unit, args_t *a, void *pa)
{
    int i, tsc, sub_port, pport, lport, port;
    pbmp_t stop_traffic_pbmp;
    pbmp_t start_traffic_pbmp;
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    flexport_t *flexport_p;
    stream_integrity_t *pkt_intg;
    uint64 val64;
    int encap;
    int speed[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int rv = BCM_E_NONE;

    flexport_p = flexport_parray[unit];

    if (flexport_p->bad_input) {
        goto done;
    }

    cli_out("\nCalling flexport_test");

    flexport_p->pkt_seed = sal_rand();
    stream_set_lpbk(unit, PBMP_PORT_ALL(unit), flexport_p->loopback_mode_param);
    stream_turn_off_cmic_mmu_bkp(unit);
    stream_turn_off_fc(unit, PBMP_PORT_ALL(unit));
    set_port_property_arrays(unit, PBMP_PORT_ALL(unit),
                             flexport_p->pkt_size_param,
                             flexport_p->max_num_cells_param);
    set_up_ports(unit, PBMP_PORT_ALL(unit));
    flexport_p->vlan_pbmp = set_up_streams(unit, PBMP_PORT_ALL(unit),
                                           flexport_p->vlan_pbmp);
    send_pkts(unit, PBMP_PORT_ALL(unit), flexport_p->pkt_size_param,
              flexport_p->pkt_seed);

    cli_out("\nWait 2s for traffic to stabilize");
    sal_sleep(2);
    start_rate_measurement(unit, PBMP_PORT_ALL(unit), flexport_p->emulation_param);
    cli_out("\nMeasuring Rate over a period of %0ds",
            flexport_p->rate_calc_interval_param);
    sal_sleep(flexport_p->rate_calc_interval_param);

    flexport_p->num_del = 0;
    flexport_p->num_add = 0;
    for(i = 0; i < SOC_MAX_NUM_PIPES; i++) {
        flexport_p->last_assigned_port[i] = -1;
    }
    SOC_PBMP_CLEAR(stop_traffic_pbmp);
    SOC_PBMP_CLEAR(start_traffic_pbmp);
    SOC_PBMP_CLEAR(flexport_p->port_down_pbmp);
    SOC_PBMP_CLEAR(flexport_p->port_up_pbmp);
    SOC_PBMP_CLEAR(flexport_p->speed_change_pbmp);
    if (flexport_p->flex_by_command_line) {
        for (i = 0; i < TDMV_NUM_TSC(unit); i++) {
            if (flexport_p->tsc_param[i] != NULL) {
                BCM_IF_ERROR_RETURN(flexport_by_tsc(unit, i,
                                                    flexport_p->tsc_param[i]));
            }
        }
    } else if (flexport_p->config_file_name != NULL){
        BCM_IF_ERROR_RETURN(flexport_by_config_file(unit,
                                                    flexport_p->config_file_name));
    }

    stop_traffic_pbmp = flexport_p->port_down_pbmp;
    BCM_PBMP_OR(stop_traffic_pbmp, flexport_p->speed_change_pbmp);
    if (BCM_PBMP_NOT_NULL(stop_traffic_pbmp)) {
        if (stream_chk_mib_counters(unit, stop_traffic_pbmp, 0) != BCM_E_NONE) {
            flexport_p->test_fail = 1;
        }

        if (check_rates(unit, stop_traffic_pbmp,
                       flexport_p->rate_tolerance_lr_param,
                       flexport_p->rate_tolerance_ov_param,
                       flexport_p->emulation_param) != BCM_E_NONE) {
            flexport_p->test_fail = 1;
        }

        if (flexport_p->check_packet_integrity_param != 0) {
            pkt_intg = sal_alloc(sizeof(stream_integrity_t), "pkt integrity");
            sal_memset(pkt_intg, 0, sizeof(stream_integrity_t));

            pkt_intg->rx_pbmp = stop_traffic_pbmp;
            PBMP_ITER(stop_traffic_pbmp, port) {
                if (port < SOC_MAX_NUM_PORTS) {
                    pkt_intg->port_pkt_seed[port] = flexport_p->pkt_seed;
                    pkt_intg->port_flood_cnt[port] = lossless_flood_cnt(unit, port);
                    pkt_intg->rx_vlan[port] = VLAN + port;
                    pkt_intg->tx_vlan[port] = VLAN + port;
                    sal_memcpy(pkt_intg->mac_da[port], mac_da, NUM_BYTES_MAC_ADDR);
                    sal_memcpy(pkt_intg->mac_sa[port], mac_sa, NUM_BYTES_MAC_ADDR);
                }
            }
            rv = stream_chk_pkt_integrity(unit, pkt_intg);
            if (rv != BCM_E_NONE) {
                flexport_p->test_fail = 1;
            }
            sal_free(pkt_intg);
        }
    }

    if (flexport_p->num_del + flexport_p->num_add > 0) {
        stream_set_lpbk(unit, flexport_p->port_down_pbmp, BCM_PORT_LOOPBACK_NONE);
        flexport_call_bcm_api(unit);
        stream_set_lpbk(unit, flexport_p->port_up_pbmp, flexport_p->loopback_mode_param);
    }

    if (flexport_p->emulation_param) {
        for(tsc = 0; tsc < TDMV_NUM_TSC(unit); tsc++) {
            if(flexport_p->flex_tsc_map[tsc] == 1) {
                encap = BCM_PORT_ENCAP_IEEE;
                for(sub_port = 0; sub_port < TDMV_NUM_TSC_LANES; sub_port++) {
                    pport = 1 + (tsc * 4) + sub_port;
                    lport = SOC_INFO(unit).port_p2l_mapping[pport];

                    speed[sub_port] = 0;
                    if(lport == -1) continue;

                    if(IS_HG_PORT(unit, lport)) {
                        encap = BCM_PORT_ENCAP_HIGIG2;
                    }

                    BCM_IF_ERROR_RETURN(bcm_port_speed_get(unit, lport, &speed[sub_port]));
                }
                config_tsc_shim(unit, tsc, TDMV_NUM_TSC_LANES, speed, encap);
            }
        }
    }

    start_traffic_pbmp = flexport_p->port_up_pbmp;
    BCM_PBMP_OR(start_traffic_pbmp, flexport_p->speed_change_pbmp);

    if (flexport_p->clear_new_port_counts) {
        COMPILER_64_ZERO(val64);
        BCM_IF_ERROR_RETURN(soc_counter_set_by_port(unit, start_traffic_pbmp, val64));
    }

    stream_turn_off_fc(unit, flexport_p->port_up_pbmp);
    set_port_property_arrays(unit, start_traffic_pbmp,
                             flexport_p->pkt_size_param,
                             flexport_p->max_num_cells_param);
    set_up_ports(unit, flexport_p->port_up_pbmp);
    flexport_p->vlan_pbmp = set_up_streams(unit, start_traffic_pbmp,
                                           flexport_p->vlan_pbmp);
    send_pkts(unit, start_traffic_pbmp, flexport_p->pkt_size_param,
              flexport_p->pkt_seed);


    cli_out("\nWait 2s for traffic to stabilize");
    sal_sleep(2);

    start_rate_measurement(unit, start_traffic_pbmp, flexport_p->emulation_param);

    cli_out("\nMeasuring Rate over a period of %0ds",
            flexport_p->rate_calc_interval_param);
    sal_sleep(flexport_p->rate_calc_interval_param);

    if (stream_chk_mib_counters(unit, PBMP_PORT_ALL(unit), 0) != BCM_E_NONE) {
        flexport_p->test_fail = 1;
    }
    if (check_rates(unit, PBMP_PORT_ALL(unit),
                   flexport_p->rate_tolerance_lr_param,
                   flexport_p->rate_tolerance_ov_param,
                   flexport_p->emulation_param) != BCM_E_NONE) {
        flexport_p->test_fail = 1;
    }
    if (flexport_p->check_packet_integrity_param != 0) {
        pkt_intg = sal_alloc(sizeof(stream_integrity_t), "pkt integrity");
        sal_memset(pkt_intg, 0, sizeof(stream_integrity_t));

        pkt_intg->rx_pbmp = PBMP_PORT_ALL(unit);
        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            if (port < SOC_MAX_NUM_PORTS) {
                pkt_intg->port_pkt_seed[port] = flexport_p->pkt_seed;
                pkt_intg->port_flood_cnt[port] = lossless_flood_cnt(unit, port);
                pkt_intg->rx_vlan[port] = VLAN + port;
                pkt_intg->tx_vlan[port] = VLAN + port;
                sal_memcpy(pkt_intg->mac_da[port], mac_da, NUM_BYTES_MAC_ADDR);
                sal_memcpy(pkt_intg->mac_sa[port], mac_sa, NUM_BYTES_MAC_ADDR);
            }
		}
        rv = stream_chk_pkt_integrity(unit, pkt_intg);
        if (rv != BCM_E_NONE) {
            flexport_p->test_fail = 1;
        }
        sal_free(pkt_intg);
    }

done:
    return rv;
}


/*
 * Function:
 *      flexport_test_cleanup
 * Purpose:
 *      Do test end checks and free all allocated memory.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     SOC_E_XXXX
 */

int
flexport_test_cleanup(int unit, void *pa)
{
    flexport_t *flexport_p;
    rate_calc_t *rate_calc_p;
    int rv = BCM_E_NONE;

    cli_out("\nCalling flexport_test_cleanup");

    flexport_p = flexport_parray[unit];

    if (flexport_p->emulation_param) {
        close(flexport_p->sockfd);
    }

    if (flexport_p->linkscan_enable) {
        cli_out("\nEnabling linkscan");
        BCM_IF_ERROR_RETURN(bcm_linkscan_mode_set_pbm(unit, PBMP_PORT_ALL(unit),
                                                      BCM_LINKSCAN_MODE_SW));
        BCM_IF_ERROR_RETURN(bcm_linkscan_enable_set(unit, 1));
    }
    if (flexport_p->counter_interval > 0) {
        cli_out("\nEnabling counter collection with interval %0d", flexport_p->counter_interval);
        BCM_IF_ERROR_RETURN(soc_counter_start(unit, flexport_p->counter_flags,
                                                 flexport_p->counter_interval,
                                                 PBMP_PORT_ALL(unit)));
    }

    if (flexport_p->bad_input == 1) {
        flexport_p->test_fail = 1;
    }

    if (flexport_p->test_fail == 1) {
        rv = BCM_E_FAIL;
    }

    rate_calc_p = rate_calc_parray[unit];

    sal_free(flexport_p);
    sal_free(rate_calc_p);
    bcm_vlan_init(unit);

    cli_out("\n");

    return rv;
}

#endif /*(BCM_ESW_SUPPORT) && \
           (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)) && \
           !defined(NO_SAL_APPL) && (!defined(__KERNEL__)) */
