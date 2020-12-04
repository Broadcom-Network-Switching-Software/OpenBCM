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
#include <bcm/stat.h>

#include "testlist.h"
#include "gen_pkt.h"

#if defined(BCM_TRIDENT3_SUPPORT)

#define TD3_CELL_SIZE 256
#define TD3_NUM_HPIPE_PER_PIPE 2
#define TD3_NUM_PIPE 2
#define TD3_NUM_PORTS_PER_HPIPE 32
#define NUM_BITS_PER_BYTE 8
#define TD3_FIRST_CELL_SIZE 192
#define TD3_PKT_SIZE_WC_ETHER 64
#define TD3_PKT_OH_ETHER 20

#define TD3_FREQ_1700MHZ 1700
#define TD3_FREQ_1625MHZ 1625
#define TD3_FREQ_1525MHZ 1525
#define TD3_FREQ_1425MHZ 1425
#define TD3_FREQ_1325MHZ 1325
#define TD3_FREQ_1275MHZ 1275
#define TD3_FREQ_1012MHZ 1012
#define TD3_FREQ_850MHZ 850
#define TD3_BW_1700MHZ 1142000
#define TD3_BW_1625MHZ 1092000
#define TD3_BW_1525MHZ 1024000
#define TD3_BW_1425MHZ 957000
#define TD3_BW_1325MHZ 890000
#define TD3_BW_1275MHZ 856000
#define TD3_BW_1012MHZ 680000
#define TD3_BW_850MHZ 571000

#define TD3_FREQ_1525MHZ_CAL_LEN 409
#define TD3_FREQ_1425MHZ_CAL_LEN 383
#define TD3_FREQ_1325MHZ_CAL_LEN 356
#define TD3_FREQ_1012MHZ_CAL_LEN 272
#define TD3_FREQ_850MHZ_CAL_LEN 228
#define TD3_ANCL_CELLS 12

#endif

#define MAC_DA {0x12,0x34,0x56,0x78,0x9a,0xbc}
#define MAC_SA {0xfe,0xdc,0xba,0x98,0x76,0x54}
#define TPID 0x8100
#define VLAN 0x0a00
#define MIN_PKT_SIZE 64
#define MTU 9216
#define MTU_CELL_CNT 45
#define SPEED_FACTOR_DEFAULT 1
#define PKT_SIZE_PARAM_DEFAULT 0
#define FLOOD_PKT_CNT_PARAM_DEFAULT 0
#define RATE_CALC_INTERVAL_PARAM_DEFAULT 10
#define RATE_TOLERANCE_LR_PARAM_DEFAULT 1
#define RATE_TOLERANCE_OV_PARAM_DEFAULT 3
#define CHECK_PACKET_INTEGRITY_PARAM_DEFAULT 1
#define NUM_LPORT_TAB_ENTRIES 256
#define NUM_PORT_TAB_ENTRIES 136
#define MISC_BW 10000
#define NUM_SUBP_OBM 4
#define MAX_NUM_CELLS_PARAM_DEFAULT 4

#ifdef BCM_TOMAHAWK3_SUPPORT
#define TARGET_CELL_COUNT 300
#else
#define TARGET_CELL_COUNT 80
#endif

#define TX_CHAN 0
#define RX_CHAN 1

#define HG2_WC_PKT_SIZE 64
#define ENET_WC_PKT_SIZE 145
#define ENET_IPG 12
#define ENET_PREAMBLE 8
#define HG2_IPG 8
#define HG2_HDR 12

#define CELL_SIZE 208
#define FIRST_CELL_ENET 144
#define FIRST_CELL_HG2 148
#define RXDMA_TIMEOUT 10000

#define NUM_BYTES_MAC_ADDR 6
#define NUM_BYTES_CRC 4

#define TH_FREQ_850 850
#define TH_FREQ_765 765
#define TH_FREQ_672 672
#define TH_FREQ_645 645
#define TH_FREQ_545 545
#define TH_BW_850 561000
#define TH_BW_765 504000
#define TH_BW_672 442000
#define TH_BW_645 423000
#define TH_BW_545 358000

#define TD2P_FREQ_793 793
#define TD2P_FREQ_635 635
#define TD2P_FREQ_529 529
#define TD2P_FREQ_421 421
#define TD2P_BW_793 500000
#define TD2P_BW_635 380000
#define TD2P_BW_529 340000
#define TD2P_BW_421 260000

#define TH2_FREQ_1700 1700
#define TH2_FREQ_1625 1625
#define TH2_FREQ_1525 1525
#define TH2_FREQ_1425 1425
#define TH2_FREQ_1325 1325
#define TH2_FREQ_1275 1275
#define TH2_FREQ_1225 1225
#define TH2_FREQ_1125 1125
#define TH2_FREQ_1050 1050
#define TH2_FREQ_950 950
#define TH2_FREQ_850 850

#define CMICM_MMU_BKP_OFF_THRESHOLD 0x20
#define CMICX_MMU_BKP_OFF_THRESHOLD 0xFF

#define DMA_CHAN_PER_CMC(unit) (SOC_VCHAN_NUM(unit) / SOC_CMCS_NUM(unit))

#if defined(BCM_ESW_SUPPORT) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))

typedef struct streaming_s {

    uint32 num_fp_ports;
    int *port_speed;
    int *port_list;
    uint32 *port_oversub;
    uint32 num_pipes;
    uint32 total_chip_bw;
    uint32 oversub_mode;
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
    uint32 speed_factor_param;
    uint32 bad_input;
    int test_fail;
    uint32 pkt_seed;
} streaming_t;

static streaming_t *streaming_parray[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *      streaming_parse_test_params
 * Purpose:
 *      Parse CLI parameters, create test structure and flag bad inputs.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     Nothing
 * Notes:
 *      streaming_p->bad_input set from here - tells test to crash out in case 
 *      CLI input combination is invalid.
 */

static void
streaming_parse_test_params(int unit, args_t *a)
{
    parse_table_t parse_table;
    streaming_t *streaming_p = streaming_parray[unit];

    const char tr501_test_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nDocumentation too long to be displayed with -pedantic compiler\n";
#else
    "Streaming test to check TDM table and programming. Each port loops traffic\n"
    "back to itself using port bridging and higig lookup for HG ports. Test test\n"
    "calculates expected rates based on port config and oversub ratio and checks\n"
    "against it.\n"
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

    streaming_p->bad_input = 0;
    streaming_p->oversub_mode = 0;
    streaming_p->pkt_size_param = PKT_SIZE_PARAM_DEFAULT;
    streaming_p->flood_pkt_cnt_param = FLOOD_PKT_CNT_PARAM_DEFAULT;
    streaming_p->rate_calc_interval_param = RATE_CALC_INTERVAL_PARAM_DEFAULT;
    streaming_p->rate_tolerance_lr_param = RATE_TOLERANCE_LR_PARAM_DEFAULT;
    streaming_p->rate_tolerance_ov_param = RATE_TOLERANCE_OV_PARAM_DEFAULT;
    streaming_p->check_packet_integrity_param
                            = CHECK_PACKET_INTEGRITY_PARAM_DEFAULT;
    streaming_p->max_num_cells_param = MAX_NUM_CELLS_PARAM_DEFAULT;
    streaming_p->speed_factor_param  = SPEED_FACTOR_DEFAULT;

    /*Parse CLI opts */

    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table, "PktSize", PQ_INT|PQ_DFL, 0,
                    &(streaming_p->pkt_size_param), NULL);
    parse_table_add(&parse_table, "FloodCnt", PQ_INT|PQ_DFL, 0,
                    &(streaming_p->flood_pkt_cnt_param), NULL);
    parse_table_add(&parse_table, "RateCalcInt", PQ_INT|PQ_DFL, 0,
                    &(streaming_p->rate_calc_interval_param), NULL);
    parse_table_add(&parse_table, "TolLr", PQ_INT|PQ_DFL, 0,
                    &(streaming_p->rate_tolerance_lr_param), NULL);
    parse_table_add(&parse_table, "TolOv", PQ_INT|PQ_DFL, 0,
                    &(streaming_p->rate_tolerance_ov_param), NULL);
    parse_table_add(&parse_table, "ChkPktInteg", PQ_INT|PQ_DFL, 0,
                    &(streaming_p->check_packet_integrity_param), NULL);
    parse_table_add(&parse_table, "MaxNumCells", PQ_INT|PQ_DFL, 0,
                    &(streaming_p->max_num_cells_param), NULL);
    parse_table_add(&parse_table, "SpeedFactor", PQ_INT|PQ_DFL, 0,
                    &(streaming_p->speed_factor_param), NULL);

    if (parse_arg_eq(a, &parse_table) < 0 || ARG_CNT(a) != 0) {
        test_msg("%s", tr501_test_usage);
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        streaming_p->bad_input = 1;
        parse_arg_eq_done(&parse_table);
    } else {
        cli_out("\n ------------- PRINTING TEST PARAMS ------------------");
        cli_out("\npkt_size_param = %0d", streaming_p->pkt_size_param);
        cli_out("\nrate_calc_interval_param = %0d",
                streaming_p->rate_calc_interval_param);
        cli_out("\nflood_pkt_cnt_param = %0d", streaming_p->flood_pkt_cnt_param);
        cli_out("\nrate_tolerance_lr_param = %0d",
                streaming_p->rate_tolerance_lr_param);
        cli_out("\nrate_tolerance_ov_param = %0d",
                streaming_p->rate_tolerance_ov_param);
        cli_out("\ncheck_packet_integrity_param = %0d",
                streaming_p->check_packet_integrity_param);
        cli_out("\nmax_num_cells_param = %0d", streaming_p->max_num_cells_param);
        cli_out("\nspeed_factor_param = %0d",streaming_p->speed_factor_param);
        cli_out("\n -----------------------------------------------------");
    }

    if (streaming_p->max_num_cells_param == 0) {
        /* coverity[dont_call : FALSE] */
        streaming_p->max_num_cells_param = (sal_rand() % (MTU_CELL_CNT - 1)) + 1;
    }

    if (streaming_p->pkt_size_param == 0) {
        cli_out
            ("\nUsing worst case packet sizes - 145B for Ethernet "
             "and 76B (64B + 12B hg-hdr) for HG2");
    } else if (streaming_p->pkt_size_param == 1) {
        cli_out("\nUsing random packet sizes");
    } else if (streaming_p->pkt_size_param < MIN_PKT_SIZE) {
        test_error(unit,"\n*ERROR: Packet size cannot be lower than %0dB\n",
                MIN_PKT_SIZE);
        streaming_p->bad_input = 1;
    } else if (streaming_p->pkt_size_param > MTU) {
        test_error(unit,"\n*ERROR: Packet size cannot be higher than %0dB (Device MTU)\n",
                MTU);
        streaming_p->bad_input = 1;
    }

    if (streaming_p->flood_pkt_cnt_param == 0) {
        cli_out("\nFloodCnt=0, test will automatically calculate number of"
                " packets to flood each port");
    }
}

