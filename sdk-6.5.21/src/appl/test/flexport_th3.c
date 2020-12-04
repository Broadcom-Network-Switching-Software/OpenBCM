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
 * flex_tpl.txt &config_dict.txt files. Once the traffic reaches steady state, rate and packet
 * integrity are checked on the flex ports. Then, traffic will be stopped on the
 * flex ports before calling the BCM FlexPort API to perform the flexport
 * operation. Traffic on the non-flex ports will not be interrupted. The rate
 * calculation is done by dividing the transmit packet count changes and
 * transmit byte count changes over a programmable interval. The rates are
 * checked against expected rates based on port configuration and
 * oversubscription ratio. Packet integrity check is achieved by disabling the
 * VLAN so the packets go back to the CPU as unknown VLAN. The packets are
 * compared against expected packets to ensure packet integrity.
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
/*#include <soc/portmod/portmod.h>*/
/*#include <stdlib.h>*/
#include <sal/core/libc.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)

#include <soc/init/tomahawk3_flexport_defines.h>
#include <soc/tomahawk3.h>

#include "testlist.h"
#include "gen_pkt.h"
#include "streaming_lib.h"

#if defined(BCM_ESW_SUPPORT) && \
           (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)) && \
           !defined(NO_SAL_APPL) && (!defined(__KERNEL__))

typedef struct flex_entry {
    uint32 pm_num;
    uint32 subport;
    uint32 speed;
    uint32 num_lanes;
    uint32 fec;
} flex_entry_t;

typedef struct flexport_s {
    pbmp_t vlan_pbmp;
    pbmp_t port_down_pbmp;
    pbmp_t port_up_pbmp;
    uint32 pkt_size_param;
    uint32 flood_pkt_cnt_param;
    uint32 rate_calc_interval_param;
    uint32 rate_tolerance_lr_param;
    uint32 rate_tolerance_ov_param;
    uint32 check_packet_integrity_param;
    uint32 emulation_param;
    uint32 loopback_mode_param;
    uint32 no_speed_change_param;
    uint32 init_tsc_shim;
    char *tsc_param[65];
    uint32 num_del;
    uint32 num_add;
    bcm_port_resource_t del_resource[SOC_MAX_NUM_PORTS];
    bcm_port_resource_t add_resource[SOC_MAX_NUM_PORTS];
    int linkscan_enable;
    uint32 counter_flags;
    int counter_interval;
    pbmp_t counter_pbm;
    uint32 bad_input;
    uint32 test_fail;
    uint32 test_fail_iter[20];
    uint32 pkt_seed;
    int sockfd;
    int clear_new_port_counts;
    int fec_type_param;
    int input_mode_param;
    int flex_id_start_param;
    int flex_id_end_param;
    int flex_iter;
    /* TH3 fields */
    int num_flex_entries;
    flex_entry_t flex_entry[256];
    int per_port_pkt_size[SOC_MAX_NUM_PORTS];
    int per_port_pkt_cnt[SOC_MAX_NUM_PORTS];
    char *tpl_section;
    int cut_thru_en;
    int sku;
    int rand_pm_en;
    int rand_pm_shift;
} flexport_t;

typedef struct rate_calc_s {
    uint32 ovs_ratio_x1000[SOC_MAX_NUM_PIPES];
    uint32 rand_pkt_sizes[SOC_MAX_NUM_PORTS][TARGET_CELL_COUNT];
    uint64 stime[SOC_MAX_NUM_PORTS];
    uint64 exp_rate[SOC_MAX_NUM_PORTS];
    uint64 exp_max_rate[SOC_MAX_NUM_PORTS];
    uint64 tpkt_start[SOC_MAX_NUM_PORTS];
    uint64 tbyt_start[SOC_MAX_NUM_PORTS];
} rate_calc_t;

typedef enum flex_port_phy_fec_e {
    FLX_FEC_DEFAULT_REQUEST = -1,
    FLX_FEC_INVALID,
    FLX_FEC_NONE,
    FLX_FEC_BASE_R,
    FLX_FEC_RS_FEC,
    FLX_FEC_RS_544,
    FLX_FEC_RS_272,
    FLX_FEC_RS_206,
    FLX_FEC_RS_108,
    FLX_FEC_RS_545,
    FLX_FEC_RS_304,
    FLX_FEC_RS_544_2XN,
    FLX_FEC_RS_272_2XN,
    FLX_FEC_COUNT
} flex_port_phy_fec_t;


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
    "ServerName:   Name or IP address of the TSC clock server.\n"
    "              Required for emulation\n."
    "SocketNum:    Socket number of the TSC clock server.\n"
    "              Required for emulation\n."
    "MaxNumCells:  Maximum number of cells for random packet sizes.\n"
    "              Set to 0 for random cell sizes. (default is 4)\n"
    "LoopbackMode: Loopback mode. Set to 1 for MAC loopback, 2 for PHY loopback.\n"
    "              (default is MAC loopback)\n"
    "Tsc<num>:     TSC to be flexed and port mode to flex to, e.g. Tsc5=4x10G\n"
    "              The supported mode/speed in Ethernet encapsulation are:\n"
    "NoSpeedChg:   NOT used in TH3; Do not call the bcm_port_speed change. (default)\n"
    "FecType:      Fec Type \n"
    "InputMode:    Specifies how user enters flex info \n"
    "              0: input directly from command line; flex info passed per port\n"
    "              1: input using config_dict.txt in command line; flex info passed per PM/CD\n"
    "              2: input using FLEX_ID in flex_tpl.txt file\n"
    "              default is 2\n"
    "TplSec:       TPL Section\n"
    "FlexIdStart:  Initial FLEX_ID in TPL Section\n"
    "FlexIdEnd:    Final   FLEX_ID in TPL Section\n"
    "CutThruEn:    Enable Cut Thru while flexing\n"
    "Sku:          Chip SKU under test\n"
    "RandPmEn:     PM Shift Random Enable\n"
    ;
