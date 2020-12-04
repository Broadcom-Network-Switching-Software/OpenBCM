/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * Copyright: (c) 2012 Broadcom Corp.
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : temod_diagnostics.c
 * Description: C sequences which utilize tier1s to build configurations
 *              which target only the diagnostic features of the Serdes
 *-----------------------------------------------------------------------*/

#ifndef _DV_TB_
#define _SDK_TEMOD_ 1
#endif 

#ifdef _DV_TB_
#include "temod_phyreg.h"
#include <stdint.h>
#include "temod_main.h"
#include "temod_defines.h"
#include "temod_phyreg.h"
#include "tePCSRegEnums.h"
#include "tePMDRegEnums.h"
#include "phy_tsc_iblk.h"
#include "bcmi_tsce_xgxs_defs.h"
#endif

#ifdef _SDK_TEMOD_
#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_config.h>
#include <phymod/phymod_debug.h>
#include <phymod/chip/bcmi_tsce_xgxs_defs.h>
#include "temod_enum_defines.h"
#include "temod.h"
#include "temod_sc_lkup_table.h"
#include "tePCSRegEnums.h"
#endif

#define _TRGO_(a,l) PHYMOD_DEBUG_ERROR(("<< PHY_TRG: Adr:%06d Ln:%02d\n",a,l));
#define _TRGC_(a,l) PHYMOD_DEBUG_ERROR((">> PHY TRG: Adr:%06d Ln:%02d\n",a,l));
#ifdef _SDK_TEMOD_
  #define PHYMOD_ST const phymod_access_t
  #define _TRG_OPN(a) _TRGO_(a->addr, a->lane_mask)
  #define _TRG_CLS(a) _TRGC_(a->addr, a->lane_mask)
  #define _TRG_LANE(a) (a->lane_mask)
#endif
#ifdef _DV_TB_
  #define PHYMOD_ST  temod_st
  #define _TRG_OPN(a) _TRGO_(a->prt_ad, a->this_lane)
  #define _TRG_CLS(a) _TRGC_(a->prt_ad, a->this_lane)
  #define _TRG_LANE(a) (a->this_lane)
#endif

int temod_diag_link(PHYMOD_ST *pc);
int temod_diag_st(PHYMOD_ST *pc, int resolved_speed);

/**
@brief   Field OVR diagnostic function
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE
@details Field OVR Details
   Check if the field OVR is set, and if OVR enabled, print the OVR value
*/

int temod_diag_field_or(PHYMOD_ST *pc)
{
  SC_X4_FLD_OVRR_EN0_TYPEr_t field_or_0;
  SC_X4_FLD_OVRR_EN1_TYPEr_t field_or_1;
  SC_X4_LN_NUM_OVRRr_t       num_lanes_or;
  RX_X4_PMA_CTL0r_t          os_or;
  TX_X4_MISCr_t              tx_misc_or;
  TX_X4_ENC0r_t              encode_or;
  RX_X4_PCS_CTL0r_t          rx_x4_pcs_ctrl_or;
  RX_X2_MISC0r_t             chk_end_or;
  RX_X4_DEC_CTL0r_t          blk_sync_or;
  CL72_LNK_CTLr_t            cl72_or;
  RX_X4_CL36_RX0r_t          cl36_or;
  TX_X4_CRED0r_t             crd_or;
  TX_X4_CRED1r_t             crd1_or;
  TX_X4_LOOPCNTr_t           loopcnt_or;
  TX_X4_MAC_CREDGENCNTr_t    mac_cgc_or;
  TX_X4_PCS_CLKCNT0r_t       pcs_crd_or;
  TX_X4_PCS_CREDGENCNTr_t    pcs_cgc_or;

  READ_CL72_LNK_CTLr(pc,&cl72_or);

  READ_SC_X4_FLD_OVRR_EN0_TYPEr(pc, &field_or_0);
  READ_SC_X4_FLD_OVRR_EN1_TYPEr(pc, &field_or_1);
  READ_SC_X4_LN_NUM_OVRRr(pc,&num_lanes_or);

  READ_RX_X4_PMA_CTL0r(pc,&os_or);
  READ_RX_X4_PCS_CTL0r(pc,&rx_x4_pcs_ctrl_or);
  READ_RX_X2_MISC0r(pc,&chk_end_or);
  READ_RX_X4_DEC_CTL0r(pc,&blk_sync_or);
  READ_RX_X4_CL36_RX0r(pc,&cl36_or);

  READ_TX_X4_MISCr(pc,&tx_misc_or);
  READ_TX_X4_ENC0r(pc,&encode_or);
  READ_TX_X4_CRED0r(pc,&crd_or);
  READ_TX_X4_CRED1r(pc,&crd1_or);
  READ_TX_X4_LOOPCNTr(pc,&loopcnt_or);
  READ_TX_X4_MAC_CREDGENCNTr(pc,&mac_cgc_or);
  READ_TX_X4_PCS_CLKCNT0r(pc,&pcs_crd_or);
  READ_TX_X4_PCS_CREDGENCNTr(pc,&pcs_cgc_or);

  PHYMOD_DEBUG_ERROR(("|                          OEN SET OVR VALUE                                   |\n"));
  PHYMOD_DEBUG_ERROR(("+----------------+----------------+--------------------------------------------+\n"));
  PHYMOD_DEBUG_ERROR(("| NUM LANES: 0x%x | FEC ENA  : 0x%X |                                            |\n",
    SC_X4_LN_NUM_OVRRr_NUM_LANES_OVERRIDE_VALUEf_GET(num_lanes_or),
	TX_X4_MISCr_FEC_ENABLEf_GET(tx_misc_or)));
	/* e2s_temod_os_mode_type[RX_X4_PMA_CTL0r_OS_MODEf_GET(os_or)]+6)); */

  PHYMOD_DEBUG_ERROR(("| 64B66DECR: 0x%x | CHKEND   : 0x%X | %-43s|\n",
	RX_X4_PCS_CTL0r_BRCM64B66_DESCRAMBLER_ENABLEf_GET(rx_x4_pcs_ctrl_or),
	RX_X2_MISC0r_CHK_END_ENf_GET(chk_end_or),
	e2s_temod_scr_mode[TX_X4_MISCr_SCR_MODEf_GET(tx_misc_or)]+6));

  PHYMOD_DEBUG_ERROR(("| FECENABL : 0x%x | REORDER  : 0x%X | %-43s|\n",
	CL72_LNK_CTLr_LINK_CONTROL_FORCEVALf_GET(cl72_or),
         RX_X4_CL36_RX0r_REORDER_ENf_GET(cl36_or),
	e2s_temod_encode_mode[TX_X4_ENC0r_ENCODEMODEf_GET(encode_or)]+6));

  PHYMOD_DEBUG_ERROR(("| CL36ENA  : 0x%x | SGMIISPD : 0x%X | %-43s|\n",
         RX_X4_CL36_RX0r_CL36_ENf_GET(cl36_or),
         TX_X4_CRED0r_SGMII_SPD_SWITCHf_GET(crd_or),
    e2s_temod_descrambler_mode[RX_X4_PCS_CTL0r_DESCRAMBLERMODEf_GET(rx_x4_pcs_ctrl_or)]+6));

  PHYMOD_DEBUG_ERROR(("| CLKCNT0  : 0x%x | CLKCNT1  : 0x%X | %-43s|\n",
         TX_X4_CRED0r_CLOCKCNT0f_GET(crd_or),
         TX_X4_CRED1r_CLOCKCNT1f_GET(crd1_or),
   e2s_temod_decoder_mode[RX_X4_PCS_CTL0r_DECODERMODEf_GET(rx_x4_pcs_ctrl_or)]+6));

  PHYMOD_DEBUG_ERROR(("| LP CNT0  : 0x%x | LP CNT1  : 0x%X | %-43s|\n",
         TX_X4_LOOPCNTr_LOOPCNT0f_GET(loopcnt_or),
         TX_X4_LOOPCNTr_LOOPCNT1f_GET(loopcnt_or),
    e2s_temod_deskew_mode[RX_X4_PCS_CTL0r_DESKEWMODEf_GET(rx_x4_pcs_ctrl_or)]+6));

  PHYMOD_DEBUG_ERROR(("| MACCRDGEN: 0x%x | REPLCNT  : 0x%X | %-43s|\n",
         TX_X4_MAC_CREDGENCNTr_MAC_CREDITGENCNTf_GET(mac_cgc_or),
         TX_X4_PCS_CLKCNT0r_REPLICATION_CNTf_GET(pcs_crd_or),
    e2s_temod_descrambler2_mode[RX_X4_PCS_CTL0r_DESC2_MODEf_GET(rx_x4_pcs_ctrl_or)]+6));

  PHYMOD_DEBUG_ERROR(("| PCSCRDENA: 0x%x | CLK CNT  : 0x%X | %-43s|\n",
         TX_X4_PCS_CLKCNT0r_PCS_CREDITENABLEf_GET(pcs_crd_or),
         TX_X4_PCS_CLKCNT0r_PCS_CLOCKCNT0f_GET(pcs_crd_or),
	e2s_temod_byte_del_mode[RX_X4_PCS_CTL0r_CL36BYTEDELETEMODEf_GET(rx_x4_pcs_ctrl_or)]+6));


  PHYMOD_DEBUG_ERROR(("| PCSCRDGEN: 0x%x |                | %-43s|\n",
         TX_X4_PCS_CREDGENCNTr_PCS_CREDITGENCNTf_GET(pcs_cgc_or),
	e2s_temod_blocksync_mode[RX_X4_DEC_CTL0r_BLOCK_SYNC_MODEf_GET(blk_sync_or)]+6));

/*
  if(TSCE_FIELD_OVERRIDE_ENABLE1_TYPEr_CREDITENABLE_OENf_GET(field_or_1) == 1) {
    PHYMOD_DEBUG_ERROR(("crediten_OEN set. ovr_value:0x%x\n",
         TSCE_TX_X4_CREDIT0r_CREDITENABLEf_GET(crd_or)));
  }
*/

  PHYMOD_DEBUG_ERROR(("+----------------+--+-------------+------+------------------+------------------+\n"));
  return PHYMOD_E_NONE;
}