/*
 * Function:
 *      streaming_soc_set_up_mac_lpbk
 * Purpose:
 *      Enable MAC loopback on all ports
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */


static void
streaming_soc_set_up_mac_lpbk(int unit)
{
    int p;

    cli_out("\nSetting up MAC loopbacks");
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        (void) bcm_port_loopback_set(unit, p, BCM_PORT_LOOPBACK_MAC);
    }
}

/*
 * Function:
 *      streaming_turn_off_cmic_mmu_bkp
 * Purpose:
 *      Turn off CMIC to MMU backpressure
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
streaming_turn_off_cmic_mmu_bkp(int unit)
{
    int cmc, ch;
    int vchan;
    uint32 threshold = CMICM_MMU_BKP_OFF_THRESHOLD;

    if (soc_feature(unit, soc_feature_cmicm)
            || soc_feature(unit, soc_feature_cmicd_v2)
            || soc_feature(unit, soc_feature_cmicd_v3)) {
        threshold = CMICM_MMU_BKP_OFF_THRESHOLD;
    } else if (soc_feature(unit, soc_feature_cmicx)) {
        threshold = CMICX_MMU_BKP_OFF_THRESHOLD;
    }

    for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < DMA_CHAN_PER_CMC(unit); ch++) {
            vchan = cmc * DMA_CHAN_PER_CMC(unit) + ch;
            soc_dma_chan_rxbuf_threshold_cfg(unit, vchan, threshold);
        }
    }

    if (soc_feature(unit, soc_feature_cmicx)) {
        for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
            soc_dma_cmc_rxbuf_threshold_cfg(unit, cmc, threshold);
        }
    }
}
static void
streaming_soc_turn_off_idb_fc(int unit, soc_reg_t *idb_fcc_regs, int a_size)
{
    int obm_subp;
    int idx;
    uint64 idb_fc;
    soc_field_t idb_flow_control_config_fields[] =
        { PORT_FC_ENf, LOSSLESS1_FC_ENf, LOSSLESS0_FC_ENf,
        LOSSLESS1_PRIORITY_PROFILEf, LOSSLESS0_PRIORITY_PROFILEf
    };
    uint32 idb_flow_control_config_values[] = { 0x0, 0x0, 0x0, 0xff, 0xff };

    cli_out("\nTurning off flow control at IDB/MMU");

    COMPILER_64_SET(idb_fc, 0x0, 0x0);

    if (SOC_REG_IS_VALID(unit, IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r)) {
        soc_reg_fields32_modify(unit, IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r,
                                REG_PORT_ANY, 5, idb_flow_control_config_fields,
                                idb_flow_control_config_values);
        (void) soc_reg_get(unit, IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r, 0, 0, &idb_fc);
    }

    for (obm_subp = 0; obm_subp < NUM_SUBP_OBM; obm_subp++) {
        for(idx = 0; idx < a_size; idx++) {
            if(SOC_REG_IS_VALID(unit, idb_fcc_regs[idx])) {
                (void) soc_reg_set(unit, idb_fcc_regs[idx], 0, obm_subp, idb_fc);
            }
        }
    }
}
/*
 * Function:
 *      streaming_turn_off_fc
 * Purpose:
 *      Turn off flow control at the MAC, IDB and MMU.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
streaming_soc_turn_off_fc(int unit)
{
    int p;
    int idx;
    int a_size = 0;
    soc_reg_t idb_fcc_regs[] = {
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM8_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM9_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM10_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM11_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM12_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM13_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM14_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM15_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM8_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM9_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM10_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM11_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM12_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM13_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM14_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM15_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM8_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM9_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM10_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM11_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM12_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM13_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM14_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM15_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM8_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM9_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM10_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM11_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM12_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM13_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM14_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM15_FLOW_CONTROL_CONFIG_PIPE3r
    };
#ifdef BCM_TRIDENT3_SUPPORT
    soc_reg_t idb_fcc_regs_td3[] = {
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM8_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM9_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM10_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM11_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM12_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM13_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM14_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM15_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM8_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM9_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM10_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM11_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM12_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM13_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM14_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM15_FLOW_CONTROL_CONFIG_PIPE1r
    };
#endif
    soc_reg_t pgw_fcc_regs[] = {
        PGW_OBM_PORT0_FC_CONFIGr,
        PGW_OBM_PORT1_FC_CONFIGr,
        PGW_OBM_PORT2_FC_CONFIGr,
        PGW_OBM_PORT3_FC_CONFIGr,
        PGW_OBM_PORT4_FC_CONFIGr,
        PGW_OBM_PORT5_FC_CONFIGr,
        PGW_OBM_PORT6_FC_CONFIGr,
        PGW_OBM_PORT7_FC_CONFIGr,
        PGW_OBM_PORT8_FC_CONFIGr,
        PGW_OBM_PORT9_FC_CONFIGr,
        PGW_OBM_PORT10_FC_CONFIGr,
        PGW_OBM_PORT11_FC_CONFIGr,
        PGW_OBM_PORT12_FC_CONFIGr,
        PGW_OBM_PORT13_FC_CONFIGr,
        PGW_OBM_PORT14_FC_CONFIGr,
        PGW_OBM_PORT15_FC_CONFIGr
    };


    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        bcm_port_pause_set(unit, p, FALSE, FALSE);
        soc_reg_field32_modify(unit, THDI_INPUT_PORT_XON_ENABLESr, p,
                               PORT_PAUSE_ENABLEf, 0x0);
    }
    soc_reg_field32_modify(unit, THDI_INPUT_PORT_XON_ENABLESr, 0,
                           PORT_PAUSE_ENABLEf, 0x0);

#ifdef BCM_TRIDENT3_SUPPORT
    if(SOC_IS_TRIDENT3X(unit))
    {
        a_size = sizeof(idb_fcc_regs_td3) / sizeof(soc_reg_t);
        streaming_soc_turn_off_idb_fc(unit, idb_fcc_regs_td3, a_size);
    }
#endif
    if(SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT2PLUS(unit))
    {
        a_size = sizeof(idb_fcc_regs) / sizeof(soc_reg_t);
        streaming_soc_turn_off_idb_fc(unit, idb_fcc_regs, a_size);
        for (idx = 0; idx < (sizeof(pgw_fcc_regs) / sizeof(soc_reg_t)); idx++) {
            if (SOC_REG_IS_VALID(unit, pgw_fcc_regs[idx])) {
                soc_reg_field32_modify(unit, pgw_fcc_regs[idx], REG_PORT_ANY,
                                   PORT_FC_ENABLEf, 0x0);
            }
        }
    }
}

/*
 * Function:
 *      stream_get_reg_tpkt_tbyt
 * Purpose:
 *      Get register of TPKTr and TBYTr
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      reg_tpkt: ptr to register TPKTr
 *      reg_tbyt: ptr to register TBYTr
 *
 * Returns:
 *    BCM_E_XXX
 */