#endif

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

    flexport_p->pkt_size_param = 0;
    flexport_p->flood_pkt_cnt_param = 0;
    flexport_p->rate_calc_interval_param = 5;
    flexport_p->rate_tolerance_lr_param = 2;
    flexport_p->rate_tolerance_ov_param = 2;
    flexport_p->check_packet_integrity_param = 1;
    flexport_p->emulation_param = 0;
    flexport_p->loopback_mode_param = BCM_PORT_LOOPBACK_PHY;
    flexport_p->no_speed_change_param = 1;
    flexport_p->init_tsc_shim = 0;
    flexport_p->clear_new_port_counts = 0;
    flexport_p->fec_type_param = -1;
    flexport_p->input_mode_param = 3;
    flexport_p->flex_id_start_param = 0;
    flexport_p->flex_id_end_param = 0;
    flexport_p->cut_thru_en = 1;
    flexport_p->sku = 56980;
    flexport_p->rand_pm_en = 0;
    flexport_p->rand_pm_shift = 0;

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
    parse_table_add(&parse_table, "LoopbackMode", PQ_INT|PQ_DFL, 0,
                    &flexport_p->loopback_mode_param, NULL);
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
    parse_table_add(&parse_table, "FecType", PQ_INT|PQ_DFL, 0,
                    &flexport_p->fec_type_param, NULL);
    parse_table_add(&parse_table, "InputMode", PQ_INT|PQ_DFL, 0,
                    &flexport_p->input_mode_param, NULL);
    parse_table_add(&parse_table, "FlexIdStart", PQ_INT|PQ_DFL, 0,
                    &flexport_p->flex_id_start_param, NULL);
    parse_table_add(&parse_table, "FlexIdEnd", PQ_INT|PQ_DFL, 0,
                    &flexport_p->flex_id_end_param, NULL);
    parse_table_add(&parse_table, "TplSec", PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tpl_section, NULL);
    parse_table_add(&parse_table, "CutThruEn", PQ_INT|PQ_DFL, 0,
                    &flexport_p->cut_thru_en, NULL);
    parse_table_add(&parse_table, "Sku", PQ_INT|PQ_DFL, 0,
                    &flexport_p->sku, NULL);
    parse_table_add(&parse_table, "RandPmEn", PQ_INT|PQ_DFL, 0,
                    &flexport_p->rand_pm_en, NULL);

    for (i = 0; i < 64; i++) {
        char flx_str[20]="flx";
        char flx_num_str[10];

        /*flx_str = (char *) sal_alloc(sizeof(char) * 20, "flx_str");
        sal_strcpy(flx_str)*/

        /*cli_out("DB01 i=%0d flx_str= %s\n", i, flx_str);*/
        sal_itoa(flx_num_str, i, 10, 0, 0);
        /*cli_out("DB02 i=%0d flx_num_str= %s\n", i, flx_num_str);*/
        strcat(flx_str, flx_num_str);
        /*cli_out("DB03 i=%0d flx_str= %s\n", i, flx_str);*/
        parse_table_add(&parse_table, flx_str, PQ_STRING|PQ_DFL, 0,
                    &flexport_p->tsc_param[i], NULL);
        if (flexport_p->tsc_param[i] != NULL) {
            cli_out("\n DBG0 i=%0d  = %s", i, flexport_p->tsc_param[i]);
        }
    }

    if (parse_arg_eq(a, &parse_table) < 0 || ARG_CNT(a) != 0) {
        cli_out("%s", flexport_test_usage);
        test_error(unit, "\n*ERROR PARSING ARGS\n");
    }

    flexport_p->flex_iter = 0;
    flexport_p->num_flex_entries = 0;

    sal_srand( (int)sal_time() );
    flexport_p->rand_pm_shift = sal_rand() % _TH3_NUM_OF_TSCBH;

    cli_out("\n ------------- PRINTING TEST PARAMS ------------------");
    cli_out("\nPktSize      = %0d", flexport_p->pkt_size_param);
    cli_out("\nRateCalcInt  = %0d ms", flexport_p->rate_calc_interval_param * 100);
    cli_out("\nFloodCnt     = %0d", flexport_p->flood_pkt_cnt_param);
    cli_out("\nTolLr        = %0d", flexport_p->rate_tolerance_lr_param);
    cli_out("\nTolOv        = %0d", flexport_p->rate_tolerance_ov_param);
    cli_out("\nChkPktInteg  = %0d", flexport_p->check_packet_integrity_param);
    cli_out("\nEmulation    = %0d", flexport_p->emulation_param);
    cli_out("\nLoopbackMode = %0d", flexport_p->loopback_mode_param);
    cli_out("\nClrNewPortCounts   = %0d",  flexport_p->clear_new_port_counts);
    cli_out("\nFecType      = %0d",  flexport_p->fec_type_param);
    cli_out("\nInputMode    = %0d",  flexport_p->input_mode_param);
    cli_out("\nFlexIdStart  = %0d",  flexport_p->flex_id_start_param);
    cli_out("\nFlexIdEnd    = %0d",  flexport_p->flex_id_end_param);
    cli_out("\nCutThruEn    = %0d",  flexport_p->cut_thru_en);
    cli_out("\nSku          = %0d",  flexport_p->sku);
    cli_out("\nRandPmEn     = %0d",  flexport_p->rand_pm_en);
    if (flexport_p->rand_pm_en == 1) {
    cli_out("\nRandPM Shift = %0d",  flexport_p->rand_pm_shift);
    }

    if (flexport_p->tpl_section != NULL) {
        cli_out("\nTplSec       = %s",  flexport_p->tpl_section);
    }

    for (i = 0; i < 65; i++) {
       if (flexport_p->tsc_param[i] != NULL) {
           cli_out("\nTsc%2d       = %s", i, flexport_p->tsc_param[i]);
       }
    }

    cli_out("\nNoSpeeedChg   = %0d", flexport_p->no_speed_change_param);
    cli_out("\nInitTscShim   = %0d", flexport_p->init_tsc_shim);
    cli_out("\n -----------------------------------------------------");

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

    if ((flexport_p->input_mode_param < 0) ||
        (flexport_p->input_mode_param > 3) ) {
        test_error(unit,"*ERROR: Please specify a valid InputMode; Current one is InputMode=%0d\n",
                   flexport_p->input_mode_param);
        flexport_p->bad_input = 1;
    }

    flexport_p->test_fail = 0;
    for (i = 0; i < 20; i++) {
        flexport_p->test_fail_iter[i] = 0;
    }
}

static void get_mac_da(int unit,
    int port,
    bcm_mac_t mac_addr)
{
    int i;
    bcm_mac_t l_mac_addr = {0x12, 0x34, 0x56, 0x78, 0x9a, 0x00};

    for (i = 0; i < 6; i++) {
        mac_addr[i] = l_mac_addr[i];
    }
    mac_addr[5] = port;
}

static void get_mac_sa(int unit,
    int port,
    bcm_mac_t mac_addr)
{
    int i;
    bcm_mac_t l_mac_addr = {0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54};

    for (i = 0; i < 6; i++) {
        mac_addr[i] = l_mac_addr[i];
    }
}

static uint32 get_vlan_id(
    int unit,
    int port)
{
    return (0x0a00 + port);
}



static void
get_per_pipe_log_port_range(int unit, int pport,
              int *log_start, int *log_end, int *pipe) {

    if(SOC_IS_TOMAHAWK2(unit)) {
        *pipe = (pport - 1) / 64;
        *log_start = (*pipe) * 34;
        *log_end = (*pipe) * 34 + 34 - 1;
    } else if (SOC_IS_TOMAHAWK3(unit)) {
        /* Only front panel ports */
        *pipe = (pport - 1) / 32;
        *log_start = ((*pipe) == 0) ? 1 : (*pipe) * 20;
        *log_end = ((*pipe) == 0) ? 18 : 17 + ((*pipe) * 20);
    } else {
        *pipe = 0;
        *log_start = 0;
        *log_end = SOC_MAX_NUM_PORTS;
    }
}


/* Get BST counters
 * temp API until main bcm API is working
 */
static int th3_get_bst_total_queue(
    int unit,
    int port,
    int queue,
    int *count)
{
    uint32 pipe, itm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int index, is_mc, copyno;
    soc_mem_t mem;

    pipe = port / 20;
    itm = SOC_INFO(unit).ipipe_itm_map[pipe];
    itm -= 1;
    /*itm = ((pipe < 2) || (pipe > 5)) ? 0 : 1;*/
    /*for (i = 0; i < 8; i++) {
        cli_out("th3_get_bst_total_queue ipipe_itm_map[%0d]=%0d\n",
             i, SOC_INFO(unit).ipipe_itm_map[i]);
    }*/

    is_mc = 0;
    soc_th3_chip_queue_num_get(unit,
        port, queue, is_mc, &index);

    mem = (itm == 0) ?
        MMU_THDO_BST_TOTAL_QUEUE_ITM0m :
        MMU_THDO_BST_TOTAL_QUEUE_ITM1m;

    copyno = -1;
    sal_memset(&entry, 0, sizeof(entry));

    /*Read only from the 1st memory in list*/
    SOC_IF_ERROR_RETURN(soc_mem_read
        (unit, mem, copyno, index, entry));

    *count = entry[0];

    cli_out("th3_get_bst_total_queue port=%0d itm=%0d queue=%0d index=%0d BST_TOTAL_QUEUE=%0d\n",
             port, itm, queue, index, *count);

    return SOC_E_NONE;
}


static bcm_error_t th3_check_cdport_intr_status(
    int unit,
    int port)
{
    bcm_error_t rv = BCM_E_NONE;
    /*uint32 rval32;*/
    uint64 rval64;
    int phy_port;
    /*flexport_t *flexport_p = flexport_parray[unit];*/
    int fail = 0;
    soc_info_t *si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];

    /*
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
          CDPORT_INTR_STATUSr, phy_port, 0, &rval32));
    if (rval32 != 0) {
        cli_out("*ERROR th3_check_cdport_intr_status() port=%0d phy_port=%0d CDPORT_INTR_STATUSr=0x%x and !=0\n",
                port, phy_port, rval32);
        fail = 1;
    }
    */

    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit,
          CDPORT_TSC_INTR_STATUSr, phy_port, 0, &rval64));
    if (!COMPILER_64_IS_ZERO(rval64)) {
        cli_out("*ERROR th3_check_cdport_intr_status() port=%0d phy_port=%0d CDPORT_TSC_INTR_STATUSr= HI=0x%x LO=0x%x and !=0\n",
                port, phy_port, COMPILER_64_HI(rval64), COMPILER_64_LO(rval64));
        fail = 1;
    } else {
    /*  cli_out("th3_check_cdport_intr_status() port=%0d phy_port=%0d CDPORT_TSC_INTR_STATUSr= HI=0x%x LO=0x%x\n",
     *      port, phy_port, COMPILER_64_HI(rval64), COMPILER_64_LO(rval64));
     */
    }

    if (fail == 1) {
        /*flexport_p->test_fail = 1;
        flexport_p->test_fail_iter[flexport_p->flex_iter] = 1;*/
    }

    return rv;
}