/**
@brief   pmd pcs lane cfg diagnostic function
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   lane The lane under consideration (TBD)
@returns PHYMOD_E_NONE
@details Get the uc controls for pmd lane cfg
  Read the pcs_lane_cfg information from uc RAM. To check if the pmd is to be
  programmed from pcs lane cfg.
 */
int temod_diag_pmd_pcs_lane_cfg(PHYMOD_ST *pc, int lane)
{
  return PHYMOD_E_NONE;
} /* temod_diag_pmd_pcs_lane_cfg */

/**
@brief   Speed diagnostic function
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE 
@details Speed Details
 1. Get the Speed set
 2. Get the Speed valis and Speed Done Status
 3. Get the final stats of the speed
 */
int temod_diag_speed(PHYMOD_ST *pc)
{
  SC_X4_STSr_t sc_ctrl_status;
  SC_X4_RSLVD_SPDr_t sc_final_resolved_speed;
  SC_X4_RSLVD0r_t sc_final_resolved_0;
  SC_X4_RSLVD1r_t sc_final_resolved_1;
  SC_X4_RSLVD2r_t sc_final_resolved_2;
  SC_X4_RSLVD3r_t sc_final_resolved_3;
  SC_X4_RSLVD4r_t sc_final_resolved_4;
  SC_X4_RSLVD5r_t sc_final_resolved_5;
  SC_X4_RSLVD6r_t sc_final_resolved_6;
  SC_X4_RSLVD7r_t sc_final_resolved_7;
  SC_X4_RSLVD8r_t sc_final_resolved_8;
  MAIN0_SETUPr_t setup_reg;
  PLL_CAL_CTL7r_t pll_div_reg;
  CKRST_OSR_MODE_CTLr_t    reg_osr_mode;

  PHYMOD_DEBUG_ERROR(("| TRG ADR : %06d LANE: %02d    SPEED                                           |\n", pc->addr, pc->lane_mask));
  PHYMOD_DEBUG_ERROR(("+---------------------+-----------------------------+-----------+--------------+\n"));

  /*1. Get the Speed valid and Speed Done Status */
  READ_SC_X4_STSr(pc,&sc_ctrl_status);

  /*2. Get the Speed set */
  /*2a PLLDIV and the refclk settings */
  READ_SC_X4_RSLVD_SPDr(pc,&sc_final_resolved_speed);
  READ_MAIN0_SETUPr(pc,&setup_reg);
  READ_PLL_CAL_CTL7r(pc,&pll_div_reg);
  READ_CKRST_OSR_MODE_CTLr(pc, &reg_osr_mode);

  /*2b Check if the Speed id is existent in ST */
  temod_diag_st(pc, SC_X4_RSLVD_SPDr_SPEEDf_GET(sc_final_resolved_speed));
  temod_diag_field_or(pc);

  /*3. Get the final stats for the speed */
  READ_SC_X4_RSLVD0r(pc,&sc_final_resolved_0);
  READ_SC_X4_RSLVD2r(pc,&sc_final_resolved_2);


  READ_SC_X4_RSLVD1r(pc,&sc_final_resolved_1);

  READ_SC_X4_RSLVD3r(pc,&sc_final_resolved_3);
  READ_SC_X4_RSLVD4r(pc,&sc_final_resolved_4);
  READ_SC_X4_RSLVD5r(pc,&sc_final_resolved_5);
  READ_SC_X4_RSLVD6r(pc,&sc_final_resolved_6);
  READ_SC_X4_RSLVD7r(pc,&sc_final_resolved_7);
  READ_SC_X4_RSLVD8r(pc,&sc_final_resolved_8);


  PHYMOD_DEBUG_ERROR(("|        SPEED      |        STATS0      |      STATS1      |   CREDIT STATS   |\n"));
  PHYMOD_DEBUG_ERROR(("+-------------------+--------------------+------------------+------------------+\n"));
  PHYMOD_DEBUG_ERROR(("| SPD CHG VLD  %d    |     OS MODE %-6s | DESCR MODE %-6s| SGMII SPD : %04d |\n",
     SC_X4_STSr_SW_SPEED_CONFIG_VLDf_GET(sc_ctrl_status),
     e2s_temod_os_mode_type[CKRST_OSR_MODE_CTLr_OSR_MODE_FRC_VALf_GET(reg_osr_mode)]+18,
     e2s_temod_descrambler_mode[SC_X4_RSLVD1r_DESCRAMBLERMODEf_GET(sc_final_resolved_1)]+20,
     SC_X4_RSLVD3r_SGMII_SPD_SWITCHf_GET(sc_final_resolved_3)));

  PHYMOD_DEBUG_ERROR(("| SPD CHG DONE %d    |    SCR MODE %-6s |DECODE MODE %-6s|  CLK CNT0 : %04d |\n",
     SC_X4_STSr_SW_SPEED_CHANGE_DONEf_GET(sc_ctrl_status),
     e2s_temod_scr_mode[SC_X4_RSLVD0r_SCR_MODEf_GET(sc_final_resolved_0)]+15,
     e2s_temod_decoder_mode[SC_X4_RSLVD1r_DECODERMODEf_GET(sc_final_resolved_1)]+19,
     SC_X4_RSLVD3r_CLOCKCNT0f_GET(sc_final_resolved_3)));

  PHYMOD_DEBUG_ERROR(("| SPD RESOLVED %04d |    ENC MODE %-6s |DESKEW MODE %-6s|  CLK CNT1 : %04d |\n",
     SC_X4_RSLVD_SPDr_SPEEDf_GET(sc_final_resolved_speed),
     e2s_temod_encode_mode[SC_X4_RSLVD0r_ENCODEMODEf_GET(sc_final_resolved_0)]+18,
     e2s_temod_deskew_mode[SC_X4_RSLVD1r_DESKEWMODEf_GET(sc_final_resolved_1)]+20,
     SC_X4_RSLVD4r_CLOCKCNT1f_GET(sc_final_resolved_4)));

  PHYMOD_DEBUG_ERROR(("| #LN RESOLVED %04d |BLKSYNC MODE %-6s |DESCR2 MODE %-6s|   LP CNT0 : %04d |\n",
     SC_X4_RSLVD_SPDr_NUM_LANESf_GET(sc_final_resolved_speed),
     e2s_temod_blocksync_mode[SC_X4_RSLVD2r_BLOCK_SYNC_MODEf_GET(sc_final_resolved_2)]+21,
     e2s_temod_descrambler2_mode[SC_X4_RSLVD1r_DESC2_MODEf_GET(sc_final_resolved_1)]+17,
     SC_X4_RSLVD5r_LOOPCNT0f_GET(sc_final_resolved_5)));

  PHYMOD_DEBUG_ERROR(("| PLL DIV      %04d |   CL72  ENA : %02d   |  BYTE  DEL %-6s|   LP CNT1 : %04d |\n",
     PLL_CAL_CTL7r_PLL_MODEf_GET(pll_div_reg),
     SC_X4_RSLVD0r_CL72_ENABLEf_GET(sc_final_resolved_0),
     e2s_temod_byte_del_mode[SC_X4_RSLVD1r_CL36BYTEDELETEMODEf_GET(sc_final_resolved_1)]+28,
     SC_X4_RSLVD5r_LOOPCNT1f_GET(sc_final_resolved_5)));

  PHYMOD_DEBUG_ERROR(("| REF CLOCK    %04d | CHKEND  ENA : %02d   |64b66DEC EN %d     |  MAC  CGC : %04d |\n",
     MAIN0_SETUPr_REFCLK_SELf_GET(setup_reg),
     SC_X4_RSLVD2r_CHK_END_ENf_GET(sc_final_resolved_2),
     SC_X4_RSLVD1r_BRCM64B66_DESCRAMBLER_ENABLEf_GET(sc_final_resolved_1),
     SC_X4_RSLVD6r_MAC_CREDITGENCNTf_GET(sc_final_resolved_6)));

  PHYMOD_DEBUG_ERROR(("|                   |    FEC  ENA : %02d   |                  |  REP  CNT : %04d |\n",
     SC_X4_RSLVD0r_FEC_ENABLEf_GET(sc_final_resolved_0),
     SC_X4_RSLVD7r_REPLICATION_CNTf_GET(sc_final_resolved_7)));

  PHYMOD_DEBUG_ERROR(("|                   |REORDER  ENA : %02d   |                  |PCS CRD EN : %04d |\n",
     SC_X4_RSLVD2r_REORDER_ENf_GET(sc_final_resolved_2),
     SC_X4_RSLVD7r_PCS_CREDITENABLEf_GET(sc_final_resolved_7)));

  PHYMOD_DEBUG_ERROR(("|                   |   CL36  ENA : %02d   |                  |PCS CK CNT : %04d |\n",
     SC_X4_RSLVD2r_CL36_ENf_GET(sc_final_resolved_2),
     SC_X4_RSLVD7r_PCS_CLOCKCNT0f_GET(sc_final_resolved_7)));

  PHYMOD_DEBUG_ERROR(("|                   |                    |                  |CRDGEN CNT : %04d |\n",
     SC_X4_RSLVD8r_PCS_CREDITGENCNTf_GET(sc_final_resolved_8)));


  PHYMOD_DEBUG_ERROR(("+-------------------+--------------------+------------------+------------------+\n"));

  return PHYMOD_E_NONE;
}

