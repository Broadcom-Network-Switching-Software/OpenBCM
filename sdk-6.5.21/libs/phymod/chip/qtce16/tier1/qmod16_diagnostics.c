/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */ 

#ifndef _DV_TB_
#define _SDK_QMOD16_ 1 
#endif

#ifdef _DV_TB_
#include "qmod16_phyreg.h"
#include <stdint.h>
#include "qmod16_main.h"
#include "qmod16_enum_defines.h"
#include "qmod16.h"
#include "qmod16_defines.h"
#include "qmod16_phyreg.h"
#include "qe16PCSRegEnums.h"
#include "phy_tsc_iblk.h"
#endif

#ifdef _SDK_QMOD16_
#include "phy_tsc_iblk.h"
#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_debug.h>
#include <phymod/chip/bcmi_qtce16_xgxs_defs.h>
#include "qmod16_enum_defines.h"
#include "qmod16.h"
#include "qmod16_device.h"
#include "qmod16_sc_lkup_table.h"
#include "qe16PCSRegEnums.h"
#endif

#define _TRGO_(a,l) PHYMOD_DEBUG_ERROR(("<< PHY_TRG: Adr:%06d Ln:%02d\n",a,l));
#define _TRGC_(a,l) PHYMOD_DEBUG_ERROR((">> PHY TRG: Adr:%06d Ln:%02d\n",a,l));
#ifndef PHYMOD_ST
#ifdef _SDK_QMOD16_
#define PHYMOD_ST phymod_access_t
#define _TRG_OPN(a) _TRGO_(a->addr, a->lane_mask)
#define _TRG_CLS(a) _TRGC_(a->addr, a->lane_mask)
#endif /*_SDK_QMOD16_ */
#endif /* PHYMOD_ST */

#ifdef _DV_TB_
#define PHYMOD_ST  qmod16_st
#define _TRG_OPN(a) _TRGO_(a->prt_ad, a->this_lane)
#define _TRG_CLS(a) _TRGC_(a->prt_ad, a->this_lane)
#endif

int qmod16_diag_link(PHYMOD_ST *ws);
int qmod16_diag_st(PHYMOD_ST *ws, int resolved_speed);

/**
   @brief   Field OVR diagnostic function
   @param   pc handle to current TSCE context (#PHYMOD_ST)
   @returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
   @details Field OVR Details
   Check if the field OVR is set, and if OVR enabled, print the OVR value
*/

int qmod16_diag_field_or(PHYMOD_ST *ws)
{
    return PHYMOD_E_NONE;
}

/**
   @brief   pmd pcs lane cfg diagnostic function
   @param   pc handle to current TSCE context (#PHYMOD_ST)
   @returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
   @details Get the uc controls for pmd lane cfg
   Read the pcs_lane_cfg information from uc RAM. To check if the pmd is to be
   programmed from pcs lane cfg.
 */
int qmod16_diag_pmd_pcs_lane_cfg(PHYMOD_ST *ws, int lane)
{
    return PHYMOD_E_NONE ;
} /* qmod16_diag_pmd_pcs_lane_cfg */

STATIC
int qmod16_an_hcd_spd_get (uint16_t spd, char *mystr, int len)
{
  char *an_spd = "UNDEF";

  if (spd == 1) an_spd ="10M";
  if (spd == 2) an_spd ="100M";
  if (spd == 3) an_spd ="1000M";
  if (spd == 5) an_spd ="1G_KX1";
  if (spd == 6) an_spd ="2p5G_X1";

  PHYMOD_STRNCPY(mystr, an_spd, len);

  return PHYMOD_E_NONE;
}

STATIC
int qmod16_an_local_spd_get (uint16_t spd, char *mystr, int len)
{
  char *an_spd = "RESERVE";

  if (spd == 0) an_spd ="10Mb/s";
  if (spd == 1) an_spd ="100Mb/s";
  if (spd == 2) an_spd ="1Gb/s";
  if (spd == 4) an_spd ="2.5Gb/s";

  PHYMOD_STRNCPY(mystr, an_spd, len);

  return PHYMOD_E_NONE;
}



/**
   @brief   Speed diagnostic function
   @param   pc handle to current TSCE context (#PHYMOD_ST)
   @returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
   @details Speed Details
   1. Get the Speed set
   2. Get the Speed valis and Speed Done Status
   3. Get the final stats of the speed
 */