/* NEW FRAMEWORK */
static bcm_error_t flexport_pre_traffic_checks(
    int unit,
    pbmp_t pbmp)
{
    bcm_error_t rv = BCM_E_NONE;
    int port;

    if (BCM_PBMP_NOT_NULL(pbmp)) {
        PBMP_ITER(pbmp, port) {
            if ((port < SOC_MAX_NUM_PORTS) &&
                (!IS_MANAGEMENT_PORT(unit,port))) {
                th3_check_cdport_intr_status(unit, port);
            }
        }
    }

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

    cli_out("flexport_call_bcm_api() Disabled ports DOWN");
    flexport_pre_traffic_checks(unit, PBMP_PORT_ALL(unit));

    for (i = 0; i < nports; i ++) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "resource[%0d]: physical_port = %0d\n"),
                             i, resource[i].physical_port));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "resource[%0d]: port  = %0d\n"),
                             i, resource[i].port));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "resource[%0d]: speed = %0d\n"),
                             i, resource[i].speed));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "resource[%0d]: lanes = %0d\n"),
                             i, resource[i].lanes));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "resource[%0d]: encap = %0d\n"),
                             i, resource[i].encap));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "resource[%0d]: fec_type = %0d\n\n"),
                             i, resource[i].fec_type));
        /*cli_out("\nresource[%0d]: physical_port =%0d port=%0d speed=%0dG lanes=%0d encap=%0d fec_type=%0d link_training=%0d\n",
                i, resource[i].physical_port, resource[i].port, resource[i].speed, 
                resource[i].lanes, resource[i].encap, resource[i].fec_type, resource[i].link_training);*/
    }

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "Calling bcm_port_resource_multi:\n")));
    rv = bcm_port_resource_multi_set(unit, nports, resource);
    if (BCM_FAILURE(rv)) {
        cli_out("\nbcm_port_resource_multi_set: %s", bcm_errmsg(rv));
        sal_free(resource);
        flexport_p->test_fail = 1;
        flexport_p->test_fail_iter[flexport_p->flex_iter] = 1;
        return rv;
    }

    cli_out("Wait 0.1s for TSC configuration changes to take effect\n");
    sal_usleep(100000);

    cli_out("flexport_call_bcm_api() Enabled ports UP");
    flexport_pre_traffic_checks(unit, PBMP_PORT_ALL(unit));


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


/* NEW FRAMEWORK */
static int flex_get_phy_port(
    int unit,
    flex_entry_t *flex_entry)
{
    if (SOC_IS_TOMAHAWK3(unit)){
        return (1 + (8 * flex_entry->pm_num) + flex_entry->subport);
    } else {
        return (1 + (4 * flex_entry->pm_num) + flex_entry->subport);
    }
}

/* NEW FRAMEWORK */
static int get_flex_entry(
    int unit,
    char *str,
    flex_entry_t *flex_entry,
    int start_indx)
{
    char str2[20]= "_";
    char c_temp[10];
    int entry_indx, i;
    int value;
    int fec_type;
    flexport_t *flexport_p = flexport_parray[unit];

    strcat(str, str2);
    /*cli_out("\n Original str=%s\n", str);*/

    fec_type = 0;
    entry_indx = start_indx;
    i = 0;
    /*while(*str && (*str != '\0')) {*/
    flex_entry->fec = 0;
    do {
        /*cli_out("test=%0d", *str);*/
        if ((*str != '_') && (*str !='\0')) {
            c_temp[i] = *str;
            i++;
        } else {
            c_temp[i] = '\0';
            value = sal_atoi(c_temp);
            switch (entry_indx) {
                case 0 :  flex_entry->pm_num = value; break;
                case 1 :  flex_entry->subport = value; break;
                case 2 :  flex_entry->speed = value; break;
                case 3 :  flex_entry->num_lanes = value; break;
                case 4 :  fec_type = value; break;
                default : cli_out("\n char_convert() to many entries entry_indx=%0d\n",
                          entry_indx); break;
            }
            i = 0;
            entry_indx++;
        }
        str++;
    } while (*str && (*str != '\0'));

    if (flexport_p->fec_type_param != -1) {
        fec_type = flexport_p->fec_type_param;
    }

    switch(fec_type) {
        case   0 : flex_entry->fec = FLX_FEC_NONE;   break; /**< no FEC */
        case  74 : flex_entry->fec = FLX_FEC_BASE_R; break; /**< CL74/Base-R. 64/66b KR FEC for fabric */
        case 528 : flex_entry->fec = FLX_FEC_RS_FEC; break; /**< CL91/RS-FEC also knwon as RS 528*/
        case 544 : if (flex_entry->speed == 400) {
                       flex_entry->fec = FLX_FEC_RS_544_2XN;  /**< Rs544 2xN*/
                   } else {
                       flex_entry->fec = FLX_FEC_RS_544;  /**< Rs544 1xN*/
                   }
                   break;
        case 5442: flex_entry->fec = FLX_FEC_RS_544_2XN; break;   /**< Rs544 2xN*/
        case 272 : flex_entry->fec = FLX_FEC_RS_272; break; /**< Rs272 */
        default  : flex_entry->fec = FLX_FEC_NONE;   break; /**< no FEC */
    }

    return SOC_E_NONE;
}

/* NEW FRAMEWORK */
static int find_str_dict(
    int unit,
    char *new_cdmac_config_str,
    char *cdmac_config[4],
    int *num_entries)
{
    char fname[20] = "cdmac_cfg.dict";
    FILE *fp;
    char line[256];
    int len;
    char cfg_key_str[30] = "CDMAC_CONFIG=";
    char *cdmac_config_str;
    int parse_current_config;
    int cdmac_config_found;
    int i;
    flexport_t *flexport_p = flexport_parray[unit];

    /* Open bcm config file */
    fp = fopen(fname, "r");
    if (fp == NULL) {
        cli_out("\n*ERROR: Cannot open bcm config file: '%s'\n", fname);
        return SOC_E_FAIL;
    } else {
        cli_out("Loading bcm config file: '%s'\n", fname);
    }

    parse_current_config = 0;
    cdmac_config_found = 0;
    i = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (sal_strstr(line, "#") != line) {
            len = strlen(line);
            if (len > 30) {
                fclose(fp);
                return SOC_E_FAIL;
            }

            if(len > 1) {
                /*cli_out("FILE: line_size=%0d  =%s", len, line);*/
                if (sal_strstr(line, cfg_key_str) == line) {
                    /*cli_out("CDMAC_CONFIG: %s", line);*/
                    cdmac_config_str = sal_strtok(line, cfg_key_str);
                    /*cli_out("CDMAC_CONFIG is %s", cdmac_config_str);*/
                    sal_strtok(cdmac_config_str, "\n");
                    if (sal_strcmp(cdmac_config_str, new_cdmac_config_str) == 0) {
                        cli_out("CDMAC_CONFIG found %s\n", new_cdmac_config_str);
                        parse_current_config = 1;
                        cdmac_config_found = 1;
                    } else {
                        parse_current_config = 0;
                    }
                } else {
                    if (parse_current_config == 1) {
                        sal_strtok(line, "\n");
                        /*cli_out("MATCHED CONFIG LINE: %s\n", line);*/
                        /*cdmac_config[i] = line;*/
                        sal_strcpy(cdmac_config[i], line);
                        i++;
                    }
                }
            }
        }
    }
    *num_entries = i;

    if (cdmac_config_found == 0) {
        cli_out("\n*ERROR: Could not find CDMAC_CONFIG=%s in config file: '%s'\n", new_cdmac_config_str, fname);
        flexport_p->test_fail = 1;
        flexport_p->test_fail_iter[flexport_p->flex_iter] = 1;
    }

    fclose(fp);

    return SOC_E_NONE;
}