/**
@brief   Autoneg Ability diagnostic function
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE
@details Autoneg diags
 1.  Get the AN Abilities for CL37 and CL73 
*/
int temod_diag_autoneg_abilities(PHYMOD_ST *pc)
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

int tempd_an_hcd_spd_get (uint16_t spd, char *mystr, int len)
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

  PHYMOD_STRNCPY(mystr, an_spd, len);
  
  return PHYMOD_E_NONE;
}
/**
@brief   Autoneg diagnostic function
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE
@details Autoneg diags
 1.  Get the AN complete status for AN37/73
 2.  Get the resolved HCD
 3.  Get the link up status after AN
*/
int temod_diag_autoneg(PHYMOD_ST *pc)
{
  AN_X4_AN_PAGE_SEQUENCER_STSr_t an_page_seq_status;
  AN_X4_AN_ABIL_RESOLUTION_STSr_t an_hcd_status;
  AN_X4_AN_MISC_STSr_t an_misc_status;
  char  an_spd[16];

  READ_AN_X4_AN_PAGE_SEQUENCER_STSr(pc,&an_page_seq_status);
  READ_AN_X4_AN_ABIL_RESOLUTION_STSr(pc,&an_hcd_status);
  READ_AN_X4_AN_MISC_STSr(pc,&an_misc_status);
  tempd_an_hcd_spd_get (AN_X4_AN_ABIL_RESOLUTION_STSr_AN_HCD_SPEEDf_GET(an_hcd_status), an_spd, sizeof(an_spd));

  PHYMOD_DEBUG_ERROR(("| TRG ADR : %06d LANE: %02d     AUTONEG                                        |\n", 
              pc->addr, pc->lane_mask));
  PHYMOD_DEBUG_ERROR(("+-------------+-------------+------------------------------+-------------------+\n"));
  PHYMOD_DEBUG_ERROR(("| AN37: %c     | AN73 : %c    | AN HCD SPD : %-16s|  AN LINK : %s     |\n",
    (AN_X4_AN_PAGE_SEQUENCER_STSr_AN_ST_CL37_COMPLETEf_GET(an_page_seq_status) == 1) ? 'Y' : 'N',
    (AN_X4_AN_PAGE_SEQUENCER_STSr_AN_ST_CL73_COMPLETEf_GET(an_page_seq_status) == 1) ? 'Y' : 'N', an_spd,
    (((AN_X4_AN_PAGE_SEQUENCER_STSr_AN_ST_CL73_COMPLETEf_GET(an_page_seq_status) == 1) ||
     (AN_X4_AN_PAGE_SEQUENCER_STSr_AN_ST_CL37_COMPLETEf_GET(an_page_seq_status) == 1)) &&
     (AN_X4_AN_MISC_STSr_AN_COMPLETEf_GET(an_misc_status) == 1))? "UP":"DN"));

  temod_diag_autoneg_abilities(pc);
  PHYMOD_DEBUG_ERROR(("+-------------------+--------------------+-----------------+-------------------+\n"));

  return PHYMOD_E_NONE;
}