static bcm_error_t
stream_get_reg_tpkt_tbyt(int unit, int port, soc_reg_t* reg_tpkt,
                         soc_reg_t* reg_tbyt)
{
    bcm_error_t rv = BCM_E_NONE;

    if (soc_feature(unit, soc_feature_cxl_mib)) {
        if (SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit,
            SOC_INFO(unit).port_l2p_mapping[port]), SOC_BLK_CLPORT)) {
            (*reg_tpkt) = CLMIB_TPKTr;
            (*reg_tbyt) = CLMIB_TBYTr;
        } else if (IS_QSGMII_PORT(unit, port) ||
                   IS_EGPHY_PORT(unit, port)) {
            (*reg_tpkt) = GTPKTr;
            (*reg_tbyt) = GTBYTr;
        } else {
            (*reg_tpkt) = XLMIB_TPKTr;
            (*reg_tbyt) = XLMIB_TBYTr;
        }
    } else {
        (*reg_tpkt) = TPKTr;
        (*reg_tbyt) = TBYTr;
    }

    return rv;
}
/*
 * Function:
 *      get_pipe
 * Purpose:
 *      Get pipe number for a given device port.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      port: Device port #
 *
 * Returns:
 *     Pipe number.
 */

static int
get_pipe(int unit, int port)
{
    int pipe;
    soc_info_t *si = &SOC_INFO(unit);

    for (pipe = 0; pipe < si->num_pipe; pipe++) {
        if (SOC_PBMP_MEMBER(si->pipe_pbm[pipe], port)) {
            break;
        }
    }
    return pipe;
}

/*
 * Function:
 *      num_cells
 * Purpose:
 *      Get the cell count for a given packet size.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      port: Device port #
 *
 * Returns:
 *     Cell count
 */

static uint32
num_cells(int unit, uint32 pkt_size, int port)
{
    uint32 i;
    uint32 num_cells = 1;


    if (IS_HG_PORT(unit, port)) {
        i = FIRST_CELL_HG2;
    }
    else {
        i = FIRST_CELL_ENET;
#ifdef BCM_TRIDENT3_SUPPORT
        if(SOC_IS_TRIDENT3X(unit)) {
            i = TD3_FIRST_CELL_SIZE;
        }
#endif

    }
    while (i < pkt_size) {
        num_cells++;
#ifdef BCM_TRIDENT3_SUPPORT
        if(SOC_IS_TRIDENT3X(unit))
            i += TD3_CELL_SIZE;
#endif
        if (SOC_IS_TOMAHAWK3(unit)) {
            i += 254;
        } else if(SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT2PLUS(unit))
            i += CELL_SIZE;
    }
    return num_cells;
}
/*
 * Function:
 *      lossless_flood_cnt
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
lossless_flood_cnt(int unit, uint32 pkt_size, int port)
{
    uint32 flood_cnt = 0;
    uint32 total_cells = 0;

    streaming_t *streaming_p = streaming_parray[unit];

    if (pkt_size == 1) {
        while (total_cells < TARGET_CELL_COUNT) {
            total_cells += num_cells(unit,
                                     streaming_p->rand_pkt_sizes[port][flood_cnt],
                                     streaming_p->port_list[port]);
            flood_cnt++;
        }
    } else {
        flood_cnt = TARGET_CELL_COUNT / (num_cells(unit, pkt_size,
                                               streaming_p->port_list[port]));
    }

    if (flood_cnt < 3) {
        flood_cnt = 3;
    }
    return(flood_cnt);
}

/*
 * Function:
 *      safe_pkt_size
 * Purpose:
 *      Calculate safe packet size based on oversub ratio. The assumption is that
 *      a front panel port will behave essentially as a line rate port and show no
 *      bandwidth degradation for packet sizes above the safe packet size.
 *      This is based on input from device microarchitects. Broadcom internal
 *      users of this test can contact microarchitecture for more info.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      ovs_ratio_x1000: Oversub ratio x1000
 *
 * Returns:
 *     Safe packet size.
 */

static uint32
safe_pkt_size(int unit, int ovs_ratio_x1000)
{
    uint32 safe_size = 64;

    if (ovs_ratio_x1000 <= 1507) {
        safe_size =
            145 +
            ((((229000 - 144000) / (1507 - 1000)) * (ovs_ratio_x1000 -
                                                     1000)) / 1000);
    } else if ((ovs_ratio_x1000 > 1507) && (ovs_ratio_x1000 <= 1760)) {
        safe_size =
            353 +
            ((((416000 - 353000) / (1760 - 1508)) * (ovs_ratio_x1000 -
                                                     1508)) / 1000);
    } else if ((ovs_ratio_x1000 > 1760) && (ovs_ratio_x1000 <= 1912)) {
        safe_size =
            562 +
            ((((611000 - 562000) / (1912 - 1760)) * (ovs_ratio_x1000 -
                                                     1760)) / 1000);
    } else {
        safe_size = 770;
    }

    return safe_size;

}

/*
 * Function:
 *      calc_oversub_ratio
 * Purpose:
 *      Calculate oversub ratio for each pipe based on port config
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *    Nothing
 */


static void
calc_oversub_ratio(int unit)
{
    int i;
    int pipe;
    int pipe_bandwidth = 0;
    int lr_bandwidth = 0;
    int ov_bandwidth = 0;
    uint16 dev_id;
    uint8 rev_id;
    soc_info_t *si = &SOC_INFO(unit);
    streaming_t *streaming_p = streaming_parray[unit];
    int cal_universal;

    streaming_p->ovs_ratio_x1000 =
        (uint32 *) sal_alloc(si->num_pipe * sizeof(uint32), "ovs_ratio");

    cli_out("\nCalculating oversub ratios");
    cli_out("\nOperating Freq = %0d", si->frequency);

    soc_cm_get_id(unit, &dev_id, &rev_id);
    cal_universal = si->fabric_port_enable ? 1 : 0;

    if (dev_id == BCM56970_DEVICE_ID) {
        switch (si->frequency) {
        case TH2_FREQ_1700:
            pipe_bandwidth = cal_universal ? 1050 : 1095;
            break;
        case TH2_FREQ_1625:
            pipe_bandwidth = cal_universal ? 1002 : 1047;
            break;
        case TH2_FREQ_1525:
            pipe_bandwidth = cal_universal ? 940 : 980;
            break;
        case TH2_FREQ_1425:
            pipe_bandwidth = cal_universal ? 877 : 915;
            break;
        case TH2_FREQ_1325:
            pipe_bandwidth = cal_universal ? 812 : 847;
            break;
        case TH2_FREQ_1275:
            pipe_bandwidth = cal_universal ? 782 : 815;
            break;
        case TH2_FREQ_1225:
            pipe_bandwidth = cal_universal ? 750 : 782;
            break;
        case TH2_FREQ_1125:
            pipe_bandwidth = cal_universal ? 687 : 717;
            break;
        case TH2_FREQ_1050:
            pipe_bandwidth = cal_universal ? 640 : 667;
            break;
        case TH2_FREQ_950:
            pipe_bandwidth = cal_universal ? 575 : 600;
            break;
        case TH2_FREQ_850:
            pipe_bandwidth = cal_universal ? 512 : 535;
            break;
        default:
            pipe_bandwidth = cal_universal ? 1050 : 1095;
        }

        pipe_bandwidth = pipe_bandwidth * 1000;
    }
    else if (dev_id == BCM56960_DEVICE_ID) {
        switch (si->frequency) {
        case TH_FREQ_850:
            pipe_bandwidth = TH_BW_850;
            break;
        case TH_FREQ_765:
            pipe_bandwidth = TH_BW_765;
            break;
        case TH_FREQ_672:
            pipe_bandwidth = TH_BW_672;
            break;
        case TH_FREQ_645:
            pipe_bandwidth = TH_BW_645;
            break;
        case TH_FREQ_545:
            pipe_bandwidth = TH_BW_545;
            break;
        default:
            pipe_bandwidth = TH_BW_850;
        }
    }
    else if (dev_id == BCM56860_DEVICE_ID || dev_id == BCM56850_DEVICE_ID) {
        switch (si->frequency) {
        case TD2P_FREQ_793:
            pipe_bandwidth = TD2P_BW_793;
            break;
        case TD2P_FREQ_635:
            pipe_bandwidth = TD2P_BW_635;
            break;
        case TD2P_FREQ_529:
            pipe_bandwidth = TD2P_BW_529;
            break;
        case TD2P_FREQ_421:
            pipe_bandwidth = TD2P_BW_421;
            break;
        default:
            pipe_bandwidth = TD2P_BW_793;
        }
    }
#ifdef BCM_TRIDENT3_SUPPORT
    else if (dev_id == BCM56870_DEVICE_ID)
    {
        switch (si->frequency) {
        case TD3_FREQ_1700MHZ:
            pipe_bandwidth = TD3_BW_1700MHZ;
            break;
        case TD3_FREQ_1625MHZ:
            pipe_bandwidth = TD3_BW_1625MHZ;
            break;
        case TD3_FREQ_1525MHZ:
            pipe_bandwidth = TD3_BW_1525MHZ;
            break;
        case TD3_FREQ_1425MHZ:
            pipe_bandwidth = TD3_BW_1425MHZ;
            break;
        case TD3_FREQ_1325MHZ:
            pipe_bandwidth = TD3_BW_1325MHZ;
            break;
        case TD3_FREQ_1275MHZ:
            pipe_bandwidth = TD3_BW_1275MHZ;
            break;
        case TD3_FREQ_1012MHZ:
            pipe_bandwidth = TD3_BW_1012MHZ;
            break;
        case TD3_FREQ_850MHZ:
            pipe_bandwidth = TD3_BW_850MHZ;
            break;

        }
    }
#endif
    else {
        pipe_bandwidth = TD2P_BW_793;
    }

    pipe_bandwidth = pipe_bandwidth - MISC_BW;

    cli_out("\nPer pipe BW = %0d", pipe_bandwidth);

    for (pipe = 0; pipe < si->num_pipe; pipe++) {
        lr_bandwidth = 0;
        ov_bandwidth = 0;
        for (i = 0; i < streaming_p->num_fp_ports; i++) {
            if (SOC_PBMP_MEMBER(si->pipe_pbm[pipe], streaming_p->port_list[i])) {
                if (streaming_p->port_oversub[i] == 1) {
                    ov_bandwidth += streaming_p->port_speed[i];
                } else {
                    lr_bandwidth += streaming_p->port_speed[i];
                }
            }
        }
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
        if(ov_bandwidth != 0)
            streaming_p->oversub_mode = 1;
#endif
        streaming_p->ovs_ratio_x1000[pipe] =
            (ov_bandwidth) / ((pipe_bandwidth - lr_bandwidth) / 1000);

        if (streaming_p->pkt_size_param >
            safe_pkt_size(unit, streaming_p->ovs_ratio_x1000[pipe])) {
            streaming_p->ovs_ratio_x1000[pipe] = 1000;
        }

        if (streaming_p->ovs_ratio_x1000[pipe] < 1000) {
            streaming_p->ovs_ratio_x1000[pipe] = 1000;
        }

        cli_out
            ("\nFor pipe %0d, LR BW = %0d, OV BW = %0d, ovs_ratio(x1000) = %0d",
             pipe, lr_bandwidth, ov_bandwidth,
             streaming_p->ovs_ratio_x1000[pipe]);
    }
}
#if defined(BCM_TRIDENT3_SUPPORT)
static int
get_cal_length(int core_clk)
{
    int calc_len;
    switch (core_clk) {
        case TD3_FREQ_1525MHZ:
            calc_len = TD3_FREQ_1525MHZ_CAL_LEN;
            break;
        case TD3_FREQ_1425MHZ:
            calc_len = TD3_FREQ_1425MHZ_CAL_LEN;
            break;
        case TD3_FREQ_1325MHZ:
            calc_len = TD3_FREQ_1325MHZ_CAL_LEN;
            break;
        case TD3_FREQ_1012MHZ:
            calc_len = TD3_FREQ_1012MHZ_CAL_LEN;
            break;
        case TD3_FREQ_850MHZ:
            calc_len = TD3_FREQ_850MHZ_CAL_LEN;
            break;
        default:
            calc_len = TD3_FREQ_1525MHZ_CAL_LEN;
            break;
    }
    return calc_len;
}

