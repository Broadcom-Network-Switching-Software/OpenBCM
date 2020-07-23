/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_debug.h>
#include <phymod/chip/bcmi_tsce16_xgxs_defs.h>
#include "temod16_enum_defines.h"
#include "temod16.h"

#define PHYMOD_ST const phymod_access_t
#define _TRG_OPN(a) _TRGO_(a->addr, a->lane_mask)
#define _TRG_CLS(a) _TRGC_(a->addr, a->lane_mask)
#define _TRG_LANE(a) (a->lane_mask)

/*
 * Forward declarations
*/
int temod16_diag_link(PHYMOD_ST *pc);
int temod16_diag_autoneg_abilities(PHYMOD_ST *pc);
int tempd16_an_hcd_spd_get (uint16_t spd, char *mystr, int len);
int temod16_diag_autoneg_abilities(PHYMOD_ST *pc);
int temod16_diag_autoneg(PHYMOD_ST *pc);

int temod16_diag_link(PHYMOD_ST *pc)
{
    PMD_X1_STSr_t pmd_live_status;
    PMD_X4_STSr_t pmd_lane_live_status;
    PMD_X4_LATCH_STSr_t pmd_lane_latched_status[4];
    RX_X4_PCS_LIVE_STSr_t pcs_live_status;
    RX_X4_PCS_LATCH_STS0r_t pcs_latched_status;
    phymod_access_t *lpc = (phymod_access_t*)pc;

    int i,rx_lock_sts[4],sig_det_sts[4], this_lane;

    /*1. pmd_live_status  */
    READ_PMD_X1_STSr(pc, &pmd_live_status);

    /*2a. pmd_lane_live_status  */
    /*2b. pmd_lane_latched_status  */

    this_lane = _TRG_LANE(lpc);
    for(i=0; i < 4; i++) {
        _TRG_LANE(lpc) = 1<<i;
        READ_PMD_X4_STSr(lpc, &pmd_lane_live_status);
        sig_det_sts[i] = PMD_X4_STSr_SIGNAL_DETECT_STSf_GET(pmd_lane_live_status);
        rx_lock_sts[i] = PMD_X4_STSr_RX_LOCK_STSf_GET(pmd_lane_live_status);

        PMD_X4_LATCH_STSr_CLR(pmd_lane_latched_status[i]);
        READ_PMD_X4_LATCH_STSr(pc, &pmd_lane_latched_status[i]);
    }
    _TRG_LANE(lpc) = this_lane;

    /*3a. pcs_live_status  */
    /*3b. pcs_latched_status  */
    READ_RX_X4_PCS_LIVE_STSr(pc, &pcs_live_status);
    READ_RX_X4_PCS_LATCH_STS0r(pc, &pcs_latched_status);

    /* Print the information here */
    PHYMOD_DEBUG_ERROR(("+-----------------------+-------------------+---------------+---------+--------+\n"));
    PHYMOD_DEBUG_ERROR(("| TRG ADR : %06d LANE: %02d    LINK STATE                   |   LH    |   LL   |\n", pc->addr, pc->lane_mask));
    PHYMOD_DEBUG_ERROR(("+-----------------------+-------------------+---------------+---------+--------+\n"));
    PHYMOD_DEBUG_ERROR(("| PMD PLL LOCK   : %c    | PCS SYNC   : %c    | PCS SYNC STAT : %04d    : %04d   |\n",
        (PMD_X1_STSr_PLL_LOCK_STSf_GET(pmd_live_status) == 1) ? 'Y' : 'N',
        (RX_X4_PCS_LIVE_STSr_SYNC_STATUSf_GET(pcs_live_status) == 1) ? 'Y' : 'N',
        RX_X4_PCS_LATCH_STS0r_SYNC_STATUS_LHf_GET(pcs_latched_status),
        RX_X4_PCS_LATCH_STS0r_SYNC_STATUS_LLf_GET(pcs_latched_status)));

    PHYMOD_DEBUG_ERROR(("| PMD SIG DETECT : %c%c%c%c | PCS LINK   : %c    | PCS LINK STAT : %04d    : %04d   |\n",
        (sig_det_sts[3] == 1)? 'Y':'N',
        (sig_det_sts[2] == 1)? 'Y':'N',
        (sig_det_sts[1] == 1)? 'Y':'N',
        (sig_det_sts[0] == 1)? 'Y':'N',
        (RX_X4_PCS_LIVE_STSr_LINK_STATUSf_GET(pcs_live_status) == 1) ? 'Y' : 'N',
        RX_X4_PCS_LATCH_STS0r_LINK_STATUS_LHf_GET(pcs_latched_status),
        RX_X4_PCS_LATCH_STS0r_LINK_STATUS_LLf_GET(pcs_latched_status)));

    PHYMOD_DEBUG_ERROR(("| PMD LOCKED     : %c%c%c%c | PCS HI BER : %04d | PCS HIGH BER  : %04d    : %04d   |\n",
        (rx_lock_sts[3] == 1)? 'Y':'N',
        (rx_lock_sts[2] == 1)? 'Y':'N',
        (rx_lock_sts[1] == 1)? 'Y':'N',
        (rx_lock_sts[0] == 1)? 'Y':'N',
        RX_X4_PCS_LIVE_STSr_HI_BERf_GET(pcs_live_status),
        RX_X4_PCS_LATCH_STS0r_HI_BER_LLf_GET(pcs_latched_status),
        RX_X4_PCS_LATCH_STS0r_HI_BER_LHf_GET(pcs_latched_status)));

    PHYMOD_DEBUG_ERROR(("| PMD LATCH HI   : %d%d%d%d | PCS DESKEW : %04d | PCS DESKEW    : %04d    : %04d   |\n",
        PMD_X4_LATCH_STSr_SIGNAL_DETECT_LHf_GET(pmd_lane_latched_status[3]),
        PMD_X4_LATCH_STSr_SIGNAL_DETECT_LHf_GET(pmd_lane_latched_status[2]),
        PMD_X4_LATCH_STSr_SIGNAL_DETECT_LHf_GET(pmd_lane_latched_status[1]),
        PMD_X4_LATCH_STSr_SIGNAL_DETECT_LHf_GET(pmd_lane_latched_status[0]),
        RX_X4_PCS_LIVE_STSr_DESKEW_STATUSf_GET(pcs_live_status),
        RX_X4_PCS_LATCH_STS0r_DESKEW_STATUS_LLf_GET(pcs_latched_status),
        RX_X4_PCS_LATCH_STS0r_DESKEW_STATUS_LHf_GET(pcs_latched_status)));

    PHYMOD_DEBUG_ERROR(("| PMD LATCH LO   : %d%d%d%d | PCS AMLOCK : %04d | PCS AM LOCK   : %04d    : %04d   |\n",
        PMD_X4_LATCH_STSr_SIGNAL_DETECT_LLf_GET(pmd_lane_latched_status[3]),
        PMD_X4_LATCH_STSr_SIGNAL_DETECT_LLf_GET(pmd_lane_latched_status[2]),
        PMD_X4_LATCH_STSr_SIGNAL_DETECT_LLf_GET(pmd_lane_latched_status[1]),
        PMD_X4_LATCH_STSr_SIGNAL_DETECT_LLf_GET(pmd_lane_latched_status[0]),
        RX_X4_PCS_LIVE_STSr_AM_LOCKf_GET(pcs_live_status),
        RX_X4_PCS_LATCH_STS0r_AM_LOCK_LLf_GET(pcs_latched_status),
        RX_X4_PCS_LATCH_STS0r_AM_LOCK_LHf_GET(pcs_latched_status)));

    PHYMOD_DEBUG_ERROR(("| RXLOCK LATCH HI: %d%d%d%d |                   |                                  |\n",
        PMD_X4_LATCH_STSr_RX_LOCK_LHf_GET(pmd_lane_latched_status[3]),
        PMD_X4_LATCH_STSr_RX_LOCK_LHf_GET(pmd_lane_latched_status[2]),
        PMD_X4_LATCH_STSr_RX_LOCK_LHf_GET(pmd_lane_latched_status[1]),
        PMD_X4_LATCH_STSr_RX_LOCK_LHf_GET(pmd_lane_latched_status[0])));

    PHYMOD_DEBUG_ERROR(("| RXLOCK LATCH LO: %d%d%d%d |                   |                                  |\n",
        PMD_X4_LATCH_STSr_RX_LOCK_LLf_GET(pmd_lane_latched_status[3]),
        PMD_X4_LATCH_STSr_RX_LOCK_LLf_GET(pmd_lane_latched_status[2]),
        PMD_X4_LATCH_STSr_RX_LOCK_LLf_GET(pmd_lane_latched_status[1]),
        PMD_X4_LATCH_STSr_RX_LOCK_LLf_GET(pmd_lane_latched_status[0])));

    PHYMOD_DEBUG_ERROR(("+-----------------------+-------------------+----------------------------------+\n"));

    return PHYMOD_E_NONE;
}