/* NEW FRAMEWORK */
static int process_flex_entry_option(
    int unit,
    char *flex_entry_str)
{
    /*char str_config[20] = "2x100";*/ /*"1x400";*/
    char *cdmac_config[4];
    flex_entry_t *flex_entry;
    int i, num_entries = 0;
    char *token;
    int base_subport[2];
    char *pm_cd, *cd, *cd_cfg;
    int is_2nd_mac;
    char *cd_1, *cd_cfg_1;
    flexport_t *flexport_p = flexport_parray[unit];
    int pm_num, entry_num;

    pm_cd = sal_alloc(20*sizeof(char), "char");
    cd = sal_alloc(20*sizeof(char), "char");
    cd_cfg = sal_alloc(20*sizeof(char), "char");
    cd_1 = sal_alloc(20*sizeof(char), "char");
    cd_cfg_1 = sal_alloc(20*sizeof(char), "char");

   /* get the first token */
   token = sal_strtok(flex_entry_str, "_");
   i = 0;
   is_2nd_mac = 0;
   while( token != NULL ) {
      if (i == 0) { sal_strcpy(pm_cd, token);}
      if (i == 1) { sal_strcpy(cd, token);}
      if (i == 2) { sal_strcpy(cd_cfg, token);}
      if (i == 3) { sal_strcpy(cd_1, token);}
      if (i == 4) { sal_strcpy(cd_cfg_1, token);}
      /*cli_out( "token=%s pm_cd=%s\n", token, pm_cd);*/
      token = sal_strtok(NULL, "_");
      i++;
   }
   if (i > 3) { is_2nd_mac = 1;}
   /*cli_out( "%s %s\n", pm_num, cdmac_num);
   cli_out( "token=%s\n", token);*/


   token = sal_strtok(pm_cd, "PM");
   pm_num = sal_atoi(token);
   token = sal_strtok(cd, "CD");
   base_subport[0] = 4 * sal_atoi(token);
   base_subport[1] = 0;
   if(is_2nd_mac == 1) {
       token = sal_strtok(cd_1, "CD");
       base_subport[1] = 4 * sal_atoi(token);
   }

   /*cli_out( "pm_num=%0d base_subport0=%0d base_subport1=%0d cd_cfg=%s cd_cfg_1=%s\n",
       pm_num, base_subport[0], base_subport[1], cd_cfg, cd_cfg_1);*/

    for (i = 0; i < 4; i++){
        cdmac_config[i] = sal_alloc(20*sizeof(char), "char");
    }

    find_str_dict(unit, cd_cfg, cdmac_config, &num_entries);

    entry_num = flexport_p->num_flex_entries;
    for (i = 0; i < num_entries; i++){
        flex_entry = &(flexport_p->flex_entry[entry_num]);
        flex_entry->pm_num = pm_num;
        /*cli_out("PM[%0d]_CDMAC[%0d]=%s\n",
           pm_num, i, cdmac_config[i]);*/
        get_flex_entry(unit, cdmac_config[i], flex_entry, 1);
        flex_entry->subport += base_subport[0];
        /*cli_out("\n pm_num=%0d subport=%0d speed=%0d num_lanes=%0d fec=%0d\n",
            flex_entry->pm_num, flex_entry->subport, flex_entry->speed,
            flex_entry->num_lanes, flex_entry->fec);*/
        /*if fec specified as global param, add it here*/
        entry_num++;
    }
    flexport_p->num_flex_entries = entry_num;

    if (is_2nd_mac == 1) {
        find_str_dict(unit, cd_cfg_1, cdmac_config, &num_entries);
        entry_num = flexport_p->num_flex_entries;
        for (i = 0; i < num_entries; i++){
            flex_entry = &(flexport_p->flex_entry[entry_num]);
            flex_entry->pm_num = pm_num;
            /*cli_out("PM[%0d]_CDMAC[%0d]=%s\n",
               pm_num, i, cdmac_config[i]);*/
            get_flex_entry(unit, cdmac_config[i], flex_entry, 1);
            flex_entry->subport += base_subport[1];
            /*cli_out("\n pm_num=%0d subport=%0d speed=%0d num_lanes=%0d fec=%0d\n",
                flex_entry->pm_num, flex_entry->subport, flex_entry->speed,
                flex_entry->num_lanes, flex_entry->fec);*/
            /*if fec specified as global param, add it here*/
            entry_num++;
        }
        flexport_p->num_flex_entries = entry_num;
   }

    sal_free(pm_cd);
    sal_free(cd);
    sal_free(cd_cfg);
    sal_free(cd_1);
    sal_free(cd_cfg_1);

    for (i = 0; i < 4; i++){
        sal_free(cdmac_config[i]);
    }

    return SOC_E_NONE;
}



/* NEW FRAMEWORK */
static int flexport_get_new_lport(
    int unit,
    int pport,
    pbmp_t log_pbmp,
    int *new_lport)
{
    int rv = BCM_E_NONE;
    int p;
    int log_start, log_end, pipe;
    int lport_found;

    get_per_pipe_log_port_range(unit, pport,
        &log_start, &log_end, &pipe);

    *new_lport = 0;
    lport_found = 0;
    /* find the first available log port in pipe */
    for (p = log_start; p <= log_end; p++) {
        if (!SOC_PBMP_MEMBER(log_pbmp, p)) {
            *new_lport = p;
            lport_found = 1;
            break;
        }
    }

    if (lport_found == 0) {
        rv = BCM_E_FAIL;
        cli_out("\nflexport_get_new_lport() Unable to find an available log port for phy_port=%0d",
                   pport);
    }

    return rv;
}

/* NEW FRAMEWORK */
static int flexport_apply_sku_constraints(
    int unit)
{
    flexport_t *flexport_p = flexport_parray[unit];
    int rv = BCM_E_NONE;
    int i, old_pm, new_pm, quadrant, shift = 0;

    /* If SKU 9.6T then :
     * pm[3] is not valid in pipes {0,1,4,5}
     * pm[0] is not valid in pipes {2,3,6,7}
     * have a deterministic transformation to PM[0:2] / PM[1:3]
     */
    if (flexport_p->sku == 56981) {
        for (i = 0; i < flexport_p->num_flex_entries; i++) {
            old_pm = flexport_p->flex_entry[i].pm_num;
            quadrant = old_pm / 8;
            shift = ((quadrant % 2) == 0) ? 0 : 1;
            new_pm = ((old_pm / 4) * 4) + (old_pm % 3) + shift;
            flexport_p->flex_entry[i].pm_num = new_pm;
        }
    }

    /* Half Chip SKU :
     * pipes active are {0,1,6,7}
     * move a pm from non valid pipe to a valid one
     */
    if (flexport_p->sku == 56983) {
        for (i = 0; i < flexport_p->num_flex_entries; i++) {
            old_pm = flexport_p->flex_entry[i].pm_num;
            if (((old_pm + (_TH3_NUM_OF_TSCBH / 4)) % _TH3_NUM_OF_TSCBH) >= (_TH3_NUM_OF_TSCBH / 2)) {
                new_pm = (old_pm + (_TH3_NUM_OF_TSCBH / 2)) % _TH3_NUM_OF_TSCBH;
                flexport_p->flex_entry[i].pm_num = new_pm;
            }
        }
    }

    /* 2.5 PMs per pipe SKU :
     * PMs not active: 0,1,7,8,14,15,16,17,23,24,30,31
     */
    if (flexport_p->sku == 56982) {
        for (i = 0; i < flexport_p->num_flex_entries; i++) {
            old_pm = flexport_p->flex_entry[i].pm_num;
            /* Move flex from inactive PMs to active ones */
            switch(old_pm % 16) {
                case 0:
                case 1:
                case 7:
                case 8:
                case 14:
                case 15:
                    new_pm = (old_pm + 4) % 32;
                    break;
                default :
                    new_pm = old_pm;
                    break;
            }
            flexport_p->flex_entry[i].pm_num = new_pm;
        }
    }


    return rv;
}

/* NEW FRAMEWORK */
static int flexport_apply_transformation(
    int unit)
{
    flexport_t *flexport_p = flexport_parray[unit];
    int rv = BCM_E_NONE;
    int i, old_pm, new_pm;

    /* Apply random transformation
     * Circular shift: new_pm = (old_pm + RAND) % CHIP_NUM_PMs
     * RAND is randomized at the beginning of the test &
     * fixed for the entire test run
     */
    if (flexport_p->rand_pm_en == 1) {
        for (i = 0; i < flexport_p->num_flex_entries; i++) {
            old_pm = flexport_p->flex_entry[i].pm_num;
            new_pm = (old_pm + flexport_p->rand_pm_shift) % _TH3_NUM_OF_TSCBH;
            flexport_p->flex_entry[i].pm_num = new_pm;
        }
    }

    return rv;
}