static int
get_packet_size(int unit, int port)
{
    int pkt_size, flood_cnt, i;
    streaming_t *streaming_p = streaming_parray[unit];
    if (streaming_p->pkt_size_param == 0) {
            if (IS_HG_PORT(unit, streaming_p->port_list[port])) {
                pkt_size = HG2_WC_PKT_SIZE;
            } else {
                pkt_size = ENET_WC_PKT_SIZE;
            }
    } else {
        pkt_size = streaming_p->pkt_size_param;
    }

    /* random packets */
    if (pkt_size == 1) {
        if (streaming_p->flood_pkt_cnt_param == 0) {
            flood_cnt = lossless_flood_cnt(unit, pkt_size, port);
        } else {
            flood_cnt = streaming_p->flood_pkt_cnt_param;
        }
        for(i = 0; i< flood_cnt; i++) {
            pkt_size += streaming_p->rand_pkt_sizes[port][i];
        }
        pkt_size = pkt_size / flood_cnt;
    }
    return pkt_size;
}
#endif
/*
 * Function:
 *      set_exp_rates
 * Purpose:
 *      Set expected rates array (streaming_p->exp_rate). This is based on port
 *      speed and oversub ratio (for oversubscribed ports).
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */


static void
set_exp_rates(int unit)
{
    int i;
    char exp_rate_str[32];
    uint64 ovs_ratio_x1000_64;
    streaming_t *streaming_p = streaming_parray[unit];
#ifdef BCM_TRIDENT3_SUPPORT
    int lgc_port, phy_port, pipe_id, hpipe_id;
    int pkt_size, cell_num;
    uint32 packet_rate, cell_rate;
    int core_clk, dpp_clk, calc_len, dpp_clk_ratio_x10,
        pkt_slot_ratio_x100, cell_slot_ratio_x100;
    soc_info_t *si = &SOC_INFO(unit);
    int hpipe_bw[TD3_NUM_PIPE][TD3_NUM_HPIPE_PER_PIPE] = {
            {0, 0},
            {0, 0}
    };
#endif
    cli_out("\nSetting expected rates");
    streaming_p->exp_rate =
        (uint64 *) sal_alloc(streaming_p->num_fp_ports * sizeof(uint64),
                             "exp_rate");
#ifdef BCM_TRIDENT3_SUPPORT
    if(SOC_IS_TRIDENT3X(unit)) {

        if(streaming_p->oversub_mode) {

            core_clk = si->frequency;
            calc_len = get_cal_length(core_clk);
            dpp_clk_ratio_x10 = (core_clk > TD3_FREQ_1012MHZ)? 15:10;
            dpp_clk = core_clk * 10 / dpp_clk_ratio_x10 / TD3_NUM_HPIPE_PER_PIPE;
            pkt_slot_ratio_x100 = (calc_len - dpp_clk_ratio_x10 * TD3_ANCL_CELLS / 10) * 100/ calc_len;
            cell_slot_ratio_x100 = (calc_len - TD3_ANCL_CELLS) * 100 / calc_len;
            for(i = 0; i < streaming_p->num_fp_ports; i++) {

                lgc_port = streaming_p->port_list[i];
                phy_port = si->port_l2p_mapping[lgc_port];
                pipe_id = si->port_pipe[lgc_port];
                hpipe_id = (phy_port/TD3_NUM_PORTS_PER_HPIPE) % TD3_NUM_HPIPE_PER_PIPE;
                hpipe_bw[pipe_id][hpipe_id] += streaming_p->port_speed[i] / 1000; /* unit Gbits/s */

            }
            for(i = 0; i < streaming_p->num_fp_ports; i++) {
                if (streaming_p->port_oversub[i] == 1 && streaming_p->port_speed[i] != 0) {

                    lgc_port = streaming_p->port_list[i];
                    phy_port = si->port_l2p_mapping[lgc_port];
                    pipe_id = si->port_pipe[lgc_port];
                    hpipe_id = (phy_port / TD3_NUM_PORTS_PER_HPIPE) % TD3_NUM_HPIPE_PER_PIPE;
                    pkt_size = get_packet_size(unit, i) + TD3_PKT_OH_ETHER;
                    cell_num = num_cells(unit, pkt_size, i);

                    packet_rate =  dpp_clk * pkt_slot_ratio_x100 * pkt_size / hpipe_bw[pipe_id][hpipe_id]
                                       * (streaming_p->port_speed[i] / 1000) / 100 * NUM_BITS_PER_BYTE;

                    packet_rate = packet_rate > streaming_p->port_speed[i] ? streaming_p->port_speed[i]: packet_rate;

                    cell_rate = (core_clk / TD3_NUM_HPIPE_PER_PIPE) *  cell_slot_ratio_x100
                                    * pkt_size / 100 / cell_num / hpipe_bw[pipe_id][hpipe_id]
                                        * (streaming_p->port_speed[i] / 1000) * NUM_BITS_PER_BYTE;
                    cell_rate = cell_rate > streaming_p->port_speed[i] ? streaming_p->port_speed[i]: cell_rate;

                    COMPILER_64_SET(streaming_p->exp_rate[i], 0,
                                packet_rate > cell_rate? cell_rate : packet_rate);
                    COMPILER_64_UMUL_32(streaming_p->exp_rate[i], 1000000);
                }
            }

        } else {
            for(i = 0; i < streaming_p->num_fp_ports; i++) {
                COMPILER_64_SET(streaming_p->exp_rate[i], 0,
                                streaming_p->port_speed[i]);
                COMPILER_64_UMUL_32(streaming_p->exp_rate[i], 1000000);
            }
        }

    }
#endif
    if(SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT2PLUS(unit)) {
        for (i = 0; i < streaming_p->num_fp_ports; i++) {
            if (streaming_p->port_oversub[i] == 1) {
                COMPILER_64_SET(streaming_p->exp_rate[i], 0,
                                streaming_p->port_speed[i]);
                COMPILER_64_UMUL_32(streaming_p->exp_rate[i], 1000000);
                COMPILER_64_SET(ovs_ratio_x1000_64, 0,
                                streaming_p->ovs_ratio_x1000[get_pipe(unit,
                                                            streaming_p->
                                                                port_list[i])]);
                COMPILER_64_UMUL_32(streaming_p->exp_rate[i], 1000);
                COMPILER_64_UDIV_64(streaming_p->exp_rate[i], ovs_ratio_x1000_64);
            } else {
                COMPILER_64_SET(streaming_p->exp_rate[i], 0,
                                streaming_p->port_speed[i]);
                COMPILER_64_UMUL_32(streaming_p->exp_rate[i], 1000000);
            }
        }
    }

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nPRINTING EXPECTED RATES")));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\n=======================")));
    for (i = 0; i < streaming_p->num_fp_ports; i++) {
        format_uint64_decimal(exp_rate_str, streaming_p->exp_rate[i], 0);
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nFor port %0d, exp_rate[%0d] = %s"), i,
                  i, exp_rate_str));
    }
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\n=======================")));
}