/**
@brief   Topology diagnostic function.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE
@details Get the topology details
1. LoopBack
   a. Remote Loopback - RX loopback
      i.  PCS loopback
      ii. PMD loopback  //Not tested in TSCE
   b. Local loopback/gloop - TX loopback
      i.  PCS loopback
      ii. PMD loopback
2. LaneSwap
   a. PCS LaneSwap(RX)
   b. PMD LaneSwap(TX)
3. Polarity Inversion
   a. tx
   b. rx
4. Master PortNum, port mode and single port mode
*/

int temod_diag_topology(PHYMOD_ST *pc)
{
  MAIN0_LPBK_CTLr_t pcs_loopback_ctrl;
  TLB_RX_DIG_LPBK_CFGr_t pmd_loopback_dig;
  TLB_TX_RMT_LPBK_CFGr_t pmd_loopback_remote;
  MAIN0_LN_SWPr_t pcs_lane_swap;
  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_t pmd_lane_addr_swap_1;
  DIG_LN_ADDR_2_3r_t pmd_lane_addr_swap_2;
  DIG_TX_LN_MAP_0_1_2r_t pmd_lane_addr_swap_3;
  TLB_TX_TLB_TX_MISC_CFGr_t tx_pol_inv;
  TLB_RX_TLB_RX_MISC_CFGr_t rx_pol_inv;
  MAIN0_SETUPr_t reg_main0_setup;
 
  /* Main0 Setup Register */
  /*1. Loopback  */
  READ_MAIN0_LPBK_CTLr(pc, &pcs_loopback_ctrl);
  READ_TLB_RX_DIG_LPBK_CFGr(pc, &pmd_loopback_dig);
  READ_TLB_TX_RMT_LPBK_CFGr(pc, &pmd_loopback_remote);
 
   /*2. LaneSwap  */
  READ_MAIN0_LN_SWPr(pc, &pcs_lane_swap);
  READ_DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r(pc, &pmd_lane_addr_swap_1);
  READ_DIG_LN_ADDR_2_3r(pc, &pmd_lane_addr_swap_2);
  READ_DIG_TX_LN_MAP_0_1_2r(pc, &pmd_lane_addr_swap_3);
 
   /*3.Polarity Inversion */
  READ_TLB_TX_TLB_TX_MISC_CFGr(pc, &tx_pol_inv);
  READ_TLB_RX_TLB_RX_MISC_CFGr(pc, &rx_pol_inv);
 
   /* 4. Master PortNum, port mode abd single port mode */
  READ_MAIN0_SETUPr(pc, &reg_main0_setup);
 

  /* Print the information here */
  {
    uint16_t pcs_lcl_lb, pcs_rmt_lb, pmd_dig_lb, pmd_rmt_lb;
  pcs_lcl_lb = MAIN0_LPBK_CTLr_LOCAL_PCS_LOOPBACK_ENABLEf_GET(pcs_loopback_ctrl);
  pcs_rmt_lb = MAIN0_LPBK_CTLr_REMOTE_PCS_LOOPBACK_ENABLEf_GET(pcs_loopback_ctrl);
  pmd_dig_lb = TLB_RX_DIG_LPBK_CFGr_DIG_LPBK_ENf_GET(pmd_loopback_dig);
  pmd_rmt_lb = TLB_TX_RMT_LPBK_CFGr_RMT_LPBK_ENf_GET(pmd_loopback_remote);

  PHYMOD_DEBUG_ERROR(("\n"));
  PHYMOD_DEBUG_ERROR(("+------------------------------------------------------------------------------+\n"));
  PHYMOD_DEBUG_ERROR(("| TRG ADR : %06d LANE: %02d     TOPOLOGY                                       |\n", pc->addr, pc->lane_mask));
  PHYMOD_DEBUG_ERROR(("+-------------------+--------------------------+---------------+---------------+\n"));
  PHYMOD_DEBUG_ERROR(("| PCSLCL LPBK: %c%c%c%c | PCS LANE SWAP L2P : %d%d%d%d | TX POLARITY : %d | PORT NUM : %d|\n", 
	(pcs_lcl_lb & 8) ? 'Y':'N',
	(pcs_lcl_lb & 4) ? 'Y':'N',
	(pcs_lcl_lb & 2) ? 'Y':'N',
	(pcs_lcl_lb & 1) ? 'Y':'N',
        MAIN0_LN_SWPr_LOG3_TO_PHY_LNSWAP_SELf_GET(pcs_lane_swap),
        MAIN0_LN_SWPr_LOG2_TO_PHY_LNSWAP_SELf_GET(pcs_lane_swap),
        MAIN0_LN_SWPr_LOG1_TO_PHY_LNSWAP_SELf_GET(pcs_lane_swap),
        MAIN0_LN_SWPr_LOG0_TO_PHY_LNSWAP_SELf_GET(pcs_lane_swap),
        TLB_TX_TLB_TX_MISC_CFGr_TX_PMD_DP_INVERTf_GET(tx_pol_inv),
        MAIN0_SETUPr_MASTER_PORT_NUMf_GET(reg_main0_setup)));

  PHYMOD_DEBUG_ERROR(("| PCSRMT LPBK: %c%c%c%c | PMD LANE ADDR IDX : %d%d%d%d |                 | SNGLMODE : %d|\n",
	(pcs_rmt_lb & 8) ? 'Y':'N',
	(pcs_rmt_lb & 4) ? 'Y':'N',
	(pcs_rmt_lb & 2) ? 'Y':'N',
	(pcs_rmt_lb & 1) ? 'Y':'N',
        DIG_LN_ADDR_2_3r_LANE_ADDR_3f_GET(pmd_lane_addr_swap_2),
        DIG_LN_ADDR_2_3r_LANE_ADDR_2f_GET(pmd_lane_addr_swap_2),
        DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_LANE_ADDR_1f_GET(pmd_lane_addr_swap_1),
        DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_LANE_ADDR_0f_GET(pmd_lane_addr_swap_1),
        MAIN0_SETUPr_SINGLE_PORT_MODEf_GET(reg_main0_setup)));

  PHYMOD_DEBUG_ERROR(("| PMDDIG LPBK: %c%c%c%c | PMD TO AFE        : %d%d%d%d | RX POLARITY : %d | PORT MODE: %d|\n",
	(pmd_dig_lb & 8) ? 'Y':'N',
	(pmd_dig_lb & 4) ? 'Y':'N',
	(pmd_dig_lb & 2) ? 'Y':'N',
	(pmd_dig_lb & 1) ? 'Y':'N',
        DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_TX_LANE_MAP_3f_GET(pmd_lane_addr_swap_1),
        DIG_TX_LN_MAP_0_1_2r_TX_LANE_MAP_2f_GET(pmd_lane_addr_swap_3),
        DIG_TX_LN_MAP_0_1_2r_TX_LANE_MAP_1f_GET(pmd_lane_addr_swap_3),
        DIG_TX_LN_MAP_0_1_2r_TX_LANE_MAP_0f_GET(pmd_lane_addr_swap_3),
        TLB_RX_TLB_RX_MISC_CFGr_RX_PMD_DP_INVERTf_GET(rx_pol_inv),
        MAIN0_SETUPr_PORT_MODE_SELf_GET(reg_main0_setup)));

  PHYMOD_DEBUG_ERROR(("| PMDREM LPBK: %c%c%c%c |                          |                               |\n",
	(pmd_rmt_lb & 8) ? 'Y':'N',
	(pmd_rmt_lb & 4) ? 'Y':'N',
	(pmd_rmt_lb & 2) ? 'Y':'N',
	(pmd_rmt_lb & 1) ? 'Y':'N'));
  PHYMOD_DEBUG_ERROR(("+-------------------+--------------------------+-------------------------------+\n"));

  }

  return PHYMOD_E_NONE;
}