/* NEW FRAMEWORK */
static int process_flex_entries(
    int unit)
{
    int i;
    int pport, lport;
    int curr_speed, curr_num_lanes;
    int new_speed, new_num_lanes;
    int rv = BCM_E_NONE;
    pbmp_t log_pbmp;
    int new_lport, lane;
    soc_info_t *si;
    flexport_t *flexport_p = flexport_parray[unit];

    si = &SOC_INFO(unit);

    /* pbm used to get available log ports */
    log_pbmp = PBMP_PORT_ALL(unit);

    flexport_apply_transformation(unit);
    flexport_apply_sku_constraints(unit);

    /* Firstly, delete ports */
    flexport_p->num_del = 0;
    for (i = 0; i < flexport_p->num_flex_entries; i++) {
        pport = flex_get_phy_port(unit, &(flexport_p->flex_entry[i]));
        lport = si->port_p2l_mapping[pport];
        if (lport != -1) {
            rv = bcm_port_speed_get(unit, lport, &curr_speed);
            if (BCM_FAILURE(rv) && rv != BCM_E_PORT) {
                cli_out("\nbcm_port_speed_get %0d %0d: %s",
                        lport, rv, bcm_errmsg(rv));
                return rv;
            }
            curr_num_lanes = si->port_num_lanes[lport];
        } else {
            curr_speed = 0;
            curr_num_lanes = 0;
        }
        new_speed = flexport_p->flex_entry[i].speed;
        new_speed = new_speed * 1000;
        new_num_lanes = flexport_p->flex_entry[i].num_lanes;

        /* Delete port if:
         * curr_speed != 0
         * AND curr_speed/num_lanes different that new_speed/num_lanes
         */
        if ((curr_speed != 0) &&
            ((curr_speed != new_speed) ||
             (curr_num_lanes != new_num_lanes))) {
            cli_out("Removing phy_port=%0d log_port=%0d\n",
                       pport, lport);
            bcm_port_resource_t_init(
                &flexport_p->del_resource[flexport_p->num_del]);
            flexport_p->del_resource[flexport_p->num_del].physical_port = -1;
            flexport_p->del_resource[flexport_p->num_del].port = lport;
            flexport_p->num_del++;
            SOC_PBMP_PORT_ADD(flexport_p->port_down_pbmp, lport);
            /* Free log port to be reused */
            SOC_PBMP_PORT_REMOVE(log_pbmp, lport);
            SOC_PBMP_PORT_REMOVE(flexport_p->counter_pbm, lport);
        }

        /* Also, remove all actvie ports in [pport : pport+new_num_lanes-1] range */
        for (lane = 1; lane < new_num_lanes; lane ++) {
            lport = si->port_p2l_mapping[pport + lane];
            if (lport != -1) {
                cli_out("Removing also phy_port=%0d log_port=%0d\n",
                       pport + lane, lport);
                bcm_port_resource_t_init(
                    &flexport_p->del_resource[flexport_p->num_del]);
                flexport_p->del_resource[flexport_p->num_del].physical_port = -1;
                flexport_p->del_resource[flexport_p->num_del].port = lport;
                flexport_p->num_del++;
                SOC_PBMP_PORT_ADD(flexport_p->port_down_pbmp, lport);
                /* Free log port to be reused */
                SOC_PBMP_PORT_REMOVE(log_pbmp, lport);
                SOC_PBMP_PORT_REMOVE(flexport_p->counter_pbm, lport);
            }
        }
    }


    /* Secondly, add ports */
    flexport_p->num_add = 0;
    for (i = 0; i < flexport_p->num_flex_entries; i++) {
        pport = flex_get_phy_port(unit, &(flexport_p->flex_entry[i]));
        lport = si->port_p2l_mapping[pport];
        if (lport != -1) {
            rv = bcm_port_speed_get(unit, lport, &curr_speed);
            if (BCM_FAILURE(rv) && rv != BCM_E_PORT) {
                cli_out("\nbcm_port_speed_get %0d %0d: %s",
                        lport, rv, bcm_errmsg(rv));
                return rv;
            }
            curr_num_lanes = si->port_num_lanes[lport];
        } else {
            curr_speed = 0;
            curr_num_lanes = 0;
        }
        new_speed = flexport_p->flex_entry[i].speed;
        new_speed = new_speed * 1000;
        new_num_lanes = flexport_p->flex_entry[i].num_lanes;

        /* Add port if:
         * new_speed != 0
         * AND curr_speed/num_lanes different that new_speed/num_lanes
         */
        if ((new_speed != 0) &&
            ((curr_speed != new_speed) ||
             (curr_num_lanes != new_num_lanes))) {
            rv = flexport_get_new_lport(unit, pport, log_pbmp, &new_lport);
            if (BCM_FAILURE(rv)) {
                cli_out("\nUnable to find available port");
                flexport_p->test_fail = 1;
                flexport_p->test_fail_iter[flexport_p->flex_iter] = 1;
                return rv;
            }
            bcm_port_resource_t_init(
                &flexport_p->add_resource[flexport_p->num_add]);
            cli_out("Adding phy_port=%0d log_port=%0d speed=%0dG num_lanes=%0d\n",
                       pport, new_lport, new_speed/1000, new_num_lanes);
            flexport_p->add_resource[flexport_p->num_add].physical_port = pport;
            flexport_p->add_resource[flexport_p->num_add].port  = new_lport;
            flexport_p->add_resource[flexport_p->num_add].speed = new_speed;
            flexport_p->add_resource[flexport_p->num_add].lanes = new_num_lanes;
            flexport_p->add_resource[flexport_p->num_add].encap = BCM_PORT_ENCAP_IEEE; /* Only ETH for now */
            flexport_p->add_resource[flexport_p->num_add].fec_type = flexport_p->flex_entry[i].fec;
            flexport_p->add_resource[flexport_p->num_add].link_training = 0;
            flexport_p->num_add++;
            SOC_PBMP_PORT_ADD(flexport_p->port_up_pbmp, new_lport);
            SOC_PBMP_PORT_ADD(log_pbmp, new_lport);
            SOC_PBMP_PORT_ADD(flexport_p->counter_pbm, new_lport);
        }
    }

    return rv;
}

/* NEW FRAMEWORK */
static int process_flex_id(
    int unit,
    int flex_id,
    char *section)
{
    char fname[20] = "flexport_th3.tpl";
    FILE *fp;
    char line[256];
    int len;
    char flex_key_str[60] = "FLEX_ID=";
    char *line_flex_str;
    int parse_current_config;
    int string_found;
    int i, line_flex_id;
    flexport_t *flexport_p = flexport_parray[unit];
    char sec_key_str[30] = "SECTION=";
    int input_mode;
    int found_section;

    input_mode = flexport_p->input_mode_param;
    found_section = (input_mode == 2) ? 1 : 0;

    fp = fopen(fname, "r");
    if (fp == NULL) {
        cli_out("\n*ERROR: Cannot open bcm config file: '%s'\n", fname);
        return SOC_E_FAIL;
    } else {
        cli_out("\nLoading bcm config file: '%s'\n", fname);
    }

    parse_current_config = 0;
    string_found = 0;
    i = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (sal_strstr(line, "#") != line) {
            len = strlen(line);
            if (len > 60) {
                fclose(fp);
                return SOC_E_FAIL;
            }

            if(len > 1) {
                if ((input_mode == 3) &&
                    (sal_strstr(line, sec_key_str) == line)) {
                    line_flex_str = sal_strtok(line, sec_key_str);
                    sal_strtok(line_flex_str, "\n");
                    if (sal_strcmp(line_flex_str, section) == 0) {
                        found_section = 1;
                        cli_out("FOUND SECTION: %s\n", line_flex_str);
                    } else {
                        found_section = 0;
                        parse_current_config = 0;
                    }
                } else if ((sal_strstr(line, flex_key_str) == line) &&
                    (found_section == 1) ) {
                    line_flex_str = sal_strtok(line, flex_key_str);
                    sal_strtok(line_flex_str, "\n");
                    line_flex_id = sal_atoi(line_flex_str);
                    if (line_flex_id == flex_id) {
                        /*cli_out("FLEX_ID found %s %0d\n", line_flex_str, line_flex_id);*/
                        parse_current_config = 1;
                        string_found = 1;
                    } else {
                        parse_current_config = 0;
                    }
                } else {
                    if (parse_current_config == 1) {
                        sal_strtok(line, "\n");
                        /*cli_out("MATCHED FLEX ENTRY LINE: %s\n", line);*/
                        process_flex_entry_option(unit, line);
                        i++;
                    }
                }
            }
        }
    }

    if (string_found == 0) {
        cli_out("\n*ERROR: Could not find FLEX_%04d in config file: '%s'\n", flex_id, fname);
        flexport_p->test_fail = 1;
        flexport_p->test_fail_iter[flexport_p->flex_iter] = 1;
    }

    fclose(fp);

    return SOC_E_NONE;
}