/*
 * Function:
 *      set_port_property_arrays
 * Purpose:
 *      Set port_list, port_speed and port_oversub arrays. Also call set_exp_rates
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Safe packet size.
 */

static void
set_port_property_arrays(int unit)
{
    int p;
    int i, j;
    uint32 pkt_size;
    soc_info_t *si = &SOC_INFO(unit);
    streaming_t *streaming_p = streaming_parray[unit];

    streaming_p->num_fp_ports = 0;

    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        streaming_p->num_fp_ports++;
    }

    streaming_p->port_list =
        (int *)sal_alloc(streaming_p->num_fp_ports * sizeof(uint32),
                         "port_list");
    streaming_p->port_speed =
        (int *)sal_alloc(streaming_p->num_fp_ports * sizeof(uint32),
                         "port_speed_array");
    streaming_p->port_oversub =
        (uint32 *) sal_alloc(streaming_p->num_fp_ports * sizeof(uint32),
                             "port_oversub_array");
    streaming_p->rand_pkt_sizes =
        (uint32 **) sal_alloc(streaming_p->num_fp_ports * sizeof(uint32 *),
                              "rand_pkt_sizes_array*");

    for (i = 0; i < streaming_p->num_fp_ports; i++) {
        streaming_p->rand_pkt_sizes[i] =
            (uint32 *) sal_alloc(TARGET_CELL_COUNT * sizeof(uint32),
                                "rand_pkt_sizes_array");
    }

    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        streaming_p->port_list[i] = p;
        i++;
    }

    for (i = 0; i < streaming_p->num_fp_ports; i++) {
        streaming_p->port_speed[i] =
            si->port_speed_max[streaming_p->port_list[i]];

        switch(streaming_p->port_speed[i]) {
            case 11000:
                streaming_p->port_speed[i] = 10600;
            break;
            case 27000:
                streaming_p->port_speed[i] = 26500;
            break;
            case 42000:
                streaming_p->port_speed[i] = 42400;
            break;
        }
    }

    for (i = 0; i < streaming_p->num_fp_ports; i++) {
        if (SOC_PBMP_MEMBER(si->oversub_pbm, streaming_p->port_list[i])
            &&
            (!(SOC_PBMP_MEMBER(si->management_pbm, streaming_p->port_list[i]))))
        {
            streaming_p->port_oversub[i] = 1;
        } else {
            streaming_p->port_oversub[i] = 0;
        }
    }

    calc_oversub_ratio(unit);

    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\nPRINTING PORT SPEED")));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\n===============================")));

    for (i = 0; i < streaming_p->num_fp_ports; i++) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nFor port %0d, port_speed[%0d] = %0d"), i,
                  i, streaming_p->port_speed[i]));
    }

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\n===============================\n")));

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nPRINTING PORT OVERSUB ARRAY")));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\n===============================")));

    for (i = 0; i < streaming_p->num_fp_ports; i++) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nFor port %0d, port_oversub[%0d] = %0d"),
                  i, i, streaming_p->port_oversub[i]));
    }
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\n===============================\n")));


    for (i = 0; i < streaming_p->num_fp_ports; i++) {
        for (j = 0; j < TARGET_CELL_COUNT; j++) {
            do {
                /* coverity[dont_call : FALSE] */
                pkt_size = (sal_rand() % (MTU - MIN_PKT_SIZE + 1)) + MIN_PKT_SIZE;
            } while (num_cells(unit, pkt_size, streaming_p->port_list[i]) >
                            streaming_p->max_num_cells_param);
            streaming_p->rand_pkt_sizes[i][j] = pkt_size;
        }
    }

    set_exp_rates(unit);
}

/*
 * Function:
 *      set_up_streams
 * Purpose:
 *      VLAN programming for streaming. Each port is put on an unique VLAN.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      ovs_ratio_x1000: Oversub ratio x1000
 *
 * Returns:
 *     Safe packet size.
 */

static void
set_up_streams(int unit)
{
    int i;
    pbmp_t pbm, ubm;
    uint32 vlan = VLAN;
    streaming_t *streaming_p = streaming_parray[unit];


    BCM_PBMP_CLEAR(ubm);

    bcm_vlan_destroy_all(unit);

    for (i = 0; i < streaming_p->num_fp_ports; i++) {
        BCM_PBMP_CLEAR(pbm);
        bcm_vlan_create(unit, (bcm_vlan_t) (vlan));
        BCM_PBMP_PORT_ADD(pbm, streaming_p->port_list[i]);
        bcm_vlan_port_add(unit, (bcm_vlan_t) (vlan), pbm, ubm);
        vlan++;
    }
}



/*
 * Function:
 *      streaming_gen_random_l2_pkt
 * Purpose:
 *      Generate random L2 packet with seq ID
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

void
streaming_gen_random_l2_pkt(uint8 *pkt_ptr, uint32 pkt_size,
                  uint8 mac_da[NUM_BYTES_MAC_ADDR],
                  uint8 mac_sa[NUM_BYTES_MAC_ADDR], uint16 tpid,
                  uint16 vlan_id, uint32 seq_id)
{
    uint32 crc;
    tgp_gen_random_l2_pkt(pkt_ptr, pkt_size, mac_da, mac_sa, tpid, vlan_id);
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 6] = (seq_id >> 24) & 0xff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 7] = (seq_id >> 16) & 0xff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 8] = (seq_id >> 8) & 0xff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 9] = (seq_id) & 0xff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 10] = (pkt_size >> 8) & 0xff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 11] = (pkt_size) & 0xff;

    tgp_populate_crc_table();
    crc = tgp_generate_calculate_crc(pkt_ptr, pkt_size);

    pkt_ptr[pkt_size - NUM_BYTES_CRC + 3] = (crc >> 24) & 0xff;
    pkt_ptr[pkt_size - NUM_BYTES_CRC + 2] = (crc >> 16) & 0xff;
    pkt_ptr[pkt_size - NUM_BYTES_CRC + 1] = (crc >> 8) & 0xff;
    pkt_ptr[pkt_size - NUM_BYTES_CRC] = (crc) & 0xff;
}


/*
 * Function:
 *      send_pkts
 * Purpose:
 *      Send packets to flood VLANs and create a swirl on each port.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
send_pkts(int unit)
{
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    uint8 *packet_store_ptr;
    uint32 pkt_size;
    int i, j;
    pbmp_t lp_pbm, empty_pbm0, empty_pbm1;
    dv_t *dv_tx;
    int channel_done;
    int flags = 0;
    uint32 pkt_count = 0;
    uint32 flood_cnt;
    uint32 use_random_packet_sizes = 0;
    streaming_t *streaming_p = streaming_parray[unit];

    soc_dma_init(unit);

    dv_tx = soc_dma_dv_alloc(unit, DV_TX, 3);

    SOC_PBMP_CLEAR(lp_pbm);
    SOC_PBMP_PORT_ADD(lp_pbm, 1);
    SOC_PBMP_CLEAR(empty_pbm0);
    SOC_PBMP_CLEAR(empty_pbm1);

    cli_out("\nSending packets ...");

    for (i = 0; i < streaming_p->num_fp_ports; i++) {
        if (streaming_p->pkt_size_param == 0) {
            if (IS_HG_PORT(unit, streaming_p->port_list[i])) {
                pkt_size = HG2_WC_PKT_SIZE;
            } else {
                pkt_size = ENET_WC_PKT_SIZE;
            }
        } else {
            pkt_size = streaming_p->pkt_size_param;
        }

        if (streaming_p->flood_pkt_cnt_param == 0) {
            flood_cnt = lossless_flood_cnt(unit, pkt_size, i);
        } else {
            flood_cnt = streaming_p->flood_pkt_cnt_param;
        }

        if (pkt_size == 1) {
            use_random_packet_sizes = 1;
        }

        cli_out("\nflood_cnt for test port %0d = %0d", i, flood_cnt);

        for (j = 0; j < flood_cnt; j++) {
            if (use_random_packet_sizes == 1) {
                pkt_size = streaming_p->rand_pkt_sizes[i][j];
            }
            packet_store_ptr =
                sal_dma_alloc(pkt_size * sizeof(uint8), "packet");

            pkt_count++;
            channel_done = 0;
            soc_dma_abort_dv(unit, dv_tx);
            sal_srand(streaming_p->pkt_seed + i + j);
            streaming_gen_random_l2_pkt(packet_store_ptr, pkt_size, mac_da,
                                        mac_sa, TPID, (VLAN + i), j);
            soc_dma_dv_reset(DV_TX, dv_tx);
            soc_dma_desc_add(dv_tx, (sal_vaddr_t) (packet_store_ptr), pkt_size,
                             lp_pbm, empty_pbm0, empty_pbm1, flags, NULL);
            soc_dma_desc_end_packet(dv_tx);
            (void) soc_dma_chan_config(unit, TX_CHAN, DV_TX, SOC_DMA_F_POLL);
            soc_dma_start(unit, TX_CHAN, dv_tx);

            while (channel_done == 0) {
                soc_dma_chan_poll_done(unit, TX_CHAN,
                                             SOC_DMA_POLL_CHAIN_DONE,
                                             &channel_done);
            }
            sal_dma_free(packet_store_ptr);
        }
        cli_out("\n%0d Packets sent", pkt_count);
    }
    soc_dma_dv_free(unit, dv_tx);
}

/*
 * Function:
 *      get_rates
 * Purpose:
 *      Read packet counters in the MAC to measure rate over a given interval.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      rate_calc_int: Interval in seconds over which rate is measured
 *
 * Returns:
 *     Nothing
 */