int qmod16_diag_speed(PHYMOD_ST *pc)
{
    SC_X4_STSr_t sc_ctrl_status;
    MAIN_SETUPr_t setup_reg;
    PLL_CTL8r_t pll_div_reg;
    RXTXCOM_OSR_MODE_CTLr_t    reg_osr_mode;

    SC_X4_FI_SP0_RSLVD_SPDr_t   sc_final_sp0_resolved_speed;
    SC_X4_FI_SP1_RSLVD_SPDr_t   sc_final_sp1_resolved_speed;
    SC_X4_FI_SP2_RSLVD_SPDr_t   sc_final_sp2_resolved_speed;
    SC_X4_FI_SP3_RSLVD_SPDr_t   sc_final_sp3_resolved_speed;
    SC_X4_FI_SP0_RSLVD0r_t      sc_final_sp0_resolved_0;
    SC_X4_FI_SP1_RSLVD0r_t      sc_final_sp1_resolved_0;
    SC_X4_FI_SP2_RSLVD0r_t      sc_final_sp2_resolved_0;
    SC_X4_FI_SP3_RSLVD0r_t      sc_final_sp3_resolved_0;

    SC_X4_FI_SP0_RSLVD1r_t      sc_final_sp0_resolved_1;
    SC_X4_FI_SP1_RSLVD1r_t      sc_final_sp1_resolved_1;
    SC_X4_FI_SP2_RSLVD1r_t      sc_final_sp2_resolved_1;
    SC_X4_FI_SP3_RSLVD1r_t      sc_final_sp3_resolved_1;

    SC_X4_FI_SP0_RSLVD2r_t      sc_final_sp0_resolved_2;
    SC_X4_FI_SP1_RSLVD2r_t      sc_final_sp1_resolved_2;
    SC_X4_FI_SP2_RSLVD2r_t      sc_final_sp2_resolved_2;
    SC_X4_FI_SP3_RSLVD2r_t      sc_final_sp3_resolved_2;

    SC_X4_FI_SP0_RSLVD3r_t      sc_final_sp0_resolved_3;
    SC_X4_FI_SP1_RSLVD3r_t      sc_final_sp1_resolved_3;
    SC_X4_FI_SP2_RSLVD3r_t      sc_final_sp2_resolved_3;
    SC_X4_FI_SP3_RSLVD3r_t      sc_final_sp3_resolved_3;

    SC_X4_FI_SP0_RSLVD4r_t      sc_final_sp0_resolved_4;
    SC_X4_FI_SP1_RSLVD4r_t      sc_final_sp1_resolved_4;
    SC_X4_FI_SP2_RSLVD4r_t      sc_final_sp2_resolved_4;
    SC_X4_FI_SP3_RSLVD4r_t      sc_final_sp3_resolved_4;

    SC_X4_FI_SP0_RSLVD5r_t      sc_final_sp0_resolved_5;
    SC_X4_FI_SP1_RSLVD5r_t      sc_final_sp1_resolved_5;
    SC_X4_FI_SP2_RSLVD5r_t      sc_final_sp2_resolved_5;
    SC_X4_FI_SP3_RSLVD5r_t      sc_final_sp3_resolved_5;

    SC_X4_FI_SP0_RSLVD6r_t      sc_final_sp0_resolved_6;
    SC_X4_FI_SP1_RSLVD6r_t      sc_final_sp1_resolved_6;
    SC_X4_FI_SP2_RSLVD6r_t      sc_final_sp2_resolved_6;
    SC_X4_FI_SP3_RSLVD6r_t      sc_final_sp3_resolved_6;

    SC_X4_FI_SP0_RSLVD_SPDr_CLR(sc_final_sp0_resolved_speed);
    SC_X4_FI_SP1_RSLVD_SPDr_CLR(sc_final_sp1_resolved_speed);
    SC_X4_FI_SP2_RSLVD_SPDr_CLR(sc_final_sp2_resolved_speed);
    SC_X4_FI_SP3_RSLVD_SPDr_CLR(sc_final_sp3_resolved_speed);
    SC_X4_FI_SP0_RSLVD0r_CLR(sc_final_sp0_resolved_0);
    SC_X4_FI_SP1_RSLVD0r_CLR(sc_final_sp1_resolved_0);
    SC_X4_FI_SP2_RSLVD0r_CLR(sc_final_sp2_resolved_0);
    SC_X4_FI_SP3_RSLVD0r_CLR(sc_final_sp3_resolved_0);
    SC_X4_FI_SP0_RSLVD1r_CLR(sc_final_sp0_resolved_1);
    SC_X4_FI_SP1_RSLVD1r_CLR(sc_final_sp1_resolved_1);
    SC_X4_FI_SP2_RSLVD1r_CLR(sc_final_sp2_resolved_1);
    SC_X4_FI_SP3_RSLVD1r_CLR(sc_final_sp3_resolved_1);
    SC_X4_FI_SP0_RSLVD2r_CLR(sc_final_sp0_resolved_2);
    SC_X4_FI_SP1_RSLVD2r_CLR(sc_final_sp1_resolved_2);
    SC_X4_FI_SP2_RSLVD2r_CLR(sc_final_sp2_resolved_2);
    SC_X4_FI_SP3_RSLVD2r_CLR(sc_final_sp3_resolved_2);
    SC_X4_FI_SP0_RSLVD3r_CLR(sc_final_sp0_resolved_3);
    SC_X4_FI_SP1_RSLVD3r_CLR(sc_final_sp1_resolved_3);
    SC_X4_FI_SP2_RSLVD3r_CLR(sc_final_sp2_resolved_3);
    SC_X4_FI_SP3_RSLVD3r_CLR(sc_final_sp3_resolved_3);
    SC_X4_FI_SP0_RSLVD4r_CLR(sc_final_sp0_resolved_4);
    SC_X4_FI_SP1_RSLVD4r_CLR(sc_final_sp1_resolved_4);
    SC_X4_FI_SP2_RSLVD4r_CLR(sc_final_sp2_resolved_4);
    SC_X4_FI_SP3_RSLVD4r_CLR(sc_final_sp3_resolved_4);
    SC_X4_FI_SP0_RSLVD5r_CLR(sc_final_sp0_resolved_5);
    SC_X4_FI_SP1_RSLVD5r_CLR(sc_final_sp1_resolved_5);
    SC_X4_FI_SP2_RSLVD5r_CLR(sc_final_sp2_resolved_5);
    SC_X4_FI_SP3_RSLVD5r_CLR(sc_final_sp3_resolved_5);
    SC_X4_FI_SP0_RSLVD6r_CLR(sc_final_sp0_resolved_6);
    SC_X4_FI_SP1_RSLVD6r_CLR(sc_final_sp1_resolved_6);
    SC_X4_FI_SP2_RSLVD6r_CLR(sc_final_sp2_resolved_6);
    SC_X4_FI_SP3_RSLVD6r_CLR(sc_final_sp3_resolved_6);

    /*1. Get the Speed valid and Speed Done Status */
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_STSr(pc,&sc_ctrl_status));

    /*2. Get the Speed set */
    /*2a PLLDIV and the refclk settings */
    PHYMOD_IF_ERR_RETURN(READ_MAIN_SETUPr(pc,&setup_reg));
    PHYMOD_IF_ERR_RETURN(READ_PLL_CTL8r(pc,&pll_div_reg));
    PHYMOD_IF_ERR_RETURN(READ_RXTXCOM_OSR_MODE_CTLr(pc, &reg_osr_mode));

    /*3. Get the final stats for the speed */
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP0_RSLVD0r(pc,&sc_final_sp0_resolved_0));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP1_RSLVD0r(pc,&sc_final_sp1_resolved_0));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP2_RSLVD0r(pc,&sc_final_sp2_resolved_0));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP3_RSLVD0r(pc,&sc_final_sp3_resolved_0));

    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP0_RSLVD1r(pc,&sc_final_sp0_resolved_1));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP1_RSLVD1r(pc,&sc_final_sp1_resolved_1));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP2_RSLVD1r(pc,&sc_final_sp2_resolved_1));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP3_RSLVD1r(pc,&sc_final_sp3_resolved_1));

    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP0_RSLVD2r(pc,&sc_final_sp0_resolved_2));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP1_RSLVD2r(pc,&sc_final_sp1_resolved_2));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP2_RSLVD2r(pc,&sc_final_sp2_resolved_2));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP3_RSLVD2r(pc,&sc_final_sp3_resolved_2));

    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP0_RSLVD3r(pc,&sc_final_sp0_resolved_3));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP1_RSLVD3r(pc,&sc_final_sp1_resolved_3));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP2_RSLVD3r(pc,&sc_final_sp2_resolved_3));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP3_RSLVD3r(pc,&sc_final_sp3_resolved_3));

    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP0_RSLVD4r(pc,&sc_final_sp0_resolved_4));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP1_RSLVD4r(pc,&sc_final_sp1_resolved_4));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP2_RSLVD4r(pc,&sc_final_sp2_resolved_4));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP3_RSLVD4r(pc,&sc_final_sp3_resolved_4));

    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP0_RSLVD5r(pc,&sc_final_sp0_resolved_5));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP1_RSLVD5r(pc,&sc_final_sp1_resolved_5));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP2_RSLVD5r(pc,&sc_final_sp2_resolved_5));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP3_RSLVD5r(pc,&sc_final_sp3_resolved_5));

    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP0_RSLVD6r(pc,&sc_final_sp0_resolved_6));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP1_RSLVD6r(pc,&sc_final_sp1_resolved_6));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP2_RSLVD6r(pc,&sc_final_sp2_resolved_6));
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_FI_SP3_RSLVD6r(pc,&sc_final_sp3_resolved_6));

    PHYMOD_DEBUG_ERROR(("| TRG ADR : %06d LANE: %02d    SPEED                                            |\n", pc->addr, pc->lane_mask));
    PHYMOD_DEBUG_ERROR(("+------------------+-------------------------+----------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("|   SPEED (LANE)   |     STATS (SUBPORT)     |     CREDIT STATS (SUBPORT)       |\n"));
    PHYMOD_DEBUG_ERROR(("+------------------+-------------------------+----------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("| SPD CHG VLD : %2d | QSGMII EN : %s %s %s %s     | SGMII SPD  : %d %d %d %d             |\n",
    SC_X4_STSr_SW_SPEED_CONFIG_VLDf_GET(sc_ctrl_status),
    SC_X4_FI_SP3_RSLVD0r_QSGMII_ENf_GET(sc_final_sp3_resolved_0)? "Y" : "N",
    SC_X4_FI_SP2_RSLVD0r_QSGMII_ENf_GET(sc_final_sp2_resolved_0)? "Y" : "N",
    SC_X4_FI_SP1_RSLVD0r_QSGMII_ENf_GET(sc_final_sp1_resolved_0)? "Y" : "N",
    SC_X4_FI_SP0_RSLVD0r_QSGMII_ENf_GET(sc_final_sp0_resolved_0)? "Y" : "N",
    SC_X4_FI_SP3_RSLVD1r_SGMII_SPD_SWITCHf_GET(sc_final_sp3_resolved_1),
    SC_X4_FI_SP2_RSLVD1r_SGMII_SPD_SWITCHf_GET(sc_final_sp2_resolved_1),
    SC_X4_FI_SP1_RSLVD1r_SGMII_SPD_SWITCHf_GET(sc_final_sp1_resolved_1),
    SC_X4_FI_SP0_RSLVD1r_SGMII_SPD_SWITCHf_GET(sc_final_sp0_resolved_1)));

    PHYMOD_DEBUG_ERROR(("| SPD CHG DONE: %2d | DEL MODE  : %02d %02d %02d %02d | CLK CNT0   : %04d %04d %04d %04d |\n",
    SC_X4_STSr_SW_SPEED_CHANGE_DONEf_GET(sc_ctrl_status),
    SC_X4_FI_SP3_RSLVD0r_CL36BYTEDELETEMODEf_GET(sc_final_sp3_resolved_0),
    SC_X4_FI_SP2_RSLVD0r_CL36BYTEDELETEMODEf_GET(sc_final_sp2_resolved_0),
    SC_X4_FI_SP1_RSLVD0r_CL36BYTEDELETEMODEf_GET(sc_final_sp1_resolved_0),
    SC_X4_FI_SP0_RSLVD0r_CL36BYTEDELETEMODEf_GET(sc_final_sp0_resolved_0),
    SC_X4_FI_SP3_RSLVD1r_CLOCKCNT0f_GET(sc_final_sp3_resolved_1),
    SC_X4_FI_SP2_RSLVD1r_CLOCKCNT0f_GET(sc_final_sp2_resolved_1),
    SC_X4_FI_SP1_RSLVD1r_CLOCKCNT0f_GET(sc_final_sp1_resolved_1),
    SC_X4_FI_SP0_RSLVD1r_CLOCKCNT0f_GET(sc_final_sp0_resolved_1)));

    PHYMOD_DEBUG_ERROR(("| REF CLOCK   : %2d | SPD RSLVD : %02d %02d %02d %02d | CLK CNT1   : %04d %04d %04d %04d |\n",
    MAIN_SETUPr_REFCLK_SELf_GET(setup_reg),
    SC_X4_FI_SP3_RSLVD_SPDr_SPEEDf_GET(sc_final_sp3_resolved_speed),
    SC_X4_FI_SP2_RSLVD_SPDr_SPEEDf_GET(sc_final_sp2_resolved_speed),
    SC_X4_FI_SP1_RSLVD_SPDr_SPEEDf_GET(sc_final_sp1_resolved_speed),
    SC_X4_FI_SP0_RSLVD_SPDr_SPEEDf_GET(sc_final_sp0_resolved_speed),
    SC_X4_FI_SP3_RSLVD2r_CLOCKCNT1f_GET(sc_final_sp3_resolved_2),
    SC_X4_FI_SP2_RSLVD2r_CLOCKCNT1f_GET(sc_final_sp2_resolved_2),
    SC_X4_FI_SP1_RSLVD2r_CLOCKCNT1f_GET(sc_final_sp1_resolved_2),
    SC_X4_FI_SP0_RSLVD2r_CLOCKCNT1f_GET(sc_final_sp0_resolved_2)));

    PHYMOD_DEBUG_ERROR(("| PLL DIV     : %2d |                         | LP CNT0    : %04d %04d %04d %04d |\n",
    PLL_CTL8r_I_NDIV_INTf_GET(pll_div_reg),
    SC_X4_FI_SP3_RSLVD3r_LOOPCNT0f_GET(sc_final_sp3_resolved_3),
    SC_X4_FI_SP2_RSLVD3r_LOOPCNT0f_GET(sc_final_sp2_resolved_3),
    SC_X4_FI_SP1_RSLVD3r_LOOPCNT0f_GET(sc_final_sp1_resolved_3),
    SC_X4_FI_SP0_RSLVD3r_LOOPCNT0f_GET(sc_final_sp0_resolved_3)));

    PHYMOD_DEBUG_ERROR(("| OS MODE     : %2s |                         | LP CNT1    : %04d %04d %04d %04d |\n",
    e2s_qmod16_os_mode_type[RXTXCOM_OSR_MODE_CTLr_OSR_MODE_FRC_VALf_GET(reg_osr_mode)]+19,
    SC_X4_FI_SP3_RSLVD3r_LOOPCNT1f_GET(sc_final_sp3_resolved_3),
    SC_X4_FI_SP2_RSLVD3r_LOOPCNT1f_GET(sc_final_sp2_resolved_3),
    SC_X4_FI_SP1_RSLVD3r_LOOPCNT1f_GET(sc_final_sp1_resolved_3),
    SC_X4_FI_SP0_RSLVD3r_LOOPCNT1f_GET(sc_final_sp0_resolved_3)));


    PHYMOD_DEBUG_ERROR(("|                                            | MAC  CGC   : %04d %04d %04d %04d |\n",
    SC_X4_FI_SP3_RSLVD4r_MAC_CREDITGENCNTf_GET(sc_final_sp3_resolved_4),
    SC_X4_FI_SP2_RSLVD4r_MAC_CREDITGENCNTf_GET(sc_final_sp2_resolved_4),
    SC_X4_FI_SP1_RSLVD4r_MAC_CREDITGENCNTf_GET(sc_final_sp1_resolved_4),
    SC_X4_FI_SP0_RSLVD4r_MAC_CREDITGENCNTf_GET(sc_final_sp0_resolved_4)));

    PHYMOD_DEBUG_ERROR(("|                                            | REP: %d%d%d%d  | PCS CRD EN : %d%d%d%d   |\n",
    SC_X4_FI_SP3_RSLVD5r_REPLICATION_CNTf_GET(sc_final_sp3_resolved_5),
    SC_X4_FI_SP2_RSLVD5r_REPLICATION_CNTf_GET(sc_final_sp2_resolved_5),
    SC_X4_FI_SP1_RSLVD5r_REPLICATION_CNTf_GET(sc_final_sp1_resolved_5),
    SC_X4_FI_SP0_RSLVD5r_REPLICATION_CNTf_GET(sc_final_sp0_resolved_5),
    SC_X4_FI_SP3_RSLVD5r_PCS_CREDITENABLEf_GET(sc_final_sp3_resolved_5),
    SC_X4_FI_SP2_RSLVD5r_PCS_CREDITENABLEf_GET(sc_final_sp2_resolved_5),
    SC_X4_FI_SP1_RSLVD5r_PCS_CREDITENABLEf_GET(sc_final_sp1_resolved_5),
    SC_X4_FI_SP0_RSLVD5r_PCS_CREDITENABLEf_GET(sc_final_sp0_resolved_5)));

    PHYMOD_DEBUG_ERROR(("|                                            | PCS CK CNT : %04d %04d %04d %04d |\n",
    SC_X4_FI_SP3_RSLVD5r_PCS_CLOCKCNT0f_GET(sc_final_sp3_resolved_5),
    SC_X4_FI_SP2_RSLVD5r_PCS_CLOCKCNT0f_GET(sc_final_sp2_resolved_5),
    SC_X4_FI_SP1_RSLVD5r_PCS_CLOCKCNT0f_GET(sc_final_sp1_resolved_5),
    SC_X4_FI_SP0_RSLVD5r_PCS_CLOCKCNT0f_GET(sc_final_sp0_resolved_5)));

    PHYMOD_DEBUG_ERROR(("|                                            | CRDGEN CNT : %04d %04d %04d %04d |\n",
    SC_X4_FI_SP3_RSLVD6r_PCS_CREDITGENCNTf_GET(sc_final_sp3_resolved_6),
    SC_X4_FI_SP2_RSLVD6r_PCS_CREDITGENCNTf_GET(sc_final_sp2_resolved_6),
    SC_X4_FI_SP1_RSLVD6r_PCS_CREDITGENCNTf_GET(sc_final_sp1_resolved_6),
    SC_X4_FI_SP0_RSLVD6r_PCS_CREDITGENCNTf_GET(sc_final_sp0_resolved_6)));

    PHYMOD_DEBUG_ERROR(("+------------------+-------------------------+----------------------------------+\n"));

    return PHYMOD_E_NONE;
}


int qmod16_diag_autoneg_abilities(PHYMOD_ST *pc)
{
    AN_X4_ABI_SP0_ENSr_t    sp0_an_ena;
    AN_X4_ABI_SP1_ENSr_t    sp1_an_ena;
    AN_X4_ABI_SP2_ENSr_t    sp2_an_ena;
    AN_X4_ABI_SP3_ENSr_t    sp3_an_ena;

    AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_t   sp0_cl37_base;
    AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_t   sp1_cl37_base;
    AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_t   sp2_cl37_base;
    AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_t   sp3_cl37_base;

    AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr_t    sp0_cl37_bam;
    AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_t    sp1_cl37_bam;
    AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr_t    sp2_cl37_bam;
    AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr_t    sp3_cl37_bam;

    AN_X4_ABI_SP0_LOC_DEV_OVER1G_ABIL0r_t     sp0_over1g;
    AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r_t     sp1_over1g;
    AN_X4_ABI_SP2_LOC_DEV_OVER1G_ABIL0r_t     sp2_over1g;
    AN_X4_ABI_SP3_LOC_DEV_OVER1G_ABIL0r_t     sp3_over1g;

    AN_X4_ABI_SP0_CTLSr_t                     sp0_an_ctrl;
    AN_X4_ABI_SP1_CTLSr_t                     sp1_an_ctrl;
    AN_X4_ABI_SP2_CTLSr_t                     sp2_an_ctrl;
    AN_X4_ABI_SP3_CTLSr_t                     sp3_an_ctrl;

    char sgmii_spd[4][8];

    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP0_ENSr(pc, &sp0_an_ena));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP1_ENSr(pc, &sp1_an_ena));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP2_ENSr(pc, &sp2_an_ena));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP3_ENSr(pc, &sp3_an_ena));

    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr(pc, &sp0_cl37_base));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr(pc, &sp1_cl37_base));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr(pc, &sp2_cl37_base));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr(pc, &sp3_cl37_base));

    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr(pc, &sp0_cl37_bam));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr(pc, &sp1_cl37_bam));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr(pc, &sp2_cl37_bam));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr(pc, &sp3_cl37_bam));

    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP0_LOC_DEV_OVER1G_ABIL0r(pc, &sp0_over1g));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r(pc, &sp1_over1g));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP2_LOC_DEV_OVER1G_ABIL0r(pc, &sp2_over1g));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP3_LOC_DEV_OVER1G_ABIL0r(pc, &sp3_over1g));

    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP0_CTLSr(pc, &sp0_an_ctrl));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP1_CTLSr(pc, &sp1_an_ctrl));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP2_CTLSr(pc, &sp2_an_ctrl));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_ABI_SP3_CTLSr(pc, &sp3_an_ctrl));

    PHYMOD_DEBUG_ERROR(("+----------------------------------------+----------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("| ANX4 CTRL: 0x%04X 0x%04X 0x%04X 0x%04X | ANX4 ENS : 0x%04X 0x%04X 0x%04X 0x%04X |\n",
    AN_X4_ABI_SP3_CTLSr_GET(sp3_an_ctrl),
    AN_X4_ABI_SP2_CTLSr_GET(sp2_an_ctrl),
    AN_X4_ABI_SP1_CTLSr_GET(sp1_an_ctrl),
    AN_X4_ABI_SP0_CTLSr_GET(sp0_an_ctrl),
    AN_X4_ABI_SP3_ENSr_GET(sp3_an_ena),
    AN_X4_ABI_SP2_ENSr_GET(sp2_an_ena),
    AN_X4_ABI_SP1_ENSr_GET(sp1_an_ena),
    AN_X4_ABI_SP0_ENSr_GET(sp0_an_ena)));

    PHYMOD_DEBUG_ERROR(("| CL37 BASE: 0x%04X 0x%04X 0x%04X 0x%04X | CL37 BAM : 0x%04X 0x%04X 0x%04X 0x%04X |\n",
    AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_GET(sp3_cl37_base),
    AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_GET(sp2_cl37_base),
    AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_GET(sp1_cl37_base),
    AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_GET(sp0_cl37_base),
    AN_X4_ABI_SP3_LOC_DEV_CL37_BAM_ABILr_GET(sp3_cl37_bam),
    AN_X4_ABI_SP2_LOC_DEV_CL37_BAM_ABILr_GET(sp2_cl37_bam),
    AN_X4_ABI_SP1_LOC_DEV_CL37_BAM_ABILr_GET(sp1_cl37_bam),
    AN_X4_ABI_SP0_LOC_DEV_CL37_BAM_ABILr_GET(sp0_cl37_bam)));

    PHYMOD_DEBUG_ERROR(("| ANX4 OVR0: 0x%04X 0x%04X 0x%04X 0x%04X |                                        |\n",
    AN_X4_ABI_SP3_LOC_DEV_OVER1G_ABIL0r_GET(sp3_over1g),
    AN_X4_ABI_SP2_LOC_DEV_OVER1G_ABIL0r_GET(sp2_over1g),
    AN_X4_ABI_SP1_LOC_DEV_OVER1G_ABIL0r_GET(sp1_over1g),
    AN_X4_ABI_SP0_LOC_DEV_OVER1G_ABIL0r_GET(sp0_over1g)));

    PHYMOD_DEBUG_ERROR(("+----------------------+-----------------------+-------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("| CL37 ENA      : %d%d%d%d | BAM  ENA       : %d%d%d%d | SGMII ENA  : %d%d%d%d       |\n",
    AN_X4_ABI_SP3_ENSr_CL37_ENABLEf_GET(sp3_an_ena),
    AN_X4_ABI_SP2_ENSr_CL37_ENABLEf_GET(sp2_an_ena),
    AN_X4_ABI_SP1_ENSr_CL37_ENABLEf_GET(sp1_an_ena),
    AN_X4_ABI_SP0_ENSr_CL37_ENABLEf_GET(sp0_an_ena),
    AN_X4_ABI_SP3_ENSr_CL37_BAM_ENABLEf_GET(sp3_an_ena),
    AN_X4_ABI_SP2_ENSr_CL37_BAM_ENABLEf_GET(sp2_an_ena),
    AN_X4_ABI_SP1_ENSr_CL37_BAM_ENABLEf_GET(sp1_an_ena),
    AN_X4_ABI_SP0_ENSr_CL37_BAM_ENABLEf_GET(sp0_an_ena),
    AN_X4_ABI_SP3_ENSr_CL37_SGMII_ENABLEf_GET(sp3_an_ena),
    AN_X4_ABI_SP2_ENSr_CL37_SGMII_ENABLEf_GET(sp2_an_ena),
    AN_X4_ABI_SP1_ENSr_CL37_SGMII_ENABLEf_GET(sp1_an_ena),
    AN_X4_ABI_SP0_ENSr_CL37_SGMII_ENABLEf_GET(sp0_an_ena)));

    PHYMOD_DEBUG_ERROR(("| AN RESTART    : %d%d%d%d | QSGMII ENA     : %d%d%d%d | USXGMII ENA: %d%d%d%d       |\n",
    AN_X4_ABI_SP3_ENSr_CL37_AN_RESTARTf_GET(sp3_an_ena),
    AN_X4_ABI_SP2_ENSr_CL37_AN_RESTARTf_GET(sp2_an_ena),
    AN_X4_ABI_SP1_ENSr_CL37_AN_RESTARTf_GET(sp1_an_ena),
    AN_X4_ABI_SP0_ENSr_CL37_AN_RESTARTf_GET(sp0_an_ena),
    AN_X4_ABI_SP3_ENSr_QSGMII_ENf_GET(sp3_an_ena),
    AN_X4_ABI_SP2_ENSr_QSGMII_ENf_GET(sp2_an_ena),
    AN_X4_ABI_SP1_ENSr_QSGMII_ENf_GET(sp1_an_ena),
    AN_X4_ABI_SP0_ENSr_QSGMII_ENf_GET(sp0_an_ena),
    AN_X4_ABI_SP3_ENSr_USXGMII_ENf_GET(sp3_an_ena),
    AN_X4_ABI_SP2_ENSr_USXGMII_ENf_GET(sp2_an_ena),
    AN_X4_ABI_SP1_ENSr_USXGMII_ENf_GET(sp1_an_ena),
    AN_X4_ABI_SP0_ENSr_USXGMII_ENf_GET(sp0_an_ena)));

    PHYMOD_DEBUG_ERROR(("| BAM2SGMII ENA : %d%d%d%d | SGMII2CL37 ENA : %d%d%d%d |                         |\n",
    AN_X4_ABI_SP3_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_GET(sp3_an_ena),
    AN_X4_ABI_SP2_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_GET(sp2_an_ena),
    AN_X4_ABI_SP1_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_GET(sp1_an_ena),
    AN_X4_ABI_SP0_ENSr_CL37_BAM_TO_SGMII_AUTO_ENABLEf_GET(sp0_an_ena),
    AN_X4_ABI_SP3_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_GET(sp3_an_ena),
    AN_X4_ABI_SP2_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_GET(sp2_an_ena),
    AN_X4_ABI_SP1_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_GET(sp1_an_ena),
    AN_X4_ABI_SP0_ENSr_SGMII_TO_CL37_AUTO_ENABLEf_GET(sp0_an_ena)));

    PHYMOD_DEBUG_ERROR(("| FAIL COUNT LIM: %-4d %-4d %-4d %-4d                                    |\n",
    AN_X4_ABI_SP3_CTLSr_AN_FAIL_COUNT_LIMITf_GET(sp3_an_ctrl),
    AN_X4_ABI_SP2_CTLSr_AN_FAIL_COUNT_LIMITf_GET(sp2_an_ctrl),
    AN_X4_ABI_SP1_CTLSr_AN_FAIL_COUNT_LIMITf_GET(sp1_an_ctrl),
    AN_X4_ABI_SP0_CTLSr_AN_FAIL_COUNT_LIMITf_GET(sp0_an_ctrl)));

    PHYMOD_IF_ERR_RETURN
      (qmod16_an_local_spd_get(AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_GET(sp0_cl37_base), sgmii_spd[0], sizeof(sgmii_spd[0])));
    PHYMOD_IF_ERR_RETURN
      (qmod16_an_local_spd_get(AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_GET(sp1_cl37_base), sgmii_spd[1], sizeof(sgmii_spd[1])));
    PHYMOD_IF_ERR_RETURN
      (qmod16_an_local_spd_get(AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_GET(sp2_cl37_base), sgmii_spd[2], sizeof(sgmii_spd[2])));
    PHYMOD_IF_ERR_RETURN
      (qmod16_an_local_spd_get(AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_SGMII_SPEEDf_GET(sp3_cl37_base), sgmii_spd[3], sizeof(sgmii_spd[3])));

    PHYMOD_DEBUG_ERROR(("+---------------- -+-------------------+---------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("|                          CL37 ABILITIES (SUBPORT)                            |\n"));
    PHYMOD_DEBUG_ERROR(("+------------------+-----------------------------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("| SWRST DIS : %d%d%d%d | ANRST DIS    : %d%d%d%d       |\n",
    AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_GET(sp3_cl37_base),
    AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_GET(sp2_cl37_base),
    AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_GET(sp1_cl37_base),
    AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_SW_RESTART_RESET_DISABLEf_GET(sp0_cl37_base),
    AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_GET(sp3_cl37_base),
    AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_GET(sp2_cl37_base),
    AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_GET(sp1_cl37_base),
    AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_AN_RESTART_RESET_DISABLEf_GET(sp0_cl37_base)));

    PHYMOD_DEBUG_ERROR(("| LINK STS  : %d%d%d%d | NEXT PAGE    : %d%d%d%d       |\n",
    AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_LINK_STATUSf_GET(sp3_cl37_base),
    AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_LINK_STATUSf_GET(sp2_cl37_base),
    AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_LINK_STATUSf_GET(sp1_cl37_base),
    AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_LINK_STATUSf_GET(sp0_cl37_base),
    AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_GET(sp3_cl37_base),
    AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_GET(sp2_cl37_base),
    AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_GET(sp1_cl37_base),
    AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_NEXT_PAGEf_GET(sp0_cl37_base)));

    PHYMOD_DEBUG_ERROR(("| HALF DUPLX: %d%d%d%d | FULL DUPLEX  : %d%d%d%d       |\n",
    AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_HALF_DUPLEXf_GET(sp3_cl37_base),
    AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_HALF_DUPLEXf_GET(sp2_cl37_base),
    AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_HALF_DUPLEXf_GET(sp1_cl37_base),
    AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_HALF_DUPLEXf_GET(sp0_cl37_base),
    AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_GET(sp3_cl37_base),
    AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_GET(sp2_cl37_base),
    AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_GET(sp1_cl37_base),
    AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_FULL_DUPLEXf_GET(sp0_cl37_base)));

    PHYMOD_DEBUG_ERROR(("| PAUSE     : %d%d%d%d | SGMII MASTER : %d%d%d%d       |\n",
    AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_GET(sp3_cl37_base),
    AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_GET(sp2_cl37_base),
    AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_GET(sp1_cl37_base),
    AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_CL37_PAUSEf_GET(sp0_cl37_base),
    AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_GET(sp3_cl37_base),
    AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_GET(sp2_cl37_base),
    AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_GET(sp1_cl37_base),
    AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_MASTER_MODEf_GET(sp0_cl37_base)));

    PHYMOD_DEBUG_ERROR(("| SGMII FDUP: %d%d%d%d | SGMII SPD    : %-8s %-8s %-8s %-8s |\n",
    AN_X4_ABI_SP3_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_GET(sp3_cl37_base),
    AN_X4_ABI_SP2_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_GET(sp2_cl37_base),
    AN_X4_ABI_SP1_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_GET(sp1_cl37_base),
    AN_X4_ABI_SP0_LOC_DEV_CL37_BASE_ABILr_SGMII_FULL_DUPLEXf_GET(sp0_cl37_base),
    sgmii_spd[3],sgmii_spd[2], sgmii_spd[1], sgmii_spd[0]));

    return PHYMOD_E_NONE;
}


/**
   @brief   Autoneg diagnostic function
   @param   pc handle to current QTCE16 context (#PHYMOD_ST)
   @returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
   @details Autoneg diags
   1.  Get the AN complete status for AN37/73
   2.  Get the resolved HCD
   3.  Get the link up status after AN
*/
int qmod16_diag_autoneg(PHYMOD_ST *pc)
{
    AN_X4_SW_SP0_AN_PAGE_SEQUENCER_STSr_t sp0_an_page_seq_status;
    AN_X4_SW_SP1_AN_PAGE_SEQUENCER_STSr_t sp1_an_page_seq_status;
    AN_X4_SW_SP2_AN_PAGE_SEQUENCER_STSr_t sp2_an_page_seq_status;
    AN_X4_SW_SP3_AN_PAGE_SEQUENCER_STSr_t sp3_an_page_seq_status;
    AN_X4_SW_SP0_AN_ABIL_RESOLUTION_STSr_t sp0_an_hcd_status;
    AN_X4_SW_SP1_AN_ABIL_RESOLUTION_STSr_t sp1_an_hcd_status;
    AN_X4_SW_SP2_AN_ABIL_RESOLUTION_STSr_t sp2_an_hcd_status;
    AN_X4_SW_SP3_AN_ABIL_RESOLUTION_STSr_t sp3_an_hcd_status;
    AN_X4_SW_SP0_AN_MISC_STSr_t sp0_an_misc_status;
    AN_X4_SW_SP1_AN_MISC_STSr_t sp1_an_misc_status;
    AN_X4_SW_SP2_AN_MISC_STSr_t sp2_an_misc_status;
    AN_X4_SW_SP3_AN_MISC_STSr_t sp3_an_misc_status;
    char  an_spd[4][8];

    PHYMOD_IF_ERR_RETURN(READ_AN_X4_SW_SP0_AN_PAGE_SEQUENCER_STSr(pc, &sp0_an_page_seq_status));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_SW_SP1_AN_PAGE_SEQUENCER_STSr(pc, &sp1_an_page_seq_status));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_SW_SP2_AN_PAGE_SEQUENCER_STSr(pc, &sp2_an_page_seq_status));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_SW_SP3_AN_PAGE_SEQUENCER_STSr(pc, &sp3_an_page_seq_status));

    PHYMOD_IF_ERR_RETURN(READ_AN_X4_SW_SP0_AN_ABIL_RESOLUTION_STSr(pc, &sp0_an_hcd_status));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_SW_SP1_AN_ABIL_RESOLUTION_STSr(pc, &sp1_an_hcd_status));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_SW_SP2_AN_ABIL_RESOLUTION_STSr(pc, &sp2_an_hcd_status));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_SW_SP3_AN_ABIL_RESOLUTION_STSr(pc, &sp3_an_hcd_status));

    PHYMOD_IF_ERR_RETURN(READ_AN_X4_SW_SP0_AN_MISC_STSr(pc, &sp0_an_misc_status));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_SW_SP1_AN_MISC_STSr(pc, &sp1_an_misc_status));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_SW_SP2_AN_MISC_STSr(pc, &sp2_an_misc_status));
    PHYMOD_IF_ERR_RETURN(READ_AN_X4_SW_SP3_AN_MISC_STSr(pc, &sp3_an_misc_status));

    PHYMOD_IF_ERR_RETURN
        (qmod16_an_hcd_spd_get (AN_X4_SW_SP0_AN_ABIL_RESOLUTION_STSr_AN_HCD_SPEEDf_GET(sp0_an_hcd_status), an_spd[0], sizeof(an_spd[0])));
    PHYMOD_IF_ERR_RETURN
        (qmod16_an_hcd_spd_get (AN_X4_SW_SP1_AN_ABIL_RESOLUTION_STSr_AN_HCD_SPEEDf_GET(sp1_an_hcd_status), an_spd[1], sizeof(an_spd[1])));
    PHYMOD_IF_ERR_RETURN
        (qmod16_an_hcd_spd_get (AN_X4_SW_SP2_AN_ABIL_RESOLUTION_STSr_AN_HCD_SPEEDf_GET(sp2_an_hcd_status), an_spd[2], sizeof(an_spd[2])));
    PHYMOD_IF_ERR_RETURN
        (qmod16_an_hcd_spd_get (AN_X4_SW_SP3_AN_ABIL_RESOLUTION_STSr_AN_HCD_SPEEDf_GET(sp3_an_hcd_status), an_spd[3], sizeof(an_spd[3])));

    PHYMOD_DEBUG_ERROR(("| TRG ADR : %06d LANE: %02d     AUTONEG (SUBPORT)                              |\n",
              pc->addr, pc->lane_mask));
    PHYMOD_DEBUG_ERROR(("+----------------+-------------+------------------------------+-------------------+\n"));
    PHYMOD_DEBUG_ERROR(("| AN37 CMPL: %c%c%c%c | AN HCD SPD : %-8s %-8s %-8s %-8s | AN LINK : %s %s %s %s |\n",
    (AN_X4_SW_SP3_AN_PAGE_SEQUENCER_STSr_AN_ST_CL37_COMPLETEf_GET(sp3_an_page_seq_status) == 1) ? 'Y' : 'N',
    (AN_X4_SW_SP2_AN_PAGE_SEQUENCER_STSr_AN_ST_CL37_COMPLETEf_GET(sp2_an_page_seq_status) == 1) ? 'Y' : 'N',
    (AN_X4_SW_SP1_AN_PAGE_SEQUENCER_STSr_AN_ST_CL37_COMPLETEf_GET(sp1_an_page_seq_status) == 1) ? 'Y' : 'N',
    (AN_X4_SW_SP0_AN_PAGE_SEQUENCER_STSr_AN_ST_CL37_COMPLETEf_GET(sp0_an_page_seq_status) == 1) ? 'Y' : 'N',
     an_spd[3], an_spd[2], an_spd[1], an_spd[0],
    ((AN_X4_SW_SP3_AN_PAGE_SEQUENCER_STSr_AN_ST_CL37_COMPLETEf_GET(sp3_an_page_seq_status) == 1) &&
     (AN_X4_SW_SP3_AN_MISC_STSr_AN_COMPLETEf_GET(sp3_an_misc_status) == 1))? "UP":"DN",
    ((AN_X4_SW_SP2_AN_PAGE_SEQUENCER_STSr_AN_ST_CL37_COMPLETEf_GET(sp2_an_page_seq_status) == 1) &&
     (AN_X4_SW_SP2_AN_MISC_STSr_AN_COMPLETEf_GET(sp2_an_misc_status) == 1))? "UP":"DN",
    ((AN_X4_SW_SP1_AN_PAGE_SEQUENCER_STSr_AN_ST_CL37_COMPLETEf_GET(sp1_an_page_seq_status) == 1) &&
     (AN_X4_SW_SP1_AN_MISC_STSr_AN_COMPLETEf_GET(sp1_an_misc_status) == 1))? "UP":"DN",
    ((AN_X4_SW_SP0_AN_PAGE_SEQUENCER_STSr_AN_ST_CL37_COMPLETEf_GET(sp0_an_page_seq_status) == 1) &&
     (AN_X4_SW_SP0_AN_MISC_STSr_AN_COMPLETEf_GET(sp0_an_misc_status) == 1))? "UP":"DN"));

    PHYMOD_IF_ERR_RETURN
        (qmod16_diag_autoneg_abilities(pc));
    PHYMOD_DEBUG_ERROR(("+-------------------+--------------------+-----------------+-------------------+\n"));

    return PHYMOD_E_NONE;
}