/* NEW FRAMEWORK */
static int flexport_by_entries(
    int unit)
{
    flexport_t *flexport_p = flexport_parray[unit];
    int i, entry_num;
    int flex_id;

/* Input Mode: per port in CLI */
if(flexport_p->input_mode_param == 0) {
    entry_num = 0;
    for (i = 0; i < 64; i++) {
        if (flexport_p->tsc_param[i] != NULL) {
            get_flex_entry(unit, flexport_p->tsc_param[i], &(flexport_p->flex_entry[entry_num]), 0);
            entry_num++;
        }
    }
    flexport_p->num_flex_entries = entry_num;
/* Input Mode: per PM using config_dict.txt */
} else if(flexport_p->input_mode_param == 1) {
    for (i = 0; i < 64; i++) {
        if (flexport_p->tsc_param[i] != NULL) {
            process_flex_entry_option(unit, flexport_p->tsc_param[i]);
        }
    }
/* Input Mode: per FLEX_ID using flex_tpl.txt & config_dict.txt */
} else {
    flex_id = flexport_p->flex_id_start_param + flexport_p->flex_iter;
    process_flex_id(unit, flex_id, flexport_p->tpl_section);
}

    process_flex_entries(unit);

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

    cli_out("CALL set_up_ports\n");

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
if (!SOC_IS_TOMAHAWK3(unit)){
        soc_mem_field32_set(unit, LPORT_TABm, lport_tab_entry.entry_data,
                            ALLOW_SRC_MODf, 0x1);
}
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
            vlan = get_vlan_id(unit, p);
            if (SOC_PBMP_MEMBER(vlan_pbmp, p)) {
                /*cli_out("\n set_up_streams() port=%0d calling stream_set_vlan()", p);*/
                stream_set_vlan(unit, vlan, 1);
            } else {
                /*cli_out("\n set_up_streams() port=%0d calling bcm_vlan_port_add()", p);*/
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

static bcm_error_t
set_up_streams_l2(
    int unit,
    pbmp_t pbmp,
    int delete)
{
    bcm_error_t rv = BCM_E_NONE;
    int p;
    bcm_l2_addr_t l2_addr;
    bcm_mac_t mac_da;
    bcm_vlan_t vlan;

    PBMP_ITER(pbmp, p) {
        if (p < SOC_MAX_NUM_PORTS) {
            vlan = get_vlan_id(unit, p);
            get_mac_da(unit, p, mac_da);
            if (delete == 0) { /* add L2 entry */
                bcm_l2_addr_t_init(&l2_addr, mac_da, vlan);
                l2_addr.port = p;
                l2_addr.flags = 0;
                BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
            } else { /* delete L2 entry */
                BCM_IF_ERROR_RETURN(bcm_l2_addr_delete(unit, mac_da, vlan));
            }
        }
    }

    return rv;
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
send_pkts(int unit, pbmp_t pbmp)
{
    int port;
    bcm_mac_t mac_da;
    bcm_mac_t mac_sa;
    stream_pkt_t *tx_pkt;
    flexport_t *flexport_p = flexport_parray[unit];
    rate_calc_t *rate_calc_p = rate_calc_parray[unit];

    tx_pkt = sal_alloc(sizeof(stream_pkt_t), "tx_pkt");
    sal_memset(tx_pkt, 0, sizeof(stream_pkt_t));

    cli_out("\n==================================================\n");
    cli_out("Sending packets ...\n");
    PBMP_ITER(pbmp, port) {
        if (port < SOC_MAX_NUM_PORTS) {
            tx_pkt->port = port;
            tx_pkt->pkt_seed = flexport_p->pkt_seed;
            tx_pkt->num_pkt = flexport_p->per_port_pkt_cnt[port]; /*lossless_flood_cnt(unit, port);*/
            tx_pkt->pkt_size = flexport_p->per_port_pkt_size[port];
            /*cli_out("\nSending packets on port %0d size %0d num %0d.",
                       port, tx_pkt->pkt_size, tx_pkt->num_pkt);*/
            if(flexport_p->pkt_size_param == 1) {
                tx_pkt->rand_pkt_size_en = 1;
                tx_pkt->rand_pkt_size = rate_calc_p->rand_pkt_sizes[port];
            } else {
                tx_pkt->rand_pkt_size_en = 0;
                tx_pkt->rand_pkt_size = NULL;
            }
            tx_pkt->tx_vlan = get_vlan_id(unit, port);
            get_mac_da(unit, port, mac_da);
            get_mac_sa(unit, port, mac_sa);
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
    flexport_t *flexport_p = flexport_parray[unit];

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
        flexport_p->test_fail = 1;
        flexport_p->test_fail_iter[flexport_p->flex_iter] = 1;
    } else {
        cli_out("\n********** RATE CHECK PASSED ***********");
    }

    sal_free(etime);
    sal_free(tpkt_end);
    sal_free(tbyt_end);
    sal_free(rate);
    return rv;
}


static int flexport_get_lr_pkt_size(
    int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    int pkt_size;
    int dpr_freq;

    dpr_freq = soc_property_get(unit, spn_DPR_CLOCK_FREQUENCY, -1);

    pkt_size = 295;
    /* min pkt size depends also on max IO BW/ SKU */
    if (si->frequency >= 1325) {
        if (dpr_freq >= 1000) { /* SKU 12.8T */
            pkt_size = 295;
        } else {
            pkt_size = 298;
        }
    } else if (si->frequency >= 1000) {
        if (dpr_freq >= 750) { /* SKU 9.6T */
            pkt_size = 297;
        } else {  /* SKU 9.6T */
            pkt_size = 301;
        }
    } else if (si->frequency >= 925) {
        pkt_size = 161;
    }

    LOG_INFO(BSL_LS_APPL_TESTS,
            (BSL_META_U(unit,
               "flexport_get_lr_pkt_size() CORE freq=%0d DPR freq=%0d lr_pkt_size=%0d\n"),
               si->frequency, dpr_freq, pkt_size));

    return pkt_size;
}


/* NEW FRAMEWORK */
static bcm_error_t flexport_set_pkt_size(
    int unit,
    pbmp_t pbmp)
{
    bcm_error_t rv = BCM_E_NONE;
    int final_pkt_size, port;
    flexport_t *flexport_p = flexport_parray[unit];
    int lr_pkt_size;

    lr_pkt_size = flexport_get_lr_pkt_size(unit);

    final_pkt_size = MIN_PKT_SIZE;
    switch(flexport_p->pkt_size_param) {
        /* random pkt between 64: max pkt */
        case 0 :
            final_pkt_size = lr_pkt_size;
            break;
        case 1 :
            final_pkt_size = (sal_rand() % (MTU - MIN_PKT_SIZE + 1)) +
                             MIN_PKT_SIZE;
            break;
        default :
            final_pkt_size = flexport_p->pkt_size_param;
            break;
    }

    PBMP_ITER(pbmp, port) {
        if (port < SOC_MAX_NUM_PORTS) {
            flexport_p->per_port_pkt_size[port] = final_pkt_size;
        }
    }

    return rv;
}

/* NOTE: number of pkts to get full BW depends on:
 * - port speed
 * - avg pkt size
 * - pkt round trip latency which is a function of
 *   - device design
 *   - core clock
 *   - pp clock
 */
static bcm_error_t th3_stream_get_pkt_cnt(
    int unit,
    int port,
    int avg_pkt_size,
    int loopback_mode,
    int fec_type,
    int traffic_load,
    int *num_pkts)
{
    bcm_error_t rv = BCM_E_NONE;
    int round_trip_latency, port_speed;
    soc_info_t *si = &SOC_INFO(unit);
    int final_pkt_cnt = 0;

    switch (loopback_mode) {
        /* MAC Loopback */
        case BCM_PORT_LOOPBACK_MAC  : round_trip_latency = 1350 / 2 /*1190 / 2*/ /* 1154 */; break;
        /* PHY Loopback */
        case BCM_PORT_LOOPBACK_PHY  : round_trip_latency = 1650 / 2 /*1520 / 2*/ /*1474 */; break;
        /* EXT Loopback; not easy to figure out; cables, etc.*/
        case BCM_PORT_LOOPBACK_NONE : round_trip_latency = 1950 / 2; break;
        default :                     round_trip_latency = 1350 / 2; break;
    }

    port_speed = si->port_speed_max[port];
    final_pkt_cnt =
        ((round_trip_latency * (port_speed / 1000)) / ((avg_pkt_size + 20) * 8))
        + 1;

    /* If low speed port then add 50% more pkts */
    if (port_speed < 50000) {
        final_pkt_cnt = ((final_pkt_cnt * 150) / 100) + 1;
    } else if(port_speed < 100000) {
        final_pkt_cnt = ((final_pkt_cnt * 120) / 100) + 1;
    } else if(port_speed < 200000) {
        final_pkt_cnt = ((final_pkt_cnt * 110) / 100) + 1;
    }

    if (si->frequency < 1000) {
        final_pkt_cnt = ((final_pkt_cnt * 130) / 100) + 1;
    } else if (si->frequency < 1325) {
        final_pkt_cnt = ((final_pkt_cnt * 140) / 100) + 1;
    }

    *num_pkts = final_pkt_cnt;

    return rv;
}




/* NEW FRAMEWORK */
static bcm_error_t flexport_set_pkt_count(
    int unit,
    pbmp_t pbmp)
{
    bcm_error_t rv = BCM_E_NONE;
    flexport_t *flexport_p = flexport_parray[unit];
    int port;
    int avg_pkt_size, final_pkt_cnt;
    int fec_type, traffic_load;

    fec_type = 0;
    traffic_load = 100;

    PBMP_ITER(pbmp, port) {
        if (port < SOC_MAX_NUM_PORTS) {
            if(flexport_p->flood_pkt_cnt_param == 0) {
                /* For now avg pkt size is the actual pkt size */
                avg_pkt_size = flexport_p->per_port_pkt_size[port];
                th3_stream_get_pkt_cnt(unit, port,
                                       avg_pkt_size,
                                       flexport_p->loopback_mode_param,
                                       fec_type,
                                       traffic_load,
                                       &final_pkt_cnt);
            } else {
                final_pkt_cnt = flexport_p->flood_pkt_cnt_param;
            }
            flexport_p->per_port_pkt_cnt[port] = final_pkt_cnt;
        }
    }

    return rv;
}

/* NEW FRAMEWORK */
static int flexport_get_oversub_ratio(
    int unit,
    int port,
    int pkt_size)
{
    /* for now assume Line Rate only */
    return 1000;
}

static bcm_error_t flexport_set_exp_rate(
    int unit,
    pbmp_t pbmp)
{
    bcm_error_t rv = BCM_E_NONE;
    rate_calc_t *rate_calc_p = rate_calc_parray[unit];
    flexport_t *flexport_p = flexport_parray[unit];
    soc_info_t *si = &SOC_INFO(unit);
    int oversub_ratio;
    int port;

    PBMP_ITER(pbmp, port) {
        if (port < SOC_MAX_NUM_PORTS) {
            COMPILER_64_SET(rate_calc_p->exp_max_rate[port],
                            0, si->port_speed_max[port]);
            oversub_ratio = flexport_get_oversub_ratio(unit, port,
                                flexport_p->per_port_pkt_size[port]);
            COMPILER_64_SET(rate_calc_p->exp_rate[port],
                            0, si->port_speed_max[port]);
            COMPILER_64_UDIV_32(rate_calc_p->exp_rate[port], oversub_ratio);
            COMPILER_64_UMUL_32(rate_calc_p->exp_rate[port], 1000);
        }
    }

    return rv;
}

/* NEW FRAMEWORK */
/* Sets 
 * pkt size;
 * the number of pkts to be send out
 * expected rate
 */
static bcm_error_t flexport_set_port_arrays(
    int unit,
    pbmp_t pbmp)
{
    bcm_error_t rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(flexport_set_pkt_size(unit, pbmp));
    BCM_IF_ERROR_RETURN(flexport_set_pkt_count(unit, pbmp));
    BCM_IF_ERROR_RETURN(flexport_set_exp_rate(unit, pbmp));

    return rv;
}


static bcm_error_t set_up_cutthru(
    int unit,
    pbmp_t pbmp,
    int asf_mode)
{
    bcm_error_t rv = BCM_E_NONE;
    int port;
    int curr_asf_mode = 0;

    PBMP_ITER(pbmp, port) {
        if (port < SOC_MAX_NUM_PORTS) {
            bcm_switch_control_port_get(unit, port, bcmSwitchAlternateStoreForward, &curr_asf_mode);
            bcm_switch_control_port_set(unit, port, bcmSwitchAlternateStoreForward, asf_mode);
            cli_out("bcm_switch_control_port_get() port=%0d curr_asf_mode=%0d new_asf_mode=%0d\n",
                port, curr_asf_mode, asf_mode);
        }
    }

    return rv;
}

/* NEW FRAMEWORK */
static bcm_error_t flexport_setup_switch(
    int unit,
    pbmp_t pbmp,
    int delete)
{
    bcm_error_t rv = BCM_E_NONE;
    flexport_t *flexport_p = flexport_parray[unit];
    int loopback_mode_param;

    loopback_mode_param = (delete == 0) ? flexport_p->loopback_mode_param :
                                          BCM_PORT_LOOPBACK_NONE;
    stream_set_lpbk(unit, pbmp, loopback_mode_param);
    if (delete == 0) { /* Only if ports come UP */
        /*stream_turn_off_fc(unit, pbmp);*/
        set_up_ports(unit, pbmp);
        flexport_p->vlan_pbmp = set_up_streams(unit, pbmp,
                                           flexport_p->vlan_pbmp);

        if (flexport_p->cut_thru_en == 1) {
        /* Cut-thru forwarding between same
           speed ports */
            set_up_cutthru(unit, pbmp, 1); /* bcmPortAsfModeSameSpeed = 1 */
        }
    }
    set_up_streams_l2(unit, pbmp, delete);

    return rv;
}

/* NEW FRAMEWORK */
static bcm_error_t flexport_start_traffic(
    int unit,
    pbmp_t pbmp)
{
    bcm_error_t rv = BCM_E_NONE;
    int port, queue, count = 0;
    /*flexport_t *flexport_p = flexport_parray[unit];*/

    send_pkts(unit, pbmp);

    queue = 0;
    if (BCM_PBMP_NOT_NULL(pbmp)) {
        /* After traffic is injected clear BST counters that track traffic from CPU to the port 
         * We want to make sure only port to port traffic is checket for not being in CT
         */
        PBMP_ITER(pbmp, port) {
            if (port < SOC_MAX_NUM_PORTS) {
                th3_get_bst_total_queue(unit, port, queue, &count);
                if (count == 0) {
                    cli_out("*WARNING flexport_start_traffic() port=%0d BST THDO should be !=0\n", port);
                    /*flexport_p->test_fail = 1;
                    flexport_p->test_fail_iter[flexport_p->flex_iter] = 1;*/
                }
            }
        }
    }

    return rv;
}

/* NEW FRAMEWORK */
static bcm_error_t flexport_start_rate_measurement(
    int unit,
    pbmp_t pbmp)
{
    bcm_error_t rv = BCM_E_NONE;
    flexport_t *flexport_p = flexport_parray[unit];

    cli_out("Wait 0.2s for traffic to stabilize\n");
    sal_usleep(100000);
    start_rate_measurement(unit, pbmp, flexport_p->emulation_param);

    return rv;
}

/* NEW FRAMEWORK */
static bcm_error_t flexport_get_flex_info(
    int unit,
    int flex_iter)
{
    bcm_error_t rv = BCM_E_NONE;
    flexport_t *flexport_p = flexport_parray[unit];

    flexport_p->num_flex_entries = 0;
    flexport_p->num_del = 0;
    flexport_p->num_add = 0;

    SOC_PBMP_CLEAR(flexport_p->port_down_pbmp);
    SOC_PBMP_CLEAR(flexport_p->port_up_pbmp);

    flexport_by_entries(unit);

    return rv;
}


/* NEW FRAMEWORK */
static bcm_error_t flexport_check_rate(
    int unit,
    pbmp_t pbmp)
{
    bcm_error_t rv = BCM_E_NONE;
    flexport_t *flexport_p = flexport_parray[unit];

    cli_out("===================================\n");
    cli_out("\nMeasuring Rate over a period of %0d ms\n",
            flexport_p->rate_calc_interval_param * 100);
    sal_usleep(flexport_p->rate_calc_interval_param * 100000);


    if (BCM_PBMP_NOT_NULL(pbmp)) {
        if (!SOC_IS_TOMAHAWK3(unit)){
            if (stream_chk_mib_counters(unit, pbmp, 0) != BCM_E_NONE) {
                flexport_p->test_fail = 1;
                flexport_p->test_fail_iter[flexport_p->flex_iter] = 1;
            }
        }
        if (check_rates(unit, pbmp,
                       flexport_p->rate_tolerance_lr_param,
                       flexport_p->rate_tolerance_ov_param,
                       flexport_p->emulation_param) != BCM_E_NONE) {
            flexport_p->test_fail = 1;
            flexport_p->test_fail_iter[flexport_p->flex_iter] = 1;
        }
    }

    return rv;
}

/* NEW FRAMEWORK */
static bcm_error_t flexport_stop_traffic(
    int unit,
    pbmp_t pbmp)
{
    bcm_error_t rv = BCM_E_NONE;
    flexport_t *flexport_p = flexport_parray[unit];
    bcm_mac_t mac_da;
    bcm_mac_t mac_sa;
    stream_integrity_t *pkt_intg;
    int port;

    if (BCM_PBMP_NOT_NULL(pbmp)) {
        if (flexport_p->check_packet_integrity_param != 0) {
            pkt_intg = sal_alloc(sizeof(stream_integrity_t), "pkt integrity");
            sal_memset(pkt_intg, 0, sizeof(stream_integrity_t));
            pkt_intg->rx_pbmp = pbmp;
            PBMP_ITER(pbmp, port) {
                if (port < SOC_MAX_NUM_PORTS) {
                    pkt_intg->port_pkt_seed[port] = flexport_p->pkt_seed;
                    pkt_intg->port_flood_cnt[port] = flexport_p->per_port_pkt_cnt[port]; /*lossless_flood_cnt(unit, port);*/
                    pkt_intg->rx_vlan[port] = get_vlan_id(unit, port);
                    pkt_intg->tx_vlan[port] = get_vlan_id(unit, port);
                    get_mac_da(unit, port, mac_da);
                    get_mac_sa(unit, port, mac_sa);
                    sal_memcpy(pkt_intg->mac_da[port], mac_da, NUM_BYTES_MAC_ADDR);
                    sal_memcpy(pkt_intg->mac_sa[port], mac_sa, NUM_BYTES_MAC_ADDR);
                }
            }
            rv = stream_chk_pkt_integrity(unit, pkt_intg);
            if (rv != BCM_E_NONE) {
                flexport_p->test_fail = 1;
                flexport_p->test_fail_iter[flexport_p->flex_iter] = 1;
            }
            sal_free(pkt_intg);
        }
    }

    return rv;
}

/* NEW FRAMEWORK */
static bcm_error_t flexport_post_traffic_checks(
    int unit,
    pbmp_t pbmp)
{
    bcm_error_t rv = BCM_E_NONE;
    flexport_t *flexport_p = flexport_parray[unit];
    int port, queue, count=0;
    int fail = 0;

    /* Check CutThru */
    queue = 0;
    fail = 0;
    if (BCM_PBMP_NOT_NULL(pbmp)) {
        PBMP_ITER(pbmp, port) {
            if (port < SOC_MAX_NUM_PORTS) {
                th3_get_bst_total_queue(unit, port, queue, &count);
                if (flexport_p->cut_thru_en == 1) {
                    if ((count != 0) && (!IS_MANAGEMENT_PORT(unit,port))) {
                        cli_out("*ERROR flexport_post_traffic_checks() CutThru wasn't fully maintain for port=%0d\n", port);
                        fail = 1;
                    }
                    if ((count == 0) && (IS_MANAGEMENT_PORT(unit,port))) {
                        cli_out("*ERROR flexport_post_traffic_checks() MGM port=%0d should have THDO BST !=0\n", port);
                        fail = 1;
                    }
                } else {
                    if (count == 0) {
                        cli_out("*ERROR flexport_post_traffic_checks() port=%0d should have THDO BST !=0\n", port);
                        fail = 1;
                    }
                }
            }
        }
    }

    if (fail == 1) {
        flexport_p->test_fail = 1;
        flexport_p->test_fail_iter[flexport_p->flex_iter] = 1;
    }

    return rv;
}


/* NEW FRAMEWORK */
static bcm_error_t flexport_do_flexport(
    int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    flexport_t *flexport_p = flexport_parray[unit];
    uint64 val64;

    BCM_IF_ERROR_RETURN(soc_counter_status(unit, &flexport_p->counter_flags,
                                                 &flexport_p->counter_interval,
                                                 &flexport_p->counter_pbm));

    if (flexport_p->num_del + flexport_p->num_add > 0) {
        if (flexport_p->counter_interval > 0) {
            cli_out("\nDisabling counter collection before FlexPort\n");
            soc_counter_stop(unit);
        }
        flexport_call_bcm_api(unit);
        if (flexport_p->counter_interval > 0) {
            cli_out("\nEnabling counter collection after FlexPort\n");
            BCM_IF_ERROR_RETURN(soc_counter_start(unit, flexport_p->counter_flags,
                              flexport_p->counter_interval,
                              flexport_p->counter_pbm));
        }
    }

    if (flexport_p->clear_new_port_counts) {
        COMPILER_64_ZERO(val64);
        BCM_IF_ERROR_RETURN(soc_counter_set_by_port(unit,
                            flexport_p->port_up_pbmp, val64));
    }

    return rv;
}


static bcm_error_t set_up_bst_counters(
    int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_switch_control_t type;
    int arg, THDO_en, THDI_en;
    /*int CFAP_en;*/
    uint32 rval;

    THDO_en = 0x1 << 0;
    THDI_en = 0x1 << 1;
    /*CFAP_en = 0x1 << 2;*/

    arg = THDO_en | THDI_en;
    type = bcmSwitchBstEnable;
    bcm_switch_control_set(unit, type, arg);
    type = bcmSwitchBstTrackingMode;
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, type, arg));

    /* Make COR on */
    READ_MMU_THDO_BST_CONFIGr(unit, &rval);
    soc_reg_field_set(unit, MMU_THDO_BST_CONFIGr,
                &rval, HWM_CORf, 0x1);
    WRITE_MMU_THDO_BST_CONFIGr(unit, rval);

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
flexport_th3_test_init(int unit, args_t *a, void **pa)
{
    flexport_t *flexport_p;
    rate_calc_t *rate_calc_p;

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

/*if (!SOC_IS_TOMAHAWK3(unit)){*/
    BCM_IF_ERROR_RETURN(soc_counter_status(unit, &flexport_p->counter_flags,
                                                 &flexport_p->counter_interval,
                                                 &flexport_p->counter_pbm));
/*
    if (flexport_p->counter_interval > 0) {
        cli_out("\nDisabling counter collection");
        soc_counter_stop(unit);
    }
}*/
    BCM_IF_ERROR_RETURN(bcm_linkscan_enable_get(unit,
                                                &flexport_p->linkscan_enable));
    if (flexport_p->linkscan_enable) {
        cli_out("\nDisabling linkscan");
        BCM_IF_ERROR_RETURN(bcm_linkscan_enable_set(unit, 0));
    }

    /* Enable BST tracking */

    BCM_IF_ERROR_RETURN(set_up_bst_counters(unit));

    return BCM_E_NONE;
}


/* NEW FRAMEWORK */
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
flexport_th3_test(int unit, args_t *a, void *pa)
{
    flexport_t *flexport_p;
    int flex_iter = 0;
    int delete = 0;
    bcm_error_t rv = BCM_E_NONE;
    int num_flex_iterations;

    flexport_p = flexport_parray[unit];

    if (flexport_p->bad_input) {
        goto done;
    }

    cli_out("Calling flexport_test\n");

    flexport_p->pkt_seed = sal_rand();
    num_flex_iterations = flexport_p->flex_id_end_param
                          - flexport_p->flex_id_start_param + 1;

    /*===  BEFORE flex LOOP  ===*/
    flexport_pre_traffic_checks(unit, PBMP_PORT_ALL(unit));
    flexport_set_port_arrays(unit, PBMP_PORT_ALL(unit));
    delete = 0;
    flexport_setup_switch(unit, PBMP_PORT_ALL(unit), delete);
    flexport_pre_traffic_checks(unit, PBMP_PORT_ALL(unit));
    flexport_start_traffic(unit, PBMP_PORT_ALL(unit));
    flexport_start_rate_measurement(unit, PBMP_PORT_ALL(unit));

    /*======== FOR EACH FLEX ITERATION  flex_iter: =======*/ 

    for (flex_iter = 0; flex_iter < num_flex_iterations; flex_iter++) {
        cli_out("====================================================\n");
        cli_out("START Flex Iteration SECTION=%s FLEX_ID=%0d\n", 
                flexport_p->tpl_section,
                flexport_p->flex_id_start_param + flexport_p->flex_iter);
    /* BEFORE FLEXPORT */
        flexport_get_flex_info(unit, flex_iter);
        flexport_check_rate(unit, flexport_p->port_down_pbmp);
        flexport_stop_traffic(unit, flexport_p->port_down_pbmp);
        flexport_post_traffic_checks(unit, flexport_p->port_down_pbmp);
        delete = 1;
        flexport_setup_switch(unit, flexport_p->port_down_pbmp, delete);
        flexport_pre_traffic_checks(unit, PBMP_PORT_ALL(unit));

    /* ACTUAL FLEXPORT */
        flexport_do_flexport(unit);

    /* AFTER FLEXPORT */
        flexport_pre_traffic_checks(unit, PBMP_PORT_ALL(unit));
        flexport_set_port_arrays(unit, flexport_p->port_up_pbmp);
        delete = 0;
        flexport_setup_switch(unit, flexport_p->port_up_pbmp, delete);
        flexport_pre_traffic_checks(unit, PBMP_PORT_ALL(unit));
        flexport_start_traffic(unit, flexport_p->port_up_pbmp);
        flexport_start_rate_measurement(unit, flexport_p->port_up_pbmp);
        cli_out("FINISH Flex Iteration SECTION=%s FLEX_ID=%0d\n", 
                flexport_p->tpl_section,
                flexport_p->flex_id_start_param + flexport_p->flex_iter);
        cli_out("====================================================\n");
        flexport_p->flex_iter++;
    }

    /*===  AFTER flex LOOP  ===*/
    flexport_check_rate(unit, PBMP_PORT_ALL(unit));
    flexport_stop_traffic(unit, PBMP_PORT_ALL(unit));
    flexport_post_traffic_checks(unit, PBMP_PORT_ALL(unit));
    flexport_pre_traffic_checks(unit, PBMP_PORT_ALL(unit));

    if (flexport_p->bad_input == 1) {
        flexport_p->test_fail = 1;
    }
    if (flexport_p->test_fail == 1) {
        rv = BCM_E_FAIL;
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
flexport_th3_test_cleanup(int unit, void *pa)
{
    flexport_t *flexport_p;
    rate_calc_t *rate_calc_p;
    int rv = BCM_E_NONE;
    int num_flex_iterations, i;

    cli_out("\nCalling flexport_test_cleanup");

    flexport_p = flexport_parray[unit];

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

    num_flex_iterations = flexport_p->flex_id_end_param
                          - flexport_p->flex_id_start_param + 1;
    for (i = 0; i < num_flex_iterations; i++) {
        if (flexport_p->test_fail_iter[i] == 1) {
            cli_out("**  FLEX_ID=%0d   FAILED **\n",
                    flexport_p->flex_id_start_param + i);
        }/* else {
            cli_out("\n**  Flexport iteration %0d   PASSED **");
        }*/
    }

    if (flexport_p->test_fail == 1) {
        rv = BCM_E_FAIL;
        cli_out("\n**  Flexport test   FAILED SECTION=%s START_FLEX_ID=%0d**\n",
                flexport_p->tpl_section, flexport_p->flex_id_start_param);
    } else {
        cli_out("\n**  Flexport test  PASSED ** SECTION=%s START_FLEX_ID=%0d\n\n",
                flexport_p->tpl_section, flexport_p->flex_id_start_param);
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
#endif /* BCM_TOMAHAWK3_SUPPORT */