static void
get_rates(int unit, uint32 rate_calc_int)
{
    int p;
    int i;
    uint32 ipg, preamble;
    uint64 rdata, tpkt, tbyt;
    uint64 tpkt_delta;
    uint64 tbyt_delta;
    uint64 rate_calc_int_64;
    soc_reg_t reg_tpkt = TPKTr, reg_tbyt = TBYTr;

    streaming_t *streaming_p = streaming_parray[unit];

    COMPILER_64_SET(rate_calc_int_64, 0, rate_calc_int);

    cli_out("\nCalculating Rates:");

    streaming_p->tpkt_start =
        (uint64 *) sal_alloc(streaming_p->num_fp_ports * sizeof(uint64),
                             "tpkt_start");
    streaming_p->tpkt_end =
        (uint64 *) sal_alloc(streaming_p->num_fp_ports * sizeof(uint64),
                             "tpkt_end");
    streaming_p->tbyt_start =
        (uint64 *) sal_alloc(streaming_p->num_fp_ports * sizeof(uint64),
                             "tbyt_start");
    streaming_p->tbyt_end =
        (uint64 *) sal_alloc(streaming_p->num_fp_ports * sizeof(uint64),
                             "tbyt_end");
    streaming_p->rate =
        (uint64 *) sal_alloc(streaming_p->num_fp_ports * sizeof(uint64),
                             "rate");

    cli_out("\nWait 2s for traffic to stabilize");
    sal_usleep(2000000);

    cli_out("\nMeasuring Rate over a period of %0ds", rate_calc_int);

    bcm_stat_sync(unit);

    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {

        if (SOC_IS_TOMAHAWK3(unit)) {
            bcm_stat_get(unit, p, snmpDot1dTpPortOutFrames, &tpkt);
            COMPILER_64_SET(streaming_p->tpkt_start[i], COMPILER_64_HI(tpkt),
                            COMPILER_64_LO(tpkt));
            i++;
        } else {
        if (stream_get_reg_tpkt_tbyt(unit, p, &reg_tpkt, &reg_tbyt) !=
            BCM_E_FAIL) {
            /* coverity[check_return : FALSE] */
            soc_reg_get(unit, reg_tpkt, p, 0, &rdata);
            COMPILER_64_SET(streaming_p->tpkt_start[i], COMPILER_64_HI(rdata),
                            COMPILER_64_LO(rdata));
            i++;
        }
    }
    }

    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        if (SOC_IS_TOMAHAWK3(unit)) {
            bcm_stat_get(unit, p, snmpIfOutOctets, &tbyt);
            COMPILER_64_SET(streaming_p->tbyt_start[i], COMPILER_64_HI(tbyt),
                            COMPILER_64_LO(tbyt));
            i++;
        } else {
        if (stream_get_reg_tpkt_tbyt(unit, p, &reg_tpkt, &reg_tbyt) !=
            BCM_E_FAIL) {
            /* coverity[check_return : FALSE] */
            soc_reg_get(unit, reg_tbyt, p, 0, &rdata);
            COMPILER_64_SET(streaming_p->tbyt_start[i], COMPILER_64_HI(rdata),
                            COMPILER_64_LO(rdata));
            i++;
        }
    }
    }

    sal_usleep(rate_calc_int * 1000000);

    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        if (SOC_IS_TOMAHAWK3(unit)) {
            bcm_stat_get(unit, p, snmpIfOutOctets, &tbyt);
            COMPILER_64_SET(streaming_p->tbyt_end[i], COMPILER_64_HI(tbyt),
                            COMPILER_64_LO(tbyt));
            i++;
        } else {
        if (stream_get_reg_tpkt_tbyt(unit, p, &reg_tpkt, &reg_tbyt) !=
            BCM_E_FAIL) {
            /* coverity[check_return : FALSE] */
            soc_reg_get(unit, reg_tbyt, p, 0, &rdata);
            COMPILER_64_SET(streaming_p->tbyt_end[i], COMPILER_64_HI(rdata),
                            COMPILER_64_LO(rdata));
            i++;
        }
    }
    }

    i = 0;
    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        if (SOC_IS_TOMAHAWK3(unit)) {
            bcm_stat_get(unit, p, snmpDot1dTpPortOutFrames, &tpkt);
            COMPILER_64_SET(streaming_p->tpkt_end[i], COMPILER_64_HI(tpkt),
                            COMPILER_64_LO(tpkt));
            i++;
        } else {
        if (stream_get_reg_tpkt_tbyt(unit, p, &reg_tpkt, &reg_tbyt) !=
            BCM_E_FAIL) {
            /* coverity[check_return : FALSE] */
            soc_reg_get(unit, reg_tpkt, p, 0, &rdata);
            COMPILER_64_SET(streaming_p->tpkt_end[i], COMPILER_64_HI(rdata),
                            COMPILER_64_LO(rdata));
            i++;
        }
    }
    }

    for (i = 0; i < streaming_p->num_fp_ports; i++) {
        if (IS_HG_PORT(unit, streaming_p->port_list[i])) {
            ipg = HG2_IPG;
            preamble = 0;
        } else {
            ipg = ENET_IPG;
            preamble = ENET_PREAMBLE;
        }
        COMPILER_64_DELTA(tbyt_delta, streaming_p->tbyt_start[i],
                          streaming_p->tbyt_end[i]);
        COMPILER_64_DELTA(tpkt_delta, streaming_p->tpkt_start[i],
                          streaming_p->tpkt_end[i]);
        COMPILER_64_UMUL_32(tpkt_delta, (ipg + preamble));
        COMPILER_64_ADD_64(tbyt_delta, tpkt_delta);
        COMPILER_64_UMUL_32(tbyt_delta, 8);
        COMPILER_64_SET(streaming_p->rate[i], COMPILER_64_HI(tbyt_delta),
                        COMPILER_64_LO(tbyt_delta));
        COMPILER_64_UDIV_64(streaming_p->rate[i], rate_calc_int_64);
    }
}

/*
 * Function:
 *      check_rates
 * Purpose:
 *      Check rates against expected rates.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     SOC_E_XXXX
 */

static void
check_rates(int unit)
{
    uint64 min_rate;
    uint64 max_rate;
    uint32 tolerance;
    uint64 margin_of_error;
    uint64 port_line_rate;
    uint64 hundred_64;
    int i;
    int fail = 0;
    char avg_rate_str[32], min_rate_str[32], max_rate_str[32];
    streaming_t *streaming_p = streaming_parray[unit];

    COMPILER_64_SET(hundred_64, 0, 100);

    for (i = 0; i < streaming_p->num_fp_ports; i++) {
        if (!(COMPILER_64_IS_ZERO(streaming_p->exp_rate[i]))) {
            if (streaming_p->port_oversub[i] == 1) {
                tolerance = streaming_p->rate_tolerance_ov_param;
            } else {
                tolerance = streaming_p->rate_tolerance_lr_param;
            }

            COMPILER_64_SET(margin_of_error,
                            COMPILER_64_HI(streaming_p->exp_rate[i]),
                            COMPILER_64_LO(streaming_p->exp_rate[i]));
            COMPILER_64_UMUL_32(margin_of_error, tolerance);
            COMPILER_64_UDIV_64(margin_of_error, hundred_64);

            COMPILER_64_DELTA(min_rate, margin_of_error,
                              streaming_p->exp_rate[i]);

            COMPILER_64_SET(port_line_rate, 0, streaming_p->port_speed[i]);
            COMPILER_64_UMUL_32(port_line_rate, 1000000);
            COMPILER_64_ZERO(max_rate);
            COMPILER_64_SET(margin_of_error,
                            COMPILER_64_HI(port_line_rate),
                            COMPILER_64_LO(port_line_rate));
            COMPILER_64_UMUL_32(margin_of_error, tolerance);
            COMPILER_64_UDIV_64(margin_of_error, hundred_64);
            COMPILER_64_SET(max_rate, COMPILER_64_HI(port_line_rate),
                            COMPILER_64_LO(port_line_rate));
            COMPILER_64_ADD_64(max_rate, margin_of_error);

            cli_out("\n");
            COMPILER_64_UMUL_32(streaming_p->rate[i], streaming_p->speed_factor_param);
            if ((COMPILER_64_LT(streaming_p->rate[i], min_rate))
                || (COMPILER_64_GT(streaming_p->rate[i], max_rate))) {
                cli_out("*ERROR: ");
                fail = 1;
            }
            format_uint64_decimal(avg_rate_str, streaming_p->rate[i], ',');
            format_uint64_decimal(min_rate_str, min_rate, ',');
            format_uint64_decimal(max_rate_str, max_rate, ',');
            cli_out
                ("Test Port = %0d, Device Port = %0d, Rate = %s, "
                 "min_rate = %s, max_rate = %s",
                 i, streaming_p->port_list[i],
                 avg_rate_str, min_rate_str, max_rate_str);
        }
    }
    if (fail == 1) {
        test_error(unit,
                   "\n*************** RATE CHECKS FAILED ***************\n");
        streaming_p->test_fail = 1;
    } else {
        cli_out("\n****************** RATE CHECKS PASSED ******************\n");
    }
}