/**
@brief   Get the link status
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE
@details Call all the Diags sequentially.
  #temod_diag_topology
  #temod_diag_link
  #temod_diag_autoneg
  #temod_diag_speed
*/
int temod_diag_general(PHYMOD_ST *pc)
{
  temod_diag_topology(pc);
  temod_diag_link(pc);
  temod_diag_autoneg(pc);
  temod_diag_speed(pc);

  return (PHYMOD_E_NONE);
}

/**
@brief   Get the link status
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE
@details Print the link status.
-  pmd_live_status
  -# pll_lock_sts : PMD PLL lock indication
- pmd_lane_live_status
  -# signal_detect_sts : The signal detected on the lane
  -# rx_lock_sts : The recovery clock locked for the rx lane
- pmd_lane_latched_status
- pcs_live_status
  -# Live per port sync status indicator: cl36/cl48/brcm mode/Comma detect/code sync
  -# Live per lane Block Lock Status indicator: cl49/cl82/Sync bits locked, on the 2 sync bits of the 66 bits(of 64/66 encoding)
   b. link_status : Link status for that port
- pcs_latched_status
*/
int temod_diag_link(PHYMOD_ST *pc)
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

/**
@brief   Internal Tfc info (PRTP. vs. PktGen vs. MAC dataetc.).
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE
@details Print the Internal Tfc types.
*/
int temod_diag_internal_tfc(PHYMOD_ST *pc)
{
  PKTGEN_CTL1r_t reg_pktgen_ctl1;
  PKTGEN_CTL2r_t reg_pktgen_ctl2;
  PKTGEN_PRTPCTLr_t reg_prtp_ctl;

  READ_PKTGEN_CTL1r(pc, &reg_pktgen_ctl1);
  READ_PKTGEN_CTL2r(pc, &reg_pktgen_ctl2);
  READ_PKTGEN_PRTPCTLr(pc, &reg_prtp_ctl);

  PHYMOD_DEBUG_ERROR(("| TRG ADR : %06d LANE: %02d    INTERNAL TFC                                    |\n", 
                       pc->addr, pc->lane_mask));
  PHYMOD_DEBUG_ERROR(("+------------------------------------------+-----------------------------------+\n"));

  if (PKTGEN_CTL1r_TX_PRTP_ENf_GET(reg_pktgen_ctl1) || 
      PKTGEN_PRTPCTLr_RX_PRTP_ENf_GET(reg_prtp_ctl)) {

       PHYMOD_DEBUG_ERROR(("| TX/RX_PRT_EN %-8s (Tx = 0x%0x, Rx=0x%0X) | PKT_OR_PRTP %-8s              |\n",
          ((PKTGEN_CTL1r_TX_PRTP_ENf_GET(reg_pktgen_ctl1) == 0 ) || 
           (PKTGEN_PRTPCTLr_RX_PRTP_ENf_GET(reg_prtp_ctl) == 0)) ? "Not Set" : "Set", 
           PKTGEN_CTL1r_TX_PRTP_ENf_GET(reg_pktgen_ctl1),
           PKTGEN_PRTPCTLr_RX_PRTP_ENf_GET(reg_prtp_ctl),
           (PKTGEN_CTL1r_PKT_OR_PRTPf_GET(reg_pktgen_ctl1) == 1) ? "Set" : "Not Set"));

  } else if(PKTGEN_CTL2r_PKTGEN_ENf_GET(reg_pktgen_ctl2)){
    PHYMOD_DEBUG_ERROR(("| Traffic_type: CJ/CRPAT                   | PKT_OR_PRTP : %-16s    |\n",
        (PKTGEN_CTL1r_PKT_OR_PRTPf_GET(reg_pktgen_ctl1) == 0)? "Set" : "Not Set"));
  } else {
    PHYMOD_DEBUG_ERROR(("| Traffic_type: MAC                      |                                     |\n"));
  }

  PHYMOD_DEBUG_ERROR(("+------------------------------------------+-----------------------------------+\n"));
  return PHYMOD_E_NONE;
}