int temod16_diag_autoneg_abilities(PHYMOD_ST *pc)
{
    AN_X4_ENSr_t                      an_ena;
    AN_X4_LOC_DEV_CL37_BASE_ABILr_t   cl37_base;
    AN_X4_LOC_DEV_CL37_BAM_ABILr_t    cl37_bam;
    AN_X4_LOC_DEV_OVER1G_ABIL0r_t     over1g;
    AN_X4_LOC_DEV_OVER1G_ABIL1r_t     over1g1;
    AN_X4_LOC_DEV_CL73_BAM_ABILr_t    cl73_bam;
    AN_X4_LOC_DEV_CL73_BASE_ABIL0r_t  cl73_base_0;
    AN_X4_LOC_DEV_CL73_BASE_ABIL1r_t  cl73_base_1;
    AN_X4_CTLSr_t                     an_ctrl;
    uint16_t                          sgmii_spd, lanes, base_sel;

    READ_AN_X4_ENSr                     (pc, &an_ena);
    READ_AN_X4_LOC_DEV_CL37_BASE_ABILr  (pc, &cl37_base);
    READ_AN_X4_LOC_DEV_CL37_BAM_ABILr   (pc, &cl37_bam);
    READ_AN_X4_LOC_DEV_OVER1G_ABIL0r    (pc, &over1g);
    READ_AN_X4_LOC_DEV_OVER1G_ABIL1r    (pc, &over1g1);
    READ_AN_X4_LOC_DEV_CL73_BAM_ABILr   (pc, &cl73_bam);
    READ_AN_X4_LOC_DEV_CL73_BASE_ABIL0r (pc, &cl73_base_0);
    READ_AN_X4_LOC_DEV_CL73_BASE_ABIL1r (pc, &cl73_base_1);
    READ_AN_X4_CTLSr                    (pc, &an_ctrl);

    PHYMOD_DEBUG_ERROR(("+-------------------+-------+------+-----------------------+-------------------+\n"));
    PHYMOD_DEBUG_ERROR(("| ANX4 CTRL: 0x%04X | ENS : 0x%04X | CL37 BAM:0x%04X BASE :0x%04X              |\n",
        AN_X4_CTLSr_GET(an_ctrl), AN_X4_ENSr_GET(an_ena),
        AN_X4_LOC_DEV_CL37_BAM_ABILr_GET(cl37_bam), AN_X4_LOC_DEV_CL37_BASE_ABILr_GET(cl37_base)));

    PHYMOD_DEBUG_ERROR(("| ANX4 OVR0: 0x%04X | OVR1: 0x%04X | CL73 BAM:0x%04X BASE1:0x%04X BASE0:0x%04X |\n",
        AN_X4_LOC_DEV_OVER1G_ABIL0r_GET(over1g), AN_X4_LOC_DEV_OVER1G_ABIL1r_GET(over1g1),
        AN_X4_LOC_DEV_CL73_BAM_ABILr_GET(cl73_bam), AN_X4_LOC_DEV_CL73_BASE_ABIL0r_GET(cl73_base_0),
        AN_X4_LOC_DEV_CL73_BASE_ABIL1r_GET(cl73_base_1)));

    lanes = AN_X4_ENSr_NUM_ADVERTISED_LANESf_GET(an_ena);
    PHYMOD_DEBUG_ERROR(("+-------------------+--------------+----+--------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("|      CLAUSE 37    |      CLAUSE 73    |                                      |\n"));
    PHYMOD_DEBUG_ERROR(("+-------------------+-------------------+--------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("| BAM ENA       : %d | BAM  ENA     : %d  | NUM ADV LANES : %d                    |\n",
        AN_X4_ENSr_CL37_BAM_ENABLEf_GET(an_ena),
        AN_X4_ENSr_CL73_BAM_ENABLEf_GET(an_ena),
        (lanes == 0? 1 : (lanes == 1? 2 : (lanes == 3? 10 : 1)))));

    PHYMOD_DEBUG_ERROR(("| AN  ENA       : %d | AN   ENA     : %d  | FAIL COUNT LIM: %d                    |\n",
        AN_X4_ENSr_CL37_ENABLEf_GET(an_ena),
        AN_X4_ENSr_CL73_ENABLEf_GET(an_ena),
        AN_X4_CTLSr_AN_FAIL_COUNT_LIMITf_GET(an_ctrl)));

    PHYMOD_DEBUG_ERROR(("| SGMII ENA     : %d | HPAM ENA     : %d  |                                      |\n",
        AN_X4_ENSr_CL37_SGMII_ENABLEf_GET(an_ena),
        AN_X4_ENSr_CL73_HPAM_ENABLEf_GET(an_ena)));

    PHYMOD_DEBUG_ERROR(("| BAM2SGMII ENA : %d | BAM3HPAM ENA : %d  |                                      |\n",
        AN_X4_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_GET(an_ena),
        AN_X4_ENSr_CL73_BAM_TO_HPAM_AUTO_ENABLEf_GET(an_ena)));

    PHYMOD_DEBUG_ERROR(("| SGMII2CL37 ENA: %d | HPAM2CL73 ENA: %d  |                                      |\n",
        AN_X4_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_GET(an_ena),
        AN_X4_ENSr_HPAM_TO_CL73_AUTO_ENABLEf_GET(an_ena)));

    PHYMOD_DEBUG_ERROR(("| AN RESTART    : %d | AN RESTART   : %d  |                                      |\n",
        AN_X4_ENSr_CL37_AN_RESTARTf_GET(an_ena),
        AN_X4_ENSr_CL73_AN_RESTARTf_GET(an_ena)));

    sgmii_spd = AN_X4_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_GET(cl37_base);
    base_sel  = AN_X4_LOC_DEV_CL73_BASE_ABIL1r_BASE_SELECTORf_GET(cl73_base_1);

    PHYMOD_DEBUG_ERROR(("+-------------------+-------------------+--------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("|            CL37 ABILITIES             |         CL73 ABILITIES               |\n"));
    PHYMOD_DEBUG_ERROR(("+---------------+-----------------------+-----------------+--------------------+\n"));
    PHYMOD_DEBUG_ERROR(("| SWRST DIS : %d | ANRST DIS    :%d       | NONCE OVR : %d   | NONCE VAL: %d       |\n",
        AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_GET(cl37_base),
        AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_GET(cl37_base),
        AN_X4_LOC_DEV_CL73_BASE_ABIL1r_CL73_NONCE_MATCH_OVERf_GET(cl73_base_1),
        AN_X4_LOC_DEV_CL73_BASE_ABIL1r_CL73_NONCE_MATCH_VALf_GET(cl73_base_1)));

    PHYMOD_DEBUG_ERROR(("| PD 2 CL37 : %d | NEXT PAGE    :%d       | TX NONCE  : 0x%X| BASE SEL : %-7s |\n",
        AN_X4_LOC_DEV_CL37_BASE_ABILr_AN_PD_TO_CL37_ENABLEf_GET(cl37_base),
        AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_GET(cl37_base),
        AN_X4_LOC_DEV_CL73_BASE_ABIL1r_TRANSMIT_NONCEf_GET(cl73_base_1),
        (base_sel == 0? "Rsvd" :
        (base_sel == 1? "802.3" :
        (base_sel == 2? "802.9" :
        (base_sel == 3? "802.5":
        (base_sel == 4? "1394": "UNKNOWN"))))) ));

    PHYMOD_DEBUG_ERROR(("| HALF DUPLX: %d | FULL DUPLEX  :%d       | NEXT PAGE  : %d  | FEC      : %d       |\n",
        AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_GET(cl37_base),
        AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_HALF_DUPLEXf_GET(cl37_base),
        AN_X4_LOC_DEV_CL73_BASE_ABIL0r_NEXT_PAGEf_GET(cl73_base_0),
        AN_X4_LOC_DEV_CL73_BASE_ABIL0r_FECf_GET(cl73_base_0)));

    PHYMOD_DEBUG_ERROR(("| PAUSE     : %d | SGMII MASTER :%d       | REMOTE FLT : %d  | PAUSE    : %d       |\n",
        AN_X4_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_GET(cl37_base),
        AN_X4_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_GET(cl37_base),
        AN_X4_LOC_DEV_CL73_BASE_ABIL0r_CL73_REMOTE_FAULTf_GET(cl73_base_0),
        AN_X4_LOC_DEV_CL73_BASE_ABIL0r_CL73_PAUSEf_GET(cl73_base_0)));

    PHYMOD_DEBUG_ERROR(("| SGMII FDUP: %d | SGMII SPD    :%-7s |-----------------+--------------------|\n",
        AN_X4_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_GET(cl37_base),
        (sgmii_spd == 0? "10Mb/s" :
        (sgmii_spd == 1? "100Mb/s":
        (sgmii_spd == 2? "1Gb/s" : "Rsvd"))) ));

    PHYMOD_DEBUG_ERROR(("| OVR1G ABIL: %d | OVR1G PAGECNT:%d       | 1000BASE KX : %d | 10GBASE KX4 :%d     |\n",
        AN_X4_LOC_DEV_CL37_BAM_ABILr_OVER1G_ABILITYf_GET(cl37_bam),
        AN_X4_LOC_DEV_CL37_BAM_ABILr_OVER1G_PAGE_COUNTf_GET(cl37_bam),
        AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_1000BASE_KXf_GET(cl73_base_0),
        AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_10GBASE_KX4f_GET(cl73_base_0)));

    PHYMOD_DEBUG_ERROR(("| BAM CODE      : 0x%04X                | 10GBASE KR  : %d | 10GBASE KR4 :%d     |\n",
        AN_X4_LOC_DEV_CL37_BAM_ABILr_CL37_BAM_CODEf_GET(cl37_bam),
        AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_10GBASE_KRf_GET(cl73_base_0),
        AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_40GBASE_KR4f_GET(cl73_base_0)));

    PHYMOD_DEBUG_ERROR(("|---------------+-----------------------| 40GBASE CR4 : %d | 100GBASE CR1:%d     |\n",
        AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_40GBASE_CR4f_GET(cl73_base_0),
        AN_X4_LOC_DEV_CL73_BASE_ABIL0r_BASE_100GBASE_CR10f_GET(cl73_base_0)));

    PHYMOD_DEBUG_ERROR(("|                                       | HPAM_20GKR2 : %d | BAM CODE    :0x%04X|\n",
        AN_X4_LOC_DEV_CL73_BAM_ABILr_HPAM_20GKR2f_GET(cl73_bam),
        AN_X4_LOC_DEV_CL73_BAM_ABILr_CL73_BAM_CODEf_GET(cl73_bam)));

    PHYMOD_DEBUG_ERROR(("|                                       | 20GBASE CR2 : %d | 20GBASE KR2 : %d    |\n",
        AN_X4_LOC_DEV_CL73_BAM_ABILr_BAM_20GBASE_CR2f_GET(cl73_bam),
        AN_X4_LOC_DEV_CL73_BAM_ABILr_BAM_20GBASE_KR2f_GET(cl73_bam)));

    PHYMOD_DEBUG_ERROR(("+---------------------------------------+--------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("|                               OVER1G ABILITIES                               |\n"));
    PHYMOD_DEBUG_ERROR(("+-------------------+--------------------+-----------------+-------------------+\n"));
    PHYMOD_DEBUG_ERROR(("| HG2         : %d   | FEC          : %d   | CL72         : %d|                   |\n",
        AN_X4_LOC_DEV_OVER1G_ABIL1r_HG2f_GET(over1g1),
        AN_X4_LOC_DEV_OVER1G_ABIL1r_FECf_GET(over1g1),
        AN_X4_LOC_DEV_OVER1G_ABIL1r_CL72f_GET(over1g1)));

    PHYMOD_DEBUG_ERROR(("| 40GBASE X4  : %d   | 32P7GBASE X4 : %d   | 31P5GBASE X4 : %d| 25P455GBASE X4: %d |\n",
        AN_X4_LOC_DEV_OVER1G_ABIL1r_BAM_40GBASE_X4f_GET(over1g1),
        AN_X4_LOC_DEV_OVER1G_ABIL1r_BAM_32P7GBASE_X4f_GET(over1g1),
        AN_X4_LOC_DEV_OVER1G_ABIL1r_BAM_31P5GBASE_X4f_GET(over1g1),
        AN_X4_LOC_DEV_OVER1G_ABIL1r_BAM_25P455GBASE_X4f_GET(over1g1)));

    PHYMOD_DEBUG_ERROR(("| 21GBASE X4  : %d   | 20GBASEX2 CX4: %d   | 20GBASE X2   : %d| 20GBASE X4    : %d |\n",
        AN_X4_LOC_DEV_OVER1G_ABIL1r_BAM_21GBASE_X4f_GET(over1g1),
        AN_X4_LOC_DEV_OVER1G_ABIL1r_BAM_20GBASE_X2_CX4f_GET(over1g1),
        AN_X4_LOC_DEV_OVER1G_ABIL1r_BAM_20GBASE_X2f_GET(over1g1),
        AN_X4_LOC_DEV_OVER1G_ABIL1r_BAM_20GBASE_X4f_GET(over1g1)));

    PHYMOD_DEBUG_ERROR(("| 16GBASE X4  : %d   | 15P75GBASE X2: %d   | 15GBASE X4   : %d| 13GBASE X4    : %d |\n",
        AN_X4_LOC_DEV_OVER1G_ABIL1r_BAM_16GBASE_X4f_GET(over1g1),
        AN_X4_LOC_DEV_OVER1G_ABIL1r_BAM_15P75GBASE_X2f_GET(over1g1),
        AN_X4_LOC_DEV_OVER1G_ABIL1r_BAM_15GBASE_X4f_GET(over1g1),
        AN_X4_LOC_DEV_OVER1G_ABIL1r_BAM_13GBASE_X4f_GET(over1g1)));

    return PHYMOD_E_NONE;
}

int tempd16_an_hcd_spd_get (uint16_t spd, char *mystr, int len)
{
    char *an_spd = "UNDEF";

    if (spd == 1) an_spd ="10M";
    if (spd == 2) an_spd ="100M";
    if (spd == 3) an_spd ="1000M";
    if (spd == 4) an_spd ="1G_CX1";
    if (spd == 5) an_spd ="1G_KX1";
    if (spd == 6) an_spd ="2p5G_X1";
    if (spd == 7) an_spd ="5G_X1";
    if (spd == 8) an_spd ="10G_CX4";
    if (spd == 9) an_spd ="10G_KX4";
    if (spd == 0xa) an_spd ="10G_X4";
    if (spd == 0xb) an_spd ="13G_X4";
    if (spd == 0xc) an_spd ="15G_X4";
    if (spd == 0xd) an_spd ="16G_X4";
    if (spd == 0xe) an_spd ="20G_CX4";
    if (spd == 0xf) an_spd ="10G_CX2";
    if (spd == 0x10) an_spd ="10G_X2";
    if (spd == 0x11) an_spd ="20G_X4";
    if (spd == 0x12) an_spd ="10p5G_X2";
    if (spd == 0x13) an_spd ="21G_X4";
    if (spd == 0x14) an_spd ="12p7G_X2";
    if (spd == 0x15) an_spd ="25p45G_X4";
    if (spd == 0x16) an_spd ="15p75G_X2";
    if (spd == 0x17) an_spd ="31P5G_X4";
    if (spd == 0x18) an_spd ="31p5G_KR4";
    if (spd == 0x19) an_spd ="20G_CX2";
    if (spd == 0x1a) an_spd ="20G_X2";
    if (spd == 0x1b) an_spd ="40G_X4";
    if (spd == 0x1c) an_spd ="10G_KR1";
    if (spd == 0x1d) an_spd ="10p6_X1";
    if (spd == 0x1e) an_spd ="20G_KR2";
    if (spd == 0x1f) an_spd ="20G_CR2";
    if (spd == 0x20) an_spd ="21G_X2";
    if (spd == 0x21) an_spd ="40G_KR4";
    if (spd == 0x22) an_spd ="40G_CR4";
    if (spd == 0x23) an_spd ="42G_X4";
    if (spd == 0x24) an_spd ="100G_CR10";
    if (spd == 0x25) an_spd ="107G_X10";
    if (spd == 0x26) an_spd ="120G_X12";
    if (spd == 0x27) an_spd ="127G_X12";
    if (spd == 0x31) an_spd ="5G_KR1";
    if (spd == 0x32) an_spd ="10p5G_X4";
    if (spd == 0x35) an_spd ="10M_10p3125";
    if (spd == 0x36) an_spd ="100M_10p3125";
    if (spd == 0x37) an_spd ="1000M_10p3125";
    if (spd == 0x38) an_spd ="2p5G_X1_10p3125";
    if (spd == 0x39) an_spd ="10G_KX4_10p3125";
    if (spd == 0x3a) an_spd ="10G_CX4_10p3125";
    if (spd == 0x3b) an_spd ="10G_X4_10p3125";

    PHYMOD_STRNCPY(mystr, an_spd, len);

    return PHYMOD_E_NONE;
}

int temod16_diag_autoneg(PHYMOD_ST *pc)
{
    AN_X4_AN_PAGE_SEQUENCER_STSr_t an_page_seq_status;
    AN_X4_AN_ABIL_RESOLUTION_STSr_t an_hcd_status;
    AN_X4_AN_MISC_STSr_t an_misc_status;
    char  an_spd[16];

    READ_AN_X4_AN_PAGE_SEQUENCER_STSr(pc,&an_page_seq_status);
    READ_AN_X4_AN_ABIL_RESOLUTION_STSr(pc,&an_hcd_status);
    READ_AN_X4_AN_MISC_STSr(pc,&an_misc_status);
    tempd16_an_hcd_spd_get (AN_X4_AN_ABIL_RESOLUTION_STSr_AN_HCD_SPEEDf_GET(an_hcd_status), an_spd, sizeof(an_spd));

    PHYMOD_DEBUG_ERROR(("| TRG ADR : %06d LANE: %02d     AUTONEG                                        |\n",
              pc->addr, pc->lane_mask));
    PHYMOD_DEBUG_ERROR(("+-------------+-------------+------------------------------+-------------------+\n"));
    PHYMOD_DEBUG_ERROR(("| AN37: %c     | AN73 : %c    | AN HCD SPD : %-16s|  AN LINK : %s     |\n",
        (AN_X4_AN_PAGE_SEQUENCER_STSr_AN_ST_CL37_COMPLETEf_GET(an_page_seq_status) == 1) ? 'Y' : 'N',
        (AN_X4_AN_PAGE_SEQUENCER_STSr_AN_ST_CL73_COMPLETEf_GET(an_page_seq_status) == 1) ? 'Y' : 'N', an_spd,
        (((AN_X4_AN_PAGE_SEQUENCER_STSr_AN_ST_CL73_COMPLETEf_GET(an_page_seq_status) == 1) ||
        (AN_X4_AN_PAGE_SEQUENCER_STSr_AN_ST_CL37_COMPLETEf_GET(an_page_seq_status) == 1)) &&
        (AN_X4_AN_MISC_STSr_AN_COMPLETEf_GET(an_misc_status) == 1))? "UP":"DN"));

    temod16_diag_autoneg_abilities(pc);
    PHYMOD_DEBUG_ERROR(("+-------------------+--------------------+-----------------+-------------------+\n"));

    return PHYMOD_E_NONE;
}

int temod16_diag_disp(PHYMOD_ST *pc, const char* cmd_str)
{
    temod16_diag_link(pc);
    temod16_diag_autoneg(pc);
    return PHYMOD_E_NONE;
}