/*
 * Function:
 *      set_up_ports
 * Purpose:
 *      Enable port bridging and HiGig lookup for HG2 ports
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
set_up_ports(int unit)
{
    int i;
    lport_tab_entry_t lport_tab_entry;
    port_tab_entry_t port_tab_entry;
    streaming_t *streaming_p = streaming_parray[unit];
    soc_field_t ihg_lookup_fields[] =
        { HYBRID_MODE_ENABLEf, USE_MH_PKT_PRIf, USE_MH_VIDf, HG_LOOKUP_ENABLEf,
        REMOVE_MH_SRC_PORTf
    };
    uint32 ihg_lookup_values[] = { 0x0, 0x1, 0x1, 0x1, 0x0 };

    if (!SOC_IS_TOMAHAWK3(unit)) {
        cli_out("\nEnabling HG_LOOKUP on HG ports");

        for (i = 0; i < streaming_p->num_fp_ports; i++) {
            if (IS_HG_PORT(unit, streaming_p->port_list[i])) {
                soc_reg_fields32_modify(unit, IHG_LOOKUPr,
                                        streaming_p->port_list[i], 5,
                                        ihg_lookup_fields, ihg_lookup_values);
            }
        }
    }

    cli_out("\nEnabling Port bridging");

    for (i = 0; i < soc_mem_index_max(unit, LPORT_TABm); i++) {
        (void) soc_mem_read(unit, LPORT_TABm, COPYNO_ALL, i,
                     lport_tab_entry.entry_data);
        if (soc_mem_field_valid(unit, LPORT_TABm, ALLOW_SRC_MODf)) {
        soc_mem_field32_set(unit, LPORT_TABm, lport_tab_entry.entry_data,
                            ALLOW_SRC_MODf, 0x1);
        }
        soc_mem_field32_set(unit, LPORT_TABm, lport_tab_entry.entry_data,
                            PORT_BRIDGEf, 0x1);
        soc_mem_write(unit, LPORT_TABm, COPYNO_ALL, i,
                      lport_tab_entry.entry_data);
    }

    for (i = 0; i < soc_mem_index_max(unit, PORT_TABm); i++) {
        (void) soc_mem_read(unit, PORT_TABm, COPYNO_ALL, i, port_tab_entry.entry_data);
        soc_mem_field32_set(unit, PORT_TABm, port_tab_entry.entry_data,
                            PORT_BRIDGEf, 0x1);
        soc_mem_write(unit, PORT_TABm, COPYNO_ALL, i,
                          port_tab_entry.entry_data);
    }
}

/*
 * Function:
 *      check_mib_counters
 * Purpose:
 *      Checks Error counters in MAC
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
check_mib_counters(int unit)
{
    uint32 i, j, column;
    uint64 rdata;
    int rv;
    int pblk;
    streaming_t *streaming_p = streaming_parray[unit];
    soc_reg_t error_counters[][4] = {
                                    {RFLRr,   XLMIB_RFLRr,   CLMIB_RFLRr,   GRFLRr},
                                    {RFCSr,   XLMIB_RFCSr,   CLMIB_RFCSr,   GRFCSr},
                                    {RJBRr,   XLMIB_RJBRr,   CLMIB_RJBRr,   GRJBRr},
                                    {RMTUEr,  XLMIB_RMTUEr,  CLMIB_RMTUEr,  GRMTUEr},
                                    {RTRFUr,  XLMIB_RTRFUr,  CLMIB_RTRFUr,  INVALIDr},
                                    {RERPKTr, XLMIB_RERPKTr, CLMIB_RERPKTr, GRCDEr},
                                    {RRPKTr,  XLMIB_RRPKTr,  CLMIB_RRPKTr,  GRRPKTr},
                                    {RUNDr,   XLMIB_RUNDr,   CLMIB_RUNDr,   GRUNDr},
                                    {RFRGr,   XLMIB_RFRGr,   CLMIB_RFRGr,   GRFRGr},
                                    {RRBYTr,  XLMIB_RRBYTr,  CLMIB_RRBYTr,  GRRBYTr},
                                    {TJBRr,   XLMIB_TJBRr,   CLMIB_TJBRr,   GTJBRr},
                                    {TFCSr,   XLMIB_TFCSr,   CLMIB_TFCSr,   GTFCSr},
                                    {TERRr,   XLMIB_TERRr,   CLMIB_TERRr,   GTXCLr},
                                    {TFRGr,   XLMIB_TFRGr,   CLMIB_TFRGr,   GTFRGr},
                                    {TRPKTr,  XLMIB_TRPKTr,  CLMIB_TRPKTr,  INVALIDr},
                                    {TUFLr,   XLMIB_TUFLr,   CLMIB_TUFLr,   INVALIDr},
                                    {TPCEr,   TPCE_64r,      TPCE_64r,      TPCE_64r},
                                    {RDISCr,  RDISC_64r,     RDISC_64r,     RDISC_64r},
                                    {RIPHE4r, RIPHE4_64r,    RIPHE4_64r,    RIPHE4_64r},
                                    {RIPHE6r, RIPHE6_64r,    RIPHE6_64r,    RIPHE6_64r},
                                    {RIPD4r,  RIPD4_64r,     RIPD4_64r,     RIPD4_64r},
                                    {RIPD6r,  RIPD6_64r,     RIPD6_64r,     RIPD6_64r},
                                    {RPORTDr, RPORTD_64r,    RPORTD_64r,    RPORTD_64r}
                                 };

    for (i = 0; i < streaming_p->num_fp_ports; i++) {
        column = 0;

        if (soc_feature(unit, soc_feature_cxl_mib)) {
            pblk = SOC_PORT_BLOCK(unit,
                   SOC_INFO(unit).port_l2p_mapping[streaming_p->port_list[i]]);
            if (SOC_BLOCK_IS_CMP(unit, pblk, SOC_BLK_CLPORT)) {
                column = 2;
            } else if (IS_QSGMII_PORT(unit, streaming_p->port_list[i]) ||
                       IS_EGPHY_PORT(unit, streaming_p->port_list[i])) {
                column = 3;
            } else if (SOC_BLOCK_IS_CMP(unit, pblk, SOC_BLK_XLPORT)) {
                column = 1;
            }
        }
        for (j = 0; j < (sizeof(error_counters) / sizeof(error_counters[0])); j++) {
            if (SOC_REG_IS_VALID(unit, error_counters[j][column])) {
                rv = soc_reg_get(unit, error_counters[j][column],
                                streaming_p->port_list[i], 0, &rdata);

                if (SOC_FAILURE(rv)) {
                    test_error(unit, "\nError reading MIB counter %s",
                                            SOC_REG_NAME(unit, error_counters[j][column]));
                    streaming_p->test_fail = 1;
                } else if (!(COMPILER_64_IS_ZERO(rdata))) {
                    test_error (unit, "\n*ERROR: Error counter %s has a non zero value "
                                "for device port %0d",
                                SOC_REG_NAME(unit, error_counters[j][column]),
                                streaming_p->port_list[i]);
                    streaming_p->test_fail = 1;
                }
            }
        }
    }
}

/*
 * Function:
 *      invalidate_vlan
 * Purpose:
 *      Invalidates VLAN in VLAN table
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      test_port - Test port number
 *
 * Returns:
 *     Nothing
 */

static void
invalidate_vlan(int unit, uint32 vlan)
{
    vlan_tab_entry_t vlan_tab_entry;

    (void) soc_mem_read(unit, VLAN_TABm, COPYNO_ALL, vlan, vlan_tab_entry.entry_data);
    soc_mem_field32_set(unit, VLAN_TABm, vlan_tab_entry.entry_data,
                        VALIDf, 0x0);
    soc_mem_write(unit, VLAN_TABm, COPYNO_ALL, vlan, vlan_tab_entry.entry_data);
}