/**
   @brief   Topology diagnostic function.
   @param   pc handle to current TSCE context (#PHYMOD_ST)
   @returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
   @details Get the topology details
   1. LoopBack
   a. Remote Loopback - RX loopback
   i.  PCS loopback
   ii. PMD loopback  
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
int qmod16_diag_topology(PHYMOD_ST *pc)
{
    MAIN_LPBK_CTLr_t pcs_loopback_ctrl;
    TLB_RX_DIG_LPBK_CFGr_t pmd_loopback_dig;
    TLB_TX_RMT_LPBK_CFGr_t pmd_loopback_remote;
    MAIN_LN_SWPr_t pcs_lane_swap;
    LN_ADDR0r_t pmd_lane_addr_0;
    LN_ADDR1r_t pmd_lane_addr_1;
    LN_ADDR2r_t pmd_lane_addr_2;
    LN_ADDR3r_t pmd_lane_addr_3;
    TLB_TX_TLB_TX_MISC_CFGr_t tx_pol_inv;
    TLB_RX_TLB_RX_MISC_CFGr_t rx_pol_inv;
    MAIN_SETUPr_t reg_main0_setup;
    uint16_t pcs_lcl_lb, pcs_rmt_lb, pmd_dig_lb, pmd_rmt_lb;
 
    /* Main0 Setup Register */
    /*1. Loopback  */
    PHYMOD_IF_ERR_RETURN(READ_MAIN_LPBK_CTLr(pc, &pcs_loopback_ctrl));
    PHYMOD_IF_ERR_RETURN(READ_TLB_RX_DIG_LPBK_CFGr(pc, &pmd_loopback_dig));
    PHYMOD_IF_ERR_RETURN(READ_TLB_TX_RMT_LPBK_CFGr(pc, &pmd_loopback_remote));

    /*2. LaneSwap  */
    PHYMOD_IF_ERR_RETURN(READ_MAIN_LN_SWPr(pc, &pcs_lane_swap));
    PHYMOD_IF_ERR_RETURN(READ_LN_ADDR0r(pc, &pmd_lane_addr_0));
    PHYMOD_IF_ERR_RETURN(READ_LN_ADDR1r(pc, &pmd_lane_addr_1));
    PHYMOD_IF_ERR_RETURN(READ_LN_ADDR2r(pc, &pmd_lane_addr_2));
    PHYMOD_IF_ERR_RETURN(READ_LN_ADDR3r(pc, &pmd_lane_addr_3));

    /*3.Polarity Inversion */
    PHYMOD_IF_ERR_RETURN(READ_TLB_TX_TLB_TX_MISC_CFGr(pc, &tx_pol_inv));
    PHYMOD_IF_ERR_RETURN(READ_TLB_RX_TLB_RX_MISC_CFGr(pc, &rx_pol_inv));

    /* 4. Master PortNum */
    PHYMOD_IF_ERR_RETURN(READ_MAIN_SETUPr(pc, &reg_main0_setup));

    /* Print the information here */
    pcs_lcl_lb = MAIN_LPBK_CTLr_LOCAL_PCS_LOOPBACK_ENABLEf_GET(pcs_loopback_ctrl);
    pcs_rmt_lb = MAIN_LPBK_CTLr_REMOTE_PCS_LOOPBACK_ENABLEf_GET(pcs_loopback_ctrl);
    pmd_dig_lb = TLB_RX_DIG_LPBK_CFGr_DIG_LPBK_ENf_GET(pmd_loopback_dig);
    pmd_rmt_lb = TLB_TX_RMT_LPBK_CFGr_RMT_LPBK_ENf_GET(pmd_loopback_remote);

    PHYMOD_DEBUG_ERROR(("\n"));
    PHYMOD_DEBUG_ERROR(("+---------------------------------------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("| TRG ADR : %06d LANE: %02d     TOPOLOGY (LANE)                       |\n", pc->addr, pc->lane_mask));
    PHYMOD_DEBUG_ERROR(("+-------------------+-----------------------------+-------------------+\n"));
    PHYMOD_DEBUG_ERROR(("| PCSLCL LPBK: %c%c%c%c | PCS LANE SWAP TX L2P : %d%d%d%d | TX POLARITY : %d   |\n",
      (pcs_lcl_lb & 8) ? 'Y':'N',
      (pcs_lcl_lb & 4) ? 'Y':'N',
      (pcs_lcl_lb & 2) ? 'Y':'N',
      (pcs_lcl_lb & 1) ? 'Y':'N',
      MAIN_LN_SWPr_LOG3_TO_PHY_TX_LNSWAP_SELf_GET(pcs_lane_swap),
      MAIN_LN_SWPr_LOG2_TO_PHY_TX_LNSWAP_SELf_GET(pcs_lane_swap),
      MAIN_LN_SWPr_LOG1_TO_PHY_TX_LNSWAP_SELf_GET(pcs_lane_swap),
      MAIN_LN_SWPr_LOG0_TO_PHY_TX_LNSWAP_SELf_GET(pcs_lane_swap),
      TLB_TX_TLB_TX_MISC_CFGr_TX_PMD_DP_INVERTf_GET(tx_pol_inv)));

    PHYMOD_DEBUG_ERROR(("| PCSRMT LPBK: %c%c%c%c | PCS LANE SWAP RX L2P : %d%d%d%d | RX POLARITY : %d   |\n",
      (pcs_rmt_lb & 8) ? 'Y':'N',
      (pcs_rmt_lb & 4) ? 'Y':'N',
      (pcs_rmt_lb & 2) ? 'Y':'N',
      (pcs_rmt_lb & 1) ? 'Y':'N',
      MAIN_LN_SWPr_LOG3_TO_PHY_RX_LNSWAP_SELf_GET(pcs_lane_swap),
      MAIN_LN_SWPr_LOG2_TO_PHY_RX_LNSWAP_SELf_GET(pcs_lane_swap),
      MAIN_LN_SWPr_LOG1_TO_PHY_RX_LNSWAP_SELf_GET(pcs_lane_swap),
      MAIN_LN_SWPr_LOG0_TO_PHY_RX_LNSWAP_SELf_GET(pcs_lane_swap),
      TLB_RX_TLB_RX_MISC_CFGr_RX_PMD_DP_INVERTf_GET(rx_pol_inv)));

    PHYMOD_DEBUG_ERROR(("| PMDDIG LPBK: %c%c%c%c | PMD LANE TX ADDR     : %d%d%d%d | PORT NUM    : %d   |\n",
      (pmd_dig_lb & 8) ? 'Y':'N',
      (pmd_dig_lb & 4) ? 'Y':'N',
      (pmd_dig_lb & 2) ? 'Y':'N',
      (pmd_dig_lb & 1) ? 'Y':'N',
      LN_ADDR3r_TX_LANE_ADDR_3f_GET(pmd_lane_addr_3),
      LN_ADDR2r_TX_LANE_ADDR_2f_GET(pmd_lane_addr_2),
      LN_ADDR1r_TX_LANE_ADDR_1f_GET(pmd_lane_addr_1),
      LN_ADDR0r_TX_LANE_ADDR_0f_GET(pmd_lane_addr_0),
      MAIN_SETUPr_MASTER_PORT_NUMf_GET(reg_main0_setup)));

    PHYMOD_DEBUG_ERROR(("| PMDREM LPBK: %c%c%c%c | PMD LANE RX ADDR     : %d%d%d%d | SNGLMODE    : %d   |\n",
      (pmd_rmt_lb & 8) ? 'Y':'N',
      (pmd_rmt_lb & 4) ? 'Y':'N',
      (pmd_rmt_lb & 2) ? 'Y':'N',
      (pmd_rmt_lb & 1) ? 'Y':'N',
      LN_ADDR3r_RX_LANE_ADDR_3f_GET(pmd_lane_addr_3),
      LN_ADDR2r_RX_LANE_ADDR_2f_GET(pmd_lane_addr_2),
      LN_ADDR1r_RX_LANE_ADDR_1f_GET(pmd_lane_addr_1),
      LN_ADDR0r_RX_LANE_ADDR_0f_GET(pmd_lane_addr_0),
      MAIN_SETUPr_SINGLE_PORT_MODEf_GET(reg_main0_setup)));

    PHYMOD_DEBUG_ERROR(("+-------------------+----------------------+-----------------------------+\n"));

    return PHYMOD_E_NONE;
}

/**
   @brief   Get the link status
   @param   pc handle to current TSCE context (#PHYMOD_ST)
   @returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
   @details Call all the Diags
*/
int qmod16_diag_general(PHYMOD_ST *ws)
{
    qmod16_diag_topology(ws);
    qmod16_diag_link(ws);
    qmod16_diag_autoneg(ws);
    qmod16_diag_speed(ws);

    return (PHYMOD_E_NONE);
}

/**
   @brief   Get the link status
   @param   pc handle to current TSCE context (#PHYMOD_ST)
   @returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
   @details Print the link status.
   1. pmd_live_status
   a. pll_lock_sts : PMD PLL lock indication
   2a. pmd_lane_live_status
   a. signal_detect_sts : The signal detected on the lane
   b. rx_lock_sts : The recovery clock locked for the rx lane
   2b. pmd_lane_latched_status
   3a. pcs_live_status
   a. sync status : per port: Live Sync Status indicator for cl36, cl48, brcm mode. Comma detect/code sync
   per lane: Live Block Lock Status indicator for cl49, cl82.Sync bits locked, on the 2 sync bits of the 66 bits(of 64/66 encoding)
   b. link_status : Link status for that port
   3b. pcs_latched_status
*/
int qmod16_diag_link(PHYMOD_ST *pc)
{
    PMD_X1_STSr_t pmd_live_status;
    PMD_X4_STSr_t pmd_lane_live_status;
    PMD_X4_LATCH_STSr_t pmd_lane_latched_status[4];
    PCS_LIVE_STSr_t pcs_live_status;
    PCS_LATCH_STSr_t pcs_latched_status;
    RX_X4_CL49_STS0_PCS_LIVE_STSr_t cl49_live_status;
    RX_X4_CL49_STS0_PCS_LATCH_STS0r_t cl49_latch_status;
    phymod_access_t *lpc = (phymod_access_t*)pc;

    int i,rx_lock_sts[4],sig_det_sts[4], this_lane;

    /*1. pmd_live_status  */
    PHYMOD_IF_ERR_RETURN(READ_PMD_X1_STSr(pc, &pmd_live_status));

    /*2a. pmd_lane_live_status  */
    /*2b. pmd_lane_latched_status  */

    this_lane = lpc->lane_mask;
    for(i=0; i < 4; i++) {
        lpc->lane_mask = 1<<i;
        PHYMOD_IF_ERR_RETURN(READ_PMD_X4_STSr(lpc, &pmd_lane_live_status));
        sig_det_sts[i] = PMD_X4_STSr_SIGNAL_DETECT_STSf_GET(pmd_lane_live_status);
        rx_lock_sts[i] = PMD_X4_STSr_RX_LOCK_STSf_GET(pmd_lane_live_status);

        PMD_X4_LATCH_STSr_CLR(pmd_lane_latched_status[i]);
        PHYMOD_IF_ERR_RETURN(READ_PMD_X4_LATCH_STSr(pc, &pmd_lane_latched_status[i]));
    }
    lpc->lane_mask = this_lane;

    /*3a. pcs_live_status  */
    /*3b. pcs_latched_status  */
    PHYMOD_IF_ERR_RETURN(READ_PCS_LIVE_STSr(pc, &pcs_live_status));
    PHYMOD_IF_ERR_RETURN(READ_PCS_LATCH_STSr(pc, &pcs_latched_status));
    PHYMOD_IF_ERR_RETURN(READ_RX_X4_CL49_STS0_PCS_LIVE_STSr(pc, &cl49_live_status));
    PHYMOD_IF_ERR_RETURN(READ_RX_X4_CL49_STS0_PCS_LATCH_STS0r(pc, &cl49_latch_status));

    /* Print the information here */
    PHYMOD_DEBUG_ERROR(("| TRG ADR : %06d LANE: %02d           LINK STATE          |   LH    |   LL   |\n", pc->addr, pc->lane_mask));
    PHYMOD_DEBUG_ERROR(("+---------LANE----------+----------------SUBPORT----------+---------+--------+\n"));
    PHYMOD_DEBUG_ERROR(("| PMD PLL LOCK   : %c    | PCS SYNC : %c%c%c%c | PCS SYNC STAT :  %d%d%d%d   :   %d%d%d%d |\n",
    (PMD_X1_STSr_PLL_LOCK_STSf_GET(pmd_live_status) == 1) ? 'Y' : 'N',
    (PCS_LIVE_STSr_SYNC_STATUS_SP3f_GET(pcs_live_status) == 1) ? 'Y' : 'N',
    (PCS_LIVE_STSr_SYNC_STATUS_SP2f_GET(pcs_live_status) == 1) ? 'Y' : 'N',
    (PCS_LIVE_STSr_SYNC_STATUS_SP1f_GET(pcs_live_status) == 1) ? 'Y' : 'N',
    (PCS_LIVE_STSr_SYNC_STATUS_SP0f_GET(pcs_live_status) == 1) ? 'Y' : 'N',
    PCS_LATCH_STSr_SYNC_STATUS_LH_SP3f_GET(pcs_latched_status),
    PCS_LATCH_STSr_SYNC_STATUS_LH_SP2f_GET(pcs_latched_status),
    PCS_LATCH_STSr_SYNC_STATUS_LH_SP1f_GET(pcs_latched_status),
    PCS_LATCH_STSr_SYNC_STATUS_LH_SP0f_GET(pcs_latched_status),
    PCS_LATCH_STSr_SYNC_STATUS_LL_SP3f_GET(pcs_latched_status),
    PCS_LATCH_STSr_SYNC_STATUS_LL_SP2f_GET(pcs_latched_status),
    PCS_LATCH_STSr_SYNC_STATUS_LL_SP1f_GET(pcs_latched_status),
    PCS_LATCH_STSr_SYNC_STATUS_LL_SP0f_GET(pcs_latched_status)));

    PHYMOD_DEBUG_ERROR(("| PMD SIG DETECT : %c%c%c%c | PCS LINK : %c%c%c%c | PCS LINK STAT :  %d%d%d%d   :   %d%d%d%d |\n",
    (sig_det_sts[3] == 1)? 'Y':'N',
    (sig_det_sts[2] == 1)? 'Y':'N',
    (sig_det_sts[1] == 1)? 'Y':'N',
    (sig_det_sts[0] == 1)? 'Y':'N',
    (PCS_LIVE_STSr_LINK_STATUS_SP3f_GET(pcs_live_status) == 1) ? 'Y' : 'N',
    (PCS_LIVE_STSr_LINK_STATUS_SP2f_GET(pcs_live_status) == 1) ? 'Y' : 'N',
    (PCS_LIVE_STSr_LINK_STATUS_SP1f_GET(pcs_live_status) == 1) ? 'Y' : 'N',
    (PCS_LIVE_STSr_LINK_STATUS_SP0f_GET(pcs_live_status) == 1) ? 'Y' : 'N',
    PCS_LATCH_STSr_LINK_STATUS_LH_SP3f_GET(pcs_latched_status),
    PCS_LATCH_STSr_LINK_STATUS_LH_SP2f_GET(pcs_latched_status),
    PCS_LATCH_STSr_LINK_STATUS_LH_SP1f_GET(pcs_latched_status),
    PCS_LATCH_STSr_LINK_STATUS_LH_SP0f_GET(pcs_latched_status),
    PCS_LATCH_STSr_LINK_STATUS_LL_SP3f_GET(pcs_latched_status),
    PCS_LATCH_STSr_LINK_STATUS_LL_SP2f_GET(pcs_latched_status),
    PCS_LATCH_STSr_LINK_STATUS_LL_SP1f_GET(pcs_latched_status),
    PCS_LATCH_STSr_LINK_STATUS_LL_SP0f_GET(pcs_latched_status)));

    /* AM lock status and AM lock latch status is pre lane. Repeat 4 time for 4 subprots  */
    PHYMOD_DEBUG_ERROR(("| PMD LOCK       : %c%c%c%c | AM LOCK  : %c%c%c%c | AM LOCK STAT  :  %d%d%d%d   :   %d%d%d%d |\n",
    (rx_lock_sts[3] == 1)? 'Y':'N',
    (rx_lock_sts[2] == 1)? 'Y':'N',
    (rx_lock_sts[1] == 1)? 'Y':'N',
    (rx_lock_sts[0] == 1)? 'Y':'N',
    RX_X4_CL49_STS0_PCS_LIVE_STSr_AM_LOCKf_GET(cl49_live_status) ? 'Y' : 'N',
    RX_X4_CL49_STS0_PCS_LIVE_STSr_AM_LOCKf_GET(cl49_live_status) ? 'Y' : 'N',
    RX_X4_CL49_STS0_PCS_LIVE_STSr_AM_LOCKf_GET(cl49_live_status) ? 'Y' : 'N',
    RX_X4_CL49_STS0_PCS_LIVE_STSr_AM_LOCKf_GET(cl49_live_status) ? 'Y' : 'N',
    RX_X4_CL49_STS0_PCS_LATCH_STS0r_AM_LOCK_LHf_GET(cl49_latch_status),
    RX_X4_CL49_STS0_PCS_LATCH_STS0r_AM_LOCK_LHf_GET(cl49_latch_status),
    RX_X4_CL49_STS0_PCS_LATCH_STS0r_AM_LOCK_LHf_GET(cl49_latch_status),
    RX_X4_CL49_STS0_PCS_LATCH_STS0r_AM_LOCK_LHf_GET(cl49_latch_status),
    RX_X4_CL49_STS0_PCS_LATCH_STS0r_AM_LOCK_LLf_GET(cl49_latch_status),
    RX_X4_CL49_STS0_PCS_LATCH_STS0r_AM_LOCK_LLf_GET(cl49_latch_status),
    RX_X4_CL49_STS0_PCS_LATCH_STS0r_AM_LOCK_LLf_GET(cl49_latch_status),
    RX_X4_CL49_STS0_PCS_LATCH_STS0r_AM_LOCK_LLf_GET(cl49_latch_status)));

    PHYMOD_DEBUG_ERROR(("| PMD LATCH HI   : %d%d%d%d |                                                    |\n",
    PMD_X4_LATCH_STSr_SIGNAL_DETECT_LHf_GET(pmd_lane_latched_status[3]),
    PMD_X4_LATCH_STSr_SIGNAL_DETECT_LHf_GET(pmd_lane_latched_status[2]),
    PMD_X4_LATCH_STSr_SIGNAL_DETECT_LHf_GET(pmd_lane_latched_status[1]),
    PMD_X4_LATCH_STSr_SIGNAL_DETECT_LHf_GET(pmd_lane_latched_status[0])));

    PHYMOD_DEBUG_ERROR(("| PMD LATCH LO   : %d%d%d%d |                                                    |\n",
    PMD_X4_LATCH_STSr_SIGNAL_DETECT_LLf_GET(pmd_lane_latched_status[3]),
    PMD_X4_LATCH_STSr_SIGNAL_DETECT_LLf_GET(pmd_lane_latched_status[2]),
    PMD_X4_LATCH_STSr_SIGNAL_DETECT_LLf_GET(pmd_lane_latched_status[1]),
    PMD_X4_LATCH_STSr_SIGNAL_DETECT_LLf_GET(pmd_lane_latched_status[0])));

    PHYMOD_DEBUG_ERROR(("| RXLOCK LATCH HI: %d%d%d%d |                                                    |\n",
    PMD_X4_LATCH_STSr_RX_LOCK_LHf_GET(pmd_lane_latched_status[3]),
    PMD_X4_LATCH_STSr_RX_LOCK_LHf_GET(pmd_lane_latched_status[2]),
    PMD_X4_LATCH_STSr_RX_LOCK_LHf_GET(pmd_lane_latched_status[1]),
    PMD_X4_LATCH_STSr_RX_LOCK_LHf_GET(pmd_lane_latched_status[0])));

    PHYMOD_DEBUG_ERROR(("| RXLOCK LATCH LO: %d%d%d%d |                                                    |\n",
    PMD_X4_LATCH_STSr_RX_LOCK_LLf_GET(pmd_lane_latched_status[3]),
    PMD_X4_LATCH_STSr_RX_LOCK_LLf_GET(pmd_lane_latched_status[2]),
    PMD_X4_LATCH_STSr_RX_LOCK_LLf_GET(pmd_lane_latched_status[1]),
    PMD_X4_LATCH_STSr_RX_LOCK_LLf_GET(pmd_lane_latched_status[0])));

    PHYMOD_DEBUG_ERROR(("+-----------------------+------------------+---------------------------------+\n"));

    return PHYMOD_E_NONE;
}


/**
   @brief   Internal Tfc info (PRTP. vs. PktGen vs. MAC dataetc.).
   @param   pc handle to current TSCE context (#PHYMOD_ST)
   @returns PHYMOD_E_NONE if no errors. PHYMOD_E_ERROR else.
   @details Print the Internal Tfc types.
*/
int qmod16_diag_internal_tfc(PHYMOD_ST *ws)
{
    return PHYMOD_E_NONE;
}

/**
   @brief   Soft Table diagnostic function
   @param   pc handle to current TSCE context (#PHYMOD_ST)
   @returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
   @details ST diags
   Print the soft table enteries
   if the resolved_speed is not set, then all the table enteries are displayed
*/
int qmod16_diag_st(PHYMOD_ST *ws, int resolved_speed)
{
    return PHYMOD_E_NONE;
} /*qmod16_diag_st */

/**
   @brief   AN Timers diagnostic function
   @param   pc handle to current TSCE context (#PHYMOD_ST)
   @returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
   @details Prints the AN timers
*/
int qmod16_diag_an_timers(PHYMOD_ST* ws)
{
    return PHYMOD_E_NONE;
}

/**
   @brief   Prints out the debug state register
   @param   pc handle to current TSCE context (#PHYMOD_ST)
   @returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
   @details Prints the debug state regs.
*/
int qmod16_diag_state(PHYMOD_ST* ws)
{
    return PHYMOD_E_NONE;
}

/**
   @brief  TBD
   @param   pc handle to current TSCE context (#PHYMOD_ST)
   @returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
   @details  Misc debug
*/
int qmod16_diag_debug(PHYMOD_ST* ws)
{
    return PHYMOD_E_NONE;
}

int qmod16_diag_g_rd_ram(PHYMOD_ST *ws, int mode, int lane)
{
    return PHYMOD_E_NONE ;
}

int qmod16_diag_g_rd_uC(PHYMOD_ST *ws, int lane)
{
    return PHYMOD_E_NONE ;
}

int qmod16_diag_g_info(PHYMOD_ST *ws)
{
    return PHYMOD_E_NONE ;
}

int qmod16_diag_ieee(PHYMOD_ST *ws)
{
    return PHYMOD_E_NONE;
}

/*!

*/
int qmod16_diag(PHYMOD_ST *pc, qmod16_diag_type diag_type)
{
    int rv;
    rv = PHYMOD_E_NONE;

    PHYMOD_DEBUG_ERROR(("+------------------------------------------------------------------------------+\n"));
    if (diag_type & QMOD16_DIAG_GENERAL)    { rv |= qmod16_diag_general(pc); }
    if (diag_type & QMOD16_DIAG_TOPOLOGY)   { rv |= qmod16_diag_topology(pc); }
    if (diag_type & QMOD16_DIAG_LINK)       { rv |= qmod16_diag_link(pc); }
    if (diag_type & QMOD16_DIAG_SPEED)      { rv |= qmod16_diag_speed(pc); }
    if (diag_type & QMOD16_DIAG_ANEG)       { rv |= qmod16_diag_autoneg(pc); }
    if (diag_type & QMOD16_DIAG_TFC)        { rv |= qmod16_diag_internal_tfc(pc); }
    if (diag_type & QMOD16_DIAG_AN_TIMERS)  { rv |= qmod16_diag_an_timers(pc); }
    if (diag_type & QMOD16_DIAG_STATE)      { rv |= qmod16_diag_state(pc); }
    if (diag_type & QMOD16_DIAG_DEBUG)      { rv |= qmod16_diag_debug(pc); }
    return rv;
}

/*!
 */
int qmod16_diag_disp(PHYMOD_ST *pc, const char* cmd_str)
{
    qmod16_diag_type type;

    if (!cmd_str) {
        type = QMOD16_DIAG_GENERAL;
    } else if (!PHYMOD_STRCMP(cmd_str, "topo")) {
        type = QMOD16_DIAG_TOPOLOGY;
    } else if (!PHYMOD_STRCMP(cmd_str, "link")) {
        type = QMOD16_DIAG_LINK;
    } else if (!PHYMOD_STRCMP(cmd_str, "speed")) {
        type = QMOD16_DIAG_SPEED;
    } else if (!PHYMOD_STRCMP(cmd_str, "aneg")) {
        type = QMOD16_DIAG_ANEG;
    } else if (!PHYMOD_STRCMP(cmd_str, "tfc")) {
        type = QMOD16_DIAG_TFC;
    } else if (!PHYMOD_STRCMP(cmd_str, "antimers")) {
        type = QMOD16_DIAG_AN_TIMERS;
    } else if (!PHYMOD_STRCMP(cmd_str, "state")) {
        type = QMOD16_DIAG_STATE;
    } else if (!PHYMOD_STRCMP(cmd_str, "debug")) {
        type = QMOD16_DIAG_DEBUG;
    } else {
        type = QMOD16_DIAG_GENERAL;
    }
    return (qmod16_diag(pc, type));
}



