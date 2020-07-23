/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <sal/types.h>
#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

/* These are the Tier1 functions */

/* special handle for MDK */
#define WCMOD_INIT              "WCMOD_INIT"

#define MASTER_SLAVE_CONTROL     "MASTER_SLAVE_CONTROL"
#define SLAVE_WAKEUP_CONTROL     "SLAVE_WAKEUP_CONTROL"
#define RX_LOOPBACK_PCSBYP       "RX_LOOPBACK_PCS_BYPASS"
#define PCS_BYPASS_CTL           "PCS_BYPASS_CTL"
#define TX_TAP_CONTROL           "TX_TAP_CONTROL"
#define TX_PI_CONTROL            "TX_PI_CONTROL"
#define TX_AMP_CONTROL           "TX_AMP_CONTROL"
#define RX_P1_SLICER_CONTROL     "RX_P1_SLICER_CONTROL"
#define RX_M1_SLICER_CONTROL     "RX_M1_SLICER_CONTROL"
#define RX_D_SLICER_CONTROL      "RX_D_SLICER_CONTROL"
#define REFCLK_SET               "REFCLK_SET"
#define ASYMMETRIC_MODE_SET      "ASYMMETRIC_MODE_SET"
#define SPD_INTF_CONTROL         "SPD_INTF_CONTROL"
/* put an _ since this begins with a number */
#define _100FX_CONTROL           "100FX_CONTROL"
#define TX_LANE_DISABLE          "TX_LANE_DISABLE"
#define DUPLEX_CONTROL           "DUPLEX_CONTROL"
#define POWER_CONTROL            "POWER_CONTROL"
#define AUTONEG_SET              "AUTONEG_SET"
#define AUTONEG_CONTROL          "AUTONEG_CONTROL"
#define AUTONEG_PAGE_SET         "AUTONEG_PAGE_SET"
#define ANEG_CONTROL             "ANEG_CONTROL"
#define CARRIER_EXTENSION        "CARRIER_EXTENSION"
#define REG_READ                 "REG_READ"
#define REG_WRITE                "REG_WRITE"
#define PRBS_CHECK               "PRBS_CHECK"
#define CJPAT_CRPAT_CONTROL      "CJPAT_CRPAT_CONTROL"
#define CJPAT_CRPAT_CHECK        "CJPAT_CRPAT_CHECK"
#define WCMOD_DIAG               "WCMOD_DIAG"
#define LANE_SWAP                "LANE_SWAP"
#define PARALLEL_DETECT_CONTROL  "PARALLEL_DETECT_CONTROL"
#define CLAUSE_72_CONTROL        "CLAUSE_72_CONTROL"
#define PLL_SEQUENCER_CONTROL    "PLL_SEQUENCER_CONTROL"
#define PROG_DATA                "PROG_DATA"
#define REVID_READ               "REVID_READ"
#define RX_SEQ_CONTROL           "RX_SEQ_CONTROL"
#define SET_POLARITY             "SET_POLARITY"
#define SET_PORT_MODE            "SET_PORT_MODE"
#define PRBS_CONTROL             "PRBS_CONTROL"
#define PRBS_DECOUPLE_CONTROL    "PRBS_DECOUPLE_CONTROL"
#define SCRAMBLER_CONTROL        "SCRAMBLER_CONTROL"
#define RX_PF_CONTROL            "RX_PF_CONTROL"
#define RX_LOW_FREQ_PF_CONTROL   "RX_LOW_FREQ_PF_CONTROL"
#define RX_DFE_TAP1_CONTROL      "RX_DFE_TAP1_CONTROL"
#define RX_DFE_TAP2_CONTROL      "RX_DFE_TAP2_CONTROL"
#define RX_DFE_TAP3_CONTROL      "RX_DFE_TAP3_CONTROL"
#define RX_DFE_TAP4_CONTROL      "RX_DFE_TAP4_CONTROL"
#define RX_DFE_TAP5_CONTROL      "RX_DFE_TAP5_CONTROL"
#define RX_VGA_CONTROL           "RX_VGA_CONTROL"        
#define SOFT_RESET               "SOFT_RESET"
#define TX_RESET                 "TX_RESET"
#define RX_RESET                 "RX_RESET"
#define AER_LANE_SELECT          "AER_LANE_SELECT"
#define SET_SPD_INTF             "SET_SPD_INTF"
#define TX_BERT_CONTROL          "TX_BERT_CONTROL"
#define RX_LOOPBACK_CONTROL      "RX_LOOPBACK_CONTROL"
#define TX_LOOPBACK_CONTROL      "TX_LOOPBACK_CONTROL"
#define PORT_STAT_DISPLAY        "PORT_STAT_DISPLAY"
#define CORE_RESET               "CORE_RESET"
#define HGSOLO                   "HGSOLO"
#define FIRMWARE_MODE_SET        "FIRMWARE_MODE_SET"
#define EEE_PASS_THRU_SET        "EEE_PASS_THRU_SET"