/*
 * Function:
 *      check_packet_integrity
 * Purpose:
 *      Redirect all packets back to CPU and check packet integrity
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
check_packet_integrity(int unit)
{
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    uint8 *ref_pkt_ptr;
    uint8 *rx_pkt_ptr;
    uint32 pkt_size;
    int i, j, k;
    pbmp_t lp_pbm, empty_pbm0, empty_pbm1;
    dv_t *dv_rx;
    int channel_done;
    int flags = 0;
    uint32 pkt_count = 0;
    uint32 pkt_count_port = 0;
    uint32 flood_cnt;
    int timeout = RXDMA_TIMEOUT;
    uint32 match = 1;
    uint32 zero_crc = 1;
    uint32 seq_id;
    streaming_t *streaming_p = streaming_parray[unit];
    uint32 header_size = 0;

    dv_rx = soc_dma_dv_alloc(unit, DV_RX, 3);

    SOC_PBMP_CLEAR(lp_pbm);
    SOC_PBMP_PORT_ADD(lp_pbm, 1);
    SOC_PBMP_CLEAR(empty_pbm0);
    SOC_PBMP_CLEAR(empty_pbm1);

    cli_out("\nRouting all packets back to CPU to check pkt integrity ...");

    soc_dma_chan_cos_ctrl_set(unit, RX_CHAN, 1, 0xffffffff);
    soc_dma_chan_cos_ctrl_set(unit, RX_CHAN, 2, 0xffffffff);

    soc_dma_header_size_get(unit, &header_size);

    soc_reg_field32_modify(unit, CPU_CONTROL_0r, 0, UVLAN_TOCPUf, 0x1);

    for (i = 0; i < streaming_p->num_fp_ports; i++) {
        if (streaming_p->pkt_size_param == 0) {
            if (IS_HG_PORT(unit, streaming_p->port_list[i])) {
                pkt_size = HG2_WC_PKT_SIZE;
            } else {
                pkt_size = ENET_WC_PKT_SIZE;
            }
        } else {
            pkt_size = streaming_p->pkt_size_param;
        }

        if (streaming_p->flood_pkt_cnt_param == 0) {
            flood_cnt = lossless_flood_cnt(unit, pkt_size, i);
        } else {
            flood_cnt = streaming_p->flood_pkt_cnt_param;
        }
        for (j = 0; j < flood_cnt; j++) {
            ref_pkt_ptr =
                sal_dma_alloc(MTU * sizeof(uint8), "ref_packet");
            rx_pkt_ptr =
                sal_dma_alloc(MTU * sizeof(uint8) + header_size, "rx_pkt");

            for (k = 0; k < MTU + header_size; k++) {
                rx_pkt_ptr[k] = 0x00;
            }

            channel_done = 0;
            soc_dma_abort_dv(unit, dv_rx);

            soc_dma_dv_reset(DV_RX, dv_rx);
            soc_dma_desc_add(dv_rx, (sal_vaddr_t) (rx_pkt_ptr), MTU + 1,
                             lp_pbm, empty_pbm0, empty_pbm1, flags, NULL);
            soc_dma_desc_end_packet(dv_rx);
            (void) soc_dma_chan_config(unit, RX_CHAN, DV_RX, SOC_DMA_F_POLL);

            soc_dma_start(unit, RX_CHAN, dv_rx);
            invalidate_vlan(unit, (VLAN + i));

            while (channel_done == 0 && timeout > 0) {
                soc_dma_chan_poll_done(unit, RX_CHAN,
                                             SOC_DMA_POLL_CHAIN_DONE,
                                             &channel_done);
                timeout--;
            }

            if (timeout > 0) {
                pkt_count++;
                pkt_count_port++;
                seq_id = 0x00000000;
                pkt_size = 0x00000000;

                seq_id |= (rx_pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 6 + header_size] << 24);
                seq_id |= (rx_pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 7 + header_size] << 16);
                seq_id |= (rx_pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 8 + header_size] << 8);
                seq_id |= (rx_pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 9 + header_size]);
                pkt_size |= (rx_pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 10 + header_size] << 8);
                pkt_size |= (rx_pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 11 + header_size]);

                sal_srand(streaming_p->pkt_seed + i + seq_id);
                streaming_gen_random_l2_pkt(ref_pkt_ptr, pkt_size, mac_da,
                                            mac_sa, TPID, (VLAN + i), seq_id);

                for (k = 0; k < (pkt_size - NUM_BYTES_CRC); k++) {
                    if (rx_pkt_ptr[k + header_size] != ref_pkt_ptr[k]) {
                        match = 0;
                        test_error(unit, "\n*ERROR: Test port %0d, "
                                         "Device Port %0d has packet corruption"
                                         " on received pkt %0d",
                                          i, streaming_p->port_list[i], j);
                        streaming_p->test_fail = 1;
                        while(k < (pkt_size - NUM_BYTES_CRC))
                        {
                            cli_out("\n Expected byte: %d -- Received byte: %d",ref_pkt_ptr[k], rx_pkt_ptr[k + header_size]);
                            k++;
                        }
                        break;
                    }
                }

                for(k = 0; k < NUM_BYTES_CRC; k++) {
                    if (rx_pkt_ptr[header_size + pkt_size - NUM_BYTES_CRC + k] != 0) {
                        zero_crc = 0;
                    }
                }

                if (zero_crc == 1) {
                    match = 0;
                    test_error(unit, "\n*ERROR: Test port %0d, "
                                        "Device Port %0d has packet corruption"
                                        "(zero CRC) on received pkt %0d",
                                        i, streaming_p->port_list[i], j);
                    streaming_p->test_fail = 1;
                }

                zero_crc = 1;
            } else
            {
                cli_out("\n first packet was not sent back to CPU");
                timeout = RXDMA_TIMEOUT;
                sal_dma_free(rx_pkt_ptr);
                sal_dma_free(ref_pkt_ptr);
                break;
            }

            timeout = RXDMA_TIMEOUT;
            sal_dma_free(rx_pkt_ptr);
            sal_dma_free(ref_pkt_ptr);
        }
        cli_out("\n%0d Packets received", pkt_count);

        if (pkt_count_port < flood_cnt) {
            match = 0;
            test_error(unit, "\n*ERROR: Test port %0d, Device Port %0d"
                             " expected %0d pkts, received %0d packets",
                             i, streaming_p->port_list[i], flood_cnt,
                             pkt_count_port);
            streaming_p->test_fail = 1;
        }
        pkt_count_port = 0;
    }

    soc_dma_dv_free(unit, dv_rx);

    if (match == 1) {
        cli_out("\n***** PACKET INTEGRITY CHECKS PASSED *****\n");
    }
    else {
        test_error(unit, "\n***** PACKET INTEGRITY CHECKS FAILED *****\n");
    }
}

/*
 * Function:
 *      streaming_test_init
 * Purpose:
 *      Test init function. Parse CLI params and do necessary init.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */


int
streaming_test_init(int unit, args_t *a, void **pa)
{
    streaming_t *streaming_p;

    streaming_p = streaming_parray[unit];

    streaming_p = sal_alloc(sizeof(streaming_t), "streaming_test");
    sal_memset(streaming_p, 0, sizeof(streaming_t));
    streaming_parray[unit] = streaming_p;
    cli_out("\nCalling streaming_test_init");
    streaming_parse_test_params(unit, a);

    streaming_p->test_fail = 0;

    if (streaming_p->bad_input == 1) {
        goto done;
    }

    streaming_soc_set_up_mac_lpbk(unit);
    streaming_turn_off_cmic_mmu_bkp(unit);
    streaming_soc_turn_off_fc(unit);
    /* coverity[dont_call : FALSE] */
    streaming_p->pkt_seed = sal_rand();

done:
    return 0;
}

/*
 * Function:
 *      streaming_test
 * Purpose:
 *      Set up ports/streams and send packets.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */


int
streaming_test(int unit, args_t *a, void *pa)
{
    streaming_t *streaming_p;
    int rv = BCM_E_NONE;

    streaming_p = streaming_parray[unit];

    if (streaming_p->bad_input == 1) {
        goto done;
    }

    cli_out("\nCalling streaming_test");
    set_port_property_arrays(unit);
    set_up_ports(unit);
    set_up_streams(unit);
    send_pkts(unit);

    if (streaming_p->test_fail) {
        rv = BCM_E_FAIL;
    }

done:
    return rv;
}

/*
 * Function:
 *      streaming_test_cleanup
 * Purpose:
 *      Do test end checks and free all allocated memory.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

int
streaming_test_cleanup(int unit, void *pa)
{
    streaming_t *streaming_p;
    int rv;

    streaming_p = streaming_parray[unit];

    if (streaming_p->bad_input == 1) {
        goto done;
    }
    cli_out("\nCalling streaming_test_cleanup");

    get_rates(unit, streaming_p->rate_calc_interval_param);
    check_mib_counters(unit);
    check_rates(unit);

    if (streaming_p->check_packet_integrity_param != 0) {
        check_packet_integrity(unit);
    }

    sal_free(streaming_p->port_speed);
    sal_free(streaming_p->port_oversub);
    sal_free(streaming_p->ovs_ratio_x1000);
    sal_free(streaming_p->rate);
    sal_free(streaming_p->exp_rate);
    sal_free(streaming_p->tpkt_start);
    sal_free(streaming_p->tpkt_end);

done:
    if (streaming_p->bad_input == 1) {
        streaming_p->test_fail = 1;
    }

    if (streaming_p->test_fail == 1) {
        rv = BCM_E_FAIL;
    }
    else {
        rv = BCM_E_NONE;
    }

    sal_free(streaming_p);

    cli_out("\n");

    return rv;
}
#endif /* BCM_ESW_SUPPORT */