#define _TRG_ST_PRINT(pc, st_num, ovr, sc_st)                                                                              \
{                                                                                                                          \
  SC_X1_SPD_##ovr##_SPDr_t sc_st##_speed;                                                                                  \
  SC_X1_SPD_##ovr##_0r_t sc_st##_0;                                                                                        \
  SC_X1_SPD_##ovr##_1r_t sc_st##_1;                                                                                        \
  SC_X1_SPD_##ovr##_2r_t sc_st##_2;                                                                                        \
  SC_X1_SPD_##ovr##_3r_t sc_st##_3;                                                                                        \
  SC_X1_SPD_##ovr##_4r_t sc_st##_4;                                                                                        \
  SC_X1_SPD_##ovr##_5r_t sc_st##_5;                                                                                        \
  SC_X1_SPD_##ovr##_6r_t sc_st##_6;                                                                                        \
  SC_X1_SPD_##ovr##_7r_t sc_st##_7;                                                                                        \
  SC_X1_SPD_##ovr##_8r_t sc_st##_8;                                                                                        \
  uint8_t x1_speed;                                                                                                        \
  READ_SC_X1_SPD_##ovr##_SPDr(pc,&sc_st##_speed);                                                                          \
  x1_speed = SC_X1_SPD_##ovr##_SPDr_SPEEDf_GET(sc_st##_speed);                                                             \
  if (x1_speed == 0xFF) {                                                                                                  \
  PHYMOD_DEBUG_ERROR(("| ST [%d]               | SPD_ILLEGAL                 | NUM LANES : %d           |\n",      \
       st_num,                                                                                                             \
       SC_X1_SPD_##ovr##_SPDr_NUM_LANESf_GET(sc_st##_speed)));                                                             \
  } else {                                                                                                                 \
    PHYMOD_DEBUG_ERROR(("| ST [%d]               | 0x%02X                        | NUM LANES : %d           |\n",      \
           st_num, x1_speed,                                                                                               \
           SC_X1_SPD_##ovr##_SPDr_NUM_LANESf_GET(sc_st##_speed)));                                                         \
  }                                                                                                                        \
	                                                                                                                          \
  if((resolved_speed == 0xff) || (resolved_speed==SC_X1_SPD_##ovr##_SPDr_SPEEDf_GET(sc_st##_speed))) { /* ST has Entry */  \
    READ_SC_X1_SPD_##ovr##_0r(pc,&sc_st##_0);                                                                              \
    READ_SC_X1_SPD_##ovr##_1r(pc,&sc_st##_1);                                                                              \
    READ_SC_X1_SPD_##ovr##_2r(pc,&sc_st##_2);                                                                              \
    READ_SC_X1_SPD_##ovr##_3r(pc,&sc_st##_3);                                                                              \
    READ_SC_X1_SPD_##ovr##_4r(pc,&sc_st##_4);                                                                              \
    READ_SC_X1_SPD_##ovr##_5r(pc,&sc_st##_5);                                                                              \
    READ_SC_X1_SPD_##ovr##_6r(pc,&sc_st##_6);                                                                              \
    READ_SC_X1_SPD_##ovr##_7r(pc,&sc_st##_7);                                                                              \
    READ_SC_X1_SPD_##ovr##_8r(pc,&sc_st##_8);                                                                              \
                                                                                                                           \
  PHYMOD_DEBUG_ERROR(("| %-20s | SGMII SPD : %04d            |                         |\n",                               \
        e2s_temod_descrambler_mode[SC_X1_SPD_##ovr##_1r_DESCRAMBLERMODEf_GET(sc_st##_1)]+6,                                \
        SC_X1_SPD_##ovr##_3r_SGMII_SPD_SWITCHf_GET(sc_st##_3)));                                                           \
                                                                                                                           \
  PHYMOD_DEBUG_ERROR(("| %-20s | %-28s|  CLK CNT0 : %04d        |\n",                                                     \
        e2s_temod_scr_mode[SC_X1_SPD_##ovr##_0r_SCR_MODEf_GET(sc_st##_0)]+6,                                               \
        e2s_temod_decoder_mode[SC_X1_SPD_##ovr##_1r_DECODERMODEf_GET(sc_st##_1)]+6,                                        \
        SC_X1_SPD_##ovr##_3r_CLOCKCNT0f_GET(sc_st##_3)));                                                                  \
                                                                                                                           \
  PHYMOD_DEBUG_ERROR(("| %-20s | %-28s|  CLK CNT1 : %04d        |\n",                                                     \
        e2s_temod_encode_mode[SC_X1_SPD_##ovr##_0r_ENCODEMODEf_GET(sc_st##_0)]+6,                                          \
        e2s_temod_deskew_mode[SC_X1_SPD_##ovr##_1r_DESKEWMODEf_GET(sc_st##_1)]+6,                                          \
        SC_X1_SPD_##ovr##_4r_CLOCKCNT1f_GET(sc_st##_4)));                                                                  \
			                                                                                                               \
  PHYMOD_DEBUG_ERROR(("| %-20s | %-28s|   LP CNT0 : %04d        |\n",                                                     \
        e2s_temod_blocksync_mode[SC_X1_SPD_##ovr##_2r_BLOCK_SYNC_MODEf_GET(sc_st##_2)]+6,                                  \
        e2s_temod_descrambler2_mode[SC_X1_SPD_##ovr##_1r_DESC2_MODEf_GET(sc_st##_1)]+6,                                    \
        SC_X1_SPD_##ovr##_5r_LOOPCNT0f_GET(sc_st##_5)));                                                                   \
                                                                                                                           \
  PHYMOD_DEBUG_ERROR(("|                      |                             |CRDGEN CNT : %04d        |\n",                \
        SC_X1_SPD_##ovr##_8r_PCS_CREDITGENCNTf_GET(sc_st##_8)));                                                           \
                                                                                                                           \
  PHYMOD_DEBUG_ERROR(("|   CL72  ENA : %02d     | %-28s|   LP CNT1 : %04d        |\n",                                     \
        SC_X1_SPD_##ovr##_0r_CL72_ENABLEf_GET(sc_st##_0),                                                                  \
        e2s_temod_byte_del_mode[SC_X1_SPD_##ovr##_1r_CL36BYTEDELETEMODEf_GET(sc_st##_1)]+6,                                \
        SC_X1_SPD_##ovr##_5r_LOOPCNT1f_GET(sc_st##_5)));                                                                   \
                                                                                                                           \
  PHYMOD_DEBUG_ERROR(("|  CHKEND  ENA : %02d    | 64b66DEC EN %d               |  MAC  CGC : %04d        |\n",             \
        SC_X1_SPD_##ovr##_2r_CHK_END_ENf_GET(sc_st##_2),                                                                   \
        SC_X1_SPD_##ovr##_1r_BRCM64B66_DESCRAMBLER_ENABLEf_GET(sc_st##_1),                                                 \
        SC_X1_SPD_##ovr##_6r_MAC_CREDITGENCNTf_GET(sc_st##_6)));                                                           \
			                                                                                                               \
  PHYMOD_DEBUG_ERROR(("|     FEC  ENA : %02d    |                             |  REP  CNT : %04d        |\n",              \
        SC_X1_SPD_##ovr##_0r_FEC_ENABLEf_GET(sc_st##_0),                                                                   \
        SC_X1_SPD_##ovr##_7r_REPLICATION_CNTf_GET(sc_st##_7)));                                                            \
                                                                                                                           \
  PHYMOD_DEBUG_ERROR(("| REORDER  ENA : %02d    |                             |PCS CRD EN : %04d        |\n",              \
        SC_X1_SPD_##ovr##_2r_REORDER_ENf_GET(sc_st##_2),                                                                   \
        SC_X1_SPD_##ovr##_7r_PCS_CREDITENABLEf_GET(sc_st##_7)));                                                           \
			                                                                                                               \
  PHYMOD_DEBUG_ERROR(("|    CL36  ENA : %02d    |                             |PCS CK CNT : %04d        |\n",              \
        SC_X1_SPD_##ovr##_2r_CL36_ENf_GET(sc_st##_2),                                                                      \
        SC_X1_SPD_##ovr##_7r_PCS_CLOCKCNT0f_GET(sc_st##_7)));                                                              \
                                                                                                                           \
  PHYMOD_DEBUG_ERROR(("+----------------------+-----------------------------+----------+--------------+\n"));              \
  }                                                                                                                        \
}

/**
@brief   Soft Table diagnostic function
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   resolved_speed The speed to which AN is already resolved.
@returns PHYMOD_E_NONE
@details Soft Table diags
 Print the soft table enteries
 if the resolved_speed is not set, then all the table enteries are displayed
*/
int temod_diag_st(PHYMOD_ST *pc, int resolved_speed)
{
  _TRG_ST_PRINT(pc, 0, OVRR0, sc_st0)
  _TRG_ST_PRINT(pc, 1, OVRR1, sc_st1) 
  _TRG_ST_PRINT(pc, 2, OVRR2, sc_st2)
  _TRG_ST_PRINT(pc, 3, OVRR2, sc_st3)

  PHYMOD_DEBUG_ERROR(("+----------------------+------------------------------+---------+--------------+\n"));

  return PHYMOD_E_NONE;
} /*temod_diag_st */

/**
@brief   AN Timers diagnostic function
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE 
@details Prints the AN timers for both CL37 and CL73 autonegotiations.
*/
int temod_diag_an_timers(PHYMOD_ST* pc)
{
  AN_X1_CL37_RESTARTr_t  reg_cl37_restart;
  AN_X1_CL37_ACKr_t      reg_cl37_ack;
  AN_X1_CL37_ERRr_t      reg_cl37_err;
  AN_X1_CL73_BRK_LNKr_t  reg_cl73_break_link;
  AN_X1_CL73_ERRr_t      reg_cl73_err;
  AN_X1_CL73_DME_LOCKr_t reg_cl73_dme_lock;
  AN_X1_LNK_UPr_t        reg_link_up;
  AN_X1_LNK_FAIL_INHBT_TMR_CL72r_t     reg_inhibit_timer;
  AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_t reg_inhibit_not_timer;
  AN_X1_PD_SD_TMRr_t                   reg_pd_sd_timer;
  AN_X1_CL37_SYNC_STS_FILTER_TMRr_t    reg_sync_status_filter_timer;
  AN_X1_PD_TO_CL37_LNK_WAIT_TMRr_t     reg_link_wait_timer;
  AN_X1_IGNORE_LNK_TMRr_t              reg_ignore_link_timer;
  AN_X1_DME_PAGE_TMRr_t                reg_dme_page_timer;
  AN_X1_SGMII_CL73_TMR_TYPEr_t         reg_sgmii_cl73_timer;

  READ_AN_X1_CL37_RESTARTr(pc, &reg_cl37_restart);
  READ_AN_X1_CL37_ACKr(pc, &reg_cl37_ack);
  READ_AN_X1_CL37_ERRr(pc, &reg_cl37_err);
  READ_AN_X1_CL73_BRK_LNKr(pc, &reg_cl73_break_link);
  READ_AN_X1_CL73_ERRr(pc, &reg_cl73_err);
  READ_AN_X1_CL73_DME_LOCKr(pc, &reg_cl73_dme_lock);
  READ_AN_X1_LNK_UPr(pc, &reg_link_up);
  READ_AN_X1_LNK_FAIL_INHBT_TMR_CL72r(pc, &reg_inhibit_timer);
  READ_AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r(pc, &reg_inhibit_not_timer);
  READ_AN_X1_PD_SD_TMRr(pc, &reg_pd_sd_timer);
  READ_AN_X1_CL37_SYNC_STS_FILTER_TMRr(pc, &reg_sync_status_filter_timer);
  READ_AN_X1_PD_TO_CL37_LNK_WAIT_TMRr(pc, &reg_link_wait_timer);
  READ_AN_X1_IGNORE_LNK_TMRr(pc, &reg_ignore_link_timer);
  READ_AN_X1_DME_PAGE_TMRr(pc, &reg_dme_page_timer);
  READ_AN_X1_SGMII_CL73_TMR_TYPEr(pc, &reg_sgmii_cl73_timer);

  PHYMOD_DEBUG_ERROR(("| TRG ADR : %06d LANE: %02d    AN TIMERS                                       |\n", 
                       pc->addr, pc->lane_mask));
  PHYMOD_DEBUG_ERROR(("+--------------------------------------+---------------------------------------+\n"));
  PHYMOD_DEBUG_ERROR(("| CL37 RESTART          : 0x%08X   | CL37 ACK               : 0x%08X   |\n",
      AN_X1_CL37_RESTARTr_CL37_RESTART_TIMER_PERIODf_GET(reg_cl37_restart),
      AN_X1_CL37_ACKr_CL37_ACK_TIMER_PERIODf_GET(reg_cl37_ack)));

  PHYMOD_DEBUG_ERROR(("| CL37 ERR              : 0x%08X   | CL37 LINK BREAK        : 0x%08X   |\n",
      AN_X1_CL37_ERRr_CL37_ERROR_TIMER_PERIODf_GET(reg_cl37_err),
      AN_X1_CL73_BRK_LNKr_TX_DISABLE_TIMER_PERIODf_GET(reg_cl73_break_link)));

  PHYMOD_DEBUG_ERROR(("| CL73 ERR              : 0x%08X   | CL73 DME LOCK          : 0x%08X   |\n", 
      AN_X1_CL73_ERRr_CL73_ERROR_TIMER_PERIODf_GET(reg_cl73_err),
      AN_X1_CL73_DME_LOCKr_PD_DME_LOCK_TIMER_PERIODf_GET(reg_cl73_dme_lock)));

  PHYMOD_DEBUG_ERROR(("| LINK_UP               : 0x%08X   | PS SD                  : 0x%08x   |\n",
      AN_X1_LNK_UPr_CL73_LINK_UP_TIMER_PERIODf_GET(reg_link_up),
      AN_X1_PD_SD_TMRr_PD_SD_TIMER_PERIODf_GET(reg_pd_sd_timer)));

  PHYMOD_DEBUG_ERROR(("| SYNC STATUS           : 0x%08X   | PD TO CL37             : 0x%08X   |\n",
      AN_X1_CL37_SYNC_STS_FILTER_TMRr_CL37_SYNC_STATUS_FILTER_TIMER_PERIODf_GET(reg_sync_status_filter_timer),
      AN_X1_PD_TO_CL37_LNK_WAIT_TMRr_PD_TO_CL37_LINK_WAIT_TIMERf_GET(reg_link_wait_timer)));

  PHYMOD_DEBUG_ERROR(("| IGNORE LINK           : 0x%08X   | SGMII                  : 0x%08X   |\n",
      AN_X1_IGNORE_LNK_TMRr_IGNORE_LINK_TIMER_PERIODf_GET(reg_ignore_link_timer),
      AN_X1_SGMII_CL73_TMR_TYPEr_SGMII_TIMERf_GET(reg_sgmii_cl73_timer)));

  PHYMOD_DEBUG_ERROR(("| DME PAGE MIN          : 0x%08X   | DME PAGE MAX           : 0x%08X   |\n",
      AN_X1_DME_PAGE_TMRr_CL73_PAGE_TEST_MIN_TIMERf_GET(reg_dme_page_timer),
      AN_X1_DME_PAGE_TMRr_CL73_PAGE_TEST_MAX_TIMERf_GET(reg_dme_page_timer)));

  PHYMOD_DEBUG_ERROR(("| FAIL INHIBIT W/O CL72 : 0x%08X   | FAIL INHIBIT WITH CL72 : 0x%08X   |\n",
      AN_X1_LNK_FAIL_INHBT_TMR_NOT_CL72r_LINK_FAIL_INHIBIT_TIMER_NCL72_PERIODf_GET(reg_inhibit_not_timer),
      AN_X1_LNK_FAIL_INHBT_TMR_CL72r_LINK_FAIL_INHIBIT_TIMER_CL72_PERIODf_GET(reg_inhibit_timer)));

  PHYMOD_DEBUG_ERROR(("+--------------------------------------+---------------------------------------+\n"));
  return PHYMOD_E_NONE;
}

/**
@brief   Prints out the debug state register
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE
@details Prints the debug state regs.
*/
int temod_diag_state(PHYMOD_ST* pc)
{
  SC_X4_DBGr_t               reg_sc_dbg_st;
  AN_X4_TLA_SEQUENCER_STSr_t reg_an_tla_seq;

  READ_SC_X4_DBGr(pc, &reg_sc_dbg_st);
  READ_AN_X4_TLA_SEQUENCER_STSr(pc, &reg_an_tla_seq);

  PHYMOD_DEBUG_ERROR(("|                                 DEBUG STATE                                  |\n"));
  PHYMOD_DEBUG_ERROR(("+--------------------------------------+---------------------------------------+\n"));
  PHYMOD_DEBUG_ERROR(("| SC_DEBUG_STATE    : 0x%08x       |  FSM_STATUS : 0x%08X              |\n",
       SC_X4_DBGr_GET(reg_sc_dbg_st), SC_X4_DBGr_SC_FSM_STATUSf_GET(reg_sc_dbg_st)));

  PHYMOD_DEBUG_ERROR(("| TLA_SEQUENCER STS : 0x%08x       |                                       |\n",
       AN_X4_TLA_SEQUENCER_STSr_GET(reg_an_tla_seq)));
  PHYMOD_DEBUG_ERROR(("+--------------------------------------+---------------------------------------+\n"));

  return PHYMOD_E_NONE;
}

/*
@brief  TBD
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE
@details  Misc debug
*/
int temod_diag_debug(PHYMOD_ST* pc)
{
  return PHYMOD_E_NONE;
}

int temod_diag_g_rd_ram(PHYMOD_ST *pc, int mode, int lane)
{
  return PHYMOD_E_NONE;
}

int temod_diag_g_rd_uC(PHYMOD_ST *pc, int lane)
{
  return PHYMOD_E_NONE;
}

int temod_diag_g_info(PHYMOD_ST *pc)
{
  return PHYMOD_E_NONE;
}

int temod_diag_ieee(PHYMOD_ST *pc)
{
  return PHYMOD_E_NONE;
}

/*!

*/
int temod_diag(PHYMOD_ST *pc, temod_diag_type diag_type)
{
  int rv;
  rv = PHYMOD_E_NONE;

  PHYMOD_DEBUG_ERROR(("+------------------------------------------------------------------------------+\n"));
  if(diag_type & TEMOD_DIAG_GENERAL)    rv |= temod_diag_general(pc);
  if(diag_type & TEMOD_DIAG_TOPOLOGY)   rv |= temod_diag_topology(pc);
  if(diag_type & TEMOD_DIAG_LINK)       rv |= temod_diag_link(pc);
  if(diag_type & TEMOD_DIAG_SPEED)      rv |= temod_diag_speed(pc);
  if(diag_type & TEMOD_DIAG_ANEG)       rv |= temod_diag_autoneg(pc);
  if(diag_type & TEMOD_DIAG_TFC)        rv |= temod_diag_internal_tfc(pc);
  if(diag_type & TEMOD_DIAG_AN_TIMERS)  rv |= temod_diag_an_timers(pc);
  if(diag_type & TEMOD_DIAG_STATE)      rv |= temod_diag_state(pc);
  if(diag_type & TEMOD_DIAG_DEBUG)      rv |= temod_diag_debug(pc);
  return rv;
}

/*!
*/
int temod_diag_disp(PHYMOD_ST *pc, const char* cmd_str)
{
    temod_diag_type type;
 
    if (!cmd_str) 
        type = TEMOD_DIAG_GENERAL;
    else if (!PHYMOD_STRCMP(cmd_str, "topo"))
        type = TEMOD_DIAG_TOPOLOGY;
    else if (!PHYMOD_STRCMP(cmd_str, "link"))
        type = TEMOD_DIAG_LINK;
    else if (!PHYMOD_STRCMP(cmd_str, "speed"))
        type = TEMOD_DIAG_SPEED;
    else if (!PHYMOD_STRCMP(cmd_str, "aneg"))
        type = TEMOD_DIAG_ANEG;
    else if (!PHYMOD_STRCMP(cmd_str, "tfc"))
        type = TEMOD_DIAG_TFC;
    else if (!PHYMOD_STRCMP(cmd_str, "antimers"))
        type = TEMOD_DIAG_AN_TIMERS;
    else if (!PHYMOD_STRCMP(cmd_str, "state"))
        type = TEMOD_DIAG_STATE;
    else if (!PHYMOD_STRCMP(cmd_str, "debug"))
        type = TEMOD_DIAG_DEBUG;
    else
        type = TEMOD_DIAG_GENERAL;

    return (temod_diag(pc, type));
}