#define _100G_MASTER             "100G_MASTER"
#define _100G_MASTER_ANEG        "100G_MASTER_ANEG"
#define _100G_SLAVE              "100G_SLAVE"
#define _100G_SLAVE_2LANES       "100G_SLAVE_2LANES"
#define _100G_SLAVE_3LANES       "100G_SLAVE_3LANES"
#define _100G_SLAVE_4LANES       "100G_SLAVE_4LANES"
#define _1_2p5G_1100             "1_2p5G_1100"
#define _1_2p5G_1010             "1_2p5G_1010"

/* Diagnostic functions */
#define WCMOD_DIAG         "WCMOD_DIAG"

#define SCRAMBLER_CONTROL  "SCRAMBLER_CONTROL"
  
/* These are the Tier2+ functions */

#define _100M_MII                    "100M_MII"
#define _PCSBYP_6P25                 "PCSBYP_6P25"
#define XFI                          "XFI"
#define _1G_GMII_COMBO               "1G_GMII_COMBO"
#define _1G_SGMII_COMBO              "1G_SGMII_COMBO"
#define _1G_GMII                     "1G_GMII"
#define _1G_SGMII                    "1G_SGMII"
#define _2p5G                        "2.5G"
#define _10G_XAUI                    "10G_XAUI"
#define _10G_HI                      "10G_HI"
#define _10G_X2                      "10G_X2"
#define _10G_HI_DXGXS                "10G_HI_DXGXS"
#define _10G_DXGXS                   "10G_DXGXS"
#define _10G_HI_DXGXS_SCR            "10G_HI_DXGXS_SCR"
#define _10p5G_HI_DXGXS              "10.5G_HI_DXGXS"
#define _10p5G_DXGXS                 "10.5G_DXGXS"
#define _12G_HI_DXGXS                "12G_HI_DXGXS"
#define _12G_R2                      "12G_R2"
#define _12G_DXGXS                   "12G_DXGXS"
#define _15G_DXGXS                   "15G_DXGXS"
#define _20G_DXGXS                   "20G_DXGXS"
#define _21G_DXGXS                   "21G_DXGXS"
#define _20G_CR2                     "20G_CR2"
#define _21G_XAUI                    "21G_XAUI"
#define _25G_XAUI                    "25G_XAUI"
#define _31p5G_XAUI                  "31.5G_XAUI"
#define _40G_XAUI                    "40G_XAUI"
#define _40G_XLAUI                   "40G_XLAUI"
#define _42G_XLAUI                   "42G_XLAUI"
#define _40G_KR4                     "40G_KR4"
#define _42G_KR4                     "42G_KR4"



#define PRBS7_ENABLE           "PRBS7_ENABLE"
#define PRBS15_ENABLE          "PRBS15_ENABLE"
#define PRBS23_ENABLE          "PRBS23_ENABLE"
#define PRBS31_ENABLE          "PRBS31_ENABLE"
#define PRBS_DISABLE           "PRBS_DISABLE"
/*PRBS_CHECK is also a Tier2 function */
#define GLOOP_ON               "GLOOP_ON"
#define GLOOP_OFF              "GLOOP_OFF" 
 

#define CARRIER_EXTENSION_ENABLED       "CARRIER_EXTENSION_ENABLED"
#define CARRIER_EXTENSION_DISABLED      "CARRIER_EXTENSION_DISABLED"

#endif
